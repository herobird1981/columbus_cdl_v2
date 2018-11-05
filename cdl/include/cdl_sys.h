/*--------------------------------------------------------------------------------------------------*/
/*                                                                                                											*/
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                							*/
/*                                                                                                											*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef __CDL_SYS_H__
#define __CDL_SYS_H__

#include "cdl_pll.h"
#include "cdl_dmac.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// PRCM Registter Map

// ARM Reset Control Registers
#define GLOBAL_SOFT_RST_REG			((uint32_t)p_prcm->regs + 0x0010)	// RW, Software Global Reset register
#define ARM_SOFT_RST_REG			((uint32_t)p_prcm->regs + 0x0014)	// RW, ARM subsystem software Reset register
#define ARM_BLK_RST_REG				((uint32_t)p_prcm->regs + 0x0018)	// RW, ARM block Reset register
#define ARM_DSP_SUB_RST_CFG_REG		((uint32_t)p_prcm->regs + 0x001C)	// RW, DSP sub reset configuration register from ARM sub.

// ARM Clock Gating Control Registers
#define SYS_BLK_CG_REG				((uint32_t)p_prcm->regs + 0x0020)	// RW, ARM clock gate off register

// ARM Power Gating Control Register
#define SYS_ARM_PWD_REG				((uint32_t)p_prcm->regs + 0x0030)	// RW, power domain register
#define SYS_FAILSAFE_TIMER_REG		((uint32_t)p_prcm->regs + 0x0034)	// RW, Failsafe timer threshold register
#define SYS_ARM_PMU_INT_STATUS_REG	((uint32_t)p_prcm->regs + 0x0038)	// RW, EPF high to low transition interrupt status register for ARM subsystem

// Boot Control Registers
#define SYS_BOOT_ST_REG				((uint32_t)p_prcm->regs + 0x0040)	// RO,    0x0040, Boot Control status Registers
#define SYS_BOOT_MODE_REG			((uint32_t)p_prcm->regs + 0x0044)	// RO,     0x0044, Boot Control mode Registers

/*************************************************************************************************************/
// Chip ID Registers
#define ARM_CHIP_ID_REG				((uint32_t)p_misc->regs + 0x0060)	// RO,   ARM Chip ID Registers

// MISC Registter Map
#define ARM_TIMER_OUTPUT_REG		((uint32_t)p_misc->regs + 0x0070)	// RW,   ARM Functionality Auxilary Register

// ARM DMA Handshaking  External  INT  Source Selection
#define ARM_DMA_HS_EXT_INT_SRC_SEL_REG	((uint32_t)p_misc->regs + 0x0094)	// RW,   ARM DMA Handshaking  External  INT  Source Selection

// ARM Sharing Controller Register
#define ARM_PINMUX_SD0_REG			((uint32_t)p_misc->regs + 0x00A0)	// RW, SD0_PINMUX(sdio0_iodis)  register
#define ARM_PINMUX_SD1_REG			((uint32_t)p_misc->regs + 0x00A4)	// RW, SD1_PINMUX(sdio1_iodis)  register
#define ARM_PINMUX_UART_REG			((uint32_t)p_misc->regs + 0x00A8)	// RW, ARM_UART_PINMUX register
#define ARM_PINMUX_SPI_REG			((uint32_t)p_misc->regs + 0x00AC)	// RW, ARM_SPI_PINMUX register
#define ARM_PINMUX_ITS_REG			((uint32_t)p_misc->regs + 0x00B0)	// RW, ARM_ITS_PINMUX(I2C&TIMER&SYSTEM) register

// ARM IO Pull Up/Pull Down Configuration 
#define ARM_SD_PU_REG				((uint32_t)p_misc->regs + 0x00C0)	// RW, ARM_SD_PU register
#define ARM_UART_PU_REG				((uint32_t)p_misc->regs + 0x00C4)	// RW, ARM_UART_PU register
#define ARM_SPI_I2C_PU_REG			((uint32_t)p_misc->regs + 0x00C8)	// RW, ARM_SPI_I2C_PU register
#define ARM_PU_REG					((uint32_t)p_misc->regs + 0x00CC)	// RW, ARM_PU(GPIOB,TIMER,SYSTEM) register

// ARM IO Pull Disable Register
#define ARM_SD_PD_REG				((uint32_t)p_misc->regs + 0x00D0)	// RW, ARM_SD_PD register
#define ARM_UART_PD_REG				((uint32_t)p_misc->regs + 0x00D4)	// RW, ARM_UART_PD register
#define ARM_SPI_I2C_PD_REG			((uint32_t)p_misc->regs + 0x00D8)	// RW, ARM_SPI_I2C_PD register
#define ARM_PD_REG					((uint32_t)p_misc->regs + 0x00DC)	// RW, ARM_PD(GPIOB,TIMER,SYSTEM) register

// ARM IO Driving Strength Select Register
#define ARM_UART_I2C_DS_REG			((uint32_t)p_misc->regs + 0x00E0)	// RW, ARM_UART_I2C_DS register

// ARM IO Open Drain Configuration Register
// (Only valid when the pin is as general purpose input/output. It is not valid when the pin is as function IP pin such UART)
#define ARM_SD_OD_REG				((uint32_t)p_misc->regs + 0x00F0)	// RW, ARM_SD_OD register
#define ARM_UART_OD_REG				((uint32_t)p_misc->regs + 0x00F4)	// RW, ARM_UART_OD register
#define ARM_SPI_I2C_OD_REG			((uint32_t)p_misc->regs + 0x00F8)	// RW, ARM_SPI_I2C_OD register
#define ARM_OD_REG					((uint32_t)p_misc->regs + 0x00FC)	// RW, ARM_OD(GPIOB,TIMER,SYSTEM) register

// ARM IO Output Disable Register
// (The registers are only valid in ARM sleep mode)
#define ARM_SD_OUTDIS_REG			((uint32_t)p_misc->regs + 0x0100)	// RW, ARM_SD_OUTDIS register
#define ARM_UART_OUTDIS_REG			((uint32_t)p_misc->regs + 0x0104)	// RW, ARM_UART_OUTDIS register
#define ARM_SPI_I2C_OUTDIS_REG		((uint32_t)p_misc->regs + 0x0108)	// RW, ARM_SPI_I2C_OUTDIS register
#define ARM_OUTDIS_REG				((uint32_t)p_misc->regs + 0x010C)	// RW, ARM_OUTDIS(GPIOB,TIMER,SYSTEM) register

// ARM IO Output Value Register
// (The registers are only valid in ARM sleep mode)
#define ARM_SD_OV_REG				((uint32_t)p_misc->regs + 0x0110)	// RW, ARM_SD_OV register
#define ARM_UART_OV_REG				((uint32_t)p_misc->regs + 0x0114)	// RW, ARM_UART_OV register
#define ARM_SPI_I2C_OV_REG			((uint32_t)p_misc->regs + 0x0118)	// RW, ARM_SPI_I2C_OV register
#define ARM_OV_REG					((uint32_t)p_misc->regs + 0x011C)	// RW, ARM_OV(GPIOB,TIMER,SYSTEM) register

#ifdef TAPEOUT_V2
#define ARM_DMA_ERR_RESP_EN         ((uint32_t)p_misc->regs + 0x0160)   // [2:0], bit 0 -- dmac0, bit 1 -- dmac1, bit2 -- dmac2 
#define ARM_DMA0_TIMEOUT_CNT        ((uint32_t)p_misc->regs + 0x0164)   // [31:0], ahb clk unit
#define ARM_DMA1_TIMEOUT_CNT        ((uint32_t)p_misc->regs + 0x0168)   // [31:0], ahb clk unit
#define ARM_DMA2_TIMEOUT_CNT        ((uint32_t)p_misc->regs + 0x016c)   // [31:0], ahb clk unit

#define ARM_WDT_RSTN_IODIS        	((uint32_t)p_misc->regs + 0x01B0)

#define ARM_DDR_SCRAM_UPDATE        ((uint32_t)p_misc->regs + 0x01c0)
#define ARM_TRNG_EXT_CTRL	        ((uint32_t)p_misc->regs + 0x01c4)
#define ARM_RNG_SEL					((uint32_t)p_misc->regs + 0x01c8)

// RNG SEL
typedef enum
{
	BRITE_RNG           = 0,
	DWC_TRNG           = 1
}E_TRNG_SEL;

#endif


/*************************************************************************************************************/
// UART Port Number
typedef enum
{
	UART0           = 0,
	UART1           = 1,
	UART2           = 2,
	UART3           = 3,
	NUM_UART_PORTS  = 4
}E_UART_PORT;

// SDIO Port Number
typedef enum
{
	SDIO0           = 0,
	SDIO1           = 1,
	NUM_SDIO_PORTS  = 2
}E_SDIO_PORT;

// SPI Port Number
typedef enum
{
	SPI0           = 0,
	SPI1           = 1,
	SPI2           = 2,
	NUM_SPI_PORTS  = 3
}E_SPI_PORT;

// I2C Port Number
typedef enum
{
	I2C0           = 0,
	I2C1           = 1,
	I2C2           = 2,
	NUM_I2C_PORTS  = 3
}E_I2C_PORT;

// GPIO Port Number
typedef enum
{
	GPIOA           = 0,
	GPIOB           = 1,
	GPIOC           = 2,
	GPIOD           = 3,
	NUM_GPIO_PORTS  = 4
}E_GPIO_PORT;

// GPIO Pin Number
typedef enum
{
	PIN_0           = 0,
	PIN_1           = 1,
	PIN_2           = 2,
	PIN_3           = 3,
	PIN_4           = 4,
	PIN_5           = 5,
	PIN_6           = 6,
	PIN_7           = 7,
	PIN_8           = 8,
	PIN_9           = 9,
	PIN_10          = 10,
	PIN_11          = 11,
	PIN_12          = 12,
	PIN_13          = 13,
	PIN_14          = 14,
	PIN_15          = 15,
	PIN_16          = 16,
	PIN_17          = 17,
	PIN_18          = 18,
	PIN_19          = 19,
	PIN_20          = 20,
	PIN_21          = 21,
	PIN_22          = 22,
	PIN_23          = 23,
	PIN_24          = 24,
	PIN_25          = 25,
	PIN_26          = 26,
	PIN_27          = 27,
	PIN_28          = 28,
	PIN_29          = 29,
	PIN_30          = 30,
	PIN_31          = 31,	
	NUM_PIN_PORTS  	= 32
}E_PIN_PORT;

// Timer Port Number
typedef enum
{
	TIMER0           = 0,
	TIMER1           = 1,
	TIMER2           = 2,
	TIMER3           = 3,
	TIMER4           = 4,
	TIMER5           = 5,
	TIMER6           = 6,
	TIMER7           = 7,
	NUM_TIMER_PORTS  = 8
}E_TIMER_PORT;

// Timer W/X/Y/Z Port Number
typedef enum
{
	TIMER_W           		= 0,
	TIMER_X           		= 1,
	TIMER_Y           		= 2,
	TIMER_Z           		= 3,
	NUM_TIMER_WXYZ_PORTS  	= 4
}E_TIMER_WXYZ_PORT;

// UART Port Number
typedef enum
{
	USB0           = 0,
	USB1           = 1,
	NUM_USB_PORTS  = 2
}E_USB_PORT;

// offset: 0x0010, GLOBAL_SOFT_RST register
// Write 0xAA55A5A5 to this register will cause the global reset. When enable, it resets whole chip except clock generate and related control registers.
// Write other value will take no effect
#define GLOVAL_SOFT_RST_VAL		0xAA55A5A5

// offset: 0x0014, ARM_SOFT_RST register
// Bit [31:0]: ARM subsystem software Reset.
// Write 0xAA55A5A5 to this register will cause the ARM subsystem reset. When enable, it resets whole ARM subsystem.
// Write other value will take no effect.
#define ARM_SUBSYS_SOFT_RST_VAL	0xAA55A5A5

// offset: 0x0018, ARM_BLK_RST register
typedef enum
{
	
	ARM_NFC_RST_SHIFT 	= 0, 		// Bit [0]: NFC module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_SDMMC0_RST_SHIFT = 1, 		// Bit [1]: SDMMC0 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_SDMMC1_RST_SHIFT = 2,		// Bit [2]: SDMMC1 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_DMA0_RST_SHIFT = 3,			// Bit [3]: DMA0 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_DMA1_RST_SHIFT = 4,			// Bit [4]: DMA1 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_DMA2_RST_SHIFT = 5,			// Bit [5]: DMA2 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_QSPI_RST_SHIFT = 6,			// Bit [6]: QSPI module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_CRC_RST_SHIFT = 7,			// Bit [7]: CRC module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_ECC_RST_SHIFT = 8,			// Bit [8]: ECC module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_SHA_RST_SHIFT = 9,			// Bit [9]: SHA module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_AES_RST_SHIFT = 10,			// Bit [10]: AES module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_OTP_CTRL_RST_SHIFT = 11,	// Bit [11]: OTP_CTRL module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_IPC_RST_SHIFT = 12,			// Bit [12]: IPC module reset. Default: 1.
									// 1: Reset.
									// 0: Release.

	ARM_DEBUG0_RST_SHIFT = 13,		// Bit [13]: DEBUG module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_DEBUG1_RST_SHIFT = 14,		// Bit [14]: DEBUG module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_I2C0_RST_SHIFT = 15,		// Bit [15]: I2C0 module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_I2C1_RST_SHIFT = 16,		// Bit [16]: I2C1 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_I2C2_RST_SHIFT = 17,		// Bit [17]: I2C2 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_SPI0_RST_SHIFT = 18,		// Bit [18]: SPI0 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_SPI1_RST_SHIFT = 19,		// Bit [19]: SPI1 module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_SARADC_CTRL_RST_SHIFT = 20,	// Bit [20]: SARADC_CTRL module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_UART0_RST_SHIFT = 21,		// Bit [21]: UART0 module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_UART1_RST_SHIFT = 22,		// Bit [22]: UART1 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_UART2_RST_SHIFT = 23,		// Bit [23]: UART2 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_UART3_RST_SHIFT = 24,		// Bit [24]: UART3 module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_WDT_RST_SHIFT = 25,			// Bit [25]: WDT module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_TIMER_RST_SHIFT = 26,		// Bit [26]: TIMER module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_GPIO_RST_SHIFT = 27,		// Bit [27]: GPIO module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_RNG_RST_SHIFT = 28,			// Bit [28]: RNG module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_USBOTG0_RST_SHIFT = 29,		// Bit [29]: USBOTG0 module reset. Default: 1
									// 0: Reset.
									// 1: Release.

	ARM_USBOTG1_RST_SHIFT = 30,		// Bit [30]: USBOTG1 module reset. Default: 1.
									// 0: Reset.
									// 1: Release.

	ARM_DDR_CTRL_RST_SHIFT = 31		// Bit [31]: DDR_CTRL module reset. Default: 1
									// 0: Reset.
									// 1: Release.
}E_ARM_BLK_RST_SHIFT;

// offset: 0x001C, DSP_RST_CFG register(DSP sub reset configuration register from ARM sub)
typedef enum
{
	
	ARM_PLC_SUBSYS_SOFT_RST_SHIFT 		= 0,	// Bit [0]: PLC subsystem soft reset. Default: 1
												// 0: Reset.
												// 1: Release.

	ARM_RF_SUBSYS_SOFT_RST_SHIFT 		= 1,	// Bit [1]: RF subsystem soft reset. Default: 1
												// 0: Reset.
												// 1: Release.

	ARM_PLC_DSP_CORE_SOFT_RST_SHIFT 	= 2,	// Bit [2]: Bit [2]: PLC_DSP_CORE soft reset. Default: 1
												// 0: Reset.
												// 1: Release.

	ARM_RF_DSP_CORE_SOFT_RST_SHIFT 		= 3,	// Bit [3]: RF_DSP_CORE soft reset. Default: 1
												// 0: Reset.
												// 1: Release.

	ARM_PLC_SUBSYS_CLK_GATE_CTRL_SHIFT 	= 4,	// Bit [4]: PLC subsystem clocks gate control. Default: 0
												// 1: clock gating off all the PLC subsystem modules.

	ARM_RF_SUBSYS_CLK_GATE_CTRL_SHIFT 	= 5,	// Bit [5]: RF subsystem clocks gate control. Default: 0
												// 1: clock gating off all the RF subsystem modules.

	ARM_PLC_OCD_HALT_ON_RST_SHIFT 		= 6,	// Bit[6]: Controls OCDHaltOnReset of PLC core. 
												// 1: PLC core will be in OCD Halt mode after reset. 
												// 0: Normal operation									

	ARM_RF_OCD_HALT_ON_RST_SHIFT 		= 7,	// Bit[7]: Controls OCDHaltOnReset of RF core. Defaut 0.
												// 1: RF core will be in OCD Halt mode after reset. When debug is disabled by DSP OTP, this bit will has no effect.
												// 0: Normal operation									

	ARM_PLC_RUNSTALL_SHIFT 				= 8,	// Bit[8]: Control RunStall of PLC core. Default: 0
												// 1: The internal pipeline of PLC core will be stalled.
												// 0: normal operation.
									
	ARM_RF_RUNSTALL_SHIFT 				= 9,	// Bit[9]: Control RunStall of RF core. Default: 0
												// 1: The internal pipeline of RF core will be stalled.
												// 0: normal operation.
									
	ARM_DSP_REVD_SHIFT 					= 10	// Bit [31:10]: Reserved.
}E_ARM_DSP_RST_CFG_SHIFT;

typedef enum
{
	// arm block reset
	ARM_NFC_RST 		= ARM_NFC_RST_SHIFT,
	ARM_SDMMC0_RST 		= ARM_SDMMC0_RST_SHIFT,
	ARM_SDMMC1_RST 		= ARM_SDMMC1_RST_SHIFT,
	ARM_DMA0_RST 		= ARM_DMA0_RST_SHIFT,	
	ARM_DMA1_RST 		= ARM_DMA1_RST_SHIFT,	
	ARM_DMA2_RST 		= ARM_DMA2_RST_SHIFT,	
	ARM_QSPI_RST 		= ARM_QSPI_RST_SHIFT,	
	ARM_CRC_RST 		= ARM_CRC_RST_SHIFT,
	ARM_ECC_RST 		= ARM_ECC_RST_SHIFT,
	ARM_SHA_RST 		= ARM_SHA_RST_SHIFT,
	ARM_AES_RST 		= ARM_AES_RST_SHIFT,
	ARM_OTP_CTRL_RST 	= ARM_OTP_CTRL_RST_SHIFT,
	ARM_IPC_RST 		= ARM_IPC_RST_SHIFT,
	ARM_DEBUG0_RST 		= ARM_DEBUG0_RST_SHIFT,
	ARM_DEBUG1_RST 		= ARM_DEBUG1_RST_SHIFT,
	ARM_I2C0_RST 		= ARM_I2C0_RST_SHIFT,
	ARM_I2C1_RST 		= ARM_I2C1_RST_SHIFT,
	ARM_I2C2_RST 		= ARM_I2C2_RST_SHIFT,
	ARM_SPI0_RST 		= ARM_SPI0_RST_SHIFT,
	ARM_SPI1_RST 		= ARM_SPI1_RST_SHIFT,
	ARM_SARADC_CTRL_RST = ARM_SARADC_CTRL_RST_SHIFT,
	ARM_UART0_RST 		= ARM_UART0_RST_SHIFT,
	ARM_UART1_RST 		= ARM_UART1_RST_SHIFT,
	ARM_UART2_RST 		= ARM_UART2_RST_SHIFT,	
	ARM_UART3_RST 		= ARM_UART3_RST_SHIFT,	
	ARM_WDT_RST 		= ARM_WDT_RST_SHIFT,	
	ARM_TIMER_RST 		= ARM_TIMER_RST_SHIFT,
	ARM_GPIO_RST 		= ARM_GPIO_RST_SHIFT,
	ARM_RNG_RST 		= ARM_RNG_RST_SHIFT,
	ARM_USBOTG0_RST 	= ARM_USBOTG0_RST_SHIFT,
	ARM_USBOTG1_RST 	= ARM_USBOTG1_RST_SHIFT,
	ARM_DDR_CTRL_RST 	= ARM_DDR_CTRL_RST_SHIFT, 

	// dsp subsystem software reset from arm
	ARM_PLC_SUBSYS_SOFT_RST 	= (32 + ARM_PLC_SUBSYS_SOFT_RST_SHIFT),
	ARM_RF_SUBSYS_SOFT_RST 		= (32 + ARM_RF_SUBSYS_SOFT_RST_SHIFT),
	ARM_PLC_DSP_CORE_SOFT_RST 	= (32 + ARM_PLC_DSP_CORE_SOFT_RST_SHIFT),
	ARM_RF_DSP_CORE_SOFT_RST 	= (32 + ARM_RF_DSP_CORE_SOFT_RST_SHIFT),

	// whole chip software reset
	GLOBAL_SOFT_RST				= 36,

	// arm subsystem software reset
	ARM_SOFT_RST				= 37
}E_ARM_IP_RST;

// offset: 0x0020, ARM Clock Gating Control Registers
typedef enum
{
	ARM_BLK_CG_NFC_DIS_SHIFT 		= 0,	// Bit[0]:NFC_clk_dis; Default:0;
											// 1: clock gating off the NFC module;

	ARM_BLK_CG_SDMMC0_DIS_SHIFT 	= 1,	// Bit [1]: SDMMC0_clk_dis; Default:0;
											// 1: clock gating off the SDMMC0 module;

	ARM_BLK_CG_SDMMC1_DIS_SHIFT 	= 2,	// Bit [2]:SDMMC1_clk_dis; Default:0;
											// 1: clock gating off the SDMMC1 module;

	ARM_BLK_CG_DAM0_DIS_SHIFT 		= 3,	// Bit[3]: DAM0_clk_dis; Default:0;
											// 1: clock gating off the DMA0 module;

	ARM_BLK_CG_DAM1_DIS_SHIFT 		= 4,	// Bit [4]: DMA1_clk_dis; Default:0;
											// 1: clock gating off the DMA1 module;

	ARM_BLK_CG_DAM2_DIS_SHIFT 		= 5,	// Bit [5]: DMA2_clk_dis; Default:0;
											// 1: clock gating off the DMA2 module;

	ARM_BLK_CG_QSPI_DIS_SHIFT 		= 6,	// Bit [6]:QSPI_clk_dis; Default:0;
											// 1: clock gating off the QSPI module;

	ARM_BLK_CG_CRC_DIS_SHIFT 		= 7,	// Bit [7]: CRC_clk_dis; Default:0;
											// 1: clock gating off the CRC module;

	ARM_BLK_CG_ECC384_DIS_SHIFT 	= 8,	// Bit [8]: ECC384_clk_dis; Default:0;
											// 1: clock gating off the ECC384;

	ARM_BLK_CG_SHA384_DIS_SHIFT 	= 9,	// Bit [9]: SHA384_clk_dis; Default:0;
											// 1: clock gating off  the SHA384;

	ARM_BLK_CG_AES_DIS_SHIFT 		= 10,	// Bit [10]: AES_clk_dis; Default:0;
											// 1: clock gating off the AES;

	ARM_BLK_CG_OTP_DIS_SHIFT 		= 11,	// Bit [11]: OTP_clk_dis: Default:0
											// 1: clock gating off ARM_OTP; 

	ARM_BLK_CG_IPC_DIS_SHIFT 		= 12,	// Bit [12]: IPC_clk_dis: Default:0
											// 1: clock gating off IPC module;

	ARM_BLK_CG_DBG_TCLK_DIS_SHIFT 	= 13,	// Bit [13]: DBG_tclk_dis: Default:0, Reserved
											// 1: clock gating off ARM corsight;

	ARM_BLK_CG_DBG_PCLK_DIS_SHIFT 	= 14,	// Bit [14]: DBG_pclk_dis: Default:0, Reserved
											// 1: clock gating off ARM corsight

	ARM_BLK_CG_I2C0_DIS_SHIFT 		= 15,	// Bit [15]: I2C0_clk_dis; Default:0;
											// 1: clock gating off the I2C0;

	ARM_BLK_CG_I2C1_DIS_SHIFT 		= 16,	// Bit [16]: I2C1_clk_dis; Default:0;
											// 1: clock gating off the I2C1;

	ARM_BLK_CG_I2C2_DIS_SHIFT 		= 17,	// Bit [17]:I2C2_clk_dis; Default:0;
											// 1: clock gating off the I2C2;

	ARM_BLK_CG_SPI0_DIS_SHIFT 		= 18,	// Bit [18]: SPI0_clk_dis; Default:0;
											// 1: clock gating off SPI0;

	ARM_BLK_CG_SPI1_DIS_SHIFT 		= 19,	// Bit [19]: SPI1_clk_dis; Default:0;
											// 1: clock gating off SPI1;

	ARM_BLK_CG_SARADC_DIS_SHIFT 	= 20,	// Bit [20]: SARADC_clk_dis;Default:0;
											// 1: clock gating off SAR ADC;

	ARM_BLK_CG_UART0_DIS_SHIFT 		= 21,	// Bit [21]: UART0_clk_dis; Default:0;
											// 1: clock gating off UART0;

	ARM_BLK_CG_UART1_DIS_SHIFT 		= 22,	// Bit [22]: UART1_clk_dis; Default:0;
											// 1: clock gating off UART1;

	ARM_BLK_CG_UART2_DIS_SHIFT 		= 23,	// Bit [23]: UART2_clk_dis; Default:0;
											// 1: clock gating off UART2;

	ARM_BLK_CG_UART3_DIS_SHIFT 		= 24,	// Bit [24]: UART3_clk_dis; Default:0;
											// 1: clock gating off UART3;

	ARM_BLK_CG_WDT_DIS_SHIFT 		= 25,	// Bit [25]: WDT_clk_dis; Default:0;
											// 1: clock gating off WDT;

	ARM_BLK_CG_TIMER_DIS_SHIFT 		= 26, 	// Bit [26]: Timer_clk_dis; Default:0;
											// 1: clock gating off Timer;

	ARM_BLK_CG_GPIO_DIS_SHIFT 		= 27,	// Bit [27]: GPIO_clk_dis;Default:0
											// 1: clock gating off GPIO;

	ARM_BLK_CG_RNG_DIS_SHIFT 		= 28,	// Bit [28]: RNG_clk_dis: Default:0
											// 1: clock gating off RNG;

	ARM_BLK_CG_USBOTG0_DIS_SHIFT 	= 29,	// Bit [29]: usbotg0_clk_dis: Default:0
											// 1: clock gating off usbotg0;

	ARM_BLK_CG_USBOTG1_DIS_SHIFT 	= 30,	// Bit [30]: usbotg1_clk_dis: Default:0
											// 1: clock gating off usbotg1;

	ARM_BLK_CG_DDR_CTRL_DIS_SHIFT 	= 31	// Bit [31]: ddr_cntl_clk_dis: Default:0
											// 1: clock gating off ddr controller;
} E_ARM_BLK_CG_SHIFT;

typedef enum
{
	ARM_BLK_CG_NFC 		= 0,
	ARM_BLK_CG_SDMMC0 	= 1,
	ARM_BLK_CG_SDMMC1 	= 2,
	ARM_BLK_CG_DAM0 	= 3,
	ARM_BLK_CG_DAM1 	= 4,
	ARM_BLK_CG_DAM2 	= 5,
	ARM_BLK_CG_QSPI 	= 6,
	ARM_BLK_CG_CRC 		= 7,
	ARM_BLK_CG_ECC384 	= 8,
	ARM_BLK_CG_SHA384 	= 9,
	ARM_BLK_CG_AES 		= 10,
	ARM_BLK_CG_OTP 		= 11,
	ARM_BLK_CG_IPC 		= 12,
	ARM_BLK_CG_DBG_TCLK	= 13,
	ARM_BLK_CG_DBG_PCLK = 14,
	ARM_BLK_CG_I2C0 	= 15,
	ARM_BLK_CG_I2C1 	= 16,
	ARM_BLK_CG_I2C2 	= 17,
	ARM_BLK_CG_SPI0 	= 18,
	ARM_BLK_CG_SPI1 	= 19,
	ARM_BLK_CG_SARADC 	= 20,
	ARM_BLK_CG_UART0 	= 21,
	ARM_BLK_CG_UART1 	= 22,
	ARM_BLK_CG_UART2 	= 23,
	ARM_BLK_CG_UART3 	= 24,
	ARM_BLK_CG_WDT 		= 25,
	ARM_BLK_CG_TIMER 	= 26,
	ARM_BLK_CG_GPIO 	= 27,
	ARM_BLK_CG_RNG 		= 28,
	ARM_BLK_CG_USBOTG0 	= 29,
	ARM_BLK_CG_USBOTG1 	= 30,
	ARM_BLK_CG_DDR_CTRL = 31
} E_ARM_BLK_CG;

// arm block clock gate control mask and value
typedef enum
{
	ARM_BLK_CG_EN 	= 0,
	ARM_BLK_CG_DIS 	= 1
} E_ARM_BLK_CG_CTRL;

#define ARM_BLK_CG_CTRL_MASK(blk)		(0x1<<blk)
#define ARM_BLK_CG_CTRL_VAL(blk, val)	((val)<<blk)

// offset: 0x0030, ARM power domain register
typedef enum
{
	ARM_PWD_HS_KEEPING_DONE_ST_SHIFT = 0,	// Bit [0]: ARM power domain housekeeping done status. Default: 0x1;
											// 0: housekeeping in process;
											// 1: housekeeping done
	 
	ARM_PWD_REVD0_SHIFT 			= 1,	// Bit [15:1]: Reserved.

	ARM_PWD_STATE_MACHINE_SHIFT 	= 16, 	// Bit [18:16]: ARM state machine's current state.														
											// They are read only registers.
											// 3'b000: EPF sleep mode;
											// 3'b001: Power up transition state;
											// 3'b011: Active mode;
											// 3'b111: EPF shutdown mode;
											// 3'b010: Power down transition state.													

	ARM_PWD_REVD1_SHIFT 			= 19, 	// Bit [31:19]: Reserved.
} E_ARM_PWD_SHIFT;

// arm house keeping done status mask and value
#define ARM_PWD_HS_KEEPING_DONE_ST_MASK		(0x1<<ARM_PWD_HS_KEEPING_DONE_ST_SHIFT)
#define ARM_PWD_HS_KEEPING_DONE_ST_VAL(val)	((val)<<ARM_PWD_HS_KEEPING_DONE_ST_SHIFT)

// arm house keeping done status
typedef enum
{
	ARM_PWD_HS_KEEPING_PENDING	= 0,
	ARM_PWD_HS_KEEPING_DONE		= 1
} E_ARM_HS_KEEPING_DONE_ST;

// arm status machine mask
#define ARM_PWD_ST_MACHINE_MASK		(0x7<<ARM_PWD_STATE_MACHINE_SHIFT)

// arm state machine's status
typedef enum
{
	ARM_EPF_SLEEP_MODE		= 0,	// EPF sleep mode
	ARM_PU_TRANS_MODE		= 1,	// Power up transition state
	ARM_ACTIVE_MODE			= 3,	// Active mode
	ARM_EPF_SHUTDOWN_MODE	= 7,	// EPF shutdown mode
	ARM_PD_TRANS_MODE		= 2		// Power down transition state	
} E_ARM_PWD_ST_MACHINE;

// offset: 0x0034, Failsafe timer threshold register
typedef enum
{
	ARM_FAILSAFE_TIMER_SHIFT 	= 0,	// Bit [7:0]: failsafe timer threshold register. Default: 0x64
										// Upon EPF transition from high to low, The failsafe timer begins counting, it will increase 1 per 1ms, 
										// when the threshold is reached. ARM will be forced to enter into sleep mode. 
										// The failsafe timer will be disabled if housekeeping is done before expired
										// When setting the register to 0x0, the failsafe timer will be disabled.
		
	ARM_FAILSAFE_TIMER_REVD_SHIFT = 8, // Bit [31:8]: Reserved.
} E_ARM_FAILSAFE_TIMER_SHIFT;

#define ARM_FAILSAFE_TIMER_MASK		(0xFF<<ARM_FAILSAFE_TIMER_SHIFT)
#define ARM_FAILSAFE_TIMER_VAL(val)	((val)<<ARM_FAILSAFE_TIMER_SHIFT)

#define ARM_FAILSAFE_TIMER_MAX		0xFF


// offset: 0x0038, ARM pmu interrupt status register
typedef enum
{
	ARM_PMU_INT_ST_SHIFT 		= 0,	// Bit[0]: EPF high to low transition interrupt status register for ARM subsystem.
											// 0: no interrupt;
											// 1: EPF H2L interrupt generated.
											// Write 1 to this bit will clear the interrupt status, write 0 has no effect.
	
	ARM_PMU_INT_ST_REVD_SHIFT 	= 1 	// Bit [31:1]: Reserved.
} E_ARM_PMU_INT_ST_SHIFT;

#define ARM_PMU_INT_ST_MASK		(1<<ARM_PMU_INT_ST_SHIFT)

// offset: 0x0060, ARM Chip ID Register
#define ARM_CHIP_ID				0x20161115

// offset: 0x0070, ARM Functionality Auxilary Register
typedef enum
{
	
	ARM_TIMER_W_OUT_SHIFT 		= 0, 	// Bit [2:0]: TIMER_W_OUT source select signals. Default :3'h0	
										// 3'h0: output from ARM Timer 0
										// 3'h1: output from ARM Timer 1
										// 3'h2: output from ARM Timer 2
										// 3'h3: output from ARM Timer 3
										// 3'h4: output from ARM Timer 4
										// 3'h5: output from ARM Timer 5
										// 3'h6: output from ARM Timer 6
										// 3'h7: output from ARM Timer 7

	ARM_TIMER_OUT_REVD0_SHIFT  	= 3, 	// Bit[3]: Reserved
	
											
	ARM_TIMER_X_OUT_SHIFT 		= 4, 	// Bit [6:4]: TIMER_X_OUT source select signals. Default :3'h1
										// 3'h0: output from ARM Timer 0
										// 3'h1: output from ARM Timer 1
										// 3'h2: output from ARM Timer 2
										// 3'h3: output from ARM Timer 3
										// 3'h4: output from ARM Timer 4
										// 3'h5: output from ARM Timer 5
										// 3'h6: output from ARM Timer 6
										// 3'h7: output from ARM Timer 7

	ARM_TIMER_OUT_REVD1_SHIFT 	= 7, 	// Bit[7]: Reserved

	ARM_TIMER_Y_OUT_SHIFT 	  	= 8, 	// Bit [10:8]: TIMER_Y_OUT source select signals. Default :3'h2
										// 3'h0: output from ARM Timer 0
										// 3'h1: output from ARM Timer 1
										// 3'h2: output from ARM Timer 2
										// 3'h3: output from ARM Timer 3
										// 3'h4: output from ARM Timer 4
										// 3'h5: output from ARM Timer 5
										// 3'h6: output from ARM Timer 6
										// 3'h7: output from ARM Timer 7

	ARM_TIMER_OUT_REVD2_SHIFT 	= 11, 	// Bit[11]: Reserved	

	ARM_TIMER_Z_OUT_SHIFT 		= 12, 	// Bit [14:12]: TIMER_Z_OUT source select signals. Default :3'h3
										// 3'h0: output from ARM Timer 0
										// 3'h1: output from ARM Timer 1
										// 3'h2: output from ARM Timer 2
										// 3'h3: output from ARM Timer 3
										// 3'h4: output from ARM Timer 4
										// 3'h5: output from ARM Timer 5
										// 3'h6: output from ARM Timer 6
										// 3'h7: output from ARM Timer 7

	ARM_TIMER_OUT_REVD3_SHIFT 	= 15 	// Bit[31:15]: Reserved
} E_ARM_PINMUX_TIMER_OUT_SHIFT;

// timer output pin mask
#define ARM_PINMUX_TIMERW_OUTPUT_MASK	(7<<ARM_TIMER_W_OUT_SHIFT)
#define ARM_PINMUX_TIMERX_OUTPUT_MASK	(7<<ARM_TIMER_X_OUT_SHIFT)
#define ARM_PINMUX_TIMERY_OUTPUT_MASK	(7<<ARM_TIMER_Y_OUT_SHIFT)
#define ARM_PINMUX_TIMERZ_OUTPUT_MASK	(7<<ARM_TIMER_Z_OUT_SHIFT)

// timer output pin value
#define ARM_PINMUX_TIMERW_OUTPUT_VAL(port)	((port)<<ARM_TIMER_W_OUT_SHIFT)
#define ARM_PINMUX_TIMERX_OUTPUT_VAL(port)	((port)<<ARM_TIMER_X_OUT_SHIFT)
#define ARM_PINMUX_TIMERY_OUTPUT_VAL(port)	((port)<<ARM_TIMER_Y_OUT_SHIFT)
#define ARM_PINMUX_TIMERZ_OUTPUT_VAL(port)	((port)<<ARM_TIMER_Z_OUT_SHIFT)

// offset: 0x0094, ARM DMA Handshaking External  INT  Source Selection
typedef enum
{
	// This register determins the 3 ext interrupt sources 
	// that are used as the handshake signals, as 
	// specified in "0x90 ARM DMA Handshaking selection 6¡± 
	// [3:0] ext interrupt 0 source, default value: 0x0 
	// [7:4] ext interrupt 1 source, default value: 0x1 
	// [11:8] ext interrupt 2 source, default value: 0x2 
	ARM_DMA_HS_EXT_INT0_SHIFT = 0,	// [3:0] ext interrupt 0 source, default value: 0x0 
	ARM_DMA_HS_EXT_INT1_SHIFT = 4,	// [7:4] ext interrupt 1 source, default value: 0x1
	ARM_DMA_HS_EXT_INT2_SHIFT = 8,	// [11:8] ext interrupt 2 source, default value: 0x2
	ARM_DMA_HS_EXT_REVD_SHIFT = 12	// [31:12] Reserved. 
} E_ARM_DMA_HS_EXT_INT_SRC_SEL_SHIFT;

// dma external interrupt source
typedef enum
{
	ARM_DMA_HS_EXT_GPIOA_INT0 	= 0,	// 4¡¯b0000: GPIO Bank A int 0 
	ARM_DMA_HS_EXT_GPIOA_INT1 	= 1,	// 4¡¯b0001: GPIO Bank A int 1 
	ARM_DMA_HS_EXT_GPIOB_INT0 	= 2,	// 4¡¯b0010: GPIO Bank B int 0 
	ARM_DMA_HS_EXT_GPIOB_INT1 	= 3,	// 4¡¯b0011: GPIO Bank B int 1 
	ARM_DMA_HS_EXT_GPIOC_INT0 	= 4,	// 4¡¯b0100: GPIO Bank C int 0 
	ARM_DMA_HS_EXT_GPIOC_INT1 	= 5,	// 4¡¯b0101: GPIO Bank C int 1 
	ARM_DMA_HS_EXT_GPIOD_INT0 	= 6,	// 4¡¯b0110: GPIO Bank D int 0 
	ARM_DMA_HS_EXT_GPIOD_INT1 	= 7,	// 4¡¯b0111: GPIO Bank D int 1 
	ARM_DMA_HS_EXT_TIMER0 		= 8,	// 4¡¯b1000: Timer 0	
	ARM_DMA_HS_EXT_TIMER1 		= 9,	// 4¡¯b1001: Timer 1	
	ARM_DMA_HS_EXT_TIMER2 		= 10,	// 4¡¯b1010: Timer 2	
	ARM_DMA_HS_EXT_TIMER3 		= 11,	// 4¡¯b1011: Timer 3
	ARM_DMA_HS_EXT_TIMER4 		= 12,	// 4¡¯b1100: Timer 4   
	ARM_DMA_HS_EXT_TIMER5 		= 13,	// 4¡¯b1101: Timer 5   
	ARM_DMA_HS_EXT_TIMER6 		= 14,	// 4¡¯b1110: Timer 6   
	ARM_DMA_HS_EXT_TIMER7 		= 15,	// 4¡¯b1111: Timer 7   	 
	ARM_DMA_HS_EXT_SRC_NUM 		= 16		
} E_ARM_DMA_HS_EXT_INT_SRC;

// dma handshake external interrupt source selection mask and value
#define ARM_DMA_HS_EXT_INT0_MASK			(0X0F<<ARM_DMA_HS_EXT_INT0_SHIFT)
#define ARM_DMA_HS_EXT_INT0_VAL(val)		((val)<<ARM_DMA_HS_EXT_INT0_SHIFT)

#define ARM_DMA_HS_EXT_INT1_MASK			(0X0F<<ARM_DMA_HS_EXT_INT1_SHIFT)
#define ARM_DMA_HS_EXT_INT1_VAL(val)		((val)<<ARM_DMA_HS_EXT_INT1_SHIFT)

#define ARM_DMA_HS_EXT_INT2_MASK			(0X0F<<ARM_DMA_HS_EXT_INT2_SHIFT)
#define ARM_DMA_HS_EXT_INT2_VAL(val)		((val)<<ARM_DMA_HS_EXT_INT2_SHIFT)

// offset: 0x00A0, SD0_PINMUX(sdio0_iodis)  register
typedef enum
{
	
	ARM_PINMUX_SD0_CLK_GPIOC0_SHIFT = 0, 	// Bit[0]: SDIO0_CLK select signal. Default: 1 
											// 0: SDIO0_CLK
											// 1: GPIOC[0] 
											// In reset state, it is GPIOC[0] as input, weak pull up

	ARM_PINMUX_SD0_CMD_GPIOC1_SHIFT = 1, 	// Bit[1]: SDIO0_CMD select signal. Default: 1 
											// 0: SDIO0_CMD 
											// 1: GPIOC[1] 
											// In reset state, it is GPIOC[1] as input, weak pull up 

	ARM_PINMUX_SD0_D0_GPIOC2_SHIFT 	= 2,	// Bit[2]: SDIO0_DATA[0] select signal. Default: 1 
											// 0: SDIO0_DATA0 
											// 1: GPIOC[2] 
											// In reset state, it is GPIOC[2] as input, weak pull up 

	ARM_PINMUX_SD0_D1_GPIOC3_SHIFT 	= 3,	// Bit[3]: SDIO0_DATA[1] select signal. Default: 1 
											// 0: SDIO0_DATA1
											// 1: GPIOC[3] 
											// In reset state, it is GPIOC[3] as input, weak pull up

	ARM_PINMUX_SD0_D2_GPIOC4_SHIFT 	= 4,	// Bit[4]: SDIO0_DATA[2] select signal. Default:1 
											// 0: SDIO0_DATA2 
											// 1: GPIOC[4] 
											// In reset state, it is GPIOC[4] as input, weak pull up 

	ARM_PINMUX_SD0_D3_GPIOC5_SHIFT 	= 5,	// Bit[5]: SDIO0_DATA[3] select signal. Default:1 
											// 0: SDIO0_DATA3 
											// 1: GPIOC[5] 
											// In reset state, it is GPIOC[5] as input, weak pull up

	ARM_PINMUX_SD0_D4_GPIOC6_SHIFT 	= 6,	// Bit[6]: SDIO0_DATA[4] select signal. Default: 1 
											// 0: SDIO0_DATA4 
											// 1: GPIOC[6] 
											// In reset state, it is GPIOC[6] as input, weak pull up 

	ARM_PINMUX_SD0_D5_GPIOC7_SHIFT 	= 7,	// Bit[7]: SDIO0_DATA[5] select signal. Default: 1 
											// 0: SDIO0_DATA5 
											// 1: GPIOC[7] 
											// n reset state, it is GPIOC[7] as input, weak pull up 

	ARM_PINMUX_SD0_D6_GPIOC8_SHIFT 	= 8,	// Bit[8]: SDIO0_DATA[6] select signal. Default:  1 
											// 0: SDIO0_DATA6 
											// 1: GPIOC[8] 
											// In reset state, it is GPIOC[8] as input, weak pull up 

	ARM_PINMUX_SD0_D7_GPIOC9_SHIFT 	= 9,	// Bit[9]: SDIO0_DATA[7] select signal. Default: 1 
											// 0: SDIO0_DATA7 
											// 1: GPIOC[9] 
											// In reset state, it is GPIOC[9] as input, weak pull up

	ARM_PINMUX_SD0_CD_GPIOC25_SHIFT = 10,	// Bit[10]: SDIO0_CD select signal. Default:  1 
											// 0: SDIO0_CD 
											// 1: GPIOC[25] 
											// In reset state, it is GPIOC[25] as input, weak pull up

	ARM_PINMUX_SD0_WP_GPIOC26_SHIFT = 11,	// Bit[11]: SDIO0_WP select signal. Default:  1 
											// 0: SDIO0_WP 
											// 1: GPIOC[26] 
											// In reset state, it is GPIOC[26] as input, weak pull up 

	ARM_PINMUX_SD0_RSTN_GPIOC29_SHIFT = 12,	// Bit[12]: SDIO0_RSTN select signal. Default:  1 
											// 0: SDIO0_WP 
											// 1: GPIOC[29] 
											// In reset state, it is GPIOC[29] as input, weak pull up 

	ARM_PINMUX_SD0_REVD_SHIFT 		= 13	// Bit[31:13]: Reserved
}E_ARM_PINMUX_SD0_SHIFT;

// SDIO bus width
typedef enum
{
	
	SDIO_BW_1BIT = 1,
	SDIO_BW_4BIT = 4,
	SDIO_BW_8BIT = 8,
}E_SDIO_BW;

// sd0 control pin mask and value
#define ARM_PINMUX_SD0_CTRL_MASK	(0x1C03<<ARM_PINMUX_SD0_CLK_GPIOC0_SHIFT)

// sd0 bus width pin mask and value
#define ARM_PINMUX_SD0_1BIT_MASK	(ARM_PINMUX_SD0_CTRL_MASK |\
									(0x1<<ARM_PINMUX_SD0_D0_GPIOC2_SHIFT))

#define ARM_PINMUX_SD0_4BIT_MASK	(ARM_PINMUX_SD0_CTRL_MASK |\
									(0xF<<ARM_PINMUX_SD0_D0_GPIOC2_SHIFT))

#define ARM_PINMUX_SD0_8BIT_MASK	(ARM_PINMUX_SD0_CTRL_MASK |\
									(0xFF<<ARM_PINMUX_SD0_D0_GPIOC2_SHIFT))
// gpioc pin mask
#define ARM_PINMUX_GPIOC_PIN0_MASK	(1<<ARM_PINMUX_SD0_CLK_GPIOC0_SHIFT)
#define ARM_PINMUX_GPIOC_PIN1_MASK	(1<<ARM_PINMUX_SD0_CMD_GPIOC1_SHIFT)
#define ARM_PINMUX_GPIOC_PIN2_MASK	(1<<ARM_PINMUX_SD0_D0_GPIOC2_SHIFT)
#define ARM_PINMUX_GPIOC_PIN3_MASK	(1<<ARM_PINMUX_SD0_D1_GPIOC3_SHIFT)
#define ARM_PINMUX_GPIOC_PIN4_MASK	(1<<ARM_PINMUX_SD0_D2_GPIOC4_SHIFT)
#define ARM_PINMUX_GPIOC_PIN5_MASK	(1<<ARM_PINMUX_SD0_D3_GPIOC5_SHIFT)
#define ARM_PINMUX_GPIOC_PIN6_MASK	(1<<ARM_PINMUX_SD0_D4_GPIOC6_SHIFT)
#define ARM_PINMUX_GPIOC_PIN7_MASK	(1<<ARM_PINMUX_SD0_D5_GPIOC7_SHIFT)
#define ARM_PINMUX_GPIOC_PIN8_MASK	(1<<ARM_PINMUX_SD0_D6_GPIOC8_SHIFT)
#define ARM_PINMUX_GPIOC_PIN9_MASK	(1<<ARM_PINMUX_SD0_D7_GPIOC9_SHIFT)
#define ARM_PINMUX_GPIOC_PIN25_MASK	(1<<ARM_PINMUX_SD0_CD_GPIOC25_SHIFT)
#define ARM_PINMUX_GPIOC_PIN26_MASK	(1<<ARM_PINMUX_SD0_WP_GPIOC26_SHIFT)
#define ARM_PINMUX_GPIOC_PIN29_MASK	(1<<ARM_PINMUX_SD0_RSTN_GPIOC29_SHIFT)

// gpioc pin value
#define ARM_PINMUX_GPIOC_PIN0_VAL	(1<<ARM_PINMUX_SD0_CLK_GPIOC0_SHIFT)
#define ARM_PINMUX_GPIOC_PIN1_VAL	(1<<ARM_PINMUX_SD0_CMD_GPIOC1_SHIFT)
#define ARM_PINMUX_GPIOC_PIN2_VAL	(1<<ARM_PINMUX_SD0_D0_GPIOC2_SHIFT)
#define ARM_PINMUX_GPIOC_PIN3_VAL	(1<<ARM_PINMUX_SD0_D1_GPIOC3_SHIFT)
#define ARM_PINMUX_GPIOC_PIN4_VAL	(1<<ARM_PINMUX_SD0_D2_GPIOC4_SHIFT)
#define ARM_PINMUX_GPIOC_PIN5_VAL	(1<<ARM_PINMUX_SD0_D3_GPIOC5_SHIFT)
#define ARM_PINMUX_GPIOC_PIN6_VAL	(1<<ARM_PINMUX_SD0_D4_GPIOC6_SHIFT)
#define ARM_PINMUX_GPIOC_PIN7_VAL	(1<<ARM_PINMUX_SD0_D5_GPIOC7_SHIFT)
#define ARM_PINMUX_GPIOC_PIN8_VAL	(1<<ARM_PINMUX_SD0_D6_GPIOC8_SHIFT)
#define ARM_PINMUX_GPIOC_PIN9_VAL	(1<<ARM_PINMUX_SD0_D7_GPIOC9_SHIFT)
#define ARM_PINMUX_GPIOC_PIN25_VAL	(1<<ARM_PINMUX_SD0_CD_GPIOC25_SHIFT)
#define ARM_PINMUX_GPIOC_PIN26_VAL	(1<<ARM_PINMUX_SD0_WP_GPIOC26_SHIFT)
#define ARM_PINMUX_GPIOC_PIN29_VAL	(1<<ARM_PINMUX_SD0_RSTN_GPIOC29_SHIFT)

// offset: 0x00A4, SD1_PINMUX(sdio1_iodis)  register
typedef enum
{
	
	ARM_PINMUX_SD1_CLK_GPIOC10_SHIFT = 0, 	// Bit[0]: SDIO1_CLK select signal. Default: 1 
											// 0: SDIO1_CLK
											// 1: GPIOC[10] 
											// In reset state, it is GPIOC[0] as input, weak pull up

	ARM_PINMUX_SD1_CMD_GPIOC11_SHIFT = 1, 	// Bit[1]: SDIO1_CMD select signal. Default: 1 
											// 0: SDIO1_CMD 
											// 1: GPIOC[11] 
											// In reset state, it is GPIOC[1] as input, weak pull up 

	ARM_PINMUX_SD1_D0_GPIOC12_SHIFT = 2,	// Bit[2]: SDIO1_DATA[0] select signal. Default: 1 
											// 0: SDIO1_DATA0 
											// 1: GPIOC[12] 
											// In reset state, it is GPIOC[2] as input, weak pull up 

	ARM_PINMUX_SD1_D1_GPIOC13_SHIFT = 3,	// Bit[3]: SDIO1_DATA[1] select signal. Default: 1 
											// 0: SDIO1_DATA1
											// 1: GPIOC[13] 
											// In reset state, it is GPIOC[3] as input, weak pull up

	ARM_PINMUX_SD1_D2_GPIOC14_SHIFT = 4,	// Bit[4]: SDIO1_DATA[2] select signal. Default:1 
											// 0: SDIO1_DATA2 
											// 1: GPIOC[14] 
											// In reset state, it is GPIOC[4] as input, weak pull up 

	ARM_PINMUX_SD1_D3_GPIOC15_SHIFT = 5,	// Bit[5]: SDIO1_DATA[3] select signal. Default:1 
											// 0: SDIO1_DATA3 
											// 1: GPIOC[15] 
											// In reset state, it is GPIOC[5] as input, weak pull up

	ARM_PINMUX_SD1_D4_GPIOC16_TIMER0_IN_SHIFT 	= 6,	// Bit[6, 7]: SDIO1_DATA[4] select signal. Default: 11 
														// 2'b00: SDIO1_DATA4 
														// 2'b01: ARM_TIMER0_INPUT 
														// 2'b10: GPIOC[16] 
														// 2'b11: GPIOC[16] 
														// In reset state, it is GPIOC[6] as input, weak pull up

	ARM_PINMUX_SD1_D5_GPIOC17_TIMER1_IN_SHIFT 	= 8,	// Bit[9:8]: SDIO1_DATA[5] select signal. Default: 2¡¯b1
														// 2'b00: SDIO1_DATA5 
														// 2'b01: ARM_TIMER1_INPUT 
														// 2'b10: GPIOC[17] 
														// 2'b11: GPIOC[17] 
														// In reset state, it is GPIOC[17] as input, weak pull up 


	ARM_PINMUX_SD1_D6_GPIOC18_TIMER2_IN_SHIFT 	= 10,	// Bit[11:10]: SDIO1_DATA[6] select signal. Default: 2¡¯b11 
														// 2'b00: SDIO1_DATA6 
														// 2'b01: ARM_TIMER2_INPUT 
														// 2'b10: GPIOC[18] 
														// 2'b11: GPIOC[18] 
														// In reset state, it is GPIOC[18] as input, weak pull up


	ARM_PINMUX_SD1_D7_GPIOC19_TIMER3_IN_SHIFT 	= 12,	// Bit[13:12]: SDIO1_DATA[7] select signal. Default: 2¡¯b11 
														// 2'b00: SDIO1_DATA7 
														// 2'b01: ARM_TIMER3_INPUT 
														// 2'b10: GPIOC[19] 
														// 2'b11: GPIOC[19] 
														// In reset state, it is GPIOC[19] as input, weak pull up

	ARM_PINMUX_SD1_CD_GPIOC27_SHIFT = 14,				// Bit[14]: SDIO1_CD select signal. Default: 1 
														// 0: SDIO1_CD 
														// 1: GPIOC[27] 
														// In reset state, it is GPIOC[27] as input, weak pull up 


	ARM_PINMUX_SD1_WP_GPIOC28_SHIFT = 15,				// Bit[15]: SDIO1_WP select signal. Default:  1 
														// 0: SDIO1_WP 
														// 1: GPIOC[28] 
														// In reset state, it is GPIOC[28] as input, weak pull up 

	ARM_PINMUX_SD1_RSTN_GPIOC30_SHIFT = 16,				// Bit[16]: SDIO1_RSTN select signal. Default:  1 
														// 0: SDIO1_WP 
														// 1: GPIOC[30] 
														// In reset state, it is GPIOC[30] as input, weak pull up 

	ARM_PINMUX_SD1_REVD_SHIFT 		= 17				// Bit[31:17]: Reserved
}E_ARM_PINMUX_SD1_SHIFT;

// sd1 control pin mask and value
#define ARM_PINMUX_SD1_CTRL_MASK	(0x1C003<<ARM_PINMUX_SD1_CLK_GPIOC10_SHIFT)

// sd1 bus width pin mask and value
#define ARM_PINMUX_SD1_1BIT_MASK	(ARM_PINMUX_SD1_CTRL_MASK |\
									(0x1<<ARM_PINMUX_SD1_D0_GPIOC12_SHIFT))

#define ARM_PINMUX_SD1_4BIT_MASK	(ARM_PINMUX_SD1_CTRL_MASK |\
									(0xF<<ARM_PINMUX_SD1_D0_GPIOC12_SHIFT))

#define ARM_PINMUX_SD1_8BIT_MASK	(ARM_PINMUX_SD1_CTRL_MASK |\
									(0xFFF<<ARM_PINMUX_SD1_D0_GPIOC12_SHIFT))

// gpioc pin mask
#define ARM_PINMUX_GPIOC_PIN10_MASK	(1<<ARM_PINMUX_SD1_CLK_GPIOC10_SHIFT)
#define ARM_PINMUX_GPIOC_PIN11_MASK	(1<<ARM_PINMUX_SD1_CMD_GPIOC11_SHIFT)
#define ARM_PINMUX_GPIOC_PIN12_MASK	(1<<ARM_PINMUX_SD1_D0_GPIOC12_SHIFT)
#define ARM_PINMUX_GPIOC_PIN13_MASK	(1<<ARM_PINMUX_SD1_D1_GPIOC13_SHIFT)
#define ARM_PINMUX_GPIOC_PIN14_MASK	(1<<ARM_PINMUX_SD1_D2_GPIOC14_SHIFT)
#define ARM_PINMUX_GPIOC_PIN15_MASK	(1<<ARM_PINMUX_SD1_D3_GPIOC15_SHIFT)
#define ARM_PINMUX_GPIOC_PIN16_TIMER0_IN_MASK	(3<<ARM_PINMUX_SD1_D4_GPIOC16_TIMER0_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN17_TIMER1_IN_MASK	(3<<ARM_PINMUX_SD1_D5_GPIOC17_TIMER1_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN18_TIMER2_IN_MASK	(3<<ARM_PINMUX_SD1_D6_GPIOC18_TIMER2_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN19_TIMER3_IN_MASK	(3<<ARM_PINMUX_SD1_D7_GPIOC19_TIMER3_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN27_MASK	(1<<ARM_PINMUX_SD1_CD_GPIOC27_SHIFT)
#define ARM_PINMUX_GPIOC_PIN28_MASK	(1<<ARM_PINMUX_SD1_WP_GPIOC28_SHIFT)
#define ARM_PINMUX_GPIOC_PIN30_MASK	(1<<ARM_PINMUX_SD1_RSTN_GPIOC30_SHIFT)

#define ARM_PINMUX_GPIOC_PIN10_VAL	(1<<ARM_PINMUX_SD1_CLK_GPIOC10_SHIFT)
#define ARM_PINMUX_GPIOC_PIN11_VAL	(1<<ARM_PINMUX_SD1_CMD_GPIOC11_SHIFT)
#define ARM_PINMUX_GPIOC_PIN12_VAL	(1<<ARM_PINMUX_SD1_D0_GPIOC12_SHIFT)
#define ARM_PINMUX_GPIOC_PIN13_VAL	(1<<ARM_PINMUX_SD1_D1_GPIOC13_SHIFT)
#define ARM_PINMUX_GPIOC_PIN14_VAL	(1<<ARM_PINMUX_SD1_D2_GPIOC14_SHIFT)
#define ARM_PINMUX_GPIOC_PIN15_VAL	(1<<ARM_PINMUX_SD1_D3_GPIOC15_SHIFT)
#define ARM_PINMUX_GPIOC_PIN16_VAL	(3<<ARM_PINMUX_SD1_D4_GPIOC16_TIMER0_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN17_VAL	(3<<ARM_PINMUX_SD1_D5_GPIOC17_TIMER1_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN18_VAL	(3<<ARM_PINMUX_SD1_D6_GPIOC18_TIMER2_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN19_VAL	(3<<ARM_PINMUX_SD1_D7_GPIOC19_TIMER3_IN_SHIFT)
#define ARM_PINMUX_GPIOC_PIN27_VAL	(1<<ARM_PINMUX_SD1_CD_GPIOC27_SHIFT)
#define ARM_PINMUX_GPIOC_PIN28_VAL	(1<<ARM_PINMUX_SD1_WP_GPIOC28_SHIFT)
#define ARM_PINMUX_GPIOC_PIN30_VAL	(1<<ARM_PINMUX_SD1_RSTN_GPIOC30_SHIFT)

// timer
#define ARM_PINMUX_TIMER0_IN_VAL	(1<<ARM_PINMUX_SD1_D4_GPIOC16_TIMER0_IN_SHIFT)
#define ARM_PINMUX_TIMER1_IN_VAL	(1<<ARM_PINMUX_SD1_D5_GPIOC17_TIMER1_IN_SHIFT)
#define ARM_PINMUX_TIMER2_IN_VAL	(1<<ARM_PINMUX_SD1_D6_GPIOC18_TIMER2_IN_SHIFT)
#define ARM_PINMUX_TIMER3_IN_VAL	(1<<ARM_PINMUX_SD1_D7_GPIOC19_TIMER3_IN_SHIFT)

// offset: 0x00A8, ARM_UART_PINMUX register
typedef enum
{
	
	ARM_PINMUX_UART0_TXD_GPIOA0_SHIFT 	= 0,	// Bit[0](uart0_iodis[0]): UART0_TXD select signal. Default :0 
												// 0: UART0_TXD 
												// 1: GPIOA[0] 
												// In reset state, it is UART0_TXD as output,weak pull up

	ARM_PINMUX_UART0_RXD_GPIOA1_SHIFT 	= 1, 	// Bit[1](uart0_iodis[1]): UART0_RXD select signal. Default: 0 
												// 0: UART0_RXD 
												// 1: GPIOA[1] 
												// In reset state, it is UART0_RXD as input,weak pull up

	ARM_PINMUX_UART0_CTSN_GPIOA2_SHIFT 	= 2,	// Bit[2](uart0_iodis[2]):  UART0_CTSN select signal. Default:0 
												// 0: UART0_CTSN 
												// 1: GPIOA[2] 
												// In reset state, it is UART0_CTSN as input,weak pull up 

	ARM_PINMUX_UART0_RTSN_GPIOA3_SHIFT 	= 3,	// Bit[3](uart0_iodis[3]): UART0_RTSN select signal. Default: 0 
												// 0: UART0_RTSN 
												// 1: GPIOA[3] 
												// In reset state, it is UART0_RTSN as output,weak pull up

	ARM_PINMUX_UART1_TXD_GPIOA4_SHIFT 	= 4,	// Bit[4](uart1_iodis[0]): UART1_TXD select signal. Default: 1 
												// 0: UART1_TXD 
												// 1: GPIOA[4] 
												// In reset state, it is GPIOA[4] as input,weak pull up

	ARM_PINMUX_UART1_RXD_GPIOA5_SHIFT 	= 5,	// Bit[5](uart1_iodis[1]): UART1_RXD select signal. Default:1
												// 0: UART1_RXD 
												// 1: GPIOA[5] 
												// In reset state, it is GPIOA[5] as input,weak pull up

	ARM_PINMUX_UART1_CTSN_GPIOA6_SHIFT 	= 6,	// Bit[6](uart1_iodis[2]):  UART1_CTSN select signal. Default:1 
												// 0: UART1_CTSN 
												// 1: GPIOA[6] 
												// In reset state, it is GPIOA[6] as input,weak pull up

	ARM_PINMUX_UART1_RTSN_GPIOA7_SHIFT 	= 7,	// Bit[7](uart1_iodis[3]):  UART1_RTSN select signal. Default:1 
												// 0: UART1_RTSN 
												// 1: GPIOA[7] 
												// In reset state, it is GPIOA[7] as input,weak pull up  

	ARM_PINMUX_UART2_TXD_GPIOA8_SHIFT 	= 8,	//Bit[8](uart2_iodis[0]): UART2_TXD select signal. Default: 1 
												// 0: UART2_TXD 
												// 1: GPIOA[8] 
												// In reset state, it is GPIOA[8] as input,weak pull up

	ARM_PINMUX_UART2_RXD_GPIOA9_SHIFT 	= 9,	// Bit[9](uart2_iodis[1]): UART2_RXD select signal. Default: 1 
												// 0: UART2_RXD 
												// 1: GPIOA[9] 
												// In reset state, it is GPIOA[9] as input,weak pull up 

	ARM_PINMUX_UART2_CTSN_GPIOA10_SHIFT = 10,	// Bit[10](uart2_iodis[2]):  UART2_CTSN select signal. Default: 1 
												// 0: UART2_CTSN 
												// 1: GPIOA[10] 
												// In reset state, it is GPIOA[10] as input,weak pull up

	ARM_PINMUX_UART2_RTSN_GPIOA11_SHIFT = 11,	// Bit[11](uart2_iodis[3]):  UART2_RTSN select signal. Default:1 
												// 0: UART2_RTSN 
												// 1: GPIOA[11] 
												// In reset state, it is GPIOA[11] as input,weak pull up

	ARM_PINMUX_UART3_TXD_GPIOA12_SHIFT = 12,	// Bit[12](uart3_iodis[0]):  UART3_TXD select signal. Default:1 
												// 0: UART3_TXD 
												// 1: GPIOA[12] 
												// In reset state, it is GPIOA[12] as input,weak pull up 

	ARM_PINMUX_UART3_RXD_GPIOA13_SHIFT = 13,	// Bit[13](uart3_iodis[1]):  UART3_RXD select signal. Default:1 
												// 0: UART3_RXD 
												// 1: GPIOA[13] 
												// In reset state, it is GPIOA[13] as input,weak pull up 

	ARM_PINMUX_UART3_CTSN_GPIOA14_SHIFT = 14,	// Bit[14](uart3_iodis[2]):  UART3_CTSN select signal. Default:1 
												// 0: UART3_CTSN 
												// 1: GPIOA[14] 
												// In reset state, it is GPIOA[14] as input,weak pull up

	ARM_PINMUX_UART3_RTSN_GPIOA15_SHIFT = 15,	// Bit[15](uart3_iodis[3]):  UART3_RTSN select signal. Default:1 
												// 0: UART3_RTSN 
												// 1: GPIOA[15] 
												// In reset state, it is GPIOA[15] as input,weak pull up 	

	ARM_PINMUX_UART3_DCDN_GPIOA16_SHIFT = 16,	// Bit[16](uart3_iodis[4]):  UART3_DCDN select signal. Default:1 
												// 0: UART3_DCDN 
												// 1: GPIOA[16] 
												// In reset state, it is GPIOA[16] as input,weak pull up

	ARM_PINMUX_UART3_DSR_GPIOA17_SHIFT = 17,	// Bit[17](uart3_iodis[5]): UART3_DSR  select signal. Default: 1 
												// 0: UART3_DSR 
												// 1: GPIOA[17] 
												// In reset state, it is GPIOA[17] as input,weak pull up	

	ARM_PINMUX_UART3_DTR_GPIOA18_SHIFT = 18,	// Bit[18](uart3_iodis[6]): UART3_DTR  select signal. Default: 1 
												// 0: UART3_DTR 
												// 1: GPIOA[18] 
												// In reset state, it is GPIOA[18] as an input,weak pull up 	

	ARM_PINMUX_UART3_RIN_GPIOA19_SHIFT = 19,	// Bit[19](uart3_iodis[7]): UART3_RIN select signal. Default: 1 
												// 0: UART3_RIN 
												// 1: GPIOA[19] 
												// In reset state, it is GPIOA[19] as input,weak pull up 	

	ARM_PINMUX_UART_REVD_SHIFT			= 20	// Bit[31:20]: Reserved
}E_ARM_PINMUX_UART_SHIFT;

// uart0/1/2/3 mask
#define ARM_PINMUX_UART0_MASK	(0xF)
#define ARM_PINMUX_UART1_MASK	(0xF0)
#define ARM_PINMUX_UART2_MASK	(0xF00)
#define ARM_PINMUX_UART3_MASK	(0xFF000)

// uart0/1/2/3 value
#define ARM_PINMUX_UART0_VAL	(0)
#define ARM_PINMUX_UART1_VAL	(0)
#define ARM_PINMUX_UART2_VAL	(0)
#define ARM_PINMUX_UART3_VAL	(0)

// gpioa mask
#define ARM_PINMUX_GPIOA_PIN0_MASK	(1<<ARM_PINMUX_UART0_TXD_GPIOA0_SHIFT)
#define ARM_PINMUX_GPIOA_PIN1_MASK	(1<<ARM_PINMUX_UART0_RXD_GPIOA1_SHIFT)
#define ARM_PINMUX_GPIOA_PIN2_MASK	(1<<ARM_PINMUX_UART0_CTSN_GPIOA2_SHIFT)
#define ARM_PINMUX_GPIOA_PIN3_MASK	(1<<ARM_PINMUX_UART0_RTSN_GPIOA3_SHIFT)
#define ARM_PINMUX_GPIOA_PIN4_MASK	(1<<ARM_PINMUX_UART1_TXD_GPIOA4_SHIFT)
#define ARM_PINMUX_GPIOA_PIN5_MASK	(1<<ARM_PINMUX_UART1_RXD_GPIOA5_SHIFT)
#define ARM_PINMUX_GPIOA_PIN6_MASK	(1<<ARM_PINMUX_UART1_CTSN_GPIOA6_SHIFT)
#define ARM_PINMUX_GPIOA_PIN7_MASK	(1<<ARM_PINMUX_UART1_RTSN_GPIOA7_SHIFT)
#define ARM_PINMUX_GPIOA_PIN8_MASK	(1<<ARM_PINMUX_UART2_TXD_GPIOA8_SHIFT)
#define ARM_PINMUX_GPIOA_PIN9_MASK	(1<<ARM_PINMUX_UART2_RXD_GPIOA9_SHIFT)
#define ARM_PINMUX_GPIOA_PIN10_MASK	(1<<ARM_PINMUX_UART2_CTSN_GPIOA10_SHIFT)
#define ARM_PINMUX_GPIOA_PIN11_MASK	(1<<ARM_PINMUX_UART2_RTSN_GPIOA11_SHIFT)
#define ARM_PINMUX_GPIOA_PIN12_MASK	(1<<ARM_PINMUX_UART3_TXD_GPIOA12_SHIFT)
#define ARM_PINMUX_GPIOA_PIN13_MASK	(1<<ARM_PINMUX_UART3_RXD_GPIOA13_SHIFT)
#define ARM_PINMUX_GPIOA_PIN14_MASK	(1<<ARM_PINMUX_UART3_CTSN_GPIOA14_SHIFT)
#define ARM_PINMUX_GPIOA_PIN15_MASK	(1<<ARM_PINMUX_UART3_RTSN_GPIOA15_SHIFT)
#define ARM_PINMUX_GPIOA_PIN16_MASK	(1<<ARM_PINMUX_UART3_DCDN_GPIOA16_SHIFT)
#define ARM_PINMUX_GPIOA_PIN17_MASK	(1<<ARM_PINMUX_UART3_DSR_GPIOA17_SHIFT)
#define ARM_PINMUX_GPIOA_PIN18_MASK	(1<<ARM_PINMUX_UART3_DTR_GPIOA18_SHIFT)
#define ARM_PINMUX_GPIOA_PIN19_MASK	(1<<ARM_PINMUX_UART3_RIN_GPIOA19_SHIFT)

// gpioa value
#define ARM_PINMUX_GPIOA_PIN0_VAL	(1<<ARM_PINMUX_UART0_TXD_GPIOA0_SHIFT)
#define ARM_PINMUX_GPIOA_PIN1_VAL	(1<<ARM_PINMUX_UART0_RXD_GPIOA1_SHIFT)
#define ARM_PINMUX_GPIOA_PIN2_VAL	(1<<ARM_PINMUX_UART0_CTSN_GPIOA2_SHIFT)
#define ARM_PINMUX_GPIOA_PIN3_VAL	(1<<ARM_PINMUX_UART0_RTSN_GPIOA3_SHIFT)
#define ARM_PINMUX_GPIOA_PIN4_VAL	(1<<ARM_PINMUX_UART1_TXD_GPIOA4_SHIFT)
#define ARM_PINMUX_GPIOA_PIN5_VAL	(1<<ARM_PINMUX_UART1_RXD_GPIOA5_SHIFT)
#define ARM_PINMUX_GPIOA_PIN6_VAL	(1<<ARM_PINMUX_UART1_CTSN_GPIOA6_SHIFT)
#define ARM_PINMUX_GPIOA_PIN7_VAL	(1<<ARM_PINMUX_UART1_RTSN_GPIOA7_SHIFT)
#define ARM_PINMUX_GPIOA_PIN8_VAL	(1<<ARM_PINMUX_UART2_TXD_GPIOA8_SHIFT)
#define ARM_PINMUX_GPIOA_PIN9_VAL	(1<<ARM_PINMUX_UART2_RXD_GPIOA9_SHIFT)
#define ARM_PINMUX_GPIOA_PIN10_VAL	(1<<ARM_PINMUX_UART2_CTSN_GPIOA10_SHIFT)
#define ARM_PINMUX_GPIOA_PIN11_VAL	(1<<ARM_PINMUX_UART2_RTSN_GPIOA11_SHIFT)
#define ARM_PINMUX_GPIOA_PIN12_VAL	(1<<ARM_PINMUX_UART3_TXD_GPIOA12_SHIFT)
#define ARM_PINMUX_GPIOA_PIN13_VAL	(1<<ARM_PINMUX_UART3_RXD_GPIOA13_SHIFT)
#define ARM_PINMUX_GPIOA_PIN14_VAL	(1<<ARM_PINMUX_UART3_CTSN_GPIOA14_SHIFT)
#define ARM_PINMUX_GPIOA_PIN15_VAL	(1<<ARM_PINMUX_UART3_RTSN_GPIOA15_SHIFT)
#define ARM_PINMUX_GPIOA_PIN16_VAL	(1<<ARM_PINMUX_UART3_DCDN_GPIOA16_SHIFT)
#define ARM_PINMUX_GPIOA_PIN17_VAL	(1<<ARM_PINMUX_UART3_DSR_GPIOA17_SHIFT)
#define ARM_PINMUX_GPIOA_PIN18_VAL	(1<<ARM_PINMUX_UART3_DTR_GPIOA18_SHIFT)
#define ARM_PINMUX_GPIOA_PIN19_VAL	(1<<ARM_PINMUX_UART3_RIN_GPIOA19_SHIFT)

// offset: 0x00AC, ARM_SPI_PINMUX register
typedef enum
{
	
	ARM_PINMUX_SPI0_CLK_GPIOB0_SHIFT = 0, 		// Bit[0](spi0_iodis[0]): SPI0_CLK select signal. Default: 0 
												// 0: SPI0_CLK 
												// 1: GPIOB[0] 
												// In reset state, it is SPI0_CLK as output

	ARM_PINMUX_SPI0_CSN0_GPIOB1_SHIFT = 1, 		// Bit[1](spi0_iodis[1]): SPI0_CSN0 select signal. Default: 0 
												// 0: SPI0_CSN0 
												// 1: GPIOB[1] 
												// In reset state, it is SPI0_CSN0 as output 

	ARM_PINMUX_SPI0_CSN1_GPIOB2_SHIFT 	= 2,	// Bit[2](spi0_iodis[2]): SPI0_CSN1 select signal. Default: 0 
												// 0: SPI0_CSN1 
												// 1: GPIOB[2] 
												// In reset state, it is SPI0_CSN1 as output 

	ARM_PINMUX_SPI0_D0_GPIOB3_SHIFT 	= 3,	// Bit[3](spi0_iodis[3]): SPI0_DIO0 select signal. Default: 0 
												// 0: SPI0_DIO0 
												// 1: GPIOB[3] 
												// In reset state, it is SPI0_DIO0 as input 

	ARM_PINMUX_SPI0_D1_GPIOB4_SHIFT 	= 4,	// Bit[4](spi0_iodis[4]): SPI0_DIO1 select signal. Default: 0 
												// 0: SPI0_DIO1 
												// 1: GPIOB[4] 
												// In reset state, it is SPI0_DIO1 as input

	ARM_PINMUX_SPI0_D2_GPIOB5_SHIFT 	= 5,	// Bit[5](spi0_iodis[5]): SPI0_DIO2 select signal. Default: 0 
												// 0: SPI0_DIO2 
												// 1: GPIOB[5] 
												// In reset state, it is SPI0_DIO2 as input 

	ARM_PINMUX_SPI0_D3_GPIOB6_SHIFT 	= 6,	// Bit[6](spi0_iodis[6]): SPI0_DIO3 select signal. Default: 0 
												// 0: SPI0_DIO3 
												// 1: GPIOB[6] 
												// In reset state, it is SPI0_DIO3 as input 

	ARM_PINMUX_SPI0_REVD0_SHIFT 		= 7,	// Bit[7]: reserved  

	ARM_PINMUX_SPI1_CLK_GPIOB7_SHIFT 	= 8,	// Bit[8](spi1_iodis[0]): SPI1_CLK  select signal.Default:1 
												// 0: SPI1_CLK 
												// 1: GPIOB[7] 
												// In reset state, it is GPIOB[7] as input, weak pull up

	ARM_PINMUX_SPI1_CSN0_GPIOB8_SHIFT 	= 9,	// Bit[9](spi1_iodis[1]): SPI1_CSN0  select signal. Default: 1 
												// 0: SPI1_CSN0 
												// 1: GPIOB[8] 
												// In reset state, it is GPIOB[8] as input, weak pull up

	ARM_PINMUX_SPI1_CSN1_GPIOB9_SHIFT 	= 10,	// Bit[10](spi1_iodis[2]): SPI1_CSN1  select signal. Default: 1 
												// 0: SPI1_CSN1 
												// 1: GPIOB[9] 
												// In reset state, it is GPIOB[9] as input, weak pull up

	ARM_PINMUX_SPI1_D0_GPIOB10_SHIFT 	= 11,	// Bit[11](spi1_iodis[3]): SPI1_DIO0 select signal. Default:1 
												// 0: SPI1_DIO0 
												// 1: GPIOB[10] 
												// In reset state, it is GPIOB[10] as input, weak pull up 

	ARM_PINMUX_SPI1_D1_GPIOB11_SHIFT 	= 12,	// Bit[12](spi1_iodis[4]): SPI1_DIO1 select signal. Default:1 
												// 0: SPI1_DIO1 
												// 1: GPIOB[11] 
												// In reset state, it is GPIOB[11] as input,weak pull up 

	ARM_PINMUX_SPI1_REVD0_SHIFT 		= 13,	// Bit[15:13]: Reserved. 


	ARM_PINMUX_SPI2_CLK_GPIOB12_SHIFT 	= 16,	// Bit[16](spi2_iodis[0]): SPI2_CLK select signal. Default: 1 
												// 0: SPI2_CLK 
												// 1: GPIOB[12] 
												// In reset state, it is GPIOB[12] as input, weak pull up 

	ARM_PINMUX_SPI2_CSN0_GPIOB13_SHIFT 	= 17,	// Bit[17](spi2_iodis[1]): SPI2_CSN0 select signal. Default: 1 
												// 0: SPI2_CSN0 
												// 1: GPIOB[13] 
												// In reset state, it is GPIOB[13] as input, weak pull up	

	ARM_PINMUX_SPI2_CSN1_GPIOB14_SHIFT 	= 18,	// Bit[18](spi2_iodis[2]): SPI2_CSN1 select signal. Default: 1 
												// 0: SPI2_CSN1 
												// 1: GPIOB[14] 
												// In reset state, it is GPIOB[14] as input, weak pull up 	

	ARM_PINMUX_SPI2_D0_GPIOB15_SHIFT 	= 19,	// Bit[19](spi2_iodis[3]): SPI2_DIO0 select signal. Default: 1 
												// 0: SPI2_DIO0 
												// 1: GPIOB[15] 
												// In reset state, it is GPIOB[15] as input, weak pull up  	

	ARM_PINMUX_SPI2_D1_GPIOB16_SHIFT 	= 20,	// Bit[20](spi2_iodis[4]): SPI2_DIO1 select signal. Default:1 
												// 0: SPI2_DIO1 
												// 1: GPIOB[16] 
												// In reset state, it is GPIOB[16] as input, weak pull up 

	ARM_PINMUX_SPI2_REVD_SHIFT 			= 21	// Bit[31:21]: Reserved. 
}E_ARM_PINMUX_SPI_SHIFT;

// spi0/1/2 mask
#define ARM_PINMUX_SPI0_MASK	(0x7F)
#define ARM_PINMUX_SPI1_MASK	(0x1F00)
#define ARM_PINMUX_SPI2_MASK	(0x1F0000)

// spi0/1/2 value
#define ARM_PINMUX_SPI0_VAL	(0)
#define ARM_PINMUX_SPI1_VAL	(0)
#define ARM_PINMUX_SPI2_VAL	(0)

// gpiob mask
#define ARM_PINMUX_GPIOB_PIN0_MASK	(1<<ARM_PINMUX_SPI0_CLK_GPIOB0_SHIFT)
#define ARM_PINMUX_GPIOB_PIN1_MASK	(1<<ARM_PINMUX_SPI0_CSN0_GPIOB1_SHIFT)
#define ARM_PINMUX_GPIOB_PIN2_MASK	(1<<ARM_PINMUX_SPI0_CSN1_GPIOB2_SHIFT)
#define ARM_PINMUX_GPIOB_PIN3_MASK	(1<<ARM_PINMUX_SPI0_D0_GPIOB3_SHIFT)
#define ARM_PINMUX_GPIOB_PIN4_MASK	(1<<ARM_PINMUX_SPI0_D1_GPIOB4_SHIFT)
#define ARM_PINMUX_GPIOB_PIN5_MASK	(1<<ARM_PINMUX_SPI0_D2_GPIOB5_SHIFT)
#define ARM_PINMUX_GPIOB_PIN6_MASK	(1<<ARM_PINMUX_SPI0_D3_GPIOB6_SHIFT)
#define ARM_PINMUX_GPIOB_PIN7_MASK	(1<<ARM_PINMUX_SPI1_CLK_GPIOB7_SHIFT)
#define ARM_PINMUX_GPIOB_PIN8_MASK	(1<<ARM_PINMUX_SPI1_CSN0_GPIOB8_SHIFT)
#define ARM_PINMUX_GPIOB_PIN9_MASK	(1<<ARM_PINMUX_SPI1_CSN1_GPIOB9_SHIFT)
#define ARM_PINMUX_GPIOB_PIN10_MASK	(1<<ARM_PINMUX_SPI1_D0_GPIOB10_SHIFT)
#define ARM_PINMUX_GPIOB_PIN11_MASK	(1<<ARM_PINMUX_SPI1_D1_GPIOB11_SHIFT)
#define ARM_PINMUX_GPIOB_PIN12_MASK	(1<<ARM_PINMUX_SPI2_CLK_GPIOB12_SHIFT)
#define ARM_PINMUX_GPIOB_PIN13_MASK	(1<<ARM_PINMUX_SPI2_CSN0_GPIOB13_SHIFT)
#define ARM_PINMUX_GPIOB_PIN14_MASK	(1<<ARM_PINMUX_SPI2_CSN1_GPIOB14_SHIFT)
#define ARM_PINMUX_GPIOB_PIN15_MASK	(1<<ARM_PINMUX_SPI2_D0_GPIOB15_SHIFT)
#define ARM_PINMUX_GPIOB_PIN16_MASK	(1<<ARM_PINMUX_SPI2_D1_GPIOB16_SHIFT)

// gpiob value
#define ARM_PINMUX_GPIOB_PIN0_VAL	(1<<ARM_PINMUX_SPI0_CLK_GPIOB0_SHIFT)
#define ARM_PINMUX_GPIOB_PIN1_VAL	(1<<ARM_PINMUX_SPI0_CSN0_GPIOB1_SHIFT)
#define ARM_PINMUX_GPIOB_PIN2_VAL	(1<<ARM_PINMUX_SPI0_CSN1_GPIOB2_SHIFT)
#define ARM_PINMUX_GPIOB_PIN3_VAL	(1<<ARM_PINMUX_SPI0_D0_GPIOB3_SHIFT)
#define ARM_PINMUX_GPIOB_PIN4_VAL	(1<<ARM_PINMUX_SPI0_D1_GPIOB4_SHIFT)
#define ARM_PINMUX_GPIOB_PIN5_VAL	(1<<ARM_PINMUX_SPI0_D2_GPIOB5_SHIFT)
#define ARM_PINMUX_GPIOB_PIN6_VAL	(1<<ARM_PINMUX_SPI0_D3_GPIOB6_SHIFT)
#define ARM_PINMUX_GPIOB_PIN7_VAL	(1<<ARM_PINMUX_SPI1_CLK_GPIOB7_SHIFT)
#define ARM_PINMUX_GPIOB_PIN8_VAL	(1<<ARM_PINMUX_SPI1_CSN0_GPIOB8_SHIFT)
#define ARM_PINMUX_GPIOB_PIN9_VAL	(1<<ARM_PINMUX_SPI1_CSN1_GPIOB9_SHIFT)
#define ARM_PINMUX_GPIOB_PIN10_VAL	(1<<ARM_PINMUX_SPI1_D0_GPIOB10_SHIFT)
#define ARM_PINMUX_GPIOB_PIN11_VAL	(1<<ARM_PINMUX_SPI1_D1_GPIOB11_SHIFT)
#define ARM_PINMUX_GPIOB_PIN12_VAL	(1<<ARM_PINMUX_SPI2_CLK_GPIOB12_SHIFT)
#define ARM_PINMUX_GPIOB_PIN13_VAL	(1<<ARM_PINMUX_SPI2_CSN0_GPIOB13_SHIFT)
#define ARM_PINMUX_GPIOB_PIN14_VAL	(1<<ARM_PINMUX_SPI2_CSN1_GPIOB14_SHIFT)
#define ARM_PINMUX_GPIOB_PIN15_VAL	(1<<ARM_PINMUX_SPI2_D0_GPIOB15_SHIFT)
#define ARM_PINMUX_GPIOB_PIN16_VAL	(1<<ARM_PINMUX_SPI2_D1_GPIOB16_SHIFT)

// offset: 0x00B0, ARM_ITS_PINMUX(I2C&TIMER&SYSTEM) register
typedef enum
{
	
	ARM_PINMUX_I2C0_GPIOA20_21_SHIFT 		= 0, 	// Bit[0](i2c0_iodis): I2C0_SDA/I2C0_SCL select signal. Default:1 
													// 0: I2C0_SDA, I2C0_SCL 
													// 1: GPIOA[20], GPIOA[21] 
													// In reset state, it is GPIOA[20] as input,weak pull up 
													// In reset state, it is GPIOA[21] as input,weak pull up

	ARM_PINMUX_I2C1_GPIOA22_23_SHIFT 		= 1, 	// Bit[1](i2c1_iodis): I2C1_SDA/I2C1_SCL select signal. Default:1 
													// 0: I2C1_SDA, I2C1_SCL 
													// 1: GPIOA[22],  GPIOA[23] 
													// In reset state, it is GPIOA[22] as input,weak pull up 
													// In reset state, it is GPIOA[23] as input,weak pull up 
 

	ARM_PINMUX_I2C2_GPIOA24_25_SHIFT 		= 2,	// Bit[2](i2c2_iodis): I2C2_SDA/I2C2_SCL select signal. Default:1 
													// 0: I2C2_SDA, I2C2_SCL 
													// 1: GPIOA[24], GPIOA[25] 
													// In reset state, it is GPIOA[24] as input,weak pull up 
													// In reset state, it is GPIOA[25] as input,weak pull up 
 
	ARM_PINMUX_ITS_REVD0_SHIFT 				= 3,	// Bit[3]: Reserved.

	ARM_PINMUX_TIMER_W_OUT_GPIOD0_SHIFT 	= 4,	// Bit[4](timer_w_iodis): TIMER_W_OUT select signal. Default: 1 
													// 0: TIMER_W_OUT 
													// 1: GPIOD[0] 
													// In reset state, it is GPIOD[0] as input,weak pull up 

	ARM_PINMUX_TIMER_X_OUT_GPIOD1_SHIFT 	= 5,	// Bit[5](timer_x_iodis): TIMER_X_OUT select signal. Default: 1 
													// 0: TIMER_X_OUT 
													// 1: GPIOD[1] 
													// In reset state, it is GPIOD[1] as input,weak pull up

	ARM_PINMUX_TIMER_Y_OUT_GPIOD2_SHIFT 	= 6,	// Bit[6](timer_y_iodis): TIMER_Y_OUT select signal. Default: 1 
													// 0: TIMER_Y_OUT 
													// 1: GPIOD[2] 
													// In reset state, it is GPIOD[2] as input,weak pull up

	ARM_PINMUX_TIMER_Z_OUT_GPIOD3_SHIFT 	= 7,	// Bit[7](timer_z_iodis): TIMER_Z_OUT select signal. Default:1 
													// 0: TIMER_Z_OUT 
													// 1: GPIOD[3] 
													// In reset state, it is GPIOD[3] as input,weak pull up 

	ARM_PINMUX_RTC_CLKOUT_GPIOD31_SHIFT 	= 8,	// Bit[8](rtc_clkout_iodis): RTC_CLKOUT select signal. Default:1 
													// 0: RTC_CLKOUT 
													// 1: GPIOD[31] 
													// In reset state, it is GPIOD[31] as input,weak pull up

	ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_SHIFT = 9,	// Bit[9](rtc_cal_clk_iodis): RTC_CAL_CLKOUT select signal. Default: 1 
													// 0: RTC_CAL_OUT 
													// 1: GPIOD[29] 
													// In reset state, it is GPIOD[29] as input,weak pull up 

	ARM_PINMUX_CLKOUT_GPIOD30_SHIFT 		= 10,	// Bit[10](clkout_iodis): CLKOUT select signal. Default: 1 
													// 0: CLKOUT
													// 1: GPIOD[30] 
													// In reset state, it is GPIOD[30] as input,weak pull up 


	ARM_PINMUX_ITS_REVD1_SHIFT				= 11,	// Bit[11]: Reserved. 

	ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT 		= 12,	// Bit[12](arm_sleep_iodis): HOST_LOW_POWER_OUT select signal. Default: 0 
	ARM_PINMUX_EPF_SLEEP_MODE_DONE_GPIOA29_31_SHIFT 	= 12,
																// 0: HOST_LOW_POWER_OUT, ARM_EPF_SLEEP_MODE_DONE 
																// 1: GPIOA[29], GPIOA[31]  
																// In reset state, it is GPIOA[29] as input,weak pull up 
																// In reset state, it is GPIOA[31] as input,weak pull up  

	ARM_PINMUX_EPF_FAIL_TIMER_OUT_GPIOB30_SHIFT = 13,	// Bit[13](epf_failsafe_iodis): EPF_FAILSAFE_TIMER_OUT select signal. Default: 0 
														// 0: EPF_FAILSAFE_TIMER_OUT 
														// 1: GPIOA[30] 
														// In reset state, it is GPIOA[30] as input,weak pull up  


	ARM_PINMUX_ITS_REVD2_SHIFT 					= 14	// Bit[31:14]:Reserved. 
}E_ARM_PINMUX_ITS_SHIFT;

// i2c0/1/2 mask
#define ARM_PINMUX_I2C0_MASK	(1<<ARM_PINMUX_I2C0_GPIOA20_21_SHIFT)
#define ARM_PINMUX_I2C1_MASK	(1<<ARM_PINMUX_I2C1_GPIOA22_23_SHIFT)
#define ARM_PINMUX_I2C2_MASK	(1<<ARM_PINMUX_I2C2_GPIOA24_25_SHIFT)

// i2c0/1/2 value
#define ARM_PINMUX_I2C0_VAL	(0)
#define ARM_PINMUX_I2C1_VAL	(0)
#define ARM_PINMUX_I2C2_VAL	(0)

// gpioa mask and value
#define ARM_PINMUX_GPIOA_PIN20_MASK	(1<<ARM_PINMUX_I2C0_GPIOA20_21_SHIFT)
#define ARM_PINMUX_GPIOA_PIN21_MASK	(1<<ARM_PINMUX_I2C0_GPIOA20_21_SHIFT)
#define ARM_PINMUX_GPIOA_PIN22_MASK	(1<<ARM_PINMUX_I2C1_GPIOA22_23_SHIFT)
#define ARM_PINMUX_GPIOA_PIN23_MASK	(1<<ARM_PINMUX_I2C1_GPIOA22_23_SHIFT)
#define ARM_PINMUX_GPIOA_PIN24_MASK	(1<<ARM_PINMUX_I2C2_GPIOA24_25_SHIFT)
#define ARM_PINMUX_GPIOA_PIN25_MASK	(1<<ARM_PINMUX_I2C2_GPIOA24_25_SHIFT)
#define ARM_PINMUX_GPIOA_PIN29_MASK	(1<<ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT)
#define ARM_PINMUX_GPIOA_PIN30_MASK	(1<<ARM_PINMUX_EPF_FAIL_TIMER_OUT_GPIOB30_SHIFT)
#define ARM_PINMUX_GPIOA_PIN31_MASK	(1<<ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT)

#define ARM_PINMUX_GPIOA_PIN20_VAL	(1<<ARM_PINMUX_I2C0_GPIOA20_21_SHIFT)
#define ARM_PINMUX_GPIOA_PIN21_VAL	(1<<ARM_PINMUX_I2C0_GPIOA20_21_SHIFT)
#define ARM_PINMUX_GPIOA_PIN22_VAL	(1<<ARM_PINMUX_I2C1_GPIOA22_23_SHIFT)
#define ARM_PINMUX_GPIOA_PIN23_VAL	(1<<ARM_PINMUX_I2C1_GPIOA22_23_SHIFT)
#define ARM_PINMUX_GPIOA_PIN24_VAL	(1<<ARM_PINMUX_I2C2_GPIOA24_25_SHIFT)
#define ARM_PINMUX_GPIOA_PIN25_VAL	(1<<ARM_PINMUX_I2C2_GPIOA24_25_SHIFT)
#define ARM_PINMUX_GPIOA_PIN29_VAL	(1<<ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT)
#define ARM_PINMUX_GPIOA_PIN30_VAL	(1<<ARM_PINMUX_EPF_FAIL_TIMER_OUT_GPIOB30_SHIFT)
#define ARM_PINMUX_GPIOA_PIN31_VAL	(1<<ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT)

// gpiod mask and value
#define ARM_PINMUX_GPIOD_PIN0_TIMERW_OUT_MASK	(1<<ARM_PINMUX_TIMER_W_OUT_GPIOD0_SHIFT)
#define ARM_PINMUX_GPIOD_PIN1_TIMERX_OUT_MASK	(1<<ARM_PINMUX_TIMER_X_OUT_GPIOD1_SHIFT)
#define ARM_PINMUX_GPIOD_PIN2_TIMERY_OUT_MASK	(1<<ARM_PINMUX_TIMER_Y_OUT_GPIOD2_SHIFT)
#define ARM_PINMUX_GPIOD_PIN3_TIMERZ_OUT_MASK	(1<<ARM_PINMUX_TIMER_Z_OUT_GPIOD3_SHIFT)
#define ARM_PINMUX_RTC_CLKOUT_GPIOD31_MASK		(1<<ARM_PINMUX_RTC_CLKOUT_GPIOD31_SHIFT)
#define ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_MASK	(1<<ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_SHIFT)
#define ARM_PINMUX_CLKOUT_GPIOD30_MASK			(1<<ARM_PINMUX_CLKOUT_GPIOD30_SHIFT)

#define ARM_PINMUX_GPIOD_PIN0_VAL	(1<<ARM_PINMUX_TIMER_W_OUT_GPIOD0_SHIFT)
#define ARM_PINMUX_GPIOD_PIN1_VAL	(1<<ARM_PINMUX_TIMER_X_OUT_GPIOD1_SHIFT)
#define ARM_PINMUX_GPIOD_PIN2_VAL	(1<<ARM_PINMUX_TIMER_Y_OUT_GPIOD2_SHIFT)
#define ARM_PINMUX_GPIOD_PIN3_VAL	(1<<ARM_PINMUX_TIMER_Z_OUT_GPIOD3_SHIFT)
#define ARM_PINMUX_GPIOD_PIN31_VAL	(1<<ARM_PINMUX_RTC_CLKOUT_GPIOD31_SHIFT)
#define ARM_PINMUX_GPIOD_PIN29_VAL	(1<<ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_SHIFT)
#define ARM_PINMUX_GPIOD_PIN30_VAL	(1<<ARM_PINMUX_CLKOUT_GPIOD30_SHIFT)

// timer w/x/y/z output mask and value
#define ARM_PINMUX_TIMERW_OUT_VAL	(0<<ARM_PINMUX_TIMER_W_OUT_GPIOD0_SHIFT)
#define ARM_PINMUX_TIMERX_OUT_VAL	(0<<ARM_PINMUX_TIMER_X_OUT_GPIOD1_SHIFT)
#define ARM_PINMUX_TIMERY_OUT_VAL	(0<<ARM_PINMUX_TIMER_Y_OUT_GPIOD2_SHIFT)
#define ARM_PINMUX_TIMERZ_OUT_VAL	(0<<ARM_PINMUX_TIMER_Z_OUT_GPIOD3_SHIFT)

// rtc clkout mask and value
#define ARM_PINMUX_RTC_CLKOUT_VAL	(0<<ARM_PINMUX_RTC_CLKOUT_GPIOD31_SHIFT)

// rtc cal clkout mask and value
#define ARM_PINMUX_RTC_CAL_CLKOUT_VAL	(0<<ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_SHIFT)

// arm clkout mask and value
#define ARM_PINMUX_CLKOUT_VAL	(0<<ARM_PINMUX_CLKOUT_GPIOD30_SHIFT)

// host low power out mask and value
#define ARM_PINMUX_HOST_LOW_POWER_OUT_MASK	(1<<ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT)
#define ARM_PINMUX_HOST_LOW_POWER_OUT_VAL	(0<<ARM_PINMUX_HOST_LOW_POWER_OUT_GPIOA29_31_SHIFT)

// epf sleep mode done mask and value
#define ARM_PINMUX_ARM_EPF_SLEEP_MODE_DONE_MASK	(1<<ARM_PINMUX_EPF_SLEEP_MODE_DONE_GPIOA29_31_SHIFT)
#define ARM_PINMUX_ARM_EPF_SLEEP_MODE_DONE_VAL	(0<<ARM_PINMUX_EPF_SLEEP_MODE_DONE_GPIOA29_31_SHIFT)

// epf fail time out mask and value
#define ARM_PINMUX_EPF_FAIL_TIME_OUT_MASK		(1<<ARM_PINMUX_EPF_FAIL_TIMER_OUT_GPIOB30_SHIFT)
#define ARM_PINMUX_EPF_FAIL_TIMER_OUT_VAL		(0<<ARM_PINMUX_EPF_FAIL_TIMER_OUT_GPIOB30_SHIFT)

// ARM IO Pull Up/Pull Down Configuration
typedef enum
{
	
	IO_PU_DOWN 	= 0, // pull down											
	IO_PU_UP 	= 1	 // pull up
}E_IO_PU;

// offset: 0x00C0, SD_PU register
typedef enum
{
	ARM_SDIO0_CLK_GPIOC0_PU_SHIFT		= 0,	// Bit[0]: SDIO0_CLK Pin(GPIOC[0]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[0]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[0]=0)

	ARM_SDIO0_CMD_GPIOC1_PU_SHIFT		= 1,	// Bit[1]: SDIO0_CMD Pin(GPIOC[1]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[1]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[1]=0)

	ARM_SDIO0_DATA0_GPIOC2_PU_SHIFT		= 2,	// Bit[2]: SDIO0_DATA0 Pin(GPIOC[2]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[2]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[2]=0)

	ARM_SDIO0_DATA1_GPIOC3_PU_SHIFT		= 3,	// Bit[3]: SDIO0_DATA1 Pin(GPIOC[3]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[3]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[3]=0)

	ARM_SDIO0_DATA2_GPIOC4_PU_SHIFT		= 4,	// Bit[4]: SDIO0_DATA2 Pin(GPIOC[4]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[4]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[4]=0)

	ARM_SDIO0_DATA3_GPIOC5_PU_SHIFT		= 5,	// Bit[5]: SDIO0_DATA3 Pin(GPIOC[5]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[5]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[5]=0)

	ARM_SDIO0_DATA4_GPIOC6_PU_SHIFT		= 6,	// Bit[6]: SDIO0_DATA4 Pin(GPIOC[6]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[6]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[6]=0)

	ARM_SDIO0_DATA5_GPIOC7_PU_SHIFT		= 7,	// Bit[7]: SDIO0_DATA5 Pin(GPIOC[7]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[7]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[7]=0)

	ARM_SDIO0_DATA6_GPIOC8_PU_SHIFT		= 8,	// Bit[8]: SDIO0_DATA6 Pin(GPIOC[8]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[8]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[8]=0)

	ARM_SDIO0_DATA7_GPIOC9_PU_SHIFT		= 9,	// Bit[9]: SDIO0_DATA7 Pin(GPIOC[9]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[9]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[9]=0)

	ARM_SDIO0_CD_GPIOC25_PU_SHIFT		= 10,	// Bit[10]: SDIO0_CD Pin(GPIOC[25]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[10]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[10]=0)

	ARM_SDIO0_WP_GPIOC26_PU_SHIFT		= 11,	// Bit[11]: SDIO0_WP Pin(GPIOC[26]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[11]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[11]=0)

	ARM_SDIO1_CLK_GPIOC10_PU_SHIFT		= 12,	// Bit[12]: SDIO1_CLK Pin(GPIOC[10]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[12]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[12]=0)

	ARM_SDIO1_CMD_GPIOC11_PU_SHIFT		= 13,	// Bit[13]: SDIO1_CMD Pin(GPIOC[11]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[13]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[13]=0)

	ARM_SDIO1_DATA0_GPIOC12_PU_SHIFT	= 14,	// Bit[14]: SDIO1_DATA0 Pin(GPIOC[12]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[14]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[14]=0)

	ARM_SDIO1_DATA1_GPIOC13_PU_SHIFT	= 15,	// Bit[15]: SDIO1_DATA1 Pin(GPIOC[13]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[15]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[15]=0)

	ARM_SDIO1_DATA2_GPIOC14_PU_SHIFT	= 16,	// Bit[16]: SDIO1_DATA2 Pin(GPIOC[14]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[16]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[16]=0)

	ARM_SDIO1_DATA3_GPIOC15_PU_SHIFT	= 17,	// Bit[17]: SDIO1_DATA3 Pin(GPIOC[15]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[17]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[17]=0)

	ARM_SDIO1_DATA4_GPIOC16_TIMER0_IN_PU_SHIFT	= 18,	// Bit[18]: SDIO1_DATA4 Pin(GPIOC[16], timer0 in) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(SD_PD[18]=0)
														// 1: pull up is selected when pull logic is active(SD_PD[18]=0)

	ARM_SDIO1_DATA5_GPIOC17_TIMER1_IN_PU_SHIFT	= 19,	// Bit[19]: SDIO1_DATA5 Pin(GPIOC[17], timer1 in) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(SD_PD[19]=0)
														// 1: pull up is selected when pull logic is active(SD_PD[19]=0)

	ARM_SDIO1_DATA6_GPIOC18_TIMER2_IN_PU_SHIFT	= 20,	// Bit[20]: SDIO1_DATA6 Pin(GPIOC[18], timer2 in) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(SD_PD[20]=0)
														// 1: pull up is selected when pull logic is active(SD_PD[20]=0)

	ARM_SDIO1_DATA7_GPIOC19_TIMER3_IN_PU_SHIFT	= 21,	// Bit[21]: SDIO1_DATA7 Pin(GPIOC[19], timer3 in) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(SD_PD[21]=0)
														// 1: pull up is selected when pull logic is active(SD_PD[21]=0)

	ARM_SDIO1_CD_GPIOC27_PU_SHIFT		= 22,	// Bit[22]: SDIO1_CD Pin(GPIOC[27]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[22]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[22]=0)

	ARM_SDIO1_WP_GPIOC28_PU_SHIFT		= 23,	// Bit[23]: SDIO1_WP Pin(GPIOC[28]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[23]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[23]=0)

	ARM_SDIO0_RSTN_GPIOC29_PU_SHIFT		= 24,	// Bit[24]: SDIO0_RSTN Pin(GPIOC[29]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[24]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[24]=0)

	ARM_SDIO1_RSTN_GPIOC30_PU_SHIFT		= 25,	// Bit[25]: SDIO1_RSTN Pin(GPIOC[30]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(SD_PD[25]=0)
												// 1: pull up is selected when pull logic is active(SD_PD[25]=0)

	ARM_SD_REVD_PU_SHIFT				= 26	// Bit[31:26]: reserved.
}E_ARM_SD_PU_SHIFT;

// sdio0/1 pull up/down mask and value
#define ARM_SDIO0_PU_MASK	((0xFFF<<ARM_SDIO0_CLK_GPIOC0_PU_SHIFT)  | (1<<ARM_SDIO0_RSTN_GPIOC29_PU_SHIFT))
#define ARM_SDIO1_PU_MASK	((0xFFF<<ARM_SDIO1_CLK_GPIOC10_PU_SHIFT) | (1<<ARM_SDIO1_RSTN_GPIOC30_PU_SHIFT))

#define ARM_SDIO0_PU_VAL	((0xFFF<<ARM_SDIO0_CLK_GPIOC0_PU_SHIFT)  | (1<<ARM_SDIO0_RSTN_GPIOC29_PU_SHIFT))
#define ARM_SDIO1_PU_VAL	((0xFFF<<ARM_SDIO1_CLK_GPIOC10_PU_SHIFT) | (1<<ARM_SDIO1_RSTN_GPIOC30_PU_SHIFT))

// SDIO0 control pin mask and value
#define ARM_SDIO0_CTRL_PU_MASK	((1<<ARM_SDIO0_CLK_GPIOC0_PU_SHIFT) |\
                                 (1<<ARM_SDIO0_CMD_GPIOC1_PU_SHIFT) |\
                                 (1<<ARM_SDIO0_CD_GPIOC25_PU_SHIFT) |\
                                 (1<<ARM_SDIO0_WP_GPIOC26_PU_SHIFT) |\
                                 (1<<ARM_SDIO0_RSTN_GPIOC29_PU_SHIFT))

// SDIO0 pin mask and value
#define ARM_SDIO0_1BIT_PU_MASK	(ARM_SDIO0_CTRL_PU_MASK |\
                                (1<<ARM_SDIO0_DATA0_GPIOC2_PU_SHIFT))

#define ARM_SDIO0_4BIT_PU_MASK	(ARM_SDIO0_CTRL_PU_MASK |\
                                (0xF<<ARM_SDIO0_DATA0_GPIOC2_PU_SHIFT))

#define ARM_SDIO0_8BIT_PU_MASK	(ARM_SDIO0_CTRL_PU_MASK |\
                                (0xFF<<ARM_SDIO0_DATA0_GPIOC2_PU_SHIFT))

#define ARM_SDIO0_1BIT_PU_VAL	ARM_SDIO0_1BIT_PU_MASK
#define ARM_SDIO0_4BIT_PU_VAL	ARM_SDIO0_4BIT_PU_MASK
#define ARM_SDIO0_8BIT_PU_VAL	ARM_SDIO0_8BIT_PU_MASK

// SDIO1 control pin mask and value
#define ARM_SDIO1_CTRL_PU_MASK	((1<<ARM_SDIO1_CLK_GPIOC10_PU_SHIFT) |\
                                 (1<<ARM_SDIO1_CMD_GPIOC11_PU_SHIFT) |\
                                 (1<<ARM_SDIO1_CD_GPIOC27_PU_SHIFT)  |\
                                 (1<<ARM_SDIO1_WP_GPIOC28_PU_SHIFT)  |\
                                 (1<<ARM_SDIO1_RSTN_GPIOC30_PU_SHIFT))

// SDIO1 pin mask and value
#define ARM_SDIO1_1BIT_PU_MASK	(ARM_SDIO1_CTRL_PU_MASK |\
                                (1<<ARM_SDIO1_DATA0_GPIOC12_PU_SHIFT))

#define ARM_SDIO1_4BIT_PU_MASK	(ARM_SDIO1_CTRL_PU_MASK |\
                                (0xF<<ARM_SDIO1_DATA0_GPIOC12_PU_SHIFT))

#define ARM_SDIO1_8BIT_PU_MASK	(ARM_SDIO1_CTRL_PU_MASK |\
                                (0xFF<<ARM_SDIO1_DATA0_GPIOC12_PU_SHIFT))

#define ARM_SDIO1_1BIT_PU_VAL	ARM_SDIO1_1BIT_PU_MASK
#define ARM_SDIO1_4BIT_PU_VAL	ARM_SDIO1_4BIT_PU_MASK
#define ARM_SDIO1_8BIT_PU_VAL	ARM_SDIO1_8BIT_PU_MASK


// timer0/1/2/3 pull up/down mask and value
#define ARM_TIMER0_IN_PU_MASK			(0x1<<ARM_SDIO1_DATA4_GPIOC16_TIMER0_IN_PU_SHIFT)
#define ARM_TIMER1_IN_PU_MASK			(0x1<<ARM_SDIO1_DATA5_GPIOC17_TIMER1_IN_PU_SHIFT)
#define ARM_TIMER2_IN_PU_MASK			(0x1<<ARM_SDIO1_DATA6_GPIOC18_TIMER2_IN_PU_SHIFT)
#define ARM_TIMER3_IN_PU_MASK			(0x1<<ARM_SDIO1_DATA7_GPIOC19_TIMER3_IN_PU_SHIFT)

#define ARM_TIMER0_IN_PU_VAL(val)		((val)<<ARM_SDIO1_DATA4_GPIOC16_TIMER0_IN_PU_SHIFT)
#define ARM_TIMER1_IN_PU_VAL(val)		((val)<<ARM_SDIO1_DATA5_GPIOC17_TIMER1_IN_PU_SHIFT)
#define ARM_TIMER2_IN_PU_VAL(val)		((val)<<ARM_SDIO1_DATA6_GPIOC18_TIMER2_IN_PU_SHIFT)
#define ARM_TIMER3_IN_PU_VAL(val)		((val)<<ARM_SDIO1_DATA7_GPIOC19_TIMER3_IN_PU_SHIFT)

// gpioc pull up/down mask and value
#define ARM_GPIOC0_PU_MASK			(0x1<<ARM_SDIO0_CLK_GPIOC0_PU_SHIFT)
#define ARM_GPIOC1_PU_MASK			(0x1<<ARM_SDIO0_CMD_GPIOC1_PU_SHIFT)
#define ARM_GPIOC2_PU_MASK			(0x1<<ARM_SDIO0_DATA0_GPIOC2_PU_SHIFT)
#define ARM_GPIOC3_PU_MASK			(0x1<<ARM_SDIO0_DATA1_GPIOC3_PU_SHIFT)
#define ARM_GPIOC4_PU_MASK			(0x1<<ARM_SDIO0_DATA2_GPIOC4_PU_SHIFT)
#define ARM_GPIOC5_PU_MASK			(0x1<<ARM_SDIO0_DATA3_GPIOC5_PU_SHIFT)
#define ARM_GPIOC6_PU_MASK			(0x1<<ARM_SDIO0_DATA4_GPIOC6_PU_SHIFT)
#define ARM_GPIOC7_PU_MASK			(0x1<<ARM_SDIO0_DATA5_GPIOC7_PU_SHIFT)
#define ARM_GPIOC8_PU_MASK			(0x1<<ARM_SDIO0_DATA6_GPIOC8_PU_SHIFT)
#define ARM_GPIOC9_PU_MASK			(0x1<<ARM_SDIO0_DATA7_GPIOC9_PU_SHIFT)

#define ARM_GPIOC25_PU_MASK			(0x1<<ARM_SDIO0_CD_GPIOC25_PU_SHIFT)
#define ARM_GPIOC26_PU_MASK			(0x1<<ARM_SDIO0_WP_GPIOC26_PU_SHIFT)

#define ARM_GPIOC10_PU_MASK			(0x1<<ARM_SDIO1_CLK_GPIOC10_PU_SHIFT)
#define ARM_GPIOC11_PU_MASK			(0x1<<ARM_SDIO1_CMD_GPIOC11_PU_SHIFT)
#define ARM_GPIOC12_PU_MASK			(0x1<<ARM_SDIO1_DATA0_GPIOC12_PU_SHIFT)
#define ARM_GPIOC13_PU_MASK			(0x1<<ARM_SDIO1_DATA1_GPIOC13_PU_SHIFT)
#define ARM_GPIOC14_PU_MASK			(0x1<<ARM_SDIO1_DATA2_GPIOC14_PU_SHIFT)
#define ARM_GPIOC15_PU_MASK			(0x1<<ARM_SDIO1_DATA3_GPIOC15_PU_SHIFT)
#define ARM_GPIOC16_PU_MASK			(0x1<<ARM_SDIO1_DATA4_GPIOC16_TIMER0_IN_PU_SHIFT)
#define ARM_GPIOC17_PU_MASK			(0x1<<ARM_SDIO1_DATA5_GPIOC17_TIMER1_IN_PU_SHIFT)
#define ARM_GPIOC18_PU_MASK			(0x1<<ARM_SDIO1_DATA6_GPIOC18_TIMER2_IN_PU_SHIFT)
#define ARM_GPIOC19_PU_MASK			(0x1<<ARM_SDIO1_DATA7_GPIOC19_TIMER3_IN_PU_SHIFT)

#define ARM_GPIOC27_PU_MASK			(0x1<<ARM_SDIO1_CD_GPIOC27_PU_SHIFT)
#define ARM_GPIOC28_PU_MASK			(0x1<<ARM_SDIO1_WP_GPIOC28_PU_SHIFT)
#define ARM_GPIOC29_PU_MASK			(0x1<<ARM_SDIO0_RSTN_GPIOC29_PU_SHIFT)
#define ARM_GPIOC30_PU_MASK			(0x1<<ARM_SDIO1_RSTN_GPIOC30_PU_SHIFT)


#define ARM_GPIOC0_PU_VAL(val)			((val)<<ARM_SDIO0_CLK_GPIOC0_PU_SHIFT)
#define ARM_GPIOC1_PU_VAL(val)			((val)<<ARM_SDIO0_CMD_GPIOC1_PU_SHIFT)
#define ARM_GPIOC2_PU_VAL(val)			((val)<<ARM_SDIO0_DATA0_GPIOC2_PU_SHIFT)
#define ARM_GPIOC3_PU_VAL(val)			((val)<<ARM_SDIO0_DATA1_GPIOC3_PU_SHIFT)
#define ARM_GPIOC4_PU_VAL(val)			((val)<<ARM_SDIO0_DATA2_GPIOC4_PU_SHIFT)
#define ARM_GPIOC5_PU_VAL(val)			((val)<<ARM_SDIO0_DATA3_GPIOC5_PU_SHIFT)
#define ARM_GPIOC6_PU_VAL(val)			((val)<<ARM_SDIO0_DATA4_GPIOC6_PU_SHIFT)
#define ARM_GPIOC7_PU_VAL(val)			((val)<<ARM_SDIO0_DATA5_GPIOC7_PU_SHIFT)
#define ARM_GPIOC8_PU_VAL(val)			((val)<<ARM_SDIO0_DATA6_GPIOC8_PU_SHIFT)
#define ARM_GPIOC9_PU_VAL(val)			((val)<<ARM_SDIO0_DATA7_GPIOC9_PU_SHIFT)

#define ARM_GPIOC25_PU_VAL(val)			((val)<<ARM_SDIO0_CD_GPIOC25_PU_SHIFT)
#define ARM_GPIOC26_PU_VAL(val)			((val)<<ARM_SDIO0_WP_GPIOC26_PU_SHIFT)

#define ARM_GPIOC10_PU_VAL(val)			((val)<<ARM_SDIO1_CLK_GPIOC10_PU_SHIFT)
#define ARM_GPIOC11_PU_VAL(val)			((val)<<ARM_SDIO1_CMD_GPIOC11_PU_SHIFT)
#define ARM_GPIOC12_PU_VAL(val)			((val)<<ARM_SDIO1_DATA0_GPIOC12_PU_SHIFT)
#define ARM_GPIOC13_PU_VAL(val)			((val)<<ARM_SDIO1_DATA1_GPIOC13_PU_SHIFT)
#define ARM_GPIOC14_PU_VAL(val)			((val)<<ARM_SDIO1_DATA2_GPIOC14_PU_SHIFT)
#define ARM_GPIOC15_PU_VAL(val)			((val)<<ARM_SDIO1_DATA3_GPIOC15_PU_SHIFT)
#define ARM_GPIOC16_PU_VAL(val)			((val)<<ARM_SDIO1_DATA4_GPIOC16_TIMER0_IN_PU_SHIFT)
#define ARM_GPIOC17_PU_VAL(val)			((val)<<ARM_SDIO1_DATA5_GPIOC17_TIMER1_IN_PU_SHIFT)
#define ARM_GPIOC18_PU_VAL(val)			((val)<<ARM_SDIO1_DATA6_GPIOC18_TIMER2_IN_PU_SHIFT)
#define ARM_GPIOC19_PU_VAL(val)			((val)<<ARM_SDIO1_DATA7_GPIOC19_TIMER3_IN_PU_SHIFT)

#define ARM_GPIOC27_PU_VAL(val)			((val)<<ARM_SDIO1_CD_GPIOC27_PU_SHIFT)
#define ARM_GPIOC28_PU_VAL(val)			((val)<<ARM_SDIO1_WP_GPIOC28_PU_SHIFT)
#define ARM_GPIOC29_PU_VAL(val)			((val)<<ARM_SDIO0_RSTN_GPIOC29_PU_SHIFT)
#define ARM_GPIOC30_PU_VAL(val)			((val)<<ARM_SDIO1_RSTN_GPIOC30_PU_SHIFT)

// offset: 0x00C4, ARM_UART_PU register
typedef enum
{
	ARM_UART0_TXD_GPIOA0_PU_SHIFT		= 0,	// Bit[0]: UART0_TXD Pin(GPIOA[0]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[0]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[0]=0)

	ARM_UART0_RXD_GPIOA1_PU_SHIFT		= 1,	// Bit[1]: UART0_RXD Pin(GPIOA[1]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[1]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[1]=0)

	ARM_UART0_CTSN_GPIOA2_PU_SHIFT		= 2,	// Bit[2]: UART0_CTSN Pin(GPIOA[2]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[2]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[2]=0)

	ARM_UART0_RTSN_GPIOA3_PU_SHIFT		= 3,	// Bit[3]: UART0_RTSN Pin(GPIOA[3]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[3]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[3]=0)

	ARM_UART1_TXD_GPIOA4_PU_SHIFT		= 4,	// Bit[4]: UART1_TXD Pin(GPIOA[4]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[4]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[4]=0)

	ARM_UART1_RXD_GPIOA5_PU_SHIFT		= 5,	// Bit[5]: UART1_RXD Pin(GPIOA[5]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[5]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[5]=0)

	ARM_UART1_CTSN_GPIOA6_PU_SHIFT		= 6,	// Bit[6]: UART1_CTSN Pin(GPIOA[6]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[6]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[6]=0)

	ARM_UART1_RTSN_GPIOA7_PU_SHIFT		= 7,	// Bit[7]: UART1_RTSN Pin(GPIOA[7]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[7]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[7]=0)

	ARM_UART2_TXD_GPIOA8_PU_SHIFT		= 8,	// Bit[8]: UART2_TXD Pin(GPIOA[8]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[8]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[8]=0)

	ARM_UART2_RXD_GPIOA9_PU_SHIFT		= 9,	// Bit[9]: UART2_RXD Pin(GPIOA[9]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[9]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[9]=0)

	ARM_UART2_CTSN_GPIOA10_PU_SHIFT		= 10,	// Bit[10]: UART2_CTSN Pin(GPIOA[10]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[10]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[10]=0)

	ARM_UART2_RTSN_GPIOA11_PU_SHIFT		= 11,	// Bit[11]: UART2_RTSN Pin(GPIOA[11]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[11]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[11]=0)

	ARM_UART3_TXD_GPIOA12_PU_SHIFT		= 12,	// Bit[12]: UART3_TXD Pin(GPIOA[12]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[12]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[12]=0)

	ARM_UART3_RXD_GPIOA13_PU_SHIFT		= 13,	// Bit[13]: UART3_RXD Pin(GPIOA[13]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[13]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[13]=0)

	ARM_UART3_CTSN_GPIOA14_PU_SHIFT		= 14,	// Bit[14]: UART3_CTSN Pin(GPIOA[14]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[14]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[14]=0)

	ARM_UART3_RTSN_GPIOA15_PU_SHIFT		= 15,	// Bit[15]: UART3_RTSN Pin(GPIOA[15]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[15]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[15]=0)

	ARM_UART3_DCDN_GPIOA16_PU_SHIFT		= 16,	// Bit[16]: UART3_DCDN Pin(GPIOA[16]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[16]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[16]=0)

	ARM_UART3_DSR_GPIOA17_PU_SHIFT		= 17,	// Bit[17]: UART3_DSR Pin(GPIOA[17]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[17]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[17]=0)

	ARM_UART3_DTR_GPIOA18_PU_SHIFT		= 18,	// Bit[18]: UART3_DTR Pin(GPIOA[18]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[18]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[18]=0)

	ARM_UART3_RIN_GPIOA19_PU_SHIFT		= 19,	// Bit[19]: UART3_RIN Pin(GPIOA[19]) pull up/pull down select signal. Default: 1
												// 0: pull down is selected when pull logic is active(ARM_UART_PD[19]=0)
												// 1: pull up is selected when pull logic is active(ARM_UART_PD[19]=0)
		
	ARM_UART_REVD_PU_SHIFT				= 20	// Bit[31:20]: Reserved.

}E_ARM_UART_PU_SHIFT;

// uart0/1/2/3 pull up/down mask and value
#define ARM_UART0_PU_MASK			(0x0F<<ARM_UART0_TXD_GPIOA0_PU_SHIFT)
#define ARM_UART1_PU_MASK			(0x0F<<ARM_UART1_TXD_GPIOA4_PU_SHIFT)
#define ARM_UART2_PU_MASK			(0x0F<<ARM_UART2_TXD_GPIOA8_PU_SHIFT)
#define ARM_UART3_PU_MASK			(0xFF<<ARM_UART3_TXD_GPIOA12_PU_SHIFT)

#define ARM_UART0_PU_VAL			(0x0F<<ARM_UART0_TXD_GPIOA0_PU_SHIFT)
#define ARM_UART1_PU_VAL			(0x0F<<ARM_UART1_TXD_GPIOA4_PU_SHIFT)
#define ARM_UART2_PU_VAL			(0x0F<<ARM_UART2_TXD_GPIOA8_PU_SHIFT)
#define ARM_UART3_PU_VAL			(0xFF<<ARM_UART3_TXD_GPIOA12_PU_SHIFT)

// gpioa pull up/down mask and value
#define ARM_GPIOA0_PU_MASK			(0x1<<ARM_UART0_TXD_GPIOA0_PU_SHIFT)
#define ARM_GPIOA1_PU_MASK			(0x1<<ARM_UART0_RXD_GPIOA1_PU_SHIFT)
#define ARM_GPIOA2_PU_MASK			(0x1<<ARM_UART0_CTSN_GPIOA2_PU_SHIFT)
#define ARM_GPIOA3_PU_MASK			(0x1<<ARM_UART0_RTSN_GPIOA3_PU_SHIFT)
#define ARM_GPIOA4_PU_MASK			(0x1<<ARM_UART1_TXD_GPIOA4_PU_SHIFT)
#define ARM_GPIOA5_PU_MASK			(0x1<<ARM_UART1_RXD_GPIOA5_PU_SHIFT)
#define ARM_GPIOA6_PU_MASK			(0x1<<ARM_UART1_CTSN_GPIOA6_PU_SHIFT)
#define ARM_GPIOA7_PU_MASK			(0x1<<ARM_UART1_RTSN_GPIOA7_PU_SHIFT)
#define ARM_GPIOA8_PU_MASK			(0x1<<ARM_UART2_TXD_GPIOA8_PU_SHIFT)
#define ARM_GPIOA9_PU_MASK			(0x1<<ARM_UART2_RXD_GPIOA9_PU_SHIFT)
#define ARM_GPIOA10_PU_MASK			(0x1<<ARM_UART2_CTSN_GPIOA10_PU_SHIFT)
#define ARM_GPIOA11_PU_MASK			(0x1<<ARM_UART2_RTSN_GPIOA11_PU_SHIFT)
#define ARM_GPIOA12_PU_MASK			(0x1<<ARM_UART3_TXD_GPIOA12_PU_SHIFT)
#define ARM_GPIOA13_PU_MASK			(0x1<<ARM_UART3_RXD_GPIOA13_PU_SHIFT)
#define ARM_GPIOA14_PU_MASK			(0x1<<ARM_UART3_CTSN_GPIOA14_PU_SHIFT)
#define ARM_GPIOA15_PU_MASK			(0x1<<ARM_UART3_RTSN_GPIOA15_PU_SHIFT)
#define ARM_GPIOA16_PU_MASK			(0x1<<ARM_UART3_DCDN_GPIOA16_PU_SHIFT)
#define ARM_GPIOA17_PU_MASK			(0x1<<ARM_UART3_DSR_GPIOA17_PU_SHIFT)
#define ARM_GPIOA18_PU_MASK			(0x1<<ARM_UART3_DTR_GPIOA18_PU_SHIFT)
#define ARM_GPIOA19_PU_MASK			(0x1<<ARM_UART3_RIN_GPIOA19_PU_SHIFT)

#define ARM_GPIOA0_PU_VAL(val)		((val)<<ARM_UART0_TXD_GPIOA0_PU_SHIFT)
#define ARM_GPIOA1_PU_VAL(val)		((val)<<ARM_UART0_RXD_GPIOA1_PU_SHIFT)
#define ARM_GPIOA2_PU_VAL(val)		((val)<<ARM_UART0_CTSN_GPIOA2_PU_SHIFT)
#define ARM_GPIOA3_PU_VAL(val)		((val)<<ARM_UART0_RTSN_GPIOA3_PU_SHIFT)
#define ARM_GPIOA4_PU_VAL(val)		((val)<<ARM_UART1_TXD_GPIOA4_PU_SHIFT)
#define ARM_GPIOA5_PU_VAL(val)		((val)<<ARM_UART1_RXD_GPIOA5_PU_SHIFT)
#define ARM_GPIOA6_PU_VAL(val)		((val)<<ARM_UART1_CTSN_GPIOA6_PU_SHIFT)
#define ARM_GPIOA7_PU_VAL(val)		((val)<<ARM_UART1_RTSN_GPIOA7_PU_SHIFT)
#define ARM_GPIOA8_PU_VAL(val)		((val)<<ARM_UART2_TXD_GPIOA8_PU_SHIFT)
#define ARM_GPIOA9_PU_VAL(val)		((val)<<ARM_UART2_RXD_GPIOA9_PU_SHIFT)
#define ARM_GPIOA10_PU_VAL(val)		((val)<<ARM_UART2_CTSN_GPIOA10_PU_SHIFT)
#define ARM_GPIOA11_PU_VAL(val)		((val)<<ARM_UART2_RTSN_GPIOA11_PU_SHIFT)
#define ARM_GPIOA12_PU_VAL(val)		((val)<<ARM_UART3_TXD_GPIOA12_PU_SHIFT)
#define ARM_GPIOA13_PU_VAL(val)		((val)<<ARM_UART3_RXD_GPIOA13_PU_SHIFT)
#define ARM_GPIOA14_PU_VAL(val)		((val)<<ARM_UART3_CTSN_GPIOA14_PU_SHIFT)
#define ARM_GPIOA15_PU_VAL(val)		((val)<<ARM_UART3_RTSN_GPIOA15_PU_SHIFT)
#define ARM_GPIOA16_PU_VAL(val)		((val)<<ARM_UART3_DCDN_GPIOA16_PU_SHIFT)
#define ARM_GPIOA17_PU_VAL(val)		((val)<<ARM_UART3_DSR_GPIOA17_PU_SHIFT)
#define ARM_GPIOA18_PU_VAL(val)		((val)<<ARM_UART3_DTR_GPIOA18_PU_SHIFT)
#define ARM_GPIOA19_PU_VAL(val)		((val)<<ARM_UART3_RIN_GPIOA19_PU_SHIFT)

// offset: 0x00C8, ARM_SPI_I2C_PU register
typedef enum
{
	ARM_SPI0_CLK_GPIOB0_PU_SHIFT	= 0,	// Bit[0]: SPI0_CLK Pin(GPIOB[0]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[0]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[0]=0)

	ARM_SPI0_CSN0_GPIOB1_PU_SHIFT	= 1,	// Bit[1]: SPI0_CSN0 Pin(GPIOB[1]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[1]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[1]=0)

	ARM_SPI0_CSN1_GPIOB2_PU_SHIFT	= 2,	// Bit[2]: SPI0_CSN1 Pin(GPIOB[2]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[2]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[2]=0)

	ARM_SPI0_DIO0_GPIOB3_PU_SHIFT	= 3,	// Bit[3]: SPI0_DIO0 Pin(GPIOB[3]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[3]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[3]=0)

	ARM_SPI0_DIO1_GPIOB4_PU_SHIFT	= 4,	// Bit[4]: SPI0_DIO1 Pin(GPIOB[4]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[4]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[4]=0)

	ARM_SPI0_DIO2_GPIOB5_PU_SHIFT	= 5,	// Bit[5]: SPI0_DIO2 Pin(GPIOB[5]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[5]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[5]=0)

	ARM_SPI0_DIO3_GPIOB6_PU_SHIFT	= 6,	// Bit[6]: SPI0_DIO3 Pin(GPIOB[6]) pull up /pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[6]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[6]=0)

	ARM_SPI1_CLK_GPIOB7_PU_SHIFT	= 7,	// Bit[7]: SPI1_CLK Pin(GPIOB[7]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[7]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[7]=0)

	ARM_SPI1_CSN0_GPIOB8_PU_SHIFT	= 8,	// Bit[8]: SPI1_CSN0 Pin(GPIOB[8]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[8]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[8]=0)

	ARM_SPI1_CSN1_GPIOB9_PU_SHIFT	= 9,	// Bit[9]: SPI1_CSN1 Pin(GPIOB[9]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[9]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[9]=0)

	ARM_SPI1_DIO0_GPIOB10_PU_SHIFT	= 10,	// Bit[10]: SPI1_DIO0 Pin(GPIOB[10]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[10]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[10]=0)

	ARM_SPI1_DIO1_GPIOB11_PU_SHIFT	= 11,	// Bit[11]: SPI1_DIO1 Pin(GPIOB[11]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[11]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[11]=0)

	ARM_SPI2_CLK_GPIOB12_PU_SHIFT	= 12,	// Bit[12]: SPI2_CLK Pin(GPIOB[12]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[12]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[12]=0)

	ARM_SPI2_CSN0_GPIOB13_PU_SHIFT	= 13,	// Bit[13]: SPI2_CSN0 Pin(GPIOB[13]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[13]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[13]=0)

	ARM_SPI2_CSN1_GPIOB14_PU_SHIFT	= 14,	// Bit[14]: SPI2_CSN1 Pin(GPIOB[14]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[14]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[14]=0)

	ARM_SPI2_DIO0_GPIOB15_PU_SHIFT	= 15,	// Bit[15]: SPI2_DIO0 Pin(GPIOB[15]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[15]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[15]=0)

	ARM_SPI2_DIO1_GPIOB16_PU_SHIFT	= 16,	// Bit[16]: SPI2_DIO1 Pin(GPIOB[16]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[16]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[16]=0)

	ARM_SPI_I2C_REVD0_PU_SHIFT		= 17,	// Bit[19:17]: reserved.

	ARM_I2C0_SDA_GPIOA20_PU_SHIFT	= 20,	// Bit[20]: I2C0_SDA Pin(GPIOA[20]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[20]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[20]=0)

	ARM_I2C0_SCL_GPIOA21_PU_SHIFT	= 21,	// Bit[21]: I2C0_SCL Pin(GPIOA[21]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[21]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[21]=0)

	ARM_I2C1_SDA_GPIOA22_PU_SHIFT	= 22,	// Bit[22]: I2C1_SDA Pin(GPIOA[22]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[22]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[22]=0)

	ARM_I2C1_SCL_GPIOA23_PU_SHIFT	= 23,	// Bit[23]: I2C1_SCL Pin(GPIOA[23]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[23]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[23]=0)
	
	ARM_I2C2_SDA_GPIOA24_PU_SHIFT	= 24,	// Bit[24]: I2C2_SDA Pin(GPIOA[24]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[24]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[24]=0)

	ARM_I2C2_SCL_GPIOA25_PU_SHIFT	= 25,	// Bit[25]: I2C2_SCL Pin(GPIOA[25]) pull up/pull down select signal. Default: 1
											// 0: pull down is selected when pull logic is active(ARM_SPI_I2C_PD[25]=0)
											// 1: pull up is selected when pull logic is active(ARM_SPI_I2C_PD[25]=0)
	
	ARM_SPI_I2C_REVD1_PU_SHIFT		= 26	// // Bit[31:26]: Reserved
}E_ARM_SPI_I2C_PU_SHIFT;

// spi0/1/2 pull up/down mask and value
#define ARM_SPI0_PU_MASK		(0x7F<<ARM_SPI0_CLK_GPIOB0_PU_SHIFT)
#define ARM_SPI1_PU_MASK		(0x1F<<ARM_SPI1_CLK_GPIOB7_PU_SHIFT)
#define ARM_SPI2_PU_MASK		(0x1F<<ARM_SPI2_CLK_GPIOB12_PU_SHIFT)

#define ARM_SPI0_PU_VAL			(0x7F<<ARM_SPI0_CLK_GPIOB0_PU_SHIFT)
#define ARM_SPI1_PU_VAL			(0x1F<<ARM_SPI1_CLK_GPIOB7_PU_SHIFT)
#define ARM_SPI2_PU_VAL			(0x1F<<ARM_SPI2_CLK_GPIOB12_PU_SHIFT)

// i2c0/1/2 pull up/down mask and value
#define ARM_I2C0_PU_MASK		(0x3<<ARM_I2C0_SDA_GPIOA20_PU_SHIFT)
#define ARM_I2C1_PU_MASK		(0x3<<ARM_I2C1_SDA_GPIOA22_PU_SHIFT)
#define ARM_I2C2_PU_MASK		(0x3<<ARM_I2C2_SDA_GPIOA24_PU_SHIFT)

#define ARM_I2C0_PU_VAL			(0x3<<ARM_I2C0_SDA_GPIOA20_PU_SHIFT)
#define ARM_I2C1_PU_VAL			(0x3<<ARM_I2C1_SDA_GPIOA22_PU_SHIFT)
#define ARM_I2C2_PU_VAL			(0x3<<ARM_I2C2_SDA_GPIOA24_PU_SHIFT)

// gpiob pull up/down mask and value
#define ARM_GPIOB0_PU_MASK			(0x1<<ARM_SPI0_CLK_GPIOB0_PU_SHIFT)
#define ARM_GPIOB1_PU_MASK			(0x1<<ARM_SPI0_CSN0_GPIOB1_PU_SHIFT)
#define ARM_GPIOB2_PU_MASK			(0x1<<ARM_SPI0_CSN1_GPIOB2_PU_SHIFT)
#define ARM_GPIOB3_PU_MASK			(0x1<<ARM_SPI0_DIO0_GPIOB3_PU_SHIFT)
#define ARM_GPIOB4_PU_MASK			(0x1<<ARM_SPI0_DIO1_GPIOB4_PU_SHIFT)
#define ARM_GPIOB5_PU_MASK			(0x1<<ARM_SPI0_DIO2_GPIOB5_PU_SHIFT)
#define ARM_GPIOB6_PU_MASK			(0x1<<ARM_SPI0_DIO3_GPIOB6_PU_SHIFT)
#define ARM_GPIOB7_PU_MASK			(0x1<<ARM_SPI1_CLK_GPIOB7_PU_SHIFT)
#define ARM_GPIOB8_PU_MASK			(0x1<<ARM_SPI1_CSN0_GPIOB8_PU_SHIFT)
#define ARM_GPIOB9_PU_MASK			(0x1<<ARM_SPI1_CSN1_GPIOB9_PU_SHIFT)
#define ARM_GPIOB10_PU_MASK			(0x1<<ARM_SPI1_DIO0_GPIOB10_PU_SHIFT)
#define ARM_GPIOB11_PU_MASK			(0x1<<ARM_SPI1_DIO1_GPIOB11_PU_SHIFT)
#define ARM_GPIOB12_PU_MASK			(0x1<<ARM_SPI2_CLK_GPIOB12_PU_SHIFT)
#define ARM_GPIOB13_PU_MASK			(0x1<<ARM_SPI2_CSN0_GPIOB13_PU_SHIFT)
#define ARM_GPIOB14_PU_MASK			(0x1<<ARM_SPI2_CSN1_GPIOB14_PU_SHIFT)
#define ARM_GPIOB15_PU_MASK			(0x1<<ARM_SPI2_DIO0_GPIOB15_PU_SHIFT)
#define ARM_GPIOB16_PU_MASK			(0x1<<ARM_SPI2_DIO1_GPIOB16_PU_SHIFT)

#define ARM_GPIOB0_PU_VAL(val)		((val)<<ARM_SPI0_CLK_GPIOB0_PU_SHIFT)
#define ARM_GPIOB1_PU_VAL(val)		((val)<<ARM_SPI0_CSN0_GPIOB1_PU_SHIFT)
#define ARM_GPIOB2_PU_VAL(val)		((val)<<ARM_SPI0_CSN1_GPIOB2_PU_SHIFT)
#define ARM_GPIOB3_PU_VAL(val)		((val)<<ARM_SPI0_DIO0_GPIOB3_PU_SHIFT)
#define ARM_GPIOB4_PU_VAL(val)		((val)<<ARM_SPI0_DIO1_GPIOB4_PU_SHIFT)
#define ARM_GPIOB5_PU_VAL(val)		((val)<<ARM_SPI0_DIO2_GPIOB5_PU_SHIFT)
#define ARM_GPIOB6_PU_VAL(val)		((val)<<ARM_SPI0_DIO3_GPIOB6_PU_SHIFT)
#define ARM_GPIOB7_PU_VAL(val)		((val)<<ARM_SPI1_CLK_GPIOB7_PU_SHIFT)
#define ARM_GPIOB8_PU_VAL(val)		((val)<<ARM_SPI1_CSN0_GPIOB8_PU_SHIFT)
#define ARM_GPIOB9_PU_VAL(val)		((val)<<ARM_SPI1_CSN1_GPIOB9_PU_SHIFT)
#define ARM_GPIOB10_PU_VAL(val)		((val)<<ARM_SPI1_DIO0_GPIOB10_PU_SHIFT)
#define ARM_GPIOB11_PU_VAL(val)		((val)<<ARM_SPI1_DIO1_GPIOB11_PU_SHIFT)
#define ARM_GPIOB12_PU_VAL(val)		((val)<<ARM_SPI2_CLK_GPIOB12_PU_SHIFT)
#define ARM_GPIOB13_PU_VAL(val)		((val)<<ARM_SPI2_CSN0_GPIOB13_PU_SHIFT)
#define ARM_GPIOB14_PU_VAL(val)		((val)<<ARM_SPI2_CSN1_GPIOB14_PU_SHIFT)
#define ARM_GPIOB15_PU_VAL(val)		((val)<<ARM_SPI2_DIO0_GPIOB15_PU_SHIFT)
#define ARM_GPIOB16_PU_VAL(val)		((val)<<ARM_SPI2_DIO1_GPIOB16_PU_SHIFT)

// gpioa pull up/down mask and value
#define ARM_GPIOA20_PU_MASK			(0x1<<ARM_I2C0_SDA_GPIOA20_PU_SHIFT)
#define ARM_GPIOA21_PU_MASK			(0x1<<ARM_I2C0_SCL_GPIOA21_PU_SHIFT)
#define ARM_GPIOA22_PU_MASK			(0x1<<ARM_I2C1_SDA_GPIOA22_PU_SHIFT)
#define ARM_GPIOA23_PU_MASK			(0x1<<ARM_I2C1_SCL_GPIOA23_PU_SHIFT)
#define ARM_GPIOA24_PU_MASK			(0x1<<ARM_I2C2_SDA_GPIOA24_PU_SHIFT)
#define ARM_GPIOA25_PU_MASK			(0x1<<ARM_I2C2_SCL_GPIOA25_PU_SHIFT)

#define ARM_GPIOA20_PU_VAL(val)		((val)<<ARM_I2C0_SDA_GPIOA20_PU_SHIFT)
#define ARM_GPIOA21_PU_VAL(val)		((val)<<ARM_I2C0_SCL_GPIOA21_PU_SHIFT)
#define ARM_GPIOA22_PU_VAL(val)		((val)<<ARM_I2C1_SDA_GPIOA22_PU_SHIFT)
#define ARM_GPIOA23_PU_VAL(val)		((val)<<ARM_I2C1_SCL_GPIOA23_PU_SHIFT)
#define ARM_GPIOA24_PU_VAL(val)		((val)<<ARM_I2C2_SDA_GPIOA24_PU_SHIFT)
#define ARM_GPIOA25_PU_VAL(val)		((val)<<ARM_I2C2_SCL_GPIOA25_PU_SHIFT)

// offset: 0x00CC, ARM_PU(GPIOB,TIMER,SYSTEM) register
typedef enum
{
	ARM_GPIOB17_PU_SHIFT						= 0,	// Bit[0]: GPIOB17 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[0]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[0]=0)

	ARM_GPIOB18_PU_SHIFT						= 1,	// Bit[1]: GPIOB18 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[1]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[1]=0)

	ARM_GPIOB19_PU_SHIFT						= 2,	// Bit[2]: GPIOB19 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[2]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[2]=0)

	ARM_GPIOB20_PU_SHIFT						= 3,	// Bit[3]: GPIOB20 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[3]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[3]=0)

	ARM_GPIOB21_PU_SHIFT						= 4,	// Bit[4]: GPIOB21 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[4]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[4]=0)

	ARM_GPIOB22_PU_SHIFT						= 5,	// Bit[5]: GPIOB22 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[5]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[5]=0)

	ARM_GPIOB23_PU_SHIFT						= 6,	// Bit[6]: GPIOB23 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[6]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[6]=0)

	ARM_GPIOB24_PU_SHIFT						= 7,	// Bit[7]: GPIOB24 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[7]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[7]=0)

	ARM_GPIOB25_PU_SHIFT						= 8,	// Bit[8]: GPIOB25 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[8]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[8]=0)

	ARM_GPIOB26_PU_SHIFT						= 9,	// Bit[9]: GPIOB26 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[9]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[9]=0)

	ARM_GPIOB27_PU_SHIFT						= 10,	// Bit[10]: GPIOB27 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[10]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[10]=0)

	ARM_GPIOB28_PU_SHIFT						= 11,	// Bit[11]: GPIOB28 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[11]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[11]=0)

	ARM_GPIOB29_PU_SHIFT						= 12,	// Bit[12]: GPIOB29 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[12]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[12]=0)

	ARM_GPIOB30_PU_SHIFT						= 13,	// Bit[13]: GPIOB30 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[13]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[13]=0)

	ARM_GPIOB31_PU_SHIFT						= 14,	// Bit[14]: GPIOB31 Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[14]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[14]=0)

	ARM_REVD0_PU_SHIFT							= 15,	// Bit[15]: Reserved.

	ARM_USB0_OC_PU_SHIFT						= 16,	// Bit[16]: USB0_OC Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[16]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[16]=0)

	ARM_USB1_OC_PU_SHIFT						= 17,	// Bit[17]: USB1_OC Pin pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[17]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[17]=0)

	ARM_TIMER_W_OUT_GPIOD0_PU_SHIFT				= 18,	// Bit[18]: TIMER_W_OUT Pin(GPIOD[0]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[18]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[18]=0)

	ARM_TIMER_X_OUT_GPIOD1_PU_SHIFT				= 19,	// Bit[19]: TIMER_X_OUT Pin(GPIOD[1]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[19]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[19]=0)

	ARM_TIMER_Y_OUT_GPIOD2_PU_SHIFT				= 20,	// Bit[20]: TIMER_Y_OUT Pin(GPIOD[2]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[20]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[20]=0)

	ARM_TIMER_Z_OUT_GPIOD3_PU_SHIFT				= 21,	// Bit[21]: TIMER_Z_OUT Pin(GPIOD[3]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[21]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[21]=0)

	ARM_RTC_CLKOUT_GPIOD31_PU_SHIFT				= 22,	// Bit[22]: RTC_CLKOUT Pin(GPIOD[31]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[22]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[22]=0)

	ARM_RTC_CAL_CLKOUT_GPIOD29_PU_SHIFT			= 23,	// Bit[23]: RTC_CAL_CLKOUT Pin(GPIOD[29]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[23]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[23]=0)

	ARM_CLKOUT_GPIOD30_PU_SHIFT					= 24,	// Bit[24]: CLKOUT Pin(GPIOD[30]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[24]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[24]=0)

	ARM_HOST_LOW_POWER_OUT_GPIOA29_PU_SHIFT		= 25,	// Bit[25]: HOST_LOW_POWER_OUT Pin(GPIOA[29]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[25]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[25]=0)

	ARM_EPF_FAILSAFE_TIMER_OUT_GPIOA30_PU_SHIFT	= 26,	// Bit[26]: EPF_FAILSAFE_TIMER_OUT Pin(GPIOA[30]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[26]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[26]=0)

	ARM_EPF_SLEEP_MODE_DONE_GPIOA31_PU_SHIFT	= 27,	// Bit[27]: ARM_EPF_SLEEP_MODE_DONE Pin(GPIOA[31]) pull up/pull down select signal. Default: 1
														// 0: pull down is selected when pull logic is active(ARM_PD[27]=0)
														// 1: pull up is selected when pull logic is active(ARM_PD[27]=0)
	
	ARM_REVD1_PU_SHIFT							= 28	// Bit[31:28]: Reserved.
}E_ARM_PU_SHIFT;

// gpiob pull up/down mask and value
#define ARM_GPIOB17_PU_MASK			(0x1<<ARM_GPIOB17_PU_SHIFT)
#define ARM_GPIOB18_PU_MASK			(0x1<<ARM_GPIOB18_PU_SHIFT)
#define ARM_GPIOB19_PU_MASK			(0x1<<ARM_GPIOB19_PU_SHIFT)
#define ARM_GPIOB20_PU_MASK			(0x1<<ARM_GPIOB20_PU_SHIFT)
#define ARM_GPIOB21_PU_MASK			(0x1<<ARM_GPIOB21_PU_SHIFT)
#define ARM_GPIOB22_PU_MASK			(0x1<<ARM_GPIOB22_PU_SHIFT)
#define ARM_GPIOB23_PU_MASK			(0x1<<ARM_GPIOB23_PU_SHIFT)
#define ARM_GPIOB24_PU_MASK			(0x1<<ARM_GPIOB24_PU_SHIFT)
#define ARM_GPIOB25_PU_MASK			(0x1<<ARM_GPIOB25_PU_SHIFT)
#define ARM_GPIOB26_PU_MASK			(0x1<<ARM_GPIOB26_PU_SHIFT)
#define ARM_GPIOB27_PU_MASK			(0x1<<ARM_GPIOB27_PU_SHIFT)
#define ARM_GPIOB28_PU_MASK			(0x1<<ARM_GPIOB28_PU_SHIFT)
#define ARM_GPIOB29_PU_MASK			(0x1<<ARM_GPIOB29_PU_SHIFT)
#define ARM_GPIOB30_PU_MASK			(0x1<<ARM_GPIOB30_PU_SHIFT)
#define ARM_GPIOB31_PU_MASK			(0x1<<ARM_GPIOB31_PU_SHIFT)

#define ARM_GPIOB17_PU_VAL(val)		((val)<<ARM_GPIOB17_PU_SHIFT)
#define ARM_GPIOB18_PU_VAL(val)		((val)<<ARM_GPIOB18_PU_SHIFT)
#define ARM_GPIOB19_PU_VAL(val)		((val)<<ARM_GPIOB19_PU_SHIFT)
#define ARM_GPIOB20_PU_VAL(val)		((val)<<ARM_GPIOB20_PU_SHIFT)
#define ARM_GPIOB21_PU_VAL(val)		((val)<<ARM_GPIOB21_PU_SHIFT)
#define ARM_GPIOB22_PU_VAL(val)		((val)<<ARM_GPIOB22_PU_SHIFT)
#define ARM_GPIOB23_PU_VAL(val)		((val)<<ARM_GPIOB23_PU_SHIFT)
#define ARM_GPIOB24_PU_VAL(val)		((val)<<ARM_GPIOB24_PU_SHIFT)
#define ARM_GPIOB25_PU_VAL(val)		((val)<<ARM_GPIOB25_PU_SHIFT)
#define ARM_GPIOB26_PU_VAL(val)		((val)<<ARM_GPIOB26_PU_SHIFT)
#define ARM_GPIOB27_PU_VAL(val)		((val)<<ARM_GPIOB27_PU_SHIFT)
#define ARM_GPIOB28_PU_VAL(val)		((val)<<ARM_GPIOB28_PU_SHIFT)
#define ARM_GPIOB29_PU_VAL(val)		((val)<<ARM_GPIOB29_PU_SHIFT)
#define ARM_GPIOB30_PU_VAL(val)		((val)<<ARM_GPIOB30_PU_SHIFT)
#define ARM_GPIOB31_PU_VAL(val)		((val)<<ARM_GPIOB31_PU_SHIFT)

// usb0/1 oc pull up/down mask and value  
#define ARM_USB0_OC_PU_MASK			(0x1<<ARM_USB0_OC_PU_SHIFT)
#define ARM_USB1_OC_PU_MASK			(0x1<<ARM_USB1_OC_PU_SHIFT)

#define ARM_USB0_OC_PU_VAL(val)		((val)<<ARM_USB0_OC_PU_SHIFT)
#define ARM_USB1_OC_PU_VAL(val)		((val)<<ARM_USB1_OC_PU_SHIFT)

// gpiod pull up/down mask and value
#define ARM_GPIOD0_PU_MASK			(0x1<<ARM_TIMER_W_OUT_GPIOD0_PU_SHIFT)
#define ARM_GPIOD1_PU_MASK			(0x1<<ARM_TIMER_X_OUT_GPIOD1_PU_SHIFT)
#define ARM_GPIOD2_PU_MASK			(0x1<<ARM_TIMER_Y_OUT_GPIOD2_PU_SHIFT)
#define ARM_GPIOD3_PU_MASK			(0x1<<ARM_TIMER_Z_OUT_GPIOD3_PU_SHIFT)
#define ARM_GPIOD31_PU_MASK			(0x1<<ARM_RTC_CLKOUT_GPIOD31_PU_SHIFT)
#define ARM_GPIOD29_PU_MASK			(0x1<<ARM_RTC_CAL_CLKOUT_GPIOD29_PU_SHIFT)
#define ARM_GPIOD30_PU_MASK			(0x1<<ARM_CLKOUT_GPIOD30_PU_SHIFT)

#define ARM_GPIOD0_PU_VAL(val)		((val)<<ARM_TIMER_W_OUT_GPIOD0_PU_SHIFT)
#define ARM_GPIOD1_PU_VAL(val)		((val)<<ARM_TIMER_X_OUT_GPIOD1_PU_SHIFT)
#define ARM_GPIOD2_PU_VAL(val)		((val)<<ARM_TIMER_Y_OUT_GPIOD2_PU_SHIFT)
#define ARM_GPIOD3_PU_VAL(val)		((val)<<ARM_TIMER_Z_OUT_GPIOD3_PU_SHIFT)
#define ARM_GPIOD31_PU_VAL(val)		((val)<<ARM_RTC_CLKOUT_GPIOD31_PU_SHIFT)
#define ARM_GPIOD29_PU_VAL(val)		((val)<<ARM_RTC_CAL_CLKOUT_GPIOD29_PU_SHIFT)
#define ARM_GPIOD30_PU_VAL(val)		((val)<<ARM_CLKOUT_GPIOD30_PU_SHIFT)

// gpioa pull up/down mask and value
#define ARM_GPIOA29_PU_MASK			(0x1<<ARM_HOST_LOW_POWER_OUT_GPIOA29_PU_SHIFT)
#define ARM_GPIOA30_PU_MASK			(0x1<<ARM_EPF_FAILSAFE_TIMER_OUT_GPIOA30_PU_SHIFT)
#define ARM_GPIOA31_PU_MASK			(0x1<<ARM_EPF_SLEEP_MODE_DONE_GPIOA31_PU_SHIFT)

#define ARM_GPIOA29_PU_VAL(val)		((val)<<ARM_HOST_LOW_POWER_OUT_GPIOA29_PU_SHIFT)
#define ARM_GPIOA30_PU_VAL(val)		((val)<<ARM_EPF_FAILSAFE_TIMER_OUT_GPIOA30_PU_SHIFT)
#define ARM_GPIOA31_PU_VAL(val)		((val)<<ARM_EPF_SLEEP_MODE_DONE_GPIOA31_PU_SHIFT)

// time w/x/y/z output pull up/down mask and value
#define ARM_TIMERW_OUT_PU_MASK					(0x1<<ARM_TIMER_W_OUT_GPIOD0_PU_SHIFT)
#define ARM_TIMERX_OUT_PU_MASK					(0x1<<ARM_TIMER_X_OUT_GPIOD1_PU_SHIFT)
#define ARM_TIMERY_OUT_PU_MASK					(0x1<<ARM_TIMER_Y_OUT_GPIOD2_PU_SHIFT)
#define ARM_TIMERZ_OUT_PU_MASK					(0x1<<ARM_TIMER_Z_OUT_GPIOD3_PU_SHIFT)

#define ARM_TIMERW_OUT_PU_VAL(val)				((val)<<ARM_TIMER_W_OUT_GPIOD0_PU_SHIFT)
#define ARM_TIMERX_OUT_PU_VAL(val)				((val)<<ARM_TIMER_X_OUT_GPIOD1_PU_SHIFT)
#define ARM_TIMERY_OUT_PU_VAL(val)				((val)<<ARM_TIMER_Y_OUT_GPIOD2_PU_SHIFT)
#define ARM_TIMERZ_OUT_PU_VAL(val)				((val)<<ARM_TIMER_Z_OUT_GPIOD3_PU_SHIFT)

// rtc clkout up/down mask and value
#define ARM_RTC_CLKOUT_PU_MASK					(0x1<<ARM_RTC_CLKOUT_GPIOD31_PU_SHIFT)
#define ARM_RTC_CLKOUT_PU_VAL(val)				((val)<<ARM_RTC_CLKOUT_GPIOD31_PU_SHIFT)

// rtc cal clkout up/down mask and value
#define ARM_RTC_CAL_CLKOUT_PU_MASK				(0x1<<ARM_RTC_CAL_CLKOUT_GPIOD29_PU_SHIFT)
#define ARM_RTC_CAL_CLKOUT_PU_VAL(val)			((val)<<ARM_RTC_CAL_CLKOUT_GPIOD29_PU_SHIFT)

// arm clkout pull up/down mask and value
#define ARM_CLKOUT_PU_MASK						(0x1<<ARM_CLKOUT_GPIOD30_PU_SHIFT)
#define ARM_CLKOUT_PU_VAL(val)					((val)<<ARM_CLKOUT_GPIOD30_PU_SHIFT)

// host low poer out pull up/down mask and value
#define ARM_HOST_LOW_POWER_OUT_PU_MASK			(0x1<<ARM_HOST_LOW_POWER_OUT_GPIOA29_PU_SHIFT)
#define ARM_HOST_LOW_POWER_OUT_PU_VAL(val)		((val)<<ARM_HOST_LOW_POWER_OUT_GPIOA29_PU_SHIFT)

// epf failsafe timer out pull up/down mask and value
#define ARM_EPF_FAILSAFE_TIMER_OUT_PU_MASK		(0x1<<ARM_EPF_FAILSAFE_TIMER_OUT_GPIOA30_PU_SHIFT)
#define ARM_EPF_FAILSAFE_TIMER_OUT_PU_VAL(val)	((val)<<ARM_EPF_FAILSAFE_TIMER_OUT_GPIOA30_PU_SHIFT)

// epf sleep mode done pull up/down mask and value
#define ARM_EPF_SLEEP_MODE_DONE_PU_MASK			(0x1<<ARM_EPF_SLEEP_MODE_DONE_GPIOA31_PU_SHIFT)
#define ARM_EPF_SLEEP_MODE_DONE_PU_VAL(val)		((val)<<ARM_EPF_SLEEP_MODE_DONE_GPIOA31_PU_SHIFT)

// ARM IO Pull Disable Register
typedef enum
{
	
	IO_PD_EN 	= 0, // pull up/pull down is active										
	IO_PD_DIS 	= 1	 // pull up/pull down is disabled
}E_IO_PD;

// offset: 0x00D0, SD_PD register
typedef enum
{
	ARM_SDIO0_CLK_GPIOC0_PD_SHIFT		= 0,	// Bit[0]: SDIO0_CLK Pin(GPIOC[0]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_CMD_GPIOC1_PD_SHIFT		= 1,	// Bit[1]: SDIO0_CMD Pin(GPIOC[1]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA0_GPIOC2_PD_SHIFT		= 2,	// Bit[2]: SDIO0_DATA0 Pin(GPIOC[2]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA1_GPIOC3_PD_SHIFT		= 3,	// Bit[3]: SDIO0_DATA1 Pin(GPIOC[3]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA2_GPIOC4_PD_SHIFT		= 4,	// Bit[4]: SDIO0_DATA2 Pin(GPIOC[4]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA3_GPIOC5_PD_SHIFT		= 5,	// Bit[5]: SDIO0_DATA3 Pin(GPIOC[5]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA4_GPIOC6_PD_SHIFT		= 6,	// Bit[6]: SDIO0_DATA4 Pin(GPIOC[6]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA5_GPIOC7_PD_SHIFT		= 7,	// Bit[7]: SDIO0_DATA5 Pin(GPIOC[7]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA6_GPIOC8_PD_SHIFT		= 8,	// Bit[8]: SDIO0_DATA6 Pin(GPIOC[8]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_DATA7_GPIOC9_PD_SHIFT		= 9,	// Bit[9]: SDIO0_DATA7 Pin(GPIOC[9]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_CD_GPIOC25_PD_SHIFT		= 10,	// Bit[10]: SDIO0_CD Pin(GPIOC[25]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_WP_GPIOC26_PD_SHIFT		= 11,	// Bit[11]: SDIO0_WP Pin(GPIOC[26]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_CLK_GPIOC10_PD_SHIFT		= 12,	// Bit[12]: SDIO1_CLK Pin(GPIOC[10]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_CMD_GPIOC11_PD_SHIFT		= 13,	// Bit[13]: SDIO1_CMD Pin(GPIOC[11]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA0_GPIOC12_PD_SHIFT	= 14,	// Bit[14]: SDIO1_DATA0 Pin(GPIOC[12]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA1_GPIOC13_PD_SHIFT	= 15,	// Bit[15]: SDIO1_DATA1 Pin(GPIOC[13]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA2_GPIOC14_PD_SHIFT	= 16,	// Bit[16]: SDIO1_DATA2 Pin(GPIOC[14]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA3_GPIOC15_PD_SHIFT	= 17,	// Bit[17]: SDIO1_DATA3 Pin(GPIOC15) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA4_GPIOC16_PD_SHIFT	= 18,	// Bit[18]: SDIO1_DATA4 Pin(GPIOC[16]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA5_GPIOC17_PD_SHIFT	= 19,	// Bit[19]: SDIO1_DATA5 Pin(GPIOC[17]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA6_GPIOC18_PD_SHIFT	= 20,	// Bit[20]: SDIO1_DATA6 Pin(GPIOC[18]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_DATA7_GPIOC19_PD_SHIFT	= 21,	// Bit[21]: SDIO1_DATA7 Pin(GPIOC[19]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_CD_GPIOC27_PD_SHIFT		= 22,	// Bit[22]: SDIO1_CD Pin(GPIOC[27]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_WP_GPIOC28_PD_SHIFT		= 23,	// Bit[23]: SDIO1_WP Pin(GPIOC[28]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO0_RSTN_GPIOC29_PD_SHIFT		= 24,	// Bit[24]: SDIO0_RSTN Pin(GPIOC[29]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled

	ARM_SDIO1_RSTN_GPIOC30_PD_SHIFT		= 25,	// Bit[25]: SDIO1_RSTN Pin(GPIOC[30]) pull disable signal. Default: 0
												// 0: pull up/pull down is active
												// 1: pull up/pull down is disabled
		
	ARM_SD_REVD_PD_SHIFT				= 26	// Bit[31:26]: reserved.
}E_ARM_SD_PD_SHIFT;

// offset: 0x00D4, ARM_UART_PD register
typedef enum
{
	ARM_UART0_TXD_GPIOA0_PD_SHIFT	= 0,	// Bit[0]: UART0_TXD Pin(GPIOA[0]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART0_RXD_GPIOA1_PD_SHIFT	= 1,	// Bit[1]: UART0_RXD Pin(GPIOA[1]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART0_CTSN_GPIOA2_PD_SHIFT	= 2,	// Bit[2]: UART0_CTSN Pin(GPIOA[2]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART0_RTSN_GPIOA3_PD_SHIFT	= 3,	// Bit[3]: UART0_RTSN Pin(GPIOA[3]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART1_TXD_GPIOA4_PD_SHIFT	= 4,	// Bit[4]: UART1_TXD Pin(GPIOA[4]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART1_RXD_GPIOA5_PD_SHIFT	= 5,	// Bit[5]: UART1_RXD Pin(GPIOA[5]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART1_CTSN_GPIOA6_PD_SHIFT	= 6,	// Bit[6]: UART1_CTSN Pin(GPIOA[6]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART1_RTSN_GPIOA7_PD_SHIFT	= 7,	// Bit[7]: UART1_RTSN Pin(GPIOA[7]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART2_TXD_GPIOA8_PD_SHIFT	= 8,	// Bit[8]: UART2_TXD Pin(GPIOA[8]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART2_RXD_GPIOA9_PD_SHIFT	= 9,	// Bit[9]: UART2_RXD Pin(GPIOA[9]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART2_CTSN_GPIOA10_PD_SHIFT	= 10,	// Bit[10]: UART2_CTSN Pin(GPIOA[10]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART2_RTSN_GPIOA11_PD_SHIFT	= 11,	// Bit[11]: UART2_RTSN Pin(GPIOA[11]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_TXD_GPIOA12_PD_SHIFT	= 12,	// Bit[12]: UART3_TXD Pin(GPIOA[12]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_RXD_GPIOA13_PD_SHIFT	= 13,	// Bit[13]: UART3_RXD Pin(GPIOA[13]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_CTSN_GPIOA14_PD_SHIFT	= 14,	// Bit[14]: UART3_CTSN Pin(GPIOA[14]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_RTSN_GPIOA15_PD_SHIFT	= 15,	// Bit[15]: UART3_RTSN Pin(GPIOA[15]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_DCDN_GPIOA16_PD_SHIFT	= 16,	// Bit[16]: UART3_DCDN Pin(GPIOA[16]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_DSR_GPIOA17_PD_SHIFT	= 17,	// Bit[17]: UART3_DSR Pin(GPIOA[17]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_DTR_GPIOA18_PD_SHIFT	= 18,	// Bit[18]: UART3_DTR Pin(GPIOA[18]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_UART3_RIN_GPIOA19_PD_SHIFT	= 19,	// Bit[19]: UART3_RIN Pin(GPIOA[19]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled
	
	ARM_UART_REVD_PD_SHIFT			= 20	// Bit[31:20]: Reserved.
}E_ARM_UART_PD_SHIFT;

// offset: 0x00D8, ARM_SPI_I2C_PD register
typedef enum
{
	ARM_SPI0_CLK_GPIOB0_PD_SHIFT	= 0,	// Bit[0]: SPI0_CLK Pin(GPIOB[0]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI0_CSN0_GPIOB1_PD_SHIFT	= 1,	// Bit[1]: SPI0_CSN0 Pin(GPIOB[1]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI0_CSN1_GPIOB2_PD_SHIFT	= 2,	// Bit[2]: SPI0_CSN1 Pin(GPIOB[2]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI0_DIO0_GPIOB3_PD_SHIFT	= 3,	// Bit[3]: SPI0_DIO0 Pin(GPIOB[3]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI0_DIO1_GPIOB4_PD_SHIFT	= 4,	// Bit[4]: SPI0_DIO1 Pin(GPIOB[4]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI0_DIO2_GPIOB5_PD_SHIFT	= 5,	// Bit[5]: SPI0_DIO2 Pin(GPIOB[5]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI0_DIO3_GPIOB6_PD_SHIFT	= 6,	// Bit[6]: SPI0_DIO3 Pin(GPIOB[6]) pull disable signal. Default: 1
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI1_CLK_GPIOB7_PD_SHIFT	= 7,	// Bit[7]: SPI1_CLK Pin(GPIOB[7]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI1_CSN0_GPIOB8_PD_SHIFT	= 8,	// Bit[8]: SPI1_CSN0 Pin(GPIOB[8]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI1_CSN1_GPIOB9_PD_SHIFT	= 9,	// Bit[9]: SPI1_CSN1 Pin(GPIOB[9]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI1_DIO0_GPIOB10_PD_SHIFT	= 10,	// Bit[10]: SPI1_DIO0 Pin(GPIOB[10]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI1_DIO1_GPIOB11_PD_SHIFT	= 11,	// Bit[11]: SPI1_DIO1 Pin(GPIOB[11]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI2_CLK_GPIOB12_PD_SHIFT	= 12,	// Bit[12]: SPI2_CLK Pin(GPIOB[12]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI2_CSN0_GPIOB13_PD_SHIFT	= 13,	// Bit[13]: SPI2_CSN0 Pin(GPIOB[13]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI2_CSN1_GPIOB14_PD_SHIFT	= 14,	// Bit[14]: SPI2_CSN1 Pin(GPIOB[14]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI2_DIO0_GPIOB15_PD_SHIFT	= 15,	// Bit[15]: SPI2_DIO0 Pin(GPIOB[15]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI2_DIO1_GPIOB16_PD_SHIFT	= 16,	// Bit[16]: SPI2_DIO1 Pin(GPIOB[16]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI_I2C_REVD0_PD_SHIFT		= 17,	// Bit[19:17]: reserved.

	ARM_I2C0_SDA_GPIOA20_PD_SHIFT	= 20,	// Bit[20]: I2C0_SDA Pin(GPIOA[20]) pull diable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_I2C0_SCL_GPIOA21_PD_SHIFT	= 21,	// Bit[21]: I2C0_SCL Pin(GPIOA[21]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_I2C1_SDA_GPIOA22_PD_SHIFT	= 22,	// Bit[22]: I2C1_SDA Pin(GPIOA[22]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_I2C1_SCL_GPIOA23_PD_SHIFT	= 23,	// Bit[23]: I2C1_SCL Pin(GPIOA[23]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_I2C_SDA_GPIOA24_PD_SHIFT	= 24,	// Bit[24]: I2C2_SDA Pin(GPIOA[24]) pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_I2C_SCL_GPIOA25_PD_SHIFT	= 25,	// Bit[25]: I2C2_SCL(GPIOA[25]) Pin pull disable signal. Default: 0
											// 0: pull up/pull down is active
											// 1: pull up/pull down is disabled

	ARM_SPI_I2C_REVD1_PD_SHIFT		= 26	// Bit[31:26]: Reserved
}E_ARM_SPI_I2C_PD_SHIFT;

// offset: 0x00DC, ARM_PD(GPIOB,TIMER,SYSTEM) register
typedef enum
{
	ARM_GPIOB17_PD_SHIFT						= 0,	// Bit[0]: GPIOB17 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB18_PD_SHIFT						= 1,	// Bit[1]: GPIOB18 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB19_PD_SHIFT						= 2,	// Bit[2]: GPIOB19 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB20_PD_SHIFT						= 3,	// Bit[3]: GPIOB20 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB21_PD_SHIFT						= 4,	// Bit[4]: GPIOB21 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB22_PD_SHIFT						= 5,	// Bit[5]: GPIOB22 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB23_PD_SHIFT						= 6,	// Bit[6]: GPIOB23 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB24_PD_SHIFT						= 7,	// Bit[7]: GPIOB24 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB25_PD_SHIFT						= 8,	// Bit[8]: GPIOB25 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB26_PD_SHIFT						= 9,	// Bit[9]: GPIOB26 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB27_PD_SHIFT						= 10,	// Bit[10]: GPIOB27 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB28_PD_SHIFT						= 11,	// Bit[11]: GPIOB28 Pin pull disable  signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB29_PD_SHIFT						= 12,	// Bit[12]: GPIOB29 Pin pull disable  signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB30_PD_SHIFT						= 13,	// Bit[13]: GPIOB30 Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_GPIOB31_PD_SHIFT						= 14,	// Bit[14]: GPIOB31 Pin pull disable  signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_REVD0_PD_SHIFT							= 15,	// Bit[15]: Reserved.

	ARM_USB0_OC_PD_SHIFT						= 16,	// Bit[16]: USB0_OC Pin pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_USB1_OC_PD_SHIFT						= 17,	// Bit[17]: USB1_OC Pin pull disable  signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_TIMER_W_OUT_GPIOD0_PD_SHIFT				= 18,	// Bit[18]: TIMER_W_OUT Pin(GPIOD[0]) pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_TIMER_X_OUT_GPIOD1_PD_SHIFT				= 19,	// Bit[19]: TIMER_X_OUT Pin(GPIOD[1]) pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled
	
	ARM_TIMER_Y_OUT_GPIOD2_PD_SHIFT				= 20,	// Bit[20]: TIMER_Y_OUT Pin(GPIOD[2]) pull disable  signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_TIMER_Z_OUT_GPIOD3_PD_SHIFT				= 21,	// Bit[21]: TIMER_Z_OUT Pin(GPIOD[3]) pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_RTC_CLKOUT_GPIOD31_PD_SHIFT				= 22,	// Bit[22]: RTC_CLKOUT Pin(GPIOD[31]) pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_RTC_CAL_CLKOUT_GPIOD29_PD_SHIFT			= 23,	// Bit[23]: RTC_CAL_CLKOUT Pin(GPIOD[29]) pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_CLKOUT_GPIOD30_PD_SHIFT					= 24,	// Bit[24]: CLKOUT Pin(GPIOD[30]) pull disable signal. Default: 0
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_HOST_LOW_POWER_OUT_GPIOA29_PD_SHIFT		= 25,	// Bit[25]: HOST_LOW_POWER_OUT Pin(GPIOA[29]) pull disable  signal. Default: 1
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_EPF_FAILSAFE_TIMER_OUT_GPIOA30_PD_SHIFT	= 26,	// Bit[26]: EPF_FAILSAFE_TIMER_OUT Pin(GPIOA[30]) pull disable signal. Default: 1
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled

	ARM_EPF_SLEEP_MODE_DONE_GPIOA31_PD_SHIFT	= 27,	// Bit[27]: ARM_EPF_SLEEP_MODE_DONE Pin(GPIOA[31]) pull disable signal. Default: 1
														// 0: pull up/pull down is active
														// 1: pull up/pull down is disabled
	
	ARM_REVD1_PD_SHIFT							= 28	// Bit[31:28]: Reserved.
}E_ARM_PD_SHIFT;

// ARM IO Driving Strength Select Register
// offset: 0x00E0, ARM_UART_I2C_DS register
typedef enum
{
	ARM_UART0_TXD_DS_SHIFT		= 0,	// Bit[0]: UART0_TXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART0_RXD_DS_SHIFT		= 1,	// Bit[1]: UART0_RXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART0_CTSN_DS_SHIFT		= 2,	// Bit[2]: UART0_CTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART0_RTSN_DS_SHIFT		= 3,	// Bit[3]: UART0_RTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART1_TXD_DS_SHIFT		= 4,	// Bit[4]: UART1_TXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART1_RXD_DS_SHIFT		= 5,	// Bit[5]: UART1_RXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART1_CTSN_DS_SHIFT		= 6,	// Bit[6]: UART1_CTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART1_RTSN_DS_SHIFT		= 7,	// Bit[7]: UART1_RTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART2_TXD_DS_SHIFT		= 8,	// Bit[8]: UART2_TXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART2_RXD_DS_SHIFT		= 9,	// Bit[9]: UART2_RXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART2_CTSN_DS_SHIFT		= 10,	// Bit[10]: UART2_CTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART2_RTSN_DS_SHIFT		= 11,	// Bit[11]: UART2_RTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_TXD_DS_SHIFT		= 12,	// Bit[12]: UART3_TXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_RXD_DS_SHIFT		= 13,	// Bit[13]: UART3_RXD driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_CTSN_DS_SHIFT		= 14,	// Bit[14]: UART3_CTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_RTSN_DS_SHIFT		= 15,	// Bit[15]: UART3_RTSN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_DCDN_DS_SHIFT		= 16,	// Bit[16]: UART3_DCDN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_DSR_DS_SHIFT		= 17,	// Bit[17]: UART3_DSR driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_DTR_DS_SHIFT		= 18,	// Bit[18]: UART3_DTR driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_UART3_RIN_DS_SHIFT		= 19,	// Bit[19]: UART3_RIN driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_I2C0_SDA_DS_SHIFT		= 20,	// Bit[20]: I2C0_SDA driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_I2C0_SCL_DS_SHIFT		= 21,	// Bit[21]: I2C0_SCL driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_I2C1_SDA_DS_SHIFT		= 22,	// Bit[22]: I2C1_SDA driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_I2C1_SCL_DS_SHIFT		= 23,	// Bit[23]: I2C1_SCL driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_I2C2_SDA_DS_SHIFT		= 24,	// Bit[24]: I2C2_SDA driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA

	ARM_I2C2_SCL_DS_SHIFT		= 25,	// Bit[25]: I2C2_SCL driving strength select signal. Default:0
										// 0: 4mA
										// 1: 8mA
	
	ARM_UART_I2C_REVD_DS_SHIFT	= 26	// Bit[31:26]: Reserved
}E_ARM_UART_I2C_DS_SHIFT;

// IO block
typedef enum
{
	UART0_DS 	= 0,											
	UART1_DS 	= 1,
	UART2_DS 	= 2,
	UART3_DS 	= 3,
	I2C0_DS 	= 4,
	I2C1_DS 	= 5,
	I2C2_DS 	= 6
}E_IO_DS_BLK;

// IO driving strength
typedef enum
{
	IO_DS_4MA 	= 0, // 4 mA											
	IO_DS_8MA 	= 1	 // 8 mA
}E_IO_DS_MA;

// uart 0/1/2/3 IO driving strength mask and value
#define ARM_UART0_DS_MASK					(0x0F<<ARM_UART0_TXD_DS_SHIFT)
#define ARM_UART1_DS_MASK					(0x0F<<ARM_UART1_TXD_DS_SHIFT)
#define ARM_UART2_DS_MASK					(0x0F<<ARM_UART2_TXD_DS_SHIFT)
#define ARM_UART3_DS_MASK					(0xFF<<ARM_UART3_TXD_DS_SHIFT)

#define ARM_UART0_DS_VAL(val)				((val)<<ARM_UART0_TXD_DS_SHIFT)
#define ARM_UART1_DS_VAL(val)				((val)<<ARM_UART1_TXD_DS_SHIFT)
#define ARM_UART2_DS_VAL(val)				((val)<<ARM_UART2_TXD_DS_SHIFT)
#define ARM_UART3_DS_VAL(val)				((val)<<ARM_UART3_TXD_DS_SHIFT)

// uart 0/1/2/3 IO driving strength mask and value
#define ARM_I2C0_DS_MASK					(0x03<<ARM_I2C0_SDA_DS_SHIFT)
#define ARM_I2C1_DS_MASK					(0x03<<ARM_I2C1_SDA_DS_SHIFT)
#define ARM_I2C2_DS_MASK					(0x03<<ARM_I2C2_SDA_DS_SHIFT)

#define ARM_I2C0_DS_VAL(val)				((val)<<ARM_I2C0_SDA_DS_SHIFT)
#define ARM_I2C1_DS_VAL(val)				((val)<<ARM_I2C1_SDA_DS_SHIFT)
#define ARM_I2C2_DS_VAL(val)				((val)<<ARM_I2C2_SDA_DS_SHIFT)

// ARM IO Open Drain Configuration Register
// (Only valid when the pin is as general purpose input/output. It is not valid when the pin is as function IP pin such UART)
typedef enum
{
	
	IO_OD_DIS 	= 0, // normal (it is also called push pull)											
	IO_OD_EN 	= 1	 // open drain
}E_IO_OD;

// offset: 0x00F0, SD_OD register
typedef enum
{
	ARM_SDIO0_CLK_GPIOC0_OD_SHIFT		= 0,	// Bit[0]: SDIO0_CLK Pin(GPIOC[0]) open drain select signal. Default: 0
												// 0: normal (it is also called push pull)
												// 1: open drain

	ARM_SDIO0_CMD_GPIOC1_OD_SHIFT		= 1,	// Bit[1]: SDIO0_CMD Pin(GPIOC[1]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA0_GPIOC2_OD_SHIFT		= 2,	// Bit[2]: SDIO0_DATA0 Pin(GPIOC[2]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA1_GPIOC3_OD_SHIFT		= 3,	// Bit[3]: SDIO0_DATA1 Pin(GPIOC[3]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA2_GPIOC4_OD_SHIFT		= 4,	// Bit[4]: SDIO0_DATA2 Pin(GPIOC[4]) open drain select signal. Default: 0
												// 0: normal
												// 1: pull down

	ARM_SDIO0_DATA3_GPIOC5_OD_SHIFT		= 5,	// Bit[5]: SDIO0_DATA3 Pin(GPIOC[5]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA4_GPIOC6_OD_SHIFT		= 6,	// Bit[6]: SDIO0_DATA4 Pin(GPIOC[6]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA5_GPIOC7_OD_SHIFT		= 7,	// Bit[7]: SDIO0_DATA5 Pin(GPIOC[7]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA6_GPIOC8_OD_SHIFT		= 8,	// Bit[8]: SDIO0_DATA6 Pin(GPIOC[8]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_DATA7_GPIOC9_OD_SHIFT		= 9,	// Bit[9]: SDIO0_DATA7 Pin(GPIOC[9]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain
	
	ARM_SDIO0_CD_GPIOC25_OD_SHIFT		= 10,	// Bit[10]: SDIO0_CD Pin(GPIOC[25]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_WP_GPIOC26_OD_SHIFT		= 11,	// Bit[11]: SDIO0_WP Pin(GPIOC[26]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_CLK_GPIOC10_OD_SHIFT		= 12,	// Bit[12]: SDIO1_CLK Pin(GPIOC[10]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_CMD_GPIOC11_OD_SHIFT		= 13,	// Bit[13]: SDIO1_CMD Pin(GPIOC[11]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA0_GPIOC12_OD_SHIFT	= 14,	// Bit[14]: SDIO1_DATA0 Pin(GPIOC[12]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA1_GPIOC13_OD_SHIFT	= 15,	// Bit[15]: SDIO1_DATA1 Pin(GPIOC[13]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA2_GPIOC14_OD_SHIFT	= 16,	// Bit[16]: SDIO1_DATA2 Pin(GPIOC[14]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA3_GPIOC15_OD_SHIFT	= 17,	// Bit[17]: SDIO1_DATA3 Pin(GPIOC[15]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA4_GPIOC16_OD_SHIFT	= 18,	// Bit[18]: SDIO1_DATA4 Pin(GPIOC[16]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA5_GPIOC17_OD_SHIFT	= 19,	// Bit[19]: SDIO1_DATA5 Pin(GPIOC[17]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA6_GPIOC18_OD_SHIFT	= 20,	// Bit[20]: SDIO1_DATA6 Pin(GPIOC[18]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_DATA7_GPIOC19_OD_SHIFT	= 21,	// Bit[21]: SDIO1_DATA7 Pin(GPIOC[19]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_CD_GPIOC27_OD_SHIFT		= 22,	// Bit[22]: SDIO1_CD Pin(GPIOC[27]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_WP_GPIOC28_OD_SHIFT		= 23,	// Bit[23]: SDIO1_WP Pin(GPIOC[28]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO0_RSTN_GPIOC29_OD_SHIFT		= 24,	// Bit[24]: SDIO0_RSTN Pin(GPIOC[29]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SDIO1_RSTN_GPIOC30_OD_SHIFT		= 25,	// Bit[25]: SDIO1_RSTN Pin(GPIOC[30]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SD_REVD_OD_SHIFT				= 26	// Bit[31:26]: reserved.
}E_ARM_SD_OD_SHIFT;

// offset: 0x00F4, ARM_UART_OD register
typedef enum
{
	ARM_UART0_TXD_GPIOA0_OD_SHIFT		= 0,	// Bit[0]: UART0_TXD Pin(GPIOA[0]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART0_RXD_GPIOA1_OD_SHIFT		= 1,	// Bit[1]: UART0_RXD Pin(GPIOA[1]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART0_CTSN_GPIOA2_OD_SHIFT		= 2,	// Bit[2]: UART0_CTSN Pin(GPIOA[2]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART0_RTSN_GPIOA3_OD_SHIFT		= 3,	// Bit[3]: UART0_RTSN Pin(GPIOA[3]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART1_TXD_GPIOA4_OD_SHIFT		= 4,	// Bit[4]: UART1_TXD Pin(GPIOA[4]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART1_RXD_GPIOA5_OD_SHIFT		= 5,	// Bit[5]: UART1_RXD Pin(GPIOA[5]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART1_CTSN_GPIOA6_OD_SHIFT		= 6,	// Bit[6]: UART1_CTSN Pin(GPIOA[6]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART1_RTSN_GPIOA7_OD_SHIFT		= 7,	// Bit[7]: UART1_RTSN Pin(GPIOA[7]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART2_TXD_GPIOA8_OD_SHIFT		= 8,	// Bit[8]: UART2_TXD Pin(GPIOA[8]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART2_RXD_GPIOA9_OD_SHIFT		= 9,	// Bit[9]: UART2_RXD Pin(GPIOA[9]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART2_CTSN_GPIOA10_OD_SHIFT		= 10,	// Bit[10]: UART2_CTSN Pin(GPIOA[10]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART2_RTSN_GPIOA11_OD_SHIFT		= 11,	// Bit[11]: UART2_RTSN Pin(GPIOA[11]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_TXD_GPIOA12_OD_SHIFT		= 12,	// Bit[12]: UART3_TXD Pin(GPIOA[12]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_RXD_GPIOA13_OD_SHIFT		= 13,	// Bit[13]: UART3_RXD Pin(GPIOA[13]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_CTSN_GPIOA14_OD_SHIFT		= 14,	// Bit[14]: UART3_CTSN Pin(GPIOA[14]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_RTSN_GPIOA15_OD_SHIFT		= 15,	// Bit[15]: UART3_RTSN Pin(GPIOA[15]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_DCDN_GPIOA16_OD_SHIFT		= 16,	// Bit[16]: UART3_DCDN Pin(GPIOA[16]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_DSR_GPIOA17_OD_SHIFT		= 17,	// Bit[17]: UART3_DSR Pin(GPIOA[17]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_DTR_GPIOA18_OD_SHIFT		= 18,	// Bit[18]: UART3_DTR Pin(GPIOA[18]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_UART3_RIN_GPIOA19_OD_SHIFT		= 19,	// Bit[19]: UART3_RIN Pin(GPIOA[19]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain
	
	ARM_UART_REVD_OD_SHIFT				= 20	// Bit[31:20]: Reserved.
}E_ARM_UART_OD_SHIFT;

// offset: 0x00F8, ARM_SPI_I2C_OD register
typedef enum
{
	ARM_SPI0_CLK_GPIOB0_OD_SHIFT		= 0,	// Bit[0]: SPI0_CLK Pin(GPIOB[0]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI0_CSN0_GPIOB1_OD_SHIFT		= 1,	// Bit[1]: SPI0_CSN0 Pin(GPIOB[1]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI0_CSN1_GPIOB2_OD_SHIFT		= 2,	// Bit[2]: SPI0_CSN1 Pin(GPIOB[2]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI0_DIO0_GPIOB3_OD_SHIFT		= 3,	// Bit[3]: SPI0_DIO0 Pin(GPIOB[3]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI0_DIO1_GPIOB4_OD_SHIFT		= 4,	// Bit[4]: SPI0_DIO1 Pin(GPIOB[4]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI0_DIO2_GPIOB5_OD_SHIFT		= 5,	// Bit[5]: SPI0_DIO2 Pin(GPIOB[5]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI0_DIO3_GPIOB6_OD_SHIFT		= 6,	// Bit[6]: SPI0_DIO3 Pin(GPIOB[6]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI1_CLK_GPIOB7_OD_SHIFT		= 7,	// Bit[7]: SPI1_CLK Pin(GPIOB[7]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI1_CSN0_GPIOB8_OD_SHIFT		= 8,	// Bit[8]: SPI1_CSN0 Pin(GPIOB[8]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI1_CSN1_GPIOB9_OD_SHIFT		= 9,	// Bit[9]: SPI1_CSN1 Pin(GPIOB[9]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI1_DIO0_GPIOB10_OD_SHIFT		= 10,	// Bit[10]: SPI1_DIO0 Pin(GPIOB[10]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI1_DIO1_GPIOB11_OD_SHIFT		= 11,	// Bit[11]: SPI1_DIO1 Pin(GPIOB[11]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI2_CLK_GPIOB12_OD_SHIFT		= 12,	// Bit[12]: SPI2_CLK Pin(GPIOB[12]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI2_CSN0_GPIOB13_OD_SHIFT		= 13,	// Bit[13]: SPI2_CSN0 Pin(GPIOB[13]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI2_CSN1_GPIOB14_OD_SHIFT		= 14,	// Bit[14]: SPI2_CSN1 Pin(GPIOB[14]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI2_DIO0_GPIOB15_OD_SHIFT		= 15,	// Bit[15]: SPI2_DIO0 Pin(GPIOB[15]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI2_DIO1_GPIOB16_OD_SHIFT		= 16,	// Bit[16]: SPI2_DIO1 Pin(GPIOB[16]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_SPI_I2C_REVD0_OD_SHIFT			= 17,	// Bit[19:17]: reserved.

	ARM_I2C0_SDA_GPIOA20_OD_SHIFT		= 20,	// Bit[20]: I2C0_SDA Pin(GPIOA[20]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_I2C0_SCL_GPIOA21_OD_SHIFT		= 21,	// Bit[21]: I2C0_SCL Pin(GPIOA[21]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_I2C1_SDA_GPIOA22_OD_SHIFT		= 22,	// Bit[22]: I2C1_SDA Pin(GPIOA[22]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_I2C1_SCL_GPIOA23_OD_SHIFT		= 23,	// Bit[23]: I2C1_SCL Pin(GPIOA[23]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_I2C2_SDA_GPIOA24_OD_SHIFT		= 24,	// Bit[24]: I2C2_SDA Pin(GPIOA[24]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain

	ARM_I2C2_SCL_GPIOA25_OD_SHIFT		= 25,	// Bit[25]: I2C2_SCL Pin(GPIOA[25]) open drain select signal. Default: 0
												// 0: normal
												// 1: open drain
	
	ARM_SPI_I2C_REVD1_OD_SHIFT			= 26	// Bit[31:26]: Reserved
}E_ARM_SPI_I2C_OD_SHIFT;

// offset: 0x00FC, ARM_OD(GPIOB,TIMER,SYSTEM) register
typedef enum
{
	ARM_GPIOB17_OD_SHIFT						= 0,	// Bit[0]: GPIOB17 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB18_OD_SHIFT						= 1,	// Bit[1]: GPIOB18 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB19_OD_SHIFT						= 2,	// Bit[2]: GPIOB19 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB20_OD_SHIFT						= 3,	// Bit[3]: GPIOB20 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB21_OD_SHIFT						= 4,	// Bit[4]: GPIOB21 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB22_OD_SHIFT						= 5,	// Bit[5]: GPIOB22 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB23_OD_SHIFT						= 6,	// Bit[6]: GPIOB23 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB24_OD_SHIFT						= 7,	// Bit[7]: GPIOB24 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB25_OD_SHIFT						= 8,	// Bit[8]: GPIOB25 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB26_OD_SHIFT						= 9,	// Bit[9]: GPIOB26 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB27_OD_SHIFT						= 10,	// Bit[10]: GPIOB27 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB28_OD_SHIFT						= 11,	// Bit[11]: GPIOB28 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB29_OD_SHIFT						= 12,	// Bit[12]: GPIOB29 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB30_OD_SHIFT						= 13,	// Bit[13]: GPIOB30 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_GPIOB31_OD_SHIFT						= 14,	// Bit[14]: GPIOB31 Pin open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_REVD0_OD_SHIFT							= 15,	// Bit[17:15]: Reserved.
	
	ARM_TIMER_W_OUT_GPIOD0_OD_SHIFT				= 18,	// Bit[18]: TIMER_W_OUT Pin(GPIOD[0]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_TIMER_X_OUT_GPIOD1_OD_SHIFT				= 19,	// Bit[19]: TIMER_X_OUT Pin(GPIOD[1]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_TIMER_Y_OUT_GPIOD2_OD_SHIFT				= 20,	// Bit[20]: TIMER_Y_OUT Pin(GPIOD[2]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_TIMER_Z_OUT_GPIOD3_OD_SHIFT				= 21,	// Bit[21]: TIMER_Z_OUT Pin(GPIOD[3]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_RTC_CLKOUT_GPIOD31_OD_SHIFT				= 22,	// Bit[22]: RTC_CLKOUT Pin(GPIOD[31]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_RTC_CAL_CLKOUT_GPIOD29_OD_SHIFT			= 23,	// Bit[23]: RTC_CAL_CLKOUT Pin(GPIOD[29]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_CLKOUT_GPIOD30_OD_SHIFT					= 24,	// Bit[24]: CLKOUT Pin(GPIOD[30]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_HOST_LOW_POWER_OUT_GPIOA29_OD_SHIFT		= 25,	// Bit[25]: HOST_LOW_POWER_OUT Pin(GPIOA[29]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	EPF_FAILSAFE_TIMER_OUT_GPIOA30_OD_SHIFT		= 26,	// Bit[26]: EPF_FAILSAFE_TIMER_OUT Pin(GPIOA[30]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain

	ARM_EPF_SLEEP_MODE_DONE_GPIOA31_OD_SHIFT	= 27,	// Bit[27]: ARM_EPF_SLEEP_MODE_DONE Pin(GPIOA[31]) open drain select signal. Default: 0
														// 0: normal
														// 1: open drain
	
	ARM_REVD1_OD_SHIFT							= 28	// Bit[31:28]: Reserved.
}E_ARM_OD_SHIFT;

// ARM IO Output Disable Register
// (The registers are only valid in ARM sleep mode)

// offset: 0x0100, SD_OUTDIS register
typedef enum
{
	// Bit[0]: SDIO0_CLK Pin(GPIOC[0]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[1]: SDIO0_CMD Pin(GPIOC[1]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[2]: SDIO0_DATA0 Pin(GPIOC[2]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[3]: SDIO0_DATA1 Pin(GPIOC[3]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[4]: SDIO0_DATA2 Pin(GPIOC[4]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[5]: SDIO0_DATA3 Pin(GPIOC[5]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[6]: SDIO0_DATA4 Pin(GPIOC[6]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[7]: SDIO0_DATA5 Pin(GPIOC[7]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[8]: SDIO0_DATA6 Pin(GPIOC[8]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[9]: SDIO0_DATA7 Pin(GPIOC[9]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[10]: SDIO0_CD Pin(GPIOC[25]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[11]: SDIO0_WP Pin(GPIOC[26]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[12]: SDIO1_CLK Pin(GPIOC[10]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[13]: SDIO1_CMD Pin(GPIOC[11]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[14]: SDIO1_DATA0 Pin(GPIOC[12]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[15]: SDIO1_DATA1 Pin(GPIOC[13]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[16]: SDIO1_DATA2 Pin(GPIOC[14]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[17]: SDIO1_DATA3 Pin(GPIOC15) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[18]: SDIO1_DATA4 Pin(GPIOC[16]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[19]: SDIO1_DATA5 Pin(GPIOC[17]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[20]: SDIO1_DATA6 Pin(GPIOC[18]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[21]: SDIO1_DATA7 Pin(GPIOC[19]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[22]: SDIO1_CD Pin(GPIOC[27]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[23]: SDIO1_WP Pin(GPIOC[28]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[24]: SDIO0_RSTN Pin(GPIOC[29]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[25]: SDIO1_RSTN Pin(GPIOC[30]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	ARM_SD_REVD_OUTDIS_SHIFT					= 26	// Bit[31:26]: reserved.
}E_ARM_SD_OUTDIS_SHIFT;

// offset: 0x0104, ARM_UART_OUTDIS register
typedef enum
{
	// Bit[0]: UART0_TXD Pin(GPIOA[0]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[1]: UART0_RXD Pin(GPIOA[1]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[2]: UART0_CTSN Pin(GPIOA[2]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[3]: UART0_RTSN Pin(GPIOA[3]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[4]: UART1_TXD Pin(GPIOA[4]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[5]: UART1_RXD Pin(GPIOA[5]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[6]: UART1_CTSN Pin(GPIOA[6]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[7]: UART1_RTSN Pin(GPIOA[7]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[8]: UART2_TXD Pin(GPIOA[8]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[9]: UART2_RXD Pin(GPIOA[9]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[10]: UART2_CTSN Pin(GPIOA[10]) pull disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[11]: UART2_RTSN Pin(GPIOA[11]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[12]: UART3_TXD Pin(GPIOA[12]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[13]: UART3_RXD Pin(GPIOA[13]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[14]: UART3_CTSN Pin(GPIOA[14]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[15]: UART3_RTSN Pin(GPIOA[15]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[16]: UART3_DCDN Pin(GPIOA[16]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[17]: UART3_DSR Pin(GPIOA[17]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[18]: UART3_DTR Pin(GPIOA[18]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[19]: UART3_RIN Pin(GPIOA[19]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	ARM_UART_REVD_OUTDIS_SHIFT					= 20	// Bit[31:20]: Reserved.
}E_ARM_UART_OUTDIS_SHIFT;

// offset: 0x0108, ARM_SPI_I2C_OUTDIS register
typedef enum
{
	// Bit[0]: SPI0_CLK Pin(GPIOB[0]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[1]: SPI0_CSN0 Pin(GPIOB[1]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[2]: SPI0_CSN1 Pin(GPIOB[2]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[3]: SPI0_DIO0 Pin(GPIOB[3]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[4]: SPI0_DIO1 Pin(GPIOB[4]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[5]: SPI0_DIO2 Pin(GPIOB[5]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[6]: SPI0_DIO3 Pin(GPIOB[6]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[7]: SPI1_CLK Pin(GPIOB[7]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[8]: SPI1_CSN0 Pin(GPIOB[8]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[9]: SPI1_CSN1 Pin(GPIOB[9]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[10]: SPI1_DIO0 Pin(GPIOB[10]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[11]: SPI1_DIO1 Pin(GPIOB[11]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[12]: SPI2_CLK Pin(GPIOB[12]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[13]: SPI2_CSN0 Pin(GPIOB[13]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[14]: SPI2_CSN1 Pin(GPIOB[14]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[15]: SPI2_DIO0 Pin(GPIOB[15]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[16]: SPI2_DIO1 Pin(GPIOB[16]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[19:17]: reserved.
	
	// Bit[20]: I2C0_SDA Pin(GPIOA[20]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[21]: I2C0_SCL Pin(GPIOA[21]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[22]: I2C1_SDA Pin(GPIOA[22]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[23]: I2C1_SCL Pin(GPIOA[23]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[24]: I2C2_SDA Pin(GPIOA[24]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[25]: I2C2_SCL(GPIOA[25]) Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	ARM_SPI_I2C_REVD_OUTDIS_SHIFT					= 26	// Bit[31:26]: Reserved
}E_ARM_SPI_I2C_OUTDIS_SHIFT;

// offset: 0x010C, ARM_OUTDIS(GPIOB,TIMER,SYSTEM) register
typedef enum
{
	// Bit[0]: GPIOB17 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[1]: GPIOB18 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[2]: GPIOB19 Pin pull disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[3]: GPIOB20 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[4]: GPIOB21 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[5]: GPIOB22 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[6]: GPIOB23 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[7]: GPIOB24 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[8]: GPIOB25 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[9]: GPIOB26 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[10]: GPIOB27 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[11]: GPIOB28 Pin output disable  signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[12]: GPIOB29 Pin output disable  signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[13]: GPIOB30 Pin output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[14]: GPIOB31 Pin output disable  signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[17:15]: Reserved.
	
	// Bit[18]: TIMER_W_OUT Pin(GPIOD[0]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[19]: TIMER_X_OUT Pin(GPIOD[1]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[20]: TIMER_Y_OUT Pin(GPIOD[2]) output disable  signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[21]: TIMER_Z_OUT Pin(GPIOD[3]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[22]: RTC_CLKOUT Pin(GPIOD[31]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[23]: RTC_CAL_CLKOUT Pin(GPIOD[29]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	// Bit[24]: CLKOUT Pin(GPIOD[30]) output disable signal. Default: 1
	// 0: output enable, IO will be output 0 or 1 in ARM sleep mode
	// 1: output is disabled, IO will high-z in ARM sleep mode
	
	ARM_REVD_OUTDIS_SHIFT					= 25	// Bit[31:25]: Reserved.
}E_ARM_OUTDIS_SHIFT;

// ARM IO Output Value Register
// (The registers are only valid in ARM sleep mode)

// offset: 0x0110, SD_OV register
typedef enum
{
	// Bit[0]: SDIO0_CLK Pin(GPIOC[0]) output value signal. Default: 0
	// 0: output low(0) when output enable(SD_OUTDIS[0]=0) in ARM Sleep mode
	// 1: output high(1) when output enable(SD_OUTDIS[0]=0) in ARM Sleep mode
	
	// Bit[1]: SDIO0_CMD Pin(GPIOC[1]) output value signal. Default: 0
	// 0: output low(0) when output enable(SD_OUTDIS[1]) in ARM Sleep mode
	// 1: output high(1) when output enable(SD_OUTDIS[1]) in ARM Sleep mode
	
	// Bit[2]: SDIO0_DATA0 Pin(GPIOC[2]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[3]: SDIO0_DATA1 Pin(GPIOC[3]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[4]: SDIO0_DATA2 Pin(GPIOC[4]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[5]: SDIO0_DATA3 Pin(GPIOC[5]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[6]: SDIO0_DATA4 Pin(GPIOC[6]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[7]: SDIO0_DATA5 Pin(GPIOC[7]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[8]: SDIO0_DATA6 Pin(GPIOC[8]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[9]: SDIO0_DATA7 Pin(GPIOC[9]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[10]: SDIO0_CD Pin(GPIOC[25]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[11]: SDIO0_WP Pin(GPIOC[26]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[12]: SDIO1_CLK Pin(GPIOC[10]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[13]: SDIO1_CMD Pin(GPIOC[11]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[14]: SDIO1_DATA0 Pin(GPIOC[12]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[15]: SDIO1_DATA1 Pin(GPIOC[13]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[16]: SDIO1_DATA2 Pin(GPIOC[14]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[17]: SDIO1_DATA3 Pin(GPIOC15) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[18]: SDIO1_DATA4 Pin(GPIOC[16]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[19]: SDIO1_DATA5 Pin(GPIOC[17]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[20]: SDIO1_DATA6 Pin(GPIOC[18]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[21]: SDIO1_DATA7 Pin(GPIOC[19]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[22]: SDIO1_CD Pin(GPIOC[27]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[23]: SDIO1_WP Pin(GPIOC[28]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[24]: SDIO0_RSTN Pin(GPIOC[29]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[25]: SDIO1_RSTN Pin(GPIOC[30]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	ARM_SD_REVD_OV_SHIFT					= 26	// Bit[31:26]: reserved.
}E_ARM_SD_OV_SHIFT;


// offset: 0x0114, ARM_UART_OV register
typedef enum
{
	// Bit[0]: UART0_TXD Pin(GPIOA[0]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[1]: UART0_RXD Pin(GPIOA[1]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[2]: UART0_CTSN Pin(GPIOA[2]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[3]: UART0_RTSN Pin(GPIOA[3]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[4]: UART1_TXD Pin(GPIOA[4]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[5]: UART1_RXD Pin(GPIOA[5]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[6]: UART1_CTSN Pin(GPIOA[6]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[7]: UART1_RTSN Pin(GPIOA[7]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[8]: UART2_TXD Pin(GPIOA[8]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[9]: UART2_RXD Pin(GPIOA[9]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[10]: UART2_CTSN Pin(GPIOA[10]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[11]: UART2_RTSN Pin(GPIOA[11]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[12]: UART3_TXD Pin(GPIOA[12]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[13]: UART3_RXD Pin(GPIOA[13]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[14]: UART3_CTSN Pin(GPIOA[14]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[15]: UART3_RTSN Pin(GPIOA[15]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[16]: UART3_DCDN Pin(GPIOA[16]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[17]: UART3_DSR Pin(GPIOA[17]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[18]: UART3_DTR Pin(GPIOA[18]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[19]: UART3_RIN Pin(GPIOA[19]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	ARM_UART_REVD_OV_SHIFT					= 20	// Bit[31:20]: Reserved.
}E_ARM_UART_OV_SHIFT;


// offset: 0x0118, ARM_SPI_I2C_OV register
typedef enum
{	
	// Bit[0]: SPI0_CLK Pin(GPIOB[0]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[1]: SPI0_CSN0 Pin(GPIOB[1]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[2]: SPI0_CSN1 Pin(GPIOB[2]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[3]: SPI0_DIO0 Pin(GPIOB[3]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[4]: SPI0_DIO1 Pin(GPIOB[4]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[5]: SPI0_DIO2 Pin(GPIOB[5]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[6]: SPI0_DIO3 Pin(GPIOB[6]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[7]: SPI1_CLK Pin(GPIOB[7]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[8]: SPI1_CSN0 Pin(GPIOB[8]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[9]: SPI1_CSN1 Pin(GPIOB[9]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[10]: SPI1_DIO0 Pin(GPIOB[10]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[11]: SPI1_DIO1 Pin(GPIOB[11]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[12]: SPI2_CLK Pin(GPIOB[12]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[13]: SPI2_CSN0 Pin(GPIOB[13]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[14]: SPI2_CSN1 Pin(GPIOB[14]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[15]: SPI2_DIO0 Pin(GPIOB[15]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[16]: SPI2_DIO1 Pin(GPIOB[16]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[19:17]: reserved.
	
	// Bit[20]: I2C0_SDA Pin(GPIOA[20]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[21]: I2C0_SCL Pin(GPIOA[21]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[22]: I2C1_SDA Pin(GPIOA[22]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[23]: I2C1_SCL Pin(GPIOA[23]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[24]: I2C2_SDA Pin(GPIOA[24]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[25]: I2C2_SCL(GPIOA[25]) Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	ARM_SPI_I2C_REVD_OV_SHIFT					= 26	// Bit[31:26]: Reserved
}E_ARM_SPI_I2C_OV_SHIFT;

// offset: 0x011C, ARM_OV(GPIOB,TIMER,SYSTEM) register
typedef enum
{
	// Bit[0]: GPIOB17 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[1]: GPIOB18 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[2]: GPIOB19 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[3]: GPIOB20 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[4]: GPIOB21 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[5]: GPIOB22 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[6]: GPIOB23 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[7]: GPIOB24 Pin pull disable signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[8]: GPIOB25 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[9]: GPIOB26 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[10]: GPIOB27 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[11]: GPIOB28 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[12]: GPIOB29 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[13]: GPIOB30 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[14]: GPIOB31 Pin output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[17:15]: Reserved.
	
	// Bit[18]: TIMER_W_OUT Pin(GPIOD[0]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[19]: TIMER_X_OUT Pin(GPIOD[1]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[20]: TIMER_Y_OUT Pin(GPIOD[2]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[21]: TIMER_Z_OUT Pin(GPIOD[3]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[22]: RTC_CLKOUT Pin(GPIOD[31]) output signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[23]: RTC_CAL_CLKOUT Pin(GPIOD[29]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	// Bit[24]: CLKOUT Pin(GPIOD[30]) output value signal. Default: 0
	// 0: output low(0) when output enable in ARM Sleep mode
	// 1: output high(1) when output enable in ARM Sleep mode
	
	ARM_REVD_OV_SHIFT					= 25	// Bit[31:25]: Reserved.
}E_ARM_OV_SHIFT;

// ARM WDT IO pinmux Register
// offset: 0x01B0, ARM_WDT_RSTN_IODIS register
typedef enum
{
	ARM_WDT_RSTN_GPIPB17_SHIFT		= 0,	// Bit[0]: WDT_RSTN_IODIS select signal. 
											// Default: 1 
											// When "wdt_rstn_iodis" is 
											// 0: WDT_RSTN 
											// 1: GPIOB[17] 
	
	ARM_WDT_RSTN_IODIS_REVD_SHIFT	= 1		// Bit[31:1]: Reserved
}E_ARM_WDT_RSTN_IODIS_SHIFT;

// wdt reset pin mask and value
#define ARM_PINMUX_WDT_RSTN_MASK	(1<<ARM_WDT_RSTN_GPIPB17_SHIFT)
#define ARM_PINMUX_WDT_RSTN_VAL		(0<<ARM_WDT_RSTN_GPIPB17_SHIFT)

// gpiob17 pin mask and value
#define ARM_PINMUX_GPIOB_PIN17_MASK	(1<<ARM_WDT_RSTN_GPIPB17_SHIFT)
#define ARM_PINMUX_GPIOB_PIN17_VAL	(1<<ARM_WDT_RSTN_GPIPB17_SHIFT)



/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
// Define PRCM Controller structure 
struct brite_prcm {
	void *regs;
};

// Define MISC Controller structure 
struct brite_misc {
	void *regs;
};

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern struct brite_prcm *p_prcm;
extern struct brite_misc *p_misc;

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
void mem_cpy8(void *p_dst, const void *p_src, uint32_t size);
void mem_cpy32(void *p_dst, const void *p_src, uint32_t size);
void mem_set8(void *p_dst, uint8_t val, uint32_t size);
void mem_set32(void *p_dst, uint32_t val, uint32_t size);
int mem_cmp8(void *p_dst, void *p_src, uint32_t size);
uint32_t mem_cmp32(void *p_dst, void *p_src, uint32_t size);
void mem_set_seq(uint8_t *p_data, uint32_t size);
void mem_printf(uint32_t *p_data, uint32_t size);
void mem_print_line(uint8_t *p_data, uint32_t size);

uint8_t power(uint32_t size);
void sw_udelay_10x(u32 cnt);

int sw_mutex_lock(int timeout_ms);
int sw_mutex_unlock(void);

void prcm_var_init(uint32_t base);
void misc_var_init(uint32_t base);

// prcm 
void prcm_reset(E_ARM_IP_RST ip);
void prcm_blk_cg(E_ARM_BLK_CG blk, E_ARM_BLK_CG_CTRL ctrl);
uint32_t prcm_pwd_read(void);
void prcm_pwd_hs_keeping_ctrl(E_ARM_HS_KEEPING_DONE_ST st);
void prcm_fail_safe_timer_init(uint32_t ms);
uint32_t prcm_pmu_int_st_read(void);
void prcm_pmu_int_st_clr(void);

// misc
bool misc_uart_pin_init(E_UART_PORT port);
bool misc_sdio_pin_init(E_SDIO_PORT port, E_SDIO_BW bw);
bool misc_spi_pin_init(E_SPI_PORT port);
bool misc_i2c_pin_init(E_I2C_PORT port);
bool misc_gpio_pin_init(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PU pu, E_IO_PD pd, E_IO_OD od);
bool misc_timer_input_pin_init(E_TIMER_PORT port, E_IO_PU pu, E_IO_PD pd);
bool misc_timer_output_pin_init(E_TIMER_WXYZ_PORT port, E_TIMER_PORT timer, E_IO_PU pu, E_IO_PD pd);
void misc_rtc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd);
void misc_rtc_cal_clkout_pin_init(E_IO_PU pu, E_IO_PD pd);
void misc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd);
void misc_host_low_power_out_pin_init(E_IO_PU pu, E_IO_PD pd);
void misc_epf_failsafe_timer_out_pin_init(E_IO_PU pu, E_IO_PD pd);
void misc_sleep_mode_done_pin_init(E_IO_PU pu, E_IO_PD pd);
void misc_usb_oc_pin_init(E_USB_PORT port, E_IO_PU pu, E_IO_PD pd);
bool misc_uart_ds(E_UART_PORT port, E_IO_DS_MA ds);
bool misc_i2c_ds(E_I2C_PORT port, E_IO_DS_MA ds);

#ifdef TAPEOUT_V2
bool misc_enable_dma_err_resp(int dmac_index, int timeout_val);
bool misc_disable_dma_err_resp(int dmac_index);
void misc_trng_sel(int trng_index);
#endif

bool misc_dma_ext_int_src_sel(E_DMA_PORT dma_port, E_ARM_DMA_HS_EXT_INT_SRC ext_src);

void misc_wdt_rstn_pin_init(void);

#endif

