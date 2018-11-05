/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2017 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_dwc_trng.h"
#include "cdl_dwc_trng.h"
#include "memleak.h"

#if (CONFIG_DWC_TRNG_TEST == 1)

/* enable or disable the KAT test */
#define DWC_TRNG_KAT_ENABLE 1


#define MAX_NUM (12*1024*0124) //400000
#define MAX_READ 1024

struct dwc_trng_priv dwc_trng_dev;
struct dwc_trng_priv *p_dwc_trng_dev = &dwc_trng_dev;


static uint32_t cv_seed_data[8] = {
  0x56789ABC,
  0x6789ABCD,
  0x789ABCDE,
  0x89ABCDEF,
  0x12345678,
  0x23456789,
  0x3456789A,
  0x456789AB
};

static uint32_t cv_rng_out[4] = {
  0xB9076EF0,
  0xA126CAB,
  0xF1B20E14,
  0xD202B674
};

cmdline_entry dwc_trng_test_menu[];

static int cmd_help(int argc, char *argv[])
{
	return cmdline_help_general(&dwc_trng_test_menu[0]);
}

static int test_rand(int a[],int num,int style)
{
	float *b;
	float ans;
	int i,t;
	int l,w;
	float pi=3.1415926535898;
	float diff;
	unsigned long diff_int;

	b = (float*)malloc(MAX_NUM*sizeof(int));
	if(b == NULL)
	{
		info("rng malloc error\n");
		goto __end0;
	}

	for(i=0;i<num;i++)
	{
		b[i]=(float)a[i]/65535.0;
	}

	l=0;
	w=0;
	t=num/2;
	for(i=0;i<t;i++)
	{
		if( b[i]*b[i]+b[i+t]*b[i+t]<= 1 )/*x^2+y^2>1 ==>pi/4*/
			l++;
		else w++;
	}
	ans=(float)l/(float)(l+w)*4;
	diff = ans-pi;
	if (diff<0)
		diff = -diff;
	diff = diff/pi;
	diff_int = (unsigned long)(diff*1000000);
	info("diff with pi = %d x 10^(-6)\n",diff_int);
__end0:
	free(b);

	return 0;
}

static int 	trng_manual(int argc, char* argv[])
{
	char cc = 0;
	int i = 0;

	info("trng_manual: press any key to show 16 byte random data,'q' to quit\n");

	// Check the validity of parameter count
	if (argc < 1) 
	{
		info("wrong cmd_format: rng_manual\n");
		return 0;
	}

	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);
	dwc_trng_set_nonce(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_addin_present(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_pred_resist(p_dwc_trng_dev, 0, 1);
	dwc_trng_cmd_seed(p_dwc_trng_dev, NULL, 1);
    dwc_trng_cmd_create_state(p_dwc_trng_dev, NULL, 1);
	while (1) {
#if 1
		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
				break;
		}
#endif
		dwc_trng_cmd_gen_random(p_dwc_trng_dev, 1, (uint32_t *)test_send_buff, 1);
		for (i=0;i<16;i++)
			info("%02x ",test_send_buff[i]);
		info("\n");
	}

	info("trng_manual: test terminate\n");
	return 0;
}

static int trng_manual_more(int argc, char* argv[])
{
	int i=0, j=0;
	int *rand_buf;

	info("rng_manual_more: generate 1600K bytes\n");
    info("please wait for several minutes to finish this test case\n");
	
	if (argc < 1) 
	{
		info("wrong cmd_format: rng_manual_more\n");
		return 0;
	}

	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);
	dwc_trng_set_nonce(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_addin_present(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_pred_resist(p_dwc_trng_dev, 0, 1);
	dwc_trng_cmd_seed(p_dwc_trng_dev, NULL, 1);
    dwc_trng_cmd_create_state(p_dwc_trng_dev, NULL, 1);


    rand_buf = (int*)malloc(MAX_NUM*sizeof(int));
	if(rand_buf == NULL)
	{
		info("rng malloc error\n");
		goto __end0;
	}

	dwc_trng_cmd_gen_random(p_dwc_trng_dev, MAX_NUM/4, (uint32_t *)rand_buf, 1);
		
	for (i=0;i<MAX_NUM;i++){
		for(j=0; j<32; j++){
			
			info("%01x",rand_buf[i]&0x01);
			rand_buf[i] = rand_buf[i]>>1;
		}
	}
	info("\n");
__end0:	
	free(rand_buf);

	info("rng_manual_more: test terminate\n");
	return 0;
}


static int trng_quality(int argc, char* argv[])
{
	int *rand_buf;
	int i;
	int ev_mode=0;

	info("rng quality evaluation:  rng_quality ev_mode\n");
	info("ev_mode:0 hw | 1 sw pseudo-random | 2 sw pseudo-random change seed per rand\n");
	info("please wait for several seconds to finish this test case\n");

	if(argc < 2) {
		info("wrong cmd_format: rng_quality ev_mode\n");
		return 0;
	}
	
	ev_mode = get_arg_ulong(argv[1]);

	if (ev_mode > 2 || ev_mode < 0)
	{
		info("Valid mode number is: 0/1/2");
		return 0;
	}

	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);
	dwc_trng_set_nonce(p_dwc_trng_dev, 1, 1);
	dwc_trng_set_addin_present(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_pred_resist(p_dwc_trng_dev, 0, 1);
	dwc_trng_cmd_seed(p_dwc_trng_dev, NULL, 1);
    dwc_trng_cmd_create_state(p_dwc_trng_dev, NULL, 1);

	rand_buf = (int*)malloc(MAX_NUM*sizeof(int));
	if(rand_buf == NULL)
	{
		info("rng malloc error\n");
		goto __end0;
	}	

	info("using mode %d\n",ev_mode);
	if (ev_mode>=1) {
		srand(get_ticks());
		for (i=0;i<MAX_NUM;i++) {
			if (ev_mode==2)
				srand(get_ticks());
			*(rand_buf+i) = cb_rand()%65535;
		}
	}else {
		dwc_trng_cmd_gen_random(p_dwc_trng_dev, MAX_NUM/4, (uint32_t *)rand_buf, 1);
		for (i=0;i<MAX_NUM;i++) {
			*(rand_buf+i) = (*(rand_buf+i))%65535;
		}
	}
	test_rand(rand_buf,MAX_NUM,0);
__end0:
	free(rand_buf);
	return 0;
}

static volatile uint32_t g_dwc_trng_int_flag = 0;
void dwc_trng_int_test_ISR(void* ptr)
{
	struct dwc_trng_priv *p_dev = (struct dwc_trng_priv *)ptr;
	uint32_t int_sts = 0;

	info("dwc_trng_int_test_ISR!\n");
	int_sts = dwc_trng_get_interrupt_status(p_dev);
	dwc_trng_clear_int_status(p_dev, int_sts);
	dwc_trng_enable_interrupt(p_dev, 0);
	info("int_sts:%x\n", int_sts);
	if (int_sts & DWC_TRNG_REG_ISTAT_DONE) 
	{
		info("DWC_TRNG_REG_ISTAT_DONE:%x\n", DWC_TRNG_REG_ISTAT_DONE);
		g_dwc_trng_int_flag = 1;
	}
		
	if (int_sts & DWC_TRNG_REG_ISTAT_ALARMS) 
	{
		info("DWC_TRNG_REG_ISTAT_ALARMS:%x\n", DWC_TRNG_REG_ISTAT_ALARMS);
		g_dwc_trng_int_flag = 1;
	}

	// tx fifo not empty when request read,clr the interrupt
	if (int_sts & DWC_TRNG_REG_ISTAT_KAT_COMPLETE) 
	{
		info("DWC_TRNG_REG_ISTAT_KAT_COMPLETE:%x\n", DWC_TRNG_REG_ISTAT_KAT_COMPLETE);
		g_dwc_trng_int_flag = 1;
	}
		
	if (int_sts & DWC_TRNG_REG_ISTAT_ZEROIZE) 
	{
		info("DWC_TRNG_REG_ISTAT_ZEROIZE:%x\n", DWC_TRNG_REG_ISTAT_ZEROIZE);		
		g_dwc_trng_int_flag = 1;
	}

	return;
}

static int trng_int(int argc, char* argv[])
{
	int int_bit=0;
	int loop = 0;
	uint32_t dbg = 0;

	info("trng int test:  trng_int int_bit\n");

	if(argc < 2) {
		info("wrong cmd_format: trng_int int_bit\n");
		return 0;
	}
	
	int_bit = get_arg_ulong(argv[1]);

	if (int_bit != 0 && int_bit != 4)
	{
		info("int_bit shuold be: 0/4\n");
		return 0;
	}

	g_dwc_trng_int_flag = 0;
	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);

	
	// Register int handler and enable it
	irq_generic_install_handler(p_dwc_trng_dev->int_vector, dwc_trng_int_test_ISR, p_dwc_trng_dev);
	irq_generic_config_int(p_dwc_trng_dev->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(p_dwc_trng_dev->int_vector);	

	// Enable interrupt
	dwc_trng_enable_interrupt(p_dwc_trng_dev, 1<<int_bit);

	dwc_trng_cmd_zeroize(p_dwc_trng_dev, 1);
	dwc_trng_set_nonce(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_addin_present(p_dwc_trng_dev, 0, 1);
	dwc_trng_set_pred_resist(p_dwc_trng_dev, 0, 1);	
	dwc_trng_cmd_seed(p_dwc_trng_dev, NULL, 1);
    dwc_trng_cmd_create_state(p_dwc_trng_dev, NULL, 1);	
	dwc_trng_cmd_gen_random(p_dwc_trng_dev, 1, (uint32_t *)test_send_buff, 1);

	// wait interrupt handle, and wait user break test case
	while (g_dwc_trng_int_flag == 0) 
	{
		mdelay(1000);
		if (loop++ > 3)
			break;
	}
	
	dbg = dwc_trng_get_interrupt_status(p_dwc_trng_dev);

	if (g_dwc_trng_int_flag == 0) 
		info("%s: test fail\n", argv[0]);
	else 
		info("%s: test pass\n", argv[0]);

	// disable interrupt
	irq_generic_disable_int(p_dwc_trng_dev->int_vector);
	irq_generic_remove_handler(p_dwc_trng_dev->int_vector, NULL, NULL);
    dwc_trng_enable_interrupt(p_dwc_trng_dev, 0);	

	return 0;
}

#if (DWC_TRNG_KAT_ENABLE)
#if 0
uint32_t trng_basic_test(int argc, char* argv[])
{
	int i = 0;
	
	info("trng basic test:\n");

	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);

	/* step 1: get random data*/
	info("Test 1: Should be able to read random data\n");
	dwc_trng_cmd_gen_random(p_dwc_trng_dev, 1, (uint32_t *)test_send_buff, 1);
	for (i=0;i<16;i++)
		info("%02x ",test_send_buff[i]);
	info("\n");

	/* step 2: Given different initial nonce seeds the rng data is different*/
    info("Test 2: Given different initial nonce seeds the rng data is different\n");
	
	/* step 3: Given the same initial nonce seed the rng data is the same*/
    info("Test 3: Given the same initial nonce seed the rng data is the same\n");
	
	if(0 == alarms)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}
#endif

int trng_kat_test(int argc, char* argv[])
{
	int kat_sel=0, kat_vec=0;
	int32_t alarms = 0;

	info("trng kat test:\n");

	if(argc < 3) {
		info("wrong cmd_format: trng_kat_test kat_sel kat_vec\n");
		info("kat_sel:0/1, kat_vec:0/1\n");
		return 0;
	}
	
	kat_sel = get_arg_ulong(argv[1]) & 0x1;
	kat_vec = get_arg_ulong(argv[2]) & 0x1;

	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);
	dwc_trng_cmd_kat(p_dwc_trng_dev, kat_sel, kat_vec, &alarms, 1);

	if(0 != alarms)
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}
#endif

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry dwc_trng_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "trng_manual",trng_manual, "   : random generator manual test" },
	{ "trng_manual_more",trng_manual_more, "   : random generator manual test" },
	{ "trng_quality",trng_quality,  "  : random generator quality test" },
	{ "trng_int",trng_int,  "  : trng interrupt test" },
    { "trng_kat",trng_kat_test,  "  : trng kat test" },	
	{ 0, 0, 0 }
};

#define TRNG_CTRL_DEF_VAL	0x0
bool dwc_trng_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	info("%s\n", __func__);
	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);
	
	info("write rng ctrl reg\n");
	
	// write register
	__raw_writel(*p_wr_val, (uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);

	// read&compare
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("rng reset\n");
	prcm_reset(ARM_RNG_RST);
	
	// read register
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);
	if (*p_rd_val != TRNG_CTRL_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool dwc_trng_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_RNG;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	dwc_trng_init(p_dwc_trng_dev, (uint32_t *)DWC_TRNG_BASE, INT_DWC_TRNG, MAX_READ);
	
	// write register
	info("write rng ctrl reg\n");
	__raw_writel(*p_wr_val, (uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);

	// read&compare
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

#if 0
	// write register
	info("write rng ctrl reg\n");
	__raw_writel((*p_wr_val+2), (uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);
#else
	info("write rng ctrl reg not supported after clock gate disable\n");	
#endif

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dwc_trng_dev->base + DWC_TRNG_REG_MODE);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool dwc_trng_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool dwc_trng_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif


