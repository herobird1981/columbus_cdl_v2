/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "stdlib_config.h"

#include "std_common.h"

#ifndef CMDLINE_MAX_ARGS
#define CMDLINE_MAX_ARGS				16
#endif

static char *cmdline_argv[CMDLINE_MAX_ARGS + 1];
extern int console_switched;

/*return: argc*/
int cmdline_to_argv( char* cmd,int* argc,char** argv)
{
	char* p_cmd;
	int argc_num;
	int find_flag = 1;

	argc_num = 0;
	p_cmd = cmd;

	while ( *p_cmd )
	{
		//
		// If there is a space, then replace it with a zero, and set the flag
		// to search for the next argument.
		//
		if ( *p_cmd == ' ' )
		{
			*p_cmd = 0;
			find_flag = 1;
		}

		//
		// Otherwise it is not a space, so it must be a character that is part
		// of an argument.
		//
		else
		{
			//
			// If bFindArg is set, then that means we are looking for the start
			// of the next argument.
			//
			if ( find_flag )
			{
				//
				// As long as the maximum number of arguments has not been
				// reached, then save the pointer to the start of this new arg
				// in the argv array, and increment the count of args, argc.
				//
				if ( argc_num < CMDLINE_MAX_ARGS )
				{
					cmdline_argv[argc_num] = p_cmd;
					argc_num++;
					find_flag = 0;
				}
				else
				{
					return( CMDLINE_TOO_MANY_ARGS );
				}
			}
		}
		p_cmd++;
	}

	*argc =  argc_num;
	*argv = (char*)cmdline_argv;
	return( 0 );
}

int cmdline_process( char* cmd, cmdline_entry* entry )
{
	static char* argv[CMDLINE_MAX_ARGS + 1];
	char* p_cmd;
	int argc;
	int find_flag = 1;

	argc = 0;
	p_cmd = cmd;

	if (console_switched) {
		console_switched = 0;
		return CMDLINE_QUIT;
	}
	while ( *p_cmd )
	{
		//
		// If there is a space, then replace it with a zero, and set the flag
		// to search for the next argument.
		//
		if ( *p_cmd == ' ' )
		{
			*p_cmd = 0;
			find_flag = 1;
		}

		//
		// Otherwise it is not a space, so it must be a character that is part
		// of an argument.
		//
		else
		{
			//
			// If bFindArg is set, then that means we are looking for the start
			// of the next argument.
			//
			if ( find_flag )
			{
				//
				// As long as the maximum number of arguments has not been
				// reached, then save the pointer to the start of this new arg
				// in the argv array, and increment the count of args, argc.
				//
				if ( argc < CMDLINE_MAX_ARGS )
				{
					argv[argc] = p_cmd;
					argc++;
					find_flag = 0;
				}
				else
				{
					return( CMDLINE_TOO_MANY_ARGS );
				}
			}
		}
		p_cmd++;
	}

	//
	// If one or more arguments was found, then process the command.
	//
	if ( argc )
	{
		//check quit 'q' command
		if ( !strcmp( argv[0], "q" ) )
		{
			return CMDLINE_QUIT;
		}
		while ( entry->cmd )
		{
			if ( !strcmp( argv[0], entry->cmd ) )
			{
				return( entry->pfn_cmd( argc, argv ) );
			}
			entry++;
		}
	}
	return( CMDLINE_BAD_CMD );
}

int cmdline_help_general( cmdline_entry* entry )
{
	console_printf( "\nAvailable commands\n" );
	console_printf( "------------------\n" );

	while ( entry->cmd )
	{
		console_printf( "%s%s\n", entry->cmd, entry->help );

		entry++;
	}
	return( 0 );
}

unsigned long get_arg_ulong(const char* buf)
{
	unsigned long ret = 0;
	int base = 10;
	char *ptr = (char *)buf;
	int flag = 0;

	if (!strncmp(buf,"0x",2) || !strncmp(buf,"0X",2)) {
		base = 16;
		ptr = (char *)buf+2;
	}else if (!strncmp(buf,"-",1)) {
		ptr = (char *)buf+1;
		flag = 1;
	}
	ret = simple_strtoul((const char*)ptr, 0, base);
	if (flag)
		ret = -ret;

	return ret;
}