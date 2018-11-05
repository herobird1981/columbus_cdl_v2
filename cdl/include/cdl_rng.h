/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_RNG_H__
#define __CDL_RNG_H__

#define RNG_CTRL		(0x0)
#define RNG_LATCH2DDR	(0x4)
#define RNG_LATCH2BUS	(0x8)
#define RNG_LATCH_STS   (0xc)
#define RNG_RN0			(0x10)
#define RNG_RN1			(0x14)
#define RNG_RN2			(0x18)
#define RNG_RN3			(0x1c)

struct brite_rng {
	void* regs;
	unsigned int latch_mode;
};

extern struct brite_rng* p_rng;

/*apis*/
void rng_init(unsigned long base, int latch_mode);
void rng_power_ctrl(int mode);
void rng_latch2ddr(void);
int rng_rand(void);
void rng_rand_buffer(void* buf, int len);

#endif

