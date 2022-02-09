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
 * Filename:     udc_def.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains general definitions for UDC driver
 *
 * Notes:
 ******************************************************************************/

#if !defined(_UDC_DEF_H_)
#define      _UDC_DEF_H_

//#include "global_types.h"

#define UDC_FIFO_32BITS_ALIGNMENT               ( 0x03U )

#define UDC_ENDPOINT_GROUP_LIMIT                ( UDC_ENDPOINT_Q )
#define UDC_IS_ENDPOINT_GROUP_0(eNDPOINT)       ((eNDPOINT) < UDC_ENDPOINT_GROUP_LIMIT)
#define UDC_IS_ENDPOINT_GROUP_1(eNDPOINT)       ( !UDC_IS_ENDPOINT_GROUP_0(eNDPOINT) )

#define UDC_GET_PACKET_COMPLETE_INTERRUPT_MASK(eNDPOINT)        \
                                                ((UDC_IS_ENDPOINT_GROUP_0(eNDPOINT)) ?              \
                                                (0x01U << ((eNDPOINT)<<1)) :                        \
                                                (0x01U << (((eNDPOINT)<<1) - UDC_ENDPOINT_GROUP_LIMIT)))

#define UDC_IS_ENDPOINT_0(eNDPOINT)             ( (eNDPOINT) == UDC_ENDPOINT_0 )
#define UDC_IS_ENDPOINT_GENERAL(eNDPOINT)       ( !IS_ENDPOINT_CONTROL(eNDPOINT) )

#define UDC_EP0_SETUP_CMD_MASK                  (UDC_UDCCSR0_OPC | UDC_UDCCSR0_SA)

#define UDC_EP0_IS_RX_FIFO_HAS_DATA()           (((_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0]) & UDC_UDCCSR0_RNE) != 0)
#define UDC_EP0_IS_SETUP_CMD(uDDCSR0vALUE)      (((uDDCSR0vALUE) & (UDC_EP0_SETUP_CMD_MASK)) == (UDC_EP0_SETUP_CMD_MASK))
//#define UDC_EP0_IS_OUT_PACKET(uDDCSR0vALUE)     (((uDDCSR0vALUE) & (UDC_EP0_SETUP_CMD_MASK)) == (UDC_UDCCSR0_OPC))
//#define UDC_EP0_IS_IN_PACKET(uDDCSR0vALUE)      (((uDDCSR0vALUE) & (UDC_EP0_SETUP_CMD_MASK)) == 0)

//#define UDC_EP0_IS_OUT_PACKET(uDDCSR0vALUE)     (((uDDCSR0vALUE) & (UDC_UDCCSR0_OPC)) == (UDC_UDCCSR0_OPC))
//#define UDC_EP0_IS_IN_PACKET(uDDCSR0vALUE)      (((uDDCSR0vALUE) & (UDC_UDCCSR0_OPC)) == 0)

#define UDC_EP0_IS_OUT_PACKET(uDDCSR0vALUE)     (((UINT32)((UINT32)(uDDCSR0vALUE) & (UDC_UDCCSR0_OPC))) == (UDC_UDCCSR0_OPC))
#define UDC_EP0_IS_IN_PACKET(uDDCSR0vALUE)      (((UINT32)((UINT32)(uDDCSR0vALUE) & (UDC_UDCCSR0_OPC))) == 0)


//#define UDC_EP0_SET_SHORT_PACKET()              (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = UDC_UDCCSR0_IPR)
//#define UDC_EP0_SET_IPR(uDDCSR0vALUE)           (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = (uDDCSR0vALUE) | UDC_UDCCSR0_IPR)
#define UDC_EP0_SET_IPR()                       (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = UDC_UDCCSR0_IPR)
#define UDC_EP0_SET_UDCCSR0(uDDCSR0vALUE)       (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = (uDDCSR0vALUE))

#define UDC_EP0_GET_FIFO_ADDRESS()              (UDC_GET_FIFO_ADDRESS(UDC_ENDPOINT_0))
#define UDC_EP0_GET_UDCCSR()                    (UDC_GET_UDCCSRAX(UDC_ENDPOINT_0))

#define UDC_EP0_CLEAR_OPC()                     (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = UDC_UDCCSR0_OPC)
//#define UDC_EP0_CLEAR_OPC(uDDCSR0vALUE)         (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = (uDDCSR0vALUE) | UDC_UDCCSR0_OPC)
#define UDC_EP0_CLEAR_SETUP_CMD()               (_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0] = UDC_UDCCSR0_SA | UDC_UDCCSR0_OPC)

#define UDC_IS_PACKET_COMPLETE(uDDCSRvALUE)     (((uDDCSRvALUE) & UDC_UDCCSR_PC) != 0)  //bit is set
#define UDC_IS_BUFFER_NOT_EMPTY(uDDCSRvALUE)    (((uDDCSRvALUE) & UDC_UDCCSR_BNE_BNF) != 0)
#define UDC_IS_BUFFER_NOT_FULL(uDDCSRvALUE)     (((uDDCSRvALUE) & UDC_UDCCSR_BNE_BNF) != 0)
#define UDC_IS_SHORT_PACKET(uDDCSRvALUE)        (((uDDCSRvALUE) & UDC_UDCCSR_SP) != 0)
#define UDC_IS_ROOM_IN_FIFO(uDDCSRvALUE)        (((uDDCSRvALUE) & UDC_UDCCSR_FS) != 0) 
#define UDC_IS_SST_SET(uDDCSRvALUE)             (((uDDCSRvALUE) & UDC_UDCCSR_SST) != 0) 


#define UDC_GET_FRAME_NUMBER()                  ((_pUdcRegisters->UDCFNR) & UDC_UDCFNR_MASK)
#define UDC_GET_FIFO_BYTE_COUNT(eNDPOINT)       (_pUdcRegisters->UDCBCR[eNDPOINT])
#define UDC_GET_UDCCSRAX(eNDPOINT)              (_pUdcRegisters->UDCCSR[eNDPOINT])
#define UDC_GET_UDCCSR_ADDRESS(eNDPOINT)        (&(UDC_GET_UDCCSRAX(eNDPOINT)))
#define UDC_GET_FIFO_ADDRESS(eNDPOINT)          ((volatile UINT32 *)(&(_pUdcRegisters->UDCDR[eNDPOINT])))
//#define UDC_GET_FIFO_ADDRESS(eNDPOINT)          ((volatile UINT32 *)0x1000)

#define UDC_GET_MAX_PACKET_SIZE(eNDPOINT)       (_u1dcDefaultEndpointConfig[eNDPOINT].usbMaxPacketSize)
#define UDC_GET_ENDPOINT_TYPE(eNDPOINT)         (_u1dcDefaultEndpointConfig[eNDPOINT].usbEndpointType)


#define UDC_SET_SHORT_PACKET(eNDPOINT)          (TURN_BIT_ON(_pUdcRegisters->UDCCSR[eNDPOINT], UDC_UDCCSR_SP))
#define UDC_SET_STALL_ENDPOINT(eNDPOINT)        (TURN_BIT_ON(_pUdcRegisters->UDCCSR[eNDPOINT], UDC_UDCCSR_FST))

#define UDC_CLEAR_PC(eNDPOINT)                  (TURN_BIT_ON(_pUdcRegisters->UDCCSR[eNDPOINT], UDC_UDCCSR_PC))
#define UDC_CLEAR_STALL(eNDPOINT)               (TURN_BIT_ON(_pUdcRegisters->UDCCSR[eNDPOINT], UDC_UDCCSR_SST))
#define UDC_CLEAR_EFE(eNDPOINT)                 (TURN_BIT_ON(_pUdcRegisters->UDCCSR[eNDPOINT], UDC_UDCCSR_EFE))

#define UDC_CLEAR_PC_EFE(eNDPOINT)              (TURN_BIT_ON(_pUdcRegisters->UDCCSR[eNDPOINT], (UDC_UDCCSR_PC |UDC_UDCCSR_EFE)))

#define UDC_ACTIVATE_HARDWARE(E)                (TURN_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_UDE) )
#define UDC_DEACTIVATE_HARDWARE()               (TURN_BIT_OFF(_pUdcRegisters->UDCCR, UDC_UDCCR_UDE) )
#define UDC_IS_UDC_ACTIVATED()                  ((IS_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_UDE)))

#define UDC_CLEAR_EVENT(eVENT)                  ( TURN_BIT_ON(_pUdcRegisters->UDCISR1, (eVENT)) )
#define UDC_CLEAR_PACKET_COMPLETE_0(eNDPOINT)   ( TURN_BIT_ON(_pUdcRegisters->UDCISR0, ((eNDPOINT) << 1)) )
#define UDC_CLEAR_PACKET_COMPLETE_1(eNDPOINT)   ( TURN_BIT_ON(_pUdcRegisters->UDCISR1, ((eNDPOINT) << 1)) )
//#define UDC_CLEAR_INTER_ST_0(eNDPOINT,eVENT)    ( TURN_BIT_ON(_pUdcRegisters->UDCISR0, (eVENT)) )
//#define UDC_CLEAR_INTER_ST_1(eNDPOINT,eVENT)    ( TURN_BIT_ON(_pUdcRegisters->UDCISR1, (eVENT)) )

#define UDC_CONVERT_EVENT_TO_BIT(eVENTeNUM)     (                                                               \
                                                (((eVENTeNUM) == UDC_EVENT_RESET)         ? UDC_UDCICR1_IERS  : \
                                                (((eVENTeNUM) == UDC_EVENT_SUSPEND)       ? UDC_UDCICR1_IESU  : \
                                                (((eVENTeNUM) == UDC_EVENT_RESUME)        ? UDC_UDCICR1_IERU  : \
                                                (((eVENTeNUM) == UDC_EVENT_SOF)           ? UDC_UDCICR1_IESOF : \
                                                (((eVENTeNUM) == UDC_EVENT_CONFIG_CHANGE) ? UDC_UDCICR1_IECC  : \
                                                0 ))))))

//YG - Maybe needs TURN_BIT_ON_MASK(reg,bit,mask) - for RESERVED BITS

#define TEST_BIT_ON(vALUE,bITmASK)              ( (vALUE) & (bITmASK) )
#define IS_BIT_ON(vALUE,bITmASK)                ( ((vALUE) & (bITmASK)) ? TRUE : FALSE )

//YG - move to usb_device ???
//Anton: please, never define a macro with function call (expression) inside (): this won't compile in case PMUPeripheralBothClocks is an empty macro
#if defined(PERIPHERAL_CLOCKS_VIA_PMU_API)
#define UDC_CLOCK_CONTROL(pMU_eNABLE)           (PMUPeripheralBothClocks(BOTH_USB, pMU_eNABLE))
#else
#define UDC_CLOCK_CONTROL(pMU_eNABLE)
#endif

#define UDC_IS_ENDPOINT_OUT(e)          (_u1dcDefaultEndpointConfig[e].usbEndpointDir == UDC_EP_DIR_OUT)





#endif /*_UDC_DEF_H_*/
