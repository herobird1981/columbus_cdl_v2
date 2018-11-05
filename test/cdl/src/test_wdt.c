/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_wdt.h"
#include "cdl_pll.h"

#if (CONFIG_WDT_TEST == 1)
#include "cdl_wdt.h"

cmdline_entry wdt_test_menu[];

extern struct brite_wdt* p_wdt;

#define WDT_DEFAULT_DIVISOR		0x92 /* pclk/1024/4, 50MHz ~=> 12.2KHz, 75Mhz ~=> 18.3KHz */

/*mode: 0  expected decrease counter
	1  expected fixed counter
return: 0 success >0 fail
*/
static int wdt_check_counter(int mode)
{
	unsigned long counter;
	unsigned long prev_counter;
	int fail = 0;
	int i;
	unsigned long long start;
	unsigned long long ms_elapsed;
	unsigned long high, low;

	start = get_timer(0);
	prev_counter = wdt_get_counter();
	for (i = 0; i < 10; i++) {
		udelay(1000);
		counter = wdt_get_counter();
		ms_elapsed = get_timer(start);
		high = (ms_elapsed >> 32);
		low = (ms_elapsed & 0xFFFFFFFF);
		if (high)
			info("time: 0x%x_%x ms, counter: 0x%lx\n", high, low, counter);
		else
			info("time: %ld ms, counter: 0x%lx\n", low, counter);
		/*request wdt counter should decrease*/
		if (mode == 0) {
			if (counter >= prev_counter) {
				fail++;
			}
		}
		/*request wdt counter should keep*/
		else if (mode == 1) {
			if (counter != prev_counter) {
				fail++;
			}
		}
		prev_counter = counter;
	}
	if (fail) {
		if (mode == 0) {
			info(
				"wdt_check_counter fail for counter not decrease\n");
		} else if (mode == 1) {
		  	info(
		  		"wdt_check_counter fail for counter still change\n");
		}
	}
	return fail;
}

/*timeout:	wait timeout seconds for reset generated
result:  0 success >0 fail
*/
static int wdt_check_reset_generated(unsigned long timeout)
{
	unsigned long long start,end;
	unsigned long high, low;
	unsigned long counter;
	int fail = 0;

	start = get_timer(0);
	info("please test the rst output pin \n");
	do {
		end = get_timer(start);
		counter = wdt_get_counter();
		/*had stoped by reset */
		if (counter >= 0xffffffff - 0x1000) {
			break;
		}
		/*had stoped by reset*/
		if (!wdt_is_enabled()) {
			break;
		}
	} while (end < timeout * 1000);
	if (end < (timeout)*1000) {
		high = (end >> 32);
		low = (end & 0xFFFFFFFF);
		if (high)
			info("reset generate at 0x%x_%x ms\n", high, low);
		else
			info("reset generate at %d ms\n", low);
	} else {
		info("no reset generated after %d seconds\n", timeout);
		fail++;
	}
	return fail;
}

/*
command process functions
*/

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&wdt_test_menu[0]);
}

/*testcase:wdt start/stop test*/
static int wdt_start_stop(int argc, char* argv[])
{
	int fail = 0;

	info("%s: wdt start/stop test\n",argv[0]);
	wdt_init(WDT_BASE, WDT_PCLK, WDT_DEFAULT_DIVISOR);

	/*start timer and check counter*/
	wdt_start();
	info("wdt started\n");
	fail += wdt_check_counter(0);

	/*stop timer and check counter*/
	wdt_stop();
	info("wdt stoped\n");
	fail += wdt_check_counter(1);

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/*
test case: wdt load value test
test case: wdt restart test
cmd format: wdt_reload start|restart load_val
*/
static int wdt_reload(int argc, char* argv[])
{
	unsigned long counter = 0;
	unsigned long load_val;
	int fail = 0;
	unsigned long long start, end;
	unsigned long high, low;
	int i = 0;

	start = get_timer(0);

	info("%s: wdt reload test\n",argv[0]);
	if (argc < 3) {
		info("wrong cmd_format: %s start|restart load_val\n",argv[0]);
		return 0;
	}

	load_val = get_arg_ulong(argv[2]);
	info("cmd: %s load_val 0x%lx\n", argv[1], load_val);

	wdt_init(WDT_BASE, WDT_PCLK, WDT_DEFAULT_DIVISOR);
	wdt_set_options(WDT_RST_LEN_DEFAULT,
		WDT_ACTION_RESET,
		WDT_TIMEOUT_64K,
		load_val);

	/* start timer and check counter */
	end = get_timer(start);
	high = (end >> 32);
	low = (end & 0xFFFFFFFF);
	if (high)
		info("time: 0x%x_%x ms, wdt started\n", high, low);
	else
		info("time: %ld ms, wdt started\n", low);
	wdt_start();

	if (!strcmp(argv[1], "start")) {
		counter = wdt_get_counter();
		if (load_val - counter < 0x5 || load_val - counter > 0x1000) {
			info("Fail: loal_val %lx counter 0x%lx\n", load_val, counter);
			fail++;
		}
		for(i=0; i < 5; i++) {
			mdelay(100);
			counter = wdt_get_counter();
			end = get_timer(start);
			high = (end >> 32);
			low = (end & 0xFFFFFFFF);
			if (high)
				info("time: 0x%x_%x ms, current counter 0x%lx\n", high, low, counter);
			else
				info("time: %ld ms, current counter 0x%lx\n", low, counter);
		}
	} else if (!strcmp(argv[1], "restart")) {
		counter = wdt_get_counter();
		end = get_timer(start);
		high = (end >> 32);
		low = (end & 0xFFFFFFFF);
		if (high)
			info("time 0x%x_%x ms, init counter 0x%lx\n", high, low, counter);
		else
			info("time %ld ms, init counter 0x%lx\n", low, counter);
		for(i=0; i < 5; i++) {
			mdelay(100);
			counter = wdt_get_counter();
			end = get_timer(start);
			high = (end >> 32);
			low = (end & 0xFFFFFFFF);
			if (high)
				info("time 0x%x_%x ms, counter after delay 0x%lx\n", high, low, counter);
			else
				info("time %ld ms, counter after delay 0x%lx\n", low, counter);
		}
		wdt_restart();
		counter = wdt_get_counter();
		if (load_val - counter > 0x1000) {
			info("Fail: loal_val %lx counter 0x%lx\n", load_val, counter);
			fail++;
		}
		for(i=0; i < 5; i++) {
			mdelay(100);
			counter = wdt_get_counter();
			end = get_timer(start);
			high = (end >> 32);
			low = (end & 0xFFFFFFFF);
			if (high)
				info("time: 0x%x_%x ms, counter after reload: 0x%lx\n", high, low, counter);
			else
				info("time: %ld ms, counter after reload: 0x%lx\n", low, counter);
		}
	} else
		info("invalid sub command %s\n",argv[1]);

	/*stop timer and check counter*/
	wdt_stop();
	info("wdt stoped\n");

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/*test case:wdt generate reset test*/
static int wdt_reset(int argc, char* argv[])
{
	unsigned long reset_len;
	unsigned long timeout;
	long timeout_ms, ms_elapsed;
	unsigned long load_val;
	int fail = 0;
	unsigned long long start;
	unsigned long long cur_time;
	unsigned long high, low;

	info("%s: wdt reset test\n",argv[0]);
	if (argc < 3) {
		info("wrong cmd_format: %s reset_len timeout\n",argv[0]);
		return 0;
	}
	reset_len = get_arg_ulong(argv[1]);
	timeout = get_arg_ulong(argv[2]);

	if (reset_len >0xffff) {
		info("max reset pulse lenth: 0xffff exceed,use the max\n");
		reset_len = 0xffff;
	}
	info("cmd: reset_len=0x%lx timeout=%ld s\n", reset_len, timeout);

	wdt_init(WDT_BASE, WDT_PCLK, WDT_DEFAULT_DIVISOR);

	/* cnt=cyc_per_s*timeout */
	load_val = p_wdt->freq * timeout; /* n second to generate reset */
	if(load_val/p_wdt->freq != timeout) {
		info("!!! WARNING: load_val overflow! timeout won't be accurate!\n");
	}
	timeout_ms = cb_lldiv((uint64_t)load_val*1000, p_wdt->freq);
	wdt_set_options(reset_len, WDT_ACTION_RESET, WDT_TIMEOUT_64K, load_val);

	/* start timer and check counter */
	info("wdt started\n");
	wdt_start();

	/*when watch dog reset,system restart, so this case can't execute to end*/
	info("reset will occur after %ld ms, cdl test will restart\n",timeout_ms);
	start = get_timer(0);
	ms_elapsed = 1;
	fail = 1;
	do {
		cur_time = get_timer(start);
		if (cur_time > ms_elapsed) {
			high = (cur_time >> 32);
			low = (cur_time & 0xFFFFFFFF);
			if (high)
				info("time elapse 0x%x_%x ms\n", high, low);
			else
				info("time elapse %d ms\n", low);
			ms_elapsed+=500;
			ms_elapsed = min(ms_elapsed, timeout_ms-200);
		}
		if(console_check_char('q'))
			break;
	} while (cur_time < (timeout_ms+1000));

	/*fail += wdt_check_reset_generated(timeout + 1);*/

	wdt_stop();

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
		info("adjust test pass/fail according result of viewing the rst pin\n");
	}
	return 0;
}

/*testcase:wdt frequency division test*/
static int wdt_div(int argc, char* argv[])
{
	unsigned long divisor;
	unsigned long div1, div2;
	unsigned long load_val;
	int fail = 0;
	unsigned long div_act;
	unsigned long start;
	unsigned long long cur_time;
	long ms_elapsed;
	unsigned long start_counter;
	unsigned long current_counter;
	unsigned long prev_counter;
	long timeout_ms;
	unsigned long high, low;

	info("%s: wdt clock division test\n",argv[0]);
	if (argc < 3) {
		info("wrong cmd_format: %s div1 div2\n"
				"div1: 0-13, div2: 0-14\n"
				"actual div = 2^(div1+1) * 2^div2\n", argv[0]);
		return 0;
	}
	div1 = get_arg_ulong(argv[1]);
	div2 = get_arg_ulong(argv[2]);
	if ((div1 > 13) || (div2>14)) {
		info("invalid parameters:div1(0-13) div2(0-14),set %ld %ld\n",
			div1,div2);
		return 0;
	}
	divisor = (div1<<4)|div2;
	div_act = (1<<(div1+1))*(1<<div2);

	/* init divdor: user case should calculate this divisor carefully */
	wdt_init(WDT_BASE, WDT_PCLK, divisor);

	/* second to generate reset if set min divisor(1/2)*/
	/* wdt_clk=abp1_clk/div_act; cyc_per_s = wdt_clk; cnt=cyc_per_s*timeout */
	load_val = p_wdt->freq * 5;
	if (load_val == 0)
		load_val = 5;
	wdt_set_options(WDT_RST_LEN_DEFAULT,
		WDT_ACTION_RESET,
		WDT_TIMEOUT_64K,
		load_val);

	/* start timer and check current_counter */
	info("wdt started\n");
	wdt_start();

	/*when watch dog reset,system restart, so this case can't execute to end*/
	info("div1(high div bits) %ld div2(low div bits) %ld, actual_div %lu actual_freq %lu, load_val %lu\n",
		div1, div2, div_act, p_wdt->freq, load_val);
	timeout_ms = cb_lldiv((uint64_t)load_val * 1000, p_wdt->freq);
	info("reset will occur after %d ms, cdl test will restart\n", timeout_ms);
	start = get_timer(0);
	ms_elapsed = 1;
	fail = 1;
	start_counter = wdt_get_counter();
	prev_counter = start_counter;
	info("current current_counter %lu\n", current_counter);
	do {
		cur_time = get_timer(start);
		if (cur_time >= ms_elapsed) {
			current_counter = wdt_get_counter();
			high = (cur_time >> 32);
			low = (cur_time & 0xFFFFFFFF);
			if (high)
				info("time elapse 0x%x_%x ms, counter %lu, counter diff %lu, wdt time elapsed %lu ms\n",
						high, low, current_counter, prev_counter - current_counter,
						cb_lldiv((uint64_t)(start_counter - current_counter) * 1000, p_wdt->freq));
			else
				info("time elapse %ld ms, counter %lu, counter diff %lu, wdt time elapsed %lu ms\n",
						low, current_counter, prev_counter - current_counter,
						cb_lldiv((uint64_t)(start_counter - current_counter) * 1000, p_wdt->freq));
			prev_counter = current_counter;
			ms_elapsed += 500;
			ms_elapsed = min(ms_elapsed, timeout_ms-200);
		}
		if(console_check_char('q'))
			break;
	} while (cur_time < 30 * 1000);

	/*fail += wdt_check_reset_generated(30);*/

	wdt_stop();

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
		info("adjust test pass/fail according result of viewing the rst pin\n");
	}
	return 0;
}

/*testcase:wdt interrupt generate test*/
static int wdt_interrupt_timeout(int argc, char* argv[])
{
	unsigned long div1, div2;
	unsigned long divisor;
	unsigned long timeout_range;
	unsigned long start_counter;
	unsigned long prev_counter;
	unsigned long current_counter;
	unsigned long load_val;
	unsigned long long start;
	unsigned long long cur_time;
	unsigned long long ms_elapsed;
	unsigned long high, low;
	int int_status;
	int fail = 0;

	info("%s: wdt interrupt timeout test,you can press 'q' to quit when read second\n",argv[0]);
	if (argc < 4) {
		info("wrong cmd_format: %s div1 div2 timeout_range\n"
				"div1: 0-13, div2: 0-14\n"
				"timeout_range: 0-6, timeout counter = (2^timeout_range)*1000", argv[0]);
		return 0;
	}
	div1 = get_arg_ulong(argv[1]);
	div2 = get_arg_ulong(argv[2]);
	if ((div1 > 13) || (div2>14)) {
		info("invalid parameters: %ld %ld, limites are: div1(0-13) div2(0-14)\n",
			div1, div2);
		return 0;
	}
	divisor = (div1<<4)|div2;

	timeout_range = get_arg_ulong(argv[3]);
	if (timeout_range > 6) {
		info("invalid timeout config, force using max 6\n");
		timeout_range = 6;
	}
	wdt_init(WDT_BASE, WDT_PCLK, divisor);

	/* 1 second to generate reset*/
	load_val = p_wdt->freq * 1;
	wdt_set_options(WDT_RST_LEN_DEFAULT,
		WDT_ACTION_INTERRUPT,
		6 - timeout_range,
		load_val);

	info("wdt freq %ld action %s timeout_range %ld timeout %ld ms reset_len %ld ticks load_val %ld\n",
			p_wdt->freq, p_wdt->action ? "irq" : "reset", timeout_range, p_wdt->timeout_ms, p_wdt->reset_pulse_len, load_val);
	info("reset will occur after %ld ms, %lu counter, cdl test will restart\n", p_wdt->timeout_ms, (1<<timeout_range)*1024);
	/* start timer and check counter */
	info("wdt started\n");
	wdt_start();
	start = get_timer(0);

	int_status = wdt_get_int_status();
	if (int_status)
		info("int status = %x should be 0\n", int_status);

	/* check interrupt generated, int status */
	do {
		ms_elapsed = get_timer(start);
		int_status = wdt_get_int_status();
		if (int_status)
			break;
	} while (ms_elapsed < 3 * 1000);

	start_counter = wdt_get_counter();
	prev_counter = start_counter;
	start = get_timer(0);
	info("counter %lu when irq generated\n", prev_counter);

	if (ms_elapsed < 3 * 1000) {
		low = (ms_elapsed & 0xFFFFFFFF);
		info("interrupt generate at %ld ms\n", low);
	} else {
		info("no interupt generated after long time\n");
		fail++;
	}

	/*wait interrupt timeout reset occur,
	when watch dog reset,system restart,
	so this case can't execute to ms_elapsed
	*/
	ms_elapsed = min(1, p_wdt->timeout_ms);
	do {
		cur_time = get_timer(start);
		if (cur_time > ms_elapsed) {
			current_counter = wdt_get_counter();
			high = (cur_time >> 32);
			low = (cur_time & 0xFFFFFFFF);
			if (high)
				info("time elapse 0x%x_%x ms, counter %lu counter elapsed %lu, wdt time elapsed %lu ms\n",
						high, low, current_counter, prev_counter - current_counter,
						cb_lldiv((uint64_t)(start_counter - current_counter) * 1000, p_wdt->freq));
			else
				info("time elapse %d ms, counter %lu counter elapsed %lu, wdt time elapsed %lu ms\n",
						low, current_counter, prev_counter - current_counter,
						cb_lldiv((uint64_t)(start_counter - current_counter) * 1000, p_wdt->freq));
			ms_elapsed += 500;
			ms_elapsed = min(ms_elapsed, p_wdt->timeout_ms-200);
		}
		if(console_check_char('q'))
			break;
		int_status = wdt_get_int_status();
		if (!int_status) {
			break;
		}
	} while (cur_time < (p_wdt->timeout_ms + 20 * 1000));

	wdt_stop();

	info("%s: test fail, reset not generated.\n",argv[0]);

	return 0;
}

static int wdt_int_counter = 0;
void wdt_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = wdt_get_int_status();
	if (int_status) {
		wdt_int_counter++;
		wdt_clear_int_status();
		wdt_restart();
	}
	info("wdt_int_handler,status=%x\n", int_status);
}

/*testcase:wdt interrupt handle test*/
static int wdt_int(int argc, char* argv[])
{
	int fail = 0;
	unsigned long load_val;
	int i = 0;

	info("%s: wdt interrupt handle test\n",argv[0]);

	/*register int handler and enable it*/
	irq_generic_install_handler(INT_WDT, wdt_int_handler, NULL);
	irq_generic_config_int(INT_WDT, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_WDT);

	/* init divdor: 0x92-->1/4096 bus freq, 12207 */
	wdt_init(WDT_BASE, WDT_PCLK, WDT_DEFAULT_DIVISOR);

	/*1 second to generate reset if set min divisor(1/2)*/
	load_val = p_wdt->freq;
	wdt_set_options(WDT_RST_LEN_DEFAULT,
		WDT_ACTION_INTERRUPT,
		WDT_TIMEOUT_64K,
		load_val);

	/* start timer and check counter */
	info("wdt started\n");
	wdt_int_counter = 0;
	wdt_start();

	while(!wdt_int_counter && i++ < 20)
		mdelay(100); /* delay 2 second for interrupt msg output */
	if (wdt_int_counter == 0)
		fail++;

	wdt_stop();

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	/*disable interrupt*/
	irq_generic_disable_int(INT_WDT);
	irq_generic_remove_handler(INT_WDT,NULL,NULL);

	return 0;
}

/*
on the fly read/write register when watch dog timer running
*/
static int wdt_fly(int argc, char* argv[])
{
	unsigned long counter = 0;
	unsigned long load_val;
	unsigned long new_load_val;
	int i;

	info("%s: wdt on-the-fly test\n", argv[0]);

	load_val = 0x40000000;
	wdt_init(WDT_BASE, WDT_PCLK, WDT_DEFAULT_DIVISOR);
	wdt_set_options(WDT_RST_LEN_DEFAULT,
		WDT_ACTION_RESET,
		WDT_TIMEOUT_64K,
		load_val);

	/* start timer and check counter */
	info("wdt started, loadval 0x%lx\n",load_val);
	wdt_start();

	for (i=0;i<10;i++) {
		counter = wdt_get_counter();
		info("counter: 0x%lx\n",counter);
	}
	wdt_restart();
	info("reload\n");
	for (i=0;i<10;i++) {
		counter = wdt_get_counter();
		info("counter: 0x%lx\n",counter);
	}

	new_load_val = load_val + 0x20000000;
	info("set new load val and reload, loadval=0x%lx\n", new_load_val);
	writel(new_load_val, (unsigned long)p_wdt->regs + WDT_LDR);
	wdt_restart();
	for (i=0;i<10;i++) {
		counter = wdt_get_counter();
		info("counter: 0x%lx\n", counter);
	}

	/*stop timer and check counter*/
	wdt_stop();
	info("wdt stoped\n");

	if(counter > load_val)
		info("%s: test pass\n",argv[0]);
	else
		info("%s: test fail\n",argv[0]);

	return 0;
}

#define WDT_REG_LEN_IN_DWORD 6
static int wdt_reg_dump(int argc, char* argv[])
{
	info("wdt_reg_dump for observe register\n");

	dump_memory32(WDT_BASE, WDT_REG_LEN_IN_DWORD);
	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry wdt_test_menu[] = {
	{ "help",cmd_help,	" : Display list of commands" },
	{ "h",	cmd_help,	"    : alias for help" },
	{ "?",	cmd_help,	"    : alias for help" },
	{ "wdt_start_stop",wdt_start_stop,"  : start/stop test" },
	{ "wdt_reload",wdt_reload,"  : wdt load value and restart test" },
	{ "wdt_reset",wdt_reset,"  : wdt generate reset test" },
	{ "wdt_div",wdt_div,	"  : wdt frequency division test" },
	{ "wdt_interrupt_timeout",wdt_interrupt_timeout,"  : wdt interrupt timeout test" },
	{ "wdt_fly",wdt_fly,"  : wdt on-the-fly read/write test" },
	{ "wdt_int",wdt_int,	"  : wdt interrupt handle test" },
	{"wdt_reg_dump",		wdt_reg_dump,		": dump wdt register"},
	{ "q",	NULL,	" : quit wdt test" },{ 0,0,0 }
};

int wdt_test_prepare(int sel)
{
	info("%s\n", __func__);
	
	// Set pinmux
	misc_wdt_rstn_pin_init();

	return 0;
}


#define WDT_PLR_DEF_VAL	0xFF
bool wdt_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s:\n", __func__);
	wdt_init(WDT_BASE, WDT_PCLK, 0x0);

	// write register
	info("write load reg\n");

	__raw_writel(*p_wr_val, ((uint32_t)p_wdt->regs + WDT_PLR));
	
	info("wdt&compare plr reg\n");
	
	*p_rd_val = __raw_readl((uint32_t)p_wdt->regs + WDT_PLR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("wdt reset\n");
	prcm_reset(ARM_WDT_RST);
	
	// read register
	info("read&compare plr reg\n");
	
	*p_rd_val = __raw_readl((uint32_t)p_wdt->regs + WDT_PLR);
	if (*p_rd_val != WDT_PLR_DEF_VAL)
	{
		return false;
	}
	
	return true;
}

bool wdt_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_WDT;
	
	info("%s:\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	wdt_init(WDT_BASE, WDT_PCLK, 0x0);

	// write register
	info("write load reg\n");
	__raw_writel(*p_wr_val, ((uint32_t)p_wdt->regs + WDT_PLR));
	
	info("wdt&compare plr reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_wdt->regs + WDT_PLR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write load reg\n");
	__raw_writel((*p_wr_val+1), ((uint32_t)p_wdt->regs + WDT_PLR));

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare plr reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_wdt->regs + WDT_PLR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	return true;
}
#else
bool wdt_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool wdt_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

