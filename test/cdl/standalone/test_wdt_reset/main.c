/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"

#define CMD_BUF_SIZE		64
static char g_cmd_buf[CMD_BUF_SIZE];

#if (CONFIG_WDT_TEST ==1)
extern cmdline_entry wdt_test_menu[];
#else
#define wdt_test_menu NULL
#endif

extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;
extern unsigned long _esvc_stack;

void init_data_area(void)
{
	unsigned long *src, *dst;

	/*fill data area*/
	src = (unsigned long*)&_etext;
	for(dst = (unsigned long*)&_data; dst < (unsigned long*)&_edata; ) {
	      *dst++ = *src++;
	}

	/*zero bss*/
	for(dst = (unsigned long*)&_bss; dst < (unsigned long*)&_esvc_stack; ) {
	      *dst++ = 0;
	}
}

int one_device_test( cmdline_entry* test_entry )
{
	int ret;

	//display the help,list all support test command for the device
	//ret = cmdline_process( "help", test_entry );

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
	init_data_area();

	//init console first
	console_init( CONSOLE_BASE, CONSOLE_PCLK );

  timer_init(TIMER_BASE,TIMER_PCLK,SYS_TIMER);
	debug( "\n" );
	debug( "test pattern for WDT reset\n" );

	info("please input 'wdt_reset 0xffff 3' to trigger watch dog\n");

	one_device_test( wdt_test_menu );

	debug( "test pattern done\n" );
	while ( 1 );
	return 0;
}


