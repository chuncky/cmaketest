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
 * Title:        USB DEF - general USB definitions
 *
 * Filename:     usb_def.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains definitions/macros that needed by USB
 *
 * Notes:
 ******************************************************************************/

#if !defined(_USB_DEF_H_)
#define      _USB_DEF_H_

//#include "global_types.h"
//#include "utils.h"
#include "common.h"

#define UDC_EP0_CTRL_TRANSFER_SETUP_PACKET_SIZE     (8)
#define USB_SETUP_CMD_LENGTH                        (8)

#define UDC2_EP0_CTRL_TRANSFER_SETUP_PACKET_SIZE    (16)


#define USB_LANG_ID_LENGTH                      (4)
#define USB_LANG_LIST_REQ                       (0)
#define USB_LANG_ENGLISH_ID                     (0x0409)

#define USB_DATA_STAGE_DIRECTION_MASK           (0x80)
#define USB_DATA_STAGE_H2D_MASK                 (0x00)
#define USB_DATA_STAGE_D2H_MAASK                (0x80)

#define GET_8BIT_LSB(vAR16bIT)                  ((UINT8)(((vAR16bIT)     ) & 0xFF))
#define GET_8BIT_MSB(vAR16bIT)                  ((UINT8)(((vAR16bIT) >> 8) & 0xFF))

/* USB request types */
typedef enum
{
    USB_REQ_TYPE_STANDARD            = 0x00,
    USB_REQ_TYPE_CLASS               = 0x01,
    USB_REQ_TYPE_VENDOR              = 0x02,
    USB_REQ_TYPE_RESERVED            = 0x03
}USB_ReqTypesE;

/* Enumerate Standard Request types value */
typedef enum
{
    USB_STANDARD_REQ_GET_STATUS_ID           = 0,
    USB_STANDARD_REQ_CLEAR_FEATURE_ID        = 1,
    USB_STANDARD_REQ_RESERVED_1              = 2,
    USB_STANDARD_REQ_SET_FEATURE_ID          = 3,
    USB_STANDARD_REQ_RESERVED_2              = 4,
    USB_STANDARD_REQ_SET_ADDRESS_ID          = 5,
    USB_STANDARD_REQ_GET_DESCRIPTOR_ID       = 6,
    USB_STANDARD_REQ_SET_DESCRIPTOR_ID       = 7,
    USB_STANDARD_REQ_GET_CONFIGURATION_ID    = 8,
    USB_STANDARD_REQ_SET_CONFIGURATION_ID    = 9,
    USB_STANDARD_REQ_GET_INTERFACE_ID        = 10,
    USB_STANDARD_REQ_SET_INTERFACE_ID        = 11,
    USB_STANDARD_REQ_SYNC_FRAME_ID           = 12
}USB_StandardReqE;


typedef enum
{
    USB_DESCRIPTOR_TYPE_DEVICE              = 1,
    USB_DESCRIPTOR_TYPE_CONFIGURATION       = 2,
    USB_DESCRIPTOR_TYPE_STRING              = 3,
    USB_DESCRIPTOR_TYPE_INTERFACE           = 4,
    USB_DESCRIPTOR_TYPE_ENDPOINT            = 5,
    USB_DESCRIPTOR_TYPE_QUALIFIER           = 6,       /* Starting from here - USB2.0 Addition */
    USB_DESCRIPTOR_TYPE_SPEED               = 7,
    USB_DESCRIPTOR_TYPE_POWER               = 8,
    USB_DESCRIPTOR_TYPE_OTG                 = 9
}USB_DescriptorTypesE;

typedef __packed struct
{
    UINT8                   bmRequestType;
    UINT8                   bRequest;
    UINT16                  wValue;
    UINT16                  wIndex;
    UINT16                  wLength;
    UINT8                   *p_data;
}USB_SetupCmdS;




#define USB_DEVICE_ASSERT(cOND)     {ASSERT(cOND)}

#endif /*_USB_DEF_H_*/
