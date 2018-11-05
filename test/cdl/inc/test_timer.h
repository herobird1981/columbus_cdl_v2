/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_TIMER_H__
#define __TEST_TIMER_H__
#include "test_config.h"
#include "cdl_timer.h"

#if (CONFIG_TIMER_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry timer_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
int timer_test_prepare(int sel);

int timer_test_cleanup(int sel);

int timer_rtc_capture(int argc, char *argv[], uint32_t trim_time, uint32_t *p_timer_ns, uint32_t *p_avg);

#else
#define timer_test_menu 		NULL
#define timer_test_prepare 		NULL
#define timer_test_cleanup 		NULL
#endif


bool timer_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

bool timer_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif
