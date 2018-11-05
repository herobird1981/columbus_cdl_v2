/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_dma.h"

#if (CONFIG_DMA_TEST == 1)

struct dw_dmac dma_dev;
struct dw_dmac* p_dma_dev = &dma_dev;

cmdline_entry dma_test_menu[];

static int cmd_help( int argc, char* argv[] )
{
	return cmdline_help_general( &dma_test_menu[0] );
}
static int test_case1( int argc, char* argv[] )
{
	debug( "dmac test case1\n" );
	return 0;
}
static int test_case2( int argc, char* argv[] )
{
	debug( "dmac test case2\n" );
	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry dma_test_menu[] =
{
	{ "help", 	cmd_help, 		 " : Display list of commands" },
	{ "h",			cmd_help, 	"    : alias for help" },
	{ "?",			cmd_help, 	"    : alias for help" },
	{ "tc1",	test_case1, 		 "   : test case1" },
	{ "tc2",		test_case2, 		 "  : test cas2" },
	{ 0, 0, 0 }
};

/* if return none zero, something wrong,please check and can't continue test*/
int dma_test_prepare( int sel )
{
	int ret = 0;

	memset( p_dma_dev, 0, sizeof( struct dw_dmac ) );
	if ( sel == 0 )
	{
		p_dma_dev->regs = ( void* )DMAC_BASE;
	}
	else
	{
		console_printf( "wrong device id\n" );
		ret = 1;
	}

	return ret;
}
#endif