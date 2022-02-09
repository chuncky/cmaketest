#ifndef _MVUSBSTORAGE_H_
#define _MVUSBSTORAGE_H_
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbStorage.h


GENERAL DESCRIPTION

    This file contains the definition for USB mass storage.

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
12/23/07   zhoujin    Created module
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
//#include "osa.h"
//#include "global_types.h"
#include "udc2_hw.h"
#include "usb_device.h"
//#include "platNvmConfig.h"

/*===========================================================================

                                LOCAL MACRO
===========================================================================*/

/* The Max USB Mass storage Logical Unit Number*/
#define MSC_MAX_LUN                             0x3

/* Event Descriptor Length */
#define CDROM_NOTIFICATION_LENGTH               0x0

/* CDROM TOC Length */
#define CDROM_TOC_LENGTH                        0x23

/* CDROM ID offset */
#define CDROM_ID_OFFSET                         0x8000

/* Mass storage flash manage task*/
#define FlashUSBMSC_STACKSIZE                   4096
#define FlashUSBMSC_PRIORITY                    120
#define FlashUSBMSC_MESSAGE_Q_MAX               10
#define FlashUSBMSC_UPDATE_INTERVAL             100

/* CBW and CSW signatrue */
#define USB_DCBWSIGNATURE                       (0x43425355)
#define USB_DCSWSIGNATURE                       (0x53425355)
#define USB_CBW_DIRECTION_BIT                   (0x80)

/* The Max receive size */
#define MSC_RX_MAXSIZE                          (131072)

/* Mass storage block size */
#define MSC_BLOCK_SIZE                          NAND_SECTOR_SIZE

/* SD block count */
#define MSC_SD_BLOCK_CNT                        0x80000

/* The last sector CDROM read */
#define CDROM_INSTALL_LASTSECTOR                0x00d0

/* size of CDROM block: flash is 512B, CDROM is 2048B */
#define CDROM_BLOCK_SIZE                        2048

/* The ARC is little-endian, just like USB */
#define USB_uint16_low8(x)                      ((x) & 0xFF)
#define USB_uint16_high8(x)                     (((x) >> 8) & 0xFF)
#define USB_uint32_byte3(x)                     (((x)>>24) & 0xFF)
#define USB_uint32_byte2(x)                     (((x)>>16) & 0xFF)
#define USB_uint32_byte1(x)                     (((x)>>8) & 0xFF)
#define USB_uint32_byte0(x)                     ((x) & 0xFF)

/*===========================================================================

            Type declarations and Struct definition.

===========================================================================*/

/* USB Command Block Wrapper */
typedef struct
{
    UINT8                   buff[512];
} Flash_SectorS;

/* USB MSC media type*/
typedef enum
{
    USBMSC_FLASHDISK        = 0,
    USBMSC_CDROM            = 1,
    USBMSC_SDCARD           = 2,
    USBMSC_MEDIA_U32        = 0x7fffffff // force to 32-bit
}USBMSC_MEDIA;

/* USB Command Block Wrapper */
typedef struct cbw_struct {
    UINT32                  DCBWSIGNATURE;
    UINT32                  DCBWTAG;
    UINT32                  DCBWDATALENGTH;
    UINT8                   BMCBWFLAGS;
    UINT8                   BCBWCBLUN;
    UINT8                   BCBWCBLENGTH;
    UINT8                   CBWCB[16];
} CBW_STRUCT, * CBW_STRUCT_PTR;

/* USB Command Status Wrapper */
typedef  struct csw_struct {
    UINT32                  DCSWSIGNATURE;
    UINT32                  DCSWTAG;
    UINT32                  DCSWDATARESIDUE;
    UINT8                   BCSWSTATUS;
} CSW_STRUCT, * CSW_STRUCT_PTR;

/* USB Mass storage READ CAPACITY Data */
typedef struct mass_storage_read_capacity {
    UINT8                   LAST_LOGICAL_BLOCK_ADDRESS[4];
    UINT8                   BLOCK_LENGTH_IN_BYTES[4];
} MASS_STORAGE_READ_CAPACITY_STRUCT, *MASS_STORAGE_READ_CAPACITY_STRUCT_PTR;

/* USB MSC Properties type. */
typedef struct mvUsbMscProperties_T {
    UINT32                  Lun0StartAddress;
    UINT32                  Lun0EndAddress;
    UINT32                  Lun1StartAddress;
    UINT32                  Lun1EndAddress;
    UINT32                  Lun2StartAddress;
    UINT32                  Lun2EndAddress;
    UINT32                  MscMaxLun;
    USBMSC_MEDIA            Media[MSC_MAX_LUN];
} mvUsbMscProperties_T;

/* USB Mass storage Device information */
typedef struct mass_storage_device_info {
    UINT8                   PERIPHERAL_DEVICE_TYPE;     /* Bits 0-4. All other bits reserved */
    UINT8                   RMB;                        /* Bit 7. All other bits reserved */
    UINT8                   ANSI_ECMA_ISO_VERSION;      /* ANSI: bits 0-2, ECMA: bits 3-5 */
                                                        /* ISO: bits 6-7 */
    UINT8                   RESPONSE_DATA_FORMAT;       /* bits 0-3. All other bits reserved */
    UINT8                   ADDITIONAL_LENGTH;          /* For UFI device: always set to 0x1F */
    UINT8                   RESERVED1[3];
    UINT8                   VENDOR_INFORMATION[8];
    UINT8                   PRODUCT_ID[16];
    UINT8                   PRODUCT_REVISION_LEVEL[4];
} MASS_STORAGE_DEVICE_INFO_STRUCT, * MASS_STORAGE_DEVICE_INFO_PTR;

/* USB CDROM TOC */
typedef struct mass_storage_CDROM_TOC{
    UINT8                   DataLength[2];              /* TOC Data Length*/
    UINT8                   FirstTrack;                 /* First Track Number*/
    UINT8                   LastTrack;                  /* Last Track Number */
    UINT8                   Desc[33];
} MASS_STORAGE_CDROM_TOC_STRUCT, * MASS_STORAGE_CDROM_TOC_PTR;

/* USB CDROM GET EVENT STATUS NOTIFICATION */
typedef struct mass_storage_CDROM_NOTIFICATION{
    UINT8                   EventDescriptorLength[2];   /* Event Descriptor Length*/
    UINT8                   NEA_NotificationClass;      /* NEA: bits 7, Notification Class:bits 2-0*/
    UINT8                   SupportedEventClasses;      /* Supported Event Classes*/
} MASS_STORAGE_CDROM_NOTIFICATION_STRUCT, * MASS_STORAGE_CDROM_NOTIFICATION_PTR;

typedef struct mass_storage_buffer {
    USBDevice_EndpointE     endpoint;
	UINT8                   *buffer;
	UINT32                  length;
	BOOL                    endOfMsg;
} MASS_STORAGE_BUFFER_STRUCT, * MASS_STORAGE_BFFER_PTR;

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* System exception flag. */
extern UINT32 assertFlag;


/* Max logical number. */
extern UINT8 USBGetMaxLun;

/*===========================================================================

                          EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION _usb_dci_vusb20_diag_tx_isr

DESCRIPTION
  This function process the usb isr in system exception mode.

DEPENDENCIES
  none

RETURN VALUE
  FS type

SIDE EFFECTS
  none

===========================================================================*/
extern void _usb_dci_vusb20_diag_tx_isr(void);

/*===========================================================================

                          INTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DataCard_Identify_FS

DESCRIPTION
  Identify the FS type of Mass storage.

DEPENDENCIES
  none

RETURN VALUE
  FS type

SIDE EFFECTS
  none

===========================================================================*/
void DataCard_Identify_FS(BOOL identify);

/*===========================================================================

FUNCTION service_bulk_endpoint

DESCRIPTION
  Mass stotage receive enpoint service process.

DEPENDENCIES
  none

RETURN VALUE
  return staus

SIDE EFFECTS
  none

===========================================================================*/
void service_bulk_endpoint(UINT8 direction, UINT8* buffer,UINT32 length);

/*===========================================================================

FUNCTION GetMscProperties

DESCRIPTION
  Get Usb MSC proerties.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

mvUsbMscProperties_T *GetMscProperties(void);

/*===========================================================================

FUNCTION MSCRX_HISR

DESCRIPTION
  Mass storage receive HISR.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void MSCRX_HISR(void);

/*===========================================================================

FUNCTION MSCRX_LISR

DESCRIPTION
  Mass storage receive LISR.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void MSCRX_LISR(USBDevice_EndpointE endpoint, UINT8 *msgData, UINT32 length, BOOL endOfMsg);

/*===========================================================================

FUNCTION MSCRX_LISR

DESCRIPTION
  Mass storage transmit LISR.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void MSCTX_LISR(USBDevice_EndpointE endpoint, UINT8 *msgData, UINT32 length, BOOL endOfMsg);

/*===========================================================================

FUNCTION proc_flash_msgs

DESCRIPTION
  Flash message process.

DEPENDENCIES
  none

RETURN VALUE
  FS type

SIDE EFFECTS
  none

===========================================================================*/
static void proc_flash_msgs (UINT8*  rxmsg);

/*===========================================================================

FUNCTION FlashUSBMSC_Task

DESCRIPTION
  usb mass storage task.

DEPENDENCIES
  none

RETURN VALUE
  FS type

SIDE EFFECTS
  none

===========================================================================*/
static void FlashUSBMSC_Task (void* input);

/*===========================================================================

FUNCTION mvUsbStorage_Init

DESCRIPTION
  usb mass storage initialize.

DEPENDENCIES
  none

RETURN VALUE
  FS type

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbStorage_Init(void);

/*===========================================================================

FUNCTION mvUsbStorageNotifyHost

DESCRIPTION
  usb mass storage notify host of usb active.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#define mvUsbStorageNotifyUsbActive()                                       \
{                                                                           \
    ;\
}

//add by yazhouren@asrmicro.com
/* message IDs*/
typedef enum
{
    FLASH_SECTOR_READ,
    FLASH_SECTOR_READDONE,
    FLASH_SECTOR_READERROR,

    FLASH_SECTOR_WRITE,
    FLASH_SECTOR_WRITEDONE,
    FLASH_SECTOR_WRITEERROR,


    /* Must be at the end */
    FLASH_OPS_ENUM_32_BIT									= 0x7FFFFFFF //32bit enum compiling enforcement
} FLASH_OpsMsgID;

typedef enum
{
    FLASHZONE_LUN_0     = 0,
    FLASHZONE_LUN_1     = 1,
    FLASHZONE_LUN_2     = 2,
    FLASHZONE_LUN_MAX   = 0x7FFFFFFF
} FLASH_ZoneID;


typedef struct
{
    FLASH_OpsMsgID  MsgId;
    FLASH_ZoneID    ZoneId;
    UINT32      FlashAddress;  //Sector/Page/Block address
    UINT32		Buffer;
    UINT32	    Length;
} FlashMessage, *PFlashMessage;

#define FLASH_MESSAGE_SIZE 	sizeof(FlashMessage)

#if 0
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
#endif

/* The nand sector size */
#define NAND_SECTOR_SIZE				    512

/* The nand sector shit bit */
#define FLASH_SECTOR_BITSHIFT               9

/* The unused flash address */
#define UNUSED                              0xFFFFFFFF
#if 0
#define MIFI_LOG_TRACE(cat1, cat2, cat3, fmt, args...)      \
{                                                           \
}
#endif
void mvUsbMscAddNextRxBuffer(void);

#endif  /* _MVUSBSTORAGE_H_*/
