
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
// Option: MHZ   : Simulation MHz                = 200
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
#ifndef __LPDDR_PHY_200_CL3_BL4_H__
#define __LPDDR_PHY_200_CL3_BL4_H__
#define               DENALI_LPDDR_PHY_00_200MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_0:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_01_200MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_0:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_02_200MHZ_CL3_BL4_DATA 0x80010090 // DEN_PHY_GATE_LPBK_CTRL_REG_0:RW:0:32:=0x80010090
#define               DENALI_LPDDR_PHY_03_200MHZ_CL3_BL4_DATA 0x00000033 // DEN_PHY_READ_CTRL_REG_0:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_04_200MHZ_CL3_BL4_DATA 0x08120057 // PHY_DLL_MASTER_CTRL_REG_0:RW:0:32:=0x08120057
#define               DENALI_LPDDR_PHY_05_200MHZ_CL3_BL4_DATA 0x00007070 // PHY_DLL_SLAVE_CTRL_REG_0:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_06_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_07_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_08_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_09_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_10_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_0:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_11_200MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_0:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_12_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_13_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_14_200MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_0:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_15_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_16_200MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_1:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_17_200MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_1:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_18_200MHZ_CL3_BL4_DATA 0x80010090 // DEN_PHY_GATE_LPBK_CTRL_REG_1:RW:0:32:=0x80010090
#define               DENALI_LPDDR_PHY_19_200MHZ_CL3_BL4_DATA 0x00000033 // DEN_PHY_READ_CTRL_REG_1:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_20_200MHZ_CL3_BL4_DATA 0x08120057 // PHY_DLL_MASTER_CTRL_REG_1:RW:0:32:=0x08120057
#define               DENALI_LPDDR_PHY_21_200MHZ_CL3_BL4_DATA 0x00007070 // PHY_DLL_SLAVE_CTRL_REG_1:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_22_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_23_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_24_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_25_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_26_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_1:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_27_200MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_1:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_28_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_29_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_30_200MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_1:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_31_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_32_200MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_2:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_33_200MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_2:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_34_200MHZ_CL3_BL4_DATA 0x80010090 // DEN_PHY_GATE_LPBK_CTRL_REG_2:RW:0:32:=0x80010090
#define               DENALI_LPDDR_PHY_35_200MHZ_CL3_BL4_DATA 0x00000033 // DEN_PHY_READ_CTRL_REG_2:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_36_200MHZ_CL3_BL4_DATA 0x08120057 // PHY_DLL_MASTER_CTRL_REG_2:RW:0:32:=0x08120057
#define               DENALI_LPDDR_PHY_37_200MHZ_CL3_BL4_DATA 0x00007070 // PHY_DLL_SLAVE_CTRL_REG_2:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_38_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_39_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_40_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_41_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_42_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_2:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_43_200MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_2:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_44_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_45_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_46_200MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_2:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_47_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_48_200MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_3:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_49_200MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_3:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_50_200MHZ_CL3_BL4_DATA 0x80010090 // DEN_PHY_GATE_LPBK_CTRL_REG_3:RW:0:32:=0x80010090
#define               DENALI_LPDDR_PHY_51_200MHZ_CL3_BL4_DATA 0x00000033 // DEN_PHY_READ_CTRL_REG_3:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_52_200MHZ_CL3_BL4_DATA 0x08120057 // PHY_DLL_MASTER_CTRL_REG_3:RW:0:32:=0x08120057
#define               DENALI_LPDDR_PHY_53_200MHZ_CL3_BL4_DATA 0x00007070 // PHY_DLL_SLAVE_CTRL_REG_3:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_54_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_55_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_56_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_57_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_58_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_3:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_59_200MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_3:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_60_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_61_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_62_200MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_3:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_63_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_64_200MHZ_CL3_BL4_DATA 0x04130414 // DEN_PHY_DQ_TIMING_REG_4:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_65_200MHZ_CL3_BL4_DATA 0x04150416 // DEN_PHY_DQS_TIMING_REG_4:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_66_200MHZ_CL3_BL4_DATA 0x80010090 // DEN_PHY_GATE_LPBK_CTRL_REG_4:RW:0:32:=0x80010090
#define               DENALI_LPDDR_PHY_67_200MHZ_CL3_BL4_DATA 0x00000033 // DEN_PHY_READ_CTRL_REG_4:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_68_200MHZ_CL3_BL4_DATA 0x08120057 // PHY_DLL_MASTER_CTRL_REG_4:RW:0:32:=0x08120057
#define               DENALI_LPDDR_PHY_69_200MHZ_CL3_BL4_DATA 0x00007070 // PHY_DLL_SLAVE_CTRL_REG_4:RW+:0:32:=0x00007040
#define               DENALI_LPDDR_PHY_70_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_71_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_72_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_1_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_73_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_DLL_OBS_REG_2_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_74_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_LVL_DBG_CONT_REG_4:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_75_200MHZ_CL3_BL4_DATA 0x0000818a // PHY_LVL_CONFIG_REG_4:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_76_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_GTLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_77_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_RDLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_78_200MHZ_CL3_BL4_DATA 0x4240f000 // PHY_WRLVL_CONFIG_REG_4:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_79_200MHZ_CL3_BL4_DATA 0x00000000 // PHY_WRLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_80_200MHZ_CL3_BL4_DATA 0x00106005 // DEN_PHY_CTRL_REG:RW:0:32:=0x00106005
#define               DENALI_LPDDR_PHY_81_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_CALVL_DEBUG_STEP_REG:WR:0:1:=0x00
#define               DENALI_LPDDR_PHY_82_200MHZ_CL3_BL4_DATA 0x02401338 // DEN_PHY_CALVL_CTRL_REG:RW:0:32:=0x02401338
#define               DENALI_LPDDR_PHY_83_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_CALVL_OBS_REG:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_84_200MHZ_CL3_BL4_DATA 0x00000000 // DEN_PHY_LP_WAKEUP_REG:RW:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_85_200MHZ_CL3_BL4_DATA 0x00010101 // DEN_PHY_PAD_TSEL_REG:RW:0:32:=0x00010101

#endif
