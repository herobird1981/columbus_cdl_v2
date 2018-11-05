/*
 * MUSB OTG driver u-boot specific functions
 *
 * Copyright © 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef __MUSB_UBOOT_H__
#define __MUSB_UBOOT_H__

#include <usb.h>
#include "linux-compat.h"
#include "usb-compat.h"
#include "musb_core.h"

struct musb_host_data {
	struct musb *host;
	struct usb_hcd hcd;
	enum usb_device_speed host_speed;
	struct usb_host_endpoint hep;
	struct urb urb;
};

extern struct dm_usb_ops musb_usb_ops;

int musb_lowlevel_init(struct musb_host_data *host);
int usb_lowlevel_stop(int index);
int usb_lowlevel_init(int index, enum usb_init_type init, void **controller);
int usb_reset_root_port(struct usb_device *dev);

int submit_bulk_msg(struct usb_device *dev, unsigned long pipe,
			    void *buffer, int length);
int submit_control_msg(struct usb_device *dev, unsigned long pipe,
		       void *buffer, int length, struct devrequest *setup);
int submit_int_msg(struct usb_device *dev, unsigned long pipe,
		   void *buffer, int length, int interval);

struct int_queue *create_int_queue(struct usb_device *dev,
		unsigned long pipe, int queuesize, int elementsize,
		void *buffer, int interval);
void *poll_int_queue(struct usb_device *dev, struct int_queue *queue);
int destroy_int_queue(struct usb_device *dev, struct int_queue *queue);

int usb_gadget_handle_interrupts(int index);
int usb_gadget_register_driver(struct usb_gadget_driver *driver);
int usb_gadget_unregister_driver(struct usb_gadget_driver *driver);

int musb_register(struct musb_hdrc_platform_data *plat, void *bdata,
			void *ctl_regs);
void musb_unregister(struct musb_hdrc_platform_data *plat);

#endif
