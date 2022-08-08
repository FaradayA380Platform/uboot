/*
 * (C) Copyright 2013 - 2015 Xilinx, Inc.
 *
 * Xilinx Zynq SD Host Controller Interface
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <fdtdec.h>
//#include <libfdt.h>
#include <sdhci.h>

#include <asm/arch/clock.h>

struct ftsdc021_sdhci_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

static int ftsdc021_sdhci_probe(struct udevice *dev)
{
	struct ftsdc021_sdhci_plat *plat = dev_get_plat(dev);
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct sdhci_host *host = dev_get_priv(dev);
	int ret;

	host->quirks = 0;

	host->max_clk = clock_get_rate(MMC_CLK);

	host->mmc = &plat->mmc;
	host->mmc->priv = host;
	host->mmc->dev = dev;
	upriv->mmc = host->mmc;

	ret = sdhci_setup_cfg(&plat->cfg, host, 0, 0);
	if (ret)
		return ret;

	return sdhci_probe(dev);
}

static int ftsdc021_sdhci_ofdata_to_platdata(struct udevice *dev)
{
	struct sdhci_host *host = dev_get_priv(dev);
	fdt_addr_t addr;

	host->name = dev->name;

	addr = devfdt_get_addr(dev);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	host->ioaddr = (void *)addr;

	return 0;
}

static int ftsdc021_sdhci_bind(struct udevice *dev)
{
	struct ftsdc021_sdhci_plat *plat = dev_get_plat(dev);

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id ftsdc021_sdhci_ids[] = {
	{ .compatible = "faraday,ftsdc021-sdhci" },
	{ }
};

U_BOOT_DRIVER(ftsdc021_sdhci_drv) = {
	.name		= "ftsdc021_sdhci",
	.id		= UCLASS_MMC,
	.of_match	= ftsdc021_sdhci_ids,
	.of_to_plat = ftsdc021_sdhci_ofdata_to_platdata,
	.ops		= &sdhci_ops,
	.bind		= ftsdc021_sdhci_bind,
	.probe		= ftsdc021_sdhci_probe,
	.priv_auto = sizeof(struct sdhci_host),
	.plat_auto = sizeof(struct ftsdc021_sdhci_plat),
};
