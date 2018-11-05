#include <pka_types.h>

struct rng_regs {
	u32 micro_pd;
	u32 latch2ddr_en;
	u32 latch2bus;
	u32 reserved1;
	u32 rn[4];
};

static volatile struct rng_regs * rng_regs;

void brite_prng_set_reg_base(volatile void *reg) {
	rng_regs = reg;
}

void rng_pon(void){
	rng_regs->micro_pd = 1;
	mdelay(2);
	rng_regs->micro_pd =0;
	mdelay(2);
}

static u32 rng_rns[4];
static u32 unused_rns_in_bytes = 0;
void rng_read2local(void) {
	int i=0;
	for(i=0; i<4; i++)
		rng_rns[i] = rng_regs->rn[i];
	unused_rns_in_bytes = 16;
}

unsigned char rng_readb(void) {
	if(!unused_rns_in_bytes){
		rng_read2local();
	}
	return ((unsigned char *)rng_rns)[16-(unused_rns_in_bytes--)];
}

u32 rng_readl(void) {
	u32 ret;
	if(unused_rns_in_bytes<4){
		rng_read2local();
	}
	ret = rng_rns[16/4-unused_rns_in_bytes/4];
	unused_rns_in_bytes -= 4;
	return ret;
}

u8 pesudo_rng_readb(void) {
	static u8 seed = 0;
	seed++;
	u8 ret;
	ret = seed;
	if(seed & 0x1)
		ret = ~ret;
	return ret;
}

unsigned long rng_hw(unsigned char *buf, unsigned long len,
                               void (*callback)(void))
{
	int i=0;
#if 0
	if(rng_regs == NULL) {
		printf("Error: brite prng reg base not set!");
	}
	if(rng_regs->micro_pd)
		rng_pon();
#endif
	for(i=0; i<len; i++) {
		buf[i] = pesudo_rng_readb();
	}
	return len;
}
