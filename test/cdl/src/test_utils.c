/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"

unsigned char test_send_buff[TEST_BUFFER_SIZE];
unsigned char test_recv_buff[TEST_BUFFER_SIZE];

void print_hex_dump(const char* what, const unsigned long group, const void *p1, const unsigned long len)
{
	const unsigned char* p = p1;
	unsigned long g;
	unsigned long x;
	info("%s contents: \n", what);
	for (x = 0; x < len; ) {
		if (!(x % 16)) {
			info("%04lx: ", (unsigned long) p+x);
		}
		for(g=group; g>0; g--) {
			if(x+g-1 >= len)
				continue;
			info("%02x", p[x+g-1]);
		}
		info(" ");
		x += group;
		if (group ==1 && !(x % 4) && (x % 16)) {
			info("  ");
		}
		if (!(x % 16)) {
			info("\n");
		}
	}
	info("\n");
}

/*!*************************************************************************//**
*  @brief    dump_memory32
*  @details  dump memory in 32bit mode,the caller need make sure the memory address and range are legal!!!
*  @param    addr   -  unsigned long Is the start memory address, must be 4bytes alignment.
*  @param    len    -  unsigned long dump length in 32bit unit.
*
*  @return   none
*  @date     April 7, 2017
*  @author   Edbert Hu
******************************************************************************/
void dump_memory32(unsigned long addr, unsigned long len)
{
	int i = 0;
	unsigned long *p_buf = (unsigned long *)addr;

	if((addr%4) != 0)
	{
		info("Warning: start address must be 4B align!\r\n");
		return;
	}
	
	info("==========dump start===========\r\n");
	
	info("dump memory from 0x%x, length(32bits unit) is 0x%x\r\n", addr, len);
	for(i=0; i<len; i++)
	{
		if((i%4) == 0)
			info("\r\n");		
		info("0x%08x	", *(p_buf+i));
	}

	info("\r\n==========dumpend============\r\n");
}


#define noinline __attribute ((noinline))
unsigned long noinline cache_test_mul2(unsigned long idx) {
	idx %= 18;
	switch (idx/2) {
	case 0:
		idx += 1;
		break;
	case 1:
		idx += 3;
		break;
	case 2:
		idx += 5;
		break;
	case 3:
		idx += 1;
		break;
	case 4:
		idx += 2;
		break;
	case 5:
		idx += 7;
		break;
	case 6:
		idx += 4;
		break;
	case 7:
		idx += 9;
		break;
	case 8:
		idx += 6;
		break;
	default:
		break;
	}
	return idx;
}

#define noinline __attribute ((noinline))
unsigned long noinline cache_test_fill(unsigned long idx) {
	idx %= 9;
	switch (idx) {
	case 0:
		idx *= 1;
		break;
	case 1:
		idx *= 3;
		break;
	case 2:
		idx *= 5;
		break;
	case 3:
		idx *= 1;
		break;
	case 4:
		idx *= 2;
		break;
	case 5:
		idx *= 7;
		break;
	case 6:
		idx *= 4;
		break;
	case 7:
		idx *= 9;
		break;
	case 8:
		idx *= 6;
		break;
	default:
		break;
	}
	return cache_test_mul2(idx);
}

unsigned long noinline test_icache_perf(void) {
	unsigned long long start;
	unsigned long long ms_elapsed;
	unsigned long high, low;
	unsigned long i = 0;
	unsigned long ret = 1;

	start = get_timer(0);
#if 1
	while (i++ < 1000000) {
		ret += cache_test_fill(i);
	}
#else
	mdelay(100);
#endif
	ms_elapsed = get_timer(start);
	high = (ms_elapsed >> 32);
	low = (ms_elapsed & 0xFFFFFFFF);
	if (high)
		info("%s time used 0x%x_%x ms\n", __func__, high, low);
	else
		info("%s time used %ld ms\n", __func__, low);
	return ret;
}
