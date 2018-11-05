/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __TEST_TYPES_H__
#define __TEST_TYPES_H__

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long ulong;
typedef unsigned long long u64;
typedef int  bool;
#define NULL	0
typedef ulong lbaint_t;

#define __raw_writeb(v,a)	(*(volatile u8 *)(a) = ((u8)v))
#define __raw_writew(v,a)	(*(volatile u16 *)(a) = ((u16)v))
#define __raw_writel(v,a)	(*(volatile u32 *)(a) = ((u32)v))
#define __raw_readb(a)	(*(volatile u8 *)(a))
#define __raw_readw(a)	(*(volatile u16 *)(a))
#define __raw_readl(a)	(*(volatile u32 *)(a))

#define readl	__raw_readl
#define writel	__raw_writel

#define typeof __typeof__
#define __aligned(x)		__attribute__((aligned(x)))

#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#define lower_32_bits(n) ((u32)(n))

#define min(a, b) (((a) < (b)) ? (a) : (b))

#endif
