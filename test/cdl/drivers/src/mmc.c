/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "drivers_config.h"
#include "cdl_mmc.h"
#include "mmc.h"

/*
#define CONFIG_MMC_TRACE  0
*/

static struct mmc mmc_memory_dev;
static struct mmc* p_mmc = &mmc_memory_dev;

/* frequency bases */
/* divided by 10 to be nice to platforms without floating point */
static const int fbase[] = {
	10000,100000,1000000,10000000,
};

/* Multiplier values for TRAN_SPEED.  Multiplied by 10 to be nice
 * to platforms without floating point.
 */
static const int multipliers[] = {
	0,	/* reserved */
	10,12,13,15,20,25,30,35,40,45,50,55,60,70,80,
};

/*wrapper handler to call host real functions*/
static int wrapper_send_cmd(struct mmc* mmc, struct mmc_cmd* cmd,
	struct mmc_data* data)
{
	struct dwmci_host* host = (struct dwmci_host*)mmc->priv;

	return (host->cfg.ops->send_cmd(host, (struct dw_mmc_cmd*)cmd,
		(struct dw_mmc_data*)data));
}
static void wrapper_set_ios(struct mmc* mmc)
{
	struct dwmci_host* host = (struct dwmci_host*)mmc->priv;

	host->bus_width = mmc->bus_width;
	host->clock = mmc->clock;
	host->ddr_mode = mmc->ddr_mode;
	if (host->cfg.ops->set_ios)
		host->cfg.ops->set_ios(host);
}

static int wrapper_init_hw(struct mmc* mmc)
{
	struct dwmci_host* host = (struct dwmci_host*)mmc->priv;

	if (host->cfg.ops->init)
		return host->cfg.ops->init(host);
	return 0;
}
static int wrapper_getcd(struct mmc* mmc)
{
	struct dwmci_host* host = (struct dwmci_host*)mmc->priv;

	if (host->cfg.ops->getcd)
		return host->cfg.ops->getcd(host);
	return 1;
}
static int wrapper_getwp(struct mmc* mmc)
{
	struct dwmci_host* host = (struct dwmci_host*)mmc->priv;

	if (host->cfg.ops->getwp)
		return host->cfg.ops->getwp(host);
	return 0;
}

static const struct mmc_ops drivers_mmc_ops = {
	.send_cmd = wrapper_send_cmd,
	.set_ios = wrapper_set_ios,
	.init = wrapper_init_hw,
	.getcd = wrapper_getcd,
	.getwp = wrapper_getwp,
};
static struct mmc_config drivers_config;

static int mmc_getwp(struct mmc* mmc)
{
	int wp;

	wp = -1;

	if (wp < 0) {
		if (mmc->cfg->ops->getwp)
			wp = mmc->cfg->ops->getwp(mmc);
		else
			wp = 0;
	}

	return wp;
}
static int mmc_getcd(struct mmc* mmc)
{
	int cd;

	cd = -1;

	if (cd < 0) {
		if (mmc->cfg->ops->getcd)
			cd = mmc->cfg->ops->getcd(mmc);
		else
			cd = 1;
	}

	return cd;
}

static int mmc_send_cmd(struct mmc* mmc, struct mmc_cmd* cmd,
	struct mmc_data* data)
{
	int ret;

#ifdef CONFIG_MMC_TRACE
	int i;
	u8* ptr;

	info("CMD_SEND:%d\n", cmd->cmdidx);
	info("\t\tARG\t\t\t 0x%08X\n", cmd->cmdarg);
	ret = mmc->cfg->ops->send_cmd(mmc, cmd, data);
	switch (cmd->resp_type) {
	case MMC_RSP_NONE:
		info("\t\tMMC_RSP_NONE\n");
		break;
	case MMC_RSP_R1:
		info("\t\tMMC_RSP_R1,5,6,7 \t 0x%08X \n", cmd->response[0]);
		break;
	case MMC_RSP_R1b:
		info("\t\tMMC_RSP_R1b\t\t 0x%08X \n", cmd->response[0]);
		break;
	case MMC_RSP_R2:
		info("\t\tMMC_RSP_R2\t\t 0x%08X \n", cmd->response[0]);
		info("\t\t          \t\t 0x%08X \n", cmd->response[1]);
		info("\t\t          \t\t 0x%08X \n", cmd->response[2]);
		info("\t\t          \t\t 0x%08X \n", cmd->response[3]);
		info("\n");
		info("\t\t\t\t\tDUMPING DATA\n");
		for (i = 0; i < 4; i++) {
			int j;
			info("\t\t\t\t\t%03d - ", i * 4);
			ptr = (u8 *)&cmd->response[i];
			ptr += 3;
			for (j = 0; j < 4; j++)
				info("%02X ", *ptr--);
			info("\n");
		}
		break;
	case MMC_RSP_R3:
		info("\t\tMMC_RSP_R3,4\t\t 0x%08X \n", cmd->response[0]);
		break;
	default:
		info("\t\tERROR MMC rsp not supported\n");
		break;
	}
#else
	ret = mmc->cfg->ops->send_cmd(mmc, cmd, data);
#endif
	return ret;
}

static int mmc_send_status(struct mmc* mmc, int timeout)
{
	struct mmc_cmd cmd;
	int err, retries = 5;
#ifdef CONFIG_MMC_TRACE
	int status;
#endif

	cmd.cmdidx = MMC_CMD_SEND_STATUS;
	cmd.resp_type = MMC_RSP_R1;
	if (!mmc_host_is_spi(mmc))
		cmd.cmdarg = mmc->rca << 16;

	while (1) {
		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (!err) {
			if ((cmd.response[0] & MMC_STATUS_RDY_FOR_DATA) &&
				(
				cmd.response[0] &
				MMC_STATUS_CURR_STATE) != MMC_STATE_PRG)
				break;
			else if (cmd.response[0] & MMC_STATUS_MASK) {
				info("Status Error: 0x%08X\n", cmd.response[0]);
				return COMM_ERR;
			}
		} else if (--retries < 0)
			return err;

		if (timeout-- <= 0)
			break;

		udelay(1000);
	}

#ifdef CONFIG_MMC_TRACE
	status = (cmd.response[0] & MMC_STATUS_CURR_STATE) >> 9;
	info("CURR STATE:%d\n", status);
#endif
	if (timeout <= 0) {
		info("Timeout waiting card ready\n");
		return TIMEOUT;
	}
	if (cmd.response[0] & MMC_STATUS_SWITCH_ERROR)
		return SWITCH_ERR;

	return 0;
}

static int mmc_go_idle(struct mmc* mmc)
{
	struct mmc_cmd cmd;
	int err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	udelay(2000);

	return 0;
}

static int mmc_send_if_cond(struct mmc* mmc)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	/* We set the bit if the host supports voltages between 2.7 and 3.6 V */
	cmd.cmdarg = ((mmc->cfg->voltages & 0xff8000) != 0) << 8 | 0xaa;
	cmd.resp_type = MMC_RSP_R7;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return UNUSABLE_ERR;
	else
		mmc->version = SD_VERSION_2;

	return 0;
}

static int mmc_send_op_cond_iter(struct mmc* mmc, int use_arg)
{
	struct mmc_cmd cmd;
	int err;

	cmd.cmdidx = MMC_CMD_SEND_OP_COND;
	cmd.resp_type = MMC_RSP_R3;
	cmd.cmdarg = 0;
	if (use_arg && !mmc_host_is_spi(mmc))
		cmd.cmdarg = OCR_HCS |
			(mmc->cfg->voltages & (mmc->ocr &
			OCR_VOLTAGE_MASK)) | (mmc->ocr & OCR_ACCESS_MODE);

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		return err;
	mmc->ocr = cmd.response[0];
	return 0;
}

static int mmc_send_op_cond(struct mmc* mmc)
{
	int err,i;

	/* Some cards seem to need this */
	mmc_go_idle(mmc);

	/* Asking to the card its capabilities */
	for (i = 0; i < 2; i++) {
		err = mmc_send_op_cond_iter(mmc, i != 0);
		if (err)
			return err;

		/* exit if not busy (flag seems to be inverted) */
		if (mmc->ocr & OCR_BUSY)
			break;
	}
	mmc->op_cond_pending = 1;
	return 0;
}

static int mmc_complete_op_cond(struct mmc* mmc)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	unsigned long long start;
	int err;

	mmc->op_cond_pending = 0;
	if (!(mmc->ocr & OCR_BUSY)) {
		start = get_timer(0);
		while (1) {
			err = mmc_send_op_cond_iter(mmc, 1);
			if (err)
				return err;
			if (mmc->ocr & OCR_BUSY)
				break;
			if (get_timer(start) > timeout)
				return UNUSABLE_ERR;
			udelay(100);
		}
	}

	if (mmc_host_is_spi(mmc)) {
		/* read OCR for spi */
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		mmc->ocr = cmd.response[0];
	}

	mmc->version = MMC_VERSION_UNKNOWN;

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 1;

	return 0;
}


static int mmc_send_ext_csd(struct mmc* mmc, u8* ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	data.addr.dest = (char*)ext_csd;
	data.blocks = 1;
	data.blocksize = MMC_MAX_BLOCK_LEN;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	return err;
}

static int mmc_switch(struct mmc* mmc, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;
	int timeout = 1000;
	int ret;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (index << 16) |
		(value << 8);

	ret = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	if (!ret)
		ret = mmc_send_status(mmc, timeout);

	return ret;
}

static int mmc_change_freq(struct mmc* mmc)
{
	u8 ext_csd[MMC_MAX_BLOCK_LEN];
	char cardtype;
	int err;

	mmc->card_caps = 0;

	if (mmc_host_is_spi(mmc))
		return 0;

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;

	mmc->card_caps |= MMC_MODE_4BIT | MMC_MODE_8BIT;

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	cardtype = ext_csd[EXT_CSD_CARD_TYPE] & 0xf;
#if 0
	/*if user request 50M,high speed, try to switch*/
	if (mmc->user_speed >= mmc->sys_max_tran_speed) {
		err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

		if (err)
			return err == SWITCH_ERR ? 0 : err;
	}
#else
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 1);

	if (err)
		return err == SWITCH_ERR ? 0 : err;
#endif
	/* Now check to see that it worked */
	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	/* No high-speed support */
	if (!ext_csd[EXT_CSD_HS_TIMING])
		return 0;

	/* High Speed is set, there are two types: 52MHz and 26MHz */
	if (cardtype & EXT_CSD_CARD_TYPE_52) {
		if (cardtype & EXT_CSD_CARD_TYPE_DDR_1_8V)
			mmc->card_caps |= MMC_MODE_DDR_52MHz;
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS;
	} else {
		mmc->card_caps |= MMC_MODE_HS;
	}

	return 0;
}

static int mmc_set_capacity(struct mmc* mmc, int part_num)
{
	switch (part_num) {
	case 0:
		mmc->capacity = mmc->capacity_user;
		break;
	case 1:
	case 2:
		mmc->capacity = mmc->capacity_boot;
		break;
	case 3:
		mmc->capacity = mmc->capacity_rpmb;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		mmc->capacity = mmc->capacity_gp[part_num - 4];
		break;
	default:
		return -1;
	}

	mmc->block_dev.lba = cb_lldiv(mmc->capacity, mmc->read_bl_len);

	return 0;
}

static int sd_send_op_cond(struct mmc* mmc)
{
	int timeout = 1000;
	int err;
	struct mmc_cmd cmd;

	while (1) {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;

		/*
		 * Most cards do not answer if some reserved bits
		 * in the ocr are set. However, Some controller
		 * can set bit 7 (reserved for low voltages), but
		 * how to manage low voltages SD card is not yet
		 * specified.
		 */
		cmd.cmdarg = mmc_host_is_spi(mmc) ? 0 :
			(mmc->cfg->voltages & 0xff8000);

		if (mmc->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		if (cmd.response[0] & OCR_BUSY)
			break;

		if (timeout-- <= 0)
			return UNUSABLE_ERR;

		udelay(1000);
	}

	if (mmc->version != SD_VERSION_2)
		mmc->version = SD_VERSION_1_0;

	if (mmc_host_is_spi(mmc)) {
		/* read OCR for spi */
		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}

	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

static int sd_switch(struct mmc* mmc, int mode, int group, u8 value, u8* resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);

	data.addr.dest = (char*)resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}

static int sd_change_freq(struct mmc* mmc)
{
	int err;
	struct mmc_cmd cmd;
	uint scr[2];
	uint switch_status[16];
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	if (mmc_host_is_spi(mmc))
		return 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;

	timeout = 3;

	retry_scr:
	data.addr.dest = (char*)scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		if (timeout--)
			goto retry_scr;

		return err;
	}

	mmc->scr[0] = __be32_to_cpu(scr[0]);
	mmc->scr[1] = __be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
	case 0:
		mmc->version = SD_VERSION_1_0;
		break;
	case 1:
		mmc->version = SD_VERSION_1_10;
		break;
	case 2:
		mmc->version = SD_VERSION_2;
		if ((mmc->scr[0] >> 15) & 0x1)
			mmc->version = SD_VERSION_3;
		break;
	default:
		mmc->version = SD_VERSION_1_0;
		break;
	}

	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc,
			SD_SWITCH_CHECK,
			0,
			1,
			(u8 *)switch_status);

		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

	/* If high-speed isn't supported, we return */
	if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED)) {
		info("card high-speed isn't really supported\n");
		return 0;
	}

	/*
	 * If the host doesn't support SD_HIGHSPEED, do not switch card to
	 * HIGHSPEED mode even if the card support SD_HIGHSPPED.
	 * This can avoid furthur problem when the card runs in different
	 * mode between the host.
	 */
	if (!((mmc->cfg->host_caps & MMC_MODE_HS_52MHz) &&
		(mmc->cfg->host_caps & MMC_MODE_HS)))
		return 0;
#if 0
	/*if user request 50M,high speed, try to switch*/
	if (mmc->user_speed >= mmc->sys_max_tran_speed) {
		err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)switch_status);

		if (err) {
			info("switch card to high speed mode fail\n");
			return err;
		}
		if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
			mmc->card_caps |= MMC_MODE_HS;
	}
#else
	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)switch_status);

	if (err) {
		info("switch card to high speed mode fail\n");
		return err;
	}
	if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000)
		mmc->card_caps |= MMC_MODE_HS;
#endif
	return 0;
}

static int mmc_set_blocklen(struct mmc* mmc, int len)
{
	struct mmc_cmd cmd;
	int err, retries = MMC_CMD_RETRIES;

	if (mmc->ddr_mode)
		return 0;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;

	while (1) {
		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (!err || (--retries < 0))
			break;
	}

	return err;
}

static int mmc_read_blocks(struct mmc* mmc, void* dst, unsigned long start,
	unsigned long blkcnt)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->read_bl_len;

	cmd.resp_type = MMC_RSP_R1;

	data.addr.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = mmc->read_bl_len;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (blkcnt > 1) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		if (mmc_send_cmd(mmc, &cmd, NULL))
			return 0;
	}

	return err ? 0 : blkcnt;
}

unsigned long mmc_bread(unsigned long start, unsigned long blkcnt, char* dst)
{
	unsigned long cur, blocks_todo = blkcnt;
	int setlen_retries, read_retries;

	if (blkcnt == 0)
		return 0;

	struct mmc* mmc = p_mmc;
	if (!mmc)
		return 0;

	if ((start + blkcnt) > mmc->block_dev.lba) {
		info("MMC: block number 0x%lx exceeds max(0x%lx)\n",
			start + blkcnt,
			mmc->block_dev.lba);
		return 0;
	}

	setlen_retries = MMC_CMD_RETRIES;
retry_setlen:
	if (mmc_set_blocklen(mmc, mmc->read_bl_len)) {
		if (setlen_retries--) {
			goto retry_setlen;
		}
		return 0;
	}

	do {
		read_retries = MMC_CMD_RETRIES;
		cur = (blocks_todo > mmc->cfg->b_max) ? mmc->cfg->b_max :
			blocks_todo;
retry_read:
		if (mmc_read_blocks(mmc, dst, start, cur) != cur) {
			if (read_retries--) {
				info("mmc_read_blocks failed retry:%d\n", read_retries);
				goto retry_read;
			}
			if (setlen_retries--) {
				goto retry_setlen;
			}
			return 0;
		}
		blocks_todo -= cur;
		start += cur;
		dst += cur * mmc->read_bl_len;
	} while (blocks_todo > 0);

	return blkcnt;
}

static ulong mmc_write_blocks(struct mmc* mmc, unsigned long start,
	unsigned long blkcnt, const void* src)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int timeout = 1000;
	int err;

	if ((start + blkcnt) > mmc->block_dev.lba) {
		info("MMC: block number 0x%lx exceeds max(0x%lx)\n",
			start + blkcnt,
			mmc->block_dev.lba);
		return 0;
	}

	if (blkcnt == 0)
		return 0;
	else if (blkcnt == 1)
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;

	if (mmc->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * mmc->write_bl_len;

	cmd.resp_type = MMC_RSP_R1;

	data.addr.src = src;
	data.blocks = blkcnt;
	data.blocksize = mmc->write_bl_len;
	data.flags = MMC_DATA_WRITE;

	err = mmc_send_cmd(mmc, &cmd, &data);

	/* SPI multiblock writes terminate using a special
	 * token, not a STOP_TRANSMISSION request.
	 */
	if (!mmc_host_is_spi(mmc) && blkcnt > 1) {
		cmd.cmdidx = MMC_CMD_STOP_TRANSMISSION;
		cmd.cmdarg = 0;
		cmd.resp_type = MMC_RSP_R1b;
		err |= mmc_send_cmd(mmc, &cmd, NULL);
	}

	/* Waiting for the ready status */
	if (mmc_send_status(mmc, timeout))
		return 0;

	return err ? 0 : blkcnt;
}

unsigned long mmc_bwrite(unsigned long start, unsigned long blkcnt, char* src)
{
	unsigned long cur, blocks_todo = blkcnt;
	struct mmc* mmc = p_mmc;
	int setlen_retries, write_retries;

	if (!mmc)
		return 0;

	if (mmc_getwp(mmc) == 1) {
		info("Error: card is write protected!\n");
		return 0;
	}

	setlen_retries = MMC_CMD_RETRIES;
retry_setlen:
	if (mmc_set_blocklen(mmc, mmc->write_bl_len)) {
		if (setlen_retries--) {
			goto retry_setlen;
		}
		return 0;
	}

	do {
		write_retries = MMC_CMD_RETRIES;
		cur = (blocks_todo > mmc->cfg->b_max) ? mmc->cfg->b_max :
			blocks_todo;
retry_write:
		if (mmc_write_blocks(mmc, start, cur, src) != cur) {
			if (write_retries--) {
				info("mmc_write_blocks failed retry:%d\n", write_retries);
				goto retry_write;
			}
			if (setlen_retries--) {
				goto retry_setlen;
			}
			return 0;
		}
		blocks_todo -= cur;
		start += cur;
		src += cur * mmc->write_bl_len;
	} while (blocks_todo > 0);

	return blkcnt;
}

static void mmc_set_ios(struct mmc* mmc)
{
	if (mmc->cfg->ops->set_ios)
		mmc->cfg->ops->set_ios(mmc);
}

void mmc_set_clock(struct mmc* mmc, uint clock)
{
	if (clock > mmc->cfg->f_max)
		clock = mmc->cfg->f_max;

	if (clock < mmc->cfg->f_min)
		clock = mmc->cfg->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

static void mmc_set_bus_width(struct mmc* mmc, uint width)
{
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}

static int mmc_startup(struct mmc* mmc)
{
	int err,i;
	uint mult,freq;
	u64 cmult,csize,capacity;
	struct mmc_cmd cmd;
	u8 ext_csd[MMC_MAX_BLOCK_LEN];
	u8 test_csd[MMC_MAX_BLOCK_LEN];

	int timeout = 1000;
	bool has_parts = false;
	bool part_completed;

	/* Put the Card in Identify Mode */
	cmd.cmdidx = mmc_host_is_spi(mmc) ? MMC_CMD_SEND_CID :
		MMC_CMD_ALL_SEND_CID; /* cmd not supported in spi */
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	memcpy(mmc->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	if (!mmc_host_is_spi(mmc)) {
		/* cmd not supported in spi */
		cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
		cmd.cmdarg = mmc->rca << 16;
		cmd.resp_type = MMC_RSP_R6;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		if (IS_SD(mmc))
			mmc->rca = (cmd.response[0] >> 16) & 0xffff;
	}

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = mmc->rca << 16;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	/* Waiting for the ready status */
	mmc_send_status(mmc, timeout);

	if (err)
		return err;

	mmc->csd[0] = cmd.response[0];
	mmc->csd[1] = cmd.response[1];
	mmc->csd[2] = cmd.response[2];
	mmc->csd[3] = cmd.response[3];

	if (mmc->version == MMC_VERSION_UNKNOWN) {
		int version = (cmd.response[0] >> 26) & 0xf;

		switch (version) {
		case 0:
			mmc->version = MMC_VERSION_1_2;
			break;
		case 1:
			mmc->version = MMC_VERSION_1_4;
			break;
		case 2:
			mmc->version = MMC_VERSION_2_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_3;
			break;
		case 4:
			mmc->version = MMC_VERSION_4;
			break;
		default:
			mmc->version = MMC_VERSION_1_2;
			break;
		}
	}

	/* divide frequency by 10, since the mults are 10x bigger */
	freq = fbase[(cmd.response[0] & 0x7)];
	mult = multipliers[((cmd.response[0] >> 3) & 0xf)];

	mmc->tran_speed = freq * mult;

	mmc->dsr_imp = ((cmd.response[1] >> 12) & 0x1);
	mmc->read_bl_len = 1 << ((cmd.response[1] >> 16) & 0xf);

	if (IS_SD(mmc))
		mmc->write_bl_len = mmc->read_bl_len;
	else
		mmc->write_bl_len = 1 << ((cmd.response[3] >> 22) & 0xf);

	if (mmc->high_capacity) {
		csize = (mmc->csd[1] & 0x3f) << 16 |
			(mmc->csd[2] & 0xffff0000) >> 16;
		cmult = 8;
	} else {
		csize = (mmc->csd[1] & 0x3ff) << 2 |
	  		(mmc->csd[2] & 0xc0000000) >> 30;
		cmult = (mmc->csd[2] & 0x00038000) >> 15;
	}

	mmc->capacity_user = (csize + 1) << (cmult + 2);
	mmc->capacity_user *= mmc->read_bl_len;
	mmc->capacity_boot = 0;
	mmc->capacity_rpmb = 0;
	for (i = 0; i < 4; i++)
		mmc->capacity_gp[i] = 0;

	if (mmc->read_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->read_bl_len = MMC_MAX_BLOCK_LEN;

	if (mmc->write_bl_len > MMC_MAX_BLOCK_LEN)
		mmc->write_bl_len = MMC_MAX_BLOCK_LEN;

	if ((mmc->dsr_imp) && (0xffffffff != mmc->dsr)) {
		cmd.cmdidx = MMC_CMD_SET_DSR;
		cmd.cmdarg = (mmc->dsr & 0xffff) << 16;
		cmd.resp_type = MMC_RSP_NONE;
		if (mmc_send_cmd(mmc, &cmd, NULL))
			info("MMC: SET_DSR failed\n");
	}

	/* Select the card, and put it into Transfer Mode */
	if (!mmc_host_is_spi(mmc)) {
		/* cmd not supported in spi */
		cmd.cmdidx = MMC_CMD_SELECT_CARD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = mmc->rca << 16;
		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;
	}

	/*
	 * For SD, its erase group is always one sector
	 */
	mmc->erase_grp_size = 1;
	mmc->part_config = MMCPART_NOAVAILABLE;
	if (!IS_SD(mmc) && (mmc->version >= MMC_VERSION_4)) {
		/* check  ext_csd version and capacity */
		err = mmc_send_ext_csd(mmc, ext_csd);
		if (err)
			return err;
		if (ext_csd[EXT_CSD_REV] >= 2) {
			/*
			 * According to the JEDEC Standard, the value of
			 * ext_csd's capacity is valid if the value is more
			 * than 2GB
			 */
			capacity = ext_csd[EXT_CSD_SEC_CNT] << 0 |
				ext_csd[EXT_CSD_SEC_CNT + 1] <<
				8 |
				ext_csd[EXT_CSD_SEC_CNT + 2] <<
				16 | ext_csd[EXT_CSD_SEC_CNT + 3] << 24;
			capacity *= MMC_MAX_BLOCK_LEN;
			if ((capacity >> 20) > 2 * 1024)
				mmc->capacity_user = capacity;
		}

		switch (ext_csd[EXT_CSD_REV]) {
		case 1:
			mmc->version = MMC_VERSION_4_1;
			break;
		case 2:
			mmc->version = MMC_VERSION_4_2;
			break;
		case 3:
			mmc->version = MMC_VERSION_4_3;
			break;
		case 5:
			mmc->version = MMC_VERSION_4_41;
			break;
		case 6:
			mmc->version = MMC_VERSION_4_5;
			break;
		case 7:
			mmc->version = MMC_VERSION_5_0;
			break;
		}

		/* The partition data may be non-zero but it is only
		 * effective if PARTITION_SETTING_COMPLETED is set in
		 * EXT_CSD, so ignore any data if this bit is not set,
		 * except for enabling the high-capacity group size
		 * definition (see below). */
		part_completed = !!(ext_csd[EXT_CSD_PARTITION_SETTING] &
			EXT_CSD_PARTITION_SETTING_COMPLETED);

		/* store the partition info of emmc */
		mmc->part_support = ext_csd[EXT_CSD_PARTITIONING_SUPPORT];
		if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) ||
			ext_csd[EXT_CSD_BOOT_MULT])
			mmc->part_config = ext_csd[EXT_CSD_PART_CONF];
		if (part_completed &&
			(
			ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & ENHNCD_SUPPORT))
			mmc->part_attr = ext_csd[EXT_CSD_PARTITIONS_ATTRIBUTE];

		mmc->capacity_boot = ext_csd[EXT_CSD_BOOT_MULT] << 17;

		mmc->capacity_rpmb = ext_csd[EXT_CSD_RPMB_MULT] << 17;

		for (i = 0; i < 4; i++) {
			int idx = EXT_CSD_GP_SIZE_MULT + i * 3;
			uint mult = (ext_csd[idx + 2] << 16) +
					(ext_csd[idx + 1] << 8) + ext_csd[idx];
			if (mult)
				has_parts = true;
			if (!part_completed)
				continue;
			mmc->capacity_gp[i] = mult;
			mmc->capacity_gp[i] *=
			ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE];
			mmc->capacity_gp[i] *= ext_csd[EXT_CSD_HC_WP_GRP_SIZE];
			mmc->capacity_gp[i] <<= 19;
		}

		if (part_completed) {
			mmc->enh_user_size = (
				ext_csd[EXT_CSD_ENH_SIZE_MULT + 2] << 16) + (
				ext_csd[EXT_CSD_ENH_SIZE_MULT + 1] <<
				8) + ext_csd[EXT_CSD_ENH_SIZE_MULT];
			mmc->enh_user_size *=
			ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE];
			mmc->enh_user_size *= ext_csd[EXT_CSD_HC_WP_GRP_SIZE];
			mmc->enh_user_size <<= 19;
			mmc->enh_user_start = (
				ext_csd[EXT_CSD_ENH_START_ADDR + 3] << 24) + (
				ext_csd[EXT_CSD_ENH_START_ADDR + 2] <<
				16) +
				(
				ext_csd[EXT_CSD_ENH_START_ADDR + 1] <<
				8) + ext_csd[EXT_CSD_ENH_START_ADDR];
			if (mmc->high_capacity)
				mmc->enh_user_start <<= 9;
		}

		/*
		 * Host needs to enable ERASE_GRP_DEF bit if device is
		 * partitioned. This bit will be lost every time after a reset
		 * or power off. This will affect erase size.
		 */
		if (part_completed)
			has_parts = true;
		if ((ext_csd[EXT_CSD_PARTITIONING_SUPPORT] & PART_SUPPORT) &&
			(
			ext_csd[EXT_CSD_PARTITIONS_ATTRIBUTE] &
			PART_ENH_ATTRIB))
			has_parts = true;
		if (has_parts) {
			err = mmc_switch(mmc,
				EXT_CSD_CMD_SET_NORMAL,
				EXT_CSD_ERASE_GROUP_DEF,
				1);

			if (err)
				return err;
			else
				ext_csd[EXT_CSD_ERASE_GROUP_DEF] = 1;
		}

		if (ext_csd[EXT_CSD_ERASE_GROUP_DEF] & 0x01) {
			/* Read out group size from ext_csd */
			mmc->erase_grp_size =
			ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] * 1024;
			/*
			 * if high capacity and partition setting completed
			 * SEC_COUNT is valid even if it is smaller than 2 GiB
			 * JEDEC Standard JESD84-B45, 6.2.4
			 */
			if (mmc->high_capacity && part_completed) {
				capacity = (ext_csd[EXT_CSD_SEC_CNT]) |
					(ext_csd[EXT_CSD_SEC_CNT + 1] <<
					8) | (
					ext_csd[EXT_CSD_SEC_CNT + 2] <<
					16) |
					(ext_csd[EXT_CSD_SEC_CNT + 3] << 24);
				capacity *= MMC_MAX_BLOCK_LEN;
				mmc->capacity_user = capacity;
			}
		} else {
		  	/* Calculate the group size from the csd value. */
			int erase_gsz,erase_gmul;
			erase_gsz = (mmc->csd[2] & 0x00007c00) >> 10;
			erase_gmul = (mmc->csd[2] & 0x000003e0) >> 5;
			mmc->erase_grp_size = (erase_gsz + 1) *
		  		(erase_gmul + 1);
		}

		mmc->hc_wp_grp_size = 1024 *
			ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] *
			ext_csd[EXT_CSD_HC_WP_GRP_SIZE];

		mmc->wr_rel_set = ext_csd[EXT_CSD_WR_REL_SET];
	}

	err = mmc_set_capacity(mmc, mmc->block_dev.hwpart);
	if (err)
		return err;

	if (IS_SD(mmc))
		err = sd_change_freq(mmc);
	else
		err = mmc_change_freq(mmc);

	if (err)
		return err;

	/* Restrict card's capabilities by what the host can do */
	mmc->card_caps &= mmc->cfg->host_caps;
	info("mmc_startup: mmc->card_caps=0x%x, mmc->cfg->host_caps=0x%x\n", 
			mmc->card_caps, mmc->cfg->host_caps);

	if (IS_SD(mmc)) {
		if ((mmc->user_bus_width >=4) || (mmc->user_bus_width==0)) {
			if (mmc->user_bus_width != 4)
				info("SD Card, only support 1-4 bit,now %d bit, force to 4 bit\n", mmc->user_bus_width);
			if (mmc->card_caps & MMC_MODE_4BIT) {
				cmd.cmdidx = MMC_CMD_APP_CMD;
				cmd.resp_type = MMC_RSP_R1;
				cmd.cmdarg = mmc->rca << 16;

				err = mmc_send_cmd(mmc, &cmd, NULL);
				if (err)
					return err;

				cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
				cmd.resp_type = MMC_RSP_R1;
				cmd.cmdarg = 2;
				err = mmc_send_cmd(mmc, &cmd, NULL);
				if (err)
					return err;

				mmc_set_bus_width(mmc, 4);
			}
		}

		if (mmc->card_caps & MMC_MODE_HS)
			mmc->tran_speed = mmc->sys_max_tran_speed;
		else
			mmc->tran_speed = mmc->sys_max_tran_speed / 2;
	} else if (mmc->version >= MMC_VERSION_4) {
	  	/* Only version 4 of MMC supports wider bus widths */
		int idx;

	  	/* An array of possible bus widths in order of preference */
		static unsigned ext_csd_bits[] = {
			EXT_CSD_DDR_BUS_WIDTH_8,EXT_CSD_DDR_BUS_WIDTH_4,
	  		EXT_CSD_BUS_WIDTH_8,EXT_CSD_BUS_WIDTH_4,
	  		EXT_CSD_BUS_WIDTH_1,
	  	};

	  	/* An array to map CSD bus widths to host cap bits */
		static unsigned ext_to_hostcaps[] = {
			[EXT_CSD_DDR_BUS_WIDTH_4] = MMC_MODE_DDR_52MHz |
	  		MMC_MODE_4BIT,
	  		[EXT_CSD_DDR_BUS_WIDTH_8] = MMC_MODE_DDR_52MHz |
	  		MMC_MODE_8BIT,[EXT_CSD_BUS_WIDTH_4] = MMC_MODE_4BIT,
	  		[EXT_CSD_BUS_WIDTH_8] = MMC_MODE_8BIT,
	  	};

	  	/* An array to map chosen bus width to an integer */
		static unsigned widths[] = {
			8,4,8,4,1,
		};

		for (idx = 0; idx < ARRAY_SIZE(ext_csd_bits); idx++) {
			unsigned int extw = ext_csd_bits[idx];
			unsigned int caps = ext_to_hostcaps[extw];

	  		/*
			 * If the bus width is still not changed,
			 * don't try to set the default again.
			 * Otherwise, recover from switch attempts
			 * by switching to 1-bit bus width.
			 */
			if (extw == EXT_CSD_BUS_WIDTH_1 && mmc->bus_width == 1) {
				err = 0;
				break;
			}

	  		/*
			 * Check to make sure the card and controller support
			 * these capabilities
			 */
			if ((mmc->card_caps & caps) != caps)
				continue;

			/*user request a low bit width*/
			if (mmc->user_bus_width!=0 && mmc->user_bus_width < widths[idx])
				continue;

			err = mmc_switch(mmc,
	  			EXT_CSD_CMD_SET_NORMAL,
	  			EXT_CSD_BUS_WIDTH,
	  			extw);

			if (err)
				continue;

			mmc->ddr_mode = (caps & MMC_MODE_DDR_52MHz) ? 1 : 0;
			mmc_set_bus_width(mmc, widths[idx]);

			err = mmc_send_ext_csd(mmc, test_csd);

			if (err)
				continue;

	  		/* Only compare read only fields */
			if (
	  			ext_csd[EXT_CSD_PARTITIONING_SUPPORT] ==
	  			test_csd[EXT_CSD_PARTITIONING_SUPPORT] &&
	  			ext_csd[EXT_CSD_HC_WP_GRP_SIZE] ==
	  			test_csd[EXT_CSD_HC_WP_GRP_SIZE] &&
	  			ext_csd[EXT_CSD_REV] ==
	  			test_csd[EXT_CSD_REV] &&
	  			ext_csd[EXT_CSD_HC_ERASE_GRP_SIZE] ==
	  			test_csd[EXT_CSD_HC_ERASE_GRP_SIZE] &&
	  			memcmp(&ext_csd[EXT_CSD_SEC_CNT],
	  			&test_csd[EXT_CSD_SEC_CNT],
	  			4) == 0)
				break;
			else
				err = SWITCH_ERR;
		}

		if (err)
			return err;
#if 0
		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc->tran_speed = 52000000;
			else
				mmc->tran_speed = 26000000;
		}
#else
		if (mmc->card_caps & MMC_MODE_HS) {
			if (mmc->card_caps & MMC_MODE_HS_52MHz)
				mmc->tran_speed = mmc->sys_max_tran_speed;
			else
				mmc->tran_speed = mmc->sys_max_tran_speed / 2;
		}
#endif
	}

	if ((mmc->user_speed==0) || (mmc->user_speed > mmc->tran_speed))
		mmc_set_clock(mmc, mmc->tran_speed);
	else
		mmc_set_clock(mmc, mmc->user_speed);

	/* Fix the block length for DDR mode */
	if (mmc->ddr_mode) {
		mmc->read_bl_len = MMC_MAX_BLOCK_LEN;
		mmc->write_bl_len = MMC_MAX_BLOCK_LEN;
	}

	/* fill in device description */
	mmc->block_dev.lun = 0;
	mmc->block_dev.hwpart = 0;
	mmc->block_dev.type = 0;
	mmc->block_dev.blksz = mmc->read_bl_len;
	mmc->block_dev.log2blksz = LOG2(mmc->block_dev.blksz);
	mmc->block_dev.lba = cb_lldiv(mmc->capacity, mmc->read_bl_len);

	sprintf(mmc->block_dev.vendor, "Man %06x Snr %04x%04x", mmc->cid[0] >> 24,
		(mmc->cid[2] & 0xffff),
		(mmc->cid[3] >> 16) & 0xffff);
	sprintf(mmc->block_dev.product,
		"%c%c%c%c%c%c",
		mmc->cid[0] & 0xff,
		(mmc->cid[1] >> 24),
		(mmc->cid[1] >> 16) & 0xff,
		(mmc->cid[1] >> 8) & 0xff,
		mmc->cid[1] & 0xff,
		(mmc->cid[2] >> 24) & 0xff);
	sprintf(mmc->block_dev.revision, "%d.%d", (mmc->cid[2] >> 20) & 0xf,
		(mmc->cid[2] >> 16) & 0xf);

	return 0;
}



struct mmc* mmc_create(const struct mmc_config* cfg, void* priv)
{
	struct mmc* mmc;

	/* quick validation */
	if (cfg == NULL || cfg->ops == NULL || cfg->ops->send_cmd == NULL ||
		cfg->f_min == 0 || cfg->f_max == 0 || cfg->b_max == 0)
		return NULL;

	mmc = p_mmc;
	memset(mmc, 0, sizeof(struct mmc));

	/* save the config locally */
	memcpy(&drivers_config, cfg, sizeof(struct mmc_config));
	drivers_config.ops = &drivers_mmc_ops;

	mmc->cfg = &drivers_config;
	mmc->priv = priv;

	/* the following chunk was mmc_register() */

	/* Setup dsr related values */
	mmc->dsr_imp = 0;
	mmc->dsr = 0xffffffff;
	/* Setup the universal parts of the block interface just once */
	mmc->block_dev.if_type = IF_TYPE_MMC;

	return mmc;
}


static int mmc_start_init(struct mmc* mmc)
{
	int err;

	/* we pretend there's no card when init is NULL */
	if (mmc_getcd(mmc) == 0 || mmc->cfg->ops->init == NULL) {
		mmc->has_init = 0;
		info("MMC: no card present\n");
		return NO_CARD_ERR;
	}

	if (mmc->has_init)
		return 0;

	/* made sure it's not NULL earlier */
	err = mmc->cfg->ops->init(mmc);

	if (err)
		return err;

	mmc->ddr_mode = 0;
	mmc_set_bus_width(mmc, 1);
	mmc_set_clock(mmc, 1);

	/* Reset the Card */
	err = mmc_go_idle(mmc);

	if (err)
		return err;

	/* The internal partition reset to user partition(0) at every CMD0*/
	mmc->block_dev.hwpart = 0;

	/* Test for SD version 2 */
	err = mmc_send_if_cond(mmc);

	/* Now try to get the SD card's operating condition */
	err = sd_send_op_cond(mmc);

	/* If the command timed out, we check for an MMC card */
	if (err == TIMEOUT) {
		err = mmc_send_op_cond(mmc);

		if (err) {
			info("Card did not respond to voltage select!\n");
			return UNUSABLE_ERR;
		}
	}

	if (!err)
		mmc->init_in_progress = 1;

	return err;
}

static int mmc_complete_init(struct mmc* mmc)
{
	int err = 0;

	mmc->init_in_progress = 0;
	if (mmc->op_cond_pending)
		err = mmc_complete_op_cond(mmc);

	if (!err)
		err = mmc_startup(mmc);
	if (err)
		mmc->has_init = 0;
	else
		mmc->has_init = 1;
	return err;
}

int mmc_init_card(struct mmc* mmc)
{
	int err = 0;
	unsigned long long start;
	unsigned long long ms_elapsed;
	unsigned long high, low;

	if (mmc->has_init)
		return 0;

	start = get_timer(0);

	if (!mmc->init_in_progress)
		err = mmc_start_init(mmc);

	if (!err)
		err = mmc_complete_init(mmc);

	ms_elapsed = get_timer(start);
	high = (ms_elapsed >> 32);
	low = (ms_elapsed & 0xFFFFFFFF);
	if (high)
		debug("%s: %d, time 0x%x_%x ms\n", __func__, err, high, low);
	else
		debug("%s: %d, time %lu ms\n", __func__, err, low);
	return err;
}

