/*
 * board/faraday/gc1601/clock.c
 *
 * (C) Copyright 2020 Faraday Technology
 * B.C. Chen <bcchen@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>

DECLARE_GLOBAL_DATA_PTR;

#define XTAL    (25*1000*1000)      // 25MHz

//#define MACH_GC1601_VP

/*
 * ToDo:
 *
 * Determine the real clock rate from SCU registers
 */
unsigned long pll0_ns(void)
{
#ifndef MACH_GC1601_VP
	return ((readl(PLATFORM_SYSC_BASE + 0x30) >> 24) & 0xff) * 4;
#else
	return 64;
#endif
}

unsigned long pll0_ms(void)
{
#ifndef MACH_GC1601_VP
	return (readl(PLATFORM_SYSC_BASE + 0x30) >> 16) & 0xff;
#else
	return 1;
#endif
}

static inline ulong clk_get_rate_axi(void)
{
	ulong ref_clk, pll0_ckout1, axi_clk;

	ref_clk = XTAL;
	pll0_ckout1 = (ref_clk/pll0_ms()) * pll0_ns() / 3;
#ifndef MACH_GC1601_VP
	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 20) & 0x3) {
	case 1:
	default:
		axi_clk = pll0_ckout1/4;
		break;
	case 0:
		axi_clk = pll0_ckout1/8;
		break;
	}
#else
	axi_clk = pll0_ckout1/4;
#endif

	return axi_clk;
}

static inline ulong clk_get_rate_apb(void)
{
	ulong ref_clk, pll0_ckout1, apb_clk;

	ref_clk = XTAL;
	pll0_ckout1 = (ref_clk/pll0_ms()) * pll0_ns() / 3;
#ifndef MACH_GC1601_VP
	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 20) & 0x3) {
	case 1:
	default:
		apb_clk = pll0_ckout1/4;
		break;
	case 0:
		apb_clk = pll0_ckout1/8;
		break;
	}
#else
	apb_clk = pll0_ckout1/4;
#endif

	return apb_clk;
}

static inline ulong clk_get_rate_cpu(void)
{
	ulong ref_clk, pll0_ckout1, cpu_clk;

	ref_clk = XTAL;
	pll0_ckout1 = (ref_clk/pll0_ms()) * pll0_ns() / 3;
#ifndef MACH_GC1601_VP
	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 16) & 0x3) {
	case 2:
		cpu_clk = pll0_ckout1/1;
		break;
	case 1:
		cpu_clk = pll0_ckout1/2;
		break;
	case 0:
	default:
		cpu_clk = pll0_ckout1/4;
		break;
	}
#else
	cpu_clk = pll0_ckout1/1;
#endif

	return cpu_clk;
}

static inline ulong clk_get_rate_spi(void)
{
	ulong ref_clk, pll0_ckout2, cpu_clk;

	ref_clk = XTAL;
	pll0_ckout2 = (ref_clk/pll0_ms()) * pll0_ns() / 4;
#ifndef MACH_GC1601_VP
	switch ((readl(PLATFORM_SYSC_BASE + 0x810) >> 1) & 0x1) {
	case 1:
		cpu_clk = pll0_ckout2/2;
		break;
	case 0:
	default:
		cpu_clk = pll0_ckout2/8;
		break;
	}
#else
	cpu_clk = pll0_ckout2/8;
#endif

	return cpu_clk;
}

ulong clk_get_rate(char *id)
{
	ulong clk = 0;

	if (!strcmp(id, "AXI"))
		clk = clk_get_rate_axi();
	else if (!strcmp(id, "APB"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "CPU"))
		clk = clk_get_rate_cpu();
	else if (!strcmp(id, "I2C"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "SPI"))
		clk = clk_get_rate_spi();

	return clk;
}

ulong clock_get_rate(enum clk_id id)
{
	switch (id) {
	case SYS_CLK:
	case AXI_CLK:
		return clk_get_rate_axi();
	case APB_CLK:
		return clk_get_rate_apb();
	case CPU_CLK:
		return clk_get_rate_cpu();
	case I2C_CLK:
		return clk_get_rate_apb();
	case SPI_CLK:
		return clk_get_rate_spi();
	default:
		return 0;
	}
}

void clock_init(void)
{
	gd->arch.timer_rate_hz = clk_get_rate_apb();
}

#define SYS_UART_BASE   PLATFORM_FTUART010_BASE
#define UART_LSR        (SYS_UART_BASE + 0x14)        /* In:  Line Status Register */ 
#define UART_LSR_TEMT   0x40                          /* Transmitter empty */ 
#define UART_LSR_THRE   0x20                          /* Transmit-hold-register empty */ 
#define UART_FCR        (SYS_UART_BASE + 8)           /* Out: FIFO Control Register */ 

void ftscu100_interrupt(void *arg) 
{ 
	u8  *regs = (u8 *)PLATFORM_SYSC_BASE; 
	u32 st; 

	st = readl(regs + 0x24);
	writel(st, regs + 0x24);    /* clear interrupt */
} 

void scu_int_init(void) 
{ 
	irq_install_handler(32, ftscu100_interrupt, 0);
}

void st_set_wfi(void)
{
	asm("mcr     p15, 0, r0, c7, c0, 4");	//wait for interrupt
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
	u8  *ddrc_regs = (u8 *)PLATFORM_DDRC_BASE;
	u8  *sysc_regs = (u8 *)PLATFORM_SYSC_BASE;
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

	writel(0x00010000, sysc_regs + 0x0); /* Clear power button event */
	writel(0x000200ff, sysc_regs + 0x4);
	writel(0x00000001, sysc_regs + 0x0);
	writel(0x00003f00, sysc_regs + 0x8);
	writel(0xffffffff, sysc_regs + 0x0);
	
	/* Enable FCS, BUS & CPU speed change interrupts */
	//writel(0x70, sysc_regs + 0x28); /* Already set by hardware default value */

	/* Exit DDR self-refresh mode */
	val = readl(ddrc_regs + 0x04);
	if (val & (0x1<<10)) {
		val = readl(ddrc_regs + 0x04) | (0x1<<3);
		writel(val, ddrc_regs + 0x04);
	}

	/* Start BUS & CPU speed change sequence */
	/* Adjust CPU & BUS MUX*/
	if(mode == 0){ 
		writel(0x00000070, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
	}else if(mode == 1){ 
		writel(0x00110070, sysc_regs + 0x20); /* Half CPU speed & Full Bus speed */
	}else{ 
		writel(0x00120070, sysc_regs + 0x20); /* Full CPU speed & Full Bus speed */
	}
	
	st_set_wfi(); 

	printf("FCS: speed change finished %x\n",readl(sysc_regs + 0x20)); 
}

int soc_clk_set(u32 mode)
{
	int i;

	if(mode == 3) {
		for(i = 1; i <= 10000; i++)
		{
			mode = i%3;
			printf("Round: %d\n",i);
			platform_clock_set(mode);
		}
	} else {
		platform_clock_set(mode);
	}

	return 0;
}