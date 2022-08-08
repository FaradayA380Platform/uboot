#include <common.h> 
#include <asm/io.h> 
#include <asm/arch/hardware.h>
#include "gic_v3.h" 
#include <asm/arch/bits.h>

/**********************************************************************************************************/
#define DEBUG_LEVEL 1

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

/**********************************************************************************************************/

#define NR_IRQS    160

typedef void (interrupt_handler_t)(void *);

u32 irq_start; 
void __iomem *gic_cpu_base_addr;
void __iomem *gic_dist_base_addr = GICD_BASE; 

static struct { 
	void  *data; 
	void (*func)(void *data); 
} irq_hndl[NR_IRQS]; 


int gic_v3_find_pending_irq(void) {
	int val;

    val = readl(gic_cpu_base_addr + GICC_AIAR);
	return val;
}


static void gic_dist_init(u32 *base,int irq_start0) 
{ 
} 

void gicd_enable_int(irq_no irq) {

    *REG_GIC_GICD_ISENABLER( (irq / GIC_GICD_ISENABLER_PER_REG) ) =
		1U << ( irq % GIC_GICD_ISENABLER_PER_REG );
}

void irq_enable(int irq) 
{
	gicd_enable_int(irq);
	__asm__ __volatile__("msr DAIFClr, %0\n\t" : : "i" (DAIF_IRQ_BIT)  : "memory");
}

void irq_disable(int irq) 
{ 
	*REG_GIC_GICD_ICENABLER( (irq / GIC_GICD_ICENABLER_PER_REG) ) = 
		1U << ( irq % GIC_GICD_ICENABLER_PER_REG );
} 

static void irq_acknowledge(int irq) 
{	
	//writel(irq, GICC_BASE+ GICC_AEOIR);
	asm volatile("msr S3_0_c12_c12_1 , %0" : : "r" (irq)); 
} 
#if 0
void do_irq(struct pt_regs *pt_regs, unsigned int esr) 
{ 
	uint32_t stat; 

    stat = gic_v3_find_pending_irq();
	//stat = readl(gic_cpu_base_addr + GIC_CPU_INTACK)&0x3ff; 
	//irq  = stat; 
    if (irq_hndl[stat].func) 
		irq_hndl[stat].func(irq_hndl[stat].data); 
	else 
		printf("Unhandled IRQ = %d\n", stat); 
	irq_acknowledge(stat); 
} 
#endif
void irq_install_handler(int irq, interrupt_handler_t *hndl, void *data) 
{
	irq_hndl[irq].func = hndl; 
	irq_hndl[irq].data = data; 
	irq_enable(irq); 
}

void irq_free_handler(int irq) 
{
	irq_hndl[irq].func = NULL; 
	irq_hndl[irq].data = NULL; 
	irq_disable(irq); 
} 

static uint32_t get_periph_base(void)
{
      unsigned long long CBAR_val;
      asm volatile("mrs %0,S3_1_C15_C3_0" : "=r"( CBAR_val) :: "cc");
      CBAR_val &=  0x7FFFFC0000;
	
      return (uint32_t)CBAR_val;		//periph_base is 32 bit,so it can directly use uint32_t to express the value
}

int arch_interrupt_init(void) 
{

  uint64_t icc_sre_el3, scr_el3, icc_sre_el2, icc_sre_el1, icc_pmr_el1, icc_igrpen1_el3, val, sre_el1;
  
  gic_cpu_base_addr = get_periph_base();
  #if 0
 
  //gicd_ctlr egrp1a
  val = readl(0x400000);
  val |= BIT5|BIT0;
  writel(val, 0x400000);
  //wait waker
  while(readl(0x440014)!=0){
	  writel(0,0x440014);
  }
  //set SRE_EL1 SRE=1
  asm volatile("mrs %0, scr_el3" : "=r" (sre_el1));
  sre_el1|= 1;
  asm volatile("msr S3_0_C12_C12_5 , %0" : : "r" (sre_el1)); //icc_sre_el1 |= ICC_SRE_SRE_BIT;  
  val = readl(gic_cpu_base_addr);//ICC_CTLR
  val |= BIT1|BIT6;
  writel(val, gic_cpu_base_addr);
  #endif
  //set edgetrigger
 // writel(2<<28, 0x450c04);
  //daif.i = 0
  
  //set gicr_isenabledr0
  //val = readl(0x450100);
 // writel(val|(0x1<<30), 0x450100);
  //gic_init();    
  //GICD ISNABLE set to 0xffffffff

	return 0; 
}
