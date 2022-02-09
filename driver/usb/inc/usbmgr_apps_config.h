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

#ifndef _USBMGR_APPS_CONFIG_H_
#define _USBMGR_APPS_CONFIG_H_

#include "usbmgr_apps_def.h"
#include "usb_device.h"


#define   USBMGR_USE_TTPCOM_APPS

#define   USBMGR_INTEL_VID                  0x8086
#define   USBMGR_TTPCOM_VID                 0x0AEB
#define   USBMGR_DEMO_VID                   0x0001


#if defined(USBMGR_USE_INTEL_APPS)

#define   USBMGR_CUST_BASE_PRODOUCT_ID      0x4000
#define   USBMGR_CUST_VID                   USBMGR_INTEL_VID

#define   USBMGR_STRING_MANUFACTURER        "Intel USB"
#define   USBMGR_STRING_PRODUCT             "Intel GSM Ref Design"
#define   USBMGR_STRING_SERIAL_NUMBER       ""

#define   USBMGR_ICAT_APPID                 USBMGR_APP_ID_1
#define   USBMGR_TEST_APPID                 USBMGR_APP_ID_2
#define   USBMGR_DEMO_APPID_3               USBMGR_APP_ID_3
#define   USBMGR_DEMO_APPID_4               USBMGR_APP_ID_4
#define   USBMGR_DEMO_APPID_5               USBMGR_APP_ID_5
#define   USBMGR_DEMO_APPID_6               USBMGR_APP_ID_6
#define   USBMGR_DEMO_APPID_7               USBMGR_APP_ID_7
#define   USBMGR_DEMO_APPID_8               USBMGR_APP_ID_8

#elif defined(USBMGR_USE_TTPCOM_APPS)

#if defined(USBMGR_USE_TTPCOM_APPS_DEBUG)
#define   USBMGR_CUST_BASE_PRODOUCT_ID      0x4000
#define   USBMGR_CUST_VID                   USBMGR_INTEL_VID

#else  /*of USBMGR_USE_TTPCOM_APPS_DEBUG*/

#define   USBMGR_CUST_BASE_PRODOUCT_ID      0x5000
#define   USBMGR_CUST_VID                   USBMGR_TTPCOM_VID

#endif

#define   USBMGR_STRING_MANUFACTURER        "TTPcom USB"
#define   USBMGR_STRING_PRODUCT             "Intel GSM Ref Design"
#define   USBMGR_STRING_SERIAL_NUMBER       ""


//alla - TTP applications Ids , should not be changed, used in ttpal,and ps_init
//!! should not be changed !!!
#define   USBMGR_ICAT_APPID                 USBMGR_APP_ID_Bit1
#define   USBMGR_MODEM_APPID                USBMGR_APP_ID_Bit2
#define   USBMGR_GENIE_APPID                USBMGR_APP_ID_Bit3
#define   USBMGR_MAST_APPID                 USBMGR_APP_ID_Bit4
#define   USBMGR_TTPAL_APPID_LAST           USBMGR_MAST_APPID
//


//PHYSYCAL Endpoints numbers definition
#define USB_MAST_IN_ENDPOINT_PHY          	    USB_DEVICE_ENDPOINT_3
#define USB_MAST_OUT_ENDPOINT_PHY  			    USB_DEVICE_ENDPOINT_4

#define USB_EMMI_IN_ENDPOINT_PHY          		USB_DEVICE_ENDPOINT_9
#define USB_EMMI_OUT_ENDPOINT_PHY         		USB_DEVICE_ENDPOINT_10

#define USB_ICAT_IN_ENDPOINT_PHY          	    USB_DEVICE_ENDPOINT_7
#define USB_ICAT_OUT_ENDPOINT_PHY  			    USB_DEVICE_ENDPOINT_8


#define USB_COMM_CTRL_IN_ENDPOINT_PHY           USB_DEVICE_ENDPOINT_5
#define USB_COMM_DATA_IN_ENDPOINT_PHY     		USB_DEVICE_ENDPOINT_1
#define USB_COMM_DATA_OUT_ENDPOINT_PHY    		USB_DEVICE_ENDPOINT_2


#elif defined(USBMGR_USE_CUST_APPS)

#define   USBMGR_CUST_BASE_PRODOUCT_ID      0x6000
#define   USBMGR_CUST_VID                   USBMGR_DEMO_VID

#define   USBMGR_STRING_MANUFACTURER        "Cust Name"
#define   USBMGR_STRING_PRODUCT             "Cust Product"
#define   USBMGR_STRING_SERIAL_NUMBER       "Cust SN"

#define   USBMGR_DEMO_APPID_1               USBMGR_APP_ID_1
#define   USBMGR_DEMO_APPID_2               USBMGR_APP_ID_2
#define   USBMGR_DEMO_APPID_3               USBMGR_APP_ID_3
#define   USBMGR_DEMO_APPID_4               USBMGR_APP_ID_4
#define   USBMGR_DEMO_APPID_5               USBMGR_APP_ID_5
#define   USBMGR_DEMO_APPID_6               USBMGR_APP_ID_6
#define   USBMGR_DEMO_APPID_7               USBMGR_APP_ID_7
#define   USBMGR_DEMO_APPID_8               USBMGR_APP_ID_8





#endif /*apps*/

#endif /*_USBMGR_APPS_CONFIG_H_*/
