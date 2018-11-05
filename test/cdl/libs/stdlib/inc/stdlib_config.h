/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __STDLIB_CONFIG_H__
#define __STDLIB_CONFIG_H__

#define CONFIG_SYS_PBSIZE		512
#define CONFIG_PANIC_HANG
#define ARCH_DMA_MINALIGN	64

extern void __assert_fail( const char* assertion, const char* file, unsigned line,
	const char* function );

#define assert(x) \
	({ if (!(x) && DEBUG) \
		__assert_fail(#x, __FILE__, __LINE__, __func__); })

#define debug(...)	console_printf (__VA_ARGS__)

/* dependence on external functions*/
extern int console_printf( const char* fmt, ... );
extern void console_puts( const char* s );
extern void console_putc( const char c );
extern int console_gets(char* buf, int len);
extern char console_getc(void);

#define puts console_puts
#define putc console_putc
#define printf console_printf
#define gets console_gets
#define getc console_getc

#endif