/*
 * (C) Copyright 2020 Faraday Technology
 * Bo-Cun Chen <bcchen@faraday-tech.com>
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
#include <linux/delay.h>
#include <asm/arch/bits.h>

#if defined(CONFIG_NAND_FTNANDC024)
#include <faraday/ftnandc024.h>
#include <faraday/nand.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define CPU_RELEASE_MAGIC_VALUE                 0x76543210
#define CPU_RELEASE_JUMPPC_VALUE               CONFIG_SYS_TEXT_BASE
#define CPU_RELEASE_MAGIC_ADDR          		0x201008a8
#define CPU_RELEASE_JUMPPC_ADDR                0x201008ac

#define FTSCU100_SCU_BASE 		0x20108000
#define SCU_EXT_BASE 			0x20108000

#define PCIE0_APB_REG_BASE  	0x22100000
#define PCIE_SLV_DBI_ENABLE	BIT(21)
#define PCIE_PHY_DATA_CONTROL_OFFSET		0x0
#define PCIE_PHY_DATA_OFFSET				0x4
#define PCIE_SLV_AW_MISC					0x250
#define PCIE_SLV_AR_MISC					0x25C

#define PCIE_SIDEBAND00                       0x0
#define PCIE_SIDEBAND01                       0x4
#define PCIE_SIDEBAND02                       0x8
#define PCIE_SIDEBAND03                       0xc
#define PCIE_SIDEBAND40                       0xa0
#define PCIE_SIDEBAND59                       0xec
#define PCIE_SIDEBAND72				   0x120
#define PCIE_SIDEBAND93				   0x260


#define WRITE(addr,val) 	writel(val,addr)
#define READ(addr)		readl(addr)
#define sw(addr,val)		writel(val,addr)


#define __sev()   __asm__ __volatile__ ("sev" : : : "memory")

#define WRITE(addr,val) writel(val,addr)


extern void clock_init(void);

/* System Control Uint (pinmux) */
static void pinmux_init(void)
{
}

int board_early_init_f(void)
{
#ifndef CONFIG_VDK_GC1610
	writel(0x55000000,FTSCU100_SCU_BASE + 0x844);
	writel(0x00055555,FTSCU100_SCU_BASE + 0x848);
#endif
	pinmux_init();
	clock_init();
	return 0;
}
#if defined(CONFIG_NAND_FTNANDC024)

#define CFG_START_CE			0
#define CFG_BI_RESERVE_SMALL	6
#define CFG_BI_RESERVE_LARGE	1

static struct nand_chip nand_chip;
static struct platform_nand_flash platform_nand;

void board_nand_init(void)
{
#ifndef CONFIG_DM_NAND
	int devnum = 0;
	struct mtd_info *mtd;

	nand_chip.page_shift = 11;	/* 2048 */
	//chip->phys_erase_shift = chip->page_shift + 5; //block = 32 pages per block
	nand_chip.phys_erase_shift = nand_chip.page_shift + 6; //block = 64 pages per block
	platform_nand.row_addrcycles = 3;

	if (nand_chip.page_shift == 9) {
		platform_nand.col_addrcycles = 1;
		platform_nand.ecc_bits_dat = 2;
		platform_nand.ecc_bits_oob = 1;//according to signoff form. same as rom code
		if (ftnandc024_init(&nand_chip, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_SMALL))
			goto bni_err;
	} else {
		platform_nand.col_addrcycles = 2;
		platform_nand.ecc_bits_dat = 4;
		platform_nand.ecc_bits_oob = 2;//according to signoff form. same as rom code
		if (ftnandc024_init(&nand_chip, CONFIG_SYS_NAND_BASE, CONFIG_SYS_NAND_DATA_BASE, CFG_START_CE, &platform_nand, CFG_BI_RESERVE_LARGE))
			goto bni_err;
	}

	mtd = nand_to_mtd(&nand_chip);

	if (mtd->writesize && ((1 << nand_chip.page_shift) != mtd->writesize)) {
		printf("ftnandc024: page size is supposed to be %d, not %d\n",
			mtd->writesize, 1 << nand_chip.page_shift);
	}

	if (mtd->erasesize && ((1 << nand_chip.phys_erase_shift) != mtd->erasesize)) {
		printf("ftnandc024: block size is supposed to be %d, not %d\n",
			mtd->erasesize, 1 << nand_chip.phys_erase_shift);
	}

	nand_register(devnum,mtd);

	return;

bni_err:
	free(mtd->priv);
#else
    return;
#endif
}



#define FTNANDC024_REG_BASE 	0x10000000
#define FTNANDC024_BASE 		0x10100000
#define FTNANDC024_NANDC_MEM 0x108
#define FTNANDC024_ECC_CTL  	0x8
#define FTNANDC024_NANDC_AC0_CH0 0x110
#define FTNANDC024_NANDC_AC1_CH0 0x114
#define FTNANDC024_NANDC_AC2_CH0 0x118
#define FTNANDC024_NANDC_AC3_CH0 0x11C
#define FTNANDC024_NANDC_INT_EN 	0x150
#define FTNANDC024_NANDC_INT_STS	0x154
#define FTNANDC024_BMC_REG_STS 	0x400
#define FTNANDC024_SP_SRAM_BASE 0x1000
#define FTNANDC024_CMDQUE_CTL_CH0	0x30c
#define FTNANDC024_CMDQUE_CNT_CH0 	0x308
#define FTNANDC024_CMDQUE_IDX_CH0 	0x300

#define FTNANDC024_PAGE_READ_SP_BASE 	0x48
#define FTNANDC024_BK_ERASE_BASE 	0x68
#define FTNANDC024_RESET_BASE 	0x65
#define FTNANDC024_PAGE_WRITE_SP_BASE 0x26
void nand_test()
{
	unsigned int read_data = 0;
	unsigned int i=0;
	printf("nand_test\n");

writel(0x55000000, FTSCU100_SCU_BASE + 0x844);
writel(0x00055555, FTSCU100_SCU_BASE + 0x848);

// Register setting
// Memory attribute setting

//sw(FTNANDC024_REG_BASE+FTNANDC024_NANDC_MEM, (0x02<<16)+(0x5<<12)+(0xff<<2)+(0x0<<0), HSIZE_WORD);//original nand model
writel((0x01<<16)+(0x3<<12)+(0xff<<2)+(0x0<<0) ,FTNANDC024_REG_BASE+FTNANDC024_NANDC_MEM);


// ECC engine setting
writel((1<<8) , FTNANDC024_REG_BASE+FTNANDC024_ECC_CTL);

// Channel 0 AC timing
writel( (0xA<<24)+(0xA<<16)+(0xA<<8)+(0xA<<0) , FTNANDC024_REG_BASE+FTNANDC024_NANDC_AC0_CH0);

writel((0x0<<24)+(0x0<<16)+(0x40<<8)+(0xA<<0) , FTNANDC024_REG_BASE+FTNANDC024_NANDC_AC1_CH0);

writel((0x40<<24)+(0x40<<16)+(0x40<<8)+(0x40<<0), FTNANDC024_REG_BASE+FTNANDC024_NANDC_AC2_CH0);

writel((0xA<<24)+(0x4<<16)+(0x3<<8)+(0x3<<0) , FTNANDC024_REG_BASE+FTNANDC024_NANDC_AC3_CH0);

// Interrupt enable setting
writel(0xffff , FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_EN);

// Prepare spare data
for (i=0; i<64; i=i+4) {
        writel(((i+3)<<24)+((i+2)<<16)+((i+1)<<8)+i, FTNANDC024_REG_BASE+FTNANDC024_SP_SRAM_BASE+i);
}        

// Reset one block
printf("NAND Channel 0 Flash reset\n");

writel((FTNANDC024_RESET_BASE<<8)+0x1 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CTL_CH0);

// Wait for command complete and clear complete status
read_data = 0;
while( read_data != 0x00010000 ){
        read_data = readl(FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);
        read_data = read_data & 0x00010000 ;
}
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);

// Block erase
printf("NAND channel 0 block erase\n");

writel(0xff80, FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_IDX_CH0);
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CNT_CH0);

writel((FTNANDC024_BK_ERASE_BASE<<8)+0x05 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CTL_CH0);

// Wait for command complete and clear complete status
read_data = 0;
while( read_data != 0x00010000 ){
	read_data  = readl(FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);
        read_data = read_data & 0x00010000 ;
}
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);

// Sector write with SPARE
printf("NAND channel 0 sector write with spare\n");
writel(0xff80, FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_IDX_CH0);
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CNT_CH0);

writel((0x1<<19)+(FTNANDC024_PAGE_WRITE_SP_BASE<<8)+0x01 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CTL_CH0);

// Write data
for (i=0; i<512; i=i+1){
        writel(0x12345678 , FTNANDC024_BASE);
}
// Wait for command complete and clear complete status
read_data = 0;
while( read_data != 0x00010000 ){
        read_data  = readl(FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);
        read_data = read_data & 0x00010000 ;
}
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);


//===========================================================================
// Block erase
printf("NAND channel 0 block erase\n");

writel(0xff80, FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_IDX_CH0);
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CNT_CH0);

writel((FTNANDC024_BK_ERASE_BASE<<8)+0x05 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CTL_CH0);

// Wait for command complete and clear complete status
read_data = 0;
while( read_data != 0x00010000 ){
	read_data  = readl(FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);
        read_data = read_data & 0x00010000 ;
}
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);

// Sector write with SPARE
printf("NAND channel 0 sector write with spare\n");

writel(0xff80, FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_IDX_CH0);
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CNT_CH0);

writel((0x1<<19)+(FTNANDC024_PAGE_WRITE_SP_BASE<<8)+0x01 , FTNANDC024_REG_BASE+FTNANDC024_CMDQUE_CTL_CH0);

// Write data
for (i=0; i<512; i=i+1){
        writel(0x12345678 , FTNANDC024_BASE);
}
// Wait for command complete and clear complete status
read_data = 0;
while( read_data != 0x00010000 ){
        read_data  = readl(FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);
        read_data = read_data & 0x00010000 ;
}
writel(0x1<<16 , FTNANDC024_REG_BASE+FTNANDC024_NANDC_INT_STS);



	printf("nand_test done\n");
}
#endif
void pcie_loopback()
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



	printf("Wait for LTSSM LOOPBACK");

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

	sw(0x40300000 + 0x0108, 0x40000000); //inbound iATU Lower Base Address 
	sw(0x40300000 + 0x010c, 0x00000000); //inbound iATU Higher Base Address 
	sw(0x40300000 + 0x0110, 0x4fffffff); //inbound iATU (Lower) Limit Address 
	sw(0x40300000 + 0x0114, 0x00200000); //inbound iATU Lower Target Address  //RAM FTEMC030_1
	sw(0x40300000 + 0x0118, 0x00000000); //inbound iATU Higher Target Address 
	sw(0x40300000 + 0x0100, 0x00000000); //inbound iATU Region Control 1 (Region is MEM)
	sw(0x40300000 + 0x0104, 0x80000000); //inbound iATU Region Control 2 (Region enable)

	////PCIE controller awmisc setting -----------------------------//
	sw(0x22100000 + 0x0250, 0x00000000);  //set awmisc = 22'h000000 //DBI disable

	// sw(0x40000000 + 0x4000, 0x55aa1234, HSIZE_WORD); //TX DATA
	printf("Write to PCIE Slave\n");
	for( i=0;i<128;i=i+1) {
		axi_addr = (0x4000 + (0x8*i));                              //CPU -> Write to PCIe Slave -> PCIe Master Write to SRAM
		axi_wdata = ((0x120034005500aa00)+(0x2000000000000*i)+(0x100000000*i)+(0x20000*i)+(0x1*i));
		sw((0x40000000 + axi_addr),axi_wdata);
	}

	for( i=0;i<128;i=i+1) {
		axi_addr = (0x4000 + (0x8*i));                              //CPU -> Read from PCIe Slave -> PCIe Master Read from SRAM
		axi_wdata = ((0x120034005500aa00)+(0x2000000000000*i)+(0x100000000*i)+(0x20000*i)+(0x1*i));
		if(readl(0x40000000 + axi_addr) !=axi_wdata)
			printf("check read data fail\n");
	}


	for( i=0;i<128;i=i+1) {
		axi_addr = (0x4000 + (0x8*i));                             //CPU -> Read from SRAM
		axi_wdata = ((0x120034005500aa00)+(0x2000000000000*i)+(0x100000000*i)+(0x20000*i)+(0x1*i));
		if(readl(0x00200000 + axi_addr) !=axi_wdata)
			printf("check read data fail\n");
	}




	sw(0x22100000 + 0x0250, 0x00200000);  //set awmisc = 22'h200000 //DBI enable

	sw(0x40000000 + 0x0710, 0x10120); //Clear LOOPBACK Enable

	printf("Wait for LTSSM L0\n");

	rdata = 0x00000000;
	while (rdata  != 0x00000110) {
	rdata = readl(0x22100000+0x0120);  //wait for ltssm_state to 11 (L0)
	rdata = rdata & 0x000001f0;
	} 
	printf("LTSSM Go To L0\n");

}

static void pcie_phy_write(u16 val,u16 offset)
{
	printf("pcie_phy_writel val = %x , offset = %x\n",val,offset);
	
	writel(val<<16, 0x22100000);									//set[31:16] cr_wdata = 0x0402  (Set CM_TIME_OVRD_EN = 1, CM_TIME_OVRD = 1)	
	writel(offset<<16|0x2c04, 0x22100000);		//set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b00 (clr)
	writel(offset<<16|0x2e04, 0x22100000);		//set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b10 (wr)
}

static u16 pcie_phy_read(u16 offset)
{
	printf("pcie_phy_read offset = %x\n",offset);
	writel(offset<<16|0x2c04, 0x22100000);		//set[31:16] cr_addr = offse [9:8] cr_cmd = 2'b00 (clr)
	writel(offset<<16|0x2f04, 0x22100000);		//set[31:16] cr_addr = offse [9:8] cr_cmd = 2'b11 (rd)
	return readw(0x22100004);
}

void pcie_testlink()
{
	u32 val;

    writel( 0x0e000210 , FTSCU100_SCU_BASE  + 0x870);
    writel( 0x00000190 , FTSCU100_SCU_BASE  + 0x874);
    writel( 0x14443300 , FTSCU100_SCU_BASE  + 0x878);
    writel( readl(FTSCU100_SCU_BASE  + 0x874) | 0x00001000,FTSCU100_SCU_BASE  + 0x874);
    val = 0;
    while ( val != 0x200 ) { 
          val = readl (FTSCU100_SCU_BASE+0x800);
          val = val & 0x200;
        }
    writel( readl(FTSCU100_SCU_BASE  + 0x898)|0x20f , FTSCU100_SCU_BASE  + 0x898);
	printf("pcie lock\n");

#if 1
	    // CMLTX EN = 1
	    val = readl (FTSCU100_SCU_BASE+0x83c);
	    val = val | 0x3;
	    writel( val , FTSCU100_SCU_BASE  + 0x83c);
#endif

	val = readl(PCIE0_APB_REG_BASE) |PCIE_SLV_DBI_ENABLE;		
	writel(val, PCIE0_APB_REG_BASE + PCIE_SLV_AW_MISC);					//enable dbi
	writel(val, PCIE0_APB_REG_BASE + PCIE_SLV_AR_MISC);

	writel(readl(PCIE0_APB_REG_BASE + PCIE_SIDEBAND02)|0xc0000000, PCIE0_APB_REG_BASE + PCIE_SIDEBAND02);

	val = readl(PCIE0_APB_REG_BASE + PCIE_SIDEBAND93) & BIT31;

	printf("wait\n");
	while(val !=BIT31)
		val = readl(PCIE0_APB_REG_BASE + PCIE_SIDEBAND93) & BIT31;
	printf("wait done\n");

	writel(0x00000004, PCIE0_APB_REG_BASE + PCIE_SIDEBAND40);
	writel(0x00000044, PCIE0_APB_REG_BASE + PCIE_SIDEBAND59);
	writel(0x04020000, PCIE0_APB_REG_BASE + 0x44);
	writel(0x10112c04, PCIE0_APB_REG_BASE + PCIE_SIDEBAND01);
	writel(0x10112e04, PCIE0_APB_REG_BASE + PCIE_SIDEBAND00);
	writel(0x10112c04, PCIE0_APB_REG_BASE + PCIE_SIDEBAND00);
	writel(0x10112f04, PCIE0_APB_REG_BASE + PCIE_SIDEBAND00);

#if 0
	val = pcie_phy_read(0x1010);
	printf("before write 0x1010 val = %x\n",val);
	mdelay(1);
	pcie_phy_write(val |0x3 ,0x1010);
	mdelay(1);
	val = pcie_phy_read(0x1010);
	printf("after write 0x1010 val = %x\n",val);
#endif
	val = (readl(PCIE0_APB_REG_BASE + PCIE_SIDEBAND72) >> 4) & 0x3f;
	while(val!=0x11)
	{
		val = (readl(PCIE0_APB_REG_BASE + PCIE_SIDEBAND72) >> 4) & 0x3f;
		printf("ltssm = %x\n",val);
	}
	printf("Link to L0\n");
}

#if 0
pcie_phy_writel(0x00000103,	hw->ctl_base, 0x101b);

static void pcie_phy_writel(u16 val, u32 base,u16 offset)
{
	writel(val<<16, base + PCIE_PHY_DATA_OFFSET);									//set[31:16] cr_wdata = 0x0402  (Set CM_TIME_OVRD_EN = 1, CM_TIME_OVRD = 1)	
	writel(timing_offset<<16|0x2c04, base + PCIE_PHY_DATA_CONTROL_OFFSET);		//set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b00 (clr)
	writel(timing_offset<<16|0x2e04, base + PCIE_PHY_DATA_CONTROL_OFFSET);		//set[31:16] cr_addr = 0x1011 [9:8] cr_cmd = 2'b10 (wr)
}

static u16 pcie_phy_read(u16 val, u32 base,u16 offset)
{

	writel(offset<<16|0x2c04, base + PCIE_PHY_DATA_CONTROL_OFFSET);		//set[31:16] cr_addr = offse [9:8] cr_cmd = 2'b00 (clr)
	writel(offset<<16|0x2f04, base + PCIE_PHY_DATA_CONTROL_OFFSET);		//set[31:16] cr_addr = offse [9:8] cr_cmd = 2'b11 (rd)
	return readw(base + PCIE_PHY_DATA_CONTROL_OFFSET);
}
#endif


int board_init(void)
{
	unsigned int val;
	invalidate_icache_all();
	invalidate_dcache_all();
#if !defined(CONFIG_VDK_GC1610)
        writel(CPU_RELEASE_JUMPPC_VALUE, CPU_RELEASE_JUMPPC_ADDR);
        writel(CPU_RELEASE_MAGIC_VALUE, CPU_RELEASE_MAGIC_ADDR);
	__sev();	

//       mdelay(10);
//        writel(0, CPU_RELEASE_MAGIC_ADDR);
        writel(0, CPU_RELEASE_ADDR);
/*
	writel(readl(SCU_EXT_BASE+0x70)|0x403, SCU_EXT_BASE+0x70);
	writel(readl(SCU_EXT_BASE+0xa4)|0x38, SCU_EXT_BASE+0xa4);
*/
	val = readl (FTSCU100_SCU_BASE+0x840);
	writel (val | 0x22000000, FTSCU100_SCU_BASE+0x840);

	writel(readl(FTSCU100_SCU_BASE+0x60)|(BIT10|BIT11|BIT12|BIT13), FTSCU100_SCU_BASE+0x60);

  // PLLUM28HPCPFRAC PLL_USB init
  // [27:16]: FBDIV=0x54; [14:12]: POSTDIV2=0x5; [10:8]: POSTDIV1=0x7;
  WRITE((FTSCU100_SCU_BASE+0x858),(READ(FTSCU100_SCU_BASE+0x858))|0x00545700);
  // [29:24]: REFIV=0x1;
  WRITE((FTSCU100_SCU_BASE+0x85c),(READ(FTSCU100_SCU_BASE+0x85c))|0x01000000);
  // [6]: PD=0, [5]: FOUTVCOPD=1; [4]: FOUTPOSTDIVPD=0, [3]: FOUT4PHASEPD=0
  WRITE((FTSCU100_SCU_BASE+0x858),(READ(FTSCU100_SCU_BASE+0x858))&0xFFFFFFA7);
  // [6]: LOCK=1
  val = 0;
  while( val != 0x00000040 ){
        val = readl(FTSCU100_SCU_BASE+0x800);
        val = val & 0x00000040 ;
  }



    val = readl (FTSCU100_SCU_BASE + 0x860);
    val = val & 0xffffffb9;
    writel( val, FTSCU100_SCU_BASE  + 0x860);  // TDM PLL enable

   // Wait for PLL lock
    val = 0;
    while ( val != 0x80 ) { 
          val = readl(FTSCU100_SCU_BASE+0x800);
          val = val & 0x80;
        }

    val = readl(FTSCU100_SCU_BASE + 0x860);
    val = val & 0xffffffc7;
    writel( val, FTSCU100_SCU_BASE  + 0x860);  // clock output enable
    writel( readl(FTSCU100_SCU_BASE  + 0x854) |0x300 , FTSCU100_SCU_BASE  + 0x854);  // GCK enable
//    printf("PLL Lock!\n");
#if 	0		//for UART2 flow control and pinmux switch
	writel(0xc1,0x20900008);
	writel(0x3,0x2090000c);
	writel(0x3,0x20900010);
	writel(0x83,0x2090000c);
	writel(0x11,0x20900000);
	writel(0x3,0x2090000c);
	writel(0x155,0x20100844);	
#endif

#if 1 	//run linux have to enable		//PCIE function pre init about SCU

    writel( 0x0e000210 , FTSCU100_SCU_BASE  + 0x870);
    writel( 0x00000190 , FTSCU100_SCU_BASE  + 0x874);
    writel( 0x14443300 , FTSCU100_SCU_BASE  + 0x878);
    writel( readl(FTSCU100_SCU_BASE  + 0x874) | 0x00001000,FTSCU100_SCU_BASE  + 0x874);
    val = 0;
    while ( val != 0x200 ) { 
          val = readl (FTSCU100_SCU_BASE+0x800);
          val = val & 0x200;
        }
    writel( readl(FTSCU100_SCU_BASE  + 0x898)|0x20f , FTSCU100_SCU_BASE  + 0x898);
	printf("pcie lock\n");


	    // CMLTX EN = 1
	    val = readl (FTSCU100_SCU_BASE+0x83c);
	    val = val | 0x3;
	    writel( val , FTSCU100_SCU_BASE  + 0x83c);

#else	//baremetal pcietest

//	pcie_loopback();

//	pcie_testlink();
#endif
#endif
//	nand_test();

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
	return 0;
}

int board_mmc_init(struct bd_info *bis)
{
#ifdef CONFIG_FTSDC021
	return ftsdc021_sdhci_init(CONFIG_FTSDC021_BASE);
#endif
	return 0;
}
