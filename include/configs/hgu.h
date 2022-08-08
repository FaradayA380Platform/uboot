#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/hardware.h>

/* Autoboot */
/* #define CONFIG_BOOTCOMMAND          "bootf" */
/* #define CONFIG_BOOTCOMMAND "clk 1; mw 0x90900444 0x4; nand read 0x42000000 0x200000 0xe00000; go 0x42000000" */
/*#define CONFIG_BOOTCOMMAND " "*/

/* Disable MMU/D-CACHE */
/* #define CONFIG_SYS_DCACHE_OFF */

/*
 * Serial Info
 */
#define CONFIG_SYS_NS16550_CLK		33000000
#define CONFIG_BAUDRATE				115200
#define CONFIG_SPL_BOARD_INIT

#define CONFIG_SPL_STACK \
	(CONFIG_SYS_SRAM_BASE + SZ_4K - GENERATED_GBL_DATA_SIZE)

/*
 * Memory Configuration
 */
#define CONFIG_SYS_MALLOC_LEN       SZ_16M 
 
#define CONFIG_SYS_SDRAM_BASE		0x40000000
#define CONFIG_SYS_SDRAM_SIZE		SZ_64M

#define CONFIG_SYS_MEMTEST_START	(CONFIG_SYS_SDRAM_BASE + SZ_1M)
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + SZ_16M)

/* NAND Flash */
#define CONFIG_NAND_FTNANDC024_VERSION			210
#define CONFIG_SYS_MAX_NAND_DEVICE      1 /* Max. num. of devices */
#define CONFIG_NAND_BSP_LOOKUP_TABLE_BLOCK 4 /* Lookup table offset */
#define CONFIG_NAND_BSP_BLOCK  6 /* data image offset */

#define CONFIG_NAND_BSP_LOOKUP_TABLE_SCAN_NUM 5
#define CONFIG_SPL_BOOTIMG_MAGIC_NUMBER 0x41333830
#define CONFIG_BSP_BOOTIMG_MAGIC_NUMBER 0x11111111
#define CONFIG_LOOKUP_TABLE_MAGIC_NUMBER 0x22222222
#define CONFIG_DATAIMG_MAGIC_NUMBER 0x33333333

/*
 * USB EHCI

#define CONFIG_USB_EHCI_BASE        CONFIG_FOTG210_BASE1
#define CONFIG_USB_MAX_CONTROLLER_COUNT    1
 */
/*
 * USB Gadget/Device

#define CONFIG_SUPP_USB_RNDIS       1
#define CONFIG_USB_GADGET_FOTG210_V1_11_0	1	 *//* rev. 1.11.0+ */

#define FIFO_0			0
#define FIFO_1			1
#define FIFO_IN		FIFO_0
#define FIFO_OUT	FIFO_1

/*
 * Environment
 */

/*#define CONFIG_USE_IRQ */

#define DFU_ALT_INFO_RAM \
	"ramdisk ram 0x41000000 0x100000"

/*#define CONFIG_BOOTCOMMAND "setenv dfu_alt_info ${dfu_alt_info_ram};dfu 0 ram 0"*/

#define DFU_ALT_INFO_RAM \
	"ramdisk ram 0x41000000 0x100000"


/*#define CONFIG_USE_IRQ */
#define CONFIG_EXTRA_ENV_SETTINGS \
	/* Default network configuration */ \
	"ethaddr=00:41:71:00:00:50\0" \
	"serverip=10.0.0.128\0" \
	"ipaddr=10.0.0.192\0" \
	"dfu_alt_info_ram=" DFU_ALT_INFO_RAM "\0" \
	"dfu_bufsiz=0x100000\0"

/*
 * Faraday Common Configuration
 */
#include "faraday-common.h"

#define CONFIG_CMD_MEMTEST			1
#define CONFIG_CMD_LOADB			1
/*
#define CONFIG_CMD_NAND_YAFFS		1
#define CONFIG_CMD_NAND_YAFFS_COMPAT_YAFFS2UTIL	1
#define CONFIG_CMD_CLK      1  
#define CONFIG_CMD_TMR 1
*/
#endif
