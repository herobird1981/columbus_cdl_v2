/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_bburam.h"

#define BRAM_TIMEOUT (1000) //ms

void brite_bram_init(struct brite_bram *priv, u32 base)
{
	priv->regs = (void*)base;
}

void brite_bram_readbs(struct brite_bram *priv, u32 offset, u32 len, u8 * p_buf)
{
	int i;
	for(i=0; i<len; i++)
		*(p_buf+i) = __raw_readb((u32)priv->regs + offset + i);
}

void brite_bram_writebs(struct brite_bram *priv, u32 offset, u32 len, u8 * p_buf)
{
	int i = 0;
	unsigned long long start = 0;

	for(i=0; i<len; i++)
	{
		start = get_timer(0);
		while(__raw_readb((u32)priv->regs + BRAM_PEND_WR) & 0x01) {
			if(get_timer(start) > BRAM_TIMEOUT) {
				info("brite_bram_writebs check i=%d BRAM_PEND_WR timeout\n", i);
				return ;
			}
		}
		__raw_writeb(*(p_buf+i), (u32)priv->regs + offset + i);
	}

	start = get_timer(0);
	while(__raw_readb((u32)priv->regs + BRAM_PEND_WR) & 0x01) {
		if(get_timer(start) > BRAM_TIMEOUT) {
			info("brite_bram_writebs check BRAM_PEND_WR timeout last\n");
			break;
		}
	}
}


