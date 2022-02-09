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

#ifndef _USBMGR_APPS_DEF_H_
#define _USBMGR_APPS_DEF_H_

//#include "global_types.h"
#include "common.h"


typedef enum
{
    USBMGR_APP_ID_Bit1 = 1,
    USBMGR_APP_ID_Bit2 = 2,
    USBMGR_APP_ID_Bit3 = 4,
    USBMGR_APP_ID_Bit4 = 8,
    USBMGR_APP_ID_Bit5 = 16,
    USBMGR_APP_ID_Bit6 = 32,
    USBMGR_APP_ID_Bit7 = 64,
    USBMGR_APP_ID_Bit8 = 128
}USBMgrAppID;

#endif
