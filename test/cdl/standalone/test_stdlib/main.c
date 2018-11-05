/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

#include "std_common.h"

int test1;
int test2;
int test3 = 55;

int main( void )
{
	char abuf[128];
	char bbuf[128];
	int aa = 302;
	int b = 3;
	int c = 0;
	int d = 0;
	char *buf;

	/*demo string lib*/
	strcpy( abuf, "12345678" );
	memcpy( bbuf, abuf, strlen( abuf ) );

	/*demo * and / supported*/
	c = aa / b;
	d = aa * b;
	memcpy( abuf, &aa, 4 );
	memcpy( bbuf, &b, 4 );
	memcpy( bbuf, &c, 4 );
	memcpy( bbuf, &d, 4 );
	test3 = 22;
	test1 = 11;

	sprintf( abuf, "test3 is %d\n", test3 );

	mem_malloc_init(0x100000,0x1000000);

	buf = malloc(1000);
	free(buf);

	console_puts("done\n");
	while ( 1 );
	return 0;
}

