/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_TAMP_H__
#define __CDL_TAMP_H__

#include "cdl_types.h"

/*register offset*/
/*tamp detector regs*/
#define TAMP_CONFIG_REG			0x000
#define TAMP_CTRL_REG  			0x004
#define TAMP_COUNT_REG  		0x008
#define TAMP_COUNT_CLR_REG 		0x00C
#define TAMP_IRQ_STA_REG 		0x010
#define TAMP_IRQ_CLR_REG  		0x014
#define TAMP_IRQ_EN_REG  		0x018
#define TAMP_PEND_WR_REG 		0x01C

/*time regs*/
#define TAMP_T1_SEC_REG			0x040
#define TAMP_T1_MIN_REG			0x044
#define TAMP_T1_HOUR_REG  		0x048
#define TAMP_T1_DAY_REG 		0x04C
#define TAMP_T1_MONTH_REG 		0x050
#define TAMP_T1_YEAR_REG  		0x054

#define TAMP_T2_SEC_REG			0x060
#define TAMP_T2_MIN_REG			0x064
#define TAMP_T2_HOUR_REG  		0x068
#define TAMP_T2_DAY_REG 		0x06C
#define TAMP_T2_MONTH_REG 		0x070
#define TAMP_T2_YEAR_REG  		0x074

#define TAMP_T3_SEC_REG			0x080
#define TAMP_T3_MIN_REG			0x084
#define TAMP_T3_HOUR_REG  		0x088
#define TAMP_T3_DAY_REG 		0x08C
#define TAMP_T3_MONTH_REG 		0x090
#define TAMP_T3_YEAR_REG  		0x094

/*TAMP_CONFIG bit*/
#define CONFIG_TAMP_DEB_TIME_OFF	0
#define CONFIG_TAMP_DEB_TIME_MSK	(0x3 << 0)
#define DEB_TIME_NO					0x0  
#define DEB_TIME_2MS				0x1
#define DEB_TIME_4MS				0x2
#define DEB_TIME_6MS				0x3

#define CONFIG_TAMP_DET_MODE_OFF	2
#define CONFIG_TAMP_DET_MODE_MSK	(0x7 << 2)
#define DET_MODE_RISING_EDGE		0x0  
#define DET_MODE_FALLING_EDGE		0x1
#define DET_MODE_BOTH_EDGE			0x2
#define DET_MODE_POSITIVE_PULSE		0x3
#define DET_MODE_NEGATIVE_PULSE		0x4

/*bit define*/
/*TAMP_CTRL bit*/
#define  TAMP_CTRL_EN			(1 << 0)

/*TAMP_COUNT bit*/
#define TAMP_COUNT_MASK			(0x3F << 0)
#define TAMP_COUNT_CLR			(1 << 0)

/*IRQ bit*/
#define TAMP_IRQ_STA			(1 << 0)
#define TAMP_IRQ_CLR			(1 << 0)
#define TAMP_IRQ_EN				(1 << 0)

/*TAMP_PEND_WR bit*/
#define TAMP_PEND_WR			(1 << 0)

/*TAMP_Tx_SEC bit*/
#define TAMP_SEC_Tx				(0x7F << 0)

/*TAMP_Tx_MIN bit*/
#define TAMP_MIN_Tx				(0x7F << 0)
 
/*TAMP_Tx_HOUR bit*/
#define Tx_HR12_24				(1 << 7)
#define Tx_H20_PA				(1 << 5)
#define Tx_HOUR19				(0x1F << 0)
#define Tx_HOUR20				(0x3F << 0)

/*TAMP_Tx_DAY bit*/
#define TAMP_DAY_Tx				(0x3F << 0)

/*TAMP_Tx_MONTH bit*/
#define TAMP_MONTH_Tx			(0x1F << 0)

/*TAMP_Tx_YEAR bit*/
#define TAMP_YEAR_Tx			(0xFF << 0)

#define TAMP_MODE_HOUR12 0
#define TAMP_MODE_HOUR24 1

#define TAMP_HOUR_AM	0
#define TAMP_HOUR_PM	1

struct brite_tamp {
	void* regs;
};

struct tamp_time {
	u32 tm_sec;
	u32 tm_min;
	u32 tm_hour;
	u32 tm_day;
	u32 tm_mon;
	u32 tm_year;
};

/*APIs*/
/*configure*/
void brite_tamp_init(u32 base);
void brite_tamp_enable(u8 en);
void brite_tamp_set_config(u8 det_mode, u8 deb_time);
u8 brite_tamp_get_event_count(void);
void brite_tamp_clear_event_count(u8 val);

/*irq*/
void brite_tamp_enable_irq(u8 mask);
void brite_tamp_disable_irq(u8 mask);
void brite_tamp_clear_irq(u8 val);
u8 brite_tamp_get_irq_status(void);

/*get time*/
u8 brite_tamp_get_hour(u8 id);
void brite_tamp_gettime(u8 id, struct tamp_time *tm);
#endif

