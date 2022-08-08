/*
 * (C) Copyright 2012 Faraday Technology
 * Bing-Yao Luo <bjluo@faraday-tech.com>
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

#include <config.h>
#include <asm/arch/vango.h>
#include <asm/arch/xparameters_ps.h>


.globl lowlevel_init
lowlevel_init:

_exit:
	mov	pc, lr

	#This code will be moved to address 0 for CPU1 wake up
secondary:
	adr 	r1, base_addr
	ldr 	r2, [r1]
	ldr 	r3, [r1, #4]

	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache
	mcr	p15, 0, r0, c7, c5, 6	@ invalidate BP array
	dsb
	isb

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00000002	@ set bit 1 (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-cache
	mcr	p15, 0, r0, c1, c0, 0
	isb	sy

_loop:
	ldr 	r1, [r3, #0x100]
	cmp	r1, #0
	movne	pc, r1

	mov	r1, #0x1
	str	r1, [r2, #GIC_CPU_CTRL]
	mov	r1, #0xf0
	str	r1, [r2, #GIC_CPU_PRIMASK]

	/* ensure the following code reside in the same I-cache line */
	.align 5

	/* wait for interrupt */
	wfi

	mov	r1, #0x0
	str	r1, [r2, #GIC_CPU_CTRL]
	mov	r1, #0x0
	str	r1, [r2, #GIC_CPU_PRIMASK]

	b	_loop

base_addr:
	.long	GIC_CPU_BASE
	.long	CONFIG_SCU_BASE
	.long	0xFFFFFFFF
