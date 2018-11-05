/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "cdl_onfi.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/


/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
/*****************************************************************************/
//  Function:  uint16_t onfi_crc16_cpu_calc(uint8_t *p_data, int32_t size)
//
//  Parameters:
//		p_data [in]	: data buffer which is to be writen to the crc module
//
//		size			: the size of the data buffer 
//
//  Return:
//		crc out value.
//		
//  Description:
//      Caculate the crc value with data input by cpu.
// 	   CRC-16:     x^16 + x^15 + x^2 + x^0		(0x8005) 
//
/*****************************************************************************/
#define CRC_16_POLYNOMIALS      0x8005
uint16_t onfi_crc16_cpu_calc(uint8_t *p_data, int32_t size)
{
	uint16_t crc = 0x4F4E;
	uint8_t i;
	uint8_t *ptr = p_data;

	while (size--) 
	{
		for (i = 0x80; i != 0; i = (i>>1)) 
		{
			if ((crc & 0x8000) != 0) 
			{
				crc <<= 1;
				crc ^= CRC_16_POLYNOMIALS;
			}
			else 
			{
				crc <<= 1;
			}
			
			if ((*ptr & i) != 0) 
			{
				crc ^= CRC_16_POLYNOMIALS; 
			}
		}
		
		ptr++;
	}

	return crc;
}

void onfi_para_page_print(uint8_t *p_data)
{
	onfi_t *p_onfi = (onfi_t *)p_data;
	uint16_t crc;

	debug("struc size: %x\n", sizeof(onfi_t));
	
	// byte[0, 31], Revision information and features block
	debug("sig:%x\n", p_onfi->sig);
	debug("rev_num:%x\n", p_onfi->rev_num);
	debug("fea_sup:%x\n", p_onfi->fea_sup);
	debug("cmd_sup:%x\n", p_onfi->cmd_sup);

	// byte[32, 79], Manufacturer information bloc
	debug("manu_id:%x\n", p_onfi->manu_id);
	
	// byte[80, 127], Memory organization block
	debug("bytes_per_page:%x\n", get_32_bits_val(p_onfi->bytes_per_page));
	debug("bytes_per_spare:%x\n", get_16_bits_val(p_onfi->bytes_per_spare));
	debug("bytes_per_partial_page:%x\n", get_32_bits_val(p_onfi->bytes_per_partial_page));
	debug("bytes_per_partial_spare:%x\n", get_16_bits_val(p_onfi->bytes_per_partial_spare));
	debug("pages_per_blk:%x\n", get_32_bits_val(p_onfi->pages_per_blk));
	debug("blks_per_lun:%x\n", get_32_bits_val(p_onfi->blks_per_lun));
	debug("num_luns:%x\n", p_onfi->num_luns);
	debug("addr_cycle:%x\n", p_onfi->addr_cycle);
	debug("bits_per_cell:%x\n", p_onfi->bits_per_cell);	
	debug("max_bad_blk_per_lun:%x\n", get_16_bits_val(p_onfi->max_bad_blk_per_lun));
	debug("blk_endurance:%x\n", get_16_bits_val(p_onfi->blk_endurance));
	debug("valid_blks_guaranteed:%x\n", p_onfi->valid_blks_guaranteed);	
	debug("valid_blk_endurance:%x\n", get_16_bits_val(p_onfi->valid_blk_endurance));
	debug("prog_num_per_page:%x\n", p_onfi->prog_num_per_page);	
	debug("partial_prog_attr:%x\n", p_onfi->partial_prog_attr);	
	debug("bits_ecc:%x\n", p_onfi->bits_ecc);	
	debug("intv_addr_bits:%x\n", p_onfi->intv_addr_bits);	
	debug("intv_attr:%x\n", p_onfi->intv_attr);	

	// byte[128, 163], Electrical parameters block
	debug("io_pin_cap:%x\n", p_onfi->io_pin_cap);
	debug("timing_mode:%x\n", get_16_bits_val(p_onfi->timing_mode));
	debug("prog_cache_timing_mode:%x\n", get_16_bits_val(p_onfi->prog_cache_timing_mode));
	debug("max_t_prog:%x\n", get_16_bits_val(p_onfi->max_t_prog));
	debug("max_t_bers:%x\n", get_16_bits_val(p_onfi->max_t_bers));
	debug("max_t_r:%x\n", get_16_bits_val(p_onfi->max_t_r));
	debug("min_t_ccs:%x\n", get_16_bits_val(p_onfi->min_t_ccs));

	// byte[164, 255], Vendor block
	debug("crc:%x\n", get_16_bits_val(p_onfi->crc));
	crc = onfi_crc16_cpu_calc(p_data, (ONFI_SIZE - 2));
	debug("crc calc:%x\n", crc);
}


