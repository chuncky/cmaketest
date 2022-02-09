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
***  This file contains USB device chapter9 specific functions general for
*** all devices
***
**************************************************************************
**END*********************************************************/

#include "mvUsbDevApi.h"
#include "mvUsbDevPrv.h"
#include "mvUsbCh9.h"
#include "mvUsbDebug.h"

#include "usb_device_types.h"
#include "usb_def.h"
#include "usb_device_def.h"
#include "udc_types.h"
#include "mvUsbStorage.h"
#include "mvUsbNet.h"
#include "mvUsbModem.h"

#ifdef MV_USB2_MASS_STORAGE
//#include "diag_nvm.h"
#endif

//#include "diag.h"
#include "usbTrace.h"
#include "usb_macro.h"
#include "platform.h"
#include "rndis.h"
#include "usb_init.h"
#include "mvUsbNet.h"
#include "mvUsbMemory.h"

//#include "ECM.h"
//#ifdef MV_USB_MBIM
#include "MBIM.h"
//#endif

/* NCM Set NTB input staus */
char mvUsbSetNTBInputStatus = 0;
#ifdef USB_REMOTEWAKEUP
BOOL SetRemoteWakeupFeature = FALSE;

BOOL EnableRemoteWakeup = FALSE;
#endif
/* Usb net response Queue head. */
extern mvUsbNetQ UsbNetRspQHdr;

/* Usb net request Queue head. */
extern mvUsbNetQ UsbNetReqQHdr;

extern UINT16 PacketSizeOffsetNum;
extern UINT16 PacketSizeOffset[20];
extern volatile UINT32 usb_shutdown;

uint_8 alt_setings = 0;

uint_8 usb_current_config = 0;
BOOL UsbGetEncapsulatedRsp = FALSE;

static volatile boolean  ENTER_TEST_MODE = FALSE;
static volatile uint_16  test_mode_index = 0;


extern __align(8) UINT8 ep0_lang_id_string_buf[];

extern void USB2ProcessGetConfigDescRequest(UINT16 value);
#ifdef MV_USB2_MASS_STORAGE
extern void USB2ReEnumerate(PlatformUsbDescType Desc);
extern BOOL USB2IsAutoInstallEnable(void);
extern BOOL USB2IsCdromOnlyDescriptor(void);
#endif
#ifdef USB_REMOTEWAKEUP
extern void mvUsbDataSignal(void);
#endif

extern UINT8 IsMbimEnabled(void);

/* Will be sent */
uint_16 mv_usb_status;
//#ifdef MV_USB_MBIM
volatile uint_32 pm_usb_busy = 0;
volatile uint_32 usb_enumerated = 0;
volatile UINT32 usb_enumerating = 0;
volatile UINT32 SDNeedtoRemount = 0;

/* NCM/MBIM NTB Parameters */
const mbim_ntb_parameters ntb_parameters = {
	sizeof(mbim_ntb_parameters),    //wLength
	USB_CDC_FORMATS_SUPPORTED,      //bmNtbFormatsSupported
	USB_CDC_NCM_NTB_IN_MAX_SIZE,    //dwNtbInMaxSize
	1,                              //wNdpInDivisor
	0,                              //wNdpInPayloadRemainder
	4,                              //wNdpInAlignment
	0,                              //wPadding1
	0x800,   						//dwNtbOutMaxSize
	4,                              //wNdpOutDivisor
	0,                              //wNdpOutPayloadRemainder
	4,                              //wNdpOutAlignment
	1                               //wNtbOutMaxDatagrams, change to 1 to use non-copy in ul
};

/* NCM/MBIM parser option */
const ndp_parser_opts ndp16_opts = {
	USB_CDC_NCM_NTH16_SIGN,         //nth_sign
	USB_CDC_NCM_NDP16_NOCRC_SIGN,   //ndp_sign
	sizeof(mbim_ncm_nth16),         //nth_size
	sizeof(mbim_ncm_ndp16),         //ndp_size
	4,			                    //ndplen_align
	1,                              //dgram_item_len
	1,	                            //block_length
	1,                              //fp_index
	0,                              //reserved1
	0,                              //reserved2
	1                               //next_fp_index
};

/* NCM/MBIM parser option */
const ndp_parser_opts ndp32_opts = {
	USB_CDC_NCM_NTH32_SIGN,         //nth_sign
	USB_CDC_NCM_NDP32_NOCRC_SIGN,   //ndp_sign
	sizeof(mbim_ncm_nth32),         //nth_size
	sizeof(mbim_ncm_ndp32),         //ndp_size
	8,                              //ndplen_align
	2,                              //dgram_item_len
	2,                              //block_length
	2,                              //fp_index
	1,                              //reserved1
	2,                              //reserved2
	2                               //next_fp_index
};

/* MBIM params */
static mbim_parameters mbim_dev_params = {0};

/* MBIM params */
#define MBIM_DEV_PARAMS_PTR() &mbim_dev_params

//#endif

void    mvUsbCh9GetStatus(_usb_device_handle handle, boolean setup,
                         SETUP_STRUCT* ctrl_req)
{ /* Body */
    uint_8   endpoint;


    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_SETUP, "%s: setup=%d\n", __FUNCTION__, (int)setup);

    if(!setup)
        return;

    switch (ctrl_req->REQUESTTYPE)
    {
       case (REQ_DIR_IN | REQ_RECIP_DEVICE):
          /* Device request */
          _usb_device_get_status(handle, MV_USB_STATUS_DEVICE, &mv_usb_status);
          break;

       case (REQ_DIR_IN | REQ_RECIP_INTERFACE):
          /* Interface request */
          _usb_device_get_status(handle, MV_USB_STATUS_INTERFACE, &mv_usb_status);
          break;

       case (REQ_DIR_IN | REQ_RECIP_ENDPOINT):
          /* Endpoint request */
          endpoint = ctrl_req->INDEX & MV_USB_STATUS_ENDPOINT_NUMBER_MASK;

          _usb_device_get_status(handle, MV_USB_STATUS_ENDPOINT | endpoint, &mv_usb_status);
          break;

       default:
          /* Unknown request */
           MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Unknown request type 0x%x\n", __FUNCTION__, ctrl_req->REQUESTTYPE);
          _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
          return;
    } /* Endswitch */

    /* Send the requested data */
    mv_usb_status = mv_usb_status;

    mvUsbCh9SendEp0Data(handle, (uint_8_ptr)&mv_usb_status, 2 /*size*/);

    /* status phase */
    mvUsbCh9RecvEp0Data(handle, NULL, 0);

    return;
} /* Endbody */

void    mvUsbCh9ClearFeature(_usb_device_handle handle, boolean setup,
                            SETUP_STRUCT* setup_ptr)
{ /* Body */
    uint_8   endpoint;
    uint_16  usb_status;

    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_SETUP, "%s: setup=%d\n", __FUNCTION__, (int)setup);

    _usb_device_get_status(handle, MV_USB_STATUS_DEVICE_STATE, &usb_status);
    if ((usb_status != MV_USB_STATE_CONFIG) && (usb_status != MV_USB_STATE_ADDRESS))
    {
        MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Wrong state %d\n", __FUNCTION__, usb_status);
        _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
        return;
    } /* Endif */

    if(!setup)
        return;

    switch (setup_ptr->REQUESTTYPE)
    {
        case (REQ_DIR_OUT | REQ_RECIP_DEVICE):
            /* DEVICE */
            switch(setup_ptr->VALUE)
            {
                case DEVICE_REMOTE_WAKEUP:
                    /* clear remote wakeup */
                    _usb_device_get_status(handle, MV_USB_STATUS_DEVICE, &usb_status);
                    usb_status &= ~MV_USB_REMOTE_WAKEUP;
                    _usb_device_set_status(handle, MV_USB_STATUS_DEVICE, usb_status);
#ifdef USB_REMOTEWAKEUP
					uart_printf("clear remotewakeup\n");
                    EnableRemoteWakeup = FALSE;
                    SetRemoteWakeupFeature  = FALSE;
                    pc_netif_status(1);
					mvUsbDataSignal();
					pm_usb_busy = 1;
#endif
                    break;

                case DEVICE_TEST_MODE:
                    /* Exit Test Mode */
                    _usb_device_set_status(handle, MV_USB_STATUS_TEST_MODE, 0);
                    break;

                default:
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Unknown Device feature %d\n",
                                __FUNCTION__, setup_ptr->VALUE);
                    _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
                    return;
            } /* Endif */
            break;

        case (REQ_DIR_OUT | REQ_RECIP_ENDPOINT):
            /* ENDPOINT */
            if (setup_ptr->VALUE != ENDPOINT_HALT)
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Wrong Endpoint feature %d\n",
                            __FUNCTION__, setup_ptr->VALUE);
                _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
                return;
            } /* Endif */

            endpoint = setup_ptr->INDEX & MV_USB_STATUS_ENDPOINT_NUMBER_MASK;
            /* unstall */
            _usb_device_set_status(handle, MV_USB_STATUS_ENDPOINT | endpoint, 0);
            break;

        default:
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Unknown REQUEST_TYPE %d\n",
                                __FUNCTION__, setup_ptr->REQUESTTYPE);

            _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
            return;
    } /* Endswitch */

    /* status phase */
    _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
    mvUsbCh9SendEp0Data(handle, 0, 0);
}

void    mvUsbCh9SetFeature(_usb_device_handle handle, boolean setup,
                          SETUP_STRUCT* setup_ptr)
{
   uint_16                  usb_status;
   uint_8                   endpoint;
   USB_DEV_STATE_STRUCT*    usb_dev_ptr = (USB_DEV_STATE_STRUCT*)handle;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_SETUP, "%s: setup=%d\n", __FUNCTION__, (int)setup);

   if (setup)
   {
      switch (setup_ptr->REQUESTTYPE)
      {
         case (REQ_DIR_OUT | REQ_RECIP_DEVICE):
            /* DEVICE */
            switch (setup_ptr->VALUE)
            {
               case DEVICE_REMOTE_WAKEUP:
                  /* set remote wakeup */
                  _usb_device_get_status(handle, MV_USB_STATUS_DEVICE, &usb_status);
                  usb_status |= MV_USB_REMOTE_WAKEUP;
                  _usb_device_set_status(handle, MV_USB_STATUS_DEVICE, usb_status);
#ifdef USB_REMOTEWAKEUP
				  uart_printf("set remotewakeup\n");
	              SetRemoteWakeupFeature = TRUE;
#endif
                  break;

               case DEVICE_TEST_MODE:
                  /* Test Mode */
                  if( (setup_ptr->INDEX & 0x00FF) || (usb_dev_ptr->SPEED != MV_USB_SPEED_HIGH) )
                  {
                     MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Wrong Test mode parameters: mode=%d, speed=%d\n",
                                __FUNCTION__, (setup_ptr->INDEX & 0x00FF), usb_dev_ptr->SPEED);
                     _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
                     return;
                  } /* Endif */

                  _usb_device_get_status(handle, MV_USB_STATUS_DEVICE_STATE, &usb_status);
                  if( (usb_status == MV_USB_STATE_CONFIG)  ||
                      (usb_status == MV_USB_STATE_ADDRESS) ||
                      (usb_status == MV_USB_STATE_DEFAULT))
                  {
                      /* wait with Set Test mode */
                      ENTER_TEST_MODE = TRUE;
                      test_mode_index = (setup_ptr->INDEX & 0xFF00);
                      MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO,"%s: Prepare for Test mode 0x%x\n",
                                                __FUNCTION__, test_mode_index);
                  }
                  else
                  {
                     MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Wrong USB state for Test mode: state=%d\n",
                                __FUNCTION__, usb_status);
                     _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
                     return;
                  } /* Endif */
                  break;

               default:
                    MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Unknown Device feature %d\n",
                                __FUNCTION__, setup_ptr->VALUE);
                  _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
                  return;
            } /* Endswitch */
            break;

         case (REQ_DIR_OUT | REQ_RECIP_ENDPOINT):
            /* ENDPOINT */
            if (setup_ptr->VALUE != ENDPOINT_HALT)
            {
                MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Unknown Endpoint feature %d\n",
                            __FUNCTION__, setup_ptr->VALUE);
                _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
                return;
            } /* Endif */

            endpoint = setup_ptr->INDEX & MV_USB_STATUS_ENDPOINT_NUMBER_MASK;
            /* set stall */
            _usb_device_set_status(handle, MV_USB_STATUS_ENDPOINT | endpoint, 1);
            break;

         default:
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_WARNING,"%s: Unknown REQUEST_TYPE %d\n",
                       __FUNCTION__, setup_ptr->REQUESTTYPE);

            _usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
            return;
      } /* Endswitch */

      /* status phase */
      _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
      mvUsbCh9SendEp0Data(handle, 0, 0);
   }
   else
   {
      if (ENTER_TEST_MODE)
      {
         /* Enter Test Mode */
          MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_INFO,"%s: Activate Test mode 0x%x\n", __FUNCTION__, test_mode_index);
         _usb_device_set_status(handle, MV_USB_STATUS_TEST_MODE, test_mode_index);
      } /* Endif */
   } /* Endif */
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : ch9SetAddress
* Returned Value : None
* Comments       :
*     Chapter 9 SetAddress command
*     We setup a TX packet of 0 length ready for the IN token
*     Once we get the TOK_DNE interrupt for the IN token, then
*     we change the ADDR register and go to the ADDRESS state.
*
*END*--------------------------------------------------------------------*/
void    mvUsbCh9SetAddress(_usb_device_handle handle,
                        boolean setup, SETUP_STRUCT* setup_ptr)
{ // Body
   static uint_8  new_address;

   MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "usbDisk %s: setup=%d, address=%d\n",
                                    __FUNCTION__, (int)setup, setup_ptr->VALUE);

  MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "\nmvUsbCh9SetAddress-------> ENTRY!!!\n");

   if (setup)
   {
      USB_TRACE("Set address");

      new_address = setup_ptr->VALUE;
      /*******************************************************
       * if hardware assitance is enabled for set_address (see
       * hardware rev for details) we need to do the set_address
       * before queuing the status phase.
       *******************************************************/
#ifdef SET_ADDRESS_HARDWARE_ASSISTANCE
       _usb_device_set_status(handle, USB_STATUS_ADDRESS, new_address);
       MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "\nmvUsbCh9SetAddress-------> 1\n");
#endif
       MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "\nmvUsbCh9SetAddress-------> 2\n");

      /* ack */
      _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
      mvUsbCh9SendEp0Data(handle, 0, 0);
   }
   else
   {

#ifndef SET_ADDRESS_HARDWARE_ASSISTANCE
      _usb_device_set_status(handle, MV_USB_STATUS_ADDRESS, new_address);
       MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "\nmvUsbCh9SetAddress-------> 3\n");
#endif
      _usb_device_set_status(handle, MV_USB_STATUS_DEVICE_STATE, MV_USB_STATE_ADDRESS);
       MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_ADDR, "\nmvUsbCh9SetAddress-------> 4\n");
   }
}


UINT8 USBGetMaxLun=0;
void mvUsbCh9GetDescriptior(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    uint_32  pktSize, stringIndex;
    uint_8  *payload = "UNHANDLED";
    pktSize = sizeof(*payload);
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbGetDescInfo *pUsbDesInfo = mvUsbGetDescriptorInfo();

    if (setup)
    {
        pUsbDesInfo->DesCnt++;

        USB_TRACE("DesCnt %u, Value 0x%x", pUsbDesInfo->DesCnt, setup_ptr->VALUE);

        /* Load the appropriate string depending on the descriptor requested.*/
        switch (setup_ptr->VALUE >> 8)
        {
            case DESC_TYPE_DEVICE:
            {
                ASSERT(_usbDeviceDescriptors.pDeviceDescriptor != NULL);
                pktSize = USB_DEVICE_MAX_LENGTH(_usbDeviceDescriptors.deviceDescriptorLength, setup_ptr->LENGTH);
                payload = _usbDeviceDescriptors.pDeviceDescriptor;
                break;
            }

            case DESC_TYPE_CONFIG:
            {
                uint_16 i = 0, offset = 0;

                if(pUsbDesInfo->DesCnt < 32)
                {
                    pUsbDesInfo->DesType[pUsbDesInfo->DesCnt] = DESC_TYPE_CONFIG;
                }

                if((pUsbDrvConfig->os_detect == USB_OS_DETECT_ENABLE)&&
                   (pUsbDesInfo->DesType[3] == DESC_TYPE_CONFIG))
                {
                    /* This is WINXP OS.              */
                    if( mvUsbGetNetType() !=  USB_NET_RNDIS )
                    {
                         /* ECM->RNDIS.                     */
                        mvUsbNetSwitchTo(USB_NET_RNDIS);
                    }
                }

                USB2ProcessGetConfigDescRequest((setup_ptr->VALUE) & 0xFF);

                ASSERT(_usbDeviceDescriptors.pConfigDescriptor != NULL);

                payload = _usbDeviceDescriptors.pConfigDescriptor;

                if ( USB2_IS_FULL_SPEED())
                {
                    // Full Speed
                    for(i = 0; i < PacketSizeOffsetNum; i++)
                    {
                        offset = PacketSizeOffset[i];

                        payload[offset]   =  0x40;
                        payload[offset+1] =  0x00;
                    }
                }
                else
                {
                    // Hi Speed
                    for(i = 0; i < PacketSizeOffsetNum; i++)
                    {
                        offset = PacketSizeOffset[i];

                        payload[offset]   =  0x00;
                        payload[offset+1] =  0x02;
                    }
                }

                pktSize = USB_DEVICE_MAX_LENGTH(_usbDeviceDescriptors.configDescriptorLength, setup_ptr->LENGTH);
                payload = _usbDeviceDescriptors.pConfigDescriptor;
                break;
            }

            case DESC_TYPE_STRING:
            {
                UINT16  langID = setup_ptr->INDEX;
                stringIndex = ((setup_ptr->VALUE) & 0xFF);

                if(pUsbDesInfo->DesCnt < 32)
                {
                    pUsbDesInfo->DesType[pUsbDesInfo->DesCnt] = DESC_TYPE_STRING;
                }

                if((pUsbDrvConfig->os_detect == USB_OS_DETECT_ENABLE)&&
                   (pUsbDesInfo->DesType[3] == DESC_TYPE_STRING)&&
                   (pUsbDesInfo->DesType[4] == DESC_TYPE_STRING))
                {
                    if( mvUsbGetNetType() !=  USB_NET_ECM )
                    {
                        /* RNDIS->ECM.                      */
                        mvUsbNetSwitchTo(USB_NET_ECM);
                    }
                }

#if defined(MV_USB2_MASS_STORAGE)
                if((stringIndex == 0x05)&&
                  USB2IsAutoInstallEnable()&&
                  USB2IsCdromOnlyDescriptor())
                {
                    USB2ReEnumerate( USB_MAX_DESCRIPTOR_TYPE );
                }
#endif

                ASSERT(_usbDeviceDescriptors.stringDescriptorTotal != 0);

                /* is this Language list request? */
                if(langID == USB_LANG_LIST_REQ)
                {
                        payload  = ep0_lang_id_string_buf;
                }
                else
                {
                    if(langID == USB_LANG_ENGLISH_ID)
                    {
                        if(stringIndex >= USB_DEVICE_TOTAL_STRINGS)
                        {
                            _usb_device_stall_endpoint(handle, 0, 0);
                            return;
                        }

                        USB_DEVICE_ASSERT(_usbDeviceDescriptors.pStringDescriptor[stringIndex]);
                        payload = _usbDeviceDescriptors.pStringDescriptor[stringIndex];
                    }
                    else
                    {
                        //USB_DEVICE_ASSERT(FALSE); /* Language requested is wrong */
                        _usb_device_stall_endpoint(handle, 0, 0);
                    }
                }
                pktSize = (USB_DEVICE_MAX_LENGTH(payload[0], setup_ptr->LENGTH));
                break;
            }

            case DESC_TYPE_INTERFACE:
            {
                _usb_device_stall_endpoint(handle, 0, 0);
                return;
            }

            case DESC_TYPE_ENDPOINT:
            {
                _usb_device_stall_endpoint(handle, 0, 0);
                return;
            }

            case DESC_TYPE_QUALIFIER:
            {
                ASSERT(_usbDeviceDescriptors.pQualifDescriptor != NULL);
                pktSize = _usbDeviceDescriptors.qualifDescriptorLength;

                if ( USB2_IS_FULL_SPEED())
                {
                    _usbDeviceDescriptors.pQualifDescriptor[7] =  0x40;    /* bMaxPacketSize0 in HS is 64 bytes */
                }
                else
                {
                    // Hi_Speed
                    _usbDeviceDescriptors.pQualifDescriptor[7] =  0x10;    /* bMaxPacketSize0 in FS is 16 bytes */
                }
                payload = _usbDeviceDescriptors.pQualifDescriptor;
                break;
            }

            case DESC_TYPE_OTHER_SPEED:
            {
                uint_16 usb_cnfg = 0;
                uint_16 i = 0, offset = 0;
                mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

                ASSERT(_usbDeviceDescriptors.pOtherSpeedDescriptor != NULL);
                pktSize = USB_DEVICE_MAX_LENGTH(_usbDeviceDescriptors.otherSpeedDescriptorLength, setup_ptr->LENGTH);

                payload = _usbDeviceDescriptors.pOtherSpeedDescriptor;

                _usb_device_get_status(handle, MV_USB_STATUS_CURRENT_CONFIG, (uint_16*)(&usb_cnfg));
                payload[5]  = usb_cnfg & 0xFF ; // bConfigurationValue


                if ( USB2_IS_FULL_SPEED())
                {
                    // Hi Speed
                    for(i = 0; i < PacketSizeOffsetNum; i++)
                    {
                        offset = PacketSizeOffset[i];

                        payload[offset]   =  0x00;
                        payload[offset+1] =  0x02;
                    }
                }
                else
                {
                    for(i = 0; i < PacketSizeOffsetNum; i++)
                    {
                        offset = PacketSizeOffset[i];

                        payload[offset]   =  0x40;
                        payload[offset+1] =  0x00;
                    }
                }
                break;
            }

            default:
            {
                _usb_device_stall_endpoint(handle, 0, 0);
                return;
            }

        } // Endswitch

        // send required data
        _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
        mvUsbCh9SendEp0Data (handle, payload, MIN(setup_ptr->LENGTH, pktSize));

        // status phase
        _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
        mvUsbCh9RecvEp0Data(handle, 0, 0);
    } // Endif
    return;

}


void mvUsbCh9SetDescriptior(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
}

void mvUsbCh9GetConfig(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
/*
 * If wValue, wIndex, or wLength are not as specified in "9.4.2 Get Configuration" (USB2.0 spec)
 * then the device behavior is not specified.
 *
 * Default state: Device behavior when this request is received while the device is in the Default state is not specified.
 * Address state: The value zero must be returned.
 * Configured state: The non-zero bConfigurationValue of the current configuration must be returned.
 *
 * */
    uint_16 usb_currCnfg = 0;
	uint_16 usb_status = 0;

	_usb_device_get_status(handle, MV_USB_STATUS_DEVICE_STATE, &usb_status);

  	if ((setup_ptr->VALUE != 0) || (setup_ptr->INDEX != 0) || (setup_ptr->LENGTH != 1) || (usb_status == MV_USB_STATE_DEFAULT))
   		return;

	if (usb_status == MV_USB_STATE_ADDRESS)
	{
		usb_currCnfg = 0;
	}
	else  // handle->STATE == MV_USB_STATE_CONFIG
	{
		_usb_device_get_status(handle, MV_USB_STATUS_CURRENT_CONFIG, (uint_16_ptr)(&usb_currCnfg));
	}

	usb_current_config = usb_currCnfg & 0xFF;

    /* Send the requested data */
    mvUsbCh9SendEp0Data(handle, &usb_current_config, 1 /*size*/);

     /* status phase */
    mvUsbCh9RecvEp0Data(handle, NULL, 0);
}


unsigned long USB_IS_CONNECTED(void)
{
	return pm_usb_busy;
}

unsigned long USB_IS_ENUMERATED(void)
{
	return usb_enumerated;
}

void SetUsbEnumFlag(uint_8 val)
{
	usb_enumerated = val;
}

void mvUsbCh9SetConfig(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    uint_16 usb_state;

    if (setup)
    {

        if ((setup_ptr->VALUE & 0x00FF) > 10)
        { /* generate stall */
            _usb_device_stall_endpoint(handle, 0, 0);
            return;
        } /* Endif */

      /* 0 indicates return to unconfigured state */
        if ((setup_ptr->VALUE & 0x00FF) == 0)
        {
            _usb_device_get_status(handle, MV_USB_STATUS_DEVICE_STATE, &usb_state);
            if ((usb_state == MV_USB_STATE_CONFIG) || (usb_state == MV_USB_STATE_ADDRESS))
            {
                /* clear the currently selected config value */
                _usb_device_set_status(handle, MV_USB_STATUS_CURRENT_CONFIG, 0);
                _usb_device_set_status(handle, MV_USB_STATUS_DEVICE_STATE, MV_USB_STATE_ADDRESS);
                /* status phase */
                _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
                mvUsbCh9SendEp0Data(handle, 0, 0);
            }
            else
            {
                _usb_device_stall_endpoint(handle, 0, 0);
            } /* Endif */
            return;
        } /* Endif */

          /*
          ** If the configuration value (setup_ptr->VALUE & 0x00FF) differs
          ** from the current configuration value, then endpoints must be
          ** reconfigured to match the new device configuration
          */
        _usb_device_get_status(handle, MV_USB_STATUS_CURRENT_CONFIG, &usb_state);
        if (usb_state != (setup_ptr->VALUE & 0x00FF))
        {
             /* Reconfigure endpoints here
             switch (setup_ptr->VALUE & 0x00FF)
            {

                default:
                   break;
            }  Endswitch */
            _usb_device_set_status(handle, MV_USB_STATUS_CURRENT_CONFIG, setup_ptr->VALUE & 0x00FF);
            _usb_device_set_status(handle, MV_USB_STATUS_DEVICE_STATE, MV_USB_STATE_CONFIG);
            /* status phase */
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
            mvUsbCh9SendEp0Data(handle, 0, 0);
            USB2DevicePerformAfterEnum();

			pm_usb_busy = 1;

            SetUsbEnumFlag(1);
            usb_enumerating = 0;
            SDNeedtoRemount = 0;

//			usb_shutdown = 1;
            return;
        } /* Endif */
        _usb_device_set_status(handle, MV_USB_STATUS_DEVICE_STATE, MV_USB_STATE_CONFIG);
        /* status phase */
        mvUsbCh9SendEp0Data(handle, 0, 0);
    } /* Endif */
    return;
}

void mvUsbCh9GetInterface(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
/* If wValue or wLength are not as specified above, then the device behavior is not specified.
 * If the interface specified does not exist, then the device responds with a Request Error.
 *
 * Default state: Device behavior when this request is received while the device is in the Default state is not specified.
 * Address state: A Request Error response is given by the device.
 * Configured state: This is a valid request when the device is in the Configured state.
 * */
	uint_16 usb_status = 0;
	_usb_device_get_status(handle, MV_USB_STATUS_DEVICE_STATE, &usb_status);

  	if ((setup_ptr->VALUE != 0) ||
		(setup_ptr->LENGTH != 1) ||
		/* (setup_ptr->INDEX <= MAX_NUM_INTERFACE_USED) || see TO DO comment bellow */
		(usb_status == MV_USB_STATE_DEFAULT))
   		return;

	if (usb_status == MV_USB_STATE_ADDRESS)   /* in such state need to response with Request Error response */
	{
		_usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
	}

	/* ysc TO DO - change here to reteive Alternate setting of required interface (setup_ptr->INDEX) dynamicly from used descriptores.*/

	/* Send the requested data */
    mvUsbCh9SendEp0Data(handle, &alt_setings, 1 /*size*/);

     /* status phase */
    mvUsbCh9RecvEp0Data(handle, NULL, 0);
}

void mvUsbCh9SetInterface(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
/*
 * If the interface or the alternate setting does not exist, then the device responds with a Request Error.
 * If wLength is non-zero, then the behavior of the device is not specified.
 *
 * Default state: Device behavior when this request is received while the device is in the Default state is not specified.
 * Address state: The device must respond with a Request Error.
 * Configured state: This is a valid request when the device is in the Configured state.
 **/
	uint_16 usb_status = 0;

	_usb_device_get_status(handle, MV_USB_STATUS_DEVICE_STATE, &usb_status);

	if ((setup_ptr->LENGTH != 0) ||
		/* (!isExistInterface(INTERFACE, setup_ptr->INDEX) || !isExistAltSettings(setup_ptr->INDEX, setup_ptr->VALUE) TO DO - ysc implement this to check validity dynamicly */
		(usb_status == MV_USB_STATE_DEFAULT))
   		return;

	if (usb_status == MV_USB_STATE_ADDRESS)   /* in such state need to response with Request Error response */
	{
		_usb_device_stall_endpoint(handle, 0, MV_USB_RECV);
	}

	/* TO DO - ysc set the required Alt Settings (setup_ptr->VALUE) in the corresponding interface (setup_ptr->INDEX) */
     /* status phase */

    if(setup)
    {
        mvUsbCh9SendEp0Data(handle, 0, 0);
    }

}

void ch9Class(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
}
// YSS - RETURN HERE TO HANDLE PROPERLY
const UINT8 ci2NoVendorString[] = "NoVendorString";

//---------------------------------------------------------------------------------------
// mvUsbCh9ProcessVendorRequest()
//
//
//---------------------------------------------------------------------------------------
void mvUsbCh9ProcessVendorRequest( void *handle, boolean setup, SETUP_STRUCT* setup_ptr )
{
   int_32 pktSize;
   void  *pPayload;

   if( setup )
   {

	     pktSize = sizeof(ci2NoVendorString);
	     pPayload = (UINT8 *)ci2NoVendorString;


       _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
       mvUsbCh9SendEp0Data (handle, (UINT8 *)pPayload, MIN(setup_ptr->LENGTH, pktSize));

	   /* status phase */
	   mvUsbCh9RecvEp0Data(handle, 0, 0);
   }

}




/****************************************************************************
* Function: mvUsbCh9SetEthernetMulticastFilters
*
* Descripition:
*   This request sets the Ethernet device multicast filters as specified
* in the sequential list of 48 bit Ethernet multicast addresses.
*
* Parameters:
*   Inputs:
*      <_usb_evice_handle> handle:
*      <          boolean> setup:
*      <     SETUP_STRUCT> *setup_ptr:
*
* Returns:
*       N/A.
* Notes:
*     bmRequestType | bRequestCode | wValue         | wIndex    | wLength | Data                   |
*     ______________|______________|________________|___________|__________________________________|
*      00100001B    |     0x40     |   Number       | Interface |  N*6    | A list of N 48 bit     |
*                   |              |  of filters    |           |         | Multicast addresses.   |
*
******************************************************************************/
void mvUsbCh9SetEthernetMulticastFilters( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr )
{
    if( setup )
    {
       mvUsbCh9SendEp0Data(handle, 0, 0);
    }
}


/*===========================================================================

FUNCTION mvUsbCh9SetEthernetPacketFilter

DESCRIPTION
  This request is used to configure device Ethernet packet filter settings.
  The Packet Filter is the inclusive OR of the bitmap shown in Table 8.
  Though network adapters for faster buses (e.g., PCI) may offer other hardware
  filters, the medium speed networking devices (< 10Mbit/s) attached via USB
  are only required to support promiscuous and all multicast modes.  The host
  networking software driver is responsible for performing additional filtering
  as required.

  Note that for some device types, the ability to run in promiscuous mode may
  be severely restricted or prohibited.

  For example, DOCSIS cable modems are only permitted to forward certain frames
  to its attached host.

  Even if forwarding of all frames were allowed, the raw cable modem downstream
  rate available on the RF interface can be many times the maximum USB throughput.

  Bit position  Description
    D15..D5  RESERVED  (Reset to zero)
    D4  PACKET_TYPE_MULTICAST
        1:  All multicast packets enumerated in the device's multicast address
            list are forwarded up to the host. (required)
        0:  Disabled.  The ability to disable forwarding of these multicast
            packets is optional.
    D3  PACKET_TYPE_BROADCAST
        1:  All broadcast packets received by the networking device are forwarded
            up to the host. (required)
        0:  Disabled.  The ability to disable forwarding of broadcast packets is
            optional.
    D2  PACKET_TYPE_DIRECTED
        1:  Directed packets received containing a destination address equal to
            the MAC address of the networking device are forwarded up to the host.
            (required)
        0:  Disabled.  The ability to disable forwarding of directed packets is
            optional. ***
    D1  PACKET_TYPE_ALL_MULTICAST
        1:  ALL multicast frames received by the networking device are forwarded
            up to the host, not just the ones enumerated in the device's multicast
            address list (required)
        0:  Disabled.
    D0  PACKET_TYPE_PROMISCUOUS:
        1:  ALL frames received by the networking device are forwarded up to the
            host (required)
        0: Disabled.

DEPENDENCIES

  -------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |     wValue      |      wIndex        |     wLength      |  Data  |
 | 00100001B     |    0x43      |  Packet Filter  |     Interface      |        0         |  None  |
 |               |              |     Bitmap      |                    |                  |        |
 --------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbCh9SetEthernetPacketFilter(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    if( setup )
    {
        if(setup_ptr->LENGTH == 0)
        {
            mvUsbCh9SendEp0Data(handle, 0, 0);

            EcmParamInit();
        }
    }

}

//#ifdef MV_USB_MBIM
/*=================================================================================================

FUNCTION mvUsbCh9GetNTBParameters

DESCRIPTION
  This request retrieves the parameters that describe NTBs for each direction. In response to this
  request, the function shall return these elements as listed  in Table 6-3.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |     Data      |
 | 10100001B     |    0x80      |  zero  | NCM Communications |  Number of bytes |  NTB Parameter|
 |               |              |        |   Interface        |     to read      |   Structure   |
 ------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9GetNTBParameters(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    if (setup)
    {
        if(setup_ptr->LENGTH != 0 && setup_ptr->VALUE == 0)
        {
            mvUsbCh9SendEp0Data (handle, (UINT8 *)&ntb_parameters,
                                MIN(setup_ptr->LENGTH, sizeof(ntb_parameters)));

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, 0, 0);
        }
	}
}

/*=================================================================================================

FUNCTION mvUsbCh9GetNTBInputSize

DESCRIPTION
  This request returns NTB input size currently being used by the function.
  To get the NTB input size, the host should set wLength to at least 4.  To get the full NTB input
  size structure, the host should set wLength to at least 8. If bit D5 is set in field bmNetworkCapabilities
  of the function¡¯s NCM Functional Descriptor, the function shall never return more than 8 bytes in
  response to this command. If bit D5 is reset, the function shall never return more than 4 bytes in
  response to this command. The fields in the input size structure are returned in little-endian order.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |   The NTB     |
 | 10100001B     |    0x85      |  zero  | NCM Communications |  Number of bytes |    input      |
 |               |              |        |   Interface        |     to read      |   Structure   |
 ------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9GetNTBInputSize(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    mbim_parameters *mbim_param_ptr = MBIM_DEV_PARAMS_PTR();

    if (setup)
    {
        if(setup_ptr->LENGTH >= 4 && setup_ptr->VALUE == 0)
        {
            mvUsbCh9SendEp0Data (handle, (UINT8 *)&mbim_param_ptr->fixed_in_len, 4);

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, 0, 0);
        }
	}
}

/*=================================================================================================

FUNCTION mvUsbCh9SetNTBInputSize

DESCRIPTION
  This request selects the maximum size of NTB that the device is permitted to send to the host,
  and optionally the maximum number of datagrams that the device is permitted to encode into a single
  NTB.

  If bit D5 is set in the bmNetworkCapabilities field of function's NCM Functional Descriptor, the host
  may set wLength either to 4 or to 8. If wLength is 4, the function shall assume that wNtbInMaxDatagrams
  is to be set to zero. If wLength is 8, then the function shall use the provided value as the limit.
  The function shall return an error response (a STALL PID) if wLength is set to any other value.

  If bit D5 is reset in the bmNetworkCapabilities field of the function's NCM Functional Descriptor,
  the host shall set wLength to 4.  The function shall return an error response (a STALL PID) if wLength
  is set to any other value.

  If the value passed in the data phase is not valid, or if wLength is not valid, the function shall
  return an error response (a STALL PID) and shall not change the value it uses for preparing NTBs.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |    DATA     |
 | 00100001B     |    0x86      |  zero  | NCM Communications |     4 or 8       |             |
 |               |              |        |   Interface        |                  |    Note:    |
 ------------------------------------------------------------------------------------------------

 Note: If wLength is 8, then this is the NTB Input Size Structure. If wLength is 4, then this is the
 dwNtbInMaxSize field of the NTB Input Size Structure.


                   Table 6-4. NTB Input Size Structure
 ------------------------------------------------------------------------------------------------
 |           |                  |               |                |                               |
 |  Offset   |      Field       |       Size    |       Value    |          Description          |
  ------------------------------------------------------------------------------------------------
 |           |                  |               |                |IN NTB Maximum size in bytes.  |
 |           |                  |               |                |The host shall select a size   |
 |           |                  |               |                |that is at least 2048, and no  |
 |     0     |  dwNtbInMaxSize  |       4       |      Number    |larger than the maximum size   |
 |           |                  |               |                |permitted by the function,     |
 |           |                  |               |                |according to the value given   |
 |           |                  |               |                |in the NTB Parameter Structure.|
 |           |                  |               |                |                               |
  -----------------------------------------------------------------------------------------------
 |           |                  |               |                |                               |
 |     4     |wNtbInMaxDatagrams|       2       |      Number    | Maximum number of datagrams   |
 |           |                  |               |                | within the IN NTB.            |
 |           |                  |               |                | Zero means no limit.          |
  ------------------------------------------------------------------------------------------------
 |           |                  |               |                |                               |
 |     6     |  reserved        |       2       |      Number    | Shall be transmitted as zero  |
 |           |                  |               |                | and ignored upon receipt.     |
  ------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9SetNTBInputSize(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    UINT8 rx_status = 0;
    static UINT8 *NTBInputCtrlData = NULL;
    mbim_ntb_input_size_type *NTBInputSize = NULL;
    mbim_parameters *mbim_param_ptr = MBIM_DEV_PARAMS_PTR();

    if (setup)
    {
        if((setup_ptr->LENGTH != 4) && (setup_ptr->LENGTH != 8) || (setup_ptr->VALUE != 0))
        {
            _usb_device_stall_endpoint(handle, 0, 0);
        }
        else
        {
            NTBInputCtrlData = mvUsbAllocate(0);
            USB_ALLOC_MEM_STATUS(NTBInputCtrlData);

            rx_status = mvUsbCh9RecvEp0Data(handle, NTBInputCtrlData, setup_ptr->LENGTH);     /* Ack with ZLP. */
            USB_DEVICE_ASSERT(rx_status == 0);

            mvUsbSetNTBInputStatus = 1;
        }
	}
	else
	{
        if(mvUsbSetNTBInputStatus)
        {
            rx_status = mvUsbCh9SendEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(rx_status == 0);

            NTBInputSize = (mbim_ntb_input_size_type *)NTBInputCtrlData;
            mbim_param_ptr->fixed_in_len = NTBInputSize->dwNtbInMaxSize;

            if(!NTBInputCtrlData)
            {
                mvUsbDeallocate(NTBInputCtrlData);
                NTBInputCtrlData = NULL;
            }

            /* status phase */
            mvUsbSetNTBInputStatus = 0;
        }
	}
}

/*=================================================================================================

FUNCTION mvUsbCh9GetNTBFormat

DESCRIPTION
  This request returns the NTB data format currently being used by the function.

  To get the full response, the host should set wLength to at least 2. The function shall never
  return more than 2 bytes in response to this command.

  This command must be supported by the function if it declares support for an NTB size other than
  16bit in bmNtbFormatsSupported.

  If the function does not support NTB sizes other than 16bit, then the host must not issue this
  command to the function.

DEPENDENCIES

 -----------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode | wValue |      wIndex        |     wLength      |    DATA     |
 | 10100001B     |    0x83      |  zero  | NCM Communications |  Number of bytes |             |
 |               |              |        |   Interface        |    to read       |    Note:    |
 ------------------------------------------------------------------------------------------------

 Note: The NTB format code (2 bytes, little-endian), as defined under wValue in SetNtbFormat (6.2.5).

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9GetNTBFormat(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    UINT16 format;
    mbim_parameters *mbim_param_ptr = MBIM_DEV_PARAMS_PTR();

    if (setup)
    {
        if((setup_ptr->LENGTH >= 2) && (setup_ptr->VALUE == 0))
        {
		    format = (mbim_param_ptr->parser_opts == &ndp16_opts) ? 0x0000 : 0x0001;

		     mvUsbCh9SendEp0Data (handle, (UINT8 *)&format, 2);

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, 0, 0);
		}
	}
}

/*=================================================================================================

FUNCTION mvUsbCh9SetNTBFormat

DESCRIPTION
  This request selects the format of NTB to be used for NTBs transmitted from the function to the host.
  The host must choose one of the available choices from the bmNtbFormatsSupported bitmap element from
  the GetNtbParameters command response (Table 6-3).

  The command format uses the same format, with a single choice selected. The host shall only send this
  command while the NCM Data Interface is in alternate setting 0.

  The function's NTB format setting may be changed by events beyond the scope of this command;
  see sec-tion 7.1 for details.

  If the value passed in wValue is not supported, the function shall return an error response (a STALL PID)
  and shall not change the format it is using to send and receive NTBs.

  This command must be supported by the function if it declares support for an NTB size other than 16bit
  in bmNtbFormatsSupported. If the function does not support NTB sizes other than 16bit, then the host must
  not issue this command to the function.

DEPENDENCIES

 --------------------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |           wValue                  |      wIndex        |  wLength   | DATA  |
 |               |              |      NTB Format Selection:        |                    |            |       |
 |  00100001B    |    0x84      |      0000h: NTB-16                | NCM Communications |   0        | None: |
 |               |              |      0001h: NTB-32                |      Interface     |            |       |
 |               |              | All other values are reserved     |                    |            |       |
 --------------------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9SetNTBFormat(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    mbim_parameters *mbim_param_ptr = MBIM_DEV_PARAMS_PTR();

    if (setup)
    {
        if(setup_ptr->LENGTH == 0)
        {
		    switch (setup_ptr->VALUE)
		    {
        		case 0x0000:
        		{
        			mbim_param_ptr->parser_opts = (ndp_parser_opts *)(&ndp16_opts);
        			break;
    			}

        		case 0x0001:
        		{
        			mbim_param_ptr->parser_opts = (ndp_parser_opts *)(&ndp32_opts);
        			break;
    			}

        		default:
        		{
        			break;
    			}
		   }

		   mvUsbCh9SendEp0Data(handle, 0, 0);
		}
	}
}

/*=================================================================================================

FUNCTION mvUsbCh9GetCRCMode

DESCRIPTION
  This request returns the currently selected CRC mode for NTBs formatted by the function.

  To get the full response, the host should set wLength to at least 2.  The function shall never
  return more than 2 bytes in response to this command.

  Two values are possible.  The function shall return 0000h if CRCs are not being appended to datagrams.
  The function shall return 0001h if CRCs are being appended to datagrams.

DEPENDENCIES

 --------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |    wValue   |      wIndex        |  wLength   |      DATA        |
 |               |              |             |                    |   Number   | The current CRC  |
 |  10100001B    |    0x89      |     Zero    | NCM Communications |of bytes to | mode in little   |
 |               |              |             |      Interface     |    read    | endian order.    |
 |               |              |             |                    |            |  (2 bytes)       |
 ---------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9GetCRCMode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    UINT16 is_crc;
    mbim_parameters *mbim_param_ptr = MBIM_DEV_PARAMS_PTR();

    if (setup)
    {
        if((setup_ptr->LENGTH >= 2)&&(setup_ptr->VALUE == 0))
        {
            is_crc = mbim_param_ptr->is_crc ? 0x0001 : 0x0000;

            mvUsbCh9SendEp0Data (handle, (UINT8 *)&is_crc, 2);

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, 0, 0);
		}
	}
}

/*=================================================================================================

FUNCTION mvUsbCh9SetCRCMode

DESCRIPTION
  This request controls whether the function will append CRCs to datagrams when formatting NTBs to
  be sent to the host. If the value passed in wValue is not valid, the function shall return an error
  response (a STALL PID) and shall not change the CRC mode.

  The function's CRC mode is set to a default value by events outside the scope of this command;
  see section 7.1 for details.

DEPENDENCIES

 --------------------------------------------------------------------------------------------------------------------
 | bmRequestType | bRequestCode |             wValue                 |      wIndex        |  wLength   |   DATA     |
 |               |              |            CRC mode:               |                    |            |            |
 |  00100001B    |    0x8A      | 0000h: CRCs shall not be appended  | NCM Communications |     0      |    None    |
 |               |              | 0001h: CRCs shall be appended      |      Interface     |            |            |
 |               |              | All other values are reserved.     |                    |            |            |
 --------------------------------------------------------------------------------------------------------------------

RETURN VALUE
  return staus

SIDE EFFECTS
  none

=====================================================================================================*/
void mvUsbCh9SetCRCMode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    int ndp_hdr_crc = 0;
    mbim_parameters *mbim_param_ptr = MBIM_DEV_PARAMS_PTR();

    if (setup)
    {
        if(setup_ptr->LENGTH == 0)
        {
    		switch (setup_ptr->VALUE)
    		{
        		case 0x0000:
        		{
        			mbim_param_ptr->is_crc = FALSE;
        			ndp_hdr_crc = NCM_NDP_HDR_NOCRC;
        			break;
    			}
        		case 0x0001:
        		{
        			mbim_param_ptr->is_crc = TRUE;
        			ndp_hdr_crc = NCM_NDP_HDR_CRC;
        			break;
    			}
        		default:
        		{
                    break;
    			}
    		}
    		mbim_param_ptr->parser_opts->ndp_sign &= ~NCM_NDP_HDR_CRC_MASK;
    		mbim_param_ptr->parser_opts->ndp_sign |= ndp_hdr_crc;
		}
	}
}
//#endif  /* MV_USB_MBIM.     */


/***********************************************************************************************
* Function: mvUsbCh9SetNetAddress
*
* Descripition:
*   This request returns the function¡¯s current EUI-48 station address.
*
* Parameters:
*   Inputs:
*   Outputs:
*
* Returns:
*   N/A.
*
* Notes:
*   N/A.
*************************************************************************************************/
void mvUsbCh9SetNetAddress( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr )
{
    return;

}



/***********************************************************************************************
* Function: mvUsbCh9GetNetAddress
*
* Descripition:
*   This request returns the function¡¯s current EUI-48 station address.
*
* Parameters:
*   Inputs:
*   Outputs:
*
* Returns:
*   N/A.
*
* Notes:
*   N/A.
*************************************************************************************************/
void mvUsbCh9GetNetAddress( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr )
{
    return;

}

//For Class Request, temporary
UINT8 USB2_SetComm2LineFlag=0;
void mvUsbCh9SetCommFeature(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    mvUsbNetQ *pCtrlQ = NULL;
    uint_8 *ctrl_data = NULL;

    if(setup)
    {
        if ( setup_ptr->LENGTH == 0 )
        {
            ErrorLogPrintf("SetLineCode: Zero Length");
        }
        else
        {
            /* host only transmits setup packet, notify user so user can reply with ZLP */
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_SET_COMM_FEA;
            USB_DEVICE_ASSERT(setup_ptr->LENGTH <= USB_EP0_MAX_RX_TRANSFER_SIZE );

            pCtrlQ = (mvUsbNetQ *)mvUsbAllocate(0);
            USB_ALLOC_MEM_STATUS(pCtrlQ);

            pCtrlQ->resv  = REQ_RCV_COMM_FEATURE_DATA;
            pCtrlQ->cnt   = setup_ptr->INDEX;

            ctrl_data = (uint_8 *)pCtrlQ + sizeof(mvUsbNetQ);

            mvUsbCh9RecvEp0Data(handle, ctrl_data, setup_ptr->LENGTH);
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "H2D - SETUP - prepare recv OUT data (H2D) len = %d\n", setup_ptr->LENGTH);
        }
    }

}


void mvUsbCh9GetCommFeature(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    static UINT16 Comm_Feature;

    if(setup)
    {
        if ( setup_ptr->VALUE== 2 )
        {
            _usb_device_stall_endpoint(handle, 0, 0);
        }
        else
        {
            /* host only transmits setup packet, notify user so user can reply with ZLP */
            Comm_Feature = mvUsbGetCommFeature(setup_ptr->INDEX);
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
            mvUsbCh9SendEp0Data(handle, (uint_8_ptr)&Comm_Feature, 2 /*size*/);

            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, NULL, 0);
        }
    }

}

void mvUsbCh9ClearCommFeature(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
	if(setup)
	{
        _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;
        /* status phase */
        mvUsbCh9SendEp0Data(handle, 0, 0);
        mvUsbClearCommFeature(setup_ptr->INDEX);
	}

}

void mvUsbCh9SetCtrlLineST(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
	if(setup)
	{

        _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

        /* status phase */
        mvUsbCh9SendEp0Data(handle, 0, 0);
        mvUsbCommNotifyControlLineSt(setup_ptr->INDEX, setup_ptr->VALUE);
	}

}

UINT8 USB_Line_Coding[7]={0x00, 0x10, 0x0e, 0x00,  0x00, 0x00, 0x08};


//For Class Request, temporary
UINT8 SetLine_Time = 0;

void mvUsbCh9SetLineCode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    mvUsbNetQ *pCtrlQ = NULL;
    uint_8 *ctrl_data = NULL;

    if(setup)
    {
        if ( setup_ptr->LENGTH == 0 )
        {
            ErrorLogPrintf("SetLineCode: Zero Length");
            _usb_device_stall_endpoint(handle, 0, 0);
        }
        else
        {
            /* host only transmits setup packet, notify user so user can reply with ZLP */
            USB_DEVICE_ASSERT(setup_ptr->LENGTH <= USB_EP0_MAX_RX_TRANSFER_SIZE );

            _usbDeviceDatabase.ep0.state =USB_DEVICE_EP0_SET_LINE_CODE;

            SetLine_Time++;

            pCtrlQ = (mvUsbNetQ *)mvUsbAllocate(0);
            USB_ALLOC_MEM_STATUS(pCtrlQ);

            pCtrlQ->resv  = REQ_RCV_LINE_CODING_DATA;
            pCtrlQ->cnt   = setup_ptr->INDEX;

            ctrl_data = (uint_8 *)pCtrlQ + sizeof(mvUsbNetQ);

            /* Ack with ZLP. */
            mvUsbCh9RecvEp0Data(handle, ctrl_data, setup_ptr->LENGTH);
            MV_USB_DEBUG_TRACE(MV_USB_DEBUG_FLAG_EP0, "H2D - SETUP - prepare recv OUT data (H2D) len = %d\n", setup_ptr->LENGTH);
        }

        if(SetLine_Time >1)
        {
            ErrorLogPrintf("Error receive con LineCode");
        }

    }

}

void mvUsbCh9GetLineCode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    if(setup)
    {
        if ( setup_ptr->LENGTH == 0 )
        {
            _usb_device_stall_endpoint(handle, 0, 0);
        }
        else
        {
            /* host only transmits setup packet, notify user so user can reply with ZLP */
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;

            mvUsbCommGetLineCoding( setup_ptr->INDEX, USB_Line_Coding);

            mvUsbCh9SendEp0Data (handle, (UINT8 *)USB_Line_Coding, setup_ptr->LENGTH);
            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, 0, 0);
        }
    }

}

#ifdef MV_USB2_MASS_STORAGE
void mvUsbCh9GetMaxLun(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

	USBGetMaxLun=1;

	if(setup)
	{
        //_usbDeviceDatabase.ep0.ctrl_out_data_stage_current_length = 0;
        if (( setup_ptr->VALUE != 0 )||( setup_ptr->INDEX > 0x6 ))
        {
            _usb_device_stall_endpoint(handle, 0, 0);
        }
        else // setupPacket.LENGTH > 0) /* host only transmits setup packet, notify user so user can reply with ZLP */
        {
            if(setup_ptr->LENGTH >= 1)
            {
                _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_DATA_IN;
                mvUsbCh9SendEp0Data (handle, (UINT8 *)&(pMscProp->MscMaxLun), 1);
                _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

                _usb_dci_vusb20_reset_data_toggle(handle, USB_MASS_STORAGE_RX_ENDPOINT, MV_USB_RECV);
                _usb_dci_vusb20_reset_data_toggle(handle, USB_MASS_STORAGE_TX_ENDPOINT, MV_USB_SEND);
            }

            /* status phase */
            mvUsbCh9RecvEp0Data(handle, 0, 0);
        }

	}

}

void mvUsbCh9BotMscReset(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    mvUsbNetQ *pCtrlQ = NULL;
    uint_8 *ctrl_data = NULL;

	if(setup)
	{
        //_usbDeviceDatabase.ep0.ctrl_out_data_stage_current_length = 0;
        if (( setup_ptr->VALUE != 0 )||( setup_ptr->INDEX > 0x6 ))
        {
            _usb_device_stall_endpoint(handle, 0, 0);
        }
        else
        {
            if(setup_ptr->LENGTH != 0)
            {
                pCtrlQ = (mvUsbNetQ *)mvUsbAllocate(0);
                USB_ALLOC_MEM_STATUS(pCtrlQ);

                pCtrlQ->resv  = REQ_RCV_BOT_MSC_RESET_DATA;

                ctrl_data = (uint_8 *)pCtrlQ + sizeof(mvUsbNetQ);

                /* Ack with ZLP. */
                mvUsbCh9RecvEp0Data(handle, ctrl_data, setup_ptr->LENGTH);
            }
            else
            {
                _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

                /* status phase */
                mvUsbCh9SendEp0Data(handle, 0, 0);
            }
        }
	}
}
#endif

/*******************************************************************************
* Function    : mvUsbCh9EncapsulatedCmd
* Parameter   : void
* Returns     : Nothing
* Description : This function process the RNDIS Encapsulated command.
*
******************************************************************************/

void mvUsbCh9EncapsulatedCmd(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    uint_8 rx_status = 0;
    mvUsbNetQ *pCtrlQ = NULL;
    uint_8 *ctrl_data = NULL;

    if(setup)
    {
        if ( setup_ptr->LENGTH == 0 )
        {
            ErrorLogPrintf("Encapsulated Cmd: Zero Length");

            rx_status = mvUsbCh9SendEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(rx_status == 0);
        }
        else
        {
			 if(mvUsbGetNetType() == USB_NET_RNDIS) {
             	USB_DEVICE_ASSERT(setup_ptr->LENGTH <= USB_EP0_MAX_RX_TRANSFER_SIZE );
			 }

             pCtrlQ = (mvUsbNetQ *)mvUsbAllocate(0);
             USB_ALLOC_MEM_STATUS(pCtrlQ);

             pCtrlQ->resv  = REQ_RCV_ENCAPSULATED_DATA;

             ctrl_data = (uint_8 *)pCtrlQ + sizeof(mvUsbNetQ);

             ctrl_data[0] = 0;

            USB_TRACE("mvUsbCh9EncapsulatedCmd, recv data:\n");
             rx_status = mvUsbCh9RecvEp0Data(handle, (uint_8_ptr)ctrl_data, setup_ptr->LENGTH);     /* Ack with ZLP. */
             USB_DEVICE_ASSERT(rx_status == 0);
        }
    }
}

/*******************************************************************************
* Function    : mvUsbCh9GetEncapsulatedRsp
* Parameter   : void
* Returns     : Nothing
* Description : This function process the RNDIS Encapsulated Response.
*
******************************************************************************/

void mvUsbCh9GetEncapsulatedRsp(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr)
{
    uint_8 rx_status = 0;
    mvUsbNetQ *pRspQ = NULL;

    if(setup)
    {
        USB_TRACE("mvUsbCh9GetEncapsulatedRsp\n");

        if ( setup_ptr->LENGTH == 0 )
        {
            USB_TRACE("Encapsulated Rsp: zero Length\n");

            rx_status = mvUsbCh9SendEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(rx_status == 0);
        }
        else
        {
            pRspQ = mvUsbNetQGet(&UsbNetRspQHdr);
            if(pRspQ == NULL)
            {
                USB_TRACE("No Encaps Rsp\n");
                return;
            }

            pRspQ->resv = REQ_GET_ENCAPSULATED_RESPONSE;


            rx_status = mvUsbCh9SendEp0Data(handle, pRspQ->data, pRspQ->len);
            USB_DEVICE_ASSERT(rx_status == 0);

            rx_status = mvUsbCh9RecvEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(rx_status == 0);

            UsbGetEncapsulatedRsp = TRUE;
        }
    }

}

void mvUsbProcessNonSetupData(void* handle, uint_8 dir, uint_8* buffer, uint_32 length)
{
    uint_8 status = 0;
    mvUsbNetQ *pDataQ = NULL;

    if(!mvUsbCheckPoolAddressValidity((unsigned int)buffer))
    {
        return;
    }

    pDataQ = (mvUsbNetQ *)(buffer - sizeof(mvUsbNetQ));

    switch(pDataQ->resv)
    {
        case REQ_RCV_COMM_FEATURE_DATA:
        {
            USB_DEVICE_ASSERT((dir == MV_USB_RECV) && (length != 0));

            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

            status = mvUsbCh9SendEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(status == 0);

            mvUsbSetCommFeature(pDataQ->cnt, buffer[0]);

            USB2_SetComm2LineFlag = 1;

            mvUsbDeallocate((void *)pDataQ);
            break;
        }

        case REQ_RCV_LINE_CODING_DATA:
        {
            USB_DEVICE_ASSERT((dir == MV_USB_RECV) && (length != 0));

            memcpy(USB_Line_Coding, buffer, 0x07);

            SetLine_Time = 0;

            _usbDeviceDatabase.ep0.state = USB_DEVICE_EP0_STATE_STATUS;

            status = mvUsbCh9SendEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(status == 0);

            if(USB2_SetComm2LineFlag)
            {
                mvUsbCommSetLineCoding(pDataQ->cnt, USB_Line_Coding);
                USB2_SetComm2LineFlag = 0;
            }

            mvUsbDeallocate((void *)pDataQ);
            break;
        }

        case REQ_RCV_ENCAPSULATED_DATA:
        {
            USB_TRACE("REQ_RCV_ENCAPSULATED_DATA\n");

            USB_DEVICE_ASSERT((dir == MV_USB_RECV) && (length != 0));

            status = mvUsbCh9SendEp0Data(handle, 0, 0);
            USB_DEVICE_ASSERT(status == 0);

            pDataQ->data = buffer;
            pDataQ->len  = length;

            mvUsbNetType type = mvUsbGetNetType();

            switch(type)
            {
                case USB_NET_RNDIS:
                {
                    pDataQ->resv  = USBNET_RNDIS_REQ_MSG;
                    break;
                }

                case USB_NET_MBIM:
                {
                    pDataQ->resv  = USBNET_MBIM_REQ_MSG;
                    break;
                }

                case USB_NET_ECM:
                {
                    pDataQ->resv  = USBNET_ECM_REQ_MSG;
                    break;
                }

                default:
                {
                    ASSERT(0);
                    break;
                }

            }

            mvUsbNetQPut(&UsbNetReqQHdr, pDataQ);

            /* Parse usb net control message. */
            mvUsbNetCtrlLISR(USB_NET_RNDIS, buffer, length);
            break;
        }

        case REQ_GET_ENCAPSULATED_RESPONSE:
        {
            USB_DEVICE_ASSERT((dir == MV_USB_SEND) && (length != 0));

            mvUsbDeallocate((void *)pDataQ);

            USB_TRACE("REQ_GET_ENCAPSULATED_RESPONSE end\n");
            break;
        }

        case REQ_RCV_BOT_MSC_RESET_DATA:
        {
            USB_DEVICE_ASSERT((dir == MV_USB_RECV) && (length != 0));

            mvUsbDeallocate((void *)pDataQ);

            _usb_device_stall_endpoint(handle, 0, 0);
            break;
        }

//#ifdef MV_USB_MBIM
        case REQ_SET_NTB_INPUT_SIZE:
        {
            break;
        }
//#endif
        default:
        {
            ErrorLogPrintf("Unknown NonSetup type 0x%x", pDataQ->resv);
            break;
        }
    }

}

void mvUsbEP0MemFree(void *buffer)
{
    if(!mvUsbCheckPoolAddressValidity((unsigned int)buffer))
    {
        return;
    }

    mvUsbDeallocate((void *)((uint_8 *)buffer - sizeof(mvUsbNetQ)));
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCh9RecvEp0Data                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function receive data from endpoint 0.                      */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                Data pointer                 */
/*      len                                 Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
uint_8 mvUsbCh9RecvEp0Data(_usb_device_handle handle, uint_8_ptr buff_ptr, uint_32 size)
{
    uint_8 rx_status = 0;

    if (_usb_device_get_transfer_status(handle, 0, MV_USB_RECV) != MV_USB_STATUS_IDLE)
    {
        _usb_dci_vusb20_free_ep_transfer( 0, MV_USB_RECV, mvUsbEP0MemFree );
    }

    rx_status = _usb_device_recv_data(handle, 0, buff_ptr, size);

    return rx_status;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCh9SendEp0Data                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function send data to endpoint 0.                           */
/*                                                                       */
/* CALLED BY                                                             */
/*                                                                       */
/*      Application                                                      */
/*                                                                       */
/* CALLS                                                                 */
/*                                                                       */
/*      Application                         The application function     */
/*                                                                       */
/* INPUTS                                                                */
/*                                                                       */
/*      data                                Data pointer                 */
/*      len                                 Data length                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      none                                N/A                          */
/*                                                                       */
/*************************************************************************/
uint_8 mvUsbCh9SendEp0Data(_usb_device_handle handle, uint_8_ptr buff_ptr, uint_32 size)
{
    uint_8 rx_status = 0;

    if (_usb_device_get_transfer_status(handle, 0, MV_USB_SEND) != MV_USB_STATUS_IDLE)
    {
        _usb_dci_vusb20_free_ep_transfer( 0, MV_USB_SEND, mvUsbEP0MemFree );
    }

    rx_status = _usb_device_send_data(handle, 0, buff_ptr, size);

    return rx_status;
}

