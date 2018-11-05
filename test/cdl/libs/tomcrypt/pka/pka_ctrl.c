#include <stdio.h>
#include "pka.h"

//#define DEBUG
#undef DEBUG

volatile struct pka_regs *pka_regs; // = (volatile void *) ECC_REG_BASE;


void pka_dump_regs(volatile struct pka_regs *ecc_regs) {
	printf("ctrl			0x%08lx\n", ecc_regs->ctrl);
	printf("prog_addr		0x%08lx\n", ecc_regs->prog_addr);
	printf("ret_code		0x%08lx\n", ecc_regs->ret_code);
	printf("build_config	0x%08lx\n", ecc_regs->build_config);
	printf("sp				0x%08lx\n", ecc_regs->sp);
	printf("instr_cnt		0x%08lx\n", ecc_regs->instr_cnt);
	printf("param_sp		0x%08lx\n", ecc_regs->param_sp);
	printf("config			0x%08lx\n", ecc_regs->config);
	printf("irq				0x%08lx\n", ecc_regs->irq);
	printf("flags			0x%08lx\n", ecc_regs->flags);
	printf("wdt_instr_cnt2halt		0x%08lx\n", ecc_regs->wdt_instr_cnt2halt);
	printf("cyc_cnt			0x%08lx\n", ecc_regs->cyc_cnt);
	printf("idx_i			0x%08lx\n", ecc_regs->idx_i);
	printf("idx_j			0x%08lx\n", ecc_regs->idx_j);
	printf("idx_k			0x%08lx\n", ecc_regs->idx_k);
	printf("idx_l			0x%08lx\n", ecc_regs->idx_l);
	printf("ien				0x%08lx\n", ecc_regs->ien);
	printf("jump_proba		0x%08lx\n", ecc_regs->jump_proba);
	printf("proba_lfsr_seed		0x%08lx\n", ecc_regs->proba_lfsr_seed);
	printf("bank_sw_a		0x%08lx\n", ecc_regs->bank_sw_a);
	printf("bank_sw_b		0x%08lx\n", ecc_regs->bank_sw_b);
	printf("bank_sw_c		0x%08lx\n", ecc_regs->bank_sw_c);
	printf("bank_sw_d		0x%08lx\n", ecc_regs->bank_sw_d);
}

void pka_set_reg_base(volatile void *reg) {
	pka_regs = reg;
}

#ifdef PKA_RAM_FW
void load_pka_ram_fw(void) {
	volatile u32 *load = PKA_FIRMWARE_ADDR;
	int i=0;
	for(i=0; i<ARRAY_SIZE(pka_fw); i++) {
		*load++ = pka_fw[i];
	}
}
#endif

void pka_wr_op_blk(u32 alg_width, volatile u32 * op_blk, u32 idx, mpz_t n) {
	volatile u32 * op_blk_xn = (volatile void *)op_blk + ROUND_UP(alg_width, 256)/8*idx;
	u32 cnt = abs(n->_mp_size);
	u32 offset = 0;
	u32 *buf;
	buf = n->_mp_d;
#ifdef DEBUG
	printf("op_blk %p _xn %p\n", op_blk, op_blk_xn);
	printf("ROUND_UP(%ld, %d)=%ld\n", alg_width, 256, ROUND_UP(alg_width, 256));
#endif
	for(offset = 0; offset < cnt; offset++) {
		*(op_blk_xn+offset) = buf[offset];
	}
	for(; offset < ROUND_UP(alg_width, 256)/32; offset++) {
		*(op_blk_xn+offset) = 0x0UL;
	}
	dmb();
	return;
}

void pka_rd_op_blk(u32 alg_width, volatile u32 * op_blk, u32 idx, mpz_t n) {
	volatile u32 * op_blk_xn = (volatile void *)op_blk + ROUND_UP(alg_width, 256)/8*idx;
	u32 cnt = ROUND_UP(alg_width, 32)/32;
	u32 offset = 0;
	u32 *buf;
	dmb();
	mpz_realloc2(n, alg_width);
	buf = n->_mp_d;
#ifdef DEBUG
	printf("op_blk %p _xn %p\n", op_blk, op_blk_xn);
	printf("ROUND_UP(%ld, %d)=%ld\n", alg_width, 256, ROUND_UP(alg_width, 256));
#endif
	for(offset = 0; offset < cnt; offset++) {
		buf[offset] = *(op_blk_xn+offset);
	}
	n->_mp_size = cnt;
	mpz_rm_lead0(n);
	return;
}

int log2i(u32 n) {
	int ret = 0;
	if(n == 0) {
		printf("FATAL: log2(0)\n");
		return -1;
	}
	do {
		if(n&0x1)
			break;
		ret++;
		n >>= 1;
	} while(n > 0);
	if(n != 1) {
		printf("FATAL: can't log2 to int n");
		return -2;
	}
	return ret;
}

bool is_part_radix(int radix) {
	switch(radix) {
	case 256:
	case 512:
	case 1024:
	case 2048:
	case 4096:
		return false;
	default:
		return true;
	}
}

void pka_ctrl(int go, int stop_req, int alg_width) {
	u32 val = 0;
	u32 base_radix = 0;
	u32 part_radix = 0;

	if(go)
		val |= PKA_CTRL_GO;
	if(stop_req)
		val |= PKA_CTRL_STOP_RQST;
	if(alg_width > 512) {
		val |= PKA_CTRL_M521_MODE;
	}

	base_radix = log2i(ROUND_UP(alg_width, 256)/PKA_BC_ALU_WIDTH)-1;
	if(is_part_radix(alg_width))
		part_radix = ((alg_width+PKA_BC_ALU_WIDTH-1)/PKA_BC_ALU_WIDTH)*(PKA_BC_ALU_WIDTH/32);
	val |= (base_radix<<PKA_CTRL_BASE_RADIX_OFFSET);
	val |= (part_radix<<PKA_CTRL_PART_RADIX_OFFSET);
#ifdef DEBUG
	printf("ctrl val: 0x%08lx\n", val);
#endif
	pka_regs->ctrl = val;
	dmb();
}

int pka_pre_check(void) {
	int ret = 0;
	if(pka_regs->ctrl & PKA_CTRL_GO) {
		printf("Error: previous pka run still ongoing!\n");
		ret++;
	}
	if(pka_regs->irq & PKA_STAT_IRQ) {
		printf("Error: previous IRQ_STAT not cleared!\n");
		ret++;
	}
	if(pka_regs->ctrl & PKA_CTRL_GO) {
		printf("Warning: stop previous request!");
		pka_regs->ctrl = (pka_regs->ctrl & ~PKA_CTRL_GO) | PKA_CTRL_STOP_RQST;
		while(!(pka_regs->irq & PKA_STAT_IRQ))
			;
		pka_regs->irq = PKA_STAT_IRQ;
		pka_regs->param_sp = 0x0;
		pka_regs->sp = 0x0;
	}
	return ret;
}

void pka_do_clr(void) {
	pka_regs->param_sp = 0x0;
	pka_regs->sp = 0x0;
	pka_regs->ien = 0x0;
	pka_regs->config = 0x0;
}

int pka_run_func_no_clr(u32 alg_width, u32 func_entry, u32 flags) {
	pka_pre_check();
	pka_regs->flags = flags;

	pka_regs->prog_addr = func_entry;
	dmb();

	pka_ctrl(1, 0, alg_width);

	// TODO: timeout
	while(!(pka_regs->irq & PKA_STAT_IRQ))
		;
	if(pka_regs->sp != 0x0) {
#ifdef DEBUG
		printf("Ooops! sp=0x%lx is not zero\n", pka_regs->sp);
#endif
	}
	pka_regs->irq  = PKA_STAT_IRQ;
	if(pka_regs->ret_code & PKA_RC_STOP_REASON) {
		printf("Ooops! stop reason %lu\n",
				(pka_regs->ret_code & PKA_RC_STOP_REASON) >> PKA_RC_STOP_REASON_OFFSET);
		return -1;
	}
	return 0;
}

int pka_run_func(u32 alg_width, u32 func_entry, u32 flags) {
	pka_pre_check();
	pka_do_clr();
	pka_regs->flags = flags;

	pka_regs->prog_addr = func_entry;
	dmb();

	pka_ctrl(1, 0, alg_width);

	// TODO: timeout
	while(!(pka_regs->irq & PKA_STAT_IRQ))
		;
	if(pka_regs->sp != 0x0) {
#ifdef DEBUG
		printf("Ooops! sp=0x%lx is not zero\n", pka_regs->sp);
#endif
	}
	pka_regs->irq  = PKA_STAT_IRQ;
	if(pka_regs->ret_code & PKA_RC_STOP_REASON) {
		printf("Ooops! stop reason %lu\n",
				(pka_regs->ret_code & PKA_RC_STOP_REASON) >> PKA_RC_STOP_REASON_OFFSET);
		return -1;
	}
	return 0;
}
