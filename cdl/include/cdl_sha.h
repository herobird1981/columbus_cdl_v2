/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/

#ifndef __CDL_SHA_H__
#define __CDL_SHA_H__

#define SHA_CMD			0x00
#define SHA_MODE_CFG		0x04
#define SHA_MSG_CFG		0x08


#define SHA_MSG_TOTAL_BYTES	0x18
#define SHA_DATA_IN		0x1C

#define SHA_ICV_00		0x20
#define SHA_ICV_01		0x24
#define SHA_ICV_02		0x28
#define SHA_ICV_03		0x2C
#define SHA_ICV_04		0x30
#define SHA_ICV_05		0x34
#define SHA_ICV_06		0x38
#define SHA_ICV_07		0x3C
#define SHA_ICV_08		0x40
#define SHA_ICV_09		0x44
#define SHA_ICV_10		0x48
#define SHA_ICV_11		0x4C

#define SHA_VERI_RESULT		0x50
#define SHA_IRQ_EN		0x54
#define SHA_IRQ_CLR		0x58
#define SHA_IRQ_STA		0x5C
#define SHA_BURST_SIZE		0x60

#define SHA_GO		(1<<0)
#define SHA_DONE	(1<<1)

#define SHA_ALG_MODE		(0xF<<0)

#define SHA_MSG_END		(1<<0)
#define SHA_MSG_BEGIN		(1<<1)
#define SHA_NUM_BYTES		(0x7FFF<<12)


#define TOTAL_BYTES 		(0xFFFFFF<<0)


#define SHA_VERI_FAIL		(1<<0)
#define SHA_VERI_VLD		(1<<1)

#define SHA_IRQ_EN_BIT		(1<<0)

#define SHA_IRQ_CLR_BIT		(1<<0)


#define SHA_IRQ_STA_BIT		(1<<0)

#define SHA_DMA_BURST_SIZE	(7<<0)

#define MODE_SHA256		1
#define MODE_SHA384		2

#define SHA_TIMEOUT	(500)

struct brite_sha {
	void* regs;

	int burst_size;

	int dmac_idx;
	int dma_ch_tx;
	int dma_req_tx;

	int int_vector;
};

/*apis*/
void sha_init(unsigned long base,int burst_size);
void sha_enable_irq(void);
void sha_disable_irq(void);
void sha_clear_irq(void);
int sha_get_int_status(void);

int sha256(char* buf,int len,char* out);
int sha384(char* buf,int len,char* out);
#endif


