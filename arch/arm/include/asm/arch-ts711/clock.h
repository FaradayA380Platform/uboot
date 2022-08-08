/*
 * asm/arch-faraday/clock.h
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _ASM_ARCH_FARADAY_CLOCK_H
#define _ASM_ARCH_FARADAY_CLOCK_H

/* Clock identifiers */
enum clk_id {
	SYS_CLK,
	AHB_CLK,
	APB_CLK,
	CPU_CLK,
	I2C_CLK,
	MMC_CLK,
	SDC_CLK,
	SPI_CLK,
	SSP_CLK,
	MAC_CLK,
};

void  clock_init(void);
extern ulong clock_get_rate(enum clk_id id);
extern ulong clk_get_rate(char *id);
#endif	/* _ASM_ARCH_FARADAY_CLOCK_H */
