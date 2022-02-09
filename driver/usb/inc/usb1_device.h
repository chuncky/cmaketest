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
 * Title:        USB DEVICE - USB Device API
 *
 * Filename:     udc_device.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This is the header file for the USB Device Stack & API
 *
 * Notes:
 ******************************************************************************/

#if !defined(_USB_DEVICE_H_)
#define      _USB_DEVICE_H_

#include "common.h"
//#include "global_types.h"
#include "usb_def.h"
#include "usb_device.h"
#include "usb_cable.h"

/*******************************************************************************
 *                      A P I ' S
 ******************************************************************************/

/* General Transmit/Receive services */
USBCDevice_ReturnCodeE  USB1DeviceEndpointMultiTransmit(USBDevice_EndpointE              endpoint,
                                                       UINT32                           numOfBuffers,
                                                       USBDevice_MultiTransmitListS     *pMultiList);

USBCDevice_ReturnCodeE  USB1DeviceEndpointTransmit(USBDevice_EndpointE       endpoint,
                                                  UINT8                     *pTxBuffer,
                                                  UINT32                    txLength,
                                                  BOOL                      autoZLP);

USBCDevice_ReturnCodeE  USB1DeviceEndpointReceiveCompleted(USBDevice_EndpointE          endpoint);
USBCDevice_ReturnCodeE USB1DeviceEndpointReceiveCompletedExt(USBDevice_EndpointE   endpoint,
                                                            UINT8                *pRxBuffer,
                                                            UINT32                next_packet_expected_length,
                                                            BOOL                  expect_zlp);

USBCDevice_ReturnCodeE  USB1DeviceEndpointReceive(USBDevice_EndpointE        endpoint,
                                                 UINT8                      *pRxBuffer,
                                                 UINT32                     length,
                                                 BOOL                       expect_zlp);

/* Status/Endpoint0 services */
void                    USB1DeviceVendorClassResponse(UINT8                          *pBuffer,
                                                     UINT16                         bufferLength);

/* Configuration/System services */
USBCDevice_ReturnCodeE  USB1DeviceEndpointAbort(USBDevice_EndpointE      endpoint);

USBCDevice_ReturnCodeE  USB1DeviceEndpointClose(USBDevice_EndpointE      endpoint);

USBCDevice_ReturnCodeE  USB1DeviceEndpointOpen(USBDevice_EndpointE                       endpoint,
                                              USBDevice_UsageTypeE                      usageType,
                                              UINT8                                     dmaChannel,
                                              UINT8                                     *pDescBuffer,
                                              UINT16                                    descBufferLength,
                                              USBDeviceTransactionCompletedNotifyFn     transactionCompletedNotifyFn);

USBCDevice_ReturnCodeE         USB1DeviceEndpointStall(USBDevice_EndpointE endpoint);
/*USBCDevice_ReturnCodeE         USBDeviceEndpointClearStall(USBDevice_EndpointE endpoint);*/

USBCDevice_ReturnCodeE  USB1DeviceRegister(USBDeviceStatusNotifyFn                     statusNotifyFn,
                                          USBDeviceVendorClassRequestNotifyFn         vendorClassRequestNotifyFn,
                                          USBDeviceEndpointZeroNotifyFn               endpointZeroNotifyFn);

USBCDevice_ReturnCodeE USB1DeviceRegisterPatchTempFunc(USBDeviceStatusNotifyFn                 statusNotifyFn,
                                         USBDeviceVendorClassRequestNotifyFn     vendorClassRequestNotifyFn,
                                         USBDeviceEndpointZeroNotifyFn           endpointZeroNotifyFn);

USBDevice_StatusE       USB1DeviceStatusGet(void);

void                    USB1DevicePhase2Init(void);
void                    USB1DevicePhase1Init(void);

void                    USB1DeviceSimulatePlug(USBD_SimulatePlugE plug_operation);

void                    USB1DeviceSetDescriptor( USB_DescriptorTypesE descType,
                                               UINT8 *pDesc,
                                               UINT8 descLength,
                                               UINT8 id);

BOOL                    USB1DeviceIsControllerEnabled (void);

void USB1DeviceCableDetectionNotify(USBCable_StatusE cableStatus);


//using struct tag to avoid include
struct UDC_EndpointConfigS_tag *     USB1DeviceEndpointGetHWCfg(USBDevice_EndpointE endpoint);






#endif /*_USB_DEVICE_H_*/
