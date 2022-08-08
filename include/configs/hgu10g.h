#ifndef __CONFIG_H
#define __CONFIG_H

/*#define CONFIG_HGU10G_PLATFORM                  1*/

#include <asm/hardware.h>

/* Autoboot */
/* #define CONFIG_BOOTCOMMAND                      "bootf" */
/* #define CONFIG_BOOTCOMMAND                      "clk 1; mw 0x90900444 0x4; nand read 0x42000000 0x200000 0xe00000; go 0x42000000" */
#define CONFIG_BOOTCOMMAND                      	" "/*"clk 1; mw 0x90900444 0x4; nand boot; go 0x42000070"*/

/* Disable MMU/D-CACHE */
/* #define CONFIG_SYS_DCACHE_OFF */

/*
 * Serial Info
 */
#define CONFIG_SYS_NS16550_CLK                  31250000
#define CONFIG_BAUDRATE                         115200

/*
 * Memory Configuration
 */

#define CONFIG_SYS_SDRAM_BASE                   0x80000000
#define CONFIG_SYS_SDRAM_SIZE                   SZ_64M

#define CONFIG_SYS_TEXT_BASE                    0x80800000

#define CONFIG_SYS_MEMTEST_START                (CONFIG_SYS_SDRAM_BASE + SZ_1M)
#define CONFIG_SYS_MEMTEST_END                  (CONFIG_SYS_SDRAM_BASE + SZ_16M)

/* NAND Flash */
#define CONFIG_NAND_FTNANDC024_VERSION			230
/*#define CONFIG_NANDC_INFO_STRAP_FULL			1 *//*strap pin setting,  used in real project */
#define CONFIG_SYS_MAX_NAND_DEVICE              1 /* Max. num. of devices */
#define CONFIG_NAND_BSP_LOOKUP_TABLE_BLOCK      4 /* Lookup table offset */
#define CONFIG_NAND_BSP_BLOCK                   6 /* data image offset */

#define CONFIG_NAND_BSP_LOOKUP_TABLE_SCAN_NUM   5
#define CONFIG_SPL_BOOTIMG_MAGIC_NUMBER         0x41333830
#define CONFIG_BSP_BOOTIMG_MAGIC_NUMBER         0x11111111
#define CONFIG_LOOKUP_TABLE_MAGIC_NUMBER        0x22222222
#define CONFIG_DATAIMG_MAGIC_NUMBER             0x33333333

/*
 * USB EHCI

#define CONFIG_USB_EHCI_BASE                    CONFIG_DWC_OTG3_BASE
#define CONFIG_USB_MAX_CONTROLLER_COUNT         1
 */
/*
 * USB Gadget/Device

#define CONFIG_SUPP_USB_RNDIS                   1
 */
/*
 * Environment
 */

#define CONFIG_USE_IRQ
#define CONFIG_EXTRA_ENV_SETTINGS \
	/* Default network configuration */ \
	"ethaddr=00:41:71:00:00:50\0" \
	"serverip=10.0.0.128\0" \
	"ipaddr=10.0.0.192\0"

/*
 * Faraday Common Configuration
 */
#include "faraday-common.h"

#define CONFIG_CMD_MEMTEST                      1
#define CONFIG_CMD_LOADB                        1

#define CONFIG_CMD_CLK                          1

#endif
