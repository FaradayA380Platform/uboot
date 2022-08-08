/*
 * Faraday SPI Flash Controller
 *
 * (C) Copyright 2010 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#ifndef _FTSPI020_H
#define _FTSPI020_H

/*
 * FTSPI020 Registers
 */
struct ftspi020_regs {
	/* 0x000 ~ 0x0fc */
	uint32_t	cmd[4];	/* Command Register */
	uint32_t	cr;		/* Control Register */
	uint32_t	atr;	/* AC Timing Register */
	uint32_t	sr;		/* Status Register */
	uint32_t	rsvd0[1];
	uint32_t	ier;	/* Interrupt Enable Register */
	uint32_t	isr;	/* Interrupt Status Register */
	uint32_t	rdsr;	/* Read Status Register */
	uint32_t	rsvd1[9];
	uint32_t	revr;	/* Revision Register */
	uint32_t	fear;	/* Feature Register */
	uint32_t	rsvd2[42];

	/* 0x100 ~ 0x1fc */
	uint32_t	dr;		/* Data Register */
};

/*
 * Control Register offset 0x10
 */
#define CR_READY_LOC_MASK   ~(0x7 << 16)
#define CR_READY_LOC(x)     (((x) & 0x7) << 16)
#define CR_ABORT            BIT_MASK(8)
#define CR_CLK_MODE_0       0
#define CR_CLK_MODE_3       BIT_MASK(4)
#define CR_CLK_DIVIDER_MASK ~(3 << 0)
#define CR_CLK_DIVIDER_2    (0 << 0)
#define CR_CLK_DIVIDER_4    (1 << 0)
#define CR_CLK_DIVIDER_6    (2 << 0)
#define CR_CLK_DIVIDER_8    (3 << 0)

/*
 * Status Register offset 0x18
 */
#define SR_RFR              BIT_MASK(1) /* RX FIFO Ready */
#define SR_TFR              BIT_MASK(0) /* TX FIFO Ready */

/*
 * Interrupt Control Register
 */
#define ICR_RFTH(x)         (((x) & 0x3) << 12) /* RX FIFO Threshold */
#define ICR_TFTH(x)         (((x) & 0x3) << 8)  /* TX FIFO Threshold */
#define ICR_DMA             BIT_MASK(0) /* DMA HW Handshake Enable */

/*
 * Interrupt Status Register
 */
#define ISR_CMD             BIT_MASK(0) /* Command Complete/Finish  */

/*
 * Feature Register
 */
#define FEAR_CLK_MODE(reg)       (((reg) >> 25) & 0x1)
#define FEAR_DTR_MODE(reg)       (((reg) >> 24) & 0x1)
#define FEAR_CMDQ_DEPTH(reg)     (((reg) >> 16) & 0xff)
#define FEAR_RXFIFO_DEPTH(reg)   (((reg) >>  8) & 0xff)
#define FEAR_TXFIFO_DEPTH(reg)   (((reg) >>  0) & 0xff)

/*
 * CMD1 Register offset 4: Command Queue Second Word
 */
#define CMD1_CREAD               BIT_MASK(28)
#define CMD1_ILEN(x)             (((x) & 0x03) << 24)
#define CMD1_DCYC(x)             (((x) & 0xff) << 16)
#define CMD1_ALEN(x)             ((x) & 0x07)

/*
 * CMD3 Register offset 0xc: Command Queue Fourth Word
 */
#define CMD3_OPC(x)              (((x) & 0xff) << 24)
#define CMD3_OPC_CREAD(x)        (((x) & 0xff) << 16)
#define CMD3_CS(x)               (((x) & 0x3) << 8)
#define CMD3_SERIAL              (0 << 5)
#define CMD3_DUAL                (1 << 5)
#define CMD3_QUAD                (2 << 5)
#define CMD3_DUAL_IO             (3 << 5)
#define CMD3_QUAD_IO             (4 << 5)

#define CMD3_DTR                 BIT_MASK(4)

#define CMD3_RDST_SW             BIT_MASK(3)
#define CMD3_RDST_HW             0

#define CMD3_RDST                BIT_MASK(2)

#define CMD3_WRITE               BIT_MASK(1)
#define CMD3_READ                0

#ifndef CONFIG_FTSPI020_V1_1_0
#define CMD3_CMDIRQ              BIT_MASK(0)
#else
#define CMD3_CMDIRQ              0
#endif

#endif
