/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _NFC_REG_H_
#define _NFC_REG_H_
#include "cdl_config.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define NFC_CTRL_BASE_ADDR NFC_BASE

#define NFC_COMMAND_REG				(NFC_CTRL_BASE_ADDR + 0x0000)	// 0x00000000, Controller commands register. 
#define NFC_CONTROL_REG				(NFC_CTRL_BASE_ADDR + 0x0004)	// 0x00000000, The main configurations register. 
#define NFC_STATUS_REG				(NFC_CTRL_BASE_ADDR + 0x0008)	// 0x00000000, The controller status register. 
#define NFC_STATUS_MASK_REG			(NFC_CTRL_BASE_ADDR + 0x000C)	// 0x00004040, The Mask register for the READ STATUS commands. 
#define NFC_INT_MASK_REG			(NFC_CTRL_BASE_ADDR + 0x0010)	// 0x00000000, Interrupts mask register.
#define NFC_INT_STATUS_REG			(NFC_CTRL_BASE_ADDR + 0x0014)	// 0x00000000, Interrupts status register. 
#define NFC_ECC_CTRL_REG			(NFC_CTRL_BASE_ADDR + 0x0018)	// 0x00000000, The ECC module control register. 
#define NFC_ECC_OFFSET_REG			(NFC_CTRL_BASE_ADDR + 0x001C)	// 0x00000000, The ECC offset in the spare area. 
#define NFC_ECC_STAT_REG			(NFC_CTRL_BASE_ADDR + 0x0020)	// 0x00000000, The ECC module status register. 
#define NFC_ADDR0_COL_REG			(NFC_CTRL_BASE_ADDR + 0x0024)	// 0x00000000, The Column address register 0. 
#define NFC_ADDR0_ROW_REG			(NFC_CTRL_BASE_ADDR + 0x0028)	// 0x00000000, The Row address register 0. 
#define NFC_ADDR1_COL_REG			(NFC_CTRL_BASE_ADDR + 0x002C)	// 0x00000000, The Column address register 1. 
#define NFC_ADDR1_ROW_REG			(NFC_CTRL_BASE_ADDR + 0x0030)	// 0x00000000, The Row address register 1. 
#define NFC_PROTECT_REG				(NFC_CTRL_BASE_ADDR + 0x0034)	// 0x00000000, The hardware protects against the write/erase process control register. 
#define NFC_FIFO_DATA_REG			(NFC_CTRL_BASE_ADDR + 0x0038)	// 0x00000000, The FIFO module interface. 
#define NFC_DATA_REG_REG			(NFC_CTRL_BASE_ADDR + 0x003C)	// 0x00000000, The data register. 
#define NFC_DATA_REG_SIZE_REG		(NFC_CTRL_BASE_ADDR + 0x0040)	// 0x00000000, The data register size select. 
#define NFC_DEV0_PTR_REG			(NFC_CTRL_BASE_ADDR + 0x0044)	// 0x00000000, The records table pointer in the system memory for the device 0 in the memory bank. 
#define NFC_DEV1_PTR_REG			(NFC_CTRL_BASE_ADDR + 0x0048)	// 0x00000000, The records table pointer in the system memory for the device 1 in the memory bank. 

#define NFC_DMA_ADDR_L_REG			(NFC_CTRL_BASE_ADDR + 0x0064)	// 0x00000000, The least significant part of the DMA base address. 

#define NFC_DMA_CNT_REG				(NFC_CTRL_BASE_ADDR + 0x006C)	// 0x00000000, The DMA module counter initial value. 
#define NFC_DMA_CTRL_REG			(NFC_CTRL_BASE_ADDR + 0x0070)	// 0x00000000, The DMA module control register. 
#define NFC_BBM_CTRL_REG			(NFC_CTRL_BASE_ADDR + 0x0074)	// 0x00000000, The BBM module control register 

#define NFC_MEM_CTRL_REG			(NFC_CTRL_BASE_ADDR + 0x0080)	// 0x00000000, The memory device control register. 
#define NFC_DATA_SIZE_REG			(NFC_CTRL_BASE_ADDR + 0x0084)	// 0x00000000, The page size value. 
#define NFC_TIMINGS_ASYN_REG		(NFC_CTRL_BASE_ADDR + 0x0088)	// 0x00000000, The timing configurations register 0. 

#define NFC_TIME_SEQ_0_REG			(NFC_CTRL_BASE_ADDR + 0x0090)	// 0x00000000, The command sequence timing configuration SFR. 
#define NFC_TIME_SEQ_1_REG			(NFC_CTRL_BASE_ADDR + 0x0094)	// 0x00000000, The command sequence timing configuration SFR. 

#define NFC_TIME_GEN_SEQ_0_REG		(NFC_CTRL_BASE_ADDR + 0x0098)	// 0x00000000, The generic command sequence timing configuration SFR. 
#define NFC_TIME_GEN_SEQ_1_REG		(NFC_CTRL_BASE_ADDR + 0x009C)	// 0x00000000, The generic command sequence timing configuration SFR. 
#define NFC_TIME_GEN_SEQ_2_REG		(NFC_CTRL_BASE_ADDR + 0x00A0)	// 0x00000000, The generic command sequence timing configuration SFR. 


#define NFC_FIFO_INIT_REG			(NFC_CTRL_BASE_ADDR + 0x00B0)	// 0x00000000, The Control register for the FIFO module. 
#define NFC_FIFO_STATE_REG			(NFC_CTRL_BASE_ADDR + 0x00B4)	// 0x00000000, The FIFO module status. 
#define NFC_GEN_SEQ_CTRL_REG		(NFC_CTRL_BASE_ADDR + 0x00B8)	// 0x00000000, The GENERIC_SEQ register. 
#define NFC_MLUN_REG				(NFC_CTRL_BASE_ADDR + 0x00BC)	// 0x00000000, The MLUN register. 
#define NFC_DEV0_SIZE_REG			(NFC_CTRL_BASE_ADDR + 0x00C0)	// 0x00000000, The remap table size expressed in the number of the records.  
#define NFC_DEV1_SIZE_REG			(NFC_CTRL_BASE_ADDR + 0x00C4)	// 0x00000000, The remap table size expressed in the number of the records.  

#define NFC_SS_CCNT0_REG			(NFC_CTRL_BASE_ADDR + 0x00E0)	// 0x00000000, The Super Sequence command 0 and 1 counters initial values
#define NFC_SS_CCNT1_REG			(NFC_CTRL_BASE_ADDR + 0x00E4)	// 0x00000000, The Super Sequence command 2 and 3 counters initial values
#define NFC_SS_SCNT_REG				(NFC_CTRL_BASE_ADDR + 0x00E8)	// 0x00000000, The Super Sequence internal and external command sequence loop counters initial values.
#define NFC_SS_ADDR_DEV_CTRL_REG	(NFC_CTRL_BASE_ADDR + 0x00EC)	// 0x00000000, The Super Sequence control register. 
#define NFC_SS_CMD0_REG				(NFC_CTRL_BASE_ADDR + 0x00F0)	// 0x00000000, The Super Sequence command 0 initial value. 
#define NFC_SS_CMD1_REG				(NFC_CTRL_BASE_ADDR + 0x00F4)	// 0x00000000, The Super Sequence command 1 initial value. 
#define NFC_SS_CMD2_REG				(NFC_CTRL_BASE_ADDR + 0x00F8)	// 0x00000000, The Super Sequence command 2 initial value. 
#define NFC_SS_CMD3_REG				(NFC_CTRL_BASE_ADDR + 0x00FC)	// 0x00000000, The Super Sequence command 3 initial value. 

#define NFC_SS_ADDR_REG				(NFC_CTRL_BASE_ADDR + 0x0100)	// 0x00000000, The Super Sequence address initial value. 
#define NFC_SS_MSEL_REG				(NFC_CTRL_BASE_ADDR + 0x0104)	// 0x00000000, The Super Sequence command/device select. 
#define NFC_SS_REQ_REG				(NFC_CTRL_BASE_ADDR + 0x0108)	// 0x00000000, The Super Sequence trigger request register. 
#define NFC_SS_BRK_REG				(NFC_CTRL_BASE_ADDR + 0x010C)	// 0x00000000, The Super Sequence breaks request register. 

#define NFC_DMA_TLVL_REG			(NFC_CTRL_BASE_ADDR + 0x0114)	// 0x00000000,   The DMA trigger level value.  

#define NFC_CMD_MARK_REG			(NFC_CTRL_BASE_ADDR + 0x0124)	// 0x00000000, The CMD ID initial value. 
#define NFC_LUN_STATUS_0_REG		(NFC_CTRL_BASE_ADDR + 0x0128)	// 0x00000000, The LUN per device status register. 

#define NFC_TIME_GEN_SEQ_3_REG		(NFC_CTRL_BASE_ADDR + 0x0134)	// 0x00000000, The generic command sequence timing configuration SFR. 

#define NFC_INTERNAL_STATUS_REG		(NFC_CTRL_BASE_ADDR + 0x0148)	// 0x00000000, The INTERNAL STAUS register. 

#define NFC_BBM_OFFSET_REG			(NFC_CTRL_BASE_ADDR + 0x014C)	// 0x00000000, The BBM_OFFSET register.
#define NFC_PARAM_REG_REG			(NFC_CTRL_BASE_ADDR + 0x0150)	// depends on he controller core configuration , The PARAM_REG register contains controller configuration. 

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// NAND command set
// reset command
#define NFC_REST_CMD0				0xFF	// seq_0

// identification operation
#define NFC_READ_ID_CMD0			0x90	// seq_1
#define NFC_PARA_PAGE_CMD0			0xEC	// seq_2
#define NFC_UNIQUE_ID_CMD0			0xED	// seq_2

// configuration operation
#define NFC_GET_FEATURE_CMD0		0xEE	// seq_2
#define NFC_SET_FEATURE_CMD0		0xEF	// seq_3

// status operation
#define NFC_READ_ST_CMD0			0x70	// seq_4

// column address operation
#define NFC_CHANGE_RD_COL_CMD0		0x05	// seq_6
#define NFC_CHANGE_RD_COL_CMD2		0xE0

// read operation
#define NFC_READ_PAGE_CMD0			0x00	// seq_10
#define NFC_READ_PAGE_CMD2			0x30

// program operation
#define NFC_PROG_PAGE_CMD0			0x80	// seq_12
#define NFC_PROG_PAGE_CMD1_3		0x10

// erase operation
#define NFC_ERASE_BLK_CMD0			0x60	// seq_14
#define NFC_ERASE_BLK_CMD1_3		0xD0

// OTP operation
#define NFC_PROG_OTP_CMD0			0xA0	// seq_12
#define NFC_PROG_OTP_CMD1_3			0x10

#define NFC_DATA_PROT_OTP_CMD0		0xA5	// seq_9
#define NFC_DATA_PROT_OTP_CMD1_3	0x10

#define NFC_READ_PAGE_OTP_CMD0		0xAF	// seq_10
#define NFC_READ_PAGE_OTP_CMD2		0x30

// Feature Address Definitons
typedef enum
{
	FA_TIMING_MODE 			= 1,	// Timing mode
	FA_PROG_OUT_DRV_STREN 	= 0x80,	// Programmable output drive strength
	FA_PROG_RB_PD_STREN		= 0x81,	// Programmable RB# pull-down strength
	FA_ARRAY_OP_MODE  		= 0x90	// Array operation mode
}E_NFC_FA;

// Feature Address 0x90 - Array operation mode
typedef enum
{
	OP_MODE_NOR 		= 0,	// Normal
	OP_MODE_OTP 		= 1,	// OTP operation
	OP_MODE_OTP_PROT	= 3		// OTP protection
}E_NFC_ARRAY_OP_MODE;

/*************************************************************************************************************/

// offset: 0x0000,  COMMAND register
typedef enum
{
	CMD_SEQ_SHIFT  		= 0,
	INPUT_SEL_SHIFT 	= 6,
	DATA_SEL_SHIFT 		= 7,
	CMD_0_SHIFT			= 8,
	CMD_1_3_SHIFT  		= 16,
	CMD_2_SHIFT  		= 24
}E_NFC_COMMAND_SHIFT;

// CMD_SEQ 
typedef enum
{
	CMD_SEQ_0  	= 0x00,
	CMD_SEQ_1 	= 0x21,
	CMD_SEQ_2 	= 0x22,
	CMD_SEQ_3	= 0x03,
	CMD_SEQ_4  	= 0x24,
	CMD_SEQ_5  	= 0x25,
	CMD_SEQ_6  	= 0x26,
	CMD_SEQ_7 	= 0x27,
	CMD_SEQ_8 	= 0x08,
	CMD_SEQ_9	= 0x29,
	CMD_SEQ_10  = 0x2A,
	CMD_SEQ_11  = 0x2B,
	CMD_SEQ_12  = 0x0C,
	CMD_SEQ_13 	= 0x0D,
	CMD_SEQ_14 	= 0x0E,
	CMD_SEQ_15	= 0x2F,
	CMD_SEQ_16  = 0x00, // revd
	CMD_SEQ_17  = 0x31,
	CMD_SEQ_18  = 0x32,
	CMD_SEQ_19  = 0x23,
	CMD_SEQ_20  = 0x34,
	CMD_SEQ_21  = 0x25,
	CMD_SEQ_22  = 0x36,
	CMD_SEQ_23  = 0x27,
	CMD_SEQ_24  = 0x28,
	CMD_SEQ_25  = 0x39
}E_NFC_CMD_SEQ;

// INPUT_SEL 
typedef enum
{
	INPUT_SEL_SIU = 0,
	INPUT_SEL_DMA = 1
}E_NFC_COMMAND_INPUT_SEL;

// DATA_SEL 
typedef enum
{
	SEL_FIFO_MODULE = 0,
	SEL_DATA_REG 	= 1
}E_NFC_COMMAND_DATA_SEL;

#define NFC_GET_CMD_VAL(cmd_seq, input_sel, data_sel, cmd0, cmd1_3, cmd2)\
			(((unsigned)(cmd2) << CMD_2_SHIFT)|\
			((cmd1_3) << CMD_1_3_SHIFT)|\
			((cmd0) << CMD_0_SHIFT)|\
			((data_sel) << DATA_SEL_SHIFT)|\
			((input_sel) << INPUT_SEL_SHIFT)|\
			((cmd_seq) << CMD_SEQ_SHIFT))

// offset: 0x0004,  CONTROL register
typedef enum
{
	CTRL_READ_STATUS_EN_SHIFT 		= 0,
	CTRL_ECC_BLOCK_SIZE_SHIFT 		= 1,
	CTRL_RETRY_EN_SHIFT 			= 3,
	CTRL_INT_EN_SHIFT				= 4,
	CTRL_ECC_EN_SHIFT  				= 5,
	CTRL_BLOCK_SIZE_SHIFT  			= 6,
	CTRL_IO_WIDTH_SHIFT  			= 12,
	CTRL_BBM_EN_SHIFT  				= 13,
	CTRL_PROT_EN_SHIFT  			= 14,
	CTRL_ADDR0_AUTO_INCR_SHIFT 		= 16,
	CTRL_ADDR1_AUTO_INCR_SHIFT 		= 17,
	CTRL_SMALL_BLOCK_EN_SHIFT  		= 21,
	CTRL_MLUN_EN_SHIFT  			= 22,
	CTRL_AUTO_READ_STAT_EN_SHIFT 	= 23
}E_NFC_CONTROL_SHIFT;

// ECC_BLOCK_SIZE 
typedef enum
{
	ECC_BLK_SIZE_256B  = 0,
	ECC_BLK_SIZE_512B  = 1,
	ECC_BLK_SIZE_1024B = 2,
	ECC_BLK_SIZE_2048B = 3
}E_NFC_CONTROL_ECC_BLK_SIZE;

// INT_EN 
typedef enum
{
	CTRL_INT_DIS = 0,
	CTRL_INT_EN  = 1
}E_NFC_CONTROL_INT_EN;

// ECC_EN 
typedef enum
{
	ECC_DIS = 0,
	ECC_EN  = 1
}E_NFC_CONTROL_ECC_EN;

// BLOCK_SIZE 
typedef enum
{
	CTRL_BLK_SIZE_32_PAGES  = 0,
	CTRL_BLK_SIZE_64_PAGES  = 1,
	CTRL_BLK_SIZE_128_PAGES = 2,
	CTRL_BLK_SIZE_256_PAGES = 3
}E_NFC_CONTROL_BLOCK_SIZE;

// IO_WIDTH 
typedef enum
{
	IO_WIDTH_X08 = 0,
	IO_WIDTH_X16 = 1
}E_NFC_CONTROL_IO_WIDTH;

// PROT_EN 
typedef enum
{
	CTRL_PROT_DIS = 0,
	CTRL_PROT_EN  = 1
}E_NFC_CONTROL_PROT_EN;

#define NFC_GET_CTRL_VAL(ecc_blk_size, int_en, ecc_en, io_width)\
			(((1) << CTRL_AUTO_READ_STAT_EN_SHIFT)|\
			((0) << CTRL_MLUN_EN_SHIFT)|\
			((0) << CTRL_SMALL_BLOCK_EN_SHIFT)|\
			((0) << CTRL_ADDR1_AUTO_INCR_SHIFT)|\
			((0) << CTRL_ADDR0_AUTO_INCR_SHIFT)|\
			((0) << CTRL_PROT_EN_SHIFT)|\
			((0) << CTRL_BBM_EN_SHIFT)|\
			((io_width) << CTRL_IO_WIDTH_SHIFT)|\
			((CTRL_BLK_SIZE_64_PAGES) << CTRL_BLOCK_SIZE_SHIFT)|\
			((ecc_en) << CTRL_ECC_EN_SHIFT)|\
			((int_en) << CTRL_INT_EN_SHIFT)|\
			((0) << CTRL_RETRY_EN_SHIFT)|\
			((ecc_blk_size) << CTRL_ECC_BLOCK_SIZE_SHIFT)|\
			((1) << CTRL_READ_STATUS_EN_SHIFT))

// offset: 0x0008,  STATUS register 
typedef enum
{
	MEM0_ST_SHIFT 			= 0,
	MEM1_ST_SHIFT 			= 1,
	CTRL_ST_SHIFT 			= 8,
	DATASIZE_ERROR_ST_SHIFT	= 9,
	DATA_REG_ST_SHIFT  		= 10,
	CMD_ID_SHIFT  			= 16
}E_NFC_STATUS_SHIFT;

#define NFC_ST_MEM0_READY	(1<<MEM0_ST_SHIFT)
#define NFC_ST_MEM1_READY	(1<<MEM1_ST_SHIFT)

// offset: 0x000C,  STATUS_MASK register 
typedef enum
{
	STATE_MASK_SHIFT = 0,
	ERROR_MASK_SHIFT = 8
}E_NFC_STATUS_MASK_SHIFT;

#define NFC_GET_ST_MASK_VAL(st_mask, err_mask)\
			(((err_mask) << ERROR_MASK_SHIFT)|\
			((st_mask) << STATE_MASK_SHIFT))

// offset: 0x0014, INT_STATUS Register
typedef enum
{
	PROT_INT_FL_SHIFT		= 0,
	CMD_END_INT_FL_SHIFT	= 1,
	DATA_REG_FL_SHIFT		= 2,
	DMA_INT_FL_SHIFT 		= 3,
	TRANS_ERR_FL_SHIFT 		= 4,
	SS_READY_INT_FL_SHIFT 	= 5,
	PG_SZ_ERR_INT_FL_SHIFT 	= 6,
	MEM0_RDY_INT_FL_SHIFT 	= 8,
	MEM1_RDY_INT_FL_SHIFT 	= 9,
	STAT_ERR_INT0_FL_SHIFT 	= 16,
	STAT_ERR_INT1_FL_SHIFT  = 17,
	ECC_INT0_FL_SHIFT 		= 24,
	ECC_INT1_FL_SHIFT  		= 25
}E_NFC_INT_STATUS_SHIFT;

#define NFC_CMD_DONE_MASK	(1<<CMD_END_INT_FL_SHIFT)
#define NFC_CHK_CMD_DONE	(1<<CMD_END_INT_FL_SHIFT)


// offset: 0x0018,  ECC_CTRL  register 
typedef enum
{
	ECC_CAP_SHIFT 	= 0,
	ERR_THRSH_SHIFT = 8,
	ECC_SEL_SHIFT  	= 16
}E_NFC_ECC_CTRL_SHIFT;

// ECC_CAP
typedef enum
{
	ECC_CAP_2_BITS 	 = 0, // 256/512/1024 B, ecc correction bytes per blk(async, 4B) 
	ECC_CAP_4_BITS   = 1, // 256/512/1024 B, ecc correction bytes per blk(async, 7B)
	ECC_CAP_8_BITS   = 2, // 256/512/1024 B, ecc correction bytes per blk(async, 14B)
	ECC_CAP_16_BITS  = 3, // 256/512/1024 B, ecc correction bytes per blk(async, 28B)
	ECC_CAP_24_BITS  = 4, // 256/512/1024 B, ecc correction bytes per blk(async, 42B)
	ECC_CAP_32_BITS  = 5  // 256/512/1024 B, ecc correction bytes per blk(async, 56B)
}E_NFC_ECC_CAP;

// Size of Correction Bytes per 1KByte ECC block in Asynchronous mode
// This enumeration is defined here ONLY for documentation purposes because
// the ONLY difference between E_NFC_ECC_SIZE_ASYNC and E_NFC_ECC_SIZE_SYNC is
// for 4 bits ECC, and the smallest part (512Mbits flash) that we consider
// already has enough spare area to support 8 bits ECC (It has 16Bytes per page)
// Therefore ONLY E_NFC_ECC_SIZE_SYNC is really necessary from the code stand point.
typedef enum
{
	MIN_ASYC_SIZE4_2BITS_ECC    = 4,    // 2 Bits ECC requires 4bytes per 1KB of data
	MIN_ASYC_SIZE4_4BITS_ECC    = 7,    // 4 Bits ECC requires 8bytes per 1KB of data
	MIN_ASYC_SIZE4_8BITS_ECC    = 14,   // 8 Bits ECC requires 14bytes per 1KB of data
	MIN_ASYC_SIZE4_16BITS_ECC   = 28,   // 16 Bits ECC requires 28bytes per 1KB of data
	MIN_ASYC_SIZE4_24BITS_ECC   = 42,   // 24 Bits ECC requires 42bytes per 1KB of data
	MIN_ASYC_SIZE4_32BITS_ECC   = 56    // 32 Bits ECC requires 56bytes per 1KB of data
}E_NFC_ECC_SIZE_ASYNC;

// Size of Correction Bytes per 1KByte ECC block in Synchronous mode Or IO_Width =16
typedef enum
{
	MIN_SYC_SIZE4_2BITS_ECC     = 4,    // 2 Bits ECC requires 4bytes per 1KB of data
	MIN_SYC_SIZE4_4BITS_ECC     = 8,    // 4 Bits ECC requires 8bytes per 1KB of data
	MIN_SYC_SIZE4_8BITS_ECC     = 14,   // 8 Bits ECC requires 14bytes per 1KB of data
	MIN_SYC_SIZE4_16BITS_ECC    = 28,   // 16 Bits ECC requires 28bytes per 1KB of data
	MIN_SYC_SIZE4_24BITS_ECC    = 42,   // 24 Bits ECC requires 42bytes per 1KB of data
	MIN_SYC_SIZE4_32BITS_ECC    = 56    // 32 Bits ECC requires 56bytes per 1KB of data
}E_NFC_ECC_SIZE_SYNC;

// ECC_SEL
typedef enum
{
	ECC_ERR 	= 0,
	ECC_UNC 	= 1,
	ECC_OVER  	= 2
}E_NFC_ECC_SEL;

#define NFC_GET_ECC_CTRL_VAL(ecc_cap, ecc_thrsh, ecc_sel)\
			(((ecc_sel) << ECC_SEL_SHIFT)|\
			((ecc_thrsh) << ERR_THRSH_SHIFT)|\
			((ecc_cap) << ECC_CAP_SHIFT))

// offset: 0x0020, ECC_STAT register
typedef enum
{
	ECC_ERR_0_SHIFT		= 0,
	ECC_ERR_1_SHIFT		= 1,
	ECC_UNC_0_SHIFT		= 8,
	ECC_UNC_1_SHIFT		= 9,
	ECC_OVER_0_SHIFT  	= 16,
	ECC_OVER_1_SHIFT  	= 17
}E_NFC_ECC_STAT_SHIFT;

// offset: 0x0070, ECC_STAT register
typedef enum
{
	DMA_READY_SHIFT  	= 0,
	ERR_FLAG_SHIFT  	= 1,
	DMA_BURST_SHIFT		= 2,
	DMA_MODE_SHIFT		= 5,
	DMA_START_SHIFT		= 7
}E_NFC_DMA_CTRL_SHIFT;

// DMA_BURST
typedef enum
{
	ADDR_INC_4_TRANS  			= 0, // incrementing precise burst of precisely four transfers (address increment) 
	ADDR_CONST_16_TRANS  		= 1, // stream precise burst of precisely sixteen transfers (address constant)
	ADDR_INC_1_TRANS			= 2, // single transfer (address increment) 
	ADDR_INC_UNSPECIFIED_TRANS	= 3, // burst of unspecified length (address increment) 
	ADDR_INC_8_TRANS			= 4, // incrementing precise burst of precisely eight transfers (address increment) 
	ADDR_INC_16_TRANS			= 5  // incrementing precise burst of precisely sixteen transfers (address increment) 
}E_NFC_DMA_BURST;

#define NFC_DMA_CTRL_VAL(burst_type)\
			(((1) << DMA_START_SHIFT)|\
			((0) << DMA_MODE_SHIFT)|\
			((burst_type) << DMA_BURST_SHIFT)|\
			((0) << ERR_FLAG_SHIFT)|\
			((0) << DMA_READY_SHIFT))

// offset: 0x0088, TIMINGS_ASYN Register
typedef enum
{
	TIMING_ASYN_TRWP_SHIFT  	= 0,	// RE# or WE# pulse width. 
	TIMING_ASYN_TRWH_SHIFT  	= 4, 	// RE# or WE# high hold time.
	TIMING_ASYN_CLK_DLY_SHIFT  	= 8		// To support mode2&3 of NFC.These two bits are used to control the delay of nfc_clk when sampling the nfc_dq signals.
										// 2'b00:no additional delay, same as 1st TO
										// 2'b01: one additional nfc_clk cycle delay
										// 2'b10: two additional nfc_clk cycle delay
										// 2'b11: thress additional nfc_clk cycle delay
}E_NFC_TIMINGS_ASYN_SHIFT;

// DMA_BURST
typedef enum
{
	NFC_CLK_DLY_0 = 0, // no additional delay, same as 1st TO
	NFC_CLK_DLY_1 = 1, // one additional nfc_clk cycle delay
	NFC_CLK_DLY_2 = 2, // two additional nfc_clk cycle delay
	NFC_CLK_DLY_3 = 3, // three additional nfc_clk cycle delay
}E_NFC_CLK_DLY;

#define NFC_TIMINGS_ASYN_VAL(trwp, trwh, clk_dly)\
			(((clk_dly) << TIMING_ASYN_CLK_DLY_SHIFT)|\
			((trwh) << TIMING_ASYN_TRWH_SHIFT)|\
			((trwp) << TIMING_ASYN_TRWP_SHIFT))

// offset: 0x0090, TIME_SEQ_0 Register
typedef enum
{
	TIME_TCCS_SHIFT   	= 0,	// Change column setup. 
	TIME_REVD0_SHIFT  	= 6,	// Reserved for future use. 
	
	TIME_TADL_SHIFT  	= 8,	// ALE to data loading time for synchronous interface. 
								// ALE to data start time for asynchronous interface. 
								
	TIME_REVD1_SHIFT  	= 14,	// Reserved for future use. 
	
	TIME_TRHW_SHIFT  	= 16,	// Data output to command, address, or data input time for synchronous interface. 
								// RE# high to WE# low time for asynchronous interface. 
								
	TIME_REVD2_SHIFT  	= 22,	// Reserved for future use. 
	
	TIME_TWHR_SHIFT  	= 24,	// Command cycle to data output time for synchronous interface
								// WE# high to RE# low time for asynchronous interface
								
	TIME_REVD3_SHIFT  	= 30 	// Read high to Read low
	
}E_NFC_TIME_SEQ_0_SHIFT;

#define NFC_TIME_SEQ_0_VAL(tccs, tadl, trhw, twhr)\
			(((twhr) << TIME_TWHR_SHIFT)|\
			((trhw) << TIME_TRHW_SHIFT)|\
			((tadl) << TIME_TADL_SHIFT)|\
			((tccs) << TIME_TCCS_SHIFT))


// offset: 0x0094, TIME_SEQ_1 Register 
typedef enum
{
	TIME_TWB_SHIFT  = 0,	// Busy time for interface change

	TIME_TRR_SHIFT  = 8, 	// Read high to Read low
	
	TIME_TWW_SHIFT  = 16 	// WP# transition to command cycle delay. his field is used to 
							// parameterize the tWW delay. The expected tWW delay value will be 
							// TWW+5 clkb clock cycles. 
}E_NFC_TIME_SEQ_1_SHIFT;

#define NFC_TIME_SEQ_1_VAL(twb, trr, tww)\
			(((tww) << TIME_TWW_SHIFT)|\
			((trr) << TIME_TRR_SHIFT)|\
			((twb) << TIME_TWB_SHIFT))

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/


#endif

