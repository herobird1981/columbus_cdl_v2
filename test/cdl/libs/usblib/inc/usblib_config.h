#ifndef __USBLIB_CONFIG_H__
#define __USBLIB_CONFIG_H__

#include "usblib_types.h"

/*#define DEBUG_OUTPUT*/
#ifdef 	DEBUG_OUTPUT
#define debug(...)	console_printf (__VA_ARGS__)
#else
#define debug(...)
#endif
#define info(...)	console_printf(__VA_ARGS__)

#define CONFIG_USB_GADGET_DOWNLOAD
#define CONFIG_USB_FUNCTION_THOR
#define CONFIG_USB_FUNCTION_DFU
#define CONFIG_USB_FUNCTION_MASS_STORAGE
#define CONFIG_USB_FUNCTION_FASTBOOT
#define CONFIG_USB_ETHER
#define CONFIG_USB_GADGET_VBUS_DRAW	2
#define CONFIG_USB_MUSB_PIO_ONLY
#define CONFIG_USB_MUSB_DISABLE_BULK_COMBINE_SPLIT  /*sunny add*/

//#define ARCH_DMA_MINALIGN	64
#define CONFIG_SYS_CACHELINE_SIZE	32
#define CONFIG_ARM

#define CONFIG_USB_MUSB_GADGET
#define CONFIG_USB_MUSB_HOST
#define CONFIG_PARTITIONS
#define HAVE_BLOCK_DEVICE


#define CONFIG_G_DNL_VENDOR_NUM		0x03FD
#define CONFIG_G_DNL_PRODUCT_NUM	0x0300
#define CONFIG_G_DNL_MANUFACTURER	"Xilinx"



#define CONFIG_FASTBOOT_BUF_ADDR	0x1000000
#define CONFIG_FASTBOOT_BUF_SIZE	0x1000000

#define CONFIG_USB_STORAGE
#define CONFIG_USB_GADGET_DUALSPEED

#define CONFIG_SYS_HZ	1000

extern void mdelay(unsigned long msec);
/* dependence on external functions*/
extern int console_printf( const char* fmt, ... );
extern void console_puts( const char* s );
extern void console_putc( const char c );
extern char console_getc(void);
extern int console_tstc(void);
#define puts console_puts
#define putc console_putc
#define printf console_printf

#define error info
#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif
#define debug_cond(cond, fmt, args...)			\
	do {						\
		if (cond)				\
			printf(pr_fmt(fmt), ##args);	\
	} while (0)

extern unsigned long long get_timer(unsigned long long start);
extern unsigned int rand_r( unsigned int* seedp );
extern unsigned long simple_strtoul( const char* cp, char** endp, unsigned int base );
extern int ctrlc(void);
extern int run_command(const char *cmd, int flag);
extern void udelay(unsigned long usec);
extern int checkboard(void);
extern char *getenv(const char *name);
#endif

