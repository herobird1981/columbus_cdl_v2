/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_spi.h"
#include "cdl_pll.h"

#if (CONFIG_SPI_TEST == 1)
#include "cdl_spi.h"

#include "spi_flash.h"

/* bit0: clock phase; bit1: clock polarity*/
#define SPI_MODE_0	(0x0)
#define SPI_MODE_1	(0x1)
#define SPI_MODE_2	(0x2)
#define SPI_MODE_3	(0x3)

struct brite_spi spi_dev;
struct brite_spi* p_spi_dev = &spi_dev;
struct brite_spi spi_dev_slave;
struct brite_spi* p_slave = &spi_dev_slave;

cmdline_entry spi_test_menu[];

/* spi protocol:
  write/read is for master side; slave should be coresponding read/write
  1.for bus width 4-8:  use low 4 bit only
	cmd: 1 byte	bit1-0: 2 write/read 1 write 0 read;
	index: 1 byte	bit1-0: write buf index
			bit3-2: read buf index
	len: 6 byte	(write/read len): all low 4 bit group to 3 byte length;
	data: len bytes
  2. for bus width 9-16
	cmd & index: 1 byte:	bit1-0: 2 write/read 1 write 0 read;
			  	bit3-2: write buf index
			 	bit5-4: read buf index
	reserved: 1 byte
	len: 6 byte	(write/read len): 3 byte length; byte 2,4,6 valid
	data: len bytes
  3. for bus width 17-32
	cmd & index: 1 byte:	bit1-0: 2 write/read 1 write 0 read;
			  	bit3-2: write buf index
			 	bit5-4: read buf index
	len: 7 byte	(write/read len): 3 byte length; byte 1,4,5 valid
	data: len bytes
*/
#define SPI_SLAVE_MAX_SIZE	(10*1024)
unsigned char spi_slave_buf_local[4][SPI_SLAVE_MAX_SIZE];
unsigned char *spi_slave_buf;

unsigned long slave_cmd; /*0 read 1 write 2 read/write*/
unsigned long slave_cmd_tx_idx;/*0-3: slave send/master received*/
unsigned long slave_cmd_rx_idx;/*0-3: master send/slave received*/
unsigned long slave_len;
/*
   0: idle
   1: command phase
   2: data phase: received
   3: data phase: send
   4: data phase: send/receive
   5: wait send complete
*/
int slave_state = 0;
unsigned char slave_cmd_buf[8];
unsigned long slave_cmd_ptr = 0;
unsigned long slave_data_tx_ptr = 0;
unsigned long slave_data_rx_ptr = 0;
int full_deplex = 0;
struct dw_dmac* p_dma_slave;
int slave_rx_dummy = 0;
unsigned long slave_data_rx_len = 0;
unsigned long slave_data_tx_len = 0;

static void generate_spi_cmd(int bus_width,unsigned char*cmd_buf,int cmd,int tid,int rid,int len)
{
	if (bus_width <=8) {
		cmd_buf[0] = cmd & 3;
		cmd_buf[1] = (tid&3) | ((rid&3)<<2);
		cmd_buf[2] = (len >> 20)&0xf;
		cmd_buf[3] = (len >> 16)&0xf;
		cmd_buf[4] = (len >> 12)&0xf;
		cmd_buf[5] = (len >> 8)&0xf;
		cmd_buf[6] = (len >> 4)&0xf;
		cmd_buf[7] = len & 0xf;
	} else if (bus_width <=16) {
		cmd_buf[0] = (cmd &3)| ((tid&3)<<2) | ((rid&3)<<4);
		cmd_buf[1] = 0;
		cmd_buf[2] = (len>>16)&0xff;
		cmd_buf[3] = 0;
		cmd_buf[4] = (len >> 8)&0xff;
		cmd_buf[5] = 0;
		cmd_buf[6] = len & 0xff;
		cmd_buf[7] = 0;
	} else {
		cmd_buf[0] = (cmd &3)| ((tid&3)<<2) | ((rid&3)<<4);
		cmd_buf[1] = (len>>16)&0xff;
		cmd_buf[2] = 0;
		cmd_buf[3] = 0;
		cmd_buf[4] = (len >> 8)&0xff;
		cmd_buf[5] = len & 0xff;
		cmd_buf[6] = 0;
		cmd_buf[7] = 0;
	}
}

static int spi_test_init(int argc,char* argv[])
{
	unsigned long dma_mode;
	unsigned long speed;
	unsigned char type;
	unsigned long phase;
	unsigned long cs;
	unsigned long bus_width;

	if (argc < 7) {
		info("format: %s dma_mode speed type phase cs bus_width\n",argv[0]);
		return 1;
	}
	dma_mode = get_arg_ulong(argv[1]);
	speed = get_arg_ulong(argv[2]);
	type = get_arg_ulong(argv[3]);
	phase = get_arg_ulong(argv[4]);
	cs = get_arg_ulong(argv[5]);
	bus_width = get_arg_ulong(argv[6]);

	if (phase >3)
		phase = SPI_MODE_0;

	if ((bus_width < 4) || (bus_width > 32)) {
		info("bus_width should be 4 <= width <=32,force set 8 for invalid range\n");
		bus_width = 8;
	}
	if (cs > 1) {
		info("cs should be 0/1; force set to 0 for invalid range\n");
		cs = 0;
	}

	brite_spi_init(p_spi_dev,
		p_spi_dev->pclk,
		1,
		speed,
		dma_mode,
		type,
		phase,
		cs,
		bus_width);
	return 0;
}

static int spi_check_data(unsigned char* src,unsigned char*dst,unsigned long len,int bus_width)
{
	int i;
	int step;
	unsigned long mask = 0;
	int fail = 0;
	u16 *ptr16_src,*ptr16_dst;
	u32 *ptr32_src,*ptr32_dst;

	if ((bus_width >= 4) && (bus_width <= 8)) {
		step = 1;
	} else if ((bus_width >= 9) && (bus_width <= 16)) {
		step = 2;
	} else {
		step = 4;
	}
	for (i=0;i<bus_width;i++)
		mask |= 1<<i;

	if (step ==1) {
		for (i = 0; i < len; i++) {
			if ((src[i] & mask) != (dst[i] & mask)) {
				info("spi check data fail @0x%x: expected 0x%x receive 0x%x\n",
					i,
					src[i],
					dst[i]);
				fail++;
				break;
			}
		}
	}else if (step ==2) {
		ptr16_src= (u16*)src;
		ptr16_dst = (u16*)dst;
		for (i = 0; i < len/2; i++) {
			if ((ptr16_src[i] & mask) != (ptr16_dst[i] & mask)) {
				info("spi check data fail @0x%x: expected 0x%x receive 0x%x\n",
					i*2,
					ptr16_src[i],
					ptr16_dst[i]);
				fail++;
				break;
			}
		}
	} else {
		ptr32_src= (u32*)src;
		ptr32_dst = (u32*)dst;
		for (i = 0; i < len/4; i++) {
			if ((ptr32_src[i] & mask) != (ptr32_dst[i] & mask)) {
				info("spi check data fail @0x%x: expected 0x%x receive 0x%x\n",
					i*4,
					ptr32_src[i],
					ptr32_dst[i]);
				fail++;
				break;
			}
		}
	}
	if (fail)
		return 1;
	return 0;
}

static int spi_package_xfer(struct brite_spi* priv, unsigned int bitlen,
	const void* dout, void* din, unsigned long flags)
{
	unsigned long len = bitlen / priv->bits_per_word;
	unsigned long packet_len = 0;
	unsigned long out_offset = (unsigned long)dout;
	unsigned long in_offset = (unsigned long)din;
	int ret;

	while(len >0)
	{
		if(len > 32)
			packet_len = 32;
		else
			packet_len = len;

		len -= packet_len;

		//mdelay(50);
		/*transfer data*/
		ret = brite_spi_xfer(p_spi_dev,
			packet_len * p_spi_dev->bits_per_word,
			(void*)out_offset,
			(void*)in_offset,
			SPI_XFER_ONCE);
		if (ret) {
			info("dma data fail,ret=%d\n", ret);
			return ret;
		}
		if(dout != NULL)
		   out_offset += packet_len;
		if(din != NULL)
		   in_offset += packet_len;
	}

	return ret;
}

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&spi_test_menu[0]);
}

static int spi_slave_debug(int argc, char* argv[])
{
	char cc;
	unsigned long dma_mode;
	unsigned long speed;
	unsigned char type;
	unsigned long phase;
	unsigned long cs;
	unsigned long bus_width;
	unsigned char cmd_buf[8];
	int i;
	int ret;

	info("spi_slave:spi slave mode test,input 'q' to quit this testcase\n");

	if (argc < 7) {
		info("wrong cmd_format: spi_slave dma_mode speed type phase cs bus_width\n");
		return 0;
	}
	dma_mode = get_arg_ulong(argv[1]);
	speed = get_arg_ulong(argv[2]);
	type = get_arg_ulong(argv[3]);
	phase = get_arg_ulong(argv[4]);
	cs = get_arg_ulong(argv[5]);
	bus_width = get_arg_ulong(argv[6]);

	if (phase >3)
		phase = SPI_MODE_0;

	if ((bus_width < 4) || (bus_width > 32)) {
		info("bus_width should be 4 <= width <=32,force set 8 for invalid range\n");
		bus_width = 8;
	}
	if (cs > 1) {
		info("cs should be 0/1; force set to 0 for invalid range\n");
		cs = 0;
	}

	brite_spi_init(p_spi_dev,
		p_spi_dev->pclk,
		0,
		speed,
		dma_mode,
		type,
		phase,
		cs,
		bus_width);

	/*generate random data*/
	srand(get_ticks());
	spi_slave_buf = (unsigned char*)&spi_slave_buf_local[0][0];
	for (i = 0; i < SPI_SLAVE_MAX_SIZE; i++) {
		*(spi_slave_buf+i) = cb_rand() & 0xff;
	}

	while (1) {
		if (console_tstc()) {
			cc = console_getc();
			if (cc == 'q') {
				break;
			}
		}

		/*receive command and length*/
		ret = brite_spi_xfer(p_spi_dev, 1 * p_spi_dev->bits_per_word, NULL, cmd_buf, 0);
		info("0x%x\n", *(u32*)&cmd_buf[0]);
	}

	info("spi_slave: test terminate\n");

	return 0;
}

static int spi_debug(int argc, char* argv[])
{
	int i;
	unsigned long len;
	int ret;
	unsigned long test_val=0;
	u16 *u16_ptr;
	u32 *u32_ptr;
	u32 test_pattern;

	info("%s:spi master mode debug\n",argv[0]);

	if (argc < 9) {
		info("wrong cmd_format: %s dma_mode speed type phase cs bus_width val len\n",argv[0]);
		return 0;
	}
	if (spi_test_init(argc,argv))
		return 0;
	test_val = get_arg_ulong(argv[7]);
	len = get_arg_ulong(argv[8]);
	if (len > SPI_SLAVE_MAX_SIZE) {
		info("spi test slave support max size is %d\n",
			SPI_SLAVE_MAX_SIZE);
		len = SPI_SLAVE_MAX_SIZE;
	}
	if ((p_spi_dev->step == 2) && (len & 1)) {
		info("bus width %d need transfer len is mutiple of 2\n",
			p_spi_dev->bits_per_word);
		return 0;
	}
	if ((p_spi_dev->step == 4) && (len & 3)) {
		info("bus width %d need transfer len is mutiple of 2\n",
			p_spi_dev->bits_per_word);
		return 0;
	}
	if (p_spi_dev->step ==1) {
		test_pattern = test_val &0xff;
		for (i = 0; i < len; i++) {
			test_send_buff[i] = test_val &0xff;
		}
	}else if (p_spi_dev->step ==2) {
		test_pattern = test_val & 0xffff;
		u16_ptr = (u16*)test_send_buff;
		for (i = 0; i < len/2; i++) {
			u16_ptr[i] = test_val &0xffff;
		}
	}else {
		test_pattern = test_val;
		u32_ptr = (u32*)test_send_buff;
		for (i = 0; i < len/4; i++) {
			u32_ptr[i] = test_val;
		}
	}
	info("spi send pattern: 0x%x, total length %d bytes\n",test_pattern,len);
	ret = brite_spi_xfer(p_spi_dev, len * p_spi_dev->bits_per_word, test_send_buff, NULL, SPI_XFER_ONCE);
	if (ret) {
		info("spi send fail,ret=%d\n", ret);
	}
	info("%s: send done\n",argv[0]);
	return 0;
}

static int spi_slave_rx(unsigned char*buf)
{
	unsigned long val;

	val = readl((u32)p_slave->regs + BRITE_SPI_RXREG);
	if (!buf)
		return 0;
	if (p_slave->step ==1) {
		*(buf) = val&0xff;
		return 1;
	}
	else if (p_slave->step==2) {
		*(u16*)(buf) = val&0xffff;
		return 2;
	}
	else {
		*(u32*)buf = val;
		return 4;
	}
}

static int spi_slave_tx(unsigned char*buf)
{
	unsigned long val;

	if (p_slave->step ==1) {
		val = *(buf);
	}
	else if (p_slave->step==2) {
		val = *(u16*)(buf);
	}
	else {
		val = *(u32*)(buf);
	}
	writel(val,(u32)p_slave->regs + BRITE_SPI_TXREG);

	return p_slave->step;
}

static int spi_slave_parse_cmd(void)
{
	int i;

	if (p_slave->bits_per_word <=8) {
		slave_cmd = slave_cmd_buf[0] & 0x3;
		slave_cmd_rx_idx = (slave_cmd_buf[1]) & 0x3;
		slave_cmd_tx_idx = (slave_cmd_buf[1] >> 2) & 0x3;
		slave_len = 0;
		for (i=0;i<5;i++) {
			slave_len |= slave_cmd_buf[i+2] & 0xf;
			slave_len <<=4;
		}
		slave_len |= slave_cmd_buf[7] & 0xf;
	}else if (p_slave->bits_per_word <=16) {
		slave_cmd = slave_cmd_buf[0] & 0x3;
		slave_cmd_rx_idx = (slave_cmd_buf[0] >> 2) & 0x3;
		slave_cmd_tx_idx = (slave_cmd_buf[0] >> 4) & 0x3;
		slave_len = 0;
		slave_len |= slave_cmd_buf[2];
		slave_len <<=8;
		slave_len |= slave_cmd_buf[4];
		slave_len <<=8;
		slave_len |= slave_cmd_buf[6];
	}else {
		slave_cmd = slave_cmd_buf[0] & 0x3;
		slave_cmd_rx_idx = (slave_cmd_buf[0] >> 2) & 0x3;
		slave_cmd_tx_idx = (slave_cmd_buf[0] >> 4) & 0x3;
		slave_len = 0;
		slave_len |= slave_cmd_buf[1];
		slave_len <<=8;
		slave_len |= slave_cmd_buf[4];
		slave_len <<=8;
		slave_len |= slave_cmd_buf[5];
	}

	/*
	info("slave:%s command:tid=%d rid=%d len=%d\n",
		(slave_cmd==2)?"read/write":((slave_cmd==1)?"write":"read"),
		slave_cmd_tx_idx,
		slave_cmd_rx_idx,
		slave_len);
	*/
	if (slave_len > SPI_SLAVE_MAX_SIZE)
	{
		info("slave transfer len is too large\n");
		return 1;
	}
	return 0;
}


static int spi_slave_process(int status)
{
	int ret;
	int spi_state;
	int timeout = 3000; /* ms */
	unsigned long long start = get_timer(0);

	spi_state = readl((u32)p_slave->regs+BRITE_SPI_CSTAT);
	while ((spi_state & SPI_RXAVL)) {
		if (slave_state ==1) {
			slave_cmd_ptr += spi_slave_rx(&slave_cmd_buf[slave_cmd_ptr]);
			if (slave_cmd_ptr >=8) {
				slave_state=2;
				/*parse command*/
				ret = spi_slave_parse_cmd();
				if (ret) {
					slave_state = 1;
					slave_cmd_ptr = 0;
				} else {
					if (slave_cmd ==1) {
						slave_state = 2;
					}
					else if (slave_cmd ==2) {
						slave_state = 4;
						full_deplex = 0;
					}
					else
						slave_state =3;
					slave_data_rx_ptr = 0;
					slave_data_tx_ptr = 0;
				}
				spi_state = readl((u32)p_slave->regs+BRITE_SPI_CSTAT);
				continue;
			}
		}else if ((slave_state ==2) || (slave_state==4)) {
			if (slave_data_rx_ptr < slave_len) {
				slave_data_rx_ptr += spi_slave_rx(spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_rx_idx+slave_data_rx_ptr);
				if (slave_data_rx_ptr >= slave_len) {
					//info("received data len=%d\n",slave_data_rx_ptr);
					if (slave_state ==2) {
						slave_state=1;
						slave_cmd_ptr = 0;
					}else if (slave_state ==4) {
						full_deplex++;
						if (full_deplex>=2) {
							slave_state=5;
							slave_cmd_ptr = 0;
						}
					}
					break;
				}
			}else {
				/*just read the data from fifo and discard*/
				spi_slave_rx(NULL);
			}
		} else {
			/*just read the data from fifo and discard*/
			spi_slave_rx(NULL);
		}
		spi_state = readl((u32)p_slave->regs+BRITE_SPI_CSTAT);
		if(get_timer(start) > timeout) {
			info("spi rx data timeout\n");
			break;
		}
	}

	if ((status & SPI_TXEPT_INTF) && (slave_state==5)) {
		slave_state=1;
		slave_cmd_ptr = 0;
	}
	if ((slave_state ==3)||(slave_state==4)) {/*need transfer to master*/
		if (slave_data_tx_ptr < slave_len) {
			spi_state = readl((u32)p_slave->regs+BRITE_SPI_CSTAT);
			while (!(spi_state & SPI_TXFULL)) {
				slave_data_tx_ptr += spi_slave_tx(spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr);
				if (slave_data_tx_ptr >= slave_len) {
					//info("send data len=%d\n",slave_data_tx_ptr);
					if (slave_state==3) {
						slave_state=5;
						slave_cmd_ptr = 0;
					}else if (slave_state==4) {
						full_deplex++;
						if (full_deplex>=2) {
							slave_state=5;
							slave_cmd_ptr = 0;
						}
					}
					break;
				}
				spi_state = readl((u32)p_slave->regs+BRITE_SPI_CSTAT);
				if(get_timer(start) > timeout) {
					info("spi tx full timeout\n");
					break;
				}
			}
		}
	}
	
	return 0;
}

static void spi_slave_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = brite_spi_get_int_status(p_slave);
	if (int_status != 0) {
		brite_spi_set_int_clear(p_slave,int_status);
		spi_slave_process(int_status);		
	}
}

static void spi_slave_dma_int_handler(void* ptr)
{
	int int_status = 0;
	int ch_rx_status = 0;
	int ch_tx_status = 0;
	int ch_tx_err = 0;
	int ch_rx_err = 0;
	int ret;

	int_status = dw_dmac_get_int_status(p_dma_slave);
	ch_rx_status = dw_dmac_get_ch_int_status(p_dma_slave,
					p_slave->dma_ch_rx,
					DMAC_TFR);
	ch_rx_err = dw_dmac_get_ch_int_status(p_dma_slave,
					p_slave->dma_ch_rx,
					DMAC_ERR);
	ch_tx_status = dw_dmac_get_ch_int_status(p_dma_slave,
					p_slave->dma_ch_tx,
					DMAC_TFR);
	ch_tx_err = dw_dmac_get_ch_int_status(p_dma_slave,
					p_slave->dma_ch_tx,
					DMAC_ERR);
	if (ch_rx_status) {
		if (slave_state ==1) {
				/*parse command*/
				ret = spi_slave_parse_cmd();
				if (ret) {
					slave_state = 1;
					/*restart rx dma*/
					brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
				} else {
					slave_data_rx_ptr = 0;
					slave_data_tx_ptr = 0;
					slave_data_rx_len = 0;
					slave_data_tx_len = 0;
					if (slave_cmd ==1) {
						slave_state = 2;
						/*start rx for data*/
						slave_data_rx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_rx_ptr,NULL,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_rx_idx+slave_data_rx_ptr);
					}
					else if (slave_cmd ==2) {
						slave_state = 4;
						slave_rx_dummy = 0;
						/*start rx/tx for data*/
						slave_data_tx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_rx_idx+slave_data_rx_ptr);
					}
					else {
						slave_state =3;
						slave_rx_dummy = 0;
						/*start tx for data*/
						slave_data_tx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr,NULL);
					}
				}
		}else if (slave_state ==2) {
			//info("received: %d \n",slave_len);
			slave_data_rx_ptr += slave_data_rx_len;
			if (slave_data_rx_ptr < slave_len) {
				/*start rx for data*/
				slave_data_rx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_rx_ptr,NULL,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_rx_idx+slave_data_rx_ptr);
			} else {
				slave_state = 1;
				/*restart rx dma*/
				brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
			}
		} else if (slave_state == 4) {
			//info("full duplex received: %d \n",slave_len);
			/*wait rx dummy done*/
			slave_rx_dummy++;
			if (slave_rx_dummy>=2) {
				slave_rx_dummy = 0;
				slave_data_tx_ptr += slave_data_tx_len;
				slave_data_rx_ptr += slave_data_tx_len;
				if (slave_data_tx_ptr < slave_len) {
					/*start tx/rx for data*/
					slave_data_tx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_rx_idx+slave_data_rx_ptr);
				} else {
					slave_state = 1;
					/*restart rx dma*/
					brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
				}
			}
		} else if (slave_state ==3) {
			/*wait rx dummy done*/
			slave_rx_dummy++;
			if (slave_rx_dummy>=2) {
				slave_rx_dummy = 0;
				slave_data_tx_ptr += slave_data_tx_len;
				if (slave_data_tx_ptr < slave_len) {
					/*start tx for data*/
					slave_data_tx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr,NULL);
				} else {
					slave_state = 1;
					/*restart rx dma*/
					brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
				}
			}
		}
	}
	if (ch_tx_status) {
		//info("%s send %d\n",(slave_state==4)?"duplex":"tx only",slave_len);
		if (slave_state ==3) {
			slave_rx_dummy++;
			if (slave_rx_dummy>=2) {/*tx done,rx done*/
				slave_rx_dummy = 0;
				slave_data_tx_ptr += slave_data_tx_len;
				if (slave_data_tx_ptr < slave_len) {
					/*start tx for data*/
					slave_data_tx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr,NULL);
				} else {
					slave_state = 1;
					/*restart rx dma*/
					brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
				}
			}
		} else if (slave_state ==4) {
			slave_rx_dummy++;
			if (slave_rx_dummy>=2) {
				slave_rx_dummy = 0;
				slave_data_tx_ptr += slave_data_tx_len;
				slave_data_rx_ptr += slave_data_tx_len;
				if (slave_data_tx_ptr < slave_len) {
					/*start tx/rx for data*/
					slave_data_tx_len = brite_spi_xfer_slave(p_slave,slave_len-slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_tx_idx+slave_data_tx_ptr,spi_slave_buf+SPI_SLAVE_MAX_SIZE*slave_cmd_rx_idx+slave_data_rx_ptr);
				} else {
					slave_state = 1;
					/*restart rx dma*/
					brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
				}
			}
		}
	}
	if (ch_tx_err) {
		info("tx err\n");
	}
	if (ch_rx_err) {
		info("rx err\n");
	}
	if (ch_rx_status || ch_rx_err) {
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_rx,
						DMAC_TFR);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_rx,
						DMAC_BLOCK);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_rx,
						DMAC_SRC_TRAN);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_rx,
						DMAC_DST_TRAN);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_rx,
						DMAC_ERR);
	}
	if (ch_tx_status || ch_tx_err) {
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_tx,
						DMAC_TFR);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_tx,
						DMAC_BLOCK);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_tx,
						DMAC_SRC_TRAN);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_tx,
						DMAC_DST_TRAN);
		dw_dmac_set_ch_int_clear(p_dma_slave,
						p_slave->dma_ch_tx,
						DMAC_ERR);
	}
}

static int spi_slave(int argc, char* argv[])
{
	unsigned long dma_mode;
	unsigned long speed;
	unsigned char type;
	unsigned long phase;
	unsigned long cs;
	unsigned long bus_width;
	int i;

	info("%s: spi slave mode using interrupt mode\n",argv[0]);

	if (argc < 7) {
		info("wrong cmd_format: %s dma_mode speed type phase cs bus_width\n",argv[0]);
		return 0;
	}
	dma_mode = get_arg_ulong(argv[1]);
	speed = get_arg_ulong(argv[2]);
	type = get_arg_ulong(argv[3]);
	phase = get_arg_ulong(argv[4]);
	cs = get_arg_ulong(argv[5]);
	bus_width = get_arg_ulong(argv[6]);

	if(dma_mode == 0)
	{
		info("force to use DMA mode for slave to reduce the data loss possibility\n");
		dma_mode = 1;
	}
	if (phase >3)
		phase = SPI_MODE_0;

	if ((bus_width < 4) || (bus_width > 32)) {
		info("bus_width should be 4 <= width <=32,force set 8 for invalid range\n");
		bus_width = 8;
	}
	if (cs > 1) {
		info("cs should be 0/1; force set to 0 for invalid range\n");
		cs = 0;
	}

	memcpy(p_slave,p_spi_dev,sizeof(struct brite_spi));
	brite_spi_init(p_slave,
		p_slave->pclk,
		0,
		speed,
		dma_mode,
		type,
		phase,
		cs,
		bus_width);

	/*init slave register buffer*/
	spi_slave_buf = (unsigned char*)&spi_slave_buf_local[0][0];
	//spi_slave_buf = (unsigned char*)SRAM_BASE;
	for (i = 0; i < SPI_SLAVE_MAX_SIZE; i++) {
		*(spi_slave_buf+i) = i;
		*(spi_slave_buf+SPI_SLAVE_MAX_SIZE+i) = i;
		*(spi_slave_buf+2*SPI_SLAVE_MAX_SIZE+i) = i;
		*(spi_slave_buf+3*SPI_SLAVE_MAX_SIZE+i) = i;
	}

	if (!dma_mode) {
		/*register int handler and enable it*/
		irq_generic_install_handler(p_slave->int_vector,
			spi_slave_int_handler,
			NULL);
		irq_generic_config_int(p_slave->int_vector, IRQ_TYPE_LEVEL_LOW);
		irq_generic_enable_int(p_slave->int_vector);

		slave_state = 1;
		slave_cmd_ptr = 0;
		slave_data_rx_ptr = 0;
		slave_data_tx_ptr = 0;

		/*enable interrupt*/
		brite_spi_set_int_enable(p_slave, 1);
		brite_spi_set_int_mask(p_slave, 0x42);
	}else {
		p_dma_slave = &dma_dev_list[p_slave->dmac_idx];
		/*register int handler and enable it*/
		irq_generic_install_handler(p_dma_slave->int_vector,
			spi_slave_dma_int_handler,
			NULL);
		irq_generic_config_int(p_dma_slave->int_vector, IRQ_TYPE_LEVEL_LOW);
		irq_generic_enable_int(p_dma_slave->int_vector);

		slave_state = 1;
		slave_cmd_ptr = 0;
		slave_data_rx_ptr = 0;
		slave_data_tx_ptr = 0;

		/*enable interrupt*/
		/*enable the ch interrupt*/
		dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_tx, DMAC_TFR, 1);
		//dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_tx, DMAC_BLOCK, 1);
		//dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_tx, DMAC_SRC_TRAN, 1);
		//dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_tx, DMAC_DST_TRAN, 1);
		dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_tx, DMAC_ERR, 1);

		dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_rx, DMAC_TFR, 1);
		//dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_rx, DMAC_BLOCK, 1);
		//dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_rx, DMAC_SRC_TRAN, 1);
		//dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_rx, DMAC_DST_TRAN, 1);
		dw_dmac_set_ch_int_mask(p_dma_slave, p_slave->dma_ch_rx, DMAC_ERR, 1);

		/*start received command*/
		brite_spi_xfer_slave(p_slave,8,NULL,slave_cmd_buf);
	}
	info("%s: spi slave using %s mode,start successfully\n",argv[0],
		dma_mode?"dma":"fifo");
	return 0;
}

static int spi_loopback(int argc, char* argv[])
{
	int i;
	unsigned long len;
	int ret;
	int fail = 0;

	info("spi_loopback:spi master mode loopback test\n\n");

	info("format: %s dma_mode speed type phase cs bus_width len\n",argv[0]);
	info("dma_mode: 0 not using dma mode ; 1 using dma mode\n");
	info("speed: spi data rate/clk freq\n");
	info("type: reserved now\n");
	info("phase: spi mode: 0/1/2/3 ; Clock Polarity and Clock Phase\n");
	info("cs: chip select number\n");
	info("bus_width: 4-32\n\n");
	if (argc < 8) {
		info("wrong format: %s dma_mode speed type phase cs bus_width len\n",argv[0]);
		return 0;
	}
	if (spi_test_init(argc,argv))
		return 0;
	len = get_arg_ulong(argv[7]);
	if (len > TEST_BUFFER_SIZE) {
		info("test len: %d exceed test buffer size: %d,force use max of buffer size\n",
			len,TEST_BUFFER_SIZE);
		len = TEST_BUFFER_SIZE;
	}

	info("loopback test:bus width=%d len=%d\n",p_spi_dev->bits_per_word,len);

	/*generate random data*/
	srand(get_ticks());
	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	/*transfer data*/
	ret = spi_package_xfer(p_spi_dev,
		len * p_spi_dev->bits_per_word,
		test_send_buff,
		test_recv_buff,
		SPI_XFER_ONCE);
	if (ret) {
		info("spi read/write loopback fail,ret=%d\n", ret);
		fail++;
	}

	/*check result*/
	fail += spi_check_data(test_send_buff,test_recv_buff,len,p_spi_dev->bits_per_word);

	if (fail)
		info("%s: test fail\n",argv[0]);
	else
		info("%s: test pass\n",argv[0]);

	return 0;
}

static int spi_master(int argc, char* argv[])
{
	unsigned char cmd_buf[8];
	int i;
	unsigned long len;
	int ret;
	int fail = 0;

	info("%s:spi master mode test\n",argv[0]);

	if (argc < 8) {
		info("wrong cmd_format: %s dma_mode speed type phase cs bus_width len\n",argv[0]);
		return 0;
	}
	if (spi_test_init(argc,argv))
		return 0;
	len = get_arg_ulong(argv[7]);
	if (len > SPI_SLAVE_MAX_SIZE) {
		info("spi test slave support max size is %d\n",
			SPI_SLAVE_MAX_SIZE);
		len = SPI_SLAVE_MAX_SIZE;
	}
	if ((p_spi_dev->step == 2) && (len & 1)) {
		info("bus width %d need transfer len is mutiple of 2\n",
			p_spi_dev->bits_per_word);
		return 0;
	}
	if ((p_spi_dev->step == 4) && (len & 3)) {
		info("bus width %d need transfer len is mutiple of 4\n",
			p_spi_dev->bits_per_word);
		return 0;
	}

	/*generate random data*/
	srand(get_ticks());
	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	/*transfer command first*/
	/*write only,index 0*/
	generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,1,0,0,len);

	ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
	if (ret) {
		info("send write command fail,ret=%d\n", ret);
		fail++;
		goto end;
	}
	/*transfer data*/
	ret = spi_package_xfer(p_spi_dev,
		len * p_spi_dev->bits_per_word,
		test_send_buff,
		NULL,
		SPI_XFER_ONCE);
	if (ret) {
		info("send write data fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*transfer command first*/
	/*read only,index 0*/
	generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,0,0,0,len);

	ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
	if (ret) {
		info("send read command fail,ret=%d\n", ret);
		fail++;
		goto end;
	}
	
	/*transfer data*/
	ret = spi_package_xfer(p_spi_dev,
		len * p_spi_dev->bits_per_word,
		NULL,
		test_recv_buff,
		SPI_XFER_ONCE);
	if (ret) {
		info("read data fail,ret=%d\n", ret);
		fail++;
		goto end;
	}
	
	/*check result*/
	fail += spi_check_data(test_send_buff,test_recv_buff,len,p_spi_dev->bits_per_word);

end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	return 0;
}

/* testcase: spi master/slave tx/rx both transfer test */
static int spi_full_duplex(int argc, char* argv[])
{
	unsigned char cmd_buf[8];
	int i;
	unsigned long len;
	int ret;
	int fail = 0;

	info("%s:spi master mode full duplex test\n",argv[0]);

	if (argc < 8) {
		info("wrong cmd_format: %s dma_mode speed type phase cs bus_width len\n",argv[0]);
		return 0;
	}
	if (spi_test_init(argc,argv))
		return 0;
	len = get_arg_ulong(argv[7]);
	if (len > SPI_SLAVE_MAX_SIZE) {
		info("spi test slave support max size is %d\n",
			SPI_SLAVE_MAX_SIZE);
		len = SPI_SLAVE_MAX_SIZE;
	}
	
	if ((p_spi_dev->step == 2) && (len & 1)) {
		info("bus width %d need transfer len is mutiple of 2\n",
			p_spi_dev->bits_per_word);
		return 0;
	}
	if ((p_spi_dev->step == 4) && (len & 3)) {
		info("bus width %d need transfer len is mutiple of 4\n",
			p_spi_dev->bits_per_word);
		return 0;
	}

	/*generate random data*/
	srand(get_ticks());
	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
		test_send_buff[i+SPI_SLAVE_MAX_SIZE] = cb_rand() & 0xff;
	}
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	/*write spi buffer index 1*/
	/*write only,index 1*/
	generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,1,1,0,len);

	ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
	if (ret) {
		info("send write command fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*transfer data*/
	ret = spi_package_xfer(p_spi_dev,
		len * p_spi_dev->bits_per_word,
		test_send_buff+SPI_SLAVE_MAX_SIZE,
		NULL,
		SPI_XFER_ONCE);
	if (ret) {
		info("send write data fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*write/read, write index 0, read index 1*/
	generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,2,0,1,len);

	ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
	if (ret) {
		info("send write/read command fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*transfer data*/
	ret = spi_package_xfer(p_spi_dev,
		len * p_spi_dev->bits_per_word,
		test_send_buff,
		test_recv_buff+SPI_SLAVE_MAX_SIZE,
		SPI_XFER_ONCE);
	if (ret) {
		info("send/receive data fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*read spi buffer index 0*/
	/*read only,index 0*/
	generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,0,0,0,len);
	ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
	if (ret) {
		info("send read command fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*transfer data*/
	ret = spi_package_xfer(p_spi_dev,
		len * p_spi_dev->bits_per_word,
		NULL,
		test_recv_buff,
		SPI_XFER_ONCE);
	if (ret) {
		info("read data fail,ret=%d\n", ret);
		fail++;
		goto end;
	}

	/*check result*/
	/*index0: write/read to write; then read only to read, should be same*/
	fail += spi_check_data(test_send_buff,test_recv_buff,len,p_spi_dev->bits_per_word);
	/*index1: write to write, then write/read read, should be same*/
	fail += spi_check_data(test_send_buff+ SPI_SLAVE_MAX_SIZE,test_recv_buff+ SPI_SLAVE_MAX_SIZE,len,p_spi_dev->bits_per_word);

end:
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	return 0;
}

static int spi_full_duplex_robust(int argc, char* argv[])
{
	unsigned char cmd_buf[8];
	int i;
	unsigned long len;
	int ret;
	int fail = 0;
	char cc;
	int counter = 0;
	int counter_fail = 0;

	info("%s:spi full duplex robust test\n",argv[0]);

	if (spi_test_init(argc,argv))
		return 0;

	while (1) {
		if (console_tstc()) {
			cc = console_getc();
			if (cc == 'q') {
				break;
			}
		}
		/*generate random data*/
		srand(get_ticks());

		len = cb_rand() % DMAC_MAX_SIZE_PER_BLOCK;
		if (len == 0) {
			len = DMAC_MAX_SIZE_PER_BLOCK;
		}
		if ((p_spi_dev->step == 2)) {
			len &= ~1;
			if (len==0)
				len = 2;
		}
		else if ((p_spi_dev->step == 4)) {
			len &= ~3;
			if (len ==0)
				len = 4;
		} else {
			if (len==0)
				len = 1;
		}
		for (i = 0; i < len; i++) {
			test_send_buff[i] = cb_rand() & 0xff;
			test_send_buff[i+SPI_SLAVE_MAX_SIZE] = cb_rand() & 0xff;
		}
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		/*write spi buffer index 1*/
		/*write only,index 1*/
		generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,1,1,0,len);

		ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
		if (ret) {
			info("send write command fail,ret=%d\n", ret);
			fail++;
			goto failed;
		}
		/*transfer data*/
		ret = spi_package_xfer(p_spi_dev,
			len * p_spi_dev->bits_per_word,
			test_send_buff+SPI_SLAVE_MAX_SIZE,
			NULL,
			SPI_XFER_ONCE);
		if (ret) {
			info("send write data fail,ret=%d\n", ret);
			fail++;
			goto failed;
		}

		/*write/read, write index 0, read index 1*/
		generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,2,0,1,len);

		ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
		if (ret) {
			info("send write/read command fail,ret=%d\n", ret);
			fail++;
			goto failed;
		}

		/*transfer data*/
		ret = spi_package_xfer(p_spi_dev,
			len * p_spi_dev->bits_per_word,
			test_send_buff,
			test_recv_buff+SPI_SLAVE_MAX_SIZE,
			SPI_XFER_ONCE);
		if (ret) {
			info("send/receive data fail,ret=%d\n", ret);
			fail++;
			goto failed;
		}

		/*read spi buffer index 0*/
		/*read only,index 0*/
		generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,0,0,0,len);
		ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
		if (ret) {
			info("send read command fail,ret=%d\n", ret);
			fail++;
			goto failed;
		}

		/*transfer data*/
		ret = spi_package_xfer(p_spi_dev,
			len * p_spi_dev->bits_per_word,
			NULL,
			test_recv_buff,
			SPI_XFER_ONCE);
		if (ret) {
			info("read data fail,ret=%d\n", ret);
			fail++;
			goto failed;
		}

		/*check result*/
		/*index0: write/read to write; then read only to read, should be same*/
		fail += spi_check_data(test_send_buff,test_recv_buff,len,p_spi_dev->bits_per_word);
		/*index1: write to write, then write/read read, should be same*/
		fail += spi_check_data(test_send_buff+ SPI_SLAVE_MAX_SIZE,test_recv_buff+ SPI_SLAVE_MAX_SIZE,len,p_spi_dev->bits_per_word);

failed:
		counter++;
		if (fail) {
			fail = 0;
			counter_fail++;
		}
		info("spi_full_duplex_robust: %d loop, len=%d bytes total failed %d\n",
				counter,len,counter_fail);
		mdelay(100);
	}
	info("spi_full_duplex_robust: total %d loop,total failed %d\n",
				counter,counter_fail);
	return 0;
}

#if (CONFIG_DRIVERS_GIC==1)
static int spi_int_counter = 0;
static int predef_int_status;
static void spi_int_handler(void* ptr)
{
	int int_status = 0;

	int_status = brite_spi_get_int_status(p_spi_dev);
	if (int_status != 0) {
		if((int_status & predef_int_status) == predef_int_status)
			spi_int_counter++;
		brite_spi_set_int_clear(p_spi_dev,int_status);
	}
	info("spi_int_handler, status=0x%x\n", int_status);
	brite_spi_set_int_mask(p_spi_dev, 0);
}
#endif

/*testcase:spi interrupt test*/
static int spi_int(int argc, char* argv[])
{
	int fail = 0;
	int is_tx = 1;
	int rx_thres = SPI_FIFO_DEPTH/2;
	int rx_fill_level = rx_thres;
	int gctl_rx_thres = SPI_RXTLF_16;
	unsigned long ien;
	int time_used = 0;

	info("%s: interrupt handle test\n",argv[0]);

	if (argc < 2) {
		info("wrong cmd format, %s tx | [rx rx_threshold rx_fill_level]\n", __func__);
		return -1;
	}

	if (argc >= 2) {
		if(strcmp(argv[1], "tx") && strcmp(argv[1], "rx")) {
			info("wrong int type, should be tx or rx or rx_thres\n");
			return -1;
		}
		if(strcmp(argv[1], "tx"))
			is_tx = 0;
	}

	spi_int_counter = 0;
	brite_spi_set_int_mask(p_spi_dev, 0);

	brite_spi_init(p_spi_dev,
		p_spi_dev->pclk,
		1,
		1000000,
		0,
		0,
		0,
		0,
		8);

	if(is_tx) {
		ien = SPI_TXEPT_INTF;
	} else {
		if (argc >= 3)
			rx_thres = get_arg_ulong(argv[2]);
		switch(rx_thres) {
		case 1:
			gctl_rx_thres = SPI_RXTLF_1;
			break;
		case SPI_FIFO_DEPTH/8:
			gctl_rx_thres = SPI_RXTLF_4;
			break;
		case SPI_FIFO_DEPTH/4:
			gctl_rx_thres = SPI_RXTLF_8;
			break;
		case SPI_FIFO_DEPTH/2:
			gctl_rx_thres = SPI_RXTLF_16;
			break;
		default:
			info("wrong rx threshold, should be one of 1/%d/%d/%d\n",
					SPI_FIFO_DEPTH/8,
					SPI_FIFO_DEPTH/4,
					SPI_FIFO_DEPTH/2);
			return -1;
		}
		brite_spi_set_txrx_threshold(p_spi_dev, gctl_rx_thres | SPI_TXTLF_16);

		if(argc >= 4)
			rx_fill_level = get_arg_ulong(argv[3]);

		if (rx_fill_level == SPI_FIFO_DEPTH)
			ien = SPI_RXFIFO_FULL_INTF;
		else if (rx_fill_level > SPI_FIFO_DEPTH)
			ien = SPI_RXOERR_INTF;
		else if (rx_fill_level >= rx_thres)
			ien = SPI_RX_INTF;
		else if (rx_fill_level < rx_thres)
			ien = SPI_RXFIFO_FULL_INTF | SPI_RXOERR_INTF | SPI_RX_INTF;

		info("rx_thres %d gctl_rx_thres 0x%x rx_fill_level %d\n", rx_thres, gctl_rx_thres, rx_fill_level);
	}
	predef_int_status = ien;
	info("predef_int_status 0x%x\n", predef_int_status);

	/* brite_spi_enable_txrx(p_spi_dev, is_tx ? SPI_TXEN : SPI_RXEN); */

	/*register int handler and enable it*/
	irq_generic_install_handler(p_spi_dev->int_vector,
		spi_int_handler,
		NULL);
	irq_generic_config_int(p_spi_dev->int_vector, IRQ_TYPE_LEVEL_LOW);
	irq_generic_enable_int(p_spi_dev->int_vector);

	/*enable interrupt*/
	brite_spi_set_int_clear(p_spi_dev, 0xFFFF);
	brite_spi_set_int_enable(p_spi_dev, 1);

	/* brite_spi_dump_status(p_spi_dev); */

	if(is_tx)
		spi_package_xfer(p_spi_dev, rx_fill_level * p_spi_dev->bits_per_word, test_send_buff, NULL, SPI_XFER_ONCE);
	else
		brite_spi_rx_cnt(p_spi_dev, rx_fill_level);

	brite_spi_set_int_mask(p_spi_dev, ien);

	while(time_used++ < 500 && spi_int_counter == 0)
		mdelay(1); /* delay some second for interrupt msg output */

	if (spi_int_counter == 0)
		fail++;

	if (fail)
		info("%s: test fail\n",argv[0]);
	else
		info("%s: test pass\n",argv[0]);

	/* brite_spi_dump_status(p_spi_dev); */
	brite_spi_empty_fifo(p_spi_dev);

	/*disable interrupt*/
	irq_generic_disable_int(p_spi_dev->int_vector);
	irq_generic_remove_handler(p_spi_dev->int_vector,NULL,NULL);
	brite_spi_set_int_enable(p_spi_dev, 0);
	brite_spi_set_int_mask(p_spi_dev, 0);

	return 0;
}

static int spi_robust(int argc, char* argv[])
{
	unsigned char cmd_buf[8];
	int i;
	unsigned long len;
	int ret;
	int fail = 0;
	char cc;
	int counter = 0;
	int counter_fail = 0;

	info("%s:spi robust test\n",argv[0]);

	if (spi_test_init(argc,argv))
		return 0;

	while (1) {
		if (console_tstc()) {
			cc = console_getc();
			if (cc == 'q') {
				break;
			}
		}
		/*generate random data*/
		srand(get_ticks());

		len = cb_rand() % DMAC_MAX_SIZE_PER_BLOCK;
		if (len == 0) 
			len = DMAC_MAX_SIZE_PER_BLOCK;
		
		if ((p_spi_dev->step == 2)) {
			len &= ~1;
			if (len==0)
				len = 2;
		}
		else if ((p_spi_dev->step == 4)) {
			len &= ~3;
			if (len ==0)
				len = 4;
		} else {
			if (len==0)
				len = 1;
		}
		for (i = 0; i < len; i++) {
			test_send_buff[i] = cb_rand() & 0xff;
		}
		memset(test_recv_buff, 0, sizeof(test_recv_buff));

		/*transfer command first*/
		/*write only,index 0*/
		generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,1,0,0,len);

		ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
		if (ret) {
			info("send write command fail,ret=%d\n", ret);
			fail++;
			continue;
		}

		/*transfer data*/
		ret = spi_package_xfer(p_spi_dev,
			len * p_spi_dev->bits_per_word,
			test_send_buff,
			NULL,
			SPI_XFER_ONCE);
		if (ret) {
			info("send write data fail,ret=%d\n", ret);
			fail++;
		}

		/*transfer command first*/
		/*read only,index 0*/
		generate_spi_cmd(p_spi_dev->bits_per_word,cmd_buf,0,0,0,len);

		ret = spi_package_xfer(p_spi_dev, 8 * p_spi_dev->bits_per_word, cmd_buf, NULL, SPI_XFER_ONCE);
		if (ret) {
			info("send read command fail,ret=%d\n", ret);
			fail++;
			continue;
		}

		/*transfer data*/
		ret = spi_package_xfer(p_spi_dev,
			len * p_spi_dev->bits_per_word,
			NULL,
			test_recv_buff,
			SPI_XFER_ONCE);
		if (ret) {
			info("read data fail,ret=%d\n", ret);
			fail++;
		}
		/*check result*/
		fail += spi_check_data(test_send_buff,test_recv_buff,len,p_spi_dev->bits_per_word);

		counter++;
		if (fail) {
			fail = 0;
			counter_fail++;
		}
		info("spi_robust: %d loop, len=%d bytes total failed %d\n",
				counter,len,counter_fail);
		mdelay(100);
	}
	info("spi_robust: total %d loop,total failed %d\n",
				counter,counter_fail);
	return 0;
}

static int spi_flash_test(int argc, char* argv[])
{
	int i;
	unsigned long len;
	int ret;

	info("%s:access spi data flash test\n",argv[0]);

	if (spi_test_init(argc,argv))
		return 0;

	/* set flash access using this spi port*/
	sf_set_controller(p_spi_dev, (spi_xfer_fn)spi_package_xfer);

	ret = sf_probe();
	if (ret) {
		info("unknow or wrong flash id detected,skip other test\n");
		goto end;
	}

	/* first test erase, erase one sector(4k) */
	len = 4096;
	ret = sf_erase(0, len);
	if (ret) {
		info("erase fail\n");
		goto end;
	}

	/* read to verify,should be 'ff' */
	memset(test_recv_buff, 0, sizeof(test_recv_buff));
	if (sf_read(0, len, (char*)test_recv_buff,0)) {
		info("Check read failed\n");
		goto end;
	}
	for (i = 0; i < len; i++) {
		if (test_recv_buff[i] != 0xff) {
			info("Check failed at %d:%x\n", i, test_recv_buff[i]);
			ret = 1;
			goto end;
		}
	}

	/*generate random data*/
	srand(get_ticks());

	for (i = 0; i < len; i++) {
		test_send_buff[i] = cb_rand() & 0xff;
	}
	memset(test_recv_buff, 0, sizeof(test_recv_buff));

	/* write to flash*/
	if (sf_write(0, len, (char*)test_send_buff,0)) {
		info("Write failed\n");
		ret = 1;
		goto end;
	}

	/* read back to verify*/
	if (sf_read(0, len, (char*)test_recv_buff,0)) {
		info("Read failed\n");
		ret = 1;
		goto end;
	}

	for (i = 0; i < len; i++) {
		if (test_recv_buff[i] != test_send_buff[i]) {
			info("Verify failed at %d, good data:\n", i);
			ret = 1;
			goto end;
		}
	}

end:
	if (ret) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	return 0;
}
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry spi_test_menu[] = {
	{ "help", 	cmd_help," : Display list of commands" },
	{ "h",		cmd_help, 	"    : alias for help" },
	{ "?",		cmd_help, 	"    : alias for help" },
	{ "spi_loopback",spi_loopback,	"   : spi master mode loopback test" },
	{ "spi_slave",	spi_slave, 	 "   : spi slave mode using interrupt mode" },
	{ "spi_master",	spi_master, 	 "   : spi master mode test" },
	{ "spi_full_duplex",spi_full_duplex,"  : spi write/read full Duplex test" },
	{ "spi_full_duplex_robust",spi_full_duplex_robust,"  : spi write/read full Duplex robust test" },
	{ "spi_robust",	spi_robust, 	 "   : spi robust test" },
	{ "spi_int",	spi_int,"  : spi interrupt handle test" },
	{ "spi_flash_test",spi_flash_test, "   : spi robust test" },
	{ "spi_slave_debug",spi_slave_debug,  "   : spi slave mode debug" },
	{ "spi_debug",spi_debug, "   : spi master mode debug" },
	{ 0, 0, 0 }
};

/* if return none zero, something wrong,please check and can't continue test*/
int spi_test_prepare(int sel)
{
	memset(p_spi_dev, 0, sizeof(struct brite_spi));

	if (!misc_spi_pin_init(sel))
	{
		return 1;
	}	

	prcm_reset(sel==1?ARM_SPI0_RST : ARM_SPI1_RST);

	if (sel == 1) 
	{
		p_spi_dev->regs = (void*)SPI1_BASE;
		p_spi_dev->dmac_idx = 2;
		p_spi_dev->dma_ch_tx = 1;
		p_spi_dev->dma_ch_rx = 0;
		p_spi_dev->dma_req_tx = DMA_REQ_SPI1_TX;
		p_spi_dev->dma_req_rx = DMA_REQ_SPI1_RX;
		p_spi_dev->int_vector = INT_SPI1;
		p_spi_dev->fifo_depth = SPI_FIFO_DEPTH;
		brite_spi_init(p_spi_dev, SPI_PCLK, 1, 10000000, 0, 1, SPI_MODE_0, 0, 8);
	} 
	else if (sel == 2) 
	{
		p_spi_dev->regs = (void*)SPI2_BASE;
		p_spi_dev->dmac_idx = 1;
		p_spi_dev->dma_ch_tx = 1;
		p_spi_dev->dma_ch_rx = 0;
		p_spi_dev->dma_req_tx = DMA_REQ_SPI2_TX;
		p_spi_dev->dma_req_rx = DMA_REQ_SPI2_RX;
		p_spi_dev->int_vector = INT_SPI2;
		p_spi_dev->fifo_depth = SPI_FIFO_DEPTH;
		brite_spi_init(p_spi_dev, SPI_PCLK, 1, 10000000, 0, 1, SPI_MODE_0, 0, 8);
	} 
	else 
	{
		info("wrong device id\n");
		return 1;
	}
	
	return 0;
}

#define SPI_CCTL_DEF_VAL	0x700
bool spi_reset_test(E_SPI_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_IP_RST ip;
	struct brite_spi *p_dev = p_spi_dev;

	info("%s: port:%d\n", __func__, port);
	spi_test_prepare(port);
	
	info("write cctl reg\n");
	
	// write register
	brite_spi_write(p_dev, BRITE_SPI_CCTL, *p_wr_val);
	
	info("read&compare cctl reg\n");
	*p_rd_val = brite_spi_read(p_dev, BRITE_SPI_CCTL);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("spi reset\n");
	if 		(port == SPI1)	ip = ARM_SPI0_RST;
	else					ip = ARM_SPI1_RST;
	prcm_reset(ip);
	
	// read register
	info("read&compare cctl reg\n");
	*p_rd_val = brite_spi_read(p_dev, BRITE_SPI_CCTL);

	if (*p_rd_val != SPI_CCTL_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool spi_cg_test(E_SPI_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk;
	struct brite_spi *p_dev = p_spi_dev;

	if 		(port == SPI1)	blk = ARM_BLK_CG_SPI0;
	else					blk = ARM_BLK_CG_SPI1;

	info("%s: port:%d\n", __func__, port);

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	spi_test_prepare(port);
	
	// write register
	info("write cctl reg\n");
	brite_spi_write(p_dev, BRITE_SPI_CCTL, *p_wr_val);
	
	info("read&compare cctl reg\n");
	*p_rd_val = brite_spi_read(p_dev, BRITE_SPI_CCTL);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write alg mode reg\n");
	brite_spi_write(p_dev, BRITE_SPI_CCTL, (*p_wr_val+1));

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	// read register
	info("read&compare cctl reg\n");
	*p_rd_val = brite_spi_read(p_dev, BRITE_SPI_CCTL);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool spi_reset_test(E_SPI_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool spi_cg_test(E_SPI_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

#endif

