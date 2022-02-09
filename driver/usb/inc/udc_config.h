/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*******************************************************************************
 *                      M O D U L E     B O D Y
 *******************************************************************************
 *  COPYRIGHT (C) 2003, 2004 Intel Corporation.
 *
 *  This software as well as the software described in it is furnished under
 *  license and may only be used or copied in accordance with the terms of the
 *  license. The information in this file is furnished for informational use
 *  only, is subject to change without notice, and should not be construed as
 *  a commitment by Intel Corporation. Intel Corporation assumes no
 *  responsibility or liability for any errors or inaccuracies that may appear
 *  in this document or any software that may be provided in association with
 *  this document.
 *  Except as permitted by such license, no part of this document may be
 *  reproduced, stored in a retrieval system, or transmitted in any form or by
 *  any means without the express written consent of Intel Corporation.
 *
 *******************************************************************************
 *
 * Title:        UDC DRIVER - USB Device Controller header file
 *
 * Filename:     udc_config.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains the ENDPOITS configuration table needed by
 *               the UDC driver
 *
 * Notes:
 ******************************************************************************/

#if !defined(_UDC_CONFIG_H_)
#define      _UDC_CONFIG_H_
#include "common.h"
#include "usb_device_config.h"
//#include "global_types.h"
#include "udc_driver.h"
#include "udc_types.h"
#include "udc_hw.h"
#include "usb_macro.h"
//#include "usb_device.h"

/* UDC Endpoint Configuration table:
 * Below is a table for the UDC Endpoint configuration.
 * This table should be handled carefully or error in configuration will occur.
 * Note: ENDPOINT_0 (control endpoint) is on list just for convenience - user should NOT change it
 */
/* UDC Endpoint Configuration Structure - even though this should be in udc_types.h there is a compilation
 * due to crossed header include. Therefore this structure placed here */
typedef struct UDC_EndpointConfigS_tag
{
    UINT8                       usbConfigNum;               /* Usb Configuration Number: (1-3) */
    UINT8                       usbInterfaceNum;            /* Usb Interface Number: (1-7) */
    UINT8                       usbIntAltSettingsNum;       /* Usb Interface Alternate Settings Number: (1-7) */
    UINT8                       usbEndpointNum;             /* Usb Endpoint Number: (1-15) */
    UDC_EndpointTypeE           usbEndpointType;            /* Usb Endpoint type: Bulk, Iso, Interrupt */
    UDC_EndpointDirE            usbEndpointDir;             /* Usb Endpoint direction: IN, OUT */
    UINT16                      usbMaxPacketSize;           /* Max. Packet Size: (1-1023) */
    UDC_EndpointDoubleBufE      doubleBuffEnabled;          /* Double Buffering Enabled if set to one */
    UDC_EndpointEnableE         endpointEnabled;            /* Endpoint Enabled if set to one */
}UDC_EndpointConfigS;

#if defined(_UDC_TYPES_)

const UDC_EndpointConfigS _u1dcDefaultEndpointConfig[UDC_TOTAL_ENDPOINTS] =
{
/*  UDC        Config  Int   Alt.     USB         Endpoint          Endpoint        Max.         Double     Endpoint
   Endpoint     Num.   Num.  Set.   Endpoint        Type            Direction      Packet        Buffer      Enable
                                      Num                                           Size         Enable                  */
/*ENDPOINT_0   { 0,     0,    0,  0          , UDC_EP_CONTROL  , 0xFF          , UDC_MPS_16  , UDC_DB_ENA, UDC_EP_ENA },   ENDPOINT_0*/
/*ENDPOINT_0*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_0*/
/*ENDPOINT_A*/ { 1,     0,    0,  1          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA },  /*ENDPOINT_A*/
/*ENDPOINT_B*/ { 1,     0,    0,  2          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_B*/
/*ENDPOINT_C*/ { 1,     0,    0,  3          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_C*/
/*ENDPOINT_D*/ { 1,     0,    0,  4          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_D*/
/*ENDPOINT_E*/ { 1,     0,    0,  5          , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_E*/
/*ENDPOINT_F*/ { 1,     0,    0,  6          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA },  /*ENDPOINT_F*/
/*ENDPOINT_G*/ { 1,     0,    0,  7          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_G*/
/*ENDPOINT_H*/ { 1,     0,    0,  8          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_H*/
/*ENDPOINT_I*/ { 1,     0,    0,  9          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_I*/
/*ENDPOINT_J*/ { 1,     0,    0,  0xA        , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_J*/
/*ENDPOINT_K*/ { 1,     0,    0,  0xB        , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_K*/
/*ENDPOINT_L*/ { 1,     0,    0,  0xC        , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_L*/
/*ENDPOINT_M*/ { 1,     0,    0,  0xD        , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_M*/
/*ENDPOINT_N*/ { 1,     0,    0,  0xE        , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_64  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_N*/
/*ENDPOINT_P*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_P*/
/*ENDPOINT_Q*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_Q*/
/*ENDPOINT_R*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_R*/
/*ENDPOINT_S*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_S*/
/*ENDPOINT_T*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_T*/
/*ENDPOINT_U*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_U*/
/*ENDPOINT_V*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_V*/
/*ENDPOINT_W*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_W*/
/*ENDPOINT_X*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }  /*ENDPOINT_X*/
};

const UDC_EndpointConfigS _u2dcDefaultEndpointConfig[UDC_TOTAL_ENDPOINTS] =
{
/*  UDC        Config  Int   Alt.     USB         Endpoint          Endpoint        Max.         Double     Endpoint
   Endpoint     Num.   Num.  Set.   Endpoint        Type            Direction      Packet        Buffer      Enable
                                      Num                                           Size         Enable                  */
/*ENDPOINT_0   { 0,     0,    0,  0          , UDC_EP_CONTROL  , 0xFF          , UDC_MPS_16  , UDC_DB_ENA, UDC_EP_ENA },   ENDPOINT_0*/
/*ENDPOINT_0*/ { 0,     0,    0,  0          , 0               , 0             , UDC_MPS_64  , 0         , 0          }, /*ENDPOINT_0*/
/*ENDPOINT_A*/ { 1,     0,    0,  1          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA },/*ENDPOINT_A*/
/*ENDPOINT_B*/ { 1,     0,    0,  2          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_B*/
/*ENDPOINT_C*/ { 1,     0,    0,  3          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_C*/
/*ENDPOINT_D*/ { 1,     0,    0,  4          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA },/*ENDPOINT_D*/
/*ENDPOINT_E*/ { 1,     0,    0,  5          , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_E*/
/*ENDPOINT_F*/ { 1,     0,    0,  6          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_F*/
/*ENDPOINT_G*/ { 1,     0,    0,  7          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA },/*ENDPOINT_G*/
/*ENDPOINT_H*/ { 1,     0,    0,  8          , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_H*/
/*ENDPOINT_I*/ { 1,     0,    0,  9          , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_I*/
/*ENDPOINT_J*/ { 1,     0,    0,  0xA        , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_J*/
/*ENDPOINT_K*/ { 1,     0,    0,  0xB        , UDC_EP_INTERRUPT, UDC_EP_DIR_IN , UDC_MPS_16  , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_K*/
/*ENDPOINT_L*/ { 1,     0,    0,  0xC        , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_L*/
/*ENDPOINT_M*/ { 1,     0,    0,  0xD        , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_M*/
/*ENDPOINT_N*/ { 1,     0,    0,  0xE        , UDC_EP_BULK     , UDC_EP_DIR_IN , UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_N*/
/*ENDPOINT_P*/ { 1,     0,    0,  0xF        , UDC_EP_BULK     , UDC_EP_DIR_OUT, UDC_MPS_512 , UDC_DB_DIS, UDC_EP_ENA }, /*ENDPOINT_P*/
/*ENDPOINT_Q*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_Q*/
/*ENDPOINT_R*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_R*/
/*ENDPOINT_S*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_S*/
/*ENDPOINT_T*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_T*/
/*ENDPOINT_U*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_U*/
/*ENDPOINT_V*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_V*/
/*ENDPOINT_W*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }, /*ENDPOINT_W*/
/*ENDPOINT_X*/ { 0,     0,    0,  0          , 0               , 0             , 0           , 0         , 0          }  /*ENDPOINT_X*/
};
#else /*_UDC_TYPES_*/
extern const UDC_EndpointConfigS _u1dcDefaultEndpointConfig[UDC_TOTAL_ENDPOINTS];
extern const UDC_EndpointConfigS _u2dcDefaultEndpointConfig[UDC_TOTAL_ENDPOINTS];
#endif /*_UDC_TYPES_*/


#endif /*_UDC_CONFIG_H_*/
