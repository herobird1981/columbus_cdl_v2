/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2017 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_DWC_TRNG_H__
#define __TEST_DWC_TRNG_H__
#include "test_config.h"
#include "cdl_dwc_trng.h"

#if (CONFIG_DWC_TRNG_TEST == 1)
extern cmdline_entry dwc_trng_test_menu[];

#else
#define dwc_trng_test_menu 			NULL
#define dwc_trng_test_prepare 		NULL
#endif

bool dwc_trng_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val);
bool dwc_trng_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif

