#ifndef _CHARGER_API_H_
#define _CHARGER_API_H_

#include "plat_types.h"
#include "pmic_api.h"
#ifdef __cplusplus
extern "C" {
#endif
// ============================================================================
//  MACROS
// ============================================================================
#define For_Battery_M20H 1
//#define For_Watch_Battery  1
//#define For_Watch_Battery_K_632832H 1
//#define For_Watch_Battery_Y_653028V  1
#if For_Watch_Battery_K_632832H
#define Watch_K_632832H_battery_id_vol 200  //need change this value by battery id
#endif

extern BOOL IschargerDebugEn(void);

#ifdef For_Watch_Battery
#define PMIC_TRACE(...) do{\
        if(IschargerDebugEn()) \
            fatal_printf(__VA_ARGS__); \
        else \
            raw_uart_log(__VA_ARGS__); \
    }while(0)
#else
#define PMIC_TRACE raw_uart_log
#endif

#ifdef For_Watch_Battery
#define PMIC_DEBUG_TRACE(...) do{\
        if(IschargerDebugEn()) \
            fatal_printf(__VA_ARGS__); \
        else \
            raw_uart_log(__VA_ARGS__); \
    }while(0)
#else
#define PMIC_DEBUG_TRACE raw_uart_log
#endif

#define PMIC_BAD_VALUE  0xFFFF

typedef enum
{
    /// Starting state before the charger detection has occured once
    PMIC_CHARGER_UNKNOWN,            // 0
    /// No Charger is plugged
    PMIC_CHARGER_UNPLUGGED,          // 1
    /// Charger is plugged, but charge is not running
    PMIC_CHARGER_PLUGGED,            // 2
    /// Charger is plugged, charge in precharge phase (whe souldn't be on anyway)
    PMIC_CHARGER_PRECHARGE,          // 3
    /// Charger is plugged, charge in fast mode (constant current)
    PMIC_CHARGER_FAST_CHARGE,        // 4
    /// Charger is plugged, charge in pulsed mode (constant voltage, active)
    PMIC_CHARGER_PULSED_CHARGE,      // 5
    /// Charger is plugged, charge in pulsed mode (constant voltage, inactive)
    PMIC_CHARGER_PULSED_CHARGE_WAIT, // 6
    /// Charger is plugged, charge done.
    PMIC_CHARGER_FULL_CHARGE,        // 7
    /// Charger is plugged, charge stopped for safety (error).
    PMIC_CHARGER_ERROR,              // 8
    /// Charger is plugged, charge stopped for safety (timeout).
    PMIC_CHARGER_TIMED_OUT,          // 9
    /// Charger is plugged, charge stopped for safety (temperature).
    PMIC_CHARGER_TEMPERATURE_ERROR,  // 10
    /// Charger is plugged, charge stopped for safety (voltage).
    PMIC_CHARGER_VOLTAGE_ERROR,      // 11

    PMIC_CHARGER_STATUS_QTY
} PMIC_CHARGER_STATUS_T;


typedef enum
{
    /// Charge with a  100 mA current
    PMIC_CHARGER_100MA,
    /// Charge with a 200 mA current
    PMIC_CHARGER_200MA,
    /// Charge with a 300 mA current
    PMIC_CHARGER_300MA,
    /// Charge with a 400 mA current
    PMIC_CHARGER_400MA,
    /// Charge with a 450 mA current
    PMIC_CHARGER_450MA,
    /// Charge with a 500 mA current
    PMIC_CHARGER_500MA,
    /// Charge with a 550 mA current
    PMIC_CHARGER_550MA,
    /// Charge with a 600 mA current
    PMIC_CHARGER_600MA,
    /// Charge with a 650 mA current
    PMIC_CHARGER_650MA,
    /// Charge with a 700 mA current
    PMIC_CHARGER_700MA,
    /// Charge with a 750 mA current
    PMIC_CHARGER_750MA,
    /// Charge with a 800 mA current
    PMIC_CHARGER_800MA,
    /// Charge with a 850 mA current
    PMIC_CHARGER_850MA,
    /// Charge with a 900 mA current
    PMIC_CHARGER_900MA,
    /// Charge with a 950 mA current
    PMIC_CHARGER_950MA,
    /// Charge with a 1000 mA current
    PMIC_CHARGER_1000MA,
} PMIC_CHARGE_CURRENT_T;

typedef enum
{
    /// Charge with a  75 mA current
    PRE_CHARGER_75MA,
    /// Charge with a 50 mA current
    PRE_CHARGER_50MA,
    /// Charge with a 100 mA current
    PRE_CHARGER_100MA,
    /// Charge with a 150 mA current
    PRE_CHARGER_150MA,
} PRECHARGE_CURRENT_T;

typedef enum
{
    /// Charge with a  50 mA current
    TER_CHARGER_50MA,
    /// Charge with a 25 mA current
    TER_CHARGER_25MA,
    /// Charge with a 75 mA current
    TER_CHARGER_75MA,
    /// Charge with a 100 mA current
    TER_CHARGER_100MA,
} TERMINATION_CURRENT_T;

typedef enum
{
    BAT_VOLTAGE_4200MV,
    BAT_VOLTAGE_4350MV,
    BAT_VOLTAGE_4150MV,
    BAT_VOLTAGE_4100MV,
} BATTERY_VOLTAGE_T;

typedef enum
{
    TRICKIE_CHARGER_TOUT_6M,
    TRICKIE_CHARGER_TOUT_9M,
    TRICKIE_CHARGER_TOUT_12M,
    TRICKIE_CHARGER_TOUT_15M,
} TRICKIE_CHARGER_T;

typedef enum
{
    PRE_CHARGER_TOUT_24M,
    PRE_CHARGER_TOUT_26M,
    PRE_CHARGER_TOUT_48M,
    PRE_CHARGER_TOUT_60M,
} PRE_CHARGER_T;

typedef enum
{
    CCCV_TOUT_96M,
    CCCV_TOUT_144M,
    CCCV_TOUT_192M,
    CCCV_TOUT_240M,
} CCCV_CHARGER_T;

struct PMIC_BATTERY_LEVEL_CFG_T
{
    UINT8 battery_warning_threshold;
    UINT8 battery_level0_threshold;
    UINT8 battery_level1_threshold;
    UINT8 battery_level2_threshold;
    UINT8 battery_level3_threshold;
    UINT8 battery_level4_threshold;
};
typedef struct PMIC_BATTERY_LEVEL_CFG_T  PMIC_BATTERY_LEVEL_CFG;

// =============================================================================
struct PMIC_CONFIG_STRUCT_T
{
    /// usually 4200 (4.2V) for LiIon batteries.
    UINT16 batteryLevelFullMV;
    UINT16 batteryLevelPrechargeMV; 
//    UINT32 batteryChargeTimeout;
    UINT16 battery_termination_cur_mA;
    /// the power-on battery level in milivolts
    UINT16 powerOnVoltageMV;
    /// the power-down battery level in milivolts
    UINT16 powerDownVoltageMV;
    /// the battery charge current
    PMIC_CHARGE_CURRENT_T batteryChargeCurrent;
    UINT32 mppt_CurrentUpperLimit;
    PRECHARGE_CURRENT_T pre_charge_Current;
    TERMINATION_CURRENT_T termination_charger_Current;
    VBAT_VOLTAGE battery_cv_Voltage;
    TRICKIE_CHARGER_T trickle_ChargeTimeout;
    PRE_CHARGER_T pre_Charge_Timeout;
    CCCV_CHARGER_T cccv_Charge_Timeout;
    UINT16 batteryIR;
    UINT16 recharger_vol_mV;
    BOOL charger_rise_up_vol;
	BOOL Has_battery_discharger_table;
	unsigned short BatteryDischarger_table[101];
	BOOL Has_battery_temp;
	UINT32 battery_temp[29];
};

typedef struct PMIC_CONFIG_STRUCT_T PMIC_CONFIG_T;
extern PMIC_BATTERY_LEVEL_CFG* g_bat_level_cfg;

PUBLIC UINT16 pmic_GetBatteryLevel(UINT8 * pPercent);
PUBLIC UINT8 pmic_BatteryLevelToPercent(UINT16 batteryLevel);
PUBLIC PMIC_CHARGER_STATUS_T pmic_GetChargerStatus(VOID);
PUBLIC UINT8 pmic_GetPowerOnVolt(VOID);
PUBLIC UINT16 pmic_GetBatteryTemperature(VOID);
PUBLIC UINT8 pmic_GetPowerDownVolt(VOID);
typedef void (*ChgIntCallback)(void);
PUBLIC VOID pmic_SetChargerStatusHandler(ChgIntCallback handler);
PUBLIC VOID pmic_SetChargeCurrent(PMIC_CHARGE_CURRENT_T current);
PUBLIC UINT32 pmic_GetBatteryGpadcChannel(VOID);
PUBLIC UINT16 pmic_GetGpadcBatteryLevel(VOID);
PUBLIC UINT32 pmic_GetEarpieceGpadcChannel(VOID);
PUBLIC UINT32 pmic_GetChargerGpadcChannel(VOID);
PUBLIC BOOL pmic_ChargerChrVoltMonEnabled(VOID);
PUBLIC UINT16 pmic_GetGpadcBatteryLevel(VOID);
PUBLIC VOID pmic_InitCharger(VOID);

typedef int (*Usb_PlugCallback)(unsigned int action);
typedef unsigned int (*Sim_CallCallback)(unsigned int SimId);
typedef int (*Battery_Callback)(unsigned char bcs,unsigned char bcl,unsigned short batLevel);
BOOL ChargerCallbackRegister(Usb_PlugCallback Usb_charger_status,Sim_CallCallback get_call_status,Battery_Callback battery_status);
void pmic_set_mppt(void);
UINT32 charger_get_Time_To_MMI_Evt(void);
BOOL chargerCfgGetSetting(void);

#ifdef __cplusplus
}
#endif

#endif //_PMIC_M_H_

