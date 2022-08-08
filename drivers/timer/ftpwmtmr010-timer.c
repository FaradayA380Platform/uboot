/*
 * drivers/timer/ftpwmtmr010.c
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
#include <faraday/ftpwmtmr010.h>

DECLARE_GLOBAL_DATA_PTR;

struct ftpwmtmr010_timer_plat {
	struct ftpwmtmr010 *regs;
};

static u64 ftpwmtmr010_timer_get_count(struct udevice *dev)
{
	struct ftpwmtmr010_timer_plat *priv = dev_get_priv(dev);

	return timer_conv_64(0xffffffff - readl(&priv->regs->timer1_counter));
}

static int ftpwmtmr010_timer_probe(struct udevice *dev)
{
	struct timer_dev_priv *uc_priv = dev_get_uclass_priv(dev);
	struct ftpwmtmr010_timer_plat *priv = dev_get_priv(dev);
	ulong freq = gd->arch.timer_rate_hz;

	uc_priv->clock_rate = freq;

	/* Disable Timer1 */
	writel(0, &priv->regs->timer1_ctrl);

	/* Clear Timer1 interrupts */
	writel(FTPWMTMR010_TM1_OVERFLOW, &priv->regs->interrupt_status);

	/* Setup a longest periodic timer */
	writel(0xffffffff, &priv->regs->timer1_counter);
	writel(0xffffffff, &priv->regs->timer1_load);

	writel(0, &priv->regs->timer1_compare);

	/* Enable Timer1 with overflow interrupt */
	setbits_le32(&priv->regs->timer1_ctrl,
	             FTPWMTMR010_CTRL_AUTO | FTPWMTMR010_CTRL_START);

	return 0;
}

static int ftpwmtmr010_timer_ofdata_to_platdata(struct udevice *dev)
{
	struct ftpwmtmr010_timer_plat *priv = dev_get_priv(dev);

	priv->regs = (void *)devfdt_get_addr(dev);

	return 0;
}

static const struct timer_ops ftpwmtmr010_timer_ops = {
	.get_count = ftpwmtmr010_timer_get_count,
};

static const struct udevice_id ftpwmtmr010_timer_ids[] = {
	{ .compatible = "faraday,ftpwmtmr010-timer" },
	{ }
};

U_BOOT_DRIVER(ftpwmtmr010_timer) = {
	.name = "ftpwmtmr010_timer",
	.id = UCLASS_TIMER,
	.of_match = ftpwmtmr010_timer_ids,
	.of_to_plat = ftpwmtmr010_timer_ofdata_to_platdata,
	.priv_auto = sizeof(struct ftpwmtmr010_timer_plat),
	.probe = ftpwmtmr010_timer_probe,
	.ops = &ftpwmtmr010_timer_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
