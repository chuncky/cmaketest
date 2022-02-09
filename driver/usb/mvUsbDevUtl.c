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
***  This file contains USB device API specific utility functions.
***
**************************************************************************
**END*********************************************************/

#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"

#include "usbTrace.h"


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_unstall_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Unstalls the endpoint in specified direction
*
*END*-----------------------------------------------------------------*/
void _usb_device_unstall_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
//   USB_DEV_STATE_STRUCT_PTR    usb_dev_ptr;
   int							lockKey;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
//   uart_printf("ust %d,%p\r\n",ep_num,__return_address());

   lockKey = USB_lock();

   _usb_dci_vusb20_unstall_endpoint(handle, ep_num, direction);

   USB_unlock(lockKey);

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_get_status
* Returned Value : USB_OK or error code
* Comments       :
*     Provides API to access the USB internal state.
*
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_get_status
   (
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,

      /* [IN] What to get the status of */
      uint_8               component,

      /* [OUT] The requested status */
      uint_16_ptr          status
   )
{ /* Body */
   	USB_DEV_STATE_STRUCT_PTR 	usb_dev_ptr;
	int							lockKey;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   lockKey = USB_lock();

   switch (component)
   {
      case MV_USB_STATUS_DEVICE_STATE:
         *status = usb_dev_ptr->USB_STATE;
         break;

      case MV_USB_STATUS_DEVICE:
         *status = usb_dev_ptr->USB_DEVICE_STATE;
         break;

      case MV_USB_STATUS_INTERFACE:
          *status = 0;
         break;

      case MV_USB_STATUS_ADDRESS:
         *status = usb_dev_ptr->DEVICE_ADDRESS;
         break;

      case MV_USB_STATUS_CURRENT_CONFIG:
         *status = usb_dev_ptr->USB_CURR_CONFIG;
         break;

      case MV_USB_STATUS_SOF_COUNT:
         *status = usb_dev_ptr->USB_SOF_COUNT;
         break;

      default:
         if (component & MV_USB_STATUS_ENDPOINT)
         {

            *status = _usb_dci_vusb20_get_endpoint_status(handle,
                            component & MV_USB_STATUS_ENDPOINT_NUMBER_MASK);
         }
         else
         {
            USB_unlock(lockKey);
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_get_status, bad status\n");
            return USBERR_BAD_STATUS;
         } /* Endif */
         break;

   } /* Endswitch */
   USB_unlock(lockKey);

   return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_set_status
* Returned Value : USB_OK or error code
* Comments       :
*     Provides API to set internal state
*
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_set_status
   (
      /* [IN] Handle to the usb device */
      _usb_device_handle   handle,

      /* [IN] What to set the status of */
      uint_8               component,

      /* [IN] What to set the status to */
      uint_16              setting
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR usb_dev_ptr;
   int 					    lockKey;
   uint_8 retVal = USB_OK;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,
            "set_status: component=0x%x, value=0x%x\n", component, setting);

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   lockKey = USB_lock();

   switch (component)
   {
      case MV_USB_STATUS_DEVICE_STATE:
         usb_dev_ptr->USB_STATE = setting;
         break;

      case MV_USB_STATUS_DEVICE:
         usb_dev_ptr->USB_DEVICE_STATE = setting;
         break;

      case MV_USB_STATUS_INTERFACE:
         break;

      case MV_USB_STATUS_CURRENT_CONFIG:
         usb_dev_ptr->USB_CURR_CONFIG = setting;
         break;

      case MV_USB_STATUS_SOF_COUNT:
         usb_dev_ptr->USB_SOF_COUNT = setting;
         break;

      case MV_USB_FORCE_FULL_SPEED:
         _usb_dci_vusb20_set_speed_full((pointer)usb_dev_ptr, setting);
         break;

      case MV_USB_PHY_LOW_POWER_SUSPEND:
         _usb_dci_vusb20_suspend_phy((pointer)usb_dev_ptr, setting);
         break;

      case MV_USB_STATUS_ADDRESS:
         usb_dev_ptr->DEVICE_ADDRESS = setting;

         _usb_dci_vusb20_set_address((pointer)usb_dev_ptr, setting);
         break;

      case MV_USB_STATUS_TEST_MODE:
         _usb_dci_vusb20_set_test_mode(handle, setting);
         break;

      default:
         if (component & MV_USB_STATUS_ENDPOINT)
         {
            _usb_dci_vusb20_set_endpoint_status(handle,
                        component & MV_USB_STATUS_ENDPOINT_NUMBER_MASK, setting);
         }
         else
         {
            USB_unlock(lockKey);
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_set_status, bad status\n");
            retVal =  USBERR_BAD_STATUS;
         } /* Endif */
         break;

   } /* Endswitch */

   USB_unlock(lockKey);

   return retVal;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_stall_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Stalls the endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_stall_endpoint
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
   )
{ /* Body */
//   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
//	uart_printf("st %d,%p\r\n",ep_num,__return_address());
   _usb_dci_vusb20_stall_endpoint(handle, ep_num, direction);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_process_resume
*  Returned Value : USB_OK or error code
*  Comments       :
*        Process Resume event
*
*END*-----------------------------------------------------------------*/
void _usb_device_assert_resume
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle
   )
{ /* Body */
//   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   _usb_dci_vusb20_assert_resume(handle);

} /* EndBody */


/***************************/
/* MV USB Debug functions */
/***************************/

//ICAT EXPORTED FUNCTION - USB,OTG,STATUS
void _usb_status(void)
{
    if(global_mv_usb_dev == NULL)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"USB Device core is not initialized\n");
        return;
    }

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"\n\tUSB Status\n\n");

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"DEV_NUM=%d, DEV_ADDR=%d, CAP_REGS=0x%x, DEV_REGS=0x%x, MAX_EP=%d\n",
                global_mv_usb_dev->DEV_NUM,
                global_mv_usb_dev->DEVICE_ADDRESS,
                (unsigned)global_mv_usb_dev->CAP_REGS_PTR,
                (unsigned)global_mv_usb_dev->DEV_PTR,
                global_mv_usb_dev->MAX_ENDPOINTS);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"BUS_RESET=%s, USB_STATE=0x%02x, USB_DEV_STATE=0x%02x, SPEED=%d, ERRORS=0x%04x\n",
                global_mv_usb_dev->BUS_RESETTING ? "Yes" : "No",
                global_mv_usb_dev->USB_STATE,
                global_mv_usb_dev->USB_DEVICE_STATE,
                global_mv_usb_dev->SPEED,
                global_mv_usb_dev->ERRORS);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"DRIVER_MEMORY_VIRT(PHYS)=0x%08x(0x%08x), EP_QUEUE_HEAD==0x%08x, SERVICE_HEAD=0x%08x\n",
                (unsigned)global_mv_usb_dev->DRIVER_MEMORY_VIRT,
                (unsigned)global_mv_usb_dev->DRIVER_MEMORY_PHYS,
                (unsigned)global_mv_usb_dev->EP_QUEUE_HEAD_PTR,
                (unsigned)global_mv_usb_dev->SERVICE_HEAD_PTR);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"XD_SCRATCH_BASE=0x%08x,  XD_BASE=0x%08x,  XD_HEAD=0x%08x,  XD_TAIL=0x%08x,  XD_ENTRIES=%d\n",
                (unsigned)global_mv_usb_dev->XD_SCRATCH_STRUCT_BASE,
                (unsigned)global_mv_usb_dev->XD_BASE,
                (unsigned)global_mv_usb_dev->XD_HEAD,
                (unsigned)global_mv_usb_dev->XD_TAIL,
                (unsigned)global_mv_usb_dev->XD_ENTRIES);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATUS,"DTD_SCRATCH_BASE=0x%08x, DTD_BASE=0x%08x, DTD_HEAD=0x%08x, DTD_TAIL=0x%08x, DTD_ENTRIES=%d\n",
                (unsigned)global_mv_usb_dev->SCRATCH_STRUCT_BASE,
                (unsigned)global_mv_usb_dev->DTD_ALIGNED_BASE_PTR,
                (unsigned)global_mv_usb_dev->DTD_HEAD,
                (unsigned)global_mv_usb_dev->DTD_TAIL,
                (unsigned)global_mv_usb_dev->DTD_ENTRIES);
}

//ICAT EXPORTED FUNCTION - USB,OTG,STATISTICS
void _usb_stats(void)
{
    int     i;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS,"\n\tUSB Statistics\n\n");

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS,"isr=%u, empty_isr=%u, reset=%u, setup=%u, read_setup=%u\n",
                    mv_usb_isr_count, mv_usb_empty_isr_count, mv_usb_reset_count,
                    mv_usb_setup_count, mv_usb_read_setup_count);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS,"recv=%u, send=%u, add=%u (%u), cancel=%u\n",
                mv_usb_recv_count, mv_usb_send_count, mv_usb_add_count,
                mv_usb_add_not_empty_count, mv_usb_cancel_count);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS,"free_XD=%u, free_dTD=%u\n", mv_usb_free_XD_count, mv_usb_free_dTD_count);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS,"complete_isr=%u, complete=%u, empty_complete=%u, max_complete=%u\n",
                 mv_usb_complete_isr_count, mv_usb_complete_count,
                mv_usb_empty_complete_count, mv_usb_complete_max_count);

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS, "port_change=%u, suspend=%u\n", mv_usb_port_change_count, mv_usb_suspend_count);
    for(i=0; i<(global_mv_usb_dev->MAX_ENDPOINTS); i++)
    {
        if( (mv_usb_complete_ep_count[i*2] == 0) && (mv_usb_complete_ep_count[i*2+1] == 0) )
            continue;

        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS, "EP #%d: RECV (OUT) = %3u, \tSEND (IN) = %u\n", i,
                    mv_usb_complete_ep_count[i*2], mv_usb_complete_ep_count[i*2+1]);
    }
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATISTICS, "\n");
}

void _usb_clear_stats(void)
{
    int i;

    mv_usb_isr_count = mv_usb_reset_count = mv_usb_read_setup_count = 0;
    mv_usb_send_count = mv_usb_recv_count = mv_usb_setup_count = 0;
    mv_usb_free_XD_count = mv_usb_free_dTD_count = 0;
    mv_usb_add_not_empty_count = mv_usb_empty_isr_count = 0;
    mv_usb_complete_isr_count = mv_usb_complete_count = 0;
    mv_usb_empty_complete_count = mv_usb_complete_max_count = 0;
    mv_usb_port_change_count = mv_usb_suspend_count = 0;

    for(i=0; i<MV_USB_MAX_ENDPOINTS*2; i++)
        mv_usb_complete_ep_count[i] = 0;
}

//ICAT EXPORTED FUNCTION - USB,OTG,REGISTERS
void _usb_regs(void)
{
    VUSB20_REG_STRUCT   *cap_regs, *dev_regs;
//    int                 dev_num;

    if(global_mv_usb_dev == NULL)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING, "USB Device core is not initialized\n");
        return;
    }
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "\n\tUSB Capability Registers\n\n");

    cap_regs = global_mv_usb_dev->CAP_REGS_PTR;
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "CAPLENGTH_HCIVER (0x%08x) = 0x%08x\n",
        (unsigned)&cap_regs->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER,
        (unsigned)USB_32BIT_LE(cap_regs->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "DCI_VERSION      (0x%08x) = 0x%08x\n",
        (unsigned)&cap_regs->REGISTERS.CAPABILITY_REGISTERS.DCI_VERSION,
        (unsigned)USB_32BIT_LE(cap_regs->REGISTERS.CAPABILITY_REGISTERS.DCI_VERSION));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "DCC_PARAMS       (0x%08x) = 0x%08x\n",
        (unsigned)&cap_regs->REGISTERS.CAPABILITY_REGISTERS.DCC_PARAMS,
        (unsigned)USB_32BIT_LE(cap_regs->REGISTERS.CAPABILITY_REGISTERS.DCC_PARAMS));

    dev_regs = global_mv_usb_dev->DEV_PTR;
//    dev_num = global_mv_usb_dev->DEV_NUM;
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "\n\tUSB Device Operational Registers\n\n");

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "USB_CMD          (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "USB_STS          (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "USB_INTR         (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "USB_FRINDEX      (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_FRINDEX,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_FRINDEX));

    /* Skip CTRLDSSEGMENT register */
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "DEVICE_ADDR      (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "EP_LIST_ADDR     (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR));

    /* Skip CONFIG_FLAG register */

    /* Skip PORTSCX[0..15] registers*/
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "PORTSCX         (0x%08x) = 0x%08x\n",         (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX));

    /* Skip OTGSC register */

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "USB_MODE         (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "ENDPT_SETUP_STAT (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "ENDPTPRIME       (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "ENDPTFLUSH       (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "ENDPTSTATUS      (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_REGS, "ENDPTCOMPLETE    (0x%08x) = 0x%08x\n",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE));
}


void _usb_dump_regs(void)
{
    VUSB20_REG_STRUCT   *dev_regs;

    if(global_mv_usb_dev == NULL)
    {
        uart_printf("USB is not initialized");
        return;
    }

    dev_regs = global_mv_usb_dev->DEV_PTR;

    if(dev_regs == NULL)
    {
        uart_printf("USB is not initialized");
        return;
    }

    uart_printf("USB_CMD          (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD));
    uart_printf("USB_STS          (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS));
    uart_printf("USB_INTR         (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR));

    /* Skip PORTSCX[0..15] registers*/
    uart_printf("PORTSCX         (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX));

    uart_printf("USB_MODE         (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE));

    uart_printf("ENDPT_SETUP_STAT (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT));

    uart_printf("ENDPTPRIME       (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME));

    uart_printf("ENDPTFLUSH       (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH));

    uart_printf("ENDPTSTATUS      (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS));

    uart_printf("ENDPTCOMPLETE    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE,
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE));

    uart_printf("ENDPTCTRLX[0]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x0],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x0]));

    uart_printf("ENDPTCTRLX[1]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x1],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x1]));

    uart_printf("ENDPTCTRLX[2]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x2],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x2]));

    uart_printf("ENDPTCTRLX[3]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x3],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x3]));

    uart_printf("ENDPTCTRLX[4]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x4],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x4]));

    uart_printf("ENDPTCTRLX[5]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x5],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x5]));

    uart_printf("ENDPTCTRLX[6]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x6],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x6]));

    uart_printf("ENDPTCTRLX[7]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x7],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x7]));

    uart_printf("ENDPTCTRLX[8]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x8],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x8]));

    uart_printf("ENDPTCTRLX[9]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x9],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0x9]));

    uart_printf("ENDPTCTRLX[A]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xA],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xA]));

    uart_printf("ENDPTCTRLX[B]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xB],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xB]));

    uart_printf("ENDPTCTRLX[C]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xC],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xC]));

    uart_printf("ENDPTCTRLX[D]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xD],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xD]));

    uart_printf("ENDPTCTRLX[E]    (0x%08x) = 0x%08x",
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xE],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0xE]));
}


void _usb_ep_status(int ep_num, int direction)
{
    int                                  i, ep_idx;
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR      ep_queue_head_ptr;
    VUSB20_EP_TR_STRUCT_PTR              dTD_ptr, head_dTD_ptr, tail_dTD_ptr, next_dTD_ptr;
    XD_STRUCT_PTR                        xd_ptr, next_xd_ptr;
    VUSB20_REG_STRUCT_PTR                dev_regs;

    if(global_mv_usb_dev == NULL)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING, "USB Device core is not initialized\n");
        return;
    }

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS , "\n\tUSB Endpoint #%d - %s status\n\n", ep_num,
        (direction == MV_USB_SEND) ? "SEND (IN)" : "RECV (OUT)" );

    ep_idx = ep_num*2 + direction;
    dev_regs = global_mv_usb_dev->DEV_PTR;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"ENDPTCTRLX[%d]    (0x%08x) = 0x%08x\n", ep_num,
        (unsigned)&dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num],
        (unsigned)USB_32BIT_LE(dev_regs->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num]));

    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)global_mv_usb_dev->EP_QUEUE_HEAD_PTR + ep_idx;

    head_dTD_ptr = global_mv_usb_dev->EP_DTD_HEADS[ep_idx];
    tail_dTD_ptr = global_mv_usb_dev->EP_DTD_TAILS[ep_idx];

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"EP_QUEUE_HEAD=0x%08x: MAX_PKT=0x%x, SIZE_IOC_INT_STS=0x%x, CURR_DTD=0x%x, NEXT_DTD=0x%x\n",
                (unsigned)ep_queue_head_ptr, (unsigned)USB_32BIT_LE(ep_queue_head_ptr->MAX_PKT_LENGTH),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->SIZE_IOC_INT_STS),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->CURR_DTD_PTR),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->NEXT_DTD_PTR));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"\tBUF_0=0x%08x, BUF_1=0x%08x, BUF_2=0x%08x, BUF_3=0x%08x, BUF_4=0x%08x\n",
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->BUFF_PTR0),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->BUFF_PTR1),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->BUFF_PTR2),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->BUFF_PTR3),
                (unsigned)USB_32BIT_LE(ep_queue_head_ptr->BUFF_PTR4));

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"\tSETUP_BUFFER: ");
    for(i=0; i<sizeof(ep_queue_head_ptr->SETUP_BUFFER); i++)
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"%02x", ep_queue_head_ptr->SETUP_BUFFER[i] & 0xFF);
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"\n");

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"\ndTD_HEAD=0x%08x, dTD_TAIL=0x%08x\n",
                (unsigned)head_dTD_ptr, (unsigned)tail_dTD_ptr);

    dTD_ptr = head_dTD_ptr;
    i = 0;
    while(dTD_ptr != NULL)
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"%d. dTD=0x%08x, SIZE_IOC_STS=0x%08x, BUF_0=0x%08x, NEXT=0x%08x\n",
                    i, (unsigned)dTD_ptr, (unsigned)USB_32BIT_LE(dTD_ptr->SIZE_IOC_STS),
                    (unsigned)USB_32BIT_LE(dTD_ptr->BUFF_PTR0),
                    (unsigned)USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR));

        xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;

        next_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(global_mv_usb_dev,
                           (uint_32)(USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK));
        if(next_dTD_ptr != NULL)
            next_xd_ptr = (XD_STRUCT_PTR)next_dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        else
            next_xd_ptr = NULL;

        if(next_xd_ptr != xd_ptr)
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP_STATUS ,"\tXD=0x%08x, ADDR=0x%08x, SIZE=%u, STATUS=0x%02x\n",
                (unsigned)xd_ptr, (unsigned)xd_ptr->WSTARTADDRESS,
                (unsigned)xd_ptr->WTOTALLENGTH, xd_ptr->BSTATUS);
        }
        i++;
        dTD_ptr = next_dTD_ptr;
    }
}


/* DEBUG */
uint_32 usbDebugFlags = 0;/* 0; */
                        //MV_USB_DEBUG_FLAG_STATUS        // leave this uncommented for debug prints
                        //| MV_USB_DEBUG_FLAG_REGS        // leave this uncommented for debug prints
                        //| MV_USB_DEBUG_FLAG_STATISTICS  // leave this uncommented for debug prints
                     // | MV_USB_DEBUG_FLAG_TEST
                       // | MV_USB_DEBUG_FLAG_INFO
                     // | MV_USB_DEBUG_FLAG_STATS
                     // | MV_USB_DEBUG_FLAG_ISR
                     // | MV_USB_DEBUG_FLAG_INFO
                        // MV_USB_DEBUG_FLAG_ERROR
                       // | MV_USB_DEBUG_FLAG_STALL
                        //MV_USB_DEBUG_FLAG_WARNING;
                      //  | MV_USB_DEBUG_FLAG_ADDR
                      //  | MV_USB_DEBUG_FLAG_EP0
                     // | MV_USB_DEBUG_FLAG_NON_EP0;
                      //| MV_USB_DEBUG_FLAG_RX
                      //| MV_USB_DEBUG_FLAG_TX;  


//ICAT EXPORTED FUNCTION - USB,OTG,DEBUG_FILTER
void    _usb_debug_set_flags(void* pflags)
{
    usbDebugFlags = *(uint_32*)(pflags);
}


uint_32 _usb_debug_get_flags(void)
{
    return usbDebugFlags;
}

#if defined(MV_USB_TRACE_LOG)

uint_16 DEBUG_TRACE_ARRAY_COUNTER = 0;
char    DEBUG_TRACE_ARRAY[TRACE_ARRAY_SIZE][MAX_STRING_SIZE];

void  _usb_debug_init_trace_log(void)
{
    USB_memzero(DEBUG_TRACE_ARRAY, TRACE_ARRAY_SIZE*MAX_STRING_SIZE);
	DEBUG_TRACE_ARRAY_COUNTER =0;
}

//ICAT EXPORTED FUNCTION - USB,OTG,PRINT_LOG
void    _usb_debug_print_trace_log(void)
{
    int     i;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_LOG_PRINTS ,"USB Trace log: start=0x%x, end=0x%x, idx=%d, flags=0x%x\n\n",
               &DEBUG_TRACE_ARRAY[0][0], &DEBUG_TRACE_ARRAY[TRACE_ARRAY_SIZE-1][0],
               DEBUG_TRACE_ARRAY_COUNTER, usbDebugFlags);

    for(i=DEBUG_TRACE_ARRAY_COUNTER; i<TRACE_ARRAY_SIZE; i++)
    {
        if(DEBUG_TRACE_ARRAY[i][0] == '\0')
            continue;

        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_LOG_PRINTS ,"%3d. %s", i, DEBUG_TRACE_ARRAY[i]);
    }
    for(i=0; i<DEBUG_TRACE_ARRAY_COUNTER; i++)
    {
        if(DEBUG_TRACE_ARRAY[i][0] == '\0')
            continue;
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_LOG_PRINTS ,"%3d. %s", i, DEBUG_TRACE_ARRAY[i]);
    }
}
#else
void  _usb_debug_init_trace_log(void)
{
}

void    _usb_debug_print_trace_log(void)
{
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_LOG_PRINTS ,"USB trace log is not supported\n");
}
#endif /* MV_USB_TRACE_LOG */




