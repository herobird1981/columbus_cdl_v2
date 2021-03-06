/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2005              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_NONE_BOARD_CONFIG_H__
#define __MUSB_NONE_BOARD_CONFIG_H__

/*
 * AFS-specific board configuration
 * $Revision: 1.5 $
 */
#include <console.h>

#define MUSB_VBUS_ERROR_RETRIES 4

#define MUSB_MAX_CONTROLLERS	1

#define	MUSB_TS_SESSREQ		8	/* SRP, WAIT_A_BCON, WAIT_B_ACON */
#define	MUSB_MAX_TRANSITION_TM	250	/* 250 msec for transition coalesce */
#define MUSB_TB_SRP_FAIL	5

/*sunny add */
#define MUSB_MHDRC
/*#define MUSB_FORCE_FULLSPEED*/

#define MUSB_DIAG 2
#define MUSB_DISABLE_BULK_COMBINE_SPLIT

/*read configdata register to decide define it or not*/
#if 0  /*for dynamic fifo alloc*/
#define MUSB_C_DYNFIFO_DEF
#define MUSB_DISABLE_DB_DYNFIFO
#endif

/*#define SET_ADDR_FIRST*/

#define info(...)	console_printf(__VA_ARGS__)

#endif	/* multiple inclusion protection */
