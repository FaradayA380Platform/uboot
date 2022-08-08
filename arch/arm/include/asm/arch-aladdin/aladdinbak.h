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

#ifndef __ALADDIN_H
#define __ALADDIN_H

#include <linux/sizes.h>

#ifndef __ASSEMBLY__

#define REG32(off)  *(volatile unsigned long *)(off)
#define REG16(off)  *(volatile unsigned short *)(off)
#define REG8(off)   *(volatile unsigned char *)(off)

#endif /* __ASSEMBLY__ */



/*
 * Hardware register bases
 */
#define CONFIG_FTAHBC020_BASE	0x90100000	/* AHB Controller */
#define CONFIG_FTTMR010_BASE    0x90010000  /*Timer*/
#define CONFIG_FTUART010_BASE   0x90020000  /* UART controller */
#define CONFIG_SCU_BASE			0x900A0000  /* SCU */
#define CONFIG_FTWDT010_BASE    0x90C00000  /* Watchdog */
#define CONFIG_FTINTC0_BASE     0x96800000  /* ftintc030 */                                                          
#define CONFIG_FTNANDC024_BASE  0xC0600000
#define CONFIG_SYS_RTC_PL031_BASE 0x90090000
#define CONFIG_FTDDRIII030_BASE  0x92F00000  /* DDR3 Controller */


#define NR_IRQS 64                                                                                                   
#define CONFIG_FTTMR010_IRQ      13


#define GEM_STATISTICS_TX_OCTECTS_HIGH  0x104
#define GEM_STATISTICS_TX_OCTECTS_LOW   0x100
#define GEM_STATISTICS_TX_FRAME         0x108
#define GEM_STATISTICS_TX_FRAME_64      0X118
#define GEM_STATISTICS_TX_FRAME_65_127  0x11c
#define GEM_STATISTICS_TX_FRAME_128_255 0x120
#define GEM_STATISTICS_TX_FRAME_256_511 0x124
#define GEM_STATISTICS_TX_FRAME_512_1023 0x120
#define GEM_STATISTICS_TX_FRAME_1024_1518 0x12c
#define GEM_STATISTICS_RX_OCTECTS_HIGH  0x154
#define GEM_STATISTICS_RX_OCTECTS_LOW   0x150
#define GEM_STATISTICS_RX_FRAME         0x158
#define GEM_STATISTICS_RX_FRAME_64      0X168
#define GEM_STATISTICS_RX_FRAME_65_127  0x16c
#define GEM_STATISTICS_RX_FRAME_128_255 0x170
#define GEM_STATISTICS_RX_FRAME_256_511 0x174
#define GEM_STATISTICS_RX_FRAME_512_1023 0x178
#define GEM_STATISTICS_RX_FRAME_1024_1518 0x17c


/* SCU Base */
#define SYSC_REG_BASE 		0x90A00000

/* GEM */
#define CONFIG_GMAC_BASE         0x93000000
#ifdef CONFIG_GMAC_BASE
#define CONFIG_CADENCE_GEM         1
#define CONFIG_TXDES_NUM           64
#define CONFIG_RXDES_NUM           64
#define CONFIG_RXBUF_NUM           CONFIG_RXDES_NUM
#define CONFIG_RXBUF_SIZE          1024
#define CONFIG_TFTP_BLOCKSIZE      512
#define CONFIG_GMAC_U4_BASE		   CONFIG_GMAC_BASE
#endif

/* PCIE Gen3*/
#define PCIE_WRAP_BASE		0x93600000
#define PCIE_G3_REG_BASE	0x93800000

/* SPI020 */
#define CONFIG_FTSPI020_BASE	0xC0A00000

/* SerDes */
#define SERDES_MEMORY_BASE0 0x92600000
#define SERDES_MEMORY_BASE1 0x92700000
#define SERDES_MEMORY_BASE2 0x92800000

#define SERDES_GEM_BASE		SERDES_MEMORY_BASE2

/* SD/MMC */
/*#define CONFIG_FTSDC021_BASE 0xc0400000*/

#endif	/* __A380_H */
