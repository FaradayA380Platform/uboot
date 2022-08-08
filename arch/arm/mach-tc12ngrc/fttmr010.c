#include <common.h>
#include <timer.h>
#include <asm/io.h>
#include <faraday/fttmr010.h>

DECLARE_GLOBAL_DATA_PTR;

struct fttmr010 *regs;

unsigned long timer_read_counter(void)
{
	return readl(&regs->timer1_counter);
}

int timer_init (void)
{
	ulong freq = gd->arch.timer_rate_hz;

	regs = (void *)PLATFORM_FTTMR010_BASE;

	/* Disable Timer1 */
	clrbits_le32(&regs->cr, FTTMR010_TM1_CRMASK);

	/* Disable & Clear Timer1 interrupts */
	writel(FTTMR010_TM1_ISRMASK, &regs->interrupt_mask);
	writel(FTTMR010_TM1_ISRMASK, &regs->interrupt_state);

	/* Setup a longest periodic timer */
	writel(0, &regs->timer1_counter);
	writel(0, &regs->timer1_load);

	writel((0xffffffff / freq) * freq, &regs->timer1_match1);
	writel((0xffffffff / freq) * freq, &regs->timer1_match2);

	/* Disable match interrupts */
	writel(FTTMR010_TM1_MATCH1 | FTTMR010_TM1_MATCH2,
	       &regs->interrupt_mask);

	/* Enable Timer1 with overflow interrupt */
	setbits_le32(&regs->cr,
	             FTTMR010_TM1_OFENABLE | FTTMR010_TM1_ENABLE | FTTMR010_TM1_UPDOWN);

	return 0;
}