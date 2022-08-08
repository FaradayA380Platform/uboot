/*
 * (C) Copyright 2009 Faraday Technology
 * Po-Yu Chuang <ratbert@faraday-tech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Timer
 */
#ifndef __ALADDINTIMER_H
#define __ALADDINTIMER_H

struct titmr {
	unsigned int	timer1_load;		/* 0x00 */
	unsigned int	timer1_val;		/* 0x04 */
	unsigned int	timer1_cr;		/* 0x08 */
	unsigned int	int_clr;		/* 0x0c */
	unsigned int	raw_int;		/* 0x10 */
	unsigned int	mask_int;		/* 0x14 */
	unsigned int	extend_mode_cr;		/* 0x18 */
};

/*
 * Timer Control Register
 */
#define TITIMER_ONESHOT					(1 << 0)
#define TITIMER_WRAPPING				(0 << 0)
#define TITIMER_32BIT						(1 << 1)
#define TITIMER_16BIT						(0 << 1)
#define TITIMER_1_1_PRESCALER		(0 << 2)
#define TITIMER_1_16_PRESCALER	(1 << 2)
#define TITIMER_1_256_PRESCALER	(2 << 2)
#define TITIMER_NO_PRESCALER		(3 << 2)
#define TITIMER_INT_EN					(1 << 5)
#define TITIMER_ENABLE					(1 << 7)

#endif