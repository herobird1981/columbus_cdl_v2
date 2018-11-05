/*
 * types.h
 *
 *  Created on: Aug 6, 2016
 *      Author: bobz
 */

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_


#include <stdint.h>
#include <stddef.h>

#if !defined(__CDL_TYPES_H__) && !defined(__TEST_TYPES_H__)
// for 32 bit machine
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef unsigned long long	u64;
typedef int	bool;
#define true	1UL
#define false	0UL
#endif

#endif /* SRC_TYPES_H_ */
