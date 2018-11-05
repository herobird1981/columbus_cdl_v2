/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"
#include "timer.h"
#include "std_common.h"

int do_fastboot(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	//init console first
	console_init( CONSOLE_BASE, CONSOLE_PCLK );
	timer_init();
#if (CONFIG_DRIVERS_GIC==1)
	irq_generic_init(GIC_BASE,GIC_CPU_BASE);
	enable_interrupts();
#endif
	/*init dmac*/
	dw_dmac_init(&dma_dev_list[0], DMAC0_BASE);
	dw_dmac_init(&dma_dev_list[1], DMAC1_BASE);
	dw_dmac_init(&dma_dev_list[2], DMAC2_BASE);

	debug( "\n" );
	debug( "test pattern for usb device\n" );

	/* usb dev test*/
	//do_fastboot(argc,argv);
  do_usb(NULL,0,argc,argv);
	debug( "test pattern done\n" );
	while ( 1 );
	return 0;
}


