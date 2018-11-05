/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/
#include "cdl_config.h"
#include "cdl_adc.h"
#include "cdl_dmac.h"

struct sar_adc adc;
struct sar_adc* p_adc = &adc;

unsigned long adc_read(struct sar_adc* dev, unsigned long offset)
{
	return __raw_readl((unsigned long)dev->regs + offset);
}

void adc_write(struct sar_adc* dev, unsigned long offset, 	unsigned long val)
{
	__raw_writel(val, (unsigned long)dev->regs + offset);
}

void adc_enable_channel(int ch, int en)
{
	unsigned long reg_value = 0;

	if ((ch < 0) || (ch > 3)) {
		debug("ch %d not valid\n", ch);
		return;
	}
	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~(ADC_EN_CH(ch));
	if (en)
		reg_value |= ADC_EN_CH(ch);
	adc_write(p_adc, ADC_GCR, reg_value);
}

void adc_enable(int en)
{
	unsigned long reg_value = 0;

	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~ADC_EN;
	if (en)
		reg_value |= ADC_EN;
	adc_write(p_adc, ADC_GCR, reg_value);
}

void adc_start(int en)
{
	unsigned long reg_value = 0;

	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~ADC_START_EN;
	reg_value &= ~RXFIFO_EN;
	adc_write(p_adc, ADC_GCR, reg_value);

	if (en) {
		reg_value |= ADC_START_EN | RXFIFO_EN;
	}
	adc_write(p_adc, ADC_GCR, reg_value);
}

void adc_power_down(int en)
{
	unsigned long reg_value = 0;

	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~ADC_PD_EN;
	if (en)
		reg_value |= ADC_PD_EN;
	adc_write(p_adc, ADC_GCR, reg_value);
}
void adc_internal_buffer_enable(int en)
{
	unsigned long reg_value = 0;
  
	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~ADC_PD_BUF_EN;
	if (en)
		reg_value |= ADC_PD_BUF_EN;
	adc_write(p_adc, ADC_GCR, reg_value);
}

void adc_clear_fifo(void)
{
	unsigned long reg_value;
    
	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~RXFIFO_EN;
	adc_write(p_adc, ADC_GCR, reg_value);
	reg_value |= RXFIFO_EN;
	adc_write(p_adc, ADC_GCR, reg_value);	

}

void adc_set_mode(int mode)
{
	unsigned long reg_value;
    
	p_adc->mode = mode;
	reg_value = adc_read(p_adc, ADC_GCR);
	reg_value &= ~ (CONTINUOUS | RXTLF);
	if (mode)
		reg_value |= (CONTINUOUS | RXTLF);
	adc_write(p_adc, ADC_GCR, reg_value);
}

int adc_read_single_data(unsigned int *pbuf, int len)
{
	int i = 0;
	int delay_cnt = 0xfffff;
	
	if (p_adc->mode != ADC_MODE_SINGLE || p_adc->dma_mode != 0)
		return 0;

	for(i=0; i<len;)
	{
		while (!((adc_read(p_adc,ADC_CSATA)&0x1)))
		{
			if(0 == (--delay_cnt))
				return i;
		}
		*(pbuf+i++) = adc_read(p_adc,ADC_RXREG) & CH_DATA_MASK;
	}

	return i;
}

static void adc_dma_receive(unsigned long* buf, int len)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[p_adc->dmac_idx];

	/* config src */
	dw_dmac_channel_config(p_dma_rx, p_adc->dma_ch_rx, 0, WORD_WIDTH,
			BURST_1,FIXED, PERI_TO_MEM, p_adc->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, p_adc->dma_ch_rx, 1, WORD_WIDTH,
			BURST_1,INCREASE, PERI_TO_MEM, p_adc->dma_req_rx);

	dw_dmac_channel_single_transfer(p_dma_rx, p_adc->dma_ch_rx,
		(unsigned long)p_adc->regs + ADC_RXREG, (unsigned long) buf,
		len,0);
}

int adc_wait_dma_transfer_done(void)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[p_adc->dmac_idx];

	return dw_dmac_channel_wait_done(p_dma_rx, p_adc->dma_ch_rx, DMAC_TIMEOUT);
}

int adc_read_cont_data(unsigned long* buf,int len)
{		
	if ((p_adc->mode != ADC_MODE_CONTINUOUS) || (p_adc->dma_mode == 0))
		return -1;
	adc_dma_receive(buf,len);

	return 0;
}

unsigned long adc_get_int_status(void)
{
	return adc_read(p_adc, ADC_ISR);
}

void adc_enable_interrupt(unsigned long mask)
{
	adc_write(p_adc, ADC_IER, mask);
}
void adc_clear_interrupt(unsigned long val)
{
	adc_write(p_adc, ADC_ICR, val);
}

void adc_init(unsigned long base, unsigned long pclk, int adc_freq,
	int clk_sel,int dma_mode, int switch_count)
{
	unsigned long ctrl=0;
	memset(p_adc, 0, sizeof(struct sar_adc));

	p_adc->regs = (void*)base;
	p_adc->pclk = pclk;
	p_adc->adc_freq = adc_freq;
	p_adc->clk_sel = clk_sel;
	p_adc->switch_count = switch_count;
	p_adc->dma_mode = dma_mode;
	p_adc->mode = ADC_MODE_SINGLE;

	p_adc->dmac_idx = 2;
	p_adc->dma_ch_rx = 0;
	p_adc->dma_req_rx = 7;//DMA_REQ_SARADC;
	p_adc->int_vector = 63;//INT_ADC;

	adc_write(p_adc, ADC_GCR, 0x300); // reset to default status

	/* disable all interrupt */
	adc_enable_interrupt(0);
	adc_clear_interrupt(0xffff);

	/*set clk div*/
#if 0 // spec update the formula?
	p_adc->div = p_adc->pclk / 2 / adc_freq - 1;
#else
	p_adc->div = p_adc->pclk / adc_freq;
#endif
	if (p_adc->div < 1)
		p_adc->div = 1;
	adc_write(p_adc, ADC_CDR, p_adc->div & 0xffff);

	ctrl = ADC_RCLR_EN | RXFIFO_EN;
	if (p_adc->clk_sel)
		ctrl |= ADC_CLK_SEL; //system clk
	if (p_adc->dma_mode)
		ctrl |= DMAMODE;
	
	//if (p_adc->mode != ADC_MODE_SINGLE)
	//	ctrl |= CONTINUOUS;

	adc_write(p_adc,ADC_COUNT, p_adc->switch_count);
	adc_write(p_adc,ADC_GCR,ctrl);

	//adc_enable(1);
}

