/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __SPI_FLASH_H__
#define __SPI_FLASH_H__

typedef int (*spi_xfer_fn)(void*,
	unsigned int,
	const void*,
	void*,
	unsigned long);

/* Erase commands */
#define CMD_ERASE_4K			0x20
#define CMD_ERASE_32K			0x52
#define CMD_ERASE_CHIP			0xc7
#define CMD_ERASE_64K			0xd8

/* Write commands */
#define CMD_WRITE_STATUS		0x01
#define CMD_PAGE_PROGRAM		0x02
#define CMD_WRITE_DISABLE		0x04
#define CMD_WRITE_ENABLE		0x06
#define CMD_QUAD_PAGE_PROGRAM	0x32
#define CMD_WRITE_EVCR			0x61

/* Read commands */
#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b
#define CMD_READ_DUAL_OUTPUT_FAST	0x3b
#define CMD_READ_DUAL_IO_FAST		0xbb
#define CMD_READ_QUAD_OUTPUT_FAST	0x6b
#define CMD_READ_QUAD_IO_FAST	0xeb
#define CMD_READ_ID			0x9f
#define CMD_READ_STATUS		0x05
#define CMD_READ_STATUS1		0x35
#define CMD_READ_CONFIG		0x35
#define CMD_FLAG_STATUS		0x70
#define CMD_READ_EVCR			0x65

#define SPI_FLASH_3B_ADDR_LEN		3
#define SPI_FLASH_CMD_LEN		(1 + SPI_FLASH_3B_ADDR_LEN)

/* Flash timeout values */
#define SPI_FLASH_PROG_TIMEOUT	(2 * 100)
#define SPI_FLASH_PAGE_ERASE_TIMEOUT	(5 * 100)

/* Common status */
#define STATUS_WIP			(1<<0)
#define STATUS_QEB_WINSPAN		(1<<1)
#define STATUS_QEB_MXIC		(1<<6)
#define STATUS_PEC			(1<<7)
#define STATUS_QEB_MICRON		(1<<7)
#define SR_BP0				(1<<2)  /* Block protect 0 */
#define SR_BP1				(1<<3)  /* Block protect 1 */
#define SR_BP2				(1<<4)  /* Block protect 2 */

enum spi_nor_option_flags {
	SNOR_F_SST_WR = 0x1,
	SNOR_F_USE_FSR = 0x2,
};

struct spi_flash {
	void* spi;
	spi_xfer_fn xfer;

	unsigned long size;/*total size(bytes) of flash*/

	unsigned long block_size;
	unsigned long sector_size;
	unsigned long page_size;

	unsigned long erase_size;

	unsigned char command[16];

	int dummy_byte; /*dummy bytes after command*/

	u8 read_cmd;
	u8 write_cmd;
	u8 read_cmd_dual;
	u8 read_cmd_quad;
	u8 write_cmd_quad;
	u8 erase_cmd;

	int flags;

	int spi_nand_flag;
	u8 spi_nand_prefetch_cmd;
	u8 spi_nand_program_cmd;
};

/*apis*/
int sf_set_controller(void* dev,spi_xfer_fn xfer);
int sf_probe(void);
int sf_read(unsigned long offset, int len, char* buf,int mode);
int sf_write(unsigned long offset, int len, char* buf,int mode);
int sf_erase(unsigned long offset, int len);
int sf_protect(unsigned long offset, int len, int en);

#endif
