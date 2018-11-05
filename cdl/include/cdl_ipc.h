/*------------------------------------------------------------------------------------------------*/
/*                                                                                                */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                */
/*                                                                                                */
/*------------------------------------------------------------------------------------------------*/


#ifndef __CDL_IPC_H__
#define __CDL_IPC_H__

#define A7_AHB_BASE		0x79300000
#define A7_SRAM_BASE		0x79600000

#define RFDSP_AHB_BASE		0x72400000
#define RFDSP_SRAM_BASE	0x72600000

#define PLCDSP_AHB_BASE	0x61500000
#define PLCDSP_SRAM_BASE	0x61600000

//arm
//message regs
#define A7TORFIPCCOMM		0x00
#define A7TORFIPCADDR		0x04
#define A7TORFIPCDATA0		0x08
#define A7TORFIPCDATA1		0x0C
#define A7TOPLCIPCCOMM		0x10
#define A7TOPLCIPCADDR		0x14
#define A7TOPLCIPCDATA0	0x18
#define A7TOPLCIPCDATA1	0x1C

#define RFTOA7IPCCOMM		0x20
#define RFTOA7IPCADDR		0x24
#define RFTOA7IPCDATA0		0x28
#define RFTOA7IPCDATA1		0x2C
#define RFTOPLCIPCCOMM		0x30
#define RFTOPLCIPCADDR		0x34
#define RFTOPLCIPCDATA0	0x38
#define RFTOPLCIPCDATA1	0x3C

#define PLCTOA7IPCCOMM		0x40
#define PLCTOA7IPCADDR		0x44
#define PLCTOA7IPCDATA0	0x48
#define PLCTOA7IPCDATA1	0x4C
#define PLCTORFIPCCOMM		0x50
#define PLCTORFIPCADDR		0x54
#define PLCTORFIPCDATA0	0x58
#define PLCTORFIPCDATA1	0x5C

// free timer
#define IPCTMRSCALER		0x80 	// Free running 64bit timestamp counter prescaler register.
#define IPCCOUNTERL			0x84	// Free running 64bit timestamp counter low register.
#define IPCCOUNTERH			0x88	// Free running 64bit timestamp counter high register.
#define IPCTMRCONT			0x8C	// Free running 64bit timestamp counter high register.
#define SRMSEL0				0x90
#define SRMSEL1				0x94

//event regs
#define A7TORFIPCSET		0x100
#define A7TORFIPCCLR		0x104
#define A7TORFIPCFLG		0x108
#define RFTOA7IPCACK		0x10C
#define RFTOA7IPCSTS		0x110
#define A7TOPLCIPCSET		0x114
#define A7TOPLCIPCCLR		0x118
#define A7TOPLCIPCFLG		0x11C
#define PLCTOA7IPCACK		0x120
#define PLCTOA7IPCSTS		0x124

#define A7SRP00REQ			0x200
#define A7SRP01REQ			0x204
#define A7SRP02REQ			0x208
#define A7SRP03REQ			0x20C
#define A7SRP04REQ			0x210
#define A7SRP05REQ			0x214
#define A7SRP06REQ			0x218
#define A7SRP07REQ			0x21C
#define A7SRP08REQ			0x220
#define A7SRP09REQ			0x224
#define A7SRP10REQ			0x228
#define A7SRP11REQ			0x22C
#define A7SRP12REQ			0x230
#define A7SRP13REQ			0x234
#define A7SRP14REQ			0x238
#define A7SRP15REQ			0x23C
#define A7SRP16REQ			0x240
#define A7SRP17REQ			0x244
#define A7SRP18REQ			0x248
#define A7SRP19REQ			0x24C
#define A7SRP20REQ			0x250
#define A7SRP21REQ			0x254
#define A7SRP22REQ			0x258
#define A7SRP23REQ			0x25C
#define A7SRP24REQ			0x260
#define A7SRP25REQ			0x264
#define A7SRP26REQ			0x268
#define A7SRP27REQ			0x26C
#define A7SRP28REQ			0x270
#define A7SRP29REQ			0x274
#define A7SRP30REQ			0x278
#define A7SRP31REQ			0x27C

//rf
#define RFTOA7IPCSET	0x300
#define RFTOA7IPCCLR	0x304
#define RFTOA7IPCFLG	0x308
#define A7TORFIPCACK	0x30C
#define A7TORFIPCSTS	0x310
#define RFTOPLCIPCSET	0x314
#define RFTOPLCIPCCLR	0x318
#define RFTOPLCIPCFLG	0x31C
#define PLCTORFIPCACK	0x320
#define PLCTORFIPCSTS	0x324

#define RFSRP00REQ		0x400
#define RFSRP01REQ		0x404
#define RFSRP02REQ		0x408
#define RFSRP03REQ		0x40C
#define RFSRP04REQ		0x410
#define RFSRP05REQ		0x414
#define RFSRP06REQ		0x418
#define RFSRP07REQ		0x41C
#define RFSRP08REQ		0x420
#define RFSRP09REQ		0x424
#define RFSRP10REQ		0x428
#define RFSRP11REQ		0x42C
#define RFSRP12REQ		0x430
#define RFSRP13REQ		0x434
#define RFSRP14REQ		0x438
#define RFSRP15REQ		0x43C
#define RFSRP16REQ		0x440
#define RFSRP17REQ		0x444
#define RFSRP18REQ		0x448
#define RFSRP19REQ		0x44C
#define RFSRP20REQ		0x450
#define RFSRP21REQ		0x454
#define RFSRP22REQ		0x458
#define RFSRP23REQ		0x45C
#define RFSRP24REQ		0x460
#define RFSRP25REQ		0x464
#define RFSRP26REQ		0x468
#define RFSRP27REQ		0x46C
#define RFSRP28REQ		0x470
#define RFSRP29REQ		0x474
#define RFSRP30REQ		0x478
#define RFSRP31REQ		0x47C
//plc
#define PLCTOA7IPCSET	0x500
#define PLCTOA7IPCCLR	0x504
#define PLCTOA7IPCFLG	0x508
#define A7TOPLCIPCACK	0x50C
#define A7TOPLCIPCSTS	0x510
#define PLCTORFIPCSET	0x514
#define PLCTORFIPCCLR	0x518
#define PLCTORFIPCFLG	0x51C
#define RFTOPLCIPCACK	0x520
#define RFTOPLCIPCSTS	0x524

#define PLCSRP00REQ		0x600
#define PLCSRP01REQ		0x604
#define PLCSRP02REQ		0x608
#define PLCSRP03REQ		0x60C
#define PLCSRP04REQ		0x610
#define PLCSRP05REQ		0x614
#define PLCSRP06REQ		0x618
#define PLCSRP07REQ		0x61C
#define PLCSRP08REQ		0x620
#define PLCSRP09REQ		0x624
#define PLCSRP10REQ		0x628
#define PLCSRP11REQ		0x62C
#define PLCSRP12REQ		0x630
#define PLCSRP13REQ		0x634
#define PLCSRP14REQ		0x638
#define PLCSRP15REQ		0x63C
#define PLCSRP16REQ		0x640
#define PLCSRP17REQ		0x644
#define PLCSRP18REQ		0x648
#define PLCSRP19REQ		0x64C
#define PLCSRP20REQ		0x650
#define PLCSRP21REQ		0x654
#define PLCSRP22REQ		0x658
#define PLCSRP23REQ		0x65C
#define PLCSRP24REQ		0x660
#define PLCSRP25REQ		0x664
#define PLCSRP26REQ		0x668
#define PLCSRP27REQ		0x66C
#define PLCSRP28REQ		0x670
#define PLCSRP29REQ		0x674
#define PLCSRP30REQ		0x678
#define PLCSRP31REQ		0x67C


//command
#define IPC_DATA_READ		0x0000007
#define A7_REQ_KEY			0xBE97A3D
#define RFDSP_REQ_KEY		0x3589BCD
#define PLCDSP_REQ_KEY		0x58AF6C1
#define IPC_END_MSG 		0x55aa
#define IPC_PAGE_LEN		(1024)

// offset: 0x0080, Free running 64bit timestamp counter prescaler register.
typedef enum
{
	IPC_TMR_PRESCALER_SHIFT  	= 0,	// bit[15:0], The prescaler value used to derive the free running timer's clock: Ftimer=Fcpu/(prescaler+1). 							
										// 			Prescale counter is reset whenever this setting or IPCTMRCNT is changed
										// 			It can be only configured by RF Core
										
	IPC_TMR_REVD_SHIFT 			= 16,	// bit[31:16], Reserved
} E_IPC_TMRSCALER_SHIFT;

#define IPC_TMR_PRESCALER_MASK		(0xFFFF<<IPC_TMR_PRESCALER_SHIFT)
#define IPC_TMR_PRESCALER_VAL(val)	((val)<<IPC_TMR_PRESCALER_SHIFT)

#define IPC_TMR_COUNTUP_MAX			0xFFFF


// offset: 0x008C, Free running 64bit timestamp counter high register.
typedef enum
{
	IPC_TMR_COUNTUP_SHIFT  		= 0,	// bit[0], timer count mode
										// 0x1: Timer as upward counter
										// 0x0: Timer as downward counter
										// It can be only configured by RF Core
										// Timer count value is reset to 0x0 (count up) or 0xffffffff_ffffffff (count down) whenever TMRCOUNTUP setting is changed
																		
	IPC_TMR_COUNT_REVD_SHIFT 	= 1,	// bit[31:1], Reserved
} E_IPC_TMRCOUNT_SHIFT;

#define IPC_TMR_COUNTUP_MASK		(0x1<<IPC_TMR_COUNTUP_SHIFT)
#define IPC_TMR_COUNTUP_VAL(val)	((val)<<IPC_TMR_COUNTUP_SHIFT)

// Timer counting mode
typedef enum
{
	IPC_TMR_DOWN 	= 0,
	IPC_TMR_UP 		= 1
} E_IPC_TMR_COUNT_MODE;

struct a7_ipc {
	void* reg_base;
	void* ram_base;
	int   int_rf[8];
	int   int_plc[8];
};

/*apis*/
void ipc_writel(struct a7_ipc *priv, unsigned long val,int reg);
unsigned long ipc_readl(struct a7_ipc *priv,int reg);

int a7_ipc_get_int_status_rf(struct a7_ipc *priv);
int a7_ipc_get_int_status_plc(struct a7_ipc *priv);
int a7_ipc_clear_rf_int(struct a7_ipc *priv, int flag);
int a7_ipc_clear_plc_int(struct a7_ipc *priv, int flag);
void ipc_clear_plctoa7_flag(struct a7_ipc *priv, int flag);
void ipc_clear_rftoa7_flag(struct a7_ipc *priv, int flag);

int ipc_set_msg_a7torf(struct a7_ipc *priv, int flag, u32* data, int len);
int ipc_set_msg_a7toplc(struct a7_ipc *priv, int flag, u32* data, int len);
int ipc_get_msg_rftoa7(struct a7_ipc *priv, int flag, u32 * data, int *lens);
int ipc_get_msg_plctoa7(struct a7_ipc *priv, int flag, u32 * data, int *lens);

int a7_get_sram_page(struct a7_ipc *priv, int page);
int a7_free_page(struct a7_ipc *priv, int page);
#ifdef TAPEOUT_V2
E_IPC_TMR_COUNT_MODE ipc_get_time_mode(struct a7_ipc *priv);
uint32_t ipc_get_timer_prescaler(struct a7_ipc *priv);
void ipc_set_time_mode(struct a7_ipc *priv, E_IPC_TMR_COUNT_MODE mode);
void ipc_set_timer_prescaler(struct a7_ipc *priv, uint32_t prescaler);
#endif
unsigned long long ipc_get_time(struct a7_ipc *priv);

#endif
