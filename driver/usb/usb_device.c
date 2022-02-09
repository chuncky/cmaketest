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
 * Filename:     usb_device.c
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains the USB Device Stack & API
 *
 * Notes:
 ******************************************************************************/
#include "usb_device.h"    // this include must come after _USB_DEVICE_ definition
#if !defined USB2ONLY
#include "usb1_device.h"
#endif
#include "udc_config.h"
#include "usb_cable.h"
#include "usb_device_types.h"
//#include "SysDynSilicon.h"          // for Sys_usbVersion definition
#if !defined NOCICODE
#include "usb2_device.h"
#include "mvUsbDebug.h"
#endif

#include "usb_macro.h"


/*----------------------------------------------------
 *  Function Declaration
 * --------------------------------------------------*/
UDC_EndpointE USBDeviceConvertUSB2UDCEndpoint(USBDevice_EndpointE endpoint);
#if defined  MV_USB_TRACE_PRINT
void UsbDeviceSetupUARTforDebug(void);
#endif

/*----------------------------------------------------
 *  Global Variables
 * --------------------------------------------------*/
static USBDevice_USBVersionE    _usbDevice_UsbVersion = USB_VER_1_1;
char    _usbDeviceDebugBuffer[ MAX_STRING_SIZE ];
__align(8) UINT8 ep0_lang_id_string_buf[USB_LANG_ID_LENGTH] = {
        USB_LANG_ID_LENGTH,
        USB_DESCRIPTOR_TYPE_STRING,
        (GET_8BIT_LSB(USB_LANG_ENGLISH_ID)),
        (GET_8BIT_MSB(USB_LANG_ENGLISH_ID))
};
USBDeviceStatusNotifyFn       statusNotifyFn_Patch=0;

/* Function Pointers to simplify single interface to both USB1.1 & USB2.0 */
fpUSBDevicePhase2Init            USBDevicePhase2Init = NULL;
fpUSBDeviceIsControllerEnabled   USBDeviceIsControllerEnabled = NULL;
fpUSBDeviceEndpointGetHWCfg      USBDeviceEndpointGetHWCfg = NULL;
fpUSBDeviceEndpointStall         USBDeviceEndpointStall = NULL;
fpUSBDeviceEndpointOpen          USBDeviceEndpointOpen = NULL;
fpUSBDeviceEndpointClose         USBDeviceEndpointClose = NULL;
fpUSBDeviceEndpointAbort		 USBDeviceEndpointAbort = NULL;
fpUSBDeviceVendorClassResponse 	 USBDeviceVendorClassResponse = NULL;
fpUSBDeviceEndpointReceive		 USBDeviceEndpointReceive = NULL;
fpUSBDeviceEndpointReceiveCompletedExt	USBDeviceEndpointReceiveCompletedExt = NULL;
fpUSBDeviceEndpointReceiveCompleted	USBDeviceEndpointReceiveCompleted = NULL;
fpUSBDeviceEndpointTransmit 	  USBDeviceEndpointTransmit = NULL;
fpUSBDeviceEndpointCancelTransmit USBDeviceEndpointCancelTransmit = NULL;
fpUSBDeviceEndpointCancelReceive USBDeviceEndpointCancelReceive = NULL;
fpUSBDeviceEndpointMultiTransmit USBDeviceEndpointMultiTransmit = NULL;
fpUSBDeviceCableDetectionNotify  USBDeviceCableDetectionNotify = NULL;

USBDevice_USBVersionE USBDeviceGetUSBVersionInUse(void)
{
//    _usbDevice_UsbVersion = USB_VER_2_0;
//    _usbDevice_UsbVersion = USB_VER_1_1;
    return  _usbDevice_UsbVersion;
}

USBDevice_USBSpeedE USBDeviceGetUSBSpeedInUse(void)
{
	if ( USBDeviceGetUSBVersionInUse() == USB_VER_1_1)
	{
		return USB_SPEED_FS;
	}
	else   // USB_VER_2_0
	{
	    if( USB2_IS_FULL_SPEED())
	    {
		    return USB_SPEED_FS;
	    }
	    else
	    {
		    return USB_SPEED_HS;
	    }
	}
}


/*******************************************************************************
 * Function:    USBDeviceRegister
 *******************************************************************************
 * Description: Register for USB Device operations
 *
 * Parameters:  statusNotifyFn                  - status change notify
 *              vendorClassRequestNotifyFn      - Vendor/Class request notify
 *              endpointZeroNotifyFn            - endpoint ZERO notify
 *
 * Output:      none
 *
 * Returns:     USBCDevice_ReturnCodeE - return code
 *
 * Notes: temp patch will be fixed later
 ******************************************************************************/

USBCDevice_ReturnCodeE USBDeviceRegister(USBDeviceStatusNotifyFn                 statusNotifyFn,
                                         USBDeviceVendorClassRequestNotifyFn     vendorClassRequestNotifyFn,
                                         USBDeviceEndpointZeroNotifyFn           endpointZeroNotifyFn)
{
    statusNotifyFn_Patch = statusNotifyFn;
    return USB_DEVICE_RC_OK;
} /* End of < USBDeviceRegister > */



/*******************************************************************************
 * Function:    USBDeviceRegisterPatchTempFunc
 *******************************************************************************
 * Description: Register for USB Device operations
 *
 * Parameters:  statusNotifyFn                  - status change notify
 *              vendorClassRequestNotifyFn      - Vendor/Class request notify
 *              endpointZeroNotifyFn            - endpoint ZERO notify
 *
 * Output:      none
 *
 * Returns:     USBCDevice_ReturnCodeE - return code
 *
 * Notes: temp patch
 ******************************************************************************/
USBCDevice_ReturnCodeE USBDeviceRegisterPatchTempFunc(USBDeviceStatusNotifyFn                 statusNotifyFn,
                                         USBDeviceVendorClassRequestNotifyFn     vendorClassRequestNotifyFn,
                                         USBDeviceEndpointZeroNotifyFn           endpointZeroNotifyFn)
{
    _usbDeviceDatabase.statusNotifyFn                       = statusNotifyFn;
    _usbDeviceDatabase.vendorClassRequestNotifyFn           = vendorClassRequestNotifyFn;
    _usbDeviceDatabase.endpointZeroNotifyFn                 = endpointZeroNotifyFn;
    return USB_DEVICE_RC_OK;
} /* End of < USBDeviceRegister > */

/*******************************************************************************
 * Function:    USBDeviceStatusGet
 *******************************************************************************
 * Description: Return the current USB DEVICE status
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     USBDevice_StatusE - USB device status result
 *
 * Notes:
 ******************************************************************************/
USBDevice_StatusE USBDeviceStatusGet(void)
{
    return(_usbDeviceDatabase.status);

} /* End of < USBDeviceStatusGet > */


/*******************************************************************************
 * Function:    USBDevicePhase1Init
 *******************************************************************************
 * Description: Phase 1 of initialization process
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USBDevicePhase1Init(void)
{
//Hongji 201011
   // _usbDevice_UsbVersion = ((Sys_usbVersion() == 1) ? USB_VER_1_1 : USB_VER_2_0);
   _usbDevice_UsbVersion = USB_VER_2_0;


    if ( _usbDevice_UsbVersion == USB_VER_1_1 )
    {
#if !defined USB2ONLY
         USB1DevicePhase1Init();
#endif
    }
    else
    {
#if !defined NOCICODE
        USB2DevicePhase1Init();
#endif
    }

} /* End of < USBDevicePhase1Init > */


void USBDeviceSetDescriptors(void)
{

}

/******************************************************************************/
/*                  COMMON FUNCTION TO USB1.1 & USB2.0                        */
/******************************************************************************/


/************************************************************************
* Function: USBDeviceSetDescriptor
*************************************************************************
* Description: updates USB device driver descriptor
*
* Parameters:
*
* Return value:
*
* Notes: must be called before enumeration happens
************************************************************************/

void USBDeviceSetDescriptor( USB_DescriptorTypesE descType, UINT8 *pDesc, UINT16 descLength, UINT8 id)
{
    switch(descType)
    {
    case USB_DESCRIPTOR_TYPE_DEVICE:
        _usbDeviceDescriptors.pDeviceDescriptor             = pDesc;
        _usbDeviceDescriptors.deviceDescriptorLength        = descLength;

    case USB_DESCRIPTOR_TYPE_CONFIGURATION:
        _usbDeviceDescriptors.pConfigDescriptor            = pDesc;
        _usbDeviceDescriptors.configDescriptorLength       = descLength;
        break;
    case USB_DESCRIPTOR_TYPE_STRING:
        //USB_DEVICE_ASSERT(id <= USB_DEVICE_TOTAL_STRINGS);
        if(id < USB_DEVICE_TOTAL_STRINGS)
            _usbDeviceDescriptors.pStringDescriptor[id]     =  pDesc;
		if (_usbDeviceDescriptors.stringDescriptorTotal < id )
			_usbDeviceDescriptors.stringDescriptorTotal = id;
        break;
    case USB_DESCRIPTOR_TYPE_QUALIFIER:
        _usbDeviceDescriptors.pQualifDescriptor            = pDesc;
        _usbDeviceDescriptors.qualifDescriptorLength       = descLength;
		break;
    case USB_DESCRIPTOR_TYPE_SPEED:
        _usbDeviceDescriptors.pOtherSpeedDescriptor            = pDesc;
        _usbDeviceDescriptors.otherSpeedDescriptorLength       = descLength;
		break;
    case USB_DESCRIPTOR_TYPE_INTERFACE:
    case USB_DESCRIPTOR_TYPE_ENDPOINT:
    default:
        /* wrong type */
        USB_DEVICE_ASSERT(0); //not supporting any other descriptors
        break;
    }
}


void USBCableDetectionNotify(USBCable_StatusE cableStatus)
{
    if(cableStatus == USB_CABLE_OUT)
    {/*USB_CABLE_OUT - USB OFF */
        USBDeviceCableDetectionNotify(cableStatus);
    }
    else
    {/*USB_CABLE_IN - USB ON */
       USBDeviceCableDetectionNotify(cableStatus);
    }
}

/*******************************************************************************
 * Function:    USBDeviceSimulatePlug
 *******************************************************************************
 * Description:  SW simulation of cable plug , causes USB to perform all actions
 * it would perform in regular cable IN/OUT
 *
 * Parameters:  plug_operation
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USBDeviceSimulatePlug(USBD_SimulatePlugE plug_operation)
{
    switch(plug_operation)
    {
    case USBD_SimulatePlugE_IN: //assuming that either out was called before or it is the first time
        //need to add validation for that
        USBDeviceCableDetectionNotify(USB_CABLE_IN);
        break;
    case USBD_SimulatePlugE_OUT: //can be called in OUT as well
        USBDeviceCableDetectionNotify(USB_CABLE_OUT);
        break;
    case USBD_SimulatePlugE_OUT_AND_IN:
        USBDeviceCableDetectionNotify(USB_CABLE_OUT);
        USBDeviceCableDetectionNotify(USB_CABLE_IN);
        break;
    }
}

/*******************************************************************************
 * Function:    USBDeviceConvertUSB2UDCEndpoint
 *******************************************************************************
 * Description: Convert USB endpoint number into UDC endpoint
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:       Search UDC endpoint number within the endpoint configuration
 ******************************************************************************/
UDC_EndpointE USBDeviceConvertUSB2UDCEndpoint(USBDevice_EndpointE endpoint)
{
    UDC_EndpointE       udcEndpoint;

   // uart_printf("_usbDevice_UsbVersion is %d\r\n",_usbDevice_UsbVersion);
    if ( _usbDevice_UsbVersion == USB_VER_1_1 )
    {
        /* search in endpoint configuration */
        for(udcEndpoint=UDC_ENDPOINT_A; udcEndpoint<UDC_TOTAL_ENDPOINTS; udcEndpoint++)
        {
            if(_u1dcDefaultEndpointConfig[udcEndpoint].usbEndpointNum == endpoint)
            {/* usb endpoint found - break */
                break;
            }
        }
    }
    else
    {
        /* search in endpoint configuration */
        for(udcEndpoint=UDC_ENDPOINT_A; udcEndpoint<UDC_TOTAL_ENDPOINTS; udcEndpoint++)
        {
            if(_u2dcDefaultEndpointConfig[udcEndpoint].usbEndpointNum == endpoint)
            {/* usb endpoint found - break */
                break;
            }
        }
    }

    return udcEndpoint;
} /* End of < USBDeviceConvertUSB2UDCEndpoint > */


#if defined MV_USB_TRACE_PRINT
void UsbDeviceSetupUARTforDebug(void)
{
    const UARTConfiguration configuration = { UART_FIFO_CTL,  UART_INT_TRIGGER_L32,  UART_BAUD_115200,  UART_WORD_LEN_8,  UART_ONE_STOP_BIT,
         UART_NO_PARITY_BITS, UART_BASIC_INTERFACE,  FALSE,  FALSE,  UART_ACTIVITY_RX|UART_ACTIVITY_EXT|UART_GPIO_WAKEUP, {FALSE,FALSE,FALSE,FALSE,FALSE} };

    if ( UARTConfigure(UART_PORT_FFUART ,&configuration)!= UART_RC_OK)
    {
//        DIAG_FILTER(HW_PLAT, UART, configureBadRC, UART_TEST)
//        diagPrintf ("the uartConfigure fail return code is %d",returnCode);
    }
    else /* UARTConfigure succeed*/
    {
        if (UARTOpen(UART_PORT_FFUART) != UART_RC_OK)
        {
//            DIAG_FILTER(HW_PLAT, UART, openBadRC, UART_TEST)
//            diagPrintf ("the uartOpen fail. return code is %d",returnCode);
        }
    }

}
#endif /*  MV_USB_TRACE_PRINT */

