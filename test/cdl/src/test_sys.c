/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_ddr.h"
#include "test_gpio.h"
#include "test_i2c.h"
#include "test_timer.h"
#include "test_wdt.h"
#include "test_uart.h"
#include "test_spi.h"
#include "test_qspi.h"
#include "test_crc.h"
#include "test_nand.h"
#include "test_mmc.h"
#include "test_ecc.h"
#include "test_dma.h"
#include "test_sha.h"
#include "test_aes.h"
#include "test_otp.h"
#include "test_ipc.h"
#include "test_rng.h"
#include "test_usb.h"
#include "test_adc.h"
#include "test_utils.h"
#include "test_dwc_trng.h"

#if (CONFIG_SYS_TEST == 1)

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define SYS_TIMEOUT (1000) //ms

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
cmdline_entry sys_test_menu[];

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/
#if (DHRYSTONE_TEST == 1)
extern int startDhry(void);
#endif

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
// command process functions
static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&sys_test_menu[0]);
}

#define SYS_ETB_LOCK_DEF_VAL	0xC5ACCE55
#define SYS_ETB_LOCK_ADDR		0x20140FB0
#define SYS_ETB_LOCK_ST_ADDR	0x20140FB4

#define SYS_ETB_RWP_DEF_VAL	0x0
#define SYS_ETB_RWP_ADDR	0x20140018
static bool debug0_etb_atb_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	unsigned long long start = 0;

	info("%s\n", __func__);

	// unlock the ETB component
	info("unlock the etb component\n");
	__raw_writel(SYS_ETB_LOCK_DEF_VAL, SYS_ETB_LOCK_ADDR);
	start = get_timer(0);
	while (1)
	{
		if (0 != __raw_readl(SYS_ETB_LOCK_ST_ADDR))
		{
			break;
		}

		if(get_timer(start) > SYS_TIMEOUT) {
			info("debug0_etb_atb_reset_test unlock the etb component timeout\n");
			return false;
		}
	}

	// write register
	info("write reg\n");
	__raw_writel(*p_wr_val, SYS_ETB_RWP_ADDR);

	// read&compare
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_RWP_ADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("debug0 reset\n");
	prcm_reset(ARM_DEBUG0_RST);
	
	// read register
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_RWP_ADDR);
	if (*p_rd_val != SYS_ETB_RWP_DEF_VAL)
	{
		return false;
	}

	return true;
}

static bool debug0_etb_atb_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
#if 0
	E_ARM_BLK_CG blk = ARM_BLK_CG_DBG_TCLK;
	unsigned long long start = 0;

	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// unlock the ETB component
	info("unlock the etb component\n");
	__raw_writel(SYS_ETB_LOCK_DEF_VAL, SYS_ETB_LOCK_ADDR);
	start = get_timer(0);
	while (1)
	{
		if (0 != __raw_readl(SYS_ETB_LOCK_ST_ADDR))
		{
			break;
		}
		if(get_timer(start) > SYS_TIMEOUT) {
			info("debug0_etb_atb_cg_test unlock the etb component timeout\n");
			return false;
		}
	}

	// write register
	info("write reg\n");
	__raw_writel(*p_wr_val, SYS_ETB_RWP_ADDR);

	// read&compare
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_RWP_ADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write reg\n");
	__raw_writel((*p_wr_val+1), SYS_ETB_RWP_ADDR);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_RWP_ADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}

	return true;
#else
	return false;
#endif
}


#define SYS_ETB_CTRL_DEF_VAL	0x0
#define SYS_ETB_CTRL_ADDR		0x20140020
static bool debug1_etb_apb_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	unsigned long long start = 0;

	info("%s\n", __func__);

	// unlock the ETB component
	info("unlock the etb component\n");
	__raw_writel(SYS_ETB_LOCK_DEF_VAL, SYS_ETB_LOCK_ADDR);

	start = get_timer(0);
	while (1)
	{
		if (0 != __raw_readl(SYS_ETB_LOCK_ST_ADDR))
		{
			break;
		}
		if(get_timer(start) > SYS_TIMEOUT) {
			info("debug1_etb_apb_reset_test unlock the etb component timeout\n");
			return false;
		}
	}	

	// write register
	info("write reg\n");
	__raw_writel(*p_wr_val, SYS_ETB_CTRL_ADDR);

	// read&compare
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_CTRL_ADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("debug1 reset\n");
	prcm_reset(ARM_DEBUG1_RST);
	
	// read register
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_CTRL_ADDR);
	if (*p_rd_val != SYS_ETB_CTRL_DEF_VAL)
	{
		return false;
	}

	return true;
}

static bool debug1_etb_apb_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
#if 0
	E_ARM_BLK_CG blk = ARM_BLK_CG_DBG_PCLK;
	unsigned long long start = 0;

	info("%s\n", __func__);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// unlock the ETB component
	info("unlock the etb component\n");
	__raw_writel(SYS_ETB_LOCK_DEF_VAL, SYS_ETB_LOCK_ADDR);

	start = get_timer(0);
	while (1)
	{
		if (0 != __raw_readl(SYS_ETB_LOCK_ST_ADDR))
		{
			break;
		}
		if(get_timer(start) > SYS_TIMEOUT) {
			info("debug1_etb_apb_cg_test unlock the etb component timeout\n");
			return false;
		}
	}

	// write register
	info("write reg\n");
	__raw_writel(*p_wr_val, SYS_ETB_CTRL_ADDR);

	// read&compare
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_CTRL_ADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}

	
#if 0	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write reg\n");
	__raw_writel((*p_wr_val+1), SYS_ETB_CTRL_ADDR);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare reg\n");
	*p_rd_val = __raw_readl(SYS_ETB_CTRL_ADDR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}

	return true;
#else
	info("arm internal debug ip, shouldn't be gated, otherwise the cpu will be pending!\n");
	return false;
#endif
#else
	return false;
#endif
}

static bool ip_reset(E_ARM_IP_RST ip, uint32_t *p_wr_val, uint32_t *p_rd_val)
{	
	bool ret_val;

	prcm_reset(ip);
	switch (ip)
	{
		case ARM_NFC_RST:
				*p_wr_val = 0xFFFF;
				ret_val = nfc_reset_test(p_wr_val, p_rd_val);
			break;
		case ARM_SDMMC0_RST:
				*p_wr_val = 0x12345678;
				ret_val = mmc_reset_test(SDIO0, p_wr_val, p_rd_val);
			break;	
		case ARM_SDMMC1_RST:
			#if defined(EVB_BOARD)
				*p_wr_val = 0x12345678;
				ret_val = mmc_reset_test(SDIO1, p_wr_val, p_rd_val);
			#else
				info("Not supported on FPGA!\n");
				return 0;
			#endif
			break;	
		case ARM_DMA0_RST:
				*p_wr_val = 0x12345678;
				ret_val = dma_reset_test(DMA0, p_wr_val, p_rd_val);							
			break;	
		case ARM_DMA1_RST:
				*p_wr_val = 0x12345678;
				ret_val = dma_reset_test(DMA1, p_wr_val, p_rd_val);
			break;	
		case ARM_DMA2_RST:
				*p_wr_val = 0x12345678;
				ret_val = dma_reset_test(DMA2, p_wr_val, p_rd_val);
			break;	
		case ARM_QSPI_RST:
				*p_wr_val = 0x0;
				ret_val = qspi_reset_test(p_wr_val, p_rd_val);
			break;		
		case ARM_CRC_RST:
				*p_wr_val = 0x0;
				ret_val = crc_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_ECC_RST:
				*p_wr_val = 0xFF;
				ret_val = ecc_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_SHA_RST:
				*p_wr_val = 0x0;
				ret_val = sha_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_AES_RST:
				*p_wr_val = 0xFF;
				ret_val = aes_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_OTP_CTRL_RST:
				*p_wr_val = 0xFFFF;
				ret_val = otp_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_IPC_RST:
				*p_wr_val = 0xFFFF;
				ret_val = ipc_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_DEBUG0_RST:
				*p_wr_val = 0x1;
				ret_val = debug0_etb_atb_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_DEBUG1_RST:
				*p_wr_val = 0x1;
				ret_val = debug1_etb_apb_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_I2C0_RST:	
				*p_wr_val = 0xAA;
				ret_val = i2c_reset_test(I2C0, p_wr_val, p_rd_val);
			break;
		case ARM_I2C1_RST:
				*p_wr_val = 0xAA;
				ret_val = i2c_reset_test(I2C1, p_wr_val, p_rd_val);
			break;
		case ARM_I2C2_RST:
				*p_wr_val = 0xAA;
				ret_val = i2c_reset_test(I2C2, p_wr_val, p_rd_val);
			break;	
		case ARM_SPI0_RST:
				*p_wr_val = 0x3;
				ret_val = spi_reset_test(SPI1, p_wr_val, p_rd_val);
			break;
		case ARM_SPI1_RST:
				*p_wr_val = 0x3;
				ret_val = spi_reset_test(SPI2, p_wr_val, p_rd_val);
			break;	
		case ARM_SARADC_CTRL_RST:
				*p_wr_val = 0xAA;
				ret_val = adc_reset_test(p_wr_val, p_rd_val);
			break;	
		case ARM_UART0_RST:
				*p_wr_val = 0x3;
				ret_val = uart_reset_test(UART0, p_wr_val, p_rd_val);
			break;	
		case ARM_UART1_RST:
				*p_wr_val = 0x3;
				ret_val = uart_reset_test(UART1, p_wr_val, p_rd_val);
			break;
		case ARM_UART2_RST:
				*p_wr_val = 0x3;
				ret_val = uart_reset_test(UART2, p_wr_val, p_rd_val);
			break;			
		case ARM_UART3_RST:
				*p_wr_val = 0x3;
				ret_val = uart_reset_test(UART3, p_wr_val, p_rd_val);
			break;	
		case ARM_WDT_RST:
				*p_wr_val = 0x1234;
				ret_val = wdt_reset_test(p_wr_val, p_rd_val);
			break;
		case ARM_TIMER_RST:
				*p_wr_val = 0x12345678;
				ret_val = timer_reset_test(p_wr_val, p_rd_val);
			break;
		case ARM_GPIO_RST:
				*p_wr_val = 0x12345678;
				ret_val = gpio_reset_test(p_wr_val, p_rd_val);
			break;
		case ARM_RNG_RST:
				
#ifdef TAPEOUT_V2	
				*p_wr_val = 0x8;
				ret_val = dwc_trng_reset_test(p_wr_val, p_rd_val);
#else
				*p_wr_val = 0x1;
				ret_val = rng_reset_test(p_wr_val, p_rd_val);
#endif											
			break;
		case ARM_USBOTG0_RST:
				*p_wr_val = 0x7F;
				ret_val = usb_reset_test(USB0, p_wr_val, p_rd_val);
			break;
		case ARM_USBOTG1_RST:
#if defined(EVB_BOARD)
				*p_wr_val = 0x7F;
				ret_val = usb_reset_test(USB1, p_wr_val, p_rd_val);
#else
				info("Not supported on FPGA!\n");
				return 0;
#endif
			break;
		case ARM_DDR_CTRL_RST:
			break;
		case ARM_PLC_SUBSYS_SOFT_RST:
			break;
		case ARM_RF_SUBSYS_SOFT_RST:
			break;	
		case ARM_PLC_DSP_CORE_SOFT_RST:
			break;	
		case ARM_RF_DSP_CORE_SOFT_RST:
			break;	
		case GLOBAL_SOFT_RST:
			break;	
		case ARM_SOFT_RST:
			break;				
		default:
			break;
	}

	if (ip != ARM_TIMER_RST) {
		// reset
		prcm_reset(ip);
	}
	/* restore the DMA controller enable status */
	dw_dmac_init(&dma_dev_list[0], DMAC0_BASE,INT_DMA0);
	dw_dmac_init(&dma_dev_list[1], DMAC1_BASE,INT_DMA1);
	dw_dmac_init(&dma_dev_list[2], DMAC2_BASE,INT_DMA2);

	return ret_val;
}

static int prcm_reset_test(int argc, char *argv[])
{
	E_ARM_IP_RST ip = 0;
	uint32_t reg_val;
	uint32_t wr_val, rd_val;

	info("%s: prcm reset test\n", argv[0]);

	// Get the ip index
	if (argc >= 2)
	{
		ip = get_arg_ulong(argv[1]);
	}	
	
	// Check the validity of parameter count
	if ((argc < 2) || (ip > ARM_SOFT_RST))
	{
		info("wrong cmd_format: %s ip\n", argv[0]);

		info("Please input valid ip\n");
		info("0:  ARM_NFC_RSTn\n");
		info("1:  ARM_SDMMC0_RST\n");
		info("2:  ARM_SDMMC1_RST\n");
		info("3:  ARM_DMA0_RST\n");
		info("4:  ARM_DMA1_RST\n");
		info("5:  ARM_DMA2_RST\n");
		info("6:  ARM_QSPI_RST\n");
		info("7:  ARM_CRC_RST\n");
		info("8:  ARM_ECC_RST\n");
		info("9:  ARM_SHA_RST\n");
		info("10: ARM_AES_RST\n");
		info("11: ARM_OTP_CTRL_RST\n");
		info("12: ARM_IPC_RST\n");
		info("13: ARM_DEBUG0_RST\n");
		info("14: ARM_DEBUG1_RST\n");
		info("15: ARM_I2C0_RST\n");
		info("16: ARM_I2C1_RST\n");
		info("17: ARM_I2C2_RST\n");
		info("18: ARM_SPI0_RST\n");
		info("19: ARM_SPI1_RST\n");
		info("20: ARM_SARADC_CTRL_RST\n");
		info("21: ARM_UART0_RST\n");
		info("22: ARM_UART1_RST\n");
		info("23: ARM_UART2_RST\n");
		info("24: ARM_UART3_RST\n");
		info("25: ARM_WDT_RST\n");
		info("26: ARM_TIMER_RST\n");
		info("27: ARM_GPIO_RST\n");
		info("28: ARM_RNG_RST\n");
		info("29: ARM_USBOTG0_RST\n");
		info("30: ARM_USBOTG1_RST\n");
		info("31: ARM_DDR_CTRL_RST(don't care)\n");
		info("32: ARM_PLC_SUBSYS_SOFT_RST(plc re-boot)\n");
		info("33: ARM_RF_SUBSYS_SOFT_RST(rf re-boot)\n");
		info("34: ARM_PLC_DSP_CORE_SOFT_RST(plc re-boot)\n");
		info("35: ARM_RF_DSP_CORE_SOFT_RST(rf re-boot)\n");
		info("36: GLOBAL_SOFT_RST(arm&plc&rf re-boot)\n");
		info("37: ARM_SOFT_RST(arm re-boot)\n");
		return 1;
	}

	// Reset the IP
	if (!ip_reset(ip, &wr_val, &rd_val))
	{
		info("ip(%d) wr(0x%x) rd(0x%x) reset fail\n", ip, wr_val, rd_val);
	}
	else
	{
		info("ip(%d) wr(0x%x) rd(0x%x) reset pass\n", ip, wr_val, rd_val);
	}

	return 0;
}

static int misc_chip_id_test(int argc, char *argv[])
{
	uint32_t reg_val;
	
	info("%s: misc test\n", argv[0]);

	reg_val = __raw_readl(ARM_CHIP_ID_REG);

	info("Chip ID: %x\n", reg_val);
	
	return 0;
}

static bool blk_cg(E_ARM_BLK_CG blk, uint32_t *p_wr_val, uint32_t *p_rd_val)
{	
	bool ret_val;
	
	switch (blk)
	{
		case ARM_BLK_CG_NFC:
				*p_wr_val = 0xFFFF;
				ret_val = nfc_cg_test(p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_SDMMC0:
				*p_wr_val = 0x12345678;
				ret_val = mmc_cg_test(SDIO0, p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_SDMMC1:
				*p_wr_val = 0x12345678;
				ret_val = mmc_cg_test(SDIO1, p_wr_val, p_rd_val);			
			break;	
		case ARM_BLK_CG_DAM0:
				*p_wr_val = 0x12345678;
				ret_val = dma_cg_test(DMA0, p_wr_val, p_rd_val);			
			break;	
		case ARM_BLK_CG_DAM1:
				*p_wr_val = 0x12345678;
				ret_val = dma_cg_test(DMA1, p_wr_val, p_rd_val);				
			break;	
		case ARM_BLK_CG_DAM2:
				*p_wr_val = 0x12345678;
				ret_val = dma_cg_test(DMA2, p_wr_val, p_rd_val);				
			break;	
		case ARM_BLK_CG_QSPI:
				*p_wr_val = 0x0;
				ret_val = qspi_cg_test(p_wr_val, p_rd_val);
			break;		
		case ARM_BLK_CG_CRC:
				*p_wr_val = 0x0;
				ret_val = crc_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_ECC384:
				*p_wr_val = 0xFF;
				ret_val = ecc_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_SHA384:
				*p_wr_val = 0x0;
				ret_val = sha_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_AES:
				*p_wr_val = 0xFF;
				ret_val = aes_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_OTP:
				*p_wr_val = 0xFFFF;
				ret_val = otp_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_IPC:
				*p_wr_val = 0xFFFF;
				ret_val = ipc_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_DBG_TCLK:
				*p_wr_val = 0x1;
				ret_val = debug0_etb_atb_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_DBG_PCLK:
				*p_wr_val = 0x1;
				ret_val = debug1_etb_apb_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_I2C0:	
				*p_wr_val = 0xAA;
				ret_val = i2c_cg_test(I2C0, p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_I2C1:
				*p_wr_val = 0xAA;
				ret_val = i2c_cg_test(I2C1, p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_I2C2:
				*p_wr_val = 0xAA;
				ret_val = i2c_cg_test(I2C2, p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_SPI0:
				*p_wr_val = 0x3;
				ret_val = spi_cg_test(SPI1, p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_SPI1:
				*p_wr_val = 0x3;
				ret_val = spi_cg_test(SPI2, p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_SARADC:
				*p_wr_val = 0xAA;
				ret_val = adc_cg_test(p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_UART0:
				*p_wr_val = 0x3;
				ret_val = uart_cg_test(UART0, p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_UART1:
				*p_wr_val = 0x3;
				ret_val = uart_cg_test(UART1, p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_UART2:
				*p_wr_val = 0x3;
				ret_val = uart_cg_test(UART2, p_wr_val, p_rd_val);
			break;			
		case ARM_BLK_CG_UART3:
				*p_wr_val = 0x3;
				ret_val = uart_cg_test(UART3, p_wr_val, p_rd_val);
			break;	
		case ARM_BLK_CG_WDT:
				*p_wr_val = 0x1234;
				ret_val = wdt_cg_test(p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_TIMER:
				*p_wr_val = 0x12345678;
				ret_val = timer_cg_test(p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_GPIO:
				*p_wr_val = 0x12345678;
				ret_val = gpio_cg_test(p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_RNG:
				*p_wr_val = 0x8;
				ret_val = dwc_trng_cg_test(p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_USBOTG0:
				*p_wr_val = 0x7F;
				ret_val = usb_cg_test(USB0, p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_USBOTG1:
				*p_wr_val = 0x7F;
				ret_val = usb_cg_test(USB1, p_wr_val, p_rd_val);
			break;
		case ARM_BLK_CG_DDR_CTRL:
				info("not implemented!\n");
				ret_val = false;
			break;			
		default:
			break;
	}
	
	return ret_val;
}

static int prcm_blk_cg_test(int argc, char *argv[])
{
	uint32_t reg_val;
	uint32_t wr_val, rd_val;
	E_ARM_BLK_CG blk; 

	info("%s\n", __func__);

	// Get the ip index
	if (argc >= 2)
	{
		blk = get_arg_ulong(argv[1]);
	}	
	
	// Check the validity of parameter count
	if ((argc < 2) || (blk > ARM_BLK_CG_DDR_CTRL)) 
	{
		info("wrong cmd_format: %s blk\n", argv[0]);
		info("blk: [0, 31]\n");
		
		info("Please input valid block\n");
		info("0:  ARM_BLK_CG_NFC\n");
		info("1:  ARM_BLK_CG_SDMMC0\n");
		info("2:  ARM_BLK_CG_SDMMC1\n");
		info("3:  ARM_BLK_CG_DAM0\n");
		info("4:  ARM_BLK_CG_DAM1\n");
		info("5:  ARM_BLK_CG_DAM2\n");
		info("6:  ARM_BLK_CG_QSPI\n");
		info("7:  ARM_BLK_CG_CRC\n");
		info("8:  ARM_BLK_CG_ECC384\n");
		info("9:  ARM_BLK_CG_SHA384\n");
		info("10: ARM_BLK_CG_AES\n");
		info("11: ARM_BLK_CG_OTP\n");
		info("12: ARM_BLK_CG_IPC\n");
		info("13: ARM_BLK_CG_DBG_TCLK(removed)\n");
		info("14: ARM_BLK_CG_DBG_PCLK(removed)\n");
		info("15: ARM_BLK_CG_I2C0\n");
		info("16: ARM_BLK_CG_I2C1\n");
		info("17: ARM_BLK_CG_I2C2\n");
		info("18: ARM_BLK_CG_SPI0\n");
		info("19: ARM_BLK_CG_SPI1\n");
		info("20: ARM_BLK_CG_SARADC\n");
		info("21: ARM_BLK_CG_UART0\n");
		info("22: ARM_BLK_CG_UART1\n");
		info("23: ARM_BLK_CG_UART2\n");
		info("24: ARM_BLK_CG_UART3\n");
		info("25: ARM_BLK_CG_WDT\n");
		info("26: ARM_BLK_CG_TIMER\n");
		info("27: ARM_BLK_CG_GPIO\n");
		info("28: ARM_BLK_CG_RNG\n");
		info("29: ARM_BLK_CG_USBOTG0\n");
		info("30: ARM_BLK_CG_USBOTG1\n");
		info("31: ARM_BLK_CG_DDR_CTRL(not implemented!)\n");		
		return 1;
	}	

	// block gate test
	if (!blk_cg(blk, &wr_val, &rd_val))
	{
		info("blk(%d) clock gate fail\n", blk);
	}
	else
	{
		info("blk(%d) clock gate pass\n", blk);
	}

	return 0;
}

static bool blk_ds(E_IO_DS_BLK blk, E_IO_DS_MA ds, uint32_t pin_val)
{	
	bool ret_val;
	E_GPIO_PORT port = GPIOA;  
	E_PIN_PORT pin;

	switch (blk)
	{
		case UART0_DS:
				for (pin = PIN_0; pin <= PIN_3; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);	
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_uart_ds(UART0, ds);	
			break;
		case UART1_DS:
				for (pin = PIN_4; pin <= PIN_7; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);	
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_uart_ds(UART1, ds);	
			break;			
		case UART2_DS:
				for (pin = PIN_8; pin <= PIN_11; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);	
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_uart_ds(UART2, ds);	
			break;			
		case UART3_DS:
				for (pin = PIN_12; pin <= PIN_19; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);		
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_uart_ds(UART3, ds);			
			break;	
		case I2C0_DS:
				for (pin = PIN_20; pin <= PIN_21; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);	
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_i2c_ds(I2C0, ds);			
			break;	
		case I2C1_DS:
				for (pin = PIN_22; pin <= PIN_23; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);		
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_i2c_ds(I2C1, ds);
			break;	
		case I2C2_DS:
				for (pin = PIN_24; pin <= PIN_25; pin++)
				{
					// initialize gpio pin
					if (!misc_gpio_pin_init(port, pin, IO_PU_UP, IO_PD_DIS, IO_OD_DIS))
					{
						return 0;
					}

					gpio_init(port, GPIO_BASE, GPIO_PCLK, 0);
					gpio_set_direction(port, pin, GPIO_OUTPUT);	
					gpio_set_output(port, pin, pin_val);
				}
				
				ret_val = misc_i2c_ds(I2C2, ds);
			break;				
		default:
			break;
	}
	
	return ret_val;
}

static int misc_io_ds_test(int argc, char *argv[])
{
	E_IO_DS_BLK blk; 
	E_IO_DS_MA ds;
	uint32_t pin_val;

	info("%s\n", __func__);

	// Get the ip index
	if (argc >= 4)
	{
		blk = get_arg_ulong(argv[1]);
	}	
	
	// Check the validity of parameter count
	if ((argc < 4) || (blk > I2C2_DS)) 
	{
		info("wrong cmd_format: %s blk ds pin_val\n", argv[0]);
		info("blk: [0, 6]\n");
		info("ds: 0, 4mA; 1, 8mA\n");
		info("gpio output: 0, iol; 1, ioh\n");
		
		info("Please input valid block\n");
		info("0:  UART0_DS(console used, don't test)\n");
		info("1:  UART1_DS\n");
		info("2:  UART2_DS\n");
		info("3:  UART3_DS\n");
		info("4:  I2C0_DS\n");
		info("5:  I2C1_DS\n");
		info("6:  I2C2_DS\n");
		return 1;
	}	

	ds = get_arg_ulong(argv[2]);
	if (ds > IO_DS_8MA)
	{
		ds = IO_DS_8MA;
	}

	pin_val = get_arg_ulong(argv[3]);
	if (pin_val > 1)
	{
		pin_val = 1;	
	}
	
	// block gate test
	blk_ds(blk, ds, pin_val);

	info("%s: view the oscilloscope result for pass or fail\n", argv[0]);

	return 0;
}

static int mem_dumps(int argc, char *argv[])
{
	unsigned long addr = 0;
	unsigned long lens = 0;

	info("%s: generally used as memory dump\n",argv[0]);
	info("%s: !!!no para check and please make sure the addr & lens is legal\n",argv[0]);

	if (argc < 3) {
		info("wrong cmd_format: %s addr len\n",argv[0]);
		return -1;
	}

	addr = get_arg_ulong(argv[1]);
	lens = get_arg_ulong(argv[2]);

	dump_memory32(addr, lens);
	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry sys_test_menu[] = {
	{"help",		cmd_help,			" : Display list of commands"},
	{"h",			cmd_help,			"    : alias for help"},
	{"?",			cmd_help,			"    : alias for help"},
	{"reset",		prcm_reset_test,	"  : prcm reset test"},
#if defined(EVB_BOARD)
	{"cg",			prcm_blk_cg_test,	"  : block clock gate test"},
	{"ds",			misc_io_ds_test,	"  : misc io strength driving test"},
#endif
	{"chip_id",		misc_chip_id_test,	"  : misc chip id test"},
	{"dump",		mem_dumps,			"  : memory dump"},
	{"q",			NULL,				" : quit sys test"},
	{0,				0,					0}
};


#endif

