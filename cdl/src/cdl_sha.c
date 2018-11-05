/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_sha.h"
#include "cdl_dmac.h"

struct brite_sha sha_dev;
struct brite_sha* p_sha_dev = &sha_dev;

static inline unsigned long brite_sha_read(struct brite_sha* priv,
	unsigned long offset)
{
	return __raw_readl((unsigned long)priv->regs + offset);
}


static inline void brite_sha_write(struct brite_sha* priv, unsigned long offset,
	unsigned long val)
{
	__raw_writel(val, (unsigned long)priv->regs + offset);
}

static int sha_wait_done(unsigned long timeout)
{
	unsigned long long timebase = get_timer(0);
	do {
		if (brite_sha_read(p_sha_dev, SHA_CMD) & SHA_DONE)
			return 0;
	} while (get_timer(timebase) < timeout);
	return -ETIMEDOUT;
}

void sha_init(unsigned long base,int burst_size)
{
	memset(p_sha_dev,0,sizeof(struct brite_sha));
	p_sha_dev->regs = (void*)base;

	p_sha_dev->burst_size = burst_size;

	/*disable interrupt and clear pending*/
	sha_disable_irq();
	sha_clear_irq();
}

void sha_enable_irq(void)
{
	brite_sha_write(p_sha_dev, SHA_IRQ_EN, SHA_IRQ_EN_BIT);
}

void sha_disable_irq(void)
{
	brite_sha_write(p_sha_dev, SHA_IRQ_EN, 0);
}
void sha_clear_irq(void)
{
	brite_sha_write(p_sha_dev, SHA_IRQ_CLR, SHA_IRQ_CLR_BIT);
}
int sha_get_int_status(void)
{
	return brite_sha_read(p_sha_dev,SHA_IRQ_STA);
}


static void sha_dma_transfer(const char* buf, int len)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[p_sha_dev->dmac_idx];
	int ret = 0;
	u32 enbl;

	/* config src */
	dw_dmac_channel_config(p_dma_tx,
		p_sha_dev->dma_ch_tx,
		0,
		WORD_WIDTH,
		p_sha_dev->burst_size,
		INCREASE,		
		MEM_TO_PERI,
		p_sha_dev->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx,
		p_sha_dev->dma_ch_tx,
		1,
		WORD_WIDTH,
		p_sha_dev->burst_size,
		FIXED,		
		MEM_TO_PERI,
		p_sha_dev->dma_req_tx);

	dw_dmac_channel_single_transfer(p_dma_tx,
		p_sha_dev->dma_ch_tx,
		(unsigned long) buf,
		(unsigned long)p_sha_dev->regs + SHA_DATA_IN,
		len >> (WORD_WIDTH),
		0);
}

static int sha_exec(int mode,char* buf,int len,char* out)
{
	int segment_size;
	u32 cfg;
	int off;
	int rest;
	int trans_len;
	int ret =0;
	u32 *icv = (u32*)(p_sha_dev->regs+SHA_ICV_00);
	struct dw_dmac* p_dma_tx = &dma_dev_list[p_sha_dev->dmac_idx];
	u32 *outptr = (u32*)out;

	/*configure dma burst size*/
	brite_sha_write(p_sha_dev, SHA_BURST_SIZE, p_sha_dev->burst_size);

	/*configure mode*/
	brite_sha_write(p_sha_dev, SHA_MODE_CFG, mode);

	if (mode == MODE_SHA256)
		segment_size = 64;
	else
		segment_size = 128;

	off = 0;
	rest = len;
	while (rest>0) {
		cfg = 0;
		if (off==0)
			cfg |= SHA_MSG_BEGIN;
		if (rest<= segment_size){
			cfg |= SHA_MSG_END;
		}
		trans_len = min(segment_size,rest);
		cfg |= trans_len<<12;

		brite_sha_write(p_sha_dev, SHA_MSG_CFG, cfg);
		brite_sha_write(p_sha_dev, SHA_MSG_TOTAL_BYTES, off+trans_len);

		sha_dma_transfer(buf+off,(trans_len + 3) & 0xfffffffc);

		brite_sha_write(p_sha_dev, SHA_CMD, SHA_GO);

		ret = dw_dmac_channel_wait_done(p_dma_tx, p_sha_dev->dma_ch_tx,
			DMAC_TIMEOUT);
		if (ret) {
			return ret;
		}
		ret = sha_wait_done(SHA_TIMEOUT);
		if (ret)
			return ret;
		off += trans_len;
		rest -= trans_len;
	}
	/*copy the result*/
	if (mode == MODE_SHA256) {
		icv = (u32*)(p_sha_dev->regs+SHA_ICV_04);
		rest = 8;
	}
	else
		rest = 12;
	for(off=0;off<rest;off++) {
		outptr[off] = icv[off];
	}
	return 0;
}
int sha256(char* buf,int len,char* out)
{
	return sha_exec(MODE_SHA256,buf,len,out);
}
int sha384(char* buf,int len,char* out)
{
	return sha_exec(MODE_SHA384,buf,len,out);
}

