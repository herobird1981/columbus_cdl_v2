/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/

#include "cdl_config.h"
#include "cdl_rng.h"
#include "cdl_sys.h"

#define RNG_TIMEOUT (1000) //ms

struct brite_rng rng_dev;
struct brite_rng* p_rng = &rng_dev;

void rng_init(unsigned long base, int latch_mode)
{
	p_rng->regs = (void*)base;
	p_rng->latch_mode = latch_mode & 0x1;

#ifdef TAPEOUT_V2
	misc_trng_sel(0);
#endif	
	writel((latch_mode&0x1)<<1, (u32)p_rng->regs + RNG_CTRL);
}

void rng_power_ctrl(int mode)
{
	unsigned int reg = readl((u32)p_rng->regs + RNG_CTRL);
	if (mode)
		writel(reg | 0x1, (u32)p_rng->regs + RNG_CTRL);
	else
		writel(reg & 0x2, (u32)p_rng->regs + RNG_CTRL);
	udelay(2000);
}
void rng_latch2ddr(void)
{
	writel(1, (u32)p_rng->regs + RNG_LATCH2DDR);
}

int rng_rand(void)
{
	writel(1, (u32)p_rng->regs + RNG_LATCH2BUS);
	return readl((u32)p_rng->regs + RNG_RN0);
}

#if 0
static unsigned FindOneInNumber(unsigned int x)
{
    const unsigned MASK1  = 0x55555555;
    const unsigned MASK2  = 0x33333333;
    const unsigned MASK4  = 0x0f0f0f0f;
    const unsigned MASK8  = 0x00ff00ff;
    const unsigned MASK16 = 0x0000ffff;

    x = (x&MASK1 ) + (x>>1 &MASK1 );
    x = (x&MASK2 ) + (x>>2 &MASK2 );
    x = (x&MASK4 ) + (x>>4 &MASK4 );
    x = (x&MASK8 ) + (x>>8 &MASK8 );
    x = (x&MASK16) + (x>>16&MASK16);
    return x;
}
#endif

#if 0
static unsigned long long fun(unsigned long long x) 
{
	unsigned long long = 0;
	int i = 0;
	for(i = 0; i < 8; i++) {
		val += x & 0x0101010101010101L;
		x >>= 1;
	}
	val += (val >> 32); 
	val += (val >> 16); 
	val += (val >> 8); 
	return val & 0xFF;
}
#endif

//#define MONITOR_RNG_DATA

void rng_rand_buffer(void* buf, int len)
{
	u8* ptr = (u8*)buf;
	int blen = 0;
	u32 sts = 0;
#ifdef MONITOR_RNG_DATA	
    u64 ticks = 0L;
#endif	
	blen = 16;
	unsigned long long start = 0;

	while (len > 0) {
#ifdef MONITOR_RNG_DATA			
		ticks = get_ticks();
#endif
		if (len < 16)
			blen = len;

		start = get_timer(0);
		if(p_rng->latch_mode == 1) {
			while((readl((u32)p_rng->regs + RNG_LATCH_STS) & 0x1) ==0) {
				if(get_timer(start) > RNG_TIMEOUT) {
					info("rng_rand_buffer check RNG_LATCH_STS timeout\n");
					return ;
				}
			}
		} else
			writel(1, (u32)p_rng->regs + RNG_LATCH2BUS);
		memcpy(ptr, (void*)((u32)p_rng->regs + RNG_RN0), blen);
		//clear latch status bit
		if(p_rng->latch_mode == 1)
	    writel(1, (u32)p_rng->regs + RNG_LATCH_STS);		
		ptr += blen;
		len -= blen;
#ifdef MONITOR_RNG_DATA			
		ticks = get_ticks() - ticks;
		info("ticks per round:%d\n", ticks);
#endif
	}
}
