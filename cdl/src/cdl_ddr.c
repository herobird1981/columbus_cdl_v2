/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/

#include "cdl_config.h"
#include "cdl_ddr.h"
#include "cdl_sys.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
#define DDR_TIMEOUT (1000) //ms

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
struct brite_ddr_ctrl ddr_ctrl_dev;
struct brite_ddr_ctrl *p_ddr_ctrl = &ddr_ctrl_dev;

struct brite_ddr_phy ddr_phy_dev;
struct brite_ddr_phy *p_ddr_phy = &ddr_phy_dev;

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
void ddr_var_init(uint32_t ctrl_base, uint32_t phy_base)
{
	ddr_info("ddr_var_init\n");

	p_ddr_ctrl->regs = (void *)ctrl_base;
	p_ddr_phy->regs = (void *)phy_base;
}

void ddr_set_clk(uint32_t base, E_DDR1_CLK clk)
{
	uint32_t reg_val;
	uint32_t div;

	switch (clk)
	{
		case DDR_CLK_100MHZ:
				div = 0x00002c89;

				ddr_info("================================\n");
				ddr_info("|  PLL is select to 100 M !!! |\n");
				ddr_info("================================\n");
			break;
		case DDR_CLK_133MHZ:
				div = 0x000023b2;

				ddr_info("================================\n");
				ddr_info("|  PLL is select to 133 M !!! |\n");
				ddr_info("================================\n");
			break;
		case DDR_CLK_200MHZ:
				div = 0x00001c89;

				ddr_info("================================\n");
				ddr_info("|  PLL is select to 200 M !!! |\n");
				ddr_info("================================\n");
			break;
		case DDR_CLK_400MHZ:
				div = 0x00000c89;

				ddr_info("================================\n");
				ddr_info("|  PLL is select to 400 M !!! |\n");
				ddr_info("================================\n");
			break;
		case DDR_CLK_533MHZ:
				div = 0x00000945;

				ddr_info("================================\n");
				ddr_info("|  PLL is select to 533 M !!! |\n");
				ddr_info("================================\n");
			break;
		default:
				ddr_info("================================\n");
				ddr_info("|  PLL select is not valid !!! |\n");
				ddr_info("================================\n");
			break;
	}

	// configure PLL n, m and od
	reg_val = __raw_readl(base + DDR_PLL_CFG_REG);
	reg_val &= 0xffffc000;
	reg_val |= div;
	__raw_writel(reg_val, (base + DDR_PLL_CFG_REG));

	// PLL is power down
	reg_val = __raw_readl(base + DDR_PLL_CFG_REG);
	reg_val |= 0x00008000;
	__raw_writel(reg_val, (base + DDR_PLL_CFG_REG));

	// PLL is power on
	reg_val = __raw_readl(base + DDR_PLL_CFG_REG);
	reg_val &= 0xffff7fff;
	__raw_writel(reg_val, (base + DDR_PLL_CFG_REG));

	//check PLL status
	// 2.4. Check bit31 of ARM_PLL_CFG until the PLLs lock are ready;
	//		HW make sure the lock bit must be set in max 500us
	while (1)
	{
		reg_val = __raw_readl(PRCM_BASE + DDR_PLL_CFG_REG);
		if ((reg_val>>31) == 0x1)
		{
			break;
		}
	}	
}

static void lpddr_ctrl_100mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_CTL_00_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR_CTL_01_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR_CTL_02_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR_CTL_03_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR_CTL_04_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR_CTL_05_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR_CTL_06_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR_CTL_07_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR_CTL_08_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR_CTL_09_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR_CTL_10_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR_CTL_11_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR_CTL_12_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR_CTL_13_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR_CTL_14_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR_CTL_15_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR_CTL_16_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR_CTL_17_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR_CTL_18_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR_CTL_19_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR_CTL_20_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR_CTL_21_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR_CTL_22_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR_CTL_23_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR_CTL_24_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR_CTL_25_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR_CTL_26_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR_CTL_27_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR_CTL_28_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR_CTL_29_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR_CTL_30_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR_CTL_31_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR_CTL_32_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR_CTL_33_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR_CTL_34_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR_CTL_35_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR_CTL_36_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR_CTL_37_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR_CTL_38_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR_CTL_39_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR_CTL_40_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR_CTL_41_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR_CTL_42_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR_CTL_43_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR_CTL_44_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR_CTL_45_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR_CTL_46_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR_CTL_47_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR_CTL_48_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR_CTL_49_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR_CTL_50_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR_CTL_51_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR_CTL_52_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR_CTL_53_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR_CTL_54_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR_CTL_55_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR_CTL_56_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR_CTL_57_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR_CTL_58_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR_CTL_59_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR_CTL_60_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR_CTL_61_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR_CTL_62_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR_CTL_63_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR_CTL_64_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR_CTL_65_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR_CTL_66_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR_CTL_67_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR_CTL_68_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR_CTL_69_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR_CTL_70_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR_CTL_71_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR_CTL_72_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR_CTL_73_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR_CTL_74_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR_CTL_75_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR_CTL_76_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR_CTL_77_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR_CTL_78_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR_CTL_79_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR_CTL_80_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR_CTL_81_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR_CTL_82_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR_CTL_83_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR_CTL_84_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR_CTL_85_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR_CTL_86_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR_CTL_87_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR_CTL_88_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR_CTL_89_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR_CTL_90_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR_CTL_91_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR_CTL_92_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR_CTL_93_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR_CTL_94_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR_CTL_95_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR_CTL_96_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR_CTL_97_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR_CTL_98_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR_CTL_99_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR_CTL_100_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR_CTL_101_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR_CTL_102_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR_CTL_103_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR_CTL_104_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR_CTL_105_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR_CTL_106_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR_CTL_107_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR_CTL_108_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR_CTL_109_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR_CTL_110_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR_CTL_111_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR_CTL_112_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR_CTL_113_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR_CTL_114_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR_CTL_115_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR_CTL_116_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR_CTL_117_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR_CTL_118_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR_CTL_119_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR_CTL_120_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR_CTL_121_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR_CTL_122_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR_CTL_123_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR_CTL_124_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR_CTL_125_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR_CTL_126_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR_CTL_127_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR_CTL_128_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR_CTL_129_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR_CTL_130_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR_CTL_131_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR_CTL_132_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR_CTL_133_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR_CTL_134_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR_CTL_135_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR_CTL_136_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR_CTL_137_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR_CTL_138_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR_CTL_139_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR_CTL_140_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR_CTL_141_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR_CTL_142_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR_CTL_143_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR_CTL_144_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR_CTL_145_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR_CTL_146_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR_CTL_147_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR_CTL_148_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR_CTL_149_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR_CTL_150_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR_CTL_151_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR_CTL_152_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR_CTL_153_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR_CTL_154_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR_CTL_155_100MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr_ctrl_133mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_CTL_00_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR_CTL_01_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR_CTL_02_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR_CTL_03_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR_CTL_04_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR_CTL_05_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR_CTL_06_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR_CTL_07_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR_CTL_08_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR_CTL_09_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR_CTL_10_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR_CTL_11_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR_CTL_12_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR_CTL_13_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR_CTL_14_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR_CTL_15_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR_CTL_16_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR_CTL_17_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR_CTL_18_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR_CTL_19_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR_CTL_20_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR_CTL_21_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR_CTL_22_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR_CTL_23_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR_CTL_24_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR_CTL_25_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR_CTL_26_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR_CTL_27_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR_CTL_28_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR_CTL_29_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR_CTL_30_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR_CTL_31_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR_CTL_32_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR_CTL_33_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR_CTL_34_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR_CTL_35_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR_CTL_36_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR_CTL_37_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR_CTL_38_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR_CTL_39_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR_CTL_40_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR_CTL_41_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR_CTL_42_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR_CTL_43_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR_CTL_44_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR_CTL_45_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR_CTL_46_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR_CTL_47_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR_CTL_48_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR_CTL_49_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR_CTL_50_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR_CTL_51_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR_CTL_52_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR_CTL_53_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR_CTL_54_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR_CTL_55_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR_CTL_56_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR_CTL_57_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR_CTL_58_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR_CTL_59_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR_CTL_60_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR_CTL_61_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR_CTL_62_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR_CTL_63_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR_CTL_64_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR_CTL_65_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR_CTL_66_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR_CTL_67_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR_CTL_68_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR_CTL_69_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR_CTL_70_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR_CTL_71_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR_CTL_72_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR_CTL_73_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR_CTL_74_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR_CTL_75_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR_CTL_76_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR_CTL_77_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR_CTL_78_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR_CTL_79_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR_CTL_80_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR_CTL_81_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR_CTL_82_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR_CTL_83_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR_CTL_84_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR_CTL_85_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR_CTL_86_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR_CTL_87_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR_CTL_88_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR_CTL_89_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR_CTL_90_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR_CTL_91_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR_CTL_92_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR_CTL_93_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR_CTL_94_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR_CTL_95_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR_CTL_96_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR_CTL_97_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR_CTL_98_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR_CTL_99_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR_CTL_100_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR_CTL_101_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR_CTL_102_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR_CTL_103_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR_CTL_104_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR_CTL_105_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR_CTL_106_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR_CTL_107_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR_CTL_108_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR_CTL_109_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR_CTL_110_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR_CTL_111_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR_CTL_112_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR_CTL_113_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR_CTL_114_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR_CTL_115_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR_CTL_116_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR_CTL_117_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR_CTL_118_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR_CTL_119_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR_CTL_120_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR_CTL_121_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR_CTL_122_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR_CTL_123_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR_CTL_124_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR_CTL_125_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR_CTL_126_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR_CTL_127_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR_CTL_128_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR_CTL_129_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR_CTL_130_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR_CTL_131_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR_CTL_132_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR_CTL_133_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR_CTL_134_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR_CTL_135_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR_CTL_136_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR_CTL_137_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR_CTL_138_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR_CTL_139_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR_CTL_140_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR_CTL_141_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR_CTL_142_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR_CTL_143_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR_CTL_144_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR_CTL_145_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR_CTL_146_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR_CTL_147_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR_CTL_148_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR_CTL_149_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR_CTL_150_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR_CTL_151_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR_CTL_152_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR_CTL_153_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR_CTL_154_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR_CTL_155_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr_ctrl_133mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_CTL_00_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR_CTL_01_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR_CTL_02_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR_CTL_03_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR_CTL_04_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR_CTL_05_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR_CTL_06_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR_CTL_07_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR_CTL_08_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR_CTL_09_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR_CTL_10_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR_CTL_11_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR_CTL_12_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR_CTL_13_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR_CTL_14_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR_CTL_15_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR_CTL_16_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR_CTL_17_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR_CTL_18_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR_CTL_19_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR_CTL_20_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR_CTL_21_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR_CTL_22_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR_CTL_23_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR_CTL_24_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR_CTL_25_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR_CTL_26_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR_CTL_27_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR_CTL_28_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR_CTL_29_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR_CTL_30_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR_CTL_31_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR_CTL_32_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR_CTL_33_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR_CTL_34_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR_CTL_35_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR_CTL_36_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR_CTL_37_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR_CTL_38_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR_CTL_39_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR_CTL_40_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR_CTL_41_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR_CTL_42_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR_CTL_43_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR_CTL_44_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR_CTL_45_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR_CTL_46_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR_CTL_47_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR_CTL_48_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR_CTL_49_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR_CTL_50_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR_CTL_51_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR_CTL_52_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR_CTL_53_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR_CTL_54_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR_CTL_55_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR_CTL_56_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR_CTL_57_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR_CTL_58_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR_CTL_59_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR_CTL_60_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR_CTL_61_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR_CTL_62_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR_CTL_63_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR_CTL_64_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR_CTL_65_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR_CTL_66_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR_CTL_67_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR_CTL_68_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR_CTL_69_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR_CTL_70_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR_CTL_71_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR_CTL_72_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR_CTL_73_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR_CTL_74_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR_CTL_75_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR_CTL_76_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR_CTL_77_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR_CTL_78_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR_CTL_79_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR_CTL_80_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR_CTL_81_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR_CTL_82_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR_CTL_83_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR_CTL_84_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR_CTL_85_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR_CTL_86_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR_CTL_87_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR_CTL_88_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR_CTL_89_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR_CTL_90_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR_CTL_91_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR_CTL_92_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR_CTL_93_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR_CTL_94_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR_CTL_95_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR_CTL_96_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR_CTL_97_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR_CTL_98_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR_CTL_99_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR_CTL_100_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR_CTL_101_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR_CTL_102_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR_CTL_103_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR_CTL_104_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR_CTL_105_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR_CTL_106_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR_CTL_107_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR_CTL_108_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR_CTL_109_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR_CTL_110_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR_CTL_111_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR_CTL_112_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR_CTL_113_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR_CTL_114_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR_CTL_115_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR_CTL_116_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR_CTL_117_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR_CTL_118_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR_CTL_119_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR_CTL_120_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR_CTL_121_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR_CTL_122_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR_CTL_123_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR_CTL_124_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR_CTL_125_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR_CTL_126_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR_CTL_127_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR_CTL_128_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR_CTL_129_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR_CTL_130_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR_CTL_131_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR_CTL_132_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR_CTL_133_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR_CTL_134_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR_CTL_135_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR_CTL_136_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR_CTL_137_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR_CTL_138_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR_CTL_139_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR_CTL_140_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR_CTL_141_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR_CTL_142_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR_CTL_143_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR_CTL_144_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR_CTL_145_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR_CTL_146_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR_CTL_147_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR_CTL_148_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR_CTL_149_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR_CTL_150_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR_CTL_151_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR_CTL_152_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR_CTL_153_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR_CTL_154_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR_CTL_155_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}


static void lpddr_ctrl_200mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_CTL_00_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR_CTL_01_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR_CTL_02_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR_CTL_03_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR_CTL_04_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR_CTL_05_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR_CTL_06_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR_CTL_07_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR_CTL_08_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR_CTL_09_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR_CTL_10_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR_CTL_11_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR_CTL_12_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR_CTL_13_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR_CTL_14_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR_CTL_15_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR_CTL_16_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR_CTL_17_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR_CTL_18_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR_CTL_19_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR_CTL_20_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR_CTL_21_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR_CTL_22_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR_CTL_23_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR_CTL_24_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR_CTL_25_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR_CTL_26_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR_CTL_27_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR_CTL_28_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR_CTL_29_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR_CTL_30_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR_CTL_31_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR_CTL_32_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR_CTL_33_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR_CTL_34_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR_CTL_35_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR_CTL_36_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR_CTL_37_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR_CTL_38_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR_CTL_39_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR_CTL_40_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR_CTL_41_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR_CTL_42_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR_CTL_43_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR_CTL_44_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR_CTL_45_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR_CTL_46_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR_CTL_47_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR_CTL_48_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR_CTL_49_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR_CTL_50_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR_CTL_51_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR_CTL_52_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR_CTL_53_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR_CTL_54_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR_CTL_55_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR_CTL_56_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR_CTL_57_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR_CTL_58_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR_CTL_59_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR_CTL_60_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR_CTL_61_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR_CTL_62_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR_CTL_63_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR_CTL_64_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR_CTL_65_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR_CTL_66_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR_CTL_67_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR_CTL_68_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR_CTL_69_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR_CTL_70_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR_CTL_71_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR_CTL_72_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR_CTL_73_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR_CTL_74_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR_CTL_75_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR_CTL_76_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR_CTL_77_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR_CTL_78_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR_CTL_79_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR_CTL_80_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR_CTL_81_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR_CTL_82_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR_CTL_83_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR_CTL_84_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR_CTL_85_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR_CTL_86_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR_CTL_87_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR_CTL_88_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR_CTL_89_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR_CTL_90_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR_CTL_91_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR_CTL_92_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR_CTL_93_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR_CTL_94_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR_CTL_95_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR_CTL_96_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR_CTL_97_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR_CTL_98_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR_CTL_99_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR_CTL_100_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR_CTL_101_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR_CTL_102_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR_CTL_103_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR_CTL_104_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR_CTL_105_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR_CTL_106_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR_CTL_107_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR_CTL_108_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR_CTL_109_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR_CTL_110_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR_CTL_111_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR_CTL_112_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR_CTL_113_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR_CTL_114_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR_CTL_115_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR_CTL_116_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR_CTL_117_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR_CTL_118_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR_CTL_119_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR_CTL_120_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR_CTL_121_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR_CTL_122_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR_CTL_123_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR_CTL_124_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR_CTL_125_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR_CTL_126_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR_CTL_127_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR_CTL_128_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR_CTL_129_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR_CTL_130_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR_CTL_131_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR_CTL_132_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR_CTL_133_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR_CTL_134_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR_CTL_135_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR_CTL_136_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR_CTL_137_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR_CTL_138_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR_CTL_139_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR_CTL_140_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR_CTL_141_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR_CTL_142_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR_CTL_143_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR_CTL_144_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR_CTL_145_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR_CTL_146_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR_CTL_147_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR_CTL_148_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR_CTL_149_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR_CTL_150_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR_CTL_151_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR_CTL_152_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR_CTL_153_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR_CTL_154_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR_CTL_155_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr_ctrl_200mhz_cl3_bl8_config(void)
{
	__raw_writel(DENALI_LPDDR_CTL_00_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR_CTL_01_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR_CTL_02_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR_CTL_03_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR_CTL_04_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR_CTL_05_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR_CTL_06_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR_CTL_07_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR_CTL_08_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR_CTL_09_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR_CTL_10_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR_CTL_11_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR_CTL_12_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR_CTL_13_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR_CTL_14_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR_CTL_15_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR_CTL_16_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR_CTL_17_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR_CTL_18_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR_CTL_19_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR_CTL_20_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR_CTL_21_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR_CTL_22_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR_CTL_23_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR_CTL_24_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR_CTL_25_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR_CTL_26_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR_CTL_27_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR_CTL_28_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR_CTL_29_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR_CTL_30_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR_CTL_31_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR_CTL_32_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR_CTL_33_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR_CTL_34_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR_CTL_35_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR_CTL_36_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR_CTL_37_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR_CTL_38_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR_CTL_39_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR_CTL_40_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR_CTL_41_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR_CTL_42_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR_CTL_43_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR_CTL_44_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR_CTL_45_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR_CTL_46_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR_CTL_47_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR_CTL_48_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR_CTL_49_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR_CTL_50_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR_CTL_51_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR_CTL_52_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR_CTL_53_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR_CTL_54_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR_CTL_55_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR_CTL_56_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR_CTL_57_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR_CTL_58_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR_CTL_59_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR_CTL_60_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR_CTL_61_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR_CTL_62_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR_CTL_63_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR_CTL_64_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR_CTL_65_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR_CTL_66_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR_CTL_67_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR_CTL_68_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR_CTL_69_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR_CTL_70_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR_CTL_71_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR_CTL_72_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR_CTL_73_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR_CTL_74_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR_CTL_75_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR_CTL_76_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR_CTL_77_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR_CTL_78_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR_CTL_79_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR_CTL_80_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR_CTL_81_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR_CTL_82_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR_CTL_83_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR_CTL_84_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR_CTL_85_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR_CTL_86_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR_CTL_87_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR_CTL_88_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR_CTL_89_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR_CTL_90_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR_CTL_91_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR_CTL_92_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR_CTL_93_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR_CTL_94_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR_CTL_95_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR_CTL_96_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR_CTL_97_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR_CTL_98_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR_CTL_99_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR_CTL_100_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR_CTL_101_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR_CTL_102_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR_CTL_103_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR_CTL_104_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR_CTL_105_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR_CTL_106_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR_CTL_107_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR_CTL_108_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR_CTL_109_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR_CTL_110_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR_CTL_111_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR_CTL_112_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR_CTL_113_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR_CTL_114_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR_CTL_115_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR_CTL_116_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR_CTL_117_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR_CTL_118_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR_CTL_119_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR_CTL_120_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR_CTL_121_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR_CTL_122_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR_CTL_123_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR_CTL_124_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR_CTL_125_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR_CTL_126_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR_CTL_127_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR_CTL_128_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR_CTL_129_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR_CTL_130_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR_CTL_131_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR_CTL_132_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR_CTL_133_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR_CTL_134_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR_CTL_135_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR_CTL_136_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR_CTL_137_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR_CTL_138_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR_CTL_139_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR_CTL_140_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR_CTL_141_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR_CTL_142_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR_CTL_143_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR_CTL_144_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR_CTL_145_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR_CTL_146_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR_CTL_147_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR_CTL_148_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR_CTL_149_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR_CTL_150_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR_CTL_151_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR_CTL_152_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR_CTL_153_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR_CTL_154_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR_CTL_155_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
}

static void lpddr2_ctrl_133mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_133MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_133mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_133MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_200mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_200MHZ_CL3_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_200mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_200MHZ_CL3_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_400mhz_cl6_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_400MHZ_CL6_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_400mhz_cl6_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_400MHZ_CL6_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_533mhz_cl8_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_533MHZ_CL8_BL4_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

static void lpddr2_ctrl_533mhz_cl8_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_CTL_00_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_CTL_01_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_CTL_02_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_CTL_03_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_CTL_04_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_CTL_05_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_CTL_06_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_CTL_07_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_CTL_08_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_CTL_09_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_CTL_10_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_CTL_11_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_CTL_12_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_CTL_13_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_CTL_14_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_CTL_15_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_CTL_16_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_CTL_17_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_CTL_18_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_CTL_19_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_CTL_20_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_CTL_21_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_CTL_22_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_CTL_23_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_CTL_24_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_CTL_25_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_CTL_26_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_CTL_27_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_CTL_28_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_CTL_29_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_CTL_30_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_CTL_31_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_CTL_32_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_CTL_33_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_CTL_34_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_CTL_35_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_CTL_36_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_CTL_37_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_CTL_38_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_CTL_39_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_CTL_40_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_CTL_41_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_CTL_42_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_CTL_43_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_CTL_44_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_CTL_45_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_CTL_46_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_CTL_47_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_CTL_48_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_CTL_49_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_CTL_50_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_CTL_51_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_CTL_52_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_CTL_53_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_CTL_54_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_CTL_55_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_CTL_56_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_CTL_57_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_CTL_58_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_CTL_59_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_CTL_60_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_CTL_61_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_CTL_62_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_CTL_63_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_CTL_64_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_CTL_65_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_CTL_66_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_CTL_67_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_CTL_68_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_CTL_69_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_CTL_70_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_CTL_71_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_CTL_72_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_CTL_73_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_CTL_74_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_CTL_75_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_CTL_76_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_CTL_77_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_CTL_78_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_CTL_79_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (79*4));
	__raw_writel(DENALI_LPDDR2_CTL_80_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_CTL_81_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_CTL_82_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_CTL_83_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_CTL_84_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_CTL_85_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (85*4));
	__raw_writel(DENALI_LPDDR2_CTL_86_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (86*4));
	__raw_writel(DENALI_LPDDR2_CTL_87_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (87*4));
	__raw_writel(DENALI_LPDDR2_CTL_88_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (88*4));
	__raw_writel(DENALI_LPDDR2_CTL_89_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (89*4));
	__raw_writel(DENALI_LPDDR2_CTL_90_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_CTL_91_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (91*4));
	__raw_writel(DENALI_LPDDR2_CTL_92_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (92*4));
	__raw_writel(DENALI_LPDDR2_CTL_93_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (93*4));
	__raw_writel(DENALI_LPDDR2_CTL_94_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (94*4));
	__raw_writel(DENALI_LPDDR2_CTL_95_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (95*4));
	__raw_writel(DENALI_LPDDR2_CTL_96_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (96*4));
	__raw_writel(DENALI_LPDDR2_CTL_97_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (97*4));
	__raw_writel(DENALI_LPDDR2_CTL_98_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (98*4));
	__raw_writel(DENALI_LPDDR2_CTL_99_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_ctrl->regs + (99*4));
	__raw_writel(DENALI_LPDDR2_CTL_100_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (100*4));
	__raw_writel(DENALI_LPDDR2_CTL_101_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (101*4));
	__raw_writel(DENALI_LPDDR2_CTL_102_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (102*4));
	__raw_writel(DENALI_LPDDR2_CTL_103_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (103*4));
	__raw_writel(DENALI_LPDDR2_CTL_104_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (104*4));
	__raw_writel(DENALI_LPDDR2_CTL_105_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (105*4));
	__raw_writel(DENALI_LPDDR2_CTL_106_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (106*4));
	__raw_writel(DENALI_LPDDR2_CTL_107_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (107*4));
	__raw_writel(DENALI_LPDDR2_CTL_108_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (108*4));
	__raw_writel(DENALI_LPDDR2_CTL_109_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (109*4));
	__raw_writel(DENALI_LPDDR2_CTL_110_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (110*4));
	__raw_writel(DENALI_LPDDR2_CTL_111_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (111*4));
	__raw_writel(DENALI_LPDDR2_CTL_112_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (112*4));
	__raw_writel(DENALI_LPDDR2_CTL_113_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (113*4));
	__raw_writel(DENALI_LPDDR2_CTL_114_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (114*4));
	__raw_writel(DENALI_LPDDR2_CTL_115_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (115*4));
	__raw_writel(DENALI_LPDDR2_CTL_116_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (116*4));
	__raw_writel(DENALI_LPDDR2_CTL_117_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (117*4));
	__raw_writel(DENALI_LPDDR2_CTL_118_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (118*4));
	__raw_writel(DENALI_LPDDR2_CTL_119_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (119*4));
	__raw_writel(DENALI_LPDDR2_CTL_120_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (120*4));
	__raw_writel(DENALI_LPDDR2_CTL_121_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (121*4));
	__raw_writel(DENALI_LPDDR2_CTL_122_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (122*4));
	__raw_writel(DENALI_LPDDR2_CTL_123_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (123*4));
	__raw_writel(DENALI_LPDDR2_CTL_124_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (124*4));
	__raw_writel(DENALI_LPDDR2_CTL_125_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (125*4));
	__raw_writel(DENALI_LPDDR2_CTL_126_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (126*4));
	__raw_writel(DENALI_LPDDR2_CTL_127_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (127*4));
	__raw_writel(DENALI_LPDDR2_CTL_128_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (128*4));
	__raw_writel(DENALI_LPDDR2_CTL_129_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (129*4));
	__raw_writel(DENALI_LPDDR2_CTL_130_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (130*4));
	__raw_writel(DENALI_LPDDR2_CTL_131_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (131*4));
	__raw_writel(DENALI_LPDDR2_CTL_132_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (132*4));
	__raw_writel(DENALI_LPDDR2_CTL_133_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (133*4));
	__raw_writel(DENALI_LPDDR2_CTL_134_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (134*4));
	__raw_writel(DENALI_LPDDR2_CTL_135_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (135*4));
	__raw_writel(DENALI_LPDDR2_CTL_136_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (136*4));
	__raw_writel(DENALI_LPDDR2_CTL_137_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (137*4));
	__raw_writel(DENALI_LPDDR2_CTL_138_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (138*4));
	__raw_writel(DENALI_LPDDR2_CTL_139_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (139*4));
	__raw_writel(DENALI_LPDDR2_CTL_140_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (140*4));
	__raw_writel(DENALI_LPDDR2_CTL_141_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (141*4));
	__raw_writel(DENALI_LPDDR2_CTL_142_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (142*4));
	__raw_writel(DENALI_LPDDR2_CTL_143_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (143*4));
	__raw_writel(DENALI_LPDDR2_CTL_144_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (144*4));
	__raw_writel(DENALI_LPDDR2_CTL_145_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (145*4));
	__raw_writel(DENALI_LPDDR2_CTL_146_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (146*4));
	__raw_writel(DENALI_LPDDR2_CTL_147_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (147*4));
	__raw_writel(DENALI_LPDDR2_CTL_148_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (148*4));
	__raw_writel(DENALI_LPDDR2_CTL_149_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (149*4));
	__raw_writel(DENALI_LPDDR2_CTL_150_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (150*4));
	__raw_writel(DENALI_LPDDR2_CTL_151_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (151*4));
	__raw_writel(DENALI_LPDDR2_CTL_152_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (152*4));
	__raw_writel(DENALI_LPDDR2_CTL_153_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (153*4));
	__raw_writel(DENALI_LPDDR2_CTL_154_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (154*4));
	__raw_writel(DENALI_LPDDR2_CTL_155_533MHZ_CL8_BL8_DATA, (uint32_t)p_ddr_ctrl->regs + (155*4));
#endif
}

void ddr_ctrl_config(E_DDR_CONFIG ddr_cfg)
{
	switch (ddr_cfg)
	{
		case LPDDR_CONFIG_100MHZ_CL3_BL8 :
				lpddr_ctrl_100mhz_cl3_bl8_config();
			break;
		case LPDDR_CONFIG_133MHZ_CL3_BL4 :
				lpddr_ctrl_133mhz_cl3_bl4_config();
			break;
		case LPDDR_CONFIG_133MHZ_CL3_BL8 :
				lpddr_ctrl_133mhz_cl3_bl8_config();
			break;
		case LPDDR_CONFIG_200MHZ_CL3_BL4 :
				lpddr_ctrl_200mhz_cl3_bl4_config();
			break;
		case LPDDR_CONFIG_200MHZ_CL3_BL8 :
				lpddr_ctrl_200mhz_cl3_bl8_config();
			break;
		case LPDDR2_CONFIG_133MHZ_CL3_BL4 :
				lpddr2_ctrl_133mhz_cl3_bl4_config();
			break;
		case LPDDR2_CONFIG_133MHZ_CL3_BL8 :
				lpddr2_ctrl_133mhz_cl3_bl8_config();
			break;
		case LPDDR2_CONFIG_200MHZ_CL3_BL4 :
				lpddr2_ctrl_200mhz_cl3_bl4_config();
			break;
		case LPDDR2_CONFIG_200MHZ_CL3_BL8 :
				lpddr2_ctrl_200mhz_cl3_bl8_config();
			break;
		case LPDDR2_CONFIG_400MHZ_CL6_BL4 :
				lpddr2_ctrl_400mhz_cl6_bl4_config();
			break;
		case LPDDR2_CONFIG_400MHZ_CL6_BL8 :
				lpddr2_ctrl_400mhz_cl6_bl8_config();
			break;
		case LPDDR2_CONFIG_533MHZ_CL8_BL4 :
				lpddr2_ctrl_533mhz_cl8_bl4_config();
			break;
		case LPDDR2_CONFIG_533MHZ_CL8_BL8 :
				lpddr2_ctrl_533mhz_cl8_bl8_config();
			break;
		default:
				ddr_info("ddr contrller configre invalid!");
			break;
	}
}

static void lpddr_phy_100mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_PHY_80_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR_PHY_81_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR_PHY_82_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR_PHY_83_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR_PHY_84_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR_PHY_85_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR_PHY_00_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR_PHY_01_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR_PHY_02_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR_PHY_03_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR_PHY_04_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR_PHY_05_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR_PHY_06_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR_PHY_07_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR_PHY_08_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR_PHY_09_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR_PHY_10_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR_PHY_11_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR_PHY_12_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR_PHY_13_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR_PHY_14_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR_PHY_15_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR_PHY_16_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR_PHY_17_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR_PHY_18_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR_PHY_19_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR_PHY_20_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR_PHY_21_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR_PHY_22_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR_PHY_23_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR_PHY_24_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR_PHY_25_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR_PHY_26_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR_PHY_27_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR_PHY_28_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR_PHY_29_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR_PHY_30_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR_PHY_31_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR_PHY_32_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR_PHY_33_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR_PHY_34_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR_PHY_35_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR_PHY_36_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR_PHY_37_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR_PHY_38_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR_PHY_39_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR_PHY_40_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR_PHY_41_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR_PHY_42_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR_PHY_43_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR_PHY_44_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR_PHY_45_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR_PHY_46_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR_PHY_47_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR_PHY_48_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR_PHY_49_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR_PHY_50_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR_PHY_51_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR_PHY_52_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR_PHY_53_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR_PHY_54_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR_PHY_55_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR_PHY_56_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR_PHY_57_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR_PHY_58_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR_PHY_59_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR_PHY_60_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR_PHY_61_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR_PHY_62_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR_PHY_63_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR_PHY_64_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR_PHY_65_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR_PHY_66_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR_PHY_67_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR_PHY_68_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR_PHY_69_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR_PHY_70_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR_PHY_71_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR_PHY_72_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR_PHY_73_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR_PHY_74_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR_PHY_75_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR_PHY_76_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR_PHY_77_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR_PHY_78_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR_PHY_79_100MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr_phy_133mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_PHY_80_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR_PHY_81_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR_PHY_82_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR_PHY_83_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR_PHY_84_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR_PHY_85_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR_PHY_00_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR_PHY_01_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR_PHY_02_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR_PHY_03_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR_PHY_04_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR_PHY_05_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR_PHY_06_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR_PHY_07_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR_PHY_08_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR_PHY_09_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR_PHY_10_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR_PHY_11_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR_PHY_12_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR_PHY_13_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR_PHY_14_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR_PHY_15_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR_PHY_16_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR_PHY_17_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR_PHY_18_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR_PHY_19_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR_PHY_20_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR_PHY_21_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR_PHY_22_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR_PHY_23_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR_PHY_24_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR_PHY_25_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR_PHY_26_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR_PHY_27_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR_PHY_28_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR_PHY_29_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR_PHY_30_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR_PHY_31_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR_PHY_32_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR_PHY_33_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR_PHY_34_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR_PHY_35_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR_PHY_36_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR_PHY_37_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR_PHY_38_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR_PHY_39_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR_PHY_40_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR_PHY_41_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR_PHY_42_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR_PHY_43_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR_PHY_44_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR_PHY_45_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR_PHY_46_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR_PHY_47_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR_PHY_48_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR_PHY_49_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR_PHY_50_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR_PHY_51_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR_PHY_52_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR_PHY_53_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR_PHY_54_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR_PHY_55_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR_PHY_56_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR_PHY_57_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR_PHY_58_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR_PHY_59_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR_PHY_60_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR_PHY_61_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR_PHY_62_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR_PHY_63_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR_PHY_64_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR_PHY_65_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR_PHY_66_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR_PHY_67_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR_PHY_68_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR_PHY_69_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR_PHY_70_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR_PHY_71_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR_PHY_72_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR_PHY_73_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR_PHY_74_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR_PHY_75_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR_PHY_76_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR_PHY_77_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR_PHY_78_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR_PHY_79_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr_phy_133mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_PHY_80_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR_PHY_81_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR_PHY_82_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR_PHY_83_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR_PHY_84_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR_PHY_85_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR_PHY_00_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR_PHY_01_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR_PHY_02_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR_PHY_03_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR_PHY_04_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR_PHY_05_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR_PHY_06_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR_PHY_07_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR_PHY_08_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR_PHY_09_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR_PHY_10_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR_PHY_11_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR_PHY_12_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR_PHY_13_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR_PHY_14_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR_PHY_15_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR_PHY_16_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR_PHY_17_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR_PHY_18_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR_PHY_19_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR_PHY_20_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR_PHY_21_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR_PHY_22_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR_PHY_23_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR_PHY_24_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR_PHY_25_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR_PHY_26_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR_PHY_27_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR_PHY_28_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR_PHY_29_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR_PHY_30_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR_PHY_31_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR_PHY_32_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR_PHY_33_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR_PHY_34_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR_PHY_35_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR_PHY_36_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR_PHY_37_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR_PHY_38_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR_PHY_39_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR_PHY_40_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR_PHY_41_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR_PHY_42_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR_PHY_43_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR_PHY_44_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR_PHY_45_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR_PHY_46_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR_PHY_47_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR_PHY_48_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR_PHY_49_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR_PHY_50_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR_PHY_51_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR_PHY_52_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR_PHY_53_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR_PHY_54_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR_PHY_55_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR_PHY_56_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR_PHY_57_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR_PHY_58_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR_PHY_59_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR_PHY_60_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR_PHY_61_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR_PHY_62_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR_PHY_63_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR_PHY_64_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR_PHY_65_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR_PHY_66_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR_PHY_67_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR_PHY_68_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR_PHY_69_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR_PHY_70_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR_PHY_71_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR_PHY_72_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR_PHY_73_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR_PHY_74_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR_PHY_75_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR_PHY_76_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR_PHY_77_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR_PHY_78_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR_PHY_79_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr_phy_200mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR_PHY_80_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR_PHY_81_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR_PHY_82_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR_PHY_83_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR_PHY_84_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR_PHY_85_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR_PHY_00_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR_PHY_01_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR_PHY_02_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR_PHY_03_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR_PHY_04_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR_PHY_05_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR_PHY_06_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR_PHY_07_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR_PHY_08_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR_PHY_09_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR_PHY_10_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR_PHY_11_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR_PHY_12_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR_PHY_13_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR_PHY_14_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR_PHY_15_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR_PHY_16_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR_PHY_17_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR_PHY_18_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR_PHY_19_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR_PHY_20_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR_PHY_21_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR_PHY_22_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR_PHY_23_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR_PHY_24_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR_PHY_25_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR_PHY_26_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR_PHY_27_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR_PHY_28_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR_PHY_29_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR_PHY_30_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR_PHY_31_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR_PHY_32_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR_PHY_33_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR_PHY_34_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR_PHY_35_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR_PHY_36_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR_PHY_37_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR_PHY_38_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR_PHY_39_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR_PHY_40_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR_PHY_41_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR_PHY_42_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR_PHY_43_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR_PHY_44_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR_PHY_45_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR_PHY_46_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR_PHY_47_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR_PHY_48_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR_PHY_49_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR_PHY_50_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR_PHY_51_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR_PHY_52_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR_PHY_53_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR_PHY_54_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR_PHY_55_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR_PHY_56_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR_PHY_57_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR_PHY_58_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR_PHY_59_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR_PHY_60_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR_PHY_61_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR_PHY_62_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR_PHY_63_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR_PHY_64_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR_PHY_65_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR_PHY_66_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR_PHY_67_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR_PHY_68_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR_PHY_69_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR_PHY_70_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR_PHY_71_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR_PHY_72_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR_PHY_73_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR_PHY_74_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR_PHY_75_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR_PHY_76_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR_PHY_77_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR_PHY_78_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR_PHY_79_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr_phy_200mhz_cl3_bl8_config(void)
{
	__raw_writel(DENALI_LPDDR_PHY_80_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR_PHY_81_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR_PHY_82_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR_PHY_83_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR_PHY_84_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR_PHY_85_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR_PHY_00_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR_PHY_01_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR_PHY_02_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR_PHY_03_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR_PHY_04_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR_PHY_05_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR_PHY_06_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR_PHY_07_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR_PHY_08_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR_PHY_09_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR_PHY_10_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR_PHY_11_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR_PHY_12_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR_PHY_13_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR_PHY_14_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR_PHY_15_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR_PHY_16_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR_PHY_17_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR_PHY_18_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR_PHY_19_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR_PHY_20_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR_PHY_21_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR_PHY_22_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR_PHY_23_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR_PHY_24_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR_PHY_25_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR_PHY_26_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR_PHY_27_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR_PHY_28_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR_PHY_29_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR_PHY_30_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR_PHY_31_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR_PHY_32_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR_PHY_33_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR_PHY_34_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR_PHY_35_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR_PHY_36_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR_PHY_37_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR_PHY_38_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR_PHY_39_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR_PHY_40_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR_PHY_41_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR_PHY_42_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR_PHY_43_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR_PHY_44_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR_PHY_45_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR_PHY_46_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR_PHY_47_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR_PHY_48_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR_PHY_49_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR_PHY_50_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR_PHY_51_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR_PHY_52_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR_PHY_53_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR_PHY_54_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR_PHY_55_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR_PHY_56_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR_PHY_57_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR_PHY_58_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR_PHY_59_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR_PHY_60_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR_PHY_61_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR_PHY_62_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR_PHY_63_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR_PHY_64_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR_PHY_65_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR_PHY_66_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR_PHY_67_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR_PHY_68_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR_PHY_69_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR_PHY_70_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR_PHY_71_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR_PHY_72_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR_PHY_73_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR_PHY_74_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR_PHY_75_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR_PHY_76_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR_PHY_77_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR_PHY_78_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR_PHY_79_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
}

static void lpddr2_phy_133mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_133MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_133mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_133MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_200mhz_cl3_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_200MHZ_CL3_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_200mhz_cl3_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_200MHZ_CL3_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_400mhz_cl6_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_400MHZ_CL6_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_400mhz_cl6_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_400MHZ_CL6_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_533mhz_cl8_bl4_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_533MHZ_CL8_BL4_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}

static void lpddr2_phy_533mhz_cl8_bl8_config(void)
{
#ifdef DDR_DEBUG
	__raw_writel(DENALI_LPDDR2_PHY_80_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (80*4));
	__raw_writel(DENALI_LPDDR2_PHY_81_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (81*4));
	__raw_writel(DENALI_LPDDR2_PHY_82_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (82*4));
	__raw_writel(DENALI_LPDDR2_PHY_83_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (83*4));
	__raw_writel(DENALI_LPDDR2_PHY_84_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (84*4));
	__raw_writel(DENALI_LPDDR2_PHY_85_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (85*4));
	//__raw_writel(DENALI_PHY_90_DATA,  (uint32_t)p_ddr_phy->regs + (90*4));
	__raw_writel(DENALI_LPDDR2_PHY_00_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (0*4));
	__raw_writel(DENALI_LPDDR2_PHY_01_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (1*4));
	__raw_writel(DENALI_LPDDR2_PHY_02_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (2*4));
	__raw_writel(DENALI_LPDDR2_PHY_03_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (3*4));
	__raw_writel(DENALI_LPDDR2_PHY_04_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (4*4));
	__raw_writel(DENALI_LPDDR2_PHY_05_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (5*4));
	__raw_writel(DENALI_LPDDR2_PHY_06_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (6*4));
	__raw_writel(DENALI_LPDDR2_PHY_07_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (7*4));
	__raw_writel(DENALI_LPDDR2_PHY_08_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (8*4));
	__raw_writel(DENALI_LPDDR2_PHY_09_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (9*4));
	__raw_writel(DENALI_LPDDR2_PHY_10_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (10*4));
	__raw_writel(DENALI_LPDDR2_PHY_11_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (11*4));
	__raw_writel(DENALI_LPDDR2_PHY_12_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (12*4));
	__raw_writel(DENALI_LPDDR2_PHY_13_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (13*4));
	__raw_writel(DENALI_LPDDR2_PHY_14_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (14*4));
	__raw_writel(DENALI_LPDDR2_PHY_15_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (15*4));
	__raw_writel(DENALI_LPDDR2_PHY_16_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (16*4));
	__raw_writel(DENALI_LPDDR2_PHY_17_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (17*4));
	__raw_writel(DENALI_LPDDR2_PHY_18_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (18*4));
	__raw_writel(DENALI_LPDDR2_PHY_19_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (19*4));
	__raw_writel(DENALI_LPDDR2_PHY_20_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (20*4));
	__raw_writel(DENALI_LPDDR2_PHY_21_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (21*4));
	__raw_writel(DENALI_LPDDR2_PHY_22_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (22*4));
	__raw_writel(DENALI_LPDDR2_PHY_23_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (23*4));
	__raw_writel(DENALI_LPDDR2_PHY_24_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (24*4));
	__raw_writel(DENALI_LPDDR2_PHY_25_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (25*4));
	__raw_writel(DENALI_LPDDR2_PHY_26_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (26*4));
	__raw_writel(DENALI_LPDDR2_PHY_27_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (27*4));
	__raw_writel(DENALI_LPDDR2_PHY_28_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (28*4));
	__raw_writel(DENALI_LPDDR2_PHY_29_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (29*4));
	__raw_writel(DENALI_LPDDR2_PHY_30_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (30*4));
	__raw_writel(DENALI_LPDDR2_PHY_31_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (31*4));
	__raw_writel(DENALI_LPDDR2_PHY_32_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (32*4));
	__raw_writel(DENALI_LPDDR2_PHY_33_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (33*4));
	__raw_writel(DENALI_LPDDR2_PHY_34_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (34*4));
	__raw_writel(DENALI_LPDDR2_PHY_35_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (35*4));
	__raw_writel(DENALI_LPDDR2_PHY_36_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (36*4));
	__raw_writel(DENALI_LPDDR2_PHY_37_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (37*4));
	__raw_writel(DENALI_LPDDR2_PHY_38_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (38*4));
	__raw_writel(DENALI_LPDDR2_PHY_39_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (39*4));
	__raw_writel(DENALI_LPDDR2_PHY_40_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (40*4));
	__raw_writel(DENALI_LPDDR2_PHY_41_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (41*4));
	__raw_writel(DENALI_LPDDR2_PHY_42_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (42*4));
	__raw_writel(DENALI_LPDDR2_PHY_43_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (43*4));
	__raw_writel(DENALI_LPDDR2_PHY_44_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (44*4));
	__raw_writel(DENALI_LPDDR2_PHY_45_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (45*4));
	__raw_writel(DENALI_LPDDR2_PHY_46_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (46*4));
	__raw_writel(DENALI_LPDDR2_PHY_47_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (47*4));
	__raw_writel(DENALI_LPDDR2_PHY_48_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (48*4));
	__raw_writel(DENALI_LPDDR2_PHY_49_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (49*4));
	__raw_writel(DENALI_LPDDR2_PHY_50_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (50*4));
	__raw_writel(DENALI_LPDDR2_PHY_51_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (51*4));
	__raw_writel(DENALI_LPDDR2_PHY_52_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (52*4));
	__raw_writel(DENALI_LPDDR2_PHY_53_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (53*4));
	__raw_writel(DENALI_LPDDR2_PHY_54_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (54*4));
	__raw_writel(DENALI_LPDDR2_PHY_55_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (55*4));
	__raw_writel(DENALI_LPDDR2_PHY_56_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (56*4));
	__raw_writel(DENALI_LPDDR2_PHY_57_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (57*4));
	__raw_writel(DENALI_LPDDR2_PHY_58_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (58*4));
	__raw_writel(DENALI_LPDDR2_PHY_59_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (59*4));
	__raw_writel(DENALI_LPDDR2_PHY_60_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (60*4));
	__raw_writel(DENALI_LPDDR2_PHY_61_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (61*4));
	__raw_writel(DENALI_LPDDR2_PHY_62_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (62*4));
	__raw_writel(DENALI_LPDDR2_PHY_63_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (63*4));
	__raw_writel(DENALI_LPDDR2_PHY_64_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (64*4));
	__raw_writel(DENALI_LPDDR2_PHY_65_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (65*4));
	__raw_writel(DENALI_LPDDR2_PHY_66_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (66*4));
	__raw_writel(DENALI_LPDDR2_PHY_67_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (67*4));
	__raw_writel(DENALI_LPDDR2_PHY_68_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (68*4));
	__raw_writel(DENALI_LPDDR2_PHY_69_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (69*4));
	__raw_writel(DENALI_LPDDR2_PHY_70_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (70*4));
	__raw_writel(DENALI_LPDDR2_PHY_71_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (71*4));
	__raw_writel(DENALI_LPDDR2_PHY_72_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (72*4));
	__raw_writel(DENALI_LPDDR2_PHY_73_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (73*4));
	__raw_writel(DENALI_LPDDR2_PHY_74_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (74*4));
	__raw_writel(DENALI_LPDDR2_PHY_75_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (75*4));
	__raw_writel(DENALI_LPDDR2_PHY_76_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (76*4));
	__raw_writel(DENALI_LPDDR2_PHY_77_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (77*4));
	__raw_writel(DENALI_LPDDR2_PHY_78_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (78*4));
	__raw_writel(DENALI_LPDDR2_PHY_79_533MHZ_CL8_BL8_DATA,  (uint32_t)p_ddr_phy->regs + (79*4));
#endif
}


void ddr_phy_config(E_DDR_CONFIG ddr_cfg)
{
	uint8_t mode, ds,odt,set_nzq,set_pzq,zqn_value,zqp_value,pdp,pup,pdn,pun;

	switch (ddr_cfg)
	{
		case LPDDR_CONFIG_100MHZ_CL3_BL8 :
				lpddr_phy_100mhz_cl3_bl8_config();
			break;
		case LPDDR_CONFIG_133MHZ_CL3_BL4 :
				lpddr_phy_133mhz_cl3_bl4_config();
			break;
		case LPDDR_CONFIG_133MHZ_CL3_BL8 :
				lpddr_phy_133mhz_cl3_bl8_config();
			break;
		case LPDDR_CONFIG_200MHZ_CL3_BL4 :
				lpddr_phy_200mhz_cl3_bl4_config();
			break;
		case LPDDR_CONFIG_200MHZ_CL3_BL8 :
				lpddr_phy_200mhz_cl3_bl8_config();
			break;
		case LPDDR2_CONFIG_133MHZ_CL3_BL4 :
				lpddr2_phy_133mhz_cl3_bl4_config();
			break;
		case LPDDR2_CONFIG_133MHZ_CL3_BL8 :
				lpddr2_phy_133mhz_cl3_bl8_config();
			break;
		case LPDDR2_CONFIG_200MHZ_CL3_BL4 :
				lpddr2_phy_200mhz_cl3_bl4_config();
			break;
		case LPDDR2_CONFIG_200MHZ_CL3_BL8 :
				lpddr2_phy_200mhz_cl3_bl8_config();
			break;
		case LPDDR2_CONFIG_400MHZ_CL6_BL4 :
				lpddr2_phy_400mhz_cl6_bl4_config();
			break;
		case LPDDR2_CONFIG_400MHZ_CL6_BL8 :
				lpddr2_phy_400mhz_cl6_bl8_config();
			break;
		case LPDDR2_CONFIG_533MHZ_CL8_BL4 :
				lpddr2_phy_533mhz_cl8_bl4_config();
			break;
		case LPDDR2_CONFIG_533MHZ_CL8_BL8 :
				lpddr2_phy_533mhz_cl8_bl8_config();
			break;
		default:
				ddr_info("ddr phy configre invalid!");
			break;
	}

	mode=0x2;
	ds=0x6;
	odt=0x0;
	set_nzq=0x1;
	set_pzq=0x1;
	zqn_value=0x0d;
	zqp_value=0x0b;
	pdp=0x0;
	pup=0x0;
	pdn=0x0;
	pun=0x0;
	__raw_writel(((pdn<<23)|(pdp<<22)|(pun<<21)|(pup<<20)|(mode<<18)|(ds<<15)|(odt<<12)|(set_nzq<<11)|(set_pzq<<10)|(zqn_value<<5)|zqp_value), (uint32_t)p_ddr_phy->regs + (87*4));
	__raw_writel((1<<19), (uint32_t)p_ddr_phy->regs + (88*4));
}

void ddr_extra_config(void)
{
	uint32_t reg_val;

	ddr_info("================================\n");
	ddr_info("|  EXTRA Initialize BEGIN !!!  |\n");
	ddr_info("================================\n");

#ifdef BSTLEN8
	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (28*4));
	reg_val = (reg_val & 0xfffffff8) | 0x3;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (28*4));
#endif

#ifdef BSTLEN4
	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (28*4));
	reg_val = (reg_val & 0xfffffff8) | 0x2;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (28*4));
#endif

#ifdef REG_DIMM
	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (29*4));
	reg_val |= 0x100;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (29*4));
#endif

#ifdef AP_ON
	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (26*4));
	reg_val |= 0x1000000;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (26*4));
#endif

#ifdef REDUC_ON
	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (100*4));
	reg_val |= 0x10000;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (100*4));
#endif

#ifndef DDR_PCPCS_EN
	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (47*4));
	reg_val |= 0x300;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (47*4));
#endif

	//DENALI_CTL_101_DATA 0x00000000
	// Force in-order accept to help guarantee this.
	//SET_PARAM(`IN_ORDER_ACCEPT_ADDR, `IN_ORDER_ACCEPT_OFFSET, `IN_ORDER_ACCEPT_WIDTH, 1'b1);
	__raw_writel(0x00000100, (uint32_t)p_ddr_ctrl->regs + (101*4));

	//DENALI_CTL_99_DATA 0x00010b01
	// Disable special R/W group switches so that R/W group placement is always at END of R/W group.
	//SET_PARAM(`DISABLE_RW_GROUP_W_BNK_CONFLICT_ADDR, `DISABLE_RW_GROUP_W_BNK_CONFLICT_OFFSET, `DISABLE_RW_GROUP_W_BNK_CONFLICT_WIDTH, 0);
	__raw_writel(0x00010b00, (uint32_t)p_ddr_ctrl->regs + (99*4));

	//DENALI_CTL_98_DATA 0x01010101
	// Disable rank grouping within a R/W group so that R/W group placement is always at END of R/W group.
	//SET_PARAM(`CS_SAME_EN_ADDR, `CS_SAME_EN_OFFSET, `CS_SAME_EN_WIDTH, 0);
	__raw_writel(0x01000101, (uint32_t)p_ddr_ctrl->regs + (98*4));

	ddr_info("================================\n");
	ddr_info("|  EXTRA Initialize END !!!  |\n");
	ddr_info("================================\n");
}

void ddr_scram_ctrl(E_DDR_SCRAM scram_mode)
{
	uint32_t reg_val = __raw_readl(MISC_BASE + DDR_CONTROL);

	reg_val &= 0xFFFFFFFE;
	reg_val |= scram_mode;
	__raw_writel(reg_val, MISC_BASE + DDR_CONTROL);	

	if (scram_mode == DDR_SCRAM_EN)
	{
		ddr_info("================================\n");
		ddr_info("|  scramble mode enabled !!! |\n");
		ddr_info("================================\n");
	}
	else
	{
		ddr_info("================================\n");
		ddr_info("|  scramble mode disabled !!! |\n");
		ddr_info("================================\n");	
	}	
}

void ddr_rng_data_load (void) 
{
	uint32_t data_tmp,key1,key2,key3,key4;
	unsigned long long start = get_timer(0);

	data_tmp = __raw_readl(DWC_TRNG_BASE+TRNG_STAT);
	while(((data_tmp>>31)&0x1) == 0x1){
		data_tmp = __raw_readl(DWC_TRNG_BASE+TRNG_STAT);
		if(get_timer(start) > DDR_TIMEOUT) {
			debug("ddr_rng_data_load check TRNG_STAT timeout\n");
			return ;
		}
	}

	int i = 0xfffff;
	__raw_writel(CMD_GEN_RANDOM, DWC_TRNG_BASE+TRNG_CTRL);
	while(i--);
	__raw_writel(0x00000001, MISC_BASE+DDR_SCRAM_UPDATE);
	key1 = __raw_readl(0x20006024);
	key2 = __raw_readl(0x20006028);
	key3 = __raw_readl(0x2000602c);
	key4 = __raw_readl(0x20006030);
	__raw_writel(0x00000000, MISC_BASE+DDR_SCRAM_UPDATE);
	//debug("scramble key %x_%x_%x_%x\n",key1,key2,key3,key4);

	return;
}


void ddr_ctrl_start(void)
{
	uint32_t reg_val;

	ddr_info("================================\n");
	ddr_info("|  START DDR CONTROLLER !!!    |\n");
	ddr_info("================================\n");

	reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (0*4));
	reg_val |= 0x1;
	__raw_writel(reg_val, (uint32_t)p_ddr_ctrl->regs + (0*4));
}

	//get the locked dll value
static uint8_t get_dll_val(uint32_t *dll_val_tmp,uint32_t slice_num){
    uint32_t phy_reg_dat;
	//  phy_reg_dat = __raw_readl(7*4 + (uint32_t)p_ddr_phy->regs);
    phy_reg_dat = __raw_readl((7+slice_num*16)*4 + (uint32_t)p_ddr_phy->regs);
    //check the dll status:
    if((phy_reg_dat & 0x7) != 0x1){
        if((phy_reg_dat & 0x1) != 0x1){
            return 1;
        }else if((phy_reg_dat & 0x6) != 0x00){
            return 2;
        }
    }
    //get the value
    *dll_val_tmp = (phy_reg_dat >>8) & 0xff;
    return 0;
}

static void update_slice_param(uint32_t phy_opt_val,uint8_t slice_num){
    uint32_t slice_reg_data;
    //update slice0~slice3
    if(slice_num<4){    
	    slice_reg_data = __raw_readl(DDR_PHY_BASE+(5+(16*slice_num))*4);
	    slice_reg_data &= 0xffff0000;
	    slice_reg_data |= ((phy_opt_val & 0xff) << 8);//write
	    slice_reg_data |= (phy_opt_val & 0xff );//read   
        __raw_writel(slice_reg_data, DDR_PHY_BASE+(5+16*slice_num)*4);	
    }else{
    //update slice4
        slice_reg_data = __raw_readl(DDR_PHY_BASE+(5+(16*4))*4);
	    slice_reg_data &= 0xffff00ff;
	    slice_reg_data |= ((phy_opt_val & 0xff) << 8);//write
        __raw_writel(slice_reg_data, DDR_PHY_BASE+(5+16*4)*4);
    }
}

//reset ddr controller
static void reset_ddr_ctrl(void)
{
    uint32_t reg_dat_tmp,i;
	
    reg_dat_tmp = __raw_readl(PRCM_BASE+0x18);
    reg_dat_tmp &= ~(0x01<<31);
    __raw_writel(reg_dat_tmp, PRCM_BASE+0x18);
    for(i=0;i<50;i++);
    reg_dat_tmp |= (0x01<<31);
    __raw_writel(reg_dat_tmp, PRCM_BASE+0x18);
    for(i=0;i<50;i++);
}

//configure IO
void reset_IO(void) {
	uint8_t mode,ds_ca,odt,set_nzq,set_pzq,zqn_value,zqp_value,pdp,pup,pdn,pun;
	uint8_t ds_s0,ds_s1,ds_s2,ds_s3;
	uint32_t val_tmp;
	ds_s0 = 0x6;
	ds_s1 = 0x6;
	ds_s2 = 0x6;
	ds_s3 = 0x6;
	ds_ca=0x6;

	mode=0x2;
	odt=0x0;
	set_nzq=0x1;
	set_pzq=0x1;
#ifdef ENABLE_AUTO_ZQ_INI
	zqn_value=0x10;
	zqp_value=0x10;
#else//manul setting
	zqn_value=0x0d;
	zqp_value=0x0d;
#endif
	pdp=0x0;
	pup=0x0;
	pdn=0x0;
	pun=0x0;
	__raw_writel(((pdn<<23)|(pdp<<22)|(pun<<21)|(pup<<20)|(mode<<18)|(ds_ca<<15)|(odt<<12)|(set_nzq<<11)|(set_pzq<<10)|(zqn_value<<5)|zqp_value), 87*4 + DDR_PHY_BASE);
    //set drive-strength of slice0~slice3
	val_tmp = __raw_readl(DDR_PHY_BASE+80*4);
	val_tmp &= 0xf000ffff;
	val_tmp |= (ds_s0<<16) | (ds_s1<<19) | (ds_s2<<22) | (ds_s3<<25);
	__raw_writel(val_tmp, 80*4 + DDR_PHY_BASE);
#ifdef ENABLE_AUTO_ZQ_INI
	__raw_writel((1<<19), 88*4 + DDR_PHY_BASE);
#endif

    return;
}



/*
ddr pll initialize
*/
static int ddr_init_pll(E_DDR_CONFIG ddr_cfg, E_DDR_SCRAM scram_mode, E_DDR1_CLK ddr_clk) {
	
	uint32_t reg_val, temp;
	unsigned long long start = 0;

	// initialize ddr clock
	ddr_set_clk(PRCM_BASE, ddr_clk);
	
	// reset ddr controller
	reset_ddr_ctrl();

  	// set ddr scramble mode
	ddr_scram_ctrl(scram_mode);

	// configure controller parameters
	ddr_ctrl_config(ddr_cfg);

	// configure phy parameters
	ddr_phy_config(ddr_cfg);

	//configure IO parameters
	reset_IO();

  	//other configuration
	ddr_extra_config();
	
	if (scram_mode == DDR_SCRAM_EN)
	{
		ddr_rng_data_load();
	}
	
	ddr_ctrl_start();

	// check if DDR initialization is done then clear the interrupt status bit
	start = get_timer(0);
	while (1)
	{
		reg_val = __raw_readl((uint32_t)p_ddr_ctrl->regs + (INT_STATUS_ADDR*4));

		if ((reg_val & 0x8) == 0x8)
		{
			__raw_writel(0x8, ((uint32_t)p_ddr_ctrl->regs + (INT_ACK_ADDR*4)));
			ddr_info("Clear DDR Interrupt\n");

			ddr_info("================================\n");
			ddr_info("|  DDR Controller Initialization DONE !!! |\n");
			ddr_info("================================\n");

			break;
		}
		if(get_timer(start) > DDR_TIMEOUT) {
			ddr_info("DDR Controller Initialization timeout\n");
			return -1;
		}
	}

	// check if zq initialization is done
	start = get_timer(0);
	while(1)
	{
		reg_val = __raw_readl((uint32_t)p_ddr_phy->regs + (88*4));
		if((reg_val>>31) == 0x1) 
		{
			break;
		}

		if(get_timer(start) > DDR_TIMEOUT) {
			ddr_info("zq initialization timeout\n");
			return -1;
		}
	}

	return 0;
}

static int ddr_scan_pattern(uint32_t scan_data, uint32_t scan_size)
{
	uint32_t i;
	uint8_t *p_addr = (uint8_t *)DDR_BASE;
	//uint8_t *p_addr = (uint8_t *)test_send_buff;

	// write data
	ddr_info("write done\n");

	for (i = 0; i< scan_size; i++)
	{
		p_addr[i] = (uint8_t)scan_data;

		//srand(get_ticks());
		//test_send_buff[i] = cb_rand() & 0xff;
	}

	// read data
	// compare data
	ddr_info("read and compare...\n");
	for (i = 0; i < scan_size; i++)
	{
		if (p_addr[i] != scan_data)
		{
			return 1;
		}
	}

	return 0;
}
  
uint8_t cal_opt_val(uint32_t phy400m_dll_val,uint32_t phy200m_dll_val,uint32_t *phy_opt_val,uint32_t slice_ac_flag)
{
    uint32_t phy_opt_val_tmp,phy_opt_val_tmp_ac;
    uint32_t mult_tmp0,mult_tmp1;
    uint8_t ret_val;
    ///////////////////////////calculate the opt value///////////////////////////
    //slice0~3:delay 1/4 period slice4:delay 7/16 period
    ///////////////a:400m dll value  b:200m dll value
    ///////////////1.(a>53) and  (b<=53)
    ///////////////     fail
    ///////////////2.(a<=53) and (b>53)
    ///////////////     opt_value=128a/b    opt_value_ac = (7/4)*opt_value
    ///////////////3.(a>53) and (b>53)
    ///////////////     if(3a<b+2*53) opt_value=128*53(b-a)/b(b+53-2a)
    ///////////////     if(3a>=b+2*53) opt_value=128*(3a-b)/b
    ///////////////     if(9a<=b+8*53) opt_value_ac=(7/4)*opt_value
    ///////////////     if(9a>b+8*53) opt_value_ac=32*(9a-b)/b
    ///////////////4.(a<=53) and (b<=53) 
    ///////////////     opt_value=64
    ///////////////     opt_value_ac=112
    ////////////////////////////////////////////////////////////////////////////
    if((phy400m_dll_val > 53) & (phy200m_dll_val <= 53)){
        return 5;
    }else if((phy400m_dll_val <= 53) & (phy200m_dll_val > 53)){
        mult_tmp0 = 128*phy400m_dll_val;
        mult_tmp1 = phy200m_dll_val; 
        phy_opt_val_tmp = (uint32_t)(mult_tmp0/mult_tmp1);
        phy_opt_val_tmp_ac = (uint32_t)((phy_opt_val_tmp*7)/4);
    }else if((phy400m_dll_val > 53) & (phy200m_dll_val > 53)){
        if((phy400m_dll_val*3)<(phy200m_dll_val+106)){
            mult_tmp0 = 6784*(phy200m_dll_val-phy400m_dll_val);
            mult_tmp1 = phy200m_dll_val * (phy200m_dll_val+53-2*phy400m_dll_val);
            phy_opt_val_tmp = (uint32_t)(mult_tmp0/mult_tmp1);
        }else{
            mult_tmp0 = 128*(3*phy400m_dll_val-phy200m_dll_val);
            mult_tmp1 = phy200m_dll_val; 
            phy_opt_val_tmp = (uint32_t)(mult_tmp0/mult_tmp1);
        }
        //slice4
        if((phy400m_dll_val*9)<=(phy200m_dll_val+424)){
            phy_opt_val_tmp_ac = (uint32_t)((phy_opt_val_tmp*7)/4);
        }else{
            mult_tmp0 = 32*(9*phy400m_dll_val-phy200m_dll_val);
            mult_tmp1 = phy200m_dll_val; 
            phy_opt_val_tmp_ac = (uint32_t)(mult_tmp0/mult_tmp1);
        }
    }else{
        phy_opt_val_tmp = 64;
        phy_opt_val_tmp_ac = 112;
    }

    if(slice_ac_flag){
        *phy_opt_val=phy_opt_val_tmp_ac ;
    }else{
        *phy_opt_val=phy_opt_val_tmp ;
    }
    return 0;
}

//ddr initialization
int ddr_init_0(E_DDR_CONFIG ddr_cfg, E_DDR_SCRAM scram_mode){
    uint32_t phy200m_dll_val0,phy200m_dll_val1,phy200m_dll_val2,phy200m_dll_val3,phy200m_dll_val4;
    uint32_t phy400m_dll_val0,phy400m_dll_val1,phy400m_dll_val2,phy400m_dll_val3,phy400m_dll_val4;
    uint32_t phy_opt_val0,phy_opt_val1,phy_opt_val2,phy_opt_val3,phy_opt_val4;
    uint8_t ret_val,data_temp;
	
    ddr_var_init(DDR_CTRL_BASE, DDR_PHY_BASE);
	
    //set PLL to 400M
    ddr_init_pll(ddr_cfg, scram_mode, DDR_CLK_400MHZ);

    //slice0
    ret_val = get_dll_val(&phy400m_dll_val0, 0);
    if(ret_val){
        return (ret_val);// 1 or 2
    }
    //slice1
    ret_val = get_dll_val(&phy400m_dll_val1, 1);
    if(ret_val){
        return (ret_val+0x10);// 11 or 12
    }
    //slice2
    ret_val = get_dll_val(&phy400m_dll_val2, 2);
    if(ret_val){
        return (ret_val+0x20);// 21 or 22
    }
    //slice3
    ret_val = get_dll_val(&phy400m_dll_val3, 3);
    if(ret_val){
        return (ret_val+0x30);// 31 or 32
    }
    //slice4
    ret_val = get_dll_val(&phy400m_dll_val4, 4);
    if(ret_val){
        return (ret_val+0x40);// 41 or 42
    }
    
    // set PLL to 200M
    ddr_init_pll(ddr_cfg, scram_mode, DDR_CLK_200MHZ);
    //slice0
    ret_val = get_dll_val(&phy200m_dll_val0,0);
    if(ret_val){
        return (ret_val+0x02);// 3 or 4
    }
    
    //slice1
    ret_val = get_dll_val(&phy200m_dll_val1, 1);
    if(ret_val){
        return (ret_val+0x12);// 13 or 14
    }
    
    //slice2
    ret_val = get_dll_val(&phy200m_dll_val2, 2);
    if(ret_val){
        return (ret_val+0x22);// 23 or 24
    }

    //slice3
    ret_val = get_dll_val(&phy200m_dll_val3, 3);
    if(ret_val){
        return (ret_val+0x32);// 33 or 34
    }

    //slice4
    ret_val = get_dll_val(&phy200m_dll_val4, 4);
    if(ret_val){
        return (ret_val+0x42);// 43 or 44
    }
  
    //cal slice;
    if(cal_opt_val(phy400m_dll_val0,phy200m_dll_val0,&phy_opt_val0,0)){
        return 0x50;
    }
    if(cal_opt_val(phy400m_dll_val1,phy200m_dll_val1,&phy_opt_val1,0)){
        return 0x51;
    }
    if(cal_opt_val(phy400m_dll_val2,phy200m_dll_val2,&phy_opt_val2,0)){
        return 0x52;
    }
    if(cal_opt_val(phy400m_dll_val3,phy200m_dll_val3,&phy_opt_val3,0)){
        return 0x53;
    }
    if(cal_opt_val(phy400m_dll_val4,phy200m_dll_val4,&phy_opt_val4,1)){
        return 0x54;
    }

    //update the parameters
    update_slice_param(phy_opt_val0,0);
    update_slice_param(phy_opt_val1,1);
    update_slice_param(phy_opt_val2,2);
    update_slice_param(phy_opt_val3,3);
    update_slice_param(phy_opt_val4,4);

    debug("\n a0:%x b0:%x ,phy_opt_val0:%x \n",phy400m_dll_val0,phy200m_dll_val0,phy_opt_val0);
    debug("\n a1:%x b1:%x ,phy_opt_val1:%x \n",phy400m_dll_val1,phy200m_dll_val1,phy_opt_val1);
    debug("\n a2:%x b2:%x ,phy_opt_val2:%x \n",phy400m_dll_val2,phy200m_dll_val2,phy_opt_val2);
    debug("\n a3:%x b3:%x ,phy_opt_val3:%x \n",phy400m_dll_val3,phy200m_dll_val3,phy_opt_val3);
    debug("\n a4:%x b4:%x ,phy_opt_val4:%x \n",phy400m_dll_val4,phy200m_dll_val4,phy_opt_val4);
	//Tigger dfi_ctrlupd_req signal
    data_temp = __raw_readl((uint32_t)p_ddr_ctrl->regs+102*4);
    data_temp |= 0x1;
	__raw_writel(data_temp, (uint32_t)p_ddr_ctrl->regs+102*4);

    return 0;
}

/*
ddr scan
*/
int ddr_scan(uint32_t scan_data, uint32_t scan_size)
{
	// Scan the memory
	if (0 == ddr_scan_pattern(scan_data, scan_size))
	{
		ddr_info("scan_size:0x%x, scan_data:0x%x compare pass\n", scan_size, scan_data);
	}
	else
	{
		ddr_info("scan_size:0x%x, scan_data:0x%x compare fail\n", scan_size, scan_data);
	}

	return 0;
}

