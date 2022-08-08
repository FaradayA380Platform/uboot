/*
 * board/faraday/hgu10g/board.c
 *
 * (C) Copyright 2015 Faraday Technology
 * B.C. Chen <bcchen@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#include <common.h>
#include <dm.h>
#include <spi.h>
#include <nand.h>
#include <netdev.h>
#include <ns16550.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <faraday/ftnandc024.h>
//#include <faraday/nand.h>

DECLARE_GLOBAL_DATA_PTR;

/* System Control Uint (pinmux) */
static void pinmux_init(void)
{
	/* These pins are floating in Faraday HGU10G ASIC platform. */
#if 1
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 11) & 0x4) {
	case 2: /* SPI Boot */
	case 3:
		/* FTNANDC024: off + FTSPI020: on */
		clrbits_le32(PLATFORM_SYSC_BASE + 0x45C, 0x00FFF000);
		setbits_le32(PLATFORM_SYSC_BASE + 0x45C, 0x00555000);
		break;
	case 1: /* NAND Boot */
		/* FTNANDC024: on + FTSPI020: off */
		clrbits_le32(PLATFORM_SYSC_BASE + 0x458, 0xF0000000);
		clrbits_le32(PLATFORM_SYSC_BASE + 0x45C, 0x00FFFFFF);
	default:
		break;
	}

	/* FTSDC021, disable write protect */
	clrbits_le32(PLATFORM_SYSC_BASE + 0x460, 0x0FFFF000);
	setbits_le32(PLATFORM_SYSC_BASE + 0x460, 0x01555000);
#endif
	//i2c
	clrbits_le32(PLATFORM_SYSC_BASE + 0x450, 0x000f0000);

	//ssp010
	clrbits_le32(PLATFORM_SYSC_BASE + 0x454, 0x0000003f);
	clrbits_le32(PLATFORM_SYSC_BASE + 0x450, 0x0f000000);
	
}

static void usb_init(void)
{
	u32 temp;

	// OTG PLL CORECLKIN
	temp = readl(PLATFORM_SYSC_BASE + 0x47C);
	temp = temp | 0x13;
	writel(temp, PLATFORM_SYSC_BASE + 0x47C);

	// OTG PLL divider reset deassert
	temp = readl(PLATFORM_SYSC_BASE + 0x47C);
	temp = temp | 0x4;
	writel(temp, PLATFORM_SYSC_BASE + 0x47C);

	// OTG PHY PONRST deassert
	temp = readl(PLATFORM_SYSC_BASE + 0x448);
	temp = temp | 0x1;
	writel(temp, PLATFORM_SYSC_BASE + 0x448);

	// OTG CTRL RESET deassert
	temp = readl(PLATFORM_SYSC_BASE + 0x448);
	temp = temp | 0x80;
	writel(temp, PLATFORM_SYSC_BASE + 0x448);

	// OTG_1 PHY PONRST deassert
	temp = readl(PLATFORM_SYSC_BASE + 0x44C);
	temp = temp | 0x1;
	writel(temp, PLATFORM_SYSC_BASE + 0x44C);

	// OTG_1 CTRL RESET deassert
	temp = readl(PLATFORM_SYSC_BASE + 0x44C);
	temp = temp | 0x80;
	writel(temp, PLATFORM_SYSC_BASE + 0x44C);
}

/*
 * Miscellaneous platform dependent initialisations
 */
int board_early_init_f(void)
{
	pinmux_init();
	clock_init();
	usb_init();
	return 0;
}

#ifdef CONFIG_SPL_BUILD
void board_init_f(ulong dummy)
{
	board_early_init_f();
}
#endif

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_FARADAY;
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
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

#ifdef CONFIG_SYS_NAND_SELF_INIT
#define CFG_START_CE            0
#define CFG_BI_RESERVE_SMALL    6
#define CFG_BI_RESERVE_LARGE    1

static struct platform_nand_flash platform_nand;

void get_nandc_info_strap(struct nand_chip *chip, struct platform_nand_flash *plat_nand)
{
	/* These pins are floating in Faraday HGU10G ASIC platform. */
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 13) & 0x3) {
		case 0: /* NAND 512 */
			chip->page_shift = 9;   /* 512 bytes per page */
			break;
		case 2: /* NAND 4K */
			chip->page_shift = 12;  /* 4096 bytes per page */
			break;
		case 3: /* NAND 8K */
			chip->page_shift = 13;  /* 8192 bytes per page */
			break;
		case 1: /* NAND 2K */
		default:
			chip->page_shift = 11;  /* 2048 bytes per page */
			break;
	}
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 16) & 0x3) {
		case 0:
			chip->phys_erase_shift = chip->page_shift + 5;  /* 32 pages per block */
			break;
		case 2:
			chip->phys_erase_shift = chip->page_shift + 7;  /* 128 pages per block */
			break;
		case 3:
			chip->phys_erase_shift = chip->page_shift + 8;  /* 256 pages per block */
			break;
		case 1:
		default:
			chip->phys_erase_shift = chip->page_shift + 6;  /* 64 pages per block */
			break;
	}
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 15) & 0x1) {
		case 0:
			plat_nand->row_addrcycles = 3;    /* 3 row address cycle */
			break;
		case 1:
		default:
			plat_nand->row_addrcycles = 2;    /* 2 row address cycle */
			break;
	}

	if (chip->page_shift == 9) {
		switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 18) & 0x3) {
			case 0:
				plat_nand->ecc_bits_dat = 1;    /* 1 bit ecc */
				break;
			case 1:
			default:
				plat_nand->ecc_bits_dat = 2;    /* 2 bit ecc */
				break;	 
		}
		plat_nand->ecc_bits_oob = 1;//according to signoff form. same as rom code
	} else {
		switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 18) & 0x3) {
			case 0:
				plat_nand->ecc_bits_dat = 2;    /* 2 bit ecc */
				break;
			case 2:
				plat_nand->ecc_bits_dat = 8;    /* 8 bit ecc */
				break;
			case 3:
				plat_nand->ecc_bits_dat = 12;    /* 12 bit ecc */
				break;
			case 1:
			default:
				plat_nand->ecc_bits_dat = 4;    /* 4 bit ecc */
				break;
		}
		plat_nand->ecc_bits_oob = 2;//according to signoff form. same as rom code
	}
}

void board_nand_init(void)
{
	int devnum = 0, rac;
	struct mtd_info *mtd = &nand_info[devnum];
	struct nand_chip *chip = calloc(1, sizeof(struct nand_chip));

	if (!chip)
		return;

	mtd->priv = chip;
	if (!mtd->priv)
		return;

#ifdef CONFIG_NAND_FTNANDC024_USE_STRAP_PIN
	get_nandc_info_strap(chip, &platform_nand);
#else
	chip->page_shift = 11;  /* 2048 bytes per page */
	chip->phys_erase_shift = chip->page_shift + 6;  //block = 64 page
	platform_nand.row_addrcycles = 3;    /* 3 row address cycle for large page */
#endif

	if (chip->page_shift == 9) {
#ifndef CONFIG_NAND_FTNANDC024_USE_STRAP_PIN
		platform_nand.col_addrcycles = 1;
		platform_nand.ecc_bits_dat = 2;
		platform_nand.ecc_bits_oob = 1;//according to signoff form. same as rom code
#endif
		if (ftnandc024_init(mtd->priv, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_SMALL))
			goto bni_err;
	} else {
#ifndef CONFIG_NAND_FTNANDC024_USE_STRAP_PIN
		platform_nand.col_addrcycles = 2;
		platform_nand.ecc_bits_dat = 4;
		platform_nand.ecc_bits_oob = 2;//according to signoff form. same as rom code
#endif
		if (ftnandc024_init(mtd->priv, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_LARGE))
			goto bni_err;
	}

	mtd = nand_to_mtd(chip);

	if (mtd->writesize && ((1 << chip->page_shift) != mtd->writesize)) {
		printf("ftnandc024: page size is supposed to be %d, not %d\n",
			mtd->writesize, 1 << chip->page_shift);
	}

	if (mtd->erasesize && ((1 << chip->phys_erase_shift) != mtd->erasesize)) {
		printf("ftnandc024: block size is supposed to be %d, not %d\n",
			mtd->erasesize, 1 << chip->phys_erase_shift);
	}

	nand_register(devnum, mtd);

	return;

bni_err:
	free(mtd->priv);
}
#endif /* #ifdef CONFIG_SYS_NAND_SELF_INIT */

int board_eth_init(struct bd_info *bd)
{
	int ret = 0;
#ifdef CONFIG_USB_ETHER
	ret = usb_eth_initialize(bd);
#endif
	return ret;
}

static const struct ns16550_plat fa_serial_plat= {
	.base = 0x04100000,
	.reg_shift = 2,
	.clock = 31250000,
	.fcr = UART_FCR_DEFVAL,
};

U_BOOT_DRVINFO(fa_serial) = {
	"ns16550_serial",&fa_serial_plat,
};
