/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef __PM_H__
#define __PM_H__
#include "plat_basic_api.h"
#include "charger_api.h"
#include "ui_log_api.h"

#include "ui_os_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PM_BATTERY_STATE_NORMAL      0
#define PM_BATTERY_STATE_LOW         1
#define PM_BATTERY_STATE_CRITICAL    2
#define PM_BATTERY_STATE_SHUTDOWN    3
#define PM_BATTERY_STATE_UNKNOWN     4

#define PM_CHARGER_DISCONNECTED      0
#define PM_CHARGER_CONNECTED         1
#define PM_CHARGER_CHRGING           2
#define PM_CHARGER_FINISHED          3
#define PM_CHARGER_ERROR_TEMPERATURE 4
#define PM_CHARGER_ERROR_VOLTAGE     5
#define PM_CHARGER_ERROR_UNKNOWN     9

BOOL PM_GetACPowerStatus (unsigned char* pACStatus);

//get ADC information,Gp_adc_In0 has been ocupied by battery voltage
typedef struct _PM_GPADC_INFO {
 unsigned short Gpadc_In1;
 unsigned short Gpadc_In2;
 unsigned short Gpadc_In3;
 unsigned char pading[2];
} PM_GPADC_INFO;

typedef struct _PM_BATTERY_INFO {
  unsigned char  nBatteryFlag;
  unsigned char  nBatteryLifePercent;
  unsigned short nBatteryRealTimeVoltage;
  unsigned short nBatteryVoltage;
  unsigned short nBatteryCurrent;
  unsigned short nBatteryTemperature;
  unsigned char  padding[2];
  unsigned int nBatteryLifeTime;
  unsigned int nBatteryFullLifeTime;
} PM_BATTERY_INFO;

BOOL PM_GetBatteryState(PM_BATTERY_INFO* pBatInfo);

BOOL PM_GetBatteryInfo (unsigned char* pBcs, unsigned char* pBcl, unsigned short* pMpc);

#define PM_BATTERY_CHEMISTRY_ALKALINE   0
#define PM_BATTERY_CHEMISTRY_NICD       1
#define PM_BATTERY_CHEMISTRY_HIMH       2
#define PM_BATTERY_CHEMISTRY_LION       3
#define PM_BATTERY_CHEMISTRY_LIPOLY     4
#define PM_BATTERY_CHEMISTRY_UNKNOWN    5

//BOOL PM_BatteryChemistry (unsigned char* pChemistryStatus);

#define PM_BATTERY_IND_ENABLE    0
#define PM_BATTERY_IND_DISABLE   1

VOID PM_StartEarPieceGpadcMonitor(BOOL start);
BOOL PM_GetEarPieceGpadcMonitorStatus(VOID);
VOID PM_DisableEarPieceGpadcMonitor(BOOL disable);

#define PM_BATTER_FLAG_GENERAL     0
#define PM_BATTER_FLAG_BACKUP      1

#define PM_SLEEP_MODE              0x01
#define PM_FULL_MODE               0x02
#define PM_DISABLE_T_RF_MODE       0x04
#define PM_DISABLE_R_RF_MODE       0x08
#define PM_DISABLE_TR_RF_MODE      0x10
#define PM_RST_FULL_MODE           0x20
#define PM_WATCHDOG_MODE           0x40

BOOL PM_SetPhoneFunctionality(unsigned short nMode, unsigned short nValue);

BOOL PM_GetPhoneFunctionality(unsigned short* pMode);

typedef enum
{
	PM_GPADC_CHAN_0,
	PM_GPADC_CHAN_1,
	PM_GPADC_CHAN_2,
	PM_GPADC_CHAN_3,
    PM_GPADC_CHAN_QTY
} PM_GPADC_CHAN_T;

// just for the factory mode now
// note: the battery volt is double of the return value.

unsigned short PM_GetGpadcValue(PM_GPADC_CHAN_T channel);
BOOL IsBatteryInCharge(void);

#ifdef __cplusplus
}
#endif

#endif // __PM_H__

