/*
 * board/faraday/leo/scu.c 
 *
 * (C) Copyright 2020 Faraday Technology
 * Kay Hsu <kayhsu@faraday-tech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

#define DRAM_SPACE 0x88000000

#define SYS_UART_BASE   FTUART010_0_BASE
#define UART_LSR        (SYS_UART_BASE + 0x14)        /* In:  Line Status Register */ 
#define UART_LSR_TEMT   0x40                          /* Transmitter empty */ 
#define UART_LSR_THRE   0x20                          /* Transmit-hold-register empty */ 
#define UART_FCR        (SYS_UART_BASE + 8)           /* Out: FIFO Control Register */ 

#define sw(addr,val,size) writel(val,addr)
#define HSIZE_WORD 4

extern void scu_int_init(void);
extern void st_set_wfi(void);

void src(u32 addr, u32 val, u32 size)
{
	if (readl(addr) != val) {
		printf("Error, addr %x readback %x, should be %x\n", addr, readl(addr), val);
	}
}

void setup_rtc(void)
{
	//clear rtc count register;
	writel(0, FTSCU100_BASE + 0x200);
	writel(0 ,FTSCU100_BASE + 0x204);
	writel(0, FTSCU100_BASE + 0x210);

	//set rtc alarm
	writel(10, FTSCU100_BASE + 0x208);	//alarm for 10 second

	//enable rtc alarm (bit 1)
	//rtc_alarm_enable_ctrl(1);
	writel(readl(FTSCU100_BASE + 0x210) | 0x3, FTSCU100_BASE + 0x210);
}

void scu_softoff_testing(u32 mode)
{
	uint32_t data;
	uint32_t timeout = 0x100;

	/* wait uart tx over */
	while (timeout-- > 0) {
		if (readl(UART_LSR) & UART_LSR_TEMT) {
			break;
		}
		udelay(10);
	}

	//clean all bootup event status
	writel(0xFFFF0001, FTSCU100_BASE + 0x00);

	//enable event
	writel(0x00F60005, FTSCU100_BASE + 0x04);	//only rtc

	//write pwrup/ pwrdn bit
	data = readl(FTSCU100_BASE + 0x08);
	data &= ~0x303;
	data |= 0x102;
	writel(data, FTSCU100_BASE + 0x08);

	//clean PWRBTN_STS status
	writel(0x00000001, FTSCU100_BASE + 0xC0);

	//enable RTC alarm event
	writel(0x0000EFFF, FTSCU100_BASE + 0xC4);

	//program debounce time
	data = readl(FTSCU100_BASE + 0x08);
	writel((data & 0xFFFFFFFF) | 0x0, FTSCU100_BASE + 0x08);

	//set cpu idle checked by WFI
	writel(0x0000000E, FTSCU100_BASE + 0xB4);
	src(FTSCU100_BASE + 0xB4, 0xE, HSIZE_WORD);

	scu_int_init();

	//setup rtc
	setup_rtc();

	//write data to iram for later verification
	writel(0x12121212, DRAM_SPACE + 0x00);
	writel(0x34343434, DRAM_SPACE + 0x04);
	writel(0x56565656, DRAM_SPACE + 0x08);
	writel(0x78787878, DRAM_SPACE + 0x0c);
	writel(0x9a9a9a9a, DRAM_SPACE + 0x10);
	writel(0xbcbcbcbc, DRAM_SPACE + 0x14);
	writel(0xdededede, DRAM_SPACE + 0x18);
	writel(0xfafafafa, DRAM_SPACE + 0x1c);

	//set wake up PAD as input mode
	//set scu_gpio[2]=1'b0 as input mode
	data = readl(FTSCU100_BASE + 0x04);
	writel(data & 0xfffffffb, FTSCU100_BASE + 0x04);

	//setting CLK_EN for sleep mode !!
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x58);
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x68);
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x88);

	// enter sleep, disable DDR selfr_cmd
	writel(0x80000002, FTSCU100_BASE + 0x20);

	st_set_wfi();  
	//system will poweroff, and waked up by RTC alarm
}

void scu_print_00_events(int val)
{
	switch (val&0x000ffff) {
		case 0x100:
			printf("Hardware reset will occur\n");
			break;
		case 0x200:
			printf("Watchdog reset will occur.\n");
			break;
		case 0x400:
			printf("Boot-up from the dormant mode\n");
			break;
		case 0x800:
			printf("Boot up from the power-off mode\n");
			break;
	}

	switch (val&0xffff0000) {
		case 0x10000:
			printf("Boot up from the power button\n");
			break;
		case 0x20000:
			printf("Boot up by RTC alarm\n");
			break;
		case 0x40000:
			printf("Boot up from X_SPI_DCX2 status\n");
			break;
		case 0x80000:
			printf("Boot up from X_SPI_DCX1 status\n");
			break;
		case 0x100000:
			printf("Boot up from X_UART8_RX_h status\n");
			break;
		case 0x200000:
			printf("Boot up from X_UART8_RX status\n");
			break;
		case 0x400000:
			printf("Boot up from X_UART8_TX status\n");
			break;
		case 0x800000:
			printf("Boot up from X_UART9_RX status\n");
			break;
		case 0x1000000:
			printf("Boot up from X_UART7_RX status\n");
			break;
		case 0x2000000:
			printf("Boot up from X_UART7_TX status\n");
			break;
		case 0x4000000:
			printf("Boot up from X_UART9_TX status\n");
			break;
		case 0x8000000:
			printf("Boot up from X_UART6_RX status\n");
			break;
		case 0x10000000:
			printf("Boot up from X_UART6_TX status\n");
			break;
		case 0x20000000:
			printf("Boot up from X_UART5_RX_h status\n");
			break;
		case 0x40000000:
			printf("Boot up from X_UART5_RX status\n");
			break;
		case 0x80000000:
			printf("Boot up from X_UART5_TX status\n");
			break;
	}
}

void scu_print_c0_events(int val)
{
	switch (val) {
		case 0x1:
			printf("Wakeup from sleep by the boot-up events\n");
			break;
		case 0x2:
			printf("Wake up from X_SPI_DCX2 events\n");
			break;
		case 0x4:
			printf("Wake up from X_SPI_DCX1 events\n");
			break;
		case 0x8:
			printf("Wake up from X_UART8_RX_h events\n");
			break;
		case 0x10:
			printf("Wake up from X_UART8_RX events\n");
			break;
		case 0x20:
			printf("Wake up from X_UART8_TX events\n");
			break;
		case 0x40:
			printf("Wake up from X_UART9_RX events\n");
			break;
		case 0x80:
			printf("Wake up from X_UART7_RX events\n");
			break;
		case 0x100:
			printf("Wake up from X_UART7_TX events\n");
			break;
		case 0x200:
			printf("Wake up from X_UART9_TX events\n");
			break;
		case 0x400:
			printf("Wake up from X_UART6_RX events\n");
			break;
		case 0x800:
			printf("Wake up from X_UART6_TX events\n");
			break;
		case 0x1000:
			printf("Wake up from X_UART5_RX_h events\n");
			break;
		case 0x2000:
			printf("Wake up from X_UART5_RX events\n");
			break;
		case 0x4000:
			printf("Wake up from X_UART5_TX events\n");
			break;
	}
}

void scu_sleep_testing(u32 mode)
{
	uint32_t data;
	uint32_t timeout = 0x100;

	/* wait uart tx over */
	while(timeout-- > 0) {
		if (readl(UART_LSR) & UART_LSR_TEMT) {
			break;
		}
		udelay(10);
	}

	//clean all bootup event status
	writel(0xFFFF0001, FTSCU100_BASE + 0x00);

	//enable event
	writel(readl(FTSCU100_BASE + 0x04) | 0xfffc0000, FTSCU100_BASE + 0x04);	//enable all button event

	//clean PWRBTN_STS status
	writel(0x00000001, FTSCU100_BASE + 0xC0);

	//enable RTC alarm event
	writel(0x0000EFFF, FTSCU100_BASE + 0xC4);	//wake up by which button

	//program debounce time
	data = readl(FTSCU100_BASE + 0x08);
	writel((data & 0xFFFFFFFF) | 0x0, FTSCU100_BASE + 0x08);

	//set cpu idle checked by WFI
	writel(0x0000000E, FTSCU100_BASE + 0xB4);
	src(FTSCU100_BASE + 0xB4, 0xE, HSIZE_WORD);

	scu_int_init();

	//write data to iram for later verification
	writel(0x12121212, DRAM_SPACE + 0x00);
	writel(0x34343434, DRAM_SPACE + 0x04);
	writel(0x56565656, DRAM_SPACE + 0x08);
	writel(0x78787878, DRAM_SPACE + 0x0c);
	writel(0x9a9a9a9a, DRAM_SPACE + 0x10);
	writel(0xbcbcbcbc, DRAM_SPACE + 0x14);
	writel(0xdededede, DRAM_SPACE + 0x18);
	writel(0xfafafafa, DRAM_SPACE + 0x1c);

	//set wake up PAD as input mode
	//set scu_gpio[2]=1'b0 as input mode
	data = readl(FTSCU100_BASE + 0x04);
	writel(data & 0xfffffffb, FTSCU100_BASE + 0x04);

	//setting CLK_EN for sleep mode !!
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x58);
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x68);
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x88);

	// enter sleep, disable DDR selfr_cmd
	writel(0x80000008, FTSCU100_BASE + 0x20);

	//printf("__wfi function!");
	st_set_wfi();  

	src(DRAM_SPACE + 0x00, 0x12121212, HSIZE_WORD);
	src(DRAM_SPACE + 0x04, 0x34343434, HSIZE_WORD);
	src(DRAM_SPACE + 0x08, 0x56565656, HSIZE_WORD);
	src(DRAM_SPACE + 0x0c, 0x78787878, HSIZE_WORD);
	src(DRAM_SPACE + 0x10, 0x9a9a9a9a, HSIZE_WORD);
	src(DRAM_SPACE + 0x14, 0xbcbcbcbc, HSIZE_WORD);
	src(DRAM_SPACE + 0x18, 0xdededede, HSIZE_WORD);
	src(DRAM_SPACE + 0x1c, 0xfafafafa, HSIZE_WORD);

	scu_print_00_events(readl(FTSCU100_BASE + 0x00));
	scu_print_c0_events(readl(FTSCU100_BASE + 0xc0));
}

void scu_ddr_retein_testing(u32 mode)
{
	uint32_t data;
	uint32_t timeout = 0x100;

	/* wait uart tx over */
	while (timeout-- > 0) {
		if (readl(UART_LSR) & UART_LSR_TEMT) {
			break;
		}
		udelay(10);
	}

	//clean all bootup event status
	writel(0xFFFF0001, FTSCU100_BASE + 0x00);

	//enable event
	writel(0x00F60005, FTSCU100_BASE + 0x04);	//only rtc

	//write pwrup/ pwrdn bit
	data = readl(FTSCU100_BASE + 0x08);
	data &= ~0x303;
	data |= 0x102;
	writel(data, FTSCU100_BASE + 0x08);

	//clean PWRBTN_STS status
	writel(0x00000001, FTSCU100_BASE + 0xC0);

	//enable RTC alarm event
	writel(0x0000EFFF, FTSCU100_BASE + 0xC4);

	//program debounce time
	data = readl(FTSCU100_BASE + 0x08);
	writel((data & 0xFFFFFFFF) | 0x0, FTSCU100_BASE + 0x08);

	//set cpu idle checked by WFI
	writel(0x0000000E, FTSCU100_BASE + 0xB4);
	src(FTSCU100_BASE + 0xB4, 0xE , HSIZE_WORD);

	scu_int_init();

	//setup rtc
	setup_rtc();

	//write data to iram for later verification
	writel(0x12121212, DRAM_SPACE + 0x00);
	writel(0x34343434, DRAM_SPACE + 0x04);
	writel(0x56565656, DRAM_SPACE + 0x08);
	writel(0x78787878, DRAM_SPACE + 0x0c);
	writel(0x9a9a9a9a, DRAM_SPACE + 0x10);
	writel(0xbcbcbcbc, DRAM_SPACE + 0x14);
	writel(0xdededede, DRAM_SPACE + 0x18);
	writel(0xfafafafa, DRAM_SPACE + 0x1c);

	// DDR in self-refresh mode
	data = readl(FTDDR3030_BASE + 0x04);
	writel(0x00000004, FTDDR3030_BASE + 0x04); 
	do {
		// self-refresh status
		data = readl(FTDDR3030_BASE + 0x04);
	} while((data & 0x00000400) == 0);
	printf("DDR is in self-refresh mode!!!");

	//set wake up PAD as input mode
	//set scu_gpio[2]=1'b0 as input mode
	data = readl(FTSCU100_BASE + 0x04);
	writel(data & 0xfffffffb, FTSCU100_BASE + 0x04);

	//setting CLK_EN for sleep mode !!
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x58);
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x68);
	writel(0x5a5a5a5a, FTSCU100_BASE + 0x88);

	// enter sleep, disable DDR selfr_cmd
	writel(0x80000002, FTSCU100_BASE + 0x20);

	st_set_wfi();  
	//system will poweroff, and waked up by RTC alarm
}

void ipc_interrupt(void *arg) 
{
	printf("IPC interrupt\n");

	//send interrupt back to cm33 by writing 0x57f00104
	writel(readl(0x57f00100)&(~0x1), 0x57f00100);	//clear status
	writel(0x00000003, 0x57f00104);
} 

void ipc_int_init(void) 
{
	irq_install_handler(122 + 32, ipc_interrupt, 0);
}

void cti_testing(void)
{
	sw(0x57F00310,  0x00000001, 2);             //Enable semaphore's CTI interrupt enable

	//set ROMADDR/SELFADDR for system
	sw(0x2800000C,  0x10029000, 2);
	sw(0x2800000C,  0x02010000, 2);

	//core 0
	sw(0x29110FB0,  0xC5ACCE55, 2);             //Set Lock access key
	sw(0x29110F00,  0x00000001, 2);
	sw(0x29110EF8,  0x00000001, 2);

	src(0x57F00310, 0x00000003, HSIZE_WORD);    //Check CTI interrupt status
	sw(0x57F00310,  0x00000003, 2);             //Clear CTI interrupt status
	sw(0x29110EF8,  0x00000000, 2);             //Clear DBGACK

	//core 1
	sw(0x29112FB0,  0xC5ACCE55, 2);             //Set Lock access key
	sw(0x29112F00,  0x00000001, 2);
	sw(0x29112EF8,  0x00000001, 2);

	src(0x57F00310, 0x00000003, HSIZE_WORD);    //Check CTI interrupt status
	sw(0x57F00310,  0x00000003, 2);             //Clear CTI interrupt status
	sw(0x29112EF8,  0x00000000, 2);             //Clear DBGACK

	//core 2
	sw(0x29114FB0,  0xC5ACCE55, 2);             //Set Lock access key
	sw(0x29114F00,  0x00000001, 2);
	sw(0x29114EF8,  0x00000001, 2);

	src(0x57F00310, 0x00000003, HSIZE_WORD);    //Check CTI interrupt status
	sw(0x57F00310,  0x00000003, 2);             //Clear CTI interrupt status
	sw(0x29114EF8,  0x00000000, 2);             //Clear DBGACK

	//core 3
	sw(0x29116FB0,  0xC5ACCE55, 2);             //Set Lock access key
	sw(0x29116F00,  0x00000001, 2);
	sw(0x29116EF8,  0x00000001, 2);

	src(0x57F00310, 0x00000003, HSIZE_WORD);    //Check CTI interrupt status
	sw(0x57F00310,  0x00000003, 2);             //Clear CTI interrupt status
	sw(0x29116EF8,  0x00000000, 2);             //Clear DBGACK

	printf("CA7 CTI CM33 Completed !!");
}

void ipc_testing(void)
{
	ipc_int_init();

	while(1);
}