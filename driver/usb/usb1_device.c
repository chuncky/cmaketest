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
 * Filename:     usb_device.c  changed to usb1_device.c
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains the USB Device Stack & API
 *
 * Notes:
 ******************************************************************************/
#include "usb1_device.h"
#define _USB_DEVICE_TYPES_
#include "usb_device_types.h"
#undef  _USB_DEVICE_TYPES_
#include "usb_device_def.h"
#include "usb_device_config.h"

#include "udc_types.h"
#include "udc_driver.h"
#include "udc_def.h"
#include "udc_config.h"
#include "usb_cable.h"
#include "usb_def.h"

#include "string.h"     // for memset, memcpy definition

#if (USB_DEVICE_DIAG_ENABLED == 1)
#include "diag_ram.h"
#endif
#if defined (_TAVOR_BOERNE_)
    #include "prm.h"
#endif


static __align(8) UINT8 ep0_out_data_buffer[USB_EP0_MAX_RX_TRANSFER_SIZE];
static __align(8) UINT8 ep0_ctrl_setup_buff[UDC_EP0_CTRL_TRANSFER_SETUP_PACKET_SIZE];
extern __align(8) UINT8 ep0_lang_id_string_buf[];

extern USBDeviceStatusNotifyFn       statusNotifyFn_Patch;
extern UDC_EndpointE USBDeviceConvertUSB2UDCEndpoint(USBDevice_EndpointE endpoint);
extern void USBDeviceSetDescriptors(void);

/*******************************************************************************
 *                      S T A T I C  F U N C T I O N S
 ******************************************************************************/

/*******************************************************************************
 * Function:    USB1DeviceDatabaseReset
 *******************************************************************************
 * Description: Initialize internal database
 *
 * Parameters:  fullInit    - full initialization (after powerup) or partial
 *                            (during operation)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceDatabaseReset(BOOL fullInit)
{
    UINT32          idx;

    /* Descriptors */
    if(fullInit)
    {
        _usbDeviceDescriptors.pDeviceDescriptor                     = NULL;
        _usbDeviceDescriptors.deviceDescriptorLength                = 0;
        _usbDeviceDescriptors.pConfigDescriptor                     = NULL;
        _usbDeviceDescriptors.configDescriptorLength                = 0;

        _usbDeviceDescriptors.stringDescriptorTotal                 = 0;

        for(idx=0; idx<USB_DEVICE_TOTAL_STRINGS; idx++)
        {
            _usbDeviceDescriptors.pStringDescriptor[idx]            = NULL;
        }
    }

    /* USB device data base */
    _usbDeviceDatabase.status                                       = USB_DEVICE_STATUS_NOT_CONNECTED;
    if(fullInit)
    {
        _usbDeviceDatabase.statusNotifyFn                           = NULL;
        _usbDeviceDatabase.vendorClassRequestNotifyFn               = NULL;
        _usbDeviceDatabase.endpointZeroNotifyFn                     = NULL;
    }

    /* Endpoint 0 */
    _usbDeviceDatabase.ep0.state                                    = USB_DEVICE_EP0_STATE_IDLE;
    _usbDeviceDatabase.ep0.operation                                = USB_DEVICE_EP0_NO_OPERATION;
    _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff                 = ep0_out_data_buffer;
    _usbDeviceDatabase.ep0.ctrl_setup_buff                          = ep0_ctrl_setup_buff;
    _usbDeviceDatabase.ep0.crtl_trasnfers_cnt                       = 0;
    _usbDeviceDatabase.ep0.ep0_int_cnt                              = 0;
    _usbDeviceDatabase.ep0.ep0_IN_cnt                               = 0;
    _usbDeviceDatabase.ep0.ep0_OUT_cnt                              = 0;

    /* General Endpoint */
    for(idx=0; idx<USB_DEVICE_TOTAL_ENDPOINTS; idx++)
    {
        _usbDeviceEndpoint[idx].endpointBusy                         = FALSE;
        if(fullInit)
        {
            _usbDeviceEndpoint[idx].endpointOpen                     = FALSE;
            _usbDeviceEndpoint[idx].udcEndpoint                      = UDC_INVALID_ENDPOINT;
            _usbDeviceEndpoint[idx].transactionCompletedNotifyFn     = NULL;
        }
    }




} /* End of < USB1DeviceDatabaseReset > */


/*******************************************************************************
 * Function:    USB1DeviceAbortAllEndpoints
 *******************************************************************************
 * Description: Abort all endpoints
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceAbortAllEndpoints(void)
{
    USBDevice_EndpointE ep;

    for(ep=USB_DEVICE_ENDPOINT_0; ep<USB_DEVICE_TOTAL_ENDPOINTS; ep++)
    {
        USB1DeviceEndpointAbort(ep);
    }
} /* End of < USB1DeviceAbortAllEndpoints > */

/*******************************************************************************
 * Function:    USB1DeviceStatusChange
 *******************************************************************************
 * Description: Handle changes in USB status
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceStatusChange(USBDevice_StatusChangeE newStatusChange)
{
    BOOL                    needToNotify    = FALSE;

    switch(newStatusChange)
    {
        case USB_DEVICE_ST_CHANGE_CABLE_OUT:

            USB1DeviceAbortAllEndpoints();
            UDCDriverDeactivateHardware();
            USB1DeviceDatabaseReset(FALSE); // reset endpoint management states etc
            UDCDriverDatabaseReset(FALSE); // reset endpoint management states etc in UDC

            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_NOT_CONNECTED;
            needToNotify                = TRUE;
#if defined(UDC_USE_WAKEUP_LED)
            usbWakeIndication(USB_CABLE_PLUGOUT);
#endif
            break;

        case USB_DEVICE_ST_CHANGE_CABLE_IN:
            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_CONNECTED;
            USBDeviceSetDescriptors();     // update descriptor
            USB_DEVICE_ASSERT( UDCDriverConfigureEndpoints()); //configure HW again
            UDCDriverActivateHardware(); //actually enable interrupts and UDC
#if defined(UDC_USE_WAKEUP_LED)
            usbWakeIndication(USB_CABLE_PLUGIN);
#endif
            break;

        case USB_DEVICE_ST_CHANGE_RESET:
            USB1DeviceAbortAllEndpoints();
            USB1DeviceDatabaseReset(FALSE);
            UDCDriverDatabaseReset(FALSE); // reset endpoint management states etc in UDC

            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_RESET;
            needToNotify                = TRUE;
            break;

        case USB_DEVICE_ST_CHANGE_GET_DEVICE_DESC:
            /* When using COMPOSITE device, GET_DEVICE_DESC can be received few times
             * due to the PC enumeration process.
             * Therefore, GET_DEVICE_DESC changes the current status only if previous state was RESET
             */
            if(_usbDeviceDatabase.status == USB_DEVICE_STATUS_RESET)
            {

                _usbDeviceDatabase.status   = USB_DEVICE_STATUS_ENUM_IN_PROCESS;
                needToNotify            = TRUE;
            }
            else
            {/* do nothing - ignore this */
                return;
            }
            break;

        //case USB_DEVICE_ST_CHANGE_SET_CONFIGURATION:
        case USB_DEVICE_ST_CHANGE_CONFIG_CHANGE:            /* Connection turned ON  */

            _usbDeviceDatabase.status   = USB_DEVICE_STATUS_CONNECTED;
            needToNotify                = TRUE;
            break;

        case USB_DEVICE_ST_CHANGE_SUSPEND:
            UDCDriverSuspend();
            break;
        case USB_DEVICE_ST_CHANGE_RESUME:
            UDCDriverResume();
            break;
    }



    if(needToNotify &&  _usbDeviceDatabase.statusNotifyFn)
    {
        _usbDeviceDatabase.statusNotifyFn(_usbDeviceDatabase.status);

        if(statusNotifyFn_Patch)
            (statusNotifyFn_Patch)(_usbDeviceDatabase.status);
    }
} /* End of < USB1DeviceStatusChange > */

/*******************************************************************************
 * Function:    USB1DeviceEP0HandleReceiving
 *******************************************************************************
 * Description: Load endpoint with data - used for interrupt mode
 *
 * Parameters:  udcEndpoint     - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     BOOL - notify the user or not (not nessecary endOfMessage)
 *
 * Notes:
 ******************************************************************************/
static void  USB1DeviceEP0HandleReceiving(void)
{
    UINT16              length          = UDC_GET_FIFO_BYTE_COUNT(UDC_ENDPOINT_0);

    if(length > 0)
    {
        USB_DEVICE_ASSERT(_usbDeviceDatabase.ep0.ctrl_out_data_stage_current_length+length <=
                          _usbDeviceDatabase.ep0.ctrl_out_data_stage_total_length);


        UDCDriverReadFromFifo(UDC_EP0_GET_FIFO_ADDRESS(),
                          _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff + _usbDeviceDatabase.ep0.ctrl_out_data_stage_current_length,
                          length);

        _usbDeviceDatabase.ep0.ctrl_out_data_stage_current_length += length;


        if(length < UDC_EP0_MAX_PACKET_SIZE)
        {
             // short packet
            _usbDeviceDatabase.ep0.operation =  USB_DEVICE_EP0_RECEIVED_LAST_DATA_PACKET;
        }
     }
     else
     {
         // ZLP
         _usbDeviceDatabase.ep0.operation =  USB_DEVICE_EP0_RECEIVED_LAST_DATA_PACKET;
     }



    _usbDeviceDatabase.ep0.ep0_OUT_cnt++;


     /* clear OPC */
    UDC_EP0_CLEAR_OPC();

} /* End of  USB1DeviceEP0HandleReceiving > */



/*******************************************************************************
 * Function:    USB1DeviceEP0ReceiveSetupStagePacket
 *******************************************************************************
 * Description:  used for interrupt mode
 *
 * Parameters:  udcEndpoint     - udc endpoint 0
 *
 * Output:      none
 *
 * Returns:     BOOL - notify the user or not (not nessecary endOfMessage)
 *
 * Notes:
 ******************************************************************************/
static BOOL USB1DeviceEP0ReceiveSetupStagePacket(void)
{
   //get length from register
   UINT16              length          = UDC_GET_FIFO_BYTE_COUNT(UDC_ENDPOINT_0);

#if !defined (_TAVOR_Z0_SILICON_)
	//Li's workaround for BCR zero problem
   if(length ==0)
   {
	   UDC_EP0_CLEAR_OPC();
	   length = UDC_GET_FIFO_BYTE_COUNT(UDC_ENDPOINT_0);
   }
   // Li's workaround for BCR zero problem
#endif /* _TAVOR_Z0_SILICON_ */

   USB_DEVICE_ASSERT(length == UDC_EP0_CTRL_TRANSFER_SETUP_PACKET_SIZE);

   _usbDeviceDatabase.ep0.operation  = USB_DEVICE_EP0_RECEIVED_SETUP_PACKET;
   _usbDeviceDatabase.ep0.crtl_trasnfers_cnt++;

   //read setup packet from FIFO
   UDCDriverReadFromFifo(UDC_EP0_GET_FIFO_ADDRESS(),_usbDeviceDatabase.ep0.ctrl_setup_buff, length);

   /* clear the SETUP command bits */
    UDC_EP0_CLEAR_SETUP_CMD();

   return FALSE;
}

/*******************************************************************************
 * Function:    USB1DeviceEP0HandleTransmitting
 *******************************************************************************
 * Description: Handle EP0 transmission
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0HandleTransmitting(void)
{
    UINT8               fifo_load_size=0;
    BOOL                set_short_packet    = FALSE;
    UINT16              left_to_transmit    = _usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length -
                                              _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length;

    if(left_to_transmit >= UDC_EP0_MAX_PACKET_SIZE)
    {/* size to transmit is bigger than or equal to MPS (FIFO size) */
        fifo_load_size    = UDC_EP0_MAX_PACKET_SIZE;
        _usbDeviceDatabase.ep0.operation  = USB_DEVICE_EP0_TRANSMITTED_DATA_PACKET;
    }
    else
    {
        fifo_load_size  = left_to_transmit;
        set_short_packet = TRUE;


        _usbDeviceDatabase.ep0.operation  = USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET;

    }

    if(fifo_load_size)
    {
        UDCDriverWriteToFifo(UDC_EP0_GET_FIFO_ADDRESS(),
                         _usbDeviceDatabase.ep0.ctrl_in_data_stage_buff + _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length,
                         fifo_load_size);


        _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length += fifo_load_size;
    }
   // else
   // {
   //     _usbDeviceDatabase.ep0.operation  = USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET_NOTIFY;
   // }

    /* if short packet is needed - set the bit */
    if(set_short_packet)
    {
        UDC_EP0_SET_IPR();
    }


    _usbDeviceDatabase.ep0.ep0_IN_cnt++;
} /* End of < USB1DeviceEP0HandleTransmitting > */


/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessGetDescriptorString
 *******************************************************************************
 * Description: Process STANDARD request (in SETUP command)
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0ProcessGetDescriptorString(USB_SetupCmdS *pSetupCmd)
{
    UINT8           str_desc_ind = pSetupCmd->wValue & 0xFF;
    UINT16          langID      = pSetupCmd->wIndex;

    /* is this Language list request? */
    if(langID == USB_LANG_LIST_REQ)
    {
        _usbDeviceDatabase.ep0.ctrl_in_data_stage_buff  = ep0_lang_id_string_buf;
    }
    else
	{
		if(langID == USB_LANG_ENGLISH_ID)
		{
			USB_DEVICE_ASSERT(str_desc_ind < USB_DEVICE_TOTAL_STRINGS);
			USB_DEVICE_ASSERT(_usbDeviceDescriptors.pStringDescriptor[str_desc_ind]);
			_usbDeviceDatabase.ep0.ctrl_in_data_stage_buff  = _usbDeviceDescriptors.pStringDescriptor[str_desc_ind];
		}
		else
		{
			USB_DEVICE_ASSERT(FALSE); /* Language requested is wrong */
		}
	}
    _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length = 0;
    _usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length   =
	(USB_DEVICE_MAX_LENGTH(_usbDeviceDatabase.ep0.ctrl_in_data_stage_buff[0], pSetupCmd->wLength));

} /* End of < USB1DeviceEP0ProcessGetDescriptorString > */

/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessGetDescriptor
 *******************************************************************************
 * Description: Process STANDARD request (in SETUP command)
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0ProcessGetDescriptor(USB_SetupCmdS *pSetupCmd)
{
    UINT8           descType  = (pSetupCmd->wValue >> 8) & 0xFF;
    UINT8           maxLength;
    //YG UINT8           descIndex = pSetupCmd->wValue & 0xFF;

    switch(descType)
    {
        case USB_DESCRIPTOR_TYPE_DEVICE:
            maxLength = USB_DEVICE_MAX_LENGTH(_usbDeviceDescriptors.deviceDescriptorLength, pSetupCmd->wLength);
            _usbDeviceDatabase.ep0.ctrl_in_data_stage_buff              = _usbDeviceDescriptors.pDeviceDescriptor;
            _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length    = 0;
            _usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length      = maxLength;
            USB1DeviceEP0HandleTransmitting();
            USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_GET_DEVICE_DESC);
            break;

        case USB_DESCRIPTOR_TYPE_CONFIGURATION:
            maxLength = USB_DEVICE_MAX_LENGTH(_usbDeviceDescriptors.configDescriptorLength, pSetupCmd->wLength);
            _usbDeviceDatabase.ep0.ctrl_in_data_stage_buff           = _usbDeviceDescriptors.pConfigDescriptor;
            _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length = 0;
            _usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length   = maxLength;
            USB1DeviceEP0HandleTransmitting();
            break;

        case USB_DESCRIPTOR_TYPE_STRING:
           USB1DeviceEP0ProcessGetDescriptorString(pSetupCmd);
           USB1DeviceEP0HandleTransmitting();
            break;

        case USB_DESCRIPTOR_TYPE_INTERFACE:
            USB_DEVICE_ASSERT(FALSE);
            break;

        case USB_DESCRIPTOR_TYPE_ENDPOINT:
            USB_DEVICE_ASSERT(FALSE);
            break;

#if defined(USB_DEBUG)
        default:
            /* wrong type */
            USB_DEVICE_ASSERT(FALSE);
            break;
#endif /*USB_DEBUG*/
    }
} /* End of < USB1DeviceEP0ProcessGetDescriptor > */

/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessVendorClassReq
 *******************************************************************************
 * Description: Process STANDARD request (in SETUP command)
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
/*static void USB1DeviceEP0ProcessVendorClassReq(USB_SetupCmdS *pSetupCmd)
{
    if(pSetupCmd->wLength == 0)
    {// command with NO data-phase
        _usbDeviceDatabase.vendorClassRequestNotifyFn(pSetupCmd, 0, 0);
    }
    else
    {// data phase exists
        USBCDevice_DataStageDirectionE dataDir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(pSetupCmd->bmRequestType);

        _usbDeviceDatabase.vendorClassRequestNotifyFn(pSetupCmd, dataDir, pSetupCmd->wLength);
    }

}*/
 //End of < USBDeviceEP0ProcessVendorClassReq >

/*******************************************************************************
 * Function:    USBDeviceEP0ProcessStandardReq
 *******************************************************************************
 * Description: Process STANDARD request (in SETUP command)
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
volatile UINT8 setFeature = 0;
static void USB1DeviceEP0ProcessStandardReq(USB_SetupCmdS *pSetupCmd)
{
    switch(pSetupCmd->bRequest)
    {
        case USB_STANDARD_REQ_GET_DESCRIPTOR_ID:
           _usbDeviceDatabase.ep0.isTransmitNeedNotify = FALSE;
            USB1DeviceEP0ProcessGetDescriptor(pSetupCmd);
            break;

        case USB_STANDARD_REQ_SET_CONFIGURATION_ID:
            //YG - this SET_CONFIG should be handled also in "EventNotify"
            //so no need for double-action - before changing this code - make sure the other one does work
            USB_DEVICE_ASSERT(FALSE);
            USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_SET_CONFIGURATION);
            break;
        case USB_STANDARD_REQ_SET_FEATURE_ID:
            setFeature++;
            break;

        case USB_STANDARD_REQ_SET_DESCRIPTOR_ID:
        case USB_STANDARD_REQ_GET_CONFIGURATION_ID:
        case USB_STANDARD_REQ_GET_STATUS_ID:
        case USB_STANDARD_REQ_CLEAR_FEATURE_ID:
        case USB_STANDARD_REQ_SET_ADDRESS_ID:
        case USB_STANDARD_REQ_GET_INTERFACE_ID:
        case USB_STANDARD_REQ_SET_INTERFACE_ID:
        case USB_STANDARD_REQ_SYNC_FRAME_ID:
            /* not currently handled - need to add functionality according to UDC responses */
            USB_DEVICE_ASSERT(FALSE);
            break;

#if defined(USB_DEBUG)
        default:
            /* wrong bRequest */
            USB_DEVICE_ASSERT(FALSE);
            break;
#endif /*USB_DEBUG*/
    }
} /* End of < USB1DeviceEP0ProcessStandardReq > */

/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessSetupCmd
 *******************************************************************************
 * Description: Process SETUP command and act upon it
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceEP0ProcessSetupCmd(void)
{

    USB_SetupCmdS   setup_packet;
    USB_ReqTypesE   request_type;
    USBCDevice_DataStageDirectionE data_dir;

    memcpy((char*)&setup_packet,(char*)(_usbDeviceDatabase.ep0.ctrl_setup_buff),sizeof(USB_SetupCmdS));
    request_type    = (USB_ReqTypesE)USB_DEVICE_GET_REQ_TYPE(setup_packet.bmRequestType);
    data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(setup_packet.bmRequestType);

    switch(request_type)
    {
        case USB_REQ_TYPE_STANDARD:
            USB1DeviceEP0ProcessStandardReq(((USB_SetupCmdS *)_usbDeviceDatabase.ep0.ctrl_setup_buff));
            break;
        case USB_REQ_TYPE_CLASS:
        case USB_REQ_TYPE_VENDOR:
             if(data_dir == USBCDevice_DataStageDirectionH2D)
             { //prepare fields
                 _usbDeviceDatabase.ep0.ctrl_out_data_stage_total_length = setup_packet.wLength;
                 _usbDeviceDatabase.ep0.ctrl_out_data_stage_current_length = 0;

                 if(setup_packet.wLength == 0) //host only transmits setup packet, notify user so user can reply with ZLP
                 {
                     USB_DEVICE_ASSERT(_usbDeviceDatabase.vendorClassRequestNotifyFn ); //must not be NULL
                     setup_packet.p_data =0;

                     _usbDeviceDatabase.vendorClassRequestNotifyFn(&setup_packet);
                 }
             }
             else
             {
                 _usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length = setup_packet.wLength;
                 _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length = 0;

                 USB_DEVICE_ASSERT(_usbDeviceDatabase.vendorClassRequestNotifyFn ); //must not be NULL
                 setup_packet.p_data =0;

                  //must notify upper layer SW & expect transmit
                 _usbDeviceDatabase.vendorClassRequestNotifyFn(&setup_packet);
            }
            break;
        default:
            USB_DEVICE_ASSERT(0);
            break;
    }

} /* End of < USB1DeviceEP0ProcessSetupCmd > */

/*******************************************************************************
 * Function:    USB1DeviceEP0TransferCompleted()
 *******************************************************************************
 * Description: notify user upon control transfer completion
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes: In setup struct and data buffers must be copied to other location in the notify context
 * After returning from user Notify these buffer might be used by driver again
 * User must check the fields to determine request type , length and other information
 ************************************************************************************************************/
void USB1DeviceEP0TransferCompleted()
{

    USB_SetupCmdS *p_setup_packet = (USB_SetupCmdS *)_usbDeviceDatabase.ep0.ctrl_setup_buff;
    //USBCDevice_DataStageDirectionE data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(p_setup_packet->bmRequestType);
    USB_ReqTypesE   request_type    = (USB_ReqTypesE)USB_DEVICE_GET_REQ_TYPE(p_setup_packet->bmRequestType);


    /*if(data_dir == USBCDevice_DataStageDirectionD2H)
    {
        if(p_setup_packet->wLength == 0)
             p_setup_packet->p_data = 0;
        else
             p_setup_packet->p_data = _usbDeviceDatabase.ep0.ctrl_in_data_stage_buff;
    }
    else
    {
        if(p_setup_packet->wLength == 0)
            p_setup_packet->p_data = 0;
        else
            p_setup_packet->p_data = _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff;
    }*/


    //?? need to think if any notify at all is required  here
    /*if(  request_type ==  USB_REQ_TYPE_CLASS || request_type ==  USB_REQ_TYPE_VENDOR)
    {
        if(_usbDeviceDatabase.vendorClassRequestNotifyFn)
        {
            _usbDeviceDatabase.vendorClassRequestNotifyFn(p_setup_packet);
        }
    }
    else*/
    {
        if(request_type ==  USB_REQ_TYPE_STANDARD)
        {
            if(_usbDeviceDatabase.endpointZeroNotifyFn)
            {
                _usbDeviceDatabase.endpointZeroNotifyFn(p_setup_packet);
            }
        }
    }

} /* End of < USB1DeviceEP0TransactionCompleted > */

/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessOutData
 *******************************************************************************
 * Description: Process OUT (RX) state for EP0
 *
 * Parameters:  udccsr0Reg      - UDCCSR0 register value
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0ProcessOutData(UINT32 udccsr0Reg)
{
    USB_DEVICE_ASSERT(UDC_EP0_IS_OUT_PACKET(udccsr0Reg));
    USB1DeviceEP0HandleReceiving();


} /* End of < USB1DeviceEP0ProcessOutData > */

/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessInData
 *******************************************************************************
 * Description: Process IN (TX) state for EP0
 *
 * Parameters:  udccsr0Reg      - UDCCSR0 register value
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0ProcessInData(UINT32 udccsr0Reg)
{

	//win2000 gets assert , for test removing
    //USB_DEVICE_ASSERT(UDC_EP0_IS_IN_PACKET(udccsr0Reg));

    /* Write data to Endpoint 0 data buffer */
    USB1DeviceEP0HandleTransmitting();

} /* End of < USB1DeviceEP0ProcessInData > */
/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessIdleState
 *******************************************************************************
 * Description: Process the IDLE state of EP0
 *
 * Parameters:  udccsr0Reg      - UDCCSR0 register value
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static BOOL USB1DeviceEP0ProcessIdleState(UINT32 udccsr0Reg)
{
    BOOL        result = TRUE;

    /* make sure this is a setup command */
    if( !UDC_EP0_IS_SETUP_CMD(udccsr0Reg) )
    {
        USB_DEVICE_ASSERT(0); /* could be a wrong state-machine handling */
    }

    //read setup packet
    USB1DeviceEP0ReceiveSetupStagePacket();
    return result;
} /* End of < USB1DeviceEP0ProcessIdle > */


/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessSetupState
 *******************************************************************************
 * Description: Process the Setup state of EP0
 *
 * Parameters:  udccsr0Reg      - UDCCSR0 register value
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes: Called upon receiving an interrupt from UDC , while in Setup
 ******************************************************************************/
void USB1DeviceEP0ProcessSetupState(UINT32 udccsr0Reg)
{
    USB_SetupCmdS *p_setup_packet = (USB_SetupCmdS *)_usbDeviceDatabase.ep0.ctrl_setup_buff;

    USBCDevice_DataStageDirectionE data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(p_setup_packet->bmRequestType);

    if(data_dir ==  USBCDevice_DataStageDirectionD2H)
    {

        if(p_setup_packet->wLength) // if wlength is not zero expect data IN
        {
              USB1DeviceEP0ProcessInData(udccsr0Reg);
        }
        else //can not be , if D->H , then host requests some data
        {
              USB_DEVICE_ASSERT(0);
        }
    }
    else //USBCDevice_DataStageDirectionH2D
    {
        if(p_setup_packet->wLength) // if wlength is not zero expect data OUT- host transfers data
        {
            USB1DeviceEP0ProcessOutData(udccsr0Reg);
        }
        else// if wlength is zero read data IN notify, device  closes transfer
        {
            USB1DeviceEP0ProcessInData(udccsr0Reg);
        }
    }
}


/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessInDataState
 *******************************************************************************
 * Description: Validate & Transit State for EP0
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
 static void USB1DeviceEP0ProcessInDataState(UINT32 udccsr0Reg)
 {
     if(_usbDeviceDatabase.ep0.operation  == USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET)
     {
         _usbDeviceDatabase.ep0.operation = USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET_NOTIFY;
         return;
     }

     USB1DeviceEP0ProcessInData(udccsr0Reg);
 }



/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessOutDataState
 *******************************************************************************
 * Description: Validate & Transit State for EP0
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
 static void USB1DeviceEP0ProcessOutDataState(UINT32 udccsr0Reg)
 {
     USB1DeviceEP0ProcessOutData(udccsr0Reg);
 }


/*******************************************************************************
 * Function:    USB1DeviceEP0DataOutIsOverNotify
 *******************************************************************************
 * Description: Validate & Transit State for EP0
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
 static void USB1DeviceEP0DataOutIsOverNotify()
 {
      USB_SetupCmdS setup_packet;
//      USBCDevice_DataStageDirectionE data_dir;
      USB_ReqTypesE   request_type;

      memcpy((char*)&setup_packet,(char *)_usbDeviceDatabase.ep0.ctrl_setup_buff,sizeof(USB_SetupCmdS));
//      data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(setup_packet.bmRequestType);
      request_type = (USB_ReqTypesE)USB_DEVICE_GET_REQ_TYPE(setup_packet.bmRequestType);

      if(_usbDeviceDatabase.ep0.operation == USB_DEVICE_EP0_RECEIVED_LAST_DATA_PACKET)
      {
         if(request_type == USB_REQ_TYPE_CLASS || request_type == USB_REQ_TYPE_VENDOR)
         {
             setup_packet.p_data = _usbDeviceDatabase.ep0.ctrl_out_data_stage_buff;

             //host has transmitted all data , now device has to provide status packet
             USB_DEVICE_ASSERT(_usbDeviceDatabase.vendorClassRequestNotifyFn);
             _usbDeviceDatabase.vendorClassRequestNotifyFn(&setup_packet);
         }
      }
 }
/*******************************************************************************
 * Function:    USB1DeviceEP0ProcessStatusState
 *******************************************************************************
 * Description: Validate & Transit State for EP0
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0ProcessStatusState(UINT32 udccsr0Reg)
{
    USB_SetupCmdS *p_setup_packet = (USB_SetupCmdS *)_usbDeviceDatabase.ep0.ctrl_setup_buff;
    USBCDevice_DataStageDirectionE data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(p_setup_packet->bmRequestType);

    if(data_dir == USBCDevice_DataStageDirectionH2D)//OUT
    {
        //opposite direction
        USB1DeviceEP0ProcessInData(udccsr0Reg); //receive last transmitted tx packet notify interrupt
    }
    else //USBCDevice_DataStageDirectionD2H //IN
    {
        //opposite direction
        USB1DeviceEP0ProcessOutData(udccsr0Reg);//rerecive last ZLP
    }

    //notify
    USB1DeviceEP0TransferCompleted();
}
/*******************************************************************************
 * Function:    USB1DeviceEP0TransitState
 *******************************************************************************
 * Description: Validate & Transit State for EP0
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void USB1DeviceEP0TransitState()
{
    USB_SetupCmdS *p_seup_cmd = (USB_SetupCmdS *)_usbDeviceDatabase.ep0.ctrl_setup_buff;
    USBCDevice_DataStageDirectionE data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(p_seup_cmd->bmRequestType);

    switch(_usbDeviceDatabase.ep0.state) //check current state
    {
        case USB_DEVICE_EP0_STATE_IDLE: //---could only receive setup packet
            switch(_usbDeviceDatabase.ep0.operation)
            {
            case USB_DEVICE_EP0_RECEIVED_SETUP_PACKET:
                _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_SETUP;
                break;
            default:
                USB_DEVICE_ASSERT(0); //wrong operation
                break;
            }
            break;
         case USB_DEVICE_EP0_STATE_SETUP: //need to
             switch(_usbDeviceDatabase.ep0.operation)
             {
             case USB_DEVICE_EP0_RECEIVED_DATA_PACKET:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_OUT;
                 break;
             case USB_DEVICE_EP0_RECEIVED_LAST_DATA_PACKET:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
                 break;
             case USB_DEVICE_EP0_TRANSMITTED_DATA_PACKET:
             case USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
                 break;
             case USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET_NOTIFY:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
                 break;
             default:
                USB_DEVICE_ASSERT(0); //wrong operation
                break;
             }
           break;
        case USB_DEVICE_EP0_STATE_DATA_OUT:/*RX*/
             switch(_usbDeviceDatabase.ep0.operation)
             {
             case USB_DEVICE_EP0_RECEIVED_DATA_PACKET:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_OUT;
                 break;
             case USB_DEVICE_EP0_RECEIVED_LAST_DATA_PACKET:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
                 break;
             default:
                USB_DEVICE_ASSERT(0); //wrong operation
                break;
             }
            break;

        case USB_DEVICE_EP0_STATE_DATA_IN:/*TX*/
            switch(_usbDeviceDatabase.ep0.operation)
             {
             case USB_DEVICE_EP0_TRANSMITTED_DATA_PACKET:
             case USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
                 break;
             case USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET_NOTIFY:
                 _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
                 break;
             default:
                USB_DEVICE_ASSERT(0); //wrong operation
                break;
             }
            break;

        case USB_DEVICE_EP0_STATE_STATUS: //must receive ZLP opposite in the transaction direction
            if(data_dir == USBCDevice_DataStageDirectionH2D)
            {
                USB_DEVICE_ASSERT(_usbDeviceDatabase.ep0.operation == USB_DEVICE_EP0_TRANSMITTED_LAST_DATA_PACKET_NOTIFY);
            }
            else //USBCDevice_DataStageDirectionD2H
            {
                USB_DEVICE_ASSERT(_usbDeviceDatabase.ep0.operation == USB_DEVICE_EP0_RECEIVED_LAST_DATA_PACKET);
            }
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_IDLE;
            break;

         default:
            USB_DEVICE_ASSERT(0);
            break;
    }

}

/*******************************************************************************
 *                      E X P O R T E D   F U N C T I O N S
 ******************************************************************************/

/*******************************************************************************
 * Function:    USB1DeviceEP0InterruptHandler
 *******************************************************************************
 * Description: EP0 interrupt handler
 *
 * Parameters:  statusBits      - interrupt status bits
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceEP0InterruptHandler(UINT32 udccsr0Reg)
{
    /* if SETUP cmd received - even in middle of other transactions
     * need to move into IDLE state - could be pre-mature state which is valid */
    if( UDC_EP0_IS_SETUP_CMD(udccsr0Reg) )
    {
        _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_IDLE;
    }

    _usbDeviceDatabase.ep0.ep0_int_cnt++;

    switch(_usbDeviceDatabase.ep0.state) //check current state
    {
        case USB_DEVICE_EP0_STATE_IDLE:
            USB1DeviceEP0ProcessIdleState(udccsr0Reg);
            USB1DeviceEP0TransitState();
            //process setup packet
            USB1DeviceEP0ProcessSetupCmd();
            break;
        case USB_DEVICE_EP0_STATE_SETUP:
            USB1DeviceEP0ProcessSetupState(udccsr0Reg);
            USB1DeviceEP0TransitState();
            USB1DeviceEP0DataOutIsOverNotify();//must do it after state is changed into status
            break;
        case USB_DEVICE_EP0_STATE_DATA_OUT:/*RX*/
            USB1DeviceEP0ProcessOutDataState(udccsr0Reg);
            USB1DeviceEP0TransitState();
            USB1DeviceEP0DataOutIsOverNotify();;//must do it after state is changed into status
            break;
        case USB_DEVICE_EP0_STATE_DATA_IN:/*TX*/
            USB1DeviceEP0ProcessInDataState(udccsr0Reg);
            USB1DeviceEP0TransitState();
            break;
        case USB_DEVICE_EP0_STATE_STATUS:
            USB1DeviceEP0ProcessStatusState(udccsr0Reg);
            break;
        default:
            USB_DEVICE_ASSERT(0);
            break;
    }

    //UDC_CLEAR_INTER_ST_0(UDC_ENDPOINT_0, statusBits); YG - done in driver

} /* End of < USB1DeviceEP0InterruptHandler > */


/*******************************************************************************
 * Function:    USB1DeviceEventNotify
 *******************************************************************************
 * Description: Handle event changes
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceEventNotify(UDC_DeviceEventE event, UINT32 param)
{
    switch(event)
    {
        case UDC_EVENT_RESET:
            USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_RESET);
            break;

        case UDC_EVENT_SUSPEND:
            USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_SUSPEND);
            break;

        case UDC_EVENT_RESUME:
            USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_RESUME);
            break;

        case UDC_EVENT_CONFIG_CHANGE:
            USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_CONFIG_CHANGE);
            break;
    }
} /* End of < USB1DeviceEventNotify > */

/*******************************************************************************
 * Function:    USB1DeviceMultiTransmitNotifyFn
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceMultiTransmitNotifyFn(UINT8           usbEndpointNum,
                                    UINT32          numOfItems,
                                    BOOL            endOfChain)
{
} /* End of < USB1DeviceMultiTransmitNotifyFn > */

/*******************************************************************************
 * Function:    USB1DeviceReceiveNotifyFn
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceReceiveNotifyFn(USBDevice_EndpointE          endpoint,
                              UINT8                        *pRxBuffer,
                              UINT32                       length,
                              BOOL                         endOfMessage)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];


    pEndpoint->endpointBusy     = (endOfMessage == FALSE);  //if not end of message still busy
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_RX_COMPLETED;

    pEndpoint->transactionCompletedNotifyFn(endpoint, pRxBuffer, length, endOfMessage);

} /* End of < USB1DeviceReceiveNotifyFn > */

/*******************************************************************************
 * Function:    USB1DeviceTransmitNotifyFn
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceTransmitNotifyFn(UINT8            usbEndpointNum,
                               UINT8            *pTxBuffer,
                               UINT32           length,
                               BOOL              end_of_transmit)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[usbEndpointNum];

    if(end_of_transmit)
        pEndpoint->endpointBusy     = FALSE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_NOT_BUSY;

    pEndpoint->transactionCompletedNotifyFn((USBDevice_EndpointE)usbEndpointNum, pTxBuffer, length, end_of_transmit);

} /* End of < USB1DeviceTransmitNotifyFn > */

/*******************************************************************************
 * Function:    USB1DeviceCableDetectionNotify
 *******************************************************************************
 * Description: EP0 interrupt handler
 *
 * Parameters:  cableStatus     - cable status
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DeviceCableDetectionNotify(USBCable_StatusE cableStatus)
{
    if(cableStatus == USB_CABLE_OUT)
    {/*USB_CABLE_OUT - USB OFF */

        USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_CABLE_OUT);

    }
    else
    {/*USB_CABLE_IN - USB ON */

       USB1DeviceStatusChange(USB_DEVICE_ST_CHANGE_CABLE_IN);
    }


} /* End of < USB1DeviceCableDetectionNotify > */




/*******************************************************************************
 *                      A P I ' S    F U N C T I O N S
 ******************************************************************************/

/*******************************************************************************
 * Function:    USB1DeviceVendorClassResponse
 *******************************************************************************
 * Description: Response function for Vendor/Class request
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes: Can be called only as a respinse to VendorClassReq callback
 *
 ******************************************************************************/
void USB1DeviceVendorClassResponse(UINT8                             *pBuffer,
                                  UINT16                            bufferLength)
{
      /* no re-entrancy allowed for same endpoint !!! */
    UINT32 cpsrReg = disableInterrupts(); /* lock */

    BOOL response_is_legal;


    USB_SetupCmdS *p_setup_packet = (USB_SetupCmdS *)_usbDeviceDatabase.ep0.ctrl_setup_buff;
    USBCDevice_DataStageDirectionE data_dir = USB_DEVICE_EP0_GET_TRANSACTION_DIR(p_setup_packet->bmRequestType);

    //Host is sending only setup packet and no data stage, user was notified and has to return empty packet
    BOOL status_after_setup = (_usbDeviceDatabase.ep0.state == USB_DEVICE_EP0_STATE_SETUP && p_setup_packet->wLength==0 && data_dir ==  USBCDevice_DataStageDirectionH2D );

    //Host requested data from dvice , user was notified, and must return packet in length equa to requested by host
    BOOL start_data_in = (_usbDeviceDatabase.ep0.state == USB_DEVICE_EP0_STATE_SETUP && p_setup_packet->wLength && data_dir ==  USBCDevice_DataStageDirectionD2H );

    //host sent data to device, user was notified and has to return empty packet
    BOOL status_after_data_out = (_usbDeviceDatabase.ep0.state == USB_DEVICE_EP0_STATE_STATUS && p_setup_packet->wLength && data_dir ==  USBCDevice_DataStageDirectionH2D );


    response_is_legal = (start_data_in || status_after_data_out || status_after_setup);

    if(response_is_legal) //ignore illegal responses
    {
        if(status_after_setup || status_after_data_out)
            USB_DEVICE_ASSERT(pBuffer == 0 && bufferLength == 0); //IN empty packet is returned by user

        if(start_data_in)
            USB_DEVICE_ASSERT(bufferLength == p_setup_packet->wLength); //IN empty packet is returned by user


        _usbDeviceDatabase.ep0.ctrl_in_data_stage_buff              = pBuffer;
        _usbDeviceDatabase.ep0.ctrl_in_data_stage_total_length      = bufferLength;
        _usbDeviceDatabase.ep0.ctrl_in_data_stage_current_length    = 0;


        USB1DeviceEP0HandleTransmitting();
    }

    restoreInterrupts(cpsrReg); /* unlock */
} /* End of < USB1DeviceVendorClassResponse > */

/*******************************************************************************
 * Function:    USB1DeviceEndpointReceiveCompleted
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointReceiveCompleted(USBDevice_EndpointE          endpoint)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];

    UDCDriverEndpointSetupOUT(pEndpoint->udcEndpoint, 0, 0,TRUE);


    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointReceiveCompleted > */

/*******************************************************************************
 * Function:    USB1DeviceEndpointReceiveCompletedExt
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  USBDevice_EndpointE          endpoint
 *
 * Output:      UINT32 next_packet_expected_length
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointReceiveCompletedExt(USBDevice_EndpointE   endpoint, UINT8  *pRxBuffer,UINT32 next_packet_expected_length,BOOL expect_zlp)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];

    UDCDriverEndpointSetupOUT(pEndpoint->udcEndpoint, pRxBuffer, next_packet_expected_length,expect_zlp);


    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointReceiveCompleted > */


/*******************************************************************************
 * Function:    USB1DeviceEndpointMultiTransmit
 *******************************************************************************
 * Description: Transmit data through endpoint
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *              pTxBuffer       - buffer to transmit
 *              length          - length of buffer
 *              autoZLP         - whether to send ZLP automatically or not
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointMultiTransmit(USBDevice_EndpointE            endpoint,
                                                     UINT32                         numOfBuffers,
                                                     USBDevice_MultiTransmitListS   *multiList)
{
    unsigned long           cpsrReg;
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];

    DIAG_RAM_REPORT_FORMATTED_INDEX_2P(usbErrorDebug, 0x30, 0xAA);

    /* check for USB line status */
    if(_usbDeviceDatabase.status != USB_DEVICE_STATUS_CONNECTED)
        return USB_DEVICE_RC_NOT_CONNECTED;

    /* check for endpoint  */
    if(!pEndpoint->endpointOpen)
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;


    /* no re-entrance is allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_TX_STARTED;
    restoreInterrupts(cpsrReg); /* unlock */

    /* Prepare the hardware (DMA) for Transmit (IN) */
    if(UDCDriverEndpointSetupINMultiTransmitWithDma(pEndpoint->udcEndpoint,numOfBuffers,multiList) != USB_DEVICE_RC_OK)
    {
        /* error result */
        pEndpoint->endpointBusy     = FALSE;
        //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_NOT_BUSY;

        return USB_DEVICE_RC_DMA_ERROR;
    }

    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointMultiTransmit > */

/*******************************************************************************
 * Function:    USBDeviceEndpointReceive
 *******************************************************************************
 * Description: Prepare an endpoint for receive
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointReceive(USBDevice_EndpointE         endpoint,
                                               UINT8                        *pRxBuffer,
                                               UINT32                       length,
                                               BOOL                         expect_zlp)
{
    unsigned long           cpsrReg;
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];

    DIAG_RAM_REPORT_FORMATTED_INDEX_2P(usbErrorDebug, 0x30, 0xAA);

    /* check for endpoint  */
    if(!pEndpoint->endpointOpen)
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;

    /* sanity check for length */
    USB_DEVICE_ASSERT(length != 0);

    /* no re-entrancy allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_RX_STARTED;
    restoreInterrupts(cpsrReg); /* unlock */

    /* Prepare the hardware (Interrupt or DMA) for Receive (OUT) */
    if( !UDCDriverEndpointSetupOUT(pEndpoint->udcEndpoint, pRxBuffer, length,expect_zlp) )
    {/* check for error */
        pEndpoint->endpointBusy     = FALSE;
        //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_NOT_BUSY;

        return USB_DEVICE_RC_DMA_ERROR;
    }

    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointReceive > */

/*******************************************************************************
 * Function:    USB1DeviceEndpointTransmit
 *******************************************************************************
 * Description: Transmit data through endpoint
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *              pTxBuffer       - buffer to transmit
 *              length          - length of buffer
 *              autoZLP         - whether to send ZLP automatically or not
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointTransmit(USBDevice_EndpointE     endpoint,
                                                UINT8                   *pTxBuffer,
                                                UINT32                  txLength,
                                                BOOL                    autoZLP)
{
    unsigned long           cpsrReg;
    USBCDevice_ReturnCodeE   result;
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];

    DIAG_RAM_REPORT_FORMATTED_INDEX_2P(usbErrorDebug, 0x30, 0xAA);

    /* check for USB line status */
    if(_usbDeviceDatabase.status != USB_DEVICE_STATUS_CONNECTED)
    {
        return USB_DEVICE_RC_NOT_CONNECTED;
    }

    /* check for endpoint  */
    if(!pEndpoint->endpointOpen)
    {
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;
    }


    /* sanity check for length */
    USB_DEVICE_ASSERT(txLength != 0);

    /* no re-entrance is allowed for same endpoint !!! */
    cpsrReg = disableInterrupts(); /* lock */

    if( pEndpoint->endpointBusy )
    {
        restoreInterrupts(cpsrReg); /* unlock */
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }

    pEndpoint->endpointBusy     = TRUE;
    //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_TX_STARTED;
    restoreInterrupts(cpsrReg); /* unlock */

    /* Prepare the hardware (Interrupt or DMA) for Transmit (IN) */
    result = (UDCDriverEndpointSetupIN(pEndpoint->udcEndpoint, pTxBuffer, txLength, autoZLP) == TRUE ? USB_DEVICE_RC_OK : USB_DEVICE_RC_ERROR);
    if(result != USB_DEVICE_RC_OK)
    {/* error result */
        pEndpoint->endpointBusy     = FALSE;
        //pEndpoint->transferState    = USB_DEVICE_TRANSFER_STATE_NOT_BUSY;

       // return result;
    }

    return result;
} /* End of < USB1DeviceEndpointTransmit > */

/*******************************************************************************
 * Function:    USB1DeviceEndpointAbort
 *******************************************************************************
 * Description: Abort any endpoint activity
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointAbort(USBDevice_EndpointE endpoint)
{
    if( _usbDeviceEndpoint[endpoint].endpointOpen)
        UDCDriverEndpointFlush(_usbDeviceEndpoint[endpoint].udcEndpoint);

    _usbDeviceEndpoint[endpoint].endpointBusy = FALSE;

    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointAbort > */

/*******************************************************************************
 * Function:    USB1DeviceEndpointStall
 *******************************************************************************
 * Description: stall endpoint
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *
 * Output:      none
 *
 * Returns:     USB_DEVICE_RC_OK if stall is entered and cleared USB_DEVICE_RC_ERROR otherwise
 *
 * Notes: can only be called from task,  blocked.
 ******************************************************************************/
USBCDevice_ReturnCodeE  USB1DeviceEndpointStall(USBDevice_EndpointE endpoint)
{
//    unsigned long           cpsrReg;
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
    volatile int k=0,j=0,loop_cnt=0;
    UINT32 max_loop_cnt = 10000; //it is measured that it takes about 1.8-2 milli till EP enters STALL state
	UINT32   		clkTimeStart, clkTime;

/* --> currently ignoring busy - believes that stall precede every action - need to check if required
    // no re-entrance  is allowed for same endpoint !!!
    cpsrReg = disableInterrupts(); // lock


    if( pEndpoint->endpointBusy)
    {
        restoreInterrupts(cpsrReg); // unlock
        return USB_DEVICE_RC_ENDPOINT_BUSY;
    }
*/

	UDCDriverEndpointStall(pEndpoint->udcEndpoint);

	// need to wait a while for the STALL to leave UE otherwise it'w ruin IN packet.
    while( !UDCDriverEndpointClearStall(pEndpoint->udcEndpoint) && (loop_cnt++ < max_loop_cnt) )
    {
		clkTimeStart = cp14ReadCCNT();
		do
		{
		   clkTime = (cp14ReadCCNT() - clkTimeStart) / 0x5;	// converting to Micro Sec.

		} while ( clkTime < 10 );         // < 10 micro sec.
    }


	return(USB_DEVICE_RC_OK);
}
/*******************************************************************************
 * Function:    USB1DeviceEndpointClearStall
 *******************************************************************************
 * Description: clear stall-condition endpoint
 *
 * Parameters:  endpoint        - handle to udcEndpoint
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
/*USBCDevice_ReturnCodeE         USB1DeviceEndpointClearStall(USBDevice_EndpointE endpoint)
{
    USBDevice_EndpointDataS *pEndpoint = &_usbDeviceEndpoint[endpoint];
	int i=0, j=0;

    while (!UDCDriverEndpointClearStall(pEndpoint->udcEndpoint))
    {
		if(i++ > 100000)
		{
			i = 0;
			if(j++ > 100)
			{
				USB_DEVICE_ASSERT(FALSE);//YG - are we getting to this?
			}
		}
	}

    return(USB_DEVICE_RC_OK);
}*/

/*******************************************************************************
 * Function:    USB1DeviceEndpointClose
 *******************************************************************************
 * Description: Open endpoint for usage
 *
 * Parameters:  endpoint        - endpoint number
 *              usageType       - usage type (DMA / Interrupt)
 *              dmaChannel      - in case of DMA this is channel number
 *
 * Output:      none
 *
 * Returns:     USBCDevice_ReturnCodeE - return code
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointClose(USBDevice_EndpointE endpoint)
{
    /* check if already open */
    if( !_usbDeviceEndpoint[endpoint].endpointOpen )
    {
        return USB_DEVICE_RC_ENDPOINT_NOT_OPENED;
    }

    UDCDriverEndpointDeactivate(_usbDeviceEndpoint[endpoint].udcEndpoint);

    _usbDeviceEndpoint[endpoint].endpointOpen                   = FALSE;
    _usbDeviceEndpoint[endpoint].transactionCompletedNotifyFn   = NULL;

    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointClose > */

/*******************************************************************************
 * Function:    USB1DeviceEndpointOpen
 *******************************************************************************
 * Description: Open endpoint for usage
 *
 * Parameters:  endpoint            - endpoint number
 *              usageType           - usage type (DMA / Interrupt)
 *              dmaChannel          - in case of DMA this is channel number
 *              *pDescBuffer        - pointer to descriptor buffer
 *              descBufferLength    - descriptor buffer length
 *              transactionCompletedNotifyFn - transaction completed notify function
 *
 * Output:      none
 *
 * Returns:     USBCDevice_ReturnCodeE - return code
 *
 * Notes:
 ******************************************************************************/
USBCDevice_ReturnCodeE USB1DeviceEndpointOpen(USBDevice_EndpointE                       endpoint,
                                            USBDevice_UsageTypeE                      usageType,
                                            UINT8                                     dmaChannel,
                                            UINT8                                     *pDescBuffer,
                                            UINT16                                    descBufferLength,
                                            USBDeviceTransactionCompletedNotifyFn     transactionCompletedNotifyFn)
{
    BOOL                result;

    /* check if already open */
    if(_usbDeviceEndpoint[endpoint].endpointOpen)
    {
        return USB_DEVICE_RC_ENDPOINT_IN_USE;
    }

    /* convert USB endpoint into UDC endpoint */
    _usbDeviceEndpoint[endpoint].udcEndpoint = USBDeviceConvertUSB2UDCEndpoint(endpoint);

    /* if endpoint not found this means it is not in configuration */
    if(_usbDeviceEndpoint[endpoint].udcEndpoint >= UDC_TOTAL_ENDPOINTS)
    {
        return USB_DEVICE_RC_ENDPOINT_NOT_IN_CONFIG;
    }

    /* act upon usage type */
    if ((usageType & USB_DEVICE_USAGE_DMA) == USB_DEVICE_USAGE_DMA)
    {/* DMA  */
        if(dmaChannel > USB_DEVICE_MAX_DMA_CHANNEL)
        {
            return USB_DEVICE_RC_DMA_ERROR;
        }

        result = UDCDriverEndpointActivate(_usbDeviceEndpoint[endpoint].udcEndpoint, endpoint,
                                              TRUE, dmaChannel, pDescBuffer, descBufferLength);
    }
    else
    {/* interrupt */
        result = UDCDriverEndpointActivate(_usbDeviceEndpoint[endpoint].udcEndpoint, endpoint,
                                              FALSE, 0, NULL, 0);
    }

    if( !result )
    {
        return USB_DEVICE_RC_OPEN_ERROR;
    }

    _usbDeviceEndpoint[endpoint].endpointOpen                   = TRUE;
    _usbDeviceEndpoint[endpoint].transactionCompletedNotifyFn   = transactionCompletedNotifyFn;

    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceEndpointOpen > */

/*******************************************************************************
 * Function:    USB1DeviceRegister
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

USBCDevice_ReturnCodeE USB1DeviceRegister(USBDeviceStatusNotifyFn                 statusNotifyFn,
                                         USBDeviceVendorClassRequestNotifyFn     vendorClassRequestNotifyFn,
                                         USBDeviceEndpointZeroNotifyFn           endpointZeroNotifyFn)
{
    statusNotifyFn_Patch = statusNotifyFn;
    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceRegister > */



/*******************************************************************************
 * Function:    USB1DeviceRegister
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
USBCDevice_ReturnCodeE USB1DeviceRegisterPatchTempFunc(USBDeviceStatusNotifyFn                 statusNotifyFn,
                                         USBDeviceVendorClassRequestNotifyFn     vendorClassRequestNotifyFn,
                                         USBDeviceEndpointZeroNotifyFn           endpointZeroNotifyFn)
{
    _usbDeviceDatabase.statusNotifyFn                       = statusNotifyFn;
    _usbDeviceDatabase.vendorClassRequestNotifyFn           = vendorClassRequestNotifyFn;
    _usbDeviceDatabase.endpointZeroNotifyFn                 = endpointZeroNotifyFn;
    return USB_DEVICE_RC_OK;
} /* End of < USB1DeviceRegister > */

/*******************************************************************************
 * Function:    USB1DevicePhase2Init
 *******************************************************************************
 * Description: Phase 2 of initialization process
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void USB1DevicePhase2Init(void)
{
#if defined MV_USB_TRACE_PRINT
    UsbDeviceSetupUARTforDebug();
#endif
    if( !UDCDriverPhase2Init() )
    {
        USB_DEVICE_ASSERT(FALSE);//YG
        return;
    }
} /* End of < USB1DevicePhase2Init > */



/*******************************************************************************
 * Function:    USB1DeviceEndpointGetHWCfg
 *******************************************************************************
 * Description:  returns endpoint HW configuration
 *
 *
 * Parameters:
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
UDC_EndpointConfigS* USB1DeviceEndpointGetHWCfg(USBDevice_EndpointE endpoint)
{
    return (UDC_EndpointConfigS*)(&(_u1dcDefaultEndpointConfig[USBDeviceConvertUSB2UDCEndpoint(endpoint)]));
}


/*******************************************************************************
 * Function:    USB1DeviceIsControllerEnabled
 *******************************************************************************
 * Description:  returns USB Device controller state
 *
 *
 * Parameters:
 *
 * Output:      none
 *
 * Returns:     return TRUE if USB Device controller is active, FALSE otherwise
 *
 * Notes:
 ******************************************************************************/
BOOL USB1DeviceIsControllerEnabled (void)
{
    return UDCDriverIsDeviceControllerEnabled ();
}


/*******************************************************************************
 * Function:    USB1DevicePhase1Init
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
void USB1DevicePhase1Init(void)
{
    USBDevicePhase2Init             = USB1DevicePhase2Init;
    USBDeviceEndpointGetHWCfg       = USB1DeviceEndpointGetHWCfg;
    USBDeviceIsControllerEnabled    = USB1DeviceIsControllerEnabled;
    USBDeviceEndpointStall          = USB1DeviceEndpointStall;
    USBDeviceEndpointOpen           = USB1DeviceEndpointOpen;
	USBDeviceEndpointClose			= USB1DeviceEndpointClose;
	USBDeviceEndpointAbort			= USB1DeviceEndpointAbort;
	USBDeviceVendorClassResponse	= USB1DeviceVendorClassResponse;
	USBDeviceEndpointReceive		= USB1DeviceEndpointReceive;
	USBDeviceEndpointReceiveCompletedExt = USB1DeviceEndpointReceiveCompletedExt;
	USBDeviceEndpointReceiveCompleted = USB1DeviceEndpointReceiveCompleted;
	USBDeviceEndpointTransmit 	    = USB1DeviceEndpointTransmit;
    USBDeviceEndpointMultiTransmit  = USB1DeviceEndpointMultiTransmit;
    USBDeviceCableDetectionNotify   = USB1DeviceCableDetectionNotify;

#if defined (_TAVOR_BOERNE_)
    PRMManage(PRM_SRVC_UDC,PRM_RSRC_ALLOC);
#endif

    USB1DeviceDatabaseReset(TRUE);

    UDCDriverPhase1Init();


} /* End of < USB1DevicePhase1Init > */
