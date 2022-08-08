/* Copyright 2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:    GPL-2.0+ */

#include <common.h>
#include <cpu_func.h>
#include <dm.h>
#include <dm/root.h>
#include <hang.h>
#include <image.h>
#include <init.h>
#include <serial.h>
#include <spl.h>
#include <spi_flash.h>
#include <version.h>
#include <malloc.h>

#include <asm/cache.h>
#include <asm/io.h>
#include <asm/u-boot.h>

#include <linux/compiler.h>
#include <linux/delay.h>
//#include "dwc_ddrphy_phyinit_out_ddr4_train1d.h"

//#include "board.h"

void platform_init(void)
{

}

#define CONFIG_PART 1



#ifdef CONFIG_SPL_FRAMEWORK

void spl_board_init(void)
{
	board_early_init_f();

	platform_init();

	preloader_console_init();

	ddr_init();
}

u32 spl_boot_device(void)
{
	u32 mode;

	printf("STRP PIN:%lx\n",readl(PLATFORM_SYSC_BASE + 0x18));
	switch ((readl(PLATFORM_SYSC_BASE + 0x18) >> 12) & 0x03) {
		case 0:
			printf("BOOT_DEVICE_UART\n");
			mode = BOOT_DEVICE_UART;
			break;
		case 1:
			printf("BOOT_DEVICE_SPI\n");
			mode = BOOT_DEVICE_SPI;
			break;
		case 2:
			printf("BOOT_DEVICE_SPI NAND not support\n");
//			mode = BOOT_DEVICE_SPI;
			hang();
		case 3:
			printf("BOOT_DEVICE_NAND not support\n");
//			mode = BOOT_DEVICE_NAND;
			hang();
		default:
			puts("Unsupported boot mode selected\n");
			hang();
	}

	return mode;
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
#if 0
static int getcxmodem(void)
{
	if (tstc())
		return (getchar());
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

//			printf("ymodemBuf = %lx\n",ymodemBuf);
//			printf("store_addr = %lx\n",store_addr);

	res = xyzModem_stream_open(&info, &err);
	if (!res) {

		while ((res =
			xyzModem_stream_read(ymodemBuf, 1024, &err)) > 0) {
			store_addr = addr + offset;
			size += res;
			addr += res;
//			printf("ymodemBuf = %lx\n",ymodemBuf);
			printf("store_addr = %lx\n",store_addr);
			memcpy((char *)(store_addr), ymodemBuf, res);
		}
	} else {
		printf("%s\n", xyzModem_error(err));
	}

			printf("111");
	xyzModem_stream_close(&err);
			printf("222");
	xyzModem_stream_terminate(false, &getcxmodem);
			printf("333");

	flush_cache(offset, ALIGN(size, ARCH_DMA_MINALIGN));
			printf("444");
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
		printf("SPI probe failed.\n");
		return -ENODEV;
	}

	/* Load u-boot, mkimage header is 64 bytes. */
	err = spi_flash_read(flash, CONFIG_SYS_SPI_U_BOOT_OFFS, 0x80000,
			     (void *)CONFIG_SYS_LOAD_ADDR);
	if (err) {
		printf("%s: Failed to read from SPI flash (err=%d)\n",
		      __func__, err);
		return err;
	}

	return err;
}

void jump_to_image(void (*image_entry)(void))
{
	image_entry();
}
#endif

void board_init_f(ulong dummy)
{
	int val;

#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	gd->malloc_limit = CONFIG_VAL(SYS_MALLOC_F_LEN);
	gd->malloc_ptr = 0;
#endif

	board_early_init_f();

//	platform_init();

	console_init();
	printf("console init done\n");


	ddr_init();

	while(1);
	
//	val=1548000;
//	    asm volatile("msr cntfrq_el0 , %0": : "r" (val) : "cc");
//	timer_init();

//	tzc_init();

//	ddr_init();

}

#endif
