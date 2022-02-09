/*******************************************************************************
** File          : $HeadURL$
** Author        : $Author$
** Project       : HSCTRL
** Instances     :
** Creation date :
********************************************************************************
********************************************************************************
** ChipIdea Microelectronica - IPCS
** TECMAIA, Rua Eng. Frederico Ulrich, n 2650
** 4470-920 MOREIRA MAIA
** Portugal
** Tel: +351 229471010
** Fax: +351 229471011
** e_mail: chipidea.com
********************************************************************************
** ISO 9001:2000 - Certified Company
** (C) 2005 Copyright Chipidea(R)
** Chipidea(R) - Microelectronica, S.A. reserves the right to make changes to
** the information contained herein without notice. No liability shall be
** incurred as a result of its use or application.
********************************************************************************
** Modification history:
** $Date$
** $Revision$
*******************************************************************************
*** Description:
***  This file contains USB device API specific function to receive
***  data.
***
**************************************************************************
**END*********************************************************/
#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"

#include "usb_device_types.h"  // for _u2dcDefaultEndpointConfig definition

#include "usbTrace.h"

#define ErrorLogPrintf uart_printf


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_recv_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Receives data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_recv_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] buffer to receive data */
      uint_8_ptr                 buff_ptr,

      /* [IN] length of the transfer */
      uint_32                    size
   )
{ /* Body */
    int                              lockKey;
    uint_8                           error = USB_OK;
    XD_STRUCT_PTR                    xd_ptr;
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_RX, "recv_data: ep=%d, buf_ptr=0x%x, size=%d\n",
                                       ep_num, (unsigned)buff_ptr, (int)size);

    MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_recv_count++));

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    if(buff_ptr != NULL)
    {
        USB_dcache_inv((pointer)buff_ptr,size);
    }

    lockKey = USB_lock();

    if (!usb_dev_ptr->XD_ENTRIES)
    {
        ErrorLogPrintf("%s: No free XD entries", __FUNCTION__);
        _usb_dci_vusb20_get_dtd_information();

        USB_unlock(lockKey);
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_recv_data, transfer in progress\n");

        USB2MgrDeviceUnplugPlug();
        return USB_OK;

    } /* Endif */

    /* Get a transfer descriptor for the specified endpoint
    ** and direction
    */
    USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);

    usb_dev_ptr->XD_ENTRIES--;

    /* Initialize the new transfer descriptor */
    xd_ptr->EP_NUM = ep_num;
    xd_ptr->BDIRECTION = MV_USB_RECV;
    xd_ptr->WTOTALLENGTH = size;
    xd_ptr->WSOFAR = 0;
    xd_ptr->WSTARTADDRESS = buff_ptr;

    xd_ptr->BSTATUS = MV_USB_STATUS_TRANSFER_ACCEPTED;

    error = _usb_dci_vusb20_add_dTD(handle, xd_ptr);

    USB_unlock(lockKey);

    if (error)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_recv_data, receive failed\n");

        ErrorLogPrintf("Rev failed 0x%x", error);
        return USBERR_RX_FAILED;
    } /* Endif */

   _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_OUT;
    _usbDeviceDatabase.ep0.ep0_OUT_cnt++;
    return error;

} /* EndBody */
