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
#include <nand.h>
#include <netdev.h>

#include <asm/arch/scu.h>
#include <asm/arch-echonous/echonous.h>
#include <faraday/ftsmc020.h>
#include <faraday/ftlcdc200.h>
#include <faraday/ftsdc010.h>
#include <faraday/ftnandc021.h>

#include <dm.h>
#include <ns16550.h>
DECLARE_GLOBAL_DATA_PTR;

void clock_init(void);

int board_early_init_f(void)
{
	clock_init();
	return 0;
}

int board_init(void)
{
	icache_enable();
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	return 0;
}

void lowlevel_init(void)
{
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size =  gd->ram_size;
}

int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE; 
	return 0;
}

//extern int ftgmac100_initialize(bd_t *bd);
int board_eth_init(bd_t *bd)
{
#ifdef CONFIG_USB_ETHER
	return usb_eth_initialize(bd);
#elif defined(CONFIG_FTGMAC100)
	return ftgmac100_initialize(bd);
#else
	return 0;
#endif
}

#ifdef CONFIG_SYS_NAND_SELF_INIT
void board_nand_init(void)
{
	int alen, devnum = 0;
	struct mtd_info *mtd = &nand_info[devnum];
	struct nand_chip *chip;
	uint32_t iobase = CONFIG_SYS_NAND_BASE;
	uint32_t ehwcfg = readl(&scu->ehwcfg);

	chip = calloc(1, sizeof(*chip));
	if (!chip)
		return;
	mtd->priv = chip;

	/* page size */
	switch (EHWCFG_NAND_PS(ehwcfg)) {
	case 0:
		chip->page_shift = 9;	/* 512 */
		break;
	case 1:
		chip->page_shift = 11;	/* 2048 */
		break;
	default:
		chip->page_shift = 12;	/* 4096 */
		break;
	}

	/* block size */
	chip->phys_erase_shift = chip->page_shift + 4
		+ EHWCFG_NAND_BK(ehwcfg);

	/* address length/cycle */
	alen = 3 + EHWCFG_NAND_AC(ehwcfg);

	if (ftnandc021_init(chip, iobase, alen))
		goto bni_err;

	if (nand_scan(mtd, CONFIG_SYS_NAND_MAX_CHIPS))
		goto bni_err;

	nand_register(devnum);

	return;

bni_err:
	free(chip);
}
#endif /* #ifdef CONFIG_SYS_NAND_SELF_INIT */

int board_late_init(void)
{
//	reset_timer();
	return 0;
}


extern int ftsdc021_mmc_init(int dev_index);
int board_mmc_init(bd_t *bis)
{
#ifdef CONFIG_FTSDC021
	return ftsdc021_sdhci_init(CONFIG_FTSDC021_BASE);
#else
	return 0;
#endif  

	return 0;
}
struct fa_serial_platdata {
      uint32_t *reg;  /* address of registers in physical memory */
      bool use_dte;
};

static const struct ns16550_platdata fa_serial_plat= {
	.base = CONFIG_FTUART010_BASE,
    .reg_shift = 2,
    .clock = CONFIG_SYS_NS16550_CLK,
	.fcr = UART_FCR_DEFVAL,
};

/*
static struct fa_serial_platdata fa_serial_plat = {
      .reg = 0x90400000,
};
*/

U_BOOT_DEVICE(fa_serial) = {
      .name   = "ns16550_serial",
      .platdata = &fa_serial_plat,
};
int misc_init_r()
{
	
	return 0;
}

EXPORT_SYMBOL(lowlevel_init);
