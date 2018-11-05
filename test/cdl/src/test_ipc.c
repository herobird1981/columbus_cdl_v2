/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_ipc.h"

#if (CONFIG_IPC_TEST == 1)

cmdline_entry ipc_test_menu[];

extern struct a7_ipc* p_ipc_dev;

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&ipc_test_menu[0]);
}

void a7_ipc_init(struct a7_ipc *priv, unsigned long reg_base, unsigned long ram_base)
{
	int i = 0;
	
	priv->reg_base = (void*)reg_base;
	priv->ram_base = (void*)ram_base;

	for(i=0; i<8; i++){
       priv->int_rf[i] = INT_IPC_RF0 + i;
	   priv->int_plc[i] = INT_IPC_PLC0 + i;
	}
}

/**
 * @brief ipc_set_msg - ARM sends and receives back data to/from DSP by polling mode.
 * @param mode input, 0:RF 1:PLC
 * @param flag input, IPC handshake channel, should be 0-15
 * @param len input, data length in bytes, should be 0-1024
 * @param [pattern] input, fill data buffer with pattern or random data
 *
 * @return directly if failed
 */
static int ipc_set_msg(int argc, char* argv[])
{
	int mode, flag, ret, fail = 0, len, i, lens = 0;
	u32 addr;
	char cc;
	int cf = 0, cs=0;
	unsigned long pattern;
	int random_flag = 1;

	info("ipc_set_msg\n");

	if (argc < 4) {
		info("wrong cmd_format: %s mode(1=plc, 0=rf) flag(0-15) len(0-1024) [pattern]\n", argv[0]);
		return 0;
	}

	mode = get_arg_ulong(argv[1]);
	flag = get_arg_ulong(argv[2]);
	len = get_arg_ulong(argv[3]);
	if (argc > 4) {
		pattern = get_arg_ulong(argv[4]);
		random_flag = 0;
	}

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);	

	if (len > 1024 || flag > 15 || mode > 1 || mode < 0 || flag < 0){
		info("wrong cmd_format: %s mode(1=plc, 0=rf) flag(0-15) len(0-1024)\n", argv[0]);
		return 0;
	}
	while (1) {
		fail = 0;
		if (random_flag) {
			for (i = 0; i < len; i++) {
				test_send_buff[i] = cb_rand() & 0xff;
			}
		} else {
			info("Test IPC set message with specified test pattern: 0x%x\n", pattern);
			for (i = 0; i < len; i++) {
				test_send_buff[i] = pattern & 0xff;
			}
		}
		memset(test_recv_buff, 0x0, sizeof(test_recv_buff));

		if(mode == 0) {
			ret = ipc_set_msg_a7torf(p_ipc_dev, flag, (u32 *)test_send_buff, len);
			if(ret < 0){
				fail++;
			}
			info("ipc_set_msg_a7torf:input 'q' to quit this testcase\n");
			while(1)
			{
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q') {
						break;
					}
				}
				if(flag == 15)
					ret = ipc_get_msg_rftoa7(p_ipc_dev, flag - 1, (u32 *)test_recv_buff, &lens);
				else
					ret = ipc_get_msg_rftoa7(p_ipc_dev, flag + 1, (u32 *)test_recv_buff, &lens);
				if(ret == 1){
					break;
				}
			}
		}else if(mode == 1){
			ret = ipc_set_msg_a7toplc(p_ipc_dev, flag, (u32 *)test_send_buff, len);
			if (ret < 0) {
				fail++;
			}

			info("ipc_set_msg_a7toplc:input 'q' to quit this testcase\n");
			while(1)
			{
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q') {
						break;
					}
				}
				if(flag == 15)
					ret = ipc_get_msg_plctoa7(p_ipc_dev, flag - 1, (u32 *)test_recv_buff, &lens);
				else
					ret = ipc_get_msg_plctoa7(p_ipc_dev, flag + 1, (u32 *)test_recv_buff, &lens);
				if(ret == 1){
					break;
				}
			}
		}

		/*check result*/
		for (i = 0; i < len; i++) {
			if (test_send_buff[i] != test_recv_buff[i]) {
				debug(
					"ipc set msg check data fail @%d: expected %x receive %x\n",
					i,
					test_send_buff[i],
					test_recv_buff[i]);
					fail++;
					break;
			}
		}

		if(fail > 0){
			cf++;
			info("%s : test fail.\n",argv[0]);
		}else{
			cs++;
			info("%s : test pass.\n",argv[0]);
		}
		info("%d pass, %d fail\n",cs, cf);
		if((cs+cf) == 100)
		{
			return 0;
		}
	}
	return 0;
}

/**
 * @brief ipc_get_msg - ARM receives and sends back data from/to DSP by polling mode.
 * @param mode input, 0:RF 1:PLC
 * @param flag input, IPC handshake channel, should be 0-15
 *
 * @return directly if failed
 */
static int ipc_get_msg(int argc, char* argv[])
{
	int mode, flag, ret, fail = 0, lens = 0;
	u32 addr;
	char cc;
	int cs=0,cf =0;
	info("ipc_get_msg\n");

	if (argc < 3) {
		info("wrong cmd_format: %s mode(1=plc, 0=rf) flag(0-15)\n", argv[0]);
		return 0;
	}

	mode = get_arg_ulong(argv[1]);
	flag = get_arg_ulong(argv[2]);

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	while(1)
	{
		fail = 0;
		memset(test_recv_buff, 0x0, sizeof(test_recv_buff));
		if(mode == 0){
			info("ipc_get_msg_rftoa7:input 'q' to quit this testcase\n");
			while(1)
			{
				ret = 0;
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q') {
						if(ret != 1){
							fail++;
						}
						break;
					}
				}
				ret = ipc_get_msg_rftoa7(p_ipc_dev, flag, (u32 *)test_recv_buff, &lens);
				if(ret == 1){
					break;
				}
			}
			if(flag == 15)
				ret = ipc_set_msg_a7torf(p_ipc_dev, flag-1, (u32 *)test_recv_buff, lens);
			else
				ret = ipc_set_msg_a7torf(p_ipc_dev, flag+1, (u32 *)test_recv_buff, lens);
			if(ret<0){
				fail++;
			}
		}else if(mode == 1){
			info("ipc_get_msg_plctoa7:input 'q' to quit this testcase\n");
			while(1)
			{
				ret = 0;
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q') {
						if(ret != 1){
							fail++;
						}
						break;
					}
				}
				ret = ipc_get_msg_plctoa7(p_ipc_dev, flag, (u32 *)test_recv_buff, &lens);
				if(ret == 1){
					break;
				}
			}
			//info("lens = %d\n", lens);
			if(flag == 15)
				ret = ipc_set_msg_a7toplc(p_ipc_dev, flag-1, (u32 *)test_recv_buff, lens);
			else
				ret = ipc_set_msg_a7toplc(p_ipc_dev, flag+1, (u32 *)test_recv_buff, lens);
			if(ret<0){
				fail++;
			}
		}else{
			info("no this mode.\n");
			return 0;
		}

		if(fail > 0){
			cf++;
			info("%s : test fail.\n",argv[0]);
		}else{
			cs++;
			info("%s : test pass.\n",argv[0]);
		}
		info("%d pass, %d fail\n",cs, cf);
		if((cs+cf) == 100)
		{
			return 0;
		}
	}
	return 0;
}

static int ipc_int_counter = 0;

void a7_ipc_int_rf(void* para)
{
	int int_status = 0;
	int ret=0, lens=0, flag=*((int*)para);
	unsigned long long time=0L;

	int_status = a7_ipc_get_int_status_rf(p_ipc_dev);
	if (int_status & (1 << flag))
	{
		time = ipc_get_time(p_ipc_dev);
		info("rf event 0 @ time %llx\n",time);
		ipc_int_counter++;
		memset(test_recv_buff, 0x0, sizeof(test_recv_buff));
		ipc_get_msg_rftoa7(p_ipc_dev, flag, (u32 *)test_recv_buff, &lens);
		ret = ipc_set_msg_a7torf(p_ipc_dev, flag + 1, (u32 *)test_recv_buff, lens);
		a7_ipc_clear_rf_int(p_ipc_dev, flag);
	}
}

void a7_ipc_int_plc(void* para)
{
	int int_status = 0;
	int ret=0, lens=0, flag=*((int*)para);
	unsigned long long time=0L;

	int_status = a7_ipc_get_int_status_plc(p_ipc_dev);
	if (int_status & (1 << flag))
	{
		time = ipc_get_time(p_ipc_dev);
		info("plc event 0 @ time %llx\n",time);
		ipc_int_counter++;
		memset(test_recv_buff, 0x0, sizeof(test_recv_buff));
		ipc_get_msg_plctoa7(p_ipc_dev, flag, (u32 *)test_recv_buff, &lens);
		ret = ipc_set_msg_a7toplc(p_ipc_dev, flag + 1, (u32 *)test_recv_buff, lens);
		a7_ipc_clear_plc_int(p_ipc_dev, flag);
	}
}

/**
 * @brief ipc_int - ARM receives and sends back data to/from DSP by interrupting mode.
 * @param mode input, 0:RF 1:PLC
 * @param flag input, IPC handshake channel, should be 0-7
 *
 * @return directly if failed
 */
static int ipc_int(int argc, char* argv[])
{
	int mode=0, flag=0, ret=0;
	int int_vector=0;
	char cc = 0;

	if (argc < 3) {
		info("wrong cmd_format: %s mode(1=plc, 0=rf) flag(0-7)\n", argv[0]);
		return 0;
	}
	mode = get_arg_ulong(argv[1]);
	flag = get_arg_ulong(argv[2]);
	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	if(mode == 0)
	{
		info("ipc_int_rf\n");
		int_vector = flag + INT_IPC_RF0;
		/*register int handler and enable it*/
		irq_generic_install_handler(p_ipc_dev->int_rf[flag], a7_ipc_int_rf, &flag);	
	}
	else if(mode == 1)
	{
		info("ipc_int_plc\n");
		int_vector = flag + INT_IPC_PLC0;
		/*register int handler and enable it*/
		irq_generic_install_handler(p_ipc_dev->int_plc[flag], a7_ipc_int_plc, &flag);			
	}
	else
	{
		info("no this mode.\n");
		return 0;
	}
	irq_generic_config_int(int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(int_vector);

	while(1){
		if (console_tstc()){
			cc = console_getc();
			if (cc == 'q'){
				break;
			}
		}
	}

	/*disable interrupt*/
	irq_generic_disable_int(int_vector);
	irq_generic_remove_handler(int_vector,NULL,NULL);

	return 0;
}

#ifdef TAPEOUT_V2
/**
 * @brief ipc_free_time - A 64-bit free running timer test
 * @param mode input, count mode, 0:down 1:up
 * @param [prescaler] input, Programmable prescalers from main CPU clock
 *
 * @return directly if failed
 */
static int ipc_free_time(int argc, char* argv[])
{
	int32_t fail = 0;
	E_IPC_TMR_COUNT_MODE mode = IPC_TMR_DOWN;
	unsigned long long prev_cnt=0L, cur_cnt=0L;
	uint32_t i=0;
	uint32_t prescaler = 0;

	info("%s\n", __func__);

	// Check the validity of parameter count
	if (argc < 2) 
	{
		info("wrong cmd_format: %s mode(1=up, 0=down) [prescaler]\n", argv[0]);
		return 1;
	}

	// Get the parameters
	mode = get_arg_ulong(argv[1]);
	if (argc > 2)
	{
		prescaler = get_arg_ulong(argv[2]);
		if (prescaler > IPC_TMR_COUNTUP_MAX)
		{
			info("Please input valid div value: 0 ~ 65535");
			return 1;
		}
	}

	info("ipc count mode:%s, div:0x%x\n\n", ((mode==IPC_TMR_DOWN) ? "down" : "up"), prescaler);
	
	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// Time mode can be only accessed & configured by ARM Core
	ipc_set_timer_prescaler(p_ipc_dev, prescaler);
	ipc_set_time_mode(p_ipc_dev, mode); 

	info("initial = %016llx\n", prev_cnt);
	prev_cnt = ipc_get_time(p_ipc_dev);

	for (i = 0; i < 10; i++) 
	{
		cur_cnt = ipc_get_time(p_ipc_dev);
		info("mode:%s, prescale:0x%x, time = %016llx\n", ((mode==IPC_TMR_DOWN)?"Down":"Up"), prescaler, cur_cnt);

		if (mode == IPC_TMR_DOWN) 
		{
			if (cur_cnt > prev_cnt) 
			{
				fail++;
			}
		} 
		else 
		{
			if (cur_cnt < prev_cnt) 
			{
				fail++;
			}
		}
		
		prev_cnt = cur_cnt;
	}

	if (fail) 
		info("%s: test fail\n", argv[0]);
	else 
		info("%s: test pass\n", argv[0]);
	
	return 0;
}
#else
static int ipc_free_time(int argc, char* argv[])
{
	uint32_t i = 0;
	unsigned long long cur_cnt=0L;

	info("%s\n", __func__);
	
	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	for (i = 0; i < 10; i++) 
	{
		cur_cnt = ipc_get_time(p_ipc_dev);
		info("time = %016llx\n", cur_cnt);
	}

	info("Pls Refer to RF's free timer setting(countup/countdown/prescaler) to judge ARM's free timer test results.");

	return 0;
}
#endif

#define ALIGN(n, align)	(((n)+(align)-1)&~((align)-1))

/**
 * @brief plc_boot_test - ARM loadbin PLC image to DDR and send DDR data to SHRAM,
 * PLC bootrom copy the SHRAM data to DRAM/IRAM,then boot...
 *
 * @param offset input, the PLC image DDR load address
 *
 * @return directly if failed
 */
static int plc_boot_test(int argc, char* argv[])
{
	unsigned int request_len = 0;
	unsigned int request_addr = 0;
	unsigned int i = 0;
	unsigned int plctoa7flg = 0;
	unsigned char *img_buf = NULL;
	char cc;

	if (argc < 2) {
		info("wrong cmd_format: %s offset\n", argv[0]);
		return 0;
	}    	
    img_buf = (unsigned char *)get_arg_ulong(argv[1]);

	// prepare data: fetch the data from specified DDR space
	info("arm try to fetch PLC image from\n");

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// clear pending flags
	ipc_clear_plctoa7_flag(p_ipc_dev, 0);
	// release all allocated share ram
	for (i=0; i<32; i++)
		a7_free_page(p_ipc_dev, i);
	while(1)
	{
		// waiting for request from PLC
		info("plc_boot_test:input 'q' to quit this testcase\n");
		do {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					return -1;
				}
			}
			plctoa7flg = ipc_readl(p_ipc_dev, PLCTOA7IPCSTS);
		} while(plctoa7flg == 0);


		// parse request from PLC
		if(plctoa7flg & 0x1)
			info("ipc_get_msg_plctoa7 flag %d\n", plctoa7flg);

		if(ipc_readl(p_ipc_dev,  PLCTOA7IPCCOMM) != 0x09700000)
			info("ipc get error msg from plc 0x%8x!\n", ipc_readl(p_ipc_dev, PLCTOA7IPCCOMM));

		request_addr = ipc_readl(p_ipc_dev, PLCTOA7IPCADDR);
		request_len = ipc_readl(p_ipc_dev, PLCTOA7IPCDATA0);
		if(request_len > 1024)
			request_len = 1024;
		request_len = ALIGN(request_len, 4);

		ipc_writel(p_ipc_dev, 1, PLCTOA7IPCACK );

		// apply 1k share ram and copy DDR data to share ram
		if (a7_get_sram_page(p_ipc_dev, 6) < 0) {
			info("plc_boot: a7 get sram page 6 failed\n");
			return -1;
		}

		memcpy(p_ipc_dev->ram_base + 6*IPC_PAGE_LEN,  img_buf + request_addr, request_len);
		// send ACK to PLC
		ipc_writel(p_ipc_dev, 0x09710000, A7TOPLCIPCCOMM );
		ipc_writel(p_ipc_dev, 6*IPC_PAGE_LEN, A7TOPLCIPCADDR );
		ipc_writel(p_ipc_dev, request_addr, A7TOPLCIPCDATA0 );
		ipc_writel(p_ipc_dev, request_len, A7TOPLCIPCDATA1 );
		ipc_writel(p_ipc_dev, 1, A7TOPLCIPCSET );

		// waiting for ACK from PLC
		while(ipc_readl(p_ipc_dev, A7TOPLCIPCFLG) & 0x1 == 1) {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					a7_free_page(p_ipc_dev, 6);
					return -1;
				}
			}
			info("wait ipc ack\n");
		}
		// release allocated share ram
		a7_free_page(p_ipc_dev, 6);
		//if(request_len < 1024)
		//	break;
		info("plc one package send done\n");
	}
	return 0;
}

/**
 * @brief plc_spboot_test - special case:ARM loadbin PLC image(<16K) to DDR and send DDR data to SHRAM.
 * PLC bootrom copy header data from SHRAM to DRAM and then get SHRAM ownership then boot...
 *
 * @param offset input, the PLC image DDR load address
 * @param file_len input, the PLC image length, should be < 16K
 *
 * @return directly if failed
 */
static int plc_spboot_test(int argc, char* argv[])
{
	unsigned int request_len = 0;
	unsigned int request_addr = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int plctoa7flg = 0;
	char cc;
	unsigned char *img_buf = NULL;
    		
	if (argc < 3) {
		info("wrong cmd_format: %s offset file_len\n", argv[0]);
		return 0;
	}

    img_buf = (unsigned char *)get_arg_ulong(argv[1]);

	// prepare data: fetch the data from specified DDR space
	info("arm try to fetch PLC image from\n");

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// clear pending flags
	ipc_clear_plctoa7_flag(p_ipc_dev, 0);
	// release all allocated share ram
	for (i=0; i<16; i++)
		a7_free_page(p_ipc_dev, i);
	while(1)
	{
		// waiting for request from PLC
		do {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					return -1;
				}
			}
			plctoa7flg = ipc_readl(p_ipc_dev, PLCTOA7IPCSTS);
		} while(plctoa7flg == 0);


		// parse request from PLC
		if(plctoa7flg & 0x1)
			info("ipc_get_msg_plctoa7 flag %d\n", plctoa7flg);

		if(ipc_readl(p_ipc_dev,  PLCTOA7IPCCOMM) != 0x09700000)
			info("ipc get error msg from plc 0x%8x!\n", ipc_readl(p_ipc_dev, PLCTOA7IPCCOMM));

		request_addr = ipc_readl(p_ipc_dev, PLCTOA7IPCADDR);
		request_len = ipc_readl(p_ipc_dev, PLCTOA7IPCDATA0);
		if(request_len > 1024)
			request_len = 1024;
		request_len = ALIGN(request_len, 4);

		ipc_writel(p_ipc_dev, 1, PLCTOA7IPCACK );

		// apply 1k share ram and copy DDR data to share ram
		for(i=0; i<16; i++) {
			if (a7_get_sram_page(p_ipc_dev, i) < 0) {
				info("plc_spboot: a7 get sram page %d failed\n", i);
				for(j=0; j<i; j++)
					a7_free_page(p_ipc_dev, j);
				return -1;
			}
		}
		memcpy(p_ipc_dev->ram_base + 0*IPC_PAGE_LEN,  img_buf + 1024, 16*1024);
		memcpy(p_ipc_dev->ram_base + 15*IPC_PAGE_LEN,  img_buf, 1024);

		request_len = get_arg_ulong(argv[2]); //force special case's transfer length

		// send ACK to PLC
		ipc_writel(p_ipc_dev, 0x09710000, A7TOPLCIPCCOMM );
		ipc_writel(p_ipc_dev, 15*IPC_PAGE_LEN, A7TOPLCIPCADDR );
		ipc_writel(p_ipc_dev, request_addr, A7TOPLCIPCDATA0 );
		ipc_writel(p_ipc_dev, request_len, A7TOPLCIPCDATA1 );
		ipc_writel(p_ipc_dev, 1, A7TOPLCIPCSET );

		// waiting for ACK from PLC
		while(ipc_readl(p_ipc_dev, A7TOPLCIPCFLG) & 0x1 == 1) {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q') {
					for(i=0; i<16; i++)
						a7_free_page(p_ipc_dev, i);
					return -1;
				}
			}
			info("wait ipc ack\n");
		}
		// release allocated share ram
		for(i=0; i<16; i++)
			a7_free_page(p_ipc_dev, i);
		//if(request_len < 1024)
		//	break;
		info("plc special package send done\n");
	}
	return 0;
}

/**
 * @brief rf_boot_test - ARM loadbin RF image to DDR and send DDR data to SHRAM,
 * RF bootrom copy the SHRAM data to DRAM/IRAM,then boot...
 *
 * @param offset input, the RF image DDR load address
 *
 * @return directly if failed
 */
static int rf_boot_test(int argc, char* argv[])
{
	unsigned int request_len = 0;
	unsigned int request_addr = 0;
	unsigned int i = 0;
	unsigned int rftoa7flg = 0;
	unsigned char *img_buf = NULL;    
	char cc;

	if (argc < 2) {
		info("wrong cmd_format: %s offset\n", argv[0]);
		return 0;
	}  
	
    img_buf = (unsigned char *)get_arg_ulong(argv[1]);

	// prepare data: fetch the data from specified DDR space
	info("arm try to fetch RF image from\n");

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// clear pending flags
	ipc_clear_rftoa7_flag(p_ipc_dev, 0);
	// release all allocated share ram
	for (i=0; i<32; i++)
		a7_free_page(p_ipc_dev, i);
	while(1)
	{
		// waiting for request from RF
		do {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					return -1;
				}
			}
			rftoa7flg = ipc_readl(p_ipc_dev, RFTOA7IPCSTS);
		} while(rftoa7flg == 0);

		// parse request from RF
		if(rftoa7flg & 0x1)
			info("ipc_get_msg_rftoa7 flag %d\n", rftoa7flg);

		if(ipc_readl(p_ipc_dev,  RFTOA7IPCCOMM) != 0x0a700000)
			info("ipc get error msg from rf 0x%8x!\n", ipc_readl(p_ipc_dev, RFTOA7IPCCOMM));

		request_addr = ipc_readl(p_ipc_dev, RFTOA7IPCADDR);
		request_len = ipc_readl(p_ipc_dev, RFTOA7IPCDATA0);
		if(request_len > 1024)
			request_len = 1024;
		request_len = ALIGN(request_len, 4);

		ipc_writel(p_ipc_dev, 1, RFTOA7IPCACK );
		// apply 1k share ram and copy DDR data to share ram
		if (a7_get_sram_page(p_ipc_dev, 6) < 0) {
			info("rf_boot: a7 get sram page 6 failed\n");
			return -1;
		}

		memcpy(p_ipc_dev->ram_base + 6*IPC_PAGE_LEN,  img_buf + request_addr, request_len);

		// send ACK to RF
		ipc_writel(p_ipc_dev, 0x0a710000, A7TORFIPCCOMM );
		ipc_writel(p_ipc_dev, 6*IPC_PAGE_LEN, A7TORFIPCADDR );
		ipc_writel(p_ipc_dev, request_addr, A7TORFIPCDATA0 );
		ipc_writel(p_ipc_dev, request_len, A7TORFIPCDATA1 );
		ipc_writel(p_ipc_dev, 1, A7TORFIPCSET );

		// waiting for ACK from RF
		while(ipc_readl(p_ipc_dev, A7TORFIPCFLG) & 0x1 == 1) {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					a7_free_page(p_ipc_dev, 6);
					return -1;
				}
			}
			info("wait ipc ack\n");
		}
		// release allocated share ram
		a7_free_page(p_ipc_dev, 6);
		//if(request_len < 1024)
		//	break;
		info("rf one package send done\n");
	}
	return 0;
}

/**
 * @brief rf_spboot_test - special case:ARM loadbin RF image(<16K) to DDR and send DDR data to SHRAM.
 * RF bootrom copy header data from SHRAM to DRAM and then get SHRAM ownership then boot...
 *
 * @param offset input, the RF image DDR load address
 * @param file_len input, the RF image length, should be < 16K
 *
 * @return directly if failed
 */
static int rf_spboot_test(int argc, char* argv[])
{
	unsigned int request_len = 0;
	unsigned int request_addr = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int rftoa7flg = 0;
	char cc;
	unsigned char *img_buf = NULL;    	

	if (argc < 3) {
		info("wrong cmd_format: %s offset file_len\n", argv[0]);
		return 0;
	}

    img_buf = (unsigned char *)get_arg_ulong(argv[1]);

	// prepare data: fetch the data from specified DDR space
	info("arm try to fetch RF image from\n");

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// clear pending flags
	ipc_clear_rftoa7_flag(p_ipc_dev, 0);
	// release all allocated share ram
	for (i=16; i<32; i++)
		a7_free_page(p_ipc_dev, i);
	while(1)
	{
		// waiting for request from RF
		do {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					return -1;
				}
			}
			rftoa7flg = ipc_readl(p_ipc_dev, RFTOA7IPCSTS);
		} while(rftoa7flg == 0);

		// parse request from RF
		if(rftoa7flg & 0x1)
			info("ipc_get_msg_rftoa7 flag %d\n", rftoa7flg);

		if(ipc_readl(p_ipc_dev,  RFTOA7IPCCOMM) != 0x0a700000)
			info("ipc get error msg from rf 0x%8x!\n", ipc_readl(p_ipc_dev, RFTOA7IPCCOMM));

		request_addr = ipc_readl(p_ipc_dev, RFTOA7IPCADDR);
		request_len = ipc_readl(p_ipc_dev, RFTOA7IPCDATA0);
		if(request_len > 1024)
			request_len = 1024;
		request_len = ALIGN(request_len, 4);

		ipc_writel(p_ipc_dev, 1, RFTOA7IPCACK );
		// apply 1k share ram and copy DDR data to share ram
		for(i=16; i<32; i++) {
			if (a7_get_sram_page(p_ipc_dev, i) < 0) {
				info("rf_spboot: a7 get sram page %d failed\n", i);
				for(j=16; j<i; j++)
					a7_free_page(p_ipc_dev, j);
				return -1;
			}
		}
		memcpy(p_ipc_dev->ram_base + 16*IPC_PAGE_LEN,  img_buf + 1024, 16*1024);
		memcpy(p_ipc_dev->ram_base + 31*IPC_PAGE_LEN,  img_buf, 1024);

		request_len = get_arg_ulong(argv[2]); //force special case's transfer length

		// send ACK to RF
		ipc_writel(p_ipc_dev, 0x0a710000, A7TORFIPCCOMM );
		ipc_writel(p_ipc_dev, 31*IPC_PAGE_LEN, A7TORFIPCADDR );
		ipc_writel(p_ipc_dev, request_addr, A7TORFIPCDATA0 );
		ipc_writel(p_ipc_dev, request_len, A7TORFIPCDATA1 );
		ipc_writel(p_ipc_dev, 1, A7TORFIPCSET );

		// waiting for ACK from RF
		while(ipc_readl(p_ipc_dev, A7TORFIPCFLG) & 0x1 == 1) {
			if (console_tstc()){
				cc = console_getc();
				if (cc == 'q'){
					for(i=16; i<32; i++)
						a7_free_page(p_ipc_dev, i);
					return -1;
				}
			}
			info("wait ipc ack\n");
		}
		// release allocated share ram
		for(i=16; i<32; i++)
		    a7_free_page(p_ipc_dev, i);
		//if(request_len < 1024)
		//	break;
		info("rf one package send done\n");
	}
	return 0;
}

/**
 * @brief plcrf_boot_test - ARM loadbin DSP image to DDR and send DDR data to SHRAM,
 * DSP bootrom copy the SHRAM data to DRAM/IRAM,then boot...
 *
 * @param plc_offset input, the PLC image DDR load address
 * @param rf_offset input, the RF image DDR load address
 *
 * @return directly if failed
 */
static int plcrf_boot_test(int argc, char* argv[])
{
	unsigned int plc_request_len = 0;
	unsigned int plc_request_addr = 0;
	unsigned int plctoa7flg = 0;
	unsigned char *plc_img_buf = NULL; 

	unsigned int rf_request_len = 0;
	unsigned int rf_request_addr = 0;
	unsigned int rftoa7flg = 0;
	unsigned char *rf_img_buf = NULL;
	char cc;

	unsigned int i = 0;  

	if (argc < 3) {
		info("wrong cmd_format: %s plc_offset rf_offset\n", argv[0]);
		return 0;
	}  	
    plc_img_buf = (unsigned char *)get_arg_ulong(argv[1]);
    rf_img_buf = (unsigned char *)get_arg_ulong(argv[2]);

	// prepare data: fetch the data from specified DDR space
	info("arm try to feed PLC & RF image\n");

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// clear pending flags
	ipc_clear_rftoa7_flag(p_ipc_dev, 0);
	ipc_clear_plctoa7_flag(p_ipc_dev, 0);

	// release all allocated share ram
	for (i=0; i<32; i++)
		a7_free_page(p_ipc_dev, i);

	while(1)
	{
		if (console_tstc()){
			cc = console_getc();
			if (cc == 'q'){
				return -1;
			}
		}
		// waiting for request from PLC or RF
	    plctoa7flg = ipc_readl(p_ipc_dev, PLCTOA7IPCSTS);
		if(plctoa7flg != 0)
		{
			// parse request from PLC
			if(plctoa7flg & 0x1)
				info("ipc_get_msg_plctoa7 flag %d\n", plctoa7flg);

			if(ipc_readl(p_ipc_dev,  PLCTOA7IPCCOMM) != 0x09700000)
				info("ipc get error msg from plc 0x%8x!\n", ipc_readl(p_ipc_dev, PLCTOA7IPCCOMM));

			plc_request_addr = ipc_readl(p_ipc_dev, PLCTOA7IPCADDR);
			plc_request_len = ipc_readl(p_ipc_dev, PLCTOA7IPCDATA0);
			if(plc_request_len > 1024)
				plc_request_len = 1024;
			plc_request_len = ALIGN(plc_request_len, 4);

			ipc_writel(p_ipc_dev, 1, PLCTOA7IPCACK );

			// apply 1k share ram and copy DDR data to share ram
			if (a7_get_sram_page(p_ipc_dev, 5) < 0) {
				info("plcrf_boot: a7 get sram page 5 failed\n");
				return -1;
			}

			memcpy(p_ipc_dev->ram_base + 5*IPC_PAGE_LEN,  plc_img_buf + plc_request_addr, plc_request_len);
			// send ACK to PLC
			ipc_writel(p_ipc_dev, 0x09710000, A7TOPLCIPCCOMM );
			ipc_writel(p_ipc_dev, 5*IPC_PAGE_LEN, A7TOPLCIPCADDR );
			ipc_writel(p_ipc_dev, plc_request_addr, A7TOPLCIPCDATA0 );
			ipc_writel(p_ipc_dev, plc_request_len, A7TOPLCIPCDATA1 );
			ipc_writel(p_ipc_dev, 1, A7TOPLCIPCSET );

			// waiting for ACK from PLC
			while(ipc_readl(p_ipc_dev, A7TOPLCIPCFLG) & 0x1 == 1) {
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q'){
						a7_free_page(p_ipc_dev, 5);
						return -1;
					}
				}
				info("wait ipc ack\n");
			}
			// release allocated share ram
			a7_free_page(p_ipc_dev, 5);
			//if(request_len < 1024)
			//	break;
			info("plc one package send done\n");
		}

		rftoa7flg = ipc_readl(p_ipc_dev, RFTOA7IPCSTS);
		if(rftoa7flg != 0)
		{
			// parse request from RF
			if(rftoa7flg & 0x1)
				info("ipc_get_msg_rftoa7 flag %d\n", rftoa7flg);

			if(ipc_readl(p_ipc_dev,  RFTOA7IPCCOMM) != 0x0a700000)
				info("ipc get error msg from rf 0x%8x!\n", ipc_readl(p_ipc_dev, RFTOA7IPCCOMM));

			rf_request_addr = ipc_readl(p_ipc_dev, RFTOA7IPCADDR);
			rf_request_len = ipc_readl(p_ipc_dev, RFTOA7IPCDATA0);
			if(rf_request_len > 1024)
				rf_request_len = 1024;
			rf_request_len = ALIGN(rf_request_len, 4);

			ipc_writel(p_ipc_dev, 1, RFTOA7IPCACK );
			// apply 1k share ram and copy DDR data to share ram
			if (a7_get_sram_page(p_ipc_dev, 6) < 0) {
				info("plcrf_boot: a7 get sram page 6 failed\n");
				return -1;
			}

			memcpy(p_ipc_dev->ram_base + 6*IPC_PAGE_LEN,  rf_img_buf + rf_request_addr, rf_request_len);
			// send ACK to RF
			ipc_writel(p_ipc_dev, 0x0a710000, A7TORFIPCCOMM );
			ipc_writel(p_ipc_dev, 6*IPC_PAGE_LEN, A7TORFIPCADDR );
			ipc_writel(p_ipc_dev, rf_request_addr, A7TORFIPCDATA0 );
			ipc_writel(p_ipc_dev, rf_request_len, A7TORFIPCDATA1 );
			ipc_writel(p_ipc_dev, 1, A7TORFIPCSET );

			// waiting for ACK from RF
			while(ipc_readl(p_ipc_dev, A7TORFIPCFLG) & 0x1 == 1) {
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q'){
						a7_free_page(p_ipc_dev, 6);
						return -1;
					}
				}
				info("wait ipc ack\n");
			}
			// release allocated share ram
			a7_free_page(p_ipc_dev, 6);
			//if(request_len < 1024)
			//	break;
			info("rf one package send done\n");
		}
	}
	return 0;
}

/**
 * @brief plcrf_spboot_test - special case:ARM loadbin DSP image(<16K) to DDR and send DDR data to SRAM.
 * DSP bootrom copy header data from SHRAM to DRAM and then get SHRAM ownership then boot...
 *
 * @param plc_offset input, the PLC image DDR load address
 * @param rf_offset input, the RF image DDR load address
 * @param plc_file_len input, the PLC image length, should be < 16K
 * @param rf_file_len input, the RF image length, should be < 16K
 *
 * @return directly if failed
 */
static int plcrf_spboot_test(int argc, char* argv[])
{
	unsigned int plc_request_len = 0;
	unsigned int plc_request_addr = 0;
	unsigned int plctoa7flg = 0;
	unsigned char *plc_img_buf = NULL;

	unsigned int rf_request_len = 0;
	unsigned int rf_request_addr = 0;
	unsigned int rftoa7flg = 0;
	unsigned char *rf_img_buf = NULL;
	char cc;

	unsigned int i = 0;
	unsigned int j = 0;

	// prepare data: fetch the data from specified DDR space
	info("arm try to feed PLC & RF image\n");

	if (argc < 5) {
		info("wrong cmd_format: %s plc_offset rf_offset plc_file_len rf_file_len\n", argv[0]);
		return 0;
	}


    plc_img_buf = (unsigned char *)get_arg_ulong(argv[1]);
    rf_img_buf = (unsigned char *)get_arg_ulong(argv[2]);

	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);

	// clear pending flags
	ipc_clear_rftoa7_flag(p_ipc_dev, 0);
	ipc_clear_plctoa7_flag(p_ipc_dev, 0);

	// release all allocated share ram
	for (i=0; i<32; i++)
		a7_free_page(p_ipc_dev, i);

	while(1)
	{
		if (console_tstc()){
			cc = console_getc();
			if (cc == 'q'){
				return -1;
			}
		}
		// waiting for request from PLC or RF
	    plctoa7flg = ipc_readl(p_ipc_dev, PLCTOA7IPCSTS);
		if(plctoa7flg != 0)
		{
			// parse request from PLC
			if(plctoa7flg & 0x1)
				info("ipc_get_msg_plctoa7 flag %d\n", plctoa7flg);

			if(ipc_readl(p_ipc_dev,  PLCTOA7IPCCOMM) != 0x09700000)
				info("ipc get error msg from plc 0x%8x!\n", ipc_readl(p_ipc_dev, PLCTOA7IPCCOMM));

			plc_request_addr = ipc_readl(p_ipc_dev, PLCTOA7IPCADDR);
			plc_request_len = ipc_readl(p_ipc_dev, PLCTOA7IPCDATA0);
			if(plc_request_len > 1024)
				plc_request_len = 1024;
			plc_request_len = ALIGN(plc_request_len, 4);

			ipc_writel(p_ipc_dev, 1, PLCTOA7IPCACK );

			// apply 1k share ram and copy DDR data to share ram
			for(i=0; i<16; i++) {
				if (a7_get_sram_page(p_ipc_dev, i) < 0) {
					info("plcrf_spboot: a7 get sram page %d failed\n", i);
					for(j=0; j<i; j++)
						a7_free_page(p_ipc_dev, j);
					return -1;
				}
			}
			//memcpy(p_ipc_dev->ram_base + 0*IPC_PAGE_LEN,  plc_img_buf + plc_request_addr, 14*1024);
			memcpy(p_ipc_dev->ram_base + 0*IPC_PAGE_LEN,  plc_img_buf + 1024, 16*1024);
			memcpy(p_ipc_dev->ram_base + 15*IPC_PAGE_LEN,  plc_img_buf, 1024);

			plc_request_len = get_arg_ulong(argv[3]); //force special case's transfer length
			// send ACK to PLC
			ipc_writel(p_ipc_dev, 0x09710000, A7TOPLCIPCCOMM );
			ipc_writel(p_ipc_dev, 15*IPC_PAGE_LEN, A7TOPLCIPCADDR );
			ipc_writel(p_ipc_dev, plc_request_addr, A7TOPLCIPCDATA0 );
			ipc_writel(p_ipc_dev, plc_request_len, A7TOPLCIPCDATA1 );
			ipc_writel(p_ipc_dev, 1, A7TOPLCIPCSET );

			// waiting for ACK from PLC
			while(ipc_readl(p_ipc_dev, A7TOPLCIPCFLG) & 0x1 == 1) {
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q'){
						for(i=0; i<16; i++)
							a7_free_page(p_ipc_dev, i);
						return -1;
					}
				}
				info("wait ipc ack\n");
			}
			// release allocated share ram
			for(i=0; i<16; i++)
			    a7_free_page(p_ipc_dev, i);
			//if(request_len < 1024)
			//	break;
			info("plc one package send done\n");
		}

		rftoa7flg = ipc_readl(p_ipc_dev, RFTOA7IPCSTS);
		if(rftoa7flg != 0)
		{
			// parse request from RF
			if(rftoa7flg & 0x1)
				info("ipc_get_msg_rftoa7 flag %d\n", rftoa7flg);

			if(ipc_readl(p_ipc_dev,  RFTOA7IPCCOMM) != 0x0a700000)
				info("ipc get error msg from rf 0x%8x!\n", ipc_readl(p_ipc_dev, RFTOA7IPCCOMM));

			rf_request_addr = ipc_readl(p_ipc_dev, RFTOA7IPCADDR);
			rf_request_len = ipc_readl(p_ipc_dev, RFTOA7IPCDATA0);
			if(rf_request_len > 1024)
				rf_request_len = 1024;
			rf_request_len = ALIGN(rf_request_len, 4);

			ipc_writel(p_ipc_dev, 1, RFTOA7IPCACK );
			// apply 1k share ram and copy DDR data to share ram
			for(i=16; i<32; i++) {
				if (a7_get_sram_page(p_ipc_dev, i) < 0) {
					info("plcrf_spboot: a7 get sram page %d failed\n", i);
					for(j=16; j<i; j++)
						a7_free_page(p_ipc_dev, j);
					return -1;
				}
			}
			//memcpy(p_ipc_dev->ram_base + 16*IPC_PAGE_LEN,  rf_img_buf + rf_request_addr, rf_request_len);
			memcpy(p_ipc_dev->ram_base + 16*IPC_PAGE_LEN,  rf_img_buf + 1024, 16*1024);
			memcpy(p_ipc_dev->ram_base + 31*IPC_PAGE_LEN,  rf_img_buf, 1024);

			rf_request_len = get_arg_ulong(argv[4]); //force special case's transfer length
			// send ACK to RF
			ipc_writel(p_ipc_dev, 0x0a710000, A7TORFIPCCOMM );
			ipc_writel(p_ipc_dev, 31*IPC_PAGE_LEN, A7TORFIPCADDR );
			ipc_writel(p_ipc_dev, rf_request_addr, A7TORFIPCDATA0 );
			ipc_writel(p_ipc_dev, rf_request_len, A7TORFIPCDATA1 );
			ipc_writel(p_ipc_dev, 1, A7TORFIPCSET );

			// waiting for ACK from RF
			while(ipc_readl(p_ipc_dev, A7TORFIPCFLG) & 0x1 == 1) {
				if (console_tstc()){
					cc = console_getc();
					if (cc == 'q'){
						for(i=16; i<32; i++)
							a7_free_page(p_ipc_dev, i);
						return -1;
					}
				}
				info("wait ipc ack\n");
			}
			// release allocated share ram
			for(i=16; i<32; i++)
			    a7_free_page(p_ipc_dev, i);
			//if(request_len < 1024)
			//	break;
			info("rf one package send done\n");
		}
	}
	return 0;
}

int ipc_test_prepare(int sel)
{
	int i = 0;
	
    a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);
	
	// clear pending flags
	for (i=0; i<16; i++)
	{
		ipc_clear_plctoa7_flag(p_ipc_dev, i);
		ipc_clear_rftoa7_flag(p_ipc_dev, i);
	}
	// release all allocated share ram
	for (i=0; i<32; i++)
		a7_free_page(p_ipc_dev, i);

	return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry ipc_test_menu[] = {
	{ "help", cmd_help, 		 " : Display list of commands" },
	{ "h",	cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "ipc_set_msg",ipc_set_msg, 		 "  : ipc_set_msg" },
	{ "ipc_get_msg",ipc_get_msg, 		 "  : ipc_get_msg" },
	{ "ipc_int",ipc_int, 		 "  : ipc_int" },
	{ "ipc_free_time",ipc_free_time, 		 "  :ipc_free_time" },
	{ "plc_boot_test",plc_boot_test, 		 "  :plc_boot_test" },
	{ "rf_boot_test",rf_boot_test, 		 "  :rf_boot_test" },
	{ "plcrf_boot_test",plcrf_boot_test, "  :plcrf_boot_test" },
	{ "plc_spboot_test",plc_spboot_test, "  :plc_spboot_test" },
	{ "rf_spboot_test",rf_spboot_test, "  :rf_spboot_test" },
	{ "plcrf_spboot_test",plcrf_spboot_test, "  :plcrf_spboot_test" },
	{ 0, 0, 0 }
};

#define IPC_A7TORFIPCCOMM_DEF_VAL	0x0
bool ipc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	struct a7_ipc* p_dev = p_ipc_dev;

	info("%s\n", __func__);
	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);
	
	info("write A7TORFIPCCOMM reg\n");
	
	// write register
	__raw_writel(*p_wr_val, (uint32_t)p_dev->reg_base + A7TORFIPCCOMM);

	// read&compare
	info("read&compare A7TORFIPCCOMM reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->reg_base + A7TORFIPCCOMM);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("ipc reset\n");
	prcm_reset(ARM_IPC_RST);
	
	// read register
	info("read&compare A7TORFIPCCOMM reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->reg_base + A7TORFIPCCOMM);
	if (*p_rd_val != IPC_A7TORFIPCCOMM_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool ipc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk = ARM_BLK_CG_IPC;
	struct a7_ipc* p_dev = p_ipc_dev;

	info("%s\n", __func__);
	
	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	a7_ipc_init(p_ipc_dev, A7_IPC_REG_BASE, A7_IPC_RAM_BASE);
	
	// write register
	info("write A7TORFIPCCOMM reg\n");
	__raw_writel(*p_wr_val, (uint32_t)p_dev->reg_base + A7TORFIPCCOMM);

	// read&compare
	info("read&compare A7TORFIPCCOMM reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->reg_base + A7TORFIPCCOMM);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write A7TORFIPCCOMM reg\n");
	__raw_writel((*p_wr_val+1), (uint32_t)p_dev->reg_base + A7TORFIPCCOMM);	

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare A7TORFIPCCOMM reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->reg_base + A7TORFIPCCOMM);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool ipc_reset_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool ipc_cg_test(uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

