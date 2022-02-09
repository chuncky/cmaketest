#ifndef _MVUSBMEMORY_H
#define _MVUSBMEMORY_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbMemory.h


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
#include "platform.h"

/*===========================================================================

                       LOCAL MACRO FOR MODULE
===========================================================================*/

/* The maximum usb pool name. */
#define  USB_MAX_NAME                   8

/* The maximum usb pool name. */
#define  USB_PARTITON_SIZE              1024

/* Usb memory Pool Free mask. */
#define  USB_POOL_FREE                  0xBEAD0000

/* Usb memory Pool Used mask. */
#define  USB_POOL_USED                  0xBEADBEAD

/* Usb memory Pool guard.  */
#define  USB_POOL_GUARD                 0xBEEFBEEF

/* Usb aligned size.  */
#define  USB_ALIGN_SIZE                 8

/* Usb Net receive buffer size */
#define  USBNET_RX_BUF_SIZE		            1792

/* The maximum Usb Net Rx count */
#ifdef CRANE_MCU_DONGLE
#define  USBNET_MAX_RX_CNT		       60	// 40
#else
#define  USBNET_MAX_RX_CNT		       60	// 40
#endif

/* The maximum Usb Net TX count */
#define  USBNET_MAX_TX_CNT              128

/* Usb pool size. */
#define  USBNET_POOL_SIZE		    	(16 * USB_PARTITON_SIZE)

/* Usb reserve pool size. */
#define  USBNET_RESERVE_SIZE		    (2 * USB_PARTITON_SIZE)

/* Usb Net receive buffer size */
#define  USBNET_RX_MAX_SIZE             (USBNET_RX_BUF_SIZE * USBNET_MAX_RX_CNT)

/* Usb maximum pool size. */
#define  USBNET_MAX_POOL_SIZE	        (USBNET_POOL_SIZE + USBNET_RESERVE_SIZE)

/* Usb align.  */
#define  USB_ALIGN(size)                ((size + USB_ALIGN_SIZE - 1) & ~(USB_ALIGN_SIZE - 1))

/* Usb memory Pool overhead.  */
#define  USB_POOL_OVERHEAD              USB_ALIGN(sizeof(mvUsbPoolHdr))
#define ErrorLogPrintf uart_printf
/* Usb error trace.  */
#define  UsbErrTrace(fmt,args...)       ErrorLogPrintf(fmt, ##args)

/* Usb normal information trace. */
#define  UsbInfoTrace(fmt,args...)          uart_printf(fmt, ##args)


/*
 * Macros.
 */
#define     USB_ALLOC_MEM_STATUS(point)                             \
    {                                                                  \
        if ( point == NULL )                                           \
        {                                                              \
            USB2MgrDeviceUnplugPlug();                                 \
            return;                                                    \
        }                                                              \
    }

/*===========================================================================

                     Type definition FOR MODULE.

===========================================================================*/

/* Define the Partition Pool Control Block data type.  */
typedef struct mvUsbPCB
{
    unsigned int            pool_id;                    /* Internal Pool ID       */
    void                   *pool_start_address;         /* Starting pool address  */
    unsigned int            pool_size;                  /* Size of pool           */
    unsigned int            partition_size;             /* Size of each partition */
    unsigned int            pool_available;             /* Available partitions   */
    unsigned int            pool_allocated;             /* Allocated partitions   */
    struct mvUsbPoolHdr    *pool_available_list;        /* Available list         */
    unsigned int            pool_guard;                  /* Internal Pool ID       */

} mvUsbPCB;

/* The header structure before each memory partition.  */
typedef struct mvUsbPoolHdr
{
    unsigned int            pool_id;                 /* Internal Pool ID       */
    struct mvUsbPoolHdr    *next_available;          /* Next available partition */
    mvUsbPCB               *partition_pool;          /* Partition pool pointer */
    unsigned int            pool_guard;              /* Internal Pool ID       */
} mvUsbPoolHdr;

/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrDeviceUnplugPlug                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function simulate usb plug in/out.                           */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
extern void USB2MgrDeviceUnplugPlug(void);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

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
void *mvUsbAllocatePartition(mvUsbPCB *pool_ptr, UINT8 direction);

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
void mvUsbDeallocatePartition(void *partition);

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
void *mvUsbAllocateReserveAdress(UINT8 direction);

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
void mvUsbMemPoolInit(void);

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
void *mvUsbAllocate(UINT8 direction);

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
void mvUsbDeallocate(void *partition);

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
BOOL mvUsbCheckMemPoolValidity(mvUsbPCB *pool_ptr);

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
BOOL mvUsbCheckPartitionValidity(mvUsbPoolHdr *Partition);

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
BOOL mvUsbCheckPoolAddressValidity(unsigned int address);

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
BOOL mvUsbCheckReserveAddress(unsigned int address);

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
                                unsigned int pool_size, unsigned int partition_size);

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
unsigned int mvUsbAvailableMemoryCnt(void);

#endif /*_MVUSBMEMORY_H*/
