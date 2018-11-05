/*---------------------------------------------------------------------------------------------------*/
/*                                                                                               											 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.               							 */
/*                                                                                               											 */
/*---------------------------------------------------------------------------------------------------*/

#ifndef __CDL_CONFIG_H__
#define __CDL_CONFIG_H__

#include "cdl_types.h"
#include "cdl_errno.h"
#include "hw_maps.h"

#define DEBUG_OUTPUT
#ifdef 	DEBUG_OUTPUT
#define debug(...)	console_printf (__VA_ARGS__)
#else
#define debug(...)
#endif
#define info(...)	console_printf(__VA_ARGS__)

/*dma ddr workaroud*/
#define dma_virt_to_phy(x)	(x)

/*dependence apis*/
extern unsigned long long get_timer(unsigned long long start);
extern int console_printf(const char* fmt, ...);
extern void* memcpy(void*, const void*, unsigned int);
extern void* memset(void*, int, unsigned int);
extern void udelay(unsigned long usec);
//extern void *malloc(unsigned int size);
//extern void free(void* p);
#endif

