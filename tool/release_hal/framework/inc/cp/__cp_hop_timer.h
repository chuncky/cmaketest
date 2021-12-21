/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_HOP_TIMER_H__
#define __CP_HOP_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef int TIMER_STATUS;		/* timer's function execution status*/
typedef int TIMER_ID;
typedef void(*ACC_TIMER_CALLBACK)(UINT32);	/* Function pointer */

typedef enum   /* The meaning of the API flag*/ 		
{
	ACC_TIMER_PERIODIC = 0x1,		/* periodic execution */
	ACC_TIMER_AUTO_DELETE = 0x2 	/* one execution */
}ACC_TIMER_FLAG;

typedef enum	/* determine whether the timer is started or not*/
{
	ACC_ACTIVE, 					 /* the node has been added in the active_list and callback function has not been executed */
	ACC_INACTIVE,					 /* the node is not in the active_list*/
	ACC_TIMER_ID_NOT_EXIST			 /* timer_id does not exist */				   
}ACC_TIMER_STATUS;

TIMER_STATUS AccTimerDelete(TIMER_ID acc_timer_id);

TIMER_STATUS AccTimerStop(TIMER_ID acc_timer_id);

/*
* param:
* 1. period:us is unit.
*/
TIMER_STATUS AccTimerStartEx(
	unsigned int flag,
	unsigned int period,
	ACC_TIMER_CALLBACK timerCallbackFunc,
	unsigned int timerParams);


ACC_TIMER_STATUS GetTimerStatus(TIMER_ID acc_timer_id);


#ifdef __cplusplus
}
#endif


#endif
