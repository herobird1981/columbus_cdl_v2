/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_rtc.h"
#include "cdl_types.h"
#include "cdl_sys.h"

#define RTC_TIMEOUT (1000) //ms

struct brite_rtc rtc;
struct brite_rtc* p_rtc = &rtc;
int init_rtc = 0;

// DAY : Range of this counter is 
// 01 to 31 : Jan, Mar, May, Jul, Aug, Oct, Dec 
// 01 to 30 : Apr, Jun, Sep, Nov 
// 01 to 29 : Feb in a leap year 
// 01 to 28 : Feb in a common year 
static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const unsigned short rtc_ydays[2][13] = {
	/* Normal years */
	
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
	/* Leap years */
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

static const unsigned short rtc_hour[2][24] = {
	/* 12-hour clock mode */
	{0x12, 0x1, 0x2, 0x3, 0x4, 0x5,  0x6,   0x7,  0x8,   0x9,  0x10, 0x11,
	0x32, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x30,0x31 },
	
 	/* 24-hour clock mode */
 	{ 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x90, 0x91,
 	0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xa0, 0xa1, 0xa2, 0xa3 }
};

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)

/*****************************************************************************/
//  Function: static inline uint8_t brite_rtc_read(struct brite_rtc* priv, uint32_t offset)
//
//  Parameters:
//		priv		:  rtc pointer
//
//		offset	: register offset
//
//  Return:
//		register value
//		
//  Description:
//      Read the value from a register.
//
/*****************************************************************************/
static inline uint8_t brite_rtc_read(struct brite_rtc* priv, uint32_t offset)
{
	uint32_t val = __raw_readl((unsigned long)priv->regs + offset);
	
	return (val&0xFF);
}

/*****************************************************************************/
//  Function: static inline void brite_rtc_write(struct brite_rtc* priv, uint32_t offset, uint32_t val)
//
//  Parameters:
//		priv		:  rtc pointer
//
//		offset	: register offset
//
//		val		: the value to write to a register
//
//  Return:
//		none
//		
//  Description:
//      Write the value to a register.
//
/*****************************************************************************/
static inline void brite_rtc_write(struct brite_rtc* priv, uint32_t offset, uint32_t val)
{
	uint32_t reg_val;
	unsigned long long start = get_timer(0);

	while (1)
	{
		reg_val = __raw_readl((u32)p_rtc->regs + RTC_PEND_WR);

		if ((reg_val & RTC_TRIM_PEND_WR_MASK) != RTC_TRIM_PEND_WR_MASK)
		{
			break;
		}

		if(get_timer(start) > RTC_TIMEOUT) {
			info("brite_rtc_write check RTC_PEND_WR S timeout\n");
			break;
		}
	}

	__raw_writel(val, (unsigned long)priv->regs + offset);
	
	while (1)
	{
		reg_val = __raw_readl((u32)p_rtc->regs + RTC_PEND_WR);

		if ((reg_val & RTC_TRIM_PEND_WR_MASK) != RTC_TRIM_PEND_WR_MASK)
		{
			break;
		}

		if(get_timer(start) > RTC_TIMEOUT) {
			info("brite_rtc_write check RTC_PEND_WR timeout\n");
			break;
		}
	}
}

/*****************************************************************************/
//  Function: static inline bool is_leap_year(unsigned int year)
//
//  Parameters:
//		year		:  year
//
//  Return:
//		FALSE 	: common year
//
//		TRUE 	: leap year		
//
//  Description:
//      Check if the year is a leap year.
//
/*****************************************************************************/
static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

unsigned long mktime(const unsigned int year0, const unsigned int mon0,
	const unsigned int day, const unsigned int hour, const unsigned int min,
	const unsigned int sec)
{
	unsigned int mon = mon0, year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int)(mon -= 2)) {
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return ((((unsigned long)
	(year / 4 - year / 100 + year / 400 + 367 * mon / 12 + day) + year *
		365 -
		719499) *
		24 +
		hour /* now have hours */) *
		60 + min /* now have minutes */) * 60 + sec; /* finally seconds */
}

/*
 * The number of days in the month.
 */
int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}

/*
 * The number of days since January 1. (0 to 365)
 */
int rtc_year_days(unsigned int day, unsigned int month, unsigned int year)
{
	return rtc_ydays[is_leap_year(year)][month] + day - 1;
}

#ifdef TAPEOUT_V2
int check_time(int year, int month, int day, int hour, int min, int sec, int thsec100)
{
	if ((year < 1970) || (year > 2099) || (month < 1) || (month > 12) ||
		(day < 1) || (day > 31) || (hour < 0) || (hour > 23) ||
		(min <	0) || (min > 59) || (sec < 0) || (sec > 59) ||
		(thsec100 < 0) || (thsec100 > 99) || ((month == 2) && (day > 29)) ||
		(!((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0)) &&
		((month == 2) && (day > 28))) ||
		((month == 4 || month == 6 || month == 9 || month == 11) && (day>30))) {
		return -1;
	}

	return 0;
}
#else
int check_time(int year, int month, int day, int hour, int min, int sec)
{
	if ((year < 1970) || (year > 2099) || (month < 1) || (month > 12) ||
		(day < 1) || (day > 31) || (hour < 0) || (hour > 23) ||
		(min <	0) || (min > 59) || (sec < 0) || (sec > 59) ||
		((month == 2) && (day > 29)) ||
		(!((year % 4 == 0) && (year % 100 != 0) || (year % 400 == 0)) &&
		((month == 2) && (day > 28)))
		((month == 4 || month == 6 || month == 9 || month == 11) && (day>30))) {
		return -1;
	}

	return 0;
}
#endif
static int div_s64_rem(unsigned long dividend, int divisor, int* remainder)
{
	*remainder = dividend % divisor;
	return dividend / divisor;
}
static int div_s64(unsigned long dividend, int divisor)
{
	int remainder;
	return div_s64_rem(dividend, divisor, &remainder);
}

/*
 * Convert seconds since 01-01-1970 00:00:00 to Gregorian date.
 */
void rtc_time_to_tm(unsigned long time, struct rtc_time* tm)
{
	unsigned int month,year;
	int days;

	days = div_s64(time, 86400);
	time -= (unsigned int) days * 86400;

	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365 + LEAPS_THRU_END_OF(year - 1) -
		LEAPS_THRU_END_OF(1970 - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + is_leap_year(year);
	}
	tm->tm_year = year - 1900;
	tm->tm_yday = days + 1;

	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - rtc_month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = month;
	tm->tm_mday = days + 1;

	tm->tm_hour = time / 3600;
	time -= tm->tm_hour * 3600;
	tm->tm_min = time / 60;
	tm->tm_sec = time - tm->tm_min * 60;

	tm->tm_isdst = 0;
}

/*
 * Convert Gregorian date to seconds since 01-01-1970 00:00:00.
 */
int rtc_tm_to_time(struct rtc_time* tm, unsigned long* time)
{
	*time = mktime(tm->tm_year + 1900,
		tm->tm_mon + 1,
		tm->tm_mday,
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec);
	return 0;
}

static inline unsigned int bcd2bin(u8 val)
{
	return ((val)&0x0f) + ((val) >> 4) * 10;
}

static inline u8 bin2bcd(unsigned int val)
{
	return (((val / 10) << 4) | (val % 10));
}

static void start_rtc_read(void)
{
	unsigned long long start = get_timer(0);

	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("start_rtc_read check RTC_PEND_WR timeout first\n");
			return ;
		}
	}
	__raw_writeb(RTC_RDSTA_EN, (u32)p_rtc->regs + RTC_RDSTA_A7);

	start = get_timer(0);
	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("start_rtc_read check RTC_PEND_WR timeout last\n");
			break;
		}
	}
}

static void stop_rtc_read(void)
{
	unsigned long long start = get_timer(0);

	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("stop_rtc_read check RTC_PEND_WR timeout first\n");
			return ;
		}
	}
	__raw_writeb(RTC_RDSTP_EN, (u32)p_rtc->regs + RTC_RDSTP_A7);

	start = get_timer(0);
	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("stop_rtc_read check RTC_PEND_WR timeout last\n");
			break;
		}
	}
}

static void start_rtc_write(void)
{
	unsigned long long start = get_timer(0);

	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("start_rtc_write check RTC_PEND_WR timeout first\n");
			return ;
		}
	}
	__raw_writeb(RTC_WRSTA_EN, (u32)p_rtc->regs + RTC_WRSTA_A7);

	start = get_timer(0);
	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("start_rtc_write check RTC_PEND_WR timeout last\n");
			break;
		}
	}
}

void stop_rtc_write(void)
{
	unsigned long long start = get_timer(0);

	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("stop_rtc_write check RTC_PEND_WR timeout first\n");
			return ;
		}
	}
	__raw_writeb(RTC_WRSTP_EN, (u32)p_rtc->regs + RTC_WRSTP_A7);

	start = get_timer(0);
	while (__raw_readb((u32)p_rtc->regs + RTC_PEND_WR)) {
		if(get_timer(start) > RTC_TIMEOUT) {
			info("stop_rtc_write check RTC_PEND_WR timeout last\n");
			break;
		}
	}
}

static u8 rtc_read_100thsec(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_100THSEC);
	return val;
}

#ifdef TAPEOUT_V2
static u8 rtc_write_100thsec(u8 val)
{
	brite_rtc_write(p_rtc, RTC_100THSEC, val);
}
#endif

static u8 rtc_read_sec(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_SEC);
	return val;
}
static void rtc_write_sec(u8 val)
{
	brite_rtc_write(p_rtc, RTC_SEC, val);
}

static u8 rtc_read_min(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_MIN);
	return val;
}
static void rtc_write_min(u8 val)
{
	brite_rtc_write(p_rtc, RTC_MIN, val);
}

static u8 rtc_read_hour(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_HOUR);
	return val;
}

static void rtc_write_hour(u8 val)
{
	if (p_rtc->mode & RTC_MODE_HOUR24)/*24-hour clock mode. */ {
		val = bcd2bin(val);
		val = rtc_hour[1][val];
		val |= (RTC_MODE_HOUR24 << 7);
	} else {
		val = bcd2bin(val);
		val = rtc_hour[0][val];
	}
	brite_rtc_write(p_rtc, RTC_HOUR, val);
}

static u8 rtc_read_week(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_WEEK);
	return val;
}
static void rtc_write_week(u8 val)
{
	brite_rtc_write(p_rtc, RTC_WEEK, val);
}

static u8 rtc_read_day(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_DAY);
	return val;
}
static void rtc_write_day(u8 val)
{
	brite_rtc_write(p_rtc, RTC_DAY, val);
}

static u8 rtc_read_year(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_YEAR);
	return val;
}
static void rtc_write_year(u8 val)
{
	brite_rtc_write(p_rtc, RTC_YEAR, val);
}

/*read cen+month*/
static u8 rtc_read_cen_month(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_CEN_MONTH);
	return val;
}

/*bit7 - cen: Centuary counter. 0: 1900; 1: 2000 */
static u8 rtc_read_cent(void)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_CEN_MONTH);
	val = (val >> 7);
	return val;
}

/*write cent+month*/
static void rtc_write_cen_month(u8 month)
{
	u8 val;

	val = p_rtc->cent << 7;
	val |= month ;
	brite_rtc_write(p_rtc, RTC_CEN_MONTH, val);
}

/* ALM1_MASK register 0x50 */
void alarm_set_mask(u8 mask)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_ALM1_MASK);
	val |= mask;
	brite_rtc_write(p_rtc, RTC_ALM1_MASK, val);
}
/* ALM1_MASK register 0x50 */
void alarm_clear_mask(u8 mask)
{
	u8 val;
	val = brite_rtc_read(p_rtc, RTC_ALM1_MASK);
	val &= (~mask);
	brite_rtc_write(p_rtc, RTC_ALM1_MASK, val);
}

void alarm_enable(unsigned char id, int en)
{
	u8 val = 0;

	val = brite_rtc_read(p_rtc, RTC_ALM_EN);
	if (id == 1) {
		val &= ~RTC_ALM1_EN;
		if (en)
			val |= RTC_ALM1_EN;
	} else {
		val &= ~RTC_ALM2_EN;
		if (en)
			val |= RTC_ALM2_EN;
	}
	brite_rtc_write(p_rtc, RTC_ALM_EN, val);
}
static void alarm_write_hour(u8 val)
{
	if (p_rtc->mode & RTC_MODE_HOUR24)/*24-hour clock mode. */ {
		val = bcd2bin(val);
		val = rtc_hour[1][val];
		val |= (RTC_MODE_HOUR24 << 7);
	} else {
		val = bcd2bin(val);
		val = rtc_hour[0][val];
	}
	brite_rtc_write(p_rtc, RTC_ALM1_HOUR, val);
}

/*****************************************************************************/
//	Function: int brite_rtc_init(u32 base, RTC_MODE_HOUR12_24 mode)
//
//	Parameters:
//		base		: base address
//
//		mode	: hour mode
//
//	Return:
//		rtc initialized flag.
//		0: no initialized
//		1: initialized
//
//	Description:
//		Initialize RTC controller.
//
/*****************************************************************************/
int brite_rtc_init(u32 base, RTC_MODE_HOUR12_24 mode)
{
	if(init_rtc == 0)
	{
		memset(p_rtc, 0, sizeof(struct brite_rtc));

		p_rtc->regs = (void*)base;
		p_rtc->mode = mode;
		p_rtc->sec_100thsec_99 = -1;

		/*clear int status,disable int*/
		brite_rtc_set_int_mask(0xff);
		brite_rtc_clear_int(0x3);
		alarm_enable(1, 0);
		alarm_enable(2, 0);
		/*clear alarm setting*/
		alarm_clear_mask(0xff);
		init_rtc = 1;
	}
	return 0;
}


/*****************************************************************************/
//	Function: void brite_rtc_gettime(struct rtc_time* tm)
//
//	Parameters:
//		tm	: time structure
//
//	Return:
//		none
//
//	Description:
//		Get rtc time.
//
/*****************************************************************************/
void brite_rtc_gettime_internal(struct rtc_time* tm)
{
	u8 hour = 0;
	u8 month = 0;
	int cent = 0;

	start_rtc_read();
	tm->tm_100thsec = rtc_read_100thsec();

	tm->tm_mday = rtc_read_day();
	month = rtc_read_cen_month();
	tm->tm_mon = month & 0x1f;
	cent = month >> 7;
	tm->tm_year = rtc_read_year();

	hour = rtc_read_hour();
	if (hour & HOUR12_24) {
		tm->tm_hour = hour & 0x3f;
	} else {
		tm->tm_hour = hour & 0x1f;
	}

	tm->tm_min = rtc_read_min();
	tm->tm_sec = rtc_read_sec();
	tm->tm_wday = rtc_read_week();

	/*bcd convert*/
	tm->tm_100thsec = bcd2bin(tm->tm_100thsec);
	tm->tm_sec = bcd2bin(tm->tm_sec);
	tm->tm_min = bcd2bin(tm->tm_min);
	tm->tm_hour = bcd2bin(tm->tm_hour);
	tm->tm_mday = bcd2bin(tm->tm_mday);
	tm->tm_mon = bcd2bin(tm->tm_mon) & 0x1f;
	tm->tm_year = bcd2bin(tm->tm_year);
	tm->tm_year += cent * 100 + 1900;

	if (!(hour & HOUR12_24)) {
		if (hour & H20_PA) {
			if (tm->tm_hour != 12)
				tm->tm_hour += 12;
		} else if (tm->tm_hour == 12) {
			tm->tm_hour -= 12;
		}
	}

	stop_rtc_read();
}

static int brite_rtc_gettime_second(void)
{
	int sec;

	start_rtc_read();
	sec = rtc_read_sec();
	sec = bcd2bin(sec);
	stop_rtc_read();

	return sec;
}


/*****************************************************************************/
//	Function: void brite_rtc_gettime(struct rtc_time* tm)
//
//	Parameters:
//		tm	: time structure
//
//	Return:
//		none
//
//	Description:
//		Get rtc time.
//
/*****************************************************************************/
void brite_rtc_gettime(struct rtc_time* tm)
{
	int sec = 0;
	int i;

	if (sw_mutex_lock(5000)) {
		info("arm mutex lock failed\n");
		return;
	}

	brite_rtc_gettime_internal(tm);

	if (tm->tm_100thsec == 0)
	{
		for(i=0; i<10; i++)
		{
			sec = brite_rtc_gettime_second(); 
			if(sec != tm->tm_sec)
			{
				tm->tm_100thsec = 99;
				break;
			}
		}
	}

	if (sw_mutex_unlock()) {
		info("arm mutex unlock failed\n");
		return;
	}
}


/*****************************************************************************/
//	Function: void brite_rtc_settime(struct rtc_time* tm)
//
//	Parameters:
//		tm	: time structure
//
//	Return:
//		none
//
//	Description:
//		Set rtc time.
//
/*****************************************************************************/
void brite_rtc_settime(struct rtc_time* tm)
{
	p_rtc->sec_100thsec_99 = -1;

#ifdef TAPEOUT_V2
	rtc_trim_ctrl(RTC_TRIM_DIS);
#endif
	start_rtc_write();

	int year = tm->tm_year - 100;
	if (year >= 0) {
		p_rtc->cent = 1;
	} else {
		p_rtc->cent = 0;
		year = tm->tm_year;
	}
#ifdef TAPEOUT_V2
	rtc_write_100thsec(bin2bcd(tm->tm_100thsec));
#endif
	rtc_write_sec(bin2bcd(tm->tm_sec));
	rtc_write_min(bin2bcd(tm->tm_min));
	rtc_write_hour(bin2bcd(tm->tm_hour));
	rtc_write_week(tm->tm_wday);
	rtc_write_day(bin2bcd(tm->tm_mday));
	rtc_write_cen_month(bin2bcd(tm->tm_mon + 1));
	rtc_write_year(bin2bcd(year));
	stop_rtc_write();

#ifdef TAPEOUT_V2
	rtc_trim_ctrl(RTC_TRIM_EN);
#endif
}

/*****************************************************************************/
//	Function: int rtc_calculation_week(struct rtc_time* tm)
//
//	Parameters:
//		tm	: time structure
//
//	Return:
//		week
//
//	Description:
//		Get the week value(Sun=0, Mon=1...Sat=6).
//
/*****************************************************************************/
int rtc_calculation_week(struct rtc_time* tm)
{
	int y,m,d,w;
	y = tm->tm_year;
	m = tm->tm_mon;
	d = tm->tm_mday;
	if (m == 1 | m == 2) {
		m += 12;
		y--;
	}
	w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400 + 1) %
		7;
	if (tm->tm_wday != w) {
		debug("input week day error! right week is %d.\n", w);
	}

	return w;
}

/*****************************************************************************/
//	Function: void brite_rtc_get_trigger_mode(void)
//
//	Parameters:
//		none
//
//	Return:
//		none
//
//	Description:
//		Check the alarm trigger mode.
//
/*****************************************************************************/
void brite_rtc_get_trigger_mode(void)
{
	u8 alm_en;
	alm_en = brite_rtc_read(p_rtc, RTC_ALM1_MASK);

	// Get alarm interrupt trigger mode
	if (alm_en & ALM_MASK_A1100THSEC){
		debug("\nThis alarm was triggered by '100THSEC'.\n");
	}

	if (alm_en & ALM_MASK_A1MSEC){
		debug("\nThis alarm was triggered by 'SECOND'.\n");
	}
	else if (alm_en & ALM_MASK_A1MMIN){
		debug("\nThis alarm was triggered by 'MINUTE'.\n");
	}
	else if (alm_en & ALM_MASK_A1MHOUR){
		debug("\nThis alarm was triggered by 'HOUR'.\n");
	}
	else if (alm_en & ALM_MASK_A1MDAY){
		debug("\nThis alarm was triggered by 'DAY'.\n");
	}
	else if (alm_en & ALM_MASK_A1MMON){
		debug("\nThis alarm was triggered by 'MONTH'.\n");
	}
	else if (alm_en & ALM_MASK_A1MYEAR){
		debug("\nThis alarm interrupt was triggered by 'YEAR'.\n");
	}
}

void brite_rtc_get_trigger_mode_multi(void)
{
	u8 alm_en, i;
	alm_en = brite_rtc_read(p_rtc, RTC_ALM1_MASK);

	for (i = 0; i < 8; i++)
	{
		switch (alm_en & (1<<i))
		{
			case ALM_MASK_A1100THSEC: 
				debug("\nThis alarm interrupt was triggered by '100THSEC'.\n");
				break;
			case ALM_MASK_A1MSEC: 
				debug("\nThis alarm interrupt was triggered by 'SECOND'.\n");
				break;	
			case ALM_MASK_A1MMIN: 
				debug("\nThis alarm interrupt was triggered by 'MINUTE'.\n");
				break;
			case ALM_MASK_A1MHOUR: 
				debug("\nThis alarm interrupt was triggered by 'HOUR'.\n");
				break;
			case ALM_MASK_A1MDAY: 
				debug("\nThis alarm interrupt was triggered by 'DAY'.\n");
				break;
			case ALM_MASK_A1MMON: 
				debug("\nThis alarm interrupt was triggered by 'MONTH'.\n");
				break;
			case ALM_MASK_A1MYEAR: 	
				debug("\nThis alarm interrupt was triggered by 'YEAR'.\n");	
				break;	
			default:
				break;
		}
	}
}


/*****************************************************************************/
//	Function: void brite_rtc_getalarm(struct rtc_wkalrm* alrm, unsigned char* hour_mode)
//
//	Parameters:
//		alrm			: alarm structure
//
//		hour_mode	: hout mode
//
//	Return:
//		none
//
//	Description:
//		Get alarm value and alarm flag.
//
/*****************************************************************************/
void brite_rtc_getalarm(struct rtc_wkalrm* alrm, unsigned char* hour_mode)
{
	struct rtc_time* alm_tm = &alrm->time;
	u8 alm_en,cen;
	u8 hour;

	alm_tm->tm_sec = brite_rtc_read(p_rtc, RTC_ALM1_SEC);
	alm_tm->tm_min = brite_rtc_read(p_rtc, RTC_ALM1_MIN);
	hour = brite_rtc_read(p_rtc, RTC_ALM1_HOUR);
	if (hour & HOUR12_24) {
		alm_tm->tm_hour = hour & 0x3f;
	} else {
		alm_tm->tm_hour = hour & 0x1f;
	}
	*hour_mode = hour;

	alm_tm->tm_mday = brite_rtc_read(p_rtc, RTC_ALM1_DAY);
	alm_tm->tm_mon = brite_rtc_read(p_rtc, RTC_ALM1_MONTH);
	alm_tm->tm_year = brite_rtc_read(p_rtc, RTC_ALM1_YEAR);
	alm_tm->tm_wday = brite_rtc_read(p_rtc, RTC_ALM1_WEEK);

	alm_en = brite_rtc_read(p_rtc, RTC_ALM1_MASK);

	/* decode the alarm enable field */
	if (alm_en & ALM_MASK_A1MSEC)
		alm_tm->tm_sec = bcd2bin(alm_tm->tm_sec);
	else
		alm_tm->tm_sec = -1;

	if (alm_en & ALM_MASK_A1MMIN)
		alm_tm->tm_min = bcd2bin(alm_tm->tm_min);
	else
		alm_tm->tm_min = -1;

	if (alm_en & ALM_MASK_A1MHOUR)
		alm_tm->tm_hour = bcd2bin(alm_tm->tm_hour);
	else
		alm_tm->tm_hour = -1;

	if (alm_en & ALM_MASK_A1MDAY)
		alm_tm->tm_mday = bcd2bin(alm_tm->tm_mday);
	else
		alm_tm->tm_mday = -1;

	if (alm_en & ALM_MASK_A1MMON) {
		alm_tm->tm_mon = bcd2bin(alm_tm->tm_mon);
	} else {
		alm_tm->tm_mon = -1;
	}

	cen = rtc_read_cent();
	if (alm_en & ALM_MASK_A1MYEAR)
		alm_tm->tm_year = bcd2bin(alm_tm->tm_year) + 100 * cen + 1900;
	else
		alm_tm->tm_year = -1;
}

/*****************************************************************************/
//	Function: void brite_rtc_setalarm(struct rtc_wkalrm* alrm, int flag)
//
//	Parameters:
//		alrm		: alarm structure
//
//		flag		: alarm flag
//	Return:
//		none
//
//	Description:
//		Set alarm value and alarm flag.
//
/*****************************************************************************/
void brite_rtc_setalarm(struct rtc_wkalrm* alrm, int flag)
{
	struct rtc_time* tm = &alrm->time;
	u8 alrm_en,cen;

	brite_rtc_write(p_rtc, RTC_ALM1_MASK, 0);
#ifdef TAPEOUT_V2
	brite_rtc_write(p_rtc, RTC_ALM1_100THSEC, 0);
#endif
	brite_rtc_write(p_rtc, RTC_ALM1_SEC, 0);
	brite_rtc_write(p_rtc, RTC_ALM1_MIN, 0);
	brite_rtc_write(p_rtc, RTC_ALM1_HOUR, 0);
	brite_rtc_write(p_rtc, RTC_ALM1_WEEK, 0);
	brite_rtc_write(p_rtc, RTC_ALM1_DAY, 0);
	brite_rtc_write(p_rtc, RTC_ALM1_MONTH, 0);
	brite_rtc_write(p_rtc, RTC_ALM1_YEAR, 0);

#ifdef TAPEOUT_V2
	if (tm->tm_100thsec < 100 && tm->tm_100thsec >= 0) {
		brite_rtc_write(p_rtc, RTC_ALM1_100THSEC, bin2bcd(tm->tm_100thsec));
	}
#endif
	if (tm->tm_sec < 60 && tm->tm_sec >= 0) {
		brite_rtc_write(p_rtc, RTC_ALM1_SEC, bin2bcd(tm->tm_sec));
	}

	if (tm->tm_min < 60 && tm->tm_min >= 0) {
		brite_rtc_write(p_rtc, RTC_ALM1_MIN, bin2bcd(tm->tm_min));
	}

	if (tm->tm_hour < 24 && tm->tm_hour >= 0) {
		alarm_write_hour(bin2bcd(tm->tm_hour));
	}

	if (tm->tm_mday <= 31 && tm->tm_mday > 0) {
		brite_rtc_write(p_rtc, RTC_ALM1_DAY, bin2bcd(tm->tm_mday));
	}

	if (tm->tm_mon < 12 && tm->tm_mon >= 0) {
		brite_rtc_write(p_rtc, RTC_ALM1_MONTH, bin2bcd(tm->tm_mon + 1));
	}

	cen = rtc_read_cent();
	if ((tm->tm_year - (cen * 100)) < 100 &&
		(tm->tm_year - (cen * 100)) >= 0) {
		brite_rtc_write(p_rtc,
			RTC_ALM1_YEAR,
			bin2bcd(tm->tm_year - cen * 100));
	}


	if (tm->tm_wday < 7 && tm->tm_wday > 0) {
		brite_rtc_write(p_rtc, RTC_ALM1_WEEK, bin2bcd(tm->tm_wday));
	}

	alrm_en = flag;
	brite_rtc_write(p_rtc, RTC_ALM1_MASK, alrm_en);

	return;
}

/*****************************************************************************/
//  Function: unsigned char brite_rtc_get_int_status(void)
//
//  Parameters:
//		none
//
//  Return:
//		interrupt status
//
//  Description:
//      Get interrupt status.
//
/*****************************************************************************/
unsigned char brite_rtc_get_int_status(void)
{
	return brite_rtc_read(p_rtc, RTC_IRQ_STA);
}

/*****************************************************************************/
//  Function: unsigned char brite_rtc_get_raw_int_status(void)
//
//  Parameters:
//		none
//
//  Return:
//		raw interrupt status
//
//  Description:
//      Get raw interrupt status.
//
/*****************************************************************************/
unsigned char brite_rtc_get_raw_int_status(void)
{
	return brite_rtc_read(p_rtc, RTC_IRQ_RAW);
}

/*****************************************************************************/
//  Function: void brite_rtc_set_int_mask(unsigned char mask)
//
//  Parameters:
//		mask	: mask value
//
//  Return:
//		none
//
//  Description:
//      Mask interrupt.
//
/*****************************************************************************/
void brite_rtc_set_int_mask(unsigned char mask)
{
	brite_rtc_write(p_rtc, RTC_IRQ_MASK, mask);
}

/*****************************************************************************/
//  Function: void brite_rtc_clear_int(unsigned char val)
//
//  Parameters:
//		val	: clear value
//
//  Return:
//		none
//
//  Description:
//      Clear interrupt status.
//
/*****************************************************************************/
void brite_rtc_clear_int(unsigned char val)
{
	brite_rtc_write(p_rtc, RTC_IRQ_CLR, val);
}

/*****************************************************************************/
//  Function: int8_t rtc_read_trim(void)
//
//  Parameters:
//		none
//
//  Return:
//		trim value
//
//  Description:
//      Get trim value.
//
/*****************************************************************************/
int8_t rtc_read_trim(void)
{
	return (int8_t)brite_rtc_read(p_rtc, RTC_TRIM_REG);
}

/*****************************************************************************/
//  Function: void rtc_write_trim(int8_t val)
//
//  Parameters:
//		val	: trim value
//
//  Return:
//		none
//
//  Description:
//      Set trim value.
//
/*****************************************************************************/
void rtc_write_trim(int8_t val)
{
	brite_rtc_write(p_rtc, RTC_TRIM_REG, (uint32_t)val);
}

/*****************************************************************************/
//  Function: void rtc_trim_ctrl(E_RTC_TRIM_EN ctrl)
//
//  Parameters:
//		ctrl	: trim enable/disable
//
//  Return:
//		none
//
//  Description:
//      Trim control.
//
/*****************************************************************************/
void rtc_trim_ctrl(E_RTC_TRIM_EN ctrl)
{
	brite_rtc_write(p_rtc, RTC_TRIM_EN_REG, ctrl);
}

/*****************************************************************************/
//  Function: void rtc_trim_vld_ctrl(E_RTC_TRIM_VLD_EN ctrl)
//
//  Parameters:
//		ctrl	: trim vld enable/disable
//
//  Return:
//		none
//
//  Description:
//      Trim vld control.
//
/*****************************************************************************/
void rtc_trim_vld_ctrl(E_RTC_TRIM_VLD_EN ctrl)
{
	brite_rtc_write(p_rtc, RTC_TRIM_VLD_EN_REG, ctrl);
}

/*****************************************************************************/
//  Function: uint8_t rtc_read_trim_mode(void)
//
//  Parameters:
//		none
//
//  Return:
//		none
//
//  Description:
//      Read trim mode.
//
/*****************************************************************************/
uint8_t rtc_read_trim_mode(void)
{
	return brite_rtc_read(p_rtc, RTC_TRIM_MODE_REG);
}

/*****************************************************************************/
//  Function: void rtc_write_trim_mode(E_RTC_TRIM_MODE trim_mode, E_RTC_TRIM_VLD_MODE vld_mode)
//
//  Parameters:
//		trim_mode	:  trim mode
//
//		vld_mode	: trim vld mode
//  Return:
//		none
//
//  Description:
//      Set trim mode and trim vld mode.
//
/*****************************************************************************/
void rtc_write_trim_mode(E_RTC_TRIM_MODE trim_mode, E_RTC_TRIM_VLD_MODE vld_mode)
{
	uint8_t reg_val = brite_rtc_read(p_rtc, RTC_TRIM_MODE_REG);

	reg_val &= ~RTC_TRIM_MODE_MASK;
	reg_val |= RTC_TRIM_MODE_VAL(trim_mode);

	reg_val &= ~RTC_TRIM_VLD_MODE_MASK;
	reg_val |= RTC_TRIM_VLD_MODE_VAL(vld_mode);	
	
	brite_rtc_write(p_rtc, RTC_TRIM_MODE_REG, reg_val);
}


