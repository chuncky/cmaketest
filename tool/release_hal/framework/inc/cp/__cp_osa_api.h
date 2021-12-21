/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP__OSA__API_H__
#define __CP__OSA__API_H__


#ifdef __cplusplus
extern "C" {
#endif

#ifndef OSA_QUEUE_NAMES
#define OSA_QUEUE_NAMES
#endif

#ifndef PLAT_USE_ALIOS
#ifndef PLAT_USE_THREADX
#define PLAT_USE_THREADX
#endif
#endif

#include "plat_types.h"

/*****************************************************************************
 * OSA Data Types
 ****************************************************************************/
#define IMPC

#undef NULL
#define NULL 0L

//copy from glb_config.h
#define OSA_TICK_FREQ_IN_MILLISEC   5
#define RTI_THREAD_ARRAY_SIZE	200//128

//TODO:copy from cp/.../osa.h
#define OSA_SUSPEND             0xFFFFFFFF

#define OSA_NO_SUSPEND             0x0

#define OSA_FLAG_AND            5
#define OSA_FLAG_AND_CLEAR      6
#define OSA_FLAG_OR             7
#define OSA_FLAG_OR_CLEAR       8
#define OSA_FIXED               9
#define OSA_VARIABLE            10
#define OSA_FIFO                11
#define OSA_PRIORITY            12

#if 0
typedef unsigned char UINT8;
typedef unsigned long UINT32;
#endif

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

enum
{
  OS_SUCCESS = 0,		 /* 0x0 -no errors										  */
  OS_FAIL,				 /* 0x1 -operation failed code							  */
  OS_TIMEOUT,			 /* 0x2 -Timed out waiting for a resource				  */
  OS_NO_RESOURCES,		 /* 0x3 -Internal OS resources expired					  */
  OS_INVALID_POINTER,	 /* 0x4 -0 or out of range pointer value				  */
  OS_INVALID_REF,		 /* 0x5 -invalid reference								  */
  OS_INVALID_DELETE,	 /* 0x6 -deleting an unterminated task					  */
  OS_INVALID_PTR,		 /* 0x7 -invalid memory pointer 						  */
  OS_INVALID_MEMORY,	 /* 0x8 -invalid memory pointer 						  */
  OS_INVALID_SIZE,		 /* 0x9 -out of range size argument 					  */
  OS_INVALID_MODE,		 /* 0xA, 10 -invalid mode								  */
  OS_INVALID_PRIORITY,	 /* 0xB, 11 -out of range task priority 				  */
  OS_UNAVAILABLE,		 /* 0xC, 12 -Service requested was unavailable or in use  */
  OS_POOL_EMPTY,		 /* 0xD, 13 -no resources in resource pool				  */
  OS_QUEUE_FULL,		 /* 0xE, 14 -attempt to send to full messaging queue	  */
  OS_QUEUE_EMPTY,		 /* 0xF, 15 -no messages on the queue					  */
  OS_NO_MEMORY, 		 /* 0x10, 16 -no memory left							  */
  OS_DELETED,			 /* 0x11, 17 -service was deleted						  */
  OS_SEM_DELETED,		 /* 0x12, 18 -semaphore was deleted 					  */
  OS_MUTEX_DELETED, 	 /* 0x13, 19 -mutex was deleted 						  */
  OS_MSGQ_DELETED,		 /* 0x14, 20 -msg Q was deleted 						  */
  OS_MBOX_DELETED,		 /* 0x15, 21 -mailbox Q was deleted 					  */
  OS_FLAG_DELETED,		 /* 0x16, 22 -flag was deleted							  */
  OS_INVALID_VECTOR,	 /* 0x17, 23 -interrupt vector is invalid				  */
  OS_NO_TASKS,			 /* 0x18, 24 -exceeded max # of tasks in the system 	  */
  OS_NO_FLAGS,			 /* 0x19, 25 -exceeded max # of flags in the system 	  */
  OS_NO_SEMAPHORES, 	 /* 0x1A, 26 -exceeded max # of semaphores in the system  */
  OS_NO_MUTEXES,		 /* 0x1B, 27 -exceeded max # of mutexes in the system	  */
  OS_NO_QUEUES, 		 /* 0x1C, 28 -exceeded max # of msg queues in the system  */
  OS_NO_MBOXES, 		 /* 0x1D, 29 -exceeded max # of mbox queues in the system */
  OS_NO_TIMERS, 		 /* 0x1E, 30 -exceeded max # of timers in the system	  */
  OS_NO_MEM_POOLS,		 /* 0x1F, 31 -exceeded max # of mem pools in the system   */
  OS_NO_INTERRUPTS, 	 /* 0x20, 32 -exceeded max # of isr's in the system 	  */
  OS_FLAG_NOT_PRESENT,	 /* 0x21, 33 -requested flag combination not present	  */
  OS_UNSUPPORTED,		 /* 0x22, 34 -service is not supported by the OS		  */
  OS_NO_MEM_CELLS,		 /* 0x23, 35 -no global memory cells					  */
  OS_DUPLICATE_NAME,	 /* 0x24, 36 -duplicate global memory cell name 		  */
  OS_INVALID_PARM		 /* 0x25, 37 -invalid parameter 						  */
};

/*****************************************************************************
 * OSA Function Prototypes
 ****************************************************************************/


/*========================================================================
 *  OSA Initialization:
 *
 *  Initializes OSA internal structures, tables, and OS specific services.
 *
 *========================================================================*/

/***********************************************************************
 *
 * Name:        OSAInitialize()
 *
 * Description: Initialize OS specific stuff.
 *
 * Parameters:  None
 *
 * Returns:     
 *	OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAInitialize()     OsaInit(NULL)


/***********************************************************************
 *
 * Name:        OSARun()
 *
 * Description: Activates the OS.
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSARun()            OsaRun(NULL)


/*========================================================================
 *  OSA Task Management:
 *========================================================================*/

/***********************************************************************
 *
 * Name:        OSATaskCreate
 *
 * Description: Create Task.
 *
 * Parameters:
 *  OSATaskRef                 	*taskRef      [OUT]    OS task reference 
 *  void		                  	*stackPtr      [IN]    pointer to start of task stack area   
 *  UINT32 	                  	stackSize      [IN]    number of bytes in task stack area 
 *  UINT8                	    	priority     [IN]    task priority 0 - 31  
 *  CHAR                 		*taskName      [IN]    task name
 *  void                 			*taskStart(void*)      [IN]   pointer to task entry point 
 *  void                 			*argv      [IN]    task entry argument pointer  
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSATaskCreate(
    OSATaskRef* taskRef,     
    void*       stackPtr,  
    UINT32      stackSize,      
    UINT8       priority,       
    CHAR        *taskName,      
    void        (*taskStart)(void*), 
    void*       argv            
 );

IMPC extern OSA_STATUS OSATaskCreateEx(
    OSATaskRef* taskRef,     
    void*       stackPtr,  
    UINT32      stackSize,      
    UINT8       priority,       
    CHAR        *taskName,      
    void        (*taskStart)(void*), 
    void*       argv            
 );

/***********************************************************************
 *
 * Name:        OSATaskTerminate
 *
 * Description: Terminate Task.
 *
 * Parameters:
 *  OSATaskRef                 oSArEF      [IN]    Task Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskTerminate(oSArEF)                      OsaTaskTerminate(oSArEF,NULL)

IMPC extern OSA_STATUS  OsaTaskDelete(
    OsaRefT                 OsaRef,             //  [IN]    Reference.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSATaskDelete
 *
 * Description: Delete Task.
 *
 * Parameters:
 *  OSATaskRef                 oSArEF      [IN]    Task Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskDelete(oSArEF)                       OsaTaskDelete(oSArEF,NULL)



IMPC extern OSA_STATUS  OsaTaskSuspend(
    OsaRefT                 OsaRef,             //  [IN]    Reference.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSATaskSuspend
 *
 * Description: Suspend Task.
 *
 * Parameters:
 *  OSATaskRef                 oSArEF      [IN]    Task Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskSuspend(oSArEF)                      OsaTaskSuspend(oSArEF,NULL)


IMPC extern OSA_STATUS OsaTaskResume(
    OsaRefT                 OsaRef,             //  [IN]    Reference.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSATaskResume
 *
 * Description: Resume Task.
 *
 * Parameters:
 *  OSATaskRef                 oSArEF      [IN]    Task Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskResume(oSArEF)                       OsaTaskResume(oSArEF,NULL)


IMPC extern void        OsaTaskSleep(
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



IMPC extern OSA_STATUS  OsaTaskChangePriority(
    OsaRefT                 OsaRef,             //  [IN]    Reference.
    UINT8                   newPriority,        //  [IN]    New task priority.
    UINT8                   *oldPriority,       //  [OT]    Old task priority.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSATaskChangePriority
 *
 * Description: Change task priority.
 *
 * Parameters:
 *  OSATaskRef          oSArEF          [IN]    Task Reference.
 *  UINT8                   nEW     [IN]    New Priority.
 *  UINT8                  *oLD    [OUT]    Old Priority.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskChangePriority(oSArEF,nEW,oLD)       OsaTaskChangePriority(oSArEF,nEW,oLD,NULL)



IMPC extern OSA_STATUS  OsaTaskGetPriority(
    OsaRefT                 OsaRef,             //  [IN]    Reference.
    UINT8                   *pPriority,         //  [OT]    Task priority.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSATaskGetPriority
 *
 * Description: Get task priority.
 *
 * Parameters:
 *  OSATaskRef                 oSArEF          [IN]    Task Reference.
 *  UINT8                   *pRIORITY      [OUT]    Priority.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskGetPriority(oSArEF,pRIORITY)         OsaTaskGetPriority(oSArEF,pRIORITY,NULL)


IMPC extern OSA_STATUS OsaHISRGetPriority( 
	OsaRefT 				OsaRef, 			//  [IN]    Reference.
	UINT8 					*pPriority, 		//  [OT]    Task priority.
	void 					*pForFutureUse 
	) ;

/***********************************************************************
 *
 * Name:        OSAHISRGetPriority
 *
 * Description: Get HISR priority.
 *
 * Parameters:
 *  OSATaskRef                 oSArEF          [IN]    HISR Reference.
 *  UINT8                   *pRIORITY      [OUT]    Priority.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAHISRGetPriority(oSArEF,pRIORITY)         OsaHISRGetPriority(oSArEF,pRIORITY,NULL)


IMPC extern void        OsaTaskYield(  void *pForFutureUse  ) ;
/***********************************************************************
 *
 * Name:        OSATaskYield
 *
 * Description: Allow other task to run.
 *
 * Parameters: None
 *
 * Returns:  None
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskYield()                              OsaTaskYield(NULL)


IMPC extern OSA_STATUS  OsaTaskGetCurrentRef(
    OsaRefT                 *pOsaRef,           //  [OT]    Reference.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSATaskGetCurrentRef
 *
 * Description: Get Task Ref.
 *
 * Parameters:
 *  OSATaskRef                 *PoSArEF    [OUT]    Task Reference.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATaskGetCurrentRef(PoSArEF)               OsaTaskGetCurrentRef(PoSArEF,NULL)



IMPC extern OSA_STATUS  OsaHISRGetCurrentRef(
    OsaRefT                 *pOsaRef,           //  [OT]    Reference.
    void                    *pForFutureUse
    ) ;
/***********************************************************************
 *
 * Name:        OSAHISRGetCurrentRef
 *
 * Description: Get HISR Ref.
 *
 * Parameters:
 *  OSAHISRRef                 *PoSArEF    [OUT]    HISR Reference.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAHISRGetCurrentRef(PoSArEF)				OsaHISRGetCurrentRef(PoSArEF,NULL)

#if 1
IMPC extern OSA_STATUS  OsaSemaphoreAcquire(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    UINT32                      timeout,            //  [IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout in OS ticks.
    void                        *pForFutureUse
    ) ;

IMPC extern OSA_STATUS  OsaSemaphoreRelease(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) ;

IMPC extern OSA_STATUS  OsaSemaphoreDelete(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) ;
#endif

/*========================================================================
 *  OSA Semaphore Management
 *========================================================================*/

/***********************************************************************
 *
 * Name:        OSASemaphoreCreate
 *
 * Description: Create Semaphore.
 *
 * Parameters:
 *  OSASemaRef                 		*semaRef      [OUT]    OS semaphore reference  
 *  UINT32		                  	initialCount      [IN]     initial count of the semaphore   
 *  UINT8 	                  		waitingMode      [IN]    mode of tasks waiting (OS_FIFO, OS_PRIORITY)
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSASemaphoreCreate (
    OSASemaRef   *semaRef,   
    UINT32      initialCount,   
    UINT8       waitingMode    
 );

/***********************************************************************
 *
 * Name:        OSASemaphoreAcquire
 *
 * Description: Acquire semaphore.
 *
 * Parameters:
 *  OSASemaRef                 	oSArEF          [IN]    Reference.
 *  UINT32                  		tIMEoUT         [IN]    Timeout in OS Ticks.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSASemaphoreAcquire(oSArEF,tIMEoUT)         OsaSemaphoreAcquire(oSArEF,tIMEoUT,NULL)

/***********************************************************************
 *
 * Name:        OSASemaphorePoll
 *
 * Description: Poll semaphore.
 *
 * Parameters:
 *  OSASemaRef                 	oSArEF          [IN]    Reference.
 *  UINT32                  		*PcOUNT         [OUT]    Current semaphore count.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSASemaphorePoll(oSArEF,PcOUNT)             OsaSemaphorePoll(oSArEF,PcOUNT,NULL)

/***********************************************************************
 *
 * Name:        OSASemaphoreRelease
 *
 * Description: Release semaphore.
 *
 * Parameters:
 *  OSASemaRef                 oSArEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSASemaphoreRelease(oSArEF)                 OsaSemaphoreRelease(oSArEF,NULL)

/***********************************************************************
 *
 * Name:        OSASemaphoreDelete
 *
 * Description: Delete semaphore.
 *
 * Parameters:
 *  OSASemaRef                 oSArEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSASemaphoreDelete(oSArEF)                  OsaSemaphoreDelete(oSArEF,NULL)


IMPC extern OSA_STATUS  OsaMutexLock(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    UINT32                      timeout,            //  [IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout in OS ticks.
    void                        *pForFutureUse
    ) ;

IMPC extern OSA_STATUS  OsaMutexUnlock(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) ;

IMPC extern OSA_STATUS  OsaMutexDelete(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) ;

/*========================================================================
 *  OSA Mutex Management
 *========================================================================*/
IMPC extern OSA_STATUS OSAMutexCreate (
    OSMutexRef  *mutexRef,       /* OS mutex reference                         */
    UINT8       waitingMode      /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );

/***********************************************************************
 *
 * Name:        OSAMutexLock
 *
 * Description: 
 *
 * Parameters:
 *  OSAMutexRef                 oSArEF          [IN]    Reference.
 *  UINT32                  tIMEoUT         [IN]    Timeout in OS Ticks.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/

#define     OSAMutexLock(oSArEF,tIMEoUT)    OsaMutexLock(oSArEF,tIMEoUT,NULL)

/***********************************************************************
 *
 * Name:        OSAMutexUnlock
 *
 * Description: 
 *
 * Parameters:
 *  OSAMutexRef                 oSArEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMutexUnlock(oSArEF)          OsaMutexUnlock(oSArEF,NULL)

/***********************************************************************
 *
 * Name:        OSAMutexDelete
 *
 * Description: 
 *
 * Parameters:
 *  OSAMutexRef                 oSArEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMutexDelete(oSArEF)          OsaMutexDelete(oSArEF,NULL)


/*========================================================================
 *  OSA Interrupt Control
 *========================================================================*/


/***********************************************************************
 *
 * Name:        OSACreateHISR
 *
 * Description: Create ISR.
 *
 *  Parameters:
 *  OSAHISRRef                 	*oSArEF      [OUT]  oOS HISR  reference 
 *  CHAR		                  	*name      [IN]    HISR name
  *  void                 		*hisr_entry(void)      [IN]   pointer to HISR entry point 
 *  UINT8                	    	priority     [IN]    HISR priority 0 - 2  
 *
 * Returns:
 *  None
 *
 * Notes:
 *
 ***********************************************************************/

#define OSACreateHISR(oSArEF, name, hisr_entry, priority)		Os_Create_HISR(hisr, name, hisr_entry, priority)


/***********************************************************************
 *
 * Name:        OSADeleteHISR
 *
 * Description: Delete ISR.
 *
 *  Parameters:
 *  OSAHISRRef                 *oSArEF      [IN]    reference 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define OSADeleteHISR(oSArEF)		Os_Delete_HISR(oSArEF)


/***********************************************************************
 *
 * Name:        OSAActivateHISR
 *
 * Description: Activate ISR.
 *
 *  Parameters:
 *  OSAHISRRef                 *oSArEF      [IN]    reference 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define OSAActivateHISR(oSArEF)		Os_Activate_HISR(oSArEF)


/*===========================================================================
 *  OSA Real-Time Access:
 *=========================================================================*/
 /***********************************************************************
 *
 * Name:        OSAGetTicks
 *
 * Description: Number of ticks that passed since last reset.
 *
 * Parameters: None
 *
 * Returns:  UINT32   Number of ticks that passed since last reset
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAGetTicks()           OsaGetTicks(NULL)

/***********************************************************************
 *
 * Name:        OSAGetClockRate
 *
 * Description: Get the current system clock rate.
 *
 * Parameters: None
 *
 * Returns:     UNIT32  - current clock rate (ms / tick)
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAGetClockRate()       OsaGetClockRate(NULL)

/***********************************************************************
 *
 * Name:        OSATick
 *
 * Description: Tick the OS.
 *
 * Parameters: None
 *
 * Returns: None
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATick()               OsaTick(NULL)


/*========================================================================
 *  OSA Sys Context info
 *========================================================================*/

/***********************************************************************
 *
 * Name:        OSAContextLock
 *
 * Description: Lock context - No interrupts and no preemptions.
 *
 * Parameters:  None
 *
 * Returns:		OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAContextLock()                    OsaContextLock(NULL)


/***********************************************************************
 *
 * Name:       OSAContextUnlock
 *
 * Description: Restore the context.
 *
 * Parameters: None
 *
 * Returns:    OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAContextUnlock()                  OsaContextRestore(NULL)


 IMPC extern OSA_STATUS  OsaMsgQPoll(
	 OsaRefT					 OsaRef,			 //  [IN]	 Reference.
	 UINT32 					 *pCount,			 //  [OT]	 Number of messages in Q.
	 void						 *pForFutureUse
	 ) ;
 
 IMPC extern OSA_STATUS  OsaMsgQDelete(
	 OsaRefT					 OsaRef,			 //  [IN]	 Reference.
	 void						 *pForFutureUse
	 ) ;

 IMPC extern OSA_STATUS 	 OSAMsgQFrontSend(OSAMsgQRef queue_ptr, void *message,
										 UINT32 size, UINT32 suspend);

 /*========================================================================
  *  OSA Message Passing
  *========================================================================*/

/***********************************************************************
 *
 * Name:        OSAMsgQCreate
 *
 * Description: create message queue
 *
 * Parameters:
 *  OSMsgQRef                 	*msgQRef          [OUT]    OS message queue reference .
 *  char                  		*queueName       	 [IN]   name of message queue.
 *  UINT32                  		maxSize         [IN]    max message size the queue supports .
 *  UINT32                  		maxNumber         [IN]    max # of messages in the queue  . 
 *  UINT32                  		waitingMode         [IN]    mode of tasks waiting OS_FIFO, OS_PRIORITY.
  
 * Returns:    OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMsgQCreate(
    OSMsgQRef   *msgQRef,       
#if defined (OSA_QUEUE_NAMES)
    char        *queueName,     
#endif
    UINT32      maxSize,       
    UINT32      maxNumber,      
    UINT8       waitingMode    
 );


/***********************************************************************
 *
 * Name:		OSAMsgQSend
 *
 * Description: Send to message Q.
 *
 * Parameters:
 *  OSMsgQRef                 	msgQRef          [IN]    OS message queue reference .
 *  UINT32                  		size         [IN]    size of the message 
 *  UINT8                  		*msgPtr         [IN]    start address of the data to be sent
 *  UINT32                  		timeout         [IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout

 *
 * Returns:
 *	OSA_STATUS	OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMsgQSend (
    OSMsgQRef   msgQRef,       
    UINT32      size,           
    UINT8       *msgPtr,        
    UINT32      timeout       
 );


/***********************************************************************
 *
 * Name:		OSAMsgQRecv
 *
 * Description: Recieve from message Q..
 *
 * Parameters:
 *  OSMsgQRef                 	msgQRef          [IN]    OS message queue reference .
 *  UINT8                  		*recvMsg         OUT]     pointer to the message received
 *  UINT32                  		size         [IN]    size of the message 
 *  UINT32                  		timeout         [IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout

 *
 * Returns:
 *	OSA_STATUS	OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMsgQRecv (
    OSMsgQRef   msgQRef,       
    UINT8       *recvMsg,       
    UINT32      size,          
    UINT32      timeout         
 );

/***********************************************************************
 *
 * Name:        OSAMsgQPoll
 *
 * Description: Get the number of messages in queue.
 *
 * Parameters:
 *  OSAMsgQRef          oSArEF          [IN]    Reference.
 *  UINY32                  *cOUNT         [OT]    Number of messages in queue.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMsgQPoll(oSArEF,cOUNT)                           OsaMsgQPoll(oSArEF,cOUNT,NULL)


/***********************************************************************
 *
 * Name:        OSAMsgQDelete
 *
 * Description: Delete message Q.
 *
 * Parameters:
 *  OSAMsgQRef                 oSArEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMsgQDelete(oSArEF)                               OsaMsgQDelete(oSArEF,NULL)


 /*========================================================================
  *  OSA Mailboxes
  *========================================================================*/

/***********************************************************************
 *
 * Name:        OSAMailboxQCreate
 *
 * Description: Create a mailbox Q.
 *              Each mailbox entry is 32 bit.
 *
 * Parameters:
 *  OSMailboxQRef                     *mboxQRef    [OUT]    OS mailbox queue reference.
 *  char                  			*queueName       	 [IN]   name of mailbox queue 
 *  UINT32                  			maxNumber         [IN]    max # of messages in the queue  . 
 *  UINT8                  			waitingMode         [IN]    mode of tasks waiting OS_FIFO, OS_PRIORITY.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMailboxQCreate (
    OSMailboxQRef   *mboxQRef,      /* OS mailbox queue reference              */
#if defined (OSA_QUEUE_NAMES)
    char            *queueName,     /* name of mailbox queue                   */
#endif
    UINT32          maxNumber,      /* max # of messages in the queue          */
    UINT8           waitingMode     /* mode of tasks waiting OS_FIFO, OS_PRIORITY */
 );


/***********************************************************************
 *
 * Name:        OSAMailboxQSend
 *
 * Description: Write data to mailbox Q.
 *
 * Parameters:
 *  OSAMailboxQRef                 	oSArEF          [IN]    Reference.
 *  UINT32                  			pTR       	 [IN]    Data to put in mailbox.
 *  UINT32                  			tIMEOUT         [IN]    timeout.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMailboxQSend(oSArEF,pTR,tIMEOUT)             OsaMailboxQSend(oSArEF,(UINT32)pTR,tIMEOUT,NULL)


/***********************************************************************
 *
 * Name:        OSAMailboxQRecv
 *
 * Description: Read from mailbox Q.
 *
 * Parameters:
 *  OSAMailboxQRef                 	oSArEF          [IN]    Reference.
 *  UINT32                 			*pTR        [OUT]    Data to receive from mailbox.
 *  UINT32                  			tIMEOUT         [IN]    timeout.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMailboxQRecv(oSArEF,pTR,tIMEOUT)             OsaMailboxQRecv(oSArEF,(UINT32 *)pTR,tIMEOUT,NULL)


/***********************************************************************
 *
 * Name:        OSAMailboxQPoll
 *
 * Description: Gen the number of messages in queue.
 *
 * Parameters:
 *  OSAMailboxQRef                 oSArEF          [IN]    Reference.
 *  UINY32                  		   *cOUNT         [OUT]    Number of messages in queue.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMailboxQPoll(oSArEF,cOUNT)                   OsaMailboxQPoll(oSArEF,cOUNT,NULL)


/***********************************************************************
 *
 * Name:        OSAMailboxQDelete
 *
 * Description: Delete mailbox Q.
 *
 * Parameters:
 *  OSAMailboxQRef                 oSArEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMailboxQDelete(oSArEF)                       OsaMailboxQDelete(oSArEF,NULL)


/*========================================================================
 *  OSA Event Management:
 *========================================================================*/

/***********************************************************************
 *
 * Name:        OsaFlagWait
 *
 * Description: Wait for event.
 *
 * Parameters:
 *  OSFlagRef                 flagRef          [IN]    OS reference to the flag.
 *  UINT32                  mask       	 	[IN]    flag mask to wait for .
 *  UINT32                  operation         [IN]    OSA_FLAG_AND, OSA_FLAG_AND_CLEAR,  OSA_FLAG_OR, OSA_FLAG_OR_CLEAR.
 *  UINT32                  *flags       	 	[OUT]    Current value of all flags 
 *  UINT32                  timeout       	[IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAFlagWait(
    OSFlagRef   flagRef,        
    UINT32      mask,          
    UINT32      operation,                         
    UINT32      *flags,       
    UINT32      timeout        
 );



typedef struct
{
    char                *name ;                     //  [OP]    Pointer to a NULL terminated string.
    BOOL                bSharedForIpc ;             //  [OP]    TRUE - The object can be accessed from all processes.
}
OsaFlagCreateParamsT ;

typedef struct
{
    UINT32              mask ;                      //  [IN]    Flag mask.
    UINT32              operation ;                 //  [IN]    OSA_FLAG_AND, OSA_FLAG_AND_CLEAR, OSA_FLAG_OR, OSA_FLAG_OR_CLEAR.
    UINT32              timeout ;                   //  [IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout in OS ticks.
    UINT32              *flags ;                    //  [OT]    Current value of the flag.
}
OsaFlagWaitParamsT ;


IMPC extern OSA_STATUS  OsaFlagCreate(
    OsaRefT                     *pOsaRef,           //  [OT]    Reference.
    OsaFlagCreateParamsT        *pParams            //  [OP,IN] Input Parameters (see datatype for details).
    ) ;

/***********************************************************************
 *
 * Name:        OSAFlagCreate
 *
 * Description: Create a flag event group.
 *
 * Parameters:
 *  OSFlagRef                     *rEF    [OT]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAFlagCreate(rEF)              OsaFlagCreate(rEF,NULL)

IMPC extern OSA_STATUS  OsaFlagSet(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    UINT32                      mask,               //  [IN]    Flag mask.
    UINT32                      operation,          //  [IN]    OSA_FLAG_AND, OSA_FLAG_OR.
    void                        *pForFutureUse
    ) ;

/***********************************************************************
 *
 * Name:        OSAFlagSet
 *
 * Description: Set an event.
 *
 * Parameters:
 *  OSFlagRef                 rEF          [IN]    Reference.
 *  UINT32                  mASK            [IN]    Flag mask.
 *  UINT32                  oP       [IN]    OSA_FLAG_AND, OSA_FLAG_OR.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAFlagSet(rEF,mASK,oP)         OsaFlagSet(rEF,mASK,oP,NULL)


IMPC extern OSA_STATUS  OsaFlagDelete(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) ;

/***********************************************************************
 *
 * Name:        OSAFlagDelete
 *
 * Description: Delete event group.
 *
 * Parameters:
 *  OSFlagRef                 rEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAFlagDelete(rEF)              OsaFlagDelete(rEF,NULL)


/*========================================================================
 *  OSA Timer Management:
 *========================================================================*/

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

IMPC extern OSA_STATUS  OsaTimerCreate(
    OsaRefT                     *pOsaRef,           //  [OT]    Reference.
    OsaTimerParamsT             *pParams            //  [OP,IN] Input Parameters (see datatype for details).
    ) ;

IMPC extern OSA_STATUS  OsaTimerStop(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) ;


/***********************************************************************
 *
 * Name:        OSATimerStart
 *
 * Description: Create a Timer if needed and activate it.
 *
 * Parameters:
 *  OSTimerRef                 	timerRef          [IN]    OS supplied timer reference.
 *  UINT32                  		initialTime       	 	[IN]     initial expiration time in ms 
 *  UINT32                 	 	rescheduleTime         [IN]    timer period after initial expiration
 *  void                  		(*callBackRoutine)(UINT32)       	 	[IN]    timer call-back routine 
 *  UINT32                  		timerArgc       	[IN]    argument to be passed to call-back on expiration 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSATimerStart(
    OSTimerRef  timerRef,   
    UINT32      initialTime,    
    UINT32      rescheduleTime,
    void        (*callBackRoutine)(UINT32),
    UINT32      timerArgc 
 );
typedef struct
{
	UINT32	status;
}
OsaTimerStatusParamsT;

typedef OsaTimerStatusParamsT OSATimerStatus ;

/***********************************************************************
 *
 * Name:        OSATimerCreate
 *
 * Description: Create a Timer, if no input params the timer will be created on OsaTimerStart.
 *
 * Parameters:
 *  OSTimerRef                     *rEF    [OUT]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATimerCreate(rEF)             OsaTimerCreate(rEF,NULL)

/***********************************************************************
 *
 * Name:        OSATimerDelete
 *
 * Description: Delete timer.
 *
 * Parameters:
 *  OSTimerRef                 rEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATimerDelete(rEF)             OsaTimerDelete(rEF,NULL)


/***********************************************************************
 *
 * Name:        OSATimerStop
 *
 * Description: Stop timer.
 *
 * Parameters:
 *  OSTimerRef                 rEF          [IN]    Reference.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATimerStop(rEF)               OsaTimerStop(rEF,NULL)

/***********************************************************************
 *
 * Name:        OSATimerGetStatus
 *
 * Description: Get timer status.
 *
 * Parameters:
 *  OSTimerRef                 rEF          [IN]    Reference.
 *  OSATimerStatus                *sTATUS          [OT]    timer status.
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSATimerGetStatus(rEF,sTATUS)   OsaTimerStatus(rEF,sTATUS)


/*========================================================================
 *  OSA Memory Heap Access
 *
 *  Allocating Memory -
 *
 *  Deallocating Memory -
 *
 *========================================================================*/


/***********************************************************************
 *
 * Name:        OSAMemPoolCreate
 *
 * Description: Create a memory pool
 *
 * Parameters:
 *  OSPoolRef                 	*poolRef          [OUT]    OS assigned reference to the pool.
 *  UINT32                  		poolType       	 	[IN]    OSA_FIXED or OS_VARIABLE
 *  UINT8                 	 	*poolBase         [IN]    pointer to start of pool memory
 *  UINT32                  		poolSize      	 	[IN]    number of bytes in the memory pool  
 *  UINT32                  		partitionSize       	[IN]     size of partitions in fixed pools 
 *  UINT8                 	 	waitingMode         [IN]    mode of tasks waiting OS_FIFO, OS_PRIORITY
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMemPoolCreate (
    OSPoolRef *poolRef,         
    UINT32    poolType,         
    UINT8*    poolBase,         
    UINT32    poolSize,         
    UINT32    partitionSize,    
    UINT8     waitingMode     
 );


/***********************************************************************
 *
 * Name:        OSAMemPoolAlloc
 *
 * Description:  allocation from memory pool
 *
 * Parameters:
 *  OSPoolRef                 	poolRef          [IN]    OS assigned reference to the pool.
 *  UINT32                  		size       	 	[IN]    number of bytes to be allocated   
 *  void                 	 		**mem         [OUT]    pointer to start of allocated memory
 *  UINT32                  		timeout      	 	[IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMemPoolAlloc (
    OSPoolRef poolRef,        
    UINT32    size,           
    void**    mem,           
    UINT32    timeout           
 );


/***********************************************************************
 *
 * Name:        OSAMemPoolFree
 *
 * Description:  free memory
 *
 * Parameters:
 *  OSPoolRef                 	poolRef          [IN]    OS assigned reference to the pool. Unused, can be NULL
 *  void                 	 		*mem           [IN]     pointer to start of memory to be freed 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
IMPC extern OSA_STATUS OSAMemPoolFree (
    OSPoolRef poolRef,       
    void*     mem               
 );

/***********************************************************************
 *
 * Name:        OSAMemPoolDelete
 *
 * Description:  delete memory pool
 *
 * Parameters:
 *  OSPoolRef                 	rEF          [IN]    OS assigned reference to the pool. Unused, can be NULL
 *
 * Returns:
 *  OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define     OSAMemPoolDelete(rEF)                                       OS_SUCCESS
#if 0
#define     OSAMemPoolFixedFree(PmEM)                                   OSAMemPoolFree(NULL,PmEM)
#define     OSAMemPoolCreateGlobal(rEF,nAME,bASE,pOOLsIZE, pARTsIZE)    OSAMemPoolCreate(rEF,OSA_GLOBAL,bASE,pOOLsIZE, pARTsIZE,OS_PRIORITY)
#endif

/***********************************************************************
 *
 * Name:        OsaPartitionPoolCreate
 *
 * Description: Create Partition Pool.
 *
 * Parameters:
 *  OSAPartitionPoolRef         *rEF       	 	[OUT]    OS assigned reference to the pool
 *  char                 	 		*nAME         [IN]    pool name
 *  void                  		*sTARTaDDR      	 	[IN]     pointer to start of pool memory 
 *  UINT32                  		pOOLsIZE       	[IN]      number of bytes in the memory pool 
 *  UINT32                  		pARTsIZE       	[IN]     size of partitions in fixed pools 
 *  UINT8                 	 	sUSPEND         [IN]    mode of tasks waiting OS_FIFO, OS_PRIORITY
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/


/**********************************************
 * get the MMI pool reference
 *
 **********************************************/
IMPC extern OsaRefT GetMMIPool(
    void
) ;

#define OSAGetMemPoolFreeSize(poolRef)					OsaGetMemPoolFreeSize(poolRef)

#define OSAPartitionPoolCreate(rEF, nAME, sTARTaDDR, pOOLsIZE, pARTsIZE, sUSPEND )		OsaPartitionPoolCreate(rEF, nAME, sTARTaDDR, pOOLsIZE, pARTsIZE, sUSPEND)

/***********************************************************************
 *
 * Name:        OSAPartitionAllocate
 *
 * Description: allocation a block from Partition Pool.
 *
 * Parameters:
 *  OSAPartitionPoolRef         *rEF       	 	[IN]    OS assigned reference to the pool
 *  void                  		**pBLOCK       	[OUT]      pointer to start of allocated block 
 *  UINT8                 	 	sUSPEND         [IN]    OS_SUSPEND, OS_NO_SUSPEND, or timeout
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define OSAPartitionAllocate(rEF, pBLOCK, sUSPEND)										OsaPartitionAllocate(rEF, pBLOCK, sUSPEND)


/***********************************************************************
 *
 * Name:        OSAPartitionFree
 *
 * Description: free the block
 *
 * Parameters:
 *  void                  		*pBLOCK       	[IN]      the block to be freedl 
 *
 * Returns:
 *  OSA_STATUS  OSA Complition Code.
 *
 * Notes:
 *
 ***********************************************************************/
#define OSAPartitionFree(pBLOCK)														OsaPartitionFree(pBLOCK)

		
/* Remap old names to new ones to remain backwards compatibility with old names.
 */
#define OS_NULL                OSA_NULL
#define OS_TIMER_DEAD          OSA_TIMER_DEAD
#define OS_TIMER_CREATED       OSA_TIMER_CREATED
#define OS_TIMER_ACTIVE        OSA_TIMER_ACTIVE
#define OS_TIMER_INACTIVE      OSA_TIMER_INACTIVE
#define OS_ENABLED             OSA_ENABLED
#define OS_DISABLED            OSA_DISABLED
#define OS_SUSPEND             OSA_SUSPEND
#define OS_NO_SUSPEND          OSA_NO_SUSPEND
#define OS_FLAG_AND            OSA_FLAG_AND
#define OS_FLAG_AND_CLEAR      OSA_FLAG_AND_CLEAR
#define OS_FLAG_OR             OSA_FLAG_OR
#define OS_FLAG_OR_CLEAR       OSA_FLAG_OR_CLEAR
#define OS_LOWEST_PRIORITY     OSA_LOWEST_PRIORITY
#define OS_HIGHEST_PRIORITY    OSA_HIGHEST_PRIORITY
#define OS_POOL_MEM_OVERHEAD   OSA_POOL_MEM_OVERHEAD

#define OS_FIXED               OSA_FIXED
#define OS_VARIABLE            OSA_VARIABLE
#define OS_FIFO                OSA_FIFO
#define OS_PRIORITY            OSA_PRIORITY

#define OSInitialize            OSAInitialize
#define OSRun                   OSARun
#define OSTaskCreate            OSATaskCreate
#define OSTaskCreateEx          OSATaskCreateEx
#define OSTaskDelete            OSATaskDelete
#define OSTaskPriority          OSATaskChangePriority
#define OSTaskGetPriority       OSATaskGetPriority
#define OSTaskSuspend           OSATaskSuspend
#define OSTaskResume            OSATaskResume
#define OSTaskSleep             OSATaskSleep
#define OSTaskYield             OSATaskYield
#define OSFlagCreate            OSAFlagCreate
#define OSFlagDelete            OSAFlagDelete
#define OSFlagSet               OSAFlagSet
#define OSFlagWait              OSAFlagWait
#define OSFlagPeek              OSAFlagPeek
#define OSTimerCreate           OSATimerCreate
#define OSTimerDelete           OSATimerDelete
#define OSTimerStart            OSATimerStart
#define OSTimerStop             OSATimerStop
#define OSTimerGetStatus        OSATimerGetStatus
#define OSMsgQCreate            OSAMsgQCreate
#define OSMsgQDelete            OSAMsgQDelete
#define OSMsgQSend              OSAMsgQSend
#define OSMsgQRecv              OSAMsgQRecv
#define OSMsgQPoll              OSAMsgQPoll
#define OSMailboxQCreate        OSAMailboxQCreate
#define OSMailboxQDelete        OSAMailboxQDelete
#define OSMailboxQSend          OSAMailboxQSend
#define OSMailboxQRecv          OSAMailboxQRecv
#define OSMailboxQPoll          OSAMailboxQPoll
#define OSSemaphoreCreate       OSASemaphoreCreate
#define OSSemaphoreDelete       OSASemaphoreDelete
#define OSSemaphoreAcquire      OSASemaphoreAcquire
#define OSSemaphoreRelease      OSASemaphoreRelease
#define OSSemaphorePoll         OSASemaphorePoll
#define OSMutexCreate           OSAMutexCreate
#define OSMutexDelete           OSAMutexDelete
#define OSMutexLock             OSAMutexLock
#define OSMutexUnlock           OSAMutexUnlock
#define OSIsrCreate             OSAIsrCreate
#define OSIsrDelete             OSAIsrDelete
#define OSIsrEnable             OSAIsrEnable
#define OSIsrDisable            OSAIsrDisable
#define OSIsrNotify             OSAIsrNotify
#define OSGetTicks              OSAGetTicks
#define OSGetClockRate          OSAGetClockRate
#define OSTick                  OSATick
#define OSMemPoolCreate         OSAMemPoolCreate
#define OSMemPoolDelete         OSAMemPoolDelete
#define OSMemPoolAlloc          OSAMemPoolAlloc
#define OSMemPoolFree           OSAMemPoolFree
#define OSContextLock           OSAContextLock
#define OSContextUnlock         OSAContextUnlock

typedef enum 
{
	OSA_TASK_READY,
	OSA_TASK_COMPLETED,
	OSA_TASK_TERMINATED,
	OSA_TASK_SUSPENDED,
	OSA_TASK_SLEEP,
	OSA_TASK_QUEUE_SUSP,
	OSA_TASK_SEMAPHORE_SUSP,
	OSA_TASK_EVENT_FLAG,
	OSA_TASK_BLOCK_MEMORY,
	OSA_TASK_STATE_UNKNOWN,
}OSA_TASK_STATE;

typedef struct OSA_TASK_STRUCT
{
    char                *task_name;                /* Pointer to thread's name  */		
    unsigned int        task_priority;             /* Priority of thread (0-255)*/	
    unsigned long       task_stack_def_val;        /* default vaule of  thread  */
    OSA_TASK_STATE      task_state;                /* Thread's execution state  */
    unsigned long       task_stack_ptr;            /* Thread's stack pointer    */
    unsigned long       task_stack_start;          /* Stack starting address    */
    unsigned long       task_stack_end;            /* Stack ending address      */
    unsigned long       task_stack_size;           /* Stack size                */	
    unsigned long       task_run_count;            /* Thread's run counter      */

} OSA_TASK;

IMPC extern OSA_STATUS OsaGetTaskInfo(OsaRefT OsaRef, OSA_TASK *task_info);

IMPC extern OSA_STATUS OsaGetCurrentThreadRef(
	OsaRefT                 *pOsaRef,           //  [OT]    Reference.
    void                    *pForFutureUse
  );
IMPC extern OSA_STATUS OsaGetThreadListHead(
	OsaRefT *pListHead, //  [OT]    Reference.
	void *pForFutureUse 
);

IMPC extern OSA_STATUS OsaGetCreatedThreadCount(
	unsigned long *count, //  [OT]    count
	void *pForFutureUse 
);

IMPC extern OSA_STATUS OsaGetMaxThreadCount(
	unsigned long *maxCnt, //  [OT]	  count
	void *pForFutureUse 
);
IMPC extern unsigned long OsaThreadList(OsaRefT *ptrList, unsigned long maximum_pointers,void *pForFutureUse);

extern void OSAGetTaskInfo(OSATaskRef taskRef, OSA_TASK *task_info);

/***********************************************************************
 *
 * Name:        OSAGetCurrentThreadRef
 *
 * Description: Get Current thread Ref.
 *
 * Parameters:
 *  OSATaskRef                 *PoSArEF    [OUT]    thread Reference.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAGetCurrentThreadRef(PoSArEF)				OsaGetCurrentThreadRef(PoSArEF,NULL)


/***********************************************************************
 *
 * Name:        OSAGetTaskInfo
 *
 * Description: Get the detailed information of specific thread
 *
 * Parameters:
 *  OSATaskRef                 oSArEF          [IN]    thread Reference.
 *  OSA_TASK                   *pTaskInfo      [OUT]    structure of thread info.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAGetTaskInfo(oSArEF, pTaskInfo)				OsaGetTaskInfo(oSArEF,pTaskInfo)

/***********************************************************************
 *
 * Name:        OSAGetThreadListHead
 *
 * Description: Get the head of created thread reference list
 *
 * Parameters:
 *  OSATaskRef                 *pListHead    [OUT]   list head.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAGetThreadListHead(pListHead)				OsaGetThreadListHead(pListHead,NULL)

/***********************************************************************
 *
 * Name:        OSAGetCreatedThreadCount
 *
 * Description: Get the count of created thread
 * Parameters:
 *  unsigned long                 *pCount    [OUT]   out.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAGetCreatedThreadCount(pCount)				OsaGetCreatedThreadCount(pCount,NULL)

/***********************************************************************
 *
 * Name:        OSAGetMaxThreadCount
 *
 * Description: Get the max number of thread that system can be created 
 * Parameters:
 *  unsigned long                 *pCount    [OUT]   out.
 *
 * Returns:
 *      OS_SUCCESS
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAGetMaxThreadCount(pCount)				OsaGetMaxThreadCount(pCount,NULL)


/***********************************************************************
 *
 * Name:        OSAThreadList
 *
 * Description: Get the reference list of created thread 
 * Parameters:
 *  OSATaskRef                 *ptrList          [OUT]    thread Reference list.
 *  unsigned long               maximum_pointerst    [IN]   length of array
 *
 * Returns:
 *      unsigned long        Number of threads placed in list
 *
 * Notes:
 *
 ***********************************************************************/
#define		OSAThreadList(ptrList,maximum_pointers)				OsaThreadList(ptrList,maximum_pointers,NULL)


#ifdef __cplusplus
}
#endif

#endif /* __OSA_API_H__ */
