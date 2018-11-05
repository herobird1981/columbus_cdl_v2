#include "usblib_config.h"

#include <asm/io.h>

#include "cdl_usb.h"
#include "usblib.h"

#include <common.h>
#include <asm/errno.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>

#include <usb.h>
#include "linux-compat.h"
#include "usb-compat.h"
#include "musb_core.h"
#include "musb_host.h"
#include "musb_gadget.h"
#include "musb_uboot.h"
#include <usb/ulpi.h>

#include "hw_maps.h"

static int ctrlc_was_pressed = 0;

int ctrlc(void)
{
	if (console_tstc()) {
		switch (console_getc()) {
		case 0x03:		/* ^C - Control C */
			ctrlc_was_pressed = 1;
			return 1;
		case 'q':		/* Quit */
			return 1;
		default:
			break;
		}
	}


	return 0;
}

static int musb_init(struct musb *musb)
{
	info("%s\n",__FUNCTION__);

	return 0;
}

static int musb_exit(struct musb *musb)
{
	info("%s\n",__FUNCTION__);

	return 0;
}

const struct musb_platform_ops musb_ops = {
	.init		= musb_init,
	.exit		= musb_exit,
};

static struct musb_hdrc_config musb_config = {
	.multipoint     = 1,
	.dyn_fifo       = 1,
	.num_eps        = 16,
	.ram_bits       = 12,			/* ram address size */
};

static struct musb_hdrc_platform_data musb_plat = {
	.config         = &musb_config,
	.power          = 250,
	.platform_ops	= &musb_ops,
};

#define ULPI_TEST_VALUE		0x55	/* 0x55 == 0b01010101 */
#define ULPI_ID_REGS_COUNT	4

static struct ulpi_regs *ulpi = (struct ulpi_regs *)0;
int musb_ulpi_read(unsigned long base, u32 offset);
int musb_ulpi_init(unsigned long usb_base, int usb_mode)
{
	u32 val, id = 0;
	u32 reg = (u32)(&ulpi->product_id_high);
	int i;

	/* Assemble ID from four ULPI ID registers (8 bits each). */
	for (i = 0; i < ULPI_ID_REGS_COUNT; i++) {
		val = musb_ulpi_read(usb_base, reg - i);
		if (val == ULPI_ERROR)
			return val;

		id = (id << 8) | val;
	}

	if(id != 0x70424)
	{
		info("read usb3320 id error,please check again\n");
		return -EAGAIN;
	}
	/* Split ID into vendor and product ID. */
	info("ULPI transceiver ID 0x%04x:0x%04x\n", id >> 16, id & 0xffff);

	return 0;
}
int musb_board_init(struct musb_controller *dev)
{
	int ret = 0;
	u8 vid_l;
	u8 vid_h;
	struct ulpi_regs *ulpi = (struct ulpi_regs *)0;
	u32 val;

	musb_plat.mode = dev->usb_mode;

	/*reset usb controller0 and 1*/
	val = readl(PRCM_BASE + 0x18);
	writel(val & (~((1<<29)|(1<<30))), (PRCM_BASE + 0x18));
	mdelay(100);
	writel(val | ((1<<29)|(1<<30)), (PRCM_BASE + 0x18));

	ret = musb_register(&musb_plat, NULL, dev->regs);

	return ret;
}

void musb_board_exit(void)
{
	musb_unregister(&musb_plat);
}

