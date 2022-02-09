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
 * Title:        USB DEVICE - USB Device API header file
 *
 * Filename:     usb_device_def.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains definitions/macros that needed by
 *               the USB Device module
 *
 * Notes:
 ******************************************************************************/

#if !defined(_USB_DEVICE_DEF_H_)
#define      _USB_DEVICE_DEF_H_

#define USB_DEVICE_EP0_GET_TRANSACTION_DIR(bmRequestType)                               \
(( (bmRequestType & USB_DATA_STAGE_DIRECTION_MASK) == USB_DATA_STAGE_H2D_MASK) ?        \
            USBCDevice_DataStageDirectionH2D :USBCDevice_DataStageDirectionD2H)
#define USB_DEVICE_REQ_TYPE_SHIFT                   (5)
#define USB_DEVICE_REQ_TYPE_MASK                    (0x3 << USB_DEVICE_REQ_TYPE_SHIFT) /* use to identify a Request Type */
#define USB_DEVICE_GET_REQ_TYPE(bmREQUESTtYPE)      (((bmREQUESTtYPE) & USB_DEVICE_REQ_TYPE_MASK) >> USB_DEVICE_REQ_TYPE_SHIFT)

#define USB_DEVICE_DMA_BASE_PERIPHERAL              (DMA_USB_ENDPOINT_0)
#define USB_DEVICE_GET_DMA_PERIPHERAL(eNDPOINT)     (USB_DEVICE_DMA_BASE_PERIPHERAL + (eNDPOINT))

#define USB_DEVICE_MAX_LENGTH(lENGTH_1,lENGTH_2)    ((lENGTH_1 < lENGTH_2) ? lENGTH_1 : lENGTH_2)

#define INTEL_VENDOR_ID             (0x8086)
#define INTEL_VENDOR_ID_LSB         (INTEL_VENDOR_ID&0x00FF)
#define INTEL_VENDOR_ID_MSB         (INTEL_VENDOR_ID>>8)

#define TTPCOM_VENDOR_ID            (0x0AEB)
#define TTPCOM_VENDOR_ID_LSB        (TTPCOM_VENDOR_ID&0x00FF)
#define TTPCOM_VENDOR_ID_MSB        (TTPCOM_VENDOR_ID>>8)

#define USB_DEVICE_ICAT_CLASS       (0xFF)
#define USB_DEVICE_EMMI_CLASS       (0xFF)
#define USB_DEVICE_COMMS_CLASS      (0x02)
#define USB_DEVICE_MAST_CLASS       (0xFF)

#define USB_DEVICE_ICAT_SUB_CLASS   (0xFF)
#define USB_DEVICE_EMMI_SUB_CLASS   (0x01)
#define USB_DEVICE_COMMS_SUB_CLASS  (0xFF)
#define USB_DEVICE_MAST_SUB_CLASS   (0xFF)

#define USB_DEVICE_ICAT_PROTOCOL    (0xFF)
#define USB_DEVICE_EMMI_PROTOCOL    (0x01)
#define USB_DEVICE_COMMS_PROTOCOL   (0xFF)
#define USB_DEVICE_MAST_PROTOCOL    (0xFF)
#endif /*_USB_DEVICE_DEF_H_*/

