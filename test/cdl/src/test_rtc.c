/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_rtc.h"
#include "test_timer.h"

#if (CONFIG_RTC_TEST == 1)
#include "cdl_rtc.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/

/*************************************************************************************************************/
// global variable definition
/*************************************************************************************************************/
struct rtc_time  rtc_tm_test;
struct rtc_time* p_rtc_tm_test = &rtc_tm_test;

struct rtc_wkalrm  rtc_wkalrm_test;

cmdline_entry rtc_test_menu[];

/*************************************************************************************************************/
// function prototype
/*************************************************************************************************************/

/*************************************************************************************************************/
// function implementation
/*************************************************************************************************************/
static void display_rtc_time(struct rtc_time *tm, RTC_MODE_HOUR12_24 disp_mode)
{
	int hour;
	int is_pm = 0; /*0 am 1 pm*/

	hour = tm->tm_hour;

	/* info("%s hour %d disp_mode %d\n", __func__, hour, disp_mode); */

	// hardware setting using 24 hour mode
	// display using 12 hour mode
	if (disp_mode != RTC_MODE_HOUR24)
	{
		if(hour == 12) {
			is_pm = 1;
		} else if (hour > 12) {
			hour -= 12;
			is_pm = 1;
		} else if(tm->tm_hour == 0) {
			hour = 12;
		}
	}


	if (disp_mode == RTC_MODE_HOUR24)
	{
		info("rtc time %04d.%02d.%02d %02d:%02d:%02d:%02d week=%d\n",
			 tm->tm_year, tm->tm_mon, tm->tm_mday,
			 hour, tm->tm_min, tm->tm_sec, tm->tm_100thsec,
			 tm->tm_wday);
	} else {
		info("rtc time %04d.%02d.%02d %02d:%02d:%02d:%02d %s week=%d\n",
			tm->tm_year, tm->tm_mon, tm->tm_mday,
			hour, tm->tm_min, tm->tm_sec, tm->tm_100thsec,
			is_pm?"pm":"am",
			tm->tm_wday);
	}
}

static int cmd_help(int argc, char *argv[])
{
	return cmdline_help_general(&rtc_test_menu[0]);
}

//************************************************************************************************************
// Get rtc hour mode
//************************************************************************************************************
int rtc_get_mode_hour(const char *mode_str, bool is_set, RTC_MODE_HOUR12_24 *mode, int *hour) {
	int ret = 0;
	int dummy = 12;

	if (!mode) {
		info("mode points to NULL\n");
		return -1;
	}
	if(!hour)
		hour = &dummy;

	if(is_set) { /* set time */
		if (!strcmp(mode_str, "am") || !strcmp(mode_str, "pm")) {
			*mode = RTC_MODE_HOUR12;
			if(*hour > 12) {
				info("hour > 12 in 12h mode\n");
				return -1;
			}
			if(*hour == 12)
				*hour = 0;
			if (!strcmp(mode_str, "pm"))
				*hour += 12;
			return 0;
		}
	} else { /* display time */
		if (!strcmp(mode_str, "12h")) {
			*mode = RTC_MODE_HOUR12;
			return 0;
		}
	}

	if (!strcmp(mode_str, "24h")) {
		*mode = RTC_MODE_HOUR24;
		return 0;
	}

	info("wrong mode!\n");
	return -1;
}

//************************************************************************************************************
// Set rtc time
//************************************************************************************************************
static int rtc_set_time(int argc, char *argv[])
{
	uint32_t test_time = 0;
	int sec = 0, min = 0, hour = 0, week = 0, day = 0, cen_month = 0, year = 0;
#ifdef TAPEOUT_V2
	int thsec100 = 0;
#endif
	int ret = 0;
	RTC_MODE_HOUR12_24 mode = 0;

	info("%s\n", __func__);
#ifdef TAPEOUT_V2
	// Check the validity of parameter count
	if (argc < 9) 
	{
		info("wrong cmd_format: %s mode year month day hour min sec 100thsec.\n"
				"mode: am|pm|24h\n", argv[0]);
		return 0;
	}
#else
	// Check the validity of parameter count
	if (argc < 8) 
	{
		info("wrong cmd_format: %s mode year month day hour min sec.\n"
				"mode: am|pm|24h\n", argv[0]);
		return 0;
	}
#endif
	// Get the parameters
	year = get_arg_ulong(argv[2]);
	cen_month = get_arg_ulong(argv[3]);
	day = get_arg_ulong(argv[4]);
	hour = get_arg_ulong(argv[5]);
	min = get_arg_ulong(argv[6]);
	sec = get_arg_ulong(argv[7]);
#ifdef TAPEOUT_V2
	thsec100 = get_arg_ulong(argv[8]);
#endif
	if(rtc_get_mode_hour(argv[1], true, &mode, &hour))
		return -1;

#ifdef TAPEOUT_V2
	info("%d/%d/%d  %d:%d:%d:%d\n", year, cen_month, day, hour, min, sec, thsec100);
#else
	info("%d/%d/%d	%d:%d:%d\n", year, cen_month, day, hour, min, sec);
#endif
	memset(&rtc_tm_test, 0, sizeof(struct rtc_time));

#ifdef TAPEOUT_V2
	ret = check_time(year, cen_month, day, hour, min, sec, thsec100);
#else
	ret = check_time(year, cen_month, day, hour, min, sec);
#endif
	if(ret < 0)
	{
		info("invalid date time, please input right time.\n");
		info("valid date : 1970-1-1 to 2099-12-31\n");
		return 0;
	}


	test_time = mktime(year, cen_month, day, hour, min, sec);
	info("mktime: seconds = %u\n", test_time);

	brite_rtc_init(RTC_BASE, mode);

	rtc_time_to_tm(test_time, &rtc_tm_test);
#ifdef TAPEOUT_V2
	rtc_tm_test.tm_100thsec = thsec100;
#endif
	// set rtc time
	brite_rtc_settime(&rtc_tm_test);
	// get rtc time
	memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
	brite_rtc_gettime(&rtc_tm_test);

	// display rtc time
	display_rtc_time(&rtc_tm_test, mode);

	return 0;
}

//************************************************************************************************************
// Get rtc time
//************************************************************************************************************
static int rtc_get_time(int argc, char *argv[])
{
	RTC_MODE_HOUR12_24 mode;

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s mode.\n", argv[0]);
		info("mode: 12h|24h\n");
		return 0;
	}

	// Get the parameters
	if(rtc_get_mode_hour(argv[1], false, &mode, NULL))
		return -1;

	brite_rtc_init(RTC_BASE, mode);

	// get rtc time
	memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
	brite_rtc_gettime(&rtc_tm_test);

	// display rtc time
	display_rtc_time(&rtc_tm_test, mode);

	return 0;
}

//************************************************************************************************************
// Get rtc time continuously
//************************************************************************************************************
static int rtc_get_time_robust(int argc, char *argv[])
{
	int len;
	int i;
	RTC_MODE_HOUR12_24 mode;
	int ret;
	int test_count = 0;

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s mode\n"
				"mode: 12h|24h\n", argv[0]);
		return 0;
	}

	// Get the parameters
	if(rtc_get_mode_hour(argv[1], false, &mode, NULL))
		return -1;
	
	brite_rtc_init(RTC_BASE, mode);

	info("rtc get time robust test, press 'q' to quit\n");
	while (1) 
	{
		if (console_check_char('q'))
			break;

		test_count++;

		info("%d loop: ",test_count);
		
		// get rtc time
		memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
		brite_rtc_gettime(&rtc_tm_test);

		// display rtc time
		display_rtc_time(&rtc_tm_test, mode);
	}

	info("%s test terminate,total %d\n", argv[0], test_count);
	return 0;
}

//************************************************************************************************************
// Get rtc trigger mode
//************************************************************************************************************
int rtc_get_trigger_mode(const char *trigger_mode_str, int *trigger_mode, struct rtc_time *tm) 
{
	char m = 0;
	struct rtc_time dummy;

	if (!trigger_mode_str)
	{
		return -1;
	}
	
	if(!tm)
	{
		tm = &dummy;
	}
	
	m = trigger_mode_str[0];
	switch (m) 
	{
#ifdef TAPEOUT_V2
	case 'T':
		*trigger_mode = 0;
		tm->tm_100thsec += 10;
		tm->tm_100thsec = tm->tm_100thsec % 100;
		break;
#endif
	case 'S':
		*trigger_mode = 1;
		break;
	case 'M':
		*trigger_mode = 2;
		break;
	case 'H':
		*trigger_mode = 3;
		break;
	case 'd':
		*trigger_mode = 5;
		break;
	case 'm':
		*trigger_mode = 6;
		break;
	case 'y':
		*trigger_mode = 7;
		break;
	default:
		info("unrecognized trigger mode.\n");
		return -1;
	}
	
	return 0;
}

int rtc_get_multi_trigger_mode(const char *trigger_mode_str, int *trigger_mode, struct rtc_time *tm)
{
	struct rtc_time dummy;
	uint8_t m, i = 0, mode = 0;
	uint8_t *p_str = trigger_mode_str;

	if (!p_str)
	{
		return -1;
	}
	
	if(!tm)
	{
		tm = &dummy;
	}

	while (p_str[i] != NULL)
	{
		info("p_str[%d]:%c\n", i, p_str[i]);

		switch (p_str[i]) 
		{
			case 'T':
				mode |= 1 << 0;
				tm->tm_100thsec += 10;
				tm->tm_100thsec = tm->tm_100thsec % 100;
				break;
			case 'S':
				mode |= 1 << 1;
				break;
			case 'M':
				mode |= 1 << 2;
				break;
			case 'H':
				mode |= 1 << 3;
				break;
			case 'd':
				mode |= 1 << 5;
				break;
			case 'm':
				mode |= 1 << 6;
				break;
			case 'y':
				mode |= 1 << 7;
				break;
			default:
				*trigger_mode = mode;
				info("unrecognized trigger mode.\n");
				return -1;
		}

		i++;
	}

	info("mode: %x\n", mode);
	*trigger_mode = mode;
	return 0;
}

//************************************************************************************************************
// RTC alarm test
//************************************************************************************************************
static int rtc_alarm(int argc, char *argv[])
{
	u32 test_time=0;
	int sec, min, hour, week, day, cen_month, year;
#ifdef TAPEOUT_V2
	int thsec100;
#endif
	int ret = 0;
	struct rtc_time *alm_tm = &rtc_wkalrm_test.time;
	u8 status;
	int trigger_mode;
	RTC_MODE_HOUR12_24 mode;
	int fail = 0;

	info("%s\n", __func__);
	// Check the validity of parameter count
	if (argc < 10) 
	{
		info("wrong cmd_format: %s mode year month day hour min sec 100thsec trigger_mode.\n"
				"mode: am|pm|24h\n"
				"trigger_mode: y/m/d/H/M/S/T => year/month/day/Hour/Minute/Second/100thSec\n",
				argv[0]);
		return 0;
	}

	// Get the parameters
	year = get_arg_ulong(argv[2]);
	cen_month = get_arg_ulong(argv[3]);
	day = get_arg_ulong(argv[4]);
	hour = get_arg_ulong(argv[5]);
	min = get_arg_ulong(argv[6]);
	sec = get_arg_ulong(argv[7]);
	thsec100 = get_arg_ulong(argv[8]);
	if (rtc_get_mode_hour(argv[1], true, &mode, &hour))
		return -1;

	if (rtc_get_trigger_mode(argv[9], &trigger_mode, NULL))
		return -1;
	info("%d/%d/%d	%d:%d:%d:%d\n", year, cen_month, day, hour, min, sec, thsec100);

	memset(&rtc_wkalrm_test, 0, sizeof(struct rtc_wkalrm ));
	ret = check_time(year, cen_month, day, hour, min, sec, thsec100);

	if(ret < 0)
	{
		info("invalid date time, please input right time.\n");
		return 0;
	}

	test_time = mktime(year, cen_month, day, hour, min, sec);

	/* init system rtc*/
	brite_rtc_init(RTC_BASE,mode);
	rtc_time_to_tm(test_time, &rtc_tm_test);
	rtc_tm_test.tm_100thsec = thsec100;

	/*set rtc time*/
	brite_rtc_settime(&rtc_tm_test);
	/*get rtc time*/
	memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
	brite_rtc_gettime(&rtc_tm_test);

	/*display rtc time*/
	display_rtc_time(&rtc_tm_test, mode);

	/* 2- set alarm1 with no int*/
	brite_rtc_set_int_mask(0xff);
	brite_rtc_clear_int(0x3);

	rtc_wkalrm_test.enabled = 1;
	if(trigger_mode != 0)
	{
		rtc_time_to_tm(test_time+3, alm_tm);
		alm_tm->tm_100thsec = thsec100;
	}
	else
	{
		rtc_time_to_tm(test_time, alm_tm);
		alm_tm->tm_100thsec += thsec100;
		if (rtc_get_trigger_mode(argv[9], &trigger_mode, alm_tm))
			return -1;		
	}
	brite_rtc_setalarm(&rtc_wkalrm_test,(1<<trigger_mode));

	alarm_enable(1, 1);

	// wait alarm int generated
	info("press 'q' before alarm or when alarm occur to quit\n");
	while (1) 
	{
		if (console_check_char('q'))
		{
			fail++;
			break;
		}
		else
		{
			status = brite_rtc_get_raw_int_status();
			if (status & RTC_ALM1_EN)
			{				
				break;
			}
		}		
	}

    if(fail)
    {
    	info("%s: test fail. \n", argv[0]);
    }
	else
    {
		brite_rtc_gettime(&rtc_tm_test);
		info("\ncurrent rtc alarm time:\n");
		display_rtc_time(&rtc_tm_test, mode);
		brite_rtc_get_trigger_mode();	
		info("%s: test pass. \n", argv[0]);
    }

    alarm_enable(1, 0);
	return 0;
}

//************************************************************************************************************
// rtc interrupt routine
//************************************************************************************************************
static int rtc_int_counter = 0;
void rtc_int_handler(void* ptr)
{
	int int_status = 0, mode;
	int_status = brite_rtc_get_int_status();
	info("rtc_int_handler,status=%x\n", int_status);
	if (int_status) {
		rtc_int_counter++;
		memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
		brite_rtc_gettime(&rtc_tm_test);

		/*display rtc time*/
		display_rtc_time(&rtc_tm_test, mode);
		brite_rtc_clear_int(int_status);
	}
}

//************************************************************************************************************
// RTC interrupt test
//************************************************************************************************************
static int rtc_int(int argc, char* argv[])
{
	u32 test_time=0;
	int sec, min, hour, week, day, cen_month, year;
#ifdef TAPEOUT_V2
	int thsec100;
#endif
	int ret = 0;
	struct rtc_time *alm_tm = &rtc_wkalrm_test.time;
	u8 status;
	int trigger_mode;
	RTC_MODE_HOUR12_24 mode;
	int fail = 0;

	info("%s, press 'q' before alarm or when alarm occur to quit\n", __func__);
#ifdef TAPEOUT_V2
	if (argc < 10) {
		info("wrong cmd_format: %s mode year month day hour min sec thsec100 trigger_mode.\n"
				"mode: am|pm|24h\n"
				"trigger_mode: y/m/d/H/M/S/T => year/month/day/Hour/Minute/Second/100thSec\n",
				argv[0]);
		return 0;
	}
#else
	if (argc < 9) {
		info("wrong cmd_format: %s mode year month day hour min sec trigger_mode.\n"
				"mode: am|pm|24h\n"
				"trigger_mode: y/m/d/H/M/S => year/month/day/Hour/Minute/Second\n",
				argv[0]);
		return 0;
	}
#endif
	year = get_arg_ulong(argv[2]);
	cen_month = get_arg_ulong(argv[3]);
	day = get_arg_ulong(argv[4]);
	hour = get_arg_ulong(argv[5]);
	min = get_arg_ulong(argv[6]);
	sec = get_arg_ulong(argv[7]);
#ifdef TAPEOUT_V2
	thsec100 = get_arg_ulong(argv[8]);
#endif
	if (rtc_get_mode_hour(argv[1], true, &mode, &hour))
		return -1;
#ifdef TAPEOUT_V2
	if (rtc_get_trigger_mode(argv[9], &trigger_mode, NULL))
		return -1;

	info("%d/%d/%d  %d:%d:%d:%d\n", year, cen_month, day, hour, min, sec, thsec100);
#else
	if (rtc_get_trigger_mode(argv[8], &trigger_mode, NULL))
		return -1;

	info("%d/%d/%d  %d:%d:%d\n", year, cen_month, day, hour, min, sec);
#endif

	memset(&rtc_wkalrm_test, 0, sizeof(struct rtc_wkalrm ));

	/*register int handler and enable it*/
	irq_generic_install_handler(INT_RTC, rtc_int_handler, NULL);
	irq_generic_config_int(INT_RTC, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_RTC);

#ifdef TAPEOUT_V2
	ret = check_time(year, cen_month, day, hour, min, sec, thsec100);
#else
	ret = check_time(year, cen_month, day, hour, min, sec);
#endif
	if(ret < 0)
	{
		info("invalid date time, please input right time.\n");
		return 0;
	}

	test_time = mktime(year, cen_month, day, hour, min, sec);

	/* init system rtc*/
	brite_rtc_init(RTC_BASE,mode);
	rtc_time_to_tm(test_time, &rtc_tm_test);
#ifdef TAPEOUT_V2
	rtc_tm_test.tm_100thsec = thsec100;
#endif
	/*set rtc time*/
	brite_rtc_settime(&rtc_tm_test);
	/*get rtc time*/
	memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
	brite_rtc_gettime(&rtc_tm_test);

	/*display rtc time*/
	display_rtc_time(&rtc_tm_test, mode);

	/* 2- set alarm1 with int*/
	/*enable interrupt*/
	brite_rtc_set_int_mask(~RTC_ALM1_EN);
	brite_rtc_clear_int(0x3);
	rtc_int_counter = 0;

	rtc_wkalrm_test.enabled = 1;
	rtc_time_to_tm(test_time, alm_tm);
	/*test case should careful design to be sure trigger the setting after 5 second*/
#ifdef TAPEOUT_V2
	alm_tm->tm_100thsec = thsec100;
	if (rtc_get_trigger_mode(argv[9], &trigger_mode, alm_tm))
#else
	if (rtc_get_trigger_mode(argv[8], &trigger_mode, alm_tm))
#endif
		return -1;

	brite_rtc_setalarm(&rtc_wkalrm_test,(1<<trigger_mode));
	alarm_enable(1,1);

	/*wait alarm int generated*/
	while (1) {
		if (console_check_char('q'))
			break;
		if (rtc_int_counter > 0)
			break;
	}

	if (rtc_int_counter == 0)
		fail++;

	if (fail)
		info("%s: test fail.\n",argv[0]);
	else
		info("%s: test pass.\n",argv[0]);

	/*disable interrupt*/
	irq_generic_disable_int(INT_RTC);
	irq_generic_remove_handler(INT_RTC,NULL,NULL);
	brite_rtc_set_int_mask(0xff);
	alarm_enable(1, 0);

	return 0;
}

static int rtc_alarm_multi_mode(int argc, char* argv[])
{
	u32 test_time=0;
	int sec, min, hour, week, day, cen_month, year;
#ifdef TAPEOUT_V2
	int thsec100;
#endif
	int ret = 0;
	struct rtc_time *alm_tm = &rtc_wkalrm_test.time;
	u8 status;
	int trigger_mode;
	RTC_MODE_HOUR12_24 mode;
	int fail = 0;

	info("%s, press 'q' before alarm or when alarm occur to quit\n", __func__);
	if (argc < 10) {
		info("wrong cmd_format: %s mode year month day hour min sec thsec100 trigger_mode.\n"
				"mode: am|pm|24h\n"
				"trigger_mode: y/m/d/H/M/S/T => year/month/day/Hour/Minute/Second/100thSec\n",
				argv[0]);
		return 0;
	}

	year = get_arg_ulong(argv[2]);
	cen_month = get_arg_ulong(argv[3]);
	day = get_arg_ulong(argv[4]);
	hour = get_arg_ulong(argv[5]);
	min = get_arg_ulong(argv[6]);
	sec = get_arg_ulong(argv[7]);
	thsec100 = get_arg_ulong(argv[8]);

	if (rtc_get_mode_hour(argv[1], true, &mode, &hour))
		return -1;
	
	if (rtc_get_multi_trigger_mode(argv[9], &trigger_mode, NULL))
		return -1;

	info("%d/%d/%d  %d:%d:%d:%d\n", year, cen_month, day, hour, min, sec, thsec100);

	memset(&rtc_wkalrm_test, 0, sizeof(struct rtc_wkalrm ));

	/*register int handler and enable it*/
	irq_generic_install_handler(INT_RTC, rtc_int_handler, NULL);
	irq_generic_config_int(INT_RTC, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_RTC);

	ret = check_time(year, cen_month, day, hour, min, sec, thsec100);
	if(ret < 0)
	{
		info("invalid date time, please input right time.\n");
		return 0;
	}

	test_time = mktime(year, cen_month, day, hour, min, sec);

	/* init system rtc*/
	brite_rtc_init(RTC_BASE,mode);
	rtc_time_to_tm(test_time, &rtc_tm_test);
	rtc_tm_test.tm_100thsec = thsec100;

	/*set rtc time*/
	brite_rtc_settime(&rtc_tm_test);
	/*get rtc time*/
	memset(&rtc_tm_test, 0, sizeof(struct rtc_time));
	brite_rtc_gettime(&rtc_tm_test);

	/*display rtc time*/
	display_rtc_time(&rtc_tm_test, mode);

	/* 2- set alarm1 with int*/
	/*enable interrupt*/
	brite_rtc_set_int_mask(~RTC_ALM1_EN);
	brite_rtc_clear_int(0x3);
	rtc_int_counter = 0;

	rtc_wkalrm_test.enabled = 1;
	if(trigger_mode != 1)
	{
		rtc_time_to_tm(test_time+3, alm_tm);
		alm_tm->tm_100thsec = thsec100;
	}
	else
	{
		rtc_time_to_tm(test_time, alm_tm);	
		alm_tm->tm_100thsec += thsec100;
		if (rtc_get_multi_trigger_mode(argv[9], &trigger_mode, alm_tm))
			return -1;
	}	

	brite_rtc_setalarm(&rtc_wkalrm_test,trigger_mode);
	alarm_enable(1,1);

	/*wait alarm int generated*/
	while (1) {
		if (console_check_char('q'))
			break;
		if (rtc_int_counter > 0)
			break;
	}

	if (rtc_int_counter == 0)
		fail++;

	if (fail)
		info("%s: test fail.\n",argv[0]);
	else
	{
		info("%s: test pass.\n",argv[0]);		
		brite_rtc_get_trigger_mode_multi();
	}

	/*disable interrupt*/
	irq_generic_disable_int(INT_RTC);
	irq_generic_remove_handler(INT_RTC,NULL,NULL);
	brite_rtc_set_int_mask(0xff);
	alarm_enable(1, 0);

	return 0;
}


//************************************************************************************************************
// RTC interrupt disable
//************************************************************************************************************
static int rtc_int_disable(int argc, char* argv[])
{
	/*disable interrupt*/
	irq_generic_disable_int(INT_RTC);
	irq_generic_remove_handler(INT_RTC,NULL,NULL);
	brite_rtc_set_int_mask(0xff);
}

//************************************************************************************************************
// RTC interrupt for DSP
//************************************************************************************************************
static int rtc_int_for_dsp(int argc, char* argv[])
{
	int sec, min, hour, week, day, cen_month, year;
#ifdef TAPEOUT_V2
	int thsec100;
#endif
	int trigger_mode;
	int ret = 0;
	struct rtc_time *alm_tm = &rtc_wkalrm_test.time;
	u32 test_time=0;
	RTC_MODE_HOUR12_24 mode;
	int fail = 0;

	info("rtc alarm interrupt for dsp, press 'q' to quit.\n");
#ifdef TAPEOUT_V2
	if (argc < 10) {
		info("wrong cmd_format: %s mode year month day hour min sec 100thsec trigger_mode.\n"
				"mode: am|pm|24h\n"
				"trigger_mode: y/m/d/H/M/S/T => year/month/day/Hour/Minute/Second/100thSec\n",
				argv[0]);
		return 0;
	}
#else
	if (argc < 9) {
		info("wrong cmd_format: %s mode year month day hour min sec trigger_mode.\n"
				"mode: am|pm|24h\n"
				"trigger_mode: y/m/d/H/M/S => year/month/day/Hour/Minute/Second\n",
				argv[0]);
		return 0;
	}
#endif
	year = get_arg_ulong(argv[2]);
	cen_month = get_arg_ulong(argv[3]);
	day = get_arg_ulong(argv[4]);
	hour = get_arg_ulong(argv[5]);
	min = get_arg_ulong(argv[6]);
	sec = get_arg_ulong(argv[7]);
#ifdef TAPEOUT_V2
	thsec100 = get_arg_ulong(argv[8]);
#endif
	if (rtc_get_mode_hour(argv[1], true, &mode, &hour))
	    return -1;
#ifdef TAPEOUT_V2
	if (rtc_get_trigger_mode(argv[9], &trigger_mode, NULL))
		return -1;

	info("%d/%d/%d  %d:%d:%d:%d\n", year, cen_month, day, hour, min, sec, thsec100);
#else
	if (rtc_get_trigger_mode(argv[8], &trigger_mode, NULL))
		return -1;

	info("%d/%d/%d	%d:%d:%d\n", year, cen_month, day, hour, min, sec);
#endif
	memset(&rtc_wkalrm_test, 0, sizeof(struct rtc_wkalrm ));
#ifdef TAPEOUT_V2
	ret = check_time(year, cen_month, day, hour, min, sec, thsec100);
#else
	ret = check_time(year, cen_month, day, hour, min, sec);
#endif
	if(ret < 0)
	{
		info("invalid date time, please input right time.\n");
		return 0;
	}

#if 0
	/*register int handler and enable it*/
	irq_generic_install_handler(INT_RTC, rtc_int_handler, NULL);
	irq_generic_config_int(INT_RTC, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(INT_RTC);
#endif
	/* init system rtc*/
	brite_rtc_init(RTC_BASE,mode);
	test_time = mktime(year, cen_month, day, hour, min, sec);
	rtc_time_to_tm(test_time, &rtc_tm_test);
#ifdef TAPEOUT_V2
	rtc_tm_test.tm_100thsec = thsec100;
#endif
	/*set rtc time*/
	brite_rtc_settime(&rtc_tm_test);

	/*enable interrupt*/
	brite_rtc_set_int_mask(~RTC_ALM1_EN);
	brite_rtc_clear_int(0x3);

	/*set and enable alarm 1*/
	rtc_wkalrm_test.enabled = 1;
	rtc_time_to_tm(test_time, alm_tm);
#ifdef TAPEOUT_V2
	alm_tm->tm_100thsec = thsec100;
	if (rtc_get_trigger_mode(argv[9], &trigger_mode, alm_tm))
#else
	if (rtc_get_trigger_mode(argv[8], &trigger_mode, alm_tm))
#endif
		return -1;

	brite_rtc_setalarm(&rtc_wkalrm_test, (1<<trigger_mode));
	alarm_enable(1, 1);

	/*wait alarm int generated*/
	while (1) {
		if (console_check_char('q'))
			break;
	}

#if 0
	/*disable interrupt*/
	irq_generic_disable_int(INT_RTC);
	irq_generic_remove_handler(INT_RTC,NULL,NULL);
#endif
	brite_rtc_set_int_mask(0xff);
	alarm_enable(1, 0);

	return 0;
}

//************************************************************************************************************
// RTC calibrate test
//************************************************************************************************************
static int rtc_calibrate(int argc, char *argv[])
{
	int32_t fail = 0;	
	E_RTC_TRIM_MODE trim_mode;
	E_RTC_TRIM_VLD_MODE vld_mode;
	int8_t cur_trim_val, trim_val[2] = {0, 0};
	uint32_t i, trim_time, trim_avg[2], trim_mode_cycle;
	int32_t trim_diff, trim_avg_diff;
	uint32_t timer_ns, clk_ratio;

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 4) 
	{
		info("wrong cmd_format: %s timer_id trim_mode trim_val\n", argv[0]);
		info("trim_mode: 0:60s, 1:30s, 2:15s, 3:6s\n");
		info("trim_val: [-128, 127]\n");
		return 1;
	}

	// Get the parameters	
	trim_mode = get_arg_ulong(argv[2]);
	if (trim_mode > RTC_TRIM_06S)
	{
		trim_mode = RTC_TRIM_06S;
	}
	
	vld_mode = trim_mode;

	if 		(trim_mode == RTC_TRIM_60S)	trim_time = 60;
	else if (trim_mode == RTC_TRIM_30S)	trim_time = 30;
	else if (trim_mode == RTC_TRIM_15S)	trim_time = 15;
	else								trim_time = 6;

	cur_trim_val = get_arg_ulong(argv[3]);

	brite_rtc_init(RTC_BASE, RTC_MODE_HOUR12);
	
	// Set pinmux
	misc_rtc_clkout_pin_init(IO_PU_UP, IO_PD_EN);
	misc_rtc_cal_clkout_pin_init(IO_PU_UP, IO_PD_EN);
	misc_clkout_pin_init(IO_PU_UP, IO_PD_EN);

	trim_val[1] = cur_trim_val;
	for (i = 0; i < 2; i++)
	{
		// trim scaning
		info("\ntrim_mode:%d seconds, vld_mode:%d, trim_val:%d\n", trim_time, vld_mode, trim_val[i]);
		
		// rtc configuration
		rtc_trim_vld_ctrl(RTC_TRIM_VLD_DIS);
		rtc_trim_ctrl(RTC_TRIM_DIS);		
		rtc_write_trim(trim_val[i]);
		rtc_write_trim_mode(trim_mode, vld_mode);
		rtc_trim_vld_ctrl(RTC_TRIM_VLD_EN);
		rtc_trim_ctrl(RTC_TRIM_EN);

		if (timer_rtc_capture(argc, argv, trim_time, &timer_ns, &trim_avg[i]))
		{
			return 1;
		}

		info("trim_avg:0x%x\n", trim_avg[i]);
	}

	clk_ratio = (1000000000/OSC_32KHZ_PCLK)/timer_ns;
	
	if (cur_trim_val != 0)
	{
		trim_diff = (int32_t)(((int32_t)(trim_avg[1] - trim_avg[0]))/cur_trim_val);
		
		if ((trim_diff < (clk_ratio - 1)) || (trim_diff > (clk_ratio + 1)))
		{
			fail++;
		}
	}
	else
	{
		trim_diff = (int32_t)(((int32_t)(trim_avg[1] - trim_avg[0]))/1);
		if ((trim_diff < -10) || (trim_diff > 10))
		{
			fail++;
		}
	}

	trim_mode_cycle = cb_lldiv((trim_time*1000000*(TIMER_PCLK/1000000)), 32);
	trim_avg_diff = (int32_t)(trim_avg[1] - trim_mode_cycle);
	
	info("trim_diff:%d, trim_avg_diff:%x\n", trim_diff, trim_avg_diff);
	info("clk_ratio:0x%x, trim_mode_cycle:0x%x\n", clk_ratio, trim_mode_cycle);
	
	if (fail) 
	{
		info("%s: test fail\n", argv[0]);
	} 
	else 
	{
		rtc_trim_ctrl(RTC_TRIM_EN);
		rtc_trim_vld_ctrl(RTC_TRIM_VLD_EN);
		info("%s: test pass\n", argv[0]);
	}

	return 0;
}

//************************************************************************************************************
// Show rtc calibration parameter
//************************************************************************************************************
static int rtc_calibration_para_show(int argc, char *argv[])
{
	uint32_t trim_val = rtc_read_trim();
	uint32_t trim_mode = rtc_read_trim_mode();

	info("rtc_calibration_para_show---\n");
	info("trim_mode:0x%x, trim_val:0x%x\n", trim_mode, trim_val);

	return 0;
}

//************************************************************************************************************
// Set rtc trim test
//************************************************************************************************************
static int rtc_set_trim(int argc, char *argv[])
{
	int32_t i, min, max, diff, tmp_trim_val[4], mod[4], trim_val;
	uint32_t fx;
	E_RTC_TRIM_MODE min_trim_mode, max_trim_mode, trim_mode;
	E_RTC_TRIM_VLD_MODE vld_mode;
	
	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s crystal_clk(x100HZ)\n", argv[0]);
		info("crystal clock(HZ)x100\n");
		info("e.g. measured crystal clock = 32766.57HZ, command should be rtc_set_trim 3276657\n");
		return 1;
	}

	fx = get_arg_ulong(argv[1]);

	diff = fx - (OSC_32KHZ_PCLK * 100);
	tmp_trim_val[0] = diff * 60;
	tmp_trim_val[1] = diff * 30;
	tmp_trim_val[2] = diff * 15;
	tmp_trim_val[3] = diff * 6;

	info("trim_val:%d, %d, %d, %d\n", tmp_trim_val[0], tmp_trim_val[1], tmp_trim_val[2], tmp_trim_val[3]);

	for (i = 0; i < 4; i++)
	{
		mod[i] = tmp_trim_val[i] % 100;	

		if (mod[i] < 0)
		{
			mod[i] = 0 - mod[i];
		}
	}

	info("mod:%d, %d, %d, %d\n", mod[0], mod[1], mod[2], mod[3]);

	// look for the minimum
	min = mod[0];
	min_trim_mode = 0;
	for (i = 0; i < 4; i++)
	{
		if (min > mod[i])
		{
			min = mod[i];
			min_trim_mode = i;
		}
	}

	// look for the maximum
	max = mod[0];
	max_trim_mode = 0;
	for (i = 0; i < 4; i++)
	{
		if (max < mod[i])
		{
			max = mod[i];
			max_trim_mode = i;
		}
	}

	info("min:%d, max:%d\n", min, max);

	trim_mode = (min < (100 - max)) ? min_trim_mode : max_trim_mode;

	vld_mode = trim_mode;
	trim_val = (tmp_trim_val[trim_mode] + (tmp_trim_val[trim_mode] % 100)) / 100;

	info("trim_mod:%d, cur_trim_val:%d\n", trim_mode, trim_val);

	brite_rtc_init(RTC_BASE, RTC_MODE_HOUR12);
	
	// Set pinmux
	misc_rtc_clkout_pin_init(IO_PU_UP, IO_PD_EN);
	misc_rtc_cal_clkout_pin_init(IO_PU_UP, IO_PD_EN);

	// rtc configuration
	rtc_trim_vld_ctrl(RTC_TRIM_VLD_DIS);
	rtc_trim_ctrl(RTC_TRIM_DIS);	
	rtc_write_trim(trim_val);
	rtc_write_trim_mode(trim_mode, vld_mode);
	rtc_trim_vld_ctrl(RTC_TRIM_VLD_EN);
	rtc_trim_ctrl(RTC_TRIM_EN);
	
	info("%s: pass\n", argv[0]);

	return 0;
}

int rtc_test_prepare(int sel)
{
	// Set pinmux
	misc_rtc_clkout_pin_init(IO_PU_UP, IO_PD_EN);
	misc_rtc_cal_clkout_pin_init(IO_PU_UP, IO_PD_EN);

	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry rtc_test_menu[] = {
	{"help",  				cmd_help, 		 	"                : Display list of commands"},
	{"h",					cmd_help, 			"                   : alias for help"},
	{"?",					cmd_help, 			"                   : alias for help"},
	{"rtc_set_time",		rtc_set_time, 		"        : rtc_set_time"},
	{"rtc_get_time",		rtc_get_time, 		"        : rtc_get_time"},
	{"rtc_get_time_robust",	rtc_get_time_robust," : rtc_get_time robust test"},
	{"rtc_alarm",			rtc_alarm, 		  	"           : test_alarm"},
	{"rtc_alarm_multi_mode",rtc_alarm_multi_mode," : test_alarm with multi mode"},
	{"rtc_int",				rtc_int, 	 		"             : alarm interrupt test for arm"},
	{"rtc_int_for_dsp",		rtc_int_for_dsp,	"     : alarm interrupt generate for dsp"},
	{"rtc_calibrate",		rtc_calibrate,		"       : rtc calibrate"},
	{"rtc_calibrate_para_show",		rtc_calibration_para_show,		"		: show rtc calibrate mode and value"},	
	{"rtc_set_trim",		rtc_set_trim,		"       : rtc set trim"},
	{0, 					0, 					0}
};

#endif

