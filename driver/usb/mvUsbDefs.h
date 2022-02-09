#ifndef __mvUsbDefs_h__
#define __mvUsbDefs_h__ 1
/*******************************************************************************
** File          : $HeadURL$
** Author        : $Author$
** Project       : HSCTRL
** Instances     :
** Creation date :
********************************************************************************
********************************************************************************
** ChipIdea Microelectronica - IPCS
** TECMAIA, Rua Eng. Frederico Ulrich, n 2650
** 4470-920 MOREIRA MAIA
** Portugal
** Tel: +351 229471010
** Fax: +351 229471011
** e_mail: chipidea.com
********************************************************************************
** ISO 9001:2000 - Certified Company
** (C) 2005 Copyright Chipidea(R)
** Chipidea(R) - Microelectronica, S.A. reserves the right to make changes to
** the information contained herein without notice. No liability shall be
** incurred as a result of its use or application.
********************************************************************************
** Modification history:
** $Date$
** $Revision$
*******************************************************************************
*** Comments:
***   This file contains USB Device API defines for state and function
***  returns.
**************************************************************************
**END*********************************************************/
#include "common.h"
#include "udc2_hw.h"
//#include "utils.h"      // for disableInterrupts definition
//#include "csw_mem.h"    // for malloc, free definition
#include "string.h"     // for memset, memcpy definition
//#include "mmap.h"       // for NON_CACHED_TO_CACHED, CACHED_TO_NON_CACHED macro definition
#define CACHED_TO_NON_CACHED_OFFSET 0x00000000

#define CACHED_TO_NON_CACHED(cACHED)    ((void *)(((UINT32)(cACHED)) | (UINT32)CACHED_TO_NON_CACHED_OFFSET))
#define NON_CACHED_TO_CACHED(nON_CACHED)  ((void *)(((UINT32)(nON_CACHED)) & (~(UINT32)CACHED_TO_NON_CACHED_OFFSET)))
/*******************************************/
/* USB Core Types                          */
/*******************************************/

#define _PTR_      *
#define _CODE_PTR_ *

typedef char _PTR_                    char_ptr;    /* signed character       */

typedef signed   char  int_8, _PTR_   int_8_ptr;   /* 8-bit signed integer   */
typedef unsigned char  uint_8, _PTR_   uint_8_ptr;  /* 8-bit signed integer   */

typedef          short int_16, _PTR_   int_16_ptr;  /* 16-bit signed integer  */
typedef unsigned short uint_16, _PTR_  uint_16_ptr; /* 16-bit unsigned integer*/

typedef          int   int_32, _PTR_   int_32_ptr;  /* 32-bit signed integer  */
typedef unsigned int   uint_32, _PTR_  uint_32_ptr; /* 32-bit unsigned integer*/

typedef unsigned long  boolean;  /* Machine representation of a boolean */

typedef void _PTR_     pointer;  /* Machine representation of a pointer */

UINT32* USB_uncached_memalloc(UINT32 size, UINT32 phyAddr);
//hongji 201011
void USBMemPoolAlignFree(void* alignAddress);

/*******************************************/
/* Macro                                   */
/*******************************************/

#define USB_virt_to_phys(PvIRT)                         (uint_32)(PvIRT)
#define USB_get_cap_reg_addr(dEV)                       ((void*)USB2_CAP_LENGTH_REG)
//#define USB_uncached_memfree(PvIRT, sIZE, pHYSaDDR)     free(NON_CACHED_TO_CACHED(PvIRT))
#define USB_uncached_memfree(PvIRT, sIZE, pHYSaDDR)     USBMemPoolAlignFree((void *)PvIRT)
#define USB_memalloc(sIZE)                              USB_uncached_memalloc(sIZE,0)
#define USB_memfree(pTR)                                USB_uncached_memfree(pTR, NULL, NULL)
#define USB_memzero(pTR, n)                             memset(pTR, 0, n)
#define USB_memcopy(sRC, dST, n)                        memcpy(dST, sRC, n)
#define USB_dcache_inv(pTR, sIZE)                       CacheInvalidateMemory(pTR, sIZE)
#define USB_dcache_flush(pTR, sIZE)                     CacheCleanMemory( pTR, sIZE)
#define USB_lock()                                      disableInterrupts();
#define USB_unlock(key)                                 restoreInterrupts(key);
/* Endianess macros.                                                        */
#define USB_16BIT_LE(X)  (X)
#define USB_32BIT_LE(X)  (X)

#define USB_MEM_ALIGN(n, align)            ((n) + (-(n) & (align-1)))

/* Macro for aligning the EP queue head to 32 byte boundary */
#define USB_MEM32_ALIGN(n)                  USB_MEM_ALIGN(n, 32)

/* Macro for aligning the EP queue head to 1024 byte boundary */
#define USB_MEM1024_ALIGN(n)                USB_MEM_ALIGN(n, 1023)

/* Macro for aligning the EP queue head to 1024 byte boundary */
#define USB_MEM2048_ALIGN(n)                USB_MEM_ALIGN(n, 2047)

/*******************************************/
/* Host specific                           */
/*******************************************/
#define  USB_DEBOUNCE_DELAY                  (101)
#define  USB_RESET_RECOVERY_DELAY            (11)
#define  USB_RESET_DELAY                     (60)

/*******************************************/
/* Error codes                             */
/*******************************************/
#define  USB_OK                              (0x00)
#define  USBERR_ALLOC                        (0x81)
#define  USBERR_BAD_STATUS                   (0x82)
#define  USBERR_CLOSED_SERVICE               (0x83)
#define  USBERR_OPEN_SERVICE                 (0x84)
#define  USBERR_TRANSFER_IN_PROGRESS         (0x85)
#define  USBERR_ENDPOINT_STALLED             (0x86)
#define  USBERR_ALLOC_STATE                  (0x87)
#define  USBERR_DRIVER_INSTALL_FAILED        (0x88)
#define  USBERR_DRIVER_NOT_INSTALLED         (0x89)
#define  USBERR_INSTALL_ISR                  (0x8A)
#define  USBERR_INVALID_DEVICE_NUM           (0x8B)
#define  USBERR_ALLOC_SERVICE                (0x8C)
#define  USBERR_INIT_FAILED                  (0x8D)
#define  USBERR_SHUTDOWN                     (0x8E)
#define  USBERR_INVALID_PIPE_HANDLE          (0x8F)
#define  USBERR_OPEN_PIPE_FAILED             (0x90)
#define  USBERR_INIT_DATA                    (0x91)
#define  USBERR_SRP_REQ_INVALID_STATE        (0x92)
#define  USBERR_TX_FAILED                    (0x93)
#define  USBERR_RX_FAILED                    (0x94)
#define  USBERR_EP_INIT_FAILED               (0x95)
#define  USBERR_EP_DEINIT_FAILED             (0x96)
#define  USBERR_TR_FAILED                    (0x97)
#define  USBERR_BANDWIDTH_ALLOC_FAILED       (0x98)
#define  USBERR_INVALID_NUM_OF_ENDPOINTS     (0x99)

#define  USBERR_DEVICE_NOT_FOUND             (0xC0)
#define  USBERR_DEVICE_BUSY                  (0xC1)
#define  USBERR_NO_DEVICE_CLASS              (0xC3)
#define  USBERR_UNKNOWN_ERROR                (0xC4)
#define  USBERR_INVALID_BMREQ_TYPE           (0xC5)
#define  USBERR_GET_MEMORY_FAILED            (0xC6)
#define  USBERR_INVALID_MEM_TYPE             (0xC7)
#define  USBERR_NO_DESCRIPTOR                (0xC8)
#define  USBERR_NULL_CALLBACK                (0xC9)
#define  USBERR_NO_INTERFACE                 (0xCA)
#define  USBERR_INVALID_CFIG_NUM             (0xCB)
#define  USBERR_INVALID_ANCHOR               (0xCC)
#define  USBERR_INVALID_REQ_TYPE             (0xCD)

/*******************************************/
/* Error Codes for lower-layer             */
/*******************************************/
#define  USBERR_ALLOC_EP_QUEUE_HEAD          (0xA8)
#define  USBERR_ALLOC_TR                     (0xA9)
#define  USBERR_ALLOC_DTD_BASE               (0xAA)
#define  USBERR_CLASS_DRIVER_INSTALL         (0xAB)


/*******************************************/
/* Pipe Types                              */
/*******************************************/
#define  USB_ISOCHRONOUS_PIPE                (0x01)
#define  USB_INTERRUPT_PIPE                  (0x02)
#define  USB_CONTROL_PIPE                    (0x03)
#define  USB_BULK_PIPE                       (0x04)

#define  MV_USB_STATE_UNKNOWN               (0xff)
#define  MV_USB_STATE_POWERED               (0x03)
#define  MV_USB_STATE_DEFAULT               (0x02)
#define  MV_USB_STATE_ADDRESS               (0x01)
#define  MV_USB_STATE_CONFIG                (0x00)
#define  MV_USB_STATE_SUSPEND               (0x80)

#define  MV_USB_SELF_POWERED                (0x01)
#define  MV_USB_REMOTE_WAKEUP               (0x02)

/*******************************************/
/* Bus Control values                      */
/*******************************************/
#define  MV_USB_NO_OPERATION                (0x00)
#define  MV_USB_ASSERT_BUS_RESET            (0x01)
#define  MV_USB_DEASSERT_BUS_RESET          (0x02)
#define  MV_USB_ASSERT_RESUME               (0x03)
#define  MV_USB_DEASSERT_RESUME             (0x04)
#define  MV_USB_SUSPEND_SOF                 (0x05)
#define  MV_USB_RESUME_SOF                  (0x06)

/*******************************************/
/* possible values of XD->bStatus          */
/*******************************************/
#define  MV_USB_STATUS_IDLE                 (0)
#define  MV_USB_STATUS_TRANSFER_ACCEPTED    (1)
#define  MV_USB_STATUS_TRANSFER_PENDING     (2)
#define  MV_USB_STATUS_TRANSFER_IN_PROGRESS (3)
#define  MV_USB_STATUS_ERROR                (4)
#define  MV_USB_STATUS_DISABLED             (5)
#define  MV_USB_STATUS_STALLED              (6)
#define  MV_USB_STATUS_TRANSFER_QUEUED      (7)

#define  MV_USB_RECV                        (0)
#define  MV_USB_SEND                        (1)

#define  MV_USB_DEVICE_DONT_ZERO_TERMINATE  (0x1)

#define  MV_USB_SETUP_DATA_XFER_DIRECTION   (0x80)

#define  MV_USB_SPEED_FULL                  (0)
#define  MV_USB_SPEED_LOW                   (1)
#define  MV_USB_SPEED_HIGH                  (2)

#define  MV_USB_MAX_PKTS_PER_UFRAME         (0x6)


//extern void*  memcpy(void*,const void*,unsigned long );
//extern void*  malloc (size_t size);


/* USB 1.1 Setup Packet */
typedef struct setup_struct {
   uint_8      REQUESTTYPE;
   uint_8      REQUEST;
   uint_16     VALUE;
   uint_16     INDEX;
   uint_16     LENGTH;
} SETUP_STRUCT, _PTR_ SETUP_STRUCT_PTR;

typedef pointer _usb_device_handle;

#endif /* __mvUsbDefs_h__ */

/* EOF */
