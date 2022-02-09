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

/********************************************************************
*                      M O D U L E     B O D Y                       *
**********************************************************************
* Title: Universal Serial Bus Manager                                *
*                                                                    *
* Filename: USBMgr.c                                                 *
*                                                                    *
*                                                                    *
*                                                                    *
* Author:                                                            *
*                                                                    *
* Description: This is the main file to handle the                   *
*              the USB Manager Package                               *
*                                                                    *
* Notes: This file is using functions from usbmgr.c file             *
**********************************************************************/


#include <string.h>
#include "usb_init.h"
//#include "utils.h"
//#include "usb.h"
//#include "csw_mem.h"
//#include "bsp_hisr.h"
#include "UsbMgr.h"
#ifdef LWIP_IPNETBUF_SUPPORT
#include "usbTrace.h"
#endif
#include "usbmgr_types.h"
#include "UsbMgr_config.h"
#include "UsbMgr_util.h"
//#include "usbmgr_apps_def.h"
#include "usbmgr_apps_config.h"

#include "Usbmgr_Descriptors.h"

//#include "usbmgrtest.h"

#include "udc_config.h"

#ifndef LWIP_IPNETBUF_SUPPORT
#include <bsp.h>
#ifdef MV_USB2_MASS_STORAGE
#include "mvUsbStorage.h"
#endif
/**********************************************************************/
/*-- Local variables declarations --------------------------------------*/


//USBMgrDataRegDBStruct         *_regDB = 0;
//static BOOL                 _usbmgrEndpointAvailable[USB_DEVICE_TOTAL_ENDPOINTS] ;
//static BOOL                 _usbMgrCableStatusReg;
//static BOOL                   _usbMgrUsbCableOut;
//static BOOL                   _usbMgrFreeQueues;
//static BOOL                   _continueTransmit;
//static BOOL                 _usbMgrTxQFull[USB_DEVICE_TOTAL_ENDPOINTS];
//static UINT8                  _usbMgrInterfaceHnd;   /**registration Handle**/
static USBMgrDBGblPrmsStruct  _usbMgrPrm;


          /*Indication Parameters */
static USBMgrLisrPrmsStruct    _usbMgrLisrParams;

/*static USBMgrLisrTxPrmsStruct   _usbMgrLisrTxParams[MAX_TX_ENDPOINTS];
static USBMgrLisrRxPrmsStruct     _usbMgrLisrRxParams[MAX_RX_ENDPOINTS];
static USBMgrLisrCtrlPrmsStruct  _usbMgrLisrCtrlParams;
static USBMgrLisrSetupPrmsStruct _usbMgrLisrSetupParams; */

//static UINT8                     _usbMgrLisrTxEndpoint;
//static UINT8                     _usbMgrLisrRxEndpoint;

static unsigned char bEp0MaxPacketSize=0, bcdUSB_LSB=0, bcdUSB_MSB=0, bBulkMaxPacketSize_LSB=0, bBulkMaxPacketSize_MSB=0, bInterval=0;
static USBDevice_USBVersionE  _usbMgr_UsbVersion = USB_VER_2_0 /*USB_VER_1_1*/;
static USBDevice_USBSpeedE  _usbMgr_UsbSpeed = USB_SPEED_FS /* Full Speed */;

__align(8)  UINT8 _modem_rx_data_buffer[4096];
__align(8)  UINT8 _modem_rx_data_buffer_modem2[512];

__align(8)  UINT8 _modem_tx_test_buffer[8]={'e','r','r','o','r'};



/********** Descriptors Parameters **********************************************************/


//static BOOL                  _usbMgrfirstRegistration;
static UINT8                  descriptorSizeTbl[MAX_ENDPOINTS];  /* actually maximum size of registered interfaces*/
static UINT8                  *_usbMgrDevDescriptor = NULL;
static UINT8                  *_usbMgrCfgDescriptor = NULL;
static UINT8                  *_usbMgrQualifierDescriptor = NULL;
static UINT8                  *_usbMgrOtherSpeedDescriptor = NULL;
static UINT8                  *_usbMgrRxDataBuffer = NULL;
static UINT8                  *_usbMgrDmaDescriptor = NULL;

//static UINT16                  _usbMgrTotalcfgDescSize;

static UINT8                  _enableHISRs = FALSE;
//will indicate for tx HISR which enpoint is ready
static UINT8                  tx_completed_endpoints_num[MAX_ENDPOINTS];


//EP 0  tranfser status , for debug
UINT8   ep0_setup_transfer_started;

/***************************************************************************************************************************/
static OS_HISR    USBMgrTxHISRBlock; /* HISR control block */
static OS_HISR    USBMgrRxHISRBlock; /* HISR control block */
static OS_HISR    USBMgrCtrlHISRBlock; /* HISR control block */
static OS_HISR    USBMgrSetupHISRBlock; /* HISR control block */

static OSATaskRef usbMgrRxTaskRef;

static  UINT32    usbMgrRxTaskStack[USBMGR_STACK_SIZE/sizeof(UINT32)];

OSAFlagRef    	  usbMgrRxEventRef;

static UINT32     usbMgrRxEventsMask=0;
static UINT32     usbMgrRxEventsMaskUpdated=0xffffffff;//first time expect everything

/************************************ Static Tables ********************************************************************************/
USBMgrDBInfoStruct    _usbMgrDBEpArry[USB_DEVICE_TOTAL_ENDPOINTS];
/*************************** Private DEFINITIONS ***********************************************************************************/

# define     SINGLE_INTERFACE      1




static void UpdateRXEventMask(UINT8 out_ep_number)
{
    usbMgrRxEventsMask|= (UINT32)(((UINT32)1)<<out_ep_number);
    usbMgrRxEventsMaskUpdated = usbMgrRxEventsMask;
}

/*******  Temporary -for Yossi ********************************************************************/
#define _DEVICE_DESCRIPTOR 0
#define _CONFIG_DESCRIPTOR 1

void USBSetDescriptor( USB_DescriptorTypesE descType, UINT8 *pDesc, UINT8 descLength, UINT8 id)
{
}

/************************************************************************
* Function:  USBMgrFindAppEndpoint
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes: alla - temp static endpoint allocation by APPid , only htose are supported
************************************************************************/

static USBMgr_RC USBMgrFindAppEndpoint (UINT8 numEndpoint ,USBMgr_EndpointStruct *endpointInfo , USBMGREndpointsReqStruct endpointReq[] /*output parameter*/, UINT8 appID  )
{
    UINT8       ep;

    switch( appID )
    {
    case USBMGR_GENIE_APPID:
        for(ep = 0; ep<numEndpoint ;ep++ ,endpointInfo++)
        {
            if(endpointInfo->direction == USBMGR_DIR_IN)
            {

                endpointReq[ep].endpoint = USB_EMMI_IN_ENDPOINT_PHY;
                endpointReq[ep].endpointDir = USBMGR_DIR_IN;
            }
            else
            {
                endpointReq[ep].endpoint = USB_EMMI_OUT_ENDPOINT_PHY;
                UpdateRXEventMask(USB_EMMI_OUT_ENDPOINT_PHY);
                endpointReq[ep].endpointDir = USBMGR_DIR_OUT;
            }

            _usbMgrDBEpArry[endpointReq[ep].endpoint].available = FALSE;
        }
      break;

    case USBMGR_MAST_APPID:
        for(ep = 0; ep<numEndpoint ;ep++ ,endpointInfo++)
        {
            if(endpointInfo->direction == USBMGR_DIR_IN)
            {
                endpointReq[ep].endpoint = USB_MAST_IN_ENDPOINT_PHY;
                endpointReq[ep].endpointDir = USBMGR_DIR_IN;

            }
            else
            {
                endpointReq[ep].endpoint = USB_MAST_OUT_ENDPOINT_PHY;
                UpdateRXEventMask(USB_MAST_OUT_ENDPOINT_PHY);
                endpointReq[ep].endpointDir = USBMGR_DIR_OUT;

            }
            _usbMgrDBEpArry[endpointReq[ep].endpoint].available = FALSE;
        }
      break;

      case USBMGR_MODEM_APPID:
        for(ep = 0; ep<numEndpoint ;ep++ ,endpointInfo++)
        {
            if(endpointInfo->type == USBMGR_EP_BULK)
            {
                if(endpointInfo->direction == USBMGR_DIR_IN)
                {
                    endpointReq[ep].endpoint = USB_COMM_DATA_IN_ENDPOINT_PHY;
                    endpointReq[ep].endpointDir = USBMGR_DIR_IN;
                }
                else
                {
                    endpointReq[ep].endpoint = USB_COMM_DATA_OUT_ENDPOINT_PHY;
                    UpdateRXEventMask(USB_COMM_DATA_OUT_ENDPOINT_PHY);
                    endpointReq[ep].endpointDir = USBMGR_DIR_OUT;
                }
            }
            else
            { //ctrl

                endpointReq[ep].endpoint = USB_COMM_CTRL_IN_ENDPOINT_PHY;
                endpointReq[ep].endpointDir = USBMGR_DIR_IN;
            }

            _usbMgrDBEpArry[endpointReq[ep].endpoint].available = FALSE;
        }
      break;

      case USBMGR_ICAT_APPID:
        for(ep = 0; ep<numEndpoint ;ep++ ,endpointInfo++)
        {
            if(endpointInfo->direction == USBMGR_DIR_IN)
            {
                endpointReq[ep].endpoint = USB_ICAT_IN_ENDPOINT_PHY;
                endpointReq[ep].endpointDir = USBMGR_DIR_IN;

            }
            else
            {
                endpointReq[ep].endpoint = USB_ICAT_OUT_ENDPOINT_PHY;
                UpdateRXEventMask(USB_ICAT_OUT_ENDPOINT_PHY);
                endpointReq[ep].endpointDir = USBMGR_DIR_OUT;
            }
            _usbMgrDBEpArry[endpointReq[ep].endpoint].available = FALSE;
        }
      break;
      default:
          ASSERT(0); //!!! currently no other applications are supported

    };

    return(USBMGR_RC_OK);
}
/************************************************************************
* Function: USBMgrFindEpAddrAndSize
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrFindEpAddrAndSize( USBDevice_EndpointE endpoint , UINT8 *endpointAddr ,UINT16 *endpointSize ,UINT8 *endpointType)
{

    switch(endpoint)
    {
        case USB_DEVICE_ENDPOINT_1:
            *endpointAddr = USBMGR_BULK_IN1_ADDR;
            *endpointSize = USBMGR_BULK_IN_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


        case USB_DEVICE_ENDPOINT_2:
            *endpointAddr = USBMGR_BULK_OUT1_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


        case USB_DEVICE_ENDPOINT_3:
            *endpointAddr = USBMGR_ISO_IN_ADDR;
            *endpointSize = USBMGR_ISO_IN_LENGTH;
            *endpointType = USBMGR_ISO_TYPE ;
        break;


        case USB_DEVICE_ENDPOINT_4:
            *endpointAddr = USBMGR_ISO_OUT_ADDR;
            *endpointSize = USBMGR_ISO_OUT_LENGTH;
            *endpointType = USBMGR_ISO_TYPE ;
        break;


        case USB_DEVICE_ENDPOINT_5:
            *endpointAddr = USBMGR_INT_IN_ADDR;
            *endpointSize = USBMGR_INT_IN_LENGTH;
            *endpointType = USBMGR_INT_TYPE ;
        break;


        case USB_DEVICE_ENDPOINT_6:
            *endpointAddr = USBMGR_INT_OUT_ADDR;
            *endpointSize = USBMGR_INT_OUT_LENGTH;
            *endpointType = USBMGR_INT_TYPE;
        break;


      case USB_DEVICE_ENDPOINT_7:
            *endpointAddr = USBMGR_BULK_IN2_ADDR;
            *endpointSize = USBMGR_BULK_IN_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;
//dummy part
      case USB_DEVICE_ENDPOINT_8:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;

      case USB_DEVICE_ENDPOINT_9:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;

       case USB_DEVICE_ENDPOINT_10:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


         case USB_DEVICE_ENDPOINT_11:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


         case USB_DEVICE_ENDPOINT_12:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


         case USB_DEVICE_ENDPOINT_13:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


         case USB_DEVICE_ENDPOINT_14:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;


         case USB_DEVICE_ENDPOINT_15:
            *endpointAddr = USBMGR_BULK_OUT2_ADDR;
            *endpointSize = USBMGR_BULK_OUT_LENGTH;
            *endpointType = USBMGR_BULK_TYPE;
        break;

      default:
            ASSERT(0);
    }

}




/************************************************************************
* Function: USBMgrUpdateDeviceDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrUpdateDeviceDescriptor( UINT8 *descriptor , UINT8 appID, BOOL add)
{
    USBMgrDevDescStruct   *devDesc;

    devDesc = (USBMgrDevDescStruct*)descriptor;
}

/************************************************************************
* Function: USBMgrInitDeviceDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrInitDeviceDescriptor( USBMgrDevDescStruct *descriptor )
{

    descriptor->devDescLength       = DEVICE_DESC_LENGTH;
    descriptor->devDescType         = DEVICE_DESC_TYPE;
    descriptor->devDescUSBProtocol  = DEVICE_DESC_USB_PROTOCOL;
    descriptor->devDescClass        = DEVICE_DESC_CLASS;
    descriptor->devDescSubClass     = DEVICE_DESC_SUBCLASS;
    descriptor->devDescProtocol     = DEVICE_DESC_PROTOCOL;
    descriptor->devDescEP0MaxSize   = DEVICE_DESC_EP0_SIZE;
    descriptor->devDescVendorID     = 0;
    descriptor->devDescPrdID        = USBMGR_CUST_BASE_PRODOUCT_ID;
    descriptor->devDescBCD          = DEVICE_DESC_BCD;

    if(strlen(USBMGR_STRING_MANUFACTURER) == 0)
    {
        descriptor->devDescManStrIdx = DEVICE_DESC_NO_STRING;
    }
    else
    {
        descriptor->devDescManStrIdx = DEVICE_DESC_MAN_STR;
    }

    if(strlen(USBMGR_STRING_PRODUCT ) == 0)
    {
        descriptor->devDescPrdStrIdx = DEVICE_DESC_NO_STRING;
    }
    else
    {
        descriptor->devDescPrdStrIdx = DEVICE_DESC_PRD_STR;
    }

    if(strlen(USBMGR_STRING_SERIAL_NUMBER ) == 0)
    {
        descriptor->devDescSerNoStr = DEVICE_DESC_NO_STRING;
    }
    else
    {
        descriptor->devDescSerNoStr = DEVICE_DESC_SER_NO_STR;
    }

    descriptor->devDescCfgNo = DEVICE_DESC_CFG_NO;
}


/************************************************************************
* Function: USBMgrUpdateIntDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
 static void USBMgrUpdateIntDescriptor( USBMgrCfgDescIntStruct *descriptor ,UINT8 appID  , USBMgr_DeviceClassStruct *deviceClass ,UINT8 numberOfEndpoints)
 {

     descriptor->cfgDescIntLength     = INT_DESC_LENGTH;
     descriptor->cfgDescIntType       = CONFIG_INT_DESC_TYPE;
     descriptor->cfgDescIntIdxNo      = (UINT8) appID;
     descriptor->cfgDescAltSetting    = 0;
     descriptor->cfgDescEndpointNo    = numberOfEndpoints;
     descriptor->cfgDescIntClass      = deviceClass->class;
     descriptor->cfgDescIntSubClass   = deviceClass->subClass;
     descriptor->cfgDescIntProtocol   = deviceClass->protocol;
     descriptor->cfgDescIntStr        = DEVICE_DESC_INT_STR;
 }

/************************************************************************
* Function: USBMgrUpdateEpDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
 static  void USBMgrUpdateEpDescriptor( USBMgrCfgDescEpStruct *descriptor ,UINT8 appID  ,UINT8 numberOfEndpoints , USBDevice_EndpointE endpoint)
 {
    UINT8  endpointAddr;
    UINT8  endpointType;
    UINT16 endpointSize;

    USBMgrFindEpAddrAndSize(endpoint , &endpointAddr ,&endpointSize, &endpointType);

    descriptor->cfgDescEpLength = EP_DESC_LENGTH ;
    descriptor->cfgDescEpType = CONFIG_EP_DESC_TYPE;
    descriptor->cfgDescEpAdd =  endpointAddr;
    descriptor->cfgDescEpTransferType = endpointType;
    descriptor->cfgDescEpMaxSize = endpointSize;
    descriptor->cfgDescEpPollInter =0;
 }


/************************************************************************
* Function: USBMgrUpdateCfgDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void *USBMgrUpdateCfgDescriptor( UINT8 *descriptor,UINT16 descriptorSize ,UINT8 appID ,USBMgr_DeviceClassStruct *deviceClass ,UINT8 numberOfEndpoints ,USBMgr_EndpointStruct *endpointsDef ,UINT8 endpoints[])
{
    UINT8                      epIdx;
    USBMgr_EndpointStruct      *epDefIdx;
    UINT8                      *newCfgDec;
    UINT8                      *intDesc ,*epDesc;
    USBMgrCfgDescHeaderStruct  *cfgDec;

    cfgDec = (USBMgrCfgDescHeaderStruct*)descriptor;

    cfgDec->cfgDescTotalLength += INT_DESC_LENGTH + numberOfEndpoints * EP_DESC_LENGTH;
    cfgDec->cfgDescInterfaceNo++;

    /*update for unregister information*/
    descriptorSizeTbl[_usbMgrPrm.interfaceCounter] =  INT_DESC_LENGTH + numberOfEndpoints * EP_DESC_LENGTH;

    if(_usbMgrPrm.interfaceCounter)
    {
        newCfgDec = USBMgrUtilAllocateDescriptors(FALSE ,(UINT16)(descriptorSize + sizeof(USBMgrCfgDescIntStruct) + (numberOfEndpoints*sizeof(USBMgrCfgDescEpStruct))) );
        memcpy(newCfgDec , descriptor , (UINT32) ( /*sizeof(USBMgrCfgDescHeaderStruct) + */ descriptorSize) );
        free((void*)descriptor);
        intDesc = newCfgDec + descriptorSize;

    }
    else
    {
        newCfgDec = descriptor;
        intDesc = newCfgDec +sizeof(USBMgrCfgDescHeaderStruct);
    }
    USBMgrUpdateIntDescriptor((USBMgrCfgDescIntStruct*)intDesc ,appID , deviceClass , numberOfEndpoints );

    for(epIdx = 0,epDefIdx = endpointsDef ; epIdx < numberOfEndpoints ; epIdx++ ,epDefIdx++)
    {
        epDesc = intDesc+ sizeof(USBMgrCfgDescIntStruct) + epIdx* sizeof(USBMgrCfgDescEpStruct);
        USBMgrUpdateEpDescriptor( (USBMgrCfgDescEpStruct*)epDesc ,appID ,numberOfEndpoints, (USBDevice_EndpointE )endpoints[epIdx] );
    }
    return((void *)newCfgDec);
}

/************************************************************************
* Function: USBMgrDelCfgDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void *USBMgrDelCfgDescriptor( UINT8 *descriptor,UINT8 interfaceOffset , UINT8 ifNumberOfEndpoints )
{

    UINT8                       *newCfgDec , *cfgDecIdx ,*newCfgDecIdx;
    USBMgrCfgDescHeaderStruct   *cfgHdrDesc;

    UINT8   ifIndex     = 0;
    UINT16  totalSize   = 0;



    cfgDecIdx = descriptor + sizeof(USBMgrCfgDescHeaderStruct);


    newCfgDec = USBMgrUtilAllocateDescriptors(FALSE ,(UINT16)(_usbMgrPrm.totalcfgDescSize - sizeof(USBMgrCfgDescIntStruct) - (ifNumberOfEndpoints*sizeof(USBMgrCfgDescEpStruct))) );
	if(newCfgDec == NULL)
	{
		ASSERT(0);
		/*Fixed coverity[missing_return]*/
		return NULL;
	}

    memcpy(newCfgDec , descriptor , sizeof(USBMgrCfgDescHeaderStruct) );

    cfgHdrDesc = (USBMgrCfgDescHeaderStruct*)newCfgDec;

    cfgHdrDesc->cfgDescTotalLength = cfgHdrDesc->cfgDescTotalLength- sizeof(USBMgrCfgDescIntStruct) - (ifNumberOfEndpoints*sizeof(USBMgrCfgDescEpStruct));

    cfgHdrDesc->cfgDescInterfaceNo--;


    newCfgDecIdx = newCfgDec +sizeof(USBMgrCfgDescHeaderStruct);
     /*start from primery descriptors */

    while(interfaceOffset != ifIndex)
    {
        memcpy(newCfgDecIdx, cfgDecIdx  ,descriptorSizeTbl[ifIndex] );
        newCfgDecIdx    += descriptorSizeTbl[ifIndex];
        cfgDecIdx       += descriptorSizeTbl[ifIndex];
        totalSize       += descriptorSizeTbl[ifIndex];
        ifIndex++;
    }

    if(_usbMgrPrm.interfaceCounter != interfaceOffset +1 ) /*this is not the last interface in cfg descriptor */
    {
        totalSize       += descriptorSizeTbl[interfaceOffset];
        memcpy(newCfgDecIdx ,cfgDecIdx +descriptorSizeTbl[interfaceOffset] ,(UINT32)(_usbMgrPrm.totalcfgDescSize  - totalSize));

    }

    _usbMgrPrm.totalcfgDescSize = cfgHdrDesc->cfgDescTotalLength;
    free((void*)descriptor); /*free old descriptor*/

    return((void *)newCfgDec);
}
/************************************************************************
* Function: USBMgrInitCfgDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrInitCfgDescriptor( USBMgrCfgDescHeaderStruct *descriptor )
{

    descriptor->cfgDescHLength = CONFIG_DESC_H_LENGTH;
    descriptor->cfgDescType =  CONFIG_DESC_TYPE;
    descriptor->cfgDescTotalLength = CONFIG_DESC_H_LENGTH;
    descriptor->cfgDescInterfaceNo = 0;
    descriptor->cfgDescValue = CONFIG_DESC_VALUE;
    descriptor->cfgDescStrIdx = CONFIG_DESC_STR_IDX;
    descriptor->cfgDescAttr = CONFIG_DESC_ATTR;
    descriptor->cfgDescMaxPower = CONFIG_DESC_MAX_POWER;
}


/************************************************************************
* Function: USBMgrSetDescriptors
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrSetDescriptors( UINT8 endpointNum ,UINT8 appID , USBMgr_DeviceClassStruct *deviceClass ,USBMgr_EndpointStruct *endpointsDef ,UINT8 endpoints[] )
{



    if(!_usbMgrPrm.interfaceCounter)
    {
        _usbMgrDevDescriptor = USBMgrUtilAllocateDescriptors(FALSE ,(UINT16) sizeof(USBMgrDevDescStruct));

		//[klockwork][issue id: 189]
		if(_usbMgrDevDescriptor == NULL)
		{
			ASSERT(0);
			return ;
		}
		
        USBMgrInitDeviceDescriptor( (USBMgrDevDescStruct *)_usbMgrDevDescriptor);

        _usbMgrCfgDescriptor = (UINT8*)USBMgrUtilAllocateDescriptors(FALSE ,(UINT16)(sizeof(USBMgrCfgDescHeaderStruct)+ sizeof(USBMgrCfgDescIntStruct) + (endpointNum*sizeof(USBMgrCfgDescEpStruct)) ));

		//[klockwork][issue id: 183]
		if(_usbMgrCfgDescriptor == NULL)
		{
			ASSERT(0);
			return ;
		}
		
        USBMgrInitCfgDescriptor((USBMgrCfgDescHeaderStruct *)_usbMgrCfgDescriptor);
		/*unused code*/
		/*coverity[leaked_storage]*/
        USBMgrUpdateCfgDescriptor(_usbMgrCfgDescriptor, 0 ,appID ,deviceClass ,endpointNum ,endpointsDef ,endpoints );
        _usbMgrPrm.totalcfgDescSize = sizeof(USBMgrCfgDescHeaderStruct);

    }
    else
        _usbMgrCfgDescriptor = (UINT8* )USBMgrUpdateCfgDescriptor(_usbMgrCfgDescriptor ,_usbMgrPrm.totalcfgDescSize/*-sizeof(USBMgrCfgDescHeaderStruct) */,appID , deviceClass ,endpointNum ,endpointsDef ,endpoints );
    USBMgrUpdateDeviceDescriptor(_usbMgrDevDescriptor , appID ,TRUE);
    USBSetDescriptor( (USB_DescriptorTypesE)_DEVICE_DESCRIPTOR ,_usbMgrDevDescriptor , sizeof(USBMgrDevDescStruct), 0);

    _usbMgrPrm.totalcfgDescSize += sizeof(USBMgrCfgDescIntStruct) + (endpointNum*sizeof(USBMgrCfgDescEpStruct));
    USBSetDescriptor( (USB_DescriptorTypesE)_CONFIG_DESCRIPTOR ,_usbMgrCfgDescriptor , _usbMgrPrm.totalcfgDescSize, 0);

}


/************************************************************************
  Function: USBMgrSetRegisterDBDefault
* Function:
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrSetRegisterDBDefault(USBMgrDataRegDBStruct *regDB)
{
    UINT8 regIndex ,txRegIdx,rxRegIdx;


    regDB->numOfEndpoints       = 0;

    for ( regIndex = 0; regIndex<MAX_ENDPOINTS; regIndex++)
        regDB->endpointName[regIndex] = (USBDevice_EndpointE)0;

    for ( txRegIdx = 0 ; txRegIdx<MAX_TX_ENDPOINTS; txRegIdx++)
    {
        regDB->endpointQueue[txRegIdx].txEndpoint                           = 0;
        regDB->endpointQueue[txRegIdx].txEndpointBusy                       = FALSE;
        regDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.nextItemToRel    = 0;
        regDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.nextItemToSend   = 0;
        regDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.nextFreeItem     = 0;
        regDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.freeAgain        = 0;
        regDB->endpointQueue[txRegIdx].endpointTxQueueAddr                  = NULL;
        regDB->endpointQueue[txRegIdx].fullQueue                            = FALSE;
    }

    for ( rxRegIdx = 0 ; rxRegIdx<MAX_RX_ENDPOINTS; rxRegIdx++)
    {
        regDB->endpointRxPrms[rxRegIdx].rxEndpoint      = 0;
        regDB->endpointRxPrms[rxRegIdx].in_first_message_chunk       = TRUE;
        regDB->endpointRxPrms[rxRegIdx].list_of_rx_data_start        = NULL;
        regDB->endpointRxPrms[rxRegIdx].list_of_rx_data_curr         = NULL;
        regDB->endpointRxPrms[rxRegIdx].rx_total_data_length         = 0;
        regDB->endpointRxPrms[rxRegIdx].rx_ext_buffer                = 0;
        regDB->endpointRxPrms[rxRegIdx].rx_ext_buffer_length         = 0;
    }
    regDB->endpoint           = NULL;
    regDB->nextInterface      = NULL;



}

/************************************************************************
* Function: USBMgrRetrieveOUTEndpoint
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes: bit index set indicates the number of endpoints
************************************************************************/
static void USBMgrRetrieveOUTEndpoint( UINT32 rxFlag ,UINT8 *endpoint )
{
    UINT8 endpoint_num=0;

    while( (rxFlag & 1)  == 0)
    {
        rxFlag>>=1;
        endpoint_num++;
    }

   *endpoint = endpoint_num;
}


/************************************************************************
* Function:  USBMgrUSBRxNotify
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void USBMgrUSBRxNotify(USBDevice_EndpointE endpoint ,UINT8 *dataRecPtr ,UINT32 dataRecLen ,BOOL endOfRecMsg)
{
    STATUS osStatus;
    UINT8  rxEndpointIdx;


    rxEndpointIdx = _usbMgrDBEpArry[endpoint].rxLisrIdx;

    _usbMgrLisrParams.rxPrms[rxEndpointIdx].rxEndpoint = endpoint;
    _usbMgrLisrParams.rxPrms[rxEndpointIdx].recPtr = dataRecPtr;
    _usbMgrLisrParams.rxPrms[rxEndpointIdx].recLen = dataRecLen;
    _usbMgrLisrParams.rxPrms[rxEndpointIdx].endOfRecMsg = endOfRecMsg;
    _usbMgrPrm.lisrRxEndpointsMask |= (1<<endpoint); /* Information for sending event */


    osStatus = OS_Activate_HISR( &USBMgrRxHISRBlock ) ;
    ASSERT(osStatus == OS_SUCCESS);


}

/************************************************************************
* Function:  USBMgrUSBMultiTxCompletedNotify
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void USBMgrUSBMultiTxCompletedNotify(USBDevice_EndpointE endpoint ,UINT8 *p, UINT32 noOfTxBuffers ,BOOL allTransmit )
{

    STATUS osStatus;
    UINT8  txEndpointIdx;

     txEndpointIdx = _usbMgrDBEpArry[endpoint].txLisrIdx;

    _usbMgrLisrParams.txPrms[txEndpointIdx].txEndpoint = endpoint;
    _usbMgrLisrParams.txPrms[txEndpointIdx].noOfTxBuffers = noOfTxBuffers;
    _usbMgrLisrParams.txPrms[txEndpointIdx].transmitAll = allTransmit;
    tx_completed_endpoints_num[endpoint] = 1; //indicate that certain endpoint received tx completed
    osStatus = OS_Activate_HISR( &USBMgrTxHISRBlock );
    ASSERT(osStatus == OS_SUCCESS);

}

/************************************************************************
* Function:  USBMgrUSBTxCompletedNotify
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void USBMgrUSBTxCompletedNotify(USBDevice_EndpointE endpoint,  UINT8 *p, UINT32 len ,BOOL dummy)
{
    STATUS osStatus;
    UINT8  txEndpointIdx;



     txEndpointIdx = _usbMgrDBEpArry[endpoint].txLisrIdx;

    _usbMgrLisrParams.txPrms[txEndpointIdx].txEndpoint = endpoint;
    _usbMgrLisrParams.txPrms[txEndpointIdx].noOfTxBuffers = 0;
    _usbMgrLisrParams.txPrms[txEndpointIdx].transmitAll = FALSE;

    tx_completed_endpoints_num[endpoint] = 1; //indicate that certain endpoint received tx completed

    osStatus = OS_Activate_HISR( &USBMgrTxHISRBlock);
    ASSERT(osStatus == OS_SUCCESS);

}


/************************************************************************
* Function:  USBMgrUSBStatusNotify
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void USBMgrUSBStatusNotify(USBDevice_StatusE usbStatus  )
{
    STATUS                 osStatus;



    switch(usbStatus)
    {
        case USB_DEVICE_STATUS_NOT_CONNECTED:     /* usb is not connected */
          _usbMgrLisrParams.ctrlPrms.usbMgrStatus = USBMGR_CABLE_OUT;
        //alla - we currently do not support cable detection

          break;

        case USB_DEVICE_STATUS_RESET:
            _usbMgrLisrParams.ctrlPrms.usbMgrStatus = USBMGR_RESET_DETECT;
          break;


        case USB_DEVICE_STATUS_CONNECTED:
            _usbMgrLisrParams.ctrlPrms.usbMgrStatus = USBMGR_INTERFACE_ACTIVE;
          break;

        case USB_DEVICE_STATUS_SUSPEND:
            _usbMgrLisrParams.ctrlPrms.usbMgrError = USBMGR_COMM_ERROR;
          break;

        case USB_DEVICE_STATUS_ENUM_IN_PROCESS:
          break;

        default:
          ASSERT(0);
          break;
    }

    osStatus = OS_Activate_HISR( &USBMgrCtrlHISRBlock ) ;
    ASSERT(osStatus == OS_SUCCESS);

}

/************************************************************************
* Function:  USBMgrUSBClassReqNotify
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
UINT32 vendor_hisr_wake_cnt=0;
void USBMgrUSBVendorClassReqNotify (USB_SetupCmdS *packet)
{

    STATUS                 osStatus;

	memcpy(_usbMgrLisrParams.setUpPrms.setupData,(UINT8*)packet,8);
    _usbMgrLisrParams.setUpPrms.setupType     = USBMGR_CLASS;
    _usbMgrLisrParams.setUpPrms.p_data = packet->p_data; // pointer to data

	vendor_hisr_wake_cnt++;
    ep0_setup_transfer_started=TRUE;

     osStatus = OS_Activate_HISR( &USBMgrSetupHISRBlock ) ;
    ASSERT(osStatus == OS_SUCCESS);

}

void USBMgrDeviceEndpointZeroNotifyFn(USB_SetupCmdS * p_packet)
{
	return;
}
/************************************************************************
* Function: USBMgrInitUSBRegistration
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static USBMgr_RC  USBMgrInitUSBRegistration(void)
{

    //alla- temp patch will be fixed after diag is over manager
    if(USBDeviceRegisterPatchTempFunc(USBMgrUSBStatusNotify, USBMgrUSBVendorClassReqNotify, USBMgrDeviceEndpointZeroNotifyFn) != USB_DEVICE_RC_OK)
        return(USBMGR_RC_REGISTER_ERROR);

	return USBMGR_RC_OK; /* added in warning sweep - a possible bug? */
}

/************************************************************************
* Function: USBMgrMultiTransmit
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static USBMgr_RC USBMgrMultiTransmit(USBDevice_EndpointE endpoint , USBMgrDataRegDBStruct *interfaceDB)
{
    USBCDevice_ReturnCodeE        usbStatus;
    USBMgrSendMessage     *msg;
    UINT8                 queueIndex ,txIdx;
    USBMgr_EndpointStruct *endpointIndex;
    UINT32                noOfMsg,queueSize ,multiListFirstSize;
    USBDevice_MultiTransmitListS *multiList;
    USBMgr_RC             usbMgrStatus;
    UINT32                cpsr;


    endpointIndex   = interfaceDB->endpoint +(_usbMgrDBEpArry[endpoint].endpointIdx);
    queueSize       = endpointIndex->endpointDef.txDef.txQueueLen;
    queueIndex      = _usbMgrDBEpArry[endpoint].queueIdx;

    cpsr = disableInterrupts();
    noOfMsg = NUMBER_MSG_TO_SEND(endpointIndex->endpointDef.txDef.txQueueLen,interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextFreeItem,
                                interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend );
    restoreInterrupts(cpsr);


    if(noOfMsg >MAX_MSG_IN_MULTITRANSMIT)
        noOfMsg = MAX_MSG_IN_MULTITRANSMIT;

    cpsr = disableInterrupts();
    usbMgrStatus = USBMgrUtilGetFromQueue(endpoint, interfaceDB, &msg ,NEXT_MSG2SEND_PTR,queueSize);
    restoreInterrupts(cpsr);

    if(usbMgrStatus == USBMGR_RC_NO_MSG)
        return(USBMGR_RC_NO_MSG);

    multiList = (USBDevice_MultiTransmitListS *)msg;
    /*if  number of message to send is longer then rest of messages until the end of queue */
    if(interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend > interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextFreeItem)
    {
        cpsr = disableInterrupts();
        multiListFirstSize = (queueSize) - (interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend);
        usbStatus = USBDeviceEndpointMultiTransmit(endpoint, multiListFirstSize, multiList);
        noOfMsg = multiListFirstSize;
    }
    else /*number of messages are  inside queue limits */
    {
        cpsr = disableInterrupts();
        usbStatus = USBDeviceEndpointMultiTransmit(endpoint, noOfMsg, multiList);
    }
    if ( usbStatus != USB_DEVICE_RC_OK )
        usbMgrStatus = USBMGR_RC_TRANSMIT_ERROR;
    else
    {
        usbMgrStatus = USBMGR_RC_OK;
        for(txIdx = 0;txIdx<noOfMsg; txIdx++)
            INC_QUEUE_ITEM_PTR(interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend ,queueSize);
    }

    restoreInterrupts(cpsr);
    return(usbMgrStatus);

}
/************************************************************************
* Function: USBMgrSingleTransmit
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static USBMgr_RC USBMgrSingleTransmit(USBDevice_EndpointE endpoint , USBMgrDataRegDBStruct *interfaceDB)
{
    USBCDevice_ReturnCodeE        usbStatus;
    USBMgrSendMessage     *msg;
    UINT32                queueIndex,queueSize;
    USBMgr_EndpointStruct *endpointIndex;
    UINT32                cpsr;




    endpointIndex   = interfaceDB->endpoint +(_usbMgrDBEpArry[endpoint].endpointIdx);
    queueSize       = endpointIndex->endpointDef.txDef.txQueueLen;
    queueIndex = _usbMgrDBEpArry[endpoint].queueIdx;

    cpsr = disableInterrupts();
    if(USBMgrUtilGetFromQueue(endpoint, interfaceDB, &msg ,NEXT_MSG2SEND_PTR,queueSize ) == USBMGR_RC_NO_MSG )
    {
        restoreInterrupts(cpsr);
        return(USBMGR_RC_NO_MSG);
    }

    if(msg->dataPtr == NULL)
        ASSERT(0);

	usbStatus = USBDeviceEndpointTransmit((USBDevice_EndpointE)endpoint ,(UINT8 *)msg->dataPtr, msg->dataLen, msg->zlp);

	if ( usbStatus != USB_DEVICE_RC_OK )
    {
        restoreInterrupts(cpsr);
		switch(usbStatus)
        {
		case  USB_DEVICE_RC_ENDPOINT_STALLED:
			return (USBMGR_RC_ENDPOINT_STALLED);
		case USB_DEVICE_RC_NOT_CONNECTED:
			return (USBMGR_RC_DEVICE_NOT_CONNECTED);
        default:
			return(USBMGR_RC_TRANSMIT_ERROR);
		}
    }
    else
    {
        INC_QUEUE_ITEM_PTR(interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend ,queueSize);
        restoreInterrupts(cpsr);
        return(USBMGR_RC_OK);
    }
}



/************************************************************************
* Function: USBMgrRegister
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrRegister(UINT8 numEndpoint, USBMgr_EndpointStruct *endpointCfg, USBMgr_DeviceClassStruct *deviceClass,
                         CHAR *interfaceString, UINT8 appID, USBMgr_CallbackIndicationStruct *indicationCallback, USBMGR_IF_HANDLER *interfaceHnd)

{
    USBMGREndpointsReqStruct       regEndpoints[MAX_ENDPOINTS];
    USBMgr_RC                      usbMgrStatus;
    USBCDevice_ReturnCodeE                 usbStatus, usbStatus1;
    UINT8                          regIndex , descIdx ,descEndpoints[MAX_ENDPOINTS];
    UINT8                          txRegIdx ,rxRegIdx ,epIdx ,rxLisrIndex,txLisrIndex,rx_pt_ind;
    UINT8                          *pTxDataQueue;
    UINT8                          *pRxDataBuffer = NULL;
    UINT32                         rxDataLength = 0;
    USBDeviceTransactionCompletedNotifyFn        dataReceivedNotifyFn = NULL;
    USBDeviceTransactionCompletedNotifyFn   transmitCompletedNotifyFn = NULL;
    USBDeviceTransactionCompletedNotifyFn       multiTransmitNotifyFn = NULL;
	USBDeviceTransactionCompletedNotifyFn     transactionCompletedNotifyFn = NULL;
    USBMgr_EndpointStruct          *endpointIdx ,*endpoint;
    USBMgrDataRegDBStruct          *newRegDB = NULL;
    USBDevice_UsageTypeE           usageType = USB_DEVICE_USAGE_INTERRUPT;
    UINT8                          dmaChannel = 0;
    UINT8                          *pDescBuffer = NULL;
    UINT8                          allowTransmit[MAX_TX_ENDPOINTS]={0};
    UINT16                         descBufferSize = 0;
	BOOL						   expect_zlp = FALSE;



#define   DEL_ENDPOINT_REGISTRATION  {for( regIndex = 0 ; regIndex< numEndpoint;regIndex++) \
                                         _usbMgrDBEpArry[regEndpoints[regIndex].endpoint].available = TRUE;\
                                     }


//uart_printf("hal USBMgrRegister is called \r\n");

#if defined(USBMGR_ONE_APP_ONLY)
    if(_usbMgrPrm.interfaceCounter)
        return(USBMGR_RC_INTERFACE_INVALID);
#endif

    usbMgrStatus = USBMgrFindAppEndpoint(numEndpoint ,endpointCfg , regEndpoints, appID );
    if (usbMgrStatus != USBMGR_RC_OK)
        return (usbMgrStatus);

    /** Start to register information to Data Base After all USB bindings had been succeded ***/
    USBMgrUtilInsertList( &_usbMgrPrm.regDB , &newRegDB);  /* Bind the new interface to the interfaces linked list and initializes it **/
    USBMgrSetRegisterDBDefault(newRegDB);
    newRegDB->numOfEndpoints = numEndpoint;
    for (txRegIdx = 0,rxRegIdx = 0 ,regIndex = 0,endpointIdx = endpointCfg ; regIndex<numEndpoint; regIndex++,endpointIdx++)
    {
        newRegDB->endpointName[regIndex] = regEndpoints[regIndex].endpoint;
        if(regEndpoints[regIndex].endpointDir == USBMGR_DIR_IN) /*IN endpoint ,Tx */
        {         /*Create transmit queue and registers it's address in DB**/
            USBMgrUtilCreateTxQueue(endpointIdx->endpointDef.txDef, &pTxDataQueue);
            newRegDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.nextItemToRel = 0;
            newRegDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.nextItemToSend = 0;
            newRegDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.nextFreeItem = 0;
            newRegDB->endpointQueue[txRegIdx].endpointTxQueuePtrs.freeAgain = allowTransmit[txRegIdx];

            newRegDB->endpointQueue[txRegIdx].endpointTxQueueAddr = pTxDataQueue;
            newRegDB->endpointQueue[txRegIdx].txEndpoint = (UINT8)regEndpoints[regIndex].endpoint;
            txRegIdx++;
        }
        else   /* OUT endpoint */
        {
            newRegDB->endpointRxPrms[rxRegIdx].rxEndpoint = regEndpoints[regIndex].endpoint;
            newRegDB->endpointRxPrms[rxRegIdx].in_first_message_chunk   = TRUE;
            newRegDB->endpointRxPrms[rxRegIdx].list_of_rx_data_start    = NULL;
            newRegDB->endpointRxPrms[rxRegIdx].list_of_rx_data_curr     = NULL;
            newRegDB->endpointRxPrms[rxRegIdx].curr_rx_buffers_num      = 0;
            newRegDB->endpointRxPrms[rxRegIdx].rx_packets_received      = 0;
            newRegDB->endpointRxPrms[rxRegIdx].rx_packets_confirmed     = 0;
            newRegDB->endpointRxPrms[rxRegIdx].rx_chunks_received       = 0;
            rxRegIdx++;
        }
    }

    newRegDB->endpoint = (USBMgr_EndpointStruct *)( malloc( (UINT32)(numEndpoint* (sizeof(USBMgr_EndpointStruct) )) ) );
	//[klockwork][issue id: 190]
	if(newRegDB->endpoint == NULL)
	{
		ASSERT(0);
		/*Fixed coverity[missing_return]*/
		return USBMGR_RC_ENDPOINT_INVALID;

	}

	endpoint = endpointCfg;
	newRegDB->applicationID = appID;
	for (epIdx =0,endpointIdx = newRegDB->endpoint; epIdx<numEndpoint;epIdx++,endpointIdx++ ,endpoint++)
    {
        endpointIdx->type = endpoint->type;
        endpointIdx->direction = endpoint->direction;
        endpointIdx->useDMA = endpoint->useDMA;
        if(endpointIdx->direction == USBMGR_DIR_IN)
            endpointIdx->endpointDef.txDef = endpoint->endpointDef.txDef;
        else
            endpointIdx->endpointDef.rxDef = endpoint->endpointDef.rxDef;
    }
    newRegDB->callbackFunctions.controllIndFunc = indicationCallback->controllIndFunc;
    newRegDB->callbackFunctions.setupCommandIndFunc = indicationCallback->setupCommandIndFunc;
    newRegDB->callbackFunctions.TxIndFunc = indicationCallback->TxIndFunc;
    newRegDB->callbackFunctions.RxIndFunc = indicationCallback->RxIndFunc;

    newRegDB->interfaceNumber = _usbMgrPrm.interfaceCounter; /*for specific interface location in list and descriptor list */

	//usbStatus = USBDeviceRegister(statusNotifyFn, NULL, NULL);

	/*start registration to USB Services */
    rx_pt_ind=0;
    for ( regIndex = 0 ,endpointIdx = endpointCfg; regIndex<numEndpoint; regIndex++ ,endpointIdx++ )
    {
        if(regEndpoints[regIndex].endpointDir == USBMGR_DIR_IN) /*IN endpoint */
        {
            if( indicationCallback->TxIndFunc != NULL )
            {
                if( endpointIdx->endpointDef.txDef.multiTransmitEnabled == TRUE )/* move to next endpoint */
				{
                    multiTransmitNotifyFn = USBMgrUSBMultiTxCompletedNotify;
					transmitCompletedNotifyFn = dataReceivedNotifyFn = NULL;
				}
                else
                {
					multiTransmitNotifyFn = dataReceivedNotifyFn = NULL;
                	transmitCompletedNotifyFn = USBMgrUSBTxCompletedNotify;
				}
            }
            else
            {
                DEL_ENDPOINT_REGISTRATION;
                USBMgrUtilDelList(_usbMgrPrm.regDB,newRegDB ); /* delete the specify  interface list */
				return(USBMGR_RC_REGISTER_ERROR);
            }
            if(endpointIdx->endpointDef.txDef.txQueueLen<5)
                allowTransmit[regIndex] = 1;
            else
                allowTransmit[regIndex] = (endpointIdx->endpointDef.txDef.txQueueLen)/5;
            dataReceivedNotifyFn = NULL;
        }
        else       /*OUT endpoint*/
        {
            if( indicationCallback->RxIndFunc != NULL )
                dataReceivedNotifyFn = USBMgrUSBRxNotify;
            else
            {
                DEL_ENDPOINT_REGISTRATION;
                USBMgrUtilDelList(_usbMgrPrm.regDB,newRegDB ); /* delete the specify  interface list */
				return(USBMGR_RC_REGISTER_ERROR);
            }
            transmitCompletedNotifyFn = multiTransmitNotifyFn = NULL;
        }  /* of out put endpoint */


        if(endpointIdx->useDMA)
        {
           // uart_printf("usb DMA is %d, never should come here\r\n",endpointIdx->useDMA );
        }                                   /* does not use DMA */
        else
        {
            usageType = USB_DEVICE_USAGE_INTERRUPT;
            dmaChannel = 0;
            pDescBuffer = NULL;
            descBufferSize = 0;
        }

        if(regEndpoints[regIndex].endpointDir == USBMGR_DIR_OUT) /*OUT endpoint */
        {
            rxDataLength =  endpointIdx->endpointDef.rxDef.rxBuffQuantumLen;
            if(endpointIdx->endpointDef.rxDef.optionalRxBufferPtr != NULL) /* If application allocated memory for receiveing data **/
			{
				pRxDataBuffer = endpointIdx->endpointDef.rxDef.optionalRxBufferPtr;

			}
			else
                 pRxDataBuffer = malloc(endpointIdx->endpointDef.rxDef.rxBuffQuantumLen);

			expect_zlp = endpointIdx->endpointDef.rxDef.expect_zlp;
            _usbMgrRxDataBuffer = pRxDataBuffer;

            rx_pt_ind++;
        }

		/* is it only one that is not NULL? */
		if (multiTransmitNotifyFn != NULL)
			transactionCompletedNotifyFn = multiTransmitNotifyFn;
		else if (transmitCompletedNotifyFn != NULL)
		transactionCompletedNotifyFn = transmitCompletedNotifyFn;
		else if (dataReceivedNotifyFn != NULL)
		transactionCompletedNotifyFn = dataReceivedNotifyFn;

		usbStatus = USBDeviceEndpointOpen(regEndpoints[regIndex].endpoint, usageType, dmaChannel, pDescBuffer, descBufferSize, transactionCompletedNotifyFn);

		if(regEndpoints[regIndex].endpointDir == USBMGR_DIR_OUT) /*OUT endpoint */
		{
			usbStatus1 = USBDeviceEndpointReceive(regEndpoints[regIndex].endpoint, pRxDataBuffer, rxDataLength,expect_zlp);
		}
		else
			usbStatus1 = USB_DEVICE_RC_OK;

		if ((usbStatus != USB_DEVICE_RC_OK) || (usbStatus1 != USB_DEVICE_RC_OK))
        {
            DEL_ENDPOINT_REGISTRATION;
            free((void*)pDescBuffer);
            USBMgrUtilDelList(_usbMgrPrm.regDB,newRegDB ); /* delete the specify  interface list */
			if( (pRxDataBuffer != NULL) && (endpointIdx->endpointDef.rxDef.optionalRxBufferPtr == NULL) )
                free((void*)pRxDataBuffer);
            return(USBMGR_RC_REGISTER_ERROR);
        }



    } /** of for loop */

    for( descIdx = 0 ; descIdx < numEndpoint ; descIdx++ )
        descEndpoints[descIdx] = (UINT8)regEndpoints[descIdx].endpoint;
    USBMgrSetDescriptors( numEndpoint ,appID , deviceClass ,endpointCfg ,descEndpoints );

    _usbMgrPrm.interfaceCounter++;

    *interfaceHnd =(USBMGR_IF_HANDLER *)newRegDB;


    /**Update Fast table for Tx/Rx usage ****************/
    for (epIdx =0 ; epIdx<numEndpoint;epIdx++)
    {
        _usbMgrDBEpArry[newRegDB->endpointName[epIdx]].interfaceHnd = (UINT32)newRegDB;
        _usbMgrDBEpArry[newRegDB->endpointName[epIdx]].endpointIdx = epIdx;
        if(regEndpoints[epIdx].endpointDir == USBMGR_DIR_IN)
        {
            FIND_QUEUE_INDEX(newRegDB , newRegDB->endpointName[epIdx] ,txRegIdx );
            _usbMgrDBEpArry[newRegDB->endpointName[epIdx]].queueIdx = txRegIdx;
            FIND_TX_LISR_PRMS_INDEX( newRegDB->endpointName[epIdx] ,txLisrIndex);
            _usbMgrDBEpArry[newRegDB->endpointName[epIdx]].txLisrIdx = txLisrIndex;
        }
        else
        {
            FIND_OUT_ENDPOINT_INDEX(newRegDB ,newRegDB->endpointName[epIdx] ,rxRegIdx);
            _usbMgrDBEpArry[newRegDB->endpointName[epIdx]].rxBufferIdx = rxRegIdx;
            FIND_RX_LISR_PRMS_INDEX( newRegDB->endpointName[epIdx] ,rxLisrIndex)
            _usbMgrDBEpArry[newRegDB->endpointName[epIdx]].rxLisrIdx = rxLisrIndex;
        }

    }
	_enableHISRs = TRUE;
    return (USBMGR_RC_OK);
}

/************************************************************************
* Function: USBMgrUnregister
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrUnregister( USBMGR_IF_HANDLER interfaceHnd)
{
    USBMgrDataRegDBStruct   *reqRegDB;
    USBMgr_EndpointStruct   *endpointIndex;
    USBCDevice_ReturnCodeE          usbStatus;
    UINT8                   regIndex ,epIdx; //,rxEndpointIdx;



    reqRegDB        = (USBMgrDataRegDBStruct*)interfaceHnd;

    /*Delete the specific interface descriptors and related information in device descriptor */


    USBMgrUpdateDeviceDescriptor(_usbMgrDevDescriptor , reqRegDB->applicationID ,FALSE);
    USBSetDescriptor( (USB_DescriptorTypesE)_DEVICE_DESCRIPTOR ,_usbMgrDevDescriptor , sizeof(USBMgrDevDescStruct), 0);

    _usbMgrCfgDescriptor = (UINT8 *)USBMgrDelCfgDescriptor( _usbMgrCfgDescriptor,reqRegDB->interfaceNumber , reqRegDB->numOfEndpoints );
    USBSetDescriptor( (USB_DescriptorTypesE)_CONFIG_DESCRIPTOR ,_usbMgrCfgDescriptor , _usbMgrPrm.totalcfgDescSize, 0);


    /*close USB Endpoint */

    for ( regIndex = 0; regIndex<reqRegDB->numOfEndpoints; regIndex++ )
    {
        usbStatus = USBDeviceEndpointClose((USBDevice_EndpointE)reqRegDB->endpointName[regIndex]);
        if(usbStatus != USB_DEVICE_RC_OK)
            return(USBMGR_RC_REGISTER_ERROR);
    }

    for ( regIndex = 0 ,endpointIndex = reqRegDB->endpoint+regIndex; regIndex< MAX_TX_ENDPOINTS; regIndex++ )
    {
        if( (reqRegDB->endpointQueue[regIndex].txEndpoint != 0 )&& (endpointIndex->endpointDef.txDef.optionalTxQueuePtr == NULL ) )
            USBMgrUtilDeleteTxQueue(reqRegDB->endpointQueue[regIndex].endpointTxQueueAddr);
    }


    if(_usbMgrRxDataBuffer != NULL)       /*This is the case that application did not allocate  buffer  */
        free((void*)_usbMgrRxDataBuffer);

    /*Release DMA Descriptors */

    free((void*)_usbMgrDmaDescriptor);

    _usbMgrPrm.interfaceCounter--;

    for (epIdx =0 ; epIdx<reqRegDB->numOfEndpoints; epIdx++)
    {
        _usbMgrDBEpArry[reqRegDB->endpointName[epIdx]].interfaceHnd  = 0;
        _usbMgrDBEpArry[reqRegDB->endpointName[epIdx]].endpointIdx   = 0;
        _usbMgrDBEpArry[reqRegDB->endpointName[epIdx]].queueIdx      = 0;
        _usbMgrDBEpArry[reqRegDB->endpointName[epIdx]].rxBufferIdx   = 0;
        _usbMgrDBEpArry[reqRegDB->endpointName[epIdx]].txLisrIdx     = 0;
        _usbMgrDBEpArry[reqRegDB->endpointName[epIdx]].rxLisrIdx     = 0;

    }

    USBMgrUtilDelList(_usbMgrPrm.regDB,reqRegDB ); /* delete the specify  interface list */

    return(USBMGR_RC_OK);
}



/************************************************************************
* Function: USBMgrFlushTxQueue
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrFlushTxQueue( USBMGR_IF_HANDLER interfaceHnd ,UINT8 endpoint/*index at User table-indicate the relative location of specific endpoint*/)
{
    USBMgrDataRegDBStruct  *interfaceDB;
    UINT8                   queueIndex;
    USBMgr_EndpointStruct  *endpointIndex;
    UINT32                  queueSize;

    interfaceDB     = (USBMgrDataRegDBStruct *)interfaceHnd;
    queueIndex      = _usbMgrDBEpArry[interfaceDB->endpointName[endpoint]].queueIdx;
    endpointIndex   = interfaceDB->endpoint +endpoint;
    queueSize       = endpointIndex->endpointDef.txDef.txQueueLen;


    interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToRel    = 0;
    interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend   = 0;
    interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextFreeItem     = 0;
    interfaceDB->endpointQueue[queueIndex].txEndpointBusy                       = FALSE;
    USBDeviceEndpointAbort(interfaceDB->endpointName[endpoint]);
}
/************************************************************************
* Function: USBMgrFlushRxBuffer
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrFlushRxBuffer ( USBMGR_IF_HANDLER interfaceHnd ,UINT8 endpoint/*index at User table*/)
{
    UINT8                 rxEndpointIdx;
    USBMgrDataRegDBStruct *interfaceDB;

    interfaceDB = (USBMgrDataRegDBStruct *)interfaceHnd;
    rxEndpointIdx = _usbMgrDBEpArry[interfaceDB->endpointName[endpoint]].rxLisrIdx;

    _usbMgrLisrParams.rxPrms[rxEndpointIdx].recPtr            = NULL;
    _usbMgrLisrParams.rxPrms[rxEndpointIdx].recLen            = 0;
    _usbMgrLisrParams.rxPrms[rxEndpointIdx].endOfRecMsg       = TRUE;
    interfaceDB->endpointRxPrms[rxEndpointIdx].list_of_rx_data_start   = NULL;

    USBDeviceEndpointAbort(interfaceDB->endpointName[endpoint]);
}


/************************************************************************
* Function: USBMgrTransmit
*************************************************************************
* Description:
*
* Parameters:
*
*
*
* Return value: void
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrTransmit(USBMGR_IF_HANDLER interfaceHnd,UINT8 endpoint/*index at User table*/, UINT8 *dataBuffer , UINT32 dataLen,BOOL endOfMessage/*,UINT32 cooky*/ )
{
    USBMgrDataRegDBStruct     *interfaceDB;
    UINT8                     queueIndex;
    UINT32                    queueSize;
    USBMgr_EndpointStruct     *endpointIndex;
    USBMgrSendMessage         msgToSend;
    USBMgr_RC                 usbMgrStatus = USBMGR_RC_OK;
    UINT32                    cpsr;


    if(_usbMgrPrm.usbCableOut)
        return(USBMGR_RC_TRANSMIT_ERROR);


    /**check if transmit endpoint is valid ***/
    /*check endpoint ownership */
    interfaceDB     = (USBMgrDataRegDBStruct*)interfaceHnd;
    endpointIndex   = interfaceDB->endpoint + endpoint;
    queueSize       = endpointIndex->endpointDef.txDef.txQueueLen;
    queueIndex      = _usbMgrDBEpArry[interfaceDB->endpointName[endpoint]].queueIdx;

    if(interfaceDB->endpointQueue[queueIndex].fullQueue == TRUE)
        return(USBMGR_RC_TX_BUFFER_FULL);

    msgToSend.dataPtr = dataBuffer;
    msgToSend.dataLen = dataLen;
	msgToSend.zlp	  = endOfMessage;

    cpsr = disableInterrupts();
    usbMgrStatus = USBMgrUtilInsertToQueue(interfaceDB->endpointName[endpoint], interfaceDB, &msgToSend ,queueSize );
    //avoid busy race restoreInterrupts(cpsr);

    if( usbMgrStatus != USBMGR_RC_OK )
    {
        interfaceDB->endpointQueue[queueIndex].fullQueue = TRUE;
    }

    if(interfaceDB->endpointQueue[queueIndex].txEndpointBusy == FALSE )   /** USB endpoint is able to transmit now **/
    {
        interfaceDB->endpointQueue[queueIndex].txEndpointBusy = TRUE;
		//restoreInterrupts(cpsr); -- no point in restoring here USBMgrSingleTransmit will disable anyway
        //getting from queue should be under disable otherwise possibly getting USBMGR_RC_NO_MSG assert

        // avoid busy race cpsr = disableInterrupts();

        if((endpointIndex->endpointDef.txDef.multiTransmitEnabled) &&
           (NUMBER_MSG_TO_SEND(endpointIndex->endpointDef.txDef.txQueueLen,interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextFreeItem,
                                interfaceDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend ) > MIN_MSG_IN_MULTITRANSMIT))
        {
            //restoreInterrupts(cpsr);
            usbMgrStatus = USBMgrMultiTransmit(interfaceDB->endpointName[endpoint] ,interfaceDB);
        }
        else
        {/*not multi transmit or not enought messages for multi*/
            //restoreInterrupts(cpsr);
            usbMgrStatus = USBMgrSingleTransmit(interfaceDB->endpointName[endpoint] ,interfaceDB);
        }

        ASSERT(usbMgrStatus != USBMGR_RC_NO_MSG);
        if (usbMgrStatus != USBMGR_RC_OK )
            interfaceDB->endpointQueue[queueIndex].txEndpointBusy = FALSE;
    }

	restoreInterrupts(cpsr);
    return(usbMgrStatus);

}


/************************************************************************
* Function: USBMgrEndpointStall
*************************************************************************
* Description:
*
* Parameters:
*
*
*
* Return value: void
*
* Notes: stall endpoint, can only be called from task, blocked, takes ~2milli to enter stall
************************************************************************/
USBMgr_RC   USBMgrEndpointStall(USBMGR_IF_HANDLER interfaceHnd,UINT8 endpoint)
{
    USBMgrDataRegDBStruct     *interfaceDB;
    interfaceDB     = (USBMgrDataRegDBStruct*)interfaceHnd;

    if(USBDeviceEndpointStall(interfaceDB->endpointName[endpoint]) == USB_DEVICE_RC_OK)
        return(USBMGR_RC_OK);
    else
        return(USBMGR_RC_GENERAL_ERROR);
}


/************************************************************************
* Function:  HandleRxConfirm
*************************************************************************
* Description: USBMgr internal actions in response to rx confirm from application
*
* Parameters:
*
* Return value:
*
* Notes:
**********************************************************************/
static void   HandleRxConfirm(USBDevice_EndpointE endpoint)
{
    UINT8 rxEndpointIndex   = _usbMgrDBEpArry[endpoint].rxBufferIdx;
    USBMgrDataRegDBStruct  *reqDB = (USBMgrDataRegDBStruct *)_usbMgrDBEpArry[endpoint].interfaceHnd;

    if(reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff)
    {
        ShmemFree(reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff);
        reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff = NULL;
    }

    reqDB->endpointRxPrms[rxEndpointIndex].rx_packets_confirmed++;
}

/************************************************************************
* Function:  USBMgrRecConfirmation
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes: every application must call rx confirmation
**********************************************************************/
void    USBMgrRxConfirmation(USBDevice_EndpointE endpoint)
{
    HandleRxConfirm( endpoint);

    USBDeviceEndpointReceiveCompleted(endpoint); //notify driver that it can send data
}

/************************************************************************
* Function:  USBMgrRxConfirmationExt
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  every application must call rx confirmation
**********************************************************************/
void    USBMgrRxConfirmationExt(USBDevice_EndpointE endpoint ,UINT32 next_rx_packet_length, BOOL expect_zlp)
{


	USBMgrDataRegDBStruct  *reqDB = (USBMgrDataRegDBStruct *)_usbMgrDBEpArry[endpoint].interfaceHnd;
    UINT8 *next_time_buff = reqDB->endpoint->endpointDef.rxDef.optionalRxBufferPtr;
    UINT8 rxEndpointIndex   = _usbMgrDBEpArry[endpoint].rxBufferIdx;

	HandleRxConfirm( endpoint);

    //case when next packet is bigger than user provided buffer
    if(reqDB->endpoint->endpointDef.rxDef.rxBuffQuantumLen < next_rx_packet_length)
    {
        //if buffer was allocated before && its size is bigger than next_packet do nothing, otherwise,reallocate
        if(reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer_length < next_rx_packet_length )
        {
            reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer_length  =  next_rx_packet_length;
            if(reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer)
                ShmemFree(reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer);
            reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer = ShmemAlloc(next_rx_packet_length);
        }

        next_time_buff = reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer;
    }
    else
    {
        //check if need to free anything , if original buffer is bigger than next packet , free previously allocated buffer
        if(reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer_length && reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer)
        {
            reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer_length  = 0;
            ShmemFree(reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer);
            reqDB->endpointRxPrms[rxEndpointIndex].rx_ext_buffer = 0;
        }
    }

    USBDeviceEndpointReceiveCompletedExt(endpoint, next_time_buff, next_rx_packet_length,  expect_zlp); //notify driver that it can send data

}

#define DO_FOREVER while(1)

/************************************************************************
* Function:  USBMgrRxTask
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void usbMgrRxTask (UINT32 v, void *argv)
{

    UINT32                 rxMsgFlag;
    //UINT32   			   cpsr;
	UINT8                  rxEndpointIndex ,rxEndpointIdx ,rxEndpoint;
    UINT8                   *dataRecPtr;
    UINT32                 dataRecLen ,lengthOffset;
    BOOL                   endOfMsg;
    USBMgrDataRegDBStruct  *reqDB;
    USBMgr_EndpointStruct   *endpointIdx;
    USBMgrRxDataList       *rx_list_ptr_temp,*rx_list_ptr_temp_to_free;
    UINT8                  ch_ind=0;

	OS_STATUS			   osStatus;


    DO_FOREVER
    {
		osStatus = OSAFlagWait(usbMgrRxEventRef, usbMgrRxEventsMaskUpdated, OSA_FLAG_OR_CLEAR, &rxMsgFlag, OSA_SUSPEND);
		ASSERT(osStatus == OS_SUCCESS);


        while(rxMsgFlag)
        {
            USBMgrRetrieveOUTEndpoint( rxMsgFlag ,&rxEndpoint );

            //clean the bit
            rxMsgFlag &= (~(1<<rxEndpoint));

            rxEndpointIdx = _usbMgrDBEpArry[rxEndpoint].rxLisrIdx;
            if(_usbMgrLisrParams.rxPrms[rxEndpointIdx].rxEndpoint != rxEndpoint)
		    {
		    	/*Fix coverity[uninit_use_in_call]*/
			    //restoreInterrupts(cpsr);
			    ASSERT(0);
		    }
            dataRecPtr = _usbMgrLisrParams.rxPrms[rxEndpointIdx].recPtr;
            dataRecLen = _usbMgrLisrParams.rxPrms[rxEndpointIdx].recLen;
            endOfMsg   = _usbMgrLisrParams.rxPrms[rxEndpointIdx].endOfRecMsg;
		    reqDB             = (USBMgrDataRegDBStruct *)_usbMgrDBEpArry[rxEndpoint].interfaceHnd;
            endpointIdx       = reqDB->endpoint +(_usbMgrDBEpArry[rxEndpoint].endpointIdx);
            rxEndpointIndex   = _usbMgrDBEpArry[rxEndpoint].rxBufferIdx;

            reqDB->endpointRxPrms[rxEndpointIndex].rx_chunks_received++;
            if(endOfMsg)/*end of receiving message */
            {
                reqDB->endpointRxPrms[rxEndpointIndex].rx_packets_received++;
                if(reqDB->endpointRxPrms[rxEndpointIndex].in_first_message_chunk)  /* do not have to concatenate. This is a new and single piece of data   */
                {
                    reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff  =  NULL; //no additional buffer was allocated
                    reqDB->callbackFunctions.RxIndFunc(rxEndpoint,dataRecPtr,dataRecLen);
                }
                else   /*end of concatenated message**/
                {
                    reqDB->endpointRxPrms[rxEndpointIndex].in_first_message_chunk = TRUE; //for next time
                    reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->rxDataPtr = malloc(dataRecLen);
                    reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->rxDataLen = dataRecLen;
                    reqDB->endpointRxPrms[rxEndpointIndex].rx_total_data_length+=dataRecLen;
                    memcpy(reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->rxDataPtr ,  dataRecPtr , dataRecLen);
                    reqDB->endpointRxPrms[rxEndpointIndex].curr_rx_buffers_num++;
                    reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->p_next = 0; //last

                    rx_list_ptr_temp = reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_start;

                    /* now allocates  large amount of memory in order to copy all pieces of data */
                    reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff = ShmemAlloc(reqDB->endpointRxPrms[rxEndpointIndex].rx_total_data_length);
                    lengthOffset=0;
                    for(ch_ind=0; ch_ind<reqDB->endpointRxPrms[rxEndpointIndex].curr_rx_buffers_num;ch_ind++)
                    {

                        memcpy( reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff + lengthOffset, rx_list_ptr_temp->rxDataPtr, rx_list_ptr_temp->rxDataLen);
                        free((void*)rx_list_ptr_temp->rxDataPtr);
                        rx_list_ptr_temp->rxDataPtr = NULL;

                        lengthOffset += rx_list_ptr_temp->rxDataLen;  //
                        rx_list_ptr_temp_to_free =  rx_list_ptr_temp; //will be freed
                        rx_list_ptr_temp = rx_list_ptr_temp->p_next;
						/*Fixed coverity[deref_after_free]*/
						#if 0
                        free(rx_list_ptr_temp);
						#else
						free(rx_list_ptr_temp_to_free);
						#endif
                    }

                    reqDB->callbackFunctions.RxIndFunc(rxEndpoint,reqDB->endpointRxPrms[rxEndpointIndex].rx_complex_buff,reqDB->endpointRxPrms[rxEndpointIndex].rx_total_data_length);
                }                                 /* of end of concatenated message */
            }
            else                                  /* a part of whole message  */
            {
                if(reqDB->endpointRxPrms[rxEndpointIndex].in_first_message_chunk)  /* new buffer that must concatenate */
                {
                    reqDB->endpointRxPrms[rxEndpointIndex].in_first_message_chunk = FALSE;     /* this is the first piece of data so must reset the flag  */
                    reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_start  = malloc(sizeof(USBMgrRxDataList));
                    reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr = reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_start;
                    reqDB->endpointRxPrms[rxEndpointIndex].curr_rx_buffers_num =0;
                    reqDB->endpointRxPrms[rxEndpointIndex].rx_total_data_length = 0;
                }

                reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->rxDataPtr = malloc(dataRecLen);
                memcpy(reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->rxDataPtr ,  dataRecPtr , dataRecLen);

                reqDB->endpointRxPrms[rxEndpointIndex].rx_total_data_length+=dataRecLen;
                reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->rxDataLen = dataRecLen;
                reqDB->endpointRxPrms[rxEndpointIndex].curr_rx_buffers_num++;


                reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->p_next = malloc(sizeof(USBMgrRxDataList));
                reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr = reqDB->endpointRxPrms[rxEndpointIndex].list_of_rx_data_curr->p_next;
                USBDeviceEndpointReceiveCompleted((USBDevice_EndpointE)rxEndpoint); //notify driver to bring next portion of data
            }                                   /* of part  of whole message */

            /* must concatenate    */
        }

    } /*of DO _FOREVEER */
}

/************************************************************************
* Function:  usbMgrRxIndicationHisr
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
**********************************************************************/
void usbMgrRxIndicationHisr(void)
{
	OS_STATUS	osStatus;    

    UINT32   cpsr;
    volatile UINT32   ep_mask;

	if (_enableHISRs == FALSE) return;
    cpsr = disableInterrupts();

    ep_mask = _usbMgrPrm.lisrRxEndpointsMask;
    _usbMgrPrm.lisrRxEndpointsMask = 0;
    restoreInterrupts(cpsr);

    if(ep_mask)
    {
    	osStatus = OSAFlagSet(usbMgrRxEventRef, ep_mask, OSA_FLAG_OR);
		ASSERT(osStatus == OS_SUCCESS);
	
    }

}
/************************************************************************
* Function:  usbMgrFindEndpointNumber
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  find endpoint number that was marked as 1
**********************************************************************/
UINT8 usbMgrFindMarkedEndpointNumber(UINT8 *ep_num_arr)
{
    UINT8 ind=0;
    for(ind=0;ind<MAX_ENDPOINTS;ind++)
    {
        if(ep_num_arr[ind])
            return(ind);
    }

    return(0xff);//not found
}
/************************************************************************
* Function:  usbMgrTxIndicationHisr
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void usbMgrTxIndicationHisr(void)
{
    UINT8                  queueIndex ,indIdx ,txLisrIdx;
    UINT8                  txEndpoint;
    USBMgrDataRegDBStruct  *reqDB;
    USBMgr_EndpointStruct  *endpointIdx;
    USBMgrSendMessage      *msgToFree;
    USBMgrSendMessage      emptyMsg;
    UINT32                 cpsr , noOfTxBuffers ,queueSize;                    /* if not multi transmit always zero */
    BOOL                   transmitAll;                      /* if not multi transmit always FALSE */
    USBMgr_RC              usbMgrStatus;

	if (_enableHISRs == FALSE) return;

    //find enpoint number that HISR was received for
    txEndpoint = usbMgrFindMarkedEndpointNumber(tx_completed_endpoints_num);
    ASSERT(txEndpoint != 0xFF);
    tx_completed_endpoints_num[txEndpoint]=0; //clean it for the next time

    reqDB       = (USBMgrDataRegDBStruct *)_usbMgrDBEpArry[txEndpoint].interfaceHnd;
    endpointIdx = reqDB->endpoint +(_usbMgrDBEpArry[txEndpoint].endpointIdx);
    queueSize   = endpointIdx->endpointDef.txDef.txQueueLen;
    queueIndex  = _usbMgrDBEpArry[txEndpoint].queueIdx;
    txLisrIdx   = _usbMgrDBEpArry[txEndpoint].txLisrIdx;
    if(txEndpoint != _usbMgrLisrParams.txPrms[txLisrIdx].txEndpoint)
        ASSERT(0);

    cpsr = disableInterrupts();
    if(FREE_QUEUE(reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs ,queueSize) && (reqDB->endpointQueue[queueIndex].fullQueue) )
        reqDB->endpointQueue[queueIndex].fullQueue = FALSE;
    restoreInterrupts(cpsr);


    emptyMsg.dataPtr = NULL;
    emptyMsg.dataLen = 0;
	/*Fix coverity[uninit_use_in_call]*/
	 emptyMsg.zlp = FALSE;

    if(_usbMgrLisrParams.txPrms[txLisrIdx].noOfTxBuffers == 0) /* not multi transmit */
    {
        cpsr = disableInterrupts();
        USBMgrUtilGetFromQueue(txEndpoint, reqDB, &msgToFree ,NEXT_MSG2FREE_PTR,queueSize );
        restoreInterrupts(cpsr);

        reqDB->callbackFunctions.TxIndFunc(endpointIdx->endpointDef.txDef.cooky,txEndpoint,msgToFree->dataPtr,msgToFree->dataLen );

        cpsr = disableInterrupts();
		memcpy ((NEXT_ITEM_PTR_ON_QUEUE(reqDB->endpointQueue[queueIndex].endpointTxQueueAddr,reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToRel)) ,
               (UINT8*)&emptyMsg, sizeof(USBMgrSendMessage));
        INC_QUEUE_ITEM_PTR(reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToRel,queueSize );
        restoreInterrupts(cpsr);


        if(_usbMgrPrm.usbCableOut == FALSE)
        {

        /*Transmit the next messages in queue if exist */
            cpsr = disableInterrupts();

            if(!EMPTY_QUEUE(reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs) )
            {


                if(NUMBER_MSG_TO_SEND(endpointIdx->endpointDef.txDef.txQueueLen,reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextFreeItem,
                         reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend ) > MIN_MSG_IN_MULTITRANSMIT)
                {
					restoreInterrupts(cpsr);
					usbMgrStatus = USBMgrMultiTransmit((USBDevice_EndpointE)txEndpoint ,reqDB);

                }
                else
                {
					restoreInterrupts(cpsr);
					usbMgrStatus = USBMgrSingleTransmit((USBDevice_EndpointE)txEndpoint ,reqDB);

				}
                if(usbMgrStatus != USBMGR_RC_OK )
                {
                    reqDB->endpointQueue[queueIndex].txEndpointBusy = FALSE;
            /*        if(usbMgrStatus != USBMGR_RC_NO_MSG)
                       ASSERT(0);*/
                }

            } /*of empty Q*/
            else  /* empty Q*/
            {
                reqDB->endpointQueue[queueIndex].txEndpointBusy = FALSE;
                restoreInterrupts(cpsr);
            }

        } /*of cable out */
    }
    else    /**multi transmit indication **/
    {

        noOfTxBuffers = _usbMgrLisrParams.txPrms[txLisrIdx].noOfTxBuffers;
        transmitAll = _usbMgrLisrParams.txPrms[txLisrIdx].transmitAll;
        for(indIdx = 0; indIdx<noOfTxBuffers;indIdx++ )
        {
            cpsr = disableInterrupts();
            USBMgrUtilGetFromQueue(txEndpoint, reqDB, &msgToFree ,NEXT_MSG2FREE_PTR ,queueSize);
            restoreInterrupts(cpsr);
            reqDB->callbackFunctions.TxIndFunc(endpointIdx->endpointDef.txDef.cooky,txEndpoint,msgToFree->dataPtr,msgToFree->dataLen );

            cpsr = disableInterrupts();
			memcpy ((NEXT_ITEM_PTR_ON_QUEUE(reqDB->endpointQueue[queueIndex].endpointTxQueueAddr,reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToRel)) ,
               (UINT8*)&emptyMsg, sizeof(USBMgrSendMessage));
            INC_QUEUE_ITEM_PTR(reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToRel ,queueSize);
            restoreInterrupts(cpsr);


        }
        if(transmitAll)
        {
            if(_usbMgrPrm.usbCableOut == FALSE)
            {

            /*Transmit the next messages in queue if exist */
                cpsr = disableInterrupts();
                if(!EMPTY_QUEUE(reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs) )
                {
                    if(NUMBER_MSG_TO_SEND(endpointIdx->endpointDef.txDef.txQueueLen,reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextFreeItem,
                         reqDB->endpointQueue[queueIndex].endpointTxQueuePtrs.nextItemToSend ) > MIN_MSG_IN_MULTITRANSMIT)
                    {
                        restoreInterrupts(cpsr);
                        usbMgrStatus = USBMgrMultiTransmit((USBDevice_EndpointE)txEndpoint ,reqDB);
                    }
                    else
                    {
                        restoreInterrupts(cpsr);
                        usbMgrStatus = USBMgrSingleTransmit((USBDevice_EndpointE)txEndpoint ,reqDB);
                    }
                    if(usbMgrStatus != USBMGR_RC_OK )
                    {
                        reqDB->endpointQueue[queueIndex].txEndpointBusy = FALSE;
                        ASSERT(0);
                    }
                }/*of Empty Q */
                else  /* empty Q*/
                {
                    reqDB->endpointQueue[queueIndex].txEndpointBusy = FALSE;
                    restoreInterrupts(cpsr);
                }
            }/*of !_usbMgrUsbCableOut */
        } /*of transmit all */
    }                                     /* of multi transmit indication */


}

/************************************************************************
* Function:  usbMgrControlIndicationHisr
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void usbMgrControlIndicationHisr(void)
{
    UINT8                  epIdx ,index;
    USBMgrDataRegDBStruct  *interfaceIdx;
    USBMgr_Status           usbMgrStatus = USBMGR_STATUS_NONE;
    USBMgr_Error           usbMgrError = USBMGR_ERROR_NONE;
    UINT32                 cpsr;

	if (_enableHISRs == FALSE) return;
    cpsr = disableInterrupts();

    usbMgrStatus                             = _usbMgrLisrParams.ctrlPrms.usbMgrStatus;
    usbMgrError                              = _usbMgrLisrParams.ctrlPrms.usbMgrError;
    _usbMgrLisrParams.ctrlPrms.usbMgrStatus  = USBMGR_STATUS_NONE;
    _usbMgrLisrParams.ctrlPrms.usbMgrError   = USBMGR_ERROR_NONE;

    restoreInterrupts(cpsr);

    if( (usbMgrStatus == USBMGR_STATUS_NONE) && (usbMgrError == USBMGR_ERROR_NONE) )
        return;       /* no change !!*/



    if(usbMgrStatus == USBMGR_RESET_DETECT)
    {
        index = 0;
        interfaceIdx = _usbMgrPrm.regDB;
        do
        {
            if(index > 0)
                interfaceIdx = interfaceIdx->nextInterface;
            for(epIdx = 0; epIdx<interfaceIdx->numOfEndpoints;epIdx++)
            {
                USBMgrFlushTxQueue((USBMGR_IF_HANDLER)interfaceIdx,interfaceIdx->endpointName[epIdx]);
                USBMgrFlushRxBuffer((USBMGR_IF_HANDLER)interfaceIdx ,interfaceIdx->endpointName[epIdx]);
            }
            index++;

        } while(interfaceIdx->nextInterface != NULL );

    }

    index = 0;
    interfaceIdx = _usbMgrPrm.regDB;
	if (interfaceIdx)  // might be NULL - we got interrupt before registering
	{
    	do
    	{
        	if(index > 0)
            	interfaceIdx = interfaceIdx->nextInterface;
        	interfaceIdx->callbackFunctions.controllIndFunc(/*UINT32 - cooky */usbMgrStatus,usbMgrError,0);
        	index++;

    	}while(interfaceIdx->nextInterface != NULL );
	}


}

/************************************************************************
* Function:  USBMgrRecConfirmation
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
**********************************************************************/
void USBMgrSetupCommandRsp(UINT8 *setupPacket ,UINT16 setupLen )
{
	USBDeviceVendorClassResponse(setupPacket, setupLen);

    ep0_setup_transfer_started = FALSE;

}
/************************************************************************
* Function:usbMgrSetupReqHisr
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  from LISR context
**********************************************************************/
void usbMgrSetupReqHisr(void)
{
	 USBMgrDataRegDBStruct  *interfaceIdx;
	USBMgr_SetupDataStruct  setupPrms;
	UINT32 cpsr;

	/***Temporary!!!!****/
    if (_enableHISRs == FALSE) return;

    cpsr = disableInterrupts(); //can not receive another message while copying

    setupPrms.hostToDeviceDataPtr =  0;

    memcpy(setupPrms.setup_packet,_usbMgrLisrParams.setUpPrms.setupData,USB_SETUP_REQUEST_SIZE);
    setupPrms.dataStagelength = *((UINT16*)(_usbMgrLisrParams.setUpPrms.setupData+6)); /*apply to host-to-device and device-to-host*/


    if (*(_usbMgrLisrParams.setUpPrms.setupData )& 0x01 )
		setupPrms.recipient = USBMGR_INTERFACE; //always interface
    else
    	ASSERT(0);

    setupPrms.recipient = USBMGR_INTERFACE;
	if(_usbMgrLisrParams.setUpPrms.setupType == USBMGR_VENDOR)
	{

		ASSERT(*(_usbMgrLisrParams.setUpPrms.setupData )& 0x40);
		setupPrms.type = USBMGR_VENDOR;
	}

	else
	{
		ASSERT(*(_usbMgrLisrParams.setUpPrms.setupData )& 0x20);
		setupPrms.type = USBMGR_CLASS;
	}


	if (*(_usbMgrLisrParams.setUpPrms.setupData )& 0x80 )
		setupPrms.direction = USBMGR_DEVICE_TO_HOST;
	else
		setupPrms.direction = USBMGR_HOST_TO_DEVICE;


	setupPrms.index = *((UINT16*)(_usbMgrLisrParams.setUpPrms.setupData+4));    //This is the interface number


    if(setupPrms.dataStagelength && setupPrms.direction == USBMGR_HOST_TO_DEVICE)
    {
        setupPrms.hostToDeviceDataPtr = malloc(setupPrms.dataStagelength);

		//[klockwork][issue id: 181]
		if(setupPrms.hostToDeviceDataPtr == NULL)
		{
			ASSERT(0);
			return ;
		}
        memcpy(setupPrms.hostToDeviceDataPtr,_usbMgrLisrParams.setUpPrms.p_data,setupPrms.dataStagelength);
    }


    restoreInterrupts(cpsr);


    interfaceIdx = _usbMgrPrm.regDB;
    while(interfaceIdx)
    {
        interfaceIdx->callbackFunctions.setupCommandIndFunc( &setupPrms);
        interfaceIdx = interfaceIdx->nextInterface;
    }

    if(setupPrms.hostToDeviceDataPtr)
        free(setupPrms.hostToDeviceDataPtr);
}

/************************************************************************
* Function:  USBMgrInititializeGlobalPrms
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static void USBMgrInitGlobalPrms(void)
{

    UINT8  endpointIdx = 0,rxEndpointIdx = 0 ,txEndpointIdx = 0;

    _usbMgrPrm.cableStatusReg          = FALSE;
    _usbMgrPrm.interfaceCounter        = 0;
    _usbMgrPrm.lisrRxEndpointsMask     = 0;
    _usbMgrPrm.lisrTxEndpoint          = 0;
    _usbMgrPrm.usbCableOut             = FALSE;
    _usbMgrPrm.continueTransmit        = FALSE;
    _usbMgrPrm.totalcfgDescSize        = 0;
    _usbMgrPrm.freeQueues              = FALSE;
    _usbMgrPrm.regDB                   = NULL;



    /****************intialize tx Lisr Params ********************/
    //_usbMgrLisrParams.txPrms[0].txEndpoint = USB_DEVICE_ENDPOINT_9;
    _usbMgrLisrParams.txPrms[0].txEndpoint = USB_COMM_DATA_IN_ENDPOINT_PHY;
    _usbMgrLisrParams.txPrms[1].txEndpoint = USB_COMM_CTRL_IN_ENDPOINT_PHY;
    _usbMgrLisrParams.txPrms[2].txEndpoint = USB_EMMI_IN_ENDPOINT_PHY;
    _usbMgrLisrParams.txPrms[3].txEndpoint = USB_MAST_IN_ENDPOINT_PHY;
    _usbMgrLisrParams.txPrms[4].txEndpoint = USB_ICAT_IN_ENDPOINT_PHY;

    for( txEndpointIdx = 0;txEndpointIdx < MAX_TX_ENDPOINTS ;txEndpointIdx++)
    {
        _usbMgrLisrParams.txPrms[txEndpointIdx].noOfTxBuffers = 0;
        _usbMgrLisrParams.txPrms[txEndpointIdx].transmitAll = FALSE;
    }


    /*****************intialize Rx Lisr Params ********************/
    _usbMgrLisrParams.rxPrms[0].rxEndpoint = USB_COMM_DATA_OUT_ENDPOINT_PHY;
    _usbMgrLisrParams.rxPrms[1].rxEndpoint = USB_EMMI_OUT_ENDPOINT_PHY;
    _usbMgrLisrParams.rxPrms[2].rxEndpoint = USB_MAST_OUT_ENDPOINT_PHY;
    _usbMgrLisrParams.rxPrms[3].rxEndpoint = USB_ICAT_OUT_ENDPOINT_PHY;

    for( rxEndpointIdx = 0;rxEndpointIdx < MAX_RX_ENDPOINTS ;rxEndpointIdx++)
    {
        _usbMgrLisrParams.rxPrms[rxEndpointIdx].recPtr       = NULL;
        _usbMgrLisrParams.rxPrms[rxEndpointIdx].recLen       = 0;
        _usbMgrLisrParams.rxPrms[rxEndpointIdx].endOfRecMsg  = TRUE;
    }

    for (endpointIdx = USB_DEVICE_ENDPOINT_1 ; endpointIdx<USB_DEVICE_TOTAL_ENDPOINTS; endpointIdx++)
    {
        _usbMgrDBEpArry[endpointIdx].available     = TRUE;
        _usbMgrDBEpArry[endpointIdx].interfaceHnd  = 0;
        _usbMgrDBEpArry[endpointIdx].endpointIdx   = 0;
        _usbMgrDBEpArry[endpointIdx].queueIdx      = 0;
        _usbMgrDBEpArry[endpointIdx].rxBufferIdx   = 0;
        _usbMgrDBEpArry[endpointIdx].txLisrIdx     = 0;
        _usbMgrDBEpArry[endpointIdx].rxLisrIdx     = 0;

    }


    _usbMgrLisrParams.ctrlPrms.ctrlEndpoint = 0;
    _usbMgrLisrParams.ctrlPrms.usbMgrStatus = USBMGR_STATUS_NONE;
    _usbMgrLisrParams.ctrlPrms.usbMgrError = USBMGR_ERROR_NONE;
    usbMgrRxEventsMask=0;


    if ( _usbMgr_UsbVersion == USB_VER_2_0 )
    {
        bcdUSB_LSB = 0x00;
        bcdUSB_MSB = 0x02;
        bEp0MaxPacketSize = (unsigned char)USB2_EP0_MAX_PACKET_SIZE_LSB;

     }
    else
    {
        bcdUSB_LSB = 0x10;
        bcdUSB_MSB = 0x01;
        bEp0MaxPacketSize = (unsigned char)USB1_EP0_MAX_PACKET_SIZE_LSB;
     }
    if ( _usbMgr_UsbSpeed == USB_SPEED_FS)    //Full Speed
    {
		bInterval = 0x80;
        bBulkMaxPacketSize_LSB = (unsigned char)USB1_BULK_EP_MAX_PACKET_SIZE_LSB;
        bBulkMaxPacketSize_MSB = (unsigned char)USB1_BULK_EP_MAX_PACKET_SIZE_MSB;
    }
    else  // Hi-Speed
    {
		bInterval = 0x1;
        bBulkMaxPacketSize_LSB = (unsigned char)USB2_BULK_EP_MAX_PACKET_SIZE_LSB;
        bBulkMaxPacketSize_MSB = (unsigned char)USB2_BULK_EP_MAX_PACKET_SIZE_MSB;
    }
}

/************************************************************************
* Function: USBMgrPhase1Init
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrPhase1Init(void)
{
    _usbMgr_UsbVersion = USBDeviceGetUSBVersionInUse();
    _usbMgr_UsbSpeed = USBDeviceGetUSBSpeedInUse();

    /** Intialize Global Parameters */
    USBMgrInitGlobalPrms();

    #if defined(USBMGR_TEST)
        USBMgrTestPhase1Init();
    #endif
}


/************************************************************************
* Function: USBMgrPhase2Init
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrPhase2Init(void)
{

	OS_STATUS	osStatus;	 

     //uart_printf("USBMgrPhase2Init start \r\n");

    Manitoba_Create_HISR(&USBMgrTxHISRBlock   , "USBMgrTxH"   , usbMgrTxIndicationHisr        , HISR_PRIORITY_2);
    Manitoba_Create_HISR(&USBMgrRxHISRBlock   , "USBMgrRxH"   , usbMgrRxIndicationHisr        , HISR_PRIORITY_2);
    Manitoba_Create_HISR(&USBMgrCtrlHISRBlock , "USBMgrCtrlH" , usbMgrControlIndicationHisr   , HISR_PRIORITY_2);
    Manitoba_Create_HISR(&USBMgrSetupHISRBlock, "USBMgrSetupH", usbMgrSetupReqHisr            , HISR_PRIORITY_2);

	osStatus = OSAFlagCreate(&usbMgrRxEventRef);
	ASSERT (osStatus == OS_SUCCESS);		 


  if(USBMgrInitUSBRegistration() != USBMGR_RC_OK)
    ASSERT (0);

	memset(usbMgrRxTaskStack, 0xA5, sizeof(usbMgrRxTaskStack));
	osStatus = OSATaskCreate(&usbMgrRxTaskRef,
							usbMgrRxTaskStack,
							sizeof(usbMgrRxTaskStack),
							USBMGR_PRIORITY,
							"USBMgrRxT",
							(void (*)(void*))usbMgrRxTask,
							NULL);
	ASSERT(osStatus == OS_SUCCESS);		 
}


void USBMgrEnumerateDevice(void)
{
    USBDeviceSimulatePlug(USBD_SimulatePlugE_OUT_AND_IN);
}

static BOOL USBMgrIsDeviceControllerEnabled (void)
{
    return USBDeviceIsControllerEnabled();
}


extern OSSemaRef _txSerialUartSem;


extern void setExtUsbWayLock(BOOL ifLock);
//ICAT EXPORTED FUNCTION - USB,USBMgr,Unplug
void USBMgrDeviceUnplug(void)
{
		OSA_STATUS  status;
		setExtUsbWayLock(TRUE);

		extern BOOL diag_block, diag_open;

		diag_block=TRUE;		
		diag_open=FALSE;
		

	

	 USBDeviceSimulatePlug(USBD_SimulatePlugE_OUT);
	  return;

}

//ICAT EXPORTED FUNCTION - USB,USBMgr,Plugin
void USBMgrDevicePlugIn(void)
{
    USBDeviceSimulatePlug(USBD_SimulatePlugE_IN);
	OSA_STATUS  status;
	return;

}



/************************************************************************
* Function:  USBMgrTempFuncUpdateHardCodedUSBDescriptor
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  must be called before pluging in USB
**********************************************************************/
//!!!!!! NOTE THIS IS ALL TEMPORARY IT THIS CODE WILL BE DELETED WHEN DYNAMIC DESCRIPTOR BUILDING WILL BE SUPPPORTED !!!!!
// note this is temp function USB descriptor must be created during applications registration
//and not set hard code , but at first stage it is not supported and USBmgr works only with
//listed below predefined applications
//each bit indicates which application to set
//bit 0 - ICAT ,
//bit 1 - Modem
//bit 2 - Genie
//bit 3 - MAST
//bit 4 - Modem single interface test , debug only
//!!! note that not all possible combinations are supported only a few selected combinations:


static __align(8) UINT8 devDesc[18];
static __align(8) UINT8 configDesc[128]; // setting maximum
static __align(8) UINT8 qualifDesc[10]; // not relevant to USB1 (USB2 only)
static __align(8) UINT8 otherSpeedDesc[128]; // // setting maximum (USB2 only)

//Manufacturer String descriptor
static __align(8) UINT8 strDescLang[16]=
{0x03,0x03,0x09,0x04};//supporting 0x0409 English (United States)

//Manufacturer String descriptor
static __align(8) UINT8 strDescManufacturer[16]=
{0x10,0x03,0x4D,0x00,0x61,0x00,0x72,0x00,0x76,0x00,0x65,0x00,0x6C,0x00,0x6C,0x00};//Marvell

//Product Composite String Descriptor
static __align(8) UINT8 strDescCompositeProduct[56]=
{0x38,0x03,0x4D,0x00,0x6F,0x00,0x62,0x00,0x69,0x00,0x6C,0x00,0x65,0x00,0x20,0x00,//"Mobile "
 0x43,0x00,0x6F,0x00,0x6D,0x00,0x70,0x00,0x6F,0x00,0x73,0x00,0x69,0x00,0x74,0x00,0x65,0x00,0x20,0x00,//"Composite "
 0x55,0x00,0x53,0x00,0x42,0x00,0x20,0x00, //"USB "
 0x44,0x00,0x65,0x00,0x76,0x00,0x69,0x00,0x63,0x00,0x65,0x00}; //"Device"

//Product String Descriptor
static __align(8) UINT8 strDescProduct[36]=
{0x24,0x03,0x4D,0x00,0x6F,0x00,0x62,0x00,0x69,0x00,0x6C,0x00,0x65,0x00,0x20,0x00,//"Mobile "
 0x55,0x00,0x53,0x00,0x42,0x00,0x20,0x00, //"USB "
 0x44,0x00,0x65,0x00,0x76,0x00,0x69,0x00,0x63,0x00,0x65,0x00}; //"Device"

//Serial number String Descriptor
static __align(8) UINT8 strDescSerialNumber[32]=
{0x20, 0x03, 0x5B, 0x00, 0x53, 0x00, 0x65, 0x00, 0x72, 0x00, 0x69, 0x00, 0x61, 0x00, 0x6C, 0x00,0x20, 0x00, //"[Serial "
 0x4E, 0x00, 0x75, 0x00, 0x6D, 0x00, 0x62, 0x00, 0x65, 0x00, 0x72, 0x00, 0x5D, 0x00}; //"Number]"

static __align(8) UINT8 strDescUsbNet[28]=
{0x1C, 0x03, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, 0x4e, 0x00, 0x45, 0x00, 0x54, 0x00,
 0x20, 0x00, 0x44, 0x00, 0x45, 0x00, 0x56, 0x00, 0x49, 0x00, 0x43, 0x00, 0x45, 0x00 }; //"Number]"
 #ifdef MV_USB2_MASS_STORAGE
 static __align(8) UINT8 strDescMassStorage[0x30]=
	 {
	 0x30, 0x03, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, \
	 0x20, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x73, 0x00, \
	 0x73, 0x00, 0x20, 0x00, 0x53, 0x00, 0x74, 0x00, \
	 0x6F, 0x00, 0x72, 0x00, 0x61, 0x00, 0x67, 0x00, \
	 0x65, 0x00, 0x20, 0x00, 0x44, 0x00, 0x65, 0x00, \
	 0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00
	 };
#endif
static  UINT32 current_usb_app_mask=0;

/******************* USB Descriptors Start****************/

#include "mvUsbCh9.h"

/*====================== For Nezha DKB ===================*/
typedef struct _FULL_USB_DESCRIPTOR_NZDKB
{
	DEVICE_DESCRIPTOR dev_desc;

	QUALIFIER_DESCRIPTOR qualif_desc;

	CONFIGURATION_DESCRIPTOR config_desc;

	INTERFACE_DESCRIPTOR intf_desc0;
	ENDPOINT_DESCRIPTOR ep_desc00;
	ENDPOINT_DESCRIPTOR ep_desc01;
	ENDPOINT_DESCRIPTOR ep_desc02;

	INTERFACE_DESCRIPTOR intf_desc1;
	ENDPOINT_DESCRIPTOR ep_desc10;
	ENDPOINT_DESCRIPTOR ep_desc11;

	INTERFACE_DESCRIPTOR intf_desc2;
	UINT8 				class_desc1[19];
	ENDPOINT_DESCRIPTOR ep_desc20;
	ENDPOINT_DESCRIPTOR ep_desc21;
	ENDPOINT_DESCRIPTOR ep_desc22;
	

#if DUAL_AT_MODEM
	INTERFACE_DESCRIPTOR intf_desc3;
	UINT8 				class_desc2[19];
	ENDPOINT_DESCRIPTOR ep_desc202;
	ENDPOINT_DESCRIPTOR ep_desc212;
	ENDPOINT_DESCRIPTOR ep_desc222;
#endif
#if 1
     INTERFACE_DESCRIPTOR intf_desc4;
     ENDPOINT_DESCRIPTOR ep_desc40;
     ENDPOINT_DESCRIPTOR ep_desc41;
#endif
#ifdef MV_USB2_MASS_STORAGE
    INTERFACE_DESCRIPTOR intf_desc5;
    ENDPOINT_DESCRIPTOR ep_desc50;
    ENDPOINT_DESCRIPTOR ep_desc51;
#endif

} FULL_USB_DESC_NZDKB;

FULL_USB_DESC_NZDKB FullUsbDesc_NZDKB = 
{
	//device decriptor
	{
		.bLength = sizeof(DEVICE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
		.bcdUSD[0] = USB2_BCD_LSB,
		.bcdUSD[1] = USB2_BCD_MSB,
		.bDeviceClass = 0x00,
		.bDeviceSubClass = 0x00,
		.bDeviceProtocol = 0x00,
		.bMaxPacketSize = USB2_EP0_MAX_PACKET_SIZE_LSB,
		.idVendor[0] = 0x86, // marvell
		.idVendor[1] = 0x12,
		.idProduct[0] = 0x0E,
		.idProduct[1] = 0x81,
		.bcdDevice[0] = 0x01,
		.bcdDevice[1] = 0x00,
		.iManufacturer =0x00,// 0x01,
		.iProduct = 0x00,//0x03,
		.iSerialNumber = 0x00,//0x04,
		.bNumConfigurations = 0x01
	},

	//qualifier descriptor
	{
		.bLength = sizeof(QUALIFIER_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_QUALIFIER,
		.bcdUSD[0] = 0x00,
		.bcdUSD[1] = 0x02,
		.bDeviceClass = 0x00,
		.bDeviceSubClass = 0x00,
		.bDeviceProtocol = 0x00,
		.bMaxPacketSize = 0x40,
		.bNumConfigurations = 0x01,
		.bReserved = 0
	},
	
	//configuration desc
	{
		.bLength  = 0x09, // bLength				- Descriptor length
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_CONFIGURATION, // bDescriptorType		- Descriptor Type
        .wTotalLength[0]  = (sizeof(FULL_USB_DESC_NZDKB) 
					        - sizeof(DEVICE_DESCRIPTOR)
					        - sizeof(QUALIFIER_DESCRIPTOR)) & 0xFF,
        .wTotalLength[1]  = (sizeof(FULL_USB_DESC_NZDKB) 
					        - sizeof(DEVICE_DESCRIPTOR)
					        - sizeof(QUALIFIER_DESCRIPTOR))  >> 8,
#ifdef MV_USB2_MASS_STORAGE
 #if DUAL_AT_MODEM
	.bNumInterfaces  = 0x06, // bNumInterfaces		- Number of interfaces this configuration supports
 #else
	.bNumInterfaces  = 0x05,
 #endif
#else
 #if DUAL_AT_MODEM
	.bNumInterfaces  = 0x05,
 #else
	.bNumInterfaces  = 0x04,
 #endif
#endif
        .bConfigurationValue  = 0x01, // bConfigurationValue   - The Value that should be used to select this configuration
        .iConfig  = 0x00, // iConfiguration		- Index of string descriptor describing this configuration
        .bmAttributes  = 0xC0,//0xE0, // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        .bMaxPower  = 0xFA // MaxPower				- Maximum power consumption for this configuration (mA)},
    },
    //interface 0 desc: usbnet
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x00,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 0x03,
		.bInterfaceClass = 0xff,
		.bInterfaceSubClass = 0xff,
		.bInterfaceProtocol = 0x00,
		.iInterface = 0x00,//0x05
	},
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x86,//0x85, //IN 5
		.bmAttributes = 0x03,
		.wMaxPacketSize[0] = 0x0F,
		.wMaxPacketSize[1] = 0x00,
		.iInterval = 0x01
	},

	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x83,//0x81, //IN 1
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x04,//0x02, //OUT 2
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	//interface 1 desc: acat
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x01,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 0x02,
		.bInterfaceClass = 0xff,
		.bInterfaceSubClass = 0x00,
		.bInterfaceProtocol = 0x00,
		.iInterface = 0x00
	},
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x87, //IN 7
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x08, //OUT 8
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},

	//interface 2 desc: modem
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x02,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 0x03,
		.bInterfaceClass = 0x02,
		.bInterfaceSubClass = 0x02,
		.bInterfaceProtocol = 0x01,
		.iInterface = 0x00
	},
	//class specific desc
	{
	   0x05, // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
	   0x24, // descriptor type
	   0x00, // subtype = header
	   0x10, // BCD
	   0x01, //

	   0x05, // 2. descriptor length - Call Manangement Func Desc
	   0x24, // descriptor type
	   0x01, // subtype = Call management
	   0x00, //bmcapabilities, MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
	   0x01, // Interface number of data class interface

	   0x04, // 3.descriptor length  Abstract Control Func Desc
	   0x24, // descriptor type
	   0x02, // subtype = Abstract CM
	   0x00, // bmcapabilities 

	   0x05, // Size of descriptor. Number of interfaces plus 3 bytes of header
	   0x24, // 0x24 - CS_INTERFACE
	   0x06, // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
	   0x00,
	   0x01
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x85,//0x86, //IN 6
		.bmAttributes = 0x03,
		.wMaxPacketSize[0] = 0x0F, 
		.wMaxPacketSize[1] = 0, 
		.iInterval = 0x01	
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x81,//0x83, //IN 3
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x02,//0x04, //OUT 4
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
		
#if DUAL_AT_MODEM
	//interface 3 desc: modem
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x03,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 0x03,
		.bInterfaceClass = 0x02,
		.bInterfaceSubClass = 0x02,
		.bInterfaceProtocol = 0x01,
		.iInterface = 0x00
	},
	//class specific desc
	{
	   0x05, // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
	   0x24, // descriptor type
	   0x00, // subtype = header
	   0x10, // BCD
	   0x01, //

	   0x05, // 2. descriptor length - Call Manangement Func Desc
	   0x24, // descriptor type
	   0x01, // subtype = Call management
	   0x00, //bmcapabilities, MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
	   0x02, // Interface number of data class interface

	   0x04, // 3.descriptor length  Abstract Control Func Desc
	   0x24, // descriptor type
	   0x02, // subtype = Abstract CM
	   0x00, // bmcapabilities 

	   0x05, // Size of descriptor. Number of interfaces plus 3 bytes of header
	   0x24, // 0x24 - CS_INTERFACE
	   0x06, // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
	   0x02,
	   0x03
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x8b, 
		.bmAttributes = 0x03,
		.wMaxPacketSize[0] = 0x0F, 
		.wMaxPacketSize[1] = 0, 
		.iInterval = 0x01	
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x89, 
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x0a, 
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
#endif
#if 1
    //interface 4 desc: sulog
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x04,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 0x02,
		.bInterfaceClass = 0x0a,
		.bInterfaceSubClass = 0x00,
		.bInterfaceProtocol = 0xff,
		.iInterface = 0x00
	},
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x8c, //IN c
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x0d, //OUT d
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
#endif
#if MV_USB2_MASS_STORAGE
    //interface 5 desc: usb mass storage
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x05,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 0x02,
		.bInterfaceClass = 0x08,
		.bInterfaceSubClass = 0x06,
		.bInterfaceProtocol = 0x50,
		.iInterface = 0x06 //string index
	},
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x8e, //IN c, tx
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
	
	//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
   		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
   		.bEndpointAddress = 0x0f, //OUT d, rx
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB, 
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB, 
		.iInterval = 0x00		
	},
#endif


};

/*====================== For Nezha HSIC(2chip) ===================*/
typedef struct _FULL_USB_DESCRIPTOR_NZ2CHIP
{
	DEVICE_DESCRIPTOR dev_desc;

	QUALIFIER_DESCRIPTOR qualif_desc;

	CONFIGURATION_DESCRIPTOR config_desc;

	INTERFACE_DESCRIPTOR intf_desc0;
	ENDPOINT_DESCRIPTOR ep_desc00;
	ENDPOINT_DESCRIPTOR ep_desc01;
	ENDPOINT_DESCRIPTOR ep_desc02;
	ENDPOINT_DESCRIPTOR ep_desc03;
	ENDPOINT_DESCRIPTOR ep_desc04;
	ENDPOINT_DESCRIPTOR ep_desc05;
	ENDPOINT_DESCRIPTOR ep_desc06;
	ENDPOINT_DESCRIPTOR ep_desc07;
	ENDPOINT_DESCRIPTOR ep_desc08;
	ENDPOINT_DESCRIPTOR ep_desc09;
	ENDPOINT_DESCRIPTOR ep_desc0A;
	ENDPOINT_DESCRIPTOR ep_desc0B;
	ENDPOINT_DESCRIPTOR ep_desc0C;
	ENDPOINT_DESCRIPTOR ep_desc0D;
	
} FULL_USB_DESC_NZ2CHIP;

FULL_USB_DESC_NZ2CHIP FullUsbDesc_NZ2CHIP = {

	//.dev_desc = 
	//device decriptor
	{
		.bLength = sizeof(DEVICE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
		.bcdUSD[0] = USB2_BCD_LSB,
		.bcdUSD[1] = USB2_BCD_MSB,
		.bDeviceClass = 0x02,
		.bDeviceSubClass = 0x00,
		.bDeviceProtocol = 0x00,
		.bMaxPacketSize = USB2_EP0_MAX_PACKET_SIZE_LSB,
		.idVendor[0] = 0x86, // marvell
		.idVendor[1] = 0x12,
		.idProduct[0] = 0x30,
		.idProduct[1] = 0x81,
		.bcdDevice[0] = 0x01,
		.bcdDevice[1] = 0x00,
		.iManufacturer = 0x01,
		.iProduct = 0x03,
		.iSerialNumber = 0x04,
		.bNumConfigurations = 0x01
	},

	//.qualif_desc = 
	//qualifier descriptor
	{
		.bLength = sizeof(QUALIFIER_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_QUALIFIER,
		.bcdUSD[0] = 0x00,
		.bcdUSD[1] = 0x02,
		.bDeviceClass = 0x00,
		.bDeviceSubClass = 0x00,
		.bDeviceProtocol = 0x00,
		.bMaxPacketSize = 0x40,
		.bNumConfigurations = 0x01,
		.bReserved = 0
	},

	//.config_desc = 
	//configuration desc
	{
		.bLength  = 0x09, // bLength				- Descriptor length
        .bDescriptorType  = USB_DESCRIPTOR_TYPE_CONFIGURATION, // bDescriptorType		- Descriptor Type
        .wTotalLength[0]  = (sizeof(FULL_USB_DESC_NZ2CHIP) 
					        - sizeof(DEVICE_DESCRIPTOR)
					        - sizeof(QUALIFIER_DESCRIPTOR)) & 0xFF,
        .wTotalLength[1]  = (sizeof(FULL_USB_DESC_NZ2CHIP) 
					        - sizeof(DEVICE_DESCRIPTOR)
					        - sizeof(QUALIFIER_DESCRIPTOR))  >> 8,
        .bNumInterfaces  = 0x01, // bNumInterfaces		- Number of interfaces this configuration supports
        .bConfigurationValue  = 0x01, // bConfigurationValue   - The Value that should be used to select this configuration
        .iConfig  = 0x00, // iConfiguration		- Index of string descriptor describing this configuration
        .bmAttributes  = 0xE0, // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        .bMaxPower  = 0xFA // MaxPower				- Maximum power consumption for this configuration (mA)},
    },

	//.intf_desc0 = 
    //interface 0 desc: all channel in one interface
	{	
		.bLength = sizeof(INTERFACE_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
		.bInterfaceNumber = 0x00,
		.bAlternateSetting = 0x00,
		.bNumEndpoints = 14,
		.bInterfaceClass = 0x02,
		.bInterfaceSubClass = 0x00,
		.bInterfaceProtocol = 0x00,
		.iInterface = 0x05
	},

	//.ep_desc00 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x81, //IN 1
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc01 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x02, //OUT 2
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},
	
	//.ep_desc02 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x83, //IN 3
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc03 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x04, //OUT 4
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},
	
	//.ep_desc04 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x85, //IN 5
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc05 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x06, //OUT 6
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc06 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x87, //IN 7
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc07 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x08, //OUT 8
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc08 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x89, //IN 9
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc09 =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x0A, //OUT 10
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},
	
	//.ep_desc0A =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x8B, //IN 11
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc0B =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x0C, //OUT 12
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},
	
	//.ep_desc0C =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x8D, //IN 13
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

	//.ep_desc0D =//endpoint descriptor
	{
		.bLength = sizeof(ENDPOINT_DESCRIPTOR),
		.bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
		.bEndpointAddress = 0x0E, //OUT 14
		.bmAttributes = 0x02,
		.wMaxPacketSize[0] = USB2_BULK_EP_MAX_PACKET_SIZE_LSB,
		.wMaxPacketSize[1] = USB2_BULK_EP_MAX_PACKET_SIZE_MSB,
		.iInterval = 0x00
	},

};
/******************* USB Descriptors End****************/
void set_current_usb_app_mask(UINT32 usb_app_mask)
{
	current_usb_app_mask  = usb_app_mask;
}

UINT32 get_current_usb_app_mask(void)
{
	/* fixed by pxxiong for workaround in Protocol Stack calling */
	return current_usb_app_mask;

}
#ifdef MV_USB2_MASS_STORAGE
void mvUsbStorageConfigure(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbMscProperties_T *pMscProp = GetMscProperties();
    //mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    //FlashLayoutConfInfo *pFlashLayout = GetFlashLayoutConfig();

    memset(pMscProp, 0x00, sizeof(mvUsbMscProperties_T));

    /* Initialize MSC0 to invalid media type. */
    pMscProp->Media[0]  = USBMSC_MEDIA_U32;

    if (sdcard_is_insert()/* && !diag_online_sdl_enable()*/)
    {
        uart_printf("mvUsbStorageConfigure init :%s:%d\n", __FILE__, __LINE__);
        pMscProp->MscMaxLun = 0;

        /* Set the current Logical Unit disc to SD-Disk. */
		pMscProp->Media[pMscProp->MscMaxLun] = USBMSC_SDCARD;
    }
    /* If MSC0 is invalid media, We should disable mass storage. */
    if (pMscProp->Media[0] == USBMSC_MEDIA_U32)
    {
        pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
    }

    uart_printf("mass_storage %d, MscMaxLun %d, MscStartAddress [0x%x, 0x%x, 0x%x]",
                pUsbDrvConfig->mass_storage, pMscProp->MscMaxLun, pMscProp->Lun0StartAddress,
                pMscProp->Lun1StartAddress,  pMscProp->Lun2StartAddress);

    ASSERT(pMscProp->MscMaxLun < MSC_MAX_LUN);
}
#endif

void USBMgrTempFuncUpdateHardCodedUSBDescriptor(UINT32 usb_app_mask)
{
    UINT16 dev_desc_length,config_desc_length, qualif_desc_length, other_speed_desc_length;

    current_usb_app_mask  = usb_app_mask;
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	#ifdef MV_USB2_MASS_STORAGE
	Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    if (pUsbDrvConfig->mass_storage == MASS_STORAGE_ENABLE)
    {
        mvUsbStorageConfigure();
    }
	#endif

	{
		dev_desc_length 	= sizeof(DEVICE_DESCRIPTOR);	   //device desc length
		qualif_desc_length	= sizeof(QUALIFIER_DESCRIPTOR);
	    config_desc_length  = FullUsbDesc_NZDKB.config_desc.wTotalLength[0];    //config descriptor length

		/* Other Speed Desciptore relevant only for HW supporting USB2.0  */    
		memcpy(otherSpeedDesc, &FullUsbDesc_NZDKB.config_desc, config_desc_length);
		otherSpeedDesc[1] =  USB_DESCRIPTOR_TYPE_SPEED;     /* overwrite bDescType to DEVICE Qualifier descriptor type */
		other_speed_desc_length = config_desc_length;

	    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_DEVICE, &FullUsbDesc_NZDKB.dev_desc, dev_desc_length, 0);		
	    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_CONFIGURATION , &FullUsbDesc_NZDKB.config_desc, config_desc_length, 0);
	    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_QUALIFIER, &FullUsbDesc_NZDKB.qualif_desc, qualif_desc_length, 0);
	    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_SPEED, &FullUsbDesc_NZDKB.config_desc, other_speed_desc_length, 0);
	}

	
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescLang, sizeof(strDescLang), 0);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescManufacturer, sizeof(strDescManufacturer), 1);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescProduct, sizeof(strDescProduct), 2);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescCompositeProduct, sizeof(strDescCompositeProduct), 3);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescSerialNumber, sizeof(strDescSerialNumber), 4);
	//strDescUsbNet should be added when USB net is chosen
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescUsbNet, sizeof(strDescUsbNet),5);
	#ifdef MV_USB2_MASS_STORAGE
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescMassStorage, sizeof(strDescMassStorage),6);
    #endif
/*---------------------------discard all code below this line-----------------*/

	return;


    if(usb_app_mask == 1)  //ICAT ONLY
    {

		//Device descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = bcdUSB_LSB; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
        devDesc[3]  = bcdUSB_MSB; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = bEp0MaxPacketSize; //  bMaxPacketSize0     - Max Packet Size for EP zero
        devDesc[8]  = 0x86; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x12; //  idVendor (MSB)
        devDesc[10] = 0x07; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x81; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x02; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x04; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x20; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x32; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0xff; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0xff; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0xff; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //Endpoint Descriptor
        configDesc[18] = 0x07; // bLength				- Descriptor length
        configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

        //Endpoint Descriptor
        configDesc[25] = 0x07; // bLength				- Descriptor length
        configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[31] = 0x00; // bInterval				- Polling Interval (ms)
    }


    if(usb_app_mask == 2)  //MODEM ONLY
    {
		//Device Descriptor
        devDesc[0]  = 0x12;  //  bLength				- Descriptor length
        devDesc[1]  = 0x01;  //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10;  //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01;  //  bcdUSB (MSB)
        devDesc[4]  = 0x02;  //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00;  //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00;  //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10;  //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB;  //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A;  //  idVendor (MSB)
        devDesc[10] = 0x12;  //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00;  //  idProduct (MSB)
        devDesc[12] = 0x00;  //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00;  //  bcdDevice (MSB)
        devDesc[14] = 0x00;  //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x00;  //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x00;  //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01;  //  bNumConfigurations	- Number of configurations

		//Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x43; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x02; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

		//Interface Descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x01; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Modem Class Specific Descriptors
		configDesc[18] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
        configDesc[19] = 0x24; // descriptor type
        configDesc[20] = 0x00; // subtype = header
        configDesc[21] = 0x00; // BCD
        configDesc[22] = 0x01; //

		configDesc[23] = 0x05; // 2. descriptor length - Call Manangement Func Desc
        configDesc[24] = 0x24; // descriptor type
        configDesc[25] = 0x01; // subtype = Call management
        configDesc[26] = 0x03; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
        configDesc[27] = 0x01; // Interface number of data class interface

        configDesc[28] = 0x04; // 3.descriptor length  Abstract Control Func Desc
        configDesc[29] = 0x24; // descriptor type
        configDesc[30] = 0x02; // subtype = Abstract CM
        configDesc[31] = 0x03; // bmcapabilities USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_NETCON_NOTIFY,USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_SEND_BREAK

		configDesc[32] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
        configDesc[33] = 0x24; // 0x24 - CS_INTERFACE
        configDesc[34] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
        configDesc[35] = 0x00;
        configDesc[36] = 0x01;

		//Endpoint Descriptor
		configDesc[37] = 0x07; // bLength				- Descriptor length
        configDesc[38] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[39] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[40] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[41] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[42] = 0x00; // wMaxPacketSize (MSB)
        configDesc[43] = bInterval; // bInterval				- Polling Interval (ms)

		//Interface Descriptor for Data
		configDesc[44] = 0x09; // bLength				- Descriptor length
        configDesc[45] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[46] = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[47] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[48] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[49] = 0x0A; // bInterfaceClass       - Class of this Interface
        configDesc[50] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[51] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[52] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Endpoint Descriptor
		configDesc[53] = 0x07; // Length of descriptor
        configDesc[54] = 0x05; // Type of descriptor
        configDesc[55] = 0x81; // Endpoint address
        configDesc[56] = 0x02; // Endpoint attributes
        configDesc[57] = 0x40; // 64 Maximum packet size (LSB)
        configDesc[58] = 0x00; // Maximum packet size (MSB)
        configDesc[59] = 0x00; // Polling interval

		//Endpoint Descriptor
		configDesc[60] = 0x07; // bLength				- Descriptor length
        configDesc[61] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[62] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[63] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[64] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[65] = 0x00; // wMaxPacketSize (MSB)
        configDesc[66] = 0x00; // bInterval				- Polling Interval (ms)
//modem end
    }


    if(usb_app_mask == 4)  //GENNIE ONLY
    {
		//Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x10; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x20; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface Descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x01; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //GENIE endpoints
        configDesc[18] = 0x07; // bLength				- Descriptor length
        configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] = 0x89; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] = 0x00; // wMaxPacketSize (MSB)
        configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[25] = 0x07; // bLength				- Descriptor length
        configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] = 0x0A; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] = 0x00; // wMaxPacketSize (MSB)
        configDesc[32] = 0x00; // bInterval				- Polling Interval (ms)

    }




    if(usb_app_mask == 8)  //MAST ONLY
    {
		//Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x30; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x02; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x03; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations


		//Configuration descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x20; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

		//Interface Descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x08; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x06; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x50; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//EP IN desc
        configDesc[18] = 0x07; // bLength				- Descriptor length
        configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] = 0x83; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] = 0x00; // wMaxPacketSize (MSB)
        configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

		//EP OUT desc
        configDesc[25] = 0x07; // bLength				- Descriptor length
        configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] = 0x04; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] = 0x00; // wMaxPacketSize (MSB)
        configDesc[31] = 0x00; // bInterval				- Polling Interval (ms)


    }


     if(usb_app_mask == 16) //modem try only , debug
     {
        //Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x02; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x40; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x3A; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface Descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x03; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Modem class specific descriptors
        configDesc[18] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
        configDesc[19] = 0x24; // descriptor type
        configDesc[20] = 0x00; // subtype = header
        configDesc[21] = 0x00; // BCD
        configDesc[22] = 0x01; //

		configDesc[23] = 0x05; // 2. descriptor length - Call Manangement Func Desc
        configDesc[24] = 0x24; // descriptor type
        configDesc[25] = 0x01; // subtype = Call management
        configDesc[26] = 0x03; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
        configDesc[27] = 0x01; // Interface number of data class interface

		configDesc[28] = 0x04; // 3.descriptor length  Abstract Control Func Desc
        configDesc[29] = 0x24; // descriptor type
        configDesc[30] = 0x02; // subtype = Abstract CM
        configDesc[31] = 0x03; // bmcapabilities USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_NETCON_NOTIFY,USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_SEND_BREAK

		configDesc[32] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
        configDesc[33] = 0x24; // 0x24 - CS_INTERFACE
        configDesc[34] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
        configDesc[35] = 0x00;
        configDesc[36] = 0x01;

		//Endpoint Descriptor
        configDesc[37] = 0x07; // bLength				- Descriptor length
        configDesc[38] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[39] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[40] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[41] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[42] = 0x00; // wMaxPacketSize (MSB)
        configDesc[43] = bInterval; // bInterval				- Polling Interval (ms)

        configDesc[44] = 0x07; // bLength				- Descriptor length
        configDesc[45] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[46] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[47] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[48] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[49] = 0x00; // wMaxPacketSize (MSB)
        configDesc[50] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[51] = 0x07; // bLength				- Descriptor length
        configDesc[52] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[53] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[54] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[55] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[56] = 0x00; // wMaxPacketSize (MSB)
        configDesc[57] = 0x00; // bInterval				- Polling Interval (ms)

    }




if(usb_app_mask == 66) //MODEM & ICAT
 {
       
	   //Device Descriptor
			   devDesc[0]  = 0x12; //  bLength			   - Descriptor length
			   devDesc[1]  = 0x01; //  bDescriptorType	   - Descriptor  Type
			   devDesc[2]  = bcdUSB_LSB; //  bcdUSB (LSB)		 - Device Compliant to USB specification ..
			   devDesc[3]  = bcdUSB_MSB; //  bcdUSB (MSB)
			   devDesc[4]  = 0x00; //  bDeviceClass 	   - class of the device
			   devDesc[5]  = 0x00; //  bDeviceSubClass	   - subclass of the device
			   devDesc[6]  = 0x00; //  bDeviceProtocol	   - protocol of the device
			   devDesc[7]  = bEp0MaxPacketSize; //	bMaxPacketSize0 	- Max Packet Size for EP zero
			   devDesc[8]  = 0x86; //  idVendor (LSB)	   - Vendor ID
			   devDesc[9]  = 0x12; //  idVendor (MSB)
			   devDesc[10] = 0x0D; //  idProduct (LSB)	   - Product ID
			   devDesc[11] = 0x81; //  idProduct (MSB)
			   devDesc[12] = 0x00; //  bcdDevice (LSB)	   - The device release number
			   devDesc[13] = 0x00; //  bcdDevice (MSB)
			   devDesc[14] = 0x01; //  iManufacturer	   - Index of string descriptor describing Manufacturer
			   devDesc[15] = 0x03; //  iProduct 		   - Index of string descriptor describing Product
			   devDesc[16] = 0x04; //  iSerialNumber	   - Index of string descriptor describing Serial number
			   devDesc[17] = 0x01; //  bNumConfigurations  - Number of configurations
       

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 55; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x02; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)
        

        //Interface Descriptor CDC
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0xff; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface


        configDesc[18] = 0x07; // bLength				- Descriptor length
        configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[25] = 0x07; // bLength				- Descriptor length
        configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[31] = 0x00; // bInterval				- Polling Interval (ms)

        //�nterface descriptor ICAT
        configDesc[32] = 0x09; // bLength				- Descriptor length
        configDesc[33] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[34] = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[35] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[36] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[37] = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[38] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[39] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[40] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //ICAT endpoints
        configDesc[41] = 0x07; // bLength				- Descriptor length
        configDesc[42] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[43] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[44] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[45] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[46] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[47] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[48] = 0x07; // bLength				- Descriptor length
        configDesc[49] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[50] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[51] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[52] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[53] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[54] = 0x00; // bInterval				- Polling Interval (ms)

    }


	if(usb_app_mask == 3) //MODEM & ICAT
	{
		//Device Descriptor
		devDesc[0]  = 0x12; //  bLength				- Descriptor length
		devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
		devDesc[2]  = bcdUSB_LSB; //  bcdUSB (LSB)        - Device Compliant to USB specification ..
		devDesc[3]  = bcdUSB_MSB; //  bcdUSB (MSB)
		devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
		devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
		devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
		devDesc[7]  = bEp0MaxPacketSize; //  bMaxPacketSize0     - Max Packet Size for EP zero
		devDesc[8]  = 0x86; //  idVendor (LSB)		- Vendor ID
		devDesc[9]  = 0x12; //  idVendor (MSB)
		devDesc[10] = 0x27; //  idProduct (LSB)		- Product ID
		devDesc[11] = 0x4e; //  idProduct (MSB)
		devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
		devDesc[13] = 0x00; //  bcdDevice (MSB)
		devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
		devDesc[15] = 0x03; //  iProduct			- Index of string descriptor describing Product
		devDesc[16] = 0x04; //  iSerialNumber		- Index of string descriptor describing Serial number
		devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

		//Configuration Descriptor
		configDesc[0]  = 0x09; // bLength				- Descriptor length
		configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
		configDesc[2]  = 0x51; // wTotalLength (LSB)	- Total Data length for the configuration,
		configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
		configDesc[4]  = 0x02; // bNumInterfaces		- Number of interfaces this configuration supports
		configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
		configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
		configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
		configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

		//Interface Descriptor MODEM
		configDesc[9]  = 0x09; // bLength				- Descriptor length
		configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
		configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[13] = 0x03; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
		configDesc[15] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Modem class specific Interface descriptors
		configDesc[18] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
		configDesc[19] = 0x24; // descriptor type
		configDesc[20] = 0x00; // subtype = header
		configDesc[21] = 0x00; // BCD
		configDesc[22] = 0x01; //

		configDesc[23] = 0x05; // 2. descriptor length - Call Manangement Func Desc
		configDesc[24] = 0x24; // descriptor type
		configDesc[25] = 0x01; // subtype = Call management
		configDesc[26] = 0x03; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
		configDesc[27] = 0x01; // Interface number of data class interface

		configDesc[28] = 0x04; // 3.descriptor length  Abstract Control Func Desc
		configDesc[29] = 0x24; // descriptor type
		configDesc[30] = 0x02; // subtype = Abstract CM
		configDesc[31] = 0x03; // bmcapabilities USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_NETCON_NOTIFY,USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_SEND_BREAK

		configDesc[32] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
		configDesc[33] = 0x24; // 0x24 - CS_INTERFACE
		configDesc[34] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
		configDesc[35] = 0x00;
		configDesc[36] = 0x01;

		//Endpoint Descriptor
		configDesc[37] = 0x07; // bLength				- Descriptor length
		configDesc[38] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[39] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[40] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[41] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[42] = 0x00; // wMaxPacketSize (MSB)
		configDesc[43] = bInterval; // bInterval				- Polling Interval (ms)

		configDesc[44] = 0x07; // bLength				- Descriptor length
		configDesc[45] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[46] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[47] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[48] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[49] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[50] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[51] = 0x07; // bLength				- Descriptor length
		configDesc[52] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[53] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[54] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[55] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[56] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[57] = 0x00; // bInterval				- Polling Interval (ms)

		//�nterface descriptor ICAT
		configDesc[58] = 0x09; // bLength				- Descriptor length
		configDesc[59] = 0x04; // bDescriptorType		- Descriptor Type
		configDesc[60] = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[61] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[62] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[63] = 0xFF; // bInterfaceClass       - Class of this Interface
		configDesc[64] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[65] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[66] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//ICAT endpoints
		configDesc[67] = 0x07; // bLength				- Descriptor length
		configDesc[68] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[69] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[70] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[71] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[72] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[73] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[74] = 0x07; // bLength				- Descriptor length
		configDesc[75] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[76] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[77] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[78] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[79] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[80] = 0x00; // bInterval				- Polling Interval (ms)

	}


    if(usb_app_mask == 7) //MODEM , ICAT, GENIE
    {
       //Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x37; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration  Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x68; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x03; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface Descriptor MODEM
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x03; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Modem class specific Interface descriptors
        configDesc[18] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
        configDesc[19] = 0x24; // descriptor type
        configDesc[20] = 0x00; // subtype = header
        configDesc[21] = 0x00; // BCD
        configDesc[22] = 0x01; //

		configDesc[23] = 0x05; // 2. descriptor length - Call Manangement Func Desc
        configDesc[24] = 0x24; // descriptor type
        configDesc[25] = 0x01; // subtype = Call management
        configDesc[26] = 0x03; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
        configDesc[27] = 0x01; // Interface number of data class interface

		configDesc[28] = 0x04; // 3.descriptor length  Abstract Control Func Desc
        configDesc[29] = 0x24; // descriptor type
        configDesc[30] = 0x02; // subtype = Abstract CM
        configDesc[31] = 0x03; // bmcapabilities USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_NETCON_NOTIFY,USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_SEND_BREAK

		configDesc[32] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
        configDesc[33] = 0x24; // 0x24 - CS_INTERFACE
        configDesc[34] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
        configDesc[35] = 0x00;
        configDesc[36] = 0x01;

		//Endpoint Descriptor MODEM
        configDesc[37] = 0x07; // bLength				- Descriptor length
        configDesc[38] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[39] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[40] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[41] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[42] = 0x00; // wMaxPacketSize (MSB)
        configDesc[43] = bInterval; // bInterval				- Polling Interval (ms)

        configDesc[44] = 0x07; // bLength				- Descriptor length
        configDesc[45] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[46] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[47] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[48] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[49] = 0x00; // wMaxPacketSize (MSB)
        configDesc[50] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[51] = 0x07; // bLength				- Descriptor length
        configDesc[52] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[53] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[54] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[55] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[56] = 0x00; // wMaxPacketSize (MSB)
        configDesc[57] = 0x00; // bInterval				- Polling Interval (ms)

        //Interface Descriptor ICAT
        configDesc[58] = 0x09; // bLength				- Descriptor length
        configDesc[59] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[60] = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[61] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[62] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[63] = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[64] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[65] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[66] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //ICAT endpoints
        configDesc[67] = 0x07; // bLength				- Descriptor length
        configDesc[68] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[69] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[70] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[71] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[72] = 0x00; // wMaxPacketSize (MSB)
        configDesc[73] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[74] = 0x07; // bLength				- Descriptor length
        configDesc[75] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[76] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[77] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[78] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[79] = 0x00; // wMaxPacketSize (MSB)
        configDesc[80] = 0x00; // bInterval				- Polling Interval (ms)

        //Interface Descriptor GENIE
        configDesc[81] = 0x09; // bLength				- Descriptor length
        configDesc[82] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[83] = 0x02; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[84] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[85] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[86] = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[87] = 0x01; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[88] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[89] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //GENIE endpoints
        configDesc[90] = 0x07; // bLength				- Descriptor length
        configDesc[91] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[92] = 0x89; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[93] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[94] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[95] = 0x00; // wMaxPacketSize (MSB)
        configDesc[96] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[97]  = 0x07; // bLength				- Descriptor length
        configDesc[98]  = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[99]  = 0x0A; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[100] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[101] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[102] = 0x00; // wMaxPacketSize (MSB)
        configDesc[103] = 0x00; // bInterval				- Polling Interval (ms)

    }


    if(usb_app_mask == 15) //MODEM & ICAT & GENIE & MAST
    {
        //Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x4F; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x02; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x03; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x7F; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x04; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface Descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x03; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Modem class specific descriptors
        configDesc[18] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
        configDesc[19] = 0x24; // descriptor type
        configDesc[20] = 0x00; // subtype = header
        configDesc[21] = 0x00; // BCD
        configDesc[22] = 0x01; //

		configDesc[23] = 0x05; // 2. descriptor length - Call Manangement Func Desc
        configDesc[24] = 0x24; // descriptor type
        configDesc[25] = 0x01; // subtype = Call management
        configDesc[26] = 0x03; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
        configDesc[27] = 0x01; // Interface number of data class interface

		configDesc[28] = 0x04; // 3.descriptor length  Abstract Control Func Desc
        configDesc[29] = 0x24; // descriptor type
        configDesc[30] = 0x02; // subtype = Abstract CM
        configDesc[31] = 0x03; // bmcapabilities USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_NETCON_NOTIFY,USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_SEND_BREAK

		configDesc[32] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
        configDesc[33] = 0x24; // 0x24 - CS_INTERFACE
        configDesc[34] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
        configDesc[35] = 0x00;
        configDesc[36] = 0x01;

		//Endpoint Descriptors MODEM
        configDesc[37] = 0x07; // bLength				- Descriptor length
        configDesc[38] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[39] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[40] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[41] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[42] = 0x00; // wMaxPacketSize (MSB)
        configDesc[43] = bInterval; // bInterval				- Polling Interval (ms)

        configDesc[44] = 0x07; // bLength				- Descriptor length
        configDesc[45] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[46] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[47] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[48] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[49] = 0x00; // wMaxPacketSize (MSB)
        configDesc[50] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[51] = 0x07; // bLength				- Descriptor length
        configDesc[52] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[53] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[54] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[55] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[56] = 0x00; // wMaxPacketSize (MSB)
        configDesc[57] = 0x00; // bInterval				- Polling Interval (ms)

        //Interface Descriptor ICAT
        configDesc[58] = 0x09; // bLength				- Descriptor length
        configDesc[59] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[60] = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[61] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[62] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[63] = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[64] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[65] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[66] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //ICAT endpoints
        configDesc[67] = 0x07; // bLength				- Descriptor length
        configDesc[68] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[69] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[70] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[71] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[72] = 0x00; // wMaxPacketSize (MSB)
        configDesc[73] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[74] = 0x07; // bLength				- Descriptor length
        configDesc[75] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[76] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[77] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[78] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[79] = 0x00; // wMaxPacketSize (MSB)
        configDesc[80] = 0x00; // bInterval				- Polling Interval (ms)

        //Interface Descriptor GENIE
        configDesc[81] = 0x09; // bLength				- Descriptor length
        configDesc[82] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[83] = 0x02; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[84] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[85] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[86] = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[87] = 0x01; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[88] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[89] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //GENIE endpoints
        configDesc[90] = 0x07; // bLength				- Descriptor length
        configDesc[91] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[92] = 0x89; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[93] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[94] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[95] = 0x00; // wMaxPacketSize (MSB)
        configDesc[96] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[97] = 0x07;  // bLength				- Descriptor length
        configDesc[98] = 0x05;  // bDescriptorType		- Descriptor Type
        configDesc[99] = 0x0A;  // bEndpointAddress		- Endpoint Address & Direction
        configDesc[100] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[101] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[102] = 0x00; // wMaxPacketSize (MSB)
        configDesc[103] = 0x00; // bInterval				- Polling Interval (ms)

		//Interface Descriptor MAST
        configDesc[104]  = 0x09; // bLength				- Descriptor length
        configDesc[105]  = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[106]  = 0x03; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[107]  = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[108]  = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[109]  = 0x08; // bInterfaceClass       - Class of this Interface
        configDesc[110]  = 0x06; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[111]  = 0x50; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[112]  = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//EP IN desc
        configDesc[113] = 0x07; // bLength				- Descriptor length
        configDesc[114] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[115] = 0x83; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[116] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[117] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[118] = 0x00; // wMaxPacketSize (MSB)
        configDesc[119] = 0x00; // bInterval				- Polling Interval (ms)

		//EP OUT desc
        configDesc[120] = 0x07; // bLength				- Descriptor length
        configDesc[121] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[122] = 0x04; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[123] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[124] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[125] = 0x00; // wMaxPacketSize (MSB)
        configDesc[126] = 0x00; // bInterval				- Polling Interval (ms)

    }

	if(usb_app_mask == 0xC)  //MAST & Gennie
	{
		//Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x2C; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x37; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x02; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)


		//Interface Descriptor MAST
        configDesc[9] =  0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x08; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x06; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x50; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        //MAST endpoint
        configDesc[18] =0x07; // bLength				- Descriptor length
        configDesc[19] =0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] =0x83; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] =0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] =0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] =0x00; // wMaxPacketSize (MSB)
        configDesc[24] =0x00; // bInterval				- Polling Interval (ms)

		//
        configDesc[25] =0x07; // bLength				- Descriptor length
        configDesc[26] =0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] =0x04; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] =0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] =0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] =0x00; // wMaxPacketSize (MSB)
        configDesc[31] =0x00; // bInterval				- Polling Interval (ms)



		//Interface Descriptor Genie
        configDesc[32]  = 0x09; // bLength				- Descriptor length
        configDesc[33]  = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[34]  = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[35]  = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[36]  = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[37]  = 0xFF; // bInterfaceClass       - Class of this Interface
        configDesc[38]  = 0x01; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[39]  = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[40]  = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//EP IN descs
        configDesc[41] =  0x07; // bLength				- Descriptor length
        configDesc[42] =  0x05; // bDescriptorType		- Descriptor Type
        configDesc[43] =  0x89; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[44] =  0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[45] =  0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[46] =  0x00; // wMaxPacketSize (MSB)
        configDesc[47] =  0x00; // bInterval				- Polling Interval (ms)

		//EP OUT desc
        configDesc[48] =  0x07; // bLength				- Descriptor length
        configDesc[49] =  0x05; // bDescriptorType		- Descriptor Type
        configDesc[50] =  0x0A; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[51] =  0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[52] =  0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[53] =  0x00; // wMaxPacketSize (MSB)
        configDesc[54] =  0x00; // bInterval				- Polling Interval (ms)


	}

    if(usb_app_mask == 0xA) //MODEM  & MAST
    {
        //Device Descriptor
        devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
        devDesc[3]  = 0x01; //  bcdUSB (MSB)
        devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
        devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x0A; //  idVendor (MSB)
        devDesc[10] = 0x2A; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x00; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x02; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x03; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 0x51; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x02; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface Descriptor
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x03; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//Modem class specific descriptors
        configDesc[18] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
        configDesc[19] = 0x24; // descriptor type
        configDesc[20] = 0x00; // subtype = header
        configDesc[21] = 0x00; // BCD
        configDesc[22] = 0x01; //

		configDesc[23] = 0x05; // 2. descriptor length - Call Manangement Func Desc
        configDesc[24] = 0x24; // descriptor type
        configDesc[25] = 0x01; // subtype = Call management
        configDesc[26] = 0x03; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
        configDesc[27] = 0x01; // Interface number of data class interface

		configDesc[28] = 0x04; // 3.descriptor length  Abstract Control Func Desc
        configDesc[29] = 0x24; // descriptor type
        configDesc[30] = 0x02; // subtype = Abstract CM
        configDesc[31] = 0x03; // bmcapabilities USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_NETCON_NOTIFY,USB_COMM_BMCAP_ABCON_DEV_SUPPORTS_SEND_BREAK

		configDesc[32] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
        configDesc[33] = 0x24; // 0x24 - CS_INTERFACE
        configDesc[34] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
        configDesc[35] = 0x00;
        configDesc[36] = 0x01;

		//Endpoint Descriptors MODEM
        configDesc[37] = 0x07; // bLength				- Descriptor length
        configDesc[38] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[39] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[40] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[41] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[42] = 0x00; // wMaxPacketSize (MSB)
        configDesc[43] = bInterval; // bInterval				- Polling Interval (ms)

        configDesc[44] = 0x07; // bLength				- Descriptor length
        configDesc[45] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[46] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[47] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[48] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[49] = 0x00; // wMaxPacketSize (MSB)
        configDesc[50] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[51] = 0x07; // bLength				- Descriptor length
        configDesc[52] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[53] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[54] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[55] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[56] = 0x00; // wMaxPacketSize (MSB)
        configDesc[57] = 0x00; // bInterval				- Polling Interval (ms)


		//Interface Descriptor MAST
        configDesc[58]  = 0x09; // bLength				- Descriptor length
        configDesc[59]  = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[60]  = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[61]  = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[62]  = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[63]  = 0x08; // bInterfaceClass       - Class of this Interface
        configDesc[64]  = 0x06; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[65]  = 0x50; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[66]  = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//EP IN desc
        configDesc[67] = 0x07; // bLength				- Descriptor length
        configDesc[68] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[69] = 0x83; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[70] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[71] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[72] = 0x00; // wMaxPacketSize (MSB)
        configDesc[73] = 0x00; // bInterval				- Polling Interval (ms)

		//EP OUT desc
        configDesc[74] = 0x07; // bLength				- Descriptor length
        configDesc[75] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[76] = 0x04; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[77] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[78] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[79] = 0x00; // wMaxPacketSize (MSB)
        configDesc[80] = 0x00; // bInterval				- Polling Interval (ms)
	}

	if(usb_app_mask == 5) //ICAT, GENIE
	{
		//Device Descriptor
		devDesc[0]  = 0x12; //  bLength				- Descriptor length
		devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
		devDesc[2]  = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
		devDesc[3]  = 0x01; //  bcdUSB (MSB)
		devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
		devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
		devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
		devDesc[7]  = 0x10; //  bMaxPacketSize0		- Max Packet Size for EP zero
		devDesc[8]  = 0xEB; //  idVendor (LSB)		- Vendor ID
		devDesc[9]  = 0x0A; //  idVendor (MSB)
		devDesc[10] = 0x25; //  idProduct (LSB)		- Product ID
		devDesc[11] = 0x00; //  idProduct (MSB)
		devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
		devDesc[13] = 0x00; //  bcdDevice (MSB)
		devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
		devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
		devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
		devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

		//Configuration  Descriptor
		configDesc[0]  = 0x09; // bLength				- Descriptor length
		configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
		configDesc[2]  = 0x37; // wTotalLength (LSB)	- Total Data length for the configuration,
		configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
		configDesc[4]  = 0x02; // bNumInterfaces		- Number of interfaces this configuration supports
		configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
		configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
		configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
		configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)


		//Interface Descriptor ICAT
		configDesc[9]  = 0x09; // bLength				- Descriptor length
		configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
		configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[14] = 0xFF; // bInterfaceClass       - Class of this Interface
		configDesc[15] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[16] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//ICAT endpoints
		configDesc[18] = 0x07; // bLength				- Descriptor length
		configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[20] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[22] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[23] = 0x00; // wMaxPacketSize (MSB)
		configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[25] = 0x07; // bLength				- Descriptor length
		configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[27] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[29] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[30] = 0x00; // wMaxPacketSize (MSB)
		configDesc[31] = 0x00; // bInterval				- Polling Interval (ms)

		//Interface Descriptor GENIE
		configDesc[32] = 0x09; // bLength				- Descriptor length
		configDesc[33] = 0x04; // bDescriptorType		- Descriptor Type
		configDesc[34] = 0x01; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[35] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[36] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[37] = 0xFF; // bInterfaceClass       - Class of this Interface
		configDesc[38] = 0x01; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[39] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[40] = 0x00; // iInterface			- Index of string descriptor describing this Interface

		//GENIE endpoints
		configDesc[41] = 0x07; // bLength				- Descriptor length
		configDesc[42] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[43] = 0x89; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[44] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[45] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[46] = 0x00; // wMaxPacketSize (MSB)
		configDesc[47] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[48] = 0x07; // bLength				- Descriptor length
		configDesc[49] = 0x05; // bDescriptorType		- Descriptor Type
		configDesc[50] = 0x0A; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[51] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[52] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[53] = 0x00; // wMaxPacketSize (MSB)
		configDesc[54] = 0x00; // bInterval				- Polling Interval (ms)
    }
	if(usb_app_mask == 22) //ICAT, GENIE
	{
		devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = bcdUSB_LSB; //  bcdUSB (LSB)        - Device Compliant to USB specification ..
        devDesc[3]  = bcdUSB_MSB; //  bcdUSB (MSB)
        devDesc[4]  = 0x02; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = bEp0MaxPacketSize; //  bMaxPacketSize0     - Max Packet Size for EP zero
        devDesc[8]  = 0x71; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x04; //  idVendor (MSB)
        devDesc[10] = 0x77; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x07; //  idProduct (MSB)
        devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x01; //  bcdDevice (MSB)
        devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 32; // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
        configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)

        //Interface Descriptor MODEM
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0xff; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface


        configDesc[18] = 0x07; // bLength				- Descriptor length
        configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] = 0x87; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] = 0x00; // wMaxPacketSize (MSB)
        configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[25] = 0x07; // bLength				- Descriptor length
        configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] = 0x08; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] = 0x00; // wMaxPacketSize (MSB)
        configDesc[31] = 0x00; // bInterval
    }

	/* Qualifier Desciptore relevant only for HW supporting USB2.0  */

	if(usb_app_mask == 9) //Android
	{
		//Device Descriptor
		devDesc[0]  = 0x12; //  bLength				- Descriptor length
		devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
		devDesc[2]  = 0x00; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
		devDesc[3]  = 0x00; //  bcdUSB (MSB)
		devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
		devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
		devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
		devDesc[7]  = 0x40; //  bMaxPacketSize0		- Max Packet Size for EP zero
		devDesc[8]  = 0x86; //  idVendor (LSB)		- Vendor ID
		devDesc[9]  = 0x12; //  idVendor (MSB)
		devDesc[10] = 0x02; //  idProduct (LSB)		- Product ID
		devDesc[11] = 0x80; //  idProduct (MSB)
		devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
		devDesc[13] = 0x00; //  bcdDevice (MSB)
		devDesc[14] = 0x00; //  iManufacturer		- Index of string descriptor describing Manufacturer
		devDesc[15] = 0x00; //  iProduct			- Index of string descriptor describing Product
		devDesc[16] = 0x00; //  iSerialNumber		- Index of string descriptor describing Serial number
		devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

		//Configuration  Descriptor
		configDesc[0]  =  0x09; // bLength				- Descriptor length
		configDesc[1]  =  0x02; // bDescriptorType		- Descriptor Type
		configDesc[2]  =  0x68; // wTotalLength (LSB)	- Total Data length for the configuration,
		configDesc[3]  =  0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
		configDesc[4]  =  0x03; // bNumInterfaces		- Number of interfaces this configuration supports
		configDesc[5]  =  0x02; // bConfigurationValue   - The Value that should be used to select this configuration
		configDesc[6]  =  0x00; // iConfiguration		- Index of string descriptor describing this configuration
		configDesc[7]  =  0xC0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
		configDesc[8]  =  0x01; // MaxPower				- Maximum power consumption for this configuration (mA)
		configDesc[9]  =  0x08; // bLength				- Descriptor length
		configDesc[10] =  0x0B; // bDescriptorType		- Descriptor Type
		configDesc[11] =  0x00; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[12] =  0x02; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[13] =  0x02; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[14] =  0x02; // bInterfaceClass       - Class of this Interface
		configDesc[15] =  0x00; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[16] =  0x00; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[17] =  0x03; // iInterface			- Index of string descriptor describing this Interface
		configDesc[18] =  0x09; // bLength				- Descriptor length
		configDesc[19] =  0x03; // bDescriptorType		- Descriptor Type
		configDesc[20] =  0x09; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[21] =  0x04; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[22] =  0x00; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[23] =  0x00; // wMaxPacketSize (MSB)
		configDesc[24] =  0x01; // bInterval				- Polling Interval (ms)
		configDesc[25] =  0x02; // bLength				- Descriptor length
		configDesc[26] =  0x02; // bDescriptorType		- Descriptor Type
		configDesc[27] =  0x01; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[28] =  0x00; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[29] =  0x05; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[30] =  0x24; // wMaxPacketSize (MSB)
		configDesc[31] =  0x00; // bInterval				- Polling Interval (ms)
		configDesc[32] =  0x10; // bLength				- Descriptor length
		configDesc[33] =  0x01; // bDescriptorType		- Descriptor Type
		configDesc[34] =  0x05; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[35] =  0x24; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[36] =  0x01; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[37] =  0x00; // bInterfaceClass       - Class of this Interface
		configDesc[38] =  0x01; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[39] =  0x04; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[40] =  0x24; // iInterface			- Index of string descriptor describing this Interface
		configDesc[41] =  0x02; // bLength				- Descriptor length
		configDesc[42] =  0x00; // bDescriptorType		- Descriptor Type
		configDesc[43] =  0x05; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[44] =  0x24; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[45] =  0x06; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[46] =  0x00; // wMaxPacketSize (MSB)
		configDesc[47] =  0x01; // bInterval				- Polling Interval (ms)
		configDesc[48] =  0x07; // bLength				- Descriptor length
		configDesc[49] =  0x05; // bDescriptorType		- Descriptor Type
		configDesc[50] =  0x85; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[51] =  0x03; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[52] =  0x08; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[53] =  0x00; // wMaxPacketSize (MSB)
		configDesc[54] =  0x09; // bInterval				- Polling Interval (ms)
		configDesc[55]  =  0x09; // bConfigurationValue   - The Value that should be used to select this configuration
		configDesc[56]  =  0x04; // iConfiguration		- Index of string descriptor describing this configuration
		configDesc[57]  =  0x01; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
		configDesc[58]  =  0x00; // MaxPower				- Maximum power consumption for this configuration (mA)
		configDesc[59]  =  0x02; // bLength				- Descriptor length
		configDesc[60] =  0x0A; // bDescriptorType		- Descriptor Type
		configDesc[61] =  0x00; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[62] =  0x00; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[63] =  0x00; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[64] =  0x07; // bInterfaceClass       - Class of this Interface
		configDesc[65] =  0x05; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[66] =  0x81; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[67] =  0x02; // iInterface			- Index of string descriptor describing this Interface
		configDesc[68] =  0x00; // bLength				- Descriptor length
		configDesc[69] =  0x02; // bDescriptorType		- Descriptor Type
		configDesc[70] =  0x00; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[71] =  0x07; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[72] =  0x05; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[73] =  0x02; // wMaxPacketSize (MSB)
		configDesc[74] =  0x02; // bInterval				- Polling Interval (ms)
		configDesc[75] =  0x00; // bLength				- Descriptor length
		configDesc[76] =  0x02; // bDescriptorType		- Descriptor Type
		configDesc[77] =  0x00; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[78] =  0x03; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[79] =  0x09; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[80] =  0x03; // wMaxPacketSize (MSB)
		configDesc[81] =  0x09; // bInterval				- Polling Interval (ms)
		configDesc[82] =  0x04; // bLength				- Descriptor length
		configDesc[83] =  0x02; // bDescriptorType		- Descriptor Type
		configDesc[84] =  0x00; // bInterfaceNumber      - Index (Number) of this interfaces
		configDesc[85] =  0x02; // bAlternateSetting     - The value to select alternate setting of this interface
		configDesc[86] =  0x0A; // bNumEndpoints			- Number endpoints	used by this interface
		configDesc[87] =  0x00; // bInterfaceClass       - Class of this Interface
		configDesc[88] =  0xFF; // bInterfaceSubClass    - Sub class of this Interface
		configDesc[89] =  0x00; // bInterfaceProtocol    - Protocol of this Interface
		configDesc[90] =  0x07; // iInterface			- Index of string descriptor describing this Interface
		configDesc[91] =  0x05; // bLength				- Descriptor length
		configDesc[92] =  0x87; // bDescriptorType		- Descriptor Type
		configDesc[93] =  0x02; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[94] =  0x00; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[95] =  0x02; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[96] =  0x00; // wMaxPacketSize (MSB)
		configDesc[97] =  0x07; // bInterval				- Polling Interval (ms)
		configDesc[98] =  0x05; // bLength				- Descriptor length
		configDesc[99] =  0x08; // bDescriptorType		- Descriptor Type
		configDesc[100] =  0x02; // bEndpointAddress		- Endpoint Address & Direction
		configDesc[101] =  0x00; // bmAttributes          - BULK,ISO,Interrupt
		configDesc[102] =  0x02; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[103] =  0x00; // wMaxPacketSize (MSB)
			
		
		
    }
	if(usb_app_mask == 10) //usb net
	{
	extern BOOL is_2chip_platform;
	if(is_2chip_platform==FALSE)
	{
		
		uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
		devDesc[0]  = 0x12; //  bLength				- Descriptor length
		devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
		 devDesc[2]  = bcdUSB_LSB; //  bcdUSB (LSB) 	   - Device Compliant to USB specification ..
		 devDesc[3]  = bcdUSB_MSB; //  bcdUSB (MSB)
		 devDesc[4]  = 0x00; //  bDeviceClass		 - class of the device
		 devDesc[5]  = 0x00; //  bDeviceSubClass	 - subclass of the device
		 devDesc[6]  = 0x00; //  bDeviceProtocol	 - protocol of the device
		 devDesc[7]  = bEp0MaxPacketSize; //  bMaxPacketSize0	  - Max Packet Size for EP zero
		 devDesc[8]  = 0x86; //  idVendor (LSB) 	 - Vendor ID
		 devDesc[9]  = 0x12; //  idVendor (MSB)
		 devDesc[10] = 0x0E; //  idProduct (LSB)	 - Product ID
		 devDesc[11] = 0x81; //  idProduct (MSB)
		 devDesc[12] = 0x01; //  bcdDevice (LSB)	 - The device release number
		 devDesc[13] = 0x00; //  bcdDevice (MSB)
		 devDesc[14] = 0x00; //  iManufacturer		 - Index of string descriptor describing Manufacturer
		 devDesc[15] = 0x00; //  iProduct			 - Index of string descriptor describing Product
		 devDesc[16] = 0x00; //  iSerialNumber		 - Index of string descriptor describing Serial number
		 devDesc[17] = 0x01; //  bNumConfigurations  - Number of configurations
		
		 //Configuration Descriptor
		 configDesc[0]	= 0x09; // bLength				 - Descriptor length
		 configDesc[1]	= 0x02; // bDescriptorType		 - Descriptor Type
		 configDesc[2]	= 111;	 // wTotalLength (LSB)	 - Total Data length for the configuration,
		 configDesc[3]	= 0x00; // wTotalLength (MSB)	 - includes all descriptors for this configuration
		 configDesc[4]	= 0x03; // bNumInterfaces		 - Number of interfaces this configuration supports
		 configDesc[5]	= 0x01; // bConfigurationValue	 - The Value that should be used to select this configuration
		 configDesc[6]	= 0x00; // iConfiguration		 - Index of string descriptor describing this configuration
		 configDesc[7]	= 0xC0; // bmAttributes 		 - bit6: Self-Powered, bit5: RemoteWakeup
		// configDesc[8]  = 0x00; // MaxPower				 - Maximum power consumption for this configuration (mA)
		 configDesc[8]	= 0xfa;
		
		 //Interface Descriptor MODEM
		
		 configDesc[9]	= 0x09; // bLength				 - Descriptor length
		 configDesc[10] = 0x04; // bDescriptorType		 - Descriptor Type
		 configDesc[11] = 0x00; // bInterfaceNumber 	 - Index (Number) of this interfaces
		 configDesc[12] = 0x00; // bAlternateSetting	 - The value to select alternate setting of this interface
		 configDesc[13] = 0x03; // bNumEndpoints		 - Number endpoints  used by this interface
		 configDesc[14] = 0xff; // bInterfaceClass		 - Class of this Interface
		 configDesc[15] = 0xff; // bInterfaceSubClass	 - Sub class of this Interface
		 configDesc[16] = 0x00; // bInterfaceProtocol	 - Protocol of this Interface
		 configDesc[17] = 0x00; // iInterface			 - Index of string descriptor describing this Interface
		
		
		 //Endpoint Descriptor
		 configDesc[18] = 0x07; // bLength				 - Descriptor length
		 configDesc[19] = 0x05; // bDescriptorType		 - Descriptor Type
		 configDesc[20] = 0x86; // bEndpointAddress 	 - Endpoint Address & Direction
		 configDesc[21] = 0x03; // bmAttributes 		 - BULK,ISO,Interrupt
		 configDesc[22] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
		 configDesc[23] = 0x00; // wMaxPacketSize (MSB)
		 configDesc[24] = bInterval; // bInterval				 - Polling Interval (ms)
		
		 configDesc[25] = 0x07; // bLength				 - Descriptor length
		 configDesc[26] = 0x05; // bDescriptorType		 - Descriptor Type
		 configDesc[27] = 0x83; // bEndpointAddress 	 - Endpoint Address & Direction
		 configDesc[28] = 0x02; // bmAttributes 		 - BULK,ISO,Interrupt
		 configDesc[29] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		 configDesc[30] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		 configDesc[31] = 0x00; // bInterval			 - Polling Interval (ms)
		
		 configDesc[32] = 0x07; // bLength				 - Descriptor length
		 configDesc[33] = 0x05; // bDescriptorType		 - Descriptor Type
		 configDesc[34] = 0x04; // bEndpointAddress 	 - Endpoint Address & Direction
		 configDesc[35] = 0x02; // bmAttributes 		 - BULK,ISO,Interrupt
		 configDesc[36] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		 configDesc[37] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		 configDesc[38] = 0x00; // bInterval			 - Polling Interval (ms)
		
		 //�nterface descriptor ICAT
		 configDesc[39] = 0x09; // bLength				 - Descriptor length
		 configDesc[40] = 0x04; // bDescriptorType		 - Descriptor Type
		 configDesc[41] = 0x01; // bInterfaceNumber 	 - Index (Number) of this interfaces
		 configDesc[42] = 0x00; // bAlternateSetting	 - The value to select alternate setting of this interface
		 configDesc[43] = 0x02; // bNumEndpoints		 - Number endpoints  used by this interface
		 configDesc[44] = 0xFF; // bInterfaceClass		 - Class of this Interface
		 configDesc[45] = 0x00; // bInterfaceSubClass	 - Sub class of this Interface
		 configDesc[46] = 0x00; // bInterfaceProtocol	 - Protocol of this Interface
		 configDesc[47] = 0x00; // iInterface			 - Index of string descriptor describing this Interface
		
		 //ICAT endpoints
		 configDesc[48] = 0x07; // bLength				 - Descriptor length
		 configDesc[49] = 0x05; // bDescriptorType		 - Descriptor Type
		 configDesc[50] = 0x87; // bEndpointAddress 	 - Endpoint Address & Direction
		 configDesc[51] = 0x02; // bmAttributes 		 - BULK,ISO,Interrupt
		 configDesc[52] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		 configDesc[53] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		 configDesc[54] = 0x00; // bInterval			 - Polling Interval (ms)
		
		 configDesc[55] = 0x07; // bLength				 - Descriptor length
		 configDesc[56] = 0x05; // bDescriptorType		 - Descriptor Type
		 configDesc[57] = 0x08; // bEndpointAddress 	 - Endpoint Address & Direction
		 configDesc[58] = 0x02; // bmAttributes 		 - BULK,ISO,Interrupt
		 configDesc[59] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		 configDesc[60] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		 configDesc[61] = 0x00; // bInterval			 - Polling Interval (ms)
 		//Modem class specific descriptors
        configDesc[62]  = 0x09; // bLength				- Descriptor length
        configDesc[63] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[64] = 0x02; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[65] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[66] = 0x03; // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[67] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[68] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[69] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[70] = 0x00; // iInterface			- Index of string descriptor describing this Interface

        configDesc[71] = 0x05; // descriptor length Comms Class CS_INTERFACE 'Functional Descriptors' Triplet
        configDesc[72] = 0x24; // descriptor type
        configDesc[73] = 0x00; // subtype = header
        configDesc[74] = 0x10; // BCD
        configDesc[75] = 0x01; //

		configDesc[76] = 0x05; // 2. descriptor length - Call Manangement Func Desc
        configDesc[77] = 0x24; // descriptor type
        configDesc[78] = 0x01; // subtype = Call management
        configDesc[79] = 0x00; //bmcapabilities; MS driver usbser.sys seems to ignore this bit and sends AT over the data,interface anyway
        configDesc[80] = 0x01; // Interface number of data class interface

		configDesc[81] = 0x04; // 3.descriptor length  Abstract Control Func Desc
        configDesc[82] = 0x24; // descriptor type
        configDesc[83] = 0x02; // subtype = Abstract CM
        configDesc[84] = 0x00; // bmcapabilities 

		configDesc[85] = 0x05; // Size of descriptor. Number of interfaces plus 3 bytes of header
        configDesc[86] = 0x24; // 0x24 - CS_INTERFACE
        configDesc[87] = 0x06; // 0x06 - See table 25 of document "USB Class definitions for Comms Devices"
        configDesc[88] = 0x00;
        configDesc[89] = 0x01;

		//Endpoint Descriptors MODEM
        configDesc[90] = 0x07; // bLength				- Descriptor length
        configDesc[91] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[92] = 0x85; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[93] = 0x03; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[94] = 0x0F; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[95] = 0x00; // wMaxPacketSize (MSB)
        configDesc[96] = bInterval; // bInterval				- Polling Interval (ms)

        configDesc[97] = 0x07; // bLength				- Descriptor length
        configDesc[98] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[99] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[100] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[101] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[102] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[103] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[104] = 0x07; // bLength				- Descriptor length
        configDesc[105] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[106] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[107] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[108] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[109] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[110] = 0x00; // bInterval				- Polling Interval (ms)
		
    }
	else
	{
		
		devDesc[0]  = 0x12; //  bLength				- Descriptor length
        devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
        devDesc[2]  = bcdUSB_LSB; //  bcdUSB (LSB)        - Device Compliant to USB specification ..
        devDesc[3]  = bcdUSB_MSB; //  bcdUSB (MSB)
        devDesc[4]  = 0x02; //  bDeviceClass		- class of the device
        devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
        devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
        devDesc[7]  = bEp0MaxPacketSize; //  bMaxPacketSize0     - Max Packet Size for EP zero
        devDesc[8]  = 0x86; //  idVendor (LSB)		- Vendor ID
        devDesc[9]  = 0x12; //  idVendor (MSB)
        devDesc[10] = 0x30; //  idProduct (LSB)		- Product ID
        devDesc[11] = 0x81; //  idProduct (MSB)
        devDesc[12] = 0x01; //  bcdDevice (LSB)		- The device release number
        devDesc[13] = 0x00; //  bcdDevice (MSB)
        devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
        devDesc[15] = 0x03; //  iProduct			- Index of string descriptor describing Product
        devDesc[16] = 0x04; //  iSerialNumber		- Index of string descriptor describing Serial number
        devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

        //Configuration Descriptor
        configDesc[0]  = 0x09; // bLength				- Descriptor length
        configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
        configDesc[2]  = 116;   // wTotalLength (LSB)	- Total Data length for the configuration,
        configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
        configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
        configDesc[5]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
        configDesc[6]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
        configDesc[7]  = 0xE0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
       // configDesc[8]  = 0x00; // MaxPower				- Maximum power consumption for this configuration (mA)
		configDesc[8]  = 0xfa;

        //Interface Descriptor MODEM
        configDesc[9]  = 0x09; // bLength				- Descriptor length
        configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
        configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
        configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
        configDesc[13] = 14;   // bNumEndpoints			- Number endpoints	used by this interface
        configDesc[14] = 0x02; // bInterfaceClass       - Class of this Interface
        configDesc[15] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
        configDesc[16] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
        configDesc[17] = 0x05; // iInterface			- Index of string descriptor describing this Interface


		//Endpoint Descriptor
        configDesc[18] = 0x07; // bLength				- Descriptor length
        configDesc[19] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[20] = 0x81; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[21] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[22] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[23] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[24] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[25] = 0x07; // bLength				- Descriptor length
        configDesc[26] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[27] = 0x02; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[28] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[29] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[30] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[31] = 0x00; // bInterval				- Polling Interval (ms)

        configDesc[32] = 0x07; // bLength				- Descriptor length
        configDesc[33] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[34] = 0x83; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[35] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[36] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[37] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[38] = 0x00; // bInterval				- Polling Interval (ms)

		configDesc[39] = 0x07; // bLength				- Descriptor length
        configDesc[40] = 0x05; // bDescriptorType		- Descriptor Type
        configDesc[41] = 0x04; // bEndpointAddress		- Endpoint Address & Direction
        configDesc[42] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
        configDesc[43] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
        configDesc[44] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
        configDesc[45] = 0x00; // bInterval				- Polling Interval (ms)


		configDesc[46] = 0x07; // bLength			   - Descriptor length
		configDesc[47] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[48] = 0x85; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[49] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[50] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[51] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[52] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[53] = 0x07; // bLength			   - Descriptor length
		configDesc[54] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[55] = 0x06; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[56] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[57] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[58] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[59] = 0x00; // bInterval			   - Polling Interval (ms)

		
		configDesc[60] = 0x07; // bLength			   - Descriptor length
		configDesc[61] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[62] = 0x87; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[63] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[64] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[65] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[66] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[67] = 0x07; // bLength			   - Descriptor length
		configDesc[68] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[69] = 0x08; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[70] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[71] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[72] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[73] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[74] = 0x07; // bLength			   - Descriptor length
		configDesc[75] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[76] = 0x89; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[77] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[78] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[79] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[80] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[81] = 0x07; // bLength			   - Descriptor length
		configDesc[82] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[83] = 0x0a; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[84] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[85] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[86] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[87] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[88] = 0x07; // bLength			   - Descriptor length
		configDesc[89] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[90] = 0x8b; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[91] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[92] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[93] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[94] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[95] = 0x07; // bLength			   - Descriptor length
		configDesc[96] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[97] = 0x0c; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[98] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[99] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[100] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[101] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[102] = 0x07; // bLength			   - Descriptor length
		configDesc[103] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[104] = 0x8d; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[105] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[106] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[107] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[108] = 0x00; // bInterval			   - Polling Interval (ms)

		configDesc[109] = 0x07; // bLength			   - Descriptor length
		configDesc[110] = 0x05; // bDescriptorType	   - Descriptor Type
		configDesc[111] = 0x0e; // bEndpointAddress	   - Endpoint Address & Direction
		configDesc[112] = 0x02; // bmAttributes		   - BULK,ISO,Interrupt
		configDesc[113] = bBulkMaxPacketSize_LSB; // wMaxPacketSize (LSB)  - Max packet size
		configDesc[114] = bBulkMaxPacketSize_MSB; // wMaxPacketSize (MSB)
		configDesc[115] = 0x00; // bInterval			   - Polling Interval (ms)


    }
	}


	
	qualifDesc[0] =  sizeof(qualifDesc);
	qualifDesc[1] =  0x06;     /* bDescType This is a DEVICE Qualifier descr */
	qualifDesc[2] =  0x00;     /* bcdUSB USB revision 2.0 */
	qualifDesc[3] =  0x02; 	   /* bcdUSB USB revision 2.0 */
	qualifDesc[4] =  0x00;     /* bDeviceClass */
	qualifDesc[5] =  0x00;	  /* bDeviceSubClass */
	qualifDesc[6] =  0x00;    /* bDeviceProtocol*/
	qualifDesc[7] =  0x40;    /* bMaxPacketSize0 */
	qualifDesc[8] =  0x01;    /* bNumConfigurations */
 	qualifDesc[9] =  0x00;

	/* Other Speed Desciptore relevant only for HW supporting USB2.0  */
	memcpy(otherSpeedDesc, configDesc, configDesc[2]);
	otherSpeedDesc[1] =  0x07;     /* overwrite bDescType to DEVICE Qualifier descriptor type */


    dev_desc_length     = devDesc[0];       //device desc length
    config_desc_length  = configDesc[2];    //config descriptor length
	qualif_desc_length 	= qualifDesc[0];
	other_speed_desc_length = otherSpeedDesc[2];

    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_DEVICE, devDesc, dev_desc_length, 0);
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_CONFIGURATION , configDesc, config_desc_length, 0);
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_QUALIFIER, qualifDesc, qualif_desc_length, 0);
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_SPEED, otherSpeedDesc, other_speed_desc_length, 0);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescLang, sizeof(strDescLang), 0);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescManufacturer, sizeof(strDescManufacturer), 1);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescProduct, sizeof(strDescProduct), 2);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescCompositeProduct, sizeof(strDescCompositeProduct), 3);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescSerialNumber, sizeof(strDescSerialNumber), 4);
	//strDescUsbNet should be added when USB net is chosen
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescUsbNet, sizeof(strDescUsbNet),5);
}


/************************************************************************
* Function:  UsbMgrGetIfIndexByAppId
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
**********************************************************************/
//!!!!!! NOTE THIS IS ALL TEMPORARY IT THIS CODE WILL BE DELETED WHEN DYNAMIC DESCRIPTOR BUILDING WILL BE SUPPPORTED !!!!!
//the implementation will change interface will remain the same
//as the descriptor is hard-coded the interfaces numbers are hard-coded as well
UINT8 UsbMgrGetIfIndexByAppId(UINT8 appID)
{
    switch(current_usb_app_mask)
    {
    case 1: //ICAT  only
       ASSERT(appID == USBMGR_ICAT_APPID);
       return(0);
    case 16: //MODEM ONLY
    case 2: //MODEM ONLY
        ASSERT(appID == USBMGR_MODEM_APPID);
        return(0);
    case 4: //GENNIE ONLY
        ASSERT(appID == USBMGR_GENIE_APPID);
        return(0);
    case 8: //MAST ONLY
        ASSERT(appID == USBMGR_MAST_APPID);
        return(0);
	/*mischecked by coverity*/
	/*coverity[unterminated_case]*/
    case 3: //ICAT & MODEM
        switch(appID)
        {
        case USBMGR_MODEM_APPID:
            return(0);
        case USBMGR_ICAT_APPID:
            return(1);
        default:
            ASSERT(0);
        };
	/*mischecked by coverity*/
	/*coverity[unterminated_case]*/
    case 7: //ICAT & MODEM & GENIE
        switch(appID)
        {
        case USBMGR_MODEM_APPID:
            return(0);
        case USBMGR_ICAT_APPID:
            return(1);
        case USBMGR_GENIE_APPID:
            return(2);
        default:
            ASSERT(0);
        };

	/*mischecked by coverity*/
	/*coverity[unterminated_case]*/
    case 15: //ICAT & MODEM & GENIE & MAST
        switch(appID)
        {
        case USBMGR_MODEM_APPID:
            return(0);
        case USBMGR_ICAT_APPID:
            return(1);
        case USBMGR_GENIE_APPID:
            return(2);
        case USBMGR_MAST_APPID:
            return(3);
        default:
            ASSERT(0);
        };
	/*mischecked by coverity*/
	/*coverity[unterminated_case]*/
	case 12://GENIE & MAST

		  switch(appID)
        {
        case USBMGR_GENIE_APPID:
            return(1);
        case USBMGR_MAST_APPID:
            return(0);
        default:
            ASSERT(0);
        };

	/*mischecked by coverity*/
	/*coverity[unterminated_case]*/
    case 10://MODEM  & MAST

		  switch(appID)
        {
        case USBMGR_MODEM_APPID:
            return(0);
        case USBMGR_MAST_APPID:
            return(1);
        default:
            ASSERT(0);
        };
	/*mischecked by coverity*/
	/*coverity[unterminated_case]*/
	 case 5: //ICAT &  GENIE
        switch(appID)
        {
         case USBMGR_ICAT_APPID:
            return(0);
        case USBMGR_GENIE_APPID:
            return(1);
        default:
            ASSERT(0);
        };
	default: /* added to fix warning, Alla said to add it */
		ASSERT(0);
		return(0);
    };

}


//USB endpoints HW configuration functions
/************************************************************************
* Function:  USBMgrGetHWCfgEPMaxPacketSize
*************************************************************************
* Description: returns EP HW FIFO size
*
* Parameters: ep physical number
*
* Return value:
*
* Notes:
**********************************************************************/
UINT16 USBMgrGetHWCfgEPMaxPacketSize(USBDevice_EndpointE endpoint)
{
    return((USBDeviceEndpointGetHWCfg(endpoint))->usbMaxPacketSize);

}

/************************************************************************
* Function:  USBMgrGetHWCfgEPType
*************************************************************************
* Description: returns EP HW configured type
*
* Parameters: ep physical number
*
* Return value:
*
* Notes:
**********************************************************************/
USBMgr_EPType USBMgrGetHWCfgEPType(USBDevice_EndpointE endpoint)
{
    return (USBMgr_EPType)((USBDeviceEndpointGetHWCfg(endpoint))->usbEndpointType);
}

/************************************************************************
* Function:  USBMgrGetHWCfgEPDirection
*************************************************************************
* Description: returns EP HW configured direction
*
* Parameters: ep physical number
*
* Return value:
*
* Notes:
**********************************************************************/
USBMgr_EPDir USBMgrGetHWCfgEPDirection(USBDevice_EndpointE endpoint)
{
    return (USBMgr_EPDir)((USBDeviceEndpointGetHWCfg(endpoint))->usbEndpointDir);
}
#else
/**********************************************************************/
void USBMgrTempFuncUpdateHardCodedUSBDescriptor(UINT32 usb_app_mask)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

//ICAT EXPORTED FUNCTION - USB,USBMgr,Plugin
void USBMgrDevicePlugIn(void)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

//ICAT EXPORTED FUNCTION - USB,USBMgr,Unplug
void USBMgrDeviceUnplug(void)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

/************************************************************************
* Function: USBMgrRegister
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrRegister(UINT8 numEndpoint, USBMgr_EndpointStruct *endpointCfg, USBMgr_DeviceClassStruct *deviceClass,
                         CHAR *interfaceString, UINT8 appID, USBMgr_CallbackIndicationStruct *indicationCallback, USBMGR_IF_HANDLER *interfaceHnd)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return USBMGR_RC_OK;
}

//USB endpoints HW configuration functions
/************************************************************************
* Function:  USBMgrGetHWCfgEPMaxPacketSize
*************************************************************************
* Description: returns EP HW FIFO size
*
* Parameters: ep physical number
*
* Return value:
*
* Notes:
**********************************************************************/
UINT16 USBMgrGetHWCfgEPMaxPacketSize(USBDevice_EndpointE endpoint)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return((USBDeviceEndpointGetHWCfg(endpoint))->usbMaxPacketSize);

}

/************************************************************************
* Function:  USBMgrRecConfirmation
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes: every application must call rx confirmation
**********************************************************************/
void USBMgrRxConfirmation(USBDevice_EndpointE endpoint)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

/************************************************************************
* Function:  USBMgrRxConfirmationExt
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:  every application must call rx confirmation
**********************************************************************/
void USBMgrRxConfirmationExt(USBDevice_EndpointE endpoint ,UINT32 next_rx_packet_length, BOOL expect_zlp)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

/************************************************************************
* Function:  USBMgrRecConfirmation
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
**********************************************************************/
void USBMgrSetupCommandRsp(UINT8 *setupPacket ,UINT16 setupLen )
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

/************************************************************************
* Function: USBMgrTransmit
*************************************************************************
* Description:
*
* Parameters:
*
*
*
* Return value: void
*
* Notes:
************************************************************************/
USBMgr_RC USBMgrTransmit(USBMGR_IF_HANDLER interfaceHnd,UINT8 endpoint/*index at User table*/, UINT8 *dataBuffer , UINT32 dataLen,BOOL endOfMessage/*,UINT32 cooky*/ )
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return USBMGR_RC_OK;
}


/************************************************************************
* Function: USBMgrEndpointStall
*************************************************************************
* Description:
*
* Parameters:
*
*
*
* Return value: void
*
* Notes: stall endpoint, can only be called from task, blocked, takes ~2milli to enter stall
************************************************************************/
USBMgr_RC   USBMgrEndpointStall(USBMGR_IF_HANDLER interfaceHnd,UINT8 endpoint)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return(USBMGR_RC_OK);
}

/************************************************************************
* Function: USBMgrFlushRxBuffer
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrFlushRxBuffer ( USBMGR_IF_HANDLER interfaceHnd ,UINT8 endpoint/*index at User table*/)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

/************************************************************************
* Function: USBMgrFlushTxQueue
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
void USBMgrFlushTxQueue( USBMGR_IF_HANDLER interfaceHnd ,UINT8 endpoint/*index at User table-indicate the relative location of specific endpoint*/)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return;
}

/************************************************************************
* Function:  UsbMgrGetIfIndexByAppId
*************************************************************************
* Description:
*
* Parameters:
*
* Return value:
*
* Notes:
**********************************************************************/
UINT8 UsbMgrGetIfIndexByAppId(UINT8 appID)
{
    ErrorLogPrintf("Warning: %s", __FUNCTION__);
    return 0;
}


/* It is for temp use. */
int M_KiOsGetSignalInt(void)
{
    return 0;
}
#endif
