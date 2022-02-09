#ifndef __mvUsbCh9_h__
#define __mvUsbCh9_h__
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
***   This file contains definitions for standard device requests,
***
**************************************************************************
**END*********************************************************/

#include "udc2_hw.h"
/*----------------------------------------------------------------**
** Chapter 9.4 Standard Device Requests -- all devices            **
** See Table 9-3 p. 250 of USB 2.0 spec for combinations          **
**   of request type bitfields with requests, WVALUE, WINDEX etc. **
**----------------------------------------------------------------*/
#define REQ_RECIP_MASK          0x1f
#define REQ_RECIP_DEVICE        0x00
#define REQ_RECIP_INTERFACE     0x01
#define REQ_RECIP_ENDPOINT      0x02
#define REQ_RECIP_OTHER         0x03

/* Also for class requests set the following bit */
#define REQ_TYPE_OFFSET         5
#define REQ_TYPE_MASK			(0x03 << 5)
#define REQ_TYPE_STANDARD		(0x00 << 5)
#define REQ_TYPE_CLASS			(0x01 << 5)
#define REQ_TYPE_VENDOR			(0x02 << 5)
#define REQ_TYPE_RESERVED		(0x03 << 5)

/* Combine one of the 3 above with one of the following 2 */
#define REQ_DIR_OFFSET         7
#define REQ_DIR_IN             (1 << REQ_DIR_OFFSET)
#define REQ_DIR_OUT            (0 << REQ_DIR_OFFSET)

/* Standard USB requests, see Chapter 9 */
#define REQ_GET_STATUS          0
#define REQ_CLEAR_FEATURE       1
#define REQ_SET_FEATURE         3
#define REQ_SET_ADDRESS         5
#define REQ_GET_DESCRIPTOR      6
#define REQ_SET_DESCRIPTOR      7
#define REQ_GET_CONFIGURATION   8
#define REQ_SET_CONFIGURATION   9
#define REQ_GET_INTERFACE       10
#define REQ_SET_INTERFACE       11
#define REQ_SYNCH_FRAME         12

#define DESC_TYPE_OFFSET         16
#define DESC_TYPE_MASK           0xFF
#define DESC_TYPE_DEVICE         0x01
#define DESC_TYPE_CONFIG         0x02
#define DESC_TYPE_STRING         0x03
#define DESC_TYPE_INTERFACE      0x04
#define DESC_TYPE_ENDPOINT       0x05
#define DESC_TYPE_QUALIFIER      0x06
#define DESC_TYPE_OTHER_SPEED    0x07
#define DESC_TYPE_INTF_POWER     0x08
#define DESC_TYPE_OTG            0x09


/*******************************************************************
**
** Values specific to CLEAR FEATURE commands (must go to common.h later)
*/

#define  ENDPOINT_HALT          0
#define  DEVICE_SELF_POWERED    0
#define  DEVICE_REMOTE_WAKEUP   1
#define  DEVICE_TEST_MODE       2


/* States of device instances on the device list */

/* initial device state */
#define  DEVSTATE_INITIAL        0x00

/* device descriptor [0..7]*/
#define  DEVSTATE_DEVDESC8       0x01

/* address set */
#define  DEVSTATE_ADDR_SET       0x02

/* full device descriptor */
#define  DEVSTATE_DEV_DESC       0x03

/* config descriptor [0..7] */
#define  DEVSTATE_GET_CFG9       0x04

/* config set */
#define  DEVSTATE_SET_CFG        0x05

/* full config desc. read in */
#define  DEVSTATE_CFG_READ       0x06

/* application callbacks */
#define  DEVSTATE_APP_CALL       0x07

/* Select interface done */
#define  DEVSTATE_SET_INTF       0x08

#define  DEVSTATE_ENUM_OK        0x09

#define  DEVSTATE_CHK_OTG        0x0A

/* Event codes for attach/detach etc. callback */
#define  USB_ATTACH_EVENT        1   /* device attach */
#define  USB_DETACH_EVENT        2   /* device detach */
#define  USB_CONFIG_EVENT        3   /* device reconfigured */
#define  USB_INTF_EVENT          4   /* device interface selected */

typedef unsigned char uint_8;


/************* Standard Descriptors*****************/
typedef struct usb_device_descriptor
{
   uint_8   bLength;          /* Descriptor size in bytes = 18 */
   uint_8   bDescriptorType;  /* DEVICE descriptor type = 1 */
   uint_8   bcdUSD[2];        /* USB spec in BCD, e.g. 0x0200 */
   uint_8   bDeviceClass;     /* Class code, if 0 see interface */
   uint_8   bDeviceSubClass;  /* Sub-Class code, 0 if class = 0 */
   uint_8   bDeviceProtocol;  /* Protocol, if 0 see interface */
   uint_8   bMaxPacketSize;   /* Endpoint 0 max. size */
   uint_8   idVendor[2];      /* Vendor ID per USB-IF */
   uint_8   idProduct[2];     /* Product ID per manufacturer */
   uint_8   bcdDevice[2];     /* Device release # in BCD */
   uint_8   iManufacturer;    /* Index to manufacturer string */
   uint_8   iProduct;         /* Index to product string */
   uint_8   iSerialNumber;    /* Index to serial number string */
   uint_8   bNumConfigurations; /* Number of possible configurations */ 
} DEVICE_DESCRIPTOR, _PTR_ DEVICE_DESCRIPTOR_PTR;  

typedef struct usb_configuration_descriptor
{
   uint_8   bLength;          /* Descriptor size in bytes = 9 */
   uint_8   bDescriptorType;  /* CONFIGURATION type = 2 or 7 */
   uint_8   wTotalLength[2];  /* Length of concatenated descriptors */
   uint_8   bNumInterfaces;   /* Number of interfaces, this config. */
   uint_8   bConfigurationValue;  /* Value to set this config. */ 
   uint_8   iConfig;          /* Index to configuration string */
   uint_8   bmAttributes;     /* Config. characteristics */
   #define  CONFIG_RES7       (0x80)  /* Reserved, always = 1 */
   #define  CONFIG_SELF_PWR   (0x40)  /* Self-powered device */
   #define  CONFIG_WAKEUP     (0x20)  /* Remote wakeup */
   uint_8   bMaxPower;        /* Max.power from bus, 2mA units */
} CONFIGURATION_DESCRIPTOR, _PTR_ CONFIGURATION_DESCRIPTOR_PTR;  

typedef struct usb_interface_descriptor
{
   uint_8   bLength;          /* Descriptor size in bytes = 9 */
   uint_8   bDescriptorType;  /* INTERFACE descriptor type = 4 */
   uint_8   bInterfaceNumber; /* Interface no.*/
   uint_8   bAlternateSetting;  /* Value to select this IF */
   uint_8   bNumEndpoints;    /* Number of endpoints excluding 0 */
   uint_8   bInterfaceClass;  /* Class code, 0xFF = vendor */
   uint_8   bInterfaceSubClass;  /* Sub-Class code, 0 if class = 0 */
   uint_8   bInterfaceProtocol;  /* Protocol, 0xFF = vendor */
   uint_8   iInterface;       /* Index to interface string */
} INTERFACE_DESCRIPTOR, _PTR_ INTERFACE_DESCRIPTOR_PTR;  

typedef struct usb_endpoint_descriptor
{
   uint_8   bLength;          /* Descriptor size in bytes = 7 */
   uint_8   bDescriptorType;  /* ENDPOINT descriptor type = 5 */
   uint_8   bEndpointAddress; /* Endpoint # 0 - 15 | IN/OUT */
   #define  IN_ENDPOINT    (0x80)   /* IN endpoint, device to host */
   #define  OUT_ENDPOINT   (0x00)   /* OUT endpoint, host to device */
   #define  ENDPOINT_MASK  (0x0F)   /* Mask endpoint # */
   uint_8   bmAttributes;     /* Transfer type */
   #define  CONTROL_ENDPOINT  (0x00)   /* Control transfers */
   #define  ISOCH_ENDPOINT    (0x01)   /* Isochronous transfers */
   #define  BULK_ENDPOINT     (0x02)   /* Bulk transfers */
   #define  IRRPT_ENDPOINT    (0x03)   /* Interrupt transfers */
   #define  EP_TYPE_MASK      (0x03)   /* Mask type bits */
   /* Following must be zero except for isochronous endpoints */
   #define  ISOCH_NOSYNC      (0x00)   /* No synchronization */
   #define  ISOCH_ASYNC       (0x04)   /* Asynchronous */
   #define  ISOCH_ADAPT       (0x08)   /* Adaptive */
   #define  ISOCH_SYNCH       (0x0C)   /* Synchrounous */
   #define  ISOCH_DATA        (0x00)   /* Data endpoint */
   #define  ISOCH_FEEDBACK    (0x10)   /* Feedback endpoint */
   #define  ISOCH_IMPLICIT    (0x20)   /* Implicit feedback */
   #define  ISOCH_RESERVED    (0x30)   /* Reserved */
   uint_8   wMaxPacketSize[2];   /* Bits 10:0 = max. packet size */
   /* For high-speed interrupt or isochronous only, additional
   **   transaction opportunities per microframe follow.*/
   #define  PACKET_SIZE_MASK     (0x7FF)  /* packet size bits */
   #define  NO_ADDITONAL      (0x0000)   /* 1 / microframe */
   #define  ONE_ADDITIONAL    (0x0800)   /* 2 / microframe */
   #define  TWO_ADDITIONAL    (0x1000)   /* 3 / microframe */
   #define  ADDITIONAL_MASK   (ONE_ADDITIONAL | TWO_ADDITIONAL)
   uint_8   iInterval;        /* Polling interval in (micro) frames */
} ENDPOINT_DESCRIPTOR, _PTR_ ENDPOINT_DESCRIPTOR_PTR;  

typedef struct usb_qualifier_descriptor
{
   uint_8   bLength;          /* Descriptor size in bytes = 10 */
   uint_8   bDescriptorType;  /* DEVICE QUALIFIER type = 6 */
   uint_8   bcdUSD[2];        /* USB spec in BCD, e.g. 0x0200 */
   uint_8   bDeviceClass;     /* Class code, if 0 see interface */
   uint_8   bDeviceSubClass;  /* Sub-Class code, 0 if class = 0 */
   uint_8   bDeviceProtocol;  /* Protocol, if 0 see interface */
   uint_8   bMaxPacketSize;   /* Endpoint 0 max. size */
   uint_8   bNumConfigurations; /* Number of possible configurations */
   uint_8   bReserved;        /* Reserved = 0 */ 
} QUALIFIER_DESCRIPTOR, _PTR_ QUALIFIER_DESCRIPTOR_PTR;  

/* Other-Config type 7 fields are identical to type 2 above */

/* Interface-Power descriptor  type 8 not used  in this version */

typedef struct usb_otg_descriptor
{
   uint_8   bLength;          /* Descriptor size in bytes = 9 */
   uint_8   bDescriptorType;  /* CONFIGURATION type = 2 or 7 */
   uint_8   bmAttributes;     /* OTG characteristics */
   #define  OTG_SRP_SUPPORT   (0x01)  /* Supports SRP */
   #define  OTG_HNP_SUPPORT   (0x02)  /* Supports HNP */
} OTG_DESCRIPTOR, _PTR_ OTG_DESCRIPTOR_PTR;  
                         
#endif /* __mvUsbCh9_h__ */

/* EOF */
