/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_nand.h"

#if (CONFIG_NAND_TEST == 1)
#include "cdl_pll.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
static int nand_int_counter = 0;
static int nand_int_triggered = 0;

cmdline_entry nand_test_menu[];

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
//************************************************************************************************************
// Detect the nand flash part number
//************************************************************************************************************
bool nand_detect(onfi_t *p_onfi)
{
	uint8_t i, buf[20];

	mem_set8(buf, 0x00, sizeof(buf));

	for (i = 0; i < sizeof(p_onfi->dev_model); i++)
	{
		if (p_onfi->dev_model[i] == 0x20)
		{
			break;
		}

		buf[i] = p_onfi->dev_model[i];
	}
	
	info("detect NFC nand flash: %s\n", buf);
	
	return true;
}

//************************************************************************************************************
// NAND initialization
//************************************************************************************************************
static int nand_init(int argc, char* argv[])
{
	uint32_t cs;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	uint32_t data[64];
	E_NFC_MODE mode;
	E_NFC_CLK_DLY clk_delay;
	
	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s cs mode clk_delay\n", argv[0]);
		return 1;
	}

	// Check the validity of cs
	cs = get_arg_ulong(argv[1]);
	if (cs > 0) 
	{
		info("only cs 0 support for current board\n");
		return 1;
	}

	// Check the validity of clk delay
	clk_delay = get_arg_ulong(argv[3]);
	if (clk_delay > NFC_CLK_DLY_3) 
	{
		info("the clk delay should be less than 3\n");
		return 1;
	}

	// initialize the nfc with the default parameter
	nfc_init();


	// Issue a NAND reset command to chip 0
	if (NFC_ST_DONE != nfc_reset())
	{
		return 1;
	}

	// set timing mode 1 to get the right parameter
	if (NFC_ST_DONE != nfc_set_mode(NFC_MODE_1, clk_delay))
	{
		return 1;
	}	

	// Initialize NAND Controller with values from nand_para_page
	if (!nfc_param_init((uint8_t *)data, p_nfc_para))
	{
		return 1;
	}

	nand_detect((onfi_t *)data);
	
	// Check the validity of timing mode
	mode = get_arg_ulong(argv[2]);
	if ((p_nfc_para->timing & (1<<mode)) != (1<<mode)) 
	{
		info("mode %x not supported, NFC timing supported mode: 0x%x\n", mode, p_nfc_para->timing);
		info("bit 5 : 1 = supports timing mode 5\n");
		info("bit 4 : 1 = supports timing mode 4\n");
		info("bit 3 : 1 = supports timing mode 3\n");
		info("bit 2 : 1 = supports timing mode 2\n");
		info("bit 1 : 1 = supports timing mode 1\n");
		info("bit 0 : 1 = supports timing mode 0, shall be 1\n");

		return 1;
	}
	
	// set timing mode
	if (NFC_ST_DONE != nfc_set_mode(mode, clk_delay))
	{
		return 1;
	}

	return 0;
}

//************************************************************************************************************
// Check the validity of the block
//************************************************************************************************************
static bool nand_check_para(uint32_t blk)
{
	nfc_para_t *p_nfc_para = &g_nfc_para;

	if (blk >= p_nfc_para->blks_per_chip)
	{
		info("wrong block 0x%x to erase, max 0x%x\n", blk, (p_nfc_para->blks_per_chip - 1));
		return false;
	}

	return true;
}

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&nand_test_menu[0]);
}

//************************************************************************************************************
// Probe the NAND device
//************************************************************************************************************
static int nand_probe(int argc, char *argv[])
{
	int32_t fail = 0;

	info("%s: nand probe test\n", __func__);

	// init nfc controller
	if (nand_init(argc, argv))
	{
		fail++;
	}

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// Erase a NAND block
//************************************************************************************************************
static int nand_erase(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t i, size;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	
	info("%s: block erase test\n", __func__);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s cs mode clk_delay blk\n", argv[0]);
		return 0;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}

	// Erase the block	
	debug("erase block:0x%x\n", blk);
	if (NFC_ST_DONE != nfc_blk_erase(blk<<p_nfc_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("read and check the block, all value should be 0xFF\n");
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		size = p_nfc_para->page_size + p_nfc_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (NFC_ST_DONE != nfc_raw_data_read(addr, test_recv_buff, size)) 
		{
			info("page 0x%x:read failed\n", page);
			fail++;
			goto end;
		}
		
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != 0xFF) 
			{
				info("page 0x%x check failed at 0x%x:0x%x\n", page, i, test_recv_buff[i]);
				fail++;
				goto end;
			}
		}
	}
	
end:
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// Erase all nand blocks
//************************************************************************************************************
static int nand_erase_all(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t blk;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	
	info("%s: block erase test\n", __func__);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s cs mode clk_delay\n", argv[0]);
		return 0;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	// Erase all blocks	
	for (blk = 0; blk < p_nfc_para->blks_per_chip; blk++)
	{
		info("erase blk 0x%x\n", blk);

		if (NFC_ST_FAIL == nfc_blk_erase(blk<<p_nfc_para->blk_shift_bits)) 
		{
			info("erase fail\n");
			fail++;	
		}
	}
	
end:
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// Write data to each page of a NAND block
//************************************************************************************************************
static int nand_write(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t ecc_mode, pattern;
	uint32_t random_flag = 1;
	uint32_t i, size;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	uint32_t twb, trr, tww;
	
	info("%s: page write test\n", __func__);
	info("cmd_format: %s cs mode clk_delay blk ecc_mode [pattern]\n", argv[0]);
	info("ecc_mode: 0 - raw mode | 1 - ecc mode\n");
	info("[pattern]: if specified, use pattern as test data, else use random data\n");

	// Check the validity of parameter count
	if (argc < 6) 
	{
		info("cmd_format: %s cs mode clk_delay blk ecc_mode [pattern]\n", argv[0]);
		return 1;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}	

	// set the twb, trr, tww for test
	if (argc > 7)
	{
		twb = get_arg_ulong(argv[7]) & 0xFF;
		trr = get_arg_ulong(argv[8]) & 0xFF;
		tww = get_arg_ulong(argv[9]) & 0xFF;
		REG32(NFC_TIME_SEQ_1_REG) = NFC_TIME_SEQ_1_VAL(twb, trr, tww);
		info("twb: 0x%x, trr: 0x%x, tww: 0x%x\n", twb, trr, tww);
	}
	
	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}

	ecc_mode = get_arg_ulong(argv[5]);
	if (ecc_mode > 1)
	{
		info("Invalid ecc_mode value, please use valid value: 0: raw, 1: ecc\n");
		return 1;
	}

	if (argc >= 7) 
	{
		pattern = get_arg_ulong(argv[6]);
		random_flag = 0;
	}

	debug("test block:0x%x write/read page using %s mode \n", blk, (ecc_mode) ? "ecc" : "raw");
	
	// Erase the block
	info("erase block 0x%x\n", blk);
	if (NFC_ST_DONE != nfc_blk_erase(blk<<p_nfc_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("read and check the block, all value should be 0xFF\n");
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		size = p_nfc_para->page_size + p_nfc_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (NFC_ST_DONE != nfc_raw_data_read(addr, test_recv_buff, size)) 
		{
			info("page 0x%x:read failed\n", page);
			fail++;
			goto end;
		}
		
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != 0xFF) 
			{
				info("page 0x%x check failed at 0x%x:0x%x\n", page, i, test_recv_buff[i]);
				fail++;
				goto end;
			}
		}
	}
	
	// read each page of the block to check
	info("write, read and compare...\n");
	
	srand(get_ticks());
	size = (ecc_mode == 1) ?  p_nfc_para->page_size : (p_nfc_para->page_size + p_nfc_para->spare_size);
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		info("write, read and check blk 0x%x, page 0x%x...\n", blk, page);
		
		addr = (blk << p_nfc_para->blk_shift_bits) + page;

		// Set write data
		if (random_flag) 
		{
			for (i = 0; i < size; i++) 
			{
				test_send_buff[i] = cb_rand() & 0xFF;
			}
		} 
		else 
		{
#if 1		
			for (i = 0; i < size; i++) 
			{
				test_send_buff[i] = pattern & 0xFF;
			}
#else
			for (i = 0; i < size; i += 4) 
			{
				test_send_buff[i + 0] = 0xAA;
				test_send_buff[i + 1] = 0xAA;
				test_send_buff[i + 2] = 0x55;
				test_send_buff[i + 3] = 0x55;
			}			
#endif
		}

		if (ecc_mode == 0)
		{
			// write page
			info("write blk 0x%x, page 0x%x\n", blk, page);
			if (NFC_ST_DONE != nfc_raw_data_write(addr, test_send_buff, size)) 
			{
				info("write page 0x%x failed\n", page);
				fail++;
				goto end;
			}			
		
			// read page
			info("read blk 0x%x, page 0x%x\n", blk, page);
			memset(test_recv_buff, 0, sizeof(test_recv_buff));
			if (NFC_ST_DONE != nfc_raw_data_read(addr, test_recv_buff, size)) 
			{
				info("read page 0x%x failed\n", page);
				fail++;
				goto end;
			}
		}
		else
		{
			// write page
			info("write blk 0x%x, page 0x%x\n", blk, page);
			if (NFC_ST_DONE != nfc_page_write(addr, test_send_buff, size)) 
			{
				info("write page 0x%x failed\n", page);
				fail++;
				goto end;
			}			

			// read page
			info("read blk 0x%x, page 0x%x\n", blk, page);
			memset(test_recv_buff, 0, sizeof(test_recv_buff));
			if (NFC_ST_DONE != nfc_page_read(addr, test_recv_buff, size)) 
			{
				info("read page 0x%x failed\n", page);
				fail++;
				goto end;
			}	
		}

		// compare
		info("check blk 0x%x, page 0x%x, size:%x\n", blk, page, size);
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != test_send_buff[i]) 
			{
				info("page 0x%x verify failed at 0x%x, expect 0x%x act 0x%x\n", page, i, test_send_buff[i], test_recv_buff[i]);
				fail++;
				goto end;
			}
		}
	}	

end:
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

static int nand_1wmr(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t ecc_mode, pattern;
	uint32_t random_flag = 1;
	uint32_t i, size;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	uint32_t loop;
	uint8_t ret = 0;
	uint32_t print_flag = 0;
	uint32_t read_retry = 10;
	
	info("%s: page write test\n", __func__);
	info("cmd_format: %s cs mode clk_delay blk ecc_mode loop\n", argv[0]);
	info("ecc_mode: 0 - raw mode | 1 - ecc mode\n");
	info("[pattern]: if specified, use pattern as test data, else use random data\n");

	// Check the validity of parameter count
	if (argc < 7) 
	{
		info("cmd_format: %s cs mode clk_delay blk ecc_mode loop\n", argv[0]);
		return 1;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}	
	
	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}

	ecc_mode = get_arg_ulong(argv[5]);
	if (ecc_mode > 1)
	{
		info("Invalid ecc_mode value, please use valid value: 0: raw, 1: ecc\n");
		return 1;
	}

	loop = get_arg_ulong(argv[6]);

	debug("test block:0x%x write/read page using %s mode \n", blk, (ecc_mode) ? "ecc" : "raw");
	
	// Erase the block
	info("erase block 0x%x\n", blk);
	if (NFC_ST_DONE != nfc_blk_erase(blk<<p_nfc_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("read and check the block erase, all value should be 0xFF\n");
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		size = p_nfc_para->page_size + p_nfc_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (NFC_ST_DONE != nfc_raw_data_read(addr, test_recv_buff, size)) 
		{
			info("page 0x%x:read failed\n", page);
			fail++;
			goto end;
		}
		
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != 0xFF) 
			{
				info("page 0x%x check failed at 0x%x:0x%x\n", page, i, test_recv_buff[i]);
				fail++;
				goto end;
			}
		}
	}
	
	info("write each page with given random data...\n");
	srand(get_ticks());
	size = (ecc_mode == 1) ?  p_nfc_para->page_size : (p_nfc_para->page_size + p_nfc_para->spare_size);
	for (i = 0; i < size; i++) 
		test_send_buff[i] = cb_rand() & 0xFF;	
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		info("write data into blk 0x%x, page 0x%x...\n", blk, page);		
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		for (i = 0; i < size; i++) 
			test_send_buff[4096 + i] = (test_send_buff[i]+0x5a+page)&0xff;
		if (ecc_mode == 0)
			ret = nfc_raw_data_write(addr, test_send_buff+4096, size);
		else
			ret = nfc_page_write(addr, test_send_buff+4096, size);

		if (NFC_ST_DONE != ret) 
		{
			info("write page 0x%x failed\n", page);
			fail++;
			goto end;
		}			
	}

	size = (ecc_mode == 1) ?  p_nfc_para->page_size : (p_nfc_para->page_size + p_nfc_para->spare_size);

	while(loop--)
	{
		for (page = 0; page < p_nfc_para->pages_per_blk; page++)
		{
			info("read data from blk 0x%x, page 0x%x...\n", blk, page);
			
			addr = (blk << p_nfc_para->blk_shift_bits) + page;
			memset(test_recv_buff, 0, sizeof(test_recv_buff));
			info("read blk 0x%x, page 0x%x\n", blk, page);
			if (ecc_mode == 0)
				ret = nfc_raw_data_read(addr, test_recv_buff, size);
			else
				ret = nfc_page_read(addr, test_recv_buff, size); 

			if (NFC_ST_DONE != ret) 
			{
				info("read page 0x%x failed\n", page);
				fail++;
				goto end;
			}

			// compare
			info("check blk 0x%x, page 0x%x, size:%x\n", blk, page, size);
			for (i = 0; i < size; i++) 
				test_send_buff[4096 + i] = (test_send_buff[i]+0x5a+page)&0xff;
			for (i = 0; i < size; i++) 
			{				
				if (test_recv_buff[i] != test_send_buff[4096 + i]) 
				{
					info("page 0x%x verify failed at 0x%x, expect 0x%x act 0x%x\n", page, i, test_send_buff[4096 + i], test_recv_buff[i]);				
					print_flag = 1;
					fail++;
				}
				if(print_flag)	
				{
					info("dump recv buff\r\n");
					mem_print_line((uint8_t *)test_recv_buff, size);
					info("dump send buff\r\n");
					mem_print_line((uint8_t *)(test_send_buff+4096), size);
				}
				print_flag = 0;
			}

			//only retry the 1st failed page
			if(read_retry && fail)
			{
				page--;
				read_retry--;
				if(read_retry == 0)
					goto end;
			}
		}	
	}
end:
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}


//************************************************************************************************************
// Read data from each page of a NAND block
//************************************************************************************************************
static int nand_read(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t size, ecc_mode, data_mode;
	nfc_para_t *p_nfc_para = &g_nfc_para;

	info("%s: page read test\n", __func__);
	info("cmd_format: %s cs mode clk_delay blk ecc_mode data_mode\n", argv[0]);
	info("ecc_mode : 0 - raw mode | 1 - ecc mode\n");
	info("data_mode: 0 - page data | 1 - oob data\n");

	// Check the validity of parameter count
	if (argc < 7) 
	{
		info("cmd_format: %s cs mode clk_delay blk ecc_mode data_mode\n", argv[0]);
		return 1;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}	

	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}

	ecc_mode = get_arg_ulong(argv[5]);
	if (ecc_mode > 1)
	{
		info("Invalid ecc_mode value, please use valid value: 0: raw, 1: ecc\n");
		return 1;
	}
	
	data_mode = get_arg_ulong(argv[6]);
	if (data_mode > 1)
	{
		info("Invalid mode value, please use valid value: 0 - page data, 1 - oob data\n");
		return 1;
	}

	// Read all page data or oob of the block
	debug("read block 0x%x %s...\n", blk, (data_mode) ? "oob" : "page");

	size = (data_mode) ?  p_nfc_para->spare_size : p_nfc_para->page_size;
	
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (data_mode) 
		{
			if (NFC_ST_DONE != nfc_spare_read(addr, test_recv_buff, size)) 
			{
				info("page 0x%x read failed\n", page);
				fail++;
				goto end;
			}
			
			info("block 0x%x, page 0x%x, size 0x%x oob data:\n", blk, page, size);
		} 
		else 
		{
			if (ecc_mode)
			{
				if (NFC_ST_DONE != nfc_page_read(addr, test_recv_buff, size)) 
				{
					info("read page 0x%x failed\n", page);
					fail++;
					goto end;
				}
			}
			else
			{
				if (NFC_ST_DONE != nfc_raw_data_read(addr, test_recv_buff, size)) 
				{
					info("page 0x%x read failed\n", page);
					fail++;
					goto end;
				}
			}
			
			info("block 0x%x, page 0x%x, size 0x%x page data:\n", blk, page, size);
		}
		
		mem_print_line(test_recv_buff, size);
	}

end:
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// NAND interrupt hadler
//************************************************************************************************************
void nand_int_handler(void* ptr)
{
	uint32_t reg_val;
	
	reg_val = nfc_read_int_st();
	if (reg_val != 0) 
	{
		nand_int_counter++;
		nand_int_triggered |= reg_val;
	}
	
	nfc_clr_int_st(reg_val);
	info("nand_int_handler, status=%x\n", reg_val);
}

//************************************************************************************************************
// NAND interrupt test
//************************************************************************************************************
static int nand_int(int argc, char* argv[])
{
	int fail = 0;
	int ret;
	int i;
	uint32_t blk = 0x400;
	uint32_t addr, size;
	nfc_para_t *p_nfc_para = &g_nfc_para;

	info("nand_int: interrupt handle test\n");

	nand_int_counter = 0;
	nand_int_triggered = 0;

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	addr = blk<<p_nfc_para->blk_shift_bits;
	size = p_nfc_para->page_size;

	// register int handler and enable it
	irq_generic_install_handler(INT_NFC, nand_int_handler, NULL);
	irq_generic_config_int(INT_NFC, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_NFC);

	// trigger interrupt via doing read data
	nfc_int_ctrl(CTRL_INT_EN);
	nfc_set_int_mask(0xFFFFFFFF);	
	memset(test_recv_buff, 0, sizeof(test_recv_buff));
	nfc_raw_data_read_1(addr, test_recv_buff, size);
	//mem_print_line(test_recv_buff, size);
	
	mdelay(100);

	if (nand_int_counter == 0) 
	{
		fail++;
	}

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	// disable interrupt
	irq_generic_disable_int(INT_NFC);
	irq_generic_remove_handler(INT_NFC, NULL, NULL);
	
	return 0;
}

//************************************************************************************************************
// NAND ecc bit test
//************************************************************************************************************
static int nand_ecc(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t i, size;
	int32_t ecc_bit, def_ecc_bit;
	nfc_para_t *p_nfc_para = &g_nfc_para;

	info("%s: ecc test\n", __func__);

	// Check the validity of parameter count
	if (argc < 6) 
	{
		info("wrong cmd_format: %s cs mode clk_delay blk ecc_bit\n", argv[0]);
		return 0;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}
	
	def_ecc_bit = p_nfc_para->bch_mode;
	ecc_bit = get_arg_ulong(argv[5]);
	switch (ecc_bit)
	{
		case 2:  p_nfc_para->bch_mode = ECC_CAP_2_BITS; break;
		case 4:  p_nfc_para->bch_mode = ECC_CAP_4_BITS; break;
		case 8:  p_nfc_para->bch_mode = ECC_CAP_8_BITS; break;
		case 16: p_nfc_para->bch_mode = ECC_CAP_16_BITS; break;
		case 24: p_nfc_para->bch_mode = ECC_CAP_24_BITS; break;
		case 32: p_nfc_para->bch_mode = ECC_CAP_32_BITS; break;
		default:
			info("Invalid ecc bit, please use valid value:2, 4, 8, 16, 24, 32\n"); 
			return 1;
	}

	if (p_nfc_para->bch_mode > def_ecc_bit)
	{
		info("ecc bit too large\n");
		return 1;
	}

	info("test block:0x%x ecc bit:0x%x\n", blk, ecc_bit);
	
	// Erase the block
	info("erase block 0x%x\n", blk);
	if (NFC_ST_DONE != nfc_blk_erase(blk<<p_nfc_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// Read each page of the block to check
	info("read and check the block, all value should be 0xFF\n");
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		size = p_nfc_para->page_size + p_nfc_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (NFC_ST_DONE != nfc_raw_data_read(addr, test_recv_buff, size)) 
		{
			info("page 0x%x:read failed\n", page);
			fail++;
			goto end;
		}
		
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != 0xFF) 
			{
				info("page 0x%x check failed at 0x%x:0x%x\n", page, i, test_recv_buff[i]);
				fail++;
				goto end;
			}
		}
	}


	// Write random data, then read back to verify
	srand(get_ticks());
	for (page = 0; page < p_nfc_para->pages_per_blk; page++)
	{
		addr = (blk << p_nfc_para->blk_shift_bits) + page;
		size = p_nfc_para->page_size;
		for (i = 0; i < size; i++) 
		{
			test_send_buff[i] = cb_rand() & 0xFF;
		}
	
		// write page
		info("write blk 0x%x, page 0x%x\n", blk, page);
		if (NFC_ST_DONE != nfc_page_write(addr, test_send_buff, size)) 
		{
			info("write page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		// read page
		info("read blk 0x%x, page 0x%x\n", blk, page);
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
		if (NFC_ST_DONE != nfc_page_read(addr, test_recv_buff, size)) 
		{
			info("read page 0x%x failed\n", page);
			fail++;
			goto end;
		}	

		// compare
		info("check blk 0x%x, page 0x%x, size:%x\n", blk, page, size);
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != test_send_buff[i]) 
			{
				info("page 0x%x verify failed at 0x%x, expect 0x%x act 0x%x\n", page, i, test_send_buff[i], test_recv_buff[i]);
				fail++;
				goto end;
			}
		}

		// read spare area
		size = p_nfc_para->spare_size;
		if (NFC_ST_DONE != nfc_spare_read(addr, test_recv_buff, size)) 
		{
			info("page 0x%x read spare failed\n", page);
			fail++;
			goto end;
		}
		
		info("block 0x%x, page 0x%x, size 0x%x spare data:\n", blk, page, size);
		mem_print_line(test_recv_buff, size);
	}

end:
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// Check a bad block
//************************************************************************************************************
static int nand_check_bb(int argc, char *argv[])
{
	uint32_t ret_val;
	uint32_t blk;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;
	E_BBT_TYPE type;
	
	info("%s: check bad block test\n", __func__);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s cs mode clk_delay blk\n", argv[0]);
		return 0;
	}

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}

	type = nfc_bbt_blk_verify(p_bbt, blk);
	switch (type)
	{
		case BBT_TYPE_BAD	:	info("blk 0x%x is bad\n", blk); 		break;
		case BBT_TYPE_REVD	:	info("blk 0x%x is reserved\n", blk); 	break;
		case BBT_TYPE_WORN	:	info("blk 0x%x is worn\n", blk); 		break;
		case BBT_TYPE_GOOD	:
		default			  	:
								info("blk 0x%x is good\n", blk); 		break;
	}

	return 0;
}

//************************************************************************************************************
// Scan the bad block table
//************************************************************************************************************
static int nand_scan_bbt(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t blk, va, pa;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;
	
	info("%s: scan bbt test\n", __func__);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s cs mode clk_delay\n", argv[0]);
		return 0;
	}

	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	// scan the bbt
	if (NFC_ST_DONE != nfc_bbt_scan(p_bbt))
	{
		fail++;
	}

	mem_print_line((uint8_t *)p_bbt, sizeof(bbt_t));

	// va->pa convert
	for (blk = 0; blk < p_nfc_para->blks_per_chip; blk++)
	{
		va = blk << p_nfc_para->blk_shift_bits;
		nfc_bbt_va_to_pa(p_bbt, va, &pa);
		info("blk:%x, va->pa:%x->%x\n", blk, (va>>p_nfc_para->blk_shift_bits), (pa>>p_nfc_para->blk_shift_bits));
	}	

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}	

	return 0;
}

//************************************************************************************************************
// Mark a block as bad block
//************************************************************************************************************
static int nand_mark_bb(int argc, char* argv[])
{
	uint32_t blk;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;

	info("%s: mark bad block test\n", __func__);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s cs mode clk_delay blk\n", argv[0]);
		return 0;
	}
	
	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}	

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!nand_check_para(blk)) 
	{
		return 1;
	}

	nfc_bbt_blk_marked(p_bbt, blk, BBT_TYPE_BAD);

	info("marked blk 0x%x as bad block in bbt\n", blk);

	return 0;
}

//************************************************************************************************************
// Write the bad block table to the flash
//************************************************************************************************************
static int nand_write_bbt(int argc, char* argv[])
{
	int32_t fail = 0;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;

	info("%s: write bbt test\n", __func__);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s cs mode clk_delay\n", argv[0]);
		return 0;
	}
	
	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	// write the bbt
	if (NFC_ST_DONE != nfc_bbt_write(p_bbt))
	{
		fail++;
	}

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// Read the bad block table from the flash
//************************************************************************************************************
static int nand_read_bbt(int argc, char* argv[])
{
	int32_t fail = 0;
	uint32_t blk, va, pa;
	nfc_para_t *p_nfc_para = &g_nfc_para;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;

	info("%s: read bbt test\n", __func__);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s cs mode clk_delay\n", argv[0]);
		return 0;
	}
	
	// Initialize the nfc controller and device
	if (nand_init(argc, argv))
	{
		return 1;
	}

	// read the bbt
	if (NFC_ST_DONE != nfc_bbt_read(p_bbt))
	{
		fail++;
	}

	mem_print_line((uint8_t *)p_bbt, sizeof(bbt_t));

	// va->pa convert
	for (blk = 0; blk < p_nfc_para->blks_per_chip; blk++)
	{
		va = blk << p_nfc_para->blk_shift_bits;
		nfc_bbt_va_to_pa(p_bbt, va, &pa);
		info("blk:%x, va->pa:%x->%x\n", blk, (va>>p_nfc_para->blk_shift_bits), (pa>>p_nfc_para->blk_shift_bits));
	}

	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry nand_test_menu[] = {
	{"help",				cmd_help,			"              : Display list of commands"},
	{"h", 					cmd_help,	 		"                 : alias for help"},
	{"?", 					cmd_help,	 		"                 : alias for help"},
	{"nand_probe",			nand_probe,			"        : nand probe test"},
	{"nand_erase",			nand_erase,			"        : nand erase block test"},
	{"nand_write",			nand_write,			"        : nand write/read test"},
	{"nand_1wmr",			nand_1wmr,			"        : nand write 1 and read more test"},
	{"nand_read",			nand_read,   		"         : nand read test"},
	{"nand_ecc",			nand_ecc,   		"          : nand ecc support test"},
	{"nand_int",			nand_int,   		"          : nand interrupt test"},
	{"nand_erase_all",		nand_erase_all,		"            : nand erase all blocks test"},
	{"nand_scan_bbt",		nand_scan_bbt,		"     : scan bbt"},
	{"nand_mark_bb",		nand_mark_bb,   	"      : mark bad block"},
	{"nand_check_bb",		nand_check_bb,		"	  : check block's status"},
	{"nand_write_bbt",		nand_write_bbt,		"      : write bbt to flash"},
	{"nand_read_bbt",		nand_read_bbt,		"     : read bbt from flash"},
	{"q",      				NULL,				"                 : quit nand test"},
	{0, 					0, 					0}
};

#define NFC_ST_MASK_DEF_VAL	0x4040
bool nfc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
		
	// write register
	info("write nfc st mask threshold reg\n");
	
	writel(*p_wr_val, NFC_STATUS_MASK_REG);

	// read&compare
	info("read&compare nfc st mask threshold reg\n");
	*p_rd_val = readl(NFC_STATUS_MASK_REG);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("nfc reset\n");
	prcm_reset(ARM_NFC_RST);
	
	// read register
	info("read&compare nfc st mask threshold reg\n");
	
	*p_rd_val = readl(NFC_STATUS_MASK_REG);
	if (*p_rd_val != NFC_ST_MASK_DEF_VAL)
	{
		return false;
	}

	return true;
}

bool nfc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_NFC;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// write register
	info("write nfc st mask threshold reg\n");
	writel(*p_wr_val, NFC_STATUS_MASK_REG);

	// read&compare
	info("read&compare nfc st mask threshold reg\n");
	*p_rd_val = readl(NFC_STATUS_MASK_REG);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write nfc st mask threshold reg\n");
	writel((*p_wr_val + 1), NFC_STATUS_MASK_REG);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare nfc st mask threshold reg\n");
	*p_rd_val = readl(NFC_STATUS_MASK_REG);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}

	return true;
}
#else
bool nfc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool nfc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

