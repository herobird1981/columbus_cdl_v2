/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __TEST_CONFIG_H__
#define __TEST_CONFIG_H__

#include "hw_maps.h"

#include "std_common.h"
#include "console.h"

/*macro defined*/
#define NULL 0
#define debug  console_printf
#define typeof __typeof__

/*default bus clk*/
#define BSP_BUS_PCLK	(50000000)

/*console uart select*/
#define CONSOLE_BASE	UART0_BASE
#define CONSOLE_PCLK	BSP_BUS_PCLK

/*use GIC driver or not*/
#define CONFIG_DRIVERS_GIC	1

#define CONFIG_DMA_TEST 1

#endif