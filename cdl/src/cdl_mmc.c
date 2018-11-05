/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_mmc.h"
#include "cdl_pll.h"

//#define FORCE_USE_SRAM_DMA_BUFFER
#ifndef FORCE_USE_SRAM_DMA_BUFFER
unsigned long __cur_idmac[IDMAC_DESC_MAX_SIZE];
unsigned long __dmac_buffer[MAX_BLOCK_PER_TRANSFER*512 / 4];
#else
#define SRAM_BASE	0x10000000
unsigned long * __cur_idmac = (unsigned long*)SRAM_BASE;
unsigned long * __dmac_buffer = (unsigned long*)(SRAM_BASE+IDMAC_DESC_MAX_SIZE*4);
#endif

//#define DEBUG_RW_REGS /*sunny add for debug register*/

static inline void dwmci_writel(struct dwmci_host* host, int reg, u32 val)
{
#ifdef DEBUG_RW_REGS
	info("Write32(%04x, %08x)\n", reg, val);
#endif
	__raw_writel(val, (u32)host->regs + reg);
}

static inline u32 dwmci_readl(struct dwmci_host* host, int reg)
{
#ifdef DEBUG_RW_REGS
	u32 data = *(volatile u32*)((u32)host->regs + reg);
	info("Read32(%04x) = %08x\n", reg, data);
	return data;
#else
	return __raw_readl((u32)host->regs + reg);
#endif
}

static int dwmci_wait_reset(struct dwmci_host* host, u32 value)
{
	unsigned long timeout = 1000;
	u32 ctrl;

	dwmci_writel(host, DWMCI_CTRL, value);

	while (timeout--) {
		ctrl = dwmci_readl(host, DWMCI_CTRL);
		if (!(ctrl & DWMCI_RESET_ALL)) {
			return 1;
		}
	}
	return 0;
}

static void dwmci_set_idma_desc(struct dwmci_idmac* idmac, u32 desc0, u32 desc1,
	u32 desc2)
{
	struct dwmci_idmac* desc = idmac;

	desc->flags = desc0;
	desc->cnt = desc1;
	desc->addr = dma_virt_to_phy(desc2);
	desc->next_addr = dma_virt_to_phy((ulong)desc + sizeof(struct dwmci_idmac));
}

static void dw_mci_idmac_reset(struct dwmci_host *host)
{
	u32 bmod = dwmci_readl(host, DWMCI_BMOD);
	/* Software reset of DMA */
	bmod |= DWMCI_BMOD_IDMAC_RESET;
	dwmci_writel(host, DWMCI_BMOD, bmod);
}

static void dwmci_idmac_stop_dma(struct dwmci_host *host)
{
	u32 temp;

	/* Disable and reset the IDMAC interface */
	temp = dwmci_readl(host, DWMCI_CTRL);
	temp &= ~DWMCI_IDMAC_EN;
	temp |= DWMCI_CTRL_DMA_RESET;
	dwmci_writel(host, DWMCI_CTRL, temp);

	/* Stop the IDMAC running */
	temp = dwmci_readl(host, DWMCI_BMOD);
	temp &= ~(DWMCI_BMOD_IDMAC_EN | DWMCI_BMOD_IDMAC_FB);
	temp |= DWMCI_BMOD_IDMAC_RESET;
	dwmci_writel(host, DWMCI_BMOD, temp);
}


static bool dwmci_reset(struct dwmci_host *host)
{
	u32 flags = DWMCI_CTRL_RESET | DWMCI_CTRL_FIFO_RESET;
	u32 timeout_ms = 500;
	bool ret = false;
	u32 status;
	int timeout;

	if (!host->fifo_mode)
		flags |= DWMCI_CTRL_DMA_RESET;

	if (dwmci_wait_reset(host, flags)) {
		/*
		 * In all cases we clear the RAWINTS register to clear any
		 * interrupts.
		 */
		dwmci_writel(host, DWMCI_RINTSTS, 0xFFFFFFFF);

		/* if using dma we wait for dma_req to clear */
		if (!host->fifo_mode) {
			unsigned long long start = get_timer(0);

			do {
				status = dwmci_readl(host, DWMCI_STATUS);
				if (!(status & DWMCI_STATUS_DMA_REQ))
					break;
			} while (get_timer(start) < timeout_ms);

			if (status & DWMCI_STATUS_DMA_REQ) {
				info("%s: Timeout waiting for dma_req to clear during reset\n",
					__func__);
				goto ciu_out;
			}

			/* when using DMA next we reset the fifo again */
			if (!dwmci_wait_reset(host, DWMCI_CTRL_FIFO_RESET))
				goto ciu_out;
		}
	} else {
		/* if the controller reset bit did clear, then set clock regs */
		if (!(dwmci_readl(host, DWMCI_CTRL) & DWMCI_CTRL_RESET)) {
			info("%s: fifo/dma reset bits didn't clear but ciu was reset, doing clock update\n",
				__func__);
			goto ciu_out;
		}
	}

	if (!host->fifo_mode)
		/* It is also recommended that we reset and reprogram idmac */
		dw_mci_idmac_reset(host);

	ret = true;

ciu_out:
	/* After a CTRL reset we need to have CIU set clock registers  */
	dwmci_writel(host,
		DWMCI_CLKENA,
		DWMCI_CLKEN_ENABLE);

	dwmci_writel(host,
		DWMCI_CMD,
		DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	timeout = 10000;
	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout-- < 0) {
			debug("%s: Timeout!\n", __func__);
			return -ETIMEDOUT;
		}
	} while (status & DWMCI_CMD_START);

	return ret;
}

static void dwmci_adjust_fifoth(struct dwmci_host *host, struct dw_mmc_data *data)
{
	unsigned int blksz = data->blocksize;
	int fifo_depth = 512;
	const u32 mszs[] = {1, 4, 8, 16, 32, 64, 128, 256};
	u32 fifo_width = 4;
	u32 blksz_depth = blksz / fifo_width, fifoth_val;
	u32 msize = 0, rx_wmark = 1, tx_wmark, tx_wmark_invers;
	int idx = ARRAY_SIZE(mszs) - 1;

	/* pio should ship this scenario */
	if (host->fifo_mode)
		return;

	tx_wmark = (fifo_depth) / 2;
	tx_wmark_invers = fifo_depth - tx_wmark;

	/*
	 * MSIZE is '1',
	 * if blksz is not a multiple of the FIFO width
	 */
	if (blksz % fifo_width) {
		msize = 0;
		rx_wmark = 1;
		goto done;
	}

	do {
		if (!((blksz_depth % mszs[idx]) ||
		     (tx_wmark_invers % mszs[idx]))) {
			msize = idx;
			rx_wmark = mszs[idx] - 1;
			break;
		}
	} while (--idx > 0);
	/*
	 * If idx is '0', it won't be tried
	 * Thus, initial values are uesed
	 */
done:
	fifoth_val = SDMMC_SET_FIFOTH(msize, rx_wmark, tx_wmark);
	dwmci_writel(host, DWMCI_FIFOTH, fifoth_val);
}

static void dwmci_prepare_data(struct dwmci_host* host,
	struct dw_mmc_data* data, struct dwmci_idmac* cur_idmac,
	void* bounce_buffer)
{
	unsigned long ctrl;
	unsigned int i = 0, flags, cnt, blk_cnt;

	blk_cnt = data->blocks;

	/*
	 * Decide the MSIZE and RX/TX Watermark.
	 * If current block size is same with previous size,
	 * no need to update fifoth.
	 */
	if (host->prev_blksz != data->blocksize) {
		dwmci_adjust_fifoth(host, data);
	}

	/* Make sure to reset DMA in case we did PIO before this */
	dwmci_wait_reset(host, (DWMCI_CTRL_DMA_RESET | DWMCI_CTRL_FIFO_RESET));
	dw_mci_idmac_reset(host);

	dwmci_writel(host, DWMCI_DBADDR, (ulong)dma_virt_to_phy(cur_idmac));

	do {
		flags = DWMCI_IDMAC_OWN | DWMCI_IDMAC_CH ;
		flags |= (i == 0) ? DWMCI_IDMAC_FS : 0;
		if (blk_cnt <= 8) {
			flags |= DWMCI_IDMAC_LD;
			cnt = data->blocksize * blk_cnt;
		} else {
			cnt = data->blocksize * 8;
		}

		dwmci_set_idma_desc(cur_idmac,
			flags,
			cnt,
			(ulong)bounce_buffer + (i * PAGE_SIZE));

		if (blk_cnt <= 8) {
			break;
		}
		blk_cnt -= 8;
		cur_idmac++;
		i++;
	} while (1);

	dwmci_writel(host, DWMCI_BLKSIZ, data->blocksize);
	dwmci_writel(host, DWMCI_BYTCNT, data->blocksize * data->blocks);

	/* Select IDMAC interface */
	ctrl = dwmci_readl(host, DWMCI_CTRL);
	ctrl |= DWMCI_IDMAC_EN | DWMCI_DMA_EN;
	dwmci_writel(host, DWMCI_CTRL, ctrl);

	/* drain writebuffer */
	//mb();

	/* Enable the IDMAC */
	ctrl = dwmci_readl(host, DWMCI_BMOD);
	ctrl |= DWMCI_BMOD_IDMAC_FB | DWMCI_BMOD_IDMAC_EN;
	dwmci_writel(host, DWMCI_BMOD, ctrl);

	/* Start it running */
	dwmci_writel(host, DWMCI_PLDMND, 1);
}

static int dwmci_data_transfer(struct dwmci_host* host,
	struct dw_mmc_data* data)
{
	int ret = 0;
	u32 timeout = 240000;
	u32 mask,size,i,len = 0;
	u32* buf = NULL;
	unsigned long long start = get_timer(0);
	u32 fifo_depth = 512;

	size = data->blocksize * data->blocks / 4;
	if (data->flags == MMC_DATA_READ) {
		buf = (u32 *)data->addr.dest;
	} else {
		buf = (u32 *)data->addr.src;
	}

	for (; ;) {
		mask = dwmci_readl(host, DWMCI_RINTSTS);
		/* Error during data transfer. */
		if (mask & (DWMCI_DATA_ERR | DWMCI_DATA_TOUT)) {
			debug("%s: DATA ERROR! RINTSTS=%x, STATUS:0x%08x\n", __func__,mask, dwmci_readl(host, DWMCI_STATUS));
			/*
			 * After an error, there may be data lingering
			 * in the FIFO
			 */
			if (!host->fifo_mode)
				dwmci_idmac_stop_dma(host);
			dwmci_reset(host);
			ret = -EINVAL;
			break;
		}

		if (host->fifo_mode && size) {
			if (data->flags == MMC_DATA_READ) {
				if ((dwmci_readl(host,
					DWMCI_RINTSTS) & DWMCI_INTMSK_RXDR)) {
					len = dwmci_readl(host, DWMCI_STATUS);
					len = (len >> DWMCI_FIFO_SHIFT) &
						DWMCI_FIFO_MASK;
					for (i = 0; i < len; i++) {
						*buf++ = dwmci_readl(host,
							DWMCI_DATA);
					}
					dwmci_writel(host,
						DWMCI_RINTSTS,
						DWMCI_INTMSK_RXDR);
					size = size > len ? (size - len) : 0;
				}
			} else {
				if ((dwmci_readl(host,
			  		DWMCI_RINTSTS) & DWMCI_INTMSK_TXDR)) {
					len = dwmci_readl(host, DWMCI_STATUS);
					len = fifo_depth -
			  			((len >>DWMCI_FIFO_SHIFT) &
			  			DWMCI_FIFO_MASK);
					len = min(len,size);
					for (i = 0; i < len; i++) {
			  			dwmci_writel(host,
			  				DWMCI_DATA,
			  				*buf++);
					}
			  		dwmci_writel(host,
			  			DWMCI_RINTSTS,
			  			DWMCI_INTMSK_TXDR);
					size = size > len ? (size - len) : 0;
				}
			}
		}

		/* Data arrived correctly. */
		if (mask & DWMCI_INTMSK_DTO) {
			if (host->fifo_mode && size && data->flags == MMC_DATA_READ) {
				len = dwmci_readl(host, DWMCI_STATUS);
				len = (len >> DWMCI_FIFO_SHIFT) &
					DWMCI_FIFO_MASK;
				for (i = 0; i < len; i++) {
					*buf++ = dwmci_readl(host,
						DWMCI_DATA);
				}
				size = size > len ? (size - len) : 0;
			}
			ret = 0;
			break;
		}

		/* Check for timeout. */
		if (get_timer(start) > timeout) {
			debug("%s: Timeout waiting for data!\n", __func__);
			if (!host->fifo_mode)
				dwmci_idmac_stop_dma(host);
			dwmci_reset(host);
			ret = TIMEOUT;
			break;
		}
	}

	dwmci_writel(host, DWMCI_RINTSTS, mask);

	return ret;
}

static int dwmci_set_transfer_mode(struct dwmci_host* host,
	struct dw_mmc_data* data)
{
	unsigned long mode;

	mode = DWMCI_CMD_DATA_EXP;
	if (data->flags & MMC_DATA_WRITE) {
		mode |= DWMCI_CMD_RW;
	}

	return mode;
}

static int dwmci_wait_while_busy(struct dwmci_host *host, u32 cmd_flags)
{
	unsigned long timeout = 500;
	unsigned long long start = get_timer(0);
	int ret = 0;

	/*
	 * Databook says that before issuing a new data transfer command
	 * we need to check to see if the card is busy.  Data transfer commands
	 * all have SDMMC_CMD_PRV_DAT_WAIT set, so we'll key off that.
	 */
	if (cmd_flags & DWMCI_CMD_PRV_DAT_WAIT) {
		while (dwmci_readl(host, DWMCI_STATUS) & DWMCI_BUSY) {
			if (get_timer(start) > timeout) {
				ret = TIMEOUT;
				break;
			}
			udelay(10);
		}
	}
	return ret;
}

static int dwmci_send_cmd(struct dwmci_host* host, struct dw_mmc_cmd* cmd,
	struct dw_mmc_data* data)
{
	struct dwmci_idmac* cur_idmac = (struct dwmci_idmac*)__cur_idmac;
	void* bounce_buffer = NULL;
	int ret = 0,flags = 0,i;
	u32 status, retry = 10000;
	u32 mask,ctrl;
	int timeout, busy_retries = 3;

	dwmci_writel(host, DWMCI_RINTSTS, DWMCI_INTMSK_ALL);

	if (data) {
		if (host->fifo_mode) {
			dwmci_writel(host, DWMCI_BLKSIZ, data->blocksize);
			dwmci_writel(host,
				DWMCI_BYTCNT,
				data->blocksize * data->blocks);
			if (data->flags & MMC_DATA_READ)
				dwmci_writel(host, DWMCI_CDTHRCTL, (data->blocksize << 16 | 1 << 0));
			dwmci_wait_reset(host, DWMCI_CTRL_FIFO_RESET);
			host->prev_blksz = 0;
		} else {
			if (data->flags == MMC_DATA_READ) {
				dwmci_writel(host, DWMCI_CDTHRCTL, (data->blocksize << 16 | 1 << 0));
				#ifndef FORCE_USE_SRAM_DMA_BUFFER
		  		/* not aligned to word,using __dmac_buffer as DMA use */
				if ((u32)data->addr.dest & 0x3) {
					bounce_buffer = __dmac_buffer;
				} else {
					bounce_buffer = data->addr.dest;
				}
				#else
					bounce_buffer = __dmac_buffer;
				#endif
			} else {
				#ifndef FORCE_USE_SRAM_DMA_BUFFER
		  	  	/* not aligned to word,using __dmac_buffer as DMA use */
				if ((u32)data->addr.src & 0x3) {
					bounce_buffer = __dmac_buffer;
		  	  		memcpy(bounce_buffer,
		  	  			data->addr.src,
		  	  			data->blocksize * data->blocks);
				} else {
					bounce_buffer = data->addr.dest;
				}
				#else
					bounce_buffer = __dmac_buffer;
		  	  		memcpy(bounce_buffer,
		  	  			data->addr.src,
		  	  			data->blocksize * data->blocks);
				#endif
			}
		  	dwmci_prepare_data(host,
		  		data,
		  		cur_idmac,
		  		bounce_buffer);
			/*
			 * Keep the current block size.
			 * It will be used to decide whether to update
			 * fifoth register next time.
			 */
			host->prev_blksz = data->blocksize;
		}
	}

	dwmci_writel(host, DWMCI_CMDARG, cmd->cmdarg);

	if (data) {
		flags = dwmci_set_transfer_mode(host, data);
	}

	/* this is the first command, send the initialization clock (80 clocks) */
	if (host->card_need_init) {
		host->card_need_init = 0;
		flags |= DWMCI_CMD_INIT;
	}

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY)) {
		return -1;
	}

	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION) {
		flags |= DWMCI_CMD_ABORT_STOP;
	} else {
		flags |= DWMCI_CMD_PRV_DAT_WAIT;
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		flags |= DWMCI_CMD_RESP_EXP;
		if (cmd->resp_type & MMC_RSP_136) {
			flags |= DWMCI_CMD_RESP_LENGTH;
		}
	}

	if (cmd->resp_type & MMC_RSP_CRC) {
		flags |= DWMCI_CMD_CHECK_CRC;
	}

	flags |= (cmd->cmdidx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG);

busy_retry:
	if (dwmci_wait_while_busy(host, flags)) {
		debug("%s: Timeout on data busy. STATUS:0x%08x cmd_idx=%d\n", __func__, dwmci_readl(host, DWMCI_STATUS), cmd->cmdidx);
		if (busy_retries--) {
			/* CIU set clock registers, disable low power */
			dwmci_writel(host,
				DWMCI_CLKENA,
				DWMCI_CLKEN_ENABLE);

			dwmci_writel(host,
				DWMCI_CMD,
				DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

			timeout = 10000;
			do {
				status = dwmci_readl(host, DWMCI_CMD);
				if (timeout-- < 0) {
					debug("%s: Timeout!\n", __func__);
					return TIMEOUT;
				}
			} while (status & DWMCI_CMD_START);
			goto busy_retry;
		}
		return TIMEOUT;
	}

	dwmci_writel(host, DWMCI_CMD, flags);

	for (i = 0; i < retry; i++) {
		mask = dwmci_readl(host, DWMCI_RINTSTS);
		if (mask & DWMCI_INTMSK_CDONE) {
			if (!data) {
				dwmci_writel(host, DWMCI_RINTSTS, mask);
			}
			break;
		}
	}

	if (i == retry) {
		debug("%s: Timeout.\n", __func__);
		dwmci_reset(host);
		return TIMEOUT;
	}

	if (mask & DWMCI_INTMSK_RTO) {
		/*
		 * Timeout here is not necessarily fatal. (e)MMC cards
		 * will splat here when they receive CMD55 as they do
		 * not support this command and that is exactly the way
		 * to tell them apart from SD cards. Thus, this output
		 * below shall be debug(). eMMC cards also do not favor
		 * CMD8, please keep that in mind.
		 */
		debug("%s: Response Timeout. cmd_idx=%d\n", __func__,cmd->cmdidx);
		return TIMEOUT;
	} else if (mask & DWMCI_INTMSK_RE) {
		debug("%s: Response Error. cmd_idx=%d\n", __func__,cmd->cmdidx);
		return -EIO;
	}


	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			cmd->response[0] = dwmci_readl(host, DWMCI_RESP3);
			cmd->response[1] = dwmci_readl(host, DWMCI_RESP2);
			cmd->response[2] = dwmci_readl(host, DWMCI_RESP1);
			cmd->response[3] = dwmci_readl(host, DWMCI_RESP0);
		} else {
			cmd->response[0] = dwmci_readl(host, DWMCI_RESP0);
		}
	}

	if (data) {
		ret = dwmci_data_transfer(host, data);
		if (ret) {
			info("cmd_idx=%d\n",cmd->cmdidx);
		}
		/* only dma mode need it */
		if (!host->fifo_mode) {
			ctrl = dwmci_readl(host, DWMCI_CTRL);
			ctrl &= ~(DWMCI_DMA_EN);
			dwmci_writel(host, DWMCI_CTRL, ctrl);

			if (data->flags == MMC_DATA_READ) {
				#ifndef FORCE_USE_SRAM_DMA_BUFFER
				/* not aligned to word,using __dmac_buffer as DMA use */
				if ((u32)data->addr.dest & 0x3) {
					memcpy(data->addr.dest,
						bounce_buffer,
						data->blocksize * data->blocks);
				}
				#else
					memcpy(data->addr.dest,
						bounce_buffer,
						data->blocksize * data->blocks);
				#endif
			}
		}
	}

	udelay(100);

	return ret;
}

/*set up clock */
 int dwmci_setup_bus(struct dwmci_host* host, u32 freq)
{
	u32 div,status;
	int timeout = 10000;
	unsigned long sclk;

	if ( (freq == 0) || (freq > host->pclk)) {
		return 0;
	}

	sclk = host->pclk;

	if (sclk == freq) {
		div = 0;
	} /* bypass mode */
	else {
		div = DIV_ROUND_UP(sclk, 2 * freq);
	}

	dwmci_writel(host, DWMCI_CLKENA, 0);
	dwmci_writel(host, DWMCI_CLKSRC, 0);

	dwmci_writel(host, DWMCI_CLKDIV, div);
	dwmci_writel(host,
		DWMCI_CMD,
		DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout-- < 0) {
			debug("%s: Timeout!\n", __func__);
			return -ETIMEDOUT;
		}
	} while (status & DWMCI_CMD_START);

	dwmci_writel(host,
		DWMCI_CLKENA,
		DWMCI_CLKEN_ENABLE | DWMCI_CLKEN_LOW_PWR);

	dwmci_writel(host,
		DWMCI_CMD,
		DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

	timeout = 10000;
	do {
		status = dwmci_readl(host, DWMCI_CMD);
		if (timeout-- < 0) {
			debug("%s: Timeout!\n", __func__);
			return -ETIMEDOUT;
		}
	} while (status & DWMCI_CMD_START);

	host->clock = freq;
	if (div == 0) {
		host->actual_clock = sclk;
	} else {
		host->actual_clock = sclk / (2 * div);
	}
	return 0;
}

/* set clock and buswidth and ddr mode*/
static void dwmci_set_ios(struct dwmci_host* host)
{
	u32 ctype,regs;

	dwmci_setup_bus(host, host->clock);
	switch (host->bus_width) {
	case 8:
		ctype = DWMCI_CTYPE_8BIT;
		break;
	case 4:
		ctype = DWMCI_CTYPE_4BIT;
		break;
	default:
		ctype = DWMCI_CTYPE_1BIT;
		break;
	}

	dwmci_writel(host, DWMCI_CTYPE, ctype);

	regs = dwmci_readl(host, DWMCI_UHS_REG);
	if (host->ddr_mode) {
		regs |= DWMCI_DDR_MODE;
	} else {
		regs &= ~DWMCI_DDR_MODE;
	}

	dwmci_writel(host, DWMCI_UHS_REG, regs);
}

static int dwmci_init_hw(struct dwmci_host* host)
{
	uint32_t fifo_size;

	dwmci_writel(host, DWMCI_PWREN, 1);

	host->card_need_init = 1;

	if (!dwmci_wait_reset(host, DWMCI_RESET_ALL)) {
		debug("%s[%d] Fail-reset!!\n", __func__, __LINE__);
		return -EIO;
	}

	/* Enumerate at 400KHz */
	dwmci_setup_bus(host, host->cfg.f_min);

	/*disable  and clear pending interupt*/
	dwmci_writel(host, DWMCI_RINTSTS, 0xFFFFFFFF);
	dwmci_writel(host, DWMCI_INTMASK, 0);
	/*disable internal DMAC interrupt*/
	dwmci_writel(host, DWMCI_IDINTEN, 0);

	/* set timeout counter*/
	dwmci_writel(host, DWMCI_TMOUT, 0xFFFFFFFF);

	/*do software reset*/
	dwmci_writel(host, DWMCI_BMOD, 1);

	/*calculate and set FIFO*/
	fifo_size = 512;
	if (!host->fifoth_val) {

		host->fifoth_val = MSIZE(0x2) | RX_WMARK(fifo_size / 2 - 1) |
			TX_WMARK(fifo_size / 2);
	}
	dwmci_writel(host, DWMCI_FIFOTH, host->fifoth_val);

	/*disable clock, set clk div 0*/
	dwmci_writel(host, DWMCI_CLKENA, 0);
	dwmci_writel(host, DWMCI_CLKSRC, 0);

	return 0;
}

static const struct dw_mmc_ops dwmci_ops = {
	.send_cmd = dwmci_send_cmd,
	.set_ios = dwmci_set_ios,
	.init = dwmci_init_hw,
};

int dw_mmc_init(struct dwmci_host* host, unsigned long base, unsigned long pclk,
	int bus_width,int speed)
{
	host->regs = (void*)base;
	host->pclk = pclk;
	host->bus_width = bus_width;
	host->clock = speed;

	host->cfg.ops = &dwmci_ops;
	host->cfg.f_min = DWMMC_MIN_FREQ;
	host->cfg.f_max = DWMMC_MAX_FREQ;
	host->cfg.voltages = MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_165_195;
	host->cfg.host_caps |= MMC_MODE_8BIT | MMC_MODE_4BIT ;
	host->cfg.host_caps |= MMC_MODE_HS | MMC_MODE_HS_52MHz;
	host->cfg.b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	return 0;
}

unsigned long dwmci_get_raw_int_status(struct dwmci_host* host)
{
	return dwmci_readl(host, DWMCI_RINTSTS);
}
unsigned long dwmci_get_int_status(struct dwmci_host* host)
{
	return dwmci_readl(host, DWMCI_MINTSTS);
}
void dwmci_clear_int_status(struct dwmci_host* host, unsigned long status)
{
	dwmci_writel(host, DWMCI_RINTSTS, status);
}
void dwmci_set_int_mask(struct dwmci_host* host, unsigned long mask)
{
	dwmci_writel(host, DWMCI_INTMASK, mask);
}
void dwmci_enable_int(struct dwmci_host* host,int en)
{
	u32 ctrl;

	ctrl = dwmci_readl(host, DWMCI_CTRL);
	ctrl &= ~(DWMCI_CTRL_INT_EN);
	if (en)
		ctrl |= DWMCI_CTRL_INT_EN;
	dwmci_writel(host, DWMCI_CTRL, ctrl);
}

void dwmci_display_config(struct dwmci_host* host)
{
	u32 src;
	u32 div;
	u32 freq;
	int bus_width;

	src = dwmci_readl(host, DWMCI_CLKSRC);
	div = dwmci_readl(host, DWMCI_CLKDIV);
	bus_width = dwmci_readl(host, DWMCI_CTYPE);

	freq = (div >> (8*src))&0xff;
	freq = 2*freq;
	if (freq != 0)
		freq = host->pclk/freq;
	else
		freq = host->pclk;

	if ((bus_width >>16) & 1)
		bus_width = 8;
	else if (bus_width & 1)
		bus_width = 4;
	else
		bus_width = 1;
	info("mmc using clk src %d: div=0x%x actual freq=%d\n",
		src,div,freq);
	info("mmc using bus width: %d bit\n",bus_width);
}
