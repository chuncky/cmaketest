#include "common.h"
#include "bsp.h"
#include "os_hal.h"


#define USB_POOL_SIZE  8*1024

static struct rtdm_pcb USBMemoryPool;
extern  UINT8 USBPoolArray[];

OSA_STATUS  OSATimerCreate(
    OsaRefT                     *pOsaRef,           //  [OT]    Reference.
    OsaTimerParamsT             *pParams            //  [OP,IN] Input Parameters (see datatype for details).
    ) 
{
	return OS_SUCCESS;
}

OSA_STATUS  OSATimerStop(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    void                        *pForFutureUse
    ) 
{
	return OS_SUCCESS;
}



OSA_STATUS OSATimerStart(
    OSTimerRef  timerRef,   
    UINT32      initialTime,    
    UINT32      rescheduleTime,
    void        (*callBackRoutine)(UINT32),
    UINT32      timerArgc 
 )
 {
	 
	 return OS_SUCCESS;
	 
 }



INT32 OS_Activate_HISR(VOID** hisr)
{
	uart_printf("%s-01\r\n",__func__);
	return 0;
}

void OS_Create_HISR(VOID** hisr, char* name, VOID (*hisr_entry)(VOID), unsigned char priority)
{
}



OSA_STATUS  OsaFlagCreate(
    OsaRefT                     *pOsaRef,           //  [OT]    Reference.
    OsaFlagCreateParamsT        *pParams            //  [OP,IN] Input Parameters (see datatype for details).
    ) 
{

	return OS_SUCCESS;
}	


OSA_STATUS  OSAFlagSet(
    OsaRefT                     OsaRef,             //  [IN]    Reference.
    UINT32                      mask,               //  [IN]    Flag mask.
    UINT32                      operation,          //  [IN]    OSA_FLAG_AND, OSA_FLAG_OR.
    void                        *pForFutureUse
    ) 
{

	return OS_SUCCESS;

}	



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
OSA_STATUS OSAFlagWait(
    OSFlagRef   flagRef,        
    UINT32      mask,          
    UINT32      operation,                         
    UINT32      *flags,       
    UINT32      timeout        
 )
 {
	return OS_SUCCESS;
 }


UINT32 OSAGetTicks(void)
{
	return 0;
}

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
 OSA_STATUS OSAMsgQCreate(
    OSMsgQRef   *msgQRef,       
#if defined (OSA_QUEUE_NAMES)
    char        *queueName,     
#endif
    UINT32      maxSize,       
    UINT32      maxNumber,      
    UINT8       waitingMode    
 )
 {
	 return 0;
 }

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
OSA_STATUS OSAMsgQRecv (
    OSMsgQRef   msgQRef,       
    UINT8       *recvMsg,       
    UINT32      size,          
    UINT32      timeout         
 )
 {


	 return 0;
 }




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
OSA_STATUS OSAMsgQSend (
    OSMsgQRef   msgQRef,       
    UINT32      size,           
    UINT8       *msgPtr,        
    UINT32      timeout       
 )
 {

	 return 0;

 }

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
OSA_STATUS OSATaskCreate(
    OSATaskRef* taskRef,     
    void*       stackPtr,  
    UINT32      stackSize,      
    UINT8       priority,       
    CHAR        *taskName,      
    void        (*taskStart)(void*), 
    void*       argv            
 )
 {
	 return 0;
 }

void    OsaTaskSleep(
    UINT32                  ticks,              //  [IN]    Time to sleep in ticks.
    void                    *pForFutureUse
    ) 
{

}	
	
	
/*
 * The initialized memory pool will be:
 * +-----------------------------------+--------------------------+
 * | whole freed memory block          | Used Memory Block Tailer |
 * +-----------------------------------+--------------------------+
 *
 * rtdm_block_list --> whole freed memory block
 *
 * The lenght of Used Memory Block Tailer is 0, which is prevents block merging accross list
 */
rt_status_t rtdm_memory_pool_init(struct rtdm_pcb* pool_ptr,
	void *start_addr,
	rt_uint32_t size,
	rt_uint32_t min_allocation)
{
	struct rtdm_header *header_ptr;
	
	RT_ASSERT(pool_ptr != RT_NULL);

	/* init pool pcb */
	pool_ptr->rtdm_start_address = start_addr;
	pool_ptr->rtdm_pool_size = (size / sizeof(rt_uint32_t)) * sizeof(rt_uint32_t);
	pool_ptr->rtdm_mini_allocation = RTDM_ALIGN(min_allocation);
    pool_ptr->rtdm_available = pool_ptr->rtdm_pool_size - (2 * RTDM_OVERHEAD);

#if defined(PLAT_USE_THREADX) || defined(PLAT_USE_ALIOS)
//    OSAMutexCreate(&pool_ptr->rtdm_protect, OSA_FIFO);
#else
	/* Initialize the protection structure.  */
    pool_ptr->rtdm_protect.tc_tcb_pointer = RT_NULL;
#endif
	/* init the free list header */
	header_ptr = &(pool_ptr->rtdm_free_header);
    header_ptr->magic = RTDM_HEADER_MAGIC;
    header_ptr->rtdm_memory_pool = pool_ptr;
	header_ptr->next_memory = RT_NULL;
	header_ptr->prev_memory = RT_NULL;
	header_ptr->next_free_memory = header_ptr;
	header_ptr->prev_free_memory = header_ptr;

	/* set the free list to free list header */
	pool_ptr->rtdm_free_list = header_ptr;

	/* init the first big memory block */
	header_ptr = (struct rtdm_header*) start_addr;
    header_ptr->magic = RTDM_HEADER_MAGIC;
    header_ptr->rtdm_memory_pool = pool_ptr;
    header_ptr->next_memory = (struct rtdm_header *)
           ((rt_uint8_t*) header_ptr + pool_ptr->rtdm_available + RTDM_OVERHEAD);
    header_ptr->prev_memory =  header_ptr->next_memory;

	/* block list header */
	pool_ptr->rtdm_block_list = header_ptr;

	/* place the big memory block to free list */
	header_ptr->next_free_memory = pool_ptr->rtdm_free_list->next_free_memory;
	header_ptr->prev_free_memory = pool_ptr->rtdm_free_list;
	pool_ptr->rtdm_free_list->next_free_memory->prev_free_memory = header_ptr;
	pool_ptr->rtdm_free_list->next_free_memory = header_ptr;

	/* move to the end of memory pool to build a small tailer block, which prevents block merging */
    header_ptr =  header_ptr->next_memory;
	/* it's a used  memory block */
    header_ptr->magic = RTDM_HEADER_MAGIC | RTDM_HEADER_USED;
    header_ptr->rtdm_memory_pool = pool_ptr;
    header_ptr->next_memory = (struct rtdm_header *) start_addr;
    header_ptr->prev_memory = (struct rtdm_header *) start_addr;
	/* not in free list */
    header_ptr->next_free_memory = header_ptr->prev_free_memory = RT_NULL;

	RTDM_TRACE("memory pool: start addr 0x%08x, size %d, free list header 0x%08x", start_addr, 
		size, 
		&(pool_ptr->rtdm_free_header));

    return (RT_STATUS_OK);
}

rt_status_t rtdm_memory_pool_delete(struct rtdm_pcb* pool_ptr)
{
    /* Return a successful completion.  */
    return(RT_STATUS_OK);
}

extern UINT32 EEHandlerFlag;
rt_status_t rtdm_memory_allocate(struct rtdm_pcb *pool_ptr, void **return_pointer,
	rt_uint32_t size)
{
	rt_status_t status;
	rt_uint32_t free_size;
	struct rtdm_header *header_ptr = NULL;
	unsigned long cpsr=0;

	RT_ASSERT(pool_ptr != RT_NULL);

	/* set init status as OK */
	status = RT_STATUS_OK;

	/* align alloated size */
	size = RTDM_ALIGN(size);
	if (size < pool_ptr->rtdm_mini_allocation) size = pool_ptr->rtdm_mini_allocation;

#if defined(PLAT_USE_THREADX) || defined(PLAT_USE_ALIOS)
	cpsr = disableInterrupts();
#else
    /* Protect against simultaneous access to the memory pool.  */
	if(EEHandlerFlag==0)
    	TCT_Protect(&(pool_ptr->rtdm_protect));
	else
		cpsr = disableInterrupts();
#endif
		

	RTDM_TRACE("%s %d\n", __FUNCTION__, size);
	
	if (size < pool_ptr->rtdm_available)
	{
		/* search on free list */
		free_size = 0;
		/* get the first free memory block */
		header_ptr = pool_ptr->rtdm_free_list->next_free_memory;
		while (header_ptr != pool_ptr->rtdm_free_list && free_size < size)
		{
			/* get current freed memory block size */
			free_size = (rt_uint32_t)(header_ptr->next_memory) - (rt_uint32_t)header_ptr - RTDM_OVERHEAD;

			if (free_size < size)
			{
				/* move to next free memory block */
				header_ptr = header_ptr->next_free_memory;
			}
		}

		/* determine if the memory is available.  */
		if (free_size >= size)
		{
			/* a block that satisfies the request has been found.  */

			/* determine if the block needs to be split.  */
			if (free_size >= (size + RTDM_OVERHEAD + pool_ptr->rtdm_mini_allocation))
			{
				struct rtdm_header* new_ptr;

				/* split the block.  */
				new_ptr =  (struct rtdm_header*) (((rt_uint8_t*) header_ptr) + size + RTDM_OVERHEAD);

				RTDM_TRACE("split: h[0x%08x] nm[0x%08x] pm[0x%08x] to n[0x%08x]", header_ptr, 
					header_ptr->next_memory, header_ptr->prev_memory,
					new_ptr);

				/* mark the new block as a memory block and freed. */
				new_ptr->magic = RTDM_HEADER_MAGIC;

				/* put the pool pointer into the new block.  */
				new_ptr->rtdm_memory_pool = pool_ptr;

				/* break down the block list */
				new_ptr->prev_memory = header_ptr;
				new_ptr->next_memory = header_ptr->next_memory;
				header_ptr->next_memory->prev_memory = new_ptr;
				header_ptr->next_memory = new_ptr;

				/* remove header ptr from free list */
				header_ptr->next_free_memory->prev_free_memory = header_ptr->prev_free_memory;
				header_ptr->prev_free_memory->next_free_memory = header_ptr->next_free_memory;
				header_ptr->next_free_memory = RT_NULL;
				header_ptr->prev_free_memory = RT_NULL;

				/* insert new_ptr to free list */
				new_ptr->next_free_memory = pool_ptr->rtdm_free_list->next_free_memory;
				new_ptr->prev_free_memory = pool_ptr->rtdm_free_list;
				pool_ptr->rtdm_free_list->next_free_memory->prev_free_memory = new_ptr;
				pool_ptr->rtdm_free_list->next_free_memory = new_ptr;
				RTDM_TRACE("new ptr: nf 0x%08x, pf 0x%08x", new_ptr->next_free_memory, new_ptr->prev_free_memory);

				/* decrement the available byte count.  */
				pool_ptr->rtdm_available = pool_ptr->rtdm_available - size - RTDM_OVERHEAD;
			}
			else
			{
				/* decrement the entire free size from the available bytes count.  */
				pool_ptr->rtdm_available =  pool_ptr->rtdm_available - free_size;

				/* remove header_ptr from free list */
				RTDM_TRACE("one block: h[0x%08x], nf 0x%08x, pf 0x%08x", header_ptr, 
					header_ptr->next_free_memory, header_ptr->prev_free_memory);

				header_ptr->next_free_memory->prev_free_memory = header_ptr->prev_free_memory;
				header_ptr->prev_free_memory->next_free_memory = header_ptr->next_free_memory;
				header_ptr->next_free_memory = RT_NULL;
				header_ptr->prev_free_memory = RT_NULL;
			}

			/* Mark the allocated block as not available.  */
			header_ptr->magic |=  RTDM_HEADER_USED;

			/* Return a memory address to the caller.  */
			*return_pointer =  (void*) ((rt_uint8_t*)header_ptr + RTDM_OVERHEAD);
		}
		else
		{
			/* Enough dynamic memory is not available.  Determine if suspension is
			   required. */
			status = RT_STATUS_ERROR;
			*return_pointer = RT_NULL;
		}
	}
	else
	{
		/* Enough dynamic memory is not available.  Determine if suspension is
		   required. */
		status = RT_STATUS_ERROR;
		*return_pointer = RT_NULL;
	}

	RTDM_TRACE("am: m[0x%08x], h[0x%08x], size: %d\n", *return_pointer, header_ptr, size);

#if defined(PLAT_USE_THREADX) || defined(PLAT_USE_ALIOS)
//	OSAMutexUnlock(pool_ptr->rtdm_protect);
	restoreInterrupts(cpsr);
#else
	/* Release protection of the memory pool.  */
	if(EEHandlerFlag==0)
    	TCT_Unprotect();
	else
		restoreInterrupts(cpsr);
#endif
    /* Return the completion status.  */
    return(status);
}

rt_status_t rtdm_memory_free(void* memory)
{
	struct rtdm_pcb *pool_ptr;
	struct rtdm_header *header_ptr, *new_ptr;
	rt_status_t status;
	rt_uint32_t insert_header;
	unsigned long cpsr=0;
	/* set init status as OK */
	status = RT_STATUS_OK; insert_header = 1;
	new_ptr = RT_NULL;
	header_ptr = (struct rtdm_header*)((rt_uint8_t*)memory - RTDM_OVERHEAD);

	RTDM_TRACE("fm: m[0x%08x], h[0x%08x]", memory, header_ptr);

	/* check magic */
	RT_ASSERT((header_ptr->magic & RTDM_HEADER_MAGIC_MASK) == RTDM_HEADER_MAGIC);

	/* get pool ptr */
	pool_ptr = header_ptr->rtdm_memory_pool;

#if defined(PLAT_USE_THREADX) || defined(PLAT_USE_ALIOS)
	cpsr = disableInterrupts();
#else
    /* Protect against simultaneous access to the memory pool.  */
	if(EEHandlerFlag==0)
    	TCT_Protect(&(pool_ptr->rtdm_protect));
	else
		cpsr = disableInterrupts();
#endif
    /* Mark the memory as available.  */
    header_ptr->magic &= ~RTDM_HEADER_USED;

    /* Adjust the available number of bytes.  */
    pool_ptr->rtdm_available =  pool_ptr->rtdm_available +
                        ((rt_uint32_t)(header_ptr->next_memory) -
                        (rt_uint32_t)header_ptr) - RTDM_OVERHEAD;

    /* Determine if the block can be merged with the previous neighbor.  */
    if (!RTDM_BLOCK_IS_USED(header_ptr->prev_memory))
    {
	    RTDM_TRACE("merge: left node 0x%08x", header_ptr->prev_memory);

        /* adjust the available number of bytes.  */
        pool_ptr->rtdm_available =  pool_ptr->rtdm_available + RTDM_OVERHEAD;

        /* yes, merge block with previous neighbor.  */
        (header_ptr->prev_memory)->next_memory = header_ptr->next_memory;
        (header_ptr->next_memory)->prev_memory = header_ptr->prev_memory;

        /* move header pointer to previous.  */
        header_ptr = header_ptr->prev_memory;
		insert_header = 0;	/* don't insert header to free list */
    }

    /* determine if the block can be merged with the next neighbor.  */
    if (!RTDM_BLOCK_IS_USED(header_ptr->next_memory))
    {
        /* adjust the available number of bytes.  */
        pool_ptr->rtdm_available =  pool_ptr->rtdm_available + RTDM_OVERHEAD;

        /* merge block with next neighbor.  */
        new_ptr =  header_ptr->next_memory;

	    RTDM_TRACE("merge: right node 0x%08x, nf 0x%08x, pf 0x%08x", new_ptr, new_ptr->next_free_memory, new_ptr->prev_free_memory);

        new_ptr->next_memory->prev_memory = header_ptr;
        header_ptr->next_memory = new_ptr->next_memory;

		/* remove new ptr from free list */
		new_ptr->next_free_memory->prev_free_memory = new_ptr->prev_free_memory;
		new_ptr->prev_free_memory->next_free_memory = new_ptr->next_free_memory;
    }

	if (insert_header)
	{
		/* no left merge, insert to free list */
		header_ptr->next_free_memory = pool_ptr->rtdm_free_list->next_free_memory;
		header_ptr->prev_free_memory = pool_ptr->rtdm_free_list;
		pool_ptr->rtdm_free_list->next_free_memory->prev_free_memory = header_ptr;
		pool_ptr->rtdm_free_list->next_free_memory = header_ptr;

		RTDM_TRACE("insert to free list: nf 0x%08x, pf 0x%08x", header_ptr->next_free_memory, header_ptr->prev_free_memory);
	}
#if defined(PLAT_USE_THREADX) || defined(PLAT_USE_ALIOS)
//	OSAMutexUnlock(pool_ptr->rtdm_protect);
	restoreInterrupts(cpsr);
#else
    /* Release protection of the memory pool.  */
	if(EEHandlerFlag==0)
    	TCT_Unprotect();
	else
		restoreInterrupts(cpsr);
#endif
    /* Return the completion status.  */
    return(status);
}



void InitUSBMemoryPool	(void)
{
	rt_status_t status;

	memset (&USBMemoryPool,0,sizeof(USBMemoryPool));

	status = rtdm_memory_pool_init(&USBMemoryPool, USBPoolArray, USB_POOL_SIZE, DIAG_MIN_MEMORY_ALLOCATION_SIZE);
	RT_ASSERT (status == RT_STATUS_OK);
}


void *USBMemPoolAlignMalloc(UINT32 size)
{
	UINT32 * alignAddress = NULL ;

	rtdm_memory_allocate(&USBMemoryPool, (VOID**)&alignAddress, size);

	return (void *)alignAddress;
}









