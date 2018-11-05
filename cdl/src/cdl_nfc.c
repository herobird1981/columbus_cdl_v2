/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "cdl_nfc.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
nfc_para_t g_nfc_para = {0};

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
/*****************************************************************************/
//  Function: void nfc_set_mode(E_NFC_MODE mode, E_NFC_CLK_DLY clk_delay)
//
//  Parameters:
//		mode: timing mode
//
//		clk_delay	: additional delay
//
//  Return:
//		NFC_ST_DONE	: NFC check status ok.
//
//		NFC_ST_TIME_OUT	: NFC check status timeout
//		
//  Description:
//      Set timing mode.
//
/*****************************************************************************/
uint8_t nfc_set_mode(E_NFC_MODE mode, E_NFC_CLK_DLY clk_delay)
{	
	uint8_t ret_val;
	uint8_t fea[4] = {0, 0, 0, 0};
	
	switch (mode)
	{
		case NFC_MODE_0: 
				REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(10, 8, clk_delay);
				REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(19, 39, 39, 23);
				REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(39, 7, 15);			
			break;
		case NFC_MODE_1: 
				REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(4, 4, clk_delay);
				REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(19, 19, 19, 15);
				REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(19, 3, 15);				
			break;
		case NFC_MODE_2: 
				REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(3, 3, clk_delay);
				REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(19, 19, 19, 15);
				REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(19, 3, 15);				
			break;
		case NFC_MODE_3: 
				REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(3, 1, clk_delay);
				REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(19, 19, 19, 11);
				REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(19, 3, 15);			
			break;
		case NFC_MODE_4: 
				REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(2, 1, clk_delay);
				REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(19, 13, 19, 11);
				REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(19, 3, 15);				
			break;
		case NFC_MODE_5:
		deault:
				REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(1, 1, clk_delay);
				REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(19, 13, 19, 11);
				REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(19, 3, 15);			
			break;
	}

	// get the timing mode
	if (NFC_ST_DONE != nfc_get_feature(FA_TIMING_MODE, fea))
	{
		return NFC_ST_TIME_OUT;
	}

	debug("current fea: %x\n", fea[0]);	

	// set the timing mode
	fea[0] = mode;
	if (NFC_ST_DONE != nfc_set_feature(FA_TIMING_MODE, fea))
	{
		return NFC_ST_TIME_OUT;
	}

	// get the timing mode
	fea[0] = 0;
	if (NFC_ST_DONE != nfc_get_feature(FA_TIMING_MODE, fea))
	{
		return NFC_ST_TIME_OUT;
	}

	debug("set fea: %x\n", fea[0]);

	return NFC_ST_DONE;
}

/*****************************************************************************/
//  Function:  void nfc_init(void)
//
//  Parameters:
//		none
//
//  Return:
//		none
//		
//  Description:
//      NFC initializes.
//
/*****************************************************************************/
void nfc_init(void)
{	
	// clear nfc structure
	mem_set8((uint8_t *)&g_nfc_para, 0x00, sizeof(nfc_para_t));
	
	// check the nfc timeout validity
	g_nfc_para.time_out = 10;
	g_nfc_para.time_out *= NFC_TIME_OUT;	
	
	// set timing mode1(20MHZ)
	REG32(NFC_TIMINGS_ASYN_REG) = NFC_TIMINGS_ASYN_VAL(2, 1, NFC_CLK_DLY_1);
	REG32(NFC_TIME_SEQ_0_REG) =  NFC_TIME_SEQ_0_VAL(50, 10, 10, 8);
	REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(10, 2, 10);

	REG32(NFC_MEM_CTRL_REG) = 0x300;
	REG32(NFC_STATUS_MASK_REG) = NFC_GET_ST_MASK_VAL(0x40, 0x01); 

	REG32(NFC_INT_STATUS_REG) = 0x0;
	REG32(NFC_ECC_STAT_REG) = 0x0;

	debug("NFC_TIMINGS_ASYN_REG:%x\n", REG32(NFC_TIMINGS_ASYN_REG));
	debug("NFC_TIME_SEQ_0_REG:%x\n", REG32(NFC_TIME_SEQ_0_REG));
	debug("NFC_TIME_SEQ_1_REG:%x\n", REG32(NFC_TIME_SEQ_1_REG));
	debug("NFC_STATUS_MASK_REG:%x\n", REG32(NFC_STATUS_MASK_REG));
}

/*****************************************************************************/
//  Function:  E_NFC_ST nfc_chk_st(void)
//
//  Parameters:
//		none
//
//  Return:
//		NFC_ST_DONE	: NFC check status ok.
//
//		NFC_ST_FAIL		: NFC check status fail
//
//		NFC_ST_TIME_OUT	: NFC check status timeout
//
//  Description:
//      NFC checks the interrupt status after issuing erase command without DMA.
//		
/*****************************************************************************/
E_NFC_ST nfc_chk_st(void)
{
	uint32_t reg_val;
	uint32_t mask = (1<<MEM0_RDY_INT_FL_SHIFT) | (1<<CMD_END_INT_FL_SHIFT);
	uint32_t time_out = 0;
	
	while (time_out++ < (g_nfc_para.time_out/NFC_CHK_STATUS_DELAY))
	{
		reg_val = REG32(NFC_INT_STATUS_REG);
		if ((reg_val & mask) == mask)
		{			
			if (reg_val & (1<<STAT_ERR_INT0_FL_SHIFT))
			{
				return NFC_ST_FAIL;
			}
			else
			{
				return NFC_ST_DONE;
			}
		}

		// delay 10us
		udelay(NFC_CHK_STATUS_DELAY);
	}

	return NFC_ST_TIME_OUT;
}

/*****************************************************************************/
//  Function:  E_NFC_ST nfc_chk_st_1(void)
//
//  Parameters:
//		none
//
//  Return:
//		NFC_ST_DONE	: NFC check status ok.
//
//		NFC_ST_TIME_OUT	: NFC check status timeout
//
//  Description:
//      NFC checks the interrupt status after issuing reset, read ID command without DMA.
//		
/*****************************************************************************/
E_NFC_ST nfc_chk_st_1(void)
{
	uint32_t reg_val;
	uint32_t mask = (1<<MEM0_RDY_INT_FL_SHIFT) | (1<<CMD_END_INT_FL_SHIFT);
	uint32_t time_out = 0;
	
	while (time_out++ < (g_nfc_para.time_out/NFC_CHK_STATUS_DELAY))
	{
		reg_val = REG32(NFC_INT_STATUS_REG);
		if ((reg_val & mask) == mask)
		{			
			return NFC_ST_DONE;
		}

		// delay 10us
		udelay(NFC_CHK_STATUS_DELAY);
	}

	return NFC_ST_TIME_OUT;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_chk_rd_dma_st(void)
//
//  Parameters:
//		none
//
//  Return:
//		NFC_ST_DONE	: flash reads page ok
//
//		NFC_ST_FAIL		: flash reads page fail(ecc uncorrect)
//
//		NFC_ST_TIME_OUT	: flash reads page timeout
//
//  Description:
//      NFC checks the interrupt status after issuing read command with DMA.
//		
/*****************************************************************************/
uint8_t nfc_chk_rd_dma_st(void)
{
	uint32_t reg_val;
	uint32_t mask = (1<<MEM0_RDY_INT_FL_SHIFT) | (1<<DMA_INT_FL_SHIFT) | (1<<CMD_END_INT_FL_SHIFT);
	uint32_t time_out = 0;

	while (time_out++ < (g_nfc_para.time_out/NFC_CHK_STATUS_DELAY))
	{
		reg_val = REG32(NFC_INT_STATUS_REG);
		if ((reg_val & mask) == mask)
		{		
			if (reg_val & (1<<ECC_INT0_FL_SHIFT))
			{
#ifdef EXTRA_DEBUG
				debug("\nnfc_chk_rd_dma_st() failure reg_val %x mask %x\n", reg_val, mask);
#endif
				return NFC_ST_FAIL;
			}
			else
			{
				return NFC_ST_DONE;
			}
		}

		// delay 10us
		udelay(NFC_CHK_STATUS_DELAY);
	}

	return NFC_ST_TIME_OUT;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_chk_wr_dma_st(void)
//
//  Parameters:
//		none
//
//  Return:
//		NFC_ST_DONE	: flash programs ok
//
//		NFC_ST_FAIL		: flash programs fail
//
//		NFC_ST_TIME_OUT	: flash programs timeout
//
//  Description:
//      NFC checks the interrupt status after issuing write command with DMA.
//		
/*****************************************************************************/
uint8_t nfc_chk_wr_dma_st(void)
{
	uint32_t reg_val;
	uint32_t mask = (1<<MEM0_RDY_INT_FL_SHIFT) | (1<<DMA_INT_FL_SHIFT) | (1<<CMD_END_INT_FL_SHIFT);
	uint32_t time_out = 0;

	while (time_out++ < (g_nfc_para.time_out/NFC_CHK_STATUS_DELAY))
	{
		reg_val = REG32(NFC_INT_STATUS_REG);
		if ((reg_val & mask) == mask)
		{	
			if (reg_val & (1<<STAT_ERR_INT0_FL_SHIFT))
			{
				return NFC_ST_FAIL;
			}
			else
			{
				return NFC_ST_DONE;
			}
		}

		// delay 10us
		udelay(NFC_CHK_STATUS_DELAY);		
	}

	return NFC_ST_TIME_OUT;
}

/*****************************************************************************/
//  Function:  E_NFC_ST nfc_reset(void)
//
//  Parameters:
//		none
//
//  Return:
//		E_NFC_ST Error code status
//
//  Description:
//      NFC issues the reset command to the nand flash.
//		
/*****************************************************************************/
E_NFC_ST nfc_reset(void)
{	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;
	
	REG32(NFC_MEM_CTRL_REG) = 0x300;

	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, IO_WIDTH_X08);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_0, INPUT_SEL_SIU, SEL_FIFO_MODULE, NFC_REST_CMD0, 0, 0);

	// check status		
	return nfc_chk_st_1();
}

/*****************************************************************************/
//  Function:  uint8_t nfc_read_id(uint8_t *p_id)
//
//  Parameters:
//		p_id [out]		: id buffer which is to be read from the flash
//
//  Return:
//		NFC_ST_DONE	: read id ok
//	
//		NFC_ST_TIME_OUT	: read id timeout
//
//  Description:
//      NFC reads the ID from the nand flash
//
/*****************************************************************************/
uint8_t nfc_read_id(uint8_t *p_id)
{
	uint32_t data;

	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;
	
	REG32(NFC_ADDR0_COL_REG) = 0x00;
	REG32(NFC_DATA_SIZE_REG) = 0x08;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, IO_WIDTH_X08);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_1, INPUT_SEL_SIU, SEL_FIFO_MODULE, NFC_READ_ID_CMD0, 0, 0);

	// check status
	if (NFC_ST_DONE != nfc_chk_st_1())
	{
		return NFC_ST_TIME_OUT;
	}

	data = REG32(NFC_FIFO_DATA_REG);
	p_id[0] = (data >> 0) & 0xFF;
	p_id[1] = (data >> 8) & 0xFF;
	p_id[2] = (data >> 16) & 0xFF;
	p_id[3] = (data >> 24) & 0xFF;

	data = REG32(NFC_FIFO_DATA_REG);
	p_id[4] = (data >> 0) & 0xFF;
	p_id[5] = (data >> 8) & 0xFF;
	p_id[6] = (data >> 16) & 0xFF;
	p_id[7] = (data >> 24) & 0xFF;	
	
	return NFC_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_get_feature(uint8_t fa, uint8_t *p_fea)
//
//  Parameters:
//		fa				: feature address
//							= FA_TIMING_MODE
//							= FA_PROG_OUT_DRV_STREN
//							= FA_PROG_RB_PD_STREN
//							= FA_ARRAY_OP_MODE
//
//		p_fea [out]		: feature buffer which is to be read from the flash
//
//  Return:
//		NFC_ST_DONE	: get feature ok
//	
//		NFC_ST_TIME_OUT	: get feature timeout
//
//  Description:
//      NFC gets the feature from the nand flash
//
/*****************************************************************************/
uint8_t nfc_get_feature(uint8_t fa, uint8_t *p_fea)
{
	uint32_t data;

	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;
	
	REG32(NFC_ADDR0_COL_REG) = fa;
	REG32(NFC_DATA_SIZE_REG) = 0x04;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, IO_WIDTH_X08);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_2, INPUT_SEL_SIU, SEL_FIFO_MODULE, NFC_GET_FEATURE_CMD0, 0, 0);

	// check status
	if (NFC_ST_DONE != nfc_chk_st_1())
	{
		return NFC_ST_TIME_OUT;
	}

	data = REG32(NFC_FIFO_DATA_REG);

	p_fea[0] = (data >> 0) & 0xFF;
	p_fea[1] = (data >> 8) & 0xFF;
	p_fea[2] = (data >> 16) & 0xFF;
	p_fea[3] = (data >> 24) & 0xFF;
	
	return NFC_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_set_feature(uint8_t fa, uint8_t *p_fea)
//
//  Parameters:
//		fa				: feature address
//							= FA_TIMING_MODE
//							= FA_PROG_OUT_DRV_STREN
//							= FA_PROG_RB_PD_STREN
//							= FA_ARRAY_OP_MODE
//
//		p_fea [out]		: feature buffer which is to be written to the flash
//
//  Return:
//		NFC_ST_DONE	: set feature ok
//	
//		NFC_ST_TIME_OUT	: set feature timeout
//
//  Description:
//      NFC sets the feature to the nand flash
//
/*****************************************************************************/
uint8_t nfc_set_feature(uint8_t fa, uint8_t *p_fea)
{
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;
	
	REG32(NFC_ADDR0_COL_REG) = fa;
	REG32(NFC_DATA_SIZE_REG) = 0x04;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, IO_WIDTH_X08);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_3, INPUT_SEL_SIU, SEL_FIFO_MODULE, NFC_SET_FEATURE_CMD0, 0, 0);

	REG32(NFC_FIFO_DATA_REG) = (p_fea[3] << 24) | (p_fea[2] << 16) | (p_fea[1] << 8) | (p_fea[0] << 0);
	
	// check status
	if (NFC_ST_DONE != nfc_chk_st_1())
	{
		return NFC_ST_TIME_OUT;
	}

	return NFC_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_set_array_op_mode(uint8_t op_mode)
//
//  Parameters:
//		op_mode	: array operation mode
//					= OP_MODE_NOR
//					= OP_MODE_OTP
//					= OP_MODE_OTP_PROT
//
//  Return:
//		NFC_ST_DONE	: set array mode ok
//	
//		NFC_ST_TIME_OUT	: set array mode timeout
//
//  Description:
//      NFC sets the array mode to the nand flash.
//
/*****************************************************************************/
uint8_t nfc_set_array_op_mode(uint8_t op_mode)
{
	uint8_t fea[4];

	fea[0] = op_mode;
	fea[1] = 0;
	fea[2] = 0;
	fea[3] = 0;

	return nfc_set_feature(FA_ARRAY_OP_MODE, fea);	
}

/*****************************************************************************/
//  Function:  uint8_t nfc_param_page_read(uint16_t col, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		col				: flash column address
//
//		p_data [out]		: data buffer which is to be read from the flash
//
//		size				: the size of the data buffer  
//
//  Return:
//		NFC_ST_DONE	: param page reads ok
//
//		NFC_ST_FAIL		: param page reads fail
//
//		NFC_ST_TIME_OUT	: param page reads timeout
//
//  Description:
//      NFC reads parameter page from the address of the flash without ecc.
//
/*****************************************************************************/
uint8_t nfc_param_page_read(uint16_t col, uint8_t *p_data, uint32_t size)
{
	uint8_t ret_val;

	// step 1: issue the read paramter command without DMA
	// step 1.1: configure dma register
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = 0;
	REG32(NFC_DMA_TLVL_REG) = 0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// step 1.2: issue nfc configuration
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ADDR0_COL_REG) = 0x0;
	REG32(NFC_ADDR0_ROW_REG) = 0;
	REG32(NFC_DATA_SIZE_REG) = 0;
	
	// step 1.3: issue read parameter command without DMA
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, IO_WIDTH_X08);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_2, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_PARA_PAGE_CMD0, 0, 0);

	// step 1.4: check status
	ret_val = nfc_chk_st_1();
	if (ret_val != NFC_ST_DONE)
	{
		return ret_val;
	}

	// step 2: issue the change column address and DMA
	// step 2.1: configure dma register
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// step 2.2: nfc configuration
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ADDR0_COL_REG) = col;
	REG32(NFC_ADDR0_ROW_REG) = 0;
	REG32(NFC_DATA_SIZE_REG) = size;

	// step 2.3: issue the change column address and DMA 
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, IO_WIDTH_X08);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_6, INPUT_SEL_DMA, SEL_FIFO_MODULE, 0x05, 0x00, 0xE0);
	
	// step 2.4: check dma status
	ret_val = nfc_chk_rd_dma_st();
	if (ret_val != NFC_ST_DONE)
	{
		return ret_val;
	}

	return ret_val;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_blk_erase(uint32_t addr)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//  Return:
//		NFC_ST_DONE	: block erases ok
//
//		NFC_ST_FAIL		: block erases fail
//		
//		NFC_ST_TIME_OUT	: block erases timeout
//
//  Description:
//      NFC erases a block of the flash.
//		
/*****************************************************************************/
uint8_t nfc_blk_erase(uint32_t addr)
{	
	E_NFC_ST ret_val;
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;
	
	
	REG32(NFC_ADDR0_COL_REG) = 0x0;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_14, INPUT_SEL_SIU, SEL_FIFO_MODULE, NFC_ERASE_BLK_CMD0, NFC_ERASE_BLK_CMD1_3, 0);
	
	// check status
	return nfc_chk_st();
}

/*****************************************************************************/
//  Function:  uint8_t nfc_raw_data_write(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [in]		: data buffer which is to be writen to the flash
//
//		size				: the size of the data buffer 
//
//  Return:
//		NFC_ST_DONE	: page writes ok
//
//		NFC_ST_FAIL		: page writes  fail
//	
//		NFC_ST_TIME_OUT	: page writes timeout
//
//  Description:
//      NFC writes data to the address of the flash without ecc.
//
/*****************************************************************************/
uint8_t nfc_raw_data_write(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	// configure dma
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ADDR0_COL_REG) = 0;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	REG32(NFC_DATA_SIZE_REG) = size;

	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_12, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_PROG_PAGE_CMD0, NFC_PROG_PAGE_CMD1_3, 0);

	// check dma status
	if (NFC_ST_DONE != nfc_chk_wr_dma_st())
	{
		return NFC_ST_TIME_OUT;
	}
	
	return NFC_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [out]		: data buffer which is to be read from the flash
//
//		size				: the size of the data buffer  
//
//  Return:
//		NFC_ST_DONE	: page reads ok
//
//		NFC_ST_FAIL		: page reads fail
//
//		NFC_ST_TIME_OUT	: page reads timeout
//
//  Description:
//      NFC reads data from the address of the flash without ecc.
//
/*****************************************************************************/
uint8_t nfc_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	// configure dma
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ADDR0_COL_REG) = 0;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	REG32(NFC_DATA_SIZE_REG) = size;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_10, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_READ_PAGE_CMD0, 0, NFC_READ_PAGE_CMD2);

	// check dma status
	if (NFC_ST_DONE != nfc_chk_rd_dma_st())
	{
		return NFC_ST_TIME_OUT;
	}

	return NFC_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_page_write(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [in]		: data buffer which is to be writen to the flash
//
//		size				: the size of the data buffer
//
//  Return:
//		NFC_ST_DONE	: page writes ok
//
//		NFC_ST_FAIL		: page writes  fail
//	
//		NFC_ST_TIME_OUT	: page writes timeout
//
//  Description:
//      NFC writes data to the address of the flash with ecc.
//
/*****************************************************************************/
uint8_t nfc_page_write(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	uint8_t ret_val;
	uint32_t ecc_ofst =(g_nfc_para.bus_size == IO_WIDTH_X08) ? g_nfc_para.page_size : (g_nfc_para.page_size >> 1);
	
	// configure dma
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0x0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ECC_CTRL_REG) = NFC_GET_ECC_CTRL_VAL(g_nfc_para.bch_mode, 32, ECC_UNC);
	REG32(NFC_ECC_OFFSET_REG) = ecc_ofst;
	
	REG32(NFC_ADDR0_COL_REG) = 0x0;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	REG32(NFC_DATA_SIZE_REG) = size;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_EN, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_12, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_PROG_PAGE_CMD0, NFC_PROG_PAGE_CMD1_3, 0);

	// check status
	ret_val = nfc_chk_wr_dma_st();
	
	return ret_val;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_page_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [out]		: data buffer which is to be read from the flash
//
//		size				: the size of the data buffer 
//
//  Return:
//		NFC_ST_DONE	: page reads ok
//
//		NFC_ST_FAIL		: page reads fail
//
//		NFC_ST_TIME_OUT	: page reads timeout
//
//  Description:
//      NFC reads data from the address of the flash with ecc.
//
/*****************************************************************************/
uint8_t nfc_page_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	uint8_t ret_val;
	uint32_t ecc_ofst =(g_nfc_para.bus_size == IO_WIDTH_X08) ? g_nfc_para.page_size : (g_nfc_para.page_size >> 1);

	// configure dma
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0x0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ECC_CTRL_REG) = NFC_GET_ECC_CTRL_VAL(g_nfc_para.bch_mode, 32, ECC_UNC);
	REG32(NFC_ECC_OFFSET_REG) = ecc_ofst;
	
	// Column address is always 0
	REG32(NFC_ADDR0_COL_REG) = 0x0;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	REG32(NFC_DATA_SIZE_REG) = size;

	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_EN, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_10, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_READ_PAGE_CMD0, 0, NFC_READ_PAGE_CMD2);

	// check status
	ret_val = nfc_chk_rd_dma_st();
	
	return ret_val;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_otp_page_write(uint32_t otp_page, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		otp_page			: otp page
//
//		p_data [in]		: data buffer which is to be writen to the flash
//
//		size				: the size of the data buffer 
//
//  Return:
//		NFC_ST_DONE	: otp page writes ok
//
//		NFC_ST_FAIL		: otp page writes  fail
//	
//		NFC_ST_TIME_OUT	: otp page writes timeout
//
//  Description:
//      NFC writes data to the otp page of the flash with ecc.
//
/*****************************************************************************/
uint8_t nfc_otp_page_write(uint32_t otp_page, uint8_t *p_data, uint32_t size)
{
	uint8_t ret_val;

	if (NFC_ST_DONE != nfc_set_array_op_mode(OP_MODE_OTP))
	{
		return NFC_ST_TIME_OUT;
	}

	ret_val = nfc_page_write(otp_page, p_data, size);

	if (NFC_ST_DONE != nfc_set_array_op_mode(OP_MODE_NOR))
	{
		return NFC_ST_TIME_OUT;
	}
	
	return ret_val;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_otp_page_read(uint32_t otp_page, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		otp_page			: otp page
//
//		p_data [out]		: data buffer which is to be read from the flash
//
//		size			: the size of the data buffer 
//
//  Return:
//		NFC_ST_DONE	: otp page reads ok
//
//		NFC_ST_FAIL		: otp page reads fail
//
//		NFC_ST_TIME_OUT	: otp page reads timeout
//
//  Description:
//      NFC reads data from the otp page of the flash with ecc.
//
/*****************************************************************************/
uint8_t nfc_otp_page_read(uint32_t otp_page, uint8_t *p_data, uint32_t size)
{
	uint8_t ret_val;

	if (NFC_ST_DONE != nfc_set_array_op_mode(OP_MODE_OTP))
	{
		return NFC_ST_TIME_OUT;
	}

	ret_val = nfc_page_read(otp_page, p_data, size);

	if (NFC_ST_DONE != nfc_set_array_op_mode(OP_MODE_NOR))
	{
		return NFC_ST_TIME_OUT;
	}
	
	return ret_val;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_otp_protect(uint32_t otp_page)
//
//  Parameters:
//		otp_page			: otp page
//
//  Return:
//		NFC_ST_DONE	: set otp page write protect mode ok
//
//		NFC_ST_FAIL		: set otp page write  protect mode fail
//	
//		NFC_ST_TIME_OUT	: set otp page write protect mode timeout
//
//  Description:
//      NFC set otp protect mode.
//
/*****************************************************************************/
uint8_t nfc_otp_protect(uint32_t otp_page)
{
	uint8_t ret_val;
	
	// set feature to otp protect mode
	if (NFC_ST_DONE != nfc_set_array_op_mode(OP_MODE_OTP_PROT))
	{
		return NFC_ST_TIME_OUT;
	}
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;
	
	REG32(NFC_ADDR0_COL_REG) = 0x0;
	REG32(NFC_ADDR0_ROW_REG) = otp_page;
	REG32(NFC_DATA_SIZE_REG) = 0x01;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_12, INPUT_SEL_SIU, SEL_FIFO_MODULE, NFC_PROG_PAGE_CMD0, NFC_PROG_PAGE_CMD1_3, 0);

	// fixed data to be 0
	REG32(NFC_FIFO_DATA_REG) = 0;
	
	// check status
	ret_val = nfc_chk_st_1();

	// set feature to otp normal mode
	if (NFC_ST_DONE != nfc_set_array_op_mode(OP_MODE_NOR))
	{
		return NFC_ST_TIME_OUT;
	}
	
	return ret_val;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_spare_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [out]		: data buffer which is to be read from the flash
//
//		size				: the size of the data buffer 
//
//  Return:
//		NFC_ST_DONE	: Read spare data ok
//
//		NFC_ST_FAIL		: Read spare data fail
//
//		NFC_ST_TIME_OUT	: page reads timeout
//
//  Description:
//      NFC reads spare data from the address of the flash.
//
/*****************************************************************************/
uint8_t nfc_spare_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	uint32_t col_ofst =(g_nfc_para.bus_size == IO_WIDTH_X08) ? g_nfc_para.page_size : (g_nfc_para.page_size >> 1);
	
	// configure dma
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);

	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ADDR0_COL_REG) = col_ofst;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	REG32(NFC_DATA_SIZE_REG) = size;

	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_DIS, ECC_DIS, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_10, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_READ_PAGE_CMD0, 0, NFC_READ_PAGE_CMD2);

	// check dma status
	if (NFC_ST_DONE != nfc_chk_rd_dma_st())
	{
		return NFC_ST_TIME_OUT;
	}

	return NFC_ST_DONE;	
}

/*****************************************************************************/
//  Function:  BOOL nfc_param_init(uint8_t *p_data, nfc_para_t *p_para)
//
//  Parameters:
//		p_data [in]	: data buffer which is to store the paramete page from the flash
//	
//		p_para [out]	: data buffer which is to store the nfc parameter  
//
//  Return:
//		FALSE		: the onfi parameter is not valid
//
//		TRUE		: the onfi parameter is valid
//
//  Description:
//      Initialize the nfc parameter upon the parameter page from the flash.
//
/*****************************************************************************/
BOOL nfc_param_init(uint8_t *p_data, nfc_para_t *p_para)
{	
	uint16_t col, crc;
	uint8_t ecc_blk_size; // fixed to 1KB data ECC block size
	uint8_t i;
	onfi_t *p_onfi = (onfi_t *)p_data;

	// Orlando This is not in the flow diagram, but it would be
	// a good idea to verify the integrity CRC
	// of the ONFI Parameters page (offset 254 and 255).
	// In case of failure there are redundant parameter pages
	// beginning at offset 256 and 512 that could be used.
	for (col = 0, i = 0; i < ONFI_COPY_NUM; i++, col += ONFI_SIZE)
	{ 		
		// read parameter page from the flash
		if (NFC_ST_DONE != nfc_param_page_read(col, p_data, ONFI_SIZE))
		{
			continue;
		}
		
		crc = get_16_bits_val(p_onfi->crc);
		if (crc == onfi_crc16_cpu_calc(p_data, (ONFI_SIZE - 2)))
		{
			break;
		}
	}

	if (i >= ONFI_COPY_NUM)
	{
		return FALSE;
	}

	// check the signature of the parameter page
	if (p_onfi->sig != ONFI_MARKER)
	{
		return FALSE;
	}
	
	// Initialize the nfc para from onfi para
	p_para->bus_size = p_onfi->fea_sup & 0x1;
	p_para->page_size = get_32_bits_val(p_onfi->bytes_per_page);
	p_para->spare_size = get_16_bits_val(p_onfi->bytes_per_spare);
	p_para->pages_per_blk = get_32_bits_val(p_onfi->pages_per_blk);
	p_para->blks_per_lun = get_32_bits_val(p_onfi->blks_per_lun);
	p_para->luns_per_chip = p_onfi->num_luns;
	p_para->timing = get_16_bits_val(p_onfi->timing_mode);

	// Orlando Since the ECC will be handled by the NFC and NOT
	// by the NAND chip, shouldn't ECC be determined by
	// p_onfi->bytes_per_spare instead of p_onfi->bits_ecc?
	// We want to have the best ECC we can get!

	// ecc data block size
	ecc_blk_size = p_para->spare_size / PAGE_SIZE_IN_KBYTES(p_para->page_size);
	if( ecc_blk_size >= MIN_SYC_SIZE4_32BITS_ECC)
		p_para->bch_mode = ECC_CAP_32_BITS;
	else if(ecc_blk_size >= MIN_SYC_SIZE4_24BITS_ECC)
		p_para->bch_mode = ECC_CAP_24_BITS;
	else if(ecc_blk_size >= MIN_SYC_SIZE4_16BITS_ECC)
		p_para->bch_mode = ECC_CAP_16_BITS;
	else if(ecc_blk_size >= MIN_SYC_SIZE4_8BITS_ECC)
		p_para->bch_mode = ECC_CAP_8_BITS;
	else if(ecc_blk_size >= MIN_SYC_SIZE4_4BITS_ECC)
		p_para->bch_mode = ECC_CAP_4_BITS;
	else // (ecc_blk_size >= MIN_SYC_SIZE4_2BITS_ECC)
		p_para->bch_mode = ECC_CAP_2_BITS;

	// calculate upon the para
	p_para->ecc_thresh = p_para->bch_mode;
	p_para->blk_shift_bits = power(p_para->pages_per_blk);
	p_para->blks_per_chip = p_para->luns_per_chip * p_para->blks_per_lun;

	debug("bus_size: 0x%x\n", p_para->bus_size);
	debug("bch_mode: 0x%x\n", p_para->bch_mode);
	debug("page_size: 0x%x\n", p_para->page_size);
	debug("spare_size: 0x%x\n", p_para->spare_size);
	debug("pages_per_blk: 0x%x\n", p_para->pages_per_blk);
	debug("blks_per_lun: 0x%x\n", p_para->blks_per_lun);
	debug("luns_per_chip: 0x%x\n", p_para->luns_per_chip);
	debug("blks_per_chip: 0x%x\n", p_para->blks_per_chip);
	debug("timing: 0x%x\n", p_para->timing);

	debug("ecc_thresh: 0x%x\n", p_para->ecc_thresh);
	debug("blk_shift_bits: 0x%x\n", p_para->blk_shift_bits);

	return TRUE;
}

/*****************************************************************************/
//  Function:  uint8_t nfc_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [out]		: data buffer which is to be read from the flash
//
//		size				: the size of the data buffer  
//
//  Return:
//		NFC_ST_DONE	: page reads ok
//
//		NFC_ST_FAIL		: page reads fail
//
//		NFC_ST_TIME_OUT	: page reads timeout
//
//  Description:
//      NFC reads data from the address of the flash without ecc.
//
/*****************************************************************************/
uint8_t nfc_raw_data_read_1(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	// configure dma
	REG32(NFC_DMA_ADDR_L_REG) = (uint32_t)p_data;
	REG32(NFC_DMA_CNT_REG) = size;
	REG32(NFC_DMA_TLVL_REG) = 0;
	REG32(NFC_DMA_CTRL_REG) = NFC_DMA_CTRL_VAL(ADDR_INC_16_TRANS);
	
	// issue command
	REG32(NFC_ECC_STAT_REG) = 0x0;
	REG32(NFC_INT_STATUS_REG) = 0x0;

	REG32(NFC_ADDR0_COL_REG) = 0;
	REG32(NFC_ADDR0_ROW_REG) = addr;
	REG32(NFC_DATA_SIZE_REG) = size;
	
	REG32(NFC_CONTROL_REG) = NFC_GET_CTRL_VAL(ECC_BLK_SIZE_1024B, CTRL_INT_EN, ECC_DIS, g_nfc_para.bus_size);
	REG32(NFC_COMMAND_REG) = NFC_GET_CMD_VAL(CMD_SEQ_10, INPUT_SEL_DMA, SEL_FIFO_MODULE, NFC_READ_PAGE_CMD0, 0, NFC_READ_PAGE_CMD2);

	return NFC_ST_DONE;
}


uint32_t nfc_read_int_st(void)
{
	return REG32(NFC_INT_STATUS_REG);
}

void nfc_clr_int_st(uint32_t val)
{
	uint32_t reg_val = REG32(NFC_INT_STATUS_REG);
	
	REG32(NFC_INT_STATUS_REG) &= (~val);
}

void nfc_set_int_mask(uint32_t val)
{
	uint32_t reg_val = REG32(NFC_INT_MASK_REG);
	
	REG32(NFC_INT_MASK_REG) |= val;

	debug("mask:%x\n", REG32(NFC_INT_MASK_REG));
}

void nfc_int_ctrl(E_NFC_CONTROL_INT_EN int_en)
{
	uint32_t reg_val = REG32(NFC_CONTROL_REG);

	reg_val &= (~(1<<CTRL_INT_EN_SHIFT));

	reg_val |= int_en;

	REG32(NFC_CONTROL_REG) = reg_val;

	debug("ctrl:%x\n", REG32(NFC_CONTROL_REG));
}

