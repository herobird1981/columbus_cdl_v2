/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_sha.h"

#if (CONFIG_SHA_TEST == 1)

cmdline_entry sha_test_menu[];
extern struct brite_sha* p_sha_dev;

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&sha_test_menu[0]);
}

void sha_test_init(void)
{
	sha_init(SHA_BASE,0);
	p_sha_dev->dmac_idx = 2;
	p_sha_dev->dma_req_tx = DMA_REQ_SHA_IN;
	p_sha_dev->int_vector = INT_SHA0;
}
static int test_sha384(int argc, char* argv[])
{
	hash_state md;
	unsigned char tmp[48];

	int len;
	int i;
	int fail = 0;
	int ret;

	info("test_sha384: calculate sha384 of buffer\n");

	if (argc < 2) {
		info("wrong cmd_format: %s len\n",argv[0]);
		return 0;
	}
	len = get_arg_ulong(argv[1]);
	if (len > TEST_BUFFER_SIZE) {
		info("length too large,force to %d\n", TEST_BUFFER_SIZE);
		len = TEST_BUFFER_SIZE;
	}
	if (len &0x3) {
		info("length should be mutiple of 4,curent 0x%x\n",len);
		return 0;
	}

	/*generate random data*/
	srand(get_ticks());

	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}
	memset(test_recv_buff,0,128);

	/*hardware calculate*/
	sha_test_init();

	ret = sha384(test_send_buff,len,test_recv_buff);
	if (ret) {
		info("sha384 calculate fail,ret=%d\n",ret);
		fail++;
		goto end;
	}
	/*software calculate*/
	sha384_init(&md);
	sha384_process(&md, test_send_buff, len);
	sha384_done(&md, tmp);

	/*compare the result*/
	for (i=0;i<48;i++) {
		if (tmp[i] != test_recv_buff[i]) {
			info("sha checksum fail @ %d: %x expected %x\n",
				i, test_recv_buff[i],tmp[i]);
			fail++;
			break;
		}
	}

end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

static int test_sha256(int argc, char* argv[])
{
	hash_state md;
	unsigned char tmp[48];

	int len;
	int i;
	int fail = 0;
	int ret;

	info("test_sha256: calculate sha256 of buffer\n");

	if (argc < 2) {
		info("wrong cmd_format: %s len\n",argv[0]);
		return 0;
	}
	len = get_arg_ulong(argv[1]);
	if (len > TEST_BUFFER_SIZE) {
		info("length too large,force to %d\n", TEST_BUFFER_SIZE);
		len = TEST_BUFFER_SIZE;
	}
	if (len &0x3) {
		info("length should be mutiple of 4,curent 0x%x\n",len);
		return 0;
	}

	/*generate random data*/
	srand(get_ticks());

	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}
	memset(test_recv_buff,0,128);

	/*hardware calculate*/
	sha_test_init();

	ret= sha256(test_send_buff,len,test_recv_buff);
	if (ret) {
		info("sha256 calculate fail,ret=%d\n",ret);
		fail++;
		goto end;
	}

	/*software calculate*/
	sha256_init(&md);
	sha256_process(&md, test_send_buff, len);
	sha256_done(&md, tmp);

	/*compare the result*/
	for (i=0;i<32;i++) {
		if (tmp[i] != test_recv_buff[i]) {
			info("sha checksum fail @ %d: %x expected %x\n",
				i, test_recv_buff[i],tmp[i]);
			fail++;
			break;
		}
	}
end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

static int sha_robust(int argc, char* argv[])
{
	unsigned long len;
	int i;
	unsigned short val_out;
	unsigned short soft_val;
	int ret;
	int fail = 0;
	char cc;
	int test_count = 0;
	int test_fail = 0;
	int mode;
	hash_state md;
	unsigned char tmp[48];

	info("sha_robust: sha robust test,press 'q' to quit\n");

	sha_test_init();

	while (1) {
		if (console_tstc()) {
			cc = console_getc();
			if (cc == 'q') {
				break;
			}
		}

		srand(get_ticks());

		mode = cb_rand();

		/*generate random reg,len*/
		len = cb_rand() % TEST_BUFFER_SIZE;
		len = len &0xfffffffc;
		if (len==0)
			len=4;

		/*generate random data*/
		for (i = 0; i < len; i++) {
			test_send_buff[i] = cb_rand() & 0xff;
		}
		memset(test_recv_buff,0,128);

		if (mode &1) {
			ret = sha256(test_send_buff,len,test_recv_buff);
			if (ret) {
				fail++;
			} else {
				sha256_init(&md);
				sha256_process(&md, test_send_buff, len);
				sha256_done(&md, tmp);

				/*compare the result*/
				for (i=0;i<32;i++) {
					if (tmp[i] != test_recv_buff[i]) {
						fail++;
						break;
					}
				}
			}
		} else {
			ret = sha384(test_send_buff,len,test_recv_buff);
			if (ret) {
				fail++;
			}else {
				/*software calculate*/
				sha384_init(&md);
				sha384_process(&md, test_send_buff, len);
				sha384_done(&md, tmp);

				/*compare the result*/
				for (i=0;i<48;i++) {
					if (tmp[i] != test_recv_buff[i]) {
						fail++;
						break;
					}
				}
			}
		}

		test_count++;
		if (fail) {
			test_fail++;
		}
		info("%d loop %s len=%d %s,total failed %d\n",
				test_count,
				(mode &1)?"sha256":"sha384",
				len,fail?"fail":"pass",test_fail);
		fail = 0;
	}

	info("test terminate,total test %d loop, failed %d.\n",
		test_count,test_fail);
	return 0;
}

#if (CONFIG_DRIVERS_GIC==1)
static int sha_int_counter = 0;
void sha_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = sha_get_int_status();
	if (int_status) {
		sha_int_counter++;
		sha_clear_irq();
	}
	info("sha_int_handler,status=%x\n", int_status);
}
#endif

/*testcase:wdt interrupt handle test*/
static int sha_int(int argc, char* argv[])
{
#if (CONFIG_DRIVERS_GIC==1)
	int fail = 0;
	unsigned long load_val;

	info("sha_int: sha interrupt handle test\n");

	/*register int handler and enable it*/
	irq_generic_install_handler(INT_SHA0, sha_int_handler, NULL);
	irq_generic_config_int(INT_SHA0, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_SHA0);

	sha_int_counter = 0;

	sha_test_init();

	sha_enable_irq();

	sha256(test_send_buff,4,test_recv_buff);

	mdelay(3000); /* delay 3 second for interrupt msg output */
	if (sha_int_counter == 0) {
		fail++;
	}

	if (fail) {
		debug("sha_int: test fail\n");
	} else {
		debug("sha_int: test pass\n");
	}

	/*disable interrupt*/
	irq_generic_disable_int(INT_SHA0);
	irq_generic_remove_handler(INT_SHA0,NULL,NULL);
	sha_disable_irq();

#else
	info(
		"sha_int: interrupt handle test can't be test for no GIC driver support!\n");
	return 0;
#endif

	return 0;
}

static int test_sha384_with_key2(int argc, char* argv[])
{
	int fail = 0;
	int ret;
	int flag;
	int i;
	unsigned char tmp[48] = {0};
	unsigned char buf[106] = {
		0x0, /*ca_ver*/
		0x7d, 0x1b, 0xf5, 0xeb, 0x68, 0xc4, 0x82, 0xba, 0xcf, 0x82, 0x87, 0xcc, 0x44, 0x9f, 0x8e, 0x17, 
		0xd3, 0x30, 0x51, 0xf5, 0xd6, 0xdc, 0x21, 0xe0, 0x11, 0xb4, 0xd4, 0x76, 0x54, 0x24, 0x9b, 0x9d, 
		0x42, 0xf0, 0xd1, 0x1c, 0x13, 0x51, 0x2e, 0xeb, 0x9a, 0xed, 0x9e, 0x45, 0x54, 0x16, 0xf4, 0xaa, 
		0xb1, 0xef, 0xfa, 0xfa, 0x32, 0xf7, 0xaf, 0xde, 0x7e, 0xd9, 0x13, 0x25, 0x5e, 0xcf, 0xbf, 0x1b, 
		0x8f, 0x70, 0xb4, 0xa5, 0xd1, 0x88, 0x71, 0x97, 0xe9, 0x1c, 0xe6, 0x9f, 0x74, 0xec, 0xaa, 0x79,
		0xee, 0xe1, 0x76, 0x9b, 0x0d, 0xac, 0x17, 0x65, 0x7e, 0xa9, 0xec, 0x3a, 0x5c, 0x16, 0xaa, 0xca, /*ca_pubkey*/ 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 /*ca_id*/
	};

	info("test_sha384_with_key2\n");

	if (argc < 2) {
		info("wrong cmd_format: %s flag\n",argv[0]);
		info("flag == 0: force sha384 faile; and sha value could be read out\n");
		info("flag == 1: sha384 sucess, could not read out sha result;\n");
		return 0;
	}
	
	flag = get_arg_ulong(argv[1]);
	if(flag == 0)
		buf[0] = ~buf[0];

	/*hardware calculate*/
	sha_test_init();
	ret = sha384(buf,106,tmp); 
	if (ret) {
		info("sha384 calculate fail,ret=%d\n",ret);
		fail++;
		goto end;
	}
	
	//print_hex_dump("sha-value", 1, tmp, sizeof(tmp));

	for(i=0; i<sizeof(tmp); i++)
	{
		if((0 != tmp[i]) && (flag == 1))
		{
			fail++;
			break;
		}
			
	}
end:
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
cmdline_entry sha_test_menu[] = {
	{ "help", cmd_help,	  " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "sha384",test_sha384,  "   : sha384 test case" },
	{ "sha256",test_sha256,  "  : sha256 test case" },
	{ "sha_int",sha_int, 		 "  : sha interrupt test" },
	{ "sha_internal_comp",test_sha384_with_key2, 		 "  : sha internal compare" },	
	{ "sha_robust",sha_robust, 	 "  : sha robust test" },{ 0, 0, 0 }
};

#define SHA_ALG_MODE_DEF_VAL	0x1
bool sha_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
	
	// write register
	info("write alg mode reg\n");
	__raw_writel(*p_wr_val, (SHA_BASE + 0x4));

	// read&compare
	info("read&compare alg mode reg\n");
	*p_rd_val = __raw_readl(SHA_BASE + 0x4);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("sha reset\n");
	prcm_reset(ARM_SHA_RST);
	
	// read register
	info("read&compare alg mode reg\n");
	*p_rd_val = __raw_readl(SHA_BASE + 0x4);
	if (*p_rd_val != SHA_ALG_MODE_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool sha_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_SHA384;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// write register
	info("write alg mode reg\n");
	__raw_writel(*p_wr_val, (SHA_BASE + 0x4));

	// read&compare
	info("read&compare alg mode reg\n");
	*p_rd_val = __raw_readl(SHA_BASE + 0x4);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write alg mode reg\n");
	__raw_writel((*p_wr_val+1), (SHA_BASE + 0x4));

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare alg mode reg\n");
	*p_rd_val = __raw_readl(SHA_BASE + 0x4);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool sha_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool sha_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

