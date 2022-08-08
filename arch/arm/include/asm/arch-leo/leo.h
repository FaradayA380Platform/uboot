/*
 * (C) Copyright 2009 Faraday Technology
 * Po-Yu Chuang <ratbert@faraday-tech.com>
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

#ifndef __LEO_H
#define __LEO_H

#include <linux/sizes.h>

#ifndef __ASSEMBLY__

#define REG32(off)  *(volatile unsigned long *)(off)
#define REG16(off)  *(volatile unsigned short *)(off)
#define REG8(off)   *(volatile unsigned char *)(off)

#endif /* __ASSEMBLY__ */

#define NR_IRQS                     160

#define PLATFORM_CPU_PERIPHBASE     0x70000000 

/*
 * Hardware APB peripheral
 */
#define FTGMAC030_0_BASE            0x54100000  /* MAC */
#define FTGMAC030_1_BASE            0x54200000  /* MAC */
#define FTGMAC030_2_BASE            0x54300000  /* MAC */
#define FTUART010_0_BASE            0x54E00000  /* UART */
#define FTUART010_1_BASE            0x54F00000  /* UART */
#define FTUART010_2_BASE            0x55000000  /* UART */
#define FTUART010_3_BASE            0x55100000  /* UART */
#define FTUART010_4_BASE            0x55200000  /* UART */
#define FTUART010_5_BASE            0x56900000  /* UART */
#define FTUART010_6_BASE            0x56A00000  /* UART */
#define FTUART010_7_BASE            0x56B00000  /* UART */
#define FTUART010_8_BASE            0x56C00000  /* UART */
#define FTUART010_9_BASE            0x57100000  /* UART */

/*
 * Hardware AXI1 peripheral
 */

/*
 * Hardware AXI0 peripheral
 */

/* SCU */
#define FTSCU100_BASE               0x28000000  /* SCU */

/* DDR */
#define FTDDR3030_BASE              0x28900000  /* DDR */

/* SD/MMC */
#define FOTG210_BASE0               0x10200000  /* USB */
#define FOTG210_BASE1               0x10300000  /* USB */
#define FOTG210_BASE2               0x10400000  /* USB */

/* I2C */
#define FTIIC010_0_BASE             0x55500000  /* I2C */
#define FTIIC010_1_BASE             0x55600000  /* I2C */
#define FTIIC010_2_BASE             0x55700000  /* I2C */
#define FTIIC010_3_BASE             0x56F00000  /* I2C */
#define FTIIC010_4_BASE             0x57000000  /* I2C */

/* WDT */
#define FTWDT011_0_BASE             0x28100000  /* WDT */

#define SCU_BASE                    FTSCU100_BASE

#define CONFIG_SCU_BASE             FTSCU100_BASE
#define CONFIG_FTGMAC030_BASE       FTGMAC030_2_BASE

#endif	/* __LEO_H */
