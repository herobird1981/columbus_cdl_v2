/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_config.h"
#include "test_uart.h"
#include "cdl_pll.h"
#include "memleak.h"
#include "cdl_uart.h"

struct dw_uart uart_dev;
struct dw_uart* p_uart_dev = &uart_dev;

#if (CONFIG_UART_TEST == 1)
struct dw_dmac* p_dma_test;

extern int current_console_base;
extern int console_switched;

int uart_buf_size = 0;
int uart_int_in_use[4];
int dma_int_in_use[4];

struct dw_uart uart_echo_dev_irq;
struct dw_uart uart_dev_irq;

volatile int uart_dma_mb_count = 0;

cmdline_entry uart_test_menu[];

#define UART_ECHO_SINGLE_LEN  (UART_FIFO_DEPTH/4)

#define XFER_DEBUG	0
#if XFER_DEBUG
#	define xfer_debug	info
#else
#	define xfer_debug(...)	do { } while(0)
#endif

static int uart_init(struct dw_uart *uart, int argc, char* argv[])
{
	unsigned long dma_mode;
	unsigned long baudrate;
	unsigned long parity;
	unsigned long stop_bits;
	unsigned long data_bits;
	unsigned long flow_ctl;
	unsigned long ctl = 0;

	if (argc < 7) {
		info("\nwrong cmd_format:%s dma_mode baud_rate parity data_bits stop_bits flow_ctl\n\n",
			argv[0]);
		return 1;
	}
	dma_mode = get_arg_ulong(argv[1]);
	baudrate = get_arg_ulong(argv[2]);
	if (!strcmp(argv[3], "n")) {
		parity = 0;
	} else if (!strcmp(argv[3], "e")) {
		parity = 1;
	} else if (!strcmp(argv[3], "o")) {
		parity = 2;
	} else {
		info("unknown parity setting, force to 'n'\n");
		parity = 0;
	}
	uart->parity = parity;
	data_bits = get_arg_ulong(argv[4]);
	stop_bits = get_arg_ulong(argv[5]);
	uart->flow = flow_ctl = get_arg_ulong(argv[6]);

	if ((data_bits > 8) || (data_bits < 5)) {
		info("wrong data bits, supported: 5-8\n");
		return 1;
	}
	if ((stop_bits > 2) || (stop_bits < 1)) {
		info("wrong stop bits, supported: 1, 2 (2 for 5 bits is actually 1.5 stop bits\n");
		return 1;
	}

	/*set baud rate*/
	uart->dma_mode = dma_mode;
	uart->data_bits = data_bits;
	uart->stop_bits = stop_bits;
	dw_uart_init(uart, uart->pclk, baudrate);

	/*set parity bits,data bits,stop bits*/
	if (parity == 0)// no parity
	{
		ctl |= LCR_PARITY_NONE;
	} else if (parity == 1)// even parity
	{
		ctl |= LCR_PARITY_EVEN;
	} else {
		ctl |= LCR_PARITY_ODD;
	}

	/*data bits*/
	ctl |= (data_bits - 5) + LCR_DATA_5BIT;

	if (stop_bits == 1)/* 1 stop bits*/ {
		ctl |= LCR_STOP_1BIT;
	} else if (stop_bits == 2) {
		ctl |= LCR_STOP_2BIT;
	} else {
		ctl |= LCR_STOP_1BIT;
	}

	/*set control*/
	dw_uart_set_line_control(uart, ctl);
	uart->len_per_xfer = flow_ctl ? UART_FIFO_DEPTH / 2 : UART_FIFO_DEPTH / 4;
	uart->tx_dma_burst_size = BURST_32;
	uart->rx_dma_burst_size = flow_ctl ? BURST_32 : BURST_16;

	/* set hardware flow ctrl*/
	if (flow_ctl)
		dw_uart_set_modem_ctrl(uart, UART_FLOWCTRL_AUTO);

	/* FIFO should be empty already,
	 * still force read RBR register to resolve previous deadlock condition
	 */
	dw_uart_empty_fifo(uart);

	info("uart config: using %s mode\n", dma_mode?"dma":"polling");
	info("uart config: '%d %s %d %d' %s flow ctrl\n",
		baudrate,
		(parity==1)?"e":((parity==2)?"o":"n"),
		data_bits,
		stop_bits,
		flow_ctl?"hw":"no");
	return 0;
}

extern int g_min_err_rate_of_100k;
static int uart_cfg(int argc, char* argv[])
{
	struct dw_uart *uart = p_uart_dev;
	int ret;
#if !defined(EVB_BOARD)
	g_min_err_rate_of_100k = (uart->pclk/uart->div - uart->baudrate)*100/uart->baudrate*1000;
#endif
	info("current uart config: frac_div 0x%x err_rate %d.%03d%%",
			brite_get_uart_clk_div(), g_min_err_rate_of_100k/1000, g_min_err_rate_of_100k%1000);
	info("%s baud %d parity %s bits %d stop %d\n",
			uart->dma_mode ? "dma" : "fifo", uart->baudrate,
			uart->parity ? (uart->parity == 1 ? "e" : "o") : "n", uart->data_bits, uart->stop_bits);
	ret = uart_init(p_uart_dev, argc, argv);
#if !defined(EVB_BOARD)
	g_min_err_rate_of_100k = (uart->pclk/uart->div - uart->baudrate)*100/uart->baudrate*1000;
#endif
	info("new uart config: frac_div 0x%x err_rate %d.%03d%%",
			brite_get_uart_clk_div(),  g_min_err_rate_of_100k/1000, g_min_err_rate_of_100k%1000);
	info("%s baud %d parity %s bits %d stop %d\n",
			uart->dma_mode ? "dma" : "fifo", uart->baudrate,
			uart->parity ? (uart->parity == 1 ? "e" : "o") : "n", uart->data_bits, uart->stop_bits);
	return ret;
}

static void uart_generate_random_data(int random,int len,unsigned char pattern)
{
	int i;
	int rand_off;

	if (random) {
		srand(get_ticks());

		/*generate random data: printable*/
		for (i = 0; i < len; i++) {
			rand_off = cb_rand() %0x4B;
			test_send_buff[i] = 0x30+rand_off;
		}
	} else {
		for (i = 0; i < len; i++) {
			test_send_buff[i] = pattern;
		}
	}
	memset(test_recv_buff, 0, len);
}

static int uart_compare_result(struct dw_uart *uart, int len)
{
	int i;
	int ret = 0;
	unsigned char mask=0xff;

	mask = (1<<uart->data_bits) - 1;

	for (i=0;i<len;i++) {
		if ((test_send_buff[i] & mask) != test_recv_buff[i]) {
			info("send 0x%x(%c) but received 0x%x at 0x%x(%d)\n",
				test_send_buff[i],test_send_buff[i],
				test_recv_buff[i],i,i);
			ret = 1;
			break;
		}
	}
	if(ret) {
		print_hex_dump("send", 1, test_send_buff, len);
		print_hex_dump("recv", 1, test_recv_buff, len);
	}
	return ret;
}

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&uart_test_menu[0]);
}

static int uart_transfer_test(int len_per_loop,int *loop,
	int random_flag,unsigned char pattern)
{
	int len;
	int tx_len = 0;
	int rx_len = 0;
	int fail = 0;
	unsigned long act_count=0;
	unsigned long count = *loop;

	while (count--) {
		if(console_check_char('q'))
			break;				

		info("loop %d err %d\n",act_count,fail);
		act_count++;
		/*generate random pattern*/
		len = len_per_loop;
		tx_len = 0;
		rx_len = 0;
		uart_generate_random_data(random_flag,len,pattern);
		while(tx_len < len) {
			int this_tx_len = min(p_uart_dev->len_per_xfer, len-tx_len);
			xfer_debug("this_tx_len %d\n", this_tx_len);
			dw_uart_write(p_uart_dev, (const char*)test_send_buff+tx_len, this_tx_len);
			xfer_debug("xfer tx done\n");
			rx_len += dw_uart_read(p_uart_dev, (char*)test_recv_buff+rx_len, this_tx_len);
			xfer_debug("xfer rx len %d\n", rx_len);
			tx_len += this_tx_len;
		}
		/*comapre result*/
		if (uart_compare_result(p_uart_dev, len))
			fail++;
	}
	*loop = act_count;
	return fail;
}

static int uart_transfer_test_delay(int len_per_loop,int *loop,
	int random_flag,unsigned char pattern)
{
	int len = 0;
	int fail = 0;
	unsigned long act_count=0;
	unsigned long count = *loop;

	while (count--) {
		if(console_check_char('q'))
			break;				

		info("loop %d err %d\n",act_count,fail);
		act_count++;
		/*generate random pattern*/
		len = len_per_loop;
		uart_generate_random_data(random_flag,len,pattern);
		dw_uart_write(p_uart_dev, (const char*)test_send_buff, len);
		mdelay(500);
		len = dw_uart_read(p_uart_dev, (char*)test_recv_buff, len);
		/*comapre result*/
		if (uart_compare_result(p_uart_dev, len))
			fail++;
	}
	*loop = act_count;
	return fail;
}

/*testcase:loopback transmit/receive test*/
static int uart_loopback(int argc, char* argv[])
{
	char c;
	char cc;
	unsigned long len;
	int i;
	int fail = 0;
	unsigned long long start = 0, end = 0;
	unsigned char pattern;
	int random_flag = 1;
	char bits_mask;

	info("%s:internal loopback tx/rx test\n",argv[0]);
	if (argc < 8) {
		info("wrong cmd_format:%s dma_mode baudrate parity data_bits stop_bits flow_ctl len [pattern]",
			argv[0]);
		return 0;
	}
	if (uart_init(p_uart_dev, argc,argv))
		return 0;
	len = get_arg_ulong(argv[7]);
	if (argc >=9) {
		pattern = get_arg_ulong(argv[8]);
		random_flag = 0;
	}
	bits_mask = (1UL<<p_uart_dev->data_bits)-1;

	/*set internal loopback mode*/
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_LOOP_BITS, 1);

	srand(get_ticks());

	for (i = 0; i < len; i++) {
		if (random_flag)
			c = cb_rand() & 0xff;
		else
			c = pattern & 0xff;
		dw_uart_putc(p_uart_dev, c);

		start = get_timer(0);
		do {
			if (dw_uart_tstc(p_uart_dev))
				break;
		} while ((end = get_timer(start)) < 100);
		if (end >= 100)/*timeout*/ {
			fail++;
			break;
		}
		if (dw_uart_tstc(p_uart_dev)) {
			cc = dw_uart_getc(p_uart_dev);
			if (cc != (c & bits_mask)) {
				fail++;
				debug("loopback: send 0x%x but received 0x%x\n", c,	cc);
				break;
			}
		}
	}
	/*unset loop back mode*/
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_LOOP_BITS, 0);

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/*testcase:loopback transmit/receive test*/
static int uart_external_loopback(int argc, char* argv[])
{
	char c;
	char cc;
	unsigned long len;
	int i;
	int fail = 0;
	unsigned long long start = 0, end = 0;
	unsigned char pattern;
	int random_flag = 1;
	char bits_mask;

	info("%s:external loopback tx/rx test\n",argv[0]);
	if (argc < 8) {
		info("wrong cmd_format:%s dma_mode baudrate parity data_bits stop_bits flow_ctl len [pattern]",
			argv[0]);
		return 0;
	}
	if (uart_init(p_uart_dev, argc,argv))
		return 0;
	len = get_arg_ulong(argv[7]);
	if (argc >=9) {
		pattern = get_arg_ulong(argv[8]);
		random_flag = 0;
	}
	bits_mask = (1UL<<p_uart_dev->data_bits)-1;

	srand(get_ticks());

	for (i = 0; i < len; i++) {
		if (random_flag)
			c = cb_rand() & 0xff;
		else
			c = pattern & 0xff;
		dw_uart_putc(p_uart_dev, c);

		start = get_timer(0);
		do {
			if (dw_uart_tstc(p_uart_dev))
				break;
		} while ((end = get_timer(start)) < 100);
		if (end >= 100)/*timeout*/ {
			fail++;
			break;
		}
		if (dw_uart_tstc(p_uart_dev)) {
			cc = dw_uart_getc(p_uart_dev);
			if (cc != (c & bits_mask)) {
				fail++;
				debug("external loopback: send 0x%x but received 0x%x\n", c, cc);
				break;
			}
		}
	}

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/*testcase:loopback modem control test*/
static int uart_modem_loopback(int argc, char* argv[])
{
	int fail = 0;
	unsigned long val;

	info("uart_modem_loopback:loopback modem ctrl test\n");

	/*set loop back mode*/
	dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200);
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_LOOP_BITS, 1);

	/*dtr/dsr*/
	info("checking dtr/dsr\n");
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_DTR_BITS, 0);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (val & UART_MSR_DSR) {
		info("dsr should same with dtr in loopback mode\n");
		fail++;
	}
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_DTR_BITS, 1);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (!(val & UART_MSR_DSR)) {
		info("dsr should same with dtr in loopback mode\n");
		fail++;
	}
	if (!(val & UART_MSR_DDSR)) {
		info(
			"ddsr should set for dtr change from 0 to 1 in loopback mode\n");
		fail++;
	}

	info("checking rts/cts\n");
	/*rts/cts*/
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_RTS_BITS, 0);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (val & UART_MSR_CTS) {
		info("cts should same with rts in loopback mode\n");
		fail++;
	}
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_RTS_BITS, 1);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (!(val & UART_MSR_CTS)) {
		info("cts should same with rts in loopback mode\n");
		fail++;
	}
	if (!(val & UART_MSR_DCTS)) {
		info(
			"dcts should set for dtr change from 0 to 1 in loopback mode\n");
		fail++;
	}

	info("checking out1/ri\n");
	/*out1/ri*/
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT1_BITS, 1);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (!(val & UART_MSR_RI)) {
		info("ri should same with out1 in loopback mode\n");
		fail++;
	}
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT1_BITS, 0);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (val & UART_MSR_RI) {
		info("ri should same with out1 in loopback mode\n");
		fail++;
	}

	if (!(val & UART_MSR_TERI)) {
		info(
			"teri should set for out1 change from 0 to 1 in loopback mode\n");
		fail++;
	}

	info("checking out2/dcd\n");
	/*out2/dcd*/
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT2_BITS, 0);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (val & UART_MSR_DCD) {
		info("dcd should same with out2 in loopback mode\n");
		fail++;
	}
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT2_BITS, 1);
	val = dw_uart_get_modem_status(p_uart_dev);
	if (!(val & UART_MSR_DCD)) {
		info("dcd should same with out2 in loopback mode\n");
		fail++;
	}
	if (!(val & UART_MSR_DDCD)) {
		info(
			"ddcd should set for out2 change from 0 to 1 in loopback mode\n");
		fail++;
	}

	/*unset loop back mode*/
	dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_LOOP_BITS, 0);

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}


static int transfer_line_error = 0;
void uart_transfer_int_handler(void* ptr)
{
	int int_status = 0;
	unsigned long lsr = 0;

	xfer_debug("%s\n", __func__);

	lsr = dw_uart_readl(p_uart_dev, DW_UART_LSR);
	lsr &= 0x1E;
	if(lsr){
		transfer_line_error++;
		info("Line error 0x%x\n", lsr);
	}
	int_status = dw_uart_get_int_status(p_uart_dev);
	xfer_debug("irq st 0x%x\n", int_status);
	return;
}


/*testcase:baudrate,parity bits,data bits,stop bits setting test*/
static int uart_transfer(int argc, char* argv[])
{
	int count = 0;
	int fail = 0;
	unsigned char pattern = 0;
	int random_flag = 1;

	info("uart_transfer:rs232 transmit / receive test,'q' to quit\n");

	if (argc < 8) {
		info("wrong cmd_format:%s dma_mode baudrate parity data_bits stop_bits flow_ctl loop [pattern]",
			argv[0]);
		return 0;
	}
	if (uart_init(p_uart_dev, argc,argv))
		return 0;
	count = get_arg_ulong(argv[7]);
	if (argc >=9) {
		pattern = get_arg_ulong(argv[8]);
		random_flag = 0;
	}

	transfer_line_error = 0;
	/*register int handler and enable it*/
	irq_generic_install_handler(p_uart_dev->int_vector,
		uart_transfer_int_handler,
		NULL);
	irq_generic_config_int(p_uart_dev->int_vector, IRQ_TYPE_LEVEL_LOW);
	irq_generic_enable_int(p_uart_dev->int_vector);

	/*enable interrupt*/
	dw_uart_enable_interrupt(p_uart_dev, UART_IER_RLSI);

	fail = uart_transfer_test(240,&count,random_flag,pattern);

	info("total %d loop,err %d\n",count,fail);
	if (fail || transfer_line_error) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	
	/*disable interrupt*/
	irq_generic_disable_int(p_uart_dev->int_vector);
	irq_generic_remove_handler(p_uart_dev->int_vector,NULL,NULL);
	dw_uart_enable_interrupt(p_uart_dev, 0);

	return 0;
}

/*testcase:baudrate,parity bits,data bits,stop bits setting test*/
static int uart_flowctrl(int argc, char* argv[])
{
	int count;
	int fail = 0;
	unsigned char pattern;
	int random_flag = 1;

	info("uart_flowctrl:rs232 transmit / receive test,'q' to quit\n");

	if (argc < 8) {
		info("wrong cmd_format:%s dma_mode baudrate parity data_bits stop_bits flow_ctl loop [pattern]",
			argv[0]);
		return 0;
	}
	if (uart_init(p_uart_dev, argc,argv))
		return 0;
	count = get_arg_ulong(argv[7]);
	if (argc >=9) {
		pattern = get_arg_ulong(argv[8]);
		random_flag = 0;
	}

	fail = uart_transfer_test_delay(256,&count,random_flag,pattern);

	info("total %d loop,failed %d\n",count,fail);
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	return 0;
}

void uart_echo_int_handler(void* ptr)
{
	struct dw_uart *p_uart = &uart_echo_dev_irq;
	int int_status = 0;
	unsigned long lsr;
	int len;
	int fifo_level;
	char uart_echo_buf[UART_FIFO_DEPTH];

	xfer_debug("%s\n", __func__);/*  */

	lsr = dw_uart_readl(p_uart, DW_UART_LSR);
	lsr &= 0x1E;
	if(lsr)
		info("Line error 0x%x\n", lsr);
	int_status = dw_uart_get_int_status(p_uart);
	//info("irq st 0x%x\n", int_status);/*  */
	if (int_status == 0x1) {
		return;
	}
	fifo_level = dw_uart_get_rcv_fifo_level(p_uart);
	len = dw_uart_read_non_block(p_uart,(char*)uart_echo_buf, fifo_level, 1000);
	info("irq rx len %d of fifo_level %d\n", len, fifo_level);/*  */
	dw_uart_write(p_uart, (const char*)uart_echo_buf, len);
	info("irq tx len %d\n", len);/*  */
}

/*testcase: master side of high baudrate,parity bits,data bits,stop bits setting test*/
static int uart_echo(int argc, char* argv[])
{
	int packet_len = 256;
	int packet_delay = 10;
	int packet_mode = 0;

	info("%s: uart echo server\n", __func__);

	dw_uart_enable_interrupt(p_uart_dev, 0);
	if (uart_init(p_uart_dev, argc,argv))
		return 0;
	if (argc >=8) {
		packet_len = get_arg_ulong(argv[7]);
		packet_mode = 1;
	}
	if (argc >=9) {
		packet_delay = get_arg_ulong(argv[8]);
	}

    dw_uart_set_fcr_rx_thres(p_uart_dev, UART_FCR_RX_TRIGGER_QUARTER);

	uart_int_in_use[p_uart_dev->line] = 1;
	uart_echo_dev_irq = *p_uart_dev;
	/*register int handler and enable it*/
	irq_generic_install_handler(p_uart_dev->int_vector,
		uart_echo_int_handler,
		&uart_echo_dev_irq);
	irq_generic_config_int(p_uart_dev->int_vector, IRQ_TYPE_LEVEL_LOW);
	irq_generic_enable_int(p_uart_dev->int_vector);

	/*enable interrupt*/
	dw_uart_enable_interrupt(p_uart_dev, UART_IER_RDI);

	info("uart echo server run in background\n");
	return 0;
}

/*testcase:modem control test*/
static int uart_modem(int argc, char* argv[])
{
	unsigned long val;
	char old_status[4];
	char status[4];

	info("uart_modem: modem ctrl test,input 'q' to quit this testcase\n");

	dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200);

	val = dw_uart_get_modem_status(p_uart_dev);
	memset(old_status, 1, sizeof(old_status));
	if (val & UART_MSR_CTS)
		old_status[0] = 0;
	if (val & UART_MSR_DSR)
		old_status[1] = 0;
	if (val & UART_MSR_RI)
		old_status[2] = 0;
	if (val & UART_MSR_DCD)
		old_status[3] = 0;

	info("initial status: cts %d dsr %d ri %d dcd %d\n",
			old_status[0], old_status[1], old_status[2], old_status[3]);

	/*loop check status change*/
	while (1) {
		val = dw_uart_get_modem_status(p_uart_dev);
		memset(status, 1, sizeof(status));
		if (val & UART_MSR_CTS)
			status[0] = 0;
		if (val & UART_MSR_DSR)
			status[1] = 0;
		if (val & UART_MSR_RI)
			status[2] = 0;
		if (val & UART_MSR_DCD)
			status[3] = 0;

		/*comare*/
		if (status[0] != old_status[0]) {
			info("cts change : %d to %d\n",	old_status[0],	status[0]);
			old_status[0] = status[0];
		}
		if (status[1] != old_status[1]) {
			info("dsr change : %d to %d\n",	old_status[1],	status[1]);
			old_status[1] = status[1];
		}
		if (status[2] != old_status[2]) {
			info("ri change : %d to %d\n", old_status[2], status[2]);
			old_status[2] = status[2];
		}
		if (status[3] != old_status[3]) {
			info("dcd change : %d to %d\n",	old_status[3],	status[3]);
			old_status[3] = status[3];
		}

		if(console_check_char('q'))
		{
			break;				
		}
	}

	info("uart_modem: test terminate\n");

	return 0;
}

int line_int_counter;
/*testcase: master side of modem control test*/
static int uart_modem_master(int argc, char* argv[])
{
	unsigned long val;

	info("uart_modem_master:master side modem ctrl test\n");

	if (argc < 3) {
		info("wrong cmd_format: uart_modem_master signal val\n"
				"signal: dtr|rts|out1|out2|all\n",
				"Note: ri and dcd are always input signal, and default to 1, \n"
				"      need to connect 3.3V or GND to test them.\n");
		return 0;
	}
	dw_uart_init_no_modem(p_uart_dev, p_uart_dev->pclk, 115200);

	val = get_arg_ulong(argv[2]);
	val = !!val;

	if (!strcmp(argv[1], "dtr")) {
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_DTR_BITS, !val);  /* dtr_n */
	} else if (!strcmp(argv[1], "rts")) {
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_RTS_BITS, !val);  /* rts_n */
	} else if (!strcmp(argv[1], "out1")) {
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT1_BITS, val);
	} else if (!strcmp(argv[1], "out2")) {
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT2_BITS, val);
	} else if (!strcmp(argv[1], "all"))/*set all bits*/ {
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_DTR_BITS, !val);
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_RTS_BITS, !val);
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT1_BITS, val);
		dw_uart_set_modem_ctl_bits(p_uart_dev, UART_MCR_OUT2_BITS, val);
		info("set all dtr/rts/out1/out2 to %d\n", val);
	}
	info("uart_modem_master: refer slave side console output for pass or fail\n");
	return 0;
}

void uart_line_int_handler(void* ptr)
{
	struct dw_uart *p_uart = ptr;
	int int_status = 0;
	unsigned long val;
	char bval;

	info("%s\n", __func__);

	int_status = dw_uart_get_int_status(p_uart);
	info("%s, status=%x\n", __func__, int_status);
	if (int_status != 0x6) {
		return;
	}

	val = dw_uart_get_line_status(p_uart);
	if (val & UART_LSR_BI) {
		line_int_counter++;
		/* info("!!! Slave: Detect break sequence. !!!\n"); */
		bval = dw_uart_getc(p_uart);
		if (bval !=0) {
			info("break sequence received wrong char:0x%x\n",bval);
		}
	} else {
		info("line status irq, but no break.\n");
	}
}

/*testcase:line break test*/
static int uart_line(int argc, char* argv[])
{
	info("uart_line_bg: line break test\n");
	line_int_counter = 0;
	dw_uart_enable_interrupt(p_uart_dev, 0);
	dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200);

	uart_int_in_use[p_uart_dev->line] = 1;
	uart_echo_dev_irq = *p_uart_dev;

	/*register int handler and enable it*/
	irq_generic_install_handler(p_uart_dev->int_vector,
			uart_line_int_handler,
			&uart_echo_dev_irq);
	irq_generic_config_int(p_uart_dev->int_vector, IRQ_TYPE_LEVEL_LOW);
	irq_generic_enable_int(p_uart_dev->int_vector);

	/*enable interrupt*/
	dw_uart_enable_interrupt(p_uart_dev, UART_IER_RLSI);

	info("uart line detect run in background\n");

	return 0;
}

/*testcase: master side of line break test*/
static int uart_line_master(int argc, char* argv[])
{
	unsigned long timeout_ms;
	unsigned long long start;

	info("uart_line_master:master side break test\n");

	if (argc < 2) {
		info("wrong cmd_format: %s break_length_ms\n",argv[0]);
		return 0;
	}
	line_int_counter = 0;
	/*ms between set break and clear it*/
	timeout_ms = get_arg_ulong(argv[1]);

	dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200);

	debug("set line break\n");
	start = get_timer(0);
	disable_interrupts();
	if(timeout_ms != 0)
	{
		dw_uart_set_break(p_uart_dev, 1);
		
		while(get_timer(start) < timeout_ms)
			udelay(100);
		dw_uart_set_break(p_uart_dev, 0);
	}
	enable_interrupts();
	mdelay(10); /* in case user input timeout < 1 character time, for 115200 it's 86 us*/
	info("test %s\n", line_int_counter ? "pass" : "fail");
	return 0;
}

static int uart_int_counter = 0;
static int predef_int_status;
void uart_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = dw_uart_get_int_status(p_uart_dev);
	if (int_status != 0x1 && predef_int_status == int_status) {
		uart_int_counter++;
	}
	info("uart_int_handler,status=%x\n", int_status);
	if (int_status == 0) {
		/*read modem status to clear it*/
		info("modem status change int, MSR: 0x%x\n",
				dw_uart_get_modem_status(p_uart_dev));
	} else if(int_status == UART_IIR_THRI) {
		info("Transmit holding register empty int.\n");
	} else if (int_status == UART_IIR_RLSI) {
		/*read line status to clear it*/
		info("line status change int, LSR: 0x%x\n",
				dw_uart_get_line_status(p_uart_dev));
	} else if (int_status == UART_IIR_RDI || int_status == UART_IIR_CHTO) {
		if(int_status == 4) {
			info("receive data available int, rx fifo level %d\n",
					dw_uart_get_rcv_fifo_level(p_uart_dev));
		} else {
			info("character timeout int, rx fifo level %d\n",
					dw_uart_get_rcv_fifo_level(p_uart_dev));
		}
		dw_uart_enable_interrupt(p_uart_dev, 0);
	}
	dw_uart_empty_fifo(p_uart_dev);
}

/*testcase:uart interrupt test*/
static int uart_int(int argc, char* argv[])
{
	int fail = 0;
	int is_tx = 1;
	int rx_thres = UART_FIFO_DEPTH/4;
	int rx_fill_level = rx_thres;
	unsigned long fcr_rx_thres = 0;
	unsigned long ien;
	int time_used = 0;

	info("%s: interrupt handle test,input 'q' to quit this testcase\n",
		argv[0]);

	uart_int_counter = 0;

	if(argc < 8) {
		info("wrong cmd_format, usage: %s dma baud parity bits stop_bits flow_ctrl tx|[rx rx_threshold rx_fill_level]\n",
				__func__);
		return -1;
	}

	if (uart_init(p_uart_dev, argc,argv))
		return 0;
	if (argc >= 8) {
		if(strcmp(argv[7], "tx") && strcmp(argv[7], "rx")) {
			info("wrong int type, should be tx or rx\n");
			return -1;
		}
		if(strcmp(argv[7], "tx"))
			is_tx = 0;
	}
	dw_uart_get_modem_status(p_uart_dev);
	dw_uart_get_int_status(p_uart_dev);
	if(is_tx) {
		predef_int_status = UART_IIR_THRI;
	} else {
		if (argc >= 9)
			rx_thres = get_arg_ulong(argv[8]);
		switch(rx_thres) {
		case 1:
			fcr_rx_thres = UART_FCR_RX_TRIGGER_1;
			break;
		case UART_FIFO_DEPTH/4:
			fcr_rx_thres = UART_FCR_RX_TRIGGER_QUARTER;
			break;
		case UART_FIFO_DEPTH/2:
			fcr_rx_thres = UART_FCR_RX_TRIGGER_HALF;
			break;
		case UART_FIFO_DEPTH-2:
			fcr_rx_thres = UART_FCR_RX_TRIGGER_FULL;
			break;
		default:
			info("wrong rx threshold, should be one of 1/%d/%d/%d\n",
					UART_FIFO_DEPTH/4,
					UART_FIFO_DEPTH/2,
					UART_FIFO_DEPTH-2);
			return -1;
		}
		dw_uart_set_fcr_rx_thres(p_uart_dev, fcr_rx_thres);
		// change the echo threshold as well 
		dw_uart_set_fcr_rx_thres(&uart_echo_dev_irq, fcr_rx_thres); // change the echo threshold as well 
		// for echo channel without DMA to avoid deadlock
        uart_echo_dev_irq.dma_mode = 0;
		
		if(argc >= 10)
			rx_fill_level = get_arg_ulong(argv[9]);
		predef_int_status = UART_IIR_RDI;
		if(rx_fill_level < rx_thres)
			predef_int_status = UART_IIR_CHTO;
		info("rx thres %d fcr 0x%x rx_fill_level %d\n", rx_thres, fcr_rx_thres, rx_fill_level);
	}
	info("predef_int_status 0x%x\n", predef_int_status);

	/*register int handler and enable it*/
	irq_generic_install_handler(p_uart_dev->int_vector,
		uart_int_handler,
		NULL);
	irq_generic_config_int(p_uart_dev->int_vector, IRQ_TYPE_LEVEL_LOW);
	irq_generic_enable_int(p_uart_dev->int_vector);

	/*enable interrupt*/
	ien = UART_IER_MSI | UART_IER_RLSI;
	if (is_tx)
		ien |= UART_IER_THRI;
	else
		ien |= UART_IER_RDI;

	//wait interrupt handle, and wait user break testcase
	uart_generate_random_data(1,256,0);
	if (is_tx) {
		dw_uart_write(p_uart_dev, (const char*)test_send_buff, 16);
		dw_uart_enable_interrupt(p_uart_dev, ien);
	} else {
		dw_uart_write(p_uart_dev, (const char*)test_send_buff, rx_fill_level);
		//info("sent %d bytes, no receive data available irq should be generated.\n", rx_thres-1);
		dw_uart_enable_interrupt(p_uart_dev, ien);
	}

	while(time_used++ < 5000 && uart_int_counter == 0)
		mdelay(1);

	if (uart_int_counter == 0) {
		fail++;
	}

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	/*disable interrupt*/
	irq_generic_disable_int(p_uart_dev->int_vector);
	irq_generic_remove_handler(p_uart_dev->int_vector,NULL,NULL);
	dw_uart_enable_interrupt(p_uart_dev, 0);

	return 0;
}

void uart_dma_block_int_handler(void* ptr)
{
	int int_status = 0;
	int ch_rx_status = 0;
	int ch_tx_status = 0;
	int ch_tx_err = 0;
	int ch_rx_err = 0;
	struct dw_dmac* dma;
	struct dw_uart* uart;

	uart = ptr;
	dma = &dma_dev_list[uart->dmac_idx];

	int_status = dw_dmac_get_int_status(dma);
	ch_rx_status = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_rx,
					DMAC_TFR);
	ch_rx_err = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_rx,
					DMAC_ERR);
	ch_tx_status = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_tx,
					DMAC_TFR);
	ch_tx_err = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_tx,
					DMAC_ERR);
	if (ch_rx_status) {
		xfer_debug("uart rx dma done\n");
		uart->uart_dma_rx_done = 1;
	}
	if (ch_tx_status) {
		xfer_debug("uart tx dma done\n");
		uart->uart_dma_tx_done = 1;
	}
	if (ch_tx_err) {
		info("uart tx dma err\n");
		uart->uart_dma_tx_done = -1;
	}
	if (ch_rx_err) {
		info("uart rx dma err\n");
		uart->uart_dma_rx_done = -1;
	}
	if (ch_rx_status || ch_rx_err) {
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_TFR);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_BLOCK);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_SRC_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_DST_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_ERR);
	}
	if (ch_tx_status || ch_tx_err) {
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_TFR);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_BLOCK);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_SRC_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_DST_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_ERR);
	}
}

unsigned char *test_recv_buff_echo;

void uart_dma_block_echo_int_handler(void* ptr)
{
	int int_status = 0;
	int ch_rx_status = 0;
	int ch_tx_status = 0;
	int ch_tx_err = 0;
	int ch_rx_err = 0;
	struct dw_dmac* dma;
	struct dw_uart* uart;

	uart = ptr;
	dma = &dma_dev_list[uart->dmac_idx];

	int_status = dw_dmac_get_int_status(dma);
	ch_rx_status = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_rx,
					DMAC_TFR);
	ch_rx_err = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_rx,
					DMAC_ERR);
	ch_tx_status = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_tx,
					DMAC_TFR);
	ch_tx_err = dw_dmac_get_ch_int_status(dma,
					uart->dma_ch_tx,
					DMAC_ERR);
	if (ch_rx_status) {
		xfer_debug("uart rx dma echo done\n");
		uart->uart_dma_rx_done = 1;
	}
	if (ch_tx_status) {
		xfer_debug("uart tx dma echo done\n");
		uart->uart_dma_tx_done = 1;
	}
	if (ch_tx_err) {
		info("uart tx dma echo err\n");
		uart->uart_dma_tx_done = -1;
	}
	if (ch_rx_err) {
		info("uart rx dma echo err\n");
		uart->uart_dma_rx_done = -1;
	}
	if (ch_rx_status || ch_rx_err) {
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_TFR);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_BLOCK);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_SRC_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_DST_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_rx,
						DMAC_ERR);
	}
	if (ch_tx_status || ch_tx_err) {
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_TFR);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_BLOCK);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_SRC_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_DST_TRAN);
		dw_dmac_set_ch_int_clear(dma,
						uart->dma_ch_tx,
						DMAC_ERR);
	}

	if(uart->uart_dma_rx_done > 0) {
		uart->uart_dma_rx_done = 0;
		xfer_debug("%s rx done\n", __func__);
		dw_uart_write_mb(uart, (const char*)test_recv_buff_echo, uart->dma_block_xfer_len);
	}
	if(uart->uart_dma_tx_done > 0) {
		uart_dma_mb_count--;
		uart->uart_dma_tx_done = 0;
		xfer_debug("%s tx done\n", __func__);
		if(uart_dma_mb_count > 0)
			dw_uart_read_mb(uart, (char*)test_recv_buff_echo, uart->dma_block_xfer_len);
	}
}

/* using dma multi-block & multi-channel mode; use interrupt to indicate dma done*/
static int uart_dma_block(int argc,char* argv[])
{
    int ret = 0;
    int count = 1; /*set default test loop as '1' */
    int fail = 0;
    unsigned char pattern = 0x55;
    int random_flag = 1;
    unsigned long act_count=0;
    int len = 0;
    int is_echo_mode = 0;
    struct dw_uart *uart = NULL;
	volatile unsigned char* p_tx_lli;
	volatile unsigned char* p_rx_lli;

    info("%s: transfer using dma multi-block & multi-channel mode with interrupt test\n", __func__);

    if (argc < 8) {
                info("wrong cmd_format:%s dma_mode baudrate parity data_bits stop_bits flow_ctl len [cnt] [pattern]",
                            argv[0]);
                return 0;
    }
    if(!strcmp(argv[0], "uart_dma_block_echo"))
                is_echo_mode = 1;
    uart = is_echo_mode ? &uart_echo_dev_irq : &uart_dev_irq;

	//avoid memory leakage
	p_rx_lli = uart->p_rx_lli;
	p_tx_lli = uart->p_tx_lli;
	
    *uart = *p_uart_dev;
	//avoid memory leakage
	uart->p_rx_lli = p_rx_lli;
	uart->p_tx_lli = p_tx_lli;

    if (uart_init(uart, argc, argv))
                return 0;
    len = get_arg_ulong(argv[7]);
    
    if(len > uart_buf_size) {
                info("length too large, not enough buffer\n");
                return -1;
    }
    if (argc >=9){
                count = get_arg_ulong(argv[8]);
    }
    if (argc >=10) {
                pattern = get_arg_ulong(argv[9]);
                random_flag = 0;
    }
	uart_dma_mb_count = count;

    dma_int_in_use[uart->line] = 1;
    p_dma_test = &dma_dev_list[uart->dmac_idx];

    dw_dmac_set_ch_int_clear(p_dma_test,  uart->dma_ch_tx, DMAC_TFR);
    dw_dmac_set_ch_int_clear(p_dma_test,  uart->dma_ch_tx, DMAC_ERR);         
    dw_dmac_set_ch_int_clear(p_dma_test,  uart->dma_ch_rx, DMAC_TFR);
    dw_dmac_set_ch_int_clear(p_dma_test,  uart->dma_ch_rx, DMAC_ERR);

    /*register int handler and enable it*/
    irq_generic_install_handler(p_dma_test->int_vector,
                is_echo_mode ? uart_dma_block_echo_int_handler : uart_dma_block_int_handler,
                uart);
    irq_generic_config_int(p_dma_test->int_vector, IRQ_TYPE_LEVEL_LOW);
    irq_generic_enable_int(p_dma_test->int_vector);
	
    /*enable the ch interrupt*/
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_tx, DMAC_TFR, 1);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_tx, DMAC_ERR, 1);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_rx, DMAC_TFR, 1);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_rx, DMAC_ERR, 1);

    if(!is_echo_mode) {
        if (random_flag)
                    info("test len %d bytes using random pattern, loop count=%d\n",
                                len,count);
        else
                    info("test len %d bytes using fixed pattern '0x%x', loop count=%d\n",
                                len,pattern,count);
    }

    while (count--) {
        if (console_check_char('q'))
                    goto fail;

        /*generate random pattern*/
        uart_generate_random_data(random_flag,len,pattern);

        uart->uart_dma_tx_done = 0;
        uart->uart_dma_rx_done = 0;
        uart->dma_block_xfer_len = len;

        ret = dw_uart_read_mb(uart, (char*)(is_echo_mode ? test_recv_buff_echo : test_recv_buff), len);
        if(ret)
            goto fail;
        if(is_echo_mode) {
            info("%s server in background!\n", __func__);
            return 0;
        }
        info("loop %d err %d\n",act_count,fail);
        act_count++;	
		if(!is_echo_mode) {
	        ret = dw_uart_write_mb(uart, (const char*)test_send_buff, len);
	        if(ret)
	                    goto fail;

	        /*wait interrupt for tx/rx done*/
	        while (!uart->uart_dma_tx_done) {
	            if(uart->uart_dma_tx_done < 0)
	                        goto fail;
	            if (console_check_char('q'))
	                        goto fail;
	        }
	        while (!uart->uart_dma_rx_done){
	            if(uart->uart_dma_rx_done < 0)
	                        goto fail;
	            if (console_check_char('q'))
	                        goto fail;
	        }

	        /*compare result*/
	        if (uart_compare_result(uart, len))
	            fail++;
	    }
	}

    goto end;

fail:
    fail++;
end:

    //echo side
    p_dma_test = &dma_dev_list[uart_echo_dev_irq.dmac_idx];          
    irq_generic_disable_int(p_dma_test->int_vector);
    irq_generic_remove_handler(p_dma_test->int_vector, NULL, NULL);
    dw_dmac_channel_abort(p_dma_test, uart->dma_ch_rx);
    dw_dmac_channel_abort(p_dma_test, uart->dma_ch_tx);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_tx, DMAC_TFR, 0);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_tx, DMAC_ERR, 0);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_rx, DMAC_TFR, 0);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_rx, DMAC_ERR, 0);  

	//host side
    p_dma_test = &dma_dev_list[uart_dev_irq.dmac_idx];
    irq_generic_disable_int(p_dma_test->int_vector);
    irq_generic_remove_handler(p_dma_test->int_vector, NULL, NULL);
    dw_dmac_channel_abort(p_dma_test, uart->dma_ch_rx);
    dw_dmac_channel_abort(p_dma_test, uart->dma_ch_tx);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_tx, DMAC_TFR, 0);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_tx, DMAC_ERR, 0);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_rx, DMAC_TFR, 0);
    dw_dmac_set_ch_int_mask(p_dma_test, uart->dma_ch_rx, DMAC_ERR, 0);    
	
    info("total %d loop,failed %d\n",act_count,fail);
    if (fail) {
                info("%s: test fail\n",argv[0]);
    } else {
                info("%s: test pass\n",argv[0]);
    }
	if (uart_echo_dev_irq.p_rx_lli) {
		free((void *)uart_echo_dev_irq.p_rx_lli);
		uart_echo_dev_irq.p_rx_lli = NULL;
	}
	if (uart_echo_dev_irq.p_tx_lli) {
		free((void *)uart_echo_dev_irq.p_tx_lli);
		uart_echo_dev_irq.p_rx_lli = NULL;
	}
	if (uart_dev_irq.p_rx_lli) {
		free((void *)uart_dev_irq.p_rx_lli);
		uart_dev_irq.p_rx_lli = NULL;
	}
	if (uart_dev_irq.p_tx_lli) {
		free((void *)uart_dev_irq.p_tx_lli);
		uart_dev_irq.p_rx_lli = NULL;
	}	
	dbg_mem_stat();
	return 0;
}

/*uart sw reset testcase*/
static int uart_reset(int argc, char* argv[])
{
	unsigned long val;
	unsigned long wval;
	int fail = 0;
	int i;

	info("%s: sw reset test\n",argv[0]);

	dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200);


	for (i=0;i<5;i++) {
		srand(get_ticks());
		wval = cb_rand() & 0x7f;
		writel(wval,(u32)p_uart_dev->regs+0xc);
		info("write reg 0xc: 0x%x\n",wval);
		val = readl((u32)p_uart_dev->regs+0xc);
		if (val != wval) {
			fail++;
			info("write 0x%x readback 0x%x\n",wval,val);
			break;
		}
	}
	val = readl((u32)p_uart_dev->regs+0xc);
	info("before reset, reg 0xc val: 0x%x\n",val);
	/*do sw reset, the register value should back to default 0*/
	writel(1,(u32)p_uart_dev->regs+0x88);
	val = readl((u32)p_uart_dev->regs+0xc);
	info("after reset, reg 0xc val: 0x%x\n",val);
	if (val !=0)
		fail++;

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}
/*debug,uart send with fixed string*/
static int uart_send(int argc, char* argv[])
{
	int len;
	unsigned long dma_mode;

	info("%s: send fixed string\n",argv[0]);
	if (argc < 2) {
		info("wrong cmd_format: %s dma_mode\n",argv[0]);
		return 0;
	}
	dma_mode = get_arg_ulong(argv[1]);

	p_uart_dev->dma_mode = dma_mode;
	/* dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200); */

	strcpy((char*)test_send_buff,"abcdefghijklmnopqrstuvwxyz0123456789");
	len = strlen((char*)test_send_buff);
	dw_uart_write(p_uart_dev, (const char*)test_send_buff, len);

	return 0;
}
/*debug,uart received and display received char*/
static int uart_recv(int argc, char* argv[])
{
	int len;
	unsigned long dma_mode;
	char cc;

	info("%s:received and display,press 'q' to quit\n",argv[0]);
	if (argc < 2) {
		info("wrong cmd_format: %s dma_mode\n",argv[0]);
		return 0;
	}
	dma_mode = get_arg_ulong(argv[1]);

	p_uart_dev->dma_mode = dma_mode;
	/* dw_uart_init(p_uart_dev, p_uart_dev->pclk, 115200); */

	while (1) {
		if (console_check_char('q'))
			break;

		memset(test_recv_buff,0, uart_buf_size);
		len = dw_uart_read_non_block(p_uart_dev,
			(char*)test_recv_buff, 512,10);
		if (len>0)
			info("recv: %d bytes:%s\n",len,test_recv_buff);
	}
	return 0;
}

static int uart_cleanup(int argc, char* argv[]) {
	p_dma_test = &dma_dev_list[p_uart_dev->dmac_idx];

	info("%s\n", __func__);

	if(dma_int_in_use[p_uart_dev->line]) {
		irq_generic_disable_int(p_dma_test->int_vector);
		irq_generic_remove_handler(p_dma_test->int_vector, NULL, NULL);
		dw_dmac_channel_abort(p_dma_test, p_uart_dev->dma_ch_rx);
		dw_dmac_channel_abort(p_dma_test, p_uart_dev->dma_ch_tx);
		dw_dmac_set_ch_int_mask(p_dma_test, p_uart_dev->dma_ch_tx, DMAC_TFR, 0);
		dw_dmac_set_ch_int_mask(p_dma_test, p_uart_dev->dma_ch_tx, DMAC_ERR, 0);
		dw_dmac_set_ch_int_mask(p_dma_test, p_uart_dev->dma_ch_rx, DMAC_TFR, 0);
		dw_dmac_set_ch_int_mask(p_dma_test, p_uart_dev->dma_ch_rx, DMAC_ERR, 0);
		info("%s stop dma, disable dma irq, unregister irq handlers\n", __func__);
	}
	if(uart_int_in_use[p_uart_dev->line]) {
		irq_generic_disable_int(p_uart_dev->int_vector);
		irq_generic_remove_handler(p_uart_dev->int_vector, NULL, NULL);
		dw_uart_enable_interrupt(p_uart_dev, 0);
		info("%s disable uart irq, unregister irq handlers\n", __func__);
	}
	info("done");
	return 0;
}

static int console_switch(int argc, char* argv[])
{
	unsigned long new_port;

	info("%s:switch console to other uart port\n",argv[0]);
	if (argc < 2) {
		info("wrong cmd_format: %s new_uart[0-3]\n",argv[0]);
		return 0;
	}
	new_port = get_arg_ulong(argv[1]);
	if (new_port ==0)
		console_init(UART0_BASE,UART_PCLK);
	else if (new_port == 1)
		console_init(UART1_BASE,UART_PCLK);
	else if (new_port ==2)
		console_init(UART2_BASE,UART_PCLK);
	else if (new_port ==3)
		console_init(UART3_BASE,UART_PCLK);
	else {
		info("invalid port %d\n",new_port);
		return 0;
	}
	info("switch console done,press 'enter' to active new console\n");
	console_switched = 1;
	return 0;
}

#define UART_REG_LEN_IN_DWORD 64
static int uart_reg_dump(int argc, char* argv[])
{
	uint32_t id;
	uint32_t addr_array[4] = {UART0_BASE, UART1_BASE, UART2_BASE, UART3_BASE};
	
	info("uart_reg_dump for observe regisster\n");

	if (argc < 2) 
	{
		info("wrong cmd_format: %s id(0~3)\n", argv[0]);
		return 0;
	}
	
	id = get_arg_ulong(argv[1]);
	if (id > 3)
	{
		info("uart is should from 0-3\n");
		return 0;
	}

	dump_memory32(addr_array[id], UART_REG_LEN_IN_DWORD);
	return 0;
}

static int four_uarts_tx(int argc, char* argv[]) {
     struct dw_uart uart_dev_list[4];
	 
     uart_dev_list[0].regs = (void*)UART0_BASE;
     uart_dev_list[0].dmac_idx = 2;
     uart_dev_list[0].dma_ch_tx = 0;
     uart_dev_list[0].dma_ch_rx = 1;
     uart_dev_list[0].dma_req_tx = DMA_REQ_UART0_TX;
     uart_dev_list[0].dma_req_rx = DMA_REQ_UART0_RX;
     uart_dev_list[0].int_vector = INT_UART0;
     dw_uart_init(&uart_dev_list[0], UART_PCLK, 115200);

     uart_dev_list[1].regs = (void*)UART1_BASE;
     uart_dev_list[1].dmac_idx = 0;
     uart_dev_list[1].dma_ch_tx = 0;
     uart_dev_list[1].dma_ch_rx = 1;
     uart_dev_list[1].dma_req_tx = DMA_REQ_UART1_TX;
     uart_dev_list[1].dma_req_rx = DMA_REQ_UART1_RX;
     uart_dev_list[1].int_vector = INT_UART1;
     dw_uart_init(&uart_dev_list[1], UART_PCLK, 460800);

     uart_dev_list[2].regs = (void*)UART2_BASE;
     uart_dev_list[2].dmac_idx = 1;
     uart_dev_list[2].dma_ch_tx = 0;
     uart_dev_list[2].dma_ch_rx = 1;
     uart_dev_list[2].dma_req_tx = DMA_REQ_UART2_TX;
     uart_dev_list[2].dma_req_rx = DMA_REQ_UART2_RX;
     uart_dev_list[2].int_vector = INT_UART2;
     dw_uart_init(&uart_dev_list[2], UART_PCLK, 921600);

     uart_dev_list[3].regs = (void*)UART3_BASE;
     uart_dev_list[3].dmac_idx = 1;
     uart_dev_list[3].dma_ch_tx = 2;
     uart_dev_list[3].dma_ch_rx = 3;
     uart_dev_list[3].dma_req_tx = DMA_REQ_UART3_TX;
     uart_dev_list[3].dma_req_rx = DMA_REQ_UART3_RX;
     uart_dev_list[3].int_vector = INT_UART3;
     dw_uart_init(&uart_dev_list[3], UART_PCLK, 3686400);

     info("uart 0 must be log uart, please observe tx bps of each uart, press ' q' to exit\n");

     while(1)
     {
    	 dw_uart_putc(&uart_dev_list[0], 0x5a);
         dw_uart_putc(&uart_dev_list[1], 0x5a);
         dw_uart_putc(&uart_dev_list[2], 0x5a);
         dw_uart_putc(&uart_dev_list[3], 0x5a);
         if(console_check_char('q'))
         {
             break;
         }
     }

     return 0;
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry uart_test_menu[] = {
	{ "help", cmd_help, " : Display list of commands" },
	{ "h",	cmd_help, 	": alias for help" },
	{ "?",	cmd_help, 	": alias for help" },
	{ "uart_loopback",uart_loopback,  ": uart loopback tx/rx test" },
	{ "uart_external_loopback",uart_external_loopback,  ": uart external loopback tx/rx test" },
	{ "uart_modem_loopback",uart_modem_loopback,": uart loopback modem ctrl test" },
	{ "uart_transfer",uart_transfer,": uart dma mode,baudrate,parity,data bits,stop bits test" },
	{ "uart_flowctrl",uart_flowctrl,  ": uart rs232 tx/rx with flowctrl test" },
	{ "uart_echo",uart_echo,": uart echo server in background" },
	{ "uart_modem",uart_modem,  "  : uart modem ctrl detect test" },
	{ "uart_modem_master",uart_modem_master,": uart modem ctrl control test" },
	{ "uart_line",uart_line,  "  : uart line break detect test in background" },
	{ "uart_line_master",uart_line_master,":uart line break generate test" },
	{ "uart_int",uart_int,  "  : uart interrupt test" },
	{ "uart_dma_block_echo",uart_dma_block,  "  : uart dma multiblock with interrupt test" },
	{ "uart_dma_block",uart_dma_block,  "  : uart dma multiblock with interrupt test" },
	{ "uart_reset",uart_reset,  "  : uart sw reset test" },
	{ "uart_cfg", uart_cfg,  ": uart port configure" },
	{ "uart_send",uart_send,  "  : debug send" },
	{ "uart_recv",uart_recv,  "  : debug receive" },
	{ "uart_cleanup", uart_cleanup,  "  : stop dma, disable dma irq & uart irq, unregister irq handlers" },
	{ "console_switch",console_switch,  "  : switch current console to other uart port" },
	{ "uart_reg_dump",uart_reg_dump,  "  : dump reg" },
    { "4_uart_show", four_uarts_tx, " : four uart puts char out"},
	{ "q", NULL, " : quit uart test" },{ 0, 0, 0 }
};

// if return none zero, something wrong,please check and can't continue test
int uart_test_prepare(int sel)
{
	int ret = 0;

	if ((sel == 0) && (current_console_base == UART0_BASE)) 
	{
		ret = 1;
	} 
	else if ((sel == 1) && (current_console_base == UART1_BASE)) 
	{
		ret = 1;
	} 
	else if ((sel == 2) && (current_console_base == UART2_BASE)) 
	{
		ret = 1;
	} 
	else if ((sel == 3) && (current_console_base == UART3_BASE)) 
	{
		ret = 1;
	}

	if (ret) 
	{
		info("current console use uart %d\n", sel);
		info("please change console setting to test uart %d\n", sel);
	} 
	else 
	{
		memset(p_uart_dev, 0, sizeof(struct dw_uart));
		p_uart_dev->line = sel;

		if (!misc_uart_pin_init(sel))	
		{
			return 1;
		}

		if (sel == 0) 
		{
			p_uart_dev->regs = (void*)UART0_BASE;
			p_uart_dev->dmac_idx = 2;
			p_uart_dev->dma_ch_tx = 0;
			p_uart_dev->dma_ch_rx = 1;
			p_uart_dev->dma_req_tx = DMA_REQ_UART0_TX;
			p_uart_dev->dma_req_rx = DMA_REQ_UART0_RX;
			p_uart_dev->int_vector = INT_UART0;
			dw_uart_init(p_uart_dev, UART_PCLK, 115200);
		} 
		else if (sel == 1) 
		{
			p_uart_dev->regs = (void*)UART1_BASE;
			p_uart_dev->dmac_idx = 0;
			p_uart_dev->dma_ch_tx = 0;
			p_uart_dev->dma_ch_rx = 1;
			p_uart_dev->dma_req_tx = DMA_REQ_UART1_TX;
			p_uart_dev->dma_req_rx = DMA_REQ_UART1_RX;
			p_uart_dev->int_vector = INT_UART1;
			dw_uart_init(p_uart_dev, UART_PCLK, 115200);
		} 
		else if (sel == 2) 
		{
			p_uart_dev->regs = (void*)UART2_BASE;
			p_uart_dev->dmac_idx = 1;
			p_uart_dev->dma_ch_tx = 0;
			p_uart_dev->dma_ch_rx = 1;
			p_uart_dev->dma_req_tx = DMA_REQ_UART2_TX;
			p_uart_dev->dma_req_rx = DMA_REQ_UART2_RX;
			p_uart_dev->int_vector = INT_UART2;
			dw_uart_init(p_uart_dev, UART_PCLK, 115200);
		} 
		else if (sel == 3) 
		{
			p_uart_dev->regs = (void*)UART3_BASE;
			p_uart_dev->dmac_idx = 1;
			p_uart_dev->dma_ch_tx = 2;
			p_uart_dev->dma_ch_rx = 3;
			p_uart_dev->dma_req_tx = DMA_REQ_UART3_TX;
			p_uart_dev->dma_req_rx = DMA_REQ_UART3_RX;
			p_uart_dev->int_vector = INT_UART3;
			dw_uart_init(p_uart_dev, UART_PCLK, 115200);
		} 
		else 
		{
			info("wrong device id\n");
			ret = 1;
		}
		
		uart_buf_size = TEST_BUFFER_SIZE / 2;
		test_recv_buff_echo = test_recv_buff + uart_buf_size;
	}
	
	return ret;
}

#define UART_IER_DEF_VAL	0x0
bool uart_reset_test(E_UART_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_IP_RST ip;
	struct dw_uart *p_dev = p_uart_dev;

	info("%s: port:%d\n", __func__, port);
	if (uart_test_prepare(port))
	{
		return false;
	}
	
	info("write ier reg\n");
	
	// write register
	dw_uart_writel(p_dev, DW_UART_IER, *p_wr_val);

	info("read&compare ier reg\n");
	*p_rd_val = dw_uart_readl(p_dev, DW_UART_IER);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("uart reset\n");
	if 		(port == UART0)	ip = ARM_UART0_RST;
	else if (port == UART1)	ip = ARM_UART1_RST;
	else if (port == UART2)	ip = ARM_UART2_RST;
	else					ip = ARM_UART3_RST;
	prcm_reset(ip);
	
	// read register
	info("read&compare ier reg\n");
	*p_rd_val = dw_uart_readl(p_dev, DW_UART_IER);

	if (*p_rd_val != UART_IER_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool uart_cg_test(E_UART_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk;
	struct dw_uart *p_dev = p_uart_dev;

	info("%s: port:%d\n", __func__, port);

	if 		(port == UART0)	blk = ARM_BLK_CG_UART0;
	else if (port == UART1)	blk = ARM_BLK_CG_UART1;
	else if (port == UART2)	blk = ARM_BLK_CG_UART2;
	else					blk = ARM_BLK_CG_UART3;

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);	
	
	if (uart_test_prepare(port))
	{
		return false;
	}
	
	// write register
	info("write ier reg\n");
	dw_uart_writel(p_dev, DW_UART_IER, *p_wr_val);

	info("read&compare ier reg\n");
	*p_rd_val = dw_uart_readl(p_dev, DW_UART_IER);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write ier reg\n");
	dw_uart_writel(p_dev, DW_UART_IER, (*p_wr_val+1));

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare ier reg\n");
	*p_rd_val = dw_uart_readl(p_dev, DW_UART_IER);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool uart_reset_test(E_UART_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;	
}

bool uart_cg_test(E_UART_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;	
}

#endif

