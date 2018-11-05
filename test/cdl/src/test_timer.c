/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_timer.h"
#include "cdl_pll.h"
#include "cdl_gpio.h"

#if (CONFIG_TIMER_TEST == 1)
/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define TIMER_REG_LEN_IN_DWORD 10

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
cmdline_entry timer_test_menu[];

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
// timer test may change the setting of system timer, restore it after test
static void timer_restore_system(E_TIMER_PORT id)
{
	info("timer_restore_system:TMR%d\n", id);

	if (id == SYS_TIMER)
	{
		gtimer_timer_init(TIMER_BASE, TIMER_PCLK, id);
	}
}

//************************************************************************************************************
// timer check counter test
//************************************************************************************************************
static int timer_check_counter(E_TIMER_PORT id, E_TMR_COUNT_MODE mode)
{
	uint32_t prev_counter, counter;
	int32_t i, fail = 0;

	prev_counter = gtimer_get_count(id);

	for (i = 0; i < 256; i++) 
	{ 
		counter = gtimer_get_count(id);
		info("counter:%x\n", counter);
		
		// request timer counter should decrease
		if (mode == TMR_DOWN) 
		{
			if (counter >= prev_counter) 
			{
				fail++;
			}
		} 
		else if (mode == TMR_UP) 
		{
			if (counter <= prev_counter) 
			{
				fail++;
			}
		}			
		else
		{
			// request timer counter should keep
			if (counter != prev_counter) 
			{
				fail++;
			}
		}
		
		prev_counter = counter;
	}
	
	if (fail) 
	{
		if (mode == TMR_DOWN) 
		{
			info("timer_check_counter fail, counter not decrease\n");
		} 
		else if (mode == TMR_UP) 
		{
			info("timer_check_counter fail, counter not increase\n");
		} 
		else
		{
			info("timer_check_counter fail, counter still change\n");
		}
	}
	
	return fail;
}

static int cmd_help(int argc, char *argv[])
{	
	return cmdline_help_general(&timer_test_menu[0]);
}

//************************************************************************************************************
// timer start/stop test
//************************************************************************************************************
static int timer_start_stop(int argc, char *argv[])
{
	int32_t fail = 0;
	E_TIMER_PORT id;

	info("%s: timer start/stop test\n", argv[0]);

	if (argc < 2) 
	{
		info("wrong cmd_format: %s id\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);

	// start timer and check counter
	gtimer_start(id);
	info("timer %d started,counter should increase\n", id);
	fail += timer_check_counter(id, TMR_UP);

	info("\n");
	
	// stop timer and check counter
	gtimer_stop(id);
	info("timer %d stoped,counter should keep fixed\n", id);
	fail += timer_check_counter(id, TMR_FIXED);
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// timer reload test
//************************************************************************************************************
static int timer_reload(int argc, char *argv[])
{
	uint32_t counter;
	uint32_t load_val = 0x4000000;
	int32_t fail = 0;
	E_TIMER_PORT id;
	uint32_t i;

	info("%s: timer reload test\n",argv[0]);
	
	if (argc < 2) 
	{
		info("wrong cmd_format: %s id\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_load(id, load_val);

	// start timer and check counter
	info("timer %d started, counter should near zero\n", id);
	gtimer_start(id);

	counter = gtimer_get_count(id);
	info("counter:%x\n", counter);
	
	// init counter is 0
	if (counter > 0x1000) 
	{
		fail++;
	}

	for (i = 0; i < 256; i++)
	{
		counter = gtimer_get_count(id);
		info("counter:%x\n", counter);
	}
	
	info("timer reload, counter should near %x\n", load_val);
	gtimer_reload(id);
	counter = gtimer_get_count(id);

	info("counter:%x\n", counter);
	
	if ((counter - load_val) > 0x1000) 
	{
		fail++;
	}

	for (i = 0; i < 256; i++)
	{
		counter = gtimer_get_count(id);
		info("counter:%x\n", counter);
	}	

	// stop timer and check counter
	gtimer_stop(id);
	info("timer %d stoped\n", id);

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// timer count mode test
// TMR_UP/ TMR_DOWN, TMR_ONE_SHOT/ TMR_AUTO; TMR_FREE/TMR_USER
//************************************************************************************************************
static int timer_count_mode(int argc, char* argv[])
{
	int32_t fail, total_fail = 0;
	E_TMR_MODE mode;
	E_TMR_AUTO_LD run_mode;
	E_TMR_COUNT_MODE count_mode;
	E_TIMER_PORT id;

	info("%s: timer count mode test\n",argv[0]);

	if (argc < 2) 
	{
		info("wrong cmd_format: %s id\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);

	for (mode = TMR_FREE; mode <= TMR_USER; mode++) 
	{
		for (run_mode = TMR_ONE_SHOT; run_mode <= TMR_AUTO; run_mode++) 
		{
			for (count_mode = TMR_UP; count_mode <= TMR_DOWN; count_mode++) 
			{
				fail = 0;
				
				info("\ntest timer %d: mode:'%s' run mode:'%s' count mode:'%s'\n",
					id,
					(mode == TMR_FREE) ? "free run" : "user defined",
					(run_mode == TMR_ONE_SHOT) ? "one shot" : "auto reload",
					(count_mode == TMR_UP) ? "UP" : "DOWN");
				
				gtimer_set_mode(id, count_mode, run_mode, mode);
				if (count_mode == TMR_UP) 
				{
					gtimer_set_count(id, 0);
				}
				else 
				{
					gtimer_set_count(id, 0xFFFFFFFF);
				}

				info("check counter : should %s\n", (count_mode == TMR_UP) ? "increase" : "decrease");
				
				gtimer_start(id);
				fail += timer_check_counter(id, count_mode);
				gtimer_stop(id);

				total_fail += fail;
				if (fail)
				{
					info("test fail!\n");
				}
				else
				{
					info("test pass!\n");
				}	
			}
		}
	}

	if (total_fail) 
	{
		info("\n%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("\n%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// timer overflow test
// TMR_UP/ TMR_DOWN, TMR_ONE_SHOT/ TMR_AUTO; TMR_FREE/TMR_USER
//************************************************************************************************************
static int timer_overflow(int argc, char* argv[])
{
	int32_t fail, total_fail = 0;
	E_TMR_MODE mode;
	E_TMR_AUTO_LD run_mode;
	E_TMR_COUNT_MODE count_mode;
	E_TIMER_PORT id;
	uint32_t val, init_val, load_val;
	uint32_t time_out = 0;

	info("%s: timer overflow test\n", argv[0]);

	if (argc < 2) 
	{
		info("wrong cmd_format: %s id\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);

	for (mode = TMR_FREE; mode <= TMR_USER; mode++) 
	{
		for (run_mode = TMR_ONE_SHOT; run_mode <= TMR_AUTO; run_mode++) 
		{
			for (count_mode = TMR_UP; count_mode <= TMR_DOWN; count_mode++) 
			{
				fail = 0;
				
				info("\ntest timer %d: mode:'%s' run mode:'%s' count mode:'%s'\n",
					id,
					(mode == TMR_FREE) ? "free run" : "user defined",
					(run_mode == TMR_ONE_SHOT) ? "one shot" : "auto reload",
					(count_mode == TMR_UP) ? "UP" : "DOWN");
				
				gtimer_set_mode(id, count_mode, run_mode, mode);

				if (count_mode == TMR_UP) 
				{
					init_val = 0;
					load_val = 0xffffffff - 0x100000;
				}
				else 
				{
					init_val = 0xffffffff;
					load_val = 0x100000;
				}
				
				info("initial counter %x reload value %x\n", init_val, load_val);
				
				gtimer_set_count(id, init_val);
				gtimer_set_load(id, load_val);
				gtimer_clear_raw_status(id, 0x7);
				gtimer_start(id);
				gtimer_reload(id);

				// check status
				while (!(gtimer_get_raw_int_status(id) & TMR_INT_OVERFLOW))
				{
					// delay 10us
					sw_udelay_10x(1);
					if (time_out++ > 1000 * 100)
					{
						info("gtimer get overflow int timeout\n");
						break;
					}
				}

				if (mode == TMR_FREE) 
				{
					val = gtimer_get_count(id);
					if (((count_mode == TMR_UP) && ((val - init_val) > 0x1000)) ||
						((count_mode == TMR_DOWN) && ((init_val - val) > 0x1000))) 
					{
							fail++;
					}
					
					info("timer %d overflow\n", id);
					info("check counter : should still %s\n", (count_mode == TMR_UP) ? "increase" : "decrease");
					
					fail += timer_check_counter(id, count_mode);
				}
				else 
				{
					if (run_mode == TMR_ONE_SHOT) 
					{
						info("timer %d overflow\n",id);
						info("check counter : should keep fixed\n");
						
						fail += timer_check_counter(id, 2);
						val = gtimer_get_count(id);
						if (((count_mode == TMR_UP) && (val != 0xffffffff)) ||
							((count_mode == TMR_DOWN) && (val !=0))) 
						{
							fail++;
						}
					} 
					else 
					{
						// timer should still run and reload from load value
						val = gtimer_get_count(id);
						if (((count_mode == TMR_UP) && ((val - load_val) > 0x1000)) ||
							((count_mode == TMR_DOWN) && ((load_val - val) > 0x1000))) 
						{
							fail++;
						}
						
						info("current counter:%x should near reload value\n",val);
					}
				}
				
				gtimer_stop(id);

				total_fail += fail;
				if (fail)
				{
					info("test fail!\n");
				}	
				else
				{
					info("test pass!\n");
				}	
			}
		}
	}

	if (total_fail) 
	{
		info("\n%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("\n%s: test pass\n", argv[0]);
	}
	
	return 0;
}

//************************************************************************************************************
// timer reset test
//************************************************************************************************************
static int timer_reset(int argc, char* argv[])
{
	E_TIMER_PORT id;
	uint32_t load_val = 0x200000;
	int32_t fail = 0;
	uint32_t val;
	uint32_t i;

	info("%s: timer sw reset test\n", argv[0]);
	
	if (argc < 2) 
	{
		info("wrong cmd_format: %s id\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_count(id, load_val);
	gtimer_start(id);
	fail += timer_check_counter(id, TMR_UP);

	info("reset timer %d,after reset timer should stop and all of the register be reset to default value '0'\n",id);
	info("register value before reset:\r\n");
	for (i = 0; i < 10; i++)
	{
		info("0x%04x	", *((volatile uint32_t *)(TIMER_BASE + id*0x40+i*4)));
	}

	gtimer_reset(id);
	
	info("\r\nregister value after reset:\r\n");
	for (i = 0; i < 10; i++)
	{
		info("0x%04x	", *((volatile uint32_t *)(TIMER_BASE + id*0x40+i*4)));
	}

	val = gtimer_get_count(id);
	if (val != 0x0)
	{
		fail++;
	}
	
	fail += timer_check_counter(id, TMR_FIXED);
	gtimer_stop(id);

	if (fail) 
	{
		info("\n%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("\n%s: test pass\n", argv[0]);
	}
	
	return 0;
}

//************************************************************************************************************
// timer frequency division test
//************************************************************************************************************
static int timer_div(int argc, char* argv[])
{
	E_TIMER_PORT id, tmp_id;
	uint32_t pwm_mode;
	E_TMR_PWM_OUTPUT_MODE out_mode;
	uint32_t div, divisor;
	uint32_t load_val;
	uint32_t overflow_counter = 0, second = 0;
	E_TIMER_WXYZ_PORT port_wxyz, tmp_port;

	info("%s: timer clock division test, press 'q' to quit\n", argv[0]);
	info("using timer pwm pin to view the result\n");

	if (argc < 4)
	{
		info("wrong cmd_format: %s pin(w(0)x(1)y(2)z(3)) id div\n", argv[0]);
		return 1;
	}

	port_wxyz = get_arg_ulong(argv[1]);
	if (port_wxyz >= NUM_TIMER_WXYZ_PORTS) 
	{
		info("timer output pin should from 0-3\n");
		return 1;
	}
	
	id = get_arg_ulong(argv[2]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	div = get_arg_ulong(argv[3]);
#ifdef TAPEOUT_V2
	if ((div < 0) || (div > 2047)) 
	{
		info("timer div is should from 0-2047\n");
		return 1;
	}
	divisor = div + 1;
#else
	if ((div < 0) || (div > 8)) 
	{
		info("timer div is should from 0-8\n");
		return 1;
	}
	divisor = 1 << div;
#endif

	// Set pinmux
	misc_timer_output_pin_init(port_wxyz, id, IO_PU_UP, IO_PD_EN);

	pwm_mode = PWM_OV; 		// trigger on overflow only
	out_mode = PWM_TOGGLE; 	// pwm toggle level each overflow

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);
	gtimer_set_div(id, div);
	gtimer_set_pwm_mode(id, pwm_mode, out_mode, 0);

	// set overflow period: 50 us for min div (0: 1/1)
	load_val = 0xffffffff - (TIMER_PCLK/20000) + 1; 
	gtimer_set_load(id, load_val);
	gtimer_set_count(id, load_val);
	gtimer_clear_raw_status(id, 0x7);

	info("load value=0x%x overflow period counter=0x%x\n", load_val, (0xffffffff - load_val));

	gtimer_start(id);
	
	info("timer %d started, please check PWM output\n", id); 
	info("PWM period should be: %d us, timer freq:%dMHZ\n", (100*divisor), TIMER_PCLK);
	info("please judge the test pass or fail according pwm output pin state\n");
	while (1) 
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}
	
		if (gtimer_get_raw_int_status(id) & TMR_INT_OVERFLOW) 
		{
			overflow_counter++;
			
			//if((overflow_counter % 10) == 0)
			//{
			//	info(".");
			//}
			
			gtimer_clear_raw_status(id, 0x7);
		}
		
		if ((overflow_counter * divisor) > 20000) 
		{
			second++;
			info("%d second elapsed\n", second);

			overflow_counter = 0;
		}
	}

	gtimer_stop(id);

	return 0;
}

const int timer_int_vector[8] = {
	INT_TIMER0, INT_TIMER1, INT_TIMER2, INT_TIMER3,
	INT_TIMER4, INT_TIMER5, INT_TIMER6, INT_TIMER7
};

//************************************************************************************************************
// timer interrupt handler
//************************************************************************************************************
volatile static int timer_int_counter = 0;
static int timer_id;
void timer_int_handler(void *ptr)
{
	uint32_t reg_val;

	reg_val = gtimer_get_int_status(timer_id);
	if (reg_val) 
	{
		timer_int_counter++;
		gtimer_clear_int(timer_id, reg_val);
	}
	
	if (reg_val & 0x2)
	{
		info("timer_int_handler:TMR_OVF_INT_STA trig\r\n");
	}	
	else
	{
		info("timer_int_handler,unexpect int 0x%04x\n", reg_val);
	}	
}

//************************************************************************************************************
// timer compare interrupt handler
//************************************************************************************************************
volatile static int g_timer_cmp_val = 0;
void timer_int_cmp_handler(void *ptr)
{
	uint32_t reg_val;
	
	reg_val = gtimer_get_int_status(timer_id);
	if (reg_val) 
	{
		timer_int_counter++;
		gtimer_clear_int(timer_id, reg_val);
	}

	reg_val = gtimer_get_count(timer_id);
	info("timer %d val:%x match to: %x\n", timer_id, reg_val, g_timer_cmp_val*TIMER_PCLK);
	
	gtimer_reload(timer_id);
}	


//************************************************************************************************************
// timer interrupt test
//************************************************************************************************************
static int timer_int(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t load_val;
	E_TIMER_PORT id;
	uint32_t cnt;

	info("timer_int: interrupt handle test\n");

	if (argc < 3) 
	{
		info("wrong cmd_format: %s id trig_value\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	load_val = get_arg_ulong(argv[2]);
	if (load_val > (0xffffffff/TIMER_PCLK) || (load_val == 0)) 
	{
		info("trigger value 1~max %d second can be meet\n", (0xffffffff/TIMER_PCLK));
		return 1;
	}
	info("timer %d: trigger val %d second\n", id, load_val);
	
	timer_id = id;
	timer_int_counter = 0;
	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);
	gtimer_clear_int(id, 0xff);
	gtimer_clear_raw_status(id, 0xff);

	// register int handler and enable it
	irq_generic_install_handler(timer_int_vector[id], timer_int_handler, NULL);
	irq_generic_config_int(timer_int_vector[id], IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(timer_int_vector[id]);

	gtimer_set_int_mask(id, 0x7);

	load_val *= TIMER_PCLK;
	gtimer_set_load(id, 0xffffffff - load_val);
	gtimer_start(id);
	gtimer_reload(id);

	for (cnt = 0; cnt < 0xfffffff; cnt++)
	{
		if (timer_int_counter != 0)
		{
			break;
		}	
	}
	
	if (cnt >= 0xfffffff)
	{
		fail++;
	}
	
	gtimer_stop(id);

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	// disable interrupt
	irq_generic_disable_int(timer_int_vector[id]);
	irq_generic_remove_handler(timer_int_vector[id], NULL, NULL);
	gtimer_set_int_mask(id, 0);

	return 0;
}

//************************************************************************************************************
// timer capture test
//************************************************************************************************************
static int timer_capture(int argc, char* argv[])
{
	E_TIMER_PORT id;
	E_TMR_CAP_MODE mode;
	E_TMR_CAP_EVENT event;
	uint32_t cap1, cap2, timer_cnt, tmp_cap = 0;
	uint32_t freq, cap_cnt = 0;
	uint32_t time_out = 0;

	info("timer_capture: timer capture mode test, press 'q' to quit\n");

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s id mode event\n", argv[0]);
		info("mode: 0:first; 1:second\n");
		info("event: 1:falling edge; 2:rising edge;3:both edge\n");
		return 1;
	}

	// Get the parameters
	id = get_arg_ulong(argv[1]);
	if (id >= TIMER4) 
	{
		info("timer is should from 0-3\n");
		return 1;
	}
	
	mode = get_arg_ulong(argv[2]);
	event = get_arg_ulong(argv[3]);
	if ((event > CAP_BOTH) || (event < CAP_FALLING))
	{
		info("force to be both edge capture\n");
		event = CAP_BOTH;
	}	
	
	info("timer %d: capture %s  for event %s\n",
		id,
		(mode == CAP_MODE_FIRST) ? "first" : "second",
		(event == CAP_FALLING) ? "high to low" : ((event == CAP_RISING) ? "low to high" : "both edge"));

	// Set pinmux to timer input
	info("set pinmux to timer input\n");
	if (!misc_timer_input_pin_init(id, IO_PU_UP, IO_PD_EN))
	{
		return 1;	
	}

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);

	gtimer_start(id);
	info("timer %d started\n", id);

	gtimer_clear_capture(id);
	gtimer_set_capture_mode(id, mode, event);
	
	info("now trigger the event and check console output\n");
	while (1) 
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}
		
		if (gtimer_check_capture_done(id)) 
		{
			gtimer_clear_capture(id);
			gtimer_get_capture_data(id, &cap1, &cap2);

			if (tmp_cap == 0)
			{
				tmp_cap = cap1;
				continue;
			}

			freq = TIMER_PCLK / (cap1 - tmp_cap);
			if (event == CAP_BOTH)
			{
				freq >>= 1;
			}

			info("capture: cap1:0x%x, tmp_cap:0x%x, freq:%dHZ\n", cap1, tmp_cap, freq);

			// check status
			while (!gtimer_check_capture_done(id))
			{
				// delay 10us
				sw_udelay_10x(1);
				if (time_out++ > 3000 * 100)
				{
					info("gtimer check capture timeout\n");
					break;
				}
			}
			gtimer_clear_capture(id);
			gtimer_get_capture_data(id, &cap1, &cap2);

			tmp_cap = 0;			
		}
	}

	gtimer_stop(id);
	info("please judge the test pass or fail according trigger and output\n");

	return 0;
}

//************************************************************************************************************
// timer capture for gpio test
//************************************************************************************************************
static int timer_capture_gpio(int argc, char* argv[])
{
	E_TIMER_PORT id;
	E_TMR_CAP_MODE mode;
	E_TMR_CAP_EVENT event;
	uint32_t cap1, cap2, timer_cnt, tmp_cap = 0;
	uint32_t freq, cap_cnt = 0;
	uint32_t output_port = 0;
	uint32_t output_pin = 0;
	uint32_t output_pin_st = 0;

	info("timer_capture: timer capture mode test, press 'q' to quit\n");

	// Check the validity of parameter count
	if (argc < 6) 
	{
		info("wrong cmd_format: %s id mode event ioport iopin\n", argv[0]);
		info("mode: 0:first; 1:second\n");
		info("event: 1:falling edge; 2:rising edge;3:both edge\n");
		return 1;
	}

	// Get the parameters
	id = get_arg_ulong(argv[1]);
	if (id >= TIMER4) 
	{
		info("timer is should from 0-3\n");
		return 1;
	}
	
	mode = get_arg_ulong(argv[2]);
	event = get_arg_ulong(argv[3]);
	if ((event > CAP_BOTH) || (event < CAP_FALLING))
	{
		info("force to be both edge capture\n");
		event = CAP_BOTH;
	}	

extern E_GPIO_PORT gpio_get_port(char *argv);
	output_port = gpio_get_port(argv[4]);
	output_pin = get_arg_ulong(argv[5]);
	
	info("timer %d: capture %s  for event %s\n",
		id,
		(mode == CAP_MODE_FIRST) ? "first" : "second",
		(event == CAP_FALLING) ? "high to low" : ((event == CAP_RISING) ? "low to high" : "both edge"));

	// Set pinmux to timer input
	info("set pinmux to timer input\n");
	if (!misc_timer_input_pin_init(id, IO_PU_UP, IO_PD_EN))
	{
		return 1;	
	}

	if (!misc_gpio_pin_init(output_port, output_pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
	{
		return 0;
	}
	// output pin initialize
	gpio_init(output_port, GPIO_BASE, GPIO_PCLK, 0);
	gpio_enable_int(output_port, output_pin, GPIO_INT_DIS);
	gpio_clear_int(output_port, output_pin);
	gpio_set_output(output_port, output_pin, 0);
	gpio_set_direction(output_port, output_pin, GPIO_OUTPUT);
	gpio_set_ctrl(output_port, output_pin, GPIO_CTRL_SW);
	//gpio_set_output(output_port, output_pin, 0);	

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);

	gtimer_start(id);
	info("timer %d started\n", id);

	gtimer_clear_capture(id);
	gtimer_set_capture_mode(id, mode, event);
	
	info("now trigger the event and check console output\n");
	while (1) 
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}
		
		if (gtimer_check_capture_done(id)) 
		{
			gtimer_clear_capture(id);
			// output pulse
			output_pin_st = (output_pin_st==0)? 1 : 0;
			gpio_set_output(output_port, output_pin, output_pin_st);					
		}
	}

	gtimer_stop(id);
	info("please judge the test pass or fail according trigger and output\n");

	return 0;
}

//************************************************************************************************************
// timer compare test
//************************************************************************************************************
static int timer_compare(int argc, char *argv[])
{
	E_TIMER_PORT id;
	uint32_t compare_val;
	uint32_t val;
	
	info("timer_compare: timer compare mode test, press 'q' to quit\n");

	if (argc < 3) 
	{
		info("wrong cmd_format: %s id compare_second\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}
	
	compare_val = get_arg_ulong(argv[2]);
	if (compare_val > 0xffffffff/TIMER_PCLK) 
	{
		info("current configure max %d second can be set\n",0xffffffff/TIMER_PCLK);
		return 1;
	}
	info("timer %d: commpare val %d second\n", id, compare_val);

	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);
	gtimer_clear_int(id, 0xff);
	gtimer_clear_raw_status(id, 0xff);	

	// register int handler and enable it
	timer_id = id;
	timer_int_counter = 0;
	g_timer_cmp_val = compare_val;
	irq_generic_install_handler(timer_int_vector[id], timer_int_cmp_handler, NULL);
	irq_generic_config_int(timer_int_vector[id], IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(timer_int_vector[id]);

	gtimer_set_int_mask(id, 0x7);
	
	gtimer_clear_raw_status(id, 0x7);
	gtimer_set_compare_mode(id, 1);
	gtimer_set_compare(id, compare_val*TIMER_PCLK);
	gtimer_set_load(id, 0);
	gtimer_set_count(id, 0);
	
	info("timer %d started\n", id);
	info("now check compare status,reload val=0x%x compare val*TIMER_PCLK=0x%x\n", compare_val*TIMER_PCLK, (compare_val*TIMER_PCLK));

	gtimer_start(id);
	gtimer_reload(id);
	while (1) 
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}

#if 0		
		if (gtimer_get_raw_int_status(id) & TMR_INT_MATCH) 
		{
			val = gtimer_get_count(id);
			
			info("timer %d val:%x match to: %x\n", id,val, compare_val*TIMER_PCLK);
			
			gtimer_clear_raw_status(id, 0x7);
			gtimer_reload(id);
		}
#endif
	}

	gtimer_stop(id);
	info("please judge the test pass or fail according compare val and output\n");

	return 0;
}

//************************************************************************************************************
// timer pwm test
//************************************************************************************************************
static int timer_pwm(int argc, char* argv[])
{
	E_TIMER_PORT id;
	E_TMR_PWM_MODE pwm_mode;
	E_TMR_PWM_OUTPUT_MODE out_mode;
	E_TMR_PWM_POL pwm_pol;
	uint32_t load_val, compare_val;
	E_TIMER_WXYZ_PORT port_wxyz;

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 6) 
	{
		info("wrong cmd_format: %s id pin(w(0)x(1)y(2)z(3)) pwm_mode out_mode pwm_pol\n", argv[0]);
		return 1;
	}

	// Get the parameters
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	port_wxyz = get_arg_ulong(argv[2]);
	if (port_wxyz >= NUM_TIMER_WXYZ_PORTS)
	{
		info("timer output pin is should from 0-3\n");
		return 1;
	}
	
	pwm_mode = get_arg_ulong(argv[3]);
	if ((pwm_mode > PWM_MATCH) || (pwm_mode < PWM_OV)) 
	{
		info("not valid pwm mode, force use PWM_OV(1)\n");
		pwm_mode = PWM_OV;
	}

	out_mode = get_arg_ulong(argv[4]);
	if ((out_mode != PWM_PULSE) && (out_mode != PWM_TOGGLE))
	{
		info("not valid output mode, force use PWM_TOGGLE(1)\n");
		out_mode = PWM_TOGGLE;
	}

	pwm_pol = get_arg_ulong(argv[5]);
	if ((pwm_pol != TMR_PWM_POL_POS) && (pwm_pol != TMR_PWM_POL_NEG))
	{
		info("not valid pwm_pol mode,force use TMR_PWM_POL_NEG(1)\n");
		pwm_pol = TMR_PWM_POL_NEG;
	}

	// Set pinmux
	if (!misc_timer_output_pin_init(port_wxyz, id, IO_PU_UP, IO_PD_EN))
	{
		return 1;
	}
	
	gtimer_init(id, TIMER_BASE);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);
	gtimer_clear_raw_status(id, 0x7);

	load_val = 0xffffffff - 3000;
	compare_val = 0xffffffff - 2000;

	info("load value=%x, compare val=%x, overflow period counter=%x\n", load_val, compare_val, (0xffffffff - load_val));

	if ((pwm_mode == PWM_OV_MATCH) || (pwm_mode == PWM_MATCH)) 
	{
		gtimer_set_compare_mode(id, 1);
		gtimer_set_compare(id, compare_val);
	}
	gtimer_set_pwm_mode(id, pwm_mode, out_mode, pwm_pol);

	info("timer %d: pwm mode: %s, out_mode: %s, pol: %s\n",
		id,
		(pwm_mode == PWM_OV) ? "'overflow'" : ((pwm_mode == PWM_OV_MATCH) ? "'match and overflow'" : "'match'"),
		(out_mode == PWM_PULSE) ? "'pulse'" : "'toggle'",
		(pwm_pol == TMR_PWM_POL_POS) ? "'Positive'" : "'Negative'");

	gtimer_set_load(id, load_val);
	gtimer_set_count(id, load_val);
	gtimer_start(id);
	info("timer %d started\n", id);

	info("now check PWM output\n");
	info("please judge the test pass or fail according pwm output pin(%x) state\n", port_wxyz);
	
	return 0;
}

//************************************************************************************************************
// timer capture for rtc test
//************************************************************************************************************
#define TIMER_RTC_CAP_TIMES 3
int timer_rtc_capture(int argc, char *argv[], uint32_t trim_time, uint32_t *p_timer_ns, uint32_t *p_avg)
{
	E_TIMER_PORT id;
	E_TMR_CAP_MODE mode = CAP_MODE_FIRST;
	E_TMR_CAP_EVENT event = CAP_BOTH;
	uint32_t cap1, cap2, timer_cnt, tmp_cap;
	uint32_t i, loop, diff[TIMER_RTC_CAP_TIMES], avg = 0;
	uint32_t ns, cap_thresh;

	info("timer_capture: timer capture mode test, press 'q' to quit\n");

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s id\n", argv[0]);
		return 1;
	}

	// Get the parameters
	id = get_arg_ulong(argv[1]);
	if (id >= TIMER4) 
	{
		info("timer is should from 0-3\n");
		return 1;
	}	
	
	info("timer %d: capture %s  for event %s\n",
		id,
		(mode == CAP_MODE_FIRST) ? "first" : "second",
		(event == CAP_FALLING) ? "high to low" : ((event == CAP_RISING) ? "low to high" : "both edge"));

	// Convert to ns.
	ns = 1000000000/(TIMER_PCLK/32);

	// get the timer cycles in a trim_time width pulse
	// set threshold to avoid the glitch pulse
	cap_thresh = ((1000000000/ns) * trim_time)/2;
	info("ns:0x%x, cap_thresh:0x%x\n", ns, cap_thresh);

	// Set pinmux to timer input
	info("set pinmux to timer input\n");
	if (!misc_timer_input_pin_init(id, IO_PU_UP, IO_PD_EN))
	{
		return 1;	
	}

	gtimer_init_1(id, TIMER_BASE, TMR_DIV_32);
	gtimer_config_default(id);
	gtimer_set_mode(id, TMR_UP, TMR_AUTO, TMR_USER);

	gtimer_clear_capture(id);
	gtimer_set_capture_mode(id, mode, event);
	gtimer_start(id);

	tmp_cap = 0;
	loop = 0;
	while (1)
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				return 1;
			}
		}
	
		if (gtimer_check_capture_done(id)) 
		{
			gtimer_clear_capture(id);
			gtimer_get_capture_data(id, &cap1, &cap2);

			//info("capture: cap1:0x%x, cap2:0x%x\n", cap1, cap2);

			if (cap1 > tmp_cap)
			{
				info("cap1:0x%08x, tmp_cap:0x%08x, diff:0x%08x\n", cap1, tmp_cap, (cap1-tmp_cap));
				
				if (tmp_cap != 0)
				{
					// get the valid high pulse
					if ((cap1 - tmp_cap) > cap_thresh)
					{
						diff[loop++] = cap1 - tmp_cap;
						if (loop >= TIMER_RTC_CAP_TIMES)
						{
							for (i = 0; i < TIMER_RTC_CAP_TIMES; i++)
							{
								info("diff[%d]:0x%x\n", i, diff[i]);
								avg += diff[i];
							}

							avg /= TIMER_RTC_CAP_TIMES;

							*p_avg = avg;
							info("avg:0x%x\n", avg);
							break;
						}
					}
				}

				tmp_cap = cap1;
			}
			else
			{
	
				tmp_cap = 0;
			}
		}
	}

	gtimer_stop(id);
	timer_restore_system(id);

	*p_timer_ns = ns;
	return 0;
}

int timer_test_cleanup(int sel)
{
	E_TIMER_PORT port;

	info("%s\n", __func__);

	// stop all timer
	for (port = TIMER0; port < NUM_TIMER_PORTS; port++)
	{
		gtimer_stop(port);
	}

	// timer test may change the setting of system timer, restore it after test
	timer_restore_system(SYS_TIMER);
	gtimer_restore_context(ROM_TIMER);

	return 0;
}

static int timer_reg_dump(int argc, char* argv[])
{
	E_TIMER_PORT id;
	
	info("timer_reg_dump for observe regisster\n");

	if (argc < 2) 
	{
		info("wrong cmd_format: %s id(0~7)\n", argv[0]);
		return 1;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id >= NUM_TIMER_PORTS)
	{
		info("timer id should from 0-7\n");
		return 1;
	}

	dump_memory32(TIMER_BASE + (id*0x40), TIMER_REG_LEN_IN_DWORD);
	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry timer_test_menu[] = {
	{"help", 				cmd_help,  			" : Display list of commands"},
	{"h",					cmd_help,			"    : alias for help"},
	{"?",					cmd_help, 			"    : alias for help"},
	{"timer_start_stop",	timer_start_stop,	": timer start/stop test"},
	{"timer_reload",		timer_reload,		": timer reload test"},
	{"timer_count_mode",	timer_count_mode,	": timer count mode test"},
	{"timer_overflow",		timer_overflow,		": timer overflow test"},
	{"timer_reset",			timer_reset,		": timer sw reset test"},
	{"timer_div",			timer_div,			": timer frequency division test"},
	{"timer_int",			timer_int,			": timer interrupt test"},
	{"timer_capture",		timer_capture,		": timer capture mode test"},
	{"timer_capture_gpio",	timer_capture_gpio,	": timer capture mode test with gpio"},	
	{"timer_compare",		timer_compare,		": timer compare mode test"},
	{"timer_pwm",			timer_pwm,			": timer pwm mode test"},
	{"timer_reg_dump",		timer_reg_dump,		": dump timer register"},	
	{0, 					0, 					0}
};

int timer_test_prepare(int sel)
{
	gtimer_save_context(ROM_TIMER);
	return 0;
}

#define TIMER_LOAD_DEF_VAL	0x00000000
bool timer_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_TIMER_PORT port;
	struct gtimer *priv = p_gtimer;
	bool ret = true;

	gtimer_save_context(ROM_TIMER);

	for (port = TIMER0; port < NUM_TIMER_PORTS; port++)
	{
		info("%s: port:%d\n", __func__, port);
		gtimer_init(port, TIMER_BASE);

		info("write load reg\n");
		
		// write register
		gtimer_write(priv, port, TMR_REG_LOAD, *p_wr_val);
		
		info("read&compare load reg\n");
		*p_rd_val = gtimer_read(priv, port, TMR_REG_LOAD);
		if (*p_rd_val != *p_wr_val)
		{
			ret = false;
			goto end0;
		}
		
		// reset
		info("timer reset\n");
		prcm_reset(ARM_TIMER_RST);
		
		// read register
		info("read&compare load reg\n");
		*p_rd_val = gtimer_read(priv, port, TMR_REG_LOAD);

		if (*p_rd_val != TIMER_LOAD_DEF_VAL)
		{
			ret = false;
			goto end0;
		}
	}

end0:
	// timer test may change the setting of system timer and bootrom timer, restore it after test
	timer_restore_system(SYS_TIMER);
	gtimer_restore_context(ROM_TIMER);
	return ret;
}

bool timer_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_TIMER; 
	E_TIMER_PORT port;
	struct gtimer *priv = p_gtimer;
	bool ret = true;
	gtimer_save_context(ROM_TIMER);

	for (port = TIMER0; port < NUM_TIMER_PORTS; port++)
	{
		info("%s: port:%d\n", __func__, port);

		// clock gate enable
		info("clock gate enable\n");
		prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
		gtimer_init(port, TIMER_BASE);

		// write register
		info("write load reg\n");
		gtimer_write(priv, port, TMR_REG_LOAD, *p_wr_val);
		
		info("read&compare load reg\n");
		*p_rd_val = gtimer_read(priv, port, TMR_REG_LOAD);
		if (*p_rd_val != *p_wr_val)
		{
			ret = false;
			goto end0;
		}
	
		// clock gate disable
		info("clock gate disable\n");
		prcm_blk_cg(blk, ARM_BLK_CG_DIS);

		// write register
		info("write load reg\n");
		gtimer_write(priv, port, TMR_REG_LOAD, (*p_wr_val+1));

		// clock gate enable
		info("clock gate enable\n");
		prcm_blk_cg(blk, ARM_BLK_CG_EN);

		// read register
		info("read&compare load reg\n");
		*p_rd_val = gtimer_read(priv, port, TMR_REG_LOAD);
		if (*p_rd_val != *p_wr_val)
		{
			ret = false;
			goto end0;
		}
	}

end0:
	// timer test may change the setting of system timer and bootrom timer, restore it after test
	timer_restore_system(SYS_TIMER);
	gtimer_restore_context(ROM_TIMER);
	return ret;
}
#else
bool timer_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool timer_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}
#endif

