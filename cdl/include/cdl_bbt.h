/**********************************************************************************************************/
//
//  Filename :          bbt.h
//
//  COPYRIGHT, 2016
//  The contents of this file is the sole proprietary property of, confidential
//  to, and copyrighted by Brite Company.  It may not be copied or reproduced
//  in any manner or form without the prior written consent of Brite Company.
//
//  Original Author:    Billy
//  Date created:       May 27, 2016
//
//  Description:
//      define nfc bbt basic operation
//
//  Revision:
//  Date       		Author      	Description
//  05/27/16    	Billy   		Original
//
/*************************************************************************************************************/
#ifndef _BBT_H_
#define _BBT_H_
#include "cdl_sys.h"
#include "cdl_types.h"
#include "cdl_nfc.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// BBT type
typedef enum
{
	BBT_TYPE_BAD  = 0,
	BBT_TYPE_REVD = 1,
	BBT_TYPE_WORN = 2,
	BBT_TYPE_GOOD = 3
}E_BBT_TYPE;

// bad block size(byte)
#define BBT_SIZE 	0x800	// 1 bit indicates 1 block, the size could handle max 16 blocks

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
typedef struct
{
	uint8_t str[4];					// BBT string
									//	= "Bbt0", in the last valid block
									//	= "1bbT", in the (last - 1) valid block
								
	uint8_t ver;					// BBT version
	
	uint8_t data[BBT_SIZE - 5 + 64];		// BBT data								
}bbt_t;
typedef bbt_t * bbt_t_ptr;

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern bbt_t g_bbt;

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/

#endif

