/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_aes.h"
#include "cdl_dmac.h"
#include "timer.h"
#include "memleak.h"

#define AES_TIMEOUT (100)

struct brite_aes aes_dev = { 0 };
struct brite_aes* p_aes_dev = &aes_dev;

static inline void brite_aes_write(struct brite_aes* priv,
	unsigned long offset,unsigned long val)
{
	__raw_writel(val, (unsigned long)priv->regs + offset);
}

static inline unsigned long brite_aes_read(struct brite_aes* priv,
	unsigned long offset)
{
	return __raw_readl((unsigned long)priv->regs + offset);
}

static void aes_config_key(int mode,int keysize)
{
	u32 value;

	if ((mode == KEY_MODE_CORE_KEY) || (mode == KEY_MODE_OTP_KEY1) ||
		(mode == KEY_MODE_OTP_KEY2))
		p_aes_dev->key_mode = mode;
	else {
		info("invalid mode, use default sw key mode\n");
		p_aes_dev->key_mode = KEY_MODE_CORE_KEY;
	}

	if ((keysize == 128) || (keysize == 256))
		p_aes_dev->key_size = keysize;
	else {
		info("invalid keysize,use default 32(256bits)\n");
		p_aes_dev->key_size = 256;
	}

	brite_aes_write(p_aes_dev,AES_CTX_KEY_MODE_SEL,p_aes_dev->key_mode);

	value=brite_aes_read(p_aes_dev, AES_MSG_CFG);
	value &= ~AES_KEY_SIZE;
	value |= ((p_aes_dev->key_size == 256)?2:0)<<12;
	brite_aes_write(p_aes_dev, AES_MSG_CFG,value);
}

static void aes_set_key(unsigned char * main_key)
{
	u32 *key = (u32*)main_key;

	if (!main_key) {/*may use hw otp key*/
		brite_aes_write(p_aes_dev, AES_CTX_KEY_0, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_1, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_2, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_3, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_4, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_5, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_6, 0);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_7, 0);
		return;
	}
	brite_aes_write(p_aes_dev, AES_CTX_KEY_0, key[0]);
	brite_aes_write(p_aes_dev, AES_CTX_KEY_1, key[1]);
	brite_aes_write(p_aes_dev, AES_CTX_KEY_2, key[2]);
	brite_aes_write(p_aes_dev, AES_CTX_KEY_3, key[3]);

	if (p_aes_dev->key_size == 256) {
		brite_aes_write(p_aes_dev, AES_CTX_KEY_4, key[4]);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_5, key[5]);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_6, key[6]);
		brite_aes_write(p_aes_dev, AES_CTX_KEY_7, key[7]);
	}
}
static void aes_set_iv(unsigned char *aes_iv)
{
	u32 *iv = (u32*)aes_iv;

	brite_aes_write(p_aes_dev, AES_CTX_IV_0, iv[0]);
	brite_aes_write(p_aes_dev, AES_CTX_IV_1, iv[1]);
	brite_aes_write(p_aes_dev, AES_CTX_IV_2, iv[2]);
	brite_aes_write(p_aes_dev, AES_CTX_IV_3, iv[3]);
}
static void aes_set_ctr(unsigned char * ctr)
{
	u32 *iv = (u32*)ctr;

	brite_aes_write(p_aes_dev, AES_CTX_CTR_0, iv[0]);
	brite_aes_write(p_aes_dev, AES_CTX_CTR_1, iv[1]);
	brite_aes_write(p_aes_dev, AES_CTX_CTR_2, iv[2]);
	brite_aes_write(p_aes_dev, AES_CTX_CTR_3, iv[3]);
}
static void aes_set_mac(unsigned char * aes_mac)
{
	u32 *mac = (u32*)aes_mac;

	brite_aes_write(p_aes_dev, AES_CTX_MAC_0, mac[0]);
	brite_aes_write(p_aes_dev, AES_CTX_MAC_1, mac[1]);
	brite_aes_write(p_aes_dev, AES_CTX_MAC_2, mac[2]);
	brite_aes_write(p_aes_dev, AES_CTX_MAC_3, mac[3]);
}
static void aes_set_mac_len(int mac_len)
{
	unsigned long value;

	value=brite_aes_read(p_aes_dev, AES_MSG_CFG);
	value &= ~AES_MAC_LEN;
	value |= (mac_len&0xf)<<8;
	brite_aes_write(p_aes_dev, AES_MSG_CFG,value);
}

static void aes_config_algo(unsigned char mode_value)
{
	unsigned long value;

	p_aes_dev->key_algo = mode_value;

	value=brite_aes_read(p_aes_dev, AES_MSG_CFG);
	value &= ~AES_ALG_MODE;
	value |= (mode_value & 0xf)<<4;

	brite_aes_write(p_aes_dev, AES_MSG_CFG,value);
}
static void aes_config_action(int action)
{
	unsigned long value;

	value=brite_aes_read(p_aes_dev, AES_MSG_CFG);
	value &= ~AES_ENCRYPT;
	if (action)
		value |= AES_ENCRYPT;
	brite_aes_write(p_aes_dev, AES_MSG_CFG,value);
}
static void aes_wait_done(void)
{
	u64 start = 0;

	start = get_timer(0);
	while (!(brite_aes_read(p_aes_dev, AES_DONE_STAT_REG) & AES_DONE) )
	{
		if (get_timer(start) > AES_TIMEOUT)
		{
			info("\n %s fail\n", __func__);
			break;
		}
	}		
}

static void aes_wait_ingress_dma_done(void)
{
	u64 start = 0;
	
	start = get_timer(0);
	while (!(brite_aes_read(p_aes_dev, AES_INGRESS_DMA_DONE_REG) & INGRESS_DMA_DONE) )
	{
		if (get_timer(start) > AES_TIMEOUT)
		{
			info("\n %s fail\n", __func__);
			break;
		}
	}		
}

static void aes_wait_engress_dma_done(void)
{
	u64 start = 0;

	start = get_timer(0);
	while (!(brite_aes_read(p_aes_dev, AES_ENGRESS_DMA_DONE_REG) & ENGRESS_DMA_DONE) )
	{
		if (get_timer(start) > AES_TIMEOUT)
		{
			info("\n %s fail\n", __func__);
			break;
		}
	}		
}
static void aes_start_go(void)
{
	unsigned long value;

	value=brite_aes_read(p_aes_dev, AES_MSG_CFG);
	value &= ~AES_GO;
	value |= AES_GO;
	brite_aes_write(p_aes_dev, AES_MSG_CFG,value);
}

static int aes_dma_tx(unsigned long * tx, unsigned long len)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[p_aes_dev->dmac_idx];
	int ret = 0;
	u32 val;
	u32 transfer_len;

	transfer_len = (len+(1<<p_aes_dev->dma_width)-1)>>p_aes_dev->dma_width;

	brite_aes_write(p_aes_dev, AES_INGRESS_DMA_TOTAL_NUM_REG,(transfer_len<<p_aes_dev->dma_width));
	brite_aes_write(p_aes_dev, AES_INGRESS_DMA_BST_TYPE_REG,
		(p_aes_dev->dma_burst_size<<3)|(p_aes_dev->dma_width));

	/* config src */
	dw_dmac_channel_config(p_dma_tx, p_aes_dev->dma_ch_tx, 0,
		p_aes_dev->dma_width, p_aes_dev->dma_burst_size,
		INCREASE,  MEM_TO_PERI, p_aes_dev->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, p_aes_dev->dma_ch_tx, 1,
		p_aes_dev->dma_width, p_aes_dev->dma_burst_size,
		FIXED,  MEM_TO_PERI, p_aes_dev->dma_req_tx);

	ret = dw_dmac_channel_single_transfer(p_dma_tx, p_aes_dev->dma_ch_tx,
		(unsigned long) tx, (unsigned long)p_aes_dev->regs + AES_INGRESS_FIFO,
		transfer_len,0);
	if (len > 0) {
		val=brite_aes_read(p_aes_dev, AES_INGRESS_DMA_BST_TYPE_REG);
		val |= AES_DMA_EN|PLAINTEXT_REQ;
		brite_aes_write(p_aes_dev, AES_INGRESS_DMA_BST_TYPE_REG,val);
	}
	return ret;
}

static int aes_dma_rx(unsigned long * rx, unsigned long len)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[p_aes_dev->dmac_idx];
	int ret = 0;
	u32 val;
	u32 transfer_len;

	transfer_len = (len+(1<<p_aes_dev->dma_width)-1)>>p_aes_dev->dma_width;

	brite_aes_write(p_aes_dev, AES_ENGRESS_DMA_TOTAL_NUM_REG,(transfer_len<<p_aes_dev->dma_width));
	brite_aes_write(p_aes_dev, AES_ENGRESS_DMA_BST_TYPE_REG,
		(p_aes_dev->dma_burst_size<<3)|(p_aes_dev->dma_width));

	/* config src */
	dw_dmac_channel_config(p_dma_rx, p_aes_dev->dma_ch_rx, 0,
		p_aes_dev->dma_width, p_aes_dev->dma_burst_size,
		FIXED,  PERI_TO_MEM, p_aes_dev->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, p_aes_dev->dma_ch_rx, 1,
		p_aes_dev->dma_width, p_aes_dev->dma_burst_size,
		INCREASE,  PERI_TO_MEM, p_aes_dev->dma_req_rx);

	ret = dw_dmac_channel_single_transfer(p_dma_rx, p_aes_dev->dma_ch_rx,
		(unsigned long)(p_aes_dev->regs + AES_ENGRESS_FIFO),
		(unsigned long)rx, transfer_len, 0);

	if (len > 0) {
		val=brite_aes_read(p_aes_dev, AES_ENGRESS_DMA_BST_TYPE_REG);
		val |= AES_DMA_EN|PLAINTEXT_REQ;
		brite_aes_write(p_aes_dev, AES_ENGRESS_DMA_BST_TYPE_REG,val);
	}
	return ret;
}

static int aes_zero_exec(void)
{
	u32 cfg;

	if ((p_aes_dev->blen != 0) || (p_aes_dev->ptlen != 0)) {
		info("error: this only for blen and ptlen both 0\n");
		return 1;
	}

	/*total len accross all segment*/
	brite_aes_write(p_aes_dev, AES_MSG_TOTAL_BYTES, 0);
	brite_aes_write(p_aes_dev, GCM_MODE_AAD_INFO,0);

	/*set msg segment flag*/
	cfg=brite_aes_read(p_aes_dev, AES_MSG_CFG);
	cfg &= ~(AES_MSG_BEGIN | AES_MSG_END);
	cfg |= AES_MSG_BEGIN;
	cfg |= AES_MSG_END;
	brite_aes_write(p_aes_dev, AES_MSG_CFG,cfg);

	/*set ctx flag*/
	brite_aes_write(p_aes_dev,AES_CTX_CFG,AES_CTX_RET);

	brite_aes_write(p_aes_dev, AES_MSG_AAD_BYTES,0);

	/*configure dma and start action*/
	brite_aes_write(p_aes_dev, AES_INGRESS_DMA_TOTAL_NUM_REG,0);
	brite_aes_write(p_aes_dev, AES_INGRESS_DMA_BST_TYPE_REG,
		(p_aes_dev->dma_burst_size<<3)|(p_aes_dev->dma_width));
	aes_start_go();

	aes_wait_done();
	return 0;
}

static int aes_simple_exec(unsigned char *in, unsigned char *out,int len)
{
	u8 *tx = in;
	u8 *rx = out;
	u32 off = 0;
	int trans_len;
	u32 segment_size;
	u32 cfg;
	u32 ctx_cfg;
	int ret = 0;

	/*total len accross all segment*/
	brite_aes_write(p_aes_dev, AES_MSG_TOTAL_BYTES,len);
	brite_aes_write(p_aes_dev, GCM_MODE_AAD_INFO,0);

	segment_size = 4*(DMAC_MAX_SIZE_PER_BLOCK & (~0x3));
	while (len>0) {
		if (len > segment_size)
			trans_len = segment_size;
		else
			trans_len = len;

		/*set msg segment flag and ctx flag*/
		cfg=brite_aes_read(p_aes_dev, AES_MSG_CFG);
		cfg &= ~(AES_MSG_BEGIN | AES_MSG_END);
		ctx_cfg = 0;

		if (off==0) {
			cfg |= AES_MSG_BEGIN;
			ctx_cfg |= AES_CTX_RET;
		}
		if (len<= segment_size){
			cfg |= AES_MSG_END;
			ctx_cfg |= AES_CTX_STR;
		}
		brite_aes_write(p_aes_dev, AES_MSG_CFG,cfg);
		brite_aes_write(p_aes_dev,AES_CTX_CFG,ctx_cfg);

		brite_aes_write(p_aes_dev, AES_MSG_AAD_BYTES,trans_len);

		/*configure dma and start action*/
		aes_dma_tx((unsigned long *)tx,trans_len);
		if (out)
			aes_dma_rx((unsigned long *)rx,trans_len);
		aes_start_go();

		/*wait dma done*/
		aes_wait_ingress_dma_done();
		if (out)
			aes_wait_engress_dma_done();
		aes_wait_done();

		tx += segment_size;
		rx += segment_size;
		len -= segment_size;
		off += segment_size;
	}
	return ret;
}

static int aes_exec(int flag,unsigned char *in, unsigned char *out,int len)
{
	u8 *tx = in;
	u8 *rx = out;
	u32 off = 0;
	int trans_len;
	u32 segment_size;
	u32 cfg;
	int ret = 0;

	/*total len accross all segment*/
	brite_aes_write(p_aes_dev, AES_MSG_TOTAL_BYTES,p_aes_dev->ptlen);
	if (p_aes_dev->key_algo == AES_ALGO_GCM)
		brite_aes_write(p_aes_dev, GCM_MODE_AAD_INFO,p_aes_dev->gcm_alen);
	else
		brite_aes_write(p_aes_dev, GCM_MODE_AAD_INFO,0);

	segment_size = 4*(DMAC_MAX_SIZE_PER_BLOCK & (~0x3));
	while (len>0) {
		if (len > segment_size)
			trans_len = segment_size;
		else
			trans_len = len;

		/*set msg segment flag*/
		cfg=brite_aes_read(p_aes_dev, AES_MSG_CFG);
		cfg &= ~(AES_MSG_BEGIN | AES_MSG_END);
		/*aad info,first segment*/
		if ((flag & AES_FLAG_AAD) && (off==0)) {
			cfg |= AES_MSG_BEGIN;
		}
		/*if no aad,gcm have this condition*/
		if ((flag & AES_FLAG_START) && (off==0) && p_aes_dev->blen==0) {
			cfg |= AES_MSG_BEGIN;
		}

		/*plain text, last segment*/
		if (((flag & (AES_FLAG_PLAIN_TEXT|AES_FLAG_END)) ==
			(AES_FLAG_PLAIN_TEXT|AES_FLAG_END)) &&
			len<= segment_size){
			cfg |= AES_MSG_END;
		}
		/*if no plain text(length zero)*/
		if ((flag & AES_FLAG_AAD) && (p_aes_dev->ptlen==0) && (len<=segment_size))
			cfg |= AES_MSG_END;
		brite_aes_write(p_aes_dev, AES_MSG_CFG,cfg);

		/*set ctx flag*/
		if (((flag & AES_FLAG_AAD) && (off == 0))
			|| ((flag & AES_FLAG_START) && (off == 0) && p_aes_dev->blen == 0))
			brite_aes_write(p_aes_dev,AES_CTX_CFG,AES_CTX_RET);
		else {
			brite_aes_write(p_aes_dev,AES_CTX_CFG,0);
		}
		if ((flag & AES_FLAG_AAD))
			brite_aes_write(p_aes_dev, AES_MSG_AAD_BYTES,(trans_len<<16)|trans_len);
		else
			brite_aes_write(p_aes_dev, AES_MSG_AAD_BYTES,trans_len);

		/*configure dma and start action*/
		aes_dma_tx((unsigned long *)tx,trans_len);
		if (out)
			aes_dma_rx((unsigned long *)rx,trans_len);
		aes_start_go();

		/*wait dma done*/
		aes_wait_ingress_dma_done();
		if (out)
			aes_wait_engress_dma_done();
		aes_wait_done();

		tx += segment_size;
		rx += segment_size;
		len -= segment_size;
		off += segment_size;
	}
	return ret;
}

static int aes_cmac_exec(int flag,unsigned char *in, unsigned char *out,int len)
{
	u8 *tx = in;
	u8 *rx = out;
	u32 off = 0;
	int trans_len;
	u32 segment_size;
	u32 cfg;
	u32 ctx_cfg;
	int ret = 0;

	/*total len accross all segment*/
	brite_aes_write(p_aes_dev, AES_MSG_TOTAL_BYTES,p_aes_dev->ptlen);
	brite_aes_write(p_aes_dev, GCM_MODE_AAD_INFO,0);

	segment_size = 4*(DMAC_MAX_SIZE_PER_BLOCK & (~0x3));
	while (len>0) {
		if (len > segment_size)
			trans_len = segment_size;
		else
			trans_len = len;

		/*set msg segment flag and ctx flag*/
		cfg=brite_aes_read(p_aes_dev, AES_MSG_CFG);
		cfg &= ~(AES_MSG_BEGIN | AES_MSG_END);
		ctx_cfg = 0;

		if ((flag & AES_FLAG_START) && (off==0)) {
			cfg |= AES_MSG_BEGIN;
			ctx_cfg |= AES_CTX_RET;
		}
		if ((flag & AES_FLAG_END) && (len<= segment_size)){
			cfg |= AES_MSG_END;
			ctx_cfg |= AES_CTX_STR;
		}
		brite_aes_write(p_aes_dev, AES_MSG_CFG,cfg);
		brite_aes_write(p_aes_dev,AES_CTX_CFG,ctx_cfg);

		brite_aes_write(p_aes_dev, AES_MSG_AAD_BYTES,trans_len);

		/*configure dma and start action*/
		aes_dma_tx((unsigned long *)tx,trans_len);
		if (out)
			aes_dma_rx((unsigned long *)rx,trans_len);
		aes_start_go();

		/*wait dma done*/
		aes_wait_ingress_dma_done();
		if (out)
			aes_wait_engress_dma_done();
		aes_wait_done();

		tx += segment_size;
		rx += segment_size;
		len -= segment_size;
		off += segment_size;
	}
	return ret;
}
static unsigned char aes_get_L(unsigned long ptlen,unsigned long noncelen)
{
	unsigned char L;
	int len;

	/* let's get the L value */
	len = ptlen;
	L   = 0;
	while (len) {
		++L;
		len >>= 8;
	}
	if (L <= 1) {
		L = 2;
	}

	/* increase L to match the nonce len */
	noncelen = (noncelen > 13) ? 13 : noncelen;
	if ((15 - noncelen) > L) {
		L = 15 - noncelen;
	}
	return L;
}
static int aes_encap_b(unsigned char* N, int Nlen,
				unsigned char* A, int Alen,int Plen,
				int Mlen)
{
	unsigned char* B;
	unsigned char L;
	int x;
	u8 *aad;
	int i;

	if (p_aes_dev->b_buf) {
		free(p_aes_dev->b_buf);
		p_aes_dev->b_buf = NULL;
	}
	p_aes_dev->blen = (Nlen/16 +1)*16;
	if (Alen >0) {
		if (Alen < ((1UL<<16) - (1UL<<8))) {
			if ((Alen+2)%16 ==0)
				p_aes_dev->blen += (Alen+2);
			else
				p_aes_dev->blen += ((Alen+2)/16 +1)*16;
		}else {
			if ((Alen+6)%16 ==0)
				p_aes_dev->blen += (Alen+6);
			else
				p_aes_dev->blen += ((Alen+6)/16 +1)*16;
		}
	}
	p_aes_dev->b_buf = (unsigned long*)malloc(p_aes_dev->blen);
	if (!p_aes_dev->b_buf) {
		info("allocate memory for B buffer err\n");
		return 0;
	}
	B = (unsigned char*)p_aes_dev->b_buf;
	memset(B,0,p_aes_dev->blen);

	/*B0*/
	L = aes_get_L(Plen,Nlen);
	if (L+Nlen != 15)
		return 0;

	if (Alen) /*have aad info*/
		B[0] = 0x40;
	else
		B[0] = 0x0;
	if (Mlen == 0) {
		B[0] |= (0 << 3) | (L -1);
	} else {
		B[0] |= (((Mlen-2)/2) << 3) | (L -1);
	}
	/*B1-B15: N & Q*/
	memcpy(&B[1], N, Nlen);
	if (L>4)
		L=4;
	for (i=0;i<L;i++) {
		B[(Nlen/16 +1)*16 -(i+1)] = Plen&0xff;
		Plen >>=8;
	}
	/*aad length and data*/
	if (Alen > 0) {
		x = 0;
		aad = (u8*)&B[(Nlen/16 +1)*16 + 0];
		/* store length */
		if (Alen < ((1UL<<16) - (1UL<<8))) {
			aad[x++] = (Alen>>8) & 255;
			aad[x++] = Alen & 255;
		} else {
			aad[x++] = 0xFF;
			aad[x++] = 0xFE;
			aad[x++] = (Alen>>24) & 255;
			aad[x++] = (Alen>>16) & 255;
			aad[x++] = (Alen>>8) & 255;
			aad[x++] = Alen & 255;
		}
		memcpy(&B[(Nlen/16 +1)*16 + x], A, Alen);
	}
	return p_aes_dev->blen;
}

static int aes_encap_gcm(unsigned char* N, int Nlen,
				unsigned char* A, int Alen,int Plen,
				int Mlen)
{
	unsigned char* B;

	if (p_aes_dev->b_buf) {
		free(p_aes_dev->b_buf);
		p_aes_dev->b_buf = NULL;
	}
	if ((Alen % 16) ==0)
		p_aes_dev->blen = Alen;
	else
		p_aes_dev->blen = (Alen/16 +1)*16;
	if (p_aes_dev->blen>0) {
		p_aes_dev->b_buf = (unsigned long*)malloc(p_aes_dev->blen);
		if (!p_aes_dev->b_buf) {
			info("allocate memory for B buffer err\n");
			return 0;
		}
		B = (unsigned char*)p_aes_dev->b_buf;
		memset(B,0,p_aes_dev->blen);
		memcpy(B, A, Alen);
	}
	p_aes_dev->gcm_alen = Alen;
	return p_aes_dev->blen;
}
void aes_init(unsigned long base)
{
	memset(p_aes_dev,0,sizeof(struct brite_aes));
	p_aes_dev->regs = (void*)base;

	p_aes_dev->key_mode = 1; /*Core Configured KEY*/
	p_aes_dev->key_size = 256; /*256bit*/

	p_aes_dev->dma_burst_size = 1; /*burst 4*/
	p_aes_dev->dma_width = 2; /*32bit*/
	aes_config_key(p_aes_dev->key_mode,p_aes_dev->key_size);
}

static int aes_simple_enc(unsigned char *pt, unsigned char *ct,int len)
{
	int ret = 0;

	aes_config_action(AES_ENCRYPT);
	ret = aes_simple_exec(pt,ct,len);

	return ret;
}
static int aes_simple_dec(unsigned char *ct, unsigned char *pt,int len)
{
	int ret = 0;

	aes_config_action(AES_DECRYPT);
	ret = aes_simple_exec(ct,pt,len);

	return ret;
}

int aes_ecb_start(int keymode,unsigned char* key,int keylen)
{
	aes_config_key(keymode,keylen*8);
	aes_set_key(key);

	aes_config_algo(AES_ALGO_ECB);
	return 0;
}
int aes_ecb_enc(unsigned char *pt, unsigned char *ct,int len)
{
	return aes_simple_enc(pt,ct,len);
}
int aes_ecb_dec(unsigned char *ct, unsigned char *pt,int len)
{
	return aes_simple_dec(ct,pt,len);
}

int aes_cbc_start(int keymode,unsigned char* key,unsigned char* iv,int keylen)
{
	aes_config_key(keymode,keylen*8);
	aes_set_key(key);

	aes_set_iv(iv);
	aes_config_algo(AES_ALGO_CBC);
	return 0;
}
int aes_cbc_enc(unsigned char *pt, unsigned char *ct,int len, unsigned char *out_iv)
{
	int ret = 0;
	int i = 0;
	u32 *outptr = (u32*)out_iv;
	u32 *icv = (u32*)(p_aes_dev->regs + AES_CTX_IV_0);

	ret = aes_simple_enc(pt, ct, len);

	for(i = 0; i < 4; i++) {
		outptr[i] = icv[i];
	}
	return ret;
}
int aes_cbc_dec(unsigned char *ct, unsigned char *pt,int len, unsigned char *out_iv)
{
	int ret = 0;
	int i = 0;
	u32 *outptr = (u32*)out_iv;
	u32 *icv = (u32*)(p_aes_dev->regs + AES_CTX_IV_0);

	ret = aes_simple_dec(ct, pt, len);

	for(i = 0; i < 4; i++) {
		outptr[i] = icv[i];
	}
	return ret;
}

int aes_ccm_start(int keymode,unsigned char* key,int keylen,
	unsigned char* nonce,int noncelen,
	unsigned char* aad,int aadlen,int plen,int maclen)
{
	u32 ctr_buf[4];
	u8 *ctr_ptr = (u8*)&ctr_buf[0];
	int ret;

	/* make sure the taglen is even and <= 16 */
	maclen &= ~1;
	if (maclen > 16) {
		return 1;
	}

	ret = aes_encap_b(nonce,noncelen,aad,aadlen,plen,maclen);
	if (!ret)
		return 1;

	memset(ctr_ptr,0,16);
	*(ctr_ptr) = (unsigned char)aes_get_L(plen,noncelen) -1;
	memcpy(ctr_ptr+1,nonce,noncelen);

	aes_config_key(keymode,keylen*8);
	aes_set_key(key);
	aes_set_ctr((unsigned char *)ctr_buf);
	/*set mac len*/
	aes_set_mac_len(maclen);

	aes_config_algo(AES_ALGO_CCM);

	/*store total plain text len*/
	p_aes_dev->ptlen = plen;
	return 0;
}

int aes_ccm_enc(unsigned char *pt, unsigned char *ct,int len,
	unsigned char* tag, unsigned long* taglen,int flag)
{
	int ret = 0;

	aes_config_action(AES_ENCRYPT);

	if (flag & AES_FLAG_START) {
		/*send B buffer: with nonce/aad*/
		ret = aes_exec(flag | AES_FLAG_AAD,(unsigned char*)p_aes_dev->b_buf,NULL,p_aes_dev->blen);
		if (ret)
			return ret;
	}

	/*send plain data*/
	if (len>0)
		ret = aes_exec(flag | AES_FLAG_PLAIN_TEXT,pt,ct,len);

	/*read mac data*/
	if (flag & AES_FLAG_END) {
		if (*taglen == 0)
			return ret;
		memcpy(tag,(char*)((u32)p_aes_dev->regs+AES_CTX_MAC_0),*taglen);
	}
	return ret;
}
int aes_ccm_dec(unsigned char *ct, unsigned char *pt,int len,
	unsigned char* tag, unsigned long* taglen,int flag)
{
	int ret = 0;

	aes_config_action(AES_DECRYPT);

	if (flag & AES_FLAG_START) {
		if (tag != NULL && *taglen != 0)
			aes_set_mac(tag);

		/*send B buffer: with nonce/aad*/
		ret = aes_exec(flag | AES_FLAG_AAD,(unsigned char*)p_aes_dev->b_buf,NULL,p_aes_dev->blen);
		if (ret)
			return ret;
	}
	/*send plain data*/
	if (len>0)
		ret = aes_exec(flag | AES_FLAG_PLAIN_TEXT,ct,pt,len);

	return ret;
}
void aes_ccm_done(void)
{
	if (p_aes_dev->b_buf)
		free(p_aes_dev->b_buf);
	p_aes_dev->b_buf = NULL;
}

int aes_gcm_start(int keymode,unsigned char* key,int keylen,
	unsigned char* iv,int ivlen,
	unsigned char* aad,int aadlen,int plen,int maclen)
{
	u32 ctr_buf[4];
	u8 *ctr_ptr = (u8*)&ctr_buf[0];
	int ret;

	if(aadlen > 0 && NULL == aad)
		return 1;

	/* make sure the maclen is >=12 && <=16 */
	if(maclen > 16 || maclen < 12)
		return 1;

	ret = aes_encap_gcm(iv,ivlen,aad,aadlen,plen,maclen);
	if (!ret && (aadlen>0))
		return 1;

	aes_config_key(keymode,keylen*8);
	aes_set_key(key);

	memset(ctr_ptr,0,16);
	memcpy(ctr_ptr,iv,ivlen);
	*(ctr_ptr+15) = 1;
	aes_set_ctr((unsigned char *)ctr_buf);

	/*set mac len*/
	aes_set_mac_len(maclen);
	aes_config_algo(AES_ALGO_GCM);

	/*store total plain text len*/
	p_aes_dev->ptlen = plen;
	return 0;
}
int aes_gcm_enc(unsigned char *pt, unsigned char *ct,int len,
	unsigned char* tag, unsigned long* taglen,int flag)
{
	int ret = 0;

	aes_config_action(AES_ENCRYPT);

	if (flag & AES_FLAG_START) {
		if (p_aes_dev->blen > 0) {
			/*send B buffer: with nonce/aad*/
			ret = aes_exec(flag | AES_FLAG_AAD,(unsigned char*)p_aes_dev->b_buf,NULL,p_aes_dev->blen);
			if (ret)
				return ret;
		}
	}
	/*send plain data*/
	if (len > 0)
		ret = aes_exec(flag | AES_FLAG_PLAIN_TEXT,pt,ct,len);

	/*read mac data*/
	if (flag & AES_FLAG_END) {
		if (*taglen == 0)
			return ret;
		if ((p_aes_dev->blen == 0) && (len == 0))
			aes_zero_exec();
		memcpy(tag,(char*)((u32)p_aes_dev->regs+AES_CTX_MAC_0),*taglen);
	}
	return ret;
}
int aes_gcm_dec(unsigned char *ct, unsigned char *pt,int len,
	unsigned char* tag, unsigned long* taglen,int flag)
{

	int ret = 0;

	aes_config_action(AES_DECRYPT);

	if (flag & AES_FLAG_START) {
		if (tag != NULL && *taglen != 0)
			aes_set_mac(tag);

		if (p_aes_dev->blen >0) {
			/*send B buffer: with nonce/aad*/
			ret = aes_exec(flag | AES_FLAG_AAD,(unsigned char*)p_aes_dev->b_buf,NULL,p_aes_dev->blen);
			if (ret)
				return ret;
		}
	}
	/*send plain data*/
	if (len>0)
		ret = aes_exec(AES_FLAG_PLAIN_TEXT,ct,pt,len);

	return ret;

}
void aes_gcm_done(void)
{
	if (p_aes_dev->b_buf)
		free(p_aes_dev->b_buf);
	p_aes_dev->b_buf = NULL;
}

int aes_cmac_start(int keymode,unsigned char* key,int keylen,int maclen,int plen)
{
	aes_config_key(keymode,keylen*8);
	aes_set_key(key);

	/*set mac len*/
	aes_set_mac_len(maclen);
	aes_config_algo(AES_ALGO_CMAC);

	/*store total plain text len*/
	p_aes_dev->ptlen = plen;
	return 0;
}
int aes_cmac_enc(unsigned char *pt,int len,
	unsigned char* tag, unsigned long* taglen,int flag)
{
	int ret = 0;

	aes_config_action(AES_ENCRYPT);
	if (len > 0)
		ret = aes_cmac_exec(flag,pt,NULL,len);

	/*read mac data*/
	if (flag & AES_FLAG_END) {
		if (*taglen == 0)
			return ret;
		if (len == 0)
			aes_zero_exec();
		memcpy(tag,(char*)((u32)p_aes_dev->regs+AES_CTX_MAC_0),*taglen);
	}
	return ret;
}
