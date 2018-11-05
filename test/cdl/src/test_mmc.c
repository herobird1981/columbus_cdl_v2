/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_mmc.h"
#include "cdl_pll.h"

#if (CONFIG_MMC_TEST == 1)
#include "mmc.h"

struct dwmci_host mmc_dev;
struct dwmci_host* p_mmc_dev = &mmc_dev;
struct mmc* p_test_mmc = NULL;
static uint32_t sdio_port;

cmdline_entry mmc_test_menu[];

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&mmc_test_menu[0]);
}

static int mmc_init(int argc, char* argv[])
{
	int ret;
	unsigned long dma_mode;
	unsigned long speed;
	unsigned long bus_width;
	E_SDIO_PORT port = sdio_port;

	if (argc < 4) {
		info("wrong cmd_format: %s dma_mode speed bus_width\n",
			argv[0]);
		return -1;
	}

	dma_mode = get_arg_ulong(argv[1]);
	speed = get_arg_ulong(argv[2]);
	bus_width = get_arg_ulong(argv[3]);

	// pin mux to sdio
	if (!misc_sdio_pin_init(port, bus_width))
	{
		return -1;
	}

	p_mmc_dev->fifo_mode = !dma_mode;
	dw_mmc_init(p_mmc_dev,
		(u32)p_mmc_dev->regs,
		p_mmc_dev->pclk,
		bus_width,speed);

	/*obtain mmc device structure*/
	p_test_mmc = mmc_create((const struct mmc_config *)&p_mmc_dev->cfg,
		p_mmc_dev);
	p_test_mmc->user_speed = speed;
	p_test_mmc->user_bus_width = bus_width;
	p_test_mmc->sys_max_tran_speed = SDMMC_MAX_TRAN_SPEED;

	ret = mmc_init_card(p_test_mmc);
	if (ret) {
		info("card init fail\n");
		return 1;
	}

	/*print card information*/
	if (IS_SD(p_test_mmc)) {
		info("SD card, version: %d.%d\n",
			(p_test_mmc->version>>16)&0xff,
			(p_test_mmc->version>>8)&0xff);
	}
	if (IS_MMC(p_test_mmc)) {
		info("MMC card, version: %d.%d\n",
			(p_test_mmc->version>>16)&0xff,
			(p_test_mmc->version>>8)&0xff);
	}
	info("vendor:%s\n",p_test_mmc->block_dev.vendor);
	info("product:%s\n",p_test_mmc->block_dev.product);
	info("revision:%s\n",p_test_mmc->block_dev.revision);

	info("card high speed mode: %s\n",(p_test_mmc->card_caps & MMC_MODE_HS)?"support":"not support");
	if (IS_MMC(p_test_mmc)) {
		info("card high speed 52M mode: %s\n",(p_test_mmc->card_caps & MMC_MODE_HS_52MHz)?"support":"not support");
	}
	info("card bus width support: 1 %s %s bit\n",
		(p_test_mmc->card_caps & MMC_MODE_4BIT)?"4":"-",
		(p_test_mmc->card_caps & MMC_MODE_8BIT)?"8":"-");

	info("total capacity: %lld M max lba=%d block len=%d\n",
		p_test_mmc->capacity/1024/1024,
		p_test_mmc->block_dev.lba,
		p_test_mmc->read_bl_len);
	info("\n");
	dwmci_display_config(p_mmc_dev);

	return 0;
}

/**
 * @brief mmc_probe - mmc probe card test
 * @param dma_mode input, data transfer using dma or not
 * @param speed input, data transfer speed, max:50M
 * @param bus_width input, data lines used, should be 1-4
 *
 * @return directly if failed
 */
static int mmc_probe(int argc, char* argv[])
{
	int ret;

	ret = mmc_init(argc, argv);

	if (ret<0)
		return 0;

	if (ret) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief mmc_single - mmc single block write/read test
 * @param dma_mode input, data transfer using dma or not
 * @param speed input, data transfer speed, max:50M
 * @param bus_width input, data lines used, should be 1-4
 * @param lba input, start block
 * @param cnt input, block counts
 * @param [pattern] input, r/w random data or fixed data(pattern)
 *
 * @return directly if failed
 */
static int mmc_single(int argc, char* argv[])
{
	int ret;
	unsigned long lba;
	unsigned long cnt;
	int len;
	int i;
	int fail = 0;
	unsigned long pattern;
	int random_pattern = 1;
	int write_retries;
	int read_retries;

	info("%s: mmc single block write/read test\n",argv[0]);

	if (argc < 6) {
		info("wrong cmd_format: %s dma_mode speed bus_width lba cnt [pattern]\n",
			argv[0]);
		return 0;
	}

	ret = mmc_init(argc, argv);
	if (ret)
		return 0;

	lba = get_arg_ulong(argv[4]);
	cnt = get_arg_ulong(argv[5]);
	if (argc >=7) {
		pattern = get_arg_ulong(argv[6]);
		random_pattern = 0;
	}

	info("start single block test, start lba=0x%x(%d) count=0x%x(%d)\n",
		lba,lba,cnt,cnt);
	/* write block then read back to verify, each time 1 block to assure it's single operation */
	while (cnt--) {
		write_retries = 3;
		read_retries = 3;

		/*generate random data*/
		srand(get_ticks());
		len = p_test_mmc->read_bl_len;
		for (i = 0; i < len; i+=4) {
			*(u32*)&test_send_buff[i] = random_pattern ? cb_rand() : pattern;
		}
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
retry_write:
		ret = mmc_bwrite(lba, 1, (char*)test_send_buff);
		if (ret != 1) {
			if (write_retries--) {
				info("mmc_bwrite retry:%d\n", write_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_write;
			}
			fail++;
			break;
		}

retry_read:
		ret = mmc_bread(lba, 1, (char*)test_recv_buff);
		if (ret != 1) {
			if (read_retries--) {
				info("mmc_bread retry:%d\n", read_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_read;
			}
			fail++;
			break;
		}
		/*check data*/
		for (i = 0; i < len; i++) {
			if (test_send_buff[i] != test_recv_buff[i]) {
				debug("verify fail @%d,block=%d\n", i, lba);
				if (write_retries--) {
					ret = mmc_init_card(p_test_mmc);
					if (ret) {
						info("card init fail\n");
						fail++;
						break;
					}
					goto retry_write;
				}
				fail++;
				break;
			}
		}
		if (fail)
			break;
		lba++;
	}

end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief mmc_multi - mmc mutiple block write/read test
 * @param dma_mode input, data transfer using dma or not
 * @param speed input, data transfer speed, max:50M
 * @param bus_width input, data lines used, should be 1-4
 * @param lba input, start block
 * @param cnt input, block counts
 * @param [pattern] input, r/w random data or fixed data(pattern)
 *
 * @return directly if failed
 */
static int mmc_multi(int argc, char* argv[])
{
	int ret;
	unsigned long lba;
	unsigned long cnt;
	int len;
	int i;
	int fail = 0;
	int trans_cnt;
	unsigned long pattern;
	int random_pattern = 1;
	int write_retries;
	int read_retries;

	info("%s: mmc mutiple block write/read test\n",argv[0]);

	if (argc < 6) {
		info("wrong cmd_format: %s dma_mode speed bus_width lba cnt [pattern]\n",
			argv[0]);
		return 0;
	}

	ret = mmc_init(argc, argv);
	if (ret)
		return 0;

	lba = get_arg_ulong(argv[4]);
	cnt = get_arg_ulong(argv[5]);
	if (argc >=7) {
		pattern = get_arg_ulong(argv[6]);
		random_pattern = 0;
	}

	/* write block then read back to verify */
	len = p_test_mmc->read_bl_len;

	while (cnt > 0) {
		write_retries = 3;
		read_retries = 3;
		if (cnt * len > TEST_BUFFER_SIZE)
			trans_cnt = TEST_BUFFER_SIZE / len;
		else
			trans_cnt = cnt;
		/*generate random data*/
		srand(get_ticks());
		for (i = 0; i < (len * trans_cnt); i+=4) {
			*(u32*)&test_send_buff[i] = random_pattern ? cb_rand() : pattern;
		}
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
retry_write:
		ret = mmc_bwrite(lba, trans_cnt, (char*)test_send_buff);
		if (ret != trans_cnt) {
			if (write_retries--) {
				info("mmc_bwrite retry:%d\n", write_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_write;
			}
			fail++;
			break;
		}

retry_read:
		ret = mmc_bread(lba, trans_cnt, (char*)test_recv_buff);
		if (ret != trans_cnt) {
			if (read_retries--) {
				info("mmc_bread retry:%d\n", read_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_read;
			}
			fail++;
			break;
		}
		/*check data*/
		for (i = 0; i < len* trans_cnt; i++) {
			if (test_send_buff[i] != test_recv_buff[i]) {
				debug("verify fail @%d,block=%d\n", i, lba);
				if (write_retries--) {
					ret = mmc_init_card(p_test_mmc);
					if (ret) {
						info("card init fail\n");
						fail++;
						break;
					}
					goto retry_write;
				}
				fail++;
				break;
			}
		}
		if (fail)
			break;
		lba += trans_cnt;
		cnt -= trans_cnt;
	}

end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/**
 * @brief mmc_multi_robust - mmc mutiple block write/read robust test
 * @param dma_mode input, data transfer using dma or not
 * @param speed input, data transfer speed, max:50M
 * @param bus_width input, data lines used, should be 1-4
 * @param lba input, start block
 * @param [pattern] input, r/w random data or fixed data(pattern)
 *
 * @return directly if pressing 'q'
 */
static int mmc_multi_robust(int argc, char* argv[])
{
	int ret;
	unsigned long lba;
	int bl_len;
	int i, loop = 0;
	int fail, test_fail = 0;
	int trans_cnt;
	unsigned long pattern;
	int random_pattern = 1;
	int write_retries;
	int read_retries;

	info("mmc_multi_robust: mmc mutiple robust write/read test, press 'q' to quit\n");

	if (argc < 5) {
		info("wrong cmd_format: %s dma_mode speed bus_width lba [pattern]\n",
			argv[0]);
		return 0;
	}

	ret = mmc_init(argc, argv);
	if (ret)
		return 0;

	lba = get_arg_ulong(argv[4]);
	if (argc >= 6) {
		pattern = get_arg_ulong(argv[5]);
		random_pattern = 0;
	}

	/* write block then read back to verify */
	bl_len = p_test_mmc->read_bl_len;

	while (1) {
		/* press 'q' to quit */
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}

		write_retries = 3;
		read_retries = 3;

		fail = 0;
		srand(get_ticks());

		/* generate random transfer count */
		trans_cnt = cb_rand() % (TEST_BUFFER_SIZE / bl_len);
		if (trans_cnt == 0) {
			trans_cnt = 1;
		}

		if (trans_cnt * bl_len > TEST_BUFFER_SIZE) {
			trans_cnt = TEST_BUFFER_SIZE / bl_len;
		}

		/*Fill send data*/
		srand(get_ticks());
		for (i = 0; i < (bl_len * trans_cnt); i+=4) {
			*(u32*)&test_send_buff[i] = random_pattern ? cb_rand() : pattern;
		}

		/* Clear receive buffer */
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		/* mmc write data */
		info("mmc_bwrite, block count: %d\n", trans_cnt);
retry_write:
		ret = mmc_bwrite(lba, trans_cnt, (char*)test_send_buff);
		if (ret != trans_cnt) {
			if (write_retries--) {
				info("mmc_bwrite retry:%d\n", write_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_write;
			}
			fail++;
			goto failed;
		}

		/* mmc read data */
		info("mmc_bread, block count: %d\n", trans_cnt);
retry_read:
		ret = mmc_bread(lba, trans_cnt, (char*)test_recv_buff);
		if (ret != trans_cnt) {
			if (read_retries--) {
				info("mmc_bread retry:%d\n", read_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_read;
			}
			fail++;
			goto failed;
		}

		/* check data */
		info("check data, block count: %d\n", trans_cnt);
		for (i = 0; i < (bl_len * trans_cnt); i++) {
			if (test_send_buff[i] != test_recv_buff[i]) {
				debug("verify fail @%d,block=%d\n", i, lba);
				if (write_retries--) {
					ret = mmc_init_card(p_test_mmc);
					if (ret) {
						info("card init fail\n");
						fail++;
						break;
					}
					goto retry_write;
				}
				fail++;
				break;
			}
		}
failed:
		if (fail) {
			test_fail++;
		}
		loop++;
		info("mmc_multi_robust: loop %d, trans_cnt=%d, %s, total fail=%d\n\n",
				loop, trans_cnt, (fail ? "fail" : "success"), test_fail);
	}

	info("mmc_multi_robust test terminate, total loop %d failed %d\n", loop, test_fail);
	return 0;
}

#if (CONFIG_DRIVERS_GIC==1)
static int mmc_int_counter = 0;
void mmc_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = dwmci_get_int_status(p_mmc_dev);
	info("mmc_int_handler,status=%x\n", int_status);
	if (int_status & (0x1 << 0))/*Card detect interrupt*/ {
		info("Card detect interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 1))/*Response error interrupt*/ {
		info("Response error interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 2))/*Command done interrupt*/ {
		info("Command done interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 3))/*Data transfer over interrupt*/ {
		info("Data transfer over interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 4))/* Transmit FIFO data request interrupt*/ {
		info("Transmit FIFO data request interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 5))/*Receive FIFO data request interrupt*/ {
		info("Receive FIFO data request interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 6))/*Response CRC error interrupt*/ {
		info("Response CRC error interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 7))/* Data CRC error interrupt*/ {
		info(" Data CRC error interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 8))/*Response timeout interrupt*/ {
		info("Response timeout interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 9))/*Data read timeout interrupt*/ {
		info("Data read timeout interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 10))/* Data starvation by host timeout (HTO)/Volt_switch_int interrupt*/ {
		info(" Data starvation by host timeout (HTO)/Volt_switch_int interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 11))/*FIFO underrun/overrun error interrupt*/ {
		info("FIFO underrun/overrun error interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 12))/* Hardware locked write error interrupt*/ {
		info(" Hardware locked write error interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 13))/*Start Bit Error(SBE)/Busy Clear Interrupt interrupt*/ {
		info("Start Bit Error(SBE)/Busy Clear Interrupt interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 14))/*Auto command done interrupt*/ {
		info("Auto command done interrupt\n");
		mmc_int_counter++;
	}
	if (int_status & (0x1 << 15))/*End-bit error (read)/write no CRC interrupt*/ {
		info("End-bit error (read)/write no CRC interrupt\n");
		mmc_int_counter++;
	}
	if (int_status != 0) {
		dwmci_enable_int(p_mmc_dev,0);
	}
}
#endif

/**
 * @brief usb_int - mmc interrupt test, do single read and generate interrupt
 *
 */
static int mmc_int(int argc, char* argv[])
{
#if (CONFIG_DRIVERS_GIC==1)
	int fail = 0;
	char cc;
	int ret;

	info("%s: interrupt handle test\n",argv[0]);

	dwmci_clear_int_status(p_mmc_dev, 0xffffffff);

	ret = mmc_init(argc, argv);
	if (ret)
		return 0;
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	/*register int handler and enable it*/
	irq_generic_install_handler(p_mmc_dev->int_vector,
		mmc_int_handler,
		NULL);
	irq_generic_config_int(p_mmc_dev->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(p_mmc_dev->int_vector);

	mmc_int_counter = 0;

	/*enable interrupt*/
	dwmci_set_int_mask(p_mmc_dev, 0xffff);
	dwmci_enable_int(p_mmc_dev,1);

	ret = mmc_bread(0, 1, (char*)test_recv_buff);

	if (mmc_int_counter == 0) {
		fail++;
	}

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	/*disable interrupt*/
	irq_generic_disable_int(p_mmc_dev->int_vector);
	irq_generic_remove_handler(p_mmc_dev->int_vector,NULL,NULL);
	dwmci_set_int_mask(p_mmc_dev, 0);
	dwmci_enable_int(p_mmc_dev,0);

#else
	info("mmc_int: interrupt handle test can't be test for no GIC driver support!\n");
	return 0;
#endif

	return 0;
}

/**
 * @brief mmc_read - mmc single block read test
 * @param dma_mode input, data transfer using dma or not
 * @param speed input, data transfer speed, max:50M
 * @param bus_width input, data lines used, should be 1-4
 * @param lba input, start block
 * @param cnt input, block counts
 *
 * @return directly if failed
 */
static int mmc_read(int argc, char* argv[])
{
	int ret;
	unsigned long lba;
	unsigned long cnt;
	int len;
	int i;
	int fail = 0;
	int trans_cnt;
	int off = 0;
	unsigned long trans_lba;
	int read_retries;

	if (argc < 6) {
		info("wrong cmd_format: %s dma_mode speed bus_width lba cnt\n",
			argv[0]);
		return 0;
	}

	ret = mmc_init(argc, argv);
	if (ret)
		return 0;

	lba = get_arg_ulong(argv[4]);
	cnt = get_arg_ulong(argv[5]);

	if (cnt * p_test_mmc->read_bl_len > TEST_BUFFER_SIZE)
		trans_cnt = TEST_BUFFER_SIZE / p_test_mmc->read_bl_len;
	else
		trans_cnt = cnt;

	memset(test_recv_buff, 0, sizeof(test_recv_buff));
	trans_lba = lba;
	while (cnt--) {
		read_retries = 3;
retry_read:
		ret = mmc_bread(trans_lba, 1, (char*)test_recv_buff+off);
		if (ret != 1) {
			if (read_retries--) {
				info("mmc_bread retry:%d\n", read_retries);
				ret = mmc_init_card(p_test_mmc);
				if (ret) {
					info("card init fail\n");
					fail++;
					break;
				}
				goto retry_read;
			}
			fail++;
			break;
		}
		off += p_test_mmc->read_bl_len;
		trans_lba++;
	}
	info("data of block 0x%x(%d): cnt=0x%x(%d)\n",lba,lba,trans_cnt,trans_cnt);
	print_buffer("sd block data",test_recv_buff,off);

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry mmc_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "mmc_probe",mmc_probe, "   : mmc probe card test" },
	{ "mmc_single",mmc_single,  "   :mmc single block write read test" },
	{ "mmc_multi",mmc_multi, " : mmc multi-block write read test" },
	{ "mmc_multi_robust",mmc_multi_robust, " : mmc multi-block robust write read test" },
	{ "mmc_read",mmc_read, " : mmc read and show block" },
	{ "mmc_int",mmc_int, " : mmc interrupt handle test" },{ 0, 0, 0 }
};

int mmc_test_prepare(int sel)
{
	int ret = 0;
	sdio_port = sel;
	memset(p_mmc_dev, 0, sizeof(struct dwmci_host));

	if (sel == 0) 
	{
		dw_mmc_init(p_mmc_dev, SD0_BASE, MMC_PCLK, 1,0);
		p_mmc_dev->int_vector = INT_SDIO0;
	} 
	else if (sel == 1) 
	{
		dw_mmc_init(p_mmc_dev, SD1_BASE, MMC_PCLK, 1,0);
		p_mmc_dev->int_vector = INT_SDIO1;
	} 
	else 
	{
		info("wrong device id\n");
		ret = 1;
	}
	return ret;
}

#define MMC_TMOUT_DEF_VAL	0xFFFFFF40
bool mmc_reset_test(E_SDIO_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_IP_RST ip;
	struct dwmci_host *p_dev = p_mmc_dev;

	info("%s: port:%d\n", __func__, port);
	mmc_test_prepare(port);
	
	info("write tmout reg\n");
	
	// write register
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + DWMCI_TMOUT);

	info("read&compare tmout reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DWMCI_TMOUT);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("mmc reset\n");
	if 		(port == SDIO0)	ip = ARM_SDMMC0_RST;
	else					ip = ARM_SDMMC1_RST;
	prcm_reset(ip);
	
	// read register
	info("read&compare tmout reg\n");
	
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DWMCI_TMOUT);
	if (*p_rd_val != MMC_TMOUT_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool mmc_cg_test(E_SDIO_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk;
	struct dwmci_host *p_dev = p_mmc_dev;

	if 		(port == SDIO0)	blk = ARM_BLK_CG_SDMMC0;
	else					blk = ARM_BLK_CG_SDMMC1;

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	info("%s: port:%d\n", __func__, port);
	mmc_test_prepare(port);
	
	// write register
	info("write tmout reg\n");
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + DWMCI_TMOUT);

	info("read&compare tmout reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DWMCI_TMOUT);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}

	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	info("write tmout reg\n");
	__raw_writel((*p_wr_val+1), (uint32_t)p_dev->regs + DWMCI_TMOUT);	

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare tmout reg\n");	
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DWMCI_TMOUT);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool mmc_reset_test(E_SDIO_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool mmc_cg_test(E_SDIO_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

