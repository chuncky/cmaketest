/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: Tick Manager
*
* Filename: TickManager.h
*
* Target, platform: Common Platform, SW platform
*
* Authors:  Miriam Yovel
 *
* Description:
*
* Last Updated:
*
* Notes:
*******************************************************************************/
#ifndef _TICK_MANAGER_H_
#define _TICK_MANAGER_H_

#include "common.h"
#include "timer_hw.h"

/********** need to be removed *****************************************/
typedef unsigned long long  UINT64;       /* Unsigned 64-bit quantity       */


#define UNIT_MICRO_SEC                             1000000
#define UNIT_32KHZ_CLOCK 						   32768

#define UNIT_13MHZ_CLOCK                        (13000000)
#define USECS_IN_SEC                            (1000000) /* number of uSecs in one second*/
#define MAX_TimeIn32KhzUnit                    (0xffffffffL) //the max value of hw timer

#define MAX_TimeIn13MhzUnit                    (0xffffffffL) //the max value of hw timer


#define TICK_INTERVAL_32KHZUNIT                 (164*100)     /* 163->164, (5*32768/1000) -->5 msec in 32 KHZ Unit */


#define MAX_CYCLES_1024_TICKS                  (TICK_INTERVAL_32KHZUNIT *1024)    /*    (1024*5/100)*32768   */

#define MAX_MICRO_SEC_TIME_32K_CLK            140737489 /* 0xffffffffL *32768/1000000 */

#define MAX_MICRO_SEC_TIME_13M_CLK            (USECS_IN_SEC * (MAX_TimeIn13MhzUnit/UNIT_13MHZ_CLOCK)) /* (max num of ticks) * (length of tick in uSecs) */

#define  DELTA_TICKS_UPDATE                    1


#define  NUM_OF_TICKS_TO_WAKE_BEFORE           0

//Raviv debug 12/2/6
#define TM_USECS_IN_32KHZ_TICK                 30.517578125 /*calculated*/
#define TM_32KHZ_TICKS_TO_USECS(TICKSiN32khZ)  (TICKSiN32khZ*TM_USECS_IN_32KHZ_TICK) /*calculated in the preprocessing phase*/

#define OS_TIMER_ID  TCR_1

typedef unsigned long   TimeIn32KhzUnit;


typedef void (*TickCallbackPtr)(UINT32);
typedef TimeIn32KhzUnit (*SuspendCallbackPtr)(void);
typedef void (*PrepareTimeCallbackPtr)(void);


TimeIn32KhzUnit TickSubmit( TickCallbackPtr callback_ptr, SuspendCallbackPtr suspend_callback_ptr ,PrepareTimeCallbackPtr prepareTime_callback_ptr);
void TickPhase1Init(void);
void TickPhase2Init(void);
TimeIn32KhzUnit TickTrigger(void);
TimeIn32KhzUnit TickSuspend(TimeIn32KhzUnit);
TimeIn32KhzUnit TickGetNearestEventTime(void);
BOOL TickManagerIsSuspendContext(void);
/* TickRequestHWclockSettingOnNextActivation()
   Due to bug in HW, the OS timer clock frequency
   should be set to 13Mhz on regular work (to avoid block of the bus
   due to read after write) while to 32Khz on D2/D4 for power save.
   The bug exist in Z0,Z1,A0, If fixed on B0, put all under ifdef ...
*/
void TickRequestHWclockSettingOnNextActivation(CLOCK_MODES mode);
void TickHandleBeforeDisableInt (void );

//#ifdef TICK_MANAGER_TEST
void TickManagerTestInit(void);
//#endif



#endif
