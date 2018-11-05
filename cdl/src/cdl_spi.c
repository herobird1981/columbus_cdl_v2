/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_spi.h"
#include "timer.h"
#include "cdl_dmac.h"

#define SPI_TIMEOUT (100)

unsigned char spi_mode_def[4] = {1, 0, 3, 2};

unsigned long brite_spi_read(struct brite_spi* priv, unsigned long offset)
{
	return __raw_readl((unsigned long)priv->regs + offset);
}

void brite_spi_write(struct brite_spi* priv, unsigned long offset, unsigned long val)
{
	__raw_writel(val, (unsigned long)priv->regs + offset);
}

void brite_spi_set_int_enable(struct brite_spi* priv, int enable)
{
	unsigned long value;
	value = brite_spi_read(priv, BRITE_SPI_GCTL);
	value &= ~SPI_INT_EN;
	value |= (enable ? SPI_INT_EN : 0);
	brite_spi_write(priv, BRITE_SPI_GCTL, value);
}

unsigned long brite_spi_get_int_status(struct brite_spi* priv)
{
	return brite_spi_read(priv, BRITE_SPI_INTSTAT);
}

void brite_spi_set_int_mask(struct brite_spi* priv, int mask)
{
	brite_spi_write(priv, BRITE_SPI_INTEN, mask);
}

void brite_spi_set_int_clear(struct brite_spi* priv, int clr)
{
	brite_spi_write(priv, BRITE_SPI_INTCLR, clr);
}

static void brite_spi_enable_chip(struct brite_spi* priv, int enable)
{
	unsigned long value;
	value = brite_spi_read(priv, BRITE_SPI_GCTL);
	value &= ~0x1;
	value |= (enable ? 1 : 0);
	brite_spi_write(priv, BRITE_SPI_GCTL, value);
}

void brite_spi_set_speed(struct brite_spi* priv, unsigned long speed)
{
	unsigned long clk_div;

	if (speed == 0) {
		brite_spi_enable_chip(priv, 0);
		priv->actual_freq = 0;
	} else {
		priv->freq = speed;
		clk_div = DIV_ROUND_UP(priv->pclk, priv->freq);
		if((clk_div & BRITE_SPI_SPBRG_MASK) != clk_div) {
			info("!!! Warning: clk_div 0x%x overflowed, truncated to 0x%x\n", clk_div, BRITE_SPI_SPBRG_MASK);
			clk_div = BRITE_SPI_SPBRG_MASK;
		}
		if (clk_div == 0) {
			clk_div = 1;
		}
		brite_spi_write(priv, BRITE_SPI_SPBRG, clk_div);
		priv->actual_freq = priv->pclk / clk_div;
	}
	info("spi actual clock: %d\n",priv->actual_freq);
}

void brite_spi_enable_txrx(struct brite_spi* priv, unsigned long dir)
{
	u32 val;

	val = brite_spi_read(priv,BRITE_SPI_GCTL);
	val &= ~(SPI_TXEN | SPI_RXEN);
	val |= dir;
	brite_spi_write(priv, BRITE_SPI_GCTL, val);
}

void brite_spi_set_txrx_threshold(struct brite_spi* priv, unsigned long thres)
{
	u32 val;

	val = brite_spi_read(priv,BRITE_SPI_GCTL);
	val &= ~SPI_RXTXTLF_MASK;
	val |= thres;
	brite_spi_write(priv, BRITE_SPI_GCTL, val);
}

void brite_spi_empty_fifo(struct brite_spi* priv) {
	int fifo_cnt = 0;
	u64 start = 0;
	
	start = get_timer(0);
	while (readl((u32)priv->regs + BRITE_SPI_CSTAT) & SPI_RXAVL) {
		readl((u32)priv->regs + BRITE_SPI_RXREG);
		fifo_cnt++;
		if (get_timer(start) > SPI_TIMEOUT)
		{
			info("\nSPI FIFO EMPTY FAIL\n");
			break;
		}
	}
	readl((u32)priv->regs + BRITE_SPI_RXREG);
	/* info("empty current fifo, read cnt %d\n", fifo_cnt); */
}

void brite_spi_dump_status(struct brite_spi* spi) {
	info("cstat 0x%x intstat 0x%x gctl 0x%x cctl 0x%x div 0x%x rxdnr %d cs 0x%x\n",
			brite_spi_read(spi, BRITE_SPI_CSTAT),
			brite_spi_read(spi, BRITE_SPI_INTSTAT),
			brite_spi_read(spi, BRITE_SPI_GCTL),
			brite_spi_read(spi, BRITE_SPI_CCTL),
			brite_spi_read(spi, BRITE_SPI_SPBRG),
			brite_spi_read(spi, BRITE_SPI_RXDNR),
			brite_spi_read(spi, BRITE_SPI_SCSR));
}

void brite_spi_init_hw(struct brite_spi* priv)
{
	unsigned long gctl = 0;
	unsigned long cctl = 0;
	unsigned long temp = 0;

	brite_spi_write(priv, BRITE_SPI_GCTL, 0); /* disable chip */
	brite_spi_write(priv, BRITE_SPI_INTEN, 0); /* disable all interrupt */
	brite_spi_write(priv, BRITE_SPI_INTCLR, 0xff); /* clear all interrupt */

	/* master or slave mode	*/
	gctl |= (priv->mode ? SPI_MASTER_MODE : 0);

	#if 0 /*software control cs, can't use ti mode, it force low and one high pulse before valid data*/
	/* TI mode or normal mode */
	gctl |= (priv->type ? SPI_TI_MODE : 0);
	#endif

	/* dma mode enable or not */
	gctl |= (priv->dma_mode ? SPI_DMAMODE : 0);

	/*enable tx/rx*/
	gctl |= (SPI_TXEN | SPI_RXEN);
	if (priv->dma_mode) {
		if(priv->freq >=(priv->pclk>>3)){
			gctl |= SPI_TXTLF_16 | SPI_RXTLF_16;
			priv->dma_burst_len = BURST_16;
		}
		else{
			gctl |= SPI_TXTLF_16 | SPI_RXTLF_4;
			priv->dma_burst_len = BURST_4;
		}
	}
	/* enable chip */
	gctl |= SPI_EN;

	/* clock Phase and Clock Polarity; SPI_MODE0,SPI_MODE1,SPI_MODE2,SPI_MODE3 */
	cctl |= (priv->phase & 0x3);
	/* LSB first transmit or MSB: 1 LSB */
	cctl |= (priv->lsbfe ? SPI_LSBFE : 0);
	/* spi len: data bits to transfer */
	cctl |= ((priv->bits_per_word-1) & 0x1f) << 8;

	/*
		 calculate step
		 4-8bit:	one transfer one byte (only 4-8bit valid)
		 9-16bit: one transfer two bytes (only 9-16bit valid)
		 17-32bit: one transfer four bytes(only 17-32bit valid)
		 transfer out/in data should orgnized according this
	*/

	if ((priv->bits_per_word >= 4) && (priv->bits_per_word <= 8)) {
		priv->step = 1;
	} else if ((priv->bits_per_word >= 9) && (priv->bits_per_word <= 16)) {
		priv->step = 2;
	} else {
		priv->step = 4;
	}

	/* set cs */
	if (priv->mode) {
		temp = 0xff; /*inactie all*/
		brite_spi_write(priv, BRITE_SPI_SCSR, temp);
	}

	/* set baud rate */
	brite_spi_set_speed(priv, priv->freq);
	//:sample the edge of the rxed data, high speed use 
	if((priv->actual_freq >= 30000000)&& priv->mode == 1)
		cctl |= (1<<4);
	
	brite_spi_write(priv, BRITE_SPI_CCTL, cctl); //config cctl
	brite_spi_write(priv, BRITE_SPI_GCTL, gctl); //config gctl

	brite_spi_empty_fifo(priv);
}

/* Restart the controller, disable all interrupts, clean rx fifo */
void brite_spi_init(struct brite_spi* priv, unsigned long pclk,
	int master_slave, int baudrate, int dma_mode, int type, int phase,
	int cs, int bus_width)
{	
	priv->pclk = pclk;
	priv->freq = baudrate;
	priv->dma_mode = dma_mode;
	priv->mode = master_slave;

	priv->type = type;
	priv->phase = spi_mode_def[phase&0x3];
	priv->cs = cs;
	priv->bits_per_word = bus_width;

	brite_spi_init_hw(priv);	
}

static int brite_spi_writer(struct brite_spi* priv)
{
	unsigned long txw = 0;
	unsigned long status;

	/* Wait for tx fifo not full*/
	status = brite_spi_read(priv, BRITE_SPI_CSTAT);
	if (status & SPI_TXFULL) {
		return 0;
	}
	while ((priv->tx< priv->tx_end) && (!(status & SPI_TXFULL))) {
		/* if rx fifo have more than 4 data, we should do read first,or data lost
		 check int status: rx fifo full seems come later lead to data lost
		*/
		status = brite_spi_read(priv, BRITE_SPI_CSTAT);
		if (status & SPI_RXAVL_4)
			break;
		/* Set the tx word if the transfer's original "tx" is not null */
		if ((unsigned long)priv->tx_end - priv->len) {
			if (priv->step == 1) {
				txw = *(unsigned char*)(priv->tx);
			} else if (priv->step == 2) {
				txw = *(unsigned short*)(priv->tx);
			} else {
				txw = *(unsigned long*)(priv->tx);
			}
		}
		brite_spi_write(priv, BRITE_SPI_TXREG, txw);
		priv->tx += priv->step;
		status = brite_spi_read(priv, BRITE_SPI_CSTAT);
	}
	return 0;
}

/*
check status, we must continue the loop when SPI_RXAVL
(receive fifo have data);but only check RXAVL may loss
data, because tx/rx may slow than this loop so when we
check here, tx fifo not complete send, RXAVL may not
ready now
*/
static int brite_spi_reader(struct brite_spi* priv)
{
	unsigned long long start = get_timer(0);
	unsigned long rxw;
	unsigned long status;

	/* Wait for rx data to be ready */
	status = brite_spi_read(priv, BRITE_SPI_CSTAT);
	while (!(status & SPI_RXAVL)) {
		if (priv->mode) { /* master */
			if (get_timer(start) > RX_TIMEOUT) {
				return -ETIMEDOUT;
			}
		}
		status = brite_spi_read(priv, BRITE_SPI_CSTAT);
	}

	while ((priv->rx< priv->rx_end) && (status & SPI_RXAVL)) {
		rxw = brite_spi_read(priv, BRITE_SPI_RXREG);
		/* check original "rx" buffer is not null */
		if (priv->rx_end - priv->len) {
			if (priv->step == 1) {
				*(unsigned char*)(priv->rx) = rxw;
			} else if (priv->step == 2) {
				*(unsigned short*)(priv->rx) = rxw;
			} else {
				*(unsigned long*)(priv->rx) = rxw;
			}
		}
		priv->rx += priv->step;

		/* check status */
		status = brite_spi_read(priv, BRITE_SPI_CSTAT);
	}

	return 0;
}

static int poll_transfer(struct brite_spi* priv)
{
	int ret;

	do {
		brite_spi_writer(priv);

		ret = brite_spi_reader(priv);
		if (ret < 0) {
			return ret;
		}
	} while (priv->rx_end > priv->rx);

	return 0;
}

static int dma_transfer(struct brite_spi* priv)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[priv->dmac_idx];
	struct dw_dmac* p_dma_rx = &dma_dev_list[priv->dmac_idx];
	int ret = 0;
	int fail = 0;
	unsigned long tx_buf,rx_buf;
	int tx_addr_upd,rx_addr_upd;
	unsigned long dma_len;
	unsigned long trans_len;
	int width;

	if (priv->step ==2) {
		dma_len = priv->len/2;
		width = HWORD_WIDTH;
	}
	else if (priv->step ==4) {
		dma_len = priv->len/4;
		width = WORD_WIDTH;
	}
	else {
		dma_len = priv->len;
		width = BYTE_WIDTH;
	}

	/* if the transfer's original "tx" is null,use dummy buf for dma transfer */
	if ((unsigned long)priv->tx_end - priv->len) {
		tx_buf = (unsigned long)priv->tx;
		tx_addr_upd = INCREASE;
	} else {
		tx_buf = (unsigned long) & priv->dummy_dma_buf;
		tx_addr_upd = FIXED;
	}
	/* if the transfer's original "rx" is null,use dummy buf for dma transfer */
	if ((unsigned long)priv->rx_end - priv->len) {
		rx_buf = (unsigned long)priv->rx;
		rx_addr_upd = INCREASE;
	} else {
		rx_buf = (unsigned long) & priv->dummy_dma_buf;
		rx_addr_upd = FIXED;
	}

	/* config src */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 0, width,
		priv->dma_burst_len, tx_addr_upd,  MEM_TO_PERI, priv->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 1, width,
		priv->dma_burst_len,		FIXED,  MEM_TO_PERI, priv->dma_req_tx);

	/* config src */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 0, width,
		priv->dma_burst_len,		FIXED,  PERI_TO_MEM, priv->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 1, width,
		priv->dma_burst_len, rx_addr_upd,  PERI_TO_MEM, priv->dma_req_rx);

	do {
		if (dma_len > DMAC_MAX_SIZE_PER_BLOCK)
			trans_len = DMAC_MAX_SIZE_PER_BLOCK;
		else
			trans_len = dma_len;

		ret = dw_dmac_channel_single_transfer(p_dma_rx, priv->dma_ch_rx,
				(unsigned long)priv->regs + BRITE_SPI_RXREG, rx_buf, trans_len,	0);

		ret = dw_dmac_channel_single_transfer(p_dma_tx, priv->dma_ch_tx,
				tx_buf,	(unsigned long)priv->regs + BRITE_SPI_TXREG, trans_len, 0);

		ret = dw_dmac_channel_wait_done(p_dma_tx, priv->dma_ch_tx, DMAC_TIMEOUT);
		if (ret)
			fail++;
		ret = dw_dmac_channel_wait_done(p_dma_rx, priv->dma_ch_rx, DMAC_TIMEOUT);
		if (ret)
			fail++;

		if (fail)
			return -ETIMEDOUT;

		dma_len -= trans_len;
		if ((unsigned long)priv->tx_end - priv->len)
			tx_buf += trans_len*priv->step;
		if ((unsigned long)priv->rx_end - priv->len)
			rx_buf += trans_len*priv->step;
	} while (dma_len>0);
	return 0;
}

static void brite_spi_cs_active(struct brite_spi* priv,int active)
{
	unsigned long temp = 0;

	if (active)
		temp = ~(1 << priv->cs);
	else
		temp = 0xff;
	brite_spi_write(priv, BRITE_SPI_SCSR, temp);
}

int brite_spi_xfer(struct brite_spi* priv, unsigned int bitlen,
	const void* dout, void* din, unsigned long flags)
{
	const unsigned char* tx = dout;
	unsigned char* rx = din;
	int ret = 0;
#if 0	
	uint32_t reg;
#endif
	if (bitlen % priv->bits_per_word) {
		debug("data width not match bus_width.\n");
		return -1;
	}

	/* calculate total length of tx/rx buffer */
	priv->len = (bitlen / priv->bits_per_word);

	priv->tx = (void*)tx;
	priv->tx_end = priv->tx + priv->len;
	priv->rx = rx;
	priv->rx_end = priv->rx + priv->len;

	/*set cs*/
	if (flags & SPI_XFER_BEGIN)
		brite_spi_cs_active(priv,1);

	/* Start transfer in a polling loop */
	if (priv->dma_mode) {
		ret = dma_transfer(priv);
	} else {
		ret = poll_transfer(priv);
	}
#if 0
	reg = brite_spi_read(priv, BRITE_SPI_INTSTAT);
	if(reg & SPI_UNDERRUN_INTF)
	{
		debug("\nspi SPI_UNDERRUN_INTF err\n");
	}
	if(reg & SPI_RXOERR_INTF)
	{
		debug("\nspi SPI_RXOERR_INTF err\n");
	}	
#endif
	if (flags & SPI_XFER_END)
		brite_spi_cs_active(priv,0);


	return ret;
}

void brite_spi_rx_cnt(struct brite_spi* priv, unsigned int rx_cnt) {
	unsigned long status;
	u64 start = 0;
	
	brite_spi_cs_active(priv,1);
	while(rx_cnt) {
		status = brite_spi_read(priv, BRITE_SPI_CSTAT);
		if(status & SPI_TXFULL)
			continue;
		brite_spi_write(priv, BRITE_SPI_TXREG, rx_cnt--);
	}
	
	start = get_timer(0);
	while (!(brite_spi_read(priv, BRITE_SPI_CSTAT) & SPI_TXEPT))
	{
		if (get_timer(start) > SPI_TIMEOUT)
		{
			info("\nbrite_spi_rx_cnt fail\n");
			break;
		}
	}
	brite_spi_cs_active(priv,0);
}

static int slave_dma_tx(struct brite_spi* priv,unsigned char*buf,int len)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[priv->dmac_idx];
	int width;

	//info("slave tx\n");
	if (priv->step ==2) {
		width = HWORD_WIDTH;
	}
	else if (priv->step ==4) {
		width = WORD_WIDTH;
	}
	else {
		width = BYTE_WIDTH;
	}

	/* config src */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 0, width,
		priv->dma_burst_len, INCREASE,  MEM_TO_PERI, priv->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 1, width,
		priv->dma_burst_len, FIXED,  MEM_TO_PERI, priv->dma_req_tx);
		
	dw_dmac_channel_single_transfer(p_dma_tx, priv->dma_ch_tx,
		(unsigned long)buf,(unsigned long)priv->regs + BRITE_SPI_TXREG, len,
		1);

	return 0;
}
static int slave_dma_rx(struct brite_spi* priv,unsigned char*buf,int len)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[priv->dmac_idx];
	int width;

	//info("slave rx\n");
	if (priv->step ==2)
		width = HWORD_WIDTH;
	else if (priv->step ==4)
		width = WORD_WIDTH;
	else
		width = BYTE_WIDTH;

	/* config src */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 0, width,
		priv->dma_burst_len, FIXED,  PERI_TO_MEM, priv->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 1, width,
		priv->dma_burst_len, INCREASE,  PERI_TO_MEM, priv->dma_req_rx);
	
	dw_dmac_channel_single_transfer(p_dma_rx, priv->dma_ch_rx,
		(unsigned long)priv->regs + BRITE_SPI_RXREG, (unsigned long)buf, len,
		1);
	return 0;
}
static int slave_dma_rx_dummy(struct brite_spi* priv,unsigned char*buf,int len)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[priv->dmac_idx];
	unsigned long rx_buf;
	int width;
	rx_buf = (unsigned long) & priv->dummy_dma_buf;

	//info("slave rx dummy\n");
	if (priv->step ==2)
		width = HWORD_WIDTH;
	else if (priv->step ==4)
		width = WORD_WIDTH;
	else
		width = BYTE_WIDTH;

	/* config src */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 0, width,
		priv->dma_burst_len, FIXED,  PERI_TO_MEM, priv->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 1, width,
		priv->dma_burst_len, FIXED,  PERI_TO_MEM, priv->dma_req_rx);

	dw_dmac_channel_single_transfer(p_dma_rx, priv->dma_ch_rx,
		(unsigned long)priv->regs + BRITE_SPI_RXREG, (unsigned long)rx_buf, len,
		1);
	return 0;
}
unsigned long brite_spi_xfer_slave(struct brite_spi* priv, unsigned int len,
	const void* dout, void* din)
{
	unsigned long trans_len;

	if (priv->step ==2) {
		trans_len = len/2;
	}
	else if (priv->step ==4) {
		trans_len = len/4;
	}
	else {
		trans_len = len;
	}
	if (trans_len > DMAC_MAX_SIZE_PER_BLOCK)
		trans_len = DMAC_MAX_SIZE_PER_BLOCK;

	if (dout)
		slave_dma_tx(priv,(unsigned char*)dout,trans_len);
	if (din)
		slave_dma_rx(priv,(unsigned char*)din,trans_len);

	if (dout && !din)
		slave_dma_rx_dummy(priv,(unsigned char*)din,trans_len);

	return trans_len*priv->step;
}
