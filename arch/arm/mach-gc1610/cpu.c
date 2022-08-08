/*
 * (C) Copyright 2012 Faraday Technology
 * Bing-Yao Luo <bjluo@faraday-tech.com>
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

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <asm/system.h>
#include <asm/armv8/mmu.h>

#include <asm/arch/hardware.h>
#include <asm/global_data.h>


#define CCSIDR_LINE_SIZE_MASK 7
#define CCSIDR_LINE_SIZE_OFFSET 0

#define CLIDR_CTYPE_INSTRUCTION_ONLY	1
#define CLIDR_CTYPE_DATA_ONLY		2
#define CLIDR_CTYPE_INSTRUCTION_DATA	3
#define CLIDR_CTYPE_UNIFIED		4

#ifdef CONFIG_ARCH_EARLY_INIT_R
unsigned int io_table[] = { FTUART010_0_BASE,
//			    CONFIG_FTTMR010_BASE,
			    SCU_BASE,
//			    CONFIG_FTWDT010_BASE,
			    };

#define NUM_IOS (sizeof(io_table)/sizeof(unsigned int))
#endif

static struct mm_region gc1610_mem_map[] = {
	{
		/* ROM regions */
		.phys = 0x0UL,
		.virt = 0x0UL,
		.size = 0x4000000UL,	/* 64MiB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
		         PTE_BLOCK_OUTER_SHARE | PTE_BLOCK_NS
	},
	{
		/* DDR regions0 */
		.phys = 0x80000000,
		.virt = 0x80000000,
		.size = 0x10000000UL,	/* 128MB internal registers */
//		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE |PTE_BLOCK_NS
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_INNER_SHARE | PTE_BLOCK_NS
//		.attrs = 0x611
	},
	{
		/* DDR regions1 */
		.phys = 0x81000000,
		.virt = 0x81000000,
		.size = 0x70000000UL,	/* 786MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},	
	{
		/* IO regions */
		.phys = 0x20700000,
		.virt = 0x20700000,
		.size = 0x00010000UL,	/* 64MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 0x22100000,
		.virt = 0x22100000,
		.size = 0x00010000UL,	/* 64MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 0x40000000,
		.virt = 0x40000000,
		.size = 0x00400000UL,	/* 64MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 0x41000000,
		.virt = 0x41000000,
		.size = 0x00100000UL,	/* 64MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 	0x400000,
		.virt = 	0x400000,
		.size = 	0x100000UL,	/* 768MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 	0x440000,
		.virt = 	0x440000,
		.size = 	0x100000UL,	/* 768MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 0x08000000,
		.virt = 0x08000000,
		.size = 0x04000000UL,	/* 64MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		/* IO regions */
		.phys = 0x10000000,
		.virt = 0x10000000,
		.size = 0x30000000UL,	/* 768MB internal registers */
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) | PTE_BLOCK_NON_SHARE
	},
	{
		0,
	}
};
struct mm_region *mem_map = gc1610_mem_map;


int arch_cpu_init(void)
{
	return 0;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	char cpu_name[100];
	u32 cpu_id, cpu_reg;
	u32 imp_code, line_len, level, cache_type, level_start_bit;


	asm volatile (
		"MRS %0, MIDR_EL1\n"
		: "=r"(cpu_id)/* output */
		:	    /* input */
	);

	imp_code = cpu_id >> 24;
	if (imp_code == 0x66) { /* Faraday */
		switch (cpu_id >> 16) {
		case 0x6605:
			sprintf(cpu_name, "FA%xTE",  (unsigned int)(cpu_id >> 4)  & 0xFFF);
			break;
		}
	} else {
		sprintf(cpu_name, "Main ID register (0x%08x)", cpu_id);
	}

	/* Read current CP15 Cache Size ID Register */
	asm volatile ("MRS %0, CCSIDR_EL1" : "=r" (cpu_reg));
	line_len = ((cpu_reg & CCSIDR_LINE_SIZE_MASK) >>
			CCSIDR_LINE_SIZE_OFFSET) + 2;

	/* converting from log2(linelen) to linelen */
	line_len = 1 << line_len;

	/*convert cache size word to cache size word to byte*/
	line_len <<= 2;

	/* print cpuinfo */
	printf("CPU:   %s %u MHz (Cache size= %u Bytes)\n",
		cpu_name, (unsigned int)(clk_get_rate("CPU") / 1000000),
		(unsigned int)line_len);

	level_start_bit = 0;
	asm volatile ("MRS %0, CLIDR_EL1" : "=r" (cpu_reg));
	for (level = 0; level < 3; level++) {
		cache_type = (cpu_reg >> level_start_bit) & 0x7;
		if (cache_type == CLIDR_CTYPE_DATA_ONLY)
			printf("Cache Level %d type: Data only\n", level);
		else if (cache_type == CLIDR_CTYPE_INSTRUCTION_ONLY)
			printf("Cache Level %d type: Instruction only\n", level);
		else if (cache_type == CLIDR_CTYPE_INSTRUCTION_DATA)
			printf("Cache Level %d type: Instruction and Data\n", level);
		else if (cache_type == CLIDR_CTYPE_UNIFIED)
			printf("Cache Level %d type: Unified\n", level);

		level_start_bit += 3;
	}

	printf("AHB:   %u MHz\n", (unsigned int)(clk_get_rate("AHB") / 1000000));
	printf("APB:   %u MHz\n", (unsigned int)(clk_get_rate("APB") / 1000000));

	return 0;
}
#endif	/* #ifdef CONFIG_DISPLAY_CPUINFO */

void mmu_setup(void)
{
#ifndef CONFIG_SYS_ICACHE_OFF
	DECLARE_GLOBAL_DATA_PTR;
	u64 tlb_addr_save = gd->arch.tlb_addr;
	unsigned int el = current_el();
    
	/* Reset the fill ptr */
	gd->arch.tlb_fillptr = tlb_addr_save;

	/* Create normal system page tables */
	setup_pgtables();

	/* Create emergency page tables */
	gd->arch.tlb_addr = gd->arch.tlb_fillptr;
	gd->arch.tlb_emerg = gd->arch.tlb_addr;
//       printf("A500 mmu_setup gd->arch.tlb_addr = 0x%lx\n", gd->arch.tlb_addr);
	setup_pgtables();
	gd->arch.tlb_addr = tlb_addr_save;
//       printf("A500 mmu_setup gd->arch.tlb_addr = 0x%lx, gd->arch.tlb_size = 0x%lx\n", gd->arch.tlb_addr, gd->arch.tlb_size);
	/* flush new MMU table */
	flush_dcache_range(gd->arch.tlb_addr,
			   gd->arch.tlb_addr + gd->arch.tlb_size);

	/* point TTBR to the new table */
	set_ttbr_tcr_mair(el, gd->arch.tlb_addr, get_tcr(el, NULL, NULL),
			  MEMORY_ATTRIBUTES);
	/*
	 * EL3 MMU is already enabled, just need to invalidate TLB to load the
	 * new table. The new table is compatible with the current table, if
	 * MMU somehow walks through the new table before invalidation TLB,
	 * it still works. So we don't need to turn off MMU here.
	 * When EL2 MMU table is created by calling this function, MMU needs
	 * to be enabled.
	 */
	set_sctlr(get_sctlr() | CR_M);
#endif
}

#ifdef CONFIG_ARCH_EARLY_INIT_R
#if !defined(CONFIG_SYS_DCACHE_OFF)
int arch_early_init_r(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int i;
	u32 *page_table = (u32 *)gd->arch.tlb_addr;
	u32 ca;

	dcache_disable();
	printf("Uncaching controller register memory region ...\n");
	for ( i=0; i < NUM_IOS ; i ++) {
		ca = (io_table[i] >> 20);
		page_table[ca] &= ~0xC;
	}
	dcache_enable();
	return 0;
}
#endif
#endif

void reset_cpu(unsigned long ignored)
{
	void __iomem *base = (void *)(PLATFORM_FTWDT011_BASE);
	u32 val;

	writel(0x5555, base + 0x00);       //write REPROG_KEY
	while (readl(base + 0x14) & 0x01); //wait timer disable
	val = readl(base + 0x14) | 0x0004;
	writel(val, base + 0x14);;         //reset enable
	writel(0x0001, base + 0x08);       //write counter reload value
	writel(0xCCCC, base + 0x00);       //write START_KEY
}
