/*
 * (C) Copyright 2013 Faraday Technology
 * Dante Su <dantesu@faraday-tech.com>
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/clock.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

void reset_cpu(ulong addr)
{
	uint32_t base = CPU_PERIPHBASE + 0x600;

	/* reset watchdog */
	writel(0x12345678, base + 0x34);
	writel(0x87654321, base + 0x34);
	writel(0, base + 0x28);
	writel(1, base + 0x2c);
	writel(1, base + 0x30);

	/* activate watchdog reset */
	writel(1, base + 0x20);
	writel(9, base + 0x28);
}

/*
 * This arch_preboot_os() overrides the weak function
 * in "cmd_bootm.c".
 */
void arch_preboot_os(void)
{
	cleanup_before_linux();
}

#define CPUID_VID(x)        (((x) >> 24) & 0xff)
#define CPUID_ISA(x)        (((x) >> 16) & 0xff)
#define CPUID_PID(x)        (((x) >>  4) & 0x0fff)
#define CPUID_REV(x)        ((x) & 0x0f) /* revision */
#define CPUID_NOREV(x)      (((x) >>  4) & 0x0fffffff)

/* Vendor ID */
#define CPUID_VID_ARM       0x41
#define CPUID_VID_FARADAY   0x66

/* Instruction Set Architecture */
#define CPUID_ISA_ARMV4     0x01
#define CPUID_ISA_ARMV5TE   0x05
#define CPUID_ISA_ARMV5TEJ  0x06

/* Faraday ARMv4 cores */
#define CPUID_FA526         0x6601526
#define CPUID_FA626         0x6601626

/* Faraday ARMv5TE cores */
#define CPUID_FA606TE       0x6605606
#define CPUID_FA616TE       0x6605616
#define CPUID_FA626TE       0x6605626
#define CPUID_FA726TE       0x6605726

#ifdef CONFIG_ARCH_CPU_INIT

int arch_cpu_init(void)
{
	unsigned int id;

	__asm__ __volatile__ (
		"mrc p15, 0, %0, c0, c0, 0\n"
		: "=r"(id) /* output */
		: /* input */
	);

	gd->arch.cpu_id = id;
	gd->arch.cpu_mmu = 1;

	return 0;
}

#endif    /* #ifdef CONFIG_ARCH_CPU_INIT */

#ifdef CONFIG_DISPLAY_CPUINFO

int print_cpuinfo(void)
{
	char cpu_name[32];
	uint vid = CPUID_VID(gd->arch.cpu_id);
	uint pid = CPUID_PID(gd->arch.cpu_id);

	/* build cpu_name */
	switch (vid) {
	case CPUID_VID_FARADAY:	/* Faraday */
		switch (CPUID_ISA(gd->arch.cpu_id)) {
		case CPUID_ISA_ARMV5TE:
			sprintf(cpu_name, "FA%xTE", pid);
			break;
		default:
			sprintf(cpu_name, "FA%x", pid);
			break;
		}
		break;
	case CPUID_VID_ARM:	/* ARM */
		if ((pid & 0xff0) == 0xc00)
			sprintf(cpu_name, "Cortex-A%u", (pid & 0x00f));
		else if (pid >= 0xa00)
			sprintf(cpu_name, "ARM%x", 0x1000 + (pid - 0xa00));
		else
			sprintf(cpu_name, "ARM%x", pid);
		break;
	default:
		sprintf(cpu_name, "Unknown");
		break;
	}

	/* print cpu_info */
	printf("CPU:   %s %u MHz\n",
		cpu_name, (unsigned int)(clock_get_rate(CPU_CLK) / 1000000));

	printf("AHB:   %u MHz\n",
		(unsigned int)(clock_get_rate(AHB_CLK) / 1000000));

	printf("APB:   %u MHz\n",
		(unsigned int)(clock_get_rate(APB_CLK) / 1000000));

	return 0;
}
#endif /* CONFIG_DISPLAY_CPUINFO */

#ifndef CONFIG_SYS_DCACHE_OFF

void enable_caches(void)
{
	/* Enable D-cache. I-cache is already enabled in start.S */
	if (gd->arch.cpu_mmu) {
		puts("MMU:   on\n");
		dcache_enable();
	} else {
		puts("MMU:   off\n");
	}
}

#endif /* !CONFIG_SYS_DCACHE_OFF */
