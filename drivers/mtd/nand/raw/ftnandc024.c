/*
 * Faraday NAND Flash Controller
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <malloc.h>
#include <nand.h>
#include <part.h>
#include <asm/io.h>
#include <linux/errno.h>
#include <linux/libfdt.h>
#include <linux/math64.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <faraday/ftnandc024.h>
#include <faraday/nand.h>

/*********************************************************************************/
#if CONFIG_NAND_FTNANDC024_DEBUG > 2
#define PRINTK3(args...)	printf(args)
#else
#define PRINTK3(args...)	do { } while(0)
#endif

#if CONFIG_NAND_FTNANDC024_DEBUG > 1
#define PRINTK2(args...)	printf(args)
#else
#define PRINTK2(args...)	do { } while(0)
#endif

#if CONFIG_NAND_FTNANDC024_DEBUG > 0
#define PRINTK1(args...)	printf(args)
#define PRINTK (args...)	printf(args)
#else
#define PRINTK1(args...)	do { } while(0)
#define PRINTK (args...)	printf(args)
#endif

#undef CFG_USE_SCRAMBLER
#undef CFG_NO_HWECC
#undef CFG_ECC_NOPARITY

#define CFG_CMD_TIMEOUT	(CONFIG_SYS_HZ >> 2)	/* 250ms */
#define CFG_PIO_TIMEOUT	(CONFIG_SYS_HZ >> 3)	/* 125ms */

#ifdef CONFIG_SYS_NAND_ONFI_DETECTION
#undef CONFIG_SYS_NAND_ONFI_DETECTION
#endif

/*********************************************************************************/

#if defined (CONFIG_FTNANDC024_HYNIX_H27U1G8F2)
static struct ftnandc024_chip_timing chip_timing = {
	15, 10, 10, 10, 0, 25, 15, 0, 0, 0,
	30, 25, 100, 100, 60, 0, 0, 20, 10, 0,
	0, 30, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100};
#elif defined (CONFIG_FTNANDC024_SAMSUNG_K9F1G08U0D) ||\
	defined (CONFIG_FTNANDC024_SAMSUNG_K9F4G08U0D)
static struct ftnandc024_chip_timing chip_timing = {
	10, 5, 5, 5, 0, 12, 10, 0, 0, 0,
	20, 12, 100, 0, 60, 0, 100, 20, 10, 0,
	70, 100, 0, 20, 0, 12, 10, 12, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif defined (CONFIG_FTNANDC024_SPANSION_S34ML01G100TF100)
static struct ftnandc024_chip_timing chip_timing = {
	10, 5, 5, 5, 0, 12, 10, 10, 0, 0,
	20, 12, 100, 0, 60, 0, 100, 20, 10, 25,
	70, 100, 0, 20, 0, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif defined (CONFIG_FTNANDC024_WINBOND_W29N01GVSCAA)
static struct ftnandc024_chip_timing chip_timing = {
	10, 5, 5, 5, 0, 12, 10, 0, 0, 0,
	0, 12, 100, 0, 60, 0, 100, 20, 10, 25,
	70, 100, 0, 15, 0, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#elif defined (CONFIG_FTNANDC024_MICRON_30LF4G28AD)
static struct ftnandc024_chip_timing chip_timing = {
	7, 5, 5, 5, 0, 10, 10, 0, 0, 0,
	16, 10, 100, 0, 60, 0, 60, 20, 10, 20,
	70, 60, 60, 15, 0, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#elif defined (CONFIG_FTNANDC024_ESMT_F59L1G81MB)
static struct ftnandc024_chip_timing chip_timing = {
	10, 5, 5, 5, 0, 12, 10, 0, 0, 0,
	20, 12, 100, 0, 60, 0, 100, 20, 10, 25,
	100, 100, 64, 20, 0, 12, 10, 12, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#elif defined (CONFIG_FTNANDC024_WINBOND_W29N08GZSIBA)
static struct ftnandc024_chip_timing chip_timing = {
	10,	5, 5, 5, 0, 12, 10, 0, 0, 0,
	0, 12, 100, 0, 80, 0, 100, 20, 10, 35,
	70, 100, 0, 20, 0, 10, 10, 10, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif

struct ftnandc024_chip {
	struct mtd_info *mtd;
	void __iomem    *regs;
	void __iomem    *ssram;         /* spare sram */
	void __iomem    *region;        /* data region/pio access port */
	uint32_t        col;            /* current column address */
	uint32_t        row;            /* current row address/page index */
	uint32_t        cmd;            /* current NAND command code */
	uint32_t        oobavail;       /* available oob size */
	uint8_t         bytenum;        /* byte mode max:32*/
	uint8_t         bi_reserve;     /* bi byte reserve from spare region */
	uint8_t         sel_ce;         /* select ce */

	uint32_t        cmd_status;
	uint32_t        chk_data_0xFF;
	uint32_t        chk_spare_0xFF;
	uint32_t        data_empty;
	uint32_t        spare_empty;

	char            flash_raw_id[5];

	struct platform_nand_flash *plat_nand;
};

static struct nand_ecclayout nand_lp_ecclayout = {
//	.oobavail = 32,
	.oobfree = {
		{ 2, 30 },
	},
};

static struct nand_ecclayout nand_sp_ecclayout = {
//	.oobavail = 15,
	.oobfree = {
		{ 0, 5 },
		{ 6, 10 },
	},
};

static struct nand_chip ftnand024_chip;
static struct platform_nand_flash platform_nand;

DECLARE_GLOBAL_DATA_PTR;

unsigned int ftnandc024_ecc_length_convert(unsigned char ecc_bit)
{
	unsigned char ecc_len;
	unsigned int val;

	val = ecc_bit * 14;
	ecc_len = val >> 3;
	if (val != (ecc_len<<3))
		ecc_len += 1;

	return ecc_len;
}

static int ftnandc024_ckst(struct ftnandc024_chip *priv)
{
	struct ftnandc024_regs *regs = priv->regs;
	uint32_t st = readl(&regs->nandc.rdsr[0]) & 0xff;

	if (st & NAND_STATUS_FAIL)
		return -EIO;

	if (!(st & NAND_STATUS_READY))
		return -EBUSY;

	if (!(st & NAND_STATUS_WP))
		return -EIO;

	return 0;
}

static int ftnandc024_wait(struct ftnandc024_chip *priv)
{
	struct mtd_info *mtd = priv->mtd;
	struct ftnandc024_regs *regs = priv->regs;
	int err = -1;
	uint32_t ts;

	PRINTK3("%s\n", __func__);

	for (ts = get_timer(0); get_timer(ts) < CFG_CMD_TIMEOUT; ) {
		/* channel busy */
		if (!(readl(&regs->nandc.sr) & 1))
			continue;

		/* command complete counter */
		if (readl(&regs->cmdqc.ccr) & 7) {
			err = 0;
			mtd->ecc_stats.corrected += readl(&regs->ecc.sr[0]) & 0x7f;
			mtd->ecc_stats.corrected += readl(&regs->ecc.srsr[0]) & 0x7f;

			/* ecc correction fail */
			priv->cmd_status = readl(&regs->ecc.isr);

			PRINTK2("ftnandc024_wait: regs->ecc.isr = 0x%x\n", priv->cmd_status);

			writel(0x10001, &regs->ecc.isr);

			if (priv->cmd_status & 0x10001) {
#ifdef CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
				printf("ftnandc024_wait: ecc failed.\n");
				mtd->ecc_stats.failed++;
#endif //CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
			}
			break;
		}
	}

	if (err) {
		printk("ftnandc024_wait: cmd timeout\n");
	}

	return err ? -ETIMEDOUT : 0;
}

static int ftnandc024_send_cmd(struct ftnandc024_chip *priv, uint32_t *cmdq)
{
	struct ftnandc024_regs *regs = priv->regs;
	ulong ts;

	PRINTK3("%s\n", __func__);

	/* wait until command queue ready */
	for (ts = get_timer(0); get_timer(ts) < CFG_CMD_TIMEOUT; ) {
		if (!(readl(&regs->cmdqc.sr) & FTNANDC024_CMDQC_SR_FULL))
			break;
	}

	if (readl(&regs->cmdqc.sr) & FTNANDC024_CMDQC_SR_FULL) {
		PRINTK2("ftnandc024_send_cmd: cmdq full\n");
		return -ETIMEDOUT;
	}

	/* reset ecc status */
	writel(0x10001, &regs->ecc.isr);

	/* reset nandc status */
	writel(0x01010001, &regs->nandc.isr);

	/* reset command complete counter */
	writel(1, &regs->cmdqc.ccrr);

	/* setup command queue */
	PRINTK2("ftnandc024_send_cmd: cmdq[0] = 0x%x, cmdq[1] = 0x%x, cmdq[2] = 0x%x, cmdq[3] = 0x%x\n", __func__, cmdq[0], cmdq[1], cmdq[2], cmdq[3]);
	writel(cmdq[0], &regs->cmdq[0].word[0]);
	writel(cmdq[1], &regs->cmdq[0].word[1]);
	writel(cmdq[2], &regs->cmdq[0].word[2]);
	writel(cmdq[3], &regs->cmdq[0].word[3]);	/* cmd start */

	return 0;
}

static int ftnandc024_read_oob_raw(struct mtd_info *mtd,
	uint8_t *buf, int page, int access_bi)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t cmdq[6] = { 0 };
	int val;

	val = chip->ecc.steps * chip->ecc.bytes;

	cmdq[0] |= page;

	cmdq[2] |= 1 << 16;	/* counter */
	if(access_bi)
		cmdq[2] |= mtd->writesize + priv->bi_reserve - 1;
	else
		cmdq[2] |= mtd->writesize + val;

	cmdq[3] |= FTNANDC024_CMD_RAWRD << 8;
	cmdq[3] |= (priv->sel_ce) << 29;	/* command starting CE */
	cmdq[3] |= 1 << 28;	/* byte mode */
	cmdq[3] |= (priv->oobavail - 1) << 19; /* spare data size */

	ftnandc024_send_cmd(priv, cmdq);

	if (ftnandc024_wait(priv))
		return -EIO;

	memset(buf, 0xff, mtd->oobsize);
	memcpy(buf, priv->ssram, priv->oobavail);

	return 0;
}

static int ftnandc024_read_byte_manual(struct mtd_info *mtd,
	uint8_t *buf, int page, int col, int bytenum)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t cmdq[6] = { 0 };

	cmdq[0] |= page;

	cmdq[2] |= 1 << 16; /* counter */
	cmdq[2] |= col;

	cmdq[3] |= FTNANDC024_CMD_RAWRD << 8;
	cmdq[3] |= (priv->sel_ce) << 29;	/* command starting CE */
	cmdq[3] |= 1 << 28;	/* byte mode */
	cmdq[3] |= (bytenum - 1) << 19;	/* spare data size */

	ftnandc024_send_cmd(priv, cmdq);

	if (ftnandc024_wait(priv))
		return -EIO;

	memset(buf, 0xff, bytenum);
	memcpy(buf, priv->ssram, bytenum);

	return 0;
}

static int ftnandc024_write_byte_manual(struct mtd_info *mtd,
	const uint8_t *buf, int bytenum)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t cmdq[6] = { 0 };

	memcpy(priv->ssram, buf, bytenum);

	cmdq[0] |= priv->row;
            
	cmdq[2] |= 1 << 16;	/* counter */
	cmdq[2] |= priv->col;
            
	cmdq[3] |= FTNANDC024_CMD_RAWWR << 8;
	cmdq[3] |= (priv->sel_ce) << 29;	/* command starting CE */
	cmdq[3] |= 1 << 28;	/* byte mode */
	cmdq[3] |= (bytenum - 1) << 19;	/* spare data size */

	ftnandc024_send_cmd(priv, cmdq);

	if (ftnandc024_wait(priv))
		return -EIO;

	priv->col += bytenum;

	return 0;
}

static int ftnandc024_read_byte_mode(struct mtd_info *mtd,
	uint8_t *buf, int page)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t cmdq[6] = { 0 };

	cmdq[0] |= page;
            
	cmdq[2] |= 1 << 16;	/* counter */
	cmdq[2] |= priv->col;
            
	cmdq[3] |= FTNANDC024_CMD_RAWRD << 8;
	cmdq[3] |= (priv->sel_ce) << 29;	/* command starting CE */
	cmdq[3] |= 1 << 28;	/* byte mode */
	cmdq[3] |= (priv->bytenum - 1) << 19; /* spare data size */

	ftnandc024_send_cmd(priv, cmdq);

	if (ftnandc024_wait(priv))
		return -EIO;

	memcpy(buf, priv->ssram, priv->bytenum);

	return 0;
}

static void ftnandc024_read_ecc_data(struct mtd_info *mtd,
	uint8_t *buf, int page)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	int i;
	uint8_t offset = 0;

	priv->col = chip->ecc.size;
	priv->bytenum = chip->ecc.bytes;
	for(i = 0; i < chip->ecc.steps; i++) {
		ftnandc024_read_byte_mode(mtd, buf + offset, page);
		offset += chip->ecc.bytes;
		priv->col += (chip->ecc.size + chip->ecc.bytes);
#if (CONFIG_NAND_FTNANDC024_VERSION >= 230)
		if(priv->bi_reserve > 0 && priv->col >= mtd->writesize)
			priv->col += 1;
#endif
	}
}

static int ftnandc024_read_page(struct mtd_info *mtd, struct nand_chip *chip,
	uint8_t *buf, int oob_required, int page)
{
	chip->read_buf(mtd, buf, mtd->writesize);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	return 0;
}

static int ftnandc024_write_page(struct mtd_info *mtd, struct nand_chip *chip,
	const uint8_t *buf, int oob_required, int page)
{
	chip->write_buf(mtd, buf, mtd->writesize);

	return 0;
}

static int ftnandc024_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
	uint8_t *buf, int oob_required, int page)
{
	chip->read_buf(mtd, buf, mtd->writesize);
	chip->read_buf(mtd, chip->oob_poi, mtd->oobsize);

	return 0;
}

static int ftnandc024_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
	const uint8_t *buf, int oob_required, int page)
{
	struct ftnandc024_chip *priv = chip->priv;

	chip->write_buf(mtd, buf, mtd->writesize);

	if (ftnandc024_wait(priv))
		printk("ftnandc024_write_page_raw: cmd timeout\n");

	// Residual Data
#if (CONFIG_NAND_FTNANDC024_VERSION >= 230)
	if(priv->bi_reserve > 0) {
		priv->col += 1;
		ftnandc024_write_byte_manual(mtd, buf + mtd->writesize + 1, chip->ecc.steps * chip->ecc.bytes);
	}else {
		ftnandc024_write_byte_manual(mtd, buf + mtd->writesize, chip->ecc.steps * chip->ecc.bytes);
	}

	// ECC in Spare Region
	priv->col += mtd->oobavail;
	ftnandc024_write_byte_manual(mtd, buf + mtd->writesize + 1 + chip->ecc.steps * chip->ecc.bytes + mtd->oobavail, chip->ecc.bytes);
#else
	ftnandc024_write_byte_manual(mtd, buf + mtd->writesize, chip->ecc.steps * chip->ecc.bytes);

	// ECC in Spare Region
	priv->col += mtd->oobavail;
	ftnandc024_write_byte_manual(mtd, buf + mtd->writesize + chip->ecc.steps * chip->ecc.bytes + mtd->oobavail, chip->ecc.bytes);
#endif

	return 0;
}

/*
 * Check hardware register for wait status. Returns 1 if device is ready,
 * 0 if it is still busy.
 */
static int ftnandc024_dev_ready(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;

	if (!ftnandc024_wait(chip->priv) && !ftnandc024_ckst(chip->priv))
		return 1;

	return 0;
}

static uint8_t ftnandc024_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
	uint8_t ret = 0;

	switch (priv->cmd) {
	case NAND_CMD_READID:
		ret = ((uint8_t __iomem*)priv->ssram)[priv->col++];
		break;
	case NAND_CMD_STATUS:
		ret = readl(&regs->nandc.rdsr[0]) & 0xff;
		break;
	default:
		printf("ftnandc024_read_byte: CMD%d doesn't support byte access\n", priv->cmd);
		break;
	}

	return ret;
}

static uint16_t ftnandc024_read_word(struct mtd_info *mtd)
{
	uint16_t ret = 0xffff;
	uint8_t *buf = (uint8_t *)&ret;

	/* LSB format */
	buf[0] = ftnandc024_read_byte(mtd);
	buf[1] = ftnandc024_read_byte(mtd);

	return ret;
}

/**
 * Read data from NAND controller into buffer
 * @mtd: MTD device structure
 * @buf: buffer to store date
 * @len: number of bytes to read
 */
static void ftnandc024_read_buf(struct mtd_info	*mtd,
	uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t off, val;
#ifndef CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
	uint32_t *lbuf;
	int i;
#endif

	PRINTK3("%s\n", __func__);

	switch (priv->cmd) {
	case NAND_CMD_READOOB:
#if (CONFIG_NAND_FTNANDC024_VERSION >= 230)
		if(chip->ops.mode == MTD_OPS_FTC024_OOB)
			ftnandc024_read_oob_raw(mtd, buf, priv->row, 1);
		else
#endif
			ftnandc024_read_oob_raw(mtd, buf, priv->row, 0);
		break;

	case NAND_CMD_READ0:
		if (priv->col >= mtd->writesize) {
			PRINTK2("ftnandc024_read_buf: ops mode = %d, col = 0x%x\n", chip->ops.mode, priv->col);
			off = priv->col - mtd->writesize;
			len = min(len, (int)(mtd->oobsize - off));
			val = chip->ecc.steps * chip->ecc.bytes;
			if (chip->ops.mode == MTD_OPS_FTC024_OOB)
				memcpy(buf + val, priv->ssram + off, len);
			else
				memcpy(buf, priv->ssram + off, len);
			PRINTK2("ftnandc024_read_buf: priv->col = 0x%x, len = 0x%x, off = 0x%x, val = 0x%x, mtd->oobsize = 0x%x, priv->oobavail = 0x%x\n", priv->col, len, off, val, mtd->oobsize, priv->oobavail);
			priv->col += len;
			if (chip->ops.mode == MTD_OPS_FTC024_OOB) {
				/* get remain spare data */
				off = val + priv->oobavail;
				priv->col = mtd->writesize + off;
				val = mtd->oobsize - off;
				do {
					if(val > FTNANDC024_MAX_SPARE_DATA_BYTE_SIZE) {
						val -= FTNANDC024_MAX_SPARE_DATA_BYTE_SIZE;
						priv->bytenum = FTNANDC024_MAX_SPARE_DATA_BYTE_SIZE;
					}
					else {
						priv->bytenum = val;
						val = 0;	//prepare exit while loop
					}
					PRINTK2("ftnandc024_read_buf: priv->col = 0x%x, val = 0x%x, priv->bytenum = 0x%x\n", priv->col, val, priv->bytenum);
					ftnandc024_read_byte_mode(mtd, buf + off, priv->row);
					priv->col += priv->bytenum;
					off += priv->bytenum;
				} while(val);

				/* get ecc data */
				ftnandc024_read_ecc_data(mtd, buf, priv->row);
#ifdef CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE

#else
				if (priv->chk_spare_0xFF == 1) {
					for (i = 0; i < (priv->oobavail); i++) {
						if(*(buf + i + (chip->ecc.steps * chip->ecc.bytes)) != 0xFF) {
							printf("ftnandc024_read_buf: Spare Read ECC failed i = 0x%x, value = 0x%x \n", i, *(buf + i + (chip->ecc.steps * chip->ecc.bytes)));
							priv->spare_empty = 0;
							mtd->ecc_stats.failed++;
							break;
						}
					}
					priv->chk_spare_0xFF = 0;
				}
#endif //CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
			}
		}
		/* fall-through */
	default:
		for (off = 0; off < len && priv->col < mtd->writesize; ) {
			*(uint32_t *)(buf + off) = readl(priv->region);
			priv->col += 4;
			off += 4;
		}
#ifdef CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE

#else
		if (priv->chk_data_0xFF == 1) {
			lbuf = (uint32_t *)buf;
			for (i = 0; i < (mtd->writesize >> 2); i++) {
				if (*(lbuf + i) != 0xFFFFFFFF) {
					printf("ftnandc024_read_buf: Data Real ECC failed page = 0x%x i = 0x%x value = 0x%x\n", priv->row, i, *(lbuf + i));
					priv->data_empty = 0;
					mtd->ecc_stats.failed++;
					break;
				}
			}
			priv->chk_data_0xFF = 0;
		}
#endif //CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
		break;
	}
}

/**
 * Write buffer to NAND controller
 * @mtd: MTD device structure
 * @buf: data buffer
 * @len: number of bytes to write
 */
static void ftnandc024_write_buf(struct mtd_info *mtd,
	const uint8_t *buf, int len)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t off;

	PRINTK2("ftnandc024_write_buf: buf address = 0x%p, len = 0x%x\n", buf, len);

	for (off = 0; off < len && priv->col < mtd->writesize; ) {
		writel(*(uint32_t *)(buf + off), priv->region);
		priv->col += 4;
		off += 4;
	}
}

static void ftnandc024_cmdfunc(struct mtd_info *mtd,
	unsigned cmd, int col, int row)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
	uint32_t cmdq[6] = { 0 };
	int i;
	int w_wo_spare = 1;

	PRINTK2("ftnandc024_cmdfunc: cmd = 0x%x, col = 0x%x, row = 0x%x\n", cmd, col, row);

	priv->col = col;
	priv->row = row; /* page */
	priv->cmd = cmd;

	/* flash setup */
	priv->oobavail = (chip->page_shift == 9) ? 8 : 32;
	if (mtd->erasesize && mtd->writesize) {
		writel(FTNANDC024_NANDC_MSR0_BPP(mtd->writesize) |
		       FTNANDC024_NANDC_MSR0_PPB(mtd->erasesize / mtd->writesize) |
		       FTNANDC024_NANDC_MSR0_RAC(priv->plat_nand->row_addrcycles) |
		       FTNANDC024_NANDC_MSR0_CAC(priv->plat_nand->col_addrcycles) |
		       FTNANDC024_NANDC_MSR0_BIPP(priv->bi_reserve),
		       &regs->nandc.msr[0]);
		writel(FTNANDC024_NANDC_MSR1_VPPB(mtd->erasesize / mtd->writesize),
		       &regs->nandc.msr[1]);
	}

	/* address setup */
	if (row > 0)
		cmdq[0] = row;

	/* command setup */
	switch (priv->cmd) {
	case NAND_CMD_RESET:	/* 0xFF */
		cmdq[3] |= FTNANDC024_CMD_RESET << 8;
		cmdq[3] |= (priv->sel_ce) << 29;            /* command starting CE */
		ftnandc024_send_cmd(priv, cmdq);
		ftnandc024_wait(priv);
		break;

	case NAND_CMD_READID:	/* 0x90 */
		cmdq[3] |= FTNANDC024_CMD_RDID << 8;
		cmdq[3] |= (priv->sel_ce) << 29;            /* command starting CE */
		cmdq[3] |= 1 << 28;                         /* byte mode */
		cmdq[3] |= 7 << 19;                         /* spare data size = 8 */
		cmdq[2] |= (1 << 16|col);                         /* counter = 1 */
		priv->col = 0;
		ftnandc024_send_cmd(priv, cmdq);
		ftnandc024_wait(priv);
		break;

	case NAND_CMD_STATUS:	/* 0x70 */
		cmdq[3] |= FTNANDC024_CMD_RDST << 8;
		cmdq[3] |= (priv->sel_ce) << 29;            /* command starting CE */
		cmdq[2] |= 1 << 16;                         /* counter = 1 */
		ftnandc024_send_cmd(priv, cmdq);
		ftnandc024_wait(priv);
		break;

	case NAND_CMD_READOOB:	/* 0x50 */ 
		//printf("ftnandc024_cmdfunc NAND_CMD_READOOB \n");
		break;

	case NAND_CMD_READ0:	/* 0x00 */
		if (mtd->writesize > 512)
			cmdq[3] |= FTNANDC024_CMD_RDPG << 8;
		else        
			cmdq[3] |= FTNANDC024_CMD_RDPG_SP << 8;
		cmdq[3] |= (priv->sel_ce) << 29;            /* command starting CE */
		cmdq[3] |= (priv->oobavail - 1) << 19;      /* spare data size */
		cmdq[2] |= (mtd->writesize / 512) << 16;    /* sector count */
		PRINTK2("ftnandc024_cmdfunc: NAND_CMD_READ0 cmdq[0] = 0x%x, cmdq[1] = 0x%x, cmdq[2] = 0x%x, cmdq[3] = 0x%x\n", cmdq[0], cmdq[1], cmdq[2], cmdq[3]);
		ftnandc024_send_cmd(priv, cmdq);
		ftnandc024_wait(priv);

#ifdef CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE

#else
		priv->chk_data_0xFF = priv->chk_spare_0xFF = 0;
		priv->data_empty = priv->spare_empty = 0;
		
		if (priv->cmd_status & 0x10001) {
			PRINTK2("ftnandc024_cmdfunc: ECC check start\n");
			if (priv->cmd_status == 0x10001) {
				priv->chk_data_0xFF = priv->chk_spare_0xFF = 1;
				priv->data_empty = priv->spare_empty = 1;
			}
			if (priv->cmd_status == 0x1) {
				priv->chk_data_0xFF = priv->data_empty = 1;
			}
			if (priv->cmd_status == 0x10000) {
				priv->chk_spare_0xFF = priv->spare_empty = 1;
			}
		}
#endif //CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
		break;

	case NAND_CMD_SEQIN:	/* 0x80 (Write, Stage 1) */
		for(i = 0; i < priv->oobavail; i++) {
			if(*(chip->oob_poi + i) != 0xFF) {
				printf("ftnandc024_cmdfunc: cmd = 0x%x, col = 0x%x, row = 0x%x\n", cmd, col, row);
				printf("ftnandc024_cmdfunc: oob data != 0xFF, w_wo_spare = 0. need to write oob data\n");
				w_wo_spare = 0;
				break;
			}
		}
		
		if (mtd->writesize > 512) {
			if (col < mtd->writesize) {
				if (!w_wo_spare)
					cmdq[3] |= FTNANDC024_CMD_WRPG << 8;
				else
					cmdq[3] |= FTNANDC024_CMD_WRPG_NOOB << 8;
			}
			else{
				cmdq[3] |= FTNANDC024_CMD_RAWWR << 8;
				cmdq[2] |= mtd->writesize;
		                cmdq[3] |= 1 << 28;	/* byte mode */
			}
		} else {
			if (col < mtd->writesize)
				cmdq[3] |= FTNANDC024_CMD_WRPG_SP << 8;
			else
				cmdq[3] |= FTNANDC024_CMD_WROOB_SP << 8;
		}
		cmdq[3] |= (priv->sel_ce) << 29;            /* command starting CE */
		cmdq[3] |= (priv->oobavail - 1) << 19;      /* spare data size */
		if(col >= mtd->writesize)	// OOB WRITE, count 1
			cmdq[2] |= 1 << 16;                      /* counter = 1 */
		else 
			cmdq[2] |= (mtd->writesize / 512) << 16; /* sector count */
		PRINTK2("ftnandc024_cmdfunc: NAND_CMD_SEQIN cmdq[0] = 0x%x, cmdq[1] = 0x%x, cmdq[2] = 0x%x, cmdq[3] = 0x%x\n", cmdq[0], cmdq[1], cmdq[2], cmdq[3]);
		memcpy(priv->ssram, chip->oob_poi, priv->oobavail);
		ftnandc024_send_cmd(priv, cmdq);
		break;
	case NAND_CMD_PAGEPROG:	/* 0x10 (Write, Stage 2) */
		ftnandc024_wait(priv);
		break;

	case NAND_CMD_ERASE1:	/* 0x60 (Erase, Stage 1) */
		if (mtd->writesize > 512)
			cmdq[3] |= FTNANDC024_CMD_ERBK << 8;
		else
			cmdq[3] |= FTNANDC024_CMD_ERBK_SP << 8;
		cmdq[3] |= (priv->sel_ce) << 29;            /* command starting CE */
		cmdq[3] |= 1 << 2;                          /* single block scale */
		cmdq[2] |= 1 << 16;                         /* counter = 1 */
		PRINTK2("ftnandc024_cmdfunc: NAND_CMD_ERASE1 cmdq[0] = 0x%x, cmdq[1] = 0x%x, cmdq[2] = 0x%x, cmdq[3] = 0x%x\n", cmdq[0], cmdq[1], cmdq[2], cmdq[3]);
		ftnandc024_send_cmd(priv, cmdq);
		break;
	case NAND_CMD_ERASE2:	/* 0xD0 (Erase, Stage 2) */
		ftnandc024_wait(priv);
		break;

	default:
		PRINTK2("ftnandc024_cmdfunc: unhandled cmd=0x%x\n", cmd);
	}
}

/**
 * hardware specific access to control-lines
 * @mtd: MTD device structure
 * @cmd: command to device
 * @ctrl:
 * NAND_NCE: bit 0 -> don't care
 * NAND_CLE: bit 1 -> Command Latch
 * NAND_ALE: bit 2 -> Address Latch
 *
 * NOTE: boards may use different bits for these!!
 */
static void ftnandc024_hwcontrol(struct mtd_info *mtd,
	int cmd, unsigned int ctrl)
{
	/* nothing needs to be done */
}

static void ftnandc024_read_raw_id(struct nand_chip *chip)
{
	struct ftnandc024_chip *priv = chip->priv;
	uint32_t cmdq[6] = { 0 };
	u8 id_size = 5;
	
	// Issue the RESET cmd
	cmdq[0] = 0;
	cmdq[1] = 0;
	cmdq[2] = 0;
	cmdq[3] = FTNANDC024_CMD_RESET << 8;
	cmdq[3] |= (priv->sel_ce) << 29;
	
	ftnandc024_send_cmd(priv, cmdq);
	
	ftnandc024_wait(priv);
	
	// Issue the READID cmd
	cmdq[3] = FTNANDC024_CMD_RDID << 8;
	cmdq[3] |= (priv->sel_ce) << 29;    /* command starting CE */
	cmdq[3] |= 1 << 28;                 /* byte mode */
	cmdq[3] |= 7 << 19;                 /* spare data size = 8 */
	cmdq[2] = 1 << 16;                  /* counter = 1 */
	priv->col = 0;
	
	ftnandc024_send_cmd(priv, cmdq);
	
	ftnandc024_wait(priv);

	memcpy(priv->flash_raw_id, priv->ssram, id_size);

	PRINTK1("ID:0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", 
	        priv->flash_raw_id[0], priv->flash_raw_id[1],
	        priv->flash_raw_id[2], priv->flash_raw_id[3],
	        priv->flash_raw_id[4]);
}

/* The unit of Hclk is MHz, and the unit of Time is ns.
 * We desire to calculate N to satisfy N*(1/Hclk) > Time given Hclk and Time
 * ==> N > Time * Hclk
 * ==> N > Time * 10e(-9) * Hclk *10e(6)        --> take the order out
 * ==> N > Time * Hclk * 10e(-3)
 * ==> N > Time * Hclk / 1000
 * ==> N = (Time * Hclk + 999) / 1000
 */
static void ftnandc024_calc_timing(struct nand_chip *chip)
{
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
	int tWH, tWP, tREH, tRES, tBSY, tBUF1;
	int tBUF2, tBUF3, tBUF4, tPRE, tRLAT, t1;
	int tPST, tPSTH, tWRCK;
	struct ftnandc024_chip_timing *p;
	u32 CLK, timing[4];

	CLK = FREQ_SETTING / 1000000;
	
	tWH = tWP = tREH = tRES = 0;
	tRLAT = tBSY = t1 = 0;
	tBUF4 = tBUF3 = tBUF2 = tBUF1 = 0;
	tPRE = tPST = tPSTH = tWRCK = 0;

	p = &chip_timing;
	
	// tWH = max(tWH, tCH, tCLH, tALH)
	tWH = max_4(p->tWH, p->tCH, (int)p->tCLH, (int)p->tALH);
	tWH = (tWH * CLK) / 1000;
	// tWP = tWP
	tWP = (p->tWP * CLK) / 1000;
	// tREH = tREH
	tREH = (p->tREH * CLK) / 1000;
	// tRES = max(tREA, tRSTO, tREAID)
	tRES = max_3(p->tREA, p->tRSTO, (int)p->tREAID);
	tRES = (tRES * CLK) / 1000;
	// tRLAT < (tRES + tREH) + 2
	tRLAT = tRES + tREH;
	// t1 = max(tCS, tCLS, tALS) - tWP
	t1 = max_3(p->tCS, p->tCLS, (int)p->tALS) - p->tWP;
	if (t1 < 0)
		t1 = 0;
	else 
		t1 = (t1 * CLK) / 1000;
	// tPSTH(EBI setup time) = max(tCS, tCLS, tALS)
	tPSTH = max_3(p->tCS, p->tCLS, (int)p->tALS);
	tPSTH = (tPSTH * CLK) / 1000;
	// tWRCK(EBI hold time) = max(tRHZ, tREH)
	tWRCK = max_2(p->tRHZ, p->tREH);
	tWRCK = (tWRCK * CLK) / 1000;

	// tBSY = max(tWB, tRB), min value = 1
	tBSY = max_2(p->tWB, p->tRB);
	tBSY = (tBSY * CLK) / 1000;
	if(tBSY < 1)
		tBSY = 1;
	// tBUF1 = max(tADL, tCCS)
	tBUF1 = max_2(p->tADL, (int)p->tCCS);
	tBUF1 = (tBUF1 * CLK) / 1000;
	// tBUF2 = max(tAR, tRR, tCLR, tCDQSS, tCRES, tCALS, tCALS2, tDBS)
	tBUF2 = max_2(max_4(p->tAR, p->tRR, (int)p->tCLR, (int)p->tCDQSS),
				max_4((int)p->tCRES, (int)p->tCALS, (int)p->tCALS2, (int)p->tDBS));
	tBUF2 = (tBUF2 * CLK) / 1000;
	// tBUF3 = max(tRHW, tRHZ, tDQSHZ)
	tBUF3 = max_3(p->tRHW, p->tRHZ, (int)p->tDQSHZ);
	tBUF3 = (tBUF3 * CLK) / 1000;
	// tBUF4 = max(tWHR, tWHR2)
	tBUF4 = max_2((int)p->tWHR, p->tWHR2);
	tBUF4 = (tBUF4 * CLK) / 1000;

	timing[0] = (tWH << 24) | (tWP << 16) | (tREH << 8) | tRES;
	timing[1] = (tRLAT << 16) | (tBSY << 8) | t1;
	timing[2] = (tBUF4 << 24) | (tBUF3 << 16) | (tBUF2 << 8) | tBUF1;
	timing[3] = (tPRE << 28) | (tPST << 24) | (tPSTH << 16) | tWRCK;

	writel(timing[0], &regs->nandc.atrl[0]);
	writel(timing[1], &regs->nandc.atrl[1]);
	writel(timing[2], &regs->nandc.atrh[0]);
	writel(timing[3], &regs->nandc.atrh[1]);
}

static void ftnandc024_calibrate_rlat(struct nand_chip *chip)
{
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
	uint32_t cmdq[6] = { 0 };
	int i, max_rlat;
	int id_size = 5;
	u8 *p, *golden_p;
	u8 rlat_lower_bound, rlat_upper_bound, state;
	u32 ac_reg0, ac_reg1, val;

	rlat_lower_bound = rlat_upper_bound = 0;
	p = kmalloc(id_size, GFP_KERNEL);
	golden_p = kmalloc(id_size, GFP_KERNEL);
	
	for(i = 0; i< id_size; i++)
		*(golden_p + i) = *(priv->flash_raw_id + i);

	ac_reg0 = readl(&regs->nandc.atrl[0]);
	max_rlat = (ac_reg0&0x1F) + ((ac_reg0>>8)&0xF);
	ac_reg1 = readl(&regs->nandc.atrl[1]);
	state = 0;
	for(i = 0; i <= max_rlat; i++) {
		// setting the trlat delay before READID.
		val = (ac_reg1 & ~(0x3F<<16)) | (i<<16);
		writel(val, &regs->nandc.atrl[1]);
		memset(p, 0, id_size);
		
		// Issuing the READID
		cmdq[3] = FTNANDC024_CMD_RDID << 8;
		cmdq[3] |= (priv->sel_ce) << 29;    /* command starting CE */
		cmdq[3] |= 1 << 28;                 /* byte mode */
		cmdq[3] |= 7 << 19;                 /* spare data size = 8 */
		cmdq[2] = 1 << 16;                  /* counter = 1 */
		priv->col = 0;
	
		ftnandc024_send_cmd(priv, cmdq);
	
		ftnandc024_wait(priv);

		memcpy(p, priv->ssram, id_size);
		if(state == 0 && memcmp(golden_p, p, id_size) == 0) {
			rlat_lower_bound = i;
			state = 1;
		}
		else if(state == 1 && memcmp(golden_p, p, id_size) != 0){
			rlat_upper_bound = i - 1;
			break;
		}

		PRINTK1("===============================================\n");
		PRINTK1("ID       :0x%x 0x%x 0x%x 0x%x 0x%x\n", 
		        *p, *(p+1), *(p+2), *(p+3), *(p+4));
		PRINTK1("Golden ID:0x%x 0x%x 0x%x 0x%x 0x%x\n", 
		        *golden_p, *(golden_p+1), *(golden_p+2), *(golden_p+3), *(golden_p+4));
		PRINTK1("===============================================\n");
	}
	// Prevent the dqs_upper_bound is zero when ID still accuracy on the max dqs delay
	if(i == max_rlat + 1)
		rlat_upper_bound = max_rlat;
	
	PRINTK1("Upper:%d & Lower:%d for tRLAT, then Middle:%d\n", 
	        rlat_upper_bound, rlat_lower_bound, ((rlat_upper_bound + rlat_lower_bound) >> 1));
	// Setting the middle tRLAT
	val = ac_reg1&~(0x3F<<16);
	val |= ((((rlat_upper_bound + rlat_lower_bound) >> 1) & 0x3F) << 16);
	writel(val, &regs->nandc.atrl[1]);

	kfree(p);
	kfree(golden_p);
}

static int ftnandc024_reset(struct nand_chip *chip)
{
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
#ifdef CONFIG_SYS_FTNANDC024_TIMING
	uint32_t timing[] = CONFIG_SYS_FTNANDC024_TIMING;

	/* AC timing setup */
	writel(timing[0], &regs->nandc.atrl[0]);
	writel(timing[1], &regs->nandc.atrl[1]);
	writel(timing[2], &regs->nandc.atrh[0]);
	writel(timing[3], &regs->nandc.atrh[1]);
#else
	ftnandc024_read_raw_id(chip);
	ftnandc024_calc_timing(chip);
	ftnandc024_calibrate_rlat(chip);
#endif

	if (chip->options & NAND_BUSWIDTH_16) {
		printf("ftnandc024_reset: The controller doesn't support 16-bits mode\n");
		return -EINVAL;
	}

	/* chip reset */
	writel(1, &regs->misc.gsrr);
	while(readl(&regs->misc.gsrr) & 1)
		;

	/* AHB slave setup */
	writel(FTNANDC024_MISC_SMSR_32K
		|	FTNANDC024_MISC_SMSR_RETRY
		|	FTNANDC024_MISC_SMSR_PREFETCH_FORCED
		|	FTNANDC024_MISC_SMSR_PREFETCH_SIZE(128),
		&regs->misc.smsr);

	/* General nand flash status bits */
	writel(FTNANDC024_NANDC_GSR_RDYBIT(ffs(NAND_STATUS_READY) - 1)
		|	FTNANDC024_NANDC_GSR_ERRBIT(ffs(NAND_STATUS_FAIL) - 1)
#ifdef CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
		|	FTNANDC024_NANDC_GSR_DATI
#endif //CONFIG_NAND_FTNANDC024_USE_DATA_INVERSE
#ifdef CFG_USE_SCRAMBLER
		|	FTNANDC024_NANDC_GSR_DATS
#endif
		,	&regs->nandc.gsr);

	/* Reset memory setting */
	writel(0, &regs->nandc.msr[0]);
	writel(0, &regs->nandc.msr[1]);

	/* Hardware ECC setting */
	writel(1, &regs->ecc.cr);	/* no blocking */
	writel(priv->plat_nand->ecc_bits_dat - 1, &regs->ecc.cap[0]);
	writel(priv->plat_nand->ecc_bits_oob - 1, &regs->ecc.srcr[2]);

#ifdef CFG_NO_HWECC
#ifdef CFG_ECC_NOPARITY
	setbits_le32(&regs->ecc.cr, 1 << 17);
#endif //CFG_ECC_NOPARITY
#else
	setbits_le32(&regs->ecc.cr, 1 << 8);
#endif //CFG_NO_HWECC

	chip->ecc.layout         = &nand_lp_ecclayout;
	chip->ecc.size           = 512;
	chip->ecc.bytes          = ftnandc024_ecc_length_convert(priv->plat_nand->ecc_bits_dat);	/* 3 bits ECC */
	chip->ecc.steps          = 4;
	chip->ecc.read_page      = ftnandc024_read_page;
	chip->ecc.write_page     = ftnandc024_write_page;
	chip->ecc.read_page_raw  = ftnandc024_read_page_raw;
	chip->ecc.write_page_raw = ftnandc024_write_page_raw;
	chip->ecc.mode           = NAND_ECC_HW;
	chip->ecc.strength       = priv->plat_nand->ecc_bits_oob;

	return 0;
}

int ftnandc024_dump_page(struct mtd_info *mtd, int page)
{
	struct nand_chip *chip = mtd->priv;
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
	int i, total_page_size;
	uint8_t *raw;

	total_page_size = mtd->writesize + mtd->oobsize;

	raw = calloc(1, total_page_size);

	memset(raw, 0xff, total_page_size);

	/* flash setup */
	if (mtd->erasesize && mtd->writesize) {
		writel(FTNANDC024_NANDC_MSR0_BPP(mtd->writesize) |
		       FTNANDC024_NANDC_MSR0_PPB(mtd->erasesize / mtd->writesize) |
		       FTNANDC024_NANDC_MSR0_RAC(priv->plat_nand->row_addrcycles) |
		       FTNANDC024_NANDC_MSR0_CAC(priv->plat_nand->col_addrcycles) |
		       FTNANDC024_NANDC_MSR0_BIPP(priv->bi_reserve),
		       &regs->nandc.msr[0]);
		writel(FTNANDC024_NANDC_MSR1_VPPB(mtd->erasesize / mtd->writesize),
		       &regs->nandc.msr[1]);
	}

	for (i = 0; i < total_page_size; i+=32)
	{
		ftnandc024_read_byte_manual(mtd, raw + i, page, i, 32);
	}

	printf("\n");
	for (i = 0; i < total_page_size; i++)
	{
		if (i != 0 && i%16 == 0)
			printf("\n");

		if (i%16 == 0)
			printf("0x%x	", i);

		printf("%02x ", *(raw+i));
	}
	printf("\n");

	free(raw);

	return 0;
}

int ftnandc024_init(struct nand_chip *chip, uint32_t iobase, uint32_t database, int sel_ce, struct platform_nand_flash *plat_nand, uint8_t bi_reserve)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct ftnandc024_chip *priv;

	mtd->priv = chip;
	priv = calloc(1, sizeof(struct ftnandc024_chip));
	if (!priv)
		return -ENOMEM;

	priv->mtd        = mtd;
	priv->regs       = (void __iomem *)iobase;
	priv->ssram      = (void __iomem *)iobase + 0x1000;
	priv->region     = (void __iomem *)database;
	priv->bi_reserve = bi_reserve;
	priv->sel_ce     = sel_ce;
	priv->plat_nand  = plat_nand;
	chip->priv       = priv;

	/* hwcontrol is mandatory */
	chip->cmd_ctrl   = ftnandc024_hwcontrol;
	chip->cmdfunc    = ftnandc024_cmdfunc;
	chip->dev_ready  = ftnandc024_dev_ready;
	chip->chip_delay = 0;

	chip->read_byte  = ftnandc024_read_byte;
	chip->read_word  = ftnandc024_read_word;
	chip->read_buf   = ftnandc024_read_buf;
	chip->write_buf  = ftnandc024_write_buf;

	/* hardware reset */
	if (ftnandc024_reset(chip))
		return -EINVAL;

	/* Detect NAND chips */
	if (nand_scan_ident(mtd, CONFIG_SYS_MAX_NAND_DEVICE, NULL))
		return -ENXIO;

	if (nand_scan_tail(mtd))
		return -ENXIO;

	if (mtd->writesize > 512) {
		chip->ecc.layout = &nand_lp_ecclayout;
		chip->ecc.steps = mtd->writesize / chip->ecc.size;
	}	else {
		chip->ecc.layout = &nand_sp_ecclayout;
		chip->ecc.steps = 1;
	}

//	ftnandc024_dump_page(mtd, 0);

	return 0;
}

int ftnandc024_format(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	uint32_t ppb = mtd->erasesize / mtd->writesize;
	uint32_t blk = 0;
	uint64_t off = 0;

	printf(" Page size %8d b\n", mtd->writesize);
	printf(" OOB size %8d b\n", mtd->oobsize);
	printf(" Erase size %8d b\n", mtd->erasesize);

	ftnandc024_cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	while (off < mtd->size) {
		printf("Formatting block ......%8d\n", blk);

		/* backup the factory default bad block info */
		ftnandc024_read_oob_raw(mtd, chip->oob_poi, blk * ppb, 0);

		/* erase the block */
		ftnandc024_cmdfunc(mtd, NAND_CMD_ERASE1, 0, blk * ppb);
		ftnandc024_cmdfunc(mtd, NAND_CMD_ERASE2, 0, blk * ppb);
		while (!ftnandc024_dev_ready(mtd))
			;

		/* mark it as bad block if necessary */
		if ((chip->oob_poi[0] &	chip->oob_poi[1] & chip->oob_poi[5]) != 0xff) {
			ftnandc024_cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, blk * ppb);
			ftnandc024_cmdfunc(mtd, NAND_CMD_PAGEPROG, mtd->writesize, blk * ppb);
			while (!ftnandc024_dev_ready(mtd))
				;
		}

		blk += 1;
		off += mtd->erasesize;
	}

	return 0;
}

void ftnandc024_hwecc_enable(struct nand_chip	*chip, int enable)
{
	struct ftnandc024_chip *priv = chip->priv;
	struct ftnandc024_regs *regs = priv->regs;
	int hwecc_status;

	if (enable) {
		hwecc_status = readl(&regs->ecc.cr);
		
		hwecc_status &= ~(1 << 17);
		hwecc_status |= (1 << 8);
		
		writel(hwecc_status, &regs->ecc.cr);
	} else {
		hwecc_status = readl(&regs->ecc.cr);
		
		hwecc_status |= (1 << 17);
		hwecc_status &= ~(1 << 8);
		
		writel(hwecc_status, &regs->ecc.cr);
	}
}

static int ftnandc024_probe(struct udevice *dev)
{
	unsigned int base, data_base, sel_ce, bi_reserve;
	int devnum = 0;
	struct mtd_info *mtd;

	mtd = nand_to_mtd(&ftnand024_chip);

	PRINTK1("****************%s\n", __func__);

	base = dev_read_addr(dev);
	if (!base) {
		debug("ftnandc024_probe: Can't get base address\n");
		return -EINVAL;
	}

	data_base = fdtdec_get_addr(gd->fdt_blob, dev_of_offset(dev), "data_reg");
	if (!base) {
		debug("ftnandc024_probe: Can't get base address\n");
		return -EINVAL;
	}

	sel_ce = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev), "sel_ce", 0);

#ifdef CONFIG_NAND_FTNANDC024_USE_STRAP_PIN
	get_nandc_info_strap(&ftnand024_chip, &platform_nand);
#else
	ftnand024_chip.page_shift = 11;	/* 2048 */

	ftnand024_chip.phys_erase_shift = ftnand024_chip.page_shift + 6; //block = 64 pages per block

	platform_nand.row_addrcycles =  fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev), "rac", 0);

	platform_nand.col_addrcycles = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev), "cac", 0);

	platform_nand.ecc_bits_dat =  fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev), "ecc_bits_dat", 0);

	platform_nand.ecc_bits_oob = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev), "ecc_bits_oob", 0);
#endif

	bi_reserve = fdtdec_get_int(gd->fdt_blob, dev_of_offset(dev), "bi_reserve", 0);

	if (ftnandc024_init(&ftnand024_chip, base, data_base, sel_ce, &platform_nand, bi_reserve))
		goto bni_err;

	if (nand_scan(mtd, CONFIG_SYS_NAND_MAX_CHIPS))
		goto bni_err;

	if (mtd->writesize && ((1 << ftnand024_chip.page_shift) != mtd->writesize)) {
		printf("ftnandc024: page size is supposed to be %d, not %d\n",
			mtd->writesize, 1 << ftnand024_chip.page_shift);
	}

	if (mtd->erasesize && ((1 << ftnand024_chip.phys_erase_shift) != mtd->erasesize)) {
		printf("ftnandc024: block size is supposed to be %d, not %d\n",
			mtd->erasesize, 1 << ftnand024_chip.phys_erase_shift);
	}

#if defined(CONFIG_BLK) && defined(CONFIG_MTD_BLOCK)
	mtd_blk_probe(dev, mtd);
#endif

	return nand_register(devnum,mtd);

bni_err:
	free(mtd->priv);
	return -EINVAL;
}

static const struct udevice_id ftnandc024_ids[] = {
	{ .compatible = "ftnandc024"},
	{ }
};

U_BOOT_DRIVER(ftnandc024) = {
	.name       = "ftnandc024",
	.id         = UCLASS_MTD,
	.of_match   = ftnandc024_ids,
	.probe      = ftnandc024_probe,
};
