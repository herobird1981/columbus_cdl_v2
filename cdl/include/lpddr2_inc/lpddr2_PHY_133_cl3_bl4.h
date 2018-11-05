
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
// Option: BL    : Burst Length                  = 4
// Option: CL    : CAS Latency                   = 3
// Option: MHZ   : Simulation MHz                = 133
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
#ifndef __LPDDR2_PHY_133_CL3_BL4_H__
#define __LPDDR2_PHY_133_CL3_BL4_H__
#define               DENALI_LPDDR2_PHY_00_133MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_0:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_01_133MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_0:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_02_133MHZ_CL3_BL4_DATA 0x80090069 // DEN_PHY_GATE_LPBK_CTRL_REG_0:RW:0:32:=0x80090069
#define               DENALI_LPDDR2_PHY_03_133MHZ_CL3_BL4_DATA 0x00000044 // DEN_PHY_READ_CTRL_REG_0:RW:0:32:=0x00000044
#define               DENALI_LPDDR2_PHY_04_133MHZ_CL3_BL4_DATA 0x0b12007f // PHY_DLL_MASTER_CTRL_REG_0:RW:0:32:=0x0b12007f
#define               DENALI_LPDDR2_PHY_05_133MHZ_CL3_BL4_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_0:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_06_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_07_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_08_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_09_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_10_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_0:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_11_133MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_0:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_12_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_13_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_14_133MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_0:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_15_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_16_133MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_1:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_17_133MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_1:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_18_133MHZ_CL3_BL4_DATA 0x80090069 // DEN_PHY_GATE_LPBK_CTRL_REG_1:RW:0:32:=0x80090069
#define               DENALI_LPDDR2_PHY_19_133MHZ_CL3_BL4_DATA 0x00000044 // DEN_PHY_READ_CTRL_REG_1:RW:0:32:=0x00000044
#define               DENALI_LPDDR2_PHY_20_133MHZ_CL3_BL4_DATA 0x0b12007f // PHY_DLL_MASTER_CTRL_REG_1:RW:0:32:=0x0b12007f
#define               DENALI_LPDDR2_PHY_21_133MHZ_CL3_BL4_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_1:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_22_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_23_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_24_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_25_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_26_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_1:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_27_133MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_1:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_28_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_29_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_30_133MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_1:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_31_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_32_133MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_2:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_33_133MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_2:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_34_133MHZ_CL3_BL4_DATA 0x80090069 // DEN_PHY_GATE_LPBK_CTRL_REG_2:RW:0:32:=0x80090069
#define               DENALI_LPDDR2_PHY_35_133MHZ_CL3_BL4_DATA 0x00000044 // DEN_PHY_READ_CTRL_REG_2:RW:0:32:=0x00000044
#define               DENALI_LPDDR2_PHY_36_133MHZ_CL3_BL4_DATA 0x0b12007f // PHY_DLL_MASTER_CTRL_REG_2:RW:0:32:=0x0b12007f
#define               DENALI_LPDDR2_PHY_37_133MHZ_CL3_BL4_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_2:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_38_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_39_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_40_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_41_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_42_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_2:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_43_133MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_2:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_44_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_45_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_46_133MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_2:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_47_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_48_133MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_3:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_49_133MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_3:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_50_133MHZ_CL3_BL4_DATA 0x80090069 // DEN_PHY_GATE_LPBK_CTRL_REG_3:RW:0:32:=0x80090069
#define               DENALI_LPDDR2_PHY_51_133MHZ_CL3_BL4_DATA 0x00000044 // DEN_PHY_READ_CTRL_REG_3:RW:0:32:=0x00000044
#define               DENALI_LPDDR2_PHY_52_133MHZ_CL3_BL4_DATA 0x0b12007f // PHY_DLL_MASTER_CTRL_REG_3:RW:0:32:=0x0b12007f
#define               DENALI_LPDDR2_PHY_53_133MHZ_CL3_BL4_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_3:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_54_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_55_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_56_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_57_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_58_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_3:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_59_133MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_3:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_60_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_61_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_62_133MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_3:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_63_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_64_133MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_4:RW:0:32:=0x04130413
#define               DENALI_LPDDR2_PHY_65_133MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_4:RW:0:32:=0x04150415
#define               DENALI_LPDDR2_PHY_66_133MHZ_CL3_BL4_DATA 0x80090069 // DEN_PHY_GATE_LPBK_CTRL_REG_4:RW:0:32:=0x80090069
#define               DENALI_LPDDR2_PHY_67_133MHZ_CL3_BL4_DATA 0x00000044 // DEN_PHY_READ_CTRL_REG_4:RW:0:32:=0x00000044
#define               DENALI_LPDDR2_PHY_68_133MHZ_CL3_BL4_DATA 0x0b12007f // PHY_DLL_MASTER_CTRL_REG_4:RW:0:32:=0x0b12007f
#define               DENALI_LPDDR2_PHY_69_133MHZ_CL3_BL4_DATA 0x00004040 // PHY_DLL_SLAVE_CTRL_REG_4:RW+:0:32:=0x00004040
#define               DENALI_LPDDR2_PHY_70_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_71_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_72_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_73_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_74_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_4:WR:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_75_133MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_4:RW:0:32:=0x0000818a
#define               DENALI_LPDDR2_PHY_76_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_77_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_78_133MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_4:RW:0:32:=0x4240f000
#define               DENALI_LPDDR2_PHY_79_133MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_80_133MHZ_CL3_BL4_DATA 0x00106926 // DEN_PHY_CTRL_REG:RW:0:32:=0x00106926
#define               DENALI_LPDDR2_PHY_81_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_CALVL_DEBUG_STEP_REG:WR:0:1:=0x00
#define               DENALI_LPDDR2_PHY_82_133MHZ_CL3_BL4_DATA 0x02401338 // DEN_PHY_CALVL_CTRL_REG:RW:0:32:=0x02401338
#define               DENALI_LPDDR2_PHY_83_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_CALVL_OBS_REG:RD:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_84_133MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_LP_WAKEUP_REG:RW:0:32:=0x00000000
#define               DENALI_LPDDR2_PHY_85_133MHZ_CL3_BL4_DATA 0x00010101 // DEN_PHY_PAD_TSEL_REG:RW:0:32:=0x00010101
#define               DENALI_LPDDR2_PHY_90_133MHZ_CL3_BL4_DATA 0x00000000

#endif
