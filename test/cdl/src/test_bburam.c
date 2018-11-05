/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#include "test_config.h"

#if (CONFIG_BBURAM_TEST == 1)
#include "cdl_bburam.h"

cmdline_entry bburam_test_menu[];

struct brite_bram bram;
struct brite_bram *p_bram = &bram;

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&bburam_test_menu[0]);
}

static int bram_writes_test(int argc, char* argv[])
{
	u8 pattern;
	int random_flag = 1;
	u32 length, offset;
	int i;
	u8 *buf = test_recv_buff;

	brite_bram_init(p_bram, BBU_RAM_BASE);
	info("BBU RAM write multi-byte test\n");

	if (argc < 3)
	{
		info("wrong cmd format: %s offset byte_length [pattern(e.g. 0xaa or 0x55)]\n", argv[0]);
		return 0;
	}

	offset = get_arg_ulong(argv[1]);
	if (offset < 0 || offset > 1023)
	{
		info("offset input range is: 0 ~ 1023\n");
		return 0;
	}	
	length = get_arg_ulong(argv[2]);
	if (length < 1 || length > 1024 || (length + offset) > 1024)
	{
		info("length or offset+length is out of range : 1 ~ 1024\n");
		return 0;
	}

	if (argc >= 4)
	{
		pattern = get_arg_ulong(argv[3]) & 0xff;
		for(i=0; i<length; i++)
			*(buf+i) = pattern;
	}
	else
	{
		srand(get_ticks());
		for(i=0; i<length; i++)
			*(buf+i) = cb_rand() & 0xff;;	
	}

	brite_bram_writebs(p_bram, offset, length, buf);
	info("write done \r\n");

	return 0;
}

static int bram_reads_test(int argc, char* argv[])
{
	u32 length;
	u32 offset;
	u8 *buf = test_recv_buff;
	int i;

	brite_bram_init(p_bram, BBU_RAM_BASE);
	info("BBU RAM read multi-byte test\n");

	if (argc < 3)
	{
		info("wrong cmd format: %s off byte_length\n", argv[0]);
		return 0;
	}

	offset = get_arg_ulong(argv[1]);
	if (offset < 0 || offset > 1023)
	{
		info("offset input range is: 0 ~ 1023\n");
		return 0;
	}	
	length = get_arg_ulong(argv[2]);
	if (length < 1 || length > 1024 || (length + offset) > 1024)
	{
		info("length or offset+length is out of range : 1 ~ 1024\n");
		return 0;
	}

    brite_bram_readbs(p_bram, offset, length, buf);
	info("BBU RAM Read Result as below:\n");
	for(i=0; i < length; i++)
	{
		if ((!(i % 16)) && (i !=0))
			info("\n");
		info("%x  ", buf[i]);
	}
	info("\nTotal %d bytes displayed", length);

	return 0;
}

static int bram_data_compare(int argc, char* argv[])
{
	u8 pattern;
	u8 *wbuf = test_send_buff;
	u8 *rbuf = test_recv_buff;
	int i;

	brite_bram_init(p_bram, BBU_RAM_BASE);
	info("BBU RAM data 1KB W/R compare test\n");

	if (argc < 2)
	{
__para_err:	
		info("wrong cmd format: %s random_flag [pattern]\n", argv[0]);
		return 0;
	}

	if (get_arg_ulong(argv[1]) == 0)
	{
		if (argc < 3)
			goto __para_err;
		
		pattern = get_arg_ulong(argv[2]);
		for(i=0; i<1024; i++)
			wbuf[i] = pattern;
	}
	else
	{
		srand(get_ticks());
		for(i=0; i<1024; i++)
			wbuf[i] = cb_rand()&0xff; 
	}

	brite_bram_writebs(p_bram, 0, 1024, wbuf);
	brite_bram_readbs(p_bram, 0, 1024, rbuf);
	for(i=0; i < 1024; i++)
	{
		if (wbuf[i] != rbuf[i])
		{
			info("%s: test fail @ %d\n", argv[0], i);
			return 0;
		}
	}

	info("%s: test pass\n", argv[0]);

	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry bburam_test_menu[] = {
	{ "help", cmd_help, 		 				" : Display list of commands" },
	{ "h",	cmd_help, 							"    : alias for help" },
	{ "?",	cmd_help, 							"    : alias for help" },
	{ "bram_write_buf", bram_writes_test, 		"   : BBU RAM write string test"  },
	{ "bram_read_buf", bram_reads_test, 		"   : BBU RAM read string test"  },	
	{ "bram_data_comp", bram_data_compare, 		"   : BBU RAM data compare test"  },
	{ 0, 0, 0 }
};

#endif

