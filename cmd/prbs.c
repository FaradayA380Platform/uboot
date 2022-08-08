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

#define TEST_SIZE SZ_32M
//#define TEST_BASE 0xa0000000
#define TEST_BASE TEST_NONCACHE_BASE

#define TEST_NONCACHE_BASE 0x81000000

#define ERR_DDR_READ_WRITE_COMPARE -1

#define SRAM_BASE	0x00200000

int memory_test(void)
{
	int i, j, j1, k, DQ_idx = 31;
	unsigned int pattern_array[4] = {0x41FB1E45, 0x9D4459D4, 0xFA1C49B5, 0xBD8D2EEC};
	unsigned int *buf, *buf1, *bp1, *bp2, *p1, *p2;
	unsigned long mem, chunksize;

    uint8_t start = 0x02;
    uint8_t a = start;

	printf("memory_test: TEST_SIZE:0x%X\n", TEST_SIZE);
	printf("memory_test000000\n");	
	memset((void *)TEST_BASE, 0, 0x80);
	printf("memory_test000111\n");	
	memset((void *)TEST_BASE+0x1000000, 0, TEST_SIZE);

	mem = TEST_SIZE;
	printf("memory_test111\n");	
	if (!mem) {
		return ERR_DDR_READ_WRITE_COMPARE;
	}
	printf("memory_test222\n");		
	buf = (unsigned int *)(SRAM_BASE);
	if (!buf) {
		printf("Unable to malloc memory for buf\n");
		return ERR_DDR_READ_WRITE_COMPARE;
	}

	buf1 = (unsigned int *)(TEST_BASE+0x1000000);
	if (!buf1) {
		printf("Unable to malloc memory for buf1\n");
		return ERR_DDR_READ_WRITE_COMPARE;
	}
	debug("Allocation successful.  Proceeding.\n");

#if 0
	for(i=0; i<4; i++) {
		if(i < 3)
			j1 = 32;
		else
			j1 = 31;

		printf("=======================================\ni = %d,j1 = %d\n",i,j1);
		for(j=0; j<j1; j++) {
//				printf("pattern[i]\t: %x\n",i,pattern_array[i]);			
//				printf("1<<(31-j))\t: %x\n",1<<(31-j));				
			if(pattern_array[i]&(1<<(31-j))) {
//				printf("1:DQ_idx\t= %x\n",DQ_idx);
				*buf = (0x00000000|(1<<DQ_idx)|);
//				printf("1.*buf\t= %x\n",*buf);
			} else {		
//				printf("2:DQ_idx\t= %x\n",DQ_idx);			
				*buf = (0xFFFFFFFF&~(1<<DQ_idx));
//				printf("2.*buf\t=%x\n",*buf);				
			}
			buf++;
		}
	}	
#endif

	 for(i = 0; i<128; i++) {
	        int newbit = (((a >> 6) ^ (a >> 5)) & 1);
	       a = ((a << 1) | newbit);
		*buf = a;
//	        printf("buffer%x\n", *buf);
	 }
	debug("Setup completed.\n");

	printf("Testing(PRBS7)");
	while(1) {
#if 1
		for(k=0; k<TEST_SIZE/127; k++) {
		buf = (unsigned int *)(SRAM_BASE);
			for(i=0; i<127; i++) {			
				*buf = *buf1;
//				printf("buf:\t%x = %lx\n",buf,*buf);
				printf("buf1:\t%x = %lx\n",buf1,*buf1);					
				buf++;
				buf1++;
			}
//			flush_cache(TEST_BASE+0x1000000+k, 128);
		}
//		printf(".");
		buf1 = (unsigned int *)(TEST_BASE+0x1000000);
#else
//		printf(".");
		memcpy(buf1, buf, TEST_SIZE);
		flush_cache(buf1, TEST_SIZE);
#endif
	}
}

int memory_test1()
{
	int i, j, j1, k, DQ_idx = 31;
	unsigned int pattern_array[4] = {0x41FB1E45, 0x9D4459D4, 0xFA1C49B5, 0xBD8D2EEC};
	unsigned int *buf, *buf1, *bp1, *bp2, *p1, *p2;
	unsigned long mem, chunksize;
	int rc_tmp = 0;
       u32 rc;

    uint8_t start = 0x02;
    uint8_t a = start;

	printf("memory_test: TEST_SIZE:0x%X\n", TEST_SIZE);

	memset((void *)SRAM_BASE, 0, 0x80);
	memset((void *)TEST_NONCACHE_BASE+0x1000000, 0, TEST_SIZE);

	mem = TEST_SIZE;
	if (!mem) {
		return ERR_DDR_READ_WRITE_COMPARE;
	}
	
	buf = (unsigned int *)(SRAM_BASE);
	if (!buf) {
		printf("Unable to malloc memory for buf\n");
		return ERR_DDR_READ_WRITE_COMPARE;
	}

	buf1 = (unsigned int *)(TEST_NONCACHE_BASE+0x1000000);
	if (!buf1) {
		printf("Unable to malloc memory for buf1\n");
		return ERR_DDR_READ_WRITE_COMPARE;
	}
	debug("Allocation successful.  Proceeding.\n");


	 for(i = 0; i<128; i++) {
	        int newbit = (((a >> 6) ^ (a >> 5)) & 1);
	       a = ((a << 1) | newbit);
		*buf = a;
//	        printf("buffer%x\n", *buf);
	 }
	debug("Setup completed.\n");

	printf("Testing(PRBS7)");
	while(1) {
#if 1
		for(k=0; k<TEST_SIZE/127; k++) {
			buf = (unsigned int *)(SRAM_BASE);
			for(i=0; i<127; i++) {
				printf("%x = %x\n",buf1,*buf);
				*buf1 = *buf;						
				buf++;
				buf1++;
			}
		}
//		printf(".");
//		flush_cache(TEST_BASE+0x1000000, TEST_SIZE);
		buf1 = (unsigned int *)(TEST_NONCACHE_BASE+0x1000000);
#else
//		printf(".");
		memcpy(buf1, buf, TEST_SIZE);
		flush_cache(buf1, TEST_SIZE);
#endif
	}
}
void prbs_mode0()
{
	int ret;
	printf("prbs_mode0 test start....\n");
	ret = memory_test();
	if(ret == -1)
		printf("prbs mode0 test abnormal\n");
}

int prbs_mode1()
{
#if 0
	char *wc,*rc;
	const char *cmd;
#endif	
		
	unsigned long mode = 0;
	unsigned long wc,rc;
	char *endp;
	int ret;


	printf("write cout = %d , read count = %d\n",wc,rc);

	ret = memory_test1();
	if(ret == -1)
		printf("prbs mode1 test abnormal\n");

	return 0;
#if 0	
	if (strcmp(cmd, "0") == 0)
		printf("mode 0: continuous write\n");
	else if(strcmp(cmd, "1") == 0){
		--argc;
		++argv;		
		printf("mode 1: write read interleave\n");
	cmd = argv[1];
		printf("write count = %s",wc);
	
		--argc;
		++argv;			
	cmd = argv[1];		
		printf("read count = %s",rc);		
	}else{
		printf("wrong parameter\n");
	}
	printf("prbs_test exit\n");
#endif	
}

static int prbs_main(struct cmd_tbl *cmdtp, int flag,
			       int argc, char *const argv[])
{
	const char *cmd;
	int ret;

	/* need at least two arguments */
	if (argc < 2)
		return -1;

	cmd = argv[1];
	--argc;
	++argv;


	if (strcmp(cmd, "0") == 0) {
		printf("prbs mode = 0 continuos write...\n");
		prbs_mode0();
		return 0;
	}else if (strcmp(cmd, "1") == 0) {
		printf("prbs mode = 1 write read interleave...\n");
		ret = prbs_mode1();
		return 0;
	}else{
		printf("wrong parameter!!\n");
	}
	
	return 0;
}

U_BOOT_CMD(prbs, 4, 1, prbs_main,
	"prbs test program\n",
	"\nContinuous write\t--  prbs 0\t\n"
	"Write\Read Interleave\t--  prbs 1 $write_cnt $read_cnt"
);
