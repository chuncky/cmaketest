/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*******************************************************************************
 *                      M O D U L E     B O D Y
 *******************************************************************************
 *  COPYRIGHT (C) 2003, 2004 Intel Corporation.
 *
 *  This software as well as the software described in it is furnished under
 *  license and may only be used or copied in accordance with the terms of the
 *  license. The information in this file is furnished for informational use
 *  only, is subject to change without notice, and should not be construed as
 *  a commitment by Intel Corporation. Intel Corporation assumes no
 *  responsibility or liability for any errors or inaccuracies that may appear
 *  in this document or any software that may be provided in association with
 *  this document.
 *  Except as permitted by such license, no part of this document may be
 *  reproduced, stored in a retrieval system, or transmitted in any form or by
 *  any means without the express written consent of Intel Corporation.
 *
 *******************************************************************************
 *
 * Title:        UDC DRIVER - USB Device Controller header file
 *
 * Filename:     usb_config.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This is the configuration file for the UDC driver
 *
 * Notes:
 ******************************************************************************/

#if !defined(_USB_CONFIG_H_)
#define      _USB_CONFIG_H_
#include "common.h"
//#include "global_types.h"

/* Defined this to enable cable detection using GPIO */
#define USB_USE_GPIO_CABLE_DETECTION

/* Defines the maximum number of clients that can be reigstered for CABLE detection */
#define USB_MAX_CABLE_NOTIFY_REGISTRATIONS          (2)

/* Defines the maximum active endpoints that can work in parallel */
#define UDC_MAX_ACTIVE_ENDPOINTS                    (16)

//no diag available
#define USB_DEVICE_DIAG_ENABLED                          0

#if (USB_DEVICE_DIAG_ENABLED  ==    0)
#define DIAG_RAM_REPORT_FORMATTED_INDEX_2P(a,b,c)
#endif

//Hongji 201011
//#define USB_DEVICE_DMA_ENABLED                           1
#define USB_DEVICE_DMA_ENABLED                        0

#if (USB_DEVICE_DMA_ENABLED == 0)
#define DMA_MAX_CHANNEL_NUMBER                           0
#endif

//number of buffers after which the notification is sent
#define USB_DEVICE_MULTI_TRANSMIT_NOTIFY_AMOUNT        10

#define USB_PRODUCT_ID                                  0x4000


#define USB_EP0_MAX_RX_TRANSFER_SIZE                    128

#define SUPPORT_RESET_EVENT     1
#endif /*_USB_CONFIG_H_*/
