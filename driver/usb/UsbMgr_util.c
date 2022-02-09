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
* Title: Universal Serial Bus Manager- USB Manager utilities modolue *
*                                                                    *
* Filename: UsbMgr_util.c                                            *
*                                                                    *
* Target, platform: Common Platform, SW platform                     *
*                                                                    *
* Author: Miriam Yovel                                               *
*                                                                    *
* Description:                                                       *
*                                                                    *
************************ ******************************************* */
#ifndef LWIP_IPNETBUF_SUPPORT
//#if defined(DIAG_OVER_USBMGR)

#include <string.h>
//#include "utils.h"
//#include "csw_mem.h"

#include "UsbMgr_util.h"

/************************************************************************
* Function: USBMgrUtilInsertList
*************************************************************************
* Description:
*
* Parameters:
* Return value: void
*
* Notes:
************************************************************************/
void USBMgrUtilInsertList( USBMgrDataRegDBStruct **listPtr , USBMgrDataRegDBStruct **newListPtr)
{
    USBMgrDataRegDBStruct *newListTemp ,*listTemp;

    newListTemp = *newListPtr;
    listTemp = *listPtr;

    if (newListTemp == NULL) /* Generaly before insert new list ***/
    {
        newListTemp = (USBMgrDataRegDBStruct *)malloc( (UINT32)sizeof(USBMgrDataRegDBStruct) );
		//[klockwork][issue id: 201]
		if(newListTemp == NULL)
		{
			ASSERT(0);
			return ;
		}
        *newListPtr = newListTemp;
    }
    newListTemp->nextInterface = NULL;
    if (listTemp == NULL)  /* Entry list */
        *listPtr = newListTemp;
    else
    {
        while (listTemp->nextInterface != NULL)
            listTemp = listTemp->nextInterface;
      listTemp->nextInterface = newListTemp;
    }
}

/************************************************************************
* Function: USBMgrUtilDelList
*************************************************************************
* Description:
*
* Parameters:
* Return value: void
*
* Notes:
************************************************************************/
void USBMgrUtilDelList( USBMgrDataRegDBStruct *listPtr , USBMgrDataRegDBStruct *reqListPtr)
{
    USBMgrDataRegDBStruct *prevListTemp;

    prevListTemp = listPtr;

    if(prevListTemp != reqListPtr )
    {
        while(prevListTemp->nextInterface != reqListPtr)
            prevListTemp = prevListTemp->nextInterface;
        prevListTemp->nextInterface = reqListPtr->nextInterface;

    }
    else
        prevListTemp = reqListPtr->nextInterface;
    free( (void *)reqListPtr );
}

/************************************************************************
* Function: USBMgrUtilFindListByEndpoint
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrUtilFindListByEndpoint( USBMgrDataRegDBStruct *listPtr , UINT8 endpoint , USBMgrDataRegDBStruct **reqListPtr)
{
    USBMgrDataRegDBStruct *listTemp;
    UINT8                 endpointIdx = 0;

    listTemp = listPtr;

    while( (listTemp->endpointName[endpointIdx] != endpoint) && (endpointIdx<MAX_ENDPOINTS) )
    {
        endpointIdx++;
        if( endpointIdx == MAX_ENDPOINTS)
        {
            endpointIdx = 0;
            listTemp = listTemp->nextInterface;
        }
    }

    *reqListPtr = listTemp;

}

/************************************************************************
* Function: USBMgrUtilCreateTxQueue
*************************************************************************
* Description:
*
* Parameters:
* Return value: void
*
* Notes:
************************************************************************/
void USBMgrUtilCreateTxQueue(USBMgr_TxDefStruct txDef , UINT8 **queuePtr )
{

    if( txDef.optionalTxQueuePtr == NULL)        /**USB Manager has to allocate memory for Tx queue */
    {
        *queuePtr = (UINT8*)malloc( (txDef.txQueueLen)*sizeof(USBMgrSendMessage) );
    }
    else  /***application  requested memory for TX queue    ***/
        *queuePtr = txDef.optionalTxQueuePtr;



}


/************************************************************************
* Function: USBMgrUtilDeleteTxQueue
*************************************************************************
* Description:
*
* Parameters:
* Return value: void
*
* Notes:
************************************************************************/
void USBMgrUtilDeleteTxQueue( UINT8 *queuePtr  )
{
    free( (void*)queuePtr);
}
/************************************************************************
* Function: USBMgrInsertItemToQ
*************************************************************************
* Description:
*
* Parameters:
* Return value: void
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrUtilInsertToQueue(UINT8 endpoint, USBMgrDataRegDBStruct *interfacePtr, USBMgrSendMessage *msg ,UINT32 queueSize)
{
    UINT32                cpsr;
    UINT8                 index;
    USBMgr_RC              usbmgrUtilStatus = USBMGR_RC_OK;


    index = _usbMgrDBEpArry[endpoint].queueIdx;
    cpsr = disableInterrupts();

      /*add message to QUEUE (if  not full)*/



    if( ALMOST_FULL_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueuePtrs,queueSize) )
        usbmgrUtilStatus = USBMGR_RC_TX_BUFFER_FULL;

    memcpy ((NEXT_ITEM_PTR_ON_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueueAddr,interfacePtr->endpointQueue[index].endpointTxQueuePtrs.nextFreeItem)) ,
               (UINT8*)msg, sizeof(USBMgrSendMessage));
    INC_QUEUE_ITEM_PTR(interfacePtr->endpointQueue[index].endpointTxQueuePtrs.nextFreeItem ,queueSize);
    restoreInterrupts(cpsr);

    return(usbmgrUtilStatus);

}


/************************************************************************
* Function: USBMgrUtilGetFromQueue
*************************************************************************
* Description:
*
* Parameters:
* Return value: void
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrUtilGetFromQueue(UINT8 endpoint, USBMgrDataRegDBStruct *interfacePtr, USBMgrSendMessage **msg ,USBMGRTxQueuePtrs queuePtr ,UINT32 queueSize)
{
    UINT32 cpsr;
    UINT8  index;


    index = _usbMgrDBEpArry[endpoint].queueIdx;
    cpsr = disableInterrupts();

    switch ( queuePtr)
    {
        case NEXT_FREE_PTR :
            if(FULL_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueuePtrs,queueSize) )
            {
                restoreInterrupts(cpsr);
                return(USBMGR_RC_TRANSMIT_ERROR);
            }
            *msg = (USBMgrSendMessage *)(NEXT_ITEM_PTR_ON_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueueAddr ,
                                   interfacePtr->endpointQueue[index].endpointTxQueuePtrs.nextFreeItem));

        break;



        case NEXT_MSG2SEND_PTR :
        if(EMPTY_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueuePtrs))
        {
            restoreInterrupts(cpsr);
            return(USBMGR_RC_NO_MSG);
        }
        *msg = (USBMgrSendMessage *)(NEXT_ITEM_PTR_ON_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueueAddr ,
                                   interfacePtr->endpointQueue[index].endpointTxQueuePtrs.nextItemToSend));

        break;



      case NEXT_MSG2FREE_PTR :
          *msg = (USBMgrSendMessage *)(NEXT_ITEM_PTR_ON_QUEUE(interfacePtr->endpointQueue[index].endpointTxQueueAddr ,
                                   interfacePtr->endpointQueue[index].endpointTxQueuePtrs.nextItemToRel));

      break;



      default:break;
    }

    restoreInterrupts(cpsr);
    return(USBMGR_RC_OK);
}



/************************************************************************
* Function:  USBMgrUtilGetNextOutEpMemory
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrUtilGetNextOutEpMemory( USBMgr_EndpointStruct *rxEndpoint ,UINT8 **dataPtr ,UINT32 *dataLen ,USBMGRRxMemTypes memType)
{

    if(memType == SMALL_BLOCK_MEM )
        *dataLen = rxEndpoint->endpointDef.rxDef.rxBuffQuantumLen;

    if(rxEndpoint->endpointDef.rxDef.useExternalMem == TRUE)
        *dataPtr = (UINT8*)extMemDynMalloc( *dataLen );
    else  /*internal memory*/
      *dataPtr = (UINT8*)intMemDynMalloc( *dataLen );

}

/************************************************************************
* Function:  USBMgrUtilFreeNextOutEpMemory
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrUtilFreeNextOutEpMemory(UINT8 *dataPtr)
{
    free( (void *) dataPtr );
}

/************************************************************************
* Function:  USBMgrUtilAllocateDescriptors
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void *USBMgrUtilAllocateDescriptors( BOOL dmaDescriptors ,UINT16 allocationSize)
{
    if(dmaDescriptors)
        return(alignMalloc( (UINT32)allocationSize));
    else
        return (malloc( (UINT32) allocationSize ));
}

//#endif
#endif
