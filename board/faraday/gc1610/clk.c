/*
 * (C) Copyright 2020 Faraday Technology
 * Bo-Cun Chen <bcchen@faraday-tech.com>
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

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

/***************************************************
 * Platform dependant clock routine                *
 ***************************************************/

static ulong clk_get_rate_ahb(void)
{
	return 200000000;
}

static ulong clk_get_rate_apb(void)
{
	return 100000000;
}

static ulong clk_get_rate_mmc(void)
{
	return 200000000;
}

static ulong clk_get_rate_cpu(void)
{
	return 1548000000;
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
			return clk_get_rate_apb();
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
