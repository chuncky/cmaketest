#ifndef     _GUILIN_H_
#define     _GUILIN_H_
/***********************************************************************
*      CopyRight (C). 2013. Marvell Semi. Co., ltd.
************************************************************************
* FileName: Guilin.h
*
* Descrition:
*       The Guilin PMIC register defines.
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
//#include "platform.h"
#define GUILIN_DEBUG 0

#if GUILIN_DEBUG
#define GUILIN_UART_DEBUG(fmt,...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define GUILIN_UART_DEBUG(fmt,...)
#endif

/***************************************************************************
*                       Typedefines  & Macros
****************************************************************************/
/* Guilin Registers defines.                        */
/* (1)GUILIN BASE REGISTER                          */

#define     GUILIN_ID_REG                       0x00            /* Identification Register.                         */

#define     GUILIN_STATUS_REG1                  0x01
#define     GUILIN_BAT_DETECT                   0x08            /* Battery is present.                              */
#define     GUILIN_EXTON2_DETECT                0x04            /* EXTON2_DETECT = 1.                               */
#define     GUILIN_EXTON1_DETECT                0x02            /* EXTON1_DETECT = 1.                               */
#define     GUILIN_ONKEY_PRESSED                0x01            /* ONKEYn is Pressed.                               */
#define		GUILIN_RTC_ALARM_STATUS_BIT			0x10

#define     GUILIN_STATUS_REG2                  0x02

#define     GUILIN_INT_STATUS_REG1              0x05            /* Interrupt Status Register1.                      */
#define     GUILIN_RTC_INT_OCCURRED             0x10            /* Set,when RTC alarm occurred.                     */
#define     GUILIN_BAT_INT_OCCURRED             0x08            /* Set,when BAT_DET changes.                        */
#define     GUILIN_EXTON2_INT_OCCURRED          0x04            /* Set,when EXTON2 stable.                          */
#define     GUILIN_EXTON1_INT_OCCURRED          0x02            /* Set,when EXTON1 charged.                         */
#define     GUILIN_ONKEY_INT_OCCURRED           0x01            /* Set,when ONKEY changed.                          */


#define     GUILIN_INT_STATUS_REG2              0x06            /* Interrupt Status Register2.                      */
#define     GUILIN_VINLDO_INT_OCCURRED          0x08            /* Set,when GPADC2 cross theshold.                  */
#define     GUILIN_GPADC1_INT_OCCERRED          0x04            /* Set,when GPADC1 cross threshold.                 */
#define     GUILIN_GPADC0_INT_OCCURRED          0x02            /* Set,when GPADC0 cross threshold.                 */
#define     GUILIN_TINT_OCCURRED                0x01            /* Set,when GPADC8 cross theshold.                  */


#define     GUILIN_INT_STATUS_REG3              0x07            /* Interrupt Status Register3.                      */
#define     GUILIN_INT_STATUS_REG4              0x08            /* Interrupt Status Register4.                      */

#define     GUILIN_INT_ENABLE_REG1              0x09            /* Interrupt Enable Register1.                      */
#define     GUILIN_RTC_INT_EN                   0x10            /* RTC Interrupt Enable.                            */
#define     GUILIN_BAT_INT_EN                   0x08            /* BAT Interrupt Enable.                            */
#define     GUILIN_EXTON2_INT_EN                0x04            /* EXTON2 Interrupt Enable.                         */
#define     GUILIN_EXTON1_INT_EN                0x02            /* EXTON1 Interrupt Enable.                         */
#define     GUILIN_ONKEY_INT_EN                 0x01            /* ONKEY Interrupt Enable.                          */

#define     GUILIN_INT_ENABLE_REG2              0x0A            /* Interrupt Enable Register2.                      */
#define     GUILIN_VINLDO_INT_EN                0x08            /* VINLDO Interrupt Enalbe.                         */
#define     GUILIN_GPADC1_INT_EN                0x04            /* GPADC1 Interrupt Enable.                         */
#define     GUILIN_GPADC0_INT_EN                0x02            /* GPADC0 Interrupt Enable.                         */
#define     GUILIN_TINT_INT_EN                  0x01            /* TINT Interrupt Enable.                           */

#define     GUILIN_WAKEUP_REG1                  0x0d            /* Wakeup Register1.                                */
#define     GUILIN_POWER_HOLD                   0x80            /* LPF & DVC enable, SLEEPn disable.                */
#define     GUILIN_RESET_PMIC_REG               0x40            /* Reset Ustica registers.                          */
#define     GUILIN_SW_PDOWN                     0x20            /* Entrance to 'power-down' sate.                   */
#define     GUILIN_WD_RESET                     0x10            /* Resets the Watchdog timer.                       */
//#define     GUILIN_LONGKEY_RESET                0x08            /* Resets the LONGKEY press timer.                  */
#define     GUILIN_WD_MODE                      0x01            /* WD1#->toggle RESET_OUTn,WD2#->PowerDown.         */

#define 	GUILIN_PWRUP_LOG_REG				0x10


#define     GUILIN_WD_REG                       0x1D            /* Watchdog Register                                */
#define     GUILIN_WD_DIS                       0x01            /* Watchdog disable.                                */

#define     GUILIN_RTC_CTRL_REG	                0xD0            /* RTC_Control>.                                */
#define     GUILIN_RTC_ALARM_WU_BIT	            (0x01<<4)
#define     GUILIN_RTC_ALARM_BIT	            (0x01<<5)
#define     GUILIN_RTC_USE_XO_BIT	            (0x01<<7)
#define     GUILIN_RTC_ALARM_SET1_BIT           (0x01<<0)
#define     GUILIN_RTC_ALARM_SET2_BIT           (0x01<<1)

#define     GUILIN_RTC_COUNT_REG1               0xD1            /* RTC_COUNTER<0:7>.                                */

#define     GUILIN_RTC_COUNT_REG2               0xD2            /* RTC_COUNTER<8:15>.                               */

#define     GUILIN_RTC_COUNT_REG3               0xD3            /* RTC_COUNTER<16:23>.                              */

#define     GUILIN_RTC_COUNT_REG4               0xD4            /* RTC_COUNTER<24:31>.                              */

#define     GUILIN_RTC_EXPIRE1_REG1             0xD5            /* RTC_EXPIRE1<0:7>.                                */

#define     GUILIN_RTC_EXPIRE1_REG2             0xD6            /* RTC_EXPIRE1<8:15>.                               */

#define     GUILIN_RTC_EXPIRE1_REG3             0xD7            /* RTC_EXPIRE1<16:23>.                              */

#define     GUILIN_RTC_EXPIRE1_REG4             0xD8            /* RTC_EXPIRE1<24:31>.                              */

#define     GUILIN_RTC_EXPIRE2_REG1             0xDD            /* RTC_EXPIRE2<0:7>.                                */

#define     GUILIN_RTC_EXPIRE2_REG2             0xDE            /* RTC_EXPIRE2<8:15>.                               */

#define     GUILIN_RTC_EXPIRE2_REG3             0xDF            /* RTC_EXPIRE2<16:23>.                              */

#define     GUILIN_RTC_EXPIRE2_REG4             0xE0            /* RTC_EXPIRE2<24:31>.                              */

//20200320@xiaoke implement RTC statemachine for pm802
//use expire2 as userdata property registers , 
//and only SYS_RTC_OFFSET support as register limitation , only 4byte available.
#define 	GUILIN_RTC_SYS_OFFSET_REG GUILIN_RTC_EXPIRE2_REG1
#define 	GUILIN_RTC_APP_OFFSET_REG GUILIN_RTC_EXPIRE2_REG1  //reuse RTC_EXPIRE_2 as APP_OFFSET
//20200320@xiaoke implement RTC statemachine for pm802
#define     GUILIN_RTC_MISC_3_REG				0xE3

#define     GUILIN_CLK_32K_SEL_REG				0xE4
#define     GUILIN_CLK_32K_SEL					0x04
#define     GUILIN_LONGKEY_EN2                  0x02             /* When enabled, the event will cause power-down.   */
#define     GUILIN_LONGKEY_EN1                  0x01             /* When enabled, the event will cause power-down.   */

#define 	GUILIN_LONGKEY_1					0
#define 	GUILIN_LONGKEY_2					1

#define     GUILIN_POWERDOWN_LOG_REG				0xE5
#define     GUILIN_POWERDOWN_LOG_REG2				0xE6
//#define     USTICA_USERDATA_REG1                0xEA             /* User Data Field<23:16>.                          */



#define     GUILIN_RTC_CONTROL_REG				0xF1
#define     GUILIN_RTC_USE_XO                   0x04            /* 32kHz clock and buffer clock generated form XO.  */
#define     GUILIN_CRYSTAL_CAP_SET_REG          0xF0
#define     GUILIN_CRYSTAL_CAP_22PF				0xE0
#define     GUILIN_CRYSTAL_CAP_20PF				0x60



/* (3)Guilin GPADC Registers.                 */
#define     GUILIN_GPADC_MEAS_EN_REG1           0x01            /* GPADC Measurement Enable Register1.          */
#define     GUILIN_GPADC0_MEAS_EN               (1 << 6)        /* GPADC0 measurment enable bit.                */
#define     GUILIN_GPADC1_MEAS_EN               (1 << 7)        /* GPADC1 measurment enable bit.               */

#define     GUILIN_GPADC_MEAS_EN_REG2           0x02            /* GPADC Measurement Enable Register2.          */
#define     GUILIN_VINLDO_MEAS_EN               (1 << 6)        /* VINLDO measurement enable bit.               */


#define     GUILIN_GPADC_MISC_CONFIG_REG2       0x06
#define     GUILIN_NON_STOP                     0x02            /* Enable NON_STOP GPADC operation mode.         */
#define     GUILIN_GPADC_EN                     0x01            /* Enable the GPADC periodic modes.              */
#define     GUILIN_GPADC_SW_TRIG_BIT           (0x01 << 2)

#define     GUILIN_GPADC_BIAS_REG               0x0C
#define     GUILIN_GPADC_BIAS_ENABLE            0x0D
#define     GUILIN_GPADC0_BIAS_OUT              (1 << 0)
#define     GUILIN_GPADC1_BIAS_OUT              (1 << 1)



#define     GUILIN_TINT_LOW_TH_REG              0x13             /* Low threshold for TINT interrupt.    */
#define     GUILIN_TINT_UPP_TH_REG              0x15             /* Upp threshold for TINT interrupt.    */

#define     GUILIN_GPADC1_LOW_TH                0x1B             /* Low threshold for GPADC1 interrupt.    */
#define     GUILIN_GPADC1_UPP_TH                0x1D             /* Upp threshold for GPADC1 interrupt.    */
#define     GUILIN_VINLDO_LOW_TH_REG            0x1F             /* Low threshold for VINLDO interrupt.    */
#define     GUILIN_VINLDO_UPP_TH_REG            0x21             /* Upp threshold for VINLDO interrupt.    */


#define     GUILIN_VGPADC1                      0x4E
#define     GUILIN_VGPADC1_MAX                  0x50
#define     GUILIN_VGPADC1_MIN                  0x52
#define     GUILIN_VGPADC1_AVE                  0x54

#define     GUILIN_VINLDO_REG                  0x62
#define     GUILIN_VINLDO_MAX_REG              0x64
#define     GUILIN_VINLDO_MIN_REG              0x66
#define     GUILIN_VINLDO_AVE_REG              0x68





//LDO
#define     GUILIN_LDO1_ACTIVE_VOUT_REG         0x09
#define     GUILIN_LDO2_ACTIVE_VOUT_REG         0x0c
#define     GUILIN_LDO3_ACTIVE_VOUT_REG         0x0f
#define     GUILIN_LDO4_ACTIVE_VOUT_REG         0x12
#define     GUILIN_LDO5_ACTIVE_VOUT_REG         0x15
#define     GUILIN_LDO6_ACTIVE_VOUT_REG         0x18

#define     GUILIN_LDO1_SLEEP_VOUT_REG          0x08
#define     GUILIN_LDO2_SLEEP_VOUT_REG          0x0b
#define     GUILIN_LDO3_SLEEP_VOUT_REG          0x0e
#define     GUILIN_LDO4_SLEEP_VOUT_REG          0x11
#define     GUILIN_LDO5_SLEEP_VOUT_REG          0x14
#define     GUILIN_LDO6_SLEEP_VOUT_REG          0x17

#define     GUILIN_LDO1_ENABLE_REG			GUILIN_LDO1_ACTIVE_VOUT_REG
#define     GUILIN_LDO2_ENABLE_REG			GUILIN_LDO2_ACTIVE_VOUT_REG
#define     GUILIN_LDO3_ENABLE_REG			GUILIN_LDO3_ACTIVE_VOUT_REG
#define     GUILIN_LDO4_ENABLE_REG			GUILIN_LDO4_ACTIVE_VOUT_REG
#define     GUILIN_LDO5_ENABLE_REG			GUILIN_LDO5_ACTIVE_VOUT_REG
#define     GUILIN_LDO6_ENABLE_REG			GUILIN_LDO6_ACTIVE_VOUT_REG

#define     GUILIN_LDO1_SLEEP_MODE_REG		GUILIN_LDO1_SLEEP_VOUT_REG
#define     GUILIN_LDO2_SLEEP_MODE_REG		GUILIN_LDO2_SLEEP_VOUT_REG
#define     GUILIN_LDO3_SLEEP_MODE_REG		GUILIN_LDO3_SLEEP_VOUT_REG
#define     GUILIN_LDO4_SLEEP_MODE_REG		GUILIN_LDO4_SLEEP_VOUT_REG
#define     GUILIN_LDO5_SLEEP_MODE_REG		GUILIN_LDO5_SLEEP_VOUT_REG
#define     GUILIN_LDO6_SLEEP_MODE_REG		GUILIN_LDO6_SLEEP_VOUT_REG

//VBUCK
//step 0.125v, from 0.6v to 1.6v
#define	    GUILIN_CONTAIN_VBUCK_ACTIVE_VOUT_MASK	(0x7f)
#define     GUILIN_CONTAIN_VBUCK_SLEEP_VOUT_MASK	(0x7f)


#define     GUILIN_VBUCK_0V6000	                    (0x00)
#define     GUILIN_VBUCK_0V6125	                    (GUILIN_VBUCK_0V6000+1)
#define     GUILIN_VBUCK_0V6250	                    (GUILIN_VBUCK_0V6000+2)
#define     GUILIN_VBUCK_0V6375	                    (GUILIN_VBUCK_0V6000+3)
#define     GUILIN_VBUCK_0V6500	                    (GUILIN_VBUCK_0V6000+4)
#define     GUILIN_VBUCK_0V6625	                    (GUILIN_VBUCK_0V6000+5)
#define     GUILIN_VBUCK_0V6750	                    (GUILIN_VBUCK_0V6000+6)
#define     GUILIN_VBUCK_0V6875	                    (GUILIN_VBUCK_0V6000+7)
#define     GUILIN_VBUCK_0V7000                     (GUILIN_VBUCK_0V6000+8)
#define     GUILIN_VBUCK_0V7125                     (GUILIN_VBUCK_0V6000+9)
#define     GUILIN_VBUCK_0V7250                     (GUILIN_VBUCK_0V6000+10)
#define     GUILIN_VBUCK_0V7375                     (GUILIN_VBUCK_0V6000+11)
#define     GUILIN_VBUCK_0V7500                     (GUILIN_VBUCK_0V6000+12)
#define     GUILIN_VBUCK_0V7625                     (GUILIN_VBUCK_0V6000+13)
#define     GUILIN_VBUCK_0V7750                     (GUILIN_VBUCK_0V6000+14)
#define     GUILIN_VBUCK_0V7875                     (GUILIN_VBUCK_0V6000+15)
#define     GUILIN_VBUCK_0V8000                     (GUILIN_VBUCK_0V6000+16)
#define     GUILIN_VBUCK_0V8125                     (GUILIN_VBUCK_0V6000+17)
#define     GUILIN_VBUCK_0V8250                     (GUILIN_VBUCK_0V6000+18)
#define     GUILIN_VBUCK_0V8375                     (GUILIN_VBUCK_0V6000+19)
#define     GUILIN_VBUCK_0V8500                     (GUILIN_VBUCK_0V6000+20)
#define     GUILIN_VBUCK_0V8625                     (GUILIN_VBUCK_0V6000+21)
#define     GUILIN_VBUCK_0V8750                     (GUILIN_VBUCK_0V6000+22)
#define     GUILIN_VBUCK_0V8875                     (GUILIN_VBUCK_0V6000+23)
#define     GUILIN_VBUCK_0V9000                     (GUILIN_VBUCK_0V6000+24)
#define     GUILIN_VBUCK_0V9125                     (GUILIN_VBUCK_0V6000+25)
#define     GUILIN_VBUCK_0V9250                     (GUILIN_VBUCK_0V6000+26)
#define     GUILIN_VBUCK_0V9375                     (GUILIN_VBUCK_0V6000+27)
#define     GUILIN_VBUCK_0V9500                     (GUILIN_VBUCK_0V6000+28)
#define     GUILIN_VBUCK_0V9625                     (GUILIN_VBUCK_0V6000+29)
#define     GUILIN_VBUCK_0V9750                     (GUILIN_VBUCK_0V6000+30)
#define     GUILIN_VBUCK_0V9875                     (GUILIN_VBUCK_0V6000+31)
#define     GUILIN_VBUCK_1V0000                     (GUILIN_VBUCK_0V6000+32)
#define     GUILIN_VBUCK_1V0125                     (GUILIN_VBUCK_0V6000+33)
#define     GUILIN_VBUCK_1V0250                     (GUILIN_VBUCK_0V6000+34)
#define     GUILIN_VBUCK_1V0375                     (GUILIN_VBUCK_0V6000+35)
#define     GUILIN_VBUCK_1V0500                     (GUILIN_VBUCK_0V6000+36)
#define     GUILIN_VBUCK_1V0625                     (GUILIN_VBUCK_0V6000+37)
#define     GUILIN_VBUCK_1V0750                     (GUILIN_VBUCK_0V6000+38)
#define     GUILIN_VBUCK_1V0875                     (GUILIN_VBUCK_0V6000+39)
#define     GUILIN_VBUCK_1V1000                     (GUILIN_VBUCK_0V6000+40)
#define     GUILIN_VBUCK_1V1125                     (GUILIN_VBUCK_0V6000+41)
#define     GUILIN_VBUCK_1V1250                     (GUILIN_VBUCK_0V6000+42)
#define     GUILIN_VBUCK_1V1375                     (GUILIN_VBUCK_0V6000+43)
#define     GUILIN_VBUCK_1V1500                     (GUILIN_VBUCK_0V6000+44)
#define     GUILIN_VBUCK_1V1625                     (GUILIN_VBUCK_0V6000+45)
#define     GUILIN_VBUCK_1V1750                     (GUILIN_VBUCK_0V6000+46)
#define     GUILIN_VBUCK_1V1875                     (GUILIN_VBUCK_0V6000+47)
#define     GUILIN_VBUCK_1V2000                     (GUILIN_VBUCK_0V6000+48)
#define     GUILIN_VBUCK_1V2125                     (GUILIN_VBUCK_0V6000+49)
#define     GUILIN_VBUCK_1V2250                     (GUILIN_VBUCK_0V6000+50)
#define     GUILIN_VBUCK_1V2375                     (GUILIN_VBUCK_0V6000+51)
#define     GUILIN_VBUCK_1V2500                     (GUILIN_VBUCK_0V6000+52)
#define     GUILIN_VBUCK_1V2625                     (GUILIN_VBUCK_0V6000+53)
#define     GUILIN_VBUCK_1V2750                     (GUILIN_VBUCK_0V6000+54)
#define     GUILIN_VBUCK_1V2875                     (GUILIN_VBUCK_0V6000+55)
#define     GUILIN_VBUCK_1V3000                     (GUILIN_VBUCK_0V6000+56)
#define     GUILIN_VBUCK_1V3125                     (GUILIN_VBUCK_0V6000+57)
#define     GUILIN_VBUCK_1V3250                     (GUILIN_VBUCK_0V6000+58)
#define     GUILIN_VBUCK_1V3375                     (GUILIN_VBUCK_0V6000+59)
#define     GUILIN_VBUCK_1V3500                     (GUILIN_VBUCK_0V6000+60)
#define     GUILIN_VBUCK_1V3625                     (GUILIN_VBUCK_0V6000+61)
#define     GUILIN_VBUCK_1V3750                     (GUILIN_VBUCK_0V6000+62)
#define     GUILIN_VBUCK_1V3875                     (GUILIN_VBUCK_0V6000+63)
#define     GUILIN_VBUCK_1V4000                     (GUILIN_VBUCK_0V6000+64)
#define     GUILIN_VBUCK_1V4125                     (GUILIN_VBUCK_0V6000+65)
#define     GUILIN_VBUCK_1V4250                     (GUILIN_VBUCK_0V6000+66)
#define     GUILIN_VBUCK_1V4375                     (GUILIN_VBUCK_0V6000+67)
#define     GUILIN_VBUCK_1V4500                     (GUILIN_VBUCK_0V6000+68)
#define     GUILIN_VBUCK_1V4625                     (GUILIN_VBUCK_0V6000+69)
#define     GUILIN_VBUCK_1V4750                     (GUILIN_VBUCK_0V6000+70)
#define     GUILIN_VBUCK_1V4875                     (GUILIN_VBUCK_0V6000+71)
#define     GUILIN_VBUCK_1V5000                     (GUILIN_VBUCK_0V6000+72)
#define     GUILIN_VBUCK_1V5125                     (GUILIN_VBUCK_0V6000+73)
#define     GUILIN_VBUCK_1V5250                     (GUILIN_VBUCK_0V6000+74)
#define     GUILIN_VBUCK_1V5375                     (GUILIN_VBUCK_0V6000+75)
#define     GUILIN_VBUCK_1V5500                     (GUILIN_VBUCK_0V6000+76)
#define     GUILIN_VBUCK_1V5625                     (GUILIN_VBUCK_0V6000+77)
#define     GUILIN_VBUCK_1V5750                     (GUILIN_VBUCK_0V6000+78)
#define     GUILIN_VBUCK_1V5875                     (GUILIN_VBUCK_0V6000+79)
#define     GUILIN_VBUCK_1V6000                     (GUILIN_VBUCK_0V6000+80)
//step 0.05v, from 1.65v to 3.95v
#define     GUILIN_VBUCK_1V65                       (GUILIN_VBUCK_0V6000+81)
#define     GUILIN_VBUCK_1V70                       (GUILIN_VBUCK_0V6000+82)
#define     GUILIN_VBUCK_1V75                       (GUILIN_VBUCK_0V6000+83)
#define     GUILIN_VBUCK_1V80                       (GUILIN_VBUCK_0V6000+84)
#define     GUILIN_VBUCK_1V85                       (GUILIN_VBUCK_0V6000+85)
#define     GUILIN_VBUCK_1V90                       (GUILIN_VBUCK_0V6000+86)
#define     GUILIN_VBUCK_1V95                       (GUILIN_VBUCK_0V6000+87)
#define     GUILIN_VBUCK_2V00                       (GUILIN_VBUCK_0V6000+88)
#define     GUILIN_VBUCK_2V05                       (GUILIN_VBUCK_0V6000+89)
#define     GUILIN_VBUCK_2V10                       (GUILIN_VBUCK_0V6000+90)
#define     GUILIN_VBUCK_2V15                       (GUILIN_VBUCK_0V6000+91)
#define     GUILIN_VBUCK_2V20                       (GUILIN_VBUCK_0V6000+92)
#define     GUILIN_VBUCK_2V25                       (GUILIN_VBUCK_0V6000+93)
#define     GUILIN_VBUCK_2V30                       (GUILIN_VBUCK_0V6000+94)
#define     GUILIN_VBUCK_2V35                       (GUILIN_VBUCK_0V6000+95)
#define     GUILIN_VBUCK_2V40                       (GUILIN_VBUCK_0V6000+96)
#define     GUILIN_VBUCK_2V45                       (GUILIN_VBUCK_0V6000+97)
#define     GUILIN_VBUCK_2V50                       (GUILIN_VBUCK_0V6000+98)
#define     GUILIN_VBUCK_2V55                       (GUILIN_VBUCK_0V6000+99)
#define     GUILIN_VBUCK_2V60                       (GUILIN_VBUCK_0V6000+100)
#define     GUILIN_VBUCK_2V65                       (GUILIN_VBUCK_0V6000+101)
#define     GUILIN_VBUCK_2V70                       (GUILIN_VBUCK_0V6000+102)
#define     GUILIN_VBUCK_2V75                       (GUILIN_VBUCK_0V6000+103)
#define     GUILIN_VBUCK_2V80                       (GUILIN_VBUCK_0V6000+104)
#define     GUILIN_VBUCK_2V85                       (GUILIN_VBUCK_0V6000+105)
#define     GUILIN_VBUCK_2V90                       (GUILIN_VBUCK_0V6000+106)
#define     GUILIN_VBUCK_2V95                       (GUILIN_VBUCK_0V6000+107)
#define     GUILIN_VBUCK_3V00                       (GUILIN_VBUCK_0V6000+108)
#define     GUILIN_VBUCK_3V05                       (GUILIN_VBUCK_0V6000+109)
#define     GUILIN_VBUCK_3V10                       (GUILIN_VBUCK_0V6000+110)
#define     GUILIN_VBUCK_3V15                       (GUILIN_VBUCK_0V6000+111)
#define     GUILIN_VBUCK_3V20                       (GUILIN_VBUCK_0V6000+112)
#define     GUILIN_VBUCK_3V25                       (GUILIN_VBUCK_0V6000+113)
#define     GUILIN_VBUCK_3V30                       (GUILIN_VBUCK_0V6000+114)
#define     GUILIN_VBUCK_3V35                       (GUILIN_VBUCK_0V6000+115)
#define     GUILIN_VBUCK_3V40                       (GUILIN_VBUCK_0V6000+116)
#define     GUILIN_VBUCK_3V45                       (GUILIN_VBUCK_0V6000+117)
#define     GUILIN_VBUCK_3V50                       (GUILIN_VBUCK_0V6000+118)
#define     GUILIN_VBUCK_3V55                       (GUILIN_VBUCK_0V6000+119)
#define     GUILIN_VBUCK_3V60                       (GUILIN_VBUCK_0V6000+120)
#define     GUILIN_VBUCK_3V65                       (GUILIN_VBUCK_0V6000+121)
#define     GUILIN_VBUCK_3V70                       (GUILIN_VBUCK_0V6000+122)
#define     GUILIN_VBUCK_3V75                       (GUILIN_VBUCK_0V6000+123)
#define     GUILIN_VBUCK_3V80                       (GUILIN_VBUCK_0V6000+124)
#define     GUILIN_VBUCK_3V85                       (GUILIN_VBUCK_0V6000+125)
#define     GUILIN_VBUCK_3V90                       (GUILIN_VBUCK_0V6000+126)
#define     GUILIN_VBUCK_3V95                       (GUILIN_VBUCK_0V6000+127)

//LDO NORMAL mode
//LDO1
#define     GUILIN_LDO_ACTIVE_VOUT_MASK             (0xf << 2)//BIT[5:2] for vout of Active mode

#define     GUILIN_LDO1_ACTIVE_1V20                (0x0 << 2)
#define     GUILIN_LDO1_ACTIVE_1V25                (0x1 << 2)
#define     GUILIN_LDO1_ACTIVE_1V70                (0x2 << 2)
#define     GUILIN_LDO1_ACTIVE_1V80                (0x3 << 2)
#define     GUILIN_LDO1_ACTIVE_1V85                (0x4 << 2)
#define     GUILIN_LDO1_ACTIVE_1V90                (0x5 << 2)
#define     GUILIN_LDO1_ACTIVE_2V50                (0x6 << 2)
#define     GUILIN_LDO1_ACTIVE_2V60                (0x7 << 2)
#define     GUILIN_LDO1_ACTIVE_2V70                (0x8 << 2)
#define     GUILIN_LDO1_ACTIVE_2V75                (0x9 << 2)
#define     GUILIN_LDO1_ACTIVE_2V80                (0xa << 2)
#define     GUILIN_LDO1_ACTIVE_2V85                (0xb << 2)
#define     GUILIN_LDO1_ACTIVE_2V90                (0xc << 2)
#define     GUILIN_LDO1_ACTIVE_3V00                (0xd << 2)
#define     GUILIN_LDO1_ACTIVE_3V10                (0xe << 2)
#define     GUILIN_LDO1_ACTIVE_3V30                (0xf << 2)
//LDO2
#define     GUILIN_LDO2_ACTIVE_1V20                GUILIN_LDO1_ACTIVE_1V20
#define     GUILIN_LDO2_ACTIVE_1V25                GUILIN_LDO1_ACTIVE_1V25
#define     GUILIN_LDO2_ACTIVE_1V70                GUILIN_LDO1_ACTIVE_1V70
#define     GUILIN_LDO2_ACTIVE_1V80                GUILIN_LDO1_ACTIVE_1V80
#define     GUILIN_LDO2_ACTIVE_1V85                GUILIN_LDO1_ACTIVE_1V85
#define     GUILIN_LDO2_ACTIVE_1V90                GUILIN_LDO1_ACTIVE_1V90
#define     GUILIN_LDO2_ACTIVE_2V50                GUILIN_LDO1_ACTIVE_2V50
#define     GUILIN_LDO2_ACTIVE_2V60                GUILIN_LDO1_ACTIVE_2V60
#define     GUILIN_LDO2_ACTIVE_2V70                GUILIN_LDO1_ACTIVE_2V70
#define     GUILIN_LDO2_ACTIVE_2V75                GUILIN_LDO1_ACTIVE_2V75
#define     GUILIN_LDO2_ACTIVE_2V80                GUILIN_LDO1_ACTIVE_2V80
#define     GUILIN_LDO2_ACTIVE_2V85                GUILIN_LDO1_ACTIVE_2V85
#define     GUILIN_LDO2_ACTIVE_2V90                GUILIN_LDO1_ACTIVE_2V90
#define     GUILIN_LDO2_ACTIVE_3V00                GUILIN_LDO1_ACTIVE_3V00
#define     GUILIN_LDO2_ACTIVE_3V10                GUILIN_LDO1_ACTIVE_3V10
#define     GUILIN_LDO2_ACTIVE_3V30                GUILIN_LDO1_ACTIVE_3V30
//LDO3
#define     GUILIN_LDO3_ACTIVE_1V20                GUILIN_LDO1_ACTIVE_1V20
#define     GUILIN_LDO3_ACTIVE_1V25                GUILIN_LDO1_ACTIVE_1V25
#define     GUILIN_LDO3_ACTIVE_1V70                GUILIN_LDO1_ACTIVE_1V70
#define     GUILIN_LDO3_ACTIVE_1V80                GUILIN_LDO1_ACTIVE_1V80
#define     GUILIN_LDO3_ACTIVE_1V85                GUILIN_LDO1_ACTIVE_1V85
#define     GUILIN_LDO3_ACTIVE_1V90                GUILIN_LDO1_ACTIVE_1V90
#define     GUILIN_LDO3_ACTIVE_2V50                GUILIN_LDO1_ACTIVE_2V50
#define     GUILIN_LDO3_ACTIVE_2V60                GUILIN_LDO1_ACTIVE_2V60
#define     GUILIN_LDO3_ACTIVE_2V70                GUILIN_LDO1_ACTIVE_2V70
#define     GUILIN_LDO3_ACTIVE_2V75                GUILIN_LDO1_ACTIVE_2V75
#define     GUILIN_LDO3_ACTIVE_2V80                GUILIN_LDO1_ACTIVE_2V80
#define     GUILIN_LDO3_ACTIVE_2V85                GUILIN_LDO1_ACTIVE_2V85
#define     GUILIN_LDO3_ACTIVE_2V90                GUILIN_LDO1_ACTIVE_2V90
#define     GUILIN_LDO3_ACTIVE_3V00                GUILIN_LDO1_ACTIVE_3V00
#define     GUILIN_LDO3_ACTIVE_3V10                GUILIN_LDO1_ACTIVE_3V10
#define     GUILIN_LDO3_ACTIVE_3V30                GUILIN_LDO1_ACTIVE_3V30
//LDO4
#define     GUILIN_LDO4_ACTIVE_1V20                GUILIN_LDO1_ACTIVE_1V20
#define     GUILIN_LDO4_ACTIVE_1V25                GUILIN_LDO1_ACTIVE_1V25
#define     GUILIN_LDO4_ACTIVE_1V70                GUILIN_LDO1_ACTIVE_1V70
#define     GUILIN_LDO4_ACTIVE_1V80                GUILIN_LDO1_ACTIVE_1V80
#define     GUILIN_LDO4_ACTIVE_1V85                GUILIN_LDO1_ACTIVE_1V85
#define     GUILIN_LDO4_ACTIVE_1V90                GUILIN_LDO1_ACTIVE_1V90
#define     GUILIN_LDO4_ACTIVE_2V50                GUILIN_LDO1_ACTIVE_2V50
#define     GUILIN_LDO4_ACTIVE_2V60                GUILIN_LDO1_ACTIVE_2V60
#define     GUILIN_LDO4_ACTIVE_2V70                GUILIN_LDO1_ACTIVE_2V70
#define     GUILIN_LDO4_ACTIVE_2V75                GUILIN_LDO1_ACTIVE_2V75
#define     GUILIN_LDO4_ACTIVE_2V80                GUILIN_LDO1_ACTIVE_2V80
#define     GUILIN_LDO4_ACTIVE_2V85                GUILIN_LDO1_ACTIVE_2V85
#define     GUILIN_LDO4_ACTIVE_2V90                GUILIN_LDO1_ACTIVE_2V90
#define     GUILIN_LDO4_ACTIVE_3V00                GUILIN_LDO1_ACTIVE_3V00
#define     GUILIN_LDO4_ACTIVE_3V10                GUILIN_LDO1_ACTIVE_3V10
#define     GUILIN_LDO4_ACTIVE_3V30                GUILIN_LDO1_ACTIVE_3V30
//LDO5
#define     GUILIN_LDO5_ACTIVE_1V70               (0x0 << 2)
#define     GUILIN_LDO5_ACTIVE_1V80               (0x1 << 2)
#define     GUILIN_LDO5_ACTIVE_1V90               (0x2 << 2)
#define     GUILIN_LDO5_ACTIVE_2V50               (0x3 << 2)
#define     GUILIN_LDO5_ACTIVE_2V80               (0x4 << 2)
#define     GUILIN_LDO5_ACTIVE_2V90               (0x5 << 2)
#define     GUILIN_LDO5_ACTIVE_3V10               (0x6 << 2)
#define     GUILIN_LDO5_ACTIVE_3V30               (0x7 << 2)
//LDO6
#define     GUILIN_LDO6_ACTIVE_1V70               GUILIN_LDO5_ACTIVE_1V70
#define     GUILIN_LDO6_ACTIVE_1V80               GUILIN_LDO5_ACTIVE_1V80
#define     GUILIN_LDO6_ACTIVE_1V90               GUILIN_LDO5_ACTIVE_1V90
#define     GUILIN_LDO6_ACTIVE_2V50               GUILIN_LDO5_ACTIVE_2V50
#define     GUILIN_LDO6_ACTIVE_2V80               GUILIN_LDO5_ACTIVE_2V80
#define     GUILIN_LDO6_ACTIVE_2V90               GUILIN_LDO5_ACTIVE_2V90
#define     GUILIN_LDO6_ACTIVE_3V10               GUILIN_LDO5_ACTIVE_3V10
#define     GUILIN_LDO6_ACTIVE_3V30               GUILIN_LDO5_ACTIVE_3V30

//SLEEP mode
//LDO1
#define     GUILIN_LDO_SLEEP_VOUT_MASK             (0xf)	//BIT[3:0] ldo vout for sleep mode

#define     GUILIN_LDO1_SLEEP_1V20                (0x0)
#define     GUILIN_LDO1_SLEEP_1V25                (0x1)
#define     GUILIN_LDO1_SLEEP_1V70                (0x2)
#define     GUILIN_LDO1_SLEEP_1V80                (0x3)
#define     GUILIN_LDO1_SLEEP_1V85                (0x4)
#define     GUILIN_LDO1_SLEEP_1V90                (0x5)
#define     GUILIN_LDO1_SLEEP_2V50                (0x6)
#define     GUILIN_LDO1_SLEEP_2V60                (0x7)
#define     GUILIN_LDO1_SLEEP_2V70                (0x8)
#define     GUILIN_LDO1_SLEEP_2V75                (0x9)
#define     GUILIN_LDO1_SLEEP_2V80                (0xa)
#define     GUILIN_LDO1_SLEEP_2V85                (0xb)
#define     GUILIN_LDO1_SLEEP_2V90                (0xc)
#define     GUILIN_LDO1_SLEEP_3V00                (0xd)
#define     GUILIN_LDO1_SLEEP_3V10                (0xe)
#define     GUILIN_LDO1_SLEEP_3V30                (0xf)
//LDO2
#define     GUILIN_LDO2_SLEEP_1V20                GUILIN_LDO1_SLEEP_1V20
#define     GUILIN_LDO2_SLEEP_1V25                GUILIN_LDO1_SLEEP_1V25
#define     GUILIN_LDO2_SLEEP_1V70                GUILIN_LDO1_SLEEP_1V70
#define     GUILIN_LDO2_SLEEP_1V80                GUILIN_LDO1_SLEEP_1V80
#define     GUILIN_LDO2_SLEEP_1V85                GUILIN_LDO1_SLEEP_1V85
#define     GUILIN_LDO2_SLEEP_1V90                GUILIN_LDO1_SLEEP_1V90
#define     GUILIN_LDO2_SLEEP_2V50                GUILIN_LDO1_SLEEP_2V50
#define     GUILIN_LDO2_SLEEP_2V60                GUILIN_LDO1_SLEEP_2V60
#define     GUILIN_LDO2_SLEEP_2V70                GUILIN_LDO1_SLEEP_2V70
#define     GUILIN_LDO2_SLEEP_2V75                GUILIN_LDO1_SLEEP_2V75
#define     GUILIN_LDO2_SLEEP_2V80                GUILIN_LDO1_SLEEP_2V80
#define     GUILIN_LDO2_SLEEP_2V85                GUILIN_LDO1_SLEEP_2V85
#define     GUILIN_LDO2_SLEEP_2V90                GUILIN_LDO1_SLEEP_2V90
#define     GUILIN_LDO2_SLEEP_3V00                GUILIN_LDO1_SLEEP_3V00
#define     GUILIN_LDO2_SLEEP_3V10                GUILIN_LDO1_SLEEP_3V10
#define     GUILIN_LDO2_SLEEP_3V30                GUILIN_LDO1_SLEEP_3V30
//LDO3
#define     GUILIN_LDO3_SLEEP_1V20                GUILIN_LDO1_SLEEP_1V20
#define     GUILIN_LDO3_SLEEP_1V25                GUILIN_LDO1_SLEEP_1V25
#define     GUILIN_LDO3_SLEEP_1V70                GUILIN_LDO1_SLEEP_1V70
#define     GUILIN_LDO3_SLEEP_1V80                GUILIN_LDO1_SLEEP_1V80
#define     GUILIN_LDO3_SLEEP_1V85                GUILIN_LDO1_SLEEP_1V85
#define     GUILIN_LDO3_SLEEP_1V90                GUILIN_LDO1_SLEEP_1V90
#define     GUILIN_LDO3_SLEEP_2V50                GUILIN_LDO1_SLEEP_2V50
#define     GUILIN_LDO3_SLEEP_2V60                GUILIN_LDO1_SLEEP_2V60
#define     GUILIN_LDO3_SLEEP_2V70                GUILIN_LDO1_SLEEP_2V70
#define     GUILIN_LDO3_SLEEP_2V75                GUILIN_LDO1_SLEEP_2V75
#define     GUILIN_LDO3_SLEEP_2V80                GUILIN_LDO1_SLEEP_2V80
#define     GUILIN_LDO3_SLEEP_2V85                GUILIN_LDO1_SLEEP_2V85
#define     GUILIN_LDO3_SLEEP_2V90                GUILIN_LDO1_SLEEP_2V90
#define     GUILIN_LDO3_SLEEP_3V00                GUILIN_LDO1_SLEEP_3V00
#define     GUILIN_LDO3_SLEEP_3V10                GUILIN_LDO1_SLEEP_3V10
#define     GUILIN_LDO3_SLEEP_3V30                GUILIN_LDO1_SLEEP_3V30
//LDO4
#define     GUILIN_LDO4_SLEEP_1V20                GUILIN_LDO1_SLEEP_1V20
#define     GUILIN_LDO4_SLEEP_1V25                GUILIN_LDO1_SLEEP_1V25
#define     GUILIN_LDO4_SLEEP_1V70                GUILIN_LDO1_SLEEP_1V70
#define     GUILIN_LDO4_SLEEP_1V80                GUILIN_LDO1_SLEEP_1V80
#define     GUILIN_LDO4_SLEEP_1V85                GUILIN_LDO1_SLEEP_1V85
#define     GUILIN_LDO4_SLEEP_1V90                GUILIN_LDO1_SLEEP_1V90
#define     GUILIN_LDO4_SLEEP_2V50                GUILIN_LDO1_SLEEP_2V50
#define     GUILIN_LDO4_SLEEP_2V60                GUILIN_LDO1_SLEEP_2V60
#define     GUILIN_LDO4_SLEEP_2V70                GUILIN_LDO1_SLEEP_2V70
#define     GUILIN_LDO4_SLEEP_2V75                GUILIN_LDO1_SLEEP_2V75
#define     GUILIN_LDO4_SLEEP_2V80                GUILIN_LDO1_SLEEP_2V80
#define     GUILIN_LDO4_SLEEP_2V85                GUILIN_LDO1_SLEEP_2V85
#define     GUILIN_LDO4_SLEEP_2V90                GUILIN_LDO1_SLEEP_2V90
#define     GUILIN_LDO4_SLEEP_3V00                GUILIN_LDO1_SLEEP_3V00
#define     GUILIN_LDO4_SLEEP_3V10                GUILIN_LDO1_SLEEP_3V10
#define     GUILIN_LDO4_SLEEP_3V30                GUILIN_LDO1_SLEEP_3V30
//LDO5
#define     GUILIN_LDO5_SLEEP_1V70               (0x00)
#define     GUILIN_LDO5_SLEEP_1V80               (0x01)
#define     GUILIN_LDO5_SLEEP_1V90               (0x02)
#define     GUILIN_LDO5_SLEEP_2V50               (0x03)
#define     GUILIN_LDO5_SLEEP_2V80               (0x04)
#define     GUILIN_LDO5_SLEEP_2V90               (0x05)
#define     GUILIN_LDO5_SLEEP_3V10               (0x06)
#define     GUILIN_LDO5_SLEEP_3V30               (0x07)
//LDO6
#define     GUILIN_LDO6_SLEEP_1V70               GUILIN_LDO5_SLEEP_1V70
#define     GUILIN_LDO6_SLEEP_1V80               GUILIN_LDO5_SLEEP_1V80
#define     GUILIN_LDO6_SLEEP_1V90               GUILIN_LDO5_SLEEP_1V90
#define     GUILIN_LDO6_SLEEP_2V50               GUILIN_LDO5_SLEEP_2V50
#define     GUILIN_LDO6_SLEEP_2V80               GUILIN_LDO5_SLEEP_2V80
#define     GUILIN_LDO6_SLEEP_2V90               GUILIN_LDO5_SLEEP_2V90
#define     GUILIN_LDO6_SLEEP_3V10               GUILIN_LDO5_SLEEP_3V10
#define     GUILIN_LDO6_SLEEP_3V30               GUILIN_LDO5_SLEEP_3V30

#define     GUILIN_LDO5_1V70                	(0x0)
#define     GUILIN_LDO5_1V80                	(0x1)
#define     GUILIN_LDO5_1V90                	(0x2)
#define     GUILIN_LDO5_2V50                	(0x3)
#define     GUILIN_LDO5_2V80                	(0x4)
#define     GUILIN_LDO5_2V90                	(0x5)
#define     GUILIN_LDO5_3V10                	(0x6)
#define     GUILIN_LDO5_3V30                	(0x7)


#define     GUILIN_VBUCK1_ACTIVE_VOUT_REG       (0x20)
#define     GUILIN_VBUCK2_ACTIVE_VOUT_REG       (0x30)
#define     GUILIN_VBUCK3_ACTIVE_VOUT_REG       (0x40)
#define     GUILIN_VBUCK4_ACTIVE_VOUT_REG       (0x50)
#define     GUILIN_VBUCK5_ACTIVE_VOUT_REG       (0x60)

#define     GUILIN_VBUCK1_SLEEP_VOUT_REG		(0x21)
#define     GUILIN_VBUCK2_SLEEP_VOUT_REG		(0x31)
#define     GUILIN_VBUCK3_SLEEP_VOUT_REG		(0x41)
#define     GUILIN_VBUCK4_SLEEP_VOUT_REG		(0x51)
#define     GUILIN_VBUCK5_SLEEP_VOUT_REG		(0x61)

#define     GUILIN_VBUCK1_FSM_REG1			    (0x22)
#define     GUILIN_VBUCK1_FSM_REG2			    (0x23)
#define     GUILIN_VBUCK1_FSM_REG3			    (0x24)
#define     GUILIN_VBUCK1_FSM_REG4			    (0x25)
#define     GUILIN_VBUCK1_FSM_REG5			    (0x26)
#define     GUILIN_VBUCK1_FSM_REG6			    (0x27)
#define     GUILIN_VBUCK1_FSM_REG7			    (0x28)
                                            
#define     GUILIN_VBUCK2_FSM_REG1			    (0x32)
#define     GUILIN_VBUCK2_FSM_REG2			    (0x33)
#define     GUILIN_VBUCK2_FSM_REG3			    (0x34)
#define     GUILIN_VBUCK2_FSM_REG4			    (0x35)
#define     GUILIN_VBUCK2_FSM_REG5			    (0x36)
#define     GUILIN_VBUCK2_FSM_REG6			    (0x37)
#define     GUILIN_VBUCK2_FSM_REG7			    (0x38)
                                            
#define     GUILIN_VBUCK3_FSM_REG1			    (0x42)
#define     GUILIN_VBUCK3_FSM_REG2			    (0x43)
#define     GUILIN_VBUCK3_FSM_REG3			    (0x44)
#define     GUILIN_VBUCK3_FSM_REG4			    (0x45)
#define     GUILIN_VBUCK3_FSM_REG5			    (0x46)
#define     GUILIN_VBUCK3_FSM_REG6			    (0x47)
#define     GUILIN_VBUCK3_FSM_REG7			    (0x48)

#define     GUILIN_VBUCK4_FSM_REG1			    (0x52)
#define     GUILIN_VBUCK4_FSM_REG2			    (0x53)
#define     GUILIN_VBUCK4_FSM_REG3			    (0x54)
#define     GUILIN_VBUCK4_FSM_REG4			    (0x55)
#define     GUILIN_VBUCK4_FSM_REG5			    (0x56)
#define     GUILIN_VBUCK4_FSM_REG6			    (0x57)
#define     GUILIN_VBUCK4_FSM_REG7			    (0x58)

#define     GUILIN_VBUCK5_FSM_REG1			    (0x62)
#define     GUILIN_VBUCK5_FSM_REG2			    (0x63)
#define     GUILIN_VBUCK5_FSM_REG3			    (0x64)
#define     GUILIN_VBUCK5_FSM_REG4			    (0x65)
#define     GUILIN_VBUCK5_FSM_REG5			    (0x66)
#define     GUILIN_VBUCK5_FSM_REG6			    (0x67)
#define     GUILIN_VBUCK5_FSM_REG7			    (0x68)

#define	    GUILIN_VBUCK1_DVC_ENABLE_REG		GUILIN_VBUCK1_SLEEP_VOUT_REG
#define	    GUILIN_VBUCK4_DVC_ENABLE_REG		GUILIN_VBUCK4_SLEEP_VOUT_REG

#define	    GUILIN_VBUCK1_ENABLE_REG		GUILIN_VBUCK1_ACTIVE_VOUT_REG
#define	    GUILIN_VBUCK2_ENABLE_REG		GUILIN_VBUCK2_ACTIVE_VOUT_REG
#define	    GUILIN_VBUCK3_ENABLE_REG		GUILIN_VBUCK3_ACTIVE_VOUT_REG
#define	    GUILIN_VBUCK4_ENABLE_REG		GUILIN_VBUCK4_ACTIVE_VOUT_REG
#define	    GUILIN_VBUCK5_ENABLE_REG		GUILIN_VBUCK5_ACTIVE_VOUT_REG

#define     GUILIN_VBUCK1_SLEEP_MODE_REG			GUILIN_VBUCK1_FSM_REG1
#define     GUILIN_VBUCK2_SLEEP_MODE_REG			GUILIN_VBUCK2_FSM_REG1
#define     GUILIN_VBUCK3_SLEEP_MODE_REG			GUILIN_VBUCK3_FSM_REG1
#define     GUILIN_VBUCK4_SLEEP_MODE_REG			GUILIN_VBUCK4_FSM_REG1
#define     GUILIN_VBUCK5_SLEEP_MODE_REG			GUILIN_VBUCK5_FSM_REG1

#define	    GUILIN_VBUCK1_DVC_VOUT_REG_SET0			(0x29)
#define	    GUILIN_VBUCK1_DVC_VOUT_REG_SET1			(0x2a)
#define	    GUILIN_VBUCK1_DVC_VOUT_REG_SET2			(0x2b)
#define	    GUILIN_VBUCK1_DVC_VOUT_REG_SET3			(0x2c)

#define	    GUILIN_VBUCK4_DVC_VOUT_REG_SET0			(0x59)
#define	    GUILIN_VBUCK4_DVC_VOUT_REG_SET1			(0x5a)
#define	    GUILIN_VBUCK4_DVC_VOUT_REG_SET2			(0x5b)
#define	    GUILIN_VBUCK4_DVC_VOUT_REG_SET3			(0x5c)

#define		GUILIN_CONTAIN_VBUCK_DVC_VOUT_MASK		(0x7f)


#define		GUILIN_CONTAIN_VBUCK_EN_BIT(x)		(x==GUILIN_VBUCK1_ENABLE_REG|| \
											x==GUILIN_VBUCK2_ENABLE_REG|| \
											x==GUILIN_VBUCK3_ENABLE_REG|| \
											x==GUILIN_VBUCK4_ENABLE_REG|| \
											x==GUILIN_VBUCK5_ENABLE_REG)

#define		GUILIN_CONTAIN_VBUCK_ACTIVE_VOUT_BIT(x)		GUILIN_CONTAIN_VBUCK_EN_BIT(x)

#define		GUILIN_CONTAIN_VBUCK_SLEEP_VOUT_BIT(x)	(x==GUILIN_VBUCK1_SLEEP_VOUT_REG|| \
												x==GUILIN_VBUCK2_SLEEP_VOUT_REG|| \
												x==GUILIN_VBUCK3_SLEEP_VOUT_REG|| \
												x==GUILIN_VBUCK4_SLEEP_VOUT_REG|| \
												x==GUILIN_VBUCK5_SLEEP_VOUT_REG)

#define		GUILIN_CONTAIN_VBUCK_DVC_VOUT_BIT(x)	(x==GUILIN_VBUCK1_DVC_VOUT_REG_SET0|| \
												x==GUILIN_VBUCK1_DVC_VOUT_REG_SET1|| \
												x==GUILIN_VBUCK1_DVC_VOUT_REG_SET2|| \
												x==GUILIN_VBUCK1_DVC_VOUT_REG_SET3|| \
												x==GUILIN_VBUCK4_DVC_VOUT_REG_SET0|| \
												x==GUILIN_VBUCK4_DVC_VOUT_REG_SET1|| \
												x==GUILIN_VBUCK4_DVC_VOUT_REG_SET2|| \
												x==GUILIN_VBUCK4_DVC_VOUT_REG_SET3)



#define		GUILIN_CONTAIN_VBUCK_SLEEP_MODE_BIT(x) 		(x==GUILIN_VBUCK1_SLEEP_MODE_REG|| \
												x==GUILIN_VBUCK2_SLEEP_MODE_REG|| \
												x==GUILIN_VBUCK3_SLEEP_MODE_REG|| \
												x==GUILIN_VBUCK4_SLEEP_MODE_REG|| \
												x==GUILIN_VBUCK5_SLEEP_MODE_REG)

#define		GUILIN_CONTAIN_VBUCK_DVC_EN_BIT(x) 		(x==GUILIN_VBUCK1_DVC_ENABLE_REG|| \
												x==GUILIN_VBUCK4_DVC_ENABLE_REG)


#define		GUILIN_VBUCK_ENABLE_MASK			(0x1<<7)
#define		GUILIN_VBUCK_ENABLE_DVC_MASK		(0x1<<7)
#define		GUILIN_VBUCK_SLEEP_MODE_MASK		(0x3<<3)

//00	:	BUCK OFF
//01	:	BUCK ACTIVE mode
//10	:	BUCK SLEEP mode
//11	:	BUCK NORMAL mode
#define		GUILIN_VBUCK_OFF			(0x0 <<3)
#define		GUILIN_VBUCK_ACTIVE_SLEEP	(0x1 <<3)
#define		GUILIN_VBUCK_SLEEP			(0x2 <<3)
#define		GUILIN_VBUCK_ACTIVE			(0x3 <<3)



#define		GUILIN_CONTAIN_LDO_EN_BIT(x) 		(x==GUILIN_LDO1_ENABLE_REG|| \
											x==GUILIN_LDO2_ENABLE_REG|| \
											x==GUILIN_LDO3_ENABLE_REG|| \
											x==GUILIN_LDO4_ENABLE_REG|| \
											x==GUILIN_LDO5_ENABLE_REG|| \
											x==GUILIN_LDO6_ENABLE_REG)

#define		GUILIN_CONTAIN_LDO_ACTIVE_VOUT_BIT(x)	GUILIN_CONTAIN_LDO_EN_BIT(x)


#define		GUILIN_CONTAIN_LDO_SLEEP_MODE_BIT(x) 		(x==GUILIN_LDO1_SLEEP_MODE_REG|| \
												x==GUILIN_LDO2_SLEEP_MODE_REG|| \
												x==GUILIN_LDO3_SLEEP_MODE_REG|| \
												x==GUILIN_LDO4_SLEEP_MODE_REG|| \
												x==GUILIN_LDO5_SLEEP_MODE_REG|| \
												x==GUILIN_LDO6_SLEEP_MODE_REG)
#define		GUILIN_CONTAIN_LDO_SLEEP_VOUT_BIT(x)	GUILIN_CONTAIN_LDO_SLEEP_MODE_BIT(x)



#define		GUILIN_LDO_ENABLE_MASK				(0x1<<6)
#define		GUILIN_LDO_SLEEP_MODE_MASK			(0x3<<4)

//00	:	LDO OFF
//01	:	Reserve
//10	:	LDO SLEEP mode
//11	:	LDO NORMAL mode
#define     GUILIN_LDO_OFF                      (0x0 <<4)
#define     GUILIN_LDO_SLEEP                    (0x2 <<4)
#define     GUILIN_LDO_ACTIVE                   (0x3 <<4)


//POWER_PAGE_ADDR=0x62 , BIT[6:5] (PS: hand confirmed with ASIC bit6 & bit5 , should set the same value)
//00	:	disable APT
//11	:	enable APT
//confirm with
#define		GUILIN_APT_REG						(0x62)
#define		GUILIN_APT_ENABLE_MASK				(0x3<<5)


//BASE_PAGE_ADDR=0xE2 , BIT[3:0]
#define		GUILIN_RESET_DISCHARGE_REG			(0xe2)
#define		GUILIN_RESET_DISCHARGE_MASK			(0xf)

#define		GUILIN_FAULT_WU_REG					(0xe7)
#define		GUILIN_FAULT_WU_BIT					(0x1<<3)
#define		GUILIN_FAULT_WU_ENABLE_BIT			(0x1<<2)

#define		GUILIN_RESET_REG				(0x0d)
#define		GUILIN_SW_PDOWN_BIT				(0x1<<5)
#define		GUILIN_RESET_PMIC_BIT			(0x1<<6)

#define		GUILIN_PWR_HOLD_REG				GUILIN_RESET_REG
#define		GUILIN_PWR_HOLD_BIT				(0x1<<7)

#define 	GUILIN_BUCK_ENABLE                   (0x1<<7)
#define 	GUILIN_LDO_ENABLE                    (0x1<<6)

#define		GUILIN_I2C_CotlAddr		     I2CRegBaseAddress

/* Guilin I2c address*/
#define GUILIN_BASE_SLAVE_WRITE_ADDR		     0x60
#define GUILIN_BASE_SLAVE_READ_ADDR	             0x61

#define GUILIN_POWER_SLAVE_WRITE_ADDR		     0x62
#define GUILIN_POWER_SLAVE_READ_ADDR	         0x63

#define GUILIN_GPADC_SLAVE_WRITE_ADDR		     0x64
#define GUILIN_GPADC_SLAVE_READ_ADDR	         0x65

/* There are 3 I2C Address for Guilin.  */
typedef enum
{
    GUILIN_BASE_Reg,
    GUILIN_POWER_Reg,
    GUILIN_GPADC_Reg
}Guilin_Reg_Type;
typedef enum {
	//INT ENABLE REG 2 addr=0x0a
	GUILIN_TINT_INT=0,
	GUILIN_GPADC0_INT,
	GUILIN_GPADC1_INT,
	GUILIN_VINLDO_INT,
	GUILIN_VINLDO_SLP_INT,

	//INT ENABLE REG 1 addr=0x09
	GUILIN_ONKEY_INT=8,
	GUILIN_EXTON1_INT,
	GUILIN_EXTON2_INT,
	GUILIN_BAT_INT,
	GUILIN_RTC_ALARM_INT,
} GUILIN_INTC ;

#define GUILIN_INTC_MAX 15
#define GUILIN_INTC_TO_STATUS_BIT(intc) (0x01<<intc)
#define GUILIN_INTC_TO_ENABLE_BIT(intc) (1<<(intc%8))
#define GUILIN_INTC_TO_ENABLE_REG(intc) (GUILIN_INT_ENABLE_REG2 - (intc/8))

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinRead                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read Guilin by PI2C interface.                      */
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
int GuilinRead( Guilin_Reg_Type guilin_reg_type, unsigned char reg, unsigned char *value );


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinWite                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Write Guilin by PI2C interface.                     */
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
int GuilinWrite( Guilin_Reg_Type guilin_reg_type, unsigned char reg, unsigned char value );

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      guilin_read_volt_meas_val                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read ADCs with voltage of Guilin PMIC.              */
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
unsigned short guilin_read_volt_meas_val(unsigned char meaReg);
#endif
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Guilin_VsimSleep_Enable                                          */
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
void Guilin_VsimSleep_Enable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Guilin_VsimSleep_Disable                                         */
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
void Guilin_VsimSleep_Disable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Guilin_miccoDisableUsimV                                         */
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
void Guilin_miccoDisableUsimV(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Guilin_miccoEnableUsimV                                          */
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
void Guilin_miccoEnableUsimV(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      Guilin_miccoConfigUsimV                                          */
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
void Guilin_miccoConfigUsimV(unsigned char volatge);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      DisableGuilinWDT                                                 */
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
void DisableGuilinWDT( void );

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinClkInit                                                    */
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
void GuilinClkInit( void );


int Guilin_VBUCK_Set_Enable(unsigned char reg, unsigned char enable);
int Guilin_VBUCK_Set_DVC_Enable(unsigned char reg, unsigned char enable);
int Guilin_VBUCK_Set_Slpmode(unsigned char reg, unsigned char mode);
int Guilin_VBUCK_Set_VOUT(unsigned char reg, unsigned char value);
int Guilin_LDO_Set_Enable(unsigned char reg, unsigned char enable);
int Guilin_LDO_Set_Slpmode(unsigned char reg, unsigned char mode);
int Guilin_LDO_Set_VOUT(unsigned char reg, unsigned char value);
int Guilin_APT_Set(unsigned char enable);
int Guilin_SW_Reset(void );
void Guilin_Dump_PMIC_Register(void);
void Guilin_Aditional_Workaround(void);


#endif /* _GUILIN_H_        */
