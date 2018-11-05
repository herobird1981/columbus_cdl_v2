/*---------------------------------------------------------------------------------------------------*/
/*                                                                                               											 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.               							 */
/*                                                                                               											 */
/*---------------------------------------------------------------------------------------------------*/

#include "cdl_config.h"
#include "cdl_sys.h"
#include "cdl_ipc.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
struct brite_prcm prcm_dev;
struct brite_prcm *p_prcm = &prcm_dev;

struct brite_misc misc_dev;
struct brite_misc *p_misc = &misc_dev;

extern struct a7_ipc* p_ipc_dev;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
/*************************************************************************************************************/
//  Function: void mem_cpy8(void *p_dst, const void *p_src, uint32_t size)
//
//  Parameters:
//		p_dst [out]	: the destination address
//		p_src [in]		: the source address
// 		size			: the size of the data buffer(byte aligned)
//				
//  Return:
//      	none
//
//  Description:
//		Copy 8bit data from p_src to p_dst with specified size.
//
/*************************************************************************************************************/
void mem_cpy8(void *p_dst, const void *p_src, uint32_t size)
{
	uint8_t *p_tmp_dst = (uint8_t *)p_dst;
	uint8_t *p_tmp_src = (uint8_t *)p_src;

	if ((p_dst == NULL) || (p_src == NULL)) {
		return;
	}
	
	if (size) {
		do {
			*p_tmp_dst++ = *p_tmp_src++;
		} while(--size);
	}
}

/*************************************************************************************************************/
//  Function: void mem_cpy32(void *p_dst, const void *p_src, uint32_t size)
//
//  Parameters:
//		p_dst [out]	: the destination address
//		p_src [in]		: the source address
// 		size			: the size of the data buffer(4bytes aligned)
//
//  Return:
//      	none
//
//  Description:
//		Copy 32bit data from p_src to p_dst with specified size.
//
/*************************************************************************************************************/
void mem_cpy32(void *p_dst, const void *p_src, uint32_t size)
{
	uint32_t *p_tmp_dst = (uint32_t *)p_dst;
	uint32_t *p_tmp_src = (uint32_t *)p_src;
	uint32_t len = size/4;

	if ((p_dst == NULL) || (p_src == NULL)) {
		return;
	}

	if (len) {
		do {
			*p_tmp_dst++ = *p_tmp_src++;
		} while(--len);
	}
}

/*************************************************************************************************************/
//  Function: void mem_set8(void *p_dst, uint8_t val, uint32_t size)
//
//
//  Parameters:
//		p_dst [out]	: the destination address
//		val			: data to be assigned to the destination address
// 		size			: the size of the data buffer(byte aligned)
//				
//  Return:
//      	none
//
//  Description:
//		Assigned the 8bit data to p_dst with specified size
//
/*************************************************************************************************************/
void mem_set8(void *p_dst, uint8_t val, uint32_t size)
{
	uint8_t *p_tmpdst = (uint8_t *)p_dst;

	if (p_dst == NULL) {
		return;
	}
	
	if (size) {
		do {
			*p_tmpdst++ = val;
		} while(--size);
	}
}


/*************************************************************************************************************/
//  Function: void mem_set32(void *p_dst, uint32_t val, uint32_t size)
//
//
//  Parameters:
//		p_dst [out]	: the destination address
//		val			: data to be assigned to the destination address
// 		size			: data size(byte aligned)
//				
//  Return:
//      	none
//
//  Description:
//		Assigned the 32bit data to p_dst with specified size
//
/*************************************************************************************************************/
void mem_set32(void *p_dst, uint32_t val, uint32_t size)
{
	uint32_t *p_tmpdst = (uint32_t *)p_dst;

	if (p_dst == NULL) {
		return;
	}

	size >>= 2;
	if (size) {		
		do {
			*p_tmpdst++ = val;
		} while(--size);
	}
}

/*************************************************************************************************************/
//  Function: int mem_cmp8(void *p_dst, void *p_src, uint32_t size)
//
//  Parameters:
//		p_dst [out]	: the destination address
//		p_src [in]		: the source address
// 		size			: compare data size(byte aligned)
//				
//  Return:
//      	equal data size
//		-1  : left < right
//		0	: all data are equal
//		1   : left > right
//
//  Description:
//		compare 8bit data between p_src and p_dst with specified size
//
/*************************************************************************************************************/
int mem_cmp8(void *p_dst, void *p_src, uint32_t size)
{
	uint8_t *p_tmpDst = (uint8_t *)p_dst;
	uint8_t *p_tmpSrc = (uint8_t *)p_src;
	uint32_t i = 0;

	if ((p_dst == NULL) || (p_src == NULL) || (size == 0)) {
		return 0;
	}
	
	while (i < size) {
		if (p_tmpDst[i] < p_tmpSrc[i])	{
			return -1;
		} else if (p_tmpDst[i] > p_tmpSrc[i])	{
				return 1;
		}
		i++;
	}

	return 0;
}

/*************************************************************************************************************/
//  Function: uint32_t mem_cmp32(void *p_dst, void *p_src, uint32_t size)
//
//  Parameters:
//		p_dst [out]	: the destination address
//		p_src [in]		: the source address
// 		size			: compare data size(byte aligned)
//				
//  Return:
//      	equal data size
//		0	: no data is equal
//		size	: all data are equal
//
//  Description:
//		compare 32bit data between p_src and p_dst with specified size
//
/*************************************************************************************************************/
uint32_t mem_cmp32(void *p_dst, void *p_src, uint32_t size)
{
	uint32_t *p_tmpDst = (uint32_t *)p_dst;
	uint32_t *p_tmpSrc = (uint32_t *)p_src;
	uint32_t i = 0;

	size >>= 2;
	if ((p_dst == NULL) || (p_src == NULL) || (size == 0)) {
		return 0;
	}
	
	while (i < size) {		
		if (p_tmpDst[i] != p_tmpSrc[i]) {
			return i; 
		}

		i++;
	}

	return (i << 2);
}

void mem_set_seq(uint8_t *p_data, uint32_t size)
{
	uint32_t i;

	for (i = 0; i < size; i++) {
		p_data[i] = i & 0xFF;
	}
}

void mem_printf(uint32_t *p_data, uint32_t size)
{
	uint32_t i;

	size >>= 2;
	for (i = 0; i < size; i++) {
		debug("p_data[%x]:%x\n", i, p_data[i]);
	}
}

void mem_print_line(uint8_t *p_data, uint32_t size)
{
	uint32_t i, j;
	uint32_t line;
	uint32_t rest;

	rest = size;
	for (i = 0; i < size; i += 16) 
	{
		line = (rest > 16) ? 16 : rest;
		
		for (j = 0; j < line; j++)
		{
			info("%02x ", p_data[i + j]);
		}
		
		info("\n");
		rest -= line;
	}
}

/*************************************************************************************************************/
//  Function: uint8_t power(uint32_t size)
//
//  Parameters:
// 		size			: the input size
//				
//  Return:
//      	the power of the size
//
//  Description:
//		Caculate the power of the input size
//		size			power
//		0			0
//		1			0
//		2			1
//		...			...
//		0x80000000	31
//
/*************************************************************************************************************/
uint8_t power(uint32_t size)
{
	uint8_t i;

	for (i = 31; i >= 1; i--)
	{
		if (size & (1<<i))
		{
			return i;
		}
	}

	return 0;
}

void sw_udelay_10x(u32 cnt)
{
	u32 i = 0;

	cnt *= 625;   //(600MHz:1000 loops = 16us)

	for (i = 0; i < cnt; i++) {
		__asm__ volatile("nop");
	}
}

int sw_mutex_lock(int timeout_ms)
{
	unsigned long long timebase = 0;

	p_ipc_dev->reg_base = (void*)A7_IPC_REG_BASE;

	timebase = get_timer(0);
	do {
		if (a7_get_sram_page(p_ipc_dev, 31) == 0) {
			return 0;
		}
	} while (get_timer(timebase) < timeout_ms);
	return -1;
}

int sw_mutex_unlock(void)
{
	if (a7_free_page(p_ipc_dev, 31) < 0) {
		return -1;
	}

	return 0;
}

void prcm_var_init(uint32_t base)
{
	info("prcm_var_init\n");

	p_prcm->regs = (void *)base;
}

void misc_var_init(uint32_t base)
{
	info("misc_var_init\n");

	p_misc->regs = (void *)base;
}

/*************************************************************************************************************/
//  Function: uint32_t prcm_pwd_read(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	The pwd value
//
//  Description:
//		Read the pwd value.
//
/*************************************************************************************************************/
uint32_t prcm_pwd_read(void)
{
	uint32_t reg_val = __raw_readl(SYS_ARM_PWD_REG);

	switch ((reg_val & ARM_PWD_ST_MACHINE_MASK)>>ARM_PWD_STATE_MACHINE_SHIFT)
	{
		case ARM_EPF_SLEEP_MODE :
				info("EPF sleep mode\n");
			break;
		case ARM_PU_TRANS_MODE :
				info("Power up transition state mode\n");
			break;
		case ARM_ACTIVE_MODE :
				info("Active mode\n");
			break;
		case ARM_EPF_SHUTDOWN_MODE :
				info("EPF shutdown mode\n");
			break;
		case ARM_PD_TRANS_MODE :
				info("Power down transition state	\n");
			break;
		default :
				info("Unknown mode\n");
			break;			
	}
	
	return reg_val;
}

/*************************************************************************************************************/
//  Function: void prcm_pwd_hs_keeping_ctrl(E_ARM_HS_KEEPING_DONE_ST st)
//
//  Parameters:
// 		st: house keeping status
//
//  Return:
//      	none
//
//  Description:
//		Control the house keeping status.
//
/*************************************************************************************************************/
void prcm_pwd_hs_keeping_ctrl(E_ARM_HS_KEEPING_DONE_ST st)
{
	uint32_t reg_val = __raw_readl(SYS_ARM_PWD_REG);
	
	reg_val &= (~ARM_PWD_HS_KEEPING_DONE_ST_MASK);
	reg_val |= ARM_PWD_HS_KEEPING_DONE_ST_VAL(st);
	
	writel(reg_val, SYS_ARM_PWD_REG);
}

/*************************************************************************************************************/
//  Function: void prcm_fail_safe_timer_init(uint32_t ms)
//
//  Parameters:
// 		ms: millisecond
//
//  Return:
//      	none
//
//  Description:
//		Initilize the fail safe timer threshold.
//
/*************************************************************************************************************/
void prcm_fail_safe_timer_init(uint32_t ms)
{
	uint32_t reg_val = __raw_readl(SYS_FAILSAFE_TIMER_REG);
	
	reg_val &= (~ARM_FAILSAFE_TIMER_MASK);
	reg_val |= ARM_FAILSAFE_TIMER_VAL(ms);
	
	writel(reg_val, SYS_FAILSAFE_TIMER_REG);
}

/*************************************************************************************************************/
//  Function: uint32_t prcm_pmu_int_st_read(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	The interrupt status
//
//  Description:
//		Read the pmu interrupt status.
//
/*************************************************************************************************************/
uint32_t prcm_pmu_int_st_read(void)
{
	return __raw_readl(SYS_ARM_PMU_INT_STATUS_REG);
}

/*************************************************************************************************************/
//  Function: void prcm_pmu_int_st_clr(void)
//
//  Parameters:
// 		ms: millisecond
//
//  Return:
//      	none
//
//  Description:
//		Clear the pmu interrupt status.
//
/*************************************************************************************************************/
void prcm_pmu_int_st_clr(void)
{
	uint32_t reg_val = __raw_readl(SYS_ARM_PMU_INT_STATUS_REG);
	
	reg_val |= ARM_PMU_INT_ST_MASK;
	
	writel(reg_val, SYS_ARM_PMU_INT_STATUS_REG);
}

/*************************************************************************************************************/
//  Function: void prcm_reset(E_ARM_IP_RST ip)
//
//  Parameters:
// 		ip: arm reset ip source
//
//  Return:
//      	none
//
//  Description:
//		Reset the ip
//
/*************************************************************************************************************/
void prcm_reset(E_ARM_IP_RST ip)
{
	uint32_t i;
	uint32_t reg_val, mask;

	info("reset ip:%d\n", ip);

	if ((ip >= ARM_NFC_RST) && (ip <= ARM_DDR_CTRL_RST)) 
	{
		mask = 1 << ip;
		
		// reset
		reg_val = __raw_readl(ARM_BLK_RST_REG);
		reg_val &= (~mask);
		writel(reg_val, ARM_BLK_RST_REG);

		for (i = 0; i < 0x20; i++)
		{
			__asm__ volatile("nop");
		}
        
		// realease
		__raw_writel(reg_val | mask, ARM_BLK_RST_REG);
	}
	else if ((ip >= ARM_PLC_SUBSYS_SOFT_RST) && (ip <= ARM_RF_DSP_CORE_SOFT_RST))  
	{
		mask = 1 << (ip - 32);
		
		// reset
		reg_val = __raw_readl(ARM_DSP_SUB_RST_CFG_REG);
		reg_val &= (~mask);
		writel(reg_val, ARM_DSP_SUB_RST_CFG_REG);

		for (i = 0; i < 0x20000; i++)
		{
			__asm__ volatile("nop");
		}

		// realease
		__raw_writel(reg_val | mask, ARM_DSP_SUB_RST_CFG_REG);
	}
	else if (ip == GLOBAL_SOFT_RST)
	{
		__raw_writel(GLOVAL_SOFT_RST_VAL, GLOBAL_SOFT_RST_REG);
	}
	
	else if (ip == ARM_SOFT_RST)
	{
		__raw_writel(ARM_SUBSYS_SOFT_RST_VAL, ARM_SOFT_RST_REG);
	}
	else
	{
		info("invalid ip:%d\n", ip);
	}
}

/*************************************************************************************************************/
//  Function: void prcm_blk_cg(E_ARM_BLK_CG blk, E_ARM_BLK_CG_CTRL ctrl)
//
//  Parameters:
// 		blk: clock gate block
//
//		ctrl: clock gate control
//
//  Return:
//      	none
//
//  Description:
//		Block clock gate control.
//
/*************************************************************************************************************/
void prcm_blk_cg(E_ARM_BLK_CG blk, E_ARM_BLK_CG_CTRL ctrl)
{
	uint32_t reg_val;
	uint32_t mask = ARM_BLK_CG_CTRL_MASK(blk);

	//info("blk:%d, mask:0x%x\n", blk, mask);

	reg_val = __raw_readl(SYS_BLK_CG_REG);
	reg_val &= (~mask);
	reg_val |= ARM_BLK_CG_CTRL_VAL(blk, ctrl);
	writel(reg_val, SYS_BLK_CG_REG);
}

/*************************************************************************************************************/
//  Function: static bool misc_uart_pinmux(E_UART_PORT port)
//
//  Parameters:
// 		port	: uart port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to uart function
//
/*************************************************************************************************************/
static bool misc_uart_pinmux(E_UART_PORT port)
{
	uint32_t reg_val;

	// Check the validity of the port
	if (port >= NUM_UART_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_PINMUX_UART_REG);

	switch (port)
	{
		case UART0:
				reg_val &= (~ARM_PINMUX_UART0_MASK);
				reg_val |= ARM_PINMUX_UART0_VAL;
			break;
		case UART1:
				reg_val &= (~ARM_PINMUX_UART1_MASK);
				reg_val |= ARM_PINMUX_UART1_VAL;			
			break;
		case UART2:
				reg_val &= (~ARM_PINMUX_UART2_MASK);
				reg_val |= ARM_PINMUX_UART2_VAL;			
			break;
		case UART3:
		default:	
				reg_val &= (~ARM_PINMUX_UART3_MASK);
				reg_val |= ARM_PINMUX_UART3_VAL;			
			break;
	}

	__raw_writel(reg_val, ARM_PINMUX_UART_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_sdio_pinmux(E_SDIO_PORT port, E_SDIO_BW bw)
//
//  Parameters:
// 		port	: sdio port
//
//		bw	: sdio bus width
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to sdio function
//
/*************************************************************************************************************/
static bool misc_sdio_pinmux(E_SDIO_PORT port, E_SDIO_BW bw)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SDIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	switch (port)
	{
		case SDIO0:
				reg_val = __raw_readl(ARM_PINMUX_SD0_REG);
				
				if      (bw == SDIO_BW_1BIT)	reg_val &= (~ARM_PINMUX_SD0_1BIT_MASK);
				else if (bw == SDIO_BW_4BIT)	reg_val &= (~ARM_PINMUX_SD0_4BIT_MASK);
				else							reg_val &= (~ARM_PINMUX_SD0_8BIT_MASK);
				
				__raw_writel(reg_val, ARM_PINMUX_SD0_REG);
			break;
		case SDIO1:
		default:
				reg_val = __raw_readl(ARM_PINMUX_SD1_REG);

				if      (bw == SDIO_BW_1BIT)	reg_val &= (~ARM_PINMUX_SD1_1BIT_MASK);
				else if (bw == SDIO_BW_4BIT)	reg_val &= (~ARM_PINMUX_SD1_4BIT_MASK);
				else							reg_val &= (~ARM_PINMUX_SD1_8BIT_MASK);
				
				__raw_writel(reg_val, ARM_PINMUX_SD1_REG);
			break;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_spi_pinmux(E_SPI_PORT port)
//
//  Parameters:
// 		port	: spi port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to spi function
//
/*************************************************************************************************************/
static bool misc_spi_pinmux(E_SPI_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SPI_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_PINMUX_SPI_REG);
	switch (port)
	{
		case SPI0:
				reg_val &= (~ARM_PINMUX_SPI0_MASK);
				reg_val |= ARM_PINMUX_SPI0_VAL;
			break;
		case SPI1:
				reg_val &= (~ARM_PINMUX_SPI1_MASK);
				reg_val |= ARM_PINMUX_SPI1_VAL;
			break;
		case SPI2:
		default:	
				reg_val &= (~ARM_PINMUX_SPI2_MASK);
				reg_val |= ARM_PINMUX_SPI2_VAL;
			break;
	}

	__raw_writel(reg_val, ARM_PINMUX_SPI_REG);
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_i2c_pinmux(E_I2C_PORT port)
//
//  Parameters:
// 		port	: i2c port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to spi function
//
/*************************************************************************************************************/
static bool misc_i2c_pinmux(E_I2C_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_I2C_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
	switch (port)
	{
		case I2C0:
				reg_val &= (~ARM_PINMUX_I2C0_MASK);
				reg_val |= ARM_PINMUX_I2C0_VAL;
			break;
		case I2C1:
				reg_val &= (~ARM_PINMUX_I2C1_MASK);
				reg_val |= ARM_PINMUX_I2C1_VAL;
			break;
		case I2C2:
		default:	
				reg_val &= (~ARM_PINMUX_I2C2_MASK);
				reg_val |= ARM_PINMUX_I2C2_VAL;
			break;
	}	

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioa_pinmux(E_PIN_PORT port)
//
//  Parameters:
// 		pin	: gpio pin
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_gpioa_pinmux(E_PIN_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_19))
	{
		reg_val = __raw_readl(ARM_PINMUX_UART_REG);
	}
	else if (((port >= PIN_20) && (port <= PIN_25)) ||
			 ((port >= PIN_29) && (port <= PIN_31)))
	{
		reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
	}
	else
	{
		// pin not muxed
		return true;
	}

	switch (port)
	{
		case PIN_0:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN0_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN0_VAL;
			break;
		case PIN_1:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN1_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN1_VAL;
			break;
		case PIN_2:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN2_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN2_VAL;
			break;
		case PIN_3:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN3_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN3_VAL;
			break;
		case PIN_4:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN4_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN4_VAL;
			break;
		case PIN_5:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN5_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN5_VAL;
			break;
		case PIN_6:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN6_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN6_VAL;
			break;
		case PIN_7:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN7_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN7_VAL;
			break;
		case PIN_8:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN8_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN8_VAL;
			break;
		case PIN_9:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN9_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN9_VAL;
			break;
		case PIN_10:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN10_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN10_VAL;
			break;		
		case PIN_11:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN11_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN11_VAL;
			break;	
		case PIN_12:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN12_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN12_VAL;
			break;	
		case PIN_13:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN13_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN13_VAL;
			break;	
		case PIN_14:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN14_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN14_VAL;
			break;					
		case PIN_15:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN15_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN15_VAL;
			break;		
		case PIN_16:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN16_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN16_VAL;
			break;	
		case PIN_17:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN17_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN17_VAL;
			break;	
		case PIN_18:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN18_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN18_VAL;
			break;	
		case PIN_19:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN19_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN19_VAL;
			break;	
		case PIN_20:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN20_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN20_VAL;
			break;		
		case PIN_21:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN21_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN21_VAL;
			break;	
		case PIN_22:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN22_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN22_VAL;
			break;
		case PIN_23:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN23_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN23_VAL;
			break;
		case PIN_24:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN24_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN24_VAL;
			break;
		case PIN_25:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN25_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN25_VAL;
			break;	
		case PIN_29:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN29_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN29_VAL;
			break;
		case PIN_30:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN30_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN30_VAL;
			break;
		case PIN_31:
				reg_val &= (~ARM_PINMUX_GPIOA_PIN31_MASK);
				reg_val |= ARM_PINMUX_GPIOA_PIN31_VAL;
			break;			
		default:
			break;
	}

	if ((port >= PIN_0) && (port <= PIN_19))
	{
		__raw_writel(reg_val, ARM_PINMUX_UART_REG);
	}
	else if (((port >= PIN_20) && (port <= PIN_25)) ||
			 ((port >= PIN_29) && (port <= PIN_31)))
	{
		__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
	}
	else
	{
		// pin not muxed
		return true;
	}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiob_pinmux(E_PIN_PORT port)
//
//  Parameters:
// 		pin	: gpioa pin
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_gpiob_pinmux(E_PIN_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	// Read pinmux register
	if ((port >= PIN_0) && (port <= PIN_16))
	{
		reg_val = __raw_readl(ARM_PINMUX_SPI_REG);
	}
	else
	{
		return true;
	}

	switch (port)
	{
		case PIN_0:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN0_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN0_VAL;
			break;
		case PIN_1:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN1_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN1_VAL;
			break;
		case PIN_2:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN2_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN2_VAL;
			break;
		case PIN_3:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN3_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN3_VAL;
			break;
		case PIN_4:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN4_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN4_VAL;
			break;
		case PIN_5:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN5_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN5_VAL;
			break;
		case PIN_6:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN6_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN6_VAL;
			break;
		case PIN_7:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN7_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN7_VAL;
			break;
		case PIN_8:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN8_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN8_VAL;
			break;
		case PIN_9:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN9_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN9_VAL;
			break;
		case PIN_10:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN10_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN10_VAL;
			break;		
		case PIN_11:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN11_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN11_VAL;
			break;	
		case PIN_12:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN12_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN12_VAL;
			break;	
		case PIN_13:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN13_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN13_VAL;
			break;	
		case PIN_14:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN14_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN14_VAL;
			break;					
		case PIN_15:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN15_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN15_VAL;
			break;		
		case PIN_16:
				reg_val &= (~ARM_PINMUX_GPIOB_PIN16_MASK);
				reg_val |= ARM_PINMUX_GPIOB_PIN16_VAL;
			break;
		default:
			break;
	}

	// Write pinmux register
	if ((port >= PIN_0) && (port <= PIN_16))
	{
		__raw_writel(reg_val, ARM_PINMUX_SPI_REG);
	}
	else
	{
		return true;
	}	
	
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioc_pinmux(E_PIN_PORT port)
//
//  Parameters:
// 		pin	: gpio pin
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_gpioc_pinmux(E_PIN_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0) && (port <= PIN_9)) || 
		(port == PIN_25) ||
		(port == PIN_26) ||
		(port == PIN_29))
	{
		reg_val = __raw_readl(ARM_PINMUX_SD0_REG);
	}
	else if (((port >= PIN_10) && (port <= PIN_19)) || 
			(port == PIN_27) ||
			(port == PIN_28) ||
			(port == PIN_30))
	{
		reg_val = __raw_readl(ARM_PINMUX_SD1_REG);
	}
	else
	{
		// pin not muxed
		return true;
	}

	switch (port)
	{
		case PIN_0:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN0_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN0_VAL;
			break;
		case PIN_1:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN1_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN1_VAL;
			break;
		case PIN_2:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN2_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN2_VAL;
			break;
		case PIN_3:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN3_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN3_VAL;
			break;
		case PIN_4:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN4_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN4_VAL;
			break;
		case PIN_5:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN5_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN5_VAL;
			break;
		case PIN_6:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN6_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN6_VAL;
			break;
		case PIN_7:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN7_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN7_VAL;
			break;
		case PIN_8:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN8_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN8_VAL;
			break;
		case PIN_9:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN9_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN9_VAL;
			break;
		case PIN_25:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN25_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN25_VAL;
			break;	
		case PIN_26:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN26_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN26_VAL;
			break;	
		case PIN_29:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN29_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN29_VAL;
			break;		

		case PIN_10:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN10_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN10_VAL;
			break;		
		case PIN_11:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN11_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN11_VAL;
			break;	
		case PIN_12:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN12_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN12_VAL;
			break;	
		case PIN_13:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN13_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN13_VAL;
			break;	
		case PIN_14:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN14_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN14_VAL;
			break;					
		case PIN_15:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN15_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN15_VAL;
			break;		
		case PIN_16:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN16_TIMER0_IN_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN16_VAL;
			break;	
		case PIN_17:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN17_TIMER1_IN_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN17_VAL;
			break;	
		case PIN_18:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN18_TIMER2_IN_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN18_VAL;
			break;	
		case PIN_19:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN19_TIMER3_IN_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN19_VAL;
			break;	
		case PIN_27:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN27_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN27_VAL;
			break;		
		case PIN_28:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN28_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN28_VAL;
			break;	
		case PIN_30:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN30_MASK);
				reg_val |= ARM_PINMUX_GPIOC_PIN30_VAL;
			break;			
		default:
			break;
	}

	if (((port >= PIN_0) && (port <= PIN_9)) || 
		(port == PIN_25) ||
		(port == PIN_26) ||
		(port == PIN_29))
	{
		__raw_writel(reg_val, ARM_PINMUX_SD0_REG);
	}
	else if (((port >= PIN_10) && (port <= PIN_19)) || 
			(port == PIN_27) ||
			(port == PIN_28) ||
			(port == PIN_30))
	{
		__raw_writel(reg_val, ARM_PINMUX_SD1_REG);
	}
	else
	{
		// pin not muxed
		return true;
	}
		
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiod_pinmux(E_PIN_PORT port)
//
//  Parameters:
// 		pin	: gpio pin
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_gpiod_pinmux(E_PIN_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	// Read pinmux register
	if (((port >= PIN_0) && (port <= PIN_3)) ||
		((port >= PIN_29) && (port <= PIN_31)))
	{
		reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
	}
	else
	{
		return true;
	}

	switch (port)
	{
		case PIN_0:
				reg_val &= (~ARM_PINMUX_GPIOD_PIN0_TIMERW_OUT_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN0_VAL;
			break;
		case PIN_1:
				reg_val &= (~ARM_PINMUX_GPIOD_PIN1_TIMERX_OUT_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN1_VAL;
			break;
		case PIN_2:
				reg_val &= (~ARM_PINMUX_GPIOD_PIN2_TIMERY_OUT_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN2_VAL;
			break;
		case PIN_3:
				reg_val &= (~ARM_PINMUX_GPIOD_PIN3_TIMERZ_OUT_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN3_VAL;
			break;
		case PIN_29:
				reg_val &= (~ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN29_VAL;
			break;
		case PIN_30:
				reg_val &= (~ARM_PINMUX_CLKOUT_GPIOD30_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN30_VAL;
			break;
		case PIN_31:
				reg_val &= (~ARM_PINMUX_RTC_CLKOUT_GPIOD31_MASK);
				reg_val |= ARM_PINMUX_GPIOD_PIN31_VAL;
			break;
		default:
			break;
	}	

	// Write pinmux register
	if (((port >= PIN_0) && (port <= PIN_3)) ||
		((port >= PIN_29) && (port <= PIN_31)))
	{
		__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
	}
	else
	{
		return true;
	}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpio_pinmux(E_GPIO_PORT port, E_PIN_PORT pin)
//
//  Parameters:
// 		port	: gpio port
//
// 		pin	: gpio pin
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_gpio_pinmux(E_GPIO_PORT port, E_PIN_PORT pin)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_GPIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	switch (port)
	{
		case GPIOA:
				if (true != misc_gpioa_pinmux(pin))
				{
					return false;
				}
			break;
		case GPIOB:
				if (true != misc_gpiob_pinmux(pin))
				{
					return false;
				}
			break;
		case GPIOC:				
				if (true != misc_gpioc_pinmux(pin))
				{
					return false;
				}
			break;		
		case GPIOD:
		default:
				if (true != misc_gpiod_pinmux(pin))
				{
					return false;
				}
			break;			
	}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiob_17_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_gpiob_17_pinmux(void)
{
	uint32_t reg_val = __raw_readl(ARM_WDT_RSTN_IODIS);;
	
	reg_val &= (~ARM_PINMUX_GPIOB_PIN17_MASK);
	reg_val |= ARM_PINMUX_GPIOB_PIN17_VAL;

	__raw_writel(reg_val, ARM_WDT_RSTN_IODIS);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_wdt_rstn_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to gpio function
//
/*************************************************************************************************************/
static bool misc_wdt_rstn_pinmux(void)
{
	uint32_t reg_val = __raw_readl(ARM_WDT_RSTN_IODIS);
	
	reg_val &= (~ARM_PINMUX_WDT_RSTN_MASK);
	reg_val |= ARM_PINMUX_WDT_RSTN_VAL;

	__raw_writel(reg_val, ARM_WDT_RSTN_IODIS);

	info("ARM_WDT_RSTN_IODIS(%x)\n", __raw_readl(ARM_WDT_RSTN_IODIS));
	
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_input_pinmux(E_TIMER_PORT port)
//
//  Parameters:
// 		port	: timer port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to timer input function
//
/*************************************************************************************************************/
static bool misc_timer_input_pinmux(E_TIMER_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= TIMER4)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_PINMUX_SD1_REG);
	
	switch (port)
	{
		case TIMER0:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN16_TIMER0_IN_MASK);
				reg_val |= ARM_PINMUX_TIMER0_IN_VAL;
			break;
		case TIMER1:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN17_TIMER1_IN_MASK);
				reg_val |= ARM_PINMUX_TIMER1_IN_VAL;
			break;	
		case TIMER2:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN18_TIMER2_IN_MASK);
				reg_val |= ARM_PINMUX_TIMER2_IN_VAL;
			break;			
		case TIMER3:
		default:
				reg_val &= (~ARM_PINMUX_GPIOC_PIN19_TIMER3_IN_MASK);
				reg_val |= ARM_PINMUX_TIMER3_IN_VAL;
			break;
	}

	__raw_writel(reg_val, ARM_PINMUX_SD1_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_output_pinmux(E_TIMER_WXYZ_PORT port, E_TIMER_PORT timer)
//
//  Parameters:
// 		port		: timer w/x/y/z port
//
//		timer 	: timer [0, 7]port 
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Mux pin to timer output function
//
/*************************************************************************************************************/
static bool misc_timer_output_pinmux(E_TIMER_WXYZ_PORT port, E_TIMER_PORT timer)
{
	uint32_t reg_val;
	uint32_t reg_val0;
	
	// Check the validity of the w/x/y/z port
	if (port >= NUM_TIMER_WXYZ_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	// Check the validity of the timer port
	if (timer >= NUM_TIMER_PORTS)
	{
		info("timer(%x) err!n", port);
		return false;
	}	

	// mux the pin to gpio function to avoid the same timer output on different pin(w/x/y/z)
	reg_val0 = __raw_readl(ARM_TIMER_OUTPUT_REG);
	if ((reg_val0 & ARM_PINMUX_TIMERW_OUTPUT_MASK) == ARM_PINMUX_TIMERW_OUTPUT_VAL(timer))
	{
		reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
		reg_val &= (~ARM_PINMUX_GPIOD_PIN0_TIMERW_OUT_MASK);
		reg_val |= ARM_PINMUX_GPIOD_PIN0_VAL;
		__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
	}

	if ((reg_val0 & ARM_PINMUX_TIMERX_OUTPUT_MASK) == ARM_PINMUX_TIMERX_OUTPUT_VAL(timer))
	{
		reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
		reg_val &= (~ARM_PINMUX_GPIOD_PIN1_TIMERX_OUT_MASK);
		reg_val |= ARM_PINMUX_GPIOD_PIN1_VAL;
		__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
	}

	if ((reg_val0 & ARM_PINMUX_TIMERY_OUTPUT_MASK) == ARM_PINMUX_TIMERY_OUTPUT_VAL(timer))
	{
		reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
		reg_val &= (~ARM_PINMUX_GPIOD_PIN2_TIMERY_OUT_MASK);
		reg_val |= ARM_PINMUX_GPIOD_PIN2_VAL;
		__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
	}

	if ((reg_val0 & ARM_PINMUX_TIMERZ_OUTPUT_MASK) == ARM_PINMUX_TIMERZ_OUTPUT_VAL(timer))
	{
		reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
		reg_val &= (~ARM_PINMUX_GPIOD_PIN3_TIMERZ_OUT_MASK);
		reg_val |= ARM_PINMUX_GPIOD_PIN3_VAL;
		__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
	}	

	// mux the pin to timer output	
	switch (port)
	{
		case TIMER_W:
				reg_val = __raw_readl(ARM_TIMER_OUTPUT_REG);
				reg_val &= (~ARM_PINMUX_TIMERW_OUTPUT_MASK);
				reg_val |= ARM_PINMUX_TIMERW_OUTPUT_VAL(timer);
				__raw_writel(reg_val, ARM_TIMER_OUTPUT_REG);

				reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
				reg_val &= (~ARM_PINMUX_GPIOD_PIN0_TIMERW_OUT_MASK);
				reg_val |= ARM_PINMUX_TIMERW_OUT_VAL;
				__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
			break;
		case TIMER_X:
				reg_val = __raw_readl(ARM_TIMER_OUTPUT_REG);
				reg_val &= (~ARM_PINMUX_TIMERX_OUTPUT_MASK);
				reg_val |= ARM_PINMUX_TIMERX_OUTPUT_VAL(timer);
				__raw_writel(reg_val, ARM_TIMER_OUTPUT_REG);

				reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
				reg_val &= (~ARM_PINMUX_GPIOD_PIN1_TIMERX_OUT_MASK);
				reg_val |= ARM_PINMUX_TIMERX_OUT_VAL;
				__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
			break;	
		case TIMER_Y:
				reg_val = __raw_readl(ARM_TIMER_OUTPUT_REG);
				reg_val &= (~ARM_PINMUX_TIMERY_OUTPUT_MASK);
				reg_val |= ARM_PINMUX_TIMERY_OUTPUT_VAL(timer);
				__raw_writel(reg_val, ARM_TIMER_OUTPUT_REG);

				reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
				reg_val &= (~ARM_PINMUX_GPIOD_PIN2_TIMERY_OUT_MASK);
				reg_val |= ARM_PINMUX_TIMERY_OUT_VAL;
				__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
			break;			
		case TIMER_Z:
		default:
				reg_val = __raw_readl(ARM_TIMER_OUTPUT_REG);
				reg_val &= (~ARM_PINMUX_TIMERZ_OUTPUT_MASK);
				reg_val |= ARM_PINMUX_TIMERZ_OUTPUT_VAL(timer);
				__raw_writel(reg_val, ARM_TIMER_OUTPUT_REG);

				reg_val = __raw_readl(ARM_PINMUX_ITS_REG);
				reg_val &= (~ARM_PINMUX_GPIOD_PIN3_TIMERZ_OUT_MASK);
				reg_val |= ARM_PINMUX_TIMERZ_OUT_VAL;
				__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
			break;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_clkout_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Mux pin to rtc clk output function.
//
/*************************************************************************************************************/
static void misc_rtc_clkout_pinmux(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);

	reg_val &= (~ARM_PINMUX_RTC_CLKOUT_GPIOD31_MASK);
	reg_val |= ARM_PINMUX_RTC_CLKOUT_VAL;

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_cal_clkout_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Mux pin to rtc cal clk output function.
//
/*************************************************************************************************************/
static void misc_rtc_cal_clkout_pinmux(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);

	reg_val &= (~ARM_PINMUX_RTC_CAL_CLKOUT_GPIOD29_MASK);
	reg_val |= ARM_PINMUX_RTC_CAL_CLKOUT_VAL;

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_clkout_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Mux pin to arm clk output function.
//
/*************************************************************************************************************/
static void misc_clkout_pinmux(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);

	reg_val &= (~ARM_PINMUX_CLKOUT_GPIOD30_MASK);
	reg_val |= ARM_PINMUX_CLKOUT_VAL;

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_host_low_power_out_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Mux pin to host low power output function.
//
/*************************************************************************************************************/
static void misc_host_low_power_out_pinmux(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);

	reg_val &= (~ARM_PINMUX_HOST_LOW_POWER_OUT_MASK);
	reg_val |= ARM_PINMUX_HOST_LOW_POWER_OUT_VAL;

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_sleep_mode_done_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Mux pin to epf sleep mode done output function.
//
/*************************************************************************************************************/
static void misc_epf_sleep_mode_done_pinmux(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);

	reg_val &= (~ARM_PINMUX_ARM_EPF_SLEEP_MODE_DONE_MASK);
	reg_val |= ARM_PINMUX_ARM_EPF_SLEEP_MODE_DONE_VAL;

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_failsafe_time_out_pinmux(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Mux pin to epf fail timeout function.
//
/*************************************************************************************************************/
static void misc_epf_failsafe_time_out_pinmux(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PINMUX_ITS_REG);

	reg_val &= (~ARM_PINMUX_EPF_FAIL_TIME_OUT_MASK);
	reg_val |= ARM_PINMUX_EPF_FAIL_TIMER_OUT_VAL;

	__raw_writel(reg_val, ARM_PINMUX_ITS_REG);
}

/*************************************************************************************************************/
//  Function: static bool misc_uart_pu(E_UART_PORT port)
//
//  Parameters:
// 		port	: uart port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_uart_pu(E_UART_PORT port)
{
	uint32_t reg_val;

	// Check the validity of the port
	if (port >= NUM_UART_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_UART_PU_REG);

	switch (port)
	{
		case UART0:
				reg_val &= (~ARM_UART0_PU_MASK);
				reg_val |= ARM_UART0_PU_VAL;
			break;
		case UART1:
				reg_val &= (~ARM_UART1_PU_MASK);
				reg_val |= ARM_UART1_PU_VAL;			
			break;
		case UART2:
				reg_val &= (~ARM_UART2_PU_MASK);
				reg_val |= ARM_UART2_PU_VAL;			
			break;
		case UART3:
		default:	
				reg_val &= (~ARM_UART3_PU_MASK);
				reg_val |= ARM_UART3_PU_VAL;			
			break;
	}

	__raw_writel(reg_val, ARM_UART_PU_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_sdio_pu(E_SDIO_PORT port, E_SDIO_BW bw)
//
//  Parameters:
// 		port	: sdio port
//
//		bw	: sdio bus width
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_sdio_pu(E_SDIO_PORT port, E_SDIO_BW bw)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SDIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SD_PU_REG);
	
	switch (port)
	{
		case SDIO0:
				if (bw == SDIO_BW_1BIT)	
				{
					reg_val &= (~ARM_SDIO0_1BIT_PU_MASK);
					reg_val |= ARM_SDIO0_1BIT_PU_VAL;
				}
				else if (bw == SDIO_BW_4BIT)	
				{
					reg_val &= (~ARM_SDIO0_4BIT_PU_MASK);
					reg_val |= ARM_SDIO0_4BIT_PU_VAL;
				}
				else
				{
					reg_val &= (~ARM_SDIO0_8BIT_PU_MASK);
					reg_val |= ARM_SDIO0_8BIT_PU_VAL;
				}
			break;
		case SDIO1:
		default:
				if (bw == SDIO_BW_1BIT)	
				{
					reg_val &= (~ARM_SDIO1_1BIT_PU_MASK);
					reg_val |= ARM_SDIO1_1BIT_PU_VAL;
				}
				else if (bw == SDIO_BW_4BIT)	
				{
					reg_val &= (~ARM_SDIO1_4BIT_PU_MASK);
					reg_val |= ARM_SDIO1_4BIT_PU_VAL;
				}
				else
				{
					reg_val &= (~ARM_SDIO1_8BIT_PU_MASK);
					reg_val |= ARM_SDIO1_8BIT_PU_VAL;
				}
			break;
	}	

	__raw_writel(reg_val, ARM_SD_PU_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_spi_pu(E_SPI_PORT port)
//
//  Parameters:
// 		port	: spi port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_spi_pu(E_SPI_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SPI_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SPI_I2C_PU_REG);
	switch (port)
	{
		case SPI0:
				reg_val &= (~ARM_SPI0_PU_MASK);
				reg_val |= ARM_SPI0_PU_VAL;
			break;
		case SPI1:
				reg_val &= (~ARM_SPI1_PU_MASK);
				reg_val |= ARM_SPI1_PU_VAL;
			break;
		case SPI2:
		default:	
				reg_val &= (~ARM_SPI2_PU_MASK);
				reg_val |= ARM_SPI2_PU_VAL;
			break;
	}

	__raw_writel(reg_val, ARM_SPI_I2C_PU_REG);
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_i2c_pu(E_I2C_PORT port)
//
//  Parameters:
// 		port	: i2c port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_i2c_pu(E_I2C_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_I2C_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SPI_I2C_PU_REG);
	switch (port)
	{
		case I2C0:
				reg_val &= (~ARM_I2C0_PU_MASK);
				reg_val |= ARM_I2C0_PU_VAL;
			break;
		case I2C1:
				reg_val &= (~ARM_I2C1_PU_MASK);
				reg_val |= ARM_I2C1_PU_VAL;
			break;
		case I2C2:
		default:	
				reg_val &= (~ARM_I2C2_PU_MASK);
				reg_val |= ARM_I2C2_PU_VAL;
			break;
	}	

	__raw_writel(reg_val, ARM_SPI_I2C_PU_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioa_pu(E_PIN_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port	: gpio pin port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpioa_pu(E_PIN_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_19))
	{
		reg_val = __raw_readl(ARM_UART_PU_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOA0_PU_MASK);
					reg_val |= ARM_GPIOA0_PU_VAL(pu);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOA1_PU_MASK);
					reg_val |= ARM_GPIOA1_PU_VAL(pu);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOA2_PU_MASK);
					reg_val |= ARM_GPIOA2_PU_VAL(pu);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOA3_PU_MASK);
					reg_val |= ARM_GPIOA3_PU_VAL(pu);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOA4_PU_MASK);
					reg_val |= ARM_GPIOA4_PU_VAL(pu);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOA5_PU_MASK);
					reg_val |= ARM_GPIOA5_PU_VAL(pu);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOA6_PU_MASK);
					reg_val |= ARM_GPIOA6_PU_VAL(pu);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOA7_PU_MASK);
					reg_val |= ARM_GPIOA7_PU_VAL(pu);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOA8_PU_MASK);
					reg_val |= ARM_GPIOA8_PU_VAL(pu);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOA9_PU_MASK);
					reg_val |= ARM_GPIOA9_PU_VAL(pu);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOA10_PU_MASK);
					reg_val |= ARM_GPIOA10_PU_VAL(pu);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOA11_PU_MASK);
					reg_val |= ARM_GPIOA11_PU_VAL(pu);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOA12_PU_MASK);
					reg_val |= ARM_GPIOA12_PU_VAL(pu);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOA13_PU_MASK);
					reg_val |= ARM_GPIOA13_PU_VAL(pu);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOA14_PU_MASK);
					reg_val |= ARM_GPIOA14_PU_VAL(pu);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOA15_PU_MASK);
					reg_val |= ARM_GPIOA15_PU_VAL(pu);
				break;
			case PIN_16:
					reg_val &= (~ARM_GPIOA16_PU_MASK);
					reg_val |= ARM_GPIOA16_PU_VAL(pu);
				break;
			case PIN_17:
					reg_val &= (~ARM_GPIOA17_PU_MASK);
					reg_val |= ARM_GPIOA17_PU_VAL(pu);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOA18_PU_MASK);
					reg_val |= ARM_GPIOA18_PU_VAL(pu);
				break;
			case PIN_19:
			default :	
					reg_val &= (~ARM_GPIOA19_PU_MASK);
					reg_val |= ARM_GPIOA19_PU_VAL(pu);
				break;
		}
	
		__raw_writel(reg_val, ARM_UART_PU_REG);
	}
	else if ((port >= PIN_20) && (port <= PIN_25))
	{
		reg_val = __raw_readl(ARM_SPI_I2C_PU_REG);
	
		switch (port)
		{
			case PIN_20:
					reg_val &= (~ARM_GPIOA20_PU_MASK);
					reg_val |= ARM_GPIOA20_PU_VAL(pu);
				break;
			case PIN_21:
					reg_val &= (~ARM_GPIOA21_PU_MASK);
					reg_val |= ARM_GPIOA21_PU_VAL(pu);
				break;
			case PIN_22:
					reg_val &= (~ARM_GPIOA22_PU_MASK);
					reg_val |= ARM_GPIOA22_PU_VAL(pu);
				break;
			case PIN_23:
					reg_val &= (~ARM_GPIOA23_PU_MASK);
					reg_val |= ARM_GPIOA23_PU_VAL(pu);
				break;
			case PIN_24:
					reg_val &= (~ARM_GPIOA24_PU_MASK);
					reg_val |= ARM_GPIOA24_PU_VAL(pu);
				break;
			case PIN_25:
			default :	
					reg_val &= (~ARM_GPIOA25_PU_MASK);
					reg_val |= ARM_GPIOA25_PU_VAL(pu);
				break;
		}

		__raw_writel(reg_val, ARM_SPI_I2C_PU_REG);
	}
	else if ((port >= PIN_29) && (port <= PIN_31))
	{
		reg_val = __raw_readl(ARM_PU_REG);
	
		switch (port)
		{
			case PIN_29:
					reg_val &= (~ARM_GPIOA29_PU_MASK);
					reg_val |= ARM_GPIOA29_PU_VAL(pu);
				break;
			case PIN_30:
					reg_val &= (~ARM_GPIOA30_PU_MASK);
					reg_val |= ARM_GPIOA30_PU_VAL(pu);
				break;
			case PIN_31:
			default :	
					reg_val &= (~ARM_GPIOA31_PU_MASK);
					reg_val |= ARM_GPIOA31_PU_VAL(pu);
				break;
		}

		__raw_writel(reg_val, ARM_PU_REG);
	}
	else
	{}


	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiob_pu(E_PIN_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port	: gpio pin port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpiob_pu(E_PIN_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_16))
	{
		reg_val = __raw_readl(ARM_SPI_I2C_PU_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOB0_PU_MASK);
					reg_val |= ARM_GPIOB0_PU_VAL(pu);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOB1_PU_MASK);
					reg_val |= ARM_GPIOB1_PU_VAL(pu);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOB2_PU_MASK);
					reg_val |= ARM_GPIOB2_PU_VAL(pu);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOB3_PU_MASK);
					reg_val |= ARM_GPIOB3_PU_VAL(pu);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOB4_PU_MASK);
					reg_val |= ARM_GPIOB4_PU_VAL(pu);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOB5_PU_MASK);
					reg_val |= ARM_GPIOB5_PU_VAL(pu);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOB6_PU_MASK);
					reg_val |= ARM_GPIOB6_PU_VAL(pu);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOB7_PU_MASK);
					reg_val |= ARM_GPIOB7_PU_VAL(pu);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOB8_PU_MASK);
					reg_val |= ARM_GPIOB8_PU_VAL(pu);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOB9_PU_MASK);
					reg_val |= ARM_GPIOB9_PU_VAL(pu);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOB10_PU_MASK);
					reg_val |= ARM_GPIOB10_PU_VAL(pu);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOB11_PU_MASK);
					reg_val |= ARM_GPIOB11_PU_VAL(pu);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOB12_PU_MASK);
					reg_val |= ARM_GPIOB12_PU_VAL(pu);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOB13_PU_MASK);
					reg_val |= ARM_GPIOB13_PU_VAL(pu);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOB14_PU_MASK);
					reg_val |= ARM_GPIOB14_PU_VAL(pu);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOB15_PU_MASK);
					reg_val |= ARM_GPIOB15_PU_VAL(pu);
				break;
			case PIN_16:
			default :	
					reg_val &= (~ARM_GPIOB16_PU_MASK);
					reg_val |= ARM_GPIOB16_PU_VAL(pu);
				break;
		}

		__raw_writel(reg_val, ARM_SPI_I2C_PU_REG);
	}
	else if ((port >= PIN_17) && (port <= PIN_31))
	{
		reg_val = __raw_readl(ARM_PU_REG);
	
		switch (port)
		{
			case PIN_17:
					reg_val &= (~ARM_GPIOB17_PU_MASK);
					reg_val |= ARM_GPIOB17_PU_VAL(pu);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOB18_PU_MASK);
					reg_val |= ARM_GPIOB18_PU_VAL(pu);
				break;
			case PIN_19:
					reg_val &= (~ARM_GPIOB19_PU_MASK);
					reg_val |= ARM_GPIOB19_PU_VAL(pu);
				break;
			case PIN_20:
					reg_val &= (~ARM_GPIOB20_PU_MASK);
					reg_val |= ARM_GPIOB20_PU_VAL(pu);
				break;
			case PIN_21:
					reg_val &= (~ARM_GPIOB21_PU_MASK);
					reg_val |= ARM_GPIOB21_PU_VAL(pu);
				break;
			case PIN_22:
					reg_val &= (~ARM_GPIOB22_PU_MASK);
					reg_val |= ARM_GPIOB22_PU_VAL(pu);
				break;
			case PIN_23:
					reg_val &= (~ARM_GPIOB23_PU_MASK);
					reg_val |= ARM_GPIOB23_PU_VAL(pu);
				break;
			case PIN_24:
					reg_val &= (~ARM_GPIOB24_PU_MASK);
					reg_val |= ARM_GPIOB24_PU_VAL(pu);
				break;
			case PIN_25:
					reg_val &= (~ARM_GPIOB25_PU_MASK);
					reg_val |= ARM_GPIOB25_PU_VAL(pu);
				break;
			case PIN_26:
					reg_val &= (~ARM_GPIOB26_PU_MASK);
					reg_val |= ARM_GPIOB26_PU_VAL(pu);
				break;
			case PIN_27:
					reg_val &= (~ARM_GPIOB27_PU_MASK);
					reg_val |= ARM_GPIOB27_PU_VAL(pu);
				break;
			case PIN_28:
					reg_val &= (~ARM_GPIOB28_PU_MASK);
					reg_val |= ARM_GPIOB28_PU_VAL(pu);
				break;	
			case PIN_29:
					reg_val &= (~ARM_GPIOB29_PU_MASK);
					reg_val |= ARM_GPIOB29_PU_VAL(pu);
				break;			
			case PIN_30:
					reg_val &= (~ARM_GPIOB30_PU_MASK);
					reg_val |= ARM_GPIOB30_PU_VAL(pu);
				break;
			case PIN_31:
			default :	
					reg_val &= (~ARM_GPIOB31_PU_MASK);
					reg_val |= ARM_GPIOB31_PU_VAL(pu);
				break;
		}

		__raw_writel(reg_val, ARM_PU_REG);
	}
	else
	{}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioc_pu(E_PIN_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port	: gpio pin port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpioc_pu(E_PIN_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0) && (port <= PIN_9)) || 
		((port >= PIN_10) && (port <= PIN_19)) || 
		((port >= PIN_25) && (port <= PIN_26)) ||
		((port >= PIN_27) && (port <= PIN_30)))
	{
		reg_val = __raw_readl(ARM_SD_PU_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOC0_PU_MASK);
					reg_val |= ARM_GPIOC0_PU_VAL(pu);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOC1_PU_MASK);
					reg_val |= ARM_GPIOC1_PU_VAL(pu);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOC2_PU_MASK);
					reg_val |= ARM_GPIOC2_PU_VAL(pu);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOC3_PU_MASK);
					reg_val |= ARM_GPIOC3_PU_VAL(pu);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOC4_PU_MASK);
					reg_val |= ARM_GPIOC4_PU_VAL(pu);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOC5_PU_MASK);
					reg_val |= ARM_GPIOC5_PU_VAL(pu);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOC6_PU_MASK);
					reg_val |= ARM_GPIOC6_PU_VAL(pu);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOC7_PU_MASK);
					reg_val |= ARM_GPIOC7_PU_VAL(pu);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOC8_PU_MASK);
					reg_val |= ARM_GPIOC8_PU_VAL(pu);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOC9_PU_MASK);
					reg_val |= ARM_GPIOC9_PU_VAL(pu);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOC10_PU_MASK);
					reg_val |= ARM_GPIOC10_PU_VAL(pu);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOC11_PU_MASK);
					reg_val |= ARM_GPIOC11_PU_VAL(pu);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOC12_PU_MASK);
					reg_val |= ARM_GPIOC12_PU_VAL(pu);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOC13_PU_MASK);
					reg_val |= ARM_GPIOC13_PU_VAL(pu);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOC14_PU_MASK);
					reg_val |= ARM_GPIOC14_PU_VAL(pu);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOC15_PU_MASK);
					reg_val |= ARM_GPIOC15_PU_VAL(pu);
				break;
			case PIN_16:
					reg_val &= (~ARM_GPIOC16_PU_MASK);
					reg_val |= ARM_GPIOC16_PU_VAL(pu);
				break;
			case PIN_17:
					reg_val &= (~ARM_GPIOC17_PU_MASK);
					reg_val |= ARM_GPIOC17_PU_VAL(pu);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOC18_PU_MASK);
					reg_val |= ARM_GPIOC18_PU_VAL(pu);
				break;
			case PIN_19:
					reg_val &= (~ARM_GPIOC19_PU_MASK);
					reg_val |= ARM_GPIOC19_PU_VAL(pu);
				break;
			case PIN_25:
					reg_val &= (~ARM_GPIOC25_PU_MASK);
					reg_val |= ARM_GPIOC25_PU_VAL(pu);
				break;
			case PIN_26:
					reg_val &= (~ARM_GPIOC26_PU_MASK);
					reg_val |= ARM_GPIOC26_PU_VAL(pu);
				break;
			case PIN_27:
					reg_val &= (~ARM_GPIOC27_PU_MASK);
					reg_val |= ARM_GPIOC27_PU_VAL(pu);
				break;
			case PIN_28:
					reg_val &= (~ARM_GPIOC28_PU_MASK);
					reg_val |= ARM_GPIOC28_PU_VAL(pu);
				break;
			case PIN_29:
					reg_val &= (~ARM_GPIOC29_PU_MASK);
					reg_val |= ARM_GPIOC29_PU_VAL(pu);
				break;	
			case PIN_30:
			default	:	
					reg_val &= (~ARM_GPIOC30_PU_MASK);
					reg_val |= ARM_GPIOC30_PU_VAL(pu);
				break;
		}
	
		__raw_writel(reg_val, ARM_SD_PU_REG);
	}
	else 
	{}
	

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiod_pu(E_PIN_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port	: gpio pin port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpiod_pu(E_PIN_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0)  && (port <= PIN_3)) || 
		((port >= PIN_29) && (port <= PIN_31)))
	{
		reg_val = __raw_readl(ARM_PU_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOD0_PU_MASK);
					reg_val |= ARM_GPIOD0_PU_VAL(pu);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOD1_PU_MASK);
					reg_val |= ARM_GPIOD1_PU_VAL(pu);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOD2_PU_MASK);
					reg_val |= ARM_GPIOD2_PU_VAL(pu);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOD3_PU_MASK);
					reg_val |= ARM_GPIOD3_PU_VAL(pu);
				break;			
			case PIN_29:
					reg_val &= (~ARM_GPIOD29_PU_MASK);
					reg_val |= ARM_GPIOD29_PU_VAL(pu);
				break;
			case PIN_30:
					reg_val &= (~ARM_GPIOD30_PU_MASK);
					reg_val |= ARM_GPIOD30_PU_VAL(pu);
				break;
			case PIN_31:
			default	:	
					reg_val &= (~ARM_GPIOD31_PU_MASK);
					reg_val |= ARM_GPIOD31_PU_VAL(pu);
				break;
		}
	
		__raw_writel(reg_val, ARM_PU_REG);
	}
	else 
	{}
	
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpio_pu(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PU pu)
//
//  Parameters:
// 		port	: gpio port
//
// 		pin	: gpio pin
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpio_pu(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_GPIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	switch (port)
	{
		case GPIOA:
				if (true != misc_gpioa_pu(pin, pu))
				{
					return false;
				}
			break;
		case GPIOB:
				if (true != misc_gpiob_pu(pin, pu))
				{
					return false;
				}
			break;
		case GPIOC:				
				if (true != misc_gpioc_pu(pin, pu))
				{
					return false;
				}
			break;		
		case GPIOD:
		default:
				if (true != misc_gpiod_pu(pin, pu))
				{
					return false;
				}
			break;			
	}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_input_pu(E_TIMER_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port	: timer port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_timer_input_pu(E_TIMER_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= TIMER4)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SD_PU_REG);
	
	switch (port)
	{
		case TIMER0:
				reg_val &= (~ARM_TIMER0_IN_PU_MASK);
				reg_val |= ARM_TIMER0_IN_PU_VAL(pu);
			break;
		case TIMER1:
				reg_val &= (~ARM_TIMER1_IN_PU_MASK);
				reg_val |= ARM_TIMER1_IN_PU_VAL(pu);
			break;	
		case TIMER2:
				reg_val &= (~ARM_TIMER2_IN_PU_MASK);
				reg_val |= ARM_TIMER2_IN_PU_VAL(pu);
			break;			
		case TIMER3:
		default:
				reg_val &= (~ARM_TIMER3_IN_PU_MASK);
				reg_val |= ARM_TIMER3_IN_PU_VAL(pu);
			break;
	}

	__raw_writel(reg_val, ARM_SD_PU_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_output_pu(E_TIMER_WXYZ_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port		: timer w/x/y/z port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_timer_output_pu(E_TIMER_WXYZ_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the w/x/y/z port
	if (port >= NUM_TIMER_WXYZ_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_PU_REG);
	
	switch (port)
	{
		case TIMER_W:
				reg_val &= (~ARM_TIMERW_OUT_PU_MASK);
				reg_val |= ARM_TIMERW_OUT_PU_VAL(pu);
			break;
		case TIMER_X:
				reg_val &= (~ARM_TIMERX_OUT_PU_MASK);
				reg_val |= ARM_TIMERX_OUT_PU_VAL(pu);
			break;	
		case TIMER_Y:
				reg_val &= (~ARM_TIMERY_OUT_PU_MASK);
				reg_val |= ARM_TIMERY_OUT_PU_VAL(pu);
			break;			
		case TIMER_Z:
		default:
				reg_val &= (~ARM_TIMERZ_OUT_PU_MASK);
				reg_val |= ARM_TIMERZ_OUT_PU_VAL(pu);
			break;
	}

	__raw_writel(reg_val, ARM_PU_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_clkout_pu(E_IO_PU pu)
//
//  Parameters:
//		pu	: pull up or down
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_rtc_clkout_pu(E_IO_PU pu)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PU_REG);

	reg_val &= (~ARM_RTC_CLKOUT_PU_MASK);
	reg_val |= ARM_RTC_CLKOUT_PU_VAL(pu);

	__raw_writel(reg_val, ARM_PU_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_cal_clkout_pu(E_IO_PU pu)
//
//  Parameters:
//		pu	: pull up or down
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_rtc_cal_clkout_pu(E_IO_PU pu)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PU_REG);

	reg_val &= (~ARM_RTC_CAL_CLKOUT_PU_MASK);
	reg_val |= ARM_RTC_CAL_CLKOUT_PU_VAL(pu);

	__raw_writel(reg_val, ARM_PU_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_clkout_pu(E_IO_PU pu)
//
//  Parameters:
//		pu	: pull up or down
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_clkout_pu(E_IO_PU pu)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PU_REG);

	reg_val &= (~ARM_CLKOUT_PU_MASK);
	reg_val |= ARM_CLKOUT_PU_VAL(pu);

	__raw_writel(reg_val, ARM_PU_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_host_low_power_out_pu(E_IO_PU pu)
//
//  Parameters:
//		pu	: pull up or down
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_host_low_power_out_pu(E_IO_PU pu)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PU_REG);

	reg_val &= (~ARM_HOST_LOW_POWER_OUT_PU_MASK);
	reg_val |= ARM_HOST_LOW_POWER_OUT_PU_VAL(pu);

	__raw_writel(reg_val, ARM_PU_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_failsafe_timer_out_pu(E_IO_PU pu)
//
//  Parameters:
//		pu	: pull up or down
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_epf_failsafe_timer_out_pu(E_IO_PU pu)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PU_REG);

	reg_val &= (~ARM_EPF_FAILSAFE_TIMER_OUT_PU_MASK);
	reg_val |= ARM_EPF_FAILSAFE_TIMER_OUT_PU_VAL(pu);

	__raw_writel(reg_val, ARM_PU_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_sleep_mode_done_pu(E_IO_PU pu)
//
//  Parameters:
//		pu	: pull up or down
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_epf_sleep_mode_done_pu(E_IO_PU pu)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PU_REG);

	reg_val &= (~ARM_EPF_SLEEP_MODE_DONE_PU_MASK);
	reg_val |= ARM_EPF_SLEEP_MODE_DONE_PU_VAL(pu);

	__raw_writel(reg_val, ARM_PU_REG);
}

/*************************************************************************************************************/
//  Function: static bool misc_usb_oc_pu(E_TIMER_WXYZ_PORT port, E_IO_PU pu)
//
//  Parameters:
// 		port	: usb port
//
//		pu	: pull up or down
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_usb_oc_pu(E_USB_PORT port, E_IO_PU pu)
{
	uint32_t reg_val;
	
	// Check the validity of the usb port
	if (port >= NUM_USB_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_PU_REG);
	
	switch (port)
	{
		case USB0:
				reg_val &= (~ARM_USB0_OC_PU_MASK);
				reg_val |= ARM_USB0_OC_PU_VAL(pu);
			break;		
		case USB1:
		default:
				reg_val &= (~ARM_USB1_OC_PU_MASK);
				reg_val |= ARM_USB1_OC_PU_VAL(pu);
			break;
	}

	__raw_writel(reg_val, ARM_PU_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_uart_pd(E_UART_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: uart port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_uart_pd(E_UART_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;

	// Check the validity of the port
	if (port >= NUM_UART_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_UART_PD_REG);

	switch (port)
	{
		case UART0:
				reg_val &= (~ARM_UART0_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_UART0_PU_VAL;
				}	
			break;
		case UART1:
				reg_val &= (~ARM_UART1_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_UART1_PU_VAL;
				}	
			break;
		case UART2:
				reg_val &= (~ARM_UART2_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_UART2_PU_VAL;
				}				
			break;
		case UART3:
		default:	
				reg_val &= (~ARM_UART3_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_UART3_PU_VAL;
				}			
			break;
	}

	__raw_writel(reg_val, ARM_UART_PD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_sdio_pd(E_SDIO_PORT port, E_IO_PD pd, E_SDIO_BW bw)
//
//  Parameters:
// 		port	: sdio port
//
// 		pd	: pull up/down is active/disabled
//
//		bw	: sdio bus width
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_sdio_pd(E_SDIO_PORT port, E_IO_PD pd, E_SDIO_BW bw)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SDIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SD_PD_REG);
	
	switch (port)
	{
		case SDIO0:
				if (bw == SDIO_BW_1BIT)	
				{
					reg_val &= (~ARM_SDIO0_1BIT_PU_MASK);

					if (pd == IO_PD_DIS)
					{
						reg_val |= ARM_SDIO0_1BIT_PU_VAL;
					}
				}
				else if (bw == SDIO_BW_4BIT)	
				{
					reg_val &= (~ARM_SDIO0_4BIT_PU_MASK);

					if (pd == IO_PD_DIS)
					{
						reg_val |= ARM_SDIO0_4BIT_PU_VAL;
					}
				}
				else
				{
					reg_val &= (~ARM_SDIO0_8BIT_PU_MASK);

					if (pd == IO_PD_DIS)
					{
						reg_val |= ARM_SDIO0_8BIT_PU_VAL;
					}
				}	
			break;
		case SDIO1:
		default:
				if (bw == SDIO_BW_1BIT) 
				{
					reg_val &= (~ARM_SDIO1_1BIT_PU_MASK);
				
					if (pd == IO_PD_DIS)
					{
						reg_val |= ARM_SDIO1_1BIT_PU_VAL;
					}
				}
				else if (bw == SDIO_BW_4BIT)	
				{
					reg_val &= (~ARM_SDIO1_4BIT_PU_MASK);
				
					if (pd == IO_PD_DIS)
					{
						reg_val |= ARM_SDIO1_4BIT_PU_VAL;
					}
				}
				else
				{
					reg_val &= (~ARM_SDIO1_8BIT_PU_MASK);
				
					if (pd == IO_PD_DIS)
					{
						reg_val |= ARM_SDIO1_8BIT_PU_VAL;
					}
				}
			break;
	}	

	__raw_writel(reg_val, ARM_SD_PD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_spi_pd(E_SPI_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: spi port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_spi_pd(E_SPI_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SPI_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SPI_I2C_PD_REG);
	switch (port)
	{
		case SPI0:
				reg_val &= (~ARM_SPI0_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_SPI0_PU_VAL;
				}	
			break;
		case SPI1:
				reg_val &= (~ARM_SPI1_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_SPI1_PU_VAL;
				}	
			break;
		case SPI2:
		default:	
				reg_val &= (~ARM_SPI2_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_SPI2_PU_VAL;
				}	
			break;
	}

	__raw_writel(reg_val, ARM_SPI_I2C_PD_REG);
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_i2c_pd(E_I2C_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: i2c port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_i2c_pd(E_I2C_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_I2C_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SPI_I2C_PD_REG);
	switch (port)
	{
		case I2C0:
				reg_val &= (~ARM_I2C0_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_I2C0_PU_VAL;
				}	
			break;
		case I2C1:
				reg_val &= (~ARM_I2C1_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_I2C1_PU_VAL;
				}	
			break;
		case I2C2:
		default:	
				reg_val &= (~ARM_I2C2_PU_MASK);

				if (pd == IO_PD_DIS)
				{
					reg_val |= ARM_I2C2_PU_VAL;
				}	
			break;
	}	

	__raw_writel(reg_val, ARM_SPI_I2C_PD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioa_pd(E_PIN_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpioa_pd(E_PIN_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_19))
	{
		reg_val = __raw_readl(ARM_UART_PD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOA0_PU_MASK);
					reg_val |= ARM_GPIOA0_PU_VAL(pd);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOA1_PU_MASK);
					reg_val |= ARM_GPIOA1_PU_VAL(pd);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOA2_PU_MASK);
					reg_val |= ARM_GPIOA2_PU_VAL(pd);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOA3_PU_MASK);
					reg_val |= ARM_GPIOA3_PU_VAL(pd);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOA4_PU_MASK);
					reg_val |= ARM_GPIOA4_PU_VAL(pd);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOA5_PU_MASK);
					reg_val |= ARM_GPIOA5_PU_VAL(pd);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOA6_PU_MASK);
					reg_val |= ARM_GPIOA6_PU_VAL(pd);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOA7_PU_MASK);
					reg_val |= ARM_GPIOA7_PU_VAL(pd);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOA8_PU_MASK);
					reg_val |= ARM_GPIOA8_PU_VAL(pd);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOA9_PU_MASK);
					reg_val |= ARM_GPIOA9_PU_VAL(pd);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOA10_PU_MASK);
					reg_val |= ARM_GPIOA10_PU_VAL(pd);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOA11_PU_MASK);
					reg_val |= ARM_GPIOA11_PU_VAL(pd);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOA12_PU_MASK);
					reg_val |= ARM_GPIOA12_PU_VAL(pd);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOA13_PU_MASK);
					reg_val |= ARM_GPIOA13_PU_VAL(pd);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOA14_PU_MASK);
					reg_val |= ARM_GPIOA14_PU_VAL(pd);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOA15_PU_MASK);
					reg_val |= ARM_GPIOA15_PU_VAL(pd);
				break;
			case PIN_16:
					reg_val &= (~ARM_GPIOA16_PU_MASK);
					reg_val |= ARM_GPIOA16_PU_VAL(pd);
				break;
			case PIN_17:
					reg_val &= (~ARM_GPIOA17_PU_MASK);
					reg_val |= ARM_GPIOA17_PU_VAL(pd);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOA18_PU_MASK);
					reg_val |= ARM_GPIOA18_PU_VAL(pd);
				break;
			case PIN_19:
			default :	
					reg_val &= (~ARM_GPIOA19_PU_MASK);
					reg_val |= ARM_GPIOA19_PU_VAL(pd);
				break;
		}
	
		__raw_writel(reg_val, ARM_UART_PD_REG);
	}
	else if ((port >= PIN_20) && (port <= PIN_25))
	{
		reg_val = __raw_readl(ARM_SPI_I2C_PD_REG);
	
		switch (port)
		{
			case PIN_20:
					reg_val &= (~ARM_GPIOA20_PU_MASK);
					reg_val |= ARM_GPIOA20_PU_VAL(pd);
				break;
			case PIN_21:
					reg_val &= (~ARM_GPIOA21_PU_MASK);
					reg_val |= ARM_GPIOA21_PU_VAL(pd);
				break;
			case PIN_22:
					reg_val &= (~ARM_GPIOA22_PU_MASK);
					reg_val |= ARM_GPIOA22_PU_VAL(pd);
				break;
			case PIN_23:
					reg_val &= (~ARM_GPIOA23_PU_MASK);
					reg_val |= ARM_GPIOA23_PU_VAL(pd);
				break;
			case PIN_24:
					reg_val &= (~ARM_GPIOA24_PU_MASK);
					reg_val |= ARM_GPIOA24_PU_VAL(pd);
				break;
			case PIN_25:
			default :	
					reg_val &= (~ARM_GPIOA25_PU_MASK);
					reg_val |= ARM_GPIOA25_PU_VAL(pd);
				break;
		}

		__raw_writel(reg_val, ARM_SPI_I2C_PD_REG);
	}
	else if ((port >= PIN_29) && (port <= PIN_31))
	{
		reg_val = __raw_readl(ARM_PD_REG);
	
		switch (port)
		{
			case PIN_29:
					reg_val &= (~ARM_GPIOA29_PU_MASK);
					reg_val |= ARM_GPIOA29_PU_VAL(pd);
				break;
			case PIN_30:
					reg_val &= (~ARM_GPIOA30_PU_MASK);
					reg_val |= ARM_GPIOA30_PU_VAL(pd);
				break;
			case PIN_31:
			default :	
					reg_val &= (~ARM_GPIOA31_PU_MASK);
					reg_val |= ARM_GPIOA31_PU_VAL(pd);
				break;
		}

		__raw_writel(reg_val, ARM_PD_REG);
	}
	else
	{}


	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiob_pd(E_PIN_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpiob_pd(E_PIN_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_16))
	{
		reg_val = __raw_readl(ARM_SPI_I2C_PD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOB0_PU_MASK);
					reg_val |= ARM_GPIOB0_PU_VAL(pd);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOB1_PU_MASK);
					reg_val |= ARM_GPIOB1_PU_VAL(pd);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOB2_PU_MASK);
					reg_val |= ARM_GPIOB2_PU_VAL(pd);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOB3_PU_MASK);
					reg_val |= ARM_GPIOB3_PU_VAL(pd);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOB4_PU_MASK);
					reg_val |= ARM_GPIOB4_PU_VAL(pd);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOB5_PU_MASK);
					reg_val |= ARM_GPIOB5_PU_VAL(pd);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOB6_PU_MASK);
					reg_val |= ARM_GPIOB6_PU_VAL(pd);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOB7_PU_MASK);
					reg_val |= ARM_GPIOB7_PU_VAL(pd);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOB8_PU_MASK);
					reg_val |= ARM_GPIOB8_PU_VAL(pd);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOB9_PU_MASK);
					reg_val |= ARM_GPIOB9_PU_VAL(pd);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOB10_PU_MASK);
					reg_val |= ARM_GPIOB10_PU_VAL(pd);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOB11_PU_MASK);
					reg_val |= ARM_GPIOB11_PU_VAL(pd);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOB12_PU_MASK);
					reg_val |= ARM_GPIOB12_PU_VAL(pd);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOB13_PU_MASK);
					reg_val |= ARM_GPIOB13_PU_VAL(pd);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOB14_PU_MASK);
					reg_val |= ARM_GPIOB14_PU_VAL(pd);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOB15_PU_MASK);
					reg_val |= ARM_GPIOB15_PU_VAL(pd);
				break;
			case PIN_16:
			default :	
					reg_val &= (~ARM_GPIOB16_PU_MASK);
					reg_val |= ARM_GPIOB16_PU_VAL(pd);
				break;
		}

		__raw_writel(reg_val, ARM_SPI_I2C_PD_REG);
	}
	else if ((port >= PIN_17) && (port <= PIN_31))
	{
		reg_val = __raw_readl(ARM_PD_REG);
	
		switch (port)
		{
			case PIN_17:
					reg_val &= (~ARM_GPIOB17_PU_MASK);
					reg_val |= ARM_GPIOB17_PU_VAL(pd);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOB18_PU_MASK);
					reg_val |= ARM_GPIOB18_PU_VAL(pd);
				break;
			case PIN_19:
					reg_val &= (~ARM_GPIOB19_PU_MASK);
					reg_val |= ARM_GPIOB19_PU_VAL(pd);
				break;
			case PIN_20:
					reg_val &= (~ARM_GPIOB20_PU_MASK);
					reg_val |= ARM_GPIOB20_PU_VAL(pd);
				break;
			case PIN_21:
					reg_val &= (~ARM_GPIOB21_PU_MASK);
					reg_val |= ARM_GPIOB21_PU_VAL(pd);
				break;
			case PIN_22:
					reg_val &= (~ARM_GPIOB22_PU_MASK);
					reg_val |= ARM_GPIOB22_PU_VAL(pd);
				break;
			case PIN_23:
					reg_val &= (~ARM_GPIOB23_PU_MASK);
					reg_val |= ARM_GPIOB23_PU_VAL(pd);
				break;
			case PIN_24:
					reg_val &= (~ARM_GPIOB24_PU_MASK);
					reg_val |= ARM_GPIOB24_PU_VAL(pd);
				break;
			case PIN_25:
					reg_val &= (~ARM_GPIOB25_PU_MASK);
					reg_val |= ARM_GPIOB25_PU_VAL(pd);
				break;
			case PIN_26:
					reg_val &= (~ARM_GPIOB26_PU_MASK);
					reg_val |= ARM_GPIOB26_PU_VAL(pd);
				break;
			case PIN_27:
					reg_val &= (~ARM_GPIOB27_PU_MASK);
					reg_val |= ARM_GPIOB27_PU_VAL(pd);
				break;
			case PIN_28:
					reg_val &= (~ARM_GPIOB28_PU_MASK);
					reg_val |= ARM_GPIOB28_PU_VAL(pd);
				break;	
			case PIN_29:
					reg_val &= (~ARM_GPIOB29_PU_MASK);
					reg_val |= ARM_GPIOB29_PU_VAL(pd);
				break;			
			case PIN_30:
					reg_val &= (~ARM_GPIOB30_PU_MASK);
					reg_val |= ARM_GPIOB30_PU_VAL(pd);
				break;
			case PIN_31:
			default :	
					reg_val &= (~ARM_GPIOB31_PU_MASK);
					reg_val |= ARM_GPIOB31_PU_VAL(pd);
				break;
		}

		__raw_writel(reg_val, ARM_PD_REG);
	}
	else
	{}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioc_pd(E_PIN_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpioc_pd(E_PIN_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0) && (port <= PIN_9)) || 
		((port >= PIN_10) && (port <= PIN_19)) || 
		((port >= PIN_25) && (port <= PIN_26)) ||
		((port >= PIN_27) && (port <= PIN_30)))
	{
		reg_val = __raw_readl(ARM_SD_PD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOC0_PU_MASK);
					reg_val |= ARM_GPIOC0_PU_VAL(pd);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOC1_PU_MASK);
					reg_val |= ARM_GPIOC1_PU_VAL(pd);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOC2_PU_MASK);
					reg_val |= ARM_GPIOC2_PU_VAL(pd);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOC3_PU_MASK);
					reg_val |= ARM_GPIOC3_PU_VAL(pd);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOC4_PU_MASK);
					reg_val |= ARM_GPIOC4_PU_VAL(pd);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOC5_PU_MASK);
					reg_val |= ARM_GPIOC5_PU_VAL(pd);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOC6_PU_MASK);
					reg_val |= ARM_GPIOC6_PU_VAL(pd);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOC7_PU_MASK);
					reg_val |= ARM_GPIOC7_PU_VAL(pd);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOC8_PU_MASK);
					reg_val |= ARM_GPIOC8_PU_VAL(pd);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOC9_PU_MASK);
					reg_val |= ARM_GPIOC9_PU_VAL(pd);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOC10_PU_MASK);
					reg_val |= ARM_GPIOC10_PU_VAL(pd);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOC11_PU_MASK);
					reg_val |= ARM_GPIOC11_PU_VAL(pd);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOC12_PU_MASK);
					reg_val |= ARM_GPIOC12_PU_VAL(pd);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOC13_PU_MASK);
					reg_val |= ARM_GPIOC13_PU_VAL(pd);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOC14_PU_MASK);
					reg_val |= ARM_GPIOC14_PU_VAL(pd);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOC15_PU_MASK);
					reg_val |= ARM_GPIOC15_PU_VAL(pd);
				break;
			case PIN_16:
					reg_val &= (~ARM_GPIOC16_PU_MASK);
					reg_val |= ARM_GPIOC16_PU_VAL(pd);
				break;
			case PIN_17:
					reg_val &= (~ARM_GPIOC17_PU_MASK);
					reg_val |= ARM_GPIOC17_PU_VAL(pd);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOC18_PU_MASK);
					reg_val |= ARM_GPIOC18_PU_VAL(pd);
				break;
			case PIN_19:
					reg_val &= (~ARM_GPIOC19_PU_MASK);
					reg_val |= ARM_GPIOC19_PU_VAL(pd);
				break;
			case PIN_25:
					reg_val &= (~ARM_GPIOC25_PU_MASK);
					reg_val |= ARM_GPIOC25_PU_VAL(pd);
				break;
			case PIN_26:
					reg_val &= (~ARM_GPIOC26_PU_MASK);
					reg_val |= ARM_GPIOC26_PU_VAL(pd);
				break;
			case PIN_27:
					reg_val &= (~ARM_GPIOC27_PU_MASK);
					reg_val |= ARM_GPIOC27_PU_VAL(pd);
				break;
			case PIN_28:
					reg_val &= (~ARM_GPIOC28_PU_MASK);
					reg_val |= ARM_GPIOC28_PU_VAL(pd);
				break;
			case PIN_29:
					reg_val &= (~ARM_GPIOC29_PU_MASK);
					reg_val |= ARM_GPIOC29_PU_VAL(pd);
				break;	
			case PIN_30:
			default	:	
					reg_val &= (~ARM_GPIOC30_PU_MASK);
					reg_val |= ARM_GPIOC30_PU_VAL(pd);
				break;
		}
	
		__raw_writel(reg_val, ARM_SD_PD_REG);
	}
	else 
	{}
	

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiod_pd(E_PIN_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpiod_pd(E_PIN_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0)  && (port <= PIN_3)) || 
		((port >= PIN_29) && (port <= PIN_31)))
	{
		reg_val = __raw_readl(ARM_PD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOD0_PU_MASK);
					reg_val |= ARM_GPIOD0_PU_VAL(pd);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOD1_PU_MASK);
					reg_val |= ARM_GPIOD1_PU_VAL(pd);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOD2_PU_MASK);
					reg_val |= ARM_GPIOD2_PU_VAL(pd);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOD3_PU_MASK);
					reg_val |= ARM_GPIOD3_PU_VAL(pd);
				break;			
			case PIN_29:
					reg_val &= (~ARM_GPIOD29_PU_MASK);
					reg_val |= ARM_GPIOD29_PU_VAL(pd);
				break;
			case PIN_30:
					reg_val &= (~ARM_GPIOD30_PU_MASK);
					reg_val |= ARM_GPIOD30_PU_VAL(pd);
				break;
			case PIN_31:
			default	:	
					reg_val &= (~ARM_GPIOD31_PU_MASK);
					reg_val |= ARM_GPIOD31_PU_VAL(pd);
				break;
		}
	
		__raw_writel(reg_val, ARM_PD_REG);
	}
	else 
	{}
	
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpio_pd(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PD pd)
//
//  Parameters:
// 		port	: gpio port
//
// 		pin	: gpio pin
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_gpio_pd(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_GPIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	switch (port)
	{
		case GPIOA:
				if (true != misc_gpioa_pd(pin, pd))
				{
					return false;
				}
			break;
		case GPIOB:
				if (true != misc_gpiob_pd(pin, pd))
				{
					return false;
				}
			break;
		case GPIOC:				
				if (true != misc_gpioc_pd(pin, pd))
				{
					return false;
				}
			break;		
		case GPIOD:
		default:
				if (true != misc_gpiod_pd(pin, pd))
				{
					return false;
				}
			break;			
	}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_input_pd(E_TIMER_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port	: timer port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_timer_input_pd(E_TIMER_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= TIMER4)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SD_PD_REG);
	
	switch (port)
	{
		case TIMER0:
				reg_val &= (~ARM_TIMER0_IN_PU_MASK);
				reg_val |= ARM_TIMER0_IN_PU_VAL(pd);
			break;
		case TIMER1:
				reg_val &= (~ARM_TIMER1_IN_PU_MASK);
				reg_val |= ARM_TIMER1_IN_PU_VAL(pd);
			break;	
		case TIMER2:
				reg_val &= (~ARM_TIMER2_IN_PU_MASK);
				reg_val |= ARM_TIMER2_IN_PU_VAL(pd);
			break;			
		case TIMER3:
		default:
				reg_val &= (~ARM_TIMER3_IN_PU_MASK);
				reg_val |= ARM_TIMER3_IN_PU_VAL(pd);
			break;
	}

	__raw_writel(reg_val, ARM_SD_PD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_output_pd(E_TIMER_WXYZ_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port		: timer w/x/y/z port
//
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_timer_output_pd(E_TIMER_WXYZ_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the w/x/y/z port
	if (port >= NUM_TIMER_WXYZ_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_PD_REG);
	
	switch (port)
	{
		case TIMER_W:
				reg_val &= (~ARM_TIMERW_OUT_PU_MASK);
				reg_val |= ARM_TIMERW_OUT_PU_VAL(pd);
			break;
		case TIMER_X:
				reg_val &= (~ARM_TIMERX_OUT_PU_MASK);
				reg_val |= ARM_TIMERX_OUT_PU_VAL(pd);
			break;	
		case TIMER_Y:
				reg_val &= (~ARM_TIMERY_OUT_PU_MASK);
				reg_val |= ARM_TIMERY_OUT_PU_VAL(pd);
			break;			
		case TIMER_Z:
		default:
				reg_val &= (~ARM_TIMERZ_OUT_PU_MASK);
				reg_val |= ARM_TIMERZ_OUT_PU_VAL(pd);
			break;
	}

	__raw_writel(reg_val, ARM_PD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_clkout_pd(E_IO_PD pd)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_rtc_clkout_pd(E_IO_PD pd)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PD_REG);

	reg_val &= (~ARM_RTC_CLKOUT_PU_MASK);
	reg_val |= ARM_RTC_CLKOUT_PU_VAL(pd);

	__raw_writel(reg_val, ARM_PD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_cal_clkout_pd(E_IO_PD pd)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_rtc_cal_clkout_pd(E_IO_PD pd)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PD_REG);

	reg_val &= (~ARM_RTC_CAL_CLKOUT_PU_MASK);
	reg_val |= ARM_RTC_CAL_CLKOUT_PU_VAL(pd);

	__raw_writel(reg_val, ARM_PD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_clkout_pd(E_IO_PD pd)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_clkout_pd(E_IO_PD pd)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PD_REG);

	reg_val &= (~ARM_CLKOUT_PU_MASK);
	reg_val |= ARM_CLKOUT_PU_VAL(pd);

	__raw_writel(reg_val, ARM_PD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_host_low_power_out_pd(E_IO_PD pd)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_host_low_power_out_pd(E_IO_PD pd)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PD_REG);

	reg_val &= (~ARM_HOST_LOW_POWER_OUT_PU_MASK);
	reg_val |= ARM_HOST_LOW_POWER_OUT_PU_VAL(pd);

	__raw_writel(reg_val, ARM_PD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_failsafe_timer_out_pd(E_IO_PD pd)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_epf_failsafe_timer_out_pd(E_IO_PD pd)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PD_REG);

	reg_val &= (~ARM_EPF_FAILSAFE_TIMER_OUT_PU_MASK);
	reg_val |= ARM_EPF_FAILSAFE_TIMER_OUT_PU_VAL(pd);

	__raw_writel(reg_val, ARM_PD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_sleep_mode_done_pd(E_IO_PD pd)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static void misc_epf_sleep_mode_done_pd(E_IO_PD pd)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_PD_REG);

	reg_val &= (~ARM_EPF_SLEEP_MODE_DONE_PU_MASK);
	reg_val |= ARM_EPF_SLEEP_MODE_DONE_PU_VAL(pd);

	__raw_writel(reg_val, ARM_PD_REG);
}

/*************************************************************************************************************/
//  Function: static bool misc_usb_oc_pd(E_TIMER_WXYZ_PORT port, E_IO_PD pd)
//
//  Parameters:
// 		port		: usb port
//
// 		pd		: pull up/down is active/disabled
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to pull up or down function.
//
/*************************************************************************************************************/
static bool misc_usb_oc_pd(E_USB_PORT port, E_IO_PD pd)
{
	uint32_t reg_val;
	
	// Check the validity of the usb port
	if (port >= NUM_USB_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_PD_REG);
	
	switch (port)
	{
		case USB0:
				reg_val &= (~ARM_USB0_OC_PU_MASK);
				reg_val |= ARM_USB0_OC_PU_VAL(pd);
			break;		
		case USB1:
		default:
				reg_val &= (~ARM_USB1_OC_PU_MASK);
				reg_val |= ARM_USB1_OC_PU_VAL(pd);
			break;
	}

	__raw_writel(reg_val, ARM_PD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_uart_od(E_UART_PORT port)
//
//  Parameters:
// 		port	: uart port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_uart_od(E_UART_PORT port)
{
	uint32_t reg_val;

	// Check the validity of the port
	if (port >= NUM_UART_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_UART_OD_REG);

	switch (port)
	{
		case UART0:
				reg_val &= (~ARM_UART0_PU_MASK);
			break;
		case UART1:
				reg_val &= (~ARM_UART1_PU_MASK);
			break;
		case UART2:
				reg_val &= (~ARM_UART2_PU_MASK);			
			break;
		case UART3:
		default:	
				reg_val &= (~ARM_UART3_PU_MASK);		
			break;
	}

	__raw_writel(reg_val, ARM_UART_OD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_sdio_od(E_SDIO_PORT port, E_SDIO_BW bw)
//
//  Parameters:
// 		port	: sdio port
//
//		bw	: sdio bus width
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_sdio_od(E_SDIO_PORT port, E_SDIO_BW bw)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SDIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SD_OD_REG);
	
	switch (port)
	{
		case SDIO0:
				if (bw == SDIO_BW_1BIT)	
				{
					reg_val &= (~ARM_SDIO0_1BIT_PU_MASK);
				}
				else if (bw == SDIO_BW_4BIT)	
				{
					reg_val &= (~ARM_SDIO0_4BIT_PU_MASK);
				}
				else
				{
					reg_val &= (~ARM_SDIO0_8BIT_PU_MASK);
				}
			break;
		case SDIO1:
		default:
				if (bw == SDIO_BW_1BIT)	
				{
					reg_val &= (~ARM_SDIO1_1BIT_PU_MASK);
				}
				else if (bw == SDIO_BW_4BIT)	
				{
					reg_val &= (~ARM_SDIO1_4BIT_PU_MASK);
				}
				else
				{
					reg_val &= (~ARM_SDIO1_8BIT_PU_MASK);
				}
			break;
	}	

	__raw_writel(reg_val, ARM_SD_OD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_spi_od(E_SPI_PORT port)
//
//  Parameters:
// 		port	: spi port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_spi_od(E_SPI_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_SPI_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SPI_I2C_OD_REG);
	switch (port)
	{
		case SPI0:
				reg_val &= (~ARM_SPI0_PU_MASK);
			break;
		case SPI1:
				reg_val &= (~ARM_SPI1_PU_MASK);
			break;
		case SPI2:
		default:	
				reg_val &= (~ARM_SPI2_PU_MASK);
			break;
	}

	__raw_writel(reg_val, ARM_SPI_I2C_OD_REG);
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_i2c_od(E_I2C_PORT port)
//
//  Parameters:
// 		port	: i2c port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_i2c_od(E_I2C_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_I2C_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SPI_I2C_OD_REG);
	switch (port)
	{
		case I2C0:
				reg_val &= (~ARM_I2C0_PU_MASK);
			break;
		case I2C1:
				reg_val &= (~ARM_I2C1_PU_MASK);
			break;
		case I2C2:
		default:	
				reg_val &= (~ARM_I2C2_PU_MASK);
			break;
	}	

	__raw_writel(reg_val, ARM_SPI_I2C_OD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioa_od(E_PIN_PORT port, E_IO_OD od)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		od	: normal or open drain 
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal or open drain function.
//
/*************************************************************************************************************/
static bool misc_gpioa_od(E_PIN_PORT port, E_IO_OD od)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_19))
	{
		reg_val = __raw_readl(ARM_UART_OD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOA0_PU_MASK);
					reg_val |= ARM_GPIOA0_PU_VAL(od);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOA1_PU_MASK);
					reg_val |= ARM_GPIOA1_PU_VAL(od);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOA2_PU_MASK);
					reg_val |= ARM_GPIOA2_PU_VAL(od);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOA3_PU_MASK);
					reg_val |= ARM_GPIOA3_PU_VAL(od);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOA4_PU_MASK);
					reg_val |= ARM_GPIOA4_PU_VAL(od);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOA5_PU_MASK);
					reg_val |= ARM_GPIOA5_PU_VAL(od);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOA6_PU_MASK);
					reg_val |= ARM_GPIOA6_PU_VAL(od);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOA7_PU_MASK);
					reg_val |= ARM_GPIOA7_PU_VAL(od);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOA8_PU_MASK);
					reg_val |= ARM_GPIOA8_PU_VAL(od);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOA9_PU_MASK);
					reg_val |= ARM_GPIOA9_PU_VAL(od);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOA10_PU_MASK);
					reg_val |= ARM_GPIOA10_PU_VAL(od);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOA11_PU_MASK);
					reg_val |= ARM_GPIOA11_PU_VAL(od);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOA12_PU_MASK);
					reg_val |= ARM_GPIOA12_PU_VAL(od);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOA13_PU_MASK);
					reg_val |= ARM_GPIOA13_PU_VAL(od);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOA14_PU_MASK);
					reg_val |= ARM_GPIOA14_PU_VAL(od);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOA15_PU_MASK);
					reg_val |= ARM_GPIOA15_PU_VAL(od);
				break;
			case PIN_16:
					reg_val &= (~ARM_GPIOA16_PU_MASK);
					reg_val |= ARM_GPIOA16_PU_VAL(od);
				break;
			case PIN_17:
					reg_val &= (~ARM_GPIOA17_PU_MASK);
					reg_val |= ARM_GPIOA17_PU_VAL(od);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOA18_PU_MASK);
					reg_val |= ARM_GPIOA18_PU_VAL(od);
				break;
			case PIN_19:
			default :	
					reg_val &= (~ARM_GPIOA19_PU_MASK);
					reg_val |= ARM_GPIOA19_PU_VAL(od);
				break;
		}
	
		__raw_writel(reg_val, ARM_UART_OD_REG);
	}
	else if ((port >= PIN_20) && (port <= PIN_25))
	{
		reg_val = __raw_readl(ARM_SPI_I2C_OD_REG);
	
		switch (port)
		{
			case PIN_20:
					reg_val &= (~ARM_GPIOA20_PU_MASK);
					reg_val |= ARM_GPIOA20_PU_VAL(od);
				break;
			case PIN_21:
					reg_val &= (~ARM_GPIOA21_PU_MASK);
					reg_val |= ARM_GPIOA21_PU_VAL(od);
				break;
			case PIN_22:
					reg_val &= (~ARM_GPIOA22_PU_MASK);
					reg_val |= ARM_GPIOA22_PU_VAL(od);
				break;
			case PIN_23:
					reg_val &= (~ARM_GPIOA23_PU_MASK);
					reg_val |= ARM_GPIOA23_PU_VAL(od);
				break;
			case PIN_24:
					reg_val &= (~ARM_GPIOA24_PU_MASK);
					reg_val |= ARM_GPIOA24_PU_VAL(od);
				break;
			case PIN_25:
			default :	
					reg_val &= (~ARM_GPIOA25_PU_MASK);
					reg_val |= ARM_GPIOA25_PU_VAL(od);
				break;
		}

		__raw_writel(reg_val, ARM_SPI_I2C_OD_REG);
	}
	else if ((port >= PIN_29) && (port <= PIN_31))
	{
		reg_val = __raw_readl(ARM_OD_REG);
	
		switch (port)
		{
			case PIN_29:
					reg_val &= (~ARM_GPIOA29_PU_MASK);
					reg_val |= ARM_GPIOA29_PU_VAL(od);
				break;
			case PIN_30:
					reg_val &= (~ARM_GPIOA30_PU_MASK);
					reg_val |= ARM_GPIOA30_PU_VAL(od);
				break;
			case PIN_31:
			default :	
					reg_val &= (~ARM_GPIOA31_PU_MASK);
					reg_val |= ARM_GPIOA31_PU_VAL(od);
				break;
		}

		__raw_writel(reg_val, ARM_OD_REG);
	}
	else
	{}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiob_od(E_PIN_PORT port, E_IO_OD od)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		od	: normal or open drain 
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal or open drain function.
//
/*************************************************************************************************************/
static bool misc_gpiob_od(E_PIN_PORT port, E_IO_OD od)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if ((port >= PIN_0) && (port <= PIN_16))
	{
		reg_val = __raw_readl(ARM_SPI_I2C_OD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOB0_PU_MASK);
					reg_val |= ARM_GPIOB0_PU_VAL(od);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOB1_PU_MASK);
					reg_val |= ARM_GPIOB1_PU_VAL(od);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOB2_PU_MASK);
					reg_val |= ARM_GPIOB2_PU_VAL(od);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOB3_PU_MASK);
					reg_val |= ARM_GPIOB3_PU_VAL(od);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOB4_PU_MASK);
					reg_val |= ARM_GPIOB4_PU_VAL(od);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOB5_PU_MASK);
					reg_val |= ARM_GPIOB5_PU_VAL(od);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOB6_PU_MASK);
					reg_val |= ARM_GPIOB6_PU_VAL(od);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOB7_PU_MASK);
					reg_val |= ARM_GPIOB7_PU_VAL(od);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOB8_PU_MASK);
					reg_val |= ARM_GPIOB8_PU_VAL(od);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOB9_PU_MASK);
					reg_val |= ARM_GPIOB9_PU_VAL(od);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOB10_PU_MASK);
					reg_val |= ARM_GPIOB10_PU_VAL(od);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOB11_PU_MASK);
					reg_val |= ARM_GPIOB11_PU_VAL(od);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOB12_PU_MASK);
					reg_val |= ARM_GPIOB12_PU_VAL(od);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOB13_PU_MASK);
					reg_val |= ARM_GPIOB13_PU_VAL(od);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOB14_PU_MASK);
					reg_val |= ARM_GPIOB14_PU_VAL(od);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOB15_PU_MASK);
					reg_val |= ARM_GPIOB15_PU_VAL(od);
				break;
			case PIN_16:
			default :	
					reg_val &= (~ARM_GPIOB16_PU_MASK);
					reg_val |= ARM_GPIOB16_PU_VAL(od);
				break;
		}

		__raw_writel(reg_val, ARM_SPI_I2C_OD_REG);
	}
	else if ((port >= PIN_17) && (port <= PIN_31))
	{
		reg_val = __raw_readl(ARM_OD_REG);
	
		switch (port)
		{
			case PIN_17:
					reg_val &= (~ARM_GPIOB17_PU_MASK);
					reg_val |= ARM_GPIOB17_PU_VAL(od);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOB18_PU_MASK);
					reg_val |= ARM_GPIOB18_PU_VAL(od);
				break;
			case PIN_19:
					reg_val &= (~ARM_GPIOB19_PU_MASK);
					reg_val |= ARM_GPIOB19_PU_VAL(od);
				break;
			case PIN_20:
					reg_val &= (~ARM_GPIOB20_PU_MASK);
					reg_val |= ARM_GPIOB20_PU_VAL(od);
				break;
			case PIN_21:
					reg_val &= (~ARM_GPIOB21_PU_MASK);
					reg_val |= ARM_GPIOB21_PU_VAL(od);
				break;
			case PIN_22:
					reg_val &= (~ARM_GPIOB22_PU_MASK);
					reg_val |= ARM_GPIOB22_PU_VAL(od);
				break;
			case PIN_23:
					reg_val &= (~ARM_GPIOB23_PU_MASK);
					reg_val |= ARM_GPIOB23_PU_VAL(od);
				break;
			case PIN_24:
					reg_val &= (~ARM_GPIOB24_PU_MASK);
					reg_val |= ARM_GPIOB24_PU_VAL(od);
				break;
			case PIN_25:
					reg_val &= (~ARM_GPIOB25_PU_MASK);
					reg_val |= ARM_GPIOB25_PU_VAL(od);
				break;
			case PIN_26:
					reg_val &= (~ARM_GPIOB26_PU_MASK);
					reg_val |= ARM_GPIOB26_PU_VAL(od);
				break;
			case PIN_27:
					reg_val &= (~ARM_GPIOB27_PU_MASK);
					reg_val |= ARM_GPIOB27_PU_VAL(od);
				break;
			case PIN_28:
					reg_val &= (~ARM_GPIOB28_PU_MASK);
					reg_val |= ARM_GPIOB28_PU_VAL(od);
				break;	
			case PIN_29:
					reg_val &= (~ARM_GPIOB29_PU_MASK);
					reg_val |= ARM_GPIOB29_PU_VAL(od);
				break;			
			case PIN_30:
					reg_val &= (~ARM_GPIOB30_PU_MASK);
					reg_val |= ARM_GPIOB30_PU_VAL(od);
				break;
			case PIN_31:
			default :	
					reg_val &= (~ARM_GPIOB31_PU_MASK);
					reg_val |= ARM_GPIOB31_PU_VAL(od);
				break;
		}

		__raw_writel(reg_val, ARM_OD_REG);
	}
	else
	{}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpioc_od(E_PIN_PORT port, E_IO_OD od)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		od	: normal or open drain 
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal or open drain function.
//
/*************************************************************************************************************/
static bool misc_gpioc_od(E_PIN_PORT port, E_IO_OD od)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0) && (port <= PIN_9)) || 
		((port >= PIN_10) && (port <= PIN_19)) || 
		((port >= PIN_25) && (port <= PIN_26)) ||
		((port >= PIN_27) && (port <= PIN_30)))
	{
		reg_val = __raw_readl(ARM_SD_OD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOC0_PU_MASK);
					reg_val |= ARM_GPIOC0_PU_VAL(od);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOC1_PU_MASK);
					reg_val |= ARM_GPIOC1_PU_VAL(od);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOC2_PU_MASK);
					reg_val |= ARM_GPIOC2_PU_VAL(od);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOC3_PU_MASK);
					reg_val |= ARM_GPIOC3_PU_VAL(od);
				break;
			case PIN_4:
					reg_val &= (~ARM_GPIOC4_PU_MASK);
					reg_val |= ARM_GPIOC4_PU_VAL(od);
				break;
			case PIN_5:
					reg_val &= (~ARM_GPIOC5_PU_MASK);
					reg_val |= ARM_GPIOC5_PU_VAL(od);
				break;
			case PIN_6:
					reg_val &= (~ARM_GPIOC6_PU_MASK);
					reg_val |= ARM_GPIOC6_PU_VAL(od);
				break;
			case PIN_7:
					reg_val &= (~ARM_GPIOC7_PU_MASK);
					reg_val |= ARM_GPIOC7_PU_VAL(od);
				break;
			case PIN_8:
					reg_val &= (~ARM_GPIOC8_PU_MASK);
					reg_val |= ARM_GPIOC8_PU_VAL(od);
				break;
			case PIN_9:
					reg_val &= (~ARM_GPIOC9_PU_MASK);
					reg_val |= ARM_GPIOC9_PU_VAL(od);
				break;
			case PIN_10:
					reg_val &= (~ARM_GPIOC10_PU_MASK);
					reg_val |= ARM_GPIOC10_PU_VAL(od);
				break;
			case PIN_11:
					reg_val &= (~ARM_GPIOC11_PU_MASK);
					reg_val |= ARM_GPIOC11_PU_VAL(od);
				break;	
			case PIN_12:
					reg_val &= (~ARM_GPIOC12_PU_MASK);
					reg_val |= ARM_GPIOC12_PU_VAL(od);
				break;			
			case PIN_13:
					reg_val &= (~ARM_GPIOC13_PU_MASK);
					reg_val |= ARM_GPIOC13_PU_VAL(od);
				break;
			case PIN_14:
					reg_val &= (~ARM_GPIOC14_PU_MASK);
					reg_val |= ARM_GPIOC14_PU_VAL(od);
				break;
			case PIN_15:
					reg_val &= (~ARM_GPIOC15_PU_MASK);
					reg_val |= ARM_GPIOC15_PU_VAL(od);
				break;
			case PIN_16:
					reg_val &= (~ARM_GPIOC16_PU_MASK);
					reg_val |= ARM_GPIOC16_PU_VAL(od);
				break;
			case PIN_17:
					reg_val &= (~ARM_GPIOC17_PU_MASK);
					reg_val |= ARM_GPIOC17_PU_VAL(od);
				break;
			case PIN_18:
					reg_val &= (~ARM_GPIOC18_PU_MASK);
					reg_val |= ARM_GPIOC18_PU_VAL(od);
				break;
			case PIN_19:
					reg_val &= (~ARM_GPIOC19_PU_MASK);
					reg_val |= ARM_GPIOC19_PU_VAL(od);
				break;
			case PIN_25:
					reg_val &= (~ARM_GPIOC25_PU_MASK);
					reg_val |= ARM_GPIOC25_PU_VAL(od);
				break;
			case PIN_26:
					reg_val &= (~ARM_GPIOC26_PU_MASK);
					reg_val |= ARM_GPIOC26_PU_VAL(od);
				break;
			case PIN_27:
					reg_val &= (~ARM_GPIOC27_PU_MASK);
					reg_val |= ARM_GPIOC27_PU_VAL(od);
				break;
			case PIN_28:
					reg_val &= (~ARM_GPIOC28_PU_MASK);
					reg_val |= ARM_GPIOC28_PU_VAL(od);
				break;
			case PIN_29:
					reg_val &= (~ARM_GPIOC29_PU_MASK);
					reg_val |= ARM_GPIOC29_PU_VAL(od);
				break;	
			case PIN_30:
			default	:	
					reg_val &= (~ARM_GPIOC30_PU_MASK);
					reg_val |= ARM_GPIOC30_PU_VAL(od);
				break;
		}
	
		__raw_writel(reg_val, ARM_SD_OD_REG);
	}
	else 
	{}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpiod_od(E_PIN_PORT port, E_IO_OD od)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		od	: normal or open drain 
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal or open drain function.
//
/*************************************************************************************************************/
static bool misc_gpiod_od(E_PIN_PORT port, E_IO_OD od)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_PIN_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	if (((port >= PIN_0)  && (port <= PIN_3)) || 
		((port >= PIN_29) && (port <= PIN_31)))
	{
		reg_val = __raw_readl(ARM_OD_REG);
	
		switch (port)
		{
			case PIN_0:
					reg_val &= (~ARM_GPIOD0_PU_MASK);
					reg_val |= ARM_GPIOD0_PU_VAL(od);
				break;
			case PIN_1:
					reg_val &= (~ARM_GPIOD1_PU_MASK);
					reg_val |= ARM_GPIOD1_PU_VAL(od);
				break;
			case PIN_2:
					reg_val &= (~ARM_GPIOD2_PU_MASK);
					reg_val |= ARM_GPIOD2_PU_VAL(od);
				break;
			case PIN_3:
					reg_val &= (~ARM_GPIOD3_PU_MASK);
					reg_val |= ARM_GPIOD3_PU_VAL(od);
				break;							
			case PIN_29:
					reg_val &= (~ARM_GPIOD29_PU_MASK);
					reg_val |= ARM_GPIOD29_PU_VAL(od);
				break;
			case PIN_30:
					reg_val &= (~ARM_GPIOD30_PU_MASK);
					reg_val |= ARM_GPIOD30_PU_VAL(od);
				break;
			case PIN_31:
			default	:	
					reg_val &= (~ARM_GPIOD31_PU_MASK);
					reg_val |= ARM_GPIOD31_PU_VAL(od);
				break;
		}
	
		__raw_writel(reg_val, ARM_OD_REG);
	}
	else 
	{}
	
	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_gpio_od(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_OD od)
//
//  Parameters:
// 		port	: gpio port
//
// 		pin	: gpio pin
//
// 		od	: normal or open drain 
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal or open drain function.
//
/*************************************************************************************************************/
static bool misc_gpio_od(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_OD od)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_GPIO_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	switch (port)
	{
		case GPIOA:
				if (true != misc_gpioa_od(pin, od))
				{
					return false;
				}
			break;
		case GPIOB:
				if (true != misc_gpiob_od(pin, od))
				{
					return false;
				}
			break;
		case GPIOC:				
				if (true != misc_gpioc_od(pin, od))
				{
					return false;
				}
			break;		
		case GPIOD:
		default:
				if (true != misc_gpiod_od(pin, od))
				{
					return false;
				}
			break;			
	}

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_input_od(E_TIMER_PORT port)
//
//  Parameters:
// 		port	: timer port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_timer_input_od(E_TIMER_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= TIMER4)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_SD_OD_REG);
	
	switch (port)
	{
		case TIMER0:
				reg_val &= (~ARM_TIMER0_IN_PU_MASK);
			break;
		case TIMER1:
				reg_val &= (~ARM_TIMER1_IN_PU_MASK);
			break;	
		case TIMER2:
				reg_val &= (~ARM_TIMER2_IN_PU_MASK);
			break;			
		case TIMER3:
		default:
				reg_val &= (~ARM_TIMER3_IN_PU_MASK);
			break;
	}

	__raw_writel(reg_val, ARM_SD_OD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static bool misc_timer_output_od(E_TIMER_WXYZ_PORT port)
//
//  Parameters:
// 		port		: timer w/x/y/z port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_timer_output_od(E_TIMER_WXYZ_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the w/x/y/z port
	if (port >= NUM_TIMER_WXYZ_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_OD_REG);
	
	switch (port)
	{
		case TIMER_W:
				reg_val &= (~ARM_TIMERW_OUT_PU_MASK);
			break;
		case TIMER_X:
				reg_val &= (~ARM_TIMERX_OUT_PU_MASK);
			break;	
		case TIMER_Y:
				reg_val &= (~ARM_TIMERY_OUT_PU_MASK);
			break;			
		case TIMER_Z:
		default:
				reg_val &= (~ARM_TIMERZ_OUT_PU_MASK);
			break;
	}

	__raw_writel(reg_val, ARM_OD_REG);

	return true;
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_clkout_od(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static void misc_rtc_clkout_od(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_OD_REG);

	reg_val &= (~ARM_RTC_CLKOUT_PU_MASK);

	__raw_writel(reg_val, ARM_OD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_rtc_cal_clkout_od(void)
//
//  Parameters:
// 		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static void misc_rtc_cal_clkout_od(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_OD_REG);

	reg_val &= (~ARM_RTC_CAL_CLKOUT_PU_MASK);

	__raw_writel(reg_val, ARM_OD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_clkout_od(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static void misc_clkout_od(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_OD_REG);

	reg_val &= (~ARM_CLKOUT_PU_MASK);

	__raw_writel(reg_val, ARM_OD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_host_low_power_out_od(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static void misc_host_low_power_out_od(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_OD_REG);

	reg_val &= (~ARM_HOST_LOW_POWER_OUT_PU_MASK);

	__raw_writel(reg_val, ARM_OD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_failsafe_timer_out_od(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static void misc_epf_failsafe_timer_out_od(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_OD_REG);

	reg_val &= (~ARM_EPF_FAILSAFE_TIMER_OUT_PU_MASK);

	__raw_writel(reg_val, ARM_OD_REG);
}

/*************************************************************************************************************/
//  Function: static void misc_epf_sleep_mode_done_od(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static void misc_epf_sleep_mode_done_od(void)
{
	uint32_t reg_val;

	reg_val = __raw_readl(ARM_OD_REG);

	reg_val &= (~ARM_EPF_SLEEP_MODE_DONE_PU_MASK);

	__raw_writel(reg_val, ARM_OD_REG);
}

/*************************************************************************************************************/
//  Function: static bool misc_usb_oc_od(E_USB_PORT port)
//
//  Parameters:
// 		port		: usb port
//
//  Return:
//      	false	: configure fail
//
//		true	: configure ok
//
//  Description:
//		Set pin to normal function.
//
/*************************************************************************************************************/
static bool misc_usb_oc_od(E_USB_PORT port)
{
	uint32_t reg_val;
	
	// Check the validity of the usb port
	if (port >= NUM_USB_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_OD_REG);
	
	switch (port)
	{
		case USB0:
				reg_val &= (~ARM_USB0_OC_PU_MASK);
			break;		
		case USB1:
		default:
				reg_val &= (~ARM_USB1_OC_PU_MASK);
			break;
	}

	__raw_writel(reg_val, ARM_OD_REG);

	return true;
}

#ifdef TAPEOUT_V2
/*************************************************************************************************************/
//  Function: bool misc_enable_dma_err_resp(int dmac_index, int timeout_val)
//
//  Parameters:
// 		dmac_index		: dma controller index (0~2)
//		timeout_val     : timeout value (1~0xffffffff, ahb clk unit)
//
//  Return:
//      bool			: true, succese; false, invalid damc_index parameter	 
//
//  Description:
//		enable DMA error response funciton.
//
/*************************************************************************************************************/
bool misc_enable_dma_err_resp(int dmac_index, int timeout_val)
{
	uint32_t reg_val = __raw_readl(ARM_DMA_ERR_RESP_EN);	
	if(dmac_index < 0 || dmac_index > 2)
		return false;
	
	__raw_writel(timeout_val, ARM_DMA0_TIMEOUT_CNT+(dmac_index<<2));
	__raw_writel(reg_val | (1<<dmac_index), ARM_DMA_ERR_RESP_EN);
}

/*************************************************************************************************************/
//  Function: bool misc_disable_dma_err_resp(int dmac_index)
//
//  Parameters:
// 		dmac_index		: dma controller index (0~2)
//
//  Return:
//      bool			: true, succese; false, invalid damc_index parameter
//
//  Description:
//		disable DMA error response funciton.
//
/*************************************************************************************************************/

bool misc_disable_dma_err_resp(int dmac_index)
{
	uint32_t reg_val = __raw_readl(ARM_DMA_ERR_RESP_EN);

	if(dmac_index < 0 || dmac_index > 2)
		return false;

	reg_val &= ~(1<<dmac_index);
	__raw_writel(reg_val, ARM_DMA_ERR_RESP_EN);

}

/*************************************************************************************************************/
//  Function: bool misc_trng_sel(int trng_index)
//
//  Parameters:
// 		trng_index		: trng controller index (0: Brite TRNG, 1: DWC TRNG)
//
//  Return:
//      none			: 
//
//  Description:
//		select which TRNG is enabled.
//
/*************************************************************************************************************/
void misc_trng_sel(int trng_index)
{
	__raw_writel(trng_index ? 1 : 0, ARM_RNG_SEL);
}
#endif

#if defined(EVB_BOARD)

/*************************************************************************************************************/
//  Function: bool misc_uart_pin_init(E_UART_PORT port)
//
//  Parameters:
// 		port	: uart port
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize uart pin.
//
/*************************************************************************************************************/
bool misc_uart_pin_init(E_UART_PORT port)
{
	if (!misc_uart_pinmux(port))
	{
		return false;
	}

	if (!misc_uart_pu(port))
	{
		return false;
	}

	if (!misc_uart_pd(port, IO_PD_EN))
	{
		return false;
	}

	if (!misc_uart_od(port))
	{
		return false;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_sdio_pin_init(E_SDIO_PORT port, E_SDIO_BW bw)
//
//  Parameters:
// 		port	: sdio port
//
//		
//		bw	: sdio bus width
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize sdio pin.
//
/*************************************************************************************************************/
bool misc_sdio_pin_init(E_SDIO_PORT port, E_SDIO_BW bw)
{
	if (!misc_sdio_pinmux(port, bw))
	{
		return false;
	}

	if (!misc_sdio_pu(port, bw))
	{
		return false;
	}

	if (!misc_sdio_pd(port, IO_PD_EN, bw))
	{
		return false;
	}

	if (!misc_sdio_od(port, bw))
	{
		return false;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_spi_pin_init(E_SPI_PORT port)
//
//  Parameters:
// 		port	: spi port
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize spi pin.
//
/*************************************************************************************************************/
bool misc_spi_pin_init(E_SPI_PORT port)
{
	if (!misc_spi_pinmux(port))
	{
		return false;
	}

	if (!misc_spi_pu(port))
	{
		return false;
	}

	if (!misc_spi_pd(port, IO_PD_EN))
	{
		return false;
	}

	if (!misc_spi_od(port))
	{
		return false;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_i2c_pin_init(E_I2C_PORT port)
//
//  Parameters:
// 		port	: i2c port
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize i2c pin.
//
/*************************************************************************************************************/
bool misc_i2c_pin_init(E_I2C_PORT port)
{
	if (!misc_i2c_pinmux(port))
	{
		return false;
	}

	if (!misc_i2c_pu(port))
	{
		return false;
	}

	if (!misc_i2c_pd(port, IO_PD_DIS))
	{
		return false;
	}

	if (!misc_i2c_od(port))
	{
		return false;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_gpio_pin_init(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PU pu, E_IO_PD pd, E_IO_OD od)
//
//  Parameters:
// 		port	: gpio pin port
//
// 		pin	: gpio pin
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
// 		od	: normal or open drain 
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize i2c pin.
//
/*************************************************************************************************************/
bool misc_gpio_pin_init(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PU pu, E_IO_PD pd, E_IO_OD od)
{
	if ((port == GPIOB) && (pin == PIN_17))
	{
		misc_gpiob_17_pinmux();
	}
	else
	{
		if (!misc_gpio_pinmux(port, pin))
		{
			return false;
		}
		
		if (!misc_gpio_pu(port, pin, pu))
		{
			return false;
		}
		
		if (!misc_gpio_pd(port, pin, pd))
		{
			return false;
		}

		if (!misc_gpio_od(port, pin, od))
		{
			return false;
		}	
	}

	return true;
}
/*************************************************************************************************************/
//  Function: bool misc_timer_input_pin_init(E_TIMER_PORT timer, E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//		port :  timer port 
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize timer0/1/2/3 input pin.
//
/*************************************************************************************************************/
bool misc_timer_input_pin_init(E_TIMER_PORT port, E_IO_PU pu, E_IO_PD pd)
{
	if (!misc_timer_input_pinmux(port))
	{
		return false;
	}
	
	if (!misc_timer_input_pu(port, pu))
	{
		return false;
	}
	
	if (!misc_timer_input_pd(port, pd))
	{
		return false;
	}

	if (!misc_timer_input_od(port))
	{
		return false;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_timer_output_pin_init(E_TIMER_WXYZ_PORT port, E_TIMER_PORT timer, E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
// 		port	: timer w/x/y/z port
//
//		timer : timer [0, 7]port 
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	false	: initialize fail
//
//		true	: initialize ok
//
//  Description:
//		Initialize timer(w/x/y/z) output pin.
//
/*************************************************************************************************************/
bool misc_timer_output_pin_init(E_TIMER_WXYZ_PORT port, E_TIMER_PORT timer, E_IO_PU pu, E_IO_PD pd)
{
	if (!misc_timer_output_pinmux(port, timer))
	{
		return false;
	}
	
	if (!misc_timer_output_pu(port, pu))
	{
		return false;
	}
	
	if (!misc_timer_output_pd(port, pd))
	{
		return false;
	}

	if (!misc_timer_output_od(port))
	{
		return false;
	}	

	return true;
}

/*************************************************************************************************************/
//  Function: void misc_rtc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize rtc clkout output pin.
//
/*************************************************************************************************************/
void misc_rtc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
{
	misc_rtc_clkout_pinmux();
	
	misc_rtc_clkout_pu(pu);
	
	misc_rtc_clkout_pd(pd);

	misc_rtc_clkout_od();
}

/*************************************************************************************************************/
//  Function: void misc_rtc_cal_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize rtc cal clkout output pin.
//
/*************************************************************************************************************/
void misc_rtc_cal_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
{
	misc_rtc_cal_clkout_pinmux();
	
	misc_rtc_cal_clkout_pu(pu);
	
	misc_rtc_cal_clkout_pd(pd);

	misc_rtc_cal_clkout_od();
}

/*************************************************************************************************************/
//  Function: void misc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize arm clkout output pin.
//
/*************************************************************************************************************/
void misc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
{
	misc_clkout_pinmux();
	
	misc_clkout_pu(pu);
	
	misc_clkout_pd(pd);

	misc_clkout_od();
}

/*************************************************************************************************************/
//  Function: void misc_host_low_power_out_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize host low power output pin.
//
/*************************************************************************************************************/
void misc_host_low_power_out_pin_init(E_IO_PU pu, E_IO_PD pd)
{
	misc_host_low_power_out_pinmux();
	
	misc_host_low_power_out_pu(pu);
	
	misc_host_low_power_out_pd(pd);

	misc_host_low_power_out_od();
}

/*************************************************************************************************************/
//  Function: void misc_epf_failsafe_timer_out_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize epf failsafe time out output pin.
//
/*************************************************************************************************************/
void misc_epf_failsafe_timer_out_pin_init(E_IO_PU pu, E_IO_PD pd)
{
	misc_epf_failsafe_time_out_pinmux();
	
	misc_epf_failsafe_timer_out_pu(pu);
	
	misc_epf_failsafe_timer_out_pd(pd);

	misc_epf_failsafe_timer_out_od();
}

/*************************************************************************************************************/
//  Function: void misc_sleep_mode_done_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize epf failsafe time out output pin.
//
/*************************************************************************************************************/
void misc_sleep_mode_done_pin_init(E_IO_PU pu, E_IO_PD pd)
{
	misc_epf_sleep_mode_done_pinmux();
	
	misc_epf_sleep_mode_done_pu(pu);
	
	misc_epf_sleep_mode_done_pd(pd);

	misc_epf_sleep_mode_done_od();
}

/*************************************************************************************************************/
//  Function: void misc_usb_oc_pin_init(E_IO_PU pu, E_IO_PD pd)
//
//  Parameters:
// 		port	: usb port
//
//		pu	: pull up or down
//
//		pd	: pull up/down is active/disabled
//
//  Return:
//      	none
//
//  Description:
//		Initialize epf failsafe time out output pin.
//
/*************************************************************************************************************/
void misc_usb_oc_pin_init(E_USB_PORT port, E_IO_PU pu, E_IO_PD pd)
{	
	misc_usb_oc_pu(port, pu);
	
	misc_usb_oc_pd(port, pd);

	misc_usb_oc_od(port);
}

/*************************************************************************************************************/
//  Function: void misc_wdt_rstn_pin_init(void)
//
//  Parameters:
// 		none
//
//  Return:
//      	none
//
//  Description:
//		Initialize wdt rstn output pin.
//
/*************************************************************************************************************/
void misc_wdt_rstn_pin_init(void)
{	
	misc_wdt_rstn_pinmux();
}
#else
bool misc_uart_pin_init(E_UART_PORT port)
{
	return true;
}

bool misc_sdio_pin_init(E_SDIO_PORT port, E_SDIO_BW bw)
{
	return true;
}

bool misc_spi_pin_init(E_SPI_PORT port)
{
	return true;
}

bool misc_i2c_pin_init(E_I2C_PORT port)
{
	return true;
}

bool misc_gpio_pin_init(E_GPIO_PORT port, E_PIN_PORT pin, E_IO_PU pu, E_IO_PD pd, E_IO_OD od)
{
	return true;
}

bool misc_timer_input_pin_init(E_TIMER_PORT port, E_IO_PU pu, E_IO_PD pd)
{
	return true;
}

bool misc_timer_output_pin_init(E_TIMER_WXYZ_PORT port, E_TIMER_PORT timer, E_IO_PU pu, E_IO_PD pd)
{
	return true;
}

void misc_rtc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
{
}

void misc_rtc_cal_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
{
}

void misc_clkout_pin_init(E_IO_PU pu, E_IO_PD pd)
{
}

void misc_host_low_power_out_pin_init(E_IO_PU pu, E_IO_PD pd)
{
}

void misc_epf_failsafe_timer_out_pin_init(E_IO_PU pu, E_IO_PD pd)
{
}

void misc_sleep_mode_done_pin_init(E_IO_PU pu, E_IO_PD pd)
{
}

void misc_usb_oc_pin_init(E_USB_PORT port, E_IO_PU pu, E_IO_PD pd)
{
}

void misc_wdt_rstn_pin_init(void)
{
}

#endif /*EVB_BOARD*/

/*************************************************************************************************************/
//  Function: bool misc_uart_ds(E_UART_PORT port, E_IO_DS_MA ds)
//
//  Parameters:
// 		port	: uart port
//
//		ds	: driving strength
//
//  Return:
//      	false	: set driving strength fail
//
//		true	: set driving strength ok
//
//  Description:
//		Initialize uart io driving strength.
//
/*************************************************************************************************************/
bool misc_uart_ds(E_UART_PORT port, E_IO_DS_MA ds)
{
	uint32_t reg_val;

	// Check the validity of the port
	if (port >= NUM_UART_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_UART_I2C_DS_REG);

	switch (port)
	{
		case UART0:
				reg_val &= (~ARM_UART0_DS_MASK);
				
				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_UART0_DS_VAL(ARM_UART0_DS_MASK>>ARM_UART0_TXD_DS_SHIFT);
				}
			break;
		case UART1:
				reg_val &= (~ARM_UART1_DS_MASK);

				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_UART1_DS_VAL(ARM_UART1_DS_MASK>>ARM_UART1_TXD_DS_SHIFT);
				}		
			break;
		case UART2:
				reg_val &= (~ARM_UART2_DS_MASK);
				
				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_UART2_DS_VAL(ARM_UART2_DS_MASK>>ARM_UART2_TXD_DS_SHIFT);
				}			
			break;
		case UART3:
		default:	
				reg_val &= (~ARM_UART3_DS_MASK);
				
				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_UART3_DS_VAL(ARM_UART3_DS_MASK>>ARM_UART3_TXD_DS_SHIFT);
				}			
			break;
	}

	__raw_writel(reg_val, ARM_UART_I2C_DS_REG);

	reg_val = __raw_readl(ARM_UART_I2C_DS_REG);
	info("ARM_UART_I2C_DS_REG:0x%x\n", reg_val);

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_i2c_ds(E_I2C_PORT port, E_IO_DS_MA ds)
//
//  Parameters:
// 		port	: i2c port
//
//		ds	: driving strength
//
//  Return:
//      	false	: set driving strength fail
//
//		true	: set driving strength ok
//
//  Description:
//		Initialize i2c io driving strength.
//
/*************************************************************************************************************/
bool misc_i2c_ds(E_I2C_PORT port, E_IO_DS_MA ds)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_I2C_PORTS)
	{
		info("port(%x) err!n", port);
		return false;
	}

	reg_val = __raw_readl(ARM_UART_I2C_DS_REG);
	switch (port)
	{
		case I2C0:
				reg_val &= (~ARM_I2C0_DS_MASK);
				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_I2C0_DS_VAL(ARM_I2C0_DS_MASK>>ARM_I2C0_SDA_DS_SHIFT);
				}
			break;
		case I2C1:
				reg_val &= (~ARM_I2C1_DS_MASK);
				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_I2C1_DS_VAL(ARM_I2C1_DS_MASK>>ARM_I2C1_SDA_DS_SHIFT);
				}
			break;
		case I2C2:
		default:	
				reg_val &= (~ARM_I2C2_DS_MASK);
				if (ds == IO_DS_8MA)
				{
					reg_val |= ARM_I2C2_DS_VAL(ARM_I2C2_DS_MASK>>ARM_I2C2_SDA_DS_SHIFT);
				}
			break;
	}	

	__raw_writel(reg_val, ARM_UART_I2C_DS_REG);

	reg_val = __raw_readl(ARM_UART_I2C_DS_REG);
	info("ARM_UART_I2C_DS_REG:0x%x\n", reg_val);

	return true;
}

/*************************************************************************************************************/
//  Function: bool misc_dma_ext_int_src_sel(E_ARM_DMA_HS_EXT_INT_SRC_ID int_src_id)
//
//  Parameters:
// 		int_src	: dma external interrupt source
//
//  Return:
//      	false	: set external interrupt source fail
//
//		true	: set external interrupt source ok
//
//  Description:
//		Set the dma external interrupt source id.
//
/*************************************************************************************************************/
bool misc_dma_ext_int_src_sel(E_DMA_PORT port, E_ARM_DMA_HS_EXT_INT_SRC ext_src)
{
	uint32_t reg_val;
	
	// Check the validity of the port
	if (port >= NUM_DMA_PORTS)
	{
		info("dma port(%x) err!n", port);
		return false;
	}

	if (ext_src >= ARM_DMA_HS_EXT_SRC_NUM)
	{
		info("hs ext source(%x) err!n", port);
		return false;
	}	

	reg_val = __raw_readl(ARM_DMA_HS_EXT_INT_SRC_SEL_REG);
	
	switch (port)
	{
		case DMA0:
				reg_val &= (~ARM_DMA_HS_EXT_INT0_MASK);
				reg_val |= ARM_DMA_HS_EXT_INT0_VAL(ext_src);			
			break;
		case DMA1:
				reg_val &= (~ARM_DMA_HS_EXT_INT1_MASK);
				reg_val |= ARM_DMA_HS_EXT_INT1_VAL(ext_src);			
			break;
		default:
				reg_val &= (~ARM_DMA_HS_EXT_INT2_MASK);
				reg_val |= ARM_DMA_HS_EXT_INT2_VAL(ext_src);			
			break;			
	}

	__raw_writel(reg_val, ARM_DMA_HS_EXT_INT_SRC_SEL_REG);

	reg_val = __raw_readl(ARM_DMA_HS_EXT_INT_SRC_SEL_REG);
	info("misc_dma_ext_int_src_sel:0x%x\n", reg_val);

	return true;
}

