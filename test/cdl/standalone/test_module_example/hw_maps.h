/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __COLUMBUS_HW_MAPS__
#define __COLUMBUS_HW_MAPS__

#define UART0_BASE	0x50000000
#define UART1_BASE	0x50001000
#define UART2_BASE	0x50002000
#define UART3_BASE	0x50003000

#define SPI0_BASE 	0x50004000	//QSPI
#define SPI1_BASE 	0x50005000
#define SPI2_BASE 	0x50006000

#define I2C0_BASE 	0x50007000
#define I2C1_BASE 	0x50008000
#define I2C2_BASE 	0x50009000

#define GPIO_BASE 	0x52001000
#define TIMER_BASE	0x52002000
#define WDT_BASE		0x52003000	//WATCH DOG TIMER

#define USB0_BASE 	0x70100000	// USB 0 Controller
#define USB1_BASE 	0x70200000	// USB 1 Controller
#define DMAC_BASE 	0x70300000

#define SD0_BASE		0x0
#define SD1_BASE		0x0

#endif