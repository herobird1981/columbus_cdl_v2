/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CONSOLE_H__
#define __CONSOLE_H__

void console_init(unsigned long base, unsigned long pclk);
void console_reinit_baud(unsigned long pclk);
unsigned long console_baud(void);
void console_flush(void);

int console_tstc(void);
void console_putc(const char c);
char console_getc(void);

void console_puts(const char* s);
int console_gets(char* buf, int len);

int console_printf(const char* fmt, ...);

int console_check_char(unsigned char check);

#endif
