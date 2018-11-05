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

#ifndef __MUSB_NONE_BOARD_H__
#define __MUSB_NONE_BOARD_H__

#include "mu_arch.h"
#include "hw_maps.h"
#include "hw_interrupt.h"
#include "timer.h"

/*
 * AFS-specific controller list
 * $Revision: 1.6 $
 */

MUSB_NoneController MUSB_aNoneController[] =
{
#ifdef MUSB_HDRC
    { MUSB_CONTROLLER_HDRC, (void*)0x14ab, 0x0010, TRUE },
#endif
#ifdef MUSB_MHDRC
    { MUSB_CONTROLLER_MHDRC, (void*)USB0_BASE, INT_USB0, FALSE },
#endif
#ifdef MUSB_HSFC
    { MUSB_CONTROLLER_HSFC, (void*)0x14ab, 0x0010, TRUE },
#endif
#ifdef MUSB_FDRC
    { MUSB_CONTROLLER_FDRC, (void*)0xc0000000, 9, FALSE },
#endif
};

#endif	/* multiple inclusion protection */
