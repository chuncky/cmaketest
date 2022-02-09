/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbMemory.c


GENERAL DESCRIPTION

    This file contains the core routines for USB Partition Memory Management
    component.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2011 by Marvell, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who        what, where, why
--------   ------     ----------------------------------------------------------
07/06/2013   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "common.h"
#include "mvUsbMemory.h"
//#include "utils.h"
#include "string.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Usb memory pool reference */
static mvUsbPCB mvUsbMemoryPool;

/* Duster Pool array */
#pragma arm section rwdata="UsbPool", zidata="UsbPool"
__align(32) UINT8 UsbPoolArray[USBNET_MAX_POOL_SIZE] = {0};
#pragma arm section rwdata, zidata

#pragma arm section rwdata="UsbNetRxBuf", zidata="UsbNetRxBuf"
__align(8) UINT8 UsbNetPacketRxBuf[USBNET_RX_MAX_SIZE] = {0};
#pragma arm section rwdata, zidata

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* System assert flag. */
extern BOOL assertFlag;

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION mvUsbCheckMemPoolValidity

DESCRIPTION
  This function Check whether usb pool is valid or not.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
BOOL mvUsbCheckMemPoolValidity(mvUsbPCB *pool_ptr)
{
    /* Check whether pool pointer is valid or not. */
    if(pool_ptr == NULL)
    {
        return FALSE;
    }

    /* Check whether usb pool is valid or not. */
    if(pool_ptr->pool_id != USB_POOL_USED)
    {
        UsbErrTrace("Invalid pool id 0x%x", pool_ptr->pool_id);
        return FALSE;
    }

    /* Check whether usb pool guard is valid or not. */
    if(pool_ptr->pool_guard != USB_POOL_GUARD)
    {
        UsbErrTrace("Invalid pool guard 0x%x", pool_ptr->pool_guard);
        return FALSE;
    }

    return TRUE;
}

/*===========================================================================

FUNCTION mvUsbCheckPartitionValidity

DESCRIPTION
  This function Check whether usb partition is valid or not.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
BOOL mvUsbCheckPartitionValidity(mvUsbPoolHdr *Partition)
{
    /* Check whether pool pointer is valid or not. */
    if(Partition == NULL)
    {
        return FALSE;
    }

    /* Check whether usb pool is valid or not. */
    if(Partition->pool_id != USB_POOL_USED)
    {
        UsbErrTrace("Invalid partition id 0x%x", Partition->pool_id);
        return FALSE;
    }

    /* Check whether usb pool is valid or not. */
    if(Partition->pool_guard != USB_POOL_GUARD)
    {
        UsbErrTrace("Invalid partition guard 0x%x", Partition->pool_guard);
        return FALSE;
    }

    return TRUE;
}

/*===========================================================================

FUNCTION mvUsbCheckPoolAddressValidity

DESCRIPTION
  This function Check whether it is in usb pool range or not.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
BOOL mvUsbCheckPoolAddressValidity(unsigned int address)
{
    unsigned int PoolStart = (unsigned int)UsbPoolArray;

    if(address < PoolStart)
    {
        return FALSE;
    }

    if(address > (PoolStart + USBNET_MAX_POOL_SIZE))
    {
        return FALSE;
    }

    return TRUE;
}

/*===========================================================================

FUNCTION mvUsbCheckReserveAddress

DESCRIPTION
  This function Check whether it is reserve address or not.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
BOOL mvUsbCheckReserveAddress(unsigned int address)
{
    if ((address == (unsigned int)(&UsbPoolArray[USBNET_POOL_SIZE]))||
        (address == (unsigned int)(&UsbPoolArray[USBNET_POOL_SIZE + USB_PARTITON_SIZE])))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*===========================================================================

FUNCTION mvUsbAvailableMemoryCnt

DESCRIPTION
  This function get the avaliable memory count.

DEPENDENCIES
  none

RETURN VALUE
  memory count

SIDE EFFECTS
  none

===========================================================================*/
unsigned int mvUsbAvailableMemoryCnt(void)
{
    return mvUsbMemoryPool.pool_available;
}

/*===========================================================================

FUNCTION mvUsbMemPoolInit

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  create partition pool function.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbMemPoolInit(void)
{
    BOOL             status;             /* Completion status          */
    unsigned int        size;               /* Adjusted size of partition */
    void                *start = NULL;      /* Start address              */
    mvUsbPCB            *pool = NULL;       /* Pool control block ptr     */

    /* Calculate the start address.  */
    start = (void *)UsbPoolArray;

    /* Move input pool pointer into internal pointer.  */
    pool  = (mvUsbPCB *)(&mvUsbMemoryPool);

    /* Initialize the usb Partition PCB.  */
    memset (pool, 0x00, sizeof(mvUsbPCB));

    /* Adjust the partition size to something that is evenly divisible by
       the number of bytes in an aligned size.  */
    size =  USB_ALIGN(USB_PARTITON_SIZE);

    /* Call the actual service to create the partition pool.  */
    status =  mvUsbCreatePartitionPool(pool, start, USBNET_POOL_SIZE, size);

    /* Check the return status.  */
    ASSERT(status != FALSE);
}

/*===========================================================================

FUNCTION mvUsbAllocate

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  usb allocate memory function.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
void *mvUsbAllocate(UINT8 direction)
{
    void     *address = NULL;
    mvUsbPCB *pool_ptr = &mvUsbMemoryPool;

    /* Check whether parameters is valid or not. */
    ASSERT(pool_ptr != NULL);

    /* Parameters are valid, call actual function. */
    address = mvUsbAllocatePartition(pool_ptr, direction);

    /* Return the allocate address.  */
    return address;
}

/*===========================================================================

FUNCTION mvUsbDeallocate

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  usb deallocate memory function.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbDeallocate(void *partition)
{
    /* Check whether parameters is valid or not. */
    ASSERT(partition != NULL);

    /* Parameters are valid, call actual function.  */
    mvUsbDeallocatePartition(partition);
}

/*===========================================================================

FUNCTION mvUsbCreatePartitionPool

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  usb created partition pool function.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
BOOL  mvUsbCreatePartitionPool(mvUsbPCB *pool_ptr, void *start_address,
                                unsigned int pool_size, unsigned int partition_size)
{
    unsigned int        cpsr;
    mvUsbPCB           *pool = NULL;                /* Pool control block ptr    */
    unsigned char      *pointer = NULL;             /* Working byte pointer      */
    mvUsbPoolHdr       *header_ptr = NULL;          /* Partition block header ptr*/

    /* Move input pool pointer into internal pointer.  */
    pool =  (mvUsbPCB *) pool_ptr;

    /* Disable interrupt.  */
    cpsr = disableInterrupts();

    /* First, clear the partition pool ID just in case it is an old
       pool control block.  */
    pool->pool_id = 0;

    /* Save the starting address and size parameters in the partition control
       block.  */
    pool->pool_start_address    =   start_address;
    pool->pool_size             =   pool_size;
    pool->partition_size        =   partition_size;

    /* Initialize the partition parameters.  */
    pool->pool_available        =   0;
    pool->pool_allocated        =   0;
    pool->pool_available_list   =   NULL;

    /* Convert the supplied partition size into something that is evenly
       divisible by the sizeof an unsigned int data element.  This insures
       unsigned int alignment.  */
    partition_size = USB_ALIGN(partition_size);

    /* Loop to build and link as many partitions as possible from within the
       specified memory area.  */
    pointer =  (unsigned char *)  start_address;

    while (pool_size >= (USB_POOL_OVERHEAD + partition_size))
    {
        /* There is room for another partition.  */

        /* Cast the current pointer variable to a header pointer.  */
        header_ptr =  (mvUsbPoolHdr *) pointer;

        /* Now, build a header and link it into the partition pool
           available list- at the front.  */
        header_ptr->partition_pool  =   pool;
        header_ptr->next_available  =   pool->pool_available_list;
        header_ptr->pool_id          =  USB_POOL_FREE;
        header_ptr->pool_guard       =  USB_POOL_GUARD;
        pool->pool_available_list   =   header_ptr;

        /* Increment the number of partitions available in the pool.  */
        pool->pool_available++;

        /* Decrement the number of bytes remaining in the pool.  */
        pool_size =  pool_size - (USB_POOL_OVERHEAD + partition_size);

        /* Increment the working pointer to the next partition position.  */
        pointer =  pointer + (USB_POOL_OVERHEAD + partition_size);
    }

    /* Initialize pool guard. */
    pool->pool_guard            =   USB_POOL_GUARD;

    /* At this point the partition pool is completely built.  The ID can
       now be set and it can be linked into the created partition pool list. */
    pool->pool_id               =   USB_POOL_USED;

    /* Restore interrupt.  */
    restoreInterrupts(cpsr);

    /* Return successful completion.  */
    return TRUE;
}

/*===========================================================================

FUNCTION mvUsbAllocatePartition

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  usb allocate partition function.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
void *mvUsbAllocatePartition(mvUsbPCB *pool_ptr, UINT8 direction)
{
    unsigned int     cpsr;
    void            *address = NULL;
    mvUsbPCB        *pool = NULL;                   /* Pool control block ptr    */
    mvUsbPoolHdr    *partition_ptr = NULL;          /* Pointer to partition      */

    /* System is assert, alloccate memory from reserve buffer.  */
    if(assertFlag)
    {
        /* Get memory from reserve buffer.  */
        address = mvUsbAllocateReserveAdress(direction);

        /* Return the memory address.  */
        return address;
    }

    /* Check whether usb pool is valid or not. */
    ASSERT(mvUsbCheckMemPoolValidity(pool_ptr));

    /* Move input pool pointer into internal pointer.  */
    pool =  (mvUsbPCB *) pool_ptr;

    /* Disable interrupt.  */
    cpsr = disableInterrupts();

    /* Determine if there is an available memory partition.  */
    if (pool->pool_available)
    {
        /* Partition available.  */

        /* Decrement the available count.  */
        pool->pool_available--;

        /* Increment the allocated count.  */
        pool->pool_allocated++;

        /* Unlink the first memory partition and return the pointer to the
           caller.  */
        partition_ptr = pool->pool_available_list;
        pool->pool_available_list =  partition_ptr->next_available;
        partition_ptr->next_available   =  NULL;
        partition_ptr->pool_id = USB_POOL_USED;

        /* Return a memory address to the caller.  */
        address =  (void *) (((unsigned char *) partition_ptr) + USB_POOL_OVERHEAD);
    }
    else
    {
        /* A partition is not available.  alloccate memory from reserve buffer.  */
        //ErrorLogPrintf("Fail to alloc usb memory!!!");

        address = NULL;
    }

    /* Restore interrupt.  */
    restoreInterrupts(cpsr);

    /* Return the completion status.  */
    return address;
}


/*===========================================================================

FUNCTION mvUsbDeallocatePartition

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  usb deallocate partition function.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbDeallocatePartition(void *partition)
{
    unsigned int     cpsr;
    mvUsbPCB        *pool = NULL;               /* Pool pointer              */
    mvUsbPoolHdr    *header_ptr = NULL;         /* Pointer to partition hdr  */

    /* Check whether parameters is valid or not. */
    ASSERT(partition != NULL);

    /* Pickup the associated pool's pointer.  It is inside the header of
       each partition.  */
    header_ptr =  (mvUsbPoolHdr *) (((unsigned char *) partition) - USB_POOL_OVERHEAD);

    /* Check whether it is reserve address or not. */
    if (mvUsbCheckReserveAddress((unsigned int)header_ptr))
    {
        return;
    }

    /* Check whether partition is valid or not. */
    ASSERT(mvUsbCheckPartitionValidity(header_ptr));

    pool = header_ptr->partition_pool;

    /* Check whether usb pool is valid or not. */
    ASSERT(mvUsbCheckMemPoolValidity(pool));

    /* Disable interrupt.  */
    cpsr = disableInterrupts();

    /* Increment the available partitions counter.  */
    pool->pool_available++;

    /* Decrement the allocated partitions counter.  */
    pool->pool_allocated--;

    /* Place the partition back on the available list.  */
    header_ptr->next_available  =  pool->pool_available_list;
    pool->pool_available_list   =  header_ptr;
    header_ptr->pool_id         =  USB_POOL_FREE;

    /* Restore interrupt.  */
    restoreInterrupts(cpsr);

    /* Return the completion status.  */
    return;
}

/*===========================================================================

FUNCTION mvUsbAllocateReserveAdress

DESCRIPTION
  This function performs error checking on the parameters supplied to the
  usb allocate memory from reserve buffer.

DEPENDENCIES
  none

RETURN VALUE
  BOOL status

SIDE EFFECTS
  none

===========================================================================*/
void *mvUsbAllocateReserveAdress(UINT8 direction)
{
    void    *address = NULL, *reserve = NULL;

    switch(direction)
    {
        case 0:
        {
            /* receive direction */
            reserve = &UsbPoolArray[USBNET_POOL_SIZE];
            break;
        }

        case 1:
        {
             /* send direction */
            reserve = &UsbPoolArray[USBNET_POOL_SIZE + USB_PARTITON_SIZE];
            break;
        }

        default:
        {
            ASSERT(0);
            break;
        }

    }

    /* A partition is not available.  return reserve buffer.  */
    address = (void *) (((unsigned char *) reserve) + USB_POOL_OVERHEAD);

    /* Return address.  */
    return address;
}

