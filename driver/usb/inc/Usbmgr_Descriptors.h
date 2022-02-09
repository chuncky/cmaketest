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
* Title: Universal Serial Bus - USB                                  *
*                                                                    *
* Filename: USBMgr_descriptors.h                                     *
*                                                                    *
* Target, platform: Common Platform, SW platform                     *
*                                                                    *
* Author: Miriam Yovel                                               *
*                                                                    *
* Description: This is the DESCRIPTORS definition of the USBMgr      *
* pacakge                                                            *
*                                                                    *
* Notes:                                                             *
* ***************************************************************** */

#ifndef _USBMGR_DESCRIPTORS_H_
#define _USBMGR_DESCRIPTORS_H_

/*#include "xsusb.h" */

/*descriptor configuration file for current configuration & descriptors*/
//#include "usb_descriptors_config.h"
//#include "usb_descriptors.h"

/******definitions *********************/

#define DEVICE_DESC_LENGTH       18
#define DEVICE_DESC_USB_PROTOCOL 0x0110
#define DEVICE_DESC_CLASS        0
#define DEVICE_DESC_SUBCLASS     0
#define DEVICE_DESC_PROTOCOL     0
#define DEVICE_DESC_EP0_SIZE     UDC_EP0_FIFO_SIZE
#define DEVICE_DESC_BCD          0
#define DEVICE_DESC_MAN_STR      1
#define DEVICE_DESC_PRD_STR      2
#define DEVICE_DESC_SER_NO_STR   3
#define DEVICE_DESC_NO_STRING    0

#define DEVICE_DESC_INT_STR      3 // was  0
#define DEVICE_DESC_CFG_NO       1

#define CONFIG_DESC_H_LENGTH     9
#define CONFIG_DESC_VALUE        1
#define CONFIG_DESC_STR_IDX      0
#define CONFIG_DESC_ATTR         0xC0
#define CONFIG_DESC_MAX_POWER    0x32//50

#define INT_DESC_LENGTH          9
#define EP_DESC_LENGTH           7



typedef enum
{
    DEVICE_DESC_TYPE = 0x01,
    CONFIG_DESC_TYPE,
    STRING_DESC_TYPE,
    CONFIG_INT_DESC_TYPE,
    CONFIG_EP_DESC_TYPE
}USBMgrDescriptorType;

typedef enum
{
    EP_CONTROL_TYPE = 0x00,
    EP_ISO_TYPE,
    EP_BULK_TYPE,
    EP_INT_TYPE
}USBMgrEndpointType;





typedef __packed struct
{
    UINT8  devDescLength;
    UINT8  devDescType;
    UINT16 devDescUSBProtocol;
    UINT8  devDescClass;
    UINT8  devDescSubClass;
    UINT8  devDescProtocol;
    UINT8  devDescEP0MaxSize;
    UINT16 devDescVendorID;
    UINT16 devDescPrdID;
    UINT16 devDescBCD;
    UINT8  devDescManStrIdx;
    UINT8  devDescPrdStrIdx;
    UINT8  devDescSerNoStr;
    UINT8  devDescCfgNo;
	UINT8  pad[2];
}USBMgrDevDescStruct;


typedef __packed struct
{
    UINT8  cfgDescHLength;
    UINT8  cfgDescType;
    UINT16 cfgDescTotalLength;
    UINT8  cfgDescInterfaceNo;
    UINT8  cfgDescValue;
    UINT8  cfgDescStrIdx;
    UINT8  cfgDescAttr;
    UINT8  cfgDescMaxPower;
	UINT8  pad[3];
}USBMgrCfgDescHeaderStruct;

typedef __packed struct
{
    UINT8  cfgDescIntLength;
    UINT8  cfgDescIntType;
    UINT8  cfgDescIntIdxNo;
    UINT8  cfgDescAltSetting;
    UINT8  cfgDescEndpointNo;
    UINT8  cfgDescIntClass;
    UINT8  cfgDescIntSubClass;
    UINT8  cfgDescIntProtocol;
    UINT8  cfgDescIntStr;
	UINT8  pad[3];
}USBMgrCfgDescIntStruct;

typedef __packed struct
{
    UINT8  cfgDescEpLength;
    UINT8  cfgDescEpType;
    UINT8  cfgDescEpAdd;
    UINT8  cfgDescEpTransferType;
    UINT16 cfgDescEpMaxSize;
    UINT8  cfgDescEpPollInter;
	UINT8  pad;
}USBMgrCfgDescEpStruct;

typedef __packed struct
{
    UINT8  qualDescLength;
    UINT8  qualDescType;
    UINT16 qualDescBCD;
    UINT8  qualDescDevClass;
    UINT8  qualDescDevSubClass;
    UINT8  qualDescDevProtocol;
    UINT8  qualDescMaxPcktSize;
    UINT8  qualDescNumCfg;
    UINT8  qualDescRsrv;
    UINT8  pad[2];
}USBMgrQualifierDescStruct;

typedef __packed struct
{
    UINT8  speedDescLength;
    UINT8  speedDescType;
    UINT16 speedDescTotalLength;
    UINT8  speedDescNumInterfaces;
    UINT8  speedDescConfigValue;
    UINT8  speedDescIConfiguration;
    UINT8  speedDescBmAttributes;
    UINT8  speedDescMaxPower;
    UINT8  pad[3];
}USBMgrOtherSpeedDescStruct;

#endif /*_USBMGR_DESCRIPTORS_H_*/
