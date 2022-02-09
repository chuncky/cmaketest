/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

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
***  This file contains the main VUSB_HS Device Controller interface
***  functions.
***
**************************************************************************
**END*********************************************************/

#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"
//#include "diag.h"
#include "usbTrace.h"
#include "usb_macro.h"
#include "mvUsbLog.h"
//#include "diag_macro.h"
#include "mvUsbModem.h"
#include "rndis.h"
#include "utilities.h"
#include "usb2_device.h"

#if defined(MV_USB2_MASS_STORAGE)
#include "mvUsbStorage.h"
#endif

uint_8_ptr   usb_gadget_virt_base = 0;
uint_8_ptr   usb_gadget_phys_base = 0;

volatile uint_32  _usbDeviceDebug_int_reset = 0;
volatile uint_32  _usbDeviceDebug_int_port_change = 0;
volatile uint_32  _usbDeviceDebug_int_err = 0;
volatile uint_32  _usbDeviceDebug_int_int = 0;
volatile uint_32  _usbDeviceDebug_int_suspend = 0;


extern uint_32 pm_usb_busy;


UINT32 assertFlag=0;
extern BOOL diagUsbBusy;
#ifdef USB_REMOTEWAKEUP
extern BOOL EnableRemoteWakeup;
extern BOOL SetRemoteWakeupFeature;
#endif
#ifdef MV_USB2_MASS_STORAGE
extern UINT8 USBGetMaxLun;
#endif

BOOL USBConnectStatus;
extern UINT32 Image$$DDR_NONCACHE_USB_MEMORY_REGION_ENDMARKER$$Base;
extern UINT32 Image$$DDR_NONCACHE_USB_MEMORY_REGION$$Base;
extern USB_DEV_STATE_STRUCT_PTR _usbDeviceHandle;
extern void mvUsbStorage_test_unit_ready_response(void);


#pragma arm section zidata="USB_POOL_DQH"
__attribute__((aligned(2048)))uint_8 ciQueueHeadList[2 * 16 * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)];
#pragma arm section zidata


//#define QU_HEAD_INTERNAL_SRAM_SIZE   ((((/*MAX_ENDPOINTS=*/16 * 2) * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048) + ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 32) + (sizeof(SCRATCH_STRUCT)*MAX_EP_TR_DESCRS))
//uint_8   usb_gadget_virt_base[QU_HEAD_INTERNAL_SRAM_SIZE];


#ifdef MV_USB2_MASS_STORAGE
extern void USB2ConfigureCdromDescriptor(void);
#endif

/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_enable_endpoint
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/

uint_8 _usb_dci_vusb20_enable_endpoint
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] Direction */
      uint_8                     direction
)
{ /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    /* Enable the endpoint for Rx and Tx and set the endpoint type */
    if(direction  == MV_USB_SEND)
    {
        if(!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] & EHCI_EPCTRL_TX_ENABLE))
        {
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |= (EHCI_EPCTRL_TX_ENABLE);
        }
    }
    else
    {
        if(!(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] & EHCI_EPCTRL_RX_ENABLE))
        {
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] |= (EHCI_EPCTRL_RX_ENABLE);
        }
    }

    return USB_OK;

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_stop_transter
*  Returned Value : None
*  Comments       :
*        Stop all usb transfer.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_stop_transter(void)
{
	//if(pm_usb_busy)
	{
	    ErrorLogPrintf("USB stop transter");

        pc_netif_status(FALSE);

        /* Free Endpoint transfer of EP0. */
        _usb_dci_vusb20_free_ep_transfer( 0, 0, mvUsbEP0MemFree );
        _usb_dci_vusb20_free_ep_transfer( 0, 1, mvUsbEP0MemFree );

        /* Free Endpoint transfer of RNDIS. */
        _usb_dci_vusb20_free_ep_transfer( USB_NET_TX_ENDPOINT, 1, mvUsbNetTxMemFree );
        _usb_dci_vusb20_free_ep_transfer( USB_NET_CRTL_ENDPOINT, 1, NULL );
        _usb_dci_vusb20_free_ep_transfer( USB_NET_RX_ENDPOINT, 0, mvUsbNetRxMemFree );

        /* Free Endpoint transfer of modem. */
        _usb_dci_vusb20_free_ep_transfer(MODEM_TX_ENDPOINT, 1, free);
        _usb_dci_vusb20_free_ep_transfer(MODEM_RX_ENDPOINT, 0, NULL);
        _usb_dci_vusb20_free_ep_transfer(MODEM_CTRL_ENDPOINT, 1, free);

        /* Free Endpoint transfer of Diag. */
        _usb_dci_vusb20_free_ep_transfer(DIAG_TX_ENDPOINT, 1, NULL);
        _usb_dci_vusb20_free_ep_transfer(DIAG_RX_ENDPOINT, 0, NULL);
        _usb_dci_vusb20_free_ep_transfer(DIAG_CTRL_ENDPOINT, 1, free);

#if defined(MV_USB2_MASS_STORAGE)
        /* Reset USBGetMaxLun. */
        USBGetMaxLun = 0;

        /* Free Endpoint transfer of Mass storage. */
        _usb_dci_vusb20_free_ep_transfer(USB_MASS_STORAGE_TX_ENDPOINT, 1, NULL);
        _usb_dci_vusb20_free_ep_transfer(USB_MASS_STORAGE_RX_ENDPOINT, 0, NULL);
#endif

        mvUsbNetResetParameters();

        mvUsbModemClearDummyFlag();
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_suspend_tx_transter
*  Returned Value : None
*  Comments       :
*        Stop all usb tx transfer.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_suspend_tx_transter(void)
{
    ErrorLogPrintf("Suspend USB TX");

#ifdef USB_REMOTEWAKEUP
    EnableRemoteWakeup = TRUE;
#endif

    pc_netif_status(FALSE);

    /* Free Endpoint transfer of EP0. */
    _usb_dci_vusb20_free_ep_transfer( 0, 1, mvUsbEP0MemFree );

    /* Free Endpoint transfer of RNDIS. */
    _usb_dci_vusb20_free_ep_transfer( USB_NET_TX_ENDPOINT, 1, mvUsbNetTxMemFree );
    _usb_dci_vusb20_free_ep_transfer( USB_NET_CRTL_ENDPOINT, 1, NULL );

    /* Free Endpoint transfer of modem. */
    _usb_dci_vusb20_free_ep_transfer(MODEM_TX_ENDPOINT, 1, free);
    _usb_dci_vusb20_free_ep_transfer(MODEM_CTRL_ENDPOINT, 1, free);

    /* Free Endpoint transfer of Diag. */
    _usb_dci_vusb20_free_ep_transfer(DIAG_TX_ENDPOINT, 1, NULL);
    _usb_dci_vusb20_free_ep_transfer(DIAG_CTRL_ENDPOINT, 1, free);

#if defined(MV_USB2_MASS_STORAGE)
   /* Free Endpoint transfer of Mass storage. */
    _usb_dci_vusb20_free_ep_transfer(USB_MASS_STORAGE_TX_ENDPOINT, 1, NULL);
#endif

    mvUsbNetParasInit();

    mvUsbModemClearDummyFlag();

}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_remote_wakeup
*  Returned Value : None
*  Comments       : usb remote wakeup.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_remote_wakeup(void)
{
    if(_usbDeviceHandle != NULL)
    {
    	USB_TRACE("_usb_dci_vusb20_remote_wakeup\r\n");
		uart_printf("remote_wakeup\n");
        _usb_device_assert_resume(_usbDeviceHandle);
    }
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_init
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_init
(
      /* [IN] the USB device controller to initialize */
      uint_8                     devnum,

      /* [OUT] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
//   uint_32                                      temp;
   uint_32                                      phyAddr, total_memory = 0;
   uint_8_ptr                                    driver_memory;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   usb_dev_ptr->CAP_REGS_PTR =
       (VUSB20_REG_STRUCT_PTR)USB_get_cap_reg_addr(devnum);

   /* Get the base address of the VUSB_HS registers */
   usb_dev_ptr->DEV_PTR =
      (VUSB20_REG_STRUCT_PTR)(((uint_32)usb_dev_ptr->CAP_REGS_PTR) +
       (USB_32BIT_LE(usb_dev_ptr->CAP_REGS_PTR->REGISTERS.CAPABILITY_REGISTERS.CAPLENGTH_HCIVER) &
                                                                EHCI_CAP_LEN_MASK));

   /* Get the maximum number of endpoints supported by this USB controller */
   usb_dev_ptr->MAX_ENDPOINTS =
      (USB_32BIT_LE(usb_dev_ptr->CAP_REGS_PTR->REGISTERS.CAPABILITY_REGISTERS.DCC_PARAMS) &
                                                VUSB20_DCC_MAX_ENDPTS_SUPPORTED);

   // temp = (usb_dev_ptr->MAX_ENDPOINTS * 2);

    /****************************************************************
      Consolidated memory allocation
      VUSB20_EP_QUEUE_HEAD_STRUCT = 64 bytes (64*32Ep's = 2048)
      VUSB20_EP_TR_STRUCT         = 32 bytes (32*8 = 256)
      SCRATCH_STRUCT              = 12 bytes (12*8 = 96)
                                             (2048)
                                             (32)
      total should = 2048 + 256 + 96 + 2048 + 32 = 4480
    ****************************************************************/
    total_memory = /* DCB this portion statically declared & aligned ((temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048) + */
                  ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 64) +
                  (sizeof(SCRATCH_STRUCT)*MAX_EP_TR_DESCRS);

    /* allocate new space from SRAM only if not allocated in previous "cycle" - for OTG mode */
    if(usb_gadget_virt_base == NULL)
    {
      usb_gadget_virt_base = (uint_8_ptr) USB_uncached_memalloc( total_memory, phyAddr);
      if (usb_gadget_virt_base == NULL)
      {
        ASSERT(0);
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING, "_usb_dci_vusb20_init, malloc failed\n");
        return USBERR_ALLOC;
      }
	  usb_gadget_phys_base = usb_gadget_virt_base; //phyAddr;
	}

    driver_memory = usb_gadget_virt_base;
    phyAddr = (uint_32)usb_gadget_phys_base;

 /*   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB init: CAP_REGS=0x%x, DEV_REGS=0x%x, MAX_EP=%d, virtAddr=%p, phyAddr=0x%x\n",
                (unsigned)usb_dev_ptr->CAP_REGS_PTR, (unsigned)usb_dev_ptr->DEV_PTR,
                usb_dev_ptr->MAX_ENDPOINTS, driver_memory, phyAddr);              */

    /****************************************************************
      Zero out the memory allocated
    ****************************************************************/
    USB_memzero( (void*)driver_memory, total_memory);
    USB_memzero(ciQueueHeadList, sizeof(ciQueueHeadList));

    /****************************************************************
      Flush the zeroed memory if cache is enabled
    ****************************************************************/
    USB_dcache_flush((pointer)driver_memory, total_memory);
    USB_dcache_flush(ciQueueHeadList, sizeof(ciQueueHeadList));

    /****************************************************************
     Keep a pointer to driver memory alloctaion
    ****************************************************************/
    usb_dev_ptr->DRIVER_MEMORY_SIZE = total_memory;
    usb_dev_ptr->DRIVER_MEMORY_VIRT = driver_memory;
    usb_dev_ptr->DRIVER_MEMORY_PHYS = phyAddr;

    /****************************************************************
      Assign QH base
    ****************************************************************/
	USB_dcache_inv(ciQueueHeadList, sizeof(ciQueueHeadList));
    usb_dev_ptr->EP_QUEUE_HEAD_BASE = CACHED_TO_NON_CACHED((VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)ciQueueHeadList);
    //driver_memory += ((temp * sizeof(VUSB20_EP_QUEUE_HEAD_STRUCT)) + 2048);

    // DCB RETURN HERE: 2048 ALIGN is problem ...
    //--------------------------------------------
    /* Align the endpoint queue head to 2K boundary */
    // This should pass the pointer unchanged since it's already aligned
    // statically
#if 1
    usb_dev_ptr->EP_QUEUE_HEAD_PTR = CACHED_TO_NON_CACHED(
            (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_BASE);
#else
    //usb_dev_ptr->EP_QUEUE_HEAD_PTR = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)
    //            USB_MEM2048_ALIGN((uint_32)usb_dev_ptr->EP_QUEUE_HEAD_BASE);
#endif
    /****************************************************************
      Assign DTD base
    ****************************************************************/
    usb_dev_ptr->DTD_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR)driver_memory;
    driver_memory += ((MAX_EP_TR_DESCRS * sizeof(VUSB20_EP_TR_STRUCT)) + 64);

    /* Align the dTD base to 32 byte boundary */
    usb_dev_ptr->DTD_ALIGNED_BASE_PTR = (VUSB20_EP_TR_STRUCT_PTR)
                        USB_MEM32_ALIGN((uint_32)usb_dev_ptr->DTD_BASE_PTR);

    /****************************************************************
      Assign SCRATCH Structure base
    ****************************************************************/
    /* Allocate memory for internal scratch structure */
    usb_dev_ptr->SCRATCH_STRUCT_BASE = (SCRATCH_STRUCT_PTR)driver_memory;

    usb_dev_ptr->USB_STATE = MV_USB_STATE_UNKNOWN;
    /* Initialize the VUSB_HS controller */
    _usb_dci_vusb20_chip_initialize((pointer)usb_dev_ptr);

    return USB_OK;
} /* EndBody */

extern void USB2DeviceEnableUsbInterrupt(void);

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_chip_initialize
*  Returned Value : USB_OK or error code
*  Comments       :
*        Initializes the USB device controller.
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_chip_initialize
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR            dev_ptr;
    VUSB20_EP_QUEUE_HEAD_STRUCT_PTR  ep_queue_head_ptr;
    VUSB20_EP_TR_STRUCT_PTR          dTD_ptr;
    uint_32                          i, port_control;
    SCRATCH_STRUCT_PTR               temp_scratch_ptr;
    volatile unsigned long           delay;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INIT, "chip_initialize\n");

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    /* Stop the controller */
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &= ~(USB_32BIT_LE(EHCI_CMD_RUN_STOP));

    /* Reset the controller to get default values */
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD = USB_32BIT_LE(EHCI_CMD_CTRL_RESET);

  /*  MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Init: Wait for RESET completed\n"); */

    delay = 0x100000;
    while (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &
                                    (USB_32BIT_LE(EHCI_CMD_CTRL_RESET)))
    {
        /* Wait for the controller reset to complete */
        delay--;
        if(delay == 0)
            break;
    } /* EndWhile */

    if(delay == 0)
    {	// sanity check to trap RESET bit not ready
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"USB Init: Wait for RESET completed TIMEOUT\n");
    }
    else
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_RESET,"USB Init: RESET completed\n");
    }

#ifdef NEZHA3_1826
    {   //configure TX FIFO SRAM size for Nezha3 USB2 OTG controller, 256DWs for EP1~10, 64DWs for other 6 EPs.
        *(unsigned long*)(USB_REGISTERS_BASE+0x018) = 0x77777775; //0x7 for 256DWs, 0x5 for 64DWs
        *(unsigned long*)(USB_REGISTERS_BASE+0x01C)   = 0x55575577; //0x7 for 256DWs, 0x5 for 64DWs
    }
#endif
   /* Program the controller to be the USB device controller */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE =
      (USB_32BIT_LE(VUSBHS_MODE_CTRL_MODE_DEV | VUSBHS_MODE_SETUP_LOCK_DISABLE));

   /* Initialize the internal dTD head and tail to NULL */
   usb_dev_ptr->DTD_HEAD = NULL;
   usb_dev_ptr->DTD_TAIL = NULL;
    usb_dev_ptr->DTD_ENTRIES = 0;

   /* Make sure the 16 MSBs of this register are 0s */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = USB_32BIT_LE(0);

   ep_queue_head_ptr = usb_dev_ptr->EP_QUEUE_HEAD_PTR;

   /* Initialize all device queue heads */
   for (i=0; i<(usb_dev_ptr->MAX_ENDPOINTS*2); i++)
   {
      /* Interrupt on Setup packet */
      (ep_queue_head_ptr + i)->MAX_PKT_LENGTH = (USB_32BIT_LE(
          ((uint_32)USB_MAX_CTRL_PAYLOAD << VUSB_EP_QUEUE_HEAD_MAX_PKT_LEN_POS) |
            VUSB_EP_QUEUE_HEAD_IOS));

      (ep_queue_head_ptr + i)->NEXT_DTD_PTR = (USB_32BIT_LE(VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE));
   } /* Endfor */

   /* Configure the Endpoint List Address */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.EP_LIST_ADDR =
                            (uint_32)USB_32BIT_LE(USB_VIRT_TO_PHYS(usb_dev_ptr, ep_queue_head_ptr));

   if (usb_dev_ptr->CAP_REGS_PTR->REGISTERS.CAPABILITY_REGISTERS.HCS_PARAMS &
                                        USB_32BIT_LE(VUSB20_HCS_PARAMS_PORT_POWER_CONTROL_FLAG))
   {
      port_control = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX);
      port_control &= (~EHCI_PORTSCX_W1C_BITS | ~EHCI_PORTSCX_PORT_POWER);
      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/ = USB_32BIT_LE(port_control);
   } /* Endif */

   dTD_ptr = usb_dev_ptr->DTD_ALIGNED_BASE_PTR;

   temp_scratch_ptr = usb_dev_ptr->SCRATCH_STRUCT_BASE;

   /* Enqueue all the dTDs */
   for (i=0; i<MAX_EP_TR_DESCRS; i++)
   {
      dTD_ptr->SCRATCH_PTR = temp_scratch_ptr;
      dTD_ptr->SCRATCH_PTR->FREE = _usb_dci_vusb20_free_dTD;
      /* Set the dTD to be invalid */
      dTD_ptr->NEXT_TR_ELEM_PTR = USB_32BIT_LE(VUSBHS_TD_NEXT_TERMINATE);
      /* Set the Reserved fields to 0 */
      dTD_ptr->SIZE_IOC_STS &= ~(USB_32BIT_LE(VUSBHS_TD_RESERVED_FIELDS));
      dTD_ptr->SCRATCH_PTR->PRIVATE = (pointer)usb_dev_ptr;
      _usb_dci_vusb20_free_dTD((pointer)dTD_ptr);
      dTD_ptr++;
      temp_scratch_ptr++;
   } /* Endfor */

   /* Initialize the endpoint 0 properties */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0] = USB_32BIT_LE(
      (EHCI_EPCTRL_TX_DATA_TOGGLE_RST | EHCI_EPCTRL_RX_DATA_TOGGLE_RST));
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[0] &= ~(USB_32BIT_LE
      (EHCI_EPCTRL_TX_EP_STALL | EHCI_EPCTRL_RX_EP_STALL));

   /* Enable interrupts */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR = USB_32BIT_LE(
                           EHCI_INTR_INT_EN
                         | EHCI_INTR_ERR_INT_EN
                         | EHCI_INTR_PORT_CHANGE_DETECT_EN
                         | EHCI_INTR_RESET_EN
                         | EHCI_INTR_DEVICE_SUSPEND
                       /*
                         | EHCI_INTR_SOF_UFRAME_EN
                        */
                         );

   usb_dev_ptr->USB_STATE = MV_USB_STATE_UNKNOWN;
   // DCB RETURN: TPV A0 app specific, set UTMI+
   // interface and 8 bit width
   //-------------------------------------------
   _usb_dci_vusb20_set_xcvr_interface( handle, EHCI_PORTSCX_UTMI_XCVR_SELECT, 0 );
   _usb_dci_vusb20_set_xcvr_width( handle, EHCI_PORTSCX_PTW_8BIT_SELECT, 0 );


   // DCB RETURN: TPV A0 Application specific
   // Force full speed connect/handshake.
   // According to what I interpret, for a
   // "USB DEVICE" we only configure port 0
   //----------------------------------------

#if defined MV_USB2_FULL_SPEED_MODE
   _usb_dci_vusb20_set_speed_full(handle, 0); // DCB RE-ENABLE FS FORCE BEFORE TAPE-OUT, FOR SLE ONLY
#endif

   // this is where interrupts to CPU should be enabled
   // just before the PHY connects
   //----------------------------------------------------

    USB2DeviceEnableUsbInterrupt();

   // Set the Run bit in the command register
   // This will cause the phy to connect to the USB bus
   //--------------------------------------------------

   /* Set the Run bit in the command register */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD = USB_32BIT_LE(EHCI_CMD_RUN_STOP);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_free_dTD
*  Returned Value : void
*  Comments       :
*        Enqueues a dTD onto the free DTD ring.
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_free_dTD
(
      /* [IN] the dTD to enqueue */
      pointer  dTD_ptr
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;
   int                          lockKey;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRACE, "free_dTD: dTD_ptr=0x%x\n", (unsigned)dTD_ptr);

   MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_free_dTD_count++));

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)(((VUSB20_EP_TR_STRUCT_PTR)dTD_ptr)->SCRATCH_PTR->PRIVATE);

   /*
   ** This function can be called from any context, and it needs mutual
   ** exclusion with itself.
   */
   lockKey = USB_lock();

   /*
   ** Add the dTD to the free dTD queue (linked via PRIVATE) and
   ** increment the tail to the next descriptor
   */
   EHCI_DTD_QADD(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, (VUSB20_EP_TR_STRUCT_PTR)dTD_ptr);
   usb_dev_ptr->DTD_ENTRIES++;

   USB_unlock(lockKey);

} /* Endbody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_add_dTD
*  Returned Value : USB_OK or error code
*  Comments       :
*        Adds a device transfer desriptor(s) to the queue.
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_add_dTD
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] The transfer descriptor address */
      XD_STRUCT_PTR              xd_ptr
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR            dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR          dTD_ptr, temp_dTD_ptr, first_dTD_ptr = NULL;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR  ep_queue_head_ptr;
   uint_32                          curr_pkt_len, remaining_len;
   uint_32                          curr_offset, temp, bit_pos, temp_ep_stat = 0;
   volatile unsigned long           timeout;
   static   int                     index1 = 0;
   static   int                     index2 = 0;

   /*********************************************************************
   For a optimal implementation, we need to detect the fact that
   we are adding DTD to an empty list. If list is empty, we can
   actually skip several programming steps esp. those for ensuring
   that there is no race condition.The following boolean will be useful
   in skipping some code here.
   *********************************************************************/
   boolean           list_empty = FALSE;
   boolean           read_safe = FALSE;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   remaining_len = xd_ptr->WTOTALLENGTH;

   curr_offset = 0;
   temp = (2*xd_ptr->EP_NUM + xd_ptr->BDIRECTION);
   bit_pos = (1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));

   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR + temp;



   /*********************************************************************
   This loops iterates through the length of the transfer and divides
   the data in to DTDs each handling the a max of 0x4000 bytes of data.
   The first DTD in the list is stored in a pointer called first_dTD_ptr.
   This pointer is later linked in to QH for processing by the hardware.
   *********************************************************************/

    do
    {
        /* Check if we need to split the transfer into multiple dTDs */
        if (remaining_len > VUSB_EP_MAX_LENGTH_TRANSFER)
        {
            curr_pkt_len = VUSB_EP_MAX_LENGTH_TRANSFER;
        }
        else
        {
            curr_pkt_len = remaining_len;
        } /* Endif */

        /* Get a dTD from the queue */
        EHCI_DTD_QGET(usb_dev_ptr->DTD_HEAD, usb_dev_ptr->DTD_TAIL, dTD_ptr);

        if (!dTD_ptr)
        {
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"Error: Can't get dTD\n");

            ErrorLogPrintf("Error: Can't get dTD");
            _usb_dci_vusb20_get_dtd_information();

            USB2MgrDeviceUnplugPlug();
            return USB_OK;

        } /* Endif */

        MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_add_count++));

        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRANSFER,
                        "add_dTD_%d: ep=%d %s, size=%d, CURR_dTD=0x%x, xd_ptr=0x%x, dTD_ptr=0x%x\n",
                        mv_usb_add_count, xd_ptr->EP_NUM, xd_ptr->BDIRECTION ? "SEND" : "RECV",
                        (int)remaining_len, (unsigned)USB_32BIT_LE(ep_queue_head_ptr->CURR_DTD_PTR),
                        (unsigned)xd_ptr, (unsigned)dTD_ptr);

        remaining_len -= curr_pkt_len;

        usb_dev_ptr->DTD_ENTRIES--;

        if (curr_offset == 0)
        {
            first_dTD_ptr = dTD_ptr;
        } /* Endif */

        /* Zero the dTD. Leave the last 4 bytes as that is the scratch pointer */
        USB_memzero((void *) dTD_ptr,(sizeof(VUSB20_EP_TR_STRUCT) - 4));

        /* Initialize the dTD */
        dTD_ptr->SCRATCH_PTR->PRIVATE = handle;

        /* Set the Terminate bit */
        dTD_ptr->NEXT_TR_ELEM_PTR = USB_32BIT_LE(VUSB_EP_QUEUE_HEAD_NEXT_TERMINATE);

        /*************************************************************
        FIX ME: For hig-speed and high-bandwidth ISO IN endpoints,
        we must initialize the multiplied field so that Host can issues
        multiple IN transactions on the endpoint. See the DTD data
        structure for MultiIO field.

        S Garg 11/06/2003
        *************************************************************/

        /* Fill in the transfer size */
        if (!remaining_len)
        {
            dTD_ptr->SIZE_IOC_STS = USB_32BIT_LE((curr_pkt_len <<
                    VUSBHS_TD_LENGTH_BIT_POS) | (VUSBHS_TD_IOC) | (VUSBHS_TD_STATUS_ACTIVE));
        }
        else
        {
            dTD_ptr->SIZE_IOC_STS = USB_32BIT_LE((curr_pkt_len << VUSBHS_TD_LENGTH_BIT_POS)
                                                   | VUSBHS_TD_STATUS_ACTIVE);
        } /* Endif */

        /* Set the reserved field to 0 */
        dTD_ptr->SIZE_IOC_STS &= ~USB_32BIT_LE(VUSBHS_TD_RESERVED_FIELDS);

        /* 4K apart buffer page pointers */
        if(xd_ptr->WSTARTADDRESS != NULL)
        {
            uint_32 physAddr = USB_virt_to_phys((uint_8*)xd_ptr->WSTARTADDRESS + curr_offset);

            dTD_ptr->BUFF_PTR0 = USB_32BIT_LE(physAddr);

            physAddr += 4096;
            dTD_ptr->BUFF_PTR1 = USB_32BIT_LE(physAddr);

            physAddr += 4096;
            dTD_ptr->BUFF_PTR2 = USB_32BIT_LE(physAddr);

            physAddr += 4096;
            dTD_ptr->BUFF_PTR3 = USB_32BIT_LE(physAddr);

            physAddr += 4096;
            dTD_ptr->BUFF_PTR4 = USB_32BIT_LE(physAddr);
        }
        else
        {
            dTD_ptr->BUFF_PTR0 = dTD_ptr->BUFF_PTR1 = dTD_ptr->BUFF_PTR2 = 0;
            dTD_ptr->BUFF_PTR3 = dTD_ptr->BUFF_PTR4 = 0;
        }
        curr_offset += curr_pkt_len;

      /* Maintain the first and last device transfer descriptor per
      ** endpoint and direction
      */
      if (!usb_dev_ptr->EP_DTD_HEADS[temp])
      {
         usb_dev_ptr->EP_DTD_HEADS[temp] = dTD_ptr;
         /***********************************************
         If list does not have a head, it means that list
         is empty. An empty condition is detected.
         ***********************************************/
         list_empty = TRUE;
      } /* Endif */

      /* Check if the transfer is to be queued at the end or beginning */
      temp_dTD_ptr = usb_dev_ptr->EP_DTD_TAILS[temp];

      /* Remember which XD to use for this dTD */
      dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD = (pointer)xd_ptr;

      /* New tail */
      usb_dev_ptr->EP_DTD_TAILS[temp] = dTD_ptr;
      if (temp_dTD_ptr)
      {
         /* Should not do |=. The Terminate bit should be zero */
         temp_dTD_ptr->NEXT_TR_ELEM_PTR = (uint_32)USB_32BIT_LE(USB_VIRT_TO_PHYS(usb_dev_ptr, dTD_ptr));
      } /* Endif */
   } while (remaining_len); /* EndWhile */


   /**************************************************************
   In the loop above DTD has already been added to the list
   However endpoint has not been primed yet. If list is not empty
   we need safter ways to add DTD to the existing list.
   Else we just skip to adding DTD to QH safely.
   **************************************************************/

   if(list_empty == FALSE)
   {
        /*********************************************************
        Hardware v3.2+ require the use of semaphore to ensure that
        QH is safely updated.
        *********************************************************/
        MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_add_not_empty_count++));


        /*********************************************************
        Check the prime bit. If set goto done
        *********************************************************/
        if( USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos)
        {
           goto done;
        }

        read_safe = FALSE;
        timeout = 10000;
        while(read_safe == FALSE)
        {
            timeout--;
            if(timeout <= 0)
            {

				ErrorLogPrintf("%s: Timeout for ATDTW_TRIPWIRE reg = 0x%x\n", __FUNCTION__,
                    (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD));

                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Timeout for ATDTW_TRIPWIRE reg = 0x%x\n", __FUNCTION__,
                    (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD));
                return USBERR_TR_FAILED;
            }

           /*********************************************************
           start with setting the semaphores
           *********************************************************/
           dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD |=
                                          USB_32BIT_LE(EHCI_CMD_ATDTW_TRIPWIRE_SET);

           /*********************************************************
           Read the endpoint status
           *********************************************************/
           temp_ep_stat = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos;

           /*********************************************************
           Reread the ATDTW semaphore bit to check if it is cleared.
           When hardware see a hazard, it will clear the bit or
           else we remain set to 1 and we can proceed with priming
           of endpoint if not already primed.
           *********************************************************/
           if( USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD) &
                                          EHCI_CMD_ATDTW_TRIPWIRE_SET)
           {
               read_safe = TRUE;
           }

        }/*end while loop */

        /*********************************************************
        Clear the semaphore
        *********************************************************/
        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &=
                                       USB_32BIT_LE(EHCI_CMD_ATDTW_TRIPWIRE_CLEAR);

        /*********************************************************
         * If endpoint is not active, we activate it now.
         *********************************************************/
         if(!temp_ep_stat)
         {
            /* No other transfers on the queue */
            ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)USB_32BIT_LE(
                        USB_VIRT_TO_PHYS(usb_dev_ptr, first_dTD_ptr));
            ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

            /* Read DQH to wait for DDR ready before prime endpoint. */
            for(index1 = 0; index1 < 1; index1++)
            {
                index2++;
            }

            index2 = ep_queue_head_ptr->MAX_PKT_LENGTH;

            /* Prime the Endpoint */
            dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = USB_32BIT_LE(bit_pos);
         }
   }
   else
   {
         /* No other transfers on the queue */
         ep_queue_head_ptr->NEXT_DTD_PTR = (uint_32)USB_32BIT_LE(
                            USB_VIRT_TO_PHYS(usb_dev_ptr, first_dTD_ptr));
         ep_queue_head_ptr->SIZE_IOC_INT_STS = 0;

         /* Read DQH to wait for DDR ready before prime endpoint. */
         for(index1 = 0; index1 < 1; index1++)
         {
             index2++;
         }

         index2 = ep_queue_head_ptr->MAX_PKT_LENGTH;

         /* Prime the Endpoint */
         dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME = USB_32BIT_LE(bit_pos);
   }

done:
    /* wait a little */
	 {

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

	 }

   return USB_OK;
   /* End CR 1015 */
} /* EndBody */
extern volatile usbIntDebug_t usbIntDebugLog[16];
volatile unsigned long usbIntDebugLog_index = 0;
extern UINT32 EEHandlerFlag;


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_tr_complete
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_tr_complete
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   volatile VUSB20_REG_STRUCT_PTR               dev_ptr;
   volatile VUSB20_EP_TR_STRUCT_PTR             dTD_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      temp_dTD_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;
   uint_32                                      temp, i, ep_num = 0, direction = 0, bit_pos;
   uint_32                                      remaining_length = 0;
   uint_32                                      actual_transfer_length = 0;
   uint_32                                      counter, errors = 0;
   XD_STRUCT_PTR                                xd_ptr;
   XD_STRUCT_PTR                                temp_xd_ptr = NULL;
   uint_8_ptr                                    buff_start_address = NULL;
   boolean                                      endpoint_detected = FALSE;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "process_tr_complete_isr\n");
   MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_complete_isr_count++));

   /* We use separate loops for ENDPTSETUPSTAT and ENDPTCOMPLETE because the
   ** setup packets are to be read ASAP
   */

   /* Process all Setup packet received interrupts */
   bit_pos = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT);
   if(EEHandlerFlag == 0){
   usbIntDebugLog[usbIntDebugLog_index].ep0 = bit_pos;
   	}
   if (bit_pos)
   {
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "setup_isr: bit_pos=0x%x\n", (unsigned)bit_pos);
      for(i=0; i<USB_MAX_CONTROL_ENDPOINTS; i++)
      {
         if (bit_pos & (1 << i))
         {
            MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_setup_count++));
            _usb_device_call_service(handle, i, TRUE, 0, 0, 8, 0);
         } /* Endif */
      } /* Endfor */
   } /* Endif */

   /* Don't clear the endpoint setup status register here. It is cleared as a
   ** setup packet is read out of the buffer
   */

   /* Process non-setup transaction complete interrupts */
   bit_pos = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE);
   if(EEHandlerFlag == 0){
   usbIntDebugLog[usbIntDebugLog_index].epx = bit_pos;
   	}
   /* Clear the bits in the register */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE = USB_32BIT_LE(bit_pos);

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "tr_complete: bit_pos = 0x%x\n", (unsigned)bit_pos);
   if (bit_pos)
   {
        MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_complete_count++));

        /* Get the endpoint number and the direction of transfer */
        counter = 0;
        for (i=0; i<(MV_USB_MAX_ENDPOINTS*2); i++)
        {
            endpoint_detected = FALSE;
            if ((i < MV_USB_MAX_ENDPOINTS) && (bit_pos & (1 << i)))
            {
                ep_num = i;
                direction = 0;
                endpoint_detected = TRUE;
            }
            else
            {
                if( (i >= MV_USB_MAX_ENDPOINTS) &&
                    (bit_pos & (1 << (i+16-MV_USB_MAX_ENDPOINTS))))
                {
                    ep_num = (i - MV_USB_MAX_ENDPOINTS);
                    direction = 1;
                    endpoint_detected = TRUE;
                }
            }

            if(endpoint_detected)
            {
                temp = (2*ep_num + direction);

                /* Get the first dTD */
                dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

                ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR + temp;

                /* Process all the dTDs for respective transfers */
                while (dTD_ptr)
                {
                    if (USB_32BIT_LE(dTD_ptr->SIZE_IOC_STS) & VUSBHS_TD_STATUS_ACTIVE)
                    {
                        /* No more dTDs to process. Next one is owned by VUSB */
                        if(counter == 0)
                        {
                            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "tr_complete - break: ep=%d %s, bit_pos=0x%x\n",
                                    (unsigned)ep_num, direction ? "SEND" : "RECV", (unsigned)bit_pos);

                            MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_empty_complete_count++));
                        }
                        break;
                    } /* Endif */

                    /* Get the correct internal transfer descriptor */
                    xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
                    if (xd_ptr)
                    {
                        buff_start_address = xd_ptr->WSTARTADDRESS;
                        actual_transfer_length = xd_ptr->WTOTALLENGTH;
                        temp_xd_ptr = xd_ptr;
                    } /* Endif */

                    /* Get the address of the next dTD */
                    temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(usb_dev_ptr,
                                (uint_32)(USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK) );

                    /* Read the errors */
                    errors = (USB_32BIT_LE(dTD_ptr->SIZE_IOC_STS) & VUSBHS_TD_ERROR_MASK);
                    if (!errors)
                    {
                        /* No errors */
                        /* Get the length of transfer from the current dTD */
                        remaining_length += ((USB_32BIT_LE(dTD_ptr->SIZE_IOC_STS) & VUSB_EP_TR_PACKET_SIZE) >> 16);
                        actual_transfer_length -= remaining_length;
                    }
                    else
                    {
                        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"complete_tr error: ep=%d %s: error = 0x%x\n",
                                    (unsigned)ep_num, direction ? "SEND" : "RECV", (unsigned)errors);
                        if (errors & VUSBHS_TD_STATUS_HALTED)
                        {
                            /* Clear the errors and Halt condition */
                            ep_queue_head_ptr->SIZE_IOC_INT_STS &= ~errors;
                        } /* Endif */

                        ErrorLogPrintf("_usb_dci_vusb20_process_tr_complete, errors:%x", errors);

                    } /* Endif */

                    /* Retire the processed dTD */
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "tr_complete - cancel: ep=%d %s, bit_pos = 0x%x\n",
                                    (unsigned)ep_num, direction ? "SEND" : "RECV", (unsigned)bit_pos);

                    counter++;
                    _usb_dci_vusb20_cancel_transfer(handle, ep_num, direction);
                    if( (temp_dTD_ptr == NULL) ||
                        (temp_dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD != temp_xd_ptr) )
                    {
                        /* Transfer complete. Call the register service function for the endpoint */
                        MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_complete_ep_count[temp]++));
                        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRANSFER,
                            "tr_complete completed: ep=%d %s, pBuf=%p, size=%d, errors=0x%x\n",
                            (unsigned)ep_num, direction ? "SEND" : "RECV", buff_start_address,
                            actual_transfer_length, errors);
#if 0
					///////////////////////////
				      {
				      		extern unsigned long usb_is_busy;
							usb_is_busy &= ~(1<<ep_num);
					  }
#endif
					///////////////////////////

                        _usb_device_call_service(handle, ep_num, FALSE, direction,
                                   buff_start_address, actual_transfer_length, errors);
                        remaining_length = 0;
                    } /* Endif */
                    else
                    {
                        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRANSFER, "tr_complete not completed: ep=%d %s\n",
                                (unsigned)ep_num, direction ? "SEND" : "RECV");
                    }
                    dTD_ptr = temp_dTD_ptr;
                    errors = 0;
                } /* Endwhile */
            } /* Endif */
        } /* Endfor */
        MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS,
               ( {if(mv_usb_complete_max_count < counter) mv_usb_complete_max_count = counter;}));
   } /* Endif */
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_isr
*  Returned Value : None
*  Comments       :
*        Services all the VUSB_HS interrupt sources
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_isr
(
      _usb_device_handle handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      status;
   static uint_32 index = 0;

   MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_isr_count++));

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   status = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS);

   status &= USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR);


#ifdef PLAT_USE_ALIOS
   if((status == 0) && (EEHandlerFlag == 0))
#else
   if(status == 0)
#endif   
   {
   		
       *(volatile unsigned long*)0xD428287C |= 0x1<<4; //clear USB wakeup int
       triggerUsbWakeUpHisr();

       MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_empty_isr_count++));
       return;
   } /* Endif */
   if(EEHandlerFlag == 0){
	memset((UINT8*)&usbIntDebugLog[usbIntDebugLog_index], 0, sizeof(usbIntDebug_t));
	usbIntDebugLog[usbIntDebugLog_index].timeStamp = timerCountRead(TS_TIMER_ID);

   	}
    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_STATS, "%s: INTERRUPT STATUS=0x%x\n", __FUNCTION__, status);

   /* Clear all the interrupts occured */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS = USB_32BIT_LE(status);

   if (status & EHCI_STS_RESET)
   {      
   	  ErrorLogPrintf("USB Reset");
      _usbDeviceDebug_int_reset++;
      _usb_dci_vusb20_process_reset((pointer)usb_dev_ptr);
   } /* Endif */

   if (status & EHCI_STS_PORT_CHANGE)
   {
      ErrorLogPrintf("USB Port change");
      _usbDeviceDebug_int_port_change++;
      _usb_dci_vusb20_process_port_change((pointer)usb_dev_ptr);
   } /* Endif */

   if (status & EHCI_STS_ERR)
   {
	  ErrorLogPrintf("USB Err");
      _usbDeviceDebug_int_err++;
      _usb_dci_vusb20_process_error((pointer)usb_dev_ptr);
   } /* Endif */

   if (status & EHCI_STS_SOF)
   {
      _usb_dci_vusb20_process_SOF((pointer)usb_dev_ptr);
   } /* Endif */

   if (status & EHCI_STS_INT)
   {

   	  if(!USBConnectStatus)
      {
    	 USBConnectStatus = TRUE;
      }


	  do
	  {
	      _usbDeviceDebug_int_int++;
	      _usb_dci_vusb20_process_tr_complete((pointer)usb_dev_ptr);
	  } while ( dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE );
   } /* Endif */
	if(EEHandlerFlag == 0){
	usbIntDebugLog_index++;
	if(usbIntDebugLog_index > 15)
		usbIntDebugLog_index = 0;
		}
	if (status & EHCI_STS_SUSPEND)
	{
		_usbDeviceDebug_int_suspend++;			
        ErrorLogPrintf("USB Suspend");
		_usb_dci_vusb20_process_suspend((pointer)usb_dev_ptr);
        _usb_device_set_status(_usbDeviceHandle, MV_USB_STATUS_CURRENT_CONFIG, 0);
		return;
	} /* Endif */

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_diag_tx_isr
*  Returned Value : None
*  Comments       :
*        Services all the VUSB_HS interrupt sources
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_diag_tx_isr(void)
{
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR                        dev_ptr;
    uint_32                                      bit_pos;
    uint_32                                      status;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle;
    if((usb_dev_ptr == NULL)||(!assertFlag))
    {
        return;
    }

    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    status = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS);
    status &= USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR);

    if(status == 0)
    {
        return;
    }

    /* Clear all the interrupts occured */
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_STS = USB_32BIT_LE(status);

    if (dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT & (1 << 0))
    {
        _usb_device_stall_endpoint(_usbDeviceHandle, 0, 0);
        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = (1 << 0);
    }

    bit_pos = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE);
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE = USB_32BIT_LE(bit_pos);

    if(bit_pos & (1<<(DIAG_TX_ENDPOINT+16)))
    {
        /* Clear the bits in the register */
        _usb_dci_vusb20_cancel_transfer(_usbDeviceHandle, DIAG_TX_ENDPOINT, MV_USB_SEND);
        //diagUsbBusy = FALSE;
    }

#ifdef MV_USB2_MASS_STORAGE
    if(bit_pos & (1<<USB_MASS_STORAGE_RX_ENDPOINT))
    {
        /* Clear the bits in the register */
        _usb_dci_vusb20_cancel_transfer(_usbDeviceHandle, USB_MASS_STORAGE_RX_ENDPOINT, MV_USB_RECV);
        mvUsbStorage_test_unit_ready_response();
    }

    if(bit_pos & (1<<(USB_MASS_STORAGE_TX_ENDPOINT+16)))
    {
        /* Clear the bits in the register */
        _usb_dci_vusb20_cancel_transfer(_usbDeviceHandle, USB_MASS_STORAGE_TX_ENDPOINT, MV_USB_SEND);
    }
#endif
}

extern UINT32 sulog_status(void);
void print_USB_debug_info(void)
{
	int i = 0;
//	if(USBRestFailed)
    uart_printf("USB rest:%d\n",_usbDeviceDebug_int_reset);
	sulog_status();
	uart_printf("\r\nUSB_MODE 0x%lx;",global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_MODE);
	uart_printf("ENDPT_SETUP_STAT 0x%lx;",global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT);
	uart_printf("ENDPTPRIME 0x%lx;",global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME);
	uart_printf("ENDPTFLUSH 0x%lx;",global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH);
	uart_printf("ENDPTSTATUS 0x%lx;",global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS);
	uart_printf("1 ENDPTCOMPLETE 0x%lx\r\n",global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE);
	for(i=0;i<15;i++)
	{	
		uart_printf("ENDPTCTRLX %d 0x%lx\r\n",i,global_mv_usb_dev->DEV_PTR->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[i]);
	}

}
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_reset
*  Returned Value : None
*  Comments       :
*        Services USB BUS reset interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_reset
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR          usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;
   uint_32                           temp;
   uint_8                            i;
   uint_32 try_count = 0;
   uint_32 printed = 0;
   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "process_reset\n");
   MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_reset_count++));

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Inform the application so that it can cancel all previously queued transfers */
   //_usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_BUS_RESET, 0, 0, 0, 0, 0);

   /* The address bits are past bit 25-31. Set the address */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR &= ~USB_32BIT_LE(0xFE000000);

   /* Clear all the setup token semaphores */
   temp = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT);
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = USB_32BIT_LE(temp);

   /* Clear all the endpoint complete status bits */
   temp = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE);
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCOMPLETE = USB_32BIT_LE(temp);

   // Wait for all primes to flush
   //------------------------------------------
   while (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & 0xFFFFFFFF)
   {
   		try_count++;
		if(try_count > 10000 && printed ==0)
		{
			print_USB_debug_info();
			printed = 1;
			break;
		}
      /* Wait until all ENDPTPRIME bits cleared */
   } /* Endif */

   /* Write 1s to the Flush register */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTFLUSH = USB_32BIT_LE(0xFFFFFFFF);

   /* Unstall all endpoints */
   for (i=0;i<usb_dev_ptr->MAX_ENDPOINTS;i++) {
      _usb_dci_vusb20_unstall_endpoint(handle, i, 0);
      _usb_dci_vusb20_unstall_endpoint(handle, i, 1);
   } /* Endfor */

   if (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/) &
                                                EHCI_PORTSCX_PORT_RESET)
   {
      usb_dev_ptr->BUS_RESETTING = TRUE;
      usb_dev_ptr->USB_STATE = MV_USB_STATE_POWERED;
   }
												
#if 0
   else
   {
      /* re-initialize */
      _usb_dci_vusb20_chip_initialize((pointer)usb_dev_ptr);
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INIT, "process_reset, Chip reinit hw\n");
      return;
   } /* Endif */
#endif

   _usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_BUS_RESET, 0, 0, 0, 0, 0);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_suspend
*  Returned Value : None
*  Comments       :
*        Services suspend interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_suspend
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR     usb_dev_ptr;

   mvUsbGetDescInfo *pUsbDesInfo = mvUsbGetDescriptorInfo();

   memset((void *)pUsbDesInfo, 0x00, sizeof(mvUsbGetDescInfo));

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "process_suspend\n");
   MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_suspend_count++));

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;

   usb_dev_ptr->USB_STATE = MV_USB_STATE_SUSPEND;

   USB_TRACE("USB Suspend");


	//CommPMUsbWakeupHandle(1);


   /* Inform the upper layers */
   _usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_SLEEP, 0, 0, 0, 0, 0);

    //PlatformUsbSetSuspendTicks(OSAGetTicks());
} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_SOF
*  Returned Value : None
*  Comments       :
*        Services SOF interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_SOF
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "process_SOF\n");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Inform the upper layer */
   _usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_SOF, 0, 0, 0,
      USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_FRINDEX), 0);

} /* EndBody */


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_port_change
*  Returned Value : None
*  Comments       :
*        Services port change detect interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_port_change
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ISR, "process_port_change\n");
   MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_port_change_count++));

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   USB_TRACE("USB Port Change");
   //PlatformUsbSetSuspendTicks(0xFFFFFFFF);


    //plug-out triggers sleep
	if(!IS_BIT_ON( dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX,
		EHCI_PORTSCX_CURRENT_CONNECT_STATUS))
	{
	    ErrorLogPrintf("U Suspend");
		return;
	}


    USB2DeviceEnableEndpoint();

   if (usb_dev_ptr->BUS_RESETTING) {
      /* Bus reset operation complete */
      usb_dev_ptr->BUS_RESETTING = FALSE;
   } /* Endif */

   if (!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/) &
                                                EHCI_PORTSCX_PORT_RESET))
   {
      /* Get the speed */
      if (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/) &
                                            EHCI_PORTSCX_PORT_HIGH_SPEED)
      {
         usb_dev_ptr->SPEED = MV_USB_SPEED_HIGH;
      }
      else
      {
         usb_dev_ptr->SPEED = MV_USB_SPEED_FULL;
      } /* Endif */

      /* Inform the upper layers of the speed of operation */
      _usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_SPEED_DETECTION, 0, 0,
                                0, usb_dev_ptr->SPEED, 0);

   } /* Endif */

   if (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/) &
                                                EHCI_PORTSCX_PORT_SUSPEND)
   {

      usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND = usb_dev_ptr->USB_STATE;
      usb_dev_ptr->USB_STATE = MV_USB_STATE_SUSPEND;

      /* Inform the upper layers */
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO,"USB suspend\n");
      _usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_SUSPEND, 0, 0, 0, 0, 0);
   } /* Endif */

   if (!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[0]*/) & EHCI_PORTSCX_PORT_SUSPEND)
                    && (usb_dev_ptr->USB_STATE == MV_USB_STATE_SUSPEND))
   {
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO,"USB resume\n");
      usb_dev_ptr->USB_STATE = usb_dev_ptr->USB_DEV_STATE_B4_SUSPEND;
      /* Inform the upper layers */
      _usb_device_call_service(usb_dev_ptr, MV_USB_SERVICE_RESUME, 0, 0, 0, 0, 0);

      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_SUSPEND, "process_port_change, SUCCESSFUL, resumed\n");
      return;
   } /* Endif */

   usb_dev_ptr->USB_STATE = MV_USB_STATE_DEFAULT;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_error
*  Returned Value : None
*  Comments       :
*        Services error interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_process_error
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR          usb_dev_ptr;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ERROR, "process_error\n");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /* Increment the error count */
   usb_dev_ptr->ERRORS++;


   ErrorLogPrintf("%s", __FUNCTION__);


} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_set_speed_full
*  Returned Value : None
*  Comments       :
*        Force the controller port in full speed mode.
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_set_speed_full
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* The port number on the device */
      uint_8                     port_number
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      port_control;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO, "FORCE set_speed_full\n");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   port_control = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port_number]*/);
   port_control |= EHCI_PORTSCX_FORCE_FULL_SPEED_CONNECT;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port_number]*/ = USB_32BIT_LE(port_control);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_suspend_phy
*  Returned Value : None
*  Comments       :
*        Suspends the PHY in low power mode
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_suspend_phy
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* The port number on the device */
      uint_8                     port_number
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   uint_32                                      port_control;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_SUSPEND, "set_suspend_phy\n");

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   port_control = USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port_number]*/);
   port_control |= EHCI_PORTSCX_PHY_CLOCK_DISABLE;
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port_number]*/ = USB_32BIT_LE(port_control);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_set_address
*  Returned Value : None
*  Comments       :
*        Sets the newly assigned device address
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_set_address
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* Address of the device assigned by the host */
      uint_8                     address
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR          usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR             dev_ptr;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "set_address: address=%d\n",address);

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

#ifdef SET_ADDRESS_HARDWARE_ASSISTANCE
   /***********************************************************
   Hardware Rev 4.0 onwards have special assistance built in
   for handling the set_address command. As per the USB specs
   a device should be able to receive the response on a new
   address, within 2 msecs after status phase of set_address is
   completed. Since 2 mili second may be a very small time window
   (on high interrupt latency systems) before software could
   come to the code below and write the device register,
   this routine will be called in advance when status phase of
   set_address is still not finished. The following line in the
   code will set the bit 24 to '1' and hardware will take
   the address and queue it in an internal buffer. From which
   it will use it to decode the next USB token. Please look
   at hardware rev details for the implementation of this
   assistance.

   Also note that writing bit 24 to 0x01 will not break
   any old hardware revs because it was an unused bit.
   ***********************************************************/
   /* The address bits are past bit 25-31. Set the address
   also set the bit 24 to 0x01 to start hardware assitance*/
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR =
      USB_32BIT_LE((uint_32)address << VUSBHS_ADDRESS_BIT_SHIFT) |
      (0x01 << (VUSBHS_ADDRESS_BIT_SHIFT -1));
#else
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.DEVICE_ADDR =
      USB_32BIT_LE((uint_32)address << VUSBHS_ADDRESS_BIT_SHIFT);
#endif /* SET_ADDRESS_HARDWARE_ASSISTANCE */

   usb_dev_ptr->USB_STATE = MV_USB_STATE_ADDRESS;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_setup_data
*  Returned Value : None
*  Comments       :
*        Reads the Setup data from the 8-byte setup buffer
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_get_setup_data
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [OUT] address of the buffer to read the setup data into */
      uint_8_ptr                  buffer_ptr
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   volatile VUSB20_REG_STRUCT_PTR               dev_ptr;
   volatile VUSB20_EP_QUEUE_HEAD_STRUCT_PTR     ep_queue_head_ptr;
   volatile boolean                             read_safe = FALSE;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Get the endpoint queue head */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR +
                                                                2*ep_num + MV_USB_RECV;

   /********************************************************************
   CR 1219. Hardware versions 2.3+ have a implementation of tripwire
   semaphore mechanism that requires that we read the contents of
   QH safely by using the semaphore. Read the USBHS document to under
   stand how the code uses the semaphore mechanism. The following are
   the steps in brief

   1. USBCMD Write 1 to Setup Tripwire in register.
   2. Duplicate contents of dQH.StatusBuffer into local software byte
      array.
   3  Read Setup TripWire in register. (if set - continue; if
      cleared goto 1.)
   4. Write '0' to clear Setup Tripwire in register.
   5. Process setup packet using local software byte array copy and
      execute status/handshake phases.


   ********************************************************************/
  while(!read_safe)
  {
      /*********************************************************
      start with setting the semaphores
      *********************************************************/

      dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD |=
                                                USB_32BIT_LE(EHCI_CMD_SETUP_TRIPWIRE_SET);

      /* Copy the setup packet to private buffer */
      USB_memcopy((uint_8_ptr)ep_queue_head_ptr->SETUP_BUFFER, buffer_ptr, 8);

      /* Reset Usb Request type */
      ep_queue_head_ptr->SETUP_BUFFER[0] = 0x60;

      /*********************************************************
      If setup tripwire semaphore is cleared by hardware it means
      that we have a danger and we need to restart.
      else we can exit out of loop safely.
      *********************************************************/
      if(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD) &
                                                   EHCI_CMD_SETUP_TRIPWIRE_SET)
      {
          read_safe = TRUE; /* we can proceed exiting out of loop*/
      }
      MV_USB_DEBUG_CODE(MV_USB_DEBUG_FLAG_STATS, (mv_usb_read_setup_count++));
  }

   /*********************************************************
   Clear the semaphore bit now
   *********************************************************/
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &=
                                    USB_32BIT_LE(EHCI_CMD_SETUP_TRIPWIRE_CLEAR);

   /* Clear the bit in the ENDPTSETUPSTAT */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPT_SETUP_STAT = USB_32BIT_LE(1 << ep_num);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_init_endpoint
*  Returned Value : None
*  Comments       :
*        Initializes the specified endpoint and the endpoint queue head
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_init_endpoint
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the transaction descriptor address */
      XD_STRUCT_PTR              xd_ptr
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_QUEUE_HEAD_STRUCT _PTR_            ep_queue_head_ptr;
   uint_32                                      bit_pos;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Get the endpoint queue head address */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR +
                                                    2*xd_ptr->EP_NUM + xd_ptr->BDIRECTION;

   bit_pos = (1 << (16 * xd_ptr->BDIRECTION + xd_ptr->EP_NUM));

   /* Check if the Endpoint is Primed */
   if ((!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos)) &&
       (!(USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)))
   {
      /* Set the max packet length, interrupt on Setup and Mult fields */
      if (xd_ptr->EP_TYPE == MV_USB_ISOCHRONOUS_ENDPOINT)
      {
         /* Mult bit should be set for isochronous endpoints */
         ep_queue_head_ptr->MAX_PKT_LENGTH = USB_32BIT_LE((xd_ptr->WMAXPACKETSIZE << 16) |
            ((xd_ptr->MAX_PKTS_PER_UFRAME ?  xd_ptr->MAX_PKTS_PER_UFRAME : 1) <<
            VUSB_EP_QUEUE_HEAD_MULT_POS));
      }
      else
      {
         if (xd_ptr->EP_TYPE != MV_USB_CONTROL_ENDPOINT)
         {
             /* BULK or INTERRUPT */
            ep_queue_head_ptr->MAX_PKT_LENGTH = USB_32BIT_LE((xd_ptr->WMAXPACKETSIZE << 16) |
               (xd_ptr->DONT_ZERO_TERMINATE ? VUSB_EP_QUEUE_HEAD_ZERO_LEN_TER_SEL : 0));
         }
         else
         {
             /* CONTROL */
            ep_queue_head_ptr->MAX_PKT_LENGTH = USB_32BIT_LE((xd_ptr->WMAXPACKETSIZE << 16) |
                                                    VUSB_EP_QUEUE_HEAD_IOS);
         } /* Endif */
      } /* Endif */

      /* Enable the endpoint for Rx and Tx and set the endpoint type */
      if(xd_ptr->BDIRECTION  == MV_USB_SEND)
      {
        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM] |=
                         USB_32BIT_LE((EHCI_EPCTRL_TX_ENABLE | EHCI_EPCTRL_TX_DATA_TOGGLE_RST) |
                          (xd_ptr->EP_TYPE << EHCI_EPCTRL_TX_EP_TYPE_SHIFT));
      }
      else
      {
        dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM] |=
                         USB_32BIT_LE((EHCI_EPCTRL_RX_ENABLE | EHCI_EPCTRL_RX_DATA_TOGGLE_RST) |
                          (xd_ptr->EP_TYPE << EHCI_EPCTRL_RX_EP_TYPE_SHIFT));
      }
      /* Note: Disabled endpoint must be changed from default control type */
      if( (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM]) &
            EHCI_EPCTRL_RX_ENABLE) == 0)
      {
          dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM] |=
              USB_32BIT_LE(MV_USB_BULK_ENDPOINT << EHCI_EPCTRL_RX_EP_TYPE_SHIFT);
      }

      if( (USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM]) &
            EHCI_EPCTRL_TX_ENABLE) == 0)
      {
          dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM] |=
              USB_32BIT_LE(MV_USB_BULK_ENDPOINT << EHCI_EPCTRL_TX_EP_TYPE_SHIFT);
      }

      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INIT,
                    "init ep #%d %s: type=0x%x, max_pkt=%d, zero_term=%s, ENDPTCTRLX=0x%x\n",
                        xd_ptr->EP_NUM, xd_ptr->BDIRECTION ? "SEND" : "RECV", xd_ptr->EP_TYPE,
                        xd_ptr->WMAXPACKETSIZE, xd_ptr->DONT_ZERO_TERMINATE ? "NO" : "YES",
                        (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[xd_ptr->EP_NUM]));
   }
   else
   {
      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"ep=%d %s: Init ERROR: ENDPTPRIME=0x%x, ENDPTSTATUS=0x%x, bit_pos=0x%x\n",
                (unsigned)xd_ptr->EP_NUM, xd_ptr->BDIRECTION ? "SEND" : "RECV",
                (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME),
                (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS),
                (unsigned)bit_pos);
        return USBERR_EP_INIT_FAILED;
   } /* Endif */

   return USB_OK;

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_transfer_status
*  Returned Value : USB_OK or error code
*  Comments       :
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_get_transfer_status
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
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr;
   XD_STRUCT_PTR                                xd_ptr;
   uint_8                                       status;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;

   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2*ep_num + direction];

   if (dTD_ptr)
   {
      /* Get the transfer descriptor for the dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      status = xd_ptr->BSTATUS;
   }
   else
   {
      status = MV_USB_STATUS_IDLE;
   } /* Endif */

   return (status);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_dtd_cnt
*  Returned Value : USB_OK or error code
*  Comments       :
*        Gets the packet address of a transfer
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_get_dtd_cnt
(
      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction,

      /* [OUT] address */
      uint_32_ptr                StartTick
)
{ /* Body */
    USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
    volatile VUSB20_EP_TR_STRUCT_PTR             dTD_ptr;
    XD_STRUCT_PTR                                xd_ptr;
    VUSB20_EP_TR_STRUCT_PTR                      temp_dTD_ptr;
    uint_32                                      temp;
    uint_32                                      count = 0;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle;
    ASSERT(usb_dev_ptr != NULL);

    /* MV_USB_RECV: 0, MV_USB_SEND: 1 */
    temp = (2*ep_num + direction);

    /* Get the first dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

    if (dTD_ptr)
    {
        if(StartTick != NULL)
        {
            /* Get the transfer descriptor for the dTD */
            xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;

            if((xd_ptr != NULL) && (xd_ptr->WSTARTADDRESS != NULL))
            {
                switch(ep_num)
                {
                    case USB_NET_TX_ENDPOINT:
                    {
                        *StartTick = *((uint_32_ptr)((uint_32)xd_ptr->WSTARTADDRESS - PMSG_SIZE));
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
        }

        /* Process all the dTDs for respective transfers */
        while (dTD_ptr)
        {
            /* Get the address of the next dTD */
            temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(usb_dev_ptr,
                        (uint_32)(USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK) );

            dTD_ptr = temp_dTD_ptr;
            count++;
        } /* Endwhile */

    }

    return count;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_transfer_details
*  Returned Value : pointer to structure that has details for transfer
*        Gets the status of a transfer
*
*END*-----------------------------------------------------------------*/
XD_STRUCT_PTR  _usb_dci_vusb20_get_transfer_details
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
//   VUSB20_REG_STRUCT_PTR                        dev_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      dTD_ptr, temp_dTD_ptr;
   XD_STRUCT_PTR                                xd_ptr;
   uint_32                                      temp, remaining_bytes;
   VUSB20_EP_QUEUE_HEAD_STRUCT_PTR              ep_queue_head_ptr;


   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
//   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
   temp = (2*ep_num + direction);

   /* get a pointer to QH for this endpoint */
   ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR + temp;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_TRACE, "get_transfer_details\n");

   /* Unlink the dTD */
   dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[2*ep_num + direction];

   if (dTD_ptr)
   {
      /* Get the transfer descriptor for the dTD */
      xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
      if(!xd_ptr) return NULL;

      /* Initialize the transfer length field */
      xd_ptr->WSOFAR =0;
      remaining_bytes =0;

      /*if length of this transfer is greater than 20K
      we have multiple DTDs to count */
      if(xd_ptr->WTOTALLENGTH > VUSB_EP_MAX_LENGTH_TRANSFER)
      {
         /* it is a valid DTD. We should parse all DTDs for this XD
         and find the total bytes used so far */
         temp_dTD_ptr = dTD_ptr;

         /*loop through the list of DTDS until an active DTD is found
         or list has finished */
         while(!(USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_NEXT_TERMINATE))
         {

            /**********************************************************
            If this DTD has been overlayed, we take the actual length
            from QH.
            **********************************************************/

            if (/*(uint_32)*/(USB_32BIT_LE(ep_queue_head_ptr->CURR_DTD_PTR) & VUSBHS_TD_ADDR_MASK) ==
                                     (uint_32)USB_VIRT_TO_PHYS(usb_dev_ptr, temp_dTD_ptr) )
            {
                remaining_bytes +=
                  ((USB_32BIT_LE(ep_queue_head_ptr->SIZE_IOC_INT_STS) & VUSB_EP_TR_PACKET_SIZE) >> 16);
            }
            else
            {
               /* take the length from DTD itself */
                remaining_bytes +=
                  ((USB_32BIT_LE(temp_dTD_ptr->SIZE_IOC_STS) & VUSB_EP_TR_PACKET_SIZE) >> 16);
            }

            dTD_ptr = temp_dTD_ptr;

            /* Get the address of the next dTD */
            temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(usb_dev_ptr,
                                (uint_32)(USB_32BIT_LE(temp_dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK) );
         }
         xd_ptr->WSOFAR = xd_ptr->WTOTALLENGTH - remaining_bytes;
      }
      else
      {
         /*look at actual length from QH*/
         xd_ptr->WSOFAR = xd_ptr->WTOTALLENGTH -
            ((USB_32BIT_LE(ep_queue_head_ptr->SIZE_IOC_INT_STS) & VUSB_EP_TR_PACKET_SIZE) >> 16);
      }
   }
   else
   {
      xd_ptr = NULL;
   } /* Endif */

   return (xd_ptr);

} /* EndBody */

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_deinit_endpoint
*  Returned Value : None
*  Comments       :
*        Disables the specified endpoint and the endpoint queue head
*
*END*-----------------------------------------------------------------*/
uint_8 _usb_dci_vusb20_deinit_endpoint
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle,

      /* [IN] the Endpoint number */
      uint_8                     ep_num,

      /* [IN] direction */
      uint_8                     direction
)
{ /* Body */
    USB_DEV_STATE_STRUCT_PTR        usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR           dev_ptr;
    VUSB20_EP_QUEUE_HEAD_STRUCT*    ep_queue_head_ptr;
    uint_32                         bit_pos;
    uint_8                          status = USB_OK;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ANY, "deinit_endpoint ep=%d %s\n",
                    (unsigned)ep_num, direction ? "SEND" : "RECV");

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

    /* Get the endpoint queue head address */
    ep_queue_head_ptr = (VUSB20_EP_QUEUE_HEAD_STRUCT_PTR)usb_dev_ptr->EP_QUEUE_HEAD_PTR +
                                                                    (2*ep_num + direction);

    bit_pos = (1 << (16 * direction + ep_num));

    /* Check if the Endpoint is Primed */
    if( ((USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME) & bit_pos)) ||
        ((USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS) & bit_pos)) )
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"ep=%d %s: Deinit ERROR: ENDPTPRIME=0x%x, ENDPTSTATUS=0x%x, bit_pos=0x%x\n",
                (unsigned)ep_num, direction ? "SEND" : "RECV",
                (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTPRIME),
                (unsigned)USB_32BIT_LE(dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTSTATUS),
                (unsigned)bit_pos);
        status = USBERR_EP_DEINIT_FAILED;
    }

    /* Reset the max packet length and the interrupt on Setup */
    ep_queue_head_ptr->MAX_PKT_LENGTH = 0;

    /* Disable the endpoint for Rx or Tx and reset the endpoint type */
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.ENDPTCTRLX[ep_num] &=
         USB_32BIT_LE( ~((direction ? EHCI_EPCTRL_TX_ENABLE : EHCI_EPCTRL_RX_ENABLE) |
                        (direction ? EHCI_EPCTRL_TX_TYPE : EHCI_EPCTRL_RX_TYPE)));

   return status;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_shutdown
*  Returned Value : None
*  Comments       :
*        Shuts down the VUSB_HS Device
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_shutdown
(
      /* [IN] the USB_dev_initialize state structure */
      _usb_device_handle         handle
)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   VUSB20_REG_STRUCT_PTR                        dev_ptr;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
   dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;

   /* Disable interrupts */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_INTR &=
      ~(USB_32BIT_LE(EHCI_INTR_INT_EN | EHCI_INTR_ERR_INT_EN |
      EHCI_INTR_PORT_CHANGE_DETECT_EN | EHCI_INTR_RESET_EN));

   USB_uncached_memfree(usb_dev_ptr->DRIVER_MEMORY_VIRT,
                        usb_dev_ptr->DRIVER_MEMORY_SIZE,
                        usb_dev_ptr->DRIVER_MEMORY_PHYS);

   /* Reset the Run the bit in the command register to stop VUSB */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD &= ~USB_32BIT_LE(EHCI_CMD_RUN_STOP);

   /* Reset the controller to get default values */
   dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.USB_CMD = USB_32BIT_LE(EHCI_CMD_CTRL_RESET);

} /* EndBody */

/* EOF */

//----------------------------------------------------------------------------------------
//  _usb_dci_vusb20_set_xcvr_interface()
//
//
//----------------------------------------------------------------------------------------
void _usb_dci_vusb20_set_xcvr_interface( _usb_device_handle handle, UINT32 setting, UINT32 port )
{
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR            dev_ptr;
    UINT32                           temp;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr     = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
    temp        = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port]*/;

    temp &= ~EHCI_PORTSCX_PAR_XCVR_SELECT;
    temp |= setting;
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port]*/ = temp;
}

//----------------------------------------------------------------------------------------
//  _usb_dci_vusb20_set_xcvr_width()
//
//
//----------------------------------------------------------------------------------------
void _usb_dci_vusb20_set_xcvr_width( _usb_device_handle handle, UINT32 setting, UINT32 port )
{
    USB_DEV_STATE_STRUCT_PTR         usb_dev_ptr;
    VUSB20_REG_STRUCT_PTR            dev_ptr;
    UINT32                           temp;

    usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)handle;
    dev_ptr = (VUSB20_REG_STRUCT_PTR)usb_dev_ptr->DEV_PTR;
    temp = dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port]*/;

    temp &= ~EHCI_PORTSCX_PTW_SELECT_MASK;
    temp |= setting;
    dev_ptr->REGISTERS.OPERATIONAL_DEVICE_REGISTERS.PORTSCX/*[port]*/ = temp;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_process_tr_complete
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_trace_dtd_information(UINT8 endpoint, UINT8 direction)
{ /* Body */
   USB_DEV_STATE_STRUCT_PTR                     usb_dev_ptr;
   volatile VUSB20_EP_TR_STRUCT_PTR             dTD_ptr;
   XD_STRUCT_PTR                                xd_ptr;
   VUSB20_EP_TR_STRUCT_PTR                      temp_dTD_ptr;
   uint_32                                      temp;

   usb_dev_ptr = (USB_DEV_STATE_STRUCT_PTR)_usbDeviceHandle;
   temp = (2*endpoint + direction);

   ASSERT(usb_dev_ptr != NULL);

    /* Get the first dTD */
    dTD_ptr = usb_dev_ptr->EP_DTD_HEADS[temp];

    /* Process all the dTDs for respective transfers */
    while (dTD_ptr)
    {
        if(direction == 0)
        {
            ErrorLogPrintf("Rx Ep[%x]: SIZE_IOC_STS %x, NEXT_TR_ELEM_PTR %x",
                            endpoint,
                            dTD_ptr->SIZE_IOC_STS,
                            dTD_ptr->NEXT_TR_ELEM_PTR
                            );
        }
        else
        {
            ErrorLogPrintf("Tx Ep[%x]: SIZE_IOC_STS %x, NEXT_TR_ELEM_PTR %x",
                            endpoint,
                            dTD_ptr->SIZE_IOC_STS,
                            dTD_ptr->NEXT_TR_ELEM_PTR
                            );
        }

        /* Get the correct internal transfer descriptor */
        xd_ptr = (XD_STRUCT_PTR)dTD_ptr->SCRATCH_PTR->XD_FOR_THIS_DTD;
        if (xd_ptr)
        {
            UINT8 *buf = (UINT8 *)xd_ptr->WSTARTADDRESS;

            ErrorLogPrintf("XD: WSTARTADDRESS %x, BSTATUS %x",
                            xd_ptr->WSTARTADDRESS,
                            xd_ptr->BSTATUS
                            );

            if(buf != NULL)
            {
                ErrorLogPrintf("%x %x %x %x  %x %x %x %x  %x %x %x %x  %x %x %x %x",
                            buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
                            buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]
                            );
            }
        } /* Endif */

        /* Get the address of the next dTD */
        temp_dTD_ptr = (VUSB20_EP_TR_STRUCT_PTR)USB_PHYS_TO_VIRT(usb_dev_ptr,
                    (uint_32)(USB_32BIT_LE(dTD_ptr->NEXT_TR_ELEM_PTR) & VUSBHS_TD_ADDR_MASK) );


        dTD_ptr = temp_dTD_ptr;

    } /* Endwhile */

}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_free_ep_transfer
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_free_ep_transfer(UINT8 ep_num, UINT8 direction, void (*free)(void*))
{
    /* MV_USB_SEND = 1, MV_USB_RECV = 0*/
    if(_usbDeviceHandle != NULL)
    {
        _usb_device_free_transfer(_usbDeviceHandle, ep_num, direction, free);
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_reset_data_toggle
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/
void _usb_dci_vusb20_reset_ep_data_toggle(UINT8 ep_num, UINT8 direction)
{
    /* MV_USB_SEND = 1, MV_USB_RECV = 0*/
    if(_usbDeviceHandle != NULL)
    {
        _usb_device_reset_data_toggle(_usbDeviceHandle, ep_num, direction);
    }
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_dci_vusb20_get_dtd_information
*  Returned Value : None
*  Comments       :
*        Services transaction complete interrupt
*
*END*-----------------------------------------------------------------*/

void _usb_dci_vusb20_get_dtd_information(void)
{
    ErrorLogPrintf("Ep0: TX %d, RX %d",
                    _usb_device_get_dtd_cnt(0x00, MV_USB_RECV, NULL),
                    _usb_device_get_dtd_cnt(0x00, MV_USB_SEND, NULL)
                    );

    ErrorLogPrintf("AT: TX %d, RX %d, CTRL %d",
                    _usb_device_get_dtd_cnt(0x03, MV_USB_SEND, NULL),
                    _usb_device_get_dtd_cnt(0x02, MV_USB_RECV, NULL),
                    _usb_device_get_dtd_cnt(0x01, MV_USB_SEND, NULL)
                    );

    ErrorLogPrintf("DIAG: TX %d, RX %d, CTRL %d",
                    _usb_device_get_dtd_cnt(0x06, MV_USB_SEND, NULL),
                    _usb_device_get_dtd_cnt(0x05, MV_USB_RECV, NULL),
                    _usb_device_get_dtd_cnt(0x04, MV_USB_SEND, NULL)
                    );

    ErrorLogPrintf("RNDIS: TX %d, RX %d, CTRL %d",
                    _usb_device_get_dtd_cnt(0x0E, MV_USB_SEND, NULL),
                    _usb_device_get_dtd_cnt(0x0D, MV_USB_RECV, NULL),
                    _usb_device_get_dtd_cnt(0x0C, MV_USB_SEND, NULL)
                    );

    ErrorLogPrintf("MassStorage: TX %d, RX %d",
                    _usb_device_get_dtd_cnt(0x0B, MV_USB_SEND, NULL),
                    _usb_device_get_dtd_cnt(0x0A, MV_USB_RECV, NULL)
                    );

#if 0
    _usb_device_trace_dtd_information(0x00, MV_USB_RECV);
    _usb_device_trace_dtd_information(0x00, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x01, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x02, MV_USB_RECV);
    _usb_device_trace_dtd_information(0x03, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x04, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x05, MV_USB_RECV);
    _usb_device_trace_dtd_information(0x06, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x0C, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x0D, MV_USB_RECV);
    _usb_device_trace_dtd_information(0x0E, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x0B, MV_USB_SEND);
    _usb_device_trace_dtd_information(0x0A, MV_USB_RECV);

    _usb_dump_regs();
#endif
}

