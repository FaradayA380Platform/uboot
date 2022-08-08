/*
 * board/faraday/sfu/clock.c
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * ToDo:
 *
 * Determine the real clock rate from SCU registers
 */
static inline ulong clk_get_rate_ahb(void)
{
#ifdef CONFIG_HGU_FPGA_PLATFORM
	return 25000000;
#else
	return 100000000;
#endif
}

static inline ulong clk_get_rate_apb(void)
{
#ifdef CONFIG_HGU_FPGA_PLATFORM
	return 25000000;
#else
	return 50000000;
#endif
}

static inline ulong clk_get_rate_cpu(void)
{
	return 4 * clk_get_rate_ahb();
}

static inline ulong clk_get_rate_spi(void)
{
#ifdef CONFIG_HGU_FPGA_PLATFORM
	return 25000000;
#else
	return 66500000;
#endif
}

static inline ulong clk_get_rate_mmc(void)
{
	/* clock = [sdclk1x] of ftsdc021 */
	return 50000000;
}

ulong clock_get_rate(enum clk_id id)
{
	switch (id) {
	case SYS_CLK:
	case AHB_CLK:
		return clk_get_rate_ahb();
	case APB_CLK:
		return clk_get_rate_apb();
	case CPU_CLK:
		return clk_get_rate_cpu();
	case I2C_CLK:
		return clk_get_rate_apb();
	case SPI_CLK:
		return clk_get_rate_spi();
	case MMC_CLK:
		return clk_get_rate_mmc();
	default:
		return 0;
	}
}

void clock_init(void)
{
	gd->arch.timer_rate_hz = clk_get_rate_apb();
}
