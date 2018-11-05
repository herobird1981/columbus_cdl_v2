/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"

#if (CONFIG_TAMP_TEST == 1)
#include "cdl_tamp.h"

#define TAMP_TIEM_NUM 3

struct tamp_time tamp_tm_test[TAMP_TIEM_NUM];

cmdline_entry tamp_test_menu[];

static void display_tamp_time(struct tamp_time *tm, u8 hw_mode, u8 disp_mode)
{
	u32 tm_hour;
	u8 am_pm = 0; /*0 am 1 pm*/

	tm_hour = tm->tm_hour;
	info("hw_mode:0x%x, disp_mode:0x%x\n", hw_mode, disp_mode);
	/*hardware setting using 24 hour mode*/
	if(hw_mode & Tx_HR12_24) {
		/*display using 12 hour mode*/
		if (disp_mode != TAMP_MODE_HOUR24) {
			if (tm->tm_hour > 12) {
				tm_hour = tm->tm_hour - 12;
				am_pm = 1;
			}
		}
	} else { /*hardware setting using 12 hour mode*/
		am_pm = (hw_mode & Tx_H20_PA) ? TAMP_HOUR_PM : TAMP_HOUR_AM;
		/*display using 24 hour mode*/
		if (disp_mode != TAMP_MODE_HOUR12) {
			if (am_pm == TAMP_HOUR_PM) {
				if (tm->tm_hour == 12)
				{
					tm_hour = tm->tm_hour;
				}
				else
				{
					tm_hour = tm->tm_hour + 12;
				}
			}
			else{
				if (tm->tm_hour == 12)
				{
					tm_hour = tm->tm_hour - 12;
				}
			}
		}
	}

	if(disp_mode == TAMP_MODE_HOUR24)
	{
		info("tamp time %04u.%02u.%02u %02u:%02u:%02u\n",
			 tm->tm_year, tm->tm_mon, tm->tm_day,
			 tm_hour, tm->tm_min, tm->tm_sec);
	}
	else
	{
		info("tamp time %04u.%02u.%02u %02u:%02u:%02u %s\n",
			tm->tm_year, tm->tm_mon, tm->tm_day,
			tm_hour, tm->tm_min, tm->tm_sec,
			am_pm?"pm":"am");
	}
}

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&tamp_test_menu[0]);
}


#if (CONFIG_DRIVERS_GIC == 1)
static int tamp_int_counter = 0;

void tamp_int_handler(void* ptr)
{
	u8 int_status = 0;

	int_status = brite_tamp_get_irq_status();
	info("tamp_int_handler start, status=%x\n", int_status);
	if (int_status) {
		tamp_int_counter++;
		brite_tamp_clear_irq(int_status);
	}
	info("tamp_int_handler end, status=%x\n", int_status);
}
#endif

static int tamp_int(int argc, char* argv[])
{
#if (CONFIG_DRIVERS_GIC == 1)
	u8 event_count;
	u8 hour[TAMP_TIEM_NUM];
	char cc;
	u8 det_mode, deb_time, display_mode;
	u8 i, id;
	int fail = 0;
	int int_type;
	info("tamp_int, press 'q' before interrup to quit\n");

	if (argc < 4) {
		info("wrong cmd_format: %s det_mode[0-4] deb_time[1-3] display_mode[0:12h, 1:24h]\n", argv[0]);
		return 0;
	}
	det_mode = (u8)get_arg_ulong(argv[1]);
	deb_time = (u8)get_arg_ulong(argv[2]);
	display_mode = (u8)get_arg_ulong(argv[3]);
	if(det_mode > 4) {
		info("invalid detect mode, please input right det_mode[0-4].\n");
		return 0;
	}
	if(deb_time > 3) {
		info("invalid De-bounce time setting, please input right value[0-3].\n");
		return 0;
	}

	if (display_mode)
		display_mode = TAMP_MODE_HOUR24;
	info("det_mode:%u, deb_time:%u, display_mode:%u\n", det_mode, deb_time, display_mode);
	switch (det_mode) {
	case 0:
		int_type = IRQ_TYPE_EDGE_RISING;
		break;
	case 1:
		int_type = IRQ_TYPE_EDGE_FALLING;
		break;
	case 2:
		int_type = IRQ_TYPE_EDGE_BOTH;
		break;
	case 3:
		int_type = IRQ_TYPE_LEVEL_HIGH;
		break;
	case 4:
		int_type = IRQ_TYPE_LEVEL_LOW;
		break;
	default:
		int_type = IRQ_TYPE_LEVEL_HIGH;
		break;
	}

	/* init system tamp*/
	brite_tamp_init(TAMPER_BASE);
	/*disable tampering detector function */
	brite_tamp_enable(0);

	/*get default tamp time and display*/
	memset(&tamp_tm_test[0], 0, (sizeof(struct tamp_time) * TAMP_TIEM_NUM));
	for (i = 0; i < TAMP_TIEM_NUM; i++) {
		id = i + 1;
		brite_tamp_gettime(id, &tamp_tm_test[i]);

		hour[i] = brite_tamp_get_hour(id);
		info("\ndefault tamp time%u stamp:\n", id);
		display_tamp_time(&tamp_tm_test[i], hour[i], display_mode);
	}

	/* 2- configure int*/
	/*register int handler and enable it*/
	irq_generic_install_handler(INT_TAMPER, tamp_int_handler, NULL);
	irq_generic_config_int(INT_TAMPER, int_type);
	irq_generic_enable_int(INT_TAMPER);

	/*enable tampering detector function */
	brite_tamp_enable(1);

	/*enable and clear int status*/
	brite_tamp_enable_irq(TAMP_IRQ_EN);
	brite_tamp_clear_irq(TAMP_IRQ_CLR);

	/*configure event*/
	brite_tamp_set_config(det_mode, deb_time);

	tamp_int_counter = 0;
	info("\nwait interrupt...\n\n");
	/*wait tamp int generated*/
	while (1) {
		if (console_tstc()) {
			cc = console_getc();
			if (cc == 'q') {
				break;
			}
		}
		if (tamp_int_counter > 0)
			break;
	}
	if (tamp_int_counter == 0) {
		fail++;
	}

	/*disable interrupt*/
	irq_generic_disable_int(INT_TAMPER);
	irq_generic_remove_handler(INT_TAMPER,NULL,NULL);
	info("*********************************\n");

	/*get after irq tamp time and display*/
	memset(&tamp_tm_test[0], 0, (sizeof(struct tamp_time) * TAMP_TIEM_NUM));
	for (i = 0; i < TAMP_TIEM_NUM; i++) {
		id = i + 1;
		brite_tamp_gettime(id, &tamp_tm_test[i]);

		hour[i] = brite_tamp_get_hour(id);
		info("\ncurrent tamp time%u stamp:\n", id);
		display_tamp_time(&tamp_tm_test[i], hour[i], display_mode);
	}

	event_count = brite_tamp_get_event_count();
	info("event_count:%u, int_status:0x%x\n", event_count, brite_tamp_get_irq_status());
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return fail;
#else
	info("tamp_int : interrupt handle test can't be test for no GIC driver support!\n");
#endif
	return 0;
}

static int tamp_read(int argc, char* argv[])
{
	u8 event_count;
	u8 hour[TAMP_TIEM_NUM];
	char cc;
	u8 display_mode;
	u8 i, id;

	if (argc < 2) {
		info("wrong cmd_format: %s display_mode[0:12h, 1:24h]\n", argv[0]);
		return 0;
	}
	display_mode = (u8)get_arg_ulong(argv[1]);

	if (display_mode)
		display_mode = TAMP_MODE_HOUR24;
	info("display_mode:%u\n", display_mode);

	/* init system tamp*/
		brite_tamp_init(TAMPER_BASE);

	/*get tamp time and display*/
	memset(&tamp_tm_test[0], 0, (sizeof(struct tamp_time) * TAMP_TIEM_NUM));
	for (i = 0; i < TAMP_TIEM_NUM; i++) {
		id = i + 1;
		brite_tamp_gettime(id, &tamp_tm_test[i]);

		hour[i] = brite_tamp_get_hour(id);
		info("\ncurrent tamp time%u stamp:\n", id);
		display_tamp_time(&tamp_tm_test[i], hour[i], display_mode);
	}

	event_count = brite_tamp_get_event_count();
	info("\nevent_count: %u\n", event_count);
	return 0;
}

static int tamp_count_clear(int argc, char* argv[])
{
	/*clear event count*/
	brite_tamp_clear_event_count(TAMP_COUNT_CLR);
	info("Event count cleared!\n");
	return 0;
}


//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry tamp_test_menu[] = {
	{ "help", cmd_help, 		 		" : Display list of commands" },
	{ "h",	cmd_help, 				"    : alias for help" },
	{ "?",	cmd_help, 				"    : alias for help" },
	{ "tamp_int", tamp_int, "   : tamp interrupt test"  },
	{ "tamp_read", tamp_read, "   : tamp read test"  },
	{ "tamp_count_clear", tamp_count_clear, "   : clear event count"  },
	{ 0, 0, 0 }
};

#endif

