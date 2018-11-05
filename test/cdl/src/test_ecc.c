/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_ecc.h"
#include "memleak.h"

#if (CONFIG_ECC_TEST == 1)

cmdline_entry ecc_test_menu[];

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&ecc_test_menu[0]);
}

#define USE_PKA
static int ecc_test_result;
prng_state yarrow_prng;

#ifdef LTC_MECC

static unsigned int sizes[] = {
#ifdef LTC_ECC112
14,
#endif
#ifdef LTC_ECC128
16,
#endif
#ifdef LTC_ECC160
20,
#endif
#ifdef LTC_ECC192
24,
#endif
#ifdef LTC_ECC224
28,
#endif
#ifdef LTC_ECC256
32,
#endif
#ifdef LTC_ECC384
48,
#endif
#ifdef LTC_ECC521
65
#endif
};


#include <tomcrypt_test.h>

#define exit(x)		do { printf("exit with %d, test failed!\n", x); ecc_test_result++; } while(0);
void run_cmd(int res, int line, char *file, char *cmd, const char *algorithm)
{
   if (res != CRYPT_OK) {
      ecc_test_result++;
      printf("%s (%d)%s%s\n%s:%d:%s\n",
              error_to_string(res), res,
              (algorithm ? " - " : ""), (algorithm ? algorithm : ""),
              file, line, cmd);
      if (res != CRYPT_NOP) {
         exit(EXIT_FAILURE);
      }
   }
}

/*
 * unregister ciphers, hashes & prngs
 */
static void _unregister_all(void)
{
#ifdef LTC_RIJNDAEL
  unregister_cipher(&aes_desc);
#endif
#ifdef LTC_BLOWFISH
  unregister_cipher(&blowfish_desc);
#endif
#ifdef LTC_XTEA
  unregister_cipher(&xtea_desc);
#endif
#ifdef LTC_RC5
  unregister_cipher(&rc5_desc);
#endif
#ifdef LTC_RC6
  unregister_cipher(&rc6_desc);
#endif
#ifdef LTC_SAFERP
  unregister_cipher(&saferp_desc);
#endif
#ifdef LTC_TWOFISH
  unregister_cipher(&twofish_desc);
#endif
#ifdef LTC_SAFER
  unregister_cipher(&safer_k64_desc);
  unregister_cipher(&safer_sk64_desc);
  unregister_cipher(&safer_k128_desc);
  unregister_cipher(&safer_sk128_desc);
#endif
#ifdef LTC_RC2
  unregister_cipher(&rc2_desc);
#endif
#ifdef LTC_DES
  unregister_cipher(&des_desc);
  unregister_cipher(&des3_desc);
#endif
#ifdef LTC_CAST5
  unregister_cipher(&cast5_desc);
#endif
#ifdef LTC_NOEKEON
  unregister_cipher(&noekeon_desc);
#endif
#ifdef LTC_SKIPJACK
  unregister_cipher(&skipjack_desc);
#endif
#ifdef LTC_KHAZAD
  unregister_cipher(&khazad_desc);
#endif
#ifdef LTC_ANUBIS
  unregister_cipher(&anubis_desc);
#endif
#ifdef LTC_KSEED
  unregister_cipher(&kseed_desc);
#endif
#ifdef LTC_KASUMI
  unregister_cipher(&kasumi_desc);
#endif
#ifdef LTC_MULTI2
  unregister_cipher(&multi2_desc);
#endif
#ifdef LTC_CAMELLIA
  unregister_cipher(&camellia_desc);
#endif

#ifdef LTC_TIGER
  unregister_hash(&tiger_desc);
#endif
#ifdef LTC_MD2
  unregister_hash(&md2_desc);
#endif
#ifdef LTC_MD4
  unregister_hash(&md4_desc);
#endif
#ifdef LTC_MD5
  unregister_hash(&md5_desc);
#endif
#ifdef LTC_SHA1
  unregister_hash(&sha1_desc);
#endif
#ifdef LTC_SHA224
  unregister_hash(&sha224_desc);
#endif
#ifdef LTC_SHA256
  unregister_hash(&sha256_desc);
#endif
#ifdef LTC_SHA384
  unregister_hash(&sha384_desc);
#endif
#ifdef LTC_SHA512
  unregister_hash(&sha512_desc);
#endif
#ifdef LTC_SHA512_224
  unregister_hash(&sha512_224_desc);
#endif
#ifdef LTC_SHA512_256
  unregister_hash(&sha512_256_desc);
#endif
#ifdef LTC_RIPEMD128
  unregister_hash(&rmd128_desc);
#endif
#ifdef LTC_RIPEMD160
  unregister_hash(&rmd160_desc);
#endif
#ifdef LTC_RIPEMD256
  unregister_hash(&rmd256_desc);
#endif
#ifdef LTC_RIPEMD320
  unregister_hash(&rmd320_desc);
#endif
#ifdef LTC_WHIRLPOOL
  unregister_hash(&whirlpool_desc);
#endif
#ifdef LTC_CHC_HASH
  unregister_hash(&chc_desc);
#endif

  unregister_prng(&yarrow_desc);
#ifdef LTC_FORTUNA
  unregister_prng(&fortuna_desc);
#endif
#ifdef LTC_RC4
  unregister_prng(&rc4_desc);
#endif
#ifdef LTC_SOBER128
  unregister_prng(&sober128_desc);
#endif
} /* _cleanup() */

void reg_algs(void)
{
  int err;

  //atexit(_unregister_all);

#ifdef LTC_RIJNDAEL
  register_cipher (&aes_desc);
#endif
#ifdef LTC_BLOWFISH
  register_cipher (&blowfish_desc);
#endif
#ifdef LTC_XTEA
  register_cipher (&xtea_desc);
#endif
#ifdef LTC_RC5
  register_cipher (&rc5_desc);
#endif
#ifdef LTC_RC6
  register_cipher (&rc6_desc);
#endif
#ifdef LTC_SAFERP
  register_cipher (&saferp_desc);
#endif
#ifdef LTC_TWOFISH
  register_cipher (&twofish_desc);
#endif
#ifdef LTC_SAFER
  register_cipher (&safer_k64_desc);
  register_cipher (&safer_sk64_desc);
  register_cipher (&safer_k128_desc);
  register_cipher (&safer_sk128_desc);
#endif
#ifdef LTC_RC2
  register_cipher (&rc2_desc);
#endif
#ifdef LTC_DES
  register_cipher (&des_desc);
  register_cipher (&des3_desc);
#endif
#ifdef LTC_CAST5
  register_cipher (&cast5_desc);
#endif
#ifdef LTC_NOEKEON
  register_cipher (&noekeon_desc);
#endif
#ifdef LTC_SKIPJACK
  register_cipher (&skipjack_desc);
#endif
#ifdef LTC_KHAZAD
  register_cipher (&khazad_desc);
#endif
#ifdef LTC_ANUBIS
  register_cipher (&anubis_desc);
#endif
#ifdef LTC_KSEED
  register_cipher (&kseed_desc);
#endif
#ifdef LTC_KASUMI
  register_cipher (&kasumi_desc);
#endif
#ifdef LTC_MULTI2
  register_cipher (&multi2_desc);
#endif
#ifdef LTC_CAMELLIA
  register_cipher (&camellia_desc);
#endif

#ifdef LTC_TIGER
  register_hash (&tiger_desc);
#endif
#ifdef LTC_MD2
  register_hash (&md2_desc);
#endif
#ifdef LTC_MD4
  register_hash (&md4_desc);
#endif
#ifdef LTC_MD5
  register_hash (&md5_desc);
#endif
#ifdef LTC_SHA1
  register_hash (&sha1_desc);
#endif
#ifdef LTC_SHA224
  register_hash (&sha224_desc);
#endif
#ifdef LTC_SHA256
  register_hash (&sha256_desc);
#endif
#ifdef LTC_SHA384
  register_hash (&sha384_desc);
#endif
#ifdef LTC_SHA512
  register_hash (&sha512_desc);
#endif
#ifdef LTC_SHA512_224
  register_hash (&sha512_224_desc);
#endif
#ifdef LTC_SHA512_256
  register_hash (&sha512_256_desc);
#endif
#ifdef LTC_RIPEMD128
  register_hash (&rmd128_desc);
#endif
#ifdef LTC_RIPEMD160
  register_hash (&rmd160_desc);
#endif
#ifdef LTC_RIPEMD256
  register_hash (&rmd256_desc);
#endif
#ifdef LTC_RIPEMD320
  register_hash (&rmd320_desc);
#endif
#ifdef LTC_WHIRLPOOL
  register_hash (&whirlpool_desc);
#endif
#ifdef LTC_CHC_HASH
  register_hash(&chc_desc);
  if ((err = chc_register(register_cipher(&aes_desc))) != CRYPT_OK) {
     printf("chc_register error: %s\n", error_to_string(err));
     exit(EXIT_FAILURE);
  }
#endif


#ifndef LTC_YARROW
   #error This demo requires Yarrow.
#endif
register_prng(&yarrow_desc);
#ifdef LTC_FORTUNA
register_prng(&fortuna_desc);
#endif
#ifdef LTC_RC4
register_prng(&rc4_desc);
#endif
#ifdef LTC_SOBER128
register_prng(&sober128_desc);
#endif

   if ((err = rng_make_prng(128, find_prng("yarrow"), &yarrow_prng, NULL)) != CRYPT_OK) {
      printf("rng_make_prng failed: %s\n", error_to_string(err));
      exit(EXIT_FAILURE);
   }

   if (strcmp("CRYPT_OK", error_to_string(err))) {
       exit(EXIT_FAILURE);
   }

}


void libtomcrypt_init(void) {
   pka_set_reg_base((volatile void *)ECC_BASE);
   ecc_test_result = 0;
   printf("ecc_init\n\r");
   reg_algs();

#ifdef USE_LTM
   ltc_mp = ltm_desc;
#elif defined(USE_TFM)
   ltc_mp = tfm_desc;
#elif defined(USE_GMP)
   ltc_mp = gmp_desc;
#elif defined(USE_PKA)
   ltc_mp = pka_desc;
#else
   extern ltc_math_descriptor EXT_MATH_LIB;
   ltc_mp = EXT_MATH_LIB;
#endif

   printf("build == \n%s\n", crypt_build_settings);
}


#ifdef LTC_MECC

/**
  Perform on the ECC system
  @return CRYPT_OK if successful
*/

int ecc_test_basic_ex(void)
{
   void     *modulus, *order;
   ecc_point  *G, *GG;
   int i, err, primality;

   if ((err = mp_init_multi(&modulus, &order, NULL)) != CRYPT_OK) {
      return err;
   }

   G   = ltc_ecc_new_point();
   GG  = ltc_ecc_new_point();
   if (G == NULL || GG == NULL) {
      mp_clear_multi(modulus, order, NULL);
      ltc_ecc_del_point(G);
      ltc_ecc_del_point(GG);
      return CRYPT_MEM;
   }

   for (i = 0; ltc_ecc_sets[i].size; i++) {
       #if 1
          printf("%s: Testing %d\n", __func__, ltc_ecc_sets[i].size);
       #endif
       if ((err = mp_read_radix(modulus, (char *)ltc_ecc_sets[i].prime, 16)) != CRYPT_OK)   { goto done; }
       if ((err = mp_read_radix(order, (char *)ltc_ecc_sets[i].order, 16)) != CRYPT_OK)     { goto done; }

       /* is prime actually prime? */
       if ((err = mp_prime_is_prime(modulus, 8, &primality)) != CRYPT_OK)                   { goto done; }
       if (primality == 0) {
          err = CRYPT_FAIL_TESTVECTOR;
          goto done;
       }

       /* is order prime ? */
       if ((err = mp_prime_is_prime(order, 8, &primality)) != CRYPT_OK)                     { goto done; }
       if (primality == 0) {
          err = CRYPT_FAIL_TESTVECTOR;
          goto done;
       }
#ifdef USE_PKA
       pka_set_alg_width(ltc_ecc_sets[i].size*8);
#endif
       if ((err = mp_read_radix(G->x, (char *)ltc_ecc_sets[i].Gx, 16)) != CRYPT_OK)         { goto done; }
       if ((err = mp_read_radix(G->y, (char *)ltc_ecc_sets[i].Gy, 16)) != CRYPT_OK)         { goto done; }
       mp_set(G->z, 1);

       /* then we should have G == (order + 1)G */
       if ((err = mp_add_d(order, 1, order)) != CRYPT_OK)                                   { goto done; }
       if ((err = ltc_mp.ecc_ptmul(order, G, GG, modulus, 1)) != CRYPT_OK)                  { goto done; }
       if (mp_cmp(G->x, GG->x) != LTC_MP_EQ || mp_cmp(G->y, GG->y) != LTC_MP_EQ) {
          err = CRYPT_FAIL_TESTVECTOR;
          goto done;
       }
   }
   err = CRYPT_OK;
done:
   ltc_ecc_del_point(GG);
   ltc_ecc_del_point(G);
   mp_clear_multi(order, modulus, NULL);

#ifdef USE_PKA
   pka_set_alg_width(0);
#endif
   return err;
}

#endif

/**
 * do basic ECC point multiplication (from the NIST curves)
 * G == (order + 1)G
 *
 * @return 0 if successful
 */
static int ecc_test_basic(int argc, char* argv[])
{
	libtomcrypt_init();
	printf("sizeof u32 %d unsigned long %d\n", sizeof(u32), sizeof(unsigned long));

	DO(ecc_test_basic_ex ());
#if 0
	DO(ecc_test_basic_ex ());
	DO(ecc_test_basic_ex ());
	DO(ecc_test_basic_ex ());
#endif
	if(!ecc_test_result)
	      printf("%s: test passed!", __func__);
	dbg_mem_stat();

	return 0;
}

typedef unsigned char (*array_4_4k)[4][4096];

/**
 * Sign a message digest and verify an ECC signature
 *
 * @return directly if failed
 */
static int ecc_test_sign_verify (int argc, char* argv[])
{
  array_4_4k buf = (array_4_4k)test_send_buff;
  unsigned char buf_sp[68];
  unsigned long x, y, z, s;
  int           stat, stat2;
  ecc_key usera, userb, pubKey, privKey;

  libtomcrypt_init();

#define LTC_ECC_SIGN_VERIFY
//#define LTC_ECC_TEST_KEY

#ifdef LTC_ECC_SIGN_VERIFY
  for (s = 0; s < (sizeof(sizes)/sizeof(sizes[0])); s++) {
	 #if 1
	   printf("\n%s: Testing %d\n", __func__, ltc_ecc_sets[s].size);
	 #endif
#ifdef USE_PKA
	 pka_set_alg_width(ltc_ecc_sets[s].size*8);
#endif

#ifdef LTC_ECC_TEST_KEY
     /* make up two keys */
     DO(ecc_make_key (&yarrow_prng, find_prng ("yarrow"), sizes[s], &usera));
     DO(ecc_make_key (&yarrow_prng, find_prng ("yarrow"), sizes[s], &userb));

     /* make the shared secret */
     x = sizeof((*buf)[0]);
     DO(ecc_shared_secret (&usera, &userb, (*buf)[0], &x));

     y = sizeof((*buf)[1]);
     DO(ecc_shared_secret (&userb, &usera, (*buf)[1], &y));

     if (y != x) {
       printf("ecc Shared keys are not same size.");
       return 1;
     }

     if (memcmp ((*buf)[0], (*buf)[1], x)) {
       printf("ecc Shared keys not same contents.");
       return 1;
     }

     /* now export userb */
     y = sizeof((*buf)[0]);
     DO(ecc_export ((*buf)[1], &y, PK_PUBLIC, &userb));
     ecc_free (&userb);

     /* import and make the shared secret again */
     DO(ecc_import ((*buf)[1], y, &userb));

     z = sizeof((*buf)[0]);
     DO(ecc_shared_secret (&usera, &userb, (*buf)[2], &z));

     if (z != x) {
       printf("failed.  Size don't match?");
       return 1;
     }
     if (memcmp ((*buf)[0], (*buf)[2], x)) {
       printf("Failed.  Contents didn't match.");
       return 1;
     }

     /* export with ANSI X9.63 */
     y = sizeof((*buf)[1]);
     DO(ecc_ansi_x963_export(&userb, (*buf)[1], &y));
     ecc_free (&userb);

     /* now import the ANSI key */
     DO(ecc_ansi_x963_import((*buf)[1], y, &userb));

     /* shared secret */
     z = sizeof((*buf)[0]);
     DO(ecc_shared_secret (&usera, &userb, (*buf)[2], &z));

     if (z != x) {
       printf("failed.  Size don't match?");
       return 1;
     }
     if (memcmp ((*buf)[0], (*buf)[2], x)) {
       printf("Failed.  Contents didn't match.");
       return 1;
     }

     ecc_free (&usera);
     ecc_free (&userb);
#endif

     /* test encrypt_key */
     DO(ecc_make_key (&yarrow_prng, find_prng ("yarrow"), sizes[s], &usera));

     /* export key */
     x = sizeof((*buf)[0]);
     DO(ecc_export((*buf)[0], &x, PK_PUBLIC, &usera));
     DO(ecc_import((*buf)[0], x, &pubKey));
     x = sizeof((*buf)[0]);
     DO(ecc_export((*buf)[0], &x, PK_PRIVATE, &usera));
     DO(ecc_import((*buf)[0], x, &privKey));

     for (x = 0; x < 32; x++) {
    	 (*buf)[0][x] = x;
     }
     y = sizeof ((*buf)[1]);
     DO(ecc_encrypt_key ((*buf)[0], 32, (*buf)[1], &y, &yarrow_prng, find_prng ("yarrow"), find_hash ("sha256"), &pubKey));
     zeromem ((*buf)[0], sizeof ((*buf)[0]));
     x = sizeof ((*buf)[0]);
     DO(ecc_decrypt_key ((*buf)[1], y, (*buf)[0], &x, &privKey));
     if (x != 32) {
       printf("Failed (length)");
       return 1;
     }
     for (x = 0; x < 32; x++) {
        if ((*buf)[0][x] != x) {
           printf("Failed (contents)");
           return 1;
        }
     }

     /* test sign_hash */
     for (x = 0; x < 16; x++) {
        (*buf)[0][x] = x;
     }
     x = sizeof ((*buf)[1]);
     DO(ecc_sign_hash ((*buf)[0], 16, (*buf)[1], &x, &yarrow_prng, find_prng ("yarrow"), &privKey, 1, buf_sp));
     DO(ecc_verify_hash ((*buf)[1], x, (*buf)[0], 16, &stat, &pubKey));
     if(stat != 1) {
    	 printf("hardware sign/verify failed.\n");
    	 return 1;
     }
     ltc_mp = gmp_desc;
     DO(ecc_sign_hash ((*buf)[0], 16, (*buf)[2], &x, &yarrow_prng, find_prng ("yarrow"), &privKey, 0, buf_sp));
     DO(ecc_verify_hash ((*buf)[2], x, (*buf)[0], 16, &stat, &pubKey));
     if(stat != 1) {
    	 printf("software sign/verify failed.\n");
    	 return 1;
     }
     if(memcmp((*buf)[1], (*buf)[2], x)) {
    	 printf("software sign result doesn't match hardware sign result.\n");
    	 print_hex("(*buf)[1]", 4, (*buf)[1], x);
    	 print_hex("(*buf)[2]", 4, (*buf)[2], x);
    	 return 1;
     }
     (*buf)[0][0] ^= 1;
     ltc_mp = pka_desc;
     DO(ecc_verify_hash ((*buf)[1], x, (*buf)[0], 16, &stat2, &privKey));
     if(stat2 == 1) {
    	 printf("hardware sign/verify failed on bad hash.\n");
    	 return 1;
     }
     ltc_mp = gmp_desc;
     DO(ecc_verify_hash ((*buf)[2], x, (*buf)[0], 16, &stat2, &privKey));
     if(stat2 == 1) {
    	 printf("software sign/verify failed on bad hash.\n");
    	 return 1;
     }
     ltc_mp = pka_desc;
     if (!(stat == 1 && stat2 == 0)) {
        printf("ecc_verify_hash failed %d, %d, ", stat, stat2);
        return 1;
     }
     ecc_free (&usera);
     ecc_free (&pubKey);
     ecc_free (&privKey);
  }
#endif
#ifdef USE_PKA
  pka_set_alg_width(0);
#endif

  if(!ecc_test_result)
	printf("%s: test passed!", __func__);
  dbg_mem_stat();

  return 0;
#endif
}


#ifdef LTC_ECC_SHAMIR
/**
 * test montgomery method
 * Computes kA*A + kB*B = C using Shamir's Trick and compare the result with the older method
 *
 * @return directly if failed
 */
static int ecc_test_shamir(int argc, char* argv[])
{
   void *modulus, *mp, *kA, *kB, *rA, *rB;
   ecc_point *G, *A, *B, *C1, *C2;
   int x, y, z;
   unsigned char buf[ECC_BUF_SIZE];

   libtomcrypt_init();

   DO(mp_init_multi(&kA, &kB, &rA, &rB, &modulus, NULL));
   LTC_ARGCHK((G  = ltc_ecc_new_point()) != NULL);
   LTC_ARGCHK((A  = ltc_ecc_new_point()) != NULL);
   LTC_ARGCHK((B  = ltc_ecc_new_point()) != NULL);
   LTC_ARGCHK((C1 = ltc_ecc_new_point()) != NULL);
   LTC_ARGCHK((C2 = ltc_ecc_new_point()) != NULL);

   for (x = 0; x < (int)(sizeof(sizes)/sizeof(sizes[0])); x++) {
       /* get the base point */
	   printf("%s: Testing %d\n", __func__, sizes[x]);
#if 0
       for (z = 0; ltc_ecc_sets[z].name; z++) {
           if (sizes[z] < (unsigned int)ltc_ecc_sets[z].size) break;
       }
       LTC_ARGCHK(ltc_ecc_sets[z].name != NULL);
#else
       z = x;
#endif
       /* load it */
       DO(mp_read_radix(G->x, ltc_ecc_sets[z].Gx, 16));
       DO(mp_read_radix(G->y, ltc_ecc_sets[z].Gy, 16));
       DO(mp_set(G->z, 1));
       DO(mp_read_radix(modulus, ltc_ecc_sets[z].prime, 16));
       DO(mp_montgomery_setup(modulus, &mp));
#ifdef USE_PKA
       pka_set_alg_width(ltc_ecc_sets[z].size*8);
#endif

       /* do 100 random tests */
       for (y = 0; y < 100; y++) {
          /* pick a random r1, r2 */
          LTC_ARGCHK(yarrow_read(buf, sizes[x], &yarrow_prng) == sizes[x]);
          DO(mp_read_unsigned_bin(rA, buf, sizes[x]));
          LTC_ARGCHK(yarrow_read(buf, sizes[x], &yarrow_prng) == sizes[x]);
          DO(mp_read_unsigned_bin(rB, buf, sizes[x]));

          /* compute rA * G = A */
          DO(ltc_mp.ecc_ptmul(rA, G, A, modulus, 1));

          /* compute rB * G = B */
          DO(ltc_mp.ecc_ptmul(rB, G, B, modulus, 1));

          /* pick a random kA, kB */
          LTC_ARGCHK(yarrow_read(buf, sizes[x], &yarrow_prng) == sizes[x]);
          DO(mp_read_unsigned_bin(kA, buf, sizes[x]));
          LTC_ARGCHK(yarrow_read(buf, sizes[x], &yarrow_prng) == sizes[x]);
          DO(mp_read_unsigned_bin(kB, buf, sizes[x]));

          /* now, compute kA*A + kB*B = C1 using the older method */
          DO(ltc_mp.ecc_ptmul(kA, A, C1, modulus, 0));
          DO(ltc_mp.ecc_ptmul(kB, B, C2, modulus, 0));
          DO(ltc_mp.ecc_ptadd(C1, C2, C1, modulus, mp));
          DO(ltc_mp.ecc_map(C1, modulus, mp));

          /* now compute using mul2add */
          DO(ltc_mp.ecc_mul2add(A, kA, B, kB, C2, modulus));

          /* is they the sames?  */
          if ((mp_cmp(C1->x, C2->x) != LTC_MP_EQ) || (mp_cmp(C1->y, C2->y) != LTC_MP_EQ) || (mp_cmp(C1->z, C2->z) != LTC_MP_EQ)) {
             printf("ECC failed shamir test: size=%d, testno=%d\n", sizes[x], y);
             return 1;
          }
      }
      mp_montgomery_free(mp);
  }
  ltc_ecc_del_point(C2);
  ltc_ecc_del_point(C1);
  ltc_ecc_del_point(B);
  ltc_ecc_del_point(A);
  ltc_ecc_del_point(G);
  mp_clear_multi(kA, kB, rA, rB, modulus, NULL);
#ifdef USE_PKA
  pka_set_alg_width(0);
#endif

  if(!ecc_test_result)
  	printf("%s: test passed!", __func__);
  dbg_mem_stat();

  return 0;
}
#endif


//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry ecc_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "ecc_test_basic",		ecc_test_basic, 	"   : ecc_test_basic" },
	{ "ecc_test_sign_verify",	ecc_test_sign_verify, "  : ecc_test_sign_verify" },
	{ "ecc_test_shamir",		ecc_test_shamir,	"  : ecc_test_shamir" },
	{ 0, 0, 0 }
};

#define ECC_CTRL_DEF_VAL	0x0
bool ecc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
	
	// write register
	info("write ctrl reg\n");
	__raw_writel(*p_wr_val, (ECC_BASE + 0));

	// read&compare
	info("read&compare ctrl reg\n");
	*p_rd_val = __raw_readl(ECC_BASE + 0);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("ecc reset\n");
	prcm_reset(ARM_ECC_RST);
	
	// read register
	info("read&compare ctrl reg\n");
	*p_rd_val = __raw_readl(ECC_BASE + 0);
	if (*p_rd_val != ECC_CTRL_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool ecc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_ECC384;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// write register
	info("write ctrl reg\n");
	__raw_writel(*p_wr_val, (ECC_BASE + 0));

	// read&compare
	info("read&compare ctrl reg\n");
	*p_rd_val = __raw_readl(ECC_BASE + 0);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write ctrl reg\n");
	__raw_writel((*p_wr_val+1), (ECC_BASE + 0));

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare ctrl reg\n");
	*p_rd_val = __raw_readl(ECC_BASE + 0);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool ecc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool ecc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

