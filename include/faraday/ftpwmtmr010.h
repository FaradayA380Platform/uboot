/*
 * (C) Copyright 2009 Faraday Technology
 * Po-Yu Chuang <ratbert@faraday-tech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Timer
 */
#ifndef __FTPWMTMR010_H
#define __FTPWMTMR010_H

struct ftpwmtmr010 {
	unsigned int	interrupt_status;   /* 0x00 */
	unsigned int	revd[3];
	unsigned int	timer1_ctrl;        /* 0x10 */
	unsigned int	timer1_load;        /* 0x14 */
	unsigned int	timer1_compare;     /* 0x18 */
	unsigned int	timer1_counter;     /* 0x1c */
	unsigned int	timer2_ctrl;        /* 0x20 */
	unsigned int	timer2_load;        /* 0x24 */
	unsigned int	timer2_compare;     /* 0x28 */
	unsigned int	timer2_counter;     /* 0x2c */
	unsigned int	timer3_ctrl;        /* 0x30 */
	unsigned int	timer3_load;        /* 0x34 */
	unsigned int	timer3_compare;     /* 0x38 */
	unsigned int	timer3_counter;     /* 0x3c */
	unsigned int	timer4_ctrl;        /* 0x40 */
	unsigned int	timer4_load;        /* 0x44 */
	unsigned int	timer4_compare;     /* 0x48 */
	unsigned int	timer4_counter;     /* 0x4c */
	unsigned int	timer5_ctrl;        /* 0x50 */
	unsigned int	timer5_load;        /* 0x54 */
	unsigned int	timer5_compare;     /* 0x58 */
	unsigned int	timer5_counter;     /* 0x5c */
	unsigned int	timer6_ctrl;        /* 0x60 */
	unsigned int	timer6_load;        /* 0x64 */
	unsigned int	timer6_compare;     /* 0x68 */
	unsigned int	timer6_counter;     /* 0x6c */
	unsigned int	timer7_ctrl;        /* 0x70 */
	unsigned int	timer7_load;        /* 0x74 */
	unsigned int	timer7_compare;     /* 0x78 */
	unsigned int	timer7_counter;     /* 0x7c */
	unsigned int	timer8_ctrl;        /* 0x80 */
	unsigned int	timer8_load;        /* 0x84 */
	unsigned int	timer8_compare;     /* 0x88 */
	unsigned int	timer8_counter;     /* 0x8c */
	unsigned int	revision;           /* 0x90 */
};

/*
 * Timer Control Register
 */
#define FTPWMTMR010_CTRL_DZ             (((x) & 0xff) << 24)
#define FTPWMTMR010_CTRL_PWM_EN         (1 << 8)
#define FTPWMTMR010_CTRL_DMA_EN         (1 << 7)
#define FTPWMTMR010_CTRL_INT_MODE       (1 << 6)
#define FTPWMTMR010_CTRL_INT_EN         (1 << 5)
#define FTPWMTMR010_CTRL_AUTO           (1 << 4)
#define FTPWMTMR010_CTRL_OUT_INV        (1 << 3)
#define FTPWMTMR010_CTRL_UPDATE         (1 << 2)
#define FTPWMTMR010_CTRL_START          (1 << 1)
#define FTPWMTMR010_CTRL_SRC            (1 << 0)

/*
 * Timer Interrupt Status
 */
#define FTPWMTMR010_TM8_OVERFLOW        (1 << 7)
#define FTPWMTMR010_TM7_OVERFLOW        (1 << 6)
#define FTPWMTMR010_TM6_OVERFLOW        (1 << 5)
#define FTPWMTMR010_TM5_OVERFLOW        (1 << 4)
#define FTPWMTMR010_TM4_OVERFLOW        (1 << 3)
#define FTPWMTMR010_TM3_OVERFLOW        (1 << 2)
#define FTPWMTMR010_TM2_OVERFLOW        (1 << 1)
#define FTPWMTMR010_TM1_OVERFLOW        (1 << 0)

#endif	/* __FTPWMTMR010_H */
