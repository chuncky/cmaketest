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
* Title: Universal Serial Bus - USB Manager                          *
*                                                                    *
* Filename: usbmgr_types.h                                           *
*                                                                    *
* Target, platform: Common Platform, SW platform                     *
*                                                                    *
* Author:                                                  *
*                                                                    *
* Description: This file contains types for use in the USB           *
*                                                                    *
*                                                                    */

#ifndef _USBMGR_TYPES_H_
#define _USBMGR_TYPES_H_
//#include "global_types.h"
#include "common.h"
//Hongji 201011
//#include "dma_assign.h"
//#include "man_hw.h"
#include "UsbMgr.h"



/*************************** Macro Definitions ************************/

#define USB_SETUP_REQUEST_SIZE     8
#define  MAX_TX_ENDPOINTS          5
#define  MAX_RX_ENDPOINTS          4
#define  MAX_ENDPOINTS             USB_DEVICE_TOTAL_ENDPOINTS-1
#define  MAX_REC_MESSAGESS         5

#define FIND_ENDPOINT_INDEX(Interface , Endpoint ,Index ,EndpointIndex ) { (EndpointIndex) = (Interface)->endpoint;                 \
                                                                           (Index) = 1;                                           \
                                                                            while( ((Interface)->endpointName[(Index)]!=(Endpoint) )&& ((Index)< USB_DEVICE_TOTAL_ENDPOINTS ) ) \
                                                                            {                                                    \
                                                                              (Index)++;                                           \
                                                                              (EndpointIndex)++;                                   \
                                                                            }                                                    \
                                                                          }

#define FIND_QUEUE_INDEX(Interface , Endpoint ,QueueIndex )      {     (QueueIndex) = 0;             \
                                                                       while (((Interface)->endpointQueue[QueueIndex].txEndpoint != (Endpoint) ) && ((QueueIndex)< MAX_TX_ENDPOINTS) ) \
                                                                       (QueueIndex)++;                                                      \
                                                                  }

#define FIND_OUT_ENDPOINT_INDEX(Interface , Endpoint ,EndpointIndex) {     (EndpointIndex) = 0;       \
                                                                           while( ((Interface)->endpointRxPrms[EndpointIndex].rxEndpoint != (Endpoint) )&& ((EndpointIndex) < MAX_RX_ENDPOINTS) )\
                                                                           (EndpointIndex)++;                                                     \
                                                                      }


#define FIND_TX_LISR_PRMS_INDEX( Endpoint ,Index)                     {     (Index) = 0;                                     \
                                                                           while( (_usbMgrLisrParams.txPrms[(Index)].txEndpoint != (Endpoint))&&((Index) < MAX_TX_ENDPOINTS) ) \
                                                                           (Index)++;                                               \
                                                                      }


#define FIND_RX_LISR_PRMS_INDEX( Endpoint ,Index)                     {     (Index) = 0;                                     \
                                                                           while( (_usbMgrLisrParams.rxPrms[(Index)].rxEndpoint != (Endpoint))&&((Index) < MAX_RX_ENDPOINTS) ) \
                                                                           (Index)++;                                               \
                                                                      }




#define FIND_DMA_CHANNEL_BY_ENDPOINT( Endpoint)                       (MANITOBA_GET_DMA_CHANNEL( (Endpoint) + DMA_USB_ENDPOINT_1-1 ))




typedef enum{
    NEXT_FREE_PTR,
    NEXT_MSG2SEND_PTR,
    NEXT_MSG2FREE_PTR
}USBMGRTxQueuePtrs;

typedef enum{
    SMALL_BLOCK_MEM,
    LARGE_BLOCK_MEM
}USBMGRRxMemTypes;

typedef struct{
    BOOL    available;
    UINT32  interfaceHnd;
    UINT8   endpointIdx;
    UINT8   queueIdx;
    UINT8   rxBufferIdx;
    UINT8   txLisrIdx;
    UINT8   rxLisrIdx;
}USBMgrDBInfoStruct;

typedef struct{
    USBMgr_EPDir  endpointDir;                 /*  endpoint direction */
    USBDevice_EndpointE endpoint;                  /* the endpoint that found available */
}USBMGREndpointsReqStruct;


typedef struct{
    UINT8  nextItemToRel;
    UINT8  nextFreeItem;
    UINT8  nextItemToSend;
    UINT8  freeAgain;
}USBMgrQPointers;

typedef struct{
    UINT8            txEndpoint;
    BOOL             txEndpointBusy;
    USBMgrQPointers  endpointTxQueuePtrs;
    UINT8            *endpointTxQueueAddr;
    BOOL             fullQueue;
}USBMgrEndpointQueueStruct;


/************** Transmit Definitions ************************************************/
typedef struct{
    UINT8     *dataPtr;
    UINT32   dataLen;
	BOOL	 zlp; //send zero length packet
}USBMgrSendMessage;


        /* Transmit Indication Structure */

typedef struct{
    USBDevice_EndpointE   txEndpoint;
    UINT32          noOfTxBuffers;                    /* if not multi transmit always zero */
    BOOL            transmitAll;                      /* if not multi transmit always FALSE */
}USBMgrLisrTxPrmsStruct;

          /* Receive Indication Structure */


typedef struct{
    USBDevice_EndpointE   rxEndpoint;
    UINT8           *recPtr;
    UINT32          recLen;
    BOOL            endOfRecMsg;
}USBMgrLisrRxPrmsStruct;

typedef struct{
    UINT8        ctrlEndpoint;
    USBMgr_Status usbMgrStatus;
    USBMgr_Error  usbMgrError;
}USBMgrLisrCtrlPrmsStruct;

typedef struct{
    UINT8               setupEndpoint;
    USBMgr_SetupDataType setupType;
    UINT8               setupData[USB_SETUP_REQUEST_SIZE]; //setup  packet
    UINT8               *p_data; //pointer to data buffer       
}USBMgrLisrSetupPrmsStruct;


typedef struct{
    USBMgrLisrTxPrmsStruct      txPrms[MAX_TX_ENDPOINTS];
    USBMgrLisrRxPrmsStruct      rxPrms[MAX_RX_ENDPOINTS];
    USBMgrLisrCtrlPrmsStruct    ctrlPrms;
    USBMgrLisrSetupPrmsStruct   setUpPrms;
}USBMgrLisrPrmsStruct;




typedef struct USBMgrRxDataListStruct
{
    UINT8               *rxDataPtr;
    UINT32              rxDataLen;
    struct USBMgrRxDataListStruct    *p_next;
} USBMgrRxDataList;


typedef struct{
    UINT8            rxEndpoint;
    BOOL             in_first_message_chunk;

    //for complex message containing more than 1 chunk
    USBMgrRxDataList *list_of_rx_data_start;
    USBMgrRxDataList *list_of_rx_data_curr;
    UINT16            curr_rx_buffers_num;
    UINT32            rx_total_data_length;
    //this will contain the pointer to the buffer allocated by mgr if buffer had more than one chunk
    UINT8*            rx_complex_buff; 
    UINT32            rx_packets_received; 
    UINT32            rx_chunks_received; 
    UINT32            rx_packets_confirmed;  
    
    //this will contain pointer allocated by Mgr if extended rx confirmation was called
    //that is application knows how many bytes must arrive next, but the buffer is not enough
    UINT8*            rx_ext_buffer;
    UINT32            rx_ext_buffer_length;
}USBMgrRxPrmsStruct;



/**************Data Base definitions ************************************************/

typedef struct USBMgrDataRegDBTypeStruct{
    UINT32                              interfaceNumber; /**specific registered interface number ***/
    UINT8                               numOfEndpoints;
    UINT8                               applicationID;
    USBDevice_EndpointE                       endpointName[MAX_ENDPOINTS];
    USBMgrEndpointQueueStruct           endpointQueue[MAX_TX_ENDPOINTS];  /* transmit endpoint queue address and pointers */
    USBMgrRxPrmsStruct                  endpointRxPrms[MAX_RX_ENDPOINTS];        /* receive  endpoint parameters*/
    USBMgr_EndpointStruct               *endpoint;    /**Endpoints structure -unit in DB **/
    USBMgr_CallbackIndicationStruct     callbackFunctions;      /**Callback Indication functions ***/
    struct USBMgrDataRegDBTypeStruct    *nextInterface;
}USBMgrDataRegDBStruct;


typedef struct{
    BOOL                    cableStatusReg;
    UINT8                   interfaceCounter;
    UINT32                  lisrRxEndpointsMask;
    UINT8                   lisrTxEndpoint;
    BOOL                    usbCableOut;
    BOOL                    continueTransmit;
    UINT16                  totalcfgDescSize;
    BOOL                    freeQueues;
    USBMgrDataRegDBStruct   *regDB;
}USBMgrDBGblPrmsStruct;


#endif /*_USBMGR_TYPES_H_*/
