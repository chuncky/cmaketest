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
 * Filename:     udc_hw.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This file contains the hardware definitions for the
 *               UDC hardware in ManitobaEP & Hermon
 *
 * Notes:        Some parts were taken from xllp_udc by Intel
 ******************************************************************************/

#if !defined(_UDC_HW_H_)
#define      _UDC_HW_H_
#include "common.h"
//#include "global_types.h"
#include "udc_driver.h"

//#define UDC_MAX_UDC_ENDPOINTS       UDC_TOTAL_ENDPOINTS  /* including EP0 */
//#define UDC_MAX_UDC_ENDPOINTS       24  /* including EP0 */

//#define UDC_USB_MAX_ENDPOINTS       16  /* including EP0 */
/* UDC Controller Registers' Base */
#define UDC_REGISTERS_BASE          (0x40600000) 
#ifdef _VIRTIO_PLATFORM_
extern UINT32 	 array_registers_UDCpad[60];
#define UDC_PAD_REGISTERS_BASE      &array_registers_UDCpad[0]
#else

#define UDC_PAD_REGISTERS_BASE      (0x42900200)
#endif

//#define UDC_PAD_REGISTERS_OFFSET    (0x00000084)

#define UDC_EP0_MAX_PACKET_SIZE     (16)

#define USB1_EP0_MAX_PACKET_SIZE_LSB        (0x10)
#define USB2_EP0_MAX_PACKET_SIZE_LSB        (0x40)
#define USB1_BULK_EP_MAX_PACKET_SIZE_LSB    (0x40)
#define USB1_BULK_EP_MAX_PACKET_SIZE_MSB    (0x00)
#define USB2_BULK_EP_MAX_PACKET_SIZE_LSB    (0x00)
#define USB2_BULK_EP_MAX_PACKET_SIZE_MSB    (0x02)

#define USB2_BCD_LSB							(0x00)
#define USB2_BCD_MSB							(0x02)




/* UDCCR - UDC Control Register */
#define UDC_UDCCR_UDE               ( 0x1U << 0 )   /* RW: UDC Enabled */
#define UDC_UDCCR_UDA               ( 0x1U << 1 )   /* RO: UDC Active */
#define UDC_UDCCR_UDR               ( 0x1U << 2 )   /* RW: UDC Resume (Forces the usb out of suspend state) */
#define UDC_UDCCR_EMCE              ( 0x1U << 3 )   /* RW: Endpoint Memory Config. Error */
#define UDC_UDCCR_SMAC              ( 0x1U << 4 )   /* RW: Switch Endpoint memory to Active config. */

/* Current Configuration/Interface/Alternate settings numers */
#define UDC_UDCCR_AAISN_SHIFT       5               /* Shift and a mask for the Alternate Interface */
#define UDC_UDCCR_AAISN_MASK        ( 0x7U << UDC_UDCCR_AAISN_SHIFT )  /* RO: Alternate Settings  (0-7) */
#define UDC_UDCCR_AIN_SHIFT         8               /* Shift and a mask for the Interface */
#define UDC_UDCCR_AIN_MASK          ( 0x7U << UDC_UDCCR_AIN_SHIFT )    /* RO: Interface Number    (0-7) */
#define UDC_UDCCR_ACN_SHIFT         11              /* Shift and a mask for the Configuration */
#define UDC_UDCCR_ACN_MASK          ( 0x3U << UDC_UDCCR_ACN_SHIFT )    /* RO: Config. Number    (0-3) */

#define UDC_UDCCR_DRWF              ( 0x1U << 16 )  /* RO: Device Remote Wakeup Feature */

/* OTG Bits */
#define UDC_UDCCR_BHNP              ( 0x1U << 28 )  /* RO: B-device Host Negotation Protocol Enable */
#define UDC_UDCCR_AHNP              ( 0x1U << 29 )  /* RO: A-device Host Negotation Protocol Enable */
#define UDC_UDCCR_AALTHNP           ( 0x1U << 30 )  /* RO: A-device Alternate Host Negotation Protocol Port Support */
#define UDC_UDCCR_OEN               ( 0x1U << 31 )  /* RW: On-The-Go Enable */


/* UDCICR0 - UDC Interrupt Control Register 0 */
#define UDC_UDCICR0_IE0_0           ( 0x1U << 0 )   /* Packet Complete Interrupt Enable - Endpoint 0 */
#define UDC_UDCICR0_IE0_1           ( 0x1U << 1 )   /* FIFO Error Interrupt Enable - Endpoint 0 */
#define UDC_UDCICR0_IEA_0           ( 0x1U << 2 )   /* Packet Complete Interrupt Enable - Endpoint A */
#define UDC_UDCICR0_IEA_1           ( 0x1U << 3 )   /* FIFO Error Interrupt Enable - Endpoint A */
#define UDC_UDCICR0_IEB_0           ( 0x1U << 4 )   /* Packet Complete Interrupt Enable - Endpoint B */
#define UDC_UDCICR0_IEB_1           ( 0x1U << 5 )   /* FIFO Error Interrupt Enable - Endpoint B */
#define UDC_UDCICR0_IEC_0           ( 0x1U << 6 )   /* Packet Complete Interrupt Enable - Endpoint C */
#define UDC_UDCICR0_IEC_1           ( 0x1U << 7 )   /* FIFO Error Interrupt Enable - Endpoint C */
#define UDC_UDCICR0_IED_0           ( 0x1U << 8 )   /* Packet Complete Interrupt Enable - Endpoint D */
#define UDC_UDCICR0_IED_1           ( 0x1U << 9 )   /* FIFO Error Interrupt Enable - Endpoint D */
#define UDC_UDCICR0_IEE_0           ( 0x1U << 10 )  /* Packet Complete Interrupt Enable - Endpoint E */
#define UDC_UDCICR0_IEE_1           ( 0x1U << 11 )  /* FIFO Error Interrupt Enable - Endpoint E */
#define UDC_UDCICR0_IEF_0           ( 0x1U << 12 )  /* Packet Complete Interrupt Enable - Endpoint F */
#define UDC_UDCICR0_IEF_1           ( 0x1U << 13 )  /* FIFO Error Interrupt Enable - Endpoint F */
#define UDC_UDCICR0_IEG_0           ( 0x1U << 14 )  /* Packet Complete Interrupt Enable - Endpoint G */
#define UDC_UDCICR0_IEG_1           ( 0x1U << 15 )  /* FIFO Error Interrupt Enable - Endpoint G */
#define UDC_UDCICR0_IEH_0           ( 0x1U << 16 )  /* Packet Complete Interrupt Enable - Endpoint H */
#define UDC_UDCICR0_IEH_1           ( 0x1U << 17 )  /* FIFO Error Interrupt Enable - Endpoint H */
#define UDC_UDCICR0_IEI_0           ( 0x1U << 18 )  /* Packet Complete Interrupt Enable - Endpoint I */
#define UDC_UDCICR0_IEI_1           ( 0x1U << 19 )  /* FIFO Error Interrupt Enable - Endpoint I */
#define UDC_UDCICR0_IEJ_0           ( 0x1U << 20 )  /* Packet Complete Interrupt Enable - Endpoint J */
#define UDC_UDCICR0_IEJ_1           ( 0x1U << 21 )  /* FIFO Error Interrupt Enable - Endpoint J */
#define UDC_UDCICR0_IEK_0           ( 0x1U << 22 )  /* Packet Complete Interrupt Enable - Endpoint K */
#define UDC_UDCICR0_IEK_1           ( 0x1U << 23 )  /* FIFO Error Interrupt Enable - Endpoint K */
#define UDC_UDCICR0_IEL_0           ( 0x1U << 24 )  /* Packet Complete Interrupt Enable - Endpoint L */
#define UDC_UDCICR0_IEL_1           ( 0x1U << 25 )  /* FIFO Error Interrupt Enable - Endpoint L */
#define UDC_UDCICR0_IEM_0           ( 0x1U << 26 )  /* Packet Complete Interrupt Enable - Endpoint M */
#define UDC_UDCICR0_IEM_1           ( 0x1U << 27 )  /* FIFO Error Interrupt Enable - Endpoint M */
#define UDC_UDCICR0_IEN_0           ( 0x1U << 28 )  /* Packet Complete Interrupt Enable - Endpoint N */
#define UDC_UDCICR0_IEN_1           ( 0x1U << 29 )  /* FIFO Error Interrupt Enable - Endpoint N */
#define UDC_UDCICR0_IEP_0           ( 0x1U << 30 )  /* Packet Complete Interrupt Enable - Endpoint P */
#define UDC_UDCICR0_IEP_1           ( 0x1U << 31 )  /* FIFO Error Interrupt Enable - Endpoint P */

#define UDC_UDCICR0_ENABLE_ALL      0xFFFFFFFE      /* Mask to enable all endpoint A - P interrupts */


/* UDCICR1 - UDC Interrupt Control Register 1 */
#define UDC_UDCICR1_IEQ_0           ( 0x1U << 0 )   /* Packet Complete Interrupt Enable - Endpoint Q */
#define UDC_UDCICR1_IEQ_1           ( 0x1U << 1 )   /* FIFO Error Interrupt Enable - Endpoint Q */
#define UDC_UDCICR1_IER_0           ( 0x1U << 2 )   /* Packet Complete Interrupt Enable - Endpoint R */
#define UDC_UDCICR1_IER_1           ( 0x1U << 3 )   /* FIFO Error Interrupt Enable - Endpoint R */
#define UDC_UDCICR1_IES_0           ( 0x1U << 4 )   /* Packet Complete Interrupt Enable - Endpoint S */
#define UDC_UDCICR1_IES_1           ( 0x1U << 5 )   /* FIFO Error Interrupt Enable - Endpoint S */
#define UDC_UDCICR1_IET_0           ( 0x1U << 6 )   /* Packet Complete Interrupt Enable - Endpoint T */
#define UDC_UDCICR1_IET_1           ( 0x1U << 7 )   /* FIFO Error Interrupt Enable - Endpoint T */
#define UDC_UDCICR1_IEU_0           ( 0x1U << 8 )   /* Packet Complete Interrupt Enable - Endpoint U */
#define UDC_UDCICR1_IEU_1           ( 0x1U << 9 )   /* FIFO Error Interrupt Enable - Endpoint U */
#define UDC_UDCICR1_IEV_0           ( 0x1U << 10 )  /* Packet Complete Interrupt Enable - Endpoint V */
#define UDC_UDCICR1_IEV_1           ( 0x1U << 11 )  /* FIFO Error Interrupt Enable - Endpoint V */
#define UDC_UDCICR1_IEW_0           ( 0x1U << 12 )  /* Packet Complete Interrupt Enable - Endpoint W */
#define UDC_UDCICR1_IEW_1           ( 0x1U << 13 )  /* FIFO Error Interrupt Enable - Endpoint W */
#define UDC_UDCICR1_IEX_0           ( 0x1U << 14 )  /* Packet Complete Interrupt Enable - Endpoint X */
#define UDC_UDCICR1_IEX_1           ( 0x1U << 15 )  /* FIFO Error Interrupt Enable - Endpoint X */

/* Events */
#define UDC_UDCICR1_IERS            ( 0x1U << 27 )  /* Interrupt Enable - Reset */
#define UDC_UDCICR1_IESU            ( 0x1U << 28 )  /* Interrupt Enable - Suspend */
#define UDC_UDCICR1_IERU            ( 0x1U << 29 )  /* Interrupt Enable - Resume */
#define UDC_UDCICR1_IESOF           ( 0x1U << 30 )  /* Interrupt Enable - SOF */
#define UDC_UDCICR1_IECC            ( 0x1U << 31 )  /* Interrupt Enable - Configuration Change */

#define UDC_UDCICR1_EVENTS          ( UDC_UDCICR1_IERS | UDC_UDCICR1_IESU | UDC_UDCICR1_IERU | \
                                      UDC_UDCICR1_IESOF | UDC_UDCICR1_IECC )    /* Mask to enable all event interrupts */
#define UDC_UDCICR1_ENABLE_ALL 0xFFFF          /* Mask to enable all endpoint Q - X interrupts */


/* UDCISR0 - UDC Interrupt Status Register 0 */
#define UDC_UDCISR0_IR0_0           ( 0x1U << 0 )   /* Packet Complete Interrupt Request - Endpoint 0 */
#define UDC_UDCISR0_IR0_1           ( 0x1U << 1 )   /* FIFO Error Interrupt Request - Endpoint 0 */
#define UDC_UDCISR0_IRA_0           ( 0x1U << 2 )   /* Packet Complete Interrupt Request - Endpoint A */
#define UDC_UDCISR0_IRA_1           ( 0x1U << 3 )   /* FIFO Error Interrupt Request - Endpoint A */
#define UDC_UDCISR0_IRB_0           ( 0x1U << 4 )   /* Packet Complete Interrupt Request - Endpoint B */
#define UDC_UDCISR0_IRB_1           ( 0x1U << 5 )   /* FIFO Error Interrupt Request - Endpoint B */
#define UDC_UDCISR0_IRC_0           ( 0x1U << 6 )   /* Packet Complete Interrupt Request - Endpoint C */
#define UDC_UDCISR0_IRC_1           ( 0x1U << 7 )   /* FIFO Error Interrupt Request - Endpoint C */
#define UDC_UDCISR0_IRD_0           ( 0x1U << 8 )   /* Packet Complete Interrupt Request - Endpoint D */
#define UDC_UDCISR0_IRD_1           ( 0x1U << 9 )   /* FIFO Error Interrupt Request - Endpoint D */
#define UDC_UDCISR0_IRE_0           ( 0x1U << 10 )  /* Packet Complete Interrupt Request - Endpoint E */
#define UDC_UDCISR0_IRE_1           ( 0x1U << 11 )  /* FIFO Error Interrupt Request - Endpoint E */
#define UDC_UDCISR0_IRF_0           ( 0x1U << 12 )  /* Packet Complete Interrupt Request - Endpoint F */
#define UDC_UDCISR0_IRF_1           ( 0x1U << 13 )  /* FIFO Error Interrupt Request - Endpoint F */
#define UDC_UDCISR0_IRG_0           ( 0x1U << 14 )  /* Packet Complete Interrupt Request - Endpoint G */
#define UDC_UDCISR0_IRG_1           ( 0x1U << 15 )  /* FIFO Error Interrupt Request - Endpoint G */
#define UDC_UDCISR0_IRH_0           ( 0x1U << 16 )  /* Packet Complete Interrupt Request - Endpoint H */
#define UDC_UDCISR0_IRH_1           ( 0x1U << 17 )  /* FIFO Error Interrupt Request - Endpoint H */
#define UDC_UDCISR0_IRI_0           ( 0x1U << 18 )  /* Packet Complete Interrupt Request - Endpoint I */
#define UDC_UDCISR0_IRI_1           ( 0x1U << 19 )  /* FIFO Error Interrupt Request - Endpoint I */
#define UDC_UDCISR0_IRJ_0           ( 0x1U << 20 )  /* Packet Complete Interrupt Request - Endpoint J */
#define UDC_UDCISR0_IRJ_1           ( 0x1U << 21 )  /* FIFO Error Interrupt Request - Endpoint J */
#define UDC_UDCISR0_IRK_0           ( 0x1U << 22 )  /* Packet Complete Interrupt Request - Endpoint K */
#define UDC_UDCISR0_IRK_1           ( 0x1U << 23 )  /* FIFO Error Interrupt Request - Endpoint K */
#define UDC_UDCISR0_IRL_0           ( 0x1U << 24 )  /* Packet Complete Interrupt Request - Endpoint L */
#define UDC_UDCISR0_IRL_1           ( 0x1U << 25 )  /* FIFO Error Interrupt Request - Endpoint L */
#define UDC_UDCISR0_IRM_0           ( 0x1U << 26 )  /* Packet Complete Interrupt Request - Endpoint M */
#define UDC_UDCISR0_IRM_1           ( 0x1U << 27 )  /* FIFO Error Interrupt Request - Endpoint M */
#define UDC_UDCISR0_IRN_0           ( 0x1U << 28 )  /* Packet Complete Interrupt Request - Endpoint N */
#define UDC_UDCISR0_IRN_1           ( 0x1U << 29 )  /* FIFO Error Interrupt Request - Endpoint N */
#define UDC_UDCISR0_IRP_0           ( 0x1U << 30 )  /* Packet Complete Interrupt Request - Endpoint P */
#define UDC_UDCISR0_IRP_1           ( 0x1U << 31 )  /* FIFO Error Interrupt Request - Endpoint P */

/* Mask to combine EP0 interrupts */
#define UDC_UDCISR0_EP0_BITS        ( UDC_UDCISR0_IR0_0 | UDC_UDCISR0_IR0_1)

/* Mask to combine all general endpoints interrupts */
#define UDC_UDCISR0_GENERAL_EP_MASK ( 0xFFFFFFFFU & (~UDC_UDCISR0_EP0_BITS))

/* UDCICR1 - UDC Interrupt Status Register 1 */
#define UDC_UDCISR1_IRQ_0           ( 0x1U << 0 )   /* Packet Complete Interrupt Request - Endpoint Q */
#define UDC_UDCISR1_IRQ_1           ( 0x1U << 1 )   /* FIFO Error Interrupt Request - Endpoint Q */
#define UDC_UDCISR1_IRR_0           ( 0x1U << 2 )   /* Packet Complete Interrupt Request - Endpoint R */
#define UDC_UDCISR1_IRR_1           ( 0x1U << 3 )   /* FIFO Error Interrupt Request - Endpoint R */
#define UDC_UDCISR1_IRS_0           ( 0x1U << 4 )   /* Packet Complete Interrupt Request - Endpoint S */
#define UDC_UDCISR1_IRS_1           ( 0x1U << 5 )   /* FIFO Error Interrupt Request - Endpoint S */
#define UDC_UDCISR1_IRT_0           ( 0x1U << 6 )   /* Packet Complete Interrupt Request - Endpoint T */
#define UDC_UDCISR1_IRT_1           ( 0x1U << 7 )   /* FIFO Error Interrupt Request - Endpoint T */
#define UDC_UDCISR1_IRU_0           ( 0x1U << 8 )   /* Packet Complete Interrupt Request - Endpoint U */
#define UDC_UDCISR1_IRU_1           ( 0x1U << 9 )   /* FIFO Error Interrupt Request - Endpoint U */
#define UDC_UDCISR1_IRV_0           ( 0x1U << 10 )  /* Packet Complete Interrupt Request - Endpoint V */
#define UDC_UDCISR1_IRV_1           ( 0x1U << 11 )  /* FIFO Error Interrupt Request - Endpoint V */
#define UDC_UDCISR1_IRW_0           ( 0x1U << 12 )  /* Packet Complete Interrupt Request - Endpoint W */
#define UDC_UDCISR1_IRW_1           ( 0x1U << 13 )  /* FIFO Error Interrupt Request - Endpoint W */
#define UDC_UDCISR1_IRX_0           ( 0x1U << 14 )  /* Packet Complete Interrupt Request - Endpoint X */
#define UDC_UDCISR1_IRX_1           ( 0x1U << 15 )  /* FIFO Error Interrupt Request - Endpoint X */

/* Mask to combine all general endpoints interrupts */
#define UDC_UDCISR1_GENERAL_EP_MASK  ( 0x0000FFFFU )

/* Events */
#define UDC_UDCISR1_IRRS            ((UINT32)( 0x1U << 27 ))  /* Interrupt Request - Reset */
#define UDC_UDCISR1_IRSU            ((UINT32)( 0x1U << 28 ))  /* Interrupt Request - Suspend */
#define UDC_UDCISR1_IRRU            ((UINT32)( 0x1U << 29 ))  /* Interrupt Request - Resume */
#define UDC_UDCISR1_IRSOF           ((UINT32)( 0x1U << 30 ))  /* Interrupt Request - SOF */
#define UDC_UDCISR1_IRCC            ((UINT32)( 0x1U << 31 ))  /* Interrupt Request - Configuration Change */

/* Mask to combine all events */
#define UDC_UDCISR1_EVENTS          ( UDC_UDCISR1_IRRS | UDC_UDCISR1_IRSU | UDC_UDCISR1_IRRU | \
                                      UDC_UDCISR1_IRSOF | UDC_UDCISR1_IRCC ) /* Mask to clear all event interrupts */


/* UDCFNR - UDC Frame Number Register */
#define UDC_UDCFNR_MASK             ( 0x7FFU << 0 )  /* Frame Number associated with last received SOF [10..0]*/


/* UDCOTGICR - UDC On-The-Go Interrupt Control Resigter */
#define UDC_UDCOTGICR_IEIDF         ( 0x1U << 0 )  /* OTG ID Change Falling Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEIDR         ( 0x1U << 1 )  /* OTG ID Change Rising Edge Interrupt Enable */
#define UDC_UDCOTGICR_IESDF         ( 0x1U << 2 )  /* OTG A-Device SRP Detect Falling Edge Interrupt Enable */
#define UDC_UDCOTGICR_IESDR         ( 0x1U << 3 )  /* OTG A-Device SRP Detect Rising Edge Interrupt Enable */
#define UDC_UDCOTGICR_IESVF         ( 0x1U << 4 )  /* OTG Session Valid Falling Edge Interrupt Enable */
#define UDC_UDCOTGICR_IESVR         ( 0x1U << 5 )  /* OTG Session Vaind Rising Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEVV44F       ( 0x1U << 6 )  /* OTG Vbus Valid 4.4v Falling Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEVV44R       ( 0x1U << 7 )  /* OTG Vbus Valid 4.4v Rising Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEVV40F       ( 0x1U << 8 )  /* OTG Vbus Valid 4.0v Falling Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEVV40R       ( 0x1U << 9 )  /* OTG Vbus Valid 4.0v Rising Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEXF          ( 0x1U << 16 ) /* External Transceiver Interrupt Falling Edge Interrupt Enable */
#define UDC_UDCOTGICR_IEXR          ( 0x1U << 17 ) /* External Transceiver Interrupt RisingInterrupt Enable */
#define UDC_UDCOTGICR_IESF          ( 0x1U << 24 ) /* OTG SET_FEATURE command received */


/* UDCOTGISR - UDC On-The-Go Interrupt Control Resigter */
#define UDC_UDCOTGISR_IRIDF         ( 0x1U << 0 )  /* OTG ID Change Falling Edge Interrupt Request */
#define UDC_UDCOTGISR_IRIDR         ( 0x1U << 1 )  /* OTG ID Change Rising Edge Interrupt Request */
#define UDC_UDCOTGISR_IRSDF         ( 0x1U << 2 )  /* OTG A-Device SRP Detect Falling Edge Interrupt Request */
#define UDC_UDCOTGISR_IRSDR         ( 0x1U << 3 )  /* OTG A-Device SRP Detect Rising Edge Interrupt Request */
#define UDC_UDCOTGISR_IRSVF         ( 0x1U << 4 )  /* OTG Session Valid Falling Edge Interrupt Request */
#define UDC_UDCOTGISR_IRSVR         ( 0x1U << 5 )  /* OTG Session Vaind Rising Edge Interrupt Request */
#define UDC_UDCOTGISR_IRVV44F       ( 0x1U << 6 )  /* OTG Vbus Valid 4.4v Falling Edge Interrupt Request */
#define UDC_UDCOTGISR_IRVV44R       ( 0x1U << 7 )  /* OTG Vbus Valid 4.4v Rising Edge Interrupt Request */
#define UDC_UDCOTGISR_IRVV40F       ( 0x1U << 8 )  /* OTG Vbus Valid 4.0v Falling Edge Interrupt Request */
#define UDC_UDCOTGISR_IRVV40R       ( 0x1U << 9 )  /* OTG Vbus Valid 4.0v Rising Edge Interrupt Request */
#define UDC_UDCOTGISR_IRXF          ( 0x1U << 15 ) /* External Transceiver Interrupt Falling Edge Interrupt Request */
#define UDC_UDCOTGISR_IRXR          ( 0x1U << 16 ) /* External Transceiver Interrupt RisingInterrupt Request */
#define UDC_UDCOTGISR_IRSF          ( 0x1U << 24 ) /* OTG SET_FEATURE command received */


/* UP2OCR - USB Port 2 Output Control Register */
#define UDC_UP2OCR_CPVEN            ( 0x1U << 0 )   /* Change Pump Vbus Enable */
#define UDC_UP2OCR_CPVPE            ( 0x1U << 1 )   /* Change Pump Vbus Pulse Enable */
#define UDC_UP2OCR_DPPDE            ( 0x1U << 2 )   /* Host Port 2 Transceiver D+ Pull Down Enable */
#define UDC_UP2OCR_DMPDE            ( 0x1U << 3 )   /* Host Port 2 Transceiver D- Pull Down Enable */
#define UDC_UP2OCR_DPPUE            ( 0x1U << 4 )   /* Host Port 2 Transceiver D+ Pull Up Enable */
#define UDC_UP2OCR_DMPUE            ( 0x1U << 5 )   /* Host Port 2 Transceiver D- Pull Up Enable */
#define UDC_UP2OCR_DPPUBE           ( 0x1U << 6 )   /* Host Port 2 Transceiver D+ Pull Up Bypass Enable */
#define UDC_UP2OCR_DMPUBE           ( 0x1U << 7 )   /* Host Port 2 Transceiver D- Pull Up Bypass Enable */
#define UDC_UP2OCR_EXSP             ( 0x1U << 8 )   /* External Transceiver Speed Control */
#define UDC_UP2OCR_EXSUS            ( 0x1U << 9 )   /* External Transceiver Suspend Enable */
#define UDC_UP2OCR_IDON             ( 0x1U << 10 )  /* OTG ID Read Enable */
#define UDC_UP2OCR_HXS              ( 0x1U << 16 )  /* Host Port 2 Transceiver Output Select */
#define UDC_UP2OCR_HXOE             ( 0x1U << 17 )  /* Host Port 2 Transceiver Output Enable */
#define UDC_UP2OCR_CXOE             ( 0x1U << 20 )  /* Device Controller Transceiver Output Enable */
#define UDC_UP2OCR_SEOS_SHIFT       24              /* Shift and a mask for Host Single-Ended Output Select */
#define UDC_UP2OCR_SEOS_MASK        ( 0x7U << UDC_UP2OCR_SEOS_SHIFT )  /* RW: Select  (0-7) */


/* UDCCSR0 - UDC Endpoint 0 Control/Status Register */
#define UDC_UDCCSR0_OPC             ( 0x1U << 0 )   /* RW: OUT packet to endpoint zero received */
#define UDC_UDCCSR0_IPR             ( 0x1U << 1 )   /* RW: Packet loaded and ready from transmission */
#define UDC_UDCCSR0_FTF             ( 0x1U << 2 )   /* RW: Flush the Tx FIFO */
#define UDC_UDCCSR0_DME             ( 0x1U << 3 )   /* RW: DMA Enable  */
#define UDC_UDCCSR0_SST             ( 0x1U << 4 )   /* RW: UDC sent stall handshake */
#define UDC_UDCCSR0_FST             ( 0x1U << 5 )   /* RW: Force the UDC to issue a stall handshake */
#define UDC_UDCCSR0_RNE             ( 0x1U << 6 )   /* RO: There is unread data in the Rx FIFO */
#define UDC_UDCCSR0_SA              ( 0x1U << 7 )   /* RW: Setup Active - Current packet in FIFO is a setup command */
#define UDC_UDCCSR0_AREN            ( 0x1U << 8 )   /* RW: ACK Response Enable */
#define UDC_UDCCSR0_ACM             ( 0x1U << 9 )   /* RW: ACK Control Mode */

/* UDCCSRA-UDCCSRX - UDC Endpoint Control/Status Registers A-X */
#define UDC_UDCCSR_FS               ( 0x1U << 0 )   /* RO: FIFO needs service */
#define UDC_UDCCSR_PC               ( 0x1U << 1 )   /* RW: Packet Complete */
#define UDC_UDCCSR_EFE              ( 0x1U << 2 )   /* RW: Endpoint FIFO error */
//#define UDC_UDCCSR_TRN              ( 0x1U << 2 )   /* RW: Endpoint FIFO error */
#define UDC_UDCCSR_DME              ( 0x1U << 3 )   /* RW: DMA Enable */
#define UDC_UDCCSR_SST              ( 0x1U << 4 )   /* RW: Sent STALL */
#define UDC_UDCCSR_FST              ( 0x1U << 5 )   /* RW: Force STALL */
#define UDC_UDCCSR_BNE_BNF          ( 0x1U << 6 )   /* RO: Buffer not empty/Buffer not full */
#define UDC_UDCCSR_SP               ( 0x1U << 7 )   /* IN-WO, OUT-RO: Short Packet */
#define UDC_UDCCSR_FEF              ( 0x1U << 8 )   /* RW: Flush Endpoint FIFO */
#define UDC_UDCCSR_DPE              ( 0x1U << 9 )   /* RO: Data Packet Error */


/* UDCCRA-UDCCRX Endpoint A-X Configuration Registers */
#define UDC_UDCCRAX_EE              ( 0x1U << 0 )                       /* Endpoint Enable */
#define UDC_UDCCRAX_DE_SHIFT        1
#define UDC_UDCCRAX_DE              ( 0x1U << UDC_UDCCRAX_DE_SHIFT)     /* Double-buffering Enable */
#define UDC_UDCCRAX_MPS_SHIFT       2
#define UDC_UDCCRAX_MPS_MASK        ( 0x3FFU << UDC_UDCCRAX_MPS_SHIFT)  /* Maximum Packet Size */
#define UDC_UDCCRAX_ED_SHIFT        12
#define UDC_UDCCRAX_ED              ( 0x1U << UDC_UDCCRAX_ED_SHIFT)     /* Endpoint Direction */
#define UDC_UDCCRAX_ET_SHIFT        13
#define UDC_UDCCRAX_ET_MASK         ( 0x3U << UDC_UDCCRAX_ET_SHIFT)     /* Endoint Type */
#define UDC_UDCCRAX_EN_SHIFT        15
#define UDC_UDCCRAX_EN_MASK         ( 0xFU << UDC_UDCCRAX_EN_SHIFT)     /* USB Endoint Number */
#define UDC_UDCCRAX_AISN_SHIFT      19
#define UDC_UDCCRAX_AISN_MASK       ( 0x7U << UDC_UDCCRAX_AISN_SHIFT)   /* Interface Alternate Settings Number */
#define UDC_UDCCRAX_IN_SHIFT        22
#define UDC_UDCCRAX_IN_MASK         ( 0x7U << UDC_UDCCRAX_IN_SHIFT)     /* Interface Number */
#define UDC_UDCCRAX_CN_SHIFT        25
#define UDC_UDCCRAX_CN_MASK         ( 0x3U << UDC_UDCCRAX_CN_SHIFT)     /* Configuration Number */

#define UDC_UDCCRAX_MASK            ( 0x07FFFFFFU )

/* UDCBCR0-UDCCBRX - UDC Endpoint 0 and Endpoint A - X Byte Count Register*/
#define UDC_UDCBCR_BC_MASK          0x3FF


/* UDC Endpoints */
/*-----------------02/24/2004 4:30PM----------------
 * YG - move to udc_driver.h for API
 * --------------------------------------------------
 typedef enum
{
    UDC_ENDPOINT_0 = 0,
    UDC_ENDPOINT_A,
    UDC_ENDPOINT_B,
    UDC_ENDPOINT_C,
    UDC_ENDPOINT_D,
    UDC_ENDPOINT_E,
    UDC_ENDPOINT_F,
    UDC_ENDPOINT_G,
    UDC_ENDPOINT_H,
    UDC_ENDPOINT_I,
    UDC_ENDPOINT_J,
    UDC_ENDPOINT_K,
    UDC_ENDPOINT_L,
    UDC_ENDPOINT_M,
    UDC_ENDPOINT_N,
    UDC_ENDPOINT_P,
    UDC_ENDPOINT_Q,
    UDC_ENDPOINT_R,
    UDC_ENDPOINT_S,
    UDC_ENDPOINT_T,
    UDC_ENDPOINT_U,
    UDC_ENDPOINT_V,
    UDC_ENDPOINT_W,
    UDC_ENDPOINT_X,
    UDC_TOTAL_ENDPOINTS
} UDC_EndpointEold;*/

/* UDC Registers */
typedef volatile struct
{
    volatile UINT32   UDCCR;                            /* UDC Control Register */
    volatile UINT32   UDCICR0;                          /* UDC Interrrupt Control Register 0 */
    volatile UINT32   UDCICR1;                          /* UDC Interrrupt Control Register 1 */
    volatile UINT32   UDCISR0;                          /* UDC Interrrupt Status Register 0 */
    volatile UINT32   UDCISR1;                          /* UDC Interrrupt Status Register 1 */
    volatile UINT32   UDCFNR;                           /* UDC Frame Number Register */
    volatile UINT32   UDCOTGICR;                        /* UDC OTG Interrupt Control Register */
    volatile UINT32   UDCOTGISR;                        /* UDC OTG Interrupt Status Register */
    volatile UINT32   UP2OCR;                           /* UDC Port 2 Output Control Register */
    volatile UINT32   UP3OCR;                           /* UDC Port 3 Output Control Register */
    volatile UINT32   RESERVED0[54];                    /* Reserved 0x40600028-0x406000FF */
    volatile UINT32   UDCCSR[UDC_TOTAL_ENDPOINTS];      /* UDC Control/Status registers 0, A-X */
    volatile UINT32   RESERVED1[40];                    /* Reserved 0x40600160-0x406001FF */
    volatile UINT32   UDCBCR[UDC_TOTAL_ENDPOINTS];      /* UDC Byte Count Registers 0, A-X */
    volatile UINT32   RESERVED2[40];                    /* Reserved 0x40600260-0x406002FF */
    volatile UINT32   UDCDR[UDC_TOTAL_ENDPOINTS];       /* UDC Data Registers 0, A-X */
    volatile UINT32   RESERVED3[40];                    /* Reserved 0x40600360-0x406003FF */
    volatile UINT32   RESERVED4[1];                     /* Reserved 0x40600400-0x40600403 YG - why have it separeately ??? */
    volatile UINT32   UDCCRAX[UDC_TOTAL_ENDPOINTS-1];   /* 23 UDC Configuration Registers A-X */
    //volatile UINT32   UDCCRAX[UDC_TOTAL_ENDPOINTS];   /* 23 UDC Configuration Registers A-X (Zero as dummy) */
} UDC_RegistersS;//YG, *P_UDC_RegistersS;

typedef volatile struct
{
    volatile UINT32   RESERVED1[33];                    /* skip 0x0-0x83 */
    volatile UINT32   UMCE;                             /* UDC */
    volatile UINT32   USRSTC;                           /* UDC */
    volatile UINT32   OTGPSRC;                          /* UDC */
    volatile UINT32   OTGP12ASR;                        /* UDC */
    volatile UINT32   UPWER;                            /* UDC */
    volatile UINT32   UP1OCR;                           /* UDC */
    volatile UINT32   RESERVED2[18];                    /* skip 0x9C-0xE3 */
    volatile UINT32   UPIWKUPEN;                        /* UDC */
} UDC_PadRegistersS;

#endif /*_UDC_HW_H_*/
