/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _QSPI_REG_H_
#define _QSPI_REG_H_
#include "cdl_config.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define QSPI_CTRL_BASE_ADDR SPI0_BASE
#define QSPI_BASE_ADDR		QSPI_AHB_BASE


// register layout 
#define QSPI_CFG_REG					((QSPI_CTRL_BASE_ADDR + 0x00) >> 0) // 0x00, R/W, QSPI Configuration Register
#define QSPI_DEVRDINST_REG				((QSPI_CTRL_BASE_ADDR + 0x04) >> 0) // 0x04, R/W, Device Read Instruction Register
#define QSPI_DEVWRINST_REG				((QSPI_CTRL_BASE_ADDR + 0x08) >> 0) // 0x08, R/W, Device Write Instruction Register 
#define QSPI_DEVDLY_REG					((QSPI_CTRL_BASE_ADDR + 0x0C) >> 0) // 0x0C, R/W, QSPI Device Delay Register
#define QSPI_RDDATACAP_REG				((QSPI_CTRL_BASE_ADDR + 0x10) >> 0) // 0x10, R/W, Read Data Capture Register 
#define QSPI_DEVSIZE_REG				((QSPI_CTRL_BASE_ADDR + 0x14) >> 0) // 0x14, R/W, Device Size Register 
#define QSPI_SRAMPART_REG				((QSPI_CTRL_BASE_ADDR + 0x18) >> 0) // 0x18, R/W, SRAM Partition Register  
#define QSPI_INDRECTAHBADDRTRI_REG		((QSPI_CTRL_BASE_ADDR + 0x1C) >> 0) // 0x1C, R/W, Indirect AHB Address Trigger Register 
#define QSPI_DMAPERIPH_REG				((QSPI_CTRL_BASE_ADDR + 0x20) >> 0) // 0x20, R/W, DMA Peripheral Register 
#define QSPI_REMAPADDR_REG				((QSPI_CTRL_BASE_ADDR + 0x24) >> 0) // 0x24, R/W, Remap Address Register 
#define QSPI_MODEBIT_REG				((QSPI_CTRL_BASE_ADDR + 0x28) >> 0) // 0x28, R/W, Mode Bit Register
#define QSPI_SRAMFILLLEVEL_REG			((QSPI_CTRL_BASE_ADDR + 0x2C) >> 0) // 0x2C, RO,   SRAM Fill Level Register  
#define QSPI_TXTHRESH_REG				((QSPI_CTRL_BASE_ADDR + 0x30) >> 0) // 0x30, R/W, TX Threshold Register
#define QSPI_RXTHRESH_REG				((QSPI_CTRL_BASE_ADDR + 0x34) >> 0) // 0x34, R/W, RX Threshold Register 
#define QSPI_WRCOMPCTRL_REG				((QSPI_CTRL_BASE_ADDR + 0x38) >> 0) // 0x38, R/W, Write Completion Control Register 
#define QSPI_POLLINGEXP_REG				((QSPI_CTRL_BASE_ADDR + 0x3C) >> 0) // 0x3C, R/W, Polling Expiration Register 
#define QSPI_INTST_REG					((QSPI_CTRL_BASE_ADDR + 0x40) >> 0) // 0x40, R/W, Interrupt Status Register
#define QSPI_INTMASK_REG				((QSPI_CTRL_BASE_ADDR + 0x44) >> 0) // 0x44, R/W, Interrupt Mask  
#define QSPI_REVD0_REG					((QSPI_CTRL_BASE_ADDR + 0x48) >> 0) // 0x48~0x4F, reserved,
#define QSPI_LOWRPROTECT_REG			((QSPI_CTRL_BASE_ADDR + 0x50) >> 0) // 0x50, R/W, Lower Write Protection Register 
#define QSPI_UPWRPROTECT_REG			((QSPI_CTRL_BASE_ADDR + 0x54) >> 0) // 0x54, R/W, Upper Write Protection Register  
#define QSPI_WRPROTECT_REG				((QSPI_CTRL_BASE_ADDR + 0x58) >> 0) // 0x58, R/W, Write Protection Register 
#define QSPI_REVD1_REG					((QSPI_CTRL_BASE_ADDR + 0x5C) >> 0) // 0x5C,         reserved
#define QSPI_INDIRECTRDTRANS_REG		((QSPI_CTRL_BASE_ADDR + 0x60) >> 0) // 0x60, R/W, Indirect Read Transfer Register
#define QSPI_INDIRRDTRANSWM_REG			((QSPI_CTRL_BASE_ADDR + 0x64) >> 0) // 0x64, R/W, Indirect Read Transfer Watermark Register
#define QSPI_INDIRRDTRANSSTADDR_REG		((QSPI_CTRL_BASE_ADDR + 0x68) >> 0) // 0x68, R/W, Indirect Read Transfer Start Address Register
#define QSPI_INDIRRDTRANSNUMBYTES_REG	((QSPI_CTRL_BASE_ADDR + 0x6C) >> 0) // 0x6C, R/W, Indirect Read Transfer Number Bytes Register 
#define QSPI_INDIRWRTRANS_REG			((QSPI_CTRL_BASE_ADDR + 0x70) >> 0) // 0x70, R/W, Indirect Write Transfer Register 
#define QSPI_INDIRWRTRANSWM_REG			((QSPI_CTRL_BASE_ADDR + 0x74) >> 0) // 0x74, R/W, Indirect Write Transfer Watermark Register 
#define QSPI_INDIRWRTRANSSTADDR_REG		((QSPI_CTRL_BASE_ADDR + 0x78) >> 0) // 0x78, R/W, Indirect Write Transfer Start Address Register
#define QSPI_INDIRWRTRANSCNT_REG		((QSPI_CTRL_BASE_ADDR + 0x7C) >> 0) // 0x7C, R/W, Indirect Write Transfer Count Register
#define QSPI_INDIRAHBTRIGADDRRANGE_REG	((QSPI_CTRL_BASE_ADDR + 0x80) >> 0) // 0x80, R/W, Indirect AHB Trigger Address Range Register 
#define QSPI_REVD2_REG					((QSPI_CTRL_BASE_ADDR + 0x84) >> 0) // 0x84~0x8F, reserved
#define QSPI_FLASHCMD_REG				((QSPI_CTRL_BASE_ADDR + 0x90) >> 0) // 0x90, R/W, Flash Command Register 
#define QSPI_FLASHCMDADDR_REG			((QSPI_CTRL_BASE_ADDR + 0x94) >> 0) // 0x94, R/W, Flash Command Address Registers 
#define QSPI_REVD3_REG					((QSPI_CTRL_BASE_ADDR + 0x98) >> 0) // 0x98~0x9F, reserved
#define QSPI_FLASHCMDRDDATALO_REG		((QSPI_CTRL_BASE_ADDR + 0xA0) >> 0) // 0xA0, RO,   Flash Command Read Data Register (Lower) 
#define QSPI_FLASHCMDRDDATAUP_REG		((QSPI_CTRL_BASE_ADDR + 0xA4) >> 0) // 0xA4, RO,   Flash Command Read Data Register (Upper) 
#define QSPI_FLASHCMDWRDATALO_REG		((QSPI_CTRL_BASE_ADDR + 0xA8) >> 0) // 0xA8, R/W, Flash Command Write Data Register (Lower) 
#define QSPI_FLASHCMDWRDATAUP_REG		((QSPI_CTRL_BASE_ADDR + 0xAC) >> 0) // 0xAC, R/W, Flash Command Write Data Register (Upper)  
#define QSPI_FLASHST_REG				((QSPI_CTRL_BASE_ADDR + 0xB0) >> 0) // 0xB0, RO,   Flash Status Register 
#define QSPI_REVD4_REG					((QSPI_CTRL_BASE_ADDR + 0xB4) >> 0) // 0xB4~0xFB, reserved
#define QSPI_MODULEID_REG				((QSPI_CTRL_BASE_ADDR + 0xFC) >> 0) // 0xFC, RO,   Module ID Register

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

// offset: 0x0000, QSPI Configuration Register
typedef enum
{
	QSPI_CFG_EN_SHIFT  			= 0,	// R/W, QSPI Enable
										//         	0 = disable the Multiple-SPI interface once current transfer of the data word is completed.
										//		1 = enable the Multiple-SPI interface
										//		When set to low, all output enables are inactive and all pins are
										// 		set to input mode. This bit is synchronized in hardware.
	
	QSPI_CFG_POLARITY_SHIFT 	= 1,	// R/W, Clock polarity outside SPI word - This maps to the standard SPI CPOL transfer format.
										// 		Keep this bit low when operating in DDR Mode or DDR
										// 		Protocol to ensure the falling edge of SPI clock as thelast one.
	
	QSPI_CFG_PHASE_SHIFT 		= 2,	// R/W, Clock phase - This maps to the standard SPI CPHA transfer format.
										//		Keep this bit low when operating in DDR Mode or DDR
										//		Protocol to ensure the falling edge of SPI clock as the last one.
	
	QSPI_CFG_REVD_SHIFT			= 3,	// RO,   Unused bits. Read as zero
	
	QSPI_CFG_DIRECT_EN_SHIFT  	= 7,	// R/W, Enable Direct Access Controller
										//	     	0 = disable the Direct Access Controller once current transfer of the data word is completed.
										//		1 = enable the Direct Access Controller
										//		When the Direct Access Controller and Indirect Access Controller are both disabled, all AHB requests are completed
										//		with an error response.
										//		This bit is synchronized in hardware.

	QSPI_CFG_LEGACY_EN_SHIFT  	= 8,	// R/W, Legacy IP Mode Enable
										//	     0 = Use Direct Access Controller/Indirect Access Controller or STIG interface for data transfers
										//		1 = When set to "1", legacy mode is enabled. In this mode, any
										//		write to the controller via the AHB interface is serialized and
										//		sent to the FLASH device. Any valid AHB read will pop the
										//		internal RX-FIFO, retrieving data that was forwarded by the
										//		external FLASH device on the SPI lines. 4, 2 or 1 byte transfers
										//		are permitted and controlled via the HSIZE input.
										//		This bit is synchronized in hardware.

	
	QSPI_CFG_PER_SEL_DEC_SHIFT  = 9,	// R/W, Peripheral select decode
										//		0 = only 1 of 4 selects n_ss_out[3:0] is active
										//		1 = allow external 4-to-16 decode (n_ss_out=ss)

	
	QSPI_CFG_PER_SEL_CHIP_SHIFT  = 10,	// R/W, Peripheral chip select lines
										//		If bit 9 of this register = 0, ss[3:0] are output thus:
										//		ss[3:0] n_ss_out[3:0]
										//		4'bxxx0 4'b1110
										//		4'bxx01 4'b1101
										//		4'bx011 4'b1011
										//		4'b0111 4'b0111
										//		4'b1111 4'b1111 (no peripheral selected)
										//		If bit 9 of this register = 1, ss[3:0] directly drives n_ss_out[3:0]

									
	QSPI_CFG_WP_SHIFT  		= 14,		// R/W, Set to drive the Write Protect pin of the FLASH device.
										//		This is resynchronized to the generated memory clock as necessary.
										//		Note that the WP pin is only valid in SINGLE or DUAL
										//		transfer modes. During QUAD transfers, the WP pin is used for transferring data and therefore any setting of this register bit will
										//		be ignored.
										//		This bit is synchronized in hardware.

	
	QSPI_CFG_DMA_EN_SHIFT  	= 15,		// R/W, Enable DMA Peripheral Interface
										//		Set to "1" to enable the DMA handshaking logic. When enabled
										//		the QSPI will trigger DMA transfer requests via the DMA
										//		peripheral interface.
										//		Set to '0' to disable DMA Peripheral Interface.
										//		This bit is synchronized in hardware.

									
	QSPI_CFG_AHB_ADDR_REMAP_EN_SHIFT  		= 16,	// R/W, Enable AHB Address Re-mapping(Direct Access Mode Only)
													//		When set to "1", the incoming AHB address will be adapted and
													//		sent to the FLASH device as (address + N), where N is the value
													//		stored in the remap address register.
													//		Use this register after the controller has ensured the attached
													//		FLASH device has been configured to be ready to enter XIP mode.
													//		This bit is synchronized in hardware.
									
	QSPI_CFG_ENTER_XIP_NEXT_SHIFT  			= 17,	// R/W, Enter XIP Mode on next READ
													//		0 = If XIP is enabled, then setting to "0" will cause the controller
													//		to exit XIP mode after the next READ instruction has completed
													//		1 = If XIP is disabled, then setting to "1" will inform the
													//		controller that the device is ready to enter XIP on the next READ
													//		instruction. The controller will therefore send the appropriate
													//		command sequence, including mode bits to cause the device to
													//		enter XIP mode.	
									
	QSPI_CFG_ENTER_XIP_IMMEDIATELY_SHIFT  	= 18,	// R/W, Enter XIP Mode immediately
													//		0 = If XIP is enabled, then setting to "0" will cause the controller
													//		to exit XIP mode after the next READ instruction has completed.
													//		1 = Operate the device in XIP mode immediately

	QSPI_CFG_BD_DIV_SHIFT  					= 19,	// R/W, Master mode baud rate divisor (2 to 32),
													//		SPI baud rate = (master reference clock)/BD
													//		Where BD is:
													//		4'b0000 = /2
													//		4'b0001 = /4
													//		4'b0010 = /6
													//		4'b0011 = /8
													//		4'b0100 = /10
													//		4'b0101 = /12
													//		4'b0110 = /14
													//		4'b0111 = /16
													//		4'b1000 = /18
													//		...
													//		4'b1111 = /32
													//		Set this register up before enabling the QSPI controller.
	
	QSPI_CFG_AHB_DEC_EN_SHIFT				= 23, 	// R/W, Enable AHB Decoder(Direct Access Mode Only)

	QSPI_CFG_DTR_EN_SHIFT					= 24, 	// R/W, Enable DTR Protocol
													// This bit should be set if device is configured to work in DTR protocol.

	QSPI_CFG_REV1_SHIFT						= 25,	// RO,   Unused bits. Read as zero

	QSPI_CFG_ST_SHIFT						= 31	// RO,   Serial Interface and QSPI pipeline is IDLE.
													//		This is a STATUS read-only bit. Note this is a re-timed signal, so there will be some
													//		inherent delay on the generation of this status signal.
}E_QSPI_CFG_SHIFT;

// QSPI interface Enable 
typedef enum
{
	QSPI_INF_DIS = 0,
	QSPI_INF_EN	 = 1
}E_QSPI_INF_EN;

#define QSPI_INF_EN_MASK		(1<<QSPI_CFG_EN_SHIFT)
#define QSPI_INF_EN_VAL(val)	((val)<<QSPI_CFG_EN_SHIFT)

// QSPI clock polarity and phase
// bit [0], polarity
// bit [1], phase
typedef enum
{
	QSPI_PHASE_0		= 0,
	QSPI_PHASE_1		= 1,
	QSPI_PHASE_2		= 2,
	QSPI_PHASE_3		= 3,
}E_QSPI_PHASE;

#define QSPI_PHASE_MASK			(3<<QSPI_CFG_POLARITY_SHIFT)
#define QSPI_PHASE_VAL(val)		((val)<<QSPI_CFG_POLARITY_SHIFT)

// QSPI acces mode 
typedef enum
{
	QSPI_MODE_DIRECT	= 0,
	QSPI_MODE_INDIRECT	= 1,
	QSPI_MODE_LEGACY	= 2
}E_QSPI_MODE;

#define QSPI_CFG_DIRECT_EN_MASK 		(1<<QSPI_CFG_DIRECT_EN_SHIFT)
#define QSPI_CFG_LEGACY_EN_MASK 		(1<<QSPI_CFG_LEGACY_EN_SHIFT)
#define QSPI_CFG_DMA_EN_MASK 			(1<<QSPI_CFG_DMA_EN_SHIFT)
#define QSPI_CFG_AHB_ADDR_REMAP_EN_MASK (1<<QSPI_CFG_AHB_ADDR_REMAP_EN_SHIFT)

// QSPI BD div mask and value
#define QSPI_BD_DIV_MASK			(0xF<<QSPI_CFG_BD_DIV_SHIFT)
#define QSPI_BD_DIV_VAL(val)		((val)<<QSPI_CFG_BD_DIV_SHIFT)

#define QSPI_PIPLINE_IDLE_MASK (1UL<<31)

// offset: 0x000C, Device Delay Register
typedef enum
{
	DEV_DLY_CSSOT_SHIFT  	= 0,	// R/W,   CSSOT ¡§Cchip Select Start Of Transfer
									// Delay in master reference clocks between setting n_ss_out low and first bit transfer.
									// By default (CSSOT=0), chip select will be asserted half a SCLK  period before the first rising edge of sclk_out.
									// If CSSOT=N, chip select will be asserted half a sclk_out period before the first rising edge of sclk_out + N ref_clks
	
	DEV_DLY_CSEOT_SHIFT 	= 8,	// R/W, CSEOT ¡§C Chip Select End Of Transfer
									// Delay in master reference clocks between last bit of current
									// transaction and de-asserting the device chip select (n_ss_out).
									// By default (when CSEOT=0), the chip select will be de-asserted
									// on the last falling edge of sclk_out in SPI Mode 0 or on the
									// last rising edge + half spi clock cycle in SPI Mode 3 at the
									// completion of the current transaction.
									// If CSEOT=N, then chip selected will de-assert N ref_clks after
									// the last falling edge of sclk_out in SPI Mode 0 or on the last
									// rising edge + half spi clock cycle in SPI Mode 3.

	DEV_DLY_CSDADS_SHIFT 	= 16,	// R/W, CSDADS ¡§C Chip Select De-Assert Different Slaves
									// Delay in master reference clocks (ref_clk) between one chip
									// select being de-activated and the activation of another. This
									// is used to ensure a quiet period between the selection of two
									// different slaves. CSDADS is only relevant when switching
									// between 2 different external flash devices.
									// An example scenario might be if you have 2 FLASH devices
									// named A and B, each connected to the controller on a different
									// chip select. You might have a requirement to have chip select
									// inactive for a particular period of time between each successive
									// non-sequential transfer of the same device. You would control
									// this using bits CSDA. There may be a different requirement to
									// hold chip select inactive for a longer period of time between the
									// end of one transfer on device A and the start of the next transfer
									// on device B. You would control this using CSDADS.
									// The minimum delay (CSDADS=0) is:
									// 1 sclk_out + 3 ref_clk.
									// If CSDA=N, then the chip select de-assert time will be:
									// 1 sclk_out + 1 ref_clk + N ref_clks.
	
	DEV_DLY_CSDA_SHIFT		= 24	// R/W, CSDA - Chip Select De-Assert	
									// Added delay in master reference clocks (ref_clk) for the length
									// that the master mode chip select outputs are de-asserted between transactions.
									// The minimum delay for chip select to be de-asserted (CSDA=0) is:
									// 1 sclk_out + 1 ref_clk to ensure the chip select is never re-asserted within an sclk_out period.
									// If CSDA=N, then the chip select de-assert time will be:
									// 1 sclk_out + 1 ref_clk + N ref_clks.
}E_QSPI_DEV_DLY_SHIFT;

// CSSOT mask and value
#define QSPI_CSSOT_MASK			(0xFF<<DEV_DLY_CSSOT_SHIFT)
#define QSPI_CSSOT_VAL(val)		((val)<<DEV_DLY_CSSOT_SHIFT)

// CSEOT mask and value
#define QSPI_CSEOT_MASK			(0xFF<<DEV_DLY_CSEOT_SHIFT)
#define QSPI_CSEOT_VAL(val)		((val)<<DEV_DLY_CSEOT_SHIFT)

// CSDADS mask and value
#define QSPI_CSDADS_MASK		(0xFF<<DEV_DLY_CSDADS_SHIFT)
#define QSPI_CSDADS_VAL(val)	((val)<<DEV_DLY_CSDADS_SHIFT)

// CSDA mask and value
#define QSPI_CSDA_MASK			(0xFF<<DEV_DLY_CSDA_SHIFT)
#define QSPI_CSDA_VAL(val)		((val)<<DEV_DLY_CSDA_SHIFT)

// offset: 0x0010, Read Data Capture Register
typedef enum
{
	RD_DATA_BYPASS_SHIFT  			= 0,	// R/W,   Bypass of the adapted loopback clock circuit
											// 0 = enable the adapted loopback clock circuit
											// 1 = disable the adapted loopback clock circuit
	
	RD_DATA_DELAY_RD_DATA_SHIFT 	= 1,	// R/W, Delay the read data capturing logic by the programmable number of ref_clk cycles

	RD_DATA_SAMP_EDGE_SEL_SHIFT 	= 5,	// R/W, Sample edge selection (of the flash memory data outputs).
											// 0 = data outputs from flash memory are sampled on falling edge of the ref_clk
											// 1 = data outputs from flash memory are sampled on rising edge of the ref_clk
	
	RD_DATA_REVD0_SHIFT				= 6,	// RO, Unused bits. Read as zero.
	
	RD_DATA_DELAY_TRANS_DATA_SHIFT  = 16,	// R/W, Delay the transmitted data (Master Output) by the programmable number of 
											// ref_clk cycles (in order to improve the hold timing during transfers in DDR Mode).
											// Note
											// This field is only relevant when DDR Read Command is executed. Otherwise can be ignored.
											// delay_value = 1 x ref_clk + [field_value] x ref_clk
	
	RD_DATA_REVD1_SHIFT				= 20,	// RO, Unused bits. Read as zero.
}E_QSPI_RD_DATA_CAP_SHIFT;

// QSPI data bypass 
typedef enum
{
	QSPI_DATA_BYPASS_EN 	= 0,
	QSPI_DATA_BYPASS_DIS	= 1
}E_QSPI_DATA_BYPASS;

#define QSPI_DATA_BYPASS_MASK		(1<<RD_DATA_BYPASS_SHIFT)
#define QSPI_DATA_BYPASS_VAL(val)	((val)<<RD_DATA_BYPASS_SHIFT)

// read data delay
#define QSPI_READ_CAPTURE_MAX_DELAY	15

#define QSPI_DELAY_RD_DATA_MASK		(0x0F<<RD_DATA_DELAY_RD_DATA_SHIFT)
#define QSPI_DELAY_RD_DATA_VAL(val)	((val)<<RD_DATA_DELAY_RD_DATA_SHIFT)

// offset: 0x0040, Interrupt Status Register Register
#if 0
typedef enum
{
	INT_ST_MODE_FAIL_SHIFT  			= 0,	// RWC,   Mode fail
												// 		Legacy interrupt which is not being used any more.

	
	//INT_ST_MODE_FAIL_SHIFT  			= 0,	// RWC,  
									
	INDIRECT_RD_REVD1_SHIFT				= 8		// RO,   Unused bits. Read as zero
}E_QSPI_INT_ST_SHIFT;
#endif

// offset: 0x0060, Indirect Read Transfer Control Register
typedef enum
{
	INDIRECT_RD_START_SHIFT  			= 0,	// WO,   Start indirect read (control)
												//		Writing a "1" to this bit will trigger an indirect READ operatio
												//		The assumption is that the indirect start address and Indirect
												//		number of bytes register is setup before triggering the indirect 	READ operation.
												//		This bit is internally synchronized in hardware.

	
	INDIRECT_RD_CANCEL_SHIFT 			= 1,	// WO,   Cancel indirect read (control)
												//		Writing a "1" to this bit will cancel an ongoing Indirect READ
												//		operation. This will cancel all indirect operations currently in progress.
												//		This bit is internally synchronized in hardware.

	INDIRECT_RD_ONGOING_SHIFT 			= 2,	// RO,  Indirect read operation in progress (status)
												//		This bit can only be cleared by H/W.
	
	INDIRECT_RD_SRAM_FULL_SHIFT			= 3,	// R/W,   SRAM full and unable to immediately complete indirect operation (status)
	
	INDIRECT_RD_TWO_READ_QUEUED_SHIFT  	= 4,	// RO, Two indirect read operations have been queued (status)
												//	     This bit is set and cleared by H/W.
	
	INDIRECT_RD_DONE_SHIFT  			= 5,	// R/W, Indirect Completion Status (status)
												//	     Set by H/W when an indirect operation has completed, cleared by writing a "1"
	
	INDIRECT_RD_NUM_DONE_SHIFT  		= 6,	// RO, Number of indirect operations completed. 
												//		This is used in conjunction with bit 5. Incremented by H/W when an indirect
												//		operation has completed, decremented when bit 5 is written with a "1"
									
	INDIRECT_RD_REVD1_SHIFT				= 8		// RO,   Unused bits. Read as zero
}E_QSPI_INDIRECT_RD_CTRL_SHIFT;


// offset: 0x0070, Indirect Write Transfer Control Register
typedef enum
{
	INDIRECT_WR_START_SHIFT  			= 0,	// WO,   Start indirect write (control)
												//		Writing a "1" to this bit will trigger an indirect WRITE operation.
												//		The assumption is that the indirect start address and Indirect
												//		number of bytes register is setup before triggering the indirect
												//		WRITE operation. 
												//		This bit is internally synchronized in hardware.	
	
	INDIRECT_WR_CANCEL_SHIFT 			= 1,	// WO,   Cancel indirect write (control)
												//		Writing a "1" to this bit will cancel an ongoing Indirect WRITE
												//		operation. This will cancel all indirect operations currently inprogress.
												//		This bit is internally synchronized in hardware.

	
	INDIRECT_WR_ONGOING_SHIFT 			= 2,	// RO,  Indirect write operation in progress (status)
												//		This bit can only be cleared by H/W.
	
	INDIRECT_WR_REVD0_SHIFT				= 3,	// RO,   Unused bits. Read as zero
	
	INDIRECT_WR_TWO_READ_QUEUED_SHIFT  	= 4,	// RO, Two indirect read operations have been queued (status)
												//	     This bit is set and cleared by H/W.
	
	INDIRECT_WR_DONE_SHIFT  			= 5,	// R/W, Indirect Completion Status (status)
												//	     Set by H/W when an indirect operation has completed, cleared by writing a "1"
	
	INDIRECT_WR_NUM_DONE_SHIFT 			= 6,	// RO, Number of indirect operations completed. 
												//		This is used in conjunction with bit 5. Incremented by H/W when an indirect
												//		operation has completed, decremented when bit 5 is written with a "1"
									
	INDIRECT_WR_REVD1_SHIFT				= 8		// RO,   Unused bits. Read as zero
}E_QSPI_INDIRECT_WR_CTRL_SHIFT;

// offset: 0x0090, Flash Command Control Register (Using STIG)
typedef enum
{
	CMD_EXE_SHIFT  			= 0,	// WO,   Execute the command
	
	CMD_STIG_EXE_ST_SHIFT 	= 1,	// RO,   STIG command execution in progress.
	
	CMD_STIG_BANK_EN_SHIFT 	= 2,	// R/W, STIG Memory Bank enable bit.
	
	CMD_REVD_SHIFT			= 3,	// RO,   Unused bits. Read as zero
	
	CMD_DUMMY_CYCLES_SHIFT  = 7,	// R/W, Number of Dummy Cycles
									//	     Set to the number of dummy cycles required for command specified in bits 31:24.
	
	CMD_WR_BYTES_SHIFT  	= 12,	// R/W, Number of Write Data Bytes
									//	     Up to 8 Data bytes may be written using this command (Set to 0 for 1 byte, 7 for 8 bytes).
	
	CMD_WR_EN_SHIFT  		= 15,	// R/W, Write Data Enable
									//		Set to "1" if the command specified in bits 31:24 requires write data bytes to be sent to the device.
									
	CMD_ADDR_BYTES_SHIFT  	= 16,	// R/W, Number of Address Bytes
									// 	     Set to the number of address bytes required (the address itself is programmed in the Flash 
									//		Command Address Register).
									//		This should be setup before triggering the command via bit 0 of this register.
									//		2'b00 = 1 address byte	
									//		2'b01 = 2 address bytes
									//		2'b10 = 3 address bytes
									//		2'b11 = 4 address bytes
									
	CMD_MODE_BIT_EN_SHIFT  	= 18,	// R/W, Set to "1" to ensure the mode bits as defined in the Mode Bit Configuration Register are sent following the address bytes.
	
	CMD_ADDR_EN_SHIFT  		= 19,	// R/W, Command Address Enable
									//		Set to "1" if the command specified in bits 31:24 requires an address
									//		This should be setup before triggering the command via bit 0 of this register.
									
	CMD_RD_BYTES_SHIFT  	= 20,	// R/W, Number of Read Data Bytes
									// 	     Up to 8 Data bytes may be read using this command (Set to 0 for 1 byte, 7 for 8 bytes).
									// 	     In case of Flash Command Control Register (Using STIG) Register bit[2] is enabled, this field is not to care and Number
									//	     of Read Data Bytes is as specified in Flash Command Control Memory Register (Using STIG) Register bit[15:8].
									
	CMD_RD_EN_SHIFT  		= 23,	// R/W, Read Data Enable
									//		Set to "1" if the command specified in bits 31:24 requires read data bytes to be received from the device.
									
	CMD_OPCODE_SHIFT  		= 24	// R/W, Command Opcode	
}E_QSPI_CMD_CTRL_SHIFT;

// bit[0], Execute the command
// bit[1], STIG command execution in progress.
#define QSPI_STIG_ST_MASK	(1<<CMD_STIG_EXE_ST_SHIFT)

// bit[16, 17],  Number of Address Bytes
typedef enum
{
	CMD_ADDR_BYTES_1 = 0,
	CMD_ADDR_BYTES_2 = 1,
	CMD_ADDR_BYTES_3 = 2,
	CMD_ADDR_BYTES_4 = 3
}E_QSPI_CMD_ADDR_BYTES;

// bit[12, 14], Number of Write Data Bytes
// bit[20, 22], Number of Read Data Bytes
typedef enum
{
	CMD_RW_BYTES_1 = 0,
	CMD_RW_BYTES_2 = 1,
	CMD_RW_BYTES_3 = 2,
	CMD_RW_BYTES_4 = 3,
	CMD_RW_BYTES_5 = 4,
	CMD_RW_BYTES_6 = 5,
	CMD_RW_BYTES_7 = 6,
	CMD_RW_BYTES_8 = 7	
}E_QSPI_CMD_RW_BYTES;

// normal command without address and data
#define QSPI_ISSUE_CMD(op_code)\
			((((unsigned)(op_code)) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// erase command with address
#define QSPI_ISSUE_ERASE_CMD(op_code, addr_bytes)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_ADDR_EN_SHIFT)|\
			((addr_bytes) << CMD_ADDR_BYTES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// read status command
#define QSPI_ISSUE_RD_ST_CMD(op_code, rd_bytes)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_RD_EN_SHIFT)|\
			((rd_bytes) << CMD_RD_BYTES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// nand erase command
#define QSPI_ISSUE_WR_ST_CMD(op_code, wr_bytes)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((0) << CMD_ADDR_EN_SHIFT)|\
			((0) << CMD_ADDR_BYTES_SHIFT)|\
			((1) << CMD_WR_EN_SHIFT)|\
			((wr_bytes) << CMD_WR_BYTES_SHIFT)|\
			((0) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))


// read data command
#define QSPI_ISSUE_RD_DATA_CMD(op_code, rd_bytes, addr_bytes, dummy_cycles)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_RD_EN_SHIFT)|\
			((rd_bytes) << CMD_RD_BYTES_SHIFT)|\
			((1) << CMD_ADDR_EN_SHIFT)|\
			((addr_bytes) << CMD_ADDR_BYTES_SHIFT)|\
			((dummy_cycles) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// read identification command
#define QSPI_ISSUE_RD_ID_CMD(op_code, rd_bytes, dummy_cycles)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_RD_EN_SHIFT)|\
			((rd_bytes) << CMD_RD_BYTES_SHIFT)|\
			((0) << CMD_ADDR_EN_SHIFT)|\
			((0) << CMD_ADDR_BYTES_SHIFT)|\
			((dummy_cycles) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// write data command
#define QSPI_ISSUE_WR_DATA_CMD(op_code, wr_bytes, addr_bytes, dummy_cycles)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_ADDR_EN_SHIFT)|\
			((addr_bytes) << CMD_ADDR_BYTES_SHIFT)|\
			((1) << CMD_WR_EN_SHIFT)|\
			((wr_bytes) << CMD_WR_BYTES_SHIFT)|\
			((dummy_cycles) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// nand read ID command
#define QSPI_NAND_ISSUE_RD_ID_CMD(op_code, rd_bytes, dummy_cycles)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_RD_EN_SHIFT)|\
			((rd_bytes) << CMD_RD_BYTES_SHIFT)|\
			((0) << CMD_ADDR_EN_SHIFT)|\
			((0) << CMD_ADDR_BYTES_SHIFT)|\
			((dummy_cycles) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// nand erase command
#define QSPI_NAND_ISSUE_ERASE_CMD(op_code, addr_bytes, dummy_cycles)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_ADDR_EN_SHIFT)|\
			((addr_bytes) << CMD_ADDR_BYTES_SHIFT)|\
			((0) << CMD_WR_EN_SHIFT)|\
			((0) << CMD_WR_BYTES_SHIFT)|\
			((dummy_cycles) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))

// nand erase command
#define QSPI_NAND_ISSUE_RANDOM_RD_DATA(op_code, addr_bytes, dummy_cycles)\
			(((unsigned)(op_code) << CMD_OPCODE_SHIFT)|\
			((1) << CMD_ADDR_EN_SHIFT)|\
			((addr_bytes) << CMD_ADDR_BYTES_SHIFT)|\
			((0) << CMD_WR_EN_SHIFT)|\
			((0) << CMD_WR_BYTES_SHIFT)|\
			((dummy_cycles) << CMD_DUMMY_CYCLES_SHIFT)|\
			((1) << CMD_EXE_SHIFT))


			
// QSPI_DEVRDINST_REG
// bit[28, 24], Number of Dummy Clock Cycles required by device for Read Instruction
typedef enum
{
	DUMMY_CLKS_0 = 0,
	DUMMY_CLKS_8 = 8
}E_QSPI_DummyClks;

// bit[17, 16], Data Transfer Type for Standard SPI modes 
typedef enum
{
	SIO_MODE = 0,
	DIO_MODE = 1,
	QIO_MODE = 2
}E_QSPI_IOMode;

// QSPI_DEVSIZE_REG
// bit[3, 0], Number of address bytes
typedef enum
{
	ADDR_BYTES_1 = 0,
	ADDR_BYTES_2 = 1,
	ADDR_BYTES_3 = 2
}E_QSPI_AddrBytes;

// QSPI status
typedef enum
{
	QSPI_ST_DONE		= 0,
	QSPI_ST_FAIL		= 1,
	QSPI_ST_TIME_OUT	= 2
}E_QSPI_ST;

#endif

