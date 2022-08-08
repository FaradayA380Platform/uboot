#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/hardware.h>

/* Autoboot */
/*#define CONFIG_BOOTCOMMAND                      "clk 2; nand boot; go 0x85000000"*/

/* Disable MMU/D-CACHE */
/* #define CONFIG_SYS_DCACHE_OFF */

/*
 * CPU and Board Configuration Options
 */
#define CONFIG_USE_IRQ

#ifdef CONFIG_USE_IRQ

#define CONFIG_STACKSIZE_IRQ     SZ_32K
#define CONFIG_STACKSIZE_FIQ     SZ_32K
#endif

/*
 * Serial Info
 */
#ifdef CONFIG_SPL_BUILD
#define CONFIG_CONS_INDEX                       1 /* Console Index port 1(COM1) */
#define CONFIG_SYS_NS16550_COM1                 PLATFORM_FTUART010_BASE
#define CONFIG_SYS_NS16550_SERIAL
#endif
#define CONFIG_SYS_NS16550_CLK                  33000000
#define CONFIG_SYS_NS16550_REG_SIZE             (-4)
#define CONFIG_BAUDRATE                         115200

/*
 * Timer Info
 */
#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_TIMER_RATE                   50000000
#endif

/*
 * Memory Configuration
 */
#define CONFIG_NR_DRAM_BANKS                    1
#define CONFIG_SYS_SDRAM_BASE                   0x40000000
#define CONFIG_SYS_SDRAM_SIZE                   SZ_32M

#define CONFIG_SYS_MEMTEST_START                (CONFIG_SYS_SDRAM_BASE + SZ_1M)
#define CONFIG_SYS_MEMTEST_END                  (CONFIG_SYS_SDRAM_BASE + SZ_16M)

#ifdef CONFIG_SPL
#define CONFIG_SPL_STACK                        0x003F0000
#define CONFIG_SYS_LOAD_ADDR                    CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_SPI_U_BOOT_OFFS              0x00000000/*0x00080000*/
#endif

/*
 * USB XHCI
 */
#define CONFIG_USB_MAX_CONTROLLER_COUNT         1
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS      15

/*
 * Environment
 */
#define CONFIG_ENV_SIZE                         SZ_64K
#define CONFIG_EXTRA_ENV_SETTINGS \
	/* Default network configuration */ \
	"ethaddr=00:41:71:00:00:50\0" \
	"serverip=10.0.0.128\0" \
	"ipaddr=10.0.0.192\0"

/*
 * Faraday Common Configuration
 */
#include "faraday-common.h"

#define CONFIG_CMD_PCI                          1
#define CONFIG_CMD_MEMTEST                      1
#define CONFIG_CMD_LOADB                        1
#define CONFIG_CMD_CLK                          1

#endif
