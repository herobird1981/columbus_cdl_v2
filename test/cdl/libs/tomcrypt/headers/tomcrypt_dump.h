#ifndef __TOMCRYPT_DUMP_H__
#define __TOMCRYPT_DUMP_H__

void dump_mpz(const char * what, mpz_t n);
void dump_ecc_point(const char * what, ecc_point *p);
void dump_ltc_ecc_type(const char * what, const ltc_ecc_set_type *dp);
void dump_ecc_key(const char * what, ecc_key *key);

#endif

