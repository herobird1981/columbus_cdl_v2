/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"
#include "cdl_usb.h"
#include "usblib.h"

#if 0
int ctrlc (void)
{
	if (console_tstc()) {
		switch (console_getc ()) {
		case 0x03:		/* ^C - Control C */
			return 1;
		default:
			break;
		}
	}
	return 0;
}

int board_usb_init(int index, enum usb_init_type init)
{
	return 0;
}
int board_usb_cleanup(int index, enum usb_init_type init)
{
	return 0;
}
int g_dnl_board_usb_cable_connected(void)
{
	return 1;
}

int do_fastboot(int argc, char* argv[])
{
	int controller_index;
	int ret;

	if (argc < 2) {
		info("cmd_format: %s controller_idx\n",argv[0]);
		return 1;
	}

	controller_index = simple_strtoul((const char*)argv[1], NULL, 10);

	ret = board_usb_init(controller_index, USB_INIT_DEVICE);
	if (ret) {
		info("USB init failed: %d", ret);
		return 1;
	}

	g_dnl_clear_detach();
	ret = g_dnl_register("usb_dnl_fastboot");
	if (ret) {
		info("register fastboot dev fail\n");
		return ret;
	}

	if (!g_dnl_board_usb_cable_connected()) {
		info("\rUSB cable not detected.\n" \
		     "Command exit.\n");
		ret = 1;
		goto exit;
	}

	while (1) {
		if (g_dnl_detach())
			break;
		if (ctrlc())
			break;
		usb_gadget_handle_interrupts(controller_index);
	}

	ret = 0;

exit:
	g_dnl_unregister();
	g_dnl_clear_detach();
	board_usb_cleanup(controller_index, USB_INIT_DEVICE);

	return ret;
}
#endif