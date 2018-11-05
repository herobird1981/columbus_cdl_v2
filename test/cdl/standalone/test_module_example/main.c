/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"
#include "timer.h"
#include "std_common.h"

#define CMD_BUF_SIZE		64
static char g_cmd_buf[CMD_BUF_SIZE];

#if (CONFIG_DMA_TEST == 1)
extern cmdline_entry dma_test_menu[];
extern int dma_test_prepare( int );
#else
#define dma_test_menu NULL
#define dma_test_prepare NULL
#endif


int one_device_test( cmdline_entry* test_entry )
{
	int ret;

	//display the help,list all support test command for the device
	ret = cmdline_process( "help", test_entry );

	while ( 1 )
	{
		//
		// Print a prompt to the console.  Show the CWD.
		//
		console_printf( "\n%s> ", "cmd:" );

		//
		// Get a line of text from the user.
		//
		console_gets( g_cmd_buf, sizeof( g_cmd_buf ) );

		//
		// Pass the line from the user to the command processor.
		// It will be parsed and valid commands executed.
		//
		ret = cmdline_process( g_cmd_buf, test_entry );

		//
		// Handle the case of bad command.
		//
		if ( ret == CMDLINE_QUIT )
		{
			console_printf( "Quit Module test!\n" );
			break;
		}
		else if ( ret == CMDLINE_BAD_CMD )
		{
			console_printf( "Bad command!\n" );
		}
			//
			// Handle the case of too many arguments.
			//
		else if ( ret == CMDLINE_TOO_MANY_ARGS )
		{
			console_printf( "Too many arguments for command processor!\n" );
		}
			//
			// Otherwise the command was executed.	Print the error
			// code if one was returned.
			//
		else if ( ret != 0 )
		{
			console_printf( "Command returned error code %d\n", ret );
		}
	}
	return 0;
}

int main( void )
{
	//init console first
	console_init( CONSOLE_BASE, CONSOLE_PCLK );

	debug( "\n" );
	debug( "test pattern for DMAC\n" );

	one_device_test( dma_test_menu );

	debug( "test pattern done\n" );
	while ( 1 );
	return 0;
}


