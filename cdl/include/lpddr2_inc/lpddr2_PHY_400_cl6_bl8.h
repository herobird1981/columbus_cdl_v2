
/* ****************************************************************
 *        CADENCE                    Copyright (c) 2001-2011      *
 *                                   Cadence Design Systems, Inc. *
 *                                   All rights reserved.         *
 *                                                                *
 *  This work may not be copied, modified, re-published,          *
 *  uploaded, executed, or distributed in any way, in any medium, *
 *  whether in whole or in part, without prior written permission *
 *  from Cadence Design Systems, Inc.                             *
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
 * editing may result in undetermined behavior, and may be in
 * violation of the Cadence Support Agreement.
 ******************************************************************
 ******************************************************************/
// REL: brite.anapass-Odysseus__PRELIMINARY__2013-11-19



// ********************************************************************
// Option: IP    : IP Mode                       = PHY
// Option: BL    : Burst Length                  = 8
// Option: CL    : CAS Latency                   = 6
// Option: MHZ   : Simulation MHz                = 400
// Option: AP    : Auto Precharge Mode     (0/1) = 0
// Option: DLLBP : DLL Bypass Mode         (0/1) = 0
// Option: HALF  : Half-Memory Support     (0/1) = 0
// Option: RDIMM : Registered Dimm Support (0/1) = 0
// Option: AL    : Additive Latency              = 0
// Option: RSV3  : Reserved                  (0) = 0
// Option: TCK   : Simulation period in ns       = 
// Option: SOMA  : Memory-SOMA file(s)           = memory.xml.sim
// ********************************************************************
// Memory: memory.xml.sim
// ********************************************************************
#ifndef __LPDDR2_PHY_400_CL6_BL8_H__
#define __LPDDR2_PHY_400_CL6_BL8_H__
#define               DENALI_LPDDR2_PHY_00_400MHZ_CL6_BL8_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_0:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_01_400MHZ_CL6_BL8_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_0:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_02_400MHZ_CL6_BL8_DATA 0xe0090091 // DEN_PHY_GATE_LPBK_CTRL_REG_0:RW:0:32:=0xe0090091
#define               DENALI_LPDDR2_PHY_03_400MHZ_CL6_BL8_DATA 0x00000066 // DEN_PHY_READ_CTRL_REG_0:RW:0:32:=0x00000066
#define               DENALI_LPDDR2_PHY_04_400MHZ_CL6_BL8_DATA 0x0e12002a // PHY_DLL_MASTER_CTRL_REG_0:RW:0:32:=0x0e12002a
#define               DENALI_LPDDR2_PHY_05_400MHZ_CL6_BL8_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_0:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_06_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_07_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_08_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_1_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_09_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_2_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_10_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_0:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_11_400MHZ_CL6_BL8_DATA 0x0000818a // PHY_LVL_CONFIG_REG_0:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_12_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_GTLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_13_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_RDLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_14_400MHZ_CL6_BL8_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_0:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_15_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_WRLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_16_400MHZ_CL6_BL8_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_1:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_17_400MHZ_CL6_BL8_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_1:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_18_400MHZ_CL6_BL8_DATA 0xe0090091 // DEN_PHY_GATE_LPBK_CTRL_REG_1:RW:0:32:=0xe0090091
#define               DENALI_LPDDR2_PHY_19_400MHZ_CL6_BL8_DATA 0x00000066 // DEN_PHY_READ_CTRL_REG_1:RW:0:32:=0x00000066
#define               DENALI_LPDDR2_PHY_20_400MHZ_CL6_BL8_DATA 0x0e12000a // PHY_DLL_MASTER_CTRL_REG_1:RW:0:32:=0x0e12002a
#define               DENALI_LPDDR2_PHY_21_400MHZ_CL6_BL8_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_1:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_22_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_23_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_24_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_1_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_25_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_2_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_26_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_1:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_27_400MHZ_CL6_BL8_DATA 0x0000818a // PHY_LVL_CONFIG_REG_1:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_28_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_GTLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_29_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_RDLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_30_400MHZ_CL6_BL8_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_1:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_31_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_WRLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_32_400MHZ_CL6_BL8_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_2:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_33_400MHZ_CL6_BL8_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_2:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_34_400MHZ_CL6_BL8_DATA 0xe0090091 // DEN_PHY_GATE_LPBK_CTRL_REG_2:RW:0:32:=0xe0090091
#define               DENALI_LPDDR2_PHY_35_400MHZ_CL6_BL8_DATA 0x00000066 // DEN_PHY_READ_CTRL_REG_2:RW:0:32:=0x00000066
#define               DENALI_LPDDR2_PHY_36_400MHZ_CL6_BL8_DATA 0x0e12000a // PHY_DLL_MASTER_CTRL_REG_2:RW:0:32:=0x0e12002a
#define               DENALI_LPDDR2_PHY_37_400MHZ_CL6_BL8_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_2:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_38_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_39_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_40_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_1_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_41_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_2_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_42_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_2:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_43_400MHZ_CL6_BL8_DATA 0x0000818a // PHY_LVL_CONFIG_REG_2:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_44_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_GTLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_45_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_RDLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_46_400MHZ_CL6_BL8_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_2:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_47_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_WRLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_48_400MHZ_CL6_BL8_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_3:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_49_400MHZ_CL6_BL8_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_3:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_50_400MHZ_CL6_BL8_DATA 0xe0090091 // DEN_PHY_GATE_LPBK_CTRL_REG_3:RW:0:32:=0xe0090091
#define               DENALI_LPDDR2_PHY_51_400MHZ_CL6_BL8_DATA 0x00000066 // DEN_PHY_READ_CTRL_REG_3:RW:0:32:=0x00000066
#define               DENALI_LPDDR2_PHY_52_400MHZ_CL6_BL8_DATA 0x0e12000a // PHY_DLL_MASTER_CTRL_REG_3:RW:0:32:=0x0e12002a
#define               DENALI_LPDDR2_PHY_53_400MHZ_CL6_BL8_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_3:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_54_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_55_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_56_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_1_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_57_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_2_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_58_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_3:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_59_400MHZ_CL6_BL8_DATA 0x0000818a // PHY_LVL_CONFIG_REG_3:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_60_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_GTLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_61_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_RDLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_62_400MHZ_CL6_BL8_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_3:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_63_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_WRLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_64_400MHZ_CL6_BL8_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_4:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_65_400MHZ_CL6_BL8_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_4:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_66_400MHZ_CL6_BL8_DATA 0xe0090091 // DEN_PHY_GATE_LPBK_CTRL_REG_4:RW:0:32:=0xe0090091
#define               DENALI_LPDDR2_PHY_67_400MHZ_CL6_BL8_DATA 0x00000066 // DEN_PHY_READ_CTRL_REG_4:RW:0:32:=0x00000066
#define               DENALI_LPDDR2_PHY_68_400MHZ_CL6_BL8_DATA 0x0e12000a // PHY_DLL_MASTER_CTRL_REG_4:RW:0:32:=0x0e12002a
#define               DENALI_LPDDR2_PHY_69_400MHZ_CL6_BL8_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_4:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_70_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_71_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_72_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_1_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_73_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_DLL_OBS_REG_2_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_74_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_4:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_75_400MHZ_CL6_BL8_DATA 0x0000818a // PHY_LVL_CONFIG_REG_4:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_76_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_GTLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_77_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_RDLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_78_400MHZ_CL6_BL8_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_4:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_79_400MHZ_CL6_BL8_DATA 0x00000000 // PHY_WRLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_80_400MHZ_CL6_BL8_DATA 0x00006928 // DEN_PHY_CTRL_REG:RW:0:32:=0x00006928
#define               DENALI_LPDDR2_PHY_81_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_CALVL_DEBUG_STEP_REG:WR:0:1:=0x00
#define               DENALI_LPDDR2_PHY_82_400MHZ_CL6_BL8_DATA 0x02401338 // DEN_PHY_CALVL_CTRL_REG:RW:0:32:=0x02401338
#define               DENALI_LPDDR2_PHY_83_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_CALVL_OBS_REG:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_84_400MHZ_CL6_BL8_DATA 0x00000000 // DEN_PHY_LP_WAKEUP_REG:RW:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_85_400MHZ_CL6_BL8_DATA 0x00010101 // DEN_PHY_PAD_TSEL_REG:RW:0:32:=0x00010101
#define               DENALI_LPDDR2_PHY_90_400MHZ_CL6_BL8_DATA 0x00000000

#endif
