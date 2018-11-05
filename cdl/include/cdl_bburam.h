/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_BBURAM_H__
#define __CDL_BBURAM_H__
#include "cdl_types.h"

/*register offset*/
#define BRAM_PEND_WR	(0x400)

struct brite_bram {
	void* regs;
};

/*APIs*/
void brite_bram_init(struct brite_bram *priv, u32 base);
void brite_bram_readbs(struct brite_bram *priv, u32 offset, u32 len, u8 * p_buf);
void brite_bram_writebs(struct brite_bram *priv, u32 offset, u32 len, u8 * p_buf);

#endif

