/*
 * Faraday LCD Controller
 *
 * (C) Copyright 2010 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#ifndef __FTLCDC200_H
#define __FTLCDC200_H

/* FTLCDC200 Registers */
struct ftlcdc200_regs {
	/* 0x000 ~ 0x0ff */
	uint32_t fer;  /* 0x000: Function Enable Register */
	uint32_t ppr;  /* 0x004: Panel Pixel Register */
	uint32_t ier;  /* 0x008: Interrupt Enable Register */
	uint32_t iscr; /* 0x00C: Interrupt Status Clear Register */
	uint32_t isr;  /* 0x010: Interrupt Status Register */
	uint32_t rsvd0[1];
	uint32_t fb0;  /* 0x018: Framebuffer Base Register 0 */
	uint32_t rsvd1[2];
	uint32_t fb1;  /* 0x024: Framebuffer Base Register 1 */
	uint32_t rsvd2[2];
	uint32_t fb2;  /* 0x030: Framebuffer Base Register 2 */
	uint32_t rsvd3[2];
	uint32_t fb3;  /* 0x03C: Framebuffer Base Register 3 */
	uint32_t rsvd4[2];
	uint32_t patg; /* 0x048: Pattern Generator */
	uint32_t fifo; /* 0x04C: FIFO Threshold */
	uint32_t gpio; /* 0x050: GPIO */
	uint32_t rsvd5[43];

	/* 0x100 ~ 0x1ff */
	uint32_t htcr;    /* Horizontal Timing Control Register */
	uint32_t vtcr[2]; /* Vertical Timing Control Register */
	uint32_t pcr;     /* Polarity Control Register */
	uint32_t rsvd6[60];

	/* 0x200 ~ 0x2ff */
	uint32_t sppr;    /* Serial Panel Pixel Register */
	uint32_t ccir;    /* CCIR565 Register */
	uint32_t rsvd7[62];

	/* 0x300 ~ 0x3ff */
	uint32_t pipr;    /* Picture-In-Picture Register */
	uint32_t pip1pos; /* Sub-picture 1 position */
	uint32_t pip1dim; /* Sub-picture 1 dimension */
	uint32_t pip2pos; /* Sub-picture 2 position */
	uint32_t pip2dim; /* Sub-picture 2 dimension */
	uint32_t rsvd8[59];

	/* 0x400 ~ 0x5ff */
	uint32_t cmnt[4]; /* Color Management */
	uint32_t rsvd9[124];

	/* 0x600 ~ 0x6ff */
	uint32_t gamma_r[64]; /* RED - Gamma Correct */

	/* 0x700 ~ 0x7ff */
	uint32_t gamma_g[64]; /* GREEN - Gamma Correct */

	/* 0x800 ~ 0x8ff */
	uint32_t gamma_b[64]; /* BLUE - Gamma Correct */

	/* 0x900 ~ 0x9ff */
	uint32_t rsvd10[64];

	/* 0xa00 ~ 0xbff */
	uint32_t palette[128];  /* Palette Write Port */

	/* 0xc00 ~ 0xcff */
	uint32_t cstn_cr;       /* CSTN Control Register */
	uint32_t cstn_pr;       /* CSTN Parameter Register */
	uint32_t rsvd11[62];

	/* 0xd00 ~ 0xdff */
	uint32_t cstn_bmap[16]; /* CSTN bitmap write port */
	uint32_t rsvd12[48];
};

/* LCD Function Enable Register */
#define FER_EN          (1 << 0)    /* chip enabled */
#define FER_ON          (1 << 1)    /* screen on */
#define FER_YUV420      (3 << 2)
#define FER_YUV422      (2 << 2)
#define FER_YUV         (1 << 3)    /* 1:YUV, 0:RGB */

/* LCD Panel Pixel Register */
#define PPR_BPP_1       (0 << 0)
#define PPR_BPP_2       (1 << 0)
#define PPR_BPP_4       (2 << 0)
#define PPR_BPP_8       (3 << 0)
#define PPR_BPP_16      (4 << 0)
#define PPR_BPP_24      (5 << 0)
#define PPR_BPP_MASK    (7 << 0)
#define PPR_PWROFF      (1 << 3)
#define PPR_BGR         (1 << 4)
#define PPR_ENDIAN_LBLP (0 << 5)
#define PPR_ENDIAN_BBBP (1 << 5)
#define PPR_ENDIAN_LBBP (2 << 5)
#define PPR_ENDIAN_MASK (3 << 5)
#define PPR_RGB1        (PPR_BPP_1)
#define PPR_RGB2        (PPR_BPP_2)
#define PPR_RGB4        (PPR_BPP_4)
#define PPR_RGB8        (PPR_BPP_8)
#define PPR_RGB12       (PPR_BPP_16 | (2 << 7))
#define PPR_RGB16_555   (PPR_BPP_16 | (1 << 7))
#define PPR_RGB16_565   (PPR_BPP_16 | (0 << 7))
#define PPR_RGB24       (PPR_BPP_24)
#define PPR_RGB32       (PPR_BPP_24)
#define PPR_RGB_MASK    (PPR_BPP_MASK | (3 << 7))
#define PPR_VCOMP_VSYNC (0 << 9)
#define PPR_VCOMP_VBP   (1 << 9)
#define PPR_VCOMP_VAIMG (2 << 9)
#define PPR_VCOMP_VFP   (3 << 9)
#define PPR_VCOMP_MASK  (3 << 9)
#define	PPR_PANEL_6BIT  (0 << 11)
#define	PPR_PANEL_8BIT  (1 << 11)
#define	PPR_DITHER565   (0 << 12)
#define	PPR_DITHER555   (1 << 12)
#define	PPR_DITHER444   (2 << 12)
#define	PPR_HCLK_RESET  (1 << 14)
#define	PPR_LCCLK_RESET (1 << 15)

/* LCD Interrupt Enable Register */
#define IER_FIFOUR      (1 << 0)
#define IER_NEXTFB      (1 << 1)
#define IER_VCOMP       (1 << 2)
#define IER_BUSERR      (1 << 3)

/* LCD Interrupt Status Register */
#define ISR_FIFOUR      (1 << 0)
#define ISR_NEXTFB      (1 << 1)
#define ISR_VCOMP       (1 << 2)
#define ISR_BUSERR      (1 << 3)

/* LCD Horizontal Timing Control Register */
#define HTCR_HBP(x)     ((((x) - 1) & 0xff) << 24)
#define HTCR_HFP(x)     ((((x) - 1) & 0xff) << 16)
#define HTCR_HSYNC(x)   ((((x) - 1) & 0xff) << 8)
#define HTCR_PL(x)      (((x >> 4) - 1) & 0xff)

/* LCD Vertical Timing Control Register 0 */
#define VTCR0_VFP(x)    (((x) & 0xff) << 24)
#define VTCR0_VSYNC(x)  ((((x) - 1) & 0x3f) << 16)
#define VTCR0_LF(x)     (((x) - 1) & 0xfff)

/* LCD Vertical Timing Control Register 1 */
#define VTCR1_VBP(x)    ((x) & 0xff)

/* LCD Polarity Control Register */
#define PCR_IVS         (1 << 0)
#define PCR_IHS         (1 << 1)
#define PCR_ICK         (1 << 2)
#define PCR_IDE         (1 << 3)
#define PCR_IPWR        (1 << 4)
#define PCR_DIV(x)      ((((x) - 1) & 0x7f) << 8)

/* LCD Serial Panel Pixel Register */
#define SPPR_SERIAL     (1 << 0)
#define SPPR_DELTA      (1 << 1)
#define SPPR_CS(x)      ((x) << 2)
#define SPPR_CS_RGB     (0 << 2)
#define SPPR_CS_BRG     (1 << 2)
#define SPPR_CS_GBR     (2 << 2)
#define SPPR_LSR        (1 << 4)
#define SPPR_AUO052     (1 << 5)

/* LCD CCIR656 Register */
#define CCIR_PAL        (0 << 0)
#define CCIR_NTSC       (1 << 0)
#define CCIR_P640       (0 << 1)
#define CCIR_P720       (1 << 1)
#define CCIR_PHASE(x)   ((x) << 2)

#endif /* __FTLCDC200_H */
