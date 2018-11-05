/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef unsigned char u8;
typedef unsigned long long u64;
typedef		unsigned long long u_int64_t;
typedef		unsigned long long uint64_t;

typedef signed long long s64;
typedef signed long long int64_t;

typedef long long loff_t;

#ifndef NULL
#define NULL						0
#endif

#define notrace __attribute__((no_instrument_function))

#define INT_MAX		((int)(~0U>>1))

#endif