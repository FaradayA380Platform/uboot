/*
 * arch/arm/cpu/faraday/fttmr010.c
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <asm/io.h>
#include <faraday/fttmr010.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

static struct fttmr010 *regs = (void __iomem *)FTTMR010_BASE;

void udelay_masked(unsigned long usec)
{
	ulong freq = gd->arch.timer_rate_hz;

	/* Disable Timer2 */
	clrbits_le32(&regs->cr, FTTMR010_TM2_CRMASK);
	/* Disable Timer2 interrupts */
	writel(FTTMR010_TM2_ISRMASK, &regs->interrupt_mask);
	/* Clear Timer2 interrupts */
	writel(FTTMR010_TM2_ISRMASK, &regs->interrupt_state);

	/* Configure Timer2 */
	writel((freq / 1000000) * usec, &regs->timer2_counter);
	writel(0, &regs->timer2_load);
	writel(0, &regs->timer2_match1);
	writel(0, &regs->timer2_match2);

	/* Enable Timer2 */
	setbits_le32(&regs->cr,
		FTTMR010_TM2_OFENABLE | FTTMR010_TM2_ENABLE);

	/* Wait until timeout */
	while (!(readl(&regs->interrupt_state) & FTTMR010_TM2_ISRMASK))
		;
}

void reset_timer_masked(void)
{
	ulong freq = gd->arch.timer_rate_hz;

	/* Disable Timer1 */
	clrbits_le32(&regs->cr, FTTMR010_TM1_CRMASK);

	/* Disable & Clear Timer1 interrupts */
	writel(FTTMR010_TM1_ISRMASK, &regs->interrupt_mask);
	writel(FTTMR010_TM1_ISRMASK, &regs->interrupt_state);

	/* Setup a longest periodic timer */
	writel((0xffffffff / freq) * freq, &regs->timer1_counter);
	writel((0xffffffff / freq) * freq, &regs->timer1_load);

	writel(0, &regs->timer1_match1);
	writel(0, &regs->timer1_match2);

	/* Disable match interrupts */
	writel(FTTMR010_TM1_MATCH1 | FTTMR010_TM1_MATCH2,
		&regs->interrupt_mask);

	/* Enable Timer1 with overflow interrupt */
	setbits_le32(&regs->cr,
		FTTMR010_TM1_OFENABLE | FTTMR010_TM1_ENABLE);
}

ulong get_timer_masked(void)
{
	ulong freq = gd->arch.timer_rate_hz;
	ulong secs = 0xffffffff / freq;
	ulong ms = freq / CONFIG_SYS_HZ;

	if (readl(&regs->interrupt_state) & FTTMR010_TM1_ISRMASK) {
		writel(FTTMR010_TM1_ISRMASK, &regs->interrupt_state);
		gd->arch.tbl += secs * CONFIG_SYS_HZ;
	}

	return gd->arch.tbl
		+ ((secs * freq) - readl(&regs->timer1_counter)) / ms;
}

int timer_init(void)
{
	gd->arch.tbl = 0;
	reset_timer_masked();
	return 0;
}

void reset_timer(void)
{
	reset_timer_masked();
}

ulong get_timer(ulong base)
{
	return get_timer_masked() - base;
}

void __udelay(unsigned long usec)
{
	udelay_masked(usec);
}


/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned __weak long long get_ticks(void)
{
	return get_timer(0);
}

unsigned __weak long long clock(void)
{
  return get_ticks();
}
#
/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong __weak get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
#
