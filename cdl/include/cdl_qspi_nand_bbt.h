/**********************************************************************************************************/
//
//  Filename :          qspi_nand_bbt.h
//
//  COPYRIGHT, 2016
//  The contents of this file is the sole proprietary property of, confidential
//  to, and copyrighted by Brite Company.  It may not be copied or reproduced
//  in any manner or form without the prior written consent of Brite Company.
//
//  Original Author:    Billy
//  Date created:       Feb 14, 2017
//
//  Description:
//      define qspi nand bbt basic operation
//
//  Revision:
//  Date       		Author      	Description
//  02/14/17    	Billy   		Original
//
/*************************************************************************************************************/
#ifndef _QSPI_NAND_BBT_H_
#define _QSPI_NAND_BBT_H_
#include "cdl_sys.h"
#include "cdl_qspi_nand.h"
#include "cdl_bbt.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/


/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
BOOL qspi_bbt_va_to_pa(bbt_t *p_bbt, uint32_t va, uint32_t *p_pa);

BOOL qspi_bbt_pa_to_va(bbt_t *p_bbt, uint32_t pa, uint32_t *p_va);

uint8_t qspi_bbt_scan(bbt_t *p_bbt);

uint8_t qspi_bbt_write(bbt_t *p_bbt);

uint8_t qspi_bbt_read(bbt_t *p_bbt);

void qspi_bbt_blk_marked(bbt_t *p_bbt, uint32_t blk, E_BBT_TYPE type);

E_BBT_TYPE qspi_bbt_blk_verify(bbt_t *p_bbt, uint32_t blk);

uint8_t qspi_nand_bbt_test(void);

#endif

