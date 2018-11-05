/*------------------------------------------------------------------------------------------------*/
/*      											  */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.      	  */
/*      											  */
/*------------------------------------------------------------------------------------------------*/


#include "cdl_config.h"
#include "cdl_otp.h"
#include "cdl_sys.h"
#include "timer.h"

#define OTP_TIMEOUT (500)   /*500 ms*/
#define RETRY_TIMES (0)

const struct brite_otp_timing default_otp_timing = {
	0x00000f50,0x002A0D10,0x02A085D7,0x0ef27779,
	0x540fc150,0x01d50690,0x00000022
};

static inline void otp_write(struct brite_otp* priv, u32 data, int reg_offset)
{
	__raw_writel(data, (u32)priv->regs + reg_offset);
}

static inline uint32_t otp_read(struct brite_otp* priv, u32 reg_offset)
{
	return __raw_readl((u32)priv->regs + reg_offset);
}

#ifdef TAPEOUT_V2
void otp_sw_rest(struct brite_otp* priv)
{
	/* reset */
	otp_write(priv, 0x1, OTP_SOFTRST);
	/* config timing*/
	otp_write(priv, priv->otp_timing.rst_param, OTP_RST_PARAM);
	otp_write(priv, priv->otp_timing.init_param, OTP_INIT_PARAM);
	otp_write(priv, priv->otp_timing.rd_param, OTP_RD_PARAM);
	otp_write(priv, priv->otp_timing.prg_param0, OTP_PRG_PARAM0);
	otp_write(priv, priv->otp_timing.prg_param1, OTP_PRG_PARAM1);
	otp_write(priv, priv->otp_timing.prg_param2, OTP_PRG_PARAM2);
	otp_write(priv, priv->otp_timing.prg_param3, OTP_PRG_PARAM3);
	/* release */
	otp_write(priv, 0x0, OTP_SOFTRST);
}
#endif

//always success as designer's suggestion
static int otp_access_request(struct brite_otp* priv)
{
#ifdef TAPEOUT_V2
	u32 reg_val = 0;
	u64 start = 0;

    start = get_timer(0);
	do
	{
		otp_write(priv, 0x1, OTP_REQ);
		reg_val = otp_read(priv, OTP_STATUS);
		if (get_timer(start) > OTP_TIMEOUT) {
			info("otp_access_request: request lock timeout OTP_STATUS:0x%x, refresh:0x%x\n", otp_read(priv, OTP_STATUS),
					otp_read(priv, OTP_REFRESH_CTRL_REG));
			return -ETIME;
		}
	} while ((reg_val& OTP_ST_LOCKED_MASK) != OTP_ST_LOCKED_MASK);
#endif
	return 0;
}

static int otp_prog_bit(struct brite_otp* priv, unsigned long row,
	unsigned char col, unsigned char data)
{
	u8 np1 = 0,np2 = 0;
	int ret = 0;
	u64 start = 0;

    if ((otp_get_sys_status(priv) & OTP_PROG_DIS) || (otp_access_request(priv) != 0))
		return -EPERM;

	/* 1. Set proper values to all xx_PARAM registers based on clock frequency*/
	otp_write(priv, PRG_START_EN_FLAG, OTP_PRG_EN);

	/* 2. Polling the OTP_RDY bit of OTP_STATUS register. If it's high, then go on; otherwise
	wait for it until high come.
	*/
#ifndef TAPEOUT_V2
	while ((otp_read(priv,
		OTP_STATUS) & OTP_ST_READY_MASK) != OTP_ST_READY_MASK);
#endif
	/* 3. Set OTP row address in ADDR_ROW register.*/
	otp_write(priv, row, OTP_ADDR_ROW);

	/* 4. Set OTP column address in ADDR_COL register.*/
	otp_write(priv, col, OTP_ADDR_COL);

	/* 5. Write 1 to DATA_IN bit of DATA_PRG register.*/
	otp_write(priv, data, OTP_DATA_PRG);

	/* 6. Set a variable NP1 = 0.*/
	for (np1 = 0; np1 < 3; np1++) {
		/* 7. Assert PRG_START register to start OTP program.*/
		otp_write(priv, OTP_PRG_ST_EN, OTP_PRG_START);

		/* 8. Polling the PRG_COMP bit of PRG_STATUS register. The program is
				completed until this bit become high.
				*/
		start = get_timer(0);
		while ((otp_read(priv,
			OTP_PRG_STATUS) & PRG_COMP) == 0){
			if (get_timer(start) > OTP_TIMEOUT)
			{
				ret = -ETIME;
				goto end;
			}

		}
		/* 9. Check the NP1 value, if NP1 < 3, then set NP1 = NP1 + 1, jump to step 7.*/
	}

	/* 10. If NP1 = 3, jump to step 11.*/
	/* 11. Check the PRG_FAIL bit of PRG_STATUS register.
	If PRG_FAIL = 0, then jump to step 3 to program next address.
	*/
	if ((otp_read(priv, OTP_PRG_STATUS) & PRG_FAIL) == 0) {
		ret = 0;
		goto end;
	}

	/* 12. Otherwise set a variable NP2 = 0, then jump to step 13.*/
	for (np2 = 0; np2 < 16; np2++) {
		/* 13. Assert PRG_START register to start OTP program.*/
		otp_write(priv, OTP_PRG_ST_EN, OTP_PRG_START);
		/* 14. Polling the PRG_COMP bit of PRG_STATUS register.
				The program is completed until this bit become high.
				*/
		start = get_timer(0);
		while ((otp_read(priv, OTP_PRG_STATUS) & PRG_COMP) == 0)
		{
			if (get_timer(start) > OTP_TIMEOUT)
			{
				ret = -ETIME;
				goto end;
			}
		}

		/* 15. Check the PRG_FAIL bit of PRG_STATUS register.
				If PRG_FAIL = 0, then jump to step 3 to program next address.
				*/
		if ((otp_read(priv,
			OTP_PRG_STATUS) & PRG_FAIL) == 0) {
			ret = 0;	
			goto end;
		}

		/* 16. Otherwise check the NP2 value, if NP2 < 16, then set NP2 = NP2 + 1,
				jump to step 13.
				*/
	}

	/* 17. If NP2 = 16, programming failed.*/
	ret = -EIO;
end:
#ifdef TAPEOUT_V2
	otp_write(priv, 0x1, OTP_PRG_FIN);
#endif	
	return ret;
}

static int otp_get_bit(struct brite_otp* priv, unsigned long row,
	unsigned char col, unsigned char *value)
{
	int ret = otp_access_request(priv);
	if(ret != 0)
		return ret;

	*value = (__raw_readb(priv->regs + row) >> col) & 0x1;	
	return 0;
}

static int otp_get_byte(struct brite_otp* priv, unsigned long row, unsigned char *value)
{
    int ret = otp_access_request(priv);
    if(ret != 0)
		return ret;

	*value = __raw_readb(priv->regs + row);
	return 0;
}

static int otp_init(struct brite_otp* priv)
{
	u32 auto_refresh_reg;
	u64 start = 0;

	memcpy(&priv->otp_timing,
		&default_otp_timing,
		sizeof(priv->otp_timing));
#ifndef TAPEOUT_V2
	while ((otp_read(priv,
		OTP_STATUS) & OTP_ST_READY_MASK) != OTP_ST_READY_MASK);

	if (otp_read(priv, OTP_STATUS) & OTP_ST_CORELOCK_MASK) {
		/* go standby, select */
		do {
			otp_write(priv, OTP_GO_STANDBY, OTP_LOW_POWER_EN);
		} while (!(otp_read(priv, OTP_LOW_POWER_EN) & OTP_ST_STANDBY));
		sw_udelay_10x(3);
		otp_write(priv, OTP_GO_SEL, OTP_LOW_POWER_EN);
		sw_udelay_10x(1);
	}
#else
	/* check the ready and auto download complete */
	start = get_timer(0);
	while ((otp_read(priv, OTP_STATUS) & OTP_ST_AUTODW_MASK) != OTP_ST_AUTODW_MASK) {
		if (get_timer(start) > OTP_TIMEOUT) {
			info("otp_init:auto download timeout OTP_STATUS:0x%x, refresh:0x%x\n", otp_read(priv, OTP_STATUS),
					otp_read(priv, OTP_REFRESH_CTRL_REG));
			return -ETIME;
		}
	}
	otp_sw_rest(priv);
	auto_refresh_reg = otp_read(priv, OTP_REFRESH_CTRL_REG);
	otp_write(priv, (auto_refresh_reg | REFRESH_EN), OTP_REFRESH_CTRL_REG);
	udelay(100);
#endif
	priv->state = STATE_READY;

	/* disable interrupt*/
	otp_write(priv, 0x0, OTP_INT_EN);
	otp_write(priv, 0x3, OTP_INT_MASK);
	otp_write(priv, 0x3, OTP_INT_CLR);

#ifndef TAPEOUT_V2
	/*disable refresh*/
	otp_write(priv, 0x0, OTP_REFRESH_CTRL_REG);
#else
	/*release request*/
	otp_write(priv, 0x1, OTP_PRG_FIN);
#endif
	return 0;
}

void otp_enable_int(struct brite_otp* priv, unsigned char type)
{
	unsigned long value = 0;
	value = otp_read(priv, OTP_INT_EN);

	if (type == INT_PRG)
		value |= (1 << 0);
	else if (type == INT_PRGERR)
		value |= (1 << 1);

	otp_write(priv, value, OTP_INT_EN);
}

void otp_disable_int(struct brite_otp* priv, unsigned char type)
{
	unsigned long value = 0;
	value = otp_read(priv, OTP_INT_EN);

	if (type == INT_PRG)
		value &= (~(1 << 0));
	else if (type == INT_PRGERR)
		value &= (~(1 << 1));

	otp_write(priv, value, OTP_INT_EN);
}

unsigned long otp_get_raw_int_status(struct brite_otp* priv)
{
	return otp_read(priv, OTP_INT_RAW);
}

unsigned long otp_get_int_status(struct brite_otp* priv)
{
	return otp_read(priv, OTP_INT_STATUS);
}

void otp_set_int_mask(struct brite_otp* priv, unsigned char mask,
	unsigned char val)
{
	unsigned long value = 0;
	value = otp_read(priv, OTP_INT_MASK);

	value &= ~mask;
	value |= val;

	otp_write(priv, value, OTP_INT_MASK);
}

void otp_clear_int(struct brite_otp* priv, unsigned char status)
{
	otp_write(priv, status, OTP_INT_CLR);
}

unsigned long otp_get_sys_status(struct brite_otp* priv)
{
	return otp_read(priv, OTP_SYSTEM_STATUS);
}

void otp_disable_key(struct brite_otp* priv, int id)
{
	u32 val = 1 << id;

	otp_write(priv, val, OTP_KEY_DISABLE);
}
int otp_get_key_status(struct brite_otp* priv, int id)
{
	u32 val = otp_read(priv, OTP_KEY_DISABLE);;

	return val & (1 << id);
}

const char *power_state_string(enum power_state state)
{
	static const char *const names[] = {
		[STATE_UNDEFINED] = "UNDEFINED",
		[STATE_READY] = "ready",
		[STATE_STANDBY] = "standby",
		[STATE_SLEEP] = "sleep",
	};

	if (state < 0 || state >= ARRAY_SIZE(names))
		return "UNKNOWN";

	return names[state];
}

int power_state_change(struct brite_otp* priv, enum power_state to)
{
	u32 auto_refresh_reg = 0;
	u32 low_power_en_reg = 0;
	u32 otp_status_reg = 0;
	u64 start = 0;

	if (priv->state != to) {
		switch (to) {
		case STATE_READY:
			if (priv->state == STATE_STANDBY) {
				/*1. polling RTSB_RDY */
				start = get_timer(0);
				do {
					low_power_en_reg = otp_read(priv, OTP_LOW_POWER_EN);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_LOW_POWER_EN:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_LOW_POWER_EN));
						return -ETIME;
					}
				} while (!(low_power_en_reg & OTP_ST_RTSB_RDY));

				/*2. set select to ready*/
				otp_write(priv, OTP_GO_SEL, OTP_LOW_POWER_EN);

				/*3. check low power register */
				start = get_timer(0);
				while (otp_read(priv, OTP_LOW_POWER_EN) != 0) {
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_LOW_POWER_EN:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_LOW_POWER_EN));
						return -ETIME;
					}
				}
				/*4 enable auto refresh */
				auto_refresh_reg = otp_read(priv, OTP_REFRESH_CTRL_REG);
				otp_write(priv, (auto_refresh_reg | 0x1), OTP_REFRESH_CTRL_REG);

				/*5. check the ready and auto download complete */
				start = get_timer(0);
				while ((otp_read(priv, OTP_STATUS) & OTP_ST_READY_MASK) != OTP_ST_READY_MASK) {
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_STATUS:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_STATUS));

						return -ETIME;
					}
				}
				info("change from %s to %s success\n",
						power_state_string(priv->state), power_state_string(to));
				priv->state = STATE_READY;
			} else {
				info("change from %s to %s unsupport\n",
						power_state_string(priv->state), power_state_string(to));
			}
			break;
		case STATE_STANDBY:
			if (priv->state == STATE_READY) {
				/*1. check the ready and auto download complete */
				info("OTP_STATUS:0x%x\n", otp_read(priv, OTP_STATUS));
				start = get_timer(0);
				while ((otp_read(priv, OTP_STATUS) & OTP_ST_READY_MASK) != OTP_ST_READY_MASK) {
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_STATUS:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_STATUS));

						return -ETIME;
					}
				}
				/*2. disable auto refresh */
				auto_refresh_reg = otp_read(priv, OTP_REFRESH_CTRL_REG);
				otp_write(priv, (auto_refresh_reg & ~REFRESH_EN), OTP_REFRESH_CTRL_REG);

				/*3. check low power register */
				if (otp_read(priv, OTP_LOW_POWER_EN) != 0) {
					info("change from %s to %s failed OTP_LOW_POWER_EN:0x%x\n",
							power_state_string(priv->state), power_state_string(to),
							otp_read(priv, OTP_LOW_POWER_EN));
					return -EAGAIN;
				}
				/*4. check the ready again */
				start = get_timer(0);
				while ((otp_read(priv, OTP_STATUS) & (1 << OTP_RDY_SHIFT)) != (1 << OTP_RDY_SHIFT)) {
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_STATUS:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_STATUS));

						return -ETIME;
					}

				}

				/*5. go standby */
				start = get_timer(0);
				do {
					otp_write(priv, OTP_GO_STANDBY, OTP_LOW_POWER_EN);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_LOW_POWER_EN:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_LOW_POWER_EN));
						return -ETIME;
					}
				} while (!(otp_read(priv, OTP_LOW_POWER_EN) & OTP_ST_STANDBY));
				/*6. check the request lock in standby*/
				start = get_timer(0);
				do
				{
					otp_write(priv, 0x1, OTP_REQ);
					otp_status_reg = otp_read(priv, OTP_STATUS);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("standy: request lock timeout. standby indeed\n");
						break;
					}
				} while((otp_status_reg&OTP_ST_LOCKED_MASK) != OTP_ST_LOCKED_MASK);

				info("change from %s to %s success\n",
						power_state_string(priv->state), power_state_string(to));
				priv->state = STATE_STANDBY;
			} else if (priv->state == STATE_SLEEP) {
				/*1. set wakeup to standby*/
				start = get_timer(0);
				do {
					otp_write(priv, OTP_GO_WAKEUP, OTP_LOW_POWER_EN);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_LOW_POWER_EN:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_LOW_POWER_EN));
						return -ETIME;
					}
				} while (!(otp_read(priv, OTP_LOW_POWER_EN) & OTP_ST_STANDBY));
				info("change from %s to %s success\n",
						power_state_string(priv->state), power_state_string(to));
				priv->state = STATE_STANDBY;
			} else {
				info("change from %s to %s unsupport\n",
						power_state_string(priv->state), power_state_string(to));
			}
			break;
		case STATE_SLEEP:
			if (priv->state == STATE_STANDBY) {
				/*1.  polling RTSB_RDY */
				start = get_timer(0);
				do {
					low_power_en_reg = otp_read(priv, OTP_LOW_POWER_EN);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_LOW_POWER_EN:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_LOW_POWER_EN));
						return -ETIME;
					}
				} while (!(low_power_en_reg & OTP_ST_RTSB_RDY));
				/*2. go sleep */
				start = get_timer(0);
				do {
					otp_write(priv, OTP_GO_SLEEP, OTP_LOW_POWER_EN);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("change from %s to %s timeout OTP_LOW_POWER_EN:0x%x\n",
								power_state_string(priv->state), power_state_string(to),
								otp_read(priv, OTP_LOW_POWER_EN));
						return -ETIME;
					}
				} while (!(otp_read(priv, OTP_LOW_POWER_EN) & OTP_ST_SLEEP));

				/*3. check the request lock in sleep*/
				start = get_timer(0);
				do
				{
					otp_write(priv, 0x1, OTP_REQ);
					otp_status_reg = otp_read(priv, OTP_STATUS);
					if (get_timer(start) > OTP_TIMEOUT) {
						info("sleep: request lock timeout. sleep indeed\n");
						break;
					}
				} while((otp_status_reg&OTP_ST_LOCKED_MASK) != OTP_ST_LOCKED_MASK);
				info("change from %s to %s success\n",
						power_state_string(priv->state), power_state_string(to));
				priv->state = STATE_SLEEP;
			} else {
				info("change from %s to %s unsupport\n",
						power_state_string(priv->state), power_state_string(to));
			}
			break;
		default:
			info("change from %s to %s unsupport\n",
					power_state_string(priv->state), power_state_string(to));
			break;
		}
	} else {
		info("change from %s to %s omit\n",
				power_state_string(priv->state), power_state_string(to));
	}
	return 0;
}

/*api wrapper for access lock or auto download failed*/
int otp_init_wrapper(struct brite_otp* priv)
{
	int retry_times = RETRY_TIMES;
	int ret = 0;

	do {
		ret = otp_init(priv);
		if (ret != 0) {
			debug("otp_init_wrapper result fail %d @times:%d\r\n", ret, retry_times);
			mdelay(50);
		}
	} while ((ret != 0) && (--retry_times > 0));
	return ret;
}

int otp_prog_bit_wrapper(struct brite_otp* priv, unsigned long row,
	unsigned char col, unsigned char data)
{
	int retry_times = RETRY_TIMES;
	int ret = 0;

	do {
		ret = otp_prog_bit(priv, row, col, data);
		if (ret != 0) {
			debug("otp_prog_bit_wrapper 0x%x result fail %d @times:%d\r\n", row, ret, retry_times);
			mdelay(50);
		}
	} while ((ret != 0) && (--retry_times > 0));
	return ret;
}

int otp_get_bit_wrapper(struct brite_otp* priv, unsigned long row,
	unsigned char col, unsigned char *value)
{
	int retry_times = RETRY_TIMES;
	int ret = 0;

	do {
		ret = otp_get_bit(priv, row, col, value);
		if (ret != 0) {
			debug("otp_get_bit_wrapper 0x%x result fail %d @times:%d\r\n", row, ret, retry_times);
			mdelay(50);
		}
	} while ((ret != 0) && (--retry_times > 0));
	return ret;

}

int otp_get_byte_wrapper(struct brite_otp* priv, unsigned long row, unsigned char *value)
{
	int retry_times = RETRY_TIMES;
	int ret = 0;

	do {
		ret = otp_get_byte(priv, row, value);
		if (ret != 0) {
			debug("otp_get_byte_wrapper 0x%x result fail %d @times:%d\r\n", row, ret, retry_times);
			mdelay(50);
		}
	} while ((ret != 0) && (--retry_times > 0));
	return ret;
}

int otp_prog(struct brite_otp* priv, unsigned long row,
	unsigned char* p_data, unsigned long size)
{
	int ret_val = 0;
	u32 i = 0;
	u8 col = 0;

	for (i = 0; i < size; i++) {
		for (col = 0; col < 8; col++) {
			if ((p_data[i] >> col) & 1) {
				ret_val = otp_prog_bit_wrapper(priv, (row + i), col, 1);
				if (ret_val != 0) {
					return ret_val;
				}
			}
		}
	}

	return 0;
}

int otp_prog_byte(struct brite_otp* priv, unsigned long row,
	unsigned char data)
{
	int fail = 0;
	int i = 0;
	u8 read_val = 0;

	if (row >= OTP_NON_SEC_BASE_ROW)/*non-secure*/{
		if(0 != otp_get_byte_wrapper(priv, (OTP_NON_SECURE + row- OTP_NON_SEC_BASE_ROW), &read_val))
			fail++;
	}
	else {
		if(0 != otp_get_byte_wrapper(priv, (OTP_SECURE + row), &read_val))
			fail++;
	}
#ifndef TAPEOUT_V2
	// force read_val as 0 if it's key 2/5/6 in ARM side
	if(((row >=  0x40) && (row<= 0x6F)) || ((row >=  0xd0) && (row<= 0x10F)))
		read_val = 0;	
#else
	// force read_val as 0 if it's key 2/4/5/6 in ARM side
	if(((row >=  0x40) && (row<= 0x6F)) || ((row >=  0xa0) && (row<= 0x10F)))
		read_val = 0;	
#endif
	/*check data with current otp value: 1->1 skip 0->1 accept 1->0 reject*/
	for (i=0;i<8;i++) {
		if (((read_val >> i) & 1) && !((data >> i) & 1))
			fail++;
	}
	if (fail)
		return -EIO;

	/*program bit by bit*/
	for (i=0;i<8;i++) {
		/*1->1 skip*/
		//if (((read_val >> i) & 1) && ((data >> i) & 1))
		//	continue;
		/*0->0 skip,0->1 accept and program*/
		if (((data >> i) & 1)) {
			fail += otp_prog_bit_wrapper(priv,row,i,1);
		}
	}
	if (fail)
		return -EIO;
	return 0;
}
