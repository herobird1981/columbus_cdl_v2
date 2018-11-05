/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#define BSP_INTERRUPT_VECTOR_MIN	0
#ifdef  TAPEOUT_V2
#define BSP_INTERRUPT_VECTOR_MAX	113
#else
#define BSP_INTERRUPT_VECTOR_MAX	96
#endif
#define BSP_INTERRUPT_VECTOR_NUMBER 	(BSP_INTERRUPT_VECTOR_MAX - BSP_INTERRUPT_VECTOR_MIN + 1)

typedef void (*irq_generic_handler_fn)(void*);

struct irq_generic_handler_entry {
	irq_generic_handler_fn handler;
	void* arg;
};

/*apis*/
void irq_generic_init(unsigned long base,unsigned long cpubase);
int irq_generic_install_handler(unsigned long vector,
        irq_generic_handler_fn handler, void* arg);
int irq_generic_remove_handler(unsigned long vector,
        irq_generic_handler_fn handler, void* arg);
void irq_generic_enable_int(unsigned long vector);
void irq_generic_disable_int(unsigned long vector);
void irq_generic_clear_int(unsigned long vector);
int irq_generic_config_int(unsigned long vector, int type);

/* irq handler to CPU*/
void irq_generic_handler(void);

/*arm generic IRQ enable/disable*/
void enable_interrupts (void);
int disable_interrupts (void);
#endif
