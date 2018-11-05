#ifndef __ECC_PKA_H__
#define __ECC_PKA_H__

#include "pka_types.h"
#include "mini-gmp.h"
#include "pka_utils.h"
#include "pka_fw.h"

// control
#define PKA_CTRL_GO			0x80000000
#define PKA_CTRL_STOP_RQST	0x08000000
#define PKA_CTRL_M521_MODE	0x001F0000
#define PKA_CTRL_BASE_RADIX	0x00000700
#define PKA_CTRL_PART_RADIX	0x000000FF
#define PKA_CTRL_BASE_RADIX_OFFSET	8
#define PKA_CTRL_PART_RADIX_OFFSET	0

// build configuration
#define PKA_BC_ALU_WIDTH	32
#define PKA_BC_FW_ROM_MASK  0xC00
#define PKA_BC_FW_RAM_MASK  0x300

// return code
#define PKA_RC_STOP_REASON	0x00FF0000
#define PKA_RC_STOP_REASON_OFFSET	16

// irq
#define PKA_STAT_IRQ		0x40000000

#define PKA_FLAG_ZERO		0x1
#define PKA_FLAG_MEMBIT		0x2
#define PKA_FLAG_BORROW		0x4
#define PKA_FLAG_CARRY		0x8
#define PKA_FLAG_F0			0x10
#define PKA_FLAG_F1			0x20
#define PKA_FLAG_F2			0x40
#define PKA_FLAG_F3			0x80


struct pka_regs {
	u32 ctrl;				/* 0x00 */
	u32 prog_addr;
	u32 ret_code;
	/* build configuration is constant: */
	u32 build_config;	/* build configuration is constant:
						 * val: 0x80009801
						 * FORMAT_TYPE: 2'b10 :TYPE-2 FORMAT
						 * ALU_SIZE: 2'b00 : 32 bits
						 * RSA_SIZE: 3'b000 : Not Present
						 * ECC_SIZE: 2'b10 : 512 bits
						 * FW_ROM: 3'b011 : 1024 words
						 * FW_RAM: 3'b000 : 0 words
						 * BAND_SW_D: 2'b00 : 1 bank
						 *
						 */
	u32 sp;					/* 0x10 */
	u32 instr_cnt;
	u32 param_sp;
	u32 config;
	u32 irq;				/* 0x20	*/
	u32 flags;
	u32 wdt_instr_cnt2halt;
	u32 cyc_cnt;
	u32 idx_i;				/* 0x30 */
	u32 idx_j;
	u32 idx_k;
	u32 idx_l;
	u32 ien;				/* 0x40	*/
	u32 jump_proba;
	u32 proba_lfsr_seed;
	u32	reserved1;
	u32 bank_sw_a;			/* 0x50	*/
	u32 bank_sw_b;
	u32 bank_sw_c;
	u32 bank_sw_d;
};

extern volatile struct pka_regs *pka_regs;

#define PKA_OP_BLK_A	((volatile void *)((u32)pka_regs+0x400))
#define PKA_OP_BLK_B	((volatile void *)((u32)pka_regs+0x800))
#define PKA_OP_BLK_C	((volatile void *)((u32)pka_regs+0xC00))
#define PKA_OP_BLK_D	((volatile void *)((u32)pka_regs+0x1000))
#define PKA_FIRMWARE_ADDR	((volatile void *)((u32)pka_regs+0x4000))

#define PKA_FUNC_CALC_MP		0x10
#define PKA_FUNC_CALC_R_INV		0x11
#define PKA_FUNC_CALC_R_SQR		0x12

#define PKA_FUNC_MODMULT		0x0A
#define PKA_FUNC_MODADD			0x0B
#define PKA_FUNC_MODDIV			0x0D
#define PKA_FUNC_MODSUB			0x0C
#define PKA_FUNC_MODINV			0x0E
#define PKA_FUNC_REDUCE			0x0F

#define PKA_FUNC_PMULT			0x13
#define PKA_FUNC_PDBL			0x14
#define PKA_FUNC_PDBL_STD_PRJ	0x15
#define PKA_FUNC_PADD			0x16
#define PKA_FUNC_PADD_STD_PRJ	0x17
#define PKA_FUNC_PVER			0x18
#define PKA_FUNC_STD_PRJ_TO_AFFINE	0x19
#define PKA_FUNC_IS_P_EQUAL_Q	0x1A
#define PKA_FUNC_IS_P_REFLECT_Q	0x1B
#define PKA_FUNC_IS_A_M3		0x1C
#define PKA_FUNC_SHAMIR			0x1D


void pka_dump_regs(volatile struct pka_regs *ecc_regs);
void pka_set_reg_base(volatile void *reg);
void mpz_rm_lead0(mpz_t n);
void load_pka_ram_fw(void);
void pka_wr_op_blk(u32 alg_width, volatile u32 * op_blk, u32 idx, mpz_t n);
void pka_rd_op_blk(u32 alg_width, volatile u32 * op_blk, u32 idx, mpz_t n);
int pka_run_func(u32 alg_width, u32 func_entry, u32 flags);

int pka_montg_calc_r_inv(u32 alg_width, mpz_t m, mpz_t r_inv);
int pka_montg_calc_mp(u32 alg_width, mpz_t r_inv, mpz_t m, mpz_t mp);
int pka_montg_r_sqr(u32 alg_width, mpz_t r_inv, mpz_t m, mpz_t r_sqr);

int pka_modmult(u32 alg_width, mpz_t x, mpz_t y, mpz_t m,
		mpz_t mp, mpz_t r_sqr, mpz_t c);
int pka_modadd(u32 alg_width, mpz_t x, mpz_t y, mpz_t m, mpz_t c);
int pka_modsub(u32 alg_width, mpz_t x, mpz_t y, mpz_t m, mpz_t c);
int pka_reduce(u32 alg_width, mpz_t x, mpz_t m, mpz_t c);
int pka_moddiv(u32 alg_width, mpz_t y, mpz_t x, mpz_t m, mpz_t c);
int pka_modinv(u32 alg_width, mpz_t x, mpz_t m, mpz_t c);

int pka_ecc_pmul(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t a, mpz_t k,
		mpz_t w, mpz_t p, mpz_t mp, mpz_t r_sqr, bool en_blind, bool a_3modp,
		u16 loops, mpz_t Qx, mpz_t Qy);
int pka_ecc_padd(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t Qx, mpz_t Qy, mpz_t a,
		mpz_t p, mpz_t mp, mpz_t r_sqr, mpz_t Rx, mpz_t Ry);
int pka_ecc_shamir(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t Qx, mpz_t Qy, mpz_t a, mpz_t k,
		mpz_t l, mpz_t p, mpz_t mp, mpz_t r_sqr, bool a_3modp,
		mpz_t Rx, mpz_t Ry);
int pka_ecc_pdbl(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t a,
		mpz_t p, mpz_t mp, mpz_t r_sqr, mpz_t Qx, mpz_t Qy);
int pka_ecc_pverf(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t a, mpz_t b,
		mpz_t p, mpz_t mp, mpz_t r_sqr, bool *ok);
int pka_is_a_m3(u32 alg_width, mpz_t a, mpz_t p, bool *a_3modp);

void ecc_192_multi_hc(void);


void pka_init_params(void);
void pka_clear_params(void);
void pka_montg_comp_cached(mpz_t m);
void pka_set_curve_a_b(mpz_t a, mpz_t b, mpz_t m);
void pka_set_alg_width(int alg_width);


#endif //__ECC_PKA_H__
