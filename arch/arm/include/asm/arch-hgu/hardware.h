/*
 * arch/arm/include/asm/arch-hgu/hardware.h
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

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <linux/sizes.h>

/*
 * CA9 private memory region
 */
#define CPU_PERIPHBASE	0x96000000

/*
 * SCU
 */
#define SYSC_BASE		0x90900000

/*
 * Timer
 */
#define FTTMR010_BASE	0x90f00000

/*
 * UART
 */
#define FTUART010_BASE	0x90400000

/*
 * I2C

#define CONFIG_FTI2C010_BASE	0x90200000
 */
/*
 * SPI Flash Controller
 */
#define	PLATFORM_FTSPI020_BASE	0xa0100000

/*
 * NAND Flash Controller

#define	CONFIG_FTNANDC024_BASE	0xa0200000
#define	CONFIG_FTNANDC024_DATA_BASE	0xa0300000
 */
/*
 * USB/OTG Controller

#define	CONFIG_FOTG210_BASE		0xa0400000
#define	CONFIG_FOTG210_BASE1	0xa0500000
 */
/*
 * SDHCI Controller

#define	CONFIG_FTSDC021_BASE	0xa0600000
 */

#define FOTG210_BASE0		0xa0400000
 
#endif
