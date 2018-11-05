/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_AES_H__
#define __TEST_AES_H__
#include "test_config.h"
#include "cdl_aes.h"
#include "tomcrypt.h"

#if (CONFIG_AES_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry aes_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/

#else
#define aes_test_menu 		NULL
#endif

bool aes_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

bool aes_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif

