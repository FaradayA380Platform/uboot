/*
 * arch/arm/include/asm/arch-a369/scu.h
 *
 * (C) Copyright 2010 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#ifndef __ASM_ARCH_SCU_H
#define __ASM_ARCH_SCU_H

struct a369scu_regs {
	/* 0x000 ~ 0x0ff */
	uint32_t idr;      /* ID Register */
	uint32_t revr;     /* SCU revision id */
	uint32_t hwcfg;    /* HW configuration strap */
	uint32_t cpumcfr;  /* CPUM (master) freq. control */
	uint32_t cr;       /* SCU control register */
	uint32_t sr;       /* SCU status register */
	uint32_t rsvd0[1];
	uint32_t osccr;    /* OSC control register */
	uint32_t pllcr;    /* PLL1 control register */
	uint32_t dllcr;    /* DLL control register */
	uint32_t hclkgr;   /* HCLK gating register */
	uint32_t pclkgr;   /* PCLK gating register */
	uint32_t rsvd1[52];

	/* 0x100 ~ 0x1ff */
	uint32_t spr[16];  /* Scratchpad register */
	uint32_t rsvd2[48];

	/* 0x200 ~ 0x2ff */
	uint32_t gpmux;    /* General PINMUX */
	uint32_t ehwcfg;   /* Extended HW configuration strap */
	uint32_t rsvd3[8];
	uint32_t sccfg[2]; /* Special clock configuration */
	uint32_t scer;     /* Special clock enable register */
	uint32_t rsvd;
	uint32_t mfpmux[2];/* Multi-function pinmux */
	uint32_t dcsrcr[2];/* Driving cap. & Slew rate control */
	uint32_t rsvd4[3];
	uint32_t dccr;     /* Delay chain control register */
	uint32_t pcr;      /* Power control register */
};

/* HW configuration strap */
#define HWCFG_PLL1NS(x)   (((x) >> 5) & 0x3f)
#define HWCFG_CPUM_MUL(x) ((((x) >> 3) & 0x3) > 2 ? 2 : (((x) >> 3) & 0x3))
#define HWCFG_DLL_OFF     (1 << 2)
#define HWCFG_PLL_OFF     (1 << 1)
#define HWCFG_OSCHCNT_OFF (1 << 0)

/* Extended HW configuration strap */
#define EHWCFG_AST         (1 << 15)
#define EHWCFG_DBG         (1 << 14)
#define EHWCFG_DBGBYSW     (1 << 13)
#define EHWCFG_SATA_HOST   (1 << 12)
#define EHWCFG_PCIE_RC     (1 << 11)
#define EHWCFG_NAND_BK(x)  (((x) >> 9) & 0x3)
#define EHWCFG_NAND_BK16   (0 << 9) /* 16 page per block */
#define EHWCFG_NAND_BK32   (1 << 9) /* 32 page per block */
#define EHWCFG_NAND_BK64   (2 << 9) /* 64 page per block */
#define EHWCFG_NAND_BK128  (3 << 9) /* 128 page per block */
#define EHWCFG_NAND_PS(x)  (((x) >> 7) & 0x3)
#define EHWCFG_NAND_PS512  (0 << 7) /* 512 bytes per page */
#define EHWCFG_NAND_PS2K   (1 << 7) /* 2048 bytes per page */
#define EHWCFG_NAND_PS4K   (2 << 7) /* 4096 bytes per page */
#define EHWCFG_NAND_AC(x)  (((x) >> 5) & 0x3)
#define EHWCFG_NAND_AC3    (0 << 5) /* addr cycle = 3 */
#define EHWCFG_NAND_AC4    (1 << 5) /* addr cycle = 4 */
#define EHWCFG_NAND_AC5    (2 << 5) /* addr cycle = 5 */
#define EHWCFG_NAND_16X    (1 << 4) /* NAND: 16bit mode */
#define EHWCFG_EXTCPU      (1 << 2) /* external cpu mode */
#define EHWCFG_BOOT_NAND   (0 << 0) /* boot from nand */
#define EHWCFG_BOOT_SPI    (1 << 0) /* boot from spi */
#define EHWCFG_BOOT_NOR    (2 << 0) /* boot from nor */

/* General PINMUX */
#define GPMUX_PLLGMAC      (1 << 15) /* PLL = GMAC PLL(PLL2) */
#define GPMUX_EXTIRQ       (1 << 14) /* re-direct irq to external cpu */
#define GPMUX_CS0REL       (1 << 13) /* release CS0 memory space */
#define GPMUX_IOEN         (1 << 12) /* IO output enable */
#define GPMUX_CPUS_START   (1 << 11) /* start slave cpu (fa606te) */
#define GPMUX_SATA_RESET   (1 << 8)
#define GPMUX_PDD          (1 << 7)  /* power-down detection enable */
#define GPMUX_USBH_ALIVE   (1 << 6)
#define GPMUX_USBH_PHYOSC  (1 << 5)
#define GPMUX_OTG_ALIVE    (1 << 4)
#define GPMUX_OTG_PHYOSC   (1 << 3)
#define GPMUX_IRQMASK1     (1 << 2)
#define GPMUX_IRQMASK0     (1 << 1)
#define GPMUX_RESET        (1 << 0)

#define GPMUX_DEFAULT      0x1078 /* USB keep alive + IO output */

/* HCLK gating register */
#define HCLKGR_CPUM        (1 << 31)
#define HCLKGR_CPUS        (1 << 30)
#define HCLKGR_AHBTSIF     (1 << 28)
#define HCLKGR_AHBC3       (1 << 27)
#define HCLKGR_AHBC2       (1 << 26)
#define HCLKGR_AHBC1       (1 << 25)
#define HCLKGR_APBBRG      (1 << 24)
#define HCLKGR_NANDC       (1 << 23)
#define HCLKGR_SMC         (1 << 22)
#define HCLKGR_DMAC1       (1 << 21)
#define HCLKGR_DMAC0       (1 << 20)
#define HCLKGR_H264        (1 << 19)
#define HCLKGR_MPEG4       (1 << 18)
#define HCLKGR_2DGRA       (1 << 17)
#define HCLKGR_LCD         (1 << 16)
#define HCLKGR_ISP         (1 << 15)
#define HCLKGR_AES         (1 << 14)
#define HCLKGR_GMAC        (1 << 13)
#define HCLKGR_SATAH       (1 << 12)
#define HCLKGR_SATAD       (1 << 11)
#define HCLKGR_PCIE        (1 << 10)
#define HCLKGR_USBH        (1 << 9)
#define HCLKGR_OTG         (1 << 8)
#define HCLKGR_SD1         (1 << 7)
#define HCLKGR_SD0         (1 << 6)
#define HCLKGR_IDE         (1 << 5)
#define HCLKGR_EM1         (1 << 4)
#define HCLKGR_EM0         (1 << 3)
#define HCLKGR_IRQ1        (1 << 2)
#define HCLKGR_IRQ0        (1 << 1)
#define HCLKGR_SCU         (1 << 0)

/* Special clock configuration */
#define SCCFG0_SATA_25M       (1 << 29)
#define SCCFG0_SATA_OFF       (1 << 28)
#define SCCFG0_GMAC_CLK_IO    (1 << 27)
#define SCCFG0_GMAC_PLL_OFF   (1 << 26)
#define SCCFG0_GMAC_PLL_NS(x) (((x) & 0x3f) << 20)
#define SCCFG0_ISP_BFREQ(x)   (((x) & 0xf) << 16)
#define SCCFG0_ISP_AFREQ(x)   (((x) & 0xf) << 12)
#define SCCFG0_IDE_FREQ(x)    (((x) & 0xf) << 8)
#define SCCFG0_EXTAHB_FREQ(x) (((x) & 0xf) << 4)
#define SCCFG0_EXTAHB_MASK    0xf0
#define SCCFG0_LCD_SCK_AHB    (0 << 2) /* LCD scalar clock source */
#define SCCFG0_LCD_SCK_APB    (1 << 2)
#define SCCFG0_LCD_SCK_EXT    (2 << 2)
#define SCCFG0_LCD_CK_AHB     (0 << 0) /* LCD clock source */
#define SCCFG0_LCD_CK_APB     (1 << 0)
#define SCCFG0_LCD_CK_EXT     (2 << 0)

#define SCCFG0_DEFAULT        0x26877330

#define SCCFG1_SD1_CK_2AHB    (0 << 18) /* SD1 clock source */
#define SCCFG1_SD1_CK_3APB    (1 << 18) /* SD1 clock source */
#define SCCFG1_SD1_CK_AHB     (2 << 18) /* SD1 clock source */
#define SCCFG1_SD0_CK_2AHB    (0 << 16) /* SD0 clock source */
#define SCCFG1_SD0_CK_3APB    (1 << 16) /* SD0 clock source */
#define SCCFG1_SD0_CK_AHB     (2 << 16) /* SD0 clock source */
#define SCCFG1_SSP1_CK_EXT    (1 << 12) /* SSP1 clock source */
#define SCCFG1_SSP1_FREQ(x)   (((x) & 0xf) << 8)
#define SCCFG1_SSP0_CK_EXT    (1 << 4)  /* SSP0 clock source */
#define SCCFG1_SSP0_FREQ(x)   (((x) & 0xf) << 0)

#define SCCFG1_DEFAULT \
	(SCCFG1_SD1_CK_AHB | SCCFG1_SD0_CK_AHB \
	| SCCFG1_SSP1_FREQ(0xA) | SCCFG1_SSP0_FREQ(0xA))

/* Special clock enable register */
#define SCER_GMAC125M  (1 << 13)
#define SCER_LCDSC     (1 << 12) /* LCD scalar clock */
#define SCER_LCD       (1 << 11)
#define SCER_ISPB      (1 << 10)
#define SCER_ISPA      (1 << 9)
#define SCER_IDE       (1 << 8)
#define SCER_EXTAHB    (1 << 7)
#define SCER_DDRD      (1 << 6)
#define SCER_DDRF      (1 << 5)
#define SCER_SD1       (1 << 4)
#define SCER_SD0       (1 << 3)
#define SCER_SSP1      (1 << 2)
#define SCER_SSP0      (1 << 1)
#define SCER_TSC       (1 << 0)

/* Multi-function pinmux register */
#define MFPMUX0_EBI(x)    (((x) & 0x3) << 10)
#define MFPMUX0_LCD(x)    (((x) & 0x3) << 8)
#define MFPMUX0_TS(x)     (((x) & 0x3) << 6)
#define MFPMUX0_ISP(x)    (((x) & 0x3) << 4)
#define MFPMUX0_SATA(x)   (((x) & 0x3) << 2)
#define MFPMUX0_EXTAHB(x) (((x) & 0x3) << 0)

#define MFPMUX0_DEFAULT   0x241 /* SD0 disabled, SD1 enabled */

#define MFPMUX1_KBC(x)    (((x) & 0x3) << 20)
#define MFPMUX1_GPIO0(x)  (((x) & 0x3) << 18)
#define MFPMUX1_I2C1(x)   (((x) & 0x3) << 16)
#define MFPMUX1_PWM1(x)   (((x) & 0x3) << 14)
#define MFPMUX1_PWM0(x)   (((x) & 0x3) << 12)
#define MFPMUX1_GMAC(x)   (((x) & 0x3) << 10)
#define MFPMUX1_SSP1(x)   (((x) & 0x3) << 8)
#define MFPMUX1_SSP0(x)   (((x) & 0x3) << 6)
#define MFPMUX1_UART3(x)  (((x) & 0x3) << 4)
#define MFPMUX1_UART2(x)  (((x) & 0x3) << 2)
#define MFPMUX1_UART1(x)  (((x) & 0x3) << 0)

/* PLL1 control register */
#define PLLCR_NS(x)    (((x) >> 24) & 0x3f)
#define PLLCR_STABLE   (1 << 1)
#define PLLCR_OFF      (1 << 0)

/* DLL control register */
#define DLLCR_STABLE   (1 << 1)
#define DLLCR_ON       (1 << 0)

#endif
