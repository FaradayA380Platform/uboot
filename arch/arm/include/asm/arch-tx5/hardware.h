/*
 * (C) Copyright 2021 Faraday Technology
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
#define PLATFORM_CPU_PERIPHBASE         0x03000000

/*
 * DDRC
 */
#define PLATFORM_DDRC_BASE              0x0C200000

/*
 * GPIO
 */
#define PLATFORM_FTGPIO010_BASE         0x04800000

/*
 * I2C
 */
#define PLATFORM_FTI2C010_BASE          0x0CA00000
#define PLATFORM_FTI2C010_1_BASE        0x0CB00000
#define PLATFORM_FTI2C010_2_BASE        0x0CC00000
#define PLATFORM_FTI2C010_3_BASE        0x0CD00000

/*
 * MAC
 */
#define PLATFORM_FTGMAC030_BASE         0x05200000
#define PLATFORM_FTGMAC030_1_BASE       0x05300000

#define CONFIG_FTGMAC030_BASE           PLATFORM_FTGMAC030_BASE

/*
 * SCU
 */
#define PLATFORM_SYSC_BASE              0x0C100000

/*
 * SDC
 */
#define PLATFORM_FTSDC021_BASE          0x02200000
#define PLATFORM_FTSDC021_1_BASE        0x02300000

/*
 * SPI Flash Controller
 */
#define PLATFORM_FTSPI020_BASE          0x03600000

/*
 * Timer
 */
#define PLATFORM_FTPWMTMR010_BASE       0x04F00000
#define PLATFORM_FTPWMTMR010_1_BASE     0x05000000
#define PLATFORM_FTTMR010_BASE          0x05100000

/*
 * UART
 */
#define PLATFORM_FTUART010_BASE         0x0C600000
#define PLATFORM_FTUART010_1_BASE       0x0C700000
#define PLATFORM_FTUART010_2_BASE       0x0C800000
#define PLATFORM_FTUART010_3_BASE       0x0C900000

/*
 * USART
 */
#define PLATFORM_FTUSART010_BASE        0x0C400000
#define PLATFORM_FTUSART010_1_BASE      0x0C500000

/*
 * USB OTG Controller
 */
#define PLATFORM_FOTG330_BASE           0x03100000
#define PLATFORM_FOTG210_BASE           0x03200000
#define PLATFORM_FOTG210_1_BASE         0x03300000

/*
 * Watchdog Controller
 */
#define PLATFORM_FTWDT011_BASE          0x04D00000
#define PLATFORM_FTWDT011_1_BASE        0x04E00000

#endif	/* __HARDWARE_H */
