/*
 * (C) Copyright 2020 Faraday Technology
 * Bo-Cun Chen <bcchen@faraday-tech.com>
 *
 * Configuration settings for the Faraday GC1610 board.
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

#include <linux/sizes.h>
#include <asm/arch/hardware.h>

/*
 * mach-type definition
 */
#define MACH_TYPE_FARADAY           758
#define CONFIG_MACH_TYPE            MACH_TYPE_FARADAY

/*
 * CPU
 */
#undef CONFIG_SKIP_LOWLEVEL_INIT
//#define CONFIG_ARMV8_SWITCH_TO_EL1

#define COUNTER_FREQUENCY           774000000
#define CPU_RELEASE_ADDR            0x201008a0

/*
 * GIC and Interrupt Configuration Options
 */
//#define CONFIG_GICV2 
#define CONFIG_GICV3 
/*#define CONFIG_USE_IRQ*/

#define GICD_BASE                   0x400000
#define GICR_BASE					0x440000         
//#define GICC_BASE                   PLATFORM_CPU_PERIPHBASE + 0x2000
#define GICC_BASE                   PLATFORM_CPU_PERIPHBASE//PLATFORM_CPU_PERIPHBASE + 0x2000

#define CONFIG_SYS_NS16550_REG_SIZE             (-4)


/*
 * NIC
 */
#define CONFIG_ETHADDR              00:84:14:72:61:69  /* used by common/env_common.c */
#define CONFIG_NETMASK              255.255.255.0
#define CONFIG_IPADDR               192.168.1.100
#define CONFIG_SERVERIP             192.168.1.1
#define CONFIG_NET_MULTI            1
#define CONFIG_NET_RETRY_COUNT      20
/*
#define CONFIG_DRIVER_ETHER         1
*/
/*
 * Environment variables
 */
#define CONFIG_ENV_OFFSET           0x07FC0000
#define CONFIG_ENV_OFFSET_REDUND    0x07FE0000

/*
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.
 * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */
#define CONFIG_SYS_SDRAM_BASE       0x80000000
#define CONFIG_SYS_SDRAM_SIZE       SZ_256M
#define CONFIG_SYS_LOAD_ADDR        CONFIG_SYS_TEXT_BASE

#ifdef CONFIG_SPL_BUILD
#define CONFIG_CONS_INDEX           1 /* Console Index port 1(COM1) */
#define CONFIG_SYS_NS16550_CLK      31250000
#define CONFIG_SYS_NS16550_REG_SIZE (-4)
#define CONFIG_SYS_NS16550_COM1     PLATFORM_FTUART010_BASE
#define CONFIG_SYS_NS16550_SERIAL

#define CONFIG_SPL_MAX_SIZE         0x7000
#define CONFIG_SPL_BSS_START_ADDR   0x00206000
#define CONFIG_SPL_BSS_MAX_SIZE     0x1000
#define CONFIG_SPL_STACK            0x208000
#define CONFIG_SYS_SPI_U_BOOT_OFFS 0x8000

#endif

/*
 * Initial stack pointer: 4k - GENERATED_GBL_DATA_SIZE in internal SRAM,
 * leaving the correct space for initial global data structure above
 * that address while providing maximum stack area below.
 */
/*
#define CONFIG_SYS_MEMTEST_START    (CONFIG_SYS_SDRAM_BASE + SZ_16M)
#define CONFIG_SYS_MEMTEST_END      (CONFIG_SYS_SDRAM_BASE + SZ_32M)
*/
/*
 * Initial stack pointer: GENERATED_GBL_DATA_SIZE in internal SRAM.
 * Inside the board_init_f, the gd is first assigned to
 * (CONFIG_SYS_INIT_SP_ADDR) & ~0x07) and then relocated to DRAM
 * while calling relocate_code.
 */
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + SZ_4K - GENERATED_GBL_DATA_SIZE)
/*	
#define CONFIG_STACKSIZE            SZ_512K
#define CONFIG_STACKSIZE_IRQ        SZ_32K
#define CONFIG_STACKSIZE_FIQ        SZ_32K
*/
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
#define CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE         CONFIG_SYS_CBSIZE
#define CONFIG_SYS_BOOTM_LEN        SZ_64M

/*
 * NAND Flash
 */
#if 1
#define CONFIG_SYS_NAND_SELF_INIT 1
#define CONFIG_SYS_MAX_NAND_DEVICE      		1	/* Max. num. of devices */

#define NAND_BSP_LOOKUP_TABLE_BLOCK		4	/* Lookup table offset */
#define NAND_BSP_BLOCK  					6	/* data image offset */
#define NAND_BSP_LOOKUP_TABLE_SCAN_NUM	5
#define SPL_BOOTIMG_MAGIC_NUMBER			0x41333830
#define BSP_BOOTIMG_MAGIC_NUMBER			0x11111111
#define LOOKUP_TABLE_MAGIC_NUMBER		0x22222222
#define DATAIMG_MAGIC_NUMBER				0x33333333

#define NANDC024_BASE 0x10000000
#define NANDC024_DATA_BASE 0x10100000
# define CONFIG_SYS_NAND_BASE           NANDC024_BASE
# define CONFIG_SYS_NAND_DATA_BASE		NANDC024_DATA_BASE
# define CONFIG_MTD_NAND_VERIFY_WRITE
/*# define CONFIG_CMD_NAND*/
#endif

/*
 * Linux kernel tagged list
 * Make the "bootargs" environment variable is used by Linux kernel as
 * command-line tag.
 */
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG 
#define CONFIG_SETUP_MEMORY_TAGS

/*#define CONFIG_EXTRA_ENV_SETTINGS	 "fdtcontroladdr=0x81000000\0"*/

#define CONFIG_SYS_MAX_NAND_DEVICE 1


#endif	/* __CONFIG_H */
