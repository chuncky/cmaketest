#ifndef _OS_HAL_H_
#define _OS_HAL_H_

typedef void*   OSATaskRef;
typedef void*   OSAHISRRef;
typedef void*   OSASemaRef;
typedef void*   OSAMutexRef;
typedef void*   OSAMsgQRef;
typedef void*   OSAMailboxQRef;
typedef void*   OSAPoolRef;
typedef void*   OSATimerRef;
typedef void*   OSAFlagRef;
typedef void*	OSAPartitionPoolRef;
/* Remain for backwards compatibility */
typedef void*   OSTaskRef;
typedef void*   OSSemaRef;
typedef void*   OSMutexRef;
typedef void*   OSMsgQRef;
typedef void*   OSMailboxQRef;
typedef void*   OSPoolRef;
typedef void*   OSTimerRef;
typedef void*   OSFlagRef;
typedef UINT8   OS_STATUS;
typedef UINT8   OSA_STATUS;

typedef void    *OsaRefT ;


typedef UINT32 rt_uint32_t;
typedef UINT8  rt_uint8_t;




typedef struct
{
    UINT32              initialTime ;                   //  [IN]    Initial expiration time in clock ticks.
    UINT32              rescheduleTime ;                //  [IN]    Periodic expiration time in clock ticks. 0=One shot timer.
    void                (*callBackRoutine)(UINT32) ;    //  [IN]    Routine to call when timer expiers.
    UINT32              timerArgc ;                     //  [IN]    Argument to be passed to the callBackRoutine
    char                *name ;                         //  [OP]    Pointer to a NULL terminated string.
    BOOL                bSharedForIpc ;                 //  [OP]    TRUE - The object can be accessed from all processes.
}
OsaTimerParamsT ;





#define RT_NULL NULL
#define RT_ASSERT ASSERT

// #define RTDM_TRACE	UartLogPrintf
#if 1
#define RTDM_TRACE(fmt, ...)	
#else
#define RTDM_TRACE uart_printf
#endif

void        OsaTaskSleep(
    UINT32                  ticks,              //  [IN]    Time to sleep in ticks.
    void                    *pForFutureUse
    ) ;

/***********************************************************************
 *
 * Name:        OSATaskSleep
 *
 * Description: Task sleep.
 *
 * Parameters:
 *  UINT32              tICKS           [IN]    Ticks to sleep.(1s=200ticks)
 *
 * Returns: None
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskSleep(tICKS)                         OsaTaskSleep(tICKS,NULL)


typedef enum
{
	RT_STATUS_OK,
	RT_STATUS_ERROR
}rt_status_t;


#define DIAG_MIN_MEMORY_ALLOCATION_SIZE              (0x4UL)


/* dynamic pool magic and mask */
#define RTDM_HEADER_MAGIC		0x1ea01ea0
#define RTDM_HEADER_MAGIC_MASK	0xfffffffe

#define RTDM_HEADER_USED		0x01
#define RTDM_HEADER_FREED		0x00

#define RTDM_ALIGN(size)		((size + sizeof(rt_uint32_t) - 1) & ~(sizeof(rt_uint32_t) - 1))
#define RTDM_OVERHEAD			RTDM_ALIGN(sizeof(struct rtdm_header))

#define RTDM_BLOCK_IS_USED(h)	(h->magic & RTDM_HEADER_USED)





struct rtdm_header
{
	rt_uint32_t magic;
	struct rtdm_header *next_memory, *prev_memory;
	struct rtdm_header *next_free_memory, *prev_free_memory;

	struct rtdm_pcb* rtdm_memory_pool;
};


/* RT dynamic pool control block */
struct rtdm_pcb
{
	/* pool start address and size */
	void *rtdm_start_address;
	rt_uint32_t rtdm_pool_size;

	/* the minimum allocation size and the available size in pool */
	rt_uint32_t rtdm_mini_allocation;
	rt_uint32_t rtdm_available;

	/* used block list */
	struct rtdm_header* rtdm_block_list;

	/* free block list and free block list header */
	struct rtdm_header* rtdm_free_list;
	struct rtdm_header  rtdm_free_header;
};




typedef struct
{
    char                *name ;                     //  [OP]    Pointer to a NULL terminated string.
    BOOL                bSharedForIpc ;             //  [OP]    TRUE - The object can be accessed from all processes.
}
OsaFlagCreateParamsT ;

OSA_STATUS  OsaFlagCreate(
    OsaRefT                     *pOsaRef,           //  [OT]    Reference.
    OsaFlagCreateParamsT        *pParams            //  [OP,IN] Input Parameters (see datatype for details).
    );

#define     OSAFlagCreate(rEF)              OsaFlagCreate(rEF,NULL)























rt_status_t rtdm_memory_free(void* memory);





#endif	/* _OS_HAL_H_ */

