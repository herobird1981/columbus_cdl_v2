/**********************************************************************************************************/
//
//  Filename :          qspi_nand.c
//
//  COPYRIGHT, 2016
//  The contents of this file is the sole proprietary property of, confidential
//  to, and copyrighted by Brite Company.  It may not be copied or reproduced
//  in any manner or form without the prior written consent of Brite Company.
//
//  Original Author:    Billy
//  Date created:       August 24, 2016
//
//  Description:
//      implement qspi nand basic operation
//
//  Revision:
//  Date       		Author      	Description
//  08/24/16    	Billy   		Original
//
/*************************************************************************************************************/
#include "cdl_qspi_nand.h"
#include "cdl_dmac.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
qspi_nand_para_t g_qspi_nand_para = {0};
uint32_t g_qspi_nand_oob[2] = {0, 0};
uint16_t g_qspi_nand_oob_ofst[2] = {(2048+4), (2048+20)};

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/
void qspi_nand_set_drir(uint8_t dummy_clks, uint8_t cmd_rd);

void qspi_nand_set_dwir(uint8_t cmd_wr);

void qspi_nand_set_ddr(uint8_t dsda, uint8_t csdads, uint8_t cseot, uint8_t cssot);

void qspi_nand_set_dsr(uint8_t addr_bytes, uint16_t page_bytes, uint8_t blk_bytes);

E_QSPI_ST qspi_nand_wr_ctrl(uint8_t cmd);

E_QSPI_ST qspi_nand_chk_st(uint8_t *p_st);

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
static void qspi_inferface_ctrl(E_QSPI_INF_EN en)
{
	uint32_t reg_val = REG32(QSPI_CFG_REG);

	reg_val &= ~QSPI_INF_EN_MASK;
	reg_val |= QSPI_INF_EN_VAL(en);

	REG32(QSPI_CFG_REG) = reg_val;
}

static void qspi_read_data_capture(E_QSPI_DATA_BYPASS bypass, unsigned int delay)
{
	uint32_t reg_val = REG32(QSPI_CFG_REG);

	qspi_inferface_ctrl(QSPI_INF_DIS);

	reg_val = REG32(QSPI_RDDATACAP_REG);

	// configure bypass
	reg_val &= ~QSPI_DATA_BYPASS_MASK;
	reg_val |= QSPI_DATA_BYPASS_VAL(bypass);

	// configure read data delay
	reg_val &= ~QSPI_DELAY_RD_DATA_MASK;
	reg_val |= QSPI_DELAY_RD_DATA_VAL(delay);

	REG32(QSPI_RDDATACAP_REG) = reg_val;

	qspi_inferface_ctrl(QSPI_INF_EN);
}

static void qspi_config_baudrate_div(uint32_t ref_clk_hz, uint32_t sclk_hz)
{
	uint32_t reg_val;
	uint32_t div = 0, real_div = 0;

	div = ref_clk_hz / sclk_hz;

	if (div > 32) 
	{
		div = 32;
	}

	switch (div)
	{
		case 1:
		case 2:	real_div = 0; break;
		case 3:
		case 4: real_div = 1; break;
		case 5:
		case 6: real_div = 2; break;
		case 7:
		case 8: real_div = 3; break;
		case 9:
		case 10: real_div = 4; break;
		case 11:
		case 12: real_div = 5; break;
		case 13:
		case 14: real_div = 6; break;
		case 15:
		case 16: real_div = 7; break;
		case 17:
		case 18: real_div = 8; break;
		case 19:
		case 20: real_div = 9; break;
		case 21:
		case 22: real_div = 10; break;
		case 23:
		case 24: real_div = 11; break;
		case 25:
		case 26: real_div = 12; break;
		case 27:
		case 28: real_div = 13; break;
		case 29:
		case 30: real_div = 14; break;
		case 31:
		case 32:
		default: real_div = 15; break;
	}

	div = (div & 1) ? (div + 1) : div;
	debug("%s: ref_clk %dHz, sclk %dHz, Div %x, real_clk:%dHz\n", __func__, ref_clk_hz, sclk_hz, div, (ref_clk_hz/div));

	qspi_inferface_ctrl(QSPI_INF_DIS);

	reg_val = REG32(QSPI_CFG_REG);
	reg_val &= ~QSPI_BD_DIV_MASK;
	reg_val |= QSPI_BD_DIV_VAL(real_div);
	REG32(QSPI_CFG_REG) = reg_val;

	qspi_inferface_ctrl(QSPI_INF_EN);
}

#define QSPI_CAL_DELAY(tdelay_ns, tref_ns, tsclk_ns) 	(((tdelay_ns) - (tsclk_ns)) / (tref_ns))
static void qspi_apb_delay(uint32_t sclk_hz)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint32_t reg_val;
	uint32_t ref_clk_ns, sclk_ns;
	uint32_t tshsl, tchsh, tslch, tsd2d;

	qspi_inferface_ctrl(QSPI_INF_DIS);

	// Convert to ns. 
	ref_clk_ns = (1000000000) / p_qspi_para->pclk;

	// Convert to ns. 
	sclk_ns = (1000000000) / sclk_hz;

	// Plus 1 to round up 1 clock cycle. 
	tshsl = QSPI_CAL_DELAY(p_qspi_para->tshsl_ns, ref_clk_ns, sclk_ns) + 1;
	tchsh = QSPI_CAL_DELAY(p_qspi_para->tchsh_ns, ref_clk_ns, sclk_ns) + 1;
	tslch = QSPI_CAL_DELAY(p_qspi_para->tslch_ns, ref_clk_ns, sclk_ns) + 1;
	tsd2d = QSPI_CAL_DELAY(p_qspi_para->tsd2d_ns, ref_clk_ns, sclk_ns) + 1;

	reg_val = QSPI_CSSOT_VAL(tslch&0xFF)  |
			  QSPI_CSEOT_VAL(tchsh&0xFF)  |
			  QSPI_CSDADS_VAL(tsd2d&0xFF) |
			  QSPI_CSDA_VAL(tshsl&0xFF);
	
	REG32(QSPI_DEVDLY_REG) = reg_val;

	qspi_inferface_ctrl(QSPI_INF_EN);
}

static void qspi_write_speed(uint32_t hz)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	
	qspi_config_baudrate_div(p_qspi_para->pclk, hz);

	// Reconfigure delay timing if speed is changed. 
	qspi_apb_delay(hz);
}

// Calibration sequence to determine the read data capture delay register
uint8_t qspi_calibration(uint32_t hz)
{
	uint8_t ret_val;
	uint8_t id[3], tmp_id[3];
	int8_t i, range_lo = -1, range_hi = -1, range_mid = 0;

	// start with slowest clock (1 MHz)
	qspi_write_speed(1000000);

	// configure the read data capture delay register to 0 
	qspi_read_data_capture(QSPI_DATA_BYPASS_DIS, 0);

	// read the ID which will be our golden value
	ret_val = qspi_nand_read_id(id, 3);
	if (ret_val != QSPI_ST_DONE)
	{
		info("SF: Calibration failed (read)\n");
		return ret_val;
	}

	// use back the intended clock and find low range
	qspi_write_speed(hz);
	for (i = 0; i <= QSPI_READ_CAPTURE_MAX_DELAY; i++) 
	{
		// reconfigure the read data capture delay register
		qspi_read_data_capture(QSPI_DATA_BYPASS_DIS, i);

		// issue a RDID to get the ID value
		ret_val = qspi_nand_read_id(tmp_id, 3);
		if (ret_val != QSPI_ST_DONE)
		{
			info("SF: Calibration failed (read)\n");
			return ret_val;
		}		

		// search for range low
		if ((range_lo == -1) && 
			((id[0] == tmp_id[0]) && (id[1] == tmp_id[1]) && (id[2] == tmp_id[2]))) 
		{
			range_lo = i;
			continue;
		}

		// search for range high
		if ((range_lo != -1) && 
			((id[0] != tmp_id[0]) || (id[1] != tmp_id[1]) || (id[2] != tmp_id[2]))) 
		{
			range_hi = i - 1;
			break;
		}
		
		range_hi = i;
	}

	if (range_lo == -1) 
	{
		debug("SF: Calibration failed (low range)\n");
		return QSPI_ST_FAIL;
	}

	// configure the final value for read data capture delay register
	range_mid = (range_hi + range_lo) / 2;
	qspi_read_data_capture(QSPI_DATA_BYPASS_DIS, range_mid);
	debug("SF: Read data capture delay calibrated to %i (%i - %i)\n", range_mid, range_lo, range_hi);

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  void qspi_nand_set_drir(uint8_t dummy_clks, uint8_t cmd_rd)
//
//  Parameters:
//      	dummy_clks 	: Number of Dummy Clock Cycles required by device for Read Instruction
//						= DUMMY_CLKS_0
//						= DUMMY_CLKS_8
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
void qspi_nand_set_drir(uint8_t dummy_clks, uint8_t cmd_rd)
{
	REG32(QSPI_DEVRDINST_REG) = (dummy_clks<<24) | (g_qspi_nand_para.io_rmode<<16) | (cmd_rd<<0);
}

/*****************************************************************************/
//  Function:  void qspi_nand_set_dwir(uint8_t cmd_wr)
//
//  Parameters:
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
void qspi_nand_set_dwir(uint8_t cmd_wr)
{
	REG32(QSPI_DEVWRINST_REG) = (g_qspi_nand_para.io_wmode<<16) | (1<<8) | (cmd_wr<<0);
}

/*****************************************************************************/
//  Function:  void qspi_nand_set_ddr(uint8_t dsda, uint8_t csdads, uint8_t cseot, uint8_t cssot)
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
void qspi_nand_set_ddr(uint8_t dsda, uint8_t csdads, uint8_t cseot, uint8_t cssot)
{
	REG32(QSPI_DEVDLY_REG) = (dsda<<24) | (csdads<<16) | (cseot<<8) | (cssot<<0);
}

/*****************************************************************************/
//  Function:  void qspi_nand_set_dsr(uint8_t addr_bytes, uint16_t page_bytes, uint8_t blk_bytes)
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
void qspi_nand_set_dsr(uint8_t addr_bytes, uint16_t page_bytes, uint8_t blk_bytes)
{
	REG32(QSPI_DEVSIZE_REG) = (blk_bytes<<16) | (page_bytes<<4) | (addr_bytes<<0);
}

uint8_t qspi_get_brdiv(uint32_t ref_clk_hz, uint32_t sclk_hz)
{
	uint32_t div = 0, real_div = 0;

	div = ref_clk_hz / sclk_hz;

	if (div > 32) 
	{
		div = 32;
	}

	switch (div)
	{
		case 1:
		case 2:	real_div = 0; break;
		case 3:
		case 4: real_div = 1; break;
		case 5:
		case 6: real_div = 2; break;
		case 7:
		case 8: real_div = 3; break;
		case 9:
		case 10: real_div = 4; break;
		case 11:
		case 12: real_div = 5; break;
		case 13:
		case 14: real_div = 6; break;
		case 15:
		case 16: real_div = 7; break;
		case 17:
		case 18: real_div = 8; break;
		case 19:
		case 20: real_div = 9; break;
		case 21:
		case 22: real_div = 10; break;
		case 23:
		case 24: real_div = 11; break;
		case 25:
		case 26: real_div = 12; break;
		case 27:
		case 28: real_div = 13; break;
		case 29:
		case 30: real_div = 14; break;
		case 31:
		case 32:
		default: real_div = 15; break;
	}	

	div = (div & 1) ? (div + 1) : div;
	debug("%s: ref_clk %dHz, sclk %dHz, Div %d, real_clk:%dHz\n", __func__, ref_clk_hz, sclk_hz, div, (ref_clk_hz/div));

	return real_div;
}


/*****************************************************************************/
//  Function:  uint8_t qspi_nand_quad_enable(void)
//
//  Parameters:
//		none
//
//  Return:
//		QSPI_ST_DONE		: set quad enable ok
//		
//		QSPI_ST_TIME_OUT	: set quad enable timeout
//		
//  Description:
//      Set quad enable.
//		
/*****************************************************************************/
uint8_t qspi_nand_quad_enable(void)
{
	uint8_t fea = 0;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;

	if (g_qspi_nand_para.manu_id == QSPI_MANU_ID_MXIC)
	{
		if (QSPI_ST_DONE != qspi_nand_get_feature(QSPI_NAND_OTP_FA, &fea))
		{
			return QSPI_ST_TIME_OUT;
		}
		
		fea |= (1<<QSPI_MXIC_OTP_QE_SHIFT);
		
		if (QSPI_ST_DONE != qspi_nand_set_feature(QSPI_NAND_OTP_FA, fea))
		{
			return QSPI_ST_TIME_OUT;
		}		
	}
	
	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t qspi_nand_blk_protect_ctrl(uint8_t bp)
//
//  Parameters:
//		bp	: block protectiion bits
//				= BP_ALL_UNLOCKED
//				= BP_UPPER_1_64_LOCKED
//				= BP_UPPER_1_32_LOCKED
//				= BP_UPPER_1_16_LOCKED
//				= BP_UPPER_1_8_LOCKED
//				= BP_UPPER_1_4_LOCKED
//				= BP_UPPER_1_2_LOCKED
//				= BP_ALL_LOCKED
//
//				= BP_LOWER_1_64_LOCKED
//				= BP_LOWER_1_32_LOCKED
//				= BP_LOWER_1_16_LOCKED
//				= BP_LOWER_1_8_LOCKED
//				= BP_LOWER_1_4_LOCKED
//				= BP_LOWER_1_2_LOCKED
//				= BP_LOWER_63_64_LOCKED
//				= BP_LOWER_31_32_LOCKED
//				= BP_LOWER_15_16_LOCKED
//				= BP_LOWER_7_8_LOCKED
//				= BP_LOWER_3_4_LOCKED
//				= BP_BLK0_LOCKED
//				= BP_UPPER_63_64_LOCKED
//				= BP_UPPER_31_32_LOCKED
//				= BP_UPPER_15_16_LOCKED
//				= BP_UPPER_7_8_LOCKED
//				= BP_UPPER_3_4_LOCKED
//
//  Return:
//		QSPI_ST_DONE		: set block protect ok
//		
//		QSPI_ST_TIME_OUT	: set block protect timeout
//		
//  Description:
//      Set block protect.
//		
/*****************************************************************************/
uint8_t qspi_nand_blk_protect_ctrl(uint8_t bp)
{
	if (QSPI_ST_DONE != qspi_nand_set_feature(QSPI_NAND_BLK_PROT_FA, bp))
	{
		return QSPI_ST_TIME_OUT;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  void qspi_nand_init(uint8_t div, E_QSPI_PHASE phase, uint8_t cs)
//
//  Parameters:
//		div		: qspi divider
//
//		phase	: clock phase
//
//		cs		: chip select
//
//  Return:
//		none
//		
//  Description:
//      qspi initialize.
//
/*****************************************************************************/
void qspi_nand_init(uint8_t div, E_QSPI_PHASE phase, uint8_t cs)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	
	// check the qspi timeout validity
	p_qspi_para->time_out = 1 + 1;
	p_qspi_para->time_out *= QSPI_TIME_OUT;
	
	// qspi clk
	phase = (((phase>>0)&1)<<1) | 
			(((phase>>1)&1)<<0);
	qspi_set_cfgr(div, phase, cs);	
	qspi_set_clk_mode(phase);	

	qspi_set_access_mode(QSPI_MODE_INDIRECT);
	
	qspi_set_ddr(0, 0, 0, 0);	

	// disable status polling
	REG32(QSPI_WRCOMPCTRL_REG) |= (1<<14);

	// 3 bytes address
	// 256 bytes per page
	// 64K bytes per block
	qspi_set_dsr(ADDR_BYTES_3, 0x800, 0x10);

	p_qspi_para->io_wmode = SIO_MODE;
	p_qspi_para->io_rmode = SIO_MODE;

	p_qspi_para->sio_pp_load_cmd = QSPI_NAND_PP_LOAD;
	p_qspi_para->sio_pp_load_rand_cmd = QSPI_NAND_PP_LOAD_RAND;
	p_qspi_para->qio_pp_load_cmd = QSPI_NAND_PP_LOAD_X4;
	p_qspi_para->qio_pp_load_rand_cmd = QSPI_NAND_PP_LOAD_RAND_X4;
	p_qspi_para->io_pp_exe_cmd = QSPI_NAND_PP_EXE;
	
	p_qspi_para->io_rd_cmd = QSPI_NAND_RD;
	p_qspi_para->sio_rd_cache_cmd = QSPI_NAND_RD_CACHE;
	p_qspi_para->dio_rd_cache_cmd = QSPI_NAND_RD_CACHE_X2;
	p_qspi_para->qio_rd_cache_cmd = QSPI_NAND_RD_CACHE_X4;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_set_feature(uint8_t addr, uint8_t data)
//
//  Parameters:
//		addr		: feature address
//					= QSPI_NAND_OTP_FA
//					= QSPI_NAND_ST_FA
//					= QSPI_NAND_BLK_PROT_FA
//	
//		data		: feature data
//
//  Return:
//		QSPI_ST_DONE		: set feature ok
//		
//		QSPI_ST_TIME_OUT	: set feature timeout
//		
//  Description:
//      Set feature to the device
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_set_feature(uint8_t addr, uint8_t data)
{	
	uint8_t st;
		
	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// set address
	REG32(QSPI_FLASHCMDADDR_REG) = addr;

	// set data
	REG32(QSPI_FLASHCMDWRDATALO_REG) = data;

	// issue the command(1 address, 1 byte read data)
	REG32(QSPI_FLASHCMD_REG) = QSPI_ISSUE_WR_DATA_CMD(QSPI_NAND_CMD_SET_FEA, CMD_RW_BYTES_1, CMD_ADDR_BYTES_1, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status	
	if (QSPI_ST_DONE != qspi_nand_chk_st(&st))
	{
		return QSPI_ST_TIME_OUT;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_get_feature(uint8_t addr, uint8_t *p_data)
//
//  Parameters:
//		addr			: feature address
//						= QSPI_NAND_OTP_FA
//						= QSPI_NAND_ST_FA
//						= QSPI_NAND_BLK_PROT_FA
//	
//		p_data [out]	: feature data
//
//  Return:
//		QSPI_ST_DONE		: get feature ok
//		
//		QSPI_ST_TIME_OUT	: get feature timeout
//		
//  Description:
//      Get feature from the device.
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_get_feature(uint8_t addr, uint8_t *p_data)
{
	uint32_t reg_val;

	// set address
	REG32(QSPI_FLASHCMDADDR_REG) = addr;

	// issue the comman(1 address, 1 byte read data)
	REG32(QSPI_FLASHCMD_REG) = QSPI_ISSUE_RD_DATA_CMD(QSPI_NAND_CMD_GET_FEA, CMD_RW_BYTES_1, CMD_ADDR_BYTES_1, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// get data
	reg_val = REG32(QSPI_FLASHCMDRDDATALO_REG);
	
	p_data[0] = reg_val & 0xFF;
		
	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_wr_ctrl(uint8_t cmd)
//
//  Parameters:
//		cmd	: write enable/disable command
//				= QSPI_NAND_CMD_WREN
//				= QSPI_NAND_CMD_WRDI
//
//  Return:
//		QSPI_ST_DONE		: chip erases ok
//		
//		QSPI_ST_TIME_OUT	: chip erases timeout
//		
//  Description:
//      Issue write enable/disable command to the device.
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_wr_ctrl(uint8_t cmd)
{	
	// write enable
	REG32(QSPI_FLASHCMD_REG) = QSPI_ISSUE_CMD(cmd);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t qspi_nand_ecc_ctrl(E_QSPI_ECC_MODE ecc_mode)
//
//  Parameters:
//		ecc_mode				: ecc mode
//
//  Return:
//		QSPI_ST_DONE		: set ecc mode ok
//		
//		QSPI_ST_TIME_OUT	: set ecc mode  timeout
//		
//  Description:
//      Set ecc mode.
//		
/*****************************************************************************/
uint8_t qspi_nand_ecc_ctrl(E_QSPI_ECC_MODE ecc_mode)
{
	uint8_t fea = 0;

	if (QSPI_ST_DONE != qspi_nand_get_feature(QSPI_NAND_OTP_FA, &fea))
	{
		return QSPI_ST_TIME_OUT;
	}

	fea &= (~(1<<QSPI_MXIC_OTP_ECC_EN_SHIFT));		
	fea |= (ecc_mode << QSPI_MXIC_OTP_ECC_EN_SHIFT);

	if (QSPI_ST_DONE != qspi_nand_set_feature(QSPI_NAND_OTP_FA, fea))
	{
		return QSPI_ST_TIME_OUT;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t qspi_nand_buf_mode_ctrl(E_QSPI_BUF_MODE buf_mode)
//
//  Parameters:
//		buf_mode				: buf mode control
//
//  Return:
//		QSPI_ST_DONE		: set quad enable ok
//		
//		QSPI_ST_TIME_OUT	: set quad enable timeout
//		
//  Description:
//      Set buf mode.
//		
/*****************************************************************************/
uint8_t qspi_nand_buf_mode_ctrl(E_QSPI_BUF_MODE buf_mode)
{
	uint8_t fea = 0;

	if (QSPI_ST_DONE != qspi_nand_get_feature(QSPI_NAND_OTP_FA, &fea))
	{
		return QSPI_ST_TIME_OUT;
	}

	fea &= (~(1<<QSPI_WINBOND_OTP_BUF_MODE_SHIFT));		
	fea |= (buf_mode << QSPI_WINBOND_OTP_BUF_MODE_SHIFT);

	if (QSPI_ST_DONE != qspi_nand_set_feature(QSPI_NAND_OTP_FA, fea))
	{
		return QSPI_ST_TIME_OUT;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  uint8_t qspi_nand_otp_enter_ctrl(E_QSPI_OTP_E_MODE otp_mode)
//
//  Parameters:
//		otp_mode			: otp mode control
//
//  Return:
//		QSPI_ST_DONE		: set quad enable ok
//		
//		QSPI_ST_TIME_OUT	: set quad enable timeout
//		
//  Description:
//      Set otp enter mode.
//		
/*****************************************************************************/
uint8_t qspi_nand_otp_enter_ctrl(E_QSPI_OTP_E_MODE otp_mode)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t fea = 0;

	if (QSPI_ST_DONE != qspi_nand_get_feature(QSPI_NAND_OTP_FA, &fea))
	{
		return QSPI_ST_TIME_OUT;
	}

	if (p_qspi_para->manu_id == QSPI_MANU_ID_MICRON)
	{
		fea &= (~((1<<QSPI_MICRON_OTP_CFG0_SHIFT) 	|
				(1<<QSPI_MICRON_OTP_CFG1_SHIFT)		|
				(1<<QSPI_MICRON_OTP_CFG2_SHIFT)));		
		fea |= (otp_mode << QSPI_MICRON_OTP_CFG1_SHIFT);
	}
	else
	{
		// mxic
		// winbond
		fea &= (~(1<<QSPI_MXIC_OTP_EN_SHIFT));		
		fea |= (otp_mode << QSPI_MXIC_OTP_EN_SHIFT);
	}
	
	if (QSPI_ST_DONE != qspi_nand_set_feature(QSPI_NAND_OTP_FA, fea))
	{
		return QSPI_ST_TIME_OUT;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_chk_st(uint8_t *p_st)
//
//  Parameters:
//		p_st [out]	: device status
//
//  Return:
//		QSPI_ST_DONE		: check status ok
//		
//		QSPI_ST_TIME_OUT	: check status timeout
//
//  Description:
//      Check status of the flash.
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_chk_st(uint8_t *p_st)
{
	uint32_t time_out = 0;

	// check status
	while (1) 
	{
		if (QSPI_ST_DONE != qspi_nand_get_feature(QSPI_NAND_ST_FA, p_st))
		{
#ifdef EXTRA_DEBUG
			debug("\nqspi_nand_get_feature() Timeout\n");
#endif
			return QSPI_ST_TIME_OUT;
		}

		if ((*p_st & (1 << QSPI_ST_OIP_SHIFT)) == 0)
		{
			return QSPI_ST_DONE;
		}

		// delay 10us
		udelay(10);

		if (time_out++ > g_qspi_nand_para.time_out)
		{
			return QSPI_ST_TIME_OUT;
		}
	}
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_sel_die(uint32_t addr)
//
//  Parameters:
//		addr					: flash address(block + page)
//
//  Return:
//		QSPI_ST_DONE		: get feature ok
//		
//		QSPI_ST_TIME_OUT	: get feature timeout
//		
//  Description:
//      Select nand die to be active.
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_sel_die(uint32_t addr)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint32_t die;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// write enable
		if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
		{
			return QSPI_ST_TIME_OUT;
		}

		die = ((addr>>p_qspi_para->blk_shift_bits) < p_qspi_para->blks_per_lun) ? QSPI_WINBOND_NAND_DIE0 : QSPI_WINBOND_NAND_DIE1;

		// set address
		REG32(QSPI_FLASHCMDADDR_REG) = die;

		// issue write comman(1 bytes address, 0 byte dummy, 0 byte data)
		REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(QSPI_WINBOND_NAND_SW_DIE_SEL, ADDR_BYTES_1, 0);
		if (QSPI_ST_DONE != qspi_cmd_chk_st())
		{
			return QSPI_ST_TIME_OUT;
		}	
	}
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MICRON)
	{
		die = ((addr>>p_qspi_para->blk_shift_bits) < p_qspi_para->blks_per_lun) ? QSPI_MICRON_NAND_DIE0 : QSPI_MICRON_NAND_DIE1;
		
		if (QSPI_ST_DONE != qspi_nand_set_feature(QSPI_MICRON_NAND_SW_DIE_SEL, die))
		{
			return QSPI_ST_TIME_OUT;
		}
	}
	else
	{}
		
	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_reset(void)
//
//  Parameters:
//		none
//
//  Return:
//		QSPI_ST_DONE		: get feature ok
//		
//		QSPI_ST_TIME_OUT	: get feature timeout
//		
//  Description:
//      Reset the device.
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_reset(void)
{
	uint8_t st;
	
	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// issue the comman(0 byte address, 0 dummy cycles, 0 byte data)
	REG32(QSPI_FLASHCMD_REG) = QSPI_ISSUE_CMD(QSPI_NAND_CMD_RST);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status	
	return qspi_nand_chk_st(&st);
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_read_id(uint8_t *p_id, uint32_t size)
//
//  Parameters:
//		p_id [out]			: id buffer which is to be read from the flash
//
//		size					: the size of the id buffer 
//
//  Return:
//		QSPI_ST_DONE		: read id  ok
//
//		QSPI_ST_FAIL			: read id fail
//		
//		QSPI_ST_TIME_OUT	: read id timeout
//		
//  Description:
//      Reads id from the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_read_id(uint8_t *p_id, uint32_t size)
{
	uint32_t id;
	uint32_t i;
	
	// set address
	REG32(QSPI_FLASHCMDADDR_REG) = 0;

	// issue the comman(0 address, 8 dummy cycle, 2 bytes read data)
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_RD_ID_CMD(QSPI_NAND_RD_ID, CMD_RW_BYTES_4, 8);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// get data
	id = REG32(QSPI_FLASHCMDRDDATALO_REG);

	for (i = 0; i < size; i++)
	{
		p_id[i] = (id >> (i*8)) & 0xFF;	
	}
		
	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_blk_erase(uint32_t addr)
//
//  Parameters:
//		addr					: flash address(block + page)
//
//  Return:
//		QSPI_ST_DONE		: block erases ok
//
//		QSPI_ST_FAIL			: block erases fail
//		
//		QSPI_ST_TIME_OUT	: block erases timeout
//
//  Description:
//      Erase a block of the flash.
//		
/*****************************************************************************/
E_QSPI_ST qspi_nand_blk_erase(uint32_t addr)
{	
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MICRON)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
	}
	else
	{
		// mxic(don't care)
	}
	
	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// set address
	REG32(QSPI_FLASHCMDADDR_REG) = addr;

	// issue write comman(3 bytes address, 0 byte dummy, 0 byte data)
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(QSPI_NAND_BE, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	if (QSPI_ST_DONE != qspi_nand_chk_st(&st))
	{
		return QSPI_ST_TIME_OUT;
	}

	if (st & (1 << QSPI_ST_ERS_FAIL_SHIFT))
	{
		return QSPI_ST_FAIL;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_page_write(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [in]		: data buffer which is to be writen to the flash
//
//		size				: the size of the data buffer 
//
//  Return:
//		QSPI_ST_DONE	: page writes ok
//
//		QSPI_ST_FAIL		: page writes  fail
//	
//		QSPI_ST_TIME_OUT	: page writes timeout
//
//  Description:
//      Write data to the address of the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_page_write(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t pp_load;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}

	// ecc enable
	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_EN))
	{
		return QSPI_ST_TIME_OUT;
	}
	
	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}
	
	// program load(2 bytes address, 0 byte dummy, x bytes)
	if (p_qspi_para->io_wmode == SIO_MODE)
	{
		pp_load = p_qspi_para->sio_pp_load_cmd;
	}
	else
	{
		pp_load = p_qspi_para->qio_pp_load_cmd;
	}
	
	qspi_nand_set_dwir(pp_load);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode	
	qspi_indirect_wr_setup(col, size);

	// dma data 
	dma_qspi_copy_m2p(DMA0, CH0, (uint32_t *)QSPI_BASE_ADDR, (uint32_t *)p_data, size, CH_PRIOR_0);
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_wr_chk_st();

	// program load random data(2 bytes address, 0 byte dummy, x bytes)
	qspi_nand_set_dwir(QSPI_NAND_PP_LOAD_RAND_X4);

	// write oob to the  ecc protect area
	MEM32(QSPI_BASE_ADDR + g_qspi_nand_oob_ofst[0]) = g_qspi_nand_oob[0];
	MEM32(QSPI_BASE_ADDR + g_qspi_nand_oob_ofst[1]) = g_qspi_nand_oob[1];	
	
	// program execute(3 bytes address, 0 byte dummy, 0 byte data)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_pp_exe_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	if (QSPI_ST_DONE != qspi_nand_chk_st(&st))
	{
		return QSPI_ST_TIME_OUT;
	}

	if (st & (1 << QSPI_ST_PGM_FAIL_SHIFT))
	{
		return QSPI_ST_FAIL;
	}

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_page_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr					: flash address(block + page)
//
//		p_data [out]			: data buffer which is to be read from the flash
//
//		size					: the size of the data buffer  
//
//  Return:
//		QSPI_ST_DONE		: page reads ok
//
//		QSPI_ST_FAIL			: page reads fail
//
//		QSPI_ST_TIME_OUT	: page reads timeout
//
//  Description:
//      Read data from the address of the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_page_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t ret_val;
	uint8_t rd_cache_cmd;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}	
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}	
	}
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}

	// ecc enable
	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_EN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}	

	// page read(3 bytes address, 0 byte dummy, 0 bytes)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_rd_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	ret_val = qspi_nand_chk_st(&st);
	if (QSPI_ST_DONE != ret_val)
	{
#ifdef EXTRA_DEBUG
		debug("\nqspi_nand_chk_st() failure ret_val=%x\n", ret_val);
#endif
		return QSPI_ST_TIME_OUT;
	}

	// random data read(2 bytes address, 1 byte dummy, x bytes)
	if (p_qspi_para->io_rmode == SIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->sio_rd_cache_cmd;
	}
	else if (p_qspi_para->io_rmode == DIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->dio_rd_cache_cmd;
	}
	else
	{
		rd_cache_cmd = p_qspi_para->qio_rd_cache_cmd;
	}

	qspi_nand_set_drir(DUMMY_CLKS_8, rd_cache_cmd);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	qspi_indirect_rd_setup(col, size);

	// dma data 
	dma_qspi_copy_p2m(DMA0, CH0, (uint32_t *)p_data, (uint32_t *)QSPI_BASE_ADDR, size, CH_PRIOR_0);	
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_rd_chk_st();

	// get oob from ecc protect area
	g_qspi_nand_oob[0] = MEM32(QSPI_BASE_ADDR + g_qspi_nand_oob_ofst[0]);
	g_qspi_nand_oob[1] = MEM32(QSPI_BASE_ADDR + g_qspi_nand_oob_ofst[1]);	

	if (((st >> QSPI_ST_ECC_SHIFT) & 3) >= 2)
	{
		return QSPI_ST_FAIL;
	}	
	
	return QSPI_ST_DONE;
}

E_QSPI_ST qspi_nand_page_read_without_oob(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t ret_val;
	uint8_t rd_cache_cmd;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}	
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}	
	}
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}

	// ecc enable
	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_EN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}	

	// page read(3 bytes address, 0 byte dummy, 0 bytes)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_rd_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	ret_val = qspi_nand_chk_st(&st);
	if (QSPI_ST_DONE != ret_val)
	{
#ifdef EXTRA_DEBUG
		debug("\nqspi_nand_chk_st() failure ret_val=%x\n", ret_val);
#endif
		return QSPI_ST_TIME_OUT;
	}

	// random data read(2 bytes address, 1 byte dummy, x bytes)
	if (p_qspi_para->io_rmode == SIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->sio_rd_cache_cmd;
	}
	else if (p_qspi_para->io_rmode == DIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->dio_rd_cache_cmd;
	}
	else
	{
		rd_cache_cmd = p_qspi_para->qio_rd_cache_cmd;
	}

	qspi_nand_set_drir(DUMMY_CLKS_8, rd_cache_cmd);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	qspi_indirect_rd_setup(col, size);

	// dma data 
	dma_qspi_copy_p2m(DMA0, CH0, (uint32_t *)p_data, (uint32_t *)QSPI_BASE_ADDR, size, CH_PRIOR_0);	
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_rd_chk_st();

	if (((st >> QSPI_ST_ECC_SHIFT) & 3) >= 2)
	{
		return QSPI_ST_FAIL;
	}	
	
	return QSPI_ST_DONE;
}


/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_param_page_read(uint16_t col, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		col					: flash column address
//
//		p_data [out]			: data buffer which is to store the data read from the flash
//	
//		size					: the size of the data buffer  
//
//  Return:
//		QSPI_ST_DONE		: parameter page reads ok
//
//		QSPI_ST_TIME_OUT	: parameter page reads timeout
//
//  Description:
//      Read parameter page from the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_param_page_read(uint16_t col, uint8_t *p_data, uint32_t size)
{
	uint8_t st;

	// secure otp enable and ECC disable	
	if (QSPI_ST_DONE != qspi_nand_otp_enter_ctrl(QSPI_OTP_E_EN))
	{
		return QSPI_ST_TIME_OUT;
	}

	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_DIS))
	{
		qspi_nand_otp_enter_ctrl(QSPI_OTP_E_DIS);
		return QSPI_ST_TIME_OUT;
	}

	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		qspi_nand_otp_enter_ctrl(QSPI_OTP_E_DIS);
		return QSPI_ST_TIME_OUT;
	}	

	// page read(3 bytes address, 0 byte dummy, 0 bytes)
	REG32(QSPI_FLASHCMDADDR_REG) = 0x01;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(QSPI_NAND_RD, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		qspi_nand_otp_enter_ctrl(QSPI_OTP_E_DIS);
		return QSPI_ST_TIME_OUT;
	}

	// check status
	if (QSPI_ST_DONE != qspi_nand_chk_st(&st))
	{	
		qspi_nand_otp_enter_ctrl(QSPI_OTP_E_DIS);	
		
		return QSPI_ST_TIME_OUT;
	}

	// random data read(2 bytes address, 1 byte dummy, x bytes)
	qspi_nand_set_drir(DUMMY_CLKS_8, 0x03);
	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	qspi_indirect_rd_setup(col, size);

	// dma data 
	dma_qspi_copy_p2m(DMA0, CH0, (uint32_t *)p_data, (uint32_t *)QSPI_BASE_ADDR, size, CH_PRIOR_0);	
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_rd_chk_st();

	// secure otp disable and ECC enable
	if (QSPI_ST_DONE != qspi_nand_otp_enter_ctrl(QSPI_OTP_E_DIS))
	{
		return QSPI_ST_TIME_OUT;
	}
	
	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  BOOL qspi_nand_ONFI_param_init(uint8_t *p_data, qspi_nand_para_t *p_para)
//
//  Parameters:
//		p_data [in]	: data buffer which is to store the paramete page from the flash
//	
//		p_para [out]	: data buffer which is to store the qspi nand parameter  
//
//  Return:
//		FALSE		: the onfi parameter is not valid
//
//		TRUE		: the onfi parameter is valid
//
//  Description:
//      Initialize the qspi nand parameter upon the parameter page from the flash.
//
/*****************************************************************************/
BOOL qspi_nand_ONFI_param_init(uint8_t *p_data, qspi_nand_para_t *p_para)
{
	uint16_t col, onfi_crc, cal_crc;
	uint8_t *p_byte = (uint8_t *)&cal_crc; 
	onfi_t *p_onfi = (onfi_t *)p_data;
	uint8_t i;

	// Orlando This is not in the flow diagram, but it would be
	// a good idea to verify the integrity CRC
	// of the ONFI Parameters page (offset 254 and 255).
	// In case of failure there are redundant parameter pages
	// beginning at offset 256 and 512 that could be used.
	for (col = 0, i = 0; i < ONFI_COPY_NUM; i++, col += ONFI_SIZE)
	{ 		
		// read parameter page from the flash
		if (QSPI_ST_DONE != qspi_nand_param_page_read(col, p_data, ONFI_SIZE))
		{
			continue;
		}
		
		onfi_crc = get_16_bits_val(p_onfi->crc);
		cal_crc = onfi_crc16_cpu_calc(p_data, (ONFI_SIZE - 2));
		if ((onfi_crc == cal_crc) || 
			(onfi_crc == ((p_byte[0]<<8) | p_byte[1])))
		{
			onfi_para_page_print((uint8_t *)p_data);
			break;
		}	
	}

	if (i >= ONFI_COPY_NUM)
	{
		return FALSE;
	}

	if (p_onfi->sig != ONFI_MARKER)
	{
		return FALSE;
	}
	
	// initiallize the qspi para from onfi para
	p_para->manu_id = p_onfi->manu_id;
	p_para->page_size = get_32_bits_val(p_onfi->bytes_per_page);
	p_para->pages_per_blk = get_32_bits_val(p_onfi->pages_per_blk);
	p_para->blks_per_lun = get_32_bits_val(p_onfi->blks_per_lun);
	p_para->spare_size = get_16_bits_val(p_onfi->bytes_per_spare);
	p_para->luns_per_chip = p_onfi->num_luns;

	// calculate upon the para
	p_para->blk_shift_bits = power(p_para->pages_per_blk);
	p_para->blks_per_chip = p_para->luns_per_chip * p_para->blks_per_lun;

	if (p_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		p_para->plane_sel = (p_para->blks_per_chip < 0x800) ?  QSPI_PLANE_NON_SEL : QSPI_PLANE_SEL;
	}
	else if (p_para->manu_id == QSPI_MANU_ID_MICRON)
	{
		p_para->plane_sel = (p_para->blks_per_chip < 0x800) ?  QSPI_PLANE_NON_SEL : QSPI_PLANE_SEL;
	}
	else
	{
		p_para->plane_sel = QSPI_PLANE_NON_SEL;
	}

	debug("manu_id:%x\n", p_para->manu_id);
	debug("page_size:%x\n", p_para->page_size);
	debug("pages_per_blk:%x\n", p_para->pages_per_blk);
	debug("blks_per_lun:%x\n", p_para->blks_per_lun);
	debug("spare_size: 0x%x\n", p_para->spare_size);
	debug("luns_per_chip:%x\n", p_para->luns_per_chip);
	debug("blks_per_chip:%x\n", p_para->blks_per_chip);
	debug("blk_shift_bits:%x\n", p_para->blk_shift_bits);

	return TRUE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_raw_data_write(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr				: flash address(block + page)
//
//		p_data [in]		: data buffer which is to be writen to the flash
//
//		size				: the size of the data buffer 
//
//  Return:
//		QSPI_ST_DONE	: page writes ok
//
//		QSPI_ST_FAIL		: page writes  fail
//	
//		QSPI_ST_TIME_OUT	: page writes timeout
//
//  Description:
//      Write data to the address of the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_raw_data_write(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t pp_load;
	uint32_t dma_size;	

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}
	}
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}	
	
	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_DIS))
	{
		return QSPI_ST_TIME_OUT;
	}
	
	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// program load(2 bytes address, 0 byte dummy, x bytes)
	if (p_qspi_para->io_wmode == SIO_MODE)
	{
		pp_load = p_qspi_para->sio_pp_load_cmd;
	}
	else
	{
		pp_load = p_qspi_para->qio_pp_load_cmd;
	}
	
	qspi_nand_set_dwir(pp_load);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	dma_size = p_qspi_para->page_size;
	qspi_indirect_wr_setup(col, dma_size);

	// dma data 
	dma_qspi_copy_m2p(DMA0, CH0, (uint32_t *)QSPI_BASE_ADDR, (uint32_t *)p_data, dma_size, CH_PRIOR_0);
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_wr_chk_st();

	// program random load(2 bytes address, 0 byte dummy, x bytes)
	if (p_qspi_para->io_wmode == SIO_MODE)
	{
		pp_load = p_qspi_para->sio_pp_load_rand_cmd;
	}
	else
	{
		pp_load = p_qspi_para->qio_pp_load_rand_cmd;
	}
	
	qspi_nand_set_dwir(pp_load);	

	// setup qspi indirect mode	
	dma_size = size - p_qspi_para->page_size;
	qspi_indirect_wr_setup((col + p_qspi_para->page_size), dma_size);

	// dma data 
	dma_qspi_copy_m2p(DMA0, CH0, (uint32_t *)QSPI_BASE_ADDR, (uint32_t *)(p_data + p_qspi_para->page_size), dma_size, CH_PRIOR_0);
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_wr_chk_st();

	// program execute(3 bytes address, 0 byte dummy, 0 byte data)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_pp_exe_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	if (QSPI_ST_DONE != qspi_nand_chk_st(&st))
	{
		return QSPI_ST_TIME_OUT;
	}

	if (st & (1 << QSPI_ST_PGM_FAIL_SHIFT))
	{
		return QSPI_ST_FAIL;
	}		

	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr					: flash address(block + page)
//
//		p_data [out]			: data buffer which is to be read from the flash
//
//		size					: the size of the data buffer  
//
//  Return:
//		QSPI_ST_DONE		: page reads ok
//
//		QSPI_ST_FAIL			: page reads fail
//
//		QSPI_ST_TIME_OUT	: page reads timeout
//
//  Description:
//      Read data from the address of the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t ret_val;
	uint8_t rd_cache_cmd;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}	
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}	
	}
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}	

	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_DIS))
	{
		return QSPI_ST_TIME_OUT;
	}
	
	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// page read(3 bytes address, 0 byte dummy, 0 bytes)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_rd_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	ret_val = qspi_nand_chk_st(&st);
	if (QSPI_ST_DONE != ret_val)
	{
#ifdef EXTRA_DEBUG
		debug("\nqspi_nand_chk_st() failure ret_val=%x\n", ret_val);
#endif
		return QSPI_ST_TIME_OUT;
	}

	// random data read(2 bytes address, 1 byte dummy, x bytes)
	if (p_qspi_para->io_rmode == SIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->sio_rd_cache_cmd;
	}
	else if (p_qspi_para->io_rmode == DIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->dio_rd_cache_cmd;
	}
	else
	{
		rd_cache_cmd = p_qspi_para->qio_rd_cache_cmd;
	}

	qspi_nand_set_drir(DUMMY_CLKS_8, rd_cache_cmd);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	qspi_indirect_rd_setup(col, size);

	// dma data 
	dma_qspi_copy_p2m(DMA0, CH0, (uint32_t *)p_data, (uint32_t *)QSPI_BASE_ADDR, size, CH_PRIOR_0);	
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_rd_chk_st();
	
	return QSPI_ST_DONE;
}

/*****************************************************************************/
//  Function:  E_QSPI_ST qspi_nand_spare_read(uint32_t addr, uint8_t *p_data, uint32_t size)
//
//  Parameters:
//		addr					: flash address(block + page)
//
//		p_data [out]			: data buffer which is to be read from the flash
//
//		size					: the size of the data buffer  
//
//  Return:
//		QSPI_ST_DONE		: spare data reads ok
//
//		QSPI_ST_FAIL			: spare data reads fail
//
//		QSPI_ST_TIME_OUT	: spare data reads timeout
//
//  Description:
//      Read spare data from the address of the flash.
//
/*****************************************************************************/
E_QSPI_ST qspi_nand_spare_read(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t ret_val;
	uint8_t rd_cache_cmd;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}	
	}
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}	

	col += p_qspi_para->page_size;

	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_DIS))
	{
		return QSPI_ST_TIME_OUT;
	}	

	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// page read(3 bytes address, 0 byte dummy, 0 bytes)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_rd_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	ret_val = qspi_nand_chk_st(&st);
	if (QSPI_ST_DONE != ret_val)
	{
#ifdef EXTRA_DEBUG
		debug("\nqspi_nand_chk_st() failure ret_val=%x\n", ret_val);
#endif
		return QSPI_ST_TIME_OUT;
	}

	// random data read(2 bytes address, 1 byte dummy, x bytes)
	if (p_qspi_para->io_rmode == SIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->sio_rd_cache_cmd;
	}
	else if (p_qspi_para->io_rmode == DIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->dio_rd_cache_cmd;
	}
	else
	{
		rd_cache_cmd = p_qspi_para->qio_rd_cache_cmd;
	}

	qspi_nand_set_drir(DUMMY_CLKS_8, rd_cache_cmd);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	qspi_indirect_rd_setup(col, size);

	// dma data 
	dma_qspi_copy_p2m(DMA0, CH0, (uint32_t *)p_data, (uint32_t *)QSPI_BASE_ADDR, size, CH_PRIOR_0);	
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_rd_chk_st();	
	
	return QSPI_ST_DONE;
}

// interrupt
E_QSPI_ST qspi_nand_page_read_1(uint32_t addr, uint8_t *p_data, uint32_t size)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t st;
	uint16_t col = 0;
	uint8_t ret_val;
	uint8_t rd_cache_cmd;

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}	
		
		if (addr >= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits))
		{
			addr -= (p_qspi_para->blks_per_lun<<p_qspi_para->blk_shift_bits);
		}
	}
	else if (p_qspi_para->manu_id == QSPI_MANU_ID_MXIC)
	{
		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}	
	}	
	else
	{
		// micron
		// select die
		if (QSPI_ST_DONE != qspi_nand_sel_die(addr))
		{
			return QSPI_ST_TIME_OUT;
		}

		// get the plane address if the plane selected
		if (p_qspi_para->plane_sel == QSPI_PLANE_SEL)
		{
			col = (((addr>>p_qspi_para->blk_shift_bits)&1)<<12);
		}		
	}

	if (QSPI_ST_DONE != qspi_nand_ecc_ctrl(QSPI_ECC_EN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// write enable
	if (QSPI_ST_DONE != qspi_nand_wr_ctrl(QSPI_NAND_CMD_WREN))
	{
		return QSPI_ST_TIME_OUT;
	}

	// page read(3 bytes address, 0 byte dummy, 0 bytes)
	REG32(QSPI_FLASHCMDADDR_REG) = addr;
	REG32(QSPI_FLASHCMD_REG) = QSPI_NAND_ISSUE_ERASE_CMD(p_qspi_para->io_rd_cmd, ADDR_BYTES_3, 0);
	if (QSPI_ST_DONE != qspi_cmd_chk_st())
	{
		return QSPI_ST_TIME_OUT;
	}

	// check status
	ret_val = qspi_nand_chk_st(&st);
	if (QSPI_ST_DONE != ret_val)
	{
#ifdef EXTRA_DEBUG
		debug("\nqspi_nand_chk_st() failure ret_val=%x\n", ret_val);
#endif
		return QSPI_ST_TIME_OUT;
	}

	// random data read(2 bytes address, 1 byte dummy, x bytes)
	if (p_qspi_para->io_rmode == SIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->sio_rd_cache_cmd;
	}
	else if (p_qspi_para->io_rmode == DIO_MODE)
	{
		rd_cache_cmd = p_qspi_para->dio_rd_cache_cmd;
	}
	else
	{
		rd_cache_cmd = p_qspi_para->qio_rd_cache_cmd;
	}

	qspi_nand_set_drir(DUMMY_CLKS_8, rd_cache_cmd);

	qspi_nand_set_dsr(ADDR_BYTES_2, 0x800, 0x10);

	// setup qspi indirect mode
	qspi_indirect_rd_setup_1(col, size);

	// dma data 
	dma_qspi_copy_p2m(DMA0, CH0, (uint32_t *)p_data, (uint32_t *)QSPI_BASE_ADDR, size, CH_PRIOR_0);	
	dma_chk_st(DMA0, CH0);	

	// check indirect process done
	qspi_indirect_rd_chk_st();

	if (((st >> QSPI_ST_ECC_SHIFT) & 3) >= 2)
	{
		return QSPI_ST_FAIL;
	}	
	
	return QSPI_ST_DONE;
}


