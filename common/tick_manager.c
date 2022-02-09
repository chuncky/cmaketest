/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*************************************************************************************************************************/
/******************************************************************************
*               MODULE IMPLEMENTATION FILE
*******************************************************************************
* Title: Tavor tick manager file
*
* Filename: tick manager.c
*
* Target, platform: Common Platform, SW platform
*
* Authors: Miriam Yovel
*
* Description:   This pacakge suplies timer ticks services to pre registered applications
*
* Last Updated:
*	Dec 24 06	Yaeli Karni		- make tick manager more genral -to be used on Boerne and other platforms.
*
* Notes:
******************************************************************************/

#include "common.h"
//#include "pm_ext_debug.h"
#include "timer_hw.h"
//#include "utils.h"
//#include "bsp_hisr.h"
//#include "asserts.h"
#include "tick_manager.h"
//#include "bsp.h"
//#include "UART.h"

/*----------- local defines -----------------------------------------*/
#define MAX_TICK_REGISTER		5

#ifdef TICK_MANAGER_TEST
UINT32                  TickManagerCounter = 0;
#endif

#define CPSR_MODE_SVC 0x13


/*
*		Put all global variables in global struct - easier to control  less code
*		produced.
*/

typedef struct {
	TickCallbackPtr		TickRegisterPtr[MAX_TICK_REGISTER];    /* table of registerd calbacks */
	SuspendCallbackPtr	SuspendRegisterPtr[MAX_TICK_REGISTER]; /* table of registerd suspend calbacks */
	PrepareTimeCallbackPtr	PrepareTimesRegisterPtr[MAX_TICK_REGISTER]; /* table of registerd suspend calbacks */
	CLOCK_MODES			OStimerHWmode;				/* OS timer clock mode (Fast/32kHZ) - changes due to
															   HW bug in timers (z0, A0) (B0 - unknonwn yet May 06)
															   Must be set to 13Mhz when normal work - not to blcok
															   other clock reading (read after write blocks the bus,
															   and hte OS timer clock is being set frequently,
															   needs to be set to 32Khz for D2 (power save) */

	CLOCK_MODES			prevOStimerHWmode;				/* OS timer clock mode previous setting */
	TimeIn32KhzUnit TickPeriod;                    /* fixed interval value */
	TimeIn32KhzUnit TickLastOSTick;

	BOOL         TickSusFuncReg ;           /* suspend callback registration flag*/
	BOOL         TickSkipUpdate;            /* whether to skip tick update (configured by suspend)when existing from power save mode*/
	BOOL         TickTimerSetForSuspend;     // Indicates if a HW timer was set for suspend (more than 1 os tick).
	UINT32		deltaTickacc;		// accumulates delta ticks between wakups. When the delta reaches
										// 1 tick - report it too
	// debug info
	UINT32			ilegalTickSuspendReqCounter;
	UINT32			smallTickSuspendReqCounter;

} tickManagerData_S;

tickManagerData_S tickManagerData;



unsigned int scs_mode, accurate_delta;

 /******************************************************************************
 * Function     : TickPhase1Init
 *******************************************************************************
 *
 * Description  : Phase1 Initialization
 *
 * Parameters   : none
 *
 * Output Param : none
 *
 * Return value : none
 *
 * Notes:
 ******************************************************************************/

void TickPhase1Init(void)
{
	INT32	RegIndex;
	
    scs_mode = 1;

    for(RegIndex = 0; RegIndex < MAX_TICK_REGISTER; RegIndex++ )
	{

        tickManagerData.TickRegisterPtr[RegIndex]     = NULL;
		tickManagerData.SuspendRegisterPtr[RegIndex]  = NULL;
    	tickManagerData.PrepareTimesRegisterPtr[RegIndex]  = NULL;
	}


    tickManagerData.TickLastOSTick          = 0;
    tickManagerData.TickSusFuncReg          = FALSE;     /* suspend callback registeration flag*/
    tickManagerData.TickSkipUpdate          = FALSE;      /* whether to skip tick update (configured by suspend)when existing from power save mode*/
    tickManagerData.TickTimerSetForSuspend  = FALSE;
}

void TickhwTimerExpired(UINT8 cookie);     /*Timer expiration callback */


 /******************************************************************************
 * Function     : TickRequestHWclockSettingOnNextActivation
 *******************************************************************************
 * Description  : Sets HWclockMode setting which will be used in next clock
 *					change
 *				Needed due to HW bug : Z0, Z1, A0 ( what will be on B0?)
 *				  Due to bug in HW, the OS timer clock frequency
 *  should be set to 13Mhz on regular work (to avoid block of the bus
 *  due to read after write) while to 32Khz on D2/D4 for power save.
 *  The bug exist in Z0,Z1,A0, If fixed on B0, put all under ifdef ...
 *	So this API allows hte AAM to set the frequency according to the power
 *  mode about to enter...
 *
 * Parameters   : mode of clock (supported only Fast, 32Khz)
 *
 * Output Param : none
 *
 * Return value : none
 *
 *  NOTES :
 ******************************************************************************/
void TickRequestHWclockSettingOnNextActivation(CLOCK_MODES mode)
{
	{
		tickManagerData.OStimerHWmode = mode;
	}
	return;
}



 /******************************************************************************
 * Function     : TickUpdateDeltaTicks
 *******************************************************************************
 *
 * Description  : Update the time in ticks that passed from last suspend request to Resync request (after wakw up)
 * in order to notify to Tick Manager clients. Non-full tick clocks are accumulated. WIll
 * be reported when all fractions are more then one tick value. Update tickManagerData.TickLastOSTick too
 *
 * Parameters   : currentAbsTime - the currenr Absoulte time
 *
 * Output Param : none
 *
 * Return value : Delta Time In ticks
 * Notes:
 ******************************************************************************/

 static UINT32  TickUpdateDeltaTicks (TimeIn32KhzUnit  currentAbsTime )
 {

     UINT32          resultInOSTicks, deltaTicks;
     TimeIn32KhzUnit resultIn32KHzTicks;

     // Finding the difference between the times in 32KHz units.
     //  (wraparound done by compiler since both are UINT32)
     resultIn32KHzTicks = ( currentAbsTime - tickManagerData.TickLastOSTick  );

 	 // now lets update the last update time, to be now!
	 tickManagerData.TickLastOSTick = currentAbsTime;

     // Finding the difference between the times in OS ticks.
	 // we report all full ticks that already passed, and
	 // accumulate the differences. When this reaches a tick value
	 // it is reported too.

	 resultInOSTicks = resultIn32KHzTicks / tickManagerData.TickPeriod;	 //we always round down
	 tickManagerData.deltaTickacc += (resultIn32KHzTicks % tickManagerData.TickPeriod);       //and acumulate the fractions
	 if ( (tickManagerData.deltaTickacc / tickManagerData.TickPeriod) == 1)        			 //until they add uo to more then one tick
	 {                                                                   //then we simply count an extra tick
		// The accumulation has a full tick in it, lets add it
	 	resultInOSTicks++;
		// now, reduce this tick value from the accumulator
		tickManagerData.deltaTickacc -= tickManagerData.TickPeriod;
	 }

	if(scs_mode)
	{
		// 26M/793 = 32787
		// 5ms = 32787/200 = 163.935
		// 164 - 163.935 = 0.065
		accurate_delta += 65 * resultInOSTicks;
	}
	else
	{
		// 5ms = 32768/200 = 163.84
		// 164 - 163.84 = 0.16
		accurate_delta += 160 * resultInOSTicks;
	}

	if(accurate_delta >= 1000)
	{
		tickManagerData.deltaTickacc += accurate_delta / 1000;
		accurate_delta = accurate_delta % 1000;
	}
	
	deltaTicks = tickManagerData.deltaTickacc / tickManagerData.TickPeriod;
	if(deltaTicks >= 1) 
	{
		resultInOSTicks += deltaTicks;
		tickManagerData.deltaTickacc %= tickManagerData.TickPeriod;		
		accurate_delta += ((scs_mode ? 65 : 160) * deltaTicks);
	}

	return resultInOSTicks;
 }



 /******************************************************************************
 * Function     : TickHWtimerSet
 *******************************************************************************
 *
 * Description  : Configures and sets HW Timer 3 for Tick Manager usage
 *
 * Parameters   : time interval for kick off  timer 3 in 32 KHZ Units
 *
 * Output Param : none
 *
 * Return value : none
 *
 * Notes:
 ******************************************************************************/

static void TickHWtimerSet(TimeIn32KhzUnit TimeInterval)
{
    TIMER_RETURN_CODE  timerStatus;
	TIMER_TCR_CONFIGURE_RC timerstat2;
	TCR_CONFIGURATION       timer_config;
	UINT32					uSec;

	// deactivate OS_TIMER_ID
	timerStatus = timerDeActivate((TCR_NUMBER)OS_TIMER_ID);

    // Raviv debug 21/2/6
    //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TIMER_DEACTIVATE);


    if (timerStatus != TIMER_RC_OK)
    {
        ASSERT(FALSE);
    }

    // configure OS_TIMER_ID
    timer_config.clock_mode  = tickManagerData.OStimerHWmode; // set by caller application (AAM) was: CLK_32KHZ;/*Raviv debug 13M 21/2/6*/ //CLK_FAST;        // 32.768Khz ==> 1 hw tick clock = 30517.578125 nSec = 30.517 Usec
	// lets make sure we remember last HW timer-mode setting (must be considered on changes)
	tickManagerData.prevOStimerHWmode = tickManagerData.OStimerHWmode;
    //timer_config.tcmr_mode   = C_TCMR_HW_PERIODIC_TCMR_MODE; //  Configured to periodic mode
    //timer_config.tplcr_mode  = C_TPLCR_FREE_RUNNING ;
    timer_config.tcmr_mode   = C_TCMR_HW_PERIODIC_TCMR_MODE; //  Configured to periodic mode
    timer_config.tplcr_mode  = C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_O ;

    timer_config.tplvr_value = 0;

	/*
	 * In order to get longer time, we use 64 bit calculations.
	 *
	 * Also, we must test against an overflow in the number of uSec, the limit is 0xFFFFFFFF.
	 * Ovfl = 0x100000000 / 1000000 * 32768 = 140737488.355328
	 *	==> (140737488 * 1000000) / 32768 = 0xFFFFFFF5
	 *  ==> (140737489 * 1000000) / 32768 = 0x100000013 ==> Overflow
	 */

	uSec = TimeInterval;

	timer_config.microSecTimeInterval = uSec;
    timer_config.cookie = OS_TIMER_ID;
    timer_config.timerNotificationPtr = TickhwTimerExpired; //Expiration callback
    timer_config.interrupt_required = TO_INTERRUPT_REQUIRED|TO_LISR_NOTIFICATION;  // called from LISR Context
    timerstat2 = timerTCRconfigure((TCR_NUMBER)OS_TIMER_ID, &timer_config);
    ASSERT(timerstat2 == TCR_CONFIGURE_OK);

    timerStatus = timerActivate(OS_TIMER_ID);

    // Raviv debug 21/2/6
    //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TIMER_ACTIVATE);

    if (timerStatus != TIMER_RC_OK)
    {
        ASSERT(FALSE);
    }

}

/*****************************************************************************
 * The procedure    TickHWtimerChange()  is similar to the TickHWtimerSet()
 * The only changes are:
 * - No timerDeactivate() / timerActivate() used
 * - The timerTCRReConfigure() is used instead of the timerTCRconfigure()
 * As result the "change" is working much more faster then the "set"
 * NOTE:
 *   The timerTCRReConfigure(0 is implemented for HARBELL only.
 *   So this procedure is also wrapped with HARBEL flag.
 ***************************************************************************/

static void TickHWtimerChange(TimeIn32KhzUnit TimeInterval)
{
	TIMER_TCR_CONFIGURE_RC timerstat2;
	TCR_CONFIGURATION       timer_config;
	UINT32					uSec;

    // configure OS_TIMER_ID
    timer_config.clock_mode  = tickManagerData.OStimerHWmode; // set by caller application (AAM) was: CLK_32KHZ;/*Raviv debug 13M 21/2/6*/ //CLK_FAST;        // 32.768Khz ==> 1 hw tick clock = 30517.578125 nSec = 30.517 Usec
	// lets make sure we remember last HW timer-mode setting (must be considered on changes)
	tickManagerData.prevOStimerHWmode = tickManagerData.OStimerHWmode;
//    timer_config.tcmr_mode   = C_TCMR_HW_PERIODIC_TCMR_MODE; //  Configured to periodic mode
//    timer_config.tplcr_mode  = C_TPLCR_FREE_RUNNING ;
    timer_config.tcmr_mode   = C_TCMR_HW_PERIODIC_TCMR_MODE; //  Configured to periodic mode
    timer_config.tplcr_mode  = C_TPLCR_ENABLE_PRELOAD_WITH_MATCH_O ;

    timer_config.tplvr_value = 0;

	/*
	 * In order to get longer time, we use 64 bit calculations.
	 *
	 * Also, we must test against an overflow in the number of uSec, the limit is 0xFFFFFFFF.
	 * Ovfl = 0x100000000 / 1000000 * 32768 = 140737488.355328
	 *	==> (140737488 * 1000000) / 32768 = 0xFFFFFFF5
	 *  ==> (140737489 * 1000000) / 32768 = 0x100000013 ==> Overflow
	 */
	uSec = TimeInterval;

	timer_config.microSecTimeInterval = uSec;
    timer_config.cookie = OS_TIMER_ID;
    timer_config.timerNotificationPtr = TickhwTimerExpired; //Expiration callback
    timer_config.interrupt_required = TO_INTERRUPT_REQUIRED|TO_LISR_NOTIFICATION;  // called from LISR Context
    timerstat2 = timerTCRReConfigure((TCR_NUMBER)OS_TIMER_ID, &timer_config);
	ASSERT(timerstat2 == TCR_CONFIGURE_OK);
}

 /******************************************************************************
 * Function     : TickConfigAndActive
 *******************************************************************************
 *
 * Description  : Configures and activates Timer 3 for a specific time interval
 *
 * Parameters   : time interval for kick off  timer 0 in 32 KHZ Units
 *
 * Output Param : none
 *
 * Return value : none
 *
 * Notes:
 ******************************************************************************/
static VOID TickConfigAndActive( TimeIn32KhzUnit TimeInterval )
{

	tickManagerData.TickPeriod = TimeInterval;


    TickHWtimerSet( TimeInterval);  /*configure OS_TIMER_ID to start periodically ticks*/

}

static TimeIn32KhzUnit tickLast5SecOSTick = 0;

TimeIn32KhzUnit TickTrigger(void)
{

	INT8 RegIndex;

    UINT32          cpsrReg;
    UINT32          deltaTick;
    TimeIn32KhzUnit currentTime = timerCountRead(TS_TIMER_ID);
	UINT32 maxTime=0,time1,timediff, callerAddr;
    cpsrReg = disableInterrupts(); //lock interrupts

	if((currentTime - tickLast5SecOSTick)>= 1*33)
	{
		//SetCpuUsageDumpFlag();
		tickLast5SecOSTick = currentTime;
	}
	
    /*
     *  An OS tick can occur either in this function or in the TickSyncAfterSuspend API.
     *  whenever an OS tick occurs the TickLastOSTick is updated. If enough time
     *  has passed since the last OS tick another OS tick is generated.
     */
    deltaTick = TickUpdateDeltaTicks (currentTime );
	
    /*
     *  Inform the OS clients only if more than zero OS tick has passed (>=1),
     *  and there is a need to generate a new OS tick.
     *
     */
	if (deltaTick > 0)
	{
	    for(RegIndex = 0; RegIndex < MAX_TICK_REGISTER; RegIndex++)
	    { // Notify the OS clients that deltaTick OS ticks have passed.
		        if( tickManagerData.TickRegisterPtr[RegIndex] != NULL ){
					time1 = timerCountRead(TS_TIMER_ID);
		 	        tickManagerData.TickRegisterPtr[RegIndex]( deltaTick ); //should be 1 in case of regular tick.
					timediff = timerCountRead(TS_TIMER_ID)-time1;
					if(timediff>maxTime)
					{
						maxTime = timediff;
						callerAddr = (UINT32)tickManagerData.TickRegisterPtr[RegIndex];
					}
		        }
		}
	}
	if (tickManagerData.TickTimerSetForSuspend)
	{
		// Returning to normal operation mode, kick off OS Timer for starting ticks again
        tickManagerData.TickTimerSetForSuspend = FALSE;
        TickHWtimerChange(tickManagerData.TickPeriod);
	}
	
	restoreInterrupts(cpsrReg); //unlock interrupts

	return currentTime;
}

 /******************************************************************************
 * Function     : TickhwTimerExpired
 *******************************************************************************
 *
 * Description  : the activation function of Timer 3 LIsr : calls to Tick Trigger
 *
 * Parameters   : cookie - for any usage
 *
 * Output Param : none
 *
 * Return value : none
 *
 * Notes:
 ******************************************************************************/
void TickhwTimerExpired(UINT8 coockie)
{
		//uart_printf("%s-01\r\n",__func__);
		TickTrigger();
}



 /******************************************************************************
 * Function     : TickPhase2Init
 *******************************************************************************
 *
 * Description  : Phase1 Initialization
 *
 * Parameters   : none
 *
 * Output Param : none
 *
 * Return value : none
 *
 * Notes:
 ******************************************************************************/
void TickPhase2Init(void)
{
	/* Configure  Timer for Tick Manager interrupts*/
	UINT32 tick_value= TICK_INTERVAL_32KHZUNIT;		// different value for _QT_ !!

	// lest set the Timer freq (Fast_clk, 32Khz, .. ) according to platform (Harbell -> fast, Boerne -> 32Khz)
	// Currently, only on Harbell special setting of Clk-fast (13Mhz)
	//#if defined ( _TAVOR_HARBELL_) || defined(SILICON_PV2)


    uart_printf("use CLK_FAST for crane evb !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    TickRequestHWclockSettingOnNextActivation(CLK_FAST); //PHS_DEMO_FIX_TIMER_BUG_20080826



	// for QT - the run is much slower! Thus:
	// In HRBL, the Free-running clock is set to FAST clcok and the time passed seems ok
	// In BRN we will set hte tick interval to 2 instead of 163
	// (while ration seems to be 1:500 - it will be slower, but not 500 times, but ~3 times)
	TickConfigAndActive( tick_value );

}



UINT32 NutickCompensateMaxTick = 0;
UINT32 NutickCompensateMaxCost= 0;
UINT32 NutickTotalCompensateTicks= 0;
UINT32 NutickTotalCompensateCnts= 0;

#define NU_TICK_DELAY_CONTEXT_ARR_LEN	32
UINT8 NutickDelayContextCnt=0;

UINT32 NutickDelayContextInfo[NU_TICK_DELAY_CONTEXT_ARR_LEN][4];

UINT32 gNuTickDelayOwner;


/******************************************************************************
* Function: NUMaximumSuspend
*******************************************************************************
* Description: registered callback to check maximum allowed time to aleep
*
* Parameters:  None.
*
* Return value:  maximum time to sleep in 32KHZUnit.
*
* Notes:
******************************************************************************/
//To be placed into "tick_manager.h" and also used in the "tick_manager.c"
#define MAX_MAX_SLEEP_CLOCK32K    (((MAX_TimeIn32KhzUnit/2) / TICK_INTERVAL_32KHZUNIT) * TICK_INTERVAL_32KHZUNIT)

TimeIn32KhzUnit NUMaximumSuspend( void )
{
    UINT32 maxSleepOsTick;
	uart_printf("%s-01\r\n",__func__);
    maxSleepOsTick = 0xff;

    if(maxSleepOsTick > (MAX_MAX_SLEEP_CLOCK32K/TICK_INTERVAL_32KHZUNIT))
        return(MAX_MAX_SLEEP_CLOCK32K);

    return(maxSleepOsTick*TICK_INTERVAL_32KHZUNIT);
}

/*----------- EXTERNAL Defines  -----------------------------------------*/

 /******************************************************************************
 * Function     : TickSubmit
 *******************************************************************************
 *
 * Description  : Registration to callback  functions
 *
 * Parameters   : pointer to callback function
 *
 * Output Param : none
 *
 * Return value : the current time in 32 KHZ Units
 *
 * Notes:
 ******************************************************************************/
TimeIn32KhzUnit TickSubmit( TickCallbackPtr callback_ptr, SuspendCallbackPtr suspend_callback_ptr ,PrepareTimeCallbackPtr prepareTime_callback_ptr)

{
	INT8 RegIndex;


    if (suspend_callback_ptr !=NULL )
		tickManagerData.TickSusFuncReg = TRUE;                  /*indicate suspend callbak registered for  Fatal error checking*/


	for(RegIndex = 0; RegIndex < MAX_TICK_REGISTER; RegIndex++)
        if( tickManagerData.TickRegisterPtr[RegIndex] == NULL )                      /*update callback in table if empty and break from loop*/
        {
            tickManagerData.TickRegisterPtr[RegIndex] 	 = callback_ptr;
			tickManagerData.SuspendRegisterPtr[RegIndex] = suspend_callback_ptr;
			tickManagerData.PrepareTimesRegisterPtr[RegIndex] = prepareTime_callback_ptr; /* table of registerd suspend calbacks */
			break;
        }

	ASSERT( RegIndex < MAX_TICK_REGISTER );                 /* no available place in callback arry*/
	return (timerCountRead(TS_TIMER_ID));
}











void  NUTickUpdate( void )
{
	UINT32          cpsrReg;
	cpsrReg = disableInterrupts(); //lock interrupts
    //OsaTick(NULL);
	uart_printf("%s-01\r\n",__func__);
	restoreInterrupts(cpsrReg); //unlock interrupts
	//dumpNuTickDelayInfo();
}

/******************************************************************************
* Function: NUTickUpdateAfterSuspend
*******************************************************************************
* Description:  NUcleus Adaptation layer that updates Nu Ticks to manage Nucleus sw Timers
*
* Parameters:  None.
*
* Return value:  None.
*
* Notes:
******************************************************************************/
void NUTickUpdateAfterSuspend(UINT32 NUTickDeltaTicks)
{
	UINT32          cpsrReg;
	UINT32 			moreticks=0, i;

	cpsrReg = disableInterrupts(); //lock interrupts
	//OsaTickUpdate(NUTickDeltaTicks);
	uart_printf("%s-01,%d\r\n",__func__,NUTickDeltaTicks);

	restoreInterrupts(cpsrReg); //unlock interrupts

}

 /******************************************************************************
 * Function     : TickManagerIsSuspendContext
 *******************************************************************************
 *
 * Description  : returns if tick manager is in suspend context (needed by NUtick - for Nucleues)
 *
 * Parameters   : none
 *
 * Output Param : none
 *
 * Return value : True - suspend context, False - non-suspend context
 *
 * Notes:
 ******************************************************************************/
BOOL TickManagerIsSuspendContext(void)
{
	return tickManagerData.TickTimerSetForSuspend;
}


/******************************************************************************
* Function: NUTickLisr
*******************************************************************************
* Description:  Tick Manager registration callback of Nucleus
*
* Parameters:  deltaInTicks - ticks that passed and need to be updated to NU AL
*
* Return value:  None.
*
* Notes:
******************************************************************************/
static UINT32 NutickFirstDelay = 0;
UINT32 getNUTikcFirstDelay(void)
{
	return NutickFirstDelay;
}
void NUTickLisr(UINT32 deltaInTicks)
{
	   STATUS  osStatus;
	   UINT32 time_1, time_2, time_interval;
	   //uart_printf("%s-01\r\n",__func__);
       if(  TickManagerIsSuspendContext() )
       {/* Raviv debug 13/2/6 add XDA event */
	   		uart_printf("%s-02\r\n",__func__);
	   	   NUTickUpdateAfterSuspend(deltaInTicks);
           //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TM_NU_TICK,PM_EXT_DBG_EVENT_NO_DATA);            
       }
	   else
	   {
	   	   if(deltaInTicks > 1)
	   	   {
	   	   		if(NutickFirstDelay == 0)
	   	   		{
	   	   			NutickFirstDelay = deltaInTicks;
	   	   		}
				time_1 = timerCountRead(TS_TIMER_ID);
				NUTickUpdateAfterSuspend(deltaInTicks);
				time_2 = timerCountRead(TS_TIMER_ID);
				time_interval = time_2 - time_1;
				NutickTotalCompensateTicks += (deltaInTicks-1);
				NutickTotalCompensateCnts++;
				
				if((NutickDelayContextCnt < NU_TICK_DELAY_CONTEXT_ARR_LEN) &&(deltaInTicks > 2))
				{
					NutickDelayContextInfo[NutickDelayContextCnt][0] = gNuTickDelayOwner;
					NutickDelayContextInfo[NutickDelayContextCnt][1] = deltaInTicks;
					NutickDelayContextInfo[NutickDelayContextCnt][2] = time_1;
					NutickDelayContextInfo[NutickDelayContextCnt][3] = ReadMode_R14(CPSR_MODE_SVC);
					NutickDelayContextCnt++;
				}

				
				if(time_interval > NutickCompensateMaxCost)
				{
					NutickCompensateMaxCost = time_interval;
				}

				if(NutickCompensateMaxTick < deltaInTicks)
				{
					NutickCompensateMaxTick = deltaInTicks;
				}

	   	   	}
			else
			{
				//osStatus = OS_Activate_HISR(&_NUTickUpdate);   /*activates HISR to handle NU Update */
				//ASSERT(osStatus == OS_SUCCESS);
				NUTickUpdate();
			}
           //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_ACTIVATE_NU_HISR,PM_EXT_DBG_EVENT_NO_DATA);
	   }

}  /*timerActivateOSHisr*/


void NUTickRegister(void)
{
    // 26/2/6 Raviv - Changed priority from "1" to "0"
	//OS_Create_HISR(&_NUTickUpdate, "NUTick", NUTickUpdate, TM_OS_HISR_PRIORITY );       /* Creates  HISR to handle NU Update */
	TickSubmit( NUTickLisr ,NUMaximumSuspend, NULL );
}







#if 0

/*----------- local functions  -----------------------------------------*/






/******************************************************************************
 * Function     : TickHandleBeforeDisableInt
 *******************************************************************************
 *
 * Description  : Handle AAM request before Disable Interrupt within LPT
 *
 * Parameters   : none
 *
 * Output Param : none
 *
 * Return value : none
 *
 * Notes:
 ******************************************************************************/
void TickHandleBeforeDisableInt (void )
{

  INT32 RegIndex;


  for(RegIndex = 0; RegIndex < MAX_TICK_REGISTER; RegIndex++) /*execute  pre-registered callbacks*/
  {   // Update the OS clients that deltaTick OS ticks has passed
      if( tickManagerData.PrepareTimesRegisterPtr[RegIndex] != NULL )
  	    tickManagerData.PrepareTimesRegisterPtr[RegIndex]( );
  }

}








 /******************************************************************************
 * Function     : TickTrigger
 *******************************************************************************
 *
 * Description  : Activates the registered callback functions .This routine is called from LISR context periodically
 * when called from any other task modoule it also kick off Timer3 for next period
 *
 * Parameters   : none
 *
 * Output Param : none
 *
 * Return value : the current time in 32 KHZ Units - if called from external task
 *                0 - if called from timer expiration
 * Notes:
 ******************************************************************************/
extern UINT32 commpmdebuglog[];

extern 	void SetCpuUsageDumpFlag(void);
extern 	UINT32 getCpuDumpInterval(void);


 /******************************************************************************
 * Function     : TickSuspend
 *******************************************************************************
 *
 * Description  : Suspend tick trigger foe specific time interval
 *
 * Parameters   : the time to be suspend in 32KHZUnit
 *
 * Output Param : none
 *
 * Return value : Suspend time in 32K units
 *
 * Notes:
 ******************************************************************************/
TimeIn32KhzUnit TickSuspend(TimeIn32KhzUnit Suspendtime )
{
	UINT32          cpsrReg;

    cpsrReg = disableInterrupts(); //lock interrupts


	/* Configure Timer  OS_TIMER_ID to expire after suspend time ,if suspend time is inside the Tick Period interval  so no suspension time
	 is required */

	/*if user asked to awake N ticks before */

 #ifdef TICKS_TO_WAKE_BEFORE
		if ( Suspendtime > (NUM_OF_TICKS_TO_WAKE_BEFORE +1)*(tickManagerData.TickPeriod) )
		{
			Suspendtime = Suspendtime - (tickManagerData.TickPeriod*NUM_OF_TICKS_TO_WAKE_BEFORE);
		}
		else
			Suspendtime = tickManagerData.TickPeriod;

 #endif


	/*-----------------12/17/2007 4:49PM----------------
	 * after we will wake from suspend we could be in a situation that we will miss a tick becuase of the difference between
	 * the sleep entry time and the last tick trigger time plus the accumulated delta, this will cuase us to sleep x ticks, but
	 * to update the x+1 ticks have passed, this could cause and overshoot issue (waking after timer should have expired)
	 * to solve this (at least partially) we will subtract the accumulated slow clocks value from the suspend time.
	 * we will not reach a value lesser then zero becuae we will not enter suspend unless we will suspend for more then 1 tick.
	 * --------------------------------------------------*/
	Suspendtime -= tickManagerData.deltaTickacc;
   if (Suspendtime < 5)
	   	Suspendtime = 5;	// at least 5 32Khz-ticks
//#if defined (_TAVOR_HARBELL_) || defined(SILICON_PV2)
#if defined(SILICON_PV2)
        TickHWtimerChange(Suspendtime);
#else
      	TickHWtimerSet(Suspendtime);
#endif
        // Raviv debug 14/2/6 TickLastAbsSuspendTimer = currentTime ;  /*for synchronize trigger after sleep*/
        tickManagerData.TickTimerSetForSuspend = TRUE;
        //PM_EXT_DBG_EVENT_SEND(PM_EXT_DBG_EVENT_TM_TICK_SUSPENDED);

	restoreInterrupts(cpsrReg); //unlock interrupts

	return (Suspendtime );
}



 /******************************************************************************
 * Function     : TickGetNearestEventTime
 *******************************************************************************
 * Description  : Enables the retrieving of he nearest OS event time when system is
 *                not going to sleep
 * Parameters   : none
 *
 * Output Param : none
 *
 * Return value : Updated absoulute time to nearest event
 *
 *  NOTES :
 ******************************************************************************/
extern unsigned long pdi_nearestOsEvent_func;
extern BOOL PMDebugEnable(void);

#if defined(PLAT_USE_THREADX)
static TimeIn32KhzUnit tick_nearest_event_time_;

TimeIn32KhzUnit TickGetNearestEventTime_(void)
{
	return tick_nearest_event_time_;
}
#endif

TimeIn32KhzUnit TickGetNearestEventTime(void)
{

	INT8 			RegIndex;
	/*
		tempTime is the max time for setting Timer, Originally it was only MAX_TimeIn32KhzUnit/2
		but when the system did not have any timer request, all clients returned this value, and
		then it was translated to abs-time, but when it returned to the tickmanager to configure
		the timer, the value managed to be in the past, since some 32Khz clocks have past.
		One needs to go over all logic in tick-manger and aam and the translation back and worth
		of absolute time vs. current time and the D2-entry-exit time to understand how when no timer is
		defined in the system we keep waking up any regular tick-perido (currently 5 ms),
		By setting the Max to compare to value not MAX32Khz/2 but a bit less (which is still a lot of time ahead)
		this problem is solved and the system goes to sleep till someone else (DRX need, APPs via MSL, UART ...)
		wakes it up.
	*/
	TimeIn32KhzUnit tempTime = MAX_TimeIn32KhzUnit/2 - 3*tickManagerData.TickPeriod;
#if defined(PLAT_USE_THREADX)	
	TimeIn32KhzUnit tempTime_ = tempTime;
#endif
#if 0
	if(PMDebugEnable() == TRUE)
		pdi_nearestOsEvent_func = NULL;
#endif

	/*if called to this service so suspend callback is mandatory*/
    if (FALSE == tickManagerData.TickSusFuncReg)
    {
        ASSERT( FALSE );
    }

	/*going over registered suspend callbacks and find the minimum time to nearest event */
	/* check with subscriber suspend callback for how long it is possible for subscriber to sleep and pick the lowest between the
	 maximun time set by the suspend and the time allowed to sleep   */


	for(RegIndex = 0; RegIndex < MAX_TICK_REGISTER; RegIndex++)
	{

		if( tickManagerData.SuspendRegisterPtr[RegIndex] != NULL )
		{
			commpmdebuglog[31] = (UINT32)tickManagerData.SuspendRegisterPtr[RegIndex];
			commpmdebuglog[6] = timerCountRead(TS_TIMER_ID);
			TimeIn32KhzUnit temp = tickManagerData.SuspendRegisterPtr[RegIndex]();
#if defined(PLAT_USE_THREADX)
			extern TimeIn32KhzUnit NUMaximumSuspend( void );
			extern TimeIn32KhzUnit NUMaximumSuspend_( void );
			TimeIn32KhzUnit temp_;
			if(tickManagerData.SuspendRegisterPtr[RegIndex] == NUMaximumSuspend)
				temp_ = NUMaximumSuspend_();
			else
				temp_ = temp;
#endif			
			commpmdebuglog[31] = 0xFF;
			if (temp < tempTime )
            {
                tempTime = temp;
#if 0				
				if(PMDebugEnable() == TRUE)
					pdi_nearestOsEvent_func = (unsigned long)tickManagerData.SuspendRegisterPtr[RegIndex];
#endif				
				extern void rti_record_nearest_event(unsigned long func, unsigned long time);
				rti_record_nearest_event((unsigned long)tickManagerData.SuspendRegisterPtr[RegIndex], temp);
            }

#if defined(PLAT_USE_THREADX)
			if (temp_ < tempTime_ ) tempTime_ = temp_;
#endif	
		}
	}
	commpmdebuglog[31] = 0x80;

#if defined(PLAT_USE_THREADX)
	tick_nearest_event_time_ = tempTime_;
#endif
	/*return nearest event time*/
	return tempTime;


}





#endif

