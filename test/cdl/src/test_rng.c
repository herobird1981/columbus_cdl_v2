/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_rng.h"
#include "memleak.h"

#if (CONFIG_RNG_TEST == 1)

cmdline_entry rng_test_menu[];

#define MAX_NUM 400000
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

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&rng_test_menu[0]);
}
static int rng_manual(int argc, char* argv[])
{
	char cc = 0;
	int i = 0;
	int latch_mode = 0;

	info("rng_manual: press any key to show 16 byte random data,'q' to quit\n");

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: rng_manual latch_mode\n");
		return 0;
	}

	// Get the parameters
	latch_mode = get_arg_ulong(argv[1]);
	
	rng_init(RNG_BASE, latch_mode);

	while (1) {

		if (console_tstc()) 
		{
			if ('q' == console_getc()) 
			{
				break;
			}
		}		

		rng_rand_buffer(test_send_buff,16);
		for (i=0;i<16;i++)
			info("%02x ",test_send_buff[i]);
		info("\n");
	}

	info("rng_manual: test terminate\n");
	
	return 0;
}

static int rng_manual_more(int argc, char* argv[])
{
	int i=0, j=0;
	int *rand_buf;
	int latch_mode = 0;

	info("rng_manual_more: generate 1600K bytes\n");

	if (argc < 2) 
	{
		info("wrong cmd_format: rng_manual_more latch_mode\n");
		return 0;
	}
	// Get the parameters
	latch_mode = get_arg_ulong(argv[1]);	

	rng_init(RNG_BASE, latch_mode);

    rand_buf = (int*)malloc(MAX_NUM*sizeof(int));
	if(rand_buf == NULL)
	{
		info("rng malloc error\n");
		goto __end0;
	}

	rng_rand_buffer(rand_buf,MAX_NUM*4);
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
	dbg_mem_stat();
	return 0;
}


static int rng_quality(int argc, char* argv[])
{
	int *rand_buf;
	int i;
	int ev_mode=0;
	int latch_mode = 0;	

	info("rng quality evaluation:  rng_quality latch_mode ev_mode\n");
	info("ev_mode:0 hw | 1 sw pseudo-random | 2 sw pseudo-random change seed per rand\n");
	info("please wait for several minutes to finish this test case\n");

	if(argc < 2) {
		info("wrong cmd_format: rng_quality latch_mode ev_mode\n");
		return 0;
	}

	latch_mode = get_arg_ulong(argv[1]);
	
	if (argc > 2) {
		ev_mode = get_arg_ulong(argv[2]);
	}

	if (ev_mode > 2 || ev_mode < 0)
	{
		info("Valid mode number is: 0/1/2");
		return 0;
	}
	rng_init(RNG_BASE, latch_mode);

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
		rng_rand_buffer(rand_buf,MAX_NUM*4);
		for (i=0;i<MAX_NUM;i++) {
			*(rand_buf+i) = (*(rand_buf+i))%65535;
		}
	}
	test_rand(rand_buf,MAX_NUM,0);
__end0:
	free(rand_buf);
	dbg_mem_stat();
	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry rng_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "rng_manual",rng_manual, "   : random generator manual check" },
	{ "rng_manual_more",rng_manual_more, "   : random generator manual check" },
	{ "rng_quality",rng_quality,  "  : random generator quality check" },
	{ 0, 0, 0 }
};

#define RNG_CTRL_DEF_VAL	0x0
bool rng_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	struct brite_rng *p_dev = p_rng;
	
	info("%s\n", __func__);
	rng_init(RNG_BASE, 0);
	
	info("write rng ctrl reg\n");
	
	// write register
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + RNG_CTRL);

	// read&compare
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + RNG_CTRL);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("rng reset\n");
	prcm_reset(ARM_RNG_RST);

	// read register
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + RNG_CTRL);
	if (*p_rd_val != RNG_CTRL_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool rng_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_RNG;
	struct brite_rng *p_dev = p_rng;
	
	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	rng_init(RNG_BASE, 0);
	
	// write register
	info("write rng ctrl reg\n");
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + RNG_CTRL);

	// read&compare
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + RNG_CTRL);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write rng ctrl reg\n");
	__raw_writel((*p_wr_val+1), (uint32_t)p_dev->regs + RNG_CTRL);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare rng ctrl reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + RNG_CTRL);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool rng_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool rng_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

