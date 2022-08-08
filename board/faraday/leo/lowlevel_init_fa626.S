/*
 * Board specific setup info
 *
 * (C) Copyright 2010
 * Faraday Technology Inc. <www.faraday-tech.com>
 * Dante Su <dantesu@gmail.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm-offsets.h>
#include <config.h>
#include <common.h>
#include <version.h>

	.macro	_v3_mw32

	ldr r0, =0x1008
	ldr r3, =0x1108
_v3_mw32_loop:
	cmp   r0, r3
	movhs r1, #0
	ldrlo r1, [r0, #0]
	ldrlo r2, [r0, #4]
	teq   r1, #0
	strne r2, [r1, #0]
	addne r0, #8
	bne   _v3_mw32_loop

	.endm	/* _v3_mw32 */

/* Set up the platform, once the cpu has been initialized */
.globl lowlevel_init
lowlevel_init:
	/* Check image header */
	ldr   r1, =0x1000
	ldr   r0, [r1, #0]
	ldr   r1, =0x00484946	/* "FIH\0" */
	ldr   r2, =0x33333639	/* "3369" */
	teq   r0, r1
	teqne r0, r2
	bne _exit
	_v3_mw32

	/*
	 * invalidate i-cache all to make sure the codes bellow
	 * to be fetched into a single 32-bytes cache line
	 */
	mcr p15, 0, r0, c7, c5, 0

	.align 5

_remap_wait:
	str r5, [r0, #0]
	ldr r1, [r0, #0]
	teq r1, r5
	bne _remap_wait		/* while(magic != REG32(addr)) */

_exit:
	mov	pc, lr
