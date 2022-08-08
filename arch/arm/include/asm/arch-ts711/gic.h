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
#define GIC_CPU_CTRL                    0x00 
#define GIC_CPU_PRIMASK                 0x04 
#define GIC_CPU_BINPOINT                0x08 
#define GIC_CPU_INTACK                  0x0c 
#define GIC_CPU_EOI                     0x10 
#define GIC_CPU_RUNNINGPRI              0x14 
#define GIC_CPU_HIGHPRI                 0x18 

#define GIC_DIST_CTRL                   0x000 
#define GIC_DIST_CTR                    0x004 
#define GIC_DIST_ENABLE_SET             0x100 
#define GIC_DIST_ENABLE_CLEAR           0x180 
#define GIC_DIST_PENDING_SET            0x200 
#define GIC_DIST_PENDING_CLEAR          0x280 
#define GIC_DIST_ACTIVE_BIT             0x300 
#define GIC_DIST_PRI                    0x400 
#define GIC_DIST_TARGET                 0x800 
#define GIC_DIST_CONFIG                 0xc00 
#define GIC_DIST_SOFTINT                0xf00 

#ifdef CONFIG_USE_IRQ
enum {
	IRQ_TYPE_NONE           = 0x00000000,
	IRQ_TYPE_EDGE_RISING    = 0x00000001,
	IRQ_TYPE_EDGE_FALLING   = 0x00000002,
	IRQ_TYPE_EDGE_BOTH      = (IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING),
	IRQ_TYPE_LEVEL_HIGH     = 0x00000004,
	IRQ_TYPE_LEVEL_LOW      = 0x00000008,
	IRQ_TYPE_LEVEL_MASK     = (IRQ_TYPE_LEVEL_LOW | IRQ_TYPE_LEVEL_HIGH),
	IRQ_TYPE_SENSE_MASK     = 0x0000000f,
};

void irq_install_handler (int irq, interrupt_handler_t handle_irq, void *data);
int arch_interrupt_init(void);

#endif
