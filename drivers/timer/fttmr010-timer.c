/*
 * drivers/timer/fttmr010.c
 *
 * (C) Copyright 2017 Faraday Technology
 * B.C. Chen <bcchen@faraday-tech.com>
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
#include <dm.h>
#include <errno.h>
#include <timer.h>
#include <asm/io.h>
#include <faraday/fttmr010.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

struct fttmr010_timer_plat {
	struct fttmr010 *regs;
};

static u64 fttmr010_timer_get_count(struct udevice *dev)
{
	struct fttmr010_timer_plat *priv = dev_get_priv(dev);

	return timer_conv_64(readl(&priv->regs->timer1_counter));
}

static int fttmr010_timer_probe(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct fttmr010_timer_plat *priv = dev_get_priv(dev);
	ulong freq = gd->arch.timer_rate_hz;

	uc_priv->clock_rate = freq;

	/* Disable Timer1 */
	clrbits_le32(&priv->regs->cr, FTTMR010_TM1_CRMASK);

	/* Disable & Clear Timer1 interrupts */
	writel(FTTMR010_TM1_ISRMASK, &priv->regs->interrupt_mask);
	writel(FTTMR010_TM1_ISRMASK, &priv->regs->interrupt_state);

	/* Setup a longest periodic timer */
	writel(0, &priv->regs->timer1_counter);
	writel(0, &priv->regs->timer1_load);

	writel((0xffffffff / freq) * freq, &priv->regs->timer1_match1);
	writel((0xffffffff / freq) * freq, &priv->regs->timer1_match2);

	/* Disable match interrupts */
	writel(FTTMR010_TM1_MATCH1 | FTTMR010_TM1_MATCH2,
	       &priv->regs->interrupt_mask);

	/* Enable Timer1 with overflow interrupt */
	setbits_le32(&priv->regs->cr,
	             FTTMR010_TM1_OFENABLE | FTTMR010_TM1_ENABLE | FTTMR010_TM1_UPDOWN);

	return 0;
}

static int fttmr010_timer_ofdata_to_platdata(struct udevice *dev)
{
	struct fttmr010_timer_plat *priv = dev_get_priv(dev);

	priv->regs = (void *)devfdt_get_addr(dev);

	return 0;
}

static const struct timer_ops fttmr010_timer_ops = {
	.get_count = fttmr010_timer_get_count,
};

static const struct udevice_id fttmr010_timer_ids[] = {
	{ .compatible = "faraday,fttmr010-timer" },
	{ }
};

U_BOOT_DRIVER(fttmr010_timer) = {
	.name = "fttmr010_timer",
	.id = UCLASS_TIMER,
	.of_match = fttmr010_timer_ids,
	.of_to_plat = fttmr010_timer_ofdata_to_platdata,
	.priv_auto = sizeof(struct fttmr010_timer_plat),
	.probe = fttmr010_timer_probe,
	.ops = &fttmr010_timer_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
