#******************************************************************************
#
# Makefile - Rules for building the usblib
#
#
#******************************************************************************

#
# Defines the directory suffix that this project uses.
#
SUFFIX=
TARGET=libtomcrypt.a

#
# The base directory for StellarisWare.
#
ROOT=$(shell readlink -m ../..)

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

CFLAGS+=-D__ARM_PCS_VFP

#
# Where to find source files that do not live in this directory.
#
VPATH=.

#
# Where to find header files that do not live in the source directory.
#
#$(warning ROOT: $(ROOT))
IPATH+=headers
IPATH+=mini-gmp
IPATH+=pka
IPATH+=$(ROOT)/../../cdl/include
IPATH+=$(ROOT)/libs/stdlib/inc
IPATH+=$(ARMGCC_ROOT)/include

CSRCS = \
	ciphers/aes/aes.c \
	ciphers/aes/aes_tab.c \
	encauth/ccm/ccm_add_aad.c \
	encauth/ccm/ccm_add_nonce.c \
	encauth/ccm/ccm_done.c \
	encauth/ccm/ccm_init.c \
	encauth/ccm/ccm_memory.c \
	encauth/ccm/ccm_memory_ex.c \
	encauth/ccm/ccm_process.c \
	encauth/ccm/ccm_reset.c \
	encauth/gcm/gcm_add_aad.c \
	encauth/gcm/gcm_add_iv.c \
	encauth/gcm/gcm_done.c \
	encauth/gcm/gcm_gf_mult.c \
	encauth/gcm/gcm_init.c \
	encauth/gcm/gcm_memory.c \
	encauth/gcm/gcm_mult_h.c \
	encauth/gcm/gcm_process.c \
	encauth/gcm/gcm_reset.c \
	hashes/helper/hash_memory.c \
	hashes/sha1.c \
	hashes/sha2/sha224.c \
	hashes/sha2/sha256.c \
	hashes/sha2/sha384.c \
	hashes/sha2/sha512.c \
	hashes/sha2/sha512_224.c \
	hashes/sha2/sha512_256.c \
	mac/omac/omac_process.c \
	mac/omac/omac_memory_multi.c \
	mac/omac/omac_memory.c \
	mac/omac/omac_init.c \
	mac/omac/omac_file.c \
	mac/omac/omac_done.c \
	math/gmp_desc.c \
	math/ltm_desc.c \
	math/multi.c \
	math/pka_desc.c \
	math/rand_bn.c \
	math/rand_prime.c \
	mini-gmp/mini-gmp.c \
	misc/crypt/crypt.c \
	misc/crypt/crypt_argchk.c \
	misc/crypt/crypt_cipher_descriptor.c \
	misc/crypt/crypt_cipher_is_valid.c \
	misc/crypt/crypt_find_hash.c \
	misc/crypt/crypt_find_hash_oid.c \
	misc/crypt/crypt_find_prng.c \
	misc/crypt/crypt_hash_descriptor.c \
	misc/crypt/crypt_hash_is_valid.c \
	misc/crypt/crypt_ltc_mp_descriptor.c \
	misc/crypt/crypt_prng_descriptor.c \
	misc/crypt/crypt_prng_is_valid.c \
	misc/crypt/crypt_register_cipher.c \
	misc/crypt/crypt_register_hash.c \
	misc/crypt/crypt_register_prng.c \
	misc/crypt/crypt_unregister_cipher.c \
	misc/crypt/crypt_unregister_hash.c \
	misc/crypt/crypt_unregister_prng.c \
	misc/crypt/crypt_find_cipher.c \
	misc/error_to_string.c \
	misc/zeromem.c \
	misc/mem_neq.c \
	misc/tomcrypt_dump.c \
	modes/ecb/ecb_decrypt.c \
	modes/ecb/ecb_encrypt.c \
	modes/ecb/ecb_start.c \
	modes/ecb/ecb_done.c \
	modes/cbc/cbc_decrypt.c \
	modes/cbc/cbc_encrypt.c \
	modes/cbc/cbc_getiv.c \
	modes/cbc/cbc_setiv.c \
	modes/cbc/cbc_start.c \
	modes/cbc/cbc_done.c \
	modes/ctr/ctr_decrypt.c \
	modes/ctr/ctr_done.c \
	modes/ctr/ctr_encrypt.c \
	modes/ctr/ctr_getiv.c \
	modes/ctr/ctr_setiv.c \
	modes/ctr/ctr_start.c \
	modes/ctr/ctr_test.c \
	pk/asn1/der/bit/der_decode_bit_string.c \
	pk/asn1/der/bit/der_decode_raw_bit_string.c \
	pk/asn1/der/bit/der_encode_bit_string.c \
	pk/asn1/der/bit/der_encode_raw_bit_string.c \
	pk/asn1/der/bit/der_length_bit_string.c \
	pk/asn1/der/boolean/der_decode_boolean.c \
	pk/asn1/der/boolean/der_encode_boolean.c \
	pk/asn1/der/boolean/der_length_boolean.c \
	pk/asn1/der/choice/der_decode_choice.c \
	pk/asn1/der/ia5/der_decode_ia5_string.c \
	pk/asn1/der/ia5/der_encode_ia5_string.c \
	pk/asn1/der/ia5/der_length_ia5_string.c \
	pk/asn1/der/integer/der_decode_integer.c \
	pk/asn1/der/integer/der_encode_integer.c \
	pk/asn1/der/integer/der_length_integer.c \
	pk/asn1/der/object_identifier/der_decode_object_identifier.c \
	pk/asn1/der/object_identifier/der_encode_object_identifier.c \
	pk/asn1/der/object_identifier/der_length_object_identifier.c \
	pk/asn1/der/octet/der_decode_octet_string.c \
	pk/asn1/der/octet/der_encode_octet_string.c \
	pk/asn1/der/octet/der_length_octet_string.c \
	pk/asn1/der/printable_string/der_decode_printable_string.c \
	pk/asn1/der/printable_string/der_encode_printable_string.c \
	pk/asn1/der/printable_string/der_length_printable_string.c \
	pk/asn1/der/sequence/der_decode_sequence_ex.c \
	pk/asn1/der/sequence/der_decode_sequence_flexi.c \
	pk/asn1/der/sequence/der_decode_sequence_multi.c \
	pk/asn1/der/sequence/der_decode_subject_public_key_info.c \
	pk/asn1/der/sequence/der_encode_sequence_ex.c \
	pk/asn1/der/sequence/der_encode_sequence_multi.c \
	pk/asn1/der/sequence/der_encode_subject_public_key_info.c \
	pk/asn1/der/sequence/der_length_sequence.c \
	pk/asn1/der/sequence/der_sequence_free.c \
	pk/asn1/der/set/der_encode_set.c \
	pk/asn1/der/set/der_encode_setof.c \
	pk/asn1/der/short_integer/der_decode_short_integer.c \
	pk/asn1/der/short_integer/der_encode_short_integer.c \
	pk/asn1/der/short_integer/der_length_short_integer.c \
	pk/asn1/der/teletex_string/der_decode_teletex_string.c \
	pk/asn1/der/teletex_string/der_length_teletex_string.c \
	pk/asn1/der/utctime/der_decode_utctime.c \
	pk/asn1/der/utctime/der_encode_utctime.c \
	pk/asn1/der/utctime/der_length_utctime.c \
	pk/asn1/der/utf8/der_decode_utf8_string.c \
	pk/asn1/der/utf8/der_encode_utf8_string.c \
	pk/asn1/der/utf8/der_length_utf8_string.c \
	pk/ecc/ecc.c \
	pk/ecc/ecc_ansi_x963_export.c \
	pk/ecc/ecc_ansi_x963_import.c \
	pk/ecc/ecc_decrypt_key.c \
	pk/ecc/ecc_encrypt_key.c \
	pk/ecc/ecc_export.c \
	pk/ecc/ecc_free.c \
	pk/ecc/ecc_get_size.c \
	pk/ecc/ecc_import.c \
	pk/ecc/ecc_make_key.c \
	pk/ecc/ecc_shared_secret.c \
	pk/ecc/ecc_sign_hash.c \
	pk/ecc/ecc_sizes.c \
	pk/ecc/ecc_test.c \
	pk/ecc/ecc_verify_hash.c \
	pk/ecc/ltc_ecc_is_valid_idx.c \
	pk/ecc/ltc_ecc_map.c \
	pk/ecc/ltc_ecc_mul2add.c \
	pk/ecc/ltc_ecc_mulmod.c \
	pk/ecc/ltc_ecc_mulmod_timing.c \
	pk/ecc/ltc_ecc_points.c \
	pk/ecc/ltc_ecc_projective_add_point.c \
	pk/ecc/ltc_ecc_projective_dbl_point.c \
	pka/pka_basic_math.c \
	pka/pka_ctrl.c \
	pka/pka_ecc_math.c \
	pka/pka_fw.c \
	pka/pka_tests.c \
	pka/pka_utils.c \
	prngs/brite_prng.c \
	prngs/fortuna.c \
	prngs/rc4.c \
	prngs/rng_get_bytes.c \
	prngs/rng_make_prng.c \
	prngs/sober128.c \
	prngs/sober128tab.c \
	prngs/sprng.c \
	prngs/yarrow.c \

COBJS = $(addprefix ${OUTPUT}/, $(CSRCS:.c=.o))

#
# The default rule
#
.PHONY: ${OUTPUT}
all: ${OUTPUT} ${OUTPUT}/$(TARGET)

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${OUTPUT} ${wildcard *~}

#
# The rule to create the target directory.
#
${OUTPUT}:
	@mkdir -p ${OUTPUT}

#
# Rules for building the USB library.
#
${OUTPUT}/$(TARGET): $(COBJS)

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include $(COBJS:%.o=%.d) __dummy__
endif
