/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_PLL_H__
#define __CDL_PLL_H__

#include "cdl_types.h"

/*register offset*/
#define ARM_PLL_CFG_REG			      0x00
#define ARM_CLK_CFG_REG  			  0x04
#define DDR_PLL_CFG_REG  		      0x08
#define BOOT_STATUS_REG				  0x40

/*ARM_PLL_CFG/DDR_PLL_CFG bit*/
#define PLL_CFG_NDIV_MASK         (0xF)
#define PLL_CFG_MDIV_MASK         (0xFF)
#define PLL_CFG_ODDIV_MASK        (0x3)
#define PLL_CFG_BP_MASK           (0x1)
#define PLL_CFG_POWER_MASK        (0x1)
#define PLL_CFG_SRC_MASK          (0x1)
#define PLL_CFG_LOCK_STAT_MASK    (0x1)

#define PLL_CFG_NDIV_SHIFT        (0)
#define PLL_CFG_MDIV_SHIFT        (4)
#define PLL_CFG_ODDIV_SHIFT       (12)
#define PLL_CFG_BP_SHIFT          (14)
#define PLL_CFG_POWER_SHIFT       (15)
#define PLL_CFG_SRC_SHIFT         (16)
#define PLL_CFG_LOCK_STAT_SHIFT   (31)

/*ARM_CLK_CFG bit*/
#define CLK_CFG_AXIDIV_MASK       (0x3)
#define CLK_CFG_AHBDIV_MASK       (0x3)
#define CLK_CFG_APB0DIV_MASK      (0x3)
#define CLK_CFG_APB1DIV_MASK      (0x3)
#define CLK_CFG_SDMMCDIV_MASK     (0xF)
#define CLK_CFG_NFCDIV_MASK       (0xF)
#define CLK_CFG_UARTDIV_MASK      (0xFF)
#define CLK_CFG_UARTDIV_RST_MASK  (0x1)
#define CLK_CFG_ARM_CLK_SRC_MASK  (0x1)
#define CLK_CFG_ROMDIV_MASK       (0x3)

#define CLK_CFG_AXIDIV_SHIFT       (0)
#define CLK_CFG_AHBDIV_SHIFT       (2)
#define CLK_CFG_APB0DIV_SHIFT      (4)
#define CLK_CFG_APB1DIV_SHIFT      (6)
#define CLK_CFG_SDMMCDIV_SHIFT     (8)
#define CLK_CFG_NFCDIV_SHIFT       (12)
#define CLK_CFG_UARTDIV_SHIFT      (16)
#define CLK_CFG_UARTDIV_RST_SHIFT  (24)
#define CLK_CFG_ARM_CLK_SRC_SHIFT  (25)
#define CLK_CFG_ROMDIV_SHIFT       (26)

/*BOOT_STATUS bit*/
#define BOOT_STATUS_MCLK_MASK       (0x1)
#define BOOT_STATUS_MCLK_SHIFT      (2)

#define PLL_SRC_FROM_MCLK           (0)
#define PLL_SRC_FROM_ROSC           (1)

#define MCLK_36M                    (0)
#define MCLK_24M                    (1)

#define ARM_CLK_SRC_FROM_ROSC_100M  (0)
#define ARM_CLK_SRC_FROM_ARM_PLL    (1)

#define ARM_PLL_TYPE                (0)
#define DDR_PLL_TYPE                (1)

/*ARM BUS CLK div*/
#define BUS_CLK_DIV_1               (0)
#define BUS_CLK_DIV_2               (1)
#define BUS_CLK_DIV_3               (2)
#define BUS_CLK_DIV_4               (3)
#define BUS_CLK_DIV_5               (4)
#define BUS_CLK_DIV_6               (5)
#define BUS_CLK_DIV_7               (6)
#define BUS_CLK_DIV_8               (7)
#define BUS_CLK_DIV_9               (8)
#define BUS_CLK_DIV_10              (9)
#define BUS_CLK_DIV_11              (10)
#define BUS_CLK_DIV_12              (11)
#define BUS_CLK_DIV_13              (12)
#define BUS_CLK_DIV_14              (13)
#define BUS_CLK_DIV_15              (14)
#define BUS_CLK_DIV_16              (15)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define BRITE_PLL_RATE(_rate, _m, _n, _od)			\
	{							\
		.rate	=	(_rate),				\
		.mdiv	=	(_m),				\
		.ndiv	=	(_n),				\
		.oddiv	=	(_od),				\
	}

struct brite_clk_pll {
	void *regs;
	unsigned int rate_count;
	const struct brite_pll_rate_table *rate_table;
};


/* NOTE: Rate table should be kept sorted in descending order. */
struct brite_pll_rate_table {
	unsigned int rate;
	unsigned int ndiv;
	unsigned int mdiv;
	unsigned int oddiv;
};

#if defined(EVB_BOARD)
// MCL is 24MHZ/36MHZ, only for ASIC
#define ARM_PLL_DEFAULT_CLK         (600*1000*1000)          // default: 600MHZ

#define ARM_DEFAULT_CLK         ARM_PLL_DEFAULT_CLK          // default: 600MHZ
#define ARM_AXI_DEFAULT_CLK     (ARM_DEFAULT_CLK/2)          // default: 300MHZ
#define ARM_AHB_DEFAULT_CLK     (ARM_AXI_DEFAULT_CLK/2)      // default: 150MHZ
#define ARM_APB0_DEFAULT_CLK    ARM_AHB_DEFAULT_CLK          // default: 150MHZ
#define ARM_APB1_DEFAULT_CLK    (ARM_AXI_DEFAULT_CLK/4)      // default: 75MHZ
#define ARM_ROM_DEFAULT_CLK     (ARM_AXI_DEFAULT_CLK/2)      // default: 150MHZ

#define ARM_SDMMC_DEFAULT_CLK   (ARM_DEFAULT_CLK/6)          // default: 100MHZ
#define ARM_NFC_DEFAULT_CLK     (ARM_DEFAULT_CLK/3)          // default: 100MHZ, changed to 200MHZ
#define ARM_UART_DEFAULT_CLK    (ARM_AXI_DEFAULT_CLK/5)		 // default: 60MHZ

#define ARM_SRD_AHB_DEFAULT_CLK ARM_AHB_DEFAULT_CLK          // veriry with asic team

#define ARM_PLL         arm_pll.get_rate(&arm_pll)  //(600*1000*1000)      // default: 600MHZ

#define ARM_CLK         arm_clk.get_rate(&arm_clk)  //ARM_PLL              // default: 600MHZ
#define ARM_AXI_CLK     arm_axi_clk.get_rate(&arm_axi_clk)  //(ARM_CLK/2)          // default: 300MHZ
#define ARM_AHB_CLK     arm_ahb_clk.get_rate(&arm_ahb_clk)  //(ARM_AXI_CLK/2)      // default: 150MHZ
#define ARM_APB0_CLK    arm_apb0_clk.get_rate(&arm_apb0_clk)  //ARM_AHB_CLK          // default: 150MHZ
#define ARM_APB1_CLK    arm_apb1_clk.get_rate(&arm_apb1_clk)  //(ARM_AHB_CLK/2)      // default: 75MHZ
#define ARM_ROM_CLK     arm_rom_clk.get_rate(&arm_rom_clk)  //(ARM_AXI_CLK/2)      // default: 150MHZ

#define ARM_SDMMC_CLK   arm_sdmmc_clk.get_rate(&arm_sdmmc_clk)  //(ARM_CLK/6)         // default: 100MHZ
#define ARM_NFC_CLK     arm_nfc_clk.get_rate(&arm_nfc_clk)  //(ARM_CLK/6)          // default: 100MHZ
#define ARM_UART_CLK    arm_uart_clk.get_rate(&arm_uart_clk)  //(ARM_AXI_CLK/5)		 // default: 60MHZ
#define ARM_SRD_AHB_CLK ARM_AHB_CLK          // veriry with asic team

#ifdef TAPEOUT_V2
#define DDR_PLL         (400*1000*1000)      // default: 400MHZ
#else
#define DDR_PLL         (200*1000*1000)      // default: 200MHZ
#endif
#define DDR_PHY_CLK     DDR_PLL              // default: 200MHZ

#define SDMMC_MAX_TRAN_SPEED 50000000
#else
// MCLK is 50MHZ, only for FPGA
#define ARM_CLK         (50*1000*1000)
#define ARM_PLL         ARM_CLK
#define DDR_PLL         ARM_CLK

#define ARM_AXI_CLK     ARM_CLK
#define ARM_AHB_CLK     ARM_CLK
#define ARM_APB0_CLK    ARM_CLK
#define ARM_APB1_CLK    ARM_CLK
#define ARM_ROM_CLK     ARM_CLK
#define ARM_SDMMC_CLK   ARM_CLK
#define ARM_UART_CLK    ARM_CLK
#define ARM_NFC_CLK     ARM_CLK
#define ARM_SRD_AHB_CLK ARM_CLK

#define SDMMC_MAX_TRAN_SPEED 25000000
#endif

/*default bus clk*/
#define BSP_BUS_PCLK    ARM_CLK

#define CONSOLE_PCLK    ARM_UART_CLK
#define UART_PCLK       ARM_UART_CLK
#define GPIO_PCLK       ARM_APB1_CLK
#define I2C_PCLK        ARM_APB0_CLK
#define SPI_PCLK        ARM_APB0_CLK
#define QSPI_PCLK       ARM_AXI_CLK
#define MMC_PCLK        ARM_SDMMC_CLK
#define ADC_PCLK        ARM_APB0_CLK
#define TIMER_PCLK      ARM_APB1_CLK
#define WDT_PCLK        ARM_APB1_CLK
#define NFC_PCLK        ARM_NFC_CLK

// from external crystal
#define OSC_32KHZ_PCLK		32768


struct clk {
	char *name;
	struct clk *parent;
	struct clk *child;
	struct clk *sibling;
	int (*set_parent)(struct clk *clk, struct clk *parent);
	int (*set_rate)(struct clk *clk, u32 rate);
	u32 (*get_rate)(struct clk *clk);

	int (*rate_change_cb)(struct clk *clk);
};

extern struct clk root_xtal_24mhz;
extern struct clk root_xtal_36mhz;
extern struct clk *root_xtal_act;
extern struct clk root_rosc_100mhz;
extern struct clk root_rosc_25mhz;
extern struct clk arm_pll;
extern struct clk arm_clk;
extern struct clk arm_axi_clk;
extern struct clk arm_ahb_clk;
extern struct clk arm_apb0_clk;
extern struct clk arm_apb1_clk;
extern struct clk arm_rom_clk;
extern struct clk arm_sdmmc_clk;
extern struct clk arm_nfc_clk;
extern struct clk arm_uart_clk;


#define CLK_IS_ROOT	0x1
#define CLK_IS_PLL	0x2
#define CLK_IS_BUS	0x4
#define CLK_IS_PERIPH	0x8
#define CLK_IS_ARM_PLL_SRC	0x10
#define CLK_IS_ARM_CLK_SRC	0x20

struct clk_name_map {
	const char *name;
	struct clk *clk;
	unsigned long flags;
};

int brite_arm_pll_set_parent(struct clk *clk, struct clk *parent);
u32 brite_arm_pll_get_rate(struct clk *clk);
u32 brite_arm_pll_get_actual_rate(u32 clk_out, unsigned int xin_type);
u32 brite_arm_pll_get_current_xin_type(void);
int brite_arm_pll_set_rate(struct clk *clk, u32 clk_out);

int brite_set_arm_clk_parent(struct clk *clk, struct clk *parent);
int brite_arm_clk_src_select(unsigned int src_type);
u32 brite_get_arm_clk_rate(struct clk *clk);

u32 brite_get_arm_axi_clk_rate(struct clk *clk);
int brite_set_arm_axi_clk_rate(struct clk *clk, u32 clk_out);

u32 brite_get_arm_ahb_clk_rate(struct clk *clk);
int brite_set_arm_ahb_clk_rate(struct clk *clk, u32 clk_out);

u32 brite_get_arm_apb0_clk_rate(struct clk *clk);
int brite_set_arm_apb0_clk_rate(struct clk *clk, u32 clk_out);

u32 brite_get_arm_apb1_clk_rate(struct clk *clk);
int brite_set_arm_apb1_clk_rate(struct clk *clk, u32 clk_out);

u32 brite_get_arm_rom_clk_rate(struct clk *clk);
int brite_set_arm_rom_clk_rate(struct clk *clk, u32 clk_out);

u32 brite_get_sdmmc_clk_rate(struct clk *clk);
int brite_set_sdmmc_clk_rate(struct clk *clk, u32 clk_out);

u32 brite_get_nfc_clk_rate(struct clk *clk);
int brite_set_nfc_clk_rate(struct clk *clk, u32 clk_out);

void brite_uart_clk_div_block_reset(int reset);
u32 brite_get_uart_clk_rate_2(struct clk *clk);


/*APIs*/
void brite_pll_init(u32 base);

struct clk * clk_name_to_clk(const char *name, unsigned long filter);
void print_clk_name(unsigned long filter);

u32 clk_get_parent_rate(struct clk *clk);
u32 clk_get_rate(struct clk *clk);
int clk_set_rate(struct clk *clk, u32 rate);
int clk_set_parent(struct clk *clk, struct clk *parent);

void clk_register_callback(struct clk *clk, int (*cb)(struct clk *));

int uart_tune_clk_for_baud(struct clk *clk, unsigned long baud);
int console_pclk_change_cb(struct clk *clk);
int system_timer_pclk_change_cb(struct clk *clk);

u32 brite_ddr_pll_get_rate(void);
int brite_ddr_pll_set_rate(u32 clk_out, unsigned int xin_type);

u32 brite_get_uart_clk_rate(void);
u32 brite_get_uart_clk_div(void);
int brite_set_uart_clk_div(u32 div);

int brite_set_all_clock_default(void);
void brite_get_all_clock_rate(void);

#endif

