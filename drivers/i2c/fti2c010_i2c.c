// SPDX-License-Identifier: GPL-2.0
/*
 * (C) Copyright 2002
 *
 */
#include <common.h>
#include <errno.h>
#include <dm.h>
#include <fdtdec.h>
#include <asm/arch/clock.h>
#include <asm/io.h>
#include <i2c.h>
#include "fti2c010_i2c.h"

#ifndef CONFIG_SYS_I2C_SPEED
#define CONFIG_SYS_I2C_SPEED    5000
#endif

#ifndef CONFIG_SYS_I2C_SLAVE
#define CONFIG_SYS_I2C_SLAVE    0
#endif

#ifndef CONFIG_FTI2C010_CLOCK
#define CONFIG_FTI2C010_CLOCK   clk_get_rate("I2C")
#endif

#ifndef CONFIG_FTI2C010_TIMEOUT
#define CONFIG_FTI2C010_TIMEOUT 10 /* ms */
#endif

/* 7-bit dev address + 1-bit read/write */
#define I2C_RD(dev)             ((((dev) << 1) & 0xfe) | 1)
#define I2C_WR(dev)             (((dev) << 1) & 0xfe)
DECLARE_GLOBAL_DATA_PTR;

/*
 * Wait til the byte transfer is completed.
 *
 * @param i2c- pointer to the appropriate i2c register bank.
 * @return I2C_OK, if transmission was ACKED
 *         I2C_NACK, if transmission was NACKED
 *         I2C_NOK_TIMEOUT, if transaction did not complete in I2C_TIMEOUT_MS
 */

static int fti2c010_wait(struct fti2c010_regs *regs, uint32_t mask)
{
	int ret = -1;
	uint32_t stat, ts;

	for (ts = get_timer(0); get_timer(ts) < CONFIG_FTI2C010_TIMEOUT; ) {
		stat = readl(&regs->sr);
#ifdef CONFIG_SYS_I2C_FTI2C010_V1_16_0
		writel(stat, &regs->sr);
#endif
		if ((stat & mask) == mask) {
			ret = 0;
			break;
		}
	}

	return ret;
}

static unsigned int fti2c010_set_i2c_bus_speed_reg(struct fti2c010_regs *regs,
	unsigned int speed)
{
	unsigned int clk = CONFIG_FTI2C010_CLOCK;
	unsigned int gsr = 5;
	unsigned int tsr = 5;
	unsigned int div, rate;

	for (div = 0; div < 0x3ffff; ++div) {
		/* SCLout = PCLK/(2*(COUNT + 2) + GSR) */
		rate = clk / (2 * (div + 2) + gsr);
		if (rate <= speed)
			break;
	}

	writel(TGSR_GSR(gsr) | TGSR_TSR(tsr), &regs->tgsr);
	writel(CDR_DIV(div), &regs->cdr);

	return rate;
}

static int fti2c010_reset(struct fti2c010_regs *regs)
{
	ulong ts;
	int ret = -1;

	writel(CR_I2CRST, &regs->cr);
	for (ts = get_timer(0); get_timer(ts) < CONFIG_FTI2C010_TIMEOUT; ) {
		if (!(readl(&regs->cr) & CR_I2CRST)) {
			ret = 0;
			break;
		}
	}

	if (ret)
		printf("fti2c010: reset timeout\n");

	return ret;
}

static void i2c_ch_init(struct fti2c010_regs *regs, int speed, int slaveadd)
{
	fti2c010_reset(regs);
	fti2c010_set_i2c_bus_speed_reg(regs, speed);
}

static int faraday_i2c_set_bus_speed(struct udevice *dev, unsigned int speed)
{
	struct faraday_i2c_bus *i2c_bus = dev_get_priv(dev);

	i2c_bus->clock_frequency = speed;

	i2c_ch_init(i2c_bus->regs, i2c_bus->clock_frequency,
		    CONFIG_SYS_I2C_SLAVE);

	return 0;
}

static int faraday_i2c_probe(struct udevice *dev, uint chip, uint chip_flags)
{
	struct faraday_i2c_bus *i2c_bus = dev_get_priv(dev);
	struct fti2c010_regs *regs = i2c_bus->regs;
	int ret;

	/* 1. Select slave device (7bits Address + 1bit R/W) */
	writel(I2C_WR(chip), &regs->dr);
	writel(CR_ENABLE | CR_TBEN | CR_START, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
	ret = fti2c010_wait(regs, SR_DT);
#else
	ret = fti2c010_wait(regs, SR_TD);
#endif
	if (ret)
		return ret;

	/* 2. Select device register */
	writel(0, &regs->dr);
	writel(CR_ENABLE | CR_TBEN, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
	ret = fti2c010_wait(regs, SR_DT);
#else
	ret = fti2c010_wait(regs, SR_TD);
#endif
	return ret;
}

static void to_i2c_addr(u8 *buf, uint32_t addr, int alen)
{
	int i, shift;

	if (!buf || alen <= 0)
		return;

	/* MSB first */
	i = 0;
	shift = (alen - 1) * 8;
	while (alen-- > 0) {
		buf[i++] = (u8)(addr >> shift);
		shift -= 8;
	}
}

static int fti2c010_read(struct fti2c010_regs *regs,
			u8 dev, uint addr, int alen, uchar *buf, int len)
{
	int ret, pos;
	uchar paddr[4] = { 0 };

	to_i2c_addr(paddr, addr, alen);

	/*
	 * Phase A. Set register address
	 */

	/* A.1 Select slave device (7bits Address + 1bit R/W) */
	writel(I2C_WR(dev), &regs->dr);
	writel(CR_ENABLE | CR_TBEN | CR_START, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
	ret = fti2c010_wait(regs, SR_DT);
#else
	ret = fti2c010_wait(regs, SR_TD);
#endif
	if (ret)
		return ret;

	/* A.2 Select device register */
	for (pos = 0; pos < alen; ++pos) {
		uint32_t ctrl = CR_ENABLE | CR_TBEN;

		writel(paddr[pos], &regs->dr);
		writel(ctrl, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
		ret = fti2c010_wait(regs, SR_DT);
#else
		ret = fti2c010_wait(regs, SR_TD);
#endif
		if (ret)
			return ret;
	}

	/*
	 * Phase B. Get register data
	 */

	/* B.1 Select slave device (7bits Address + 1bit R/W) */
	writel(I2C_RD(dev), &regs->dr);
	writel(CR_ENABLE | CR_TBEN | CR_START, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
	ret = fti2c010_wait(regs, SR_DT);
#else
	ret = fti2c010_wait(regs, SR_TD);
#endif
	if (ret)
		return ret;

	/* B.2 Get register data */
	for (pos = 0; pos < len; ++pos) {
		uint32_t ctrl = CR_ENABLE | CR_TBEN;
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
		uint32_t stat = SR_DR;
#else
		uint32_t stat = SR_TD;
#endif
		if (pos == len - 1) {
			ctrl |= CR_NAK | CR_STOP;
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
			stat |= SR_ACK;
#endif
		}
		writel(ctrl, &regs->cr);
		ret = fti2c010_wait(regs, stat);
		if (ret)
			break;
		buf[pos] = (uchar)(readl(&regs->dr) & 0xFF);
	}

	return ret;
}

static int fti2c010_write(struct fti2c010_regs *regs,
			u8 dev, uint addr, int alen, u8 *buf, int len)
{
	int ret, pos;
	uchar paddr[4] = { 0 };

	to_i2c_addr(paddr, addr, alen);

	/*
	 * Phase A. Set register address
	 *
	 * A.1 Select slave device (7bits Address + 1bit R/W)
	 */
	writel(I2C_WR(dev), &regs->dr);
	writel(CR_ENABLE | CR_TBEN | CR_START, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
	ret = fti2c010_wait(regs, SR_DT);
#else
	ret = fti2c010_wait(regs, SR_TD);
#endif
	if (ret)
		return ret;

	/* A.2 Select device register */
	for (pos = 0; pos < alen; ++pos) {
		uint32_t ctrl = CR_ENABLE | CR_TBEN;

		writel(paddr[pos], &regs->dr);
		writel(ctrl, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
		ret = fti2c010_wait(regs, SR_DT);
#else
		ret = fti2c010_wait(regs, SR_TD);
#endif
		if (ret)
			return ret;
	}

	/*
	 * Phase B. Set register data
	 */
	for (pos = 0; pos < len; ++pos) {
		uint32_t ctrl = CR_ENABLE | CR_TBEN;

		if (pos == len - 1)
			ctrl |= CR_STOP;

		writel(buf[pos], &regs->dr);
		writel(ctrl, &regs->cr);
#ifndef CONFIG_SYS_I2C_FTI2C010_V1_16_0
		ret = fti2c010_wait(regs, SR_DT);
#else
		ret = fti2c010_wait(regs, SR_TD);
#endif
		if (ret)
			break;
	}

	return ret;
}

static int faraday_i2c_xfer(struct udevice *dev, struct i2c_msg *msg,
			    int nmsgs)
{
	struct faraday_i2c_bus *i2c_bus = dev_get_priv(dev);
	struct fti2c010_regs *regs = i2c_bus->regs;
	struct i2c_msg *m_start = msg;
	unsigned int alen, addr, internal_address = 0;
	int i, ret = 0;

	debug("i2c_xfer: %d messages\n", nmsgs);
	if (nmsgs == 2) {
		/* 1st msg is put into the internal address, start with 2nd */
		m_start = &msg[1];

		/* the max length of internal address is 3 bytes */
		if (msg->len > 3)
			return -EFAULT;

		debug("msg->len %d (alen)\n", msg->len);
		alen = msg->len;
		for (i = 0; i < msg->len; ++i) {
			addr = msg->buf[msg->len - 1 - i];
			internal_address |= addr << (8 * i);
			debug("i2c_xfer: internal_address 0x%x\n", internal_address);
		}
	} else {
		debug("msg->len %d (alen + 1)\n", msg->len);
		alen = msg->len - 1;
		for (i = 0; i < msg->len - 1 ; ++i) {
			addr = msg->buf[msg->len - 2 - i];
			internal_address |= addr << (8 * i);
			debug("i2c_xfer: internal_address 0x%x\n", internal_address);
		}
	}

	if (nmsgs == 2) { //normal read command
		debug("i2c_xfer: nmsgs=%d, chip=0x%x, len=0x%x, flags=0x%x, internal_addr=0x%x\n",
		       nmsgs, m_start->addr, m_start->len, m_start->flags, internal_address);
		if (m_start->flags & I2C_M_RD) {
			debug("M_RD\n");
			ret = fti2c010_read(regs, m_start->addr, internal_address, alen, m_start->buf, m_start->len);
		} else {
//			debug("M_WR\n");
//			ret = fti2c010_write(regs, m_start->addr, internal_address, alen, m_start->buf, m_start->len);
		}
		if (ret) {
			debug("i2c_xfer: error %s\n", (m_start->flags & I2C_M_RD) ? "reading" : "writing");
			return -EREMOTEIO;
		}
	}
	if (nmsgs == 1) { //should check what command
		debug("i2c_xfer: nmsgs=%d, chip=0x%x, len=0x%x, flags=0x%x, internal_addr=0x%x\n",
		       nmsgs, msg->addr, msg->len, msg->flags, internal_address);
		if (msg->flags & I2C_M_RD) {
			debug("M_RD\n");
			ret = fti2c010_read(regs, msg->addr, internal_address, alen, msg->buf, msg->len);
		} else {
			debug("M_WR\n");
			ret = fti2c010_write(regs, msg->addr, internal_address, alen, &(msg->buf[msg->len-1]), 1); //in u-boot, it's 1 byte 1 byte...
		}
		if (ret) {
			debug("i2c_xfer: error %s\n", (msg->flags & I2C_M_RD) ? "reading" : "writing");
			return -EREMOTEIO;
		}
	}

	return 0;
}

static int faraday_i2c_ofdata_to_platdata(struct udevice *dev)
{
	const void *blob = gd->fdt_blob;
	struct faraday_i2c_bus *i2c_bus = dev_get_priv(dev);
	int node;
	fdt_addr_t addr;
	
	node = dev_of_offset(dev);

	addr = devfdt_get_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	i2c_bus->regs = (struct fti2c010_regs *)addr;

	i2c_bus->clock_frequency = fdtdec_get_int(blob, node,
						  "clock-frequency", 100000);
	i2c_bus->node = node;
	i2c_bus->bus_num = dev->seq_;
	i2c_bus->active = true;

	return 0;
}

static const struct dm_i2c_ops faraday_i2c_ops = {
	.xfer		= faraday_i2c_xfer,
	.probe_chip	= faraday_i2c_probe,
	.set_bus_speed	= faraday_i2c_set_bus_speed,
};

static const struct udevice_id fti2c010_i2c_ids[] = {
	{ .compatible = "faraday,fti2c010-i2c" },
	{ }
};

U_BOOT_DRIVER(i2c_faraday) = {
	.name	= "i2c_faraday",
	.id	= UCLASS_I2C,
	.of_match = fti2c010_i2c_ids,
	.of_to_plat = faraday_i2c_ofdata_to_platdata,
	.priv_auto = sizeof(struct faraday_i2c_bus),
	.ops	= &faraday_i2c_ops,
};
