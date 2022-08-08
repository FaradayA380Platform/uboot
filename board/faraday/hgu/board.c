/*
 * board/faraday/hgu/board.c
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#include <common.h>
#include <linux/errno.h>
#include <asm/io.h>
#include <spi.h>
#include <nand.h>
#include <netdev.h>
#include <malloc.h>

#include <faraday/ftnandc024.h>

DECLARE_GLOBAL_DATA_PTR;

/* System Control Uint (pinmux) */
static void pinmux_init(void)
{
#if 0 /* These pins are floating in Faraday HGU ASIC platform. */
	switch ((readl(SYSC_BASE + 0x18) >> 11) & 7) {
	case 1: /* SPI */
		/* FTSPI020: on + FTNANDC024: off */
		clrbits_le32(SYSC_BASE + 0x448, 0xf0000000);
		clrbits_le32(SYSC_BASE + 0x44C, 0xf);
		setbits_le32(SYSC_BASE + 0x448, (1 << 30) | (1 << 28));
		setbits_le32(SYSC_BASE + 0x44C, (1 << 2) | (1 << 0));
		break;
	case 2: /* NAND 512 */
	case 3: /* NAND 2K */
	case 4: /* NAND 4K */
	case 5: /* NAND 8K */
		/* FTSPI020: off + FTNANDC024: on */
		clrbits_le32(SYSC_BASE + 0x448, 0xff000000);
		clrbits_le32(SYSC_BASE + 0x44C, 0xfff);
		break;
	default:
		break;
	}
#elif 0
	/* FTSPI020: on + FTNANDC024: off */
	clrbits_le32(SYSC_BASE + 0x448, 0xf0000000);
	clrbits_le32(SYSC_BASE + 0x44C, 0xf);
	setbits_le32(SYSC_BASE + 0x448, (1 << 30) | (1 << 28));
	setbits_le32(SYSC_BASE + 0x44C, (1 << 0) | (1 << 2));
#else
	/* FTSPI020: off + FTNANDC024: on */
	clrbits_le32(SYSC_BASE + 0x448, 0xff000000);
	clrbits_le32(SYSC_BASE + 0x44C, 0xfff);
#endif

	/* FTNANDC024 driving */
	//setbits_le32(SYSC_BASE + 0x400, (1 << 29));
	/* FTSDC021, disable write protect */
	setbits_le32(SYSC_BASE + 0x444, (1 << 2));
	/* Enable FTSDC021, disable MDIO & DYING-IN/OUT */
	clrbits_le32(SYSC_BASE + 0x450,
		0xfff);
	setbits_le32(SYSC_BASE + 0x450,
		(1 << 0) | (1 << 2) | (1 << 4) | (1 << 6) | (1 << 8) | (1 << 10));

	/* FOTG210 Gadget */
	setbits_le32(SYSC_BASE + 0x520,
		(1 << 16));
}
void lowlevel_init()
{
}
/*
 * Miscellaneous platform dependent initialisations
 */
int board_early_init_f(void)
{
	pinmux_init();
	clock_init();
	return 0;
}
void board_usb_init(void)
{
	u32 regs = SYSC_BASE;
	u32 val;
	val = readl(regs + 0x520);
	val &= (~(0x1E01980C));
	val |= (0x1E019804);    //usb1 : host ; usb0 : dev ;
	writel(val, regs + 0x520);

	val = readl(regs + 0x524);
	val &= (~(0x1E019800));
	val |= (0x1E019800);
	writel(val, regs + 0x524);	
}
int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_FARADAY;
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
	board_usb_init();
	return 0;
}

int dram_init(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_SDRAM_SIZE;

	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	return 0;
}

int board_mmc_init(struct bd_info *bis)
{
#ifdef CONFIG_FTSDC021
	return ftsdc021_sdhci_init(CONFIG_FTSDC021_BASE);
#else
	return 0;
#endif
}
#if 0
#ifdef CONFIG_SYS_NAND_SELF_INIT
#define CFG_START_CE		0
#define CFG_BI_RESERVE_SMALL	0
#define CFG_BI_RESERVE_LARGE	0
void board_nand_init(void)
{
	int devnum = 0;
	struct mtd_info *mtd = &nand_info[devnum];
	struct nand_chip *chip = calloc(1, sizeof(struct nand_chip));

	if (!chip)
		return;

	mtd->priv = chip;
	if (!mtd->priv)
		return;

#if 0 /* These pins are floating in Faraday HGU ASIC platform. */
	switch ((readl(SYSC_BASE + 0x18) >> 11) & 7) {
	case 2: /* NAND 512 */
		chip->page_shift = 9;	/* 512 */
		break;
	case 3: /* NAND 2K */
		chip->page_shift = 11;	/* 2048 */
		break;
	case 4: /* NAND 4K */
		chip->page_shift = 12;	/* 4096 */
		break;
	case 5: /* NAND 8K */
		chip->page_shift = 13;	/* 8192 */
		break;
	default:
		break;
	}
#else
	chip->page_shift = 11;	/* 2048 */
#endif
	/* block = 32 pages: see BootROM HGU_signoff.xlsx */
	//chip->phys_erase_shift = chip->page_shift + 5;
	chip->phys_erase_shift = chip->page_shift + 6; //block = 64 page
	if (chip->page_shift == 9) {
		if (ftnandc024_init(mtd->priv, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, 3, 1, CFG_BI_RESERVE_SMALL))
			goto bni_err;
	} else {
		if (ftnandc024_init(mtd->priv, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, 3, 2, CFG_BI_RESERVE_LARGE))
		//if (ftnandc024_init(mtd->priv, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, 2, 2, CFG_BI_RESERVE_LARGE))
			goto bni_err;
	}

	if (nand_scan(mtd, CONFIG_SYS_NAND_MAX_CHIPS))
		goto bni_err;

	if (mtd->writesize && ((1 << chip->page_shift) != mtd->writesize)) {
		printf("ftnandc024: page size is supposed to be %d, not %d\n",
			mtd->writesize, 1 << chip->page_shift);
	}

	if (mtd->erasesize && ((1 << chip->phys_erase_shift) != mtd->erasesize)) {
		printf("ftnandc024: block size is supposed to be %d, not %d\n",
			mtd->erasesize, 1 << chip->phys_erase_shift);
	}

	nand_register(devnum);

	return;

bni_err:
	free(mtd->priv);
}
#endif /* #ifdef CONFIG_SYS_NAND_SELF_INIT */
#endif
int board_eth_init(struct bd_info *bd)
{
	int ret = 0;
#ifdef CONFIG_USB_ETHER
	ret = usb_eth_initialize(bd);
#endif
	return ret;
}
