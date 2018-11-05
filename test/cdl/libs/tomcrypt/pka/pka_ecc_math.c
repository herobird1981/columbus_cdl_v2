#include "pka.h"

//volatile struct pka_regs *pka_regs = (volatile void *) ECC_REG_BASE;


/* ECC (Weierstrass) Point Multiplication
 * Q=kP provided Pxy is on the curve defined by a, b, and p, and (2<k<order-of-base-point),
 * w is a random number such that (0<w<p)
 * Computes the EC Point Multiplication over the curve y^2=x^3+ax+b mod p
 */
int pka_ecc_pmul(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t a, mpz_t k,
		mpz_t w, mpz_t p, mpz_t mp, mpz_t r_sqr, bool en_blind, bool a_3modp,
		u16 loops, mpz_t Qx, mpz_t Qy){
	u32 ret;
	u32 flags = 0;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 2, Px);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 2, Py);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 6, a);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 7, k);
	if(w)
		pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 7, w);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, p);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	if(en_blind)
		flags |= PKA_FLAG_F0;
	if(a_3modp)
		flags |= PKA_FLAG_F3;
	pka_regs->idx_l = loops;
	ret = pka_run_func(alg_width, PKA_FUNC_PMULT, flags);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 2, Qx);
	pka_rd_op_blk(alg_width, PKA_OP_BLK_B, 2, Qy);
	return 0;
}

/* ECC (Weierstrass) Point Addition
 * R=P+Q provided Pxy and Qxy are on the curve defined by a, b, and p, and Px != +-Qx,
 * Computes the EC Point Addition over the curve y^2=x^3+ax+b mod p
 */
int pka_ecc_padd(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t Qx, mpz_t Qy, mpz_t a,
		mpz_t p, mpz_t mp, mpz_t r_sqr, mpz_t Rx, mpz_t Ry){
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 2, Px);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 2, Py);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 3, Qx);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 3, Qy);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 6, a);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, p);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	ret = pka_run_func(alg_width, PKA_FUNC_PADD, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 2, Rx);
	pka_rd_op_blk(alg_width, PKA_OP_BLK_B, 2, Ry);
	return 0;
}

/* ECC (Weierstrass) Shamir's Trick
 * R=kP+lQ provided Pxy and Qxy on the curve defined by a, b, and p, and (2<k and l<order-of-base-point),
 * Pxy != Qxy, and Pxy != -Qxy
 */
int pka_ecc_shamir(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t Qx, mpz_t Qy, mpz_t a, mpz_t k,
		mpz_t l, mpz_t p, mpz_t mp, mpz_t r_sqr, bool a_3modp,
		mpz_t Rx, mpz_t Ry){
	u32 ret;
	u32 flags = 0;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 2, Px);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 2, Py);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 3, Qx);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 3, Qy);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 6, a);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 7, k);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 7, l);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, p);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	if(a_3modp)
		flags |= PKA_FLAG_F3;
	ret = pka_run_func(alg_width, PKA_FUNC_SHAMIR, flags);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 3, Rx);
	pka_rd_op_blk(alg_width, PKA_OP_BLK_B, 3, Ry);
	return 0;
}

/* ECC (Weierstrass) Point Double
 * Q=2P provided Pxy is on the curve defined by a, b, and p, and Px != +-Qx,
 * Computes the EC Point Doubling over the curve y^2=x^3+ax+b mod p
 */
int pka_ecc_pdbl(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t a,
		mpz_t p, mpz_t mp, mpz_t r_sqr, mpz_t Qx, mpz_t Qy){
	u32 ret;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 3, Px);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 3, Py);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 6, a);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, p);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	ret = pka_run_func(alg_width, PKA_FUNC_PDBL, 0);
	if(ret)
		return ret;
	pka_rd_op_blk(alg_width, PKA_OP_BLK_A, 2, Qx);
	pka_rd_op_blk(alg_width, PKA_OP_BLK_B, 2, Qy);
	return 0;
}


/* ECC (Weierstrass) Point Verification
 * Computes y^2=x^3+ax+b mod p and verified LHS == RHS
 */
int pka_ecc_pverf(u32 alg_width, mpz_t Px, mpz_t Py, mpz_t a, mpz_t b,
		mpz_t p, mpz_t mp, mpz_t r_sqr, bool *ok){
	u32 ret;
	*ok = false;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 2, Px);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_B, 2, Py);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 6, a);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 7, b);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, p);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 1, mp);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 3, r_sqr);
	ret = pka_run_func(alg_width, PKA_FUNC_PVER, 0);
	if(ret)
		return ret;
	if(pka_regs->flags & PKA_FLAG_ZERO)
		*ok = true;
	return 0;
}


/* Is a == -3 mod p
 * Computes a == -3 mod p
 */
int pka_is_a_m3(u32 alg_width, mpz_t a, mpz_t p, bool *a_3modp) {
	u32 ret;
	*a_3modp = false;
	pka_wr_op_blk(alg_width, PKA_OP_BLK_A, 6, a);
	pka_wr_op_blk(alg_width, PKA_OP_BLK_D, 0, p);
	ret = pka_run_func(alg_width, PKA_FUNC_IS_A_M3, 0);
	if(ret)
		return ret;
	if(pka_regs->flags & PKA_FLAG_F3)
		*a_3modp = true;
	return 0;
}
