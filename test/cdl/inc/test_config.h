/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __TEST_CONFIG_H__
#define __TEST_CONFIG_H__

#include "test_types.h"

#include "hw_maps.h"
#include "hw_interrupt.h"
#include "hw_dmac.h"

#include "cdl_gic.h"
#include "cdl_dmac.h"

#include "std_common.h"
#include "console.h"
#include "timer.h"
#include "interrupt.h"

#include "test_utils.h"
#include "cdl_sys.h"

#define DEBUG_OUTPUT

#ifdef 	DEBUG_OUTPUT
#define debug(...) console_printf (__VA_ARGS__)
#else
#define debug(...)
#endif

#define info(...)	console_printf(__VA_ARGS__)

/*console uart select*/
#define CONSOLE_BASE				UART0_BASE

/*use GIC driver or not*/
#define CONFIG_DRIVERS_GIC	1

/*Add nand_flash_detect_onfi implementation*/
#define CONFIG_SYS_NAND_ONFI_DETECTION

/* include test device list*/
#define CONFIG_OTP_TEST		1
#define CONFIG_CRC_TEST		1

#ifdef TAPEOUT_V2
#define CONFIG_RNG_TEST		0
#define CONFIG_DWC_TRNG_TEST		1
#else
#define CONFIG_RNG_TEST		1
#define CONFIG_DWC_TRNG_TEST		0
#endif
#define CONFIG_ECC_TEST		1
#define CONFIG_AES_TEST		1
#define CONFIG_SHA_TEST		1
#define CONFIG_IPC_TEST		1
#define CONFIG_EPF_TEST		1

#define CONFIG_TIMER_TEST	1
#define CONFIG_WDT_TEST		1
#define CONFIG_RTC_TEST		1
#define CONFIG_TAMP_TEST	1
#define CONFIG_GIC_TEST		1

#define CONFIG_I2C_TEST		1
#define CONFIG_DMA_TEST		1
#define CONFIG_GPIO_TEST 	1

#define CONFIG_UART_TEST	1
#define CONFIG_QSPI_TEST	1
#define CONFIG_SPI_TEST		1
#define CONFIG_MMC_TEST		1
#define CONFIG_NAND_TEST	1
#define CONFIG_USB_TEST		1

#define CONFIG_API_TEST     1
#define DHRYSTONE_TEST		1 // only evb and must enable dcache

#if defined(EVB_BOARD)
#define CONFIG_ADC_TEST     1
#define CONFIG_PLL_TEST     1
#define CONFIG_DDR_TEST     1
#else
#define CONFIG_ADC_TEST     0
#define CONFIG_PLL_TEST     0
#define CONFIG_DDR_TEST     0
#endif

#define CONFIG_SYS_TEST     1
#define CONFIG_BBURAM_TEST  1
#endif

