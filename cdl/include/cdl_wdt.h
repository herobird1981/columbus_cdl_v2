/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_WDT_H__
#define __CDL_WDT_H__

/*register offset*/
#define WDT_LDR (0x00)
#define WDT_CRR (0x04)
#define WDT_TCR (0x08)
#define WDT_PLR (0x0c)
#define WDT_CNT (0x10)
#define WDT_ISR (0x14)

#define WDT_CNT_RESTART_VALUE 0x76

#define WDT_TIMEOUT_64K 	0
#define WDT_TIMEOUT_32K 	1
#define WDT_TIMEOUT_16K 	2
#define WDT_TIMEOUT_8K		3
#define WDT_TIMEOUT_4K		4
#define WDT_TIMEOUT_2K		5
#define WDT_TIMEOUT_1K		6

#define WDT_ACTION_RESET	0
#define WDT_ACTION_INTERRUPT	1

#define WDT_RST_LEN_DEFAULT 	0xffff

struct brite_wdt {
	void* regs;
	unsigned long pclk;
	unsigned long freq;
	unsigned long div;

	unsigned long reset_pulse_len;
	unsigned long action;
	unsigned long timeout_range;
	long timeout_ms;
	unsigned long load_val;
};

extern struct brite_wdt* p_wdt;

/*apis*/
int wdt_init(unsigned long base, unsigned long pclk, unsigned long div);
void wdt_set_options(unsigned long rst_len, unsigned long action,
	unsigned long timeout, unsigned long load_val);
void wdt_start(void);
void wdt_stop(void);
void wdt_restart(void);
int wdt_is_enabled(void);
unsigned long wdt_get_counter(void);
int wdt_get_int_status(void);
void wdt_clear_int_status(void);

#endif
