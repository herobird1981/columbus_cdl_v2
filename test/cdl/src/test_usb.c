/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_usb.h"
#include "cdl_pll.h"
#include "memleak.h"

#if (CONFIG_USB_TEST == 1)

struct musb_controller usb_dev;
struct musb_controller* p_usb = &usb_dev;

cmdline_entry usb_test_menu[];

static unsigned long stor_sel=0; /* 0 using DDR; 1 using MMC0; 2 using MMC1; 3 using NAND */
extern int do_usb(void *cmdtp, int flag, int argc, char * const argv[]);

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&usb_test_menu[0]);
}

#define DEV_MASS_DDR_START	(DDR_BASE)
#define DEV_MASS_DDR_LBA	(50*1024*1024/512) /*50M*/
int dev_mass_read_sector(ulong start, lbaint_t blkcnt, void *buf)
{
	unsigned long len;
	unsigned long off;
	int ret;

	if (stor_sel==0) {
		off = start*512;
		len = 512*blkcnt;
		memcpy(buf,(void*)DEV_MASS_DDR_START+off,len);
	} else if ((stor_sel ==1) || (stor_sel==2)) {
		ret = mmc_bread(start, blkcnt, (char*)buf);
		if (ret != blkcnt) {
			debug("request block cnt not match with read\n");
		}
	} else if (stor_sel ==3) {
	}
	return blkcnt;
}

int dev_mass_write_sector(ulong start, lbaint_t blkcnt, const void *buf)
{
	unsigned long len;
	unsigned long off;
	int ret;

	if (stor_sel==0) {
		off = start*512;
		len = 512*blkcnt;
		memcpy((void*)DEV_MASS_DDR_START+off,buf,len);
	} else if ((stor_sel ==1) || (stor_sel==2)) {
		ret = mmc_bwrite(start, blkcnt, (char*)buf);
		if (ret != blkcnt) {
			debug("request block cnt not match with write\n");
		}
	} else if (stor_sel ==3) {
	}
	return blkcnt;
}

extern struct dwmci_host* p_mmc_dev;
extern struct mmc* p_test_mmc;
int dev_mass_init(void)
{
	int ret=0;

	/*DDR always success*/

	if ((stor_sel ==1)  || (stor_sel ==2)) {
		/*mmc0/1*/
		memset(p_mmc_dev, 0, sizeof(struct dwmci_host));
		if (stor_sel ==1) {
			dw_mmc_init(p_mmc_dev, SD0_BASE, MMC_PCLK, 4,0);
			p_mmc_dev->int_vector = INT_SDIO0;
		} else {
			dw_mmc_init(p_mmc_dev, SD1_BASE, MMC_PCLK, 4,0);
			p_mmc_dev->int_vector = INT_SDIO1;
		}
		p_mmc_dev->fifo_mode =0;
		p_mmc_dev->pclk = MMC_PCLK;
		p_mmc_dev->clock = SDMMC_MAX_TRAN_SPEED;

		/*obtain mmc device structure*/
		p_test_mmc = mmc_create((const struct mmc_config *)&p_mmc_dev->cfg,
			p_mmc_dev);
		p_test_mmc->user_speed = SDMMC_MAX_TRAN_SPEED;
		p_test_mmc->sys_max_tran_speed = SDMMC_MAX_TRAN_SPEED;

		ret = mmc_init_card(p_test_mmc);
		if (ret) {
			info("mmc card init fail\n");
		} else {
			/*print card information*/
			if (IS_SD(p_test_mmc))
				info("SD card detect\n");
			if (IS_MMC(p_test_mmc))
				info("MMC card detect\n");
			info("total capacity: %lld M max lba=%d block len=%d\n",
				p_test_mmc->capacity/1024/1024,
				p_test_mmc->block_dev.lba,
				p_test_mmc->read_bl_len);
		}
	}
	else if (stor_sel ==3) {
		/*nand*/
		info("nand not support now\n");
		return 1;
	}
	return ret;
}
unsigned long dev_mass_get_lba(void)
{
	if (stor_sel ==0)
		return DEV_MASS_DDR_LBA;
	else if ((stor_sel ==1) || (stor_sel == 2))
		return p_test_mmc->block_dev.lba;
	else if (stor_sel ==3)
		return 0;
	return 0;
}
extern int msd_main(void *regs, int irq, int high_speed, int dwSendSrp);

/**
 * @brief ums_test - finish speed test, HNP and SRP test"
 * @param [speed] input, speed choice,  0:full speed 1:high speed
 * @param [srp] input, srp test, B-cable device set to 1, A-cable device set to 0, A running firstly to receive B SRP signal
 *
 * @return directly if pressing "CTRL+C" or "Q"
 */
static int ums_test(int argc, char* argv[])
{
	unsigned long dma_mode;
	E_ARM_IP_RST ip;
	int speed = 1;
	int srp = 1;
	int ret;
	char cc;
	u32 val;

	info("cmd_format: ums_test [speed] [srp]\n");
	info("speed:0 full speed | 1 high speed\n");
	info("srp:SRP test:B-cable device set to 1, A-cable device set to 0, A running firstly to receive B SRP signal.\n");
	info("CTRL+C or Q to quit\n");

	if (argc > 1) {
		speed = get_arg_ulong(argv[1]);
	}
	if (argc > 2) {
		srp = get_arg_ulong(argv[2]);
	}
	if (speed != 0 && speed != 1) {
		info("speed should be 0 or 1\n");
		return 0;
	}
	if (srp != 0 && srp != 1) {
		info("srp should be 0 or 1\n");
		return 0;
	}

	if (p_usb->idx == MUSB_USB0)
		ip = ARM_USBOTG0_RST;
	else
		ip = ARM_USBOTG1_RST;	
	prcm_reset(ip);

#if 0
	/*reset usb controller0 and 1*/
	val = readl(0x20005018);
	writel(val & (~((1<<29)|(1<<30))),0x20005018);
	mdelay(100);
	writel(val | ((1<<29)|(1<<30)),0x20005018);

	/*reset usb phy*/
	val = readl(0x20004300);
	writel(val & ~0x3,0x20004300);
	mdelay(100);
	writel(val | 0x3,0x20004300);
#endif
	msd_main(p_usb->regs, p_usb->int_vector, speed, srp);

	info("ums_test end: please check the media for pass or fail\n");
	return 0;
}

/**
 * @brief usb_dev_mass - controller works as a mass storage disk and PC do read and write test. 
 * @param dma_mode input, usb data transfer using dma or not
 * @param flash_type input, store/read the data to/from ddr|mmc0|mmc1
 *
 * @return directly if failed
 */
static int usb_dev_mass(int argc, char* argv[])
{
	unsigned long dma_mode;
	int ret;
	char cc;

	info("usb_dev_mass\n");
	
	//set pinmux to SDIO for sd card test
	if (!misc_sdio_pin_init(SDIO0, SDIO_BW_8BIT)|| !misc_sdio_pin_init(SDIO1, SDIO_BW_8BIT))
	{
		return 0;
	}

	if (argc < 3) {
		info(
			"wrong cmd_format: %s dma_mode ddr|mmc0|mmc1|nand,'ctrl C' to quit\n",
			argv[0]);
		return 0;
	}
	dma_mode = get_arg_ulong(argv[1]);

	if (!strcmp(argv[2], "ddr"))
		stor_sel = 0;
	else if (!strcmp(argv[2], "mmc0")) {
		stor_sel = 1;
	}
	else if (!strcmp(argv[2], "mmc1"))
		stor_sel = 2;
	else if (!strcmp(argv[2], "nand")) {
		stor_sel = 3;
	}
	else {
		info("unknown storage,ternate test\n");
		return 0;
	}

	ret = dev_mass_init();
	if (ret) {
		info("storage media init fail\n");
		return 0;
	}
	p_usb->dma_mode = (dma_mode)?1:0;
	ret = usb_dev_init(p_usb);
	if (ret) {
		info("usb dev init fail\n");
		return 0;
	}
	/* enable usb gadage mass storage class*/
	/*do usb process*/
	ret = usb_dev_mass_storage();
	if (ret) {
		info("usb process ret %d\n", ret);
	}
	usb_dev_exit();

	info("please check the media for pass or fail\n");
	return 0;
}

/**
 * @brief usb_host_mass - controller works as a mass storage host and do read and write test 
 * between the external u-disk and internal memory.
 *
 * @param dma_mode input, usb data transfer using dma or not
 * @param start input, start block
 * @param cnt input, block counts
 * @param repeat input, r/w repeat times
 * @param [pattern] input, r/w random data or fixed data(pattern)
 *
 * @return directly if failed
 */
static int usb_host_mass(int argc, char* argv[])
{
	int fail = 0;
	int ret;
	int dma_mode;
	int counter = 10;
	int len;
	int blk_start;
	int blk_cnt;
	int i;
	int c;
	u8* rx = test_recv_buff;
	u8* tx = test_send_buff;
	uint32_t pattern = 0;
	uint32_t random_flag = 1;
	int retries;

	info("usb_host_mass: test usb host functions\n");

	if (argc < 5) {
		info(
			"wrong cmd_format: %s dma_mode start cnt repeat [pattern]\n",
			argv[0]);
		return 0;
	}
	if (argc >= 6) {
		pattern = get_arg_ulong(argv[5]);
		random_flag = 0;
	}
	dma_mode = get_arg_ulong(argv[1]);
	blk_start = get_arg_ulong(argv[2]);
	blk_cnt = get_arg_ulong(argv[3]);
	counter = get_arg_ulong(argv[4]);

	p_usb->dma_mode = (dma_mode) ? 1 : 0;
	retries = 3;
retry:
	ret = usb_host_init(p_usb);
	if (ret < 0) {
		fail++;
		goto end;
	}

	ret = usb_host_reset();
	if (ret < 0) {
		fail++;
		goto end;
	}

	len = blk_cnt * 512;
	if (len > TEST_BUFFER_SIZE) {
		blk_cnt = TEST_BUFFER_SIZE / 512;
		len = blk_cnt * 512;
		info("write len too large,force to %d block\n", blk_cnt);
	}

	c = 0;
	while (c < counter) {
		info("usb w/r loop %d: block %d cnt %d\n",c,blk_start,blk_cnt);
		if (random_flag) {
			srand(get_ticks());
			/*generate random data*/
			for (i = 0; i < len; i++) {
				tx[i] = cb_rand() & 0xff;
			}
		} else
			memset(tx, pattern & 0xff, len);

		memset(rx, 0, len);

		ret = usb_host_write(tx,blk_start,blk_cnt);
		if (ret) {
			fail++;
			goto end;
		}

		ret = usb_host_read(rx,blk_start,blk_cnt);
		if (ret) {
			fail++;
			goto end;
		}

		/*check result*/
		for (i = 0; i < len; i++) {
			if (tx[i] != rx[i]) {
				debug(
					"usb check data fail @%d: expected %x receive %x\n",
					i,
					tx[i],
					rx[i]);
				fail++;
				break;
			}
		}
		c++;
	}

end:
	usb_host_stop();
	usb_host_exit();

	if (fail) {
		info("%s: test fail\n",argv[0]);
		if (retries--) {
			info("usb_host_mass retry:%d\n", retries);
			fail = 0;
			goto retry;
		}
	} else {
		info("%s: test pass\n",argv[0]);
	}
	dbg_mem_stat();
	return 0;
}

/**
 * @brief usb_host_mass_fix - controller works as a mass storage host and do read and write test with fixed data 
 * between the external u-disk and internal memory.
 *
 * @param dma_mode input, usb data transfer using dma or not
 * @param start input, start block
 * @param cnt input, block counts
 *
 * @return directly if failed
 */
static int usb_host_mass_fix(int argc, char* argv[])
{
	int fail = 0;
	int ret;
	int dma_mode;
	int counter = 1;
	int len;
	int blk_start;
	int blk_cnt;
	int i;
	int c;
	u8* rx = test_recv_buff;
	u8* tx = test_send_buff;

	info("usb_host_mass: write fix value(0-255) to block\n");

	if (argc < 4) {
		info(
			"wrong cmd_format: %s dma_mode start cnt\n",
			argv[0]);
		return 0;
	}
	dma_mode = get_arg_ulong(argv[1]);
	blk_start = get_arg_ulong(argv[2]);
	blk_cnt = get_arg_ulong(argv[3]);
	counter = 1;

	p_usb->dma_mode = (dma_mode)?1:0;
	ret = usb_host_init(p_usb);
	if (ret < 0) {
		fail++;
		goto end;
	}

	ret = usb_host_reset();
	if (ret < 0) {
		fail++;
		goto end;
	}

	len = blk_cnt*512;
	if (len >TEST_BUFFER_SIZE) {
		blk_cnt = TEST_BUFFER_SIZE /512;
		len = blk_cnt*512;
		info("write len too large,force to %d block\n",blk_cnt);
	}

	info("usb w/r loop %d: block %d cnt %d\n",c,blk_start,blk_cnt);
	/*generate random data*/
	for (i = 0; i < len; i++) {
		tx[i] = i & 0xff;
	}
	memset(rx, 0, len);

	ret = usb_host_write(tx,blk_start,blk_cnt);
	if (ret) {
		fail++;
		goto end;
	}

	ret = usb_host_read(rx,blk_start,blk_cnt);
	if (ret) {
		fail++;
		goto end;
	}

	/*check result*/
	for (i = 0; i < len; i++) {
		if (tx[i] != rx[i]) {
			debug(
				"usb check data fail @%d: expected %x receive %x\n",
				i,
				tx[i],
				rx[i]);
			fail++;
			break;
		}
	}

end:
	usb_host_stop();
	usb_host_exit();

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}
#if (CONFIG_DRIVERS_GIC==1)
static int usb_int_counter = 0;
static int aux_int_counter = 0;

void usb_int_handler(void* ptr)
{
	int int_status = 0;
#ifdef TAPEOUT_V2
	u32 aux_int_status = 0;
	u32 aux_int_mask = 0;
#endif

	int_status = usb_get_int_status(p_usb);
#ifndef TAPEOUT_V2
	info("\nusb_int_handler int_status:%x\n", int_status);
#else
	aux_int_status = usb_aux_int_status_get(p_usb);
	aux_int_mask = usb_aux_int_mask_get(p_usb);
	usb_aux_int_status_clear(p_usb, aux_int_status);
	info("\nusb_int_handler int_status:%x, aux_status:0x%x, aux_int_mask:0x%x\n",
			int_status, aux_int_status, aux_int_mask);
#endif
	if (int_status & (0x1 << 0))/*suspend interrupt*/ {
		info("suspend interrupt, auto enter if without activity > 3ms\n");
		usb_int_counter++;
	}
	if (int_status & (0x1 << 1))/*resume interrupt*/ {
		info("resume interrupt\n");
		usb_int_counter++;
	}
	if (int_status & (0x1 << 2))/*reset interrupt, all mask will be default*/ {
		info("reset interrupt\n");
		__raw_writew(0x0,(u32)p_usb->regs+0x6);
		__raw_writew(0x0,(u32)p_usb->regs+0x8);
		usb_enable_int(p_usb,0xf7);
		usb_int_counter++;
	}
	if (int_status & (0x1 << 4))/*connect interrupt*/ {
		info("connect interrupt\n");
		usb_int_counter++;
	}
	if (int_status & (0x1 << 5))/*disconnect interrupt*/ {
		info("disconnect interrupt\n");
		usb_int_counter++;
	}
	if (int_status & (0x1 << 6))/*session request interrupt*/ {
		info("session request interrupt\n");
		usb_int_counter++;
	}
	if (int_status & (0x1 << 7))/*vbus error interrupt*/ {
		info("vbus error interrupt\n");
		usb_int_counter++;
	}
#ifdef TAPEOUT_V2
	if (aux_int_status & (0x1 << 0))/*ID posedge interrupt*/ {
		aux_int_counter++;
		info("aux ID posedge interrupt %d\n", aux_int_counter);
	}

	if (aux_int_status & (0x1 << 1))/*ID negedge interrupt*/ {
		aux_int_counter++;
		info("aux ID negedge interrupt %d\n", aux_int_counter);
	}
#endif
	if (usb_int_counter > 20) /*disable interrupt*/
		usb_enable_int(p_usb,0);
}
#endif

/**
 * @brief usb_int - usb interrupt test, USB id detect interrupt and USB controller interrupt
 *
 */
static int usb_int(int argc, char* argv[])
{
#if (CONFIG_DRIVERS_GIC==1)
	int fail = 0;
	char cc;
	int ret;
	u32 deb_cnt = 0;

	info(
		"usb_int: interrupt handle test,input 'q' to quit this testcase\n");

#ifdef TAPEOUT_V2
	info("usage: usb_int [deb_cnt: 0~0xFFFFFFFF]\n\n");

	if (argc > 1) {
		deb_cnt = get_arg_ulong(argv[1]);
		if (deb_cnt > 0xFFFFFFFF) {
			info("deb_cnt too large, force to max: 0xFFFFFFFF\n");
			deb_cnt = 0xFFFFFFFF;
		}
	}
#endif

	/*register int handler and enable it*/
	irq_generic_install_handler(p_usb->int_vector,
		usb_int_handler,
		NULL);
	irq_generic_config_int(p_usb->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(p_usb->int_vector);

	stor_sel = 0;
	usb_int_counter = 0;
	aux_int_counter = 0;
	ret = dev_mass_init();
	if (ret) {
		info("storage media init fail\n");
		return 0;
	}
	p_usb->dma_mode = 0;
	ret = usb_dev_init(p_usb);
	if (ret) {
		info("usb dev init fail\n");
		return 0;
	}

	/*enable controller interrupt*/
	usb_enable_int(p_usb,0xf7);
	__raw_writew(0x0,(u32)p_usb->regs+0x6);
	__raw_writew(0x0,(u32)p_usb->regs+0x8);

#ifdef TAPEOUT_V2
	/*enable USB Auxilary interrupt */
	usb_aux_vdt_enable(p_usb->aux_regs, p_usb->idx);
	usb_aux_int_enable(p_usb, AUX_INT_ID_POSEDGE | AUX_INT_ID_NEGEDGE);
	info("get id debounce now:0x%x\n", usb_aux_id_debounce_get(p_usb->aux_regs, p_usb->idx));

	if (argc > 1) {
		usb_aux_id_debounce_set(p_usb->aux_regs, p_usb->idx, deb_cnt);
		info("set id debounce:0x%x, get:0x%x\n", deb_cnt, usb_aux_id_debounce_get(p_usb->aux_regs, p_usb->idx));
	}
#endif
	/*session stop and start*/
	__raw_writeb(0x98, (u32)p_usb->regs+0x60);
	__raw_writeb(0x99, (u32)p_usb->regs+0x60);

	/*soft disconnect and connect*/
	__raw_writeb(0x00, (u32)p_usb->regs+1);
	__raw_writeb(0x40, (u32)p_usb->regs+1);

	/*wait interrupt handle*/
	mdelay(5000);

	if (usb_int_counter == 0) {
		fail++;
	}
	info("total usb_int_counter:%d, aux_int_counter:%d\n",usb_int_counter, aux_int_counter);
	if (fail) {
		debug("usb_int: test fail\n");
	} else {
		debug("usb_int: test pass\n");
	}
#ifdef TAPEOUT_V2
	/*auxilary interrupt disable*/
	usb_aux_int_disable(p_usb, AUX_INT_ID_POSEDGE | AUX_INT_ID_NEGEDGE);
	usb_aux_vdt_disable(p_usb->aux_regs, p_usb->idx);
#endif
	/*disable interrupt*/
	irq_generic_disable_int(p_usb->int_vector);
	irq_generic_remove_handler(p_usb->int_vector,NULL,NULL);
	usb_enable_int(p_usb,0);
	usb_dev_exit();

#else
	info(
		"usb_int: interrupt handle test can't be test for no GIC driver support!\n");
	return 0;
#endif
	dbg_mem_stat();

	return 0;
}

static int usb(int argc, char* argv[])
{
	unsigned long dma_mode;
	char cc;
	int ret;

	info("usb host test for debug\n");

	if (!strcmp(argv[1],"start")) {
		usb_host_init(p_usb);
	}
	do_usb(NULL,0,argc,argv);
	usb_host_exit();
	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry usb_test_menu[] = {
	{ "help", 	cmd_help,  " : Display list of commands" },
	{ "h",		cmd_help, 	"    : alias for help" },
	{ "?",		cmd_help, 	"    : alias for help" },
	{ "ums_test",ums_test, 	 "   :simple usb mass storage r/w test;speed test;HNP and SRP test;cable decide who is host" },
	{ "usb_dev_mass",usb_dev_mass, 	 "   :usb dev mass storage test" },
	{ "usb_host_mass",usb_host_mass, "  : usb host mass storage test" },
	{ "usb_host_mass_fix",usb_host_mass_fix, "  : usb host mass storage test" },
	{ "usb",usb, "  : usb host debug" },
	{ "usb_int",usb_int, "  : usb interrupt test" },{ 0, 0, 0 }
};

/* if return none zero, something wrong,please check and can't continue test*/
int usb_test_prepare(int sel)
{
	int ret = 0;

	memset(p_usb, 0, sizeof(struct musb_controller));

	if (sel == 0) 
	{
		p_usb->regs = (void*)USB0_BASE;
		p_usb->aux_regs = (void*)MISC_BASE;
		p_usb->int_vector = INT_USB0;
		p_usb->int_dma_vector = INT_USB0_DMA;
		p_usb->idx = MUSB_USB0;
		info("%s: USB0_BASE=0x%x, INT_USB0=%d, INT_USB0_DMA=%d\n", __func__, USB0_BASE, INT_USB0, INT_USB0_DMA);
	} 
	else if (sel == 1) 
	{
		p_usb->regs = (void*)USB1_BASE;
		p_usb->aux_regs = (void*)MISC_BASE;
		p_usb->int_vector = INT_USB1;
		p_usb->int_dma_vector = INT_USB1_DMA;
		p_usb->idx = MUSB_USB1;
		info("%s: USB1_BASE=0x%x, INT_USB1=%d, INT_USB1_DMA=%d\n", __func__, USB1_BASE, INT_USB1, INT_USB1_DMA);
	} 
	else 
	{
		info("wrong device id\n");
		ret = 1;
	}

	return ret;
}


#define USB_POWER_DEF_VAL	0x00
bool usb_reset_test(E_USB_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_IP_RST ip;
	struct musb_controller *priv = p_usb;

	info("%s: port:%d\n", __func__, port);
	usb_test_prepare(port);	

	// write register
	info("write usb power reg %x\n", priv->regs);
	__raw_writeb(*p_wr_val, (uint32_t)priv->regs + MUSB_FADDR);

	// read&compare
	info("read&compare usb power reg\n");
	*p_rd_val = __raw_readb((uint32_t)priv->regs + MUSB_FADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("usb reset\n");
	if      (port == USB0) 	ip = ARM_USBOTG0_RST;
	else					ip = ARM_USBOTG1_RST;	
	prcm_reset(ip);
	
	// read register
	info("read&compare usb power reg\n");
	*p_rd_val = __raw_readb((uint32_t)priv->regs + MUSB_FADDR);
	if (*p_rd_val != USB_POWER_DEF_VAL)
	{
		return false;
	}	

	return true;
}

bool usb_cg_test(E_USB_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk;
	struct musb_controller *priv = p_usb;

	info("%s: port:%d\n", __func__, port);
	
	if      (port == USB0) 	blk = ARM_BLK_CG_USBOTG0;
	else					blk = ARM_BLK_CG_USBOTG1;

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	usb_test_prepare(port);	

	// write register
	info("write usb power reg %x\n", priv->regs);
	__raw_writeb(*p_wr_val, (uint32_t)priv->regs + MUSB_FADDR);

	// read&compare
	info("read&compare usb power reg\n");
	*p_rd_val = __raw_readb((uint32_t)priv->regs + MUSB_FADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write usb power reg %x\n", priv->regs);
	__raw_writeb((*p_wr_val+1), (uint32_t)priv->regs + MUSB_FADDR);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare usb power reg\n");
	*p_rd_val = __raw_readb((uint32_t)priv->regs + MUSB_FADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}	

	return true;
}
#else
bool usb_reset_test(E_USB_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool usb_cg_test(E_USB_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

