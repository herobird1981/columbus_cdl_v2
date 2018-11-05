/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _NFC_H_
#define _NFC_H_
#include "cdl_config.h"
#include "cdl_nfc_reg.h"
#include "cdl_onfi.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// NFC status
typedef enum
{
	NFC_ST_DONE		= 0,
	NFC_ST_FAIL		= 1,
	NFC_ST_TIME_OUT	= 2
}E_NFC_ST;

// NFC timing mode
typedef enum
{
	NFC_MODE_0	= 0,	// 100 ns
	NFC_MODE_1	= 1,	// 50 ns
	NFC_MODE_2	= 2,	// 35 ns
	NFC_MODE_3	= 3,	// 30 ns
	NFC_MODE_4	= 4,	// 25 ns
	NFC_MODE_5	= 5		// 20 ns
}E_NFC_MODE;

// nfc timeout
#define NFC_TIME_OUT                        (15000000)               // 15s
#define NFC_CHK_STATUS_DELAY                (10)                    // 10uSec
#define DIV_BY1024(_Divisor)                (_Divisor >> 10)
#define PAGE_SIZE_IN_KBYTES(_SizeInBytes)   (DIV_BY1024(_SizeInBytes))
/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
// NFC parameter
typedef struct
{
	uint8_t bus_size;				// NFC bus size
									// 	= IO_WIDTH_X08
									// 	= IO_WIDTH_X16		
								
	E_NFC_ECC_CAP bch_mode;			// NFC bch control mode for 1KB data block
									//	= ECC_CAP_2_BITS
									// 	= ECC_CAP_4_BITS
									//	= ECC_CAP_8_BITS
									//	= ECC_CAP_16_BITS
									//	= ECC_CAP_24_BITS
									//	= ECC_CAP_32_BITS
	
	uint8_t blk_shift_bits; 		// block shift bits(relative to page, numPagePerBlock = 128, blkShiftBits = 7) 	

	uint8_t ecc_thresh; 			// ecc threshold

	uint16_t page_size;				// page size

	uint16_t spare_size;			// spare area size

	uint16_t pages_per_blk;			// the page number per block

	uint16_t blks_per_lun;			// the block number per lun

	uint8_t luns_per_chip;			// the lun number per die

	uint8_t revd[3];				// reserved for future use
	
	uint32_t blks_per_chip;			// the block number per chip
									//	= luns_per_chip * blks_per_lun

	uint32_t timing;				// NFC timing mode

	uint32_t time_out;				// uint(10 us) 									
}nfc_para_t;
typedef nfc_para_t * nfc_para_t_ptr;

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern nfc_para_t g_nfc_para;

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
uint8_t nfc_set_mode(E_NFC_MODE mode, E_NFC_CLK_DLY clk_delay);

void nfc_init(void);

E_NFC_ST nfc_reset(void);

uint8_t nfc_read_id(uint8_t *p_id);

uint8_t nfc_get_feature(uint8_t fa, uint8_t *p_fea);

uint8_t nfc_set_feature(uint8_t fa, uint8_t *p_fea);

uint8_t nfc_set_array_op_mode(uint8_t op_mode);

uint8_t nfc_param_page_read(uint16_t col, uint8_t *p_data, uint32_t size);
	
uint8_t nfc_blk_erase(uint32_t addr);

uint8_t nfc_raw_data_write(uint32_t addr, uint8_t *p_data, uint32_t size);

uint8_t nfc_raw_data_read(uint32_t addr, uint8_t *p_data, uint32_t size);

uint8_t nfc_page_write(uint32_t addr, uint8_t *p_data, uint32_t size);

uint8_t nfc_page_read(uint32_t addr, uint8_t *p_data, uint32_t size);

uint8_t nfc_otp_page_write(uint32_t otp_page, uint8_t *p_data, uint32_t size);

uint8_t nfc_otp_page_read(uint32_t otp_page, uint8_t *p_data, uint32_t size);

uint8_t nfc_otp_protect(uint32_t otp_page);

uint8_t nfc_spare_read(uint32_t addr, uint8_t *p_data, uint32_t size);

BOOL nfc_param_init(uint8_t *p_data, nfc_para_t *p_para);

uint32_t nfc_read_int_st(void);

void nfc_clr_int_st(uint32_t st);

void nfc_set_int_mask(uint32_t val);

void nfc_int_ctrl(E_NFC_CONTROL_INT_EN int_en);

uint8_t nfc_raw_data_read_1(uint32_t addr, uint8_t *p_data, uint32_t size);

#endif

