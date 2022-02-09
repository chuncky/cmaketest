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
 * Title:        UDC DRIVER - USB Device Controller Driver source file
 *
 * Filename:     udc_driver.c
 *
 * Author:       Yossi Gabay
 *
 * Description:  This is the main file for UDC hardware in ManitobaEP & Hermon
 *
 * Notes:
 ******************************************************************************/

#include "udc_driver.h"

#define _UDC_TYPES_
#include "udc_config.h"
#include "udc_types.h"
#undef  _UDC_TYPES_

#include "udc_hw.h"
#include "udc_def.h"

#include "intc.h"
#if (USB_DEVICE_DMA_ENABLED ==1)
#include "dma.h"
#endif
//#include "pmu.h"
//#include "pmue.h"

#define UDC_DRIVER_DEBUG_ENABLED        0
#if  (UDC_DRIVER_DEBUG_ENABLED  == 1)

#define UDC_DRV_DBG_MT_BUFF_LENGTH      1024
volatile UINT32 udc_dbg_mt_buff_ind=0,udc_mt_dbg_buff[UDC_DRV_DBG_MT_BUFF_LENGTH];

#define UDC_DRV_DBG_NOTIFY_BUFF_LENGTH      1024
volatile UINT32 udc_dbg_nt_buff_ind=0,udc_nt_dbg_buff[UDC_DRV_DBG_NOTIFY_BUFF_LENGTH];


//debug codes
#define UDC_DGB_START_OF_TRANSMIT               0xFFFFFFFF   // transmit started
//after UDC_DGB_START_OF_TRANSMIT
//number of buffers
//multilist header

#define UDC_DGB_WRITE_LESS_THAN_FIFO            0xAAAAAAAA   // descriptor was set to complete till fifo size
#define UDC_DGB_WRITE_TILL_COMPLETE_FIFO        0xAAAABBBB
#define UDC_DGB_WRITE_MULTIPLE_FIFO             0xBBBBBBBB
#define UDC_DGB_WRITE_LEFT_OVER_FIFO            0xCCCCCCCC
//total length closes the message

#if (USB_DEVICE_DMA_ENABLED == 1)
#define UDC_DBG_CHECK_EOB(INDEX,MAX_LENGTH)                                         \
    if(INDEX == (MAX_LENGTH) )                                                      \
        INDEX=0;

#define UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_DATA,FILTER)                      \
    if(FILTER)                                                                      \
    {                                                                               \
        if(udc_mt_dbg_buff == DBG_BUFF)                                             \
        {                                                                           \
            UDC_DBG_CHECK_EOB(udc_dbg_mt_buff_ind,UDC_DRV_DBG_MT_BUFF_LENGTH) ;     \
            udc_mt_dbg_buff[udc_dbg_mt_buff_ind++] = DBG_MSG_DATA;                  \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            UDC_DBG_CHECK_EOB(udc_dbg_nt_buff_ind,UDC_DRV_DBG_NOTIFY_BUFF_LENGTH) ; \
            udc_nt_dbg_buff[udc_dbg_nt_buff_ind++] = DBG_MSG_DATA;                  \
        }                                                                           \
    }

#define UDC_DBG_DEBUG_MSG_2PARAMS(DBG_BUFF,DBG_MSG_CODE,DBG_MSG_DATA,FILTER)        \
        UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_CODE,FILTER);                     \
        UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_DATA,FILTER);

#define UDC_DBG_DEBUG_MSG_3PARAMS(DBG_BUFF,DBG_MSG_CODE,DBG_MSG_DATA1,DBG_MSG_DATA2,FILTER)     \
        UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_CODE,FILTER);                                 \
        UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_DATA1,FILTER);                                \
        UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_DATA2,FILTER);
#endif
#else //NO DEBUG
#if (USB_DEVICE_DMA_ENABLED == 1)
#define UDC_DBG_CHECK_EOB(INDEX,MAX_LENGTH)
#define UDC_DBG_DEBUG_MSG_1PARAM(DBG_BUFF,DBG_MSG_DATA,FILTER)
#define UDC_DBG_DEBUG_MSG_2PARAMS(DBG_BUFF,DBG_MSG_CODE,DBG_MSG_DATA,FILTER)
#define UDC_DBG_DEBUG_MSG_3PARAMS(DBG_BUFF,DBG_MSG_CODE,DBG_MSG_DATA1,DBG_MSG_DATA2,FILTER)
#endif

#endif // DEBUG

// GET_DMA_TRANSFER_LIMIT calculate the biggest devisor of packetSizeLimit which is smaller or equal to DMA_MAX_TRANSFER_LENGTH (e.g. DMASingleDescriptorSetup restrict transfers to 0x1FFF to assuming packetSizeLimit is 64bytes the results will be 0x1FC0).
#define GET_DMA_TRANSFER_LIMIT(packetSizeLimit)		((DMA_MAX_TRANSFER_LENGTH / packetSizeLimit) * packetSizeLimit)

// DMA TX END NOTIFICATION (UDCDriverDMATransferCompletedNotify) BEHAVIOUR:
// The original implementation was waiting under the DMA end notification callback (LISR context) until PC&FS bits are set (depending on last packet type)
// This caused problems as the wait time depends on the host and could become very long in rare case (observed up to 40ms wait).
#define UDC_DMA_TX_DONT_WAIT_FOR_PC_FS // limited wait for PC/FS bits once DMA end notification arrives, don't assert on timeout

//#define MAX_WAIT_FOR_STATUS_CNT   100000 // Loops (around 80ms!), limits the wait time (asserts if limit violated)
// The timeout for UDC_DMA_TX_DONT_ASSERT_FOR_PC_FS: 1ms
// The typical time is less than 100us; the timeout is set so that there will be enough guard time over the typical value,
// and still staying on LISR for this time will not disrupt other real-time activities.
#define MAX_WAIT_FOR_STATUS_TIME    (1000*5) // 1ms in CP14.CCNT units (x64 mode) = 200ns

//Waiting for PC bit to get set after receiving RX interrupt
//Sometimes it is delayed
//The delay is in CP14 counter units (200 us) - in the worst case (very unlikely) this will constitute a 200us latency for all pending interrupts
#define MAX_WAIT_FOR_PC_BIT_SET_AFTER_RX_CNT    1000
//map of PREQUESTs for UDC endpoints defined in dma_list.h
#if (USB_DEVICE_DMA_ENABLED ==1)
const UINT8   _udc_ep_dma_preq_map[UDC_TOTAL_ENDPOINTS]= {
    0,                  //UDC_ENDPOINT_0 - does not exist, should not access this entry
    DMA_UDC_ENDPOINT_A,
    DMA_UDC_ENDPOINT_B,
    DMA_UDC_ENDPOINT_C,
    DMA_UDC_ENDPOINT_D,
    DMA_UDC_ENDPOINT_E,
    DMA_UDC_ENDPOINT_F,
    DMA_UDC_ENDPOINT_G,
    DMA_UDC_ENDPOINT_H,
    DMA_UDC_ENDPOINT_I,
    DMA_UDC_ENDPOINT_J,
    DMA_UDC_ENDPOINT_K,
    DMA_UDC_ENDPOINT_L,
    DMA_UDC_ENDPOINT_M,
    DMA_UDC_ENDPOINT_N,
    DMA_UDC_ENDPOINT_P,
    DMA_UDC_ENDPOINT_Q,
    DMA_UDC_ENDPOINT_R,
    DMA_UDC_ENDPOINT_S,
    DMA_UDC_ENDPOINT_T,
    DMA_UDC_ENDPOINT_U,
    DMA_UDC_ENDPOINT_V,
    DMA_UDC_ENDPOINT_W,
    DMA_UDC_ENDPOINT_X,
};

#define UDC_GET_DMA_PERIPHERAL(eNDPOINT)        (_udc_ep_dma_preq_map[eNDPOINT])

#endif

/*******************************************************************************
 *                      P R O T O T Y P E S   F U N C T I O N S
 ******************************************************************************/
static void UDCDriverInterruptHandlerReceiveWithDma(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits);
static void UDCDriverInterruptHandlerReceiveWithInterrupt(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits);
static void UDCDriverInterruptHandlerTransmitWithDma(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits);
static void UDCDriverInterruptHandlerTransmitWithInterrupt(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits);

/*******************************************************************************
 *                      E X T E R N  F U N C T I O N S
 ******************************************************************************/
extern void    USB1DeviceEventNotify(UDC_DeviceEventE event, UINT32 param);

extern void    USB1DeviceEP0InterruptHandler(UINT32 udccsr0Reg);

extern void    USB1DeviceReceiveNotifyFn(UINT8              usbEndpointNum,
                                        UINT8              *pRxBuffer,
                                        UINT32             length,
                                        BOOL               endOfMessage);

extern void    USB1DeviceTransmitNotifyFn(UINT8             usbEndpointNum,
                                         UINT8             *pTxBuffer,
                                         UINT32            length,
                                         BOOL              end_of_transmit);

extern void* malloc(unsigned int);

#if defined (_VIRTIO_PLATFORM_)
UINT32 	 array_registers_UDCpad[60];
#endif


/*******************************************************************************
 *                      S T A T I C  F U N C T I O N S
 ******************************************************************************/

/*******************************************************************************
 * Function:    UDCDriverDatabaseReset
 *******************************************************************************
 * Description: Reset the internal database
 *
 * Parameters:  fullReset - is it full initialization (power-up) or partial
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverDatabaseReset(BOOL fullInit)
{
    UINT32          idx;

    /* UDC database */
    if(fullInit)
    {
        _udcDatabase.hwState                    = UDC_HW_STATE_NONE;
        _udcDatabase.totalActiveEndpoints       = 0;
    }

    /* UDC Endpoints */
    for(idx=0; idx<UDC_MAX_ACTIVE_ENDPOINTS; idx++)
    {


        _udcEndpoint[idx].lengthCounter         = 0;
        _udcEndpoint[idx].IN_bytes_num_in_fifo  = 0;
        _udcEndpoint[idx].max_packet_alligned_big_desc_cnt = 0;
        _udcEndpoint[idx].max_packet_alligned_desc_cnt     = 0;
        _udcEndpoint[idx].short_packet_desc_cnt            = 0;
        _udcEndpoint[idx].multi_total_buffers              = 0;
        _udcEndpoint[idx].interrupt_cnt                    = 0;

        _udcEndpoint[idx].endpointStalled           = UDC_STALL_DISABLE;

        if(fullInit)
        {
            _udcEndpoint[idx].pBuffer               = NULL;
            _udcEndpoint[idx].udcEndpoint           = UDC_INVALID_ENDPOINT;
            _udcEndpoint[idx].usbEndpointNum        = 0xFF;
            _udcEndpoint[idx].totalLength           = 0;

            _udcEndpoint[idx].state                 = UDC_EP_STATE_DEACTIVE;
            _udcEndpoint[idx].dmaEnabled            = FALSE;
            _udcEndpoint[idx].dmaChannel            = 0xFF;
            _udcEndpoint[idx].pDescBuffer           = NULL;
            _udcEndpoint[idx].descBufferSize        = 0;
            _udcEndpoint[idx].interruptMask         = 0;
            _udcEndpoint[idx].interruptHandler      = NULL;
        }
        else
        {
            /* set into active only only ones which are truely ACTIVE (not DEACTIVE) */
            if(_udcEndpoint[idx].state != UDC_EP_STATE_DEACTIVE)
            {
                _udcEndpoint[idx].state         = UDC_EP_STATE_ACTIVE;
            }
        }
    }/*for*/

    /* DEBUG data */
#if defined(UDC_DEBUG)
    if(fullInit)
    {
        _udcDebugData.resetCounter              = 0;
        _udcDebugData.suspendCounter            = 0;
        _udcDebugData.resumeCounter             = 0;
        _udcDebugData.SOFCounter                = 0;
        _udcDebugData.configChangeCounter       = 0;
        _udcDebugData.suspendCancelledCounter   = 0;
    }
#endif /*UDC_DEBUG*/

} /* End of < UDCDriverDatabaseReset > */


/*******************************************************************************
 * Function:    UDCDriverComputeConfigRegisterValue
 *******************************************************************************
 * Description: Compute the 32bit configuration for UDCCRA-UDCCRX register
 *
 * Parameters:  pEndpointsConfig - configuration parameters
 *
 * Output:      none
 *
 * Returns:     UINT32 - configuration register value
 *
 * Notes:
 ******************************************************************************/
static UINT32 UDCDriverComputeConfigRegisterValue(const UDC_EndpointConfigS *pEndpointsConfig)
{
    UINT32 config;

    if(pEndpointsConfig->endpointEnabled)
    {
        config =
            /* Usb Configuration Number: (1-3) */
            (pEndpointsConfig->usbConfigNum          << UDC_UDCCRAX_CN_SHIFT)     |
            /* Usb Interface Number: (1-7) */
            (pEndpointsConfig->usbInterfaceNum       << UDC_UDCCRAX_IN_SHIFT)     |
            /* Usb Interface Alternate Settings Number: (1-7) */
            (pEndpointsConfig->usbIntAltSettingsNum  << UDC_UDCCRAX_AISN_SHIFT)   |
            /* Usb Endpoint Number: (1-15) */
            (pEndpointsConfig->usbEndpointNum        << UDC_UDCCRAX_EN_SHIFT)     |
            /* Usb Endpoint type: Bulk, Iso, Interrupt */
            (pEndpointsConfig->usbEndpointType       << UDC_UDCCRAX_ET_SHIFT)     |
            /* Usb Endpoint direction: IN, OUT */
            (pEndpointsConfig->usbEndpointDir        << UDC_UDCCRAX_ED_SHIFT)     |
            /* Max. Packet Size: (1-1023) */
            ((pEndpointsConfig->usbMaxPacketSize & UDC_UDCCRAX_MPS_MASK) << UDC_UDCCRAX_MPS_SHIFT) |
            /* Double Buffering enabled\disabled */
            (pEndpointsConfig->doubleBuffEnabled     << UDC_UDCCRAX_DE_SHIFT)           |
            /* Endpoint enabled/disabled */
            UDC_EP_ENA;
    }
    else
    {
        config = 0x00U;
    }

    return config;
} /* End of < UDCDriverComputeConfigRegisterValue > */

/*******************************************************************************
 * Function:    UDCDriverConfigureEndpoints
 *******************************************************************************
 * Description: Configure endpoints according the pre-defined table
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     BOOL - configuration success or not
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverConfigureEndpoints(void)
{
    volatile int         i,j,k=0;
    int         ep;

    /* if UDC is ON - registers can not be accessed */
    //UDC_CLOCK_CONTROL(PMU_ON);
#if defined (INTEL_2CHIP_PLAT_BVD)
/*-----------------12/18/2005 7:02PM----------------
 * when running from SDRAM the usb was already initialized by the flasher
 * so, we don't have to check this assert
 * --------------------------------------------------*/
	if (!isSDramAsImageFlash())
 #endif //INTEL_2CHIP_PLAT_BVD
	if(UDC_IS_UDC_ACTIVATED())
    		{
       		 	return FALSE;
    		}



    /* load endpoint configuration */
    for(ep=UDC_ENDPOINT_A; ep<UDC_TOTAL_ENDPOINTS; ep++)
    {
        UINT32      config;

        /* calculate configuration value */
        config = UDCDriverComputeConfigRegisterValue( &_u1dcDefaultEndpointConfig[ep] );

        /* write endpoint configuration to hardware - starts from Endpoint_A (skip zero) */
        _pUdcRegisters->UDCCRAX[ep - 1] = config;
    }
     /* Make some delay */
    for(i=0; i<1000; i++)
    {
        for(j=0;j<100;j++)
        {
            k++;
        }
    }

    /* update the hardware - to load & check the validity of the parameters */
    UDC_ACTIVATE_HARDWARE();//alla - need to check how it affects the enumeration etc.

     /* Make some delay */
    for(i=0; i<1000; i++)
    {
        for(j=0;j<100;j++)
        {
            k++;
        }
    }

    /* check for error in configuration */
    if(IS_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_EMCE))
    {
        /* clear the status & disable the UDC */
        _pUdcRegisters->UDCCR = UDC_UDCCR_EMCE;

        /* configuration error */
        _udcDatabase.hwState  = UDC_HW_STATE_CONFIG_FAIL;

        /* configuration error */
        return FALSE;
    }

    _udcDatabase.hwState = UDC_HW_STATE_CONFIG_OK;

    for(ep=UDC_ENDPOINT_A; ep<UDC_MAX_ACTIVE_ENDPOINTS; ep++)
    {
        if(_udcEndpoint[ep].dmaEnabled)
        {
            //AA enable DMA request
             TURN_BIT_ON(_pUdcRegisters->UDCCSR[ep],UDC_UDCCSR_DME);
            //TURN_BIT_ON(_pUdcRegisters->UDCCSR[udcEndpoint],UDC_UDCCSR_FEF);//flush the fifo
        }
    }
    /* Make some delay */
    for(i=0; i<1000; i++)
    {
        for(j=0;j<100;j++)
        {
            k++;
        }
    }

    /* Turn the hardware back to OFF */
    UDC_DEACTIVATE_HARDWARE();


     /* Make some delay */
    for(i=0; i<1000; i++)
    {
        for(j=0;j<100;j++)
        {
            k++;
        }
    }

    return TRUE;
} /* End of < UDCDriverConfigureEndpoints > */

/*******************************************************************************
 * Function:    UCDDriverGetInterruptHandler
 *******************************************************************************
 * Description: Reset the UDC hardware
 *
 * Parameters:  udcEndpoint     - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void *UCDDriverGetInterruptHandler(UDC_EndpointE udcEndpoint)
{
    void *func;

    if(_u1dcDefaultEndpointConfig[udcEndpoint].usbEndpointDir == UDC_EP_DIR_IN)
    {/* IN (Tx) */
        if(_udcEndpoint[udcEndpoint].dmaEnabled)
        {/*DMA*/
            func = (void *)UDCDriverInterruptHandlerTransmitWithDma;
        }
        else
        {/*Interrupt*/
            func = (void *)UDCDriverInterruptHandlerTransmitWithInterrupt;
        }
    }
    else
    {/* OUT (Rx) */
        if(_udcEndpoint[udcEndpoint].dmaEnabled)
        {/*DMA*/
            func = (void *)UDCDriverInterruptHandlerReceiveWithDma;
        }
        else
        {/*Interrupt*/
            func = (void *)UDCDriverInterruptHandlerReceiveWithInterrupt;
        }
    }

    return func;
} /* End of < UCDDriverGetInterruptHandler > */

/*******************************************************************************
 * Function:    UDCDriverLoadActiveConfiguration
 *******************************************************************************
 * Description: Loads the configuration registers into the active configuration
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
/*void UDCDriverLoadActiveConfiguration(void)
{
    TURN_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_SMAC);
}*/ /* End of < UDCDriverLoadActiveConfiguration > */

static void UDCDriverCopyToUserBuffer(UDC_EndpointDataS *pUdcEndpoint)
{
     UINT32   user_buf_ind=pUdcEndpoint->lengthCounter; //point to the current index in user buffer
     UINT32   help_buf_ind=pUdcEndpoint->rx_help_buf_copied_length; //point to index in help buffer

     //copy data to user , start from pUdcEndpoint->lengthCounter
    for( ;(user_buf_ind<pUdcEndpoint->totalLength) && (help_buf_ind < pUdcEndpoint->rx_help_buf_read_length); user_buf_ind++,help_buf_ind++)
    {
        pUdcEndpoint->pBuffer[user_buf_ind] = pUdcEndpoint->p_rx_help_buf[help_buf_ind];
    }

    pUdcEndpoint->lengthCounter             = user_buf_ind;
    pUdcEndpoint->rx_help_buf_copied_length = help_buf_ind;
}

/*******************************************************************************
 * Function:    UDCDriverUnloadEndpointFifo
 *******************************************************************************
 * Description: Load endpoint with data - used for interrupt mode
 *
 * Parameters:  pUdcEndpoint *   - pointer to udc endpoint (A-X)
 *
 * Output:      none
 *
 * Returns:     UDC_UnloadStateE - status of unloaded buffer
 *
 * Notes:
 ******************************************************************************/
static UDC_UnloadStateE UDCDriverUnloadEndpointFifo(UDC_EndpointDataS *pUdcEndpoint)
{
    UINT32                  in_fifo_length          = UDC_GET_FIFO_BYTE_COUNT(pUdcEndpoint->udcEndpoint);
    UINT32                  max_packet_size         = UDC_GET_MAX_PACKET_SIZE(pUdcEndpoint->udcEndpoint);


    pUdcEndpoint->rx_help_buf_copied_length = 0;//start new
    pUdcEndpoint->rx_help_buf_read_length   = in_fifo_length;

    //read to help buffer
    UDCDriverReadFromFifo(UDC_GET_FIFO_ADDRESS(pUdcEndpoint->udcEndpoint), pUdcEndpoint->p_rx_help_buf, in_fifo_length);

    UDCDriverCopyToUserBuffer(pUdcEndpoint);

    //if was not enough space in the buffer
    if( pUdcEndpoint->rx_help_buf_copied_length < pUdcEndpoint->rx_help_buf_read_length)
    {
       return(UDC_UNLOAD_BUFFER_FULL);
    }


    /* check if this is short packet */
    if(in_fifo_length < max_packet_size)
    {
        return (UDC_UNLOAD_SHORT_PACKET);
    }
    else //in_fifo_length == max_packet_size & reached total length and not expecting zero length packet
    {
        if((pUdcEndpoint->lengthCounter == pUdcEndpoint->totalLength) &&
           (pUdcEndpoint->state == UDC_EP_STATE_RX_STARTED_NO_ZLP)
          )
            return(UDC_UNLOAD_COMPLETE);
    }


    return (UDC_UNLOAD_NO_ACTION);
} /* End of < UDCDriverUnloadEndpointFifo > */

/*******************************************************************************
 * Function:    UDCDriverLoadEndpointFifo
 *******************************************************************************
 * Description: Load endpoint with data - used for interrupt mode
 *
 * Parameters:  udcEndpoint     - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverLoadEndpointFifo(UDC_EndpointDataS *pUdcEndpoint)
{
    UINT32                  maxPacketSize   = UDC_GET_MAX_PACKET_SIZE(pUdcEndpoint->udcEndpoint);
    BOOL                    shortPacket     = FALSE;
    UINT32                  loadSize;

    if((pUdcEndpoint->totalLength - pUdcEndpoint->lengthCounter) > maxPacketSize)
    {/* size to transmit is bigger than MPS (FIFO size) */
        loadSize    = maxPacketSize;
    }
    else
    {/* last packet size is either exact FIFO size (ZLP is needed) or smaller (short packet) */
        loadSize    = pUdcEndpoint->totalLength - pUdcEndpoint->lengthCounter;

        /* if this is exact MPS then ZLP will be needed in the next interrupt */
        if(loadSize == maxPacketSize)
        {
            /* if user requested for ZLP - set it as needed */
            if(pUdcEndpoint->state == UDC_EP_STATE_TX_ZLP_AUTO)
            {
                pUdcEndpoint->state = UDC_EP_STATE_TX_ZLP_NEEDED;
            }
        }
        else
        {
            shortPacket = TRUE;
        }
    }


    UDCDriverWriteToFifo(UDC_GET_FIFO_ADDRESS(pUdcEndpoint->udcEndpoint), pUdcEndpoint->pBuffer + pUdcEndpoint->lengthCounter, loadSize);

    pUdcEndpoint->lengthCounter += loadSize;
    /* if short packet is needed - set the bit */
    if(shortPacket)
    {
        UDC_SET_SHORT_PACKET(pUdcEndpoint->udcEndpoint);
    }

    //return shortPacket;
} /* End of < UDCDriverLoadEndpointFifo > */


/*******************************************************************************
 * Function:    UDCDriverInterruptHandlerEvent
 *******************************************************************************
 * Description: Reset the UDC hardware
 *
 * Parameters:  endpointNum   - endpoint number
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverInterruptHandlerEvent(UINT32 eventBits)
{
    /* Reset */
    if(eventBits & UDC_UDCISR1_IRRS)
    {
#if defined(UDC_DEBUG)
        _udcDebugData.resetCounter++;
#endif /*UDC_DEBUG*/

        USB1DeviceEventNotify(UDC_EVENT_RESET, 0);
        UDC_CLEAR_EVENT(UDC_UDCISR1_IRRS);
    }

    /* Suspend */
    if(eventBits & UDC_UDCISR1_IRSU)
    {
        if( ((eventBits & UDC_UDCISR1_IRRS) == 0) && ((eventBits & UDC_UDCISR1_IRRU)==0)) //if suspend was the only event arrived
        {

#if defined(UDC_DEBUG)
        _udcDebugData.suspendCounter++;
#endif /*UDC_DEBUG*/

        /* Suspend actions...*/
        UDC_CLEAR_EVENT(UDC_UDCISR1_IRSU);
        USB1DeviceEventNotify(UDC_EVENT_SUSPEND, 0);
        }
        else
        {

#if defined(UDC_DEBUG)
            _udcDebugData.suspendCancelledCounter++;
#endif /*UDC_DEBUG*/


            UDC_CLEAR_EVENT(UDC_UDCISR1_IRSU);
        }

    }

    /* Resume */
    if(eventBits & UDC_UDCISR1_IRRU)
    {
#if defined(UDC_DEBUG)
        _udcDebugData.resumeCounter++;
#endif /*UDC_DEBUG*/

        /* Resume actions... */
        USB1DeviceEventNotify(UDC_EVENT_RESUME, 0);

        UDC_CLEAR_EVENT(UDC_UDCISR1_IRRU);
    }

#if defined(UDC_SOF_DEBUG)
    /* SOF */
    if(eventBits & UDC_UDCISR1_IRSOF)
    {
        _udcDebugData.SOFCounter;

        /* SOF actions... */

        UDC_CLEAR_EVENT(UDC_UDCISR1_IRSOF);
    }
#endif /*UDC_SOF_DEBUG*/

    /* Configuration Change */
    if(eventBits & UDC_UDCISR1_IRCC)
    {
#if defined(UDC_DEBUG)
        _udcDebugData.configChangeCounter++;
#endif /*UDC_DEBUG*/

        /* Load the active configuration into the UDC */
        TURN_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_SMAC);

        USB1DeviceEventNotify(UDC_EVENT_CONFIG_CHANGE, 0);

        UDC_CLEAR_EVENT(UDC_UDCISR1_IRCC);
    }


} /* End of < UDCDriverInterruptHandlerEvent > */

/*******************************************************************************
 * Function:    UDCDriverInterruptHandlerReceiveWithDma
 *******************************************************************************
 * Description: Handles interrupt handler for RECEIVE using DMA
 *
 * Parameters:  endpointNum   - endpoint number
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverInterruptHandlerReceiveWithDma(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits)
{
} /* End of < UDCDriverInterruptHandlerReceiveWithDma > */

/*******************************************************************************
 * Function:    UDCDriverInterruptHandlerReceiveWithInterrupt
 *******************************************************************************
 * Description: Handles interrupt handler for RECEIVE using INTERRUPT
 *
 * Parameters:  endpointNum   - endpoint number
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverInterruptHandlerReceiveWithInterrupt(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits)
{
    volatile UINT32         udccsrReg;
    volatile UINT32 		time_bit_not_set,time_now;
    UDC_UnloadStateE        unloadState = UDC_UNLOAD_NO_ACTION;

    pUdcEndpoint->interrupt_cnt++;

    /* get value of UDCCSR */
    udccsrReg = UDC_GET_UDCCSRAX(pUdcEndpoint->udcEndpoint);
    if( !(UDC_IS_PACKET_COMPLETE(udccsrReg)))
    {

      time_bit_not_set = cp14ReadCCNT();
      do
      {
          udccsrReg = UDC_GET_UDCCSRAX(pUdcEndpoint->udcEndpoint);
          time_now  = cp14ReadCCNT();

      } while( (!(UDC_IS_PACKET_COMPLETE(udccsrReg))) && (time_now-time_bit_not_set < MAX_WAIT_FOR_PC_BIT_SET_AFTER_RX_CNT)   ) ;

    }

    /* Check if Packet-Complete */
    if(UDC_IS_PACKET_COMPLETE(udccsrReg))
    {/* handle the next packet */
        /* BNE_BNF  |  SP  |  meaning          |  action
           ---------------------------------------------------
              0         0     error?              assert
              0         1     ZLP                 notify
              1         0     full fifo packet    read + notify ONLY if rx buffer full
              1         1     short packet        read + notify in any case
         */
        if(UDC_IS_BUFFER_NOT_EMPTY(udccsrReg))
        {
            unloadState = UDCDriverUnloadEndpointFifo(pUdcEndpoint);
            /* read data from fifo and act upon the "needToNotify" */
#if defined(UDC_DEBUG)
            if(unloadState == UDC_UNLOAD_NO_ACTION)
            {/* if no notify - just make sure short-packet not received */
                if(UDC_IS_SHORT_PACKET(udccsrReg))
                {
                    USB_DEVICE_ASSERT(FALSE);
                }
            }
#endif /*UDC_DEBUG*/
        }/*udccsrReg & UDC_UDCCSR_BNE_BNF*/
        else
        {
            if(UDC_IS_SHORT_PACKET(udccsrReg))
            {/* ZLP received: SP=1 & BNE=0*/
                unloadState = UDC_UNLOAD_ZLP;
            }
#if defined(UDC_DEBUG)
            else
            {/* assert for error case */
                USB_DEVICE_ASSERT(FALSE);
            }
#endif /*UDC_DEBUG*/
        }/*else udccsrReg & UDC_UDCCSR_BNE_BNF*/

        /* act if need notification action */
        if(unloadState != UDC_UNLOAD_NO_ACTION)
        {/* need to notify the user */

            //BOOL endOfMessage = (unloadState == UDC_UNLOAD_BUFFER_FULL) ? FALSE : TRUE;
            pUdcEndpoint->state =(unloadState == UDC_UNLOAD_BUFFER_FULL)? UDC_EP_STATE_RX_BUFFER_IS_FULL :UDC_EP_STATE_RX_COMPLETED;

            USB1DeviceReceiveNotifyFn(pUdcEndpoint->usbEndpointNum, pUdcEndpoint->pBuffer, pUdcEndpoint->lengthCounter, TRUE);
        }
        else
        {
            /* clear Packet-Complete in status register */
            //YG UDC_SET_UDCCSRAX(pUdcEndpoint->udcEndpoint, UDC_UDCCSR_PC);
            UDC_CLEAR_PC(pUdcEndpoint->udcEndpoint);
			udccsrReg = UDC_GET_UDCCSRAX(pUdcEndpoint->udcEndpoint);
        }
    }/*udccsrReg & UDC_UDCCSR_PC*/
    else
    {   // after STALL on OUT EP we experience immediate Interrupt which does NOT indicates on PC.
		// to overcome this behavior we "clear stall" on this specific endpoint.
        if(pUdcEndpoint->endpointStalled == UDC_STALL_IN_PROGRESS)
        {
            USB_DEVICE_ASSERT( UDCDriverEndpointClearStall(pUdcEndpoint->udcEndpoint) );
            pUdcEndpoint->state = UDC_EP_STATE_RX_COMPLETED;
        }
#if defined(UDC_DEBUG)
        else
        {
            //wierd
            USB_DEVICE_ASSERT(FALSE);//PC bit was not set, waiting for more than allowed by MAX_WAIT_FOR_PC_BIT_SET_AFTER_RX_CNT
        }
#endif /*UDC_DEBUG*/
    }

    /* clear the event in the interrupt register */
    UDCDriverClearEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);

} /* End of < UDCDriverInterruptHandlerReceiveWithInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverInterruptHandlerTransmitWithDma
 *******************************************************************************
 * Description: Handles interrupt handler for TRANSMIT using DMA
 *
 * Parameters:  endpointNum   - endpoint number
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverInterruptHandlerTransmitWithDma(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits)
{
#if (USB_DEVICE_DMA_ENABLED == 1)
    volatile INT8 i=0,j=0;

    /* clear the event in the interrupt register */
    UDCDriverClearEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);

    UDC_CLEAR_PC(pUdcEndpoint->udcEndpoint);
    for(i=0;i<100;i++)//just short delay after clearing the bit
        j++;

    UDC_CLEAR_EFE(pUdcEndpoint->udcEndpoint);
    for(i=0;i<100;i++)//just short delay after clearing the bit
        j++;


    //set endpoint to DMA mode again
    TURN_BIT_ON(_pUdcRegisters->UDCCSR[pUdcEndpoint->udcEndpoint],UDC_UDCCSR_DME);
    for(i=0;i<100;i++)//just short delay after clearing the bit
        j++;

    UDCDriverDisableEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);

    pUdcEndpoint->state  = UDC_EP_STATE_TX_COMPLETED;

    USB1DeviceTransmitNotifyFn(pUdcEndpoint->usbEndpointNum, 0, pUdcEndpoint->multi_total_buffers ,TRUE);
#endif
} /* End of < UDCDriverInterruptHandlerTransmitWithDma > */

/*******************************************************************************
 * Function:    UDCDriverInterruptHandlerTransmitWithInterrupt
 *******************************************************************************
 * Description: Handles interrupt handler for TRANSMIT using INTERRUPT
 *
 * Parameters:  endpointNum   - endpoint number
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverInterruptHandlerTransmitWithInterrupt(UDC_EndpointDataS *pUdcEndpoint, UINT32 interruptBits)
{
    UINT32                  udccsrReg;

    /* get value of UDCCSR */
    udccsrReg = UDC_GET_UDCCSRAX(pUdcEndpoint->udcEndpoint);

    /* clear the event in the interrupt register */
    UDCDriverClearEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);

    pUdcEndpoint->interrupt_cnt++;

    /* Check if Packet-Complete */
    if(UDC_IS_PACKET_COMPLETE(udccsrReg))
    {/* handle the next packet */
        /* clear Packet-Complete in status register */
        //YG UDC_SET_UDCCSRAX(pUdcEndpoint->udcEndpoint, UDC_UDCCSR_PC);
        UDC_CLEAR_PC(pUdcEndpoint->udcEndpoint);


        /* is transmission done or there is more to send ? */
        if(pUdcEndpoint->lengthCounter < pUdcEndpoint->totalLength)
        {/* There is more to send */
            UDCDriverLoadEndpointFifo(pUdcEndpoint);
        }
        else
        {/* transmission is done */
            if(pUdcEndpoint->state == UDC_EP_STATE_TX_ZLP_NEEDED)
            {/* ZLP needed */
                /* send the ZLP */
                UDC_SET_SHORT_PACKET(pUdcEndpoint->udcEndpoint);

                /* turn it off for next time - ZLP can be only once */
                pUdcEndpoint->state     = UDC_EP_STATE_TX_ZLP_NOT_NEEDED;
            }
            else
            {/* no ZLP */
                UDCDriverDisableEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);
                //pUdcEndpoint->xferCompleted     = TRUE;
                pUdcEndpoint->state     = UDC_EP_STATE_TX_COMPLETED;
                USB1DeviceTransmitNotifyFn(pUdcEndpoint->usbEndpointNum, pUdcEndpoint->pBuffer, pUdcEndpoint->totalLength,TRUE);
                //pUdcEndpoint->transactionCompletedNotifyFn(udcEndpoint, pUdcEndpoint->buffer, pUdcEndpoint->totalLength);
            }
        }
    }
    else
    {
        ASSERT(0);
    }

} /* End of < UDCDriverInterruptHandlerTransmitWithInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverInterruptHandler
 *******************************************************************************
 * Description: The main UDC interrupt handler
 *
 * Parameters:  interruptInfo - interrupt information by INTC
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverInterruptHandler(INTC_InterruptInfo interruptInfo)
{
    int                     epNum;
    UINT32                  statusISR0;
    UINT32                  statusISR1;

    /* Save status of the UDCISR0 */
    statusISR0 = _pUdcRegisters->UDCISR0;

    /* Save status of the UDCISR1 */
    statusISR1 = _pUdcRegisters->UDCISR1;

    /* Check for event interrupts */
    if(statusISR1 & UDC_UDCISR1_EVENTS)
    {
        UDCDriverInterruptHandlerEvent(statusISR1 & UDC_UDCISR1_EVENTS);
    }

    /* Check for EP0 interrupts */
    if(statusISR0 & UDC_UDCISR0_EP0_BITS)
    {
        if(statusISR0 & UDC_UDCISR0_IR0_0)
        {/* Packet-Completed */
            UINT32      udccsr0Reg = UDC_EP0_GET_UDCCSR();

            USB1DeviceEP0InterruptHandler(udccsr0Reg);

            //UDC_CLEAR_INTER_ST_0(UDC_ENDPOINT_0, statusISR0);
            UDC_CLEAR_PACKET_COMPLETE_0(UDC_ENDPOINT_0);
        }
#if defined(UDC_DEBUG)
        if(statusISR0 & UDC_UDCISR0_IR0_1)
        {/* FIFO error */
            USB_DEVICE_ASSERT(FALSE); /* There is no need for that error - overrun/underrun */
        }
#endif /*UDC_DEBUG*/

    }

    /* Check for general endpoints - group 0 interrupts */
    if(statusISR0 & UDC_UDCISR0_GENERAL_EP_MASK)
    {
        UDC_EndpointDataS           *pUdcEndpoint = _udcEndpoint;
        //YG UDC_ActiveEndpointS       *pActiveEndpoint = _udcDatabase.activeEndpoints;

        //YG - need to fix search/release
        //for(epNum=0; epNum<_udcDatabase.totalActiveEndpoints + 1; epNum++)
        for(epNum=0; epNum<UDC_MAX_ACTIVE_ENDPOINTS; epNum++)
        {
			if (pUdcEndpoint->state != UDC_EP_STATE_DEACTIVE)
			{
            	if((UDC_IS_ENDPOINT_GROUP_0(pUdcEndpoint->udcEndpoint)) &&
               	(statusISR0 & pUdcEndpoint->interruptMask))
            	{

                	pUdcEndpoint->interruptHandler((void*)pUdcEndpoint, statusISR0 & ((UDC_EndpointDataS*)pUdcEndpoint)->interruptMask);
                	//UDC_CLEAR_PACKET_COMPLETE_0(pUdcEndpoint->udcEndpoint);
            	}
            }
            pUdcEndpoint++;
        }
    }

    /* Check for general endpoints - group 1 interrupts */
    if(statusISR1 & UDC_UDCISR1_GENERAL_EP_MASK)
    {
        UDC_EndpointDataS           *pUdcEndpoint = _udcEndpoint;
        //YG UDC_ActiveEndpointS       *pActiveEndpoint = _udcDatabase.activeEndpoints;

        //for(epNum=0; epNum<_udcDatabase.totalActiveEndpoints + 1; epNum++)
        for(epNum=0; epNum<UDC_MAX_ACTIVE_ENDPOINTS; epNum++)
        {
			if (pUdcEndpoint->state != UDC_EP_STATE_DEACTIVE)
			{
            	if((UDC_IS_ENDPOINT_GROUP_1(pUdcEndpoint->udcEndpoint)) &&
               	(statusISR1 & pUdcEndpoint->interruptMask))
            	{
                	pUdcEndpoint->interruptHandler((void*)pUdcEndpoint, statusISR1 & pUdcEndpoint->interruptMask);
            	}
            }
            pUdcEndpoint++;
        }
    }

} /* End of < UDCDriverInterruptHandler > */

#if 0 // warning sweep
/*******************************************************************************
 * Function:    UDCDriverEndpointSetupOUTWithDma
 *******************************************************************************
 * Description: Setup an endpoint OUT (rx) to work in DMA mode
 *
 * Parameters:  pUdcEndpoint *  - pointer to udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     BOOL - success or not
 *
 * Notes:
 ******************************************************************************/
#if (USB_DEVICE_DMA_ENABLED ==1)
static BOOL UDCDriverEndpointSetupOUTWithDma(UDC_EndpointDataS *pUdcEndpoint)
{
    USB_DEVICE_ASSERT(FALSE);//YG not implemented yet

    return TRUE;
} /* End of < UDCDriverEndpointSetupOUTWithDma > */
#endif
#endif // 0

/*******************************************************************************
 * Function:    UDCDriverEndpointSetupINWithDma
 *******************************************************************************
 * Description: Setup an endpoint IN (tx) to work in DMA mode
 *
 * Parameters:  pUdcEndpoint *  - pointer to udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     BOOL - success or not
 *
 * Notes:
 ******************************************************************************/
static BOOL UDCDriverEndpointSetupINWithDma(UDC_EndpointDataS *pUdcEndpoint)
{
#if (USB_DEVICE_DMA_ENABLED == 1)
    UINT32                      numDescriptors=0;
    UINT32                      udccsrAddr;
    volatile UINT32             udccr_plus_sp_bit;
    DMA_DescriptorsHWConfig     descHWConfig;
    DMA_SingleHWConfig          singleHWConfig;
    //UDC_EndpointDataS           *pUdcEndpoint = &_udcEndpoint[udcEndpoint];


    /* check if DMA is free */
    if( DMAIsChannelRunning(pUdcEndpoint->dmaChannel) )  //AA - put it all in one function UDCDriverEndpointSetupINWithDma ??
        return FALSE;

    /* check DMA buffer alignment */
    if( !DMAIsBufferAligned(pUdcEndpoint->pBuffer) )
        return FALSE;

    //pUdcEndpoint->totalLength = 64; //AA for test
    // check if length is multiple of FIFO size -> ZLP is required
    if((pUdcEndpoint->totalLength % UDC_GET_MAX_PACKET_SIZE(pUdcEndpoint->udcEndpoint)) == 0)
    {
        //ZLP is needed
        if(pUdcEndpoint->state == UDC_EP_STATE_TX_ZLP_AUTO)
        {//Two descriptors are needed
            pUdcEndpoint->state                 = UDC_EP_STATE_TX_ZLP_NEEDED;
            numDescriptors                      = 2;
            descHWConfig.interruptsEnableBits   = DMA_INTERRUPT_NONE;
            singleHWConfig.peripheralFlow       = TRUE;
        }
        else
        {/*only 1 descriptor & NEED AN INTERRUPT AT THE END OF THAT DESCRIPTOR !!!  */
            pUdcEndpoint->state                 = UDC_EP_STATE_TX_ZLP_NOT_NEEDED;
            numDescriptors                      = 1;
            descHWConfig.interruptsEnableBits   = DMA_INTERRUPT_TRANSFER_ENDED;
        }
    }
    else// not multiple of fifo , will need 2 descriptors
    {//Short packet will be sent (autoZLP parameter is ignored)
        pUdcEndpoint->state                 = UDC_EP_STATE_TX_ZLP_IGNORED;
        numDescriptors                      = 2;
        descHWConfig.interruptsEnableBits   = DMA_INTERRUPT_NONE;
    }

    /* prepare descHWConfig */
    /*descHWConfig.interruptsEnableBits       = above  */
    descHWConfig.addressIncrement           = DMA_ADDRESS_INCREMENT_SOURCE;
    descHWConfig.flowControl                = DMA_FLOW_CONTROL_TARGET;
    descHWConfig.byteOrdering               = DMA_LITTLE_ENDIAN;
    descHWConfig.burstSize                  = DMA_BURST_32_BYTES;
    //descHWConfig.dataWidth                  = DMA_DATA_WIDTH_ONE_BYTE; //AA ???
   descHWConfig.dataWidth                  = DMA_DATA_WIDTH_FOUR_BYTES; //AA ???  , according to spec, 32bit word alligned

    /* prepare the chain of 2 descriptors */
    if(DMADescriptorsChainPrepare(pUdcEndpoint->dmaChannel, numDescriptors,
                                  pUdcEndpoint->pDescBuffer, pUdcEndpoint->descBufferSize) != DMA_RC_OK)
        return FALSE;

    /* setup the first descriptor (0) */
    if(DMASingleDescriptorSetup(pUdcEndpoint->dmaChannel, 0, pUdcEndpoint->pBuffer, (void *)UDC_GET_FIFO_ADDRESS(pUdcEndpoint->udcEndpoint),
                                pUdcEndpoint->totalLength, &descHWConfig) != DMA_RC_OK)
        return FALSE;

    /* setup the second descriptor if needed */
    if(numDescriptors == 2)
    {
        /*prepare singleHWConfig (for immediateCommand) */
        singleHWConfig.interruptsEnableBits     = DMA_INTERRUPT_TRANSFER_ENDED;
        singleHWConfig.peripheralAddrIncrement  = FALSE;
        singleHWConfig.peripheralFlow           = FALSE;
        singleHWConfig.byteOrdering             = DMA_LITTLE_ENDIAN;
        singleHWConfig.burstSize                = DMA_BURST_32_BYTES;
        singleHWConfig.dataWidth                = DMA_DATA_WIDTH_FOUR_BYTES;//AA ???  , according to spec, 32bit word alligned

        /* calculate the UDCCS address register for SHORT-PACKET (SP) bit command at end of transfer */
        //YG udccsAddr = (UINT32)(&(_udcRegs_p->UDCCS[endpoint]));
        udccsrAddr = (UINT32)(UDC_GET_UDCCSR_ADDRESS(pUdcEndpoint->udcEndpoint));

        udccr_plus_sp_bit = _pUdcRegisters->UDCCSR[pUdcEndpoint->udcEndpoint];
        TURN_BIT_ON(udccr_plus_sp_bit,UDC_UDCCSR_SP);

        /* set the short-packet (SP) bit in the command */
        if(DMAImmediateCommandSetup(pUdcEndpoint->dmaChannel, 1, udccr_plus_sp_bit,
                                    (void *)udccsrAddr, 1, &singleHWConfig) != DMA_RC_OK)
            return FALSE;
    }

    DMAChannelStart(pUdcEndpoint->dmaChannel);
#endif
    return TRUE;
} /* End of < UDCDriverEndpointSetupINWithDma > */




/*******************************************************************************
 * Function:    UDCDriverEndpointSetupINMultiTransmitWithDma
 *******************************************************************************
 * Description: Setup an endpoint IN (tx) to work in DMA mode
 *
 * Parameters:  pUdcEndpoint *  - pointer to udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     BOOL - success or not
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverEndpointSetupINMultiTransmitWithDma(UDC_EndpointE udcEndpoint,
                        UINT32 numOfBuffers, USBDevice_MultiTransmitListS *multiList)
{
#if (USB_DEVICE_DMA_ENABLED == 1)
    UDC_EndpointDataS *pUdcEndpoint = &_udcEndpoint[udcEndpoint];
    void *epFifoRegAddr             = (void *)UDC_GET_FIFO_ADDRESS(pUdcEndpoint->udcEndpoint);
    UINT32 maxPacketSize            = UDC_GET_MAX_PACKET_SIZE(pUdcEndpoint->usbEndpointNum);
	UINT32 maxDmaTransferSize		= GET_DMA_TRANSFER_LIMIT(maxPacketSize);
    UINT32 dmaChannel               = pUdcEndpoint->dmaChannel;

    DMA_ReturnCode dma_return_code;
    DMA_DescriptorsHWConfig descHWConfig;


    volatile UINT32 i=0, j=0, desc_ind=0, big_packet_bytes_towrite=0, total_big_packet_bytes=0;
    volatile UINT32 packet_remained_bytes_num=0,first_write_bytes_num=0,packet_left_over_length=0,max_packet_alligned_length=0;
    volatile UINT32 enable_bits,total_length_test, total_length_test_addr, dma_descriptors_total_length;//to verify length
	BOOL	bLastIter=FALSE;

    total_length_test = ((UINT16*)(multiList->pTxBuff))[0];
    total_length_test_addr = (UINT32)((UINT16*)(multiList->pTxBuff));
    dma_descriptors_total_length=0;


    // check that packet is complete
    ASSERT ( (_pUdcRegisters->UDCCSR[udcEndpoint] & UDC_UDCCSR_PC) == 0); //bit is not set  ===> buffer is empty

    //verify endpoint fields are valid

    ASSERT ( (pUdcEndpoint->state  !=  UDC_EP_STATE_MULTI_TX_STARTED) &&
             (pUdcEndpoint->state  !=  UDC_EP_STATE_MULTI_TX_DMA_NOTIFY_RECEIVED));//if arrived to transmit while previous transmit is still pending


    //init endpoint fields
    pUdcEndpoint->IN_bytes_num_in_fifo             = 0;
    pUdcEndpoint->totalLength                      = 0;
    pUdcEndpoint->multi_total_buffers              = numOfBuffers;

    UDC_DBG_DEBUG_MSG_3PARAMS(udc_mt_dbg_buff,UDC_DGB_START_OF_TRANSMIT,numOfBuffers,total_length_test,UDC_DRIVER_DEBUG_ENABLED);

    // numOfBuffers +  ZERO length packet + end of chain
    dma_return_code = DMADescriptorsChainPrepare(dmaChannel, (numOfBuffers*3)+1+3, pUdcEndpoint->pDescBuffer,
            pUdcEndpoint->descBufferSize);
    ASSERT(dma_return_code == DMA_RC_OK);


    //prepare descHWConfig
    descHWConfig.interruptsEnableBits       = DMA_INTERRUPT_NONE;
    descHWConfig.addressIncrement           = DMA_ADDRESS_INCREMENT_SOURCE;
    descHWConfig.flowControl                = DMA_FLOW_CONTROL_TARGET;
    descHWConfig.byteOrdering               = DMA_LITTLE_ENDIAN;
    descHWConfig.burstSize                  = DMA_BURST_32_BYTES;
    descHWConfig.dataWidth                  = DMA_DATA_WIDTH_FOUR_BYTES;


    //each USB buffer transmitted is handled to be max_packet_size bytes
    for(i=0; i < numOfBuffers; i++) // total buffer
    {
       pUdcEndpoint->totalLength+=multiList->bufferLength;
       UDC_DBG_DEBUG_MSG_1PARAM(udc_mt_dbg_buff,multiList->bufferLength,UDC_DRIVER_DEBUG_ENABLED);
       if((i+1) == numOfBuffers) // set notification only for last descriptor
       {
            descHWConfig.interruptsEnableBits  = DMA_INTERRUPT_TRANSFER_ENDED;
       }
       else
           descHWConfig.interruptsEnableBits  = DMA_INTERRUPT_NONE;

	   if(multiList->bufferLength + pUdcEndpoint->IN_bytes_num_in_fifo  <= maxPacketSize) //there is a room for the whole packet
       {
		     dma_return_code = DMASingleDescriptorSetup(dmaChannel, desc_ind, multiList->pTxBuff, epFifoRegAddr,
                                        multiList->bufferLength, &descHWConfig);
             ASSERT(dma_return_code == DMA_RC_OK);
             dma_descriptors_total_length+=multiList->bufferLength; //to verify length
             desc_ind++;
             pUdcEndpoint->IN_bytes_num_in_fifo  += multiList->bufferLength;

             UDC_DBG_DEBUG_MSG_2PARAMS(udc_mt_dbg_buff,UDC_DGB_WRITE_LESS_THAN_FIFO,pUdcEndpoint->IN_bytes_num_in_fifo,UDC_DRIVER_DEBUG_ENABLED);
       }
       else
       {
		    enable_bits = descHWConfig.interruptsEnableBits; //saved configured bits
            descHWConfig.interruptsEnableBits  = DMA_INTERRUPT_NONE;
            if(pUdcEndpoint->IN_bytes_num_in_fifo  != 0) //if need to complete the fifo
            {
                first_write_bytes_num = maxPacketSize - pUdcEndpoint->IN_bytes_num_in_fifo ;

                dma_return_code = DMASingleDescriptorSetup(dmaChannel, desc_ind, multiList->pTxBuff, epFifoRegAddr,
                                        first_write_bytes_num, &descHWConfig);//till the end of fifo
                ASSERT(dma_return_code == DMA_RC_OK);
                dma_descriptors_total_length+=first_write_bytes_num;//to verify length
                desc_ind++;
                pUdcEndpoint->IN_bytes_num_in_fifo  = 0; //fifo is empty
            }
            else
            {
                first_write_bytes_num  = 0;  //no first write
            }

            UDC_DBG_DEBUG_MSG_2PARAMS(udc_mt_dbg_buff,UDC_DGB_WRITE_TILL_COMPLETE_FIFO,first_write_bytes_num,UDC_DRIVER_DEBUG_ENABLED);

            packet_remained_bytes_num = multiList->bufferLength-first_write_bytes_num;
            packet_left_over_length = packet_remained_bytes_num  % maxPacketSize; // less than max
            max_packet_alligned_length  = packet_remained_bytes_num - packet_left_over_length;

            ASSERT( (packet_left_over_length > 0 && packet_left_over_length <=multiList->bufferLength) ||
                    (max_packet_alligned_length>0 && max_packet_alligned_length <= multiList->bufferLength)
                  );

            if(max_packet_alligned_length) // chunks of max_packet size
            {
		    	total_big_packet_bytes = max_packet_alligned_length;
	 		   	for (j=0; j<((max_packet_alligned_length/maxDmaTransferSize)+1) && (total_big_packet_bytes > 0); j++)
				{
					bLastIter = !((j+1)<((max_packet_alligned_length/maxDmaTransferSize)+1));
					big_packet_bytes_towrite = ((total_big_packet_bytes /maxDmaTransferSize) < 1) ? total_big_packet_bytes : maxDmaTransferSize;
 					pUdcEndpoint->max_packet_alligned_desc_cnt++;
                    if(big_packet_bytes_towrite > maxPacketSize)
						pUdcEndpoint->max_packet_alligned_big_desc_cnt++;
					else
						pUdcEndpoint->short_packet_desc_cnt++;

					if ( bLastIter && (packet_left_over_length == 0)) // last iteration & no tail, so it is the last descriptor for this packet
                    	descHWConfig.interruptsEnableBits = enable_bits;

					dma_return_code = DMASingleDescriptorSetup(dmaChannel, desc_ind, multiList->pTxBuff +first_write_bytes_num +(maxDmaTransferSize *j), epFifoRegAddr,
	                                           big_packet_bytes_towrite, &descHWConfig);
	                ASSERT(dma_return_code == DMA_RC_OK);
					total_big_packet_bytes -= maxDmaTransferSize;
	                desc_ind++;
    				UDC_DBG_DEBUG_MSG_2PARAMS(udc_mt_dbg_buff,UDC_DGB_WRITE_MULTIPLE_FIFO,big_packet_bytes_towrite,UDC_DRIVER_DEBUG_ENABLED);
				}
	            dma_descriptors_total_length+=max_packet_alligned_length;//to verify length

            }

            if(packet_left_over_length) //write the tail into the fifo
            {
                pUdcEndpoint->short_packet_desc_cnt++;
                descHWConfig.interruptsEnableBits  = enable_bits;

                dma_return_code = DMASingleDescriptorSetup(dmaChannel, desc_ind, (multiList->pTxBuff+first_write_bytes_num + max_packet_alligned_length) , epFifoRegAddr,
                                        packet_left_over_length, &descHWConfig);
                ASSERT(dma_return_code == DMA_RC_OK);
                dma_descriptors_total_length+=packet_left_over_length;//to verify length
                desc_ind++;

                pUdcEndpoint->IN_bytes_num_in_fifo  += packet_left_over_length;
                UDC_DBG_DEBUG_MSG_2PARAMS(udc_mt_dbg_buff,UDC_DGB_WRITE_LEFT_OVER_FIFO,packet_left_over_length,UDC_DRIVER_DEBUG_ENABLED);
            }
        }


        if(pUdcEndpoint->IN_bytes_num_in_fifo  == maxPacketSize)
            pUdcEndpoint->IN_bytes_num_in_fifo  = 0;

        multiList++;
    }


    ASSERT (descHWConfig.interruptsEnableBits == DMA_INTERRUPT_TRANSFER_ENDED); //verify that last descriptor, interrupt is enabled
// YS - assert bellow commented as preperation for DIAG new header and since UDC shouldn't interfere with message content.
//    ASSERT((total_length_test + 4) == pUdcEndpoint->totalLength); //verify that length specified in header is valid
    ASSERT(dma_descriptors_total_length == pUdcEndpoint->totalLength);//sum of lengths of all DMA descriptors must be equal to total transfer

    UDC_DBG_DEBUG_MSG_1PARAM(udc_mt_dbg_buff,pUdcEndpoint->totalLength,UDC_DRIVER_DEBUG_ENABLED);


    //will set short packet bit manually
    dma_return_code = DMASingleDescriptorControl(dmaChannel, desc_ind-1, TRUE, TRUE);
    ASSERT(dma_return_code == DMA_RC_OK);

    pUdcEndpoint->state                     = UDC_EP_STATE_MULTI_TX_STARTED;

    DMAChannelStart(dmaChannel);

#endif
    return(TRUE);
}



/*******************************************************************************
 *                      G L O B A L   F U N C T I O N S
 ******************************************************************************/

/*******************************************************************************
 * Function:    UDCDriverEndpointSetupOUT
 *******************************************************************************
 * Description: Setup an endpoint OUT (rx)
 *
 * Parameters:  udcEndpoint     - udc endpoint number (A-X)
 *              pRxBuffer       - buffer to transmit
 *              length          - length of buffer
 *
 * Output:      none
 *
 * Returns:     BOOL - success or not
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverEndpointSetupOUT(UDC_EndpointE udcEndpoint,
                               UINT8 *pRxBuffer, UINT32 length,BOOL expect_zlp)
{
    UDC_EndpointDataS       *pUdcEndpoint = &_udcEndpoint[udcEndpoint];


    /*handling stall case - if required */
    if(_udcEndpoint[udcEndpoint].endpointStalled == UDC_STALL_IN_PROGRESS)
    {
        USB_DEVICE_ASSERT( UDCDriverEndpointClearStall(pUdcEndpoint->udcEndpoint) );
    }

     if(pRxBuffer)
           pUdcEndpoint->pBuffer           = pRxBuffer;

     if(pUdcEndpoint->state != UDC_EP_STATE_RX_BUFFER_IS_FULL)
     pUdcEndpoint->state             = (expect_zlp == TRUE) ? UDC_EP_STATE_RX_STARTED : UDC_EP_STATE_RX_STARTED_NO_ZLP;

     if(length)
         pUdcEndpoint->totalLength       = length;



    /*check mode of operation */
    if(pUdcEndpoint->dmaEnabled == TRUE) //YG - is there a need for minimum ??? I don't think so... : && (length > USB_MIN_DMA_BUFFER_LENGTH) )
    {/* DMA */
        /* check if DMA is free */
       // if( DMAIsChannelRunning(pUdcEndpoint->dmaChannel) )
        //    return FALSE;

        //if( !UDCDriverEndpointSetupOUTWithDma(pUdcEndpoint) )
        //    return FALSE;

        //DMAChannelStart(pUdcEndpoint->dmaChannel);
    }
    else
    {/* Interrupt */
        pUdcEndpoint->lengthCounter     = 0;

        //means that there is a reminder in the buffer , ins such case copy the remains and call notify
        if(pUdcEndpoint->state == UDC_EP_STATE_RX_BUFFER_IS_FULL)
        {
            UDCDriverCopyToUserBuffer(pUdcEndpoint);
            if(pUdcEndpoint->rx_help_buf_copied_length < pUdcEndpoint->rx_help_buf_read_length)
            {
                USB1DeviceReceiveNotifyFn(pUdcEndpoint->usbEndpointNum, pUdcEndpoint->pBuffer, pUdcEndpoint->lengthCounter, FALSE);
                return TRUE;
            }
            else
            {
                pUdcEndpoint->state = UDC_EP_STATE_RX_COMPLETED;
                USB1DeviceReceiveNotifyFn(pUdcEndpoint->usbEndpointNum, pUdcEndpoint->pBuffer, pUdcEndpoint->lengthCounter, TRUE);
            }
        }
        /* Interrupt */
		/* clear the status to be ready for next packet */
        UDCDriverEndpointClearReceiveStatus(pUdcEndpoint->udcEndpoint);

		/* Interrupt */
        UDCDriverEnableEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);

    }

    return TRUE;
} /* End of < UDCDriverEndpointSetupOUT > */

/*******************************************************************************
 * Function:    UDCDriverEndpointSetupIN
 *******************************************************************************
 * Description: Setup an endpoint IN (tx)
 *
 * Parameters:  udcEndpoint     - udc endpoint number (A-X)
 *              pTxBuffer       - buffer to transmit
 *              length          - length of buffer
 *              autoZLP         - whether to transmit ZLP automaticly or not
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/

BOOL UDCDriverEndpointSetupIN(UDC_EndpointE udcEndpoint,
                              UINT8 *pTxBuffer, UINT32 length, BOOL autoZLP)
{
    UDC_EndpointDataS       *pUdcEndpoint = &_udcEndpoint[udcEndpoint];

    /*handling stall case - if required */
    if(_udcEndpoint[udcEndpoint].endpointStalled == UDC_STALL_IN_PROGRESS)
    {
        USB_DEVICE_ASSERT( UDCDriverEndpointClearStall(pUdcEndpoint->udcEndpoint) );
    }

#if defined(USE_STRANGE_CODE)   //YG - is this needed?
    if( ((_pUdcRegisters->UDCCSR[udcEndpoint]) & UDC_UDCCSR_PC) == UDC_UDCCSR_PC )
    {
        _pUdcRegisters->UDCCSR[udcEndpoint] = UDC_UDCCSR_PC;
    }
#else
     if(pUdcEndpoint->dmaEnabled == FALSE) //several packets can arriave before 1 exits
     {
         if( ((_pUdcRegisters->UDCCSR[udcEndpoint]) & UDC_UDCCSR_PC) == UDC_UDCCSR_PC )
        {
         //if we get here then we may need to enable to upper part or search for code-design errors
         USB_DEVICE_ASSERT(FALSE);  //maybe use USB_ASSERT for all
        }
     }
#endif
        //pUdcEndpoint->xferCompleted     = FALSE;
        pUdcEndpoint->state             = (autoZLP == TRUE) ? UDC_EP_STATE_TX_ZLP_AUTO : UDC_EP_STATE_TX_ZLP_NOT_NEEDED;
        pUdcEndpoint->pBuffer           = pTxBuffer;
        pUdcEndpoint->totalLength       = length;
        pUdcEndpoint->lengthCounter     = 0;
        pUdcEndpoint->interrupt_cnt     = 0;
        //YG pUdcEndpoint->zlpState          = (autoZLP == TRUE) ? UDC_ZLP_AUTO : UDC_ZLP_NOT_NEEDED;

    /*check mode of operation and minimal DMA size */
    if(pUdcEndpoint->dmaEnabled == TRUE) //YG - is there a need for minimum ??? I don't think so... : && (length > USB_MIN_DMA_BUFFER_LENGTH) )
    {/* DMA */


        if( !UDCDriverEndpointSetupINWithDma(pUdcEndpoint) )
            return FALSE;
    }
    else
    {/* Interrupt */
        unsigned long cpsrReg;

        cpsrReg = disableInterrupts();

		//enable interrupt before loading fifo
		UDCDriverEnableEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);


        UDCDriverLoadEndpointFifo(pUdcEndpoint);

        restoreInterrupts(cpsrReg);
    }

    return TRUE;
} /* End of < UDCDriverEndpointSetupIN > */

/*******************************************************************************
 * Function:    UDCDriverReadFromFifo
 *******************************************************************************
 * Description: Read data from any FIFO
 *
 * Parameters:  pFifo       - fifo address to write to
 *              buffer      - source data
 *              length      - length of buffer
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:       buffer must be aligned
 ******************************************************************************/
void UDCDriverReadFromFifo(volatile UINT32 *pFifo, UINT8 *buffer, UINT32 length)
{

    int         count;
    volatile    UINT32      *ptr32bit = (UINT32 *)buffer;
    volatile    UINT32      ptr_not_4_bytes_alligned; //last tbytes
    volatile    UINT8       *ptr8bit;

    /* Make sure the buffer is aligned to 32bits (4 bytes) */
    USB_DEVICE_ASSERT((((UINT32)buffer) & UDC_FIFO_32BITS_ALIGNMENT) == 0);


    /* first - make all 32bits access operations (div by 4) */
    for(count=0; count < (length>>2); count++)
    {
        *ptr32bit++ = *pFifo;
    }

    /* check if length not a multiple of 4 (32bits access) */
    if(((UINT32) length & UDC_FIFO_32BITS_ALIGNMENT) != 0)
    {/* copy the reminder */
        ptr8bit = (UINT8 *)ptr32bit;
        ptr_not_4_bytes_alligned = *pFifo;

        for(count=0; count < (length & UDC_FIFO_32BITS_ALIGNMENT); count++)
        {
            *ptr8bit++ = ((UINT8*)(&ptr_not_4_bytes_alligned))[count];
        }
    }
} /* End of < UDCDriverReadFromFifo > */

/*******************************************************************************
 * Function:    UDCDriverWriteToFifo
 *******************************************************************************
 * Description: Write data into any FIFO
 *
 * Parameters:  pFifo       - fifo address to write to
 *              buffer      - source data
 *              length      - length of buffer
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:       buffer must be aligned
 ******************************************************************************/
void UDCDriverWriteToFifo(volatile UINT32 *pFifo, UINT8 *buffer, UINT32 length)
{
    UINT32       i;
    UINT32      next4bytes=0,left_over,aligned_length;

    left_over      = (length & 0x3);
    aligned_length = (length - left_over);

    for(i=0;i<aligned_length;i+=4) //zero if length is less than 4 bytes ==> this loop is not performed
    {
        ((UINT8*)(&next4bytes))[3] = buffer[i+3];
		((UINT8*)(&next4bytes))[2] = buffer[i+2];
		((UINT8*)(&next4bytes))[1] = buffer[i+1];
		((UINT8*)(&next4bytes))[0] = buffer[i+0];

        *pFifo = next4bytes;
    }

    if(left_over)
    {
        for(i=0;i<left_over;i++)
         *((UINT8 *)pFifo)  = buffer[aligned_length+i];
    }

} /* End of < UDCDriverWriteToFifo > */


/*******************************************************************************
 * Function:    UDCDriverEndpointFlush
 *******************************************************************************
 * Description: Flushes endpoint FIFO and status
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
#if (USB_DEVICE_DMA_ENABLED ==1)
static UINT16 udc_dma_stopped=0;
#endif
void UDCDriverEndpointFlush(UDC_EndpointE udcEndpoint)
{
    BOOL dma_is_runing = FALSE;

    if(UDC_IS_ENDPOINT_0(udcEndpoint))
    {/* Control endpoint (EP0)*/
        TURN_BIT_ON(_pUdcRegisters->UDCCSR[UDC_ENDPOINT_0], UDC_UDCCSR0_FTF);
        //YG...
    }
    else/* General endpoint */
    {
        //check if DMA  and TX started , if yes stop DMA channel

#if (USB_DEVICE_DMA_ENABLED ==1)
        if(_udcEndpoint[udcEndpoint].dmaEnabled && _udcEndpoint[udcEndpoint].state == UDC_EP_STATE_MULTI_TX_STARTED)
        {
            if(DMAIsChannelRunning(_udcEndpoint[udcEndpoint].dmaChannel))
            {
                dma_is_runing = TRUE;
                udc_dma_stopped++;
                DMAChannelStop(_udcEndpoint[udcEndpoint].dmaChannel, FALSE);
            }
        }
 #endif
        if(!dma_is_runing)//flush only if no DMA is running
        {
              TURN_BIT_ON(_pUdcRegisters->UDCCSR[udcEndpoint], UDC_UDCCSR_FEF);
        }


        _udcEndpoint[udcEndpoint].lengthCounter                    = 0;
        _udcEndpoint[udcEndpoint].interrupt_cnt                    = 0;
        _udcEndpoint[udcEndpoint].max_packet_alligned_big_desc_cnt = 0;
        _udcEndpoint[udcEndpoint].max_packet_alligned_desc_cnt     = 0;
        _udcEndpoint[udcEndpoint].multi_total_buffers              = 0;
        _udcEndpoint[udcEndpoint].rx_help_buf_copied_length        = 0;
        _udcEndpoint[udcEndpoint].rx_help_buf_read_length          = 0;
        _udcEndpoint[udcEndpoint].short_packet_desc_cnt            = 0;

        if(_udcEndpoint[udcEndpoint].state != UDC_EP_STATE_DEACTIVE)
        {
            _udcEndpoint[udcEndpoint].state = UDC_EP_STATE_ACTIVE;
        }
    }
} /* End of < UDCDriverEndpointFlush > */

/*******************************************************************************
 * Function:    UDCDriverEndpointStall
 *******************************************************************************
 * Description: Stalls endpoint
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     true if entered stall state, false otherwise
 *
 * Notes: sets FST bit in UDCCSRA - UDCCSRX
 ******************************************************************************/
BOOL UDCDriverEndpointStall(UDC_EndpointE udcEndpoint)
{
    USB_DEVICE_ASSERT(UDC_GET_ENDPOINT_TYPE(udcEndpoint) != UDC_EP_ISO);

    if (_udcEndpoint[udcEndpoint].endpointStalled == UDC_STALL_IN_PROGRESS) //to make sure this function is not reentrant for the same endpoint.
    {
        USB_DEVICE_ASSERT(FALSE);
        UDC_CLEAR_STALL(udcEndpoint);
    }

    _udcEndpoint[udcEndpoint].endpointStalled = UDC_STALL_IN_PROGRESS;

    //UDCDriverEndpointFlush(udcEndpoint); - may be required

	UDC_SET_STALL_ENDPOINT(udcEndpoint);


	return TRUE;

}

/*******************************************************************************
 * Function:    UDCDriverEndpointClearStall
 *******************************************************************************
 * Description: Clears stall condition
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     true if SST is clear , false otherwise
 *
 * Notes: sets SST bit in UDCCSRA - UDCCSRX
 ******************************************************************************/
BOOL UDCDriverEndpointClearStall(UDC_EndpointE udcEndpoint)
{
	BOOL ret = FALSE;

    if(_udcEndpoint[udcEndpoint].endpointStalled == UDC_STALL_COMPLETED ||
       _udcEndpoint[udcEndpoint].endpointStalled == UDC_STALL_DISABLE)  // if clear was called from two sources (i.e USBDeviceEndpointStall, UDCDriverEndpointSetupIN, UDCDriverEndpointSetupOUT, UDCDriverInterruptHandlerReceiveWithInterrupt)
    {
        _udcEndpoint[udcEndpoint].endpointStalled = UDC_STALL_DISABLE;
        ret = TRUE;
    }
    else if(_udcEndpoint[udcEndpoint].endpointStalled == UDC_STALL_IN_PROGRESS)
    {
        if(UDC_IS_SST_SET(_pUdcRegisters->UDCCSR[udcEndpoint]))
        {
            _udcEndpoint[udcEndpoint].endpointStalled = UDC_STALL_COMPLETED;
            UDC_CLEAR_STALL(udcEndpoint);
            ret = TRUE;
        }
    }

	return ret;
}

/*******************************************************************************
 * Function:    UDCDriverEndpointClearReceiveStatus
 *******************************************************************************
 * Description: Flushes endpoint FIFO and status
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverEndpointClearReceiveStatus(UDC_EndpointE udcEndpoint)
{
	UINT32 cpsrReg;
    //YG - maybe need to test bits first
    /* clears both PC & EFE bits in register */
	cpsrReg = disableInterrupts();
    UDC_CLEAR_PC_EFE(udcEndpoint);
	restoreInterrupts(cpsrReg);
} /* End of < UDCDriverEndpointClearReceiveStatus > */

/*******************************************************************************
 * Function:    UDCDriverEnableEventInterrupt
 *******************************************************************************
 * Description: Enables a specific event interrupt
 *
 * Parameters:  event - the interrupt event
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverEnableEventInterrupt(UDC_DeviceEventE event)
{
    UINT32          eventBit;

    eventBit = UDC_CONVERT_EVENT_TO_BIT(event);

    TURN_BIT_ON(_pUdcRegisters->UDCICR1, eventBit);

} /* End of < UDCDriverEnableEventInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverDisableEventInterrupt
 *******************************************************************************
 * Description: Disables a specific event interrupt
 *
 * Parameters:  event - the interrupt event
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverDisableEventInterrupt(UDC_DeviceEventE event)
{
    UINT32          eventBit;

    eventBit = UDC_CONVERT_EVENT_TO_BIT(event);

    TURN_BIT_OFF(_pUdcRegisters->UDCICR1, eventBit);

} /* End of < UDCDriverDisableEventInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverClearEventInterrupt
 *******************************************************************************
 * Description: Clears a specific event interrupt
 *
 * Parameters:  event - the interrupt event
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverClearEventInterrupt(UDC_DeviceEventE event)
{
    UINT32          eventBit;

    eventBit = UDC_CONVERT_EVENT_TO_BIT(event);

    TURN_BIT_OFF(_pUdcRegisters->UDCISR1, eventBit);

} /* End of < UDCDriverClearEventInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverEnableEndpointInterrupt
 *******************************************************************************
 * Description: Enables specific endpoint interrupt
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *              interruptType - type of interrupt (packet / fifo)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverEnableEndpointInterrupt(UDC_EndpointE udcEndpoint,
                                      UDC_EndpointInterruptTypeE interruptType)
{
    UINT32          bitNum;
    UINT32 cpsrReg;

    if(UDC_IS_ENDPOINT_GROUP_0(udcEndpoint))
    {
        /* Calculate the exact bit number: shift endpointNum<<1 - to multiple by 2 */
        bitNum  = (udcEndpoint << 1) | interruptType;

        cpsrReg = disableInterrupts();
        TURN_BIT_ON(_pUdcRegisters->UDCICR0, (((UINT32)1) << bitNum));
        restoreInterrupts(cpsrReg);
    }
    else
    {
        udcEndpoint -= UDC_ENDPOINT_GROUP_LIMIT; /* align to zero */
        /* Calculate the exact bit number: shift endpointNum<<1 - to multiple by 2 */
        bitNum  = (udcEndpoint << 1) | interruptType;

        cpsrReg = disableInterrupts();
        TURN_BIT_ON(_pUdcRegisters->UDCICR1, (((UINT32)1) << bitNum));
        restoreInterrupts(cpsrReg);
    }
} /* End of < UDCDriverEnableEndpointInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverDisableEndpointInterrupt
 *******************************************************************************
 * Description: Disables specific endpoint interrupt
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *              interruptType - type of interrupt (packet / fifo)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverDisableEndpointInterrupt(UDC_EndpointE udcEndpoint,
                                       UDC_EndpointInterruptTypeE interruptType)
{
    UINT32          bitNum;
    UINT32 cpsrReg;

    if(UDC_IS_ENDPOINT_GROUP_0(udcEndpoint))
    {
        /* Calculate the exact bit number: shift endpointNum<<1 - to multiple by 2 */
        bitNum  = (udcEndpoint << 1) | interruptType;

        cpsrReg = disableInterrupts();
        TURN_BIT_OFF(_pUdcRegisters->UDCICR0, (((UINT32)1) << bitNum));
        restoreInterrupts(cpsrReg);
    }
    else
    {
        udcEndpoint -= UDC_ENDPOINT_GROUP_LIMIT; /* align to zero */
        /* Calculate the exact bit number: shift endpointNum<<1 - to multiple by 2 */
        bitNum  = (udcEndpoint << 1) | interruptType;

        cpsrReg = disableInterrupts();
        TURN_BIT_OFF(_pUdcRegisters->UDCICR1, (((UINT32)1) << bitNum));
        restoreInterrupts(cpsrReg);
    }
} /* End of < UDCDriverDisableEndpointInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverClearEndpointInterrupt
 *******************************************************************************
 * Description: Clears specific endpoint interrupt
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *              interruptType - type of interrupt (packet / fifo)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverClearEndpointInterrupt(UDC_EndpointE udcEndpoint,
                                     UDC_EndpointInterruptTypeE interruptType)
{
    UINT32          bitNum;
	volatile UINT32 read_UDCISRx;

    if(UDC_IS_ENDPOINT_GROUP_0(udcEndpoint))
    {
        /* Calculate the exact bit number: shift endpointNum<<1 - to multiple by 2 */
        bitNum  = (udcEndpoint << 1) | interruptType;

        _pUdcRegisters->UDCISR0 =  (((UINT32)1) << bitNum);
		read_UDCISRx = _pUdcRegisters->UDCISR0;
    }
    else
    {

        udcEndpoint -= UDC_ENDPOINT_GROUP_LIMIT; /* align to zero */
        /* Calculate the exact bit number: shift endpointNum<<1 - to multiple by 2 */
        bitNum  = (udcEndpoint << 1) | interruptType;

       _pUdcRegisters->UDCISR1 =  (((UINT32)1) << bitNum);
	   read_UDCISRx = _pUdcRegisters->UDCISR1;

    }


} /* End of < UDCDriverClearEndpointInterrupt > */

/*******************************************************************************
 * Function:    UDCDriverEndpointDeactivate
 *******************************************************************************
 * Description: Deactivate endpoint
 *
 * Parameters:  udcEndpoint   - udc endpoint number (A-X)
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:       ERROR !!!! need to close hole when deleting en EP !!!!!
 ******************************************************************************/
BOOL UDCDriverEndpointDeactivate(UDC_EndpointE udcEndpoint)
{
    int                         epNum;
    UDC_EndpointDataS           *pUdcEndpoint = &_udcEndpoint[udcEndpoint];

    /* are there any activated endpoints */
    if(_udcDatabase.totalActiveEndpoints == 0)
    {
        return FALSE;
    }

    /* search for the endpoint */
    for(epNum=0; epNum<UDC_MAX_ACTIVE_ENDPOINTS; epNum++)
    {
        if((pUdcEndpoint->udcEndpoint == udcEndpoint) && (pUdcEndpoint->interruptHandler != NULL))
        {
            break;
        }
        pUdcEndpoint++;
    }

    USB_DEVICE_ASSERT(epNum < UDC_MAX_ACTIVE_ENDPOINTS);

    /* if endpoint not found - MAYBE it's wrong to free un-activted endpoint */
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* Notes:       ERROR !!!! need to close hole when deleting en EP !!!!!       */
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

    pUdcEndpoint->udcEndpoint                   = UDC_INVALID_ENDPOINT;
    pUdcEndpoint->usbEndpointNum                = 0;
    pUdcEndpoint->state                         = UDC_EP_STATE_DEACTIVE;
    pUdcEndpoint->dmaEnabled                    = FALSE;
    pUdcEndpoint->dmaChannel                    = 0xFF;
    pUdcEndpoint->pDescBuffer                   = NULL;
    pUdcEndpoint->descBufferSize                = 0;
    pUdcEndpoint->interruptHandler              = NULL;

    _udcDatabase.totalActiveEndpoints--;

    return TRUE;
} /* End of < UDCDriverEndpointDeactivate > */


/************************************************************************
* Function: UDCDriverDMATransferCompletedNotify
*************************************************************************
* Description: This functions will be notified when transfer is completed
*              by the DMA for TRANSMITTING (IN)
*
* Parameters:  channel - dma channel, endpoint
*
* Return value: void
*
* Notes: if function is NULL - no call will made to that function
************************************************************************/
#if (USB_DEVICE_DMA_ENABLED == 1)
static void UDCDriverDMATransferCompletedNotify(UINT32 channel, UINT32 udcEndpoint)
{

    UDC_EndpointDataS *pUdcEndpoint = &_udcEndpoint[udcEndpoint];

    volatile BOOL short_packet_loaded=FALSE,full_packet_transmitted=FALSE;
    volatile UINT32 i,j,curr_status=0;

    if(pUdcEndpoint->state == UDC_EP_STATE_MULTI_TX_STARTED)  // multitransmit dma
    {

        //notification cases :
        //1. short packet
        //2. full packet

        //last packet  & there is short packet
        pUdcEndpoint->state  = UDC_EP_STATE_MULTI_TX_DMA_NOTIFY_RECEIVED;

        curr_status = _pUdcRegisters->UDCCSR[udcEndpoint];
        UDC_DBG_DEBUG_MSG_1PARAM(udc_nt_dbg_buff,((UINT32)0xee000000 | (UINT32)curr_status),UDC_DRIVER_DEBUG_ENABLED);

//
// The below code fragment waits for certain condition before setting the SHORT_PACKET bit, enabling the PC interrupt and exiting the ISR
//
       {
		UINT32 waitStartTs = cp14ReadCCNT();

        if(pUdcEndpoint->IN_bytes_num_in_fifo) // wait for status - short packet is loaded in buffer
        {
            do
            {
				// UDC_UDCCSR.FS && UDC_UDCCSR.BNE_BNF
                short_packet_loaded = ((UDC_IS_ROOM_IN_FIFO(curr_status)) && (UDC_IS_BUFFER_NOT_EMPTY(curr_status)));
                curr_status = _pUdcRegisters->UDCCSR[udcEndpoint];
                if((cp14ReadCCNT()-waitStartTs)>MAX_WAIT_FOR_STATUS_TIME) break;
            }
            while((short_packet_loaded == FALSE) );
         }
         else  //wait for status full packet transmitted
         {
             do
             {
				 // UDC_UDCCSR.PC && UDC_UDCCSR.FS && UDC_UDCCSR.BNE_BNF
                 full_packet_transmitted= ((UDC_IS_PACKET_COMPLETE(curr_status)) && (UDC_IS_ROOM_IN_FIFO(curr_status)) && (UDC_IS_BUFFER_NOT_FULL(curr_status)));
                 curr_status = _pUdcRegisters->UDCCSR[udcEndpoint];
                 if((cp14ReadCCNT()-waitStartTs)>MAX_WAIT_FOR_STATUS_TIME) break;
             }
             while((full_packet_transmitted == FALSE));

         }

          UDC_DBG_DEBUG_MSG_1PARAM(udc_nt_dbg_buff,((UINT32)0xeeAA0000 | (UINT32)curr_status),UDC_DRIVER_DEBUG_ENABLED);
          UDC_DBG_DEBUG_MSG_1PARAM(udc_nt_dbg_buff,wait_for_status_cnt,UDC_DRIVER_DEBUG_ENABLED);
	   }

#if !defined(UDC_DMA_TX_DONT_WAIT_FOR_PC_FS)
	      ASSERT(short_packet_loaded || full_packet_transmitted);
#endif
          UDC_CLEAR_PC(pUdcEndpoint->udcEndpoint);
          for(i=0;i<100;i++)//just short delay after clearing the bit
             j++;

          //turn DME off
          TURN_BIT_OFF(_pUdcRegisters->UDCCSR[udcEndpoint],UDC_UDCCSR_DME);
          for(i=0;i<100;i++)//just short delay after clearing the bit
             j++;


          //enable packet complete interrupt
          UDCDriverEnableEndpointInterrupt(pUdcEndpoint->udcEndpoint, UDC_INTERRUPT_PACKET_COMPLETE);

          UDC_SET_SHORT_PACKET(pUdcEndpoint->udcEndpoint);

    }
    else  //simple DMA not multitransmit
    {
        pUdcEndpoint->state  = UDC_EP_STATE_TX_COMPLETED;
    }

} /* End of < UDCDriverDMATransmitCompletedNotify > */
#endif


/************************************************************************
* Function: USBDmaTransferErrorNotify
*************************************************************************
* Description: Register USER-Callback functions onto the USB functions
*
* Parameters:  the functions
*
* Return value: void
*
* Notes: if function is NULL - no call will made to that function
************************************************************************/
#if (USB_DEVICE_DMA_ENABLED == 1)
static void UDCDriverDMATransferErrorNotify(UINT32 channel, UINT32 endpoint)
{
    ASSERT(0);
}
#endif


/************************************************************************
* Function: UDCDriverEndpointDMAChannelActivate
*************************************************************************
* Description: activate DMA chanel for specific endpoint
*
* Parameters:  endpoint - the USB endpoint
*
* Return value: BOOL
*
* Notes: NON-Reentrant
************************************************************************/
static BOOL    UDCDriverEndpointDMAChannelActivate(UDC_EndpointE    udcEndpoint)
{
    BOOL result = TRUE;
#if (USB_DEVICE_DMA_ENABLED == 1)
     UDC_EndpointDataS           *pUdcEndpoint = &_udcEndpoint[udcEndpoint];

     /* mapping the endpoint peripheral number onto the dma channel */
     result = (DMAChannelConfigure(pUdcEndpoint->dmaChannel,
                UDC_GET_DMA_PERIPHERAL(udcEndpoint), DMA_ADDRESS_NOT_ALIGNED) == DMA_RC_OK);

    if( UDC_IS_ENDPOINT_OUT(udcEndpoint) )
    {//RX

        ASSERT(0); //not supported
    }
    else
    {//TX
         if(result)
            result = (DMAChannelRegister(pUdcEndpoint->dmaChannel,
                            NULL, // No transfer Started notification - USBDmaTransferStartedNotify
                            UDCDriverDMATransferCompletedNotify,
                            NULL,
                            UDCDriverDMATransferErrorNotify,
                            (UINT32)udcEndpoint) == DMA_RC_OK);

        // open the dma channel
        if(result)
            result = (DMAChannelOpen(pUdcEndpoint->dmaChannel) == DMA_RC_OK);

    }
#endif
    return result;
} /* End of < USBEndpointOpenWithDma > */

/*******************************************************************************
 * Function:    UDCDriverEndpointActivate
 *******************************************************************************
 * Description: Activate endpoint for use
 *
 * Parameters:  udcEndpoint     - udc endpoint number (A-X)
 *              dmaEnabled      - using DMA or not
 *              dmaChannel      - if DMA - channel number
 *              pDescBuffer     - if DMA - descriptor buffer
 *              descBufferSize  - if DMA - descriptor size
 *              transactionCompletedNotifyFn - notify function
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverEndpointActivate(UDC_EndpointE    udcEndpoint,
                               UINT8            usbEndpointNum,
                               BOOL             dmaEnabled,     UINT8 dmaChannel,
                               UINT8            *pDescBuffer,   UINT32 descBufferSize)
{
    BOOL result=TRUE;

    UDC_EndpointDataS           *pUdcEndpoint = &_udcEndpoint[udcEndpoint];
    /* YG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

    /* check number of activated endpoints */
    if(_udcDatabase.totalActiveEndpoints >= UDC_MAX_ACTIVE_ENDPOINTS)
    {
        USB_DEVICE_ASSERT(FALSE);//YG temp debug assert
        //return UDC_HANDLE_ERROR;
        return FALSE;
    }

    /* search for an empty place */
    /* YG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
    /* no search currently - after fixing all search & release */
#if 0
    for(epNum=0; epNum<UDC_MAX_ACTIVE_ENDPOINTS; epNum++)
    {
        if(pUdcEndpoint->interruptHandler == NULL)
        {
            break;
        }
        pUdcEndpoint++;
    }
    /* if no free place found this is a bug */
    USB_DEVICE_ASSERT(epNum < UDC_MAX_ACTIVE_ENDPOINTS);
#endif


    pUdcEndpoint->udcEndpoint                   = udcEndpoint;
    pUdcEndpoint->usbEndpointNum                = usbEndpointNum;
    pUdcEndpoint->state                         = UDC_EP_STATE_ACTIVE;
    pUdcEndpoint->pBuffer                       = NULL;
    pUdcEndpoint->totalLength                   = 0;
    pUdcEndpoint->lengthCounter                 = 0;
    pUdcEndpoint->interruptMask                 = UDC_GET_PACKET_COMPLETE_INTERRUPT_MASK(udcEndpoint);
    pUdcEndpoint->p_rx_help_buf                 = 0;
    pUdcEndpoint->rx_help_buf_copied_length     = 0;
    if(dmaEnabled)
    {/* init DMA */


        pUdcEndpoint->dmaEnabled                = TRUE;
        pUdcEndpoint->dmaChannel                = dmaChannel;
        pUdcEndpoint->pDescBuffer               = pDescBuffer;
        pUdcEndpoint->descBufferSize            = descBufferSize;

        result = UDCDriverEndpointDMAChannelActivate(udcEndpoint);

    }
    else
    {/* no DMA */
        pUdcEndpoint->dmaEnabled                = FALSE;
        pUdcEndpoint->dmaChannel                = 0xFF;
        pUdcEndpoint->pDescBuffer               = NULL;
        pUdcEndpoint->descBufferSize            = 0;
        pUdcEndpoint->p_rx_help_buf             = malloc(UDC_GET_MAX_PACKET_SIZE(pUdcEndpoint->udcEndpoint));
        USB_DEVICE_ASSERT (((UINT32)pUdcEndpoint->p_rx_help_buf & 0x3) == 0);
    }

    pUdcEndpoint->interruptHandler              = (UDCDriverEndpointInterruptHandler)UCDDriverGetInterruptHandler(udcEndpoint);
    _udcDatabase.totalActiveEndpoints++;

    //return ((UDC_HANDLE)pUdcEndpoint);
    return result;
} /* End of < UDCDriverEndpointActivate > */

/*******************************************************************************
 * Function:    UDCDriverGetCurrentConfigurationSettings
 *******************************************************************************
 * Description: Returns the current configuration parameters
 *
 * Parameters:  none
 *
 * Output:      configNum - configuration number
 *              interfaceNum - interface number
 *              altSettingsNum - alternate settings number
 *
 * Returns:     BOOL - UDC is active or not
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverGetCurrentConfigurationSettings(UINT8 *configNum, UINT8 *interfaceNum, UINT8 *altSettingsNum)
{
    UINT32          udccr;

    udccr = _pUdcRegisters->UDCCR;

    *configNum      = (udccr & UDC_UDCCR_ACN_MASK)      >> UDC_UDCCR_ACN_SHIFT;
    *interfaceNum   = (udccr & UDC_UDCCR_AIN_MASK)      >> UDC_UDCCR_AIN_SHIFT;
    *altSettingsNum = (udccr & UDC_UDCCR_AAISN_MASK)    >> UDC_UDCCR_AAISN_SHIFT;

    return (IS_BIT_ON(udccr, UDC_UDCCR_UDA));
} /* End of < UDCDriverGetCurrentConfigurationSettings > */

/*******************************************************************************
 * Function:    UDCDriverIsHostEnabledRemoteWakeup
 *******************************************************************************
 * Description: Returns whether the HOST enabled remote-wakeup feature on the
 *              device
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     BOOL - HOST enabled or not
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverIsHostEnabledRemoteWakeup(void)
{
    return (IS_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_DRWF));
} /* End of < UDCDriverIsHostEnabledRemoteWakeup > */

/*******************************************************************************
 * Function:    UDCDriverForceHostResume
 *******************************************************************************
 * Description: Forces HOST out of Suspend state
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverForceHostResume(void)
{
    TURN_BIT_ON(_pUdcRegisters->UDCCR, UDC_UDCCR_UDR);
} /* End of < UDCDriverForceHostResume > */

/*******************************************************************************
 * Function:    UDCDriverResumeHardware
 *******************************************************************************
 * Description: Forces UDC out of Suspend state
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:      called
 ******************************************************************************/

void UDCDriverResume(void)
{
} /* End of < UDCDriverResumeHardware > */

/*******************************************************************************
 * Function:    UDCDriverWakeupClear
 *******************************************************************************
 * Description:  Clear the wakeup indication
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
#if defined(UDC_USE_WAKEUP)
static UINT32 UDCDriverWakeupClear(void)
{
	#define ACR1_USB_WK_STATUS 0x00040000
    volatile UINT32 *ACR1_addr = (volatile UINT32*)0x42900068;
	UINT32 ACR1_val=*ACR1_addr;
	*ACR1_addr = ACR1_val & (~ACR1_USB_WK_STATUS); //write 0 bit 18;
    *ACR1_addr = ACR1_val | ACR1_USB_WK_STATUS;//write 1 bit 18
	return ACR1_val&ACR1_USB_WK_STATUS;
}

/*******************************************************************************
 * Function:    UDCDriverWakeupHandler
 *******************************************************************************
 * Description: Reset the UDC hardware
 *
 * Parameters:  interruptInfo
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
static void UDCDriverWakeupDisable(void)
{
	//disable wake up event detection in the pad - prevents UDC from working correcly
	_pUdcPadRegisters->UPIWKUPEN = 0;
	UDCDriverWakeupClear();
    PMUEWakeupControl(0,PMUE_WAKE_URE|PMUE_WAKE_USB2);
    INTCDisable(INTC_SRC_USB_OTG_PAD2_WAKEUP);
}

static void UDCDriverWakeupEnable(void)
{
	_pUdcPadRegisters->UPIWKUPEN = 0x00000002; //enable wake up event
    UDCDriverWakeupClear();
    PMUEWakeupControl(PMUE_WAKE_URE|PMUE_WAKE_USB2,PMUE_WAKE_URE|PMUE_WAKE_USB2);
	INTCEnable(INTC_SRC_USB_OTG_PAD2_WAKEUP);
}

static void UDCDriverWakeupHandler(INTC_InterruptInfo interruptInfo)
{
	UDCDriverWakeupDisable();
    UDC_CLOCK_CONTROL(PMU_ON);
    INTCEnable(INTC_SRC_USB);
#if defined(UDC_USE_WAKEUP_LED)
    usbWakeIndication(USB_AWAKE);
#endif //UDC_USE_WAKEUP_LED
}

#endif //UDC_USE_WAKEUP

/*******************************************************************************
 * Function:    UDCDriverSuspend
 *******************************************************************************
 * Description: Put the UDC into Suspend state
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverSuspend(void)
{
#if defined(UDC_USE_WAKEUP)
    INTCDisable(INTC_SRC_USB); //prevent
	//turn off USB clocks
	UDC_CLOCK_CONTROL(PMU_OFF);
	UDCDriverWakeupEnable();
#if defined(UDC_USE_WAKEUP_LED)
    usbWakeIndication(USB_SUSPENDED);
#endif //UDC_USE_WAKEUP_LED

#endif //UDC_USE_WAKEUP

} /* End of < UDCDriverSuspend > */


#if defined(UDC_USE_WAKEUP_LED)
#include "bsp_hisr.h"
#include "PMChip.h"
static OS_HISR usbWakeHisr;
static UINT8 usbAwakeFlag;

void usbWakeHisrFunc(void)
{
 PMC_Led_Params_t parm={PMC_FIRST_LED, PMC_LED_ENABLE, PMC_LED_NO_FLASH, PMC_LED_DC_0625, PMC_LED_INTENS_125};
 if(usbAwakeFlag&USB_AWAKE)
 {
	parm.ledId=PMC_SECOND_LED;//green
 	parm.enable = PMC_LED_ENABLE;
    PMCLEDControl(&parm);
	parm.ledId=PMC_THIRD_LED;//green
 	parm.enable = PMC_LED_DISABLE;
    PMCLEDControl(&parm);
 }
 else
 {
	parm.ledId=PMC_SECOND_LED;//green
 	parm.enable = PMC_LED_DISABLE;
    PMCLEDControl(&parm);
	parm.ledId=PMC_THIRD_LED;//green
 	parm.enable = PMC_LED_ENABLE;
    PMCLEDControl(&parm);
 }
 //cable
	parm.ledId=PMC_FIRST_LED;//blue
 	parm.enable = (usbAwakeFlag&USB_CABLE_PLUGIN)?PMC_LED_ENABLE:PMC_LED_DISABLE;
    PMCLEDControl(&parm);
}
void usbWakeIndication(UINT8 awake)
{
	switch(awake)
	{
		case USB_AWAKE: usbAwakeFlag|=USB_AWAKE; break;
		case USB_SUSPENDED: usbAwakeFlag&=~USB_AWAKE; break;
		case USB_CABLE_PLUGIN: usbAwakeFlag|=USB_CABLE_PLUGIN; break;
		case USB_CABLE_PLUGOUT: usbAwakeFlag&=~USB_CABLE_PLUGIN; break;
		default: return;
	}

	OS_Activate_HISR(&usbWakeHisr);
}
#endif //UDC_USE_WAKEUP_LED

static void USB1ConfigureWakeup(void)
{
#if defined(UDC_USE_WAKEUP)
   _pUdcPadRegisters->UMCE = 0xD;
   _pUdcPadRegisters->UPWER     = 0x00000004;
#if defined(UDC_USE_WAKEUP_LED)
   OS_Create_HISR ( &usbWakeHisr, "UsbWake", usbWakeHisrFunc,HISR_PRIORITY_0);
#endif //UDC_USE_WAKEUP_LED
#endif //UDC_USE_WAKEUP
}


/*******************************************************************************
 * Function:    UDCDriverDeactivateHardware
 *******************************************************************************
 * Description: Turn OFF UDC hardware (not to be use for suspend)
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverDeactivateHardware(void)
{
    UINT32 cpsrReg;
    volatile UINT32 j,k=0;

    cpsrReg = disableInterrupts();
#if defined(UDC_USE_WAKEUP)
    UDCDriverWakeupDisable();
#endif

    UDC_CLOCK_CONTROL(PMU_ON);          // make sure UDC registers are accessible (e.g. cable out event handling after previous suspend)

    UDCDriverDisableEventInterrupt(UDC_EVENT_CONFIG_CHANGE);
    UDCDriverDisableEventInterrupt(UDC_EVENT_RESET); //alla
    UDCDriverDisableEventInterrupt(UDC_EVENT_SUSPEND); //alla
    UDCDriverDisableEventInterrupt(UDC_EVENT_RESUME); //alla
    UDCDriverDisableEndpointInterrupt(UDC_ENDPOINT_0, UDC_INTERRUPT_PACKET_COMPLETE);
	// Must clear these otherwise a pending interrupt may prevent sleep entry (clock resume) even when UDC interrupt is masked
	UDC_CLEAR_EVENT(UDC_UDCISR1_IRRS);
	UDC_CLEAR_EVENT(UDC_UDCISR1_IRSU);
	UDC_CLEAR_EVENT(UDC_UDCISR1_IRRU);
    INTCDisable(INTC_SRC_USB);
    restoreInterrupts(cpsrReg);

    //actually unplug disables d+ pull up
    //_pUdcRegisters->UP2OCR = 0; // Host port 2 transceiver UP20CR: [HXOE]=0 (disable), [HXS]=0 (select DEVICE mode); [DPPUE]=0 (disable PU on D+)

	//this will enable PD for D+ and D-, this will cause lower current consumption in stby.
	_pUdcRegisters->UP2OCR = 0x0c; // Host port 2 transceiver UP20CR: [HXOE]=0 (disable), [HXS]=0 (select DEVICE mode); [DPPUE]=0 (disable PU on D+); UP2OCR[DPPDE,DMPDE]=1 ;

      /* Make some delay */
       for(j=0;j<100;j++)
       {
            k++;
       }

    UDC_DEACTIVATE_HARDWARE();

      /* Make some delay */
      for(j=0;j<100;j++)
      {
            k++;
      }

    //UDC_CLOCK_CONTROL(PMU_OFF);

    //YG _udcDatabase.pmuON = FALSE;


} /* End of < UDCDriverDeactivateHardware > */

/*******************************************************************************
 * Function:    UDCDriverActivateHardware
 *******************************************************************************
 * Description: Enables to UDC hardware into normal operation
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     BOOL - success or not
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverActivateHardware(void)
{
    UINT32 cpsrReg;

    cpsrReg = disableInterrupts();

	#if !defined(_TAVOR_BOERNE_)
    _pUdcPadRegisters->UPWER     = 0x00000004;
	#endif /*_TAVOR_BOERNE_*/


    UDC_CLOCK_CONTROL(PMU_ON);

    INTCEnable(INTC_SRC_USB);



    UDC_ACTIVATE_HARDWARE();




    _pUdcRegisters->UP2OCR = 0x00020010;// Host port 2 transceiver UP20CR: [HXOE]=1 (enable), [HXS]=0 (select DEVICE mode); [DPPUE]=1 (enable PU on D+)


    //alla -  it looks like those should be before UDC_ACTIVATE_HARDWARE ?? need to check
    UDCDriverEnableEventInterrupt(UDC_EVENT_CONFIG_CHANGE);
#if (SUPPORT_RESET_EVENT ==1)
	UDCDriverEnableEventInterrupt(UDC_EVENT_RESET); //alla
    UDCDriverEnableEventInterrupt(UDC_EVENT_SUSPEND); //alla
    UDCDriverEnableEventInterrupt(UDC_EVENT_RESUME);
#endif
    UDCDriverEnableEndpointInterrupt(UDC_ENDPOINT_0, UDC_INTERRUPT_PACKET_COMPLETE);

    //YG _udcDatabase.pmuON = TRUE;



    restoreInterrupts(cpsrReg);

    return TRUE;
} /* End of < UDCDriverActivateHardware > */


/*******************************************************************************
 * Function:    UDCDriverPhase2Init
 *******************************************************************************
 * Description: Phase 2 of initialization process
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
BOOL UDCDriverPhase2Init(void)
{
	/* YS - Turn the hardware OFF before UDC configuration completed, added since in some cases
	 *  OS-Loader/Fuse-override (in Tavor B0) enable UDE for the sake of Image loading process via PC application */
	UDC_DEACTIVATE_HARDWARE();
//Hongji 201011
   // INTCConfigure(INTC_SRC_USB, INTC_IRQ, INTC_RISING_EDGE, /*GPIO_PULL_UP_DOWN_DISABLE*/ GPIO_DEBOUNCE_DISABLE);

	INTCConfigure(INTC_SRC_USB, INTC_IRQ, INTC_HIGH_LEVEL);

    INTCBind(INTC_SRC_USB, UDCDriverInterruptHandler);

#if defined(UDC_USE_WAKEUP)
    INTCConfigure(INTC_SRC_USB_OTG_PAD2_WAKEUP, INTC_IRQ, INTC_RISING_EDGE, GPIO_PULL_UP_DOWN_DISABLE);
    INTCBind(INTC_SRC_USB_OTG_PAD2_WAKEUP, UDCDriverWakeupHandler);
#endif
    USB1ConfigureWakeup();
    return TRUE;
} /* End of < UDCDriverPhase2Init > */

//YG
void UDCPadActivate(void)
{
#if !defined(_TAVOR_BOERNE_)
    //_pUdcPadRegisters->UMCE         = 0x0F;
    _pUdcPadRegisters->USRSTC       = 0x01;
#endif

} /* End of < UDCPadActivate > */

/*******************************************************************************
 * Function:    UDCDriverPhase1Init
 *******************************************************************************
 * Description: Phase 1 of initialization process
 *
 * Parameters:  none
 *
 * Output:      none
 *
 * Returns:     none
 *
 * Notes:
 ******************************************************************************/
void UDCDriverPhase1Init(void)
{
    UDCDriverDatabaseReset(TRUE);

    //_pUdcRegisters      = (UDC_RegistersS *)UDC_REGISTERS_BASE;
    //_pUdcPadRegisters   = (UDC_PadRegistersS *)UDC_PAD_REGISTERS_BASE;
    //_pUdcRegisters      = (UDC_RegistersS *)UDC_REGISTERS_BASE;
    //_pUdcPadRegisters   = (UDC_PadRegistersS *)UDC_PAD_REGISTERS_BASE;

    /*enable UDC hardware */
    UDC_CLOCK_CONTROL(PMU_ON);

    /*activate the USB pad*/
    UDCPadActivate();

} /* End of < UDCDriverPhase1Init > */

BOOL  UDCDriverIsDeviceControllerEnabled (void)
{
    if (UDC_IS_UDC_ACTIVATED())
        return TRUE;
    else
        return FALSE;
}


//----------------------------------------------------------------------------------------------------------
//---------------------------------DMA related functions----------------------------------------------------
//----------------------------------------------------------------------------------------------------------
