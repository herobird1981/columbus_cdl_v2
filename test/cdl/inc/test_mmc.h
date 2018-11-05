/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_MMC_H__
#define __TEST_MMC_H__
#include "test_config.h"
#include "cdl_mmc.h"

#if (CONFIG_MMC_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry mmc_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
int mmc_test_prepare(int);

#else
#define mmc_test_menu 		NULL
#define mmc_test_prepare	NULL
#endif

bool mmc_reset_test(E_SDIO_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val);

bool mmc_cg_test(E_SDIO_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif
