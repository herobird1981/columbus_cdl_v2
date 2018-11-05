/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_i2c.h"
#include "cdl_pll.h"

#if (CONFIG_I2C_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
struct dw_i2c i2c_dev[NUM_I2C_PORTS];
struct dw_i2c *p_i2c_dev[NUM_I2C_PORTS] = {&i2c_dev[0], &i2c_dev[1], &i2c_dev[2]};

cmdline_entry i2c_test_menu[];

/*
store i2c slave data: each reg offset can access the coresponding value
continus access is permit
*/
static uint8_t i2c_slave_buf[TEST_BUFFER_SIZE] = {0};

static int g_i2c_int_cnt[NUM_I2C_PORTS] = {0, 0, 0};

static uint32_t g_i2c_port, g_i2c_slave_port;

static uint32_t g_i2c_data_len;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/
void i2c_master_int_handler(void *ptr);

void i2c_slave_int_handler(void *ptr);

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
static int cmd_help(int argc, char *argv[])
{
	return cmdline_help_general(&i2c_test_menu[0]);
}

static int i2c_slave(int argc, char* argv[])
{
	uint32_t dev, addr_mode;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	// get the slave port for interrupt
	g_i2c_slave_port = port;
	prcm_reset(ARM_I2C0_RST + port);

	info("i2c_slave:i2c slave mode test\n");

	// Check the validity of parameter count
	if (argc < 3) 
	{
		info("wrong cmd_format: i2c_slave dev addr_mode\n");
		return 0;
	}

	// Get the parameters
	dev = get_arg_ulong(argv[1]);
	addr_mode = (!strcmp(argv[2], "10b")) ? DW_I2C_SLAVE_10BIT_ADDR : DW_I2C_SLAVE_7BIT_ADDR;

	info("input cmd_format: i2c_slave dev(0x%x) addr_mode(%s)\n", dev, argv[2]);

	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		info("set pinmux to i2c fail\n");
		return 0;	
	}

	// Set i2c to slave mode
	memset(i2c_slave_buf, 0, sizeof(i2c_slave_buf));
	dw_i2c_init_slave(priv, dev, addr_mode, 4, sizeof(i2c_slave_buf));

	// Register int handler and enable it
	irq_generic_install_handler(priv->int_vector, i2c_slave_int_handler, NULL);
	irq_generic_config_int(priv->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(priv->int_vector);

	// Enable interrupt
	dw_i2c_set_int_mask(priv, (IC_RX_FULL|IC_RD_REQ|IC_STOP_DET));	

	info("i2c_slave: setup terminate\n");

	return 0;
}

// testcase: basic master mode write/read test
static int i2c_master(int argc, char *argv[])
{
	uint32_t dma_mode, speed, dev, addr_mode, len;
	int32_t ret_val;
	uint32_t fail = 0;
	uint32_t i, pattern, random_flag = 1;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	prcm_reset(ARM_I2C0_RST + port);

	info("i2c_master: master mode basic write/read test\n");

	// Check the validity of parameter count
	if (argc < 6) 
	{
		info("wrong cmd_format: i2c_master dma_mode speed dev addr_mode len [pattern]\n");
		return 0;
	}

	if (argc >= 7)
	{
		pattern = get_arg_ulong(argv[6]);
		random_flag = 0;
	}

	// Get the parameters
	dma_mode = get_arg_ulong(argv[1]) ? 1 : 0;	
	speed = get_arg_ulong(argv[2]);
	dev = get_arg_ulong(argv[3]);	
	addr_mode = (!strcmp(argv[4], "10b")) ? DW_I2C_SLAVE_10BIT_ADDR : DW_I2C_SLAVE_7BIT_ADDR;
	len = get_arg_ulong(argv[5]);

	info("input cmd_format: i2c_master dma_mode(%d) speed(%d) dev(0x%x) addr_mode(%s) len(%d)\n",
			dma_mode, speed, dev, argv[4], len);

	if(len>4095)
	{
		info("len = %d > 4095, too long\r\n", len);
		return 0;
	}
	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		return 0;	
	}

	// Set i2c to master mode
	dw_i2c_init(priv, (uint32_t)priv->regs, priv->pclk, dma_mode);
	dw_i2c_init_master(priv, speed, addr_mode);

	// Generate random/pattern data
	srand(get_ticks());
	if (random_flag)
	{
		for (i = 0; i < len; i++)
		{
			test_send_buff[i] = cb_rand() & 0xFF;
		}
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			test_send_buff[i] = pattern & 0xFF;
		}
	}

	// Clear receive buffer
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	// i2c write data 
	info("i2c write data, len: 0x%x\n", len);
	ret_val = dw_i2c_write(priv, dev, 0, 0, test_send_buff, len);
	if (ret_val) 
	{
		debug("i2c write data fail, ret=%d\n", ret_val);
		fail++;
		goto end;
	}

	// i2c read data
	info("i2c read data, len: 0x%x\n", len);
	ret_val = dw_i2c_read(priv, dev, 0, 0, test_recv_buff, len);
	if (ret_val) 
	{
		debug("i2c read data fail, ret=%d\n", ret_val);
		fail++;
		goto end;
	}

	// Check result
	info("check data, len: 0x%x\n", len);
	for (i = 0; i < len; i++) 
	{
		//debug("i2c check data @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);
		
		if (test_send_buff[i] != test_recv_buff[i]) 
		{
			debug("i2c check data fail @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);
			fail++;
			break;
		}
	}

end:
	if (fail) 
	{			    
		info("i2c_master: test fail, abrt_sts = 0x%x\n", dw_i2c_get_abrt_source(priv));
		//dw_i2c_tx_abort_recovery(priv);
		info("!!!this failure should affect next time's transfer if abrt_sts is not zero and dma is used!!!\n"); 
	} 
	else 
	{
		info("i2c_master: test pass\n");
	}

	// disable interrupt(master)
	irq_generic_disable_int(priv->int_vector);
	irq_generic_remove_handler(priv->int_vector, NULL, NULL);
	dw_i2c_set_int_mask(priv, 0);

	// disable interrupt(slave)
	priv = p_i2c_dev[g_i2c_slave_port];
	irq_generic_disable_int(priv->int_vector);
	irq_generic_remove_handler(priv->int_vector, NULL, NULL);
	dw_i2c_set_int_mask(priv, 0);

	return 0;
}

// testcase: a special test case, which is designed for i2c slave interrupt test
static int i2c_master2(int argc, char *argv[])
{
	uint32_t dma_mode, speed, dev, addr_mode, len=10;
	int32_t ret_val;
	uint32_t fail = 0;
	uint32_t i;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	prcm_reset(ARM_I2C0_RST + port);

	info("i2c_master2: a special test case, which is designed for i2c slave interrupt test\n");

	// Check the validity of parameter count
	if (argc < 3)
	{
		info("wrong cmd_format: i2c_master2 dev addr_mode \n");
		return 0;
	}

	// Get the parameters
	dev = get_arg_ulong(argv[1]);
	addr_mode = (!strcmp(argv[2], "10b")) ? DW_I2C_SLAVE_10BIT_ADDR : DW_I2C_SLAVE_7BIT_ADDR;

	info("input cmd_format: i2c_master2 dev(0x%x) addr_mode(%s)\n", dev, argv[2]);

	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		return 0;
	}

	// Set i2c to master mode
	dw_i2c_init(priv, (uint32_t)priv->regs, priv->pclk, 1);
	dw_i2c_init_master(priv, 400000, addr_mode);

	// Generate pattern data
	srand(get_ticks());

	for (i = 0; i < len; i++)
	{
		test_send_buff[i] = cb_rand() & 0xFF;
	}

	// Clear receive buffer
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	// i2c write data
	info("i2c write data, len: 0x%x\n", len);
	ret_val = dw_i2c_write(priv, dev, 0, 0, test_send_buff, len);
	if (ret_val)
	{
		debug("i2c write data fail, ret=%d\n", ret_val);
		fail++;
		goto end;
	}

	// i2c read data
	info("i2c read data, len: 0x%x\n", len);
	ret_val = dw_i2c_read(priv, dev, 0, 0, test_recv_buff, len);
	if (ret_val)
	{
		debug("i2c read data fail, ret=%d\n", ret_val);
		fail++;
		goto end;
	}

	// Check result
	info("check data, len: 0x%x\n", len);
	for (i = 0; i < len; i++)
	{
		//debug("i2c check data @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);

		if (test_send_buff[i] != test_recv_buff[i])
		{
			debug("i2c check data fail @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);
			fail++;
			break;
		}
	}

end:
	if (fail)
	{
		info("i2c_master2: test fail, abrt_sts = 0x%x\n", dw_i2c_get_abrt_source(priv));
		//dw_i2c_tx_abort_recovery(priv);
		info("!!!this failure should affect next time's transfer if abrt_sts is not zero and dma is used!!!\n");
	}
	else
	{
		info("i2c_master2: test pass\n");
	}

	// disable interrupt(master)
	irq_generic_disable_int(priv->int_vector);
	irq_generic_remove_handler(priv->int_vector, NULL, NULL);
	dw_i2c_set_int_mask(priv, 0);

	// disable interrupt(slave)
	priv = p_i2c_dev[g_i2c_slave_port];
	irq_generic_disable_int(priv->int_vector);
	irq_generic_remove_handler(priv->int_vector, NULL, NULL);
	dw_i2c_set_int_mask(priv, 0);

	return 0;
}


// testcase: robust write/read test
#define ROBUST_BUFFER_SIZE 4096
static int i2c_robust(int argc, char *argv[])
{
	uint32_t dma_mode, speed, dev,addr_mode;
	uint32_t len;
	uint32_t fail, test_fail = 0;
	uint32_t i, loop = 0, pattern, random_flag = 1;
	int32_t ret_val;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	prcm_reset(ARM_I2C0_RST + port);

	info("i2c_robust: master mode robust write/read test, press 'q' to quit\n");

	// Check the validity of parameter count
	if (argc < 5) 
	{
		info("wrong cmd_format: i2c_robust dma_mode speed dev addr_mode [pattern]\n");
		return 0;
	}

	if (argc >= 6)
	{
		pattern = get_arg_ulong(argv[5]);
		random_flag = 0;
	}

	// Get the parameters
	dma_mode = get_arg_ulong(argv[1]) ? 1 : 0;
	speed = get_arg_ulong(argv[2]);
	dev = get_arg_ulong(argv[3]);
	addr_mode = (!strcmp(argv[4], "10b")) ? DW_I2C_SLAVE_10BIT_ADDR : DW_I2C_SLAVE_7BIT_ADDR;

	info("input cmd_format: i2c_robust dma_mode(%d) speed(%d) dev(0x%x)\n",	dma_mode, speed, dev);

	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		return 0;	
	}

	// Set i2c to master mode
	dw_i2c_init(priv, (uint32_t)priv->regs, priv->pclk, dma_mode);
	dw_i2c_init_master(priv, speed, addr_mode);

	while (1) 
	{
		// press 'q' to quit
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}

		fail = 0;
		srand(get_ticks());

		// generate random len
		len = cb_rand() % (ROBUST_BUFFER_SIZE - 1);
		if (len == 0)
		{
			len = 16;
		}
		
		// generate random/pattern data
		if (random_flag)
		{
			for (i = 0; i < len; i++)
			{
				test_send_buff[i] = cb_rand() & 0xFF;
			}
		}
		else
		{
			for (i = 0; i < len; i++)
			{
				test_send_buff[i] = pattern & 0xFF;
			}
		}

		// Clear receive buffer
		info("i2c write data, len: 0x%x\n", len);
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		// i2c write data 
		ret_val = dw_i2c_write(priv, dev, 0, 0, test_send_buff, len);
		if (ret_val) 
		{
			debug("i2c write data fail, ret=%d\n", ret_val);
			fail++;
			goto repeat_done;
		}

		// i2c read data
		info("i2c read data, len: 0x%x\n", len);
		ret_val = dw_i2c_read(priv, dev, 0, 0, test_recv_buff, len);
		if (ret_val) 
		{
			debug("i2c read data fail, ret=%d\n", ret_val);
			fail++;
			goto repeat_done;
		}
		
		// Check result
		info("check data, len: 0x%x\n", len);
		for (i = 0; i < len; i++) 
		{
			if (test_send_buff[i] != test_recv_buff[i]) 
			{
				debug("i2c check data fail @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);
				fail++;
				break;
			}
		}

repeat_done:
		if (fail)
		{
			test_fail++;
		}
		
		loop++;
		info("i2c_robust: loop %d, len=0x%x, %s, total fail=%d\n", loop, len, (fail ? "fail" : "success"), test_fail);
	}

	info("i2c_robust test terminate, total loop %d failed %d\n", loop, test_fail);
	
	return 0;
}


void i2c_master_int_test_ISR(void *ptr)
{
	uint32_t int_sts = 0;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	int_sts = dw_i2c_get_int_status(priv);
	if (int_sts & IC_ACTIVITY) 
	{
		dw_i2c_readl(priv, DW_IC_CLR_ACTIVITY);
		dw_i2c_set_int_mask(priv, 0xFFFF & (~IC_ACTIVITY));
		info("IC_ACTIVITY:%x\n", IC_ACTIVITY);
		g_i2c_int_cnt[port]++;
	}
	if (int_sts & IC_START_DET) 
	{
		dw_i2c_readl(priv, DW_IC_CLR_START_DET);
		dw_i2c_set_int_mask(priv, 0xFFFF & (~IC_START_DET));
		info("IC_START_DET:%x\n", IC_START_DET);
		g_i2c_int_cnt[port]++;
	}
	if (int_sts & IC_TX_EMPTY) 
	{
	    dw_i2c_set_int_mask(priv, 0xFFFF & (~IC_TX_EMPTY));
		info("IC_TX_EMPTY:%x\n", IC_TX_EMPTY);
		g_i2c_int_cnt[port]++;
	}

	if(g_i2c_int_cnt[port] > 0x20)
		dw_i2c_set_int_mask(priv, 0x0);
	
	dw_i2c_clear_all_int(priv);

}

void i2c_slave_int_test_ISR(void *ptr)
{
	struct dw_i2c *priv = p_i2c_dev[g_i2c_slave_port];
	uint32_t int_sts = 0;
	E_I2C_PORT port = g_i2c_slave_port;

	// master read request	  
	int_sts = dw_i2c_readl(priv, DW_IC_INTR_STAT);
	info("int_sts:%x\n", int_sts);
	if (int_sts & IC_RD_REQ) 
	{
		info("IC_RD_REQ:%x\n", IC_RD_REQ);
		g_i2c_int_cnt[port]++;
	}
		
	// reg off not enough,something wrong for the protocol
	if (int_sts & IC_START_DET) 
	{
		info("IC_START_DET:%x\n", IC_START_DET);
		g_i2c_int_cnt[port]++;
	}

	// tx fifo not empty when request read,clr the interrupt
	if (int_sts & IC_TX_ABRT) 
	{
		info("IC_TX_ABRT:%x\n", IC_TX_ABRT);
		g_i2c_int_cnt[port]++;
	}
		
	if (int_sts & IC_STOP_DET) 
	{
		info("IC_STOP_DET:%x\n", IC_STOP_DET);		
		g_i2c_int_cnt[port]++;
	}
	
	if (int_sts & IC_RX_DONE) 
	{
		dw_i2c_readl(priv, DW_IC_CLR_RX_DONE);
		info("IC_RX_DONE:%x\n", IC_RX_DONE);		
		g_i2c_int_cnt[port]++;
	}
	if(g_i2c_int_cnt[port] > 0x20)
		dw_i2c_set_int_mask(priv, 0x0);

	dw_i2c_slave_transfer(priv, i2c_slave_buf);

}

void i2c_slave_int_handler(void *ptr)
{
	struct dw_i2c *priv = p_i2c_dev[g_i2c_slave_port];
	
	dw_i2c_slave_transfer(priv, i2c_slave_buf);
}

/*testcase:i2c interrupt test*/
static int i2c_int_master(int argc, char* argv[])
{
	uint32_t dma_mode, speed, dev, addr_mode, len=10;
	int32_t ret_val;
	uint32_t fail = 0;
	uint32_t i, pattern, random_flag = 1;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	prcm_reset(ARM_I2C0_RST + port);

	info("i2c_int_master: master mode interrupt test\n");

	// Check the validity of parameter count
	if (argc < 3)
	{
		info("wrong cmd_format: i2c_int_master dev addr_mode \n");
		return 0;
	}

	// Get the parameters
	dev = get_arg_ulong(argv[1]);
	addr_mode = (!strcmp(argv[2], "10b")) ? DW_I2C_SLAVE_10BIT_ADDR : DW_I2C_SLAVE_7BIT_ADDR;

	info("input cmd_format: i2c_int_master dev(0x%x) addr_mode(%s)\n", dev, argv[2]);

	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		return 0;	
	}

	g_i2c_int_cnt[port] = 0;

	// Set i2c to master mode
	dw_i2c_init(priv, (uint32_t)priv->regs, priv->pclk, 1);
	dw_i2c_init_master(priv, 100000, addr_mode);

	// register int handler and enable it
	irq_generic_install_handler(priv->int_vector, i2c_master_int_test_ISR, NULL);
	irq_generic_config_int(priv->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(priv->int_vector);

	// enable interrupt
	dw_i2c_set_int_mask(priv, 0xFFFF);

	// i2c write data 
	info("i2c write data, len: 0x%x\n", len);
	ret_val = dw_i2c_write(priv, dev, 0, 0, test_send_buff, len);
	if (ret_val) 
	{
		debug("i2c write data fail, ret=%d\n", ret_val);
		fail++;
		goto end;
	}

	// i2c read data
	info("i2c read data, len: 0x%x\n", len);
	ret_val = dw_i2c_read(priv, dev, 0, 0, test_recv_buff, len);
	if (ret_val) 
	{
		debug("i2c read data fail, ret=%d\n", ret_val);
		fail++;
		goto end;
	}

end:
	// disable interrupt(master)
	irq_generic_disable_int(priv->int_vector);
	irq_generic_remove_handler(priv->int_vector, NULL, NULL);
	dw_i2c_set_int_mask(priv, 0);

	// disable interrupt(slave)
	priv = p_i2c_dev[g_i2c_slave_port];
	irq_generic_disable_int(priv->int_vector);
	irq_generic_remove_handler(priv->int_vector, NULL, NULL);
	dw_i2c_set_int_mask(priv, 0);
	
	return 0;
}

/*testcase:i2c interrupt test*/
static int i2c_int_slave(int argc, char* argv[])
{
	uint32_t dev, addr_mode;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	// get the slave port for interrupt
	g_i2c_slave_port = port;
	prcm_reset(ARM_I2C0_RST + port);

	info("i2c_int_slave:i2c slave mode interrupt test\n");

	// Check the validity of parameter count
	if (argc < 3) 
	{
		info("wrong cmd_format: i2c_int_slave dev addr_mode\n");
		return 0;
	}

	// Get the parameters
	dev = get_arg_ulong(argv[1]);
	addr_mode = (!strcmp(argv[2], "10b")) ? DW_I2C_SLAVE_10BIT_ADDR : DW_I2C_SLAVE_7BIT_ADDR;

	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		return 0;	
	}

	g_i2c_int_cnt[port] = 0;

	// Set i2c to slave mode
	memset(i2c_slave_buf, 0, sizeof(i2c_slave_buf));
	dw_i2c_init_slave(priv, dev, addr_mode, 4, sizeof(i2c_slave_buf));

	// Register int handler and enable it
	irq_generic_install_handler(priv->int_vector, i2c_slave_int_test_ISR, NULL);
	irq_generic_config_int(priv->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(priv->int_vector);	

	// Enable interrupt
	//dw_i2c_set_int_mask(priv, IC_RX_FULL|IC_RD_REQ|IC_STOP_DET | IC_RX_DONE);	
	dw_i2c_set_int_mask(priv, (IC_RX_FULL|IC_RD_REQ|IC_STOP_DET));

	info("i2c_int_slave: setup terminate\n");

	return 0;

}

#define E2PROM_SLAVE_ADDR (	0xA0>>1)
#define E2PROM_SLAVE_CAP 	256
#define E2PROM_BURST_LEN	16

// testcase: master mode write read using real peripheral
static int i2c_real(int argc, char* argv[])
{
	uint32_t dma_mode, speed, dev;
	uint32_t fail = 0, len, addr, burst_len, trans_len;
	int32_t ret_val, i, pattern, random_flag = 1;
	E_I2C_PORT port = g_i2c_port;
	struct dw_i2c *priv = p_i2c_dev[port];

	info("i2c_real: master mode test using peripheral(E2PROM M24C08)\n");

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: i2c_real dma_mode speed len [pattern]\n");
		return 0;
	}

	if (argc >= 5)
	{
		pattern = get_arg_ulong(argv[4]);
		random_flag = 0;
	}

	// Get the parameters
	dma_mode = get_arg_ulong(argv[1]) ? 1 : 0;
	speed = get_arg_ulong(argv[2]);
	len = get_arg_ulong(argv[3]);
	if (len >= E2PROM_SLAVE_CAP)
	{
		info("limited to be 256 bytes\n");
		len = E2PROM_SLAVE_CAP;
	}

	info("input cmd_format: i2c_real dma_mode(%d) speed(%d) len(0x%x)\n", dma_mode, speed, len);

	// Set pinmux to i2c
	info("set pinmux to i2c\n");
	if (!misc_i2c_pin_init(port))
	{
		return 0;	
	}	

	// Set i2c to master mode
	dw_i2c_init(priv, (uint32_t)priv->regs, priv->pclk, dma_mode);
	dw_i2c_init_master(priv, speed, 0);	

	srand(get_ticks());
	// generate random/pattern data
	if (random_flag)
	{
		for (i = 0; i < len; i++)
		{
			test_send_buff[i] = cb_rand() & 0xFF;
		}
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			test_send_buff[i] = pattern & 0xFF;
		}
	}

	// Clear receive buffer
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	// i2c write data 
	info("i2c write data, len: 0x%x\n", len);
	trans_len = len;
	addr = 0;
	while (trans_len)
	{
		burst_len = (trans_len >= E2PROM_BURST_LEN) ? E2PROM_BURST_LEN : trans_len; 
		
		ret_val = dw_i2c_write(priv, E2PROM_SLAVE_ADDR, addr, 1, &test_send_buff[addr], burst_len);
		if (ret_val) 
		{
			debug("i2c write data fail, ret=%d\n", ret_val);
			fail++;
			goto end;
		}

		addr += burst_len;
		trans_len -= burst_len;
		
		mdelay(50);
	}

	// i2c read data
	info("i2c read data, len: 0x%x\n", len);
	trans_len = len;
	addr = 0;
	while (trans_len)
	{
		burst_len = (trans_len >= E2PROM_BURST_LEN) ? E2PROM_BURST_LEN : trans_len; 
		
		ret_val = e2prom_i2c_read(priv, E2PROM_SLAVE_ADDR, addr, 1, &test_recv_buff[addr], burst_len);
		if (ret_val) 
		{
			debug("i2c read data fail, ret=%d\n", ret_val);
			fail++;
			goto end;
		}

		addr += burst_len;
		trans_len -= burst_len;
		
		mdelay(50);
	}

	// Check result
	info("check data, len: 0x%x\n", len);
	for (i = 0; i < len; i++) 
	{
		//debug("i2c check data @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);
		
		if (test_send_buff[i] != test_recv_buff[i]) 
		{
			debug("i2c check data fail @%d: expected %x receive %x\n", i, test_send_buff[i], test_recv_buff[i]);
			fail++;
			break;
		}
	}	

end:	
	if (fail) 
	{
		info("i2c_real: test fail\n");
	} 
	else 
	{
		info("i2c_real: test pass\n");
	}
	
	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry i2c_test_menu[] = {
	{"help", 		cmd_help, 	" : Display list of commands"},
	{"h",			cmd_help, 	"    : alias for help"},
	{"?",			cmd_help, 	"    : alias for help"},
	{"i2c_slave",	i2c_slave, 	"   : i2c slave mode test"},
	{"i2c_master",	i2c_master, "  : i2c master basic read/write test"},
	{"i2c_master2",	i2c_master2, "  : a special test case, which is designed for i2c slave interrupt test"},
	{"i2c_robust",	i2c_robust, "  : i2c master robust read/write test"},
	{"i2c_real",	i2c_real,  	"  : i2c master real peripheral test"},
	{"i2c_int_master",		i2c_int_master,  	"  : i2c interrupt handle test"},
    {"i2c_int_slave",		i2c_int_slave,  	"  : i2c interrupt handle test"},	
	{0, 			0, 			0}
};

/* if return none zero, something wrong,please check and can't continue test*/
int i2c_test_prepare(int port)
{
	int32_t ret_val = 0;
	struct dw_i2c *priv = p_i2c_dev[port];

	g_i2c_port = port;
	
	memset(priv, 0, sizeof(struct dw_i2c));
	
	if (port == 0) 
	{
		dw_i2c_init(priv, I2C0_BASE, I2C_PCLK, 0);
		
		priv->dmac_idx = 0;
		priv->dma_ch_tx = 0;
		priv->dma_ch_rx = 1;
		priv->dma_req_tx = DMA_REQ_I2C0_TX;
		priv->dma_req_rx = DMA_REQ_I2C0_RX;
		priv->int_vector = INT_I2C0;
	} 
	else if (port == 1)
	{
		dw_i2c_init(priv, I2C1_BASE, I2C_PCLK, 0);
		
		priv->dmac_idx = 1;
		priv->dma_ch_tx = 0;
		priv->dma_ch_rx = 1;
		priv->dma_req_tx = DMA_REQ_I2C1_TX;
		priv->dma_req_rx = DMA_REQ_I2C1_RX;
		priv->int_vector = INT_I2C1;
	} 
	else if (port == 2) 
	{
		dw_i2c_init(priv, I2C2_BASE, I2C_PCLK, 0);
#ifndef	TAPEOUT_V2	
		priv->dmac_idx = 2;
#else
	    priv->dmac_idx = 0;
#endif
		priv->dma_ch_tx = 0;
		priv->dma_ch_rx = 1;
		priv->dma_req_tx = DMA_REQ_I2C2_TX;
		priv->dma_req_rx = DMA_REQ_I2C2_RX;
		priv->int_vector = INT_I2C2;
	} 
	else 
	{
		info("wrong device id\n");
		ret_val = 1;
	}

	return ret_val;
}

int i2c_test_cleanup(int port)
{
	struct dw_i2c *priv = p_i2c_dev[port];
	struct dw_dmac* p_dma = &dma_dev_list[priv->dmac_idx];

	dw_dmac_channel_select(p_dma, priv->dma_ch_rx);
	dw_dmac_channel_select(p_dma, priv->dma_ch_tx);

	return 0;
}

#define I2C_SAR_DEF_VAL	0x55
bool i2c_reset_test(E_I2C_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_IP_RST ip;
	struct dw_i2c *priv;
	uint32_t reg_val;

	info("%s: port:%d\n", __func__, port);

	priv = p_i2c_dev[port];
	i2c_test_prepare(port);	

	// to set speed cltr must be disabled 
	reg_val = dw_i2c_readl(priv, DW_IC_ENABLE);
	reg_val &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, reg_val, DW_IC_ENABLE);

	// write register
	info("write sar reg %x\n", priv->regs);
	dw_i2c_writel(priv, *p_wr_val, DW_IC_SAR);

	// read&compare
	info("read&compare sar reg\n");
	*p_rd_val = dw_i2c_readl(priv, DW_IC_SAR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("i2c reset\n");
	if      (port == I2C0) 	ip = ARM_I2C0_RST;
	else if (port == I2C1)	ip = ARM_I2C1_RST;
	else					ip = ARM_I2C2_RST;	
	prcm_reset(ip);

	// to set speed cltr must be disabled 
	reg_val = dw_i2c_readl(priv, DW_IC_ENABLE);
	reg_val &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, reg_val, DW_IC_ENABLE);
	
	// read register
	info("read&compare sar reg\n");
	*p_rd_val = dw_i2c_readl(priv, DW_IC_SAR);
	if (*p_rd_val != I2C_SAR_DEF_VAL)
	{
		return false;
	}	

	return true;
}

bool i2c_cg_test(E_I2C_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk;
	struct dw_i2c *priv;
	uint32_t reg_val;

	info("%s: port:%d\n", __func__, port);

	if      (port == I2C0) 	blk = ARM_BLK_CG_I2C0;
	else if (port == I2C1)	blk = ARM_BLK_CG_I2C1;
	else					blk = ARM_BLK_CG_I2C2;	

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	priv = p_i2c_dev[port];
	i2c_test_prepare(port);	

	// to set speed cltr must be disabled 
	reg_val = dw_i2c_readl(priv, DW_IC_ENABLE);
	reg_val &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, reg_val, DW_IC_ENABLE);

	// write register
	info("write sar reg %x\n", priv->regs);
	dw_i2c_writel(priv, *p_wr_val, DW_IC_SAR);

	// read&compare
	info("read&compare sar reg\n");
	*p_rd_val = dw_i2c_readl(priv, DW_IC_SAR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write sar reg %x\n", priv->regs);
	dw_i2c_writel(priv, (*p_wr_val+1), DW_IC_SAR);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// to set speed cltr must be disabled 
	reg_val = dw_i2c_readl(priv, DW_IC_ENABLE);
	reg_val &= ~IC_ENABLE_0B;
	dw_i2c_writel(priv, reg_val, DW_IC_ENABLE);

	// read register
	info("read&compare sar reg\n");
	*p_rd_val = dw_i2c_readl(priv, DW_IC_SAR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}	

	return true;
}
#else
bool i2c_reset_test(E_I2C_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool i2c_cg_test(E_I2C_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif


