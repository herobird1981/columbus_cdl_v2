/**********************************************************************************************************/
//
//  Filename :          qspi_nand_bbt.c
//
//  COPYRIGHT, 2016
//  The contents of this file is the sole proprietary property of, confidential
//  to, and copyrighted by Brite Company.  It may not be copied or reproduced
//  in any manner or form without the prior written consent of Brite Company.
//
//  Original Author:    Billy
//  Date created:       Feb 14, 2016
//
//  Description:
//      implement qspi nand bbt basic operation
//
//  Revision:
//  Date       		Author      	Description
//  02/14/17    	Billy   		Original
//
/*************************************************************************************************************/
#include "cdl_qspi_nand_bbt.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/


/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/
static uint8_t qspi_bbt_blk_chk(uint32_t blk);

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
/*****************************************************************************/
//  Function:  BOOL qspi_bbt_va_to_pa(bbt_t *p_bbt, uint32_t va, uint32_t *p_pa)
//
//  Parameters:
//		p_bbt [in]	: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//		va			: virtual address(block + page)
//
//		p_pa [out]	: physical address(block + page)
//
//  Return:
//		FALSE	: convert fail
//
//		TRUE	: convert ok
//		
//  Description:
//      Convert the virtual address(block + page) to physical address(block + page).
//
/*****************************************************************************/
BOOL qspi_bbt_va_to_pa(bbt_t *p_bbt, uint32_t va, uint32_t *p_pa)
{
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;
	uint16_t tmp_vb, vb, blk, page;
	uint16_t byte_idx, byte_ofst;

	vb = va >> p_qspi_para->blk_shift_bits;
	page = va & ((1 << p_qspi_para->blk_shift_bits) - 1);

	// va->pa
	tmp_vb = 0;
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		byte_idx = blk >> 2;
		byte_ofst = (blk & 0x3) << 1;

		if ((p_bbt->data[byte_idx] & (3 << byte_ofst)) == (BBT_TYPE_GOOD << byte_ofst))
		{
			if (tmp_vb++ == vb)
			{
				*p_pa = (blk << p_qspi_para->blk_shift_bits) + page;
				return TRUE;
			}
		}
	}

	*p_pa = 0xFFFFFFFF;
	return FALSE;
}

/*****************************************************************************/
//  Function:  void qspi_bbt_pa_to_va(bbt_t *p_bbt, uint32_t pa, uint32_t *p_va)
//
//  Parameters:
//		p_bbt [in]	: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//		pa			: physical address(block + page)
//
//		p_va [out]	: virtual address(block + page)
//
//  Return:
//		FALSE	: convert fail
//
//		TRUE	: convert ok
//		
//  Description:
//      Convert the physical address(block + page) to virtual address(block + page).
//
/*****************************************************************************/
BOOL qspi_bbt_pa_to_va(bbt_t *p_bbt, uint32_t pa, uint32_t *p_va)
{
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;
	uint16_t vb, pb, blk, page;
	uint16_t byte_idx, byte_ofst;

	pb = pa >> p_qspi_para->blk_shift_bits;
	page = pa & ((1 << p_qspi_para->blk_shift_bits) - 1);

	// Check the physical block range
	if (pb >= p_qspi_para->blks_per_lun)
	{
		*p_va = 0xFFFFFFFF;
		return FALSE;
	}

	// Check if the physical block is invalid
	byte_idx = pb >> 2;
	byte_ofst = (pb & 0x3) << 1;
	if ((p_bbt->data[byte_idx] & (3 << byte_ofst)) == (BBT_TYPE_BAD << byte_ofst))
	{
		*p_va = 0xFFFFFFFF;
		return FALSE;	
	}

	// pa->va
	vb = 0;
	for (blk = 0; blk <= pb; blk++)
	{
		byte_idx = blk >> 2;
		byte_ofst = (blk & 0x3) << 1;

		if ((p_bbt->data[byte_idx] & (3 << byte_ofst)) == (BBT_TYPE_GOOD << byte_ofst))	
		{
			vb++;
		}
	}

	*p_va = ((vb - 1) << p_qspi_para->blk_shift_bits) + page;
	return TRUE;
}

/*****************************************************************************/
//  Function:   static uint8_t qspi_bbt_blk_chk(uint32_t blk)
//
//  Parameters:
//		blk					: flash block address
//
//  Return:
// 		QSPI_ST_DONE		: valid block
//
//		QSPI_ST_FAIL			: invalid block
//
// 		QSPI_ST_TIME_OUT	: bad block checks timeout
//
//  Description:
//		Check the bad block marker to judge if it is a valid block.   
//
/*****************************************************************************/
static uint8_t qspi_bbt_blk_chk(uint32_t blk)
{
	uint8_t ret_val;
	uint32_t i, page[3];	
	uint8_t spare_data[4];
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;

	// check bad block mode
	page[0] = 0;
	page[1] = 1;
	page[2] = p_qspi_para->pages_per_blk - 1;

	for (i = 0; i < 3; i++)
	{
		ret_val = qspi_nand_spare_read(((blk<<p_qspi_para->blk_shift_bits)+page[i]), spare_data, 4);
		if (ret_val == QSPI_ST_DONE)
		{
			if ((spare_data[0] == 0) || (spare_data[1] == 0) || (spare_data[2] == 0) || (spare_data[3] == 0))	
			{
				return QSPI_ST_FAIL;
			}		
		}
		else if (ret_val == QSPI_ST_TIME_OUT) 
		{
			return QSPI_ST_TIME_OUT;
		}		
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:   uint8_t qspi_bbt_scan(bbt_t *p_bbt)
//
//  Parameters:
//		p_bbt [in]		: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//  Return:
//		QSPI_ST_DONE	: scan bbt ok
//
//		QSPI_ST_FAIL		: scan bbt fail
//
//		QSPI_ST_TIME_OUT	: scan bbt timeout
//
//  Description:
//		Scan all the blocks to get a bad block table(bbt).
//
/*****************************************************************************/
uint8_t qspi_bbt_scan(bbt_t *p_bbt)
{
	uint8_t ret_val;
	int32_t blk; 
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;
	uint16_t byte_idx, byte_ofst;
	
	// set all bbt data to be 0xFF
	mem_set8(p_bbt, 0xFF, sizeof(bbt_t));
	
	// scan all the blocks to create a bbt
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{	
		byte_idx = blk >> 2;
		byte_ofst = (blk & 0x3) << 1;

		p_bbt->data[byte_idx] &= (~(3 << byte_ofst)); 
		
		ret_val = qspi_bbt_blk_chk(blk);
		if (ret_val == QSPI_ST_FAIL)
		{
			// marked as bad block
			p_bbt->data[byte_idx] |= (BBT_TYPE_BAD << byte_ofst); 
		}
		else if (ret_val == QSPI_ST_DONE)
		{
			// marked as good block
			p_bbt->data[byte_idx] |= (BBT_TYPE_GOOD << byte_ofst); 
		}
		else if (ret_val == QSPI_ST_TIME_OUT)
		{
			return QSPI_ST_TIME_OUT;
		}		
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:   uint8_t qspi_bbt_write(bbt_t *p_bbt)
//
//  Parameters:
//		p_bbt [in]		: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//  Return:
//		QSPI_ST_DONE	: write bbt ok
//
//		QSPI_ST_FAIL		: write bbt fail
//
//		QSPI_ST_TIME_OUT	: write bbt timeout
//
//  Description:
//		Write the bbt in the last two valid blocks.
//
/*****************************************************************************/
uint8_t qspi_bbt_write(bbt_t *p_bbt)
{
	uint8_t ret_val;
	int32_t blk, i, addr; 
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;
	uint16_t byte_idx, byte_ofst;
	uint16_t bbt_blk[2]; // two blocks which is stored the bad block table(default the last valid block)  
						 // [0]: the last valid block
						 // [1]: the last - 1 valid block

	// reserve the last two valid blocks to store the bbt
	i = 0;
	for (blk = p_qspi_para->blks_per_chip - 1; blk >= 0; blk--)
	{
		byte_idx = blk >> 2;
		byte_ofst = (blk & 0x3) << 1;		

		if ((p_bbt->data[byte_idx] & (3 << byte_ofst)) != (BBT_TYPE_BAD << byte_ofst))
		{
			bbt_blk[i++] = blk;

			// marked as reserved type
			p_bbt->data[byte_idx] &= (~(3 << byte_ofst));
			p_bbt->data[byte_idx] |= (BBT_TYPE_REVD << byte_ofst); 			

			if (i >= 2)
			{
				break;
			}
		}
	}

	debug("bbt stored in blk:%x, %x\n", bbt_blk[0], bbt_blk[1]);
	
	// merge version in the BBT
	p_bbt->ver += 1;

	// set the bbt to the last two valid blocks
	for (i = 0; i < 2; i++)
	{
		addr = bbt_blk[i]<<p_qspi_para->blk_shift_bits;
		
		// erase the block
		ret_val = qspi_nand_blk_erase(addr);
		if (ret_val != QSPI_ST_DONE)
		{
			return ret_val;
		}

		// merge bbt string in the BBT
		if (i == 0)
		{
			p_bbt->str[0] = 'B';
			p_bbt->str[1] = 'b';
			p_bbt->str[2] = 't';
			p_bbt->str[3] = '0';
		}
		else
		{
			p_bbt->str[0] = '1';
			p_bbt->str[1] = 't';
			p_bbt->str[2] = 'b';
			p_bbt->str[3] = 'B';			
		}
		
		// write the bbt to the block
		ret_val = qspi_nand_page_write(addr, (uint8_t *)p_bbt, p_qspi_para->page_size);
		if (ret_val != QSPI_ST_DONE)
		{
			return ret_val;
		}		
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:   uint8_t qspi_bbt_read(bbt_t *p_bbt)
//
//  Parameters:
//		p_bbt [out]		: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//  Return:
//		QSPI_ST_DONE	: read bbt ok
//
//		QSPI_ST_FAIL		: read bbt fail
//
//		QSPI_ST_TIME_OUT	: read bbt timeout
//
//  Description:
//		Read the bbt from one of the last two valid blocks.  
//
/*****************************************************************************/
uint8_t qspi_bbt_read(bbt_t *p_bbt)
{
	uint8_t ret_val;
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;
	int32_t blk;

	// search the bbt stored in the last block of the flash
	for (blk = p_qspi_para->blks_per_chip - 1; blk >= 0; blk--)
	{		
		ret_val = qspi_nand_page_read((blk << p_qspi_para->blk_shift_bits), (uint8_t *)p_bbt, p_qspi_para->page_size);
		if (ret_val == QSPI_ST_DONE)
		{
			if (((p_bbt->str[0] == 'B') && (p_bbt->str[1] == 'b') && (p_bbt->str[2] == 't') && (p_bbt->str[3] == '0')) ||
				((p_bbt->str[0] == '1') && (p_bbt->str[1] == 't') && (p_bbt->str[2] == 'b') && (p_bbt->str[3] == 'B')))	
			{
				return QSPI_ST_DONE;
			}								
		}
		else if (ret_val == QSPI_ST_TIME_OUT)
		{
			return QSPI_ST_TIME_OUT;
		}
	}
		
	return QSPI_ST_FAIL;
}

/*****************************************************************************/
//  Function:   void qspi_bbt_blk_marked(bbt_t *p_bbt, uint32_t blk, E_BBT_TYPE type)
//
//  Parameters:
//		p_bbt [in, out]		: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//		blk				: the block
//
//		type				: block type
//
//  Return:
//		none
//
//  Description:
//		Insert the block to the bbt.
//
/*****************************************************************************/
void qspi_bbt_blk_marked(bbt_t *p_bbt, uint32_t blk, E_BBT_TYPE type)
{
	uint16_t byte_idx, byte_ofst;
	
	// insert the block to the bbt
	byte_idx = blk >> 2;
	byte_ofst = (blk & 0x3) << 1;
	
	// marked to be bad block
	p_bbt->data[byte_idx] &= (~(3 << byte_ofst)); 
	p_bbt->data[byte_idx] |= (type << byte_ofst); 
}

/*****************************************************************************/
//  Function:   E_BBT_TYPE qspi_bbt_blk_verify(bbt_t *p_bbt, uint32_t blk)
//
//  Parameters:
//		p_bbt [in]		: data buffer which is to store the bad block table(bbt)
//						  2 bits indicate a block type 
//						  	= BBT_TYPE_BAD
//						  	= BBT_TYPE_REVD
//							= BBT_TYPE_WORN
//							= BBT_TYPE_GOOD
//
//		blk				: the block
//
//  Return:
//		block type
//
//  Description:
//		Get the block type in the bbt.
//
/*****************************************************************************/
E_BBT_TYPE qspi_bbt_blk_verify(bbt_t *p_bbt, uint32_t blk)
{
	E_BBT_TYPE type;
	uint16_t byte_idx, byte_ofst;
	
	// insert the block to the bbt
	byte_idx = blk >> 2;
	byte_ofst = (blk & 0x3) << 1;
	
	// get the block type in the bbt
	type = (p_bbt->data[byte_idx] >> byte_ofst) & 3;
	
	return type; 
}


#if 0
uint8_t qspi_nand_bbt_test(void)
{
	uint8_t ret_val;
	uint32_t blk, pa, va;
	qspi_nand_para_t_ptr p_qspi_para = &g_qspi_nand_para;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;
	
	// nfc initialize
	if (!qspi_opera_begin())
	{
		return FALSE;
	}
	
	// erase all the blocks
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		ret_val = qspi_nand_blk_erase(blk<<p_qspi_para->blk_shift_bits);
		debug_1("blk:%x erasing, ret_val:%x\n", blk, ret_val);
	}

	// scan the bbt
	if (QSPI_ST_DONE != qspi_bbt_scan(p_bbt))
	{
		return FALSE;
	}

#if 1
	// insert bad block, just for testing
	//qspi_bbt_blk_marked(p_bbt, 0, BBT_TYPE_BAD);
	//qspi_bbt_blk_marked(p_bbt, 1, BBT_TYPE_BAD);
	//qspi_bbt_blk_marked(p_bbt, 2, BBT_TYPE_BAD);
	qspi_bbt_blk_marked(p_bbt, 3, BBT_TYPE_BAD);
	qspi_bbt_blk_marked(p_bbt, 4, BBT_TYPE_BAD);
	//qspi_bbt_blk_marked(p_bbt, (p_nfc_para->blks_per_chip - 1), BBT_TYPE_BAD);
	//qspi_bbt_blk_marked(p_bbt, (p_nfc_para->blks_per_chip - 2), BBT_TYPE_BAD);
#endif

	// write the bbt
	if (QSPI_ST_DONE != qspi_bbt_write(p_bbt))
	{
		return FALSE;
	}

	// read the bbt
	mem_set8(p_bbt, 0xFF, sizeof(bbt_t));
	
	if (QSPI_ST_DONE != qspi_bbt_read(p_bbt))
	{
		return FALSE;
	}

	mem_printf((uint32_t *)p_bbt, sizeof(bbt_t));

	// va->pa convert
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		va = blk << p_qspi_para->blk_shift_bits;
		qspi_bbt_va_to_pa(p_bbt, va, &pa);
		debug_1("blk:%x, va->pa:%x->%x\n", blk, va, pa);
	}

	// pa->va convert
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		pa = blk << p_qspi_para->blk_shift_bits;
		qspi_bbt_pa_to_va(p_bbt, pa, &va);
		debug_1("blk:%x, pa->va:%x->%x\n", blk, pa, va);
	}

#if 0
	// read bbt without ecc
	nfc_raw_data_read((0x7FF<<p_nfc_para->blk_shift_bits), (uint8_t *)p_bbt, (p_nfc_para->page_size + 64));
	mem_printf((uint32_t *)p_bbt, (p_nfc_para->page_size + 64));

	ret_val = nfc_spare_read((0x7FF<<p_nfc_para->blk_shift_bits), p_bbt, 4);
	mem_printf((uint32_t *)p_bbt, 16);
#endif

	debug_1("bbt testing end!");
	while (1)
	{}
	
	return 0;
}
#endif

