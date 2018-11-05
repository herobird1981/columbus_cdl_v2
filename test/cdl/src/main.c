/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_config.h"
#include "cdl_pll.h"
#include "test_ddr.h"
#include "test_sys.h"
#include "test_gpio.h"
#include "test_i2c.h"
#include "test_spi.h"
#include "test_qspi.h"
#include "test_uart.h"
#include "test_mmc.h"
#include "test_nand.h"
#include "test_timer.h"
#include "test_wdt.h"
#include "test_crc.h"
#include "test_dma.h"
#include "test_ecc.h"
#include "test_sha.h"
#include "test_aes.h"
#include "test_otp.h"
#include "test_ipc.h"
#include "test_rng.h"
#include "test_usb.h"
#include "test_adc.h"
#include "test_rtc.h"
#include "test_epf.h"
#include "test_dwc_trng.h"
#include "memleak.h"

//#define DCACHE_ENABLE

#define MMU_FULL_ACCESS   (3 << 10)
#define MMU_DOMALN        (0 << 5) 
#define MMU_SPECIAL       (1 << 3) 
#define MMU_CACHEABLE     (1 << 2)
#define MMU_BUFFERABLE    (1 << 3)
#define MMU_SECTION       (2)
#define MMU_SECDESC       (MMU_FULL_ACCESS | MMU_DOMALN | MMU_SPECIAL | MMU_SECTION)
#define MMU_SECDESC_WB    (MMU_FULL_ACCESS | MMU_DOMALN | MMU_SPECIAL | MMU_CACHEABLE | MMU_BUFFERABLE | MMU_SECTION)

void create_page_table(void)
{
    unsigned long *ttb = (unsigned long *)0x10010000;
    unsigned long vaddr, paddr;

    vaddr = 0x40000000; 
    paddr = 0x40000000; 

	//memset(0x10010000, 0x0, 16*1024);
    while(vaddr < 0x48000000)
    {
        *(ttb + (vaddr >> 20)) = (paddr & 0xFFF00000) | MMU_SECDESC_WB;
        vaddr += 0x100000;
        paddr += 0x100000;
    }
}

void cache_enable(void)
{
	__asm__(

			// Enable D-side Prefetch
			"MRC p15, 0, r1, c1, c0, 1\n"								// Read Auxiliary Control Register.
			"ORR r1, r1, #(0x1 <<2)\n"								// Enable D-side prefetch.
			"MCR p15, 0, r1, c1, c0, 1\n"								// Write Auxiliary Control Register.
			"DSB\n"
			"ISB\n"		
	
			// Initialize MMU.
			"MOV r1,#0x0\n"
			"MCR p15, 0, r1, c2, c0, 2\n"								// Write Translation Table Base Control Register.
			"LDR r1, =0x10010000\n"
			"MCR p15, 0, r1, c2, c0, 0\n"								// Write Translation Table Base Register 0.
			"DSB\n"
			"ISB\n"		
	
#if 1
			/*
			 * Enable ACTLR.SMP for Cache to work.
			*/
			"mrc	p15, 0, r0, c1, c0, 1\n"
			"mov	r0, #(1 << 6)\n"
			"mcr	p15, 0, r0, c1, c0, 1\n"
			"DSB\n"
			"ISB\n"
#endif

			// Enable MMU /I-CACHE/D-CACHE
			"MRC 	p15, 0, r0, c1, c0, 0\n"	   // Read CP15 System Control register
			"ORR 	r0, r0, #(0x1 << 12)\n"
#ifdef DCACHE_ENABLE
			"ORR 	r0, r0, #(0x1 <<  2)\n"
#endif
			"ORR 	r0, r0, #0x2\n"
#ifdef DCACHE_ENABLE
			"ORR 	r0, r0, #0x1\n"			   // Set M bit 0 to enable MMU before scatter loading
#endif
			"MCR 	p15, 0, r0, c1, c0, 0\n"	   // Write CP15 System Control register
			"DSB\n"
			"ISB\n" 	
	:
	:
	);

}
/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define CMD_BUF_SIZE		64

typedef struct test_func_menu_s {
	cmdline_entry* menu;
	int (*prepare)(int);
	int dev_sel;
	const char* desc;
	int (*cleanup)(int);
} test_func_menu;

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
static char g_cmd_buf[CMD_BUF_SIZE];
static char *g_cmd_buf_endp;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*device list for test fun*/
#if (CONFIG_GIC_TEST ==1)
extern cmdline_entry gic_test_menu[];
#else
#define gic_test_menu NULL
#endif

#if (CONFIG_RNG_TEST ==1)
extern cmdline_entry rng_test_menu[];
#else
#define rng_test_menu NULL
#endif

#if (CONFIG_TAMP_TEST ==1)
extern cmdline_entry tamp_test_menu[];
#else
#define tamp_test_menu NULL
#endif

#if (CONFIG_API_TEST ==1)
extern cmdline_entry APIs_test_menu[];
extern int APIs_test_prepare(int);
extern int APIs_test_cleanup(int);
#else
#define APIs_test_menu NULL
#define APIs_test_prepare NULL
#define APIs_test_cleanup NULL
#endif

#if (CONFIG_PLL_TEST ==1)
extern cmdline_entry pll_test_menu[];
#else
#define pll_test_menu NULL
#endif

#if (CONFIG_BBURAM_TEST ==1)
extern cmdline_entry bburam_test_menu[];
#else
#define bburam_test_menu NULL
#endif


// device list for test fun
const test_func_menu menu[] = {
	{timer_test_menu,	timer_test_prepare,	0, "timer", 			timer_test_cleanup },
	{wdt_test_menu,		wdt_test_prepare,	0, "watch dog timer", 	NULL},
	{rtc_test_menu,		rtc_test_prepare,	0, "rtc",				NULL},
	{dma_test_menu,		dma_test_prepare,	0, "dmac0",				NULL},	
	{dma_test_menu,		dma_test_prepare,	1, "dmac1",				NULL},
	{dma_test_menu,		dma_test_prepare,	2, "dmac2",				NULL},
	{gpio_test_menu,	NULL,				0, "gpio",				NULL},
	{uart_test_menu,	uart_test_prepare,	0, "uart0",				NULL},
	{uart_test_menu,	uart_test_prepare,	1, "uart1",				NULL},
	{uart_test_menu,	uart_test_prepare,	2, "uart2",				NULL},
	{uart_test_menu,	uart_test_prepare,	3, "uart3",				NULL},
	{i2c_test_menu,		i2c_test_prepare,	0, "i2c0",				i2c_test_cleanup},
	{i2c_test_menu,		i2c_test_prepare,	1, "i2c1",				i2c_test_cleanup},
#if defined(EVB_BOARD)
	{i2c_test_menu,		i2c_test_prepare,	2, "i2c2",				i2c_test_cleanup},
#endif
	{qspi_test_menu,	NULL,				0, "spi0(qspi)",		NULL},
	{spi_test_menu,		spi_test_prepare,	1, "spi1",				NULL},
	{spi_test_menu,		spi_test_prepare,	2, "spi2",				NULL},
	{mmc_test_menu,		mmc_test_prepare,	0, "mmc/sdio 0",		NULL},
#if defined(EVB_BOARD)
	{mmc_test_menu,		mmc_test_prepare,	1, "mmc/sdio 1",		NULL},
#endif
	{nand_test_menu,	NULL,				0, "nandflash",			NULL},
	{usb_test_menu,		usb_test_prepare,	0, "usb0",				NULL},
#if defined(EVB_BOARD)
	{usb_test_menu,		usb_test_prepare,	1, "usb1",				NULL},
#endif
	{otp_test_menu,		NULL,				0, "otp",				NULL},
	{crc_test_menu,		NULL,				0, "crc",				NULL},
	{adc_test_menu,		NULL,				0, "adc",				NULL},
	{ecc_test_menu,		NULL,				0, "ecc",				NULL},

#ifdef TAPEOUT_V2	
	{dwc_trng_test_menu, 	NULL, 			0, "dwc_trng",			NULL},	
#else
	{rng_test_menu, 	NULL,				0, "rng",				NULL},
#endif
	{aes_test_menu,		NULL,				0, "aes",				NULL},
	{sha_test_menu,		NULL,				0, "sha",				NULL},
	{ipc_test_menu,		ipc_test_prepare,	0, "ipc",				NULL},
	{epf_test_menu,		NULL,				0, "epf",				NULL},
	{tamp_test_menu,	NULL,				0, "tamper",			NULL},
	{bburam_test_menu,	NULL,				0, "bburam",			NULL},
	{APIs_test_menu, 	APIs_test_prepare,	0, "api",				APIs_test_cleanup},
	{pll_test_menu,		NULL,				0, "pll",				NULL},
	{ddr_test_menu, 	ddr_test_prepare,	0, "ddr",				NULL},
	{sys_test_menu, 	NULL, 				0, "sys",				NULL}
};

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
int one_device_test(int idx)
{
	int ret;

	/*display the help,list all support test command for the device*/
	info("device: %s test case\n", menu[idx].desc);
	ret = cmdline_process("help", menu[idx].menu);

	while (1) 
	{
		/* Print a prompt to the console.  Show the CWD.*/
		info("\n%s> ", "cmd:");

		/*Get a line of text from the user.*/
		console_gets(g_cmd_buf, sizeof(g_cmd_buf));

		/* Pass the line from the user to the command processor.*/
		/* It will be parsed and valid commands executed.*/
		ret = cmdline_process(g_cmd_buf, menu[idx].menu);

		/* Handle the case of bad command.*/
		if (ret == CMDLINE_QUIT) 
		{
			info("Quit %s test\n", menu[idx].desc);				
			break;
		} 
		if (ret == CMDLINE_BAD_CMD)
			info("Bad command!\n");
		else if (ret == CMDLINE_TOO_MANY_ARGS) 
			info("Too many arguments for command processor!\n");
		else if (ret != 0) 
			info("Command returned error code %d\n", ret);
	}

	if (menu[idx].cleanup != NULL) 
		(menu[idx].cleanup)(menu[idx].dev_sel);
	
	return 0;
}


extern unsigned long _malloc;
extern unsigned long _emalloc;
extern unsigned long _bss;
extern unsigned long _ebss;

static void stack_init(void)
{
	unsigned long *dst;

	/*zero bss*/
	for(dst = (unsigned long *)&_bss; dst < (unsigned long *)&_ebss; dst++) {
	      *dst = 0x0;
	}
}

static void heap_init(void)
{
	unsigned long *dst;

	/*zero bss*/
	for(dst = (unsigned long *)&_malloc; dst < (unsigned long *)&_emalloc; dst++) {
	      *dst = 0x0;
	}
}


int main(void)
{
	int i;
	int j;
	int ret;
	unsigned long usel = 0;
	stack_init();
	heap_init();
	dbg_zero_stat();
#ifdef DCACHE_ENABLE
	create_page_table();
#endif
   	cache_enable();

#if defined(EVB_BOARD)
	/*init*/
	brite_pll_init(PRCM_BASE);
	brite_set_all_clock_default();
#endif

	/*init console first*/
	console_init(CONSOLE_BASE, CONSOLE_PCLK);
	clk_register_callback(&arm_uart_clk, console_pclk_change_cb);
#if defined(EVB_BOARD)
    debug("ARM CDL V%s %s...with hskp_0 in initial(EVB)\n", __DATE__, __TIME__);
#else
	//debug("ARM CDL v2017.10.24(FPGA)\n");
	debug("ARM CDL V%s %s...(FPGA)\n", __DATE__, __TIME__);
#endif
	brite_get_all_clock_rate();

	wdt_init(WDT_BASE, WDT_PCLK, 0x0);
	wdt_stop();

#if 1 //clear arm house keeping done
	prcm_pwd_hs_keeping_ctrl(ARM_PWD_HS_KEEPING_PENDING);
#endif
	gtimer_timer_init(TIMER_BASE, TIMER_PCLK, SYS_TIMER);
	ipc_timer_init(A7_IPC_REG_BASE, ARM_AHB_CLK);	/* alternative system timer */
	clk_register_callback(&arm_apb1_clk, system_timer_pclk_change_cb);

	misc_var_init(MISC_BASE);
	prcm_var_init(PRCM_BASE);
	
#if (CONFIG_DRIVERS_GIC==1)
	irq_generic_init(GIC_BASE,GIC_CPU_BASE);
	enable_interrupts();
#endif

	/*init dmac*/
	dw_dmac_init(&dma_dev_list[0], DMAC0_BASE,INT_DMA0);
	dw_dmac_init(&dma_dev_list[1], DMAC1_BASE,INT_DMA1);
	dw_dmac_init(&dma_dev_list[2], DMAC2_BASE,INT_DMA2);

	//Driver strength force to 8mA
	misc_uart_ds(UART0, IO_DS_8MA);
	misc_uart_ds(UART1, IO_DS_8MA);
	misc_uart_ds(UART2, IO_DS_8MA);
	misc_uart_ds(UART3, IO_DS_8MA);
	misc_i2c_ds(I2C0, IO_DS_8MA);
	misc_i2c_ds(I2C1, IO_DS_8MA);
	misc_i2c_ds(I2C2, IO_DS_8MA);

	misc_clkout_pin_init(IO_PU_UP, IO_PD_EN);

	/*init dynmaic memory pool*/
	mem_malloc_init((unsigned long)&_malloc, (unsigned long) &_emalloc - (unsigned long) &_malloc);

	while (1) 
	{
		info("\nColumbus CDL test,please select device first\n");

		/*display test device list*/
		for (i = 0; i < ARRAY_SIZE(menu); i++) {
			if (menu[i].menu == 0) 
				info("%d: %s  %s\r\n", i, menu[i].desc, "!!! not implemented");
			else 
				info("%d: %s  %s\r\n", i, menu[i].desc, "");
		}

		/*get user selection and run the test*/
		info("\r\nSelect the device to test : ");
		console_gets(g_cmd_buf, sizeof(g_cmd_buf));
		usel = (unsigned long)simple_strtoul((const char*)g_cmd_buf, &g_cmd_buf_endp, 10);
		if(g_cmd_buf == g_cmd_buf_endp) {
			usel = -1;
			for(j=0; j<ARRAY_SIZE(menu); j++) {
				if(menu[j].menu && !strcmp(menu[j].desc, g_cmd_buf)) {
					usel = j;
					break;
				}
			}
		}

		if (usel >= 0 && usel < ARRAY_SIZE(menu)) {
			if (menu[usel].menu == NULL) {
				info("%s test not imlpemented now\n", menu[usel].desc);
			} else {
				if (menu[usel].prepare != NULL) 
					ret = (menu[usel].prepare)(menu[usel].dev_sel);
				else 
					ret = 0;
				
				if (!ret) 
				{
					one_device_test(usel);
					dbg_mem_stat();
				}
			}
		}
		
		info("\r\n");
	}

	disable_interrupts();
	
	while (1);
	return 0;
}


