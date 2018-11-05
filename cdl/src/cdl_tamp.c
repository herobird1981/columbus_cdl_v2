/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_tamp.h"

#define TAMP_TIMEOUT (1000) //ms

struct brite_tamp tamp;
struct brite_tamp *p_tamp = &tamp;
int init_tamp = 0;

static inline u8 brite_tamp_readb(struct brite_tamp *priv,
	u32 offset)
{
	u8 val = __raw_readb((u32)priv->regs + offset);
	return val;
}

static inline void brite_tamp_writeb(struct brite_tamp *priv, u32 offset,
	u8 val)
{
	unsigned long long start = get_timer(0);

	while ((__raw_readb((u32)priv->regs + TAMP_PEND_WR_REG)) & TAMP_PEND_WR) {
		if(get_timer(start) > TAMP_TIMEOUT) {
			info("brite_tamp_writeb check TAMP_PEND_WR_REG timeout first\n");
			return ;
		}
	}
	__raw_writeb(val, (u32)priv->regs + offset);
	
	start = get_timer(0);
	while ((__raw_readb((u32)priv->regs + TAMP_PEND_WR_REG)) & TAMP_PEND_WR) {
		if(get_timer(start) > TAMP_TIMEOUT) {
			info("brite_tamp_writeb check TAMP_PEND_WR_REG timeout last\n");
			break;
		}
	}
}

static inline u32 bcd2bin(u8 val)
{
	return (((val)&0x0f) + ((val) >> 4) * 10);
}

static u8 tamp_read_sec(u8 id)
{
	u8 val;

	switch (id) {
	case 1:
		val = (brite_tamp_readb(p_tamp, TAMP_T1_SEC_REG) & TAMP_SEC_Tx);
		break;
	case 2:
		val = (brite_tamp_readb(p_tamp, TAMP_T2_SEC_REG) & TAMP_SEC_Tx);
		break;
	case 3:
		val = (brite_tamp_readb(p_tamp, TAMP_T3_SEC_REG) & TAMP_SEC_Tx);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}

static u8 tamp_read_min(u8 id)
{
	u8 val;

	switch (id) {
	case 1:
		val = (brite_tamp_readb(p_tamp, TAMP_T1_MIN_REG) & TAMP_MIN_Tx);
		break;
	case 2:
		val = (brite_tamp_readb(p_tamp, TAMP_T2_MIN_REG) & TAMP_MIN_Tx);
		break;
	case 3:
		val = (brite_tamp_readb(p_tamp, TAMP_T3_MIN_REG) & TAMP_MIN_Tx);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}

static u8 tamp_read_hour(u8 id)
{
	u8 val;

	val = brite_tamp_get_hour(id);
	if (val & Tx_HR12_24) {
		val &= Tx_HOUR20;
	} else {
		val &= Tx_HOUR19;
	}
	return val;
}

static u8 tamp_read_day(u8 id)
{
	u8 val;

	switch (id) {
	case 1:
		val = (brite_tamp_readb(p_tamp, TAMP_T1_DAY_REG) & TAMP_DAY_Tx);
		break;
	case 2:
		val = (brite_tamp_readb(p_tamp, TAMP_T2_DAY_REG) & TAMP_DAY_Tx);
		break;
	case 3:
		val = (brite_tamp_readb(p_tamp, TAMP_T3_DAY_REG) & TAMP_DAY_Tx);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}

static u8 tamp_read_month(u8 id)
{
	u8 val;

	switch (id) {
	case 1:
		val = (brite_tamp_readb(p_tamp, TAMP_T1_MONTH_REG) & TAMP_MONTH_Tx);
		break;
	case 2:
		val = (brite_tamp_readb(p_tamp, TAMP_T2_MONTH_REG) & TAMP_MONTH_Tx);
		break;
	case 3:
		val = (brite_tamp_readb(p_tamp, TAMP_T3_MONTH_REG) & TAMP_MONTH_Tx);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}

static u8 tamp_read_year(u8 id)
{
	u8 val;

	switch (id) {
	case 1:
		val = (brite_tamp_readb(p_tamp, TAMP_T1_YEAR_REG) & TAMP_YEAR_Tx);
		break;
	case 2:
		val = (brite_tamp_readb(p_tamp, TAMP_T2_YEAR_REG) & TAMP_YEAR_Tx);
		break;
	case 3:
		val = (brite_tamp_readb(p_tamp, TAMP_T3_YEAR_REG) & TAMP_YEAR_Tx);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}


void brite_tamp_enable(u8 en)
{
	u8 ctrl = 0;
	ctrl = brite_tamp_readb(p_tamp, TAMP_CTRL_REG);
	ctrl &= ~TAMP_CTRL_EN;
	if (en)
		ctrl |= TAMP_CTRL_EN;
	brite_tamp_writeb(p_tamp, TAMP_CTRL_REG, ctrl);
}

void brite_tamp_set_config(u8 det_mode, u8 deb_time)
{
	u8 config = 0;
	config = brite_tamp_readb(p_tamp, TAMP_CONFIG_REG);
	config &= ~(CONFIG_TAMP_DEB_TIME_MSK | CONFIG_TAMP_DET_MODE_MSK);
	config |= ((deb_time << CONFIG_TAMP_DEB_TIME_OFF) | (u8)(det_mode << CONFIG_TAMP_DET_MODE_OFF));
	brite_tamp_writeb(p_tamp, TAMP_CONFIG_REG, config);
}

u8 brite_tamp_get_event_count(void)
{
	return (brite_tamp_readb(p_tamp, TAMP_COUNT_REG) & TAMP_COUNT_MASK);
}

void brite_tamp_clear_event_count(u8 val)
{
	brite_tamp_writeb(p_tamp, TAMP_COUNT_CLR_REG, val);
}

void brite_tamp_enable_irq(u8 mask)
{
	u8 irq_en = 0;

	irq_en = brite_tamp_readb(p_tamp, TAMP_IRQ_EN_REG);
	irq_en |= mask;
	brite_tamp_writeb(p_tamp, TAMP_IRQ_EN_REG, irq_en);
}

void brite_tamp_disable_irq(u8 mask)
{
	u8 irq_en = 0;

	irq_en = brite_tamp_readb(p_tamp, TAMP_IRQ_EN_REG);
	irq_en &= ~(mask);
	brite_tamp_writeb(p_tamp, TAMP_IRQ_EN_REG, irq_en);
}

u8 brite_tamp_get_irq_status(void)
{
	return brite_tamp_readb(p_tamp, TAMP_IRQ_STA_REG);
}

void brite_tamp_clear_irq(u8 val)
{
	brite_tamp_writeb(p_tamp, TAMP_IRQ_CLR_REG, val);
}

void brite_tamp_init(u32 base)
{
	if(init_tamp == 0)
	{
		memset(p_tamp, 0, sizeof(struct brite_tamp));

		p_tamp->regs = (void*)base;
#if 0
		/*disable and clear int status*/
		brite_tamp_disable_irq(TAMP_IRQ_EN);
		brite_tamp_clear_irq(TAMP_IRQ_CLR);

		/*disable tampering detector function */
		brite_tamp_enable(0);
		/*clear event count*/
		brite_tamp_clear_event_count(TAMP_COUNT_CLR);
#endif
		init_tamp = 1;
	}
}

u8 brite_tamp_get_hour(u8 id)
{
	u8 val;

	switch (id) {
	case 1:
		val = brite_tamp_readb(p_tamp, TAMP_T1_HOUR_REG);
		break;
	case 2:
		val = brite_tamp_readb(p_tamp, TAMP_T2_HOUR_REG);
		break;
	case 3:
		val = brite_tamp_readb(p_tamp, TAMP_T3_HOUR_REG);
		break;
	default:
		val = 0;
		break;
	}
	return val;
}

/*get tamp time */
void brite_tamp_gettime(u8 id, struct tamp_time *tm)
{
	u8 sec;
	u8 minute;
	u8 hour;
	u8 day;
	u8 month;
	u8 year;

	day = tamp_read_day(id);
	month = tamp_read_month(id);
	year = tamp_read_year(id);

	hour = tamp_read_hour(id);
	minute = tamp_read_min(id);
	sec = tamp_read_sec(id);

	/*bcd convert*/
	tm->tm_sec = bcd2bin(sec);
	tm->tm_min = bcd2bin(minute);
	tm->tm_hour = bcd2bin(hour);
	tm->tm_day = bcd2bin(day);
	tm->tm_mon = bcd2bin(month);
	tm->tm_year = bcd2bin(year);
	tm->tm_year += 2000;
}

