#ifndef __LOWPOWER_API_H__
#define __LOWPOWER_API_H__
#define LP_MAX_ENTRYS   10
#define LP_DBG_DISABLE_D2    0  //for dbg, temporarily disable d2
#define LP_DBG_DISABLE_C1    0  //for dbg,temporarily disable c1 
//define lowpower id
#define LP_ID_LCD 		0
#define LP_ID_CAMERA 	1
#define LP_ID_AUDIO 	2
#define LP_ID_SDCARD 	3
#define LP_ID_KEYPAD    4
#define LP_ID_VPLAYER 	5
#define LP_ID_PWRONOFF  6

//flags for cp.
#define LP_FLGAS(id,retVal) ((retVal)?0:(1<<(id)))

// return value of below functions:
// 1:can sleep
// 0:can not sleep
typedef int (*lpEnterC1Callback)(void);
typedef int (*lpExitC1Callback)(void);

typedef int (*lpEnterD2Callback)(void);
typedef int (*lpExitD2Callback)(BOOL ExitFromD2);
void uiLowpowerInit(void);

//register c1 callback interface.
void uiC1CallbackRegister(int id,lpEnterC1Callback enter,lpExitC1Callback exit);
void uiC1CallbackunRegister(int id);

//register d2 callback interface.
void uiD2CallbackRegister(int id,lpEnterD2Callback enter,lpExitD2Callback exit);
void uiD2CallbackunRegister(int id);


//called by cp c1 function.
int uiEnterC1Callback(void);
int uiExitC1Callback(void);
//called by cp d2 function.
int uiEnterD2Callback(void);
int uiExitD2Callback(BOOL ExitFromD2);
//decide whether ui can suspend or not. called by cp.
int uiSuspend(void);
//id:lowpower id
//flag: 1:allow to enter d2
//flag: 0:do not allow to enter d2
void uiSetSuspendFlag(int id,int flag);
#endif
