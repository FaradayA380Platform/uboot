/*
 * board/faraday/hgu10g/clock.c
 *
 * (C) Copyright 2015 Faraday Technology
 * B.C. Chen <bcchen@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

#define XTAL    (25*1000*1000)      // 25MHz
#define XOSC    (156.25*1000*1000)  // 156.25Hz

/*
 * ToDo:
 *
 * Determine the real clock rate from SCU registers
 */
unsigned long pll0_ns(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x30) >> 24) & 0xff;
}

unsigned long pll0_ms(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x30) >> 16) & 0xff;
}

unsigned long pll1_ns(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x40) >> 24) & 0xff;
}

unsigned long pll1_ms(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x40) >> 16) & 0xff;
}

static inline ulong clk_get_rate_ahb(void)
{
	ulong pll1_out, ahb_clk;

	pll1_out = 250*1000*1000;
#if 1
	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 20) & 3) {
	case 2:
		ahb_clk = pll1_out;
		break;
	case 1:
	default:
		ahb_clk = pll1_out/2;
		break;
	}
#else
	ahb_clk = pll1_out*1;
#endif

	return ahb_clk;
}

static inline ulong clk_get_rate_apb(void)
{
	return clk_get_rate_ahb() >> 1;
}

static inline ulong clk_get_rate_cpu(void)
{
	ulong ref_clk, pll0_out, cpu_clk;
#if 1
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 5) & 3) {
	case 1:
		ref_clk = XOSC;
		break;
	case 0:
	default:
		ref_clk = XTAL;
		break;
	}

	pll0_out = (ref_clk/pll0_ms()) * pll0_ns();

	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 16) & 3) {
	case 0:
		cpu_clk = pll0_out/4;
		break;
	case 2:
		cpu_clk = pll0_out;
		break;
	case 1:
	default:
		cpu_clk = pll0_out/2;
		break;
	}
#else
	cpu_clk = 925*1000*1000;
#endif

	return cpu_clk;
}

static inline ulong clk_get_rate_spi(void)
{
	ulong spi_clk;
#if 1
	switch (readl(PLATFORM_SYSC_BASE + 0x478) & 0x1) {
	case 1:
		spi_clk = 100*1000*1000;
		break;
	case 0:
	default:
		spi_clk = 125*1000*1000;
		break;
	}
#else
	spi_clk = 125*1000*1000;
#endif
	return spi_clk;
}

static inline ulong clk_get_rate_mmc(void)
{
	/* clock = [sdclk1x] of ftsdc021 */
	return 100*1000*1000;
}

ulong clk_get_rate(char *id)
{
	ulong clk = 0;

	if (!strcmp(id, "AHB"))
		clk = clk_get_rate_ahb();
	else if (!strcmp(id, "APB"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "CPU"))
		clk = clk_get_rate_cpu();
	else if (!strcmp(id, "SDC"))
		clk = clk_get_rate_ahb();
	else if (!strcmp(id, "I2C"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "SPI") || !strcmp(id, "SSP"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "MMC_CLK"))
		clk = clk_get_rate_mmc();

	return clk;
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
