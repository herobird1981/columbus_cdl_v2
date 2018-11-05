/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_wdt.h"

struct brite_wdt wdt;
struct brite_wdt* p_wdt = &wdt;

/*****************************************************************************/
//  Function:  static void wdt_enable(int enable)
//
//  Parameters:
//		enable	: WDT enable
//				0 : Timer is disabled(count value is set to FFFF_FFFFh)
//	
//				1: Timer is enabled
//
//  Return:
//		none
//		
//  Description:
//      WDT enable/disable
//
/*****************************************************************************/
static void wdt_enable(int enable)
{
	unsigned long val;
	val = readl((unsigned long)p_wdt->regs + WDT_TCR);
	val &= ~(1 << 9);
	if (enable) {
		val |= (1 << 9);
	}
	writel(val, (unsigned long)p_wdt->regs + WDT_TCR);
}

/*****************************************************************************/
//  Function:  int wdt_init(unsigned long base, unsigned long pclk, unsigned long div)
//
//  Parameters:
//		base	: WDT module base address
//
//		pclk	: WDT module  input clock
//
//		div	: WDT perscale divisor
//			bit [0, 3], 4 Lower Bits of the Prescale Divisor
//					The bits (3:0) further divide 4 higher bits (7:4) divided-clock by following prescale divisors 
//					4¡¯b0000: No divide 
// 					4¡¯b0001: Divide by 2 
// 					4¡¯b0010: Divide by 4 
// 					4¡¯b0011: Divide by 8 
// 					4¡¯b0100: Divide by 16 
// 					4¡¯b0101: Divide by 32 
// 					4¡¯b0110: Divide by 64 
// 					4¡¯b0111: Divide by 128 
// 					4¡¯b1000: Divide by 256 
// 					4¡¯b1001: Divide by 512 
// 					4¡¯b1010: Divide by 1024 
// 					4¡¯b1011: Divide by 2048 
// 					4¡¯b1100: Divide by 4096 
// 					4¡¯b1101: Divide by 8192 
// 					4¡¯b1110: Divide by 16384 
// 					4¡¯b1111: Reserved 
//
//			bit [4, 7], 4 Higher Bits of the Prescale Divisor
//					The 4 bits (7:4) divide the system clock by following prescale divisors. 
//					4¡¯b0000: Divide by 2 
//					4¡¯b0001: Divide by 4 
//					4¡¯b0010: Divide by 8 
//					4¡¯b0011: Divide by 16 
//					4¡¯b0100: Divide by 32 
//					4¡¯b0101: Divide by 64 
//					4¡¯b0110: Divide by 128 
//					4¡¯b0111: Divide by 256 
//					4¡¯b1000: Divide by 512 
//					4¡¯b1001: Divide by 1024 
//					4¡¯b1010: Divide by 2048 
//					4¡¯b1011: Divide by 4096 
//					4¡¯b1100: Divide by 8192 
//					4¡¯b1101: Divide by 16384
//					4¡¯b1110: Reserved 
//					4¡¯b1111: Reserved 
//
//  Return:
//		none
//		
//  Description:
//      WDT initialize
//
/*****************************************************************************/
int wdt_init(unsigned long base, unsigned long pclk, unsigned long div)
{
	int div1;
	int div2;

	memset(p_wdt, 0, sizeof(struct brite_wdt));

	p_wdt->regs = (void*)base;
	p_wdt->pclk = pclk;

	/* check valid div bits */
	if ((div & 0xf) > 14) {
		p_wdt->div = 14;
		info("!!! WARNING: reduce low 4 bits divisor to 14\n");
	} else {
		p_wdt->div = div & 0xf;
	}
	if (((div >> 4) & 0xf) > 13) {
		p_wdt->div |= (13 << 4);
		info("!!! WARNING: reduce high 4 bits divisor to 13\n");
	} else {
		p_wdt->div |= ((div >> 4) & 0xf) << 4;
	}

	/* default value */
	p_wdt->reset_pulse_len = WDT_RST_LEN_DEFAULT;
	p_wdt->action = WDT_ACTION_RESET;
	p_wdt->timeout_range = WDT_TIMEOUT_64K;
	p_wdt->load_val = 0xffffffff;
	div1 = 1<<(((p_wdt->div>>4) & 0xF)+1);
	div2 = 1<<(p_wdt->div & 0xF);
	p_wdt->freq = (p_wdt->pclk / div1) / div2;

	if(p_wdt->freq == 0) {
		info("!!! WARNING: wdt actual freq is 0, force to 1, later time display won't be accurate\n");
		p_wdt->freq = 1;
	}

	info("wdt actual frequency: %d, pclk %d divh %d divl %d\n", p_wdt->freq, p_wdt->pclk, div1, div2);

	return 0;
}

/*****************************************************************************/
//  Function:  void wdt_set_options(unsigned long rst_len, unsigned long action, unsigned long timeout, unsigned long load_val)
//
//  Parameters:
//		rst_len	: WDT timer reset pulse length
//
//		action	: WDT timer action
//			¡¯	0: When the timer count reaches zero, a reset output is generated.
//
//				1: When the timer count reaches zero, an interrupt request is generated. 
//
//		timeout	: ime-out range. When a WDT Interrupt is generated, if it is not cleared before the Time-out time, then it generates a system reset. 
//			
//				3'    b000: 64K of WDT Counter clock cycles 
//  				3¡¯b001: 32K of WDT Counter clock cycles 
//  				3¡¯b010: 16K of WDT Counter clock cycles 
//  				3¡¯b011: 8K of WDT Counter clock cycles 
//  				3¡¯b100: 4K of WDT Counter clock cycles 
// 	 			3¡¯b101: 2K of WDT Counter clock cycles 
//  				3¡¯b110: 1K of WDT Counter clock cycles 
//  				3¡¯b111: Reserved 
//
//		load_val	: WDT timer load value
//
//  Return:
//		none
//		
//  Description:
//      Set WDT options
//
/*****************************************************************************/
void wdt_set_options(unsigned long rst_len, unsigned long action,
	unsigned long timeout, unsigned long load_val)
{
	unsigned long timeout_counter;
	p_wdt->reset_pulse_len = rst_len;
	p_wdt->action = action;
	p_wdt->timeout_range = timeout;
	timeout_counter = (1<<(6-p_wdt->timeout_range)) * 1024;
	p_wdt->timeout_ms = timeout_counter * 1000 / p_wdt->freq;
	p_wdt->load_val = load_val;
}

/*****************************************************************************/
//  Function:  void wdt_start(void)
//
//  Parameters:
//		none
//
//  Return:
//		none
//		
//  Description:
//      WDT start
//
/*****************************************************************************/
void wdt_start(void)
{
	unsigned long val;

	wdt_enable(0);/* disable */
	wdt_clear_int_status();

	/* config */
	writel(p_wdt->load_val, (unsigned long)p_wdt->regs + WDT_LDR);
	writel(p_wdt->reset_pulse_len,
		(unsigned long)p_wdt->regs + WDT_PLR);

	/* set control: div, action,timeout */
	val = p_wdt->div;
	val |= p_wdt->action << 8;
	val |= p_wdt->timeout_range << 10;
	writel(val, (unsigned long)p_wdt->regs + WDT_TCR);

	/* enable timer */
	wdt_enable(1);

	/* restart it to reload value from LDR */
	writel(WDT_CNT_RESTART_VALUE,
		(unsigned long)p_wdt->regs + WDT_CRR);

	info("TCR 0x%x\n", readl((unsigned long)p_wdt->regs + WDT_TCR));
}

/*****************************************************************************/
//  Function:  void wdt_stop(void)
//
//  Parameters:
//		none
//
//  Return:
//		none
//		
//  Description:
//      WDT stop
//
/*****************************************************************************/
void wdt_stop(void)
{
	wdt_enable(0);
	wdt_clear_int_status();
}

/*****************************************************************************/
//  Function:  void wdt_restart(void)
//
//  Parameters:
//		none
//
//  Return:
//		none
//		
//  Description:
//      WDT restart
//
/*****************************************************************************/
void wdt_restart(void)
{
	wdt_clear_int_status();
	writel(WDT_CNT_RESTART_VALUE,
		(unsigned long)p_wdt->regs + WDT_CRR);
}

/*****************************************************************************/
//  Function:  int wdt_is_enabled(void)
//
//  Parameters:
//		none
//
//  Return:
//		WDT enabled status
//			0: disable
//			1: enable
//		
//  Description:
//      Check if WDT enabled 
//
/*****************************************************************************/
int wdt_is_enabled(void)
{
	unsigned long val;
	val = readl((unsigned long)p_wdt->regs + WDT_TCR);
	val &= (1 << 9);
	if (val) {
		return 1;
	}
	return 0;
}

/*****************************************************************************/
//  Function:  unsigned long wdt_get_counter(void)
//
//  Parameters:
//		none
//
//  Return:
//		WDT counter
//		
//  Description:
//      Get the current counter of WDT 
//
/*****************************************************************************/
unsigned long wdt_get_counter(void)
{
	return readl((unsigned long)p_wdt->regs + WDT_CNT);
}

/*****************************************************************************/
//  Function:  int wdt_get_int_status(void)
//
//  Parameters:
//		none
//
//  Return:
//		WDT interrupt status
//		
//  Description:
//      Get the interrupt status of WDT 
//
/*****************************************************************************/
int wdt_get_int_status(void)
{
	return readl((unsigned long)p_wdt->regs + WDT_ISR);
}

/*****************************************************************************/
//  Function:  void wdt_clear_int_status(void)
//
//  Parameters:
//		none
//
//  Return:
//		none
//		
//  Description:
//      Clear the interrupt status of WDT
//
/*****************************************************************************/
void wdt_clear_int_status(void)
{
	writel(1, (unsigned long)p_wdt->regs + WDT_ISR);
}

