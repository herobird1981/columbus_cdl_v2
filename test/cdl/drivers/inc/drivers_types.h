/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __DRIVERS_TYPES_H__
#define __DRIVERS_TYPES_H__

typedef char int8_t;
typedef unsigned char u8;
typedef unsigned char __u8;
typedef unsigned char u_char;
typedef unsigned char uint8_t;
typedef unsigned char uchar;

typedef unsigned short u16;
typedef unsigned short ushort;
typedef unsigned short uint16_t;
typedef unsigned short __le16;

typedef unsigned long u32;
typedef unsigned long __u32;
typedef unsigned long uint32_t;
typedef unsigned long u_long;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long  __le32;

typedef unsigned long long u64;
typedef unsigned long long uint64_t;
typedef unsigned long long __u64;

typedef unsigned long size_t;
typedef long long loff_t;
typedef long ssize_t;

typedef int	bool;
#define NULL	0

#define false 	0
#define true	1

#define __raw_writeb(v,a)	(*(volatile u8 *)(a) = ((u8)v))
#define __raw_writew(v,a)	(*(volatile u16 *)(a) = ((u16)v))
#define __raw_writel(v,a)	(*(volatile u32 *)(a) = ((u32)v))
#define __raw_readb(a)		(*(volatile u8 *)(a))
#define __raw_readw(a)		(*(volatile u16 *)(a))
#define __raw_readl(a)		(*(volatile u32 *)(a))

#define readl	__raw_readl
#define writel	__raw_writel
#define readw	__raw_readw
#define writew	__raw_writew
#define readb	__raw_readb
#define writeb	__raw_writeb

#define typeof	__typeof__
#define __aligned(x)	__attribute__((aligned(x)))
#define __user
#define __iomem
#define __packed	__attribute__((packed))

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define ALIGN(x,a)		__ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)	(((x)+(mask))&~(mask))
#define __always_inline	inline

#define __swab32(x) \
	((u32)( \
		(((u32)(x) & (u32)0x000000ffUL) << 24) | \
		(((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
		(((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
		(((u32)(x) & (u32)0xff000000UL) >> 24) ))
#define __be32_to_cpu(x) __swab32(x)
#define cpu_to_le16(x)		(x)
#define le16_to_cpu(x)		(x)
#define le32_to_cpu(x)		(x)

#define LOG2(x) (((x & 0xaaaaaaaa) ? 1 : 0) + ((x & 0xcccccccc) ? 2 : 0) + \
		 ((x & 0xf0f0f0f0) ? 4 : 0) + ((x & 0xff00ff00) ? 8 : 0) + \
		 ((x & 0xffff0000) ? 16 : 0))

#define BUG() do { \
	info("CDL BUG at %s:%d!\n", __FILE__, __LINE__); \
} while (0)
#define WARN_ON(x) if (x) {debug("WARNING in %s line %d\n" \
				  , __FILE__, __LINE__); }
#define BUG_ON(condition) do { if (condition) BUG(); } while(0)

#define min_t(type, x, y) min(x,y)
#define max_t(type, x, y) max(x,y)

#define spin_lock_init(lock) do {} while (0)
#define spin_lock(lock) do {} while (0)
#define spin_unlock(lock) do {} while (0)
#define init_waitqueue_head(...)	do { } while (0)
struct mutex { int i; };

#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

#define MAX_ERRNO	4095
#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)
static inline void *ERR_PTR(long error)
{
	return (void *) error;
}

static inline long PTR_ERR(const void *ptr)
{
	return (long) ptr;
}

static inline long IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

/*
 * ffs: find first bit set. This is defined the same way as
 * the libc and compiler builtin ffs routines, therefore
 * differs in spirit from the above ffz (man ffs).
 */

static inline int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}
static inline unsigned int hweight8(unsigned int w)
{
	unsigned int res = (w & 0x55) + ((w >> 1) & 0x55);
	res = (res & 0x33) + ((res >> 2) & 0x33);
	return (res & 0x0F) + ((res >> 4) & 0x0F);
}


#endif
