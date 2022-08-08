/* Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:    GPL-2.0+ */

#include <common.h>
#include <dm.h>
#include <spl.h>
#include <spi_flash.h>
#include <asm/io.h>
#include <asm/u-boot.h>
#include <nand.h>
#include <fat.h>
#include <version.h>
#include <image.h>
#include <malloc.h>
#include <dm/root.h>
#include <linux/compiler.h>

#include "board.h"

void ddr_init(void)
{
}

#ifdef CONFIG_SPL_FRAMEWORK

void spl_board_init(void)
{
	board_early_init_f();

	preloader_console_init();

	ddr_init();
}

void enter_non_secure(void)
{
    uint32_t    reg;
    reg = 0;

    asm volatile ("mrc p15,0, %0,c1,c1,0" : "=r" (reg));
    asm volatile ("bic %0, %0, #0x4a" : "=r" (reg): "0" (reg));
    asm volatile ("orr %0, %0, #0x31" : "=r" (reg): "0" (reg));
    asm volatile ("mcr p15,0,%0,c1,c1,0" : "=r" (reg));
    asm volatile ("isb");
}

u32 spl_boot_device(void)
{
  printf("spl boot_device\n");
  enter_non_secure();
  printf("should be non-secure\n");
  while(1){
    //you can do what you want..
  }
	return 1;
}

#else

#include <xyzModem.h>

void console_init(void)
{
	gd->baudrate = CONFIG_BAUDRATE;

	serial_init();		/* serial communications setup */

	gd->have_console = 1;

#if CONFIG_IS_ENABLED(BANNER_PRINT)
	puts("\nU-Boot " SPL_TPL_NAME " " PLAIN_VERSION " (" U_BOOT_DATE " - "
	     U_BOOT_TIME " " U_BOOT_TZ ")\n");
#endif
}

static int getcxmodem(void) {
	if (tstc())
		return (getc());
	return -1;
}

static ulong load_serial_ymodem(ulong offset, int mode)
{
	int size;
	int err;
	int res;
	connection_info_t info;
	char ymodemBuf[1024];
	ulong store_addr = ~0;
	ulong addr = 0;

	size = 0;
	info.mode = mode;
	res = xyzModem_stream_open(&info, &err);
	if (!res) {

		while ((res =
			xyzModem_stream_read(ymodemBuf, 1024, &err)) > 0) {
			store_addr = addr + offset;
			size += res;
			addr += res;
			memcpy((char *)(store_addr), ymodemBuf, res);
		}
	} else {
		printf("%s\n", xyzModem_error(err));
	}

	xyzModem_stream_close(&err);
	xyzModem_stream_terminate(false, &getcxmodem);


	flush_cache(offset, ALIGN(size, ARCH_DMA_MINALIGN));

	printf("## Total Size      = 0x%08x = %d Bytes\n", size, size);

	return offset;
}

static int load_spi_norflash(ulong offset)
{
	int err = 0;
	struct spi_flash *flash;

	/*
	 * Load U-Boot image from SPI flash into RAM
	 * In DM mode: defaults speed and mode will be
	 * taken from DT when available
	 */

	flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
				CONFIG_SF_DEFAULT_CS,
				CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		puts("SPI probe failed.\n");
		return -ENODEV;
	}

	/* Load u-boot, mkimage header is 64 bytes. */
	err = spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS, 0x80000,
			     (void *)CONFIG_SYS_LOAD_ADDR);
	if (err) {
		debug("%s: Failed to read from SPI flash (err=%d)\n",
		      __func__, err);
		return err;
	}

	return err;
}

void jump_to_image(void (*image_entry)(void))
{
	image_entry();
}

void board_init_f(ulong dummy)
{
#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	gd->malloc_limit = CONFIG_VAL(SYS_MALLOC_F_LEN);
	gd->malloc_ptr = 0;
#endif

	board_early_init_f();

	console_init();

	timer_init();

	ddr_init();

	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 9) & 0x01) {
#if defined(CONFIG_SPL_YMODEM_SUPPORT)
		case 0:
			printf("BOOT_DEVICE_UART\n");
			load_serial_ymodem(CONFIG_SYS_LOAD_ADDR, xyzModem_xmodem);
			break;
#endif
#if defined(CONFIG_SPL_SPI_SUPPORT) && defined(CONFIG_SPL_SPI_FLASH_SUPPORT)
		case 1:
			printf("BOOT_DEVICE_SPI\n");
			load_spi_norflash(CONFIG_SYS_LOAD_ADDR);
			break;
#endif
		default:
			puts("Unsupported boot mode selected\n");
			hang();
	}

	jump_to_image((void *)CONFIG_SYS_LOAD_ADDR);
}

#endif
