/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_dmac.h"
#include "memleak.h"
#include "cdl_sys.h"
#include "hw_interrupt.h"

struct dw_dmac dma_dev_list[3];

static inline unsigned long dw_dmac_read(struct dw_dmac* priv,
	unsigned long offset)
{
	return readl((unsigned long)priv->regs + offset);
}

static inline void dw_dmac_write(struct dw_dmac* priv, unsigned long offset,
	unsigned long val)
{
	writel(val, (unsigned long)priv->regs + offset);
}

static inline unsigned long dw_dmac_ch_read(struct dw_dmac* priv, int ch,
	unsigned long offset)
{
	return dw_dmac_read(priv, DMAC_REG_CH_START + ch * DMAC_REG_CH_SIZE + offset);
}

static inline void dw_dmac_ch_write(struct dw_dmac* priv, int ch, unsigned long offset,
	unsigned long val)
{
	dw_dmac_write(priv,	DMAC_REG_CH_START + ch * DMAC_REG_CH_SIZE + offset,	val);
}


static int dw_dmac_enable(struct dw_dmac* priv, int enable)
{
	dw_dmac_write(priv, DMAC_REG_DMACFG, (enable) ? 1 : 0);
	return 0;
}

/*check channel is idle,if not disable and clear the pending interrupt*/
int dw_dmac_channel_select(struct dw_dmac* priv, DW_DMAC_CH ch)
{
	int timeout = 500; /* ms */
	unsigned long long start = get_timer(0);
	while (dw_dmac_read(priv, DMAC_REG_CHEN) & (1 << ch)) {
		info("warning:ch %d still active, force stop it\n",ch);
		dw_dmac_write(priv, DMAC_REG_CHEN, (1 << (ch + 8)) | (0 << ch));
		if(get_timer(start) > timeout)
		{
			info("force dma stop error\n");
			return -1;
		}
	}

	/* mask all channel interrupt,clear pending interrupt */
	dw_dmac_set_ch_int_clear(priv, ch, DMAC_TFR);
	dw_dmac_set_ch_int_clear(priv, ch, DMAC_BLOCK);
	dw_dmac_set_ch_int_clear(priv, ch, DMAC_SRC_TRAN);
	dw_dmac_set_ch_int_clear(priv, ch, DMAC_DST_TRAN);
	dw_dmac_set_ch_int_clear(priv, ch, DMAC_ERR);

	return 0;
}

static void dw_dmac_channel_enable(struct dw_dmac* priv, DW_DMAC_CH ch,
	int enable)
{
	dw_dmac_write(priv, DMAC_REG_CHEN, (1 << (ch + 8)) |
		(((enable) ? 1 : 0) << ch));
}

static void dw_dmac_channel_config_hw(struct dw_dmac* priv, DW_DMAC_CH ch,
	unsigned long saddr, unsigned long daddr, unsigned long count,
	int int_en)
{
	unsigned long value;

	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_SAR,	dma_virt_to_phy(saddr));
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_DAR,	dma_virt_to_phy(daddr));

	/*configure CTL register*/
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CTL,
		(priv->ch_cfg[ch].flow_ctrl << 20) |
		(priv->ch_cfg[ch].src_burst_size << 14) |
		(priv->ch_cfg[ch].dst_burst_size << 11) |
		(priv->ch_cfg[ch].src_addr_update << 9) |
		(priv->ch_cfg[ch].dst_addr_update << 7) |
		(priv->ch_cfg[ch].src_trans_width << 4) |
		(priv->ch_cfg[ch].dst_trans_width << 1) |
		(int_en & 0x1));
	//clear DONE bits, write block transfer size
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CTL + 4,	count & 0xfff);

	/*configure CFG register*/
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CFG,
		DW_DMAC_CH_CFG_MAX_ABRST | DW_DMAC_CH_CFG_SRC_HS_POL |
		DW_DMAC_CH_CFG_DST_HS_POL | DW_DMAC_CH_CFG_NOT_SUSPEND |
		DW_DMAC_CH_CFG_CH0_PRIOR);
	value = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG +	4);
	value |= DW_DMAC_CH_CFG_FIFO_MODE_EN;
	/*hardware handshake interface*/
	value &= ~0x7f80;
	if (priv->ch_cfg[ch].flow_ctrl == MEM_TO_PERI) {
		value |= (priv->ch_cfg[ch].dst_per & 0xf) << 11;
	} else if (priv->ch_cfg[ch].flow_ctrl == PERI_TO_MEM) {
		value |= (priv->ch_cfg[ch].src_per & 0xf) << 7;
	}
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CFG + 4,	value);
}

static unsigned long dw_dmac_channel_update_addr(unsigned long addr,
	ADDRESS_UPDATE up, TRANSFRE_WIDTH width, unsigned long count)
{
	unsigned long ret_addr;

	if (up == INCREASE) {
		ret_addr = addr + count * (1 << width);
	} else if (up == DECREASE) {
		ret_addr = addr - count * (1 << width);
	} else {
		ret_addr = addr;
	}
	return ret_addr;
}

int dw_dmac_init(struct dw_dmac* priv, unsigned long base,int vector)
{
	priv->regs = (void*)base;
	priv->int_vector = vector;

	dw_dmac_enable(priv, 1);

	/* clear all interrupt status,and mask all ch interrupt */
	dw_dmac_write(priv, DMAC_REG_INT_MASK + (DMAC_TFR << 3), 0xff00);
	dw_dmac_write(priv, DMAC_REG_INT_MASK + (DMAC_BLOCK << 3), 0xff00);
	dw_dmac_write(priv, DMAC_REG_INT_MASK + (DMAC_SRC_TRAN << 3), 0xff00);
	dw_dmac_write(priv, DMAC_REG_INT_MASK + (DMAC_DST_TRAN << 3), 0xff00);
	dw_dmac_write(priv, DMAC_REG_INT_MASK + (DMAC_ERR << 3), 0xff00);
	dw_dmac_write(priv, DMAC_REG_INT_CLEAR + (DMAC_TFR << 3), 0xff);
	dw_dmac_write(priv, DMAC_REG_INT_CLEAR + (DMAC_BLOCK << 3), 0xff);
	dw_dmac_write(priv, DMAC_REG_INT_CLEAR + (DMAC_SRC_TRAN << 3), 0xff);
	dw_dmac_write(priv, DMAC_REG_INT_CLEAR + (DMAC_DST_TRAN << 3), 0xff);
	dw_dmac_write(priv, DMAC_REG_INT_CLEAR + (DMAC_ERR << 3), 0xff);

	return 0;
}

int dw_dmac_channel_config(struct dw_dmac* priv, DW_DMAC_CH ch, int is_dst,
	TRANSFRE_WIDTH width, BURST_SIZE burst, ADDRESS_UPDATE update,
	FLOW_CONTROL dir, unsigned char dev_per)
{
	/* prepare channel */
	dw_dmac_channel_select(priv, ch);

	if (!is_dst) {
		priv->ch_cfg[ch].src_trans_width = width;
		priv->ch_cfg[ch].src_burst_size = burst;
		priv->ch_cfg[ch].src_addr_update = update;
		priv->ch_cfg[ch].src_per = dev_per;
	} else {
		priv->ch_cfg[ch].dst_trans_width = width;
		priv->ch_cfg[ch].dst_burst_size = burst;
		priv->ch_cfg[ch].dst_addr_update = update;
		priv->ch_cfg[ch].dst_per = dev_per;
	}
	priv->ch_cfg[ch].flow_ctrl = dir;
	return 0;
}

/* dw_dmac_channel_single_transfer_poll
	polling mode, start single block dma transfer and wait transfer done
	when transfer size larger than max size one block support,
	loop single block transfer until complete
*/
int dw_dmac_channel_single_transfer_poll(struct dw_dmac* priv, DW_DMAC_CH ch,
	unsigned long saddr, unsigned long daddr, unsigned long count)
{
	unsigned long count_per_block;
	int ret = 0;

	/* prepare channel */
	dw_dmac_channel_select(priv, ch);

	while (count > 0) {
		if (count > DMAC_MAX_SIZE_PER_BLOCK) {
			count_per_block = DMAC_MAX_SIZE_PER_BLOCK;
		} else {
			count_per_block = count;
		}
		dw_dmac_channel_config_hw(priv, ch, saddr, daddr,
			count_per_block, 0);
		dw_dmac_channel_enable(priv, ch, 1);

		ret = dw_dmac_channel_wait_done(priv, ch, DMAC_TIMEOUT);
		if (ret == 0) {
			/* complete,continue next block */
			saddr = dw_dmac_channel_update_addr(saddr,
				priv->ch_cfg[ch].src_addr_update,
				priv->ch_cfg[ch].src_trans_width,
				count_per_block);
			daddr = dw_dmac_channel_update_addr(daddr,
				priv->ch_cfg[ch].dst_addr_update,
				priv->ch_cfg[ch].dst_trans_width,
				count_per_block);
			count -= count_per_block;
		} else {
			return -ETIMEDOUT;
		}
	}
	return 0;
}

/* dw_dmac_channel_single_transfer
	start single block dma transfer
	count must limit to not exceed the max ability of single block
*/
int dw_dmac_channel_single_transfer(struct dw_dmac* priv, DW_DMAC_CH ch,
	unsigned long saddr, unsigned long daddr, unsigned long count,
	int int_en)
{
	dw_dmac_channel_select(priv, ch);
	dw_dmac_channel_config_hw(priv, ch, saddr, daddr, count, int_en);
	dw_dmac_channel_enable(priv, ch, 1);
	return 0;
}

int dw_dmac_channel_wait_done(struct dw_dmac* priv, int ch,
	unsigned long timeout)
{
	unsigned long long timebase = get_timer(0);
	do {
		if (!(dw_dmac_read(priv, DMAC_REG_CHEN) & (1 << ch))) {
			/*should check transfer done and no err */
			if (dw_dmac_get_ch_raw_int_status(priv,ch,DMAC_TFR)) {
				if (!dw_dmac_get_ch_raw_int_status(priv,ch,DMAC_ERR))
					return 0;
				else
					return -EIO;
			} else {
				return -EIO;
			}
		}
	} while (get_timer(timebase) < timeout);
	return -ETIMEDOUT;
}

int dw_dmac_channel_check_done(struct dw_dmac* priv, int ch)
{
	if (!(dw_dmac_read(priv, DMAC_REG_CHEN) & (1 << ch))) {
		/*should check transfer done and no err */
		if (dw_dmac_get_ch_raw_int_status(priv,ch,DMAC_TFR)) {
			if (!dw_dmac_get_ch_raw_int_status(priv,ch,DMAC_ERR))
				return 1;
			else
				return 0;
		} else {
			return 0;
		}
	}

	return 0;
}

int dw_dmac_channel_get_transfered(struct dw_dmac* priv,int ch)
{
	int size;

	size = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CTL + 4);
	return (size &0xfff);
}

int dw_dmac_channel_abort(struct dw_dmac* priv,int ch)
{
	u32 cfg;
	u64 start;

	/*first set CFGx .CH_SUSP*/
	cfg = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG);
	cfg |= 1<<8;
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CFG, cfg);


	cfg = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG);
	start = get_timer(0);
	while (!(cfg & (1<<9))) {
		cfg = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG);
		if (get_timer(start) > (DMAC_TIMEOUT/16)) {
			info("wait fifo empty timeout\n");
			if (priv->regs == (void *)DMAC0_BASE) {
				prcm_reset(ARM_DMA0_RST);
				dw_dmac_init(&dma_dev_list[0], DMAC0_BASE,INT_DMA0);
			}
			else if (priv->regs == (void *)DMAC1_BASE) {
				prcm_reset(ARM_DMA1_RST);
				dw_dmac_init(&dma_dev_list[1], DMAC1_BASE,INT_DMA1);
			}
			else if (priv->regs == (void *)DMAC2_BASE) {
				prcm_reset(ARM_DMA2_RST);
				dw_dmac_init(&dma_dev_list[2], DMAC2_BASE,INT_DMA2);
			}
			else
				info("unsupported dma controller\n");

			break;
		}
	}
	/*stop ch*/
	dw_dmac_write(priv, DMAC_REG_CHEN, (1 << (ch + 8)) | (0 << ch));
	return 0;
}
/* dw_dmac_channel_lli_transfer_poll
	polling mode, start multi-block dma transfer and wait transfer done
	lli list should be configurated before this call,no int lli's int enabled
*/
int dw_dmac_channel_lli_transfer_poll(struct dw_dmac* priv, DW_DMAC_CH ch,
	void* lli_header)
{
	int ret = 0;

	/*configure and start dma*/
	dw_dmac_channel_lli_transfer(priv, ch, lli_header);

	/*wait dma done*/
	ret = dw_dmac_channel_wait_done(priv, ch, DMAC_TIMEOUT);
	return ret;
}
/* dw_dmac_channel_lli_transfer
	start multi-block dma transfer
	lli list should be configurated before this call
*/
int dw_dmac_channel_lli_transfer(struct dw_dmac* priv, DW_DMAC_CH ch,
	void* lli_header)
{
	unsigned long value;
	struct dw_dmac_lli_item* lli = (struct dw_dmac_lli_item*)lli_header;

	if(dw_dmac_channel_select(priv, ch))
		return -1;

	/*first LLI's addr*/
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_LLP,
		dma_virt_to_phy((unsigned long)lli_header) & (~0x3));
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_SAR,	lli->sar);
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_DAR,	lli->dar);
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CTL,	lli->ctl_lo);
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CTL + 4,	lli->ctl_hi);

	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CFG,
		DW_DMAC_CH_CFG_MAX_ABRST | DW_DMAC_CH_CFG_SRC_HS_POL |
		DW_DMAC_CH_CFG_DST_HS_POL | DW_DMAC_CH_CFG_NOT_SUSPEND |
		DW_DMAC_CH_CFG_CH0_PRIOR);

	value = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG +	4);
	/*hardware handshake interface*/
	value &= ~0x7f80;
	if (priv->ch_cfg[ch].flow_ctrl == MEM_TO_PERI) {
		value |= (priv->ch_cfg[ch].dst_per & 0xf) << 11;
	} else if (priv->ch_cfg[ch].flow_ctrl == PERI_TO_MEM) {
		value |= (priv->ch_cfg[ch].src_per & 0xf) << 7;
	}
	value |= DW_DMAC_CH_CFG_FIFO_MODE_EN;
	value |= DW_DMAC_CH_CFG_SS_UPD_EN | DW_DMAC_CH_CFG_DS_UPD_EN;
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CFG + 4,	value);

	/* dw_dmac_dump_status(priv, ch); */

	dw_dmac_channel_enable(priv, ch, 1);

	return 0;
}

unsigned long dw_dmac_get_ch_raw_int_status(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset)
{
	return (dw_dmac_read(priv, DMAC_REG_INT_RAW_STATUS + (offset << 3)) >>
		ch) & 0x1;
}
unsigned long dw_dmac_get_ch_int_status(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset)
{
	return (dw_dmac_read(priv, DMAC_REG_INT_STATUS + (offset << 3)) >> ch) &
		0x1;
}
unsigned long dw_dmac_get_ch_int_mask(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset)
{
	return (dw_dmac_read(priv, DMAC_REG_INT_MASK + (offset << 3)) >> ch) &
		0x1;
}
void dw_dmac_set_ch_int_mask(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset, unsigned long enable)
{
	int mask = (enable) ? 1 : 0;
	dw_dmac_write(priv, DMAC_REG_INT_MASK + (offset << 3),
		(mask << ch) | (1 << (8 + ch)));
}
void dw_dmac_set_ch_int_clear(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset)
{
	dw_dmac_write(priv, DMAC_REG_INT_CLEAR + (offset << 3), (1 << ch));
}
unsigned long dw_dmac_get_int_status(struct dw_dmac* priv)
{
	return dw_dmac_read(priv, DMA_REG_INT_STATUS_COMBINE);
}

unsigned long dw_dmac_config_lli_ctl(unsigned long ctl, int is_dst,
	TRANSFRE_WIDTH width, BURST_SIZE burst, ADDRESS_UPDATE update,
	FLOW_CONTROL dir, unsigned char dev_per)
{
	unsigned long ret = ctl;
	if (!is_dst) {
		ret |= (0 << 25) | (dir << 20)| (burst << 14) |
			(update << 9) | (width << 4);
	} else {
	  	ret |= (0 << 23) | (dir << 20)| (burst << 11) |
	  		(update << 7) | (width << 1);
	}
	return ret;
}

int dw_dmac_config_lli_item(struct dw_dmac_lli_item* prev, void* lli_item,
	unsigned long sar, unsigned long dar, unsigned long ctl,
	unsigned long block_ts, int int_en)
{
	struct dw_dmac_lli_item* lli = (struct dw_dmac_lli_item*)lli_item;

	/* check lli_item word aligned */
	if ((unsigned long)lli_item & 0x3) {
		debug("lli_item buffer should be word aligned\n");
		return 1;
	}

	lli->sar = dma_virt_to_phy(sar);
	lli->dar = dma_virt_to_phy(dar);
	lli->llp = 0;

	/* add or remove int enable bit */
	ctl &= ~0x1;
	ctl |= int_en ? 0x1 : 0;

	/* add llp_src_en/llp_dst_en */
	ctl |= (0x3 << 27);

	lli->ctl_lo = ctl;
	lli->ctl_hi = block_ts & 0xfff;

	if (prev) {
		prev->llp = dma_virt_to_phy((unsigned long)lli_item);
	}

	return 0;
}

static int dw_dmac_mb_allocate(struct dw_mb_state *state,char* src,char* dst,unsigned long ctl,int len,int int_en)
{
	int ret = 0;
	struct dw_mb_memory_list *lli_buffer = NULL;
	struct dw_mb_memory_list *lli_buffer_loop;
	struct dw_dmac_lli_item* lli_hdr = NULL;
	struct dw_dmac_lli_item* lli_prev = NULL;
	struct dw_dmac_lli_item* lli_curr;
	unsigned long bsrc,bdst;
	unsigned long blen;
	unsigned long count_per_block;
	int lli_count_used = 0;

	if (!state)
		return 1;

	lli_buffer = (struct dw_mb_memory_list *) malloc(sizeof(struct dw_mb_memory_list));
	if (!lli_buffer) {
		ret = 1;
		info("allocate memory for dma LLI err\n");
		goto end;
	}
	lli_hdr = (struct dw_dmac_lli_item *) malloc(((len/DMAC_MAX_SIZE_PER_BLOCK)+1)*sizeof(struct dw_dmac_lli_item));
	if (!lli_hdr) {
		ret = 1;
		info("allocate memory for dma LLI err\n");
		goto end;
	}

	/*store the memory allocated for lli_hdr*/
	lli_buffer->buf = lli_hdr;
	lli_buffer->next = NULL;

	lli_count_used = 0;
	lli_curr = lli_hdr;
	lli_prev = NULL;
	bsrc = (unsigned long)src;
	bdst = (unsigned long)dst;
	blen = len;
	while (blen > 0) {
		if (blen > DMAC_MAX_SIZE_PER_BLOCK) {
			count_per_block = DMAC_MAX_SIZE_PER_BLOCK;
		} else {
			count_per_block = blen;
		}
		ret = dw_dmac_config_lli_item(lli_prev,
			lli_curr,
			bsrc,
			bdst,
			ctl,
			count_per_block,
			0);
		if (ret) {
			info("configure lli err\n");
			ret = 1;
			goto end;
		}
		lli_count_used++;
		lli_prev = lli_curr;
		lli_curr += 1;
		bsrc = dw_dmac_channel_update_addr(bsrc,((ctl >>9) && 0x3),
			((ctl >>4) && 0x7),count_per_block);
		bdst = dw_dmac_channel_update_addr(bdst,((ctl >>7) && 0x3),
			((ctl >>1) && 0x7),count_per_block);
		blen -= count_per_block;
	}
	/*if enable interrupt, set int flag at last LLI*/
	if (int_en) {
		lli_prev->ctl_lo |= 1;
	}

	lli_buffer_loop = state->lli_buffer;
	if (lli_buffer_loop) {
		while (lli_buffer_loop->next)
			lli_buffer_loop = lli_buffer_loop->next;
		lli_buffer_loop->next = lli_buffer;
	}else {
		state->lli_buffer = lli_buffer;
	}
	lli_curr = state->lli_hdr;
	if (lli_curr) {
		/*found the tail of hdr*/
		while (lli_curr->llp)
			lli_curr = (struct dw_dmac_lli_item *)lli_curr->llp;
		lli_curr->llp = dma_virt_to_phy((unsigned long)lli_hdr);
	}else {
		state->lli_hdr = lli_hdr;
	}
	state->lli_count_used += lli_count_used;
	return 0;
end:
	if (ret) {
		if (lli_hdr)
			free(lli_hdr);
		if (lli_buffer)
			free(lli_buffer);
	}
	return ret;
}

#define BITS(x, s, r)	(((x)>>(s)) & ((1<<(r))-1))

void dw_dmac_dump_status(struct dw_dmac* dma, DW_DMAC_CH ch)
{
	int i = 0;
	unsigned long sar = 0;
	unsigned long dar = 0;
	unsigned long llp = 0;
	unsigned long ctl_lo = 0;
	unsigned long ctl_hi = 0;
	unsigned long sstat = 0;
	unsigned long dstat = 0;
	unsigned long sstatar = 0;
	unsigned long dstatar = 0;
	unsigned long cfg_lo = 0;
	unsigned long cfg_hi = 0;
	unsigned long sgr = 0;
	unsigned long dsr = 0;

	info("\n=== %s ch %d ===\n\n", __func__, ch);

	sar = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_SAR);
	info("SAR 0x%08x\n", sar);
	dar = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_DAR);
	info("DAR 0x%08x\n", dar);
	llp = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_LLP);
	info("LLP 0x%08x\n", llp);
	ctl_lo = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_CTL);
	ctl_hi = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_CTL + 4);
	info("CTL 0x%08x %08x\n", ctl_hi, ctl_lo);
	info("    DONE %x, BLOCK_TS %x, LLP_SRC_EN %x, LLP_DST_EN %x, SMS %x, DMS %x\n"
		 "    TT_FC %x, DST_SCATTER_EN %x, SRC_GATHER_EN %x, SRC_MSIZE %x, DST_MSIZE %x\n"
		 "    SINC %x, DINC %x, SRC_TR_WIDTH %x, DST_TR_WIDTH %x, INT_EN %x\n",
		 BITS(ctl_hi, 12, 1), BITS(ctl_hi, 0, 12), BITS(ctl_lo, 28, 1), BITS(ctl_lo, 27, 1), BITS(ctl_lo, 25, 2), BITS(ctl_lo, 23, 2),
		 BITS(ctl_lo, 20, 3), BITS(ctl_lo, 18, 1), BITS(ctl_lo, 17, 1), BITS(ctl_lo, 14, 3), BITS(ctl_lo, 11, 3),
		 BITS(ctl_lo, 9, 2), BITS(ctl_lo, 7, 2), BITS(ctl_lo, 4, 3), BITS(ctl_lo, 1, 3), BITS(ctl_lo, 0, 1));

#if 0 /* can't be read on the fly */
	sstat = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_SSTAT);
	info("SSTAT 0x%08x\n", sstat);
	dstat = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_DSTAT);
	info("DSTAT 0x%08x\n", dstat);
	sstatar = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_SSTATAR);
	info("SSTATAR 0x%08x\n", sstatar);
	dstatar = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_DSTATAR);
	info("DSTATAR 0x%08x\n", dstatar);
#endif
	cfg_lo = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_CFG);
	cfg_hi = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_CFG + 4);
	info("CFG 0x%08x %08x\n", cfg_hi, cfg_lo);
	info("    DST_PER %x, SRC_PER %x, SS_UPD_EN %x, DS_UPD_EN %x, PROTCTL %x, FIFO_MODE %x, FCMODE %x\n"
		 "    RELOAD_DST %x, RELOAD_SRC %x, MAX_ABRST %x, SRC_HS_POL %x, DST_HS_POL %x\n"
		 "    LOCK_B %x, LOCK_CH %x, LOCK_B_L %x, LOCK_CH_L %x, HS_SEL_SRC %x, HS_SEL_DST %x\n"
		 "    FIFO_EMPTY %x, CH_SUSP %x, CH_PRIOR %x\n",
		 BITS(cfg_hi, 11, 4), BITS(cfg_hi, 7, 4), BITS(cfg_hi, 6, 1), BITS(cfg_hi, 5, 1), BITS(cfg_hi, 2, 3), BITS(cfg_hi, 1, 1), BITS(cfg_hi, 0, 1),
		 BITS(cfg_lo, 31, 1), BITS(cfg_lo, 30, 1), BITS(cfg_lo, 20, 10), BITS(cfg_lo, 19, 1), BITS(cfg_lo, 18, 1),
		 BITS(cfg_lo, 17, 1), BITS(cfg_lo, 16, 1), BITS(cfg_lo, 14, 2), BITS(cfg_lo, 12, 2), BITS(cfg_lo, 11, 1), BITS(cfg_lo, 10, 1),
		 BITS(cfg_lo, 9, 1), BITS(cfg_lo, 8, 1), BITS(cfg_lo, 5, 3));
	sgr = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_SGR);
	info("SGR 0x%08x\n", sgr);
	dsr = dw_dmac_ch_read(dma, ch, DMAC_REG_CH_DSR);
	info("DSR 0x%08x\n", dsr);

	info("\n");
	info("DMACFG 0x%08x\n", dw_dmac_read(dma, DMAC_REG_DMACFG));
	info("CH_EN 0x%08x\n", dw_dmac_read(dma, DMAC_REG_CHEN));
	info("DMAID 0x%08x\n", dw_dmac_read(dma, DMAC_REG_DMAID));
	info("DMATEST 0x%08x\n", dw_dmac_read(dma, DMAC_REG_DMATEST));
	info("\n");

	for(i=0; i<6; i++)
		info("DMA_COMP_PARAMS_%d 0x%08x\n", 6-i,
				dw_dmac_read(dma, DMAC_REG_DMA_COMP_PARAMS_START + i * 8));
	info("DMA_COMP_ID 0x%08x\n", dw_dmac_read(dma, DMAC_REG_DMA_COMP_ID));
	info("\n");
}


/******************************************************************************/
//  Function:  uint8_t nfc_chk_st(E_DMA_PORT port, E_DMA_CH ch)
//
//  Parameters:
//		port			: dma port
// 						= DMA0
// 						= DMA1		
// 						= DMA2	
//
//		ch			: dma channel of a port
// 						= [CH0, CH7]
//
//  Return:
//		DMA_ST_DONE		: DMA check status ok.
//
//		DMA_ST_FAIL			: DMA check status fail
//
//		DMA_ST_TIME_OUT	: DMA check status timeout
//
//  Description:
//      DMA checks the interrupt status until the DMA completed.
//		
/*****************************************************************************/
void dump_dma_regs(int port, int ch) {
    debug("\n%s port %d ch %d\n", __func__, port, ch);
    debug("DMA Controller Regs:\n");
    debug("DMA_MASK_TFR_REG         0x%x\n", readl(DMA_MASK_TFR_REG(port)));
    debug("DMA_MASK_BLOCK_REG       0x%x\n", readl(DMA_MASK_BLOCK_REG(port)));
    debug("DMA_MASK_SRC_TRAN_REG    0x%x\n", readl(DMA_MASK_SRC_TRAN_REG(port)));
    debug("DMA_MASK_DST_TRAN_REG    0x%x\n", readl(DMA_MASK_DST_TRAN_REG(port)));
    debug("DMA_MASK_ERR_REG         0x%x\n", readl(DMA_MASK_ERR_REG(port)));

    debug("DMA Channel Regs:\n");
	
    // configure registers
    debug("DMA_SAR_REG              0x%x\n", readl(DMA_SAR_REG(port, ch)));
    debug("DMA_DAR_REG              0x%x\n", readl(DMA_DAR_REG(port, ch)));

    debug("DMA_CTL_REG              0x%x\n", readl(DMA_CTL_REG(port, ch)));
    debug("DMA_CTL_REG+4            0x%x\n", readl(DMA_CTL_REG(port, ch) + 4));

    debug("DMA_CFG_REG              0x%x\n", readl(DMA_CFG_REG(port, ch)));
    debug("DMA_CFG_REG+4            0x%x\n", readl(DMA_CFG_REG(port, ch) + 4));

    debug("DMA Controller Regs:\n");
    debug("DMA_DMACFG_REG           0x%x\n", readl(DMA_DMACFG_REG(port)));
    debug("DMA_CH_EN_REG            0x%x\n", readl(DMA_CH_EN_REG(port)));
}

#define DMA_TIMEOUT 10000 // 100ms(uini: 10us)
E_DMA_ST dma_chk_st(E_DMA_PORT port, E_DMA_CH ch)
{
	uint32_t time_out = 0;
	
#ifndef RESET_DMAC
	uint32_t reg_val;
#endif

	while (time_out++ < DMA_TIMEOUT) 
	{
		/* check status done*/
		if (readl(DMA_RAW_TFR_REG(port)) & (1 << ch))
		{			
			writel((1 << ch), DMA_CLEAR_TFR_REG(port));

			/* check error status*/
			if (readl(DMA_RAW_ERR_REG(port)) & (1 << ch))
			{
				writel((1 << ch), DMA_CLEAR_ERR_REG(port));
				return DMA_ST_FAIL;
			}

			return DMA_ST_DONE;
		}

		// delay 10us
		udelay(10);
	}
#ifdef TAPEOUT_V2
	
#else
#ifndef RESET_DMAC
	// 1. If software wishes to disable a channel prior to the DMA transfer completion, then it can set the 
	// CFGx.CH_SUSP bit to tell the DW_ahb_dmac to halt all transfers from the source peripheral. 
	// Therefore, the channel FIFO receives no new data.
	reg_val = readl(DMA_CFG_REG(port, ch));
	reg_val |= (1<<DMA_CH_SUSP_SHIFT);
	writel(reg_val, DMA_CFG_REG(port, ch));

	// 2. Software can now poll the CFGx.FIFO_EMPTY bit until it indicates that the channel FIFO is empty.
	// Spin here until g_dma_timeout
	for (time_out = 0; time_out < DMA_TIMEOUT; time_out++)
	{
		if ((readl(DMA_CFG_REG(port, ch)) & (1<<DMA_FIFO_EMPTY_SHIFT)) == (1<<DMA_FIFO_EMPTY_SHIFT))
		{
			break;
		}

		// delay 10us
		udelay(10);
	}
	
	// 3. The ChEnReg.CH_EN bit can then be cleared by software once the channel FIFO is empty.
	writel(DMA_CH_EN_VAL(ch, CH_DIS), DMA_CH_EN_REG(port));
#else
#warning sys_rst is a temporary solution until a reliable way to immediately cancel a channel transfer is found.
	sys_rst(1<<SYS_ARM_DMA0_RST_SHIFT);
#endif
#endif
	return DMA_ST_TIME_OUT;	
}

/*****************************************************************************/
//  Function:  dma_xfer_common(E_DMA_PORT port, E_DMA_CH ch, uint32_t dst_handshake, uint32_t src_handshake, uint32_t xfer_type,
//						uint32_t dst_addr_inc, uint32_t src_addr_inc,
//						uint32_t xfer_width, uint32_t burst_xfer_len,
//						uint32_t dst_hs_sel, uint32_t src_hs_sel,
//						uint32_t *p_dst, uint32_t *p_src, uint32_t size, E_DMA_CH_PRIOR prior)
//
//  Parameters:
//		port			: dma port
// 						= DMA0
//
//		ch				: dma channel of a port
// 						= [CH0, CH7]
//
//		dst_handshake/src_handshake [in]	: the handshake flag
//
//		dst_addr_inc/src_addr_inc [in]	: the source & destination address change mode
//
//		xfer_width [in]	: DMA transfer width
//
//      burst_xfer_len[in]	: DMA transfer burst size
//
//		dst_hs_sel/src_hs_sel [in]	: 
//		p_src/p_dst [in]	: the source & destination address
//
//		size			: dma size(byte aligned)
//		
//		prior			: the priority of a channel
//						= [CH_PRIOR_0, CH_PRIOR_7]
//
//  Return:
//		none
//
/*****************************************************************************/
void dma_xfer_common(E_DMA_PORT port, E_DMA_CH ch, uint32_t dst_handshake, uint32_t src_handshake, uint32_t xfer_type,
						uint32_t dst_addr_inc, uint32_t src_addr_inc,
						uint32_t xfer_width, uint32_t burst_xfer_len,
						uint32_t dst_hs_sel, uint32_t src_hs_sel,
						uint32_t *p_dst, uint32_t *p_src, uint32_t size, E_DMA_CH_PRIOR prior)
{
	uint32_t blk_ts = (size + (1<<xfer_width) - 1) >> xfer_width;
	int timeout = 500; /* ms */
	unsigned long long start = get_timer(0);

	/*check channel is idle,if not disable and clear the pending interrupt*/
	while (readl(DMA_CH_EN_REG(port)) & (1 << ch)) 
	{
		info("warning:ch %d still active, force stop it\n",ch);

		writel(((1 << (ch + 8)) | (0 << ch)), DMA_CH_EN_REG(port));
		if(get_timer(start) > timeout)
		{
			info("force dma stop error\n");
			return;
		}
	}

	/* masked the interrupt*/
	writel(readl(DMA_MASK_TFR_REG(port)) | DMA_MASK_VAL(ch, DMA_MASK_WR_EN, DMA_MASKED), DMA_MASK_TFR_REG(port));
	writel(readl(DMA_MASK_BLOCK_REG(port)) | DMA_MASK_VAL(ch, DMA_MASK_WR_EN, DMA_MASKED), DMA_MASK_BLOCK_REG(port));
	writel(readl(DMA_MASK_SRC_TRAN_REG(port)) | DMA_MASK_VAL(ch, DMA_MASK_WR_EN, DMA_MASKED), DMA_MASK_SRC_TRAN_REG(port));
	writel(readl(DMA_MASK_DST_TRAN_REG(port)) | DMA_MASK_VAL(ch, DMA_MASK_WR_EN, DMA_MASKED), DMA_MASK_DST_TRAN_REG(port));
	writel(readl(DMA_MASK_ERR_REG(port)) | DMA_MASK_VAL(ch, DMA_MASK_WR_EN, DMA_MASKED), DMA_MASK_ERR_REG(port));

	/* write 1 to clear the status*/
	writel((1 << ch), DMA_CLEAR_TFR_REG(port));
	writel((1 << ch), DMA_CLEAR_BLOCK_REG(port));
	writel((1 << ch), DMA_CLEAR_SRC_TRAN_REG(port));
	writel((1 << ch), DMA_CLEAR_DST_TRAN_REG(port));
	writel((1 << ch), DMA_CLEAR_ERR_REG(port));

	// configure registers
	writel((uint32_t)p_src, DMA_SAR_REG(port, ch));
	writel((uint32_t)p_dst, DMA_DAR_REG(port, ch));

	writel(DMA_CTL_L32BITS_VAL(DMA_INT_DIS,	xfer_width,
						   xfer_width, dst_addr_inc,
						   src_addr_inc, burst_xfer_len,
						   burst_xfer_len, GATHER_DIS,
						   SCATTER_DIS,	xfer_type,
						   LLP_DIS, LLP_DIS),
							DMA_CTL_REG(port, ch));
	writel(DMA_CTL_H32BITS_VAL(blk_ts, 0), DMA_CTL_REG(port, ch) + 4);

	writel(DMA_CFG_L32BITS_VAL(prior, dst_hs_sel, src_hs_sel, 0, 0), DMA_CFG_REG(port, ch));
	writel(DMA_CFG_H32BITS_VAL(src_handshake, dst_handshake), DMA_CFG_REG(port, ch) + 4);

	writel(DMA_CFG_VAL(DMA_EN), DMA_DMACFG_REG(port));
	writel(DMA_CH_EN_VAL(ch, CH_EN), DMA_CH_EN_REG(port));

	// dump_dma_regs(port, ch);
}


/*****************************************************************************/
//  Function:  void dma_qspi_copy_m2p(E_DMA_PORT port, E_DMA_CH ch, uint32_t *p_dst, uint32_t *p_src,
//                                    uint32_t size, E_DMA_CH_PRIOR prior)
//
//  Parameters:
//		port			: dma port
// 						= DMA0
// 						= DMA1		
// 						= DMA2	
//
//		ch			: dma channel of a port
// 						= [CH0, CH7]
//
//		p_dst [out]	: the destination address
//
//		p_src [in]		: the source address
//
//		size			: dma size(byte aligned)
//		
//		prior			: the priority of a channel
//						= [CH_PRIOR_0, CH_PRIOR_7]
//
//  Return:
//		none
//
//  Description:
//	   DMA data from memory to peripheral.
//      Configure a channel of a dma controller and start the dma transaction.	
//		
/*****************************************************************************/
void dma_qspi_copy_m2p(E_DMA_PORT port, E_DMA_CH ch, uint32_t *p_dst, uint32_t *p_src, 
		       uint32_t size, E_DMA_CH_PRIOR prior)
{	
	dma_xfer_common(port, ch, DMA_REQ_SPI0_TX, DMA_REQ_SPI0_TX, TT_M2P,
		SD_NO_CHANGE, SD_INC,
		WIDTH_32_BITS, MSIZE_4,
		HS_SEL_HW, HS_SEL_SW,
		p_dst, p_src, size, prior);
}

/*****************************************************************************/
//  Function:  void dma_qspi_copy_p2m(E_DMA_PORT port, E_DMA_CH ch, uint32_t *p_dst,
//                                    uint32_t *p_src, uint32_t size, E_DMA_CH_PRIOR prior)
//
//  Parameters:
//		port			: dma port
// 						= DMA0
// 						= DMA1		
// 						= DMA2	
//
//		ch			: dma channel of a port
// 						= [CH0, CH7]
//
//		p_dst [out]	: the destination address
//
//		p_src [in]		: the source address
//
//		size			: dma size(byte aligned)
//		
//		prior			: the priority of a channel
//						= [CH_PRIOR_0, CH_PRIOR_7]
//
//  Return:
//		none
//
//  Description:
//	   DMA data from peripheral to memory.
//      Configure a channel of a dma controller and start the dma transaction.	
//		
/*****************************************************************************/
void dma_qspi_copy_p2m(E_DMA_PORT port, E_DMA_CH ch, uint32_t *p_dst, uint32_t *p_src,
        uint32_t size, E_DMA_CH_PRIOR prior)
{	
	dma_xfer_common(port, ch, DMA_REQ_SPI0_RX, DMA_REQ_SPI0_RX, TT_P2M,
			SD_INC, SD_NO_CHANGE,
			WIDTH_32_BITS, MSIZE_4,
			HS_SEL_SW, HS_SEL_HW,
			p_dst, p_src, size, prior);
}

void dma_hs_config(struct dw_dmac* priv, E_DMA_CH ch, uint32_t dst_hs_sel, uint32_t src_hs_sel)
{
	uint32_t reg_val = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG);
	
	reg_val &= (~DMA_HS_SEL_MASK);
	reg_val |= DMA_HS_SEL_VAL(dst_hs_sel, src_hs_sel);
	
	dw_dmac_ch_write(priv, ch, DMAC_REG_CH_CFG,	reg_val);


	reg_val = dw_dmac_ch_read(priv, ch, DMAC_REG_CH_CFG);
	debug("DMAC_REG_CH_CFG: %x\n", reg_val);
}

void dma_hs_per_config(struct dw_dmac* priv, E_DMA_CH ch, uint32_t dst_per, uint32_t src_per)
{
	uint32_t reg_val = dw_dmac_ch_read(priv, ch, (DMAC_REG_CH_CFG+4));
	
	reg_val &= (~DMA_HS_PER_MASK);
	reg_val |= DMA_HS_PER_VAL(dst_per, src_per);
	reg_val |= 0x4000;
	
	dw_dmac_ch_write(priv, ch, (DMAC_REG_CH_CFG+4),	reg_val);

	reg_val = dw_dmac_ch_read(priv, ch, (DMAC_REG_CH_CFG+4));
	debug("DMAC_REG_CH_CFG h32: %x\n", reg_val);
}


void dw_dmac_channel_single_transfer_ext(struct dw_dmac* priv, DW_DMAC_CH ch,
	unsigned long saddr, unsigned long daddr, unsigned long count,
	int int_en)
{
	dw_dmac_channel_select(priv, ch);
	dw_dmac_channel_config_hw(priv, ch, saddr, daddr, count, int_en);

	dma_hs_config(priv, ch, HS_SEL_HW, HS_SEL_HW);
	dma_hs_per_config(priv, ch, 8, 8);	
	dw_dmac_channel_enable(priv, ch, 1);
}

