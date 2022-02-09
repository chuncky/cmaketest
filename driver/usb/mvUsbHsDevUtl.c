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
***  This file contains the VUSB_HS Device Controller interface functions.
***
**************************************************************************
**END*********************************************************/

#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"
#include "udc_config.h"

#define  MV_USB_TEST_MODE_TEST_PACKET_LENGTH   (53)

/* Test packet for Test Mode : TEST_PACKET. USB 2.0 Specification section 7.1.20 */
uint_8 test_packet[MV_USB_TEST_MODE_TEST_PACKET_LENGTH] =
{
   /* Synch */
   /* DATA 0 PID */
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
   0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
   0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
   0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
   0xEF, 0xF7, 0xFB, 0xFD, 0x7E
};

extern const UDC_EndpointConfigS* udcDefaultEndpointCfg;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_assert_resume
*  Returned Value : None
*  Comments       :
*        Resume signalling for remote wakeup
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_assert_resume
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   uint_32                                      temp;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Assert the Resume signal */
   temp = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/);
   temp &= ~EHCI_PORTSCX_W1C_BITS;
   temp |= EHCI_PORTSCX_PORT_FORCE_RESUME;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/ = USB_32BIT_LE(temp);

   /* Port change interrupt will be asserted at the end of resume
   ** operation
   */

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_stall_endpoint
*  Returned Value : None
*  Comments       :
*        Stalls the specified endpoint
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_stall_endpoint
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR             usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_    ep_queue_head_ptr;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Get the endpoint queue head address */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR +
                                                                    2*ep_num + direction;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STALL,
                    "STALL ep=%d %s: EPCTRLX=0x%x, CURR_dTD=0x%x, NEXT_dTD=0x%x, SIZE=0x%x\n",
                    ep_num, direction ? "SEND" : "RECV",
                    (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num]),
                    (unsigned)USB_32BIT_LE(ep_queue_head_ptr->CURR_DTD_PTR),
                    (unsigned)USB_32BIT_LE(ep_queue_head_ptr->NEXT_DTD_PTR),
                    (unsigned)USB_32BIT_LE(ep_queue_head_ptr->SIZE_IOC_INT_STS));

   /* Stall the endpoint for Rx or Tx and set the endpoint type */
   if (ep_queue_head_ptr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS)
   {
      /* This is a control endpoint so STALL both directions */
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |=
         USB_32BIT_LE((EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL));
   }
   else
   {
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |=
         USB_32BIT_LE((direction ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL));
   } /* Endif */

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_unstall_endpoint
*  Returned Value : None
*  Comments       :
*        Unstall the specified endpoint in the specified direction
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_unstall_endpoint
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR        dev_ptr;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_UNSTALL,/*MV_USB_DEBUG_FLAG_STALL,*/ "UNSTALL_ENDPOINT: ep=%d %s\n",
                    ep_num, direction ? "SEND" : "RECV");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Enable the endpoint for Rx or Tx and set the endpoint type */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] &=
      USB_32BIT_LE(direction ? ~EHCI_EPCTRL_TX_EP_STALL : ~EHCI_EPCTRL_RX_EP_STALL);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_vusb20_get_endpoint_status
* Returned Value : None
* Comments       :
*     Gets the endpoint status
*
*END*--------------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_get_endpoint_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,

      /* [IN] Endpoint to get */
      uint_8               ep
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   uint_8                            ep_num = ep & 0x0f;
   uint_8                            ep_dir = (uint_8)udcDefaultEndpointCfg[ep_num].usbEndpointDir;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRACE, "get_endpoint_status\n");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   return ((USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num]) &
      (ep_dir ? EHCI_EPCTRL_TX_EP_STALL : EHCI_EPCTRL_RX_EP_STALL)) ? 1 : 0);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_vusb20_set_test_mode
* Returned Value : None
* Comments       :
*     sets/resets the test mode
*
*END*--------------------------------------------------------------------*/
void _usb_dci_vusb20_set_test_mode
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle handle,

      /* [IN] Test mode */
      uint_16 test_mode
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      temp;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ANY, "set_test_mode\n");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   temp = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0]);

   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0] =
                                USB_32BIT_LE((temp | EHCI_EPCTRL_TX_DATA_TOGGLE_RST));

   if (test_mode == MV_USB_TEST_MODE_TEST_PACKET)
   {
      _usb_device_send_data(handle, 0, test_packet, MV_USB_TEST_MODE_TEST_PACKET_LENGTH);
   } /* Endif */

   temp = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/);
   temp &= ~EHCI_PORTSCX_W1C_BITS;

   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/ =
                                    USB_32BIT_LE(temp | ((uint_32)test_mode << 8));

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_dci_vusb20_set_endpoint_status
* Returned Value : None
* Comments       :
*     Sets the endpoint registers e.g. to enable TX, RX, control
*
*END*--------------------------------------------------------------------*/
void _usb_dci_vusb20_set_endpoint_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,

      /* [IN] Endpoint to set */
      uint_8               ep,

      /* [IN] Endpoint characteristics */
      uint_8               stall
   )
{ /* Body */

    USB_DEV_STATE_STRUCT_PTR          usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR             dev_ptr;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INIT,
                    "set_endpoint_status: ep=%d, stall=%d\n", ep, stall);

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    if (stall)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO,"STALL RECV and SEND ep=%d \n", ep);
        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep] |=
                        USB_32BIT_LE(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL);
    }
    else
    {
        int                                 direction = 0;
        VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_   ep_queue_head_ptr;

        while(direction < 2)
        {
            ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR +
                                                                                2*ep + direction;

            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STALL,
                    "UNSTALL ep=%d %s: EPCTRLX=0x%x, CURR_dTD=0x%x, NEXT_dTD=0x%x, SIZE=0x%x\n",
                        ep, direction ? "SEND" : "RECV",
                    (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep]),
                    (unsigned)USB_32BIT_LE(ep_queue_head_ptr->CURR_DTD_PTR),
                    (unsigned)USB_32BIT_LE(ep_queue_head_ptr->NEXT_DTD_PTR),
                    (unsigned)USB_32BIT_LE(ep_queue_head_ptr->SIZE_IOC_INT_STS));
            direction++;
        }

        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep] &=
                        ~(USB_32BIT_LE(EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL));

        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep] |= 
                        (USB_32BIT_LE(EHCI_EPCTRL_TX_DATA_TOGGLE_RST | EHCI_EPCTRL_RX_DATA_TOGGLE_RST));

    } /* Endif */
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_reset_data_toggle
*  Returned Value : None
*  Comments       :
*        Reset the data toggle for the specified endpoint in the
*  specified direction
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_reset_data_toggle
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_            ep_queue_head_ptr;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_reset_data_toggle");
   #endif

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Get the endpoint queue head address */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR +
      2*ep_num + direction;

   /* Reset the data toggle for this endpoint. */
   if (ep_queue_head_ptr->MAX_PKT_LENGTH & VUSB_EP_QUEUE_HEAD_IOS) {
      /* This is a control endpoint so reset both directions */
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |=
         (EHCI_EPCTRL_TX_DATA_TOGGLE_RST | EHCI_EPCTRL_RX_DATA_TOGGLE_RST);
   } else {
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |=
         (direction ? EHCI_EPCTRL_TX_DATA_TOGGLE_RST :
         EHCI_EPCTRL_RX_DATA_TOGGLE_RST);
   } /* Endif */


   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_dci_vusb20_reset_data_toggle, SUCCESSFUL");
   #endif

} /* EndBody */
