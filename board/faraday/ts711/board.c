/*
 * (C) Copyright 2012 Faraday Technology
 * Bing-Yao Luo <kay@faraday-tech.com>
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <asm/io.h>
#include <common.h>
#include <nand.h>
#include <netdev.h>
#include <usb.h>

#include <asm/arch/scu.h>
#include <faraday/ftsmc020.h>
#include <faraday/ftsdc010.h>
#include <faraday/nand.h>

#include <dm.h>
#include <ns16550.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/u-boot.h> /* boot information for Linux kernel */

int board_early_init_f(void)
{
	clock_init();
	return 0;
}

#ifdef CONFIG_SPL_BUILD
void board_init_f(ulong dummy)
{
	//early_system_init();
	board_early_init_f();
	//sdram_init();
}
#endif

int board_init(void)
{
	invalidate_icache_all();
	invalidate_dcache_all();

	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	return 0;
}

int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;

	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size =  gd->ram_size; 

	return 0;
}

#if defined(CONFIG_ARMV7_NONSEC) || defined(CONFIG_ARMV7_VIRT)
void smp_set_core_boot_addr(unsigned long addr, int corenr)
{
}

/*
void smp_kick_all_cpus(void)
{
}

void smp_waitloop(unsigned previous_address)
{
}
*/

#endif

extern int ftgmac030_initialize(struct bd_info *bd);
int board_eth_init(struct bd_info *bd)
{
#ifdef CONFIG_FTGMAC030
	return ftgmac030_initialize(bd);
#else
	return 0;
#endif
}

#ifndef CONFIG_SYS_NAND_SELF_INIT
int board_nand_init(struct nand_chip *nand)
{
	return 0;
}
#endif /* #ifndef CONFIG_SYS_NAND_SELF_INIT */

int board_late_init(void)
{
	uint8_t mac_addr[6];

#ifdef CONFIG_NET    
	int val;
	net_random_ethaddr(mac_addr) ;
	eth_env_set_enetaddr("ethaddr", mac_addr);
	if(FTGMAC030_0_BASE == FTGMAC030_0_BASE){
		val = readl(FTSCU100_BASE+0x8070);
		//enable bit 8 gmac_rxck_en(RGMII)
		val |= 0x01<<8;
		writel(val,FTSCU100_BASE+0x8070);
	}
#endif
	return 0;
}

extern int ftsdc021_mmc_init(int dev_index);
int board_mmc_init(struct bd_info *bis)
{
#ifdef CONFIG_FTSDC021
	return ftsdc021_sdhci_init(CONFIG_FTSDC021_BASE);
#else
	return 0;
#endif
}

int board_usb_init(int index, enum usb_init_type init)
{
	unsigned int temp = 0;
	void __iomem *scu_va;

	scu_va = (void __iomem *)FTSCU100_BASE;

	switch (index) {
		case 0:
			// outclksel initialize.
			temp = readl(scu_va + 0x8030);
			temp |= 0x01;	// Reference Clock from CORECLKIN for FOTG210
			if (init == USB_INIT_HOST)
				temp &= ~0x80;
			else
				temp |= 0x80;				
			writel(temp, scu_va + 0x8030);

			// otg_rst_n deassert.
			temp = readl(scu_va + 0x80A4);
			temp |= 0x08;	// for FOTG210
			writel(temp, scu_va + 0x80A4);
			break;
		case 1:
			// outclksel initialize.
			temp = readl(scu_va + 0x8034);
			temp |= 0x01;	// Reference Clock from CORECLKIN for FOTG210_1
			if (init == USB_INIT_HOST)
				temp &= ~0x80;
			else
				temp |= 0x80;	
			writel(temp, scu_va + 0x8034);

			// otg_rst_n deassert.
			temp = readl(scu_va + 0x80A4);
			temp |= 0x10;	// for FOTG210_1
			writel(temp, scu_va + 0x80A4);
			break;
		case 2:
			// outclksel initialize.
			temp = readl(scu_va + 0x80E4);
			temp |= 0x01;	// Reference Clock from CORECLKIN for FOTG210_2
			if (init == USB_INIT_HOST)
				temp &= ~0x80;
			else
				temp |= 0x80;	
			writel(temp, scu_va + 0x80E4);

			// otg_rst_n deassert.
			temp = readl(scu_va + 0x80A4);
			temp |= 0x20;	// for FOTG210_2
			writel(temp, scu_va + 0x80A4);
			break;
		default:
			printf("%s: Unknown device %d\n", __func__, index);
			return -ENODEV;
	};

	return 0;
}
