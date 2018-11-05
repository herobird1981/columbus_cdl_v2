/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_GIC_H__
#define __CDL_GIC_H__

/*register offset*/
#define GIC_CPU_CTRL			0x00
#define GIC_CPU_PRIMASK 		0x04
#define GIC_CPU_BINPOINT		0x08
#define GIC_CPU_INTACK			0x0c
#define GIC_CPU_EOI 			0x10
#define GIC_CPU_RUNNINGPRI		0x14
#define GIC_CPU_HIGHPRI 		0x18
#define GIC_CPU_ALIAS_BINPOINT		0x1c
#define GIC_CPU_ACTIVEPRIO		0xd0
#define GIC_CPU_IDENT 			0xfc

#define GIC_DIST_CTRL 			0x000
#define GIC_DIST_CTR			0x004
#define GIC_DIST_IGROUP 		0x080
#define GIC_DIST_ENABLE_SET 		0x100
#define GIC_DIST_ENABLE_CLEAR 		0x180
#define GIC_DIST_PENDING_SET		0x200
#define GIC_DIST_PENDING_CLEAR  	0x280
#define GIC_DIST_ACTIVE_SET 		0x300
#define GIC_DIST_ACTIVE_CLEAR 		0x380
#define GIC_DIST_PRI			0x400
#define GIC_DIST_TARGET 		0x800
#define GIC_DIST_CONFIG 		0xc00
#define GIC_DIST_SOFTINT		0xf00
#define GIC_DIST_SGI_PENDING_CLEAR	0xf10
#define GIC_DIST_SGI_PENDING_SET	0xf20

#define IRQ_TYPE_NONE 		0x00000000	/* Default, unspecified type */
#define IRQ_TYPE_EDGE_RISING	0x00000001	/* Edge rising type */
#define IRQ_TYPE_EDGE_FALLING	0x00000002	/* Edge falling type */
#define IRQ_TYPE_EDGE_BOTH	(IRQ_TYPE_EDGE_FALLING | IRQ_TYPE_EDGE_RISING)
#define IRQ_TYPE_LEVEL_HIGH	0x00000004	/* Level high type */
#define IRQ_TYPE_LEVEL_LOW	0x00000008	/* Level low type */
#define IRQ_TYPE_SENSE_MASK	0x0000000f	/* Mask of the above */
#define IRQ_TYPE_PROBE		0x00000010	/* Probing in progress */

#define gic_bit_to_vector(id,bit)	(id*32+bit)

struct gic_dev {
	void* dist_base;
	void* cpu_base;

	int gic_irqs;
	unsigned long min_vector;
	unsigned long max_vector;
	unsigned long min_id;
	unsigned long max_id;
};

typedef void (*irq_generic_handler_dispatch_fn)(unsigned long);

int gic_init(unsigned long base,unsigned long cpubase,
	unsigned long min,unsigned long max);
void gic_enable_interrupt(unsigned long vector);
void gic_disable_interrupt(unsigned long vector);
void gic_clear_interrupt(unsigned long vector);
void gic_dispatch_interrupt(	irq_generic_handler_dispatch_fn fn);
int gic_configure_interrupt(unsigned long vector, int type);

#endif
