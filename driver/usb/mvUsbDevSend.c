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
***  This file contains USB device API specific function to send data.
***
**************************************************************************
**END*********************************************************/

#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"

#include "usb_device_types.h"  // for usbDeviceDatabase definition

#include "usbTrace.h"

#define fatal_printf uart_printf
#define ErrorLogPrintf uart_printf

#define USB_SULOG_BASE				0xD4208000	 
#define USB_SULOG_CTRL_OFFSET		0x094
#define USB_SULOG_DQH_1W			0x098
#define USB_SULOG_DQH_2W			0x09c
#define USB_SULOG_DQH_3W			0x0a0
#define USB_SULOG_DQH_4W			0x0a4
#define USB_SULOG_DTD_1W			0x0a8
#define USB_SULOG_DTD_2W			0x0ac
#define USB_SULOG_DTD_3W			0x0b0
#define USB_SULOG_DTD_4W			0x0b4
#define USB_SULOG_DTD_5W            0x0b8
UINT32 ep_queue_head_ptr_g = 0;
uint_8 
_sulog_hw_mode_init(_usb_device_handle handle ,INT32 sulog_output_port)
{

	uint_8						 error;
	VUSB20_EP_TR_STRUCT_PTR          dTD_ptr;
	USB_DEV_STATE_STRUCT_PTR	  usb_dev_ptr;
	VUSB20_EP_QUEUE_HEAD_STRUCT_PTR  ep_queue_head_ptr;
	usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
	uint_32 temp;
	uart_printf("log usb init ---\n");
	//OSATaskSleep(200 * 1);
    /* Get a dTD from the queue */
    EHCI_DTD_QGET(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, dTD_ptr);
   	//add this dtd to usb sulog register
   	uart_printf("log usb init2 ---\n");
	//OSATaskSleep(200 * 1);
	{
       	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_1W) = 1;
    	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_2W) = (0x2fd0 << 16) | 0x0080;// 510*3*64bit 0x2fd0
    	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_3W) = 0;
    	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_4W) = 0x1000;
    	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_5W) = 0x2000;
	}
	uart_printf("log usb init3 ---:%x:%x:%x:%x\n",*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_1W),
		                                          *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_2W),
		                                          *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_3W),
		                                          *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DTD_4W));
	//OSATaskSleep(200 * 1);
	//add dqh to usb sulog register
	temp = (2*sulog_output_port+ MV_USB_SEND);

	ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR;
    ep_queue_head_ptr_g = (UINT32)ep_queue_head_ptr;
	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_1W) = 0;
	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_2W) = (UINT32)dTD_ptr;
	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_3W) = 0;
	*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_4W) = 0;
	uart_printf("log usb init4 ---%x:%x:%x:%x\n", *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_1W),
		                                          *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_2W),
		                                          *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_3W),
		                                          *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_DQH_4W));
	 //OSATaskSleep(200 * 1);
	 //*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_CTRL_OFFSET) = ((UINT32)ep_queue_head_ptr & (~0x1F))| (sulog_output_port & 0xf)<<1 | 0<<0;//port c
	 uart_printf("log usb init5 ---\n");
	 //OSATaskSleep(200 * 1);
	 return 0;
	

}
uint_8 
_sulog_hw_mode_start(_usb_device_handle handle ,INT32 sulog_output_port){
    //start sulog hw mode
    *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_CTRL_OFFSET) = (((UINT32)ep_queue_head_ptr_g & (~0x1F))| (sulog_output_port & 0xf)<<1) | 0x1 | 0x1 << 5;
	return 0;	
}
uint_8 
_sulog_hw_mode_stop(_usb_device_handle handle ,INT32 sulog_output_port){
    //start sulog hw mode
    *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_CTRL_OFFSET) = (((UINT32)ep_queue_head_ptr_g & (~0x1F))| (sulog_output_port & 0xf)<<1) &(~0x1) | 0x1 << 5;
	return 0;
}

UINT32 sulog_status(void)
{
	int i = 0;
	uart_printf("sulog: 0x%lx\r\n", *(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_CTRL_OFFSET));
	if (*(volatile UINT32 *)(USB_SULOG_BASE + USB_SULOG_CTRL_OFFSET)& 0x1) {
		uart_printf("DQH:");
		for(i = 0; i < 16; i++) {
			uart_printf(" %x", *(UINT32 *)(((UINT32)ep_queue_head_ptr_g) + 64*(2*0xc+1)+ i*4));
		}
		uart_printf("\r\n end\r\n");
	}
	for(i = 0; i < 16; i++) {
		uart_printf(" %x", *(UINT32 *)(((UINT32)ep_queue_head_ptr_g) + 64*(2*0x4+1)+ i*4));
	}
	return 0;
}
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_send_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Sends data on a specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_send_data
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] buffer to send */
      uint_8_ptr                 buff_ptr,

      /* [IN] length of the transfer */
      uint_32                    size
   )
{ /* Body */
    int 	                        lockKey;
    uint_8                       error;
    XD_STRUCT_PTR                xd_ptr;
    USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;


    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TX, "send_data: ep=%d, pBuf=0x%x, size=%d\n",
                                        ep_num, (unsigned)buff_ptr, (int)size);

    MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_send_count++));

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    USB_dcache_flush((pointer)buff_ptr, size);

    lockKey = USB_lock();

    if (!usb_dev_ptr->XD_ENTRIES)
    {
        ErrorLogPrintf("%s: No free XD entries", __FUNCTION__);
        _usb_dci_vusb20_get_dtd_information();

        USB_unlock(lockKey);

        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_send_data, transfer in progress\n");


        USB2MgrDeviceUnplugPlug();
        return USB_OK;

    } /* Endif */

    /* Get a transfer descriptor */
    USB_XD_QGET(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL, xd_ptr);

    usb_dev_ptr->XD_ENTRIES--;

    /* Initialize the new transfer descriptor */
    xd_ptr->EP_NUM = ep_num;
    xd_ptr->BDIRECTION = MV_USB_SEND;
    xd_ptr->WTOTALLENGTH = size;
    xd_ptr->WSOFAR = 0;
    xd_ptr->WSTARTADDRESS = buff_ptr;

    xd_ptr->BSTATUS = MV_USB_STATUS_TRANSFER_ACCEPTED;

#if 0
    if((ep_num!=0)&&(ep_num!=7))
    {
        uart_printf("_usb_device_send_data ep %d\r\n",ep_num);
    }
#endif

    error = _usb_dci_vusb20_add_dTD(handle, xd_ptr);

    USB_unlock(lockKey);

    if (error)
    {

        MSG_USB_LOG("_usb_device_send_data, transfer failed:0x%0x",error);

        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_send_data, transfer failed\n");
        return USBERR_TX_FAILED;
    } /* Endif */

    _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
    _usbDeviceDatabase.ep0.ep0_IN_cnt++;

    return error;

} /* EndBody */

