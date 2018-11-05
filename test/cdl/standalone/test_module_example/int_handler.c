/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"

#if (CONFIG_DRIVERS_GIC==0)
void irq_generic_handler( void )
{
	console_printf( "interrupt !\n" );
}
#endif