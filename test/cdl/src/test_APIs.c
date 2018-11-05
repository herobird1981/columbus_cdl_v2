/*
 * test_APIs.c
 *
 *  Created on: Oct 10, 2016
 *      Author: edberth
 */


#include "test_config.h"
#include "cdl_types.h"
#include "cdl_uart.h"
#include "cdl_pll.h"
#include "memleak.h"


#if (CONFIG_API_TEST == 1)
typedef int		BOOL;

extern struct dw_uart* p_uart_dev;

extern unsigned char cdl_tiny_fw_bin[];
extern unsigned char cdl_tiny_sec_fw_bin[];
extern unsigned int cdl_tiny_fw_bin_len;

#define BRITE_ROM_ADDR		0x0
#define BRITE_ROM_SIZE		0x20000
#define BRITE_ROM_INFO_OFFSET	0x40

#define BRITE_ROM_INFO_MAGIC  0x0B19B007

#ifndef TAPEOUT_V2
#define TRANSFER_INVALID 0
#define TRANSFER_VALID   1

#define IMAGE_INVALID    0
#define IMAGE_VALID      1
#else
#define TRANSFER_INVALID (0xaaaa5555)
#define TRANSFER_VALID   (0x5aa555aa)

#define IMAGE_INVALID    (0x5555aaaa)
#define IMAGE_VALID      (0xaa5555aa)

#define DUMMY_SECURE_MODE 	(0xaa55a5a5) //for all of the non 0x5aa555aa is secure mode
#define NONSECURE_MODE  (0x55aa5a5a)
#endif

//image marker
#define IMG_FW_MARKER		0x5AA5

#define CBC_IV_LENGTH    16

#define FW_HEADER_OFFSET 0
#define FW_HEADER_LENGTH 256
#define FW_IMAGE_OFFSET  FW_HEADER_LENGTH

/******************************************************************************/
// structure definition
/******************************************************************************/
struct brite_rom_info {
	u32	magic;
	u32	rom_size;
	u32	text_start;
	u32	text_end;

	u32	data_start;
	u32	data_end;
	u32	bss_start;
	u32	bss_end;

	u32	rel_dyn_start;
	u32	rel_dyn_end;
	u32	api_load_start;
	u32	api_start;

	u32	api_end;
	void	(*api_list_init)(void);
};

// define the image header struct
typedef struct
{
	uint8_t  CBC_IV[16];            //AES IV 
	uint16_t fw_marker;				// firmware marker(fixed to be 0x5AA5)
									// = IMG_FW_MARKER

	uint8_t  header_ver;				// firmware version
	uint8_t  rsv1;

	uint32_t ca_len;				// certificate length

	uint8_t	 ca_ver;				// certificate version
	uint8_t  ca_pubkey[97];	// certificate ECC P-384 public key
	uint8_t  ca_id[8];				// certificate identifier

	uint16_t image_crc;				// the crc of image data

	uint32_t image_len;				// image length(= instruction len + data len)

	uint8_t  image_sig[96];		// ECC P-384 signature

	uint32_t ins_addr;				// instruction address on ram(for DSP, on the ITCM)
	uint32_t ins_len;				// instruction length on ram(for DSP, on the ITCM)
	uint32_t data_addr;				// data address on ram(for DSP, on the DTCM)
	uint32_t data_len;				// data address on ram(for DSP, on the DTCM)

	uint8_t  rsv2[6];
	uint16_t header_crc16;				// the crc of image header
} img_header_t;

typedef enum
{
	EMPYT_KEY       = 0,
	SW_KEY			= 1,
	OTP_KEY1    	= 2,
	OTP_KEY2    	= 4
} KEY_MODE;

typedef struct _SECURITY_IP_BASEADDR{
	volatile void *SHA_baseaddr;
	volatile void *CRC_baseaddr;
	volatile void *AES_baseaddr;
	volatile void *ECC_baseaddr;

	volatile void *OTP_baseaddr;
	volatile void *RNG_baseaddr;
	volatile void *DMA_baseaddr;
	volatile void *TIMER_baseaddr;
    volatile void *PRCM_baseaddr;

	volatile void *UART_baseaddr;   /* uart base addr can be removed only if no debug info in security api */
	volatile void *MISC_baseaddr;
}SECURITY_IP_BASEADDR, *SECURITY_IP_BASEADDR_PTR;

typedef struct _GLOBAL_API_FUNC{
	/*API ptr list*/
	void (*ptr_SetRegBaseAddr)(SECURITY_IP_BASEADDR_PTR security_ip_baseaddr_ptr);
	
	uint32_t (*ptr_AES_CBC_Decrypt)(KEY_MODE key_mode, unsigned char key[32],
        unsigned char in_IV[16], void *in_buf, uint32_t buf_len, 
        void *out_buf, unsigned char out_IV[16]); 

	uint32_t (*ptr_AES_CBC_Encrypt)(KEY_MODE key_mode, unsigned char key[32], 
        unsigned char in_IV[32], void *in_buf, uint32_t buf_len,
        void *out_buf, unsigned char out_IV[32]); 

	uint32_t (*ptr_AES_GCM_Decrypt)(KEY_MODE key_mode, unsigned char key[32], 
	        unsigned char in_IV[12], void *in_buf, uint32_t buf_len,
	        uint8_t *aad, uint32_t aad_len, unsigned char *atag, unsigned long *atag_len,
	        void *out_buf); 

	uint32_t (*ptr_AES_GCM_Encrypt)(KEY_MODE key_mode, unsigned char key[32],
	        unsigned char in_IV[12], void *in_buf, uint32_t buf_len,
	        uint8_t *aad, uint32_t aad_len, unsigned char *atag, unsigned long *atag_len, 
	        void *out_buf); 

	uint32_t (*ptr_AES_CCM_Decrypt)(KEY_MODE key_mode, unsigned char key[32], 
	        unsigned char nonce[16], uint32_t nonce_len, void *in_buf, uint32_t buf_len,
	        unsigned char *aad, uint32_t aad_len, unsigned char *atag, unsigned long *atag_len,
	        void *out_buf); 

	uint32_t (*ptr_AES_CCM_Encrypt)(KEY_MODE key_mode, unsigned char key[32], 
	        unsigned char nonce[16], uint32_t nonce_len, void *in_buf, uint32_t buf_len,
	        unsigned char *aad, uint32_t aad_len, unsigned long *atag_len, 
	        void *out_buf, unsigned char *atag); 

	uint32_t (*ptr_KEK_KEY_Generate)(uint8_t key_slot, uint8_t *kek_key);
	
	int (*ptr_otp_read_secu_key)(uint8_t key_slot, uint8_t *out_key);

	int (*ptr_otp_write_sec_byte)(uint32_t row, uint8_t write_val);
		
	int (*ptr_otp_secu_key_disable)(uint8_t key_slot);

	int (*ptr_otp_read_non_secu_byte)(uint32_t row);

	int (*ptr_otp_write_non_sec_byte)(uint32_t row, uint8_t write_val);
#ifndef TAPEOUT_V2
	BOOL (*ptr_image_header_verify)(img_header_t *p_img_header, uint32_t p_img_header_dma,uint8_t secure_flag);

	int (*ptr_image_verify)(img_header_t *p_img_header, uint32_t p_img_header_dma, uint8_t secure_flag,
			uint8_t header_mode, uint8_t image_mode, uint8_t store_type);
#else
	uint32_t (*ptr_image_header_verify)(img_header_t *p_img_header, uint32_t p_img_header_dma, uint32_t secure_flag);

	uint32_t (*ptr_image_verify)(img_header_t *p_img_header, uint32_t p_img_header_dma, uint32_t secure_flag,
			uint32_t store_type);
#endif
	void (*ptr_AES_Set_B_Buffer)(void *va, uint32_t pa, uint32_t len);

	void (*ptr_SHA_Set_Sect_Buffer)(void *va, uint32_t pa, uint32_t len);

	unsigned long (*ptr_RNG_Read)(void *out, unsigned long size);

	BOOL (*ECC384_Sign)(uint8_t *priv_key, uint8_t *hash, uint8_t *sig, unsigned long flags);
#ifndef TAPEOUT_V2
	BOOL (*ECC384_Verification)(uint8_t *pub_key, uint8_t *hash, uint8_t *sig);
#else
	uint32_t (*ECC384_Verification)(uint8_t *pub_key, uint8_t *hash, uint8_t *sig);
#endif
	uint8_t (*get_boot_interface)(void);
}GLOBAL_API_FUNC, *PGLOBAL_API_FUNC;


PGLOBAL_API_FUNC api_func_ptr; // = (PGLOBAL_API_FUNC)0x10010000;  //DSP: 0x3ffffb80;

cmdline_entry APIs_test_menu[];

static int aes_cbc_test(int example_no);

static int aes_gcm_test(int example_no);

static int aes_ccm_test(int example_no);

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&APIs_test_menu[0]);
}

int sec_api_init(void) {
	struct brite_rom_info *info = (void *)(BRITE_ROM_ADDR + BRITE_ROM_INFO_OFFSET);
	int ret = 0;
	SECURITY_IP_BASEADDR periph_addrs = {
		(void *)SHA_BASE,
		(void *)CRC_BASE,
		(void *)AES_BASE,
		(void *)ECC_BASE,
		(void *)OTP_BASE,
#ifdef TAPEOUT_V2
		(void *)DWC_TRNG_BASE,
#else
		(void *)RNG_BASE,
#endif
		(void *)DMAC0_BASE,
		(void *)TIMER_BASE,
		(void *)PRCM_BASE,
		(void *)UART0_BASE,
		(void *)MISC_BASE
	};

	if(info->magic != BRITE_ROM_INFO_MAGIC) {
		ret = -1;
		goto err;
	}
	if(info->rom_size > BRITE_ROM_SIZE) {
		ret = -2;
		goto err;
	}

	api_func_ptr = (void *)info->api_start;

	api_func_ptr->ptr_SetRegBaseAddr(&periph_addrs);

err:
		return ret;
}

static int uart_init(unsigned long base, unsigned long pclk)
{
	memset(p_uart_dev, 0, sizeof(struct dw_uart));
	p_uart_dev->regs = (void*)base;
	p_uart_dev->dma_mode = 0;

	dw_uart_init(p_uart_dev, pclk, 115200); //115200,n,8,1

	return 0;
}

/**
 * @brief get_secure_key - get otp secure key by calling ptr_otp_read_secu_key api
 * @param key_idx input, key slot number, should be 0-13
 *
 * @return directly if failed
 */
static int get_secure_key(int argc, char* argv[])
{
	int key_index;
	int key_size, i;
	unsigned char key_buf[48] = {0};

	if (argc < 2) {
		info("wrong cmd_format: %s key_index\n", argv[0]);
		return 1;
	}

	key_index = (int)simple_strtoul(argv[1], NULL, 10);
	key_size = (api_func_ptr->ptr_otp_read_secu_key)(key_index, key_buf);

	if(key_size < 0)
		info("key access failed!\n");
	else
	{
		info("key[%d] = :\n", key_index);
		for(i=0; i<key_size; i++)
			info("0x%x	", key_buf[i]);
		info("\r\n");
	}

	return 0;
}

/**
 * @brief set_secure_byte - write one byte to the specified secure address by calling api
 * @param row input, write row, should be 0-511
 * @param write_val input, write value, should be 0-0xFF
 *
 * @return directly if failed
 */
static int set_secure_byte(int argc, char* argv[])
{
	int row = 0;
	int write_value = 0;
	int result = 0;

	if (argc < 3) {
		info("wrong cmd_format: %s row write_val\n", argv[0]);
		return 1;
	}

	row = get_arg_ulong(argv[1]);
	write_value = get_arg_ulong(argv[2]);

	result = (api_func_ptr->ptr_otp_write_sec_byte)(row, write_value);
	if(result < 0)
		info("row[%d] write 0x%x failed!\n", row, write_value);
	else
	{
		info("row[%d] write 0x%x success!\n", row, write_value);
	}

	return 0;
}

/**
 * @brief secure_key_disable - disable secure key by calling ptr_otp_secu_key_disable api
 * @param key_idx input, key slot number, should be 0-13
 *
 * @return directly if failed
 */
static int secure_key_disable(int argc, char* argv[])
{
	int key_index;
	int result = 0;

	if (argc < 2) {
		info("wrong cmd_format: %s key_index\n", argv[0]);
		return 1;
	}

	key_index = get_arg_ulong(argv[1]);
	result = (api_func_ptr->ptr_otp_secu_key_disable)(key_index);

	if(result < 0)
		info("key[%d] access failed!\n", key_index);
	else
	{
		info("key[%d] has been disabled\n", key_index);
	}

	return 0;
}

/**
 * @brief get_non_secure_byte - read one byte from the specified non-secure address by calling api
 * @param row input, read row, should be 0-511
 *
 * @return directly if failed
 */
static int get_non_secure_byte(int argc, char* argv[])
{
	int read_byte = 0;
	int row = 0;

	if (argc < 2) {
		info("wrong cmd_format: %s row\n", argv[0]);
		return 1;
	}

	row = get_arg_ulong(argv[1]);
	read_byte = (api_func_ptr->ptr_otp_read_non_secu_byte)(row);
	if (read_byte < 0)
		info("read row[%d] failed!\n", row);
	else
		info("read row[%d] byte 0x%x\n", row, read_byte);

	return 0;
}

/**
 * @brief set_non_secure_byte - write one byte to the specified non-secure address by calling api
 * @param row input, write row, should be 0-511
 * @param write_val input, write value, should be 0-0xFF
 *
 * @return directly if failed
 */
static int set_non_secure_byte(int argc, char* argv[])
{
	int row = 0;
	int write_value = 0;
	int result = 0;

	if (argc < 3) {
		info("wrong cmd_format: %s row write_val\n", argv[0]);
		return 1;
	}

	row = get_arg_ulong(argv[1]);
	write_value = get_arg_ulong(argv[2]);

	result = (api_func_ptr->ptr_otp_write_non_sec_byte)(row, write_value);
	if(result < 0)
		info("row[%d] write 0x%x failed!\n", row, write_value);
	else
	{
		info("row[%d] write 0x%x success!\n", row, write_value);
	}

	return 0;
}


#if 1
#define TEST_LEN 32

static void aes_print_buffer(char*name,unsigned char* buf,int len)
{
	int i,j;
	int line;
	int rest;

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

/**
 * @brief KEK KEY Generation api test
 * @param key_idx input, indicates which key will be chosen to generate the new key, should be 7-13
 *
 * @return directly if failed
 */
static int key_key_generate_test(int key_idx)
{
	int ptextlen = 0;
	int example_no = 0;
	int fail = 0;
	int ret = 0;
	int i;

	u8 hw_enc_buff[32];
	u8 *hw_enc_compare_buff;

	memset(hw_enc_buff,0,sizeof(hw_enc_buff));

	/* example 1: OTP key number 7
	Key = OTP key number 6
	PT = OTP key number 7
	0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
	0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 
	*/
	const unsigned char example_ct1[] = {
		0x23, 0x9b, 0x90, 0xb5, 0x48, 0xdf, 0x8c, 0x7a, 0xeb, 0x6b, 0x41, 0xce, 0x0c, 0xa4, 0xe0, 0x8f, 
		0xaf, 0x95, 0xa4, 0xcf, 0xf9, 0x89, 0x9c, 0xb1, 0xe6, 0x68, 0xe5, 0xf7, 0x58, 0x50, 0x77, 0x16,
	};
	/* example 2: OTP key number 8
	Key = OTP key number 6
	PT = OTP key number 8
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 
	*/
	const unsigned char example_ct2[] = {
		0xe3, 0xde, 0x35, 0x85, 0x52, 0x1b, 0x40, 0xbb, 0x14, 0x76, 0x92, 0x0f, 0xb7, 0x7c, 0xf6, 0x3d, 
		0x2c, 0xcb, 0x94, 0x0d, 0x05, 0x4c, 0x3a, 0xd7, 0xd6, 0x4d, 0xac, 0xdd, 0x81, 0x5d, 0x0b, 0xa2, 
	};
	/* example 3: OTP key number 9
	Key = OTP key number 6
	PT = OTP key number 9
	0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 
	0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 
	*/
	const unsigned char example_ct3[] = {
		0xab, 0x53, 0x69, 0x7c, 0xc6, 0x91, 0xb4, 0xd5, 0x5a, 0x4f, 0xbf, 0x45, 0x29, 0xf9, 0x64, 0x2b, 
		0x71, 0x7e, 0xd5, 0x45, 0x4b, 0x19, 0xdd, 0x13, 0xcd, 0x98, 0x65, 0x81, 0xe7, 0x12, 0x8e, 0xc9,
	};
	/* example 4: OTP key number 10
	Key = OTP key number 6
	PT = OTP key number 10
	0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 
	0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 
	*/
	const unsigned char example_ct4[] = {
		0x66, 0x0e, 0x20, 0xb1, 0x8b, 0x77, 0xc6, 0x7c, 0x93, 0xa4, 0x1f, 0xb7, 0xea, 0x8a, 0xd1, 0xbd, 
		0x7e, 0x68, 0x77, 0x03, 0xcf, 0xa3, 0xca, 0xd4, 0xd9, 0x97, 0x99, 0x48, 0xa5, 0x70, 0x99, 0x54, 
	};
	/* example 5: OTP key number 11
	Key = OTP key number 6
	PT = OTP key number 11
	0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 
	0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 
	*/
	const unsigned char example_ct5[] = {
		0x6c, 0x7c, 0xd4, 0xb7, 0xec, 0xa3, 0x10, 0x71, 0x39, 0x3a, 0xca, 0x82, 0xc1, 0x5e, 0xa9, 0xe4, 
		0xd8, 0x52, 0x69, 0x5d, 0xbd, 0xa1, 0x7a, 0x48, 0xff, 0x45, 0x51, 0x53, 0xef, 0x35, 0xb7, 0x2f,
	};
	/* example 6: OTP key number 12
	Key = OTP key number 6
	PT = OTP key number 12
	0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 
	0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 
	*/
	const unsigned char example_ct6[] = {
		0xb7, 0x49, 0x57, 0x22, 0xf5, 0x17, 0x9b, 0xb4, 0x7d, 0x7c, 0xe1, 0x27, 0x04, 0xbe, 0x18, 0x98, 
		0x97, 0xc3, 0xc2, 0x86, 0xc7, 0xfa, 0xfa, 0x6a, 0x46, 0x6a, 0xcc, 0x1a, 0x33, 0xc6, 0x77, 0x37, 
	};
	/* example 7: OTP key number 13
	Key = OTP key number 6
	PT = OTP key number 13
	0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 
	0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 
	*/
	const unsigned char example_ct7[] = {
		0x5d, 0xc2, 0x59, 0x47, 0x89, 0xa8, 0x20, 0xad, 0xdb, 0xbe, 0xb7, 0x8f, 0x1c, 0x82, 0x84, 0x88, 
		0xc0, 0xfc, 0x18, 0x73, 0xec, 0x40, 0x21, 0xbc, 0x78, 0xea, 0xee, 0x36, 0x5a, 0x05, 0x67, 0x52, 
	};

    example_no = key_idx - 6;
	ptextlen = 32;

	debug("\n");
	debug("key_key_generate_test: [key_idx: %d] should: [7-13]\n", key_idx);

	switch (example_no) {
		case 1:
			hw_enc_compare_buff = (u8*)example_ct1;
			break;
		case 2:
			hw_enc_compare_buff = (u8*)example_ct2;
			break;
		case 3:
			hw_enc_compare_buff = (u8*)example_ct3;
			break;
		case 4:
			hw_enc_compare_buff = (u8*)example_ct4;
			break;
		case 5:
			hw_enc_compare_buff = (u8*)example_ct5;
			break;
		case 6:
			hw_enc_compare_buff = (u8*)example_ct6;
			break;
		case 7:
			hw_enc_compare_buff = (u8*)example_ct7;
			break;
		default:
			debug("not support example\n");
			fail++;
			return (fail == 0);
	}

	/*do encrypt*/
	ret = (api_func_ptr->ptr_KEK_KEY_Generate)(key_idx, hw_enc_buff);

	/*compare encrypt result*/
	for (i=0;i<ptextlen;i++) {
		if (hw_enc_compare_buff[i] != hw_enc_buff[i]) {
			debug("hw aes encrypt result fail @%d\n",i);
			fail++;
			break;
		}
	}

	/*printout key and first block*/
	aes_print_buffer("hw encrypt",hw_enc_buff,ptextlen);

	if (fail) {
		debug("key_key_generate_test: test fail\n");
	} else {
		debug("key_key_generate_test: test pass\n");
	}
	debug("\n");
	return fail;
}

/**
 * @brief aes_cbc_test - AES CBC Encrypt/Decrypt api test with fixed parameters
 * @param [example_no] input, indicates which example will be chosen to run. should be 1-3
 *
 * @return directly if failed
 */
static int aes_cbc_test(int example_no)
{
	unsigned char key[32] = {0};
	unsigned char iv[16] = {0};
	unsigned char enc_out_iv[16] = {0};
	unsigned char dec_out_iv[16] = {0};
	int keylen = 0;
	int ivlen = 0;
	int keymode = 0;
	int ptextlen = 0;
	int fail = 0;
	int ret = 0;
	int i;

	u8 pbuf[TEST_LEN];
	u8 hw_enc_buff[TEST_LEN];
	u8 hw_dec_buff[TEST_LEN];
	u8 *hw_enc_compare_buff;
	u8 *hw_enc_out_iv_compare_buff;
	u8 *hw_dec_out_iv_compare_buff;

	memset(pbuf,0,sizeof(pbuf));
	memset(hw_enc_buff,0,sizeof(hw_enc_buff));
	memset(hw_dec_buff,0,sizeof(hw_dec_buff));

	/* example 1: SW key
	input:
	[Keylen = 256 bit]   :[256, 128]
	[IVlen = 16 byte]    :16
	[PTlen = 16 byte]    :[0 - 2^28-1]

	output:
	[CTlen = 16 byte]    :[0 - 2^28-1]
	[OutIVlen = 16 byte]   :16
	*/
	const unsigned char example_key1[] = {
		0xfa, 0x9d, 0x00, 0x9f, 0x40, 0x82, 0x3c, 0xe5, 0xd5, 0x47, 0xdc, 0xe4, 0xce, 0x07, 0x53, 0xc8, 
		0x8b, 0xd0, 0xde, 0x60, 0x04, 0x0a, 0xf5, 0xc4, 0x0e, 0xe2, 0x94, 0x88, 0x29, 0x6e, 0xd9, 0xb6, 
	};
	const unsigned char example_iv1[] = {
		0xfb, 0x59, 0xf5, 0x71, 0xa6, 0x31, 0x37, 0x1f, 0xf9, 0x38, 0xf3, 0x72, 0xcd, 0x83, 0x6b, 0x75, 
	};
	const unsigned char example_pt1[] = {
		0x38, 0x58, 0x76, 0xd1, 0x8e, 0x6e, 0x56, 0x26, 0x2b, 0xa4, 0x1d, 0x2a, 0x15, 0xb3, 0xd7, 0x95, 
	};
	const unsigned char example_ct1[] = {
		0x1d, 0x32, 0x89, 0x36, 0xcd, 0xe8, 0x39, 0x4f, 0x1d, 0x08, 0x2a, 0x4d, 0x9b, 0x55, 0x81, 0x8e, 
	};
	const unsigned char example_enc_out_iv1[] = {
		0x1d, 0x32, 0x89, 0x36, 0xcd, 0xe8, 0x39, 0x4f, 0x1d, 0x08, 0x2a, 0x4d, 0x9b, 0x55, 0x81, 0x8e, 
	};
	const unsigned char example_dec_out_iv1[] = {
		0x1d, 0x32, 0x89, 0x36, 0xcd, 0xe8, 0x39, 0x4f, 0x1d, 0x08, 0x2a, 0x4d, 0x9b, 0x55, 0x81, 0x8e, 
	};

	/* example 2: OTP key1(real hw key number=5)
	input:
	[Keylen = 256 bit]   :[256, 128]
	[IVlen = 16 byte]    :16
	[PTlen = 16 byte]    :[0 - 2^28-1]

	output:
	[CTlen = 16 byte]    :[0 - 2^28-1]
	[OutIVlen = 16 byte]   :16
	*/
	const unsigned char example_key2[] = {
		0x34, 0xf1, 0x4b, 0xd9, 0x1b, 0x32, 0x0f, 0x05, 0xb1, 0x5c, 0x6e, 0x72, 0x9d, 0xec, 0x42, 0x74, 
		0xd1, 0xae, 0x29, 0x36, 0x53, 0xe1, 0x89, 0x86, 0xd8, 0x88, 0xfe, 0xcc, 0x39, 0xbb, 0x30, 0x81, 
	};
	const unsigned char example_iv2[] = {
		0x08, 0x67, 0x28, 0x9b, 0xca, 0x64, 0x1f, 0xba, 0xa9, 0x6b, 0x9b, 0xab, 0x8c, 0xe6, 0x87, 0x43, 
	};
	const unsigned char example_pt2[] = {
		0x4d, 0x56, 0x8d, 0x8f, 0x1f, 0xc9, 0xe4, 0x58, 0x9a, 0x19, 0x6e, 0x1a, 0xb6, 0xf4, 0x0d, 0xdd, 
	};
	const unsigned char example_ct2[] = {
		0x6d, 0x3a, 0xcb, 0xcf, 0x42, 0xd7, 0x3c, 0x0f, 0x08, 0xd1, 0x5e, 0xab, 0xce, 0xe3, 0xb5, 0x45, 
	};
	const unsigned char example_enc_out_iv2[] = {
		0x6d, 0x3a, 0xcb, 0xcf, 0x42, 0xd7, 0x3c, 0x0f, 0x08, 0xd1, 0x5e, 0xab, 0xce, 0xe3, 0xb5, 0x45, 
	};
	const unsigned char example_dec_out_iv2[] = {
		0x6d, 0x3a, 0xcb, 0xcf, 0x42, 0xd7, 0x3c, 0x0f, 0x08, 0xd1, 0x5e, 0xab, 0xce, 0xe3, 0xb5, 0x45, 
	};

	/* example 3: OTP key2(real hw key number=6)
	input:
	[Keylen = 256 bit]   :[256, 128]
	[IVlen = 16 byte]    :16
	[PTlen = 16 byte]    :[0 - 2^28-1]

	output:
	[CTlen = 16 byte]    :[0 - 2^28-1]
	[OutIVlen = 16 byte]   :16
	*/
	const unsigned char example_key3[] = {
		0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 
		0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
	};
	const unsigned char example_iv3[] = {
		0xb9, 0xda, 0x4d, 0xca, 0x17, 0x88, 0x0d, 0x02, 0xe9, 0x15, 0x4e, 0x6f, 0xbe, 0xf4, 0x2c, 0x62, 
	};
	const unsigned char example_pt3[] = {
		0xd3, 0x2a, 0xf2, 0x1b, 0xc6, 0xaf, 0x0e, 0xbb, 0x3c, 0xbd, 0xca, 0x50, 0x27, 0x41, 0xa8, 0x77, 
	};
	const unsigned char example_ct3[] = {
		0x02, 0x41, 0x44, 0xee, 0x6f, 0x22, 0x44, 0x39, 0x30, 0xa6, 0x30, 0x5d, 0x95, 0x87, 0x7c, 0xbf, 
	};
	const unsigned char example_enc_out_iv3[] = {
		0x02, 0x41, 0x44, 0xee, 0x6f, 0x22, 0x44, 0x39, 0x30, 0xa6, 0x30, 0x5d, 0x95, 0x87, 0x7c, 0xbf, 
	};
	const unsigned char example_dec_out_iv3[] = {
		0x02, 0x41, 0x44, 0xee, 0x6f, 0x22, 0x44, 0x39, 0x30, 0xa6, 0x30, 0x5d, 0x95, 0x87, 0x7c, 0xbf, 
	};

	debug("\n");
	debug("aes_cbc_test: [example_no: %d]\n", example_no);
	debug("example_no for choice:1|2|3 total 3 example data\n");

	switch (example_no) {
	case 1:
		keymode = SW_KEY;
		keylen = sizeof(example_key1);
		memcpy(key,example_key1,sizeof(example_key1));
		ivlen = sizeof(example_iv1);
		memcpy(iv,example_iv1,ivlen);
		ptextlen = sizeof(example_pt1);
		memcpy(pbuf,example_pt1,ptextlen);
		hw_enc_compare_buff = (u8*)example_ct1;
		hw_enc_out_iv_compare_buff = (u8*)example_enc_out_iv1;
		hw_dec_out_iv_compare_buff = (u8*)example_dec_out_iv1;
		break;
	case 2:
		keymode = OTP_KEY1;
		keylen = sizeof(example_key2);
		memcpy(key,example_key2,sizeof(example_key2));
		ivlen = sizeof(example_iv2);
		memcpy(iv,example_iv2,ivlen);
		ptextlen = sizeof(example_pt2);
		memcpy(pbuf,example_pt2,ptextlen);
		hw_enc_compare_buff = (u8*)example_ct2;
		hw_enc_out_iv_compare_buff = (u8*)example_enc_out_iv2;
		hw_dec_out_iv_compare_buff = (u8*)example_dec_out_iv2;
		break;
	case 3:
		keymode = OTP_KEY2;
		keylen = sizeof(example_key3);
		memcpy(key,example_key3,sizeof(example_key3));
		ivlen = sizeof(example_iv3);
		memcpy(iv,example_iv3,ivlen);
		ptextlen = sizeof(example_pt3);
		memcpy(pbuf,example_pt3,ptextlen);
		hw_enc_compare_buff = (u8*)example_ct3;
		hw_enc_out_iv_compare_buff = (u8*)example_enc_out_iv3;
		hw_dec_out_iv_compare_buff = (u8*)example_dec_out_iv3;
		break;
	default:
		debug("not support example\n");
		fail++;
		return fail;
	}

    debug("HW encrypt/decrypt: Klen:%d, keymode:%s, IVlen: %d, Plen:%d\n",
        (keylen),
        (keymode == SW_KEY) ? "sw key" : ((keymode == OTP_KEY1) ? "otp key1" : "otp key2"),
        ivlen, ptextlen);

	/*do encrypt*/
    ret = (api_func_ptr->ptr_AES_CBC_Encrypt)(keymode, key, iv, pbuf, ptextlen, hw_enc_buff, enc_out_iv);

	/*do decrypt*/
    ret = (api_func_ptr->ptr_AES_CBC_Decrypt)(keymode, key, iv, hw_enc_buff, ptextlen, hw_dec_buff, dec_out_iv);

	/*compare encrypt/decrypt result with origin plaintext*/
	for (i=0;i<ptextlen;i++) {
		if (pbuf[i] != hw_dec_buff[i]) {
			debug("hw aes encrypt/decrypt fail @%d\n",i);
			fail++;
			break;
		}
	}
	/*compare encrypt result*/
	for (i=0;i<ptextlen;i++) {
		if (hw_enc_compare_buff[i] != hw_enc_buff[i]) {
			debug("hw aes encrypt result fail @%d\n",i);
			fail++;
			break;
		}
	}
	/*compare hw/sw out iv result*/
	for (i=0;i<ivlen;i++) {
		if (hw_enc_out_iv_compare_buff[i] != enc_out_iv[i]) {
			debug("hw/sw cbc enc outiv result diff @%d\n",i);
			fail++;
			break;
		}
		if (hw_dec_out_iv_compare_buff[i] != dec_out_iv[i]) {
			debug("hw/sw cbc dec outiv result diff @%d\n",i);
			fail++;
			break;
		}
	}

	/*printout key and first block*/
	ptextlen = (ptextlen>512)?512:ptextlen;
	aes_print_buffer("key",key,keylen);
	aes_print_buffer("iv",iv,ivlen);
	aes_print_buffer("enc_out_iv",enc_out_iv,ivlen);
	aes_print_buffer("dec_out_iv",dec_out_iv,ivlen);
	aes_print_buffer("plain",pbuf,ptextlen);
	aes_print_buffer("hw decrypt",hw_dec_buff,ptextlen);
	aes_print_buffer("hw encrypt",hw_enc_buff,ptextlen);

	if (fail) {
		debug("aes_cbc_test: test fail,if otp key, please set otp key firstly\n");
	} else {
		debug("aes_cbc_test: test pass\n");
	}
	debug("\n");
	return fail;
}

/**
 * @brief aes_gcm_test - AES GCM Encrypt/Decrypt api test with fixed parameters
 * @param [example_no] input, indicates which example will be chosen to run. should be 1-3
 *
 * @return directly if failed
 */
static int aes_gcm_test(int example_no)
{
	unsigned char key[32] = {0};
	unsigned char iv[12] = {0};
	unsigned char header[32] = {0};
	unsigned char tag_result[16] = {0};
	int keylen = 0;
	int ivlen = 0;
	int headerlen = 0;
	unsigned long taglen = 0;
	unsigned long maclen = 0;
	int keymode = 0;
	int ptextlen = 0;
	int fail = 0;
	int ret = 0;
	int i;

	u8 pbuf[TEST_LEN];
	u8 hw_enc_buff[TEST_LEN];
	u8 hw_dec_buff[TEST_LEN];
	u8 *hw_enc_compare_buff;
	u8 *hw_mac_compare_buff;

	memset(pbuf,0,sizeof(pbuf));
	memset(hw_enc_buff,0,sizeof(hw_enc_buff));
	memset(hw_dec_buff,0,sizeof(hw_dec_buff));

	/* example 1: sw key
	input:
	[Keylen = 256 bit]  :[256, 128]
	[IVlen = 12 byte]
	[Headerlen/Aadlen = 24 byte]    :[0 - 2^32-1]
	[PTlen = 16 byte]    :[0 - 2^28-1]

	output:
	[CTlen = 16 byte]
	[Maclen/Taglen = 15 byte]   [12,13,14,15,16]
	*/
	const unsigned char example_key1[] = {
		0xab, 0xed, 0x39, 0xa1, 0x72, 0xb4, 0x04, 0x39, 0x54, 0xb7, 0x95, 0x65, 0x1a, 0xa9, 0xa2, 0xe4, 
		0xab, 0x3f, 0x5c, 0x81, 0xd9, 0xdb, 0xcf, 0x88, 0xfd, 0x07, 0xcd, 0xec, 0xd4, 0x33, 0xcf, 0x99,
	};
	const unsigned char example_iv1[] = {
		0xbe, 0x1e, 0x58, 0xd7, 0xf5, 0x5b, 0xab, 0x20, 0x4f, 0x73, 0x18, 0x46, 
	};
	const unsigned char example_header1[] = {
		0x0a, 0xd7, 0x1f, 0x7e, 0x44, 0x75, 0x6e, 0xee, 0x82, 0x5d, 0x90, 0x28, 0x63, 0xf4, 0x4a, 0xa6, 
		0xa7, 0xf6, 0x43, 0x26, 0xd7, 0x4a, 0x8e, 0x44, 
	};
	const unsigned char example_pt1[] = {
		0xc8, 0x81, 0xe0, 0xc4, 0xe2, 0xd4, 0x0d, 0x4e, 0xdb, 0xde, 0x95, 0x6a, 0x8c, 0x85, 0x1f, 0xf6, 
	};
	const unsigned char example_ct1[] = {
		0x48, 0x5a, 0x24, 0x5e, 0x47, 0x75, 0x3a, 0xdd, 0x73, 0x4b, 0x4c, 0x21, 0xc8, 0x3b, 0xf7, 0x41, 
	};
	const unsigned char example_tag1[] = {
		0xeb, 0xe7, 0x41, 0x48, 0x59, 0xf4, 0xda, 0x79, 0x2c, 0x2f, 0x9d, 0xbc, 0x6a, 0x73, 0xa8, 
	};

	/* example 2: OTP key1
	input:
	[Keylen = 256 bit]  :[256, 128]
	[IVlen = 12 byte]
	[Headerlen/Aadlen = 24 byte]    :[0 - 2^32-1]
	[PTlen = 16 byte]    :[0 - 2^28-1]

	output:
	[CTlen = 16 byte]
	[Maclen/Taglen = 15 byte]   [12,13,14,15,16]
	*/
	const unsigned char example_key2[] = {
		0x34, 0xf1, 0x4b, 0xd9, 0x1b, 0x32, 0x0f, 0x05, 0xb1, 0x5c, 0x6e, 0x72, 0x9d, 0xec, 0x42, 0x74, 
		0xd1, 0xae, 0x29, 0x36, 0x53, 0xe1, 0x89, 0x86, 0xd8, 0x88, 0xfe, 0xcc, 0x39, 0xbb, 0x30, 0x81, 
	};
	const unsigned char example_iv2[] = {
		0x0b, 0xf4, 0xab, 0xb4, 0x17, 0x73, 0x87, 0x2e, 0x6c, 0x36, 0xb1, 0x7c, 
	};
	const unsigned char example_header2[] = {
		0x16, 0xe8, 0xa9, 0x53, 0x65, 0x6d, 0x0f, 0x3c, 0x21, 0xde, 0x1e, 0xfa, 0x7e, 0xb0, 0xae, 0xb6, 
		0x92, 0x4f, 0x2d, 0x1e, 0x5a, 0x16, 0xa0, 0x98, 
	};
	const unsigned char example_pt2[] = {
		0xbe, 0xc9, 0x27, 0x75, 0x92, 0x97, 0x32, 0x29, 0xef, 0x31, 0xc5, 0x5b, 0xa1, 0x15, 0x36, 0x1f, 
	};
	const unsigned char example_ct2[] = {
		0xe6, 0x73, 0x3f, 0x36, 0x1c, 0x51, 0xe3, 0xdf, 0x21, 0x28, 0xea, 0xdc, 0x20, 0x6a, 0xf0, 0x49, 
	};
	const unsigned char example_tag2[] = {
		0x85, 0x84, 0x11, 0x46, 0x46, 0x25, 0x3d, 0x00, 0x3a, 0xbd, 0x93, 0x45, 0xd9, 0xa2, 0x21, 
	};

	/* example 3: OTP key2
	input:
	[Keylen = 256 bit]  :[256, 128]
	[IVlen = 12 byte]
	[Headerlen/Aadlen = 24 byte]    :[0 - 2^32-1]
	[PTlen = 16 byte]    :[0 - 2^28-1]

	output:
	[CTlen = 16 byte]
	[Maclen/Taglen = 15 byte]   [12,13,14,15,16]
	*/
	const unsigned char example_key3[] = {
		0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 
		0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
	};
	const unsigned char example_iv3[] = {
		0x1f, 0x6e, 0xbf, 0x83, 0x87, 0x93, 0x76, 0xcd, 0x45, 0x7b, 0x3a, 0x82, 
	};
	const unsigned char example_header3[] = {
		0x84, 0x05, 0xeb, 0x42, 0xb3, 0xfb, 0xed, 0xb1, 0x69, 0xbf, 0x80, 0xa1, 0xa3, 0x2f, 0xb9, 0xd8, 
		0x4e, 0x23, 0xaf, 0x03, 0xe9, 0x60, 0x22, 0x87, 
	};
	const unsigned char example_pt3[] = {
		0x84, 0x6e, 0x66, 0xa8, 0x7a, 0x82, 0x1d, 0x1e, 0xb1, 0xa5, 0x61, 0x7a, 0x9f, 0x20, 0xbc, 0xcc, 
	};
	const unsigned char example_ct3[] = {
		0xf7, 0xaa, 0x4b, 0x66, 0x2a, 0xbc, 0xe5, 0x6d, 0x69, 0x32, 0xe3, 0xb4, 0x0a, 0x41, 0x7b, 0x34, 
	};
	const unsigned char example_tag3[] = {
		0x2a, 0xd8, 0xb8, 0xdf, 0xbf, 0xf2, 0x05, 0x2f, 0x1a, 0x49, 0xbf, 0xae, 0x69, 0xec, 0x08, 
	};

	debug("\n");
	debug("aes_gcm_test: [example_no: %d]\n", example_no);
	debug("example_no for choice:1|2|3  total 3 example data\n");

	switch (example_no) {
	case 1:
		keymode = SW_KEY;
		keylen = sizeof(example_key1);
		memcpy(key,example_key1,sizeof(example_key1));
		ivlen = sizeof(example_iv1);
		memcpy(iv,example_iv1,ivlen);
		headerlen = sizeof(example_header1);
		memcpy(header,example_header1,headerlen);
		ptextlen = sizeof(example_pt1);
		memcpy(pbuf,example_pt1,ptextlen);
		maclen = 15;
		taglen = maclen;
		hw_enc_compare_buff = (u8*)example_ct1;
		hw_mac_compare_buff = (u8*)example_tag1;
		break;
	case 2:
		keymode = OTP_KEY1;
		keylen = sizeof(example_key2);
		memcpy(key,example_key3,sizeof(example_key2));
		ivlen = sizeof(example_iv2);
		memcpy(iv,example_iv2,ivlen);
		headerlen = sizeof(example_header2);
		memcpy(header,example_header2,headerlen);
		ptextlen = sizeof(example_pt2);
		memcpy(pbuf,example_pt2,ptextlen);
		maclen = 15;
		taglen = maclen;
		hw_enc_compare_buff = (u8*)example_ct2;
		hw_mac_compare_buff = (u8*)example_tag2;
		break;
	case 3:
		keymode = OTP_KEY2;
		keylen = sizeof(example_key3);
		memcpy(key,example_key3,sizeof(example_key3));
		ivlen = sizeof(example_iv3);
		memcpy(iv,example_iv3,ivlen);
		headerlen = sizeof(example_header3);
		memcpy(header,example_header3,headerlen);
		ptextlen = sizeof(example_pt3);
		memcpy(pbuf,example_pt3,ptextlen);
		maclen = 15;
		taglen = maclen;
		hw_enc_compare_buff = (u8*)example_ct3;
		hw_mac_compare_buff = (u8*)example_tag3;
		break;
	default:
		debug("not support example\n");
		fail++;
		return fail;
	}

    debug("HW encrypt/decrypt: Klen:%d, keymode:%s, IVlen:%d, Alen:%d, Tlen: %d, Plen:%d\n",
        (keylen),
        (keymode == SW_KEY) ? "sw key" : ((keymode == OTP_KEY1) ? "otp key1" : "otp key2"),
        ivlen, headerlen, maclen, ptextlen);

	/*do encrypt*/
    ret = (api_func_ptr->ptr_AES_GCM_Encrypt)(keymode, key, iv, pbuf, ptextlen,
			header, headerlen, tag_result, &taglen, hw_enc_buff);

	/*do decrypt*/
    ret = (api_func_ptr->ptr_AES_GCM_Decrypt)(keymode, key, iv, hw_enc_buff, ptextlen,
			header, headerlen, tag_result, &taglen, hw_dec_buff);

	/*compare encrypt/decrypt result with origin plaintext*/
	for (i=0;i<ptextlen;i++) {
		if (pbuf[i] != hw_dec_buff[i]) {
			debug("hw aes encrypt/decrypt fail @%d\n",i);
			fail++;
			break;
		}
	}
	/*compare encrypt result*/
	for (i=0;i<ptextlen;i++) {
		if (hw_enc_compare_buff[i] != hw_enc_buff[i]) {
			debug("hw aes encrypt result fail @%d\n",i);
			fail++;
			break;
		}
	}

	/*compare tag*/
	if (taglen != maclen)
		debug("hw/sw taglen not match: %d %d\n", taglen, maclen);
	for (i = 0; i < maclen; i++) {
		if (hw_mac_compare_buff[i] != tag_result[i]) {
			debug("hw/sw aes encrypt tag result diff @%d\n",i);
			fail++;
			break;
		}
	}

	/*printout key and first block*/
	ptextlen = (ptextlen>512)?512:ptextlen;
	aes_print_buffer("key",key,keylen);
	aes_print_buffer("iv",iv,ivlen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,ptextlen);
	aes_print_buffer("hw decrypt",hw_dec_buff,ptextlen);
	aes_print_buffer("hw encrypt",hw_enc_buff,ptextlen);
	aes_print_buffer("hw tag", tag_result, taglen);

	if (fail) {
		debug("aes_gcm_test: test fail, if otp key, please set otp key firstly\n");
	} else {
		debug("aes_gcm_test: test pass\n");
	}
	debug("\n");
	return fail;
}

/**
 * @brief aes_ccm_test - AES CCM Encrypt/Decrypt api test with fixed parameters
 * @param [example_no] input, indicates which example will be chosen to run. should be 1-3
 *
 * @return directly if failed
 */
static int aes_ccm_test(int example_no)
{
	unsigned char key[32] = {0};
	unsigned char nonce[16] = {0};
	unsigned char header[32] = {0};
	unsigned char tag_result[16] = {0};
	int keylen = 0;
	int noncelen = 0;
	int headerlen = 0;
	unsigned long taglen = 0;
	unsigned long maclen = 0;
	int keymode = 0;
	int ptextlen = 0;
	int fail = 0;
	int ret = 0;
	int i;

	u8 pbuf[TEST_LEN];
	u8 hw_enc_buff[TEST_LEN];
	u8 hw_dec_buff[TEST_LEN];
	u8 *hw_enc_compare_buff;
	u8 *hw_mac_compare_buff;

	memset(pbuf,0,sizeof(pbuf));
	memset(hw_enc_buff,0,sizeof(hw_enc_buff));
	memset(hw_dec_buff,0,sizeof(hw_dec_buff));

	/* example 1:
	input:
	[Keylen = 256 bit]  
	[Noncelen = 12 byte]  [7-13]
	[Headerlen/Aadlen = 20 byte]    [0 - 2^32-1]
	[PTlen = 24 byte]    [0 - 2^28-1]

	output:
	[CTlen = 24 byte]
	[Maclen/Taglen = 8 byte]   [4,6,8,10,12,14,16]
	*/
	const unsigned char example_key1[] = {
		0xbf, 0x13, 0x1a, 0x75, 0x5d, 0xac, 0xfa, 0xa1, 0xeb, 0x47, 0x59, 0xee, 0x49, 0xf4, 0xb1, 0x0f, 
		0x15, 0x8d, 0x12, 0x22, 0x4e, 0xa1, 0xc7, 0x92, 0x43, 0x55, 0x29, 0x8c, 0x48, 0xe4, 0xb0, 0x70, 
	};
	const unsigned char example_nonce1[] = {
		0xb7, 0x97, 0x0f, 0x72, 0x2a, 0xc3, 0x82, 0xc7, 0x72, 0xa5, 0x4f, 0x19, 
	};
	const unsigned char example_header1[] = {
		0xff, 0x57, 0x18, 0x0b, 0x7a, 0x08, 0xe4, 0xf3, 0xca, 0x76, 0x18, 0x6f, 0xa4, 0x51, 0x9f, 0x17, 
		0x8b, 0x03, 0x20, 0x5d, 
	};
	const unsigned char example_pt1[] = {
		0x88, 0x3a, 0x87, 0x52, 0x89, 0x04, 0x5b, 0x51, 0xfe, 0x1b, 0xa7, 0x41, 0x9f, 0x45, 0x54, 0x7b, 
		0x1c, 0x7c, 0x25, 0xb3, 0x7c, 0x41, 0xbb, 0x5e, 
	};
	const unsigned char example_ct1[] = {
		0xd1, 0x70, 0xeb, 0x5c, 0x6c, 0x06, 0xcb, 0xcf, 0x7f, 0x15, 0x86, 0x5b, 0x24, 0xfb, 0x9f, 0xb2, 
		0x80, 0xdf, 0x61, 0xb4, 0x3b, 0xc1, 0x1c, 0x7e, 
	};
	const unsigned char example_tag1[] = {
		0x80, 0xfe, 0xe2, 0x84, 0x69, 0x1b, 0x66, 0x39, 
	};

	/* example 2: OTP key1
	input:
	[Keylen = 256 bit]  
	[Noncelen = 12 byte]  [7-13]
	[Headerlen/Aadlen = 20 byte]    [0 - 2^32-1]
	[PTlen = 24 byte]    [0 - 2^28-1]

	output:
	[CTlen = 24 byte]
	[Maclen/Taglen = 8 byte]   [4,6,8,10,12,14,16]
	*/
	const unsigned char example_key2[] = {
		0x34, 0xf1, 0x4b, 0xd9, 0x1b, 0x32, 0x0f, 0x05, 0xb1, 0x5c, 0x6e, 0x72, 0x9d, 0xec, 0x42, 0x74, 
		0xd1, 0xae, 0x29, 0x36, 0x53, 0xe1, 0x89, 0x86, 0xd8, 0x88, 0xfe, 0xcc, 0x39, 0xbb, 0x30, 0x81, 
	};
	const unsigned char example_nonce2[] = {
		0x90, 0xec, 0x41, 0xb9, 0x9d, 0xce, 0x9d, 0xe6, 0x97, 0xa1, 0x14, 0x70, 
	};
	const unsigned char example_header2[] = {
		0x0f, 0x97, 0x36, 0x07, 0x74, 0x32, 0xd0, 0xa7, 0xa1, 0x06, 0x3d, 0x27, 0x97, 0x73, 0x23, 0x97, 
		0xc0, 0x80, 0x08, 0x2b, 
	};
	const unsigned char example_pt2[] = {
		0xfd, 0x9a, 0xf5, 0x86, 0x35, 0x7a, 0xcc, 0x38, 0x9b, 0x3c, 0x79, 0x6b, 0xa9, 0xeb, 0xcf, 0x3f, 
		0xbf, 0x10, 0x8a, 0x5b, 0x58, 0x00, 0x14, 0xf0, 
	};
	const unsigned char example_ct2[] = {
		0x5a, 0x8d, 0xd9, 0x51, 0x74, 0xa5, 0x7a, 0x28, 0xc4, 0x4c, 0x65, 0x78, 0xf1, 0xd0, 0x95, 0x3c, 
		0xc7, 0x10, 0x40, 0xba, 0xae, 0x57, 0xf7, 0x55, 
	};
	const unsigned char example_tag2[] = {
		0xbc, 0xb1, 0x0f, 0xfb, 0xd7, 0x3b, 0xeb, 0x31, 
	};

	/* example 3: OTP key2
	input:
	[Keylen = 256 bit]  
	[Noncelen = 12 byte]  [7-13]
	[Headerlen/Aadlen = 20 byte]    [0 - 2^32-1]
	[PTlen = 24 byte]    [0 - 2^28-1]

	output:
	[CTlen = 24 byte]
	[Maclen/Taglen = 8 byte]   [4,6,8,10,12,14,16]
	*/
	const unsigned char example_key3[] = {
		0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 
		0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
	};
	const unsigned char example_nonce3[] = {
		0xd4, 0xa8, 0x87, 0xe3, 0x5c, 0xd7, 0x0f, 0x1d, 0x21, 0xd8, 0x93, 0x61, 
	};
	const unsigned char example_header3[] = {
		0xa6, 0xb7, 0x3d, 0x97, 0x1e, 0x97, 0x4a, 0x06, 0x48, 0xd1, 0xa1, 0x0a, 0x42, 0x88, 0xa6, 0xb5, 
		0x5d, 0xaf, 0x84, 0x60, 
	};
	const unsigned char example_pt3[] = {
		0xcc, 0xc7, 0xa8, 0xfd, 0xe6, 0x2a, 0xe8, 0x3c, 0x34, 0x1b, 0xfd, 0x6d, 0x6e, 0xbf, 0xb3, 0x97, 
		0x8d, 0xe6, 0x53, 0x90, 0xc5, 0xfd, 0xd3, 0x7a, 
	};
	const unsigned char example_ct3[] = {
		0x87, 0xf9, 0x79, 0xc2, 0x73, 0x0a, 0x23, 0xe6, 0x84, 0xb9, 0x6d, 0xd6, 0x3c, 0x0f, 0x3e, 0xe8, 
		0x75, 0x09, 0x68, 0x0b, 0xab, 0x02, 0xf0, 0x30, 
	};
	const unsigned char example_tag3[] = {
		0x66, 0x30, 0x6f, 0x71, 0x9c, 0xb9, 0x28, 0x11, 
	};

	debug("\n");
	debug("aes_ccm_test: [example_no: %d]\n", example_no);
	debug("example_no for choice:1|2|3  total 3 example data\n");

	switch (example_no) {
	case 1:
		keymode = SW_KEY;
		keylen = sizeof(example_key1);
		memcpy(key,example_key1,sizeof(example_key1));
		noncelen = sizeof(example_nonce1);
		memcpy(nonce,example_nonce1,noncelen);
		headerlen = sizeof(example_header1);
		memcpy(header,example_header1,headerlen);
		ptextlen = sizeof(example_pt1);
		memcpy(pbuf,example_pt1,ptextlen);
		maclen = 8;
		taglen = maclen;
		hw_enc_compare_buff = (u8*)example_ct1;
		hw_mac_compare_buff = (u8*)example_tag1;
		break;
	case 2:
		keymode = OTP_KEY1;
		keylen = sizeof(example_key2);
		memcpy(key,example_key2,sizeof(example_key2));
		noncelen = sizeof(example_nonce2);
		memcpy(nonce,example_nonce2,noncelen);
		headerlen = sizeof(example_header2);
		memcpy(header,example_header2,headerlen);
		ptextlen = sizeof(example_pt2);
		memcpy(pbuf,example_pt2,ptextlen);
		maclen = 8;
		taglen = maclen;
		hw_enc_compare_buff = (u8*)example_ct2;
		hw_mac_compare_buff = (u8*)example_tag2;
		break;
	case 3:
		keymode = OTP_KEY2;
		keylen = sizeof(example_key3);
		memcpy(key,example_key3,sizeof(example_key3));
		noncelen = sizeof(example_nonce3);
		memcpy(nonce,example_nonce3,noncelen);
		headerlen = sizeof(example_header3);
		memcpy(header,example_header3,headerlen);
		ptextlen = sizeof(example_pt3);
		memcpy(pbuf,example_pt3,ptextlen);
		maclen = 8;
		taglen = maclen;
		hw_enc_compare_buff = (u8*)example_ct3;
		hw_mac_compare_buff = (u8*)example_tag3;
		break;
	default:
		debug("not support example\n");
		fail++;
		return fail;
	}

    debug("HW encrypt/decrypt: Klen:%d, keymode:%s, Nlen:%d, Alen:%d, Tlen: %d, Plen:%d\n",
        (keylen),
        (keymode == SW_KEY) ? "sw key" : ((keymode == OTP_KEY1) ? "otp key1" : "otp key2"),
        noncelen, headerlen, maclen, ptextlen);

	/*do encrypt*/
    ret = (api_func_ptr->ptr_AES_CCM_Encrypt)(keymode, key, nonce, noncelen, pbuf, ptextlen,
			header, headerlen, &taglen, hw_enc_buff, tag_result);

	/*do decrypt*/
    ret = (api_func_ptr->ptr_AES_CCM_Decrypt)(keymode, key, nonce, noncelen, hw_enc_buff, ptextlen,
			header, headerlen, tag_result, &taglen, hw_dec_buff);

	/*compare encrypt/decrypt result with origin plaintext*/
	for (i=0;i<ptextlen;i++) {
		if (pbuf[i] != hw_dec_buff[i]) {
			debug("hw aes encrypt/decrypt fail @%d\n",i);
			fail++;
			break;
		}
	}
	/*compare encrypt result*/
	for (i=0;i<ptextlen;i++) {
		if (hw_enc_compare_buff[i] != hw_enc_buff[i]) {
			debug("hw aes encrypt result fail @%d\n",i);
			fail++;
			break;
		}
	}

	/*compare tag*/
	if (taglen != maclen)
		debug("hw/sw taglen not match: %d %d\n", taglen, maclen);
	for (i = 0; i < maclen; i++) {
		if (hw_mac_compare_buff[i] != tag_result[i]) {
			debug("hw/sw aes encrypt tag result diff @%d\n",i);
			fail++;
			break;
		}
	}

	/*printout key and first block*/
	ptextlen = (ptextlen>512)?512:ptextlen;
	aes_print_buffer("key",key,keylen);
	aes_print_buffer("nonce",nonce,noncelen);
	aes_print_buffer("aad",header,headerlen);
	aes_print_buffer("plain",pbuf,ptextlen);
	aes_print_buffer("hw decrypt",hw_dec_buff,ptextlen);
	aes_print_buffer("hw encrypt",hw_enc_buff,ptextlen);
	aes_print_buffer("hw tag", tag_result, taglen);

	if (fail) {
		debug("aes_ccm_test: test fail,if otp key, please set otp key firstly\n");
	} else {
		debug("aes_ccm_test: test pass\n");
	}
	debug("\n");
	return fail;
}


static int aes256_test(void) {
	int ret = 0;
	int i = 0;

	for (i = 1; i <= 3; i++) {
		ret += aes_cbc_test(i);
	}

	for (i = 1; i <= 3; i++) {
		ret += aes_gcm_test(i);
	}

	for (i = 1; i <= 3; i++) {
		ret += aes_ccm_test(i);
	}

	for (i = 7; i <= 13; i++) {
		ret += key_key_generate_test(i);
	}
	return ret;
}

#endif

/**
 * @brief aes_func_test - AES Encrypt/Decrypt api test with fixed parameters
 *
 */
static int aes_func_test(int argc, char* argv[])
{
	int result = 0;

	result = aes256_test();
	if(result != 0)
		info("aes_func_test failed! %d, please set OTP firstly and reset\n", result);
	else
	{
		info("aes_func_test success!\n");
	}
	return 0;
}

/**
 * @brief image_verify_test - Verify the image data by calling verify api
 * @param secure_flag input, 0:none secure; 1:secure
 * @param store_type input, 0:store with runtime mode; 1:store with sequential mode next to header
 *
 * @return directly if failed
 */
static int image_verify_test(int argc, char* argv[])
{
	// 256 Image header
	unsigned char *image_ptr = NULL;
	uint8_t secure_flag = 0;
	uint8_t header_mode, image_mode, store_mode;
	uint32_t result = 0;

#ifndef TAPEOUT_V2
	if (argc < 5) {
		info("wrong cmd_format: %s secure_flag header_mode image_mode store_mode\n", argv[0]);
		return 1;
	}

	secure_flag = get_arg_ulong(argv[1]);
	header_mode = get_arg_ulong(argv[2]);
	image_mode = get_arg_ulong(argv[3]);
	store_mode = get_arg_ulong(argv[4]);
#else
	if (argc < 3) {
		info("wrong cmd_format: %s secure_flag store_mode\n", argv[0]);
		return 1;
	}

	secure_flag = get_arg_ulong(argv[1]);
	store_mode = get_arg_ulong(argv[2]);
#endif

	if (secure_flag == 1)
		image_ptr = cdl_tiny_sec_fw_bin;
	else
		image_ptr = cdl_tiny_fw_bin;
#ifndef TAPEOUT_V2
	result = (api_func_ptr->ptr_image_verify)((img_header_t *)image_ptr, (uint32_t)image_ptr, secure_flag, header_mode, image_mode, store_mode);
#else
	result = (api_func_ptr->ptr_image_header_verify)((img_header_t *)image_ptr, (uint32_t)image_ptr, 
			(secure_flag == 1) ? DUMMY_SECURE_MODE : NONSECURE_MODE);
	if(result == TRANSFER_VALID)
	{
		result = (api_func_ptr->ptr_image_verify)((img_header_t *)image_ptr, (uint32_t)image_ptr, 
				(secure_flag == 1) ? DUMMY_SECURE_MODE : NONSECURE_MODE , store_mode);
	}
#endif
	if(result == IMAGE_VALID)
		info("image_verify_test %s success!\n", (secure_flag==1)?"secu":"non-secu");
	else
	{
		info("image_verify_test %s failed!\n",  (secure_flag==1)?"secu":"non-secu");
	}
	return 0;
}

#define MAX_RNG_LEN 64

/**
 * @brief rng_test - Generate random data by calling ptr_RNG_Read api
 * @param len input, The random data length in bytes
 *
 * @return directly if failed
 */
static int rng_test(int argc, char* argv[])
{
	int size = 0;
	int result = 0;
	uint8_t rng[MAX_RNG_LEN] = { 0 };

	if (argc < 2) {
		info("wrong cmd_format: %s len\n", argv[0]);
		return 1;
	}

	size = get_arg_ulong(argv[1]);
	if (size > MAX_RNG_LEN) {
		info("len should be < %d\n", MAX_RNG_LEN);
		return 1;
	}
	result = (api_func_ptr->ptr_RNG_Read)(rng, size);
	/*printout Generate random*/
	aes_print_buffer("rng", rng, size);
	info("RNG_test: please check the result by yourself\n");
	return 0;
}

/**
 * @brief rng_data_dump - Generate random data by calling ptr_RNG_Read api and dump
 *
 * @return directly if failed
 */
static int rng_data_dump(int argc, char* argv[])
{
	int i=0, j=0;
	int *rand_buf;
	int result = 0;

	info("rng_api_test: generate 4M*4 bytes, need more than serveral hours to dump data for nist calc\n");

	if (argc < 1) 
	{
		info("wrong cmd_format: rng_test\r\n");
		return 0;
	}
#define MAX_NUM (4*1024*1024)
	rand_buf = (int*)malloc(MAX_NUM*sizeof(int));
	if(rand_buf == NULL)
	{
		info("rng malloc error\n");
		goto __end0;
	}

	result = (api_func_ptr->ptr_RNG_Read)(rand_buf, MAX_NUM*sizeof(int));
	#if 1
	for (i=0;i<MAX_NUM;i++){
		for(j=0; j<32; j++){
			
			info("%01x",rand_buf[i]&0x01);
			rand_buf[i] = rand_buf[i]>>1;
		}
	}
	#else
	for (i=0;i<MAX_NUM;i++){
		info("%08x",rand_buf[i]);
	}

	#endif
	info("\r\n");
__end0: 
	free(rand_buf);

return 0;
}

#define ECC_KEY_SIZE 48

unsigned char test_priv_0[] = {
  0x8a, 0x10, 0x98, 0xcd, 0xf4, 0xa5, 0xad, 0x92, 0xab, 0x1a, 0x05, 0x22,
  0x2d, 0xc6, 0x38, 0x73, 0xec, 0xf7, 0x8c, 0x01, 0x07, 0x8a, 0x57, 0x34,
  0xc0, 0x98, 0xdc, 0x6c, 0x5d, 0x6a, 0x29, 0x47, 0xa9, 0xce, 0x27, 0xc3,
  0x40, 0xda, 0x7f, 0x2f, 0x4e, 0x6f, 0x09, 0x4b, 0x5d, 0xb2, 0xef, 0xf0
};

#ifdef TAPEOUT_V2
unsigned char test_priv_1[] = {
  /*0x8a,*/ 0x10, 0x98, 0xcd, 0xf4, 0xa5, 0xad, 0x92, 0xab, 0x1a, 0x05, 0x22,
  0x2d, 0xc6, 0x38, 0x73, 0xec, 0xf7, 0x8c, 0x01, 0x07, 0x8a, 0x57, 0x34,
  0xc0, 0x98, 0xdc, 0x6c, 0x5d, 0x6a, 0x29, 0x47, 0xa9, 0xce, 0x27, 0xc3,
  0x40, 0xda, 0x7f, 0x2f, 0x4e, 0x6f, 0x09, 0x4b, 0x5d, 0xb2, 0xef, 0xf0,
  0x8a,
};
/* mpz flags */
#define GMP_BN_IS_KEY4		0x1000
#endif

unsigned int test_priv_0_len = 48;
unsigned char test_pub_0[] = {
  0x7d, 0x1b, 0xf5, 0xeb, 0x68, 0xc4, 0x82, 0xba, 0xcf, 0x82, 0x87, 0xcc,
  0x44, 0x9f, 0x8e, 0x17, 0xd3, 0x30, 0x51, 0xf5, 0xd6, 0xdc, 0x21, 0xe0,
  0x11, 0xb4, 0xd4, 0x76, 0x54, 0x24, 0x9b, 0x9d, 0x42, 0xf0, 0xd1, 0x1c,
  0x13, 0x51, 0x2e, 0xeb, 0x9a, 0xed, 0x9e, 0x45, 0x54, 0x16, 0xf4, 0xaa,
  0xb1, 0xef, 0xfa, 0xfa, 0x32, 0xf7, 0xaf, 0xde, 0x7e, 0xd9, 0x13, 0x25,
  0x5e, 0xcf, 0xbf, 0x1b, 0x8f, 0x70, 0xb4, 0xa5, 0xd1, 0x88, 0x71, 0x97,
  0xe9, 0x1c, 0xe6, 0x9f, 0x74, 0xec, 0xaa, 0x79, 0xee, 0xe1, 0x76, 0x9b,
  0x0d, 0xac, 0x17, 0x65, 0x7e, 0xa9, 0xec, 0x3a, 0x5c, 0x16, 0xaa, 0xca
};
unsigned int test_pub_0_len = 96;
unsigned char test_hash_0[] = {
  0xb5, 0xd3, 0x65, 0xc3, 0x8a, 0x42, 0xe9, 0xec, 0xa8, 0x53, 0xae, 0x4a,
  0x10, 0x24, 0xc9, 0x74, 0x12, 0x57, 0x12, 0xa8, 0x03, 0x16, 0x7d, 0x51,
  0xc4, 0xba, 0x0a, 0xcf, 0x97, 0xa7, 0x69, 0xec, 0xd3, 0x11, 0xd0, 0x2b,
  0xef, 0x57, 0x9b, 0x95, 0x33, 0x88, 0x5d, 0x28, 0x71, 0x61, 0xfc, 0x4a
};
unsigned int test_hash_0_len = 48;

unsigned int test_sig_0_0_len = 96;

int repeat = 0;
bool ecc384_sign_verify_test(void)
{
	unsigned char output_sig[test_sig_0_0_len];
	memset(output_sig, 0, test_sig_0_0_len);

#ifndef TAPEOUT_V2
	if(!api_func_ptr->ECC384_Sign(test_priv_0, test_hash_0, output_sig, 0)) {
		info("ECC384_Sign sw priv key failed!\n");
		return false;
	}

	if(IMAGE_VALID != api_func_ptr->ECC384_Verification(test_pub_0, test_hash_0, output_sig)) {
		info("ECC384_Verification sw priv key failed! repeat:%d\n", repeat);
		return false;
	}
	info("ecc384_sign_verify_test sw priv key result passed! repeat:%d\n", repeat);
#else
	info("\necc384 sign and verify hw priv key start, please prepare OTP key4 firstly\n");
	if(!api_func_ptr->ECC384_Sign(test_priv_1, test_hash_0, output_sig, GMP_BN_IS_KEY4)) {
		info("ECC384_Sign hw OTP key4 failed!\n");
		return false;
	}

	if(IMAGE_VALID != api_func_ptr->ECC384_Verification(test_pub_0, test_hash_0, output_sig)) {
		info("ECC384_Verification hw OTP key4 failed! repeat:%d\n", repeat);
		repeat = 0;
		return false;
	}

	info("ecc384_sign_verify_test hw OTP key4 result passed! repeat:%d\n", repeat);
#endif
	repeat++;

	return true;
}

/**
 * @brief ecc384_test - Sign a message digest and verify an ECC signature by calling ecc384 apis
 */
static int ecc384_test(int argc, char* argv[])
{
	bool result;

	result = ecc384_sign_verify_test();
	if(result)
		info("ecc384_test success!\n");
	else
		info("ecc384_test failed! %d, please set OTP firstly and reset\n", result);
	return 0;
}

/**
 * @brief get_boot_interface - get boot interface(nand,qsip, usb or uart)
 */
static int get_boot_interface(int argc, char* argv[])
{
	int boot_interface = 0;

	boot_interface = (api_func_ptr->get_boot_interface)();

	info("get boot interface %d\n", boot_interface);

	return 0;
}

//********************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//********************************************************************
cmdline_entry APIs_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "get_secure_key", get_secure_key, "  : get_secure_key" },
	{ "set_secure_byte", set_secure_byte, "  : set_secure_byte" },
	{ "secure_key_disable", secure_key_disable, "  : secure_key_disable" },
	{ "get_non_secure_byte", get_non_secure_byte, "  : get_non_secure_byte" },
	{ "set_non_secure_byte", set_non_secure_byte, "  : set_non_secure_byte" },
	{ "aes_func_test", aes_func_test, "  : aes_func_test" },
	{ "image_verify_test", image_verify_test, "  : image_verify_test" },
	{ "rng_test", rng_test, "  : Random Number Generation test" },
	{ "rng_data_dump", rng_data_dump, "  : Random Number dump test" },
	{ "ecc384_test", ecc384_test, "  : ECC384 sign verify test" },
#ifdef TAPEOUT_V2
	{ "get_boot_interface", get_boot_interface, "  : get_boot_interface test" },
#endif
	{0,0, 0 }
};

/* if return none zero, something wrong,please check and can't continue test*/
int APIs_test_prepare(int sel)
{
	int ret = 0;

	repeat= 0;

	if (sel == 0) {
		uart_init(UART3_BASE, UART_PCLK);
		ret = sec_api_init();
	} else {
		info("wrong device id\n");
		ret = 1;
	}
	return ret;
}

int APIs_test_cleanup(int sel)
{
#ifndef TAPEOUT_V2
	/*1. EVBV1: system timer: timer7; boot timer:timer7*/
	/*2. EVBV2: system timer: timer7; boot timer:timer0*/
	/*3. So EVBV1 API test may change the setting of system timer, restore it after test*/
	info("%s timer_restore_system:TMR%d\n",__func__, SYS_TIMER);
	gtimer_timer_init(TIMER_BASE, TIMER_PCLK, SYS_TIMER);
#endif
	return 0;
}

#endif


