/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_gic.h"

struct gic_dev gic;
struct gic_dev* p_gic = &gic;

static u32 gic_vector_to_bit(u32 vector, u32* id, u32* bit)
{
	int tid = 0;
	int tbit = 0;
	u32 vec_ori;

	vec_ori = vector;
	while (vector >= 32) {
		tid ++;
		vector -= 32;
	}
	tbit = vec_ori - tid*32;

	if (id)
		*id = tid;
	if (bit)
		*bit = tbit;

	return 0;
}
static u8 gic_get_cpumask(struct gic_dev* priv)
{
	u32 mask,i;

	for (i = mask = 0; i < 32; i += 4) {
		mask = readl((u32)priv->dist_base + GIC_DIST_TARGET + i);
		mask |= mask >> 16;
		mask |= mask >> 8;
		if (mask) {
			break;
		}
	}

	return mask;
}

static void gic_dist_init(struct gic_dev* priv)
{
	unsigned int i;
	u32 cpumask;

	writel(0, (u32)priv->dist_base + GIC_DIST_CTRL);

	/*
	 * Set all global interrupts to be level triggered, active high.
	 */
	for (i = 32; i < priv->gic_irqs; i += 16) {
		writel(0, (u32)priv->dist_base + GIC_DIST_CONFIG + i * 4 / 16);
	}

	/*
	 * Set all global interrupts to this CPU only.
	 */
	cpumask = gic_get_cpumask(priv);
	cpumask |= cpumask << 8;
	cpumask |= cpumask << 16;
	for (i = 32; i < priv->gic_irqs; i += 4) {
		writel(cpumask,
			(u32)priv->dist_base + GIC_DIST_TARGET + i * 4 / 4);
	}
	/*
	 * Set priority on all global interrupts.
	 */
	for (i = 32; i < priv->gic_irqs; i += 4) {
		writel(0xa0a0a0a0,
			(u32)priv->dist_base + GIC_DIST_PRI + i * 4 / 4);
	}

	/*
	 * Disable all interrupts.Leave the PPI and SGIs alone
	 * as these enables are banked registers.
	 */
	for (i = 32; i < priv->gic_irqs; i += 32) {
		writel(0xffffffff,
			(u32)priv->dist_base + GIC_DIST_ENABLE_CLEAR + i * 4 /
			32);
	}
	writel(1, (u32)priv->dist_base + GIC_DIST_CTRL);
}

static void gic_cpu_init(struct gic_dev* priv)
{
	int i;

	/*
	 * Deal with the banked PPI and SGI interrupts - disable all
	 * PPI interrupts, ensure all SGI interrupts are enabled.
	 */
	writel(0xffff0000, (u32)priv->dist_base + GIC_DIST_ENABLE_CLEAR);
	writel(0x0000ffff, (u32)priv->dist_base + GIC_DIST_ENABLE_SET);

	/*
	 * Set priority on PPI and SGI interrupts
	 */
	for (i = 0; i < 32; i += 4) {
		writel(0xa0a0a0a0,
			(u32)priv->dist_base + GIC_DIST_PRI + i * 4 / 4);
	}
	writel(0xf0, (u32)priv->cpu_base + GIC_CPU_PRIMASK);
	writel(1, (u32)priv->cpu_base + GIC_CPU_CTRL);
}

int gic_init(unsigned long base,unsigned long cpubase,
	unsigned long min,unsigned long max)
{
	int gic_irqs = 0;
	int id;

	memset(p_gic, 0, sizeof(struct gic_dev));

	p_gic->dist_base = (void*)(base);
	p_gic->cpu_base = (void*)(cpubase);

	gic_irqs = (readl((u32)p_gic->dist_base + GIC_DIST_CTR)) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if (gic_irqs > 1020) {
		gic_irqs = 1020;
	}
	p_gic->gic_irqs = gic_irqs;

	p_gic->min_vector = min;
	if (max > p_gic->gic_irqs)
		p_gic->max_vector = p_gic->gic_irqs;
	else
		p_gic->max_vector = max;

	p_gic->min_id = p_gic->min_vector/32;
	p_gic->max_id = p_gic->max_vector/32;

	gic_dist_init(p_gic);
	gic_cpu_init(p_gic);

	/*clear all pending*/
	for (id=p_gic->min_id;id<=p_gic->max_id;id++) {
		writel(0xffffffff,(u32)p_gic->dist_base + GIC_DIST_PENDING_CLEAR + 4*id);
		writel(0xffffffff,(u32)p_gic->dist_base + GIC_DIST_ACTIVE_CLEAR + 4*id);
	}

	return 0;
}

void gic_enable_interrupt(unsigned long vector)
{
	u32 id,bit;
	u32 val;

	gic_vector_to_bit(vector, &id, &bit);

	val = readl((u32)p_gic->dist_base + GIC_DIST_ENABLE_SET + 4*id);
	val |= 1<<bit;
	writel(val,(u32)p_gic->dist_base + GIC_DIST_ENABLE_SET + 4*id);
}

void gic_disable_interrupt(unsigned long vector)
{
	u32 id,bit;
	u32 val;

	gic_vector_to_bit(vector, &id, &bit);

	val = readl((u32)p_gic->dist_base + GIC_DIST_ENABLE_CLEAR + 4*id);
	val |= 1<<bit;
	writel(val,(u32)p_gic->dist_base + GIC_DIST_ENABLE_CLEAR + 4*id);
}

void gic_clear_interrupt(unsigned long vector)
{
	u32 id,bit;
	u32 val;

	gic_vector_to_bit(vector, &id, &bit);

	val = readl((u32)p_gic->dist_base + GIC_DIST_PENDING_CLEAR + 4*id);
	val |= 1<<bit;
	writel(val,(u32)p_gic->dist_base + GIC_DIST_PENDING_CLEAR + 4*id);

	val = readl((u32)p_gic->dist_base + GIC_DIST_ACTIVE_CLEAR + 4*id);
	val |= 1<<bit;
	writel(val,(u32)p_gic->dist_base + GIC_DIST_ACTIVE_CLEAR + 4*id);
}

void gic_dispatch_interrupt(	irq_generic_handler_dispatch_fn fn)
{
#if 1
	int irq_num;
	irq_num = readl((u32)p_gic->cpu_base + GIC_CPU_INTACK);
	irq_num &= 0x1ff;
	gic_clear_interrupt(irq_num);

	fn(irq_num);

	writel(irq_num,(u32)p_gic->cpu_base + GIC_CPU_EOI);
#else
	volatile u32 irq_status,bit,vector;
	u32 pending_set = 0;
	u32 enable_en = 0;
	int id;

	for (id=p_gic->min_id;id<=p_gic->max_id;id++) {
		pending_set = readl((u32)p_gic->dist_base +
			GIC_DIST_PENDING_SET + id*4);
		enable_en = readl((u32)p_gic->dist_base +
			GIC_DIST_ENABLE_SET + id*4);
		irq_status = pending_set & enable_en;

		if (irq_status == 0) {
			continue;
		}
		/*process each interrupt*/
		for (bit = 0; bit < 32; bit++) {
			if (irq_status & (1 << bit)) {
				vector = gic_bit_to_vector(id, bit);
				/* call dispatch to invoke real interrupt handler */
				fn(vector);
				gic_clear_interrupt(vector);
			}
		}
	}
#endif
}

int gic_configure_interrupt(unsigned long vector, int type)
{
	u32 enablemask = 1 << (vector % 32);
	u32 enableoff = (vector / 32) * 4;
	u32 confmask = 0x2 << ((vector % 16) * 2);
	u32 confoff = (vector / 16) * 4;
	bool enabled = false;
	u32 val;

	/* Interrupt configuration for SGIs can't be changed */
	if (vector < 16) {
		return 0;
	}

	if (type != IRQ_TYPE_LEVEL_HIGH && type != IRQ_TYPE_EDGE_RISING) {
		return 0;
	}

	/*
	 * Read current configuration register, and insert the config
	 * for "irq", depending on "type".
	 */
	val = readl((u32)p_gic->dist_base + GIC_DIST_CONFIG + confoff);
	if (type == IRQ_TYPE_LEVEL_HIGH) {
		val &= ~confmask;
	} else if (type == IRQ_TYPE_EDGE_RISING) {
		val |= confmask;
	}

	/*
	 * As recommended by the spec, disable the interrupt before changing
	 * the configuration
	 */
	if (readl((u32)p_gic->dist_base + GIC_DIST_ENABLE_SET + enableoff) &
		enablemask) {
		writel(enablemask,
			(u32)p_gic->dist_base + GIC_DIST_ENABLE_CLEAR +
			enableoff);
		enabled = true;
	}

	/*
	 * Write back the new configuration, and possibly re-enable
	 * the interrupt.
	 */
	writel(val, (u32)p_gic->dist_base + GIC_DIST_CONFIG + confoff);

	if (enabled) {
		writel(enablemask,
			(u32)p_gic->dist_base + GIC_DIST_ENABLE_SET +
			enableoff);
	}

	return 0;
}
