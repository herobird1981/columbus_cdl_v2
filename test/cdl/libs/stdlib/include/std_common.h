/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __STD_COMMON_H__
#define __STD_COMMON_H__

#include <stdarg.h>

/* command line process */
#define CMDLINE_BAD_CMD 				(-1)
#define CMDLINE_TOO_MANY_ARGS 	(-2)
#define CMDLINE_TOO_FEW_ARGS	 (-3)
#define CMDLINE_INVALID_ARG 	(-4)
#define CMDLINE_QUIT					(-5)

typedef int ( *pfn_cmdline )( int argc, char* argv[] );

typedef struct
{
	const char* cmd;
	pfn_cmdline	pfn_cmd;
	const char* help;
} cmdline_entry;

int cmdline_process( char* cmd, cmdline_entry* entry );
int cmdline_help_general( cmdline_entry* entry );

int cmdline_to_argv( char* cmd,int* argc,char** argv);
unsigned long get_arg_ulong(const char* buf);

/* for CRC 32 */
unsigned long crc32( unsigned long crc, unsigned char* buff, int len );
unsigned short crc16(unsigned char *buf, int len);
unsigned short crc16_kermit(unsigned char *buf, int len);

/* string */
char* strcpy( char*, const char* );
char* strncpy( char*, const char*, unsigned int );
char* strcat( char*, const char* );
char* strncat( char*, const char*, unsigned int );
int strcmp( const char*, const char* );
int strncmp( const char*, const char*, unsigned int );
unsigned int strlen( const char* );

void* memset( void*, int, unsigned int );
void* memcpy( void*, const void*, unsigned int );
int memcmp( const void*, const void*, unsigned int );

/*vsprintf*/
unsigned long simple_strtoul( const char* cp, char** endp, unsigned int base );
int vsprintf( char* buf, const char* fmt, va_list args );
int vsnprintf( char* buf, unsigned int size, const char* fmt, va_list args );
int vscnprintf( char* buf, unsigned long size, const char* fmt, va_list args );
int sprintf( char* buf, const char* fmt, ... );

unsigned long long cb_lldiv( unsigned long long dividend, unsigned long divisor );

/*random*/
void srand( unsigned int seed );
unsigned int cb_rand( void );

void mem_malloc_init(unsigned long start, unsigned long size);
//void *malloc(unsigned int size);
//void free(void* p);

void print_buffer(char*name,char* buf,int len);
#endif
