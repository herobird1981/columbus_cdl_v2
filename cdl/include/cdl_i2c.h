/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_I2C_H__
#define __CDL_I2C_H__


#define I2C_PORT_SIZE 0x1000

/*
 * Registers offset
*/
#define DW_IC_CON 		(0x0)
#define DW_IC_TAR 		(0x4)
#define DW_IC_SAR 		(0x8)
#define DW_IC_HS_MADDR		(0xC)
#define DW_IC_DATA_CMD		(0x10)
#define DW_IC_SS_SCL_HCNT 	(0x14)
#define DW_IC_SS_SCL_LCNT 	(0x18)
#define DW_IC_FS_SCL_HCNT 	(0x1c)
#define DW_IC_FS_SCL_LCNT 	(0x20)
#define DW_IC_HS_SCL_HCNT 	(0x24)
#define DW_IC_HS_SCL_LCNT 	(0x28)
#define DW_IC_INTR_STAT 	(0x2c)
#define DW_IC_INTR_MASK 	(0x30)
#define DW_IC_RAW_INTR_STAT     (0x34)
#define DW_IC_RX_TL 		(0x38)
#define DW_IC_TX_TL 		(0x3c)
#define DW_IC_CLR_INTR		(0x40)
#define DW_IC_CLR_RX_UNDER	(0x44)
#define DW_IC_CLR_RX_OVER 	(0x48)
#define DW_IC_CLR_TX_OVER 	(0x4c)
#define DW_IC_CLR_RD_REQ	(0x50)
#define DW_IC_CLR_TX_ABRT 	(0x54)
#define DW_IC_CLR_RX_DONE 	(0x58)
#define DW_IC_CLR_ACTIVITY	(0x5c)
#define DW_IC_CLR_STOP_DET	(0x60)
#define DW_IC_CLR_START_DET 	(0x64)
#define DW_IC_CLR_GEN_CALL	(0x68)
#define DW_IC_ENABLE		(0x6c)
#define DW_IC_STATUS		(0x70)
#define DW_IC_TXFLR 		(0x74)
#define DW_IC_RXFLR 		(0x78)
#define DW_IC_SDA_HOLD		(0x7c)
#define DW_IC_TX_ABRT_SOURCE	(0x80)
#define DW_IC_SLV_DATA_NACK_ONLY	(0x84)
#define DW_IC_DMA_CR		(0x88)
#define DW_IC_DMA_TDLR		(0x8C)
#define DW_IC_DMA_RDLR		(0x90)
#define DW_IC_SDA_SETUP 	(0x94)
#define DW_IC_ACK_GENERAL_CALL	(0x98)
#define DW_IC_ENABLE_STATUS 	(0x9c)
#define DW_IC_FS_SPKLEN 	(0xa0)
#define DW_IC_HS_SPKLEN 	(0xa4)
#define DW_IC_CLR_RESTART_DET	(0xa8)
#define DW_IC_COMP_PARAM_1	(0xf4)
#define DW_IC_COMP_VERSION	(0xf8)
#define DW_IC_COMP_TYPE 	(0xfc)

/* Worst case timeout for 1 byte is kept as 2ms */
#define I2C_BYTE_TO 	(2)
#define I2C_STOPDET_TO	(2)
#define I2C_BYTE_TO_BB	(I2C_BYTE_TO * 16)

/* i2c control register definitions */
#define IC_CON_STOP_DET_IFADDRESSED 0x0080
#define IC_CON_SD 	0x0040
#define IC_CON_RE 	0x0020
#define IC_CON_10BITADDRMASTER	0x0010
#define IC_CON_10BITADDR_SLAVE	0x0008
#define IC_CON_SPD_MSK		0x0006
#define IC_CON_SPD_SS 	0x0002
#define IC_CON_SPD_FS 	0x0004
#define IC_CON_SPD_HS 	0x0006
#define IC_CON_MM 	0x0001

/* i2c data buffer and command register definitions */
#define IC_CMD		0x0100
#define IC_STOP 	0x0200
#define IC_RESTART	(1<<10)

/* i2c interrupt status register definitions */
#define IC_GEN_CALL 	0x0800
#define IC_START_DET	0x0400
#define IC_STOP_DET 	0x0200
#define IC_ACTIVITY 	0x0100
#define IC_RX_DONE	0x0080
#define IC_TX_ABRT	0x0040
#define IC_RD_REQ 	0x0020
#define IC_TX_EMPTY 	0x0010
#define IC_TX_OVER	0x0008
#define IC_RX_FULL	0x0004
#define IC_RX_OVER	0x0002
#define IC_RX_UNDER 	0x0001

/* fifo threshold register definitions */
#define IC_TL0		0x00
#define IC_TL1		0x01
#define IC_TL2		0x02
#define IC_TL3		0x03
#define IC_TL4		0x04
#define IC_TL5		0x05
#define IC_TL6		0x06
#define IC_TL7		0x07
#define IC_RX_TL	IC_TL0
#define IC_TX_TL	IC_TL0

/* i2c enable register definitions */
#define IC_ENABLE_0B	0x0001
#define IC_ABORT	0x0002
#define IC_TX_CMD_BLOCK	0x0004

/* i2c status register	definitions */
#define IC_STATUS_SA	0x0040
#define IC_STATUS_MA	0x0020
#define IC_STATUS_RFF 	0x0010
#define IC_STATUS_RFNE	0x0008
#define IC_STATUS_TFE 	0x0004
#define IC_STATUS_TFNF	0x0002
#define IC_STATUS_ACT 	0x0001

/* Speed Selection */
#define IC_SPEED_MODE_STANDARD	1
#define IC_SPEED_MODE_FAST	2
#define IC_SPEED_MODE_MAX 3

#define I2C_MAX_SPEED 	3400000
#define I2C_FAST_SPEED		400000
#define I2C_STANDARD_SPEED	100000

#define NANO_TO_MICRO 	1000
#define NANO_TO_S 	1000000000

/* High and low times in different speed modes (in ns) */
#define MIN_SS_SCL_HIGHTIME	4000
#define MIN_SS_SCL_LOWTIME	4700
/* accoring to spec, the FS HIGH TIME should be 600, but for enlarge the marge, we change it as 700 */
#define MIN_FS_SCL_HIGHTIME	700 //600
#define MIN_FS_SCL_LOWTIME	1300
#define MIN_HS_SCL_HIGHTIME	60
#define MIN_HS_SCL_LOWTIME	160

#define DW_I2C_ROLE_MASTER	0
#define DW_I2C_ROLE_SLAVE 	1

#define DW_I2C_SLAVE_DEFAULT_ADDR (0x50)
#define DW_I2C_SLAVE_7BIT_ADDR	0
#define DW_I2C_SLAVE_10BIT_ADDR 1

struct dw_i2c {
	void* regs;
	unsigned long pclk;
	int speed;
	int master_addr_mode; /* 1 for 10 bit mode ; 0  7 bit mode*/

	int role; /*master or slave 0: master 1:slave*/

	int dma_mode;
	int dmac_idx;
	int dma_ch_tx;
	int dma_ch_rx;
	int dma_req_tx;
	int dma_req_rx;
	unsigned long dma_dummy;

	int int_vector;

	int slave_addr;
	int slave_addr_mode;
	int slave_reg_width; /*regs width:1,2,3,4 bytes*/
	int max_len;

	int slave_state; /*slave state: 0 wait regs offset; 1 data phase*/
	int slave_tx_ptr;
	int slave_rx_ptr;
	unsigned char slave_reg_off[4]; /*store regs offset*/
};

void dw_i2c_writel(struct dw_i2c* priv, unsigned long val, 	int reg);

unsigned long dw_i2c_readl(struct dw_i2c* priv, int reg);

void dw_i2c_init(struct dw_i2c* priv, unsigned long base, unsigned long pclk, int dma_mode);

/*for master mode*/
void dw_i2c_init_master(struct dw_i2c* priv, int speed,int addr_mode);
int dw_i2c_read(struct dw_i2c* priv, unsigned long dev, unsigned int addr,
	int alen, unsigned char* buffer, int len);
int dw_i2c_write(struct dw_i2c* priv, unsigned long dev, unsigned int addr,
	int alen, unsigned char* buffer, int len);

/*for slave mode*/
void dw_i2c_init_slave(struct dw_i2c* priv, int slaveaddr, int addr_mode,
	int reg_width, int max_len);
int dw_i2c_slave_transfer(struct dw_i2c* priv, unsigned char* buffer);

unsigned long dw_i2c_get_int_status(struct dw_i2c* priv);
unsigned long dw_i2c_get_raw_int_status(struct dw_i2c* priv);
void dw_i2c_set_int_mask(struct dw_i2c* priv, int en);
void dw_i2c_clear_int(struct dw_i2c* priv, int type);
void dw_i2c_clear_all_int(struct dw_i2c* priv);

int e2prom_i2c_read(struct dw_i2c* priv, u32 dev, uint addr, int alen, u8* buffer, int len);

unsigned long dw_i2c_get_abrt_source(struct dw_i2c* priv);

void dw_i2c_tx_abort_recovery(struct dw_i2c* priv);

#endif

