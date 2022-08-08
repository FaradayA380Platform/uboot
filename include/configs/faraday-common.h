/*
 * include/configs/faraday-common.h
 *
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
 *
 * This file is released under the terms of GPL v2 and any later version.
 * See the file COPYING in the root directory of the source tree for details.
 */

#ifndef __CONFIG_FARADAY_COMMON_H
#define __CONFIG_FARADAY_COMMON_H

#include <asm/mach-types.h>

#ifndef MACH_TYPE_FARADAY
#define MACH_TYPE_FARADAY           758
#endif

/*
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.
 * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */
 /*
#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE        0x40080000
#endif
*/
#define CONFIG_BOARD_EARLY_INIT_F   1

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

/*#define CONFIG_SYS_MALLOC_LEN       SZ_8M*/  /*Config it from menuconfig*/
#define CONFIG_SYS_MONITOR_LEN      SZ_512K

/*
 * CPU
 */
/*#define CONFIG_FARADAY*/
#define CONFIG_ARCH_CPU_INIT
/*
#define CONFIG_DISPLAY_CPUINFO
*/
#ifndef CONFIG_SYS_CACHELINE_SIZE
#define CONFIG_SYS_CACHELINE_SIZE	32
#endif
#ifndef CONFIG_SYS_DCACHE_OFF
# define CONFIG_SYS_ARM_CACHE_WRITETHROUGH
#endif

/*
 * Interrupt
 */
 
#if defined(CONFIG_USE_IRQ) && defined(CONFIG_FTI2C010_BASE)
/* #define CONFIG_FTINTC020 */
#endif


/*
 * Timer
 */
#define CONFIG_SYS_HZ               1000

#ifdef FTTMR010_BASE
# define FTTMR010
# define TIMER_BASE FTTMR010_BASE
#endif /* CONFIG_FTTMR010_BASE */

#ifdef CONFIG_FTPWMTMR010_BASE
# define CONFIG_FTPWMTMR010
#endif /* CONFIG_FTPWMTMR010_BASE */

/*
 * Serial Info
 */
#ifndef CONFIG_BOARD_ALADDIN
#ifdef FTUART010_BASE
/*# define CONFIG_SYS_NS16550*/
# define CONFIG_SYS_NS16550_SERIAL




# define CONFIG_SYS_NS16550_COM1     FTUART010_BASE
# define CONFIG_SYS_NS16550_MEM32
# define CONFIG_SYS_NS16550_REG_SIZE -4
# define CONFIG_CONS_INDEX           1

# undef  CONFIG_HWFLOW
# undef  CONFIG_MODEM_SUPPORT
#endif /* #ifdef CONFIG_FTUART010_BASE */
#endif
/*
 * NIC driver
 */
#ifdef CONFIG_FTMAC110_BASE
# define CONFIG_FTMAC110
#endif
#ifdef CONFIG_FTGMAC100_BASE
# define CONFIG_FTGMAC100
# define CONFIG_FTGMAC100_EGIGA    /* Used by Ratbert's ftgmac100 only */
# define CONFIG_PHY_GIGE /* Enable giga phy support for miiphyutil.c */
#endif
#if defined(CONFIG_FTMAC110) || defined(CONFIG_FTGMAC100)
# define CONFIG_PHY_MAX_ADDR    32 /* Used by Ratbert's ftgmac100 only */
# define CONFIG_RANDOM_MACADDR
# define CONFIG_MII
# define CONFIG_NET_MULTI
# define CONFIG_NET_RETRY_COUNT 20
# define CONFIG_DRIVER_ETHER
# define CONFIG_CMD_MII
# define CONFIG_CMD_PING
#endif

/*
 * I2C Controller
 
#ifdef CONFIG_FTI2C010_BASE3
# define CONFIG_SYS_MAX_I2C_BUS      4
#elif defined(CONFIG_FTI2C010_BASE2)
# define CONFIG_SYS_MAX_I2C_BUS      3
#elif defined(CONFIG_FTI2C010_BASE1)
# define CONFIG_SYS_MAX_I2C_BUS      2
#elif defined(CONFIG_FTI2C010_BASE)
# define CONFIG_SYS_MAX_I2C_BUS      1
#else
# define CONFIG_SYS_MAX_I2C_BUS      0
#endif
#if CONFIG_SYS_MAX_I2C_BUS > 0
# define CONFIG_FTI2C010
# define CONFIG_HARD_I2C
# define CONFIG_SYS_I2C_SPEED        5000
# define CONFIG_SYS_I2C_SLAVE        0
# define CONFIG_I2C_CMD_TREE
#endif
#if CONFIG_SYS_MAX_I2C_BUS > 1
# define CONFIG_I2C_MULTI_BUS
#endif
*/
/*
 * I2C-EEPROM
 */
#ifdef CONFIG_ENV_EEPROM_IS_ON_I2C
# define CONFIG_CMD_EEPROM
# define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS      3
# define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS  5
# define CONFIG_SYS_I2C_EEPROM_ADDR_LEN         1
# define CONFIG_SYS_I2C_MULTI_EEPROMS
#endif

/*
 * SPI Bus
 */
#if 0
#ifdef CONFIG_FTSSP010_BASE
# define CONFIG_FTSSP010_SPI
# define CONFIG_SPI
# define CONFIG_HARD_SPI
# define CONFIG_CMD_SPI
# define CONFIG_ENV_SPI_BUS         0
# define CONFIG_ENV_SPI_CS          0
# define CONFIG_ENV_SPI_MAX_HZ      25000000
# define CONFIG_DEFAULT_SPI_MODE    SPI_MODE_0
#endif
#endif
/*
 * SPI Flash
 */
#ifdef CONFIG_FTSPI020_BASE
/*
# define CONFIG_FTSPI020
*/
#elif defined(CONFIG_SPI)
/*
# define CONFIG_SPI_FLASH
# define CONFIG_SPI_FLASH_MACRONIX
# define CONFIG_SPI_FLASH_WINBOND
*/
#endif

/*
#if defined(CONFIG_FTSPI020) || defined(CONFIG_SPI_FLASH)
# define CONFIG_SF_DEFAULT_MODE     SPI_MODE_0
# define CONFIG_SF_DEFAULT_SPEED    25000000
#endif
*/

/*
 * NOR Flash
 */
#ifdef CONFIG_SYS_FLASH_BASE
# define CONFIG_SYS_FLASH_CFI
# define CONFIG_FLASH_CFI_DRIVER
# define CFG_FLASH_EMPTY_INFO /* print 'E' for empty sector on flinfo */
# define CONFIG_CMD_IMLS
# define CONFIG_CMD_FLASH
#else
/*
# define CONFIG_SYS_NO_FLASH
*/
#endif /* !CONFIG_SYS_NO_FLASH */

/*
 * NAND Flash
 */
#ifdef CONFIG_FTNANDC021_BASE
# define CONFIG_SYS_NAND_SELF_INIT
# define CONFIG_NAND_FTNANDC021
# define CONFIG_SYS_NAND_BASE           CONFIG_FTNANDC021_BASE
# define CONFIG_MTD_NAND_VERIFY_WRITE 
# define CONFIG_CMD_NAND
#endif

#ifdef CONFIG_FTNANDC024_BASE
# define CONFIG_SYS_NAND_SELF_INIT
# define CONFIG_NAND_FTNANDC024
# define CONFIG_SYS_NAND_BASE           CONFIG_FTNANDC024_BASE
# define CONFIG_SYS_NAND_DATA_BASE		CONFIG_FTNANDC024_DATA_BASE
# define CONFIG_MTD_NAND_VERIFY_WRITE
# define CONFIG_CMD_NAND
#endif

/*
 * MMC/SD
 */
#ifdef CONFIG_FTSDC010_BASE
# define CONFIG_FTSDC010
# define CONFIG_FTSDC010_SDIO /* The hardware core supports SDIO */
# define CONFIG_MMC
# define CONFIG_CMD_MMC
# define CONFIG_GENERIC_MMC
#endif

#ifdef CONFIG_FTSDC021_BASE
# define CONFIG_FTSDC021
# define CONFIG_SDHCI
# define CONFIG_MMC
# define CONFIG_CMD_MMC
# define CONFIG_GENERIC_MMC
#endif

/*
 * USB EHCI
 */
#ifdef CONFIG_USB
#if CONFIG_USB_MAX_CONTROLLER_COUNT > 0
# define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS 1
# define CONFIG_USB_HUB_MIN_POWER_ON_DELAY  500
# define CONFIG_USB_EHCI
# define CONFIG_USB_EHCI_FARADAY
# define CONFIG_EHCI_IS_TDI
# define CONFIG_CMD_USB
# define CONFIG_USB_STORAGE
#endif
#endif
/*
 * USB Gadget
 */
#if defined(CONFIG_SUPP_USB_RNDIS) && defined(CONFIG_FOTG210_BASE)
# define CONFIG_USB_GADGET
# define CONFIG_USB_GADGET_FOTG210
# define CONFIG_USB_GADGET_DUALSPEED
# define CONFIG_USB_ETHER
# define CONFIG_USB_ETH_RNDIS
# define CONFIG_USBNET_DEV_ADDR     "00:41:71:00:00:55" /* U-Boot */
# define CONFIG_USBNET_HOST_ADDR    "00:41:71:00:00:54" /* Host PC */
#endif

/*
 * LCD
 */
#if defined(CONFIG_SUPP_VGA_CONSOLE) && defined(CONFIG_FTLCDC200_BASE)
# define CONFIG_FTLCDC200
# define CONFIG_FTLCDC200_800X480S_TPO
# define LCD_BPP                    4 /* 16-bit per pixel */
# define CONFIG_LCD
#endif

/*
 * U-Boot General Configurations
 */
#define CONFIG_VERSION_VARIABLE     /* Include version env variable */
#ifndef CONFIG_SYS_LOAD_ADDR        /* Default load address */
# define CONFIG_SYS_LOAD_ADDR       (CONFIG_SYS_SDRAM_BASE + SZ_16M)
#endif
/* Console Baud-Rate Table */
#define CONFIG_SYS_BAUDRATE_TABLE   { 115200, 57600, 38400, 19200, 9600 }
/* Console I/O Buffer Size */
#define CONFIG_SYS_CBSIZE           256
/* Max number of command args */
#define CONFIG_SYS_MAXARGS          32
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE \
	(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)

/*
 * Shell
 */
#define CONFIG_SYS_PROMPT           "=> "
/*
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2  "$ "
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMDLINE_EDITING
*/

/*
 * Linux kernel command line options
 */
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_TAG /* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS

/*
 * Default Commands
 */
#endif
