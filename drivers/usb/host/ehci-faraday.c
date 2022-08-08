/*
 * Faraday USB 2.0 EHCI Controller
 *
 * (C) Copyright 2010 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <usb.h>
#include <usb/fusbh200.h>
#include <usb/fotg210.h>
#include <linux/delay.h>
#include "ehci.h"

union ehci_faraday_regs {
	struct fusbh200_regs usb;
	struct fotg210_regs  otg;
};

struct faraday_ehci {
	struct ehci_ctrl ctrl;
	union ehci_faraday_regs *reg;	/* address of registers in physical memory */
};

static inline int ehci_is_fotg2xx(union ehci_faraday_regs *regs)
{
	return !readl(&regs->usb.easstr);
}

void faraday_ehci_set_usbmode(struct ehci_ctrl *ctrl)
{
	/* nothing needs to be done */
}

int faraday_ehci_get_port_speed(struct ehci_ctrl *ctrl, uint32_t reg)
{
	int spd, ret = PORTSC_PSPD_HS;
	union ehci_faraday_regs *regs;

	regs = (void __iomem *)((ulong)ctrl->hcor - 0x10);
	if (ehci_is_fotg2xx(regs))
		spd = OTGCSR_SPD(readl(&regs->otg.otgcsr));
	else
		spd = BMCSR_SPD(readl(&regs->usb.bmcsr));

	switch (spd) {
	case 0:    /* full speed */
		ret = PORTSC_PSPD_FS;
		break;
	case 1:    /* low  speed */
		ret = PORTSC_PSPD_LS;
		break;
	case 2:    /* high speed */
		ret = PORTSC_PSPD_HS;
		break;
	default:
		printf("ehci-faraday: invalid device speed\n");
		break;
	}

	return ret;
}

uint32_t *faraday_ehci_get_portsc_register(struct ehci_ctrl *ctrl, int port)
{
	/* Faraday EHCI has one and only one portsc register */
	if (port) {
		/* Printing the message would cause a scan failure! */
		debug("The request port(%d) is not configured\n", port);
		return NULL;
	}

	/* Faraday EHCI PORTSC register offset is 0x20 from hcor */
	return (uint32_t *)((uint8_t *)ctrl->hcor + 0x20);
}

/*
 * Create the appropriate control structures to manage
 * a new EHCI host controller.
 */
void usb_common_init(struct faraday_ehci *config)
{
	union ehci_faraday_regs *regs;

	regs = (void __iomem *)config->reg;

	if (ehci_is_fotg2xx(regs)) {
		/* A-device bus reset */
		/* ... Power off A-device */
		setbits_le32(&regs->otg.otgcsr, OTGCSR_A_BUSDROP);
		/* ... Drop vbus and bus traffic */
		clrbits_le32(&regs->otg.otgcsr, OTGCSR_A_BUSREQ);
		mdelay(1);
		/* ... Power on A-device */
		clrbits_le32(&regs->otg.otgcsr, OTGCSR_A_BUSDROP);
		/* ... Drive vbus and bus traffic */
		setbits_le32(&regs->otg.otgcsr, OTGCSR_A_BUSREQ);
		mdelay(1);
		/* Disable OTG & DEV interrupts, triggered at level-high/level-low */
#if defined(CONFIG_ARCH_A380) || defined(CONFIG_ARCH_HGU)
		writel(IMR_IRQLL | IMR_OTG | IMR_DEV, &regs->otg.imr);
#else
		writel(IMR_IRQLH | IMR_OTG | IMR_DEV, &regs->otg.imr);
#endif
		/* Clear all interrupt status */
		writel(ISR_HOST | ISR_OTG | ISR_DEV, &regs->otg.isr);
	} else {
		/* Interrupt=level-high */
		setbits_le32(&regs->usb.bmcsr, BMCSR_IRQLH);
		/* VBUS on */
		clrbits_le32(&regs->usb.bmcsr, BMCSR_VBUS_OFF);
		/* Disable all interrupts */
		writel(0x00, &regs->usb.bmier);
		writel(0x1f, &regs->usb.bmisr);
	}
}

static const struct ehci_ops faraday_ehci_ops = {
	.set_usb_mode		= faraday_ehci_set_usbmode,
	.get_port_speed		= faraday_ehci_get_port_speed,
	.get_portsc_register	= faraday_ehci_get_portsc_register,
};

static int ehci_usb_ofdata_to_platdata(struct udevice *dev)
{
	struct faraday_ehci *priv = dev_get_priv(dev);

	priv->reg = (union ehci_faraday_regs *)devfdt_get_addr(dev);

	return 0;
}

static int ehci_usb_probe(struct udevice *dev)
{
	struct faraday_ehci *priv = dev_get_priv(dev);
	struct ehci_hccr *hccr;
	struct ehci_hcor *hcor;

	usb_common_init(priv);

	hccr = (struct ehci_hccr *)&priv->reg->usb;;
	hcor = (struct ehci_hcor *)((uintptr_t)hccr + HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	return ehci_register(dev, hccr, hcor, &faraday_ehci_ops, 0, USB_INIT_HOST);
}

static const struct udevice_id ehci_usb_ids[] = {
	{ .compatible = "faraday-ehci" },
	{ }
};

U_BOOT_DRIVER(ehci_faraday) = {
	.name	= "ehci_faraday",
	.id	= UCLASS_USB,
	.of_match = ehci_usb_ids,
	.of_to_plat = ehci_usb_ofdata_to_platdata,
	.probe = ehci_usb_probe,
	.remove = ehci_deregister,
	.ops	= &ehci_usb_ops,
	.priv_auto = sizeof(struct faraday_ehci),
	.flags	= DM_FLAG_ALLOC_PRIV_DMA,
};

