//#include <common.h> 
#include <asm/io.h> 
#include "gic.h" 


/**********************************************************************************************************/
#define DEBUG_LEVEL 0

#if DEBUG_LEVEL > 2
#define PRINTF3(args...)  printf(args)
#else
#define PRINTF3(args...)  do { } while(0)
#endif

#if DEBUG_LEVEL > 1
#define PRINTF2(args...)  printf(args)
#else
#define PRINTF2(args...)  do { } while(0)
#endif

#if DEBUG_LEVEL > 0
#define PRINTF1(args...)  printf(args)
#define PRINTF(args...)   printf(args)
#else
#define PRINTF1(args...)  do { } while(0)
#define PRINTF(args...)   printf(args)
#endif

#define CPU_PERIPHBASE 0x96000000
/**********************************************************************************************************/


#define MAX_GIC_NR 64 
#define NR_IRQS    64 

#if 0
u32 irq_start; 
void __iomem *gic_cpu_base_addr = (void *)(CPU_PERIPHBASE + 0x100); 
void __iomem *gic_dist_base_addr = (void *)(CPU_PERIPHBASE + 0x1000); 
#else
u32 irq_start; 
u32 gic_cpu_base_addr = (u32 )(CPU_PERIPHBASE + 0x100); 
u32 gic_dist_base_addr = (u32 )(CPU_PERIPHBASE + 0x1000); 
#endif

static struct { 
        void  *data; 
        void (*func)(void *data); 
} irq_hndl[64]; 

typedef void (interrupt_handler_t)(void *);

static inline unsigned int gic_irq(unsigned int irq) 
{ 
				PRINTF3("[Rido] gic_irq - return: 0x%x (irq:0x%x - irq_start:0x%x) \n", (irq - irq_start), irq, irq_start);
        return irq - irq_start; 
} 

static void gic_dist_init(u32 *base,int irq_start0) 
{ 
        unsigned int max_irq, i; 
        u32 cpumask = (1<<8)|(1<<16); 
								
        irq_start = (irq_start0 - 1) & ~31; 
				
				PRINTF3("[Rido] irq_start: 0x%x\n", irq_start);
				
        writel(0, gic_dist_base_addr + GIC_DIST_CTRL); 
				PRINTF3("[Rido] GIC_DIST_CTRL: 0x%x\n", readl(gic_dist_base_addr + GIC_DIST_CTRL));
        /* 
         * Find out how many interrupts are supported. 
         */ 
        PRINTF3("[Rido] Find out how many interrupts are supported.\n");
        max_irq = readl(gic_dist_base_addr + GIC_DIST_CTR) & 0x1f; 
        max_irq = (max_irq + 1) * 32; 

        /* 
         * The GIC only supports up to 1020 interrupt sources. 
         * Limit this to either the architected maximum, or the 
         * platform maximum. 
         */ 
#if 0         
        if (max_irq > max(1020, NR_IRQS)) 
                max_irq = max(1020, NR_IRQS); 
#else
				max_irq = 160;
#endif
				PRINTF3("[Rido]max_irq= %d, NR_IRQS = %d\n", max_irq, NR_IRQS);
				PRINTF3("[Rido] Set all global interrupts to be level triggered, active low.\n");
        /* 
         * Set all global interrupts to be level triggered, active low. 
         */ 
        for (i = 32; i < max_irq; i += 16) {     				
                writel(0x0, gic_dist_base_addr + GIC_DIST_CONFIG + i * 4 / 16); 
                PRINTF3("[Rido] GIC_DIST_CONFIG: 0x%x\n", readl(gic_dist_base_addr + GIC_DIST_CONFIG+ i * 4 / 16));
				}

        /* 
         * Set all global interrupts to this CPU only. 
         */ 
        PRINTF3("[Rido] Set all global interrupts to this CPU only.\n");
        for (i = 32; i < max_irq; i += 4){ 
                writel(cpumask, gic_dist_base_addr + GIC_DIST_TARGET + i * 4 / 4); 
                PRINTF3("[Rido] GIC_DIST_TARGET: 0x%x\n", readl(gic_dist_base_addr + GIC_DIST_TARGET+ i * 4 / 4));
				}
        /* 
         * Set priority on all interrupts. 
         */ 

        for (i = 0; i < max_irq; i += 4) {
                writel(0xa0a0a0a0, gic_dist_base_addr + GIC_DIST_PRI + i * 4 / 4); 
				}
        /* 
         * Disable all interrupts. 
         */ 

        for (i = 0; i < max_irq; i += 32) {
                writel(0xffffffff, gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR + i * 4 / 32); 
                PRINTF3("[Rido] GIC_DIST_ENABLE_CLEAR: 0x%x\n", readl(gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR+ i * 4 / 32));
				}
        writel(1, gic_dist_base_addr+ GIC_DIST_CTRL); 

} 

static void gic_cpu_init(u32 *base) 
{ 
				PRINTF3("[Rido] gic_cpu_init \n");
				
        writel(0xf0, gic_cpu_base_addr + GIC_CPU_PRIMASK); 
        writel(1, gic_cpu_base_addr + GIC_CPU_CTRL); 
        
        PRINTF3("[Rido]GIC_CPU_PRIMASK(0x%x): 0x%x, GIC_CPU_CTRL: 0x%x"
         					,readl(gic_cpu_base_addr + GIC_CPU_PRIMASK),readl(gic_cpu_base_addr + GIC_CPU_CTRL)); 
} 

void irq_enable(int irq) 
{ 
        u32 mask = 1 << (irq % 32); 

	if(irq!=59)
		return;
  			PRINTF3("[Rido] irq_enable - irq: 0x%x, mask: 0x%x \n", irq, mask);
    		/* unmask */ 
        writel(mask, gic_dist_base_addr + GIC_DIST_ENABLE_SET + (gic_irq(irq) / 32) * 4); 
        PRINTF3("[Rido]GIC_DIST_ENABLE_SET(0x%x): 0x%x"
        				, (gic_dist_base_addr + GIC_DIST_ENABLE_SET + (gic_irq(irq) / 32) * 4)
       				  ,readl(gic_dist_base_addr + GIC_DIST_ENABLE_SET + (gic_irq(irq) / 32) * 4));    
} 		

void irq_disable(int irq) 
{ 
				PRINTF3("[Rido] irq_disable \n");
				
        u32 mask = 1 << (irq % 32); 
    /* mask */ 
        writel(mask, gic_dist_base_addr + GIC_DIST_ENABLE_CLEAR + (gic_irq(irq) / 32) * 4); 
} 

static void irq_acknowledge(int irq) 
{ 			
				PRINTF3("[Rido] irq_acknowledge \n");
				
        writel(gic_irq(irq),gic_cpu_base_addr + GIC_CPU_EOI); 
} 

#if 0
void do_irq(struct pt_regs *pt_regs) 
{ 
        int irq; 
        uint32_t stat; 
    		
    		PRINTF3("[Rido] do_irq \n");
    		    
        stat = readl(gic_cpu_base_addr + GIC_CPU_INTACK)&0x3ff; 
        irq  = stat; 
        if (stat == 0x1ff || stat >= 64 ) { 
                stat  = 0; 
        } 
        if (irq_hndl[stat].func) 
                irq_hndl[stat].func(irq_hndl[stat].data); 
        else 
                printf("Unhandled IRQ = %d\n", irq); 
    irq_acknowledge(irq); 
} 
#endif

void irq_install_handler(int irq, interrupt_handler_t *hndl, void *data) 
{ 			
				PRINTF3("[Rido] irq_install_handler \n");
				
        if (irq >= 0 && irq < 64) { 
                irq_hndl[irq].func = hndl; 
                irq_hndl[irq].data = data; 
                irq_enable(irq); 
        } 
} 

void irq_free_handler(int irq) 
{ 			
				PRINTF3("[Rido] irq_free_handler \n");
				
        if (irq >= 0 && irq < 64) { 
                irq_hndl[irq].func = NULL; 
                irq_hndl[irq].data = NULL; 
                irq_disable(irq); 
        } 
} 

int arch_interrupt_init(void) 
{ 			
				PRINTF3("[Rido] arch_interrupt_init - gic_dist_base_addr:0x%x, gic_cpu_base_addr:0x%x \n", 
				        gic_dist_base_addr, gic_cpu_base_addr);
				
        gic_dist_init(gic_dist_base_addr,29); 
        gic_cpu_init(gic_cpu_base_addr);         
        return 0; 
} 
