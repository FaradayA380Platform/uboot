#include <common.h> 
#include <asm/io.h> 
#include <asm/arch/hardware.h>
#include <faraday/ftintc030.h>

#define NR_IRQS                     80

struct ftintc030_chip_data {
	void __iomem *base;
	int nr_spi;
};

static struct ftintc030_chip_data ftintc030_data;

static struct { 
	void  *data; 
	void (*func)(void *data); 
} irq_hndl[NR_IRQS]; 

/* somehow there is a gap in the SPI registers  */
static inline unsigned int ftintc030_register_offset(unsigned int irq)
{
	return irq < 64 ? (irq / 32) * 32 : (irq / 32) * 32 + 0x20;
}

void ftintc030_raise_softirq(unsigned int cpu, unsigned int irq)
{
	void __iomem *base = ftintc030_data.base;

	writel(((1 << cpu) << 9) | (irq & 0xFF), base + FTINTC030_OFFSET_SGI_CTRL);
}

void ftintc030_irq_set_type(unsigned int irq, unsigned int type)
{
	void __iomem *base = ftintc030_data.base;
	unsigned int level, mode, offset, val;
	
	level = mode = 0;
	
	switch (type) {
	case IRQ_TYPE_LEVEL_LOW:	/* mode = 0, level = 1 */
		level = 1;
		/* fall through */

	case IRQ_TYPE_LEVEL_HIGH:	/* mode = 0, level = 0 */
		break;

	case IRQ_TYPE_EDGE_FALLING:	/* mode = 1, level = 1 */
		level = 1;
		/* fall through */

	case IRQ_TYPE_EDGE_RISING:	/* mode = 1, level = 0 */
		mode = 1;
		break;

	default:
		return;
	}
	
	if (irq < 32) {
		val = readl(base + FTINTC030_OFFSET_PPI_SGI_MODE + offset);
		val &= ~(1 << irq);
		val |= mode << irq;
		writel(val, base + FTINTC030_OFFSET_PPI_SGI_MODE + offset);
		
		val = readl(base + FTINTC030_OFFSET_PPI_SGI_LEVEL + offset);
		val &= ~(1 << irq);
		val |= level << irq;
		writel(val, base + FTINTC030_OFFSET_PPI_SGI_LEVEL + offset);
	} else {
		offset = ftintc030_register_offset(irq - 32);
		val = readl(base + FTINTC030_OFFSET_SPI_MODE + offset);
		val &= ~(1 << (irq % 32));
		val |= mode << (irq % 32);
		writel(val, base + FTINTC030_OFFSET_SPI_MODE + offset);
		
		val = readl(base + FTINTC030_OFFSET_SPI_LEVEL + offset);
		val &= ~(1 << (irq % 32));
		val |= level << (irq % 32);
		writel(val, base + FTINTC030_OFFSET_SPI_LEVEL + offset);
	}
}

static void ftintc030_dist_init(void)
{
	void __iomem *base = ftintc030_data.base;
	int nr_spi = ftintc030_data.nr_spi;
	unsigned int irq;

	/* setup PPIs and SGIs, set all to be level triggered, active high. */
	writel(0, base + FTINTC030_OFFSET_PPI_SGI_EN);
	writel(0, base + FTINTC030_OFFSET_PPI_SGI_MODE);
	writel(0, base + FTINTC030_OFFSET_PPI_SGI_LEVEL);
	writel(~0, base + FTINTC030_OFFSET_PPI_CLEAR);

	/* setup SPIs, set all to be level triggered, active high. */
	for (irq = 0; irq < nr_spi; irq += 32) {
		unsigned int offset = ftintc030_register_offset(irq);

		writel(0, base + FTINTC030_OFFSET_SPI_EN + offset);
		writel(0, base + FTINTC030_OFFSET_SPI_MODE + offset);
		writel(0, base + FTINTC030_OFFSET_SPI_LEVEL + offset);
		writel(~0, base + FTINTC030_OFFSET_SPI_CLEAR + offset);
	}

	/* setup SPI target */
#ifdef FTINTC030_SPI_TARGET_INVERSE
	for (irq = 0; irq < nr_spi; irq += 32)
		writel(~0, base + FTINTC030_OFFSET_SPI_TARGET + (irq / 32) * 4);
#else
	for (irq = 0; irq < nr_spi; irq += 4)
		writel(~0, base + FTINTC030_OFFSET_SPI_TARGET + (irq / 4) * 4);
#endif
	
	/* setup SPI priority */
	for (irq = 0; irq < nr_spi; irq += 4)
		writel(0, base + FTINTC030_OFFSET_SPI_PRIOR + (irq / 4) * 4);
}

static void ftintc030_cpu_init(void)
{ 
	void __iomem *base = ftintc030_data.base;

	writel(0x4, base + FTINTC030_OFFSET_INTCR);
	writel(0x7, base + FTINTC030_OFFSET_BINPOINT);
	writel(0xf, base + FTINTC030_OFFSET_PRIMASK);
} 

void irq_raise_softirq(unsigned int cpu, unsigned int irq)
{
	ftintc030_raise_softirq(cpu, irq);
}

void irq_set_type(unsigned int irq, unsigned int type)
{
	ftintc030_irq_set_type(irq, type);
}

void irq_enable(int irq) 
{
	void __iomem *base = ftintc030_data.base;
	unsigned int offset, mask;

	if (irq < 32) {
		mask = readl(base + FTINTC030_OFFSET_PPI_SGI_EN);
		mask |= (1 << (irq % 32));
		writel(mask, base + FTINTC030_OFFSET_PPI_SGI_EN);
	} else {
		offset = ftintc030_register_offset(irq - 32);
		mask = readl(base + FTINTC030_OFFSET_SPI_EN + offset);
		mask |= 1 << ((irq - 32) % 32);
		writel(mask, base + FTINTC030_OFFSET_SPI_EN + offset);
	}
}

void irq_disable(int irq) 
{ 
	void __iomem *base = ftintc030_data.base;
	unsigned int offset, mask;

	if (irq < 32) {
		mask = readl(base + FTINTC030_OFFSET_PPI_SGI_EN);
		mask &= ~(1 << (irq % 32));
		writel(mask, base + FTINTC030_OFFSET_PPI_SGI_EN);
	} else {
		offset = ftintc030_register_offset(irq - 32);
		mask = readl(base + FTINTC030_OFFSET_SPI_EN + offset);
		mask &= ~(1 << ((irq - 32) % 32));
		writel(mask, base + FTINTC030_OFFSET_SPI_EN + offset);
	}
} 

static void irq_acknowledge(int irq) 
{
	void __iomem *base = ftintc030_data.base;
	unsigned int offset, mask;

	if (irq < 32) {
		mask |= 1 << (irq % 32);
		writel(mask, base + FTINTC030_OFFSET_PPI_CLEAR);
	} else {
		offset = ftintc030_register_offset(irq - 32);
		mask |= 1 << ((irq - 32) % 32);
		writel(mask, base + FTINTC030_OFFSET_SPI_CLEAR + offset);
	}

	writel(irq, base + FTINTC030_OFFSET_EOI);
} 

void do_irq(struct pt_regs *pt_regs) 
{
	void __iomem *base = ftintc030_data.base;
	unsigned int irq;
	unsigned long status;

	status = readl(base + FTINTC030_OFFSET_ACK);

	// spurious interrupt? 
	irq = status & 0x1ff;
	if (irq == 511)
		return;

	if (irq_hndl[irq].func) 
		irq_hndl[irq].func(irq_hndl[irq].data); 
	else 
		printf("Unhandled IRQ = %d\n", irq); 

	irq_acknowledge(irq); 
} 

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

int arch_interrupt_init(void) 
{
	void __iomem *base = (void __iomem *)PLATFORM_FTINTC030_BASE;

	ftintc030_data.base = base;
	ftintc030_data.nr_spi = FTINTC030_FEAT1_SPINUM(readl(base + FTINTC030_OFFSET_FEAT1));

	ftintc030_dist_init();
	ftintc030_cpu_init();

	return 0; 
}