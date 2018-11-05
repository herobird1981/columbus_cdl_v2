/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#include "test_dma.h"
#include "cdl_gpio.h"
#include "cdl_timer.h"

#if (CONFIG_DMA_TEST == 1)
#include "cdl_ipc.h"

struct dw_dmac dma_dev;
struct dw_dmac* p_dma_dev = &dma_dev;

#define uswap_16(x) \
	((((x) & 0xff00) >> 8) | \
	 (((x) & 0x00ff) << 8))
#define uswap_32(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >>  8) | \
	 (((x) & 0x0000ff00) <<  8) | \
	 (((x) & 0x000000ff) << 24))
	 

cmdline_entry dma_test_menu[];

unsigned long lli_buffer[7 * 128];

extern struct a7_ipc* p_ipc_dev;

struct memory_area {
	char* name;
	unsigned long start;
	unsigned long size;
};

const struct memory_area memory[] = {
	{"sram",SRAM_BASE,SRAM_SIZE},
	{"ddr",0x41000000,0x02000000},
	//{"bbu ram",BBU_RAM_BASE,1024},
	{"share ram",A7_IPC_RAM_BASE,32*1024},
};

/*#define TEST_DMA_USING_SRAM*/

/*src/dst: buffer should word aligned
	len: transfer len
*/
static void dma_prepare_buffer(unsigned long src, unsigned long dst,
	unsigned long len, int src_width)
{
	unsigned long val;
	int i;
	unsigned long actual_len;

	srand(get_ticks());

	actual_len = len * (1 << src_width);

	/*set another 32 bytes for boundard */
	for (i = 0; i < 32; i++) {
		*(unsigned char*)(src + i) = 0xaa;
	}
	src += 32;
	for (i = 0; i < len; i++) {
		val = cb_rand();
		if (val == 0) {
			val = 0x12345678;
		}
		if (src_width == BYTE_WIDTH) {
			*(unsigned char*)(src + i) = val & 0xff;
		} else if (src_width == HWORD_WIDTH) {
			*(unsigned short*)(src + i * 2) = val & 0xffff;
		} else if (src_width == WORD_WIDTH) {
			*(unsigned long*)(src + i * 4) = val;
		} else if (src_width == TWO_WORD_WIDTH) {
			*(unsigned long*)(src + i * 8) = val;
			*(unsigned long*)(src + i * 8 + 4) = val;
		} else if (src_width == FOUR_WORD_WIDTH) {
			*(unsigned long*)(src + i * 16) = val;
			*(unsigned long*)(src + i * 16 + 4) = val;
			*(unsigned long*)(src + i * 16 + 8) = val;
			*(unsigned long*)(src + i * 16 + 12) = val;
		} else if (src_width == EIGHT_WORD_WIDTH) {
			*(unsigned long*)(src + i * 32) = val;
			*(unsigned long*)(src + i * 32 + 4) = val;
			*(unsigned long*)(src + i * 32 + 8) = val;
			*(unsigned long*)(src + i * 32 + 12) = val;
			*(unsigned long*)(src + i * 32 + 16) = val;
			*(unsigned long*)(src + i * 32 + 20) = val;
			*(unsigned long*)(src + i * 32 + 24) = val;
			*(unsigned long*)(src + i * 32 + 28) = val;
		}
	}
	/*set another 32 bytes for boundard*/
	for (i = 0; i < 32; i++) {
		*(unsigned char*)(src + actual_len + i) = 0x55;
	}

	memset((char*)dst, 0, actual_len + 32 + 32);
}

static void dma_prepare_buffer_all(int ch,int trans_len,unsigned long src_addr,
	int src_bus_width,int src_burst_size,int src_addr_update,
	unsigned long dst_addr,int dst_bus_width,int dst_burst_size,
	int dst_addr_update,int display_flag)
{
	unsigned long val;
	int i;
	unsigned long src_actual_len;
	unsigned long dst_actual_len;

	srand(get_ticks());

	if (src_addr_update != FIXED)
		src_actual_len = trans_len * (1 << src_bus_width);
	else
		src_actual_len = 4;

	if (dst_addr_update != FIXED)
		dst_actual_len = trans_len * (1 << src_bus_width);
	else
		dst_actual_len = 4;

	/*set another 32 bytes for boundard */
	for (i = 0; i < 32; i++) {
		*(unsigned char*)(src_addr + i) = 0xaa;
	}
	src_addr += 32;

	if (src_addr_update == FIXED) {
		val = cb_rand();
		if (val ==0)
			val = 0x12345678;
		*(unsigned long*)src_addr = val;
	}else {
		for (i = 0; i < trans_len; i++) {
			val = cb_rand();
			if (val == 0) {
				val = 0x12345678;
			}
			if (src_bus_width == BYTE_WIDTH) {
				*(unsigned char*)(src_addr + i) = val & 0xff;
			} else if (src_bus_width == HWORD_WIDTH) {
				*(unsigned short*)(src_addr + i * 2) = val & 0xffff;
			} else if (src_bus_width == WORD_WIDTH) {
				*(unsigned long*)(src_addr + i * 4) = val;
			}
		}
	}
	/*set another 32 bytes for boundard*/
	for (i = 0; i < 32; i++) {
		*(unsigned char*)(src_addr + src_actual_len + i) = 0x55;
	}

	memset((char*)dst_addr, 0, dst_actual_len + 32 + 32);
}

/*
default src/dst: using WORD width, burst 4, INCREASE
*/
static int dma_check_buffer_single(u32 off, int width, u32 src, u32 dst)
{
	if (width == BYTE_WIDTH) {
		if (*(u8 *)(src) == *(u8 *)(dst))
			return 0;
		else {
			debug("memory diff @0x%x src=0x%x(0x%x) dst=0x%x(0x%x)\n",
				off, 
				src, *(u8 *)(src),
				dst, *(u8 *)(dst));
			return 1;
		}
	} else if (width == HWORD_WIDTH) {
		if (*(u16 *)(src) == *(u16 *)(dst))
			return 0;
		else {
	  		debug("memory diff @0x%x src=0x%x(0x%x) dst=0x%x(0x%x)\n",
				off,
				src, *(u16 *)(src),
				dst, *(u16 *)(dst));
			return 1;
		}
	} else {
		if (*(u32 *)(src) == *(u32 *)(dst))
			return 0;
		else {
	  		debug("memory diff @0x%x src=0x%x(0x%x) dst=0x%x(0x%x)\n",
				off,
				src, *(u32 *)(src),
				dst, *(u32 *)(dst));
			return 1;
		}
	}
}

static int illegibility_match(unsigned long src_data, unsigned long dst_data)
{
	unsigned char b0 = dst_data&0xff, b1 = (dst_data&0xff00)>>8, b2 = (dst_data&0xff0000)>>16, b3 = (dst_data&0xff000000)>>24;
	unsigned char hw0 = dst_data&0xffff, hw1 = (dst_data>>16)&0xffff;
	
	if(src_data == dst_data)
		return 0;
	if(src_data == uswap_32(dst_data))
		return 0;
	if((b0 == b1 && b1 == b2 && b2 == b3) && ((b0 == (src_data & 0xff)) || (b0 == ((src_data & 0xff00) >> 8))
		|| (b0 == ((src_data & 0xff0000) >> 16)) || (b0 == ((src_data & 0xff000000) >> 24))))
		return 0;
	if((b0 == (src_data & 0xff)) || (b0 == ((src_data & 0xff00) >> 8))
		|| (b0 == ((src_data & 0xff0000) >> 16)) || (b0 == ((src_data & 0xff000000) >> 24)))
		return 0;
	if((hw0 == hw1) && ( (hw0 == (src_data&0xffff)) || (hw0 == ((src_data>>16)&0xffff)) 
		||(hw0 == uswap_16(src_data&0xffff)) || (hw0 == uswap_16((src_data>>16)&0xffff))	))
		return 0;
	if(src_data == ((hw0<<16) | hw1))
		return 0;
	if(src_data == ((b2<<24) | (b3<<16) | (b1) | (b0<<8)))
		return 0;

	return -1;
	
}

static int dma_check_buffer_all(int ch,int trans_len,unsigned long src_addr,
	int src_bus_width,int src_burst_size,int src_addr_update,
	unsigned long dst_addr,int dst_bus_width,int dst_burst_size,
	int dst_addr_update,int display_flag)
{
	int i;
	int ret = 0;
	unsigned long actual_len = trans_len * (1 << src_bus_width);
    unsigned long dst_actual_len = dst_addr_update == FIXED ? 4 : actual_len;

	unsigned long src_data = 0;
	unsigned long dst_data = 0;

	if(src_addr_update == FIXED || src_addr_update == DECREASE)
		src_data = *((unsigned long*)(src_addr + 32));
    else
		src_data = *((unsigned long*)(src_addr + 32 + actual_len - 4));

	if(dst_addr_update == FIXED || dst_addr_update == DECREASE)
		dst_data = *((unsigned long*)(dst_addr + 32));
    else
		dst_data = *((unsigned long*)(dst_addr + 32 + actual_len - 4));

	//illegibility matching is used
	ret = illegibility_match(src_data, dst_data);
	if( ret == -1)
	{
		debug("data compare error\n");
		ret = 1;
	}

	/*check dst buffer not overflow*/
	for (i = 0; i < 32; i++) {
		if (*(unsigned char*)(dst_addr + i) != 0) {
			debug("dst unexpected @0x%x val=%x should be zero\n",
				i,
				*(unsigned char*)(dst_addr + i));
			ret++;
			break;
		}
	}
	
	for (i = 0; i < 32; i++) {
		if (*(unsigned char*)(dst_addr + 32 + dst_actual_len + i) != 0) {
			debug("dst unexpected @0x%x val=%x should be zero\n",
				dst_actual_len + i + 32,
				*(unsigned char*)(dst_addr + 32 + dst_actual_len + i));
			ret++;
			break;
		}
	}
	
	return ret;
}

static int dma_check_buffer_value(u32 off, int width, u32 src, u32 val)
{
	if (width == BYTE_WIDTH) {
		if (*(u8 *)(src) == (u8)val)
			return 0;
		else {
			debug("memory diff @0x%x src=%x dst=%x\n",
				off,
				*(u8 *)(src),
				val);
			return 1;
		}
	} else if (width == HWORD_WIDTH) {
		if (*(u16 *)(src) == (u16)val)
			return 0;
		else {
	  		debug("memory diff @0x%x src=%x dst=%x\n",
	  			off,
	  			*(u16 *)(src),
	  			val);
			return 1;
		}
	} else {
		if (*(u32 *)(src) == (u32)val)
			return 0;
		else {
	  		debug("memory diff @0x%x src=%x dst=%x\n",
	  			off,
	  			*(u32 *)(src),
	  			val);
			return 1;
		}
	}
}

static int dma_check_buffer(unsigned long src, unsigned long dst,
	unsigned long len, int src_dst, int bus_width, int addr_mode)
{
	int i;
	int ret = 0;
	unsigned long actual_len;

	if (src_dst)
		actual_len = len * 4;
	else
		actual_len = len * (1 << bus_width);

	/*check buffer have same data*/
	if (addr_mode == INCREASE) {
		for (i = 0; i < actual_len; i++) {
			if (dma_check_buffer_single(32 + i,
				BYTE_WIDTH,
				src + 32 + i,
				dst + 32 + i)) {
				ret++;
				break;
			}
		}
	} else if (addr_mode == FIXED) {
		if (src_dst) {
	  		/* source using default setting */
			if (dma_check_buffer_single(32,
	  			bus_width,
	  			src + 32 + actual_len - (1 << bus_width),
	  			dst + 32)) {
				ret++;
			}
			for (i = 1; i < actual_len / 4; i++) {
				if (dma_check_buffer_value(32 + 4 * i,
	  				WORD_WIDTH,
	  				dst + 32 + i * 4,
	  				0)) {
					ret++;
					break;
				}
			}
		} else {
			for (i = 0; i < actual_len / (1 << bus_width); i++) {
				if (dma_check_buffer_single(32 +
	  	  			i * (1 << bus_width),
	  	  			bus_width,
	  	  			src + 32,
	  	  			dst + 32 + i * (1 << bus_width))) {
					ret++;
					break;
				}
			}
		}
	} else {
		for (i = 0; i < actual_len / 4; i++) {
			if (bus_width == BYTE_WIDTH) {
				if (dma_check_buffer_single(dst + actual_len +
	  				32 - i * 4 - 4 + 3,
	  				bus_width,
	  				src + 32 + i * 4,
	  				dst + actual_len + 32 - i * 4 - 4 + 3)) {
					ret++;
					break;
				}
				if (dma_check_buffer_single(dst + actual_len +
	  				32 - i * 4 - 4 + 2,
	  				bus_width,
	  				src + 32 + i * 4 + 1,
	  				dst + actual_len + 32 - i * 4 - 4 + 2)) {
					ret++;
					break;
				}
				if (dma_check_buffer_single(dst + actual_len +
	  				32 - i * 4 - 4 + 1,
	  				bus_width,
	  				src + 32 + i * 4 + 2,
	  				dst + actual_len + 32 - i * 4 - 4 + 1)) {
					ret++;
					break;
				}
				if (dma_check_buffer_single(dst + actual_len +
	  				32 - i * 4 - 4,
	  				bus_width,
	  				src + 32 + i * 4 + 3,
	  				dst + actual_len + 32 - i * 4 - 4)) {
					ret++;
					break;
				}
			} else if (bus_width == HWORD_WIDTH) {
				if (dma_check_buffer_single(dst + actual_len +
			  		32 - i * 4 - 4 + 2,
			  		bus_width,
			  		src + 32 + i * 4,
			  		dst + actual_len + 32 - i * 4 - 4 + 2)) {
					ret++;
					break;
				}
				if (dma_check_buffer_single(dst + actual_len +
			  		32 - i * 4 - 4,
			  		bus_width,
			  		src + 32 + i * 4 + 2,
			  		dst + actual_len + 32 - i * 4 - 4)) {
					ret++;
					break;
				}
			} else {
				if (dma_check_buffer_single(dst + actual_len +
			  		32 - i * 4 - 4,
			  		bus_width,
			  		src + 32 + i * 4,
			  		dst + actual_len + 32 - i * 4 - 4)) {
					ret++;
					break;
				}
			}
		}
	}

	/*check dst buffer not overflow*/
	for (i = 0; i < 32; i++) {
		if (*(unsigned char*)(dst + i) != 0) {
			debug("dst unexpected @0x%x val=%x should be zero\n",
				i,
				*(unsigned char*)(dst + i));
			ret++;
			break;
		}
	}
	for (i = 0; i < 32; i++) {
		if (*(unsigned char*)(dst + 32 + actual_len + i) != 0) {
			debug("dst unexpected @0x%x val=%x should be zero\n",
				actual_len + i + 32,
				*(unsigned char*)(dst + 32 + actual_len + i));
			ret++;
			break;
		}
	}
	return ret;
}

static int dma_check_param_valid(unsigned long addr,unsigned long trans_len,
	int bus_width,int addr_update,char** str)
{
	struct memory_area * mm_sram = (struct memory_area*)&memory[0];
	struct memory_area *mm_ddr = (struct memory_area*)&memory[1];
	struct memory_area *mm_share = (struct memory_area*)&memory[2];

	if (!(((addr >= mm_ddr->start) && (addr < mm_ddr->start+mm_ddr->size)) ||
		((addr >= mm_sram->start) && (addr < mm_sram->start+mm_sram->size)) ||
		((addr >= mm_share->start) && (addr < mm_share->start+mm_share->size)))) {
		info("addr invalid\n");
		return 1;
	}

	if ((addr >= mm_ddr->start) && (addr < mm_ddr->start+mm_ddr->size)) {
		*str = mm_ddr->name;
		if (addr + (trans_len<<bus_width) + 64 > mm_ddr->start+mm_ddr->size) {
			info("trans_len invalid\n");
			return 2;
		}
	}
	if ((addr >= mm_sram->start) && (addr < mm_sram->start+mm_sram->size)) {
		*str = mm_sram->name;
		if (addr + (trans_len<<bus_width) + 64 > mm_sram->start+mm_sram->size) {
			info("trans_len invalid\n");
			return 3;
		}
	}
	if ((addr >= mm_share->start) && (addr < mm_share->start+mm_share->size)) {
		*str = mm_share->name;
		if (addr + (trans_len<<bus_width) + 64 > mm_share->start+mm_share->size) {
			info("trans_len invalid\n");
			return 4;
		}
	}
	return 0;
}

static int dma_test_prepare_share_ram(void)
{
	int i;
	int ret = 0;

	memset(p_ipc_dev,0,sizeof(struct a7_ipc));
	p_ipc_dev->reg_base = (void*)A7_IPC_REG_BASE;
	p_ipc_dev->ram_base = (void*)A7_IPC_RAM_BASE;
	for(i=0; i<8; i++){
		p_ipc_dev->int_rf[i] = INT_IPC_RF0 + i;
		p_ipc_dev->int_plc[i] = INT_IPC_PLC0 + i;
	}
	/*obtain all share ram*/
	for (i=0;i<32;i++) {
		ret += a7_get_sram_page(p_ipc_dev, i);
	}
	return ret;
}

static int dma_test_unprepare_share_ram(void)
{
	int i;
	int ret = 0;

	for (i=0;i<32;i++) {
		ret += a7_free_page(p_ipc_dev, i);
	}
	return ret;
}

static int dma_m2m_robust(int argc, char* argv[])
{
	unsigned long src_addr,dst_addr;
	int ch=0;
	unsigned long trans_len;
	int ret;
	int fail = 0;
	int total_fail = 0;
	char cc;
	int src,dst;
	struct memory_area *src_area,*dst_area;
	int total_count = 0;
	unsigned long off = 0;
	int src_width,dst_width;

	info("%s: memory to memory test,press 'q' to quit\n",argv[0]);

	while (1) {
		total_count++;
		for (src=0;src<sizeof(memory)/sizeof(struct memory_area);src++) {
			for (dst=0;dst<sizeof(memory)/sizeof(struct memory_area);dst++) {
				if (console_tstc()) {
					cc = console_getc();
					if (cc == 'q') {
						goto exit;
					}
				}
				fail = 0;
				src_area = (struct memory_area*)&memory[src];
				dst_area = (struct memory_area*)&memory[dst];

				if (!strcmp(src_area->name,"share ram") ||
					!strcmp(dst_area->name,"share ram")) {
					ret = dma_test_prepare_share_ram();
					if (ret) {
						info("share ram init fail,skip\n");
						dma_test_unprepare_share_ram();
						continue;
					}
				}
				srand(get_ticks());
				off = cb_rand() & 0xfff;
				off = min(off,src_area->size/4);
				off = min(off,dst_area->size/4);
				off &= ~3;
				srand(get_ticks());
				trans_len = cb_rand() & 0xfff;
				trans_len = min(trans_len,src_area->size/2);
				trans_len = min(trans_len,dst_area->size/2);
				trans_len = trans_len/4;
				if (trans_len > 16)
					trans_len -= 16;
				else
					trans_len = 1;
				if (trans_len > 256*1024)
					trans_len = 256*1024;
				if ((off + trans_len*4 + 64 > src_area->size/2) ||
					(off + trans_len*4 + 64 > dst_area->size/2)) {
					off = 0;
				}

				src_addr = src_area->start;
				dst_addr = dst_area->start + dst_area->size/2;
				info("dma '%s' to '%s' transfer test: ch=%d src=0x%x dst=0x%x len=%d word\n",
					src_area->name,
					dst_area->name,
					ch,
					src_addr+off,
					dst_addr+off,
					trans_len);

				src_width = WORD_WIDTH;
				dst_width = WORD_WIDTH;
				if (!strcmp(src_area->name,"bbu ram"))
					src_width = BYTE_WIDTH;
				if (!strcmp(dst_area->name,"bbu ram"))
					dst_width = BYTE_WIDTH;

				/*config src*/
				dw_dmac_channel_config(p_dma_dev,
					ch,
					0,
					src_width,
					BURST_4,
					INCREASE,
					
					MEM_TO_MEM,
					0);
				/*config dst*/
				dw_dmac_channel_config(p_dma_dev,
					ch,
					1,
					dst_width,
					BURST_4,
					INCREASE,
					
					MEM_TO_MEM,
					0);

				/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
				dma_prepare_buffer(src_addr+off, dst_addr+off, trans_len, WORD_WIDTH);

				//start ch transfer
				ret = dw_dmac_channel_single_transfer_poll(p_dma_dev,
					ch,
					src_addr + off+32,
					dst_addr + off+32,
					trans_len);
				if (ret) {
					debug("dmac ch %d transfer timeout\n", ch);
					fail++;
				}
				if (!fail) {
					ret = dma_check_buffer(src_addr+off,
						dst_addr+off,
						trans_len,
						0,
						WORD_WIDTH,
						INCREASE);
					if (ret) {
						debug("dmac ch %d transfer result check error\n", ch);
						fail++;
					}
				}
				if (!fail)
					info("...pass!\n");
				else
					info("...fail!\n");
				total_fail += fail;

				if (!strcmp(src_area->name,"share ram") ||
					!strcmp(dst_area->name,"share ram")) {
					ret = dma_test_unprepare_share_ram();
				}
			}
		}
		info("total_count:%d total fail=%d\n\n",total_count,total_fail);
		ch++;
		if (ch>7)
			ch = 0;
	}
exit:
	if (total_fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

static int dma_m2m(int argc, char* argv[])
{
	unsigned long src_addr,dst_addr;
	int ch;
	unsigned long trans_len;
	int ret;
	int fail = 0;
	struct memory_area * mm_sram = (struct memory_area*)&memory[0];
	struct memory_area *mm_ddr = (struct memory_area*)&memory[1];
	struct memory_area *mm_share = (struct memory_area*)&memory[2];
	int display_flag = 0;
	char *src_str,*dst_str;

	info("%s: dma memory to memory test: only ddr/sram\n",argv[0]);
	info("format: %s ch src_addr dst_addr length [display]\n",argv[0]);
	info("ch:0-7\n");
	info("src_addr: src addr\n");
	info(" ddr: 0x%x - 0x%x\n",mm_ddr->start,mm_ddr->start+mm_ddr->size-1);
	info(" sram: 0x%x - 0x%x\n",mm_sram->start,mm_sram->start+mm_sram->size-1);
	info(" share ram: 0x%x - 0x%x\n",mm_share->start,mm_share->start+mm_share->size-1);
	info("length: unit is dword(4 bytes)\n");
	
	if (argc < 5) {
		info("wrong cmd_format: %s ch src_addr dst_addr length\n",argv[0]);
		return 0;
	}
	ch = get_arg_ulong(argv[1]);
	src_addr = get_arg_ulong(argv[2]);
	dst_addr = get_arg_ulong(argv[3]);
	trans_len = get_arg_ulong(argv[4]);
	if (argc>5)
		display_flag = get_arg_ulong(argv[5]);

	if (ch > 7) {
		info("wrong ch number %d,should be 0-7\n", ch);
		return 0;
	}
	/*check valid*/
	if (dma_check_param_valid(src_addr,trans_len,WORD_WIDTH,INCREASE,&src_str)) {
		info("src_addr or trans_len invalid\n");
		return 0;
	}
	if (dma_check_param_valid(dst_addr,trans_len,WORD_WIDTH,INCREASE,&dst_str)) {
			info("dst_addr or trans_len invalid\n");
			return 0;
	}
	if (!strcmp(src_str,"share ram") ||
		!strcmp(dst_str,"share ram")) {
		ret = dma_test_prepare_share_ram();
		if (ret) {
			info("share ram init fail,skip\n");
			dma_test_unprepare_share_ram();
			return 0;
		}
	}
	info("%s to %s transfer: ch=%d src=0x%x dst=0x%x len=%d word\n",
		src_str,
		dst_str,
		ch,
		src_addr,
		dst_addr,
		trans_len);

	/*config src*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		0,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		
		MEM_TO_MEM,
		0);
	/*config dst*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		1,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		
		MEM_TO_MEM,
		0);

	/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
	dma_prepare_buffer(src_addr, dst_addr, trans_len, WORD_WIDTH);

	if (display_flag) {
		info("initial buffer before dma operation:\n");
		info("note: first 32 and last 32 bytes for check boundary correct\n");
		print_buffer("src",(char*)src_addr, ((trans_len<<WORD_WIDTH) + 64));
		print_buffer("dst",(char*)dst_addr, ((trans_len<<WORD_WIDTH) + 64));
	}
	//start ch transfer
#if 0
	int i;
	unsigned long long start_time = 0, spend_time=0;
	float perf = 0.0;
	start_time = get_timer(0);
	for(i=0; i<10240; i++)
	{
	      ret = dw_dmac_channel_single_transfer_poll(p_dma_dev,
	                    ch,
	                    src_addr + 32,
	                    dst_addr + 32,
	                    trans_len);
	}
	spend_time = get_timer(start_time);
	info("Transfer size is: %d MBytes\n", trans_len*4*10240/1024/1024);
	perf = (float)(trans_len*4 *10 /1024 * 1000 /spend_time) ;
	info("Transfer speed is: %d MBytes/sec\n", (int)perf);
#else
	ret = dw_dmac_channel_single_transfer_poll(p_dma_dev,
		ch,
		src_addr + 32,
		dst_addr + 32,
		trans_len);
#endif
	if (ret) {
		debug("dmac ch %d transfer timeout\n", ch);
		fail++;
	}
	if (!fail) {
		ret = dma_check_buffer(src_addr,
			dst_addr,
			trans_len,
			0,
			WORD_WIDTH,
			INCREASE);
		if (ret) {
			debug("dmac ch %d transfer result check error\n", ch);
			fail++;
		}
	}
	if (display_flag) {
		info("initial buffer after dma operation:\n");
		info("note: first 32 and last 32 bytes for check boundary correct\n");
		print_buffer("src",(char*)src_addr, ((trans_len<<WORD_WIDTH) + 64));
		print_buffer("dst",(char*)dst_addr, ((trans_len<<WORD_WIDTH) + 64));
	}

	if (!strcmp(src_str,"share ram") ||
		!strcmp(dst_str,"share ram")) {
		ret = dma_test_unprepare_share_ram();
	}
	
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/* dma_cfg ch trans_len src_dst bus_width burst_size addr_update
ch: 0-7
trans_len: 0-4095 when src_dst==0; total transfer bytes calcaulated by settings
	when src_dst==1; total transfer bytes: trans_len*4
src_dst: 0	argv[] config dst ; 1 argv[] config src
bus_width: 0 - 5; see TRANSFRE_WIDTH defination
burst_size: 0-7; see BURST_SIZE defination
addr_update: 0-2; see ADDRESS_UPDATE defination
*/
static int dma_cfg(int argc, char* argv[])
{
	unsigned long src_addr,dst_addr;
	int ch;
	unsigned long trans_len;
	int src_dst;
	int bus_width;
	int burst_size;
	int addr_update;
	int ret;
	int fail = 0;
	unsigned long actual_trans_len;
	unsigned long act_src,act_dst;
	int display_flag = 0;

	info("%s: dma ch config test\n",argv[0]);
	if (argc < 7) {
		info("wrong cmd_format: %s ch trans_len src_dst bus_width burst_size addr_update [display]\n",argv[0]);
		return 0;
	}

#ifndef TEST_DMA_USING_SRAM
	src_addr = 0x42000000;
	dst_addr = 0x42100000;
#else
	src_addr = SRAM_BASE;
	dst_addr = SRAM_BASE + SRAM_SIZE / 2;
#endif

	ch = get_arg_ulong(argv[1]);
	trans_len = get_arg_ulong(argv[2]);
	src_dst = get_arg_ulong(argv[3]);
	bus_width = get_arg_ulong(argv[4]);
	burst_size = get_arg_ulong(argv[5]);
	addr_update = get_arg_ulong(argv[6]);
	if (argc>7)
		display_flag = get_arg_ulong(argv[7]);

	if (ch > 7) {
		info("wrong ch number %d,should be 0-7\n", ch);
		return 0;
	}
	/* check bus width, only support 32bit */
	if (bus_width > 2) {
		info("bus_width %d invalid,only support 32bit bus\n",
			bus_width);
		return 0;
	}

	if(burst_size > 0x07){
		info("burst size:src = %d (0~7), should be force to 0x7\n",burst_size);
		burst_size = 7;
	}

	/*configure dst,source use default setting*/
	if (src_dst) {
		dw_dmac_channel_config(p_dma_dev,
			ch,
			0,
			WORD_WIDTH,
			BURST_4,
			INCREASE,
			
			MEM_TO_MEM,
			0);
		dw_dmac_channel_config(p_dma_dev,
			ch,
			1,
			bus_width,
			burst_size,
			addr_update,
			
			MEM_TO_MEM,
			0);
		actual_trans_len = trans_len * 4;
	}
	/*config src,dst use default setting*/
	else {
		dw_dmac_channel_config(p_dma_dev,
			ch,
			0,
			bus_width,
			burst_size,
			addr_update,
			
			MEM_TO_MEM,
			0);
		dw_dmac_channel_config(p_dma_dev,
			ch,
			1,
			WORD_WIDTH,
			BURST_4,
			INCREASE,
			
			MEM_TO_MEM,
			0);
		actual_trans_len = trans_len * (1 << bus_width);
	}

#ifdef TEST_DMA_USING_SRAM
	/*check trans len not exceed sram size*/
	if (actual_trans_len > SRAM_SIZE / 2 - 64) {
		actual_trans_len = SRAM_SIZE / 2 - 64;
		if (src_dst)
			trans_len = actual_trans_len / 4;
		else
			trans_len = actual_trans_len / (1 << bus_width);
		info("transfer len exceed sram range,force set to %d\n",
			trans_len);
	}
#endif
	/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
	if (!src_dst) {
		dma_prepare_buffer(src_addr, dst_addr, trans_len, bus_width);
		if (addr_update == INCREASE)
			act_src = src_addr + 32;
		else if (addr_update == DECREASE) {
			if (bus_width == BYTE_WIDTH)
				act_src = src_addr + 32 + actual_trans_len - 1;
			else if (bus_width == HWORD_WIDTH)
				act_src = src_addr + 32 + actual_trans_len - 2;
			else
				act_src = src_addr + 32 + actual_trans_len - 4;
		} else
			act_src = src_addr + 32;
		act_dst = dst_addr + 32;
	} else {
		dma_prepare_buffer(src_addr, dst_addr, trans_len, WORD_WIDTH);
		act_src = src_addr + 32;
		if (addr_update == INCREASE)
			act_dst = dst_addr + 32;
		else if (addr_update == DECREASE) {
			if (bus_width == BYTE_WIDTH)
				act_dst = dst_addr + 32 + actual_trans_len - 1;
			else if (bus_width == HWORD_WIDTH)
				act_dst = dst_addr + 32 + actual_trans_len - 2;
			else
				act_dst = dst_addr + 32 + actual_trans_len - 4;
		} else
			act_dst = dst_addr + 32;
	}

	if (display_flag) {
		info("initial buffer before dma operation:\n");
		info("note: first 32 and last 32 bytes for check boundary correct\n");
		print_buffer("src",(char*)src_addr, (actual_trans_len + 64));
		print_buffer("dst",(char*)dst_addr, (actual_trans_len + 64));
	}
	/*start ch transfer*/
	ret = dw_dmac_channel_single_transfer_poll(p_dma_dev,
		ch,
		act_src,
		act_dst,
		trans_len);
	if (ret) {
		debug("dmac ch %d transfer timeout\n", ch);
		fail++;
	}
	if (!fail) {
		if (src_dst) {
			ret = dma_check_buffer(src_addr,
				dst_addr,
				trans_len,
				src_dst,
				bus_width,
				addr_update);
		} else {
			ret = dma_check_buffer(src_addr,
		  		dst_addr,
		  		trans_len,
		  		src_dst,
		  		bus_width,
		  		addr_update);
		}
		if (ret) {
			debug("dmac ch %d transfer result check error\n", ch);
			fail++;
		}
	}
	if (display_flag) {
		info("buffer after dma operation:\n");
		info("note: first 32 and last 32 bytes for check boundary correct\n");
		print_buffer("src",(char*)src_addr, (actual_trans_len + 64));
		print_buffer("dst",(char*)dst_addr, (actual_trans_len + 64));
	}
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

static int dma_cfg_all(int ch,int trans_len,unsigned long src_addr,
	int src_bus_width,int src_burst_size,int src_addr_update,
	unsigned long dst_addr,int dst_bus_width,int dst_burst_size,
	int dst_addr_update,int display_flag)
{
	int ret;
	int fail = 0;
	unsigned long actual_trans_len;
	unsigned long act_src,act_dst;

	/*configure dst,source use default setting*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		0,
		src_bus_width,
		src_burst_size,
		src_addr_update,
		
		MEM_TO_MEM,
		0);
	dw_dmac_channel_config(p_dma_dev,
		ch,
		1,
		dst_bus_width,
		dst_burst_size,
		dst_addr_update,
		
		MEM_TO_MEM,
		0);
	actual_trans_len = trans_len * (1 << src_bus_width);

	/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
	dma_prepare_buffer_all(ch,trans_len,src_addr,src_bus_width,src_burst_size,
		src_addr_update,dst_addr,dst_bus_width,dst_burst_size,dst_addr_update,
		display_flag);

	if (display_flag) {
		info("initial buffer before dma operation:\n");
		info("note: first 32 and last 32 bytes for check boundary correct\n");
		print_buffer("src",(char*)src_addr, ((trans_len<<src_bus_width) + 64));
		print_buffer("dst",(char*)dst_addr, ((trans_len<<src_bus_width) + 64));
	}

	if (src_addr_update == INCREASE)
		act_src = src_addr + 32;
	else if (src_addr_update == DECREASE) {
		if (src_bus_width == BYTE_WIDTH)
			act_src = src_addr + 32 + actual_trans_len - 1;
		else if (src_bus_width == HWORD_WIDTH)
			act_src = src_addr + 32 + actual_trans_len - 2;
		else
			act_src = src_addr + 32 + actual_trans_len - 4;
	} else
		act_src = src_addr + 32;

	if (dst_addr_update == INCREASE)
		act_dst = dst_addr + 32;
	else if (dst_addr_update == DECREASE) {
		if (dst_bus_width == BYTE_WIDTH)
			act_dst = dst_addr + 32 + actual_trans_len - 1;
		else if (dst_bus_width == HWORD_WIDTH)
			act_dst = dst_addr + 32 + actual_trans_len - 2;
		else
			act_dst = dst_addr + 32 + actual_trans_len - 4;
	} else
		act_dst = dst_addr + 32;
	
	/*start ch transfer*/
	ret = dw_dmac_channel_single_transfer_poll(p_dma_dev,
		ch,
		act_src,
		act_dst,
		trans_len);
	if (ret) {
		debug("dmac ch %d transfer timeout\n", ch);
		fail++;
	}

	if (display_flag) {
		info("buffer after dma operation:\n");
		info("note: first 32 and last 32 bytes for check boundary correct\n");
		print_buffer("src",(char*)src_addr, ((trans_len<<src_bus_width) + 64));
		print_buffer("dst",(char*)dst_addr, ((trans_len<<src_bus_width) + 64));
	}

	if (!fail) {
		ret = dma_check_buffer_all(ch,trans_len,src_addr,src_bus_width,
			src_burst_size,src_addr_update,dst_addr,dst_bus_width,
			dst_burst_size,dst_addr_update,display_flag);
		if (ret) {
			debug("dmac ch %d transfer result check error\n", ch);
			fail++;
		}
	}

	return fail;
}

/* dma_cfg2 ch trans_len src_addr src_bus_width src_burst_size src_addr_update dst_addr dst_bus_width dst_burst_size dst_addr_update [display]
ch: 0-7
trans_len: 0-4095
src/dst_address: SRAM(0x10000000 ~ 0x10020000); DDR(0x42000000 ~ 0x42200000); shared ram(0xc2600000 ~ 0xc2608000)
bus_width: 0 - 5; see TRANSFRE_WIDTH defination
burst_size: 0-7; see BURST_SIZE defination
addr_update: 0-2; see ADDRESS_UPDATE defination
*/
static int dma_cfg2(int argc, char* argv[])
{
	unsigned long src_addr,dst_addr;
	int ch;
	unsigned long trans_len;
	int src_bus_width,dst_bus_width;
	int src_burst_size,dst_burst_size;
	int src_addr_update,dst_addr_update;
	int ret;
	int fail = 0;
	int display_flag = 0;
	char *src_str,*dst_str;
	struct memory_area * mm_sram = (struct memory_area*)&memory[0];
	struct memory_area *mm_ddr = (struct memory_area*)&memory[1];
	struct memory_area *mm_share = (struct memory_area*)&memory[2];

	info("%s: dma ch config test\n",argv[0]);
	info("%s: dma memory to memory test: ddr/sram/shared ram\n",argv[0]);
	info("format: %s ch trans_len src_addr src_bus_width src_burst_size src_addr_update dst_addr dst_bus_width dst_burst_size dst_addr_update [display]\n",argv[0]);
	info("ch:0-7\n");
	info("trans_len: unit is transfer\n");
	info("src_addr: src addr\n");
	info(" ddr: 0x%x - 0x%x\n",mm_ddr->start,mm_ddr->start+mm_ddr->size-1);
	info(" sram: 0x%x - 0x%x\n",mm_sram->start,mm_sram->start+mm_sram->size-1);
	info(" share ram: 0x%x - 0x%x\n",mm_share->start,mm_share->start+mm_share->size-1);
	info("src_bus_width: 0/1/2 byte/2byte/4byte per transfer\n");
	info("src_burst_size:0-7 : 1/4/8/16/32/64/128/256 transfer per burst\n");
	info("src_addr_update: 0/1/2 : increase/decrease/fixed\n");
	info(" note: for decrease: actual start addr is src/dst addr + trans_len\n");
	info("display: 0 don't display buffer; else display buffer\n");

	if (argc < 11) {
		info("wrong cmd_format: %s ch trans_len src_addr src_bus_width src_burst_size src_addr_update dst_addr dst_bus_width dst_burst_size dst_addr_update [display]\n",argv[0]);
		return 0;
	}

	ch = get_arg_ulong(argv[1]);
	trans_len = get_arg_ulong(argv[2]);
	src_addr = get_arg_ulong(argv[3]);
	src_bus_width = get_arg_ulong(argv[4]);
	src_burst_size = get_arg_ulong(argv[5]);
	src_addr_update = get_arg_ulong(argv[6]);
	dst_addr = get_arg_ulong(argv[7]);
	dst_bus_width = get_arg_ulong(argv[8]);
	dst_burst_size = get_arg_ulong(argv[9]);
	dst_addr_update = get_arg_ulong(argv[10]);
	if (argc>11)
		display_flag = get_arg_ulong(argv[11]);

	/*check valid*/
	if (ch > 7) {
		info("wrong ch number %d,should be 0-7\n", ch);
		return 0;
	}
	/* check bus width, only support 32bit */
	if ((src_bus_width > 2) || (dst_bus_width >2)) {
		info("bus_width %d %d invalid,only support 32bit bus\n",
			src_bus_width,dst_bus_width);
		return 0;
	}

	if(src_burst_size > 0x07){
		info("src burst size:src = %d(0~7), should be force to 0x7\n",src_burst_size);
		src_burst_size = 7;
	}
	if(dst_burst_size > 0x07){
		info("dst burst size:src = %d(0~7), should be force to 0x7\n",dst_burst_size);
		dst_burst_size = 7;
	}	

	if (src_addr_update >2)
		src_addr_update = 0;
	if (dst_addr_update >2)
		dst_addr_update = 0;

	if (dma_check_param_valid(src_addr,trans_len,src_bus_width,src_addr_update,&src_str)) {
		info("src_addr or trans_len invalid\n");
		return 0;
	}
	if (dma_check_param_valid(dst_addr,trans_len,dst_bus_width,dst_addr_update,&dst_str)) {
			info("dst_addr or trans_len invalid\n");
			return 0;
	}
	if (!strcmp(src_str,"share ram") ||
		!strcmp(dst_str,"share ram")) {
		ret = dma_test_prepare_share_ram();
		if (ret) {
			info("share ram init fail,skip\n");
			dma_test_unprepare_share_ram();
			return 0;
		}
	}
	info("%s to %s transfer: ch=%d src=0x%x dst=0x%x len=%d word\n",
		src_str,
		dst_str,
		ch,
		src_addr,
		dst_addr,
		trans_len);
	info("src: bus width:%d burst size:%d addr_update:%d\n",
		src_bus_width,src_burst_size,src_addr_update);
	info("dst: bus width:%d burst size:%d addr_update:%d\n",
			dst_bus_width,dst_burst_size,dst_addr_update);

	fail = dma_cfg_all(ch,trans_len,src_addr,src_bus_width,src_burst_size,src_addr_update,
		dst_addr,dst_bus_width,dst_burst_size,dst_addr_update,display_flag);

	if (!strcmp(src_str,"share ram") ||
		!strcmp(dst_str,"share ram")) {
		ret = dma_test_unprepare_share_ram();
	}
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

static int dma_int_counter[8] = {0};
void dma_int_handler(void* ptr)
{
	int int_status = 0;
	unsigned long ch_status;
	int i;

	int_status = dw_dmac_get_int_status(p_dma_dev);
	info("dma_int_handler,status=%x\n", int_status);
	if (int_status) {
		if (int_status & DW_DMAC_INT_TFR) {
			for (i = 0; i < 8; i++) {
				ch_status = dw_dmac_get_ch_int_status(p_dma_dev,
					i,
					DMAC_TFR);
				if (ch_status) {
					info("ch %d tfr int triggered\n", i);
					dma_int_counter[i]++;
				}
				dw_dmac_set_ch_int_clear(p_dma_dev,
					i,
					DMAC_TFR);
			}
		}
		if (int_status & DW_DMAC_INT_BLOCK) {
			for (i = 0; i < 8; i++) {
				ch_status = dw_dmac_get_ch_int_status(p_dma_dev,
					i,
					DMAC_BLOCK);
				if (ch_status) {
					debug("ch %d block int triggered\n", i);
					dma_int_counter[i]++;
				}
				dw_dmac_set_ch_int_clear(p_dma_dev,
					i,
					DMAC_BLOCK);
			}
		}
		if (int_status & DW_DMAC_INT_SRCT) {
			for (i = 0; i < 8; i++) {
				ch_status = dw_dmac_get_ch_int_status(p_dma_dev,
					i,
					DMAC_SRC_TRAN);
				if (ch_status) {
					debug("ch %d srct int triggered\n", i);
					dma_int_counter[i]++;
				}
				dw_dmac_set_ch_int_clear(p_dma_dev,
					i,
					DMAC_SRC_TRAN);
			}
		}
		if (int_status & DW_DMAC_INT_DSTT) {
			for (i = 0; i < 8; i++) {
				ch_status = dw_dmac_get_ch_int_status(p_dma_dev,
					i,
					DMAC_DST_TRAN);
				if (ch_status) {
					debug("ch %d dstt int triggered\n", i);
					dma_int_counter[i]++;
				}
				dw_dmac_set_ch_int_clear(p_dma_dev,
					i,
					DMAC_DST_TRAN);
			}
		}
		if (int_status & DW_DMAC_INT_ERR) {
			for (i = 0; i < 8; i++) {
				ch_status = dw_dmac_get_ch_int_status(p_dma_dev,
					i,
					DMAC_ERR);
				if (ch_status) {
					debug("ch %d err int triggered\n", i);
					dma_int_counter[i]++;
				}
				dw_dmac_set_ch_int_clear(p_dma_dev,
					i,
					DMAC_ERR);
			}
		}
	}
}

/*testcase:dma interrupt test*/
static int dma_int(int argc, char* argv[])
{
	int fail = 0;
	unsigned long src_addr,dst_addr;
	unsigned long trans_len;
	int ch[8];
	struct memory_area *mm_ddr = (struct memory_area*)&memory[1];
	int i;

	info("dma_int: interrupt handle test\n");

	if (argc < 2) {
		info("wrong cmd_format: dma_int ch0 [ch1] [ch2] [ch3] [ch4] [ch5] [ch6] [ch7] \n");
		info("valid example test pattern: 1. dma_int 0 | 2. dma_int 0 1 | 3. dma_int 0 1 2 | 4. dma_int 0 1 2 3 | 5. dma_int 0 1 2 3 4 5 6 7");
		return 0;
	}

	for (i=0;i<8;i++)
		ch[i] = -1;
	ch[0] = get_arg_ulong(argv[1]);
	if (ch[0])
	{
		info("The first parameter should be '0'(means channel0), please follow the valid test pattern.\n");
		info("valid example test pattern: 1. dma_int 0 | 2. dma_int 0 1 | 3. dma_int 0 1 2 | 4. dma_int 0 1 2 3 | 5. dma_int 0 1 2 3 4 5 6 7");
		return 0;
	}
	info("notes: channels must be continus\n");
	if (argc>2)
		ch[1] = get_arg_ulong(argv[2]);
	if (argc>3)
		ch[2] = get_arg_ulong(argv[3]);
	if (argc>4)
		ch[3] = get_arg_ulong(argv[4]);
	if (argc>5)
		ch[4] = get_arg_ulong(argv[5]);
	if (argc>6)
		ch[5] = get_arg_ulong(argv[6]);
	if (argc>7)
		ch[6] = get_arg_ulong(argv[7]);
	if (argc>8)
		ch[7] = get_arg_ulong(argv[8]);

	for (i=0;i<8;i++) {
		if ((ch[i]!= -1) && (ch[i]>7)) {
			info("wrong ch number %d,should be 0-7\n", ch[i]);
			return 0;
		}
	}
	trans_len = 1024;
	src_addr = mm_ddr->start;
	dst_addr = mm_ddr->start+ (trans_len*4+64)*8;

	/*register int handler and enable it*/
	irq_generic_install_handler(p_dma_dev->int_vector,
		dma_int_handler,
		NULL);
	irq_generic_config_int(p_dma_dev->int_vector, IRQ_TYPE_LEVEL_HIGH);
	irq_generic_enable_int(p_dma_dev->int_vector);

	/*enable interrupt and configre transfer start*/

	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		/*config src*/
		dw_dmac_channel_config(p_dma_dev,
			ch[i],
			0,
			WORD_WIDTH,
			BURST_4,
			INCREASE,
			
			MEM_TO_MEM,
			0);
		/*config dst*/
		dw_dmac_channel_config(p_dma_dev,
			ch[i],
			1,
			WORD_WIDTH,
			BURST_4,
			INCREASE,
			
			MEM_TO_MEM,
			0);
		/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
		dma_prepare_buffer(src_addr+(trans_len*4+64)*i, dst_addr+(trans_len*4+64)*i, trans_len, WORD_WIDTH);
	}

	/*enable the ch interrupt*/
	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		dw_dmac_set_ch_int_mask(p_dma_dev, ch[i], DMAC_TFR, 1);
		dw_dmac_set_ch_int_mask(p_dma_dev, ch[i], DMAC_BLOCK, 1);
		dw_dmac_set_ch_int_mask(p_dma_dev, ch[i], DMAC_SRC_TRAN, 1);
		dw_dmac_set_ch_int_mask(p_dma_dev, ch[i], DMAC_DST_TRAN, 1);
		dw_dmac_set_ch_int_mask(p_dma_dev, ch[i], DMAC_ERR, 1);
	}
	/*start ch transfer*/
	for (i=0;i<8;i++)
		dma_int_counter[i] = 0;
	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		/*start ch transfer*/
		dw_dmac_channel_single_transfer(p_dma_dev,
			ch[i],
			src_addr+(trans_len*4+64)*i+32,
			dst_addr+(trans_len*4+64)*i+32,
			trans_len,
			1);
	}

	mdelay(3000);

	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		if (dma_int_counter[i] == 0) {
			fail++;
		}
	}

	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	/*disable interrupt*/
	irq_generic_disable_int(p_dma_dev->int_vector);
	irq_generic_remove_handler(p_dma_dev->int_vector,NULL,NULL);
	/*clear all pending int status*/
	dw_dmac_init(p_dma_dev, (unsigned long)p_dma_dev->regs,p_dma_dev->int_vector);

	return 0;
}

void dummy_gpio_int_handler(void *ptr)
{
	bool ret_val;
	E_GPIO_PORT port = GPIOB;
	E_PIN_PORT pin = PIN_31;

	//info("gpio_int_handler\n");

	ret_val = gpio_get_int_status(port, pin);
	if (ret_val) 
	{
		gpio_clear_int(port, pin);
	}

	info("gpio_int_handler, status=%x count=%d\n", ret_val);
	
	// for level interrupt, disable int to prevent so much int occur
	if (console_tstc()) 
	{
		if ('q' == console_getc()) 
		{
			gpio_set_int_mask(port, pin , GPIO_IM_EN);
		}
	}
}

/*testcase:dma interrupt test*/
static int dma_gpio_trigger(int argc, char* argv[])
{
	int fail = 0;
	unsigned long src_addr = SRAM_BASE,dst_addr = SRAM_BASE+SRAM_SIZE/2;
	unsigned long trans_len = 0x100;
	int ch = 0;
	E_GPIO_PORT gpio_port = GPIOB;
	E_PIN_PORT gpio_pin = PIN_31;
	E_GPIO_IG gpio_ig = GPIO_IG_INTR0;
	E_ARM_DMA_HS_EXT_INT_SRC dma_ext_src;
	unsigned long flag;

	int ret;

	info("dma_gpio_trigger: dma gpio trigger test\n");

	if (argc < 2) {
		info("wrong cmd_format: dma_gpio_trigger tirgger_flag(0/1)\n");
		return 0;
	}

	flag = get_arg_ulong(argv[1]);	

	// assign external interrupt source
	if (gpio_port == GPIOA)
	{
		dma_ext_src = (gpio_ig == GPIO_IG_INTR0) ? ARM_DMA_HS_EXT_GPIOA_INT0 : ARM_DMA_HS_EXT_GPIOA_INT1;
	}
	else if (gpio_port == GPIOB)
	{
		dma_ext_src = (gpio_ig == GPIO_IG_INTR0) ? ARM_DMA_HS_EXT_GPIOB_INT0 : ARM_DMA_HS_EXT_GPIOB_INT1;
	}
	else if (gpio_port == GPIOC)
	{
		dma_ext_src = (gpio_ig == GPIO_IG_INTR0) ? ARM_DMA_HS_EXT_GPIOC_INT0 : ARM_DMA_HS_EXT_GPIOC_INT1;
	}
	else if (gpio_port == GPIOD)
	{
		dma_ext_src = (gpio_ig == GPIO_IG_INTR0) ? ARM_DMA_HS_EXT_GPIOD_INT0 : ARM_DMA_HS_EXT_GPIOD_INT1;
	}
	
	if (!misc_dma_ext_int_src_sel(p_dma_dev->dma_port, dma_ext_src))
	{
		return 0;
	}	

	/*enable interrupt and configre transfer start*/
	/*config src*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		0,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		MEM_TO_PERI,
		0);
	
	/*config dst*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		1,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		MEM_TO_PERI,
		0);
	/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
	dma_prepare_buffer(src_addr, dst_addr, trans_len, WORD_WIDTH);
	/*start ch transfer*/
	dw_dmac_channel_single_transfer_ext(p_dma_dev,
		ch,
		src_addr + 32,
		dst_addr + 32,
		trans_len,
		1);
	if(flag)
	{	
		// initialize gpio pin
		if (!misc_gpio_pin_init(gpio_port, gpio_pin, IO_PU_UP, IO_PD_EN, IO_OD_DIS))
		{
			return 0;
		}
		
		gpio_init(gpio_port, GPIO_BASE, GPIO_PCLK, 0);
		gpio_enable_debounce(gpio_port, gpio_pin, GPIO_DB_EN);
		gpio_enable_int(gpio_port, gpio_pin, GPIO_INT_DIS);
		gpio_clear_int(gpio_port, gpio_pin);
		
		gpio_set_direction(gpio_port, gpio_pin, GPIO_INPUT);
		gpio_set_ctrl(gpio_port, gpio_pin, GPIO_CTRL_SW);
		gpio_set_ig(gpio_port, gpio_pin, gpio_ig);

		// register int handler and enable it			
		irq_generic_install_handler(INT_GPIOA0, dummy_gpio_int_handler, NULL);
		irq_generic_config_int(INT_GPIOA0, IRQ_TYPE_EDGE_BOTH);
		irq_generic_enable_int(INT_GPIOA0);
		
		// enable interrupt
		gpio_set_int_type(gpio_port, gpio_pin, GPIO_IS_EDGE);
		gpio_set_int_mask(gpio_port, gpio_pin, GPIO_IM_DIS);
		gpio_set_int_trig_type(gpio_port, gpio_pin, GPIO_ITYPE_EDGE_BOTH);
		gpio_enable_int(gpio_port, gpio_pin, GPIO_INT_EN);	

		info("wait for user tigger GPIO Bank 0 & Pin 0's edge, and then press 'g'\n");
		while(1) {	
			if (console_tstc()) {
				if (console_getc() == 'g') {
					break;
				}
			}		
		}
		// disable interrupt
		irq_generic_disable_int(INT_GPIOA0);
		irq_generic_remove_handler(INT_GPIOA0, NULL, NULL);
		gpio_set_int_mask(gpio_port, gpio_pin, GPIO_IM_EN);
		gpio_enable_int(gpio_port, gpio_pin, GPIO_INT_DIS);
		gpio_clear_int(gpio_port, gpio_pin);

	}
    
	ret = dw_dmac_channel_wait_done(p_dma_dev, ch, DMAC_TIMEOUT);
	if (ret != 0) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	return 0;
}

void dummy_timer_int_handler(void *ptr)
{
	uint32_t reg_val;

	reg_val = gtimer_get_int_status(TIMER1);
	if (reg_val) 
	{
		irq_generic_disable_int(INT_TIMER1);
		gtimer_clear_int(TIMER1, reg_val);		
	}
	
	if (reg_val & 0x2)
	{
		info("timer_int_handler:TMR_OVF_INT_STA trig\r\n");
	}	
	else
	{
		info("timer_int_handler,unexpect int 0x%04x\n", reg_val);
	}	
}

/*testcase:dma interrupt test*/
static int dma_timer_trigger(int argc, char* argv[])
{
	int fail = 0;
	unsigned long src_addr = SRAM_BASE,dst_addr = SRAM_BASE+SRAM_SIZE/2;
	unsigned long trans_len = 0x100;
	int ch = 0;
	unsigned long flag;
	unsigned long time_id = TIMER1;
	int ret;

	info("dma_timer_trigger: dma timer trigger test\n");

	if (argc < 2) {
		info("wrong cmd_format: dma_int tirgger_flag(0/1)\n");
		return 0;
	}

	flag = get_arg_ulong(argv[1]);

	// assign external interrupt source
	if (!misc_dma_ext_int_src_sel(p_dma_dev->dma_port, (TIMER1+8)))
	{
		return 0;
	}

	/*config src*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		0,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		MEM_TO_PERI,
		0);
	/*config dst*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		1,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		MEM_TO_PERI,
		0);
	/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
	dma_prepare_buffer(src_addr, dst_addr, trans_len, WORD_WIDTH);

	/*start ch transfer*/
	dw_dmac_channel_single_transfer_ext(p_dma_dev,
		ch,
		src_addr + 32,
		dst_addr + 32,
		trans_len,
		1);

	// set trigger event
	if(flag)
	{
		gtimer_init(time_id, TIMER_BASE);
		gtimer_config_default(time_id);
		gtimer_set_mode(time_id, TMR_UP, TMR_AUTO, TMR_USER);
		gtimer_clear_int(time_id, 0xff);
		gtimer_clear_raw_status(time_id, 0xff);
		
		// register int handler and enable it
		irq_generic_install_handler(INT_TIMER1, dummy_timer_int_handler, NULL);
		irq_generic_config_int(INT_TIMER1, IRQ_TYPE_LEVEL_HIGH);
		irq_generic_enable_int(INT_TIMER1);
		
		gtimer_set_int_mask(time_id, 0x7);

		gtimer_set_load(time_id, 0xffffffff - 0x1000);
		gtimer_start(time_id);
		gtimer_reload(time_id);
	    mdelay(100);		
		gtimer_stop(time_id);
		
		// disable interrupt
		irq_generic_disable_int(INT_TIMER1);
		irq_generic_remove_handler(INT_TIMER1, NULL, NULL);
		gtimer_set_int_mask(time_id, 0);
		gtimer_clear_int(time_id, 0xff);
		gtimer_clear_raw_status(time_id, 0xff);		
	}

	ret = dw_dmac_channel_wait_done(p_dma_dev, ch, DMAC_TIMEOUT);

	if (ret != 0) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}

	return 0;
}

/*testcase:multi-channel transfer test*/
static int dma_mc(int argc, char* argv[])
{
	unsigned long src_addr;
	unsigned long dst_addr;
	int ch[8] = {-1};
	unsigned long trans_len = 64;
	int ret;
	int fail = 0;
	int i;
	int ch_cnt = 0;
	int tmp;
	struct memory_area *mm_share = (struct memory_area*)&memory[2];

	info("dma_mc src_addr dst_addr ch1 ch2 ch3...\n");
	if (argc < 3) {
		info("wrong cmd_format: dma_mc src_addr dst_addr ch1 ch2 [ch3] [ch4] [ch5] [ch6] [ch7] [ch8]\n");
		info("notes: channels must be continus\n");
		info("{\"sram\",SRAM_BASE,SRAM_SIZE}\r\n");
		info("{\"ddr\",0x41000000,0x02000000}\r\n");
		info("{\"share ram\",A7_IPC_RAM_BASE,32*1024}\r\n");		
		return 0;
	}

	src_addr = get_arg_ulong(argv[1]);
	dst_addr = get_arg_ulong(argv[2]);
	ch[0] = get_arg_ulong(argv[3]);
	ch[1] = get_arg_ulong(argv[4]);
	if (argc>5)
		ch[2] = get_arg_ulong(argv[5]);
	if (argc>6)
		ch[3] = get_arg_ulong(argv[6]);
	if (argc>7)
		ch[4] = get_arg_ulong(argv[7]);
	if (argc>8)
		ch[5] = get_arg_ulong(argv[8]);
	if (argc>9)
		ch[6] = get_arg_ulong(argv[9]);
	if (argc>10)
		ch[7] = get_arg_ulong(argv[10]);

	/*check valid*/
	for (i=0;i<8;i++) {
		if (ch[i] > 7) {
			info("wrong ch[%d] number %d,should be 0-7\n", i, ch[i]);
			return 0;
		}
		else if (ch[i] != -1)
		{
			ch_cnt++;
		}
	}

	//src & dst parameters checking
	for(i=0; i<3; i++) {
		if ((src_addr >= memory[i].start) && (src_addr < ( memory[i].start +  memory[i].size)))
		{
			if((src_addr + (trans_len * 4 + 64) * ch_cnt) > ( memory[i].start +  memory[i].size))
			{
				info("invalid src addr \r\n");
				return 0;
			}
		}
	}

	if ((dst_addr >=  memory[i].start) && (src_addr < ( memory[i].start +  memory[i].size)))
	{
		if((dst_addr + (trans_len * 4 + 64) * ch_cnt) > ( memory[i].start +  memory[i].size))
		{
			info("invalid dst addr \r\n");
			return 0;
		}
	}		

	if(src_addr > dst_addr)
		tmp = src_addr - dst_addr;
	else
		tmp = dst_addr - src_addr;
	if(tmp  < (trans_len * 4 + 64) *ch_cnt)
	{
		info("invalid dst or src addr \r\n");
		return 0;
	}

	if ((src_addr >= mm_share->start &&
			src_addr < (mm_share->start + mm_share->size)) ||
			(dst_addr >= mm_share->start &&
			dst_addr < (mm_share->start + mm_share->size))) {
		ret = dma_test_prepare_share_ram();
		if (ret) {
			info("share ram init fail,skip\n");
			dma_test_unprepare_share_ram();
			return 0;
		}
	}
	
	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		/*config src*/
		dw_dmac_channel_config(p_dma_dev,
			ch[i],
			0,
			WORD_WIDTH,
			BURST_4,
			INCREASE,
			
			MEM_TO_MEM,
			0);
		/*config dst*/
		dw_dmac_channel_config(p_dma_dev,
			ch[i],
			1,
			WORD_WIDTH,
			BURST_4,
			INCREASE,
			
			MEM_TO_MEM,
			0);
		/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
		dma_prepare_buffer(src_addr+(trans_len*4+64)*i, dst_addr+(trans_len*4+64)*i, trans_len, WORD_WIDTH);
	}

	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		/*start ch transfer*/
		dw_dmac_channel_single_transfer(p_dma_dev,
			ch[i],
			src_addr+(trans_len*4+64)*i+32,
			dst_addr+(trans_len*4+64)*i+32,
			trans_len,
			0);
	}

	for (i=0;i<8;i++) {
		if (ch[i]== -1)
			break;
		ret = dw_dmac_channel_wait_done(p_dma_dev, ch[i], DMAC_TIMEOUT);
		if (ret) {
			fail++;
		}
	}

	if (!fail) {
		for (i=0;i<8;i++) {
			if (ch[i]== -1)
				break;
			ret = dma_check_buffer(src_addr+(trans_len*4+64)*i,
				dst_addr+(trans_len*4+64)*i,
				trans_len,
				0,
				WORD_WIDTH,
				INCREASE);
			if (ret) {
				debug("dmac ch %d transfer result check error\n", ch[i]);
				fail++;
			}
		}
	}
	if ((src_addr >= mm_share->start &&
			src_addr < (mm_share->start + mm_share->size)) ||
			(dst_addr >= mm_share->start &&
			dst_addr < (mm_share->start + mm_share->size))) {
		ret = dma_test_unprepare_share_ram();
	}
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

/*testcase:multi-block transfer test*/
static int dma_mb(int argc, char* argv[])
{
	unsigned long src_addr,dst_addr;
	int ch;
	unsigned long trans_len;
	int ret;
	int fail = 0;
	struct dw_dmac_lli_item* lli_hdr;
	struct dw_dmac_lli_item* lli_prev = NULL;
	struct dw_dmac_lli_item* lli_curr;
	unsigned long ctl;
	unsigned long count_per_block;
	int lli_count_used = 0;
	unsigned long bsrc,bdst;
	unsigned long blen;
	char *src_str,*dst_str;

	struct memory_area * mm_sram = (struct memory_area*)&memory[0];
	struct memory_area *mm_ddr = (struct memory_area*)&memory[1];
	struct memory_area *mm_share = (struct memory_area*)&memory[2];

	info("%s: dma memory to memory multi-block test: ddr/sram/shared ram\n",argv[0]);
	info("format: %s ch src_addr dst_addr length\n",argv[0]);
	info("ch:0-7\n");
	info("src_addr: src addr\n");
	info(" ddr: 0x%x - 0x%x\n",mm_ddr->start,mm_ddr->start+mm_ddr->size-1);
	info(" sram: 0x%x - 0x%x\n",mm_sram->start,mm_sram->start+mm_sram->size-1);
	info(" share ram: 0x%x - 0x%x\n",mm_share->start,mm_share->start+mm_share->size-1);
	info("length: unit is dword(4 bytes)\n");

	if (argc < 5) {
		info("wrong cmd_format: %s ch src_addr dst_addr length\n",argv[0]);
		return 0;
	}

	ch = get_arg_ulong(argv[1]);
	src_addr = get_arg_ulong(argv[2]);
	dst_addr = get_arg_ulong(argv[3]);
	trans_len = get_arg_ulong(argv[4]);
	if (ch > 7) {
		info("wrong ch number %d,should be 0-7\n", ch);
		return 0;
	}
	/*check valid*/
	if (dma_check_param_valid(src_addr,trans_len,WORD_WIDTH,INCREASE,&src_str)) {
			info("src_addr or trans_len invalid\n");
			return 0;
	}
	if (dma_check_param_valid(dst_addr,trans_len,WORD_WIDTH,INCREASE,&dst_str)) {
			info("dst_addr or trans_len invalid\n");
			return 0;
	}
	if (!strcmp(src_str,"share ram") ||
		!strcmp(dst_str,"share ram")) {
		ret = dma_test_prepare_share_ram();
		if (ret) {
			info("share ram init fail,skip\n");
			dma_test_unprepare_share_ram();
			return 0;
		}
	}
	info("%s to %s transfer: ch=%d src=0x%x dst=0x%x len=%d word\n",
		src_str,
		dst_str,
		ch,
		src_addr,
		dst_addr,
		trans_len);


	lli_hdr = (struct dw_dmac_lli_item *)lli_buffer;
    memset(lli_hdr, 0, sizeof(lli_buffer));
	/*cofigure CTL register value*/
	ctl = dw_dmac_config_lli_ctl(0,
		0,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		
		MEM_TO_MEM,
		0);
	ctl = dw_dmac_config_lli_ctl(ctl,
		1,
		WORD_WIDTH,
		BURST_4,
		INCREASE,
		
		MEM_TO_MEM,
		0);

	lli_curr = lli_hdr;
	lli_prev = NULL;
	bsrc = src_addr + 32;
	bdst = dst_addr + 32;
	blen = trans_len;
	while (blen > 0) {
		if (blen > DMAC_MAX_SIZE_PER_BLOCK) {
			count_per_block = DMAC_MAX_SIZE_PER_BLOCK;
		} else {
			count_per_block = blen;
		}
		ret = dw_dmac_config_lli_item(lli_prev,
			lli_curr,
			bsrc,
			bdst,
			ctl,
			count_per_block,
			0);
		if (ret) {
			info("configure lli fail\n");
			return 1;
		}
		lli_count_used++;
		lli_prev = lli_curr;
		lli_curr += 1;
		bsrc += count_per_block * 4;
		bdst += count_per_block * 4;
		blen -= count_per_block;
	}
	info("total %d LLI item used\n", lli_count_used);

	/*prepare buffer data,src buffer fill with random value; dst clear to 0*/
	dma_prepare_buffer(src_addr, dst_addr, trans_len, WORD_WIDTH);

	/*start ch transfer*/
	ret = dw_dmac_channel_lli_transfer_poll(p_dma_dev, ch, lli_hdr);
	if (ret) {
		debug("dmac ch %d transfer timeout\n", ch);
		fail++;
	}
	dw_dmac_channel_abort(p_dma_dev, ch);
	if (!fail) {
		ret = dma_check_buffer(src_addr,
			dst_addr,
			trans_len,
			0,
			WORD_WIDTH,
			INCREASE);
		if (ret) {
			debug("dmac ch %d transfer result check error\n", ch);
			fail++;
		}
	}
	if (!strcmp(src_str,"share ram") ||
		!strcmp(dst_str,"share ram")) {
		ret = dma_test_unprepare_share_ram();
	}
	if (fail) {
		info("%s: test fail\n",argv[0]);
	} else {
		info("%s: test pass\n",argv[0]);
	}
	return 0;
}

#ifdef TAPEOUT_V2
/*testcase:multi-block transfer test*/
static int dma_ch_reset(int argc, char* argv[])
{
#define MAX_UNIT 4095
	int ch = 0;
	int timeout_val = 0;
	int pattern = 0;
	int dmac_index = 0;
	int bdisplay = 0;
	int i = 0;
	int ret = 0;

	info("%s: dma ch reset test\n",argv[0]);
	if (argc < 5) {
		info("wrong cmd_format: %s ch timeout_val pattern bdisplay\n",argv[0]);
		return 0;
	}

	ch = get_arg_ulong(argv[1]);
	timeout_val = get_arg_ulong(argv[2]);
	pattern = get_arg_ulong(argv[3]);	
    bdisplay = get_arg_ulong(argv[4]);	
	
	memset(test_recv_buff, 0x0, sizeof(test_recv_buff));
	memset(test_send_buff, pattern, sizeof(test_send_buff));
	
	/*config src*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		0,
		BYTE_WIDTH,
		BURST_1,
		INCREASE,		
		MEM_TO_MEM,
		0);
	/*config dst*/
	dw_dmac_channel_config(p_dma_dev,
		ch,
		1,
		BYTE_WIDTH,
		BURST_1,
		INCREASE,		
		MEM_TO_MEM,
		0);

	/*start ch transfer*/
	dw_dmac_channel_single_transfer(p_dma_dev,
		ch,
		(unsigned long)test_send_buff,
		(unsigned long)test_recv_buff,
		MAX_UNIT,
		0);

	if(DMAC0_BASE == (unsigned long)p_dma_dev->regs)
		dmac_index = 0;
	else if(DMAC1_BASE == (unsigned long)p_dma_dev->regs)
		dmac_index = 1;
	else
		dmac_index = 2;
	
	misc_enable_dma_err_resp(dmac_index, timeout_val);
	ret = dw_dmac_channel_wait_done(p_dma_dev, ch, DMAC_TIMEOUT);
    misc_disable_dma_err_resp(dmac_index);

	
	if (ret) {
		info("test fail for reset take effect!!!\n");
		return 0;
	}

	if(bdisplay)
		print_buffer("dst",(char*)test_recv_buff, MAX_UNIT);
	
	for(i=0; i<MAX_UNIT; i++)
	{
		if(test_send_buff[i] != test_recv_buff[i])
		{
			info("data diff @ %x!!\n", i);
			return 0;
		}
	}

	info("test pass as what we expected!!\n");
	return 0;
}
#endif

static int cmd_help(int argc, char* argv[])
{
	return cmdline_help_general(&dma_test_menu[0]);
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
cmdline_entry dma_test_menu[] = {
	{ "help", cmd_help," : Display list of commands" },
	{ "h",cmd_help, 	"    : alias for help" },
	{ "?",	cmd_help, 	"    : alias for help" },
	{ "dma_m2m_robust",dma_m2m_robust,  "  : dma mem to mem robust test" },
	{ "dma_m2m",dma_m2m,  "   : dma mem to mem test" },
	{ "dma_cfg",dma_cfg,"  : dma channel config test" },
	{ "dma_cfg2",dma_cfg2,"  : dma channel config test" },
	{ "dma_int",				dma_int,				"  : dma interrupt test" },
	{ "dma_gpio_trigger",	dma_gpio_trigger,	"  : dma gpio trigger test" },
	{ "dma_timer_trigger",	dma_timer_trigger,	"  : dma timer tigger test" },
	{ "dma_mc",dma_mc,"  : dma multi-channel test" },
	{ "dma_mb",dma_mb,"  : dma multi-block transfer test" },
#ifdef TAPEOUT_V2
	{ "dma_ch_reset",dma_ch_reset,"  : dma channel reset test" },
#endif
	{ "q", NULL, " : quit dma test" },{ 0, 0, 0 }
};

/* if return none zero, something wrong,please check and can't continue test*/
int dma_test_prepare(int sel)
{
	int ret = 0;

	memset(p_dma_dev, 0, sizeof(struct dw_dmac));

	p_dma_dev->dma_port = sel;
	
	if (sel == 0) 
	{
		dw_dmac_init(p_dma_dev, DMAC0_BASE,INT_DMA0);
	} 
	else if (sel == 1) 
	{
		dw_dmac_init(p_dma_dev, DMAC1_BASE,INT_DMA1);
	} 
	else if (sel == 2) 
	{
		dw_dmac_init(p_dma_dev, DMAC2_BASE,INT_DMA2);
	} 
	else 
	{
		info("wrong device id\n");
		ret = 1;
	}

	return ret;
}

#define DMA_SAR_DEF_VAL	0x0
bool dma_reset_test(E_DMA_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_IP_RST ip;
	struct dw_dmac* p_dev = &dma_dev;

	info("%s: port:%d\n", __func__, port);
	dma_test_prepare(port);
	
	info("write sar reg\n");
	
	// write register
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + DMAC_REG_CH_SAR);

	info("read&compare sar reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DMAC_REG_CH_SAR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// reset
	info("dma reset\n");
	if 		(port == DMA0)	ip = ARM_DMA0_RST;
	else if (port == DMA1)	ip = ARM_DMA1_RST;
	else					ip = ARM_DMA2_RST;
	prcm_reset(ip);
	
	// read register
	info("read&compare sar reg\n");
	
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DMAC_REG_CH_SAR);
	if (*p_rd_val != DMA_SAR_DEF_VAL)
	{
		return false;
	}
		
	return true;
}

bool dma_cg_test(E_DMA_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	E_ARM_BLK_CG blk;
	struct dw_dmac* p_dev = &dma_dev;

	if 		(port == DMA0)	blk = ARM_BLK_CG_DAM0;
	else if (port == DMA1)	blk = ARM_BLK_CG_DAM1;
	else					blk = ARM_BLK_CG_DAM2;

	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);

	info("%s: port:%d\n", __func__, port);
	dma_test_prepare(port);
	
	// write register
	info("write sar reg\n");
	__raw_writel(*p_wr_val, (uint32_t)p_dev->regs + DMAC_REG_CH_SAR);

	info("read&compare sar reg\n");
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DMAC_REG_CH_SAR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
	
	// clock gate disable
	info("clock gate disable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_DIS);

	// write register
	info("write sar reg\n");
	__raw_writel((*p_wr_val+1), (uint32_t)p_dev->regs + DMAC_REG_CH_SAR);	
	
	// clock gate enable
	info("clock gate enable\n");
	prcm_blk_cg(blk, ARM_BLK_CG_EN);
	
	// read register
	info("read&compare sar reg\n");	
	*p_rd_val = __raw_readl((uint32_t)p_dev->regs + DMAC_REG_CH_SAR);
	if (*p_rd_val != *p_wr_val)
	{
		return false;
	}
		
	return true;
}
#else
bool dma_reset_test(E_DMA_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}

bool dma_cg_test(E_DMA_PORT port, uint32_t *p_wr_val, uint32_t *p_rd_val)
{
	return true;
}
#endif

