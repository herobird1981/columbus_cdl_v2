/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "cdl_pll.h"
#include "test_gpio.h"

#if (CONFIG_GPIO_TEST == 1)
/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
cmdline_entry gpio_test_menu[];

// for interrupt handler
static int gpio_int_counter = 0;
static E_GPIO_PORT gpio_port = 0;
static int gpio_pin = 0;
static uint32_t g_gpio_int_flag = 0;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/
static uint32_t gpio_vec(E_GPIO_PORT port, E_PIN_PORT pin);

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
static int cmd_help(int argc, char *argv[])
{
	return cmdline_help_general(&gpio_test_menu[0]);
}

E_GPIO_PORT gpio_get_port(char *argv)
{
	E_GPIO_PORT port;

	if ((!strcmp(argv, "a")) || (!strcmp(argv, "A")))
	{
		port = GPIOA;
	}
	else if ((!strcmp(argv, "b")) || (!strcmp(argv, "B")))
	{
		port = GPIOB;
	}
	else if ((!strcmp(argv, "c")) || (!strcmp(argv, "C")))
	{
		port = GPIOC;
	}
	else if ((!strcmp(argv, "d")) || (!strcmp(argv, "D")))
	{
		port = GPIOD;
	}
	else
	{
		port = get_arg_ulong(argv);
	}

	if (port >= NUM_GPIO_PORTS)
	{
		info("invalid, default to porta\n");
		port = GPIOA;
	}

	return port;
}

static int gpio_output(int argc, char *argv[])
{
	uint32_t val;
	E_GPIO_PORT port;
	E_PIN_PORT pin;

	info("%s:output test\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s port pin val\n", argv[0]);
		return 0;
	}

	// Get the parameters
	port = gpio_get_port(argv[1]); 
	pin = get_arg_ulong(argv[2]);
	val = get_arg_ulong(argv[3]);

	info("port:%d, pin: %d, val:%d\n", port, pin, val);

	// initialize gpio pin
	if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
	{
		return 0;
	}
	
	gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
	gpio_enable_int(port, pin, GPIO_INT_DIS);
	gpio_clear_int(port, pin);
	gpio_set_output(port, pin, val);
	gpio_set_direction(port, pin, GPIO_OUTPUT);
	gpio_set_ctrl(port, pin, GPIO_CTRL_SW);

	//gpio_set_output(port, pin, val);
	info("%s: set gpio port(%x), pin(%x) to level '%s'\n", argv[0], port, pin, val ? "high" : "low");
	info("%s: view the oscilloscope or multimeter result for pass or fail\n", argv[0]);

	return 0;
}

static int gpio_input(int argc, char *argv[])
{
	uint32_t gpio_val, input_val;
	E_GPIO_PORT port;
	E_PIN_PORT pin;

	info("%s:input test\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s port pin val\n", argv[0]);
		return 0;
	}
	
	// Get the parameters
	port = gpio_get_port(argv[1]);
	pin = get_arg_ulong(argv[2]);
	input_val = get_arg_ulong(argv[3]);
	if (input_val) 
	{
		input_val = 1;
	}

	// initialize gpio pin
	if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_EN, IO_OD_DIS))
	{
		return 0;
	}

	gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
	gpio_enable_int(port, pin, GPIO_INT_DIS);
	gpio_clear_int(port, pin);

	gpio_set_direction(port, pin, GPIO_INPUT);
	gpio_set_ctrl(port, pin, GPIO_CTRL_SW);

	gpio_val = gpio_get_input(port, pin);
	debug("gpio input: expect %d act %d\n", input_val, gpio_val);

	if (gpio_val != input_val) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}
	
	return 0;
}

static uint32_t gpio_vec(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t vec;

	if (GPIO_IG_INTR0 == gpio_get_ig(port, pin))
	{
		if  	(port == GPIOA)	vec = INT_GPIOA0;
		else if (port == GPIOB)	vec = INT_GPIOB0;
		else if (port == GPIOC)	vec = INT_GPIOC0;
		else					vec = INT_GPIOD0;
	}
	else
	{
		if  	(port == GPIOA)	vec = INT_GPIOA1;
		else if (port == GPIOB)	vec = INT_GPIOB1;
		else if (port == GPIOC)	vec = INT_GPIOC1;
		else					vec = INT_GPIOD1;
	}		

	info("vec:%d\n", vec);

	return vec;
}

void gpio_int_handler(void *ptr)
{
	bool ret_val;
	E_GPIO_PORT port = gpio_port;
	E_PIN_PORT pin = gpio_pin;

	//info("gpio_int_handler\n");

	ret_val = gpio_get_int_status(port, pin);
	if (ret_val) 
	{
		gpio_int_counter++;
		gpio_clear_int(port, pin);
	}

	info("gpio_int_handler, status=%x count=%d\n", ret_val, gpio_int_counter);
	
	// for level interrupt, disable int to prevent so much int occur
	if (console_tstc()) 
	{
		if ('q' == console_getc()) 
		{
			g_gpio_int_flag = 1;
			gpio_set_int_mask(port, pin , GPIO_IM_EN);
		}
	}
}

// for debounce
void gpio_int_handler_1(void *ptr)
{
	bool ret_val;
	E_GPIO_PORT port = gpio_port;
	E_PIN_PORT pin = gpio_pin;

	//info("gpio_handler\n");
	
	ret_val = gpio_get_int_status(port, pin);
	if (ret_val) 
	{
		gpio_int_counter++;
		gpio_clear_int(port, pin);
	}
}

// testcase: gpio interrupt type test
static int gpio_int_type(int argc, char *argv[])
{
	uint32_t fail = 0;
	int32_t int_type;
	E_GPIO_PORT port;
	E_PIN_PORT pin;
	E_GPIO_IG ig;
	E_GPIO_IS is;
	E_GPIO_ITYPE itype;
	uint32_t vec;

	info("%s: gpio interrupt type test,input 'q' to quit this testcase\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 6) 
	{
		info("wrong cmd_format: %s port pin is(level/edge) itype((high/low)/(fall/rise/both)) ig[0,1]\n", argv[0]);
		return 0;
	}

	// Get the parameters
	port = gpio_get_port(argv[1]); 
	pin = get_arg_ulong(argv[2]);
	if (!strcmp(argv[3], "level")) 
	{
		is = GPIO_IS_LEVEL;
	} 
	else if (!strcmp(argv[3], "edge")) 
	{
		is = GPIO_IS_EDGE;
	} 
	else 
	{
		is = GPIO_IS_LEVEL;
		info("wrong interrupt type, default as level\n");
	}

	// edge
	if (is == GPIO_IS_EDGE) 
	{
		if (!strcmp(argv[4], "fall")) 
		{
			itype = GPIO_ITYPE_EDGE_FALLING;
		} 
		else if (!strcmp(argv[4], "rise")) 
		{
			itype = GPIO_ITYPE_EDGE_RISING;
		}
		else  
		{
			// both
			itype = GPIO_ITYPE_EDGE_BOTH;
		}
	} 
	else
	{
		if (!strcmp(argv[4], "low"))
		{
			itype = GPIO_ITYPE_LEVEL_LOW;
		} 
		else
		{
			// high
			itype = GPIO_ITYPE_LEVEL_HI;
		}	
	}

	ig = get_arg_ulong(argv[5]);

	// initialize gpio pin
	if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_EN, IO_OD_DIS))
	{
		return 0;
	}

	gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
	gpio_enable_debounce(port, pin, GPIO_DB_EN);
	gpio_enable_int(port, pin, GPIO_INT_DIS);
	gpio_clear_int(port, pin);

	gpio_set_direction(port, pin, GPIO_INPUT);
	gpio_set_ctrl(port, pin, GPIO_CTRL_SW);
	gpio_set_ig(port, pin, ig);

	gpio_int_counter = 0;
	gpio_port = port;
	gpio_pin = pin;	
	g_gpio_int_flag = 0;

	// register int handler and enable it
	vec = gpio_vec(port, pin);
	
	if (is == GPIO_IS_EDGE) 
	{
		if      (itype == GPIO_ITYPE_EDGE_FALLING) 	int_type = IRQ_TYPE_EDGE_FALLING;
		else if (itype == GPIO_ITYPE_EDGE_RISING) 	int_type = IRQ_TYPE_EDGE_RISING;
		else 										int_type = IRQ_TYPE_EDGE_BOTH;
	}
	else 
	{
		if (itype == GPIO_ITYPE_LEVEL_LOW)			int_type = IRQ_TYPE_LEVEL_LOW;
		else										int_type = IRQ_TYPE_LEVEL_HIGH;
	}
	
	irq_generic_install_handler(vec, gpio_int_handler, NULL);
	irq_generic_config_int(vec, int_type);
	irq_generic_enable_int(vec);

	// enable interrupt
	gpio_set_int_type(port, pin, is);
	gpio_set_int_mask(port, pin, GPIO_IM_DIS);
	gpio_set_int_trig_type(port, pin, itype);
	gpio_enable_int(port, pin, GPIO_INT_EN);

	// wait interrupt handle, and wait user break testcase
	while (g_gpio_int_flag == 0) 
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}
	}

	if (gpio_int_counter == 0) 
	{
		fail++;
	}

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	// disable interrupt
	irq_generic_disable_int(vec);
	irq_generic_remove_handler(vec, NULL, NULL);
	gpio_set_int_mask(port, pin, GPIO_IM_EN);
	gpio_enable_int(port, pin, GPIO_INT_DIS);

	return 0;
}

// testcase:gpio debounce test
static int gpio_debounce(int argc, char *argv[])
{
	uint32_t fail = 0;
	uint32_t dfg, deb_pulsewidth, dfg_div, dfg_cycles;
	E_GPIO_PORT input_port, output_port;
	E_PIN_PORT input_pin, output_pin;
	E_GPIO_DB deb_en;
	uint32_t vec, output_pulse;

	info("%s: gpio debounce test,input 'q' to quit this testcase\n", argv[0]);

	if (argc < 8) 
	{
		info("wrong cmd_format: %s port(input) pin deb_en deb_pulsewidth(us) port(output) pin pulsewidth(us)\n", argv[0]);
		return 0;
	}

	input_port = gpio_get_port(argv[1]);
	input_pin = get_arg_ulong(argv[2]);
	deb_en = get_arg_ulong(argv[3]);
	deb_pulsewidth = get_arg_ulong(argv[4]);

	output_port = gpio_get_port(argv[5]);
	output_pin = get_arg_ulong(argv[6]);
	output_pulse = get_arg_ulong(argv[7]);

	// initialize gpio pin
	if (!misc_gpio_pin_init(input_port, input_pin, IO_PU_UP, IO_PD_EN, IO_OD_DIS))
	{
		return 0;
	}

	if (!misc_gpio_pin_init(output_port, output_pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
	{
		return 0;
	}
	
	// 10ms*75M/(2^8) ~= 2941.18' Db_dfg[23:0]=2929;
	// 10ms*75M/2929=256.06 ' Db_dfg[31:16]=256 -1 
	if (deb_pulsewidth < 0x100)
	{
		dfg_div = GPIO_PCLK/1000000;
		dfg_cycles = deb_pulsewidth;
	}
	else
	{
		dfg_div = (deb_pulsewidth * (GPIO_PCLK/1000000))/0x100;
		dfg_cycles = deb_pulsewidth * (GPIO_PCLK/1000000)/dfg_div;
		if (dfg_cycles > 0x100)
		{
			dfg_cycles = 0x100;
		}
	}

	info("dfg_cycles:%d, dfg_div:%d\n", dfg_cycles, dfg_div);
	dfg = ((dfg_cycles-1)<<24)|((dfg_div-1)<<0);

	// input pin initialize
	info("gpio clk:%dHZ\n", GPIO_PCLK);
	gpio_init(input_port, GPIO_BASE, GPIO_PCLK, dfg);
	gpio_enable_debounce(input_port, input_pin, deb_en);
	gpio_enable_int(input_port, input_pin, GPIO_INT_DIS);
	gpio_clear_int(input_port, input_pin);

	gpio_set_direction(input_port, input_pin, GPIO_INPUT);
	gpio_set_ctrl(input_port, input_pin, GPIO_CTRL_SW);
	gpio_set_ig(input_port, input_pin, 0);

	gpio_int_counter = 0;
	gpio_port = input_port;
	gpio_pin = input_pin;	
	g_gpio_int_flag = 0;

	// output pin initialize
	gpio_enable_int(output_port, output_pin, GPIO_INT_DIS);
	gpio_clear_int(output_port, output_pin);
	gpio_set_output(output_port, output_pin, 0);
	gpio_set_direction(output_port, output_pin, GPIO_OUTPUT);
	gpio_set_ctrl(output_port, output_pin, GPIO_CTRL_SW);
	//gpio_set_output(output_port, output_pin, 0);

	// register int handler and enable it
	vec = gpio_vec(input_port, input_pin);
	irq_generic_install_handler(vec, gpio_int_handler_1, NULL);
	irq_generic_config_int(vec, IRQ_TYPE_EDGE_BOTH);
	irq_generic_enable_int(vec);

	// enable interrupt
	gpio_set_int_type(input_port, input_pin, GPIO_IS_EDGE);
	gpio_set_int_mask(input_port, input_pin, GPIO_IM_DIS);
	gpio_set_int_trig_type(input_port, input_pin, GPIO_ITYPE_EDGE_BOTH);
	gpio_enable_int(input_port, input_pin, GPIO_INT_EN);

	// output pulse
	udelay(output_pulse);
	gpio_set_output(output_port, output_pin, 1);
	udelay(output_pulse);
	gpio_set_output(output_port, output_pin, 0);

	// wait interrupt handle, and wait user break testcase
	while (gpio_int_counter < 2)  
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				// pwm input, always interrupt 
				if (gpio_int_counter < 2)
				{
					fail++;
				}

				break;
			}
		}
	}

	info("int_cnt:%d\n", gpio_int_counter);

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	// disable interrupt
	irq_generic_disable_int(vec);
	irq_generic_remove_handler(vec, NULL, NULL);
	gpio_set_int_mask(input_port, input_pin, GPIO_IM_EN);
	gpio_enable_int(input_port, input_pin, GPIO_INT_DIS);

	return 0;
}

// testcase: gpio output push pull test
static int gpio_output_pu(int argc, char *argv[])
{
	uint32_t val;
	E_GPIO_PORT port;
	E_PIN_PORT pin;
	E_IO_PU pu; 
	E_IO_PD pd;
	E_IO_OD od;

	info("%s:output test\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s port pin pu(pullup/pulldown) output(0/1)\n", argv[0]);
		return 0;
	}

	// Get the parameters
	port = gpio_get_port(argv[1]); 
	pin = get_arg_ulong(argv[2]);
	pu = (!strcmp(argv[3], "pullup")) ? IO_PU_UP : IO_PU_DOWN;
	val = get_arg_ulong(argv[4]);
	pd = IO_PD_DIS;
	od = IO_OD_DIS;

	// initialize gpio pin
	if (!misc_gpio_pin_init(port, pin, pu, pd, od))
	{
		return 0;
	}

	gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
	gpio_enable_int(port, pin, GPIO_INT_DIS);
	gpio_clear_int(port, pin);
	gpio_set_output(port, pin, val);
	gpio_set_direction(port, pin, GPIO_OUTPUT);
	gpio_set_ctrl(port, pin, GPIO_CTRL_SW);
	//gpio_set_output(port, pin, val);

	info("%s: set gpio port(%d), pin(%d) to '%s' '%s' '%s'\n", 
		argv[0], 
		port, 
		pin, 
		(pu == IO_PU_UP) ? "pull up" 	 : "pull down",
		(pd == IO_PD_EN) ? "pull enable" : "pull disable",
		(od == IO_OD_EN) ? "open drain"  : "normal");
	
	info("%s: view the oscilloscope or multimeter result for pass or fail\n", argv[0]);

	return 0;
}

// testcase: gpio input push pull test
static int gpio_input_pu(int argc, char *argv[])
{
	uint32_t def_val, gpio_val;
	E_GPIO_PORT port;
	E_PIN_PORT pin;
	E_IO_PU pu; 
	E_IO_PD pd;
	E_IO_OD od;

	info("%s:output test\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s port pin pu(pullup/pulldown) od(en/dis)\n", argv[0]);
		return 0;
	}

	// Get the parameters
	port = gpio_get_port(argv[1]); 
	pin = get_arg_ulong(argv[2]);
	pu = (!strcmp(argv[3], "pullup")) ? IO_PU_UP : IO_PU_DOWN;
	pd = IO_PD_EN;
	od = (!strcmp(argv[4], "en"))     ? IO_OD_EN : IO_OD_DIS;

	// initialize gpio pin
	if (!misc_gpio_pin_init(port, pin, pu, pd, od))
	{
		return 0;
	}

	gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
	gpio_enable_int(port, pin, GPIO_INT_DIS);
	gpio_clear_int(port, pin);
	gpio_set_direction(port, pin, GPIO_INPUT);
	gpio_set_ctrl(port, pin, GPIO_CTRL_SW);

	gpio_val = gpio_get_input(port, pin);
	def_val = (pu == IO_PU_UP) ? 1 : 0;
	
	debug("gpio input: default %d act %d\n", def_val, gpio_val);

	info("%s: set gpio port(%d), pin(%d) to '%s' '%s' '%s'\n", 
		argv[0], 
		port, 
		pin, 
		(pu == IO_PU_UP) ? "pull up" 	 : "pull down",
		(pd == IO_PD_EN) ? "pull enable" : "pull disable",
		(od == IO_OD_EN) ? "open drain"  : "normal");
	
	if (gpio_val != def_val) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry gpio_test_menu[] = {
	{"help", 			cmd_help,		"           : Display list of commands"},
	{"h",				cmd_help, 		"              : alias for help"},
	{"?",				cmd_help, 		"              : alias for help"},
	{"gpio_output",		gpio_output,	"    : gpio output test"},
	{"gpio_input",		gpio_input,		"     : gpio input test"},
	{"gpio_int_type",	gpio_int_type,	"  : gpio interrupt type test"},
	{"gpio_debounce",	gpio_debounce,	"  : gpio debounce test"},
	{"gpio_output_pu",	gpio_output_pu,	" : gpio output push pull, open drain test"},
	{"gpio_input_pu",	gpio_input_pu,	"  : gpio input push pull test"},
	{0, 				0, 				0}
};

#define GPIO_IG_DEF_VAL	0xFFFF0000
bool gpio_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_GPIO_PORT port;

	for (port = GPIOA; port < NUM_GPIO_PORTS; port++)
	{
		info("%s: port:%d\n", __func__, port);
		
		gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);

		info("write ig reg\n");
		
		// write register
		writel(*p_wr_val, (uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));

		info("read&compare ig reg\n");
		*p_rd_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));
		if (*p_rd_val != *p_wr_val)
		{
			return false;
		}
		
		// reset
		info("gpio reset\n");
		prcm_reset(ARM_GPIO_RST);
		
		// read register
		info("read&compare ig reg\n");
		*p_rd_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));

		if (*p_rd_val != GPIO_IG_DEF_VAL)
		{
			return false;
		}
	}
	
	return true;
}

bool gpio_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_GPIO;
	E_GPIO_PORT port;

	for (port = GPIOA; port < NUM_GPIO_PORTS; port++)
	{
		info("%s: port:%d\n", __func__, port);

		// clock gate enable
		info("clock gate enable\n");
		prcm_blk_cg(blk, ARM_BLK_CG_EN);

		gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);

		// write register
		info("write ig reg\n");
		writel(*p_wr_val, (uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));

		info("read&compare ig reg\n");
		*p_rd_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));
		if (*p_rd_val != *p_wr_val)
		{
			return false;
		}
		
		// clock gate disable
		info("clock gate disable\n");
		prcm_blk_cg(blk, ARM_BLK_CG_DIS);
		
		// write register
		info("write ig reg\n");
		writel((*p_wr_val+1), (uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));
		
		// clock gate enable
		info("clock gate enable\n");
		prcm_blk_cg(blk, ARM_BLK_CG_EN);
		
		// read register
		info("read&compare ig reg\n");
		*p_rd_val = readl((uint32_t)p_gpio->regs + BRITE_GPIO_IG(port));
		if (*p_rd_val != *p_wr_val)
		{
			return false;
		}
	}
	
	return true;
}
#else
bool gpio_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool gpio_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

