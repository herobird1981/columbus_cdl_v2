#include "usblib_config.h"

#include <asm/io.h>

#include "cdl_usb.h"
#include "usblib.h"

#include <common.h>
#include <command.h>
#include <console.h>
#include <part.h>
#include <usb.h>
#include <part.h>
#include <usb.h>

extern char usb_started;

static int usb_stor_curr_dev = -1; /* current device */

int musb_board_init(struct musb_controller *dev);
void musb_board_exit(void);

int usb_host_init(struct musb_controller *dev)
{
	dev->usb_mode = 1; /*host*/
	return musb_board_init(dev);
}

void usb_host_exit(void)
{
	musb_board_exit();
}

static int do_usb_host_start(void)
{
	block_dev_desc_t *stor_dev;

	if (usb_init() < 0)
			return -1;

	/* try to recognize storage devices immediately */
	usb_stor_curr_dev = usb_stor_scan(1);
	if (usb_stor_curr_dev < 0) {
		info("no current device selected\n");
		return -1;
	}

	usb_show_tree();

	stor_dev = usb_stor_get_dev(usb_stor_curr_dev);
	info("vendor %s: max lba = %d blk_size = %d\n",
		stor_dev->vendor, stor_dev->lba,stor_dev->blksz);

	return 0;

}
int usb_host_start(void)
{
	if (usb_started)
		return 0; /* Already started */

	info("starting USB...\n");

	return do_usb_host_start();
}

int usb_host_stop(void)
{
	info("stopping USB..\n");
	usb_stop();

	return 0;
}

int usb_host_reset(void)
{
	info("resetting USB...\n");
	usb_stop();

	return do_usb_host_start();
}

int usb_host_write(unsigned char* buf, unsigned long start,unsigned long cnt)
{
	block_dev_desc_t *stor_dev;
	unsigned long n;

	if (!usb_started) {
		info("USB is stopped. Please issue 'usb start' first.\n");
		return -1;
	}
	if (usb_stor_curr_dev < 0) {
		info("no current device selected\n");
		return -1;
	}
	stor_dev = usb_stor_get_dev(usb_stor_curr_dev);
	if ((start >= stor_dev->lba) || (start+cnt >= stor_dev->lba)) {
		info("wrong arg,%d %d exceed max lba %d\n",
			start,cnt,stor_dev->lba);
		return -1;
	}

	debug("\nUSB write: device %d block # %ld, count %ld"
				" ... ", usb_stor_curr_dev, start, cnt);
	n = stor_dev->block_write(stor_dev, start, cnt,buf);
	debug("%ld blocks write: %s\n", n,
		(n == cnt) ? "OK" : "ERROR");
	if (n == cnt)
		return 0;
	return -1;
}
int usb_host_read(unsigned char* buf, unsigned long start,unsigned long cnt)
{
	block_dev_desc_t *stor_dev;
	unsigned long n;

	if (!usb_started) {
		info("USB is stopped. Please issue 'usb start' first.\n");
		return -1;
	}

	if (usb_stor_curr_dev < 0) {
		info("no current device selected\n");
		return -1;
	}
	stor_dev = usb_stor_get_dev(usb_stor_curr_dev);
	if ((start >= stor_dev->lba) || (start+cnt >= stor_dev->lba)) {
		info("wrong arg,%d %d exceed max lba %d\n",
			start,cnt,stor_dev->lba);
		return -1;
	}

	debug("\nUSB read: device %d block # %ld, count %ld"
		" ... ", usb_stor_curr_dev, start, cnt);
	n = stor_dev->block_read(stor_dev, start, cnt,buf);
	debug("%ld blocks read: %s\n", n,
		(n == cnt) ? "OK" : "ERROR");
	if (n == cnt)
		return 0;
	return -1;
}

