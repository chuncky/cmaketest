#ifndef _UI_OS_TASK_H_
#define _UI_OS_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_RUNNING	0
#define TASK_SUSPEND	1

#define MMI_HEAP_SIZE_DEBUG

typedef void (*PTASK_ENTRY)(void * pParameter);

struct ui_task_desc {
	PTASK_ENTRY task_entry;
	void *param;
	u16 msgq_size;
	u16 stack_size;
	void *stack_addr;
	u8 task_priority;
	const char *task_name;
};

typedef struct _TaskDesc
{
	void (*TaskBody)(void *);
	const char *Name;
	void *Parameter;
	UINT16 nStackSize;
	UINT8 nPriority;
}TaskDesc;

typedef struct _TASK_HANDLE
{
	TaskDesc sTaskDesc;
	UINT8 nTaskId;
	UINT8 nMailBoxId;
	// indicate the task's status
	UINT8 nStatus;
	UINT8 padding[4];
}TASK_HANDLE;

/// Task context static descriptor
typedef struct
{
    /// A pointer to a task body.
    /// The task body corresponds to a function implementing the task. 
    /// When the task enters in ACTIVE state for the first time, the task body is
    /// invoked with the address of the dynamic context as a parameter.
    void  (*TaskBody)(void *);
    /// A pointer to a string that contains the name of the task.
    const char *Name;
    // used by OSATaskCreate()
    void *Parameter;
    /// A stack size expressed in number of double words, to be allocated for the task.
    u16   StackSize;
    /// The priority [0..255] to be given to the task.
    u8    Priority;
} ui_TaskDesc_t;

/*-----------------------------------------------
 * brief Creates a thread
 * return
 *   OSA_SUCCESS
 *   OSA_ERROR
 --------------------------------------------------*/
HANDLE UOS_CreateTask(
  PTASK_ENTRY pTaskEntry,
  PVOID pParameter,
  UINT16 nMsgQSize,  
  UINT16 nStackSize,
  UINT8 nPriority,
  const char *pTaskName
);

void UOS_TaskInit (void);

BOOL UOS_DeleteTask(TASK_HANDLE *pHTask);

BOOL UOS_StopTask(TASK_HANDLE *pHTask);

BOOL UOS_SuspendTask(TASK_HANDLE *pHTask);

BOOL UOS_ResumeTask (TASK_HANDLE *pHTask);

void UOS_Sleep (UINT32 Ticks);
void UOS_SleepMs (UINT32 ms);
void UOS_SleepSeconds(UINT32 s);

const char *UOS_GetCurrentTaskName (void);

void UOS_GetCurrentStackInfo(u32 *total, u32 *used, u32 *percent);

void *UOS_GetTaskRef(u8 task_id);

u32 UOS_GetUpTime(void);

#ifdef MMI_HEAP_SIZE_DEBUG
u32 UOS_GetMMICurrentHeapSize(void);
#else
#define UOS_GetMMICurrentHeapSize()
#endif

void UOS_EnableFrequencyConversion(void);

void UOS_DisableFrequencyConversion(void);

void UOS_ChangeCoreFreqTo624M(void);

#ifdef __cplusplus
}
#endif


#endif
