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

#ifndef _USBMGR_CONFIG_H_
#define _USBMGR_CONFIG_H_

#include "common.h"
//#include "global_types.h"
#include "usb.h"
//Hongji 201011
//#include "dma.h"



#define USBMGR_STACK_SIZE                  0x1024
#define USBMGR_PRIORITY                    80
#define MIN_MSG_IN_MULTITRANSMIT           5
#define MAX_MSG_IN_MULTITRANSMIT           40
#define MAX_RX_BUFFER_SIZE                 4096


//FIFO sizes
#define UDC_EP0_FIFO_SIZE               16              // FIFO size of EP0 (control Endpoint)
#define USB_BULK_FIFO_SIZE              64
#define USB_ISO_FIFO_SIZE               256
#define USB_INT_FIFO_SIZE               8


#define USBMGR_BULK_IN1_ADDR           0x81
#define USBMGR_BULK_IN2_ADDR           0x86
#define USBMGR_BULK_OUT1_ADDR          0x02
#define USBMGR_BULK_OUT2_ADDR          0x07
#define USBMGR_ISO_IN_ADDR             0x00
#define USBMGR_ISO_OUT_ADDR            0x00
#define USBMGR_INT_IN_ADDR             0x00
#define USBMGR_INT_OUT_ADDR            0x00

#define USBMGR_BULK_IN_LENGTH           64
#define USBMGR_BULK_OUT_LENGTH          64
#define USBMGR_ISO_IN_LENGTH            256
#define USBMGR_ISO_OUT_LENGTH           256
#define USBMGR_INT_IN_LENGTH            16
#define USBMGR_INT_OUT_LENGTH           16

#define USBMGR_ISO_TYPE                 1
#define USBMGR_BULK_TYPE                2
#define USBMGR_INT_TYPE                 3



#define USBMGR_MULTI_TX_DESCRIPTOR_SIZE                USB_CALC_MULTI_TX_ALIGNED_DESC_BUF_SIZE(MAX_MSG_IN_MULTITRANSMIT)
#define USBMGR_TX_DESCRIPTOR_SIZE(Endpoint)            (USB_CALC_ALIGNED_DESC_BUF_SIZE((Endpoint), 0) )
#define USBMGR_RX_DESCRIPTOR_SIZE(Endpoint)            (USB_CALC_ALIGNED_DESC_BUF_SIZE((Endpoint), MAX_RX_BUFFER_SIZE))



#endif
