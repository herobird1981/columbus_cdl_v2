/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_i2c.h"

#include "cdl_dmac.h"
#include "timer.h"

#define I2C_TIMEOUT (100) //ms

void dw_i2c_writel(struct dw_i2c* priv, unsigned long val, int reg)
{
	__raw_writel(val, (unsigned long)priv->regs + reg);
}

unsigned long dw_i2c_readl(struct dw_i2c* priv, int reg)
{
	return __raw_readl((unsigned long)priv->regs + reg);
}

/*
 * dw_i2c_set_speed - Set the i2c speed mode (standard, high, fast)
 * @i2c_spd:	required i2c speed mode
 *
 * Set the i2c speed mode (standard, high, fast)
 */
static void dw_i2c_set_speed(struct dw_i2c* priv, int i2c_spd)
{
	unsigned int cntl;
	unsigned int hcnt,lcnt;
	unsigned int enbl;
	int micro_fix_val;

	/* to set speed cltr must be disabled */
	enbl = dw_i2c_readl(priv, DW_IC_ENABLE);
	enbl &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, enbl, DW_IC_ENABLE);

	cntl = (dw_i2c_readl(priv, DW_IC_CON) & (~IC_CON_SPD_MSK));

		switch (i2c_spd) {
#if 0			
	case IC_SPEED_MODE_MAX:
		cntl |= IC_CON_SPD_HS;
		hcnt = (priv->pclk/1000000 * MIN_HS_SCL_HIGHTIME) / NANO_TO_MICRO ; /*12 -- for 1st silicon*/
		dw_i2c_writel(priv, hcnt, DW_IC_HS_SCL_HCNT);
		lcnt = (priv->pclk/1000000 * (294- MIN_HS_SCL_HIGHTIME)) / NANO_TO_MICRO - 43/*rise tim*/;
		dw_i2c_writel(priv, lcnt, DW_IC_HS_SCL_LCNT);
		break;
#endif
	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = (priv->pclk/1000000 * MIN_SS_SCL_HIGHTIME) / NANO_TO_MICRO ;
		dw_i2c_writel(priv, hcnt, DW_IC_SS_SCL_HCNT);

		micro_fix_val = (priv->regs == I2C0_BASE) ? (-28) : (-32); 
		lcnt = (priv->pclk/1000000 * (10000- MIN_SS_SCL_HIGHTIME)) / NANO_TO_MICRO + micro_fix_val;/*rise tim*/ //10.000us
		dw_i2c_writel(priv, lcnt, DW_IC_SS_SCL_LCNT);

		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = (priv->pclk/1000000 * MIN_FS_SCL_HIGHTIME) / NANO_TO_MICRO ; /*12 -- for 1st silicon*/
		dw_i2c_writel(priv, hcnt, DW_IC_FS_SCL_HCNT);

		micro_fix_val = (priv->regs == I2C0_BASE) ? (-28) : (-36);
		lcnt = (priv->pclk/1000000 * (2500- MIN_FS_SCL_HIGHTIME)) / NANO_TO_MICRO + micro_fix_val/*rise tim*/;
		dw_i2c_writel(priv, lcnt, DW_IC_FS_SCL_LCNT);
		break;
	}
	dw_i2c_writel(priv, cntl, DW_IC_CON);

	/* Enable back i2c now speed set */
	enbl |= IC_ENABLE_0B;
	dw_i2c_writel(priv, enbl, DW_IC_ENABLE);
}

/*
 * dw_i2c_set_bus_speed - Set the i2c speed
 * @speed:	required i2c speed
 *
 * Set the i2c speed.
 */
static unsigned int dw_i2c_set_bus_speed(struct dw_i2c* priv,
	unsigned int speed)
{
	int i2c_spd;
#if 0
	if (speed >= I2C_MAX_SPEED) 
	{
		i2c_spd = IC_SPEED_MODE_MAX;
	} 
	else 
#endif
	if (speed >= I2C_FAST_SPEED) 
	{
		i2c_spd = IC_SPEED_MODE_FAST;
	} 
	else 
	{
		i2c_spd = IC_SPEED_MODE_STANDARD;
	}

	dw_i2c_set_speed(priv, i2c_spd);
	priv->speed = speed;

	return 0;
}

/*
 * dw_i2c_set_slave_address - Sets the target slave address
 * @i2c_addr: target i2c address
 *
 * Sets the target slave address.
 */
static void dw_i2c_set_slave_address(struct dw_i2c* priv, unsigned int i2c_addr)
{
	unsigned int enbl;
	unsigned int tar = 0;

	/* Disable i2c */
	enbl = dw_i2c_readl(priv, DW_IC_ENABLE);
	enbl &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, enbl, DW_IC_ENABLE);

	dw_i2c_writel(priv, i2c_addr & 0x3ff, DW_IC_TAR);

	/* Enable i2c */
	enbl = dw_i2c_readl(priv, DW_IC_ENABLE);
	enbl |= IC_ENABLE_0B;
	dw_i2c_writel(priv, enbl, DW_IC_ENABLE);
}

/*
 * dw_i2c_flush_rxfifo - Flushes the i2c RX FIFO
 *
 * Flushes the i2c RX FIFO
 */
static void dw_i2c_flush_rxfifo(struct dw_i2c* priv)
{
	unsigned long long start = get_timer(0);

	while (dw_i2c_readl(priv, DW_IC_STATUS) & IC_STATUS_RFNE) {
		dw_i2c_readl(priv, DW_IC_DATA_CMD);
		if(get_timer(start) > I2C_TIMEOUT) {
			info("i2c flush rxfifo timeout\n");
			break;
		}
	}
}

/*
 * dw_i2c_wait_for_bb - Waits for bus busy
 *
 * Waits for bus busy
 */
static int dw_i2c_wait_for_bb(struct dw_i2c* priv)
{
	unsigned long long start_time_bb = get_timer(0);
	u32 raw_stat;
	int ret = 0;

	while ((dw_i2c_readl(priv, DW_IC_STATUS) & IC_STATUS_MA) ||
		!(dw_i2c_readl(priv, DW_IC_STATUS) & IC_STATUS_TFE)) 
	{
		/* Evaluate timeout */
		if (get_timer(start_time_bb) > (unsigned long)(I2C_BYTE_TO_BB)) 
		{
			ret = 1;
			break;
		}
	}
	
	/*check tx abort, and clear it if exists,consider write fail,and we abort write*/
	raw_stat = dw_i2c_readl(priv, DW_IC_RAW_INTR_STAT);
	if (raw_stat & IC_TX_ABRT) 
	{
		dw_i2c_readl(priv, DW_IC_CLR_TX_ABRT);
	}
	
	return ret;
}

/*send address first*/
static int dw_i2c_xfer_init(struct dw_i2c* priv, unsigned long chip, uint addr, int alen)
{
	if (dw_i2c_wait_for_bb(priv)) 
	{
		return 1;
	}

	dw_i2c_set_slave_address(priv, chip);
	while (alen) 
	{
		alen--;
		
		/* high byte address going out first */
		dw_i2c_writel(priv, (addr >> (alen * 8)) & 0xFF, DW_IC_DATA_CMD);
	}
	
	return 0;
}

static int dw_i2c_xfer_finish(struct dw_i2c* priv)
{
	u64 start_stop_det = get_timer(0);
	unsigned long enbl;

	while (1) 
	{
		if ((dw_i2c_readl(priv, DW_IC_RAW_INTR_STAT) & IC_STOP_DET)) 
		{
			dw_i2c_readl(priv, DW_IC_CLR_STOP_DET);
			break;
		} 
		else if (get_timer(start_stop_det) > I2C_STOPDET_TO) 
		{
			break;
		}
	}

	if (dw_i2c_wait_for_bb(priv)) 
	{
		/*maybe send fail,we should abort to pervent something left in tx fifo*/
		enbl = dw_i2c_readl(priv, DW_IC_ENABLE);
		enbl |= IC_ABORT;
		dw_i2c_writel(priv, enbl, DW_IC_ENABLE);
		debug("Timed out waiting for bus,abort it\n");
	}

	dw_i2c_flush_rxfifo(priv);

	return 0;
}

static int dw_i2c_dma_transfer(struct dw_i2c* priv, const char* buf, int len)
{
	struct dw_dmac* p_dma_tx = &dma_dev_list[priv->dmac_idx];
	int ret = 0;
	u32 enbl;

	enbl = dw_i2c_readl(priv, DW_IC_DMA_CR);
	enbl |= 1<<1;
	dw_i2c_writel(priv, enbl, DW_IC_DMA_CR);

	/* config src */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 0, BYTE_WIDTH,
		BURST_1, INCREASE,  MEM_TO_PERI, priv->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 1, BYTE_WIDTH,
		BURST_1, FIXED,  MEM_TO_PERI, priv->dma_req_tx);

	ret = dw_dmac_channel_single_transfer_poll(p_dma_tx, priv->dma_ch_tx,
		(unsigned long) buf, (unsigned long)priv->regs + DW_IC_DATA_CMD,
		len);

	enbl = dw_i2c_readl(priv, DW_IC_DMA_CR);
	enbl &= ~(1<<1);
	dw_i2c_writel(priv, enbl, DW_IC_DMA_CR);
	return ret;
}

static int dw_i2c_dma_receive(struct dw_i2c* priv, const char* buf, int len)
{
	struct dw_dmac* p_dma_rx = &dma_dev_list[priv->dmac_idx];
	struct dw_dmac* p_dma_tx = &dma_dev_list[priv->dmac_idx];
	int ret = 0;

	priv->dma_dummy = IC_CMD;

	dw_i2c_writel(priv, 0x3, DW_IC_DMA_CR);

	/*rx dma*/
	/* config src */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 0, BYTE_WIDTH,
		BURST_1, FIXED,  PERI_TO_MEM, priv->dma_req_rx);
	/* config dst */
	dw_dmac_channel_config(p_dma_rx, priv->dma_ch_rx, 1, BYTE_WIDTH,
		BURST_1, INCREASE,  PERI_TO_MEM, priv->dma_req_rx);

	ret = dw_dmac_channel_single_transfer(p_dma_rx, priv->dma_ch_rx,
		(unsigned long)priv->regs + DW_IC_DATA_CMD, (unsigned long) buf,
		len,0);

	/*tx dummy send: read command*/
	/* config src */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 0, WORD_WIDTH,
		BURST_1, FIXED,  MEM_TO_PERI, priv->dma_req_tx);
	/* config dst */
	dw_dmac_channel_config(p_dma_tx, priv->dma_ch_tx, 1, WORD_WIDTH,
		BURST_1, FIXED,  MEM_TO_PERI, priv->dma_req_tx);

	ret = dw_dmac_channel_single_transfer_poll(p_dma_tx, priv->dma_ch_tx,
		(u32)&priv->dma_dummy, (unsigned long)priv->regs + DW_IC_DATA_CMD,
		len);
	ret = dw_dmac_channel_wait_done(p_dma_rx,priv->dma_ch_rx,DMAC_TIMEOUT);
	dw_i2c_writel(priv, 0, DW_IC_DMA_CR);
	return ret;
}

/*
 * dw_i2c_read - Read from i2c memory
 * @chip: target i2c address
 * @addr: address to read from
 * @alen:
 * @buffer: buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int dw_i2c_read(struct dw_i2c* priv, u32 dev, uint addr, int alen, u8* buffer, int len)
{
	unsigned long long start_time_rx;
	int ret=0;

	if (dw_i2c_xfer_init(priv, dev, addr, alen)) 
	{
		return 1;
	}

	if (priv->dma_mode) 
	{
		ret = dw_i2c_dma_receive(priv, (char*)buffer, len);
		if (ret) 
		{
			debug("Timed out. i2c dma read err\n");
			ret = 1;
		}
		
		dw_i2c_writel(priv, IC_CMD|IC_STOP,DW_IC_DATA_CMD);
	} 
	else 
	{
		start_time_rx = get_timer(0);

		while (len) 
		{
			if (len == 1) 
			{
	  			dw_i2c_writel(priv, IC_CMD | IC_STOP, DW_IC_DATA_CMD);
			} 
			else 
			{
				dw_i2c_writel(priv, IC_CMD, DW_IC_DATA_CMD);
			}

			if (dw_i2c_readl(priv, DW_IC_STATUS) & IC_STATUS_RFNE) 
			{
				*buffer++ = (unsigned char)dw_i2c_readl(priv, DW_IC_DATA_CMD);
				len--;
				start_time_rx = get_timer(0);
			} 
			else if (get_timer(start_time_rx) > I2C_BYTE_TO) 
			{
				debug("Timed out. i2c read err\n");
				ret = 1;
				break;
			}
		}
	}

	dw_i2c_xfer_finish(priv);
	return ret;
}

/*
 * dw_i2c_read - Read from i2c memory
 * @chip: target i2c address
 * @addr: address to read from
 * @alen:
 * @buffer: buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int e2prom_i2c_read(struct dw_i2c* priv, u32 dev, uint addr, int alen, u8* buffer, int len)
{
	unsigned long long start_time_rx;
	int ret = 0;
	int tmp_len = len;
	
	// start + dev sel + address write
	if (dw_i2c_xfer_init(priv, dev, addr, alen)) 
	{
		return 1;
	}

#if 0
	if (priv->dma_mode)
	{
		while (len)
		{
			if (tmp_len == len)
			{
				if (len > 1)
				{
					// start + dev sel read
					dw_i2c_writel(priv, (IC_RESTART | IC_CMD | dev), DW_IC_DATA_CMD);
				}
				else
				{
					dw_i2c_writel(priv, (IC_RESTART | IC_CMD | IC_STOP | dev), DW_IC_DATA_CMD);	
				}
			}
			else  
			{
				if (len == 1)
				{
	  				dw_i2c_writel(priv, (IC_CMD | IC_STOP), DW_IC_DATA_CMD);
				}
				else
				{
					dw_i2c_writel(priv, (IC_CMD), DW_IC_DATA_CMD);
				}
			} 

			ret = dw_i2c_dma_receive(priv, (char *)buffer, 1);
			if (ret) 
			{
				debug("Timed out. i2c dma read err\n");
				ret = 1;
				break;
			}

			buffer++;

			len--;
		}
	} 
	else
#endif		
	{
		start_time_rx = get_timer(0);

		while (len) 
		{
			if (tmp_len == len)
			{
				if (len > 1)
				{
					// start + dev sel read
					dw_i2c_writel(priv, (IC_RESTART | IC_CMD | dev), DW_IC_DATA_CMD);
				}
				else
				{
					dw_i2c_writel(priv, (IC_RESTART | IC_CMD | IC_STOP | dev), DW_IC_DATA_CMD);	
				}
			}
			else  
			{
				if (len == 1)
				{
	  				dw_i2c_writel(priv, (IC_CMD | IC_STOP), DW_IC_DATA_CMD);
				}
				else
				{
					dw_i2c_writel(priv, (IC_CMD), DW_IC_DATA_CMD);
				}
			} 

			if (dw_i2c_readl(priv, DW_IC_STATUS) & IC_STATUS_RFNE) 
			{
				*buffer++ = (unsigned char)dw_i2c_readl(priv, DW_IC_DATA_CMD);
				len--;
				start_time_rx = get_timer(0);
			} 
			else if (get_timer(start_time_rx) > I2C_BYTE_TO) 
			{
				debug("Timed out. i2c read err\n");
				ret = 1;
				break;
			}
		}
	}

	dw_i2c_xfer_finish(priv);
	return ret;
}


/*
 * dw_i2c_write - Write to i2c memory
 * @chip: target i2c address
 * @addr: address to read from
 * @alen:
 * @buffer: buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
int dw_i2c_write(struct dw_i2c* priv, u32 dev, uint addr, int alen, u8* buffer, int len)
{
	int nb = len;
	unsigned long long start_time_tx;
	int ret=0;

	if (dw_i2c_xfer_init(priv, dev, addr, alen)) 
	{
		return 1;
	}

	if (priv->dma_mode) 
	{
		ret = dw_i2c_dma_transfer(priv, (const char*)buffer, (len - 1));
		if (ret) 
		{
			debug("Timed out. i2c write err\n");
			ret = 1;
		}
		
		dw_i2c_writel(priv, (buffer[len - 1] | IC_STOP), DW_IC_DATA_CMD);
	} 
	else 
	{			
		start_time_tx = get_timer(0);
		while (len) 
		{
			if (dw_i2c_readl(priv, DW_IC_STATUS) & IC_STATUS_TFNF) 
			{
				if (--len == 0) 
				{
	  				dw_i2c_writel(priv, (*buffer | IC_STOP), DW_IC_DATA_CMD);
				} 
				else 
				{
	  			  	dw_i2c_writel(priv, *buffer, DW_IC_DATA_CMD);
				}
				
				buffer++;
				start_time_tx = get_timer(0);
			} 
			else if (get_timer(start_time_tx) > (nb * I2C_BYTE_TO)) 
			{
				debug("Timed out. i2c write err\n");
				ret = 1;
				break;
			}
		}
	}

	dw_i2c_xfer_finish(priv);
	return ret;
}

/*
 * dw_i2c_init_hw - Init function
 * @speed:	required i2c speed
 *
 * Initialization function.
 */
static void dw_i2c_init_hw(struct dw_i2c* priv, int speed)
{
	unsigned int enbl;
	u32 con = 0;
	int i;

	/* Disable i2c */
	enbl = dw_i2c_readl(priv, DW_IC_ENABLE);
	enbl &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, enbl, DW_IC_ENABLE);

	/* mask and clear all interrupt */
	dw_i2c_writel(priv, 0, DW_IC_INTR_MASK);
	for (i = 0; i < 11; i++) {
		dw_i2c_readl(priv, DW_IC_CLR_INTR + i * 4);
	}

	/* config master/slave mode */
	if (priv->role == DW_I2C_ROLE_MASTER) {
		con = IC_CON_SD | IC_CON_SPD_FS | IC_CON_MM | IC_CON_RE;
		if (priv->master_addr_mode)/*10b*/
			con |= IC_CON_10BITADDRMASTER;
		dw_i2c_writel(priv,con,DW_IC_CON);
	} else {
		/* set slave addr */
		dw_i2c_writel(priv, priv->slave_addr, DW_IC_SAR);

		if (priv->slave_addr_mode == DW_I2C_SLAVE_10BIT_ADDR) {
			con |= (1 << 3);
		}
		con |= IC_CON_SPD_FS;
		dw_i2c_writel(priv, con, DW_IC_CON);
	}
	dw_i2c_writel(priv, IC_RX_TL, DW_IC_RX_TL);
	dw_i2c_writel(priv, IC_TX_TL, DW_IC_TX_TL);
	if (priv->role == DW_I2C_ROLE_MASTER)
	{
		dw_i2c_set_bus_speed(priv, speed);
	}
	dw_i2c_writel(priv, 0, DW_IC_INTR_MASK);

	/* Enable i2c */
	enbl = dw_i2c_readl(priv, DW_IC_ENABLE);
	enbl |= IC_ENABLE_0B;
	enbl &= ~IC_TX_CMD_BLOCK;
	dw_i2c_writel(priv, enbl, DW_IC_ENABLE);
}

void dw_i2c_init(struct dw_i2c* priv, unsigned long base, unsigned long pclk, int dma_mode)
{
	priv->regs = (void*)base;
	priv->pclk = pclk;
	priv->dma_mode = dma_mode;
	priv->role = DW_I2C_ROLE_MASTER;

	/* slave mode default para */
	priv->slave_addr = DW_I2C_SLAVE_DEFAULT_ADDR;
	priv->slave_addr_mode = DW_I2C_SLAVE_7BIT_ADDR;
}
void dw_i2c_init_master(struct dw_i2c* priv, int speed,int addr_mode)
{
	priv->role = DW_I2C_ROLE_MASTER;
	priv->master_addr_mode = addr_mode;
	dw_i2c_init_hw(priv, speed);
}
void dw_i2c_init_slave(struct dw_i2c* priv, int slaveaddr, int addr_mode, int reg_width, int max_len)
{
	priv->role = DW_I2C_ROLE_SLAVE;
	priv->slave_addr = slaveaddr;
	priv->slave_addr_mode = addr_mode;
	priv->slave_reg_width = reg_width;
	priv->max_len = max_len;

	memset(priv->slave_reg_off, 0, 4);
	priv->slave_state = 0;
	priv->slave_tx_ptr = 0;
	priv->slave_rx_ptr = 0;

	dw_i2c_init_hw(priv, 0);
}

/*
slave received data routine
return: 1  for fifo empty;  0 reached max request len
*/
int dw_i2c_slave_recv_data(struct dw_i2c* priv, u8* buffer, int off, int max)
{
	u32 status;

	do {
		status = dw_i2c_readl(priv, DW_IC_STATUS);
		
		if (status & IC_STATUS_RFNE) 
		{
			if (priv->slave_rx_ptr + off < max) 
			{
				buffer[off + priv->slave_rx_ptr] = (u8)dw_i2c_readl(priv, DW_IC_DATA_CMD);
				priv->slave_rx_ptr++;
			} 
			else 
			{
				return 0;
			}
		}
	} while (status & IC_STATUS_RFNE);
	
	if (priv->slave_rx_ptr + off < max)
	{
		return 1;
	}
	
	return 0;
}

#define MZX_BUFFER_SIZE (128*1024)
int dw_i2c_slave_transfer(struct dw_i2c* priv, u8* buffer)
{
	u32 status;
	u32 raw_stat;
	unsigned long off;
	unsigned char val;

	// master write request	
	while(1)
	{
		status = dw_i2c_readl(priv, DW_IC_STATUS);
		if (status & IC_STATUS_RFNE)
		{
			if (priv->slave_rx_ptr < MZX_BUFFER_SIZE) 
			{
				buffer[priv->slave_rx_ptr++] = (u8)dw_i2c_readl(priv, DW_IC_DATA_CMD);
			}
			else
			{
				// have read all data,dummy read to clear fifo
				dw_i2c_readl(priv, DW_IC_DATA_CMD);
			}
		}
		else
			break;
	}	

	// master read request    
	raw_stat = dw_i2c_readl(priv, DW_IC_INTR_STAT);
	if (raw_stat & IC_RD_REQ) 
	{
		// READ clear RD_REQ
		dw_i2c_readl(priv, DW_IC_CLR_RD_REQ);
		
		// reg off not enough,something wrong for the protocol
		if (raw_stat & IC_START_DET) 
		{
			//priv->slave_state = 1;
			priv->slave_tx_ptr = 0;
			dw_i2c_readl(priv, DW_IC_CLR_START_DET);
		}

		// tx fifo not empty when request read,clr the interrupt
		if (raw_stat & IC_TX_ABRT) 
		{
			dw_i2c_readl(priv, DW_IC_CLR_TX_ABRT);
		}
		
		// write until fifo full or master stop
		status = dw_i2c_readl(priv, DW_IC_STATUS);
		if (status & IC_STATUS_TFNF) 
		{
			val = (priv->slave_tx_ptr < MZX_BUFFER_SIZE) ?  buffer[priv->slave_tx_ptr++] : 0;
			dw_i2c_writel(priv, val, DW_IC_DATA_CMD);
		}
	}

	if (raw_stat & IC_STOP_DET) 
	{
		priv->slave_tx_ptr = 0;
		priv->slave_rx_ptr = 0;
		dw_i2c_readl(priv, DW_IC_CLR_STOP_DET);
	}

	return 0;
}

unsigned long dw_i2c_get_int_status(struct dw_i2c* priv)
{
	return dw_i2c_readl(priv, DW_IC_INTR_STAT);
}
unsigned long dw_i2c_get_raw_int_status(struct dw_i2c* priv)
{
	return dw_i2c_readl(priv, DW_IC_RAW_INTR_STAT);
}
void dw_i2c_set_int_mask(struct dw_i2c* priv, int en)
{
	dw_i2c_writel(priv, en, DW_IC_INTR_MASK);
}
void dw_i2c_clear_int(struct dw_i2c* priv, int type)
{
	dw_i2c_readl(priv, type);
}
void dw_i2c_clear_all_int(struct dw_i2c* priv)
{
	int i;
	for (i = 0; i < 11; i++) {
		dw_i2c_readl(priv, DW_IC_CLR_INTR + i * 4);
	}
}

unsigned long dw_i2c_get_abrt_source(struct dw_i2c* priv)
{
	return dw_i2c_readl(priv, DW_IC_TX_ABRT_SOURCE);
}

void dw_i2c_tx_abort_recovery(struct dw_i2c* priv)
{
	u32 tmp;
	struct dw_dmac* p_dma_rx = &dma_dev_list[priv->dmac_idx];
	struct dw_dmac* p_dma_tx = &dma_dev_list[priv->dmac_idx];
	
	dw_dmac_channel_abort(p_dma_rx, priv->dma_ch_rx);
	dw_dmac_channel_abort(p_dma_tx, priv->dma_ch_tx);

	tmp = dw_i2c_readl(priv, DW_IC_DMA_CR);
	tmp &= ~(1<<1);
	dw_i2c_writel(priv, tmp, DW_IC_DMA_CR);

	tmp = dw_i2c_readl(priv, DW_IC_ENABLE);
	tmp |= IC_ABORT;
	dw_i2c_writel(priv, tmp, DW_IC_ENABLE);	
	mdelay(50);
    //while(!(dw_i2c_readl(priv,  DW_IC_RAW_INTR_STAT ) & IC_TX_ABRT));
	tmp = dw_i2c_readl(priv, DW_IC_TX_ABRT_SOURCE);
	dw_i2c_readl(priv, DW_IC_CLR_TX_ABRT);

    //tmp = dw_i2c_readl(priv, DW_IC_ENABLE);
	//tmp &= ~IC_ABORT;
	//dw_i2c_writel(priv, tmp, DW_IC_ENABLE);	
}
