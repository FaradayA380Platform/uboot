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
#include <asm/arch-optichip/optichip.h>
#include <faraday/ftsmc020.h>
#include <faraday/ftsdc010.h>

#include <dm.h>
#include <ns16550.h>

DECLARE_GLOBAL_DATA_PTR;


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

void lowlevel_init()
{
}

void dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size =  gd->ram_size;
}

int dram_init(void)
{
	unsigned long sdram_base = CONFIG_SYS_SDRAM_BASE;
	unsigned long expected_size = CONFIG_SYS_SDRAM_SIZE;
	unsigned long actual_size;

	actual_size = get_ram_size((void *)sdram_base, expected_size);

	gd->ram_size = SZ_128M;
	if (expected_size != actual_size)
		printf("Warning: Only %lu of %lu MiB SDRAM is working\n",
				actual_size >> 20, expected_size >> 20);

	return 0;
}

void GemSerDesPgm(void){
	lte_init_sgmii();
	udelay(500000);
}

//extern int ftgmac100_initialize(bd_t *bd);
int board_eth_init(bd_t *bd)
{
#ifdef CONFIG_USB_ETHER
	return usb_eth_initialize(bd);
#elif defined(CONFIG_FTGMAC100)
	return ftgmac100_initialize(bd);
#elif defined(CONFIG_GMAC_BASE)
	GemSerDesPgm();
    return gem_initialize(bd);
#endif
}

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

int misc_init_r()
{
	
	return 0;
}

EXPORT_SYMBOL(lowlevel_init);
