#include "ui_pm.h"
#include "plat_basic_api.h"
#include "power_charger.h"
#include "charger_api.h"
#include "ui_log_api.h"
#include "cmid.h"
//#include "errorcode.h"
#include "event.h"
//#include "drv.h"
//#include "task_cnf.h"
#include "base_prv.h"
//#include "ui_os_api.h"
#include "pmic_api.h"
#include "device.h"

// Timeouts
#define HAL_TICK1S 1000
#define MS_WAITING      *  HAL_TICK1S / 1000
//#ifndef PM_INCREASE_BATTERY_DETECT
//#define PM_INCREASE_BATTERY_DETECT 1
//#endif
volatile UINT32 Time_To_MMI_Evt      = 0;
#define PM_FILTRATE_BATTERY_STEP    800

#define PM_RF_TX_BATTERY_STEP       120

// Number of steps for the MEAN calculation
#define PM_AVERAGE_BATTERY_NB       32
// change threshold needed for stable value update
// 10 mV is 1% with current battery technology
#define PM_HYSTERESIS_BATTERY_STEP  50
#define PM_HYSTERESIS_BATTERY_PERCENT_EDGE 7
/// delay between battery monitoring during IDLE
#ifdef PMIC_TWO_BATTARY
#define PM_BAT_MONITOR_IDLE_TIME         40 * HAL_TICK1S
#else
#if (PM_INCREASE_BATTERY_DETECT)
#define PM_BAT_MONITOR_IDLE_TIME         15 * HAL_TICK1S
#else
#define PM_BAT_MONITOR_IDLE_TIME         15 * HAL_TICK1S//1 * 60 * HAL_TICK1S
#endif
#endif
/// delay between battery monitoring during CALL
#if (PM_INCREASE_BATTERY_DETECT)
#define PM_BAT_MONITOR_CALL_TIME         1 * HAL_TICK1S
#else
#define PM_BAT_MONITOR_CALL_TIME         5 * HAL_TICK1S
#endif
/// delay between battery monitoring during IDLE with low battery
#ifdef PMIC_TWO_BATTARY
#define PM_BAT_MONITOR_IDLE_LOWBAT_TIME   8 * HAL_TICK1S
#else
    #if (PM_INCREASE_BATTERY_DETECT)
    #define PM_BAT_MONITOR_IDLE_LOWBAT_TIME   1 * HAL_TICK1S
    #else
    #define PM_BAT_MONITOR_IDLE_LOWBAT_TIME   8 * HAL_TICK1S//15 * HAL_TICK1S
#endif
#endif
#if (PM_INCREASE_BATTERY_DETECT)
#define PM_BAT_MONITOR_IDLE_CRITICAL_TIME  1 * HAL_TICK1S
#else
#define PM_BAT_MONITOR_IDLE_CRITICAL_TIME  1 * HAL_TICK1S //5 * HAL_TICK1S

#endif

typedef struct
{
    /// sum the battery measures
    UINT32 sum;
    /// calculated mean level
    UINT16 avg;
    /// histeresys stable value
    UINT16 stable;
    /// last instant value
    UINT16 instant;
    /// array of measures to maintaint the mean
    UINT16 levels[PM_AVERAGE_BATTERY_NB];
    /// current index in the array
    UINT8 idx;
    /// number of measures (for init)
    UINT8 nb;
    /// calculated percent from the stable value
    UINT8 precent;

} PM_BATTERY_STABLE_T;

// Number of debounce for the ear detect
#define PM_AVERAGE_EAR_NB       10

typedef struct
{
    UINT8 mode;
    BOOL earon;
    BOOL gpadcMonStarted;
    BOOL gpadcMonDisabled;
} PM_EAR_DETECT_T;


PRIVATE PM_BATTERY_STABLE_T g_pmBatteryStable;

#ifdef PMIC_TWO_BATTARY
PRIVATE PM_BATTERY_STABLE_T g_pmBatteryStable_sub;


#endif

/// battery monitoring perriod
PRIVATE UINT32 g_pmBatMonTime     = PM_BAT_MONITOR_IDLE_TIME;
PRIVATE UINT32 g_pmBatMonLastDate = 0;
/// earpiece detect perriod
//PRIVATE UINT32 g_pmEarPieceDetLastDate = 0;
PRIVATE UINT8 g_pmChargeFlag           = 0;
PRIVATE UINT8 g_pmRemindTime           = 0;
// added private funtion to do the job with pmd
//PRIVATE UINT8 g_pm_poweronvolt = 0;
PRIVATE UINT8 g_pm_powerdowervolt = 0;
#ifdef PMIC_TWO_BATTARY
PRIVATE UINT8 g_pm_sub_poweronvolt = 0;
#endif

PMIC_CHARGER_STATUS_T g_pm_oldStatus = PMIC_CHARGER_UNKNOWN;

u8 gpadc_mutex = INVALID_MUTEX_ID;
PRIVATE UINT8 old_bat_percent;
UINT8 charger_present = 0;
//extern UINT8 charger_present_stop;

extern HANDLE get_dev_mon_task(void);
BOOL charger_status_is_usb = FALSE;
int send_usb_bootupStandby = 0;
BOOL Battery_charge_full = FALSE;
int is_bootup_standby = 0;
//extern BOOL cc_current_set_finished;
Usb_PlugCallback Send_usb_status = NULL;
Sim_CallCallback Get_call_status = NULL;
Battery_Callback Send_battery_status = NULL;

extern PMIC_CONFIG_T* g_pmicConfig;
UINT16 ChangeVoltageToBatteryRate(UINT8 * pPercent);

BOOL ChargerCallbackRegister(Usb_PlugCallback Usb_charger_status,Sim_CallCallback get_call_status,Battery_Callback battery_status)
{
    chargerCfgGetSetting();
    if(!Usb_charger_status || !get_call_status || !battery_status)
    {
        PMIC_DEBUG_TRACE("call back function is NULL!\n");
        return FALSE;
    }
	Send_usb_status = Usb_charger_status;
    Get_call_status = get_call_status;
    Send_battery_status = battery_status;

    return TRUE;
}
extern BOOL dm_SendPMMessage( UI_EVENT* pEv);
extern void SendMessageForUSB(unsigned int action);
/// @param status to convert
/// @return Bcs value
PRIVATE UINT8 pm_PmdChargerStatus2Bcs(PMIC_CHARGER_STATUS_T status)
{
	UINT8 bcs;
    bcs = PM_CHARGER_CONNECTED;

    // always go to the step 'connected' before any charge step
    if ( (g_pm_oldStatus <= PMIC_CHARGER_UNPLUGGED)
        && (status > PMIC_CHARGER_UNPLUGGED) )
    {
        g_pm_oldStatus = PMIC_CHARGER_PLUGGED;
        return bcs;
    }
    g_pm_oldStatus = status;

    switch (status)
    {
        case PMIC_CHARGER_UNKNOWN:
        case PMIC_CHARGER_UNPLUGGED:
            bcs = PM_CHARGER_DISCONNECTED;
            break;
        case PMIC_CHARGER_PLUGGED:
            bcs = PM_CHARGER_CONNECTED;
            break;
        case PMIC_CHARGER_PRECHARGE:
        case PMIC_CHARGER_FAST_CHARGE:
        case PMIC_CHARGER_PULSED_CHARGE:
        case PMIC_CHARGER_PULSED_CHARGE_WAIT:
            bcs = PM_CHARGER_CHRGING;
            break;
        case PMIC_CHARGER_FULL_CHARGE:
            bcs = PM_CHARGER_FINISHED;
            break;
        case PMIC_CHARGER_TEMPERATURE_ERROR:
            bcs = PM_CHARGER_ERROR_TEMPERATURE;
            break;
        case PMIC_CHARGER_VOLTAGE_ERROR:
            bcs = PM_CHARGER_ERROR_VOLTAGE;
            break;
        case PMIC_CHARGER_ERROR:
        case PMIC_CHARGER_TIMED_OUT:
        default:
            bcs = PM_CHARGER_ERROR_UNKNOWN;
            break;
    }
    return bcs;
}

PRIVATE BOOL pm_BatteryLevel2State(UINT16 batLevel, UINT8* pBcl, UINT16* pBatState)
{


    if ((pBcl == NULL) || (pBatState == NULL))
    {
        return FALSE;
    }
    if (batLevel == PMIC_BAD_VALUE)
    {
        *pBcl      = 0;
        *pBatState = PM_BATTERY_STATE_UNKNOWN;
    }
    else
    {
        if (*pBcl == 0)
        {
 //           *pBcl      = 1;
            *pBatState = PM_BATTERY_STATE_SHUTDOWN;
        }
        else
        {
            /// @todo: not hardcode levels
            if (*pBcl < g_pm_powerdowervolt) // if (*pBcl < 20)
            {
                *pBatState = PM_BATTERY_STATE_SHUTDOWN;
            }
            else if (*pBcl < (g_pm_powerdowervolt +5) )//if (*pBcl < 25)
            {
                *pBatState = PM_BATTERY_STATE_CRITICAL;
            }
            else if (*pBcl < (g_pm_powerdowervolt +10)) //if (*pBcl < 30)
            {
                *pBatState = PM_BATTERY_STATE_LOW;
            }
            else
            {
                *pBatState = PM_BATTERY_STATE_NORMAL;
            }
        }
      //  *pBatState = PM_BATTERY_STATE_NORMAL; // only use the narmal status.
    }
    
    return TRUE;
}

#ifdef PMIC_TWO_BATTARY
extern PUBLIC BOOL pmic_GetMainStatus(VOID);
#define PM_MAIN_DETECTCOUNT 30
UINT16 g_main_battery_detector = PM_AVERAGE_BATTERY_NB;
BOOL g_main_battery_flag = FALSE,g_start_quick_detect_flag = FALSE;
#define MOINTER_TEMP_FOR_PLUGIN_MIAN_BATTERY 220 MS_WAITING

PRIVATE INT pm_GetStableBatteryLevel_sub(UINT8 * pPercent)
{
    UINT8 bcl = 0,i=0;
    UINT8 avgPercent;
    INT16 batLevelTemp1 = 0,batLevelTemp2 = 0;
    UINT16 batLevel = 0;
    if(pmic_GetCurrentBattery() == PMIC_BATTERY_MAIN)
    {
        pmic_SetCurrentBattery(PMIC_BATTERY_SUB);
        batLevel = pmic_GetBatteryLevel(&bcl);
        pmic_SetCurrentBattery(PMIC_BATTERY_MAIN);
    }
    else
    {
        batLevel = pmic_GetBatteryLevel(&bcl);
    }
 
    if (batLevel == PMIC_BAD_VALUE)
    {
        PMIC_DEBUG_TRACE("pm: sub PMIC_BAD_VALUE\n");
        return PMIC_BAD_VALUE;
    }
    if (batLevel < 2500)
    {
        PMIC_DEBUG_TRACE("pm: sub PMIC_BAD_VALUE too small\n");
        return PMIC_BAD_VALUE;
    }

 
    if (g_pmBatteryStable_sub.nb == PM_AVERAGE_BATTERY_NB)
    {
        if (g_pmBatteryStable_sub.avg > batLevel)
        {
            if ((g_pmBatteryStable_sub.avg - batLevel) > PM_FILTRATE_BATTERY_STEP )
            {
                PMIC_DEBUG_TRACE("pm: sub too small %dmv\n",batLevel);
                return PMIC_BAD_VALUE;
            }
        }

    }
    g_pmBatteryStable_sub.sum -= g_pmBatteryStable_sub.levels[g_pmBatteryStable_sub.idx];
    g_pmBatteryStable_sub.sum += batLevel;
    g_pmBatteryStable_sub.levels[g_pmBatteryStable_sub.idx] = batLevel;
    if (g_pmBatteryStable_sub.nb < PM_AVERAGE_BATTERY_NB)
    {
        g_pmBatteryStable_sub.nb++;
    }
    g_pmBatteryStable_sub.idx++;
    if (g_pmBatteryStable_sub.idx >= PM_AVERAGE_BATTERY_NB)
    {
        g_pmBatteryStable_sub.idx = 0;
    }
    //.....
    if (g_pmBatteryStable_sub.nb > 10)
    {
        batLevelTemp1 = g_pmBatteryStable_sub.levels[0];
        for (i=0;i<g_pmBatteryStable_sub.nb;i++)
        {
          if (batLevelTemp1 > g_pmBatteryStable_sub.levels[i])
          {
            batLevelTemp1 = g_pmBatteryStable_sub.levels[i];
          }
        }
        batLevelTemp2 = g_pmBatteryStable_sub.levels[0];
        for (i=0;i<g_pmBatteryStable_sub.nb;i++)
        {
          if (batLevelTemp2 < g_pmBatteryStable_sub.levels[i])
          {
            batLevelTemp2 = g_pmBatteryStable_sub.levels[i];
          }
        }
        batLevelTemp1 += batLevelTemp2;
        g_pmBatteryStable_sub.avg = (g_pmBatteryStable_sub.sum - batLevelTemp1 ) / (g_pmBatteryStable_sub.nb - 2);

    }
    else
    {
        g_pmBatteryStable_sub.avg = g_pmBatteryStable_sub.sum / g_pmBatteryStable_sub.nb;
    }
    g_pmBatteryStable_sub.instant = batLevel;
    g_pmBatteryStable_sub.avg = g_pmBatteryStable_sub.sum / g_pmBatteryStable_sub.nb;
    // hysteresis leveling:
    if ((((INT32)g_pmBatteryStable_sub.stable - (INT32)g_pmBatteryStable_sub.avg)
            >= PM_HYSTERESIS_BATTERY_STEP)
        || (((INT32)g_pmBatteryStable_sub.avg - (INT32)g_pmBatteryStable_sub.stable)
            >= PM_HYSTERESIS_BATTERY_STEP))
    {
        g_pmBatteryStable_sub.stable = g_pmBatteryStable_sub.avg;
    }
    // forcing hysteresis for the last few % (near 0% and 100%) 
    avgPercent = pmic_BatteryLevelToPercent(g_pmBatteryStable_sub.avg);
    if ((avgPercent <= PM_HYSTERESIS_BATTERY_PERCENT_EDGE + 13)
        || (avgPercent >= (100-PM_HYSTERESIS_BATTERY_PERCENT_EDGE)))
    {
        g_pmBatteryStable_sub.stable = g_pmBatteryStable_sub.avg;
    }


    // calculate %, print and return value...
    g_pmBatteryStable_sub.precent = pmic_BatteryLevelToPercent(g_pmBatteryStable_sub.stable);
    if (pPercent != NULL)
    {
        *pPercent = g_pmBatteryStable_sub.precent;
    }
    PMIC_DEBUG_TRACE("pm: sub instant: %dmV, %d%%; mean: %dmV, %d%%; stable: %dmV, %d%%\n",
                              batLevel, bcl, g_pmBatteryStable_sub.avg, avgPercent, g_pmBatteryStable_sub.stable, g_pmBatteryStable_sub.precent);
    return g_pmBatteryStable_sub.stable;
}

#endif

PRIVATE INT pm_GetStableBatteryLevel(UINT8 * pPercent)
{
    UINT8 bcl = 0,i=0;
    UINT8 avgPercent;
    INT16 batLevelTemp1 = 0,batLevelTemp2 = 0;
    #ifdef PMIC_TWO_BATTARY
    UINT16 batLevel = 0;
    if (pmic_GetMainStatus() == FALSE)
    {
      return PMIC_BAD_VALUE;
    }
    if(pmic_GetCurrentBattery() == PMIC_BATTERY_SUB)
    {
        pmic_SetCurrentBattery(PMIC_BATTERY_MAIN);

        batLevel = pmic_GetBatteryLevel(&bcl);
        pmic_SetCurrentBattery(PMIC_BATTERY_SUB);
    }
    else
    {
        batLevel = pmic_GetBatteryLevel(&bcl);
    }
    #else
    
    UINT16 batLevel = pmic_GetBatteryLevel(&bcl);
//    raw_uart_log("Battery value = 0x%x\n",batLevel);
    #endif
    #ifdef PMIC_TWO_BATTARY
    if (pmic_GetMainStatus() == FALSE)
    {
      PMIC_DEBUG_TRACE("pm:  main battery off");
      return PMIC_BAD_VALUE;
    }
    
    if (batLevel > 5500)
    {
        PMIC_DEBUG_TRACE("pm:  PMIC_BAD_VALUE too larger");
        return PMIC_BAD_VALUE;
    }
    #endif

    
    if (batLevel == PMIC_BAD_VALUE)
    {
        PMIC_DEBUG_TRACE("pm:  PMIC_BAD_VALUE");
        return PMIC_BAD_VALUE;
    }
    if (batLevel < 2000)
    {
        PMIC_DEBUG_TRACE("pm:  PMIC_BAD_VALUE too small\n");
        return PMIC_BAD_VALUE;
    }
 
    if (g_pmBatteryStable.nb == PM_AVERAGE_BATTERY_NB)
    {
        if (g_pmBatteryStable.avg > batLevel)
        {
            if ((g_pmBatteryStable.avg - batLevel) > PM_FILTRATE_BATTERY_STEP )
            {
                PMIC_DEBUG_TRACE("pm:  too small %dmv",batLevel);
                #ifndef PMIC_TWO_BATTARY
                return PMIC_BAD_VALUE;
                #endif
            }
        }
    }

    if (g_pmBatteryStable.avg > batLevel + PM_RF_TX_BATTERY_STEP)
    {
        UINT32 callStatus = 0x00;
#ifdef CMID_MULTI_SIM
        CMID_SIM_ID nSimId;
        for(nSimId = CMID_SIM_0; nSimId < CMID_SIM_COUNT; nSimId++)
        {
            if(Get_call_status)
                callStatus |= Get_call_status(nSimId);
            else
                callStatus |= CMID_CcGetCallStatus(nSimId);
        }
#else
        if(Get_call_status)
            callStatus |= Get_call_status(0);
        else
            callStatus = CMID_CcGetCallStatus();
#endif

        if (callStatus != 0)  // not in CC_STATE_NULL
        {
            // The battery level might be measured at the time RF was transmitting (PA was working).
            // Try to compensate the instant battery level drop.
            batLevel = (UINT16)(batLevel/8 + (UINT32)g_pmBatteryStable.avg*7/8);
        }
    }

    g_pmBatteryStable.sum -= g_pmBatteryStable.levels[g_pmBatteryStable.idx];
    g_pmBatteryStable.sum += batLevel;
    g_pmBatteryStable.levels[g_pmBatteryStable.idx] = batLevel;
    if (g_pmBatteryStable.nb < PM_AVERAGE_BATTERY_NB)
    {
        g_pmBatteryStable.nb++;
    }
    g_pmBatteryStable.idx++;
    if (g_pmBatteryStable.idx >= PM_AVERAGE_BATTERY_NB)
    {
        g_pmBatteryStable.idx = 0;
    }
    //.....
    if (g_pmBatteryStable.nb > 10)
    {
        batLevelTemp1 = g_pmBatteryStable.levels[0];
        for (i=0;i<g_pmBatteryStable.nb;i++)
        {
          if (batLevelTemp1 > g_pmBatteryStable.levels[i])
          {
            batLevelTemp1 = g_pmBatteryStable.levels[i];
          }
        }
        batLevelTemp2 = g_pmBatteryStable.levels[0];
        for (i=0;i<g_pmBatteryStable.nb;i++)
        {
          if (batLevelTemp2 < g_pmBatteryStable.levels[i])
          {
            batLevelTemp2 = g_pmBatteryStable.levels[i];
          }
        }
        batLevelTemp1 += batLevelTemp2;
        g_pmBatteryStable.avg = (g_pmBatteryStable.sum - batLevelTemp1 ) / (g_pmBatteryStable.nb - 2);

    }
    else
    {
        g_pmBatteryStable.avg = g_pmBatteryStable.sum / g_pmBatteryStable.nb;
    }
    g_pmBatteryStable.instant = batLevel;
//    g_pmBatteryStable.avg = g_pmBatteryStable.sum / g_pmBatteryStable.nb;
    // hysteresis leveling:
    if ((((INT32)g_pmBatteryStable.stable - (INT32)g_pmBatteryStable.avg)
            >= PM_HYSTERESIS_BATTERY_STEP)
        || (((INT32)g_pmBatteryStable.avg - (INT32)g_pmBatteryStable.stable)
            >= PM_HYSTERESIS_BATTERY_STEP))
    {
        g_pmBatteryStable.stable = g_pmBatteryStable.avg;
    }
    // forcing hysteresis for the last few % (near 0% and 100%) 
    avgPercent = pmic_BatteryLevelToPercent(g_pmBatteryStable.avg);
    if ((avgPercent <= PM_HYSTERESIS_BATTERY_PERCENT_EDGE + 13)
        || (avgPercent >= (100-PM_HYSTERESIS_BATTERY_PERCENT_EDGE)))
    {
        g_pmBatteryStable.stable = g_pmBatteryStable.avg;
    }


    // calculate %, print and return value...
    g_pmBatteryStable.precent = pmic_BatteryLevelToPercent(g_pmBatteryStable.stable);
    if (pPercent != NULL)
    {
        *pPercent = g_pmBatteryStable.precent;
    }
    PMIC_DEBUG_TRACE("pm:    instant: %dmV, %d%%; mean: %dmV, %d%%; stable: %dmV, %d%%\n",
                              batLevel, bcl, g_pmBatteryStable.avg, avgPercent, g_pmBatteryStable.stable, g_pmBatteryStable.precent);

    #ifdef PMIC_TWO_BATTARY
    PMIC_DEBUG_TRACE("pm:here flag =%d, count = %d flag %d\n",g_start_quick_detect_flag,g_main_battery_detector,g_start_quick_detect_flag);
    if (g_start_quick_detect_flag == TRUE)
    {   
        if(g_main_battery_detector <= (PM_AVERAGE_BATTERY_NB -3))
        { 
            g_main_battery_detector ++;
            if (batLevel > g_pmBatteryStable.stable)
            {
                if (batLevel - g_pmBatteryStable.stable > 80)
                {
                  PMIC_DEBUG_TRACE("pm: > plugin Mbattery, %dmv count = %d\n",batLevel,g_main_battery_detector);
                  return PMIC_BAD_VALUE;
                }
            }
            else if (batLevel < g_pmBatteryStable.stable)
            {
                if (g_pmBatteryStable.stable - batLevel > 150)
                {
                  PMIC_DEBUG_TRACE("pm: < plugin Mbattery, %dmv count =%d\n",batLevel,g_main_battery_detector);
                  return PMIC_BAD_VALUE;
                }
            }   
        }
        
    }
    #endif

    return g_pmBatteryStable.stable;
}

 BOOL pm_BattChargeRemind(PMIC_CHARGER_STATUS_T status)
{
    UINT8  bcl      = 0;
    UINT8  bcs      = 0;
    UINT16 batState = 0;
    UINT16 batLevel = 0;
    UI_EVENT ev;
    if (g_pmChargeFlag == 1)
    {
        #ifndef PMIC_TWO_BATTARY

        batLevel = g_pmBatteryStable.stable;
        bcl      = g_pmBatteryStable.precent;
//        raw_uart_log("pm: Remind when plugin or out\n");
        #else
        if(pmic_GetCurrentBattery() == PMIC_BATTERY_SUB)
        {
            batLevel = g_pmBatteryStable_sub.stable;
            bcl  = g_pmBatteryStable_sub.precent;
           PMIC_DEBUG_TRACE("pm: Remind when plugin or out sub bcl=%d\n",bcl);
        }
        else
        {
           batLevel = g_pmBatteryStable.stable;
           bcl      = g_pmBatteryStable.precent;
           PMIC_DEBUG_TRACE("pm: Remind when plugin or out main bcl=%d\n",bcl);
        }

        #endif
        if(status == PMIC_CHARGER_PLUGGED)
            bcs = PM_CHARGER_CONNECTED;
		else
			bcs = PM_CHARGER_DISCONNECTED;
    }
    else
    {
        #ifdef PMIC_TWO_BATTARY
        UINT8  bcl_sub = 0;
        PMIC_DEBUG_TRACE("pm: pm_BattChargeRemind .");
        batLevel = pm_GetStableBatteryLevel(&bcl);
        UINT16 batLevel_sub = pm_GetStableBatteryLevel_sub(&bcl_sub);
        if(pmic_GetCurrentBattery() == PMIC_BATTERY_SUB)
        {
            batLevel = batLevel_sub;
            bcl = bcl_sub;
        }
        #else
		if(g_pmicConfig->Has_battery_discharger_table)
        	batLevel = ChangeVoltageToBatteryRate(&bcl);
		else
        	batLevel = pm_GetStableBatteryLevel(&bcl);
        #endif

		if ( (g_pm_oldStatus == PMIC_CHARGER_FULL_CHARGE)&& (status == PMIC_CHARGER_FULL_CHARGE))
		{
			PMIC_DEBUG_TRACE("pm: Remind Full charge not reporting to MMI anymore Bcl=%d%%, batLevel=%dmV\n", bcl, batLevel);
			return TRUE;
		}
		if ( (g_pm_oldStatus == PMIC_CHARGER_TIMED_OUT)&& (status == PMIC_CHARGER_TIMED_OUT))
		{
			PMIC_DEBUG_TRACE("pm: Remind timeout charge not reporting to MMI anymore Bcl=%d%%, batLevel=%dmV\n", bcl, batLevel);
			return TRUE;
		}
		if ((batLevel == PMIC_BAD_VALUE) || (batLevel < 2000))
		{
			PMIC_DEBUG_TRACE("pm: Remind PMIC_BAD_VALUE\n");
			return FALSE;
		}
		bcs = pm_PmdChargerStatus2Bcs(status);
#ifndef For_Watch_Battery
		pm_BatteryLevel2State(batLevel, &bcl, &batState);
//    raw_uart_log("pm:..  bcs %d  batLevel =%dmv\n",bcs,batLevel);
		if (bcs == PM_CHARGER_DISCONNECTED)
		{
			if (batState == PM_BATTERY_STATE_SHUTDOWN)
			{
				PMIC_DEBUG_TRACE("pm: PM_BATTERY_STATE_SHUTDOWN times= %d ,V =%dmV\n",g_pmRemindTime,batLevel);
				g_pmRemindTime ++;
				if (g_pmRemindTime <= 3)//if (g_pmRemindTime <= 15)
				{
					return FALSE;
				}
				else
				{
					g_pmRemindTime = 0;
				}
			}
			else
			{
				g_pmRemindTime = 0;
			}
		}
		else
		{
			g_pmRemindTime = 15;
		}

		if(IsBatteryInCharge())
		{
			if(bcl < old_bat_percent)  //set bcl not little before display bcl
				bcl = old_bat_percent;
		}
		else
		{
			if(bcl>old_bat_percent)  //set bcl not big before display bcl
				bcl = old_bat_percent;
		}
    // when no measure valid for battery : bcl = BATTERY_PERCENT_LEVEL_40
		if (batState == PM_BATTERY_STATE_UNKNOWN)
		{
			PMIC_DEBUG_TRACE("pm: Remind PMIC_BAD_VALUE ,error status\n");
			return FALSE;
		}
#endif

    // force histeresys to 100% when charge is full
		switch (status)
		{
		case PMIC_CHARGER_FULL_CHARGE:
			{
				g_pmBatteryStable.stable = g_pmBatteryStable.avg;
				g_pmBatteryStable.precent = 100;
				bcl = 100;
			}
			break;
		case PMIC_CHARGER_PLUGGED:
		case PMIC_CHARGER_PRECHARGE:
		case PMIC_CHARGER_FAST_CHARGE:
		case PMIC_CHARGER_PULSED_CHARGE:
		case PMIC_CHARGER_PULSED_CHARGE_WAIT:
			{
#ifndef For_Watch_Battery
				if (bcl > 99)
				{
					g_pmBatteryStable.precent = 99;
					bcl = 99;
				}
#endif
			}
			break;
		default:
			break;
		}
		PMIC_DEBUG_TRACE("pm: Remind Bcs=%d, Bcl=%d%%, batState=%d, stable_volt=%dmV\n", bcs, bcl, batState, batLevel);
    }
    if(Send_battery_status)
    {
        if(batLevel <= 3400)
            bcl = 0; //for watch
        Send_battery_status(bcs,bcl,batLevel);
    }
    else
    {
        ev.nEventId = EV_PM_BC_IND;
        ev.nParam1  = MAKEUINT32(bcl, bcs);
        //ev.nParam2  = MAKEUINT32(batState, 0);
        ev.nParam2  = MAKEUINT32(PM_BATTERY_STATE_NORMAL, 0);
        ev.nParam3  = MAKEUINT32(batLevel, 0);
        dm_SendPMMessage(&ev);
    }
    old_bat_percent =  bcl;
    return TRUE;
}

uint8 is_plugin_tip_out = 0;
uint8 is_plugout_msg_send = 1;
uint8 is_plugin_tip_out_charger = 0;
uint8 is_plugout_msg_send_charger = 1;
uint32 usb_cnt = 0;
uint8  usb_first = 1;
VOID pm_UsbMonitorProc(VOID)
{
	UOS_SuspendTask((TASK_HANDLE*)get_usb_mon_task());
    PMIC_DEBUG_TRACE("usb_monitor:%d\n",usb_cnt++);
#if For_Watch_Battery
	if(1) /*charger_is_usb() spends 2s*/
#else
	if(charger_is_usb())
#endif
    {
		if(pm812_get_charger_status())
		{
			charger_status_is_usb = TRUE;
			if(Send_usb_status){
				Send_usb_status(DEVUSB_DETECT_ACTION_PLUGIN);
			}
			else if(is_bootup_standby)
            {
				if(PM813_get_Vbus_ov_uv_status())
				{
					PMIC_TRACE("usb Charger voltage error!\n");
					charger_event = PMIC_CHARGE_DC_OFF;
					charger_present = 0;
					g_pmicChargerState.status = PMIC_CHARGER_VOLTAGE_ERROR;
					pm_BattChargeRemind(g_pmicChargerState.status);
            	}
				else
				{
					is_plugin_tip_out = 0;
					SendMessageForUSB(DEVUSB_DETECT_ACTION_PLUGIN);
					PMIC_TRACE("usb Charger plug in!\n");
				}
	        }
	        else{
				send_usb_bootupStandby = 1;
                PMIC_DEBUG_TRACE("usb Charger plug in!,line:%d\r\n",__LINE__);
	        }
	    }
    }
    else
	{
            charger_status_is_usb = FALSE;
            if(pm812_get_charger_status())
            {
                if(is_bootup_standby)
                {
					if(PM813_get_Vbus_ov_uv_status())
					{
						PMIC_TRACE("adapter Charger voltage error!\n");
						charger_event = PMIC_CHARGE_DC_OFF;
						charger_present = 0;
						g_pmicChargerState.status = PMIC_CHARGER_VOLTAGE_ERROR;
						pm_BattChargeRemind(g_pmicChargerState.status);
            		}
					else
					{
                		is_plugin_tip_out_charger = 0;
                    	SendMessageForUSB(DEVCHARGER_DETECT_ACTION_PLUGIN);
						PMIC_TRACE("adapter Charger plug in!\n");
					}
                }
                else
                {
                    send_usb_bootupStandby = 1;
                    PMIC_DEBUG_TRACE("adapter Charger plug in!,line:%d\r\n",__LINE__);
                }
            }
	}
}
extern BOOL getUSBConnectStatus(void); //defined in cp code
extern void SendMessageForUSB_nofull_tip(unsigned int action,unsigned char full_tip);
VOID pm_ChargerHandler(void)
{
	PMIC_CHARGER_STATUS_T status;
//	raw_uart_log("charger interrupt cnt:%d\n",charger_cnt++);
//	UOS_Sleep(6);//wait 30ms for usb_shutdown status updata
	if(getUSBConnectStatus()){
//        raw_uart_log("plug in:%d\n",plugin_cnt++);
		charger_event = PMIC_CHARGE_DC_ON;
		status = PMIC_CHARGER_PLUGGED;
        charger_present = 1;
#if For_Watch_Battery
        charger_status_is_usb = TRUE;
        if(Send_usb_status){
            Send_usb_status(DEVUSB_DETECT_ACTION_PLUGIN);
        }
#else
#ifndef CONFIG_RELEASE_WITH_NO_ASR_UI
		UOS_ResumeTask((TASK_HANDLE*)get_usb_mon_task());
#endif
#endif
	}else{
//        raw_uart_log("plug out:%d\n",plugout_cnt++);
		charger_event = PMIC_CHARGE_DC_OFF;
		status = PMIC_CHARGER_UNPLUGGED;
        charger_present = 0;
        if(charger_status_is_usb){
            if(Send_usb_status)
                Send_usb_status(DEVUSB_DETECT_ACTION_PLUGOUT);
            else if(!Battery_charge_full)
            {
                if(is_bootup_standby)
                {
                	if(is_plugin_tip_out || usb_first){
						is_plugout_msg_send = 1;
                    	SendMessageForUSB(DEVUSB_DETECT_ACTION_PLUGOUT);
						usb_first = 0;
//                    	raw_uart_log("usb Charger plug out!,line:%d\r\n",__LINE__);
                	}
					else
					{
						is_plugout_msg_send = 0;
					}
//					raw_uart_log("usb charger plug out,is_plugin_tip_out=%d \n",is_plugin_tip_out);
                }
            }
            else
            {
            	if(is_plugin_tip_out || usb_first){
					is_plugout_msg_send = 1;
                	SendMessageForUSB_nofull_tip(DEVUSB_DETECT_ACTION_PLUGOUT,1);
					usb_first = 0;
//                	raw_uart_log("charger full usb Charger plug out!\r\n");
            	}
				else
				{
					is_plugout_msg_send = 0;
				}
            }
        }
        else if(!Battery_charge_full)
        {
            if(is_bootup_standby) 
			{				
				if(is_plugin_tip_out_charger || usb_first){
					is_plugout_msg_send_charger = 1;
                	SendMessageForUSB(DEVCHARGER_DETECT_ACTION_PLUGOUT);
					usb_first = 0;
//                	raw_uart_log("adapter charger plug out \n");
				}
				else
				{
					is_plugout_msg_send_charger = 0;
				}
        	}
        }
        send_usb_bootupStandby = 0;
//        charger_present_stop = 1;
	}
    g_pmicChargerState.status = status;
    g_pmChargeFlag = 1;
    pm_BattChargeRemind(status);
    g_pmChargeFlag = 0;
//	raw_uart_log("charger interrupt end\n");
}
#ifdef PMIC_TWO_BATTARY
#define PM_BATTERY_DETECT_INTERVAL 3700
#else
#define PM_BATTERY_DETECT_INTERVAL 3620
#endif

PRIVATE VOID pm_doMonitoring(BOOL fromLowPowerEvent)
{
    UINT32 now;
	UINT32 callStatus;
	UINT16 batState,batLevel;
	UINT8  bcl;
	PMIC_CHARGER_STATUS_T status;
    now = timerCountRead(TS_TIMER_ID);
    if((now - g_pmBatMonLastDate) >= g_pmBatMonTime)
    {
        // monitor battery & charger and tell mmi
        status = pmic_GetChargerStatus();
        if (pm_BattChargeRemind(status))
        {
            // successful, record it
            g_pmBatMonLastDate = now;
        }
    }
    callStatus = 0x00;
	batLevel = g_pmBatteryStable.instant;
	bcl = pmic_BatteryLevelToPercent(batLevel);
	pm_BatteryLevel2State(batLevel, &bcl, &batState);
    if (fromLowPowerEvent)
    {
        // called from lp event, assuming not in call then ;)
    }
    else
    {
#ifdef CMID_MULTI_SIM
        CMID_SIM_ID nSimId;
        for(nSimId = CMID_SIM_0; nSimId < CMID_SIM_COUNT; nSimId++)
        {
            if(Get_call_status)
                callStatus |= Get_call_status(nSimId);
            else
                callStatus |= CMID_CcGetCallStatus(nSimId);
        }
#else
        if(Get_call_status)
            callStatus |= Get_call_status(0);
        else
            callStatus = CMID_CcGetCallStatus();
#endif
    }

    if (callStatus == 0 /*CC_STATE_NULL*/)
    {   // not in call
        if (batState == PM_BATTERY_STATE_LOW)
        {
            g_pmBatMonTime = PM_BAT_MONITOR_IDLE_LOWBAT_TIME;

        }
        else if (batState == PM_BATTERY_STATE_CRITICAL)
        {
            g_pmBatMonTime = PM_BAT_MONITOR_IDLE_CRITICAL_TIME;
        }
        else if (batState == PM_BATTERY_STATE_SHUTDOWN)
        {
            g_pmBatMonTime = PM_BAT_MONITOR_IDLE_CRITICAL_TIME;
        }
        else
        {   
            if (g_pmBatteryStable.stable > PM_BATTERY_DETECT_INTERVAL)
            {
                g_pmBatMonTime = PM_BAT_MONITOR_IDLE_TIME;
            }
            else
            {
                g_pmBatMonTime = PM_BAT_MONITOR_IDLE_LOWBAT_TIME;
            }
        }
    }
    else
    {   // in call
		if ((batState == PM_BATTERY_STATE_CRITICAL) || (batState == PM_BATTERY_STATE_SHUTDOWN))
			g_pmBatMonTime = PM_BAT_MONITOR_IDLE_CRITICAL_TIME;
		else
			g_pmBatMonTime = PM_BAT_MONITOR_CALL_TIME;
    }  
#ifdef PMIC_TWO_BATTARY
    if (g_pmBatMonTime > PM_BAT_MONITOR_IDLE_TIME)
    {
        g_pmBatMonTime = PM_BAT_MONITOR_IDLE_TIME;
    }
    if (pmic_GetMainStatus() == FALSE)
    {
      g_main_battery_detector = 0;
      g_start_quick_detect_flag = FALSE;
    }
    else
    {
       g_start_quick_detect_flag = TRUE; 
       if (g_main_battery_detector <= (PM_AVERAGE_BATTERY_NB -3))
       {
           g_pmBatMonTime = MOINTER_TEMP_FOR_PLUGIN_MIAN_BATTERY;
       }
    }
#endif

    Time_To_MMI_Evt = g_pmBatMonTime;
}

BOOL PM_GetBatteryInfo(UINT8* pBcs, UINT8* pBcl, UINT16* pMpc)
{
    if (pBcs == NULL || pBcl == NULL || pMpc == NULL)
    {
        return FALSE;
    }

    {
        PMIC_CHARGER_STATUS_T status = pmic_GetChargerStatus();
        if (status >= PMIC_CHARGER_PLUGGED)
        {
            *pBcs = PM_CHARGER_CONNECTED;
        }
        else
        {
            *pBcs = PM_CHARGER_DISCONNECTED;
        }
    {
        UINT16 batLevel;
        UINT16 batState;
        // don't actually read from battery, get value from g_pmBatteryStable
        // if available. So that the mean calculation is done on the regular reporting basis only.
        PMIC_DEBUG_TRACE("g_pmBatteryStable.nb   =%d",g_pmBatteryStable.nb);
        if (g_pmBatteryStable.nb == 0)
        {
        #ifdef PMIC_TWO_BATTARY
        if(pmic_GetCurrentBattery() == PMIC_BATTERY_SUB)
        {
            pm_GetStableBatteryLevel(pBcl);
            batLevel = pm_GetStableBatteryLevel_sub(pBcl);
        }
        else
        {
            batLevel = pm_GetStableBatteryLevel(pBcl);
        }
        #else
        batLevel = pm_GetStableBatteryLevel(pBcl);
        #endif
          
        }
        else
        {
            #ifndef PMIC_TWO_BATTARY

            batLevel = g_pmBatteryStable.stable;
            *pBcl = g_pmBatteryStable.precent;
            #else
            if(pmic_GetCurrentBattery() == PMIC_BATTERY_SUB)
            {
                batLevel = g_pmBatteryStable_sub.stable;
               *pBcl = g_pmBatteryStable_sub.precent;
              
            }
            else
            {
                batLevel = g_pmBatteryStable.stable;
               *pBcl = g_pmBatteryStable.precent;

            }
            

            #endif
        }
        pm_BatteryLevel2State(batLevel, pBcl, &batState);
        PMIC_DEBUG_TRACE("PM_GetBatteryInfo Bcs=%d,BatteryState=%d,Bcl=%d%% ,batLevel =%d mv\n",*pBcs, batState, *pBcl,batLevel);
    }
    *pMpc = 0;

    PMIC_DEBUG_TRACE("PM_GetBatteryInfo Bcs=%d, Bcl=%d%%\n", *pBcs, *pBcl);


    return TRUE;
    }
}

//BOOL PM_CheckValidPowerOnVoltage(UINT16 nVoltage)
//{
//	if(pmic_GetGpadcBatteryLevel() >= nVoltage)
//		return TRUE;
//	else
//		return FALSE;
//}

BOOL PM_GetBatteryState(PM_BATTERY_INFO* pBatInfo)
{
    // get voltage info from g_pmBatteryStable structure
    // any measure done yet ?
    if (g_pmBatteryStable.nb == 0)
    {
        // no! just exit
        return FALSE;
    }
    pBatInfo->nBatteryRealTimeVoltage = g_pmBatteryStable.instant;
    
    pBatInfo->nBatteryVoltage = g_pmBatteryStable.stable;

    pBatInfo->nBatteryLifePercent  = g_pmBatteryStable.precent;

    pBatInfo->nBatteryTemperature  = pmic_GetBatteryTemperature();
    // no way to measure:
    pBatInfo->nBatteryCurrent = 0;
#ifdef BATT_FULL_TIME
    pBatInfo->nBatteryLifeTime     = BATT_FULL_TIME * g_pmBatteryStable.precent / 100;
    pBatInfo->nBatteryFullLifeTime = BATT_FULL_TIME;
#else
    pBatInfo->nBatteryLifeTime     = 0;
    pBatInfo->nBatteryFullLifeTime = 0;
#endif
    {
        UINT8 bcl       = g_pmBatteryStable.precent;
        UINT16 batState = 0;
        pm_BatteryLevel2State(pBatInfo->nBatteryRealTimeVoltage, &bcl, &batState);
        pBatInfo->nBatteryFlag = batState;
    }
    return TRUE;
}

#ifdef PMIC_TWO_BATTARY

//#define POWER_ON_VOL_PER 25
extern VOID pmic_SetPMSub();
extern VOID pmic_SetPMMain();

#else
//#define POWER_ON_VOL_PER 10
#endif

BOOL pm_BatteryInit(VOID)
{
	UINT32 now;

    now = timerCountRead(TS_TIMER_ID);
	pmic_InitCharger();
    Time_To_MMI_Evt = g_pmBatMonTime;
    g_pmBatMonLastDate = now - g_pmBatMonTime;
	gpadc_mutex = UOS_NewMutex("gpadc");
#ifndef FPGA
    /// shutdown if battery too low & no charger
    {
        UINT16 batLevel = PMIC_BAD_VALUE;
        UINT8 bcl = 40;
        #ifdef PMIC_TWO_BATTARY
        UINT16 batLevel_sub = PMIC_BAD_VALUE;
        UINT8 bcl_sub = 40;
        #endif
/*         UINT16 batState = 0; */
        PMIC_CHARGER_STATUS_T status = PMIC_CHARGER_UNKNOWN;
//        g_pm_poweronvolt = pmic_GetPowerOnVolt();
        g_pm_powerdowervolt = pmic_GetPowerDownVolt();
        #ifdef PMIC_TWO_BATTARY
        g_pm_sub_poweronvolt = pmic_GetSubPowerOnVolt();
        #endif

        // wait for a valid charger detection and valid battery measurement
        while ((batLevel == PMIC_BAD_VALUE) || (status == PMIC_CHARGER_UNKNOWN))
        {
            status = pmic_GetChargerStatus();
            #ifdef PMIC_TWO_BATTARY
            pmic_SetCurrentBattery(PMIC_BATTERY_SUB);
            batLevel_sub = pmic_GetBatteryLevel(&bcl_sub);
            pmic_SetCurrentBattery(PMIC_BATTERY_MAIN);
            batLevel = pmic_GetBatteryLevel(&bcl);
            if (batLevel == PMIC_BAD_VALUE)
            {
                batLevel = batLevel_sub;
                bcl = bcl_sub;
                pmic_SetCurrentBattery(PMIC_BATTERY_SUB);
            }
            #else
            batLevel = pmic_GetBatteryLevel(&bcl);
            old_bat_percent = bcl;
            #endif
        }
            #ifdef PMIC_TWO_BATTARY
            pmic_SetPMMain();
            #endif

        //pm_BatteryLevel2State(batLevel, &bcl, &batState);
#if 0
        if ((status == PMIC_CHARGER_UNPLUGGED) && (bcl <= g_pm_poweronvolt) )
        {   
            #ifdef PMIC_TWO_BATTARY
                if (bcl_sub < g_pm_sub_poweronvolt)
                {
                    os_SysShutdown();
                }
                else
                {
                    pmic_SetCurrentBattery(PMIC_BATTERY_SUB);
                    pmic_SetPMSub();
                }
                  
            #else
                PMIC_DEBUG_TRACE("The Battery too low can't power up!!!battery_val = %d mV\n", batLevel);
               	pmic_power_down();
            #endif
        }
#endif
    }
#endif // !FPGA
    // timer to monitor the bat level for mmi set 100 ms for the first reporting
//    UOS_SetTimerEX(get_dev_mon_task(), PRV_BAL_BATTERY_TIMER2, UI_TIMER_MODE_SINGLE, MS_TO_TICKS(HAL_TICK1S / 10), "monitor_bat_level_first");
    // Initiate charger status
#ifdef For_Watch_Battery
    PMIC_TRACE("getUSBConnectStatus =%d\n", getUSBConnectStatus());
    if(getUSBConnectStatus()){
        pm_ChargerHandler();
    }
#endif
    // register the charger handler to get charger event faster
    if(Pmic_is_pm813() || Pmic_is_pm802()){
        PMIC_DEBUG_TRACE("!!!!The usb charger callback test\n");
        USB_WAKEUP_CALLBACK_REGISTER(pm_ChargerHandler);
    }else
        pmic_SetChargerStatusHandler(pm_ChargerHandler);
    return TRUE;
}

#define get_usec() (timerCountRead(TS_TIMER_ID)*61/2)  //for debug pmic i2c time

BOOL pm_BatteryMonitorProc(void)
{
//    unsigned int start_us,cost_us;
//    start_us = get_usec();
    if(charger_present)
        ChargerManager();
    pm_doMonitoring(FALSE);
//    cost_us = get_usec() - start_us;
//	raw_uart_log("!!!!Battery monitor cost:%d\n",cost_us);
//    UOS_Sleep(MS_TO_TICKS(Time_To_MMI_Evt));
    return ERR_SUCCESS; 
}

UINT16 PM_GetGpadcValue(PM_GPADC_CHAN_T channel)
{
    return 0;
}

VOID pm_ForceUpdateChargerStatus(VOID)
{
    pm_ChargerHandler();//pm_ChargerHandler(pmic_GetChargerStatus());
}

BOOL pm_GetUsbDownLoadStatus(VOID)
{
    BOOL ret = FALSE;
 //   if(os_AnaGpadcGet(pmic_GetEarpieceGpadcChannel()) < 300)
    {
 //       ret = TRUE;
    }
    return ret;
}

BOOL IsBatteryInCharge(void)
{
    if(Pmic_is_pm813_a3())
        return (charger_present && pm813_battery_current_direction());
    else
        return charger_present;
}

UINT16 GetAverageBatteryLevel(void)
{

    UINT16 uVoltage = pmic_GetGpadcBatteryLevel();
    g_pmBatteryStable.instant = uVoltage;
    if (uVoltage == PMIC_BAD_VALUE)
    {
        PMIC_DEBUG_TRACE("pm:  PMIC_BAD_VALUE");
        return PMIC_BAD_VALUE;
    }
    if (uVoltage < 2000)
    {
        PMIC_DEBUG_TRACE("pm:  PMIC_BAD_VALUE too small\n");
        return PMIC_BAD_VALUE;
    }

    if (g_pmBatteryStable.avg > uVoltage + PM_RF_TX_BATTERY_STEP)
    {
        UINT32 callStatus = 0x00;
#ifdef CMID_MULTI_SIM
        CMID_SIM_ID nSimId;
        for(nSimId = CMID_SIM_0; nSimId < CMID_SIM_COUNT; nSimId++)
        {
            if(Get_call_status)
                callStatus |= Get_call_status(nSimId);
            else
                callStatus |= CMID_CcGetCallStatus(nSimId);
        }
#else
        if(Get_call_status)
            callStatus |= Get_call_status(0);
        else
            callStatus = CMID_CcGetCallStatus();
#endif

        if ((callStatus != 0) && (pmic_BatteryLevelToPercent(uVoltage) > 20))  // not in CC_STATE_NULL
        {
            uVoltage = (UINT16)(uVoltage/8 + (UINT32)g_pmBatteryStable.avg*7/8);
        }
    }
	if (g_pmBatteryStable.nb >= PM_AVERAGE_BATTERY_NB)
	{
		g_pmBatteryStable.sum -= g_pmBatteryStable.levels[g_pmBatteryStable.idx];
        g_pmBatteryStable.sum += uVoltage;
        g_pmBatteryStable.avg = g_pmBatteryStable.sum / PM_AVERAGE_BATTERY_NB;
	}
	else
	{
		g_pmBatteryStable.sum += uVoltage;
		g_pmBatteryStable.nb++;
		g_pmBatteryStable.avg = g_pmBatteryStable.sum / g_pmBatteryStable.nb;
	}
    g_pmBatteryStable.levels[g_pmBatteryStable.idx] = uVoltage;
    g_pmBatteryStable.idx++;
    if (g_pmBatteryStable.idx >= PM_AVERAGE_BATTERY_NB)
    {
        g_pmBatteryStable.idx = 0;
    }
    g_pmBatteryStable.stable = g_pmBatteryStable.avg;
	return g_pmBatteryStable.avg;
}

unsigned char g_LastBatteryRate = 0;

UINT16 ChangeVoltageToBatteryRate(UINT8 * pPercent)
{

	UINT16 uVoltage = 0;
	unsigned char curBatteryRate = 0;
	int i = 0;

	uVoltage = GetAverageBatteryLevel();

    PMIC_DEBUG_TRACE("convert_cur_battery_percent voltage = %d\r\n", uVoltage);

	if(uVoltage >= g_pmicConfig->BatteryDischarger_table[0])
	{
		curBatteryRate = 100;
	}
	else if(uVoltage < g_pmicConfig->BatteryDischarger_table[100])
	{
		curBatteryRate = 0;
	}
	else
	{
		if (charger_present)//³äµç
		{
			for(i = 1; i < (101 - g_LastBatteryRate); i++)
			{
				if(uVoltage < g_pmicConfig->BatteryDischarger_table[i - 1] && uVoltage >= g_pmicConfig->BatteryDischarger_table[i])
				{
					curBatteryRate = 100 - i;

                    PMIC_DEBUG_TRACE("Charging Current_vol = %d, pre = %d\r\n", curBatteryRate, g_LastBatteryRate);
#ifdef For_Watch_Battery
					if ((curBatteryRate > g_LastBatteryRate) && ((curBatteryRate - g_LastBatteryRate) > 15))
					{
                        PMIC_DEBUG_TRACE("Battery dif is bigger than 15, dif: %d, will use new value.\r\n", (curBatteryRate - g_LastBatteryRate));
					}
					else
					{
						if (curBatteryRate < g_LastBatteryRate)
						{
							curBatteryRate = g_LastBatteryRate;
						}
						else if (curBatteryRate > g_LastBatteryRate+1 && g_LastBatteryRate != 0)
						{
							curBatteryRate = g_LastBatteryRate + 1;
						}
					}
#endif
					break;
				}
			}

			if (i == (101 - g_LastBatteryRate))
			{
#ifdef For_Watch_Battery
				curBatteryRate = g_LastBatteryRate;
#else
				for(i = 101 - g_LastBatteryRate; i < 101; i++) //when battery full charger off
				{
					if(uVoltage < g_pmicConfig->BatteryDischarger_table[i - 1] && uVoltage >= g_pmicConfig->BatteryDischarger_table[i])
					{
						curBatteryRate = 100 - i;
                    	PMIC_DEBUG_TRACE("Battery Charging Current_vol = %d, pre = %d\r\n", curBatteryRate, g_LastBatteryRate);
						break;
					}
				}
#endif
			}
		}
		else//·Åµç
		{
			int j = 0;
			if ((g_LastBatteryRate > 0) && (g_LastBatteryRate <= 100))
			{
				j = 100 - g_LastBatteryRate;
			}
			else
			{
				j = 1;
			}

			for(i = j; i < 101; i++)
			{
				if(uVoltage < g_pmicConfig->BatteryDischarger_table[i-1] && uVoltage >= g_pmicConfig->BatteryDischarger_table[i])
				{
					curBatteryRate = 100 - i;

                    PMIC_DEBUG_TRACE("Discharging Current_vol = %d, pre = %d\r\n",curBatteryRate,g_LastBatteryRate);
#ifdef For_Watch_Battery
					if(g_LastBatteryRate > 0)
                    {
					    if (curBatteryRate > g_LastBatteryRate)
					    {
						    if (curBatteryRate - g_LastBatteryRate <= 30)
						    {
							    curBatteryRate = g_LastBatteryRate;
						    }
						    else
                                PMIC_DEBUG_TRACE("Discharging Current_vol = %d error!!!\r\n",curBatteryRate);
					    }
					    else if (curBatteryRate < g_LastBatteryRate - 1)
					    {
						    curBatteryRate = g_LastBatteryRate - 1;
					    }
					}
#endif
					break;
				}
			}
			if (i == 101)
			{
				curBatteryRate = g_LastBatteryRate;
			}
		}
	}
#if 0
    //ui showndown when 0%, about 3.5v
    //lowest 1%
	if(curBatteryRate <= 0)
		curBatteryRate = 1;
#endif

//	raw_uart_log("convert_cur_battery_percent g_curr_vol = %d\r\n", curBatteryRate);
#ifdef For_Watch_Battery
	if (charger_present)//charging
	{
    	if(curBatteryRate> 95)
        {
			curBatteryRate = 95;
        }
    }
#endif
    raw_uart_log("Stable battery level = %d\r\n", curBatteryRate);
	g_LastBatteryRate = curBatteryRate;
	if (pPercent != NULL)
    {
        *pPercent = curBatteryRate;
    }
    g_pmBatteryStable.precent = curBatteryRate;
	return uVoltage;
}
UINT32 charger_get_Time_To_MMI_Evt(void)
{
    return Time_To_MMI_Evt;
}

void send_usb_Message_after_standby(void)
{
	if((send_usb_bootupStandby == 1) && pm812_get_charger_status())
	{
		if(PM813_get_Vbus_ov_uv_status())
		{
            charger_event = PMIC_CHARGE_DC_OFF;
			charger_present = 0;
			g_pmicChargerState.status = PMIC_CHARGER_VOLTAGE_ERROR;
			pm_BattChargeRemind(g_pmicChargerState.status);
        }
		else if(charger_is_usb())
		{
			raw_uart_log("######Charger is usb!\n");
			SendMessageForUSB(DEVUSB_DETECT_ACTION_PLUGIN);
			raw_uart_log("file:%s,function:%s,line:%d\r\n", ui_parse_file_name(__FILE__),__func__,__LINE__);
		}
		else
		{
			SendMessageForUSB(DEVCHARGER_DETECT_ACTION_PLUGIN);//CommonPopupScreen(STRID_COMMON_TEXT_CHARGING);
			raw_uart_log("file:%s,function:%s,line:%d\r\n", ui_parse_file_name(__FILE__),__func__,__LINE__);
		}
		send_usb_bootupStandby = 0;
	}
}

