#ifndef _UI_OS_MESSAGE_H_
#define _UI_OS_MESSAGE_H_


#ifdef __cplusplus
extern "C" {
#endif


#define MSGQ_DEBUG

// support MAX 64 message queue
#define UOS_NB_MAX_MBX_ENV		64
#define INVALID_MSGQ_ID UOS_NB_MAX_MBX_ENV

#define TASK_WITHOUT_MSGQ	0
#define TASK_MSGQ_SIZE_16	16
#define TASK_MSGQ_SIZE_32	32
#define TASK_MSGQ_SIZE_64	64
#define TASK_MSGQ_SIZE_128	128
#define TASK_MSGQ_SIZE_256	256
#define TASK_MSGQ_SIZE_512	512

#define TASK_DEFAULT_MSGQ_SIZE	TASK_MSGQ_SIZE_64

#define UOS_SEND_EVT		(1 << 0)
#define UOS_SEND_MSG		0

#define UOS_QUEUE_FIRST		(1 << 1)
#define UOS_QUEUE_LAST		0

#define UOS_SEND_ILM_EXTMSG		(1 << 2)	//OslIntMsgSendExtQueue:struct ilm_struct

typedef struct _UI_EVENT {
	UINT32 nEventId;
	UINT32 nParam1;
	UINT32 nParam2;
	UINT32 nParam3;
} UI_EVENT;

#define UOS_EVT_MBX_SIZE 4

typedef struct
{
    u32 EvtTag;
    u32 Evt[UOS_EVT_MBX_SIZE];
	u32 send_timestamp;
	u32 msgs_in_queue;	
#ifdef MSGQ_DEBUG
	u32 task_run_count;
	u32 task_state;
#endif	
} UI_OS_MSG_Q;

void UOS_MessageQueueInit (void);

u8 UOS_NewMessageQueue(const char *name, int message_num);

void UOS_FreeMessageQueue (u8);

u32 UOS_MsgQEnqueued (u8 index);

void UOS_SendMsg (void *, u8, u8);

void* UOS_WaitMsg (u32 *Evt, u8 index, UINT32 nTimeOut);

BOOL UOS_WaitEvent(HANDLE hTask, UI_EVENT* pEvent,UINT32 nTimeOut);

BOOL UOS_SendEvent(HANDLE hTask, UI_EVENT* pEvent,UINT32 nTimeOut,UINT16 nOption);

BOOL UOS_IsEventAvailable(HANDLE hTask);

u32 UOS_GetMessageQueueSize(u8 index);

u32 UOS_MsgQAvailableSize(u8 index);

#ifdef __cplusplus
}
#endif


#endif
