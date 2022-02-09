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
***  This file contains the main USB device API functions that will be
***  used by most applications.
***
**************************************************************************
**END*********************************************************/

#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"
#include "mvUsbDebug.h"     // for MV_USB_DEBUG_TRACE macro definition

//#include "mmap.h"           // for NON_CACHED_TO_CACHED, CACHED_TO_NON_CACHED macro definition


#include "usbTrace.h"


USB_DEV_STATE_STRUCT        ci2Device;
USB_DEV_STATE_STRUCT_PTR    global_mv_usb_dev =  &ci2Device;
uint_32                     mv_usb_isr_count = 0;
uint_32                     mv_usb_reset_count = 0;
uint_32                     mv_usb_send_count = 0;
uint_32                     mv_usb_recv_count = 0;
uint_32                     mv_usb_setup_count = 0;
uint_32                     mv_usb_free_XD_count = 0;
uint_32                     mv_usb_free_dTD_count = 0;
uint_32                     mv_usb_complete_ep_count[MV_USB_MAX_ENDPOINTS*2];
uint_32                     mv_usb_cancel_count = 0;
uint_32                     mv_usb_add_count = 0;
uint_32                     mv_usb_add_not_empty_count = 0;
uint_32                     mv_usb_empty_isr_count = 0;
uint_32                     mv_usb_empty_complete_count = 0;
uint_32                     mv_usb_read_setup_count = 0;
uint_32                     mv_usb_complete_isr_count = 0;
uint_32                     mv_usb_complete_count = 0;
uint_32                     mv_usb_complete_max_count = 0;
uint_32                     mv_usb_port_change_count = 0;
uint_32                     mv_usb_suspend_count = 0;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_cleanup
*  Returned Value : void
*  Comments       :
*        Cleanup allocated structures.
*
*END*-----------------------------------------------------------------*/

static void    _usb_device_cleanup(USB_DEV_STATE_STRUCT_PTR usb_dev_ptr)
{
    /* Free all internal transfer descriptors */
    if(usb_dev_ptr->XD_BASE != NULL)
        USB_memfree((pointer)usb_dev_ptr->XD_BASE);

    /* Free all XD scratch memory */
    if(usb_dev_ptr->XD_SCRATCH_STRUCT_BASE != NULL)
        USB_memfree((pointer)usb_dev_ptr->XD_SCRATCH_STRUCT_BASE);

    /* Free the temp ep init XD */
    if(usb_dev_ptr->TEMP_XD_PTR != NULL)
        USB_memfree((pointer)usb_dev_ptr->TEMP_XD_PTR);

    // null out pointers in device handle structure
    //---------------------------------------------
    usb_dev_ptr->XD_BASE = NULL;
    usb_dev_ptr->XD_SCRATCH_STRUCT_BASE = NULL;
    usb_dev_ptr->TEMP_XD_PTR = NULL;

#if 0
    /* Free the USB state structure */
    USB_memfree((pointer)usb_dev_ptr);
#endif
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_free_XD
*  Returned Value : void
*  Comments       :
*        Enqueues a XD onto the free XD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_device_free_XD
(
      /* [IN] the dTD to enqueue */
      pointer  xd_ptr
)
{ /* Body */
	int							lockKey;
   	USB_DEV_STATE_STRUCT_PTR   	usb_dev_ptr;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRACE, "free_XD: xd_ptr=0x%x\n", (unsigned)xd_ptr);

    MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_free_XD_count++));

   	usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)(((XD_STRUCT_PTR)xd_ptr)->SCRATCH_PTR->PRIVATE);

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */

   lockKey = USB_lock();

   /*
   ** Add the XD to the free XD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   USB_XD_QADD(usb_dev_ptr->XD_HEAD, usb_dev_ptr->XD_TAIL,
      (XD_STRUCT_PTR)xd_ptr);
   usb_dev_ptr->XD_ENTRIES++;

   USB_unlock(lockKey);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_handle
*  Returned Value : handle or NULL
*  Comments       :
*        Return handle of already initialized USB device (for DEBUG only)
*
*END*-----------------------------------------------------------------*/
_usb_device_handle  _usb_device_get_handle(uint_8 devnum)
{
    return global_mv_usb_dev;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_max_endpoint
*  Returned Value : handle or NULL
*  Comments       :
*        Return maximum number of endpoints supportedby USB device
*        (for DEBUG only)
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_get_max_endpoint(_usb_device_handle handle)
{
    USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    return usb_dev_ptr->MAX_ENDPOINTS;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device specific data structures and calls
*  the low-level device controller chip initialization routine.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_init
(
      /* [IN] the USB device controller to initialize */
      uint_8                    devnum,

      /* [OUT] the USB_USB_dev_initialize state structure */
      _usb_device_handle*       handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
   XD_STRUCT_PTR                    xd_ptr;
   uint_8                           i, error;
   SCRATCH_STRUCT_PTR               temp_scratch_ptr;

   if (devnum > MAX_USB_DEVICES)
   {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_init, error invalid device number");
        return USBERR_INVALID_DEVICE_NUM;
   } /* Endif */
   global_mv_usb_dev =	&ci2Device;
   usb_dev_ptr = global_mv_usb_dev;
   //uart_printf("global_mv_usb_dev address is 0x%lx\r\n",(UINT32 )&global_mv_usb_dev);
   //uart_printf("global_mv_usb_dev  is 0x%lx\r\n",(UINT32 )global_mv_usb_dev);

#if 0
   /* Allocate memory for the state structure */
   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)USB_memalloc(sizeof(USB_DEV_STATE_STRUCT));
   if (usb_dev_ptr == NULL)
   {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_init, error NULL device handle");
        return USBERR_ALLOC_STATE;
   } /* Endif */
#endif

#if 0
   /* Zero out the internal USB state structure */
   USB_memzero(usb_dev_ptr, sizeof(USB_DEV_STATE_STRUCT));
#endif

   /* Multiple devices will have different base addresses and
   ** interrupt vectors (For future)
   */
   //uart_printf("usb_dev_ptr->USB_STATE is 0x%lx\r\n",&(usb_dev_ptr->USB_STATE));
   usb_dev_ptr->USB_STATE = MV_USB_STATE_UNKNOWN;
   //uart_printf("MV_USB_STATE_UNKNOWN\r\n");

   /* Allocate MAX_XDS_FOR_TR_CALLS */
   xd_ptr = (XD_STRUCT_PTR)USB_memalloc(sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);
   //uart_printf("xd_ptr is 0x%lx\r\n",(UINT32 *)xd_ptr);
   if (xd_ptr == NULL)
   {
        ASSERT(0);
        _usb_device_cleanup(usb_dev_ptr);
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_init, malloc error");
        return USBERR_ALLOC_TR;
   } /* Endif */

   usb_dev_ptr->XD_BASE = xd_ptr;

   _usb_clear_stats();

   USB_memzero(xd_ptr, sizeof(XD_STRUCT) * MAX_XDS_FOR_TR_CALLS);

   /* Allocate memory for internal scratch structure */
   usb_dev_ptr->XD_SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)
                    USB_memalloc(sizeof(SCRATCH_STRUCT)*MAX_XDS_FOR_TR_CALLS);
   //uart_printf("XD_SCRATCH_STRUCT_BASE is 0x%lx\r\n",(UINT32 *) usb_dev_ptr->XD_SCRATCH_STRUCT_BASE);
   if (usb_dev_ptr->XD_SCRATCH_STRUCT_BASE == NULL)
   {
        _usb_device_cleanup(usb_dev_ptr);
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_init, malloc error");
        return USBERR_ALLOC;
   } /* Endif */

   temp_scratch_ptr = usb_dev_ptr->XD_SCRATCH_STRUCT_BASE;
   usb_dev_ptr->XD_HEAD = NULL;
   usb_dev_ptr->XD_TAIL = NULL;
   usb_dev_ptr->XD_ENTRIES = 0;

   /* Enqueue all the XDs */
   for (i=0;i<MAX_XDS_FOR_TR_CALLS;i++)
   {
      xd_ptr->SCRATCH_PTR = temp_scratch_ptr;
      xd_ptr->SCRATCH_PTR->FREE = _usb_device_free_XD;
      xd_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
      _usb_device_free_XD((pointer)xd_ptr);
      xd_ptr++;
      temp_scratch_ptr++;
   } /* Endfor */

   usb_dev_ptr->TEMP_XD_PTR = (XD_STRUCT_PTR)USB_memalloc(sizeof(XD_STRUCT));
   USB_memzero(usb_dev_ptr->TEMP_XD_PTR, sizeof(XD_STRUCT));

   /* Initialize the USB controller chip */
   //uart_printf("before _usb_dci_vusb20_init, devnum is %d, usb_dev_ptr  is 0x%lx\r\n",devnum,(UINT32 )usb_dev_ptr);
   error = _usb_dci_vusb20_init(devnum, usb_dev_ptr);
   if (error)
   {
        _usb_device_cleanup(usb_dev_ptr);
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_init, init failed");
        return USBERR_INIT_FAILED;
   } /* Endif */

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO,"device_init: pDev=0x%x, pXD(%d)=0x%x, pSCRATCH(%d)=0x%x, pTempXD=0x%x\n",
                (unsigned)usb_dev_ptr, MAX_XDS_FOR_TR_CALLS, (unsigned)usb_dev_ptr->XD_BASE,
                MAX_XDS_FOR_TR_CALLS, (unsigned)usb_dev_ptr->XD_SCRATCH_STRUCT_BASE,
                (unsigned)usb_dev_ptr->TEMP_XD_PTR);


   *handle = usb_dev_ptr;
   //uart_printf("_usb_device_init end\r\n");
   return USB_OK;
} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_shutdown
*  Returned Value : USB_OK or error code
*  Comments       :
*        Shutdown an initialized USB device
*
*END*-----------------------------------------------------------------*/
void _usb_device_shutdown(_usb_device_handle handle)
{ /* Body */
    USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;
    SERVICE_STRUCT_PTR           service_ptr, temp_ptr;
    int                          ep;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_CTRL, "shutdown\n");

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

    for(ep=0; ep<(usb_dev_ptr->MAX_ENDPOINTS); ep++)
    {
        /* Cancel all transfers on all endpoints */
        while(_usb_device_get_transfer_status(handle, ep, MV_USB_RECV) !=
                                                    MV_USB_STATUS_IDLE)
        {
            _usb_device_cancel_transfer(handle, ep, MV_USB_RECV);
        }
        while(_usb_device_get_transfer_status(handle, ep, MV_USB_SEND) !=
                                                    MV_USB_STATUS_IDLE)
        {
            _usb_device_cancel_transfer(handle, ep, MV_USB_SEND);
        }
    }
    _usb_dci_vusb20_shutdown(usb_dev_ptr);

    /* Free all the Callback function structure memory */
    //------------------------------------------------
    service_ptr = usb_dev_ptr->SERVICE_HEAD_PTR;
    while( service_ptr != NULL )
    {
        temp_ptr = service_ptr->NEXT;
        USB_memfree(service_ptr);
        service_ptr = temp_ptr;
    }
    usb_dev_ptr->SERVICE_HEAD_PTR = NULL;

    _usb_device_cleanup(usb_dev_ptr);
} /* EndBody */


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_register_service
* Returned Value : USB_OK or error code
* Comments       :
*     Registers a callback routine for a specified event or endpoint.
*
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_register_service
(
      /* [IN] Handle to the USB device */
      _usb_device_handle         handle,

      /* [IN] type of event or endpoint number to service */
      uint_8                     type,

      /* [IN] Pointer to the service's callback function */
      void(_CODE_PTR_ service)(pointer, uint_8, boolean, uint_8, uint_8_ptr, uint_32, uint_8)
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
   SERVICE_STRUCT_PTR         service_ptr;
   SERVICE_STRUCT_PTR   		search_ptr;
   int						  lockKey;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   lockKey = USB_lock();

   /* Search for an existing entry for type */
   //--------------------------------------
   for ( search_ptr = usb_dev_ptr->SERVICE_HEAD_PTR; search_ptr != NULL; search_ptr = search_ptr->NEXT)
   {
      if( search_ptr->TYPE == type)
      {
         /* Found an existing entry */
         USB_unlock(lockKey);
         MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_register_service, service closed\n");
         return USBERR_OPEN_SERVICE;
      } /* Endif */
   } /* Endfor */

   /* No existing entry found - create a new one */
   // 16 bytes per installed service
   //------------------------------------------------
   service_ptr = (SERVICE_STRUCT_PTR)USB_memalloc(sizeof(SERVICE_STRUCT));
   if (!service_ptr)
   {
      USB_unlock(lockKey);
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_register_service, error allocating service\n");
      return USBERR_ALLOC;
   } /* Endif */

   service_ptr->TYPE = type;
   service_ptr->SERVICE = service;
   service_ptr->NEXT = NULL;
   service_ptr->PREV    = NULL;


   //If head of list not started yet
   //--------------------------------------------
   if( usb_dev_ptr->SERVICE_HEAD_PTR == NULL)
   {
     usb_dev_ptr->SERVICE_HEAD_PTR = service_ptr;
   }
   else  // stitch onto end of list
   {
     search_ptr = usb_dev_ptr->SERVICE_HEAD_PTR;

     while( search_ptr->NEXT != NULL )
       search_ptr = search_ptr->NEXT;

     search_ptr->NEXT = service_ptr;
     service_ptr->PREV = search_ptr;
   }

   USB_unlock(lockKey);

   return USB_OK;
} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_unregister_service
* Returned Value : USB_OK or error code
* Comments       :
*     Unregisters a callback routine for a specified event or endpoint.
*
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_unregister_service
(
      /* [IN] Handle to the USB device */
      _usb_device_handle         handle,

      /* [IN] type of event or endpoint number to service */
      uint_8                     type
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR   usb_dev_ptr;
   SERVICE_STRUCT_PTR         search_ptr;
   int						  lockKey;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /* Needs mutual exclusion */
   lockKey = USB_lock();

   /* Search for an existing entry for type */
   for (search_ptr = usb_dev_ptr->SERVICE_HEAD_PTR; search_ptr; search_ptr = search_ptr->NEXT)
   {
      if( search_ptr->TYPE == type)
      {
         /* Found an existing entry - delete it */
         break;
      }
   }

   /* No existing entry found */
   if (!search_ptr)
   {
      USB_unlock(lockKey);
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"_usb_device_unregister_service, no service found\n");
      return USBERR_CLOSED_SERVICE;
   } /* Endif */

   // re-stitch the list before free operation
   //-----------------------------------------
   if( search_ptr->PREV != NULL )
   {
     search_ptr->PREV->NEXT = search_ptr->NEXT;
     if(search_ptr->NEXT)
     	search_ptr->NEXT->PREV=  search_ptr->PREV;
   }
   else  // we're freeing the head of the list
   {
     // sanity check
     if( search_ptr == usb_dev_ptr->SERVICE_HEAD_PTR )
     {
        if( usb_dev_ptr->SERVICE_HEAD_PTR->NEXT != NULL )
        {
          usb_dev_ptr->SERVICE_HEAD_PTR = usb_dev_ptr->SERVICE_HEAD_PTR->NEXT;
          usb_dev_ptr->SERVICE_HEAD_PTR->PREV = NULL;
        }
        else
           usb_dev_ptr->SERVICE_HEAD_PTR = NULL;
     }
   }

   USB_memfree(search_ptr);

   USB_unlock(lockKey);

   return USB_OK;

} /* EndBody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _usb_device_call_service
* Returned Value : USB_OK or error code
* Comments       :
*     Calls the appropriate service for the specified type, if one is
*     registered. Used internally only.
*
*END*--------------------------------------------------------------------*/
uint_8 _usb_device_call_service
(
      /* [IN] Handle to the USB device */
      _usb_device_handle   handle,

      /* [OUT] Type of service or endpoint */
      uint_8               type,

      /* [OUT] Is it a Setup transfer? */
      boolean              setup,

      /* [OUT] Direction of transmission; is it a Transmit? */
      boolean              direction,

      /* [OUT] Pointer to the data */
      uint_8_ptr           buffer_ptr,

      /* [OUT] Number of bytes in transmission */
      uint_32              length,

      /* [OUT] Any errors */
      uint_8               errors
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;
   SERVICE_STRUCT _PTR_         service_ptr;
   int							lockKey;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   /* Needs mutual exclusion */
   lockKey = USB_lock();

   // Search for an existing entry for type
   //------------------------------------------
   service_ptr = usb_dev_ptr->SERVICE_HEAD_PTR;
   while( service_ptr != NULL )
   {
      if (service_ptr->TYPE == type)
      {
         service_ptr->SERVICE(handle, type, setup, direction, buffer_ptr, length, errors);
         USB_unlock(lockKey);

         return USB_OK;
      } /* Endif */

      service_ptr = service_ptr->NEXT;

   } /* Endfor */

   USB_unlock(lockKey);

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_CTRL, "_usb_device_call_service, service %d is closed\n", type);

   return USBERR_CLOSED_SERVICE;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_init_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*     Initializes the endpoint and the data structures associated with the
*  endpoint
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_init_endpoint
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] MAX Packet size for this endpoint */
      uint_16                    max_pkt_size,

      /* [IN] Direction */
      uint_8                     direction,

      /* [IN] Type of Endpoint */
      uint_8                     type,

      /* [IN] After all data is transfered, should we terminate the transfer
      ** with a zero length packet if the last packet size == MAX_PACKET_SIZE?
      */
      uint_8                     flag
)
{ /* Body */

   uint_8         error = 0;
   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /* Initialize the transfer descriptor */
   usb_dev_ptr->TEMP_XD_PTR->EP_NUM = ep_num;
   usb_dev_ptr->TEMP_XD_PTR->BDIRECTION = direction;
   usb_dev_ptr->TEMP_XD_PTR->WMAXPACKETSIZE = max_pkt_size;
   usb_dev_ptr->TEMP_XD_PTR->EP_TYPE = type;
   usb_dev_ptr->TEMP_XD_PTR->DONT_ZERO_TERMINATE = flag;
   usb_dev_ptr->TEMP_XD_PTR->MAX_PKTS_PER_UFRAME =
                    ((flag & MV_USB_MAX_PKTS_PER_UFRAME) >> 1);

   error = _usb_dci_vusb20_init_endpoint(handle, usb_dev_ptr->TEMP_XD_PTR);

   return error;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_deinit_endpoint
*  Returned Value : USB_OK or error code
*  Comments       :
*  Disables the endpoint and the data structures associated with the
*  endpoint
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_deinit_endpoint
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                    ep_num,

      /* [IN] Direction */
      uint_8                    direction
)
{ /* Body */
   int							lockKey;
   uint_8         				error = 0;
//   USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   lockKey = USB_lock();

   error = _usb_dci_vusb20_deinit_endpoint(handle, ep_num, direction);

   USB_unlock(lockKey);

   return error;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_status
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_get_transfer_status
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
)
{ /* Body */
   uint_8   					status;
//   USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;
   int							lockKey;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   lockKey = USB_lock();

   status = _usb_dci_vusb20_get_transfer_status(handle, ep_num, direction);

   USB_unlock(lockKey);

   /* Return the status of the last queued transfer */
   return (status);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_dtd_cnt
*  Returned Value : Address of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_get_dtd_cnt
(
      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction,

      /* [OUT] address */
      uint_32_ptr                StartTick
)
{ /* Body */
   uint_8                       count;
   int							lockKey;

   lockKey = USB_lock();

   count = _usb_dci_vusb20_get_dtd_cnt(ep_num, direction, StartTick);

   USB_unlock(lockKey);

   /* Return the status of the last queued transfer */
   return (count);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_trace_dtd_information
*  Returned Value : Address of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_trace_dtd_information
(
      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
)
{ /* Body */
   int							lockKey;

   lockKey = USB_lock();

   _usb_dci_vusb20_trace_dtd_information(ep_num, direction);

   USB_unlock(lockKey);

   return;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_get_transfer_details
*  Returned Value : Status of the transfer
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_get_transfer_details
   (
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction,

      /* [OUT] transfer detail data structure maintained by driver*/
      uint_32_ptr _PTR_           xd_ptr_ptr
   )
{ /* Body */
//   UINT8   status;
//   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;
   XD_STRUCT_PTR                 temp;

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_transfer_status");
   #endif

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

//   USB_lock();

   temp = _usb_dci_vusb20_get_transfer_details(handle, ep_num, direction);
   (*xd_ptr_ptr) = (uint_32_ptr) temp;

//   USB_unlock();

   #ifdef _DEVICE_DEBUG_
      DEBUG_LOG_TRACE("_usb_device_get_transfer_status, SUCCESSFUL");
   #endif

   return;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_read_setup_data
*  Returned Value : USB_OK or error code
*  Comments       :
*        Reads the setup data from the hardware
*
*END*-----------------------------------------------------------------*/
void _usb_device_read_setup_data
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] buffer for receiving Setup packet */
      uint_8_ptr                  buff_ptr
)
{ /* Body */
//   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   _usb_dci_vusb20_get_setup_data(handle, ep_num, buff_ptr);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_device_cancel_transfer
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
)
{ /* Body */
   uint_8                        error = USB_OK;
   int							 lockKey;
//   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   lockKey = USB_lock();

   /* Cancel transfer on the specified endpoint for the specified
   ** direction
   */
   error = _usb_dci_vusb20_cancel_transfer(handle, ep_num, direction);

   USB_unlock(lockKey);

   return error;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_cancel_transfer
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_free_transfer
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction,

      /* memory free */
      void                      (*free)(void*)
)
{ /* Body */
   int							 lockKey;
//   USB_DEV_STATE_STRUCT_PTR      usb_dev_ptr;

//   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   lockKey = USB_lock();

   /* Cancel transfer on the specified endpoint for the specified
   ** direction
   */
   _usb_dci_vusb20_free_transfer(handle, ep_num, direction, free);

   USB_unlock(lockKey);

   return;
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_device_reset_data_toggle
*  Returned Value : USB_OK or error code
*  Comments       :
*        returns the status of the transaction on the specified endpoint.
*
*END*-----------------------------------------------------------------*/
void _usb_device_reset_data_toggle
(
      /* [IN] the USB_USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
)
{
    int							 lockKey;

    lockKey = USB_lock();

    /* Cancel transfer on the specified endpoint for the specified
    ** direction
    */
    _usb_dci_vusb20_reset_data_toggle(handle, ep_num, direction);

    USB_unlock(lockKey);

    return;
} /* EndBody */
