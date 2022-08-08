/*
 * (C) Copyright 2012 Faraday Technology
 * Bing-Yao Luo <bjluo@faraday-tech.com>
 *
 * Configuration settings for the Faraday A380 board.
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

#include <asm/arch/a380.h>

/*#define CONFIG_SOC_CA9*/
/*#define CONFIG_BOARD_A380*/

/*
 * mach-type definition
 */
#define MACH_TYPE_FARADAY           758
#define CONFIG_MACH_TYPE            MACH_TYPE_FARADAY

/*
 * GIC and Interrupt Configuration Options
 */
#define CONFIG_GICV2
#define CONFIG_USE_IRQ

#define GICD_BASE                   PLATFORM_CPU_PERIPHBASE + 0x1000
#define GICC_BASE                   PLATFORM_CPU_PERIPHBASE + 0x100

#define CONFIG_USB_EHCI_BASE        FOTG210_BASE0

/*
 * Timer
 */
#define CONFIG_SYS_HZ               1000	/* timer ticks per second */

/*
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.
 * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */
#define CONFIG_SYS_SDRAM_BASE       0x40000000
#define CONFIG_SYS_SDRAM_SIZE       SZ_256M
#define CONFIG_SYS_LOAD_ADDR        CONFIG_SYS_TEXT_BASE

/*
 * Initial stack pointer: GENERATED_GBL_DATA_SIZE in internal SRAM.
 * Inside the board_init_f, the gd is first assigned to
 * (CONFIG_SYS_INIT_SP_ADDR) & ~0x07) and then relocated to DRAM
 * while calling relocate_code.
 */
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + SZ_4K - GENERATED_GBL_DATA_SIZE)

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN       SZ_2M
#define CONFIG_SYS_MONITOR_LEN      SZ_512K

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_CBSIZE           256		/* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE \
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE         CONFIG_SYS_CBSIZE
#define CONFIG_SYS_BOOTM_LEN        SZ_64M

/*
 * Environment
 */
#define CONFIG_EXTRA_ENV_SETTINGS \
	"dfu_alt_info_ram=" DFU_ALT_INFO_RAM "\0" \
	"dfu_bufsiz=0x500000\0"

/*
 * Linux kernel tagged list
 * Make the "bootargs" environment variable is used by Linux kernel as
 * command-line tag.
 */
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG 
#define CONFIG_SETUP_MEMORY_TAGS

/*
 * Boot argument for uImage to pass into Linux kernel after start up
 */
/*
#define CONFIG_BOOTARGS             "mem=512M console=ttyS0,38400 root=/dev/mmcblk0p5 rw rootfstype=ext4 rootwait"
*/

/*
 * Boot from SD card Command with "uImage"
 */
/*
#define CONFIG_BOOTCOMMAND          "mmcinfo; fatload mmc 0 uImage; bootm"
*/

/*
 * USB EHCI
 */
#define CONFIG_EHCI_IS_TDI

#define DFU_ALT_INFO_RAM \
	"ramdisk ram 0x41000000 0x2500000"

/*
 * NIC driver
 */
#define CONFIG_ETHADDR              00:84:14:72:61:69	/* used by common/env_common.c */
#define CONFIG_NETMASK              255.255.255.0
#define CONFIG_IPADDR               192.168.0.100
#define CONFIG_SERVERIP             192.168.0.2
#define CONFIG_NET_MULTI            1
#define CONFIG_NET_RETRY_COUNT      20

/*
 * NAND driver
 */
#define CONFIG_SYS_MAX_NAND_DEVICE  1		/* Max. num. of devices */

#endif	/* __CONFIG_H */
