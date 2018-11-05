/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "drivers_config.h"
#include "cdl_gic.h"

#include "interrupt.h"
#include "console.h"

/* each vector locate at fixed position in hander table*/
struct irq_generic_handler_entry
irq_generic_handler_table[BSP_INTERRUPT_VECTOR_NUMBER];

static void irq_generic_empty_handler(void* arg)
{
	u32 vector = (u32)arg;
	info("empty handler,vector=%d\n", vector);
}

static bool irq_generic_is_valid_vector(u32 vector)
{
	return	BSP_INTERRUPT_VECTOR_MIN <= vector &&
	        vector <= BSP_INTERRUPT_VECTOR_MAX;
}

static void irq_generic_dispatch_handler(u32 vector)
{
	struct irq_generic_handler_entry* e;
	u32 index;

	if (!irq_generic_is_valid_vector(vector)) {
		return;
	}

	index = vector - BSP_INTERRUPT_VECTOR_MIN;
	e = &irq_generic_handler_table[index];
	(*e->handler)(e->arg);
}

/*register a interrupt handler*/
int irq_generic_install_handler(unsigned long vector,
        irq_generic_handler_fn handler, void* arg)
{
	u32 index;

	if (handler == NULL) {
		return 1;
	} else if (!irq_generic_is_valid_vector(vector)) {
        	return 2;
    }

	index = vector - BSP_INTERRUPT_VECTOR_MIN;
	irq_generic_handler_table[index].handler = handler;
	irq_generic_handler_table[index].arg = arg;

	return 0;
}

/*remove a interrupt handler,replace with default one*/
int irq_generic_remove_handler(unsigned long vector,
        irq_generic_handler_fn handler, void* arg)
{
	u32 index;

	if (!irq_generic_is_valid_vector(vector)) {
		return 2;
	}

	index = vector - BSP_INTERRUPT_VECTOR_MIN;
	irq_generic_handler_table[index].handler = irq_generic_empty_handler;
	irq_generic_handler_table[index].arg = (void*)(vector);

	return 0;
}

void irq_generic_enable_int(unsigned long vector)
{
	if (irq_generic_is_valid_vector(vector)) {
		gic_enable_interrupt(vector);
	}
}

void irq_generic_disable_int(unsigned long vector)
{
	if (irq_generic_is_valid_vector(vector)) {
		gic_disable_interrupt(vector);
	}
}
void irq_generic_clear_int(unsigned long vector)
{
	if (irq_generic_is_valid_vector(vector)) {
		gic_clear_interrupt(vector);
	}
}
int irq_generic_config_int(unsigned long vector, int type)
{
	return gic_configure_interrupt(vector, type);
}

void irq_generic_init(unsigned long base,unsigned long cpubase)
{
	u32 index;

	gic_init(base,cpubase,BSP_INTERRUPT_VECTOR_MIN,BSP_INTERRUPT_VECTOR_MAX);

	/* fill handler table with default empty handler */
	for (index = 0; index < BSP_INTERRUPT_VECTOR_NUMBER; index++) {
		irq_generic_handler_table[index].handler =
		irq_generic_empty_handler;
		irq_generic_handler_table[index].arg = (void*)(index +
		        BSP_INTERRUPT_VECTOR_MIN);
	}
}

void irq_generic_handler(void)
{
	gic_dispatch_interrupt(irq_generic_dispatch_handler);
}

/* enable IRQ interrupts */
void enable_interrupts (void)
{
	unsigned long temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "bic %0, %0, #0x80\n"
			     "msr cpsr_c, %0"
			     : "=r" (temp)
			     :
			     : "memory");
}


/*
 * disable IRQ/FIQ interrupts
 * returns true if interrupts had been enabled before we disabled them
 */
int disable_interrupts (void)
{
	unsigned long old,temp;
	__asm__ __volatile__("mrs %0, cpsr\n"
			     "orr %1, %0, #0xc0\n"
			     "msr cpsr_c, %1"
			     : "=r" (old), "=r" (temp)
			     :
			     : "memory");
	return (old & 0x80) == 0;
}
