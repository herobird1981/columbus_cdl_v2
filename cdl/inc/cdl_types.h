/*---------------------------------------------------------------------------------------------------*/
/*                                                                                               											 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.               							 */
/*                                                                                               											 */
/*---------------------------------------------------------------------------------------------------*/

#ifndef __CDL_TYPES_H__
#define __CDL_TYPES_H__
#include "stdint.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
//typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;
//typedef unsigned long size_t;
typedef unsigned int uint;

typedef int	bool;
typedef int	BOOL;

#ifndef NULL
#define NULL	0
#endif
#define true	1
#define false	0

typedef u32 dma_addr_t;
#define ULONG_MAX	(~0UL)

#define __raw_writeb(v,a)	(*(volatile u8 *)(a) = ((u8)v))
#define __raw_writew(v,a)	(*(volatile u16 *)(a) = ((u16)v))
#define __raw_writel(v,a)	(*(volatile u32 *)(a) = ((u32)v))
#define __raw_readb(a)	(*(volatile u8 *)(a))
#define __raw_readw(a)	(*(volatile u16 *)(a))
#define __raw_readl(a)	(*(volatile u32 *)(a))

#define readl	__raw_readl
#define writel	__raw_writel
#define readw	__raw_readw
#define writew	__raw_writew
#define readb	__raw_readb
#define writeb	__raw_writeb

#define typeof	__typeof__
#define __aligned(x)	__attribute__((aligned(x)))

// memory operation definition
#define REG32(addr)   		(*((volatile uint32_t *)(addr)))
#define MEM32(addr)   		(*((uint32_t *)(addr)))

#define REG16(addr)   		(*((volatile uint16_t *)(addr)))
#define MEM16(addr)  		(*((uint16_t *)(addr)))

#define REG8(addr)    		(*((volatile uint8_t *)(addr)))
#define MEM8(addr)    		(*((uint8_t *)(addr)))

#define TRUE		1
#define FALSE		0

// byte operation
#define get_16_bits_val(d) ((d[1] << 8) | (d[0] << 0)) 
#define get_32_bits_val(d) ((d[3] << 24) | (d[2] << 16) | (d[1] << 8) | (d[0] << 0)) 

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
extern int abs(int x);

#define BIT(nr)	(1UL << (nr))
#define ROUND(a,b)	(((a) + (b) - 1) & ~((b) - 1))
#define DIV_ROUND_UP(n,d)	(((n) + (d) - 1) / (d))
#define DIV_ROUND_HALF_UP(n,d)	(((n) + (d)/2) / (d))

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#define __iomem

#endif

