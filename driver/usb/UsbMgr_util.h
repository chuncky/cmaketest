/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code ("Material") are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/

/*********************************************************************
*                      M O D U L E     B O D Y                       *
**********************************************************************
* Title: Universal Serial Manager Bus - USB                          *
*                                                                    *
* Filename: USBMgr_util                                               *
*                                                                    *
* Author: Miriam Yovel                                               *
*                                                                    *
* Description: This is the main header file of                       *
*              the USB Manager Package                                       *
*                                                                    *
* Notes: This file is using functions from  file              *
*                                                                    */

#ifndef _USBMGR_UTIL_H_
#define _USBMGR_UTIL_H_

//#include "global_types.h"
#include "common.h"
#include "usbmgr_types.h"

/*************************** Macro Definitions ************************/

#define FULL_QUEUE(ptrs ,queueSize)         ( (((ptrs).nextFreeItem == queueSize-1)&&((ptrs).nextItemToRel == 0) )||((ptrs).nextFreeItem+1 == (ptrs).nextItemToRel) )

//#define ALMOST_FULL_QUEUE(ptrs ,queueSize)  ( (((ptrs).nextFreeItem == queueSize-2)&&((ptrs).nextItemToRel == 0) )||((ptrs).nextFreeItem+2 == (ptrs).nextItemToRel) )

//#define ALMOST_FULL_QUEUE(ptrs ,queueSize)    ( (((ptrs).nextFreeItem == queueSize-2)&&((ptrs).nextItemToRel == 0) )|| (((ptrs).nextFreeItem == queueSize-1)&&((ptrs).nextItemToRel == 1) ) || ((ptrs).nextFreeItem+1 == (ptrs).nextItemToRel) )

#define ALMOST_FULL_QUEUE(ptrs ,queueSize)    ( (((ptrs).nextFreeItem == queueSize-1)&&((ptrs).nextItemToRel == 0) )||( (ptrs).nextFreeItem+1 == (ptrs).nextItemToRel) )




#define FREE_QUEUE(ptrs ,queueSize)                   ( (( (ptrs).nextFreeItem + (ptrs).freeAgain == queueSize )&&((ptrs).nextItemToRel == 0) )|| ( (ptrs).nextFreeItem + (ptrs).freeAgain ==(ptrs).nextItemToRel ) )


#define EMPTY_QUEUE(ptrs)                   ((ptrs).nextFreeItem == (ptrs).nextItemToSend )

#define NEXT_ITEM_PTR_ON_QUEUE(globalAdd , nextPtr )    (globalAdd) + ((nextPtr)*sizeof(USBMgrSendMessage))

#define INC_QUEUE_ITEM_PTR(item ,queueSize)   {   if( (item) == (queueSize)-1 )  \
                                                    (item) = 0;                  \
                                                  else                           \
                                                    (item) = ((item)+1);         \
                                              }

#define NUMBER_MSG_TO_SEND(max_size ,size1 ,size2 )             ( ((size1) > (size2))?(( size1)- (size2)) :( (max_size)-(size2) + (size1) ) )


/***************************Extern ************************/

extern USBMgrDBInfoStruct    _usbMgrDBEpArry[USB_DEVICE_TOTAL_ENDPOINTS];

/***************************Private Prototype ************************/
void USBMgrUtilInsertList( USBMgrDataRegDBStruct **listPtr , USBMgrDataRegDBStruct **newListPtr);
void USBMgrUtilDelList( USBMgrDataRegDBStruct *listPtr , USBMgrDataRegDBStruct *reqListPtr);
void USBMgrUtilFindListByEndpoint( USBMgrDataRegDBStruct *listPtr , UINT8 endpoint , USBMgrDataRegDBStruct **reqListPtr);
void USBMgrUtilCreateTxQueue(USBMgr_TxDefStruct txDef , UINT8 **queuePtr );
void USBMgrUtilDeleteTxQueue( UINT8 *queuePtr  );
USBMgr_RC USBMgrUtilInsertToQueue(UINT8 endpoint, USBMgrDataRegDBStruct *interfacePtr, USBMgrSendMessage *msg ,UINT32 queueSize );
USBMgr_RC USBMgrUtilGetFromQueue(UINT8 endpoint, USBMgrDataRegDBStruct *interfacePtr, USBMgrSendMessage **msg ,USBMGRTxQueuePtrs queuePtr ,UINT32 queueSize);
void USBMgrUtilGetNextOutEpMemory( USBMgr_EndpointStruct *rxEndpoint ,UINT8 **dataPtr ,UINT32 *dataLen ,USBMGRRxMemTypes memType);
void USBMgrUtilFreeNextOutEpMemory(UINT8 *dataPtr);
void *USBMgrUtilAllocateDescriptors( BOOL dmaDescriptors ,UINT16 allocationSize);





#endif /*_USBMGR_UTIIL_H_*/























