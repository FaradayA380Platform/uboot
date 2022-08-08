/*
 * board/faraday/wideband001/board.c
 *
 * (C) Copyright 2017 Faraday Technology
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
#include <faraday/nand.h>

DECLARE_GLOBAL_DATA_PTR;

/* System Control Uint (pinmux) */
static void pinmux_init(void)
{
	/* These pins are floating in Faraday WIDEBAND001 ASIC platform. */
#if 1
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 8) & 3) {
	case 2: /* SPI Boot */
	case 3:
	case 0:
		/* FTNANDC024: off + FTSPI020: on */
		clrbits_le32(PLATFORM_SYSC_BASE + 0x858, 0x000FFF00);
		setbits_le32(PLATFORM_SYSC_BASE + 0x858, 0x00055500);
		break;
	case 1: /* NAND Boot */
	default:
		/* FTNANDC024: on + FTSPI020: off */
		clrbits_le32(PLATFORM_SYSC_BASE + 0x854, 0xFFC00000);
		clrbits_le32(PLATFORM_SYSC_BASE + 0x858, 0x000FFFFF);
		break;
	}
#elif 0
	/* FTNANDC024: off + FTSPI020: on */
	clrbits_le32(PLATFORM_SYSC_BASE + 0x858, 0x000FFF00);
	setbits_le32(PLATFORM_SYSC_BASE + 0x858, 0x00055500);
#else
	/* FTNANDC024: on + FTSPI020: off */
	clrbits_le32(PLATFORM_SYSC_BASE + 0x854, 0xFFC00000);
	clrbits_le32(PLATFORM_SYSC_BASE + 0x858, 0x000FFFFF);
#endif

	/* FTSDC021: on + enable write protect */
	clrbits_le32(PLATFORM_SYSC_BASE + 0x854, 0x003FFFC0);
}

static void addr_filtering(void)
{
   uint32_t addr=0x3a000000;
   //enable address filtering (bit 1)
   writel(0x2, 0x3a000000);
   //fill start/end address
   writel(0x80000000,0x3a000040);
   //fill end address
   writel(0xa0000000,0x3a000044);

}

/*
 * Miscellaneous platform dependent initialisations
 */
int board_early_init_f(void)
{
	pinmux_init();
	clock_init();
  //address filtering 
  addr_filtering();
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
	invalidate_icache_all();
	invalidate_dcache_all();

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

int board_mmc_init(bd_t *bis)
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

static struct nand_chip nand_chip;
static struct platform_nand_flash platform_nand;

void get_nandc_info_strap(struct nand_chip *chip, struct platform_nand_flash *plat_nand)
{
	/* These pins are floating in Faraday WIDEBAND001 ASIC platform. */
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 10) & 0x3) {
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
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 12) & 0x3) {
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
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 16) & 0x1) {
		case 0:
			plat_nand->row_addrcycles = 3;    /* 3 row address cycle */
			break;
		case 1:
		default:
			plat_nand->row_addrcycles = 2;    /* 2 row address cycle */
			break;
	}

	if (chip->page_shift == 9) {
		plat_nand->col_addrcycles = 1;
		switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 14) & 0x3) {
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
		plat_nand->col_addrcycles = 2;
		switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 14) & 0x3) {
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
		plat_nand->ecc_bits_oob = 1;//according to signoff form. same as rom code
	}
}

void board_nand_init(void)
{
#ifndef CONFIG_DM_NAND
	int devnum = 0;
	struct mtd_info *mtd;
	struct nand_chip *chip = calloc(1, sizeof(struct nand_chip));

	if (!chip)
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
		if (ftnandc024_init(chip, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_SMALL))
			goto bni_err;
	} else {
#ifndef CONFIG_NAND_FTNANDC024_USE_STRAP_PIN
		platform_nand.col_addrcycles = 2;
		platform_nand.ecc_bits_dat = 4;
		platform_nand.ecc_bits_oob = 1;//according to signoff form. same as rom code
#endif
		if (ftnandc024_init(chip, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_LARGE))
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
#endif
}
#else
int board_nand_init(struct nand_chip *nand)
{
	return 1;
}
#endif /* #ifdef CONFIG_SYS_NAND_SELF_INIT */

int board_eth_init(bd_t *bd)
{
	int ret = 0;
#ifdef CONFIG_USB_ETHER
	ret = usb_eth_initialize(bd);
#endif
	return ret;
}


#ifndef WDT_REG32
#define WDT_REG32(off)      *(volatile uint32_t __force*)(0x3a000600 + (off))
#endif

void platform_restart(int mode, const char *cmd)
{
  printf("sys reboot!\n");
  WDT_REG32(0x34) = 0x12345678;
  WDT_REG32(0x34) = 0x87654321;
  WDT_REG32(0x28) = 0;
  WDT_REG32(0x2c) = 1;
  WDT_REG32(0x30) = 1;
  WDT_REG32(0x20) = 0x20;
  WDT_REG32(0x28) = 9;
}

void sysrst(int mode)
{
    platform_restart(0,NULL);
}

#ifdef CONFIG_USB_GADGET_FUSB300
#include <linux/errno.h>
#include <linux/types.h>
#include <usb.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <usb/fusb300_udc.h>
#define PLAT_FUSB300_BASE 0x30000000
static struct fusb300 fusb300_controller = {
	.reg = (void __iomem *)PLAT_FUSB300_BASE
};

extern int __init fusb300_probe(struct fusb300 *fusb300);
int board_usb_init(int index, enum usb_init_type init)
{
    struct fusb300 *fusb300 = &fusb300_controller;

//release reset
    unsigned int temp = 0;
    void __iomem *scu_va;
    void __iomem *usb_phy_va;

	scu_va = PLATFORM_SYSC_BASE;

	// FOTG210 PHY PONRST de-aasert.
	temp = readl(scu_va + 0x848);
	temp |= 0x2;
	writel(temp, scu_va + 0x848);

	// FOTG330 PHY PONRST de-aasert.
    temp = readl(scu_va + 0x84C);
    temp |= 0x4;
	writel(temp, scu_va + 0x84C);
    printf("release reset of usb \n");
    usb_phy_va = PLAT_USB_PHY_BASE;

    temp = readl(usb_phy_va + 0x10);
	temp &= ~0xff00;
	temp |= 0xd000;
	writel(temp, usb_phy_va + 0x10);

    printf("Init fusb300 controller %x fusb300 %x\n",fusb300->reg,fusb300);
    fusb300_probe(&fusb300_controller);
	printf("over\n");
	return 0;
}
#endif
