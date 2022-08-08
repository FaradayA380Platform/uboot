/*
 * arch/arm/include/asm/arch-hgu10g/hardware.h
 *
 * (C) Copyright 2015 Faraday Technology
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
#define PLATFORM_CPU_PERIPHBASE         0x3A000000

/*
 * PCIE
 */
#define PLATFORM_FTPCIE_AXISLAVE_BASE   0x10000000
#define PLATFORM_FTPCIE_AXISLAVE_1_BASE 0x20000000
#define PLATFORM_FTPCIE_WRAP_BASE       0x06000000
#define PLATFORM_FTPCIE_WRAP_1_BASE     0x07000000
#define PLATFORM_FTPCIE_WRAP_GLUE_BASE  0x38200000
#define PLATFORM_FTPCIE_WRAP_GLUE_1_BASE 0x38300000

#define PLATFORM_FTVSEMI_PCIE_BASE      0x34600000
#define PLATFORM_FTVSEMI_PCIE_1_BASE    0x34700000

/*
 * DDRC
 */
#define PLATFORM_DDRC_BASE              0x04300000

/*
 * SCU
 */
#define PLATFORM_SYSC_BASE              0x05C00000

/*
 * Timer
 */
#define FTTMR010_BASE          0x05F00000

/*
 * UART
 */
#define PLATFORM_FTUART010_BASE         0x04100000

/*
 * I2C
 */
#define PLATFORM_FTI2C010_BASE          0x04500000

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
#define PLATFORM_DWC_OTG3_BASE          0x30000000
#define PLATFORM_DWC_OTG3_BASE1         0x31000000

/*
 * SDHCI Controller
 */
#define PLATFORM_FTSDC021_BASE          0x08100000

/*
  * ssp010
  */
#define PLATFORM_FTSSP010_BASE          0x06d00000
#endif