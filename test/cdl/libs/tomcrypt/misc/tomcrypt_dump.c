#include <stdio.h>
#include <mini-gmp.h>
#include <tomcrypt.h>
#include <tomcrypt_dump.h>
#include "pka_utils.h"

void dump_mpz(const char * what, mpz_t n) {
	print_hex(what, 4, n->_mp_d, n->_mp_size*sizeof(mp_limb_t));
}

#if 0
/** An ECC key */
typedef struct {
    /** Type of key, PK_PRIVATE or PK_PUBLIC */
    int type;

    /** Index into the ltc_ecc_sets[] for the parameters of this curve; if -1, then this key is using user supplied curve in dp */
    int idx;

	/** pointer to domain parameters; either points to NIST curves (identified by idx >= 0) or user supplied curve */
	const ltc_ecc_set_type *dp;

    /** The public key */
    ecc_point pubkey;

    /** The private key */
    void *k;
} ecc_key;

/** Structure defines a NIST GF(p) curve */
typedef struct {
   /** The size of the curve in octets */
   int size;

   /** name of curve */
   char *name;

   /** The prime that defines the field the curve is in (encoded in hex) */
   char *prime;

   /** The fields B param (hex) */
   char *B;

   /** The order of the curve (hex) */
   char *order;

   /** The x co-ordinate of the base point on the curve (hex) */
   char *Gx;

   /** The y co-ordinate of the base point on the curve (hex) */
   char *Gy;
} ltc_ecc_set_type;

/** A point on a ECC curve, stored in Jacbobian format such that (x,y,z) => (x/z^2, y/z^3, 1) when interpretted as affine */
typedef struct {
    /** The x co-ordinate */
    void *x;

    /** The y co-ordinate */
    void *y;

    /** The z co-ordinate */
    void *z;
} ecc_point;
#endif
void dump_ecc_point(const char * what, ecc_point *p) {
	printf("%s %s:\n", __func__, what);
	dump_mpz("x", p->x);
	dump_mpz("y", p->y);
	dump_mpz("z", p->z);
}
void dump_ltc_ecc_type(const char * what, const ltc_ecc_set_type *dp) {
	printf("%s %s: size %d name %s\n"
			"prime %s\n"
			"B %s\n"
			"order %s\n"
			"Gx %s\n"
			"Gy %s\n",
			__func__, what,
			dp->size, dp->name, dp->prime, dp->B, dp->order, dp->Gx, dp->Gy);
}
void dump_ecc_key(const char * what, ecc_key *key) {
	printf("%s %s: type %d idx %d\n", __func__, what, key->type, key->idx);
	dump_ltc_ecc_type("dp", key->dp);
	dump_ecc_point("pubkey", &key->pubkey);
	dump_mpz("k", key->k);
}

