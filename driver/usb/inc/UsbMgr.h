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
* Filename: USBMgr.h                                                 *
*                                                                    *
*
*                                                                    *
* Author: Miriam Yovel                                               *
*                                                                    *
* Description: This is the main header file of                       *
*              the USB Manager Package                                       *
*                                                                    *
* Notes: This file is using functions from  file              *
*
*********************************************************************/

#ifndef _USBMGR_H_
#define _USBMGR_H_

//#if defined(DIAG_OVER_USBMGR)
#include "common.h"
//#include "global_types.h"
#include "usb.h"
#include "usb_cable.h"

/* type definitions For Callback Functions */
/*------------------*/


typedef enum
{
    USBMGR_CABLE_IN,
    USBMGR_CABLE_OUT,
    USBMGR_INTERFACE_ACTIVE,
    USBMGR_INTERFACE_DEACTIVE,
    USBMGR_INTERFACE_CHANGED,
    /*USBMGR_ENUMURATION,*/
    USBMGR_RESET_DETECT,
    USBMGR_STATUS_NONE
}USBMgr_Status;  /** defines USB communication status ****/


typedef enum{
    USBMGR_BUFFERS_FULL,
    USBMGR_COMM_ERROR,
    USBMGR_ERROR_NONE
}USBMgr_Error; /** defines USB possible errors ****/


//Callback functions prototypes
//-----------------------------

/***
   UINT32       - cooky
   USBMgr_Status - USB Interface status
   USBMgr_Error  - USB communication error
   UINT32       - general parameter (depends on the notify reason)
***/

typedef void (*USBMgrControlIndicationFunc)(/*UINT32,*/ USBMgr_Status ,USBMgr_Error, UINT32 );

/***
   UINT32       - cooky
   UINT8        - endpoint
   UINT8 *       - pointer to data that had been transmiotted
   UINT32       - length of data that had been transmitted

***/

typedef void (*USBMgrTxIndicationFunc)(UINT32,UINT8,UINT8*,UINT32);



/***
   UINT8                      - endpoint
   UINT8 *                     - pointer to data that application had been confirmed on
   UINT32                     - length of data that  application had been confirmed on

***/




/***
   UINT32                     - cooky
   UINT8                      - endpoint
   UINT8 *                     - pointer to data that had been received
   UINT32                     - length of data that had been received
   USBMgrRecConfirmationFunc  - confirmation function
***/

typedef void (*USBMgrRxIndicationFunc)(/*UINT32,*/ UINT8, UINT8 *, UINT32  );







/* type definitions */
/*------------------*/

typedef enum
{
    USBMGR_EP_BULK,
    USBMGR_EP_ISO,
    USBMGR_EP_INT
}USBMgr_EPType;    /*** defines endpoint types ***/

typedef enum
{
    USBMGR_DIR_IN,
    USBMGR_DIR_OUT
}USBMgr_EPDir;       /*** defines endpoint direction ***/


typedef struct
{
    UINT32      txQueueLen;             /** numbers of entries within TX endpoint queue  -if this is IN endpoint **/
    UINT8       *optionalTxQueuePtr;    /***enables the application to request memory for TX queue  if it wishes to  ***/
    BOOL        multiTransmitEnabled ;  /***enable or disable multi transmit mode within TX endpoint**/
    BOOL        useExternalMem;         /*** whether to use internal or external memory ****/
    UINT32      cooky;
}USBMgr_TxDefStruct;


typedef struct{
    UINT32 rxBuffQuantumLen;     /*** quantum length of endpoint RX buffer. Application provides        basic length of RX buffer that will be  increase (in the same number )in any data reception event .for example the base quantity  that was allocated (as application requested) is 256Byte ,if received data (from USB stack) in first step is longer ,then USB Manager allocates 256*2Byte and if received data in the next step is still longer then 256*3Byte is being allocated, etc   . ****/
    UINT8  *optionalRxBufferPtr; /***enables the application to request memory for RX buffers. if it wishes to  ***/
    BOOL   useExternalMem;    /*** whether to use internal or external memory ****/
	BOOL   expect_zlp;
}USBMgr_RxDefStruct;


typedef struct
{
    USBMgr_EPType type;
    USBMgr_EPDir  direction;
    BOOL         useDMA;
    union
       {
          USBMgr_TxDefStruct  txDef;
          USBMgr_RxDefStruct  rxDef;
       }endpointDef;
}USBMgr_EndpointStruct;     /**Endpoint structure -unit in DB link list ***/


typedef struct
{
    UINT8       class;
    UINT8       subClass;
    UINT8       protocol;
}USBMgr_DeviceClassStruct;


typedef enum{
    USBMGR_DEVICE,
    USBMGR_INTERFACE,
    USBMGR_ENDPOINT,
    USBMGR_OTHER
}USBMgr_SetupDataRec;    /**defines setup request recipients ****/

typedef enum{
    USBMGR_STANDARD,
    USBMGR_CLASS,
    USBMGR_VENDOR
}USBMgr_SetupDataType;  /** defines setup request types ***/

typedef enum{
    USBMGR_HOST_TO_DEVICE,
    USBMGR_DEVICE_TO_HOST
}USBMgr_SetupDataStageDir; /** defines setup request data transfer definitions ***/


typedef enum{
    USBMGR_CLEAR_FEATURE        = 1,
    USBMGR_SET_FEATURE          = 3,
    USBMGR_GET_INTERFACE        = 10,
    USBMGR_SET_INTERFACE        = 11,
    USBMGR_VENDOR_CLASS         = 100
}USBMgr_SetupDataReq; /** defines setup command value definitions for application usage, setup commands that are not listed are handled by USB Manager and USB Stack***/



typedef struct{
    USBMgr_SetupDataRec          recipient;
    USBMgr_SetupDataType         type;
    USBMgr_SetupDataStageDir     direction;
    USBMgr_SetupDataReq          request;
    UINT16                       value;
    UINT16                       index;
    UINT16                       dataStagelength; /*apply to host-to-device and device-to-host*/
    UINT8                       *hostToDeviceDataPtr; /* if there is Device to Host data it will pass in the response callback and this structure field will be NULL **/
    UINT8                        setup_packet[8]; //need define
}USBMgr_SetupDataStruct;  /** standard device request structure***/


//Set up Callback functions prototypes
//-----------------------------


/***
 UINT8 *  -   setup packet data stage from device to host
 UINT16      setup packet data length from device to host
 ***/

typedef void (*USBMgrSetupCommandRspFunc)(UINT8* ,UINT16);


/***
   UINT32                     - cooky
   SetupDataStruct            - Setup Request structure
 ***/

typedef void (*USBMgrSetupCommandIndicationFunc)(/*UINT32,*/USBMgr_SetupDataStruct *);






//Callback function Struct

typedef struct{
    USBMgrControlIndicationFunc         controllIndFunc ;
    USBMgrSetupCommandIndicationFunc    setupCommandIndFunc;
    USBMgrTxIndicationFunc              TxIndFunc;
    USBMgrRxIndicationFunc              RxIndFunc;
}USBMgr_CallbackIndicationStruct;      /**Callback Indication functions Struct ***/



typedef  enum{
	USBMGR_RC_DEVICE_NOT_CONNECTED = -11,
	USBMGR_RC_GENERAL_ERROR,
    USBMGR_RC_REGISTER_ERROR,
    USBMGR_RC_ENDPOINT_IN_USE,
    USBMGR_RC_ENDPOINT_INVALID,
    USBMGR_RC_INTERFACE_NOT_ACTIVE,
    USBMGR_RC_INTERFACE_INVALID,
    USBMGR_RC_TX_BUFFER_FULL,
    USBMGR_RC_BUFFER_ERROR ,
    USBMGR_RC_TRANSMIT_ERROR,
    USBMGR_RC_ENDPOINT_STALLED,
    USBMGR_RC_NO_MSG,
    USBMGR_RC_OK = 1
}USBMgr_RC;





typedef void *          USBMGR_IF_HANDLER;




//API list
//--------


USBMgr_RC   USBMgrRegister(UINT8 numEndpoint, USBMgr_EndpointStruct *endpointCfg, USBMgr_DeviceClassStruct *deviceClass,
                         CHAR *interfaceString, UINT8 appID, USBMgr_CallbackIndicationStruct *indicationCallback, USBMGR_IF_HANDLER *interfaceHnd);
USBMgr_RC   USBMgrUnregister( USBMGR_IF_HANDLER interfaceHnd);
void        USBMgrFlushTxQueue( USBMGR_IF_HANDLER interfaceHnd ,UINT8 endpoint);
void        USBMgrFlushRxBuffer ( USBMGR_IF_HANDLER interfaceHnd ,UINT8 endpoint);
USBMgr_RC   USBMgrTransmit(USBMGR_IF_HANDLER interfaceHnd,UINT8 endpoint, UINT8 *dataBuffer , UINT32 dataLen,BOOL send_zlp );
USBMgr_RC   USBMgrEndpointStall(USBMGR_IF_HANDLER interfaceHnd,UINT8 endpoint);
void        USBMgrRxConfirmationExt(USBDevice_EndpointE endpoint ,UINT32 next_rx_packet_length, BOOL expect_zlp);
void        USBMgrRxConfirmation(USBDevice_EndpointE endpoint);
void		USBMgrSetupCommandRsp(UINT8 *setupPacket ,UINT16 setupLen);
void        USBMgrPhase1Init(void);
void        USBMgrPhase2Init(void);
void        USBMgrDeviceUnplug(void);
void        USBMgrDevicePlugIn(void);
void        USBMgrTempFuncUpdateHardCodedUSBDescriptor(UINT32 usb_app_mask);


UINT8 UsbMgrGetIfIndexByAppId(UINT8 appID);
UINT32 get_current_usb_app_mask(void);

//USB endpoints HW configuration functions
UINT16 USBMgrGetHWCfgEPMaxPacketSize(USBDevice_EndpointE endpoint);
USBMgr_EPType USBMgrGetHWCfgEPType(USBDevice_EndpointE endpoint);
USBMgr_EPDir USBMgrGetHWCfgEPDirection(USBDevice_EndpointE endpoint);



//#endif /*USBMGR*/



#endif /*_USBMGR_H_*/
