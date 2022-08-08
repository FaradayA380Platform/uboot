/*
 * arch/arm/include/asm/arch-tc12ngrc/hardware.h
 *
 * (C) Copyright 2019 Faraday Technology
 * B.C. Chen <bcchen@faraday-tech.com>
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
 * DDRC
 */
#define PLATFORM_DDRC_BASE              0x90B00000

/*
 * SCU
 */
#define PLATFORM_SYSC_BASE              0x90A00000

/*
 * Timer
 */
#define PLATFORM_FTTMR010_BASE          0x90200000
#define PLATFORM_FTTMR010_1_BASE        0x90300000

/*
 * UART
 */
#define PLATFORM_FTUART010_BASE         0x90400000

/*
 * Interrupt Controller
 */
#define PLATFORM_FTINTC030_BASE         0x96000000

/*
 * I2C
 */
#define PLATFORM_FTI2C010_BASE          0x90600000
#define PLATFORM_FTI2C010_1_BASE        0x90700000

/*
 * GPHY
 */
#define PLATFORM_GPHY_REG_BASE          0x91400000

/*
 * SerDes
 */
#define PLATFORM_SERDES_REG_BASE        0x91000000
#define PLATFORM_SERDES_MEM_BASE        0x91300000

/*
 * SPI Flash Controller
 */
#define PLATFORM_FTSPI020_BASE          0xA0000000

/*
 * Watchdog Controller
 */
#define PLATFORM_FTWDT010_BASE          0x90800000

#endif