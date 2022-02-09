#ifndef	USB_INIT_H
#define USB_INIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                usb_init.h


GENERAL DESCRIPTION

    This file is for USB initialize.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2011 by Marvell, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who        what, where, why
--------   ------     ----------------------------------------------------------
03/07/2013   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//#include "utils.h"
#include "usb_device.h"
#include "usbTrace.h"
#include "usb_macro.h"
//#include "diag_nvm.h"
//#include "rndis.h"
#include "platform.h"
#include "mvUsbStorage.h"

/*===========================================================================

                         LOCAL MACRO
===========================================================================*/

/* Usb normal mode */
#define USB_NORMAL_MODE		                            0x00

/* WIFI Calibration mode */
#define USB_WIFI_CALIBRATION_MODE		                0x01

/* RNDIS/ECM only mode */
#define USB_RNDIS_ECM_ONLY_MODE		                    0x02

/* RNDIS/ECM + Modem mode */
#define USB_RNDIS_ECM_MODEM_MODE		                0x03

/* Diag PDU check mode */
#define USB_DIAG_PDU_CHECK_MODE		                    0x10

/* Usb select task stack size */
#define USB_SELECT_TASK_STACK_SIZE                      1024

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/



/* Type definition for USB Descriptor. */
typedef enum
{
	USB_GENERIC_MIFI_DESCRIPTOR     = 27,
    USB_MARVELL_MIFI_DESCRIPTOR     = 30,
    USB_RNDIS_ONLY_DESCRIPTOR	    = 33,
    USB_ASR_MIFI_DESCRIPTOR         = 34,
    USB_MARVELL_ECM_DESCRIPTOR      = 40,
    USB_MBIM_DESCRIPTOR				= 56,
    USB_MODEM_DIAG_DESCRIPTOR       = 57,
    USB_MODEM_ONLY_DESCRIPTOR       = 58,
	USB_CDROM_ONLY_DESCRIPTOR       = 66,
	USB_CDROM_DIAG_DESCRIPTOR       = 67,
	USB_DIAG_ONLY_DESCRIPTOR        = 68,
    USB_GENERIC_MOD_ECM_DESCRIPTOR  = 69,
	USB_MAX_DESCRIPTOR_TYPE
}PlatformUsbDescType;


//ICAT EXPORTED ENUM
typedef enum
{
    /* MIFI driver*/
	USB_GENERIC_MIFI_DRIVER = 0x0,
	USB_MARVELL_MIFI_DRIVER = 0x1,
	USB_ASR_MIFI_DRIVER 	= 0x2,
	USB_ASR_MBIM_DRIVER 	= 0x6,
	USB_GENERIC_MOD_DRIVER =  0x10,
	USB_GENERIC_MOD_ECM_DRIVER = 0x12,
	USB_MAX_MIFI_DRIVER     = 0x80,

    /* Dongle driver*/
	USB_MAX_DGLE__DRIVER    = 0x90,

	/* Other driver*/
	USB_CDROM_ONLY_DRIVER   = 0x91,
	USB_CDROM_DIAG_DRIVER   = 0x92,
	USB_DIAG_ONLY_DRIVER    = 0x93,
	USB_MODEM_ONLY_DRIVER   = 0x94,
	USB_MODEM_DIAG_DRIVER   = 0x95,

	USB_MAX_DRIVER          = 0xFF
} Usb_driver_typeE;




//ICAT EXPORTED ENUM
typedef enum
{
	MASS_STORAGE_DISABLE = 0x0,
	MASS_STORAGE_ENABLE  = 0x1
} MassStorage_ConfigE;

//ICAT EXPORTED ENUM
typedef enum
{
	USB_AUTO_INSTALL_DISABLE = 0x0,
	USB_AUTO_INSTALL_ENABLE  = 0x1
} Usb_auto_install_type;

//ICAT EXPORTED ENUM
typedef enum
{
	USB_OS_DETECT_DISABLE = 0x0,
	USB_OS_DETECT_ENABLE  = 0x1
} Usb_OS_detect_type;

//ICAT EXPORTED STRUCT
typedef struct {
	Usb_driver_typeE usb_driver;
	MassStorage_ConfigE mass_storage;
	Usb_auto_install_type auto_install;
	Usb_OS_detect_type os_detect;
} Usb_DriverS;

/* Usb descriptor information. */
typedef struct
{
    char UsbMode;
    PlatformUsbDescType DefaultDesc;
    PlatformUsbDescType ReConfigDesc;
    PlatformUsbDescType CurrentDesc;
} mvUsbDescriptorInfo;


typedef struct usbIntDebug_struct
{
	unsigned long ep0;
	unsigned long epx;
	unsigned short request_type;
	unsigned short request;
	unsigned long timeStamp;
} usbIntDebug_t;




/*===========================================================================

                        EXTERN FUNCTION DECLARATIONS

===========================================================================*/
/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GetUsbMode                                                       */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function Get the usb mode.                                   */
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
extern char GetUsbMode(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      CheckIf32MNOR                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      If Manfactur ID = 0xXX19, this is 32M Nor Flash.                 */
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
extern BOOL CheckIf32MNOR(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      UsbDriverGetSetting                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function read usb configuration from NVM files.              */
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
extern void UsbDriverGetSetting(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      diagPhase2Init                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize usb diag device.                         */
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
extern void diagPhase2Init(void);

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
extern BOOL USB2IsAutoInstallEnable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbModemInitialize                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize usb modem device.                        */
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
extern void mvUsbModemInitialize(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      sdcard_is_insert                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether the SD card is insert or not.         */
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
extern BOOL sdcard_is_insert(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      diagCommSdLogEnable                                              */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether the Diag SD logging is enable or not. */
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
extern BOOL diagCommSdLogEnable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      diag_online_sdl_enable                                           */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether the online sd log is enable or not.   */
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
//extern BOOL diag_online_sdl_enable(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      GetUsbDriverConfig                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function get the usb driver configuration.                   */
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
/*      Usb_DriverS                         Driver configuration.        */
/*                                                                       */
/*************************************************************************/
extern Usb_DriverS *GetUsbDriverConfig(void);

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
extern void USB2MgrUpdateDescriptor(PlatformUsbDescType desc, char mode);

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
extern mvUsbDescriptorInfo *USB2GetDescriptorInfo(void);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbCheckProdctionMode                                          */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function check whether the production mode is enable or not. */
/*      When uAP KEY is pressdown, The 'PRMD' flag is set in memory.     */
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
/*      TRUE                                Flag is set.                 */
/*      FALSE                               Flag is not set.             */
/*                                                                       */
/*************************************************************************/
BOOL mvUsbCheckProdctionMode( void );

#ifdef MV_USB2_MASS_STORAGE
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
void mvUsbStorageConfigure(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbMassStorageInit                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the mass storage deivce.                 */
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
void mvUsbMassStorageInit(void);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbStorageRead                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function read data from mass storage.                        */
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
BOOL mvUsbStorageRead(unsigned int address, unsigned int length, unsigned char *buf, unsigned char lun);

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      mvUsbStorageWrite                                                */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function write data to mass storage.                         */
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
BOOL mvUsbStorageWrite(unsigned int address, unsigned int length, unsigned char *buf, unsigned char lun);

#endif /* MV_USB2_MASS_STORAGE */

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      usb_device_init                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The function initialize the usb devices.                         */
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
void usb_device_init(void );

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*                                                                       */
/*      USBSelectTask                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*                                                                       */
/*      The USB select task.                                             */
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
void USBSelectTask(void *argv);


#define ErrorLogPrintf uart_printf

#endif /* USB_INIT_H */
