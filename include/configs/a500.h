/*
 * (C) Copyright 2015 Faraday Technology
 * Fu-Tsung Hsu <mark_hs@faraday-tech.com>
 *
 * Configuration settings for the Faraday A369 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/a500.h>

/*
 * mach-type definition
 */
#define MACH_TYPE_FARADAY	758
#define CONFIG_MACH_TYPE	MACH_TYPE_FARADAY
/*#define CONFIG_SOC_CA9*/
/*#define CONFIG_BOARD_A389*/


#define GICD_BASE		0x18701000
#define GICC_BASE            0x18702000
#define CPU_RELEASE_ADDR    0xa10d004
#define CONFIG_GICV2
#define COUNTER_FREQUENCY	25000000

/*
 * CPU
 */

/* Uncache controller register base area */
#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_ARCH_EARLY_INIT_R
#endif

/*
 * Linux kernel tagged list
 * Make the "bootargs" environment variable is used by Linux kernel as
 * command-line tag.
 */
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG 
#define CONFIG_SETUP_MEMORY_TAGS

/*
 * CPU and Board Configuration Options
 */
/*
#define CONFIG_USE_IRQ

#ifdef CONFIG_USE_IRQ
#define CONFIG_CMD_IRQ

#define CONFIG_STACKSIZE_IRQ     SZ_32K
#define CONFIG_STACKSIZE_FIQ     SZ_32K
#endif
*/
/*
 * Timer
 */
#define CONFIG_SYS_HZ		1000	/* timer ticks per second */
#define MAIN_CLK 	33000000
/*#define CONFIG_ARMV8_SWITCH_TO_EL1*/
#define CONFIG_SYS_BOOTM_LEN		SZ_64M
/*#define CONFIG_OF_LIBFDT*/


/*
 * Serial console configuration
 */

/* FTUART is a high speed NS 16C550A compatible UART */
/*#define CONFIG_BAUDRATE		115200*/
/*#define CONFIG_CONS_INDEX	1*/ /* Console Index port 1(COM1) */
/*#define CONFIG_SYS_NS16550_COM1	CONFIG_FTUART010_BASE*/
/*#define CONFIG_SYS_NS16550*/
/*#define CONFIG_SYS_NS16550_SERIAL*/
/*#define CONFIG_SYS_NS16550_CLK		18432000*/

/*
 * Command line configuration.
 *
#include <config_cmd_default.h>

*
 * U-Boot general commands
 */

/*#define CONFIG_SPI_Faraday*/
/*#define CONFIG_CMD_NET*/		/* bootp, tftpboot, rarpboot    */
/*#define CONFIG_CMD_CACHE*/	/* cache enable/disable command */
#define CONFIG_VERSION_VARIABLE

/*
 * Environment variables
 */
#define CONFIG_ENV_OFFSET		0x07FC0000
#define CONFIG_ENV_OFFSET_REDUND	0x07FE0000

/* Init DDR on A369 board from zynq board(SoC) 
 * Wake up CPU1
 */
#undef CONFIG_SKIP_LOWLEVEL_INIT

/*
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.                                                                                          * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */


#define PHYS_SDRAM_1		0x80000000
#define PHYS_SDRAM_1_SIZE	SZ_512M


/*
#define CONFIG_SYS_TEXT_BASE        0x18008000
#define PHYS_SDRAM_1		0x18000000
#define PHYS_SDRAM_1_SIZE	SZ_2M
#define CONFIG_SYS_MALLOC_LEN       SZ_32K
#define CONFIG_STACKSIZE            SZ_16K
*/


#if (CONFIG_NR_DRAM_BANKS == 2)
#define PHYS_SDRAM_2		0x20000000
#define PHYS_SDRAM_2_SIZE	SZ_256M
#endif

#define CONFIG_SYS_SDRAM_BASE       PHYS_SDRAM_1
#define CONFIG_SYS_SDRAM_SIZE       PHYS_SDRAM_1_SIZE
#define CONFIG_SYS_INIT_RAM_SIZE    SZ_4K
/*
 * Initial stack pointer: 4k - GENERATED_GBL_DATA_SIZE in internal SRAM,
 * leaving the correct space for initial global data structure above
 * that address while providing maximum stack area below.
 */
#define CONFIG_SYS_MEMTEST_START    (CONFIG_SYS_SDRAM_BASE + SZ_16M)
#define CONFIG_SYS_MEMTEST_END      (CONFIG_SYS_SDRAM_BASE + SZ_32M)

/*
 * Size of malloc() pool
 */


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

/*#define CONFIG_BOOTCOMMAND	"sf probe;sf read 0x82000000 0xd00000 0x2000;sf read 0x83080000 0x200000 0xb00000;bootm 0x83080000 - 0x82000000;"
#define CONFIG_BOOTCOMMAND	"bootm 0x83080000 - 0x82000000;"*/

/*
 * Boot argument for uImage to pass into Linux kernel after start up
#define CONFIG_BOOTARGS 	"mem=512M console=ttyS0,38400 root=/dev/mmcblk0p5 rw rootfstype=ext4 rootwait"
 */

/*                                                                                                                             * FAT (USB & MMC)                                                                                                            
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_FAT
*/
/*
 * MMC(SD) card
 */
/* 
#define CONFIG_GENERIC_MMC
#define CONFIG_FTSDC010
#define CONFIG_CMD_MMC
#define CONFIG_MMC
*/
/*
 * Boot from SD card Command with "uImage"
 */
/*
#define CONFIG_BOOTCOMMAND	"mmcinfo; fatload mmc 0 uImage; bootm"
#define CONFIG_BOOTDELAY	3
*/
/*
 * FLASH and environment organization
 */
/*
 * NOR Flash
 */
#undef CONFIG_CMD_IMLS

#ifndef CONFIG_SYS_NO_FLASH
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_BASE    	0x20000000
#define PHYS_FLASH_SIZE  		SZ_64M
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	1024	/* max. sector number */
#define CFG_FLASH_EMPTY_INFO	/* print 'E' for empty sector on flinfo */
#define CONFIG_CMD_FLASH
#endif  /* !CONFIG_SYS_NO_FLASH */

/*
 * NAND Flash
 */
#define CONFIG_SYS_MAX_NAND_DEVICE      		1	/* Max. num. of devices */
#define CONFIG_NAND_BSP_LOOKUP_TABLE_BLOCK		4	/* Lookup table offset */
#define CONFIG_NAND_BSP_BLOCK  					6	/* data image offset */

#define CONFIG_NAND_BSP_LOOKUP_TABLE_SCAN_NUM	5
#define CONFIG_SPL_BOOTIMG_MAGIC_NUMBER			0x41333830
#define CONFIG_BSP_BOOTIMG_MAGIC_NUMBER			0x11111111
#define CONFIG_LOOKUP_TABLE_MAGIC_NUMBER		0x22222222
#define CONFIG_DATAIMG_MAGIC_NUMBER				0x33333333


/*
 * USB EHCI
 */
#define CONFIG_EHCI_IS_TDI

/*
 * USB XHCI
 */
#define CONFIG_USB_MAX_CONTROLLER_COUNT         1

/*
 * NIC driver
 */
#define CONFIG_ETHADDR				00:84:14:72:61:69  /* used by common/env_common.c */
#define CONFIG_NETMASK				255.255.255.0
#define CONFIG_IPADDR				192.168.0.100
#define CONFIG_SERVERIP				192.168.0.2
#define CONFIG_NET_MULTI			1
#define CONFIG_NET_RETRY_COUNT		20

#define CONFIG_CMD_PING				1
#define CONFIG_ETHADDR      00:84:14:72:61:69  /* used by common/env_common.c */

#include "faraday-common.h"

#endif	/* __CONFIG_H */
