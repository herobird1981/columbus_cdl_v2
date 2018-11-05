/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_epf.h"
#include "cdl_usb.h"

#if (CONFIG_EPF_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
cmdline_entry epf_test_menu[];

// for interrupt
static volatile uint32_t g_epf_int_cnt = 0;
static volatile uint32_t g_hs_keeping_st = ARM_PWD_HS_KEEPING_PENDING;
static volatile uint32_t g_sw_delay_ms = 0;
static volatile uint32_t g_failsafe_delay_ms = 0;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&epf_test_menu[0]);
}

void epf_int_handler(void *ptr)
{
	uint32_t reg_val;

    irq_generic_disable_int(INT_PMU);
	reg_val = prcm_pmu_int_st_read();
    if(reg_val & ARM_PMU_INT_ST_MASK)
    {
		g_epf_int_cnt++;
		
		prcm_pmu_int_st_clr();
	}

	reg_val = prcm_pwd_read();
	info("epf_int_handler, pwd reg=0x%x\n", reg_val);

	if (g_hs_keeping_st) 
	{
		info("waiting sw finish......\n");
#ifdef TAPEOUT_V2
		/*disable USB auxilary*/
		usb_aux_vdt_disable((void *)MISC_BASE, MUSB_USB0);
		usb_aux_vdt_disable((void *)MISC_BASE, MUSB_USB1);
#endif
		if (g_failsafe_delay_ms > g_sw_delay_ms)
		{
			info("sw finished normal, epf housekeeping done finish\n");	
		}

		mdelay(g_sw_delay_ms);

		prcm_pwd_hs_keeping_ctrl(ARM_PWD_HS_KEEPING_DONE);
	}
	else
	{
		info("epf housekeeping pending, waiting for failsafe timer expired\n");
		prcm_pwd_hs_keeping_ctrl(ARM_PWD_HS_KEEPING_PENDING);
	}	
}

// testcase:epf interrupt handle test
static int epf_test(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t reg_val;
	uint32_t hs_keeping_done_def;

	info("ARM %s\n", __func__);

	info("epf_test start:\n");
	info("first please switch the EPF_IN pin to GND\n");
	info("then observe the display info which show the interrupt event\n");
	info("finally re-switch EPF_IN pin to 3.3v, expected system reset and boot again!\n");
	info("cmd format: %s hs_keeping_done_st(def) hs_keeping_done_st fail_safe_delay_ms sw_delay_ms\n", argv[0]);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s hs_keeping_done_st(def) hs_keeping_done_st fail_safe_delay_ms sw_delay_ms\n", argv[0]);
		info("hs_keeping_done_st(def): 0: pending, 1: done\n");
		return 1;
	}

	// Get the parameters
	hs_keeping_done_def = get_arg_ulong(argv[1]);
	if (hs_keeping_done_def > ARM_PWD_HS_KEEPING_DONE)
	{
		hs_keeping_done_def = ARM_PWD_HS_KEEPING_DONE;
	}	
	
	g_hs_keeping_st = get_arg_ulong(argv[2]);
	if (g_hs_keeping_st > ARM_PWD_HS_KEEPING_DONE)
	{
		g_hs_keeping_st = ARM_PWD_HS_KEEPING_DONE;
	}
	
	g_failsafe_delay_ms = get_arg_ulong(argv[3]);
	if (g_failsafe_delay_ms > ARM_FAILSAFE_TIMER_MAX)
	{
		g_failsafe_delay_ms = ARM_FAILSAFE_TIMER_MAX;
		info("failsafe timer max should be 0x%x", ARM_FAILSAFE_TIMER_MAX);
	}
	
	g_sw_delay_ms = get_arg_ulong(argv[4]);

	// pinmux
	misc_host_low_power_out_pin_init(IO_PU_UP, IO_PD_EN);
	misc_epf_failsafe_timer_out_pin_init(IO_PU_UP, IO_PD_EN);
	misc_sleep_mode_done_pin_init(IO_PU_UP, IO_PD_EN);

	// epf cofigure
	prcm_fail_safe_timer_init(g_failsafe_delay_ms);
	prcm_pwd_hs_keeping_ctrl(hs_keeping_done_def);
	prcm_pmu_int_st_clr();

	// register int handler and enable it	
	g_epf_int_cnt = 0;
	irq_generic_install_handler(INT_PMU, epf_int_handler, NULL);
	irq_generic_config_int(INT_PMU, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_PMU);

	reg_val = prcm_pwd_read();
	info("pwd reg: 0x%x\n", reg_val);

	if (hs_keeping_done_def == ARM_PWD_HS_KEEPING_DONE)
	{
		info("power state machine will enter power down transition state and then enter EPF sleep mode\n");
		info("please observe the ARM_EPF_SLEEP_MODE_DONE pin to check the result\n");
	}
	
	info("press 'q' to quit before epf interrupt occur\n");
	while (1) 
	{
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}
		
		if (g_epf_int_cnt)
		{
			break;
		}	
	}
	
	if (g_epf_int_cnt == 0) 
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
	irq_generic_disable_int(INT_PMU);
	irq_generic_remove_handler(INT_PMU,NULL,NULL);

	return 0;
}

void epf_int_handler_noks(void *ptr)
{
	uint32_t reg_val;

	//reg_val = prcm_pmu_int_st_read();
    //if(reg_val & ARM_PMU_INT_ST_MASK)
    {
		g_epf_int_cnt++;
		// disable interrupt
		irq_generic_disable_int(INT_PMU);
		//irq_generic_remove_handler(INT_PMU,0, 0);
		prcm_pmu_int_st_clr();
	}
    prcm_pwd_hs_keeping_ctrl(ARM_PWD_HS_KEEPING_DONE);
	//info("epf_int_handler\n");
}

static int epf_test_ISR_no_hs(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t reg_val;
	uint32_t hs_keeping_done_def;

	info("ARM %s\n", __func__);

	info("epf_test start:\n");
	//IVC_verify();
#if 0
	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s hs_keeping_done_st(def) hs_keeping_done_st fail_safe_delay_ms sw_delay_ms\n", argv[0]);
		info("hs_keeping_done_st(def): 0: pending, 1: done\n");
		return 1;
	}
#endif
	// Get the parameters
	hs_keeping_done_def = get_arg_ulong(argv[1]);
#if 0
	if (hs_keeping_done_def > ARM_PWD_HS_KEEPING_DONE)
	{
		hs_keeping_done_def = ARM_PWD_HS_KEEPING_DONE;
	}	
#endif	
	g_hs_keeping_st = get_arg_ulong(argv[2]);
#if 0
	if (g_hs_keeping_st > ARM_PWD_HS_KEEPING_DONE)
	{
		g_hs_keeping_st = ARM_PWD_HS_KEEPING_DONE;
	}
#endif	
	g_failsafe_delay_ms = get_arg_ulong(argv[3]);
#if 0
	if (g_failsafe_delay_ms > ARM_FAILSAFE_TIMER_MAX)
	{
		g_failsafe_delay_ms = ARM_FAILSAFE_TIMER_MAX;
		info("failsafe timer max should be 0x%x", ARM_FAILSAFE_TIMER_MAX);
	}
#endif	
	g_sw_delay_ms = get_arg_ulong(argv[4]);

	// pinmux
	//misc_host_low_power_out_pin_init(IO_PU_UP, IO_PD_EN);
	//misc_epf_failsafe_timer_out_pin_init(IO_PU_UP, IO_PD_EN);
	//misc_sleep_mode_done_pin_init(IO_PU_UP, IO_PD_EN);

	// epf cofigure
	prcm_fail_safe_timer_init(0);
	prcm_pwd_hs_keeping_ctrl(0);
	prcm_pmu_int_st_clr();

	reg_val = prcm_pwd_read();
	info("pwd reg: 0x%x\n", reg_val);
#if 0
	if (hs_keeping_done_def == ARM_PWD_HS_KEEPING_DONE)
	{
		info("PSW -> PD -> EPF sleep mode\n");
		info("PLS check ARM_EPF_SLEEP_MODE_DONE PIN\n");
	}
#endif	
	info("press 'q' to quit\n");
	// register int handler and enable it	
	g_epf_int_cnt = 0;
	irq_generic_install_handler(INT_PMU, epf_int_handler_noks, 0);
	irq_generic_config_int(INT_PMU, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_PMU);	
	
	while (1) 
	{
#if 0	
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}
#endif		
		if (g_epf_int_cnt)
		{
			break;
		}	
	}
#if 0	
	if (g_epf_int_cnt == 0) 
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
#else
	info("%s: test pass\n", argv[0]);
#endif
	// disable interrupt
	//irq_generic_disable_int(INT_PMU);
	//irq_generic_remove_handler(INT_PMU,0, 0);

	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry epf_test_menu[] = {
	{"help",	cmd_help,	" : Display list of commands"},
	{"h",		cmd_help,	"    : alias for help"},
	{"?",		cmd_help,	"    : alias for help"},
	{"epf_test",epf_test,	"  : epf trigger test"},
	{"epf_test_simple",epf_test_ISR_no_hs,	"  : epf simple trigger test"},	
	{0,			0,			0}
};

#endif

