/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"

#if (CONFIG_PLL_TEST == 1)
#include "cdl_pll.h"

cmdline_entry pll_test_menu[];

extern struct brite_clk_pll *p_pll;

int g_min_err_rate_of_100k = 0;
int g_min_err_rate_div = 0;

#define UART_TUNE_ONLY_ONE_PORT_WORK 0

unsigned long uart_tune_clk_div(unsigned long input_rate, unsigned long baud) {
#if UART_TUNE_ONLY_ONE_PORT_WORK
	unsigned long int_div;
	unsigned long frac_div;
	unsigned long best_div = -1;
	unsigned long uart_clk;
	unsigned long baud_clk;
	unsigned long clk_diff;
	float min_err_rate = 1000.0;
	float err_rate;
	unsigned long max_baud_div;
	unsigned long baud_div;

	baud *= 16;
	max_baud_div = input_rate / baud;
	max_baud_div = min(max_baud_div, 0xFFFFUL);

	for (baud_div = max_baud_div; baud_div > 0; baud_div--) {
		int_div = input_rate/baud_div/baud;
		baud_clk = baud_div*baud;
		/* int_div means great err, not acceptable */
		if (int_div <= 2 || int_div >= 8)
			continue;
		for (frac_div = 0; frac_div < 32; frac_div++) {
			uart_clk = input_rate/(int_div*32 + frac_div)*32;
			clk_diff = uart_clk > baud_clk ? uart_clk - baud_clk : baud_clk - uart_clk;
			err_rate =  clk_diff * 1000 / (float)baud_clk;
			if (err_rate < min_err_rate) {
				min_err_rate = err_rate;
				best_div = (frac_div << 3) | int_div;
			}
			if (err_rate == 0)
				goto found;
		}
	}
error:
	if (min_err_rate > 30) {
		g_min_err_rate_of_100k = min_err_rate * 100;
		return -1;
	}

found:
	g_min_err_rate_of_100k = min_err_rate * 100;
	g_min_err_rate_div = best_div;

	return best_div;
#else
	return 0x1d; /* not support 3M*/
#endif

}

int uart_tune_clk_for_baud(struct clk *clk, unsigned long baud)
{
	unsigned long uart_src_clk_rate;
	unsigned long uart_clk_div;

	console_flush();

	uart_src_clk_rate = clk_get_parent_rate(clk);
	uart_clk_div = uart_tune_clk_div(uart_src_clk_rate, baud);
	if(uart_clk_div != -1)
		brite_set_uart_clk_div(uart_clk_div);
	console_reinit_baud(clk->get_rate(clk));

	return 0;
}

int console_pclk_change_cb(struct clk *clk) {
	uart_tune_clk_for_baud(clk, console_baud());
}

extern int timer_init(unsigned long base,unsigned long pclk,unsigned long id);
int system_timer_pclk_change_cb(struct clk *clk) {
	int ret = 0;
	ret = gtimer_timer_init(TIMER_BASE, TIMER_PCLK, SYS_TIMER);
	ret |= ipc_timer_init(A7_IPC_REG_BASE, ARM_AHB_CLK);
	return ret;
}


/*
command process functions
*/
static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&pll_test_menu[0]);
}

/**
 * @brief pll_freq_src - arm or ddr pll get and set rate test
 * @param pll_out input, out pll rate
 * @param src_clk input, pll source, 'xtal' or 'rosc25'
 * @param pll_type input, pll type, 0:arm pll 1:ddr pll
 *
 * @return directly if failed
 */
static int pll_freq_src(int argc, char* argv[])
{
	struct clk *src_clk;
	unsigned long pll_now, pll_out, pll_actual;
	unsigned int pll_type;
	int fail = 0;
	unsigned long rosc_freq_mhz = clk_get_rate(&root_rosc_100mhz)/1000000;

	if (argc < 4) {
		info("wrong cmd_format: %s pll_out src_clk pll_type\n"
			 "pll_out: max 1000M\n"
			 "src_clk:",
			 argv[0]);
		print_clk_name(CLK_IS_ARM_PLL_SRC);
		info("\n");
		info("rosc freq: %lu\n", rosc_freq_mhz);
		info("pll_type: 0 for arm, 1 for ddr\n");
		return 0;
	}
	pll_out = get_arg_ulong(argv[1]);
	src_clk = clk_name_to_clk(argv[2], CLK_IS_ARM_PLL_SRC);
	pll_type = get_arg_ulong(argv[3]);
	if (/* (pll_out < 500000000) || */(pll_out > 1000000000)) {
		info("invalid parameters:pll_out should 500M-1000M,set %u\n", pll_out);
		return 0;
	}

	if (!src_clk) {
		info("wrong src clk\n");
		return 0;
	}
	
	if ((pll_type != 0) && (pll_type != 1)) {
		info("invalid parameters:pll_type should 0 or 1,set %u\n", pll_type);
		return 0;
	}
	info("input parameters: pll_out=%u, xin_type=%s, pll_type=%u\n", pll_out, src_clk->name, pll_type);
	info("ROSC accuracy variant is +/-10%%, if used, UART output may be abnormal\n");
	if (rosc_freq_mhz != 100)
	{
		if (src_clk->name == root_rosc_25mhz.name)
		{
			info("!!! WARNING: Current ROSC100 clock is not trimmed, acutal freq is %dMHz. The test would be skipped!!\n", rosc_freq_mhz);
			return 0;
		}
	}
	if(pll_type == DDR_PLL_TYPE) {  /*if modify ddr, the cdl will crash*/
		info("Change DDR PLL not supported yet. CDL May crash!\n");
		return 0;
	}

	info("From now, will set pll...\n");
	/*4. set pll_out rate*/
	if (pll_type == ARM_PLL_TYPE) {
		clk_set_parent(&arm_pll, src_clk);
		if (clk_set_rate(&arm_pll, pll_out)) {
			info("set arm pll rate error\n");
			fail++;
			goto end;
		}
	} else {
#if 0
		if (brite_ddr_pll_set_rate(pll_out, xin_type)) {
			info("set ddr pll rate error\n");
			fail++;
			goto end;
		}
#endif
	}

	info("\n*** New clock rate set! ***\n\n");

	/*5. get now pll clock rate again*/
	if (pll_type == ARM_PLL_TYPE) {
		pll_now = clk_get_rate(&arm_pll);
	} else {
		pll_now = brite_ddr_pll_get_rate();
	}
	if (pll_now == ULONG_MAX) {
		info("get pll rate again error\n");
		fail++;
		goto end;
	}
	info("New pll_now=%u\n", pll_now);
	if (pll_now != pll_out)
		fail++;
	brite_get_all_clock_rate();

#if 0
	if (pll_type == ARM_PLL_TYPE) {
		clk_set_parent(&arm_pll, root_xtal_act);
		if (clk_set_rate(&arm_pll, ARM_PLL_DEFAULT_CLK)) {
			fail++;
			goto end;
		}
	} else {
		if (brite_ddr_pll_set_rate(DDR_PLL, PLL_SRC_FROM_MCLK)) {
			info("set ddr pll rate error\n");
			fail++;
			goto end;
		}
	}
	info("\nset ARM pll back to default!\n");
	brite_get_all_clock_rate();
#endif
end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief arm_clk_switch - switch arm clk source: ARM PLL or ROSC
 * @param src input, arm clock source, should be 'arm_pll' or 'rosc100'
 *
 * @return directly if failed
 */
static int arm_clk_switch(int argc, char* argv[])
{
	struct clk *src_clk;
	unsigned long pll_now, arm_clk_rate;
	unsigned long ref_clk;
	unsigned int src_type;
	int fail = 0;

	if (argc < 2) {
		info("wrong cmd_format: %s src\n"
			 "src:",argv[0]);
		print_clk_name(CLK_IS_ARM_CLK_SRC);
		info("\n");
		return 0;
	}
	src_clk = clk_name_to_clk(argv[1], CLK_IS_ARM_CLK_SRC);
	if (!src_clk) {
		info("wrong src clk name %s\n", argv[1]);
		return -1;
	}
	info("\nSwitch src clk from %s to %s\n", arm_clk.parent->name, src_clk->name);

	/*2. get now pll clock rate*/
	pll_now = clk_get_rate(&arm_pll);
	if (pll_now == ULONG_MAX) {
		info("get pll rate error\n");
		goto fail;
	}
	info("get pll_now %lu\n", pll_now);

	/*3. get now arm_clk rate*/
	arm_clk_rate = clk_get_rate(&arm_clk);
	if (arm_clk_rate == ULONG_MAX) {
		info("get arm clk rate error\n");
		goto fail;
	}
	info("get arm_clk_rate %lu\n", arm_clk_rate);

	/* wait for uart buffer sent out */
	mdelay(10);

	/*4. switch arm clk source*/
	if (clk_set_parent(&arm_clk, src_clk)) {
		info("switch arm clk source error\n");
		goto fail;
	}

	/*5. get now arm_clk rate*/
	arm_clk_rate = clk_get_rate(&arm_clk);
	if (arm_clk_rate == ULONG_MAX) {
		info("again get arm clk rate error\n");
		goto fail;
	}
	info("again get arm_clk_rate %lu\n", arm_clk_rate);

	ref_clk = clk_get_rate(src_clk);
	if(arm_clk_rate != ref_clk)
		fail++;

	goto end;

fail:
	fail++;
end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

static int arm_child_rate_div(int argc, char* argv[], unsigned long filter, unsigned long max_div)
{
	struct clk *bus_clk;
	struct clk *parent_clk;
	unsigned long bus_clk_rate;
	unsigned long parent_clk_rate;
	unsigned long bus_clk_div;
	unsigned int src_type;
	int fail = 0;

	src_type = 0;

	if (argc < 3) {
		info("wrong cmd format: %s clk_name div\n", argv[0]);
		info("clk_name: ");
		print_clk_name(filter);
		info("\n");
		info("div: 1-%d, the clock div\n", max_div);
		return -1;
	}
	bus_clk = clk_name_to_clk(argv[1], filter);
	parent_clk = bus_clk->parent;
	bus_clk_div = get_arg_ulong(argv[2]);
	if (!bus_clk) {
		info("wrong bus clk name\n");
		return -1;
	}
	if (bus_clk_div == 0 || bus_clk_div > max_div) {
		info("div %lu is not in range\n", bus_clk_div);
		return -1;
	}

	/*2. get now bus & parent clock rate*/
	bus_clk_rate = clk_get_rate(bus_clk);
	if (bus_clk_rate == ULONG_MAX) {
		info("get bus_clk_rate error\n");
		goto fail;
	}
	parent_clk_rate = clk_get_rate(parent_clk);
	if (parent_clk_rate == ULONG_MAX) {
		info("get parent_clk_rate error\n");
		goto fail;
	}
	info("Testing %s\n", bus_clk->name);
	info("get bus_clk_rate %lu, parent_clk_rate %lu, current_div %lu\n",
			bus_clk_rate, parent_clk_rate, parent_clk_rate/bus_clk_rate);
	info("target bus_clk_rate %lu, if > 300MHz, system may hang up!\n\n", parent_clk_rate/bus_clk_div);
	mdelay(10);
	/*3. set bus_clk_rate*/
	if (clk_set_rate(bus_clk, parent_clk_rate/bus_clk_div)) {
		info("set bus_clk_rate %lu error\n", parent_clk_rate/bus_clk_div);
		goto fail;
	}
	
	/*4. get now bus clock rate*/
	bus_clk_rate = bus_clk_rate = clk_get_rate(bus_clk);
	if (bus_clk_rate == ULONG_MAX) {
		info("get new bus_clk_rate error\n");
		goto fail;
	}
	info("get new bus_clk_rate %lu\n", bus_clk_rate);
	if (bus_clk_rate != parent_clk_rate/bus_clk_div)
		fail++;

	goto end;

fail:
	fail++;
end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief bus_rate_div - bus clk rate divide test
 * @param clk_name input, bus clk name:axi|ahb|apb0|apb1|rom
 * @param div input, prescalers from bus clock
 *
 * @return directly if failed
 */
static int bus_rate_div(int argc, char* argv[]) {
	return arm_child_rate_div(argc, argv, CLK_IS_BUS, 1<<2);
}

/**
 * @brief periph_rate_div - periph clk rate divide test
 * @param clk_name input, periph name:sdmmc|nfc
 * @param div input, prescalers from arm_clk
 *
 * @return directly if failed
 */
static int periph_rate_div(int argc, char* argv[]) {
	return arm_child_rate_div(argc, argv, CLK_IS_PERIPH, 1<<4);
}

/**
 * @brief clk_status - show all clock status and freq
 */
static int clk_status(int argc, char* argv[]) {
	brite_get_all_clock_rate();
	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry pll_test_menu[] = {
	{ "help",cmd_help,	" : Display list of commands" },
	{ "h",	cmd_help,	"    : alias for help" },
	{ "?",	cmd_help,	"    : alias for help" },
	{ "pll_freq_src", pll_freq_src,"  : pll set and get test" },
	{ "arm_clk_switch", arm_clk_switch,"  : switch arm clk source: ARM PLL or ROSC" },
	{ "bus_rate_div", bus_rate_div,"  : bus clk rate divide test" },
	{ "periph_rate_div", periph_rate_div,"  : periph clk rate divide test" },
	{ "clk_status", clk_status, "  : get all clock status and freq" },
	{ "q",	NULL,	" : quit pll test" },{ 0,0,0 }
};

#endif
