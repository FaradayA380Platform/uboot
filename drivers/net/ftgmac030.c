/*
 * Faraday FTGMAC030 Ethernet
 *
 * (C) Copyright 2009 Faraday Technology
 * Po-Yu Chuang <ratbert@faraday-tech.com>
 *
 * (C) Copyright 2010 Andes Technology
 * Macpaul Lin <macpaul@andestech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <cpu_func.h>
#include <malloc.h>
#include <miiphy.h>
#include <net.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/mii.h>
#include <phy.h>
#include "ftgmac030.h"

#define CONFIG_FTGMAC030_EGIGA

#define ETH_ZLEN	60
#define CFG_XBUF_SIZE	1536

/* RBSR - hw default init value is also 0x640 */
#define RBSR_DEFAULT_VALUE	0x640

/* PKTBUFSTX/PKTBUFSRX must both be power of 2 */
#define PKTBUFSTX	4	/* must be power of 2 */

#define TX_DMA_DESC_SIZE	(PKTBUFSTX * sizeof(struct ftgmac030_txdes))
#define RX_DMA_DESC_SIZE	(PKTBUFSRX * sizeof(struct ftgmac030_rxdes))

#ifdef CONFIG_PHY_MICREL_KSZ9896C
#define KSZ9896C_I2C_CHIP_ADDR 0x5F
extern int ksz9896c_i2c_write(uint addr, uint val, uint alen);
extern int ksz9896c_i2c_read(uint addr, uint alen);
#endif

struct ftgmac030_data {
	ulong txdes_dma;
	struct ftgmac030_txdes *txdes;
	ulong rxdes_dma;
	struct ftgmac030_rxdes *rxdes;
	int tx_index;
	int rx_index;
	int phy_addr;
	struct phy_device *phydev;
	struct mii_dev *bus;
	char mdc_cycthr;
};

/*
 * struct mii_bus functions
 */
static int ftgmac030_mdiobus_read(struct eth_device *dev, int phy_addr,
	int regnum)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;
	struct ftgmac030_data *priv = dev->priv;
	int phycr;
	int i;

	phycr = FTGMAC030_PHYCR_ST(1) | FTGMAC030_PHYCR_OP(2) |
		FTGMAC030_PHYCR_PHYAD(phy_addr) |
		FTGMAC030_PHYCR_REGAD(regnum) |
		FTGMAC030_PHYCR_MIIRD | priv->mdc_cycthr;

	writel(phycr, &ftgmac030->phycr);

	for (i = 0; i < 100; i++) {
		phycr = readl(&ftgmac030->phycr);

		if ((phycr & FTGMAC030_PHYCR_MIIRD) == 0) {
			int data;

			data = readl(&ftgmac030->phydata);
			return FTGMAC030_PHYDATA_MIIRDATA(data);
		}

		mdelay(1);
	}

	debug("mdio read timed out\n");

	return -1;
}

static int ftgmac030_mdiobus_write(struct eth_device *dev, int phy_addr,
	int regnum, u16 value)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;
	struct ftgmac030_data *priv = dev->priv;
	int phycr;
	int data;
	int i;

	phycr = FTGMAC030_PHYCR_ST(1) | FTGMAC030_PHYCR_OP(1) |
		FTGMAC030_PHYCR_PHYAD(phy_addr) |
		FTGMAC030_PHYCR_REGAD(regnum) |
		FTGMAC030_PHYCR_MIIWR | priv->mdc_cycthr;

	data = FTGMAC030_PHYDATA_MIIWDATA(value);

	writel(data, &ftgmac030->phydata);
	writel(phycr, &ftgmac030->phycr);

	for (i = 0; i < 100; i++) {
		phycr = readl(&ftgmac030->phycr);

		if ((phycr & FTGMAC030_PHYCR_MIIWR) == 0) {
			debug("(phycr & FTGMAC030_PHYCR_MIIWR) == 0: " \
				"phy_addr: %x\n", phy_addr);
			return 0;
		}

		mdelay(1);
	}

	debug("mdio write timed out\n");

	return -1;
}

#if defined(CONFIG_CMD_MII) 

static int ftgmac030_miiphy_read(struct mii_dev *bus, int phy_adr, int devad, int reg)
{
	int value;
	struct eth_device *dev = eth_get_dev_by_name(bus->name);
	struct ftgmac030_data *priv = dev->priv;

	if (priv->phy_addr != phy_adr)
		return -1;

	value = ftgmac030_mdiobus_read(dev, phy_adr, reg);

	return value;
}

static int ftgmac030_miiphy_write(struct mii_dev *bus, int phy_adr, int devad, int reg,
		      u16 value)
{
	struct eth_device *dev = eth_get_dev_by_name(bus->name);
	struct ftgmac030_data *priv = dev->priv;

	if (priv->phy_addr != phy_adr)
		return -1;

	ftgmac030_mdiobus_write(dev, phy_adr, reg, value);

	return 0;
}

#endif

int ftgmac030_phy_read(struct eth_device *dev, int addr, int reg, u16 *value)
{
#ifdef CONFIG_PHY_MICREL_KSZ9896C
	struct ftgmac030_data *priv = dev->priv;
	u16 tmp;

	tmp = ksz9896c_i2c_read(reg*2 + 0x100 + priv->phy_addr*0x1000, 2);
	tmp = (tmp << 8)| ksz9896c_i2c_read(reg*2 + 0x100 + 1 + priv->phy_addr*0x1000, 2);
	*value = tmp;
#else
	*value = ftgmac030_mdiobus_read(dev , addr, reg);
#endif
	if (*value == -1)
		return -1;

	return 0;
}

int ftgmac030_phy_write(struct eth_device *dev, int addr, int reg, u16 value)
{
#ifdef CONFIG_PHY_MICREL_KSZ9896C
	struct ftgmac030_data *priv = dev->priv;

	if (ksz9896c_i2c_write(reg*2 + 0x100 +  priv->phy_addr*0x1000, value&0xff, 2))//use port 1 , phy address stars from 0x1100
		return -1;
	if (ksz9896c_i2c_write(reg*2 + 0x100 + 1 + priv->phy_addr*0x1000, (value>>8)&0xff, 2))//use port 1 , phy address stars from 0x1100
		return -1;
#else
	if (ftgmac030_mdiobus_write(dev, addr, reg, value) == -1)
		return -1;
#endif

	return 0;
}

static int ftgmac030_phy_reset(struct eth_device *dev)
{
	struct ftgmac030_data *priv = dev->priv;
	int i;
	u16 status, adv;

	adv = ADVERTISE_CSMA | ADVERTISE_ALL;

	ftgmac030_phy_write(dev, priv->phy_addr, MII_ADVERTISE, adv);

	printf("%s: Starting autonegotiation...\n", dev->name);

	ftgmac030_phy_write(dev, priv->phy_addr,
		MII_BMCR, (BMCR_ANENABLE | BMCR_ANRESTART));

	for (i = 0; i < 150000 / 100; i++) {
		ftgmac030_phy_read(dev, priv->phy_addr, MII_BMSR, &status);

		if (status & BMSR_ANEGCOMPLETE)
			break;
		mdelay(1);
	}

	if (status & BMSR_ANEGCOMPLETE) {
		printf("%s: Autonegotiation complete\n", dev->name);
	} else {
		printf("%s: Autonegotiation timed out (status=0x%04x)\n",
		       dev->name, status);
		return 0;
	}

	return 1;
}

#ifdef CONFIG_PHY_MICREL_KSZ9896C
static int ftgmac030_ksz9896c_find_link_port(struct eth_device *dev)
{
	struct ftgmac030_data *priv = dev->priv;
	int port, tmp;

	priv->phy_addr = KSZ9896C_I2C_CHIP_ADDR;
	//check which port is linked up
	for (port = 1; port < PHY_MAX_ADDR; port++) { //check PHY BASIC STATUS register
		tmp = ksz9896c_i2c_read(0x3 + 0x100 + port*0x1000, 2);
		if (tmp == 0x6d)
			break;
	}

	return port;
}
#endif
static int ftgmac030_phy_init(struct eth_device *dev)
{
	struct ftgmac030_data *priv = dev->priv;

	int phy_addr;
	u16 phy_id, status, adv, lpa, stat_ge;
	int media, speed, duplex;
	int i;

	/* Check if the PHY is up to snuff... */
#ifdef CONFIG_PHY_MICREL_KSZ9896C
	priv->phy_addr = phy_id = ftgmac030_ksz9896c_find_link_port(dev) ;         
	goto check_phy;
#endif
	for (phy_addr = 0; phy_addr < PHY_MAX_ADDR; phy_addr++) {

		ftgmac030_phy_read(dev, phy_addr, MII_PHYSID1, &phy_id);
		/*
		 * When it is unable to found PHY,
		 * the interface usually return 0xffff or 0x0000
		 */
		if (phy_id != 0xffff && phy_id != 0x0) {
			debug("%s: found PHY at 0x%02x\n",
				dev->name, phy_addr);
			priv->phy_addr = phy_addr;
			break;
		}
	}

check_phy:
	if (phy_id == 0xffff || phy_id == 0x0) {
		printf("%s: no PHY present\n", dev->name);
		return 0;
	}

#ifdef CONFIG_PHYLIB
#ifdef CONFIG_DM_ETH
	priv->phydev = phy_connect(priv->bus, priv->phy_addr, dev,
				 priv->phy_interface);
#else
	/* need to consider other phy interface mode */
	priv->phydev = phy_connect(priv->bus, priv->phy_addr, dev,
				 PHY_INTERFACE_MODE_RGMII);
#endif
	if (!priv->phydev) {
		printf("phy_connect failed\n");
		return -ENODEV;
	}

	phy_config(priv->phydev);
#endif

#ifdef CONFIG_ARCH_LEO
	ftgmac030_phy_read(dev, priv->phy_addr, MII_BMSR, &status);

	if (!(status & BMSR_LSTATUS)) {
		/* Try to re-negotiate if we don't have link already. */
		ftgmac030_phy_reset(dev);
#ifdef CONFIG_PHY_MICREL_KSZ9896C
		priv->phy_addr = phy_id = ftgmac030_ksz9896c_find_link_port(dev) ;
#endif
		for (i = 0; i < 100000 / 100; i++) {
			ftgmac030_phy_read(dev, priv->phy_addr,
				MII_BMSR, &status);
			if (status & BMSR_LSTATUS)
				break;
			udelay(100);
		}
	}

	if (!(status & BMSR_LSTATUS)) {
		printf("%s: link down\n", dev->name);
		return 0;
	}

#ifdef CONFIG_FTGMAC030_EGIGA
	/* 1000 Base-T Status Register */
	ftgmac030_phy_read(dev, priv->phy_addr,
		MII_STAT1000, &stat_ge);

	speed = (stat_ge & (LPA_1000FULL | LPA_1000HALF)
		 ? 1 : 0);

	duplex = ((stat_ge & LPA_1000FULL)
		 ? 1 : 0);

	if (speed) { /* Speed is 1000 */
		printf("%s: link up, 1000bps %s-duplex\n",
			dev->name, duplex ? "full" : "half");
#ifdef CONFIG_PHY_MICREL_KSZ9896C
		//update ksz9896c register for 1000M
		ksz9896c_i2c_write(0x6301 ,0x18, 2);
#endif
		return 0;
	}
#endif

	ftgmac030_phy_read(dev, priv->phy_addr, MII_ADVERTISE, &adv);
	ftgmac030_phy_read(dev, priv->phy_addr, MII_LPA, &lpa);

	media = mii_nway_result(lpa & adv);
	speed = (media & (ADVERTISE_100FULL | ADVERTISE_100HALF) ? 1 : 0);
	duplex = (media & ADVERTISE_FULL) ? 1 : 0;

#ifdef CONFIG_PHY_MICREL_KSZ9896C
	//update ksz9896c register for 100M
	ksz9896c_i2c_write(0x6301 ,0x58, 2);
#endif
	printf("%s: link up, %sMbps %s-duplex\n",
	       dev->name, speed ? "100" : "10", duplex ? "full" : "half");

	return 1;
#else
	if (phy_startup(priv->phydev))
		return 1;

	return 0;
#endif
}

static int ftgmac030_update_link_speed(struct eth_device *dev)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;
	struct ftgmac030_data *priv = dev->priv;

	unsigned short stat_fe;
	unsigned short stat_ge;
	unsigned int maccr;

	/* read MAC control register and clear related bits */
	maccr = readl(&ftgmac030->maccr) &
		~(FTGMAC030_MACCR_GIGA_MODE |
		  FTGMAC030_MACCR_FAST_MODE |
		  FTGMAC030_MACCR_FULLDUP);

	if (priv->phydev->duplex & DUPLEX_FULL) {
			maccr |= FTGMAC030_MACCR_FULLDUP;
		}

	if (priv->phydev->speed == SPEED_1000) {
		maccr |= FTGMAC030_MACCR_GIGA_MODE;
	} else if (priv->phydev->speed == SPEED_100) {
			maccr |= FTGMAC030_MACCR_FAST_MODE;
		}

	/* update MII config into maccr */
	writel(maccr, &ftgmac030->maccr);

	printf("%s: link up, %d Mbps %s-duplex\n", priv->phydev->dev->name,
	       priv->phydev->speed, priv->phydev->duplex ? "full" : "half");

	return 1;
}

/*
 * Reset MAC
 */
static void ftgmac030_reset(struct eth_device *dev)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;

	debug("%s()\n", __func__);

	writel(FTGMAC030_MACCR_SW_RST, &ftgmac030->maccr);

	while (readl(&ftgmac030->maccr) & FTGMAC030_MACCR_SW_RST)
		;
}

/*
 * Set MAC address
 */
static void ftgmac030_set_mac(struct eth_device *dev,
	const unsigned char *mac)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;
	unsigned int maddr = mac[0] << 8 | mac[1];
	unsigned int laddr = mac[2] << 24 | mac[3] << 16 | mac[4] << 8 | mac[5];

	debug("%s(%x %x)\n", __func__, maddr, laddr);

	writel(maddr, &ftgmac030->mac_madr);
	writel(laddr, &ftgmac030->mac_ladr);
}

static void ftgmac030_set_mac_from_env(struct eth_device *dev)
{
	eth_env_get_enetaddr("ethaddr", dev->enetaddr);

	ftgmac030_set_mac(dev, dev->enetaddr);
}

static int ftgmac030_write_hwaddr(struct eth_device *dev)
{
	ftgmac030_set_mac(dev, dev->enetaddr);

	return 0;
}

/*
 * disable transmitter, receiver
 */
static void ftgmac030_halt(struct eth_device *dev)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;

	debug("%s()\n", __func__);

	writel(0, &ftgmac030->maccr);
}

static int ftgmac030_init(struct eth_device *dev, struct bd_info *bis)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;
	struct ftgmac030_data *priv = dev->priv;
	struct ftgmac030_txdes *txdes;
	struct ftgmac030_rxdes *rxdes;
	unsigned int maccr;
	void *buf;
	int i;

	debug("%s()\n", __func__);

	if (!priv->txdes) {
		txdes = dma_alloc_coherent(
			sizeof(*txdes) * PKTBUFSTX, &priv->txdes_dma);
		if (!txdes)
			panic("ftgmac030: out of memory\n");
		memset(txdes, 0, sizeof(*txdes) * PKTBUFSTX);
		priv->txdes = txdes;
	}
	txdes = priv->txdes;

	if (!priv->rxdes) {
		rxdes = dma_alloc_coherent(
			sizeof(*rxdes) * PKTBUFSRX, &priv->rxdes_dma);
		if (!rxdes)
			panic("ftgmac030: out of memory\n");
		memset(rxdes, 0, sizeof(*rxdes) * PKTBUFSRX);
		priv->rxdes = rxdes;
	}
	rxdes = priv->rxdes;

	writel(FTGMAC030_MACCR_SW_RST, &ftgmac030->maccr);
	for(i = 0; i < 5; i++)
	{
		maccr= readl(&ftgmac030->maccr);
		if (!(maccr & FTGMAC030_MACCR_SW_RST))
			break;
		mdelay(1);
	}

	/* set the ethernet address */
	ftgmac030_set_mac(dev, dev->enetaddr);

	/* disable all interrupts */
	writel(0, &ftgmac030->ier);

	/* initialize descriptors */
	priv->tx_index = 0;
	priv->rx_index = 0;

	txdes[PKTBUFSTX - 1].txdes0	= FTGMAC030_TXDES0_EDOTR;
	rxdes[PKTBUFSRX - 1].rxdes0	= FTGMAC030_RXDES0_EDORR;

	for (i = 0; i < PKTBUFSTX; i++) {
		/* TXBUF_BADR */
		if (!txdes[i].txdes2) {
			buf = memalign(ARCH_DMA_MINALIGN, CFG_XBUF_SIZE);
			if (!buf)
				panic("ftgmac030: out of memory\n");
			txdes[i].txdes3 = virt_to_phys(buf);
			txdes[i].txdes2 = (uintptr_t)buf;
		}
		txdes[i].txdes1 = 0;
	}

	for (i = 0; i < PKTBUFSRX; i++) {
		/* RXBUF_BADR */
		if (!rxdes[i].rxdes2) {
			buf = net_rx_packets[i];
			rxdes[i].rxdes3 = virt_to_phys(buf);
			rxdes[i].rxdes2 = (uintptr_t)buf;
		}
		rxdes[i].rxdes0 &= ~FTGMAC030_RXDES0_RXPKT_RDY;
	}

	/* transmit ring */
	writel(priv->txdes_dma, &ftgmac030->nptxr_badr);

	/* receive ring */
	writel(priv->rxdes_dma, &ftgmac030->rxr_badr);

	/* poll receive descriptor automatically */
	writel(FTGMAC030_APTC_RXPOLL_CNT(1), &ftgmac030->aptc);

	/* config receive buffer size register */
	writel(FTGMAC030_RBSR_SIZE(RBSR_DEFAULT_VALUE), &ftgmac030->rbsr);

	/* enable transmitter, receiver */
	maccr = readl(&ftgmac030->maccr);
	maccr |= (FTGMAC030_MACCR_TXMAC_EN |
		FTGMAC030_MACCR_RXMAC_EN |
		FTGMAC030_MACCR_TXDMA_EN |
		FTGMAC030_MACCR_RXDMA_EN |
		FTGMAC030_MACCR_CRC_APD |
		FTGMAC030_MACCR_FULLDUP |
		FTGMAC030_MACCR_RX_RUNT |
		FTGMAC030_MACCR_RX_BROADPKT);

	writel(maccr, &ftgmac030->maccr);

	if (!ftgmac030_phy_init(dev)) {
		if (!ftgmac030_update_link_speed(dev))
			return -1;
	}

	return 0;
}

/*
 * Get a data block via Ethernet
 */
static int ftgmac030_recv(struct eth_device *dev)
{
	struct ftgmac030_data *priv = dev->priv;
	struct ftgmac030_rxdes *curr_des;
	unsigned short rxlen;

	/* invalidate d-cache */
	dma_map_single((void *)priv->rxdes, ALIGN(RX_DMA_DESC_SIZE, ARCH_DMA_MINALIGN), DMA_FROM_DEVICE);

	curr_des = &priv->rxdes[priv->rx_index];

	if (!(curr_des->rxdes0 & FTGMAC030_RXDES0_RXPKT_RDY))
		return -1;

	if (curr_des->rxdes0 & (FTGMAC030_RXDES0_RX_ERR |
				FTGMAC030_RXDES0_CRC_ERR |
				FTGMAC030_RXDES0_FTL |
				FTGMAC030_RXDES0_RUNT |
				FTGMAC030_RXDES0_RX_ODD_NB)) {
		return -1;
	}

	rxlen = FTGMAC030_RXDES0_VDBC(curr_des->rxdes0);

	debug("%s(): RX buffer %d, %x received\n",
	       __func__, priv->rx_index, rxlen);

	/* invalidate d-cache */
	dma_map_single((void *)curr_des->rxdes3, ALIGN(rxlen, ARCH_DMA_MINALIGN), DMA_FROM_DEVICE);

	/* pass the packet up to the protocol layers. */
	net_process_received_packet((void *)curr_des->rxdes3, rxlen);

	/* release buffer to DMA */
	curr_des->rxdes0 &= ~FTGMAC030_RXDES0_RXPKT_RDY;

	priv->rx_index = (priv->rx_index + 1) % PKTBUFSRX;

	return 0;
}

/*
 * Send a data block via Ethernet
 */
static int ftgmac030_send(struct eth_device *dev, void *packet, int length)
{
	struct ftgmac030 *ftgmac030 = (struct ftgmac030 *)dev->iobase;
	struct ftgmac030_data *priv = dev->priv;
	struct ftgmac030_txdes *curr_des = &priv->txdes[priv->tx_index];
    
	if (curr_des->txdes0 & FTGMAC030_TXDES0_TXDMA_OWN) {
		debug("%s(): no TX descriptor available\n", __func__);
		printf("%s(): no TX descriptor available priv->tx_index = 0x%x\n", __func__, priv->tx_index);
		return -1;
	}

	debug("%s(%x, %x)\n", __func__, (int)packet, length);
	length = (length < ETH_ZLEN) ? ETH_ZLEN : length;
	memcpy((void *)curr_des->txdes3, (void *)packet, length);

	/* flush d-cache */
	dma_map_single((void *)curr_des->txdes3, ALIGN(length, ARCH_DMA_MINALIGN), DMA_TO_DEVICE);

	/* only one descriptor on TXBUF */
	curr_des->txdes0 &= FTGMAC030_TXDES0_EDOTR;
	curr_des->txdes0 |= FTGMAC030_TXDES0_FTS |
			    FTGMAC030_TXDES0_LTS |
			    FTGMAC030_TXDES0_TXBUF_SIZE(length) |
			    FTGMAC030_TXDES0_TXDMA_OWN ;

	/* flush d-cache */
	dma_map_single((void *)priv->txdes, ALIGN(TX_DMA_DESC_SIZE, ARCH_DMA_MINALIGN), DMA_TO_DEVICE);

	/* start transmit */
	writel(1, &ftgmac030->nptxpd);

	debug("%s(): packet sent\n", __func__);

	priv->tx_index = (priv->tx_index + 1) % PKTBUFSTX;

	return 0;
}

int ftgmac030_initialize(struct bd_info *bis)
{
	struct eth_device *dev;
#if defined(CONFIG_CMD_MII) || defined(CONFIG_PHYLIB)
	struct mii_dev *mdiodev;
#endif
	struct ftgmac030_data *priv;
	unsigned long osclk;
	int retval;

	dev = malloc(sizeof *dev);
	if (!dev) {
		printf("%s(): failed to allocate dev\n", __func__);
		retval = -ENOMEM;
		goto out;
	}

	/* Transmit and receive descriptors should align to 16 bytes */
	priv = memalign(16, sizeof(struct ftgmac030_data));
	if (!priv) {
		printf("%s(): failed to allocate priv\n", __func__);
		retval = -ENOMEM;
		goto free_dev;
	}

	memset(dev, 0, sizeof(*dev));
	memset(priv, 0, sizeof(*priv));

	strcpy(dev->name, "FTGMAC030");
	dev->iobase	= CONFIG_FTGMAC030_BASE;
	dev->init	= ftgmac030_init;
	dev->halt	= ftgmac030_halt;
	dev->send	= ftgmac030_send;
	dev->recv	= ftgmac030_recv;
	dev->write_hwaddr = ftgmac030_write_hwaddr;
	dev->priv	= priv;

	eth_register(dev);

#if defined(CONFIG_CMD_MII) || defined(CONFIG_PHYLIB)
	mdiodev = mdio_alloc();
	if (!mdiodev) {
		printf("%s(): failed to allocate mdiodev\n", __func__);
		retval = -ENOMEM;
		goto unregister_eth;
	}

	strncpy(mdiodev->name, dev->name, MDIO_NAME_LEN);
	mdiodev->read = ftgmac030_miiphy_read;
	mdiodev->write = ftgmac030_miiphy_write;

	retval = mdio_register(mdiodev);
	if (retval < 0) {
		printf("%s(): failed to register mdiodev\n", __func__);
		goto free_mdiodev;
	}

	priv->bus = mdiodev;
#endif

	osclk = clk_get_rate("MAC")/1000000;	//MHz
	priv->mdc_cycthr = (400*osclk)/1000;

	ftgmac030_reset(dev);

	return 0;

#if defined(CONFIG_CMD_MII) || defined(CONFIG_PHYLIB)
free_mdiodev:
	mdio_free(mdiodev);
unregister_eth:
	eth_unregister(dev);
#endif
free_dev:
	free(dev);
out:
	return retval;
}
