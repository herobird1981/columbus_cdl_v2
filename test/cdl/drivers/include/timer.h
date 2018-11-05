/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TIMER_H__
#define __TIMER_H__

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define SYS_TIMER	7
#define ROM_TIMER	0

//#define SYSTEM_TIMER_FROM_IPC

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
int gtimer_timer_init(unsigned long base,unsigned long pclk,unsigned long id);
int ipc_timer_init(unsigned long base, unsigned long pclk);
unsigned long long get_ticks(void);
unsigned long long get_timer(unsigned long long base);
void udelay(unsigned long usec);
void mdelay(unsigned long msec);
void sdelay(unsigned long sec);

#endif

