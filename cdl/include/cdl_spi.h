/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_SPI_H__
#define __CDL_SPI_H__

#define SPI_PORT_SIZE 0x800  // should equal to DMAC_MAX_SIZE_PER_BLOCK 
#define SPI_FIFO_DEPTH	32

/* Register offsets */
#define BRITE_SPI_TXREG 		0x00
#define BRITE_SPI_RXREG 		0x04
#define BRITE_SPI_CSTAT 		0x08
#define BRITE_SPI_INTSTAT 		0x0c
#define BRITE_SPI_INTEN 		0x10
#define BRITE_SPI_INTCLR		0x14
#define BRITE_SPI_GCTL			0x18
#define BRITE_SPI_CCTL			0x1c
#define BRITE_SPI_SPBRG 		0x20
#define BRITE_SPI_RXDNR 		0x24
#define BRITE_SPI_TXDNR 		0x28
#define BRITE_SPI_SCSR			0x2c

/*0x8--Current Status Register (CSTAT) */
#define SPI_TXEPT		0x1 /*Transmitter empty bit*/
#define SPI_RXAVL		0x2 /*Receive available byte data message*/
#define SPI_TXFULL		0x4 /*Transmitter FIFO Full Status bit*/
#define SPI_RXAVL_4		0x8 /*Receive available 4 byte data message*/

/*0xC--Interrupt Status Register (INTSTAT) */
/*0x10--Interrupt Enable Register (INTEN) */
/*0x14--Interrupt Clear Register (INTCLR) */
#define SPI_TX_INTF			0x1 /* transmitter FIFO is available */
#define SPI_RX_INTF			0x2 /* receiver FIFO has available data */
#define SPI_UNDERRUN_INTF	0x4 /*SPI slave transmitter underrun flag bit */
#define SPI_RXOERR_INTF		0x8 	/*Receiver Overrun Error interrupt Flag bit */
#define SPI_RXMATCH_INTF	0x10 /*SPI master completes a receive task */
#define SPI_RXFIFO_FULL_INTF	0x20 /*RX FIFO Full interrupt flag bit*/
#define SPI_TXEPT_INTF		0x40 	/*Transmitter empty interrupt flag bit*/

#define ENABLE_ALL_INTERRUPTS		0x7f
#define DISABLE_ALL_INTERRUPTS		(~ENABLE_ALL_INTERRUPTS)

#define TX_MASTER_NONE		(~(SPI_RXMATCH_INTF|SPI_UNDERRUN_INTF|SPI_RX_INTF))
#define TX_SLAVE_NONE		(~(SPI_RXMATCH_INTF|SPI_UNDERRUN_INTF|SPI_RX_INTF))
#define RX_MASTER_NONE		(~(SPI_UNDERRUN_INTF|SPI_TX_INTF))
#define RX_SLAVE_NONE		(~(SPI_RXMATCH_INTF|SPI_UNDERRUN_INTF|SPI_TX_INTF))

#define CLEAR_ALL_INTERRUPTS	0x7f

/*0x18--Global Control Register (GCTL) */
#define SPI_EN			0x1	/*SPI Select bits*/
#define SPI_INT_EN		0x2	/*SPI interrupt enable bit */
#define SPI_MM			0x4 /*Master Mode bit */
#define SPI_TXEN		0x8	/*Transmit Enable bit */
#define SPI_RXEN		0x10 /*Receive Enable bit*/
#define SPI_RXTXTLF_MASK	0x1E0
/* For General SPI */
#define SPI_RXTLF_1		0x00	/*RX FIFO trigger level bit --- >= 1 */
#define SPI_RXTLF_4		0x20	/*RX FIFO trigger level bit --- >= 4 */
#define SPI_RXTLF_8		0x40	/*RX FIFO trigger level bit --- >= 8 */
#define SPI_RXTLF_16	0x60	/*RX FIFO trigger level bit --- >= 16 */
#define SPI_TXTLF_NFULL		0x00 /*TX FIFO trigger level bit --- not full */
#define SPI_TXTLF_4		0x80 /*TX FIFO trigger level bit --- <= 0x1C */
#define SPI_TXTLF_8		0x100 /*TX FIFO trigger level bit --- <= 0x18 */
#define SPI_TXTLF_16		0x180 /*TX FIFO trigger level bit --- <= 0x10 */

/* For RF SPI */
#define SPI_RXTLF_HALF		0x20	/*RX FIFO trigger level bit --- >= HALF */
#define SPI_TXTLF_HALF		0x80	/*TX FIFO trigger level bit --- <= HALF */


#define SPI_DMAMODE		0x200 /*DMA access mode bit */
#define SPI_CSN_SEL		0x400 /*Select signal that from software or hardware*/
#define SPI_DUAL_MODE	0x1000/*dual_mod when SPI as a master */
#define SPI_QUAD_MODE	0x2000	/*quad_mod when SPI as a master */
#define SPI_TI_MODE		0x4000	/*TI mode or normal mode*/

#define SPI_MASTER_MODE		(0x1UL << 2)
#define SPI_SLAVE_MODE		(0x0UL << 2)

#define SPI_NORMAL_MODE		0x0

/*0x1C--Common Control Register (CCTL) */
#define SPI_MODE_MASK		0x3
#define SPI_LSBFE			0x4 /*LSB First Enable */
#define SPI_RXEDGE			0x8 /*Master mode rx data sample edge select*/
#define SPI_TXEDGE			0x10 /*Slave mode tx data transmit phase adjust bit */
#define SPI_LEN_MASK		0x1f00 /*spi length bits */

#define SPI_CKPH_L			0x1	/*Clock Phase Select bit*/
#define SPI_CKPH_H			0x0
#define SPI_CKPL_H			0x0	/*SPI Clock Polarity Select bit*/
#define SPI_CKPL_L			0x2

/*
#define SPI_MODE_0	(SPI_CKPL_H | SPI_CKPH_L) //ckpl=0,ckph=1
#define SPI_MODE_1	(SPI_CKPL_H | SPI_CKPH_H) //ckpl=0,ckph=0
#define SPI_MODE_2	(SPI_CKPL_L | SPI_CKPH_L) //ckpl=1,ckph=1
#define SPI_MODE_3	(SPI_CKPL_L | SPI_CKPH_H) //ckpl=1,ckph=0
*/

#define RX_TIMEOUT			1000	/* timeout in ms */
#define TX_TIMEOUT			1000	/* timeout in ms */

/**
 * enum ssp_data_size - number of bits in one data element
 */
enum SPI_DATA_SIZE {
	SPI_DATA_BITS_4 = 0x03UL,
	SPI_DATA_BITS_5,
	SPI_DATA_BITS_6,
	SPI_DATA_BITS_7,
	SPI_DATA_BITS_8,
	SPI_DATA_BITS_9,
	SPI_DATA_BITS_10,
	SPI_DATA_BITS_11,
	SPI_DATA_BITS_12,
	SPI_DATA_BITS_13,
	SPI_DATA_BITS_14,
	SPI_DATA_BITS_15,
	SPI_DATA_BITS_16,
	SPI_DATA_BITS_17,
	SPI_DATA_BITS_18,
	SPI_DATA_BITS_19,
	SPI_DATA_BITS_20,
	SPI_DATA_BITS_21,
	SPI_DATA_BITS_22,
	SPI_DATA_BITS_23,
	SPI_DATA_BITS_24,
	SPI_DATA_BITS_25,
	SPI_DATA_BITS_26,
	SPI_DATA_BITS_27,
	SPI_DATA_BITS_28,
	SPI_DATA_BITS_29,
	SPI_DATA_BITS_30,
	SPI_DATA_BITS_31,
	SPI_DATA_BITS_32
};

#define BRITE_SPI_SPBRG_MASK 		0xFFFF

/*0x28--Slave Chip Select Register (SCSR) */
#define SPI_CSN_SELECT	0xfe	/*Chip select output signal in	Master mode*/
#define SPI_CSN_NOT_SELECT	0xff /*Chip select output signal */


#define SPI_TMOD_TR 	(SPI_TXEN|SPI_RXEN) 	/* xmit & recv */
#define SPI_TMOD_TO 	(SPI_TXEN)		/* xmit only */
#define SPI_TMOD_RO 	(SPI_RXEN)		/* recv only */

/* SPI tranfer flags*/
#define SPI_XFER_BEGIN	(1<<0)	/* Assert CS before transfer */
#define SPI_XFER_END	(1<<1)	/* Deassert CS after transfer */
#define SPI_XFER_ONCE 	(SPI_XFER_BEGIN | SPI_XFER_END)

struct brite_spi {
	void* regs;
	unsigned long pclk; /* input clock freq */
	unsigned long freq; /* data rate */
	unsigned long actual_freq; /* actual freq running */

	unsigned long fifo_depth; /* depth of the FIFO buffer */

	unsigned char mode; 	/* master or slave */
	unsigned char type; 	/* TI/normal */
	unsigned char phase;	/* Clock Phase and Clock Polarity */
	unsigned char cs; 	/* chip select pin */
	unsigned char lsbfe;	/* LSB first or MSB first: 1 LSB*/
	int bits_per_word; /* how much bits to transfer: spilen, 4-32bit*/
	unsigned char dma_mode;

	unsigned char step; /*bytes one transfer word,depence on bits_per_word */
	unsigned char tmode;/* TR/TO/RO */

	unsigned char* tx;
	unsigned char* tx_end;
	unsigned char* rx;
	unsigned char* rx_end;
	unsigned long len;

	int dma_burst_len;
	int dmac_idx;
	int dma_ch_tx;
	int dma_ch_rx;
	int dma_req_tx;
	int dma_req_rx;
	unsigned long dummy_dma_buf;
	int int_vector;
};

/*apis*/
void brite_spi_write(struct brite_spi* priv, unsigned long offset, unsigned long val);

unsigned long brite_spi_read(struct brite_spi* priv, unsigned long offset);

void brite_spi_set_int_enable(struct brite_spi* priv, int enable);
unsigned long brite_spi_get_int_status(struct brite_spi* priv);
void brite_spi_set_int_mask(struct brite_spi* priv, int mask);
void brite_spi_set_int_clear(struct brite_spi* priv, int clr);
void brite_spi_init(struct brite_spi* priv, unsigned long pclk,
	int master_slave, int baudrate, int dma_mode, int type, int phase,
	int cs, int bus_width);
void brite_spi_enable_txrx(struct brite_spi* priv, unsigned long dir);
void brite_spi_set_txrx_threshold(struct brite_spi* priv, unsigned long thres);
void brite_spi_empty_fifo(struct brite_spi* priv);
void brite_spi_dump_status(struct brite_spi* spi);

void brite_spi_init_hw(struct brite_spi* priv);
void brite_spi_set_speed(struct brite_spi* priv, unsigned long speed);
int brite_spi_xfer(struct brite_spi* priv, unsigned int bitlen,
	const void* dout, void* din, unsigned long flags);
void brite_spi_rx_cnt(struct brite_spi* priv, unsigned int rx_cnt);

unsigned long brite_spi_xfer_slave(struct brite_spi* priv, unsigned int len,
	const void* dout, void* din);
#endif
