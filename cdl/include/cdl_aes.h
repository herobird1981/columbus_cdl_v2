/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_AES_H__
#define __CDL_AES_H__

#define AES_MSG_CFG		0x00
#define AES_KEY_SIZE		(0x3<<12)/*0 = 128bit, 2 = 256bit*/
#define AES_MAC_LEN		(0xF<<8)
#define AES_ALG_MODE		(0xF<<4)/*0=ECB,1=CBC,2=CTR,3=CCMIV,4=CMAC*/
#define AES_ENCRYPT		(1<<3)/* 1=encrypt,0=decrypt*/
#define AES_DECRYPT		(0)
#define AES_MSG_BEGIN		(1<<2)
#define AES_MSG_END		(1<<1)
#define AES_GO			(1<<0)

#define AES_ALGO_ECB		0
#define AES_ALGO_CBC		1
#define AES_ALGO_CTR		2
#define AES_ALGO_CCM		3
#define AES_ALGO_CMAC		4
#define AES_ALGO_GCM		5

#define AES_CTX_CFG		0x04
#define AES_CTX_INDEX		(0x3FF<<4)
#define AES_INV_KEY_STR		(1<<3)
#define AES_INV_KEY_RET		(1<<2)
#define AES_CTX_STR		(1<<1)
#define AES_CTX_RET		(1<<0)

#define AES_MSG_TOTAL_BYTES	0x08

#define AES_MSG_AAD_BYTES 	0x0C
#define AES_AAD_LEN		(0xFFFF<<16)
#define AES_MSG_LEN		(0xFFFF<<0)


#define GCM_MODE_AAD_INFO	0x10
#define AES_GMAC		(0x1<<31)
#define AES_AAD_LEN_TOT		(0xFFFF<<0)


#define AES_CTX_KEY_MODE_SEL	0x14
#define KEY_MODE_OTP_KEY2	(1<<2)
#define KEY_MODE_OTP_KEY1	(1<<1)
#define KEY_MODE_CORE_KEY	(1<<0)


#define AES_CTX_KEY_0		0x20//Key[31:0]
#define AES_CTX_KEY_1		0x24//Key[63:32]
#define AES_CTX_KEY_2		0x28//Key[95:64]
#define AES_CTX_KEY_3		0x2C//Key[127:96]
#define AES_CTX_KEY_4		0x30//Key[159:128]
#define AES_CTX_KEY_5		0x34//Key[191:160]
#define AES_CTX_KEY_6		0x38//Key[223:192]
#define AES_CTX_KEY_7		0x3C//Key[255:224]
#define AES_CTX_CBC_KEY_0	0x40//CBC_KEY [31:0]
#define AES_CTX_CBC_KEY_1	0x44//CBC_KEY [63:32]
#define AES_CTX_CBC_KEY_2	0x48//CBC_KEY [95:64]
#define AES_CTX_CBC_KEY_3	0x4C//CBC_KEY [127:96]
#define AES_CTX_CTR_0		0x50//CTR [31:0]
#define AES_CTX_CTR_1		0x54//CTR [63:32]
#define AES_CTX_CTR_2		0x58//CTR [95:64]
#define AES_CTX_CTR_3		0x5C//CTR [127:96]
#define AES_CTX_IV_0		0x60//CCMIV [31:0]
#define AES_CTX_IV_1		0x64//CCMIV [63:32]
#define AES_CTX_IV_2		0x68//CCMIV [95:64]
#define AES_CTX_IV_3		0x6C//CCMIV [127:96]
#define AES_CTX_MAC_0		0x70//MAC [31:0]
#define AES_CTX_MAC_1		0x74//MAC [63:32]
#define AES_CTX_MAC_2		0x78//MAC [95:64]
#define AES_CTX_MAC_3		0x7C//MAC [127:96]

#define AES_INGRESS_FIFO		0x80
#define AES_INGRESS_FIFO_STATUS		0x84

#define AES_ENGRESS_FIFO		0x88
#define	AES_ENGRESS_FIFO_STATUS		0x8C

#define AES_INGRESS_DMA_TOTAL_NUM_REG	0x90
#define AES_INGRESS_DMA_BST_TYPE_REG	0x94
#define AES_DMA_EN			(1<<30)
#define PLAINTEXT_REQ			(1<<31)

#define AES_ENGRESS_DMA_TOTAL_NUM_REG	0x98
#define AES_ENGRESS_DMA_BST_TYPE_REG	0x9c
#define CIPERTEXT_REQ			(1<<31)

#define AES_DONE_STAT_REG		0xA0
#define AES_MAC_VALID			(1<<1)
#define AES_DONE			(1<<0)

#define AES_INGRESS_DMA_DONE_REG	0xA4
#define INGRESS_DMA_DONE		(1<<0)

#define AES_ENGRESS_DMA_DONE_REG	0xA8
#define ENGRESS_DMA_DONE		(1<<0)

#define AES_FLAG_NORMAL		(0x0)
#define AES_FLAG_START		(0x1)
#define AES_FLAG_END		(0x2)
#define AES_FLAG_AAD		(0x10)
#define AES_FLAG_PLAIN_TEXT	(0x20)

struct brite_aes{
	void * regs;

	int key_mode;
	int key_size;
	int key_algo;

	int dma_burst_size;
	int dma_width;

	int dmac_idx;
	int dma_ch_tx;
	int dma_ch_rx;
	int dma_req_tx;
	int dma_req_rx;
	unsigned long dummy_dma_buf;

	int int_vector;

	unsigned long *b_buf;
	unsigned long blen;
	unsigned long gcm_alen;

	unsigned long ptlen;
};

void aes_init(unsigned long base);

int aes_ecb_start(int keymode,unsigned char* key,int keylen);
int aes_ecb_enc(unsigned char *pt, unsigned char *ct,int len);
int aes_ecb_dec(unsigned char *ct, unsigned char *pt,int len);

int aes_cbc_start(int keymode,unsigned char* key,unsigned char* iv,int keylen);
int aes_cbc_enc(unsigned char *pt, unsigned char *ct, int len, unsigned char *out_iv);
int aes_cbc_dec(unsigned char *ct, unsigned char *pt, int len, unsigned char *out_iv);

int aes_ccm_start(int keymode,unsigned char* key,int keylen,
	unsigned char* nonce,int noncelen,
	unsigned char* aad,int aadlen,int plen,int maclen);
int aes_ccm_enc(unsigned char *pt, unsigned char *ct,int len,
	unsigned char* tag, unsigned long* taglen,int flag);
int aes_ccm_dec(unsigned char *ct, unsigned char *pt,int len,
	unsigned char* tag, unsigned long* taglen,int flag);
void aes_ccm_done(void);

int aes_gcm_start(int keymode,unsigned char* key,int keylen,
	unsigned char* iv,int ivlen,
	unsigned char* aad,int aadlen,int plen,int maclen);
int aes_gcm_enc(unsigned char *pt, unsigned char *ct,int len,
	unsigned char* tag, unsigned long* taglen,int flag);
int aes_gcm_dec(unsigned char *ct, unsigned char *pt,int len,
	unsigned char* tag, unsigned long* taglen,int flag);
void aes_gcm_done(void);

int aes_cmac_start(int keymode,unsigned char* key,int keylen,int maclen,int plen);
int aes_cmac_enc(unsigned char *pt,int len,
	unsigned char* tag, unsigned long* taglen,int flag);

#endif
