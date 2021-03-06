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
* Filename: USB_cable.h                                              *
*                                                                    *
* Target, platform: Common Platform, SW platform                     *
*                                                                    *
* Author: Yossi Gabay                                                *
*                                                                    *
* Description: This header file handles CABLE DETECTION using GPIO   *
*                                                                    *
*                                                                    */


#ifndef _USB_CABLE_H_
#define _USB_CABLE_H_

//#include "global_types.h"
#include "common.h"
typedef enum
{
    USB_CABLE_OUT = 0,
    USB_CABLE_IN,
	USB_CABLE_UNKNOWN
}USBCable_StatusE;

/* Cable detection notify Function
 * USB_CableStatus - cable status */
typedef void (*fpUSBDeviceCableDetectionNotify)(USBCable_StatusE );
extern fpUSBDeviceCableDetectionNotify  USBDeviceCableDetectionNotify;

//API's
void                USBCableDetectionInit(void);
USBCable_StatusE    USBCableStatusRead(void);

//Call out function
extern void         USBCableDetectionNotify(USBCable_StatusE cableStatus);

//Management
UINT32              USBCableDetectionRegister(fpUSBDeviceCableDetectionNotify notifyFn);
void                USBCableDetectionUnRegister(UINT32 usbHandle);

typedef void (*UsbWkpCallback)(void);
void USB_WAKEUP_CALLBACK_REGISTER(UsbWkpCallback handler);

#endif /*_USB_CABLE_H_*/
