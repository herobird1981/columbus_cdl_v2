/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "drivers_config.h"

#if (CONFIG_USE_HW_CONSOLE ==1)
#include "cdl_uart.h"
#endif

#include "console.h"
#include "hw_maps.h"

#define CONSOLE_BUF_SIZE	4096
unsigned char console_buf[CONSOLE_BUF_SIZE];

struct dw_uart uart_console;
struct dw_uart* p_uart_console = &uart_console;
int current_console_base;
int console_switched = 0;


void console_init(unsigned long base, unsigned long pclk)
{
	current_console_base = base;
	memset(p_uart_console, 0, sizeof(struct dw_uart));
	p_uart_console->regs = (void*)base;
	p_uart_console->dma_mode = 0;

	dw_uart_init(p_uart_console, pclk, 115200); //115200,n,8,1
}

void console_reinit_baud(unsigned long pclk)
{
	p_uart_console->pclk = pclk;
	p_uart_console->div = dw_uart_calc_divisor(p_uart_console, pclk, p_uart_console->baudrate);
	dw_uart_setbrg(p_uart_console, p_uart_console->div);
}


int console_tstc(void)
{
	return dw_uart_tstc(p_uart_console);
}

void console_flush(void) {
	dw_uart_flush(p_uart_console);
}

unsigned long console_baud(void) {
	return p_uart_console->baudrate;
}

void console_putc(const char c)
{
	if (c == '\n') {
		dw_uart_putc(p_uart_console, '\r');
	}
	dw_uart_putc(p_uart_console, c);
}

char console_getc(void)
{
	return dw_uart_getc(p_uart_console);
}

void console_puts(const char* s)
{
	while (*s) {
		console_putc(*s++);
	}
}

#define CHAR_DEL	0x7F

/* get chars from console until newline('\n') received*/
int console_gets(char* buf, int len)
{
	unsigned long count = 0;
	char cc;
	static char last_was_cr = 0;

	/*
	 Adjust the length back by 1 to leave space for the trailing
	 null terminator.
	*/
	len--;

	/*
	 Process characters until a newline is received.
	*/
	while (1) {
		cc = console_getc();

		if (cc == '\b' || cc == CHAR_DEL) {
			/*
			 if the backspace key was pressed.
			 If there are any characters already in the buffer, then delete
			 the last.
			*/
			if (count) {
				dw_uart_write(p_uart_console, "\b \b", 3);
				count--;
			}
			continue;
		}

		/*
		If this character is LF and last was CR, then just gobble up the
		character because the EOL processing was taken care of with the CR.
		*/
		if ((cc == '\n') && last_was_cr) {
			last_was_cr = 0;
			continue;
		}

		/*
		 See if a newline or escape character was received.
		*/
		if ((cc == '\r') || (cc == '\n') || (cc == 0x1b)) {
			/*
			 If the character is a CR, then it may be followed by a LF which
			 should be paired with the CR.	So remember that a CR was
			 received.
			*/
			if (cc == '\r') {
				last_was_cr = 1;
			}

			break;
		}

		/*
		Process the received character as long as we are not at the end of
		the buffer.	If the end of the buffer has been reached then all
		additional characters are ignored until a newline is received.
		*/
		if (count < len) {
			buf[count] = cc;
			count++;
			/*echo on to user*/
			dw_uart_putc(p_uart_console, cc);
		}
	}

	/*
	 Add a null termination to the string.
	*/
	buf[count] = 0;

	/*
	 Send a CRLF pair to the terminal to end the line.
	*/
	dw_uart_write(p_uart_console, "\r\n", 2);

	return(count);
}

int console_printf(const char* fmt, ...)
{
	va_list args;
	unsigned long i;

	va_start(args, fmt);

	/*
	 * For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vscnprintf((char*)console_buf, CONSOLE_BUF_SIZE - 1, fmt, args);
	va_end(args);

	/* Print the string */
	console_buf[i] = 0;
	console_puts((const char*)console_buf);
	return i;
}

int printf(const char* fmt, ...) {
	va_list args;
	unsigned long i;

	va_start(args, fmt);

	/*
	 * For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	i = vscnprintf((char*)console_buf, CONSOLE_BUF_SIZE - 1, fmt, args);
	va_end(args);

	/* Print the string */
	console_buf[i] = 0;
	console_puts((const char*)console_buf);
	return i;
}

int console_check_char(unsigned char check)
{
	char cc;

	if (console_tstc()) {
		cc = console_getc();
		if (cc == check) {
			return 1;
		}
	}
	return 0;
}
