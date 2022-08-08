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
#define FTTMR010_BASE        0x90010000  /*Timer*/
#define FTUART010_BASE       0x90020000  /* UART controller */
#define FTWDT010_BASE        0x90060000  /* Watchdog */
#define FTUART010_1_BASE     0x90070000  /* UART controller */
#define SYS_RTC_PL031_BASE   0x90090000
#define SCU_BASE             0x90140000  /* SCU */
#define FTDDRIII030_BASE     0x90120000  /* DDR3 Controller */
#define FTSPI020_BASE        0xB8000000

#define SYSC_REG_BASE               SCU_BASE

/*
 * Hardware interrupt number
 */
#define NR_IRQS                     64
#define FTTMR010_IRQ         17

#endif	/* __ALADDIN_H */
