/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_crc.h"
#include "cdl_dmac.h"

struct brite_crc crc_dev;
struct brite_crc *p_crc = &crc_dev;

void brite_crc16_init(unsigned long base)
{
	p_crc->regs = (void*)base;
}

static int brite_crc16_dma_transfer(const char* buf, int len)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[p_crc->dmac_idx];
	int ret = 0;

	/* config src */
	dw_dmac_channel_config(p_dma_tx, p_crc->dma_ch_tx, 0, WORD_WIDTH, BURST_16,
		INCREASE,  MEM_TO_MEM, 0);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, p_crc->dma_ch_tx, 1, WORD_WIDTH, BURST_16,
		FIXED,  MEM_TO_MEM, 0);

	ret = dw_dmac_channel_single_transfer_poll(p_dma_tx, p_crc->dma_ch_tx,
		(unsigned long) buf, (unsigned long)p_crc->regs + CRC_IN,
		len);

	return ret;
}
unsigned short brite_crc16_xmodem(unsigned short seed, unsigned char *buf, int len)
{
	int i;
	unsigned long val_in;
	unsigned long div,rest;

	writel(seed & 0xffff,(u32)p_crc->regs + CRC_SEED);
	writel(0,  (u32)p_crc->regs + CRC_CTL);

	rest = len % 4;
	div = (len - rest)/4;

	if (p_crc->dma_mode) {
		brite_crc16_dma_transfer(buf,div);
	} else {
		for( i = 0; i < len-rest; i+=4) {
			val_in = (buf[i+3]<<24)|(buf[i+2]<<16)|(buf[i+1]<<8)|buf[i];
			writel(val_in, (u32)crc_dev.regs + CRC_IN);
		}
	}
	/*CRC Calculation by byte*/
	for(i = 0; i < rest; i++){
		val_in = buf[len-rest+i];
		writeb(val_in, (u32)crc_dev.regs+CRC_IN);
	}

	return readl((u32)p_crc->regs + CRC_OUT);
}

unsigned short brite_crc16_ccitt(unsigned short seed, unsigned char *buf, int len)
{
	int i;
	unsigned long val_in;
	unsigned long div,rest;

	writel(seed & 0xffff,(u32)p_crc->regs + CRC_SEED);
	writel(0x1,(u32)p_crc->regs + CRC_CTL);

	rest = len % 4;
	div = (len - rest)/4;

	if (p_crc->dma_mode) {
		brite_crc16_dma_transfer(buf,div);
	} else {
		for( i = 0; i < len-rest; i+=4) {
			val_in = (buf[i+3]<<24)|(buf[i+2]<<16)|(buf[i+1]<<8)|buf[i];
			writel(val_in, (u32)crc_dev.regs + CRC_IN);
		}
	}
	/*CRC Calculation by byte*/
	for(i = 0; i < rest; i++){
		val_in = buf[len-rest+i];
		writeb(val_in, (u32)crc_dev.regs+CRC_IN);
	}
	return ((readl((u32)p_crc->regs + CRC_OUT) & 0xFFFF0000) >> 0x10);
}