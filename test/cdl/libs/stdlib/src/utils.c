/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/
#include "stdlib_config.h"

void print_buffer(char*name,char* buf,int len)
{
	int i,j;
	int line;
	int rest;

	printf("%s(%d):\n",name,len);
	rest = len;
	for (i=0;i<len;i+=16) {
		if (!(i % 16)) {
		   printf("%04lx: ", (unsigned long)buf+i);
		}
		if (rest>16)
			line = 16;
		else
			line = rest;
		for (j=0;j<line;j++)
			printf("%02x ",*(buf+i+j));
		printf("\n");
		rest -= line;
	}
}

int abs(int x) {	return ((x > 0) ? x : -x); }
