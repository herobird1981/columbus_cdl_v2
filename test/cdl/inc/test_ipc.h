/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __TEST_IPC_H__
#define __TEST_IPC_H__
#include "test_config.h"
#include "cdl_ipc.h"
#include <hw_interrupt.h>

#if (CONFIG_IPC_TEST == 1)

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern cmdline_entry ipc_test_menu[];

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
int ipc_test_prepare(int sel);

#else
#define ipc_test_menu		NULL
#define ipc_test_prepare	NULL
#endif

bool ipc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val);
bool ipc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val);

#endif

