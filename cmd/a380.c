/*
 * Copyright 2000-2009
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
//#define A380_SERDES
#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
//#include "gic_v3.h"
#if 0
#include <asm/arch-a380/a380.h>
#ifdef A380_SERDES
#include <asm/arch/pwrup_1_25gbps.h>
#include <asm/arch/pwrup_2_5gbps.h>
#include <asm/arch/pwrup_5gbps.h>
#include <asm/arch/pwrup_3_125gbps.h>
#include <asm/arch/pwrup_8g.h>
#include <asm/arch/rxeq.h>

//bist
struct init_seq serdes_bist[] = {
{0x8002,0x81},
{0x801e,0x3},
{0xe01a,0x43},

{0xe00b,0x1b},
{0x8023,0x18},
{0xe002,0x8},
};

struct init_seq lte_insert_error[] = {
	//{  0xe002	,0x8 },
	{  0xe002	,0x8A},
	{  0x8002	,	0xc1},
	{  0x8002	,	0x81},
	{  0x8002	,	0xc1},
	{0x8002	,	0x81},
	{0x8002	,	0xc1},
	{0x8002	,	0x81},
	{0x8002	,	0xc1},
	{0x8002	,	0x81},
	{0x8002	,	0xc1},
	{0x8002	,	0x81},
	{0x8002	,	0xc1},
	{0x8002	,	0x81},
	{0x8002	,	0xc1},
	{0x8002	,	0x81},
};

struct init_seq internal_loopback[] = {
	//initialise the relevant pcs registers
	{CONFIG_GMAC_BASE+0x200 , 0x00000040},
	//prepare tx descriptor
	{0x5000f000 , 0x50006000},	
	{0x5000f004 , 0x00008072},	
	{0x5000f00c , 0x80000000},	
	{0x5000f104 , 0x80000000},
	{0x5000f204 , 0x80000000},
	{0x5000f304 , 0x80000000},
	{0x5000f404 , 0x80000000},
	{0x5000f504 , 0x80000000},
	{0x5000f604 , 0x80000000},
	{0x5000f704 , 0x80000000},
	//prepare tx packet
	{0x50006000 , 0x01010100},	
	{0x50006004 , 0x02020101},	
	{0x50006008 , 0x01020202},	
	{0x5000600c , 0xa5a56400},	
	{0x50006010 , 0xa5a5a5a5},	
	{0x50006014 , 0xa5a5a5a5},	
	{0x50006018 , 0xa5a5a5a5},	
	{0x5000601c , 0xa5a5a5a5},	
	{0x50006020 , 0xa5a5a5a5},	
	{0x50006024 , 0xa5a5a5a5},	
	{0x50006028 , 0xa5a5a5a5},	
	{0x5000602c , 0xa5a5a5a5},	
	{0x50006030 , 0xa5a5a5a5},	
	{0x50006034 , 0xa5a5a5a5},	
	{0x50006038 , 0xa5a5a5a5},	
	{0x5000603c , 0xa5a5a5a5},	
	{0x50006040 , 0xa5a5a5a5},	
	{0x50006044 , 0xa5a5a5a5},	
	{0x50006048 , 0xa5a5a5a5},	
	{0x5000604c , 0xa5a5a5a5},	
	{0x50006050 , 0xa5a5a5a5},	
	{0x50006054 , 0xa5a5a5a5},	
	{0x50006058 , 0xa5a5a5a5},	
	{0x5000605c , 0xa5a5a5a5},	
	{0x50006060 , 0xa5a5a5a5},	
	{0x50006064 , 0xa5a5a5a5},	
	{0x50006068 , 0xa5a5a5a5},
	{0x5000606c , 0xa5a5a5a5},	
	{0x50006070 , 0xb7f6a5a5},	
	{0x50006074 , 0x0000e73d},	
	{0x50006078 , 0x00000000},	
	{0x5000607c , 0x00000000},	
	//prepare rx descriptor
	{0x5000E000 , 0x50000002},	
	{0x5000E100 , 0x50000001},	
	{0x5000E200 , 0x50000001},	
	{0x5000E300 , 0x50000001},	
	{0x5000E400 , 0x50000001},	
	{0x5000E500 , 0x50000001},	
	{0x5000E600 , 0x50000001},	
	{0x5000E700 , 0x50000001},		
	//enable loopback
	{CONFIG_GMAC_BASE , 0x00000002},		
	//check and initialize the relavent registers
	{CONFIG_GMAC_BASE+0x28 , 0xffffffff},	
	{CONFIG_GMAC_BASE+0x04 , 0x08280403},	
	{CONFIG_GMAC_BASE+0x18 , 0x5000e000},	
	{CONFIG_GMAC_BASE+0x1c , 0x5000f000},	
	{CONFIG_GMAC_BASE+0x88 , 0x01010100},	
	{CONFIG_GMAC_BASE+0x8c , 0x00000101},		
	{CONFIG_GMAC_BASE+0x440 , 0x5000f100},	
	{CONFIG_GMAC_BASE+0x444 , 0x5000f200},	
	{CONFIG_GMAC_BASE+0x448 , 0x5000f300},	
	{CONFIG_GMAC_BASE+0x44c , 0x5000f400},	
	{CONFIG_GMAC_BASE+0x450 , 0x5000f500},	
	{CONFIG_GMAC_BASE+0x454 , 0x5000f600},	
	{CONFIG_GMAC_BASE+0x458 , 0x5000f700},
	{CONFIG_GMAC_BASE+0x480 , 0x5000e100},	
	{CONFIG_GMAC_BASE+0x484 , 0x5000e200},	
	{CONFIG_GMAC_BASE+0x488 , 0x5000e300},	
	{CONFIG_GMAC_BASE+0x48c , 0x5000e400},
	{CONFIG_GMAC_BASE+0x490 , 0x5000e500},	
	{CONFIG_GMAC_BASE+0x494 , 0x5000e600},	
	{CONFIG_GMAC_BASE+0x498 , 0x5000e700},	
	//start
	{CONFIG_GMAC_BASE , 0x0000020e},
};

struct init_seq external_loopback[] = {
	//initialise the relevant pcs registers
	{CONFIG_GMAC_BASE+0x200 , 0x00000040},//ewrap here
	//prepare tx descriptor
	{0x4000f000 , 0x40006000},	
	{0x4000f004 , 0x00008036},	
	{0x4000f00c , 0x80000000},	
	{0x4000f104 , 0x80000000},
	{0x4000f204 , 0x80000000},
	{0x4000f304 , 0x80000000},
	{0x4000f404 , 0x80000000},
	{0x4000f504 , 0x80000000},
	{0x4000f604 , 0x80000000},
	{0x4000f704 , 0x80000000},
	//prepare tx packet
	{0x40006000 , 0x01010100},	
	{0x40006004 , 0x02020101},	
	{0x40006008 , 0x01020202},	
	{0x4000600c , 0xa5a56400},	
	{0x40006010 , 0xa5a5a5a5},	
	{0x40006014 , 0xa5a5a5a5},	
	{0x40006018 , 0xa5a5a5a5},	
	{0x4000601c , 0xa5a5a5a5},	
	{0x40006020 , 0xa5a5a5a5},	
	{0x40006024 , 0xa5a5a5a5},	
	{0x40006028 , 0xa5a5a5a5},	
	{0x4000602c , 0xa5a5a5a5},	
	{0x40006030 , 0xa5a5a5a5},	
	{0x40006034 , 0xa5a5a5a5},	
	{0x40006038 , 0xa5a5a5a5},	
	{0x4000603c , 0xa5a5a5a5},	
	{0x40006040 , 0xa5a5a5a5},	
	{0x40006044 , 0xa5a5a5a5},	
	{0x40006048 , 0xa5a5a5a5},	
	{0x4000604c , 0xa5a5a5a5},	
	{0x40006050 , 0xa5a5a5a5},	
	{0x40006054 , 0xa5a5a5a5},	
	{0x40006058 , 0xa5a5a5a5},	
	{0x4000605c , 0xa5a5a5a5},	
	{0x40006060 , 0xa5a5a5a5},	
	{0x40006064 , 0xa5a5a5a5},	
	{0x40006068 , 0xa5a5a5a5},	
	{0x4000606c , 0xa5a5a5a5},	
	{0x40006070 , 0xb7f6a5a5},	
	{0x40006074 , 0x0000e73d},	
	{0x40006078 , 0x00000000},	
	{0x4000607c , 0x00000000},	
	//prepare rx descriptor
	{0x4000E000 , 0x40000002},	
	{0x4000E100 , 0x40100001},	
	{0x4000E200 , 0x40200001},	
	{0x4000E300 , 0x40300001},	
	{0x4000E400 , 0x40400001},	
	{0x4000E500 , 0x40500001},	
	{0x4000E600 , 0x40600001},	
	{0x4000E700 , 0x40700001},		
	//check and initialize the relavent registers
	{CONFIG_GMAC_BASE+0x28 , 0xffffffff},	
	//{CONFIG_GMAC_BASE+0x04 , 0x08080c03},
	{CONFIG_GMAC_BASE+0x04 , 0xee280813},	
	{CONFIG_GMAC_BASE+0x18 , 0x4000e000},	
	{CONFIG_GMAC_BASE+0x1c , 0x4000f000},	
	{CONFIG_GMAC_BASE+0x88 , 0x01010001},	
	{CONFIG_GMAC_BASE+0x8c , 0x00000101},		
	//{0xc100008c , 0x00000101},		
	{CONFIG_GMAC_BASE+0x440 , 0x4000f100},	
	{CONFIG_GMAC_BASE+0x444 , 0x4000f200},	
	{CONFIG_GMAC_BASE+0x448 , 0x4000f300},	
	{CONFIG_GMAC_BASE+0x44c , 0x4000f400},	
	{CONFIG_GMAC_BASE+0x450 , 0x4000f500},	
	{CONFIG_GMAC_BASE+0x454 , 0x4000f600},	
	{CONFIG_GMAC_BASE+0x458 , 0x4000f700},
	{CONFIG_GMAC_BASE+0x480 , 0x4000e100},	
	{CONFIG_GMAC_BASE+0x484 , 0x4000e200},	
	{CONFIG_GMAC_BASE+0x488 , 0x4000e300},	
	{CONFIG_GMAC_BASE+0x48c , 0x4000e400},
	{CONFIG_GMAC_BASE+0x490 , 0x4000e500},	
	{CONFIG_GMAC_BASE+0x494 , 0x4000e600},	
	{CONFIG_GMAC_BASE+0x498 , 0x4000e700},	
	//{CONFIG_GMAC_BASE+0x200 , 0x0000c140},//ewrap here
	//sart
	{CONFIG_GMAC_BASE , 0x0000020c},	
};

#ifndef GEM_REG32
#define GEM_REG32(off)		*(volatile uint32_t *)(CONFIG_GMAC_BASE + (off))
#endif


#ifndef NORMAL_REG32
#define NORMAL_REG32(off)		*(volatile uint32_t *)((off))
#endif

static uint32_t READ(uint32_t addr)
{
	return NORMAL_REG32(addr);
}

static void WRITE(uint32_t addr, uint32_t value)
{
	NORMAL_REG32(addr) = value;
	udelay(10000);
	//printf("write %x with %x , read %x\n",addr,value,READ(addr));
}

void lte_int_vesmi(unsigned int mem_addr_base, struct init_seq *seq, int entry_size)
{
	int i;
	for(i= 0; i < entry_size;i++){				
		WRITE(mem_addr_base+(seq[i].addr)*4 , seq[i].value);
		udelay(130);
	}
}
		
void serdes_help(void)
{
	printf("[1] initialize serdes0 (pcie gen2)\n");	
	printf("[2] initialize serdes1 (pcie gen3)\n");	
	printf("[3] initialize serdes2 (GMAC)\n");	
	printf("\n");
	printf("[6] sgmii(internal loopback test)\n");	
	printf("[7] sgmii(external loopback test)\n");	
	printf("[n] print gem controller statistics\n");	
	printf("\n");
	printf("[b] bist\n");
	printf("[l] serdes loopback mode\n");
	printf("[e] serdes insert error\n");
	printf("[q] quit\n");
}

void gem_print_statistics(void)
{
	printf("Tx \t  Rx\n");
	printf("%-8d  %-8d\tFrames\n",GEM_REG32(GEM_STATISTICS_TX_FRAME),GEM_REG32(GEM_STATISTICS_RX_FRAME));
	printf("%-8d  %-8d\tFrames:64\n",GEM_REG32(GEM_STATISTICS_TX_FRAME_64),GEM_REG32(GEM_STATISTICS_RX_FRAME_64));
	printf("%-8d  %-8d\tFrames:65-127\n",GEM_REG32(GEM_STATISTICS_TX_FRAME_65_127),GEM_REG32(GEM_STATISTICS_RX_FRAME_65_127));
	printf("%-8d  %-8d\tFrames:127-255\n",GEM_REG32(GEM_STATISTICS_TX_FRAME_128_255),GEM_REG32(GEM_STATISTICS_RX_FRAME_128_255));
	printf("%-8d  %-8d\tFrames:256-511\n",GEM_REG32(GEM_STATISTICS_TX_FRAME_256_511),GEM_REG32(GEM_STATISTICS_RX_FRAME_256_511));
	printf("%-8d  %-8d\tFrames:512-1023\n",GEM_REG32(GEM_STATISTICS_TX_FRAME_512_1023),GEM_REG32(GEM_STATISTICS_RX_FRAME_512_1023));
	printf("%-8d  %-8d\tFrames:1024-1518\n",GEM_REG32(GEM_STATISTICS_TX_FRAME_1024_1518),GEM_REG32(GEM_STATISTICS_RX_FRAME_1024_1518));
	printf("%-8d  %-8d\tOctects(High)\n",GEM_REG32(GEM_STATISTICS_TX_OCTECTS_HIGH),GEM_REG32(GEM_STATISTICS_RX_OCTECTS_HIGH));
	printf("%-8d  %-8d\tOctects(Low)\n",GEM_REG32(GEM_STATISTICS_TX_OCTECTS_LOW),GEM_REG32(GEM_STATISTICS_RX_OCTECTS_LOW));		
}

void gem_loopback_test(struct init_seq *seq, int entry_size)
{
	int i;
	
	for(i= 0; i < entry_size;i++){				
		WRITE((seq[i].addr) ,seq[i].value);
	}		
	
}

void lte_init_sgmii(void)
{
	uint32_t val;
	
	val = READ(SYSC_REG_BASE + 0x00001310);
	val &= ~((0x01<<20) | (0x01<<21));
	//--- assert controller reset
	WRITE( (SYSC_REG_BASE + 0x00001310), val);

	//---------------------------------------------------------------------------------------------
	// Section 0 : Init Vsemi serdes
	//---------------------------------------------------------------------------------------------

	//--- release IRST_POR_B_A
	//WRITE( (SYSC_REG_BASE + 0x00000674), 0x0083FE1E);
	WRITE( (SYSC_REG_BASE + 0x00000674), (READ(SYSC_REG_BASE + 0x00000674) | 0x00800000));

	//--- program Vsemi serdes
    lte_int_vesmi(SERDES_GEM_BASE, pwrup_1_25gbps , ARRAY_SIZE(pwrup_1_25gbps));  	  


//add ewrap here
	WRITE( (CONFIG_GMAC_BASE + 0x200), READ((CONFIG_GMAC_BASE+0x200) | 0x00000001 << 14 ));
//end 
	//--- release IRST_MULTI_HARD_SYNTH_B and IRST_MULTI_HARD_TXRX_Ln
	
	//--- wait serdes RX ready (RX_READY & RX_STATUS)
#ifndef CONFIG_BOARD_A380_VP
	while ( (READ(SYSC_REG_BASE + 0x00000600) & 0x00003333) != 0x00003333) {}
#else
	while ( (READ(SYSC_REG_BASE + 0x00000600) & 0x00003333) != 0x00001313) {}
#endif

	//--- wait serdes TX status (TX_STATUS)
	while ( (READ(SYSC_REG_BASE + 0x00000618) & 0x00000003) != 0x00000003) {}

	//--- wait serdes TX ready (TX_READY)
	while ( (READ(SYSC_REG_BASE + 0x00000614) & 0x00003300) != 0x00003300) {}


	//--- de-assert controller reset
	//WRITE( (SYSC_REG_BASE + 0x00001300), 0xFFFFFFFF);
/*
	printf("loopback ? [1] Yes [2] No\n");
    input_char = getc();  
    if(input_char == '1'){
	    WRITE( (LTE_SERDES_MEM_3_BASE + 0x00038008), 0x14 );  
	    WRITE( (LTE_SERDES_MEM_3_BASE + 0x00038014), 0x20 );  //loopback
    }
*/ 
	//--- controller reset toggle test
	val = READ(SYSC_REG_BASE + 0x00001310);
	val |= (0x01<<20) | (0x01<<21);
	//--- assert controller reset
	WRITE( (SYSC_REG_BASE + 0x00001310), val);
	
	//WRITE( (SYSC_REG_BASE + 0x00001300), 0xFFF0FFFF);
	//WRITE( (SYSC_REG_BASE + 0x00001300), 0xFFFFFFFF);
	//WRITE( (SYSC_REG_BASE + 0x00001300), 0xFFF0FFFF);
	//WRITE( (SYSC_REG_BASE + 0x00001300), 0xFFFFFFFF);

}

void lte_enable_serdes_internal_loopback(unsigned int serdes_no)
{
	switch(serdes_no){
		case 0:
			WRITE(SERDES_MEMORY_BASE0 + 0X0038008, 0X14);
			WRITE(SERDES_MEMORY_BASE0 + 0X0038014, 0X20);
			break;
			
		case 1:
			WRITE(SERDES_MEMORY_BASE1 + 0X0038008, 0X14);
			WRITE(SERDES_MEMORY_BASE1 + 0X0038014, 0X20);
			break;
			
		case 2:
			WRITE(SERDES_MEMORY_BASE2 + 0X0038008, 0X14);
			WRITE(SERDES_MEMORY_BASE2 + 0X0038014, 0X20);
			break;
			
		case 5://all serdes
			WRITE(SERDES_MEMORY_BASE0 + 0X0038008, 0X14);
			WRITE(SERDES_MEMORY_BASE0 + 0X0038014, 0X20);
			WRITE(SERDES_MEMORY_BASE1 + 0X0038008, 0X14);
			WRITE(SERDES_MEMORY_BASE1 + 0X0038014, 0X20);
			WRITE(SERDES_MEMORY_BASE2 + 0X0038008, 0X14);
			WRITE(SERDES_MEMORY_BASE2 + 0X0038014, 0X20);
			break;
	}	
}

void test_serdes(void)
{
	int input_char;

	printf("Select [1] pcie gen2 , [2] pcie gen3 , [3] GMAC\n");
	input_char = getc();			
	switch(input_char){
		case '1':
			break;

		case '2':
			break;

		case '3'://sgmii;
			gem_loopback_test(external_loopback,ARRAY_SIZE(external_loopback));
			gem_print_statistics();
			break;
	}
	printf("\n######Success########\n\n");
}

void serdes_dump_status(unsigned int serdes_addr)
{
	int i;
	int addr = (serdes_addr + 0xe004*4) & 0xfff00fff;
	
	printf("Serdes Addr : %x\n", serdes_addr);
	for(i=0;i < 4; i++)
	{
		printf("Lane: %3d ",i);
		printf("Addr %08x :%08x\n",addr ,READ(addr | (i<<15))); 
	}
}

void serdes_dump_serdes_status(unsigned int serdes_addr)
{
	int i;
	int addr = (serdes_addr + 0xe006*4) & 0xfff00fff;
	
	printf("PCSOUTRX,TX (e006)\n");
	for(i=0;i < 4; i++)
	{
		printf("Lane: %3d ",i);
		printf("Addr %08x :%08x\n",addr ,READ(addr | (i<<15))); 
	}
	printf("\n");
	addr = (serdes_addr + 0xe009*4) & 0xfff00fff;
	printf("PCSSTATUE_RX (e009)\n");
	for(i=0;i < 4; i++)
	{
		printf("Lane: %3d ",i);
		printf("Addr %08x :%08x\n",addr, READ(addr | (i<<15))); 
	}
	printf("\n");
	addr = (serdes_addr + 0xe00a*4) & 0xfff00fff;
	printf("PCSSTATUE_TX(e00a)\n");
	for(i=0;i < 4; i++)
	{
		printf("Lane: %3d ",i);
		printf("Addr %08x :%08x\n",addr, READ(addr | (i<<15))); 
	}
	printf("\n");
	addr = (serdes_addr + 0x8018*4);
	printf("PCSOUNTSYNTH_READY,LOCKSTATUS, SYHTNH_STATUS (0x8018)\n");
	printf("Addr %08x :%08x\n",addr ,READ(addr)); 
	printf("\n");
	addr = (serdes_addr + 0x801a*4);
	printf("PCSSTATE_SYNTH (0x801a)\n");
	printf("Addr %08x :%08x\n",addr ,READ(addr)); 	
}

static void membus_test(void)
{
	int i =0;
	int input_char;
	uint32_t serdes_addr;
	
	printf("Select serdes number to test[1][2][3]\n");
	input_char = getc();
	switch(input_char){
		case '1':
			serdes_addr = SERDES_MEMORY_BASE0;
			break;
		case '2':
			serdes_addr = SERDES_MEMORY_BASE1;
			break;
		case '3':
			serdes_addr = SERDES_MEMORY_BASE2;
			break;						
	}
	printf("Testing %x\n",serdes_addr);
	for(;;){
		if(tstc()){
			getc();
			break;
		}			
		WRITE(serdes_addr+0x20054 ,i);		
		if(READ(serdes_addr+0x20054) != i ){
			printf("error! i=%x read %x\n",i , READ(serdes_addr+0x20054));
		}
		i = (i+1)%255;
	}
}

#define sw(a,b,c) WRITE(a,b)

static void a380_pcie_gen3_init(void)
{	
    // Link Training Enable
    sw(PCIE_WRAP_BASE + 0x4, 0x1, HSIZE_WORD);
		// disable serdes tx detect rx
		sw(0x92700000+(0xe028*4), 0, HSIZE_WORD);
    // pipe reset de-assert
    WRITE(SYSC_REG_BASE + 0x484, 0x00700000); 

    //PCIe Mode Select is set to RC mode
    sw(PCIE_WRAP_BASE + 0x0, 0x212, HSIZE_WORD);


    // Waiting for Linking to L0 State

    while(1) {
        if( (READ(PCIE_WRAP_BASE + 0x50) & 0x1f) == 0x10 )  // Ltssm state
            if( (READ(PCIE_WRAP_BASE + 0x54) & 0x3000) != 0x0 ) // Negotiated speed
                break;
    }

    // Set Config Enable Reg
    sw(PCIE_WRAP_BASE + 0x40, 0x1, HSIZE_WORD);

    // (0x1) Set Command Reg
    // 0 : IO-space enable
    // 1 : Mem-space enable
    // 2 : Bus-Master enable
    sw(PCIE_G3_REG_BASE + 0x4, 0x7, HSIZE_WORD);

    // (0x32) Set PCIe Device Control Reg
    // 4 : Enable Relaxed Ordering
    // 11 : Enable No snoop
    sw(PCIE_G3_REG_BASE + 0xc8, 0x810, HSIZE_WORD);

    // (0x3a) Set PCIe Device Control Reg 2 : timeout setting
    sw(PCIE_G3_REG_BASE + 0xe8, 0x0, HSIZE_WORD);

    // (0x34) Set Link Control and Status Reg
    // 1:0 : Active State Power Management Control
    sw(PCIE_G3_REG_BASE + 0xd0, 0x3, HSIZE_WORD);

    // (0x82) Set Debug Mux Control Reg
    sw(PCIE_G3_REG_BASE + 0x100208, 0x80000008, HSIZE_WORD);

    //?????????? RC no such register
    // (0x24) Set MSI Control Reg
    // 16 : MSI Enable
    sw(PCIE_G3_REG_BASE + 0x90, 0x10000, HSIZE_WORD);	
}

#endif


#ifdef CONFIG_LWIP
extern int httpd_main(void);
int do_a380httpd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	httpd_main();
	return 0;
}
#endif

int do_a380(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int input_char = 0;
	int input_char2;
	uint32_t serdes_addr = 0;
		
	while(input_char != 'q'){
		serdes_help();
		input_char = getc();
		printf("\n");
		switch(input_char){			
			case '1' :
				break;
			case '2' :
				lte_int_vesmi(0x92700000, pwrup_8gbps_bist, ARRAY_SIZE(pwrup_8gbps_bist));  					
				break;
			case '3' :
				lte_init_sgmii();			
				break;
			case '4' :
				a380_pcie_gen3_init();
				break;
			case '5' :
				break;				
			case '6' : //sgmii(internal)
				gem_loopback_test(internal_loopback,ARRAY_SIZE(internal_loopback));
				break;
			case '7':  //sgmii(external)
				gem_loopback_test(external_loopback,ARRAY_SIZE(external_loopback));
				break;
			//bist
			case 'b'://bist serdes0
				printf("Serdes bits :\n");
				printf("Select [1] pcie gen2 , [2] pcie gen3 , [3]sgmii\n");
				input_char = getc();
				if(input_char == '1')
					serdes_addr = SERDES_MEMORY_BASE0;
				
				if(input_char == '2'){
					serdes_addr = SERDES_MEMORY_BASE1;
					lte_int_vesmi(serdes_addr, pwrup_8gbps_bist , ARRAY_SIZE(pwrup_8gbps_bist));
					printf("Insert errors...\n");
					for(;;){
						if(tstc()){
							getc();
							break;
						}					
						if(READ(serdes_addr+0x28)!=7){
							printf("Bist check fail, counter [%d]\n",READ(serdes_addr+0x28));
						}else{
							printf("Bist check ok, counter [%d]\n",READ(serdes_addr+0x28));
						}
						udelay(20000000);
					}
					break;
				}
				if(input_char == '3'){
					serdes_addr = SERDES_MEMORY_BASE2;
					WRITE(0x90a00674,0x0080e600);//POR
				}
				printf("Speed select : [1] 1.25G  [2] 2.5g  [3] 3.125G [4] 5G\n");
				input_char2 = getc();
				printf("serdes addr %x\n",serdes_addr);
				switch(input_char2){
					case '1'://1.25G
						lte_int_vesmi(serdes_addr, pwrup_1_25gbps_bist , ARRAY_SIZE(pwrup_1_25gbps_bist));  	  
						break;
					case '2': //2.5G
						lte_int_vesmi(serdes_addr, pwrup_2_5gbps_bist , ARRAY_SIZE(pwrup_2_5gbps_bist));  	  
						break;
					case '3': //3.125G
						lte_int_vesmi(serdes_addr, pwrup_3_125gbps_bist , ARRAY_SIZE(pwrup_3_125gbps_bist));  	  
						break;
					case '4': //5G
						lte_int_vesmi(serdes_addr, pwrup_5gbps_bist , ARRAY_SIZE(pwrup_5gbps_bist));  	  
						break;
				}				
				
				printf("Serdes internal loopback ? [1] Yes [2] No\n");
				if(getc()=='1')
					lte_enable_serdes_internal_loopback(input_char - '1');

				udelay(10000);
				lte_int_vesmi(serdes_addr, serdes_bist, ARRAY_SIZE(serdes_bist));	
				udelay(10000);			
				break;

			case 'l'://loopback mode
				printf("Loopback mode:\n");
				printf("Select [1] pcie gen2 , [2] pcie gen3 , [3]sgmii\n");
				input_char = getc();
				lte_enable_serdes_internal_loopback(input_char - '1');
				break;

			case 'e'://insert error
				printf("Insert error:\n");
				printf("Select [1] pcie gen2 , [2] pcie gen3 , [3]sgmii\n");
				input_char = getc();
				switch(input_char){
					case '1':
						lte_int_vesmi(SERDES_MEMORY_BASE0, lte_insert_error, ARRAY_SIZE(lte_insert_error));
						serdes_addr = SERDES_MEMORY_BASE0; 
						//serdes_dump_serdes_status(SERDES_MEMORY_BASE0);
						break;
					case '2':
						lte_int_vesmi(SERDES_MEMORY_BASE1, lte_insert_error, ARRAY_SIZE(lte_insert_error));
						serdes_addr = SERDES_MEMORY_BASE1; 
						//serdes_dump_serdes_status(SERDES_MEMORY_BASE1);
						break;
					case '3':
						lte_int_vesmi(SERDES_MEMORY_BASE2, lte_insert_error, ARRAY_SIZE(lte_insert_error));
						serdes_addr = SERDES_MEMORY_BASE2; 
						//serdes_dump_serdes_status(SERDES_MEMORY_BASE2);
						break;
				}		
				for(;;){
					if(tstc()){
						 getc();
						 break;
					}
					serdes_dump_status(serdes_addr);
					udelay(20000000);
				}
				break;
			case 'm'://membus read/write
				membus_test();
				break;
			case 'n': //print eth statistics
				gem_print_statistics();
				break;				
				
			case 't': //test serdes
				test_serdes();	
				break;
				
			case 'd': //dump serdes status
				printf("Dump serdes status, which one? [1~3]\n");
				input_char = getc();
				switch(input_char)
				{
					case '1':
						serdes_dump_serdes_status(SERDES_MEMORY_BASE0);	
						break;						
					case '2':
						serdes_dump_serdes_status(SERDES_MEMORY_BASE1);	
						break;
												
					case '3':
						serdes_dump_serdes_status(SERDES_MEMORY_BASE2);	
						break;		
				}				
				break;
		}
	}
	return 1;
}

U_BOOT_CMD(
	a380,	2,	1,	do_a380,
	"a380 related functions",
	""
);
#endif

int do_tmr(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  printf("Enable FTMMR010_0\n");

  writel(0x00300000, 0x20200004);
  writel(0x00100000, 0x20200008);
  writel(0x00300000, 0x20200014);
  writel(0x00100000, 0x20200008);
  writel(0x2e, 0x20200030);
  printf("done\n");
}
#ifdef CONFIG_SFU_PLATFORM
  #define TMR_BASE  			0x90200000 //TMR_1 = 0x90300000
  #define TIMER_IRQ 			6  // if TMR_1, IRQ = 10
#elif CONFIG_BOARD_ECHONOUS
  #define TMR_BASE  			0x10100000 //TMR_1 = 0x90300000
  #define TIMER_IRQ 			8  // if TMR_1, IRQ = 10
#elif CONFIG_ARCH_GC1610
  #define TMR_BASE  			0x20200000
  #define TIMER_IRQ 			34 + 32  // if TMR_1, IRQ = 10

#else  
  #define TMR_BASE  			0x90B00000 //TMR_1 = 0x90300000
  #define TIMER_IRQ 			(13 + 32)
#endif
#define TM1COUNTER 0x0
#define TM1LOAD    0x4
#define TM1MATCH1  0x8
#define TM1MATCH2  0xC
#define TMCR       0x30 
#define INTRSTATE  0x34 
#define INTRMASK 	 0x38

#define GICC_AIAR 0x20

static int gicc_base;


static void tmr_interrupt(void *arg) 
{ 

	irq_disable(TIMER_IRQ);
	
	printf("[Rido]INTRSTATE: 0x%x\n", readl(TMR_BASE + INTRSTATE));
	
	/*
	0  Tm1Match1  Tm1Match1 interrupt
			0: No effect
			1: Tmr1 counter value equals to the value in the Tm1Match1 register.	
	1  Tm1Match2  Tm1Match2 interrupt
			0: No effect
			1: Tmr1 counter value equals to the value in the Tm1Match2 register.
	2  Tm1Overflow  Tm1Overflow interrupt
			0: No effect
			1: Tmr1 counter overflows.
	*/
//	writel(0x7, TMR_BASE + INTRSTATE);
	//irq_acknowledge(TIMER_IRQ);
	printf("[Rido]Clear interrupt, INTRSTATE: 0x%x\n", readl(TMR_BASE + INTRSTATE));
	writel(readl(TMR_BASE + INTRSTATE), TMR_BASE + INTRSTATE);
	
#if 1
		writel(0xA, PLATFORM_FTUART010_BASE);  // "\n"
		writel(0x5B, PLATFORM_FTUART010_BASE);  // "["
		writel(0x52, PLATFORM_FTUART010_BASE);  // "R"
		writel(0x49, PLATFORM_FTUART010_BASE);  // "I"
		writel(0x44, PLATFORM_FTUART010_BASE);  // "D"
		writel(0x4F, PLATFORM_FTUART010_BASE);  // "O"
		writel(0x5D, PLATFORM_FTUART010_BASE);  // "]"
		
		writel(0x74, PLATFORM_FTUART010_BASE);  // "t"
		writel(0x6d, PLATFORM_FTUART010_BASE);  // "m"
		writel(0x72, PLATFORM_FTUART010_BASE);  // "r"
		
		writel(0x5f, PLATFORM_FTUART010_BASE);  // "_"
		
		writel(0x69, PLATFORM_FTUART010_BASE);  // "i"
		writel(0x6e, PLATFORM_FTUART010_BASE);  // "n"
		writel(0x74, PLATFORM_FTUART010_BASE);  // "t"
		writel(0x65, PLATFORM_FTUART010_BASE);  // "e"
		writel(0x72, PLATFORM_FTUART010_BASE);  // "r"
		writel(0x72, PLATFORM_FTUART010_BASE);  // "r"
		writel(0x75, PLATFORM_FTUART010_BASE);  // "u"
		writel(0x70, PLATFORM_FTUART010_BASE);  // "p"
		writel(0x74, PLATFORM_FTUART010_BASE);  // "t"
		
		writel(0xA, PLATFORM_FTUART010_BASE);  // "\n"
	       writel(0x0, TMR_BASE + TMCR);
	       writel(0x0, TMR_BASE);
		writel(0x0, TMR_BASE + TM1COUNTER);	
		irq_free_handler(TIMER_IRQ);
#else
		writel(0x2e, TMR_BASE);  // "."		
#endif	
}

void timer_test(void)
{
  irq_install_handler(TIMER_IRQ, tmr_interrupt, 0);
  writel(0x00300000, PLATFORM_FTTMR010_BASE+0x14);
  writel(0x00100000, PLATFORM_FTTMR010_BASE+0x18);
  writel(0x28, PLATFORM_FTTMR010_BASE+0x30);		
  
}


int do_gc6(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
  timer_test();
  //writel(0x4,0x400108);//int 34 unable
  irq_enable(TIMER_IRQ);
    
  printf("done\n");
}

U_BOOT_CMD(
	gc6,	1,	1,	do_gc6,
	"gc6 related functions",
	""
);

U_BOOT_CMD(
	tmr,	1,	1,	do_tmr,
	"tmr related functions",
	""
);
#ifdef CONFIG_LWIP
U_BOOT_CMD(
	a380httpd,	1,	1,	do_a380httpd,
	"a380 httpd",
	""
);
#endif