/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _QSPI_NAND_CMD_H_
#define _QSPI_NAND_CMD_H_
#include "cdl_config.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// manufacture ID
#define QSPI_MANU_ID_MXIC		0xC2
#define QSPI_MANU_ID_WINBOND	0xEF
#define QSPI_MANU_ID_MICRON		0x2C

/*************************************************************************************************************/
// winbond
// Software Die Select
#define	QSPI_WINBOND_NAND_SW_DIE_SEL  	0xC2

typedef enum
{
	QSPI_WINBOND_NAND_DIE0	= 0,	// NAND Die0				
	QSPI_WINBOND_NAND_DIE1	= 1 	// NAND Die1
}E_QSPI_WINBOND_NAND_DIE;

/*************************************************************************************************************/
#define	QSPI_MICRON_NAND_SW_DIE_SEL  0xD0

typedef enum
{
	QSPI_MICRON_NAND_DIE0	= 0,	// NAND Die0				
	QSPI_MICRON_NAND_DIE1	= 0x40 	// NAND Die1
}E_QSPI_MICRON_NAND_DIE;

/*************************************************************************************************************/
// Reset
#define	QSPI_NAND_CMD_RST  			0xFF	// Reset

// read ID
#define QSPI_NAND_RD_ID				0x9F	// Read ID

// Write Operation
#define	QSPI_NAND_CMD_WREN  		0x06	// Write Enable
#define	QSPI_NAND_CMD_WRDI			0x04	// Write Disable

// Feature Operation
#define	QSPI_NAND_CMD_GET_FEA		0x0F	// Get Feature
#define	QSPI_NAND_CMD_SET_FEA		0x1F	// Set Feature

// feature setting
#define QSPI_NAND_BLK_PROT_FA		0xA0	// Block Protection
#define QSPI_NAND_OTP_FA			0xB0	// Secure OTP address
#define QSPI_NAND_ST_FA				0xC0	// Status

// Block erase
#define QSPI_NAND_BE				0xD8	// Block Erase

// Page program 
#define QSPI_NAND_PP_LOAD			0x02	// Program load data
#define QSPI_NAND_PP_LOAD_X4		0x32	// Program load data(x4)
#define QSPI_NAND_PP_LOAD_RAND		0x84	// Program load random data
#define QSPI_NAND_PP_LOAD_RAND_X4	0x34	// Program load random data(x4)
#define QSPI_NAND_PP_EXE			0x10	// Program execute

// Page read
#define QSPI_NAND_RD				0x13	// Page read
#define QSPI_NAND_RD_CACHE			0x0B	// Page from cache
#define QSPI_NAND_RD_CACHE_X2		0x3B	// Page from cache(x2)
#define QSPI_NAND_RD_CACHE_X4		0x6B	// Page from cache(x4)

// MXIC Secure OTP definition(addr: 0xB0)
typedef enum
{
	QSPI_MXIC_OTP_QE_SHIFT  	= 0,	// bit [0], QE 
								
	QSPI_MXIC_OTP_REVD0_SHIFT 	= 1,	// bit [1, 3], Reserved
	
	QSPI_MXIC_OTP_ECC_EN_SHIFT 	= 4,	// bit [4], ECC enable
									
	QSPI_MXIC_OTP_REVD1_SHIFT	= 5,	// bit [5], Reserved

	QSPI_MXIC_OTP_EN_SHIFT 		= 6,	// bit [6], Secure OTP Enable
	
	QSPI_MXIC_OTP_PROTECT_SHIFT = 7		// bit [7], Secure OTP Protect
}E_QSPI_MXIC_NAND_OTP_SHIFT;

// WINBOND Secure OTP definition(addr: 0xB0)
typedef enum
{								
	QSPI_WINBOND_OTP_REVD0_SHIFT 	= 0,	// bit [0, 2], Reserved

	QSPI_WINBOND_OTP_BUF_MODE_SHIFT = 3,	// bit [3], Buf Mode
	
	QSPI_WINBOND_OTP_ECC_EN_SHIFT 	= 4,	// bit [4], ECC enable
									
	QSPI_WINBOND_OTP_L_SHIFT		= 5,	// bit [5], OTP Lock

	QSPI_WINBOND_OTP_E_SHIFT 		= 6,	// bit [6], Enter OTP Enable
	
	QSPI_MXIC_OTP_DATA_LOCK_SHIFT 	= 7		// bit [7], OTP Data Pages Lock
}E_QSPI_WINBOND_NAND_OTP_SHIFT;

// buf mode
typedef enum
{								
	QSPI_BUF_MODE_0 = 0,
	QSPI_BUF_MODE_1 = 1
}E_QSPI_BUF_MODE;

// MICRON Secure OTP definition(addr: 0xB0)
typedef enum
{								
	QSPI_MICRON_OTP_REVD0_SHIFT 	= 0,	// bit [0], Reserved

	QSPI_MICRON_OTP_CFG0_SHIFT 		= 1,	// bit [1], CFG0
	
	QSPI_MICRON_OTP_REVD1_SHIFT 	= 2,	// bit [2,3], Reserved
		
	QSPI_MICRON_OTP_ECC_EN_SHIFT 	= 4,	// bit [4], ECC enable
									
	QSPI_MICRON_OTP_LOT_EN_SHIFT	= 5,	// bit [5], LOT_EN

	QSPI_MICRON_OTP_CFG1_SHIFT 		= 6,	// bit [6], CFG1
	
	QSPI_MICRON_OTP_CFG2_SHIFT 		= 7		// bit [7], CFG2
}E_QSPI_MICRON_NAND_OTP_SHIFT;

typedef enum
{								
	QSPI_ECC_DIS = 0,
	QSPI_ECC_EN  = 1
}E_QSPI_ECC_MODE;

// OTP enter mode
typedef enum
{								
	QSPI_OTP_E_DIS = 0,
	QSPI_OTP_E_EN = 1
}E_QSPI_OTP_E_MODE;

// status definition(addr: 0xC0)
typedef enum
{
	QSPI_ST_OIP_SHIFT  		= 0,	// bit [0], Operation in progress
									// The bit value indicates whether the device is busy in operations of read/program 
									// execute/ erase/ reset command. 
									// 1: Busy,  0: Ready 
								
	QSPI_ST_WEL_SHIFT 		= 1,	// bit [1], Write enable latch
									// The bit value indicates whether the device is set to internal write enable latch. 
									// When WEL bit sets to 1, which means the internal write enable latch is set, and 
									// then the device can accept program/ erase/write status register instruction. 
									// 1: write enable,  0: not write enable
									// The bit value will be cleared (as "0") by issuing Write Disable command(04h).
	
	QSPI_ST_ERS_FAIL_SHIFT 	= 2,	// bit [2], Erase fail
									// he bit value shows the status of erase failure or if host erase any invalid
									// address or protected area (including protected blocks or protected Secure OTP area). 
									// 0: Passed, 1: Failed 
									// The bit value will be cleared (as "0") by RESET command or at the beginning of the block 
									// erase command operation.
									
	QSPI_ST_PGM_FAIL_SHIFT	= 3,	// bit [3], Program fail
									// The bit value shows the status of program failure or if host program any invalid 
									// address or protected area (including protected blocks or protected Secure OTP area). 
									// 0: Passed, 1: Failed 
									// The bit value will be cleared (as "0") by RESET command or during the program execute 
									// command operation.

	QSPI_ST_ECC_SHIFT  		= 4,	// bit [4, 5], ECC Status
									// The bit shows the status of ECC as below:
									// 00b = 0 bit error
									// 01b = 1 to 4 bits error corrected.
									// 10b = More than 4-bit error not corrected.
									// 11b = Reserved
									// The value of ECC_Sx (S1:S0) bits will be clear as "00b" by Reset command or 
									// at the start of the Read operation. After a valid Read operation completion, the 
									// bit will be updated to reflect the ECC status of the current valid Read operation. 
									// The ECC_Sx (S1:S0) value reflects the ECC status of the content of the page 0 
									// of block 0 after a power-on reset.
									// If the internal ECC is disabled by the Set feature command, the ECC_
									// Sx(S1:S0) are invalid.
	
	QSPI_ST_REVD_SHIFT  	= 6		// bit [6, 7], Reserved
}E_QSPI_NAND_ST_SHIFT;

#define QSPI_ECC_MASK	3

// Block Protection(addr: 0xA0)
typedef enum
{
	QSPI_BP_SP_SHIFT  		= 0,	// bit [0], Solid-protection
									// Once the SP bit sets as 1, the rest of the protection bits (BPx bits, 
									// Invert bits, complementary bits) cannot be changed during the current power cycle.
								
	QSPI_BP_COMP_SHIFT 		= 1,	// bit [1], Complementary
	
	QSPI_BP_INVERT_SHIFT 	= 2,	// bit [2], Complementary
									
	QSPI_BP_BPX_SHIFT		= 3,	// bit [3, 5], BPx

	QSPI_BP_REVD_SHIFT 		= 6,	// bit [6], Reserved
	
	QSPI_BP_BPRWD_SHIFT 	= 7		// bit [7], BPRWD1
									// If BPRWD is enabled and WP# is LOW, then the block protection register cannot be changed.
}E_QSPI_NAND_BLK_PROT_SHIFT;


#define QSPI_BPX_VAL(bp2, bp1, bp0, invert, comp)\
			(((bp2) << (QSPI_BP_BPX_SHIFT + 2))|\
			((bp1) << (QSPI_BP_BPX_SHIFT + 1))|\
			((bp0) << QSPI_BP_BPX_SHIFT)|\
			((invert) << QSPI_BP_INVERT_SHIFT)|\
			((comp) << QSPI_BP_COMP_SHIFT))

// Definition of Protection Bits(For 1Gb)
// Definition of Protection Bits(For 2Gb)
#define	BP_ALL_UNLOCKED  		QSPI_BPX_VAL(0, 0, 0, 0, 0)	// all unlocked
#define	BP_UPPER_1_64_LOCKED  	QSPI_BPX_VAL(0, 0, 1, 0, 0)	// upper 1/64 locked
#define	BP_UPPER_1_32_LOCKED  	QSPI_BPX_VAL(0, 1, 0, 0, 0)	// upper 1/32 locked
#define	BP_UPPER_1_16_LOCKED  	QSPI_BPX_VAL(0, 1, 1, 0, 0)	// upper 1/16 locked
#define	BP_UPPER_1_8_LOCKED  	QSPI_BPX_VAL(1, 0, 0, 0, 0)	// upper 1/8 locked
#define	BP_UPPER_1_4_LOCKED  	QSPI_BPX_VAL(1, 0, 1, 0, 0)	// upper 1/4 locked
#define	BP_UPPER_1_2_LOCKED  	QSPI_BPX_VAL(1, 1, 0, 0, 0)	// upper 1/2 locked
#define	BP_ALL_LOCKED  			QSPI_BPX_VAL(1, 1, 1, 0, 0)	// all locked (default)

// The following is only suitable for 1Gb
#define	BP_LOWER_1_64_LOCKED  	QSPI_BPX_VAL(0, 0, 1, 1, 0)	// lower 1/64 locked
#define	BP_LOWER_1_32_LOCKED  	QSPI_BPX_VAL(0, 1, 0, 1, 0)	// lower 1/32 locked
#define	BP_LOWER_1_16_LOCKED  	QSPI_BPX_VAL(0, 1, 1, 1, 0)	// lower 1/16 locked
#define	BP_LOWER_1_8_LOCKED  	QSPI_BPX_VAL(1, 0, 0, 1, 0)	// lower 1/8 locked
#define	BP_LOWER_1_4_LOCKED  	QSPI_BPX_VAL(1, 0, 1, 1, 0)	// lower 1/4 locked
#define	BP_LOWER_1_2_LOCKED  	QSPI_BPX_VAL(1, 1, 0, 1, 0)	// lower 1/2 locked
#define	BP_LOWER_63_64_LOCKED  	QSPI_BPX_VAL(0, 0, 1, 0, 1)	// lower 63/64 locked
#define	BP_LOWER_31_32_LOCKED  	QSPI_BPX_VAL(0, 1, 0, 0, 1)	// lower 31/32 locked
#define	BP_LOWER_15_16_LOCKED  	QSPI_BPX_VAL(0, 1, 1, 0, 1)	// lower 15/16 locked
#define	BP_LOWER_7_8_LOCKED  	QSPI_BPX_VAL(1, 0, 0, 0, 1)	// lower 7/8 locked
#define	BP_LOWER_3_4_LOCKED  	QSPI_BPX_VAL(1, 0, 1, 0, 1)	// lower 3/4 locked
#define	BP_BLK0_LOCKED  		QSPI_BPX_VAL(1, 1, 0, 0, 1)	// block 0
#define	BP_UPPER_63_64_LOCKED  	QSPI_BPX_VAL(0, 0, 1, 1, 1)	// upper 63/64 locked
#define	BP_UPPER_31_32_LOCKED  	QSPI_BPX_VAL(0, 1, 0, 1, 1)	// upper 31/32 locked
#define	BP_UPPER_15_16_LOCKED  	QSPI_BPX_VAL(0, 1, 1, 1, 1)	// upper 15/16 locked
#define	BP_UPPER_7_8_LOCKED  	QSPI_BPX_VAL(1, 0, 0, 1, 1)	// upper 7/8 locked
#define	BP_UPPER_3_4_LOCKED  	QSPI_BPX_VAL(1, 0, 1, 1, 1)	// upper 3/4 locked

// Definition of Protection Bits(For 2Gb)

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/


/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/

#endif

