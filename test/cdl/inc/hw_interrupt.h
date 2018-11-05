/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __HW_INTERRUPT_H__
#define __HW_INTERRUPT_H__

// [0, 31] for A7 internal use
#define INT_AES0		(32)
#define INT_ECC0		(33)
#define INT_SHA0		(34)
#define INT_SDIO0		(35)
#define INT_SDIO1		(36)
#define INT_USB0		(37)
#define INT_USB0_DMA	(38)
#define INT_USB1		(39)
#define INT_USB1_DMA	(40)
#define INT_NFC			(41)
#define INT_DMA0		(42)
#define INT_DMA1		(43)
#define INT_DMA2		(44)
#define INT_DDR			(45)
#define INT_WDT			(46)

#define INT_TIMER0		(47)
#define INT_TIMER1		(48)
#define INT_TIMER2		(49)
#define INT_TIMER3		(50)
#define INT_TIMER4		(51)
#define INT_TIMER5		(52)
#define INT_TIMER6		(53)
#define INT_TIMER7		(54)

#define INT_I2C0		(55)
#define INT_I2C1		(56)
#define INT_I2C2		(57)

#define INT_GPIOA0		(58)
#define INT_GPIOA1		(59)
#define INT_GPIOB0		(60)
#define INT_GPIOC0		(61)
#define INT_GPIOD0		(62)

#define INT_ADC			(63)	// SAR ADC interrupt

#define INT_SPI0		(64)
#define INT_SPI1		(65)
#define INT_SPI2		(66)

#define INT_UART0		(67)
#define INT_UART1		(68)
#define INT_UART2		(69)
#define INT_UART3		(70)

#define INT_OTP			(71)	
#define INT_RTC			(72)	// BBU RTC interrupt
#define INT_TAMPER		(73)	// BBU Tamper interrupt
#define INT_PMU			(74)	// PMU (for SW housekeeping notification in EPF mode)

#define INT_RESV4		(75)	// AXI Error Interrupt for A7 core that doesn't cause an abort exception.
#define INT_IPC_RF0		(76)	// IPC_RF_to_A7[0] interrupt signal.
#define INT_IPC_RF1		(77)	// IPC_RF_to_A7[1] interrupt signal
#define INT_IPC_RF2		(78)	// IPC_RF_to_A7[2] interrupt signal
#define INT_IPC_RF3		(79)	// IPC_RF_to_A7[3] interrupt signal
#define INT_IPC_RF4		(80)	// IPC_RF_to_A7[4] interrupt signal
#define INT_IPC_RF5		(81)	// IPC_RF_to_A7[5] interrupt signal
#define INT_IPC_RF6		(82)	// IPC_RF_to_A7[6] interrupt signal
#define INT_IPC_RF7		(83)	// IPC_RF_to_A7[7] interrupt signal

#define INT_IPC_PLC0	(84)	// IPC_PLC_to_A7[0] interrupt signal
#define INT_IPC_PLC1	(85)	// IPC_PLC_to_A7[1] interrupt signal
#define INT_IPC_PLC2	(86)	// IPC_PLC_to_A7[2] interrupt signal
#define INT_IPC_PLC3	(87)	// IPC_PLC_to_A7[3] interrupt signal
#define INT_IPC_PLC4	(88)	// IPC_PLC_to_A7[4] interrupt signal
#define INT_IPC_PLC5	(89)	// IPC_PLC_to_A7[5] interrupt signal
#define INT_IPC_PLC6	(90)	// IPC_PLC_to_A7[6] interrupt signal
#define INT_IPC_PLC7	(91)	// IPC_PLC_to_A7[7] interrupt signal

#define INT_GPIOB1		(92)	// GPIOB int1
#define INT_GPIOC1		(93)	// GPIOC int1
#define INT_GPIOD1		(94)	// GPIOD int1
#define INT_DEBUG		(95)	// Coresight Debug Interrupt

#ifdef TAPEOUT_V2
#define INT_IPC_PLCACK0 (96)
#define INT_IPC_PLCACK1 (97)
#define INT_IPC_PLCACK2 (98)
#define INT_IPC_PLCACK3 (99)
#define INT_IPC_PLCACK4 (100)
#define INT_IPC_PLCACK5 (101)
#define INT_IPC_PLCACK6 (102)
#define INT_IPC_PLCACK7 (103)

#define INT_IPC_RFACK0  (104)
#define INT_IPC_RFACK1  (105)
#define INT_IPC_RFACK2  (106)
#define INT_IPC_RFACK3  (107)
#define INT_IPC_RFACK4  (108)
#define INT_IPC_RFACK5  (109)
#define INT_IPC_RFACK6  (110)
#define INT_IPC_RFACK7  (111)

#define INT_DWC_TRNG   (112)
#endif

#endif

