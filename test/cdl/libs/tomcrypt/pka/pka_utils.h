/*
 * util.h
 *
 *  Created on: Aug 3, 2016
 *      Author: bobz
 */

#ifndef SRC_PKA_UTILS_H_
#define SRC_PKA_UTILS_H_
#include <mini-gmp.h>

#define abs(x)	((x)>0?(x):-(x))
#define ROUND_UP(n, align)	(((n)+(align)-1)&~((align)-1))
#ifndef ARRAY_SIZE
#	define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif
void print_hex(const char* what, const unsigned long group, const void *p1, const unsigned long len);
void mpz_rm_lead0(mpz_t n);
void dmb(void);

#endif /* SRC_UTILS_H_ */
