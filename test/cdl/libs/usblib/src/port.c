#include "usblib_config.h"

//#include "cdl_usb.h"
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

struct musb_host_data host;

static int ctrlc_was_pressed = 0;

int ctrlc(void)
{
	if (console_tstc()) {
		switch (console_getc()) {
		case 0x03:		/* ^C - Control C */
			ctrlc_was_pressed = 1;
			return 1;
		default:
			break;
		}
	}


	return 0;
}

#if 1
static irqreturn_t cb_musb_interrupt(int irq, void *__hci)
{
	unsigned long	flags;
	irqreturn_t	retval = IRQ_NONE;
	struct musb	*musb = __hci;

	spin_lock_irqsave(&musb->lock, flags);

	musb->int_tx = musb_readw(musb->mregs, MUSB_INTRTX);
	//printf("irq=%d\n",irq);
	if (irq!=0)
	{
		musb->int_usb = 0;
		musb->int_rx = 0;
	}else
	{
		musb->int_usb = musb_readb(musb->mregs, MUSB_INTRUSB);
		musb->int_rx = musb_readw(musb->mregs, MUSB_INTRRX);
	}
	if (musb->int_usb || musb->int_tx || musb->int_rx)
		retval = musb_interrupt(musb);
	//printf("retval=%d\n",retval);
	spin_unlock_irqrestore(&musb->lock, flags);

	return retval;
}
#endif

/* musb_core does not call enable / disable in a balanced manner <sigh> */
static bool enabled = false;

static int musb_init(struct musb *musb)
{
	info("%s\n",__FUNCTION__);

	/*parse DT*/
	//vul_musb_parse_dt();
#if 0
	musb->mregs = (void*)USB0_BASE;
	musb->ctrl_base = (void*)USB0_BASE;

	musb->isr = cb_musb_interrupt;
	//softreset controller
	__raw_writeb(3,USB0_BASE+MUSB_LINKINFO);
#endif
	musb->isr = cb_musb_interrupt;

	return 0;
}

static int musb_exit(struct musb *musb)
{
	info("%s\n",__FUNCTION__);

	return 0;
}

static int musb_enable(struct musb *musb)
{
	musb_ep_select(musb->mregs, 0);
	musb_writeb(musb->mregs, MUSB_FADDR, 0);

	if (enabled)
		return 0;

	enabled = true;
	return 0;
}

static void musb_disable(struct musb *musb)
{
	if (!enabled)
		return;
	mdelay(200); /* Wait for the current session to timeout */

	enabled = false;
}

const struct musb_platform_ops musb_ops = {
	.init		= musb_init,
	.exit		= musb_exit,
	.enable		= musb_enable,
	.disable	= musb_disable,
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

int musb_usb_probe(unsigned int usb_base, int usb_mode)
{
	int ret;

	ret = musb_lowlevel_init(NULL);
	if (ret == 0)
		info("MUSB OTG\n");

	return ret;
}

int musb_usb_remove()
{
	musb_stop(host.host);

	return 0;
}

#define ULPI_TEST_VALUE		0x55	/* 0x55 == 0b01010101 */
#define ULPI_ID_REGS_COUNT	4

static struct ulpi_regs *ulpi = (struct ulpi_regs *)0;

static int musb_ulpi_integrity_check(unsigned int usb_base)
{
	u32 val, tval = ULPI_TEST_VALUE;
	int err, i;

	/* Use the 'special' test value to check all bits */
	for (i = 0; i < 2; i++, tval <<= 1) {
		err = musb_ulpi_write(usb_base, &ulpi->scratch, tval);
		if (err)
			return err;

		val = musb_ulpi_read(usb_base, &ulpi->scratch);
		if (val != tval) {
			printf("ULPI integrity check failed\n");
			return val;
		}
	}

	return 0;
}


static int musb_ulpi_set_otg_flags(unsigned int usb_base, int usb_mode)
{
	u8 otg_reg_data;
	otg_reg_data = musb_ulpi_read(usb_base, &ulpi->otg_ctrl);
	if(usb_mode == MUSB_HOST)
	{
		otg_reg_data |= ULPI_OTG_DP_PULLDOWN |
			ULPI_OTG_DM_PULLDOWN | ULPI_OTG_DRVVBUS | ULPI_OTG_DRVVBUS_EXT;

		musb_ulpi_write(usb_base, &ulpi->otg_ctrl, otg_reg_data);
	}
	else if(usb_mode == MUSB_PERIPHERAL)
	{
		otg_reg_data |= ULPI_OTG_ID_PULLUP;
		otg_reg_data &= ~(ULPI_OTG_DP_PULLDOWN);
		otg_reg_data &= ~(ULPI_OTG_DM_PULLDOWN);
		musb_ulpi_write(usb_base, &ulpi->otg_ctrl, otg_reg_data);
	}

	otg_reg_data = musb_ulpi_read(usb_base, &ulpi->otg_ctrl);
	debug("otg_reg_data = 0x%x\n", otg_reg_data);
	return 0;
}

static int musb_ulpi_set_fc_flags(unsigned int usb_base)
{
	u8 fc_reg_data = 0;

	/*
	 * ULPI Specification rev.1.1 default
	 * for XcvrSelect is Full Speed.
	 */

	//fc_reg_data = ulpi_read(usb_base, FUNCTION_CTRL);
	fc_reg_data |= ULPI_FC_HIGH_SPEED;
	/*
	 * ULPI Specification rev.1.1 default
	 * for OpMode is Normal Operation.
	 */

	fc_reg_data |= ULPI_FC_OPMODE_NORMAL;

	/*
	 * ULPI Specification rev.1.1 default
	 * for SuspendM is Powered.
	 */
	fc_reg_data |= ULPI_FC_SUSPENDM;

	/*
	 * USB3320 Specification that the bit TERMSELECT should set
	 */
	fc_reg_data |= ULPI_FC_TERMSELECT;

	musb_ulpi_write(usb_base, &ulpi->function_ctrl, fc_reg_data);

	fc_reg_data = musb_ulpi_read(usb_base, &ulpi->function_ctrl);
	debug("fc_reg_data = 0x%x\n",fc_reg_data);

	return 0;
}

static int musb_ulpi_set_ic_flags(unsigned int usb_base)
{
	u8 ic_reg_data = 0;
	ic_reg_data = musb_ulpi_read(usb_base, &ulpi->iface_ctrl);

	ic_reg_data |= ULPI_IFACE_AUTORESUME;

	musb_ulpi_write(usb_base, &ulpi->iface_ctrl, ic_reg_data);
	ic_reg_data = ulpi_read(usb_base, &ulpi->iface_ctrl);
	debug("ic_reg_data = 0x%x\n", ic_reg_data);

	return 0;
}

static int musb_ulpi_set_flags(unsigned int usb_base, int usb_mode)
{
	int ret;

	ret = musb_ulpi_set_otg_flags(usb_base, usb_mode);
	if (ret)
		return ret;

	ret = musb_ulpi_set_ic_flags(usb_base);
	if (ret)
		return ret;

	return musb_ulpi_set_ic_flags(usb_base);
}
int musb_ulpi_init(unsigned int usb_base, int usb_mode)
{
	u32 val, id = 0;
	u8 reg = &ulpi->product_id_high;
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
		console_printf("read usb3320 id error,please check again\n");
		return -EAGAIN;
	}
	/* Split ID into vendor and product ID. */
	debug("ULPI transceiver ID 0x%04x:0x%04x\n", id >> 16, id & 0xffff);

	musb_ulpi_integrity_check(usb_base);

	return musb_ulpi_set_flags(usb_base, usb_mode);
	//return ulpi_set_carkit_flags(usb_base);
}


int musb_board_init(unsigned int usb_base, int usb_mode)
{
	int ret = 0;
	u8 vid_l;
	u8 vid_h;
	struct ulpi_regs *ulpi = (struct ulpi_regs *)0;
	u32 val;

	musb_plat.mode = usb_mode;

	/*reset usb and phy*/
	val = readl(0x50004300);
	writel(val & ~0x3,0X50004300);
	mdelay(100);
	writel(val | 0x3,0X50004300);

	ret = musb_ulpi_init(usb_base,usb_mode);
	if (ret) {
		info("ulpi init fail\n");
		return -1;
	}
#if 0
	/* ULPI set flags */
	ulpi_write(usb_, &ulpi->otg_ctrl,
		   ULPI_OTG_DP_PULLDOWN | ULPI_OTG_DM_PULLDOWN |
		   ULPI_OTG_EXTVBUSIND);
	ulpi_write(&ulpi_vp, &ulpi->function_ctrl,
		   ULPI_FC_FULL_SPEED | ULPI_FC_OPMODE_NORMAL |
		   ULPI_FC_SUSPENDM);
	ulpi_write(&ulpi_vp, &ulpi->iface_ctrl, 0);

	/* Set VBus */
	ulpi_write(&ulpi_vp, &ulpi->otg_ctrl_set,
		   ULPI_OTG_DRVVBUS | ULPI_OTG_DRVVBUS_EXT);
#endif
	ret = musb_register(&musb_plat, NULL, (void *)usb_base);

	return ret;
}


