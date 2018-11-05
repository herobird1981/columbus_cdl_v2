/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "cdl_config.h"
#include "cdl_timer.h"
#include "cdl_sys.h"

struct gtimer gtimer_dev;
struct gtimer* p_gtimer = &gtimer_dev;

struct gtimer_context_registers gtimer_context = { };
struct gtimer_context_registers* p_gtimer_context = &gtimer_context;

u32 gtimer_read(struct gtimer *priv, int id, u32 offset)
{
	if (id >= NUM_TIMER_PORTS)
	{
		return 0;
	}
	
	return __raw_readl((u32)priv->regs + (0x40*id) + offset);
}

void gtimer_write(struct gtimer *priv, int id, u32 offset, u32 val)
{
	if (id >= NUM_TIMER_PORTS)
	{
		return;
	}
	
	__raw_writel(val, (u32)priv->regs + (0x40*id) + offset);
}

void gtimer_init(uint32_t id, unsigned long base)
{	
	memset(p_gtimer, 0, sizeof(struct gtimer));

	p_gtimer->regs = (void*) base;

	// default setting
	p_gtimer->count_mode = TMR_UP;
	p_gtimer->run_mode = TMR_AUTO;
	p_gtimer->mode = TMR_USER;
	p_gtimer->div = TMR_DIV_1;

	gtimer_stop(id);
}

void gtimer_init_1(uint32_t id, unsigned long base, E_TMR_CLK_DIV div)
{
	memset(p_gtimer, 0, sizeof(struct gtimer));

	p_gtimer->regs = (void*) base;

	// default setting
	p_gtimer->count_mode = TMR_UP;
	p_gtimer->run_mode = TMR_AUTO;
	p_gtimer->mode = TMR_USER;
	p_gtimer->div = div;

	gtimer_stop(id);
}

void gtimer_reset(int id)
{
	unsigned long ctrl;
	int i;

	ctrl = gtimer_read(p_gtimer,id,TMR_REG_CTRL);
	gtimer_write(p_gtimer,id,TMR_REG_CTRL,ctrl | 0x8000);
	for (i=0;i<1000;i++) {
		gtimer_read(p_gtimer,id,TMR_REG_CTRL);
	}
	gtimer_write(p_gtimer,id,TMR_REG_CTRL,0x0000);
}

void gtimer_config_default(int id)
{
	u32 ctrl=0;
	int i;

	// reset
	gtimer_write(p_gtimer,id,TMR_REG_CTRL,0x8000);
	for (i=0;i<1000;i++) {
		gtimer_read(p_gtimer,id,TMR_REG_CTRL);
	}
	gtimer_write(p_gtimer,id,TMR_REG_CTRL,0x0000);

	// configure default
	gtimer_write(p_gtimer, id, TMR_REG_LOAD, 0x0);
	gtimer_write(p_gtimer, id, TMR_REG_COUNT, 0x0);
	gtimer_write(p_gtimer, id, TMR_REG_IRQ_EN, 0x0);
	gtimer_write(p_gtimer, id, TMR_REG_IRQ_RAW, 0x7);
	gtimer_write(p_gtimer, id, TMR_REG_IRQ_ST, 0x7);

	ctrl |= p_gtimer->count_mode << 14;
	ctrl |= p_gtimer->run_mode << 12;
	ctrl |= p_gtimer->mode << 11;
#ifdef TAPEOUT_V2
	ctrl |= (p_gtimer->div & 0x7ff)<<TMR_CLK_DIV_SHIFT;
#else
	ctrl |= (p_gtimer->div & 0xf)<<TMR_CLK_DIV_SHIFT;
#endif
	gtimer_write(p_gtimer,id,TMR_REG_CTRL,ctrl);
}

void gtimer_start(int id)
{
	u32 ctrl = gtimer_read(p_gtimer, id, TMR_REG_CTRL);

	ctrl |= 0x1;
	
	gtimer_write(p_gtimer, id, TMR_REG_CTRL, ctrl);
}
void gtimer_stop(int id)
{
	u32 ctrl = gtimer_read(p_gtimer, id, TMR_REG_CTRL);

	ctrl &= ~0x1;
	
	gtimer_write(p_gtimer, id, TMR_REG_CTRL, ctrl);
}

void gtimer_set_mode(int id, int count_mode, int run_mode, int mode)
{
	u32 ctrl;

	ctrl = gtimer_read(p_gtimer,id,TMR_REG_CTRL);
	ctrl &=  ~((1<<14) | (1<<12) | (1<<11));
	if (count_mode)
		ctrl |= 1<<14;
	if (run_mode)
		ctrl |= 1<<12;
	if (mode)
		ctrl |= 1<<11;

	gtimer_write(p_gtimer,id,TMR_REG_CTRL,ctrl);
}

void gtimer_set_div(int id, int div)
{
	u32 ctrl;

	if (id >= NUM_TIMER_PORTS)
	{
		return;
	}
	ctrl = gtimer_read(p_gtimer,id,TMR_REG_CTRL);
#ifdef TAPEOUT_V2
	ctrl &=  ~(0x7ff<<TMR_CLK_DIV_SHIFT);
	ctrl |= div<<TMR_CLK_DIV_SHIFT;
#else
	ctrl &=  ~(0xf<<TMR_CLK_DIV_SHIFT);
	ctrl |= div<<TMR_CLK_DIV_SHIFT;
#endif
	gtimer_write(p_gtimer,id,TMR_REG_CTRL,ctrl);
}

void gtimer_reload(int id)
{
	gtimer_write(p_gtimer, id, TMR_REG_TRIG, 0x1);
}

void gtimer_set_load(int id, unsigned long val)
{
	gtimer_write(p_gtimer, id, TMR_REG_LOAD, val);
}

void gtimer_set_count(int id, unsigned long val)
{
	gtimer_write(p_gtimer, id, TMR_REG_COUNT, val);
}

void gtimer_set_compare(int id, unsigned long val)
{
	gtimer_write(p_gtimer, id, TMR_REG_COMPARE, val);
}

unsigned long gtimer_get_count(int id)
{
	return gtimer_read(p_gtimer, id, TMR_REG_COUNT);
}

void gtimer_clear_raw_status(int id,unsigned long val)
{
	gtimer_write( p_gtimer, id, TMR_REG_IRQ_RAW,val);
}

// capture mode
void gtimer_set_capture_mode(int id, E_TMR_CAP_MODE cap_mode, E_TMR_CAP_EVENT cap_event)
{
	u32 ctrl;

	ctrl = gtimer_read(p_gtimer, id, TMR_REG_CTRL);

	// capture mode
	ctrl &=  ~(1<<TMR_CAP_MODE_SHIFT);
	ctrl |= cap_mode<<TMR_CAP_MODE_SHIFT;

	// capture event
	ctrl &=  ~(3<<TMR_CAP_EVENT_SHIFT);
	ctrl |= cap_event<<TMR_CAP_EVENT_SHIFT;
	
	gtimer_write(p_gtimer, id, TMR_REG_CTRL, ctrl);
}

int gtimer_check_capture_done(int id)
{
	u32 status = gtimer_read(p_gtimer,id,TMR_REG_IRQ_RAW);
	
	return (status & TMR_INT_CAP);
}

void gtimer_get_capture_data(int id, uint32_t *val1, uint32_t *val2)
{
	*val1 = gtimer_read(p_gtimer, id, TMR_REG_CAP1);
	*val2 = gtimer_read(p_gtimer, id, TMR_REG_CAP2);
}

void gtimer_clear_capture(int id)
{
	gtimer_write(p_gtimer, id, TMR_REG_IRQ_RAW, TMR_INT_CAP);
}

void gtimer_set_pwm_mode(int id, E_TMR_PWM_MODE pwm_mode, E_TMR_PWM_OUTPUT_MODE output_mode, E_TMR_PWM_POL pwm_pol)
{
	u32 ctrl;

	ctrl = gtimer_read(p_gtimer, id, TMR_REG_CTRL);

	// pwm mode
	ctrl &=  ~(3 << TMR_PWM_TRG_SHIFT);
	ctrl |= pwm_mode<<TMR_PWM_TRG_SHIFT;
	
	// output mode
	ctrl &=  ~(1 << TMR_PULSE_TGL_SHIFT);
	ctrl |= output_mode << TMR_PULSE_TGL_SHIFT;

	// pol
	ctrl &= ~(1 << PWM_POL_SHIFT);	
	ctrl |= pwm_pol << PWM_POL_SHIFT;

	gtimer_write(p_gtimer, id, TMR_REG_CTRL, ctrl);
}

void gtimer_set_compare_mode(int id, int en)
{
	u32 ctrl = gtimer_read(p_gtimer, id, TMR_REG_CTRL);

	ctrl &=  ~(1<<13);
	if (en)
	{
		ctrl |= 1<<13;
	}
	
	gtimer_write(p_gtimer, id, TMR_REG_CTRL, ctrl);
}

void gtimer_set_int_mask(int id, unsigned long value)
{
	gtimer_write( p_gtimer, id, TMR_REG_IRQ_EN, value);
}

unsigned long gtimer_get_int_status(int id)
{
	return gtimer_read(p_gtimer, id, TMR_REG_IRQ_ST);
}

unsigned long gtimer_get_raw_int_status(int id)
{
	return gtimer_read(p_gtimer, id, TMR_REG_IRQ_RAW);
}

void gtimer_clear_int(int id, unsigned long value)
{
	gtimer_write( p_gtimer, id, TMR_REG_IRQ_ST, value);
}

void gtimer_save_context(int id)
{
	if (p_gtimer_context == NULL)
		return;
	info("save:TMR%d load/cnt/ctrl/rawirq:0x%x/0x%x/0x%x/0x%x\n", id, 
			gtimer_read(p_gtimer, id, TMR_REG_LOAD),
			gtimer_read(p_gtimer, id, TMR_REG_COUNT),
			gtimer_read(p_gtimer, id, TMR_REG_CTRL),
			gtimer_read(p_gtimer, id, TMR_REG_IRQ_RAW));

	p_gtimer_context->index_regs[id].loadval = gtimer_read(p_gtimer, id, TMR_REG_LOAD);
	p_gtimer_context->index_regs[id].count = gtimer_read(p_gtimer, id, TMR_REG_COUNT);
	p_gtimer_context->index_regs[id].compval = gtimer_read(p_gtimer, id, TMR_REG_COMPARE);
	p_gtimer_context->index_regs[id].control = gtimer_read(p_gtimer, id, TMR_REG_CTRL);
	p_gtimer_context->index_regs[id].irqen = gtimer_read(p_gtimer, id, TMR_REG_IRQ_EN);
}

void gtimer_restore_context(int id)
{
	if (p_gtimer_context == NULL)
		return;

	gtimer_reset(id);

	gtimer_write(p_gtimer, id, TMR_REG_LOAD, p_gtimer_context->index_regs[id].loadval);
	gtimer_write(p_gtimer, id, TMR_REG_COUNT, p_gtimer_context->index_regs[id].count);
	gtimer_write(p_gtimer, id, TMR_REG_CTRL, p_gtimer_context->index_regs[id].control);
	gtimer_write(p_gtimer, id, TMR_REG_IRQ_EN, p_gtimer_context->index_regs[id].irqen);

	info("restore:TMR%d load/cnt/ctrl/rawirq:0x%x/0x%x/0x%x/0x%x\n", id, 
			gtimer_read(p_gtimer, id, TMR_REG_LOAD),
			gtimer_read(p_gtimer, id, TMR_REG_COUNT),
			gtimer_read(p_gtimer, id, TMR_REG_CTRL),
			gtimer_read(p_gtimer, id, TMR_REG_IRQ_RAW));
}

