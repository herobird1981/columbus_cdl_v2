/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_CRC_H__
#define __CDL_CRC_H__

/*register offset*/
#define CRC_IN		0x0
#define CRC_OUT		0x80000
#define CRC_CTL		0x80004
#define CRC_SEED		0x80008

struct brite_crc {
	void* regs;

	int dma_mode;
	int dmac_idx;
	int dma_ch_tx;
};

/*apis*/
void brite_crc16_init(unsigned long base);
unsigned short brite_crc16_xmodem(unsigned short seed,
	unsigned char *buf, int len);
unsigned short brite_crc16_ccitt(unsigned short seed,
	unsigned char *buf, int len);
#endif