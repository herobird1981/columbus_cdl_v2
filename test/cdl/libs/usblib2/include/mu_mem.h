/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2006              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW memory abstraction.
 * $Revision: 1.7 $
 */

#ifndef __MUSB_MEMORY_H__
#define __MUSB_MEMORY_H__

//#include <malloc.h>
#include "mu_tools.h"
#include "memleak.h"
#include "linux/string.h"

#define MUSB_MemAlloc malloc
#define MUSB_MemRealloc realloc
#define MUSB_MemFree free

#define MUSB_MemCopy(a, b, c) memcpy((a), (b), (c))
#define MUSB_MemSet(a, b, c) memset((a), (b), (c))

#endif	/* multiple inclusion protection */
