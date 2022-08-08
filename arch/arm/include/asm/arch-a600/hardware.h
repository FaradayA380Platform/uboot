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

#ifndef __HARDWARE_H
#define __HARDWARE_H

/*
 * CPU
 */
#define PLATFORM_CPU_PERIPHBASE         0x28600000

/*
 * DDRC
 */
#define PLATFORM_DDRC_BASE              0x2A600000

/*
 * GPIO
 */
#define PLATFORM_FTGPIO010_BASE         0x20E00000

/*
 * I2C
 */
#define PLATFORM_FTI2C010_BASE          0x20100000
#define PLATFORM_FTI2C010_1_BASE        0x20200000
#define PLATFORM_FTI2C010_2_BASE        0x20300000
#define PLATFORM_FTI2C010_3_BASE        0x20400000

/*
 * NIC
 */
#define PLATFORM_FTGMAC030_BASE         0x2AA00000
#define PLATFORM_FTGMAC030_1_BASE       0x2AB00000

#define CONFIG_FTGMAC030_BASE           PLATFORM_FTGMAC030_BASE

/*
 * SCU
 */
#define PLATFORM_SYSC_BASE              0x2AF00000

/*
 * SDC
 */
#define PLATFORM_FTSDC021_BASE          0x24300000
#define PLATFORM_FTSDC021_1_BASE        0x24400000

/*
 * SPI Flash Controller
 */
#define PLATFORM_FTSPI020_BASE          0x28300000

/*
 * Timer
 */
#define PLATFORM_FTPWMTMR010_BASE       0x20600000

/*
 * UART
 */
#define PLATFORM_FTUART010_BASE         0x20700000
#define PLATFORM_FTUART010_1_BASE       0x20800000
#define PLATFORM_FTUART010_2_BASE       0x20900000
#define PLATFORM_FTUART010_3_BASE       0x20A00000

/*
 * Watchdog Controller
 */
#define PLATFORM_FTWDT011_BASE          0x20500000

#endif	/* __HARDWARE_H */
