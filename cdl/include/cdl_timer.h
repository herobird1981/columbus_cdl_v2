/*-----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __CDL_TIMER_H__
#define __CDL_TIMER_H__

// regs offset
#define TMR_REG_TRIG	(0x0)	// Timer load trigger
#define TMR_REG_LOAD	(0x4)	// Timer load value
#define TMR_REG_COUNT	(0x8)	// Timer current value
#define TMR_REG_CAP1	(0xC)	// Timer captured value
#define TMR_REG_CAP2	(0x10)	// imer captured value
#define TMR_REG_COMPARE	(0x14)	// Timer match value
#define TMR_REG_CTRL	(0x18)	// Timer control register
#define TMR_REG_IRQ_ST	(0x1C)	// Timer interrupt request status
#define TMR_REG_IRQ_RAW	(0x20)	// Timer interrupt request raw status
#define TMR_REG_IRQ_EN	(0x24)	// Timer interrupt request enable

// offset: 0x0018, Timer control register
typedef enum
{
	
	TMR_EN_SHIFT 		= 0, 	// Bit[0]: 
								// Timer Enable
								// 0x1: Timer Enable 
								// 0x0: Timer disable, clock counter frozen, all
								// 	 Interrupt flag cleared. Capature state and prescaler counter reset
								// 	 Clock counter can be overwritten by APB bus write or load trigger.

#ifdef TAPEOUT_V2
	TMR_CLK_DIV_SHIFT	= 17,
#else
	TMR_CLK_DIV_SHIFT 	= 1, 	// Bit[4: 1]: Divisor for clock pre-scaler
								// 0x0:Input clock
								// 0x1:Input clock divided by 2
								// 0x2:Input clock divided by 4
								// 0x3:Input clock divided by 8
								// 0x4:Input clock divided by 16
								// 0x5:Input clock divided by 32
								// 0x6:Input clock divided by 64
								// 0x7:Input clock divided by 128
								// 0x8:Input clock divided by 256
								// Pre-scaled clock is used to clock time counter
#endif

	TMR_CAP_EVENT_SHIFT = 5,	// Bit[6:5]: Capture event setting
								// 0x0: Capture mode disabled
								// 0x1: Capture on  high-to-low transition 
								// 0x2: Capture on  low-to-high transition
								// 0x3: Capture on both transition edge
	

	TMR_CAP_MODE_SHIFT 	= 7,	// Bit[7]: caputure mode
								// 0x0: Single capture on the first valid event
								// 0x1: Capture on the second valid event
	

	TMR_PULSE_TGL_SHIFT = 8,	// Bit[8]: Output mode on PWM output pin
								// 0x0: Pulse 
								// 0x1: Toggle
	

	TMR_PWM_TRG_SHIFT 	= 9,	// Bit[10: 9]: Trigger output mode on PWM output pin
								// 0x0: No trigger
								// 0x1: Trigger on overflow only
								// 0x2: Trigger on overflow and Match
								// 0x3: Trigger on Match only
	

	TMR_MODE_SHIFT 		= 11,	// Bit[11]: timer mode
								// 0x0: Free-running mode
								// 0x1: User define mode
	

	TMR_AUTO_LD_SHIFT 	= 12,	// Bit[12]: Timer counter mode:
								// 0x0: One-shot, timer stops on overflow.
								// 	Timer can be restarted by reloading.
								// 0x1: Auto-reload
								// It applies to user define mode only (TMR_MODE = 0x1)
	

	TMR_COMP_EN_SHIFT 	= 13,	// Bit[13]: Compare mode enable
								// 0x0: Disable
								// 0x1: Enable
	
	TMR_UP_DWN_SHIFT 	= 14,	// Bit[14]: Timer counting mode
								// 0x0: Count Up, intrerrupt occurs upon counting down to 0xFFFFFFFF 
								// 0x1: Count Down, intrerrupt occurs upon counting down to 0
	
	TMR_SRST_SHIFT 		= 15,	// Bit[15]: Software reset to reset all functional circuit.
								// It also resets all system registers.
								// Read always returns 0
								// Write 0x1: Synchronous reset active
								// Write 0x0 : Synchronous reset inactive

	PWM_POL_SHIFT 		= 16,	// Bit[16]: Polarity selection of pulse on PWM port
								// 0x0: Positive
								// 0x1: Negative
								// Valid when TMR_PULSE_TGL = 0x0 only
	
	TMR_REVD_SHIFT 		= 17	// Bit[31:17]: Reserved
}E_TMR_REG_CTRL_SHIFT;

// bit[16], Polarity selection of pulse on PWM port
typedef enum
{
	TMR_PWM_POL_POS = 0,
	TMR_PWM_POL_NEG = 1
} E_TMR_PWM_POL;

// bit [15], Software reset to reset all functional circuit.
// It also resets all system registers.
typedef enum
{
	TMR_RST_DIS = 0,
	TMR_RST_EN 	= 1
} E_TMR_SRST;

// bit[14], Timer counting mode
typedef enum
{
	TMR_UP 		= 0,
	TMR_DOWN 	= 1,
	TMR_FIXED 	= 2
} E_TMR_COUNT_MODE;

// bit[13], Compare mode enable
typedef enum
{
	TMR_COMP_DIS 	= 0,
	TMR_COMP_EN 	= 1
} E_TMR_COMP_EN_CTRL;

// bit[12], Timer counter mode
typedef enum
{
	TMR_ONE_SHOT 	= 0,
	TMR_AUTO 		= 1
} E_TMR_AUTO_LD;

// bit[11], Timer mode
typedef enum
{
	TMR_FREE = 0,
	TMR_USER = 1
} E_TMR_MODE;

// bit[10:9], Trigger output mode on PWM output pin
typedef enum
{
	PWM_DIS			= 0,	// No trigger
	PWM_OV			= 1,	// Trigger on overflow only
	PWM_OV_MATCH	= 2,	// Trigger on overflow and Match
	PWM_MATCH		= 3		// Trigger on Match only
} E_TMR_PWM_MODE;

// bit[8], Output mode on PWM output pin
typedef enum
{
	PWM_PULSE	= 0,
	PWM_TOGGLE	= 1
} E_TMR_PWM_OUTPUT_MODE;

// bit[7], capture mode
typedef enum
{
	CAP_MODE_FIRST	= 0,
	CAP_MODE_SECOND	= 1
} E_TMR_CAP_MODE;

// bit[6], Capture event setting
typedef enum
{
	CAP_MODE_DIS	= 0,	// Capture mode disabled
	CAP_FALLING		= 1,	// Capture on high-to-low transition
	CAP_RISING		= 2,	// Capture on low-to-high transition
	CAP_BOTH		= 3		// Capture on both transition edge
} E_TMR_CAP_EVENT;

#ifdef TAPEOUT_V2
// bit[27:17], Divisor for clock pre-scaler
typedef enum
{
	TMR_DIV_1	= 0,
	TMR_DIV_2	= 1,
	TMR_DIV_4	= 3,
	TMR_DIV_8	= 7,
	TMR_DIV_16	= 15,
	TMR_DIV_32	= 31,
	TMR_DIV_64	= 63,
	TMR_DIV_128	= 127,
	TMR_DIV_256	= 255
} E_TMR_CLK_DIV;
#else
// bit[4:1], Divisor for clock pre-scaler
typedef enum
{
	TMR_DIV_1	= 0,
	TMR_DIV_2	= 1,
	TMR_DIV_4	= 2,
	TMR_DIV_8	= 3,
	TMR_DIV_16	= 4,
	TMR_DIV_32	= 5,
	TMR_DIV_64	= 6,
	TMR_DIV_128	= 7,
	TMR_DIV_256	= 8
} E_TMR_CLK_DIV;
#endif

// bit[0], Timer Enable
typedef enum
{
	TMR_DIS	= 0,
	TMR_EN	= 1
} E_TMR_EN_CTRL;

#define TMR_INT_CAP		(0x1<<2)
#define TMR_INT_OVERFLOW	(0x1<<1)
#define TMR_INT_MATCH		(0x1<<0)

struct gtimer_index_regs {
	u32 loadval, count, compval;
	u32 control, irqen;
};

struct gtimer_context_registers {
	struct gtimer_index_regs index_regs[8];
};

struct gtimer {
	void *regs;

	int div;

	int count_mode;
	int run_mode;
	int mode;
};

extern struct gtimer* p_gtimer;

/*apis*/
u32 gtimer_read(struct gtimer *priv, int id, u32 offset);
void gtimer_write(struct gtimer *priv, int id, u32 offset, u32 val);


void gtimer_init(uint32_t id, unsigned long base);
void gtimer_config_default(int id);
void gtimer_set_mode(int id,int count_mode,int run_mode,int mode);
void gtimer_set_div(int id,int div);

void gtimer_reset(int id);
void gtimer_start(int id);
void gtimer_stop(int id);
void gtimer_reload(int id);
void gtimer_set_load(int id,unsigned long val);
void gtimer_set_count(int id,unsigned long val);
void gtimer_set_compare(int id,unsigned long val);
unsigned long gtimer_get_count(int id);
void gtimer_clear_raw_status(int id,unsigned long val);


/*for capture mode operation*/
void gtimer_set_capture_mode(int id, E_TMR_CAP_MODE cap_mode, E_TMR_CAP_EVENT cap_event);
int gtimer_check_capture_done(int id);
void gtimer_get_capture_data(int id, uint32_t *val1, uint32_t *val2);
void gtimer_clear_capture(int id);

void gtimer_set_pwm_mode(int id, E_TMR_PWM_MODE pwm_mode, E_TMR_PWM_OUTPUT_MODE output_mode, E_TMR_PWM_POL pwm_pol);
void gtimer_set_compare_mode(int id,int en);

void gtimer_set_int_mask(int id, unsigned long value);
void gtimer_clear_int(int id,unsigned long value);
unsigned long gtimer_get_int_status(int id);
unsigned long gtimer_get_raw_int_status(int id);

void gtimer_init_1(uint32_t id, unsigned long base, E_TMR_CLK_DIV div);
void gtimer_save_context(int id);
void gtimer_restore_context(int id);
#endif

