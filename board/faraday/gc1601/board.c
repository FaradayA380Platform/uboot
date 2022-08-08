/*
 * board/faraday/gc1601/board.c
 *
 * (C) Copyright 2020 Faraday Technology
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

#include "board.h"

DECLARE_GLOBAL_DATA_PTR;

/* System Control Uint (pinmux) */
static void pinmux_init(void)
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

int board_init(void)
{
	gd->bd->bi_arch_number = MACH_TYPE_FARADAY;
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
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
	gd->bd->bi_dram[0].size =  gd->ram_size;
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

int board_eth_init(bd_t *bd)
{
	int ret = 0;
#if defined(CONFIG_USB_ETHER)
	ret = usb_eth_initialize(bd);
#elif defined(CONFIG_FTGMAC030)
	ret = ftgmac030_initialize(bd);
#endif
	return ret;
}
