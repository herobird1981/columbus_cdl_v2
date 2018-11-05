/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "drivers_config.h"
#include "cdl_timer.h"
#include "cdl_ipc.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
/* will overflow after 0xFFFFFFFF/PCLK seconds * divisor */
/* 75M pclk about 57 seconds * 300 ~= 285 minutes */
/* 50M pclk about 85 seconds * 300 ~= 425 minutes */
/* 48M pclk about 89 seconds * 300 ~= 445 minutes */
#ifdef TAPEOUT_V2
#define GTIMER_PRE_SCALER	299          /*divisor = GTIMER_PRE_SCALER + 1*/
#else
#define GTIMER_PRE_SCALER	8          /*divisor = 1 << GTIMER_PRE_SCALER*/
#endif

#define CTIMER_US_PER_SEC			1000000
#define CTIMER_MS_PER_SEC			1000

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
// counter elapsed for 1ms
unsigned long gtimer_cycles_per_ms = 0;

// for 32 bit timer, one high 32 bit to acculate when overflow
unsigned long tick_high = 0;
unsigned long tick_id;

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
int gtimer_timer_init(unsigned long base,unsigned long pclk,unsigned long id)
{
	tick_id = id;
	tick_high = 0;
	gtimer_init(id, base);
	gtimer_config_default(id);
	
	// configure near (pclk/1)M
	gtimer_set_div(id, GTIMER_PRE_SCALER); // 1/1
	gtimer_clear_raw_status(id, 0x7);
#ifdef TAPEOUT_V2
	gtimer_cycles_per_ms = pclk/(GTIMER_PRE_SCALER + 1)/CTIMER_MS_PER_SEC;
#else
	gtimer_cycles_per_ms = pclk/(1UL<<GTIMER_PRE_SCALER)/CTIMER_MS_PER_SEC;
#endif
	gtimer_start(id);

	return 0;
}

// get timer counter
unsigned long long gtime_get_ticks(void)
{
	unsigned long nowl, nowu;

	nowl = gtimer_get_count(tick_id);	
	if (gtimer_get_raw_int_status(tick_id) & TMR_INT_OVERFLOW) 
	{
		tick_high +=1;

		do {
			gtimer_clear_raw_status(tick_id,TMR_INT_OVERFLOW);
			nowl = gtimer_get_count(tick_id);
		} while ((gtimer_get_raw_int_status(tick_id) & TMR_INT_OVERFLOW) || (nowl == 0xFFFFFFFF));

		info("overflow: tick_high %u nowl %u\n", tick_high, nowl);
	}

	nowu = tick_high;
	return (((unsigned long long)nowu) << 32) | nowl;
}


struct a7_ipc ipc_timer_dev;
struct a7_ipc* p_ipc_timer_dev = &ipc_timer_dev;
unsigned long ipc_timer_cycles_per_ms = 0;
unsigned long ipc_timer_cycles_per_us = 0;

int ipc_timer_init(unsigned long base, unsigned long pclk) {
	p_ipc_timer_dev->reg_base = (void *) base;
	ipc_timer_cycles_per_ms = pclk/CTIMER_MS_PER_SEC;
	ipc_timer_cycles_per_us = pclk/CTIMER_US_PER_SEC;   // how many cycles per ms
	return 0;
}

unsigned long long ipc_get_tick(void) {
	return ipc_get_time(p_ipc_timer_dev);
}

#ifndef SYSTEM_TIMER_FROM_IPC
extern unsigned long timer_cycles_per_ms __attribute__ ((alias ("gtimer_cycles_per_ms")));
unsigned long long get_ticks(void) __attribute__ ((alias ("gtime_get_ticks")));
#else
extern unsigned long timer_cycles_per_ms __attribute__ ((alias ("ipc_timer_cycles_per_ms")));
unsigned long long get_ticks(void) __attribute__ ((alias ("ipc_get_tick")));
#endif

// return interval by ms
unsigned long long get_timer(unsigned long long base)
{
	unsigned long long nowtime = get_ticks();

	return cb_lldiv(nowtime, timer_cycles_per_ms) - base;
}

// delay us
void udelay(unsigned long usec)
{
	unsigned long long endtime  = (unsigned long long)usec * timer_cycles_per_ms;

	endtime = cb_lldiv(endtime, 1000UL);

	endtime += get_ticks();

	while (get_ticks() < endtime)
	{}
}

//delay ms
void mdelay(unsigned long msec)
{
	while (msec--) 
	{
		udelay(1000);
	}
}
/*delay seconds*/
void sdelay(unsigned long sec)
{
	unsigned long long endtime = sec * timer_cycles_per_ms * 1000;

	gtimer_reload(SYS_TIMER);
	endtime += get_ticks();
	while (get_ticks() < endtime);
}
