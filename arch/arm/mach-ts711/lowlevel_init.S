/*
 * A lowlevel_init function that sets up the stack to call a C function to
 * perform further init.
 *
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Author :
 *	Aneesh V	<aneesh@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm-offsets.h>
#include <config.h>
#include <linux/linkage.h>
#include <asm/gic.h>
#include <asm/armv7.h>

.macro is_cpu_virt_capable	tmp
	mrc	p15, 0, \tmp, c0, c1, 1		@ read ID_PFR1
	and	\tmp, \tmp, #CPUID_ARM_VIRT_MASK	@ mask virtualization bits
	cmp	\tmp, #(1 << CPUID_ARM_VIRT_SHIFT)
.endm

.macro get_cbar_addr	addr
#ifdef CONFIG_ARM_GIC_BASE_ADDRESS
	ldr	\addr, =CONFIG_ARM_GIC_BASE_ADDRESS
#else
	mrc	p15, 4, \addr, c15, c0, 0	@ read CBAR
	bfc	\addr, #0, #15			@ clear reserved bits
#endif
.endm

.macro get_gicd_addr	addr
	get_cbar_addr	\addr
	add	\addr, \addr, #GIC_DIST_OFFSET	@ GIC dist i/f offset
.endm

.macro get_gicc_addr	addr, tmp
	get_cbar_addr	\addr
	is_cpu_virt_capable \tmp
	movne	\tmp, #GIC_CPU_OFFSET_A9	@ GIC CPU offset for A9
	moveq	\tmp, #GIC_CPU_OFFSET_A15	@ GIC CPU offset for A15/A7
	add	\addr, \addr, \tmp
.endm

#if 0

#else

ENTRY(lowlevel_init)
#ifdef CONFIG_DEFAULT_NONSEC
  mrc     p15, 0, r0, c0, c0, 5  @ check if core 0 , if it was, we don't do cpu mode switch in core 0
  ands    r0, r0, #0xf
  ldr     r1, =0x0
  cmp     r0, r1
  beq skip_core0
        get_gicd_addr	r3
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn1]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn2]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn3]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn4]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn5]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn6]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn7]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn8]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRn9]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRna]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRnb]	@ allow private interrupts
        mvn	r1, #0				@ all bits to 1
        str	r1, [r3, #GICD_IGROUPRnc]	@ allow private interrupts

        get_gicc_addr	r3, r1

        mov	r1, #3				@ Enable both groups
        str	r1, [r3, #GICC_CTLR]		@ and clear all other bits
        mov	r1, #0xff
        str	r1, [r3, #GICC_PMR]		@ set priority mask register
        mrc	p15, 0, r0, c1, c1, 2
        movw	r1, #0x3fff
        movt	r1, #0x0004
        orr	r0, r0, r1
        mcr	p15, 0, r0, c1, c1, 2		@ NSACR = all copros to non-sec
#endif
skip_core0:
        mrc     p15, 0, r0, c0, c0, 5
        ands    r0, r0, #0xf
#ifdef MASTER_CPUID
        ldr     r1, =MASTER_CPUID
        cmp     r0, r1
#endif

#ifdef CONFIG_ARMV7_NONSEC
        bne     _smp_pen
#else
	#bne	_haps62_smp_init
#endif
	mrc	p15, 0, r0, c1, c0, 1
	orr	r0, r0, #(1 << 6)	@ Enable SMP/nAMP mode
	mcr	p15, 0, r0, c1, c0, 1
	ldr	r2, =CONFIG_SMP_PEN_ADDR	@ load start address
	mov	r0, #0
	str	r0, [r2]
	mov	pc, lr
ENDPROC(lowlevel_init)

/*
	.align	5
ENTRY(reset_cpu)
_loop_forever:
	b	_loop_forever
ENDPROC(reset_cpu)
*/

#ifdef CONFIG_SMP_PEN_ADDR
/* void __weak smp_waitloop(unsigned previous_address); */
	.align	5
ENTRY(smp_waitloop)
	ldr	r2, =CONFIG_SMP_PEN_ADDR	@ load start address
smp_loop:
	wfe
	ldr	r1, [r2]
	ldr r0, =CONFIG_SMP_MAGIC
	cmp	r0, r1			@ make sure we dont execute this code
	beq	smp_loop		@ again (due to a spurious wakeup)
	ldr r2, =CONFIG_SMP_PEN_ADDR+4
	ldr	r1, [r2]

#ifdef CONFIG_ARMV7_NONSEC
	mov	r0, r1
	b	_do_nonsec_entry
#else
	mov	pc, r1
#endif
ENDPROC(smp_waitloop)
#endif

/*
 *  1. initialize the GIC per-core interface
 *  2. allow coprocessor access in non-secure modes
 *
 * Called from smp_pen by secondary cores and directly by the BSP.
 * Do not assume that the stack is available and only use registers
 * r0-r3 and r12.
 *
 * PERIPHBASE is used to get the GIC address. This could be 40 bits long,
 * though, but we check this in C before calling this function.
 */
ENTRY(_haps62_smp_init)
#if 0 //kay 20200225 17:37
	cpsid	i
	cpsid	f

	get_gicd_addr	r3

	mov	r1, #0				@ all bits to 0
	str	r1, [r3, #GICD_IGROUPRn]	@ allow private interrupts

	get_gicc_addr	r3, r1

	mov	r1, #3				@ Enable both groups
	str	r1, [r3, #GICC_CTLR]		@ and clear all other bits
	mov	r1, #0xff
	str	r1, [r3, #GICC_PMR]		@ set priority mask register

/* The CNTFRQ register of the generic timer needs to be
 * programmed in secure state. Some primary bootloaders / firmware
 * omit this, so if the frequency is provided in the configuration,
 * we do this here instead.
 * But first check if we have the generic timer.
 */
#ifdef CONFIG_TIMER_CLK_FREQ
	mrc	p15, 0, r0, c0, c1, 1		@ read ID_PFR1
	and	r0, r0, #CPUID_ARM_GENTIMER_MASK	@ mask arch timer bits
	cmp	r0, #(1 << CPUID_ARM_GENTIMER_SHIFT)
	ldreq	r1, =CONFIG_TIMER_CLK_FREQ
	mcreq	p15, 0, r1, c14, c0, 0		@ write CNTFRQ
#endif

	mov	r0, r3				@ return GICC address
	adr	r0, _haps62_smp_init		@ do not use this address again
#endif
	b	smp_waitloop			@ wait for IPIs, board specific
ENDPROC(_haps62_smp_init)

#endif
