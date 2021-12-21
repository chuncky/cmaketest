#include "charger_api.h"
#include "pmic_api.h"
#include "plat_basic_api.h"
#include "power_charger.h"
#include "gpio_api.h"
#include "gpio_cus.h"
#include "ui_os_task.h"
#include "task_cnf.h"
#include "ui_log_api.h"
#include "device.h"
#include "charger_cus.h"
#include "gpio_cus.h"

#define TICK1S 32768
#define SECOND        * TICK1S

#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR

#define PMIC_TIME_CHG_MON_INTERVAL (2 SECONDS)
#define PMIC_COUNT_CHG_STATUS_STABLE (10)

#define PMIC_MV_OFFSET_CHG_RECHARGE (200)//(150)
#ifdef For_Watch_Battery
#define PMIC_MV_OFFSET_CHG_PHASE_2 (70)
#else
#define PMIC_MV_OFFSET_CHG_PHASE_2 (70)
#endif
//#define PMIC_MV_OFFSET_CHG_PHASE_3 (10)
#define PMIC_MV_OFFSET_CHG_FULL_COMPENSATION 100//20//(-10)

//#if (PMIC_MV_OFFSET_CHG_FULL_COMPENSATION >= PMIC_MV_OFFSET_CHG_PHASE_3) || \
//    (PMIC_MV_OFFSET_CHG_PHASE_3 >= PMIC_MV_OFFSET_CHG_PHASE_2)
//#error "Incorrect charger voltage offsets"
//#endif

#define PMIC_INTERVAL_CHG_ON_LONG (10 MINUTES)
#define PMIC_INTERVAL_CHG_ON_MEDIUM (6 MINUTES)
#define PMIC_CHG_TIMEOUT (240 MINUTES)
#define PMIC_INTERVAL_CHG_MEAS_STABLE (1 MINUTE)

#define PMIC_COUNT_CHG_CHR_VOLT_STABLE (5)

#define PMIC_MV_OFFSET_CHG_CHR_VOLT (100)

#define Charger_enable 70 //gpio70  need config from evb

typedef enum
{
    PMIC_CHARGING_STAGE_OFF,
    PMIC_CHARGING_STAGE_MEAS_DELAY,
    PMIC_CHARGING_STAGE_MEAS,
    PMIC_CHARGING_STAGE_PULSE,
    PMIC_CHARGING_STAGE_ON,
} PMIC_CHARGING_STAGE_T;

typedef enum
{
    PMIC_CHARGER_VOLTAGE_VALIDITY_UNKNOWN,
    PMIC_CHARGER_VOLTAGE_VALIDITY_VALID,
    PMIC_CHARGER_VOLTAGE_VALIDITY_INVALID,
} PMIC_CHARGER_VOLTAGE_VALIDITY_T;

PROTECTED PMIC_CHARGER_STATE_T g_pmicChargerState;
// Whether the charger voltage monitor is enabled
PRIVATE BOOL g_pmicChargerChrVoltMonEnabled = FALSE;
// Previous charger voltage status
//PRIVATE PMIC_CHARGER_VOLTAGE_VALIDITY_T g_pmicChargerPrevChrVoltValidity = PMIC_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
// Battery voltage array
PRIVATE UINT16 g_pmicBattVoltAry[PMIC_COUNT_CHG_STATUS_STABLE];
// Battery voltage array index
PRIVATE UINT16 g_pmicBattVoltIdx = 0;
//add for charger max cc set
//BOOL cc_current_need_set_flag = FALSE;
//BOOL cc_current_set_finished = FALSE;
//unsigned char cc_current = 0;
unsigned char charger_try_cnt = 0;

// Charger charging stage
PRIVATE PMIC_CHARGING_STAGE_T g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
// Charger charging duration between the measurement operations
PRIVATE UINT32 g_pmicChargerOnInterval = 0;
PRIVATE BOOL g_pmicChargerFirstVoltageCheck = FALSE;
PRIVATE BOOL g_pmicChargerMeasThreshReachedFlag = FALSE;
// The time when the measurement threshold is first reached
//PRIVATE UINT32 g_pmicChargerMeasThreshReachedTime = 0;
PRIVATE UINT32 g_pmicChargerFullDelayTime = 0;
// The start time of the measurement delay
PRIVATE UINT32 g_pmicChargerMeasDelayStartTime = 0;
extern UINT8 charger_present;
extern BOOL Battery_charge_full;
//HANDLE hTask = NULL;
PMIC_CHARGE_EVENT_T charger_event;
PMIC_CONFIG_T* g_pmicConfig = NULL;
PMIC_BATTERY_LEVEL_CFG* g_bat_level_cfg = NULL;
extern unsigned int pm813_charge_full_cnt;

extern U32 GetChargeStatus(void);
//extern void SendMessageForUSB(unsigned int action);
int bat_level_cnt = 0;
// ============================================================================

/* get soc from ocv: lookup table */
static int get_battery_soc_from_ocv(unsigned short ocv,unsigned short *bat_table)
{
	int i;
	int soc = 0;
    if(ocv >= bat_table[0])
	{
		soc = 100;
	}
	else if(ocv < bat_table[100])
	{
		soc = 0;
	}
	else
	{
	    for(i= 1; i < 101; i++)
		{
			if((ocv < bat_table[i - 1]) && (ocv >= bat_table[i]))
			{
				soc = 100 - i;
				break;
			}
		}
    }
    return soc;
}

PUBLIC UINT16 pmic_GetGpadcBatteryLevel(VOID)
{
    if(charger_present && Pmic_is_pm813_a3())
	{
		if(pm813_battery_current_direction())
	    	return (UINT16)pm812_get_batt_vol() + pm813_a3_get_charger_cur_mA() * g_pmicConfig->batteryIR /1000;
		else
			return (UINT16)pm812_get_batt_vol();
    }
	else
	    return (UINT16)pm812_get_batt_vol(); 
}

PUBLIC UINT16 pmic_GetBatteryLevel(UINT8 * pPercent)
{
    g_pmicChargerState.batteryLevel = pmic_GetGpadcBatteryLevel();

    if ((g_pmicChargerState.batteryLevel != PMIC_BAD_VALUE) && (pPercent != NULL))
    {
        *pPercent = pmic_BatteryLevelToPercent(g_pmicChargerState.batteryLevel);
    }
    
    return g_pmicChargerState.batteryLevel;
}

PUBLIC UINT8 pmic_BatteryLevelToPercent(UINT16 batteryLevel)
{
	INT32 percent;
    // build a % value
	if(g_pmicConfig->Has_battery_discharger_table)
       percent = get_battery_soc_from_ocv(batteryLevel,(unsigned short*)&(g_pmicConfig->BatteryDischarger_table));
	else
    	percent = 100 * (batteryLevel - g_pmicConfig->powerDownVoltageMV) / (g_pmicConfig->batteryLevelFullMV - g_pmicConfig->powerDownVoltageMV);
    if (percent <= 0)
    {
        return 0;
    }
    else if (percent >= 100)
    {
        return 100;
    }
    else
    {
        return percent;
    }
}

PUBLIC UINT16 pmic_GetBatteryTemperature(VOID)
{
    return PMIC_BAD_VALUE;
}

PUBLIC PMIC_CHARGER_STATUS_T pmic_GetChargerStatus(VOID)
{
    return g_pmicChargerState.status;
}

PUBLIC VOID pmic_SetChargerStatusHandler(ChgIntCallback handler)//PUBLIC VOID pmic_SetChargerStatusHandler(PMIC_CHARGER_HANDLER_T handler)
{
	//bind callback
	PM812_CHARGER_BIND_INTC(handler);
	//enable charger INTC
	PM812_CHARGER_INTC_ENABLE(TRUE);
}

void PM812_BATTERY_Handler()
{
	if(pm812_get_battert_status()){
		PMIC_TRACE("Battery plug in \n");
		charger_event = PMIC_CHARGE_DC_ON;
	}else{
		PMIC_TRACE("Battery plug out \n");
		charger_event = PMIC_CHARGE_DC_OFF;
	}
}

PUBLIC VOID SetBatteryStatusHandler(void)
{
	//enable battery detection BD_EN
	PM812_ENABLE_BAT_DET(TRUE);
	//bind intc callback
	PM812_BATTERY_BIND_INTC(PM812_BATTERY_Handler);
	//enable battery INTC
	PM812_BATTERY_INTC_ENABLE(TRUE);
}

PROTECTED VOID pmic_SetChargeCurrent(PMIC_CHARGE_CURRENT_T current)
{
//    g_pmicChargerState.current = current;
}

//PRIVATE VOID pmic_TuneCharger(VOID)
//{
//}

VOID pmic_ForceChargerFinished(BOOL stop)
{
    if(Pmic_is_pm812()){
	    gpio_direction_output(Charger_enable);
	    if(stop)	
		    gpio_set_value(Charger_enable,1);
	    else
		    gpio_set_value(Charger_enable,0);
    }else if(Pmic_is_pm802()){
    //need add gpio
    	if(stop)
    		Evb1601_Pm802ChargerEnonoff(0);
		else
			Evb1601_Pm802ChargerEnonoff(1);
	}else{
        Pmic_ChargerFinished(stop);
        PMIC_TRACE("charger: open %x\n",stop);
     }
}

PROTECTED PMU_CHR_STATUS_T pmic_GetChargerHwStatus(VOID)
{
    PMU_CHR_STATUS_T status = CHR_STATUS_UNKNOWN;
	int charger_online;

	charger_online = pm812_get_charger_status();
	if(charger_online) {
		status = CHR_STATUS_AC_ON;
	} else {
		PMIC_TRACE("charger: adapter off");
        status = CHR_STATUS_AC_OFF;
	}
		
    return status;
}

void battery_voltage_rise_up(VBAT_VOLTAGE bat_cv)
{
    if(pmic_GetGpadcBatteryLevel()+PMIC_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmicConfig->batteryLevelFullMV)
    {
         PMIC_TRACE("Battery not need rise volage!:%d\n",pmic_GetGpadcBatteryLevel());
         return;
    }
    switch(bat_cv)
    {
        case VBAT_VOL_4V40:
            pmic_vbat_set(VBAT_VOL_4V55);
        break;
        case VBAT_VOL_4V35:
            pmic_vbat_set(VBAT_VOL_4V55);
        break;
        case VBAT_VOL_4V15:
            pmic_vbat_set(VBAT_VOL_4V35);
        break;
        case VBAT_VOL_4V10:
            pmic_vbat_set(VBAT_VOL_4V30);
        break;
        case VBAT_VOL_4V20:
            pmic_vbat_set(VBAT_VOL_4V40);
        break;
        default:
           PMIC_TRACE("Battery not support volage!:%d\n",bat_cv);
        break;
    }
}

void pmic_set_mppt(void)
{
    pm813_auto_set_max_cc_current(g_pmicConfig->mppt_CurrentUpperLimit);//for mppt
}
extern BOOL pm813_measured_current_means_charging(void);
PROTECTED PMIC_CHARGER_STATUS_T pmic_ChargerChargingState(PMU_CHR_STATUS_T status)
{
    int i;
    PMIC_CHARGER_STATUS_T nextState = g_pmicChargerState.status;
    UINT32 meanBattVolt = 0;
    BOOL needMeas = FALSE;
    UINT32 curTime = timerCountRead(TS_TIMER_ID);
    CHGFSM_OUT charger_fsm = FSM_SHUTDOWN;
    unsigned char var;
//    NingboRead( 0, 0x26, &var );//for debug========================
	if(Pmic_is_pm813())
    	PMIC_TRACE("g_pmicChargingStage =%x,nocharge_vol=%d,direction:%d,current:%d!!!\n",g_pmicChargingStage,pm812_get_batt_vol(),pm813_battery_current_direction(),pm813_a3_get_charger_cur_mA());//debug test
    else
		PMIC_TRACE("g_pmicChargingStage =%x,nocharge_vol=%d!!!\n",g_pmicChargingStage,pm812_get_batt_vol());
#if For_Watch_Battery
    NingboRead( 0, 0x23, &var );
    var = (var >> 4) & 0x0f;
    charger_fsm = pmic_get_charger_fsm();
    if(((charger_fsm != FSM_CV_CHG) && (var <= 1)) || \
        ((charger_fsm == FSM_CV_CHG) && (var < 1))) { // cc: current <=  200mA call mppt, cv: current <  200mA call mppt
        pmic_set_mppt();
        PMIC_TRACE("Mppt called!!!\n");//debug test
    }
#else
    if(Battery_charge_full)
    {
        if(pmic_GetGpadcBatteryLevel() + g_pmicConfig->recharger_vol_mV >= g_pmicConfig->batteryLevelFullMV)
        {
            if((g_pmicChargingStage == PMIC_CHARGING_STAGE_ON) && (timerCountRead(TS_TIMER_ID) - g_pmicChargerFullDelayTime < PMIC_INTERVAL_CHG_ON_LONG))
            {
                g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
                nextState = PMIC_CHARGER_FULL_CHARGE;
                pmic_ForceChargerFinished(TRUE);
                return nextState;
            }
			else if((timerCountRead(TS_TIMER_ID) - g_pmicChargerFullDelayTime) >= PMIC_INTERVAL_CHG_ON_LONG)
				Battery_charge_full = FALSE;
        }
        else
            Battery_charge_full = FALSE;
    }

	if(Pmic_is_pm802())
	{
		if(GetChargeStatus())
		{
            g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
			nextState = PMIC_CHARGER_FULL_CHARGE;
			pm813_charge_full_cnt = 0;
			PMIC_TRACE("ETA4056 FULL status=%d !!!!!!\n",GetChargeStatus());
			g_pmicChargerFullDelayTime = timerCountRead(TS_TIMER_ID);
			Battery_charge_full = TRUE;
		}
	}
#endif
#if 0
    if (g_pmicChargingStage == PMIC_CHARGING_STAGE_MEAS_DELAY)
    {
        if (curTime - g_pmicChargerMeasDelayStartTime >= PMIC_INTERVAL_CHG_MEAS_STABLE)
        {
            g_pmicBattVoltIdx = 0;
            g_pmicChargingStage = PMIC_CHARGING_STAGE_MEAS;
        }
        return nextState;
    }
#endif
    if(g_pmicChargingStage == PMIC_CHARGING_STAGE_MEAS)
    {
        if (g_pmicBattVoltIdx < PMIC_COUNT_CHG_STATUS_STABLE)
        {
            return nextState;
        }
    }
    // Get the mean battery voltage
    for (i=0; i<PMIC_COUNT_CHG_STATUS_STABLE; i++)
    {
        meanBattVolt += g_pmicBattVoltAry[i];
    }
    meanBattVolt /= PMIC_COUNT_CHG_STATUS_STABLE;
    if(Pmic_is_pm813())
        charger_fsm = pmic_get_charger_fsm();
    switch (g_pmicChargingStage)
    {
    case PMIC_CHARGING_STAGE_OFF:
        PMIC_TRACE("charger off stage\n");
        if (meanBattVolt+ g_pmicConfig->recharger_vol_mV >= g_pmicConfig->batteryLevelFullMV)
        {
            break;
        }
#if 0
        if(charger_fsm == FSM_CC_CHG){
            if(Pmic_is_pm813_a1())
                pmic_vbat_set(VBAT_VOL_4V30);//set bat voltage 4.3v
        }
#endif
        g_pmicChargingStage = PMIC_CHARGING_STAGE_PULSE;
        g_pmicChargerMeasThreshReachedFlag = FALSE;
        needMeas = FALSE;
        nextState = PMIC_CHARGER_PULSED_CHARGE;
        if(g_pmicConfig->charger_rise_up_vol)
            battery_voltage_rise_up(g_pmicConfig->battery_cv_Voltage);//rise battery,reduce charger time.
        pmic_ForceChargerFinished(FALSE);
        break;
    case PMIC_CHARGING_STAGE_MEAS:
        // Reset voltage array index
//        g_pmicBattVoltIdx = 0;
        // Check the mean voltage
        if ((meanBattVolt+PMIC_MV_OFFSET_CHG_PHASE_2 >= g_pmicConfig->batteryLevelFullMV) || pm813_get_charger_cv_mode())
        {
            // to make sure the battery is charged full
            if(GetChargeStatus())
            {
            	g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
            // Set next state to full
            	nextState = PMIC_CHARGER_FULL_CHARGE;
                pm813_charge_full_cnt = 0;
				PMIC_TRACE("battery chargeing status=%d !!!!!!\n",GetChargeStatus());
				g_pmicChargerFullDelayTime = timerCountRead(TS_TIMER_ID);
				Battery_charge_full = TRUE;
            }
            else if(Pmic_is_pm813_a3())
            {
                PMIC_TRACE("pmd checkfull!!!,termination:%d\n",pm813_a3_get_charger_cur_mA());
                if(pm813_a3_get_charger_cur_mA() <= g_pmicConfig->battery_termination_cur_mA)
                {
                    g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
            	    nextState = PMIC_CHARGER_FULL_CHARGE;
                    pmic_ForceChargerFinished(TRUE);
//                    NingboRead( 0, 0x26, &var );//for debug========================
				    PMIC_TRACE("battery chargeing full!!!!!!\n");
                    pm813_charge_full_cnt = 0;
                    g_pmicChargerFullDelayTime = timerCountRead(TS_TIMER_ID);
                    Battery_charge_full = TRUE;
                }
            }
        }
#if 0
        else // charge is needed
        {
            PMIC_TRACE("pmd recharging!!!\n");
                // Fast charging
            g_pmicChargerOnInterval = PMIC_INTERVAL_CHG_ON_MEDIUM;
            g_pmicChargingStage = PMIC_CHARGING_STAGE_PULSE;//PMIC_CHARGING_STAGE_ON;
            needMeas = FALSE;
            // Reset the measurement threshold reached flag
            g_pmicChargerMeasThreshReachedFlag = FALSE;
            // Enable charging (h/w thresholds take effect)
            pmic_ForceChargerFinished(FALSE);
            // Set next state to fast but keep full state
 //           if (nextState != PMIC_CHARGER_FULL_CHARGE)
 //           {
            nextState = PMIC_CHARGER_FAST_CHARGE;
 //           }
        }
#endif
        if((charger_fsm == FSM_FAULT) || ((curTime - g_pmicChargerMeasDelayStartTime)>PMIC_CHG_TIMEOUT)){
            nextState = PMIC_CHARGER_TIMED_OUT;
            if(charger_fsm == FSM_FAULT)
                PMIC_TRACE("battery chargeing fault####\n");
            else
            {
                //pmic_ForceChargerFinished(TRUE);
                PMIC_TRACE("battery chargeing timeout!!!!\n");
            }
            g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
        }
        break;
    case PMIC_CHARGING_STAGE_PULSE:
    case PMIC_CHARGING_STAGE_ON:
       if(pmic_GetGpadcBatteryLevel()+PMIC_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmicConfig->batteryLevelFullMV)//if (meanBattVolt+PMIC_MV_OFFSET_CHG_FULL_COMPENSATION >= g_pmicConfig->batteryLevelFullMV)
       {
        	needMeas = TRUE;
       }
//        PMIC_TRACE("battery_volt:%d,no_charge_vbat:%d,vbus:%d,g_pmicChargerFirstVoltageCheck:%d\n",pmic_GetGpadcBatteryLevel(),pm812_get_batt_vol(),pmic_get_vbus_vol(),g_pmicChargerFirstVoltageCheck);
        if(needMeas)
        {
            if (!g_pmicChargerMeasThreshReachedFlag)
            {
//                PMIC_TRACE("battery chargeing test####!!!!\n");
//                 Save the measurement threshold reached time
//                g_pmicChargerMeasThreshReachedTime = curTime;
                if(g_pmicConfig->charger_rise_up_vol)
                    pmic_vbat_set(g_pmicConfig->battery_cv_Voltage); //back to battery termination voltage
//                 NingboRead( 0, 0x26, &var );
//                PMIC_TRACE("[0x26]=0x%x,charge_direction:%d,current:%d!!!\n",var,pm813_measured_current_means_charging(),pm813_a3_get_charger_cur_mA());//debug test
                g_pmicChargerMeasThreshReachedFlag = TRUE;
            }
#if 0
            if (g_pmicChargerFirstVoltageCheck ||
                curTime - g_pmicChargerMeasThreshReachedTime >= g_pmicChargerOnInterval)
            {
                g_pmicBattVoltIdx = 0;
                g_pmicChargerMeasDelayStartTime = curTime;
                //g_pmicChargingStage = PMIC_CHARGING_STAGE_MEAS_DELAY;
                g_pmicChargingStage = PMIC_CHARGING_STAGE_MEAS;
            }
#endif
			g_pmicChargingStage = PMIC_CHARGING_STAGE_MEAS;
        }
//        g_pmicChargerFirstVoltageCheck = FALSE;
        if(charger_fsm == FSM_TERM){
            g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
            nextState = PMIC_CHARGER_FULL_CHARGE;
            PMIC_TRACE("battery chargeing charger_fsm debug!!!!!!\n");
        } else if(charger_fsm == FSM_FAULT){
            nextState = PMIC_CHARGER_TIMED_OUT;
            g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
            PMIC_TRACE("battery chargeing fault!!!!!!,charger_fsm:%x\n",charger_fsm);
        }
        break;
    default:
        break;
    }
    return nextState;
}

#if 0
void dump_charger_register_for_debug(void)
{
    unsigned char var,i;
    for(i=0;i<8;i++)
    {
        NingboRead( 0, 0x20+i, &var );
        PMIC_TRACE("charge register 0x%x:%x\n",0x20+i,var);
    }
}
#endif

int Get_Battery_temp(void)
{
    int bat_temp = 100;
    UINT32 i;
    UINT32 bat_impedance;
    bat_impedance = Pmic_get_battemp_kohm() * 1000;
    for(i = 0;i < 29;i++)
    {
        if(bat_impedance <= g_pmicConfig->battery_temp[i])
        {
            bat_temp = bat_temp - i * 5;
            return bat_temp; //get over temp and near.
        }
    }
    if(i == 29)
    {
        PMIC_TRACE("!!!!The battery no temp pin,or temp over range\n");
        return 0xdeaddead;
    }
    return 0xdeaddead;
}

void ChargerManager(void)
{
	UINT16 curBattVolt;
	PMU_CHR_STATUS_T status;
    PMIC_CHARGER_STATUS_T nextState = g_pmicChargerState.status;
    int batt_temp;
//    PMIC_TRACE("charge manage start!\n");
	switch(charger_event)
	{
		case PMIC_CHARGE_DC_OFF:
			nextState = PMIC_CHARGER_UNPLUGGED;
            g_pmicChargingStage = PMIC_CHARGING_STAGE_OFF;
            pmic_ForceChargerFinished(TRUE);
			PMIC_TRACE("pmd charger off!\n");
			break;
		case PMIC_CHARGE_DC_ON:
//			g_pmicChargerState.startTime = timerCountRead(TS_TIMER_ID);
        	g_pmicBattVoltIdx = 0;
        	g_pmicChargerFirstVoltageCheck = TRUE;
        	g_pmicChargerMeasThreshReachedFlag = FALSE;
//            if(!cc_current_set_finished)
//                cc_current_need_set_flag = TRUE;
        	g_pmicChargingStage = PMIC_CHARGING_STAGE_ON;
 //       	g_pmicChargerPrevChrVoltValidity = PMIC_CHARGER_VOLTAGE_VALIDITY_UNKNOWN;
        	pmic_ForceChargerFinished(FALSE);
			charger_event = PMIC_CHARGE_TIMER;
			if((g_pmicConfig->charger_rise_up_vol) && !Pmic_is_pm802())
			    battery_voltage_rise_up(g_pmicConfig->battery_cv_Voltage);//rise battery,reduce charger time.
			if(!Pmic_is_pm802())
            	pmic_set_mppt();
 //           dump_charger_register_for_debug();//for debug
			break;
		case PMIC_CHARGE_TIMER:
//         PMIC_TRACE("pmd test7\n");
            if (nextState == PMIC_CHARGER_UNKNOWN ||
                nextState == PMIC_CHARGER_UNPLUGGED)
            {
                    // Upper layer thinks there is no charger at this time
                    // Notify upper layer that charger is just plugged in
                nextState = PMIC_CHARGER_PLUGGED;
            }
            else
            {
                    // Upper layer has known that charger is plugged in
                if (nextState == PMIC_CHARGER_PLUGGED)
                {
                    g_pmicBattVoltIdx = 0;
                }
                        // Report charging progress to upper layer
                nextState = pmic_ChargerChargingState(status);
                if (nextState == PMIC_CHARGER_PLUGGED)
                {
                    nextState = PMIC_CHARGER_FAST_CHARGE;
                }
            }
        // Get current battery voltage
        	curBattVolt = pmic_GetGpadcBatteryLevel();

        	if (curBattVolt != PMIC_BAD_VALUE)
        	{
            	g_pmicBattVoltAry[g_pmicBattVoltIdx++%PMIC_COUNT_CHG_STATUS_STABLE] = curBattVolt;
            	if (g_pmicBattVoltIdx == 0) // handle wrap-around issue
            	{
                	g_pmicBattVoltIdx = PMIC_COUNT_CHG_STATUS_STABLE;
            	}
        	}
//			PMIC_TRACE("pmd test9\n");

			break;
		default:
			break;
	}
    PMIC_TRACE("charger: nextState=%d ,oldstatus=%d\n",
                        nextState, g_pmicChargerState.status);
#if 0
    if(Pmic_is_pm813()){
    //for debug
//        PMIC_TRACE("charge debug read!\n");
        NingboRead( 0, 0x5b, &var );
        PMIC_TRACE("charge debug 0x5b:%x,Battery current:%d\n",var >> 5,pm813_a3_get_charger_cur_mA());
        NingboRead( 0, 0xf1, &var );
        PMIC_TRACE("charge debug 0xf1:%x\n",var);
        NingboRead( 0, 0x25, &var );
        PMIC_TRACE("charge debug 0x25:%x\n",var);
    }
#endif    
    if (nextState != g_pmicChargerState.status)
    {
        if(!charger_present)
            nextState = PMIC_CHARGER_UNPLUGGED;
        g_pmicChargerState.status = nextState;
    }

    //add battery temp check
	if(g_pmicConfig->Has_battery_temp)
	{
    	batt_temp = Get_Battery_temp();
    	if(batt_temp != 0xdeaddead)
    	{
        	if(batt_temp < 10)
        	{
        		pmic_cc_current_set(PMIC_CHARGER_200MA);
           		PMIC_TRACE("Battery is cool!\n");
        	}
        	else if((batt_temp > 45) && (batt_temp < 60))
        	{
           		pmic_vbat_set(VBAT_VOL_4V20);
           		PMIC_TRACE("Battery is hot!\n");
        	}
        	else if(batt_temp >= 60)
        	{
            	pmic_ForceChargerFinished(TRUE);
				PMIC_TRACE("Battery too hot,pmd charger off!\n");
        	}
        	PMIC_TRACE("Battery temperature:%d!\n",batt_temp);
    	}
	}
}

extern void ChargerStsEvb1601_Pm802PinmuxCfg(void);
PUBLIC VOID pmic_InitCharger(VOID)
{
	int ret ;
    //unsigned char var;//for debug
#if For_Watch_Battery_K_632832H
	if(pm813_get_batteryId_vol() == Watch_K_632832H_battery_id_vol)
		g_pmicConfig = &g_tgtPmdCfg[1];
	else
		g_pmicConfig = &g_tgtPmdCfg[0];
#else
	g_pmicConfig = &g_tgtPmdCfg[0];
#endif
    g_bat_level_cfg = &bat_level_cfg;
#ifdef For_Watch_Battery
    charger_dump_config();
#endif
	PMIC_TRACE("battery full voltage:%d,min voltage:%d,Battery level4 th=:%d\n", g_pmicConfig->batteryLevelFullMV,g_pmicConfig->batteryLevelPrechargeMV,g_bat_level_cfg->battery_level4_threshold);

    if(g_pmicConfig && Pmic_is_pm813()){
          pmic_precharge_current_set(g_pmicConfig->pre_charge_Current);
          pmic_charge_termination_current_set(g_pmicConfig->termination_charger_Current);
          pmic_cc_current_set(g_pmicConfig->batteryChargeCurrent);
          pmic_vbat_set(g_pmicConfig->battery_cv_Voltage);
          pmic_trickle_timer_set(g_pmicConfig->trickle_ChargeTimeout);
          pmic_precharge_timer_set(g_pmicConfig->pre_Charge_Timeout);
          pmic_cccv_timer_set(g_pmicConfig->cccv_Charge_Timeout);
      
//          PMIC_LONKEY_PRESS_TIME_SET(15);//set longonkey 16s power down
//          PMIC_LONKEY_EN(LONGKEY_1,1);
    }
   // PMIC_TRACE("charger debug\n",var);

	ret = pm812_get_charger_status();
    if (ret)
    {
        // Detect whether battery is in place
//        pmic_ChargerBatteryDetection();
        // Battery is in place. Go on ...
        g_pmicChargerState.status = PMIC_CHARGER_PLUGGED;
		//if(!pm812_get_battert_status())
		charger_event = PMIC_CHARGE_DC_ON;
        charger_present = 1;
#ifndef For_Watch_Battery
#ifndef CONFIG_RELEASE_WITH_NO_ASR_UI
		UOS_ResumeTask((TASK_HANDLE*)get_usb_mon_task());
#endif
#endif
//        PMIC_TRACE("####battery status:%x\n", pm812_get_battert_status());
    }
 	else
    {
        g_pmicChargerState.status = PMIC_CHARGER_UNPLUGGED;//10s
		charger_event = PMIC_CHARGE_DC_OFF;
        charger_present = 0;
    }
	SetBatteryStatusHandler();
	if(Pmic_is_pm802()){
    	ChargerStsEvb1601_Pm802PinmuxCfg();
		pm802_ldo1_active();
	}
	//pmic_wdg_enable();
}

PUBLIC UINT8 pmic_GetPowerOnVolt(VOID)
{
	//g_pmicConfig->powerOnVoltageMV = 3400;
   return pmic_BatteryLevelToPercent(g_pmicConfig->powerOnVoltageMV);
}

PUBLIC UINT8 pmic_GetPowerDownVolt(VOID)
{
	//g_pmicConfig->powerDownVoltageMV = 3400;
   return pmic_BatteryLevelToPercent(g_pmicConfig->powerDownVoltageMV);
}


PUBLIC UINT32 pmic_GetChargerGpadcChannel(VOID)
{
    return 0;//return g_pmicConfig->chargerGpadcChannel;
}

PUBLIC BOOL pmic_ChargerChrVoltMonEnabled(VOID)
{
    return g_pmicChargerChrVoltMonEnabled;
}
void charger_dump_config(void)
{
	UINT8 i = 0;
    PMIC_TRACE("batteryLevelFullMV=%d\n", g_pmicConfig->batteryLevelFullMV);
    PMIC_TRACE("mppt_CurrentUpperLimit=%d\n", g_pmicConfig->mppt_CurrentUpperLimit);
    PMIC_TRACE("battery_cv_Voltage=%d\n", g_pmicConfig->battery_cv_Voltage);
    PMIC_TRACE("termination_charger_Current=%d\n", g_pmicConfig->termination_charger_Current);
    PMIC_TRACE("batteryIR=%d\n", g_pmicConfig->batteryIR);
    PMIC_TRACE("recharger_vol_mV=%d\n", g_pmicConfig->recharger_vol_mV);
    PMIC_TRACE("charger_rise_up_vol=%d\n", g_pmicConfig->charger_rise_up_vol);
    PMIC_TRACE("Has_battery_temp=%d\n", g_pmicConfig->Has_battery_temp);

    if(g_pmicConfig->Has_battery_discharger_table) {
        PMIC_TRACE("BatteryDischarger_table:\n");
        for(i=0; i< 101; i++) {
            PMIC_TRACE("%d, ", g_pmicConfig->BatteryDischarger_table[i]);
        }
        PMIC_TRACE("\n");
    } else {
        PMIC_TRACE("no Has_battery_discharger_table\n");
    }
}
