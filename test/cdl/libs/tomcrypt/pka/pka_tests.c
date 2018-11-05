#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ARMCC_SIM
#include "exsys_msgs.h"
#include "exsys_ahb_rw.h"
#include "print_led.h"
#include "mem_map.h"
#include "dma_reg_map.h"
#include "ddr_ctrl_reg_map.h"
#include "gpio_reg_map.h"
#include "i2c_reg_map.h"
#include "nfc_reg_map.h"
#include "qspi_reg_map.h"
#include "spi_reg_map.h"
#include "wdt_reg_map.h"
#include "uart_reg_map.h"
#include "usb_reg_map.h"
#include "intr_func.h"
#include "intr_num.h"
#include "gic_reg_map.h"
#endif

#include "pka.h"

void pka_rom_dump(void) {
	int i = 0;
	int l = 0;
	printf("rom dump:\n");
	for(; i<1024*4; i+=16) {
		if(!(i % 16))
			printf("%04x: ", i/16);
		for(l=0; l<16; l+=4)
			printf("0x%08lx ", * (volatile u32 *)(PKA_FIRMWARE_ADDR+i+l));
		printf("\n");
	}
}

void ecc_192_multi_hc(void) {

	u32 *buf;
	mpz_t t1;
	mpz_t t2;
	mpz_init(t1);
	mpz_init(t2);
	buf = t1->_mp_d;
	mpz_set_ui(t1, 0x12345678UL);

	print_hex("t2", 4, (void *)t2->_mp_d, t2->_mp_size * sizeof(mp_limb_t));

	pka_montg_calc_r_inv(192, t1, t2);

	memset((void *)buf, 0x89, sizeof(buf));
	pka_wr_op_blk(192, (volatile void *) PKA_OP_BLK_A, 3, t1);
}

void pka_test_modadd_1(void) { // little number, fail!
	mpz_t a;
	mpz_t b;
	mpz_t m;
	mpz_t c;
	mpz_init_set_ui(a, 3);
	mpz_init_set_ui(b, 4);
	mpz_init_set_ui(m, 5);
	mpz_init(c);
	pka_modadd(192, a, b, m, c);
}

void pka_test_modadd_2(void) {
	mpz_t a;
	mpz_t b;
	mpz_t m;
	mpz_t c;
	char m_str[] = "E5E8EFE8F814DC39E6A80EC518C79839454C4A5A8C4CC5524164AD54654AAD1D2D65515EE60265743A22BF9CE7A5E60399E4A656479D64575FAD03051DA1070B";
	char b_str[] = "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007CF6CF577D108D4E52D3808AE37FF";
	char a_str[] = "E6694A61D918D4DDCCF7C502898571B8C06A4A75F4C1BB141DD6146C0999DB109D973CDFB9B802475BEE7A3B9E6AD0B2A589341F8C51F1A570C30D2E85478F90";
	mpz_init_set_str(a, a_str, 16);
	mpz_init_set_str(b, b_str, 16);
	mpz_init_set_str(m, m_str, 16);
	mpz_init(c);

	pka_modadd(512, a, b, m, c);
	print_hex("c=a+b", 4, c->_mp_d, c->_mp_size * sizeof(mp_limb_t));
	// c = a+b mod m should be 00805A78E103F8A3E64FB63D70BDD97F7B1E001B6874F5C1DC716717A44F2DF37031EB80D3B59CD321CBBA9EB6C4EAAF0BAC5D363A2C5E56E5FB37617054C084
}

