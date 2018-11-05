/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef _ONFI_H_
#define _ONFI_H_
#include "cdl_config.h"
#include "cdl_sys.h"
#include "cdl_types.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// define ONFI marker
#define ONFI_MARKER		0x49464E4F

// NAND oob data size(byte)
#define NAND_OOB_SIZE	8

// ONFI structure size
#define ONFI_SIZE		(sizeof(onfi_t))

// ONFI page parameter's copy number
#define ONFI_COPY_NUM	3

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
// NFC parameter page data structure definition
typedef struct
{
	// byte[0, 31], Revision information and features block
	uint32_t sig;						// byte[0, 3], Parameter page signature 
										// 			[3], 0x49, 'I'
										// 			[2], 0x46, 'F'
										// 			[1], 0x4E, 'N'
										// 			[0], 0x4F, 'O'										

	uint16_t rev_num;					// byte[4, 5], Revision number
										// 			[2-15], Reserved(0)
										// 			[1], supports ONFI version 1.0
										// 			[0], Reserved(0)
										
	uint16_t fea_sup;					// byte[6, 7], Features supported
										// 			[5-15],  Reserved (0) 
										// 			[4], 1 = supports odd to even page Copyback
										// 			[3], 1 = supports interleaved operations
										// 			[2], 1 = supports non-sequential page programming
										// 			[1], 1 = supports multiple LUN operations 
										// 			[0], 1 = supports 16-bit data bus width

	uint16_t cmd_sup;					// byte[8, 9], Optional commands supported
										// 			[6-15], Reserved (0)
										// 			[5], 1 = supports Read Unique ID
										// 			[4], 1 = supports Copyback 
										// 			[3], 1 = supports Read Status Enhanced
										// 			[2], 1 = supports Get Features and Set Features
										// 			[1], 1 = supports Read Cache commands  
										// 			[0], 1 = supports Page Cache Program command									

	uint8_t revd[22];					// byte[10, 31], Reserved (0) 

	// byte[32, 79], Manufacturer information bloc
	uint8_t dev_manu[12];				// byte[32, 43], Device manufacturer (12 ASCII characters)
	
	uint8_t dev_model[20];				// byte[44, 63], Device model (20 ASCII characters) 
	
	uint8_t manu_id;					// byte[64],      JEDEC manufacturer ID 
	
	uint8_t data_code[2];				// byte[65, 66], Date code
	
	uint8_t manu_revd[13];				// byte[67, 79], Reserved (0) 
	
	// byte[80, 127], Memory organization block
	uint8_t bytes_per_page[4];			// byte[80, 83], Number of data bytes per page
	
	uint8_t bytes_per_spare[2];			// byte[84, 85], Number of spare bytes per page
	
	uint8_t bytes_per_partial_page[4];	// byte[86, 89], Number of data bytes per partial page
	
	uint8_t bytes_per_partial_spare[2];	// byte[90, 91], Number of spare bytes per partial page 
	
	uint8_t pages_per_blk[4];			// byte[92, 95], Number of pages per block
	
	uint8_t blks_per_lun[4];			// byte[96, 99], Number of blocks per logical unit (LUN)
	
	uint8_t num_luns;					// byte[100],     Number of logical units (LUNs)
	
	uint8_t addr_cycle;					// byte[101],     Number of address cycles
										// 			  4-7, Column address cycles
										// 			  0-3, Row address cycles
																				
	uint8_t bits_per_cell;				// byte[102], 	    Number of bits per cell
	
	uint8_t max_bad_blk_per_lun[2];		// byte[103, 104], Bad blocks maximum per LUN
	
	uint8_t blk_endurance[2];			// byte[105, 106], Block endurance
	
	uint8_t valid_blks_guaranteed;		// byte[107], 	    Guaranteed valid blocks at beginning of target 
	
	uint8_t valid_blk_endurance[2];		// byte[108, 109], Block endurance for guaranteed valid blocks
	
	uint8_t prog_num_per_page;			// byte[110],        Number of programs per page
	
	uint8_t partial_prog_attr;			// byte[111], 	    Partial programming attributes
										// 			    5-7: Reserved
										// 			    4    : 1 = partial page layout is partial page data followed by partial page spare
										// 			    1-3 : Reserved
										// 			    0     : 1 = partial page programming has constraints
										
	uint8_t bits_ecc;					// byte[112], 	    Number of bits ECC correctability
										//			    This field indicates the number of bits that the host should be able to correct per 512 bytes of data.
										
	uint8_t intv_addr_bits;				// byte[113], 	    Number of interleaved address bits
										//			    4-7, Reserved (0)
										//			    0-3, Number of interleaved address bits								
		
	uint8_t intv_attr;					// byte[114], 	    Interleaved operation attributes
										// 			   	4-7: Reserved (0) 
										// 				3    : Address restrictions for program cache 
										// 				2    : 1 = program cache supported 
										// 				1    : 1 = no block address restrictions 
										// 				0    : Overlapped / concurrent interleaving support 
				
	uint8_t mem_revd[13];				// byte[115-127], Reserved (0) 	

	// byte[128, 163], Electrical parameters block
	uint8_t io_pin_cap; 				// byte[128], 	    I/O pin capacitance
	
	uint8_t timing_mode[2];				// byte[129, 130], Timing mode support 
										// 				6-15: Reserved (0) 
										// 				5     : 1 = supports timing mode 5 
										// 				4     : 1 = supports timing mode 4 
										// 				3     : 1 = supports timing mode 3 
										// 				2     : 1 = supports timing mode 2 
										// 				1     : 1 = supports timing mode 1 
										// 				0     : 1 = supports timing mode 0, shall be 1 

	uint8_t prog_cache_timing_mode[2];	// byte[131, 132], Program cache timing mode support
										// 				6-15: Reserved (0) 
										// 				5     : 1 = supports timing mode 5 
										// 				4     : 1 = supports timing mode 4 
										// 				3     : 1 = supports timing mode 3 
										// 				2     : 1 = supports timing mode 2 
										// 				1     : 1 = supports timing mode 1 
										// 				0     : 1 = supports timing mode 0 
										
	uint8_t max_t_prog[2];				// byte[133, 134],  tPROG Maximum page program time (us) 

	uint8_t max_t_bers[2];				// byte[135, 136], tBERS Maximum block erase time (us)

	uint8_t max_t_r[2];					// byte[137, 138], tR Maximum page read time (us) 

	uint8_t min_t_ccs[2];				// byte[139, 140], tCCS Minimum change column setup time (ns) 

	uint8_t elec_revd[23];				// byte[141, 163], Reserved (0)

	// byte[164, 255], Vendor block
	uint8_t vendor_rev[2];				// byte[164, 165], Vendor specific Revision number
	
	uint8_t vendor_spec[88];			// byte[166, 253], Vendor specific
	
	uint8_t crc[2];						// byte[254, 255], Integrity CRC
} onfi_t;

typedef onfi_t * onfi_t_ptr;

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
uint16_t onfi_crc16_cpu_calc(uint8_t *p_data, int32_t size);

void onfi_para_page_print(uint8_t *p_data);

#endif

