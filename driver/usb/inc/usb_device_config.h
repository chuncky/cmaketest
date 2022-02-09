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
 * Title:        USB DEVICE - USB Device API
 *
 * Filename:     udc_device_config.h
 *
 * Author:       Yossi Gabay
 *
 * Description:  This is the configuration file for the USB Device Stack & API
 *
 * Notes:
 ******************************************************************************/

#if !defined(_UDC_DEVICE_CONFIG_H_)
#define      _UDC_DEVICE_CONFIG_H_

#include "usb_config.h"

#define USB_DEVICE_MAX_DMA_CHANNEL                  (DMA_MAX_CHANNEL_NUMBER)

//#include "global_types.h"
#include "common.h"
#if (USB_DEVICE_DMA_ENABLED ==1)
#include "dma.h"
#include "dma_list.h"
#else
//define all functions used by dma



#endif



//#define UDC_USE_WAKEUP

#if defined(INTEL_2CHIP_PLAT_BVD) && defined(UDC_USE_WAKEUP)
#undef UDC_USE_WAKEUP
#endif

#if defined(UDC_USE_WAKEUP)
#define UDC_USE_WAKEUP_LED
#endif


#endif /*_UDC_DEVICE_CONFIG_H_*/
