/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_OTP_H__
#define __TEST_OTP_H__
#include "test_config.h"
#include "cdl_otp.h"

#if (CONFIG_OTP_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry otp_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/

#else
#define otp_test_menu 		NULL
#endif

bool otp_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

bool otp_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif

