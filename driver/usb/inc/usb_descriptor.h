#ifndef _USB_DESCRIPTOR_H_
#define _USB_DESCRIPTOR_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                usb_descriptor.h


GENERAL DESCRIPTION

    This file is for USB descriptor.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2018 by ASR, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who        what, where, why
--------   ------     ----------------------------------------------------------
06/12/2018   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "usb_init.h"
#include "usb_macro.h"

/*===========================================================================

                                LOCAL MACRO
===========================================================================*/

/* USB Modem control endpoint. */
#define MdmCtrlEP       (MODEM_CTRL_ENDPOINT | 0x80)

/* USB Diag control endpoint. */
#define DiagCtrlEP      (DIAG_CTRL_ENDPOINT | 0x80)

/* Usb Net control endpoint. */
#define NetCtrlEP       (USB_NET_CRTL_ENDPOINT | 0x80)

/* Usb Modem TX endpoint. */
#define MdmTxEP         (MODEM_TX_ENDPOINT | 0x80)

/* Usb Diag TX endpoint. */
#define DiagTxEP        (DIAG_TX_ENDPOINT | 0x80)

/* Usb Net TX endpoint. */
#define NetTxEP         (USB_NET_TX_ENDPOINT | 0x80)

/* Usb Mass Storage TX endpoint. */
#define MSCTxEP         (USB_MASS_STORAGE_TX_ENDPOINT | 0x80)

/* Usb Sulog TX endpoint. */
#define SulogTxEP       (SULOG_TX_ENDPOINT | 0x80)

/* Modem RX endpoint. */
#define MdmRxEP         (MODEM_RX_ENDPOINT)

/* Diag RX endpoint. */
#define DiagRxEP        (DIAG_RX_ENDPOINT)

/* Usb Net RX endpoint. */
#define NetRxEP         (USB_NET_RX_ENDPOINT)

/* Usb Mass Storage RX endpoint. */
#define MSCRxEP         (USB_MASS_STORAGE_RX_ENDPOINT)

/* Usb sulog RX endpoint. */
#define SulogRxEP       (SULOG_RX_ENDPOINT)


/* merger from 1802s as follows */
/* USB Modem 2rd control point*/
#define AtCtrlEP        (AT_CTRL_ENDPOINT | 0x80)

/* USB Modem 2rd TX point*/
#define AtTxEP          (AT_TX_ENDPOINT | 0x80)

/* USB Modem 2rd RX point*/
#define AtRxEP          (AT_RX_ENDPOINT)


/*===========================================================================

                          Struct definition.

===========================================================================*/

/*===========================================================================

            EXTERN DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      SetUsbMode                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function Set Usb Mode.                                      */
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
/*      size                                Memory size                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
extern void SetUsbMode(char mode);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      eeGetSystemAssertFlag                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function get system assert flag.                            */
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
/*      size                                Memory size                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
extern BOOL eeGetSystemAssertFlag(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      _usb_dci_vusb20_stop_transter                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      This function stop usb transfer.                                 */
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
/*      size                                Memory size                  */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
extern void _usb_dci_vusb20_stop_transter(void);;

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbStorageConfigure                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function configurate the parameters of mass storage.         */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
extern void mvUsbStorageConfigure(void);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrUpdateDescriptor                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function update the usb descriptor.                          */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2MgrUpdateDescriptor(PlatformUsbDescType desc, char mode);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2ReEnumerate                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function re-enumerate the usb device.                        */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2ReEnumerate(PlatformUsbDescType Desc);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USBIsConnected                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether usb is connected or not.              */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
char USBIsConnected(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2ConfigureCdromDescriptor                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function configurae CDROM only descriptor.                   */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2ConfigureCdromDescriptor(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2ProcessGetConfigDescRequest                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function process the get configuration descriptor request.   */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2ProcessGetConfigDescRequest(UINT16 value);


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2GetDescriptorInfo                                            */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the usb descriptor information.                 */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
mvUsbDescriptorInfo *USB2GetDescriptorInfo(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2IsAutoInstallEnable                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Check whether auto-installation is enable or not.   */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL USB2IsAutoInstallEnable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2IsCdromOnlyDescriptor                                        */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Check whether it is COROM only or not.              */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL USB2IsCdromOnlyDescriptor(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrIsDeviceControllerEnabled                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Check whether usb controller is enable or not.      */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL USB2MgrIsDeviceControllerEnabled (void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrDeviceUnplug                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function simulate usb plug out.                              */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2MgrDeviceUnplug(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrDevicePlugIn                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function simulate usb plug in.                               */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2MgrDevicePlugIn(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrDeviceUnplugPlug                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function simulate usb plug in/out.                           */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2MgrDeviceUnplugPlug(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrDeviceUnplugPlug                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function simulate usb plug in/out for SD card.               */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2MgrDeviceUnplugPlug_SD(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrDeviceUnplugPlug_WebDav                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function simulate usb plug in/out for WebDav.                */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
void USB2MgrDeviceUnplugPlug_WebDav(unsigned char enable);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrMassStorageEnable                                         */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether mass storage is enable or not.        */
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
/*      None                                N/A                          */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL USB2MgrMassStorageEnable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USB2MgrChangeUsbMode                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function change usb mode.                                    */
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
/*      mode                                usb mode                     */
/*                                                                       */
/* OUTPUTS                                                               */
/*                                                                       */
/*      None                                N/A                          */
/*                                                                       */
/*************************************************************************/
BOOL USB2MgrChangeUsbMode(char mode);
void USB2MgrDeviceUnplugPlug_plus(PlatformUsbDescType desc);

#endif /* _USB_DESCRIPTOR_H_ */
