/*
 * Faraday NAND Flash Controller
 *
 * (C) Copyright 2010 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#ifndef __FTNANDC021_H
#define __FTNANDC021_H

/* NANDC control registers */
struct ftnandc021_regs {
	/* 0x000 ~ 0x0fc */
	uint32_t ecc_pr[4];/* ECC Parity Register */
	uint32_t ecc_sr;   /* ECC Status Register */
	uint32_t rsvd0[59];

	/* 0x100 ~ 0x1fc */
	uint32_t sr;	 /* Status Register */
	uint32_t acr;	 /* Access Control Register */
	uint32_t fcr;	 /* Flow Control Register */
	uint32_t pir;	 /* Page Index Register */
	uint32_t mcr;	 /* Memory Configuration Register */
	uint32_t atr[2]; /* AC Timing Register */
	uint32_t rsvd1[1];
	uint32_t idr[2]; /* Device ID Register */
	uint32_t ier;	 /* Interrupt Enable Register */
	uint32_t iscr;	 /* Interrupt Status Clear Register */
	uint32_t rsvd2[4];
	uint32_t bbiwr;	 /* Bad Block Info Write */
	uint32_t lsn;	 /* LSN Initialize */
	uint32_t crcwr;	 /* LSN CRC Write */
	uint32_t lsnwr;	 /* LSN Write */
	uint32_t bbird;	 /* Bad Block Info Read */
	uint32_t lsnrd;	 /* LSN Read */
	uint32_t crcrd;	 /* CRC Read */
	uint32_t rsvd3[41];

	/* 0x200 ~ 0x2fc */
	uint32_t rsvd4[1];
	uint32_t icr;	 /* BMC Interrupt Control Register */
	uint32_t ior;	 /* BMC PIO Status Register */
	uint32_t bcr;	 /* BMC Burst Control Register */
	uint32_t rsvd5[60];

	/* 0x300 ~ 0x3fc */
	uint32_t dr;	 /* MLC Data Register */
	uint32_t isr;	 /* MLC Interrupt Status Register */
	uint32_t pcr;	 /* Page Count Register */
	uint32_t srr;	 /* MLC Software Reset Register */
	uint32_t rsvd7[58];
	uint32_t revr;	 /* Revision Register */
	uint32_t cfgr;	 /* Configuration Register */
};

/* ECC Status Register */
#define ECC_SR_CERR      (1 << 3)  /* correction error */
#define ECC_SR_ERR       (1 << 2)  /* ecc error */
#define ECC_SR_DEC       (1 << 1)  /* ecc decode finished */
#define ECC_SR_ENC       (1 << 0)  /* ecc encode finished */

/* Status Register */
#define SR_BLANK         (1 << 7)  /* blanking check failed */
#define SR_ECC           (1 << 6)  /* ecc timeout */
#define SR_STS           (1 << 4)  /* status error */
#define SR_CRC           (1 << 3)  /* crc error */
#define SR_CMD           (1 << 2)  /* command finished */
#define SR_READY         (1 << 1)  /* chip ready/busy */
#define SR_ENA           (1 << 0)  /* chip enabled */

/* Access Control Register */
#define ACR_CMD(x)       (((x) & 0x1f) << 8) /* command code */
#define ACR_START        (1 << 7)  /* command start */

/* Flow Control Register */
#define FCR_SWCRC        (1 << 8)  /* CRC controlled by Software */
#define FCR_IGNCRC       (1 << 7)  /* Bypass/Ignore CRC checking */
#define FCR_16BIT        (1 << 4)  /* 16 bit data bus */
#define FCR_WPROT        (1 << 3)  /* write protected */
#define FCR_NOSC         (1 << 2)  /* bypass status check error */
#define FCR_MICRON       (1 << 1)  /* Micron 2-plane command */
#define FCR_NOBC         (1 << 0)  /* skip blanking check error */

/* Interrupt Enable Register */
#define IER_ENA          (1 << 7)  /* interrupt enabled */
#define IER_ECC          (1 << 3)  /* ecc error timeout */
#define IER_STS          (1 << 2)  /* status error */
#define IER_CRC          (1 << 1)  /* crc error */
#define IER_CMD          (1 << 0)  /* command finished */

/* BMC PIO Status Register */
#define IOR_READY        (1 << 0)  /* PIO ready */

/* MLC Software Reset Register */
#define SRR_ECC_EN       (1 << 8)  /* ECC enabled */
#define SRR_NANDC_RESET  (1 << 2)  /* NANDC reset */
#define SRR_BMC_RESET    (1 << 1)  /* BMC reset */
#define SRR_ECC_RESET    (1 << 0)  /* ECC reset */
#define SRR_CHIP_RESET   (SRR_NANDC_RESET | SRR_BMC_RESET | SRR_ECC_RESET)

/* Memory Configuration Register */
#define MCR_BS16P        (0 << 16) /* page count per block */
#define MCR_BS32P        (1 << 16)
#define MCR_BS64P        (2 << 16)
#define MCR_BS128P       (3 << 16)
#define MCR_1PLANE       (0 << 14) /* memory architecture */
#define MCR_2PLANE       (1 << 14)
#define MCR_SERIAL       (0 << 12) /* interleaving: off, 2 flash, 4 flash */
#define MCR_IL2          (1 << 12)
#define MCR_IL4          (2 << 12)
#define MCR_ALEN3        (0 << 10) /* address length */
#define MCR_ALEN4        (1 << 10)
#define MCR_ALEN5        (2 << 10)
#define MCR_PS512        (0 << 8)  /* size per page (bytes) */
#define MCR_PS2048       (1 << 8)
#define MCR_PS4096       (2 << 8)
#define MCR_16MB         (0 << 4)  /* flash size */
#define MCR_32MB         (1 << 4)
#define MCR_64MB         (2 << 4)
#define MCR_128MB        (3 << 4)
#define MCR_256MB        (4 << 4)
#define MCR_512MB        (5 << 4)
#define MCR_1GB          (6 << 4)
#define MCR_2GB          (7 << 4)
#define MCR_4GB          (8 << 4)
#define MCR_8GB          (9 << 4)
#define MCR_16GB         (10 << 4)
#define MCR_32GB         (11 << 4)
#define MCR_ME(n)        (1 << (n)) /* module enable, 0 <= n <= 3 */

/* FTNANDC021 built-in command set */
#define FTNANDC021_CMD_RDID  0x01   /* read id */
#define FTNANDC021_CMD_RESET 0x02   /* reset flash */
#define FTNANDC021_CMD_RDST  0x04   /* read status */
#define FTNANDC021_CMD_RDPG  0x05   /* read page (data + oob) */
#define FTNANDC021_CMD_RDOOB 0x06   /* read oob */
#define FTNANDC021_CMD_WRPG  0x10   /* write page (data + oob) */
#define FTNANDC021_CMD_ERBLK 0x11   /* erase block */
#define FTNANDC021_CMD_WROOB 0x13   /* write oob */

int ftnandc021_init(struct nand_chip *chip, uint32_t iobase, int alen);

#endif /* EOF */
