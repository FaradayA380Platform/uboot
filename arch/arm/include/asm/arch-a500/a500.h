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

#ifndef __A500_H
#define __A500_H

#include <linux/sizes.h>

#ifndef __ASSEMBLY__

#define REG32(off)  *(volatile unsigned long *)(off)
#define REG16(off)  *(volatile unsigned short *)(off)
#define REG8(off)   *(volatile unsigned char *)(off)

#endif /* __ASSEMBLY__ */

#define NR_IRQS                 64
#define FTTMR010_IRQ            13

/*
 * Hardware APB peripheral
 */
#define SCU_BASE			0x13000000  /* SCU */
#define FTTMR010_BASE		0x13100000  /* Timer */
#define TDC_BASE			0x13100000  /* TDC */
#define GPIO0_BASE			0x13300000  /* GPIO */
#define GPIO1_BASE			0x13400000  /* GPIO */
#define GPIO2_BASE			0x13500000  /* GPIO */
#define ADCC010_BASE		0x13600000  /* ADCC010 */
#define EMC0_BASE			0x13700000  /* EMC0 */
#define EMC1_BASE			0x13800000  /* EMC1 */
#define GMAC0_BASE			0x13900000  /* GMAC0 */
#define GMAC1_BASE			0x13A00000  /* GMAC1 */
#define AXIC0_BASE			0x13B00000  /* AXIC0 */
#define AXIC1_BASE			0x13C00000  /* AXIC1 */
#define LCDC_BASE			0x13D00000  /* LCD */
#define SHA_BASE			0x13E00000  /* SHA */
#define TRNG_BASE			0x13F00000  /* TRNG */
#define PCIECTL_BASE			0x14000000  /* PCIECTL */
#define PCIEPHY_BASE			0x14100000  /* PCIEPHY */
#define DDRCTL_BASE				0x14200000  /* DDRCTL */
#define DDRPHY_BASE			0x14300000  /* DDRPHY */
#define PCIE1CTL_BASE			0x14400000  /* PCIECTL */
#define PCIE1PHY_BASE			0x14500000  /* PCIEPHY */
#define RSA_BASE				0x14600000  
#define OTGPHY_BASE			0x14700000  
#define WDT_BASE                0x14800000  /* WDT */
#define GIC_BASE                0x18700000  

/*
 * Hardware AHB peripheral
 */
#define I2C0_BASE   			0x08000000  /* I2C  */
#define I2C1_BASE			0x08100000  /* I2C  */
#define FTUART010_0_BASE   	0x08200000  /* UART */
#define FTUART010_1_BASE   	0x08300000  /* UART */
#define FTUART010_2_BASE  	0x08400000  /* UART */
#define FTUART010_3_BASE   	0x08500000  /* UART */
#define FTDMAC020_BASE  	0x09000000  /* DMAC020 */
#define FTSPI020_BASE   		0x09100000  /* SPI020 */
#define FTSDC021_BASE   		0x09200000  /* SDC021 */

/*
 * Hardware AXI1 peripheral
 */
#define FTOTG330_BASE   			0x0A000000  /* OTG330 */
#define FTEMC030_2_BASE   		0x0A100000  /* EMC330 */

/*
 * Hardware AXI0 peripheral
 */
#define FTEMC030_0_BASE   		0x18000000  /* EMC030 */
#define FTEMC030_1_BASE   		0x18100000  /* EMC030 */
#define GIC400_BASE   			0x18700000  /* GIC400 */
#define PCIE0_BASE   			0x18800000  /* PCIE */
#define PCIE1_BASE   			0x18C00000  /* PCIE */

/* SCU Base */
#define SYSC_REG_BASE           SCU_BASE

/* GEM */
/*
#define CONFIG_GMAC0_BASE       GMAC0_BASE
#define CONFIG_GMAC1_BASE       GMAC1_BASE
*/
#define CONFIG_PHYLIB			1
#define CONFIG_PHY_MARVELL		1

/* PCIE Gen3*/
#define PCIE_WRAP_BASE			0x93600000
#define PCIE_G3_REG_BASE		0x93800000

/* SerDes */
#define SERDES_MEMORY_BASE0		0x92600000
#define SERDES_MEMORY_BASE1		0x92700000
#define SERDES_MEMORY_BASE2		0x92800000
#define SERDES_GEM_BASE			SERDES_MEMORY_BASE2

/* SD/MMC */
/*
#define CONFIG_FTSDC021_BASE    0x09200000
*/

/* USB */
#define FOTG330_BASE0           0x0A000000

/* I2C */
/*
#define CONFIG_FTI2C010_BASE    I2C0_BASE
*/

#endif	/* __A500_H */
