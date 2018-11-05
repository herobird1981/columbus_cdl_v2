/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_qspi.h"
#include "cdl_pll.h"

#if (CONFIG_QSPI_TEST == 1)
#include "spi_flash.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
cmdline_entry qspi_test_menu[];

static int qspi_int_counter = 0;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
//************************************************************************************************************
// Check the validity of the block
//************************************************************************************************************
static bool qspi_check_para(uint32_t blk)
{
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;

	if (blk >= p_qspi_para->blks_per_chip)
	{
		info("wrong block 0x%x to erase, max 0x%x\n", blk, (p_qspi_para->blks_per_chip - 1));
		return false;
	}

	return true;
}

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&qspi_test_menu[0]);
}

//************************************************************************************************************
// Detect the qspi nand flash part number
//************************************************************************************************************
bool qspi_detect(onfi_t *p_onfi)
{
	uint8_t i, buf[20];
	uint8_t id[4];
	
	mem_set8(buf, 0x00, sizeof(buf));
	switch (p_onfi->manu_id)
	{
		case QSPI_MANU_ID_MXIC:
		case QSPI_MANU_ID_WINBOND:
		case QSPI_MANU_ID_MICRON:
				for (i = 0; i < sizeof(p_onfi->dev_model); i++)
				{
					if (p_onfi->dev_model[i] == 0x20)
					{
						break;
					}

					buf[i] = p_onfi->dev_model[i];
				}
				
				info("detect qspi flash: %s\n", buf);
			break;
		default:
				info("unknow or wrong flash id %x detected,skip other test\n", p_onfi->manu_id);
			return false;
	}	

	if (p_onfi->manu_id == QSPI_MANU_ID_MICRON)
	{
		g_qspi_nand_oob_ofst[0] = g_qspi_nand_para.page_size + 0x20;
		g_qspi_nand_oob_ofst[1] = g_qspi_nand_para.page_size + 0x24;
	}
	else
	{
		g_qspi_nand_oob_ofst[0] = g_qspi_nand_para.page_size + 0x4;
		g_qspi_nand_oob_ofst[1] = g_qspi_nand_para.page_size + 0x14;
	}

	if (p_onfi->manu_id == QSPI_MANU_ID_WINBOND)
	{
		qspi_nand_buf_mode_ctrl(QSPI_BUF_MODE_1);

		
		qspi_nand_read_id(id, 4);
		info("id:%x, %x, %x, %x\n", id[0], id[1], id[2], id[3]);

		if ((id[0] == 0xEF) && (id[1] == 0xAB) && (id[2] == 0x21))
		{
			g_qspi_nand_para.blks_per_chip <<= 1; 
			info("force blks_per_chip: %x\n", g_qspi_nand_para.blks_per_chip);
		}
	}
		
	return true;
}

//************************************************************************************************************
// QSPI NAND initialization
//************************************************************************************************************
static int qspi_init(int argc, char *argv[])
{
	uint32_t cs, clk;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint32_t data[64];
	uint8_t br_div, fea = 0;
	E_QSPI_PHASE phase;
	E_QSPI_ST st;

	info("%s: qspi init\n", __func__);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s cs clk(HZ) phase\n", argv[0]);
		return 1;
	}

	// Get the parameters
	cs = get_arg_ulong(argv[1]);
	if (cs > 1) 
	{
		info("cs should be 0/1; force set to 0 for invalid range\n");
		cs = 0;
	}

	clk = get_arg_ulong(argv[2]);
	phase = get_arg_ulong(argv[3]);
	if ((phase != QSPI_PHASE_0) && (phase != QSPI_PHASE_3))
	{
		info("qspi only support phase 0/3\n");
		//return 1;
	}	

	info("input cmd_format: %s cs(%d) clk(%dHZ) phase(%d)\n", argv[0], cs, clk, phase);
	
	// set pinmux
	if (!misc_spi_pin_init(SPI0))
	{
		return 1;
	}

	// set qspi para
	p_qspi_para->pclk = QSPI_PCLK;
	p_qspi_para->max_hz = 50000000;
	p_qspi_para->tshsl_ns = 50;
	p_qspi_para->tsd2d_ns = 50;
	p_qspi_para->tchsh_ns = 5;
	p_qspi_para->tslch_ns = 5;	

	// set qspi para
	br_div = qspi_get_brdiv(QSPI_PCLK, clk);
	qspi_nand_init(br_div, phase, cs);
	if (QSPI_ST_DONE != qspi_calibration(clk))
	{
		info("nor calibration fail\n");	
		return 1;		
	}

	// Issue a QSPI reset command
	if (QSPI_ST_DONE != qspi_nand_reset())
	{
		return 1;
	}

	// Initialize the QSPI with values from qspi_nand_para_page
	if (!qspi_nand_ONFI_param_init((uint8_t *)data, p_qspi_para))
	{
		return 1;
	}

	qspi_detect((onfi_t *)data);

	if (p_qspi_para->manu_id == QSPI_MANU_ID_WINBOND)
	{
		// select die
		qspi_nand_sel_die(0 << p_qspi_para->blk_shift_bits);

		// unlock block protect
		qspi_nand_blk_protect_ctrl(BP_ALL_UNLOCKED);

		// select die
		qspi_nand_sel_die(p_qspi_para->blks_per_lun << p_qspi_para->blk_shift_bits);

		// unlock block protect
		qspi_nand_blk_protect_ctrl(BP_ALL_UNLOCKED);
	}
	else
	{
		// unlock block protect
		qspi_nand_blk_protect_ctrl(BP_ALL_UNLOCKED);
	}
	
	// quad enable
	qspi_nand_quad_enable();

	qspi_nand_get_feature(QSPI_NAND_OTP_FA, &fea);
	debug("otp fea: 0x%x\n", fea);

	qspi_nand_get_feature(QSPI_NAND_BLK_PROT_FA, &fea);
	debug("blk fea: 0x%x\n", fea);

	// default write and read mode to be 4 bits mode
	p_qspi_para->io_rmode = QIO_MODE;
	p_qspi_para->io_wmode = QIO_MODE;
	
	return 0;
}

//************************************************************************************************************
// Probe the NAND device
//************************************************************************************************************
static int qspi_probe(int argc, char *argv[])
{
	int32_t fail = 0;
	info("%s: qspi probe test\n", __func__);
	
	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		fail++;
	}

	// init qspi controller
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
// Erase a QSPI NAND block
//************************************************************************************************************
static int qspi_erase(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t i, size;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;

	info("%s: qspi block erase test\n", __func__);
	
	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s cs clk(HZ) phase blk\n", argv[0]);
		return 1;
	}

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk)) 
	{
		return 1;
	}	

	// Erase the block	
	info("erase block:0x%x\n", blk);
	if (QSPI_ST_DONE != qspi_nand_blk_erase(blk<<p_qspi_para->blk_shift_bits))
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("read and check the block, all value should be 0xFF\n");
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_qspi_para->blk_shift_bits) + page;
		size = p_qspi_para->page_size + p_qspi_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr, test_recv_buff, size)) 
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
// Erase all QSPI nand blocks
//************************************************************************************************************
static int qspi_erase_all(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t i, size;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;

	info("%s: qspi block erase all test\n", __func__);

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}
	
	// Erase all blocks	
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		info("erase blk 0x%x\n", blk);

		if (QSPI_ST_DONE != qspi_nand_blk_erase(blk<<p_qspi_para->blk_shift_bits))
		{
			info("erase timeout\n");
			fail++;
			goto end;
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
// Write data to each page of a QSPI NAND block
//************************************************************************************************************
static int qspi_write(int argc, char* argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t ecc_mode, pattern;
	uint32_t random_flag = 1;
	uint32_t i, size;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	
	info("%s: qspi page write test\n", __func__);
	
 	info("cmd_format: %s cs clk(HZ) phase blk ecc_mode [pattern]\n", argv[0]);
	info("ecc_mode: 0 - raw mode | 1 - ecc mode\n");
	info("[pattern]: if specified, use pattern as test data, else use random data\n");

	// Check the validity of parameter count
	if (argc < 6) 
	{
 		info("wrong cmd_format: %s cs clk(HZ) phase blk ecc_mode [pattern]\n", argv[0]);
		return 0;
	}

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}	

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk))
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

	info("test block:0x%x write/read page using %s mode \n", blk, (ecc_mode) ? "ecc" : "raw");

	// Erase the block
	info("erase block 0x%x\n", blk);
	if (QSPI_ST_DONE != qspi_nand_blk_erase(blk<<p_qspi_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("read and check the block, all value should be 0xFF\n");
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_qspi_para->blk_shift_bits) + page;
		size = p_qspi_para->page_size + p_qspi_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr, test_recv_buff, size)) 
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
	size = (ecc_mode == 1) ?  p_qspi_para->page_size : (p_qspi_para->page_size + p_qspi_para->spare_size);
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("write, read and check blk 0x%x, page 0x%x...\n", blk, page);
		
		addr = (blk << p_qspi_para->blk_shift_bits) + page;

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
			for (i = 0; i < size; i++) 
			{
				test_send_buff[i] = pattern & 0xFF;
			}
		}
		
		if (ecc_mode == 0)
		{
			// write page
			info("write blk 0x%x, page 0x%x\n", blk, page);
			if (QSPI_ST_DONE != qspi_nand_raw_data_write(addr, test_send_buff, size)) 
			{
				info("write page 0x%x failed\n", page);
				fail++;
				goto end;
			}	
		
			// read page
			info("read blk 0x%x, page 0x%x\n", blk, page);
			memset(test_recv_buff, 0, sizeof(test_recv_buff));
			if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr, test_recv_buff, size)) 
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
			if (QSPI_ST_DONE != qspi_nand_page_write(addr, test_send_buff, size)) 
			{
				info("write page 0x%x failed\n", page);
				fail++;
				goto end;
			}			

			// read page
			info("read blk 0x%x, page 0x%x\n", blk, page);
			memset(test_recv_buff, 0, sizeof(test_recv_buff));
			if (QSPI_ST_DONE != qspi_nand_page_read(addr, test_recv_buff, size)) 
			{
				info("read page 0x%x failed\n", page);
				fail++;
				goto end;
			}	
		}

		// compare
		info("check blk 0x%x, page 0x%x, size:0x%x\n", blk, page, size);
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

//************************************************************************************************************
// Read data from each page of a QSPI NAND block
//************************************************************************************************************
static int qspi_read(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t size, ecc_mode, data_mode;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;

	info("%s: qspi page read test\n", __func__);
	
	info("cmd_format: %s cs clk(HZ) phase blk ecc_mode data_mode\n", argv[0]);
	info("ecc_mode: 0 - raw mode | 1 - ecc mode\n");
	info("data_mode: 0 - page data | 1 - oob data\n");

	// Check the validity of parameter count
	if (argc < 7) 
	{
		info("cmd_format: %s cs clk(HZ) phase blk ecc_mode data_mode\n", argv[0]);
		return 1;
	}

	// Initialize the nfc controller and device
	if (qspi_init(argc, argv))
	{
		return 1;
	}	

	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk)) 
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
	info("read block 0x%x %s...\n", blk, (data_mode) ? "oob" : "page");

	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		addr = (blk << p_qspi_para->blk_shift_bits) + page;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (data_mode) 
		{
			size = p_qspi_para->spare_size;
			if (QSPI_ST_DONE != qspi_nand_spare_read(addr, test_recv_buff, size)) 
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
				size = p_qspi_para->page_size;
				if (QSPI_ST_DONE != qspi_nand_page_read(addr, test_recv_buff, size)) 
				{
					info("read page 0x%x failed\n", page);
					fail++;
					goto end;
				}

				info("block 0x%x, page 0x%x, size 0x%x page data(ecc mode):\n", blk, page, size);
			}
			else
			{
				size = p_qspi_para->page_size + p_qspi_para->spare_size;
				if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr, test_recv_buff, size)) 
				{
					info("page 0x%x read failed\n", page);
					fail++;
					goto end;
				}

				info("block 0x%x, page 0x%x, size 0x%x page data(raw mode):\n", blk, page, size);
			}
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
// QSPI NAND interrupt hadler
//************************************************************************************************************
void qspi_int_handler(void *ptr)
{
	uint32_t reg_val;
	
	reg_val = qspi_read_int_st();
	if (reg_val != 0) 
	{
		qspi_int_counter++;
	}
	
	info("qspi_int_handler,status=%x\n", reg_val);
	qspi_clr_int_st(reg_val);
}

//************************************************************************************************************
// QSPI NAND interrupt test
//************************************************************************************************************
static int qspi_int(int argc, char* argv[])
{
	int fail = 0;
	int32_t ret_val;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;

	info("%s: qspi interrupt handle test\n", argv[0]);

	// init qspi controller
	ret_val = qspi_init(argc, argv);
	if (ret_val) 
	{
		return 1;
	}

	// register int handler and enable it
	irq_generic_install_handler(INT_SPI0, qspi_int_handler, NULL);
	irq_generic_config_int(INT_SPI0, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_SPI0);

	// enable interrupt
	qspi_set_int_mask(0xFFFFFFFF);

	// read a page to trigger the interrupt
	qspi_nand_page_read_1(0, test_recv_buff, p_qspi_para->page_size);

	mdelay(100);
	
	if (qspi_int_counter == 0) 
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
	irq_generic_disable_int(INT_SPI0);
	irq_generic_remove_handler(INT_SPI0, NULL, NULL);

	return 0;
}

//************************************************************************************************************
// QSPI NAND mode test
//************************************************************************************************************
static int qspi_mode(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t ecc_mode, pattern;
	uint32_t wmode, rmode, random_flag = 1;
	uint32_t i, size;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	
	info("%s: qspi write/read mode test\n", __func__);

	// Check the validity of parameter count
	info("%s: test different bit mode write/read: 1/2/4 bits\n", __func__);
	if (argc < 7) 
	{
		info("cmd_format: %s cs clk(HZ) phase blk wmode rmode [pattern]\n", argv[0]);
		info("wmode: 1/4 bits\n");
		info("rmode: 1/2/4 bits\n");
		return 0;
	}

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}	

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk))
	{
		return 1;
	}	

	wmode = get_arg_ulong(argv[5]);
	if ((wmode != 1) && (wmode != 4))
	{
		info("wmode(%d) shoud be 1/4 bits\n", wmode);
		return 1;
	}

	if (wmode == 1)
	{
		p_qspi_para->io_wmode = SIO_MODE;
	}
	else if (wmode == 2)
	{
		p_qspi_para->io_wmode = DIO_MODE;
	}
	else
	{
		p_qspi_para->io_wmode = QIO_MODE;
	}
	
	rmode = get_arg_ulong(argv[6]);
	if ((rmode != 1) && (rmode != 2) && (rmode != 4))
	{
		info("rmode(%d) shoud be 1/2/4 bits\n", rmode);
		return 1;
	}

	if (rmode == 1)
	{
		p_qspi_para->io_rmode = SIO_MODE;
	}
	else if (rmode == 2)
	{
		p_qspi_para->io_rmode = DIO_MODE;
	}
	else
	{
		p_qspi_para->io_rmode = QIO_MODE;
	}

	if (argc >= 8) 
	{
		pattern = get_arg_ulong(argv[7]);
		random_flag = 0;
	}

	info("write using %s bit mode, read using %s bit mode\n",
		(wmode == 1) ? "1" : ((wmode == 2) ? "2" : "4"),
		(rmode == 1) ? "1" : ((rmode == 2) ? "2" : "4"));

	// Erase the block
	info("erase block 0x%x\n", blk);
	if (QSPI_ST_DONE != qspi_nand_blk_erase(blk<<p_qspi_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("read and check the block, all value should be 0xFF\n");
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("read and check blk 0x%x, page 0x%x\n", blk, page);
		
		addr = (blk << p_qspi_para->blk_shift_bits) + page;
		size = p_qspi_para->page_size + p_qspi_para->spare_size;
		
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr, test_recv_buff, size)) 
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
	size = p_qspi_para->page_size;
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("write, read and check blk 0x%x, page 0x%x...\n", blk, page);
		
		addr = (blk << p_qspi_para->blk_shift_bits) + page;

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
			for (i = 0; i < size; i++) 
			{
				test_send_buff[i] = pattern & 0xFF;
			}
		}

		// write page
		info("write blk 0x%x, page 0x%x\n", blk, page);
		if (QSPI_ST_DONE != qspi_nand_page_write(addr, test_send_buff, size)) 
		{
			info("write page 0x%x failed\n", page);
			fail++;
			goto end;
		}			

		// read page
		info("read blk 0x%x, page 0x%x\n", blk, page);
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
		if (QSPI_ST_DONE != qspi_nand_page_read(addr, test_recv_buff, size)) 
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
// Scan the bad block table
//************************************************************************************************************
static int qspi_scan_bbt(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t blk, va, pa;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;
	
	info("%s: qspi scan bbt test\n", __func__);

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}

	// scan the bbt
	if (QSPI_ST_DONE != qspi_bbt_scan(p_bbt))
	{
		fail++;
	}

	mem_print_line((uint8_t *)p_bbt, sizeof(bbt_t));

	// va->pa convert
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		va = blk << p_qspi_para->blk_shift_bits;
		qspi_bbt_va_to_pa(p_bbt, va, &pa);
		info("blk:%x, va->pa:%x->%x\n", blk, (va>>p_qspi_para->blk_shift_bits), (pa>>p_qspi_para->blk_shift_bits));
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
static int qspi_mark_bb(int argc, char* argv[])
{
	uint32_t blk;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;

	info("%s: qspi mark bad block test\n", __func__);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s cs clk(HZ) phase blk\n", argv[0]);
		return 1;
	}

	// init qspi controller
	if (qspi_init(argc, argv))
	{
		return 1;
	}	

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk)) 
	{
		return 1;
	}

	qspi_bbt_blk_marked(p_bbt, blk, BBT_TYPE_BAD);

	info("marked blk 0x%x as bad block in bbt\n", blk);

	return 0;
}

//************************************************************************************************************
// Check a bad block
//************************************************************************************************************
static int qspi_check_bb(int argc, char *argv[])
{
	uint32_t ret_val;
	uint32_t blk;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;
	E_BBT_TYPE type;
	
	info("%s: qspi check bad block test\n", __func__);

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: %s cs clk(HZ) phase blk\n", argv[0]);
		return 1;
	}

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk)) 
	{
		return 1;
	}

	type = qspi_bbt_blk_verify(p_bbt, blk);
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
// Write the bad block table to the flash
//************************************************************************************************************
static int qspi_write_bbt(int argc, char* argv[])
{
	int32_t fail = 0;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;

	info("%s: qspi write bbt test\n", __func__);
	
	// init qspi controller
	if (qspi_init(argc, argv))
	{
		return 1;
	}

	// write the bbt
	if (QSPI_ST_DONE != qspi_bbt_write(p_bbt))
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
static int qspi_read_bbt(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t blk, va, pa;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	bbt_t *p_bbt = (bbt_t *)&g_bbt;

	info("%s: qspi read bbt test\n", __func__);
	
	// init qspi controller
	if (qspi_init(argc, argv))
	{
		return 1;
	}

	// read the bbt
	if (QSPI_ST_DONE != qspi_bbt_read(p_bbt))
	{
		fail++;
	}

	mem_print_line((uint8_t *)p_bbt, sizeof(bbt_t));

	// va->pa convert
	for (blk = 0; blk < p_qspi_para->blks_per_chip; blk++)
	{
		va = blk << p_qspi_para->blk_shift_bits;
		qspi_bbt_va_to_pa(p_bbt, va, &pa);
		info("blk:%x, va->pa:%x->%x\n", blk, (va>>p_qspi_para->blk_shift_bits), (pa>>p_qspi_para->blk_shift_bits));
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
// NAND ecc bit test
//************************************************************************************************************
static int qspi_ecc_test(int argc, char* argv[])
{
	int32_t fail = 0;
	uint32_t page, blk[2], addr[2], oob[2], data;
	uint32_t pattern;
	uint32_t random_flag = 1;
	uint32_t i, j, size;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	uint8_t *p_oob, *p_data;
	
	info("%s: \n", __func__);
	
 	info("cmd_format: %s cs clk(HZ) phase src_blk dst_blk [pattern]\n", argv[0]);
	info("[pattern]: if specified, use pattern as test data, else use random data\n");

	// Check the validity of parameter count
	if (argc < 6) 
	{
 		info("wrong cmd_format: %s cs clk(HZ) phase src_blk dst_blk [pattern]\n", argv[0]);
		return 0;
	}

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}	

	// Get the parameter
	blk[0] = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk[0]))
	{
		return 1;
	}

	blk[1] = get_arg_ulong(argv[5]);
	if (!qspi_check_para(blk[1]))
	{
		return 1;
	}	

	if (argc >= 7) 
	{
		pattern = get_arg_ulong(argv[6]);
		random_flag = 0;
	}

	// Erase the src and dst blocks
	for (j = 0; j < 2; j++)
	{
		info("erase block 0x%x\n", blk[j]);
		if (QSPI_ST_DONE != qspi_nand_blk_erase(blk[j]<<p_qspi_para->blk_shift_bits)) 
		{
			info("erase fail\n");
			fail++;
			goto end;
		}

		// read each page of the block to check
		info("read and check the block, all value should be 0xFF\n");
		for (page = 0; page < p_qspi_para->pages_per_blk; page++)
		{
			info("read and check blk 0x%x, page 0x%x\n", blk[j], page);
			
			addr[j] = (blk[j] << p_qspi_para->blk_shift_bits) + page;
			size = p_qspi_para->page_size + p_qspi_para->spare_size;
			
			memset(test_recv_buff, 0, sizeof(test_recv_buff));

			if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr[j], test_recv_buff, size)) 
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
	}

	// Write and read each page of the block to check
	info("write, read and compare src_blk with ecc...\n", blk[0]);
	
	srand(get_ticks());
	size = p_qspi_para->page_size;
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("write, read and check blk 0x%x, page 0x%x...\n", blk[0], page);
		
		addr[0] = (blk[0] << p_qspi_para->blk_shift_bits) + page;
		addr[1] = (blk[1] << p_qspi_para->blk_shift_bits) + page;

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
			for (i = 0; i < size; i++) 
			{
				test_send_buff[i] = pattern & 0xFF;
			}
		}

		g_qspi_nand_oob[0] = 0x11223300 + page;
		g_qspi_nand_oob[1] = 0x55667700 + page;

		for (j = 0; j < 2; j++)
		{
			// write page
			info("write blk 0x%x, page 0x%x\n", blk[j], page);
			if (QSPI_ST_DONE != qspi_nand_page_write(addr[j], test_send_buff, size)) 
			{
				info("write page 0x%x failed\n", page);
				fail++;
				goto end;
			}

			// read page with page size + spare size
			info("read blk 0x%x, page 0x%x, size:0x%x\n", blk[j], page, size);
			memset(test_recv_buff, 0, sizeof(test_recv_buff));
			if (QSPI_ST_DONE != qspi_nand_page_read(addr[j], test_recv_buff, size)) 
			{
				info("read page 0x%x failed\n", page);
				fail++;
				goto end;
			}	
			
			// compare
			info("check blk 0x%x, page 0x%x, size:0x%x\n", blk[j], page, size);
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
	}	

	// dump the data of src_blk without ecc, modified oob and write the data to dst_ecc with ecc
	info("dump the data from src_blk(0x%x) without ecc, modified oob and write the data to dst_blk(0x%x) with ecc...\n", blk[0], blk[1]);
	
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{		
		addr[1] = (blk[1] << p_qspi_para->blk_shift_bits) + page;

		// read page with page size + spare size(without ecc)
		size = p_qspi_para->page_size + p_qspi_para->spare_size;
		
		info("read blk 0x%x, page 0x%x, size:0x%x without ecc\n", blk[1], page, size);
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
		if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr[1], test_recv_buff, size)) 
		{
			info("read page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		// modified data
		p_data = &test_recv_buff[0];
		data = *p_data;
		
		*p_data = (*p_data & 1) ? (*p_data & 0xFE) : (*p_data | 0x1); // modified 1 bit 
		info("modify data 0x%x->0x%x at pos 0\n", data, *p_data);
		
		// modified oob
		p_oob = &test_recv_buff[g_qspi_nand_oob_ofst[0]];
		oob[0] = *p_oob;
		
		*p_oob = (*p_oob & 1) ? (*p_oob & 0xFE) : (*p_oob | 0x1); // modified 1 bit 
		info("modify oob 0x%x->0x%x at pos 0x%x\n", oob[0], *p_oob, g_qspi_nand_oob_ofst[0]);
		
		// write page with page size + spare size(without ecc)
		info("write blk 0x%x, page 0x%x without ecc(oob modified 1 bit)\n", blk[1], page);
		if (QSPI_ST_DONE != qspi_nand_raw_data_write(addr[1], test_recv_buff, size)) 
		{
			info("write page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		// read page with page size + spare size(without ecc)		
		info("read blk 0x%x, page 0x%x, size:0x%x without ecc\n", blk[1], page, size);
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
		if (QSPI_ST_DONE != qspi_nand_raw_data_read(addr[1], test_recv_buff, size)) 
		{
			info("read page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		mem_print_line(&test_recv_buff[p_qspi_para->page_size], p_qspi_para->spare_size);
	}		

	// dump the data of src_blk without ecc, modified oob and write the data to dst_ecc with ecc
	info("\nread and compare the data and oob from the src_blk(0x%x) and dst_blk(0x%x) with ecc...\n", blk[0], blk[1]);
	
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		addr[0] = (blk[0] << p_qspi_para->blk_shift_bits) + page;
		addr[1] = (blk[1] << p_qspi_para->blk_shift_bits) + page;

		// read page with page size and oob(with ecc)
		size = p_qspi_para->page_size;
		
		info("\nread src_blk 0x%x, page 0x%x, size:0x%x\n", blk[0], page, size);
		memset(test_recv_buff, 0, sizeof(test_recv_buff));
		g_qspi_nand_oob[0] = 0;
		g_qspi_nand_oob[1] = 0;
		
		if (QSPI_ST_DONE != qspi_nand_page_read(addr[0], test_recv_buff, size)) 
		{
			info("read page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		oob[0] = g_qspi_nand_oob[0];
		oob[1] = g_qspi_nand_oob[1];
		info("read src_blk: 0x%x oob: 0x%x, 0x%x\n", blk[0], oob[0], oob[1]);

		// read page with page size and oob(with ecc)
		info("read page with page size and oob(with ecc) dst_blk 0x%x, page 0x%x, size:0x%x\n", blk[1], page, size);
		memset(test_send_buff, 0, sizeof(test_send_buff));
		g_qspi_nand_oob[0] = 0;
		g_qspi_nand_oob[1] = 0;
		
		if (QSPI_ST_DONE != qspi_nand_page_read(addr[1], test_send_buff, size)) 
		{
			info("read page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		info("read dst_blk: 0x%x oob: 0x%x, 0x%x\n", blk[1], g_qspi_nand_oob[0], g_qspi_nand_oob[1]);
		
		// compare data and oob
		info("check blk 0x%x, page 0x%x, size:0x%x\n", blk[0], page, size);
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != test_send_buff[i]) 
			{
				info("page 0x%x data verify failed at 0x%x, expect 0x%x act 0x%x\n", page, i, test_send_buff[i], test_recv_buff[i]);
				fail++;
				goto end;
			}
		}

		if ((g_qspi_nand_oob[0] != oob[0]) || (g_qspi_nand_oob[1] != oob[1]))
		{
			info("page 0x%x oob verify failed, expect 0x%x 0x%x, act 0x%x 0x%x\n", page, oob[0], oob[1], g_qspi_nand_oob[0], g_qspi_nand_oob[1]);
			fail++;
			goto end;
		}

		// read page with page size and no oob(with ecc)
		info("read page with page size and no oob (with ecc) dst_blk 0x%x, page 0x%x, size:0x%x\n", blk[1], page, size);
		memset(test_send_buff, 0, sizeof(test_send_buff));
		
		if (QSPI_ST_DONE != qspi_nand_page_read_without_oob(addr[1], test_send_buff, size)) 
		{
			info("read page 0x%x failed\n", page);
			fail++;
			goto end;
		}
				
		// compare data and oob
		info("check blk 0x%x, page 0x%x, size:0x%x\n", blk[0], page, size);
		for (i = 0; i < size; i++) 
		{
			if (test_recv_buff[i] != test_send_buff[i]) 
			{
				info("page 0x%x data verify failed at 0x%x, expect 0x%x act 0x%x\n", page, i, test_send_buff[i], test_recv_buff[i]);
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

static int qspi_mode_write_only(int argc, char *argv[])
{
	int32_t fail = 0;
	uint32_t page, blk, addr;
	uint32_t ecc_mode, pattern;
	uint32_t wmode, rmode, random_flag = 1;
	uint32_t i, size;
	qspi_nand_para_t *p_qspi_para = &g_qspi_nand_para;
	
	info("%s: qspi write/read mode test\n", __func__);

	// Check the validity of parameter count
	info("%s: test different bit mode write/read: 1/2/4 bits\n", __func__);
	if (argc < 7) 
	{
		info("cmd_format: %s cs clk(HZ) phase blk wmode rmode [pattern]\n", argv[0]);
		info("wmode: 1/4 bits\n");
		info("rmode: 1/2/4 bits\n");
		return 0;
	}

	// init qspi controller
	if (qspi_init(argc, argv)) 
	{
		return 1;
	}	

	// Get the parameter
	blk = get_arg_ulong(argv[4]);
	if (!qspi_check_para(blk))
	{
		return 1;
	}	

	wmode = get_arg_ulong(argv[5]);
	if ((wmode != 1) && (wmode != 4))
	{
		info("wmode(%d) shoud be 1/4 bits\n", wmode);
		return 1;
	}

	if (wmode == 1)
	{
		p_qspi_para->io_wmode = SIO_MODE;
	}
	else if (wmode == 2)
	{
		p_qspi_para->io_wmode = DIO_MODE;
	}
	else
	{
		p_qspi_para->io_wmode = QIO_MODE;
	}
	
	rmode = get_arg_ulong(argv[6]);
	if ((rmode != 1) && (rmode != 2) && (rmode != 4))
	{
		info("rmode(%d) shoud be 1/2/4 bits\n", rmode);
		return 1;
	}

	if (rmode == 1)
	{
		p_qspi_para->io_rmode = SIO_MODE;
	}
	else if (rmode == 2)
	{
		p_qspi_para->io_rmode = DIO_MODE;
	}
	else
	{
		p_qspi_para->io_rmode = QIO_MODE;
	}

	if (argc >= 8) 
	{
		pattern = get_arg_ulong(argv[7]);
		random_flag = 0;
	}

	info("write using %s bit mode, read using %s bit mode\n",
		(wmode == 1) ? "1" : ((wmode == 2) ? "2" : "4"),
		(rmode == 1) ? "1" : ((rmode == 2) ? "2" : "4"));

	// Erase the block
	info("erase block 0x%x\n", blk);
	if (QSPI_ST_DONE != qspi_nand_blk_erase(blk<<p_qspi_para->blk_shift_bits)) 
	{
		info("erase fail\n");
		fail++;
		goto end;
	}

	// read each page of the block to check
	info("write, read and compare...\n");

	srand(get_ticks());
	size = p_qspi_para->page_size;
	for (page = 0; page < p_qspi_para->pages_per_blk; page++)
	{
		info("write, read and check blk 0x%x, page 0x%x...\n", blk, page);
		
		addr = (blk << p_qspi_para->blk_shift_bits) + page;

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
			for (i = 0; i < size; i++) 
			{
				test_send_buff[i] = pattern & 0xFF;
			}
		}

		// write page
		info("write blk 0x%x, page 0x%x\n", blk, page);
		if (QSPI_ST_DONE != qspi_nand_page_write(addr, test_send_buff, size)) 
		{
			info("write page 0x%x failed\n", page);
			fail++;
			goto end;
		}

		udelay(100000);
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

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry qspi_test_menu[] = {
	{"help", 				cmd_help, 			"           : Display list of commands"},
	{"h",					cmd_help, 			"              : alias for help"},
	{"?",					cmd_help, 			"              : alias for help"},
	{"qspi_probe",			qspi_probe,  		"     : probe test"},
	{"qspi_erase",			qspi_erase,  		"     : erase test"},
	{"qspi_write",			qspi_write,  		"     : write test"},
	{"qspi_read",			qspi_read,  		"      : read test"},
	{"qspi_mode",			qspi_mode,  		"      : bit mode test"},
	{"qspi_int",			qspi_int,  			"       : interrupt handle test"},
	{"qspi_erase_all",		qspi_erase_all, 	" : erase all blocks test"},
	{"qspi_scan_bbt",		qspi_scan_bbt,		"  : scan bbt"},
	{"qspi_mark_bb",		qspi_mark_bb,		"   : mark bad block"},
	{"qspi_check_bb",		qspi_check_bb,		"  : check block's status"},
	{"qspi_write_bbt",		qspi_write_bbt, 	" : write bbt to flash"},
	{"qspi_read_bbt",		qspi_read_bbt,		"  : read bbt from flash"},
	{"qspi_ecc_test",		qspi_ecc_test,		"  : write and read data with ecc"},
	{"qspi_mode_write_only",		qspi_mode_write_only,		"  : write only with ecc"},
	{"q",      				NULL,				"              : quit nand test"},
	{0, 					0, 					0}
};

#define QSPI_TX_THRESH_DEF_VAL	0x01
bool qspi_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
	
	info("write tx threshold reg\n");
	
	// write register
	writel(*p_wr_val, QSPI_TXTHRESH_REG);
	
	info("read&compare tx threshold reg\n");
	*p_rd_val = readl(QSPI_TXTHRESH_REG);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("qspi reset\n");
	prcm_reset(ARM_QSPI_RST);
	
	// read register
	info("read&compare tx threshold reg\n");
	*p_rd_val = readl(QSPI_TXTHRESH_REG);

	if (*p_rd_val != QSPI_TX_THRESH_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool qspi_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_QSPI;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// write register
	info("write tx threshold reg\n");
	writel(*p_wr_val, QSPI_TXTHRESH_REG);
	
	info("read&compare tx threshold reg\n");
	*p_rd_val = readl(QSPI_TXTHRESH_REG);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write tx threshold reg\n");
	writel((*p_wr_val+1), QSPI_TXTHRESH_REG);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare tx threshold reg\n");
	*p_rd_val = readl(QSPI_TXTHRESH_REG);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool qspi_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool qspi_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

