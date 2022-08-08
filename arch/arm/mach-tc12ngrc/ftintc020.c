#include <common.h> 
#include <asm/io.h> 
#include <asm/arch/hardware.h>
#include "ftintc020.h" 

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
#define NR_IRQS    64

static struct {
	void  *data;
	void (*func)(void *data);
} irq_hndl[NR_IRQS];

void __iomem *ftintc020_base = (void *)(PLATFORM_FTINTC020_BASE); 

/**********************************************************************************************************/
static inline unsigned int ftintc020_register_offset(unsigned int irq)
{
	return irq < 32 ? 0x00 : 0x60;
}

static void ftintc020_init(u32 *base) 
{
	int nr_irq;
	unsigned int irq;

	PRINTF3("ftintc020_init\n");

	nr_irq = FTINTC020_FEAT_IRQNUM(readl(base + FTINTC020_OFFSET_FEAT));

	for (irq = 0; irq < nr_irq; irq += 32) {
		unsigned int offset = ftintc020_register_offset(irq);

		writel(0, base + FTINTC020_OFFSET_IRQ_EN + offset);
		writel(0, base + FTINTC020_OFFSET_IRQ_MODE + offset);
		writel(0, base + FTINTC020_OFFSET_IRQ_LEVEL + offset);
		writel(~0, base + FTINTC020_OFFSET_IRQ_CLEAR + offset);
	}
} 

void irq_enable(int irq) 
{
	u32 offset;
	u32 mask;

	PRINTF3("irq_enable\n");

	offset = ftintc020_register_offset(irq);
	mask = readl(ftintc020_base + FTINTC020_OFFSET_IRQ_EN + offset);
	mask |= (1 << ((irq - 32) % 32));
	writel(mask, ftintc020_base + FTINTC020_OFFSET_IRQ_EN + offset);
}

void irq_disable(int irq) 
{ 
	u32 offset;
	u32 mask;

	PRINTF3("irq_disable\n");

	offset = ftintc020_register_offset(irq);
	mask = readl(ftintc020_base + FTINTC020_OFFSET_IRQ_EN + offset);
	mask &= ~(1 << ((irq - 32) % 32));
	writel(mask, ftintc020_base + FTINTC020_OFFSET_IRQ_EN + offset);
} 

static void irq_acknowledge(int irq) 
{
	u32 mask = 0;
	u32 offset;

	PRINTF3("irq_acknowledge\n");

	offset = ftintc020_register_offset(irq);
	mask |= 1 << ((irq - 32) % 32);
	writel(mask, ftintc020_base + FTINTC020_OFFSET_IRQ_CLEAR + offset);
} 

void do_irq(struct pt_regs *pt_regs) 
{ 
	unsigned int irq;
	unsigned long status;

	PRINTF3("do_irq\n");

	irq = 32;
	status = readl(ftintc020_base + FTINTC020_OFFSET_EIRQ_STATUS);     /* IRQ 32 ~ 63 */
	if (!status) {
		irq = 0;
		status = readl(ftintc020_base + FTINTC020_OFFSET_IRQ_STATUS);  /* IRQ  0 ~ 31 */
	}
	irq += ffs(status) - 1;

	// spurious interrupt?
	if (irq == -1)
		return;

	if (irq_hndl[irq].func) 
		irq_hndl[irq].func(irq_hndl[irq].data); 
	else 
		printf("Unhandled IRQ = %d\n", irq); 

	irq_acknowledge(irq); 
} 

void irq_install_handler(int irq, interrupt_handler_t *hndl, void *data) 
{
	PRINTF3("irq_install_handler\n");

	irq_hndl[irq].func = hndl; 
	irq_hndl[irq].data = data; 

	irq_enable(irq); 
}

void irq_free_handler(int irq) 
{
	PRINTF3("irq_free_handler\n");

	irq_hndl[irq].func = NULL; 
	irq_hndl[irq].data = NULL; 

	irq_disable(irq); 
} 

int arch_interrupt_init(void) 
{
	PRINTF3("arch_interrupt_init\n");

	ftintc020_init(ftintc020_base);

	return 0; 
}
