#ifndef __USBLIB_TYPES_H__
#define __USBLIB_TYPES_H__

typedef unsigned long int uintptr_t;

typedef struct bd_info {
	int aa;
}bd_t;

#define WATCHDOG_RESET() {}

#define ROUND(a,b)	(((a) + (b) - 1) & ~((b) - 1))

#endif
