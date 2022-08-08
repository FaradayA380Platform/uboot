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
#include <dm.h>
#include <nand.h>
#include <netdev.h>
#include <ns16550.h>

DECLARE_GLOBAL_DATA_PTR;

extern void clock_init(void);
extern void invalidate_icache_all(void);
extern void invalidate_dcache_all(void);

/* System Control Uint (pinmux) */
static void pinmux_init(void)
{
}

void ftlcd210_init(void)
{
	unsigned int i=0;

	writel(0x00000081, SCU_BASE + 0x844c);
	writel(0x00000001, SCU_BASE + 0x8434);
	writel(0x00000061, SCU_BASE + 0x8438);
	writel(0x00000061, SCU_BASE + 0x843c);
	writel(0x00000061, SCU_BASE + 0x8440);
	writel(0x00000061, SCU_BASE + 0x8444);
	writel(0x00000061, SCU_BASE + 0x8448);
	writel(0x00340540, SCU_BASE + 0x8338);
	writel(0x00000BFF, SCU_BASE + 0x833C);
	writel(0x00340541, SCU_BASE + 0x8338);

	// delay function ?
        for(i=0;i<0x4000;i++)
		readl(FTEMC030_0_BASE);

	writel(0x30C43040, SCU_BASE + 0x8114);
}

void sdio_soc_init(void)
{
	setbits_le32(SCU_BASE + 0x811c, 0x00000002);	// sdc_ahb enable
	setbits_le32(SCU_BASE + 0x8128, 0x00000080);	// sdc_clk enable
	setbits_le32(SCU_BASE + 0x8130, 0x00100000);	// sdc_rstn enable
	setbits_le32(SCU_BASE + 0x8434, 0x00000001);
	setbits_le32(SCU_BASE + 0x8438, 0x00000001); 
}

void fotg330_usb_init(void)
{
	clrbits_le32(SCU_BASE + 0x810c, 0x003f0000);	// fix u3_cntp6_1 typo
	setbits_le32(SCU_BASE + 0x810c, 0x001d0000);
	
	setbits_le32(SCU_BASE + 0x8128, 0x00000004);	// usb3 enable
#if 0
	setbits_le32(SCU_BASE + 0x812c, 0x00000008);	// choose SYS_XTAL as u3_coreclk
#else
	setbits_le32(SCU_BASE + 0x812c, 0x80000008);	// choose SYS_XTAL as u3_coreclk
#endif
	setbits_le32(SCU_BASE + 0x8320, 0x00000850);	// usb3 phy's parameters
	setbits_le32(SCU_BASE + 0x8118, 0x00000020);	// usb3_aclk enable
	setbits_le32(SCU_BASE + 0x8130, 0x00002000);	// usb3_rstn enable 

#if 1
	writel(0x48000786, 0x14700000 + 0x00);
	writel(0x00020821, 0x14700000 + 0x04);
	writel(0xA0C56000, 0x14700000 + 0x08);
	writel(0x00000000, 0x14700000 + 0x0c);
	writel(0x00000000, 0x14700000 + 0x10);
	writel(0x46000000, 0x14700000 + 0x14);
	writel(0x40444879, 0x14700000 + 0x18);

	writel(0x00001224, 0x14700000 + 0x20);

	writel(0x2F054273, 0x14700000 + 0x50);
	writel(0x67F04900, 0x14700000 + 0x54);
	writel(0x081090F0, 0x14700000 + 0x58);
	writel(0x80140000, 0x14700000 + 0x5c);
	writel(0x25432840, 0x14700000 + 0x60);
	writel(0x14cc3a00, 0x14700000 + 0x64);	
#endif
}

int board_early_init_f(void)
{
	pinmux_init();
	clock_init();
	return 0;
}

int board_late_init(void)
{
	unsigned int i;
	unsigned itlinesnr;

	itlinesnr = readl(0x18701000 + 0x4) & 0x1f;
	for (i = 0; i <= itlinesnr; i++)
		writel(0xffffffff, 0x18701000 + 0x80 + 4 * i);
	
	writel(0x3,0x18701000);
	writel(0x3,0x18702000);
	writel(0xf0,0x18702004);

	writel(0x1,0x08200004);
	writel(0x43,0x08200004);	

	fotg330_usb_init();
	sdio_soc_init();	
#if 1
	writel(0x000000ff, 0x13008118);
	writel(0x0000000f, 0x1300811c);
	writel(0x000007ff, 0x13008124);
	writel(0x000007ff, 0x13008128);
	writel(0xffffffff,0x13008130);	
#endif

#if 1		//for pcie test
	writel(0x000000ff, SCU_BASE + 0x8118);
	writel(0x0000000f, SCU_BASE + 0x811c);
	writel(0x000007ff, SCU_BASE + 0x8124);
	writel(0x000007ff, SCU_BASE + 0x8128);
	writel(0xffffffff, SCU_BASE + 0x8130);
#endif							 
	ftlcd210_init();	

#if 1
	writel(0x00000001, SCU_BASE + 0x84ac);
	writel(0x00100000, 0x13400020);
#endif

	return 0;
}

int board_init(void)
{
	invalidate_icache_all();
	invalidate_dcache_all();

	gd->bd->bi_arch_number = MACH_TYPE_FARADAY;
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;
#ifdef CONFIG_USE_IRQ
   arch_interrupt_init();
#endif
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

int board_eth_init(struct bd_info *bd)
{
#ifdef CONFIG_USB_ETHER
	return usb_eth_initialize(bd);
#elif defined(CONFIG_FTGMAC030)
	return ftgmac030_initialize(bd);
#endif
	return 0;
}

int board_mmc_init(struct bd_info *bis)
{
#ifdef CONFIG_FTSDC021
	return ftsdc021_sdhci_init(CONFIG_FTSDC021_BASE);
#endif
	return 0;
}
