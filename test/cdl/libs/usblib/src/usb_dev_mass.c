#include "usblib_config.h"

#include <asm/io.h>

#include "cdl_usb.h"
#include "usblib.h"


#include <errno.h>
#include <common.h>
#include <command.h>
#include <console.h>
#include <g_dnl.h>
#include <part.h>
#include <usb.h>
#include <usb_mass_storage.h>

static char ums_name[64];

int musb_board_init(struct musb_controller *dev);
void musb_board_exit(void);

int usb_dev_init(struct musb_controller *dev)
{
	dev->usb_mode = 2; /*device*/
	return musb_board_init(dev);
}

void usb_dev_exit(void)
{
	musb_board_exit();
}

__weak int dev_mass_read_sector(ulong start, lbaint_t blkcnt, void *buf)
{
	return 0;
}

__weak int dev_mass_write_sector(ulong start, lbaint_t blkcnt, const void *buf)
{
	return 0;
}
__weak int dev_mass_init(void)
{
	return 0;
}
__weak unsigned long dev_mass_get_lba(void)
{
	return 0;
}
static int ums_read_sector(struct ums *ums_dev,
			   ulong start, lbaint_t blkcnt, void *buf)
{
	lbaint_t blkstart = start + ums_dev->start_sector;

	return dev_mass_read_sector(blkstart, blkcnt, buf);
}

static int ums_write_sector(struct ums *ums_dev,
			    ulong start, lbaint_t blkcnt, const void *buf)
{
	lbaint_t blkstart = start + ums_dev->start_sector;

	return dev_mass_write_sector(blkstart, blkcnt, buf);
}

static struct ums *ums = NULL;
static int ums_count=0;

static void ums_fini(void)
{
	int i;

	free(ums);
	ums = 0;
	ums_count = 0;
}

static int ums_init(const char *devtype, const char *devnums)
{
	ums_count = 0;

	ums = realloc(ums, (ums_count + 1) * sizeof(*ums));
	if (!ums) {
		info("allocate ums fail\n");
		return -1;
	}
	ums[ums_count].read_sector = ums_read_sector;
	ums[ums_count].write_sector = ums_write_sector;
	ums[ums_count].start_sector = 0;
	ums[ums_count].num_sectors = dev_mass_get_lba();
	ums[ums_count].name = ums_name;
	memset(ums_name,0,sizeof(ums_name));
	strcpy(ums_name,"columbus");
	ums_count = 1;

	return 0;
}

int usb_dev_mass_storage(void)
{
	const char *usb_controller;
	const char *devtype;
	const char *devnum;
	unsigned int controller_index;
	int rc;
	int cable_ready_timeout __maybe_unused;

	rc = ums_init(devtype, devnum);
	if (rc < 0)
		return CMD_RET_FAILURE;

	controller_index = (unsigned int)(simple_strtoul(
				usb_controller,	NULL, 0));
	if (board_usb_init(controller_index, USB_INIT_DEVICE)) {
		info("Couldn't init USB controller.\n");
		rc = CMD_RET_FAILURE;
		goto cleanup_ums_init;
	}

	rc = fsg_init(ums, ums_count);
	if (rc) {
		info("fsg_init failed\n");
		rc = CMD_RET_FAILURE;
		goto cleanup_board;
	}

	rc = g_dnl_register("usb_dnl_ums");
	if (rc) {
		info("g_dnl_register failed\n");
		rc = CMD_RET_FAILURE;
		goto cleanup_board;
	}

	/* Timeout unit: seconds */
	cable_ready_timeout = UMS_CABLE_READY_TIMEOUT;

	if (!g_dnl_board_usb_cable_connected()) {
		/*
		 * Won't execute if we don't know whether the cable is
		 * connected.
		 */
		info("Please connect USB cable.\n");

		while (!g_dnl_board_usb_cable_connected()) {
			if (ctrlc()) {
				info("\rCTRL+C - Operation aborted.\n");
				rc = CMD_RET_SUCCESS;
				goto cleanup_register;
			}
			if (!cable_ready_timeout) {
				info("\rUSB cable not detected.\n" \
				     "Command exit.\n");
				rc = CMD_RET_SUCCESS;
				goto cleanup_register;
			}

			info("\rAuto exit in: %.2d s.\n", cable_ready_timeout);
			mdelay(1000);
			cable_ready_timeout--;
		}
		puts("\r\n");
	}

	while (1) {
		usb_gadget_handle_interrupts(controller_index);

		rc = fsg_main_thread(NULL);
		if (rc) {
			/* Check I/O error */
			if (rc == -EIO)
				info("\rCheck USB cable connection\n");

			/* Check CTRL+C */
			if (rc == -EPIPE)
				info("\rCTRL+C or Quit - Operation aborted\n");

			rc = CMD_RET_SUCCESS;
			goto cleanup_register;
		}
	}

cleanup_register:
	g_dnl_unregister();
cleanup_board:
	board_usb_cleanup(controller_index, USB_INIT_DEVICE);
cleanup_ums_init:
	ums_fini();

	return rc;
}



