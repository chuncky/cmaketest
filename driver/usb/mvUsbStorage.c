


#ifdef MV_USB2_MASS_STORAGE
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                mvUsbStorage.c


GENERAL DESCRIPTION

    This file is for Mass storage.
    USB MSC must has lower hardware to act as storage media.
    This program supports three kinds of storage media:

    1) RAM Disk : Use memory as storage
    2) Flash Disk: Use flash as storage,can be read/write
    3) CDROM: Use flash as storage, readonly CDROM subclass

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
//#include "diag.h"
//#include "utils.h"
//#include "bsp_hisr.h"
#include "mvUsbStorage.h"
#include "usb_device_types.h"
#include "mvUsbDevPrv.h"
#include "mvUsbDevApi.h"
#include "usb2_device.h"
#include "usb_init.h"
//#include "UART.h"

//#include "usbTrace.h"
//#include "usb_macro.h"
//#include "FlashManager.h"
//#include "nand_hal.h"
//#include "usb_init.h"
//#include "platform.h"
//#include "Flash_wk.h"

//add by yazhouren  >>>>>>>>>>>>>>
#if 1

Usb_DriverS         usbDrvCfg = {USB_ASR_MIFI_DRIVER, MASS_STORAGE_ENABLE, USB_AUTO_INSTALL_DISABLE, USB_OS_DETECT_DISABLE};
Usb_DriverS *GetUsbDriverConfig(void)
{
    return &usbDrvCfg;
}
#endif
extern Usb_DriverS *GetUsbDriverConfig(void);

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Flag to indicate illegal command. */
UINT8 IllegalCmd = 0;

/* The current LUN number*/
UINT8 CurrentLUN;

/* Mass Storage Class RX HISR */
OS_HISR _MSC_Rx_HISR;

/* Mass Storage Class TX HISR */
OS_HISR _MSC_Tx_HISR;

/* Mass Storage write information */
UINT32 MSC_In_Write_Status;
UINT32 MSC_In_Write_TAG;
UINT32 MSC_In_Write_Start_Block;
UINT32 MSC_In_Write_Total_Bytes;
UINT32 MSC_In_Write_Residue;

/* Mass Storage read information */
UINT32 MSC_In_Read_TAG;
UINT32 MSC_In_Read_Total_Bytes;
UINT32 MSC_In_Read_Residue;

/* Mass Storage command process state */
volatile BOOL MSC_In_Read = FALSE;
volatile BOOL MSC_In_Write = FALSE;
volatile BOOL CBW_PROCESSED = FALSE;

/*Mass storage receive_Buffer*/
MASS_STORAGE_BUFFER_STRUCT MSCRX_Buffer;

/*Mass storage message queue*/
OSAMsgQRef  FlashUSBMSC_MsgQ;

/*Mass storage timer reference*/
static OSATimerRef mvUsbMSCRef;

/*Mass storage task reference*/
static OSTaskRef _FlashUSBMSCTaskRef;

/* Mass storage task stack */
static mvUsbMscProperties_T MscProp;

/* Mass storage media type */
USBMSC_MEDIA mvUsbMscMedia[MSC_MAX_LUN];

/*MASS STORAGE SPECIFIC */
UINT8 SCSIDATA_Sense6[4]=
{
   0x03,                //Mode data length
   0x00,                //Medium type: must be 0 for SCSI block device
   0x00,                //Device-specific parameter: WP=0(No write protect),DPOFUA=0
   0x00                 //Block descriptor length
};

UINT8 SCSIDATA_Sense10[8]=
{
   0x00,                //MSB of Mode data length
   0x06,                //LSB of Mode data length
   0x00,                //Medium type: must be 0 for SCSI block device
   0x00,                //DEVICE-SPECIFIC PARAMETER
   0x00,                //Reserved
   0x00,                //Reserved
   0x00,                //MSB of Block descriptor length
   0x00                 //LSB of Block descriptor length
};

UINT8 SCSIDATA_Sense_IllegalCmd[]=
{
   0x70,                //Valid=0,respond code=0x70(Current errors)
   0x00,                //obsolete
   0x05,                //02:not ready, 05:illegal request,06:UNIT ATTENTION

   0x00,
   0x00,
   0x00,
   0x00,

   0x0A,                //Additional length

   /* command specifi information*/

   0x00,
   0x00,
   0x00,
   0x00,

   0x20,                //Additional Sense Code (ASC),                  {ASC,ASCQ}={0x20,0x00}:INVALID COMMAND OPERATION CODE
   0x00,                //Additional Sense Code Qualifier(ASCQ) ,    {ASC,ASCQ}={0x3a,0x00}:MEDIUM NOT PRESENT
                        //use along with 06:UNIT ATTENTION,   {ASC,ASCQ}={0x28,0x00}:NOT READY TO READY CHANGE, MEDIUM MAY HAVE CHANGED
   0x00,
   0x00,

   0x00,
   0x00
};

UINT8 SCSIDATA_RequestSense_NotReady[]=
{
   0x70,                //Valid=0,respond code=0x70(Current errors)
   0x00,                //obsolete
   0x02,                //02:not ready, 05:illegal request,06:UNIT ATTENTION

   0x00,
   0x00,
   0x00,
   0x00,

   0x0a,                //Additional length

   /* command specifi information*/

   0x00,
   0x00,
   0x00,
   0x00,

   0x3a,                //Additional Sense Code (ASC),                  {ASC,ASCQ}={0x20,0x00}:INVALID COMMAND OPERATION CODE
   0x00,                //Additional Sense Code Qualifier(ASCQ) ,    {ASC,ASCQ}={0x3a,0x00}:MEDIUM NOT PRESENT
                        //use along with 06:UNIT ATTENTION,   {ASC,ASCQ}={0x28,0x00}:NOT READY TO READY CHANGE, MEDIUM MAY HAVE CHANGED
   0x00,
   0x00,

   0x00,
   0x00
};

UINT8 SCSIDATA_RequestSense[]=
{
   0x70,                //Valid=0,respond code=0x70(Current errors)
   0x00,                //obsolete
   0x06,                //02:not ready, 05:illegal request,06:UNIT ATTENTION - Indicates the disc drive may have been reset.

   0x00,
   0x00,
   0x00,
   0x00,

   0x0a,                //Additional length

   /* command specifi information */

   0x00,
   0x00,
   0x00,
   0x00,

   0x28,                //Additional Sense Code (ASC),                  {ASC,ASCQ}={0x20,0x00}:INVALID COMMAND OPERATION CODE
   0x00,                //Additional Sense Code Qualifier(ASCQ) ,    {ASC,ASCQ}={0x3a,0x00}:MEDIUM NOT PRESENT
                        //use along with 06:UNIT ATTENTION,   {ASC,ASCQ}={0x28,0x00}:NOT READY TO READY CHANGE, MEDIUM MAY HAVE CHANGED
   0x00,
   0x00,

   0x00,
   0x00
};

MASS_STORAGE_DEVICE_INFO_STRUCT device_information_data = {
    0,                  //Direct access device
    0x80,               //removable
    0x02,               //ANSI-approved version 2: The device complies to SCSI-2
    0x02,               //SCSI INQUIRY data format : SCSI-2 standard
    0x1f,               //Additional length
    0,
    0,
    0,
   /* Vendor information: "DataCard" */
   {0x44, 0x61, 0x74, 0x61, 0x43, 0x61, 0x72, 0x64,},

   /* Product information: "USB Disk" */
   {0x55, 0x53, 0x42, 0x20, 0x44, 0x69, 0x73, 0x6B,
   0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20},

   /* Product Revision level: "1.00" */
   {0x31, 0x2e, 0x30, 0x30}
};

MASS_STORAGE_CDROM_TOC_STRUCT CDROM_TOC_20 = {
    USB_uint16_high8(0x12),
    USB_uint16_low8(0x12),
    1,                  //First Complete Session Number
    1,                  //Last Complete Session Number

    0,                  //reserve0
    0x14,               //ADR=0x1:Q Sub-channel encodes current position data;   Control=0x4:Data track, recorded uninterrupted
    1,                  //First Track Number in Last Complete Session
    0,                  //reserve1

    /* Logical Block Address of First Track in Last Session */

    0,                  //MSB
    0,
    2,
    0,                  //LSB

    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

 MASS_STORAGE_CDROM_TOC_STRUCT CDROM_TOC = {
    0x00,
    CDROM_TOC_LENGTH,   //number of bytes below
    0x01,               //First track number
    0x01,               //Last track number

    0x01,               //session number
    0x14,               //ADR/Control
    0x00,               //TNO
    0xa0,               //POINT
    0x00,               //Min
    0x00,               //Sec
    0x00,               //Frame
    0x00,               //Zero
    0x01,               //PMIN
    0x00,               //PSEC
    0x00,               //PFRAME

    0x01,
    0x14,
    0x00,
    0xa1,
    0x00,
    0x00,
    0x00,
    0x00,
    0x01,
    0x00,
    0x00,

    0x01,
    0x14,
    0x00,
    0xa2,
    0x00,
    0x00,
    0x00,
    0x00,
    0x0f,
    0x03,
    0x00
};

 MASS_STORAGE_CDROM_NOTIFICATION_STRUCT CDROM_NOTIFICATION = {

    /* "Event Header" section */
    USB_uint16_high8(CDROM_NOTIFICATION_LENGTH),
    USB_uint16_low8(CDROM_NOTIFICATION_LENGTH),
    0x80,               //NEA (No Event Available) is set to one, the Logical Unit supports none of the requested notification classes.
    0                   //Supported Event Classes

    /* No "Event Descriptor" section */
};

 UINT8 CDROM_REPORT_KEY[8]=
{
    0,                  //MSB of REPORT KEY Data Length
    6,                  //LSB of REPORT KEY Data Length
    0,                  //Reserved
    0,                  //Reserved

////RPC State
    0,                  //bit 7~6:Type Code , bit 5~3:# of Vendor Resets Available , bit 2:0# of User Controlled Changes Available
    0,                  //Region Mask
    0,                  //RPC Scheme
    0                   //Reserved
};

 UINT8 CDROM_READ_SUB_CHANNEL[4]=
{
    0,                  //Reserved
    0,                  //Audio Status  : Logical Units that do not support audio play operations shall always report 00h.
    0,                  //MSB of Sub-channel Data Length
    0,                  //LSB of Sub-channel Data Length

////NO Sub-channel Data
};

 UINT8 CDROM_GET_CONFIGURATION[8]=
{
////Feature Header
   0x00,                //MSB of Data Length
   0x00,                //
   0x00,                //
   0x00,                //LSB of Data Length
   0x00,                //Reserved
   0x00,                //Reserved
   0x00,                //MSB of Current Profile
   0x08                 //LSB of Current Profile    0008h: CD-ROM

////NO Feature Descriptor(s)
};

#pragma arm section rwdata="MscNoncache", zidata="MscNoncache"
/* CSW struct*/
static CSW_STRUCT csw;

/* CDROM Raw TOC*/
static UINT8 CDROM_RAW_TOC[40];

/* CDROM install flag*/
//UINT32 CDROM_INSTALL_LASTSECTOR_isRead = 0;

/* Mass storage received command*/
DIAG_ALIGN(8)  UINT8   MSC_rx_cmd[32]={0};

/* Mass storage capacity */
static UINT8 SCSIDATA_ReadFormatCapacities[12];

/* Mass storage capacity */
static MASS_STORAGE_READ_CAPACITY_STRUCT read_capacity;
#pragma arm section rwdata, zidata

#pragma arm section rwdata="Storage", zidata="Storage"
/* Mass storage receive data*/
DIAG_ALIGN(8)  UINT8  MSC_rx_data[MSC_RX_MAXSIZE]={0};

/* Mass storage transmit data*/
DIAG_ALIGN(8)  UINT8  MSC_tx_data[MSC_RX_MAXSIZE]={0};
#pragma arm section rwdata, zidata

/* Mass storage task stack */
static void* FlashUSBMSCTaskStack=NULL;

/*===========================================================================

            EXTERN DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* Usb device handle. */
extern USB_DEV_STATE_STRUCT_PTR _usbDeviceHandle;

/* CDROM device information. */
extern  MASS_STORAGE_DEVICE_INFO_STRUCT CDROM_device_information_data;

/* AZW CDROM device information. */
extern  MASS_STORAGE_DEVICE_INFO_STRUCT AZW_CDROM_device_information_data;

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION USB2ReEnumerate

DESCRIPTION
  Usb Re-enumerate.

DEPENDENCIES
  none

RETURN VALUE
  return staus

SIDE EFFECTS
  none

===========================================================================*/
//extern void USB2ReEnumerate(PlatformUsbDescType Desc);

/*===========================================================================

FUNCTION watchdog_reset

DESCRIPTION
  Watchdog reset.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void watchdog_reset(void);

/*===========================================================================

FUNCTION sdcard_get_blknum

DESCRIPTION
  Get the block number of SD card.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern UINT32 sdcard_get_blknum(void);

/*===========================================================================

FUNCTION sdcard_write_mass_storage

DESCRIPTION
  Write data of mass storage to SD card.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern UINT32 sdcard_write_mass_storage(UINT32 buffer, UINT32 loop, UINT32 blk_cnt);

/*===========================================================================

FUNCTION sdcard_read_mass_storage

DESCRIPTION
  Read data of mass storage from SD card.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern UINT32 sdcard_read_mass_storage(UINT32 buffer, UINT32 loop, UINT32 blk_cnt);

/*===========================================================================

                     INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION put_be32

DESCRIPTION
  None.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void put_be32(UINT8 *buf,UINT32 val)
{
    buf[0]=val>>24;
    buf[1]=val>>16;
    buf[2]=val>>8;
    buf[3]=val;
}

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

mvUsbMscProperties_T *GetMscProperties(void)
{
    return &MscProp;
}




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
void MSCRX_HISR(void)
{
    USBCDevice_ReturnCodeE usbStatus;
    OSA_STATUS          osaStatus;
    FlashMessage TxMsg;
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    if(MSC_In_Write)
    {
        if(pMscProp->Media[CurrentLUN] == USBMSC_CDROM)
        {
            csw.DCSWSIGNATURE = USB_DCSWSIGNATURE;
            csw.DCSWTAG = MSC_In_Write_TAG;
            csw.DCSWDATARESIDUE = MSC_In_Write_Residue;
            csw.BCSWSTATUS = MSC_In_Write_Status;

            /* Send the command status information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&csw,
                13,
                TRUE);
            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));

            CBW_PROCESSED = FALSE;
            MSC_In_Write = FALSE;
        }
        else
        {
            TxMsg.MsgId = FLASH_SECTOR_WRITE;
            TxMsg.ZoneId = (FLASH_ZoneID)CurrentLUN;

            /* Fill in the TX message */
            TxMsg.FlashAddress = MSC_In_Write_Start_Block;
            TxMsg.Length = MSC_In_Write_Total_Bytes;
            TxMsg.Buffer = (UINT32)(MSCRX_Buffer.buffer);

            osaStatus = OSAMsgQSend(FlashUSBMSC_MsgQ, FLASH_MESSAGE_SIZE, (UINT8*)&TxMsg, OSA_NO_SUSPEND);
            ASSERT(osaStatus == OS_SUCCESS);

            /* should release until message replied from flash driver to notify flash write done! */
            CBW_PROCESSED = FALSE;
            MSC_In_Write = TRUE;
        }

    }
    else
    {
        //always is bulk-out direction
        service_bulk_endpoint(0, MSCRX_Buffer.buffer, MSCRX_Buffer.length);
    }


    /* Flash USB disk need flash driver return read data/status */
    if((MSC_In_Write==FALSE)&&(MSC_In_Read==FALSE))
    {
        //call USBReadCompleted to queue another receive procedure
        //USBDeviceEndpointReceiveCompleted(MSCRX_Buffer.endpoint);
        mvUsbMscAddNextRxBuffer();
    }
}

void mvUsbMscAddNextRxBuffer(void)
{
    if(_usb_device_get_dtd_cnt(USB_MASS_STORAGE_RX_ENDPOINT, 0, NULL) != 00)
    {
        _usb_dci_vusb20_free_ep_transfer(USB_MASS_STORAGE_RX_ENDPOINT, 0, NULL);
    }

    USBDeviceEndpointReceiveCompleted(MSCRX_Buffer.endpoint);
}

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
void MSCRX_LISR(USBDevice_EndpointE endpoint, UINT8 *msgData, UINT32 length, BOOL endOfMsg)
{
    STATUS osStatus;

    MSCRX_Buffer.endpoint = endpoint;
    MSCRX_Buffer.buffer = msgData;
    MSCRX_Buffer.length = length;
    MSCRX_Buffer.endOfMsg = endOfMsg;

    osStatus = OS_Activate_HISR(&_MSC_Rx_HISR);
    ASSERT( osStatus == OS_SUCCESS );
}


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
void MSCTX_LISR(USBDevice_EndpointE endpoint, UINT8 *msgData, UINT32 length, BOOL endOfMsg)
{
    return;
}

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
void DataCard_Identify_FS(BOOL identify)
{
    UINT8 i = 0;
    /* No need to identify, Do nothing. */
    if (!identify)
    {
        return;
    }

    memset(mvUsbMscMedia, 0x00, sizeof(mvUsbMscMedia));

 }

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
static void proc_flash_msgs (UINT8*  rxmsg)
{
    unsigned char *BufferPtr = NULL;
    unsigned int FlashAddress = 0;
    FlashMessage  *FlashMsg = (FlashMessage *)rxmsg;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbMscProperties_T *pMscProp = GetMscProperties();
    USBCDevice_ReturnCodeE usbStatus;

    //FlashAddress is logical flash address for each flash zone
    FlashAddress = (unsigned int)FlashMsg->FlashAddress ;

    ASSERT(0 != FlashMsg->Buffer);

    switch(FlashMsg->MsgId)
    {
        case FLASH_SECTOR_WRITE:
        {
            BufferPtr = (unsigned char *)FlashMsg->Buffer;

            switch(pMscProp->Media[FlashMsg->ZoneId])
            {
                #if 0 //add by yazhouren
                case USBMSC_FLASHDISK:
                {
                    unsigned int Address = FlashAddress * NAND_SECTOR_SIZE;

                    if (!mvUsbStorageWrite(Address, FlashMsg->Length, BufferPtr, FlashMsg->ZoneId))
                    {
                        MSC_In_Write_Status = 1;
                        MSC_In_Write_Residue = MSC_In_Write_Total_Bytes;
                    }

                    break;
                }
                #endif
				case USBMSC_SDCARD:
				{
					sdcard_write_mass_storage((UINT32)BufferPtr, FlashAddress, FlashMsg->Length/NAND_SECTOR_SIZE);
					break;
				}

                default:
                {
                    ASSERT(pUsbDrvConfig->mass_storage == MASS_STORAGE_DISABLE);
                    break;
                }
            }

            csw.DCSWSIGNATURE = USB_DCSWSIGNATURE;
            csw.DCSWTAG = MSC_In_Write_TAG;
            csw.DCSWDATARESIDUE = MSC_In_Write_Residue;
            csw.BCSWSTATUS = MSC_In_Write_Status;

            /* Send the command status information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&csw,
                13,
                TRUE);
            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));

            CBW_PROCESSED = FALSE;
            MSC_In_Write = FALSE;

            //call USBReadCompleted to queue another receive procedure
            //USBDeviceEndpointReceiveCompleted(MSCRX_Buffer.endpoint);
            mvUsbMscAddNextRxBuffer();

            break;
        }

        case FLASH_SECTOR_READ:
        {
            //read all at one call, this can take advantage of DMA
            BufferPtr = (unsigned char *)FlashMsg->Buffer;

            switch(pMscProp->Media[FlashMsg->ZoneId])
            {
                #if 0 //add by yazhouren
                case USBMSC_CDROM:
                case USBMSC_FLASHDISK:
                {
                    unsigned int Address = FlashAddress * NAND_SECTOR_SIZE;

                    if (!mvUsbStorageRead(Address, FlashMsg->Length, BufferPtr, FlashMsg->ZoneId))
                    {
                        MSC_In_Read_Residue = MSC_In_Read_Total_Bytes;
                        memset(MSC_tx_data, 0x00, MSC_RX_MAXSIZE);
                    }
                    break;
                }
                #endif
				case USBMSC_SDCARD:
				{
					sdcard_read_mass_storage((UINT32)BufferPtr, FlashAddress, FlashMsg->Length/NAND_SECTOR_SIZE);
					break;
				}

                default:
                {
                    ASSERT(pUsbDrvConfig->mass_storage == MASS_STORAGE_DISABLE);
                    break;
                }
            }

            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                MSC_tx_data,
                MSC_In_Read_Total_Bytes,
                TRUE);

            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));

            /* Send the command status information */
            csw.DCSWSIGNATURE = USB_DCSWSIGNATURE;
            csw.DCSWTAG = MSC_In_Read_TAG;
            csw.DCSWDATARESIDUE = MSC_In_Read_Residue;

            if(MSC_In_Read_Residue != 0)
            {
                csw.BCSWSTATUS = 1;
            }
            else
            {
                csw.BCSWSTATUS = 0;
            }

            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&csw,
                13,
                TRUE);
			ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));

            CBW_PROCESSED = FALSE;
            MSC_In_Read = FALSE;

            //call USBReadCompleted to queue another receive procedure
            //USBDeviceEndpointReceiveCompleted(MSCRX_Buffer.endpoint);
            mvUsbMscAddNextRxBuffer();
            break;
        }

        default:
        {
            break;
        }
    }
}

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
static void FlashUSBMSC_Task (void* input)
{
    OSA_STATUS          osaStatus;
    UINT8     RecvMsg[FLASH_MESSAGE_SIZE];

    while(1)
    {
        osaStatus = OSAMsgQRecv(FlashUSBMSC_MsgQ, RecvMsg, FLASH_MESSAGE_SIZE, OSA_SUSPEND);
        ASSERT(osaStatus == OS_SUCCESS);

        proc_flash_msgs(RecvMsg);
    }
}

/*===========================================================================

FUNCTION mvUsbStorageTimer

DESCRIPTION
  This function process usb mass storage timer.

DEPENDENCIES
  none

RETURN VALUE
  FS type

SIDE EFFECTS
  none

===========================================================================*/
void mvUsbStorageTimer(UINT32 arg)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
	//uart_printf("mvUsbStorageTimer -------------------------------------------=-------\n");
	#if 0
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    /* Reset current usb descriptor */
    pUsbDesInfo->CurrentDesc = USB_MAX_DESCRIPTOR_TYPE;
    #endif
    /* Disable mass storage */
    //pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;

    /* Usb simulate plug out/in */
	#if 0 //add by yazhouren@asrmicro.com
    USB2MgrDeviceUnplugPlug();
	#endif
}

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
void mvUsbStorage_Init(void)
{
    USBCDevice_ReturnCodeE usbStatus;
    OSA_STATUS          osaStatus;
	//uart_printf("mvUsbStorage_Init");
    /* Message queue initialize. */
    osaStatus = OSAMsgQCreate(&FlashUSBMSC_MsgQ, "FlashUSBMSCMsgQ", FLASH_MESSAGE_SIZE, FlashUSBMSC_MESSAGE_Q_MAX, OSA_FIFO);
    ASSERT(osaStatus == OS_SUCCESS);

    FlashUSBMSCTaskStack = malloc(FlashUSBMSC_STACKSIZE);

    /* FlashUSBMSC task initialize. */
    osaStatus = OSATaskCreate
    	(&_FlashUSBMSCTaskRef,
    	FlashUSBMSCTaskStack,
    	FlashUSBMSC_STACKSIZE,
    	FlashUSBMSC_PRIORITY,
    	"FlashUSBMSC_Task",
    	FlashUSBMSC_Task ,
    	NULL);
    ASSERT(osaStatus == OS_SUCCESS);

    /*******************************************************
        In order to avoid frequentlly erase-write block,
        Flash driver has a buffer to store a FLASH block data,
        Here setup a timer to update flash periodlly
    ********************************************************/
    OSATimerCreate(&mvUsbMSCRef);

    OS_Create_HISR(&_MSC_Rx_HISR, "MSCRXHSR", MSCRX_HISR, HISR_PRIORITY_2);

    usbStatus = USBDeviceEndpointOpen(USB_MASS_STORAGE_TX_ENDPOINT,
    		USB_DEVICE_USAGE_INTERRUPT,
    		0,
    		(UINT8 *)NULL,
    		0,
    		MSCTX_LISR);
    ASSERT(usbStatus == USB_DEVICE_RC_OK);

    usbStatus = USBDeviceEndpointOpen(USB_MASS_STORAGE_RX_ENDPOINT,
    		USB_DEVICE_USAGE_INTERRUPT,
    		0,
    		(UINT8 *)NULL,
    		0,
    		MSCRX_LISR);
    ASSERT(usbStatus == USB_DEVICE_RC_OK);

    usbStatus = USBDeviceEndpointReceive(USB_MASS_STORAGE_RX_ENDPOINT,
        MSC_rx_cmd,
        32,
        TRUE);
    ASSERT(usbStatus == USB_DEVICE_RC_OK);

    /* Identify the FS type of Mass storage. */
    DataCard_Identify_FS(TRUE);
}

/***********************************************************************
* Function: mvUsbStorage_test_unit_ready_response                              *
************************************************************************
* Description: used as a call back function to notify                  *
*              that the channel is ready to use again                  *
* Parameters:                                                          *
*                                                                      *
* Return value: none                                                   *
*                                                                      *
* Notes:                                                               *
***********************************************************************/
void mvUsbStorage_test_unit_ready_response(void)
{
    USBCDevice_ReturnCodeE usbStatus;
    CBW_STRUCT_PTR cbw_ptr = NULL;

    cbw_ptr = (CBW_STRUCT_PTR)MSC_rx_cmd;

    csw.DCSWSIGNATURE = USB_DCSWSIGNATURE;
    csw.DCSWTAG = cbw_ptr->DCBWTAG;
    csw.DCSWDATARESIDUE = 0;
    csw.BCSWSTATUS = 0;

#if 0
    if(_usb_device_get_dtd_cnt(USB_MASS_STORAGE_TX_ENDPOINT, 1, NULL) >= 1)
    {
        _usb_dci_vusb20_free_ep_transfer(USB_MASS_STORAGE_TX_ENDPOINT, 1, NULL);
    }
#endif

    /* Send the command status information */
    usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
        (UINT8 *)&csw,
        13,
        TRUE);
    ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));

    //USBDeviceEndpointReceiveCompleted(MSCRX_Buffer.endpoint);
    mvUsbMscAddNextRxBuffer();
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_inquiry_command
* Returned Value : None
* Comments       :
*     Process a Mass storage class Inquiry command
*
*END*--------------------------------------------------------------------*/
void _process_inquiry_command
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    UINT32 length = 36;
    UINT8 *information = NULL;
    USBCDevice_ReturnCodeE usbStatus;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbMscProperties_T *pMscProp = GetMscProperties();
#if 0 //add by yazhouren	
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
#endif

    ASSERT((cbw_ptr->BCBWCBLUN & 0x0F)<= pMscProp->MscMaxLun);

    if (cbw_ptr->DCBWDATALENGTH)
    {
        /* The actual length will never exceed the DCBWDATALENGTH */
        if(cbw_ptr->DCBWDATALENGTH <= 36)
        {
            csw.DCSWDATARESIDUE = 0;
            length = cbw_ptr->DCBWDATALENGTH;
        }
        else
        {
            csw.DCSWDATARESIDUE = (cbw_ptr->DCBWDATALENGTH - 36);
            length = 36;
        }

        if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
        {
            switch(pMscProp->Media[(cbw_ptr->BCBWCBLUN & 0x0F)])
            {
             #if 0 //add by yazhouren
                case USBMSC_CDROM:
                {
                    if(pUsbDesInfo->DefaultDesc == USB_AZW_MIFI_DESCRIPTOR) //AZW
                    {
                        information = (UINT8 *)&AZW_CDROM_device_information_data;
                    }
                    else
                    {
                        information = (UINT8 *)&CDROM_device_information_data;
                    }

                    break;
                }

                case USBMSC_FLASHDISK:
                {
                    information = (UINT8 *)&device_information_data;
                    break;
                }
                #endif
				case USBMSC_SDCARD:
				{
					information = (UINT8 *)&device_information_data;
                    break;
				}

                default:
                {
                    ASSERT(pUsbDrvConfig->mass_storage == MASS_STORAGE_DISABLE);
                    break;
                }
            }

            /* Send the device information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
    		information,
    		length,
    		TRUE);

            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
        }
    }

    CBW_PROCESSED = FALSE;
    csw.BCSWSTATUS = 0;

    /* Send the command status information */
    usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
        (UINT8 *)&csw,
        13,
        TRUE);
    ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_unsupported_command
* Returned Value : None
* Comments       :
*     Responds appropriately to unsupported commands
*
*END*--------------------------------------------------------------------*/
void _process_unsupported_command
   (
        /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{

    USBCDevice_ReturnCodeE usbStatus;

    //If it is a Data-IN command, just send one byte in DATA phase.
    //Then send fail status in CSW phase
    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            (UINT8 *)&SCSIDATA_Sense10,
            1,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

    /* Send fail status in CSW phase */
    csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
    csw.BCSWSTATUS = 1;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_report_capacity
* Returned Value : None
* Comments       :
*     Reports the media capacity as a response to READ CAPACITY Command.
*
*END*--------------------------------------------------------------------*/
void _process_report_capacity
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    UINT32 BlkCnt = 0;
    UINT32 BlkSize = 0;
    USBCDevice_ReturnCodeE usbStatus;
    UINT8 *pCap = (UINT8 *)&read_capacity;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

	ASSERT((cbw_ptr->BCBWCBLUN & 0x0F)<= pMscProp->MscMaxLun);

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        switch(pMscProp->Media[(cbw_ptr->BCBWCBLUN & 0x0F)])
        {
        #if 0 //add by yazhouren
            case USBMSC_CDROM:
            {
                switch(cbw_ptr->BCBWCBLUN & 0x0F)
                {
                    case 0:
                    {
                        BlkCnt  = (pMscProp->Lun0EndAddress-pMscProp->Lun0StartAddress)/CDROM_BLOCK_SIZE;
                        BlkSize = CDROM_BLOCK_SIZE;
                        break;
                    }

                    case 1:
                    {
                        BlkCnt  = (pMscProp->Lun1EndAddress-pMscProp->Lun1StartAddress)/CDROM_BLOCK_SIZE;
                        BlkSize = CDROM_BLOCK_SIZE;
                        break;
                    }

                    case 2:
                    {
                        BlkCnt  = (pMscProp->Lun2EndAddress-pMscProp->Lun2StartAddress)/CDROM_BLOCK_SIZE;
                        BlkSize = CDROM_BLOCK_SIZE;
                        break;
                    }

                    default:
                    {
                        ASSERT(0);
                        break;
                    }
                }
                break;
            }

            case USBMSC_FLASHDISK:
            {
                switch(cbw_ptr->BCBWCBLUN & 0x0F)
                {
                    case 0:
                    {
                        BlkCnt  = (pMscProp->Lun0EndAddress-pMscProp->Lun0StartAddress)>>FLASH_SECTOR_BITSHIFT;
                        BlkSize = MSC_BLOCK_SIZE;
                        break;
                    }

                    case 1:
                    {
                        BlkCnt  = (pMscProp->Lun1EndAddress-pMscProp->Lun1StartAddress)>>FLASH_SECTOR_BITSHIFT;
                        BlkSize = MSC_BLOCK_SIZE;
                        break;
                    }

                    case 2:
                    {
                        BlkCnt  = (pMscProp->Lun2EndAddress-pMscProp->Lun2StartAddress)>>FLASH_SECTOR_BITSHIFT;
                        BlkSize = MSC_BLOCK_SIZE;
                        break;
                    }

                    default:
                    {
                        ASSERT(0);
                        break;
                    }
                }
                break;
            }
            #endif
            case USBMSC_SDCARD:
            {
				BlkCnt  = sdcard_get_blknum();
                BlkSize = MSC_BLOCK_SIZE;
				break;
            }

            default:
            {
                ASSERT(pUsbDrvConfig->mass_storage == MASS_STORAGE_DISABLE);
                break;
            }
        }

        pCap[0] = USB_uint32_byte3(BlkCnt-1);
        pCap[1] = USB_uint32_byte2(BlkCnt-1);
        pCap[2] = USB_uint32_byte1(BlkCnt-1);
        pCap[3] = USB_uint32_byte0(BlkCnt-1);
        pCap[4] = 0;
        pCap[5] = 0;
        pCap[6] = USB_uint16_high8(BlkSize);
        pCap[7] = USB_uint16_low8(BlkSize);

        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            pCap,
            8,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

   /* The actual length will never exceed the DCBWDATALENGTH */
    csw.DCSWDATARESIDUE = 0;
    csw.BCSWSTATUS = 0;

}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : store_cdrom_address
* Returned Value : None
* Comments       : read table of contents.
*     The READ TOC command is used by the Host Computer to identify a multi-session CD.
*
*END*--------------------------------------------------------------------*/

void store_cdrom_address(UINT8 *dest, int msf, UINT32 addr)
{
    /* Convert to Minutes-Seconds-Frames */
	if (msf)
	{
		/* Convert to 2048-byte frames */
		addr >>= 2;

		/* Lead-in occupies 2 seconds */
		addr += 2*75;

		/* Frames */
		dest[3] = addr % 75;
		addr /= 75;

		/* Seconds */
		dest[2] = addr % 60;
		addr /= 60;

		/* Minutes */
		dest[1] = addr;

		/* Reserved */
		dest[0] = 0;
	}
	else
	{
	    dest[3] = (addr >> 24) & 0x0F;

	    dest[2] = (addr >> 16) & 0x0F;

		dest[1] = (addr >> 8) & 0x0F;

		dest[0] = addr & 0x0F;
	}
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_read_TOC
* Returned Value : None
* Comments       : read table of contents.
*     The READ TOC command is used by the Host Computer to identify a multi-session CD.
*
*END*--------------------------------------------------------------------*/
void _process_read_TOC
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    UINT32 rc_length = 0;
    UINT32 BlkCnt = 0;
    UINT8 format = 0;
    USBCDevice_ReturnCodeE usbStatus;
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    /* Format type */
    format = cbw_ptr->CBWCB[2] & 0x0F;
    if(format == 0)
    {
        format = (cbw_ptr->CBWCB[9] >> 6) & 0x3;
    }

    /* host allocate length */
    rc_length = ((UINT32)cbw_ptr->CBWCB[7] << 8);
    rc_length |= (UINT32)cbw_ptr->CBWCB[8];

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        if(rc_length <= 20)
        {
            /* Send a zero-length packet */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&CDROM_TOC_20,
                rc_length,
                TRUE);
        }
        else
        {
            /* RAW TOC*/
            if(format == 2)
            {
                int i = 0;
                int len = 0;
                UINT8 msf = 0;
                UINT8 *buf = CDROM_RAW_TOC;

                switch(cbw_ptr->BCBWCBLUN & 0x0F)
                {
                    case 0:
                    {
                        BlkCnt  = (pMscProp->Lun0EndAddress-pMscProp->Lun0StartAddress)/CDROM_BLOCK_SIZE;
                        break;
                    }

                    case 1:
                    {
                        BlkCnt  = (pMscProp->Lun1EndAddress-pMscProp->Lun1StartAddress)/CDROM_BLOCK_SIZE;
                        break;
                    }

                    case 2:
                    {
                        BlkCnt  = (pMscProp->Lun2EndAddress-pMscProp->Lun2StartAddress)/CDROM_BLOCK_SIZE;
                        break;
                    }

                    default:
                    {
                        ASSERT(0);
                        break;
                    }
                }

                /* Modified for CDROM copy issue occur in MAC OS if ISO size is bigger than 1.5 MB */
                //BlkCnt = BlkCnt * 10;

                /* Calculate MSF */
                msf = cbw_ptr->CBWCB[1] & 0x02;

                /* 4 byte header + 3 descriptors */
                len = 4 + 3*11;

                /* Header + A0, A1 & A2 descriptors */
                memset(buf, 0, len);

                /* TOC Length excludes length field */
                buf[1] = len - 2;

                /* First complete session */
                buf[2] = 1;

                /* Last complete session */
                buf[3] = 1;

                /* fill in A0, A1 and A2 points */
                buf += 4;

                for (i = 0; i < 3; i++)
                {
                    /* Session number */
                    buf[0] = 1;

                    /* Data track, copying allowed */
                    buf[1] = 0x16;

                    /* 2 - Track number 0 ->  TOC */

                    /* A0, A1, A2 point */
                    buf[3] = 0xA0 + i;

                    /* 4, 5, 6 - Min, sec, frame is zero */

                    /* Pmin: last track number */
                    buf[8] = 1;

                    /* go to next track descriptor */
                    buf += 11;
                }

                /* go back to A2 descriptor */
                buf -= 11;

                /* For A2, 7, 8, 9, 10 - zero, Pmin, Psec, Pframe of Lead out */
                store_cdrom_address(&buf[7], msf, BlkCnt);

                /* Calculate the actual length. */
                if(rc_length < len)
                {
                    len = rc_length;
                }

                /* Send packet */
                usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                        (UINT8 *)CDROM_RAW_TOC,
                        len,
                        TRUE);

            }
            else
            {
                 /* Send a zero-length packet */
                usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                    (UINT8 *)&CDROM_TOC,
                    CDROM_TOC_LENGTH+2, //plus length itself
                    TRUE);
            }
        }

        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_get_NOTIFICATION
* Returned Value : None
* Comments       : GET EVENT STATUS NOTIFICATION
*     The command is used by the Host Computer to request USB CDROM to report events and statuses
*
*END*--------------------------------------------------------------------*/
void _process_get_NOTIFICATION
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        /* Send a zero-length packet */
        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            (UINT8 *)&CDROM_NOTIFICATION,
            4,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
    }

    /* The actual length will never exceed the DCBWDATALENGTH */
    csw.DCSWDATARESIDUE = 0;
    csw.BCSWSTATUS = 0;


}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_Report_KEY
* Returned Value : None
* Comments       : REPORT KEY
*     The REPORT KEY command requests the start of the authentication process and provides data
        necessary for authentication and for generating a Bus Key for the DVD Logical Unit.
*
*END*--------------------------------------------------------------------*/
void _process_Report_KEY
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        /* Send a zero-length packet */
        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            (UINT8 *)&CDROM_REPORT_KEY,
            8,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_START_STOP
* Returned Value : None
* Comments       : START_STOP
*     The START-STOP UNIT command instructs the UFI device to enable or disable
      media access operations.
*
*END*--------------------------------------------------------------------*/
void _process_START_STOP
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
#if 0 //add by yazhouren
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    if ((UINT32)cbw_ptr->CBWCB[4] & 0x02)
    {
        switch(pUsbDesInfo->DefaultDesc)
        {
            case USB_AZW_MIFI_DESCRIPTOR: //AZW
            {
                USB2ReEnumerate( USB_MAX_DESCRIPTOR_TYPE );
                break;
            }

            default:
            {
                if(pUsbDesInfo->CurrentDesc == USB_CDROM_ONLY_DESCRIPTOR)
                {
                    /* Reset the board */
                    watchdog_reset();
                }
                else
                {
                    OSATimerStop (mvUsbMSCRef);
                    OSATimerStart(mvUsbMSCRef, FlashUSBMSC_UPDATE_INTERVAL, 0, mvUsbStorageTimer, 0);
                }
                break;
            }
        }
    }
#else
                    OSATimerStop (mvUsbMSCRef);
                    OSATimerStart(mvUsbMSCRef, FlashUSBMSC_UPDATE_INTERVAL, 0, mvUsbStorageTimer, 0);

#endif

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_READ_SUB_CHANNEL
* Returned Value : None
* Comments       : READ SUB-CHANNEL
*     The READ SUB-CHANNEL command requests that the Logical Unit return the requested Subchannel
data.
*
*END*--------------------------------------------------------------------*/
void _process_READ_SUB_CHANNEL
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        /* Send a zero-length packet */
        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            (UINT8 *)&CDROM_READ_SUB_CHANNEL,
            4,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_MODE_SENSE10
* Returned Value : None
* Comments       : The MODE SENSE (10)
*     The MODE SENSE (10) command provides a means for the Initiator to specify medium, Logical Unit,
or peripheral device parameters.
*
*END*--------------------------------------------------------------------*/
void _process_MODE_SENSE10
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        /* Send a zero-length packet */
        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            (UINT8 *)&SCSIDATA_Sense10,
            8,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_get_CONFIGURATION
* Returned Value : None
* Comments       : GET CONFIGURATION
*     The GET CONFIGURATION command provides information about the Logical Unit capabilities . both
current and potential.
*
*END*--------------------------------------------------------------------*/
void _process_get_CONFIGURATION
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;
    UINT32 rc_length=0;

    //host allocate length
    rc_length = ((UINT32)cbw_ptr->CBWCB[7] << 8);
    rc_length |= (UINT32)cbw_ptr->CBWCB[8];


    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        /* Send a zero-length packet */
        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
            (UINT8 *)&CDROM_GET_CONFIGURATION,
            rc_length,
            TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
   }

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_Rezero_Unit
* Returned Value : None
* Comments       : REZERO UNIT
*     The REZERO UNIT command causes each of the medium transport axes of motion
to be driven to its respective home position. This sets the library to a defined state.
*
*END*--------------------------------------------------------------------*/
void _process_Rezero_Unit
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_read_command
* Returned Value : None
* Comments       :
*     Sends data as a response to READ Command.
*
*END*--------------------------------------------------------------------*/
void _process_read_command
   (
         /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    OSA_STATUS osaStatus;
    FlashMessage TxMsg;
    UINT32 index1 = 0, index2 = 0;
    USBCDevice_ReturnCodeE usbStatus;
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    ASSERT((cbw_ptr->BCBWCBLUN & 0x0F)<= pMscProp->MscMaxLun);

    if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
    {
        /* Send a zero-length packet */
        //modify by qigang
        index1  = ((UINT32)cbw_ptr->CBWCB[2] << 24);
        index1  |= ((UINT32)cbw_ptr->CBWCB[3] << 16);
        index1  |= ((UINT32)cbw_ptr->CBWCB[4] << 8);
        index1  |= cbw_ptr->CBWCB[5];

        index2 = ((UINT32)cbw_ptr->CBWCB[7] << 8);
        index2 |= (UINT32)cbw_ptr->CBWCB[8];

        index2 *= MSC_BLOCK_SIZE;

        ASSERT(cbw_ptr->DCBWDATALENGTH <= MSC_RX_MAXSIZE);

        /*******************************************************************
        Reset when datacard driver is installed:
        1) Already read the specific sector. (Host will read during install driver)
        2) It is dedicated for CDROM only.
        **
        ******************************************************************/

        if (cbw_ptr->DCBWDATALENGTH == 0)
        {
            csw.DCSWDATARESIDUE = 0;
            csw.BCSWSTATUS = 2;
            CBW_PROCESSED = FALSE;

            /* Send the command status information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                            (UINT8 *)&csw,
                            13,
                            TRUE);
            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
            return;
        }
        else
        {
            csw.DCSWDATARESIDUE = 0;
            csw.BCSWSTATUS = 0;

            //TBD : Read Flash
            MSC_In_Read = TRUE;
            CBW_PROCESSED = FALSE;
            MSC_In_Read_Total_Bytes = cbw_ptr->DCBWDATALENGTH;
            MSC_In_Read_TAG = cbw_ptr->DCBWTAG;

            TxMsg.MsgId = FLASH_SECTOR_READ;        //read sector
            TxMsg.ZoneId = (FLASH_ZoneID)(cbw_ptr->BCBWCBLUN & 0x0F);

            if(pMscProp->Media[(cbw_ptr->BCBWCBLUN & 0x0F)] == USBMSC_CDROM)
            {
                //CDROM has 2048B per sector, while flash has 512B
                if(MSC_In_Read_Total_Bytes == index2*4)
                {
                    TxMsg.FlashAddress = index1*4;                         //2048B sector
                    MSC_In_Read_Residue = 0;
                }
                else
                {
                    TxMsg.FlashAddress = index1;                            //512B sector

                    if(MSC_In_Read_Total_Bytes <= index2)
                    {
                        MSC_In_Read_Residue = 0;
                    }
                    else
                    {
                        MSC_In_Read_Residue = MSC_In_Read_Total_Bytes - index2;
                    }
                }
            }
            else
            {
                TxMsg.FlashAddress = index1;                                //sector index

                if(MSC_In_Read_Total_Bytes <= index2)
                {
                    MSC_In_Read_Residue = 0;
                }
                else
                {
                    MSC_In_Read_Residue = MSC_In_Read_Total_Bytes - index2;
                }
            }

            TxMsg.Length = MSC_In_Read_Total_Bytes;                         //Bytes
            TxMsg.Buffer = (UINT32)MSC_tx_data;

            osaStatus = OSAMsgQSend(FlashUSBMSC_MsgQ, FLASH_MESSAGE_SIZE, (UINT8*)&TxMsg, OSA_NO_SUSPEND);
            ASSERT(osaStatus == OS_SUCCESS);
        }
    }
    else
    {
        if(cbw_ptr->DCBWDATALENGTH != 0)
        {
            USB2DeviceEndpointStall(USB_MASS_STORAGE_RX_ENDPOINT);
        }

        csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
        csw.BCSWSTATUS = 2;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_write_command
* Returned Value : None
* Comments       :
*     Sends data as a response to WRITE Command.
*
*END*--------------------------------------------------------------------*/
void _process_write_command
   (
        /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;
    unsigned char	status;
    UINT32 index1 = 0, index2 = 0;
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    ASSERT((cbw_ptr->BCBWCBLUN & 0x0F)<= pMscProp->MscMaxLun);

    if (!(cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT))
    {
    	//modify by qigang
		index1  = ((UINT32)cbw_ptr->CBWCB[2] << 24);
		index1  |= ((UINT32)cbw_ptr->CBWCB[3] << 16);
        index1  |= ((UINT32)cbw_ptr->CBWCB[4] << 8);
        index1  |= cbw_ptr->CBWCB[5];

        index2 = ((UINT32)cbw_ptr->CBWCB[7] << 8);
        index2 |= (UINT32)cbw_ptr->CBWCB[8];

        ASSERT(cbw_ptr->DCBWDATALENGTH <= MSC_RX_MAXSIZE);

        if (cbw_ptr->DCBWDATALENGTH == 0)
        {
            /* Zero transfer length */
            csw.DCSWDATARESIDUE = 0;
            csw.BCSWSTATUS = 2;
            CBW_PROCESSED = FALSE;

            /* Send the command status information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                            (UINT8 *)&csw,
                            13,
                            TRUE);
            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
            return;
        }
        else
        {
            index2 *= MSC_BLOCK_SIZE;

            if (cbw_ptr->DCBWDATALENGTH == index2)
            {
                /* The actual length will never exceed the DCBWDATALENGTH */
                MSC_In_Write_Residue = 0;
                MSC_In_Write_Status = 0;
            }
            else if(cbw_ptr->DCBWDATALENGTH < index2)
            {
                MSC_In_Write_Residue = 0;
                MSC_In_Write_Status = 1;
            }
            else
            {
                MSC_In_Write_Residue = cbw_ptr->DCBWDATALENGTH - index2;
                MSC_In_Write_Status = 1;
            }

            if (_usb_device_get_transfer_status(_usbDeviceHandle,USB_MASS_STORAGE_RX_ENDPOINT, 0) != MV_USB_STATUS_IDLE)
            {
                _usb_device_cancel_transfer(_usbDeviceHandle,USB_MASS_STORAGE_RX_ENDPOINT, 0);
            }

            MSC_In_Write = TRUE;
            MSC_In_Write_Start_Block = index1;
            MSC_In_Write_Total_Bytes = cbw_ptr->DCBWDATALENGTH;
            MSC_In_Write_TAG = cbw_ptr->DCBWTAG;

            CurrentLUN = cbw_ptr->BCBWCBLUN & 0x0F;

            //receive to a local buffer
            status = _usb_device_recv_data(_usbDeviceHandle, USB_MASS_STORAGE_RX_ENDPOINT,
                            MSC_rx_data,
                            MSC_In_Write_Total_Bytes);
            ASSERT(status == USB_OK);
        }
    }
    else
    {
        /* Incorrect but valid CBW */
        csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
        csw.BCSWSTATUS = 2;

        usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                        (UINT8 *)&csw,
                        13,
                        TRUE);
        ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
        return;
    }
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_test_unit_ready
* Returned Value : None
* Comments       :
*     Responds appropriately to unit ready query
*
*END*--------------------------------------------------------------------*/
void _process_test_unit_ready
   (
      /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    if (cbw_ptr->DCBWDATALENGTH == 0)
    {
        /* The actual length will never exceed the DCBWDATALENGTH */
        csw.DCSWDATARESIDUE = 0;
        csw.BCSWSTATUS = 0;
    }
    else
    {
        if (!(cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT))
        {
            USB2DeviceEndpointStall(USB_MASS_STORAGE_RX_ENDPOINT);
        }

        /* Incorrect but valid CBW */
        csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
        csw.BCSWSTATUS = 1;
    }

   /***************************************************
   Return OK if:
   1) AT cmd server is ready, or
   2) CDROM only
   ****************************************************/
#if 0
    if((CDROM_INSTALL_LASTSECTOR_isRead==1)&&(current_usb_app_mask==USB_CDROM_ONLY_DESCRIPTOR))
    {
       OSATimerStart(mvUsbMSCRef, FlashUSBMSC_UPDATE_INTERVAL, 0, USB2Reconfig, 0);
    }
#endif

    csw.BCSWSTATUS = 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_prevent_allow_medium_removal
* Returned Value : None
* Comments       :
*     Responds appropriately to unit ready query
*
*END*--------------------------------------------------------------------*/
void _process_prevent_allow_medium_removal
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

    /* The actual length will never exceed the DCBWDATALENGTH */
    csw.DCSWDATARESIDUE = 0;
    csw.BCSWSTATUS = 0;

    CBW_PROCESSED = FALSE;

    /* Send the command status information */
    usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
        (UINT8 *)&csw,
        13,
        TRUE);
    ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_set_cd_speed
* Returned Value : None
* Comments       :
*     Responds appropriately to set CD Speed
*
*END*--------------------------------------------------------------------*/
void _process_set_cd_speed
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    IllegalCmd = 1;

    /* Send fail status in CSW phase */
    csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
    csw.BCSWSTATUS = 1;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_read_cd_command
* Returned Value : None
* Comments       :
*     Responds appropriately to read CD commands
*
*END*--------------------------------------------------------------------*/
void _process_read_cd_command
   (
        /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    IllegalCmd = 1;

    USB2DeviceEndpointStall(USB_MASS_STORAGE_TX_ENDPOINT);    

    /* Send fail status in CSW phase */
    csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
    csw.BCSWSTATUS = 1;
}


void _process_modesense6_command
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

    if (cbw_ptr->DCBWDATALENGTH)
    {
        if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
        {
         /* Send the device information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&SCSIDATA_Sense6,
                4,
                TRUE);
           ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
        }
    }

    /* The actual length will never exceed the DCBWDATALENGTH */
    csw.DCSWDATARESIDUE = 0;
    csw.BCSWSTATUS = 0;
}

void _process_requestsense_command
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    UINT32 length = 18;
    USBCDevice_ReturnCodeE usbStatus;

    if (cbw_ptr->DCBWDATALENGTH)
    {
        if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
        {
            /* Send the device information */
			/***************************************************
			Return not ready if:
			1) AT cmd server is not-ready, and
			2) Datacard scenario
			****************************************************/

			/* The actual length will never exceed the DCBWDATALENGTH */
            if(cbw_ptr->DCBWDATALENGTH <= 18)
            {
                csw.DCSWDATARESIDUE = 0;
                length = cbw_ptr->DCBWDATALENGTH;
                csw.BCSWSTATUS = 0;
            }
            else
            {
                csw.DCSWDATARESIDUE = (cbw_ptr->DCBWDATALENGTH - 18);
                length = 18;
                csw.BCSWSTATUS = 1;
            }
#if 0
			if((ATCmdSvrRdy==0)&&(current_usb_app_mask!=USB_CDROM_ONLY_DESCRIPTOR))
			{
	            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
	                (UINT8 *)&SCSIDATA_RequestSense_NotReady,
	                18,
	                TRUE);
			}
			else
#endif
			{
			    UINT8 *pRequestSense = NULL;

			    if(IllegalCmd)
			    {
                    pRequestSense = SCSIDATA_Sense_IllegalCmd;
                    IllegalCmd = 0;
			    }
			    else
			    {
                    pRequestSense = SCSIDATA_RequestSense;
			    }

	            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
	                (UINT8 *)pRequestSense,
	                length,
	                TRUE);
			}
           ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
        }
    }

    /* The actual length will never exceed the DCBWDATALENGTH */
    //csw.DCSWDATARESIDUE = 0;
}

void _process_verify_command
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    USBCDevice_ReturnCodeE usbStatus;

   /* The actual length will never exceed the DCBWDATALENGTH */
   csw.DCSWDATARESIDUE = 0;
   csw.BCSWSTATUS = 0;

   CBW_PROCESSED = FALSE;

   /* Send the command status information */
    usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
        (UINT8 *)&csw,
        13,
        TRUE);
    ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
}

void _process_read_format_capacity
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    UINT32 BlkCnt = 0;
    UINT32 BlkSize = 0;
    USBCDevice_ReturnCodeE usbStatus;
    UINT8 *ret_buff = SCSIDATA_ReadFormatCapacities;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbMscProperties_T *pMscProp = GetMscProperties();

    ASSERT((cbw_ptr->BCBWCBLUN & 0x0F)<= pMscProp->MscMaxLun);

    ret_buff[0] = ret_buff[1] = ret_buff[2] = 0;
    ret_buff[3] = 8;  //Capacity List Length
    ret_buff += 4;

	switch(pMscProp->Media[(cbw_ptr->BCBWCBLUN & 0x0F)])
    {
        case USBMSC_CDROM:
        {
            switch(cbw_ptr->BCBWCBLUN & 0x0F)
            {
                case 0:
                {
                    BlkCnt  = (pMscProp->Lun0EndAddress-pMscProp->Lun0StartAddress)/CDROM_BLOCK_SIZE;
                    BlkSize = CDROM_BLOCK_SIZE;
                    break;
                }

                case 1:
                {
                    BlkCnt  = (pMscProp->Lun1EndAddress-pMscProp->Lun1StartAddress)/CDROM_BLOCK_SIZE;
                    BlkSize = CDROM_BLOCK_SIZE;
                    break;
                }

                case 2:
                {
                    BlkCnt  = (pMscProp->Lun2EndAddress-pMscProp->Lun2StartAddress)/CDROM_BLOCK_SIZE;
                    BlkSize = CDROM_BLOCK_SIZE;
                    break;
                }

                default:
                {
                    ASSERT(0);
                    break;
                }
            }
            break;
        }

        case USBMSC_FLASHDISK:
        {
            switch(cbw_ptr->BCBWCBLUN & 0x0F)
            {
                case 0:
                {
                    BlkCnt  = (pMscProp->Lun0EndAddress-pMscProp->Lun0StartAddress)>>FLASH_SECTOR_BITSHIFT;
                    BlkSize = MSC_BLOCK_SIZE;
                    break;
                }

                case 1:
                {
                    BlkCnt  = (pMscProp->Lun1EndAddress-pMscProp->Lun1StartAddress)>>FLASH_SECTOR_BITSHIFT;
                    BlkSize = MSC_BLOCK_SIZE;
                    break;
                }

                case 2:
                {
                    BlkCnt  = (pMscProp->Lun2EndAddress-pMscProp->Lun2StartAddress)>>FLASH_SECTOR_BITSHIFT;
                    BlkSize = MSC_BLOCK_SIZE;
                    break;
                }

                default:
                {
                    ASSERT(0);
                    break;
                }
            }
            break;
        }

		case USBMSC_SDCARD:
		{
		    BlkCnt  = sdcard_get_blknum();
		    BlkSize = MSC_BLOCK_SIZE;
		    uart_printf("fm:%ld\r\n",BlkCnt);
			break;
		}

        default:
        {
            ASSERT(pUsbDrvConfig->mass_storage == MASS_STORAGE_DISABLE);
            break;
        }
    }

    put_be32(&ret_buff[0], BlkCnt);	  //number of blocks
	put_be32(&ret_buff[4], BlkSize);    //block length

    ret_buff[4]=0x02;       //current capacity

    if (cbw_ptr->DCBWDATALENGTH)
    {
        if (cbw_ptr->BMCBWFLAGS & USB_CBW_DIRECTION_BIT)
        {
             /* Send the device information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&SCSIDATA_ReadFormatCapacities,
                12,
                TRUE);
            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
        }
    }

    /* The actual length will never exceed the DCBWDATALENGTH */
    csw.DCSWDATARESIDUE = 0;
    csw.BCSWSTATUS = 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : _process_mass_storage_command
* Returned Value : None
* Comments       :
*     Process a Mass storage class command
*
*END*--------------------------------------------------------------------*/
void _process_mass_storage_command
   (
       /* [IN] Endpoint number */
      UINT8               ep_num,

      /* [IN] Pointer to the data buffer */
      CBW_STRUCT_PTR       cbw_ptr
   )
{
    UINT32 i;
    switch (cbw_ptr->CBWCB[0])
    {
        case 0x00:
        {
            /* Request the device to report if it is ready */
            _process_test_unit_ready( ep_num, cbw_ptr);
            break;
        }

        case 0x12:
        {
            /* Inquity command. Get device information */
            _process_inquiry_command( ep_num, cbw_ptr);
            break;
        }

        case 0x1A:
        {
            _process_modesense6_command( ep_num, cbw_ptr); //added by yqian
            /* Send the command status information */
            break;
        }

        case 0x03:
        {
            /* Transfer status sense data to the host */
            _process_requestsense_command( ep_num, cbw_ptr); //added by yqian
            break;
        }

        case 0x2F:
        {
            /* Verify data on the media */
            _process_verify_command( ep_num, cbw_ptr); //added by yqian
            break;
        }

        case 0x1E:
        {
            /* Prevent or allow the removal of media from a removable
            ** media device
            */
            _process_prevent_allow_medium_removal( ep_num, cbw_ptr);
            break;
        }

        case 0x23:
        {
            /* Read Format Capacities. Report current media capacity and
            ** formattable capacities supported by media */
            _process_read_format_capacity( ep_num, cbw_ptr); //added by yqian
            break;
        }

        case 0x25:
        {
            /* Report current media capacity */
            _process_report_capacity( ep_num, cbw_ptr);
            break;
        }

        case 0x28:
        case 0xA8:
        {
            /* Read (10) Transfer binary data from media to the host */
            _process_read_command( ep_num, cbw_ptr);
            break;
        }

        case 0x2A:
        case 0xAA:
        {
            /* Write (10) Transfer binary data from the host to the
            ** media
            */
            _process_write_command( ep_num, cbw_ptr);
            break;
        }

/***********************CDROM related: Start***************************/

/************************************************************
    01h/11h should not applied for CD-ROM and
    DIRECT ACCESS BLOCK DEVICE
    But in some very very seldom cases they will appear.

    55h applid for  CD-ROM,and Windows does not send it
*************************************************************/
#if 0
        case 0x01:
        {
            /* Position a head of the drive to zero track */
            MSG_USB_LOG("Rezero Unit");

            //Just send OK in status phase??
            _process_Rezero_Unit( ep_num, cbw_ptr);
            break;
        }

        case 0x11:
        case 0x55: /* MODE SELECT(10) */
#endif

        case 0x42:
        {
            /* READ_SUB_CHANNEL */
            _process_READ_SUB_CHANNEL( ep_num, cbw_ptr);
            break;
        }

        case 0x43:
        {
            /* READ TOC */
            //MIFI_LOG_TRACE(MIFI, USB_STORAGE, READ_TOC, "Usb storage read TOC message");
            _process_read_TOC( ep_num, cbw_ptr);
            break;
        }

        case 0x46:
        {
            /* GET CONFIGURATION */

            //MIFI_LOG_TRACE(MIFI, USB_STORAGE, GET_CONFIGURATION, "Usb storage get CONFIGURATION message");
            _process_get_CONFIGURATION( ep_num, cbw_ptr);
            break;
        }

        case 0x4a:
        {
            /* GET EVENT STATUS NOTIFICATION */
            //MIFI_LOG_TRACE(MIFI, USB_STORAGE, GET_NOTIFICATION, "Usb storage get NOTIFICATION message");
            _process_get_NOTIFICATION( ep_num, cbw_ptr);
            break;
        }

        case 0x5a:
        {
            /* MODE SENSE (10) */
            _process_MODE_SENSE10( ep_num, cbw_ptr);
            break;
        }

        case 0xa4:
        {
            /* Report_KEY */
            _process_Report_KEY( ep_num, cbw_ptr);
            break;
        }

        case 0xBB:
        {
            /* Set cd speed */
            _process_set_cd_speed( ep_num, cbw_ptr);
            break;
        }

        case 0xBE:
        {
            /* Read CD */
            _process_read_cd_command( ep_num, cbw_ptr);
            break;
        }

        case 0x1B:
        {
            /* Request a request a removable-media device to load or
            ** unload its media
            */
            //MIFI_LOG_TRACE(MIFI, USB_STORAGE, START_STOP, "Usb storage START STOP message");
            _process_START_STOP( ep_num, cbw_ptr);
            break;
        }

/***********************CDROM related: End***************************/

        case 0x15: /* mode select */

        case 0x04: /* Format unformatted media */
        case 0x1D: /* Perform a hard reset and execute diagnostics */
        case 0x2B: /* Seek the device to a specified address */
        case 0x2E: /* Transfer binary data from the host to the media and
                   ** verify data
                   */

#if 0
        case 0xA8: /* Read (12) Transfer binary data from the media to the host */
        case 0xAA: /* Write (12) Transfer binary data from the host to the
                   ** media
                   */
#endif
        default:
        {
            //MIFI_LOG_TRACE("Unknown USBMSC CMD=0x%lx,DATALENGTH=0x%lx,FLAGS=0x%x",cbw_ptr->CBWCB[0], cbw_ptr->DCBWDATALENGTH, cbw_ptr->BMCBWFLAGS);

            for(i=0;i<cbw_ptr->BCBWCBLENGTH;i++)
            {
                //MSG_USB_LOG("[%d]=0x%x", i, cbw_ptr->CBWCB[i]);
            }

            _process_unsupported_command( ep_num, cbw_ptr);
            break;
        }
   }

}

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
void service_bulk_endpoint
(
      /* [IN] Direction of the transfer.  Is it transmit? */
      UINT8               direction,

      /* [IN] Pointer to the data buffer */
      UINT8*           buffer,

      /* [IN] Length of the transfer */
      UINT32              length
)
{
    USBCDevice_ReturnCodeE usbStatus;
    CBW_STRUCT_PTR cbw_ptr = NULL;

    ASSERT(buffer != NULL);

    if ((!direction) && (!CBW_PROCESSED))
    {
        cbw_ptr = (CBW_STRUCT_PTR)((void *)buffer);
        ASSERT(cbw_ptr != NULL);

        if((cbw_ptr->DCBWSIGNATURE == USB_DCBWSIGNATURE)&&(length == 31))
        {
            /* A valid CBW was received */
            csw.DCSWSIGNATURE = USB_DCSWSIGNATURE;
            csw.DCSWTAG = cbw_ptr->DCBWTAG;
            CBW_PROCESSED = TRUE;

            /* Process the command */
            _process_mass_storage_command( USB_MASS_STORAGE_RX_ENDPOINT, cbw_ptr);    //USB_MASS_STORAGE_RX_ENDPOINT actually is not used

        }
        else
        {
            /* A invalid CBW was received */
            csw.DCSWSIGNATURE = USB_DCSWSIGNATURE;
            csw.DCSWTAG = cbw_ptr->DCBWTAG;
            csw.DCSWDATARESIDUE = cbw_ptr->DCBWDATALENGTH;
            csw.BCSWSTATUS = 1;
            CBW_PROCESSED = TRUE;
        }
    }
    else
    {
        //ErrorLogPrintf("direction %d, CBW_PROCESSED %d", direction, CBW_PROCESSED);
    }

    if((MSC_In_Write == FALSE)&&(MSC_In_Read == FALSE))
    {
        /* If a CBW was processed then send the status information and
        ** queue another cbw receive request, else just queue another CBW receive
        ** request if we received an invalid CBW
        */
        if (CBW_PROCESSED)
        {
            CBW_PROCESSED = FALSE;
            /* Send the command status information */
            usbStatus = USBDeviceEndpointTransmit(USB_MASS_STORAGE_TX_ENDPOINT,
                (UINT8 *)&csw,
                13,
                TRUE);
            ASSERT((usbStatus == USB_DEVICE_RC_OK) || (usbStatus == USB_DEVICE_RC_NOT_CONNECTED));
        }
    }


    return;
}
#endif /* MV_USB2_MASS_STORAGE */
