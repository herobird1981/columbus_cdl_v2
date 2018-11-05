/*--------------------------------------------------------------------------------------------------*/
/*                                                                                                											*/
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                							*/
/*                                                                                                											*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef __CDL_RTC_H__
#define __CDL_RTC_H__

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// rtc register layout 
#define RTC_100THSEC		0x000
#define RTC_SEC  			0x004
#define RTC_MIN  			0x008
#define RTC_HOUR 			0x00C
#define RTC_WEEK 			0x010
#define RTC_DAY  			0x014
#define RTC_CEN_MONTH  		0x018
#define RTC_YEAR 			0x01C

// alarm register layout
#define RTC_ALM1_100THSEC		0x030
#define RTC_ALM1_SEC			0x034
#define RTC_ALM1_MIN  			0x038
#define RTC_ALM1_HOUR 			0x03C
#define RTC_ALM1_WEEK 			0x040
#define RTC_ALM1_DAY  			0x044
#define RTC_ALM1_MONTH			0x048
#define RTC_ALM1_YEAR 			0x04C
#define RTC_ALM1_MASK 			0x050

#define ALM_MASK_A1100THSEC 	(1 << 0)
#define ALM_MASK_A1MSEC 		(1 << 1)
#define ALM_MASK_A1MMIN 		(1 << 2)
#define ALM_MASK_A1MHOUR 		(1 << 3)
#define ALM_MASK_A1MWEEK		(1 << 4)
#define ALM_MASK_A1MDAY 		(1 << 5)
#define ALM_MASK_A1MMON 		(1 << 6)
#define ALM_MASK_A1MYEAR 		(1 << 7)

#define RTC_ALM2_SET			0x054
#define RTC_ALM_EN				0x058
#define RTC_IRQ_RAW				0x05c
#define RTC_IRQ_MASK			0x060
#define RTC_IRQ_STA				0x064
#define RTC_IRQ_CLR				0x068
#define RTC_ALM1_EN		(1<<0)
#define RTC_ALM2_EN		(1<<1)

// rtc trim register layout
#define RTC_TRIM_REG  			0x080	// RTC Trimming Register
#define RTC_TRIM_EN_REG			0x084	// RTC Trimming Enable Register
#define RTC_TRIM_MODE_REG  		0x088	// RTC Trimming Mode Register.
#define RTC_TRIM_VLD_EN_REG  	0x08C	// RTC Trimming Validation Enable Register
#define RTC_TEST_REG  			0x090
#define RTC_TEST_MODE_REG  		0x094

// rtc ctl register layout
#define RTC_PEND_WR			0x0A0
#define RTC_WRSTA_A7	  	0x0A4
#define RTC_WRSTA_EN		(0x1)
#define RTC_WRSTP_A7	  	0x0A8
#define RTC_WRSTP_EN		(0x1)
#define RTC_RDSTA_A7 	 	0x0AC
#define RTC_RDSTA_EN		(0x1)
#define RTC_RDSTP_A7	  	0x0B0
#define RTC_RDSTP_EN		(0x1)
#define RTC_RDSTA_DSP1  	0x0B4
#define RTC_RDSTA_DSP2  	0x0B8
#define RTC_RDSTP_DSP1  	0x0BC
#define RTC_RDSTP_DSP2  	0x0C0
#define RTC_INTCLR_DSP1 	0x0C4
#define RTC_INTCLR_DSP2 	0x0C8

// offset: 0x00C, RTC hour Register
typedef enum
{
	RTC_HOUR19_SHIFT  	= 0,	// R/W
								
	
	RTC_H20_PA_SHIFT 	= 5,	// R/W 
								// 		There are hour counters. The value of HOUR19 is encoded with BCD (Binary Coded Decimal). Time format depends on clock system. 
								// 		In 12-hour clock mode, H20_PA means AM or PM. 
								// 		In 24-hour clock mode, H20_PA means the 20's place. Refer 'Hour format' section to detail.
								// 		In 12-hour clock mode, when [H20_PA, HOUR19] is counts up from [1,11] (11PM) to [0,12] (12AM), the DAY counter is incremented.
								// 		In 24-hour clock mode, when [H20_PA, HOUR19] is counts up from [1,3] (23H) to [0,0] (0H), the DAY counter is incremented.
									
	RTC_HOUR12_24_SHIFT = 7,	// R/W, Setting of the clock system.
								// 		0 : 12-hour clock mode
								// 		1 : 24-hour clock mode.
} E_RTC_HOUR_SHIFT;

#define HOUR12_24			(1<<RTC_HOUR12_24_SHIFT)
#define H20_PA				(1<<RTC_H20_PA_SHIFT)
#define HOUR19				0x1F

typedef enum {
	RTC_MODE_HOUR12 = 0,
	RTC_MODE_HOUR24 = 1
} RTC_MODE_HOUR12_24;

// offset: 0x084, RTC Trimming Enable Register
typedef enum
{
	RTC_TRIM_EN_SHIFT  	= 0,	// bit[0], R/W, Trimming enable control
								// 		0x0 : Disable
								// 		0x1 : Enable
								
	
	RTC_TRIM_REVD_SHIFT = 1,	// bit[7:1], R/W, reserved
} E_RTC_TRIM_EN_SHIFT;

typedef enum {
	RTC_TRIM_DIS 	= 0,
	RTC_TRIM_EN 	= 1
} E_RTC_TRIM_EN;

#define RTC_TRIM_EN_MASK		(1<<RTC_TRIM_EN_SHIFT)
#define RTC_TRIM_EN_VAL(val)	((val)<<RTC_TRIM_EN_SHIFT)


// offset: 0x088, RTC Trimming Mode Register.
typedef enum
{
	RTC_TRIM_MODE_SHIFT  	= 0,	// bit [1:0], R/W, This register controls the timing of clock adjustment.
									// 		0x0 : EVERY_60SEC
									// 		0x1 : EVERY_30SEC
									// 		0x2: EVERY_15SEC
									// 		0x3: EVERY_6SEC
	

	RTC_TRIM_VLD_MODE_SHIFT  = 2,	// bit[3:2], R/W, Setting for VLD_ON_O for trimming function
									// 		0x0: VLD_ON_O holds for 60 sec
									// 		0x1: VLD_ON_O holds for 30 sec
									// 		0x2: VLD_ON_O holds for 15 sec
									// 		0x3: VLD_ON_O holds for 6 sec
	
	RTC_TRIM_MODE_REVD_SHIFT = 4,	// bit[7:4], R/W, reserved
} E_RTC_TRIM_MODE_SHIFT;

typedef enum {
	RTC_TRIM_60S = 0,
	RTC_TRIM_30S = 1,
	RTC_TRIM_15S = 2,
	RTC_TRIM_06S = 3
} E_RTC_TRIM_MODE;

#define RTC_TRIM_MODE_MASK		(3<<RTC_TRIM_MODE_SHIFT)
#define RTC_TRIM_MODE_VAL(val)	((val)<<RTC_TRIM_MODE_SHIFT)

typedef enum {
	RTC_TRIM_VLD_60S = 0,
	RTC_TRIM_VLD_30S = 1,
	RTC_TRIM_VLD_15S = 2,
	RTC_TRIM_VLD_06S = 3
} E_RTC_TRIM_VLD_MODE;

#define RTC_TRIM_VLD_MODE_MASK		(3<<RTC_TRIM_VLD_MODE_SHIFT)
#define RTC_TRIM_VLD_MODE_VAL(val)	((val)<<RTC_TRIM_VLD_MODE_SHIFT)

// offset: 0x08C, RTC Trimming Validation Enable Register
typedef enum
{
	RTC_TRIM_VLD_EN_SHIFT  	= 0,	// bit[0], R/W, Validation enable control
									// 		0x0 : Disable
									// 		0x1 : Enable, VLD_ON pulse asserted periodically
								
	RTC_TRIM_VLD_REVD_SHIFT = 1,	// bit[7:1], R/W, reserved
} E_RTC_TRIM_VLD_EN_SHIFT;

typedef enum {
	RTC_TRIM_VLD_DIS 	= 0,
	RTC_TRIM_VLD_EN 	= 1
} E_RTC_TRIM_VLD_EN;

#define RTC_TRIM_VLD_EN_MASK		(1<<RTC_TRIM_VLD_EN_SHIFT)
#define RTC_TRIM_VLD_EN_VAL(val)	((val)<<RTC_TRIM_VLD_EN_SHIFT)


// offset: 0x0A0, RTC AHB write pending status register
typedef enum
{
	RTC_TRIM_PEND_WR_SHIFT  	= 0,	// bit[0], R, AHB write pending status
										// 		0x0 : No AHB write pending
										// 		0x1 : AHB Write pending
								
	RTC_TRIM_PEND_WR_REVD_SHIFT = 1,	// bit[7:1], R/W, reserved
} E_RTC_PEND_WR_SHIFT;

#define RTC_TRIM_PEND_WR_MASK		(1<<RTC_TRIM_PEND_WR_SHIFT)

typedef enum
{
	RTC_REQUEST_ARM  = 1,
	RTC_REQUEST_PLC = 2,
	RTC_REQUEST_RF = 4
}E_RTC_REQUEST_SRC;


/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
struct rtc_alarm {
	u8 sec;
	u8 min;
	u8 hour;
	u8 week;
	u8 day;
	u8 month;
	u8 year;
	u8 mask;
	u8 alm2;
};

struct brite_rtc {
	void* regs;
	RTC_MODE_HOUR12_24 mode;
	unsigned char cent;
	int sec_100thsec_99;
};

struct rtc_time {
	int tm_100thsec;
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

/*
 * This data structure is inspired by the EFI (v0.92) wakeup
 * alarm API.
 */
struct rtc_wkalrm {
	unsigned char enabled;  /* 0 = alarm disabled, 1 = alarm enabled */
	unsigned char pending;  /* 0 = alarm not pending, 1 = alarm pending */
	int int_vector;
	struct rtc_time time;   /* time the alarm is set to */
};

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/


/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
//APIs
int brite_rtc_init(unsigned long base,RTC_MODE_HOUR12_24 mode);
void brite_rtc_gettime(struct rtc_time *tm);
void brite_rtc_settime(struct rtc_time *tm);
unsigned char brite_rtc_get_int_status(void);
unsigned char brite_rtc_get_raw_int_status(void);
void brite_rtc_set_raw_int_status(void);
void brite_rtc_set_int_mask(unsigned char mask);
void brite_rtc_clear_int(unsigned char val);

int rtc_calculation_week(struct rtc_time *tm);
#ifdef TAPEOUT_V2
int check_time(int year, int month, int day, int hour, int min, int sec, int thsec100);
#else
int check_time(int year, int month, int day, int hour, int min, int sec);
#endif
unsigned long mktime(const unsigned int year, const unsigned int mon,
			    const unsigned int day, const unsigned int hour,
			    const unsigned int min, const unsigned int sec);

int rtc_month_days(unsigned int month, unsigned int year);
int rtc_year_days(unsigned int day, unsigned int month, unsigned int year);
void rtc_time_to_tm(unsigned long time, struct rtc_time *tm);
int rtc_tm_to_time(struct rtc_time *tm, unsigned long *time);
void brite_rtc_get_trigger_mode_multi(void);


/*alarm1*/
void alarm_enable(unsigned char id,int en);
void brite_rtc_get_trigger_mode(void);
void brite_rtc_getalarm( struct rtc_wkalrm *alrm,unsigned char *hour_mode);
void brite_rtc_setalarm(struct rtc_wkalrm *alrm,int flag);
void alarm_set_mask(u8 mask);
void alarm_clear_mask(u8 mask);


// trim
int8_t rtc_read_trim(void);

void rtc_write_trim(int8_t val);

void rtc_trim_ctrl(E_RTC_TRIM_EN ctrl);

void rtc_trim_vld_ctrl(E_RTC_TRIM_VLD_EN ctrl);

uint8_t rtc_read_trim_mode(void);

void rtc_write_trim_mode(E_RTC_TRIM_MODE trim_mode, E_RTC_TRIM_VLD_MODE vld_mode);

#endif

