/*----------------------------------------------------------------------------*/
/*                                                                            */
/*   Copyright (C) 2016 Brite Semiconductor Co., Ltd. All rights reserved.    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#ifndef __DMA_REG_H__
#define __DMA_REG_H__
#include "cdl_config.h"

/******************************************************************************/
/* macro definition*/
/******************************************************************************/
#define DMA_CTRL_BASE_ADDR (DMAC0_BASE)

/* register layout */

#define DMA_CH_SIZE	0x58

/* Channel Registers*/
#define DMA_SAR_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x000)	// R/W, 0x0, Channel 0 Source Address Register
#define DMA_DAR_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x008)	// R/W, 0x0, Channel 0 Destination Address Register
#define DMA_LLP_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x010)	// R/W, 0x0, Channel 0 Linked List Pointer Register
#define DMA_CTL_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x018)	// R/W, 0x0, Channel 0 Control Register
#define DMA_SSTAT_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x020)	// R/W, 0x0, Channel 0 Source Status Register
#define DMA_DSTAT_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x028)	// R/W, 0x0, Channel 0 Destination Status Register
#define DMA_SSTATAR_REG(port, ch)		(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x030)	// R/W, 0x0, Channel 0 Source Status Address Register
#define DMA_DSTATAR_REG(port, ch)		(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x038)	// R/W, 0x0, Channel 0 Destination Status Address Register
#define DMA_CFG_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x040)	// R/W, 0x0000000400000e00, Channel 0 Configuration Register
#define DMA_SGR_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x048)	// R/W, 0x0, Channel 0 Source Gather Register
#define DMA_DSR_REG(port, ch)			(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x050)	// R/W, 0x0, Channel 0 Destination Scatter Register

/* Interrupt Registers*/
#define DMA_RAW_TFR_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2C0)	// R, 0x0, Raw Status for IntTfr Interrupt
#define DMA_RAW_BLOCK_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2C8)	// R, 0x0, Raw Status for IntBlock Interrupt
#define DMA_RAW_SRC_TRAN_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2D0)	// R, 0x0, Raw St atus for IntSrcTran Interrupt
#define DMA_RAW_DST_TRAN_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2D8)	// R, 0x0, Raw Status for IntDstTran Interrupt
#define DMA_RAW_ERR_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2E0)	// R, 0x0, Raw Status for IntErr Interrupt

#define DMA_STATUS_TFR_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2E8)	// R, 0x0, Status for IntTfr Interrupt
#define DMA_STATUS_BLOCK_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2F0)	// R, 0x0, Status for IntBlock Interrupt
#define DMA_STATUS_SRC_TRAN_REG(port)	(DMA_CTRL_BASE_ADDR + (port << 20) + 0x2F8)	// R, 0x0, Status for IntSrcTran Interrupt
#define DMA_STATUS_DST_TRAN_REG(port)	(DMA_CTRL_BASE_ADDR + (port << 20) + 0x300)	// R, 0x0, Status for IntDstTran Interrupt
#define DMA_STATUS_ERR_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x308)	// R, 0x0, Status for IntErr Interrupt

#define DMA_MASK_TFR_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x310)	// R/W, 0x0, Mask for IntTfr Interrupt
#define DMA_MASK_BLOCK_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x318)	// R/W, 0x0, Mask for IntBlock Interrupt
#define DMA_MASK_SRC_TRAN_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x320)	// R/W, 0x0, Mask for IntSrcTran Interrupt
#define DMA_MASK_DST_TRAN_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x328)	// R/W, 0x0, Mask for IntDstTran Interrupt
#define DMA_MASK_ERR_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x330)	// R/W, 0x0, Mask for IntErr Interrupt

#define DMA_CLEAR_TFR_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x338)	// W, 0x0, Clear for IntTfr Interrupt
#define DMA_CLEAR_BLOCK_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x340)	// W, 0x0, Clear for IntBlock Interrupt
#define DMA_CLEAR_SRC_TRAN_REG(port)	(DMA_CTRL_BASE_ADDR + (port << 20) + 0x348)	// W, 0x0, Clear for IntSrcTran Interrupt
#define DMA_CLEAR_DST_TRAN_REG(port)	(DMA_CTRL_BASE_ADDR + (port << 20) + 0x350)	// W, 0x0, Clear for IntDstTran Interrupt
#define DMA_CLEAR_ERR_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x358)	// W, 0x0, Clear for IntErr Interrupt

#define DMA_STATUS_INT_REG(port, ch)	(DMA_CTRL_BASE_ADDR + (port << 20) + (ch * DMA_CH_SIZE) + 0x360)	// W, 0x0, Status for each interrupt type

/* Software Handshaking Registers*/
#define DMA_REQ_SRC_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x368)	// R/W, 0x0, Source Software Transaction Request Register
#define DMA_REQ_DST_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x370)	// R/W, 0x0, Destination Software Transaction Request Register
#define DMA_REQ_SGL_SRC_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x378)	// R/W, 0x0, Single Source Transaction Request Register
#define DMA_REQ_SGL_DST_REG(port)		(DMA_CTRL_BASE_ADDR + (port << 20) + 0x380)	// R/W, 0x0, Single Destination Transaction Request Register
#define DMA_LST_SRC_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x388)	// R/W, 0x0, Last Source Transaction Request Register
#define DMA_LST_DST_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x390)	// R/W, 0x0, Last Destination Transaction Request Register

/* Miscellaneous Registers*/
#define DMA_DMACFG_REG(port)			(DMA_CTRL_BASE_ADDR + (port << 20) + 0x398)	// R/W, 0x0, DMA Configuration Register
#define DMA_CH_EN_REG(port)				(DMA_CTRL_BASE_ADDR + (port << 20) + 0x3A0)	// R/W, 0x0, DMA Channel Enable Register
#define DMA_ID_REG(port)				(DMA_CTRL_BASE_ADDR + (port << 20) + 0x3A8)	// R,     0x0, DMA ID Register
#define DMA_TEST_REG(port)				(DMA_CTRL_BASE_ADDR + (port << 20) + 0x3B0)	// R/W, 0x0, DMA Test Register

/******************************************************************************/
/* structure definition*/
/******************************************************************************/
/* offset: 0x040,  Configuration Register for Channel x */
typedef enum {
	DMA_CH_PRIOR_SHIFT 	 	= 5,
	DMA_CH_SUSP_SHIFT  		= 8,
	DMA_FIFO_EMPTY_SHIFT  	= 9,
	DMA_HS_SEL_DST_SHIFT  	= 10,
	DMA_HS_SEL_SRC_SHIFT  	= 11,
	DMA_LOCK_CH_L_SHIFT  	= 12,
	DMA_LOCK_B_L_SHIFT  	= 14,
	DMA_LOCK_CH_SHIFT 		= 16,
	DMA_LOCK_B_SHIFT		= 17,
	DMA_DST_HS_POL_SHIFT  	= 18,
	DMA_SRC_HS_POL_SHIFT  	= 19,
	DMA_MAX_ABRST_SHIFT  	= 20,
	DMA_RELOAD_SRC_SHIFT  	= 30,
	DMA_RELOAD_DST_SHIFT  	= 31,
	DMA_FCMODE_SHIFT  		= 32,
	DMA_FIFO_MODE_SHIFT  	= 33,
	DMA_PROTCTL_SHIFT  		= 34,
	DMA_DS_UPD_EN_SHIFT		= 37,
	DMA_SS_UPD_EN_SHIFT		= 38,
	DMA_SRC_PER_SHIFT		= 39,
	DMA_DEST_PER_SHIFT		= 43
}E_DMA_CFG_SHIFT;

/* [b:43]*/
/* [b:39], For DMA0, no. of peripheral handshanking interface*/
typedef enum {
	DMA0_PER_NO_SPI0_TX 	= 0,
	DMA0_PER_NO_SPI0_RX 	= 1,
	DMA0_PER_NO_UART1_TX 	= 2,
	DMA0_PER_NO_UART1_RX 	= 3,
	DMA0_PER_NO_AES_IN 		= 4,
	DMA0_PER_NO_AES_OUT 	= 5,
	DMA0_PER_NO_I2C0_TX 	= 6,
	DMA0_PER_NO_I2C0_RX 	= 7
} E_DMA0_PER_NO;

/* [b:43]*/
/* [b:39], For DMA1, no. of peripheral handshanking interface*/
typedef enum {
	DMA1_PER_NO_UART2_TX 	= 0,
	DMA1_PER_NO_UART2_RX 	= 1,
	DMA1_PER_NO_UART3_TX 	= 2,
	DMA1_PER_NO_UART3_RX 	= 3,
	DMA1_PER_NO_SPI2_TX 	= 4,
	DMA1_PER_NO_SPI2_RX 	= 5,
	DMA1_PER_NO_I2C1_TX 	= 6,
	DMA1_PER_NO_I2C1_RX 	= 7
} E_DMA1_PER_NO;

/* [b:43]*/
/* [b:39], For DMA2, no. of peripheral handshanking interface*/
typedef enum {
	DMA2_PER_NO_UART0_TX 	= 0,
	DMA2_PER_NO_UART0_RX 	= 1,
	DMA2_PER_NO_SPI1_TX 	= 2,
	DMA2_PER_NO_SPI1_RX 	= 3,
	DMA2_PER_NO_I2C2_TX 	= 4,
	DMA2_PER_NO_I2C2_RX 	= 5,
	DMA2_PER_NO_SHA_IN 		= 6,
	DMA2_PER_NO_SAR_ADC_RX 	= 7
} E_DMA2_PER_NO;

#ifndef TAPEOUT_V2
// DMA0, the default hardware handshaking signals assignment
#define DMA_REQ_SPI0_TX			0
#define DMA_REQ_SPI0_RX			1
#define DMA_REQ_UART1_TX		2
#define DMA_REQ_UART1_RX		3
#define DMA_REQ_AES_IN			4
#define DMA_REQ_AES_OUT			5
#define DMA_REQ_I2C0_TX			6
#define DMA_REQ_I2C0_RX			7
#define DMA_REQ_EXTERNAL_IRQ0	8	// default GPIO Bank A int 0

// DMA1, the default hardware handshaking signals assignment
#define DMA_REQ_UART2_TX		0
#define DMA_REQ_UART2_RX		1
#define DMA_REQ_UART3_TX		2
#define DMA_REQ_UART3_RX		3
#define DMA_REQ_SPI2_TX			4
#define DMA_REQ_SPI2_RX			5
#define DMA_REQ_I2C1_TX			6
#define DMA_REQ_I2C1_RX			7
#define DMA_REQ_EXTERNAL_IRQ1	8	// default GPIO Bank A int 1

// DMA2, the default hardware handshaking signals assignment
#define DMA_REQ_UART0_TX		0
#define DMA_REQ_UART0_RX		1
#define DMA_REQ_SPI1_TX			2
#define DMA_REQ_SPI1_RX			3
#define DMA_REQ_I2C2_TX			4
#define DMA_REQ_I2C2_RX			5
#define DMA_REQ_SHA_IN			6
#define DMA_REQ_SAR_ADC_RX		7
#define DMA_REQ_EXTERNAL_IRQ2	8	// default GPIO Bank B int 0
#else
// DMA0, the default hardware handshaking signals assignment
#define DMA_REQ_SPI0_TX			0
#define DMA_REQ_SPI0_RX			1
#define DMA_REQ_UART1_TX		2
#define DMA_REQ_UART1_RX		3
#define DMA_REQ_I2C2_TX			4
#define DMA_REQ_I2C2_RX			5
#define DMA_REQ_I2C0_TX			6
#define DMA_REQ_I2C0_RX			7
#define DMA_REQ_EXTERNAL_IRQ0	8	// default GPIO Bank A int 0

// DMA1, the default hardware handshaking signals assignment
#define DMA_REQ_UART2_TX		0
#define DMA_REQ_UART2_RX		1
#define DMA_REQ_UART3_TX		2
#define DMA_REQ_UART3_RX		3
#define DMA_REQ_SPI2_TX			4
#define DMA_REQ_SPI2_RX			5
#define DMA_REQ_I2C1_TX			6
#define DMA_REQ_I2C1_RX			7
#define DMA_REQ_EXTERNAL_IRQ1	8	// default GPIO Bank A int 1

// DMA2, the default hardware handshaking signals assignment
#define DMA_REQ_UART0_TX		0
#define DMA_REQ_UART0_RX		1
#define DMA_REQ_SPI1_TX			2
#define DMA_REQ_SPI1_RX			3
#define DMA_REQ_AES_IN			4
#define DMA_REQ_AES_OUT			5
#define DMA_REQ_SHA_IN			6
#define DMA_REQ_SAR_ADC_RX		7
#define DMA_REQ_EXTERNAL_IRQ2	8	// default GPIO Bank B int 0

#endif

/* [38], */
/* [37], Source/Destination Status Update Enable*/
typedef enum {
	UPD_DIS            = 0,
	UPD_EN             = 1
} E_DMA_UPD_EN;

/* [33], FIFO Mode Select.*/
typedef enum {
	FIFO_MODE_DIS      = 0,	// Space/data available for single AHB transfer of the specified transfer width
	FIFO_MODE_EN       = 1	// Data available is greater than or equal to half the FIFO depth for destination
						    // transfers and space available is greater than half the fifo depth for source transfers.
} E_DMA_FIFO_MODE_SEL;

/* [32], Flow Control Mode */
typedef enum {
	FC_MODE_SERVICED   	= 0,
	FC_MODE_NO_SERVICED	= 1
} E_DMA_FLOW_CTRL_MODE;

/* [31]*/
/* [30], Automatic Source/Destination Reload*/
typedef enum {
	RELOAD_DIS         = 0,
	RELOAD_EN          = 1
} E_DMA_RELOAD;

/* [11]*/
/* [10], Source/Destination Software or Hardware Handshaking Select*/
typedef enum {
	HS_SEL_HW          = 0,	// Hardware handshaking interface
	HS_SEL_SW          = 1	// Software handshaking interface
} E_HS_SEL;

/* [7:5], Channel priority*/
typedef enum {
	CH_PRIOR_0         = 0,	// the lowest priority
	CH_PRIOR_1         = 1,
	CH_PRIOR_2         = 2,
	CH_PRIOR_3         = 3,
	CH_PRIOR_4         = 4,
	CH_PRIOR_5         = 5,
	CH_PRIOR_6         = 6,
	CH_PRIOR_7         = 7, // the highest priority
	NUM_DMA_PRIORITIES = 8
} E_DMA_CH_PRIOR;

/* low 32 bits*/
#define DMA_CFG_L32BITS_VAL(ch_prior, hs_sel_dst, hs_sel_src, reload_src, reload_dst)\
			(((reload_dst) << DMA_RELOAD_DST_SHIFT)|\
			((reload_src) << DMA_RELOAD_SRC_SHIFT)|\
			((0) << DMA_MAX_ABRST_SHIFT)|\
			((0) << DMA_SRC_HS_POL_SHIFT)|\
			((0) << DMA_DST_HS_POL_SHIFT)|\
			((0) << DMA_LOCK_B_SHIFT)|\
			((0) << DMA_LOCK_CH_SHIFT)|\
			((0) << DMA_LOCK_B_L_SHIFT)|\
			((0) << DMA_LOCK_CH_L_SHIFT)|\
			((hs_sel_src) << DMA_HS_SEL_SRC_SHIFT)|\
			((hs_sel_dst) << DMA_HS_SEL_DST_SHIFT)|\
			((1) << DMA_FIFO_EMPTY_SHIFT)|\
			((0) << DMA_CH_SUSP_SHIFT)|\
			((ch_prior) << DMA_CH_PRIOR_SHIFT))

/* hs mask and value*/
#define DMA_HS_SEL_MASK	((1<<DMA_HS_SEL_SRC_SHIFT)|\
                        (1<<DMA_HS_SEL_DST_SHIFT))

#define DMA_HS_SEL_VAL(hs_dst, hs_src)	(((hs_src)<<DMA_HS_SEL_SRC_SHIFT)|\
                                         ((hs_dst)<<DMA_HS_SEL_DST_SHIFT))

/* high 32 bits*/
#define DMA_CFG_H32BITS_VAL(src_per, dest_per)\
			(((dest_per) << (DMA_DEST_PER_SHIFT - 32))|\
			((src_per) << (DMA_SRC_PER_SHIFT - 32))|\
			((1) << (DMA_SS_UPD_EN_SHIFT - 32))|\
			((1) << (DMA_DS_UPD_EN_SHIFT - 32))|\
			((1) << (DMA_PROTCTL_SHIFT - 32))|\
			((0) << (DMA_FIFO_MODE_SHIFT - 32))|\
			((0) << (DMA_FCMODE_SHIFT - 32)))

/* hs mask and value*/
#define DMA_HS_PER_MASK	((0x0F<<(DMA_SRC_PER_SHIFT - 32))|\
                        (0x0F<<(DMA_DEST_PER_SHIFT - 32)))

#define DMA_HS_PER_VAL(dst_per, src_per)	(((src_per)<<(DMA_SRC_PER_SHIFT - 32))|\
                                             ((dst_per)<<(DMA_DEST_PER_SHIFT - 32)))


/* offset: 0x018,  Control Register for Channel x */
typedef enum {
	DMA_INT_EN_SHIFT 	 		= 0,
	DMA_DST_TR_WIDTH_SHIFT  	= 1,
	DMA_SRC_TR_WIDTH_SHIFT  	= 4,
	DMA_DINC_SHIFT  			= 7,
	DMA_SINC_SHIFT  			= 9,
	DMA_DEST_MSIZE_SHIFT  		= 11,
	DMA_SRC_MSIZE_SHIFT  		= 14,
	DMA_SRC_GATHER_EN_SHIFT 	= 17,
	DMA_DST_SCATTER_EN_SHIFT	= 18,
	DMA_TT_FC_SHIFT  			= 20,
	DMA_DMS_SHIFT  				= 23,
	DMA_SMS_SHIFT  				= 25,
	DMA_LLP_DST_EN_SHIFT  		= 27,
	DMA_LLP_SRC_EN_SHIFT  		= 28,
	DMA_BLOCK_TS_SHIFT  		= 32,
	DMA_DONE_SHIFT  			= 44,
} E_DMA_CTL_SHIFT;

/* Block chaining is enabled on the source/destination side*/
typedef enum {
	LLP_DIS = 0,
	LLP_EN  = 1
} E_DMA_LLP_EN;

/* Source/Destination Master Select*/
typedef enum {
	AHB_MASTER_1 = 0,
	AHB_MASTER_2 = 1,
	AHB_MASTER_3 = 2,
	AHB_MASTER_4 = 3
} E_DMA_CTL_MS;

/* CTLx.TT_FC Field Decoding */
/* Transfer Type and Flow Control*/
typedef enum {
	TT_M2M 	= 0,
	TT_M2P 	= 1,
	TT_P2M 	= 2,
	TT_P2P 	= 3
} E_DMA_CTL_TT_FC;

/* Source gather enable bit*/
typedef enum {
	SCATTER_DIS = 0,
	SCATTER_EN 	= 1
} E_DMA_CTL_DST_SCATTER_EN;

/* Source gather enable bit*/
typedef enum {
	GATHER_DIS 	= 0,
	GATHER_EN 	= 1
} E_DMA_CTL_SRC_GATHER_EN;

/* CTLx.SRC_MSIZE and DEST_MSIZE Decoding */
typedef enum {
	MSIZE_1 	= 0,	// Number of data items to be transferred(of width CTLx.SRC_TR_WIDTH or CTLx.DST_TR_WIDTH) 
	MSIZE_4 	= 1,
	MSIZE_8 	= 2,
	MSIZE_16 	= 3,
	MSIZE_32 	= 4,
	MSIZE_64 	= 5,
	MSIZE_128 	= 6,
	MSIZE_256 	= 7
} E_DMA_CTL_MSIZE;

/* Source/Destination Address Increment.*/
typedef enum {
	SD_INC 		 = 0,	// increment
	SD_DEC 		 = 1,	// decrement
	SD_NO_CHANGE = 2	// no change 
} E_DMA_CTL_SD_INC;

/* CTLx.SRC_TR_WIDTH and CTLx.DST_TR_WIDTH Decoding*/
typedef enum {
	WIDTH_8_BITS 	= 0,
	WIDTH_16_BITS 	= 1,
	WIDTH_32_BITS 	= 2,
	WIDTH_64_BITS 	= 3,
	WIDTH_128_BITS 	= 4,
	WIDTH_256_BITS 	= 5
} E_DMA_CTL_WIDTH;

/* Source/Destination Address Increment.*/
typedef enum {
	DMA_INT_DIS = 0,
	DMA_INT_EN	= 1
} E_DMA_INT_EN;


/* low 32 bits*/
#define DMA_CTL_L32BITS_VAL(int_en, dst_tr_width, src_tr_width, dinc, sinc, dst_msize,\
        src_msize, src_gather_en, dst_scatter_en, tt_fc, llp_dst_en, llp_src_en)\
			(((llp_src_en) << DMA_LLP_SRC_EN_SHIFT)|\
			((llp_dst_en) << DMA_LLP_DST_EN_SHIFT)|\
			((AHB_MASTER_1) << DMA_SMS_SHIFT)|\
			((AHB_MASTER_1) << DMA_DMS_SHIFT)|\
			((tt_fc) << DMA_TT_FC_SHIFT)|\
			((dst_scatter_en) << DMA_DST_SCATTER_EN_SHIFT)|\
			((src_gather_en) << DMA_SRC_GATHER_EN_SHIFT)|\
			((src_msize) << DMA_SRC_MSIZE_SHIFT)|\
			((dst_msize) << DMA_DEST_MSIZE_SHIFT)|\
			((sinc) << DMA_SINC_SHIFT)|\
			((dinc) << DMA_DINC_SHIFT)|\
			((src_tr_width) << DMA_SRC_TR_WIDTH_SHIFT)|\
			((dst_tr_width) << DMA_DST_TR_WIDTH_SHIFT)|\
			((int_en) << DMA_INT_EN_SHIFT))

/* high 32 bits*/
#define DMA_CTL_H32BITS_VAL(blk_ts, done)\
			(((done) << (DMA_DONE_SHIFT - 32))|\
			((blk_ts) << (DMA_BLOCK_TS_SHIFT - 32)))

/* offset: 0x398, DMA Configuration Register*/

/* dma enable*/
typedef enum
{
	DMA_DIS = 0,
	DMA_EN 	= 1
} E_DMA_DMACFG_EN;

#define DMA_CFG_VAL(en) (en << 0)

/* offset: 0x3A0, DMA Channel Enable Register*/
typedef enum {
	CH_EN_SHIFT  	= 0,
	CH_EN_WE_SHIFT 	= 8
} E_DMA_CH_EN_SHIFT;

/* channel enable*/
typedef enum {
	CH_DIS  = 0,
	CH_EN 	= 1
} E_DMA_CH_EN;

#define DMA_CH_EN_VAL(ch, en) ((CH_EN << (ch + CH_EN_WE_SHIFT)) | (en << (ch + CH_EN_SHIFT)))

/* offset: 0x310~0x330, Mask Status Register*/
typedef enum {
	DMA_INT_MASK_SHIFT  	= 0,
	DMA_INT_MASK_WE_SHIFT  	= 8	
} E_DMA_MASK_SHIFT;

/* [8], Interrupt Mask Write Enable*/
typedef enum {
	DMA_MASK_WR_DIS = 0,
	DMA_MASK_WR_EN	= 1
} E_DMA_MASK_WR_EN;

/* [0], Interrupt Mask*/
typedef enum {
	DMA_MASKED 		= 0,
	DMA_UNMASKED	= 1
} E_DMA_MASK_EN;

#define DMA_MASK_VAL(ch, mask_wr, mask) ((mask_wr << (ch + DMA_INT_MASK_WE_SHIFT)) | (mask << (ch + DMA_INT_MASK_SHIFT)))

#endif

