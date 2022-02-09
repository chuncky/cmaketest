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

#if !defined(_UDC_DEVICE_H_)
#define      _UDC_DEVICE_H_
#include "common.h"
//#include "global_types.h"
#include "usb_def.h"

/*******************************************************************************
 *                      T Y P E S
 ******************************************************************************/

/* Available endpoints in configuration - total is 15 */
typedef enum
{
    USB_DEVICE_ENDPOINT_0 = 0, /* Control Endpoint */
    USB_DEVICE_ENDPOINT_1 = 1,
    USB_DEVICE_ENDPOINT_2,
    USB_DEVICE_ENDPOINT_3,
    USB_DEVICE_ENDPOINT_4,
    USB_DEVICE_ENDPOINT_5,
    USB_DEVICE_ENDPOINT_6,
    USB_DEVICE_ENDPOINT_7,
    USB_DEVICE_ENDPOINT_8,
    USB_DEVICE_ENDPOINT_9,
    USB_DEVICE_ENDPOINT_10,
    USB_DEVICE_ENDPOINT_11,
    USB_DEVICE_ENDPOINT_12,
    USB_DEVICE_ENDPOINT_13,
    USB_DEVICE_ENDPOINT_14,
    USB_DEVICE_ENDPOINT_15,
    USB_DEVICE_TOTAL_ENDPOINTS
}USBDevice_EndpointE;

/* Device status */
typedef enum
{
    USB_DEVICE_STATUS_NOT_CONNECTED = 0,
    USB_DEVICE_STATUS_RESET,
    USB_DEVICE_STATUS_ENUM_IN_PROCESS,
    USB_DEVICE_STATUS_CONNECTED,
    USB_DEVICE_STATUS_SUSPEND   //YG - does the user need this?
}USBDevice_StatusE;

/* Device status */
typedef enum
{
    USBD_SimulatePlugE_IN = 0,
    USBD_SimulatePlugE_OUT = 1,
    USBD_SimulatePlugE_OUT_AND_IN=2
}USBD_SimulatePlugE;

/* Endpoint usage type */
typedef enum
{
    USB_DEVICE_NO_NEED_ZLP      = 0,
    USB_DEVICE_NEED_ZLP         = 2,
    USB_DEVICE_USAGE_INTERRUPT  = 4,
    USB_DEVICE_USAGE_DMA        = 8
}USBDevice_UsageTypeE;

typedef enum
{
    USBCDevice_DataStageDirectionH2D = 0, //OUT
    USBCDevice_DataStageDirectionD2H      //IN
}USBCDevice_DataStageDirectionE;

/* Multi-Transmit list */
typedef struct
{
    CHAR   *pTxBuff;
    UINT32 bufferLength;
}USBDevice_MultiTransmitListS;

/* Return codes */
typedef enum
{
    USB_DEVICE_RC_OK = 1,
    USB_DEVICE_RC_ERROR = -100,
    USB_DEVICE_RC_NOT_CONNECTED,
    USB_DEVICE_RC_ENDPOINT_IN_USE,
    USB_DEVICE_RC_ENDPOINT_NOT_OPENED,
    USB_DEVICE_RC_ENDPOINT_BUSY,
    USB_DEVICE_RC_ENDPOINT_STALLED,
    USB_DEVICE_RC_ENDPOINT_NOT_IN_CONFIG,
    USB_DEVICE_RC_TRANSFER_ABORTED,
    USB_DEVICE_RC_OPEN_ERROR,
    USB_DEVICE_RC_BUFFER_ERROR,
    USB_DEVICE_RC_DMA_ERROR
}USBCDevice_ReturnCodeE;

/* USB2.0 Additions Begin */
typedef enum
{
    USB_VER_1_1,
    USB_VER_2_0
}USBDevice_USBVersionE;

typedef enum
{
    USB_SPEED_LS,
    USB_SPEED_FS,
    USB_SPEED_HS
}USBDevice_USBSpeedE;

typedef enum
{
	AT_MODEM_SINGLE	= 1,
	AT_MODEM_DUAL	= 2,
}AT_MODEM_COUNT;


/* USB2.0 Additions end */

/*******************************************************************************
 *                      C A L L B A C K   F U N C T I O N S
 ******************************************************************************/

/* This is a general transaction completed notify function. It is used for
 * 3 different notifications. Each parameter may slightly changes it's function
 * according to the notification type:
 *
 * USBDevice_EndpointE  - endpoint: common to all notifications
 *
 * Transmit Completion Notification:
 * UINT8 *      - pointer to transmitted buffer
 * UINT32       - length of transmitted buffer
 * BOOL         - not in use (dummy)
 *
 * Data Received Notification:
 * UINT8 *      - pointer to received buffer
 * UINT32       - length of received buffer
 * BOOL         - End-Of-Message
 *
 * Multi Transmit Completion Notification:
 * UINT8 *      - NULL (not in use)
 * UINT32       - number of items transmitted in chunk
 * BOOL         - End of Multi List
 */
typedef void (*USBDeviceTransactionCompletedNotifyFn)(USBDevice_EndpointE,
                                                      UINT8 *,
                                                      UINT32,
BOOL);

/* Notify when the USB Device status changes:
 * USBDevice_StatusE    - new USB device status
 */
typedef void (*USBDeviceStatusNotifyFn)(USBDevice_StatusE);

/* Notify when Vendor or Class Request received from the host (both directions):
 * USB_SetupCmdS *                  - pointer to the SETUP command
 * USBCDevice_DataStageDirection    - data stage direction (0 if no data stage)
 * UINT32                          - data stage length (0 if no data stage)
 */
typedef void (*USBDeviceVendorClassRequestNotifyFn)(USB_SetupCmdS *);

/* Notify when Endpoint ZERO finished it's last operation requested (both directions):
 * USBCDevice_DataStageDirection    - data stage direction
 * UINT8 *                          - pointer to buffer
 * UINT32                           - data stage length
 */
typedef void (*USBDeviceEndpointZeroNotifyFn)(USB_SetupCmdS *);

/*******************************************************************************
 *                      A P I ' S
 ******************************************************************************/

USBDevice_USBVersionE USBDeviceGetUSBVersionInUse(void);
USBDevice_USBSpeedE USBDeviceGetUSBSpeedInUse(void);

/* General Transmit/Receive services */
typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointMultiTransmit)(USBDevice_EndpointE endpoint,
                                                       UINT32                           numOfBuffers,
                                                       USBDevice_MultiTransmitListS     *pMultiList);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointTransmit)(USBDevice_EndpointE       endpoint,
                                                  UINT8                     *pTxBuffer,
                                                  UINT32                    txLength,
                                                  BOOL                      autoZLP);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointCancelTransmit)(USBDevice_EndpointE      endpoint);
typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointCancelReceive)(USBDevice_EndpointE      endpoint);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointReceiveCompleted)(USBDevice_EndpointE          endpoint);
typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointReceiveCompletedExt)(USBDevice_EndpointE   endpoint,
                                                            UINT8                *pRxBuffer,
                                                            UINT32                next_packet_expected_length,
                                                            BOOL                  expect_zlp);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointReceive)(USBDevice_EndpointE        endpoint,
                                                 UINT8                      *pRxBuffer,
                                                 UINT32                     length,
                                                 BOOL                       expect_zlp);

/* Status/Endpoint0 services */
typedef void                    (*fpUSBDeviceVendorClassResponse)(UINT8             *pBuffer,
                                                     UINT16                         bufferLength);

/* Configuration/System services */
typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointAbort)(USBDevice_EndpointE      endpoint);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointClose)(USBDevice_EndpointE      endpoint);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointOpen)(USBDevice_EndpointE                       endpoint,
                                              USBDevice_UsageTypeE                      usageType,
                                              UINT8                                     dmaChannel,
                                              UINT8                                     *pDescBuffer,
                                              UINT16                                    descBufferLength,
                                              USBDeviceTransactionCompletedNotifyFn     transactionCompletedNotifyFn);

typedef USBCDevice_ReturnCodeE  (*fpUSBDeviceEndpointStall)(USBDevice_EndpointE endpoint);
/*USBCDevice_ReturnCodeE         USBDeviceEndpointClearStall(USBDevice_EndpointE endpoint);*/

USBCDevice_ReturnCodeE          USBDeviceRegister(USBDeviceStatusNotifyFn                     statusNotifyFn,
                                          USBDeviceVendorClassRequestNotifyFn         vendorClassRequestNotifyFn,
                                          USBDeviceEndpointZeroNotifyFn               endpointZeroNotifyFn);

USBCDevice_ReturnCodeE          USBDeviceRegisterPatchTempFunc(USBDeviceStatusNotifyFn                 statusNotifyFn,
                                         USBDeviceVendorClassRequestNotifyFn     vendorClassRequestNotifyFn,
                                         USBDeviceEndpointZeroNotifyFn           endpointZeroNotifyFn);



typedef void                    (*fpUSBDevicePhase2Init)(void);
void                            USBDevicePhase1Init(void);

void                            USBDeviceSimulatePlug(USBD_SimulatePlugE plug_operation);
void                            USBDeviceSetDescriptor( USB_DescriptorTypesE descType, UINT8 *pDesc, UINT16 descLength, UINT8 id);
typedef BOOL                    (*fpUSBDeviceIsControllerEnabled)(void);

//using struct tag to avoid include
typedef struct UDC_EndpointConfigS_tag *     (*fpUSBDeviceEndpointGetHWCfg)(USBDevice_EndpointE endpoint);

/* Function Pointers to simplify single interface both USB1.1 & USB2.0 using runtime selection */
extern fpUSBDevicePhase2Init            USBDevicePhase2Init;
extern fpUSBDeviceIsControllerEnabled   USBDeviceIsControllerEnabled;
extern fpUSBDeviceEndpointGetHWCfg      USBDeviceEndpointGetHWCfg;
extern fpUSBDeviceEndpointStall         USBDeviceEndpointStall;
extern fpUSBDeviceEndpointOpen          USBDeviceEndpointOpen;
extern fpUSBDeviceEndpointClose         USBDeviceEndpointClose;
extern fpUSBDeviceEndpointAbort			USBDeviceEndpointAbort;
extern fpUSBDeviceVendorClassResponse	USBDeviceVendorClassResponse;
extern fpUSBDeviceEndpointReceive		USBDeviceEndpointReceive;
extern fpUSBDeviceEndpointReceiveCompletedExt	USBDeviceEndpointReceiveCompletedExt;
extern fpUSBDeviceEndpointReceiveCompleted	USBDeviceEndpointReceiveCompleted;
extern fpUSBDeviceEndpointTransmit 		USBDeviceEndpointTransmit;
extern fpUSBDeviceEndpointCancelTransmit      USBDeviceEndpointCancelTransmit;
extern fpUSBDeviceEndpointCancelReceive      USBDeviceEndpointCancelReceive;

extern fpUSBDeviceEndpointMultiTransmit USBDeviceEndpointMultiTransmit;

#endif /*_UDC_DEVICE_H_*/
