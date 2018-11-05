/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_crc.h"

#if (CONFIG_CRC_TEST == 1)

extern struct brite_crc *p_crc;

cmdline_entry crc_test_menu[];

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&crc_test_menu[0]);
}

static int crc_init(int argc, char* argv[])
{
	if (argc < 2) {
		return 1;
	}
	brite_crc16_init(CRC_BASE);
	p_crc->dmac_idx = 0;
	p_crc->dma_ch_tx = 0;
	p_crc->dma_mode = get_arg_ulong(argv[1]);
	return 0;
}

/**CRC-16. The poly is  0x1021(x^16 + x^12 + x^5 + 1) */
/**
 * @brief crc_xmodem - calculate the CRC-16 for the data buffer in big endian
 * @param dma_mode input, input data with DMA mode or not
 * @param len input, The data length in bytes
 * @param [display_data] input, display the input data or not
 *
 * @return directly if failed
 */
static int crc_xmodem(int argc, char* argv[])
{
	int len;
	int i;
	unsigned short val_out;
	unsigned short soft_val;
	int display_data = 0;

	info("%s: calculate crc16 XMODEM of buffer\n",argv[0]);

	if (argc < 3) {
		info("wrong cmd_format: %s dma_mode len [display_data]\n",argv[0]);
		info("dma_mode:0 fifo mode; 1 dma mode\n");
		return 0;
	}

	len = get_arg_ulong(argv[2]);
	if (len > TEST_BUFFER_SIZE) {
		info("length too large,force to %d\n", TEST_BUFFER_SIZE);
		len = TEST_BUFFER_SIZE;
	}

	if(argc > 3)
		display_data = get_arg_ulong(argv[3]);

	if (crc_init(argc,argv))
		return 0;

	/*generate random data*/
	srand(get_ticks());

	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}

	info("crc16 buffer lenght: %d bytes\n",len);

	if(display_data)
		print_hex_dump("crc", 1, test_send_buff, len);

	val_out = brite_crc16_xmodem(0x0,test_send_buff,len);

	info(" CRC-16/XMODEM  results = 0x%x\n", val_out);

	soft_val = crc16(test_send_buff, len);
	info(" Software verification results:  0x%x\n", soft_val);
	if(soft_val != val_out)
	{
		info("%s : test fail.\n",argv[0]);
	}
	else
	{
		info("%s : test pass.\n",argv[0]);
	}

	return 0;
}

static unsigned int bit_reverse(unsigned int n)
{
	n = ((n >> 1) & 0x5555) | ((n << 1) & 0xaaaa);
	n = ((n >> 2) & 0x3333) | ((n << 2) & 0xcccc);
	n = ((n >> 4) & 0x0f0f) | ((n << 4) & 0xf0f0);
	n = ((n >> 8) & 0x00ff) | ((n << 8) & 0xff00);
	return n;
}

/**
 * @brief crc_multiple - calculate the CRC-16 for the multiple data buffers
 * @param dma_mode input, input data with DMA mode or not
 * @param crc_mode input, The data buffer in big endian or little endian
 * @param [display_data] input, display the input data or not
 *
 * @return directly if failed
 */
static int crc_multiple(int argc, char* argv[])
{
	int len;
	int i;
	unsigned short val_out;
	unsigned short soft_val;
	int off;
	int per_len;
	int display_data = 0;
	int crc_mode = 0;

	info("%s: calculate crc16 of multiple buffer\n",argv[0]);
	if (argc < 3) {
		info("wrong cmd_format: %s dma_mode xmodem/ccitt [display_data]\n",argv[0]);
		info("dma_mode:0 fifo mode; 1 dma mode\n");
		return 0;
	}

	crc_mode = get_arg_ulong(argv[2]);
	if(crc_mode !=0 && crc_mode != 1)
	{
		info("crc_mode is not valid: 0--xmodem, 1--ccitt\n");
		return 0;
	}

	if(argc > 3)
		display_data = get_arg_ulong(argv[3]);

	if (crc_init(argc,argv))
		return 0;

	per_len = 2048;
	len = per_len*5;

	/*generate random data*/
	srand(get_ticks());

	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}

	info("crc16 buffer length: %d bytes\n",len);

	off = 0;
	val_out = 0;
	for (i=0;i<5;i++) {
		if(display_data)
			print_hex_dump("crc", 1, test_send_buff+off, per_len);
		if(crc_mode == 0){
			info("Current CRC calcu mode: Xmodem.\n");
			val_out = brite_crc16_xmodem(val_out,test_send_buff+off,per_len);
		}
		else{
			info("Current CRC calcu mode: CCITT.\n");
			val_out = bit_reverse(val_out);
			val_out = brite_crc16_ccitt(val_out,test_send_buff+off,per_len);
		}
		off += per_len;
		info("%d buffer: len=%d bytes result: %x\n",i,per_len,val_out);
	}
	info(" final CRC-16  = 0x%x\n", val_out);

	if(crc_mode == 0)
		soft_val = crc16(test_send_buff, len);
	else
		soft_val = crc16_kermit(test_send_buff, len);
	info(" Software verification results:  0x%x\n", soft_val);
	if(soft_val != val_out)
	{
		info("%s : test fail.\n",argv[0]);
	}
	else
	{
		info("%s : test pass.\n",argv[0]);
	}

	return 0;
}

/**CRC-16. The poly is  0x1021(x^16 + x^12 + x^5 + 1) */
/**
 * @brief crc_ccitt - calculate the CRC-16 for the data buffer in little endian
 * @param dma_mode input, input data with DMA mode or not
 * @param len input, The data length in bytes
 * @param [display_data] input, display the input data or not
 *
 * @return directly if failed
 */
static int crc_ccitt(int argc, char* argv[])
{
	int len;
	int i;
	unsigned short val_out;
	unsigned short soft_val;
	int display_data = 0;

	info("%s: calculate crc16 CCITT of buffer\n",argv[0]);

	if (argc < 3) {
		info("wrong cmd_format: %s dma_mode len [display_data]\n",argv[0]);
		info("dma_mode:0 fifo mode; 1 dma mode\n");
		return 0;
	}
	len = get_arg_ulong(argv[2]);
	if (len > TEST_BUFFER_SIZE) {
		info("length too large,force to %d\n", TEST_BUFFER_SIZE);
		len = TEST_BUFFER_SIZE;
	}

	if(argc > 3)
		display_data = get_arg_ulong(argv[3]);

	if (crc_init(argc,argv))
		return 0;

	info("crc16 buffer lenght: %d bytes\n",len);

	if(display_data)
		print_hex_dump("crc", 1, test_send_buff, len);

	/*generate random data*/
	srand(get_ticks());

	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}

	val_out = brite_crc16_ccitt(0x0,test_send_buff,len);

	info(" CRC-16/CCITT  = 0x%x\n", val_out);

	soft_val = crc16_kermit(test_send_buff, len);
	info(" Software verification results:  0x%x\n", soft_val);
	if(soft_val != val_out)
	{
		info("%s : test fail.\n",argv[0]);
	}
	else
	{
		info("%s : test pass.\n",argv[0]);
	}

	return 0;
}

/**
 * @brief crc_robust - The CRC-16 robust test
 * @param dma_mode input, input data with DMA mode or not
 *
 * @return directly when pressing 'q'
 */
static int crc_robust(int argc, char* argv[])
{
	int len;
	int i;
	unsigned short val_out;
	unsigned short soft_val;
	int ret;
	int fail = 0;
	char cc;
	int test_count = 0;
	int test_fail = 0;

	info("crc_robust: crc robust test,press 'q' to quit\n");
	if (argc < 2) {
		info("wrong cmd_format: %s dma_mode\n",argv[0]);
		info("dma_mode:0 fifo mode; 1 dma mode\n");
		return 0;
	}
	if (crc_init(argc,argv))
		return 0;

	while (1) {
		if (console_tstc()) {
			cc = console_getc();
			if (cc == 'q') {
				break;
			}
		}

		test_count++;

		srand(get_ticks());

		/*generate random reg,len*/
		len = cb_rand() % TEST_BUFFER_SIZE;
		if (len==0)
			len=4;

		/*generate random data*/
		for (i = 0; i < len; i++) {
			test_send_buff[i] = cb_rand() & 0xff;
		}

		info("%d loop: len=%d",test_count,len);
		info(" XMODEM:");
		val_out = brite_crc16_xmodem(0x0,test_send_buff,len);
		soft_val = crc16(test_send_buff, len);
		/*check result*/
		if (val_out != soft_val) {
			fail++;
			info(" FAIL");
		}else
			info(" PASS");
		info(" CCITT:");
		val_out = brite_crc16_ccitt(0x0,test_send_buff,len);
		soft_val = crc16_kermit(test_send_buff, len);
		/*check result*/
		if (val_out != soft_val) {
			fail++;
			info(" FAIL");
		}else
			info(" PASS");

		if (fail) {
			test_fail++;
		}
		info(" test %s,total failed %d\n",
				fail?"fail":"pass",test_fail);
		fail = 0;
	}

	info("%s test terminate,total %d failed %d\n",argv[0],
		test_count,test_fail);
	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry crc_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "crc_xmodem",crc_xmodem, "  crc16 xmodem cal test" },
	{ "crc_multiple",crc_multiple, " crc16 xmodem cal test using mutiple buffer" },
	{ "crc_ccitt",crc_ccitt, "  crc16 ccitt cal test" },
	{ "crc_robust",crc_robust, " crc16 robust test" },
	{ 0, 0, 0 }
};

#define CRC_OUT_DEF_VAL	0x00
bool crc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
	
	// Set to the default setting
	brite_crc16_init(CRC_BASE);
	
	// reset
	info("uart reset\n");
	prcm_reset(ARM_CRC_RST);
	
	// read register
	info("read&compare crc out threshold reg\n");
	*p_rd_val = readl((uint32_t)p_crc->regs + CRC_OUT);

	if (*p_rd_val != CRC_OUT_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool crc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_CRC;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// Set to the default setting
	brite_crc16_init(CRC_BASE);
	
	// reset
	info("uart reset\n");
	prcm_reset(ARM_CRC_RST);
	
	// read register
	info("read&compare crc out threshold reg\n");
	*p_rd_val = readl((uint32_t)p_crc->regs + CRC_OUT);
	if (*p_rd_val != CRC_OUT_DEF_VAL)
	{
		return false;
	}

	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// dummy read
	*p_rd_val = readl((uint32_t)p_crc->regs + CRC_SEED);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	*p_rd_val = readl((uint32_t)p_crc->regs + CRC_OUT);
	if (*p_rd_val != CRC_OUT_DEF_VAL)
	{
		return false;
	}
		
	return true;
}
#else
bool crc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool crc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

