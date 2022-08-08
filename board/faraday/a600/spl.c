/* Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:    GPL-2.0+ */

#include <common.h>
#include <cpu_func.h>
#include <dm.h>
#include <dm/root.h>
#include <hang.h>
#include <image.h>
#include <init.h>
#include <serial.h>
#include <spl.h>
#include <spi_flash.h>
#include <version.h>
#include <malloc.h>

#include <asm/cache.h>
#include <asm/io.h>
#include <asm/u-boot.h>

#include <linux/compiler.h>
#include <linux/delay.h>

#include "board.h"

void platform_init(void)
{
	writel(0xFFFFFFFF, 0x2D000060);	// enable PCLK (APB clock control register of the SCU)
	writel(0xFFFFFFFF, 0x2D000050);	// enable HCLK (AHB clock control register of the SCU)
	writel(0xFFFFFFFF, 0x2D000080);	// enable ACLK (AXI clock control register of the SCU)
	writel(0xFFFFFFFF, 0x2D080020);	// enable IP clock (IP_EN of the secure EXTREG)
	writel(0xFFFFFFFF, 0x2AE00050);	// enable IP clock (IP_EN of the non-secure EXTREG)

	writel(0xFFFFFFFF, 0x2AE00064);	// release IP reset (sw_rstn of the non-secure EXTREG)
	writel(0xFFFFFFFF, 0x2AE0006C);	// release IP reset (sw2_rstn of the non-secure EXTREG)
	writel(0xFFFFFFFF, 0x2D080038);	// release IP reset (sw_rstn of the secure EXTREG)

	writel(0x00000000, 0x2D080144);	// config all of ports to non-secure (securecontrol of secure EXTREG)

	writel(0x00008003, 0x2AE00188);	// adjust rds_delay_adj of hb_ctrl
	writel(0x00000008, 0x28300034);	// bypass spi cache
	writel(0x00003401, 0x2AE00158);	// usb0 pwr rst
	writel(0x00003401, 0x2AE00164);	// usb1 pwr rst

	writel(0x1EA1C044, 0x2AE00150);	// usb0 select extvbus
	writel(0x1EA1C044, 0x2AE0015C);	// usb1 select extvbus

	writel(0x00000001, 0x2CC04000);	// enable timestamp counter of coresight
	writel(0x00000001, 0x2CC07000);	// enable timestamp counter of coresight
}

void tzc_init(void)
{
	// TZC_0 initialization
	// set default region of TZC_0 as non-secure
	writel(0xC000000F, 0x2D200110);	// enable secure read/write and filter
	writel(0xFFFFFFFF, 0x2D200114);	// set non-secure access ID
	writel(0x00000002, 0x2D200004);	// set reaction
	writel(0x000F000F, 0x2D200008);	// set gate keeper

	// TZC_1 initialization
	// set default region of TZC_1 as non-secure
	writel(0xC000000F, 0x2D300110);	// enable secure read/write and all of filter
	writel(0xFFFFFFFF, 0x2D300114);	// set non-secure access ID
	writel(0x00000002, 0x2D300004);	// set reaction
	writel(0x000F000F, 0x2D300008);	// set gate keeper
}

void ddr_init(void)
{
	unsigned int val;
	/*
	 * ==================================
	 *  FTDDR440 subsystem
	 * ==================================
	 *  addr[19:16] = 4'h0 ; cha_dfi
	 *  addr[19:16] = 4'h1 ; chb_dfi
	 *  addr[19:16] = 4'h2 ; cha_ctrl
	 *  addr[19:16] = 4'h3 ; chb_ctrl
	 *  addr[19:16] = 4'h4 ; cphy
	 *  addr[19:16] = 4'h5 ; cha_aphy
	 *  addr[19:16] = 4'h6 ; chb_aphy
	 *  addr[19:16] = 4'h7 ; cha_dphy_b0
	 *  addr[19:16] = 4'h8 ; cha_dphy_b1
	 *  addr[19:16] = 4'h9 ; chb_dphy_b0
	 *  addr[19:16] = 4'hA ; chb_dphy_b1
	 * ==================================
	 */

	writel(0x00000000, 0x2D080148);	// disable DDR interleave and assert PHY reset (DDR0_CTRL of secure EXTREG)
//	writel(0x00000002, 0x2D080148);	// enable DDR interleave and assert PHY reset (DDR0_CTRL of secure EXTREG)
	mdelay(100);
	writel(0x00000001, 0x2D080148);	// disable DDR interleave and release PHY reset (DDR0_CTRL of secure EXTREG)
//	writel(0x00000003, 0x2D080148);	// enable DDR interleave and release PHY reset (DDR0_CTRL of secure EXTREG)

	// ------------------- Step1 - change DDR PLL M/N - start -------------------------------

	// cha_dphy reset assert
	writel(0x00000002, 0x2A6700cc);	//a_dphy_b0
	writel(0x00000002, 0x2A6800cc);	//a_dphy_b1
	// chb_dphy reset assert
	writel(0x00000002, 0x2A6900cc);	//b_dphy_b0
	writel(0x00000002, 0x2A6a00cc);	//b_dphy_b1
	// cha_aphy reset assert
	writel(0x00000002, 0x2A6500a0);	//a_aphy
	// chb_aphy reset assert
	writel(0x00000002, 0x2A6600a0);	//b_aphy
	// cphy reset assert
	writel(0x00000002, 0x2A640058);	//cphy

	mdelay(100);

	// cphy PLL setting
	// cphy PLL_CTRL1           0x44= 0x083f4006
	// m = 10'd533 = 10'h215 =
	// p = 6'd3
	// s = 3'd0
//	writel(0x123f6403, 0x2A640044);	//cphy (400mhz)
//	writel(0x0A3f6403, 0x2A640044);	//cphy (800mhz)
//	writel(0x0A3F8543, 0x2A640044);	//cphy (1066mhz)
//	writel(0x013F6403, 0x2A640044);	//cphy (1600mhz)
//	writel(0x013F7483, 0x2A640044);	//cphy (1866mhz)
	writel(0x013F8543, 0x2A640044);	//cphy (2133mhz)

	writel(0x20821820, 0x2A66004C);	//b_aphy clk duty

	mdelay(100);

	// cha_dphy reset assert
	writel(0x00000003, 0x2A6700cc);	//a_dphy_b0
	writel(0x00000003, 0x2A6800cc);	//a_dphy_b1
	// chb_dphy reset assert
	writel(0x00000003, 0x2A6900cc);	//b_dphy_b0
	writel(0x00000003, 0x2A6a00cc);	//b_dphy_b1
	// cha_aphy reset de-assert
	writel(0x00000003, 0x2A6500a0);	//a_aphy
	// chb_aphy reset de-assert
	writel(0x00000003, 0x2A6600a0);	//b_aphy
	// cphy reset de-assert
	writel(0x00000003, 0x2A640058);	//cphy

	// wait to aphy ready - channel A
	val = readl(0x2A6500E0);
	val &= 0x00100000;
	while (val != 0x00100000)
	{
		val = readl(0x2A6500E0);
		val &= 0x00100000;
	}

	// wait to aphy ready - channel B
	val = readl(0x2A6600E0);
	val &= 0x00100000;
	while (val != 0x00100000)
	{
		val = readl(0x2A6600E0);
		val &= 0x00100000;
	}

	// ------------------- Step1 - change DDR PLL M/N - end -------------------------------

	// ------------------- Step2 - Set FTDDR440 control register - start -------------------------------

	// Channel A ctrl setting
	writel(0x08058100, 0x2A620000);	//Memory Controller Configure (LPDDR4 mode, 16-bits memory width)
	writel(0x00103f7c, 0x2A620020);	//LPDDRx Mode Register Set Values for MR1/MR2/MR3/MR4
	writel(0x00000002, 0x2A620038);	//LPDDRx Additive Latency Register
	writel(0x08000075, 0x2A62003c);	//Rank Information Register (0x80000000, 16x10x3, 1GB)
	writel(0x16162217, 0x2A620040);	//Timing Parameter 0
	writel(0x64a18c6a, 0x2A620044);	//Timing Parameter 1
	writel(0x03039a7f, 0x2A620048);	//Timing Parameter 2
	writel(0x00ff8764, 0x2A62004c);	//Timing Parameter 3
	writel(0x0000050a, 0x2A620050);	//Timing Parameter 4
	writel(0x50000000, 0x2A62005c);	//Timing Parameter 7
	writel(0x0000000e, 0x2A620060);	//Initialization of Waiting Cycle Count 1
	writel(0x000000d4, 0x2A620064);	//Initialization of Waiting Cycle Count 2
	writel(0x00000190, 0x2A620068);	//LPDDR4 2 us Wait Cycle Register
	writel(0x00061a80, 0x2A62006c);	//LPDDR4 2 ms Wait Cycle Register
	writel(0x00000000, 0x2A620074);	//EXIT SRF Control Register
	writel(0x001600e5, 0x2A620100);	//Channel Arbitration Setup Register
	writel(0x0f050506, 0x2A620104);	//Channel Arbiter Grant Count Register - A
	writel(0x05080305, 0x2A620108);	//Channel Arbiter Grant Count Register - B
	writel(0x00000002, 0x2A620118);	//Bandwidth Control Register
	writel(0x03030404, 0x2A62011c);	//Bandwidth Control Command Count Register - A
	writel(0x01010202, 0x2A620120);	//Bandwidth Control Command Count Register - B
	writel(0x00000000, 0x2A62012c);	//Channel QoS Mapping Register
	writel(0x00000000, 0x2A620130);	//AXI4 QoS Mapping Register 0
	writel(0x10052008, 0x2A620200);	//DDRx PHY Write/Read Data Timing Control Register
	writel(0x0000aa00, 0x2A620080);	//Command Flush Control Register
	writel(0x00000000, 0x2A620170);	//Traffic Monitor Clock Cycle Register
	writel(0x81818181, 0x2A62008c);	//AHB INCR Read Prefetch Length 1
	writel(0x81818181, 0x2A620090);	//AHB INCR Read Prefetch Length 2
	writel(0x00000001, 0x2A6200a0);	//DDR ELASTIC FIFO Control Register

	writel(0x00000010, 0x2A620204);	//Channel A DFI Training Control Register
	writel(0x00000019, 0x2A620210);
	writel(0x00000043, 0x2A620218);
	writel(0x007f007c, 0x2A620300);	//FSP1 MR2 and MR1
	writel(0x00660030, 0x2A620304);	//FSP1 MR11 and MR3
	writel(0x0000000e, 0x2A620308);	//FSP1 MR13 and MR12
	writel(0x0000000e, 0x2A62030c);	//FSP1 MR22 and MR14
	writel(0xa0093818, 0x2A620310);
	writel(0x26847c9a, 0x2A620314);
	writel(0x03031030, 0x2A620318);
	writel(0x00938764, 0x2A62031c);
	writel(0x00000a18, 0x2A620320);
	writel(0x00000000, 0x2A620324);
	writel(0x00000000, 0x2A620328);
	writel(0x32331414, 0x2A62032c);
	writel(0x00000002, 0x2A620330);
	writel(0x000e2011, 0x2A620334);	//FSP1 wren & trddata_en
	writel(0x00000a0a, 0x2A620338);
	writel(0x000fffff, 0x2A62033C);
	writel(0x001b003c, 0x2A620340);	//FSP2 MR2 and MR1
	writel(0x00660010, 0x2A620344);	//FSP2 MR11 and MR3
	writel(0x0000000e, 0x2A620348);	//FSP2 MR13 and MR12
	writel(0x0000000e, 0x2A62034c);	//FSP2 MR22 and MR14
	writel(0x300a120c, 0x2A620350);
	writel(0x02033524, 0x2A620354);
	writel(0x03022421, 0x2A620358);
	writel(0x00641109, 0x2A62035c);
	writel(0x0000090a, 0x2A620360);
	writel(0x00000000, 0x2A620364);
	writel(0x00000000, 0x2A620368);
	writel(0x32331414, 0x2A62036c);
	writel(0x00000002, 0x2A620370);
	writel(0x00052005, 0x2A620374);	//FSP2 wren & trddata_en
	writel(0x00000a0a, 0x2A620378);
	writel(0x000fffff, 0x2A62037C);

	// Channel B ctrl setting
	writel(0x08058100, 0x2A630000);	//Memory Controller Configure (LPDDR4 mode, 16-bits memory width)
	writel(0x00103f7c, 0x2A630020);	//LPDDRx Mode Register Set Values for MR1/MR2/MR3/MR4
	writel(0x00000002, 0x2A630038);	//LPDDRx Additive Latency Register
	writel(0x0C000075, 0x2A63003c);	//Rank Information Register (0xC0000000, 16x10x3, 1GB)
	writel(0x16162217, 0x2A630040);	//Timing Parameter 0
	writel(0x64a18c6a, 0x2A630044);	//Timing Parameter 1
	writel(0x03039a7f, 0x2A630048);	//Timing Parameter 2
	writel(0x00ff8764, 0x2A63004c);	//Timing Parameter 3
	writel(0x0000050a, 0x2A630050);	//Timing Parameter 4
	writel(0x50000000, 0x2A63005c);	//Timing Parameter 7
	writel(0x0000000e, 0x2A630060);	//Initialization of Waiting Cycle Count 1
	writel(0x000000d4, 0x2A630064);	//Initialization of Waiting Cycle Count 2
	writel(0x00000190, 0x2A630068);	//LPDDR4 2 us Wait Cycle Register
	writel(0x00061a80, 0x2A63006c);	//LPDDR4 2 ms Wait Cycle Register
	writel(0x00000000, 0x2A630074);	//EXIT SRF Control Register
	writel(0x001600e5, 0x2A630100);	//Channel Arbitration Setup Register
	writel(0x0f050506, 0x2A630104);	//Channel Arbiter Grant Count Register - A
	writel(0x05080305, 0x2A630108);	//Channel Arbiter Grant Count Register - B
	writel(0x00000002, 0x2A630118);	//Bandwidth Control Register
	writel(0x03030404, 0x2A63011c);	//Bandwidth Control Command Count Register - A
	writel(0x01010202, 0x2A630120);	//Bandwidth Control Command Count Register - B
	writel(0x00000000, 0x2A63012c);	//Channel QoS Mapping Register
	writel(0x00000000, 0x2A630130);	//AXI4 QoS Mapping Register 0
	writel(0x10052008, 0x2A630200);	//DDRx PHY Write/Read Data Timing Control Register
	writel(0x0000aa00, 0x2A630080);	//Command Flush Control Register
	writel(0x00000000, 0x2A630170);	//Traffic Monitor Clock Cycle Register
	writel(0x81818181, 0x2A63008c);	//AHB INCR Read Prefetch Length 1
	writel(0x81818181, 0x2A630090);	//AHB INCR Read Prefetch Length 2
	writel(0x00000001, 0x2A6300a0);	//DDR ELASTIC FIFO Control Register

	writel(0x00000010, 0x2A630204);	//Channel B DFI Training Control Register
	writel(0x00000019, 0x2A630210);
	writel(0x00000043, 0x2A630218);
	writel(0x007f007c, 0x2A630300);	//FSP1 MR2 and MR1
	writel(0x00660030, 0x2A630304);	//FSP1 MR11 and MR3
	writel(0x0000000e, 0x2A630308);	//FSP1 MR13 and MR12
	writel(0x0000000e, 0x2A63030c);	//FSP1 MR22 and MR14
	writel(0xa0093818, 0x2A630310);
	writel(0x26847c9a, 0x2A630314);
	writel(0x03031030, 0x2A630318);
	writel(0x00938764, 0x2A63031c);
	writel(0x00000a18, 0x2A630320);
	writel(0x00000000, 0x2A630324);
	writel(0x00000000, 0x2A630328);
	writel(0x32331414, 0x2A63032c);
	writel(0x00000002, 0x2A630330);
	writel(0x000e2011, 0x2A630334);	//FSP1 wren & trddata_en
	writel(0x00000a0a, 0x2A630338);
	writel(0x000fffff, 0x2A63033C);
	writel(0x001b003c, 0x2A630340);	//FSP2 MR2 and MR1
	writel(0x00660010, 0x2A630344);	//FSP2 MR11 and MR3
	writel(0x0000000e, 0x2A630348);	//FSP2 MR13 and MR12
	writel(0x0000000e, 0x2A63034c);	//FSP2 MR22 and MR14
	writel(0x300a120c, 0x2A630350);
	writel(0x02033524, 0x2A630354);
	writel(0x03032421, 0x2A630358);
	writel(0x00641109, 0x2A63035c);
	writel(0x0000090a, 0x2A630360);
	writel(0x00000000, 0x2A630364);
	writel(0x00000000, 0x2A630368);
	writel(0x32331414, 0x2A63036c);
	writel(0x00000002, 0x2A630370);
	writel(0x00052005, 0x2A630374);
	writel(0x00000a0a, 0x2A630378);
	writel(0x000fffff, 0x2A63037C);

	// ------------------- Step2 - Set FTDDR440 control register - end -------------------------------

	// ------------------- Step3 - Set pin swap - start -------------------------------

//	writel(0x00000001, 0x2A6001B0);	//D PHY Byte Swap
	writel(0x02105423, 0x2A600160);	//CA PHY Pin Swap Register 0
	writel(0x0000001e, 0x2A600164);	//CA PHY Pin Swap Register 1
	writel(0x32104756, 0x2A6001C0);	//D PHY Byte0 Swap regsiter 0
	writel(0x20137654, 0x2A6001C4);	//D PHY Byte0 Swap regsiter 1
	writel(0x32104756, 0x2A6001C8);	//D PHY Byte0 Swap regsiter 2
	writel(0x45673021, 0x2A6001D0);	//D PHY Byte1 Swap regsiter 0
	writel(0x45673102, 0x2A6001D4);	//D PHY Byte1 Swap regsiter 1
	writel(0x45673021, 0x2A6001D8);	//D PHY Byte1 Swap regsiter 2
//	writel(0x00000001, 0x2A6101B0);	//D PHY Byte Swap
	writel(0x02105243, 0x2A610160);	//CA PHY Pin Swap Register 0
	writel(0x0000001b, 0x2A610164);	//CA PHY Pin Swap Register 1
	writel(0x45673021, 0x2A6101C0);	//D PHY Byte0 Swap regsiter 0
	writel(0x45673102, 0x2A6101C4);	//D PHY Byte0 Swap regsiter 1
	writel(0x45673021, 0x2A6101C8);	//D PHY Byte0 Swap regsiter 2
	writel(0x32104756, 0x2A6101D0);	//D PHY Byte1 Swap regsiter 0
	writel(0x20137654, 0x2A6101D4);	//D PHY Byte1 Swap regsiter 1
	writel(0x32104756, 0x2A6101D8);	//D PHY Byte1 Swap regsiter 2

	// ------------------- Step3 - Set pin swap - end -------------------------------

	// ------------------- Step4 - Set FTDFIW400 control register - start -------------------------------

	// Channel A setting
	writel(0x00031c00, 0x2A600004);	//set freq_set_point
	writel(0x00000000, 0x2A60000c);	//disable update control
	writel(0x00000000, 0x2A600010);	//disable update control
	writel(0x00000000, 0x2A600014);	//disable update control
	writel(0x00120e5c, 0x2A600020);	//cbt
	writel(0x01030014, 0x2A600028);	//weye/reye per-bit
	writel(0x10000054, 0x2A60002C);	//weye setting
	writel(0x55555555, 0x2A600030);	//weye external pattern
	writel(0x5523201c, 0x2A600034);	//rvref setting
	writel(0x01180100, 0x2A60003C);	//wvref setting
	writel(0x00040204, 0x2A600040);	//FSP2's DFI timing setting
	writel(0x00080210, 0x2A600044);	//FSP1's DFI timing setting
	writel(0x0d0b060e, 0x2A600048);	//FSP2's timing setting
	writel(0x1e1e1e1e, 0x2A60004c);	//FSP1's timing setting
	writel(0x25241102, 0x2A600050);	//FSP2's timing setting
	writel(0x858a1e0f, 0x2A600054);	//FSP1's timing setting
	writel(0x40050325, 0x2A600058);	//FSP2's timing setting
	writel(0xf0100f25, 0x2A60005c);	//FSP1's timing setting
	writel(0x20040507, 0x2A600060);	//FSP2's timing setting
	writel(0xa00f100f, 0x2A600064);	//FSP1's timing setting
	writel(0x00002b56, 0x2A600068);	//FSP2's timing setting
	writel(0x0000ffff, 0x2A60006c);	//FSP1's timing setting
	writel(0x0010001b, 0x2A600080);	//FSP2's MR3, MR2, RL=20, WL=10
	writel(0x0010007f, 0x2A600084);	//FSP1's MR3, MR2, RL=36, WL=34
	writel(0x00000000, 0x2A600088);	//MR13
	writel(0x07ff07ff, 0x2A600100);	//FSP0's caphdly and csdly
	writel(0x00002000, 0x2A600238);	//FSP2's R0 B0 RVREF
	writel(0x00002000, 0x2A60023C);	//FSP1's R0 B0 RVREF
	writel(0x00002000, 0x2A600338);	//FSP2's R0 B1 RVREF
	writel(0x00002000, 0x2A60033C);	//FSP1's R0 B1 RVREF

	// Channel B setting
	writel(0x00031c00, 0x2A610004);	//set freq_set_point
	writel(0x00000000, 0x2A61000c);	//disable update control
	writel(0x00000000, 0x2A610010);	//disable update control
	writel(0x00000000, 0x2A610014);	//disable update control
	writel(0x00120e5c, 0x2A610020);	//cbt
	writel(0x01030014, 0x2A610028);	//weye/reye per-bit
	writel(0x10000054, 0x2A61002C);	//weye setting
	writel(0x55555555, 0x2A610030);	//weye external pattern
	writel(0x5523201c, 0x2A610034);	//rvref setting
	writel(0x01180100, 0x2A61003C);	//wvref setting
	writel(0x00040204, 0x2A610040);	//FSP2's DFI timing setting
	writel(0x00080210, 0x2A610044);	//FSP1's DFI timing setting
	writel(0x0d0b060e, 0x2A610048);	//FSP2's timing setting
	writel(0x1e1e1e1e, 0x2A61004c);	//FSP1's timing setting
	writel(0x25241102, 0x2A610050);	//FSP2's timing setting
	writel(0x858a1e0f, 0x2A610054);	//FSP1's timing setting
	writel(0x40050325, 0x2A610058);	//FSP2's timing setting
	writel(0xf0100f25, 0x2A61005c);	//FSP1's timing setting
	writel(0x20040507, 0x2A610060);	//FSP2's timing setting
	writel(0xa00f100f, 0x2A610064);	//FSP1's timing setting
	writel(0x00002b56, 0x2A610068);	//FSP2's timing setting
	writel(0x0000ffff, 0x2A61006c);	//FSP1's timing setting
	writel(0x0010001b, 0x2A610080);	//FSP2's MR3, MR2, RL=20, WL=10
	writel(0x0010007f, 0x2A610084);	//FSP1's MR3, MR2, RL=36, WL=34
	writel(0x00000000, 0x2A610088);	//MR13
	writel(0x07ff07ff, 0x2A610100);	//FSP0's caphdly and csdly
	writel(0x00002000, 0x2A610238);	//FSP2's R0 B0 RVREF
	writel(0x00002000, 0x2A61023C);	//FSP1's R0 B0 RVREF
	writel(0x00002000, 0x2A610338);	//FSP2's R0 B1 RVREF
	writel(0x00002000, 0x2A61033C);	//FSP1's R0 B1 RVREF

	// ------------------- Step4 - Set FTDFIW400 control register - end -------------------------------

	// ------------------- Step5 - Change to boot frequency and enable the training - start -------------------------------

	writel(0x00039c7f, 0x2A600004);	//enable wrlvl, reye, wl2nd, weye
	writel(0x00039c7f, 0x2A610004);	//enable wrlvl, reye, wl2nd, weye

	// wait to cha_dfi boot sequence completed
	val = readl(0x2A600004);
	val &= 0x00008000;
	while (val == 0x00008000)
	{
		val = readl(0x2A600004);
		val &= 0x00008000;
	}

	// wait to chb_dfi boot sequence completed
	val = readl(0x2A610004);
	val &= 0x00008000;
	while (val == 0x00008000)
	{
		val = readl(0x2A610004);
		val &= 0x00008000;
	}

	// ------------------- Step5 - Change to boot frequency and enable the training - end -------------------------------

	// ------------------- Step6 - Trigger DDR initialization and Training - start -------------------------------

	writel(0x00000011, 0x2A620204);	//DFI Training Control Register (dfi_auto_init_en)
	writel(0x00000011, 0x2A630204);	//DFI Training Control Register (dfi_auto_init_en)
	writel(0x00000001, 0x2A620004);	//Memory Controller Command Register (Initial command)
	writel(0x00000001, 0x2A630004);	//Memory Controller Command Register (Initial command)

	// wait to DDR0 init_ok
	// @(posedge AXI_TB.AXI_Core.U_DDR_TOP.init_ok_u0);
	val = readl(0x2D080148);
	val &= 0x80000000;
	while (val != 0x80000000)
	{
		val = readl(0x2D080148);
		val &= 0x80000000;
	}

	// wait to DDR1 init_ok
	// @(posedge AXI_TB.AXI_Core.U_DDR_TOP.init_ok_u1);
	val = readl(0x2D08014C);
	val &= 0x80000000;
	while (val != 0x80000000)
	{
		val = readl(0x2D08014C);
		val &= 0x80000000;
	}
	// ------------------- Step6 - Trigger DDR initialization and Training - end -------------------------------
}

#ifdef CONFIG_SPL_FRAMEWORK

void spl_board_init(void)
{
	board_early_init_f();

	platform_init();

	preloader_console_init();

	tzc_init();

	ddr_init();
}

u32 spl_boot_device(void)
{
	u32 mode;

	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 9) & 0x01) {
		case 0:
			printf("BOOT_DEVICE_UART\n");
			mode = BOOT_DEVICE_UART;
			break;
		case 1:
			printf("BOOT_DEVICE_SPI\n");
			mode = BOOT_DEVICE_SPI;
			break;
		default:
			puts("Unsupported boot mode selected\n");
			hang();
	}

	return mode;
}

#else

#include <xyzModem.h>

void console_init(void)
{
	gd->baudrate = CONFIG_BAUDRATE;

	serial_init();		/* serial communications setup */

	gd->have_console = 1;

#if CONFIG_IS_ENABLED(BANNER_PRINT)
	puts("\nU-Boot " SPL_TPL_NAME " " PLAIN_VERSION " (" U_BOOT_DATE " - "
	     U_BOOT_TIME " " U_BOOT_TZ ")\n");
#endif
}

static int getcxmodem(void)
{
	if (tstc())
		return (getchar());
	return -1;
}

static ulong load_serial_ymodem(ulong offset, int mode)
{
	int size;
	int err;
	int res;
	connection_info_t info;
	char ymodemBuf[1024];
	ulong store_addr = ~0;
	ulong addr = 0;

	size = 0;
	info.mode = mode;
	res = xyzModem_stream_open(&info, &err);
	if (!res) {

		while ((res =
			xyzModem_stream_read(ymodemBuf, 1024, &err)) > 0) {
			store_addr = addr + offset;
			size += res;
			addr += res;
			memcpy((char *)(store_addr), ymodemBuf, res);
		}
	} else {
		printf("%s\n", xyzModem_error(err));
	}

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, &getcxmodem);


	flush_cache(offset, ALIGN(size, ARCH_DMA_MINALIGN));

	printf("## Total Size      = 0x%08x = %d Bytes\n", size, size);

	return offset;
}

static int load_spi_norflash(ulong offset)
{
	int err = 0;
	struct spi_flash *flash;

	/*
	 * Load U-Boot image from SPI flash into RAM
	 * In DM mode: defaults speed and mode will be
	 * taken from DT when available
	 */

	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
				CONFIG_SF_DEFAULT_CS,
				CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		printf("SPI probe failed.\n");
		return -ENODEV;
	}

	/* Load u-boot, mkimage header is 64 bytes. */
	err = spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS, 0x80000,
			     (void *)CONFIG_SYS_LOAD_ADDR);
	if (err) {
		printf("%s: Failed to read from SPI flash (err=%d)\n",
		      __func__, err);
		return err;
	}

	return err;
}

void jump_to_image(void (*image_entry)(void))
{
	image_entry();
}

void board_init_f(ulong dummy)
{
	int val;

#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	gd->malloc_limit = CONFIG_VAL(SYS_MALLOC_F_LEN);
	gd->malloc_ptr = 0;
#endif

	board_early_init_f();

	platform_init();

	console_init();

	timer_init();

	tzc_init();

	ddr_init();

	printf("1. SPI\n");
	printf("2. UART\n");
	printf("Please select boot type[1-2]: ");
	val = getchar();
	printf("%c\n", val);
	switch (val) {
#if defined(CONFIG_SPL_SPI_SUPPORT) && defined(CONFIG_SPL_SPI_FLASH_SUPPORT)
		case '1':
			load_spi_norflash(CONFIG_SYS_LOAD_ADDR);
			break;
#endif
#if defined(CONFIG_SPL_YMODEM_SUPPORT)
		case '2':
			load_serial_ymodem(CONFIG_SYS_LOAD_ADDR, xyzModem_xmodem);
			break;
#endif
		default:
			puts("Unsupported boot mode selected\n");
			hang();
	}

	jump_to_image((void *)CONFIG_SYS_LOAD_ADDR);
}

#endif
