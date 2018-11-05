/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "drivers_config.h"
#include "cdl_spi.h"
#include "spi_flash.h"

struct spi_flash sf;
struct spi_flash* psf = &sf;

int sf_set_controller(void* dev,spi_xfer_fn xfer)
{
	memset(psf, 0, sizeof(struct spi_flash));
	psf->spi = dev;
	psf->xfer = xfer;
	
	return 0;
}

static int sf_read_write(const u8* cmd, size_t cmd_len, const u8* data_out, u8* data_in, size_t data_len,int mode)
{
	unsigned long flags = SPI_XFER_BEGIN;
	int ret;

	if (data_len == 0)
	{
		flags |= SPI_XFER_END;
	}
	
	ret = psf->xfer(psf->spi, cmd_len * 8, cmd, NULL, flags);
	if (ret) 
	{
		debug("SF: Failed to send command (%zu bytes): %d\n", cmd_len, ret);
	} 
	else if (data_len != 0) 
	{
		flags = SPI_XFER_END;
		flags |= mode<<8;
		ret = psf->xfer(psf->spi, data_len * 8, data_out, data_in, flags);
		if (ret)
		{
	  		debug("SF: Failed to transfer %zu bytes of data: %d\n", data_len, ret);
		}	
	}

	return ret;
}

static int sf_cmd_read(const u8* cmd, size_t cmd_len, void* data, size_t data_len, int mode)
{
	return sf_read_write(cmd, cmd_len, NULL, data, data_len, mode);
}

static int sf_cmd_write(const u8* cmd, size_t cmd_len, const void* data, size_t data_len, int mode)
{
	return sf_read_write(cmd, cmd_len, data, NULL, data_len, mode);
}

static int sf_cmd(u8 cmd, void* response, size_t len)
{
	return sf_cmd_read(&cmd, 1, response, len,0);
}

/*this is for spi nand only*/
static int get_feature(u8 addr,u8* rs)
{
	int ret;
	u8 cmd[2];

	cmd[0] = 0x0f;
	cmd[1] = addr;
	ret = sf_cmd_read(cmd, 2, rs, 1,0);
	if (ret < 0) {
		debug("SF: fail to get status register\n");
		return ret;
	}

	return 0;
}
static int set_feature(u8 addr,u8 rs)
{
	int ret;
	u8 cmd[3];

	cmd[0] = 0x1f;
	cmd[1] = addr;
	cmd[2]= rs;
	ret = sf_cmd_write(cmd, 1, &cmd[1], 2,0);
	if (ret < 0) {
		debug("SF: fail to set feature register\n");
		return ret;
	}

	return 0;
}
static int read_sr(u8* rs)
{
	int ret;
	u8 cmd;

	cmd = CMD_READ_STATUS;
	ret = sf_cmd_read(&cmd, 1, rs, 1,0);
	if (ret < 0) {
		debug("SF: fail to read status register\n");
		return ret;
	}

	return 0;
}

static int read_fsr(u8* fsr)
{
	int ret;
	const u8 cmd = CMD_FLAG_STATUS;

	ret = sf_cmd_read(&cmd, 1, fsr, 1,0);
	if (ret < 0) {
		debug("SF: fail to read flag status register\n");
		return ret;
	}

	return 0;
}

static int sf_sr_ready(void)
{
	u8 sr;
	int ret;

	ret = read_sr(&sr);
	if (ret < 0)
		return ret;

	return !(sr & STATUS_WIP);
}

static int sf_fsr_ready(void)
{
	u8 fsr;
	int ret;

	ret = read_fsr(&fsr);
	if (ret < 0)
		return ret;

	return fsr & STATUS_PEC;
}

static int sf_ready(void)
{
	int sr,fsr;
	u8 status = 0xff;
	int ret;

	if (psf->spi_nand_flag) 
	{ 
		/*spi nand*/
		ret = get_feature(0xc0, &status);
		if (ret || (status & 0x1))
		{
			return 0;
		}	
		else
		{
			return 1;
		}
	} 
	else 
	{ 
		/*spi nor*/
		sr = sf_sr_ready();
		if (sr < 0)
		{
			return sr;
		}
		
		fsr = 1;
		if (psf->flags & SNOR_F_USE_FSR) 
		{
			fsr = sf_fsr_ready();
			if (fsr < 0)
			{
				return fsr;
			}	
		}
	}
	
	return sr && fsr;
}
static int sf_cmd_wait_ready(unsigned long timeout)
{
	int ret;
	unsigned long long timebase;

	timebase = get_timer(0);

	do {
		ret = sf_ready();
		if (ret < 0)
		{
			return ret;
		}
		
		if (ret)
		{
			return 0;
		}
	} while (get_timer(timebase) < timeout);

	info("SF: sf_cmd_wait_ready timeout!\n");

	return -1;
}

/* Enable writing on the SPI flash */
static inline int sf_cmd_write_enable(void)
{
	return sf_cmd(CMD_WRITE_ENABLE, NULL, 0);
}

static int sf_write_common(const u8 *cmd,size_t cmd_len,
	const void *buf, size_t buf_len)
{
	unsigned long timeout = SPI_FLASH_PROG_TIMEOUT;
	int ret;

	if (buf == NULL)
		timeout = SPI_FLASH_PAGE_ERASE_TIMEOUT;

	ret = sf_cmd_write_enable();
	if (ret < 0) {
		debug("SF: enabling write failed\n");
		return ret;
	}

	ret = sf_cmd_write(cmd, cmd_len, buf, buf_len,0);
	if (ret < 0) {
		debug("SF: write cmd failed\n");
		return ret;
	}

	ret = sf_cmd_wait_ready(timeout);
	if (ret < 0) {
		debug("SF: write %s timed out\n",
		      timeout == SPI_FLASH_PROG_TIMEOUT ?
			"program" : "page erase");
		return ret;
	}
	return ret;
}

static int read_cr(u8 *rc)
{
	int ret;
	u8 cmd;

	cmd = CMD_READ_CONFIG;
	ret = sf_cmd_read(&cmd, 1, rc, 1,0);
	if (ret < 0) {
		debug("SF: fail to read config register\n");
		return ret;
	}

	return 0;
}

static int write_cr(u8 wc)
{
	u8 data[2];
	u8 cmd;
	int ret;

	ret = read_sr(&data[0]);
	if (ret < 0)
		return ret;

	cmd = CMD_WRITE_STATUS;
	data[1] = wc;
	ret = sf_write_common(&cmd, 1, &data, 2);
	if (ret) {
		debug("SF: fail to write config register\n");
		return ret;
	}

	return 0;
}

static int spansion_quad_enable(void)
{
	u8 qeb_status;
	int ret;

	ret = read_cr(&qeb_status);
	if (ret < 0)
		return ret;

	if (qeb_status & STATUS_QEB_WINSPAN)
		return 0;

	ret = write_cr(qeb_status | STATUS_QEB_WINSPAN);
	if (ret < 0)
		return ret;

	/* read CR and check it */
	ret = read_cr(&qeb_status);
	if (!(ret >= 0 && (qeb_status & STATUS_QEB_WINSPAN))) {
		info("SF: Spansion CR Quad bit not clear\n");
		return -EINVAL;
	}

	return ret;
}

static int mx_disable_wp(void)
{
	u8 wp_status;
	int ret;

	ret = get_feature(0xa0,&wp_status);
	if (ret < 0)
		return ret;
	//info("wp status:%x\n",wp_status);

	ret =  set_feature(0xa0,0);
	if (ret < 0)
		return ret;

	return ret;
}
static int mx_quad_enable(void)
{
	u8 wp_status;
	int ret;

	ret = get_feature(0xb0,&wp_status);
	if (ret < 0)
		return ret;
	//info("otp status:%x\n",wp_status);

	wp_status = 0x11;
	ret =  set_feature(0xb0,wp_status);
	if (ret < 0)
		return ret;

	return ret;
}

static void sf_addr(u32 addr, u8* cmd)
{
	/* cmd[0] is actual command */
	cmd[1] = addr >> 16;
	cmd[2] = addr >> 8;
	cmd[3] = addr >> 0;
}
static void sf_addr2(u32 addr, u8* cmd)
{
	/* cmd[0] is actual command */
	cmd[1] = addr >> 8;
	cmd[2] = addr >> 0;
	cmd[3] = 0;
}
static void sf_addr_spi_nand(u32 addr, u8* cmd)
{
	u32 page_addr;

	page_addr = addr / psf->page_size;
	cmd[1] = page_addr>>16;
	cmd[2] = page_addr>>8;
	cmd[3] = page_addr>>0;
}

int sf_probe(void)
{
	u8 idcode[5];
	int ret;

	ret = sf_cmd(CMD_READ_ID, idcode, sizeof(idcode));
	if (ret) 
	{
		info("SF: Failed to get idcodes\n");
		return ret;
	}

	/*identify spi nand and fill operation parameters*/
	if ((idcode[0] == 0xc8) && (idcode[1] == 0x40) && (idcode[2] == 0x15)) 
	{
		info("detect spi flash: GD25Q16B\n");
		
		psf->page_size = 256;
		psf->sector_size = psf->page_size * 16; 	/* 4k */
		psf->block_size = psf->sector_size * 16; 	/* 64k*/
		psf->size = psf->block_size * 32; 			/* 2M */

		psf->erase_cmd = CMD_ERASE_4K;
		psf->erase_size = 4096;

		psf->read_cmd = CMD_READ_ARRAY_FAST;
		psf->read_cmd_dual = CMD_READ_DUAL_OUTPUT_FAST;
		psf->read_cmd_quad = CMD_READ_QUAD_OUTPUT_FAST;

		/*use the write command don't reset cache*/
		psf->write_cmd = CMD_PAGE_PROGRAM;
		psf->write_cmd_quad = CMD_QUAD_PAGE_PROGRAM;

		psf->dummy_byte = 1;

		spansion_quad_enable();
	}  
	else if ( (idcode[1] == 0xc2) && (idcode[2] == 0x22)) 
	{
		info("detect spi flash: MX35LF2GE4AB\n");
		
		psf->page_size = 2048;
		psf->block_size = psf->page_size * 64; 	/* 128k*/
		psf->size = psf->block_size * 2048; 	/* 2G bit, 256M*/

		psf->erase_cmd = CMD_ERASE_64K;
		psf->erase_size = psf->block_size;

		psf->read_cmd = CMD_READ_ARRAY_FAST;
		psf->read_cmd_dual = CMD_READ_DUAL_OUTPUT_FAST;
		psf->read_cmd_quad = CMD_READ_QUAD_OUTPUT_FAST;

		/*use the write command don't reset cache*/
		psf->write_cmd = 0x84;
		psf->write_cmd_quad = 0x34;

		psf->dummy_byte = 0;

		psf->spi_nand_flag = 1;
		psf->spi_nand_prefetch_cmd = 0x13;
		psf->spi_nand_program_cmd = 0x10;

		mx_disable_wp();
		mx_quad_enable();
	}
	else if ( (idcode[1] == 0xc2) && (idcode[2] == 0x12)) 
	{
		info("detect spi flash: MX35LF1GE4AB\n");
		
		psf->page_size = 2048;
		psf->block_size = psf->page_size * 64; 	/* 128k*/
		psf->size = psf->block_size * 1024; 	/* 1G bit, 128M*/

		psf->erase_cmd = CMD_ERASE_64K;
		psf->erase_size = psf->block_size;

		psf->read_cmd = CMD_READ_ARRAY_FAST;
		psf->read_cmd_dual = CMD_READ_DUAL_OUTPUT_FAST;
		psf->read_cmd_quad = CMD_READ_QUAD_OUTPUT_FAST;
		
		/*use the write command don't reset cache*/
		psf->write_cmd = 0x84;
		psf->write_cmd_quad = 0x34;

		psf->dummy_byte = 0;

		psf->spi_nand_flag = 1;
		psf->spi_nand_prefetch_cmd = 0x13;
		psf->spi_nand_program_cmd = 0x10;

		mx_disable_wp();
		mx_quad_enable();
	}
	else 
	{
	  	info("unknow spi flash: %x %x %x\n", idcode[0], idcode[1], idcode[2]);
		ret = 1;
	}
	
	if (!ret) 
	{
		info("flash chip is spi '%s' flash, total size %d M bytes\n", psf->spi_nand_flag ? "nand" : "nor", psf->size/(1024*1024));
		info("page size=%d \n",psf->page_size);
		info("block size=%d \n",psf->block_size);
		info("erase size=%d \n",psf->erase_size);
		info("\n");
	}
	
	return ret;
}

int sf_read(unsigned long offset, int len, char* buf,int mode)
{
	u8* command = psf->command;
	unsigned int addr;
	u8 cmdsz;
	int ret;
	size_t chunk_len,actual;
	unsigned long byte_addr;
	u32 roff;
	u32 rtotal;
	u32 rlen;

	/* Calculate flash page/byte address */
	addr = offset;

	if (psf->spi_nand_flag) 
	{
		for (actual = 0; actual < len; actual += chunk_len) 
		{
			byte_addr = addr % psf->page_size;
			chunk_len = min(len - actual, (size_t)(psf->page_size - byte_addr));

			command[0] = psf->spi_nand_prefetch_cmd;
			
			sf_addr_spi_nand(addr,command);
			
			/*page read to cache*/
			ret = sf_cmd_write(command, SPI_FLASH_CMD_LEN, NULL, 0, 0);
			if (ret < 0) 
			{
				debug("SF: page read cmd failed\n");
				return ret;
			}

			ret = sf_cmd_wait_ready(200);
			if (ret < 0) 
			{
				debug("SF: wait page read complete timeout\n");
				return ret;
			}
			
			if (mode == 1)
			{
				command[0] = psf->read_cmd_dual;
			}	
			else if (mode == 2)
			{
				command[0] = psf->read_cmd_quad;
			}	
			else
			{
				command[0] = psf->read_cmd;
			}
			
			rtotal = chunk_len;
			rlen = 256;
			roff = 0;
			while (rtotal > 0) 
			{
				sf_addr2(roff, command);
				ret = sf_cmd_read(command, SPI_FLASH_CMD_LEN, (buf + actual + roff), rlen, mode);
				if (ret < 0) 
				{
					debug("SF: read cmd failed\n");
					return ret;
				}
				
				roff += rlen;
				rtotal -= rlen;
			}
			addr += chunk_len;
		}
	} 
	else 
	{
		if (mode == 1)
		{
			command[0] = psf->read_cmd_dual;
		}	
		else if (mode == 2)
		{
			command[0] = psf->read_cmd_quad;
		}	
		else
		{
			command[0] = psf->read_cmd;
		}
		
		sf_addr(addr, command);
		cmdsz = SPI_FLASH_CMD_LEN + psf->dummy_byte;
		
		ret = sf_cmd_read(command, cmdsz, buf, len, mode);
		if (ret) 
		{
			info("SF: read failed\n");
			return ret;
		}
	}
	
	return 0;
}

int sf_write(unsigned long offset, int len, char* buf,int mode)
{
	u8* command = psf->command;
	size_t chunk_len,actual;
	u32 write_addr;
	unsigned long byte_addr;
	int ret;
	unsigned long timeout = SPI_FLASH_PROG_TIMEOUT;
	u32 woff;
	u32 wtotal;
	u32 wlen;

	if (mode==2)
		command[0] = psf->write_cmd_quad;
	else
		command[0] = psf->write_cmd;

	for (actual = 0; actual < len; actual += chunk_len) {
		write_addr = offset;
		byte_addr = offset % psf->page_size;
		chunk_len = min(len - actual,
			(size_t)(psf->page_size - byte_addr));
		sf_addr(write_addr, command);

		ret = sf_cmd_write_enable();
		if (ret < 0) {
			debug("SF: enabling write failed\n");
			return ret;
		}

		if (psf->spi_nand_flag) {
			if (mode==2)
				command[0] = psf->write_cmd_quad;
			else
				command[0] = psf->write_cmd;
			wtotal = chunk_len;
			wlen = 256;
			woff = 0;
			while (wtotal >0) {
				sf_addr2(woff,command);
				ret = sf_cmd_write(command,
					3,
					buf + actual+woff,
					wlen,mode);
				if (ret < 0) {
					debug("SF: write cmd failed\n");
					return ret;
				}
				woff += wlen;
				wtotal -= wlen;
			}
			/*program*/
			command[0] =psf->spi_nand_program_cmd;
			sf_addr_spi_nand(write_addr,command);
			ret = sf_cmd_write(command, SPI_FLASH_CMD_LEN, NULL, 0,0);
			if (ret < 0) {
				debug("SF: page write cmd failed\n");
				return ret;
			}
		} else {
			ret = sf_cmd_write(command,
				SPI_FLASH_CMD_LEN,
				buf + actual,
				chunk_len,mode);
			if (ret < 0) {
				debug("SF: write cmd failed\n");
				return ret;
			}
		}

		ret = sf_cmd_wait_ready(timeout);
		if (ret < 0) {
			debug("SF: write %s timed out\n",
				timeout ==
				SPI_FLASH_PROG_TIMEOUT ?
				"program" : "page erase");
			return ret;
		}
		offset += chunk_len;
	}

	return 0;
}

int sf_erase(unsigned long offset, int len)
{
	u8* command = psf->command;
	u32 erase_size,erase_addr;
	int ret = -1;
	unsigned long timeout = SPI_FLASH_PAGE_ERASE_TIMEOUT;

	erase_size = psf->erase_size;

	/* Consistency checking */
	if (offset + len > psf->size) 
	{
		info("ERROR: attempting erase past flash size (%#x)\n", psf->size);
		return 1;
	}
	
	if (offset % psf->erase_size || len % psf->erase_size || len == 0) 
	{
		debug("SF: Erase offset/length not multiple of erase size offset/psf->erase_size/len = %d/%d/%d\n", offset, psf->erase_size, len);
		return -1;
	}

	command[0] = psf->erase_cmd;

	while (len) 
	{
		erase_addr = offset;
		
		if (psf->spi_nand_flag)
		{
			sf_addr_spi_nand(erase_addr, command);
		}	
		else
		{
			sf_addr(erase_addr, command);
		}
		
		debug("SF: erase %2x %2x %2x %2x (%x)\n", command[0], command[1], command[2], command[3], erase_addr);

		ret = sf_cmd_write_enable();
		if (ret < 0) 
		{
			debug("SF: enabling write failed\n");
			return ret;
		}

		ret = sf_cmd_write(command, SPI_FLASH_CMD_LEN, NULL, 0, 0);
		if (ret < 0) 
		{
			debug("SF: write cmd failed\n");
			return ret;
		}

		ret = sf_cmd_wait_ready(timeout);
		if (ret < 0) 
		{
			debug("SF: write %s timed out\n", (timeout == SPI_FLASH_PROG_TIMEOUT) ? "program" : "page erase");
			return ret;
		}

		offset += erase_size;
		len -= erase_size;
	}
	return 0;
}

int sf_protect(unsigned long offset, int len, int en)
{
	return 0;
}

