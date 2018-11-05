/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_adc.h"
#include "cdl_pll.h"
#include "cdl_adc.h"

#if (CONFIG_ADC_TEST == 1)

cmdline_entry adc_test_menu[];

#define CHANNEL_MASK 0x0f  //  only 4 bit valid
#define CHANNEL_COUNT 4

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&adc_test_menu[0]);
}

static unsigned FindOneInNumber(unsigned int x)
{
	const unsigned MASK1 = 0x55555555;
	const unsigned MASK2 = 0x33333333;
	const unsigned MASK4 = 0x0f0f0f0f;
	const unsigned MASK8 = 0x00ff00ff;
	const unsigned MASK16 = 0x0000ffff;

	x = (x&MASK1 ) + (x>>1 &MASK1 );
	x = (x&MASK2 ) + (x>>2 &MASK2 );
	x = (x&MASK4 ) + (x>>4 &MASK4 );
	x = (x&MASK8 ) + (x>>8 &MASK8 );
	x = (x&MASK16) + (x>>16&MASK16);
	return x;
}

static void multi_ch_enable(unsigned char ch_flag, unsigned int enable)
{
	int i = 0;

	for(i=0; i<CHANNEL_COUNT; i++)
	{
		if(ch_flag & (1<<i))
			adc_enable_channel(i, enable);	
	}
}

static int adc_single_mode_test(int argc, char* argv[])
{
	unsigned int adc_value[4] = {0};
	int brepeat = 0;
	int adc_clk = 0;
	int clk_sel = 0;
	int ch_bit = 1;
	int i = 0;
	int data_cnt = 0;
	int ch_cnt = 0;

	info("adc_sm: single mode test\n");

	if (argc < 5) {
		info("wrong cmd_format: %s clk_sel adc_clk ch_bit repeat\n", argv[0]);
		return 0;
	}

    // 0 : apb clock / div, non-zone: 38.4M
    #if 0
    clk_sel = get_arg_ulong(argv[1]);
	if(clk_sel == 1)
	{
		info("fail: for arm, clk sel must be 0\r");
		return 0;
	}	
	#else
	clk_sel = 0;
	#endif

    adc_clk= get_arg_ulong(argv[2]);
	ch_bit = get_arg_ulong(argv[3]) & CHANNEL_MASK;
	if(0 == ch_bit)
		ch_bit = 1;
	ch_cnt = FindOneInNumber(ch_bit);
	
	brepeat = get_arg_ulong(argv[4]);

	adc_init(ADC_BASE, ADC_PCLK, adc_clk, clk_sel, 0, 1);
	adc_set_mode(ADC_MODE_SINGLE);
	while(1)
	{	    
		multi_ch_enable(ch_bit, 1);
		adc_enable(1);
		adc_start(1);	
		data_cnt = adc_read_single_data(adc_value, ch_cnt);        
        adc_start(0);	

		multi_ch_enable(ch_bit, 0);
		adc_enable(0);
		info("adc_sm:");
		if(0 == data_cnt)
			info("no data valid in this round\n");
		else
		{
			for(i=0; i<ch_cnt; i++)
			{
				info("%04d ", adc_value[i]);
			}		
			info("\n");
		}

		if(brepeat == 0 || console_check_char('q'))
			break;
	}	
	
	info("please judge pass or fail according adc hardware configuration\n");
	return 0;
}

static int adc_continue_mode_test(int argc, char* argv[])
{
	unsigned int ch_bit = 0;
	int i = 0, j = 0;
	int adc_clk = 0;
	int clk_sel = 0;
	unsigned int *saradc_buf = (unsigned int*)test_recv_buff;
	unsigned int buf_len = sizeof(test_recv_buff);	
	int ch_cnt = 0;

	info("adc_cs: continuous mode test\n");

	if (argc < 4) {
		info("wrong cmd_format: %s clk_sel adc_clk ch_bit\n", argv[0]);
		return 0;
	}

#if 0
    clk_sel = get_arg_ulong(argv[1]);
	if(clk_sel == 1)
	{
		info("fail: for arm, clk sel must be 0\r");
		return 0;
	}
#else
	clk_sel = 0;
#endif

	adc_clk= get_arg_ulong(argv[2]);
	ch_bit = get_arg_ulong(argv[3]) & CHANNEL_MASK;
	if(0 == ch_bit)
		ch_bit = 1;
	ch_cnt = FindOneInNumber(ch_bit);
    adc_init(ADC_BASE, ADC_PCLK, adc_clk, clk_sel, 1, 1);
	adc_set_mode(ADC_MODE_CONTINUOUS);
	memset(saradc_buf, 0, buf_len);
	multi_ch_enable(ch_bit, 1);
	adc_enable(1);
	adc_read_cont_data((unsigned long*)saradc_buf, 2048);
	adc_start(1);
	if(adc_wait_dma_transfer_done()!= 0)
	{
		info("adc dma error\r\n");
	}
	
    adc_start(0);
	multi_ch_enable(ch_bit, 0);
	adc_enable(0);
	info("adc_cs data:\n");

	for(j=0; j<2048/ch_cnt; j++)
	{
		for(i=0; i<ch_cnt; i++)
		{		
			info("%04d	", (*(saradc_buf+ j*ch_cnt + i)) & 0xfff);				
		}	
		info("\n");
	}
	info("please judge pass or fail according adc hardware configuration\n");
	return 0;
}

static int adc_int_counter = 0;
void adc_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = adc_get_int_status();
	if (int_status != 0x0) {
		adc_int_counter++;
		adc_clear_interrupt(int_status);
		adc_enable_interrupt(0);
	}
	info("adc_int_counter,status=%x\n", int_status);
}

/*testcase:adc interrupt test*/
static int adc_int(int argc, char* argv[])
{
	int fail = 0;
	int ch = 0;
	int bit = 0;

	info("%s: interrupt handle test,input 'q' to quit this testcase\n",
		argv[0]);

	adc_int_counter = 0;

	if (argc < 2) {
		info("wrong cmd_format: %s trig_bit(0~3 & 8~9)\n", argv[0]);
		return 0;
	}
	
	bit = get_arg_ulong(argv[1]);
	if((bit<0) || (bit >9) || (bit>=4 && bit<=7)){
		info("trig bit invalid (valid bit: 0~3 & 8~9)\n");
		return 0;
	}

	if(bit <= 3)
		ch = bit;

	adc_init(ADC_BASE, ADC_PCLK, 1200000, 0, 0, 1);
	adc_set_mode(ADC_MODE_CONTINUOUS);
	adc_enable_channel(ch, 1);
	adc_enable(1);

	/*register int handler and enable it*/
	irq_generic_install_handler(INT_ADC, adc_int_handler, NULL);
	irq_generic_config_int(INT_ADC, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_ADC);

	/*enable interrupt ch0*/
	adc_enable_interrupt(1<<bit);

	//wait interrupt handle, and wait user break testcase
	adc_start(1);

	mdelay(1000);

	if (adc_int_counter == 0) {
		fail++;
	}

	if (fail) {
		info("%s: test fail\n", argv[0]);
	} else {
		info("%s: test pass\n", argv[0]);
	}

    adc_start(0);
	adc_enable_channel(ch, 0);
	adc_enable(0);

	/*disable interrupt*/
	irq_generic_disable_int(INT_ADC);
	irq_generic_remove_handler(INT_ADC, NULL, NULL);
	adc_enable_interrupt(0);

	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry adc_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "adc_sm",adc_single_mode_test, 	 "  : adc single mode multi-channel test" },
	{ "adc_cm",adc_continue_mode_test,	 "  : adc continuous mode mult-chanel test" },
	{ "adc_int",adc_int,	 "  : adc interrupt test" },{ 0, 0, 0 }
};

#define ADC_COUNT_DEF_VAL	0x1E
bool adc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	struct sar_adc* p_dev = p_adc;

	info("%s\n", __func__);
	adc_init(ADC_BASE, ADC_PCLK, 10000000, 0, 0, 1);
	
	// write register
	info("write count reg\n");
	adc_write(p_dev, ADC_COUNT, *p_wr_val);

	// read&compare
	info("read&compare count reg\n");
	*p_rd_val = adc_read(p_dev, ADC_COUNT);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("adc reset\n");
	prcm_reset(ARM_SARADC_CTRL_RST);
	
	// read register
	info("read&compare count reg\n");
	*p_rd_val = adc_read(p_dev, ADC_COUNT);
	if (*p_rd_val != ADC_COUNT_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool adc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_SARADC;
	struct sar_adc* p_dev = p_adc;

	info("%s\n", __func__);
	
	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	adc_init(ADC_BASE, ADC_PCLK, 10000000, 0, 0, 1);
	
	// write register
	info("write count reg\n");
	adc_write(p_dev, ADC_COUNT, *p_wr_val);

	// read&compare
	info("read&compare count reg\n");
	*p_rd_val = adc_read(p_dev, ADC_COUNT);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	info("write count reg\n");
	adc_write(p_dev, ADC_COUNT, (*p_wr_val+1));

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare count reg\n");
	*p_rd_val = adc_read(p_dev, ADC_COUNT);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}

#else
bool adc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool adc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}
#endif

