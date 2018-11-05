/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/

#include "qspi.h"
#include "cdl_qspi_nand.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
/*****************************************************************************/
//  Function:  void qspi_set_drir(uint8_t dummy_clks, uint8_t data_io_mode, uint8_t addr_io_mode, uint8_t cmd_rd)
//
//  Parameters:
//      	dummy_clks 	: Number of Dummy Clock Cycles required by device for Read Instruction
//						= DUMMY_CLKS_0
//						= DUMMY_CLKS_8
//
//		data_io_mode	: Data Transfer Type for Standard SPI modes 
//						= SIO_MODE
//						= DIO_MODE
//						= QIO_MODE
//
//		addr_io_mode	: Address Transfer Type for Standard SPI modes 
//						= SIO_MODE
//						= DIO_MODE
//						= QIO_MODE
//
//		cmd_rd		: read command
//
//  Return:
//		none
//		
//  Description:
//      set Device Read Instruction Register
//
/*****************************************************************************/
void qspi_set_drir(uint8_t dummy_clks, uint8_t data_io_mode, uint8_t addr_io_mode, uint8_t cmd_rd)
{
	REG32(QSPI_DEVRDINST_REG) = (dummy_clks<<24) | (data_io_mode<<16) | (addr_io_mode<<12) | (cmd_rd<<0);
}

/*****************************************************************************/
//  Function:  void qspi_set_dwir(uint8_t data_io_mode, uint8_t addr_io_mode, uint8_t cmd_wr)
//
//  Parameters:
//
//		data_io_mode		: Data Transfer Type for Standard SPI modes 
//						= SIO_MODE
//						= DIO_MODE
//						= QIO_MODE
//
//		addr_io_mode		: Address Transfer Type for Standard SPI modes 
//						= SIO_MODE
//						= DIO_MODE
//						= QIO_MODE
//
//		cmd_wr		: write command
//
//  Return:
//		none
//		
//  Description:
//      set Device Write Instruction Register
//
/*****************************************************************************/
void qspi_set_dwir(uint8_t data_io_mode, uint8_t addr_io_mode, uint8_t cmd_wr)
{
	REG32(QSPI_DEVWRINST_REG) = (data_io_mode<<16) | (addr_io_mode<<12) | (1<<8) | (cmd_wr<<0);
}

/*****************************************************************************/
//  Function:  void qspi_set_ddr(uint8_t dsda, uint8_t csdads, uint8_t cseot, uint8_t cssot)
//
//  Parameters:
//
//		dsda		: Data Transfer Type for Standard SPI modes 
//						
//		csdads	:  Chip Select De-Assert Different Slaves
//
//		cseot	:  Chip Select End Of Transfer 
//
//		cssot	:  Chip Select Start Of Transfer 
//
//  Return:
//		none
//		
//  Description:
//      set QSPI Device Delay Register
//
/*****************************************************************************/
void qspi_set_ddr(uint8_t dsda, uint8_t csdads, uint8_t cseot, uint8_t cssot)
{
	REG32(QSPI_DEVDLY_REG) = (dsda<<24) | (csdads<<16) | (cseot<<8) | (cssot<<0);
}

/*****************************************************************************/
//  Function:  void qspi_set_dsr(uint8_t addr_bytes, uint16_t page_bytes, uint8_t blk_bytes)
//
//  Parameters:
//
//		addr_bytes	: Number of address bytes
//						= ADDR_BYTES_1
//						= ADDR_BYTES_2
//						= ADDR_BYTES_3
//
//		page_bytes	:  Number of bytes per device page
//						= real bytes
//
//		blk_bytes		:  Number of bytes per block
//						= 0, 1 byte
//						= 1, 2 bytes
//						= 2, 4 bytes
//						= 3, 8 bytes
//						= 16, 64K bytes
//
//  Return:
//		none
//		
//  Description:
//      set Device Size Configuration Register
//
/*****************************************************************************/
void qspi_set_dsr(uint8_t addr_bytes, uint16_t page_bytes, uint8_t blk_bytes)
{
	REG32(QSPI_DEVSIZE_REG) = (blk_bytes<<16) | (page_bytes<<4) | (addr_bytes<<0);
}

/*****************************************************************************/
//  Function:  void qspi_set_cfgr(uint8_t br_div, E_QSPI_PHASE phase, uint8_t cs)
//
//  Parameters:
//
//		br_div	: Master mode baud rate divisor (2 to 32)
//						= 0x00, /2
//						= 0x01, /4
//						= ...
//						= 0x0F, /32
//
//		phase	: clock phase
//
//		cs		: chip select
//
//  Return:
//		none
//		
//  Description:
//      set QSPI Configuration Register
//		DAC			: on
//		AHB decoder 	: off
//		
/*****************************************************************************/
void qspi_set_cfgr(uint8_t br_div, E_QSPI_PHASE phase, uint8_t cs)
{
	// indirect
	REG32(QSPI_CFG_REG) = (br_div<<QSPI_CFG_BD_DIV_SHIFT) 			| 
							(0<<QSPI_CFG_AHB_ADDR_REMAP_EN_SHIFT) 	| 
							(1<<QSPI_CFG_DMA_EN_SHIFT) 				| 
							(cs<<QSPI_CFG_PER_SEL_CHIP_SHIFT) 		|
							(1<<QSPI_CFG_DIRECT_EN_SHIFT) 			|
							(phase<<QSPI_CFG_POLARITY_SHIFT) 		|
							(1<<QSPI_CFG_EN_SHIFT); 
}

/*****************************************************************************/
//  Function:  uint8_t qspi_indirect_wr_chk_st(void)
//
//  Parameters:
//		none
//
//  Return:
//		QSPI_ST_DONE		: check status ok
//
//		QSPI_ST_FAIL			: check status  fail
//	
//		QSPI_ST_TIME_OUT	: check status timeout
//
//  Description:
//      Indirect write check status.
//
/*****************************************************************************/
uint8_t qspi_indirect_wr_chk_st(void)
{
	uint32_t time_out = 0;
	
	// check indirect done
	while (1)
	{
		if (REG32(QSPI_INDIRWRTRANS_REG) & (1<<INDIRECT_WR_DONE_SHIFT))
		{
			// clear the write done status
			REG32(QSPI_INDIRWRTRANS_REG) = (1<<INDIRECT_WR_DONE_SHIFT) | (0<<INDIRECT_WR_CANCEL_SHIFT);
			break;
		}

		// delay 10us
		udelay(10);

		if (time_out++ > g_qspi_nand_para.time_out)
		{
			debug("indirect wr timeout\n");
			
			// cancel the indirect write 
			REG32(QSPI_INDIRWRTRANS_REG) = 1<<INDIRECT_WR_CANCEL_SHIFT;
			
			return QSPI_ST_TIME_OUT;
		}
	}

	// check wr fifo empty
	while (((REG32(QSPI_SRAMFILLLEVEL_REG) >> 16) & 0xFFFF) == 0)
	{
		break;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t qspi_indirect_rd_chk_st(void)
//
//  Parameters:
//		none
//
//  Return:
//		QSPI_ST_DONE		: check status ok
//
//		QSPI_ST_FAIL			: check status  fail
//	
//		QSPI_ST_TIME_OUT	: check status timeout
//
//  Description:
//      Indirect read check status.
//
/*****************************************************************************/
uint8_t qspi_indirect_rd_chk_st(void)
{
	uint32_t time_out = 0;
	
	// check indirect done
	while (1)
	{
		if (REG32(QSPI_INDIRECTRDTRANS_REG) & (1<<INDIRECT_RD_DONE_SHIFT))
		{
			// clear the read done status
			REG32(QSPI_INDIRECTRDTRANS_REG) = (1<<INDIRECT_RD_DONE_SHIFT);
			
			break;
		}

		// delay 10us
		udelay(10);

		if (time_out++ > g_qspi_nand_para.time_out)
		{
			debug("indirect rd timeout\n");
			
			// cancel the indirect read
			REG32(QSPI_INDIRECTRDTRANS_REG) = (1<<INDIRECT_RD_CANCEL_SHIFT);

			return QSPI_ST_TIME_OUT;
		}		
	}

	// check rd fifo empty
	while (((REG32(QSPI_SRAMFILLLEVEL_REG) >> 0) & 0xFFFF) == 0)
	{
		break;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  void qspi_indirect_wr_setup(uint32_t addr, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		data_size			: the dma size
// 
//  Return:
//		none
//
//  Description:
//      Indirect write setup.
//
/*****************************************************************************/
void qspi_indirect_wr_setup(uint32_t addr, uint32_t size)
{	
	// mask all interrupt
	REG32(QSPI_INTMASK_REG) = 0;

	REG32(QSPI_SRAMPART_REG) = 0x80;
	REG32(QSPI_INDIRWRTRANSSTADDR_REG) = addr;
	REG32(QSPI_INDIRWRTRANSCNT_REG) = size;
	REG32(QSPI_INDRECTAHBADDRTRI_REG) = QSPI_BASE_ADDR;
	REG32(QSPI_INDIRAHBTRIGADDRRANGE_REG) = 4;
	
	REG32(QSPI_DMAPERIPH_REG) = 0x402;

	// start indirect, set ctrl.0
	REG32(QSPI_INDIRWRTRANS_REG) = 1;
}

/*****************************************************************************/
//  Function:  void qspi_indirect_rd_setup(uint32_t addr, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		data_size			: the dma size
// 
//  Return:
//		none
//
//  Description:
//      Indirect read setup(polling used).
//
/*****************************************************************************/
void qspi_indirect_rd_setup(uint32_t addr, uint32_t size)
{	
	// mask all interrupt
	REG32(QSPI_INTMASK_REG) = 0;

	REG32(QSPI_SRAMPART_REG) = 0x80;
	REG32(QSPI_INDIRRDTRANSSTADDR_REG) = addr;
	REG32(QSPI_INDIRRDTRANSNUMBYTES_REG) = size;
	REG32(QSPI_INDRECTAHBADDRTRI_REG) = QSPI_BASE_ADDR;
	REG32(QSPI_INDIRAHBTRIGADDRRANGE_REG) = 4;
	
	REG32(QSPI_DMAPERIPH_REG) = 0x402;

	// start indirect, set ctrl.0
	REG32(QSPI_INDIRECTRDTRANS_REG) = 1;
}

/*****************************************************************************/
//  Function:  void qspi_indirect_rd_setup_1(uint32_t addr, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		data_size			: the dma size
// 
//  Return:
//		none
//
//  Description:
//      Indirect read setup(interrupt test used).
//
/*****************************************************************************/
void qspi_indirect_rd_setup_1(uint32_t addr, uint32_t size)
{	
	REG32(QSPI_SRAMPART_REG) = 0x80;
	REG32(QSPI_INDIRRDTRANSSTADDR_REG) = addr;
	REG32(QSPI_INDIRRDTRANSNUMBYTES_REG) = size;
	REG32(QSPI_INDRECTAHBADDRTRI_REG) = QSPI_BASE_ADDR;
	REG32(QSPI_INDIRAHBTRIGADDRRANGE_REG) = 4;
	
	REG32(QSPI_DMAPERIPH_REG) = 0x402;

	// start indirect, set ctrl.0
	REG32(QSPI_INDIRECTRDTRANS_REG) = 1;
}

/*****************************************************************************/
//  Function:  uint8_t qspi_cmd_chk_st(void)
//
//  Parameters:
//		none
//
//  Return:
//		QSPI_ST_DONE		: check status ok
//		
//		QSPI_ST_TIME_OUT	: check status timeout
//
//  Description:
//      Check comand executable status of the qspi IP.
//		
/*****************************************************************************/
uint8_t qspi_cmd_chk_st(void)
{
	uint32_t time_out;

	// check status
	while (1) 
	{
		// STIG command execution in progress.
		time_out = 0;
		while ((REG32(QSPI_FLASHCMD_REG) & QSPI_STIG_ST_MASK) != 0) 
		{
			// delay 1us
			udelay(1);

			if (time_out++ > g_qspi_nand_para.time_out)
			{
				return QSPI_ST_TIME_OUT;
			}
		}
		
		// Serial Interface and QSPI pipeline is IDLE.
		time_out = 0;
		while ((REG32(QSPI_CFG_REG) & QSPI_PIPLINE_IDLE_MASK) == 0) 
		{
			// delay 1us
			udelay(1);
		
			if (time_out++ > g_qspi_nand_para.time_out)
			{
				return QSPI_ST_TIME_OUT;
			}
		}

		return QSPI_ST_DONE;
	}
}


uint32_t qspi_read_int_st(void)
{
	return REG32(QSPI_INTST_REG);
}

void qspi_clr_int_st(uint32_t val)
{
	REG32(QSPI_INTST_REG) = val;
}

void qspi_set_int_mask(uint32_t val)
{
	REG32(QSPI_INTMASK_REG) |= val;
}

void qspi_set_clk_mode(E_QSPI_PHASE phase)
{
	uint32_t reg_val = REG32(QSPI_CFG_REG);

	reg_val &= (~QSPI_PHASE_MASK);
	reg_val |= QSPI_PHASE_VAL(phase);

	REG32(QSPI_CFG_REG) = reg_val;
}

/*****************************************************************************/
//  Function:  void qspi_set_access_mode(E_QSPI_MODE mode)
//
//  Parameters:
//		mode	: qspi mode
// 
//  Return:
//		none
//
//  Description:
//      Set qspi access mode.
//
/*****************************************************************************/
void qspi_set_access_mode(E_QSPI_MODE mode)
{
	uint32_t reg_val = 0;

	if (mode == QSPI_MODE_LEGACY) 
	{
		// enable legacy mode; disable direct access and remap
		reg_val = REG32(QSPI_CFG_REG);

		// enable legacy mode
		reg_val |= QSPI_CFG_LEGACY_EN_MASK;

		// set direct enable
		reg_val |= QSPI_CFG_DIRECT_EN_MASK;

		// disable remap
		reg_val &= ~QSPI_CFG_AHB_ADDR_REMAP_EN_MASK;
		
		// enable dma mode
		reg_val |= QSPI_CFG_DMA_EN_MASK;
		
		REG32(QSPI_CFG_REG) = reg_val;
		
		// set fifo threshold level to 1
		REG32(QSPI_TXTHRESH_REG) = 1;
		REG32(QSPI_RXTHRESH_REG) = 1;

		REG32(QSPI_INTMASK_REG) = 0;

		REG32(QSPI_MODEBIT_REG) = 0xFF;
		REG32(QSPI_DEVRDINST_REG) = 0x0010000B;
	} 
	else if (mode == QSPI_MODE_DIRECT) 
	{
		// set direct enable and enable remap
		reg_val = REG32(QSPI_CFG_REG);

		// clear legacy mode
		reg_val &= ~QSPI_CFG_LEGACY_EN_MASK;

		// set direct enable
		reg_val |= QSPI_CFG_DIRECT_EN_MASK;

		// enable remap
		reg_val |= QSPI_CFG_AHB_ADDR_REMAP_EN_MASK;
		
		// enable dma mode
		reg_val |= QSPI_CFG_DMA_EN_MASK;
		
		REG32(QSPI_CFG_REG) = reg_val;

		REG32(QSPI_REMAPADDR_REG) = 0;

		REG32(QSPI_INTMASK_REG) = 0;		
	} 
	else 
	{
		reg_val = REG32(QSPI_CFG_REG);

		// clear legacy mode
		reg_val &= ~QSPI_CFG_LEGACY_EN_MASK; 

		// set direct disable
		//reg_val &= ~QSPI_CFG_DIRECT_EN_MASK;
		reg_val |= QSPI_CFG_DIRECT_EN_MASK;

		// disable remap
		reg_val &= ~QSPI_CFG_AHB_ADDR_REMAP_EN_MASK;

		// enable dma mode
		reg_val |= QSPI_CFG_DMA_EN_MASK;

		REG32(QSPI_CFG_REG) = reg_val;
		
		REG32(QSPI_INTMASK_REG) = 0;
	}	
}


