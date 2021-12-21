#ifndef     _GUILIN_LITE_H_
#define     _GUILIN_LITE_H_

/*************************************************************************
*               Included Files.
**************************************************************************/
#include "common.h"

#define GUILIN_LITE_DEBUG 0

#if GUILIN_LITE_DEBUG
#define GUILIN_LITE_UART_DEBUG(fmt,...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define GUILIN_LITE_UART_DEBUG(fmt,...)
#endif

/***************************************************************************
*                       Typedefines  & Macros
****************************************************************************/
/* GuilinLite Registers defines.                        */
/* (1)GUILIN_LITE BASE REGISTER                          */

#define     GUILIN_LITE_ID_REG                       0x00            /* Identification Register.                         */

#define     GUILIN_LITE_STATUS_REG1                  0x01
#define     GUILIN_LITE_BAT_DETECT                   0x08            /* Battery is present.                              */
#define     GUILIN_LITE_RTC_ALARM_STATUS_BIT         0x04            /* RTC_ALARM = 1.                               */
#define     GUILIN_LITE_EXTON1_DETECT                0x02            /* EXTON1_DETECT = 1.                               */
#define     GUILIN_LITE_ONKEY_PRESSED                0x01            /* ONKEYn is Pressed.                               */

#define     GUILIN_LITE_INT_STATUS_REG1              0x05            /* Interrupt Status Register1.                      */
#define     GUILIN_LITE_BAT_INT_OCCURRED             0x08            /* Set,when BAT_DET changes.                        */
#define     GUILIN_LITE_RTC_INT_OCCURRED             0x04            /* Set,when RTC alarm occurred.                     */
#define     GUILIN_LITE_EXTON1_INT_OCCURRED          0x02            /* Set,when EXTON1 charged.                         */
#define     GUILIN_LITE_ONKEY_INT_OCCURRED           0x01            /* Set,when ONKEY changed.                          */

#define     GUILIN_LITE_INT_ENABLE_REG1              0x09            /* Interrupt Enable Register1.                      */
#define     GUILIN_LITE_BAT_INT_EN                   0x08            /* BAT Interrupt Enable.                            */
#define     GUILIN_LITE_RTC_INT_EN                   0x04            /* RTC Interrupt Enable.                            */
#define     GUILIN_LITE_EXTON1_INT_EN                0x02            /* EXTON1 Interrupt Enable.                         */
#define     GUILIN_LITE_ONKEY_INT_EN                 0x01            /* ONKEY Interrupt Enable.                          */

#define     GUILIN_LITE_WAKEUP_REG1                  0x0d            /* Wakeup Register1.                                */
#define     GUILIN_LITE_POWER_HOLD                   0x80            /* LPF & DVC enable, SLEEPn disable.                */
#define     GUILIN_LITE_RESET_PMIC_REG               0x40            /* Reset Ustica registers.                          */
#define     GUILIN_LITE_SW_PDOWN                     0x20            /* Entrance to 'power-down' sate.                   */
#define     GUILIN_LITE_WD_RESET                     0x10            /* Resets the Watchdog timer.                       */
#define     GUILIN_LITE_WD_MODE                      0x01            /* WD1#->toggle RESET_OUTn,WD2#->PowerDown.         */

#define 	GUILIN_LITE_PWRUP_LOG_REG				0x10

#define     GUILIN_LITE_WD_REG                       0x1D            /* Watchdog Register                                */
#define     GUILIN_LITE_WD_DIS                       0x01            /* Watchdog disable.                                */

#define     GUILIN_LITE_RTC_CTRL_REG	             0xD0            /* RTC_Control>.                                */
#define     GUILIN_LITE_RTC_DIS_ALARM_PULLDOWN_BIT   (0x01<<7)
#define     GUILIN_LITE_RTC_OTP_RELOAD_DISABLE_BIT   (0x01<<6)

#define     GUILIN_LITE_RTC_MISC_3_REG				0xE3

#define     GUILIN_LITE_RTC_USER_DATA_0_REG          0xD1            /* User defined region */
#define     GUILIN_LITE_RTC_USER_DATA_1_REG          0xD2
#define     GUILIN_LITE_RTC_USER_DATA_2_REG          0xD3
#define     GUILIN_LITE_RTC_USER_DATA_3_REG          0xD4
#define     GUILIN_LITE_RTC_USERDATA_SETTING_MARK_REG GUILIN_LITE_RTC_USER_DATA_3_REG  //reuse RTC_EXPIRE_2 as APP_OFFSET

#define     GUILIN_LITE_CLK_32K_SEL_REG				0xE4
#define     GUILIN_LITE_LONGKEY_EN2                  0x02             /* When enabled, the event will cause power-down.   */
#define     GUILIN_LITE_LONGKEY_EN1                  0x01             /* When enabled, the event will cause power-down.   */

#define 	GUILIN_LITE_LONGKEY_1					0
#define 	GUILIN_LITE_LONGKEY_2					1

#define     GUILIN_LITE_POWERDOWN_LOG_REG				0xE5
#define     GUILIN_LITE_POWERDOWN_LOG_REG2				0xE6

/* (1)GUILIN_LITE POWER REGISTER                          */

//LDO
#define     GUILIN_LITE_LDO1_ACTIVE_VOUT_REG         0x71
#define     GUILIN_LITE_LDO2_ACTIVE_VOUT_REG         0x74
#define     GUILIN_LITE_LDO3_ACTIVE_VOUT_REG         0x77
#define     GUILIN_LITE_LDO4_ACTIVE_VOUT_REG         0x7A
#define     GUILIN_LITE_LDO5_ACTIVE_VOUT_REG         0x7D
#define     GUILIN_LITE_LDO6_ACTIVE_VOUT_REG         0x80
#define     GUILIN_LITE_LDO7_ACTIVE_VOUT_REG         0x83
#define     GUILIN_LITE_LDO8_ACTIVE_VOUT_REG         0x86

#define     GUILIN_LITE_LDO1_SLEEP_VOUT_REG          0x70
#define     GUILIN_LITE_LDO2_SLEEP_VOUT_REG          0x73
#define     GUILIN_LITE_LDO3_SLEEP_VOUT_REG          0x76
#define     GUILIN_LITE_LDO4_SLEEP_VOUT_REG          0x79
#define     GUILIN_LITE_LDO5_SLEEP_VOUT_REG          0x7C
#define     GUILIN_LITE_LDO6_SLEEP_VOUT_REG          0x7F
#define     GUILIN_LITE_LDO7_SLEEP_VOUT_REG          0x82
#define     GUILIN_LITE_LDO8_SLEEP_VOUT_REG          0x85

#define     GUILIN_LITE_LDO1_ENABLE_REG			GUILIN_LITE_LDO1_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO2_ENABLE_REG			GUILIN_LITE_LDO2_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO3_ENABLE_REG			GUILIN_LITE_LDO3_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO4_ENABLE_REG			GUILIN_LITE_LDO4_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO5_ENABLE_REG			GUILIN_LITE_LDO5_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO6_ENABLE_REG			GUILIN_LITE_LDO6_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO7_ENABLE_REG			GUILIN_LITE_LDO7_ACTIVE_VOUT_REG
#define     GUILIN_LITE_LDO8_ENABLE_REG			GUILIN_LITE_LDO8_ACTIVE_VOUT_REG

#define     GUILIN_LITE_LDO1_SLEEP_MODE_REG		GUILIN_LITE_LDO1_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO2_SLEEP_MODE_REG		GUILIN_LITE_LDO2_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO3_SLEEP_MODE_REG		GUILIN_LITE_LDO3_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO4_SLEEP_MODE_REG		GUILIN_LITE_LDO4_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO5_SLEEP_MODE_REG		GUILIN_LITE_LDO5_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO6_SLEEP_MODE_REG		GUILIN_LITE_LDO6_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO7_SLEEP_MODE_REG		GUILIN_LITE_LDO7_SLEEP_VOUT_REG
#define     GUILIN_LITE_LDO8_SLEEP_MODE_REG		GUILIN_LITE_LDO8_SLEEP_VOUT_REG

//VBUCK
//step 0.125v, from 0.6v to 1.6v
#define	    GUILIN_LITE_CONTAIN_VBUCK_ACTIVE_VOUT_MASK	(0x7f)
#define     GUILIN_LITE_CONTAIN_VBUCK_SLEEP_VOUT_MASK	(0x7f)


#define     GUILIN_LITE_VBUCK_0V6000	                 (0x00)
#define     GUILIN_LITE_VBUCK_0V6125	                 (GUILIN_LITE_VBUCK_0V6000+1)
#define     GUILIN_LITE_VBUCK_0V6250	                 (GUILIN_LITE_VBUCK_0V6000+2)
#define     GUILIN_LITE_VBUCK_0V6375	                 (GUILIN_LITE_VBUCK_0V6000+3)
#define     GUILIN_LITE_VBUCK_0V6500	                 (GUILIN_LITE_VBUCK_0V6000+4)
#define     GUILIN_LITE_VBUCK_0V6625	                 (GUILIN_LITE_VBUCK_0V6000+5)
#define     GUILIN_LITE_VBUCK_0V6750	                 (GUILIN_LITE_VBUCK_0V6000+6)
#define     GUILIN_LITE_VBUCK_0V6875	                 (GUILIN_LITE_VBUCK_0V6000+7)
#define     GUILIN_LITE_VBUCK_0V7000                     (GUILIN_LITE_VBUCK_0V6000+8)
#define     GUILIN_LITE_VBUCK_0V7125                     (GUILIN_LITE_VBUCK_0V6000+9)
#define     GUILIN_LITE_VBUCK_0V7250                     (GUILIN_LITE_VBUCK_0V6000+10)
#define     GUILIN_LITE_VBUCK_0V7375                     (GUILIN_LITE_VBUCK_0V6000+11)
#define     GUILIN_LITE_VBUCK_0V7500                     (GUILIN_LITE_VBUCK_0V6000+12)
#define     GUILIN_LITE_VBUCK_0V7625                     (GUILIN_LITE_VBUCK_0V6000+13)
#define     GUILIN_LITE_VBUCK_0V7750                     (GUILIN_LITE_VBUCK_0V6000+14)
#define     GUILIN_LITE_VBUCK_0V7875                     (GUILIN_LITE_VBUCK_0V6000+15)
#define     GUILIN_LITE_VBUCK_0V8000                     (GUILIN_LITE_VBUCK_0V6000+16)
#define     GUILIN_LITE_VBUCK_0V8125                     (GUILIN_LITE_VBUCK_0V6000+17)
#define     GUILIN_LITE_VBUCK_0V8250                     (GUILIN_LITE_VBUCK_0V6000+18)
#define     GUILIN_LITE_VBUCK_0V8375                     (GUILIN_LITE_VBUCK_0V6000+19)
#define     GUILIN_LITE_VBUCK_0V8500                     (GUILIN_LITE_VBUCK_0V6000+20)
#define     GUILIN_LITE_VBUCK_0V8625                     (GUILIN_LITE_VBUCK_0V6000+21)
#define     GUILIN_LITE_VBUCK_0V8750                     (GUILIN_LITE_VBUCK_0V6000+22)
#define     GUILIN_LITE_VBUCK_0V8875                     (GUILIN_LITE_VBUCK_0V6000+23)
#define     GUILIN_LITE_VBUCK_0V9000                     (GUILIN_LITE_VBUCK_0V6000+24)
#define     GUILIN_LITE_VBUCK_0V9125                     (GUILIN_LITE_VBUCK_0V6000+25)
#define     GUILIN_LITE_VBUCK_0V9250                     (GUILIN_LITE_VBUCK_0V6000+26)
#define     GUILIN_LITE_VBUCK_0V9375                     (GUILIN_LITE_VBUCK_0V6000+27)
#define     GUILIN_LITE_VBUCK_0V9500                     (GUILIN_LITE_VBUCK_0V6000+28)
#define     GUILIN_LITE_VBUCK_0V9625                     (GUILIN_LITE_VBUCK_0V6000+29)
#define     GUILIN_LITE_VBUCK_0V9750                     (GUILIN_LITE_VBUCK_0V6000+30)
#define     GUILIN_LITE_VBUCK_0V9875                     (GUILIN_LITE_VBUCK_0V6000+31)
#define     GUILIN_LITE_VBUCK_1V0000                     (GUILIN_LITE_VBUCK_0V6000+32)
#define     GUILIN_LITE_VBUCK_1V0125                     (GUILIN_LITE_VBUCK_0V6000+33)
#define     GUILIN_LITE_VBUCK_1V0250                     (GUILIN_LITE_VBUCK_0V6000+34)
#define     GUILIN_LITE_VBUCK_1V0375                     (GUILIN_LITE_VBUCK_0V6000+35)
#define     GUILIN_LITE_VBUCK_1V0500                     (GUILIN_LITE_VBUCK_0V6000+36)
#define     GUILIN_LITE_VBUCK_1V0625                     (GUILIN_LITE_VBUCK_0V6000+37)
#define     GUILIN_LITE_VBUCK_1V0750                     (GUILIN_LITE_VBUCK_0V6000+38)
#define     GUILIN_LITE_VBUCK_1V0875                     (GUILIN_LITE_VBUCK_0V6000+39)
#define     GUILIN_LITE_VBUCK_1V1000                     (GUILIN_LITE_VBUCK_0V6000+40)
#define     GUILIN_LITE_VBUCK_1V1125                     (GUILIN_LITE_VBUCK_0V6000+41)
#define     GUILIN_LITE_VBUCK_1V1250                     (GUILIN_LITE_VBUCK_0V6000+42)
#define     GUILIN_LITE_VBUCK_1V1375                     (GUILIN_LITE_VBUCK_0V6000+43)
#define     GUILIN_LITE_VBUCK_1V1500                     (GUILIN_LITE_VBUCK_0V6000+44)
#define     GUILIN_LITE_VBUCK_1V1625                     (GUILIN_LITE_VBUCK_0V6000+45)
#define     GUILIN_LITE_VBUCK_1V1750                     (GUILIN_LITE_VBUCK_0V6000+46)
#define     GUILIN_LITE_VBUCK_1V1875                     (GUILIN_LITE_VBUCK_0V6000+47)
#define     GUILIN_LITE_VBUCK_1V2000                     (GUILIN_LITE_VBUCK_0V6000+48)
#define     GUILIN_LITE_VBUCK_1V2125                     (GUILIN_LITE_VBUCK_0V6000+49)
#define     GUILIN_LITE_VBUCK_1V2250                     (GUILIN_LITE_VBUCK_0V6000+50)
#define     GUILIN_LITE_VBUCK_1V2375                     (GUILIN_LITE_VBUCK_0V6000+51)
#define     GUILIN_LITE_VBUCK_1V2500                     (GUILIN_LITE_VBUCK_0V6000+52)
#define     GUILIN_LITE_VBUCK_1V2625                     (GUILIN_LITE_VBUCK_0V6000+53)
#define     GUILIN_LITE_VBUCK_1V2750                     (GUILIN_LITE_VBUCK_0V6000+54)
#define     GUILIN_LITE_VBUCK_1V2875                     (GUILIN_LITE_VBUCK_0V6000+55)
#define     GUILIN_LITE_VBUCK_1V3000                     (GUILIN_LITE_VBUCK_0V6000+56)
#define     GUILIN_LITE_VBUCK_1V3125                     (GUILIN_LITE_VBUCK_0V6000+57)
#define     GUILIN_LITE_VBUCK_1V3250                     (GUILIN_LITE_VBUCK_0V6000+58)
#define     GUILIN_LITE_VBUCK_1V3375                     (GUILIN_LITE_VBUCK_0V6000+59)
#define     GUILIN_LITE_VBUCK_1V3500                     (GUILIN_LITE_VBUCK_0V6000+60)
#define     GUILIN_LITE_VBUCK_1V3625                     (GUILIN_LITE_VBUCK_0V6000+61)
#define     GUILIN_LITE_VBUCK_1V3750                     (GUILIN_LITE_VBUCK_0V6000+62)
#define     GUILIN_LITE_VBUCK_1V3875                     (GUILIN_LITE_VBUCK_0V6000+63)
#define     GUILIN_LITE_VBUCK_1V4000                     (GUILIN_LITE_VBUCK_0V6000+64)
#define     GUILIN_LITE_VBUCK_1V4125                     (GUILIN_LITE_VBUCK_0V6000+65)
#define     GUILIN_LITE_VBUCK_1V4250                     (GUILIN_LITE_VBUCK_0V6000+66)
#define     GUILIN_LITE_VBUCK_1V4375                     (GUILIN_LITE_VBUCK_0V6000+67)
#define     GUILIN_LITE_VBUCK_1V4500                     (GUILIN_LITE_VBUCK_0V6000+68)
#define     GUILIN_LITE_VBUCK_1V4625                     (GUILIN_LITE_VBUCK_0V6000+69)
#define     GUILIN_LITE_VBUCK_1V4750                     (GUILIN_LITE_VBUCK_0V6000+70)
#define     GUILIN_LITE_VBUCK_1V4875                     (GUILIN_LITE_VBUCK_0V6000+71)
#define     GUILIN_LITE_VBUCK_1V5000                     (GUILIN_LITE_VBUCK_0V6000+72)
#define     GUILIN_LITE_VBUCK_1V5125                     (GUILIN_LITE_VBUCK_0V6000+73)
#define     GUILIN_LITE_VBUCK_1V5250                     (GUILIN_LITE_VBUCK_0V6000+74)
#define     GUILIN_LITE_VBUCK_1V5375                     (GUILIN_LITE_VBUCK_0V6000+75)
#define     GUILIN_LITE_VBUCK_1V5500                     (GUILIN_LITE_VBUCK_0V6000+76)
#define     GUILIN_LITE_VBUCK_1V5625                     (GUILIN_LITE_VBUCK_0V6000+77)
#define     GUILIN_LITE_VBUCK_1V5750                     (GUILIN_LITE_VBUCK_0V6000+78)
#define     GUILIN_LITE_VBUCK_1V5875                     (GUILIN_LITE_VBUCK_0V6000+79)
#define     GUILIN_LITE_VBUCK_1V6000                     (GUILIN_LITE_VBUCK_0V6000+80)
//step 0.05v, from 1.65v to 3.95v
#define     GUILIN_LITE_VBUCK_1V65                       (GUILIN_LITE_VBUCK_0V6000+81)
#define     GUILIN_LITE_VBUCK_1V70                       (GUILIN_LITE_VBUCK_0V6000+82)
#define     GUILIN_LITE_VBUCK_1V75                       (GUILIN_LITE_VBUCK_0V6000+83)
#define     GUILIN_LITE_VBUCK_1V80                       (GUILIN_LITE_VBUCK_0V6000+84)
#define     GUILIN_LITE_VBUCK_1V85                       (GUILIN_LITE_VBUCK_0V6000+85)
#define     GUILIN_LITE_VBUCK_1V90                       (GUILIN_LITE_VBUCK_0V6000+86)
#define     GUILIN_LITE_VBUCK_1V95                       (GUILIN_LITE_VBUCK_0V6000+87)
#define     GUILIN_LITE_VBUCK_2V00                       (GUILIN_LITE_VBUCK_0V6000+88)
#define     GUILIN_LITE_VBUCK_2V05                       (GUILIN_LITE_VBUCK_0V6000+89)
#define     GUILIN_LITE_VBUCK_2V10                       (GUILIN_LITE_VBUCK_0V6000+90)
#define     GUILIN_LITE_VBUCK_2V15                       (GUILIN_LITE_VBUCK_0V6000+91)
#define     GUILIN_LITE_VBUCK_2V20                       (GUILIN_LITE_VBUCK_0V6000+92)
#define     GUILIN_LITE_VBUCK_2V25                       (GUILIN_LITE_VBUCK_0V6000+93)
#define     GUILIN_LITE_VBUCK_2V30                       (GUILIN_LITE_VBUCK_0V6000+94)
#define     GUILIN_LITE_VBUCK_2V35                       (GUILIN_LITE_VBUCK_0V6000+95)
#define     GUILIN_LITE_VBUCK_2V40                       (GUILIN_LITE_VBUCK_0V6000+96)
#define     GUILIN_LITE_VBUCK_2V45                       (GUILIN_LITE_VBUCK_0V6000+97)
#define     GUILIN_LITE_VBUCK_2V50                       (GUILIN_LITE_VBUCK_0V6000+98)
#define     GUILIN_LITE_VBUCK_2V55                       (GUILIN_LITE_VBUCK_0V6000+99)
#define     GUILIN_LITE_VBUCK_2V60                       (GUILIN_LITE_VBUCK_0V6000+100)
#define     GUILIN_LITE_VBUCK_2V65                       (GUILIN_LITE_VBUCK_0V6000+101)
#define     GUILIN_LITE_VBUCK_2V70                       (GUILIN_LITE_VBUCK_0V6000+102)
#define     GUILIN_LITE_VBUCK_2V75                       (GUILIN_LITE_VBUCK_0V6000+103)
#define     GUILIN_LITE_VBUCK_2V80                       (GUILIN_LITE_VBUCK_0V6000+104)
#define     GUILIN_LITE_VBUCK_2V85                       (GUILIN_LITE_VBUCK_0V6000+105)
#define     GUILIN_LITE_VBUCK_2V90                       (GUILIN_LITE_VBUCK_0V6000+106)
#define     GUILIN_LITE_VBUCK_2V95                       (GUILIN_LITE_VBUCK_0V6000+107)
#define     GUILIN_LITE_VBUCK_3V00                       (GUILIN_LITE_VBUCK_0V6000+108)
#define     GUILIN_LITE_VBUCK_3V05                       (GUILIN_LITE_VBUCK_0V6000+109)
#define     GUILIN_LITE_VBUCK_3V10                       (GUILIN_LITE_VBUCK_0V6000+110)
#define     GUILIN_LITE_VBUCK_3V15                       (GUILIN_LITE_VBUCK_0V6000+111)
#define     GUILIN_LITE_VBUCK_3V20                       (GUILIN_LITE_VBUCK_0V6000+112)
#define     GUILIN_LITE_VBUCK_3V25                       (GUILIN_LITE_VBUCK_0V6000+113)
#define     GUILIN_LITE_VBUCK_3V30                       (GUILIN_LITE_VBUCK_0V6000+114)
#define     GUILIN_LITE_VBUCK_3V35                       (GUILIN_LITE_VBUCK_0V6000+115)
#define     GUILIN_LITE_VBUCK_3V40                       (GUILIN_LITE_VBUCK_0V6000+116)
#define     GUILIN_LITE_VBUCK_3V45                       (GUILIN_LITE_VBUCK_0V6000+117)
#define     GUILIN_LITE_VBUCK_3V50                       (GUILIN_LITE_VBUCK_0V6000+118)
#define     GUILIN_LITE_VBUCK_3V55                       (GUILIN_LITE_VBUCK_0V6000+119)
#define     GUILIN_LITE_VBUCK_3V60                       (GUILIN_LITE_VBUCK_0V6000+120)
#define     GUILIN_LITE_VBUCK_3V65                       (GUILIN_LITE_VBUCK_0V6000+121)
#define     GUILIN_LITE_VBUCK_3V70                       (GUILIN_LITE_VBUCK_0V6000+122)
#define     GUILIN_LITE_VBUCK_3V75                       (GUILIN_LITE_VBUCK_0V6000+123)
#define     GUILIN_LITE_VBUCK_3V80                       (GUILIN_LITE_VBUCK_0V6000+124)
#define     GUILIN_LITE_VBUCK_3V85                       (GUILIN_LITE_VBUCK_0V6000+125)
#define     GUILIN_LITE_VBUCK_3V90                       (GUILIN_LITE_VBUCK_0V6000+126)
#define     GUILIN_LITE_VBUCK_3V95                       (GUILIN_LITE_VBUCK_0V6000+127)

//LDO NORMAL mode
#define     GUILIN_LITE_LDO_ACTIVE_VOUT_MASK             (0xf << 0)//BIT[3:0] for vout of Active mode

//LDO2
#define     GUILIN_LITE_LDO2_ACTIVE_1V20                (0x0)
#define     GUILIN_LITE_LDO2_ACTIVE_1V25                (0x1)
#define     GUILIN_LITE_LDO2_ACTIVE_1V60                (0x2)
#define     GUILIN_LITE_LDO2_ACTIVE_1V70                (0x3)
#define     GUILIN_LITE_LDO2_ACTIVE_1V80                (0x4)
#define     GUILIN_LITE_LDO2_ACTIVE_1V90                (0x5)
#define     GUILIN_LITE_LDO2_ACTIVE_2V00                (0x6)
#define     GUILIN_LITE_LDO2_ACTIVE_2V50                (0x7)
#define     GUILIN_LITE_LDO2_ACTIVE_2V60                (0x8)
#define     GUILIN_LITE_LDO2_ACTIVE_2V70                (0x9)
#define     GUILIN_LITE_LDO2_ACTIVE_2V80                (0xa)
#define     GUILIN_LITE_LDO2_ACTIVE_2V85                (0xb)
#define     GUILIN_LITE_LDO2_ACTIVE_2V90                (0xc)
#define     GUILIN_LITE_LDO2_ACTIVE_3V00                (0xd)
#define     GUILIN_LITE_LDO2_ACTIVE_3V10                (0xe)
#define     GUILIN_LITE_LDO2_ACTIVE_3V30                (0xf)
//LDO3
#define     GUILIN_LITE_LDO3_ACTIVE_1V20                GUILIN_LITE_LDO2_ACTIVE_1V20
#define     GUILIN_LITE_LDO3_ACTIVE_1V25                GUILIN_LITE_LDO2_ACTIVE_1V25
#define     GUILIN_LITE_LDO3_ACTIVE_1V60                GUILIN_LITE_LDO2_ACTIVE_1V60
#define     GUILIN_LITE_LDO3_ACTIVE_1V70                GUILIN_LITE_LDO2_ACTIVE_1V70
#define     GUILIN_LITE_LDO3_ACTIVE_1V80                GUILIN_LITE_LDO2_ACTIVE_1V80
#define     GUILIN_LITE_LDO3_ACTIVE_1V90                GUILIN_LITE_LDO2_ACTIVE_1V90
#define     GUILIN_LITE_LDO3_ACTIVE_2V00                GUILIN_LITE_LDO2_ACTIVE_2V00
#define     GUILIN_LITE_LDO3_ACTIVE_2V50                GUILIN_LITE_LDO2_ACTIVE_2V50
#define     GUILIN_LITE_LDO3_ACTIVE_2V60                GUILIN_LITE_LDO2_ACTIVE_2V60
#define     GUILIN_LITE_LDO3_ACTIVE_2V70                GUILIN_LITE_LDO2_ACTIVE_2V70
#define     GUILIN_LITE_LDO3_ACTIVE_2V80                GUILIN_LITE_LDO2_ACTIVE_2V80
#define     GUILIN_LITE_LDO3_ACTIVE_2V85                GUILIN_LITE_LDO2_ACTIVE_2V85
#define     GUILIN_LITE_LDO3_ACTIVE_2V90                GUILIN_LITE_LDO2_ACTIVE_2V90
#define     GUILIN_LITE_LDO3_ACTIVE_3V00                GUILIN_LITE_LDO2_ACTIVE_3V00
#define     GUILIN_LITE_LDO3_ACTIVE_3V10                GUILIN_LITE_LDO2_ACTIVE_3V10
#define     GUILIN_LITE_LDO3_ACTIVE_3V30                GUILIN_LITE_LDO2_ACTIVE_3V30
//LDO4
#define     GUILIN_LITE_LDO4_ACTIVE_1V20                GUILIN_LITE_LDO2_ACTIVE_1V20
#define     GUILIN_LITE_LDO4_ACTIVE_1V25                GUILIN_LITE_LDO2_ACTIVE_1V25
#define     GUILIN_LITE_LDO4_ACTIVE_1V60                GUILIN_LITE_LDO2_ACTIVE_1V60
#define     GUILIN_LITE_LDO4_ACTIVE_1V70                GUILIN_LITE_LDO2_ACTIVE_1V70
#define     GUILIN_LITE_LDO4_ACTIVE_1V80                GUILIN_LITE_LDO2_ACTIVE_1V80
#define     GUILIN_LITE_LDO4_ACTIVE_1V90                GUILIN_LITE_LDO2_ACTIVE_1V90
#define     GUILIN_LITE_LDO4_ACTIVE_2V00                GUILIN_LITE_LDO2_ACTIVE_2V00
#define     GUILIN_LITE_LDO4_ACTIVE_2V50                GUILIN_LITE_LDO2_ACTIVE_2V50
#define     GUILIN_LITE_LDO4_ACTIVE_2V60                GUILIN_LITE_LDO2_ACTIVE_2V60
#define     GUILIN_LITE_LDO4_ACTIVE_2V70                GUILIN_LITE_LDO2_ACTIVE_2V70
#define     GUILIN_LITE_LDO4_ACTIVE_2V80                GUILIN_LITE_LDO2_ACTIVE_2V80
#define     GUILIN_LITE_LDO4_ACTIVE_2V85                GUILIN_LITE_LDO2_ACTIVE_2V85
#define     GUILIN_LITE_LDO4_ACTIVE_2V90                GUILIN_LITE_LDO2_ACTIVE_2V90
#define     GUILIN_LITE_LDO4_ACTIVE_3V00                GUILIN_LITE_LDO2_ACTIVE_3V00
#define     GUILIN_LITE_LDO4_ACTIVE_3V10                GUILIN_LITE_LDO2_ACTIVE_3V10
#define     GUILIN_LITE_LDO4_ACTIVE_3V30                GUILIN_LITE_LDO2_ACTIVE_3V30
//LDO6
#define     GUILIN_LITE_LDO6_ACTIVE_1V20                GUILIN_LITE_LDO2_ACTIVE_1V20
#define     GUILIN_LITE_LDO6_ACTIVE_1V25                GUILIN_LITE_LDO2_ACTIVE_1V25
#define     GUILIN_LITE_LDO6_ACTIVE_1V60                GUILIN_LITE_LDO2_ACTIVE_1V60
#define     GUILIN_LITE_LDO6_ACTIVE_1V70                GUILIN_LITE_LDO2_ACTIVE_1V70
#define     GUILIN_LITE_LDO6_ACTIVE_1V80                GUILIN_LITE_LDO2_ACTIVE_1V80
#define     GUILIN_LITE_LDO6_ACTIVE_1V90                GUILIN_LITE_LDO2_ACTIVE_1V90
#define     GUILIN_LITE_LDO6_ACTIVE_2V00                GUILIN_LITE_LDO2_ACTIVE_2V00
#define     GUILIN_LITE_LDO6_ACTIVE_2V50                GUILIN_LITE_LDO2_ACTIVE_2V50
#define     GUILIN_LITE_LDO6_ACTIVE_2V60                GUILIN_LITE_LDO2_ACTIVE_2V60
#define     GUILIN_LITE_LDO6_ACTIVE_2V70                GUILIN_LITE_LDO2_ACTIVE_2V70
#define     GUILIN_LITE_LDO6_ACTIVE_2V80                GUILIN_LITE_LDO2_ACTIVE_2V80
#define     GUILIN_LITE_LDO6_ACTIVE_2V85                GUILIN_LITE_LDO2_ACTIVE_2V85
#define     GUILIN_LITE_LDO6_ACTIVE_2V90                GUILIN_LITE_LDO2_ACTIVE_2V90
#define     GUILIN_LITE_LDO6_ACTIVE_3V00                GUILIN_LITE_LDO2_ACTIVE_3V00
#define     GUILIN_LITE_LDO6_ACTIVE_3V10                GUILIN_LITE_LDO2_ACTIVE_3V10
#define     GUILIN_LITE_LDO6_ACTIVE_3V30                GUILIN_LITE_LDO2_ACTIVE_3V30
//LDO7
#define     GUILIN_LITE_LDO7_ACTIVE_1V20                GUILIN_LITE_LDO2_ACTIVE_1V20
#define     GUILIN_LITE_LDO7_ACTIVE_1V25                GUILIN_LITE_LDO2_ACTIVE_1V25
#define     GUILIN_LITE_LDO7_ACTIVE_1V60                GUILIN_LITE_LDO2_ACTIVE_1V60
#define     GUILIN_LITE_LDO7_ACTIVE_1V70                GUILIN_LITE_LDO2_ACTIVE_1V70
#define     GUILIN_LITE_LDO7_ACTIVE_1V80                GUILIN_LITE_LDO2_ACTIVE_1V80
#define     GUILIN_LITE_LDO7_ACTIVE_1V90                GUILIN_LITE_LDO2_ACTIVE_1V90
#define     GUILIN_LITE_LDO7_ACTIVE_2V00                GUILIN_LITE_LDO2_ACTIVE_2V00
#define     GUILIN_LITE_LDO7_ACTIVE_2V50                GUILIN_LITE_LDO2_ACTIVE_2V50
#define     GUILIN_LITE_LDO7_ACTIVE_2V60                GUILIN_LITE_LDO2_ACTIVE_2V60
#define     GUILIN_LITE_LDO7_ACTIVE_2V70                GUILIN_LITE_LDO2_ACTIVE_2V70
#define     GUILIN_LITE_LDO7_ACTIVE_2V80                GUILIN_LITE_LDO2_ACTIVE_2V80
#define     GUILIN_LITE_LDO7_ACTIVE_2V85                GUILIN_LITE_LDO2_ACTIVE_2V85
#define     GUILIN_LITE_LDO7_ACTIVE_2V90                GUILIN_LITE_LDO2_ACTIVE_2V90
#define     GUILIN_LITE_LDO7_ACTIVE_3V00                GUILIN_LITE_LDO2_ACTIVE_3V00
#define     GUILIN_LITE_LDO7_ACTIVE_3V10                GUILIN_LITE_LDO2_ACTIVE_3V10
#define     GUILIN_LITE_LDO7_ACTIVE_3V30                GUILIN_LITE_LDO2_ACTIVE_3V30
//LDO8
#define     GUILIN_LITE_LDO8_ACTIVE_1V20                GUILIN_LITE_LDO2_ACTIVE_1V20
#define     GUILIN_LITE_LDO8_ACTIVE_1V25                GUILIN_LITE_LDO2_ACTIVE_1V25
#define     GUILIN_LITE_LDO8_ACTIVE_1V60                GUILIN_LITE_LDO2_ACTIVE_1V60
#define     GUILIN_LITE_LDO8_ACTIVE_1V70                GUILIN_LITE_LDO2_ACTIVE_1V70
#define     GUILIN_LITE_LDO8_ACTIVE_1V80                GUILIN_LITE_LDO2_ACTIVE_1V80
#define     GUILIN_LITE_LDO8_ACTIVE_1V90                GUILIN_LITE_LDO2_ACTIVE_1V90
#define     GUILIN_LITE_LDO8_ACTIVE_2V00                GUILIN_LITE_LDO2_ACTIVE_2V00
#define     GUILIN_LITE_LDO8_ACTIVE_2V50                GUILIN_LITE_LDO2_ACTIVE_2V50
#define     GUILIN_LITE_LDO8_ACTIVE_2V60                GUILIN_LITE_LDO2_ACTIVE_2V60
#define     GUILIN_LITE_LDO8_ACTIVE_2V70                GUILIN_LITE_LDO2_ACTIVE_2V70
#define     GUILIN_LITE_LDO8_ACTIVE_2V80                GUILIN_LITE_LDO2_ACTIVE_2V80
#define     GUILIN_LITE_LDO8_ACTIVE_2V85                GUILIN_LITE_LDO2_ACTIVE_2V85
#define     GUILIN_LITE_LDO8_ACTIVE_2V90                GUILIN_LITE_LDO2_ACTIVE_2V90
#define     GUILIN_LITE_LDO8_ACTIVE_3V00                GUILIN_LITE_LDO2_ACTIVE_3V00
#define     GUILIN_LITE_LDO8_ACTIVE_3V10                GUILIN_LITE_LDO2_ACTIVE_3V10
#define     GUILIN_LITE_LDO8_ACTIVE_3V30                GUILIN_LITE_LDO2_ACTIVE_3V30

//LDO1
#define     GUILIN_LITE_LDO1_ACTIVE_1V60                (0x0)
#define     GUILIN_LITE_LDO1_ACTIVE_1V70                (0x1)
#define     GUILIN_LITE_LDO1_ACTIVE_1V80                (0x2)
#define     GUILIN_LITE_LDO1_ACTIVE_1V90                (0x3)
#define     GUILIN_LITE_LDO1_ACTIVE_2V00                (0x4)
#define     GUILIN_LITE_LDO1_ACTIVE_2V70                (0x5)
#define     GUILIN_LITE_LDO1_ACTIVE_2V80                (0x6)
#define     GUILIN_LITE_LDO1_ACTIVE_2V90                (0x7)
//LDO5
#define     GUILIN_LITE_LDO5_ACTIVE_1V70               GUILIN_LITE_LDO1_ACTIVE_1V60
#define     GUILIN_LITE_LDO5_ACTIVE_1V80               GUILIN_LITE_LDO1_ACTIVE_1V70
#define     GUILIN_LITE_LDO5_ACTIVE_1V90               GUILIN_LITE_LDO1_ACTIVE_1V80
#define     GUILIN_LITE_LDO5_ACTIVE_2V50               GUILIN_LITE_LDO1_ACTIVE_1V90
#define     GUILIN_LITE_LDO5_ACTIVE_2V80               GUILIN_LITE_LDO1_ACTIVE_2V00
#define     GUILIN_LITE_LDO5_ACTIVE_2V90               GUILIN_LITE_LDO1_ACTIVE_2V70
#define     GUILIN_LITE_LDO5_ACTIVE_3V10               GUILIN_LITE_LDO1_ACTIVE_2V80
#define     GUILIN_LITE_LDO5_ACTIVE_3V30               GUILIN_LITE_LDO1_ACTIVE_2V90

//SLEEP mode
#define     GUILIN_LITE_LDO_SLEEP_VOUT_MASK             (0xf)	//BIT[3:0] ldo vout for sleep mode

//LDO2
#define     GUILIN_LITE_LDO2_SLEEP_1V20                (0x0)
#define     GUILIN_LITE_LDO2_SLEEP_1V25                (0x1)
#define     GUILIN_LITE_LDO2_SLEEP_1V60                (0x2)
#define     GUILIN_LITE_LDO2_SLEEP_1V70                (0x3)
#define     GUILIN_LITE_LDO2_SLEEP_1V80                (0x4)
#define     GUILIN_LITE_LDO2_SLEEP_1V90                (0x5)
#define     GUILIN_LITE_LDO2_SLEEP_2V00                (0x6)
#define     GUILIN_LITE_LDO2_SLEEP_2V50                (0x7)
#define     GUILIN_LITE_LDO2_SLEEP_2V60                (0x8)
#define     GUILIN_LITE_LDO2_SLEEP_2V70                (0x9)
#define     GUILIN_LITE_LDO2_SLEEP_2V80                (0xa)
#define     GUILIN_LITE_LDO2_SLEEP_2V85                (0xb)
#define     GUILIN_LITE_LDO2_SLEEP_2V90                (0xc)
#define     GUILIN_LITE_LDO2_SLEEP_3V00                (0xd)
#define     GUILIN_LITE_LDO2_SLEEP_3V10                (0xe)
#define     GUILIN_LITE_LDO2_SLEEP_3V30                (0xf)
//LDO3
#define     GUILIN_LITE_LDO3_SLEEP_1V20                GUILIN_LITE_LDO2_SLEEP_1V20
#define     GUILIN_LITE_LDO3_SLEEP_1V25                GUILIN_LITE_LDO2_SLEEP_1V25
#define     GUILIN_LITE_LDO3_SLEEP_1V60                GUILIN_LITE_LDO2_SLEEP_1V60
#define     GUILIN_LITE_LDO3_SLEEP_1V70                GUILIN_LITE_LDO2_SLEEP_1V70
#define     GUILIN_LITE_LDO3_SLEEP_1V80                GUILIN_LITE_LDO2_SLEEP_1V80
#define     GUILIN_LITE_LDO3_SLEEP_1V90                GUILIN_LITE_LDO2_SLEEP_1V90
#define     GUILIN_LITE_LDO3_SLEEP_2V00                GUILIN_LITE_LDO2_SLEEP_2V00
#define     GUILIN_LITE_LDO3_SLEEP_2V50                GUILIN_LITE_LDO2_SLEEP_2V50
#define     GUILIN_LITE_LDO3_SLEEP_2V60                GUILIN_LITE_LDO2_SLEEP_2V60
#define     GUILIN_LITE_LDO3_SLEEP_2V70                GUILIN_LITE_LDO2_SLEEP_2V70
#define     GUILIN_LITE_LDO3_SLEEP_2V80                GUILIN_LITE_LDO2_SLEEP_2V80
#define     GUILIN_LITE_LDO3_SLEEP_2V85                GUILIN_LITE_LDO2_SLEEP_2V85
#define     GUILIN_LITE_LDO3_SLEEP_2V90                GUILIN_LITE_LDO2_SLEEP_2V90
#define     GUILIN_LITE_LDO3_SLEEP_3V00                GUILIN_LITE_LDO2_SLEEP_3V00
#define     GUILIN_LITE_LDO3_SLEEP_3V10                GUILIN_LITE_LDO2_SLEEP_3V10
#define     GUILIN_LITE_LDO3_SLEEP_3V30                GUILIN_LITE_LDO2_SLEEP_3V30
//LDO4
#define     GUILIN_LITE_LDO4_SLEEP_1V20                GUILIN_LITE_LDO2_SLEEP_1V20
#define     GUILIN_LITE_LDO4_SLEEP_1V25                GUILIN_LITE_LDO2_SLEEP_1V25
#define     GUILIN_LITE_LDO4_SLEEP_1V60                GUILIN_LITE_LDO2_SLEEP_1V60
#define     GUILIN_LITE_LDO4_SLEEP_1V70                GUILIN_LITE_LDO2_SLEEP_1V70
#define     GUILIN_LITE_LDO4_SLEEP_1V80                GUILIN_LITE_LDO2_SLEEP_1V80
#define     GUILIN_LITE_LDO4_SLEEP_1V90                GUILIN_LITE_LDO2_SLEEP_1V90
#define     GUILIN_LITE_LDO4_SLEEP_2V00                GUILIN_LITE_LDO2_SLEEP_2V00
#define     GUILIN_LITE_LDO4_SLEEP_2V50                GUILIN_LITE_LDO2_SLEEP_2V50
#define     GUILIN_LITE_LDO4_SLEEP_2V60                GUILIN_LITE_LDO2_SLEEP_2V60
#define     GUILIN_LITE_LDO4_SLEEP_2V70                GUILIN_LITE_LDO2_SLEEP_2V70
#define     GUILIN_LITE_LDO4_SLEEP_2V80                GUILIN_LITE_LDO2_SLEEP_2V80
#define     GUILIN_LITE_LDO4_SLEEP_2V85                GUILIN_LITE_LDO2_SLEEP_2V85
#define     GUILIN_LITE_LDO4_SLEEP_2V90                GUILIN_LITE_LDO2_SLEEP_2V90
#define     GUILIN_LITE_LDO4_SLEEP_3V00                GUILIN_LITE_LDO2_SLEEP_3V00
#define     GUILIN_LITE_LDO4_SLEEP_3V10                GUILIN_LITE_LDO2_SLEEP_3V10
#define     GUILIN_LITE_LDO4_SLEEP_3V30                GUILIN_LITE_LDO2_SLEEP_3V30
//LDO6
#define     GUILIN_LITE_LDO6_SLEEP_1V20                GUILIN_LITE_LDO2_SLEEP_1V20
#define     GUILIN_LITE_LDO6_SLEEP_1V25                GUILIN_LITE_LDO2_SLEEP_1V25
#define     GUILIN_LITE_LDO6_SLEEP_1V60                GUILIN_LITE_LDO2_SLEEP_1V60
#define     GUILIN_LITE_LDO6_SLEEP_1V70                GUILIN_LITE_LDO2_SLEEP_1V70
#define     GUILIN_LITE_LDO6_SLEEP_1V80                GUILIN_LITE_LDO2_SLEEP_1V80
#define     GUILIN_LITE_LDO6_SLEEP_1V90                GUILIN_LITE_LDO2_SLEEP_1V90
#define     GUILIN_LITE_LDO6_SLEEP_2V00                GUILIN_LITE_LDO2_SLEEP_2V00
#define     GUILIN_LITE_LDO6_SLEEP_2V50                GUILIN_LITE_LDO2_SLEEP_2V50
#define     GUILIN_LITE_LDO6_SLEEP_2V60                GUILIN_LITE_LDO2_SLEEP_2V60
#define     GUILIN_LITE_LDO6_SLEEP_2V70                GUILIN_LITE_LDO2_SLEEP_2V70
#define     GUILIN_LITE_LDO6_SLEEP_2V80                GUILIN_LITE_LDO2_SLEEP_2V80
#define     GUILIN_LITE_LDO6_SLEEP_2V85                GUILIN_LITE_LDO2_SLEEP_2V85
#define     GUILIN_LITE_LDO6_SLEEP_2V90                GUILIN_LITE_LDO2_SLEEP_2V90
#define     GUILIN_LITE_LDO6_SLEEP_3V00                GUILIN_LITE_LDO2_SLEEP_3V00
#define     GUILIN_LITE_LDO6_SLEEP_3V10                GUILIN_LITE_LDO2_SLEEP_3V10
#define     GUILIN_LITE_LDO6_SLEEP_3V30                GUILIN_LITE_LDO2_SLEEP_3V30
//LDO7
#define     GUILIN_LITE_LDO7_SLEEP_1V20                GUILIN_LITE_LDO2_SLEEP_1V20
#define     GUILIN_LITE_LDO7_SLEEP_1V25                GUILIN_LITE_LDO2_SLEEP_1V25
#define     GUILIN_LITE_LDO7_SLEEP_1V60                GUILIN_LITE_LDO2_SLEEP_1V60
#define     GUILIN_LITE_LDO7_SLEEP_1V70                GUILIN_LITE_LDO2_SLEEP_1V70
#define     GUILIN_LITE_LDO7_SLEEP_1V80                GUILIN_LITE_LDO2_SLEEP_1V80
#define     GUILIN_LITE_LDO7_SLEEP_1V90                GUILIN_LITE_LDO2_SLEEP_1V90
#define     GUILIN_LITE_LDO7_SLEEP_2V00                GUILIN_LITE_LDO2_SLEEP_2V00
#define     GUILIN_LITE_LDO7_SLEEP_2V50                GUILIN_LITE_LDO2_SLEEP_2V50
#define     GUILIN_LITE_LDO7_SLEEP_2V60                GUILIN_LITE_LDO2_SLEEP_2V60
#define     GUILIN_LITE_LDO7_SLEEP_2V70                GUILIN_LITE_LDO2_SLEEP_2V70
#define     GUILIN_LITE_LDO7_SLEEP_2V80                GUILIN_LITE_LDO2_SLEEP_2V80
#define     GUILIN_LITE_LDO7_SLEEP_2V85                GUILIN_LITE_LDO2_SLEEP_2V85
#define     GUILIN_LITE_LDO7_SLEEP_2V90                GUILIN_LITE_LDO2_SLEEP_2V90
#define     GUILIN_LITE_LDO7_SLEEP_3V00                GUILIN_LITE_LDO2_SLEEP_3V00
#define     GUILIN_LITE_LDO7_SLEEP_3V10                GUILIN_LITE_LDO2_SLEEP_3V10
#define     GUILIN_LITE_LDO7_SLEEP_3V30                GUILIN_LITE_LDO2_SLEEP_3V30
//LDO8
#define     GUILIN_LITE_LDO8_SLEEP_1V20                GUILIN_LITE_LDO2_SLEEP_1V20
#define     GUILIN_LITE_LDO8_SLEEP_1V25                GUILIN_LITE_LDO2_SLEEP_1V25
#define     GUILIN_LITE_LDO8_SLEEP_1V60                GUILIN_LITE_LDO2_SLEEP_1V60
#define     GUILIN_LITE_LDO8_SLEEP_1V70                GUILIN_LITE_LDO2_SLEEP_1V70
#define     GUILIN_LITE_LDO8_SLEEP_1V80                GUILIN_LITE_LDO2_SLEEP_1V80
#define     GUILIN_LITE_LDO8_SLEEP_1V90                GUILIN_LITE_LDO2_SLEEP_1V90
#define     GUILIN_LITE_LDO8_SLEEP_2V00                GUILIN_LITE_LDO2_SLEEP_2V00
#define     GUILIN_LITE_LDO8_SLEEP_2V50                GUILIN_LITE_LDO2_SLEEP_2V50
#define     GUILIN_LITE_LDO8_SLEEP_2V60                GUILIN_LITE_LDO2_SLEEP_2V60
#define     GUILIN_LITE_LDO8_SLEEP_2V70                GUILIN_LITE_LDO2_SLEEP_2V70
#define     GUILIN_LITE_LDO8_SLEEP_2V80                GUILIN_LITE_LDO2_SLEEP_2V80
#define     GUILIN_LITE_LDO8_SLEEP_2V85                GUILIN_LITE_LDO2_SLEEP_2V85
#define     GUILIN_LITE_LDO8_SLEEP_2V90                GUILIN_LITE_LDO2_SLEEP_2V90
#define     GUILIN_LITE_LDO8_SLEEP_3V00                GUILIN_LITE_LDO2_SLEEP_3V00
#define     GUILIN_LITE_LDO8_SLEEP_3V10                GUILIN_LITE_LDO2_SLEEP_3V10
#define     GUILIN_LITE_LDO8_SLEEP_3V30                GUILIN_LITE_LDO2_SLEEP_3V30

//LDO1
#define     GUILIN_LITE_LDO1_SLEEP_1V60               (0x00)
#define     GUILIN_LITE_LDO1_SLEEP_1V70               (0x01)
#define     GUILIN_LITE_LDO1_SLEEP_1V80               (0x02)
#define     GUILIN_LITE_LDO1_SLEEP_1V90               (0x03)
#define     GUILIN_LITE_LDO1_SLEEP_2V00               (0x04)
#define     GUILIN_LITE_LDO1_SLEEP_2V70               (0x05)
#define     GUILIN_LITE_LDO1_SLEEP_2V80               (0x06)
#define     GUILIN_LITE_LDO1_SLEEP_2V90               (0x07)
//LDO5
#define     GUILIN_LITE_LDO5_SLEEP_1V60               GUILIN_LITE_LDO1_SLEEP_1V60
#define     GUILIN_LITE_LDO5_SLEEP_1V70               GUILIN_LITE_LDO1_SLEEP_1V70
#define     GUILIN_LITE_LDO5_SLEEP_1V80               GUILIN_LITE_LDO1_SLEEP_1V80
#define     GUILIN_LITE_LDO5_SLEEP_1V90               GUILIN_LITE_LDO1_SLEEP_1V90
#define     GUILIN_LITE_LDO5_SLEEP_2V00               GUILIN_LITE_LDO1_SLEEP_2V00
#define     GUILIN_LITE_LDO5_SLEEP_2V70               GUILIN_LITE_LDO1_SLEEP_2V70
#define     GUILIN_LITE_LDO5_SLEEP_2V80               GUILIN_LITE_LDO1_SLEEP_2V80
#define     GUILIN_LITE_LDO5_SLEEP_2V90               GUILIN_LITE_LDO1_SLEEP_2V90


#define     GUILIN_LITE_VBUCK1_ACTIVE_VOUT_REG      (0x20)
#define     GUILIN_LITE_VBUCK1_SLEEP_VOUT_REG		(0x21)

#define     GUILIN_LITE_VBUCK1_FSM_REG1			    (0x22)
#define     GUILIN_LITE_VBUCK1_FSM_REG2			    (0x23)
#define     GUILIN_LITE_VBUCK1_FSM_REG3			    (0x24)
#define     GUILIN_LITE_VBUCK1_FSM_REG4			    (0x25)
#define     GUILIN_LITE_VBUCK1_FSM_REG5			    (0x26)
#define     GUILIN_LITE_VBUCK1_FSM_REG6			    (0x27)
#define     GUILIN_LITE_VBUCK1_FSM_REG7			    (0x28)
#define     GUILIN_LITE_VBUCK1_FSM_REG8			    (0x29)
#define     GUILIN_LITE_VBUCK1_FSM_REG9			    (0x2A)
#define	    GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET0	(0x2B)
#define	    GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET1	(0x2C)
#define	    GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET2	(0x2D)
#define	    GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET3	(0x2E)
                                            
#define	    GUILIN_LITE_VBUCK1_DVC_ENABLE_REG		GUILIN_LITE_VBUCK1_SLEEP_VOUT_REG
#define	    GUILIN_LITE_VBUCK1_ENABLE_REG		    GUILIN_LITE_VBUCK1_ACTIVE_VOUT_REG
#define     GUILIN_LITE_VBUCK1_SLEEP_MODE_REG		GUILIN_LITE_VBUCK1_FSM_REG1


#define		GUILIN_LITE_CONTAIN_VBUCK_DVC_VOUT_MASK		(0x7f)


#define		GUILIN_LITE_CONTAIN_VBUCK_EN_BIT(x)		(x==GUILIN_LITE_VBUCK1_ENABLE_REG)

#define		GUILIN_LITE_CONTAIN_VBUCK_ACTIVE_VOUT_BIT(x)		GUILIN_LITE_CONTAIN_VBUCK_EN_BIT(x)

#define		GUILIN_LITE_CONTAIN_VBUCK_SLEEP_VOUT_BIT(x)	(x==GUILIN_LITE_VBUCK1_SLEEP_VOUT_REG)

#define		GUILIN_LITE_CONTAIN_VBUCK_DVC_VOUT_BIT(x)	(x==GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET0|| \
												x==GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET1|| \
												x==GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET2|| \
												x==GUILIN_LITE_VBUCK1_DVC_VOUT_REG_SET3)

#define		GUILIN_LITE_CONTAIN_VBUCK_SLEEP_MODE_BIT(x) 	(x==GUILIN_LITE_VBUCK1_SLEEP_MODE_REG)

#define		GUILIN_LITE_CONTAIN_VBUCK_DVC_EN_BIT(x) 		(x==GUILIN_LITE_VBUCK1_DVC_ENABLE_REG)


#define		GUILIN_LITE_VBUCK_ENABLE_MASK			(0x1<<7)
#define		GUILIN_LITE_VBUCK_ENABLE_DVC_MASK		(0x1<<7)
#define		GUILIN_LITE_VBUCK_SLEEP_MODE_MASK		(0x3<<3)

//00	:	BUCK OFF
//01	:	BUCK ACTIVE mode
//10	:	BUCK SLEEP mode
//11	:	BUCK NORMAL mode
#define		GUILIN_LITE_VBUCK_OFF			(0x0 <<3)
#define		GUILIN_LITE_VBUCK_ACTIVE_SLEEP	(0x1 <<3)
#define		GUILIN_LITE_VBUCK_SLEEP			(0x2 <<3)
#define		GUILIN_LITE_VBUCK_ACTIVE		(0x3 <<3)

#define		GUILIN_LITE_CONTAIN_LDO_EN_BIT(x) 		(x==GUILIN_LITE_LDO1_ENABLE_REG|| \
											x==GUILIN_LITE_LDO2_ENABLE_REG|| \
											x==GUILIN_LITE_LDO3_ENABLE_REG|| \
											x==GUILIN_LITE_LDO4_ENABLE_REG|| \
											x==GUILIN_LITE_LDO5_ENABLE_REG|| \
											x==GUILIN_LITE_LDO6_ENABLE_REG|| \
											x==GUILIN_LITE_LDO7_ENABLE_REG|| \
											x==GUILIN_LITE_LDO8_ENABLE_REG)

#define		GUILIN_LITE_CONTAIN_LDO_ACTIVE_VOUT_BIT(x)	GUILIN_LITE_CONTAIN_LDO_EN_BIT(x)


#define		GUILIN_LITE_CONTAIN_LDO_SLEEP_MODE_BIT(x) 		(x==GUILIN_LITE_LDO1_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO2_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO3_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO4_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO5_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO6_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO7_SLEEP_MODE_REG|| \
												x==GUILIN_LITE_LDO8_SLEEP_MODE_REG)
#define		GUILIN_LITE_CONTAIN_LDO_SLEEP_VOUT_BIT(x)	GUILIN_LITE_CONTAIN_LDO_SLEEP_MODE_BIT(x)

#define		GUILIN_LITE_LDO_ENABLE_MASK				(0x1<<7)
#define		GUILIN_LITE_LDO_SLEEP_MODE_MASK			(0x3<<4)

//00	:	LDO OFF
//01	:	Reserve
//10	:	LDO SLEEP mode
//11	:	LDO NORMAL mode
#define     GUILIN_LITE_LDO_OFF                      (0x0 <<4)
#define     GUILIN_LITE_LDO_SLEEP                    (0x2 <<4)
#define     GUILIN_LITE_LDO_ACTIVE                   (0x3 <<4)


//BASE_PAGE_ADDR=0xE2 , BIT[3:0]
#define		GUILIN_LITE_RESET_DISCHARGE_REG			 (0xe2)
#define		GUILIN_LITE_RESET_DISCHARGE_MASK		 (0xf)

#define		GUILIN_LITE_FAULT_WU_REG				  (0xe7)
#define		GUILIN_LITE_FAULT_WU_BIT				  (0x1<<1)
#define		GUILIN_LITE_FAULT_WU_ENABLE_BIT			  (0x1<<0)

#define		GUILIN_LITE_RESET_REG				    (0x0d)
#define		GUILIN_LITE_SW_PDOWN_BIT				(0x1<<5)
#define		GUILIN_LITE_RESET_PMIC_BIT			    (0x1<<6)

#define		GUILIN_LITE_PWR_HOLD_REG				GUILIN_LITE_RESET_REG
#define		GUILIN_LITE_PWR_HOLD_BIT				(0x1<<7)

#define 	GUILIN_LITE_BUCK_ENABLE                   (0x1<<7)
#define 	GUILIN_LITE_LDO_ENABLE                    (0x1<<7)

/* GuilinLite I2c address*/
#define GUILIN_LITE_BASE_SLAVE_WRITE_ADDR		     0x60
#define GUILIN_LITE_BASE_SLAVE_READ_ADDR	         0x61

#define GUILIN_LITE_POWER_SLAVE_WRITE_ADDR		     0x62
#define GUILIN_LITE_POWER_SLAVE_READ_ADDR	         0x63

typedef enum
{
    GUILIN_LITE_BASE_Reg,
    GUILIN_LITE_POWER_Reg,
}GuilinLite_Reg_Type;

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinLiteRead                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Read GuilinLite by PI2C interface.                      */
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
int GuilinLiteRead( GuilinLite_Reg_Type guilin_lite_reg_type, unsigned char reg, unsigned char *value );


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinLiteWite                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Write GuilinLite by PI2C interface.                     */
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
int GuilinLiteWrite( GuilinLite_Reg_Type guilin_lite_reg_type, unsigned char reg, unsigned char value );

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GuilinLiteClkInit                                                    */
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
void GuilinLiteClkInit( void );

int GuilinLite_VBUCK_Set_Enable(unsigned char reg, unsigned char enable);
int GuilinLite_VBUCK_Set_DVC_Enable(unsigned char reg, unsigned char enable);
int GuilinLite_VBUCK_Set_Slpmode(unsigned char reg, unsigned char mode);
int GuilinLite_VBUCK_Set_VOUT(unsigned char reg, unsigned char value);
int GuilinLite_LDO_Set_Enable(unsigned char reg, unsigned char enable);
int GuilinLite_LDO_Set_Slpmode(unsigned char reg, unsigned char mode);
int GuilinLite_LDO_Set_VOUT(unsigned char reg, unsigned char value);
void GuilinLite_Ldo_6_set_2_8(void);
void GuilinLite_Ldo_6_set(BOOL OnOff);
void GuilinLite_Ldo_3_set_1_8(void);
void GuilinLite_Ldo_3_set_3_0(void);
void GuilinLite_Ldo_8_set_1_8(void);
void GuilinLite_Ldo_8_set_3_0(void);
void GuilinLite_Ldo_3_set(BOOL OnOff);
void GuilinLite_Ldo_8_set(BOOL OnOff);
#endif /* _GUILIN_LITE_H_        */
