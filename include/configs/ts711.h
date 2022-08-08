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

#ifndef __CONFIG_TS711_H
#define __CONFIG_TS711_H

#include <asm/arch/hardware.h>

/*
 * mach-type definition
 */
#define MACH_TYPE_FARADAY   758 
#define CONFIG_MACH_TYPE	MACH_TYPE_FARADAY

/*
 * CPU
 */
#undef CONFIG_SKIP_LOWLEVEL_INIT

/*
 * Environment variables
 */
#if 0 
#define CONFIG_ENV_OFFSET		    0x07FC0000
#define CONFIG_ENV_OFFSET_REDUND	0x07FE0000
#define CONFIG_ENV_SIZE 		    0x20000
#endif

/*
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.
 * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */
#define PHYS_SDRAM_1		        0x20000000
#define PHYS_SDRAM_1_SIZE	        SZ_256M
#define CONFIG_SYS_SDRAM_BASE       PHYS_SDRAM_1
#define CONFIG_SYS_SDRAM_SIZE       PHYS_SDRAM_1_SIZE
#define CONFIG_SYS_INIT_RAM_SIZE    SZ_4K
#define CONFIG_SYS_LOAD_ADDR       (CONFIG_SYS_SDRAM_BASE + SZ_16M)

/*
 * Initial stack pointer: 4k - GENERATED_GBL_DATA_SIZE in internal SRAM,
 * leaving the correct space for initial global data structure above
 * that address while providing maximum stack area below.
 */
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_SDRAM_BASE + SZ_4K - GENERATED_GBL_DATA_SIZE)
#if 0	
#define CONFIG_SYS_MEMTEST_START    (CONFIG_SYS_SDRAM_BASE + SZ_16M)
#define CONFIG_SYS_MEMTEST_END      (CONFIG_SYS_SDRAM_BASE + SZ_32M)
#endif

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN       SZ_8M
#define CONFIG_SYS_MONITOR_LEN      SZ_512K 

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_CBSIZE	256		/* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	\
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

/*
 * Linux kernel tagged list
 * Make the "bootargs" environment variable is used by Linux kernel as
 * command-line tag.
 */
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG 
#define CONFIG_SETUP_MEMORY_TAGS

#endif	/* __CONFIG_H */
