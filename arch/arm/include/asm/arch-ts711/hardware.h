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

#ifndef __TS711_H
#define __TS711_H

#include <linux/sizes.h>

#ifndef __ASSEMBLY__
#define REG32(off)  *(volatile unsigned long *)(off)
#define REG16(off)  *(volatile unsigned short *)(off)
#define REG8(off)   *(volatile unsigned char *)(off)
#endif /* __ASSEMBLY__ */

#define NR_IRQS                     160

#define SYS_PL310_BASE              0xF5000000
#define CPU_PERIPHERAL_BASE         0xF4000000 

/*
 * Hardware AHB peripheral
 */
#define FTGMAC030_0_BASE            0x84500000  /* MAC */
#define FOTG210_BASE1               0x84400000
#define FOTG210_BASE0               0x84300000
#define FTSDC021_1_BASE             0x84200000
#define FTSDC021_0_BASE             0x84100000

/* DDR */
#define FTDDR3030_BASE              0x82B00000  /* DDR */

/*
 * Hardware APB peripheral
 */
#define FTSSP010_5_BASE             0x82600000
#define FTSSP010_4_BASE             0x82500000
#define FTSSP010_3_BASE             0x82400000
#define FTSSP010_2_BASE             0x82300000
#define FTSSP010_1_BASE             0x82200000
#define FTSSP010_0_BASE             0x82100000
#define FTUART010_3_BASE            0x80D00000  /* UART */
#define FTUART010_2_BASE            0x80C00000  /* UART */
#define FTUART010_1_BASE            0x80B00000  /* UART */
#define FTUART010_0_BASE            0x80A00000  /* UART */
#define FTIIC010_0_BASE             0x80500000  /* I2C */
#define FTSCU100_BASE               0x80400000  /* SCU */
#define FTTMR010_1_BASE             0x80300000
#define FTTMR010_BASE               0x80200000

#define FTSPI020_BASE               0x40000000

/* 
 * Interrupt numbers of Hierarchical Architecture 
 */
#define IRQ_SPI_START               32

#define PLAT_FTAXIC030_IRQ              (IRQ_SPI_START + 1)

#define PLAT_FTX2P030_U0_IRQ            (IRQ_SPI_START + 2)
#define PLAT_FTX2P030_U1_IRQ            (IRQ_SPI_START + 3)

#define PLAT_FTDMAC030_U0_IRQ           (IRQ_SPI_START + 4)
#define PLAT_FTDMAC030_U0_TC_IRQ        (IRQ_SPI_START + 5)
#define PLAT_FTDMAC030_U0_ERR_IRQ       (IRQ_SPI_START + 6)
#define PLAT_FTDMAC030_U0_TC7_IRQ       (IRQ_SPI_START + 7)
#define PLAT_FTDMAC030_U0_TC6_IRQ       (IRQ_SPI_START + 8)
#define PLAT_FTDMAC030_U0_TC5_IRQ       (IRQ_SPI_START + 9)
#define PLAT_FTDMAC030_U0_TC4_IRQ       (IRQ_SPI_START + 10)
#define PLAT_FTDMAC030_U0_TC3_IRQ       (IRQ_SPI_START + 11)
#define PLAT_FTDMAC030_U0_TC2_IRQ       (IRQ_SPI_START + 12)
#define PLAT_FTDMAC030_U0_TC1_IRQ       (IRQ_SPI_START + 13)
#define PLAT_FTDMAC030_U0_TC0_IRQ       (IRQ_SPI_START + 14)
#define PLAT_FTDMAC030_U0_1_IRQ         (IRQ_SPI_START + 15)
#define PLAT_FTDMAC030_U0_1_TC_IRQ      (IRQ_SPI_START + 16)
#define PLAT_FTDMAC030_U0_1_ERR_IRQ     (IRQ_SPI_START + 17)
#define PLAT_FTDMAC030_U0_1_TC7_IRQ     (IRQ_SPI_START + 18)
#define PLAT_FTDMAC030_U0_1_TC6_IRQ     (IRQ_SPI_START + 19)
#define PLAT_FTDMAC030_U0_1_TC5_IRQ     (IRQ_SPI_START + 20)
#define PLAT_FTDMAC030_U0_1_TC4_IRQ     (IRQ_SPI_START + 21)
#define PLAT_FTDMAC030_U0_1_TC3_IRQ     (IRQ_SPI_START + 22)
#define PLAT_FTDMAC030_U0_1_TC2_IRQ     (IRQ_SPI_START + 23)
#define PLAT_FTDMAC030_U0_1_TC1_IRQ     (IRQ_SPI_START + 24)
#define PLAT_FTDMAC030_U0_1_TC0_IRQ     (IRQ_SPI_START + 25)

#define PLAT_FTDDR3030_IRQ              (IRQ_SPI_START + 26)
#define PLAT_FTSPI020_IRQ               (IRQ_SPI_START + 27)
#define PLAT_FTGPIO010_IRQ              (IRQ_SPI_START + 28)
#define PLAT_FTIIC010_IRQ               (IRQ_SPI_START + 29)
#define PLAT_FTCS010_ADCC_IRQ           (IRQ_SPI_START + 30)

#define PLAT_FTTMR010_IRQ               (IRQ_SPI_START + 31)
#define PLAT_FTTMR010_T1_IRQ            (IRQ_SPI_START + 32)
#define PLAT_FTTMR010_T2_IRQ            (IRQ_SPI_START + 33)
#define PLAT_FTTMR010_T3_IRQ            (IRQ_SPI_START + 34)
#define PLAT_FTTMR010_1_IRQ             (IRQ_SPI_START + 35)
#define PLAT_FTTMR010_1_T1_IRQ          (IRQ_SPI_START + 36)
#define PLAT_FTTMR010_1_T2_IRQ          (IRQ_SPI_START + 37)
#define PLAT_FTTMR010_1_T3_IRQ          (IRQ_SPI_START + 38)

#define PLAT_CORESIGHT_PMU0_IRQ         (IRQ_SPI_START + 39)
#define PLAT_CORESIGHT_PMU1_IRQ         (IRQ_SPI_START + 40)

#define PLAT_FTOTG210_CHIP_IRQ          (IRQ_SPI_START + 41)
#define PLAT_FTOTG210_CHIP_1_IRQ        (IRQ_SPI_START + 42)

#define PLAT_FTSDC021_IRQ               (IRQ_SPI_START + 43)
#define PLAT_FTSDC021_1_IRQ             (IRQ_SPI_START + 44)

#define PLAT_FTGMAC100_S_IRQ            (IRQ_SPI_START + 45)
#define PLAT_FTAHBC020S_IRQ             (IRQ_SPI_START + 46)
#define PLAT_FTH2X030_IRQ               (IRQ_SPI_START + 47)
#define PLAT_CAN_CORE_IRQ               (IRQ_SPI_START + 50)
#define PLAT_CAN_CORE_1_IRQ             (IRQ_SPI_START + 51)
#define PLAT_SYSC_IRQ                   (IRQ_SPI_START + 52)
#define PLAT_FTUART010_IRQ              (IRQ_SPI_START + 53)
#define PLAT_FTUART010_1_IRQ            (IRQ_SPI_START + 54)
#define PLAT_FTUART010_2_IRQ            (IRQ_SPI_START + 55)
#define PLAT_FTUART010_3_IRQ            (IRQ_SPI_START + 56)
#define PLAT_FTSSP010_IRQ               (IRQ_SPI_START + 57)
#define PLAT_FTSSP010_1_IRQ             (IRQ_SPI_START + 58)
#define PLAT_FTSSP010_2_IRQ             (IRQ_SPI_START + 59)
#define PLAT_FTSSP010_3_IRQ             (IRQ_SPI_START + 60)
#define PLAT_FTSSP010_4_IRQ             (IRQ_SPI_START + 61)
#define PLAT_FTSSP010_5_IRQ             (IRQ_SPI_START + 62)

#define PLAT_FTDMAC030_U1_IRQ           (IRQ_SPI_START + 63)
#define PLAT_FTDMAC030_U1_TC_IRQ        (IRQ_SPI_START + 64)
#define PLAT_FTDMAC030_U1_ERR_IRQ       (IRQ_SPI_START + 65)
#define PLAT_FTDMAC030_U1_TC7_IRQ       (IRQ_SPI_START + 66)
#define PLAT_FTDMAC030_U1_TC6_IRQ       (IRQ_SPI_START + 67)
#define PLAT_FTDMAC030_U1_TC5_IRQ       (IRQ_SPI_START + 68)
#define PLAT_FTDMAC030_U1_TC4_IRQ       (IRQ_SPI_START + 69)
#define PLAT_FTDMAC030_U1_TC3_IRQ       (IRQ_SPI_START + 70)
#define PLAT_FTDMAC030_U1_TC2_IRQ       (IRQ_SPI_START + 71)
#define PLAT_FTDMAC030_U1_TC1_IRQ       (IRQ_SPI_START + 72)
#define PLAT_FTDMAC030_U1_TC0_IRQ       (IRQ_SPI_START + 73)

#define PLAT_FTX2P030_U2_IRQ            (IRQ_SPI_START + 74)
#define PLAT_GNSS_IRQ                   (IRQ_SPI_START + 94)
#define PLAT_OW_IRQ                     (IRQ_SPI_START + 95)
#define PLAT_CORESIGHT_PMU2_IRQ         (IRQ_SPI_START + 96)
#define PLAT_CORESIGHT_PMU3_IRQ         (IRQ_SPI_START + 97)

#endif
