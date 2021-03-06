/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_QSPI_H__
#define __TEST_QSPI_H__
#include "test_config.h"
#include "cdl_qspi_nand.h"
#include "cdl_qspi_nand_bbt.h"

#if (CONFIG_QSPI_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry qspi_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/


#else
#define qspi_test_menu 		NULL
#endif

bool qspi_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

bool qspi_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif

