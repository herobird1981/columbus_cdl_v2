/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_DMAC_H__
#define __CDL_DMAC_H__
#include "cdl_dma_reg.h"

/* Register offsets */
/*offset in channel,total 8 channels*/
#define DMAC_REG_CH_START	(0x0)
#define DMAC_REG_CH_SIZE	(0x58)

#define DMAC_REG_CH_SAR		(0x0)
#define DMAC_REG_CH_DAR		(0x8)
#define DMAC_REG_CH_LLP		(0x10)
#define DMAC_REG_CH_CTL		(0x18)
#define DMAC_REG_CH_SSTAT	(0x20)
#define DMAC_REG_CH_DSTAT	(0x28)
#define DMAC_REG_CH_SSTATAR	(0x30)
#define DMAC_REG_CH_DSTATAR	(0x38)
#define DMAC_REG_CH_CFG		(0x40)
#define DMAC_REG_CH_SGR		(0x48)
#define DMAC_REG_CH_DSR		(0x50)

/* int raw status,status,mask,clear offset*/
#define DMAC_REG_INT_RAW_STATUS		(0x2C0)
#define DMAC_REG_INT_STATUS		(0x2E8)
#define DMAC_REG_INT_MASK		(0x310)
#define DMAC_REG_INT_CLEAR		(0x338)
#define DMA_REG_INT_STATUS_COMBINE	(0x360)

#define DMAC_REG_DMACFG		(0x398)
#define DMAC_REG_CHEN		(0x3A0)
#define DMAC_REG_DMAID		(0x3A8)
#define DMAC_REG_DMATEST	(0x3B0)

#define DMAC_REG_DMA_COMP_PARAMS_START	(0x3C8)
#define DMAC_REG_DMA_COMP_ID	(0x3F8)

/*interrupt register index*/
typedef enum {
	DMAC_TFR = 0,
	DMAC_BLOCK,
	DMAC_SRC_TRAN,
	DMAC_DST_TRAN,
	DMAC_ERR
}DW_INT_OFF;

/*channel cfg reg*/
#define DW_DMAC_CH_CFG_SS_UPD_EN	(1<<6)
#define DW_DMAC_CH_CFG_DS_UPD_EN	(1<<5)
#define DW_DMAC_CH_CFG_PROTCTL  	(1<<2)
#define DW_DMAC_CH_CFG_FIFO_MODE_EN	(1<<1)
#define DW_DMAC_CH_CFG_FIFO_MODE_DIS	(0<<1)
#define DW_DMAC_CH_CFG_MAX_ABRST	(0<<20)
#define DW_DMAC_CH_CFG_SRC_HS_POL	(0<<19)
#define DW_DMAC_CH_CFG_DST_HS_POL	(0<<18)
#define DW_DMAC_CH_CFG_HARD_SEL_SRC	(0<<11)
#define DW_DMAC_CH_CFG_SOFT_SEL_SRC	(1<<11)
#define DW_DMAC_CH_CFG_HARD_SEL_DST	(0<<10)
#define DW_DMAC_CH_CFG_SOFT_SEL_DST	(1<<10)
#define DW_DMAC_CH_CFG_NOT_SUSPEND	(0<<8)
#define DW_DMAC_CH_CFG_SUSPEND		(1<<8)
#define DW_DMAC_CH_CFG_CH0_PRIOR	(0<<5)

#define DW_DMAC_ENABLE	(1<<0)
#define DW_DMAC_DISABLE	(0<<0)
#define DW_DMAC_CHANNEL_ENABLE	(1<<0)

#define DMAC_MAX_SIZE_PER_BLOCK	(2048)
#define DMAC_TIMEOUT	(2000)

/*interrupt status*/
#define DW_DMAC_INT_TFR		(1<<0)
#define DW_DMAC_INT_BLOCK	(1<<1)
#define DW_DMAC_INT_SRCT	(1<<2)
#define DW_DMAC_INT_DSTT	(1<<3)
#define DW_DMAC_INT_ERR		(1<<4)


// DMA Port Number
typedef enum {
	DMA0            = 0,
	DMA1            = 1,
	DMA2            = 2,
	NUM_DMA_PORTS   = 3
} E_DMA_PORT;

// DMA Channel Number
typedef enum {
	CH0              = 0,
	CH1              = 1,
	CH2              = 2,
	CH3              = 3,
	CH4              = 4,
	CH5              = 5,
	CH6              = 6,
	CH7              = 7,
	NUM_DMA_CHANNELS = 8
} E_DMA_CH;

// DMA status
typedef enum {
	DMA_ST_DONE		= 0,
	DMA_ST_FAIL		= 1,
	DMA_ST_TIME_OUT	= 2,
	DMA_ST_NULL_PTR = 3,        // NULL Pointer
	DMA_ST_INV_PORT = 4,        // Invalid Port
	DMA_ST_INV_CHAN = 5,        // Invalid Channel
	DMA_ST_INV_PRIO = 6         // Invalid Priority
} E_DMA_ST;

// DMA max size(byte, data width is 32bits)
#define DMA_MAX_BYTE_SIZE	4095


typedef enum {
	DW_DMAC_CH0 = 0,
	DW_DMAC_CH1,
	DW_DMAC_CH2,
	DW_DMAC_CH3,
	DW_DMAC_CH4,
	DW_DMAC_CH5,
	DW_DMAC_CH6,
	DW_DMAC_CH7
}DW_DMAC_CH;

typedef enum {
	MEM_TO_MEM = 0,
	MEM_TO_PERI,
	PERI_TO_MEM,
	PERI_TO_PERI
}FLOW_CONTROL;

typedef enum {
	BYTE_WIDTH = 0,
	HWORD_WIDTH,
	WORD_WIDTH,
	TWO_WORD_WIDTH,
	FOUR_WORD_WIDTH,
	EIGHT_WORD_WIDTH
}TRANSFRE_WIDTH;

typedef enum {
	INCREASE = 0,
	DECREASE,
	FIXED
}ADDRESS_UPDATE;

typedef enum {
	BURST_1 = 0,
	BURST_4,
	BURST_8,
	BURST_16,
	BURST_32,
	BURST_64,
	BURST_128,
	BURST_256
}BURST_SIZE;

struct dw_dmac_ch_config {
	TRANSFRE_WIDTH src_trans_width;
	BURST_SIZE src_burst_size;
	ADDRESS_UPDATE src_addr_update;
	unsigned char src_per;

	TRANSFRE_WIDTH dst_trans_width;
	BURST_SIZE dst_burst_size;
	ADDRESS_UPDATE dst_addr_update;
	unsigned char dst_per;

	FLOW_CONTROL flow_ctrl;
};

struct dw_dmac_lli_item {
	unsigned long sar;
	unsigned long dar;
	unsigned long llp;
	unsigned long ctl_lo;
	unsigned long ctl_hi;
	unsigned long sstat;
	unsigned long dstat;
};

struct dw_dmac {
	void* regs;
	struct dw_dmac_ch_config ch_cfg[8];
	int int_vector;
	int dma_port;
};


struct dw_mb_memory_list {
	struct dw_mb_memory_list *next;
	void *buf;
};
struct dw_mb_state {
	struct dw_dmac_lli_item* lli_hdr;
	unsigned long lli_count_used;

	struct dw_mb_memory_list *lli_buffer;
};

/* dmac and data structure used global by other device*/
extern struct dw_dmac dma_dev_list[3];

/*apis*/
int dw_dmac_init(struct dw_dmac* priv, unsigned long base,int vector);
int dw_dmac_channel_config(struct dw_dmac* priv, DW_DMAC_CH ch, int src_dst,
	TRANSFRE_WIDTH width, BURST_SIZE burst, ADDRESS_UPDATE update,
	FLOW_CONTROL dir, unsigned char dev_per);
int dw_dmac_channel_single_transfer_poll(struct dw_dmac* priv, DW_DMAC_CH ch,
	unsigned long saddr, unsigned long daddr, unsigned long count);
int dw_dmac_channel_single_transfer(struct dw_dmac* priv, DW_DMAC_CH ch,
	unsigned long saddr, unsigned long daddr, unsigned long count,
	int int_en);
int dw_dmac_channel_wait_done(struct dw_dmac* priv, int ch,
	unsigned long timeout);
int dw_dmac_channel_check_done(struct dw_dmac* priv, int ch);
int dw_dmac_channel_get_transfered(struct dw_dmac* priv,int ch);

int dw_dmac_channel_lli_transfer(struct dw_dmac* priv, DW_DMAC_CH ch,
	void* lli_header);
int dw_dmac_channel_lli_transfer_poll(struct dw_dmac* priv, DW_DMAC_CH ch,
	void* lli_header);
unsigned long dw_dmac_config_lli_ctl(unsigned long ctl, int src_dst,
	TRANSFRE_WIDTH width, BURST_SIZE burst, ADDRESS_UPDATE update,
	FLOW_CONTROL dir, unsigned char dev_per);
int dw_dmac_config_lli_item(struct dw_dmac_lli_item* prev, void* lli_item,
	unsigned long sar, unsigned long dar, unsigned long ctl,
	unsigned long block_ts, int int_en);

int dw_dmac_channel_abort(struct dw_dmac* priv,int ch);

unsigned long dw_dmac_get_ch_raw_int_status(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset);
unsigned long dw_dmac_get_ch_int_status(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset);
unsigned long dw_dmac_get_ch_int_mask(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset);
void dw_dmac_set_ch_int_mask(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset, unsigned long enable);
void dw_dmac_set_ch_int_clear(struct dw_dmac* priv, DW_DMAC_CH ch,
	DW_INT_OFF offset);
unsigned long dw_dmac_get_int_status(struct dw_dmac* priv);

void dw_dmac_dump_status(struct dw_dmac* dma, DW_DMAC_CH ch);

E_DMA_ST dma_chk_st(E_DMA_PORT port, E_DMA_CH ch);

void dma_qspi_copy_m2p(E_DMA_PORT port, E_DMA_CH ch, uint32_t *p_dst, uint32_t *p_src, 
		       					uint32_t size, E_DMA_CH_PRIOR prior);

void dma_qspi_copy_p2m(E_DMA_PORT port, E_DMA_CH ch, uint32_t *p_dst, uint32_t *p_src,
        						uint32_t size, E_DMA_CH_PRIOR prior);

void dw_dmac_channel_single_transfer_ext(struct dw_dmac* priv, DW_DMAC_CH ch,
													unsigned long saddr, unsigned long daddr, unsigned long count, int int_en);

int dw_dmac_channel_select(struct dw_dmac* priv, DW_DMAC_CH ch);

#endif

