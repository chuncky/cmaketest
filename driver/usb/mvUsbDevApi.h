#ifndef __mvUsbDevApi_h__
#define __mvUsbDevApi_h__ 1
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
***  This file contains the declarations specific to the USB Device API
***
**************************************************************************
**END*********************************************************/

#include "udc2_hw.h"
#include "mvUsbDebug.h"
#include "mvUsbDefs.h"


/* Endpoint types */
#define  MV_USB_CONTROL_ENDPOINT             (0)
#define  MV_USB_ISOCHRONOUS_ENDPOINT         (1)
#define  MV_USB_BULK_ENDPOINT                (2)
#define  MV_USB_INTERRUPT_ENDPOINT           (3)

/* Informational Request/Set Types */
#define  MV_USB_STATUS_DEVICE_STATE          (0x01)
#define  MV_USB_STATUS_INTERFACE             (0x02)
#define  MV_USB_STATUS_ADDRESS               (0x03)
#define  MV_USB_STATUS_CURRENT_CONFIG        (0x04)
#define  MV_USB_STATUS_SOF_COUNT             (0x05)
#define  MV_USB_STATUS_DEVICE                (0x06)
#define  MV_USB_STATUS_TEST_MODE             (0x07)
#define  MV_USB_FORCE_FULL_SPEED             (0x08)
#define  MV_USB_PHY_LOW_POWER_SUSPEND        (0x09)
#define  MV_USB_STATUS_ENDPOINT              (0x10)
#define  MV_USB_STATUS_ENDPOINT_NUMBER_MASK  (0x0F)

#define  MV_USB_TEST_MODE_TEST_PACKET        (0x0400)

/* Available service types */
/* Services 0 through 15 are reserved for endpoints */
#define  MV_USB_SERVICE_EP0                  (0x00)
#define  MV_USB_SERVICE_EP1                  (0x01)
#define  MV_USB_SERVICE_EP2                  (0x02)
#define  MV_USB_SERVICE_EP3                  (0x03)
#define  MV_USB_SERVICE_EP4                  (0x04)
#define  MV_USB_SERVICE_EP5                  (0x05)
#define  MV_USB_SERVICE_EP6                  (0x06)
#define  MV_USB_SERVICE_EP7                  (0x07)
#define  MV_USB_SERVICE_EP8                  (0x08)
#define  MV_USB_SERVICE_EP9                  (0x09)
#define  MV_USB_SERVICE_EPA                  (0x0A)
#define  MV_USB_SERVICE_EPB                  (0x0B)
#define  MV_USB_SERVICE_EPC                  (0x0C)
#define  MV_USB_SERVICE_EPD                  (0x0D)
#define  MV_USB_SERVICE_EPE                  (0x0E)
#define  MV_USB_SERVICE_EPF                  (0x0F)      // 15 IN & 15 OUT ENDPOINTS in additionto ENDPOINT 0.

#define  MV_USB_SERVICE_BUS_RESET            (0x10)
#define  MV_USB_SERVICE_SUSPEND              (0x11)
#define  MV_USB_SERVICE_SOF                  (0x12)
#define  MV_USB_SERVICE_RESUME               (0x13)
#define  MV_USB_SERVICE_SLEEP                (0x14)
#define  MV_USB_SERVICE_SPEED_DETECTION      (0x15)
#define  MV_USB_SERVICE_ERROR                (0x16)
#define  MV_USB_SERVICE_STALL                (0x17)

typedef pointer _usb_device_handle;
typedef void (*USB_SERVICE_FUNC)(void* handle, uint_8, boolean, uint_8,
                                                 uint_8_ptr, uint_32, uint_8);

#ifdef __cplusplus
extern "C" {
#endif

void 	_usb_dci_vusb20_isr(void* handle);

uint_8  _usb_device_init(uint_8 devNo, void** pHandle);
void*   _usb_device_get_handle(uint_8 devNo);

uint_8  _usb_device_get_max_endpoint(void* handle);

void    _usb_device_shutdown(void* handle);

uint_8  _usb_device_init_endpoint(void* handle, uint_8 ep_num, uint_16 max_pkt_size,
	                              uint_8 direction, uint_8 type, uint_8 flag);
uint_8  _usb_device_deinit_endpoint(void* handle, uint_8 ep_num, uint_8 direction);

uint_8  _usb_device_recv_data(void* handle, uint_8 ep_num, uint_8* buf_ptr, uint_32 size);
uint_8  _usb_device_send_data(void* handle, uint_8 ep_num, uint_8* buf_ptr, uint_32 size);
uint_8  _usb_device_cancel_transfer(void* handle, uint_8 ep_num, uint_8 direction);
void    _usb_device_free_transfer(void* handle, uint_8 ep_num, uint_8 direction, void (*free)(void*));
uint_8  _usb_device_get_transfer_status(void* handle, uint_8 ep_num, uint_8 direction);
uint_8  _usb_device_get_dtd_cnt(uint_8 ep_num, uint_8 direction, uint_32_ptr StartTick);
void    _usb_device_get_transfer_details(void* handle, uint_8 ep_num, uint_8 direction ,uint_32_ptr _PTR_ xd_ptr_ptr );
void    _usb_device_stall_endpoint(void* handle, uint_8 ep_num, uint_8 direction);
void    _usb_device_unstall_endpoint(void* handle, uint_8 ep_num, uint_8 direction);
void    _usb_device_assert_resume(void* handle);
void    _usb_device_trace_dtd_information(uint_8 ep_num, uint_8 direction);
uint_8  _usb_device_get_status(void* handle, uint_8 component, uint_16* status_ptr);
uint_8  _usb_device_set_status(void* handle, uint_8 component, uint_16 setting);
void    _usb_device_read_setup_data(void* handle, uint_8 ep_num, uint_8* buf_ptr);
void    _usb_device_reset_data_toggle(void* handle, uint_8 ep_num, uint_8 direction);
uint_8  _usb_device_register_service(void* handle, uint_8 type, USB_SERVICE_FUNC serviceFunc);

uint_8  _usb_device_unregister_service(void* handle, uint_8 type);
void    USB2DevicePerformAfterEnum(void);
void    USB2DeviceEnableEndpoint(void);
void    USB2MgrDeviceUnplugPlug(void);
void    mvUsbEP0MemFree(void *buffer);


void USB2SetConfigHISR(void);
void USB2SetConfigLISR(void);


/* These functions that implement USB 2.0 standard Chapter 9 Setup requests */
void mvUsbCh9GetStatus(void* handle, boolean setup, SETUP_STRUCT* ctrl_req);
void mvUsbCh9ClearFeature(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetFeature(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetAddress(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetDescriptior(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetDescriptior(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetConfig(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetConfig(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetInterface(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetInterface(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void ch9Class(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9ProcessVendorRequest(void* handle, boolean setup, SETUP_STRUCT* setup_ptr);

/* These Functions are for Class Specific Request*/
void mvUsbCh9SetCommFeature(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetCommFeature(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9ClearCommFeature(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetCtrlLineST(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetLineCode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetLineCode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
#ifdef MV_USB2_MASS_STORAGE
void mvUsbCh9GetMaxLun(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9BotMscReset(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
#endif
void mvUsbCh9SetNetAddress( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr );
void mvUsbCh9GetNetAddress( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr );
void mvUsbCh9SetEthernetMulticastFilters( _usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr );
void mvUsbCh9SetEthernetPacketFilter(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

/* These Functions are for Rndis */
void mvUsbCh9EncapsulatedCmd(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetEncapsulatedRsp(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

uint_8 mvUsbCh9RecvEp0Data(_usb_device_handle handle, uint_8_ptr buff_ptr, uint_32 size);
uint_8 mvUsbCh9SendEp0Data(_usb_device_handle handle, uint_8_ptr buff_ptr, uint_32 size);

/* These Functions are for MBIM */
void mvUsbCh9GetNTBParameters(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetNTBInputSize(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetNTBInputSize(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetNTBFormat(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetNTBFormat(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9GetCRCMode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);
void mvUsbCh9SetCRCMode(_usb_device_handle handle, boolean setup, SETUP_STRUCT* setup_ptr);

void mvUsbProcessNonSetupData(void* handle, uint_8 dir, uint_8* buffer, uint_32 length);

/* DEBUG Functions */
void 	_usb_dci_vusb20_set_test_mode(void* handle, uint_16 testMode);

void    _usb_debug_set_flags(void* pflags);
uint_32 _usb_debug_get_flags(void);

void    _usb_debug_init_trace_log(void);
void    _usb_debug_print_trace_log(void);

void    _usb_regs(void);
void    _usb_dump_regs(void);
void    _usb_status(void);
void    _usb_stats(void);
void    _usb_clear_stats(void);
void    _usb_ep_status(int ep_num, int direction);

#ifdef __cplusplus
}
#endif


//fix build warnning
void SetUsbEnumFlag(uint_8 val);
unsigned long USB_IS_ENUMERATED(void);
unsigned long USB_IS_CONNECTED(void);

#endif /* __mvUsbDevApi_h__ */
/* EOF */

