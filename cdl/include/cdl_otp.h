/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_OTP_H__
#define __CDL_OTP_H__

/*register offset*/
#define OTP_DATA_PRG          (0x00)
#define OTP_ADDR_COL          (0x04)
#define OTP_ADDR_ROW          (0x08)
#define OTP_PRG_EN            (0x0C)
#define OTP_PRG_START         (0x10)
#define OTP_PRG_STATUS        (0x14)
#define OTP_SYSTEM_STATUS     (0x18)
#define OTP_SOFTRST           (0x1C)
#define OTP_STATUS            (0x20)
#define OTP_KEY_DISABLE       (0x24)
#define OTP_REFRESH_CTRL_REG  (0x28)
#define OTP_LOW_POWER_EN      (0x2C)
#define OTP_INT_EN            (0x30)
#define OTP_INT_RAW           (0x34)
#define OTP_INT_MASK          (0x38)
#define OTP_INT_STATUS        (0x3C)
#define OTP_INT_CLR           (0x40)
#ifdef TAPEOUT_V2
#define OTP_SW_AUX            (0x48)
#define OTP_REQ               (0x4C)
#define OTP_PRG_FIN           (0x50)
#define SECURE_BOOT_EN_REG    (0x54)
#endif
#define OTP_RST_PARAM         (0x80)
#define OTP_INIT_PARAM        (0x84)
#define OTP_RD_PARAM          (0x88)
#define OTP_PRG_PARAM0        (0x8C)
#define OTP_PRG_PARAM1        (0x90)
#define OTP_PRG_PARAM2        (0x94)
#define OTP_PRG_PARAM3        (0x98)

#define OTP_SECURE            (0x1000)
#define OTP_NON_SECURE        (0x2000)

/*offset: 0x0C, program enable register*/
#define PRG_START_EN_FLAG     (0x5a5a55aa)

/*offset: 0x10, Program Start Register*/
#define OTP_PRG_ST_EN         (1<<0)

/*offset: 0x14, program status register*/
#define PRG_COMP              (1 << 0)
#define PRG_FAIL              (1 << 1)

/*offset: 0x18, System Status Register*/
#define OTP_PROG_DIS          (1 << 0) /* OTP program: 0 enable; 1 disable*/
#define OTP_UART0_DW_DIS      (1 << 1) /* UART0 download: 0 enable; 1 disable*/
#define OTP_USB0_DW_DIS       (1 << 2) /* USB0 download: 0 enable; 1 disable*/
#define OTP_SPI0_BOOT_DIS     (1 << 3) /* SPI0  boot: 0 enable; 1 disable*/
#define OTP_NFC_BOOT_DIS      (1 << 4) /* NFC  boot: 0 enable; 1 disable*/
#ifndef TAPEOUT_V2
#define SECURE_BOOT_EN_BIT    (1 << 5)
#else
#define NON_SECURE_BOOT_EN    (0x55aa5a5a)
#endif

/* offset: 0x20, OTP Status Register*/
typedef enum {
	OTP_RDY_SHIFT = 0, /* 0:  indicates the OTP isn't ready*/
	OTP_COMP_AUTO_DW_SHIFT = 1, /* 0: indicates auto download after power on isn't completed*/
	OTP_CORELOCK_SHIFT = 2,
	OTP_LOCKED_SHIFT = 3
} E_OTP_ST_SHIFT;

#define OTP_ST_READY_MASK       ((1 << OTP_COMP_AUTO_DW_SHIFT) | (1 << OTP_RDY_SHIFT))
#define OTP_ST_AUTODW_MASK      (1 << OTP_COMP_AUTO_DW_SHIFT)
#define OTP_ST_CORELOCK_MASK    (1 << OTP_CORELOCK_SHIFT)
#define OTP_ST_LOCKED_MASK      (1 << OTP_LOCKED_SHIFT)

/* offset: 0x24, Key Disable Register*/
typedef enum {
	/*write 0 is ignored. Only clear this bit by reset
	Write 1 to this bit will disable read the key0-13 from the secure RAM
	*/
	OTP_KEY0_DIS_SHIFT = 0,
	OTP_KEY1_DIS_SHIFT = 1,
	OTP_KEY2_DIS_SHIFT = 2,
	OTP_KEY3_DIS_SHIFT = 3,
	OTP_KEY4_DIS_SHIFT = 4,
	OTP_KEY5_DIS_SHIFT = 5,
	OTP_KEY6_DIS_SHIFT = 6,
	OTP_KEY7_DIS_SHIFT = 7,
	OTP_KEY8_DIS_SHIFT = 8,
	OTP_KEY9_DIS_SHIFT = 9,
	OTP_KEY10_DIS_SHIFT = 10,
	OTP_KEY11_DIS_SHIFT = 11,
	OTP_KEY12_DIS_SHIFT = 12,
	OTP_KEY13_DIS_SHIFT = 13
} E_OTP_KEY_DIS_SHIFT;

/* offset: 0x28, Refresh Control Register*/
#define REFRESH_EN               (1 << 0)

/* offset: 0x2C, OTP Low Power Enable Register*/
typedef enum {
	OTP_GO_WAKEUP_SHIFT = 0,
	OTP_GO_SLEEP_SHIFT = 1,
	OTP_GO_SEL_SHIFT = 2,
	OTP_GO_STANDBY_SHIFT = 3,
	OTP_ST_STANDBY_SHIFT = 4,
	OTP_ST_SLEEP_SHIFT = 5,
	OTP_ST_RTSB_RDY_SHIFT = 6
} E_OTP_LOW_POWR_EN_SHIFT;

#define OTP_GO_WAKEUP            (1UL<<OTP_GO_WAKEUP_SHIFT)
#define OTP_GO_SLEEP             (1UL<<OTP_GO_SLEEP_SHIFT)
#define OTP_GO_SEL               (1UL<<OTP_GO_SEL_SHIFT)
#define OTP_GO_STANDBY           (1UL<<OTP_GO_STANDBY_SHIFT)
#define OTP_ST_STANDBY           (1UL<<OTP_ST_STANDBY_SHIFT)
#define OTP_ST_SLEEP             (1UL << OTP_ST_SLEEP_SHIFT)
#define OTP_ST_RTSB_RDY          (1UL << OTP_ST_RTSB_RDY_SHIFT)

/*offset: 0x30-0x40, Interrupt Registers*/
#define INT_PRG                  (1 << 0)
#define INT_PRGERR               (1 << 1)

/* Non-Secure OTP Organization*/

/* byte 0*/
typedef enum {
	OTP_TEST_DIS_BIT = 0,
	OTP_PROG_DIS_BIT = 1,
	OTP_JTAG_DIS_BIT = 2,
	OTP_UART0_DL_DIS_BIT = 3,
	OTP_USB0_DL_DIS_BIT = 4,
	OTP_SPI0_BOOT_DIS_BIT = 5,
	OTP_NFC_BOOT_DIS_BIT = 6,
	OTP_SECU_BOOT_DIS_BIT = 7
}E_OTP_NON_SECU_BYTE0;

/* byte 1*/
typedef enum {
	OTP_UART0_IF_DIS_BIT = 0,
	OTP_UART1_IF_DIS_BIT = 1,
	OTP_UART2_IF_DIS_BIT = 2,
	OTP_UART3_IF_DIS_BIT = 3,
	OTP_SPI0_IF_DIS_BIT = 4,
	OTP_SPI1_BOOT_DIS_BIT = 5,
	OTP_SPI2_BOOT_DIS_BIT = 6,
	OTP_I2C0_IF_DIS_BIT = 7
}E_OTP_NON_SECU_BYTE1;

/* byte 2*/
typedef enum {
	OTP_I2C1_IF_DIS_BIT = 0,
	OTP_I2C2_IF_DIS_BIT = 1,
	OTP_USB0_IF_DIS_BIT = 2,
	OTP_USB1_IF_DIS_BIT = 3,
	OTP_SDIO0_IF_DIS_BIT = 4,
	OTP_SDIO1_IF_DIS_BIT = 5,
	OTP_REVD0_DIS_BIT = 6,
	OTP_REVD1_DIS_BIT = 7
}E_OTP_NON_SECU_BYTE2;

/* byte 3*/
typedef enum {
	OTP_TRIM0_DIS_BIT = 0,
	OTP_TRIM1_DIS_BIT = 1,
	OTP_TRIM2_DIS_BIT = 2,
	OTP_TRIM3_DIS_BIT = 3,
	OTP_TRIM4_DIS_BIT = 4,
	OTP_TRIM5_DIS_BIT = 5,
	OTP_TRIM6_DIS_BIT = 6,
	OTP_TRIM7_DIS_BIT = 7
}E_OTP_NON_SECU_BYTE3;

/*BYTE0~BYTE3 back up copy offset*/
#define BACKUP_REGION_OFFSET   0x100

/* OTP security structure*/
typedef struct {
	u8 itron_key[16];			/* key number 0,  Itron Key*/
	u8 itron_root_cer[48];		/*key number 1,  Itron Root Certificate*/
	u8 itron_code_sign_root_cer[48];	/* key number 2,  Itron Code Signing Root Certificate*/
	u8 itron_soc_cer[48];		/*key number 3,  Itron SoC Certificate*/
	u8 itron_soc_cer_private_key[48];	/* key number 4,  Itron SoC Certificate Private Key*/
	u8 itron_fw_dec_sym_key[32];	/* key number 5,  Itron Firmware Decryption Symmetric Key*/
	u8 itron_hw_sym_key[32];		/* key number 6,  Unique Restricted Processor Hardware Symmetric Key*/
	u8 itron_sec_key1[32];		/* key number 7,  Itron Security Key 1*/
	u8 itron_sec_key2[32];		/* key number 8,  Itron Security Key 2*/
	u8 itron_sec_key3[32];		/* key number 9,  Itron Security Key 3*/
	u8 itron_sec_key4[32];		/* key number 10, Itron Security Key 4*/
	u8 itron_sec_key5[32];		/* key number 11, Itron Security Key 5*/
	u8 itron_sec_key6[32];		/* key number 12, Itron Security Key 6*/
	u8 itron_sec_key7[32];		/* key number 13, Itron Security Key 7*/
	u8 itron_revd[16];			/* reserved for future use*/
} otp_secu_t;

/*OTP security and no-security base row layout*/
typedef enum {
	OTP_SEC_BASE_ROW = 0x000,
	OTP_NON_SEC_BASE_ROW = 0x200
} E_OTP_ROW;

/* OTP non-security disable status(OTP default is 0)*/
typedef enum {
	OTP_ST_EN = 0,
	OTP_ST_DIS = 1
} E_OTP_ST_EN;

/*OTP Power states*/
enum power_state {
	STATE_UNDEFINED = 0,
	STATE_READY,   /*normal operation*/
	STATE_STANDBY, /*standy mode*/
	STATE_SLEEP, /*sleep mode*/
};

struct brite_otp_timing {
	unsigned long rst_param;
	unsigned long init_param;
	unsigned long rd_param;
	unsigned long prg_param0;
	unsigned long prg_param1;
	unsigned long prg_param2;
	unsigned long prg_param3;
};

struct brite_otp {
	void* regs;
	int int_vector;
	struct brite_otp_timing otp_timing;
	enum power_state state;
};

int otp_init_wrapper(struct brite_otp* priv);

int otp_prog_bit_wrapper(struct brite_otp* priv, unsigned long row,
	unsigned char col, unsigned char data);

int otp_get_bit_wrapper(struct brite_otp* priv, unsigned long row,
	unsigned char col, unsigned char *value);

int otp_get_byte_wrapper(struct brite_otp* priv, unsigned long row, unsigned char *value);

int otp_prog(struct brite_otp* priv, unsigned long row,
	unsigned char* p_data, unsigned long size);

int otp_prog_byte(struct brite_otp* priv, unsigned long row,
	unsigned char data);

void otp_enable_int(struct brite_otp* priv, unsigned char type);
void otp_disable_int(struct brite_otp* priv, unsigned char type);
unsigned long otp_get_raw_int_status(struct brite_otp* priv);
unsigned long otp_get_int_status(struct brite_otp* priv);
void otp_set_int_mask(struct brite_otp* priv, unsigned char mask,
	unsigned char val);
void otp_clear_int(struct brite_otp* priv, unsigned char status);
unsigned long otp_get_sys_status(struct brite_otp* priv);
void otp_disable_key(struct brite_otp* priv, int id);
int otp_get_key_status(struct brite_otp* priv, int id);
int power_state_change(struct brite_otp* priv, enum power_state to);

#endif

