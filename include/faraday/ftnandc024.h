/*
 * Faraday NAND Flash Controller
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * SPDX-License-Identifier:    GPL-2.0+
 */

#ifndef __FTNANDC024_H
#define __FTNANDC024_H

#include <linux/bitops.h>
#include <faraday/nand.h>

/*
 * ECC
 */
struct ftnandc024_ecc {
	uint32_t sr[2];  /* status register */
	uint32_t cr;     /* control register */
	uint32_t rvsd0;
	uint32_t thr[2]; /* threshold register */
	uint32_t cap[2]; /* capability register */
	uint32_t ier;    /* interrupt enable register */
	uint32_t isr;    /* interrupt status register */
	uint32_t scr;    /* status clear register */
	uint32_t srsr[2];/* spare region status register */
	uint32_t srcr[4];/* spare region control register */
	uint32_t rsvd1[47];
};

/*
 * NANDC
 */
struct ftnandc024_nandc {
	uint32_t sr;      /* status register */
	uint32_t gsr;     /* general setting register */
	uint32_t msr[2];  /* memory setting register */
	uint32_t atrl[16];/* AC timing register LOW (0,1) */
	uint32_t ier;     /* interrupt enable register */
	uint32_t isr;     /* interrupt status register */
	uint32_t crar[8]; /* current row address register */
	uint32_t rdsr[2]; /* read status register */
	uint32_t atbr;    /* address toggle bit register */
	uint32_t srr;     /* software reset register */
	uint32_t actr;    /* auto compare threshold register */
	uint32_t acr;     /* auto compare register */
	uint32_t atrh[16];/* AC timing register HIGH (2,3) */
	uint32_t rsvd[12];
};

/* NANDC General Setting Register (0x104) */
#define FTNANDC024_NANDC_GSR_NRCE(n)	(((n) <= 0) ? 0 : (ffs(n) - 1))
#define FTNANDC024_NANDC_GSR_RDYBIT(n)	(((n) & 7) << 12)	/* ready bit */
#define FTNANDC024_NANDC_GSR_ERRBIT(n)	(((n) & 7) << 8)	/* error bit */
#define FTNANDC024_NANDC_GSR_WPROT		BIT_MASK(2)	/* write protect */
#define FTNANDC024_NANDC_GSR_DATI		BIT_MASK(1)	/* data inverse */
#define FTNANDC024_NANDC_GSR_DATS		BIT_MASK(0)	/* data scrambler */

/* NANDC Memory Setting Register 0 (0x108) */
#define FTNANDC024_NANDC_MSR0_BIPP(n)	\
	(MIN(6, (n) < 0 ? 0 : (n)) << 19)				/* bi per page */
#define FTNANDC024_NANDC_MSR0_BPP(n)	\
	(MIN(4, ((n) < 2048 ? 0 : ffs(n) - 11)) << 16) /* bytes per page */
#define FTNANDC024_NANDC_MSR0_RAC(n)	\
	(MIN(3, ((n) ? ((n) - 1) : (n))) << 13)        /* row address cycles */
#define FTNANDC024_NANDC_MSR0_CAC(n)	\
	(MIN(1, ((n) ? ((n) - 1) : (n))) << 12)        /* col address cycles */
#define FTNANDC024_NANDC_MSR0_PPB(n)	\
	(MIN(1023, ((n) ? ((n) - 1) : (n))) << 2)      /* pages per block */

/* NANDC Memory Setting Register 1 (0x10C) */
#define FTNANDC024_NANDC_MSR1_VPPB(n)	\
	(MIN(1023, ((n) ? ((n) - 1) : (n))) << 16)     /* valid pages per block */

/* NANDC Interrupt Enable Register (0x150) */
#define FTNANDC024_NANDC_IER			BIT_MASK(0)     /* irq enable */

/* NANDC Interrupt Status Register (0x154) */
#define FTNANDC024_NANDC_ISR_ERR		BIT_MASK(0)     /* flash status error */
#define FTNANDC024_NANDC_ISR_CMD		BIT_MASK(16)    /* flash command finish */
#define FTNANDC024_NANDC_ISR_CMP		BIT_MASK(24)    /* pattern compare failed */

/*
 * Command Queue Control
 */
struct ftnandc024_cmdqc {
	uint32_t sr;      /* status register */
	uint32_t fr;      /* flush register */
	uint32_t ccr;     /* complete counter register */
	uint32_t ccrr;    /* complete counter reset register */
	uint32_t rsvd[60];
};

/* Command Queue Status Register (0x200) */
#define FTNANDC024_CMDQC_SR_FULL			BIT_MASK(8)
#define FTNANDC024_CMDQC_SR_EMPTY			BIT_MASK(0)

/* Command Queue Flush Register (0x204) */
#define FTNANDC024_CMDQC_FR_FLUSH			BIT_MASK(0)

/* Command Queue Complete Counter Register (0x208) */
#define FTNANDC024_CMDQC_CCR_VAL(x)		((x) & 7)

/* Command Queue Complete Counter Reset Register (0x20C) */
#define FTNANDC024_CMDQC_CCR_RST			BIT_MASK(0)

/*
 * Command Queue
 */
struct ftnandc024_cmdq {
	uint32_t word[6];
	uint32_t rsvd[2];
};

/*
 * BMC
 */
struct ftnandc024_bmc {
	uint32_t sr;       /* region status register */
	uint32_t usr_pr[8];/* user mode pointer register */
	uint32_t dma_dr;   /* dma mode data register */
	uint32_t srr;      /* software reset register */
	uint32_t frfr;     /* force region fill data read register */
	uint32_t rrscr[8]; /* region remaining sector count register */
	uint32_t rsvd[44];
};

/*
 * MISC
 */
struct ftnandc024_misc {
	uint32_t revr;     /* revision register */
	uint32_t fear;     /* feature register */
	uint32_t smsr;     /* AHB slave memory space register */
	uint32_t gsrr;     /* global software reset register */
	uint32_t sdrr;     /* AHB slave data reset register */
	uint32_t ecc_capr[3];/* ecc capability register */
	uint32_t rsvd[56];
};

#define FTNANDC024_MISC_SMSR_PREFETCH_SIZE(n)	(((n) ? ((n) & 0x1ff) : 1) << 16)
#define FTNANDC024_MISC_SMSR_PREFETCH_FORCED	BIT_MASK(12)
#define FTNANDC024_MISC_SMSR_RETRY				BIT_MASK(8)
#define FTNANDC024_MISC_SMSR_64K				BIT_MASK(7)
#define FTNANDC024_MISC_SMSR_32K				BIT_MASK(6)
#define FTNANDC024_MISC_SMSR_16K				BIT_MASK(5)
#define FTNANDC024_MISC_SMSR_8K					BIT_MASK(4)
#define FTNANDC024_MISC_SMSR_4K					BIT_MASK(3)
#define FTNANDC024_MISC_SMSR_2K					BIT_MASK(2)
#define FTNANDC024_MISC_SMSR_1K					BIT_MASK(1)
#define FTNANDC024_MISC_SMSR_512				BIT_MASK(0)

/* NANDC control registers */
struct ftnandc024_regs {
	/* 0x000 ~ 0x0ff: ECC */
	struct ftnandc024_ecc ecc;

	/* 0x100 ~ 0x1ff: NANDC */
	struct ftnandc024_nandc nandc;

	/* 0x200 ~ 0x2ff: Command Queue Control */
	struct ftnandc024_cmdqc cmdqc;

	/* 0x300 ~ 0x3ff: Command Queue */
	struct ftnandc024_cmdq cmdq[8];

	/* 0x400 ~ 0x4ff: BMC */
	struct ftnandc024_bmc bmc;

	/* 0x500 ~ 0x5ff: MISC */
	struct ftnandc024_misc misc;

	/* 0x700 ~ 0x7ff: Programmable OPCODE */

	/* 0x1000 ~ 0x10ff: Spare SRAM */

	/* 0x2000 ~ 0x20ff: Programmable flow control */

	/* 0x20000 ~ 0x3ffff: Data SRAM */

	/* 0x100000 ~ 0x1fffff: Data Region/PIO Access Port */
};

/* FTNANDC024 built-in command set */
#define FTNANDC024_CMD_RDID     0x5f   /* read id */
#define FTNANDC024_CMD_RESET    0x65   /* reset flash */
#define FTNANDC024_CMD_RDST     0x96   /* read status */
#define FTNANDC024_CMD_RDPG     0x48   /* read page (data + oob) */
#define FTNANDC024_CMD_RDOOB    0x3e   /* read oob */
#define FTNANDC024_CMD_WRPG     0x26   /* write page (data + oob) */
#define FTNANDC024_CMD_WRPG_NOOB     0x54   /* write page (no oob)*/
#define FTNANDC024_CMD_WROOB    0x33   /* write oob */
#define FTNANDC024_CMD_ERBK     0x68   /* erase block */

#define FTNANDC024_CMD_RDPG_SP  0x23e  /* read page (data + oob) */
#define FTNANDC024_CMD_RDOOB_SP 0x249  /* read oob */
#define FTNANDC024_CMD_WRPG_SP  0x26c  /* write page (data + oob) */
#define FTNANDC024_CMD_WROOB_SP 0x278  /* write oob */
#define FTNANDC024_CMD_ERBK_SP  0x2c1  /* erase block */

#define FTNANDC024_CMD_RAWRD    0x8A	/* byte read*/
#define FTNANDC024_CMD_RAWWR    0x80	/* byte write */

#define FTNANDC024_MAX_SPARE_DATA_BYTE_SIZE		32

/* For recording the command execution status*/
#define CMD_SUCCESS				0					
#define CMD_CRC_FAIL				(1 << 1)
#define CMD_STATUS_FAIL				(1 << 2)
#define CMD_ECC_FAIL_ON_DATA			(1 << 3)
#define CMD_ECC_FAIL_ON_SPARE			(1 << 4)


#define FREQ_SETTING            CONFIG_NAND_FTNANDC024_HCLK


#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define max_2(a,b)              max(a,b)
#define min_2(a,b)              min(a,b)
#define max_3(a,b,c)            (max_2(a,b) > c ? max_2(a,b): c)
#define min_3(a,b,c)            (min_2(a,b) < c ? min_2(a,b): c)
#define max_4(a,b,c,d)          (max_3(a,b,c) > d ? max_3(a,b,c): d)
#define min_4(a,b,c,d)          (min_3(a,b,c) < d ? min_3(a,b,c): d)


struct ftnandc024_chip_timing {
	uint8_t tWH;
	uint8_t tCH;
	uint8_t tCLH;
	uint8_t tALH;
	uint8_t tCALH;
	uint8_t tWP;
	uint8_t tREH;
	uint8_t tCR;
	uint8_t tRSTO;
	uint8_t tREAID;
	uint8_t tREA;
	uint8_t tRP;
	uint8_t tWB;
	uint8_t tRB;
	uint8_t tWHR;
	int     tWHR2;
	uint8_t tRHW;
	uint8_t tRR;
	uint8_t tAR;
	uint8_t tRC;
	int     tADL;
	uint8_t tRHZ;
	uint8_t tCCS;
	uint8_t tCS;
	uint8_t tCS2;
	uint8_t tCLS;
	uint8_t tCLR;
	uint8_t tALS;
	uint8_t tCALS;
	uint8_t tCAL2;
	uint8_t tCRES;
	uint8_t tCDQSS;
	uint8_t tDBS;
	int     tCWAW;
	uint8_t tWPRE;
	uint8_t tRPRE;
	uint8_t tWPST;
	uint8_t tRPST;
	uint8_t tWPSTH;
	uint8_t tRPSTH;
	uint8_t tDQSHZ;
	uint8_t tDQSCK;
	uint8_t tCAD;
	uint8_t tDSL;
	uint8_t tDSH;
	uint8_t tDQSL;
	uint8_t tDQSH;
	uint8_t tDQSD;
	uint8_t tCKWR;
	uint8_t tWRCK;
	uint8_t tCK;
	uint8_t tCALS2;
	uint8_t tDQSRE;
	uint8_t tWPRE2;
	uint8_t tRPRE2;
	uint8_t tCEH;
};

/* For NANDC024 bad block mark issue */

/* config base, may move to hgu.h */
#define FTNANDC024_CONFIG_BASE      0x81000000 //0x41000000
/***** Content *****/
#define FTNANDC024_SPL_NAND_BLOCK_OFFSET		0x0
#define FTNANDC024_LOOKUP_TB_NAND_BLOCK_OFFSET	NAND_BSP_LOOKUP_TABLE_BLOCK
#define FTNANDC024_DATAIMG_NAND_BLOCK_OFFSET	NAND_BSP_BLOCK
#define FTNANDC024_LOOKUP_TB_BLOCK_SCAN_NUM        NAND_BSP_LOOKUP_TABLE_SCAN_NUM

#define FTNANDC024_SPL_NAND_HARDCOPY_NUM		(FTNANDC024_LOOKUP_TB_NAND_BLOCK_OFFSET - FTNANDC024_SPL_NAND_BLOCK_OFFSET)
#define FTNANDC024_LOOKUP_TB_NAND_HARDCOPY_NUM	(FTNANDC024_DATAIMG_NAND_BLOCK_OFFSET - FTNANDC024_LOOKUP_TB_NAND_BLOCK_OFFSET)

#define SPL_BOOTIMG_MAGIC_NUMBER	SPL_BOOTIMG_MAGIC_NUMBER
#define BSP_BOOTIMG_MAGIC_NUMBER	BSP_BOOTIMG_MAGIC_NUMBER
#define LOOKUP_TABLE_MAGIC_NUMBER	LOOKUP_TABLE_MAGIC_NUMBER
#define DATAIMG_MAGIC_NUMBER		DATAIMG_MAGIC_NUMBER

#define BSP_PAYLOAD_CHK1			0x46415241		/* FARA */
#define BSP_PAYLOAD_CHK2			0x44415948		/* DAYH */
#define BSP_PAYLOAD_CHK3			0x51504253		/* QPBS */

enum BSP_CONTENT_TYPE{
	BSP_TYPE_SPL = FTNANDC024_SPL_NAND_BLOCK_OFFSET,
	FTNANDC024_TYPE_LOOKUP_TABLE = FTNANDC024_LOOKUP_TB_NAND_BLOCK_OFFSET,
	BSP_TYPE_DATAIMG = FTNANDC024_DATAIMG_NAND_BLOCK_OFFSET,
};

/***** Struct Information *****/
/*** SPL ***/
#define SPL_BOOTIMG_CHECK_BYTE			15
#define SPL_BOOTIMG_RESERVED_BIT		3

/*** Lookup Table ***/
#define LOOKUP_CHECK_BYTE				30
#define LOOKUP_HDR_RESERVED_BIT			27	/* I wanna the header size is 16 times */
#define LOOKUP_IMG_CHECK_BYTE			84
#define LOOKUP_IMG_RESERVED_BIT			11

/*** Data Image ***/
#define DATAIMG_MAX_NAME_LEN			32	/* max length of file name */
#define DATAIMG_RESERVED_FREE_BLOCK_NUM	3
#define DATAIMG_CHECK_BYTE				60
#define DATAIMG_HDR_RESERVED_BIT		7	/* I wanna the header size is 16 times */

/***** Struct Information *****/
/***********************************************/
#define BSPSZ_VERSION_INFO_T			8
typedef struct {
	unsigned int	version_date;	/* 0xYYYYMMDD */
	unsigned int	version_time;	/* 0x00HHMMSS */
} __attribute__((__packed__)) version_info_t;

/***********************************************/
#define BSPSZ_DATAIMG_INFO_T			76
typedef struct {
	unsigned int    img_size;
	unsigned short	num_block;	/* how many block are occupied by this image? */
	unsigned char	img_name[DATAIMG_MAX_NAME_LEN];
	unsigned char	alias_name[DATAIMG_MAX_NAME_LEN];
	unsigned char	is_bootable;	/* (is_bootable)? disaplay name on menu: won't display name in menu mode */
	unsigned char	is_show_name;	/* (is_show_name)? show "img_name": show "alias_name" */
	unsigned int	load_addr;
} __attribute__((__packed__)) dataimg_info_t;

/***********************************************/
#define BSPSZ_NAND_FLASH_INFO_T			5
typedef struct {
	unsigned char   block_num;		/* #pages in one block*/
    unsigned char   page_size;
	unsigned char	sector_size;	/* sector size: 512B, 1025 = 0, 1*/
    unsigned char   ecc_bit;
	unsigned char	addr_cycle;	/* address cycle (row, column): (3, 2), (3, 1), (2, 2), (2, 1) = 0,  1,  2, 3 */
} __attribute__((__packed__)) nand_flash_info_t;

/***********************************************/
#define BSPSZ_SPL_BOOTIMG_HEADER_T		16
typedef struct	{
		unsigned int	magic_num;
		unsigned int	file_size;
		unsigned char	block_num;	/* #pages in one block*/
		unsigned char	page_size;
		unsigned char	sector_size;
		unsigned char	ecc_bit;
		unsigned char	reserved[SPL_BOOTIMG_RESERVED_BIT];
		unsigned char	chk_sum;
} __attribute__((__packed__)) spl_bootimg_header_t;

#define BSPSZ_LOOKUP_HEADER_T			64
typedef struct {
	unsigned int	magic_num;
	unsigned int	payload1;
	nand_flash_info_t	flash_info;
	unsigned int	payload2;
	unsigned char	is_initial;
	version_info_t	version;
	unsigned short	num_addrdata;
	unsigned short	num_dataimg;
	unsigned short	autoboot_mode;	/* 0: menu mode, others: boot from #dataimg */
	unsigned char   chk_sum;
	unsigned int	payload3;
	unsigned char	reserved[LOOKUP_HDR_RESERVED_BIT];
} __attribute__((__packed__)) lookup_header_t;

#define BSPSZ_LOOKUP_IMG_T				96
typedef struct {
	dataimg_info_t	img_info;	/* 1:1 mapping to dataimg_header_t.img_info */
	unsigned int	start_block;
	unsigned int	end_block;
	unsigned char   chk_sum;
	unsigned char	reserved[LOOKUP_IMG_RESERVED_BIT];
} __attribute__((__packed__)) lookup_img_t;

#define BSPSZ_LOOKUP_ADDRDATA_T			8
typedef struct {
    unsigned int    addr;
	unsigned int	data;
} __attribute__((__packed__)) lookup_addrdata_t;

#define BSPSZ_DATAIMG_HEADER_T			112
typedef struct {
    unsigned int    magic_num;
	unsigned int	payload1;
	dataimg_info_t	img_info;
	unsigned int	payload2;
	version_info_t	version;
	unsigned char   chk_sum;
	unsigned int	payload3;
	unsigned char	reserved[DATAIMG_HDR_RESERVED_BIT];
	unsigned int    magic_num_end;
} __attribute__((__packed__)) dataimg_header_t;

/* NANDC024 bad block mark issue end */

int ftnandc024_init(struct nand_chip *chip, uint32_t iobase, uint32_t database, int sel_ce, struct platform_nand_flash *plat_nand, u8 bi_reserve);

#endif /* EOF */
