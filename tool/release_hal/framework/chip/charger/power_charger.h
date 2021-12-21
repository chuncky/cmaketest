#include "charger_api.h"
#include "ui_log_api.h"


// =============================================================================
//  TYPES
// =============================================================================
typedef enum {
    CHR_STATUS_UNKNOWN,
    CHR_STATUS_AC_OFF,
    CHR_STATUS_AC_ON,
} PMU_CHR_STATUS_T;

typedef enum
{
    /// Charger is disconnected
    PMIC_CHARGE_DC_OFF,
    /// Charger is connected
    PMIC_CHARGE_DC_ON,
    /// Charger called by funtion timer
    PMIC_CHARGE_TIMER,
    /// Charger called by pmic_SetChargeCurrent (manual on/off of charge)
    PMIC_CHARGE_CURRENT,
    
    PMIC_CHARGE_EVENT_QTY
} PMIC_CHARGE_EVENT_T;

typedef struct
{
    PMIC_CHARGER_STATUS_T status;
    PMIC_CHARGE_CURRENT_T current;
    UINT16 batteryLevel;
#if 0
    UINT16 pulsedOnCount;
    UINT16 pulsedOffCount;

    UINT16 pulsedCycleCount;
    UINT16 pulsedOnMeanCount;
    UINT16 pulsedOffMeanCount;

    UINT32 startTime;
    UINT32 lastWTime;

    BOOL highActivityState;
#endif
} PMIC_CHARGER_STATE_T;

EXPORT PROTECTED PMIC_CHARGER_STATE_T g_pmicChargerState;
EXPORT PMIC_CHARGE_EVENT_T charger_event;

VOID pmic_InitCharger(VOID);
VOID pmic_RestoreChargerAtPowerOff(VOID);
VOID pmic_ChargerPlugIn(VOID);

typedef enum
{
    PMIC_PROFILE_MODE_NORMAL = 0,
    PMIC_PROFILE_MODE_LOWPOWER,

    PMIC_PROFILE_MODE_QTY,

} PMIC_PROFILE_MODE_T;

int pm812_get_charger_status(void);
unsigned int pm812_get_batt_vol(void);
int pm812_get_battert_status(void);
void ChargerManager(void);
void charger_dump_config(void);

