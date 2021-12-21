/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2007 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_thread.h                                         PORTABLE C      */ 
/*                                                           5.1          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX thread control component, including   */ 
/*    data types and external references.  It is assumed that tx_api.h    */
/*    and tx_port.h have already been included.                           */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-12-2005     William E. Lamie         Initial Version 5.0           */ 
/*  04-02-2007     William E. Lamie         Modified comment(s),          */ 
/*                                            replaced UL constant        */ 
/*                                            modifier with ULONG cast,   */ 
/*                                            added logic to use preset   */ 
/*                                            global C data, resulting    */ 
/*                                            in version 5.1              */ 
/*                                                                        */ 
/**************************************************************************/ 

#include "__cp_tx_api.h"

#ifndef TX_THREAD_H
#define TX_THREAD_H


/* Define thread control specific data definitions.  */

#define TX_THREAD_ID                            ((ULONG) 0x54485244)
#define TX_THREAD_MAX_BYTE_VALUES               256
#define TX_THREAD_PRIORITY_GROUP_MASK           ((ULONG) 0xFF)
#define TX_THREAD_PRIORITY_GROUP_SIZE           8
#define TX_THREAD_EXECUTE_LOG_SIZE              8


/* Define priority/preempt maps index constant if only 32 priority 
   levels are supported.  Otherwise, an index variable is needed
   and the priority must be modified.  */

#if TX_MAX_PRIORITIES > 32
#define MAP_INDEX                               map_index
#define MOD32(a)                                (a%32)
#else
#define MAP_INDEX                               0
#define MOD32(a)                                (a)
#endif


/* Define thread control function prototypes.  */

VOID        _tx_thread_context_save(VOID);
VOID        _tx_thread_context_restore(VOID);
UINT        _tx_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start);
UINT        _tx_thread_delete(TX_THREAD *thread_ptr);
UINT        _tx_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *, UINT));
TX_THREAD  *_tx_thread_identify(VOID);
UINT        _tx_thread_info_get(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, 
                UINT *priority, UINT *preemption_threshold, ULONG *time_slice, 
                TX_THREAD **next_thread, TX_THREAD **next_suspended_thread);
VOID        _tx_thread_initialize(VOID);
UINT        _tx_thread_interrupt_control(UINT new_posture);
UINT        _tx_thread_performance_info_get(TX_THREAD *thread_ptr, ULONG *resumptions, ULONG *suspensions, 
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, TX_THREAD **last_preempted_by);
UINT        _tx_thread_performance_system_info_get(ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts,
                ULONG *non_idle_returns, ULONG *idle_returns);
UINT        _tx_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        _tx_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        _tx_thread_relinquish(VOID);
UINT        _tx_thread_reset(TX_THREAD *thread_ptr);
UINT        _tx_thread_resume(TX_THREAD *thread_ptr);
VOID        _tx_thread_schedule(VOID);
VOID        _tx_thread_shell_entry(VOID);
UINT        _tx_thread_sleep(ULONG timer_ticks);
VOID        _tx_thread_stack_analyze(TX_THREAD *thread_ptr);
VOID        _tx_thread_stack_build(TX_THREAD *thread_ptr, VOID (*function_ptr)(VOID));
VOID        _tx_thread_stack_error(TX_THREAD *thread_ptr);
VOID        _tx_thread_stack_error_handler(TX_THREAD *thread_ptr);
UINT        _tx_thread_stack_error_notify(VOID (*stack_error_handler)(TX_THREAD *));
VOID        _tx_thread_system_preempt_check(VOID);
VOID        _tx_thread_system_resume(TX_THREAD *thread_ptr);
VOID        _tx_thread_system_return(VOID);
VOID        _tx_thread_system_suspend(TX_THREAD *thread_ptr);
UINT        _tx_thread_suspend(TX_THREAD *thread_ptr);
UINT        _tx_thread_terminate(TX_THREAD *thread_ptr);
UINT        _tx_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);
VOID        _tx_thread_time_slice(VOID);
VOID        _tx_thread_timeout(ULONG timeout_input);
UINT        _tx_thread_wait_abort(TX_THREAD *thread_ptr);


/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start, UINT thread_control_block_size);
UINT        _txe_thread_delete(TX_THREAD *thread_ptr);
UINT        _txe_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *, UINT));
TX_THREAD  *_tx_thread_identify(VOID);
UINT        _txe_thread_info_get(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, 
                UINT *priority, UINT *preemption_threshold, ULONG *time_slice, 
                TX_THREAD **next_thread, TX_THREAD **next_suspended_thread);
UINT        _txe_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        _txe_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        _txe_thread_relinquish(VOID);
UINT        _txe_thread_reset(TX_THREAD *thread_ptr);
UINT        _txe_thread_resume(TX_THREAD *thread_ptr);
UINT        _txe_thread_suspend(TX_THREAD *thread_ptr);
UINT        _txe_thread_terminate(TX_THREAD *thread_ptr);
UINT        _txe_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);
UINT        _txe_thread_wait_abort(TX_THREAD *thread_ptr);


/* Thread control component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef TX_THREAD_INIT
#define THREAD_DECLARE 
#else
#define THREAD_DECLARE extern
#endif

//THREAD_DECLARE	VOID *			_tx_irq_processing_return;		

/* Define the pointer that contains the system stack pointer.  This is
   utilized when control returns from a thread to the system to reset the
   current stack.  This is setup in the low-level initialization function. */

THREAD_DECLARE  VOID *          _tx_thread_system_stack_ptr;


/* Define the current thread pointer.  This variable points to the currently
   executing thread.  If this variable is NULL, no thread is executing.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_current_ptr;


/* Define the variable that holds the next thread to execute.  It is important
   to remember that this is not necessarily equal to the current thread 
   pointer.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_execute_ptr;


/* Define the head pointer of the created thread list.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_created_ptr;


/* Define the variable that holds the number of created threads. */

THREAD_DECLARE  ULONG           _tx_thread_created_count;


/* Define the current state variable.  When this value is 0, a thread
   is executing or the system is idle.  Other values indicate that 
   interrupt or initialization processing is active.  This variable is
   initialized to TX_INITIALIZE_IN_PROGRESS to indicate initialization is
   active.  */

#ifdef TX_THREAD_INIT
THREAD_DECLARE  volatile ULONG  _tx_thread_system_state =  TX_INITIALIZE_IN_PROGRESS;
#else
THREAD_DECLARE  volatile ULONG  _tx_thread_system_state;
#endif


/* Define the 32-bit priority bit-maps. There is one priority bit map for each
   32 priority levels supported. If only 32 priorities are supported there is 
   only one bit map. Each bit within a priority bit map represents that one 
   or more threads at the associated thread priority are ready.  */ 

THREAD_DECLARE  ULONG           _tx_thread_priority_maps[TX_MAX_PRIORITIES/32];


/* Define the priority map active bit map that specifies which of the previously 
   defined priority maps have something set. This is only necessary if more than 
   32 priorities are supported.  */

#if TX_MAX_PRIORITIES > 32
THREAD_DECLARE  ULONG           _tx_thread_priority_map_active;
#endif


#ifndef TX_DISABLE_PREEMPTION_THRESHOLD

/* Define the 32-bit preempt priority bit maps.  There is one preempt bit map 
   for each 32 priority levels supported. If only 32 priorities are supported 
   there is only one bit map. Each set set bit corresponds to a preempted priority 
   level that had preemption-threshold active to protect against preemption of a 
   range of relatively higher priority threads.  */

THREAD_DECLARE  ULONG           _tx_thread_preempted_maps[TX_MAX_PRIORITIES/32];


/* Define the preempt map active bit map that specifies which of the previously 
   defined preempt maps have something set. This is only necessary if more than 
   32 priorities are supported.  */

#if TX_MAX_PRIORITIES > 32
THREAD_DECLARE  ULONG           _tx_thread_preempted_map_active;
#endif
#endif

/* Define the variable that holds the highest priority group ready for 
   execution.  It is important to note that this is not necessarily the same
   as the priority of the thread pointed to by _tx_execute_thread.  */

THREAD_DECLARE  UINT            _tx_thread_highest_priority;


/* Define the array that contains the lowest set bit of any 8-bit pattern.  
   Portions of the priority map are used as indexes to quickly find
   the next priority group ready for execution.  This table is initialized
   in the thread control initialization processing.  */

#ifdef TX_USE_PRESET_DATA

#ifdef TX_THREAD_INIT
UCHAR  _tx_thread_lowest_bit[TX_THREAD_MAX_BYTE_VALUES] =
            {00, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 04, 00, 01, 00, 02, 00, 01, 00, 03, 00, 
             01, 00, 02, 00, 01, 00, 05, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 04, 00, 01, 00, 
             02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 06, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 
             01, 00, 04, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 05, 00, 01, 00, 02, 00, 01, 00, 
             03, 00, 01, 00, 02, 00, 01, 00, 04, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 07, 00, 
             01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 04, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 
             02, 00, 01, 00, 05, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 04, 00, 01, 00, 02, 00, 
             01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 06, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 
             04, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00, 05, 00, 01, 00, 02, 00, 01, 00, 03, 00, 
             01, 00, 02, 00, 01, 00, 04, 00, 01, 00, 02, 00, 01, 00, 03, 00, 01, 00, 02, 00, 01, 00};
#else
THREAD_DECLARE  UCHAR           _tx_thread_lowest_bit[TX_THREAD_MAX_BYTE_VALUES];
#endif
#else

THREAD_DECLARE  UCHAR           _tx_thread_lowest_bit[TX_THREAD_MAX_BYTE_VALUES];
#endif


/* Define the array of thread pointers.  Each entry represents the threads that
   are ready at that priority group.  For example, index 10 in this array
   represents the first thread ready at priority 10.  If this entry is NULL,
   no threads are ready at that priority.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_priority_list[TX_MAX_PRIORITIES];


/* Define the global preempt disable variable.  If this is non-zero, preemption is
   disabled.  It is used internally by ThreadX to prevent preemption of a thread in 
   the middle of a service that is resuming or suspending another thread.  */

THREAD_DECLARE  volatile UINT   _tx_thread_preempt_disable;


/* Define the global build options variable.  This contains a bit map representing
   how the ThreadX library was built. The following are the bit field definitions:

                    Bit(s)                   Meaning

                    31-30               Reserved
                    29-24               Priority groups 1  -> 32 priorities
                                                        2  -> 64 priorities
                                                        3  -> 96 priorities

                                                        ...

                                                        32 -> 1024 priorities
                    23                  TX_TIMER_PROCESS_IN_ISR defined
                    22                  TX_REACTIVATE_INLINE defined
                    21                  TX_DISABLE_STACK_FILLING defined
                    20                  TX_ENABLE_STACK_CHECKING defined
                    19                  TX_DISABLE_PREEMPTION_THRESHOLD defined
                    18                  TX_DISABLE_REDUNDANT_CLEARING defined
                    17                  TX_DISABLE_NOTIFY_CALLBACKS defined
                    16                  TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO defined
                    15                  TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO defined
                    14                  TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO defined
                    13                  TX_MUTEX_ENABLE_PERFORMANCE_INFO defined
                    12                  TX_QUEUE_ENABLE_PERFORMANCE_INFO defined
                    11                  TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO defined
                    10                  TX_THREAD_ENABLE_PERFORMANCE_INFO defined
                    9                   TX_TIMER_ENABLE_PERFORMANCE_INFO defined
                    8                   TX_ENABLE_EVENT_TRACE defined
                    7-0                 Port Specific   */

THREAD_DECLARE  ULONG           _tx_build_options;


#ifdef TX_ENABLE_STACK_CHECKING

/* Define the global function pointer for stack error handling. If a stack error is 
   detected and the application has registered a stack error handler, it will be 
   called via this function pointer.  */

THREAD_DECLARE  VOID            (*_tx_thread_application_stack_error_handler)(TX_THREAD *);

#endif

#ifdef TX_THREAD_ENABLE_PERFORMANCE_INFO

/* Define the total number of thread resumptions. Each time a thread enters the
   ready state this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_resume_count;


/* Define the total number of thread suspensions. Each time a thread enters a 
   suspended state this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_suspend_count;


/* Define the total number of solicited thread preemptions. Each time a thread is 
   preempted by directly calling a ThreadX service, this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_solicited_preemption_count;


/* Define the total number of interrupt thread preemptions. Each time a thread is 
   preempted as a result of an ISR calling a ThreadX service, this variable is 
   incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_interrupt_preemption_count;


/* Define the total number of priority inversions. Each time a thread is blocked by
   a mutex owned by a lower-priority thread, this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_priority_inversion_count;


/* Define the total number of time-slices.  Each time a time-slice operation is 
   actually performed (another thread is setup for running) this variable is 
   incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_time_slice_count;


/* Define the total number of thread relinquish operations.  Each time a thread 
   relinquish operation is actually performed (another thread is setup for running)
   this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_relinquish_count;


/* Define the total number of thread timeouts. Each time a thread has a 
   timeout this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_timeout_count;


/* Define the total number of thread wait aborts. Each time a thread's suspension 
   is lifted by the tx_thread_wait_abort call this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_wait_abort_count;


/* Define the total number of idle system thread returns. Each time a thread returns to 
   an idle system (no other thread is ready to run) this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_idle_return_count;


/* Define the total number of non-idle system thread returns. Each time a thread returns to 
   a non-idle system (another thread is ready to run) this variable is incremented.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_non_idle_return_count;


/* Define the last TX_THREAD_EXECUTE_LOG_SIZE threads scheduled in ThreadX. This 
   is a circular list, where the index points to the oldest entry.  */

THREAD_DECLARE  ULONG           _tx_thread_performance_execute_log_index;
THREAD_DECLARE  TX_THREAD *     _tx_thread_performance_execute_log[TX_THREAD_EXECUTE_LOG_SIZE];

#endif


#ifdef TX_THREAD_INIT
const CHAR _tx_thread_special_string[] = 
  "G-BL-N-GB-GL-M-D-DL-KML-CMR-HMR-ML2-GZ-KH2-EL-CM-RP-TC-NH-TD-AP-HA-GF-DD-AT-MF-MS-DW-USA-CA-SD-SDSU";
#endif

#endif

