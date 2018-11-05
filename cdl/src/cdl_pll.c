/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/

#include "cdl_config.h"
#include "cdl_pll.h"
#include "std_common.h"
#include "cdl_sys.h"

#if defined(EVB_BOARD)
#define PLL_TIMEOUT (50)

struct brite_clk_pll pll;
struct brite_clk_pll *p_pll = &pll;

u32 root_xtal_24mhz_get_rate(struct clk *clk) {
	return 24000000;
}

u32 root_xtal_36mhz_get_rate(struct clk *clk) {
	return 36000000;
}
#define ROSC_FREQ	66000000
u32 root_rosc_100mhz_get_rate(struct clk *clk) {
	return ROSC_FREQ;
}

u32 root_rosc_25mhz_get_rate(struct clk *clk) {
	return ROSC_FREQ/4;
}

u32 clk_get_parent_rate(struct clk *clk) {
	if(clk->parent)
		return clk->parent->get_rate(clk->parent);
	else
		return clk->get_rate(clk);
}

u32 clk_get_rate(struct clk *clk) {
	return clk->get_rate(clk);
}

int clk_set_rate(struct clk *clk, u32 rate) {
	return clk->set_rate(clk, rate);
}

int clk_set_parent(struct clk *clk, struct clk *parent) {
	return clk->set_parent(clk, parent);
}

int clk_rate_change_cb(struct clk *clk) {
	struct clk *child;

	child = clk->child;
	while (child) {
		if(child->parent == clk && child->rate_change_cb)
			child->rate_change_cb(child);
		child = child->sibling;
	}
	return 0;
}


struct clk root_xtal_24mhz = {
	.name = "root_xtal_24mhz",
	.parent = NULL,
	.get_rate = root_xtal_24mhz_get_rate,
};

struct clk root_xtal_36mhz = {
	.name = "root_xtal_36mhz",
	.parent = NULL,
	.get_rate = root_xtal_36mhz_get_rate,
};

struct clk *root_xtal_act = &root_xtal_36mhz;

struct clk root_rosc_100mhz = {
	.name = "root_rosc_100mhz",
	.parent = NULL,
	.get_rate = root_rosc_100mhz_get_rate,
};

struct clk root_rosc_25mhz = {
	.name = "root_rosc_25mhz",
	.parent = NULL,
	.get_rate = root_rosc_25mhz_get_rate,
};

struct clk arm_pll = {
	.name = "arm_pll",
	.parent = &root_xtal_36mhz,
	.child = &arm_clk,
	.set_parent = brite_arm_pll_set_parent,
	.get_rate = brite_arm_pll_get_rate,
	.set_rate = brite_arm_pll_set_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_clk = {
	.name = "arm_clk",
	.parent = &arm_pll,
	.child = &arm_axi_clk,
	.set_parent = brite_set_arm_clk_parent,
	.get_rate = brite_get_arm_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_axi_clk = {
	.name = "arm_axi_clk",
	.parent = &arm_clk,
	.child = &arm_ahb_clk,
	.sibling = &arm_sdmmc_clk,
	.get_rate = brite_get_arm_axi_clk_rate,
	.set_rate = brite_set_arm_axi_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_ahb_clk = {
	.name = "arm_ahb_clk",
	.parent = &arm_axi_clk,
	.child = &arm_apb0_clk,
	.sibling = &arm_apb1_clk,
	.get_rate = brite_get_arm_ahb_clk_rate,
	.set_rate = brite_set_arm_ahb_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_apb0_clk = {
	.name = "arm_apb0_clk",
	.parent = &arm_ahb_clk,
	.get_rate = brite_get_arm_apb0_clk_rate,
	.set_rate = brite_set_arm_apb0_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_apb1_clk = {
	.name = "arm_apb1_clk",
	.parent = &arm_axi_clk,
	.sibling = &arm_rom_clk,
	.get_rate = brite_get_arm_apb1_clk_rate,
	.set_rate = brite_set_arm_apb1_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_rom_clk = {
	.name = "arm_rom_clk",
	.parent = &arm_axi_clk,
	.sibling = &arm_uart_clk,
	.get_rate = brite_get_arm_rom_clk_rate,
	.set_rate = brite_set_arm_rom_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_sdmmc_clk = {
	.name = "arm_sdmmc_clk",
	.parent = &arm_clk,
	.sibling = &arm_nfc_clk,
	.get_rate = brite_get_sdmmc_clk_rate,
	.set_rate = brite_set_sdmmc_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};

struct clk arm_nfc_clk = {
	.name = "arm_nfc_clk",
	.parent = &arm_clk,
	.get_rate = brite_get_nfc_clk_rate,
	.set_rate = brite_set_nfc_clk_rate,
	.rate_change_cb = clk_rate_change_cb,
};


struct clk arm_uart_clk = {
	.name = "arm_uart_clk",
	.parent = &arm_axi_clk,
	.get_rate = brite_get_uart_clk_rate_2,
	//.set_rate = brite_set_uart_clk_rate,
	//.rate_change_cb = console_pclk_change_cb,
};

void clk_register_callback(struct clk *clk, int (*cb)(struct clk *)) {
	clk->rate_change_cb = cb;
}


struct clk_name_map clk_name_map[] = {
		{ "xtal", &root_xtal_36mhz, CLK_IS_ROOT | CLK_IS_ARM_PLL_SRC },
		{ "rosc100", &root_rosc_100mhz, CLK_IS_ROOT | CLK_IS_ARM_CLK_SRC },
		{ "rosc25", &root_rosc_25mhz, CLK_IS_ARM_PLL_SRC },
		{ "arm_pll", &arm_pll, CLK_IS_PLL | CLK_IS_ARM_CLK_SRC },
		{ "arm_clk", &arm_clk,  },
		{ "axi", &arm_axi_clk, CLK_IS_BUS },
		{ "ahb", &arm_ahb_clk, CLK_IS_BUS },
		{ "apb0", &arm_apb0_clk, CLK_IS_BUS },
		{ "apb1", &arm_apb1_clk, CLK_IS_BUS },
		{ "rom", &arm_rom_clk, CLK_IS_BUS },
		{ "sdmmc", &arm_sdmmc_clk, CLK_IS_PERIPH },
		{ "nfc", &arm_nfc_clk, CLK_IS_PERIPH },
		{ "uart", &arm_uart_clk, /*CLK_IS_PERIPH*/ },
};

struct clk * clk_name_to_clk(const char *name, unsigned long filter) {
	struct clk_name_map *ent;
	int i=0;
	for (i=0; i<ARRAY_SIZE(clk_name_map); i++) {
		ent = &clk_name_map[i];
		if (!strcmp(name, ent->name) && (ent->flags & filter))
			return ent->clk;
	}
	return NULL;
}

void print_clk_name(unsigned long filter) {
	struct clk_name_map *ent;
	int i=0;
	for (i=0; i<ARRAY_SIZE(clk_name_map); i++) {
		ent = &clk_name_map[i];
		if (ent->flags & filter)
			info(" %s", ent->name);
	}
}


/*
 * ARM_PLL table
 */
static struct brite_pll_rate_table arm_pll_mclk_24mhz_tbl[] = {
	/* sorted in descending order */
	/* BRITE_PLL_RATE(rate, m, n, od) */
	BRITE_PLL_RATE(1500000000, 250, 2,  1),
	BRITE_PLL_RATE(1296000000, 162, 3,  0),
	BRITE_PLL_RATE(1200000000, 200, 2,  1),
	BRITE_PLL_RATE(1000000000, 250, 3,  1),
	BRITE_PLL_RATE(960000000,  160, 2,  1),
	BRITE_PLL_RATE(900000000,  150, 2,  1),
	BRITE_PLL_RATE(800000000,  200, 3,  1),
	BRITE_PLL_RATE(700000000,  175, 3,  1),
	BRITE_PLL_RATE(648000000,  108, 2,  1),
	BRITE_PLL_RATE(600000000,  100, 2,  1),
	BRITE_PLL_RATE(500000000,  125, 3,  1),
	BRITE_PLL_RATE(450000000,   75, 2,  1),
	BRITE_PLL_RATE(300000000,   75, 3,  1),
	BRITE_PLL_RATE(150000000,   50, 4,  1),
	{ /* sentinel */ }
};

static struct brite_pll_rate_table arm_pll_mclk_36mhz_tbl[] = {
	/* sorted in descending order */
	/* BRITE_PLL_RATE(rate, m, n, od) */
	BRITE_PLL_RATE(1500000000, 250, 3,  1),
	BRITE_PLL_RATE(1296000000, 144, 2,  1),
	BRITE_PLL_RATE(1200000000, 200, 3,  1),
	BRITE_PLL_RATE(1000000000, 250, 9,  0),
	BRITE_PLL_RATE(960000000,  160, 3,  1),
	BRITE_PLL_RATE(900000000,  100, 2,  1),
	BRITE_PLL_RATE(800000000,  200, 9,  0),
	BRITE_PLL_RATE(700000000,  175, 9,  0),
	BRITE_PLL_RATE(648000000,   72, 2,  1),
	BRITE_PLL_RATE(600000000,  100, 3,  1),
	BRITE_PLL_RATE(500000000,  250, 9,  1),
	BRITE_PLL_RATE(450000000,   75, 3,  1),
	BRITE_PLL_RATE(300000000,  150, 9,  1),
	BRITE_PLL_RATE(150000000,   75, 9,  1),
	{ /* sentinel */ }
};

static struct brite_pll_rate_table arm_pll_rosc_25mhz_tbl[] = {
	/* sorted in descending order */
	/* BRITE_PLL_RATE(rate, m, n, od) */
	BRITE_PLL_RATE(1500000000, 240, 2,  1),
	BRITE_PLL_RATE(1200000000, 192, 2,  1),
	BRITE_PLL_RATE(1000000000, 160, 2,  1),
	BRITE_PLL_RATE(960000000,  192, 5,  0),
	BRITE_PLL_RATE(900000000,  144, 2,  1),
	BRITE_PLL_RATE(800000000,  128, 2,  1),
	BRITE_PLL_RATE(700000000,  112, 2,  1),
	BRITE_PLL_RATE(675000000,  108, 2,  1),
	BRITE_PLL_RATE(650000000,   52, 2,  0),
	BRITE_PLL_RATE(625000000,  100, 2,  1),
	BRITE_PLL_RATE(600000000,   96, 2,  1),
	BRITE_PLL_RATE(500000000,  200, 5,  1),
	BRITE_PLL_RATE(450000000,   72, 2,  1),
	BRITE_PLL_RATE(300000000,  120, 5,  1),
	BRITE_PLL_RATE(150000000,   60, 5,  1),
	{ /* sentinel */ }
};

/*
 * DDR_PLL table
 */
static struct brite_pll_rate_table ddr_pll_mclk_24mhz_tbl[] = {
	/* sorted in descending order */
	/* BRITE_PLL_RATE(rate, m, n, od) */
	BRITE_PLL_RATE(1500000000, 250, 2,  1),
	BRITE_PLL_RATE(1296000000, 162, 3,  0),
	BRITE_PLL_RATE(1200000000, 200, 2,  1),
	BRITE_PLL_RATE(1000000000, 250, 3,  1),
	BRITE_PLL_RATE(960000000,  160, 2,  1),
	BRITE_PLL_RATE(900000000,  150, 2,  1),
	BRITE_PLL_RATE(800000000,  200, 3,  1),
	BRITE_PLL_RATE(700000000,  175, 3,  1),	
	BRITE_PLL_RATE(600000000,  100, 2,  1),
	BRITE_PLL_RATE(500000000,  125, 3,  1),
	BRITE_PLL_RATE(400000000,  100, 3,  1),
	BRITE_PLL_RATE(200000000,  200, 6,  2),
	BRITE_PLL_RATE(133000000,  133, 6,  2),
	BRITE_PLL_RATE(100000000,   25, 3,  1),
	{ /* sentinel */ }
};

static struct brite_pll_rate_table ddr_pll_mclk_36mhz_tbl[] = {
	/* sorted in descending order */
	/* BRITE_PLL_RATE(rate, m, n, od) */
	BRITE_PLL_RATE(1500000000, 250, 3,  1),
	BRITE_PLL_RATE(1296000000, 144, 2,  1),
	BRITE_PLL_RATE(1200000000, 200, 3,  1),
	BRITE_PLL_RATE(1000000000, 250, 9,  0),
	BRITE_PLL_RATE(960000000,  160, 3,  1),
	BRITE_PLL_RATE(900000000,  100, 2,  1),
	BRITE_PLL_RATE(800000000,  200, 9,  0),
	BRITE_PLL_RATE(700000000,  175, 9,  0),
	BRITE_PLL_RATE(600000000,  100, 3,  1),
	BRITE_PLL_RATE(500000000,  250, 9,  1),
	BRITE_PLL_RATE(400000000,  200, 9,  1),
	BRITE_PLL_RATE(200000000,  200, 9,  2),
	BRITE_PLL_RATE(133000000,  133, 9,  2),
	BRITE_PLL_RATE(100000000,   50, 9,  1),
	{ /* sentinel */ }
};

static struct brite_pll_rate_table ddr_pll_rosc_25mhz_tbl[] = {
	/* sorted in descending order */
	/* BRITE_PLL_RATE(rate, m, n, od) */
	BRITE_PLL_RATE(1500000000, 240, 2,  1),
	BRITE_PLL_RATE(1200000000, 192, 2,  1),
	BRITE_PLL_RATE(1000000000, 160, 2,  1),
	BRITE_PLL_RATE(960000000,  192, 5,  0),
	BRITE_PLL_RATE(900000000,  144, 2,  1),
	BRITE_PLL_RATE(800000000,  128, 2,  1),
	BRITE_PLL_RATE(700000000,  112, 2,  1),
	BRITE_PLL_RATE(600000000,   96, 2,  1),
	BRITE_PLL_RATE(500000000,  200, 5,  1),
	BRITE_PLL_RATE(400000000,   64, 2,  1),
	BRITE_PLL_RATE(200000000,   32, 2,  1),
	BRITE_PLL_RATE(100000000,   40, 5,  1),
	{ /* sentinel */ }
};

static inline u32 pll_read(struct brite_clk_pll *priv, u32 offset)
{
	return readl((u32)priv->regs + offset);
}

static inline void pll_write(struct brite_clk_pll *priv, u32 offset, u32 val)
{
	writel(val, (u32)priv->regs + offset);
}

/*
 * static functions
 */
static void brite_get_pll_rate_table(unsigned int xin_type, u32 pll_type)
{
	u32 boot_status, mclk_type;

	if (!p_pll)
		return;

	if (pll_type == ARM_PLL_TYPE) {
		if (xin_type == PLL_SRC_FROM_ROSC) {
			p_pll->rate_count = ARRAY_SIZE(arm_pll_rosc_25mhz_tbl);
			p_pll->rate_table = arm_pll_rosc_25mhz_tbl;
		} else {
			boot_status = pll_read(p_pll, BOOT_STATUS_REG);
			mclk_type = (boot_status >> BOOT_STATUS_MCLK_SHIFT) & BOOT_STATUS_MCLK_MASK;

			if (mclk_type == MCLK_24M) {
				p_pll->rate_count = ARRAY_SIZE(arm_pll_mclk_24mhz_tbl);
				p_pll->rate_table = arm_pll_mclk_24mhz_tbl;
			} else {
				p_pll->rate_count = ARRAY_SIZE(arm_pll_mclk_36mhz_tbl);
				p_pll->rate_table = arm_pll_mclk_36mhz_tbl;
			}
		}
	} else if (pll_type == DDR_PLL_TYPE) {
		if (xin_type == PLL_SRC_FROM_ROSC) {
			p_pll->rate_count = ARRAY_SIZE(ddr_pll_rosc_25mhz_tbl);
			p_pll->rate_table = ddr_pll_rosc_25mhz_tbl;
		} else {
			boot_status = pll_read(p_pll, BOOT_STATUS_REG);
			mclk_type = (boot_status >> BOOT_STATUS_MCLK_SHIFT) & BOOT_STATUS_MCLK_MASK;
		
			if (mclk_type == MCLK_24M) {
				p_pll->rate_count = ARRAY_SIZE(ddr_pll_mclk_24mhz_tbl);
				p_pll->rate_table = ddr_pll_mclk_24mhz_tbl;
			} else {
				p_pll->rate_count = ARRAY_SIZE(arm_pll_mclk_36mhz_tbl);
				p_pll->rate_table = ddr_pll_mclk_36mhz_tbl;
			}
		}
	}
}

static const struct brite_pll_rate_table *brite_get_pll_settings(
			u32 clk_out)
{
	int i;

	if (!p_pll)
		return NULL;

	for (i = 0; i < p_pll->rate_count; i++) {
		if (clk_out == p_pll->rate_table[i].rate)
			return &p_pll->rate_table[i];
	}

	return NULL;
}

static void brite_arm_pll_set_bypass(int bypassed)
{
	u32 pll_cfg;

	if (!p_pll)
		return;

	pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);

	pll_cfg &= ~(PLL_CFG_BP_MASK << PLL_CFG_BP_SHIFT);
	if (bypassed) {
		/*pll bypassed before modifying the pll configure*/
		pll_cfg |= (PLL_CFG_BP_MASK << PLL_CFG_BP_SHIFT);
	}
	pll_write(p_pll, ARM_PLL_CFG_REG, pll_cfg);
}

static void brite_ddr_pll_set_bypass(int bypassed)
{
	u32 pll_cfg;

	if (!p_pll)
		return;

	pll_cfg = pll_read(p_pll, DDR_PLL_CFG_REG);

	pll_cfg &= ~(PLL_CFG_BP_MASK << PLL_CFG_BP_SHIFT);
	if (bypassed) {
		/*pll bypassed before modifying the pll configure*/
		pll_cfg |= (PLL_CFG_BP_MASK << PLL_CFG_BP_SHIFT);
	}
	pll_write(p_pll, DDR_PLL_CFG_REG, pll_cfg);
}

static u32 brite_pll_round_rate(u32 drate)
{
	const struct brite_pll_rate_table *rate_table = p_pll->rate_table;
	int i;

	if (!p_pll || !rate_table)
		return ULONG_MAX;

	/* Assumming rate_table is in descending order */
	for (i = 0; i < p_pll->rate_count; i++) {
		if (drate >= rate_table[i].rate)
			return rate_table[i].rate;
	}

	/* return minimum supported value */
	return rate_table[i - 1].rate;
}



/*ARM PLL APIs*/
u32 brite_arm_pll_get_rate(struct clk *clk)
{
	u32 ndiv, mdiv, oddiv, xin_type, bypassed, pll_cfg;
	u32 boot_status, mclk_type;
	u32 xin;
	u64 clk_out;

	if (!p_pll)
		return ULONG_MAX;

	pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);
	bypassed = (pll_cfg >> PLL_CFG_BP_SHIFT) & PLL_CFG_BP_MASK;
	xin_type = (pll_cfg >> PLL_CFG_SRC_SHIFT) & PLL_CFG_SRC_MASK;
	ndiv = (pll_cfg >> PLL_CFG_NDIV_SHIFT) & PLL_CFG_NDIV_MASK;
	mdiv = (pll_cfg >> PLL_CFG_MDIV_SHIFT) & PLL_CFG_MDIV_MASK;
	oddiv = (pll_cfg >> PLL_CFG_ODDIV_SHIFT) & PLL_CFG_ODDIV_MASK;
	
	xin = clk_get_parent_rate(clk);

	if (bypassed) {
		clk_out = xin;
	} else {
		clk_out = (((xin / 1000) * mdiv) / (ndiv << oddiv)) * 1000;

		if (clk_out > ULONG_MAX)
			return ULONG_MAX;
	}
	return (u32)clk_out;
}

u32 brite_arm_pll_get_actual_rate(u32 clk_out, unsigned int xin_type)
{
	const struct brite_pll_rate_table *rate;
	u32 pll_cfg, xin_type_old;
	u32 clk_actual;

	if (!p_pll)
		return ULONG_MAX;

	/* Get xin_type rate table */
	brite_get_pll_rate_table(xin_type, ARM_PLL_TYPE);
	clk_actual = brite_pll_round_rate(clk_out);
	return clk_actual;
}

u32 brite_arm_pll_get_current_xin_type(void)
{
	const struct brite_pll_rate_table *rate;
	u32 pll_cfg, xin_type;
	u32 clk_actual;

	if (!p_pll)
		return ULONG_MAX;

	pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);
	xin_type = (pll_cfg >> PLL_CFG_SRC_SHIFT) & PLL_CFG_SRC_MASK;
	return xin_type;
}

int brite_arm_pll_set_parent(struct clk *clk, struct clk *parent) {
	u32 pll_cfg, xin_type_old;
	u32 xin_type;

	if (!parent)
		return -EINVAL;
	if (clk->parent == parent)
		return 0;

	clk->parent = parent;

	brite_arm_pll_set_bypass(1);

	pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);

	if (!strcmp(parent->name, root_xtal_36mhz.name))
		xin_type = PLL_SRC_FROM_MCLK;
	else if (!strcmp(parent->name, root_rosc_25mhz.name))
		xin_type = PLL_SRC_FROM_ROSC;
	else
		return -EINVAL;

	/* Change PLL SOURCE */
	pll_cfg &= ~(PLL_CFG_SRC_MASK << PLL_CFG_SRC_SHIFT);
	pll_cfg |= (xin_type & PLL_CFG_SRC_MASK) << PLL_CFG_SRC_SHIFT;
	pll_write(p_pll, ARM_PLL_CFG_REG, pll_cfg);

	// 2.4. Check bit31 of ARM_PLL_CFG until the PLLs lock are ready;
	//		HW make sure the lock bit must be set in max 500us
	do {
		pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);
	} while (!(pll_cfg & (PLL_CFG_LOCK_STAT_MASK
				<< PLL_CFG_LOCK_STAT_SHIFT)));

	brite_arm_pll_set_bypass(0);

	if(clk->rate_change_cb)
		clk->rate_change_cb(clk);
}

int brite_arm_pll_set_rate(struct clk *clk, u32 clk_out)
{
	const struct brite_pll_rate_table *rate_cfg;
	u32 pll_cfg, xin_type;
	u32 clk_actual;
	u32 arm_clk_cfg;

	if (!p_pll)
		return -EINVAL;

	/* Get xin_type rate table */
	xin_type = brite_arm_pll_get_current_xin_type();
	brite_get_pll_rate_table(xin_type, ARM_PLL_TYPE);
	clk_actual = brite_pll_round_rate(clk_out);
	if (clk_actual != clk_out) {
		if (clk_actual == ULONG_MAX)
			return -EINVAL;
	}

	if (clk_out == clk->get_rate(clk))
		return 0;

	/* Get required rate settings from table */
	rate_cfg = brite_get_pll_settings(clk_actual);
	if (!rate_cfg) {
		return -EINVAL;
	}
	/* step 1: switch clk to rocs */
	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	pll_write(p_pll, ARM_CLK_CFG_REG, arm_clk_cfg & ~(1 << CLK_CFG_ARM_CLK_SRC_SHIFT));

	/*step 2.  start modify the pll configure register */
	/*set pll bypassed*/
	brite_arm_pll_set_bypass(1);

	pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);

	/* Change PLL N&M&OD values */
	pll_cfg &= ~((PLL_CFG_NDIV_MASK << PLL_CFG_NDIV_SHIFT) |
			(PLL_CFG_MDIV_MASK << PLL_CFG_MDIV_SHIFT) |
			(PLL_CFG_ODDIV_MASK << PLL_CFG_ODDIV_SHIFT));
	pll_cfg |= (rate_cfg->ndiv << PLL_CFG_NDIV_SHIFT) |
			(rate_cfg->mdiv << PLL_CFG_MDIV_SHIFT) |
			(rate_cfg->oddiv << PLL_CFG_ODDIV_SHIFT);
	pll_write(p_pll, ARM_PLL_CFG_REG, pll_cfg);

	/* wait_lock_time: HW make sure the lock bit must be set in max 500us */
	do {
		pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);
	} while (!(pll_cfg & (PLL_CFG_LOCK_STAT_MASK
				<< PLL_CFG_LOCK_STAT_SHIFT)));
	/*clear pll bypassed, clock from pll*/
	brite_arm_pll_set_bypass(0);

	/* step 3: switch clk back */
	pll_write(p_pll, ARM_CLK_CFG_REG, arm_clk_cfg);

	if(clk->rate_change_cb)
		clk->rate_change_cb(clk);

	return 0;
}


/*DDR PLL APIs*/
u32 brite_ddr_pll_get_rate(void)
{
	u32 ndiv, mdiv, oddiv, xin_type, bypassed, pll_cfg;
	u32 boot_status, mclk_type;
	u32 xin;
	u64 clk_out;

	if (!p_pll)
		return ULONG_MAX;

	pll_cfg = pll_read(p_pll, DDR_PLL_CFG_REG);
	bypassed = (pll_cfg >> PLL_CFG_BP_SHIFT) & PLL_CFG_BP_MASK;
	xin_type = (pll_cfg >> PLL_CFG_SRC_SHIFT) & PLL_CFG_SRC_MASK;
	ndiv = (pll_cfg >> PLL_CFG_NDIV_SHIFT) & PLL_CFG_NDIV_MASK;
	mdiv = (pll_cfg >> PLL_CFG_MDIV_SHIFT) & PLL_CFG_MDIV_MASK;
	oddiv = (pll_cfg >> PLL_CFG_ODDIV_SHIFT) & PLL_CFG_ODDIV_MASK;
	
	if (xin_type == PLL_SRC_FROM_ROSC) {
		xin = 25000000;
	} else {
		boot_status = pll_read(p_pll, BOOT_STATUS_REG);
		mclk_type = (boot_status >> BOOT_STATUS_MCLK_SHIFT) & BOOT_STATUS_MCLK_MASK;

		if (mclk_type == MCLK_24M) {
			xin = 24000000;
		} else {
			xin = 36000000;
		}
	}
	if (bypassed) {
		clk_out = xin;
	} else {
		clk_out = (((xin / 1000) * mdiv) / (ndiv << oddiv)) * 1000;

		if (clk_out > ULONG_MAX)
			return ULONG_MAX;
	}
	return (u32)clk_out;
}

/*xin_type:0-PLL_SRC_FROM_MCLK, 1-PLL_SRC_FROM_ROSC*/
int brite_ddr_pll_set_rate(u32 clk_out, unsigned int xin_type)
{
	const struct brite_pll_rate_table *rate;
	u32 pll_cfg, xin_type_old;
	u32 clk_actual;

	if (!p_pll)
		return -EINVAL;

	/* Get xin_type rate table */
	brite_get_pll_rate_table(xin_type, DDR_PLL_TYPE);
	clk_actual = brite_pll_round_rate(clk_out);
	if (clk_actual != clk_out) {
		if (clk_actual == ULONG_MAX)
			return -EINVAL;
	}

	/* Get required rate settings from table */
	rate = brite_get_pll_settings(clk_actual);
	if (!rate) {
		return -EINVAL;
	}

	/* start modify the pll configure register */
	/*pll bypassed, clock from input directyl*/
	brite_ddr_pll_set_bypass(1);

	pll_cfg = pll_read(p_pll, DDR_PLL_CFG_REG);
	xin_type_old = (pll_cfg >> PLL_CFG_SRC_SHIFT) & PLL_CFG_SRC_MASK;

	/* Change PLL SOURCE */
	if (xin_type_old != xin_type) {
		pll_cfg &= ~(PLL_CFG_SRC_MASK << PLL_CFG_SRC_SHIFT);
		if (xin_type == PLL_SRC_FROM_ROSC) {
			pll_cfg |= (PLL_SRC_FROM_ROSC << PLL_CFG_SRC_SHIFT);
		}
	}

	/* Change PLL N&M&OD values */
	pll_cfg &= ~((PLL_CFG_NDIV_MASK << PLL_CFG_NDIV_SHIFT) |
			(PLL_CFG_MDIV_MASK << PLL_CFG_MDIV_SHIFT) |
			(PLL_CFG_ODDIV_MASK << PLL_CFG_ODDIV_SHIFT));
	pll_cfg |= (rate->ndiv << PLL_CFG_NDIV_SHIFT) |
			(rate->mdiv << PLL_CFG_MDIV_SHIFT) |
			(rate->oddiv << PLL_CFG_ODDIV_SHIFT);
	pll_write(p_pll, DDR_PLL_CFG_REG, pll_cfg);

	/* wait_lock_time: HW make sure the lock bit must be set in max 500us */
	do {
		pll_cfg = pll_read(p_pll, DDR_PLL_CFG_REG);
	} while (!(pll_cfg & (PLL_CFG_LOCK_STAT_MASK
				<< PLL_CFG_LOCK_STAT_SHIFT)));
	/*pll bypassed, clock from pll*/
	brite_ddr_pll_set_bypass(0);
	return 0;
}


/*ARM CLK/AXI/AHB/APB0/APB1/SDMMC/NFC/UART functions*/

/*arm_clk = rocs_100m or arm_clk = arm_pll */ 

#if 0
int brite_arm_clk_src_select(unsigned int src_type)
{
	u32 arm_clk_cfg, source;

	if (!p_pll)
		return -EINVAL;
	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	source = (arm_clk_cfg >> CLK_CFG_ARM_CLK_SRC_SHIFT) & CLK_CFG_ARM_CLK_SRC_MASK;

	/* Change arm clk SOURCE */
	if (source != src_type) {
		arm_clk_cfg &= ~(CLK_CFG_ARM_CLK_SRC_MASK << CLK_CFG_ARM_CLK_SRC_SHIFT);
		if (src_type == ARM_CLK_SRC_FROM_ARM_PLL) {
			arm_clk_cfg |= (ARM_CLK_SRC_FROM_ARM_PLL << CLK_CFG_ARM_CLK_SRC_SHIFT);
		}
		pll_write(p_pll, ARM_CLK_CFG_REG, arm_clk_cfg);
	}
	return 0;
}
#endif
u32 brite_arm_clk_get_current_xin_type(void)
{
	const struct brite_pll_rate_table *rate;
	u32 arm_clk_cfg, xin_type;
	u32 clk_actual;

	if (!p_pll)
		return ULONG_MAX;

	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	xin_type = (arm_clk_cfg >> CLK_CFG_ARM_CLK_SRC_SHIFT) & CLK_CFG_ARM_CLK_SRC_MASK;

	return xin_type;
}

int brite_set_arm_clk_parent(struct clk *clk, struct clk *parent) {
	struct clk *child;
	u32 arm_clk_cfg, source;

	if (!parent)
		return -EINVAL;
	if (clk->parent == parent)
		return 0;

	clk->parent = parent;

	if (!strcmp(parent->name, arm_pll.name))
		source = ARM_CLK_SRC_FROM_ARM_PLL;
	else if (!strcmp(parent->name, root_rosc_100mhz.name))
		source = ARM_CLK_SRC_FROM_ROSC_100M;
	else
		return -EINVAL;

	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	arm_clk_cfg &= ~(CLK_CFG_ARM_CLK_SRC_MASK << CLK_CFG_ARM_CLK_SRC_SHIFT);
	arm_clk_cfg |= source << CLK_CFG_ARM_CLK_SRC_SHIFT;
	pll_write(p_pll, ARM_CLK_CFG_REG, arm_clk_cfg);

	if(clk->rate_change_cb)
		clk->rate_change_cb(clk);

	return 0;
}

u32 brite_get_arm_clk_rate(struct clk *clk)
{
	return clk_get_parent_rate(clk);
}

/***************************************************
 *                                                 *
 *      ARM Clock child get/set rate functions     *
 *                                                 *
 ***************************************************/

u32 brite_get_arm_clk_child_div(u32 mask, u32 shift)
{
	u32 arm_clk_cfg;
	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	return (arm_clk_cfg >> shift) & mask;
}

int brite_set_arm_clk_child_div(u32 div, u32 mask, u32 shift)
{
	u32 arm_clk_cfg;

	if (!p_pll || (div & mask) != div)
		return -EINVAL;

	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	arm_clk_cfg &= ~(mask << shift);
	arm_clk_cfg |= (div & mask) << shift;
	pll_write(p_pll, ARM_CLK_CFG_REG, arm_clk_cfg);
	return 0;
}

u32 brite_get_arm_clk_child_rate(struct clk *clk, u32 mask, u32 shift)
{
	u32 parent_clk_rate, clk_rate;
	u32 act_div;

	parent_clk_rate = clk_get_parent_rate(clk);
	if (parent_clk_rate == ULONG_MAX)
		return ULONG_MAX;

	act_div = brite_get_arm_clk_child_div(mask, shift);
	clk_rate = parent_clk_rate / (act_div + 1);
	return clk_rate;
}


int brite_set_arm_clk_child_rate(struct clk *clk, u32 clk_out, u32 mask, u32 shift)
{
	u32 div, act_div;
	u32 parent_clk_rate;
	int ret = 0;

	if (clk->get_rate(clk) == clk_out)
		return 0;

	if (brite_get_arm_clk_child_rate(clk, mask, shift) == clk_out)
		return 0;

	parent_clk_rate = clk_get_parent_rate(clk);
	if (parent_clk_rate == ULONG_MAX) {
		ret = -EINVAL;
		goto end;
	}
	div = DIV_ROUND_HALF_UP(parent_clk_rate, clk_out);
	act_div = div - 1;
	if (div == 0 || (mask & act_div) != act_div) {
		ret = -EINVAL;
		goto end;
	}
	ret = brite_set_arm_clk_child_div(act_div, mask, shift);
	if (ret)
		goto end;

	if(clk->rate_change_cb)
		clk->rate_change_cb(clk);

end:
	return ret;
}

/*arm_axi_clk=arm_clk/(bit[1:0]+1) */
/*default div: BUS_CLK_DIV_2*/

u32 brite_get_arm_axi_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_AXIDIV_MASK, CLK_CFG_AXIDIV_SHIFT);
}

int brite_set_arm_axi_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_AXIDIV_MASK, CLK_CFG_AXIDIV_SHIFT);
}

/*arm_ahb_clk=arm_axi_clk/(bit[3:2]+1)*/
/*default div: BUS_CLK_DIV_2*/

u32 brite_get_arm_ahb_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_AHBDIV_MASK, CLK_CFG_AHBDIV_SHIFT);
}

int brite_set_arm_ahb_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_AHBDIV_MASK, CLK_CFG_AHBDIV_SHIFT);
}

/*arm_apb0_clk=arm_ahb_clk/(bit[5:4]+1) */
/*default div: BUS_CLK_DIV_1*/


u32 brite_get_arm_apb0_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_APB0DIV_MASK, CLK_CFG_APB0DIV_SHIFT);
}

int brite_set_arm_apb0_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_APB0DIV_MASK, CLK_CFG_APB0DIV_SHIFT);
}

/*arm_apb1_clk=arm_axi_clk/(bit[7:6]+1)  */
/*default div: BUS_CLK_DIV_4*/

u32 brite_get_arm_apb1_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_APB1DIV_MASK, CLK_CFG_APB1DIV_SHIFT);
}

int brite_set_arm_apb1_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_APB1DIV_MASK, CLK_CFG_APB1DIV_SHIFT);
}

/*arm_rom_clk=arm_axi_clk/(bit[27:26]+1)  */
/*default div: BUS_CLK_DIV_2*/

u32 brite_get_arm_rom_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_ROMDIV_MASK, CLK_CFG_ROMDIV_SHIFT);
}

int brite_set_arm_rom_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_ROMDIV_MASK, CLK_CFG_ROMDIV_SHIFT);
}

/*sdmmc_clk=arm_clk/(bit[11:8]+1)   */
/*default div: BUS_CLK_DIV_12*/

u32 brite_get_sdmmc_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_SDMMCDIV_MASK, CLK_CFG_SDMMCDIV_SHIFT);
}

int brite_set_sdmmc_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_SDMMCDIV_MASK, CLK_CFG_SDMMCDIV_SHIFT);
}

/*nfc_clk=arm_clk/(bit[15:12]+1)  */
/*default div: BUS_CLK_DIV_6*/

u32 brite_get_nfc_clk_rate(struct clk *clk)
{
	return brite_get_arm_clk_child_rate(clk, CLK_CFG_NFCDIV_MASK, CLK_CFG_NFCDIV_SHIFT);
}

int brite_set_nfc_clk_rate(struct clk *clk, u32 clk_out)
{
	return brite_set_arm_clk_child_rate(clk, clk_out, CLK_CFG_NFCDIV_MASK, CLK_CFG_NFCDIV_SHIFT);
}

/*uart_clk= arm_axi_clk/(bit[18:16]+bit[23:19]/32)  */
/*default div: 6.25 = 6 + 8/32*/
u32 brite_get_uart_clk_div(void)
{
	return brite_get_arm_clk_child_div(CLK_CFG_UARTDIV_MASK, CLK_CFG_UARTDIV_SHIFT);
}

int brite_set_uart_clk_div(u32 div)
{
	int ret;
	unsigned long i = 0;
	ret =  brite_set_arm_clk_child_div(div, CLK_CFG_UARTDIV_MASK, CLK_CFG_UARTDIV_SHIFT);
	brite_uart_clk_div_block_reset(1);
	while(i++ < 10000)
		__asm__ volatile("dmb");
	brite_uart_clk_div_block_reset(0);
	return ret;
}

u32 brite_get_uart_clk_rate_2(struct clk *clk)
{
	u32 uart_div, div0, div1, arm_clk_cfg;
	u32 parent_clk_rate, uart_clk_rate;

	if (!p_pll)
		return ULONG_MAX;


	parent_clk_rate = clk_get_parent_rate(clk);
	if (parent_clk_rate == ULONG_MAX)
		return ULONG_MAX;

	uart_div = brite_get_arm_clk_child_div(CLK_CFG_UARTDIV_MASK, CLK_CFG_UARTDIV_SHIFT);
	div0 = (uart_div & 0x7);
	div1 = ((uart_div >> 0x3) & 0x1F);
	
	uart_clk_rate = parent_clk_rate / (div0 * 32 + div1) * 32;
	//uart_clk_rate = axi_clk_rate / ((uart_div & 0x7) + ((uart_div >> 0x3) & 0x1F) / 32);
	return uart_clk_rate;
}

u32 brite_get_uart_clk_rate(void) {
	return brite_get_uart_clk_rate_2(&arm_uart_clk);
}

/* arm_uart_clk_divider block reset*/
void brite_uart_clk_div_block_reset(int reset)
{
	u32 arm_clk_cfg;

	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);

	arm_clk_cfg &= ~(CLK_CFG_UARTDIV_RST_MASK << CLK_CFG_UARTDIV_RST_SHIFT);
	if (reset) {
		arm_clk_cfg |= (CLK_CFG_UARTDIV_RST_MASK << CLK_CFG_UARTDIV_RST_SHIFT);
	}
	pll_write(p_pll, ARM_CLK_CFG_REG, arm_clk_cfg);
}

int brite_set_all_clock_default(void)
{
	int ret = 0;

	/*1. set ARM and DDR pll default*/
	/*switch arm clk source*/
	if (clk_set_parent(&arm_pll, root_xtal_act))
		goto error;
	/*arm_clk: 600M*/
	if (clk_set_rate(&arm_pll, ARM_PLL_DEFAULT_CLK))
		goto error;
#if 0
	if (brite_ddr_pll_set_rate(DDR_PLL, PLL_SRC_FROM_MCLK))
		goto error;
#endif

	if(clk_set_parent(&arm_clk, &arm_pll))
		goto error;
	/*2. set all clock default*/

	/*arm_axi_clk: 300M*/
	if (clk_set_rate(&arm_axi_clk, ARM_AXI_DEFAULT_CLK))
		goto error;
	/*arm_ahb_clk: 150M*/
	if (clk_set_rate(&arm_ahb_clk, ARM_AHB_DEFAULT_CLK))
		goto error;
	/*arm_apb0_clk: 150M*/
	if (clk_set_rate(&arm_apb0_clk, ARM_APB0_DEFAULT_CLK))
		goto error;
	/*arm_apb1_clk: 75M*/
	if (clk_set_rate(&arm_apb1_clk, ARM_APB1_DEFAULT_CLK))
		goto error;
	/*arm_rom_clk: 150M*/
	if (clk_set_rate(&arm_rom_clk, ARM_ROM_DEFAULT_CLK))
		goto error;
	/*sdmmc_clk: 50M * 2*/
	if (clk_set_rate(&arm_sdmmc_clk, ARM_SDMMC_DEFAULT_CLK))
		goto error;
	/*nfc_clk: 100M*/
	if (clk_set_rate(&arm_nfc_clk, ARM_NFC_DEFAULT_CLK))
		goto error;
	/*uart_clk: 60M*/
	if (brite_set_uart_clk_div(0x1D))
		goto error;

	goto end;

error:
	ret = -EINVAL;
end:
	return ret;
}

void brite_get_all_clock_rate(void)
{
	u32 arm_clk_cfg, arm_pll_cfg, ddr_pll_cfg;
	u32 arm_pll_rate, ddr_pll_rate;
	u32 arm_clk_rate, axi_clk_rate, ahb_clk_rate;
	u32 apb0_clk_rate, apb1_clk_rate, rom_clk_rate;
	u32 sdmmc_clk_rate, nfc_clk_rate, uart_clk_rate;

	arm_pll_cfg = pll_read(p_pll, ARM_PLL_CFG_REG);
	arm_clk_cfg = pll_read(p_pll, ARM_CLK_CFG_REG);
	ddr_pll_cfg = pll_read(p_pll, DDR_PLL_CFG_REG);
	info("\n\nClock regs: arm_pll/arm_clk/ddr_pll %x/%x/%x\n", 
			arm_pll_cfg, arm_clk_cfg, ddr_pll_cfg);

	/*check arm_pll and ddr_pll */
	arm_pll_rate = clk_get_rate(&arm_pll);
	ddr_pll_rate = brite_ddr_pll_get_rate();
	info("PLL rate: ARM %u from %s, DDR %u\n",
			arm_pll_rate, arm_pll.parent->name, ddr_pll_rate);


	/*check arm_clk rate*/
	arm_clk_rate = clk_get_rate(&arm_clk);
	/*check arm_axi_clk */
	axi_clk_rate = clk_get_rate(&arm_axi_clk);
	/*check arm_ahb_clk*/
	ahb_clk_rate = clk_get_rate(&arm_ahb_clk);
	/*check arm_apb0_clk*/
	apb0_clk_rate = clk_get_rate(&arm_apb0_clk);
	/*check arm_apb1_clk*/
	apb1_clk_rate = clk_get_rate(&arm_apb1_clk);
	/*check arm_rom_clk*/
	rom_clk_rate = clk_get_rate(&arm_rom_clk);
	info("arm_clk_rate: %u from %s\n", arm_clk_rate, arm_clk.parent->name);
	info("axi_clk_rate: %u\n", axi_clk_rate);
	info("ahb_clk_rate: %u\n", ahb_clk_rate);
	info("apb0_clk_rate: %u\n", apb0_clk_rate);
	info("apb1_clk_rate: %u\n", apb1_clk_rate);
	info("rom_clk_rate: %u\n", rom_clk_rate);

	/*check sdmmc_clk*/
	sdmmc_clk_rate = clk_get_rate(&arm_sdmmc_clk);
	/*check nfc_clk*/
	nfc_clk_rate = clk_get_rate(&arm_nfc_clk);
	/*check uart_clk*/
	uart_clk_rate = clk_get_rate(&arm_uart_clk);
	info("sdmmc_clk_rate: %u\n", sdmmc_clk_rate);
	info("nfc_clk_rate: %u\n", nfc_clk_rate);
	info("uart_clk_rate: %u\n", uart_clk_rate);
}

/*ARM and DDR PLL common APIs*/
void brite_pll_init(u32 base)
{
	u32 boot_status;
	u32 mclk_type;
	u32 arm_pll_xin_type;
	u32 xin_type;

	memset(p_pll, 0, sizeof(struct brite_clk_pll));

	p_pll->regs = (void*)base;

	boot_status = pll_read(p_pll, BOOT_STATUS_REG);
	mclk_type = (boot_status >> BOOT_STATUS_MCLK_SHIFT) & BOOT_STATUS_MCLK_MASK;

	root_xtal_act = (mclk_type == MCLK_24M) ? &root_xtal_24mhz : &root_xtal_36mhz;
	clk_name_map[0].clk = root_xtal_act;

	arm_pll_xin_type = brite_arm_pll_get_current_xin_type();
	clk_set_parent(&arm_pll, (arm_pll_xin_type == PLL_SRC_FROM_MCLK) ? root_xtal_act : &root_rosc_25mhz);

	xin_type = brite_arm_clk_get_current_xin_type();
	clk_set_parent(&arm_clk, (xin_type == ARM_CLK_SRC_FROM_ROSC_100M) ? &root_rosc_100mhz : &arm_pll);

	/* Get default rate table for ARM pll*/
	brite_get_pll_rate_table(arm_pll_xin_type, ARM_PLL_TYPE);
}
#else
struct clk arm_uart_clk;
struct clk arm_apb1_clk;

int g_min_err_rate_of_100k;

u32 clk_get_parent_rate(struct clk *clk) {
	return -1;
}

u32 clk_get_rate(struct clk *clk) {
	return -1;
}

int clk_set_rate(struct clk *clk, u32 rate) {
	return -1;
}

int clk_set_parent(struct clk *clk, struct clk *parent) {
	return -1;
}

int uart_tune_clk_for_baud(struct clk *clk, unsigned long baud) {
	return -1;
}

int console_pclk_change_cb(struct clk *clk) {
	return -1;
}

int system_timer_pclk_change_cb(struct clk *clk) {
	return -1;
}

void clk_register_callback(struct clk *clk, int (*cb)(struct clk *)) {
}

u32 brite_get_uart_clk_div(void) {
	return -1;
}

void brite_get_all_clock_rate(void) {

}

#endif

