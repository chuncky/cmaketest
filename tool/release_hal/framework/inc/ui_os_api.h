#ifndef _UI_OS_API_H_
#define _UI_OS_API_H_

#include "plat_types.h"
#include "cp/cp_include.h"

#include "ui_os_semaphore.h"
#include "ui_os_message.h"
#include "ui_os_timer.h"
#include "ui_os_task.h"
#include "ui_os_mutex.h"
#include "ui_os_flag.h"

#define UOS_WAIT_FOREVER 0xFFFFFFFF
#define UOS_NO_WAIT    0x0

#define UOS_EVENT_PRI_NORMAL  0
#define UOS_EVENT_PRI_URGENT  1

#define UOS_EVENT_ILM_EXTMSG  (1<<2)		//OslIntMsgSendExtQueue:struct ilm_struct

//
// MMI TimerID
// 
#define UOS_MMI_TIMER_ID_BASE 0x000
#define UOS_MMI_TIMER_ID_END_ 0x3FF

//
// CSW TimerID
// 
#define UOS_CMID_TIMER_ID_BASE 0x400
#define UOS_CMID_TIMER_ID_END_ 0x4FF

#define HVY_TIMER_IN       ( 0xFFFFF000)

#endif
