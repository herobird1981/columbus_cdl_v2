
/* ****************************************************************
 *        CADENCE                    Copyright (c) 2001-2016      *
 *                                   Cadence Design Systems, Inc. *
 *                                   All rights reserved.         *
 ******************************************************************
 *  The values calculated from this script are meant to be        *
 *  representative programmings.   The values may not reflect the *
 *  actual required programming for production use.   Please      *
 *  closely review all programmed values for technical accuracy   *
 *  before use in production parts.                               *
 ******************************************************************
 *                                                                 
 *   Module:         regconfig.h
 *   Documentation:  Register programming header file
 *
 ******************************************************************
 ******************************************************************
 * WARNING:  This file was automatically generated.  Manual
 * editing may result in undetermined behavior.
 ******************************************************************
 ******************************************************************/
// REL: brite.shanghai.n1-Columbus_DDR__2016-06-04


//`timescale 10ps/10ps

// ********************************************************************
// Option: IP    : IP Mode                       = CTL
// Option: BL    : Burst Length                  = 8
// Option: CL    : CAS Latency                   = 6
// Option: MHZ   : Simulation MHz                = 400
// Option: AP    : Auto Precharge Mode     (0/1) = 0
// Option: DLLBP : DLL Bypass Mode         (0/1) = 0
// Option: HALF  : Half-Memory Support     (0/1) = 0
// Option: RDIMM : Registered Dimm Support (0/1) = 0
// Option: RSV2  : Reserved                  (0) = 0
// Option: RSV3  : Reserved                  (0) = 0
// Option: BOF   : Burst On the Fly        (0/1) = 0
// Option: WLS   : Write Latency Set       (0/1) = 0
// Option: TCK   : Simulation period in ns       = 
// Option: WRDBIEN : Write DBI Enable       (0/1) = 0
// Option: RDDBIEN : Read DBI Enable       (0/1) = 0
// Option: RDPSTMBLE : RD Postamble En      (0/1) = 0
// Option: WRPSTMBLE : WR Postamble En      (0/1) = 0
// Option: CRC : CRC En      (0/1) = 0
// Option: SOMA  : Memory-SOMA file(s)           = memory.xml.sim,memory.xml.sim,memory.xml.sim
// Option: PL    : CA Parity Latency              = -1
// ********************************************************************
// Memory: memory.xml.sim
// Memory: memory.xml.sim
// ********************************************************************
#ifndef __LPDDR2_CTL_400_CL6_BL8_H__
#define __LPDDR2_CTL_400_CL6_BL8_H__

#define               DENALI_LPDDR2_CTL_00_400MHZ_CL6_BL8_DATA 0x00000500 // VERSION:RD:16:16:=0x0000 DRAM_CLASS:RW:8:4:=0x05 START:RW:0:1:=0x00
#define               DENALI_LPDDR2_CTL_01_400MHZ_CL6_BL8_DATA 0x00000000 // WRITE_DATA_FIFO_DEPTH:RD:24:8:=0x00 MAX_CS_REG:RD:16:2:=0x00 MAX_COL_REG:RD:8:4:=0x00 MAX_ROW_REG:RD:0:5:=0x00
#define               DENALI_LPDDR2_CTL_02_400MHZ_CL6_BL8_DATA 0x00000000 // AXI0_RDFIFO_LOG2_DEPTH:RD:24:8:=0x00 AXI0_CMDFIFO_LOG2_DEPTH:RD:16:8:=0x00 ASYNC_CDC_STAGES:RD:8:8:=0x00 WRITE_DATA_FIFO_PTR_WIDTH:RD:0:8:=0x00
#define               DENALI_LPDDR2_CTL_03_400MHZ_CL6_BL8_DATA 0x00000000 // AXI0_WRCMD_PROC_FIFO_LOG2_DEPTH:RD:16:8:=0x00 AXI0_TRANS_WRFIFO_LOG2_DEPTH:RD:8:8:=0x00 AXI0_WR_ARRAY_LOG2_DEPTH:RD:0:8:=0x00
#define               DENALI_LPDDR2_CTL_04_400MHZ_CL6_BL8_DATA 0x00000028 // TINIT_F0:RW:0:24:=0x000028
#define               DENALI_LPDDR2_CTL_05_400MHZ_CL6_BL8_DATA 0x00000050 // TINIT3_F0:RW:0:24:=0x000050
#define               DENALI_LPDDR2_CTL_06_400MHZ_CL6_BL8_DATA 0x00000001 // TINIT4_F0:RW:0:24:=0x000001
#define               DENALI_LPDDR2_CTL_07_400MHZ_CL6_BL8_DATA 0x00000028 // TINIT5_F0:RW:0:24:=0x000028
#define               DENALI_LPDDR2_CTL_08_400MHZ_CL6_BL8_DATA 0x0000000d // TINIT_F1:RW:0:24:=0x00000d
#define               DENALI_LPDDR2_CTL_09_400MHZ_CL6_BL8_DATA 0x00000019 // TINIT3_F1:RW:0:24:=0x000019
#define               DENALI_LPDDR2_CTL_10_400MHZ_CL6_BL8_DATA 0x00000001 // TINIT4_F1:RW:0:24:=0x000001
#define               DENALI_LPDDR2_CTL_11_400MHZ_CL6_BL8_DATA 0x0000000d // NO_AUTO_MRR_INIT:RW:24:1:=0x00 TINIT5_F1:RW:0:24:=0x00000d
#define               DENALI_LPDDR2_CTL_12_400MHZ_CL6_BL8_DATA 0x00000000 // NO_CMD_INIT:RW:24:1:=0x00 TDLL:RW:8:16:=0x0000 INITAREF:RW:0:4:=0x00
#define               DENALI_LPDDR2_CTL_13_400MHZ_CL6_BL8_DATA 0x030c030c // WRLAT_F1:RW:24:3:=0x03 CASLAT_LIN_F1:RW:16:5:=0x0c WRLAT_F0:RW:8:3:=0x03 CASLAT_LIN_F0:RW:0:5:=0x0c
#define               DENALI_LPDDR2_CTL_14_400MHZ_CL6_BL8_DATA 0x1a040202 // TRC_F0_0:RW:24:8:=0x1a TRRD_F0_0:RW:16:8:=0x04 TCCD_0:RW:8:5:=0x02 TBST_INT_INTERVAL:RW:0:3:=0x02
#define               DENALI_LPDDR2_CTL_15_400MHZ_CL6_BL8_DATA 0x14080311 // TFAW_F0_0:RW:24:8:=0x14 TRP_F0_0:RW:16:8:=0x08 TWTR_F0_0:RW:8:6:=0x03 TRAS_MIN_F0_0:RW:0:8:=0x11
#define               DENALI_LPDDR2_CTL_16_400MHZ_CL6_BL8_DATA 0x02060802 // TWTR_F1_0:RW:24:6:=0x02 TRAS_MIN_F1_0:RW:16:8:=0x06 TRC_F1_0:RW:8:8:=0x08 TRRD_F1_0:RW:0:8:=0x02
#define               DENALI_LPDDR2_CTL_17_400MHZ_CL6_BL8_DATA 0x04020803 // TRRD_F0_1:RW:24:8:=0x04 TCCD_1:RW:16:5:=0x02 TFAW_F1_0:RW:8:8:=0x08 TRP_F1_0:RW:0:8:=0x03
#define               DENALI_LPDDR2_CTL_18_400MHZ_CL6_BL8_DATA 0x0803111a // TRP_F0_1:RW:24:8:=0x08 TWTR_F0_1:RW:16:6:=0x03 TRAS_MIN_F0_1:RW:8:8:=0x11 TRC_F0_1:RW:0:8:=0x1a
#define               DENALI_LPDDR2_CTL_19_400MHZ_CL6_BL8_DATA 0x06080214 // TRAS_MIN_F1_1:RW:24:8:=0x06 TRC_F1_1:RW:16:8:=0x08 TRRD_F1_1:RW:8:8:=0x02 TFAW_F0_1:RW:0:8:=0x14
#define               DENALI_LPDDR2_CTL_20_400MHZ_CL6_BL8_DATA 0x03080302 // TRTP_F0:RW:24:8:=0x03 TFAW_F1_1:RW:16:8:=0x08 TRP_F1_1:RW:8:8:=0x03 TWTR_F1_1:RW:0:6:=0x02
#define               DENALI_LPDDR2_CTL_21_400MHZ_CL6_BL8_DATA 0x6d600505 // TRAS_MAX_F0:RW:16:16:=0x6d60 TMOD_F0:RW:8:8:=0x05 TMRD_F0:RW:0:8:=0x05
#define               DENALI_LPDDR2_CTL_22_400MHZ_CL6_BL8_DATA 0x05020603 // TMRD_F1:RW:24:8:=0x05 TRTP_F1:RW:16:8:=0x02 TCKESR_F0:RW:8:8:=0x06 TCKE_F0:RW:0:3:=0x03
#define               DENALI_LPDDR2_CTL_23_400MHZ_CL6_BL8_DATA 0x03222e05 // TCKE_F1:RW:24:3:=0x03 TRAS_MAX_F1:RW:8:16:=0x222e TMOD_F1:RW:0:8:=0x05
#define               DENALI_LPDDR2_CTL_24_400MHZ_CL6_BL8_DATA 0x06080003 // TWR_F0_0:RW:24:6:=0x06 TRCD_F0_0:RW:16:8:=0x08 WRITEINTERP:RW:8:1:=0x00 TCKESR_F1:RW:0:8:=0x03
#define               DENALI_LPDDR2_CTL_25_400MHZ_CL6_BL8_DATA 0x06080303 // TWR_F0_1:RW:24:6:=0x06 TRCD_F0_1:RW:16:8:=0x08 TWR_F1_0:RW:8:6:=0x03 TRCD_F1_0:RW:0:8:=0x03
#define               DENALI_LPDDR2_CTL_26_400MHZ_CL6_BL8_DATA 0x00020303 // AP:RW:24:1:=0x00 TMRR:RW:16:4:=0x02 TWR_F1_1:RW:8:6:=0x03 TRCD_F1_1:RW:0:8:=0x03
#define               DENALI_LPDDR2_CTL_27_400MHZ_CL6_BL8_DATA 0x060e0100 // TDAL_F1:RW:24:8:=0x06 TDAL_F0:RW:16:8:=0x0e TRAS_LOCKOUT:RW:8:1:=0x01 CONCURRENTAP:RW:0:1:=0x00
#define               DENALI_LPDDR2_CTL_28_400MHZ_CL6_BL8_DATA 0x09030903 // TRP_AB_F0_1:RW:24:8:=0x09 TRP_AB_F1_0:RW:16:8:=0x03 TRP_AB_F0_0:RW:8:8:=0x09 BSTLEN:RW_D:0:3:=0x03
#define               DENALI_LPDDR2_CTL_29_400MHZ_CL6_BL8_DATA 0x00000003 // RESERVED:RW:24:1:=0x00 AREFRESH:WR:16:1:=0x00 REG_DIMM_ENABLE:RW:8:1:=0x00 TRP_AB_F1_1:RW:0:8:=0x03
#define               DENALI_LPDDR2_CTL_30_400MHZ_CL6_BL8_DATA 0x00003401 // TRFC_F0:RW:8:10:=0x0034 TREF_ENABLE:RW:0:1:=0x01
#define               DENALI_LPDDR2_CTL_31_400MHZ_CL6_BL8_DATA 0x00110c28 // TRFC_F1:RW:16:10:=0x0011 TREF_F0:RW:0:16:=0x0c28
#define               DENALI_LPDDR2_CTL_32_400MHZ_CL6_BL8_DATA 0x000503c7 // TREF_INTERVAL:RW:16:16:=0x0005 TREF_F1:RW:0:16:=0x03c7
#define               DENALI_LPDDR2_CTL_33_400MHZ_CL6_BL8_DATA 0x00000300 // TPDEX_F0:RW:8:16:=0x0003
#define               DENALI_LPDDR2_CTL_34_400MHZ_CL6_BL8_DATA 0x00380002 // TXSR_F0:RW:16:16:=0x0038 TPDEX_F1:RW:0:16:=0x0002
#define               DENALI_LPDDR2_CTL_35_400MHZ_CL6_BL8_DATA 0x00120038 // TXSR_F1:RW:16:16:=0x0012 TXSNR_F0:RW:0:16:=0x0038
#define               DENALI_LPDDR2_CTL_36_400MHZ_CL6_BL8_DATA 0x00000012 // RESERVED:RW:24:1:=0x00 PWRUP_SREFRESH_EXIT:RW:16:1:=0x00 TXSNR_F1:RW:0:16:=0x0012
#define               DENALI_LPDDR2_CTL_37_400MHZ_CL6_BL8_DATA 0x02000001 // CKSRE_F0:RW:24:8:=0x02 LOWPOWER_REFRESH_ENABLE:RW:16:2:=0x00 CKE_DELAY:RW:8:3:=0x00 ENABLE_QUICK_SREFRESH:RW:0:1:=0x01
#define               DENALI_LPDDR2_CTL_38_400MHZ_CL6_BL8_DATA 0x00020202 // CKSRX_F1:RW:16:8:=0x02 CKSRE_F1:RW:8:8:=0x02 CKSRX_F0:RW:0:8:=0x02
#define               DENALI_LPDDR2_CTL_39_400MHZ_CL6_BL8_DATA 0x02000000 // LPI_SR_WAKEUP_F0:RW:24:4:=0x02 LPI_PD_WAKEUP_F0:RW:16:4:=0x00 LP_CMD:WR:0:9:=0x0000
#define               DENALI_LPDDR2_CTL_40_400MHZ_CL6_BL8_DATA 0x00070f03 // LPI_PD_WAKEUP_F1:RW:24:4:=0x00 LPI_TIMER_WAKEUP_F0:RW:16:4:=0x07 LPI_DPD_WAKEUP_F0:RW:8:4:=0x0f LPI_SR_MCCLK_GATE_WAKEUP_F0:RW:0:4:=0x03
#define               DENALI_LPDDR2_CTL_41_400MHZ_CL6_BL8_DATA 0x070f0302 // LPI_TIMER_WAKEUP_F1:RW:24:4:=0x07 LPI_DPD_WAKEUP_F1:RW:16:4:=0x0f LPI_SR_MCCLK_GATE_WAKEUP_F1:RW:8:4:=0x03 LPI_SR_WAKEUP_F1:RW:0:4:=0x02
#define               DENALI_LPDDR2_CTL_42_400MHZ_CL6_BL8_DATA 0x0000031f // LPI_TIMER_COUNT:RW:8:12:=0x0003 LPI_WAKEUP_EN:RW:0:5:=0x1f
#define               DENALI_LPDDR2_CTL_43_400MHZ_CL6_BL8_DATA 0x00070004 // LP_STATE_CS0:RD:24:6:=0x00 TDFI_LP_RESP:RW:16:3:=0x07 LPI_WAKEUP_TIMEOUT:RW:0:12:=0x0004
#define               DENALI_LPDDR2_CTL_44_400MHZ_CL6_BL8_DATA 0x00000000 // LP_AUTO_EXIT_EN:RW:24:3:=0x00 LP_AUTO_ENTRY_EN:RW:16:3:=0x00 LP_ARB_STATE:RD:8:4:=0x00 LP_STATE_CS1:RD:0:6:=0x00
#define               DENALI_LPDDR2_CTL_45_400MHZ_CL6_BL8_DATA 0x00000000 // LP_AUTO_SR_IDLE:RW:24:8:=0x00 LP_AUTO_PD_IDLE:RW:8:12:=0x0000 LP_AUTO_MEM_GATE_EN:RW:0:2:=0x00
#define               DENALI_LPDDR2_CTL_46_400MHZ_CL6_BL8_DATA 0x04040100 // PCPCS_PD_EXIT_DEPTH:RW:24:4:=0x04 PCPCS_PD_ENTER_DEPTH:RW:16:4:=0x04 PCPCS_PD_EN:RW:8:1:=0x01 LP_AUTO_SR_MC_GATE_IDLE:RW:0:8:=0x00
#define               DENALI_LPDDR2_CTL_47_400MHZ_CL6_BL8_DATA 0x00030000 // LP_CS:RW:24:2:=0x00 PCPCS_CS_MAP:RW:16:2:=0x03 PCPCS_PD_MASK:RW:8:2:=0x00 PCPCS_PD_ENTER_TIMER:RW:0:8:=0x00
#define               DENALI_LPDDR2_CTL_48_400MHZ_CL6_BL8_DATA 0x00000000 // CS1_IDLE:RD:8:1:=0x00 CS0_IDLE:RD:0:1:=0x00
#define               DENALI_LPDDR2_CTL_49_400MHZ_CL6_BL8_DATA 0x000000c8 // TDPD_F0:RW:0:24:=0x0000c8
#define               DENALI_LPDDR2_CTL_50_400MHZ_CL6_BL8_DATA 0x0000003f // TDPD_CNT_DONE_STATUS:RD:24:2:=0x00 TDPD_F1:RW:0:24:=0x00003f
#define               DENALI_LPDDR2_CTL_51_400MHZ_CL6_BL8_DATA 0x20010003 // TDFI_INIT_START_F0:RW_D:24:8:=0x20 FREQ_CHANGE_ENABLE:RW:16:1:=0x01 RESERVED:RW:8:6:=0x00 PWRUP_SREFRESH_EXIT_CS:RW:0:2:=0x03
#define               DENALI_LPDDR2_CTL_52_400MHZ_CL6_BL8_DATA 0x00200400 // TDFI_INIT_START_F1:RW_D:16:8:=0x20 TDFI_INIT_COMPLETE_F0:RW_D:0:16:=0x0400
#define               DENALI_LPDDR2_CTL_53_400MHZ_CL6_BL8_DATA 0x00000400 // DFS_PHY_REG_WRITE_EN:RW:24:1:=0x00 CURRENT_REG_COPY:RD:16:1:=0x00 TDFI_INIT_COMPLETE_F1:RW_D:0:16:=0x0400
#define               DENALI_LPDDR2_CTL_54_400MHZ_CL6_BL8_DATA 0x00000100 // DFS_PHY_REG_WRITE_ADDR:RW:0:32:=0x00000100
#define               DENALI_LPDDR2_CTL_55_400MHZ_CL6_BL8_DATA 0x00000000 // DFS_PHY_REG_WRITE_DATA_F0:RW:0:32:=0x00000000
#define               DENALI_LPDDR2_CTL_56_400MHZ_CL6_BL8_DATA 0x00000001 // DFS_PHY_REG_WRITE_DATA_F1:RW:0:32:=0x00000001
#define               DENALI_LPDDR2_CTL_57_400MHZ_CL6_BL8_DATA 0x00001000 // DFS_PHY_REG_WRITE_WAIT:RW:8:16:=0x0010 DFS_PHY_REG_WRITE_MASK:RW:0:4:=0x00
#define               DENALI_LPDDR2_CTL_58_400MHZ_CL6_BL8_DATA 0x00000000 // WRITE_MODEREG:RW+:0:27:=0x00000000
#define               DENALI_LPDDR2_CTL_59_400MHZ_CL6_BL8_DATA 0x00000000 // READ_MODEREG:RW+:8:17:=0x000000 MRW_STATUS:RD:0:8:=0x00
#define               DENALI_LPDDR2_CTL_60_400MHZ_CL6_BL8_DATA 0x00000000 // AUTO_TEMPCHK_VAL:RD:16:8:=0x00 PERIPHERAL_MRR_DATA:RD:0:16:=0x0000
#define               DENALI_LPDDR2_CTL_61_400MHZ_CL6_BL8_DATA 0x00000002 // MR0_DATA_F0_0:RW:16:16:=0x0000 REFRESH_PER_AUTO_TEMPCHK:RW:0:16:=0x0002
#define               DENALI_LPDDR2_CTL_62_400MHZ_CL6_BL8_DATA 0x00040083 // MR2_DATA_F0_0:RW:16:16:=0x0004 MR1_DATA_F0_0:RW:0:16:=0x0083
#define               DENALI_LPDDR2_CTL_63_400MHZ_CL6_BL8_DATA 0x00230000 // MR1_DATA_F1_0:RW:16:16:=0x0023 MR0_DATA_F1_0:RW:0:16:=0x0000
#define               DENALI_LPDDR2_CTL_64_400MHZ_CL6_BL8_DATA 0x00000004 // MRSINGLE_DATA_0:RW:16:16:=0x0000 MR2_DATA_F1_0:RW:0:16:=0x0004
#define               DENALI_LPDDR2_CTL_65_400MHZ_CL6_BL8_DATA 0x00010001 // MR3_DATA_F1_0:RW:16:16:=0x0001 MR3_DATA_F0_0:RW:0:16:=0x0001
#define               DENALI_LPDDR2_CTL_66_400MHZ_CL6_BL8_DATA 0x00000000 // MR17_DATA_0:RW:16:8:=0x00 MR16_DATA_0:RW:8:8:=0x00 MR8_DATA_0:RD:0:8:=0x00
#define               DENALI_LPDDR2_CTL_67_400MHZ_CL6_BL8_DATA 0x00830000 // MR1_DATA_F0_1:RW:16:16:=0x0083 MR0_DATA_F0_1:RW:0:16:=0x0000
#define               DENALI_LPDDR2_CTL_68_400MHZ_CL6_BL8_DATA 0x00000004 // MR0_DATA_F1_1:RW:16:16:=0x0000 MR2_DATA_F0_1:RW:0:16:=0x0004
#define               DENALI_LPDDR2_CTL_69_400MHZ_CL6_BL8_DATA 0x00040023 // MR2_DATA_F1_1:RW:16:16:=0x0004 MR1_DATA_F1_1:RW:0:16:=0x0023
#define               DENALI_LPDDR2_CTL_70_400MHZ_CL6_BL8_DATA 0x00010000 // MR3_DATA_F0_1:RW:16:16:=0x0001 MRSINGLE_DATA_1:RW:0:16:=0x0000
#define               DENALI_LPDDR2_CTL_71_400MHZ_CL6_BL8_DATA 0x00000001 // MR16_DATA_1:RW:24:8:=0x00 MR8_DATA_1:RD:16:8:=0x00 MR3_DATA_F1_1:RW:0:16:=0x0001
#define               DENALI_LPDDR2_CTL_72_400MHZ_CL6_BL8_DATA 0x00000000 // ADDR_SPACE:RW:24:6:=0x00 BIST_RESULT:RD:16:2:=0x00 BIST_GO:WR:8:1:=0x00 MR17_DATA_1:RW:0:8:=0x00
#define               DENALI_LPDDR2_CTL_73_400MHZ_CL6_BL8_DATA 0x00000101 // BIST_ADDR_CHECK:RW:8:1:=0x01 BIST_DATA_CHECK:RW:0:1:=0x01
#define               DENALI_LPDDR2_CTL_74_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_START_ADDRESS:RW:0:34:=0x00000000
#define               DENALI_LPDDR2_CTL_75_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_START_ADDRESS:RW:0:34:=0x00
#define               DENALI_LPDDR2_CTL_76_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_DATA_MASK:RW:0:64:=0x00000000
#define               DENALI_LPDDR2_CTL_77_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_DATA_MASK:RW:0:64:=0x00000000
#define               DENALI_LPDDR2_CTL_78_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_TEST_MODE:RW:0:3:=0x00
#define               DENALI_LPDDR2_CTL_79_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_DATA_PATTERN:RW:0:128:=0x00000000
#define               DENALI_LPDDR2_CTL_80_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_DATA_PATTERN:RW:0:128:=0x00000000
#define               DENALI_LPDDR2_CTL_81_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_DATA_PATTERN:RW:0:128:=0x00000000
#define               DENALI_LPDDR2_CTL_82_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_DATA_PATTERN:RW:0:128:=0x00000000
#define               DENALI_LPDDR2_CTL_83_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_ERR_STOP:RW:16:12:=0x0000 BIST_RET_STATE:RD:8:1:=0x00 BIST_RET_STATE_EXIT:WR:0:1:=0x00
#define               DENALI_LPDDR2_CTL_84_400MHZ_CL6_BL8_DATA 0x00000000 // LONG_COUNT_MASK:RW:16:5:=0x00 BIST_ERR_COUNT:RD:0:12:=0x0000
#define               DENALI_LPDDR2_CTL_85_400MHZ_CL6_BL8_DATA 0x00900190 // ZQCL_F0:RW:16:12:=0x0090 ZQINIT_F0:RW_D:0:12:=0x0190
#define               DENALI_LPDDR2_CTL_86_400MHZ_CL6_BL8_DATA 0x007d0024 // ZQINIT_F1:RW_D:16:12:=0x007d ZQCS_F0:RW:0:12:=0x0024
#define               DENALI_LPDDR2_CTL_87_400MHZ_CL6_BL8_DATA 0x000c002d // ZQCS_F1:RW:16:12:=0x000c ZQCL_F1:RW:0:12:=0x002d
#define               DENALI_LPDDR2_CTL_88_400MHZ_CL6_BL8_DATA 0x00000200 // ZQ_ON_SREF_EXIT:RW:8:4:=0x02 ZQ_REQ:WR:0:4:=0x00
#define               DENALI_LPDDR2_CTL_89_400MHZ_CL6_BL8_DATA 0x00000040 // ZQ_INTERVAL:RW:0:32:=0x00000040
#define               DENALI_LPDDR2_CTL_90_400MHZ_CL6_BL8_DATA 0x00001400 // ZQRESET_F0:RW:8:12:=0x0014 ZQ_IN_PROGRESS:RD:0:1:=0x00
#define               DENALI_LPDDR2_CTL_91_400MHZ_CL6_BL8_DATA 0x01000007 // ZQCS_ROTATE:RW:24:1:=0x01 NO_ZQ_INIT:RW:16:1:=0x00 ZQRESET_F1:RW:0:12:=0x0007
#define               DENALI_LPDDR2_CTL_92_400MHZ_CL6_BL8_DATA 0x03030000 // ROW_DIFF_1:RW:24:3:=0x03 ROW_DIFF_0:RW:16:3:=0x03 BANK_DIFF_1:RW:8:2:=0x00 BANK_DIFF_0:RW:0:2:=0x00
#define               DENALI_LPDDR2_CTL_93_400MHZ_CL6_BL8_DATA 0x00000202 // CS_VAL_0:RW:16:16:=0x0000 COL_DIFF_1:RW:8:4:=0x02 COL_DIFF_0:RW:0:4:=0x02
#define               DENALI_LPDDR2_CTL_94_400MHZ_CL6_BL8_DATA 0x03ff0400 // CS_MSK_0:RW:16:16:=0x03ff CS_VAL_1:RW:0:16:=0x0400
#define               DENALI_LPDDR2_CTL_95_400MHZ_CL6_BL8_DATA 0x000003ff // RESERVED:RW:24:1:=0x00 RESERVED:RW:16:5:=0x00 CS_MSK_1:RW:0:16:=0x03ff
#define               DENALI_LPDDR2_CTL_96_400MHZ_CL6_BL8_DATA 0x01ffff0a // ADDR_CMP_EN:RW:24:1:=0x01 COMMAND_AGE_COUNT:RW:16:8:=0xff AGE_COUNT:RW:8:8:=0xff APREBIT:RW_D:0:4:=0x0a
#define               DENALI_LPDDR2_CTL_97_400MHZ_CL6_BL8_DATA 0x01010101 // PRIORITY_EN:RW:24:1:=0x01 PLACEMENT_EN:RW:16:1:=0x01 BANK_SPLIT_EN:RW:8:1:=0x01 RESERVED:RW:0:1:=0x01
#define               DENALI_LPDDR2_CTL_98_400MHZ_CL6_BL8_DATA 0x01010101 // W2R_SPLIT_EN:RW:24:1:=0x01 CS_SAME_EN:RW:16:1:=0x01 RW_SAME_PAGE_EN:RW:8:1:=0x01 RW_SAME_EN:RW:0:1:=0x01
#define               DENALI_LPDDR2_CTL_99_400MHZ_CL6_BL8_DATA 0x00010b01 // DISABLE_RD_INTERLEAVE:RW:24:1:=0x00 SWAP_EN:RW:16:1:=0x01 NUM_Q_ENTRIES_ACT_DISABLE:RW:8:4:=0x0b DISABLE_RW_GROUP_W_BNK_CONFLICT:RW:0:2:=0x01
#define              DENALI_LPDDR2_CTL_100_400MHZ_CL6_BL8_DATA 0x01000300 // LPDDR2_S4:RW:24:1:=0x01 REDUC:RW:16:1:=0x00 CS_MAP:RW:8:2:=0x03 INHIBIT_DRAM_CMD:RW:0:2:=0x00
#define              DENALI_LPDDR2_CTL_101_400MHZ_CL6_BL8_DATA 0x00000000 // CONTROLLER_BUSY:RD:24:1:=0x00 WR_ORDER_REQ:RW:16:2:=0x00 IN_ORDER_ACCEPT:RW:8:1:=0x00 Q_FULLNESS:RW:0:4:=0x00
#define              DENALI_LPDDR2_CTL_102_400MHZ_CL6_BL8_DATA 0x00000100 // DFI_ERROR:RD:16:5:=0x00 CTRLUPD_REQ_PER_AREF_EN:RW:8:1:=0x01 CTRLUPD_REQ:WR:0:1:=0x00
#define              DENALI_LPDDR2_CTL_103_400MHZ_CL6_BL8_DATA 0x00000000 // RESERVED:RW+:24:1:=0x00 DFI_ERROR_INFO:RD:0:20:=0x000000
#define              DENALI_LPDDR2_CTL_104_400MHZ_CL6_BL8_DATA 0x00000000 // INT_STATUS:RD:0:27:=0x00000000
#define              DENALI_LPDDR2_CTL_105_400MHZ_CL6_BL8_DATA 0x00000000 // INT_ACK:WR:0:26:=0x00000000
#define              DENALI_LPDDR2_CTL_106_400MHZ_CL6_BL8_DATA 0x00000000 // INT_MASK:RW:0:27:=0x00000000
#define              DENALI_LPDDR2_CTL_107_400MHZ_CL6_BL8_DATA 0x00000000 // OUT_OF_RANGE_ADDR:RD:0:34:=0x00000000
#define              DENALI_LPDDR2_CTL_108_400MHZ_CL6_BL8_DATA 0x00000000 // OUT_OF_RANGE_TYPE:RD:16:7:=0x00 OUT_OF_RANGE_LENGTH:RD:8:8:=0x00 OUT_OF_RANGE_ADDR:RD:0:34:=0x00
#define              DENALI_LPDDR2_CTL_109_400MHZ_CL6_BL8_DATA 0x00000000 // OUT_OF_RANGE_SOURCE_ID:RD:0:16:=0x0000
#define              DENALI_LPDDR2_CTL_110_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_EXP_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_111_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_EXP_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_112_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_EXP_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_113_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_EXP_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_114_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_FAIL_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_115_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_FAIL_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_116_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_FAIL_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_117_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_FAIL_DATA:RD:0:128:=0x00000000
#define              DENALI_LPDDR2_CTL_118_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_FAIL_ADDR:RD:0:34:=0x00000000
#define              DENALI_LPDDR2_CTL_119_400MHZ_CL6_BL8_DATA 0x00000000 // BIST_FAIL_ADDR:RD:0:34:=0x00
#define              DENALI_LPDDR2_CTL_120_400MHZ_CL6_BL8_DATA 0x00000000 // PORT_CMD_ERROR_ADDR:RD:0:34:=0x00000000
#define              DENALI_LPDDR2_CTL_121_400MHZ_CL6_BL8_DATA 0x00000000 // PORT_CMD_ERROR_TYPE:RD:24:2:=0x00 PORT_CMD_ERROR_ID:RD:8:16:=0x0000 PORT_CMD_ERROR_ADDR:RD:0:34:=0x00
#define              DENALI_LPDDR2_CTL_122_400MHZ_CL6_BL8_DATA 0x01020201 // W2W_DIFFCS_DLY_F0:RW_D:24:5:=0x01 W2R_DIFFCS_DLY_F0:RW_D:16:5:=0x02 R2W_DIFFCS_DLY_F0:RW_D:8:5:=0x02 R2R_DIFFCS_DLY_F0:RW_D:0:5:=0x01
#define              DENALI_LPDDR2_CTL_123_400MHZ_CL6_BL8_DATA 0x01020201 // W2W_DIFFCS_DLY_F1:RW_D:24:5:=0x01 W2R_DIFFCS_DLY_F1:RW_D:16:5:=0x02 R2W_DIFFCS_DLY_F1:RW_D:8:5:=0x02 R2R_DIFFCS_DLY_F1:RW_D:0:5:=0x01
#define              DENALI_LPDDR2_CTL_124_400MHZ_CL6_BL8_DATA 0x00020200 // W2R_SAMECS_DLY:RW:24:5:=0x00 R2W_SAMECS_DLY_F1:RW_D:16:5:=0x02 R2W_SAMECS_DLY_F0:RW_D:8:5:=0x02 R2R_SAMECS_DLY:RW:0:5:=0x00
#define              DENALI_LPDDR2_CTL_125_400MHZ_CL6_BL8_DATA 0x01010300 // TDQSCK_MAX_F1:RW:24:3:=0x01 TDQSCK_MIN_F0:RW:16:2:=0x01 TDQSCK_MAX_F0:RW:8:3:=0x03 W2W_SAMECS_DLY:RW:0:5:=0x00
#define              DENALI_LPDDR2_CTL_126_400MHZ_CL6_BL8_DATA 0x00000001 // SWLVL_START:WR:24:1:=0x00 SWLVL_LOAD:WR:16:1:=0x00 SW_LEVELING_MODE:RW:8:3:=0x00 TDQSCK_MIN_F1:RW:0:2:=0x01
#define              DENALI_LPDDR2_CTL_127_400MHZ_CL6_BL8_DATA 0x00000000 // SWLVL_RESP_1:RD:24:1:=0x00 SWLVL_RESP_0:RD:16:1:=0x00 SWLVL_OP_DONE:RD:8:1:=0x00 SWLVL_EXIT:WR:0:1:=0x00
#define              DENALI_LPDDR2_CTL_128_400MHZ_CL6_BL8_DATA 0x00000000 // RDLVL_GATE_REQ:WR:24:1:=0x00 RDLVL_REQ:WR:16:1:=0x00 SWLVL_RESP_3:RD:8:1:=0x00 SWLVL_RESP_2:RD:0:1:=0x00
#define              DENALI_LPDDR2_CTL_129_400MHZ_CL6_BL8_DATA 0x01000000 // DFI_PHY_RDLVL_MODE:RW:24:1:=0x01 RDLVL_GATE_SEQ_EN:RW:16:2:=0x00 RDLVL_SEQ_EN:RW:8:2:=0x00 RDLVL_CS:RW:0:1:=0x00
#define              DENALI_LPDDR2_CTL_130_400MHZ_CL6_BL8_DATA 0x00000001 // RDLVL_GATE_PERIODIC:RW:24:1:=0x00 RDLVL_ON_SREF_EXIT:RW:16:1:=0x00 RDLVL_PERIODIC:RW:8:1:=0x00 DFI_PHY_RDLVL_GATE_MODE:RW:0:1:=0x01
#define              DENALI_LPDDR2_CTL_131_400MHZ_CL6_BL8_DATA 0x00000100 // RDLVL_GATE_ROTATE:RW:24:1:=0x00 RDLVL_ROTATE:RW:16:1:=0x00 RDLVL_AREF_EN:RW:8:1:=0x01 RDLVL_GATE_ON_SREF_EXIT:RW:0:1:=0x00
#define              DENALI_LPDDR2_CTL_132_400MHZ_CL6_BL8_DATA 0x08000303 // AXI0_R_PRIORITY:RW:24:4:=0x08 AXI0_FIXED_PORT_PRIORITY_ENABLE:RW:16:1:=0x00 RDLVL_GATE_CS_MAP:RW:8:2:=0x03 RDLVL_CS_MAP:RW:0:2:=0x03
#define              DENALI_LPDDR2_CTL_133_400MHZ_CL6_BL8_DATA 0x00000008 // CKE_STATUS:RD:16:2:=0x00 AXI0_FIFO_TYPE_REG:RW:8:2:=0x00 AXI0_W_PRIORITY:RW:0:4:=0x08
#define              DENALI_LPDDR2_CTL_134_400MHZ_CL6_BL8_DATA 0x00000000 // TDFI_PHY_WRLAT:RD:24:5:=0x00 DLL_RST_ADJ_DLY:RW:16:8:=0x00 DLL_RST_DELAY:RW:0:16:=0x0000
#define              DENALI_LPDDR2_CTL_135_400MHZ_CL6_BL8_DATA 0x000f1100 // TDFI_RDDATA_EN:RD:24:5:=0x00 TDFI_PHY_RDLAT_F1:RW_D:16:5:=0x0f TDFI_PHY_RDLAT_F0:RW_D:8:5:=0x11 UPDATE_ERROR_STATUS:RD:0:7:=0x00
#define              DENALI_LPDDR2_CTL_136_400MHZ_CL6_BL8_DATA 0x18500500 // TDFI_CTRLUPD_MAX_F0:RW:16:16:=0x1850 TDFI_CTRLUPD_MIN:RW:8:4:=0x05 DRAM_CLK_DISABLE:RW:0:2:=0x00
#define              DENALI_LPDDR2_CTL_137_400MHZ_CL6_BL8_DATA 0x02000200 // TDFI_PHYUPD_TYPE1_F0:RW:16:16:=0x0200 TDFI_PHYUPD_TYPE0_F0:RW:0:16:=0x0200
#define              DENALI_LPDDR2_CTL_138_400MHZ_CL6_BL8_DATA 0x02000200 // TDFI_PHYUPD_TYPE3_F0:RW:16:16:=0x0200 TDFI_PHYUPD_TYPE2_F0:RW:0:16:=0x0200
#define              DENALI_LPDDR2_CTL_139_400MHZ_CL6_BL8_DATA 0x00001850 // TDFI_PHYUPD_RESP_F0:RW:0:16:=0x1850
#define              DENALI_LPDDR2_CTL_140_400MHZ_CL6_BL8_DATA 0x0000f320 // TDFI_CTRLUPD_INTERVAL_F0:RW:0:32:=0x0000f320
#define              DENALI_LPDDR2_CTL_141_400MHZ_CL6_BL8_DATA 0x078e0306 // TDFI_CTRLUPD_MAX_F1:RW:16:16:=0x078e WRLAT_ADJ_F0:RW:8:5:=0x03 RDLAT_ADJ_F0:RW:0:5:=0x06
#define              DENALI_LPDDR2_CTL_142_400MHZ_CL6_BL8_DATA 0x02000200 // TDFI_PHYUPD_TYPE1_F1:RW:16:16:=0x0200 TDFI_PHYUPD_TYPE0_F1:RW:0:16:=0x0200
#define              DENALI_LPDDR2_CTL_143_400MHZ_CL6_BL8_DATA 0x02000200 // TDFI_PHYUPD_TYPE3_F1:RW:16:16:=0x0200 TDFI_PHYUPD_TYPE2_F1:RW:0:16:=0x0200
#define              DENALI_LPDDR2_CTL_144_400MHZ_CL6_BL8_DATA 0x0000078e // TDFI_PHYUPD_RESP_F1:RW:0:16:=0x078e
#define              DENALI_LPDDR2_CTL_145_400MHZ_CL6_BL8_DATA 0x00004b8c // TDFI_CTRLUPD_INTERVAL_F1:RW:0:32:=0x00004b8c
#define              DENALI_LPDDR2_CTL_146_400MHZ_CL6_BL8_DATA 0x02020306 // TDFI_CTRL_DELAY_F1:RW_D:24:4:=0x02 TDFI_CTRL_DELAY_F0:RW_D:16:4:=0x02 WRLAT_ADJ_F1:RW:8:5:=0x03 RDLAT_ADJ_F1:RW:0:5:=0x06
#define              DENALI_LPDDR2_CTL_147_400MHZ_CL6_BL8_DATA 0x000a0506 // TDFI_RDLVL_EN:RW:16:8:=0x0a TDFI_DRAM_CLK_ENABLE:RW:8:4:=0x05 TDFI_DRAM_CLK_DISABLE:RW:0:4:=0x06
#define              DENALI_LPDDR2_CTL_148_400MHZ_CL6_BL8_DATA 0x0000000f // TDFI_RDLVL_RR:RW:0:10:=0x000f
#define              DENALI_LPDDR2_CTL_149_400MHZ_CL6_BL8_DATA 0x000007d0 // TDFI_RDLVL_RESP:RW:0:32:=0x000007d0
#define              DENALI_LPDDR2_CTL_150_400MHZ_CL6_BL8_DATA 0x00000000 // RDLVL_GATE_EN:RW:16:1:=0x00 RDLVL_EN:RW:8:1:=0x00 RDLVL_RESP_MASK:RW:0:4:=0x00
#define              DENALI_LPDDR2_CTL_151_400MHZ_CL6_BL8_DATA 0x00000000 // TDFI_RDLVL_MAX:RW:0:32:=0x00000000
#define              DENALI_LPDDR2_CTL_152_400MHZ_CL6_BL8_DATA 0x00000000 // RDLVL_INTERVAL:RW:8:16:=0x0000 RDLVL_ERROR_STATUS:RD:0:2:=0x00
#define              DENALI_LPDDR2_CTL_153_400MHZ_CL6_BL8_DATA 0x01010000 // TDFI_PHY_WRDATA_F1:RW:24:3:=0x01 TDFI_PHY_WRDATA_F0:RW:16:3:=0x01 RDLVL_GATE_INTERVAL:RW:0:16:=0x0000
#define              DENALI_LPDDR2_CTL_154_400MHZ_CL6_BL8_DATA 0x02050205 // TDFI_WRCSLAT_F1:RW:24:5:=0x02 TDFI_RDCSLAT_F1:RW:16:5:=0x05 TDFI_WRCSLAT_F0:RW:8:5:=0x02 TDFI_RDCSLAT_F0:RW:0:5:=0x05
#define              DENALI_LPDDR2_CTL_155_400MHZ_CL6_BL8_DATA 0x00000001 // EN_1T_TIMING:RW:0:1:=0x01

#endif
