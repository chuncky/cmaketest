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
 * Filename:     udc_driver.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This is the header file for UDC hardware in ManitobaEP & Hermon
 *
 * Notes:
 ******************************************************************************/

#if !defined(_UDC_DRIVER_H_)
#define      _UDC_DRIVER_H_
#include "common.h"
//#include "global_types.h"
#include "usb_device.h"

/* type definition for UDC_HANDLE */
typedef     void *          UDC_HANDLE;

/* define handler error */
#define UDC_HANDLE_ERROR    (NULL)


 typedef enum
{
    UDC_ENDPOINT_0 = 0,
    UDC_ENDPOINT_A,
    UDC_ENDPOINT_B,
    UDC_ENDPOINT_C,
    UDC_ENDPOINT_D,
    UDC_ENDPOINT_E,
    UDC_ENDPOINT_F,
    UDC_ENDPOINT_G,
    UDC_ENDPOINT_H,
    UDC_ENDPOINT_I,
    UDC_ENDPOINT_J,
    UDC_ENDPOINT_K,
    UDC_ENDPOINT_L,
    UDC_ENDPOINT_M,
    UDC_ENDPOINT_N,
    UDC_ENDPOINT_P,
    UDC_ENDPOINT_Q,
    UDC_ENDPOINT_R,
    UDC_ENDPOINT_S,
    UDC_ENDPOINT_T,
    UDC_ENDPOINT_U,
    UDC_ENDPOINT_V,
    UDC_ENDPOINT_W,
    UDC_ENDPOINT_X,
    UDC_TOTAL_ENDPOINTS,
    UDC_INVALID_ENDPOINT
} UDC_EndpointE;

typedef enum
{
    UDC_EVENT_RESET = 0,
    UDC_EVENT_SUSPEND,
    UDC_EVENT_RESUME,
    UDC_EVENT_SOF,
    UDC_EVENT_CONFIG_CHANGE
}UDC_DeviceEventE;

typedef enum
{
    UDC_INTERRUPT_PACKET_COMPLETE = 0,
    UDC_INTERRUPT_FIFO_ERROR
}UDC_EndpointInterruptTypeE;


//Callback functions prototypes
//-----------------------------

/* UDC_EndpointE - endpoint
 * UINT8 *       - data pointer
 * UINT32        - data size
 * BOOL          - For OUT (Rx) only: TRUE for End-Of-Message, FALSE for not completed message
 */
typedef void (*UDCDriverTransactionNofityFn)(UDC_EndpointE, UINT8 *, UINT32, BOOL);



/* USB endpoint operations */
void        UDCDriverEndpointFlush(UDC_EndpointE udcEndpoint);
BOOL        UDCDriverEndpointStall(UDC_EndpointE udcEndpoint);
BOOL        UDCDriverEndpointClearStall(UDC_EndpointE udcEndpoint);


void        UDCDriverReadFromFifo(volatile UINT32 *pFifo, UINT8 *buffer, UINT32 length);
void        UDCDriverWriteToFifo(volatile UINT32 *pFifo, UINT8 *buffer, UINT32 length);

void        UDCDriverEndpointClearReceiveStatus(UDC_EndpointE udcEndpoint);

BOOL        UDCDriverEndpointSetupIN(UDC_EndpointE udcEndpoint,
                                     UINT8 *pTxBuffer, UINT32 length, BOOL autoZLP);

BOOL        UDCDriverEndpointSetupINMultiTransmitWithDma(UDC_EndpointE udcEndpoint, 
                        UINT32 numOfBuffers, USBDevice_MultiTransmitListS *multiList);

BOOL        UDCDriverEndpointSetupOUT(UDC_EndpointE udcEndpoint,
                                     UINT8 *pRxBuffer, UINT32 length,BOOL expect_zlp);


void        UDCDriverEnableEventInterrupt(UDC_DeviceEventE event);
void        UDCDriverDisableEventInterrupt(UDC_DeviceEventE event);
void        UDCDriverClearEventInterrupt(UDC_DeviceEventE event);

BOOL        UDCDriverEndpointDeactivate(UDC_EndpointE udcEndpoint);
BOOL        UDCDriverEndpointActivate(UDC_EndpointE    udcEndpoint,
                                      UINT8            usbEndpointNum,
                                      BOOL             dmaEnabled,     UINT8 dmaChannel,
                                      UINT8            *pDescBuffer,   UINT32 descBufferSize);


/* USB device operations */
void        UDCDriverEnableEndpointInterrupt(UDC_EndpointE udcEndpoint,
                                             UDC_EndpointInterruptTypeE interruptType);
void        UDCDriverDisableEndpointInterrupt(UDC_EndpointE udcEndpoint,
                                              UDC_EndpointInterruptTypeE interruptType);
void        UDCDriverClearEndpointInterrupt(UDC_EndpointE udcEndpoint,
                                            UDC_EndpointInterruptTypeE interruptType);

BOOL        UDCDriverGetCurrentConfigurationSettings(UINT8 *configNum, UINT8 *interfaceNum, UINT8 *altSettingsNum);

BOOL        UDCDriverIsHostEnabledRemoteWakeup(void);
void        UDCDriverForceHostResume(void);
BOOL        UDCDriverIsDeviceControllerEnabled (void);

void UDCDriverDatabaseReset(BOOL fullInit);
BOOL UDCDriverConfigureEndpoints(void);

void        UDCDriverResume(void);
void        UDCDriverSuspend(void);

void        UDCDriverDeactivateHardware(void);
BOOL        UDCDriverActivateHardware(void);

/* System API's */
BOOL        UDCDriverPhase2Init(void);
void        UDCDriverPhase1Init(void);

void usbWakeIndication(UINT8 awake);

#endif /*_UDC_DRIVER_H_*/
