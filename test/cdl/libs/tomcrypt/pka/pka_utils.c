#include <stdio.h>
#include "pka_utils.h"

void print_hex(const char* what, const unsigned long group, const void *p1, const unsigned long len)
{
	const unsigned char* p = p1;
	unsigned long g;
	unsigned long x;
	unsigned long idx = 0;
	printf("%s contents: \n", what);
	for (x = 0; x < len; ) {
		if (!(x % 16)) {
			printf("%04lx: ", idx++);
		}
		for(g=group; g>0; g--) {
			if(x+g-1 >= len)
				continue;
			printf("%02x", p[x+g-1]);
		}
		printf(" ");
		x += group;
		if (group ==1 && !(x % 4) && (x % 16)) {
			printf("  ");
		}
		if (!(x % 16)) {
			printf("\n");
		}
	}
	printf("\n");
}

void dmb() {
	asm volatile("dmb");
	asm volatile("dsb");
}

void mpz_rm_lead0(mpz_t n) {
	int neg = n->_mp_size < 0;
	int lead0 = abs(n->_mp_size);
	while(lead0 > 0) {
		if(n->_mp_d[lead0-1] == (mp_limb_t)0) {
			lead0--;
		} else {
			break;
		}
	}
	n->_mp_size = neg ? -lead0 : lead0;
}
