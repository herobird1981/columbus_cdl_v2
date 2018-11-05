/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_uart.h"

#include "cdl_dmac.h"
#include "cdl_pll.h"
#include "memleak.h"

inline void dw_uart_writel(struct dw_uart* dev, int reg, unsigned long val)
{
	writel(val, (unsigned long)dev->regs + reg);
}

inline unsigned long dw_uart_readl(struct dw_uart* dev, int reg)
{
	return readl((unsigned long)dev->regs + reg);
}

//static int uart_input_clk = 0;
//extern void console_reinit_baud(unsigned long pclk);

unsigned long dw_uart_calc_divisor(struct dw_uart* dev,
	unsigned long clock, unsigned long baudrate)
{
	const unsigned int mode_x_div = 16;
	unsigned long uart_ip_div = 0;

#if 0 /* EVB_BOARD */
	uint32_t uart_pll_div;

	if(!skip_pll_cfg) {
		switch (baudrate) {
		case 3000000:
			uart_pll_div = 0x46;
			break;
		default:
			uart_pll_div = 0x1D;
			break;
		}

		/*set the uart pll divisor */
		brite_set_uart_clk_div(uart_pll_div);
		clock = brite_get_uart_clk_rate();
		if(uart_input_clk != clock) {
			console_reinit_baud(clock);
			uart_input_clk = clock;
		}
	}
#endif

	uart_ip_div = (clock + mode_x_div * baudrate / 2) / (mode_x_div * baudrate);

	return uart_ip_div;
}

void dw_uart_set_line_control(struct dw_uart* dev, int cr)
{
	unsigned long value;

	dw_uart_writel(dev, DW_UART_FCR, 0);
	value = dw_uart_readl(dev, DW_UART_LCR);
	value &= ~0x1f;
	value |= cr;
	dw_uart_writel(dev, DW_UART_LCR, value);
	dw_uart_writel(dev, DW_UART_FCR, dev->flow ? UART_FCRVAL_AFCE : UART_FCRVAL);
}

void dw_uart_set_break(struct dw_uart* dev, int set)
{
	unsigned long value = dw_uart_readl(dev, DW_UART_LCR);
	value &= ~UART_LCR_SBRK;
	value |= (set ? UART_LCR_SBRK : 0);
	dw_uart_writel(dev, DW_UART_LCR, value);
}

void dw_uart_setbrg(struct dw_uart* dev, int baud_divisor)
{
	dw_uart_writel(dev, DW_UART_FCR, 0); /* disable & reset fifo */
	unsigned long value = dw_uart_readl(dev, DW_UART_LCR);
	dw_uart_writel(dev, DW_UART_LCR, value | UART_LCR_DLAB);
	dw_uart_writel(dev, DW_UART_DLL, baud_divisor & 0xff);
	dw_uart_writel(dev, DW_UART_DLH, (baud_divisor >> 8) & 0xff);
	dw_uart_writel(dev, DW_UART_LCR, value);
	dw_uart_writel(dev, DW_UART_FCR, dev->flow ? UART_FCRVAL_AFCE : UART_FCRVAL);
}

void dw_uart_set_modem_ctrl(struct dw_uart* dev, int mode)
{
	dw_uart_writel(dev, DW_UART_MCR, mode);
}

void dw_uart_set_modem_ctl_bits(struct dw_uart* dev, int bits, int mode)
{
	unsigned long val;

	val = dw_uart_readl(dev, DW_UART_MCR);
	val &= ~(1 << bits);
	val |= (mode ? 1 : 0) << bits;
	dw_uart_writel(dev, DW_UART_MCR, val);
}

unsigned long dw_uart_get_modem_status(struct dw_uart* dev)
{
	return dw_uart_readl(dev, DW_UART_MSR);
}

unsigned long dw_uart_get_line_status(struct dw_uart* dev)
{
	return dw_uart_readl(dev, DW_UART_LSR);
}

void dw_uart_enable_interrupt(struct dw_uart* dev, int en)
{
	dw_uart_writel(dev, DW_UART_IER, en);
}

unsigned long dw_uart_get_int_status(struct dw_uart* dev)
{
	unsigned long type;

	type = dw_uart_readl(dev, DW_UART_IIR);

	return type & 0xf;
}

unsigned long dw_uart_get_rcv_fifo_level(struct dw_uart* dev)
{
	unsigned long level;

	level = dw_uart_readl(dev, DW_UART_RFL);

	return level;
}

void dw_uart_set_fcr_rx_thres(struct dw_uart* dev, unsigned long rx_thres)
{
	unsigned long fcr;

	fcr = dw_uart_readl(dev, DW_UART_FCR);
	fcr &= ~UART_FCR_RX_TRIGGER_MASK;
	fcr |= rx_thres;
	dw_uart_writel(dev, DW_UART_FCR, fcr);
}

void dw_uart_init(struct dw_uart* dev, unsigned long pclk, int baudrate)
{
	dev->pclk = pclk;
	dev->baudrate = baudrate;
	if(!dev->data_bits)
		dev->data_bits = 8;
	if(!dev->stop_bits)
		dev->stop_bits = 1;


	dw_uart_enable_interrupt(dev, 0); /* disable all interrupt */
	dw_uart_set_modem_ctrl(dev, UART_FLOWCTRL_NONE); /* no flow control */
	dw_uart_writel(dev, DW_UART_FCR, dev->flow ? UART_FCRVAL_AFCE : UART_FCRVAL); /* reset and enable fifo */
	if (baudrate != -1) {
		uart_tune_clk_for_baud(&arm_uart_clk, baudrate);
		dev->div = dw_uart_calc_divisor(dev, UART_PCLK, dev->baudrate);
		dw_uart_setbrg(dev, dev->div);
	}
	/*default set: n 8 1*/
	dw_uart_set_line_control(dev,
		LCR_DATA_8BIT | LCR_STOP_1BIT | LCR_PARITY_NONE);
}

void dw_uart_init_no_modem(struct dw_uart* dev, unsigned long pclk, int baudrate)
{
	dev->pclk = pclk;
	dev->baudrate = baudrate;
	if(!dev->data_bits)
		dev->data_bits = 8;
	if(!dev->stop_bits)
		dev->stop_bits = 1;

	dw_uart_enable_interrupt(dev, 0); /* disable all interrupt */
	dw_uart_writel(dev, DW_UART_FCR, dev->flow ? UART_FCRVAL_AFCE : UART_FCRVAL); /* reset and enable fifo */
	if (baudrate != -1) {
		uart_tune_clk_for_baud(&arm_uart_clk, baudrate);
		dev->div = dw_uart_calc_divisor(dev, dev->pclk, dev->baudrate);
		dw_uart_setbrg(dev, dev->div);
	}
	/*default set: n 8 1*/
	dw_uart_set_line_control(dev,
		LCR_DATA_8BIT | LCR_STOP_1BIT | LCR_PARITY_NONE);
}

void dw_uart_flush(struct dw_uart* dev)
{
	/* wait transmit fifo empty */
	while ((dw_uart_readl(dev, DW_UART_LSR) & UART_LSR_TEMT) == 0);
}

void dw_uart_putc(struct dw_uart* dev, char c)
{
	/* wait transmit fifo idle */
	while ((dw_uart_readl(dev, DW_UART_LSR) & UART_LSR_THRE) == 0);
	dw_uart_writel(dev, DW_UART_THR, c);
}

char dw_uart_getc(struct dw_uart* dev)
{
	while ((dw_uart_readl(dev, DW_UART_LSR) & UART_LSR_DR) == 0);
	return dw_uart_readl(dev, DW_UART_RBR);
}

char dw_uart_read_rbr(struct dw_uart* dev)
{
	return dw_uart_readl(dev, DW_UART_RBR);
}

void dw_uart_empty_fifo(struct dw_uart* dev)
{
	while (dw_uart_tstc(dev))
			dw_uart_getc(dev);
	/* force read RBR register to resolve previous deadlock condition */
	dw_uart_read_rbr(dev);
}

int dw_uart_tstc(struct dw_uart* dev)
{
	return (dw_uart_readl(dev, DW_UART_LSR) & UART_LSR_DR) != 0;
}

static int dw_uart_dma_transfer(struct dw_uart* dev, const char* buf, int len)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[dev->dmac_idx];
	int ret = 0;

	/* config src */
	dw_dmac_channel_config(p_dma_tx, dev->dma_ch_tx, 0, BYTE_WIDTH, dev->tx_dma_burst_size,
		INCREASE, MEM_TO_PERI, dev->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, dev->dma_ch_tx, 1, BYTE_WIDTH, dev->tx_dma_burst_size,
		FIXED, MEM_TO_PERI, dev->dma_req_tx);

	ret = dw_dmac_channel_single_transfer_poll(p_dma_tx, dev->dma_ch_tx,
		(unsigned long) buf, (unsigned long)dev->regs + DW_UART_THR,
		len);

	return ret;
}

static int dw_uart_dma_receive(struct dw_uart* dev, const char* buf, int len)
{
    struct dw_dmac* p_dma_rx = &dma_dev_list[dev->dmac_idx];
    int ret = 0;
    int timeout = 3000; /* ms */
    unsigned long long start = get_timer(0);
    /* config src */
    dw_dmac_channel_config(p_dma_rx, dev->dma_ch_rx, 0, BYTE_WIDTH, dev->rx_dma_burst_size,
                FIXED, PERI_TO_MEM, dev->dma_req_rx);
    /* config dst */
    dw_dmac_channel_config(p_dma_rx, dev->dma_ch_rx, 1, BYTE_WIDTH, dev->rx_dma_burst_size,
                INCREASE, PERI_TO_MEM, dev->dma_req_rx);

    ret = dw_dmac_channel_single_transfer(p_dma_rx, dev->dma_ch_rx,
                (unsigned long)dev->regs + DW_UART_RBR, (unsigned long) buf,
                len,0);
    //while (!dw_dmac_channel_check_done(p_dma_rx,dev->dma_ch_rx));
    while(1)
    {
        if(dw_dmac_channel_check_done(p_dma_rx,dev->dma_ch_rx))
           break;
        if(get_timer(start) > timeout)
           return -1;
    }

    return ret;
}
static int dw_uart_dma_receive_non_block(struct dw_uart* dev,
	const char* buf, int len,int timeout)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[dev->dmac_idx];
	int ret = 0;
	unsigned long long start;
	int old_counter = 0;
	int counter;

	/* config src */
	dw_dmac_channel_config(p_dma_rx, dev->dma_ch_rx, 0, BYTE_WIDTH, dev->rx_dma_burst_size,
		FIXED, PERI_TO_MEM, dev->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, dev->dma_ch_rx, 1, BYTE_WIDTH, dev->rx_dma_burst_size,
		INCREASE, PERI_TO_MEM, dev->dma_req_rx);

	ret = dw_dmac_channel_single_transfer(p_dma_rx, dev->dma_ch_rx,
		(unsigned long)dev->regs + DW_UART_RBR, (unsigned long) buf,
		len,0);
	ret = len;
	start = get_timer(0);
	while (!dw_dmac_channel_check_done(p_dma_rx,dev->dma_ch_rx)) {
		counter = dw_dmac_channel_get_transfered(p_dma_rx,dev->dma_ch_rx);
		if (counter > old_counter) {
			old_counter = counter;
			start = get_timer(0);
		} else if (get_timer(start) > timeout) {
			/*stop ch*/
			info("uart dma timedout, abort! transfered %d\n", counter);
			dw_dmac_channel_abort(p_dma_rx,dev->dma_ch_rx);
			break;
		}
	}
	ret = dw_dmac_channel_get_transfered(p_dma_rx,dev->dma_ch_rx);
	return ret;
}

int dw_uart_write(struct dw_uart* dev, const char* buf, int len)
{
	unsigned int i;

	if (!dev->dma_mode) {
		for (i = 0; i < len; i++) {
			dw_uart_putc(dev, buf[i]);
		}
	} else {
		i = dw_uart_dma_transfer(dev, buf, len);
		if (i == 0) {
			i = len;
		} else {
			i = 0;
		}
	}
	return(i);
}
int dw_uart_read(struct dw_uart* dev, char* buf, int len)
{
	unsigned int i;
	if (!dev->dma_mode) {
		for (i = 0; i < len; i++) {
			buf[i] = dw_uart_getc(dev);
		}
	} else {
		i = dw_uart_dma_receive(dev, buf, len);
		if (i == 0) {
			i = len;
		} else {
			i = 0;
		}
	}
	return(i);
}
/*
when timeout occur; returned, don't need read until all request 'len' reached
note: len can't exceed 4095
*/
int dw_uart_read_non_block(struct dw_uart* dev, char* buf,
	int len,int timeout)
{
	unsigned long rlen = 0;
	unsigned long long start;

	if (!dev->dma_mode) {
		start = get_timer(0);
		while (rlen < len) {
			if (dw_uart_tstc(dev)) {
				buf[rlen] = dw_uart_getc(dev);
				rlen++;
				start = get_timer(0);
			} else if (get_timer(start) > timeout) {
				break;
			}
		}
	} else {
		rlen = dw_uart_dma_receive_non_block(dev, buf, len,timeout);
	}
	return (rlen);
}

static int dw_uart_dma_mb(struct dw_uart* dev, int txrx,const char* buf, int len,int int_en)
{
	struct dw_dmac* p_dma = &dma_dev_list[dev->dmac_idx];
	int ret = 0;
	struct dw_dmac_lli_item* lli_hdr;
	struct dw_dmac_lli_item* lli_prev = NULL;
	struct dw_dmac_lli_item* lli_curr;
	unsigned long ctl;
	unsigned long bsrc, bdst;
	unsigned long blen;
	unsigned long count_per_block;
	int lli_count_used = 0;

	if (txrx==0) { /*rx*/
		if (dev->p_rx_lli) {
			free((void *)dev->p_rx_lli);
			dev->p_rx_lli = NULL;
		}
	}else {
		if (dev->p_tx_lli) {
			free((void *)dev->p_tx_lli);
			dev->p_tx_lli = NULL;
		}
	}
	lli_hdr = (struct dw_dmac_lli_item *) malloc(((len/DMAC_MAX_SIZE_PER_BLOCK)+1)*sizeof(struct dw_dmac_lli_item));
	if (!lli_hdr) {
		info("allocate memory for dma LLI fail\n");
		return 0;
	}
	memset(lli_hdr, 0, ((len/DMAC_MAX_SIZE_PER_BLOCK)+1)*sizeof(struct dw_dmac_lli_item));

	if (txrx==0) { /*rx*/
		dev->p_rx_lli = (unsigned char*)lli_hdr;

		dw_dmac_channel_config(p_dma, dev->dma_ch_rx, 0, BYTE_WIDTH, dev->rx_dma_burst_size,
			FIXED, PERI_TO_MEM, dev->dma_req_rx);
		/*cofigure CTL register value*/
		ctl = dw_dmac_config_lli_ctl(0,
			0,
			BYTE_WIDTH,
			dev->rx_dma_burst_size,
			FIXED,
			PERI_TO_MEM,
			dev->dma_req_rx);
		ctl = dw_dmac_config_lli_ctl(ctl,
			1,
			BYTE_WIDTH,
			dev->rx_dma_burst_size,
			INCREASE,
			PERI_TO_MEM,
			dev->dma_req_rx);
	} else { /*tx*/
		dev->p_tx_lli = (unsigned char*)lli_hdr;
		dw_dmac_channel_config(p_dma, dev->dma_ch_tx, 1, BYTE_WIDTH, dev->tx_dma_burst_size,
			FIXED, MEM_TO_PERI, dev->dma_req_tx);

		/*cofigure CTL register value*/
		ctl = dw_dmac_config_lli_ctl(0,
			0,
			BYTE_WIDTH,
			dev->tx_dma_burst_size,
			INCREASE,
			MEM_TO_PERI,
			dev->dma_req_tx);
		ctl = dw_dmac_config_lli_ctl(ctl,
			1,
			BYTE_WIDTH,
			dev->tx_dma_burst_size,
			FIXED,
			MEM_TO_PERI,
			dev->dma_req_tx);
	}
	lli_curr = lli_hdr;
	lli_prev = NULL;
	if (txrx==0) { /*rx*/
		bsrc = (unsigned long)dev->regs + DW_UART_RBR;
		bdst = (unsigned long)buf;
	} else { /*tx*/
		bsrc = (unsigned long)buf;
		bdst = (unsigned long)dev->regs + DW_UART_THR;
	}
	blen = len;
	while (blen > 0) {
		if (blen > DMAC_MAX_SIZE_PER_BLOCK) {
			count_per_block = DMAC_MAX_SIZE_PER_BLOCK;
		} else {
			count_per_block = blen;
		}
		ret = dw_dmac_config_lli_item(lli_prev,
			lli_curr,
			bsrc,
			bdst,
			ctl,
			count_per_block,
			0);
		if (ret) {
			info("configure lli fail\n");
			ret = 1;
			goto end;
		}
		lli_count_used++;
		lli_prev = lli_curr;
		lli_curr += 1;
		if (txrx==0) { /*rx*/
			bdst += count_per_block;
		} else { /*tx*/
			bsrc += count_per_block;
		}
		blen -= count_per_block;
	}
	/*if enable interrupt, set int flag at last LLI*/
	if (int_en) {
		lli_prev->ctl_lo |= 1;
	}


	/*start ch transfer*/
	if (txrx==0) { /*rx*/
		info("rx:total %d LLI item used for len %d\n", lli_count_used,len);
		ret = dw_dmac_channel_lli_transfer(p_dma, dev->dma_ch_rx,
			lli_hdr);
	} else { /*tx*/
		info("tx:total %d LLI item used for len %d\n", lli_count_used,len);
		ret = dw_dmac_channel_lli_transfer(p_dma, dev->dma_ch_tx,
			lli_hdr);
	}

end:
	if (ret)
		free(lli_hdr);
	return ret;
}

int dw_uart_write_mb(struct dw_uart* dev, const char* buf, int len)
{
	int ret;

	ret = dw_uart_dma_mb(dev,1,buf, len,1);

	return ret;
}
int dw_uart_read_mb(struct dw_uart* dev, char* buf, int len)
{
	int ret;

	ret = dw_uart_dma_mb(dev,0,buf, len,1);

	return ret;
}

