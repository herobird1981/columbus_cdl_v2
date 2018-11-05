/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _QSPI_NAND_H_
#define _QSPI_NAND_H_
#include "cdl_config.h"
#include "cdl_onfi.h"
#include "qspi.h"
#include "cdl_qspi_nand_cmd.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define QSPI_BOOT_CLK_DIV	0x05	// 25MHZ from 300MHZ clock

// Plane select when issuing read and write command
typedef enum
{
	QSPI_PLANE_NON_SEL  = 0,	// plane select when issuing read and write command 				
	QSPI_PLANE_SEL  	= 1 	// plane non-select when issuing read and write command
}E_QSPI_PLANE_SEL;

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
// QSPI NAND Parameter
typedef struct
{
	uint8_t manu_id;				// manufacture vendor	
									//	= QSPI_MANU_ID_MXIC
									//	= QSPI_MANU_ID_WINBOND
									//	= QSPI_MANU_ID_MICRON
	
	uint16_t page_size;				// page size

	uint16_t pages_per_blk;			// the page number per block

	uint16_t blks_per_lun;			// the block number per lun

	uint16_t spare_size;			// spare area size

	uint8_t luns_per_chip;			// the lun number per device

	uint8_t blk_shift_bits; 		// block shift bits(relative to page, numPagePerBlock = 128, blkShiftBits = 7)

	uint8_t plane_sel;				// plane select when issuing read and write command
									// = QSPI_PLANE_NON_SEL
									// = QSPI_PLANE_SEL									
	
	uint8_t io_wmode;
									// = SIO_MODE
									// = DIO_MODE
									// = QIO_MODE										

	uint8_t io_rmode;
									// = SIO_MODE
									// = DIO_MODE
									// = QIO_MODE
									
	// write command
	uint8_t sio_pp_load_cmd;

	uint8_t sio_pp_load_rand_cmd;

	uint8_t qio_pp_load_cmd;

	uint8_t qio_pp_load_rand_cmd;

	uint8_t io_pp_exe_cmd;

	// read command
	uint8_t io_rd_cmd;
	
	uint8_t sio_rd_cache_cmd;

	uint8_t dio_rd_cache_cmd;

	uint8_t qio_rd_cache_cmd;
	
	uint8_t revd[2];

	uint32_t blks_per_chip;			// the block number per chip
									//	= luns_per_chip * blks_per_lun

	uint32_t time_out;				// uint(10 us)	

	uint32_t pclk; // input clock freq
	uint32_t max_hz; // supported max data rate

	uint32_t tshsl_ns;
	uint32_t tsd2d_ns;
	uint32_t tchsh_ns;
	uint32_t tslch_ns;		
}qspi_nand_para_t;
typedef qspi_nand_para_t * qspi_nand_para_t_ptr;

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern qspi_nand_para_t g_qspi_nand_para;
extern uint32_t g_qspi_nand_oob[2];
extern uint16_t g_qspi_nand_oob_ofst[2];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
uint8_t qspi_calibration(uint32_t hz);

uint8_t qspi_nand_quad_enable(void);

uint8_t qspi_nand_blk_protect_ctrl(uint8_t bp);

uint8_t qspi_get_brdiv(uint32_t ref_clk_hz, uint32_t sclk_hz);

void qspi_nand_init(uint8_t div, E_QSPI_PHASE phase, uint8_t cs);

E_QSPI_ST qspi_nand_set_feature(uint8_t addr, uint8_t data);

E_QSPI_ST qspi_nand_get_feature(uint8_t addr, uint8_t *p_data);

E_QSPI_ST qspi_nand_reset(void);

E_QSPI_ST qspi_nand_read_id(uint8_t *p_id, uint32_t size);

E_QSPI_ST qspi_nand_blk_erase(uint32_t addr);

E_QSPI_ST qspi_nand_page_write(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_page_read(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_param_page_read(uint16_t col, uint8_t *p_data, uint32_t size);

BOOL qspi_nand_ONFI_param_init(uint8_t *p_data, qspi_nand_para_t *p_para);

E_QSPI_ST qspi_nand_raw_data_write(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_spare_read(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_page_read_1(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_page_read_without_oob(uint32_t addr, uint8_t *p_data, uint32_t size);

E_QSPI_ST qspi_nand_sel_die(uint32_t addr);

uint8_t qspi_nand_buf_mode_ctrl(E_QSPI_BUF_MODE buf_mode);

#endif

