/*
 *  linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */
#include "stdlib_config.h"

#include <common.h>
#if !defined(CONFIG_PANIC_HANG)
#include <command.h>
#endif

extern void hang(void);
int vprintf( const char* fmt, va_list args );

static void panic_finish(void) __attribute__ ((noreturn));

static void panic_finish(void)
{
	putc('\n');
#if defined(CONFIG_PANIC_HANG)
	hang();
#else
	udelay(100000);	/* allow messages to go out */
	do_reset(NULL, 0, 0, NULL);
#endif
	while (1)
		;
}

void panic_str(const char *str)
{
	puts(str);
	panic_finish();
}

void panic(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	panic_finish();
}