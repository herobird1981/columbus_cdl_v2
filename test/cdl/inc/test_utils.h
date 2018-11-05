/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#define TEST_BUFFER_SIZE		(1024*128)
extern unsigned char test_send_buff[TEST_BUFFER_SIZE];
extern unsigned char test_recv_buff[TEST_BUFFER_SIZE];

void print_hex_dump(const char* what, const unsigned long group, const void *p1, const unsigned long len);
extern void dump_memory32(unsigned long addr, unsigned long len);
#endif
