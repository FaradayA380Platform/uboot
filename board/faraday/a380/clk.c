/*
 * (C) Copyright 2010
 * Faraday Technology Inc. <www.faraday-tech.com>
 * Dante Su <dantesu@gmail.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/clock.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;


/***************************************************
 * Platform dependant clock routine                *
 ***************************************************/
#define XOSC_0			(25*1000*1000)		// 25MHz
#define XOSC_4			(33*1000*1000)		// 33MHz


unsigned long pll0_ns(void)
{
	return (REG32((void __iomem*)SCU_BASE+0xb0)>>3) & 0x3f;
}

unsigned long pll1_ns(void)
{
	return (REG32((void __iomem*)SCU_BASE+0xb0)>>9) & 0x7;
}


unsigned long pll2_ns(void)
{
	return (REG32((void __iomem*)SCU_BASE+0xb0)>>12) & 0x3f;
}


unsigned long pll4_ns(void)
{
	return (REG32((void __iomem*)SCU_BASE+0xb0)>>18) & 0x3f;
}

static ulong clk_get_rate_ahb(void)
{
#define pll5_ns					24			// pll5_ns is hardwired to 24
	return XOSC_4*pll5_ns/4;	
}

static ulong clk_get_rate_apb(void)
{
	return clk_get_rate_ahb() >> 1;
}

static ulong clk_get_rate_mmc(void)
{
	return 198000000;
}

static ulong clk_get_rate_cpu(void)
{
#ifdef CONFIG_SOC_CA9
	return XOSC_0*pll0_ns()/2*pll1_ns();
#else	
	return XOSC_0*pll4_ns();
#endif	
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

