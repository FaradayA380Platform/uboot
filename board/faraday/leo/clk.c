/*
 * board/faraday/leo/clk.c 
 *
 * (C) Copyright 2020 Faraday Technology
 * Bo-Cun Chen <bcchen@faraday-tech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/global_data.h>
#include <asm/arch/clock.h>

DECLARE_GLOBAL_DATA_PTR;

/***************************************************
 * Platform dependant clock routine                *
 ***************************************************/
#define XOSC_0			(25*1000*1000)		// 25MHz
#define ROSC_0			(120*1000*1000)		// 120MHz

#define ARCH_LEO_VP

unsigned long pll0_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x0030)>>24) & 0x0ff;
#else
	return 32;
#endif
}

unsigned long pll1_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8040)>>13) & 0x07f;
#else
	return 40;
#endif
}

unsigned long pll2_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8048)>>13) & 0x07f;
#else
	return 24;
#endif
}

unsigned long pll3_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8050)>>12) & 0x1ff;
#else
	return 213;
#endif
}

unsigned long pll4_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x0040)>>24) & 0x0ff;
#else
	return 32;
#endif
}

unsigned long pll5_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8060)>>12) & 0x1ff;
#else
	return 198;
#endif
}

unsigned long pll6_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x80c8)>>13) & 0x07f;
#else
	return 24;
#endif
}

unsigned long pll7_ns(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8114)>>13) & 0x07f;
#else
	return 32;
#endif
}

unsigned long pll0_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x0030)>>17) & 0x07f;
#else
	return 1;
#endif
}

unsigned long pll1_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8040)>> 8) & 0x01f;
#else
	return 1;
#endif
}

unsigned long pll2_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8048)>> 8) & 0x01f;
#else
	return 1;
#endif
}

unsigned long pll3_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8050)>> 8) & 0x007;
#else
	return 2;
#endif
}

unsigned long pll4_ms(void)
{
	return 1;
}

unsigned long pll5_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8060)>> 8) & 0x007;
#else
	return 2;
#endif
}

unsigned long pll6_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x80c8)>> 8) & 0x01f;
#else
	return 1;
#endif
}

unsigned long pll7_ms(void)
{
#ifndef ARCH_LEO_VP
	return (REG32((void __iomem*)CONFIG_SCU_BASE+0x8114)>> 8) & 0x01f;
#else
	return 1;
#endif
}

static unsigned long clk_get_rate_fastboot(void)
{
	unsigned long boot_ck_sel;
	unsigned long clk;

	boot_ck_sel = (REG32((void __iomem*)CONFIG_SCU_BASE+0x0018)>>13) & 0x003;
	switch (boot_ck_sel) {
		case 1:
			clk = ROSC_0 / 2;
			break;
		case 2:
			clk = ROSC_0 / 4;
			break;
		case 3:
			clk = 25 * 1000 * 1000;
			break;
		case 0:
		default:
			clk = ROSC_0;
			break;
	}

	return clk;
}

static unsigned long clk_get_rate_ahb(void)
{
	unsigned long scu_clkin_mux_2;
	unsigned long clk;

	scu_clkin_mux_2 = (REG32((void __iomem*)CONFIG_SCU_BASE+0x0030)>> 6) & 0x001;
	switch (scu_clkin_mux_2) {
		case 0:
			clk = (XOSC_0 * pll1_ns()) / pll1_ms() / 5;
			break;
		case 1:
		default:
			clk = clk_get_rate_fastboot() / 2;
			break;
	}

	return clk;
}

static unsigned long clk_get_rate_apb(void)
{
	unsigned long pll6_osc_sw_2;
	unsigned long clk;

	pll6_osc_sw_2 = (REG32((void __iomem*)CONFIG_SCU_BASE+0x807c)>> 8) & 0x001;
	switch (pll6_osc_sw_2) {
		case 1:
			clk = (XOSC_0 * pll6_ns()) / pll6_ms() / 6;
			break;
		case 0:
			clk = clk_get_rate_fastboot() / 2;
		default:
			break;
	}

	return clk;
}

static unsigned long clk_get_rate_cpu(void)
{
	unsigned long scu_clkin_mux;
	unsigned long clk;

	scu_clkin_mux = (REG32((void __iomem*)CONFIG_SCU_BASE+0x0030)>> 4) & 0x00f;
	switch (scu_clkin_mux) {
		case 0:
			clk = (XOSC_0 * pll0_ns()) / pll0_ms();
			break;
		case 1:
			clk = (XOSC_0 * pll0_ns()) / pll0_ms() / 2;
			break;
		case 2:
			clk = (XOSC_0 * pll0_ns()) / pll0_ms() / 4;
			break;
		case 3:
			clk = (XOSC_0 * pll0_ns()) / pll0_ms() / 8;
			break;
		case 4:
		default:
			clk = clk_get_rate_fastboot();
			break;
	}

	return clk;
}

static unsigned long clk_get_rate_mmc(void)
{
	unsigned long emmc_osc_sw;
	unsigned long clk;

	emmc_osc_sw = (REG32((void __iomem*)CONFIG_SCU_BASE+0x807c)>> 13) & 0x001;
	switch (emmc_osc_sw) {
		case 1:
			clk = (XOSC_0 * pll7_ns()) / pll7_ms();
			break;
		case 0:
			clk = clk_get_rate_fastboot() / 2;
		default:
			break;
	}

	clk = clk / 2 / 2;

	return clk;
}

static inline unsigned long clk_get_rate_mac(void)
{
	return clk_get_rate_ahb();
}

unsigned long clk_get_rate(char *id)
{
	ulong clk = 0;

	if (!strcmp(id, "AHB"))
		clk = clk_get_rate_ahb();
	else if (!strcmp(id, "APB"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "CPU"))
		clk = clk_get_rate_cpu();
	else if (!strcmp(id, "SDC"))
		clk = clk_get_rate_ahb();
	else if (!strcmp(id, "I2C"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "SPI") || !strcmp(id, "SSP"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "MMC_CLK"))
		clk = clk_get_rate_mmc();
	else if (!strcmp(id, "MAC"))
		clk = clk_get_rate_mac();

	return clk;
}

unsigned long clock_get_rate(enum clk_id id)
{
	switch (id) {
		case SYS_CLK:
		case AHB_CLK:
			return clk_get_rate_ahb();
		case APB_CLK:
			return clk_get_rate_apb();
		case CPU_CLK:
			return clk_get_rate_cpu();
		case I2C_CLK:
			return clk_get_rate_apb();
		case SPI_CLK:
			return clk_get_rate_apb();
		case MMC_CLK:
			return clk_get_rate_mmc();
		case MAC_CLK:
			return clk_get_rate_mac();
		default:
			return 0;
	}
}

void clock_init(void)
{
#ifdef CONFIG_SYS_ARCH_TIMER
	gd->arch.timer_rate_hz = clk_get_rate_cpu();
#else
	gd->arch.timer_rate_hz = clk_get_rate_apb();
#endif
}

#define SYS_UART_BASE   FTUART010_0_BASE
#define UART_LSR        (SYS_UART_BASE + 0x14)        /* In:  Line Status Register */ 
#define UART_LSR_TEMT   0x40                          /* Transmitter empty */ 
#define UART_LSR_THRE   0x20                          /* Transmit-hold-register empty */ 
#define UART_FCR        (SYS_UART_BASE + 8)           /* Out: FIFO Control Register */ 

void ftscu100_interrupt(void *arg) 
{
#if 0
	u8  *regs = (u8 *)0x55300000; 
	u32 st; 

	st = readl(regs + 0x00);
	printf("st = 0x%08x\n", st);
	writel(st, regs + 0x00);    /* clear interrupt */
#else
	u8  *regs = (u8 *)FTSCU100_BASE; 
	u32 st; 

	st = readl(regs + 0x24);
//	printf("st = 0x%08x\n", st);
	writel(st, regs + 0x24);    /* clear interrupt */
#endif
} 

void scu_int_init(void) 
{
	irq_install_handler(32, ftscu100_interrupt, 0);
}

void st_set_wfi(void)
{
	asm("wfi");	//wait for interrupt
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
}

void platform_clock_set(u32 mode) 
{
	u8  *ddrc_regs = (u8 *)FTDDR3030_BASE;
	u8  *sysc_regs = (u8 *)FTSCU100_BASE;
	u32 val = 0;
	u32 timeout = 10000;

	scu_int_init();

	val = readl(ddrc_regs + 0x20) | (0x1<<13);
	writel(val, ddrc_regs + 0x20);
	
	/* wait uart tx over */
	while(timeout-- > 0){
		if(readl(UART_LSR) & UART_LSR_TEMT ){
			break;
		}
		udelay(10);
	}
	
	udelay(100);
#if 1
	writel(0x00010000, sysc_regs + 0x0); /* Clear power button event */
	writel(0xFFFF0007, sysc_regs + 0x4);
	writel(0x10000300, sysc_regs + 0x8);
	writel(0xffffffff, sysc_regs + 0x0);
#else
	writel(0x00330121, sysc_regs + 0x80c8);
	writel(0xe0012343, sysc_regs + 0x807c);
	writel(0x00010000, sysc_regs + 0x0); /* Clear power button event */
	writel(0x00030000, sysc_regs + 0x4);
	writel(0x00000003, sysc_regs + 0x4);
	writel(0x110a0300, sysc_regs + 0x8);
#endif

	/* Exit DDR self-refresh mode */
	val = readl(ddrc_regs + 0x04);
	if (val & (0x1<<10)) {
		val = readl(ddrc_regs + 0x04) | (0x1<<3);
		writel(val, ddrc_regs + 0x04);
	}

	writel(0x0000000E, sysc_regs + 0xB4);

	/* Start Frequency change sequence */
	/* Adjust CLKIN_MUX */
	if (mode == 0) {
		writel(0x20010303, sysc_regs + 0x30); /* 800MHz from PLL0 */
	} else if (mode == 1) {
		writel(0x20010313, sysc_regs + 0x30); /* 400MHz from PLL0 */
	} else if (mode == 2) {
		writel(0x20010323, sysc_regs + 0x30); /* 200MHz from PLL0 */
	} else if (mode == 3) {
		writel(0x20010333, sysc_regs + 0x30); /* 100MHz from PLL0 */
	} else if (mode == 4) {
		writel(0x20010343, sysc_regs + 0x30); /* from ROSC */
	}

	writel(0x60000040, sysc_regs + 0x20);

	st_set_wfi();

	printf("FCS: speed change finished %x\n",readl(sysc_regs + 0x30));

 	//reinitial timer
	clock_init();
}

int soc_clk_set(u32 mode)
{
	int i;

	if(mode == 5) {
		for(i = 0; i < 10000; i++)
		{
			mode = i%5;
			printf("Round: %d\n", i+1);
			platform_clock_set(mode);
		}
	} else {
		platform_clock_set(mode);
	}

	return 0;
}
