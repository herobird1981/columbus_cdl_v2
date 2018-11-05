/*----------------------------------------------------------------------------------------------------------*/
/*                                                                                               											 		 */
/*           Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.                									 */
/*                                                                                                													 */
/*-----------------------------------------------------------------------------------------------------------*/
#ifndef __CDL_GPIO_H__
#define __CDL_GPIO_H__
#include "cdl_sys.h"

/*************************************************************************************************************/
// macro definition
/*************************************************************************************************************/
// Register map
#define GPIO_PORT_SIZE 0x40

#define BRITE_GPIO_DATA(port) 	((port * GPIO_PORT_SIZE) + 0x00)	// width(32), RW, default 0x00000000, Data of Input/Output register
#define BRITE_GPIO_DIR(port)	((port * GPIO_PORT_SIZE) + 0x04)	// width(32), RW, default 0x00000000, Direction of data flow register  
#define BRITE_GPIO_CTRL(port) 	((port * GPIO_PORT_SIZE) + 0x08)	// width(32), RW, default 0x00000000, Control mode select register
#define BRITE_GPIO_EXT(port)	((port * GPIO_PORT_SIZE) + 0x0C)	// width(32), RO, default 0x00000000, Port data extend register
#define BRITE_GPIO_IEN(port)	((port * GPIO_PORT_SIZE) + 0x10)	// width(32), RW, default 0x00000000, Interrupt enable register
#define BRITE_GPIO_IS(port) 	((port * GPIO_PORT_SIZE) + 0x14)	// width(32), RW, default 0x00000000, Interrupt sense register
#define BRITE_GPIO_IBE(port)	((port * GPIO_PORT_SIZE) + 0x18)	// width(32), RW, default 0x00000000, Interrupt both edge register
#define BRITE_GPIO_IEV(port)	((port * GPIO_PORT_SIZE) + 0x1C)	// width(32), RW, default 0x00000000, Interrupt event register
#define BRITE_GPIO_RIS(port)	((port * GPIO_PORT_SIZE) + 0x20)	// width(32), RO, default 0x00000000, Raw interrupt status register
#define BRITE_GPIO_IM(port) 	((port * GPIO_PORT_SIZE) + 0x24)	// width(32), RW, default 0x00000000, Interrupt mask register
#define BRITE_GPIO_MIS(port)	((port * GPIO_PORT_SIZE) + 0x28)	// width(32), RO, default 0x00000000, Masked interrupt status register
#define BRITE_GPIO_IC(port) 	((port * GPIO_PORT_SIZE) + 0x2C)	// width(32), WO, default 0x00000000, Interrupt clear register
#define BRITE_GPIO_DB(port) 	((port * GPIO_PORT_SIZE) + 0x30)	// width(32), RW, default 0x00000000, Delete Bounce flag
#define BRITE_GPIO_DFG(port)	((port * GPIO_PORT_SIZE) + 0x34)	// width(32), RW, default 0x00000000, Define Filter-ed Glitch
#define BRITE_GPIO_IG(port)		((port * GPIO_PORT_SIZE) + 0x38)	// width(32), RW, default 0xFFFF0000, Specify Interrupt Group

/*************************************************************************************************************/
// gpio_dir
// Each bit mapping to the gpio_en_n and control the direction of corresponding gpio_xp in software mode.
typedef enum
{
	GPIO_INPUT	= 0,	// "0" for input from gpio_in
	GPIO_OUTPUT	= 1		// "1" for output to gpio_out.
} E_GPIO_DIR;

// gpio_ctrl 
// "0" for software mode, each bit of gpio_dir is used to setup the direction for corresponding pin; 
// "1" for hardware mode, auxiliary hardware signal, gpio_hw_en_n, is used to configure the direction for the corresponding pin.
typedef enum
{
	GPIO_CTRL_SW	= 0,
	GPIO_CTRL_HW	= 1	
} E_GPIO_CTRL;

// gpio_ien 
typedef enum
{
	GPIO_INT_DIS	= 0,
	GPIO_INT_EN		= 1	
} E_GPIO_IEN;

// gpio_is
typedef enum
{
	GPIO_IS_EDGE	= 0, //  "0" is edge-detecting on corresponding pin
	GPIO_IS_LEVEL	= 1	 // "1" is level-sensitive on corresponding pin.
} E_GPIO_IS;

//  gpio_ibe
typedef enum
{
	GPIO_IBE_SINGLE	= 0, // "0" is single edge-sensitive (either falling edge or rising edge), decided by the bit of gpio_iev.
	GPIO_IBE_BOTH	= 1	 // "1" is both edges on corresponding pin trigger an interrupt. 	
} E_GPIO_IBE;

//  gpio_iev
typedef enum
{
	GPIO_IEV_FALLING	= 0, // "0" means that interrupt is triggered in falling edge of corresponding pins,
	GPIO_IEV_RISING		= 1	 // "1" means interrupt is triggered in rising edge of corresponding pins. 	
} E_GPIO_IEV;


typedef enum
{
	GPIO_ITYPE_EDGE_FALLING	= 0,
	GPIO_ITYPE_LEVEL_LOW	= 0,	
	GPIO_ITYPE_EDGE_RISING	= 1, 
	GPIO_ITYPE_LEVEL_HI		= 1,
	GPIO_ITYPE_EDGE_BOTH	= 2
} E_GPIO_ITYPE;

//  gpio_im
typedef enum
{
	GPIO_IM_DIS		= 0, // "0" means that the interrupt on the corresponding gpio_ext bit is unmasked.
	GPIO_IM_EN		= 1	 // "1" means that the interrupt on the corresponding gpio_ext bit is masked.
} E_GPIO_IM;

//  gpio_im
typedef enum
{
	GPIO_DB_DIS		= 0, // ""0" to disable the debounce for each line
	GPIO_DB_EN		= 1	 // "1" to enable the function of debounce for each line
} E_GPIO_DB;

// gpio_ig(Determines the interrupt group for 32 GPIO pins, each bit represents one GPIO pin)
typedef enum
{
	GPIO_IG_INTR0	= 0, // 0, associated with intr0
	GPIO_IG_INTR1	= 1	 // 1, associated with intr0
} E_GPIO_IG;

/*************************************************************************************************************/
// structure definition
/*************************************************************************************************************/
struct brite_gpio {
	void *regs;
	unsigned long pclk;

	unsigned long debounce;
};

/*************************************************************************************************************/
// global variable declaration
/*************************************************************************************************************/
extern struct brite_gpio *p_gpio;

/*************************************************************************************************************/
// function prototype declaration
/*************************************************************************************************************/
void gpio_set_output(E_GPIO_PORT port, E_PIN_PORT pin, uint32_t val);

uint32_t gpio_get_output(E_GPIO_PORT port, E_PIN_PORT pin, uint32_t val);

void gpio_set_direction(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_DIR dir);

void gpio_set_ctrl(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_CTRL ctrl);

uint32_t gpio_get_input(E_GPIO_PORT port, E_PIN_PORT pin);

void gpio_enable_int(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IEN ien);

void gpio_set_int_type(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IS is);

E_GPIO_IS gpio_get_int_type(E_GPIO_PORT port, E_PIN_PORT pin);

void gpio_set_int_trig_type(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_ITYPE itype);

uint32_t gpio_get_raw_int_status(E_GPIO_PORT port, E_PIN_PORT pin);

void gpio_set_int_mask(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IM im);

bool gpio_is_int_mask(E_GPIO_PORT port, E_PIN_PORT pin);

bool gpio_get_int_status(E_GPIO_PORT port, E_PIN_PORT pin);

void gpio_clear_int(E_GPIO_PORT port, E_PIN_PORT pin);

void gpio_enable_debounce(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_DB db);

void gpio_set_ig(E_GPIO_PORT port, E_PIN_PORT pin, E_GPIO_IG ig);

E_GPIO_IG gpio_get_ig(E_GPIO_PORT port, E_PIN_PORT pin);

void gpio_init(E_GPIO_PORT port, uint32_t base, uint32_t pclk, uint32_t dfg);

#endif

