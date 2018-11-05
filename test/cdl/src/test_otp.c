/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_otp.h"

#if (CONFIG_OTP_TEST == 1)

struct brite_otp otp;
struct brite_otp* p_otp = &otp;

cmdline_entry otp_test_menu[];

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&otp_test_menu[0]);
}
static int otp_test_init(void)
{
	int ret;
	memset(p_otp, 0, sizeof(struct brite_otp));
	p_otp->regs = (void*)OTP_BASE;
	p_otp->int_vector = INT_OTP;

	ret = otp_init_wrapper(p_otp);
	return ret;
}

#if 0
static void otp_set_non_sec_data(u8* p_data)
{
	int i = 0;
	
	memset(p_data, 0x00, 0x200);

	p_data[0] = (OTP_ST_DIS << OTP_SECU_BOOT_DIS_BIT) |
		(OTP_ST_DIS << OTP_NFC_BOOT_DIS_BIT) |
		(OTP_ST_DIS <<OTP_SPI0_BOOT_DIS_BIT) |
		(OTP_ST_DIS << OTP_USB0_DL_DIS_BIT) |
		(OTP_ST_DIS <<OTP_UART0_DL_DIS_BIT) |
		(OTP_ST_DIS <<OTP_JTAG_DIS_BIT) |
		(OTP_ST_DIS <<OTP_PROG_DIS_BIT) |
		(OTP_ST_EN << OTP_TEST_DIS_BIT);

	p_data[1] = 1;
	p_data[2] = 0;
	p_data[3] = 0;
	for (i = 4; i < 0x200; i++) {
		p_data[i] = i;
		if (i >= BACKUP_REGION_OFFSET && (i <= BACKUP_REGION_OFFSET + 3))
			p_data[i] = p_data[i - BACKUP_REGION_OFFSET];
	}
}
#endif

static void otp_set_sec_data(otp_secu_t* p_otp_secu)
{
	u8 i=0,key=0,size=0;
	u8* p_data = NULL;
	u8 ret = 0;

	//this key is just for test
	unsigned char fake_aes_priv_key[32] = { //key number5, OTP key1
		0x34, 0xf1, 0x4b, 0xd9, 0x1b, 0x32, 0x0f, 0x05, 0xb1, 0x5c, 0x6e, 0x72, 0x9d, 0xec, 0x42, 0x74,
		0xd1, 0xae, 0x29, 0x36, 0x53, 0xe1, 0x89, 0x86, 0xd8, 0x88, 0xfe, 0xcc, 0x39, 0xbb, 0x30, 0x81,
	};

	unsigned char fake_ca_hash[48] = { //key number2, sha
		0x8e, 0xe7, 0x3a, 0x78, 0x7d, 0x63, 0x85, 0xec, 0x16, 0x32, 0x52, 0xff, 0x7a, 0x3e, 0xb9, 0x07,
		0xe6, 0x1d, 0x5e, 0xc8, 0x14, 0x0c, 0xa8, 0x5d, 0x01, 0x33, 0x0e, 0x82, 0x22, 0x3a, 0x14, 0x2e,
		0x61, 0xcf, 0x76, 0x76, 0x80, 0x56, 0xff, 0x5e, 0xeb, 0x04, 0x0c, 0x9c, 0xd6, 0x21, 0x1b, 0xa3,
	};

	unsigned char ecc_key4_le[48] = { //key number4, 0x0a0
		0xf0, 0xef, 0xb2, 0x5d, 0x4b, 0x09, 0x6f, 0x4e, 0x2f, 0x7f, 0xda, 0x40,
		0xc3, 0x27, 0xce, 0xa9, 0x47, 0x29, 0x6a, 0x5d, 0x6c, 0xdc, 0x98, 0xc0,
		0x34, 0x57, 0x8a, 0x07, 0x01, 0x8c, 0xf7, 0xec, 0x73, 0x38, 0xc6, 0x2d,
		0x22, 0x05, 0x1a, 0xab, 0x92, 0xad, 0xa5, 0xf4, 0xcd, 0x98, 0x10, 0x8a,
	};

	for (key = OTP_KEY0_DIS_SHIFT; key <= (OTP_KEY13_DIS_SHIFT + 1); key++) {
		switch (key) {
		case 0:
			p_data = p_otp_secu->itron_key;
			size = sizeof(p_otp_secu->itron_key);
			break;
		case 1:
			p_data = p_otp_secu->itron_root_cer;
			size = sizeof(p_otp_secu->itron_root_cer);
			break;
		case 2:
			p_data = p_otp_secu->itron_code_sign_root_cer;
			size = sizeof(p_otp_secu->itron_code_sign_root_cer);
			break;
		case 3:
			p_data = p_otp_secu->itron_soc_cer;
			size = sizeof(p_otp_secu->itron_soc_cer);
			break;
		case 4:
			p_data = p_otp_secu->itron_soc_cer_private_key;
			size = sizeof(p_otp_secu->itron_soc_cer_private_key);
			break;
		case 5:
			p_data = p_otp_secu->itron_fw_dec_sym_key;
			size = sizeof(p_otp_secu->itron_fw_dec_sym_key);
			break;
		case 6:
			p_data = p_otp_secu->itron_hw_sym_key;
			size = sizeof(p_otp_secu->itron_hw_sym_key);
			break;
		case 7:
			p_data = p_otp_secu->itron_sec_key1;
			size = sizeof(p_otp_secu->itron_sec_key1);
			break;
		case 8:
			p_data = p_otp_secu->itron_sec_key2;
			size = sizeof(p_otp_secu->itron_sec_key2);
			break;
		case 9:
			p_data = p_otp_secu->itron_sec_key3;
			size = sizeof(p_otp_secu->itron_sec_key3);
			break;
		case 10:
			p_data = p_otp_secu->itron_sec_key4;
			size = sizeof(p_otp_secu->itron_sec_key4);
			break;
		case 11:
			p_data = p_otp_secu->itron_sec_key5;
			size = sizeof(p_otp_secu->itron_sec_key5);
			break;
		case 12:
			p_data = p_otp_secu->itron_sec_key6;
			size = sizeof(p_otp_secu->itron_sec_key6);
			break;
		case 13:
			p_data = p_otp_secu->itron_sec_key7;
			size = sizeof(p_otp_secu->itron_sec_key7);
			break;
		default:
			p_data = p_otp_secu->itron_revd;
			size = sizeof(p_otp_secu->itron_revd);
			break;
		}

		if (key == 1) {
			memset(p_data, 0x5a, 48);
		} else if (key == 2) {
			memcpy(p_data,fake_ca_hash,sizeof(fake_ca_hash));
		} else if (key == 4) {
			memcpy(p_data,ecc_key4_le,sizeof(ecc_key4_le));
		} else if (key == 5) {
			memcpy(p_data,fake_aes_priv_key,sizeof(fake_aes_priv_key));	
		} else {
			for (i = 0; i < size; i++) {
				p_data[i] = key + i;
			}
		}
	}
}

/**
 * @brief otp_set_onebit - write one bit to the specified address
 * @param mode input, 'non-secure' or 'secure'
 * @param row input, write row, should be 0-511
 * @param col input, write column, should be 0-7
 *
 * @return directly if failed
 */
static int otp_set_onebit(int argc, char* argv[])
{
	unsigned long col=0,row=0;
	unsigned char val=0,mode=0;
	int ret = 0;
	
	info("otp_set_onebit:test otp one bit program\n");

	if (argc < 4) {
		info("wrong cmd_format: %s mode['non-secure'|'secure'] row col\n",argv[0]);
		return 0;
	}

	if (!strcmp(argv[1],"secure"))
		mode = 1;
	else
		mode = 0;
	row = get_arg_ulong(argv[2]);
	col = get_arg_ulong(argv[3]);

	if (row >= 0x200 || col >= 8) {
		info("wrong: invalid argument,row < 0x200 , col < 8 \n");
		return 0;
	}

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}

	info("program %s area bit,row 0x%x col 0x%x\n",
		mode ? "secure" : "non-secure",
		row,
		col);
	if (mode == 0) {
		ret = otp_get_bit_wrapper(p_otp, (OTP_NON_SECURE + row), col, &val);
		if ((ret != 0) || (val != 0))
			info("otp_non_secure,row 0x%x col 0x%x failed %d\n",
				row, col, ret);
		else {
			if(0 != otp_prog_bit_wrapper(p_otp, (OTP_NON_SEC_BASE_ROW + row), col, 1))
			{
				info("otp_non_secure_set_onebit: test fail for program fail\n");
				return 0;
			}
			ret = otp_get_bit_wrapper(p_otp, (OTP_NON_SECURE + row), col, &val);
			if (val && (ret == 0)) {
				info("otp_non_secure_set_onebit: test pass\n");
			} else {
				info("otp_non_secure_set_onebit: test fail %d\n", ret);
			}
		}
	} else {
		ret = otp_get_bit_wrapper(p_otp, (OTP_SECURE + row), col, &val);
		if ((ret != 0) || (val != 0))
			info("otp_secure,row 0x%x col 0x%x had programed %d\n",
				row, col, ret);
		else {
			if(0 != otp_prog_bit_wrapper(p_otp, (OTP_SEC_BASE_ROW + row), col, 1))
			{
				info("test fail for program fail\n");
				return 0;
			}			
			info("otp_secure_set_onebit prog end. please reboot and run test (otp_get_onebit)\n");
		}
	}
	return 0;
}

/**
 * @brief otp_get_onebit - read one bit from the specified address
 * @param mode input, 'non-secure' or 'secure'
 * @param row input, read row, should be 0-511
 * @param col input, read column, should be 0-7
 *
 * @return directly if failed
 */
static int otp_get_onebit(int argc, char* argv[])
{
	unsigned long col=0,row=0;
	unsigned char val=0,mode=0;
	int ret = 0;

	info("otp_get_onebit:test otp one bit read\n");

	if (argc < 4) {
		info("wrong cmd_format: %s mode['non-secure'|'secure'] row col\n",argv[0]);
		return 0;
	}

	if (!strcmp(argv[1],"secure"))
		mode = 1;
	else
		mode = 0;
	row = get_arg_ulong(argv[2]);
	col = get_arg_ulong(argv[3]);

	if (row >= 0x200 || col >= 8) {
		info("wrong: invalid argument,row < 0x200 , col < 8 \n");
		return 0;
	}

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}

	if (mode == 0) {
		ret = otp_get_bit_wrapper(p_otp, (OTP_NON_SECURE + row), col, &val);
	} else {
		ret = otp_get_bit_wrapper(p_otp, (OTP_SECURE + row), col, &val);
	}
	if(ret != 0)
		info("get otp ownership failed! %d\n", ret);
	else
		info("read %s area bit,row 0x%x col 0x%x  val=0x%x\n",
			mode ? "secure" : "non-secure",
			row,
			col,
			val);

	return 0;
}

/**
 * @brief otp_set_onebyte - write one byte to the specified address
 * @param mode input, 'non-secure' or 'secure'
 * @param row input, write row, should be 0-511
 * @param val input, write value, should be 0-0xFF
 *
 * @return directly if failed
 */
static int otp_set_onebyte(int argc, char* argv[])
{
	unsigned long row=0;
	unsigned long val=0,mode=0;
	unsigned char read_val = 0;
	int fail = 0;
	int i=0;
	int ret = 0;

	info("%s:test otp one byte program\n",argv[0]);

	if (argc < 4) {
		info("wrong cmd_format: %s mode['non-secure'|'secure'] row val\n",argv[0]);
		return 0;
	}

	if (!strcmp(argv[1],"secure"))
		mode = 1;
	else
		mode = 0;
	row = get_arg_ulong(argv[2]);
	val = get_arg_ulong(argv[3]);

	if (row >= 0x200) {
		info("wrong: invalid argument,row < 0x200\n");
		return 0;
	}

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}

	info("program %s area byte,row 0x%x val 0x%x\n",
		mode ? "secure" : "non-secure",
		row,
		val);
	if (mode == 0) {
#ifndef TAPEOUT_V2
		read_val = *(u8 *)(p_otp->regs + OTP_NON_SECURE+row);
#else
		ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + row), &read_val);
		if(ret != 0)
		{
			info("get byte fail %d\n", ret);
			return 0;
		}
#endif
		for (i=0;i<8;i++) {
			if (((read_val >> i) & 1) && !((val >> i) & 1)) {
				info("invalid para: current 0x%x set 0x%x\n",
					read_val,val);
				fail++;
				break;
			}
		}
		if (!fail) {
			ret = otp_prog_byte(p_otp,OTP_NON_SEC_BASE_ROW+row,val);
			if (ret != 0)
				info("otp program non-secure byte fail %d\n", ret);
#ifndef TAPEOUT_V2
			read_val = *(u8 *)(p_otp->regs + OTP_NON_SECURE+row);
#else
			ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + row), &read_val);
			if(ret != 0)
			{
				info("get byte fail %d\n", ret);
				return 0;
			}			
#endif
			if (read_val == val) {
				info("otp_non_secure,set_byte: test pass\n");
			} else {
				info("otp_non_secure,set_byte: test fail\n");
			}
		}
	} else {
#ifndef TAPEOUT_V2
			read_val = *(u8 *)(p_otp->regs + OTP_SECURE+row);
#else
			ret = otp_get_byte_wrapper(p_otp, (OTP_SECURE + row), &read_val);
			if(ret != 0)
			{
				info("get ownership fail %d\n", ret);
				return 0;
			}			
#endif		
		for (i=0;i<8;i++) {
			if (((read_val >> i) & 1) && !((val >> i) & 1)) {
				info("invalid para: current 0x%x set 0x%x\n",
					read_val,val);
				fail++;
				break;
			}
		}
		if (!fail) {
			ret = otp_prog_byte(p_otp,OTP_SEC_BASE_ROW+row,val);
			if (ret != 0)
				info("otp program secure byte fail %d\n", ret);
			info("otp_secure,set_byte prog end. please reboot and run test (otp_get_onebyte secure row)\n");
		}
	}
	return 0;
}

/**
 * @brief otp_get_onebyte - read one byte from the specified address
 * @param mode input, 'non-secure' or 'secure'
 * @param row input, read row, should be 0-511
 *
 * @return directly if failed
 */
static int otp_get_onebyte(int argc, char* argv[])
{
	unsigned long row=0;
	unsigned char val=0,mode=0;
	int ret = 0;

	info("%s:test otp one byte read\n",argv[0]);

	if (argc < 3) {
		info("wrong cmd_format: %s mode['non-secure'|'secure'] row \n",argv[0]);
		return 0;
	}

	if (!strcmp(argv[1],"secure"))
		mode = 1;
	else
		mode = 0;
	row = get_arg_ulong(argv[2]);

	if (row >= 0x200) {
		info("wrong: invalid argument,row < 0x200\n");
		return 0;
	}

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}

	if (mode == 0) {
		ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + row), &val);
	} else {
		ret = otp_get_byte_wrapper(p_otp, (OTP_SECURE + row), &val);
	}

	if(ret != 0)
		info("otp get byte failed! %d\n", ret);
	else
		info("read %s area byte,row 0x%x val=0x%x\n",
			mode ? "secure" : "non-secure",
			row,
			val);

	return 0;
}

/**
 * @brief otp_test - write otp memory with random or fixed data
 * @param mode input, 'non-secure' or 'secure'
 *
 * @return directly if failed
 */
static int otp_test(int argc, char* argv[])
{
	unsigned char tx_data[0x200];
#ifndef TAPEOUT_V2	
	unsigned char* rx_data = NULL;
#else
	unsigned char* rx_data = test_send_buff;
	int ret = 0;
#endif
	int fail = 0, i=0, mode=0;

	info("otp_test : test otp program\n");

	if (argc < 2) {
		info("wrong cmd_format: %s mode['non-secure'|'secure']\n", argv[0]);
		return 0;
	}

	if (!strcmp(argv[1],"secure"))
		mode = 1;
	else
		mode = 0;

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}
	memset(tx_data, 0, 0x200);

	if (mode == 0) {
		//srand(get_ticks());
		for (i = 4; i < 0x200; i++) {
			//tx_data[i] = cb_rand() & 0xff;
			if ((i >= 4 && i <= 7) || (i >= 9 && i <= 11) || (i >= 13 && i <= 19) || (i >= 21 && i <= 23))
				tx_data[i] = 0x0;

			if (i == 8)
				tx_data[i] = 0x40;

			if (i == 0xc)
				tx_data[i] = 0x80;

			if (i == 0x14)
				tx_data[i] = 0x40;

			if (i == 0x18)
				tx_data[i] = 0x80;
			if ((i >= 0x30 && i < 0xf0))
				tx_data[i] = i;
		}
		if(0 != otp_prog(p_otp,
						OTP_NON_SEC_BASE_ROW,
						tx_data,
						(sizeof(otp_secu_t) - 1))) {
			info("otp_non_secure: test fail\n");
			return 0;					
		}
#ifndef TAPEOUT_V2					
		rx_data = (u8 *)(p_otp->regs + OTP_NON_SECURE);
#else
		for(i=0; i<0x200; i++) {
			ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + i), rx_data+i);
			if(ret != 0) {
				info("get byte failed %d\n", ret);
				return 0;
			}
		}
#endif
		for (i = 4; i < (sizeof(otp_secu_t) - 1); i++) {
			if (rx_data[i] != tx_data[i]) {
				info("non_secu p_data[%x]:%x expected %x\n",
					i,
					rx_data[i],
					tx_data[i]);
				fail++;
				break;
			}
		}

		if (fail) {
			info("otp_non_secure: test fail\n");
		} else {
			info("otp_non_secure: test pass\n");
		}
	} else {
		otp_set_sec_data((otp_secu_t *)tx_data);
		if(0 != otp_prog(p_otp,
						OTP_SEC_BASE_ROW,
						tx_data,
						sizeof(otp_secu_t))) {
			info("otp_non_secure: test fail\n");
			return 0;					
		}		

		info("otp secure prog end. Please reboot board, then check prog result.\n");
	}
	return 0;
}

static int otp_key_dis_chk(struct brite_otp* priv)
{
	u32 i=0;
	u8 key=0,size=0;
	u8* p_data = NULL;
	int status = 0;
	u8 value = 0;
#ifndef TAPEOUT_V2	
	otp_secu_t* p_otp_secu = (otp_secu_t*)(priv->regs + OTP_SECURE);
#else
	int ret = 0;
	u8 otp_secu_buf[512] = {0};
	otp_secu_t* p_otp_secu = (otp_secu_t*)otp_secu_buf;
	for(i=0; i<512; i++) {
		ret = otp_get_byte_wrapper(p_otp, (OTP_SECURE + i), &otp_secu_buf[i]);
		if(ret != 0) {
			info("get byte failed %d\n", ret);
			return 0;
		}
	}
#endif
	for (key = OTP_KEY0_DIS_SHIFT; key <= OTP_KEY13_DIS_SHIFT; key++)
	{
		otp_disable_key(priv,key);
#ifdef TAPEOUT_V2
		for(i=0; i<512; i++) {
			ret = otp_get_byte_wrapper(p_otp, (OTP_SECURE + i), &otp_secu_buf[i]);
			if(ret != 0) {
				info("get byte failed %d\n", ret);
				return 0;
			}
		}
#endif

		switch (key) {
		case 0:
			p_data = p_otp_secu->itron_key;
			size = sizeof(p_otp_secu->itron_key);
			break;
		case 1:
			p_data = p_otp_secu->itron_root_cer;
			size = sizeof(p_otp_secu->itron_root_cer);
			break;
		case 2:
			p_data = p_otp_secu->itron_code_sign_root_cer;
			size = sizeof(p_otp_secu->itron_code_sign_root_cer);
			break;
		case 3:
			p_data = p_otp_secu->itron_soc_cer;
			size = sizeof(p_otp_secu->itron_soc_cer);
			break;
		case 4:
			p_data = p_otp_secu->itron_soc_cer_private_key;
			size = sizeof(p_otp_secu->itron_soc_cer_private_key);
			break;
		case 5:
			p_data = p_otp_secu->itron_fw_dec_sym_key;
			size = sizeof(p_otp_secu->itron_fw_dec_sym_key);
			break;
		case 6:
			p_data = p_otp_secu->itron_hw_sym_key;
			size = sizeof(p_otp_secu->itron_hw_sym_key);
			break;
		case 7:
			p_data = p_otp_secu->itron_sec_key1;
			size = sizeof(p_otp_secu->itron_sec_key1);
			break;
		case 8:
			p_data = p_otp_secu->itron_sec_key2;
			size = sizeof(p_otp_secu->itron_sec_key2);
			break;
		case 9:
			p_data = p_otp_secu->itron_sec_key3;
			size = sizeof(p_otp_secu->itron_sec_key3);
			break;
		case 10:
			p_data = p_otp_secu->itron_sec_key4;
			size = sizeof(p_otp_secu->itron_sec_key4);
			break;
		case 11:
			p_data = p_otp_secu->itron_sec_key5;
			size = sizeof(p_otp_secu->itron_sec_key5);
			break;
		case 12:
			p_data = p_otp_secu->itron_sec_key6;
			size = sizeof(p_otp_secu->itron_sec_key6);
			break;
		case 13:
			p_data = p_otp_secu->itron_sec_key7;
			size = sizeof(p_otp_secu->itron_sec_key7);
			break;
		default:
			p_data = p_otp_secu->itron_revd;
			size = sizeof(p_otp_secu->itron_revd);
			break;
		}

		if ((key == 2) || (key == 4) || (key == 5) || (key == 6)) {
			status = 0; /*hw key,can't access by sw*/
			info("key%d not check for could not access\n", key);
		} 
		else if (otp_get_key_status(priv, key)) {
			status = 1; /* key is disabled*/
			for (i = 0; i < size; i++) {
				if (p_data[i] != 0) {
					info("key %x disabled,check fail\n",key);
					return -1;
				}
			}
		} else {
			status = 2; /*key is not disabled*/
			for (i = 0; i < size; i++) {
				if (key == 1)
					value = 0x5a;
				else
					value = (key + i);
				if (p_data[i] != value) {
					info("key %x disable fail\n", key);
					return -1;
				}
			}
		}

		info("key %x status '%s',check OK\n",
			key,
			(
			status ==
			0) ?
			"hw key" :
			((status == 1) ? "disabled" : "not disabled"));
	}

	return 0;
}

/**
 * @brief otp_key_disable - disable and check otp secure key
 *
 * @return directly if failed
 */
static int otp_key_disable(int argc, char* argv[])
{
	int ret = 0;

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}
	ret = otp_key_dis_chk(p_otp);
	if (ret < 0) {
		info("otp_key_disable: test fail\n");
	} else {
		info("otp_key_disable: test pass\n");
	}

	return 0;
}

/**
 * @brief otp_low_power_test - otp low power state test
 * @param mode input, 0:standy, 1:sleep
 *
 * @return directly if failed
 */
static int otp_low_power_test(int argc, char* argv[])
{
	int ret = 0;
	int mode = 0;
	unsigned char val = 0;
	int fail = 0;

	info("otp_low_power_test\n");

	if (argc < 2) {
		info("wrong cmd_format: %s mode\n", argv[0]);
		info("mode:0 ready->standby->initalization | 1 ready->standby->sleep->standby->initalization\n");
		return 0;
	}
	mode = get_arg_ulong(argv[1]);

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}
	if (mode == 0) {   /*test standy flow: ready->standby->initalization*/
		info("\nstandby test\n");
		/*1. go standby */
		ret = power_state_change(p_otp, STATE_STANDBY);
		if (ret != 0) {
			info("go standby failed\n");
			fail++;
			goto end0;
		}

		/*2. go ready again */
		ret = power_state_change(p_otp, STATE_READY);
		if (ret != 0) {
			info("revert ready failed\n");
			fail++;
			goto end0;
		}
		/*3. do some read operation */
		ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + 2), &val);
		if (ret != 0) {
			info("do read failed after revert ready %d\n", ret);
			fail++;
			goto end0;
		}
		info("ready: do read val:%d\n", val);

	} else { /*test sleep flow: ready->standby->sleep->standby->initalization*/
		info("\nsleep test\n");
		/*1. go standby */
		ret = power_state_change(p_otp, STATE_STANDBY);
		if (ret != 0) {
			info("go standby failed\n");
			fail++;
			goto end0;
		}
		
		/*2. go sleep */
		ret = power_state_change(p_otp, STATE_SLEEP);
		if (ret != 0) {
			info("go sleep failed\n");
			fail++;
			goto end0;
		}

		/*3. revert standby */
		ret = power_state_change(p_otp, STATE_STANDBY);
		if (ret != 0) {
			info("revert standby failed\n");
			fail++;
			goto end0;
		}
		/*5. revert ready again */
		ret = power_state_change(p_otp, STATE_READY);
		if (ret) {
			info("revert ready failed\n");
			fail++;
			goto end0;
		}
		/*5. do some read operation */
		ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + 2), &val);
		if (ret != 0) {
			info("do read failed after revert ready\n");
			fail++;
			goto end0;
		}
		info("ready: do read val:%d\n", val);
	}

end0:
	if (fail) {
		info("otp_low_power_test: test fail\n");
	} else {
		info("otp_low_power_test: test pass\n");
	}
	return 0;
}

static int otp_int_counter = 0;
void otp_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = otp_get_int_status(p_otp);
	if (int_status != 0) {
		otp_int_counter++;
		otp_clear_int(p_otp, int_status);
	}
	info("otp_int_handler,status=%x\n", int_status);
}

/**
 * @brief otp_int - otp interrupt test. it will modify non-secure 0x1ff = 0x2
 *
 * @return directly if failed
 */
static int otp_int(int argc, char* argv[])
{
	int fail = 0;
    int ret = 0;

	info("otp_int: interrupt handle test\n");
	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}

	/*register int handler and enable it*/
	irq_generic_install_handler(p_otp->int_vector,
		otp_int_handler,
		NULL);
	irq_generic_config_int(p_otp->int_vector,
		IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(p_otp->int_vector);

	otp_int_counter = 0;

	/*enable interrupt*/
	otp_enable_int(p_otp, INT_PRG);
	otp_enable_int(p_otp, INT_PRGERR);
	otp_set_int_mask(p_otp, INT_PRG | INT_PRGERR, 0);

	/* trigger interrupt via doing write*/
	if(0 != otp_prog_bit_wrapper(p_otp, 1023, 1, 1))
	{
		info("opt_int: test fail for program fail 1\n");
		return 0;
	}
	
#if 0
	if(OTP_ST_DONE != otp_prog_bit(p_otp, 1, 1, 1))
	{
		info("opt_int: test fail for program fail 2\n");
		return 0;
	}	
#endif

	mdelay(500);

	if (otp_int_counter == 0) {
		fail++;
	}

	if (fail) {
		debug("otp_int: test fail\n");
	} else {
		debug("otp_int: test pass\n");
	}

	/*disable interrupt*/
	irq_generic_disable_int(p_otp->int_vector);
	irq_generic_remove_handler(p_otp->int_vector, NULL, NULL);
	otp_disable_int(p_otp, INT_PRG);
	otp_disable_int(p_otp, INT_PRGERR);
	otp_set_int_mask(p_otp, INT_PRG | INT_PRGERR, INT_PRG | INT_PRGERR);

	return 0;
}


static int secure_key_parse_one(u8 *buf,int len,u8* dst)
{
	int i = 0;
	int num = 0;

	num += sprintf((char*)dst+num,"\n");
	for (i=0; i<len; i+=16) {
		num += sprintf((char*)dst+num,"%02x %02x %02x %02x %02x %02x %02x %02x - %02x %02x %02x %02x %02x %02x %02x %02x\n",
			buf[i],buf[i+1],buf[i+2],buf[i+3],
			buf[i+4],buf[i+5],buf[i+6],buf[i+7],
			buf[i+8],buf[i+9],buf[i+10],buf[i+11],
			buf[i+12],buf[i+13],buf[i+14],buf[i+15]);
	}
	return num;
}
static int secure_key_parse(int id,char* buf)
{
	otp_secu_t *p_otp_secu = NULL;
	u8* p_data = NULL;
	int size = NULL;
	int num = 0;
	u32 val = 0;

#ifndef TAPEOUT_V2
	p_otp_secu = (otp_secu_t *)(p_otp->regs + OTP_SECURE);
#else
	int i = 0;
	int ret = 0;
	
	p_otp_secu = (otp_secu_t *)test_send_buff;
	p_data = (u8*)p_otp_secu;
	for(i=0; i<512; i++) {
		ret = otp_get_byte_wrapper(p_otp, (OTP_SECURE + i), p_data + i);
		if(ret != 0) {
			info("get sec byte %d failed %d\n", i, ret);
			return 0;
		}
	}
#endif
	switch (id) {
		case 0:
			p_data = p_otp_secu->itron_key;
			size = sizeof(p_otp_secu->itron_key);
			num = sprintf(buf,"itron key: ");
			break;
		case 1:
			p_data = p_otp_secu->itron_root_cer;
			size = sizeof(p_otp_secu->itron_root_cer);
			num = sprintf(buf,"itron root cert: ");
			break;
		case 2:
			p_data = p_otp_secu->itron_code_sign_root_cer;
			size = sizeof(p_otp_secu->itron_code_sign_root_cer);
			num = sprintf(buf,"itron code sign cert(hw): ");
			break;
		case 3:
			p_data = p_otp_secu->itron_soc_cer;
			size = sizeof(p_otp_secu->itron_soc_cer);
			num = sprintf(buf,"itron soc cert: ");
			break;
		case 4:
			p_data = p_otp_secu->itron_soc_cer_private_key;
			size = sizeof(p_otp_secu->itron_soc_cer_private_key);
			num = sprintf(buf,"itron soc cert private(hw): ");
			break;
		case 5:
			p_data = p_otp_secu->itron_fw_dec_sym_key;
			size = sizeof(p_otp_secu->itron_fw_dec_sym_key);
			num = sprintf(buf,"itron fw dec key(hw): ");
			break;
		case 6:
			p_data = p_otp_secu->itron_hw_sym_key;
			size = sizeof(p_otp_secu->itron_hw_sym_key);
			num = sprintf(buf,"itron hw sym key(hw): ");
			break;
		case 7:
			p_data = p_otp_secu->itron_sec_key1;
			size = sizeof(p_otp_secu->itron_sec_key1);
			num = sprintf(buf,"itron key1: ");
			break;
		case 8:
			p_data = p_otp_secu->itron_sec_key2;
			size = sizeof(p_otp_secu->itron_sec_key2);
			num = sprintf(buf,"itron key2: ");
			break;
		case 9:
			p_data = p_otp_secu->itron_sec_key3;
			size = sizeof(p_otp_secu->itron_sec_key3);
			num = sprintf(buf,"itron key3: ");
			break;
		case 10:
			p_data = p_otp_secu->itron_sec_key4;
			size = sizeof(p_otp_secu->itron_sec_key4);
			num = sprintf(buf,"itron key4: ");
			break;
		case 11:
			p_data = p_otp_secu->itron_sec_key5;
			size = sizeof(p_otp_secu->itron_sec_key5);
			num = sprintf(buf,"itron key5: ");
			break;
		case 12:
			p_data = p_otp_secu->itron_sec_key6;
			size = sizeof(p_otp_secu->itron_sec_key6);
			num = sprintf(buf,"itron key6: ");
			break;
		case 13:
			p_data = p_otp_secu->itron_sec_key7;
			size = sizeof(p_otp_secu->itron_sec_key7);
			num = sprintf(buf,"itron key7: ");
			break;
		default:
			p_data = p_otp_secu->itron_revd;
			size = sizeof(p_otp_secu->itron_revd);
			num = sprintf(buf,"itron reserved: ");
			break;
	}
	/*check disabled, if disabled don't display*/
	val = readl(p_otp->regs + OTP_KEY_DISABLE);
	if (val & (1<< id))
		num+= sprintf(buf+num," disabled\n");
	else
		num+= secure_key_parse_one(p_data,size,(u8*)buf+num);
	return num;
}

/**
 * @brief otp_secure_show - dump all secure data
 *
 * @return directly if failed
 */
static int otp_secure_show(int argc, char* argv[])
{
	int i = 0;
	int num = 0;
	char* buf = (char*)test_recv_buff;
	int ret = 0;

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}
	for (i=0;i<15;i++) {
		num += secure_key_parse(i,buf+num);
	}
	buf[num] = 0;
	info("secure otp dump:\n");
	info("%s\n",buf);
	return 0;
}

/**
 * @brief otp_non_secure_show - dump all non secure data
 *
 * @return directly if failed
 */
static int otp_non_secure_show(int argc, char* argv[])
{
	int i = 0;
	int num = 0;
	u8 *rx_data = NULL;
	u8 * buf = test_recv_buff;
	int ret = 0;

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}

#ifndef TAPEOUT_V2
	rx_data = (u8 *)(p_otp->regs + OTP_NON_SECURE);
#else
	memset(test_send_buff, 0x0, sizeof(test_send_buff));
	rx_data = test_send_buff;
	for(i=0; i<512; i++) {
		ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + i), rx_data+i);
		if(ret != 0) {
			info("get byte failed %d\n", ret);
			return 0;
		}		
	}
#endif
	for (i=0;i< 512;i+=16) {
		num += sprintf((char*)buf+num,"%04x: %02x %02x %02x %02x %02x %02x %02x %02x - %02x %02x %02x %02x %02x %02x %02x %02x\n",
			i,
			rx_data[i],rx_data[i+1],rx_data[i+2],rx_data[i+3],
			rx_data[i+4],rx_data[i+5],rx_data[i+6],rx_data[i+7],
			rx_data[i+8],rx_data[i+9],rx_data[i+10],rx_data[i+11],
			rx_data[i+12],rx_data[i+13],rx_data[i+14],rx_data[i+15]);
	}
	buf[num] = 0;
	info("none secure otp dump:\n");
	info("%s\n",buf);
	return 0;
}

/**
 * @brief otp_non_secure_show_robust - dump all non secure data robust test
 *
 * @return directly if pressing 'q'
 */
static int otp_non_secure_show_robust(int argc, char* argv[])
{
	int i = 0;
	int num = 0;
	u8 *rx_data = NULL;
	u8 * buf = test_recv_buff;
	int ret = 0;
	//otp_test_init();

	int fail = 0;
	int test_count = 0;
	int test_fail = 0;

	info("otp_non_secure_show_robust:otp_non_secure_show_robust robust test,press 'q' to quit\n");

	while (1) {
		if (console_check_char('q'))
			break;

		memset(test_send_buff, 0x0, TEST_BUFFER_SIZE/2);
		memset(test_recv_buff, 0x0, TEST_BUFFER_SIZE/2);

		rx_data = test_send_buff;
		buf = test_recv_buff;
		num = 0;
		for(i=0; i<512; i++) {
			if (console_check_char('q')) {
				info("Quit: i = %d\n", i);
				info("otp_non_secure_show_robust test terminate,total %d failed %d\n", test_count, test_fail);
				return -1;
			}
			ret = otp_test_init();
			if(ret != 0) {
				info("robust loop %d: init autodownload row %d result fail\n", test_count, i);
				fail++;
			}		
			ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + i), rx_data+i);
			if(ret != 0) {
				info("robust loop %d: get wrapper row %d request lock result fail\n", test_count, i);
				fail++;
			}		
		}

		/*check result*/
		if((rx_data[8] != 0x40)	|| (rx_data[0xc] != 0x80) ||
			(rx_data[0x14] != 0x40) || (rx_data[0x18] != 0x80)) {
			info("1. check result fail \n");
			fail++;
		}
		for (i = 0x30; i < 0xf0; i++) {
			if (rx_data[i] != i) {
				fail++;
				info("2. check result fail\n");
				break;
			}
		}

		test_count++;
		if (fail) {
			test_fail++;
			
			for (i=0;i< 512;i+=16) {
				num += sprintf((char*)buf+num,"%04x: %02x %02x %02x %02x %02x %02x %02x %02x - %02x %02x %02x %02x %02x %02x %02x %02x\n",
					i,
					rx_data[i],rx_data[i+1],rx_data[i+2],rx_data[i+3],
					rx_data[i+4],rx_data[i+5],rx_data[i+6],rx_data[i+7],
					rx_data[i+8],rx_data[i+9],rx_data[i+10],rx_data[i+11],
					rx_data[i+12],rx_data[i+13],rx_data[i+14],rx_data[i+15]);
			}
			buf[num] = 0;
			info("none secure otp dump:\n");
			info("%s\n",buf);
			info("%d loop %s,total failed %d\n", test_count, fail?"fail":"pass", test_fail);
		}
		if (test_count % 100 == 0)
			info("%d loop %s,total failed %d\n", test_count, fail?"fail":"pass", test_fail);
		fail = 0;
	}

	info("otp_non_secure_show_robust test terminate,total %d failed %d\n", test_count, test_fail);
	return 0;
}

/**
 * @brief otp_config_show - dump non secure first 4 byte config
 *
 * @return directly if failed
 */
static int otp_config_show(int argc, char* argv[])
{
	int num = 0;
	u8 *rx_data = NULL;
	u8 val = 0;
	u8 * buf = test_recv_buff;
#ifdef TAPEOUT_V2
	int i = 0;
#endif
	int ret = 0;

	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return 0;
	}
#ifndef TAPEOUT_V2
	rx_data = (u8 *)(p_otp->regs + OTP_NON_SECURE);
#else
	rx_data = test_send_buff;	
	for(i=0; i<512; i++) {
		ret = otp_get_byte_wrapper(p_otp, (OTP_NON_SECURE + i), rx_data+i);
		if(ret != 0) {
			info("get byte failed %d\n", ret);
			return 0;
		}
	}
#endif

	val = rx_data[0];
#ifdef TAPEOUT_V2
	val |= rx_data[BACKUP_REGION_OFFSET];
#endif
	num += sprintf(buf+num,"otp test:     %s\n",(val&0x1)?"disable":"enable");
#ifndef TAPEOUT_V2
	num += sprintf(buf+num,"otp secure pgm:   %s\n",(val&0x2)?"disable":"enable");
#else
	num += sprintf(buf+num,"otp pgm:   %s\n",(val&0x2)?"disable":"enable");
#endif
	num += sprintf(buf+num,"jtag: 	      %s\n",(val&0x4)?"disable":"enable");
	num += sprintf(buf+num,"uart download:%s\n",(val&0x8)?"disable":"enable");
	num += sprintf(buf+num,"usb download: %s\n",(val&0x10)?"disable":"enable");
	num += sprintf(buf+num,"spi download: %s\n",(val&0x20)?"disable":"enable");
	num += sprintf(buf+num,"nand boot:    %s\n",(val&0x40)?"disable":"enable");
	num += sprintf(buf+num,"secure boot:  %s\n",(val&0x80)?"disable":"enable");

	val = rx_data[1];
#ifdef TAPEOUT_V2
	val |= rx_data[BACKUP_REGION_OFFSET + 1];
#endif
	num += sprintf(buf+num,"uart 0: %s\n",(val&0x1)?"disable":"enable");
	num += sprintf(buf+num,"uart 1: %s\n",(val&0x2)?"disable":"enable");
	num += sprintf(buf+num,"uart 2: %s\n",(val&0x4)?"disable":"enable");
	num += sprintf(buf+num,"uart 3: %s\n",(val&0x8)?"disable":"enable");
	num += sprintf(buf+num,"spi 0:  %s\n",(val&0x10)?"disable":"enable");
	num += sprintf(buf+num,"spi 1:  %s\n",(val&0x20)?"disable":"enable");
	num += sprintf(buf+num,"spi 2:  %s\n",(val&0x40)?"disable":"enable");
	num += sprintf(buf+num,"i2c 0:  %s\n",(val&0x80)?"disable":"enable");

	val = rx_data[2];
#ifdef TAPEOUT_V2
	val |= rx_data[BACKUP_REGION_OFFSET + 2];
#endif
	num += sprintf(buf+num,"i2c 1:  %s\n",(val&0x1)?"disable":"enable");
	num += sprintf(buf+num,"i2c 2:  %s\n",(val&0x2)?"disable":"enable");
	num += sprintf(buf+num,"usb 0:  %s\n",(val&0x4)?"disable":"enable");
	num += sprintf(buf+num,"usb 1:  %s\n",(val&0x8)?"disable":"enable");
	num += sprintf(buf+num,"sdio 0: %s\n",(val&0x10)?"disable":"enable");
	num += sprintf(buf+num,"sdio 1: %s\n",(val&0x20)?"disable":"enable");

	buf[num] = 0;
	info("none secure otp config dump:\n");
	info("%s\n",buf);
	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************

cmdline_entry otp_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{"otp_non_secure_show",	otp_non_secure_show,": dump all non secure data" },
	{"otp_non_secure_show_robust",	otp_non_secure_show_robust,": otp_non_secure_show_robust" },
	{"otp_config_show",otp_config_show,": dump non secure first 4 byte config" },
	{"otp_secure_show",otp_secure_show,": dump all secure data" },
	{"otp_set_onebit",otp_set_onebit,": otp set one bit" },
	{"otp_get_onebit",otp_get_onebit,": otp get one bit" },
	{"otp_set_onebyte",otp_set_onebyte,": otp set one byte" },
	{"otp_get_onebyte",otp_get_onebyte,": otp get one byte" },
	{"otp_test",	otp_test,	"  : otp test program" },
	{"otp_key_disable",	otp_key_disable,"  : otp key disable test" },
	{"otp_low_power_test",	otp_low_power_test,"  : otp_low_power_test" },
	{ "otp_int",otp_int, "  : otp interrupt test" },{ 0, 0, 0 }
};

#define OTP_PRG_PARAM0_DEF_VAL	0xEF27779
bool otp_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s not support in HW\n", __func__);
	return false;
}

bool otp_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_OTP;
	
	struct brite_otp *p_dev = p_otp;
	int ret = 0;

	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	ret = otp_test_init();
	if (ret != 0) {
		info("err: otp test init failed %d\n", ret);
		return false;
	}
	// write register
	info("write prg param0 reg\n");
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + OTP_PRG_PARAM0);

	// read&compare
	info("read&compare prg param0 reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + OTP_PRG_PARAM0);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write prg param0 reg\n");
	__raw_writel((*p_wr_val+1), (uint32_t)p_dev->regs + OTP_PRG_PARAM0);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare prg param0 reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + OTP_PRG_PARAM0);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool otp_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool otp_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

