/*
 * (C) Copyright 2012 Faraday Technology
 * Bing-Yao Luo <kay@faraday-tech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <asm/io.h>
#include <common.h>
#include <dm.h>
#include <nand.h>
#include <netdev.h>
#include <ns16550.h>

#include <faraday/ftnandc024.h>

DECLARE_GLOBAL_DATA_PTR;

extern void clock_init(void);

int board_early_init_f(void)
{
	clock_init();
	return 0;
}

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_FARADAY;
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
	return 0;
}

int board_late_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = gd->ram_size;
	return 0;
}

void mdio_pinmux_init(void)
{
	unsigned long val;

	val = readl(SCU_BASE + 0x112c);       
	val |= 0x01<<14; //mdc pinmux
	writel(val, SCU_BASE + 0x112c);
	printf("MDIO pinmux setting: register of 0x112c=0x%02x\n", readl(SCU_BASE + 0x112c));
}

#if 0
void GemSerDesPgm(void){
	lte_init_sgmii();
	udelay(500000);
}
#endif

//extern int ftgmac100_initialize(bd_t *bd);
int board_eth_init(struct bd_info *bd)
{
	//enable MDIO/MDC pinmux in SCU
	mdio_pinmux_init();
#ifdef CONFIG_USB_ETHER
	return usb_eth_initialize(bd);
#elif defined(CONFIG_FTGMAC100)
	return ftgmac100_initialize(bd);
#elif defined(CONFIG_GMAC_BASE)
//	GemSerDesPgm();
//	return gem_initialize(bd);
#endif
	return 0;
}

#ifdef CONFIG_SYS_NAND_SELF_INIT
#define CFG_START_CE			0
#define CFG_BI_RESERVE_SMALL	6
#define CFG_BI_RESERVE_LARGE	1

static struct nand_chip nand_chip;
static struct platform_nand_flash platform_nand;

void board_nand_init(void)
{
#ifndef CONFIG_DM_NAND
	int devnum = 0;
	struct mtd_info *mtd;

	nand_chip.page_shift = 11;	/* 2048 */
	//chip->phys_erase_shift = chip->page_shift + 5; //block = 32 pages per block
	nand_chip.phys_erase_shift = nand_chip.page_shift + 6; //block = 64 pages per block
	platform_nand.row_addrcycles = 3;

	if (nand_chip.page_shift == 9) {
		platform_nand.col_addrcycles = 1;
		platform_nand.ecc_bits_dat = 2;
		platform_nand.ecc_bits_oob = 1;//according to signoff form. same as rom code
		if (ftnandc024_init(&nand_chip, CONFIG_FTNANDC024_BASE, CONFIG_FTNANDC024_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_SMALL))
			goto bni_err;
	} else {
		platform_nand.col_addrcycles = 2;
		platform_nand.ecc_bits_dat = 4;
		platform_nand.ecc_bits_oob = 2;//according to signoff form. same as rom code
		if (ftnandc024_init(&nand_chip, CONFIG_FTNANDC024_BASE, CONFIG_FTNANDC024_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_LARGE))
			goto bni_err;
	}

	mtd = nand_to_mtd(&nand_chip);

	if (mtd->writesize && ((1 << nand_chip.page_shift) != mtd->writesize)) {
		printf("ftnandc024: page size is supposed to be %d, not %d\n",
			mtd->writesize, 1 << nand_chip.page_shift);
	}

	if (mtd->erasesize && ((1 << nand_chip.phys_erase_shift) != mtd->erasesize)) {
		printf("ftnandc024: block size is supposed to be %d, not %d\n",
			mtd->erasesize, 1 << nand_chip.phys_erase_shift);
	}

	nand_register(devnum,mtd);

	return;

bni_err:
	free(mtd->priv);
#else
	return;
#endif
}
#endif /* #ifdef CONFIG_SYS_NAND_SELF_INIT */

extern int ftsdc021_mmc_init(int dev_index);
int board_mmc_init(struct bd_info *bis)
{
#ifdef CONFIG_FTSDC021
	return ftsdc021_sdhci_init(CONFIG_FTSDC021_BASE);
#endif
	return 0;
}