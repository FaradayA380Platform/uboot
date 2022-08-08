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

unsigned int pattern1[] = {
	0x12345678, 0xa5a55a5a, 0x5555aaaa, 0xcccccccc, 
	0xffffffff, 0xeeeeeeee, 0xdddddddd, 0xcccccccc,
	0xfefefefe, 0xedededed, 0xdcdcdcdc, 0xcbcbcbcb,
};

unsigned int pattern2[] = {
	0x87654321, 0x11111111, 0x22222222, 0x33333333, 
	0x45454545, 0x56565656, 0x67676767, 0x78787879
};

unsigned int pattern3[] = {
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

void init_dst_buf()
{
	writel(pattern1[0],0x80000000);
	writel(pattern1[1],0x80000004);
	writel(pattern1[2],0x80000008);
	writel(pattern1[3],0x8000000c);
	writel(pattern1[4],0x80000010);
	writel(pattern1[5],0x80000014);
	writel(pattern1[6],0x80000018);
	writel(pattern1[7],0x8000001c);
	writel(pattern1[8],0x80000020);
	writel(pattern1[9],0x80000024);
	writel(pattern1[0xa],0x80000028);
	writel(pattern1[0xb],0x8000002c);	
}

void init_src_buf()
{
	writel(pattern2[0],0x80000100);
	writel(pattern2[1],0x80000104);
	writel(pattern2[2],0x80000108);
	writel(pattern2[3],0x8000010c);
	writel(pattern2[4],0x80000110);
	writel(pattern2[5],0x80000114);
	writel(pattern2[6],0x80000118);
	writel(pattern2[7],0x8000011c);
	writel(pattern2[8],0x80000120);
	writel(pattern2[9],0x80000124);
	writel(pattern2[0xa],0x80000128);
	writel(pattern2[0xb],0x8000012c);	
}

void gen_modified_cache()
{
	writel(pattern3[0],0x80000100);
	writel(pattern3[1],0x80000104);
	writel(pattern3[2],0x80000108);
	writel(pattern3[3],0x8000010c);	
	writel(pattern3[4],0x80000110);
	writel(pattern3[5],0x80000114);
	writel(pattern3[6],0x80000118);
	writel(pattern3[7],0x8000011c);	
	writel(pattern3[8],0x80000120);
	writel(pattern3[9],0x80000124);
	writel(pattern3[0xa],0x80000128);
	writel(pattern3[0xb],0x8000012c);		
}

void verify_cci_result()
{

#if 0	
	if(	(readl(0x80000000)==pattern3[0]) &&
		(readl(0x80000004)==pattern3[1]) &&
		(readl(0x80000008)==pattern3[2]) &&
		(readl(0x8000000c)==pattern3[3]) )
#else
	if(	(readl(0x80000000)==pattern3[0]) &&
		(readl(0x80000004)==pattern3[1]) &&
		(readl(0x80000008)==pattern3[2]) &&
		(readl(0x8000000c)==pattern3[3]) &&
		(readl(0x80000010)==pattern3[4]) &&
		(readl(0x80000014)==pattern3[5]) &&
		(readl(0x80000018)==pattern3[6]) &&
		(readl(0x8000001c)==pattern3[7]) &&
		(readl(0x80000020)==pattern3[8]) &&
		(readl(0x80000024)==pattern3[9]) &&
		(readl(0x80000028)==pattern3[0xa]) &&
		(readl(0x8000002c)==pattern3[0xb]) )
#endif
	{
		//MEM0/MEM1 both noncache  should be same
		//MEM0/MEM1 both cacheable  should be same

		printf("compare pattern3 pass\n");
		
	}
#if 0
	else if(	(readl(0x80000000)==pattern2[0]) &&
			(readl(0x80000004)==pattern2[1]) &&
			(readl(0x80000008)==pattern2[2]) &&
			(readl(0x8000000c)==pattern2[3]) )
#else
	else if(	(readl(0x80000000)==pattern2[0]) &&
		(readl(0x80000004)==pattern2[1]) &&
		(readl(0x80000008)==pattern2[2]) &&
		(readl(0x8000000c)==pattern2[3]) &&
		(readl(0x80000010)==pattern2[4]) &&
		(readl(0x80000014)==pattern2[5]) &&
		(readl(0x80000018)==pattern2[6]) &&
		(readl(0x8000001c)==pattern2[7]) &&
		(readl(0x80000020)==pattern2[8]) &&
		(readl(0x80000024)==pattern2[9]) &&
		(readl(0x80000028)==pattern2[0xa]) &&
		(readl(0x8000002c)==pattern2[0xb]) )
#endif
	{
		//MEM0 should not be pattern2 whether 

		printf("compare pattern2 fail\n");
		
	}
	else 
	{
		printf("!!!!!!!Unexpected compare  fail\n");
	}
}

void Test_Coherent_CI500(bool cci_on)
{
	u32 temp[4];
	int val ; 

	if(cci_on == false)
		writel(0x1, 0x30000000);

//	writel(0x1, 0x30000008);

//	setbits_32(0x22c000b8, BIT18|BIT19|BIT22|BIT23 | BIT17|BIT21);		//set 10 AxDomain
//	setbits_32(0x22c000b8, BIT18|BIT19|BIT22|BIT23 | BIT16|BIT20);		//set 01 AxDomain	
//	setbits_32(0x22c000b8, BIT18|BIT19|BIT22|BIT23 | BIT17|BIT21|BIT16|BIT20);		//set 11 AxDomain	
//	writel(0x00110000, 0x22c000b8 );		//set 00 AxDomain	

	//	init DMEM0	//cacheable
	init_dst_buf();


	//	init DMEM1	//cacheable
	init_src_buf();


	//flush to assure mem0/1  data is pattern1 / pattern2
	flush_cache(0x80000000, 0x200);

	//	init DMAC030
#if 0	
	writel(0x24800000 , DMAC030_BASE + 0x100	);
	writel(0x10000000 , DMAC030_BASE + 0x104	);
	writel(0x80000100 , DMAC030_BASE + 0x108	);		//copy DMEM1 to DMEM0
	writel(0x80000000 , DMAC030_BASE + 0x10c	);	
	writel(0x00000010 , DMAC030_BASE + 0x114);		
#else
//	writel(0x26C00000 , DMAC030_BASE + 0x100	);
	writel(0x29000000 , DMAC030_BASE + 0x100	);
	writel(0x10000000 , DMAC030_BASE + 0x104	);
	writel(0x80000100 , DMAC030_BASE + 0x108	);		//copy DMEM1 to DMEM0
	writel(0x80000000 , DMAC030_BASE + 0x10c	);	
	writel(0x00000010 , DMAC030_BASE + 0x114);	
#endif	
	//	cpu read pattern2 expect	expect pattern3 will not write immediately 
	temp[0] = (pattern2[0]);
	temp[1] = (pattern2[1]);
	temp[2] = (pattern2[2]);
	temp[3] = (pattern2[3]);

	//cpu write pattern3 into dmem1 (will cache) 
	
	gen_modified_cache();

//	flush_cache(0x80000000, 0x200);
	
	//	enable DMA
#if 0	
	writel(0x24810000 , DMAC030_BASE + 0x100);
#else
//	writel(0x26C10000 , DMAC030_BASE + 0x100	);
	writel(0x29010000 , DMAC030_BASE + 0x100	);
#endif


	while((readl(DMAC030_BASE)&0x1)!=0x1);			//wait for dma channel 0 complete
	printf("if both noncache  & CCI on ,it should compare pattern3 pass\n");
	printf("if both cacheable & CCI on ,it should compare pattern3 pass\n");
	printf("if both cacheable & CCI off ,it should compare pattern2 pass\n");	
	printf("if both noncache & CCI off ,it should compare pattern3 pass\n");		

	verify_cci_result();

}

static int do_cci_test(struct cmd_tbl *cmdtp, int flag,
			       int argc, char *const argv[])
{
		__asm_invalidate_tlb_all();
	Test_Coherent_CI500(1);
	return 0;
}

U_BOOT_CMD(cci, 4, 1, do_cci_test,
	"test cci",
	"cci \n"
	"    devtype defaults to mmc"
);
