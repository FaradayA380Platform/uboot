/*
 * drivers/gpio/gpio-ftgpio010.c
 *
 * (C) Copyright 2021 Faraday Technology
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
#include <asm/gpio.h>
#include <asm/io.h>
#include <fdtdec.h>

#define FTGPIO010_OFFSET_DATAOUT        0x00
#define FTGPIO010_OFFSET_DATAIN         0x04
#define FTGPIO010_OFFSET_PINDIR         0x08
#define FTGPIO010_OFFSET_PINPASS        0x0c
#define FTGPIO010_OFFSET_DATASET        0x10
#define FTGPIO010_OFFSET_DATACLEAR      0x14
#define FTGPIO010_OFFSET_PULLEN         0x18
#define FTGPIO010_OFFSET_PULLTYPE       0x1c
#define FTGPIO010_OFFSET_INTR_EN        0x20
#define FTGPIO010_OFFSET_INTR_STATE     0x24
#define FTGPIO010_OFFSET_INTR_MASKED    0x28
#define FTGPIO010_OFFSET_INTR_MASK      0x2c
#define FTGPIO010_OFFSET_INTR_CLEAR     0x30
#define FTGPIO010_OFFSET_INTR_TRIG      0x34
#define FTGPIO010_OFFSET_INTR_BOTH      0x38
#define FTGPIO010_OFFSET_INTR_RISENEG   0x3c
#define FTGPIO010_OFFSET_BOUNCE_EN      0x40
#define FTGPIO010_OFFSET_BOUNCE_PRESC   0x44

#define FTGPIO010_NR_GPIOS              32

struct ftgpio010 {
	void __iomem        *base;
	unsigned int        ngpios;
};

static void ftgpio010_gpio_set(struct ftgpio010 *ftgpio010, unsigned offset, int value)
{
	if (value)
		writel(1 << offset, ftgpio010->base + FTGPIO010_OFFSET_DATASET);
	else
		writel(1 << offset, ftgpio010->base + FTGPIO010_OFFSET_DATACLEAR);
}

static int ftgpio010_gpio_get(struct ftgpio010 *ftgpio010, unsigned offset)
{
	unsigned long val;

	val = readl(ftgpio010->base + FTGPIO010_OFFSET_DATAIN);

	return (val >> offset) & 1;
}

static int ftgpio010_gpio_direction_input(struct udevice *dev, unsigned offset)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);
	unsigned long val;

	val = readl(ftgpio010->base + FTGPIO010_OFFSET_PINDIR);
	val &= ~(1 << offset);
	writel(val, ftgpio010->base + FTGPIO010_OFFSET_PINDIR);

	return 0;
}

static int ftgpio010_gpio_direction_output(struct udevice *dev, unsigned int offset,
					 int value)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);
	unsigned long val;

	val = readl(ftgpio010->base + FTGPIO010_OFFSET_PINDIR);
	val |= 1 << offset;
	writel(val, ftgpio010->base + FTGPIO010_OFFSET_PINDIR);

	ftgpio010_gpio_set(ftgpio010, offset, value);

	return 0;
}

static int ftgpio010_gpio_get_value(struct udevice *dev, unsigned offset)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);

	return ftgpio010_gpio_get(ftgpio010, offset);
}

static int ftgpio010_gpio_set_value(struct udevice *dev, unsigned offset,
				  int value)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);

	ftgpio010_gpio_set(ftgpio010, offset, value);

	return 0;
}

static int ftgpio010_gpio_get_function(struct udevice *dev, unsigned offset)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);
	unsigned long val;

	val = readl(ftgpio010->base + FTGPIO010_OFFSET_PINDIR);
	val &= BIT(offset);
	if (val)
		return GPIOF_OUTPUT;
	else
		return GPIOF_INPUT;
}

static const struct dm_gpio_ops ftgpio010_ops = {
	.direction_input    = ftgpio010_gpio_direction_input,
	.direction_output   = ftgpio010_gpio_direction_output,
	.get_value          = ftgpio010_gpio_get_value,
	.set_value          = ftgpio010_gpio_set_value,
	.get_function       = ftgpio010_gpio_get_function,
};

static void ftgpio010_init(struct ftgpio010 *ftgpio010)
{
	/* input pins for interrupt */
	writel(0, ftgpio010->base + FTGPIO010_OFFSET_PINDIR);

	/* sanitize */
	writel(~0, ftgpio010->base + FTGPIO010_OFFSET_INTR_CLEAR);

	/* unmask all */
	writel(0, ftgpio010->base + FTGPIO010_OFFSET_INTR_MASK);

	/* single-falling edge trigger */
	writel(0, ftgpio010->base + FTGPIO010_OFFSET_INTR_TRIG);
	writel(0, ftgpio010->base + FTGPIO010_OFFSET_INTR_BOTH);
	writel(~0, ftgpio010->base + FTGPIO010_OFFSET_INTR_RISENEG);

	/* enable bounce prescale */
	writel(~0, ftgpio010->base + FTGPIO010_OFFSET_BOUNCE_EN);

	/* enable interrupt */
	writel(~0, ftgpio010->base + FTGPIO010_OFFSET_INTR_EN);
}

static int ftgpio010_probe(struct udevice *dev)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);
	struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);

	uc_priv->bank_name = "GPIO";
	uc_priv->gpio_count = ftgpio010->ngpios;

	ftgpio010_init(ftgpio010);

	return 0;
}

#if CONFIG_IS_ENABLED(OF_CONTROL)
static int ftgpio010_of_to_plat(struct udevice *dev)
{
	struct ftgpio010 *ftgpio010 = dev_get_priv(dev);
	fdt_addr_t addr;

	addr = dev_read_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	ftgpio010->base = (void __iomem *)addr;

	if (dev_read_u32(dev, "ngpios", &ftgpio010->ngpios) < 0) {
		printf("%s: failed to get ngpios, apply default value...\n", __func__);
		ftgpio010->ngpios = FTGPIO010_NR_GPIOS;
	}

	return 0;
}
#endif

static const struct udevice_id ftgpio010_ids[] = {
	{ .compatible = "faraday,ftgpio010" },
	{ }
};

U_BOOT_DRIVER(ftgpio010) = {
	.name       = "ftgpio010",
	.id         = UCLASS_GPIO,
	.of_match   = ftgpio010_ids,
	.of_to_plat = of_match_ptr(ftgpio010_of_to_plat),
	.ops        = &ftgpio010_ops,
	.probe      = ftgpio010_probe,
	.flags      = DM_FLAG_PRE_RELOC,
	.priv_auto  = sizeof(struct ftgpio010),
};