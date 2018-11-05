/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/
#include "cdl_config.h"
#include "cdl_gpio.h"

struct brite_gpio gpio_dev;
struct brite_gpio *p_gpio = &gpio_dev;

static void gpio_set_debounce_clk(E_GPIO_PORT port, uint32_t dfg);

void gpio_set_output(E_GPIO_PORT port, E_PIN_PORT pin, uint32_t val)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_DATA(port)) & (~(1 << pin));
	
	if (val) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_DATA(port));
}

uint32_t gpio_get_output(E_GPIO_PORT port, E_PIN_PORT pin, uint32_t val)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_DATA(port)) & (~(1 << pin));
	
	reg_val = (reg_val >> pin) & 0x1;
	
	return reg_val;
}

void gpio_set_direction(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_DIR dir)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_DIR(port)) & (~(1 << pin));

	if (dir) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_DIR(port));
}

void gpio_set_ctrl(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_CTRL ctrl)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_CTRL(port)) & (~(1 << pin));
	
	if (ctrl) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_CTRL(port));
}

uint32_t gpio_get_input(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_EXT(port));
	
	reg_val = (reg_val >> pin) & 0x1;

	return (uint32_t)reg_val;
}

void gpio_enable_int(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IEN ien)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IEN(port)) & (~(1 << pin));
	
	if (ien) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_IEN(port));
}

void gpio_set_int_type(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IS is)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IS(port)) & (~(1 << pin));
	
	if (is) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_IS(port));
}

E_GPIO_IS gpio_get_int_type(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IS(port));
	
	reg_val = (reg_val >> pin) & 0x1;
	
	return reg_val;
}

/*
val: 	0: falling edge / low level trigger
        1: rising edge / high level trigger
	2:both edges
*/
void gpio_set_int_trig_type(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_ITYPE itype)
{
	uint32_t ibe;
	uint32_t iev;

	ibe = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IBE(port)) & (~(1 << pin));
	
	if (itype == GPIO_ITYPE_EDGE_BOTH) 
	{
		// both edge sensitive
		ibe |= (1 << pin);
		writel(ibe, (uint32_t)p_gpio->regs + BRITE_GPIO_IBE(port));
	} 
	else 
	{
		// single edge sensitive
		writel(ibe, (uint32_t)p_gpio->regs + BRITE_GPIO_IBE(port));

		// falling/rising edge triggered
		iev = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IEV(port)) & (~(1 << pin));
		if (itype == GPIO_ITYPE_EDGE_RISING) 
		{
			iev |= (1 << pin);
		}
		
		writel(iev, (uint32_t)p_gpio->regs + BRITE_GPIO_IEV(port));
	}
}

/* return:
0:no raw interrupt
1:raw interrupt
*/
uint32_t gpio_get_raw_int_status(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_RIS(port));
	
	reg_val = (reg_val >> pin) & 0x1;

	return (uint32_t)reg_val;
}

void gpio_set_int_mask(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IM im)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IM(port)) & (~(1 << pin));
	
	if (im) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_IM(port));
}

bool gpio_is_int_mask(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IM(port)) & ((1 << pin));
	
	if (reg_val) 
	{
		return true;
	}
	
	return false;
}

/* return:
0:no interrupt
1:have interrupt
*/
bool gpio_get_int_status(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_MIS(port));
	
	reg_val = (reg_val >> pin) & 0x1;
	
	return (bool)reg_val;
}

// only effect for edge-sensitive interrupt
void gpio_clear_int(E_GPIO_PORT port, E_PIN_PORT pin)
{
	writel((1 << pin), (uint32_t)p_gpio->regs + BRITE_GPIO_IC(port));
}

void gpio_enable_debounce(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_DB db)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_DB(port)) & (~(1 << pin));
	
	if (db) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_DB(port));
}

void gpio_set_ig(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IG ig)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IG(port)) & (~(1 << pin));
	
	if (ig) 
	{
		reg_val |= (1 << pin);
	}
	
	writel(reg_val, (uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));
}

E_GPIO_IG gpio_get_ig(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));

	info("ig reg:%x\n", reg_val);
	
	reg_val = (reg_val >> pin) & 0x1;
	
	return reg_val;
}

// 10ms*75M/(2^8-1) ~= 2941.18' Db_dfg[23:0]=2942;
// 10ms*75M/2942=254.92 ' Db_dfg[31:16]=254
static void gpio_set_debounce_clk(E_GPIO_PORT port, uint32_t dfg)
{
	writel(dfg, (uint32_t)p_gpio->regs + BRITE_GPIO_DFG(port));
}

void gpio_init(E_GPIO_PORT port, uint32_t base, uint32_t pclk, uint32_t dfg)
{
	memset(p_gpio, 0, sizeof(struct brite_gpio));

	p_gpio->regs = (void*)base;
	p_gpio->pclk = pclk;
	p_gpio->debounce = dfg;

	gpio_set_debounce_clk(port, dfg);
}

