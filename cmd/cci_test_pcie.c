// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2011 Samsung Electronics
 * Lukasz Majewski <l.majewski@samsung.com>
 *
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
 */

#include <common.h>
#include <blk.h>
#include <command.h>
#include <console.h>
#include <errno.h>
#include <g_dnl.h>
#include <malloc.h>
#include <part.h>
#include <usb.h>
#include <usb_mass_storage.h>
#include <watchdog.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <cpu_func.h>

#define READ(addr)		readl(addr)
#define WRITE(addr,val)  writel(val,addr)
#define sw(addr,val)  writel(val,addr)

#define PCIE0_APB_REG_BASE 0x22100000
#define FTSCU100_SCU_BASE	0x20100000

#define display printf

#if 1
unsigned int pattern1_pci[] = {
	0x12345678, 0xa5a55a5a, 0x5555aaaa, 0xcccccccc, 
	0xffffffff, 0xeeeeeeee, 0xdddddddd, 0xcccccccc,
	0xfefefefe, 0xedededed, 0xdcdcdcdc, 0xcbcbcbcb,
};

unsigned int pattern2_pci[] = {
	0x87654321, 0x11111111, 0x22222222, 0x33333333, 
	0x45454545, 0x56565656, 0x67676767, 0x78787879
};

unsigned int pattern3_pci[] = {
	0x99999999, 0x88888888, 0x77777777, 0x66666666, 
	0x57575757, 0x68686868, 0x79797979, 0x8a8a8a8a,
	0x9b9b9b9b, 0xacacacac, 0xbdbdbdbd, 0xcececece,
};

#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)

#define DMAC030_0_BASE 0x22f00000
#define DMAC030_1_BASE 0x22100000
#define DMAC030_BASE DMAC030_1_BASE

void init_dst_buf_pci()
{
	writel(pattern1_pci[0],0x80000000);
	writel(pattern1_pci[1],0x80000004);
	writel(pattern1_pci[2],0x80000008);
	writel(pattern1_pci[3],0x8000000c);
	writel(pattern1_pci[4],0x80000010);
	writel(pattern1_pci[5],0x80000014);
	writel(pattern1_pci[6],0x80000018);
	writel(pattern1_pci[7],0x8000001c);
	writel(pattern1_pci[8],0x80000020);
	writel(pattern1_pci[9],0x80000024);
	writel(pattern1_pci[0xa],0x80000028);
	writel(pattern1_pci[0xb],0x8000002c);	
}

void init_src_buf_pci()
{
	writel(pattern2_pci[0],0x80000100);
	writel(pattern2_pci[1],0x80000104);
	writel(pattern2_pci[2],0x80000108);
	writel(pattern2_pci[3],0x8000010c);
	writel(pattern2_pci[4],0x80000110);
	writel(pattern2_pci[5],0x80000114);
	writel(pattern2_pci[6],0x80000118);
	writel(pattern2_pci[7],0x8000011c);
	writel(pattern2_pci[8],0x80000120);
	writel(pattern2_pci[9],0x80000124);
	writel(pattern2_pci[0xa],0x80000128);
	writel(pattern2_pci[0xb],0x8000012c);	
}

void gen_modified_cache_pci()
{
	printf("gen_modified_cache_pci\n");
	writel(pattern3_pci[0],0x80000100);
	writel(pattern3_pci[1],0x80000104);
	writel(pattern3_pci[2],0x80000108);
	writel(pattern3_pci[3],0x8000010c);	
	writel(pattern3_pci[4],0x80000110);
	writel(pattern3_pci[5],0x80000114);
	writel(pattern3_pci[6],0x80000118);
	writel(pattern3_pci[7],0x8000011c);	
	writel(pattern3_pci[8],0x80000120);
	writel(pattern3_pci[9],0x80000124);
	writel(pattern3_pci[0xa],0x80000128);
	writel(pattern3_pci[0xb],0x8000012c);		
}
#endif
void verify_ccipci_result()
{

	printf("verify_ccipci_result\n");
	printf("pci :%x , pattern : %x\n",readl(0x41000100),pattern3_pci[0]);
	printf("pci :%x , pattern : %x\n",readl(0x41000104),pattern3_pci[1]);
	printf("pci :%x , pattern : %x\n",readl(0x41000108),pattern3_pci[2]);
	printf("pci :%x , pattern : %x\n",readl(0x4100010c),pattern3_pci[3]);
	printf("pci :%x , pattern : %x\n",readl(0x41000110),pattern3_pci[4]);
	printf("pci :%x , pattern : %x\n",readl(0x41000114),pattern3_pci[5]);
	printf("pci :%x , pattern : %x\n",readl(0x41000118),pattern3_pci[6]);
	printf("pci :%x , pattern : %x\n",readl(0x4100011c),pattern3_pci[7]);
	printf("pci :%x , pattern : %x\n",readl(0x41000120),pattern3_pci[8]);
	printf("pci :%x , pattern : %x\n",readl(0x41000124),pattern3_pci[9]);
	printf("pci :%x , pattern : %x\n",readl(0x41000128),pattern3_pci[0xa]);
	printf("pci :%x , pattern : %x\n",readl(0x4100010c),pattern3_pci[0xb]);




	if(	(readl(0x41000100)==pattern3_pci[0]) &&
		(readl(0x41000104)==pattern3_pci[1]) &&
		(readl(0x41000108)==pattern3_pci[2]) &&
		(readl(0x4100010c)==pattern3_pci[3]) &&
		(readl(0x41000110)==pattern3_pci[4]) &&
		(readl(0x41000114)==pattern3_pci[5]) &&
		(readl(0x41000118)==pattern3_pci[6]) &&
		(readl(0x4100011c)==pattern3_pci[7]) &&
		(readl(0x41000120)==pattern3_pci[8]) &&
		(readl(0x41000124)==pattern3_pci[9]) &&
		(readl(0x41000128)==pattern3_pci[0xa]) &&
		(readl(0x4100012c)==pattern3_pci[0xb]) )
	{
		//MEM0/MEM1 both noncache  should be same
		//MEM0/MEM1 both cacheable  should be same

		printf("compare pattern3 pass\n");
		
	}

	else if(	(readl(0x41000100)==pattern2_pci[0]) &&
		(readl(0x41000104)==pattern2_pci[1]) &&
		(readl(0x41000108)==pattern2_pci[2]) &&
		(readl(0x4100010c)==pattern2_pci[3]) &&
		(readl(0x41000110)==pattern2_pci[4]) &&
		(readl(0x41000114)==pattern2_pci[5]) &&
		(readl(0x41000118)==pattern2_pci[6]) &&
		(readl(0x4100011c)==pattern2_pci[7]) &&
		(readl(0x41000120)==pattern2_pci[8]) &&
		(readl(0x41000124)==pattern2_pci[9]) &&
		(readl(0x41000128)==pattern2_pci[0xa]) &&
		(readl(0x4100012c)==pattern2_pci[0xb]) )

	{
		//MEM0 should not be pattern2_pci whether 

		printf("compare pattern2_pci fail\n");
		
	}
	else 
	{
		printf("!!!!!!!Unexpected compare  fail\n");
	}
}

void Test_CoherentPCI_CI500(bool cci_on)
{
		
	u32 rdata = 0;
	u32 read_data = 0;
	u32 i;
	u32 axi_addr,axi_wdata;

	printf("pcie_loopback\n");

	WRITE((FTSCU100_SCU_BASE+0x870),0x0E000210); 
	// [11:0]: FBDIV[11:0]=0x190
	WRITE((FTSCU100_SCU_BASE+0x874),0x00000190);  
	// 156.25MHz - [14:12]: POSTDIV1B=0x3; [10:8]: POSTDIV1A=0x3
	// 250MHz - [22:20]: POSTDIV3B=0x1: [18:16]: POSTDIV1B=0x4
	// 100MHz - [22:20]: POSTDIV2B=0x4; [18:16]: POSTDIV3B=0x4
	WRITE((FTSCU100_SCU_BASE+0x878),0x14443300); 
	// [12]: PLLEN=1
	WRITE((FTSCU100_SCU_BASE+0x874),(READ(FTSCU100_SCU_BASE+0x874))|0x00001000);	
	// [9]: LOCK=1  
	read_data = 0;
	while( read_data != 0x00000200 ){
	    read_data = readl(FTSCU100_SCU_BASE+0x800);
	    read_data = read_data & 0x00000200 ;
	}
	printf("pcie lock\n");

    	writel( readl(FTSCU100_SCU_BASE  + 0x898)|0x20f , FTSCU100_SCU_BASE  + 0x898);

	////PCIE controller register setting -----------------------------//
//	sw(FTSCU100_SCU_BASE + 0x0898, readl(FTSCU100_SCU_BASE + 0x0898) | 0x0000020f);  //set power_up_rst_n & button_rst_n

	////PCIE controller awmisc setting -----------------------------//
	sw(0x22100000 + 0x0250, 0x00200000);  //set awmisc = 22'h200000 //DBI enable

	////PCIE perst_n output/output enable -----------------------------//
	WRITE((PCIE0_APB_REG_BASE+0x0008),(READ(PCIE0_APB_REG_BASE+0x0008))|0xc0000000);	//set perst_n = 1

	rdata = 0x00000000;
	while (rdata  != 0x80000000) {
		rdata = readl(0x22100000+0x0260);  //wait for slv_arstn de-asserted
		rdata = rdata & 0x80000000;
	} 


	sw(0x40000000 + 0x00a0, 0x00010002);  //Target Link Speed

	//  srdata(0x80000000+0x0710, &rdata);  //
	// if (rdata != 0x10120) fail();

	sw(0x40000000 + 0x0710, 0x10124);  //Set LOOPBACK Enable

//	sw(0x40000000 + 0x0718, 0x00008000);  //Set FAST_LINK_SCALING_FACTOR = 0

	////PCIE controller diag_ctrl_bus[2] -----------------------------//
//	sw(0x22100000 + 0x00a0, 0x00000004);  //set diag_ctrl_bus[2:0] = 3'b100 //Select Fast Link Mode

	//Set app_ltssm_en = 1 
	sw(0x22100000+0xec, 0x00000044);  //set app_ltssm_en = 1 
	//
	////CR write
	//    sw(0x22100000+0x04, 0x00030000, HSIZE_WORD);  //set[31:16] cr_wdata = 0x0003  (Set DETECT_RX_RES = 1, DETECT_RX_RES_OVRD = 1)
	//    sw(0x22100000+0x00, 0x10042c04, HSIZE_WORD);  //set[31:16] cr_addr = 0x1004 [9:8] cr_cmd = 2'b00 (clr)
	//    sw(0x22100000+0x00, 0x10042e04, HSIZE_WORD);  //set[31:16] cr_addr = 0x1004 [9:8] cr_cmd = 2'b10 (wr)
	////
	////CR read
	//    sw(0x22100000+0x00, 0x10042c04, HSIZE_WORD);  //set[31:16] cr_addr = 0x1004 [9:8] cr_cmd = 2'b00 (clr)
	//    sw(0x22100000+0x00, 0x10042f04, HSIZE_WORD);  //set[31:16] cr_addr = 0x1004 [9:8] cr_cmd = 2'b11 (rd)
	//    srdata(0x22100000+0x04, &rdata);  //check read data
	////
#if 1
	//CR write
	sw(0x22100000+0x04, 0x04020000);  //set[31:16] cr_wdata = 0x0402  (Set CM_TIME_OVRD_EN = 1, CM_TIME_OVRD = 1)
	sw(0x22100000+0x00, 0x10112c04);  //set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b00 (clr)
	sw(0x22100000+0x00, 0x10112e04);  //set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b10 (wr)
	mdelay(1);
	//
	//CR read
	sw(0x22100000+0x00, 0x10112c04);  //set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b00 (clr)
	sw(0x22100000+0x00, 0x10112f04);  //set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b11 (rd)
	//srdata(0x22100000+0x04, &rdata);  //check read data
	mdelay(1);
	if(readl(0x22100000+0x04)!=0x04020402)
	{
		printf("check read data fail %x\n",readl(0x22100000+0x04));
		printf("check read data fail %x\n",readl(0x22100000+0x04));
		printf("check read data fail %x\n",readl(0x22100000+0x04));
		printf("check read data fail %x\n",readl(0x22100000+0x04));
	}
	else
		printf("check pass1\n");
	//
#endif


	rdata = 0x00000000;
	while (rdata  != 0x000001b0) {
		rdata = readl(0x22100000+0x0120);  //wait for ltssm_state to 1b (LOOPBACK ACTIVE)
		rdata = rdata & 0x000001f0;
	} 

	printf("LTSSM Go To LOOPBACK\n");

	sw(0x40000000 + 0x0004, 0x100007); //BME / MSE / IO_EN
	sw(0x40100000 + 0x0010, 0x40000001); //BAR0 DBI enable  //DBI2 AXI ADDR[20] is DBI2 bit
	sw(0x40000000 + 0x0010, 0x40000000); //BAR0   (for RX to receive and accept TLP from TX)
	sw(0x40000000 + 0x0020, 0x4fff4000); //MEM LIMIT / MEM BASE (for RX to receive and accept TLP from TX)

	sw(0x40300000 + 0x0108, 0x41000000); //inbound iATU Lower Base Address 
	sw(0x40300000 + 0x010c, 0x00000000); //inbound iATU Higher Base Address 
	sw(0x40300000 + 0x0110, 0x41ffffff); //inbound iATU (Lower) Limit Address 
	sw(0x40300000 + 0x0114, 0x80000000); //inbound iATU Lower Target Address  //RAM FTEMC030_1
	sw(0x40300000 + 0x0118, 0x00000000); //inbound iATU Higher Target Address 
	sw(0x40300000 + 0x0100, 0x00000000); //inbound iATU Region Control 1 (Region is MEM)
	sw(0x40300000 + 0x0104, 0x80000000); //inbound iATU Region Control 2 (Region enable)

	sw(0x40300000 + 0x0208, 0x41000000); //inbound iATU Lower Base Address 
	sw(0x40300000 + 0x020c, 0x00000000); //inbound iATU Higher Base Address 
	sw(0x40300000 + 0x0210, 0x410fffff); //inbound iATU (Lower) Limit Address 
	sw(0x40300000 + 0x0214, 0x41000000); //inbound iATU Lower Target Address  //RAM FTEMC030_1
	sw(0x40300000 + 0x0218, 0x00000000); //inbound iATU Higher Target Address 
	sw(0x40300000 + 0x0200, 0x00000000); //inbound iATU Region Control 1 (Region is MEM)
	sw(0x40300000 + 0x0204, 0x80000000); //inbound iATU Region Control 2 (Region enable)

	sw(0x40000000 + 0x0010, 0x41000000); //inbound iATU Region Control 2 (Region enable)	
	
	////PCIE controller awmisc setting -----------------------------//
	sw(0x22100000 + 0x0250, 0x00000000);  //set awmisc = 22'h000000 //DBI disable

	// sw(0x40000000 + 0x4000, 0x55aa1234, HSIZE_WORD); //TX DATA
	printf("Write to PCIE Slave\n");



	sw(0x22100000 + 0x0250, 0x00200000);  //set awmisc = 22'h200000 //DBI enable
	sw(0x22100000 + 0x025c, 0x00200000);  //set armisc = 22'h200000 //DBI enable

	sw(0x40000000 + 0x0710, 0x10120); //Clear LOOPBACK Enable


	rdata = 0x00000000;
	while (rdata  != 0x00000110) {
	rdata = readl(0x22100000+0x0120);  //wait for ltssm_state to 11 (L0)
	rdata = rdata & 0x000001f0;
	} 
	printf("LTSSM Go To L0\n");


		display("Wait for LTSSM L0\n");

		rdata = 0x00000000;
		while (rdata  != 0x00000110) {
			rdata = readl(0x22100000+0x0120);			
//			srdata(0x22100000+0x0120, &rdata);  //wait for ltssm_state to 11 (L0)
			rdata = rdata & 0x000001f0;
		} 
		display("LTSSM Go To L0\n");

	sw(0x22100000 + 0x025c, 0x00000000);  //set armisc = 22'h200000 //DBI enable
	sw(0x22100000 + 0x025c, 0x00000000);  //set armisc = 22'h200000 //DBI enable	
	
	

	//	init DMEM0	//cacheable
//	init_dst_buf_pci();


	//	init DMEM1	//cacheable
	init_src_buf_pci();

	gen_modified_cache_pci();

	//flush to assure mem0/1  data is pattern1 / pattern2
//	flush_cache(0x80000000, 0x200);	

	verify_ccipci_result();
}

static int do_ccipci_test(struct cmd_tbl *cmdtp, int flag,
			       int argc, char *const argv[])
{
		__asm_invalidate_tlb_all();
	Test_CoherentPCI_CI500(1);
	return 0;
}

U_BOOT_CMD(ccipci, 4, 1, do_ccipci_test,
	"test ccipci",
	"ccipci \n"
	"    devtype defaults to mmc"
);
