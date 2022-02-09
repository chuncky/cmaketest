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
 * Filename:     udc_types.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains structures/enums/types that needed by
 *               the UDC driver
 *
 * Notes:
 ******************************************************************************/

#if !defined(_UDC_TYPES_H_)
#define      _UDC_TYPES_H_

//#include "global_types.h"
#include "common.h"
#include "udc_driver.h"
//#include "udc_config.h"
#include "udc_hw.h"

#include "usb_device_config.h"

/*******************************************************************************
 *                      T Y P E S
 ******************************************************************************/




typedef void (*UDCDriverEndpointInterruptHandler)(void *, UINT32); /* void * = UDC_EndpointDataS * */

typedef enum
{
    UDC_UNLOAD_NO_ACTION = 0,
    UDC_UNLOAD_ZLP,
    UDC_UNLOAD_SHORT_PACKET,
    UDC_UNLOAD_BUFFER_FULL,
    UDC_UNLOAD_COMPLETE
}UDC_UnloadStateE;

typedef enum
{
    UDC_EP_CONTROL = 0,
    UDC_EP_ISO,
    UDC_EP_BULK,
    UDC_EP_INTERRUPT
}UDC_EndpointTypeE;

typedef enum
{
    UDC_EP_DIR_OUT = 0,
    UDC_EP_DIR_IN
}UDC_EndpointDirE;

typedef enum
{
    UDC_MPS_8    = 8,
    UDC_MPS_16   = 16,
    UDC_MPS_32   = 32,
    UDC_MPS_64   = 64,
    UDC_MPS_128  = 128,
    UDC_MPS_256  = 256,
    UDC_MPS_512  = 512,
    UDC_MPS_1023 = 1023,
    UDC_MPS_MAX  = 1023
}UDC_EndpointMpsE;

typedef enum
{
    UDC_DB_DIS = 0,
    UDC_DB_ENA
}UDC_EndpointDoubleBufE;

typedef enum
{
    UDC_EP_DIS = 0,
    UDC_EP_ENA
}UDC_EndpointEnableE;

typedef enum
{
    UDC_STALL_DISABLE = 0,
    UDC_STALL_IN_PROGRESS,
    UDC_STALL_COMPLETED
}UDC_EndpointStallE;

typedef enum
{
    UDC_HW_STATE_NONE = 0,
    UDC_HW_STATE_CONFIG_FAIL,
    UDC_HW_STATE_CONFIG_OK
    //UDC_HW_STATE_RESET_RCV,
    //UDC_HW_STATE_SET_CONFIG
}UDC_HardwareStateE;

typedef enum
{
    UDC_ZLP_AUTO = 0,
    UDC_ZLP_NEEDED,
	UDC_ZLP_NOT_NEEDED,
	UDC_ZLP_IGNORED
}UDC_ZLPStateE;

typedef enum
{
    UDC_EP_STATE_DEACTIVE = 0,
    UDC_EP_STATE_ACTIVE,
    UDC_EP_STATE_TX_ZLP_AUTO,
    UDC_EP_STATE_TX_ZLP_NEEDED,
    UDC_EP_STATE_TX_ZLP_NOT_NEEDED,
    UDC_EP_STATE_TX_ZLP_IGNORED,
    UDC_EP_STATE_TX_STARTED,
    UDC_EP_STATE_TX_COMPLETED,
    UDC_EP_STATE_MULTI_TX_STARTED,
    UDC_EP_STATE_MULTI_TX_DMA_NOTIFY_RECEIVED,
    UDC_EP_STATE_RX_STARTED,
    UDC_EP_STATE_RX_STARTED_NO_ZLP,
    UDC_EP_STATE_RX_COMPLETED,
    UDC_EP_STATE_RX_BUFFER_IS_FULL
}UDC_EndpointStateE;

#define UDC_DEBUG
#if defined(UDC_DEBUG)
typedef struct
{
    UINT8                               resetCounter;
    UINT8                               suspendCounter;
    UINT8                               resumeCounter;
    UINT8                               SOFCounter;
    UINT8                               configChangeCounter;
    UINT8                               suspendCancelledCounter;
}UDC_DebugDataS;
#endif /*UDC_DEBUG*/

typedef struct
{
    UDC_HardwareStateE                  hwState;
    UINT8                               totalActiveEndpoints;
}UDC_DatebaseS;

typedef volatile struct
{
    UDC_EndpointE                       udcEndpoint;
    UINT8                               usbEndpointNum;
    UDC_EndpointStateE                  state;
    UINT8                               *pBuffer;
    UINT32                              totalLength;
    UINT32                              lengthCounter;
    UDC_EndpointStallE                  endpointStalled;
    BOOL                                dmaEnabled;
    UINT8                               dmaChannel;
    UINT8                               *pDescBuffer;
    UINT32                              descBufferSize;
    UINT32                              interruptMask;
    UINT32                              interrupt_cnt;
    UINT32                              multi_total_buffers;       //for multitransmit only,  number of buffers
    UINT32                              IN_bytes_num_in_fifo;      //for multitransmit only,  number of bytes in FIFO
    UINT32                              max_packet_alligned_desc_cnt;         //temp, for monitoring
    UINT32                              max_packet_alligned_big_desc_cnt;     //temp, for monitoring
    UINT32                              short_packet_desc_cnt;                //temp, for monitoring
    UINT8                               *p_rx_help_buf;  //size of fifo , will hold fifo read from ,copying from this buffer to user buffer will allow solve alligment issues
    UINT32                               rx_help_buf_copied_length; //will indicate how many bytes are copied to user buffer
    UINT32                               rx_help_buf_read_length;//will indicate how many bytes are read from fifo to help buffer

    UDCDriverEndpointInterruptHandler   interruptHandler;
}UDC_EndpointDataS;


#if defined(UDC_USE_WAKEUP_LED)
typedef enum { USB_SUSPENDED, USB_AWAKE, USB_CABLE_PLUGIN, USB_CABLE_PLUGOUT } UsbStatusInd_en;
#endif //UDC_USE_WAKEUP_LED



/*******************************************************************************
 *                      G L O B A L S
 ******************************************************************************/
#if defined(_UDC_TYPES_)
    #define EXTRN
#else
    #define EXTRN      extern
#endif /*_UDC_TYPES_*/

#if defined(UDC_DEBUG)
EXTRN   UDC_DebugDataS          _udcDebugData;
#endif /*UDC_DEBUG*/

#if !defined(_UDC_TYPES_)
EXTRN   UDC_PadRegistersS       * const _pUdcPadRegisters;
EXTRN   UDC_RegistersS          * const _pUdcRegisters;
#else
EXTRN   UDC_PadRegistersS       * const _pUdcPadRegisters = (UDC_PadRegistersS *)UDC_PAD_REGISTERS_BASE;
EXTRN   UDC_RegistersS          * const _pUdcRegisters    = (UDC_RegistersS *)UDC_REGISTERS_BASE;
#endif
EXTRN   UDC_DatebaseS           _udcDatabase;
EXTRN   UDC_EndpointDataS       _udcEndpoint[UDC_MAX_ACTIVE_ENDPOINTS];

#undef EXTRN

#endif /*_UDC_TYPES_H_*/
