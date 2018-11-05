/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _QSPI_H_
#define _QSPI_H_
#include "cdl_config.h"
#include "cdl_qspi_reg.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define QSPI_TIME_OUT 10000 // 100ms

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/


/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
void qspi_set_drir(uint8_t dummy_clks, uint8_t data_io_mode, uint8_t addr_io_mode, uint8_t cmd_rd);

void qspi_set_dwir(uint8_t data_io_mode, uint8_t addr_io_mode, uint8_t cmd_wr);

void qspi_set_ddr(uint8_t dsda, uint8_t csdads, uint8_t cseot, uint8_t cssot);

void qspi_set_dsr(uint8_t addr_bytes, uint16_t page_bytes, uint8_t blk_bytes);

void qspi_set_cfgr(uint8_t br_div, E_QSPI_PHASE phase, uint8_t cs);

uint8_t qspi_indirect_wr_chk_st(void);

uint8_t qspi_indirect_rd_chk_st(void);

void qspi_indirect_wr_setup(uint32_t addr, uint32_t size);

void qspi_indirect_rd_setup(uint32_t addr, uint32_t size);

uint8_t qspi_cmd_chk_st(void);

uint32_t qspi_read_int_st(void);

void qspi_clr_int_st(uint32_t val);

void qspi_set_int_mask(uint32_t val);

void qspi_indirect_rd_setup_1(uint32_t addr, uint32_t size);

void qspi_set_clk_mode(E_QSPI_PHASE phase);

void qspi_set_access_mode(E_QSPI_MODE mode);

#endif

