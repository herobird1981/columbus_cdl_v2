/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_I2C_H__
#define __TEST_I2C_H__
#include "test_config.h"
#include "cdl_i2c.h"

#if (CONFIG_I2C_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry i2c_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
int i2c_test_prepare(int port);
int i2c_test_cleanup(int port);

#else
#define i2c_test_menu 		NULL
#define i2c_test_prepare 	NULL
#define i2c_test_cleanup 	NULL
#endif

bool i2c_reset_test(E_I2C_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val);

bool i2c_cg_test(E_I2C_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif

