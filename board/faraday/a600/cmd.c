#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <linux/delay.h>

#define FINISH              0
#define NOT_FINISH          1
#define PASS                0
#define FAIL                1

#define GPIO_INPUT0_CH      0
#define GPIO_INPUT1_CH      1

#define GPIO_DONE_CH        2
#define GPIO_PASS_FAIL_CH   3
#define GPIO_RESULT0_CH     4
#define GPIO_RESULT1_CH     5

typedef int (*init_fnc_t) (void);

void gpio_output_done(unsigned int result)
{
	if (result == FINISH)
		gpio_direction_output(GPIO_DONE_CH, 1);
	else
		gpio_direction_output(GPIO_DONE_CH, 0);
}

void gpio_output_pass_fail(unsigned int result)
{
	if (result == PASS)
		gpio_direction_output(GPIO_PASS_FAIL_CH, 1);
	else
		gpio_direction_output(GPIO_PASS_FAIL_CH, 0);
}

void gpio_output_result0(unsigned int result)
{
	if (result == 1)
		gpio_direction_output(GPIO_RESULT0_CH, 1);
	else
		gpio_direction_output(GPIO_RESULT0_CH, 0);
}

void gpio_output_result1(unsigned int result)
{
	if (result == 1)
		gpio_direction_output(GPIO_RESULT1_CH, 1);
	else
		gpio_direction_output(GPIO_RESULT1_CH, 0);
}

void gpio_reset_pass_done(void)
{
	gpio_output_done(NOT_FINISH);
	gpio_output_pass_fail(FAIL);
	gpio_output_result0(FAIL);
	gpio_output_result1(FAIL);
}

unsigned int gpio_get_mode(void)
{
	unsigned int mode;

	mode = (gpio_get_value(GPIO_INPUT1_CH) << 1) |
	       (gpio_get_value(GPIO_INPUT0_CH) << 0);

	return mode;
}

void gpio_init(void)
{
	//input
	gpio_request(GPIO_INPUT0_CH, "input0");
	gpio_direction_input(GPIO_INPUT0_CH);

	gpio_request(GPIO_INPUT1_CH, "input1");
	gpio_direction_input(GPIO_INPUT1_CH);

	//output
	gpio_request(GPIO_DONE_CH, "done");
	gpio_direction_output(GPIO_DONE_CH, 0);

	gpio_request(GPIO_PASS_FAIL_CH, "pass-fail");
	gpio_direction_output(GPIO_PASS_FAIL_CH, 0);

	gpio_request(GPIO_RESULT0_CH, "result0");
	gpio_direction_output(GPIO_RESULT0_CH, 0);

	gpio_request(GPIO_RESULT1_CH, "result1");
	gpio_direction_output(GPIO_RESULT1_CH, 0);
}

static int do_ftddr440_qc_function_test(void)
{
	unsigned long addr;
	int ret = 0;

	printf("A600: FTDDR440 function test start...\n");

	// copy test image to sram.
	memcpy((void *)0x28000000, (void *)0x10000000, 0x80000);

	// copy test image to channel_a of dram
	memcpy((void *)0x81000000, (void *)0x28000000, 0x80000);
	// copy test image to channel_b of dram
	memcpy((void *)0xc1000000, (void *)0x28000000, 0x80000);

	// compare image between channel_a of dram and sram
	if (memcmp((void *)0x81000000, (void *)0x28000000, 0x80000)) {
		printf("A600: FTDDR440 function test failed(channel a)\n");
		ret = -1;
	}

	// compare image between channel_b of dram and sram
	if (memcmp((void *)0xC1000000, (void *)0x28000000, 0x80000)) {
		printf("A600: FTDDR440 function test failed(channel b)\n");
		ret = -1;
	}

	addr = 0x00000000;
	while (addr < 0x00001080) {
		printf("0x%08x = 0x%08x\n", PLATFORM_DDRC_BASE + addr, readl(PLATFORM_DDRC_BASE + addr));
		addr += 4;
	}

	addr = 0x00010000;
	while (addr < 0x00011080) {
		printf("0x%08x = 0x%08x\n", PLATFORM_DDRC_BASE + addr, readl(PLATFORM_DDRC_BASE + addr));
		addr += 4;
	}

	printf("A600: FTDDR440 function test finished\n");

	return ret;
}

static int do_ftddr440_qc_loopback_test(void)
{
	printf("A600: FTDDR440 loopback test start...\n");

	writel(0x00002000, PLATFORM_DDRC_BASE + 0x900c4);
	writel(0x0f10205b, PLATFORM_DDRC_BASE + 0x900b0);
	writel(0x02200e38, PLATFORM_DDRC_BASE + 0x900b8);
	writel(0x01306fc4, PLATFORM_DDRC_BASE + 0x900b4);
	writel(0x03306fc4, PLATFORM_DDRC_BASE + 0x900b4);

	writel(0x04100030, PLATFORM_DDRC_BASE + 0x60088);
	writel(0x0c100030, PLATFORM_DDRC_BASE + 0x60088);

	mdelay(2000);

	gpio_output_result0((readl(PLATFORM_DDRC_BASE + 0x90168) >> 31) & 0x1);
	printf("0x2a690168 = 0x%08x\n", readl(PLATFORM_DDRC_BASE + 0x90168));
	gpio_output_result1((readl(PLATFORM_DDRC_BASE + 0x600e4) >> 23) & 0x1);
	printf("0x2a6600e4 = 0x%08x\n", readl(PLATFORM_DDRC_BASE + 0x600e4));

	printf("A600: FTDDR440 loopback test finished\n");

//	writel(0x00100030, PLATFORM_DDRC_BASE + 0x60088);
//	writel(0x00306fc4, PLATFORM_DDRC_BASE + 0x900B4);

	return 0;
}

static int wait(void)
{
	return 0;
}

static init_fnc_t init_sequence[] = { 
	wait,
	do_ftddr440_qc_function_test,
	do_ftddr440_qc_loopback_test,
	wait,
};

int run_list_num(init_fnc_t *init_seq, int num)
{
	init_fnc_t *init_fnc_ptr;
	int i;

	i = 0;
	for (init_fnc_ptr = init_seq; *init_fnc_ptr; ++init_fnc_ptr,i++) {
		if (num == i) {
			if ((*init_fnc_ptr)()) {
				printf("initcall sequence %p failed at call %p\n",  \
				       init_seq, *init_fnc_ptr);
				printf("return -1\n");
				return -1;
			}
		}
	}

	printf("return 0\n");
	return 0;
}

static int do_ftddr440_qc(struct cmd_tbl *cmdtp, int flag, int argc,
		       char *const argv[])
{
	unsigned int gpio_in_value, gpio_in_value_old = 0;
	int ret;

	printf("A600: FTDDR440 QC test start...\n");

	gpio_init();

	gpio_in_value = gpio_get_mode();
	printf("%d\t%d\t\n",                    \
	       gpio_get_value(GPIO_INPUT1_CH),  \
	       gpio_get_value(GPIO_INPUT0_CH));

	while(1)
	{
		if(gpio_in_value != gpio_in_value_old) {
			printf("%d\t%d\t\n",                    \
			       gpio_get_value(GPIO_INPUT1_CH),  \
			       gpio_get_value(GPIO_INPUT0_CH));

			printf("Rtask %d\n", gpio_in_value);

			//clear PASS/FAIL, DONE signal
			gpio_reset_pass_done();

			//update current value to old
			gpio_in_value_old = gpio_in_value;

			//run the entry;
			ret = run_list_num(init_sequence, gpio_in_value);

			//output done first;
			gpio_output_done(FINISH);
			gpio_output_pass_fail(ret);
		}

		mdelay(100);
		gpio_in_value = gpio_get_mode();
	}

	return 0;
}

static struct cmd_tbl cmd_a600_sub[] = {
	U_BOOT_CMD_MKENT(ftddr440, 2, 1, do_ftddr440_qc, "", ""),
};

static int do_a600(struct cmd_tbl *cmdtp, int flag, int argc,
		  char *const argv[])
{
	struct cmd_tbl *c;

	if (argc < 2)
		return CMD_RET_USAGE;

	/* Strip off leading 'a600' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_a600_sub[0], ARRAY_SIZE(cmd_a600_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#ifdef CONFIG_SYS_LONGHELP
static char a600_help_text[] =
	"ftddr440 - FTDDR440 QC\n";
#endif

U_BOOT_CMD(a600, 2, 1, do_a600, "A600 sub-system", a600_help_text);
