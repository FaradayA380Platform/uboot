/*
 * arch/arm/include/asm/arch-wideband001/hardware.h
 *
 * (C) Copyright 2017 Faraday Technology
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
 * CA9 private memory region
 */
#ifdef CONFIG_FARADAY_CA9_TESTCHIP
#define PLATFORM_CPU_PERIPHBASE         0xE0000000
#else
#define PLATFORM_CPU_PERIPHBASE         0x3A000000
#endif

/*
 * PCIE
 */
#define PLATFORM_FTPCIE_AXISLAVE_BASE   0x10000000
#define PLATFORM_FTPCIE_AXISLAVE_1_BASE 0x20000000
#define PLATFORM_FTPCIE_WRAP_BASE       0x05000000
#define PLATFORM_FTPCIE_WRAP_1_BASE     0x05800000
#define PLATFORM_FTPCIE_WRAP_GLUE_BASE  0x36400000
#define PLATFORM_FTPCIE_WRAP_GLUE_1_BASE 0x36500000

#define PLATFORM_FTVSEMI_PCIE_BASE      0x38100000
#define PLATFORM_FTVSEMI_DUAL_BASE      0x38200000
#define PLATFORM_FTVSEMI_PON_BASE       0x38300000

/*
 * DDRC
 */
#define PLATFORM_DDRC_BASE              0x04600000

/*
 * SCU
 */
#define PLATFORM_SYSC_BASE              0x04100000

/*
 * Timer
 */
#define PLATFORM_FTTMR010_BASE          0x04F00000

/*
 * UART
 */
#define PLATFORM_FTUART010_BASE         0x04700000

/*
 * I2C
 */
#define PLATFORM_FTI2C010_BASE          0x04900000

/*
 * SPI Flash Controller
 */
#define PLATFORM_FTSPI020_BASE          0x08400000

/*
 * NAND Flash Controller
 */
#define PLATFORM_FTNANDC024_BASE        0x08300000 
#define PLATFORM_FTNANDC024_DATA_BASE   0x08200000

/*
 * USB OTG Controller
 */
#define PLATFORM_FOTG210_BASE           0x08600000
#define PLATFORM_FOTG330_BASE1          0x06100000

/*
 * SDHCI Controller
 */
#define PLATFORM_FTSDC021_BASE          0x08100000

/*
 * ssp010
 */
#define PLATFORM_FTSSP010_BASE          0x04E00000

/*
 * USB PHY
 */
#define PLAT_USB_PHY_BASE               0x06100000

#endif