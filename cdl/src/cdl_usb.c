/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_usb.h"

void usb_enable_int(struct musb_controller *priv, unsigned long val)
{
	musb_writeb(priv->regs,MUSB_INTRUSBE,val&0xff);
}
void usb_enable_tx_ep_int(struct musb_controller *priv,int ep,int val)
{
	musb_writew(priv->regs,MUSB_INTRTXE, (val&1)<<ep);
}
void usb_enable_rx_ep_int(struct musb_controller *priv,int ep,int val)
{
	musb_writew(priv->regs,MUSB_INTRRXE, (val&1)<<ep);
}
unsigned char usb_get_int_status(struct musb_controller*priv)
{
	return musb_readb(priv->regs,MUSB_INTRUSB);
}

#ifdef TAPEOUT_V2
/* USB Auxilary functions */
void usb_aux_vdt_enable(void *base, int index)
{
	u32 vdt_ctrl = 0;

	vdt_ctrl = musb_readl(base, USB_AUX_VDT_CTRL);
	if (index == MUSB_USB0) {
		vdt_ctrl |= USB0_VDT_EN;
	} else {
		vdt_ctrl |= USB1_VDT_EN;
	}
	musb_writel(base, USB_AUX_VDT_CTRL, vdt_ctrl);
}

void usb_aux_vdt_disable(void *base, int index)
{
	u32 vdt_ctrl = 0;

	vdt_ctrl = musb_readl(base, USB_AUX_VDT_CTRL);
	if (index == MUSB_USB0) {
		vdt_ctrl &= ~USB0_VDT_EN;
	} else {
		vdt_ctrl &= ~USB1_VDT_EN;
	}
	musb_writel(base, USB_AUX_VDT_CTRL, vdt_ctrl);
}

void usb_aux_int_enable(struct musb_controller *priv, u32 value)
{
	u32 int_en = 0;

	if (value == 0)
		return;

	if (priv->idx == MUSB_USB0) {
		int_en = musb_readl(priv->aux_regs, USB0_AUX_INT_EN);
		int_en |= value;
		musb_writel(priv->aux_regs, USB0_AUX_INT_EN, int_en);
	} else {
		int_en = musb_readl(priv->aux_regs, USB1_AUX_INT_EN);
		int_en |= value;
		musb_writel(priv->aux_regs, USB1_AUX_INT_EN, int_en);
	}
}

void usb_aux_int_disable(struct musb_controller *priv, u32 value)
{
	u32 int_en = 0;

	if (value == 0)
		return;

	if (priv->idx == MUSB_USB0) {
		int_en = musb_readl(priv->aux_regs, USB0_AUX_INT_EN);
		int_en &= ~value;
		musb_writel(priv->aux_regs, USB0_AUX_INT_EN, int_en);
	} else {
		int_en = musb_readl(priv->aux_regs, USB1_AUX_INT_EN);
		int_en &= ~value;
		musb_writel(priv->aux_regs, USB1_AUX_INT_EN, int_en);
	}
}

u32 usb_aux_int_mask_get(struct musb_controller *priv)
{
	u32 int_mask = 0;

	if (priv->idx == MUSB_USB0) {
		int_mask = musb_readl(priv->aux_regs, USB0_AUX_INT_MASK);
	} else {
		int_mask = musb_readl(priv->aux_regs, USB1_AUX_INT_MASK);
	}
	return int_mask;
}

u32 usb_aux_int_status_get(struct musb_controller *priv)
{
	u32 int_status = 0;

	if (priv->idx == MUSB_USB0) {
		int_status = musb_readl(priv->aux_regs, USB0_AUX_INT_STATUS);
	} else {
		int_status = musb_readl(priv->aux_regs, USB1_AUX_INT_STATUS);
	}
	return int_status;
}

void usb_aux_int_status_clear(struct musb_controller *priv, u32 value)
{
	u32 int_clr = 0;

	if (value == 0)
		return;

	if (priv->idx == MUSB_USB0) {
		int_clr = musb_readl(priv->aux_regs, USB0_AUX_INT_CLR);
		int_clr |= value;
		musb_writew(priv->aux_regs, USB0_AUX_INT_CLR, int_clr);
	} else {
		int_clr = musb_readl(priv->aux_regs, USB1_AUX_INT_CLR);
		int_clr |= value;
		musb_writew(priv->aux_regs, USB1_AUX_INT_CLR, int_clr);
	}
}

u32 usb_aux_id_debounce_get(void *base, int index)
{
	u32 deb_count = 0;

	if (index == MUSB_USB0) {
		deb_count = musb_readl(base, ID0_DEBOUNCE_CNT);
	} else {
		deb_count = musb_readl(base, ID1_DEBOUNCE_CNT);
	}
	return deb_count;
}

void usb_aux_id_debounce_set(void *base, int index, u32 count)
{
	if (index == MUSB_USB0) {
		musb_writel(base, ID0_DEBOUNCE_CNT, count);
	} else {
		musb_writel(base, ID1_DEBOUNCE_CNT, count);
	}
}
#endif
