
#include "pka.h"


/* Montgomery r^-1 mod m Precomputation
 * r_inv = 1/r mod m
 *
 */
int pka_montg_calc_r_inv(u32 alg_width, mpz_t m, mpz_t r_inv) {
	int ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_CALC_R_INV, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_C, 0, r_inv);
	return 0;
}


/* Montgomery m' precomputation
 * mp = m' = (r*r^-1)/m
 */
int pka_montg_calc_mp(u32 alg_width, mpz_t r_inv, mpz_t m, mpz_t mp) {
	int ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_C, 0, r_inv);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_CALC_MP, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	return 0;
}

/* Montgomery r^2 mod m Precomputation
 * r_sqr = r^2 mod m
 */
int pka_montg_r_sqr(u32 alg_width, mpz_t r_inv, mpz_t m, mpz_t r_sqr) {
	int ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_C, 0, r_inv);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_CALC_R_SQR, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	return 0;
}

/* Modular Multiplication
 * c = x * y mod m provided (0<=x<m) and (0<=y<m)
 */
int pka_modmult(u32 alg_width, mpz_t x, mpz_t y, mpz_t m,
		mpz_t mp, mpz_t r_sqr, mpz_t c) {
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 0, x);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 0, y);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	ret = pka_run_func(alg_width, PKA_FUNC_MODMULT, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 0, c);
	return 0;
}

/* Modular Addition
 * c = x + y mod m provided (0<=x<2m) or (0<=x<2^(ceil(log2(m)))), whichever is less,
 * and (0<=y<2m) or (0<=y<2^(ceil(log2(m)))), whichever is less
 */
int pka_modadd(u32 alg_width, mpz_t x, mpz_t y, mpz_t m, mpz_t c) {
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 0, x);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 0, y);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_MODADD, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 0, c);
	return 0;
}

/* Modular Subtraction
 * c = x - y mod m provided (0<=x<2m) or (0<=x<2^(ceil(log2(m)))), whichever is less,
 * and (0<=y<2m) or (0<=y<2^(ceil(log2(m)))), whichever is less
 */
int pka_modsub(u32 alg_width, mpz_t x, mpz_t y, mpz_t m, mpz_t c) {
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 0, x);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 0, y);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_MODSUB, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 0, c);
	return 0;
}

/* Modular Reduction
 * c = x mod m provided (0<=x<2m) or (0<=x<2^(ceil(log2(m)))), whichever is less
 */
int pka_reduce(u32 alg_width, mpz_t x, mpz_t m, mpz_t c) {
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_C, 0, x);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_REDUCE, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 0, c);
	return 0;
}

/* Modular Division
 * c = y / x mod m provided (0<x<m) and (0<y<m)
 */
int pka_moddiv(u32 alg_width, mpz_t y, mpz_t x, mpz_t m, mpz_t c) {
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_C, 0, y);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 0, x);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_MODDIV, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_C, 0, c);
	return 0;
}

/* Modular Inversion
 * c = 1 / x mod m provided (0<=x<m)
 */
int pka_modinv(u32 alg_width, mpz_t x, mpz_t m, mpz_t c) {
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 0, x);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, m);
	ret = pka_run_func(alg_width, PKA_FUNC_MODINV, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_C, 0, c);
	return 0;
}
