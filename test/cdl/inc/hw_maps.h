/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __HW_MAPS__
#define __HW_MAPS__

#define ROM_BASE	0x0
#define SRAM_BASE	0x10000000
//#define SRAM_SIZE	0x20000
#define SRAM_SIZE	0x10000 //reserve 64K 


// ARM AHB Memory Map(fixed)
#define QSPI_AHB_BASE	0xC0000000
#define USB0_BASE		0xC1000000
#define USB1_BASE		0xC1100000
#define DMAC0_BASE		0xC1200000
#define DMAC1_BASE		0xC1300000
#define DMAC2_BASE		0xC1400000
#define NFC_BASE		0xC1500000
#define SD0_BASE		0xC1600000
#define SD1_BASE		0xC1700000
#define AES_BASE		0xC1800000
#define SHA_BASE		0xC1900000
#define ECC_BASE		0xC1A00000
#define OTP_BASE		0xC1B00000
#define DDR_CTRL_BASE	0xC1C00000
#define DDR_PHY_BASE	(DDR_CTRL_BASE + 0x400)
#define DDR_BASE		0x40000000
#define H2P_BASE		0xC1D00000
#define CRC_BASE		0xC1E00000
#define H2X0_BASE		0x10000000
#define H2X1_BASE		0x40000000
#define COM_BUS_BASE	0xC2000000

// ARM APB0 Memory Map(fixed)
#define UART0_BASE		0xC1D00000
#define UART1_BASE		0xC1D01000
#define UART2_BASE		0xC1D02000
#define UART3_BASE		0xC1D03000
#define SPI0_BASE		0xC1D04000 // SPI0-->QSPI
#define SPI1_BASE		0xC1D05000
#define SPI2_BASE		0xC1D06000
#define I2C0_BASE		0xC1D07000
#define I2C1_BASE		0xC1D08000
#define I2C2_BASE		0xC1D09000
#define ADC_BASE		0xC1D0A000

// ARM APB1 Memory Map(fixed)
#define GPIO_BASE		0x20000000
#define TIMER_BASE		0x20001000
#define WDT_BASE		0x20002000
#define RNG_BASE		0x20003000
#define DWC_TRNG_BASE	0x20006000
#define MISC_BASE		0x20004000
#define PRCM_BASE		0x20005000
#define SYS_DEBUG_BASE	0x20100000

// BBU
#define TAMPER_BASE		0xC2000000
#define RTC_BASE		0xC2100000
#define BBU_RAM_BASE	0xC2200000

// IPC
#define A7_IPC_REG_BASE	0xC2300000
#define A7_IPC_RAM_BASE	0xC2600000

#define GIC_BASE	0x21001000
#define GIC_CPU_BASE	0x21002000

#endif

