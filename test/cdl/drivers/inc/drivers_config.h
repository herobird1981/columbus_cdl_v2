/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __DRIVER_CONFIG_H__
#define __DRIVER_CONFIG_H__

#include "drivers_types.h"
#include "console.h"
#include "timer.h"

#include "errno.h"

/*timer select*/
#define CONFIG_USE_A9_GLOBAL_TIMER	0

/*console select*/
#define CONFIG_USE_A9_CONSOLE		0
#define CONFIG_USE_HW_CONSOLE		1

#define CONFIG_SYS_NAND_ONFI_DETECTION

#define DEBUG_OUTPUT
#ifdef 	DEBUG_OUTPUT
#define debug(...)	console_printf (__VA_ARGS__)
#else
#define debug(...)
#endif
#define info(...)	console_printf(__VA_ARGS__)

/*dependence on  cdl */

#define ARCH_DMA_MINALIGN	64

/*dependence on external */
#include "stdarg.h"
extern void* memcpy(void*, const void*, unsigned long);
extern void* memset(void*, int, unsigned long);
extern int memcmp(const void*, const void*, unsigned long);
extern int vscnprintf(char* buf, unsigned long size, const char* fmt,
	va_list args);
extern int sprintf(char* buf, const char* fmt, ...);
extern unsigned long long cb_lldiv(unsigned long long dividend,
	unsigned long divisor);
extern char *strim(char *s);
extern int strncmp(const char * cs,const char * ct,unsigned long count);
#endif
