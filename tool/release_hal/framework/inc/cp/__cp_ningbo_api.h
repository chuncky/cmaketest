/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef     _NINGBO_H_
#define     _NINGBO_H_
/***********************************************************************
* FileName: Ningbo.h
*
* Descrition:
*       The Ningbo PMIC register defines.
*
* Author  : Joshua Wang.
* Version : 1.0.0
* Data    : 04/01/2012
*
* Others:
*
* History:
*   1. <Version: 1.0.0>
*       Data        :  01/04/2012.
*       Author      :  zhen Han.
*       Modification:  First drift.
*
*   2.
*
*************************************************************************/

/*************************************************************************
*               Included Files.
**************************************************************************/
#include "plat_types.h"

#ifndef TRUE
  #define FALSE   0
  #define TRUE	  1
#endif	/* TRUE */

#define NINGBO_DEBUG 0

#if NINGBO_DEBUG
#define NINGBO_UART_DEBUG(fmt,...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define NINGBO_UART_DEBUG(fmt,...)
#endif
#if 0
typedef signed char     INT8;         /* Signed 8-bit quantity          */
typedef signed short    INT16;        /* Signed 16-bit quantity         */
typedef signed long     INT32;        /* Signed 32-bit quantity         */
typedef unsigned char   UINT8;        /* Unsigned 8-bit quantity        */
typedef unsigned short  UINT16;       /* Unsigned 16-bit quantity       */
typedef unsigned long   UINT32;       /* Unsigned 32-bit quantity       */
typedef unsigned long   BU_U32;       /* Unsigned 32-bit quantity       */

/* Standard typedefs */
typedef unsigned char   BOOL;         /* Boolean                        */
#endif
/***************************************************************************
*                       Typedefines  & Macros
****************************************************************************/
/* Ningbo Registers defines.                        */
/* (1)NINGBO BASE REGISTER                          */

#define     NINGBO_ID_REG                       0x00            /* Identification Register.                         */

#define     NINGBO_STATUS_REG1                  0x01
#define     NINGBO_ONKEY_STATUS_BIT	            (0x01<<0)
#define     NINGBO_EXTON1_STATUS_BIT	        (0x01<<1)
#define     NINGBO_EXTON2_STATUS_BIT	        (0x01<<2)
#define     NINGBO_VBAT_STATUS_BIT	            (0x01<<3)
#define     NINGBO_VBUS_UVLO_BIT	            (0x01<<4)
#define     NINGBO_VBUS_OVP_BIT 	            (0x01<<5)
#define     NINGBO_VBUS_STATUS_BIT	            (0x01<<6)

#define     NINGBO_STATUS_REG2                  0x02
#define     NINGBO_RTC_ALARM_STATUS_BIT	        (0x01<<7)

#define     NINGBO_INT_STATUS_REG1              0x05            /* Interrupt Status Register1.                      */
#define     NINGBO_INT_STATUS_REG2              0x06            /* Interrupt Status Register2.                      */
#define     NINGBO_INT_ENABLE_REG1              0x09            /* Interrupt Enable Register1.                      */
#define     NINGBO_INT_ENABLE_REG2              0x0A            /* Interrupt Enable Register2.                      */
#define     NINGBO_MISC_CFG_REG1                0x0D            /* Misc config Register1.                                */
#define     NINGBO_RESET_PMIC_BIT               (0x01<<6)
#define     NINGBO_SW_PDOWN_BIT	                (0x01<<5)
#define     NINGBO_MISC_CFG_REG2                0x0E            /* Misc config Register2.                                */
#define     NINGBO_PWRUP_LOG_REG             	0x10            /* Power up log register.                     */
#define     NINGBO_WD_TIMER_REG             	0x11            /* WD config register.                     */
#define     NINGBO_WD_TIMER_SLP_BITS	(0x1<<4|0x1<<5)
#define     NINGBO_WD_TIMER_ACT_BITS	(0x1<<0|0x1<<1|0x1<<2|0x1<<3)

#define NINGBO_WD_TIMER_8s       (0x3<<0)
#define NINGBO_WD_TIMER_16s      (0x4<<0)
#define NINGBO_WD_TIMER_32s      (0x5<<0)
#define NINGBO_WD_TIMER_64s      (0x7<<0)
#define NINGBO_WD_TIMER_256s     (0xB<<0)

#define     NINGBO_BAT_TEMP_CTRL_REG           	0x15            /* Battery temperature control register.                     */
#define     NINGBO_WD_REG                       0x1D            /* Watchdog Register                                */
#define     NINGBO_WD_DIS                       0x01            /* Watchdog disable.                                */
#define     NINGBO_RTC_CTRL_REG	                0xD0            /* RTC_Control>.                                */
#define     NINGBO_RTC_ALARM_WU_BIT	            (0x01<<4)
#define     NINGBO_RTC_ALARM_BIT	            (0x01<<5)
#define     NINGBO_RTC_USE_XO_BIT	            (0x01<<7)
#define     NINGBO_RTC_ALARM_SET1_BIT           (0x01<<0)
#define     NINGBO_RTC_ALARM_SET2_BIT           (0x01<<1)

#define     NINGBO_RTC_USER_DATA_0_REG          0xF3            /* User defined region */
#define     NINGBO_RTC_USER_DATA_1_REG          0xF4
#define     NINGBO_RTC_USER_DATA_2_REG          0xF5
#define     NINGBO_RTC_USER_DATA_3_REG          0xF6
#define     NINGBO_RTC_USER_DATA_4_REG          0xF7            /* NOTE to set bit 5 for A3. */

#define     NINGBO_READWRITE_DMY_1_REG          0x50            /* User defined region */
#define     NINGBO_READWRITE_DMY_2_REG          0x51
#define     NINGBO_READWRITE_DMY_3_REG          0x52
#define     NINGBO_READWRITE_DMY_4_REG          0x53
#define     NINGBO_RW_DMY_VBUS_DET_INT_SEL      (7)
#define     NINGBO_RW_DMY_DIS_VBUS_UV           (6)

#define     NINGBO_READONLY_DATA_1_REG          0x58            /* User defined region */
#define     NINGBO_READONLY_DATA_2_REG          0x59
    #define     NINGBO_RONLY_DATA_CHG_CV            (0x01<<7)
    #define     NINGBO_RONLY_DATA_CHG_MCV           (0x01<<6)
    #define     NINGBO_RONLY_DATA_VBUS_OV           (0x01<<5)
    #define     NINGBO_RONLY_DATA_VBUS_UV           (0x01<<4)
    #define     NINGBO_RONLY_DATA_CHG_OK            (0x01<<3)
#define     NINGBO_READONLY_DATA_3_REG          0x5A
    #define     NINGBO_RONLY_DATA_CHG_TERM          0
#define     NINGBO_READONLY_DATA_4_REG          0x5B
#define     NINGBO_CHG_FSM_OUT_SHIFT            5
#define     NINGBO_CHG_FSM_OUT_MSK              (0x7<<NINGBO_CHG_FSM_OUT_SHIFT)
            /* 0 -- Shutdown;       1 -- Check;
               2 -- Trickle Charge; 3 -- Pre-Charge;
               4 -- CC-Charge;      5 -- CV-Charge;
               6 -- Termination;    7 -- Fault.  */
#define     NINGBO_CHG_FSM_OUT_TERM             0x6


#define     NINGBO_RTC_COUNT_REG1               0xD1            /* RTC_COUNTER<0:7>.                                */
#define     NINGBO_RTC_COUNT_REG2               0xD2            /* RTC_COUNTER<8:15>.                               */
#define     NINGBO_RTC_COUNT_REG3               0xD3            /* RTC_COUNTER<16:23>.                              */
#define     NINGBO_RTC_COUNT_REG4               0xD4            /* RTC_COUNTER<24:31>.                              */
#define     NINGBO_RTC_EXPIRE1_REG1             0xD5            /* RTC_EXPIRE1<0:7>.                                */
#define     NINGBO_RTC_EXPIRE1_REG2             0xD6            /* RTC_EXPIRE1<8:15>.                               */
#define     NINGBO_RTC_EXPIRE1_REG3             0xD7            /* RTC_EXPIRE1<16:23>.                              */
#define     NINGBO_RTC_EXPIRE1_REG4             0xD8            /* RTC_EXPIRE1<24:31>.                              */
#define     NINGBO_RTC_EXPIRE2_REG1             0xDD            /* RTC_EXPIRE2<0:7>.                                */
#define     NINGBO_RTC_EXPIRE2_REG2             0xDE            /* RTC_EXPIRE2<8:15>.                               */
#define     NINGBO_RTC_EXPIRE2_REG3             0xDF            /* RTC_EXPIRE2<16:23>.                              */
#define     NINGBO_RTC_EXPIRE2_REG4             0xE0            /* RTC_EXPIRE2<24:31>.                              */
#define     NINGBO_RTC_MISC_3_REG               0xE3            /* No actual register name, just name it as is.     */

#define     NINGBO_CLK_32K_SEL_REG				0xE4
#define     NINGBO_CLK_32K_SEL					0x04
#define     NINGBO_LONGKEY_1	                0
#define     NINGBO_LONGKEY_2	                1
#define     NINGBO_LONGKEY_EN1                  (0x01<<NINGBO_LONGKEY_1)             /* When enabled, the event will cause power-down.   */
#define     NINGBO_LONGKEY_EN2                  (0x01<<NINGBO_LONGKEY_2)             /* When enabled, the event will cause power-down.   */

#define     NINGBO_POWERDOWN_LOG_REG	        0xE5
#define     NINGBO_POWERDOWN_LOG_REG2	        0xE6
#define     NINGBO_RTC_MISC_5_REG               0xE7
#define     NINGBO_RTC_FAULT_WU_EN_BIT           (0x01<<0)
#define     NINGBO_RTC_FAULT_WU_BIT              (0x01<<3)

#define     NINGBO_CRYSTAL_CAP_SET_REG          0xF0
#define     NINGBO_RTC_CONTROL_REG				0xF1
#define     NINGBO_VBAT_4P4_EN                  (0x1<<6)

#define     NINGBO_RTC_USE_XO                   0x04            /* 32kHz clock and buffer clock generated form XO.  */
#define     NINGBO_CRYSTAL_CAP_20PF				0x60
#define     NINGBO_CRYSTAL_CAP_25PF				0x80
#define     NINGBO_CRYSTAL_CAP_30PF				0xA0
#define     NINGBO_CRYSTAL_CAP_35PF				0xC0
#define NINGBO_RTC_SYS_OFFSET_REG NINGBO_RTC_USER_DATA_0_REG
#define NINGBO_RTC_APP_OFFSET_REG NINGBO_RTC_EXPIRE2_REG1  //reuse RTC_EXPIRE_2 as APP_OFFSET

#define     NINGBO_BAT_DETECT                   0x08            /* Battery is present.                              */
#define     NINGBO_EXTON2_DETECT                0x04            /* EXTON2_DETECT = 1.                               */
#define     NINGBO_EXTON1_DETECT                0x02            /* EXTON1_DETECT = 1.                               */
#define     NINGBO_ONKEY_PRESSED                0x01            /* ONKEYn is Pressed.                               */


#define     NINGBO_BAT_INT_OCCURRED             0x08            /* Set,when BAT_DET changes.                        */
#define     NINGBO_EXTON2_INT_OCCURRED          0x04            /* Set,when EXTON2 stable.                          */
#define     NINGBO_EXTON1_INT_OCCURRED          0x02            /* Set,when EXTON1 charged.                         */
#define     NINGBO_ONKEY_INT_OCCURRED           0x01            /* Set,when ONKEY changed.                          */


#define     NINGBO_VINLDO_INT_OCCURRED          0x08            /* Set,when GPADC2 cross theshold.                  */
#define     NINGBO_GPADC1_INT_OCCERRED          0x04            /* Set,when GPADC1 cross threshold.                 */
#define     NINGBO_GPADC0_INT_OCCURRED          0x02            /* Set,when GPADC0 cross threshold.                 */
#define     NINGBO_TINT_OCCURRED                0x01            /* Set,when GPADC8 cross theshold.                  */


#define     NINGBO_INT_STATUS_REG3              0x07            /* Interrupt Status Register3.                      */
#define     NINGBO_INT_STATUS_REG4              0x08            /* Interrupt Status Register4.                      */

#define     NINGBO_RTC_INT_EN                   0x80            /* RTC Interrupt Enable.                            */
#define     NINGBO_BAT_INT_EN                   0x08            /* BAT Interrupt Enable.                            */
#define     NINGBO_EXTON2_INT_EN                0x04            /* EXTON2 Interrupt Enable.                         */
#define     NINGBO_EXTON1_INT_EN                0x02            /* EXTON1 Interrupt Enable.                         */
#define     NINGBO_ONKEY_INT_EN                 0x01            /* ONKEY Interrupt Enable.                          */

#define     NINGBO_VINLDO_INT_EN                0x08            /* VINLDO Interrupt Enalbe.                         */
#define     NINGBO_GPADC1_INT_EN                0x04            /* GPADC1 Interrupt Enable.                         */
#define     NINGBO_GPADC0_INT_EN                0x02            /* GPADC0 Interrupt Enable.                         */
#define     NINGBO_TINT_INT_EN                  0x01            /* TINT Interrupt Enable.                           */

#define     NINGBO_POWER_HOLD                   0x80            /* LPF & DVC enable, SLEEPn disable.                */
#define     NINGBO_RESET_PMIC_REG               0x40            /* Reset Ustica registers.                          */
#define     NINGBO_SW_PDOWN                     0x20            /* Entrance to 'power-down' sate.                   */
#define     NINGBO_WD_RESET                     0x10            /* Resets the Watchdog timer.                       */
//#define     NINGBO_LONGKEY_RESET                0x08            /* Resets the LONGKEY press timer.                  */
#define     NINGBO_WD_MODE                      0x01            /* WD1#->toggle RESET_OUTn,WD2#->PowerDown.         */

/*********************** Charger module begins**************************/
#define     NINGBO_CHG_CTRL_MISC1_REG           0x20
#define     NINGBO_CHG_CTRL_CURRENT1_REG        0x21
#define     NINGBO_CHG_PRE_CUR_SHIFT            2
#define     NINGBO_CHG_TERM_CUR_SHIFT           0
#define     NINGBO_CHG_PRE_CUR_MASK             (0x3<<NINGBO_CHG_PRE_CUR_SHIFT)
#define     NINGBO_CHG_TERM_CUR_MASK            (0x3<<NINGBO_CHG_TERM_CUR_SHIFT)

#define     NINGBO_CHG_CTRL_MISC2_REG           0x22
#define     NINGBO_CHG_CTRL_CURRENT2_REG        0x23
#define     NINGBO_CHG_MAX_CURRENT_SHIFT        4
#define     NINGBO_CHG_CURRENT_SHIFT            0
#define     NINGBO_CHG_MAX_CURRENT_MASK         (0xf<<NINGBO_CHG_MAX_CURRENT_SHIFT)
#define     NINGBO_CHG_CURRENT_MASK             (0xf<<NINGBO_CHG_CURRENT_SHIFT)

#define     NINGBO_CHG_CTRL_TIMER1_REG          0x24
#define     NINGBO_CHG_CTRL_TIMER_CHK_VOL_SHIFT (6)
#define     NINGBO_CHG_CTRL_TIMER_CHK_CUR_SHIFT (4)
#define     NINGBO_CHG_CTRL_TIMER_TRI_CHG_SHIFT (2)
#define     NINGBO_CHG_CTRL_TIMER_PRE_CHG_SHIFT (0)
#define     NINGBO_CHG_CTRL_TIMER_CHK_VOL_MASK  (0x3<<NINGBO_CHG_CTRL_TIMER_CHK_VOL_SHIFT)
#define     NINGBO_CHG_CTRL_TIMER_CHK_CUR_MASK  (0x3<<NINGBO_CHG_CTRL_TIMER_CHK_CUR_SHIFT)
#define     NINGBO_CHG_CTRL_TIMER_TRI_CHG_MASK  (0x3<<NINGBO_CHG_CTRL_TIMER_TRI_CHG_SHIFT)
#define     NINGBO_CHG_CTRL_TIMER_PRE_CHG_MASK  (0x3<<NINGBO_CHG_CTRL_TIMER_PRE_CHG_SHIFT)

#define     NINGBO_CHG_CTRL_TIMER2_REG          0x25
#define     NINGBO_CHG_CTRL_TIMER_CCCV_SHIFT    (6)
#define     NINGBO_CHG_CTRL_BAT_VOLT_SHIFT      (4)
#define     NINGBO_CHG_CTRL_TIMERKEEP_SHIFT     (3)
#define     NINGBO_CHG_CTRL_DRV_SEGMENT_EN      (2)
#define     NINGBO_CHG_CTRL_TIMER_CCCV_MASK     (0x3<<NINGBO_CHG_CTRL_TIMER_CCCV_SHIFT)
#define     NINGBO_CHG_CTRL_BAT_VOLT_MASK       (0x3<<NINGBO_CHG_CTRL_BAT_VOLT_SHIFT)
#define     NINGBO_CHG_CTRL_TIMERKEEP_MASK      (1<<NINGBO_CHG_CTRL_TIMERKEEP_SHIFT)

#define     NINGBO_CHG_CTRL_MISC3_REG           0x26
#define     NINGBO_CHG_CTRL_SWITCH_SHIFT        3
#define     NINGBO_CHG_CTRL_SWITCH_ONOFF        (1<<NINGBO_CHG_CTRL_SWITCH_SHIFT)
#define     NINGBO_CHG_CTRL_MISC4_REG           0x27

/*********************** Charger module ends**************************/

//#define     USTICA_USERDATA_REG1                0xEA             /* User Data Field<23:16>.                          */

/* (3)Ningbo GPADC Registers.                 */
#define     NINGBO_GPADC_MEAS_EN_REG1           0x01            /* GPADC Measurement Enable Register1.          */
#define     NINGBO_GPADC_MEAS_EN_REG2           0x02            /* GPADC Measurement Enable Register2.          */
#define     NINGBO_GPADC_MEAS_EN_REG3           0x03            /* GPADC Measurement Enable Register3.          */
#define     NINGBO_GPADC_MEAS_EN_REG4           0x04            /* GPADC Measurement Enable Register4.          */

#define     NINGBO_GPADC_MODE_CONTROL_REG       0x06
#define     NINGBO_GPADC_SW_TRIG_BIT            (0x01 << 2)
#define     NINGBO_NON_STOP                     (0x01 << 1)            /* Enable NON_STOP GPADC operation mode.         */
#define     NINGBO_GPADC_EN                     (0x01 << 0)            /* Enable the GPADC periodic modes.              */

#define     NINGBO_GPADC_BIAS_1_REG             0x0B            /* gpadc1/0 */
#define     NINGBO_GPADC_0_BIAS_SHIFT           0
#define     NINGBO_GPADC_1_BIAS_SHIFT           4

#define     NINGBO_GPADC_BIAS_2_REG             0x0C            /* gpadc3/2 */
#define     NINGBO_GPADC_2_BIAS_SHIFT           0
#define     NINGBO_GPADC_3_BIAS_SHIFT           4

#define     NINGBO_GPADC_BIAS_ENABLE            0x0D
#define     NINGBO_GPADC_0_BIAS_OUT              (1 << 0)
#define     NINGBO_GPADC_1_BIAS_OUT              (1 << 1)
#define     NINGBO_GPADC_2_BIAS_OUT              (1 << 2)
#define     NINGBO_GPADC_3_BIAS_OUT              (1 << 3)


#define     NINGBO_TINT_LOW_TH_REG              0x13             /* Low threshold for TINT interrupt.    */
#define     NINGBO_TINT_UPP_TH_REG              0x15             /* Upp threshold for TINT interrupt.    */

#define     NINGBO_GPADC1_LOW_TH                0x1B             /* Low threshold for GPADC1 interrupt.    */
#define     NINGBO_GPADC1_UPP_TH                0x1D             /* Upp threshold for GPADC1 interrupt.    */
#define     NINGBO_VINLDO_LOW_TH_REG            0x1F             /* Low threshold for VINLDO interrupt.    */
#define     NINGBO_VINLDO_UPP_TH_REG            0x21             /* Upp threshold for VINLDO interrupt.    */

#define     NINGBO_BAT_TEMP_60D_REG1            0x27             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_60D_REG2            0x28             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_47D_REG1            0x29             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_47D_REG2            0x2A             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_43D_REG1            0x2B             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_43D_REG2            0x2C             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_12D_REG1            0x2D             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_12D_REG2            0x2E             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_8D_REG1             0x2F             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_8D_REG2             0x30             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_5D_REG1             0x31             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_5D_REG2             0x32             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_0D_REG1             0x33             /* Battery temperature values for comparison.    */
#define     NINGBO_BAT_TEMP_0D_REG2             0x34             /* Battery temperature values for comparison.    */

#define     NINGBO_GPADC_MEAS_VBUCKLDO_1_REG     0x40
#define     NINGBO_GPADC_MEAS_VBUCKLDO_2_REG     0x41
#define     NINGBO_GPADC_MEAS_VCHG_1_REG         0x42
#define     NINGBO_GPADC_MEAS_VCHG_2_REG         0x43
#define     NINGBO_GPADC_MEAS_TINT_1_REG         0x44
#define     NINGBO_GPADC_MEAS_TINT_2_REG         0x45
#define     NINGBO_GPADC_MEAS_VBATID_1_REG       0x46
#define     NINGBO_GPADC_MEAS_VBATID_2_REG       0x47
#define     NINGBO_GPADC_MEAS_VBATTEMP_1_REG     0x48
#define     NINGBO_GPADC_MEAS_VBATTEMP_2_REG     0x49

#define     NINGBO_VGPADC0                      0x4C
#define     NINGBO_VGPADC1                      0x4E
#define     NINGBO_VGPADC1_MAX                  0x50
#define     NINGBO_VGPADC1_MIN                  0x52
#define     NINGBO_VGPADC1_AVE                  0x54

#define     NINGBO_VINLDO_REG                  0x62
#define     NINGBO_VINLDO_MAX_REG              0x64
#define     NINGBO_VINLDO_MIN_REG              0x66
#define     NINGBO_VINLDO_AVE_REG              0x68
#define     NINGBO_VINLDO_SLP_REG              0x6A
#define     NINGBO_VINLDO_SLP_MAX_REG          0x6C
#define     NINGBO_VINLDO_SLP_MIN_REG          0x6E
#define     NINGBO_VINLDO_SLP_AVE_REG          0x70


//LDO
#define     NINGBO_LDO1_ACTIVE_VOUT_REG         0x71
#define     NINGBO_LDO2_ACTIVE_VOUT_REG         0x74
#define     NINGBO_LDO3_ACTIVE_VOUT_REG         0x77
#define     NINGBO_LDO4_ACTIVE_VOUT_REG         0x7A
#define     NINGBO_LDO5_ACTIVE_VOUT_REG         0x7D
#define     NINGBO_LDO6_ACTIVE_VOUT_REG         0x80
#define     NINGBO_LDO7_ACTIVE_VOUT_REG         0x83
#define     NINGBO_LDO8_ACTIVE_VOUT_REG         0x86
#define     NINGBO_LDO9_ACTIVE_VOUT_REG         0x89
#define     NINGBO_LDO10_ACTIVE_VOUT_REG        0x8C
#define     NINGBO_LDO11_ACTIVE_VOUT_REG        0x8F
#define     NINGBO_LDO12_ACTIVE_VOUT_REG        0x92
#define     NINGBO_LDO13_ACTIVE_VOUT_REG        0x95

#define     NINGBO_LDO1_SLEEP_VOUT_REG          0x70
#define     NINGBO_LDO2_SLEEP_VOUT_REG          0x73
#define     NINGBO_LDO3_SLEEP_VOUT_REG          0x76
#define     NINGBO_LDO4_SLEEP_VOUT_REG          0x79
#define     NINGBO_LDO5_SLEEP_VOUT_REG          0x7C
#define     NINGBO_LDO6_SLEEP_VOUT_REG          0x7F
#define     NINGBO_LDO7_SLEEP_VOUT_REG          0x82
#define     NINGBO_LDO8_SLEEP_VOUT_REG          0x85
#define     NINGBO_LDO9_SLEEP_VOUT_REG          0x88
#define     NINGBO_LDO10_SLEEP_VOUT_REG         0x8B
#define     NINGBO_LDO11_SLEEP_VOUT_REG         0x8E
#define     NINGBO_LDO12_SLEEP_VOUT_REG         0x91
#define     NINGBO_LDO13_SLEEP_VOUT_REG         0x94

#define     NINGBO_LDO1_ENABLE_REG			NINGBO_LDO1_ACTIVE_VOUT_REG
#define     NINGBO_LDO2_ENABLE_REG			NINGBO_LDO2_ACTIVE_VOUT_REG
#define     NINGBO_LDO3_ENABLE_REG			NINGBO_LDO3_ACTIVE_VOUT_REG
#define     NINGBO_LDO4_ENABLE_REG			NINGBO_LDO4_ACTIVE_VOUT_REG
#define     NINGBO_LDO5_ENABLE_REG			NINGBO_LDO5_ACTIVE_VOUT_REG
#define     NINGBO_LDO6_ENABLE_REG			NINGBO_LDO6_ACTIVE_VOUT_REG
#define     NINGBO_LDO7_ENABLE_REG			NINGBO_LDO7_ACTIVE_VOUT_REG
#define     NINGBO_LDO8_ENABLE_REG			NINGBO_LDO8_ACTIVE_VOUT_REG
#define     NINGBO_LDO9_ENABLE_REG			NINGBO_LDO9_ACTIVE_VOUT_REG
#define     NINGBO_LDO10_ENABLE_REG			NINGBO_LDO10_ACTIVE_VOUT_REG
#define     NINGBO_LDO11_ENABLE_REG			NINGBO_LDO11_ACTIVE_VOUT_REG
#define     NINGBO_LDO12_ENABLE_REG			NINGBO_LDO12_ACTIVE_VOUT_REG
#define     NINGBO_LDO13_ENABLE_REG			NINGBO_LDO13_ACTIVE_VOUT_REG

#define     NINGBO_LDO1_SLEEP_MODE_REG		NINGBO_LDO1_SLEEP_VOUT_REG
#define     NINGBO_LDO2_SLEEP_MODE_REG		NINGBO_LDO2_SLEEP_VOUT_REG
#define     NINGBO_LDO3_SLEEP_MODE_REG		NINGBO_LDO3_SLEEP_VOUT_REG
#define     NINGBO_LDO4_SLEEP_MODE_REG		NINGBO_LDO4_SLEEP_VOUT_REG
#define     NINGBO_LDO5_SLEEP_MODE_REG		NINGBO_LDO5_SLEEP_VOUT_REG
#define     NINGBO_LDO6_SLEEP_MODE_REG		NINGBO_LDO6_SLEEP_VOUT_REG
#define     NINGBO_LDO7_SLEEP_MODE_REG		NINGBO_LDO7_SLEEP_VOUT_REG
#define     NINGBO_LDO8_SLEEP_MODE_REG		NINGBO_LDO8_SLEEP_VOUT_REG
#define     NINGBO_LDO9_SLEEP_MODE_REG		NINGBO_LDO9_SLEEP_VOUT_REG
#define     NINGBO_LDO10_SLEEP_MODE_REG		NINGBO_LDO10_SLEEP_VOUT_REG
#define     NINGBO_LDO11_SLEEP_MODE_REG		NINGBO_LDO11_SLEEP_VOUT_REG
#define     NINGBO_LDO12_SLEEP_MODE_REG		NINGBO_LDO12_SLEEP_VOUT_REG
#define     NINGBO_LDO13_SLEEP_MODE_REG		NINGBO_LDO13_SLEEP_VOUT_REG

//VBUCK
//step 0.125v, from 0.6v to 1.6v
#define	    NINGBO_CONTAIN_VBUCK_ACTIVE_VOUT_MASK	(0x7f)
#define     NINGBO_CONTAIN_VBUCK_SLEEP_VOUT_MASK	(0x7f)

#define     NINGBO_VBUCK_0V6000	                    (0x00)
#define     NINGBO_VBUCK_0V6125	                    (NINGBO_VBUCK_0V6000+1)
#define     NINGBO_VBUCK_0V6250	                    (NINGBO_VBUCK_0V6000+2)
#define     NINGBO_VBUCK_0V6375	                    (NINGBO_VBUCK_0V6000+3)
#define     NINGBO_VBUCK_0V6500	                    (NINGBO_VBUCK_0V6000+4)
#define     NINGBO_VBUCK_0V6625	                    (NINGBO_VBUCK_0V6000+5)
#define     NINGBO_VBUCK_0V6750	                    (NINGBO_VBUCK_0V6000+6)
#define     NINGBO_VBUCK_0V6875	                    (NINGBO_VBUCK_0V6000+7)
#define     NINGBO_VBUCK_0V7000                     (NINGBO_VBUCK_0V6000+8)
#define     NINGBO_VBUCK_0V7125                     (NINGBO_VBUCK_0V6000+9)
#define     NINGBO_VBUCK_0V7250                     (NINGBO_VBUCK_0V6000+10)
#define     NINGBO_VBUCK_0V7375                     (NINGBO_VBUCK_0V6000+11)
#define     NINGBO_VBUCK_0V7500                     (NINGBO_VBUCK_0V6000+12)
#define     NINGBO_VBUCK_0V7625                     (NINGBO_VBUCK_0V6000+13)
#define     NINGBO_VBUCK_0V7750                     (NINGBO_VBUCK_0V6000+14)
#define     NINGBO_VBUCK_0V7875                     (NINGBO_VBUCK_0V6000+15)
#define     NINGBO_VBUCK_0V8000                     (NINGBO_VBUCK_0V6000+16)
#define     NINGBO_VBUCK_0V8125                     (NINGBO_VBUCK_0V6000+17)
#define     NINGBO_VBUCK_0V8250                     (NINGBO_VBUCK_0V6000+18)
#define     NINGBO_VBUCK_0V8375                     (NINGBO_VBUCK_0V6000+19)
#define     NINGBO_VBUCK_0V8500                     (NINGBO_VBUCK_0V6000+20)
#define     NINGBO_VBUCK_0V8625                     (NINGBO_VBUCK_0V6000+21)
#define     NINGBO_VBUCK_0V8750                     (NINGBO_VBUCK_0V6000+22)
#define     NINGBO_VBUCK_0V8875                     (NINGBO_VBUCK_0V6000+23)
#define     NINGBO_VBUCK_0V9000                     (NINGBO_VBUCK_0V6000+24)
#define     NINGBO_VBUCK_0V9125                     (NINGBO_VBUCK_0V6000+25)
#define     NINGBO_VBUCK_0V9250                     (NINGBO_VBUCK_0V6000+26)
#define     NINGBO_VBUCK_0V9375                     (NINGBO_VBUCK_0V6000+27)
#define     NINGBO_VBUCK_0V9500                     (NINGBO_VBUCK_0V6000+28)
#define     NINGBO_VBUCK_0V9625                     (NINGBO_VBUCK_0V6000+29)
#define     NINGBO_VBUCK_0V9750                     (NINGBO_VBUCK_0V6000+30)
#define     NINGBO_VBUCK_0V9875                     (NINGBO_VBUCK_0V6000+31)
#define     NINGBO_VBUCK_1V0000                     (NINGBO_VBUCK_0V6000+32)
#define     NINGBO_VBUCK_1V0125                     (NINGBO_VBUCK_0V6000+33)
#define     NINGBO_VBUCK_1V0250                     (NINGBO_VBUCK_0V6000+34)
#define     NINGBO_VBUCK_1V0375                     (NINGBO_VBUCK_0V6000+35)
#define     NINGBO_VBUCK_1V0500                     (NINGBO_VBUCK_0V6000+36)
#define     NINGBO_VBUCK_1V0625                     (NINGBO_VBUCK_0V6000+37)
#define     NINGBO_VBUCK_1V0750                     (NINGBO_VBUCK_0V6000+38)
#define     NINGBO_VBUCK_1V0875                     (NINGBO_VBUCK_0V6000+39)
#define     NINGBO_VBUCK_1V1000                     (NINGBO_VBUCK_0V6000+40)
#define     NINGBO_VBUCK_1V1125                     (NINGBO_VBUCK_0V6000+41)
#define     NINGBO_VBUCK_1V1250                     (NINGBO_VBUCK_0V6000+42)
#define     NINGBO_VBUCK_1V1375                     (NINGBO_VBUCK_0V6000+43)
#define     NINGBO_VBUCK_1V1500                     (NINGBO_VBUCK_0V6000+44)
#define     NINGBO_VBUCK_1V1625                     (NINGBO_VBUCK_0V6000+45)
#define     NINGBO_VBUCK_1V1750                     (NINGBO_VBUCK_0V6000+46)
#define     NINGBO_VBUCK_1V1875                     (NINGBO_VBUCK_0V6000+47)
#define     NINGBO_VBUCK_1V2000                     (NINGBO_VBUCK_0V6000+48)
#define     NINGBO_VBUCK_1V2125                     (NINGBO_VBUCK_0V6000+49)
#define     NINGBO_VBUCK_1V2250                     (NINGBO_VBUCK_0V6000+50)
#define     NINGBO_VBUCK_1V2375                     (NINGBO_VBUCK_0V6000+51)
#define     NINGBO_VBUCK_1V2500                     (NINGBO_VBUCK_0V6000+52)
#define     NINGBO_VBUCK_1V2625                     (NINGBO_VBUCK_0V6000+53)
#define     NINGBO_VBUCK_1V2750                     (NINGBO_VBUCK_0V6000+54)
#define     NINGBO_VBUCK_1V2875                     (NINGBO_VBUCK_0V6000+55)
#define     NINGBO_VBUCK_1V3000                     (NINGBO_VBUCK_0V6000+56)
#define     NINGBO_VBUCK_1V3125                     (NINGBO_VBUCK_0V6000+57)
#define     NINGBO_VBUCK_1V3250                     (NINGBO_VBUCK_0V6000+58)
#define     NINGBO_VBUCK_1V3375                     (NINGBO_VBUCK_0V6000+59)
#define     NINGBO_VBUCK_1V3500                     (NINGBO_VBUCK_0V6000+60)
#define     NINGBO_VBUCK_1V3625                     (NINGBO_VBUCK_0V6000+61)
#define     NINGBO_VBUCK_1V3750                     (NINGBO_VBUCK_0V6000+62)
#define     NINGBO_VBUCK_1V3875                     (NINGBO_VBUCK_0V6000+63)
#define     NINGBO_VBUCK_1V4000                     (NINGBO_VBUCK_0V6000+64)
#define     NINGBO_VBUCK_1V4125                     (NINGBO_VBUCK_0V6000+65)
#define     NINGBO_VBUCK_1V4250                     (NINGBO_VBUCK_0V6000+66)
#define     NINGBO_VBUCK_1V4375                     (NINGBO_VBUCK_0V6000+67)
#define     NINGBO_VBUCK_1V4500                     (NINGBO_VBUCK_0V6000+68)
#define     NINGBO_VBUCK_1V4625                     (NINGBO_VBUCK_0V6000+69)
#define     NINGBO_VBUCK_1V4750                     (NINGBO_VBUCK_0V6000+70)
#define     NINGBO_VBUCK_1V4875                     (NINGBO_VBUCK_0V6000+71)
#define     NINGBO_VBUCK_1V5000                     (NINGBO_VBUCK_0V6000+72)
#define     NINGBO_VBUCK_1V5125                     (NINGBO_VBUCK_0V6000+73)
#define     NINGBO_VBUCK_1V5250                     (NINGBO_VBUCK_0V6000+74)
#define     NINGBO_VBUCK_1V5375                     (NINGBO_VBUCK_0V6000+75)
#define     NINGBO_VBUCK_1V5500                     (NINGBO_VBUCK_0V6000+76)
#define     NINGBO_VBUCK_1V5625                     (NINGBO_VBUCK_0V6000+77)
#define     NINGBO_VBUCK_1V5750                     (NINGBO_VBUCK_0V6000+78)
#define     NINGBO_VBUCK_1V5875                     (NINGBO_VBUCK_0V6000+79)
#define     NINGBO_VBUCK_1V6000                     (NINGBO_VBUCK_0V6000+80)
//step 0.05v, from 1.65v to 3.95v
#define     NINGBO_VBUCK_1V65                       (NINGBO_VBUCK_0V6000+81)
#define     NINGBO_VBUCK_1V70                       (NINGBO_VBUCK_0V6000+82)
#define     NINGBO_VBUCK_1V75                       (NINGBO_VBUCK_0V6000+83)
#define     NINGBO_VBUCK_1V80                       (NINGBO_VBUCK_0V6000+84)
#define     NINGBO_VBUCK_1V85                       (NINGBO_VBUCK_0V6000+85)
#define     NINGBO_VBUCK_1V90                       (NINGBO_VBUCK_0V6000+86)
#define     NINGBO_VBUCK_1V95                       (NINGBO_VBUCK_0V6000+87)
#define     NINGBO_VBUCK_2V00                       (NINGBO_VBUCK_0V6000+88)
#define     NINGBO_VBUCK_2V05                       (NINGBO_VBUCK_0V6000+89)
#define     NINGBO_VBUCK_2V10                       (NINGBO_VBUCK_0V6000+90)
#define     NINGBO_VBUCK_2V15                       (NINGBO_VBUCK_0V6000+91)
#define     NINGBO_VBUCK_2V20                       (NINGBO_VBUCK_0V6000+92)
#define     NINGBO_VBUCK_2V25                       (NINGBO_VBUCK_0V6000+93)
#define     NINGBO_VBUCK_2V30                       (NINGBO_VBUCK_0V6000+94)
#define     NINGBO_VBUCK_2V35                       (NINGBO_VBUCK_0V6000+95)
#define     NINGBO_VBUCK_2V40                       (NINGBO_VBUCK_0V6000+96)
#define     NINGBO_VBUCK_2V45                       (NINGBO_VBUCK_0V6000+97)
#define     NINGBO_VBUCK_2V50                       (NINGBO_VBUCK_0V6000+98)
#define     NINGBO_VBUCK_2V55                       (NINGBO_VBUCK_0V6000+99)
#define     NINGBO_VBUCK_2V60                       (NINGBO_VBUCK_0V6000+100)
#define     NINGBO_VBUCK_2V65                       (NINGBO_VBUCK_0V6000+101)
#define     NINGBO_VBUCK_2V70                       (NINGBO_VBUCK_0V6000+102)
#define     NINGBO_VBUCK_2V75                       (NINGBO_VBUCK_0V6000+103)
#define     NINGBO_VBUCK_2V80                       (NINGBO_VBUCK_0V6000+104)
#define     NINGBO_VBUCK_2V85                       (NINGBO_VBUCK_0V6000+105)
#define     NINGBO_VBUCK_2V90                       (NINGBO_VBUCK_0V6000+106)
#define     NINGBO_VBUCK_2V95                       (NINGBO_VBUCK_0V6000+107)
#define     NINGBO_VBUCK_3V00                       (NINGBO_VBUCK_0V6000+108)
#define     NINGBO_VBUCK_3V05                       (NINGBO_VBUCK_0V6000+109)
#define     NINGBO_VBUCK_3V10                       (NINGBO_VBUCK_0V6000+110)
#define     NINGBO_VBUCK_3V15                       (NINGBO_VBUCK_0V6000+111)
#define     NINGBO_VBUCK_3V20                       (NINGBO_VBUCK_0V6000+112)
#define     NINGBO_VBUCK_3V25                       (NINGBO_VBUCK_0V6000+113)
#define     NINGBO_VBUCK_3V30                       (NINGBO_VBUCK_0V6000+114)
#define     NINGBO_VBUCK_3V35                       (NINGBO_VBUCK_0V6000+115)
#define     NINGBO_VBUCK_3V40                       (NINGBO_VBUCK_0V6000+116)
#define     NINGBO_VBUCK_3V45                       (NINGBO_VBUCK_0V6000+117)
#define     NINGBO_VBUCK_3V50                       (NINGBO_VBUCK_0V6000+118)
#define     NINGBO_VBUCK_3V55                       (NINGBO_VBUCK_0V6000+119)
#define     NINGBO_VBUCK_3V60                       (NINGBO_VBUCK_0V6000+120)
#define     NINGBO_VBUCK_3V65                       (NINGBO_VBUCK_0V6000+121)
#define     NINGBO_VBUCK_3V70                       (NINGBO_VBUCK_0V6000+122)
#define     NINGBO_VBUCK_3V75                       (NINGBO_VBUCK_0V6000+123)
#define     NINGBO_VBUCK_3V80                       (NINGBO_VBUCK_0V6000+124)
#define     NINGBO_VBUCK_3V85                       (NINGBO_VBUCK_0V6000+125)
#define     NINGBO_VBUCK_3V90                       (NINGBO_VBUCK_0V6000+126)
#define     NINGBO_VBUCK_3V95                       (NINGBO_VBUCK_0V6000+127)

//LDO NORMAL mode
//LDO1
#define     NINGBO_LDO_ACTIVE_VOUT_MASK             (0xf << 2)//BIT[5:2] for vout of Active mode 

#define     NINGBO_LDO1_ACTIVE_1V20                (0x0 << 2)
#define     NINGBO_LDO1_ACTIVE_1V25                (0x1 << 2)
#define     NINGBO_LDO1_ACTIVE_1V70                (0x2 << 2)
#define     NINGBO_LDO1_ACTIVE_1V80                (0x3 << 2)
#define     NINGBO_LDO1_ACTIVE_1V85                (0x4 << 2)
#define     NINGBO_LDO1_ACTIVE_1V90                (0x5 << 2)
#define     NINGBO_LDO1_ACTIVE_2V50                (0x6 << 2)
#define     NINGBO_LDO1_ACTIVE_2V60                (0x7 << 2)
#define     NINGBO_LDO1_ACTIVE_2V70                (0x8 << 2)
#define     NINGBO_LDO1_ACTIVE_2V75                (0x9 << 2)
#define     NINGBO_LDO1_ACTIVE_2V80                (0xa << 2)
#define     NINGBO_LDO1_ACTIVE_2V85                (0xb << 2)
#define     NINGBO_LDO1_ACTIVE_2V90                (0xc << 2)
#define     NINGBO_LDO1_ACTIVE_3V00                (0xd << 2)
#define     NINGBO_LDO1_ACTIVE_3V10                (0xe << 2)
#define     NINGBO_LDO1_ACTIVE_3V30                (0xf << 2)
//LDO2
#define     NINGBO_LDO2_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO2_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO2_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO2_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO2_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO2_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO2_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO2_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO2_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO2_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO2_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO2_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO2_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO2_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO2_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO2_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30
//LDO3
#define     NINGBO_LDO3_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO3_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO3_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO3_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO3_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO3_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO3_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO3_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO3_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO3_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO3_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO3_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO3_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO3_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO3_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO3_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30
//LDO4
#define     NINGBO_LDO4_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO4_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO4_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO4_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO4_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO4_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO4_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO4_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO4_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO4_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO4_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO4_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO4_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO4_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO4_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO4_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO6
#define     NINGBO_LDO6_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO6_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO6_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO6_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO6_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO6_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO6_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO6_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO6_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO6_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO6_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO6_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO6_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO6_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO6_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO6_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO7
#define     NINGBO_LDO7_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO7_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO7_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO7_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO7_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO7_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO7_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO7_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO7_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO7_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO7_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO7_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO7_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO7_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO7_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO7_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO8
#define     NINGBO_LDO8_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO8_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO8_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO8_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO8_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO8_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO8_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO8_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO8_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO8_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO8_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO8_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO8_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO8_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO8_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO8_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO9
#define     NINGBO_LDO9_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO9_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO9_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO9_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO9_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO9_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO9_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO9_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO9_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO9_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO9_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO9_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO9_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO9_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO9_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO9_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO10
#define     NINGBO_LDO10_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO10_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO10_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO10_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO10_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO10_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO10_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO10_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO10_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO10_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO10_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO10_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO10_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO10_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO10_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO10_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO11
#define     NINGBO_LDO11_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO11_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO11_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO11_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO11_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO11_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO11_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO11_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO11_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO11_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO11_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO11_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO11_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO11_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO11_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO11_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO12
#define     NINGBO_LDO12_ACTIVE_1V20                NINGBO_LDO1_ACTIVE_1V20
#define     NINGBO_LDO12_ACTIVE_1V25                NINGBO_LDO1_ACTIVE_1V25
#define     NINGBO_LDO12_ACTIVE_1V70                NINGBO_LDO1_ACTIVE_1V70
#define     NINGBO_LDO12_ACTIVE_1V80                NINGBO_LDO1_ACTIVE_1V80
#define     NINGBO_LDO12_ACTIVE_1V85                NINGBO_LDO1_ACTIVE_1V85
#define     NINGBO_LDO12_ACTIVE_1V90                NINGBO_LDO1_ACTIVE_1V90
#define     NINGBO_LDO12_ACTIVE_2V50                NINGBO_LDO1_ACTIVE_2V50
#define     NINGBO_LDO12_ACTIVE_2V60                NINGBO_LDO1_ACTIVE_2V60
#define     NINGBO_LDO12_ACTIVE_2V70                NINGBO_LDO1_ACTIVE_2V70
#define     NINGBO_LDO12_ACTIVE_2V75                NINGBO_LDO1_ACTIVE_2V75
#define     NINGBO_LDO12_ACTIVE_2V80                NINGBO_LDO1_ACTIVE_2V80
#define     NINGBO_LDO12_ACTIVE_2V85                NINGBO_LDO1_ACTIVE_2V85
#define     NINGBO_LDO12_ACTIVE_2V90                NINGBO_LDO1_ACTIVE_2V90
#define     NINGBO_LDO12_ACTIVE_3V00                NINGBO_LDO1_ACTIVE_3V00
#define     NINGBO_LDO12_ACTIVE_3V10                NINGBO_LDO1_ACTIVE_3V10
#define     NINGBO_LDO12_ACTIVE_3V30                NINGBO_LDO1_ACTIVE_3V30

//LDO5
#define     NINGBO_LDO5_ACTIVE_1V70               (0x0 << 2)
#define     NINGBO_LDO5_ACTIVE_1V80               (0x1 << 2)
#define     NINGBO_LDO5_ACTIVE_1V90               (0x2 << 2)
#define     NINGBO_LDO5_ACTIVE_2V50               (0x3 << 2)
#define     NINGBO_LDO5_ACTIVE_2V80               (0x4 << 2)
#define     NINGBO_LDO5_ACTIVE_2V90               (0x5 << 2)
#define     NINGBO_LDO5_ACTIVE_3V10               (0x6 << 2)
#define     NINGBO_LDO5_ACTIVE_3V30               (0x7 << 2)

//LDO13
#define     NINGBO_LDO13_ACTIVE_1V70               NINGBO_LDO5_ACTIVE_1V70
#define     NINGBO_LDO13_ACTIVE_1V80               NINGBO_LDO5_ACTIVE_1V80
#define     NINGBO_LDO13_ACTIVE_1V90               NINGBO_LDO5_ACTIVE_1V90
#define     NINGBO_LDO13_ACTIVE_2V50               NINGBO_LDO5_ACTIVE_2V50
#define     NINGBO_LDO13_ACTIVE_2V80               NINGBO_LDO5_ACTIVE_2V80
#define     NINGBO_LDO13_ACTIVE_2V90               NINGBO_LDO5_ACTIVE_2V90
#define     NINGBO_LDO13_ACTIVE_3V10               NINGBO_LDO5_ACTIVE_3V10
#define     NINGBO_LDO13_ACTIVE_3V30               NINGBO_LDO5_ACTIVE_3V30

//SLEEP mode
//LDO1
#define     NINGBO_LDO_SLEEP_VOUT_MASK             (0xf)	//BIT[3:0] ldo vout for sleep mode

#define     NINGBO_LDO1_SLEEP_1V20                (0x0)
#define     NINGBO_LDO1_SLEEP_1V25                (0x1)
#define     NINGBO_LDO1_SLEEP_1V70                (0x2)
#define     NINGBO_LDO1_SLEEP_1V80                (0x3)
#define     NINGBO_LDO1_SLEEP_1V85                (0x4)
#define     NINGBO_LDO1_SLEEP_1V90                (0x5)
#define     NINGBO_LDO1_SLEEP_2V50                (0x6)
#define     NINGBO_LDO1_SLEEP_2V60                (0x7)
#define     NINGBO_LDO1_SLEEP_2V70                (0x8)
#define     NINGBO_LDO1_SLEEP_2V75                (0x9)
#define     NINGBO_LDO1_SLEEP_2V80                (0xa)
#define     NINGBO_LDO1_SLEEP_2V85                (0xb)
#define     NINGBO_LDO1_SLEEP_2V90                (0xc)
#define     NINGBO_LDO1_SLEEP_3V00                (0xd)
#define     NINGBO_LDO1_SLEEP_3V10                (0xe)
#define     NINGBO_LDO1_SLEEP_3V30                (0xf)
//LDO2
#define     NINGBO_LDO2_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO2_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO2_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO2_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO2_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO2_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO2_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO2_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO2_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO2_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO2_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO2_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO2_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO2_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO2_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO2_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO3
#define     NINGBO_LDO3_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO3_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO3_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO3_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO3_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO3_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO3_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO3_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO3_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO3_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO3_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO3_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO3_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO3_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO3_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO3_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO4
#define     NINGBO_LDO4_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO4_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO4_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO4_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO4_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO4_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO4_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO4_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO4_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO4_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO4_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO4_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO4_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO4_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO4_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO4_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO6
#define     NINGBO_LDO6_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO6_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO6_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO6_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO6_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO6_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO6_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO6_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO6_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO6_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO6_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO6_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO6_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO6_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO6_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO6_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO7
#define     NINGBO_LDO7_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO7_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO7_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO7_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO7_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO7_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO7_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO7_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO7_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO7_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO7_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO7_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO7_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO7_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO7_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO7_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO8
#define     NINGBO_LDO8_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO8_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO8_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO8_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO8_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO8_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO8_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO8_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO8_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO8_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO8_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO8_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO8_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO8_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO8_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO8_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO9
#define     NINGBO_LDO9_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO9_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO9_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO9_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO9_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO9_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO9_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO9_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO9_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO9_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO9_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO9_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO9_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO9_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO9_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO9_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO10
#define     NINGBO_LDO10_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO10_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO10_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO10_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO10_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO10_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO10_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO10_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO10_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO10_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO10_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO10_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO10_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO10_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO10_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO10_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO11
#define     NINGBO_LDO11_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO11_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO11_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO11_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO11_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO11_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO11_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO11_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO11_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO11_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO11_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO11_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO11_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO11_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO11_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO11_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30
//LDO12
#define     NINGBO_LDO12_SLEEP_1V20                NINGBO_LDO1_SLEEP_1V20
#define     NINGBO_LDO12_SLEEP_1V25                NINGBO_LDO1_SLEEP_1V25
#define     NINGBO_LDO12_SLEEP_1V70                NINGBO_LDO1_SLEEP_1V70
#define     NINGBO_LDO12_SLEEP_1V80                NINGBO_LDO1_SLEEP_1V80
#define     NINGBO_LDO12_SLEEP_1V85                NINGBO_LDO1_SLEEP_1V85
#define     NINGBO_LDO12_SLEEP_1V90                NINGBO_LDO1_SLEEP_1V90
#define     NINGBO_LDO12_SLEEP_2V50                NINGBO_LDO1_SLEEP_2V50
#define     NINGBO_LDO12_SLEEP_2V60                NINGBO_LDO1_SLEEP_2V60
#define     NINGBO_LDO12_SLEEP_2V70                NINGBO_LDO1_SLEEP_2V70
#define     NINGBO_LDO12_SLEEP_2V75                NINGBO_LDO1_SLEEP_2V75
#define     NINGBO_LDO12_SLEEP_2V80                NINGBO_LDO1_SLEEP_2V80
#define     NINGBO_LDO12_SLEEP_2V85                NINGBO_LDO1_SLEEP_2V85
#define     NINGBO_LDO12_SLEEP_2V90                NINGBO_LDO1_SLEEP_2V90
#define     NINGBO_LDO12_SLEEP_3V00                NINGBO_LDO1_SLEEP_3V00
#define     NINGBO_LDO12_SLEEP_3V10                NINGBO_LDO1_SLEEP_3V10
#define     NINGBO_LDO12_SLEEP_3V30                NINGBO_LDO1_SLEEP_3V30

//LDO5
#define     NINGBO_LDO5_SLEEP_1V70               (0x00)
#define     NINGBO_LDO5_SLEEP_1V80               (0x01)
#define     NINGBO_LDO5_SLEEP_1V90               (0x02)
#define     NINGBO_LDO5_SLEEP_2V50               (0x03)
#define     NINGBO_LDO5_SLEEP_2V80               (0x04)
#define     NINGBO_LDO5_SLEEP_2V90               (0x05)
#define     NINGBO_LDO5_SLEEP_3V10               (0x06)
#define     NINGBO_LDO5_SLEEP_3V30               (0x07)
//LDO13
#define     NINGBO_LDO13_SLEEP_1V70               NINGBO_LDO5_SLEEP_1V70
#define     NINGBO_LDO13_SLEEP_1V80               NINGBO_LDO5_SLEEP_1V80
#define     NINGBO_LDO13_SLEEP_1V90               NINGBO_LDO5_SLEEP_1V90
#define     NINGBO_LDO13_SLEEP_2V50               NINGBO_LDO5_SLEEP_2V50
#define     NINGBO_LDO13_SLEEP_2V80               NINGBO_LDO5_SLEEP_2V80
#define     NINGBO_LDO13_SLEEP_2V90               NINGBO_LDO5_SLEEP_2V90
#define     NINGBO_LDO13_SLEEP_3V10               NINGBO_LDO5_SLEEP_3V10
#define     NINGBO_LDO13_SLEEP_3V30               NINGBO_LDO5_SLEEP_3V30


#define     NINGBO_LDO5_1V70                (0x0)
#define     NINGBO_LDO5_1V80                (0x1)
#define     NINGBO_LDO5_1V90                (0x2)
#define     NINGBO_LDO5_2V50                (0x3)
#define     NINGBO_LDO5_2V80                (0x4)
#define     NINGBO_LDO5_2V90                (0x5)
#define     NINGBO_LDO5_3V10                (0x6)
#define     NINGBO_LDO5_3V30                (0x7)


#define     NINGBO_VBUCK1_ACTIVE_VOUT_REG           (0x20)
#define     NINGBO_VBUCK2_ACTIVE_VOUT_REG           (0x30)
#define     NINGBO_VBUCK3_ACTIVE_VOUT_REG           (0x40)

#define     NINGBO_VBUCK1_SLEEP_VOUT_REG			(0x21)
#define     NINGBO_VBUCK2_SLEEP_VOUT_REG			(0x31)
#define     NINGBO_VBUCK3_SLEEP_VOUT_REG			(0x41)

#define     NINGBO_VBUCK1_FSM_REG1			(0x22)
#define     NINGBO_VBUCK1_FSM_REG2			(0x23)
#define     NINGBO_VBUCK1_FSM_REG3			(0x24)
#define     NINGBO_VBUCK1_FSM_REG4			(0x25)
#define     NINGBO_VBUCK1_FSM_REG5			(0x26)
#define     NINGBO_VBUCK1_FSM_REG6			(0x27)
#define     NINGBO_VBUCK1_FSM_REG7			(0x28)

#define     NINGBO_VBUCK2_FSM_REG1			(0x32)
#define     NINGBO_VBUCK2_FSM_REG2			(0x33)
#define     NINGBO_VBUCK2_FSM_REG3			(0x34)
#define     NINGBO_VBUCK2_FSM_REG4			(0x35)
#define     NINGBO_VBUCK2_FSM_REG5			(0x36)
#define     NINGBO_VBUCK2_FSM_REG6			(0x37)
#define     NINGBO_VBUCK2_FSM_REG7			(0x38)

#define     NINGBO_VBUCK3_FSM_REG1			(0x42)
#define     NINGBO_VBUCK3_FSM_REG2			(0x43)
#define     NINGBO_VBUCK3_FSM_REG3			(0x44)
#define     NINGBO_VBUCK3_FSM_REG4			(0x45)
#define     NINGBO_VBUCK3_FSM_REG5			(0x46)
#define     NINGBO_VBUCK3_FSM_REG6			(0x47)
#define     NINGBO_VBUCK3_FSM_REG7			(0x48)

#define	    NINGBO_VBUCK1_DVC_ENABLE_REG		NINGBO_VBUCK1_SLEEP_VOUT_REG 


#define	    NINGBO_VBUCK1_ENABLE_REG		NINGBO_VBUCK1_ACTIVE_VOUT_REG 
#define	    NINGBO_VBUCK2_ENABLE_REG		NINGBO_VBUCK2_ACTIVE_VOUT_REG 
#define	    NINGBO_VBUCK3_ENABLE_REG		NINGBO_VBUCK3_ACTIVE_VOUT_REG 

#define     NINGBO_VBUCK1_SLEEP_MODE_REG			NINGBO_VBUCK1_FSM_REG1 
#define     NINGBO_VBUCK2_SLEEP_MODE_REG			NINGBO_VBUCK2_FSM_REG1 
#define     NINGBO_VBUCK3_SLEEP_MODE_REG			NINGBO_VBUCK3_FSM_REG1 

#define	    NINGBO_VBUCK1_DVC_VOUT_REG_SET0			(0x2a)
#define	    NINGBO_VBUCK1_DVC_VOUT_REG_SET1			(0x2b)
#define	    NINGBO_VBUCK1_DVC_VOUT_REG_SET2			(0x2c)
#define	    NINGBO_VBUCK1_DVC_VOUT_REG_SET3			(0x2d)


#define		NINGBO_CONTAIN_VBUCK_DVC_VOUT_MASK		(0x7f)


#define		NINGBO_CONTAIN_VBUCK_EN_BIT(x)		(x==NINGBO_VBUCK1_ENABLE_REG|| \
											x==NINGBO_VBUCK2_ENABLE_REG|| \
											x==NINGBO_VBUCK3_ENABLE_REG)

#define		NINGBO_CONTAIN_VBUCK_ACTIVE_VOUT_BIT(x)		NINGBO_CONTAIN_VBUCK_EN_BIT(x)

#define		NINGBO_CONTAIN_VBUCK_SLEEP_VOUT_BIT(x)	(x==NINGBO_VBUCK1_SLEEP_VOUT_REG|| \
												x==NINGBO_VBUCK2_SLEEP_VOUT_REG|| \
												x==NINGBO_VBUCK3_SLEEP_VOUT_REG)

#define		NINGBO_CONTAIN_VBUCK_DVC_VOUT_BIT(x)	(x==NINGBO_VBUCK1_DVC_VOUT_REG_SET0|| \
												x==NINGBO_VBUCK1_DVC_VOUT_REG_SET1|| \
												x==NINGBO_VBUCK1_DVC_VOUT_REG_SET2|| \
												x==NINGBO_VBUCK1_DVC_VOUT_REG_SET3)



#define		NINGBO_CONTAIN_VBUCK_SLEEP_MODE_BIT(x) 		(x==NINGBO_VBUCK1_SLEEP_MODE_REG|| \
												x==NINGBO_VBUCK2_SLEEP_MODE_REG|| \
												x==NINGBO_VBUCK3_SLEEP_MODE_REG)

#define		NINGBO_CONTAIN_VBUCK_DVC_EN_BIT(x) 		(x==NINGBO_VBUCK1_DVC_ENABLE_REG)


#define		NINGBO_VBUCK_ENABLE_MASK			(0x1<<7)
#define		NINGBO_VBUCK_ENABLE_DVC_MASK		(0x1<<7)
#define		NINGBO_VBUCK_SLEEP_MODE_MASK		(0x3<<3)

//00	:	BUCK OFF
//01	:	BUCK ACTIVE mode
//10	:	BUCK SLEEP mode
//11	:	BUCK NORMAL mode
#define		NINGBO_VBUCK_OFF			(0x0 <<3)
#define		NINGBO_VBUCK_ACTIVE_SLEEP	(0x1 <<3)
#define		NINGBO_VBUCK_SLEEP			(0x2 <<3)
#define		NINGBO_VBUCK_ACTIVE			(0x3 <<3)


#define		NINGBO_CONTAIN_LDO_EN_BIT(x) 		(x==NINGBO_LDO1_ENABLE_REG|| \
											x==NINGBO_LDO2_ENABLE_REG|| \
											x==NINGBO_LDO3_ENABLE_REG|| \
											x==NINGBO_LDO4_ENABLE_REG|| \
											x==NINGBO_LDO5_ENABLE_REG|| \
											x==NINGBO_LDO5_ENABLE_REG|| \
											x==NINGBO_LDO6_ENABLE_REG|| \
											x==NINGBO_LDO7_ENABLE_REG|| \
											x==NINGBO_LDO8_ENABLE_REG|| \
											x==NINGBO_LDO9_ENABLE_REG|| \
											x==NINGBO_LDO10_ENABLE_REG|| \
											x==NINGBO_LDO11_ENABLE_REG|| \
											x==NINGBO_LDO12_ENABLE_REG|| \
											x==NINGBO_LDO13_ENABLE_REG)

#define		NINGBO_CONTAIN_LDO_ACTIVE_VOUT_BIT(x)	NINGBO_CONTAIN_LDO_EN_BIT(x)


#define		NINGBO_CONTAIN_LDO_SLEEP_MODE_BIT(x) 		(x==NINGBO_LDO1_SLEEP_MODE_REG|| \
												x==NINGBO_LDO2_SLEEP_MODE_REG|| \
												x==NINGBO_LDO3_SLEEP_MODE_REG|| \
												x==NINGBO_LDO4_SLEEP_MODE_REG|| \
												x==NINGBO_LDO5_SLEEP_MODE_REG|| \
												x==NINGBO_LDO6_SLEEP_MODE_REG|| \
												x==NINGBO_LDO7_SLEEP_MODE_REG|| \
												x==NINGBO_LDO8_SLEEP_MODE_REG|| \
												x==NINGBO_LDO9_SLEEP_MODE_REG|| \
												x==NINGBO_LDO10_SLEEP_MODE_REG|| \
												x==NINGBO_LDO11_SLEEP_MODE_REG|| \
												x==NINGBO_LDO12_SLEEP_MODE_REG|| \
												x==NINGBO_LDO13_SLEEP_MODE_REG)
												
#define		NINGBO_CONTAIN_LDO_SLEEP_VOUT_BIT(x)	NINGBO_CONTAIN_LDO_SLEEP_MODE_BIT(x)

#define		NINGBO_LDO_ENABLE_MASK				(0x1<<6)
#define		NINGBO_LDO_SLEEP_MODE_MASK			(0x3<<4)

//00	:	LDO OFF
//01	:	Reserve
//10	:	LDO SLEEP mode
//11	:	LDO NORMAL mode
#define     NINGBO_LDO_OFF                      (0x0 <<4)
#define     NINGBO_LDO_SLEEP                    (0x2 <<4)
#define     NINGBO_LDO_ACTIVE                   (0x3 <<4)


//POWER_PAGE_ADDR=0x42 , BIT[6:5] 
//00	:	disable APT
//11	:	enable APT
//confirm with
#define		NINGBO_APT_REG						(0x42)
#define		NINGBO_APT_ENABLE_MASK				(0x3<<5)


//BASE_PAGE_ADDR=0xE2 , BIT[3:0]
#define		NINGBO_RESET_DISCHARGE_REG			(0xe2)
#define		NINGBO_RESET_DISCHARGE_MASK			(0xf)

#define		NINGBO_FAULT_WU_REG					(0xe7)
#define		NINGBO_FAULT_WU_BIT					(0x1<<3)
#define		NINGBO_FAULT_WU_ENABLE_BIT			(0x1<<0)

#define		NINGBO_RESET_REG				(0x0d)     
#define		NINGBO_SW_REST_BIT				(0x1<<5)
#define		NINGBO_WD_REST_BIT				(0x1<<4)

#define		NINGBO_PWR_HOLD_REG				NINGBO_RESET_REG
#define		NINGBO_PWR_HOLD_BIT				(0x1<<7)

#define 	NINGBO_BUCK_ENABLE                   (0x1<<7)
#define 	NINGBO_LDO_ENABLE                    (0x1<<6)

#define		NINGBO_I2C_CotlAddr		     I2CRegBaseAddress

/* Ningbo I2c address*/
#define NINGBO_BASE_SLAVE_WRITE_ADDR		     0x60
#define NINGBO_BASE_SLAVE_READ_ADDR	             0x61

#define NINGBO_POWER_SLAVE_WRITE_ADDR		     0x62
#define NINGBO_POWER_SLAVE_READ_ADDR	         0x63

#define NINGBO_GPADC_SLAVE_WRITE_ADDR		     0x64
#define NINGBO_GPADC_SLAVE_READ_ADDR	         0x65

/* There are 3 I2C Address for Ningbo.  */
typedef enum
{
    NINGBO_BASE_Reg,
    NINGBO_POWER_Reg,
    NINGBO_GPADC_Reg,
    NINGBO_TEST_Reg
}Ningbo_Reg_Type;

typedef enum {
	//INT ENABLE REG 2 addr=0x0a
	NINGBO_TINT_INT=0,
	NINGBO_GPADC0_INT,
	NINGBO_GPADC1_INT,
	NINGBO_VINLDO_INT,
	NINGBO_VBAT_INT,
	NINGBO_CP_START_ERROR_DET_INT,
	NINGBO_CLASSD_OUT_DET_INT,
	NINGBO_RTC_INT,
	
	//INT ENABLE REG 1 addr=0x09
	NINGBO_ONKEY_INT=8,
	NINGBO_EXTON1_INT,
	NINGBO_EXTON2_INT,
	NINGBO_BAT_INT,
	NINGBO_VBUS_UVLO_INT,
	NINGBO_VBUS_OVP_INT,
	NINGBO_VBUS_DET_INT,
	NINGBO_CP_START_DONE_DET_INT,
} NINGBO_INTC ;

#define NINGBO_INTC_MAX 15
#define NINGBO_INTC_TO_STATUS_BIT(intc) (0x01<<intc)
#define NINGBO_INTC_TO_ENABLE_BIT(intc) (1<<(intc%8))
#define NINGBO_INTC_TO_ENABLE_REG(intc) (NINGBO_INT_ENABLE_REG2 - (intc/8))

typedef enum {
	//gpadc meas en reg 2 ,addr=0x04
	NINGBO_AVDD_MEAS_EN,
	NINGBO_DVDD_MEAS_EN,
	NINGBO_VSUP_MEAS_EN,
	NINGBO_GPADC1_MEAS_EN,
	//gpadc meas en reg 2 ,addr=0x03
	NINGBO_LDO8_MEAS_EN=8,
	NINGBO_LDO9_MEAS_EN,
	NINGBO_LDO10_MEAS_EN,
	NINGBO_LDO11_MEAS_EN,
	NINGBO_LDO12_MEAS_EN,
	NINGBO_LDO13_MEAS_EN,
	NINGBO_VPWR_MEAS_EN,
	NINGBO_VRTC_MEAS_EN,
	//gpadc meas en reg 2 ,addr=0x02
	NINGBO_GPADC0_MEAS_EN, /*16*/
	NINGBO_LDO1_MEAS_EN,
	NINGBO_LDO2_MEAS_EN,
	NINGBO_LDO3_MEAS_EN,
	NINGBO_LDO4_MEAS_EN,
	NINGBO_LDO5_MEAS_EN,
	NINGBO_LDO6_MEAS_EN,
	NINGBO_LDO7_MEAS_EN,
	//gpadc meas en reg 1 ,addr=0x01
	NINGBO_BUCK1_MEAS_EN, /*24*/
	NINGBO_BUCK2_MEAS_EN,
	NINGBO_BUCK3_MEAS_EN,
	NINGBO_VCHG_MEAS_EN, /*Vbus*/
	NINGBO_VBAT_MEAS_EN,
	NINGBO_TINT_MEAS_EN,
	NINGBO_BATID_MEAS_EN,
	NINGBO_BATTEMP_MEAS_EN,
}NINGBO_MEAS_EN ;

typedef enum {
    //gpadc bias out en reg,addr=0x0D, no bias en reg
	NINGBO_GPADC0_BIAS_OUT,
	NINGBO_GPADC1_BIAS_OUT,
	NINGBO_GPADC2_BIAS_OUT, /* Battery ID */
	NINGBO_GPADC3_BIAS_OUT, /* Battery Temperature */
}NINGBO_BIAS_OUT ;

#define NINGBO_MEAS_TO_ENABLE_REG(meas_en) ((meas_en >= NINGBO_BUCK1_MEAS_EN)? NINGBO_GPADC_MEAS_EN_REG1 : \
                                            (meas_en >= NINGBO_GPADC0_MEAS_EN)? NINGBO_GPADC_MEAS_EN_REG2: \
                                            (meas_en >= NINGBO_LDO8_MEAS_EN)? NINGBO_GPADC_MEAS_EN_REG3: \
                                            NINGBO_GPADC_MEAS_EN_REG4)
#define NINGBO_MEAS_TO_ENABLE_BIT(meas_en) (0x01<<(meas_en%8))

#define VOLT_CONVERT_12BIT_MV(meas) ((meas)*(1300*128)/(129*4096)) //(CODE)/4096*1.3*128/129 *1000 ->mv, CODE is the adc output, 12bit
#define TEMP_CONVERT_16BIT_MV(meas) ((meas)*(13*128)/(12*129)-273) //(CODE)*1.3/1.2*128/129-273, CODE is tsen output, high 10bit (total 16bit)

typedef enum 
{
	PRE_CHG_CUR_75MA,
	PRE_CHG_CUR_50MA,
	PRE_CHG_CUR_100MA,
	PRE_CHG_CUR_150MA,
	PRE_CHG_CUR_MAX=PRE_CHG_CUR_150MA
}PRE_CHG_CURRENT;

typedef enum 
{
	CC_CHG_CUR_100MA,
	CC_CHG_CUR_200MA,
	CC_CHG_CUR_300MA,
	CC_CHG_CUR_400MA,
	CC_CHG_CUR_450MA,
	CC_CHG_CUR_500MA,
	CC_CHG_CUR_550MA,
	CC_CHG_CUR_600MA,
	CC_CHG_CUR_650MA,
	CC_CHG_CUR_700MA,
	CC_CHG_CUR_750MA,
	CC_CHG_CUR_800MA,
	CC_CHG_CUR_850MA,
	CC_CHG_CUR_900MA,
	CC_CHG_CUR_950MA,
	CC_CHG_CUR_1000MA,

	CC_CHG_CUR_150MA,
	CC_CHG_CUR_250MA,
	CC_CHG_CUR_350MA,
	
	CC_CHG_CUR_MAX=CC_CHG_CUR_350MA
}CC_CHG_CURRENT;

typedef enum 
{
	TERM_CHG_CUR_50MA,
	TERM_CHG_CUR_25MA,
	TERM_CHG_CUR_75MA,
	TERM_CHG_CUR_100MA,
	TERM_CHG_CUR_MAX=TERM_CHG_CUR_100MA
}TERMINATE_CHG_CURRENT;

typedef enum 
{
	VBAT_VOL_4V20,
	VBAT_VOL_4V35,
	VBAT_VOL_4V15,
	VBAT_VOL_4V10,
	VBAT_VOL_4V30,
	VBAT_VOL_4V40,
	VBAT_VOL_4V55,
	VBAT_VOL_MAX=VBAT_VOL_4V55
}VBAT_VOLTAGE;


typedef enum 
{
	PRE_CHG_MINUTE24,
	PRE_CHG_MINUTE36,
	PRE_CHG_MINUTE48,
	PRE_CHG_MINUTE60,
	PRE_CHG_MINUTE_MAX=PRE_CHG_MINUTE60
}PRE_CHG_TIMER;

typedef enum 
{
	TRICKLE_CHG_MINUTE6,
	TRICKLE_CHG_MINUTE9,
	TRICKLE_CHG_MINUTE12,
	TRICKLE_CHG_MINUTE15,
	TRICKLE_CHG_MINUTE_MAX=TRICKLE_CHG_MINUTE15
}TRICKLE_CHG_TIMER;

typedef enum 
{
	CUR_CHK_SECOND3,
	CUR_CHK_SECOND7,
	CUR_CHK_SECOND11,
	CUR_CHK_SECOND15,
	CUR_CHK_SECOND_MAX=CUR_CHK_SECOND15
}CUR_CHK_TIMER;

typedef enum 
{
	VOL_CHK_MSECOND3,
	VOL_CHK_MSECOND7,
	VOL_CHK_MSECOND11,
	VOL_CHK_MSECOND15,
	VOL_CHK_MSECOND_MAX=VOL_CHK_MSECOND15
}VOL_CHK_TIMER;

typedef enum 
{
	CCCV_MINUTE96,
	CCCV_MINUTE144,
	CCCV_MINUTE192,
	CCCV_MINUTE240,
	CCCV_MINUTE_MAX=CCCV_MINUTE240
}CCCV_TIMER;

typedef enum 
{
	FSM_SHUTDOWN,
	FSM_CHECK,
	FSM_TRI_CHG,
	FSM_PRE_CHG,
	FSM_CC_CHG,
	FSM_CV_CHG,
	FSM_TERM,
	FSM_FAULT,
	CHGFSM_MAX=FSM_FAULT
}CHGFSM_OUT;

typedef enum 
{
    CHG_FORCE_SHUTDOWN = 0x8,
    CHG_FORCE_CHECK,      
    CHG_FORCE_TRICKLE,
    CHG_FORCE_PRE,
    CHG_FORCE_CC,         
    CHG_FORCE_CV,         
    CHG_FORCE_TERM,       
    CHG_FORCE_FAULT,      
    CHG_FORCE_MAX = CHG_FORCE_FAULT
}FORCE_CHG_STATE;

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboRead                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read Ningbo by PI2C interface.                      */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
int NingboRead( Ningbo_Reg_Type ningbo_reg_type, unsigned char reg, unsigned char *value );


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboWite                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Write Ningbo by PI2C interface.                     */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
int NingboWrite( Ningbo_Reg_Type ningbo_reg_type, unsigned char reg, unsigned char value );

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      ningbo_read_volt_meas_val                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read ADCs with voltage of Ningbo PMIC.              */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
#if 0
unsigned short ningbo_read_volt_meas_val(unsigned char meaReg);
#endif
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ningbo_VsimSleep_Enable                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function enable the VSIM sleep mode.                         */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void Ningbo_VsimSleep_Enable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ningbo_VsimSleep_Disable                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function disable the VSIM sleep mode.                        */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void Ningbo_VsimSleep_Disable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ningbo_miccoDisableUsimV                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function disable the USIM voltage.                           */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void Ningbo_miccoDisableUsimV(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ningbo_miccoEnableUsimV                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function enable the USIM voltage.                            */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void Ningbo_miccoEnableUsimV(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Ningbo_miccoConfigUsimV                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function configure the USIM voltage.                         */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void Ningbo_miccoConfigUsimV(unsigned char volatge);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      DisableNingboWDT                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Disable Watchdog of Ustia.                          */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void DisableNingboWDT( void );

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      NingboClkInit                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the Ustia clock.                         */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      meas_val                            The 12bit ADC value          */
/*                                                                       */
/*************************************************************************/
void NingboClkInit( void );


int Ningbo_VBUCK_Set_Enable(unsigned char reg, unsigned char enable);
int Ningbo_VBUCK_Set_DVC_Enable(unsigned char reg, unsigned char enable);
int Ningbo_VBUCK_Set_Slpmode(unsigned char reg, unsigned char mode);
int Ningbo_VBUCK_Set_VOUT(unsigned char reg, unsigned char value);
int Ningbo_LDO_Set_Enable(unsigned char reg, unsigned char enable);
int Ningbo_LDO_Set_Slpmode(unsigned char reg, unsigned char mode);
int Ningbo_LDO_Set_VOUT(unsigned char reg, unsigned char value);
int Ningbo_APT_Set(unsigned char enable);
int Ningbo_SW_Reset(void );
void Ningbo_Dump_PMIC_Register(void);
void Ningbo_Aditional_Workaround(void);
void Ningbo_SoftwareNotProceedUntilReach3400(void);

UINT32 pm813_get_battemp_meas_vol_mv(void);
UINT32 pm813_get_batid_meas_vol_mv(void);
UINT32 pm813_get_Vcharge_meas_vol_mv(void);
UINT32 pm813_get_Icharge_meas_cur_mA(void);
UINT32 pm813_get_bat_vol(void);
BOOL pm813_measured_current_means_charging(void);
BOOL pm813_get_battery_status(void);         
BOOL pm813_get_charger_status(void);        
BOOL pm813_charger_in_charging(void);
BOOL pm813_if_charge_in_cv_mode(void);
void pm813_precharge_current_set(UINT8 value);    
void pm813_charge_termination_current_set(UINT8 value);    
void pm813_cc_current_set(CC_CHG_CURRENT value);    
CC_CHG_CURRENT pm813_cc_current_get(void);
void pm813_cc_current_set_toohigh(UINT8 value);    
void pm813_vbat_set(UINT8 value);         
void pm813_precharge_timer_set(UINT8 value);    
void pm813_trickle_timer_set(UINT8 value);    
void pm813_current_check_timer_set(UINT8 value);    
void pm813_voltage_check_timer_set(UINT8 value);    
void pm813_cccv_timer_set(UINT8 value); 
void pm813_gpadc_write_vol_meas(UINT8 meaReg, UINT16 meas_val);
void pm813_set_battery_temp_reference(void);
BOOL pm813_charger_is_usb(void);
void pm813_charger_timer_switch(BOOL OnOff);
void pm813_fault_clear(void);
CHGFSM_OUT pm813_get_charger_FSM_state(void);
void pm813_charger_force_check(BOOL OnOff); 
UINT32 pm813_get_battery_voltage_withoutCharger(void);
BOOL pm813_get_chargerError(void);
void pm813_set_charger_fsm_uvsys_3v2(void);
void pm813_charger_restore(void);
void pm813_charger_internal_drv_segment_set(BOOL Enable);
void pm813_charger_disable_vbus_uv_set(BOOL Enable);
void pm813_charger_vbus_detect_int_select_set(BOOL Enable);
BOOL pm813_set_max_cc_current(UINT32 limit);
void pm813_force_charger_state_set(FORCE_CHG_STATE value);    
FORCE_CHG_STATE pm813_force_charger_state_get(void);
void pm813_userdata_set_for_automation_test(UINT8 flag);
UINT8 pm813_userdata_get_for_automation_test(void);

//to mark PMIC RTC APP OFFSET EVER CONFIGED
void pm813_userdata_set_app_rtc_ever_configed(void);
BOOL pm813_userdata_get_app_rtc_ever_configed(void);
void pm813_userdata_set_app_lcd_ever_configed(void);
BOOL pm813_userdata_get_app_lcd_ever_configed(void);
BOOL isLcdEverInitedUponBooting(void);
void pm813_long_onkey_to_powerdown_enable(BOOL OnOff);
BOOL pm813_lcdbacklight_status_get(void);
BOOL pm813_keypadbacklight_status_get(void);
BOOL pm813_vibrator_status_get(void);
BOOL pm813_torchlight_status_get(void);
#endif /* _NINGBO_H_        */
