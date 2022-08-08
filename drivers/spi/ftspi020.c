/*
 * Faraday FTSPI020 SPI driver
 *
 * based on bfin_spi.c
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <malloc.h>
#include <fdtdec.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
//#include "../mtd/spi/sf_internal.h"
#include "ftspi020.h"

DECLARE_GLOBAL_DATA_PTR;

#ifndef CONFIG_FTSPI20_FREQ
#define CONFIG_FTSPI20_FREQ     clk_get_rate("SPI")
#endif

#define CFG_SUPP_FASTRD         1
//#define CFG_SUPP_FASTRD_DUAL    1 /* Support fast read dual(2x) */
#define CFG_SUPP_FASTRD_QUAD    1 /* Support fast read quad(4x) */
#define CFG_SHOW_PROGRESS       1 /* print a '.' at the end of each action */

/* Flash opcodes. */
#define OPCODE_WREN             0x06 /* Write enable */
#define OPCODE_WRDI             0x04 /* Write disable */
#define OPCODE_RDSR             0x05 /* Read status register */
#define OPCODE_WRSR             0x01 /* Write status register 1 byte */
#define OPCODE_NORM_READ        0x03 /* Read (low freq.) */
#define OPCODE_NORM_READ4       0x13 /* Read (low freq., 4 bytes addr) */
#define OPCODE_FAST_READ        0x0b /* Read (high freq.) */
#define OPCODE_FAST_READ4       0x0c /* Read (high freq., 4 bytes addr) */
#define OPCODE_FAST_READ_DUAL   0x3b /* Read (high freq.) */
#define OPCODE_FAST_READ4_DUAL  0x3c /* Read (high freq., 4 bytes addr) */
#define OPCODE_FAST_READ_QUAD   0x6b /* Read (high freq.) */
#define OPCODE_FAST_READ4_QUAD  0x6c /* Read (high freq. 4 bytes addr) */
#define OPCODE_PP               0x02 /* Page program */
#define OPCODE_PP4              0x12 /* Page program (4 bytes addr) */
#define OPCODE_QPP              0x32 /* QUAD Page program */
#define OPCODE_BE_4K            0x20 /* Erase 4KiB block */
#define OPCODE_BE_4K4           0x21 /* Erase 4KiB block (4 bytes addr) */
#define OPCODE_BE_32K           0x52 /* Erase 32KiB block */
#define OPCODE_CHIP_ERASE       0xc7 /* Erase whole flash chip */
#define OPCODE_SE               0xd8 /* Sector erase */
#define OPCODE_SE4              0xdc /* Sector erase (4 bytes addr) */
#define OPCODE_RDID             0x9f /* Read JEDEC ID */

#define OPCODE_CMD_BANKADDR_BRRD  0x16 /* Bank register read*/
#define OPCODE_CMD_BANKADDR_BRWR  0x17 /* Bank register write*/

#define OPCODE_CMD_EN4B    0xB7
#define OPCODE_CMD_EX4B    0xE9

#define CMD_READ_CONFIG 0x35

/* NAND Flash opcodes */
#define CMD_RESET       0xFF
#define CMD_GET_FEATURE     0xF
#define CMD_SET_FEATURE     0x1F
#define CMD_PAGE_READ_TO_CACHE      0x13
#define CMD_PROGRAM_EXECUTE 0x10


/* Status Register bits. */
#define SR_WIP                  BIT_MASK(0) /* Write in progress */
#define SR_WEL                  BIT_MASK(1) /* Write enable latch */

struct ftspi020_spi_priv {
#ifndef CONFIG_DM_SPI_FLASH
	struct spi_slave slave;
#endif
	struct ftspi020_regs *regs;
	u8 num_cs;      /* total no. of CS available */
	u8 cs;
	u32 mode;
	u32 max_hz;
	u32 fourbyte_en;
	u32 tx_transfer_size;
	u32 rx_transfer_size;
};

static void ftspi020_rdid(struct ftspi020_spi_priv *priv)
{
	//this is not including data phase, just write offset, command 1
	struct ftspi020_regs *regs = priv->regs;
	
	/* clear isr */
	writel(ISR_CMD, &regs->isr);
	
	/* issue command */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);
}

static u32 ftspi020_rdsr_val(struct ftspi020_spi_priv *priv)
{
	//this is not including data phase, just write offset , command 1
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;	
	u32 st;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);
	writel(1, &regs->cmd[2]);
	writel(CMD3_OPC(OPCODE_RDSR) | CMD3_CS(cs)
		| CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until rx ready */
	while (!(readl(&regs->sr) & SR_RFR))
		;
	st = readl(&regs->dr);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return st & 0xff;	
}

static void ftspi020_rdsr(struct ftspi020_spi_priv *priv)
{
	//this is not including data phase, just write offset , command 1
	struct ftspi020_regs *regs = priv->regs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);
}

static int ftspi020_read(struct ftspi020_spi_priv *priv, u8 cmd, u32 off)
{
	//this is not including data phase, just write offset , command 1
	/* issue command (Rd) */
	struct ftspi020_regs *regs = priv->regs;
	
	writel(ISR_CMD, &regs->isr);
	writel(off, &regs->cmd[0]);

	if (off < 0x1000000 && !(priv->fourbyte_en)) {
		if(cmd != OPCODE_NORM_READ){
#ifndef CONFIG_MTD_SPI_NAND        
			writel(CMD1_ILEN(1) | CMD1_DCYC(8) | CMD1_ALEN(3),
			       &regs->cmd[1]);
#else
                    // for spi nand
			writel(CMD1_ILEN(1) | CMD1_DCYC(8) | CMD1_ALEN(2),
			       &regs->cmd[1]);
#endif
		}else{
			writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);
		}

	} else {
		if(cmd != OPCODE_NORM_READ){
			writel(CMD1_ILEN(1) | CMD1_DCYC(8) | CMD1_ALEN(4),
			       &regs->cmd[1]);
		}else{
			writel(CMD1_ILEN(1) | CMD1_ALEN(4), &regs->cmd[1]);
		}
	}
	return 0;
}

static int ftspi020_nand_program_execute(struct ftspi020_spi_priv *priv,u32 off)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;
	
	writel(ISR_CMD, &regs->isr);
	writel(off, &regs->cmd[0]);
	writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(CMD_PROGRAM_EXECUTE) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_nand_read_to_cache(struct ftspi020_spi_priv *priv,u32 off)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;
	
	writel(ISR_CMD, &regs->isr);
	writel(off, &regs->cmd[0]);
	writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(CMD_PAGE_READ_TO_CACHE) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_nand_set_feature(struct ftspi020_spi_priv *priv,u32 off)
{
	struct ftspi020_regs *regs = priv->regs;
	
	writel(ISR_CMD, &regs->isr);
	writel(off, &regs->cmd[0]);

	writel(CMD1_ILEN(1) | CMD1_ALEN(1),
	       &regs->cmd[1]);
	return 0;
}

static int ftspi020_nand_get_feature(struct ftspi020_spi_priv *priv,u32 off)
{
	struct ftspi020_regs *regs = priv->regs;
	
	writel(ISR_CMD, &regs->isr);
	writel(off, &regs->cmd[0]);

	writel(CMD1_ILEN(1) | CMD1_ALEN(1),
	       &regs->cmd[1]);
	return 0;
}

static int ftspi020_nand_reset(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(CMD_RESET) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_wren(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command (WE) */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(OPCODE_WREN) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_wrdi(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command (WD) */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(OPCODE_WRDI) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_en4b(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command (EN4B) */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(OPCODE_CMD_EN4B) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_ex4b(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command (EX4B) */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);

	writel(0, &regs->cmd[2]);
	writel(CMD3_OPC(OPCODE_CMD_EX4B) | CMD3_CS(cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	return 0;
}

static int ftspi020_wrsr(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command (WE) */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);
	return 0;
	writel(2, &regs->cmd[2]);
	writel(CMD3_OPC(OPCODE_WRSR) | CMD3_CS(priv->cs)
		| CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until tx ready */
	while (!(readl(&regs->sr) & SR_TFR))
		;
	writel(0x0200, &regs->dr);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;

	/* wait until device ready */
	while (ftspi020_rdsr_val(priv) & SR_WEL)
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);
	return 0;
}


static int ftspi020_write(struct ftspi020_spi_priv *priv, u32 off)
{
	//this is not including data phase, just write offset
	struct ftspi020_regs *regs = priv->regs;

	/* issue command (WE) */
	writel(off, &regs->cmd[0]);

	return 0;
}

static int ftspi020_erase(struct ftspi020_spi_priv *priv, u8 cmd, u32 offset)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;
	
	/* issue command (SE) */
	writel(offset, &regs->cmd[0]);
	writel(0x00, &regs->cmd[2]);
#ifndef CONFIG_MTD_SPI_NAND
	if (offset < 0x1000000 && !(priv->fourbyte_en)) {
		writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);
		if(cmd == OPCODE_BE_4K)
			writel(CMD3_OPC(OPCODE_BE_4K) | CMD3_CS(cs)
			       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
		else
			writel(CMD3_OPC(OPCODE_SE) | CMD3_CS(cs)
			       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
	} else {
		writel(CMD1_ILEN(1) | CMD1_ALEN(4), &regs->cmd[1]);
		if(cmd == OPCODE_BE_4K)
			writel(CMD3_OPC(OPCODE_BE_4K) | CMD3_CS(cs)
			       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
		else
			writel(CMD3_OPC(OPCODE_SE) | CMD3_CS(cs)
			       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
	}
#else
	writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);
	writel(CMD3_OPC(OPCODE_SE) | CMD3_CS(cs)
		       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
#endif
	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);
	return 0;
}
#ifndef CONFIG_MTD_SPI_NAND
static u32 get_spi_flash_addr(u8 *cmd, u32 fourbyte_en)
{
	if(fourbyte_en)
		return (u32)((cmd[1]<<24)|(cmd[2]<<16)|(cmd[3]<<8)|(cmd[4]));
	else
	return (u32)((cmd[1]<<16)|(cmd[2]<<8)|(cmd[3]));
}
#else
static u32 get_spi_flash_addr(u8 *cmd, u32 bytenum)
{
        if(bytenum == 4)
		return (u32)((cmd[1]<<24)|(cmd[2]<<16)|(cmd[3]<<8)|(cmd[4]));
        else if (bytenum == 3)
		return (u32)((cmd[1]<<16)|(cmd[2]<<8)|(cmd[3]));
        else
		return (u32)((cmd[1]<<8)|(cmd[2]));
}
#endif
static int ftspi020_rdcfg(struct ftspi020_spi_priv *priv)
{
	struct ftspi020_regs *regs = priv->regs;
	int cs = priv->cs;
	int st;

	/* clear isr */
	writel(ISR_CMD, &regs->isr);

	/* issue command */
	writel(0, &regs->cmd[0]);
	writel(CMD1_ILEN(1), &regs->cmd[1]);
	writel(1, &regs->cmd[2]);
	writel(CMD3_OPC(CMD_READ_CONFIG) | CMD3_CS(cs)
		| CMD3_CMDIRQ, &regs->cmd[3]);

	/* wait until rx ready */
	while (!(readl(&regs->sr) & SR_RFR))
		;
	st = readl(&regs->dr);

	/* wait until command finish */
	while (!(readl(&regs->isr) & ISR_CMD))
		;
	/* clear isr */
	writel(ISR_CMD, &regs->isr);
	return st & 0xff;
}

#ifdef CONFIG_DM_SPI_FLASH

static int ftspi020_spi_xfer(struct udevice *dev, unsigned int bitlen,
			    const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct ftspi020_spi_priv *priv = dev_get_priv(bus);
	struct ftspi020_regs *regs = priv->regs;
	u32 len, access_byte;
	static u8 cmd;
	static u32 offset;
	
	if(bitlen == 0) {
		printf("Error: Invalid bit length\n");
		return -EINVAL;
	}
	
	if(dout && din) {
		printf("Error: Cannot have data in and data out set\n");
		return -EINVAL;
	}
	
	len = bitlen / 8;
	
	if(dout) {
		if(flags & SPI_XFER_BEGIN) {
			//this is command section;
			cmd = *(u8 *)dout;
#ifndef CONFIG_MTD_SPI_NAND
			offset = get_spi_flash_addr((u8 *)dout, priv->fourbyte_en);
#else
			offset = get_spi_flash_addr((u8 *)dout, 2);
#endif
			debug("ftspi020: cmd=0x%x offset=0x%x", cmd, offset);
			switch(cmd)
			{
				case OPCODE_RDID:
					ftspi020_rdid(priv);
					break;
				case CMD_READ_CONFIG:
					ftspi020_rdcfg(priv);
					break;
				case OPCODE_RDSR:
					ftspi020_rdsr(priv);
					break;
				case OPCODE_NORM_READ:
				case OPCODE_CMD_BANKADDR_BRRD:
				case OPCODE_FAST_READ:
				case OPCODE_FAST_READ_DUAL:
				case OPCODE_FAST_READ_QUAD:
					ftspi020_read(priv, cmd, offset);
					break;
				case OPCODE_WREN:
					ftspi020_wren(priv);
					break;
				case OPCODE_WRDI:
					ftspi020_wrdi(priv);
					break;
				case OPCODE_WRSR:
					ftspi020_wrsr(priv);
					break;
				case OPCODE_QPP:
				case OPCODE_PP:
				case OPCODE_CMD_BANKADDR_BRWR:
					ftspi020_write(priv, offset);
					break;
				case OPCODE_CMD_EN4B:
					priv->fourbyte_en = 1;
					ftspi020_en4b(priv);
					break;
				case OPCODE_CMD_EX4B:
					priv->fourbyte_en = 0;
					ftspi020_ex4b(priv);
					break;
				case OPCODE_BE_4K:
				case OPCODE_SE:
#ifdef CONFIG_MTD_SPI_NAND
					offset = get_spi_flash_addr((u8 *)dout, 3);
#endif
					ftspi020_erase(priv, cmd, offset);
					break;
				case CMD_RESET:
				        ftspi020_nand_reset(priv);
					break;
				case CMD_GET_FEATURE:
					offset = *(u8 *)(dout + 1);
					ftspi020_nand_get_feature(priv, offset);
					break;
				case CMD_SET_FEATURE:
					offset = *(u8 *)(dout + 1);
					ftspi020_nand_set_feature(priv, offset);
					break;
				case CMD_PAGE_READ_TO_CACHE:
					offset = get_spi_flash_addr((u8 *)dout, 3);
					ftspi020_nand_read_to_cache(priv, offset);
					break;
				case CMD_PROGRAM_EXECUTE:
					offset = get_spi_flash_addr((u8 *)dout, 3);
					ftspi020_nand_program_execute(priv, offset);
					break;
				default:
					printf("Error: Not support this command(0x%x)\n", cmd);
					break;
			}
		} else {
			// this is write data section
			debug("write len=%d", len);
			writel(len, &regs->cmd[2]);
#ifndef CONFIG_MTD_SPI_NAND
			if(offset < 0x1000000 && !(priv->fourbyte_en)) {
				writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);
				if(cmd == OPCODE_QPP){
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
					       | CMD3_WRITE |CMD3_QUAD| CMD3_CMDIRQ, &regs->cmd[3]);
				}else{
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
					       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
				}
			} else {
				writel(CMD1_ILEN(1) | CMD1_ALEN(4), &regs->cmd[1]);
				if(cmd == OPCODE_QPP){
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
				       | CMD3_WRITE | CMD3_QUAD|CMD3_CMDIRQ, &regs->cmd[3]);
					
				}else{
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
				       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
				}
			}
#else	
			writel(CMD1_ILEN(1) | CMD1_ALEN(2), &regs->cmd[1]);	// for spi nand
			if(cmd == CMD_SET_FEATURE)
			{
				writel(CMD1_ILEN(1) | CMD1_ALEN(1), &regs->cmd[1]);
			} 
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);  
#endif  
			/* data phase */
			while (len) {
				access_byte = min_t(int, len, priv->tx_transfer_size);
				
				/* wait until tx ready */
				while (!(readl(&regs->sr) & SR_TFR))
					;
				
				len -= access_byte;
				
				while (access_byte) {
					if (access_byte >= 4) {
						writel(*(u32 *)dout, &regs->dr);
						dout = (void *)(((u32)dout) + 4);
						access_byte -= 4;
					} else {
						writeb(*(u8 *)dout, &regs->dr);
						dout = (void *)(((u32)dout) + 1);
						access_byte -= 1;
					}
				}
			}
			/* wait until command finish */
			while (!(readl(&regs->isr) & ISR_CMD))
				;
			/* clear isr */
			writel(ISR_CMD, &regs->isr);
		}
		debug("\n");
	} else if(din) {
		//this is read data section;
		debug("read len=%d", len);
		writel(len, &regs->cmd[2]);
#ifndef CONFIG_MTD_SPI_NAND
		if(cmd == OPCODE_FAST_READ_QUAD){
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_QUAD | CMD3_CMDIRQ,
			       &regs->cmd[3]);
		}else if(cmd == OPCODE_FAST_READ_DUAL){
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_DUAL | CMD3_CMDIRQ,
			       &regs->cmd[3]);
		}else{
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_CMDIRQ,
			       &regs->cmd[3]);
		}
		
		while (len) {
			access_byte = min_t(int, len, priv->rx_transfer_size);
			
			/* wait until rx ready */
			while (!(readl(&regs->sr) & SR_RFR))
				;
			
			len -= access_byte;
			
			while (access_byte) {
				if (access_byte >= 4) {
					*((u32 *)din) = readl(&regs->dr);
					din = (void *)(((u32)din) + 4);
					access_byte -= 4;
				} else {
					*((u8 *)din) = readb(&regs->dr);
					din = (void *)(((u32)din) + 1);
					access_byte -= 1;
				}
			}
		}
#else
		writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_CMDIRQ,
		       &regs->cmd[3]);
		
		while (len) {
			access_byte = min_t(int, len, priv->rx_transfer_size);
			
			/* wait until rx ready */
			while (!(readl(&regs->sr) & SR_RFR))
				;
			
			len -= access_byte;
			
			while (access_byte) {
				if (access_byte >= 4) {
					*((u32 *)din) = readl(&regs->dr);
					din = (void *)(((u32)din) + 4);
					access_byte -= 4;
				} else {
					*((u8 *)din) = readb(&regs->dr);
					din = (void *)(((u32)din) + 1);
					access_byte -= 1;
				}
			}
		}
#endif		
		
		/* wait until command finish */
		while (!(readl(&regs->isr) & ISR_CMD))
			;
		/* clear isr */
		writel(ISR_CMD, &regs->isr);
		debug("\n");
	}
	
	return 0;
}
static int ftspi020_spi_set_speed(struct udevice *bus, uint speed)
{
	return 0;
}

static int ftspi020_spi_set_mode(struct udevice *bus, uint mode)
{
	struct ftspi020_spi_priv *priv = dev_get_priv(bus);

	priv->mode = mode;

	return 0;
}

static int ftspi020_spi_probe(struct udevice *bus)
{
	struct ftspi020_spi_priv *priv = dev_get_priv(bus);
	struct ftspi020_regs *regs = priv->regs;
	u32 max_hz = priv->max_hz;
	u32 div = 0;
	u32 spi_mode = priv->mode;

	/* reset */
	writel(CR_ABORT, &regs->cr);
	while (readl(&regs->cr) & CR_ABORT);

	/* clock speed */
	if (max_hz > 0) {
		ulong clk =  CONFIG_FTSPI20_FREQ;
		for (div = 2; div < 8; div += 2) {
			if (clk / div <= max_hz)
				break;
		}
	}

	/* mode + clock */
	switch (spi_mode) {
	case SPI_MODE_0:
	default:
		writel((div >> 1) - 1, &regs->cr);
		break;
	case SPI_MODE_3:
		writel(CR_CLK_MODE_3 | ((div >> 1) - 1), &regs->cr);
		break;
	}

	/* AC timing: worst trace delay and cs delay */
	writel(0xff, &regs->atr);

	debug("ftspi020: div=%d\n", div);

#ifdef CONFIG_FTSPI020_V1_7_0
	priv->rx_transfer_size = (1 << ((readl(&regs->ier) >> 12) & 0x3)) * 32;
	priv->tx_transfer_size = (1 << ((readl(&regs->ier) >>  8) & 0x3)) * 32;
#else
	priv->rx_transfer_size = FEAR_RXFIFO_DEPTH(readl(&regs->fear));
	priv->tx_transfer_size = FEAR_TXFIFO_DEPTH(readl(&regs->fear));
#endif
	debug("ftspi020: tx_transfer_size=%d rx_transfer_size=%d\n", priv->tx_transfer_size, priv->rx_transfer_size);

	return 0;
}

static int ftspi020_spi_ofdata_to_platdata(struct udevice *bus)
{
	struct ftspi020_spi_priv *ds = dev_get_priv(bus);
	const void *blob = gd->fdt_blob;
	int node = dev_of_offset(bus);
	fdt_addr_t addr;

	addr = devfdt_get_addr(bus);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	ds->regs = (struct ftspi020_regs *)addr;
		
	if (!ds->regs) {
		printf("%s: could not map device address\n", __func__);
		return -EINVAL;
	}
	ds->num_cs = fdtdec_get_int(blob, node, "num-cs", 4);
	ds->cs = fdtdec_get_int(blob, node, "cs", 0);
	ds->mode = fdtdec_get_int(blob, node, "mode", SPI_MODE_0);
	ds->max_hz = fdtdec_get_int(blob, node, "max_hz", 25000000);
	ds->fourbyte_en = 0;
	debug("ftsp020: num-cs %d cs %d mode %d max_hz %d\n",ds->num_cs, ds->cs , ds->mode, ds->max_hz);
	return 0;
}

static const struct dm_spi_ops ftspi020_spi_ops = {
	.xfer		= ftspi020_spi_xfer,
	.set_speed	= ftspi020_spi_set_speed,
	.set_mode	= ftspi020_spi_set_mode,
};

static const struct udevice_id ftspi020_spi_ids[] = {
	{ .compatible = "faraday,ftspi020_spi" },
	{}
};

UCLASS_DRIVER(spi_ftspi020) = {
	.id		= UCLASS_SPI_GENERIC,
	.name		= "ftspi020_spi",
};

U_BOOT_DRIVER(ftspi020_spi_drv) = {
	.name	= "ftspi020_spi",
	.id	= UCLASS_SPI,
	.of_match = ftspi020_spi_ids,
	.ops	= &ftspi020_spi_ops,
	.of_to_plat = ftspi020_spi_ofdata_to_platdata,
	.plat_auto = sizeof(struct ftspi020_spi_priv),
	.priv_auto = sizeof(struct ftspi020_spi_priv),
	.probe	= ftspi020_spi_probe,
};

#else	/* !defined(CONFIG_DM_SPI_FLASH) */

static inline struct ftspi020_spi_priv *to_ftspi020_spi(struct spi_slave *slave)
{
	return container_of(slave, struct ftspi020_spi_priv, slave);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
			unsigned int max_hz, unsigned int mode)
{
	struct ftspi020_spi_priv *priv;
	struct ftspi020_regs *regs;

	switch (bus) {
	case 0:
		 regs = (struct ftspi020_regs *)PLATFORM_FTSPI020_BASE;
		 break;
#ifdef PLATFORM_FTSPI020_BASE1
	case 1:
		 regs = (struct ftspi020_regs *)PLATFORM_FTSPI020_BASE1;
		 break;
#endif
#ifdef PLATFORM_FTSPI020_BASE2
	case 2:
		 regs = (struct ftspi020_regs *)PLATFORM_FTSPI020_BASE2;
		 break;
#endif
#ifdef PLATFORM_FTSPI020_BASE3
	case 3:
		 regs = (struct ftspi020_regs *)PLATFORM_FTSPI020_BASE3;
		 break;
#endif
	default:
		 printf("SPI error: unsupported bus %i.  Supported busses 0 - 3\n", bus);
		 return NULL;
	}

	if (cs > 3) {
		printf("SPI error: unsupported chip select %i on bus %i\n", cs, bus);
		return NULL;
	}

	if (mode > SPI_MODE_3) {
		printf("SPI error: unsupported SPI mode %i\n", mode);
		return NULL;
	}

	priv = spi_alloc_slave(struct ftspi020_spi_priv, bus, cs);
	if (!priv) {
		printf("SPI error: malloc of SPI structure failed\n");
		return NULL;
	}

	priv->regs = regs;
	priv->cs = cs;
	priv->max_hz = max_hz;
	priv->mode = mode;

	return &priv->slave;
}

void spi_free_slave(struct spi_slave *slave)
{
	struct ftspi020_spi_priv *priv = to_ftspi020_spi(slave);

	free(priv);
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct ftspi020_spi_priv *priv = to_ftspi020_spi(slave);
	struct ftspi020_regs *regs = priv->regs;
	u32 max_hz = priv->max_hz;
	u32 div = 0;
	u32 spi_mode = priv->mode;

	/* reset */
	writel(CR_ABORT, &regs->cr);
	while (readl(&regs->cr) & CR_ABORT);

	/* clock speed */
	if (max_hz > 0) {
		ulong clk =  CONFIG_FTSPI20_FREQ;
		for (div = 2; div < 8; div += 2) {
			if (clk / div <= max_hz)
				break;
		}
	}
	/* mode + clock */
	switch (spi_mode) {
	case SPI_MODE_0:
    default:
		writel((div >> 1) - 1, &regs->cr);
		break;
	case SPI_MODE_3:
		writel(CR_CLK_MODE_3 | ((div >> 1) - 1), &regs->cr);
		break;

	}

	/* AC timing: worst trace delay and cs delay */
	writel(0xff, &regs->atr);

#ifdef CONFIG_FTSPI020_V1_7_0
	priv->rx_transfer_size = (1 << ((readl(&regs->ier) >> 12) & 0x3)) * 32;
	priv->tx_transfer_size = (1 << ((readl(&regs->ier) >>  8) & 0x3)) * 32;
#else
	priv->rx_transfer_size = FEAR_RXFIFO_DEPTH(readl(&regs->fear));
	priv->tx_transfer_size = FEAR_TXFIFO_DEPTH(readl(&regs->fear));
#endif

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	/* Nothing to do */
}

int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
	     const void *dout, void *din, unsigned long flags)
{
	struct ftspi020_spi_priv *priv = to_ftspi020_spi(slave);
	struct ftspi020_regs *regs = priv->regs;
	u32 len, access_byte;
	static u8 cmd;
	static u32 offset;
	
	if(bitlen == 0) {
		printf("Error: Invalid bit length\n");
		return -EINVAL;
	}
	
	if(dout && din) {
		printf("Error: Cannot have data in and data out set\n");
		return -EINVAL;
	}
	
	len = bitlen / 8;

	if(dout) {
		if(flags & SPI_XFER_BEGIN) {
			//this is command section;
			cmd = *(u8 *)dout;
			offset = get_spi_flash_addr((u8 *)dout, priv->fourbyte_en);
			debug("ftspi020: cmd=0x%x offset=0x%x", cmd, offset);
			switch(cmd)
			{
				case OPCODE_RDID:
					ftspi020_rdid(priv);
					break;
				case CMD_READ_CONFIG:
					ftspi020_rdcfg(priv);
					break;
				case OPCODE_RDSR:
					ftspi020_rdsr(priv);
					break;
				case OPCODE_NORM_READ:
				case OPCODE_CMD_BANKADDR_BRRD:
				case OPCODE_FAST_READ:
				case OPCODE_FAST_READ_DUAL:
				case OPCODE_FAST_READ_QUAD:
					ftspi020_read(priv, cmd, offset);
					break;
				case OPCODE_WREN:
					ftspi020_wren(priv);
					break;
				case OPCODE_WRDI:
					ftspi020_wrdi(priv);
					break;
				case OPCODE_WRSR:
					ftspi020_wrsr(priv);
					break;
				case OPCODE_QPP:
				case OPCODE_PP:
				case OPCODE_CMD_BANKADDR_BRWR:
					ftspi020_write(priv, offset);
					break;
				case OPCODE_CMD_EN4B:
					priv->fourbyte_en = 1;
					ftspi020_en4b(priv);
					break;
				case OPCODE_CMD_EX4B:
					priv->fourbyte_en = 0;
					ftspi020_ex4b(priv);
					break;
				case OPCODE_BE_4K:
				case OPCODE_SE:
					ftspi020_erase(priv, cmd, offset);
					break;
				default:
					printf("Error: Not support this command(0x%x)\n", cmd);
					break;
			}
		} else {
			// this is write data section
			debug("write len=%d", len);
			writel(len, &regs->cmd[2]);
			if(offset < 0x1000000 && !(priv->fourbyte_en)) {
				writel(CMD1_ILEN(1) | CMD1_ALEN(3), &regs->cmd[1]);
				if(cmd == OPCODE_QPP){
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
					       | CMD3_WRITE |CMD3_QUAD| CMD3_CMDIRQ, &regs->cmd[3]);
				}else{
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
					       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
				}
			} else {
				writel(CMD1_ILEN(1) | CMD1_ALEN(4), &regs->cmd[1]);
				if(cmd == OPCODE_QPP){
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
				       | CMD3_WRITE | CMD3_QUAD|CMD3_CMDIRQ, &regs->cmd[3]);
					
				}else{
					writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs)
				       | CMD3_WRITE | CMD3_CMDIRQ, &regs->cmd[3]);
				}
			}

			/* data phase */
			while (len) {
				access_byte = min_t(int, len, priv->tx_transfer_size);
				
				/* wait until tx ready */
				while (!(readl(&regs->sr) & SR_TFR))
					;
				
				len -= access_byte;
				
				while (access_byte) {
					if (access_byte >= 4) {
						writel(*(u32 *)dout, &regs->dr);
						dout = (void *)(((u32)dout) + 4);
						access_byte -= 4;
					} else {
						writeb(*(u8 *)dout,&regs->dr);
						dout = (void *)(((u32)dout) + 1);
						access_byte -= 1;
					}
				}
			}
			/* wait until command finish */
			while (!(readl(&regs->isr) & ISR_CMD))
				;
			/* clear isr */
			writel(ISR_CMD, &regs->isr);
		}
		debug("\n");
	} else if(din) {
		//this is read data section;
		debug("read len=%d", len);
		writel(len, &regs->cmd[2]);
		if(cmd == OPCODE_FAST_READ_QUAD){
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_QUAD | CMD3_CMDIRQ,
			       &regs->cmd[3]);
		}else if(cmd == OPCODE_FAST_READ_DUAL){
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_DUAL | CMD3_CMDIRQ,
			       &regs->cmd[3]);
		}else{
			writel(CMD3_OPC(cmd) | CMD3_CS(priv->cs) | CMD3_CMDIRQ,
			       &regs->cmd[3]);
		}
		
		while (len) {
			access_byte = min_t(int, len, priv->rx_transfer_size);
		
			/* wait until rx ready */
			while (!(readl(&regs->sr) & SR_RFR))
				;
			
			len -= access_byte;
			while (access_byte) {
				if (access_byte >= 4) {
					*((u32 *)din) = readl(&regs->dr);
					din = (void *)(((u32)din) + 4);
					access_byte -= 4;
				} else {
					*((u8 *)din) = readb(&regs->dr);
					din = (void *)(((u32)din) + 1);	
					access_byte -= 1;
				}
			}
		}
		
		/* wait until command finish */
		while (!(readl(&regs->isr) & ISR_CMD))
			;
		/* clear isr */
		writel(ISR_CMD, &regs->isr);
		debug("\n");
	}
	
	return 0;
}

#endif	/* CONFIG_DM_SPI_FLASH */