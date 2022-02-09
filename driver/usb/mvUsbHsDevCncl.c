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
***  This file contains the VUSB_HS Device Controller interface
***  function to cancel a transfer.
***
**************************************************************************
**END*********************************************************/
#include "usbTrace.h"
#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"

extern BOOL UsbPhyPowerDown;

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        Cancels a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_cancel_transfer
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
   VUSB20_EP_TR_STRUCT_PTR              dTD_ptr, check_dTD_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR      ep_queue_head_ptr;
   XD_STRUCT_PTR                        xd_ptr;
   uint_32                              temp, bit_pos;
   volatile unsigned long               timeout, status_timeout;
   static int index1 = 0;
   static int index2 = 0;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   bit_pos = (1 << (16 * direction + ep_num));
   temp = (2*ep_num + direction);

   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR + temp;

   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

   if (dTD_ptr)
   {
      MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_cancel_count++));

      check_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(usb_dev_ptr,
                            ((uint_32)USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK));

      if ((!UsbPhyPowerDown) && (USB_32BIT_LE(dTD_ptr->SIZE_IOC_STS) & VUSBHS_TD_STATUS_ACTIVE) &&
          (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX & EHCI_PORTSCX_PHY_CLOCK_DISABLE)))
      {
         /* Flushing will halt the pipe */
         /* Write 1 to the Flush register */
         dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = USB_32BIT_LE(bit_pos);

         /* Wait until flushing completed */
         timeout = 0x1000000;
         while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH) & bit_pos)
         {
            /* ENDPTFLUSH bit should be cleared to indicate this operation is complete */
            timeout--;
            if(timeout == 0)
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTFLUSH=0x%x, bit_pos=0x%x \n",
                      (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH),
                      (unsigned)bit_pos);
                break;
            }
         } /* EndWhile */
         status_timeout = 0x1000;
         while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)
         {
            status_timeout--;
            if(status_timeout == 0)
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTSTATUS=0x%x, bit_pos=0x%x\n",
                      (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS),
                      (unsigned)bit_pos);
                break;
            }

            /* Write 1 to the Flush register */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = USB_32BIT_LE(bit_pos);

            /* Wait until flushing completed */
            timeout = 0x10000;
            while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH) & bit_pos)
            {
               /* ENDPTFLUSH bit should be cleared to indicate this operation is complete */
               timeout--;
               if(timeout == 0)
                {
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTFLUSH=0x%x, bit_pos=0x%x\n",
                            (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH),
                            (unsigned)bit_pos);
                    break;
                }
            } /* EndWhile */
         } /* EndWhile */
      } /* Endif */

      /* Retire the current dTD */
      dTD_ptr->SIZE_IOC_STS = 0;
      dTD_ptr->NEXT_TR_ELEM_PTR = USB_32BIT_LE(VUSBHS_TD_NEXT_TERMINATE);

      /* The transfer descriptor for this dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;

      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRANSFER, "cancel_transfer_%d: ep=%d %s, xd_ptr=0x%x, dTD_ptr=0x%x\n",
                       mv_usb_cancel_count, ep_num, direction ? "SEND" : "RECV",
                       (unsigned)xd_ptr, (unsigned)dTD_ptr);

      /* Free the dTD */
      _usb_dci_vusb20_free_dTD((pointer)dTD_ptr);

      /* Update the dTD head and tail for specific endpoint/direction */
      if (!check_dTD_ptr)
      {
         usb_dev_ptr->EP_DTD_HEADS[temp] = NULL;
         usb_dev_ptr->EP_DTD_TAILS[temp] = NULL;
         if (xd_ptr)
         {
            xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
            /* Free the transfer descriptor */
            _usb_device_free_XD((pointer)xd_ptr);
         } /* Endif */
         /* No other transfers on the queue */
         ep_queue_head_ptr->NEXT_DTD_PTR = USB_32BIT_LE(VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE);
         ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
      }
      else
      {
         usb_dev_ptr->EP_DTD_HEADS[temp] = check_dTD_ptr;

         if (xd_ptr)
         {
            if ((uint_32)check_dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD != (uint_32)xd_ptr)
            {
               xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
               /* Free the transfer descriptor */
               _usb_device_free_XD((pointer)xd_ptr);
            } /* Endif */
         } /* Endif */

#if 1
         if (USB_32BIT_LE(check_dTD_ptr->SIZE_IOC_STS) & VUSBHS_TD_STATUS_ACTIVE)
#else
         if (!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos))
#endif
         {
            /* Read DQH to wait for DDR ready before prime endpoint. */
            for(index1 = 0; index1 < 1; index1++)
            {
                index2++;
            }

            index2 = ep_queue_head_ptr->MAX_PKT_LENGTH;

            /* Start CR 1015 */
            /* Prime the Endpoint */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = USB_32BIT_LE(bit_pos);

            if (!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos))
            {
               timeout = 0x100000;
               while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos)
               {
                  /* Wait for the ENDPTPRIME to go to zero */
                  timeout--;
                  if(timeout == 0)
                  {
                      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTPRIME=0x%x, bit_pos=0x%x\n",
                                (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME),
                                (unsigned)bit_pos);
                      break;
                  }
               } /* EndWhile */

               if (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)
               {
                  /* The endpoint was not not primed so no other transfers on
                  ** the queue
                  */
                  goto done;
               } /* Endif */
            }
            else
            {
               goto done;
            } /* Endif */

            /* No other transfers on the queue */
            ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)USB_32BIT_LE((uint_32)check_dTD_ptr);
            ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

            /* Read DQH to wait for DDR ready before prime endpoint. */
            for(index1 = 0; index1 < 1; index1++)
            {
                index2++;
            }

            index2 = ep_queue_head_ptr->MAX_PKT_LENGTH;

            /* Prime the Endpoint */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = USB_32BIT_LE(bit_pos);

            timeout = 1000;
            while (!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos))
            {
              /* Wait until ENDPT_SETUP_STAT bits is ready */
               timeout--;
               if(timeout <= 0)
               {
                   if(!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME & bit_pos))
                   {
                        /* Read DQH to wait for DDR ready before prime endpoint. */
                        for(index1 = 0; index1 < 1; index1++)
                        {
                            index2++;
                        }

                        index2 = ep_queue_head_ptr->MAX_PKT_LENGTH;

                        /* Prime the Endpoint again */
                        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = USB_32BIT_LE(bit_pos);
                   }
                   break;
               }
            } /* Endif */
         } /* Endif */
      } /* Endif */
   } /* Endif */

done:

   /* End CR 1015 */
   return USB_OK;
} /* EndBody */

/* EOF */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_cancel_free_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        Cancels a transfer
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_free_transfer
   (
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction,

      /* memory free */
      void                      (*free)(void*)
   )
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR             usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR              dTD_ptr = NULL;
   VUSB20_EP_TR_STRUCT_PTR              check_dTD_ptr = NULL;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR      ep_queue_head_ptr = NULL;
   XD_STRUCT_PTR                        xd_ptr;
   uint_32                              temp, bit_pos;
   volatile unsigned long               timeout, status_timeout;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   bit_pos = (1 << (16 * direction + ep_num));
   temp = (2*ep_num + direction);

   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR + temp;

   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

   while (dTD_ptr != NULL)
   {
      MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_cancel_count++));

      check_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(usb_dev_ptr,
                            ((uint_32)USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK));

      if((!UsbPhyPowerDown) && (!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX & EHCI_PORTSCX_PHY_CLOCK_DISABLE)))
      {
          if (USB_32BIT_LE(dTD_ptr->SIZE_IOC_STS) & VUSBHS_TD_STATUS_ACTIVE)
          {
             /* Flushing will halt the pipe */
             /* Write 1 to the Flush register */
             dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = USB_32BIT_LE(bit_pos);

             /* Wait until flushing completed */
             timeout = 0x1000000;
             while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH) & bit_pos)
             {
                /* ENDPTFLUSH bit should be cleared to indicate this operation is complete */
                timeout--;
                if(timeout == 0)
                {
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTFLUSH=0x%x, bit_pos=0x%x \n",
                          (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH),
                          (unsigned)bit_pos);
                    break;
                }
             } /* EndWhile */
             status_timeout = 0x1000;
             while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)
             {
                status_timeout--;
                if(status_timeout == 0)
                {
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTSTATUS=0x%x, bit_pos=0x%x\n",
                          (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS),
                          (unsigned)bit_pos);
                    break;
                }

                /* Write 1 to the Flush register */
                dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = USB_32BIT_LE(bit_pos);

                /* Wait until flushing completed */
                timeout = 0x10000;
                while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH) & bit_pos)
                {
                   /* ENDPTFLUSH bit should be cleared to indicate this operation is complete */
                   timeout--;
                   if(timeout == 0)
                    {
                        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Cancel: - TIMEOUT for ENDPTFLUSH=0x%x, bit_pos=0x%x\n",
                                (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH),
                                (unsigned)bit_pos);
                        break;
                    }
                } /* EndWhile */
             } /* EndWhile */
          } /* Endif */
      }

      /* Retire the current dTD */
      dTD_ptr->SIZE_IOC_STS = 0;
      dTD_ptr->NEXT_TR_ELEM_PTR = USB_32BIT_LE(VUSBHS_TD_NEXT_TERMINATE);

      /* The transfer descriptor for this dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;

      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRANSFER, "cancel_transfer_%d: ep=%d %s, xd_ptr=0x%x, dTD_ptr=0x%x\n",
                       mv_usb_cancel_count, ep_num, direction ? "SEND" : "RECV",
                       (unsigned)xd_ptr, (unsigned)dTD_ptr);

      if((xd_ptr != NULL)&&(xd_ptr->WSTARTADDRESS != NULL))
      {
        if(free != NULL)
        {
            free((void *)xd_ptr->WSTARTADDRESS);
            //xd_ptr->WSTARTADDRESS = 0;
        }
      }

      /* Free the dTD */
      _usb_dci_vusb20_free_dTD((pointer)dTD_ptr);

      /* Update the dTD head and tail for specific endpoint/direction */
      if (!check_dTD_ptr)
      {
         usb_dev_ptr->EP_DTD_HEADS[temp] = NULL;
         usb_dev_ptr->EP_DTD_TAILS[temp] = NULL;
         if (xd_ptr)
         {
            xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
            /* Free the transfer descriptor */
            _usb_device_free_XD((pointer)xd_ptr);
         } /* Endif */
         /* No other transfers on the queue */
         ep_queue_head_ptr->NEXT_DTD_PTR = USB_32BIT_LE(VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE);
         ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
      }
      else
      {
         usb_dev_ptr->EP_DTD_HEADS[temp] = check_dTD_ptr;

         if (xd_ptr)
         {
            if ((uint_32)check_dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD != (uint_32)xd_ptr)
            {
               xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
               /* Free the transfer descriptor */
               _usb_device_free_XD((pointer)xd_ptr);
            } /* Endif */
         } /* Endif */

        /* No other transfers on the queue */
        ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)USB_32BIT_LE((uint_32)check_dTD_ptr);
        ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;
      } /* Endif */

      dTD_ptr = check_dTD_ptr;
   } /* while */

} /* EndBody */

