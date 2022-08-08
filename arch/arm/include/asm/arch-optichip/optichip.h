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

#ifndef __OPTICHIP_H
#define __OPTICHIP_H

#include <linux/sizes.h>

#ifndef __ASSEMBLY__

#define REG32(off)  *(volatile unsigned long *)(off)
#define REG16(off)  *(volatile unsigned short *)(off)
#define REG8(off)   *(volatile unsigned char *)(off)

#endif /* __ASSEMBLY__ */

/*
 * Hardware register bases
 */
#define CONFIG_FTTMR010_BASE        0x98200000  /*Timer*/
#define CONFIG_FTUART010_BASE       0x98100000  /* UART controller */
#define CONFIG_FTWDT010_BASE        0x98400000  /* Watchdog */
#define CONFIG_SCU_BASE             0x98000000  /* SCU */
#define CONFIG_FTDDRIII030_BASE     0x98700000  /* DDR3 Controller */
#define CONFIG_FTSPI020_BASE        0x90200000
#define CONFIG_FTINTC020_BASE       0xfff00000

#define SYSC_REG_BASE               SCU_BASE

/*
 * Hardware interrupt number
 */
#define NR_IRQS                     64
#define FTTMR010_IRQ         9

#endif	/* __OPTICHIP_H */
