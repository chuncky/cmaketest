#ifndef _UI_OS_TIMER_H_
#define _UI_OS_TIMER_H_


#ifdef __cplusplus
extern "C" {
#endif

// 1 ticks = 5ms
#define MS_TO_TICKS(n) ((n) / OSA_TICK_FREQ_IN_MILLISEC ? (n) / OSA_TICK_FREQ_IN_MILLISEC : 1)


#define UOS_NB_MAX_TIMER_ENVELOPPE 64    /* Number Max of actif timerS.      */
#define INVALID_TIMER_ID	UOS_NB_MAX_TIMER_ENVELOPPE

#define UI_TIMER_MODE_SINGLE			0
#define UI_TIMER_MODE_PERIODIC			1
#define UI_TIMER_FUNC_MODE_SINGLE		2
#define UI_TIMER_FUNC_MODE_PERIODIC		3

#define PERIOD_TIMER_OPTIMIZE

void UOS_InitTimer (void);

//function timer
u8 UOS_get_FunctionTimer(void);
/*Please call the UOS_get_FunctionTimer, if you want to release, need call UOS_KillFunctionTimer to release*/
u8 UOS_StartFunctionTimer_periodic(u8 id, u32 Ticks,void(* Function)(void *),void * Param,const char * pName);
/*Please call the UOS_get_FunctionTimer, if you want to release, need call UOS_KillFunctionTimer to release*/
u8 UOS_StartFunctionTimer_single(u8 id, u32 Ticks, void (*Function)(void *), void *Param, const char *pName);

u8 UOS_KillFunctionTimer(u8);

//task timer
BOOL UOS_StartTimerEX (HANDLE hTask, UINT16 nTimerId,UINT8 nMode,UINT32 Ticks, const char *TimerName);
BOOL  UOS_KillTimerEX (HANDLE hTask,UINT16 nTimerId);
#ifdef PERIOD_TIMER_OPTIMIZE
void UOS_PeriodTimerHandleRsp(u8 timer_id, u32 event_id);
#endif

#ifdef __cplusplus
}
#endif


#endif
