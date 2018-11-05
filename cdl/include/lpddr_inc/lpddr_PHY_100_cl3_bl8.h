
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
// Option: CL    : CAS Latency                   = 3
// Option: MHZ   : Simulation MHz                = 100
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
#ifndef __LPDDR_PHY_100_CL3_BL8_H__
#define __LPDDR_PHY_100_CL3_BL8_H__
#define               DENALI_LPDDR_PHY_00_100MHZ_CL3_BL8_DATA  0x04130414 // DEN_PHY_DQ_TIMING_REG_0:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_01_100MHZ_CL3_BL8_DATA  0x04150416 // DEN_PHY_DQS_TIMING_REG_0:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_02_100MHZ_CL3_BL8_DATA  0x80010070 // DEN_PHY_GATE_LPBK_CTRL_REG_0:RW:0:32:=0x80010070
#define               DENALI_LPDDR_PHY_03_100MHZ_CL3_BL8_DATA  0x00000033 // DEN_PHY_READ_CTRL_REG_0:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_04_100MHZ_CL3_BL8_DATA  0x09120040 // PHY_DLL_MASTER_CTRL_REG_0:RW:0:32:=0x0912007f
#define               DENALI_LPDDR_PHY_05_100MHZ_CL3_BL8_DATA  0x00004040 // PHY_DLL_SLAVE_CTRL_REG_0:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_06_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_07_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_0_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_08_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_1_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_09_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_2_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_10_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_LVL_DBG_CONT_REG_0:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_11_100MHZ_CL3_BL8_DATA  0x0000818a // PHY_LVL_CONFIG_REG_0:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_12_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_GTLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_13_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_RDLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_14_100MHZ_CL3_BL8_DATA  0x4240f000 // PHY_WRLVL_CONFIG_REG_0:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_15_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_WRLVL_OBS_REG_0:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_16_100MHZ_CL3_BL8_DATA  0x04130414 // DEN_PHY_DQ_TIMING_REG_1:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_17_100MHZ_CL3_BL8_DATA  0x04150416 // DEN_PHY_DQS_TIMING_REG_1:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_18_100MHZ_CL3_BL8_DATA  0x80010070 // DEN_PHY_GATE_LPBK_CTRL_REG_1:RW:0:32:=0x80010070
#define               DENALI_LPDDR_PHY_19_100MHZ_CL3_BL8_DATA  0x00000033 // DEN_PHY_READ_CTRL_REG_1:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_20_100MHZ_CL3_BL8_DATA  0x09120040 // PHY_DLL_MASTER_CTRL_REG_1:RW:0:32:=0x0912007f
#define               DENALI_LPDDR_PHY_21_100MHZ_CL3_BL8_DATA  0x00004040 // PHY_DLL_SLAVE_CTRL_REG_1:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_22_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_23_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_0_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_24_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_1_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_25_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_2_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_26_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_LVL_DBG_CONT_REG_1:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_27_100MHZ_CL3_BL8_DATA  0x0000818a // PHY_LVL_CONFIG_REG_1:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_28_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_GTLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_29_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_RDLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_30_100MHZ_CL3_BL8_DATA  0x4240f000 // PHY_WRLVL_CONFIG_REG_1:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_31_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_WRLVL_OBS_REG_1:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_32_100MHZ_CL3_BL8_DATA  0x04130414 // DEN_PHY_DQ_TIMING_REG_2:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_33_100MHZ_CL3_BL8_DATA  0x04150416 // DEN_PHY_DQS_TIMING_REG_2:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_34_100MHZ_CL3_BL8_DATA  0x80010070 // DEN_PHY_GATE_LPBK_CTRL_REG_2:RW:0:32:=0x80010070
#define               DENALI_LPDDR_PHY_35_100MHZ_CL3_BL8_DATA  0x00000033 // DEN_PHY_READ_CTRL_REG_2:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_36_100MHZ_CL3_BL8_DATA  0x09120040 // PHY_DLL_MASTER_CTRL_REG_2:RW:0:32:=0x0912007f
#define               DENALI_LPDDR_PHY_37_100MHZ_CL3_BL8_DATA  0x00004040 // PHY_DLL_SLAVE_CTRL_REG_2:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_38_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_39_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_0_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_40_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_1_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_41_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_2_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_42_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_LVL_DBG_CONT_REG_2:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_43_100MHZ_CL3_BL8_DATA  0x0000818a // PHY_LVL_CONFIG_REG_2:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_44_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_GTLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_45_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_RDLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_46_100MHZ_CL3_BL8_DATA  0x4240f000 // PHY_WRLVL_CONFIG_REG_2:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_47_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_WRLVL_OBS_REG_2:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_48_100MHZ_CL3_BL8_DATA  0x04130414 // DEN_PHY_DQ_TIMING_REG_3:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_49_100MHZ_CL3_BL8_DATA  0x04150416 // DEN_PHY_DQS_TIMING_REG_3:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_50_100MHZ_CL3_BL8_DATA  0x80010070 // DEN_PHY_GATE_LPBK_CTRL_REG_3:RW:0:32:=0x80010070
#define               DENALI_LPDDR_PHY_51_100MHZ_CL3_BL8_DATA  0x00000033 // DEN_PHY_READ_CTRL_REG_3:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_52_100MHZ_CL3_BL8_DATA  0x09120040 // PHY_DLL_MASTER_CTRL_REG_3:RW:0:32:=0x0912007f
#define               DENALI_LPDDR_PHY_53_100MHZ_CL3_BL8_DATA  0x00004040 // PHY_DLL_SLAVE_CTRL_REG_3:RW+:0:32:=0x00004040
#define               DENALI_LPDDR_PHY_54_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_55_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_0_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_56_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_1_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_57_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_2_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_58_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_LVL_DBG_CONT_REG_3:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_59_100MHZ_CL3_BL8_DATA  0x0000818a // PHY_LVL_CONFIG_REG_3:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_60_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_GTLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_61_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_RDLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_62_100MHZ_CL3_BL8_DATA  0x4240f000 // PHY_WRLVL_CONFIG_REG_3:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_63_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_WRLVL_OBS_REG_3:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_64_100MHZ_CL3_BL8_DATA  0x04130414 // DEN_PHY_DQ_TIMING_REG_4:RW:0:32:=0x04130413
#define               DENALI_LPDDR_PHY_65_100MHZ_CL3_BL8_DATA  0x04150416 // DEN_PHY_DQS_TIMING_REG_4:RW:0:32:=0x04150415
#define               DENALI_LPDDR_PHY_66_100MHZ_CL3_BL8_DATA  0x80010070 // DEN_PHY_GATE_LPBK_CTRL_REG_4:RW:0:32:=0x80010070
#define               DENALI_LPDDR_PHY_67_100MHZ_CL3_BL8_DATA  0x00000033 // DEN_PHY_READ_CTRL_REG_4:RW:0:32:=0x00000033
#define               DENALI_LPDDR_PHY_68_100MHZ_CL3_BL8_DATA  0x09120040 // PHY_DLL_MASTER_CTRL_REG_4:RW:0:32:=0x0912007f
#define               DENALI_LPDDR_PHY_69_100MHZ_CL3_BL8_DATA  0x00007040 // PHY_DLL_SLAVE_CTRL_REG_4:RW+:0:32:=0x00007040
#define               DENALI_LPDDR_PHY_70_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_71_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_0_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_72_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_1_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_73_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_DLL_OBS_REG_2_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_74_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_LVL_DBG_CONT_REG_4:WR:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_75_100MHZ_CL3_BL8_DATA  0x0000818a // PHY_LVL_CONFIG_REG_4:RW:0:32:=0x0000818a
#define               DENALI_LPDDR_PHY_76_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_GTLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_77_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_RDLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_78_100MHZ_CL3_BL8_DATA  0x4240f000 // PHY_WRLVL_CONFIG_REG_4:RW:0:32:=0x4240f000
#define               DENALI_LPDDR_PHY_79_100MHZ_CL3_BL8_DATA  0x00000000 // PHY_WRLVL_OBS_REG_4:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_80_100MHZ_CL3_BL8_DATA  0x00106005 // DEN_PHY_CTRL_REG:RW:0:32:=0x00106005
#define               DENALI_LPDDR_PHY_81_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_CALVL_DEBUG_STEP_REG:WR:0:1:=0x00
#define               DENALI_LPDDR_PHY_82_100MHZ_CL3_BL8_DATA  0x02401338 // DEN_PHY_CALVL_CTRL_REG:RW:0:32:=0x02401338
#define               DENALI_LPDDR_PHY_83_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_CALVL_OBS_REG:RD:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_84_100MHZ_CL3_BL8_DATA  0x00000000 // DEN_PHY_LP_WAKEUP_REG:RW:0:32:=0x00000000
#define               DENALI_LPDDR_PHY_85_100MHZ_CL3_BL8_DATA  0x00010101 // DEN_PHY_PAD_TSEL_REG:RW:0:32:=0x00010101

#endif

