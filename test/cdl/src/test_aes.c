/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_aes.h"
#include "memleak.h"

#if (CONFIG_AES_TEST == 1)

cmdline_entry aes_test_menu[];

extern struct brite_aes* p_aes_dev;

#define ALIGN_UP(n, align)	(((n)+(align)-1)&~((align)-1))
#define ALIGN_DOWN(n, align)	((n)&~((align)-1))

#define swab32(x)  \
	((u32) ( \
		(((u32)(x) & (u32)0x000000ffUL) << 24) | \
		(((u32)(x) & (u32)0x0000ff00UL) <<  8) | \
		(((u32)(x) & (u32)0x00ff0000UL) >>  8) | \
		(((u32)(x) & (u32)0xff000000UL) >> 24) ))

unsigned char otp_key1[32] = {
	0x34, 0xf1, 0x4b, 0xd9, 0x1b, 0x32, 0x0f, 0x05,
	0xb1, 0x5c, 0x6e, 0x72, 0x9d, 0xec, 0x42, 0x74,
	0xd1, 0xae, 0x29, 0x36, 0x53, 0xe1, 0x89, 0x86,
	0xd8, 0x88, 0xfe, 0xcc, 0x39, 0xbb, 0x30, 0x81
};
unsigned char otp_key2[32] = {
	0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
	0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
	0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
	0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25
};

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&aes_test_menu[0]);
}
static void aes_print_buffer(char*name,unsigned char* buf,int len)
{
	int i,j;
	int line;
	int rest;
	if (buf == NULL)
		return;
	info("%s(%d):\n",name,len);
	rest = len;
	for (i=0;i<len;i+=16) {
		if (rest>16)
			line = 16;
		else
			line = rest;
		for (j=0;j<line;j++)
			info("0x%02x, ",*(buf+i+j));
		info("\n");
		rest -= line;
	}
}

void aes_test_init(void)
{
	aes_init(AES_BASE);
#ifndef TAPEOUT_V2	
	p_aes_dev->dmac_idx = 0;
#else
	p_aes_dev->dmac_idx = 2;
#endif
	p_aes_dev->dma_ch_tx = 0;
	p_aes_dev->dma_ch_rx = 1;
	p_aes_dev->dma_req_tx = DMA_REQ_AES_IN;
	p_aes_dev->dma_req_rx = DMA_REQ_AES_OUT;
	p_aes_dev->int_vector = INT_AES0;
}

/**
 * @brief KEK KEY Generation
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [key_idx] input, indicates which key will be chosen to generate the new key, should be 7-13
 *
 * @return directly if failed
 */
static int KEK_KEY_Generate(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	int keylen = 256;
	int mode = 0;
	int keymode = 0;
	int key_idx = 7;
	u32 testlen = 32;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;
	int error = 0;
	int index = 0;
	symmetric_ECB ecbAES = { 0 };
	int ret = 0;

	info("cmd format: KEK_KEY_Generate [mode][keymode][key_idx]\n");
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:256bit, testlen:32, key_idx:[7-13]\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc > 2) {
		keymode = get_arg_ulong(argv[2]);
	}
	if (argc > 3) {
		key_idx = get_arg_ulong(argv[3]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if(key_idx < 7 || key_idx > 13) {
		info("key number should be [7-13]\n");
		return 0;
	}

	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}

	info("aes %s test,keylen:%dbit keymode:%s testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		testlen);

	/* key */
	memcpy(key,otp_key2,sizeof(otp_key2));

	/*generate test plaintext*/
	for (i = 0; i < testlen; i++) {
		pbuf[i] = key_idx + i;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ecb encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		ret = aes_ecb_start(1<<keymode,key,keylen/8);
		if (ret) {
			info("hw ecb start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_ecb_enc(pbuf,hw_enc_buff,testlen);
		if (ret) {
			info("hw ecb enc fail\n");
			fail++;
			goto end1;
		}

		/*do decrypt*/
		ret = aes_ecb_dec(hw_enc_buff,hw_dec_buff,testlen);
		if (ret) {
			info("hw ecb dec fail\n");
			fail++;
			goto end1;
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw ecb encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ecb encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("sw register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}
		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		error = ecb_start(index,key,keylen/8,0,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = ecb_encrypt(pbuf,sw_enc_buff,testlen,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_encrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}
		ecb_done(&ecbAES);

		error = ecb_start(index,key,keylen/8,0,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = ecb_decrypt(sw_enc_buff,sw_dec_buff,testlen,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_decrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		ecb_done(&ecbAES);

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);

end1:
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief AES ECB Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [testlen] input, The length of the plain text, should be 0-2^28-1 and actual limited by the memory
 *
 * @return directly if failed
 */
static int aes_ecb(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	int keylen = 256;
	int mode = 0;
	int keymode = 0;
	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	symmetric_ECB ecbAES = { 0 };
	int ret = 0;
	info("aes_ecb: test aes ecb mode: aes_ecb [mode] [keylen] [keymode] [testlen]\n");
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:128: 128bit| 256: 256bit\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		testlen = get_arg_ulong(argv[4]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if (testlen & 0xf) {
		info("ecb plain text must be mutiple of 16\n");
		return 0;
	}

	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}
	info("aes %s test,keylen:%dbit keymode:%s testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		testlen);

	/*generate random key */
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ecb encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		ret = aes_ecb_start(1<<keymode,key,keylen/8);
		if (ret) {
			info("hw ecb start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_ecb_enc(pbuf,hw_enc_buff,testlen);
		if (ret) {
			info("hw ecb enc fail\n");
			fail++;
			goto end1;
		}

		/*do decrypt*/
		ret = aes_ecb_dec(hw_enc_buff,hw_dec_buff,testlen);
		if (ret) {
			info("hw ecb dec fail\n");
			fail++;
			goto end1;
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ecb encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}
		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		error = ecb_start(index,key,keylen/8,0,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = ecb_encrypt(pbuf,sw_enc_buff,testlen,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_encrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}
		ecb_done(&ecbAES);

		error = ecb_start(index,key,keylen/8,0,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = ecb_decrypt(sw_enc_buff,sw_dec_buff,testlen,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_decrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		ecb_done(&ecbAES);

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);

end1:
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	dbg_mem_stat();
	return 0;
}

/**
 * @brief AES CBC Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [testlen] input, The length of the plain text, should be 0-2^28-1 and actual limited by the memory
 *
 * @return directly if failed
 */
static int aes_cbc(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char iv[16] = { 0 };
	unsigned char sw_enc_out_iv[16] = {0};
	unsigned char sw_dec_out_iv[16] = {0};
	unsigned char hw_enc_out_iv[16] = {0};
	unsigned char hw_dec_out_iv[16] = {0};
	int keylen = 256;
	int ivlen = 16;
	int mode = 0;
	int keymode = 0;

	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	symmetric_CBC cbcAES = { 0 };
	int ret = 0;

	info("aes_cbc: test aes cbc mode: aes_cbc [mode] [keylen] [keymode] [testlen]\n");
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:128: 128bit| 256: 256bit\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		testlen = get_arg_ulong(argv[4]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if (testlen & 0xf) {
		info("cbc plain text must be mutiple of 16\n");
		return 0;
	}
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}
	info("aes %s test,keylen:%dbit keymode:%s testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;

	for (i=0;i<16;i++)
		iv[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes cbc encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		ret = aes_cbc_start(1<<keymode,key,iv,keylen/8);
		if (ret) {
			info("hw cbc start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		aes_cbc_enc(pbuf,hw_enc_buff,testlen,hw_enc_out_iv);
		if (ret) {
			info("hw cbc enc fail\n");
			fail++;
			goto end1;
		}

		/*do decrypt*/
		ret = aes_cbc_start(1<<keymode,key,iv,keylen/8);
		if (ret) {
			info("hw cbc start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_cbc_dec(hw_enc_buff,hw_dec_buff,testlen,hw_dec_out_iv);
		if (ret) {
			info("hw cbc dec fail\n");
			fail++;
			goto end1;
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes cbc encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("sw register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		error = cbc_start(index,iv,key,keylen/8,0,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = cbc_encrypt(pbuf,sw_enc_buff,testlen,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_encrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}
		cbc_done(&cbcAES);
		memcpy(sw_enc_out_iv,cbcAES.IV,sizeof(iv));

		error = cbc_start(index,iv,key,keylen/8,0,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = cbc_decrypt(sw_enc_buff,sw_dec_buff,testlen,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_decrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		cbc_done(&cbcAES);
		memcpy(sw_dec_out_iv,cbcAES.IV,sizeof(iv));

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare hw/sw iv result*/
		for (i=0;i<sizeof(iv);i++) {
			if (sw_enc_out_iv[i] != hw_enc_out_iv[i]) {
				info("hw/sw cbc enc outiv result diff @%d\n",i);
				fail++;
				goto done;
			}
			if (sw_dec_out_iv[i] != hw_dec_out_iv[i]) {
				info("hw/sw cbc dec outiv result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("iv",iv,ivlen);
	aes_print_buffer("sw_enc_out_iv",sw_enc_out_iv,ivlen);
	aes_print_buffer("hw_enc_out_iv",hw_enc_out_iv,ivlen);
	aes_print_buffer("sw_dec_out_iv",sw_dec_out_iv,ivlen);
	aes_print_buffer("hw_dec_out_iv",hw_dec_out_iv,ivlen);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);

end1:
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief AES CCM Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [noncelen] input, The length of the nonce, Nlen: 7-13
 * @param [headerlen] input, The length of the associated data A, Alen: 0-2^32-1
 * @param [maclen] input, The length of the authentication tag, Tlen:[4,6,8,10,12,14,16]
 * @param [testlen] input, The length of the plain text, should be 0-2^28-1 and actual limited by the memory
 *
 * @return directly if failed
 */
static int aes_ccm(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char nonce[16] = { 0 };
	unsigned char *header = NULL;
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	int noncelen = 13;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 testlen = 2048;
	u64 maxlen = 0;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	int ret = 0;

	info("aes_ccm: test aes ccm mode: %s [mode] [keylen] [keymode] [noncelen] [headerlen] [maclen] [testlen]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw only|2 hw only \n");
	info("keylen:128: 128bit| 256: 256bit\n");
	info("keymode: 0 sw key | 1 otp key1 | 2 otp key2\n");
	info("noncelen: 7-13: notice this parameter is relative with testlen,testlen max len = 2^((15-noncelen)*8)-1\n");
	info("headerlen: 0 - 2^32-1 (actual limit by dynamic memory allocated)\n");
	info("maclen: 4,6,8,10,12,14,16\n");
	info("testlen: plain text length,0 - 2^28-1 (actual limit by dynamic memory allocated)\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		noncelen = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		headerlen = get_arg_ulong(argv[5]);
	}
	if (argc >6) {
		maclen = get_arg_ulong(argv[6]);
	}
	if (argc >7) {
		testlen = get_arg_ulong(argv[7]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}	
	if ((noncelen < 7) || (noncelen > 13)) {
		info("noncelen should be 7-13\n");
		return 0;
	}
	/* make sure the maclen is even and >=4 && <= 16 */
	if((maclen & 1) || maclen > 16 || maclen < 4) {
		info("maclen should be 4,6,8,10,12,14,16\n");
		return 0;
	}

	maxlen = (1ULL << ((15 - noncelen) * 8)) - 1;
	if (maxlen > ((1UL << 28) - 1)) {
		maxlen = ((1UL << 28) - 1);
	}
	if (testlen > maxlen) {
		info("testlen:%u too large, max: %llu\n", testlen, maxlen);
		return 0;
	}
	if (headerlen) {
		header = (unsigned char*)malloc(headerlen);
		if (!header) {
			info("allocate memory for aad info fail\n");
			fail++;
			goto end1;
		}
	}

	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}

	info("aes %s test,keylen:%dbit keymode:%s noncelen:%d headerlen:%u maclen:%d testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		noncelen,headerlen,maclen,testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;
	for (i=0;i<16;i++)
		nonce[i] = cb_rand() & 0xff;
	for (i=0;i<headerlen;i++)
		header[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ccm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,maclen);
		if (ret) {
			info("hw ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		aes_print_buffer("B",(unsigned char *)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));
		ret = aes_ccm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("hw ccm enc fail\n");
			fail++;
			goto end1;
		}
		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,maclen);
		if (ret) {
			info("hw ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_ccm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("hw ccm dec fail\n");
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}
	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ccm encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("sw register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("sw ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("sw ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("nonce",nonce,noncelen);
	aes_print_buffer("aad",header,((headerlen > 256) ? 256 : headerlen));
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	aes_ccm_done();
	if (header)
		free(header);
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	
	dbg_mem_stat();
	return 0;
}

/**
 * @brief AES CCM Encrypt/Decrypt with the fixed data
 *
 * @return directly if failed
 */
static int aes_ccm_fixdata(int argc, char* argv[])
{
	unsigned char key[16] = { 0 };
	unsigned char nonce[8] = { 0 };
	unsigned char header[16] = { 0 };
	unsigned char tag_result[6] = { 0 };
	unsigned char tag1[6] = { 0 };
	unsigned char tag2[6] = { 0 };
	int keylen = 128;
	int noncelen = 8;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 6;
	int keymode = 0;
	int mode = 0;
	u32 testlen = 16;
	int fail = 0;
	int i = 0;
	u8 pbuf[16] = { 0 };
	u8 hw_enc_buff[16] = { 0 };
	u8 hw_dec_buff[16] = { 0 };
	u8 sw_enc_buff[16] = { 0 };
	u8 sw_dec_buff[16] = { 0 };

	int error = 0;
	int index = 0;
	int ret = 0;
	info("aes_ccm_fixdata\n");

	for (i=0; i<keylen/8; i++)
		key[i] = 0x40+i;

	for (i=0;i<noncelen;i++)
		nonce[i] = 0x10+i;

	for (i=0;i<headerlen;i++)
		header[i] = i;
	info("aes %s test,keylen:%dbit keymode:%s noncelen:%d headerlen:%u maclen:%d testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		noncelen,headerlen,maclen,testlen);

	/*generate random key and iv*/
	srand(get_ticks());

	/*generate test plaintext*/
	for (i=0;i<testlen;i++)
		pbuf[i] = 0x20+i;

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ccm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,maclen);
		if (ret) {
			info("ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		aes_print_buffer("B",(unsigned char*)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));
		ret = aes_ccm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("ccm enc fail\n");
			fail++;
			goto end1;
		}

		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,maclen);
		if (ret) {
			info("ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_ccm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("ccm dec fail\n");
			fail++;
			goto end1;
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ccm encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("sw register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("sw ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("sw ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("nonce",nonce,noncelen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,maclen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,maclen);
	}

end1:
	aes_ccm_done();
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	dbg_mem_stat();
	return 0;
}

/**
 * @brief AES GCM Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [ivlen] input, The length of the initial value data. should be 12
 * @param [headerlen] input, The length of the associated data A, Alen: 0-2^32-1
 * @param [maclen] input, The length of the authentication tag, Tlen:12-16
 * @param [testlen] input, The length of the plain text, should be 0-2^28-1 and actual limited by the memory
 *
 * @return directly if failed
 */
static int aes_gcm(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char iv[16] = { 0 };
	unsigned char *header = NULL;
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	int ivlen = 12;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	int ret = 0;

	info("aes_gcm: test aes gcm mode: %s [mode] [keylen] [keymode] [ivlen] [headerlen] [maclen] [testlen]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw only|2 hw only \n");
	info("keylen:128: 128bit| 256: 256bit\n");
	info("keymode: 0 sw key | 1 otp key1 | 2 otp key2\n");
	info("ivlen: 12\n");
	info("headerlen: 0 - 2^32-1 (actual limit by dynamic memory allocated)\n");
	info("maclen: 12,13,14,15,16\n");
	info("testlen: plain text length,0 - 2^28-1 (actual limit by dynamic memory allocated)\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		ivlen = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		headerlen = get_arg_ulong(argv[5]);
	}
	if (argc >6) {
		maclen = get_arg_ulong(argv[6]);
	}
	if (argc >7) {
		testlen = get_arg_ulong(argv[7]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if (ivlen != 12) {
		info("ivlen should be 12\n");
		return 0;
	}
#if 0
	maclen &= ~1;
	if (maclen>16)
		maclen=16;
	else if (maclen <4)
		maclen = 4;
#endif
	if ((maclen < 12) || (maclen > 16)) {
		info("maclen should be 12-16\n");
		return 0;
	}

	if (headerlen) {
		header = (unsigned char*)malloc(headerlen);
		if (!header) {
			info("allocate memory for aad info fail\n");
			fail++;
			goto end1;
		}
	}
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}
	info("aes %s test,keylen:%dbit keymode:%s ivlen:%d headerlen:%u maclen:%d testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		ivlen,headerlen,maclen,testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;
	for (i=0;i<16;i++)
		iv[i] = cb_rand() & 0xff;
	for (i=0;i<headerlen;i++)
		header[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes gcm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_gcm_start(1<<keymode,key,keylen/8,iv,ivlen,header,headerlen,testlen,maclen);
		if (ret) {
			info("gcm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		aes_print_buffer("B",(unsigned char *)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));
		ret = aes_gcm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("gcm enc fail\n");
			fail++;
			goto end1;
		}

		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_gcm_start(1<<keymode,key,keylen/8,iv,ivlen,header,headerlen,testlen,maclen);
		if (ret) {
			info("gcm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_gcm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("gcm dec fail\n");
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes gcm encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("sw register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = gcm_memory(index,
			key, keylen/8,
			iv, ivlen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, GCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("sw gcm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = gcm_memory(index,
			key, keylen/8,
			iv, ivlen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, GCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("sw gcm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("iv",iv,ivlen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	aes_gcm_done();
	if (header)
		free(header);
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	dbg_mem_stat();
	return 0;
}

/**
 * @brief AES GCM Encrypt/Decrypt with the fixed data
 *
 * @return directly if failed
 */
static int aes_gcm_fixdata(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char iv[16] = { 0 };
	unsigned char header[32] = { 0 };
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	int ivlen = 12;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 12;
	int keymode = 0;
	int mode = 0;
	u32 testlen = 16;
	int fail = 0;
	int i = 0;
	u8 pbuf[16] = { 0 };
	u8 hw_enc_buff[16] = { 0 };
	u8 hw_dec_buff[16] = { 0 };
	u8 sw_enc_buff[16] = { 0 };
	u8 sw_dec_buff[16] = { 0 };

	int error = 0;
	int index = 0;
	u32 *ptr;
	int ret = 0;
	info("aes_gcm_fixdata\n");

	/*
	[Keylen = 256]
	[IVlen = 96]
	[PTlen = 128]
	[AADlen = 128]
	[Taglen = 32]

	Count = 0
	Key = 857fc7d0670744d02407b6ff7330c59d1727fa371ba0ea02a39e7d059640f11f
	IV = 1bf14482b7b5afe3dade8dd1
	PT = 5b7ef3281a816c4a0a95acdf3e166d6d
	AAD = d9fe058ded673b1976e3322c35f73e08
	CT = 10db57cdffb1e43b27cc568e5fed98d6
	Tag = d9c68cd2
	*/

	ptr = (u32*)key;
	*(ptr+0) = swab32(0x857fc7d0);
	*(ptr+1) = swab32(0x670744d0);
	*(ptr+2) = swab32(0x2407b6ff);
	*(ptr+3) = swab32(0x7330c59d);
	*(ptr+4) = swab32(0x1727fa37);
	*(ptr+5) = swab32(0x1ba0ea02);
	*(ptr+6) = swab32(0xa39e7d05);
	*(ptr+7) = swab32(0x9640f11f);

	ptr = (u32*)iv;
	*(ptr+0) = swab32(0x1bf14482);
	*(ptr+1) = swab32(0xb7b5afe3);
	*(ptr+2) = swab32(0xdade8dd1);

	ptr = (u32*)header;
	*(ptr+0) = swab32(0xd9fe058d);
	*(ptr+1) = swab32(0xed673b19);
	*(ptr+2) = swab32(0x76e3322c);
	*(ptr+3) = swab32(0x35f73e08);

	info("aes %s test,keylen:%dbit keymode:%s ivlen:%d headerlen:%u maclen:%d testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		ivlen,headerlen,maclen,testlen);

	/*generate random key and iv*/
	srand(get_ticks());

	/*generate test plaintext*/
	ptr = (u32*)pbuf;
	*(ptr+0) = swab32(0x5b7ef328);
	*(ptr+1) = swab32(0x1a816c4a);
	*(ptr+2) = swab32(0x0a95acdf);
	*(ptr+3) = swab32(0x3e166d6d);

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes gcm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_gcm_start(1<<keymode,key,keylen/8,iv,ivlen,header,headerlen,testlen,maclen);
		if (ret) {
			info("gcm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_gcm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("gcm enc fail\n");
			fail++;
			goto end1;
		}
		aes_print_buffer("B",(unsigned char*)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));

		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_gcm_start(1<<keymode,key,keylen/8,iv,ivlen,header,headerlen,testlen,maclen);
		if (ret) {
			info("gcm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_gcm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("gcm dec fail\n");
			fail++;
			goto end1;
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes gcm encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = gcm_memory(index,
			key, keylen/8,
			iv, ivlen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, GCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("sw gcm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = gcm_memory(index,
			key, keylen/8,
			iv, ivlen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, GCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("sw gcm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("iv",iv,ivlen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	aes_gcm_done();
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief AES CMAC Encrypt
 * @param [mode] input, the encrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [maclen] input, The length of the authentication tag, Tlen:0-16
 * @param [testlen] input, The length of the plain text, should be 0-2^28-1 and actual limited by the memory
 *
 * @return directly if failed
 */
static int aes_cmac(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 testlen = 2048;
	u8 *pbuf = NULL;
	int fail = 0;
	int i = 0;

	int error = 0;
	int index = 0;
	int ret = 0;

	info("aes_cmac: test aes cmac mode: aes_cmac [mode] [keylen] [keymode] [maclen] [testlen] \n");
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:128: 128bit| 256: 256bit\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		maclen = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		testlen = get_arg_ulong(argv[5]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if(maclen > 16 || maclen < 0) {
		info("maclen should be 0-16\n");
		return 0;
	}

	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	info("aes %s test,keylen:%dbit keymode:%s maclen:%d testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		maclen,
		testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes cmac encrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_cmac_start(1<<keymode,key,keylen/8,maclen,testlen);
		if (ret) {
			info("hw aes_cmac_start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_cmac_enc(pbuf, testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("hw cmac enc fail\n");
			fail++;
			goto end1;
		}

		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes cmac encrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}
		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = omac_memory(index,
			key, keylen/8,
			pbuf, testlen,
			tag_result, &taglen1);
		if (error != CRYPT_OK) {
			info("sw omac_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		memcpy(tag2,tag_result,taglen1);
	}

	if (mode==0) {
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	if (pbuf)
		free(pbuf);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief AES CCM* Encrypt/Decrypt with the fixed data from 802.15.4-2011.pdf C.2.x
 * @param [example_no] input, indicates which example will be chosen to run. should be 0-7
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 *
 * @return directly if failed
 */ 
static int aes_ccm_star(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char nonce[16] = { 0 };
	unsigned char header[32] = { 0 };
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	int noncelen = 13;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 testlen = 64;
	int fail = 0;
	int i = 0;
	u8 pbuf[64] = { 0 };
	u8 hw_enc_buff[64] = { 0 };
	u8 hw_dec_buff[64] = { 0 };
	u8 sw_enc_buff[64] = { 0 };
	u8 sw_dec_buff[64] = { 0 };
	u8 *hw_enc_compare_buff = NULL;
	u8 *hw_mac_compare_buff = NULL;

	int error = 0;
	int index = 0;
	int example_no = 0;
	int ret = 0;

	/* example 0: 802.15.4-2011.pdf C.2.1 MAC beacon frame
	[Keylen = 128 bit]
	[noncelen = 13 byte]
	[PTlen = 0 byte]
	[AADlen = 26 byte]
	[Taglen = 8 byte]

	Key = C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF
	nonce = AC DE 48 00 00 00 00 01 || 00 00 00 05 || 02
	PT = null
	AAD = 08 D0 84 21 43 01 00 00 00 00 48 DE AC || 02 || 05 00 00 00 || 55 CF 00 00 51 52 53 54.
	CT = null
	Tag = 22 3B C1 EC 84 1A B5 53
	*/
	const unsigned char example_key0[] = {
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
		0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
	};
	const unsigned char example_nonce0[] = {
		0xAC,0xDE,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x02
	};
	const unsigned char example_aad0[] = {
		0x08,0xD0,0x84,0x21,0x43,0x01,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0x02,0x05,0x00,
		0x00,0x00,0x55,0xcf,0x00,0x00,0x51,0x52,
		0x53,0x54
	};
	const unsigned char example_mac0[] = {
		0x22,0x3b,0xc1,0xec,0x84,0x1a,0xb5,0x53
	};

	/* example 1: 802.15.4-2011.pdf C.2.2 MAC data frame
	[Keylen = 128 bit]
	[noncelen = 13 byte]
	[PTlen = 4 byte]
	[AADlen = 26 byte]
	[Taglen = 0 byte]

	Key = C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF
	nonce = AC DE 48 00 00 00 00 01 || 00 00 00 05 || 04
	PT = 61 62 63 64
	AAD = 69 DC 84 21 43 02 00 00 00 00 48 DE AC 01 00 00 00 00 48 DE AC || 04 || 05 00 00 00
	CT = D4 3E 02 2B
	Tag = 0
	*/
	const unsigned char example_key1[] = {
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
		0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
	};
	const unsigned char example_nonce1[] = {
		0xAC,0xDE,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x04
	};
	const unsigned char example_aad1[] = {
		0x69,0xDC,0x84,0x21,0x43,0x02,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0x01,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0x04,0x05,0x00,
		0x00,0x00
	};
	const unsigned char example_pt1[] = {
		0x61,0x62,0x63,0x64
	};
	const unsigned char example_ct1[] = {
		0xD4,0x3e,0x02,0x2b
	};
	/* example 2: 802.15.4-2011.pdf C.2.2 MAC command frame
	[Keylen = 128 bit]
	[noncelen = 13 byte]
	[PTlen = 1 byte]
	[AADlen = 29 byte]
	[Taglen = 8 byte]

	Key = C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF
	nonce = AC DE 48 00 00 00 00 01 || 00 00 00 05 || 06
	PT = CE
	AAD = 2B DC 84 21 43 02 00 00 00 00 48 DE AC FF FF 01 00 00 00 00 48 DE AC || 06 ||
		05 00 00 00 || 01
	CT = D8
	Tag = 4F DE 52 90 61 F9 C6 F1
	*/
	const unsigned char example_key2[] = {
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
		0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF
	};
	const unsigned char example_nonce2[] = {
		0xac,0xde,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x06
	};
	const unsigned char example_aad2[] = {
		0x2b,0xDC,0x84,0x21,0x43,0x02,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0xff,0xff,0x01,
		0x00,0x00,0x00,0x00,0x48,0xde,0xac,0x06,
		0x05,0x00,0x00,0x00,0x01
	};
	const unsigned char example_pt2[] = {
		0xce
	};
	const unsigned char example_ct2[] = {
		0xd8
	};
	const unsigned char example_mac2[] = {
		0x4f,0xde,0x52,0x90,0x61,0xf9,0xc6,0xf1
	};

	/* example 3: 802.15.4-2011.pdf C.2.1 MAC beacon frame
	[Keylen = 256 bit]
	[noncelen = 13 byte]
	[PTlen = 0 byte]
	[AADlen = 26 byte]
	[Taglen = 8 byte]

	Key = C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF |
		  D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF
	nonce = AC DE 48 00 00 00 00 01 || 00 00 00 05 || 02
	PT = null
	AAD = 08 D0 84 21 43 01 00 00 00 00 48 DE AC || 02 || 05 00 00 00 || 55 CF 00 00 51 52 53 54.
	CT = null
	Tag = 22 3B C1 EC 84 1A B5 53
	*/
	const unsigned char example_key3[] = {
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
		0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
		0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
		0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF
	};
	const unsigned char example_nonce3[] = {
		0xAC,0xDE,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x02
	};
	const unsigned char example_aad3[] = {
		0x08,0xD0,0x84,0x21,0x43,0x01,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0x02,0x05,0x00,
		0x00,0x00,0x55,0xcf,0x00,0x00,0x51,0x52,
		0x53,0x54
	};
	const unsigned char example_mac3[] = {
		0x45,0x9f,0x69,0xb9,0xda,0x5c,0xd3,0x24
	};

	/* example 4: 802.15.4-2011.pdf C.2.2 MAC data frame
	[Keylen = 256 bit]
	[noncelen = 13 byte]
	[PTlen = 4 byte]
	[AADlen = 26 byte]
	[Taglen = 0 byte]

	Key = C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF |
		  D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF
	nonce = AC DE 48 00 00 00 00 01 || 00 00 00 05 || 04
	PT = 61 62 63 64
	AAD = 69 DC 84 21 43 02 00 00 00 00 48 DE AC 01 00 00 00 00 48 DE AC || 04 || 05 00 00 00
	CT = D4 3E 02 2B
	Tag = 0
	*/
	const unsigned char example_key4[] = {
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
		0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
		0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
		0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF
	};
	const unsigned char example_nonce4[] = {
		0xAC,0xDE,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x04
	};
	const unsigned char example_aad4[] = {
		0x69,0xDC,0x84,0x21,0x43,0x02,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0x01,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0x04,0x05,0x00,
		0x00,0x00
	};
	const unsigned char example_pt4[] = {
		0x61,0x62,0x63,0x64
	};
	const unsigned char example_ct4[] = {
		0xdb,0xd3,0x3e,0xb6
	};
	/* example 5: 802.15.4-2011.pdf C.2.2 MAC command frame
	[Keylen = 256 bit]
	[noncelen = 13 byte]
	[PTlen = 1 byte]
	[AADlen = 29 byte]
	[Taglen = 8 byte]

	Key = C0 C1 C2 C3 C4 C5 C6 C7 C8 C9 CA CB CC CD CE CF |
		  D0 D1 D2 D3 D4 D5 D6 D7 D8 D9 DA DB DC DD DE DF
	nonce = AC DE 48 00 00 00 00 01 || 00 00 00 05 || 06
	PT = CE
	AAD = 2B DC 84 21 43 02 00 00 00 00 48 DE AC FF FF 01 00 00 00 00 48 DE AC || 06 ||
		05 00 00 00 || 01
	CT = D8
	Tag = 4F DE 52 90 61 F9 C6 F1
	*/
	const unsigned char example_key5[] = {
		0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,
		0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
		0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,
		0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF
	};
	const unsigned char example_nonce5[] = {
		0xac,0xde,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x06
	};
	const unsigned char example_aad5[] = {
		0x2b,0xDC,0x84,0x21,0x43,0x02,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0xff,0xff,0x01,
		0x00,0x00,0x00,0x00,0x48,0xde,0xac,0x06,
		0x05,0x00,0x00,0x00,0x01
	};
	const unsigned char example_pt5[] = {
		0xce
	};
	const unsigned char example_ct5[] = {
		0xa5
	};
	const unsigned char example_mac5[] = {
		0x1d,0x51,0x57,0x02,0xb1,0xc8,0x02,0xcf
	};
	/* example 6: OTP key1
	[Keylen = 256 bit]
	[noncelen = 13 byte]
	[PTlen = 1 byte]
	[AADlen = 29 byte]
	[Taglen = 8 byte]
	*/
	const unsigned char example_key6[] = {
		0x34, 0xf1, 0x4b, 0xd9, 0x1b, 0x32, 0x0f, 0x05, 0xb1, 0x5c, 0x6e, 0x72, 0x9d, 0xec, 0x42, 0x74, 
		0xd1, 0xae, 0x29, 0x36, 0x53, 0xe1, 0x89, 0x86, 0xd8, 0x88, 0xfe, 0xcc, 0x39, 0xbb, 0x30, 0x81
	};
	const unsigned char example_nonce6[] = {
		0xac,0xde,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x06
	};
	const unsigned char example_aad6[] = {
		0x2b,0xDC,0x84,0x21,0x43,0x02,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0xff,0xff,0x01,
		0x00,0x00,0x00,0x00,0x48,0xde,0xac,0x06,
		0x05,0x00,0x00,0x00,0x01
	};
	const unsigned char example_pt6[] = {
		0xce
	};
	const unsigned char example_ct6[] = {
		0xd9
	};
	const unsigned char example_mac6[] = {
		0x17, 0x1c, 0xff, 0x33, 0x3c, 0xf2, 0x59, 0x98
	};
	/* example 7: OTP key2
	[Keylen = 256 bit]
	[noncelen = 13 byte]
	[PTlen = 1 byte]
	[AADlen = 29 byte]
	[Taglen = 8 byte]
	*/
	const unsigned char example_key7[] = {
		0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 
		0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25
	};
	const unsigned char example_nonce7[] = {
		0xac,0xde,0x48,0x00,0x00,0x00,0x00,0x01,
		0x00,0x00,0x00,0x05,0x06
	};
	const unsigned char example_aad7[] = {
		0x2b,0xDC,0x84,0x21,0x43,0x02,0x00,0x00,
		0x00,0x00,0x48,0xde,0xac,0xff,0xff,0x01,
		0x00,0x00,0x00,0x00,0x48,0xde,0xac,0x06,
		0x05,0x00,0x00,0x00,0x01
	};
	const unsigned char example_pt7[] = {
		0xce
	};
	const unsigned char example_ct7[] = {
		0xb0
	};
	const unsigned char example_mac7[] = {
		0x50, 0x12, 0x68, 0xe9, 0xf7, 0xb1, 0x3d, 0x4a
	};

	info("cmd_format: aes_ccm_star [example_no] [keymode]\n");
	info("example_no:0|1|2|3|4|5|6|7  total 8 example data\n");
	info("keymode:0 sw key | 1 otp key1 | 2 otp key2\n");
	if (argc > 1) {
		example_no = get_arg_ulong(argv[1]);
		if (example_no < 0 || example_no > 7) {
			info("wrong example_no: should be >=0 && <= 7\n");
			return 0;
		}
	}
	if (argc > 2) {
		keymode = get_arg_ulong(argv[2]);
		if (keymode < 0 || keymode > 2) {
			info("wrong keymode: should be >=0 && <= 2\n");
			return 0;
		}
	}

	switch (example_no) {
		case 1:
			keylen = sizeof(example_key1)*8;
			memcpy(key,example_key1,sizeof(example_key1));
			noncelen = sizeof(example_nonce1);
			memcpy(nonce,example_nonce1,noncelen);
			headerlen = sizeof(example_aad1);
			memcpy(header,example_aad1,headerlen);
			maclen = 0;
			testlen = sizeof(example_pt1);
			memcpy(pbuf,example_pt1,testlen);
			if (keymode == 0)
				hw_enc_compare_buff = (u8*)example_ct1;
			else
				hw_enc_compare_buff = NULL;
			hw_mac_compare_buff = NULL;
			break;
		case 2:
			keylen = sizeof(example_key2)*8;
			memcpy(key,example_key2,sizeof(example_key2));
			noncelen = sizeof(example_nonce2);
			memcpy(nonce,example_nonce2,noncelen);
			headerlen = sizeof(example_aad2);
			memcpy(header,example_aad2,headerlen);
			maclen = 8;
			testlen = sizeof(example_pt2);
			memcpy(pbuf,example_pt2,testlen);
			if (keymode == 0) {
				hw_enc_compare_buff = (u8*)example_ct2;
				hw_mac_compare_buff = (u8*)example_mac2;
			} else {
				hw_enc_compare_buff = NULL;
				hw_mac_compare_buff = NULL;
			}
			break;
		case 3:
			keylen = sizeof(example_key3)*8;
			memcpy(key,example_key3,sizeof(example_key3));
			noncelen = sizeof(example_nonce3);
			memcpy(nonce,example_nonce3,noncelen);
			headerlen = sizeof(example_aad3);
			memcpy(header,example_aad3,headerlen);
			maclen = 8;
			testlen = 0;
			hw_enc_compare_buff = NULL;
			if (keymode == 0)
				hw_mac_compare_buff = (u8*)example_mac3;
			else
				hw_mac_compare_buff = NULL;
			break;
		case 4:
			keylen = sizeof(example_key4)*8;
			memcpy(key,example_key4,sizeof(example_key4));
			noncelen = sizeof(example_nonce4);
			memcpy(nonce,example_nonce4,noncelen);
			headerlen = sizeof(example_aad4);
			memcpy(header,example_aad4,headerlen);
			maclen = 0;
			testlen = sizeof(example_pt4);
			memcpy(pbuf,example_pt4,testlen);
			if (keymode == 0)
				hw_enc_compare_buff = (u8*)example_ct4;
			else
				hw_enc_compare_buff = NULL;
			hw_mac_compare_buff = NULL;
			break;
		case 5:
			keylen = sizeof(example_key5)*8;
			memcpy(key,example_key5,sizeof(example_key5));
			noncelen = sizeof(example_nonce5);
			memcpy(nonce,example_nonce5,noncelen);
			headerlen = sizeof(example_aad5);
			memcpy(header,example_aad5,headerlen);
			maclen = 8;
			testlen = sizeof(example_pt5);
			memcpy(pbuf,example_pt5,testlen);
			if (keymode == 0) {
				hw_enc_compare_buff = (u8*)example_ct5;
				hw_mac_compare_buff = (u8*)example_mac5;
			} else {
				hw_enc_compare_buff = NULL;
				hw_mac_compare_buff = NULL;
			}
			break;
		case 6:
			keylen = sizeof(example_key6)*8;
			memcpy(key,example_key6,sizeof(example_key6));
			noncelen = sizeof(example_nonce6);
			memcpy(nonce,example_nonce6,noncelen);
			headerlen = sizeof(example_aad6);
			memcpy(header,example_aad6,headerlen);
			maclen = 8;
			testlen = sizeof(example_pt6);
			memcpy(pbuf,example_pt6,testlen);
			if (keymode == 1) {
				hw_enc_compare_buff = (u8*)example_ct6;
				hw_mac_compare_buff = (u8*)example_mac6;
			} else {
				hw_enc_compare_buff = NULL;
				hw_mac_compare_buff = NULL;
			}
			break;
		case 7:
			keylen = sizeof(example_key7)*8;
			memcpy(key,example_key7,sizeof(example_key7));
			noncelen = sizeof(example_nonce7);
			memcpy(nonce,example_nonce7,noncelen);
			headerlen = sizeof(example_aad7);
			memcpy(header,example_aad7,headerlen);
			maclen = 8;
			testlen = sizeof(example_pt7);
			memcpy(pbuf,example_pt7,testlen);
			if (keymode == 2) {
				hw_enc_compare_buff = (u8*)example_ct7;
				hw_mac_compare_buff = (u8*)example_mac7;
			} else {
				hw_enc_compare_buff = NULL;
				hw_mac_compare_buff = NULL;
			}
			break;

		case 0:
		default:
			keylen = sizeof(example_key0)*8;
			memcpy(key,example_key0,sizeof(example_key0));
			noncelen = sizeof(example_nonce0);
			memcpy(nonce,example_nonce0,noncelen);
			headerlen = sizeof(example_aad0);
			memcpy(header,example_aad0,headerlen);
			maclen = 8;
			testlen = 0;
			hw_enc_compare_buff = NULL;
			if (keymode == 0)
				hw_mac_compare_buff = (u8*)example_mac0;
			else
				hw_mac_compare_buff = NULL;
			break;
	}

	info("using example %d data\n",example_no);
	info("aes %s test,keylen:%dbit keymode:%s noncelen:%d headerlen:%u maclen:%d testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		noncelen,headerlen,maclen,testlen);
	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ccm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,taglen);
		if (ret) {
			info("hw ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		aes_print_buffer("B",(unsigned char *)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));
		ret = aes_ccm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("hw ccm enc fail\n");
			fail++;
			goto end1;
		}

		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,taglen);
		if (ret) {
			info("hw ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		ret = aes_ccm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
		if (ret) {
			info("hw ccm dec fail\n");
			fail++;
			goto end1;
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
		/*compare encrypt result*/
		if (hw_enc_compare_buff) {
			for (i=0;i<testlen;i++) {
				if (hw_enc_compare_buff[i] != hw_enc_buff[i]) {
					info("hw aes encrypt result fail @%d\n",i);
					fail++;
					break;
				}
			}
		}
		/*compare mac*/
		if (hw_mac_compare_buff) {
			for (i=0;i<maclen;i++) {
				if (hw_mac_compare_buff[i] != tag1[i]) {
					info("hw aes mac fail @%d\n",i);
					fail++;
					break;
				}
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ccm* encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}
		memset(tag_result,0,sizeof(tag_result));

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}

done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("nonce",nonce,noncelen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,maclen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,maclen);
	}

end1:
	aes_ccm_done();
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief Multi-segments AES ECB Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [segments_len] input, The length of one segment plain text, should be align to 16 
 * @param [segments] input, The number of the segments
 *
 * @return directly if failed
 */
static int aes_ecb_multi(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	int keylen = 256;
	int mode = 0;
	int keymode = 0;
	u32 segments = 1;
	u32 segments_len = 2048;
	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	symmetric_ECB ecbAES = { 0 };
	int ret = 0;

	info("test aes ecb mode using muti segments: %s [mode] [keylen] [keymode] [segments_len] [segments]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:128: 128bit| 256: 256bit\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		segments_len = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		segments = get_arg_ulong(argv[5]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if (segments_len & 0xf) {
		info("segments_len should be mutiple of 16, force align to 16\n");
		segments_len = ALIGN_UP(segments_len, 16);
	}

	testlen = segments_len*segments;
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}
	info("aes %s test,keylen:%dbit keymode:%s segments_len:%u segments:%u testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		segments_len,segments,testlen);

	/*generate random key*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ecb encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		ret = aes_ecb_start(1<<keymode,key,keylen/8);
		if (ret) {
			info("hw ecb start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		for (i=0;i<segments;i++) {
			ret = aes_ecb_enc(pbuf+i*segments_len,hw_enc_buff+i*segments_len,segments_len);
			if (ret) {
				info("hw ecb enc fail\n");
				fail++;
				goto end1;
			}
		}
		/*do decrypt*/
		for (i=0;i<segments;i++) {
			ret = aes_ecb_dec(hw_enc_buff+i*segments_len,hw_dec_buff+i*segments_len,segments_len);
			if (ret) {
				info("hw ecb dec fail\n");
				fail++;
				goto end1;
			}
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ecb encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}
		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		error = ecb_start(index,key,keylen/8,0,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = ecb_encrypt(pbuf,sw_enc_buff,testlen,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_encrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}
		ecb_done(&ecbAES);

		error = ecb_start(index,key,keylen/8,0,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = ecb_decrypt(sw_enc_buff,sw_dec_buff,testlen,&ecbAES);
		if(error != CRYPT_OK){
			info("sw ecb_decrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		ecb_done(&ecbAES);

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);

end1:
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief Multi-segments AES CBC Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [segments_len] input, The length of one segment plain text, should be align to 16 
 * @param [segments] input, The number of the segments
 *
 * @return directly if failed
 */
static int aes_cbc_multi(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char iv[16] = { 0 };
	unsigned char sw_enc_out_iv[16] = {0};
	unsigned char sw_dec_out_iv[16] = {0};
	unsigned char hw_enc_out_iv[16] = {0};
	unsigned char hw_dec_out_iv[16] = {0};
	int keylen = 256;
	int mode = 0;
	int keymode = 0;
	u32 segments = 1;
	u32 segments_len = 2048;
	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	symmetric_CBC cbcAES = { 0 };
	int ret = 0;

	info("test aes cbc mode using muti segments: %s [mode] [keylen] [keymode] [segments_len] [segments]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:128: 128bit| 256: 256bit\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		segments_len = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		segments = get_arg_ulong(argv[5]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if (segments_len & 0xf) {
		info("segments_len should be mutiple of 16, force align to 16\n");
		segments_len = ALIGN_UP(segments_len, 16);
	}

	testlen = segments_len*segments;
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for pbuf fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for enc/dec fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}
	info("aes %s test,keylen:%dbit keymode:%s segments_len:%u segments:%u testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		segments_len,segments,testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;

	for (i=0;i<16;i++)
		iv[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes cbc encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		ret = aes_cbc_start(1<<keymode,key,iv,keylen/8);
		if (ret) {
			info("hw cbc start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		for (i=0;i<segments;i++) {
			aes_cbc_enc(pbuf+i*segments_len,hw_enc_buff+i*segments_len,segments_len,hw_enc_out_iv);
			if (ret) {
				info("hw ecb enc fail\n");
				fail++;
				goto end1;
			}
		}
		/*do decrypt*/
		ret = aes_cbc_start(1<<keymode,key,iv,keylen/8);
		if (ret) {
			info("hw cbc start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		for (i=0;i<segments;i++) {
			aes_cbc_dec(hw_enc_buff+i*segments_len,hw_dec_buff+i*segments_len,segments_len,hw_dec_out_iv);
			if (ret) {
				info("hw ecb enc fail\n");
				fail++;
				goto end1;
			}
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes cbc encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		error = cbc_start(index,iv,key,keylen/8,0,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = cbc_encrypt(pbuf,sw_enc_buff,testlen,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_encrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}
		cbc_done(&cbcAES);
		memcpy(sw_enc_out_iv,cbcAES.IV,sizeof(iv));

		error = cbc_start(index,iv,key,keylen/8,0,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_start error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		error = cbc_decrypt(sw_enc_buff,sw_dec_buff,testlen,&cbcAES);
		if(error != CRYPT_OK){
			info("sw cbc_decrypt error:%s\n",error_to_string(error));
			fail++;
			goto end1;
		}

		cbc_done(&cbcAES);
		memcpy(sw_dec_out_iv,cbcAES.IV,sizeof(iv));

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("iv",iv,keylen/8);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);

end1:
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief Multi-segments AES CCM Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [noncelen] input, The length of the nonce, Nlen: 7-13
 * @param [headerlen] input, The length of the associated data A, Alen: 0-2^32-1
 * @param [maclen] input, The length of the authentication tag, Tlen:[4,6,8,10,12,14,16]
 * @param [segments_len] input, The length of one segment plain text, should be align to 16 and 0-2^28-1
 * @param [segments] input, The number of the segments
 *
 * @return directly if failed
 */
static int aes_ccm_multi(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char nonce[16] = { 0 };
	unsigned char *header = NULL;
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	int noncelen = 13;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 segments = 1;
	u32 segments_len = 2048;
	u64 testlen = 2048;
	u64 maxlen = 0;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	int flag = 0;
	int ret = 0;

	info("test aes ccm mode using multi-segments: %s [mode] [keylen] [keymode] [noncelen] [headerlen] [maclen] [segments_len] [segments]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw only|2 hw only \n");
	info("keylen:128: 128bit| 256: 256bit\n");
	info("keymode: 0 sw key | 1 otp key1 | 2 otp key2\n");
	info("noncelen: 7-13: notice this parameter is relative with testlen,testlen max len = 2^((15-noncelen)*8)-1\n");
	info("headerlen: 0 - 2^32-1 (actual limit by dynamic memory allocated)\n");
	info("maclen: 4,6,8,10,12,14,16\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		noncelen = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		headerlen = get_arg_ulong(argv[5]);
	}
	if (argc >6) {
		maclen = get_arg_ulong(argv[6]);
	}
	if (argc >7) {
		segments_len = get_arg_ulong(argv[7]);
	}
	if (argc >8) {
		segments = get_arg_ulong(argv[8]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if ((noncelen < 7) || (noncelen > 13)) {
		info("noncelen should be 7-13\n");
		return 0;
	}
	/* make sure the maclen is even and >=4 && <= 16 */
	if((maclen & 1) || maclen > 16 || maclen < 4) {
		info("maclen should be 4,6,8,10,12,14,16\n");
		return 0;
	}
	if (segments_len > ((1UL << 28) - 1)) {
		info("segments_len:%u too large, max:%u\n", segments_len, ((1UL << 28) - 1));
		return 0;
	}
	
	if (segments_len & 0xf) {
		info("segments_len should be mutiple of 16, force align to 16\n");
		segments_len = ALIGN_UP(segments_len, 16);
	}
	testlen = segments_len*segments;
	maxlen = (1ULL << ((15 - noncelen) * 8)) - 1;
	if (testlen > maxlen) {
		info("segments_len*segments:%llu is too large, max:%llu\n", testlen, maxlen);
		return 0;
	}
	if (headerlen) {
		header = (unsigned char*)malloc(headerlen);
		if (!header) {
			info("allocate memory for aad info fail\n");
			fail++;
			goto end1;
		}
	}
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}

	info("aes %s test,keylen:%dbit keymode:%s noncelen:%d headerlen:%u maclen:%d segments_len:%u segments:%u testlen:%llu\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		noncelen,headerlen,maclen,segments_len,segments,testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;
	for (i=0;i<16;i++)
		nonce[i] = cb_rand() & 0xff;
	for (i=0;i<headerlen;i++)
		header[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes ccm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,maclen);
		if (ret) {
			info("hw ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		if (segments == 0) {
			ret = aes_ccm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
			if (ret) {
				info("hw ccm enc fail\n");
				fail++;
				goto end1;
			}
		} else {
			for (i=0;i<segments;i++) {
				flag = 0;
				if (i==0)
					flag |= AES_FLAG_START;
				if (i== segments -1)
					flag |= AES_FLAG_END;
				ret = aes_ccm_enc(pbuf+i*segments_len, hw_enc_buff+i*segments_len,segments_len, tag_result,&taglen,flag);
				if (ret) {
					info("hw ccm enc fail\n");
					fail++;
					goto end1;
				}
			}
		}
		aes_print_buffer("B",(unsigned char *)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));
		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_ccm_start(1<<keymode,key,keylen/8,nonce,noncelen,header,headerlen,testlen,maclen);
		if (ret) {
			info("hw ccm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		if (segments == 0) {
			ret = aes_ccm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
			if (ret) {
				info("hw ccm dec fail\n");
				fail++;
				goto end1;
			}
		} else {
			for (i=0;i<segments;i++) {
				flag = 0;
				if (i==0)
					flag |= AES_FLAG_START;
				if (i== segments -1)
					flag |= AES_FLAG_END;
				ret = aes_ccm_dec(hw_enc_buff+i*segments_len, hw_dec_buff+i*segments_len,segments_len, tag_result,&taglen,flag);
				if (ret) {
					info("hw ccm dec fail\n");
					fail++;
					goto end1;
				}
			}
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes ccm encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("sw ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = ccm_memory(index,
			key, keylen/8,
			NULL,
			nonce, noncelen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, CCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("sw ccm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("nonce",nonce,noncelen);
	aes_print_buffer("aad",header,((headerlen > 256) ? 256 : headerlen));
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	aes_ccm_done();
	if (header)
		free(header);
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief Multi-segments AES GCM Encrypt/Decrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [ivlen] input, The length of the initial value data. should be 12
 * @param [headerlen] input, The length of the associated data A, Alen: 0-2^32-1
 * @param [maclen] input, The length of the authentication tag, Tlen:12-16
 * @param [segments_len] input, The length of one segment plain text, should be align to 16
 * @param [segments] input, The number of the segments
 *
 * @return directly if failed
 */
static int aes_gcm_multi(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char iv[16] = { 0 };
	unsigned char *header = NULL;
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	int ivlen = 12;
	u32 headerlen = 16;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 segments = 1;
	u32 segments_len = 2048;
	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	int flag = 0;
	int ret = 0;

	info("test aes gcm mode using multi-segments: %s [mode] [keylen] [keymode] [ivlen] [headerlen] [maclen] [segments_len] [segments]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw only|2 hw only \n");
	info("keylen:128: 128bit| 256: 256bit\n");
	info("keymode: 0 sw key | 1 otp key1 | 2 otp key2\n");
	info("ivlen: 12\n");
	info("headerlen: 0 - 2^32-1 (actual limit by dynamic memory allocated)\n");
	info("maclen: 12,13,14,15,16\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		ivlen = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		headerlen = get_arg_ulong(argv[5]);
	}
	if (argc >6) {
		maclen = get_arg_ulong(argv[6]);
	}
	if (argc >7) {
		segments_len= get_arg_ulong(argv[7]);
	}
	if (argc >8) {
		segments= get_arg_ulong(argv[8]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if (ivlen != 12) {
		info("ivlen should be 12\n");
		return 0;
	}
	if ((maclen < 12) || (maclen > 16)) {
		info("maclen should be 12-16\n");
		return 0;
	}

	if (segments_len & 0xf) {
		info("segments_len should be mutiple of 16, force align to 16\n");
		segments_len = ALIGN_UP(segments_len, 16);
	}
	testlen = segments_len*segments;

	if (headerlen) {
		header = (unsigned char*)malloc(headerlen);
		if (!header) {
			info("allocate memory for aad info fail\n");
			fail++;
			goto end1;
		}
	}
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	if ((mode ==0)||(mode==2)) {
		hw_enc_buff = (u8*)malloc(testlen);
		hw_dec_buff = (u8*)malloc(testlen);
		if (!hw_enc_buff || ! hw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(hw_enc_buff,0,testlen);
		memset(hw_dec_buff,0,testlen);
	}
	if ((mode==0)||(mode==1)) {
		sw_enc_buff = (u8*)malloc(testlen);
		sw_dec_buff = (u8*)malloc(testlen);
		if (!sw_enc_buff || ! sw_dec_buff) {
			info("allocate memory for test buffer fail\n");
			fail++;
			goto end1;
		}
		memset(sw_enc_buff,0,testlen);
		memset(sw_dec_buff,0,testlen);
	}
	info("aes %s test,keylen:%dbit keymode:%s ivlen:%d headerlen:%u maclen:%d  segments_len:%u segments:%u testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		ivlen,headerlen,maclen,segments_len,segments,testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;
	for (i=0;i<16;i++)
		iv[i] = cb_rand() & 0xff;
	for (i=0;i<headerlen;i++)
		header[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes gcm encrypt/decrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_gcm_start(1<<keymode,key,keylen/8,iv,ivlen,header,headerlen,testlen,maclen);
		if (ret) {
			info("gcm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		if (segments == 0) {
			ret = aes_gcm_enc(pbuf, hw_enc_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
			if (ret) {
				info("gcm enc fail\n");
				fail++;
				goto end1;
			}
		} else {
			for (i=0;i<segments;i++) {
				flag = 0;
				if (i==0)
					flag |= AES_FLAG_START;
				if (i== segments -1)
					flag |= AES_FLAG_END;
				ret = aes_gcm_enc(pbuf+i*segments_len, hw_enc_buff+i*segments_len,segments_len, tag_result,&taglen,flag);
				if (ret) {
					info("gcm enc fail\n");
					fail++;
					goto end1;
				}
			}
		}
		aes_print_buffer("B",(unsigned char*)p_aes_dev->b_buf,((p_aes_dev->blen > 256) ? 256 : p_aes_dev->blen));

		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);

		/*do decrypt*/
		ret = aes_gcm_start(1<<keymode,key,keylen/8,iv,ivlen,header,headerlen,testlen,maclen);
		if (ret) {
			info("gcm start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		if (segments == 0) {
			ret = aes_gcm_dec(hw_enc_buff, hw_dec_buff,testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
			if (ret) {
				info("gcm dec fail\n");
				fail++;
				goto end1;
			}
		} else {
			for (i=0;i<segments;i++) {
				flag = 0;
				if (i==0)
					flag |= AES_FLAG_START;
				if (i== segments -1)
					flag |= AES_FLAG_END;
				ret = aes_gcm_dec(hw_enc_buff+i*segments_len, hw_dec_buff+i*segments_len,segments_len, tag_result,&taglen,flag);
				if (ret) {
					info("gcm dec fail\n");
					fail++;
					goto end1;
				}
			}
		}

		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != hw_dec_buff[i]) {
				info("hw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes gcm encrypt/decrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}

		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = gcm_memory(index,
			key, keylen/8,
			iv, ivlen,
			header, headerlen,
			pbuf, testlen,
			sw_enc_buff,
			tag_result, &taglen1, GCM_ENCRYPT);
		if (error != CRYPT_OK) {
			info("gcm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}

		memcpy(tag2,tag_result,taglen1);
		error = gcm_memory(index,
			key, keylen/8,
			iv, ivlen,
			header, headerlen,
			sw_dec_buff, testlen,
			sw_enc_buff,
			tag_result, &taglen1, GCM_DECRYPT);
		if (error != CRYPT_OK) {
			info("gcm_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		/*compare encrypt/decrypt result with origin plaintext*/
		for (i=0;i<testlen;i++) {
			if (pbuf[i] != sw_dec_buff[i]) {
				info("sw aes encrypt/decrypt fail @%d\n",i);
				fail++;
				break;
			}
		}
	}

	if (mode==0) {
		/*compare hw/sw enc result*/
		for (i=0;i<testlen;i++) {
			if (hw_enc_buff[i] != sw_enc_buff[i]) {
				info("hw/sw aes encrypt result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("iv",iv,ivlen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2))
		aes_print_buffer("hw decrypt",hw_dec_buff,testlen);
	if ((mode==0)||(mode==1))
		aes_print_buffer("sw decrypt",sw_dec_buff,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw encrypt",hw_enc_buff,testlen);
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw encrypt",sw_enc_buff,testlen);
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	aes_gcm_done();
	if (header)
		free(header);
	if (pbuf)
		free(pbuf);
	if (hw_enc_buff)
		free(hw_enc_buff);
	if (hw_dec_buff)
		free(hw_dec_buff);
	if (sw_enc_buff)
		free(sw_enc_buff);
	if (sw_dec_buff)
		free(sw_dec_buff);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief Multi-segments AES CMAC Encrypt
 * @param [mode] input, the encrypt/decrypt mode, should be 0(sw&hw) 1(sw only) or 2(hw only)
 * @param [keylen] input, The key length in bits, should be 128 or 256
 * @param [keymode] input, the key access mode, should be 0(sw key) 1(hw OTP key1) or 2(hw OTP key2)
 * @param [maclen] input, The length of the authentication tag, Tlen:0-16
 * @param [segments_len] input, The length of one segment plain text, should be align to 16
 * @param [segments] input, The number of the segments
 *
 * @return directly if failed
 */
static int aes_cmac_multi(int argc, char* argv[])
{
	unsigned char key[32] = { 0 };
	unsigned char tag_result[16] = { 0 };
	unsigned char tag1[16] = { 0 };
	unsigned char tag2[16] = { 0 };
	int keylen = 256;
	unsigned long taglen = 0;
	unsigned long taglen1 = 0;
	int maclen = 16;
	int keymode = 0;
	int mode = 0;
	u32 segments = 1;
	u32 segments_len = 2048;
	u32 testlen = 2048;
	int fail = 0;
	int i = 0;
	u8 *pbuf = NULL;
	u8 *hw_enc_buff = NULL;
	u8 *hw_dec_buff = NULL;
	u8 *sw_enc_buff = NULL;
	u8 *sw_dec_buff = NULL;

	int error = 0;
	int index = 0;
	int flag = 0;
	int ret = 0;

	info("test aes cmac mode using muti segments: %s [mode] [keylen] [keymode] [maclen] [segments_len] [segments]\n",argv[0]);
	info("mode: 0 sw&hw |1 sw|2 hw ;keylen:128: 128bit| 256: 256bit\n");

	if (argc > 1) {
		mode = get_arg_ulong(argv[1]);
	}
	if (argc >2) {
		keylen = get_arg_ulong(argv[2]);
	}
	if (argc >3) {
		keymode = get_arg_ulong(argv[3]);
	}
	if (argc >4) {
		maclen = get_arg_ulong(argv[4]);
	}
	if (argc >5) {
		segments_len= get_arg_ulong(argv[5]);
	}
	if (argc >6) {
		segments= get_arg_ulong(argv[6]);
	}
	if ((mode < 0) || (mode > 2)) {
		info("mode should be 0|1|2\n");
		return 0;
	}
	if ((keylen != 128) && (keylen != 256)) {
		info("aes support key len 128/256 bit only\n");
		return 0;
	}
	if ((keymode < 0) || (keymode > 2)) {
		info("keymode should be 0|1|2\n");
		return 0;
	}
	if(maclen > 16 || maclen < 0) {
		info("maclen should be 0-16\n");
		return 0;
	}

	if (segments_len & 0xf) {
		info("segments_len should be mutiple of 16, force align to 16\n");
		segments_len = ALIGN_UP(segments_len, 16);
	}
	testlen = segments_len*segments;
	pbuf = (u8*)malloc(testlen);
	if (!pbuf) {
		info("allocate memory for test buffer fail\n");
		fail++;
		goto end1;
	}
	info("aes %s test,keylen:%dbit keymode:%s maclen:%d segments_len:%u segments:%u testlen:%u\n",
		(mode==1)?"sw":((mode==2)?"hw":"sw&hw"),
		keylen,
		(keymode==0)?"sw key":((keymode==1)?"otp key1":"otp key2"),
		maclen,	segments_len, segments, testlen);

	/*generate random key and iv*/
	srand(get_ticks());
	for (i=0;i<32;i++)
		key[i] = cb_rand() & 0xff;

	/*generate test plaintext*/
	for (i=0;i<testlen;i++) {
		pbuf[i] = cb_rand() & 0xff;
	}

	/*test encrypt/decrypt using hw aes*/
	if ((mode ==0)||(mode==2)) {
		info("hw aes cmac encrypt\n");

		aes_test_init();

		/*do encrypt*/
		taglen = maclen;
		ret = aes_cmac_start(1<<keymode,key,keylen/8,maclen,testlen);
		if (ret) {
			info("hw aes_cmac_start fail,please check parameter valid\n");
			fail++;
			goto end1;
		}
		if (segments == 0) {
			ret = aes_cmac_enc(pbuf, testlen, tag_result,&taglen,AES_FLAG_START|AES_FLAG_END);
			if (ret) {
				info("segments=0: hw cmac enc fail\n");
				fail++;
				goto end1;
			}
		} else {
			for (i=0;i<segments;i++) {
				flag = 0;
				if (i==0)
					flag |= AES_FLAG_START;
				if (i == segments - 1)
					flag |= AES_FLAG_END;
				ret = aes_cmac_enc(pbuf+i*segments_len, segments_len, tag_result,&taglen,flag);
				if (ret) {
					info("hw cmac enc fail\n");
					fail++;
					goto end1;
				}
			}
		}
		/*save for futher compare*/
		memcpy(tag1,tag_result,taglen);
	}

	/*test encrypt/decrypt using sw aes*/
	if ((mode==0)||(mode==1)) {
		info("sw aes cmac encrypt\n");

		/*setup key*/
		/*key: keylen: num_rounds(0:use recommend value);state*/
		if(-1 == register_cipher(&aes_desc)){
			info("register_cipher(&aes_desc) failed\n");
			fail++;
			goto end1;
		}

		index = find_cipher("aes");
		if(index == -1){
			info("sw ciper aes not found\n");
			fail++;
			goto end1;
		}
		if (keymode==1) {
			memcpy(key,otp_key1,sizeof(otp_key1));
		}else if (keymode ==2)
			memcpy(key,otp_key2,sizeof(otp_key2));
		taglen1 = maclen;
		error = omac_memory(index,
			key, keylen/8,
			pbuf, testlen,
			tag_result, &taglen1);
		if (error != CRYPT_OK) {
			info("sw omac_memory error %s\n", error_to_string(error));
			fail++;
			goto end1;
		}
		memcpy(tag2,tag_result,taglen1);
	}

	if (mode==0) {
		/*compare tag*/
		if ((taglen != taglen1) || (taglen!=maclen))
			info("sw/hw taglen not match: %d %d\n",taglen,taglen1);
		for (i=0;i<maclen;i++) {
			if (tag1[i] != tag2[i]) {
				info("hw/sw aes encrypt tag result diff @%d\n",i);
				fail++;
				goto done;
			}
		}
	}
done:
	/*printout key and first block*/
	testlen = (testlen>256)?256:testlen;
	aes_print_buffer("key",key,keylen/8);
	aes_print_buffer("plain",pbuf,testlen);
	if ((mode==0)||(mode==2)) {
		aes_print_buffer("hw tag",tag1,taglen);
	}
	if ((mode==0)||(mode==1)) {
		aes_print_buffer("sw tag",tag2,taglen1);
	}

end1:
	if (pbuf)
		free(pbuf);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry aes_test_menu[] = {
	{ "help", cmd_help, 	 " : Display list of commands" },
	{ "h",cmd_help, 	"    : alias for help" },
	{ "?",cmd_help, 	"    : alias for help" },
	{ "KEK_KEY_Generate", KEK_KEY_Generate, "   :KEK_KEY_Generate test" },
	{ "aes_ecb",aes_ecb, 	 "   :aes ecb test" },
	{ "aes_cbc",aes_cbc, 	 "  :aes cbc test" },
	{ "aes_ccm",aes_ccm, 	 "  :aes ccm test" },
	{ "aes_ccm_fixdata",aes_ccm_fixdata, "  :aes ccm test using fix example data" },
	{ "aes_gcm",aes_gcm, 	 "  :aes gcm test" },
	{ "aes_gcm_fixdata",aes_gcm_fixdata, "  :aes gcm test using fix example data" },
	{ "aes_cmac",aes_cmac, 	 "  :aes cmac test" },
	{ "aes_ccm_star",aes_ccm_star, 	 "  :aes ccm* test" },
	{ "aes_ecb_multi",aes_ecb_multi, "  :aes ecb multi-segements test" },
	{ "aes_cbc_multi",aes_cbc_multi, "  :aes cbc multi-segements test" },
	{ "aes_ccm_multi",aes_ccm_multi, "  :aes ccm multi-segements test" },
	{ "aes_gcm_multi",aes_gcm_multi, "  :aes gcm multi-segements test" },
	{ "aes_cmac_multi",aes_cmac_multi, "  :aes cmac multi-segements test" },
	{ 0, 0, 0 }
};

#define AES_CTX_CFG_DEF_VAL	0x1
bool aes_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
	
	// write register
	info("write ctx cfg reg\n");
	__raw_writel(*p_wr_val, (AES_BASE + 0x4));

	// read&compare
	info("read&compare ctx cfg reg\n");
	*p_rd_val = __raw_readl(AES_BASE + 0x4);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("aes reset\n");
	prcm_reset(ARM_AES_RST);
	
	// read register
	info("read&compare ctx cfg reg\n");
	*p_rd_val = __raw_readl(AES_BASE + 0x4);
	if (*p_rd_val != AES_CTX_CFG_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool aes_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_AES;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// write register
	info("write ctx cfg reg\n");
	__raw_writel(*p_wr_val, (AES_BASE + 0x4));

	// read&compare
	info("read&compare ctx cfg reg\n");
	*p_rd_val = __raw_readl(AES_BASE + 0x4);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write ctx cfg reg\n");
	__raw_writel((*p_wr_val+1), (AES_BASE + 0x4));
	
	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare ctx cfg reg\n");
	*p_rd_val = __raw_readl(AES_BASE + 0x4);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool aes_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool aes_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

