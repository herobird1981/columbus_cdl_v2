/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_ADC_H__
#define __CDL_ADC_H__

#define ADC_GCR		0x00
#define ADC0_DR		0x04
#define ADC1_DR		0x08
#define ADC2_DR		0x0C
#define ADC3_DR		0x10
#define ADC4_DR		0x14
#define ADC5_DR		0x18
#define ADC6_DR		0x1C
#define ADC7_DR		0x20
#define ADC_CDR		0x24
#define ADC_ISR		0x28
#define ADC_IER		0x2C
#define ADC_ICR		0x30
#define ADC_COUNT	0x34
#define ADC_RXREG	0x38
#define ADC_CSATA	0x3C

#define ADC_EN		(1<<0)
#define CONTINUOUS	(1<<1)
#define DMAMODE		(1<<2)
#define RXFIFO_EN	(1<<3)
#define RXTLF		(1<<4)
#define ADC_RCLR_EN	(1<<5)
#define ADC_CLK_SEL	(1<<6)
#define ADC_PD_EN	(1<<8)
#define ADC_PD_BUF_EN	(1<<9)
#define ADC_START_EN	(1<<10)
#define ADC_EN_CH(n)	(1<<(16+(n)))

#define CH_DATA_MASK	(0xFFF)
#define CH_DATA_VALID	(1<<15)

#define CLKDIV		0xFF

#define INT_CH(n)	(1<<(n))
#define INT_RX		(1<<8)
#define INT_RXFIFO_FULL	(1<<9)

#define RXAVL		(1<<0)

#define ADC_MODE_SINGLE		0
#define ADC_MODE_CONTINUOUS	1

struct sar_adc {
	void* regs;
	unsigned long pclk;
	unsigned long adc_freq;
	int div;

	int mode;
	int switch_count;
	int clk_sel;

	int dma_mode;
	int dmac_idx;
	int dma_ch_rx;
	int dma_req_rx;

	int int_vector;
};

extern struct sar_adc* p_adc;

/*apis*/
unsigned long adc_read(struct sar_adc* dev, unsigned long offset);

void adc_write(struct sar_adc* dev, unsigned long offset, unsigned long val);

void adc_init(unsigned long base, unsigned long pclk, int adc_freq,
	int clk_sel,int dma_mode, int switch_count);
void adc_enable_channel(int ch, int en);
void adc_enable(int en);
void adc_start(int en);
void adc_power_down(int en);
void adc_internal_buffer_enable(int en);
void adc_set_mode(int mode);
int adc_read_single_data(unsigned int *pbuf, int len);
int adc_read_cont_data(unsigned long* buf,int len);
int adc_wait_dma_transfer_done(void);
unsigned long adc_get_int_status(void);
void adc_enable_interrupt(unsigned long mask);
void adc_clear_interrupt(unsigned long val);
void adc_clear_fifo(void);

#endif

