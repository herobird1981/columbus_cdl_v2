/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_UART_H__
#define __CDL_UART_H__

#define UART_PORT_SIZE 0x1000
#define UART_FIFO_DEPTH  64

/* Register offsets */
#define DW_UART_RBR		(0x0)
#define DW_UART_THR		(0x0)
#define DW_UART_DLL		(0x0)
#define DW_UART_IER		(0x4)
#define DW_UART_DLH		(0x4)
#define DW_UART_FCR		(0x8)
#define DW_UART_IIR		(0x8)
#define DW_UART_LCR		(0xC)
#define DW_UART_MCR		(0x10)
#define DW_UART_LSR		(0x14)
#define DW_UART_MSR		(0x18)
#define DW_UART_SCR		(0x1C)
#define DW_UART_LPDLL		(0x20)
#define DW_UART_LPDLH		(0x24)

#define DW_UART_RFL		0x84		/* DW UART receive FIFO level */

/*
 * These are the definitions for the FIFO Control Register
 */
#define UART_FCR_FIFO_EN	0x01 /* Fifo enable */
#define UART_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define UART_FCR_RX_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define UART_FCR_RX_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define UART_FCR_RX_TRIGGER_QUARTER	0x40 /* Mask for trigger set at 1/4 */
#define UART_FCR_RX_TRIGGER_HALF	0x80 /* Mask for trigger set at 1/2 */
#define UART_FCR_RX_TRIGGER_FULL	0xC0 /* Mask for trigger set at fifo_depth-2 */
#define UART_FCR_TX_TRIGGER_MASK	0x30 /* Mask for the FIFO trigger range */
#define UART_FCR_TX_TRIGGER_EMPTY	0x00 /* Mask for trigger set at empty */
#define UART_FCR_TX_TRIGGER_2	0x10 /* Mask for trigger set at 2 */
#define UART_FCR_TX_TRIGGER_QUARTER	0x20 /* Mask for trigger set at 1/4 */
#define UART_FCR_TX_TRIGGER_HALF	0x30 /* Mask for trigger set at 1/2 */

#define UART_FCR_RXSR		0x02 /* Receiver soft reset */
#define UART_FCR_TXSR		0x04 /* Transmitter soft reset */

/*
 * These are the definitions for the Modem Control Register
 */
#define UART_MCR_DTR	0x01		/* DTR	 */
#define UART_MCR_RTS	0x02		/* RTS	 */
#define UART_MCR_OUT1	0x04		/* Out 1 */
#define UART_MCR_OUT2	0x08		/* Out 2 */
#define UART_MCR_LOOP	0x10		/* Enable loopback test mode */
#define UART_MCR_AFE	0x20		/* Enable auto-RTS/CTS */
#define UART_MCR_SIRE	0x40		/* Enable SIR Mode */

#define UART_MCR_DTR_BITS	0
#define UART_MCR_RTS_BITS	1
#define UART_MCR_OUT1_BITS	2
#define UART_MCR_OUT2_BITS	3
#define UART_MCR_LOOP_BITS	4
#define UART_MCR_AFE_BITS	5
#define UART_MCR_SIRE_BITS	6

/*
 * These are the definitions for the Line Control Register
 *
 * Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
 * UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
 */
#define UART_LCR_WLS_MSK 0x03		/* character length select mask */
#define UART_LCR_WLS_5	0x00		/* 5 bit character length */
#define UART_LCR_WLS_6	0x01		/* 6 bit character length */
#define UART_LCR_WLS_7	0x02		/* 7 bit character length */
#define UART_LCR_WLS_8	0x03		/* 8 bit character length */
#define UART_LCR_STB	0x04		/* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN	0x08		/* Parity eneble */
#define UART_LCR_EPS	0x10		/* Even Parity Select */
#define UART_LCR_STKP	0x20		/* Stick Parity */
#define UART_LCR_SBRK	0x40		/* Set Break */
#define UART_LCR_DLAB	0x80		/* Divisor latch access bit */

/*
 * These are the definitions for the Line Status Register and Modem Status
 */
#define UART_LSR_DR	0x01		/* Data ready */
#define UART_LSR_OE	0x02		/* Overrun */
#define UART_LSR_PE	0x04		/* Parity error */
#define UART_LSR_FE	0x08		/* Framing error */
#define UART_LSR_BI	0x10		/* Break */
#define UART_LSR_THRE	0x20		/* Xmit holding register empty */
#define UART_LSR_TEMT	0x40		/* Xmitter empty */
#define UART_LSR_ERR	0x80		/* Error */

#define UART_MSR_DCD	0x80		/* Data Carrier Detect */
#define UART_MSR_RI	0x40		/* Ring Indicator */
#define UART_MSR_DSR	0x20		/* Data Set Ready */
#define UART_MSR_CTS	0x10		/* Clear to Send */
#define UART_MSR_DDCD	0x08		/* Delta DCD */
#define UART_MSR_TERI	0x04		/* Trailing edge ring indicator */
#define UART_MSR_DDSR	0x02		/* Delta DSR */
#define UART_MSR_DCTS	0x01		/* Delta CTS */

/*
 * These are the definitions for the Interrupt Identification Register
 */
#define UART_IIR_NO_INT	0x01	/* No interrupts pending */
#define UART_IIR_ID	0x06	/* Mask for the interrupt ID */

#define UART_IIR_MSI	0x00	/* Modem status interrupt */
#define UART_IIR_THRI	0x02	/* Transmitter holding register empty */
#define UART_IIR_RDI	0x04	/* Receiver data interrupt */
#define UART_IIR_CHTO	0x0C	/* Receiver data interrupt */
#define UART_IIR_RLSI	0x06	/* Receiver line status interrupt */

/*
 * These are the definitions for the Interrupt Enable Register
 */
#define UART_IER_MSI	0x08	/* Enable Modem status interrupt */
#define UART_IER_RLSI	0x04	/* Enable receiver line status interrupt */
#define UART_IER_THRI	0x02	/* Enable Transmitter holding register int. */
#define UART_IER_RDI	0x01	/* Enable receiver data interrupt */

/* useful defaults for LCR */
#define UART_LCR_8N1	0x03

#define LCR_DATA_5BIT	0x0
#define LCR_DATA_6BIT	0x1
#define LCR_DATA_7BIT	0x2
#define LCR_DATA_8BIT	0x3

#define LCR_STOP_1BIT	0x0	/*1 stop bit*/
#define LCR_STOP_2BIT	0x4	/*2 stop bit; 1.5 stop bit when use 5bit DATA*/

#define LCR_PARITY_NONE	0x0
#define LCR_PARITY_ODD	0x08
#define LCR_PARITY_EVEN	0x18

/* default FCR */
#define UART_FCRVAL (UART_FCR_TX_TRIGGER_HALF | UART_FCR_RX_TRIGGER_QUARTER | \
	 UART_FCR_DMA_SELECT | UART_FCR_RXSR | UART_FCR_TXSR | UART_FCR_FIFO_EN)

#define UART_FCRVAL_AFCE (UART_FCR_TX_TRIGGER_HALF | UART_FCR_RX_TRIGGER_HALF | \
	 UART_FCR_DMA_SELECT | UART_FCR_RXSR | UART_FCR_TXSR | UART_FCR_FIFO_EN)

#define UART_FLOWCTRL_NONE	(UART_MCR_DTR | UART_MCR_RTS)
#define UART_FLOWCTRL_AUTO	(UART_MCR_RTS | UART_MCR_AFE)

struct dw_uart {
	void* regs;
	int line;
	unsigned long pclk;
	unsigned long baudrate;
	unsigned long div;
	int data_bits;
	int stop_bits;
	int flow;
	int parity;
	int len_per_xfer;

	int dma_mode;
	int dmac_idx;
	int dma_ch_tx;
	int dma_ch_rx;
	int dma_req_tx;
	int dma_req_rx;
	int tx_dma_burst_size;
	int rx_dma_burst_size;

	volatile int dma_block_xfer_len;

	volatile int uart_dma_tx_done;
	volatile int uart_dma_rx_done;

	int int_vector;

	volatile unsigned char* p_tx_lli;
	volatile unsigned char* p_rx_lli;
};

/*apis*/
void dw_uart_writel(struct dw_uart* dev, int reg, unsigned long val);
unsigned long dw_uart_readl(struct dw_uart* dev, int reg);

unsigned long dw_uart_calc_divisor(struct dw_uart* dev,
	unsigned long clock, unsigned long baudrate);

void dw_uart_set_line_control(struct dw_uart* dev, int cr);
unsigned long dw_uart_get_line_status(struct dw_uart* dev);
void dw_uart_set_break(struct dw_uart* dev, int set);
void dw_uart_setbrg(struct dw_uart* dev, int baud_divisor);
void dw_uart_set_modem_ctrl(struct dw_uart* dev, int mode);
void dw_uart_set_modem_ctl_bits(struct dw_uart* dev, int bits, int mode);
unsigned long dw_uart_get_modem_status(struct dw_uart* dev);

void dw_uart_enable_interrupt(struct dw_uart* dev, int en);
unsigned long dw_uart_get_int_status(struct dw_uart* dev);
unsigned long dw_uart_get_rcv_fifo_level(struct dw_uart* dev);
void dw_uart_set_fcr_rx_thres(struct dw_uart* dev, unsigned long rx_thres);

void dw_uart_init(struct dw_uart* dev, unsigned long pclk, int baudrate);
void dw_uart_init_no_modem(struct dw_uart* dev, unsigned long pclk, int baudrate);

void dw_uart_flush(struct dw_uart* dev);
void dw_uart_putc(struct dw_uart* dev, char c);
char dw_uart_getc(struct dw_uart* dev);
int dw_uart_tstc(struct dw_uart* dev);
void dw_uart_empty_fifo(struct dw_uart* dev);

int dw_uart_write(struct dw_uart* dev, const char* buf, int len);
int dw_uart_read(struct dw_uart* dev, char* buf, int len);
int dw_uart_read_non_block(struct dw_uart* dev, char* buf,
	int len,int timeout);

int dw_uart_write_mb(struct dw_uart* dev, const char* buf, int len);
int dw_uart_read_mb(struct dw_uart* dev, char* buf, int len);
#endif
