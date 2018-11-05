/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_ipc.h"

struct a7_ipc ipc_dev;
struct a7_ipc* p_ipc_dev = &ipc_dev;

void ipc_writel(struct a7_ipc *priv, unsigned long val,int reg)
{
	writel(val, (unsigned long)priv->reg_base + reg);
}
unsigned long ipc_readl(struct a7_ipc *priv,int reg)
{
	return readl((unsigned long)priv->reg_base + reg);
}


int a7_ipc_get_int_status_rf(struct a7_ipc *priv)
{
	return ipc_readl(priv, RFTOA7IPCSTS);
}

int a7_ipc_get_int_status_plc(struct a7_ipc *priv)
{
	return ipc_readl(priv, PLCTOA7IPCSTS);
}

int a7_ipc_clear_plc_int(struct a7_ipc *priv, int flag)
{
	ipc_writel(priv, 0x1 << flag, PLCTOA7IPCACK);
	return 0;
}

int a7_ipc_clear_rf_int(struct a7_ipc *priv, int flag)
{
	ipc_writel(priv, 0x1 << flag, RFTOA7IPCACK);
	return 0;
}

void ipc_clear_plctoa7_flag(struct a7_ipc *priv, int flag)
{
	ipc_writel(priv, 0x1 << flag, PLCTOA7IPCACK);
}

void ipc_clear_rftoa7_flag(struct a7_ipc *priv, int flag)
{
	ipc_writel(priv, 0x1 << flag, RFTOA7IPCACK);
}

int shared_status(struct a7_ipc *priv, int page)
{
	int val = 0;
	if((page < 16) && (page >= 0)){
		val = ipc_readl(priv, SRMSEL0);
		val = (val >> (page * 2)) & 0x3;
	}else if((page > 15) && (page < 32)){
		val = ipc_readl(priv, SRMSEL1);
		val = (val >> ((page - 16) * 2)) & 0x3;
	}else{
		val = -1;
	}
	return val;
}
int a7_set_page_ownership(struct a7_ipc *priv, int page)
{
	int val =0;
	val = (A7_REQ_KEY<<4) | 0x1;
	ipc_writel(priv, val, A7SRP00REQ + page * 4);
	val = shared_status(priv, page);
	if(val != 1){
		/*debug("a7 set page %d ownership error.\n", page);*/
		return -1;
	}
	return 0;
}
int a7_get_sram_page(struct a7_ipc *priv, int page)
{
	int status;
	int ret = 0;

	status = shared_status(priv, page);
	if(status != 0){
		debug("cannot get a free page %d. A7_SRMSEL 0x%08x\n",
				page, ((page < 16) ? ipc_readl(priv, SRMSEL0) : ipc_readl(priv, SRMSEL1)));
		return -1;
	}
	ret = a7_set_page_ownership(priv, page);
	if(status < 0){
		debug("a7_set_page_ownership %d error. A7_SRMSEL 0x%08x\n",
				page, ((page < 16) ? ipc_readl(priv, SRMSEL0) : ipc_readl(priv, SRMSEL1)));
		return -1;
	}
	/*info("a7_get_sram_page %d\n", page);*/
	return ret;
}

int a7_free_page(struct a7_ipc *priv, int page)
{
	int val =0;
	val = shared_status(priv, page);
	if(val == 0){
		debug("page %d is free, don't need free again. A7_SRMSEL 0x%08x\n",
				page, ((page < 16) ? ipc_readl(priv, SRMSEL0) : ipc_readl(priv, SRMSEL1)));
		return 0;
	}
	val = (A7_REQ_KEY<<4) | 0x0;
	ipc_writel(priv, val, A7SRP00REQ + page * 4);
	val = shared_status(priv, page);
	if(val != 0){
		debug("page %d free error. A7_SRMSEL 0x%08x\n",
				page, ((page < 16) ? ipc_readl(priv, SRMSEL0) : ipc_readl(priv, SRMSEL1)));
		return -1;
	}
	/*info("plc free page %d\n", page);*/
	return 0;
}

int ipc_set_event_a7torf(struct a7_ipc *priv, int flag)
{
	int a7torfflg = -1, a7torfcsts = -1, count = 500;
	u32 val;

	do{
		a7torfflg = ipc_readl(priv, A7TORFIPCFLG);//check the handshake, make sure the bit is '0'
		a7torfflg = (a7torfflg >> flag) & 0x1;
		count--;
	}while(((a7torfflg == 1)&&(count > 0)));
	if(count < 0){
		debug("ipc_set_event_a7torf, this flag is used.\n");
		return -1;
	}

	val = 0x1 << flag;
	ipc_writel(priv, val, A7TORFIPCSET);//start the handshake

	return 0;
}

int ipc_set_event_a7toplc(struct a7_ipc *priv, int flag)
{
	int a7toplcflg = -1, a7toplcsts = -1, count = 500;
	u32 val;
	do{
		a7toplcflg = ipc_readl(priv, A7TOPLCIPCFLG);//check the handshake, make sure the bit is '0'
		a7toplcflg = (a7toplcflg >> flag) & 0x1;
		count--;
	}while(((a7toplcflg == 1)&&(count > 0)));
	if(count < 0){
		debug("ipc_set_event_a7toplc, this flag is used.\n");
		return -1;
	}

	val = 0x1 << flag;
	ipc_writel(priv, val, A7TOPLCIPCSET);//start the handshake

	return 0;
}

int ipc_set_msg_a7toplc(struct a7_ipc *priv, int flag, u32* data, int len)
{
	int i, ret = 0;
	int page = flag;

	if(len >1024)
	{
		debug("len is more than 1024B\n");
		return -1;
	}
	
	if (a7_get_sram_page(priv, page) < 0) {
		debug("a7toplc:a2 get sram page %d failed\n", page);
		return -1;
	}
	memset(priv->ram_base + page*IPC_PAGE_LEN, 0x0, len);
	memcpy(priv->ram_base + page*IPC_PAGE_LEN, data, len);
	ipc_writel(priv, IPC_DATA_READ, A7TOPLCIPCCOMM);
	ipc_writel(priv, (unsigned long )priv->ram_base + page*IPC_PAGE_LEN, A7TOPLCIPCADDR);
	ipc_writel(priv, len, A7TOPLCIPCDATA0);
	ipc_writel(priv, page | (IPC_END_MSG<<16), A7TOPLCIPCDATA1);

	ret = ipc_set_event_a7toplc(priv, flag);
	if (ret < 0){
		debug("ipc_set_msg_a7toplc, please used other flag.\n");
	}
	//info("ipc_set_msg_a7toplc flag %d,len = %d\n", flag, len);
	if (a7_free_page(priv, page) < 0)
		debug("a7toplc:a2 free page %d failed\n", page);

	return ret;
}

int ipc_set_msg_a7torf(struct a7_ipc *priv, int flag, u32* data, int len)
{
	int i, ret = 0;
	int page = flag;

	if(len >1024)
	{
		debug("len is more than 1024B\n");
		return -1;
	}
	
	if (a7_get_sram_page(priv, page) < 0) {
		debug("a7torf:a2 get sram page %d failed\n", page);
		return -1;
	}
	memset(priv->ram_base + page*IPC_PAGE_LEN, 0x0, len);
	memcpy(priv->ram_base + page*IPC_PAGE_LEN, data, len);
	ipc_writel(priv, IPC_DATA_READ, A7TORFIPCCOMM);
	ipc_writel(priv, (unsigned long )priv->ram_base + page*IPC_PAGE_LEN, A7TORFIPCADDR);
	ipc_writel(priv, len, A7TORFIPCDATA0);
	ipc_writel(priv, page | (IPC_END_MSG<<16), A7TORFIPCDATA1);

	ret = ipc_set_event_a7torf(priv, flag);
	if(ret < 0) {
		debug("ipc_set_event_a7torf, please used other flag.\n");
	}
	//info("ipc_set_event_a7torf flag %d,len = %d\n", flag, len);
	if (a7_free_page(priv, page) < 0)
		debug("a7torf:a2 free page %d failed\n", page);

	return ret;
}

int ipc_get_msg_plctoa7(struct a7_ipc *priv, int flag, u32 * data, int *lens)
{
	int plctoa7flg, regs, len = 0, count = 500, page;
	u32 addr;

	page = flag;
	plctoa7flg = ipc_readl(priv, PLCTOA7IPCSTS);
	plctoa7flg = (plctoa7flg >> flag) & 0x1;
	if(plctoa7flg == 1)
	{
		//info("ipc_get_msg_plctoa7 flag %d\n", flag);
		regs = ipc_readl(priv, PLCTOA7IPCCOMM);
		if(regs == IPC_DATA_READ){
			addr = ipc_readl(priv, PLCTOA7IPCADDR);
			len = ipc_readl(priv, PLCTOA7IPCDATA0);
			*lens = len;
			if((ipc_readl(priv, PLCTOA7IPCDATA1) >> 16) == IPC_END_MSG)
				{
					memcpy(data, priv->ram_base+page*IPC_PAGE_LEN, len);
				}
		}
		ipc_clear_plctoa7_flag(priv, flag);
		return 1;
	}
	return 0;
}

int ipc_get_msg_rftoa7(struct a7_ipc *priv, int flag, u32 * data, int *lens)
{
	int rftoa7flg, regs, len = 0, count = 500, page;
	u32 addr;

	page = flag;
	rftoa7flg = ipc_readl(priv, RFTOA7IPCSTS);
	rftoa7flg = (rftoa7flg >> flag) & 0x1;
	if(rftoa7flg == 1)
	{
		//info("ipc_get_msg_rftoa7 flag %d\n", flag);
		regs = ipc_readl(priv, RFTOA7IPCCOMM);
		if(regs == IPC_DATA_READ){
			addr = ipc_readl(priv, RFTOA7IPCADDR);
			len = ipc_readl(priv, RFTOA7IPCDATA0);
			*lens = len;
			if((ipc_readl(priv, RFTOA7IPCDATA1) >> 16) == IPC_END_MSG)
				{
					memcpy(data, priv->ram_base+page*IPC_PAGE_LEN, len);
				}
		}
		ipc_clear_rftoa7_flag(priv, flag);
		return 1;
	}
	return 0;
}

#ifdef TAPEOUT_V2
uint32_t ipc_get_timer_prescaler(struct a7_ipc *priv)
{
	uint32_t reg_val = ipc_readl(priv, IPCTMRSCALER);
	
	reg_val &= IPC_TMR_PRESCALER_MASK;
	
	return reg_val;
}

E_IPC_TMR_COUNT_MODE ipc_get_time_mode(struct a7_ipc *priv)
{
	return ipc_readl(priv, IPCTMRCONT);
}

void ipc_set_time_mode(struct a7_ipc *priv, E_IPC_TMR_COUNT_MODE mode)
{	
	uint32_t reg_val = ipc_readl(priv, IPCTMRCONT);

	reg_val &= ~IPC_TMR_COUNTUP_MASK;
	reg_val |= IPC_TMR_COUNTUP_VAL(mode);	
	
	ipc_writel(priv, reg_val, IPCTMRCONT);
}

void ipc_set_timer_prescaler(struct a7_ipc *priv, uint32_t prescaler)
{
	uint32_t reg_val = ipc_readl(priv, IPCTMRSCALER);
	
	reg_val &= ~IPC_TMR_PRESCALER_MASK;
	reg_val |= IPC_TMR_PRESCALER_VAL(prescaler);
	
	ipc_writel(priv, reg_val, IPCTMRSCALER);
}

#endif

unsigned long long ipc_get_time(struct a7_ipc *priv)
{
	unsigned long long time;
	
	time = ipc_readl(priv, IPCCOUNTERL);
	time |= (unsigned long long)ipc_readl(priv, IPCCOUNTERH)<<32;;
	
	return time;
}

