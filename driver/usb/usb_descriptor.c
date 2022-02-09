/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code ("Material") are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/

/********************************************************************
*                      M O D U L E     B O D Y                       *
**********************************************************************
* Title: Universal Serial Bus Manager                                *
*                                                                    *
* Filename: USBMgr.c                                                 *
*                                                                    *
*                                                                    *
*                                                                    *
* Author:                                                            *
*                                                                    *
* Description: This is the main file to handle the                   *
*              the USB Manager Package                               *
*                                                                    *
* Notes: This file is using functions from usbmgr.c file             *
**********************************************************************/


#include <string.h>
//#include "utils.h"
#include "usb_device.h"
#include "usb_def.h"
#include "usbTrace.h"
#include "mvUsbDevPrv.h"
#include "udc_hw.h"
//#include "osa.h"
#include "usb_macro.h"
//#include "diag_nvm.h"
#include "usb_init.h"
//#include "ECM.h"

#ifdef MV_USB2_MASS_STORAGE
#include "mvUsbStorage.h"
#endif

#include "usb2_device.h"
#include "mvUsbModem.h"
#include "usb_descriptor.h"

/*===========================================================================

            LOCAL MACRO
===========================================================================*/

/**********************************************************************/
/*-- Local variables declarations --------------------------------------*/

/* The USB desriptor information */
mvUsbDescriptorInfo UsbDescriptorInfo;

extern void craneCommPMPowerupUsbPhy(void);

/*******************************************************************************
 *                      S T A T I C  F U N C T I O N S
******************************************************************************/
//!!!!!! NOTE THIS IS ALL TEMPORARY IT THIS CODE WILL BE DELETED WHEN DYNAMIC DESCRIPTOR BUILDING WILL BE SUPPPORTED !!!!!
// note this is temp function USB descriptor must be created during applications registration
//and not set hard code , but at first stage it is not supported and USBmgr works only with
//listed below predefined applications
//each bit indicates which application to set
//bit 0 - ICAT ,
//bit 1 - Modem
//bit 2 - Genie
//bit 3 - MAST
//bit 4 - Modem single interface test , debug only
//!!! note that not all possible combinations are supported only a few selected combinations:

UINT16 PacketSizeOffsetNum = 0;
UINT16 PacketSizeOffset[20];

#pragma arm section rwdata="UsbDesc", zidata="UsbDesc"
static __align(8) UINT8 devDesc[18];
static __align(8) UINT8 configDesc[512]; // setting maximum
static __align(8) UINT8 qualifDesc[10]; // not relevant to USB1 (USB2 only)
static __align(8) UINT8 otherSpeedDesc[512]; // // setting maximum (USB2 only)

//Manufacturer String descriptor
//static __align(8) UINT8 strDescLang[16]=
//{0x03,0x03,0x09,0x04};//supporting 0x0409 English (United States)

//Manufacturer String descriptor
static __align(8) UINT8 strDescManufacturer[16]=
{0x10,0x03,0x4D,0x00,0x61,0x00,0x72,0x00,0x76,0x00,0x65,0x00,0x6C,0x00,0x6C,0x00};//Marvell

static __align(8) UINT8 strDescASRManufacturer[18]=
{0x12,0x03,0x41,0x00,0x53,0x00,0x52,0x00,0x4d,0x00,0x69,0x00,0x63,0x00,0x72,0x00,0x6f,0x00};//ASRMicro


//Product String Descriptor
static __align(8) UINT8 strDescProduct[36]=
{0x24,0x03,0x4D,0x00,0x6F,0x00,0x62,0x00,0x69,0x00,0x6C,0x00,0x65,0x00,0x20,0x00,//"Mobile "
 0x55,0x00,0x53,0x00,0x42,0x00,0x20,0x00, //"USB "
 0x44,0x00,0x65,0x00,0x76,0x00,0x69,0x00,0x63,0x00,0x65,0x00}; //"Device"

//Serial number String Descriptor
static __align(8) UINT8 strDescSerialNumber[32]=
{0x20, 0x03, 0x5B, 0x00, 0x4d, 0x00, 0x61, 0x00, 0x72, 0x00, 0x76, 0x00, 0x65, 0x00, 0x6C, 0x00,0x20, 0x00, //"[Serial "
 0x4E, 0x00, 0x75, 0x00, 0x6D, 0x00, 0x62, 0x00, 0x65, 0x00, 0x72, 0x00, 0x5D, 0x00}; //"Number]"

//Marvel: 		0x4d 0x61 0x72 0x76 0x65 0x6c
//HUAWEI: 	0x48 0x55 0x41 0x57 0x45 0x49

/* USB Mass Storage Device*/
static __align(8) UINT8 strMobileMass[0x30]=
{
0x30, 0x03, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, \
0x20, 0x00, 0x4D, 0x00, 0x61, 0x00, 0x73, 0x00, \
0x73, 0x00, 0x20, 0x00, 0x53, 0x00, 0x74, 0x00, \
0x6F, 0x00, 0x72, 0x00, 0x61, 0x00, 0x67, 0x00, \
0x65, 0x00, 0x20, 0x00, 0x44, 0x00, 0x65, 0x00, \
0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00
};

/* Mobile Composite Device Bus */
static __align(8) UINT8 strMobileDevice[0x38]=
{
0x38, 0x03, 0x4D, 0x00, 0x6F, 0x00, 0x62, 0x00, \
0x69, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20, 0x00, \
0x43, 0x00, 0x6F, 0x00, 0x6D, 0x00, 0x70, 0x00, \
0x6F, 0x00, 0x73, 0x00, 0x69, 0x00, 0x74, 0x00, \
0x65, 0x00, 0x20, 0x00, 0x44, 0x00, 0x65, 0x00, \
0x76, 0x00, 0x69, 0x00, 0x63, 0x00, 0x65, 0x00, \
0x20, 0x00, 0x42, 0x00, 0x75, 0x00, 0x73, 0x00
};

/* Mobile Serial number */
static __align(8) UINT8 strMobileNumber[0x26]=
{
0x26 ,0x03 ,0x32 ,0x00 ,0x30 ,0x00 ,0x30 ,0x00,
0x38 ,0x00 ,0x30 ,0x00 ,0x36 ,0x00 ,0x30 ,0x00,
0x30 ,0x00 ,0x36 ,0x00 ,0x38 ,0x00 ,0x30 ,0x00,
0x39 ,0x00 ,0x30 ,0x00 ,0x38 ,0x00 ,0x30 ,0x00,
0x30 ,0x00 ,0x30 ,0x00 ,0x30 ,0x00
};

/* Mobile AT Interface */
static __align(8) UINT8 strMobileAT[0x28]=
{
0x28, 0x03, 0x4D, 0x00, 0x6F, 0x00, 0x62, 0x00, \
0x69, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20, 0x00, \
0x41, 0x00, 0x54, 0x00, 0x20, 0x00, 0x49, 0x00, \
0x6E, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00, \
0x66, 0x00, 0x61, 0x00, 0x63, 0x00, 0x65, 0x00
};

/* Mobile Diag Interface */
static __align(8) UINT8 strMobileDiag[0x2C]=
{
0x2C, 0x03, 0x4D, 0x00, 0x6F, 0x00, 0x62, 0x00, \
0x69, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20, 0x00, \
0x44, 0x00, 0x69, 0x00, 0x61, 0x00, 0x67, 0x00, \
0x20, 0x00, 0x49, 0x00, 0x6E, 0x00, 0x74, 0x00, \
0x65, 0x00, 0x72, 0x00, 0x66, 0x00, 0x61, 0x00, \
0x63, 0x00, 0x65, 0x00
};

/* Mobile Sulog Interface */
static __align(8) UINT8 strMobileSulog[0x2E]=
{
0x2E, 0x03, 0x4D, 0x00, 0x6F, 0x00, 0x62, 0x00, \
0x69, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20, 0x00, \
0x53, 0x00, 0x75, 0x00, 0x6C, 0x00, 0x6F, 0x00, \
0x67, 0x00, 0x20, 0x00, 0x49, 0x00, 0x6E, 0x00, \
0x74, 0x00, 0x65, 0x00, 0x72, 0x00, 0x66, 0x00, \
0x61, 0x00, 0x63, 0x00, 0x65, 0x00
};


/* Mobile RNDIS Network Adapter */
static __align(8) UINT8 strMobileRNDIS[0x3A]=
{
0x3A, 0x03, 0x4D, 0x00, 0x6F, 0x00, 0x62, 0x00, \
0x69, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20, 0x00, \
0x52, 0x00, 0x4E, 0x00, 0x44, 0x00, 0x49, 0x00, \
0x53, 0x00, 0x20, 0x00, 0x4E, 0x00, 0x65, 0x00, \
0x74, 0x00, 0x77, 0x00, 0x6F, 0x00, 0x72, 0x00, \
0x6B, 0x00, 0x20, 0x00, 0x41, 0x00, 0x64, 0x00, \
0x61, 0x00, 0x70, 0x00, 0x74, 0x00, 0x65, 0x00, \
0x72, 0x00
};

/* Mobile MBIM Network Adapter */
static __align(8) UINT8 strMobileMBIM[0x38]=
{
0x38, 0x03, 0x4d, 0x00, 0x6f, 0x00, 0x62, 0x00, 0x69, 0x00, 0x6c, 0x00, 0x65, 0x00, 0x20, 0x00,
0x4d, 0x00, 0x42, 0x00, 0x49, 0x00, 0x4d, 0x00, 0x20, 0x00, 0x4e, 0x00, 0x65, 0x00, 0x74, 0x00,
0x77, 0x00, 0x6f, 0x00, 0x72, 0x00, 0x6b, 0x00, 0x20, 0x00, 0x41, 0x00, 0x64, 0x00, 0x61, 0x00,
0x70, 0x00, 0x74, 0x00, 0x65, 0x00, 0x72, 0x00
};


/* Mobile ECM Network Adapter.                 */
static __align(8) UINT8 strMobileECM[] =
{
    0x36, 0x03, 0x4D, 0x00, 0x6F, 0x00, 0x62, 0x00,\
    0x69, 0x00, 0x6C, 0x00, 0x65, 0x00, 0x20, 0x00,\
    0x45, 0x00, 0x43, 0x00, 0x4D, 0x00, 0x20, 0x00,\
    0x4E, 0x00, 0x65, 0x00, 0x74, 0x00, 0x77, 0x00,\
    0x6F, 0x00, 0x72, 0x00, 0x6B, 0x00, 0x20, 0x00,\
    0x41, 0x00, 0x64, 0x00, 0x61, 0x00, 0x70, 0x00,\
    0x74, 0x00, 0x65, 0x00, 0x72, 0x00
};

static __align(8) UINT8 strECMMACADDR[] =
{
    0x1A, 0x03, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00,\
    0x43, 0x00, 0x32, 0x00, 0x39, 0x00, 0x41, 0x00,\
    0x33, 0x00, 0x39, 0x00, 0x42, 0x00, 0x36, 0x00,\
    0x44, 0x00
};
#pragma arm section rwdata, zidata


BOOL massStorageEnabled(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    if(pUsbDrvConfig->mass_storage == MASS_STORAGE_ENABLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
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
void USB2MgrUpdateDescriptor(PlatformUsbDescType desc, char mode)
{
    BOOL storage = FALSE;
    UINT16 devDesc_num = 0, configDesc_num = 0;
    UINT16 dev_desc_length = 0, config_desc_length = 0;
    UINT16 qualif_desc_length = 0, other_speed_desc_length = 0;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    uart_printf("%s-01\r\n",__func__);
    if(pUsbDesInfo->CurrentDesc != desc)
    {
        pUsbDesInfo->CurrentDesc = desc;
    }
    else
    {
        return;
    }

    /* Mass storage parameters configure. */
	if ((desc == USB_CDROM_ONLY_DESCRIPTOR)||
        (pUsbDrvConfig->mass_storage == MASS_STORAGE_ENABLE))
    {
#ifdef MV_USB2_MASS_STORAGE
        mvUsbStorageConfigure();
#endif
    }

    PacketSizeOffsetNum = 0;

    /* Get Mass storage enable flag. */
    storage = pUsbDrvConfig->mass_storage;

    switch(desc)
    {

        case USB_GENERIC_MIFI_DESCRIPTOR: //Generic RNDIS driver
        {
            devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength				- Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType		- Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0xef; //  bDeviceClass		- class of the device
            devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass		- subclass of the device
            devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol		- protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0		- Max Packet Size for EP zero
            devDesc[devDesc_num++] = 0xCC; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x2E; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x10; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x30; //  idProduct (MSB)
            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)		- The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct			- Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber		- Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations	- Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB)	- Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++]  = 0x07; // bNumInterfaces		- Number of interfaces this configuration supports
            }
            else
            {
                configDesc[configDesc_num++]  = 0x06; // bNumInterfaces		- Number of interfaces this configuration supports
            }
#else
            configDesc[configDesc_num++]  = 0x06; // bNumInterfaces		- Number of interfaces this configuration supports
#endif
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
            configDesc[configDesc_num++]  = 0xc0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
            configDesc[configDesc_num++]  = 0xfa; // MaxPower				- Maximum power consumption for this configuration (mA)

            /* otg_descriptor*/

            configDesc[configDesc_num++] = 0x03; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x09; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x03; //bInterfaceNumber


            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x00; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0xe0; // bFunctionClass
            configDesc[configDesc_num++] = 0x01; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x03; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x05; // Index of string descriptor describing this function

            /**********************************************************/

            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x00; //bInterfaceNumber     - for RNDIS
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x01; // bNumEnd
            configDesc[configDesc_num++] = 0xe0; //bInterfaceClass
            configDesc[configDesc_num++] = 0x01; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x03; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x01; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bControlInterface
            configDesc[configDesc_num++] = 0x01; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetCtrlEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x01; //bInterfaceNumber     --- for RNDIS
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // NumEndpoints
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified
            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetTxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetRxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /* MV_USB_RNDIS end */

            /* INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x04; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0x02; // bFunctionClass
            configDesc[configDesc_num++] = 0x02; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x01; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x08; // Index of string descriptor describing this function



            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x04; // bInterfaceNumber       ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x01; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface			- Index of string descriptor describing this Interface


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x03; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bControlInterface
            configDesc[configDesc_num++] = 0x03; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagCtrlEP; //bendpointAddress for diag contrl
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x05; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0x0a; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface			- Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress		- Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

            /* INTERFACE ASSOCIATION DESCRIPTOR */

            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0x02; // bFunctionClass
            configDesc[configDesc_num++] = 0x02; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x01; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x0B; // Index of string descriptor describing this function



            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x02; // bInterfaceNumber      - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x01; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x0B; // iInterface			- Index of string descriptor describing this Interface


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x05; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x04; // bControlInterface
            configDesc[configDesc_num++] = 0x05; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = MdmCtrlEP; //bendpointAddress for modem contrl endpoint
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */


            configDesc[configDesc_num++] = 0x09;  // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x03;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; // bEndpointAddress		- Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress		- Endpoint Address & Direction for modem rx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

#ifdef MV_USB2_MASS_STORAGE
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */

            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x06;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif

            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;
        }

        // MARVELL Rndis driver
        case USB_MARVELL_MIFI_DESCRIPTOR:
        {
            devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength              - Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType      - Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0xef; //  bDeviceClass     - class of the device
            devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass      - subclass of the device
            devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol      - protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0      - Max Packet Size for EP zero
            devDesc[devDesc_num++] = 0x86; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x12; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x31; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x4E; //  idProduct (MSB)
            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)      - The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer        - Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct         - Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber        - Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations   - Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength                - Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB) - Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration
#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++]  = 0x05; // bNumInterfaces     - Number of interfaces this configuration supports
            }
            else
            {
                configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
            }
#else
            configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration     - Index of string descriptor describing this configuration
            configDesc[configDesc_num++]  = 0xc0; // bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup
            configDesc[configDesc_num++]  = 0xfa; // MaxPower               - Maximum power consumption for this configuration (mA)

            /* otg_descriptor*/

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x00; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0xe0; // bFunctionClass 0xef
            configDesc[configDesc_num++] = 0x01; // bFunctionSubClass 0x04
            configDesc[configDesc_num++] = 0x03; // bFunctionProtocol 0x01
            configDesc[configDesc_num++] = 0x05; // Index of string descriptor describing this function

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x00; //bInterfaceNumber     - for RNDIS
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x01; // bNumEnd
            configDesc[configDesc_num++] = 0xe0; //bInterfaceClass
            configDesc[configDesc_num++] = 0x01; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x03; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x01; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bControlInterface
            configDesc[configDesc_num++] = 0x01; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetCtrlEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x01; // bInterfaceNumber     --- for RNDIS
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // NumEndpoints
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified
            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetTxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetRxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval
            /* MV_USB_RNDIS end */


            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x02; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /* INTERFACE ASSOCIATION DESCRIPTOR */
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */


            configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++] = 0x04;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0xff; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

#ifdef MV_USB2_MASS_STORAGE
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */

            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x05;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif
            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;
        }


        // ASR MIFI driver
        case USB_ASR_MIFI_DESCRIPTOR:
        {
            devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength              - Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType      - Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0xef; //  bDeviceClass     - class of the device
            devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass      - subclass of the device
            devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol      - protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0      - Max Packet Size for EP zero
            devDesc[devDesc_num++] = 0xCC; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x2E; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x10; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x30; //  idProduct (MSB)
            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)      - The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer        - Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct         - Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber        - Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations   - Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength                - Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB) - Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration
#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
#ifdef PLAT_USE_ALIOS
                configDesc[configDesc_num++]  = 0x07; // bNumInterfaces     - Number of interfaces this configuration supports
#else
				configDesc[configDesc_num++]  = 0x07; // bNumInterfaces 	- Number of interfaces this configuration supports
#endif
            }
            else
            {
#ifdef PLAT_USE_ALIOS
                configDesc[configDesc_num++]  = 0x06; // bNumInterfaces     - Number of interfaces this configuration supports
#else
				configDesc[configDesc_num++]  = 0x06; // bNumInterfaces 	- Number of interfaces this configuration supports
#endif
            }
#else
#ifdef PLAT_USE_ALIOS
            configDesc[configDesc_num++]  = 0x06; // bNumInterfaces     - Number of interfaces this configuration supports
#else
			configDesc[configDesc_num++]  = 0x06; // bNumInterfaces 	- Number of interfaces this configuration supports
#endif
#endif
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration     - Index of string descriptor describing this configuration
#ifdef USB_REMOTEWAKEUP
            configDesc[configDesc_num++]  = 0xe0; // bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup
#else
			configDesc[configDesc_num++]  = 0xc0; // bmAttributes
#endif
            configDesc[configDesc_num++]  = 0xfa; // MaxPower               - Maximum power consumption for this configuration (mA)

            /* otg_descriptor*/

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x00; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0xe0; // bFunctionClass 0xef
            configDesc[configDesc_num++] = 0x01; // bFunctionSubClass 0x04
            configDesc[configDesc_num++] = 0x03; // bFunctionProtocol 0x01
            configDesc[configDesc_num++] = 0x05; // Index of string descriptor describing this function

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x00; //bInterfaceNumber     - for RNDIS
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x01; // bNumEnd
            configDesc[configDesc_num++] = 0xe0; //bInterfaceClass
            configDesc[configDesc_num++] = 0x01; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x03; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x01; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bControlInterface
            configDesc[configDesc_num++] = 0x01; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetCtrlEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x01; // bInterfaceNumber     --- for RNDIS
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // NumEndpoints
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified
            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetTxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetRxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval
            /* MV_USB_RNDIS end */

            /* INTERFACE ASSOCIATION DESCRIPTOR */
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */


            configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++] = 0x02;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0xff; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x04; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)


            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x06; // bInterfaceNumber      ------- for Sulog
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x09; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = SulogTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = SulogRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

#ifdef MV_USB2_MASS_STORAGE
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */

            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x07;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif

			configDesc[configDesc_num++] = 0x09;  // bLength				- Descriptor length
			configDesc[configDesc_num++] = 0x04;  // bDescriptorType		- Descriptor Type
			configDesc[configDesc_num++] = 0x08;  //bInterfaceNumber	   - for modem On Composite Device (AT)
			configDesc[configDesc_num++] = 0x00; // bAlt
			configDesc[configDesc_num++] = 0x02; // bNumEnd
			configDesc[configDesc_num++] = 0xff; //bInterfaceClass
			configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
			configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
			configDesc[configDesc_num++] = 0x0B; // iInterface

			configDesc[configDesc_num++] = 0x07; // bLength 			- Descriptor length
			configDesc[configDesc_num++] = 0x05; // bDescriptorType 	- Descriptor Type
			configDesc[configDesc_num++] = (0x0e | 0x80); // bEndpointAddress 	   - Endpoint Address & Direction for modem tx endpoint
			configDesc[configDesc_num++] = 0x02; // bmAttributes		  - BULK,ISO,Interrupt
			PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
			configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
			configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
			configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
			configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
			configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

			/*
			07 05 85 02  .@......				 12.2.144
			40 00 00
			bendpointAddress=85  (IN)  bmAttributes=02	Bulk mode		 wMaxPacketSize=40 00	  bInterval=   00
			*/
			configDesc[configDesc_num++] = 0x07; // bLength 			- Descriptor length
			configDesc[configDesc_num++] = 0x05; // bDescriptorType 	- Descriptor Type
			configDesc[configDesc_num++] = 0x0f; // bEndpointAddress 	   - Endpoint Address & Direction for modem tx endpoint
			configDesc[configDesc_num++] = 0x02; // bmAttributes		  - BULK,ISO,Interrupt
			PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
			configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
			configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
			configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
			configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
			configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;
        }

#ifdef CRANE_MCU_DONGLE
        case USB_MARVELL_ECM_DESCRIPTOR:
        {
            devDesc_num=0;
            /* Device Descriptor      */
            devDesc[devDesc_num++] = 0x12; /*  bLength              - Descriptor length.                                    */
            devDesc[devDesc_num++] = 0x01; /*  bDescriptorType      - Descriptor  Type.                                     */
            devDesc[devDesc_num++] = 0x00; /*  bcdUSB (LSB)     - Device Compliant to USB specification .                   */
            devDesc[devDesc_num++] = 0x02; /*  bcdUSB (MSB).                                                                */
            devDesc[devDesc_num++] = 0x02; /*  bDeviceClass     - class of the device.                                      */
            devDesc[devDesc_num++] = 0x00; /*  bDeviceSubClass      - subclass of the device .                              */
            devDesc[devDesc_num++] = 0x00; /*  bDeviceProtocol      - protocol of the device.                               */
            devDesc[devDesc_num++] = 0x40; /*  bMaxPacketSize0      - Max Packet Size for EP zero.                          */
            devDesc[devDesc_num++] = 0x86; /*  idVendor (LSB)       - Vendor ID                                             */
            devDesc[devDesc_num++] = 0x12; /*  idVendor (MSB)                                                               */
            devDesc[devDesc_num++] = 0x31; /*  idProduct (LSB)      - Product ID                                            */
            devDesc[devDesc_num++] = 0x4E; /*  idProduct (LSB)      - Product ID                                            */
            devDesc[devDesc_num++] = 0x01; /*  bcdDevice (LSB)      - The device release number.                            */
            devDesc[devDesc_num++] = 0x00; /*  bcdDevice (MSB).                                                             */
            devDesc[devDesc_num++] = 0x04; /*  iManufacturer        - Index of string descriptor describing Manufacturer.   */
            devDesc[devDesc_num++] = 0x03; /*  iProduct         - Index of string descriptor describing Product.            */
            devDesc[devDesc_num++] = 0x00; /*  iSerialNumber        - Index of string descriptor describing Serial number.  */
            devDesc[devDesc_num++] = 0x01; /*  bNumConfigurations   - Number of configurations                              */


            configDesc_num=0;

            /* Configuration Descriptor     */
            configDesc[configDesc_num++]  = 0x09; /* bLength                - Descriptor length .                                           */
            configDesc[configDesc_num++]  = 0x02; /* bDescriptorType        - Descriptor Type.                                              */
            configDesc[configDesc_num++]  = 0x50; /* wTotalLength (LSB) - Total Data length for the configuration.                          */
            configDesc[configDesc_num++]  = 0x00; /* wTotalLength (MSB) - includes all descriptors for this configuration .                 */
            configDesc[configDesc_num++]  = 0x02; /* bNumInterfaces     - Number of interfaces this configuration supports.                 */
            configDesc[configDesc_num++]  = 0x01; /* bConfigurationValue   - The Value that should be used to select this configuration.    */
            configDesc[configDesc_num++]  = 0x02; /* iConfiguration     - Index of string descriptor describing this configuration.         */
            configDesc[configDesc_num++]  = 0xE0; /* bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup.                       */
            configDesc[configDesc_num++]  = 0xFA; /* MaxPower               - Maximum power consumption for this configuration (mA).        */


            /* Standard Interface Descriptor */
            configDesc[configDesc_num++] = 0x09; /* bLength  - Size of this descriptor in bytes.                    */
            configDesc[configDesc_num++] = 0x04; /* bDescriptorType - INTERFACE Descriptor Type.                    */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceNumber -  This interface Number.                      */
            configDesc[configDesc_num++] = 0x00; /* bAlternateSetting -                                             */
                                                 /* Number Value used to select this alternate setting for the      */
                                                 /* interface identified in the prior field.                        */
            configDesc[configDesc_num++] = 0x01; /* NumEndpoints - Number of endpoints used by this interface.      */
            configDesc[configDesc_num++] = 0x02; /* bInterfaceClass - Communication Interface Class code.           */
            configDesc[configDesc_num++] = 0x06; /* bInterfaceSubclass  - ECM.                                      */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceprotocol  - ECM Protocol.                             */
            configDesc[configDesc_num++] = 0x00; /* iInterface string.                                              */


            /* CDC Header functional descriptor.  */
            configDesc[configDesc_num++] = 0x05; /* bFunctionLength.                                                */

            configDesc[configDesc_num++] = 0x24; /* bDescriptorType.                                                */

            configDesc[configDesc_num++] = 0x00; /* bDescriptorSubtype.                                             */
                                                 /* Header Functional Descriptor.                                   */
            configDesc[configDesc_num++] = 0x10;
            configDesc[configDesc_num++] = 0x01; /* BCD.                                                            */


            /* ECM functional descriptor.           */
            configDesc[configDesc_num++] = 0x0D; /* bFunctionLength                                                 */

            configDesc[configDesc_num++] = 0x24; /* bDescriptorType                                                 */

            configDesc[configDesc_num++] = 0x0F; /* bDescriptorSubtype - Ethernet Networking Functional Descriptor  */

            configDesc[configDesc_num++] = 0x0E; /* iMACAddress, Index of string descriptor.The string descriptor   */
                                                 /*   holds the 48bit Ethernet MAC address.                         */
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x00; /* bmEthernetStatistics.                                           */
                                                 /* Indicates which Ethernet statistics functions                   */
                                                 /* the device collects.                                            */

            configDesc[configDesc_num++] = 0x08;
            configDesc[configDesc_num++] = 0x06; /* wMaxSegmentSize                                                 */
                                                 /* The maximum segment size that the Ethernet device is            */
                                                 /* capable of supporting.                                          */

            configDesc[configDesc_num++] = 0x01;
            configDesc[configDesc_num++] = 0x00; /* wNumberMCFilters.                                               */
                                                 /* Contains the number of multicast filters that can be            */
                                                 /* configured by the host.                                         */

            configDesc[configDesc_num++] = 0x00; /* bNumberPowerFilters. */
                                                 /* Contains the number of pattern filters that are available       */
                                                 /* for causing wake-up of the host.                                */


             /* CDC Header functional descriptor.  */
             configDesc[configDesc_num++] = 0x05; /* bFunctionLength.                                               */

             configDesc[configDesc_num++] = 0x24; /* bDescriptorType.                                               */

             configDesc[configDesc_num++] = 0x06; /* bDescriptorSubtype.                                            */
                                                  /* Union Functional Descriptor.                                   */
             configDesc[configDesc_num++] = 0x00;
             configDesc[configDesc_num++] = 0x01; /* BCD.                                                           */


            /* Standard Endpoint Descriptor. */
            configDesc[configDesc_num++] = 0x07; /* bLength - Number Size of this descriptor in bytes               */
            configDesc[configDesc_num++] = 0x05; /* bDescriptorType - Constant ENDPOINT Descriptor Type             */
            configDesc[configDesc_num++] = NetCtrlEP; /* bEndpointAddress -                                              */
                                                 /* Bit 7:  Direction, ignored for control endpoints��              */
                                                 /* 0 = OUT endpoint��1 = IN endpoint                               */
            configDesc[configDesc_num++] = 0x03; /* bmAttributes                                                    */
            configDesc[configDesc_num++] = 0x40; /* wMaxPacketSize - Maximum packet size this endpoint is capable   */
                                                 /* of sending or receiving when this configuration is selected.    */
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x10; /* bInterval                                                       */


            /* Standard Interface Descriptor */
            configDesc[configDesc_num++] = 0x09; /* bLength  - Size of this descriptor in bytes                     */
            configDesc[configDesc_num++] = 0x04; /* bDescriptorType - INTERFACE Descriptor Type                     */
            configDesc[configDesc_num++] = 0x01; /* bInterfaceNumber -  This interface Number.                      */
            configDesc[configDesc_num++] = 0x00; /* bAlternateSetting                                               */
                                                 /* - Number Value used to select this alternate setting            */
                                                 /* for the interface identified in the prior field                 */
            configDesc[configDesc_num++] = 0x00; /* NumEndpoints - Number of endpoints used by this interface       */
            configDesc[configDesc_num++] = 0x0A; /* bInterfaceClass - Data Interface Class code                     */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceSubclass - Data Class SubClass code                   */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceprotocol                                              */
                                                 /* -Data Class Protocol code, Network transfer block               */
            configDesc[configDesc_num++] = 0x00; /* iInterface string                                               */
                                                 /* - Index of string descriptor describing this interface          */

            /* Standard Interface Descriptor */
            configDesc[configDesc_num++] = 0x09; /* bLength  - Size of this descriptor in bytes                     */
            configDesc[configDesc_num++] = 0x04; /* bDescriptorType - INTERFACE Descriptor Type                     */
            configDesc[configDesc_num++] = 0x01; /* bInterfaceNumber -  This interface Number.                      */
            configDesc[configDesc_num++] = 0x01; /* bAlternateSetting                                               */
                                                 /* -Number Value used to select this alternate setting             */
                                                 /* for the interface identified in the prior field                 */
            configDesc[configDesc_num++] = 0x02; /* NumEndpoints - Number of endpoints used by this interface       */
            configDesc[configDesc_num++] = 0x0A; /* bInterfaceClass - Data Interface Class code                     */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceSubclass - Data Class SubClass code                   */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceprotocol                                              */
                                                 /* -Data Class Protocol code, Network transfer block               */
            configDesc[configDesc_num++] = 0x00; /* iInterface string                                               */
                                                 /* -Index of string descriptor describing this interface           */


            /* Standard Endpoint Descriptor. */
            configDesc[configDesc_num++] = 0x07; /* bLength - Number Size of this descriptor in bytes               */
            configDesc[configDesc_num++] = 0x05; /* bDescriptorType - Constant ENDPOINT Descriptor Type             */
            configDesc[configDesc_num++] = NetTxEP; /* bEndpointAddress                                                */
                                                 /* Bit 7:  Direction, ignored for control endpoints��              */
                                                 /* 0 = OUT endpoint��1 = IN endpoint                               */
            configDesc[configDesc_num++] = 0x02; /* bmAttributes                                                    */
            configDesc[configDesc_num++] = 0x00; /* wMaxPacketSize                                                  */
            configDesc[configDesc_num++] = 0x02;
            configDesc[configDesc_num++] = 0x00; /* bInterval                                                       */

            /* Standard Endpoint Descriptor. */
            configDesc[configDesc_num++] = 0x07; /* bLength - Number Size of this descriptor in bytes               */
            configDesc[configDesc_num++] = 0x05; /* bDescriptorType - Constant ENDPOINT Descriptor Type             */
            configDesc[configDesc_num++] = NetRxEP; /* bEndpointAddress                                                */
                                                 /* Bit 7:  Direction, ignored for control endpoints��              */
                                                 /* 0 = OUT endpoint��1 = IN endpoint                               */
            configDesc[configDesc_num++] = 0x02; /* bmAttributes                                                    */
            configDesc[configDesc_num++] = 0x00; /* wMaxPacketSize                                                  */
            configDesc[configDesc_num++] = 0x02;
            configDesc[configDesc_num++] = 0x00; /* bInterval                                                       */

            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;

            break;

        }

		case USB_MBIM_DESCRIPTOR:
		{
            devDesc_num=0;

            CPUartLogPrint("USB_MBIM_DESCRIPTOR");

            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength              - Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType      - Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)

            devDesc[devDesc_num++] = 0xef; //  bDeviceClass     - class of the device
            devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass      - subclass of the device
            devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol      - protocol of the device

            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0      - Max Packet Size for EP zero

            devDesc[devDesc_num++] = 0xCC; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x2E; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x13; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x30; //  idProduct (MSB)

            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)      - The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer        - Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct         - Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber        - Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations   - Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength                - Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB) - Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration
#if 1
#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++]  = 0x07; // bNumInterfaces     - Number of interfaces this configuration supports
            }
            else
            {
                configDesc[configDesc_num++]  = 0x06; // bNumInterfaces     - Number of interfaces this configuration supports
            }
#else
            configDesc[configDesc_num++]  = 0x06; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
#else
            configDesc[configDesc_num++]  = 0x02;
#endif
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration     - Index of string descriptor describing this configuration
            configDesc[configDesc_num++]  = 0xc0; // bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup
            configDesc[configDesc_num++]  = 0xfa; // MaxPower               - Maximum power consumption for this configuration (mA)

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x00; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0x02; // bFunctionClass
            configDesc[configDesc_num++] = 0x0e; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x00; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x05; // Index of string descriptor describing this function

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x00; //bInterfaceNumber     - for mbim communication interface
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x01; // bNumEnd
            configDesc[configDesc_num++] = 0x02; //bInterfaceClass
            configDesc[configDesc_num++] = 0x0e; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //
#if 1
            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bControlInterface
            configDesc[configDesc_num++] = 0x01; // bsubordinateInterface
#endif

			/*mbim functional des*/
            configDesc[configDesc_num++] = 0x0c; // bFuncationLength
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x1b; // MBIM functional descriptor code
            configDesc[configDesc_num++] = 0x00; // bcd
            configDesc[configDesc_num++] = 0x01; // bcd
            configDesc[configDesc_num++] = 0x00; // wMaxControlMessage
            configDesc[configDesc_num++] = 0x02; // wMaxControlMessage
            configDesc[configDesc_num++] = 0x20; // bNumberFilters
            configDesc[configDesc_num++] = 0xc0; // bMaxFilterSize
            configDesc[configDesc_num++] = 0x00; // wMaxSegmentSize
            configDesc[configDesc_num++] = 0x08; // wMaxSegmentSize
            configDesc[configDesc_num++] = 0x20; // bmNetworkCapabilities

#if 0
			/*mbim extended des*/
            configDesc[configDesc_num++] = 0x08; // bFuncationLength
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x1c; // MBIM functional descriptor code
            configDesc[configDesc_num++] = 0x00; // bcd
            configDesc[configDesc_num++] = 0x01; // bcd
            configDesc[configDesc_num++] = 0x01; // bMaxOutstandingCommandMessages
            configDesc[configDesc_num++] = 0xdc; // wMTU
            configDesc[configDesc_num++] = 0x05; // wMTU
#endif
            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetCtrlEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval

            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x01; // bInterfaceNumber     --- for MBIM data class interface
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x00; // NumEndpoints
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x02; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified


            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x01; // bInterfaceNumber     --- for MBIM data class interface
            configDesc[configDesc_num++] = 0x01; // bAlt
            configDesc[configDesc_num++] = 0x02; // NumEndpoints
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x02; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x05; // iInterface  +++++++++++++++++++++++++++++++++++++need modified
            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetTxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05 8f  ..@.....                12.2.224
            02 40 00 00               .@..                    12.2.232

            bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = NetRxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

			//diag,at
			/* INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0x02; // bFunctionClass
            configDesc[configDesc_num++] = 0x02; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x01; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x08; // Index of string descriptor describing this function


            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x02; // bInterfaceNumber       ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x01; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface			- Index of string descriptor describing this Interface


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x03; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bControlInterface
            configDesc[configDesc_num++] = 0x03; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagCtrlEP; //bendpointAddress for diag contrl
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x03; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0x0a; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface			- Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress		- Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

            /* INTERFACE ASSOCIATION DESCRIPTOR */

            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x04; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0x02; // bFunctionClass
            configDesc[configDesc_num++] = 0x02; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x01; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x0B; // Index of string descriptor describing this function



            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x04; // bInterfaceNumber      - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x01; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x02; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x01; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x0B; // iInterface			- Index of string descriptor describing this Interface


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x05; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x04; // bControlInterface
            configDesc[configDesc_num++] = 0x05; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = MdmCtrlEP; //bendpointAddress for modem contrl endpoint
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval


            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */


            configDesc[configDesc_num++] = 0x09;  // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x05;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; // bEndpointAddress		- Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress		- Endpoint Address & Direction for modem rx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

#ifdef MV_USB2_MASS_STORAGE
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */

            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x06;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif
            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;
        }

		case USB_RNDIS_ONLY_DESCRIPTOR:
		{
			devDesc_num=0;
			//Device Descriptor
			devDesc[devDesc_num++] = 0x12; //  bLength				- Descriptor length
			devDesc[devDesc_num++] = 0x01; //  bDescriptorType		- Descriptor  Type
			devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB) 	- Device Compliant to USB specification ..
			devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
			devDesc[devDesc_num++] = 0xef; //  bDeviceClass 	- class of the device
			devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass		- subclass of the device
			devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol		- protocol of the device
			devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0		- Max Packet Size for EP zero
			devDesc[devDesc_num++] = 0xD3; //  idVendor (LSB)		- Vendor ID
			devDesc[devDesc_num++] = 0x13; //  idVendor (MSB)
			devDesc[devDesc_num++] = 0x87; //  idProduct (LSB)		- Product ID
			devDesc[devDesc_num++] = 0x34; //  idProduct (MSB)
			devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)		- The device release number
			devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
			devDesc[devDesc_num++] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
			devDesc[devDesc_num++] = 0x02; //  iProduct 		- Index of string descriptor describing Product
			devDesc[devDesc_num++] = 0x04; //  iSerialNumber		- Index of string descriptor describing Serial number
			devDesc[devDesc_num++] = 0x01; //  bNumConfigurations	- Number of configurations

			configDesc_num=0;

			//Configuration Descriptor
			configDesc[configDesc_num++]  = 0x09; // bLength				- Descriptor length
			configDesc[configDesc_num++]  = 0x02; // bDescriptorType		- Descriptor Type
			configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB) - Total Data length for the configuration,
			configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration

#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++]  = 0x03; // bNumInterfaces     - Number of interfaces this configuration supports
            }
            else
            {
                configDesc[configDesc_num++]  = 0x02; // bNumInterfaces     - Number of interfaces this configuration supports
            }
#else
            configDesc[configDesc_num++]  = 0x02; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
			configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
			configDesc[configDesc_num++]  = 0x00; // iConfiguration 	- Index of string descriptor describing this configuration
			configDesc[configDesc_num++]  = 0xc0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
			configDesc[configDesc_num++]  = 0xfa; // MaxPower				- Maximum power consumption for this configuration (mA)

            /*INTERFACE ASSOCIATION DESCRIPTOR */
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x00; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0xe0; // bFunctionClass
            configDesc[configDesc_num++] = 0x01; // bFunctionSubClass
            configDesc[configDesc_num++] = 0x03; // bFunctionProtocol
            configDesc[configDesc_num++] = 0x05; // Index of string descriptor describing this function

			/*INTERFACE ASSOCIATION DESCRIPTOR */
			configDesc[configDesc_num++] = 0x09; // bLength 			- Descriptor length
			configDesc[configDesc_num++] = 0x04; // bDescriptorType 	- Descriptor Type
			configDesc[configDesc_num++] = 0x00; //bInterfaceNumber 	- for RNDIS
			configDesc[configDesc_num++] = 0x00; // bAlt
			configDesc[configDesc_num++] = 0x01; // bNumEnd
			configDesc[configDesc_num++] = 0xE0; //bInterfaceClass
			configDesc[configDesc_num++] = 0x01; // bInterfaceSubclass
			configDesc[configDesc_num++] = 0x03; // bInterfaceprotocol
			configDesc[configDesc_num++] = 0x05; // iInterface	+++++++++++++++++++++++++++++++++++++need modified


			configDesc[configDesc_num++] = 0x05; //
			configDesc[configDesc_num++] = 0x24; // bDescriptorType
			configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
			configDesc[configDesc_num++] = 0x10; // BCD
			configDesc[configDesc_num++] = 0x01; //


			configDesc[configDesc_num++] = 0x05; //
			configDesc[configDesc_num++] = 0x24; // bDescriptorType
			configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
			configDesc[configDesc_num++] = 0x00; // bmCapabilities
			configDesc[configDesc_num++] = 0x01; //bDataInterface

			configDesc[configDesc_num++] = 0x04; //
			configDesc[configDesc_num++] = 0x24; // bDescriptorType
			configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
			configDesc[configDesc_num++] = 0x00; // bmCapabilities

			configDesc[configDesc_num++] = 0x05; //
			configDesc[configDesc_num++] = 0x24; // bDescriptorType
			configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
			configDesc[configDesc_num++] = 0x00; // bControlInterface
			configDesc[configDesc_num++] = 0x01; // bsubordinateInterface

			configDesc[configDesc_num++] = 0x07; //
			configDesc[configDesc_num++] = 0x05; //
			configDesc[configDesc_num++] = NetCtrlEP; //bendpointAddress
			configDesc[configDesc_num++] = 0x03; //bmAttributes
			configDesc[configDesc_num++] = 0x10; // wMaxPacketSize
			configDesc[configDesc_num++] = 0x00; //
			configDesc[configDesc_num++] = 0x10; // bInterval


			configDesc[configDesc_num++] = 0x09; // bLength 			- Descriptor length
			configDesc[configDesc_num++] = 0x04; // bDescriptorType 	- Descriptor Type
			configDesc[configDesc_num++] = 0x01; // bInterfaceNumber	 --- for RNDIS
			configDesc[configDesc_num++] = 0x00; // bAlt
			configDesc[configDesc_num++] = 0x02; // NumEndpoints
			configDesc[configDesc_num++] = 0x0a; //bInterfaceClass
			configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
			configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
			configDesc[configDesc_num++] = 0x05; // iInterface	+++++++++++++++++++++++++++++++++++++need modified

			configDesc[configDesc_num++] = 0x07; //
			configDesc[configDesc_num++] = 0x05; //
			configDesc[configDesc_num++] = NetTxEP; //bendpointAddress
			configDesc[configDesc_num++] = 0x02; //bmAttributes
			PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
			configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
			configDesc[configDesc_num++] = 0x00; //
#else
			configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
			configDesc[configDesc_num++] = 0x02; //
#endif
			configDesc[configDesc_num++] = 0x00; // bInterval

			configDesc[configDesc_num++] = 0x07; //
			configDesc[configDesc_num++] = 0x05; //
			configDesc[configDesc_num++] = NetRxEP; //bendpointAddress
			configDesc[configDesc_num++] = 0x02; //bmAttributes
			PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
			configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
			configDesc[configDesc_num++] = 0x00; //
#else
			configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
			configDesc[configDesc_num++] = 0x02; //
#endif
			configDesc[configDesc_num++] = 0x00; // bInterval

#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x06;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif

			configDesc[2]=configDesc_num & 0xFF;
			configDesc[3]=(configDesc_num>>8) & 0xFF;

			break;

		}

        case USB_CDROM_ONLY_DESCRIPTOR:
        {

            //Device Descriptor
            devDesc[0]  = 0x12; //  bLength				- Descriptor length
            devDesc[1]  = 0x01; //  bDescriptorType		- Descriptor  Type
            devDesc[2]  = 0x00; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
            devDesc[3]  = 0x02; //  bcdUSB (MSB)
            devDesc[4]  = 0x00; //  bDeviceClass		- class of the device
            devDesc[5]  = 0x00; //  bDeviceSubClass		- subclass of the device
            devDesc[6]  = 0x00; //  bDeviceProtocol		- protocol of the device
            devDesc[7]  = 0x40; //  bMaxPacketSize0		- Max Packet Size for EP zero

        /* These IDs have been donated by NetChip Technologies for open source
        * development.  DO NOT REUSE THESE IDs with any other driver!!
        * Ever!!  Instead: allocate your own, using normal USB-IF procedures.
#define DRIVER_VENDOR_ID	0x0525	// NetChip
#define DRIVER_PRODUCT_ID	0xa4a5	// Linux-USB File-backed Storage Gadget
#define DRIVER_RELEASE_ID	0x0399
        */

            switch(pUsbDesInfo->DefaultDesc)
            {
                case USB_ASR_MIFI_DESCRIPTOR:
                default:
                {
                    devDesc[8]  = 0xCC; //  idVendor (LSB)		- Vendor ID
                    devDesc[9]  = 0x2E; //  idVendor (MSB)
                    devDesc[10] = 0xA1; //  idProduct (LSB)		- Product ID
                    devDesc[11] = 0x1D; //  idProduct (MSB)
                    break;
                }
            }

            devDesc[12] = 0x00; //  bcdDevice (LSB)		- The device release number
            devDesc[13] = 0x01; //  bcdDevice (MSB)
            devDesc[14] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
            devDesc[15] = 0x02; //  iProduct			- Index of string descriptor describing Product
            devDesc[16] = 0x03; //  iSerialNumber		- Index of string descriptor describing Serial number
            devDesc[17] = 0x01; //  bNumConfigurations	- Number of configurations

            //Configuration Descriptor
            configDesc[0]  = 0x09; // bLength				- Descriptor length
            configDesc[1]  = 0x02; // bDescriptorType		- Descriptor Type
            configDesc[2]  = 0x20; // wTotalLength (LSB)	- Total Data length for the configuration,
            configDesc[3]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
            configDesc[4]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
            configDesc[5]  = 0x02; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[6]  = 0x04; // iConfiguration		- Index of string descriptor describing this configuration  /////
            configDesc[7]  = 0xc0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
            configDesc[8]  = 0xfa; // MaxPower				- Maximum power consumption for this configuration (mA)

            //Interface Descriptor
            configDesc[9]  = 0x09; // bLength				- Descriptor length
            configDesc[10] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[11] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
            configDesc[12] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[13] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[14] = 0x08; // bInterfaceClass       - Class of this Interface
            configDesc[15] = 0x06; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[16] = 0x50; // bInterfaceProtocol    - Protocol of this Interface

            switch(pUsbDesInfo->DefaultDesc)
            {
                case USB_ASR_MIFI_DESCRIPTOR:
                default:
                {
                    configDesc[17] = 0x00; // iInterface			- Index of string descriptor describing this Interface
                    break;
                }
            }

        /*
                   07 05  ....P...                12.2.216
                   0f 02 40 00  00
                    bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
        */
            configDesc[18] = 0x07; //
            configDesc[19] = 0x05; //
            configDesc[20] = MSCRxEP; //bendpointAddress
            configDesc[21] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = 22;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[22] = 0x40; // wMaxPacketSize      //Jackie:HS MUST set to 0x200(512),FS MUST set to 0x40
            configDesc[23] = 0x00; //
#else
            configDesc[22] = 0x00; // wMaxPacketSize      //Jackie:HS MUST set to 0x200(512),FS MUST set to 0x40
            configDesc[23] = 0x02; //
#endif
            configDesc[24] = 0x00; // bInterval

        /*
                   07 05 8f  ..@.....                12.2.224
                   02 40 00 00               .@..                    12.2.232

                    bendpointAddress=8f (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
        */
            configDesc[25] = 0x07; //
            configDesc[26] = 0x05; //
            configDesc[27] = MSCTxEP; //bendpointAddress
            configDesc[28] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = 29;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[29] = 0x40; // wMaxPacketSize      //Jackie:HS MUST set to 0x200(512),FS MUST set to 0x40
            configDesc[30] = 0x00; //
#else
            configDesc[29] = 0x00; // wMaxPacketSize      //Jackie:HS MUST set to 0x200(512),FS MUST set to 0x40
            configDesc[30] = 0x02; //
#endif
            configDesc[31] = 0x00; // bInterval
            break;
        }

        case USB_GENERIC_MOD_ECM_DESCRIPTOR: //Generic RNDIS driver, ECM+USB DIAG+2COM(AT+PPP)
        {
            devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength              - Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType      - Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0xef; //  bDeviceClass     - class of the device
            devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass      - subclass of the device
            devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol      - protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0      - Max Packet Size for EP zero

#if 1 //ASR PID/VID
            devDesc[devDesc_num++] = 0x86; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x12; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x3C; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x4E; //  idProduct (MSB)
#endif

            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)      - The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer        - Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct         - Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber        - Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations   - Number of configurations

            configDesc_num=0;
            /* Configuration Descriptor     */
            configDesc[configDesc_num++]  = 0x09; /* bLength                - Descriptor length .                                           */
            configDesc[configDesc_num++]  = 0x02; /* bDescriptorType        - Descriptor Type.                                              */
            configDesc[configDesc_num++]  = 0xeb; /* wTotalLength (LSB) - Total Data length for the configuration.                          */
            configDesc[configDesc_num++]  = 0x00; /* wTotalLength (MSB) - includes all descriptors for this configuration .                 */
            configDesc[configDesc_num++]  = 0x05; /* bNumInterfaces     - Number of interfaces this configuration supports.                 */
            configDesc[configDesc_num++]  = 0x01; /* bConfigurationValue   - The Value that should be used to select this configuration.    */
            configDesc[configDesc_num++]  = 0x00; /* iConfiguration     - Index of string descriptor describing this configuration.         */
            configDesc[configDesc_num++]  = 0xC0; /* bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup.                       */
            configDesc[configDesc_num++]  = 0xFA; /* MaxPower               - Maximum power consumption for this configuration (mA).        */


//Interface0,1 Descriptor,ECM-------------------------------------------------------------------------
            configDesc[configDesc_num++] = 0x08; // bLength
            configDesc[configDesc_num++] = 0x0b; // INTERFACE ASSOCIATION DESCRIPTOR bDescriptorType
            configDesc[configDesc_num++] = 0x00; // bFirstInterface
            configDesc[configDesc_num++] = 0x02; // bInterfaceCount
            configDesc[configDesc_num++] = 0x02; // bFunctionClass 0xef
            configDesc[configDesc_num++] = 0x06; // bFunctionSubClass 0x04
            configDesc[configDesc_num++] = 0x00; // bFunctionProtocol 0x01
            configDesc[configDesc_num++] = 0x05; // Index of string descriptor describing this function


            /* Standard Interface Descriptor */
            configDesc[configDesc_num++] = 0x09; /* bLength  - Size of this descriptor in bytes.                    */
            configDesc[configDesc_num++] = 0x04; /* bDescriptorType - INTERFACE Descriptor Type.                    */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceNumber -  This interface Number.                      */
            configDesc[configDesc_num++] = 0x00; /* bAlternateSetting -                                             */
                                                 /* Number Value used to select this alternate setting for the      */
                                                 /* interface identified in the prior field.                        */
            configDesc[configDesc_num++] = 0x01; /* NumEndpoints - Number of endpoints used by this interface.      */
            configDesc[configDesc_num++] = 0x02; /* bInterfaceClass - Communication Interface Class code.           */
            configDesc[configDesc_num++] = 0x06; /* bInterfaceSubclass  - ECM.                                      */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceprotocol  - ECM Protocol.                             */
            configDesc[configDesc_num++] = 0x05; /* iInterface string.                                              */


            /* CDC Header functional descriptor.  */
            configDesc[configDesc_num++] = 0x05; /* bFunctionLength.                                                */

            configDesc[configDesc_num++] = 0x24; /* bDescriptorType.                                                */

            configDesc[configDesc_num++] = 0x00; /* bDescriptorSubtype.                                             */
                                                 /* Header Functional Descriptor.                                   */
            configDesc[configDesc_num++] = 0x10;
            configDesc[configDesc_num++] = 0x01; /* BCD.                                                            */


            /* ECM functional descriptor.           */
            configDesc[configDesc_num++] = 0x0D; /* bFunctionLength                                                 */

            configDesc[configDesc_num++] = 0x24; /* bDescriptorType                                                 */

            configDesc[configDesc_num++] = 0x0F; /* bDescriptorSubtype - Ethernet Networking Functional Descriptor  */

            configDesc[configDesc_num++] = 0x0E; /* iMACAddress, Index of string descriptor.The string descriptor   */
                                                 /*   holds the 48bit Ethernet MAC address.                         */
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x00; /* bmEthernetStatistics.                                           */
                                                 /* Indicates which Ethernet statistics functions                   */
                                                 /* the device collects.                                            */

            configDesc[configDesc_num++] = 0x08;
            configDesc[configDesc_num++] = 0x06; /* wMaxSegmentSize                                                 */
                                                 /* The maximum segment size that the Ethernet device is            */
                                                 /* capable of supporting.                                          */

            configDesc[configDesc_num++] = 0x01;
            configDesc[configDesc_num++] = 0x00; /* wNumberMCFilters.                                               */
                                                 /* Contains the number of multicast filters that can be            */
                                                 /* configured by the host.                                         */

            configDesc[configDesc_num++] = 0x00; /* bNumberPowerFilters. */
                                                 /* Contains the number of pattern filters that are available       */
                                                 /* for causing wake-up of the host.                                */


             /* CDC Header functional descriptor.  */
             configDesc[configDesc_num++] = 0x05; /* bFunctionLength.                                               */

             configDesc[configDesc_num++] = 0x24; /* bDescriptorType.                                               */

             configDesc[configDesc_num++] = 0x06; /* bDescriptorSubtype.                                            */
                                                  /* Union Functional Descriptor.                                   */
             configDesc[configDesc_num++] = 0x00;
             configDesc[configDesc_num++] = 0x01; /* BCD.                                                           */


            /* Standard Endpoint Descriptor. */
            configDesc[configDesc_num++] = 0x07; /* bLength - Number Size of this descriptor in bytes               */
            configDesc[configDesc_num++] = 0x05; /* bDescriptorType - Constant ENDPOINT Descriptor Type             */
            configDesc[configDesc_num++] = NetCtrlEP; /* bEndpointAddress -                                              */
                                                 /* Bit 7:  Direction, ignored for control endpoints��              */
                                                 /* 0 = OUT endpoint��1 = IN endpoint                               */
            configDesc[configDesc_num++] = 0x03; /* bmAttributes                                                    */
            configDesc[configDesc_num++] = 0x40; /* wMaxPacketSize - Maximum packet size this endpoint is capable   */
                                                 /* of sending or receiving when this configuration is selected.    */
            configDesc[configDesc_num++] = 0x00;
            configDesc[configDesc_num++] = 0x10; /* bInterval                                                       */


            /* Standard Interface Descriptor */
            configDesc[configDesc_num++] = 0x09; /* bLength  - Size of this descriptor in bytes                     */
            configDesc[configDesc_num++] = 0x04; /* bDescriptorType - INTERFACE Descriptor Type                     */
            configDesc[configDesc_num++] = 0x01; /* bInterfaceNumber -  This interface Number.                      */
            configDesc[configDesc_num++] = 0x00; /* bAlternateSetting                                               */
                                                 /* - Number Value used to select this alternate setting            */
                                                 /* for the interface identified in the prior field                 */
            configDesc[configDesc_num++] = 0x00; /* NumEndpoints - Number of endpoints used by this interface       */
            configDesc[configDesc_num++] = 0x0A; /* bInterfaceClass - Data Interface Class code                     */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceSubclass - Data Class SubClass code                   */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceprotocol                                              */
                                                 /* -Data Class Protocol code, Network transfer block               */
            configDesc[configDesc_num++] = 0x05; /* iInterface string                                               */
                                                 /* - Index of string descriptor describing this interface          */

            /* Standard Interface Descriptor */
            configDesc[configDesc_num++] = 0x09; /* bLength  - Size of this descriptor in bytes                     */
            configDesc[configDesc_num++] = 0x04; /* bDescriptorType - INTERFACE Descriptor Type                     */
            configDesc[configDesc_num++] = 0x01; /* bInterfaceNumber -  This interface Number.                      */
            configDesc[configDesc_num++] = 0x01; /* bAlternateSetting                                               */
                                                 /* -Number Value used to select this alternate setting             */
                                                 /* for the interface identified in the prior field                 */
            configDesc[configDesc_num++] = 0x02; /* NumEndpoints - Number of endpoints used by this interface       */
            configDesc[configDesc_num++] = 0x0A; /* bInterfaceClass - Data Interface Class code                     */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceSubclass - Data Class SubClass code                   */
            configDesc[configDesc_num++] = 0x00; /* bInterfaceprotocol                                              */
                                                 /* -Data Class Protocol code, Network transfer block               */
            configDesc[configDesc_num++] = 0x00; /* iInterface string                                               */
                                                 /* -Index of string descriptor describing this interface           */


            /* Standard Endpoint Descriptor. */
            configDesc[configDesc_num++] = 0x07; /* bLength - Number Size of this descriptor in bytes               */
            configDesc[configDesc_num++] = 0x05; /* bDescriptorType - Constant ENDPOINT Descriptor Type             */
            configDesc[configDesc_num++] = NetTxEP; /* bEndpointAddress                                                */
                                                 /* Bit 7:  Direction, ignored for control endpoints��              */
                                                 /* 0 = OUT endpoint��1 = IN endpoint                               */
            configDesc[configDesc_num++] = 0x02; /* bmAttributes                                                    */
            configDesc[configDesc_num++] = 0x00; /* wMaxPacketSize                                                  */
            configDesc[configDesc_num++] = 0x02;
            configDesc[configDesc_num++] = 0x00; /* bInterval                                                       */

            /* Standard Endpoint Descriptor. */
            configDesc[configDesc_num++] = 0x07; /* bLength - Number Size of this descriptor in bytes               */
            configDesc[configDesc_num++] = 0x05; /* bDescriptorType - Constant ENDPOINT Descriptor Type             */
            configDesc[configDesc_num++] = NetRxEP; /* bEndpointAddress                                                */
                                                 /* Bit 7:  Direction, ignored for control endpoints��              */
                                                 /* 0 = OUT endpoint��1 = IN endpoint                               */
            configDesc[configDesc_num++] = 0x02; /* bmAttributes                                                    */
            configDesc[configDesc_num++] = 0x00; /* wMaxPacketSize                                                  */
            configDesc[configDesc_num++] = 0x02;
            configDesc[configDesc_num++] = 0x00; /* bInterval                                                       */
            /* ECM end */


//Interface2 Descriptor,,DIAG-------------------------------------------------------------------------
            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x02; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; //0x03; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface          - Index of string descriptor describing this Interface

            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)


//Interface3 Descriptor,,AT----------------------------------------------------------------------------
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x03; //0x02; // bInterfaceNumber      - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x03; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xFF; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x0B; // iInterface          - Index of string descriptor describing this Interface


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x00; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bControlInterface
            configDesc[configDesc_num++] = 0x00; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = AtCtrlEP; //MdmCtrlEP; //bendpointAddress for modem contrl endpoint
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = AtTxEP; //MdmTxEP; // bEndpointAddress       - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = AtRxEP; //MdmRxEP; // bEndpointAddress       - Endpoint Address & Direction for modem rx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

#if 1
            //Interface4 Descriptor,, PPP----------------------------------------------------------------------------
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x04; //0x03; // bInterfaceNumber      - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x03; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xFF; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x0B; // iInterface          - Index of string descriptor describing this Interface


            /*
            05 24  00 10 01
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=00  Header Function      BCD=10 01
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x00; //bDescriptorSubtype
            configDesc[configDesc_num++] = 0x10; // BCD
            configDesc[configDesc_num++] = 0x01; //


            /*
            05 24 01 03  $....$..                12.2.112
            04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=01          Call Management Functional Descriptor.
            bmCapabilities=03    bDataInterface= 04
            */

            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x01; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bmCapabilities
            configDesc[configDesc_num++] = 0x00; //bDataInterface

            /*
            04 24 02  07
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=02 Abstract Control Management Functional Descriptor.
            bmCapabilities=07    ???? WMC specify is 0x6
            */
            configDesc[configDesc_num++] = 0x04; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x02; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x02; // bmCapabilities

            /*
            05  ...$....                12.2.104
            24 06 03 04
            bDescriptorType=24   CS_INTERFACE   bDescriptorSubtype=06          Union Functional Descriptor
            bControlInterface=03    bsubordinateInterface= 04
            */
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = 0x24; // bDescriptorType
            configDesc[configDesc_num++] = 0x06; // bDescriptorSubtype
            configDesc[configDesc_num++] = 0x00; // bControlInterface
            configDesc[configDesc_num++] = 0x00; // bsubordinateInterface

            /*
            07 05  ....P...                12.2.216
            0f 02 40 00  00
            bendpointAddress=0f  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = MdmCtrlEP; //PppCtrlEP; //bendpointAddress for modem contrl endpoint
            configDesc[configDesc_num++] = 0x03; //bmAttributes
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
            configDesc[configDesc_num++] = 0x10; // bInterval

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; //PppTxEP; // bEndpointAddress      - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; //PppRxEP; // bEndpointAddress      - Endpoint Address & Direction for modem rx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
#endif//if 0

            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;

        }
#endif

#ifdef YMODEM_EEH_DUMP
		case USB_MODEM_ONLY_DESCRIPTOR:
		{
			devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength				- Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType		- Descriptor  Type
            devDesc[devDesc_num++] = 0x10; //  bcdUSB (LSB)		- Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x01; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0x00; //  bDeviceClass		- class of the device
            devDesc[devDesc_num++] = 0x00; //  bDeviceSubClass		- subclass of the device
            devDesc[devDesc_num++] = 0x00; //  bDeviceProtocol		- protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0		- Max Packet Size for EP zero
#if 0
            devDesc[devDesc_num++] = 0xd2; //  idVendor (LSB)		- Vendor ID
            devDesc[devDesc_num++] = 0x19; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x79; //  idProduct (LSB)		- Product ID
            devDesc[devDesc_num++] = 0x00; //  idProduct (MSB)
#endif
            devDesc[devDesc_num++] = 0xCC; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x2E; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x17; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x30; //  idProduct (MSB)


            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)		- The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer		- Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct			- Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x04; //  iSerialNumber		- Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations	- Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++]  = 0x7c; // wTotalLength (LSB)	- Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB)	- includes all descriptors for this configuration
            configDesc[configDesc_num++]  = 0x01; // bNumInterfaces		- Number of interfaces this configuration supports
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration		- Index of string descriptor describing this configuration
            configDesc[configDesc_num++]  = 0xc0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
            configDesc[configDesc_num++]  = 0xfa; // MaxPower				- Maximum power consumption for this configuration (mA)

            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = 0x00; // bInterfaceNumber      - Index (Number) of this interfaces
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints			- Number endpoints	used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x07; // iInterface			- Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
             bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = MdmTxEP; //bendpointAddress
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
              bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength				- Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType		- Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress		- Endpoint Address & Direction
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval				- Polling Interval (ms)

            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
			break;
		}
#endif
        case USB_CDROM_DIAG_DESCRIPTOR:
        {
            
            uart_printf("USB_CDROM_DIAG_DESCRIPTOR\r\n");
            devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength              - Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType      - Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0xef; //  bDeviceClass     - class of the device
            devDesc[devDesc_num++] = 0x02; //  bDeviceSubClass      - subclass of the device
            devDesc[devDesc_num++] = 0x01; //  bDeviceProtocol      - protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0      - Max Packet Size for EP zero
            devDesc[devDesc_num++] = 0xCC; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x2E; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x10; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x30; //  idProduct (MSB)
            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)      - The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer        - Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct         - Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber        - Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations   - Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength                - Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB) - Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration
#ifdef MV_USB2_MASS_STORAGE
            if(storage == MASS_STORAGE_ENABLE)
            {
#ifdef PLAT_USE_ALIOS
                configDesc[configDesc_num++]  = 0x05; // bNumInterfaces     - Number of interfaces this configuration supports
#else
                configDesc[configDesc_num++]  = 0x05; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
            }
            else
            {
#ifdef PLAT_USE_ALIOS
                configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
#else
                configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
            }
#else
#ifdef PLAT_USE_ALIOS
            configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
#else
            configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
#endif
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration     - Index of string descriptor describing this configuration
#ifdef USB_REMOTEWAKEUP
            configDesc[configDesc_num++]  = 0xe0; // bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup
#else
            configDesc[configDesc_num++]  = 0xc0; // bmAttributes
#endif
            configDesc[configDesc_num++]  = 0xfa; // MaxPower               - Maximum power consumption for this configuration (mA)



            /* INTERFACE ASSOCIATION DESCRIPTOR */
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */


            configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++] = 0x02;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0xff; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x04; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)


            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x06; // bInterfaceNumber      ------- for Sulog
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x09; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = SulogTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = SulogRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

#ifdef MV_USB2_MASS_STORAGE
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */

            if(storage == MASS_STORAGE_ENABLE)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x07;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif

            configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++] = 0x08;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0xff; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = (0x0e | 0x80); // bEndpointAddress      - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x0f; // bEndpointAddress       - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;
        }
        case USB_MODEM_DIAG_DESCRIPTOR:
        {
            devDesc_num=0;
            //Device Descriptor
            devDesc[devDesc_num++] = 0x12; //  bLength              - Descriptor length
            devDesc[devDesc_num++] = 0x01; //  bDescriptorType      - Descriptor  Type
            devDesc[devDesc_num++] = 0x00; //  bcdUSB (LSB)     - Device Compliant to USB specification ..
            devDesc[devDesc_num++] = 0x02; //  bcdUSB (MSB)
            devDesc[devDesc_num++] = 0x00; //  bDeviceClass     - class of the device
            devDesc[devDesc_num++] = 0x00; //  bDeviceSubClass      - subclass of the device
            devDesc[devDesc_num++] = 0x00; //  bDeviceProtocol      - protocol of the device
            devDesc[devDesc_num++] = 0x40; //  bMaxPacketSize0      - Max Packet Size for EP zero
            devDesc[devDesc_num++] = 0xCC; //  idVendor (LSB)       - Vendor ID
            devDesc[devDesc_num++] = 0x2E; //  idVendor (MSB)
            devDesc[devDesc_num++] = 0x10; //  idProduct (LSB)      - Product ID
            devDesc[devDesc_num++] = 0x30; //  idProduct (MSB)
            devDesc[devDesc_num++] = 0x00; //  bcdDevice (LSB)      - The device release number
            devDesc[devDesc_num++] = 0x01; //  bcdDevice (MSB)
            devDesc[devDesc_num++] = 0x01; //  iManufacturer        - Index of string descriptor describing Manufacturer
            devDesc[devDesc_num++] = 0x02; //  iProduct         - Index of string descriptor describing Product
            devDesc[devDesc_num++] = 0x03; //  iSerialNumber        - Index of string descriptor describing Serial number
            devDesc[devDesc_num++] = 0x01; //  bNumConfigurations   - Number of configurations

            configDesc_num=0;

            //Configuration Descriptor
            configDesc[configDesc_num++]  = 0x09; // bLength                - Descriptor length
            configDesc[configDesc_num++]  = 0x02; // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++]  = 0xe9; // wTotalLength (LSB) - Total Data length for the configuration,
            configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration
#ifdef MV_USB2_MASS_STORAGE
            if(storage)
            {
                configDesc[configDesc_num++]  = 0x04; // bNumInterfaces     - Number of interfaces this configuration supports
            }
            else
            {
                configDesc[configDesc_num++]  = 0x03; // bNumInterfaces     - Number of interfaces this configuration supports
            }
#else
            configDesc[configDesc_num++]  = 0x02; // bNumInterfaces     - Number of interfaces this configuration supports
#endif
            configDesc[configDesc_num++]  = 0x01; // bConfigurationValue   - The Value that should be used to select this configuration
            configDesc[configDesc_num++]  = 0x00; // iConfiguration     - Index of string descriptor describing this configuration
            configDesc[configDesc_num++]  = 0xc0; // bmAttributes           - bit6: Self-Powered, bit5: RemoteWakeup
            configDesc[configDesc_num++]  = 0xfa; // MaxPower               - Maximum power consumption for this configuration (mA)


            /* INTERFACE ASSOCIATION DESCRIPTOR */
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */


            configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++] = 0x02;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0xff; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface

            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmTxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /*
            07 05 85 02  .@......                12.2.144
            40 00 00
            bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = MdmRxEP; // bEndpointAddress        - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

            /* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x04; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

#ifdef MV_USB2_MASS_STORAGE
            /*
            09 04 03  00 01 02 02  ........                12.2.96
            01 09
            bInterfaceNumber=03       bAlt=00      bNumEnd=01
            bInterfaceClass=02  communication class              bInterfaceSubclass=02     Abstract Control Model
            bInterfaceprotocol= 01  AT Commands: V.250 etc  USB specification  iInterface=09
            */

            if(storage)
            {
                configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
                configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
                configDesc[configDesc_num++] = 0x05;  //bInterfaceNumber        - for CD-ROM
                configDesc[configDesc_num++] = 0x00; // bAlt
                configDesc[configDesc_num++] = 0x02; // bNumEnd
                configDesc[configDesc_num++] = 0x08; //bInterfaceClass
                configDesc[configDesc_num++] = 0x06; // bInterfaceSubclass
                configDesc[configDesc_num++] = 0x50; // bInterfaceprotocol
                configDesc[configDesc_num++] = 0x0D; // iInterface

                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCTxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)

                /*
                07 05 85 02  .@......                12.2.144
                40 00 00
                bendpointAddress=85  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
                */
                configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
                configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
                configDesc[configDesc_num++] = MSCRxEP; // bEndpointAddress        - Endpoint Address & Direction
                configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
                PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
                configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
                configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
                configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
                configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            }
#endif
            configDesc[configDesc_num++] = 0x09;  // bLength                - Descriptor length
            configDesc[configDesc_num++] = 0x04;  // bDescriptorType        - Descriptor Type
            configDesc[configDesc_num++] = 0x08;  //bInterfaceNumber       - for modem On Composite Device (AT)
            configDesc[configDesc_num++] = 0x00; // bAlt
            configDesc[configDesc_num++] = 0x02; // bNumEnd
            configDesc[configDesc_num++] = 0xff; //bInterfaceClass
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubclass
            configDesc[configDesc_num++] = 0x00; // bInterfaceprotocol
            configDesc[configDesc_num++] = 0x0B; // iInterface
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = (0x0e | 0x80); // bEndpointAddress      - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x0f; // bEndpointAddress       - Endpoint Address & Direction for modem tx endpoint
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x02; // wMaxPacketSize (MSB)
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval               - Polling Interval (ms)
            configDesc[2]=configDesc_num & 0xFF;
            configDesc[3]=(configDesc_num>>8) & 0xFF;
            break;
        }

		case USB_DIAG_ONLY_DESCRIPTOR:
		{
			uart_printf("USB_DIAG_ONLY_DESCRIPTOR\r\n");
			//Device Descriptor
			devDesc[0]	= 0x12; //	bLength 			- Descriptor length
			devDesc[1]	= 0x01; //	bDescriptorType 	- Descriptor  Type
			devDesc[2]	= 0x00; //	bcdUSB (LSB)		- Device Compliant to USB specification ..
			devDesc[3]	= 0x02; //	bcdUSB (MSB)
			devDesc[4]	= 0x00; //	bDeviceClass		- class of the device
			devDesc[5]	= 0x00; //	bDeviceSubClass 	- subclass of the device
			devDesc[6]	= 0x00; //	bDeviceProtocol 	- protocol of the device
			devDesc[7]	= 0x40; //	bMaxPacketSize0 	- Max Packet Size for EP zero
			devDesc[8]	= 0xCC; //	idVendor (LSB)		- Vendor ID
			devDesc[9]	= 0x2E; //	idVendor (MSB)
			devDesc[10] = 0x02; //	idProduct (LSB) 	- Product ID
			devDesc[11] = 0x21; //	idProduct (MSB)
			devDesc[12] = 0x00; //	bcdDevice (LSB) 	- The device release number
			devDesc[13] = 0x01; //	bcdDevice (MSB)
			devDesc[14] = 0x01; //	iManufacturer		- Index of string descriptor describing Manufacturer
			devDesc[15] = 0x02; //	iProduct			- Index of string descriptor describing Product
			devDesc[16] = 0x03; //	iSerialNumber		- Index of string descriptor describing Serial number
			devDesc[17] = 0x01; //	bNumConfigurations	- Number of configurations

			configDesc_num=0;

			//Configuration Descriptor
			configDesc[configDesc_num++]  = 0x09; // bLength				- Descriptor length
			configDesc[configDesc_num++]  = 0x02; // bDescriptorType		- Descriptor Type
			configDesc[configDesc_num++]  = 0x20; // wTotalLength (LSB) - Total Data length for the configuration,
			configDesc[configDesc_num++]  = 0x00; // wTotalLength (MSB) - includes all descriptors for this configuration
			configDesc[configDesc_num++]  = 0x02; // bNumInterfaces 	- Number of interfaces this configuration supports
			configDesc[configDesc_num++]  = 0x02; // bConfigurationValue	- The Value that should be used to select this configuration
			configDesc[configDesc_num++]  = 0x04; // iConfiguration 	- Index of string descriptor describing this configuration	/////
			configDesc[configDesc_num++]  = 0xc0; // bmAttributes			- bit6: Self-Powered, bit5: RemoteWakeup
			configDesc[configDesc_num++]  = 0xfa; // MaxPower				- Maximum power consumption for this configuration (mA)

			configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x00; // bInterfaceNumber      ------- for Diag
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x08; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = DiagTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = DiagRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

			/* INTERFACE ASSOCIATION DESCRIPTOR */
            //Interface Descriptor
            configDesc[configDesc_num++] = 0x09; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x04; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = 0x06; // bInterfaceNumber      ------- for Sulog
            configDesc[configDesc_num++] = 0x00; // bAlternateSetting     - The value to select alternate setting of this interface
            configDesc[configDesc_num++] = 0x02; // bNumEndpoints           - Number endpoints  used by this interface
            configDesc[configDesc_num++] = 0xff; // bInterfaceClass       - Class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceSubClass    - Sub class of this Interface
            configDesc[configDesc_num++] = 0x00; // bInterfaceProtocol    - Protocol of this Interface
            configDesc[configDesc_num++] = 0x09; // iInterface          - Index of string descriptor describing this Interface


            /*
            07  05 02 02 40  .......@                12.2.80
            00 00
            bendpointAddress=02  (OUT)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */

            configDesc[configDesc_num++] = 0x07; //
            configDesc[configDesc_num++] = 0x05; //
            configDesc[configDesc_num++] = SulogTxEP; //bendpointAddress for diag tx
            configDesc[configDesc_num++] = 0x02; //bmAttributes
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x00; //
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

            /*
            07 05  82 02 40 00  ......@.                12.2.88
            00
            bendpointAddress=82  (IN)  bmAttributes=02  Bulk mode        wMaxPacketSize=40 00     bInterval=   00
            */
            configDesc[configDesc_num++] = 0x07; // bLength             - Descriptor length
            configDesc[configDesc_num++] = 0x05; // bDescriptorType     - Descriptor Type
            configDesc[configDesc_num++] = SulogRxEP; // bEndpointAddress        - Endpoint Address & Direction for diag Rx
            configDesc[configDesc_num++] = 0x02; // bmAttributes          - BULK,ISO,Interrupt
            PacketSizeOffset[PacketSizeOffsetNum++] = configDesc_num;
#ifdef MV_USB2_FULL_SPEED_MODE
            configDesc[configDesc_num++] = 0x40; // wMaxPacketSize (LSB)  - Max packet size
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize (MSB)
#else
            configDesc[configDesc_num++] = 0x00; // wMaxPacketSize
            configDesc[configDesc_num++] = 0x02; //
#endif
            configDesc[configDesc_num++] = 0x00; // bInterval

			configDesc[2]=configDesc_num & 0xFF;
			configDesc[3]=(configDesc_num>>8) & 0xFF;
			break;
		}

        default:
        {
            ASSERT(0);
            break;
        }
    }

    qualifDesc[0] =  sizeof(qualifDesc);
	qualifDesc[1] =  0x06;     /* bDescType This is a DEVICE Qualifier descr */
	qualifDesc[2] =  0x00;     /* bcdUSB USB revision 2.0 */
	qualifDesc[3] =  0x02; 	   /* bcdUSB USB revision 2.0 */
	qualifDesc[4] =  0x00;     /* bDeviceClass */
	qualifDesc[5] =  0x00;	   /* bDeviceSubClass */
	qualifDesc[6] =  0x00;     /* bDeviceProtocol*/
	qualifDesc[7] =  0x40;     /* bMaxPacketSize0 */
	qualifDesc[8] =  0x01;     /* bNumConfigurations */
 	qualifDesc[9] =  0x00;

    ASSERT(PacketSizeOffsetNum <= 20);

    dev_desc_length     = devDesc[0];       //device desc length
    config_desc_length  = configDesc[3];    //config descriptor length
    config_desc_length  *= 256;    //config descriptor length
    config_desc_length  += configDesc[2];    //config descriptor length

	qualif_desc_length 	= qualifDesc[0];

	/* Other Speed Desciptore relevant only for HW supporting USB2.0  */
	memcpy(otherSpeedDesc, configDesc, config_desc_length);

	/* Set bDescType to "Other speed configuration" */
	otherSpeedDesc[1] =  0x07;

	other_speed_desc_length = config_desc_length;

    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_DEVICE, devDesc, dev_desc_length, 0);
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_CONFIGURATION , configDesc, config_desc_length, 0);
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_QUALIFIER, qualifDesc, qualif_desc_length, 0);
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_SPEED, otherSpeedDesc, other_speed_desc_length, 0);
/*
    USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_SPEED, otherSpeedDesc, other_speed_desc_length, 0);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescLang, sizeof(strDescLang), 0);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescManufacturer, sizeof(strDescManufacturer), 1);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescProduct, sizeof(strDescProduct), 2);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescCompositeProduct, sizeof(strDescCompositeProduct), 3);
	USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescSerialNumber, sizeof(strDescSerialNumber), 4);
*/

    switch(desc)
    {
        case USB_GENERIC_MIFI_DESCRIPTOR:
        case USB_MARVELL_MIFI_DESCRIPTOR:
        case USB_ASR_MIFI_DESCRIPTOR:
		case USB_RNDIS_ONLY_DESCRIPTOR:
		case USB_MODEM_ONLY_DESCRIPTOR:
		case USB_MODEM_DIAG_DESCRIPTOR:
        {
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescManufacturer, sizeof(strDescManufacturer), 1);//iManufacturer
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 2);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileNumber, sizeof(strMobileNumber), 3);//iSerialNumber
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 4);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileRNDIS, sizeof(strMobileRNDIS), 5);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileRNDIS, sizeof(strMobileRNDIS), 6);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 7);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDiag, sizeof(strMobileDiag), 8);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileSulog, sizeof(strMobileSulog), 9);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 10);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileAT, sizeof(strMobileAT), 11);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileAT, sizeof(strMobileAT), 12);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMass, sizeof(strMobileMass), 13);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescSerialNumber, sizeof(strDescSerialNumber), 14);
            break;
        }
		case USB_MBIM_DESCRIPTOR:
        {
			USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescASRManufacturer, sizeof(strDescASRManufacturer), 0);//iManufacturer
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescASRManufacturer, sizeof(strDescASRManufacturer), 1);//iManufacturer
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 2);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileNumber, sizeof(strMobileNumber), 3);//iSerialNumber
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 4);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMBIM, sizeof(strMobileMBIM), 5);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMBIM, sizeof(strMobileMBIM), 6);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 7);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDiag, sizeof(strMobileDiag), 8);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDiag, sizeof(strMobileDiag), 9);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 10);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileAT, sizeof(strMobileAT), 11);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileAT, sizeof(strMobileAT), 12);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMass, sizeof(strMobileMass), 13);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescSerialNumber, sizeof(strDescSerialNumber), 14);
            break;
        }

        case USB_GENERIC_MOD_ECM_DESCRIPTOR:
        case USB_MARVELL_ECM_DESCRIPTOR:
        {
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescASRManufacturer, sizeof(strDescASRManufacturer), 0);//iManufacturer
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescASRManufacturer, sizeof(strDescASRManufacturer), 1);//iManufacturer
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 2);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileNumber, sizeof(strMobileNumber), 3);//iSerialNumber
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 4);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileECM, sizeof(strMobileECM), 5);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileECM, sizeof(strMobileECM), 6);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 7);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDiag, sizeof(strMobileDiag), 8);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDiag, sizeof(strMobileDiag), 9);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileDevice, sizeof(strMobileDevice), 10);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileAT, sizeof(strMobileAT), 11);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileAT, sizeof(strMobileAT), 12);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMass, sizeof(strMobileMass), 13);
            //modify by qgwang 20140422, use random mac address
            Ecm_getstrECMMACADDR(strECMMACADDR, sizeof(strECMMACADDR));
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strECMMACADDR, sizeof(strECMMACADDR), 14 );

            break;
        }

        case USB_CDROM_ONLY_DESCRIPTOR:
        case USB_CDROM_DIAG_DESCRIPTOR:
        {
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescManufacturer, sizeof(strDescManufacturer), 1);//iManufacturer
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strDescProduct, sizeof(strDescProduct), 2);//iProduct
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileNumber, sizeof(strMobileNumber), 3);//iSerialNumber
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMass, sizeof(strMobileMass), 4);
            USBDeviceSetDescriptor( USB_DESCRIPTOR_TYPE_STRING , strMobileMass, sizeof(strMobileMass), 5);
            break;
        }

        default:
        {			
            ASSERT(0);
            break;
        }
    }
}

UINT32 get_current_usb_app_mask(void)
{
	mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    return pUsbDesInfo->CurrentDesc;

}

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
void USB2ReEnumerate(PlatformUsbDescType Desc)
{
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    if( USB_MAX_DESCRIPTOR_TYPE == Desc)
    {
        /* Use the default descriptor. */
        pUsbDesInfo->ReConfigDesc = pUsbDesInfo->DefaultDesc;
    }
    else
    {
        /* Use new descriptor. */
        pUsbDesInfo->ReConfigDesc = Desc;
    }

    /* If the descriptor is same to the current configuration, do nothing. */
    if(pUsbDesInfo->ReConfigDesc == pUsbDesInfo->CurrentDesc)
    {
        return;
    }

    USB_TRACE("USB2ReEnumerate 0x%x", pUsbDesInfo->ReConfigDesc);
	uart_printf("ENU 0x%x", pUsbDesInfo->ReConfigDesc);
    /* Usb simulate plug out/in */
    USB2MgrDeviceUnplugPlug();
}

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
char USBIsConnected(void)
{
    if(((*(volatile unsigned long *)(0xD4208184))&0x01)!=0)
    {
        /* USB is Connected */
        return 1;
    }
    else
    {
        /* USB is not Connected */
        return 0;
    }
}

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
void USB2ConfigureCdromDescriptor(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    if((pUsbDrvConfig->auto_install == USB_AUTO_INSTALL_ENABLE)&&
       (pUsbDesInfo->CurrentDesc != USB_CDROM_ONLY_DESCRIPTOR))
    {
        pUsbDesInfo->ReConfigDesc = USB_CDROM_ONLY_DESCRIPTOR;
        USB2MgrUpdateDescriptor(pUsbDesInfo->ReConfigDesc, pUsbDesInfo->UsbMode);
    }
}

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
void USB2ProcessGetConfigDescRequest(UINT16 value)
{
/*
    OS_STATUS status;
    UINT32 usb_app_mask;
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    uart_printf("%s: config index %d", __FUNCTION__, value);

    switch(value)
    {
        case 0:
        {
            usb_app_mask = 29;
            break;
        }

        default:
        {
            usb_app_mask = 28;
            break;
        }
    }

    if(pUsbDesInfo->CurrentDesc != usb_app_mask)
    {
        USB2MgrUpdateDescriptor(usb_app_mask, USB_NORMAL_MODE);
    }
*/

}

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
mvUsbDescriptorInfo *USB2GetDescriptorInfo(void)
{
    return &UsbDescriptorInfo;
}

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
BOOL USB2IsAutoInstallEnable(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();

    if(pUsbDrvConfig->auto_install == USB_AUTO_INSTALL_ENABLE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

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
BOOL USB2IsCdromOnlyDescriptor(void)
{
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    if(pUsbDesInfo->CurrentDesc == USB_CDROM_ONLY_DESCRIPTOR)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

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
BOOL USB2MgrIsDeviceControllerEnabled (void)
{
    return USBDeviceIsControllerEnabled();
}


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
//ICAT EXPORTED FUNCTION - USB,USBMgr,Unplug
void USB2MgrDeviceUnplug(void)
{
    //ErrorLogPrintf("USB Unplug");

    if ( !USB2MgrIsDeviceControllerEnabled() )  // YSS - no need to simulate plug out if controller is not activated.
         return;
#ifndef PHS_SW_TAVORP_YARDEN_AP
    USBDeviceSimulatePlug(USBD_SimulatePlugE_OUT);
#endif

    if(!SysIsAssert())
    {
        USB2UnPlugHandler();
    }
}

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
//ICAT EXPORTED FUNCTION - USB,USBMgr,Plugin
void USB2MgrDevicePlugIn(void)
{
    //ErrorLogPrintf("USB PlugIn");

    if ( USB2MgrIsDeviceControllerEnabled() )  // YSS - when trying to activate controler when it's already activated an assert will be trrigered.
         return;
#ifndef PHS_SW_TAVORP_YARDEN_AP //Modification to force USB start utill Modem Start
    //before plugging register again for all notifications
    //if(USB2MgrInitUSBRegistration() != USBMGR_RC_OK)
    //{
    //    ASSERT(0);
    //}

    USBDeviceSimulatePlug(USBD_SimulatePlugE_IN);
#endif

#ifdef PHS_SW_TAVORP_YARDEN_AP //Modification to force USB start utill Modem Start
{
	extern void bspUsbActivate(void);
	bspUsbActivate();
}
#endif
}
extern volatile UINT32 diagTxFail;

volatile UINT32 USBUnplugCallerAddress = 0;
volatile UINT32 USBUnplugCount = 0;
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
//ICAT EXPORTED FUNCTION - USB,USBMgr,UnplugPlug
void USB2MgrDeviceUnplugPlug(void)
{
    extern void phyreg_dump_before_reenum(void);
    phyreg_dump_before_reenum();
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    USBUnplugCallerAddress = __return_address();
    USBUnplugCount++;

	//uart_printf("uP %x\r\n", __return_address());
	ErrorLogPrintf("USB plug out/in, 0x%x", __return_address());
    //may be called in interrupt context,should not uart printf
    
#ifdef YMODEM_EEH_DUMP
    /* Don't simulate USB plug when Ymodem is enable in EEH. */
	if ((useYmodem() == 1) && (SysIsAssert() == 1))
	{
		return;
    }
#endif
    /* Usb simulate plug out. */
	USB2MgrDeviceUnplug();

	//ErrorLogPrintf("usb->%d", pUsbDesInfo->ReConfigDesc);

    /* Update Usb descriptor. */
	USB2MgrUpdateDescriptor(pUsbDesInfo->ReConfigDesc, pUsbDesInfo->UsbMode);

	/* Usb simulate plug in. */
	USB2MgrDevicePlugIn();
	diagTxFail = 0;
}

typedef enum
{
	USB_CHARGER_ONLY,
	USB_STORAGE_ONLY,
}USBDeviceType;
USBDeviceType deviceSelect = USB_CHARGER_ONLY;
#define USB_SELECT_EVENT 0x01
extern OSAFlagRef usb_connect_flag;
//ICAT EXPORTED FUNCTION - HW_PLAT,utilities,USBDeviceSelect1
void USBDeviceSelect1(void)
{
//	uart_printf("DeviceSelect:%d\r\n",DeviceSelect);
	deviceSelect = USB_CHARGER_ONLY;
	OSAFlagSet(usb_connect_flag, USB_SELECT_EVENT,OSA_FLAG_OR);
}
//ICAT EXPORTED FUNCTION - HW_PLAT,utilities,USBDeviceSelect2
void USBDeviceSelect2(USBDeviceType DeviceSelect)
{
	//uart_printf("DeviceSelect:%d\r\n",DeviceSelect);
	deviceSelect = USB_STORAGE_ONLY;
	OSAFlagSet(usb_connect_flag, USB_SELECT_EVENT,OSA_FLAG_OR);
}


//ICAT EXPORTED FUNCTION - HW_PLAT,utilities,USBDeviceSelect
void USBDeviceSelect(USBDeviceType DeviceSelect)
{
	deviceSelect = DeviceSelect;
	OSAFlagSet(usb_connect_flag, USB_SELECT_EVENT,OSA_FLAG_OR);
}
extern volatile UINT32     usb_shutdown;

void USBSelectTask(void *argv)
{
	OSA_STATUS status;
    UINT32 flags;
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();

	while(1)
	{

		status = OSAFlagWait(usb_connect_flag, USB_SELECT_EVENT, OSA_FLAG_OR_CLEAR, &flags, OSA_SUSPEND);
		ASSERT(status == OS_SUCCESS);
		if(!usb_shutdown)
		{
			switch(deviceSelect)
			{
				case USB_CHARGER_ONLY:
				    uart_printf("USB_CHARGER_ONLY\r\n");
					pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
					USB2MgrDeviceUnplugPlug_plus(USB_DIAG_ONLY_DESCRIPTOR);
					break;

				case USB_STORAGE_ONLY:
				{
                    //extern BOOL massStorageEnabled(void); 
                    //extern void udisk_diag_fd_close(void);
                    //extern void ffst_print_all_file();
				    //uart_printf("USB_STORAGE_ONLY\r\n");
                    //udisk_diag_fd_close();
#ifndef SULOG_DISABLE
                    //extern void udisk_sulog_fd_close(void);
                    //udisk_sulog_fd_close();       
#endif
                    //ffst_print_all_file();
                    void udisk_unmount(void);
                    udisk_unmount();

					pUsbDrvConfig->mass_storage = MASS_STORAGE_ENABLE;
					USB2MgrDeviceUnplugPlug_plus(USB_CDROM_DIAG_DESCRIPTOR);
					break;
				}
			}
		}
	}
}

void USB2MgrDeviceUnplugPlug_plus(PlatformUsbDescType desc)
{
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
	USB2MgrDeviceUnplug();
	//ErrorLogPrintf("usb->%d", pUsbDesInfo->ReConfigDesc);

    /* Update Usb descriptor. */
	USB2MgrUpdateDescriptor(desc, pUsbDesInfo->UsbMode);

	/* Usb simulate plug in. */
	USB2MgrDevicePlugIn();
	diagTxFail = 0;
}

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
void USB2MgrDeviceUnplugPlug_SD(void)
{
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
    uart_printf("USB2MgrDeviceUnplugPlug_SD\r\n");
    /* Reset current usb descriptor */
    pUsbDesInfo->CurrentDesc = USB_MAX_DESCRIPTOR_TYPE;
    pUsbDrvConfig->mass_storage = MASS_STORAGE_ENABLE;

    /* Usb simulate plug out/in */
    USB2MgrDeviceUnplugPlug();
}

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
void USB2MgrDeviceUnplugPlug_WebDav(unsigned char enable)
{
    mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();

    /* Reset current usb descriptor */
    pUsbDesInfo->CurrentDesc = USB_MAX_DESCRIPTOR_TYPE;
	if(enable)
	{
    	pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
	}
	else
	{
		pUsbDrvConfig->mass_storage = MASS_STORAGE_ENABLE;
	}

    /* Usb simulate plug out/in */
    USB2MgrDeviceUnplugPlug();
}

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
BOOL USB2MgrMassStorageEnable(void)
{
    Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();

    if (pUsbDrvConfig->mass_storage == MASS_STORAGE_ENABLE)
    {
        return TRUE;
	}
	else
	{
		return FALSE;
	}
}
#if 0

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
BOOL USB2MgrChangeUsbMode(char mode)
{
    SetUsbMode(mode);

    uart_printf("USB2MgrChangeUsbMode 0x%x", mode);

    return TRUE;
}
#endif
#ifdef YMODEM_EEH_DUMP
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
void USBReEnumIndump(void)
{
	mvUsbDescriptorInfo *pUsbDesInfo = USB2GetDescriptorInfo();

    /* Use new descriptor. */
    pUsbDesInfo->ReConfigDesc = USB_MODEM_ONLY_DESCRIPTOR;

 	/* If the descriptor is same to the current configuration, do nothing. */
    if(pUsbDesInfo->ReConfigDesc == pUsbDesInfo->CurrentDesc)
    {
        return;
    }

    /* Usb simulate plug out. */
	USB2MgrDeviceUnplug();

	//craneCommPMPowerdownUsbPhy();

    /* Cancel modem ep transfer. */
    _usb_dci_vusb20_free_ep_transfer(MODEM_TX_ENDPOINT, 1, NULL);
    _usb_dci_vusb20_free_ep_transfer(MODEM_RX_ENDPOINT, 0, NULL);
    _usb_dci_vusb20_free_ep_transfer(MODEM_CTRL_ENDPOINT, 1, NULL);

	uart_printf("usb->%d\n", pUsbDesInfo->ReConfigDesc);

	mvUsbChangeAtInterface();

	Usb_DriverS *pUsbDrvConfig = GetUsbDriverConfig();
	pUsbDrvConfig->auto_install = USB_AUTO_INSTALL_DISABLE;
	pUsbDrvConfig->mass_storage = MASS_STORAGE_DISABLE;
	pUsbDrvConfig->os_detect = USB_OS_DETECT_DISABLE;

    /* Update Usb descriptor. */
	USB2MgrUpdateDescriptor(pUsbDesInfo->ReConfigDesc, pUsbDesInfo->UsbMode);

	uart_printf("USBReEnumIndump1\n");
	craneCommPMPowerupUsbPhy();
	/* Usb simulate plug in. */
	USB2MgrDevicePlugIn();

	uart_printf("USBReEnumIndump2\n");

    extern volatile uint_32 pm_usb_busy;

	pm_usb_busy = 0;
	do
	{
		USB2DriverInterruptloopHandler();
	} while(pm_usb_busy == 0);
	usb_shutdown = 0;
	uart_printf("USBReEnumIndump3\n");
}
#endif
