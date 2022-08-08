/*
 * (C) Copyright 2009
 * Vipin Kumar, ST Micoelectronics, vipin.kumar@st.com.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include "timer.h"

DECLARE_GLOBAL_DATA_PTR;

#define TIMER_BASE 0x90010000 
#define TIMER3_BASE 0x900C0000 


static ulong ticks =0;				/* U-Boot ticks since startup */
static ulong sclk  = 12000000;	/* source clock (66MHz by default) */
static struct titmr __iomem *regs = (void __iomem *)TIMER_BASE;
static struct titmr __iomem *regs1 = (void __iomem *)TIMER3_BASE;


int timer_init(void)
{
	reset_timer_masked();
	return 0;
}
unsigned long long get_ticks(void)
{
	
	ulong now = readl(&regs->timer1_val) & 0xffffffff;

	if (gd->arch.lastinc >= now) {
		/* normal mode */
		gd->arch.tbl += gd->arch.lastinc - now;
	} else {
		/* we have an overflow ... */
		gd->arch.tbl += gd->arch.lastinc + gd->arch.tbu - now;
	}
	gd->arch.lastinc = now;

	return gd->arch.tbl;
}
ulong get_timer_masked(void)
{
	ulong tmr = get_ticks();

	return tmr / (gd->arch.timer_rate_hz / CONFIG_SYS_HZ);
}
ulong get_timer(ulong base)
{
	ulong val;   
	val = get_timer_masked();

	return val-base;
}

void reset_timer_masked(void)
{	
	writel(0x000000E3, &regs1->timer1_cr); 
	writel(0xFFFFFFFF, &regs1->timer1_load);   
	writel(0x000000E3, &regs->timer1_cr); 
	writel(0xffffFFFF, &regs->timer1_load);   	
}


void udelay_masked(unsigned long usec)
{
	writel(0, &regs1->timer1_cr);
	writel(0, &regs1->int_clr);

	writel(0x000000E3, &regs1->timer1_cr); 
	writel(((sclk/(1*1000*1000))*usec), &regs1->timer1_load);   

	while (!readl(&regs1->raw_int))
		;

}


void __udelay (unsigned long usec)
{
	udelay_masked(usec);
}


/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
