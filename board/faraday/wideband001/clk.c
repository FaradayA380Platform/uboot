/*
 * board/faraday/wideband001/clock.c
 *
 * (C) Copyright 2017 Faraday Technology
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
//#define PLL_UPDATE

/*
 * ToDo:
 *
 * Determine the real clock rate from SCU registers
 */
unsigned long pll0_ns(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x30) >> 24) & 0xff;
}

unsigned long pll0_ms(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x30) >> 16) & 0xff;
}

unsigned long pll1_ns(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x40) >> 24) & 0xff;
}

unsigned long pll1_ms(void)
{
	return (readl(PLATFORM_SYSC_BASE + 0x40) >> 16) & 0xff;
}

static inline ulong clk_get_rate_ahb(void)
{
	ulong pll1_out, ahb_clk;

	pll1_out = 250*1000*1000;
#if 1
	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 20) & 3) {
	case 1:
		ahb_clk = pll1_out;
		break;
	case 0:
	default:
		ahb_clk = pll1_out/2;
		break;
	}
#else
	ahb_clk = pll1_out*1;
#endif

	return ahb_clk;
}

static inline ulong clk_get_rate_apb(void)
{
	return clk_get_rate_ahb() >> 1;
}

static inline ulong clk_get_rate_cpu(void)
{
	ulong ref_clk, pll0_out, cpu_clk;
#if 1
	ref_clk = XTAL;
	pll0_out = (ref_clk/pll0_ms()) * pll0_ns();

	switch ((readl(PLATFORM_SYSC_BASE + 0x20) >> 16) & 3) {
	case 0:
		cpu_clk = pll0_out/4;
		break;
	case 2:
		cpu_clk = pll0_out;
		break;
	case 1:
	default:
		cpu_clk = pll0_out/2;
		break;
	}
#else
	cpu_clk = 925*1000*1000;
#endif

	return cpu_clk;
}

static inline ulong clk_get_rate_spi(void)
{
	ulong spi_clk;

	spi_clk = 200*1000*1000;

	return spi_clk;
}

static inline ulong clk_get_rate_mmc(void)
{
	/* clock = [sdclk1x] of ftsdc021 */
	return 100*1000*1000;
}

ulong clk_get_rate(char *id)
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
	else if (!strcmp(id, "SPI"))
		clk = clk_get_rate_spi();
	else if (!strcmp(id, "SSP"))
		clk = clk_get_rate_apb();
	else if (!strcmp(id, "MMC_CLK"))
		clk = clk_get_rate_mmc();

	return clk;
}

ulong clock_get_rate(enum clk_id id)
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
		return clk_get_rate_spi();
	case MMC_CLK:
		return clk_get_rate_mmc();
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
	irq_install_handler(73, ftscu100_interrupt, 0);
}

void st_set_wfi(void)
{
	asm("wfi"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
	asm("nop"); 
}
void  reinit_uart()
{
writel(0,0x04700004);
writel(0x83,0x0470000c);
writel(0,0x04700004);
writel(0x11,0x04700000);
writel(0,0x04700004);
writel(1,0x04700008);
writel(3,0x0470000c);
writel(3,0x04700010);
writel(1,0x04700008);
writel(7,0x04700008);
}

#if 0

#define SCU_BASE 0x04100000
//-------------------------------------------
// Register Mapping parameters
//-------------------------------------------
#define SCU_BTUPSTS          0x00000000    // bootup staus Register
#define SCU_BTUPCTL          0x00000004    // bootup control Register
#define SCU_PWRCTL           0x00000008    // Power Control Register
#define SCU_PWRUPSEQ         0x0000000C    // Power Control Register

#define SCU_PLLCTL           0x00000030    // PLL Control Register
#define SCU_DLLCTL           0x00000044    // DLL Control Register
#define SCU_PWRMOD           0x00000020    // Power Mode Register
void platform_clock_set(u32 mode) 
{
	u8  *ddrc_regs = (u8 *)PLATFORM_DDRC_BASE;
	u8  *sysc_regs = (u8 *)PLATFORM_SYSC_BASE;
	u32 val = 0;
	u32 timeout = 10000;
  u32 rdata;

	scu_int_init();

#if 0	
	/* wait uart tx over */
	while(timeout-- > 0){
		if(readl(UART_LSR) & UART_LSR_TEMT ){
			break;
		}
		udelay(10);
	}
	
	udelay(100);
#endif
	writel(0x00010000, sysc_regs + 0x0); /* Clear power button event */
	writel(0x000300ff, sysc_regs + 0x4);
	//writel(0x000200ff, sysc_regs + 0x4);
	rdata = readl(SCU_BASE+SCU_PWRCTL);
	writel(0x00000001, sysc_regs + 0x0);
	writel(0x00003f00, sysc_regs + 0x8);
	writel(0xffffffff, sysc_regs + 0x0);
	
  //srdata (SCU_BASE+SCU_PLLCTL, &rdata);
  rdata = readl(SCU_BASE+SCU_PLLCTL);
  rdata = readl(SCU_BASE+SCU_DLLCTL);
  rdata = readl(SCU_BASE+SCU_PWRMOD);

	/* Enable FCS, BUS & CPU speed change interrupts */
	//writel(0x70, sysc_regs + 0x28); /* Already set by hardware default value */

	/* Exit DDR self-refresh mode */
	val = readl(ddrc_regs + 0x04);
	if (val & (0x1<<10)) {
		//val = readl(ddrc_regs + 0x04) | (0x1<<3);
		//writel(val, ddrc_regs + 0x04);
	}

	/* Start BUS & CPU speed change sequence */
	//writel(0x025010303, sysc_regs + 0x30); /* 925MHz (PLL_NS=0x25, PLL_MS=0x01, PLL_FR=0x3, CLK_MUX=0x0) */

	writel(2, sysc_regs + 0xB4);
	
	/* Adjust CPU & BUS MUX*/
	if(mode == 0){ 
		//writel(0x00000070, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
		writel(0x00000020, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
	}else if(mode == 1){ 
		writel(0x00110020, sysc_regs + 0x20); /* Half CPU speed & Full Bus speed */
		//writel(0x00110070, sysc_regs + 0x20); /* Half CPU speed & Full Bus speed */
	}else{ 
		writel(0x00120020, sysc_regs + 0x20); /* Full CPU speed & Full Bus speed */
		//writel(0x00120070, sysc_regs + 0x20); /* Full CPU speed & Full Bus speed */
	}
	
	st_set_wfi(); 
  //reinit_uart();
	printf("FCS: speed change finished %x\n",readl(sysc_regs + 0x20)); 
}
#else
#define FTSCU100_DEBOUNCE_ON
#define HSIZE_WORD 4
#define sw(a,b,c) writel(b,a)

#define SCU_BASE 0x04100000
//-------------------------------------------
// Register Mapping parameters
//-------------------------------------------
#define SCU_BTUPSTS          0x00000000    // bootup staus Register
#define SCU_BTUPCTL          0x00000004    // bootup control Register
#define SCU_PWRCTL           0x00000008    // Power Control Register
#define SCU_PWRUPSEQ         0x0000000C    // Power Control Register

#define SCU_CHIPID           0x00000010    // Chip ID Register
#define SCU_VERID            0x00000014    // System Control Unit Version Register
#define SCU_STRAP            0x00000018    // Strap Status Register
#define SCU_OSCCTL           0x0000001C    // OSC Control Register
#define SCU_PWRMOD           0x00000020    // Power Mode Register
#define SCU_STATUS           0x00000024    // Interrupt Status Register
#define SCU_INTEN            0x00000028    // Interrupt Enable Register
#define SCU_PLLCTL           0x00000030    // PLL Control Register
#define SCU_TCICTR1          0x00000034    // TCI SSCG PLL control register1 
#define SCU_TCICTR2          0x00000038    // TCI SSCG PLL control register1 
#define SCU_TCICTR3          0x0000003C    // TCI SSCG PLL control register1 
#define SCU_PLL2CTL          0x00000040    // DLL Control Register
#define SCU_DLLCTL           0x00000044    // DLL Control Register
#define SCU_AHBCLK           0x00000050    // AHB Clock Gated Register
#define SCU_AHBCLKE          0x00000054    // AHB Clock Gated Extend Register
#define SCU_AHBCLK_SLP       0x00000058    // AHB Clock Gated Register for Sleep
#define SCU_AHBCLKE_SLP      0x0000005C    // AHB Clock Gated Extend Register for Sleep
#define SCU_APBCLK           0x00000060    // APB Clock Gated Register
#define SCU_APBCLKE          0x00000064    // APB Clock Gated Extend Register
#define SCU_APBCLK_SLP       0x00000068    // APB Clock Gated Register for Sleep
#define SCU_APBCLKE_SLP      0x0000006C    // APB Clock Gated Extend Register for Sleep
#define SCU_AXICLK           0x00000080    // AXI Clock Gated Register
#define SCU_AXICLKE          0x00000084    // AXI Clock Gated Extend Register
#define SCU_AXICLK_SLP       0x00000088    // AXI Clock Gated Register
#define SCU_AXICLKE_SLP      0x0000008C    // AXI Clock Gated Extend Register

#define SCU_LPI_MSK          0x00000090    // AXI Low Power Interface check mask
#define SCU_LPI_CKSTS        0x00000094    // AXI Low Power Interface clock status
#define SCU_LPI_ACTSTS       0x00000098    // AXI Low Power Interface active status in fail
#define SCU_LPI_ACKSTS       0x0000009c    // AXI Low Power Interface ack status in fail 
#define SCU_SCRPAD0          0x00000100    // System Control Unit Scratch Pad Register 0
#define SCU_SCRPAD1          0x00000104    // System Control Unit Scratch Pad Register 1
#define SCU_SCRPAD2          0x00000108    // System Control Unit Scratch Pad Register 2
#define SCU_SCRPAD3          0x0000010C    // System Control Unit Scratch Pad Register 3
#define SCU_SCRPAD4          0x00000110    // System Control Unit Scratch Pad Register 4
#define SCU_SCRPAD5          0x00000114    // System Control Unit Scratch Pad Register 5
#define SCU_SCRPAD6          0x00000118    // System Control Unit Scratch Pad Register 6
#define SCU_SCRPAD7          0x0000011C    // System Control Unit Scratch Pad Register 7
#define SCU_SCRPAD8          0x00000120    // System Control Unit Scratch Pad Register 8
#define SCU_SCRPAD9          0x00000124    // System Control Unit Scratch Pad Register 9
#define SCU_SCRPAD10         0x00000128    // System Control Unit Scratch Pad Register 10
#define SCU_SCRPAD11         0x0000012C    // System Control Unit Scratch Pad Register 11
#define SCU_SCRPAD12         0x00000130    // System Control Unit Scratch Pad Register 12
#define SCU_SCRPAD13         0x00000134    // System Control Unit Scratch Pad Register 13
#define SCU_SCRPAD14         0x00000138    // System Control Unit Scratch Pad Register 14
#define SCU_SCRPAD15         0x0000013C    // System Control Unit Scratch Pad Register 15

#define RTC_TIME1            0x00000200 
#define RTC_TIME2            0x00000204 
#define RTC_ALMTIME1         0x00000208
#define RTC_ALMTIME2         0x0000020C
#define RTC_CTRL             0x00000210
#define RTC_TRIM             0x00000214

// == define extern register and gating clock  default value ==
#define FTSCU100_HCLKEN_DEF      0xFFFFFFFF    // default value for HCLK gating register, 0:gated, 1: enable
#define FTSCU100_PCLKEN_DEF      0xFFFFFFFF    // default value for PCLK gating register, 0:gated, 1: enable
#define FTSCU100_ACLKEN_DEF      0xFFFFFFFF    // default value for ACLK gating register, 0:gated, 1: enable
#define FTSCU100_HCLKEN64B_DEF   0xFFFFFFFF    // default value for HCLK gating bit exteend register
#define FTSCU100_PCLKEN64B_DEF   0xFFFFFFFF    // default value for PCLK gating bit exteend register
#define FTSCU100_ACLKEN64B_DEF   0xFFFFFFFF    // default value for ACLK gating bit exteend register
#define FTSCU100_GPO_DEF         0x0
#define FTSCU100_PCDCSR_DEF     0x1             //power control driving strength, undefine to turn-off
#define FTSCU100_PCPWRDN_DEF    0x00
#define FTSCU100_PCPWRUP_DEF    0x01
#define FTSCU100_PWRUPS1_DEF    0x01
#define FTSCU100_PWRUPS2_DEF    0x07
#define FTSCU100_PWRUPS3_DEF    0x1F
#define FTSCU100_PWRUPS4_DEF    0x3F
#define FTSCU100_DEBOUNCE_DEF   0x08
//#define FTSCU100_RTCTRIM_DEF     0x7fff0000    // RTC tick divider default value
#define FTSCU100_RTCTRIM1_DEF   0x147            // RTC tick divided factor default value
#define FTSCU100_RTCTRIM2_DEF    0x2a8          // RTC tick numerator default value

#define FTSCU100_RTCTRIM1_N      10                // RTC tick divider bit number
#define FTSCU100_RTCTRIM2_N      10               // RTC tick factional divider bit number
#define FTSCU100_RTCTRIM2_D      1000             // RTC tick factional divider denominator value

//#define FTSCU100_DIV_NUM         9              // define divider number clock = OSCin/2^n (up to 15)
#define FTSCU100_OSCLSTB_N       15               // OSCL crystal PAD stable time define, up 0 to 16

#define DDR_TOP_REG_BASE PLATFORM_DDRC_BASE
void platform_clock_set(u32 mode) 
{
	u8  *ddrc_regs = (u8 *)PLATFORM_DDRC_BASE;
	u8  *sysc_regs = (u8 *)PLATFORM_SYSC_BASE;
	u32 val = 0;
	u32 timeout = 10000;
    unsigned int rdata;
    unsigned int wdata;
    unsigned int PLL_NS;
    unsigned int PLL_MS;
    unsigned int PLL_FR;
    unsigned int CLK_MUX;
    unsigned int BUS_MUX;
    unsigned int DLL_FR;
    unsigned int FCLK_DIV;
    unsigned int SELF_REFRESH;
	scu_int_init();
	printf("DDR SPEED %d Hz\n",mode*50);

	/* wait uart tx over */
	while(timeout-- > 0){
		if(readl(UART_LSR) & UART_LSR_TEMT ){
			break;
		}
		udelay(10);
	}
	
	udelay(100);
	//src (SCU_BASE+SCU_BTUPSTS,          0x00010000, HSIZE_WORD);
	sw  (SCU_BASE+SCU_BTUPSTS,          0x00010000, HSIZE_WORD);

	sw  (SCU_BASE+SCU_BTUPCTL,          0x000300ff, HSIZE_WORD);

	#ifdef FTSCU100_DEBOUNCE_ON
	rdata = (FTSCU100_PCDCSR_DEF<<28)| (FTSCU100_DEBOUNCE_DEF<<16)| (FTSCU100_PCPWRUP_DEF<<8)| FTSCU100_PCPWRDN_DEF;
	#else
	rdata = (FTSCU100_PCDCSR_DEF<<28)| (FTSCU100_PCPWRUP_DEF<<8)| FTSCU100_PCPWRDN_DEF;
	#endif
	//src (SCU_BASE+SCU_PWRCTL,           rdata,         HSIZE_WORD);
	rdata = readl(SCU_BASE+SCU_PWRCTL);
	sw  (SCU_BASE+SCU_BTUPSTS,          0x00000001, HSIZE_WORD);
	sw  (SCU_BASE+SCU_PWRCTL,           0x00003f00, HSIZE_WORD);

	//clear system enable status
	//sw  (SCU_BASE+SCU_BTUPSTS,     0xffffffff, HSIZE_WORD);

	// clear scu state
	sw  (SCU_BASE+SCU_BTUPSTS,     0xffffffff, HSIZE_WORD);
	//#10000;

	//srdata (SCU_BASE+SCU_PLLCTL, &rdata);
	rdata = readl(SCU_BASE+SCU_PLLCTL);
	PLL_NS   = (rdata>>24) & 0xFF;
	PLL_MS   = (rdata>>16) & 0xFF;
	PLL_FR   = (rdata>>8)  & 0x7;
	CLK_MUX  = (rdata>>4)  & 0x3;

	//srdata (SCU_BASE+SCU_DLLCTL, &rdata);
	rdata = readl(SCU_BASE+SCU_DLLCTL);
	DLL_FR   = (rdata>>8) & 0x1;

	//srdata (SCU_BASE+SCU_PWRMOD, &rdata);
	rdata = readl(SCU_BASE+SCU_PWRMOD);

	FCLK_DIV = (rdata>>16) &0x7;
	BUS_MUX  = (rdata>>20) &0x7;

	// Check whether DDR is in self-refresh state
	// DDR can't be in self-refresh state before speed change
	//srdata (DDR_TOP_REG_BASE+0x4, &rdata);
  rdata = readl(DDR_TOP_REG_BASE+0x4);
	SELF_REFRESH = (rdata>>10) &0x1;
	while (SELF_REFRESH)
	{
        printf("selrefresh\n");
	}
	
  //writel(2, sysc_regs + 0xB4);	
#if 1
	/* Adjust CPU MUX*/
	switch(mode){
	  case 0:
		val = readl(SCU_BASE + 0x40);
		val &=~(0xff000000);
		val |= (0x15<<24);
		writel(val,SCU_BASE + 0x40);
		val = readl(SCU_BASE + 0x44);
		val &=~(0x0000ff00);
		val |= (0x6<<8);
		writel(val,SCU_BASE + 0x44);
		writel(0x00000040, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
		break;
	  case 1:
		val = readl(SCU_BASE + 0x40);
		val &=~(0xff000000);
		val |= (0x1a<<24);
		writel(val,SCU_BASE + 0x40);
		val = readl(SCU_BASE + 0x44);
		val &=~(0x0000ff00);
		val |= (0x7<<8);
		writel(val,SCU_BASE + 0x44);
		writel(0x00000040, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
	    break;
	  case 2:
		val = readl(SCU_BASE + 0x40);
		val &=~(0xff000000);
		val |= (0x20<<24);
		writel(val,SCU_BASE + 0x40);
		val = readl(SCU_BASE + 0x44);
		val &=~(0x0000ff00);
		val |= (0x7<<8);
		writel(val,SCU_BASE + 0x44);
		writel(0x00000040, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
	    break;
      case 3://change cpu /bus frequency
		writel(0x00000030, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
		break;
      case 4://change cpu /bus frequency
		writel(0x00110030, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
		break;
      case 5://change cpu /bus frequency
		writel(0x00120030, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
		break;
      case 6://change cpu /bus frequency
		writel(0x00100030, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
		break;
      case 7://do not change frequency
		writel(0x80000040, sysc_regs + 0x20); 
		break;
	  default:
		writel(0x00020030, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
	}
#else
	//ddr only
	if(mode==7){
	}else{
		val = readl(SCU_BASE + 0x40);
		val &=~(0xff000000);
		val |= (mode<<24);
		writel(val,SCU_BASE + 0x40);
		val = readl(SCU_BASE + 0x44);
		val &=~(0x0000ff00);
		//check dllfrange
		if((mode >= 16) && (mode <18)) val |= (0x04<<8);
		if((mode >= 18) && (mode <20)) val |= (0x05<<8);
		if((mode >= 20) && (mode < 24)) val |= (0x06<<8);
		if((mode >= 24) && (mode <= 32)) val |= (0x07<<8);
		writel(val,SCU_BASE + 0x44);
	}
#ifdef PLL_UPDATE
		val = readl(sysc_regs+0x20);
		val |= (0x01<<8);
		writel(val, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
#else		
		writel(0x00000040, sysc_regs + 0x20); /* Quarter CPU speed & Half Bus speed */
#endif		
#endif

	//val = readl(ddrc_regs + 0x20) | (0x1<<13);
	//writel(val, ddrc_regs + 0x20);
#ifndef PLL_UPDATE
	st_set_wfi(); 
#endif
	printf("SPEEDCHANGE OVER:%x\n",readl(sysc_regs + 0x20)); 
	printf("cpu/bus speed change finished %x\n",readl(sysc_regs + 0x20)); 
}	
#endif

int soc_clk_set(u32 mode)
{
	int i , loop;

	if(mode == 9) {
#if 0
	  while(1){
		i++;
		if(i%2==0 ){
			platform_clock_set(16);
		}else{
			platform_clock_set(32);
		}
	  }
#endif
	    i=0;
	    while(1){
          if(i%2==0){
			for(loop=16;loop<32;loop++){
				platform_clock_set(loop);
			}
		  }else
		  for(loop=32;loop>16;loop--)
		  {
			platform_clock_set(loop);
			//platform_clock_set(mode);
		  }
		  i++;
		}
	} else if(mode==7){
	  while(1)
		platform_clock_set(mode);
	} else {
		platform_clock_set(mode);
	}
	return 0;
}
