/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/******************************************************************************
*              MODULE IMPLEMENTATION FILE
*******************************************************************************
*  COPYRIGHT (C) 2002 Intel Corporation.
*
*  This file and the software in it is furnished under
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
*  Title: SSP_DEFS
*
*  Filename: SSP_DEFS.H
*
*  Target, subsystem: Common Platform, HAL
*
*  Authors: Alexander Rivman
*
*  Description:  SSP implementation file.
*
*  Last Modified: <date>
*
*  Notes: Internal definitions of the SSP
*
******************************************************************************/
#ifndef _SSP_DEFS_H
#define _SSP_DEFS_H	(1)

#define INITIAL_TIMEOUT_VALUE                       100
#define	SSP_FIFO_SIZE								16
#define LOG2_OF_2									1 // use with '<<' to multiply by 2
#define LOG2_OF_4									2 // use with '<<' to multiply by 4
#define THREE_BUFFERS_ATLEAST						3
//#define SSP_SSTO_TIME_OUT_VALUE						(0x1450)
//#define SSP_SSTO_TIME_OUT_VALUE						(0x1450>>3)
#define SSP_SSTO_TX_TIME_OUT_VALUE                     0x1450         //16*32bit at 6.5MHz -->79 microSec timeout  --> 0x1000

#ifdef  _QT_ // Change the SSP Time out for rx
	#define SSP_SSTO_RX_TIME_OUT_VALUE                     0xf3c0
#else
	#define SSP_SSTO_RX_TIME_OUT_VALUE                     0x5f3700      //120 mSec timeout due to 100mSec timeout in the Xilinx
#endif

/* ZVI : tout value 5200 instead of 52000*/
//#define SSP_SSTO_TIME_OUT_VALUE                     1450
#define	ONE_DESCRIPTOR								1
#define	MAX_DMA_LEN									0x1FFF // 7K-1
#define TX_SSP_DMA_TRANSFER_SIZE_IN_BYTES           4*4    //number of bytes per DMA transaction
#define SCRATCH_32BYTE								64 // for Rx buffer in Tx_Only mode. DMA in "non-increment" smearth burst size of bytes
#define SSP_MEMORY_SIZE                             (0xC00UL)
#define SSP_MIN_MEMORY_ALLOCATION_SIZE              (0x4UL)
#if defined (_QT_)
    #define SSP_CLK_OFF_TIMER_INTERVAL                  10/OS_TICK_INTERVAL_IN_MILISEC    //meir - QT is slower then real chip by factor of ~2000
#else
	#define SSP_CLK_OFF_TIMER_INTERVAL                 10/OS_TICK_INTERVAL_IN_MILISEC    //idan - test
#endif

/*-------- Local macro definitions ------------------------------------------*/
#define CHECK_PORT_NUM_LEGAL(portNumber)                    if (SSP_NUMBER_OF_PORTS  <= (UINT8)portNumber)               \
                                                                return(SSP_RC_BAD_PORT_NUM)
#define CHECK_PORT_OPENED(portNumber)                       if (SSPPortsStatusArray[portNumber].portOpen == FALSE)       \
                                                                 return(SSP_RC_PORT_CLOSED)
#define CHECK_PORT_CLOSED(portNumber)                       if (SSPPortsStatusArray[portNumber].portOpen == TRUE)        \
                                                                 return(SSP_RC_PORT_OPENED)
#define CHECK_PORT_CONFIGURED(portNumber)                   if (SSPPortsStatusArray[portNumber].portConfigured == 0)     \
                                                                 return(SSP_RC_PORT_NOT_CONFIGURED);
#define CHECK_POINTER_VALID(pointer)                        if (pointer==NULL)                                           \
                                                                 return(SSP_RC_NULL_POINTER)
#define CHECK_ALLIGNED_32_BIT(pointer)                      if ( (UINT32)pointer & 0x3 )                                 \
                                                                 return(SSP_RC_POINTER_NOT_ALLIGNED_32_BIT)
#define CHECK_ALLIGNED_16_BIT(pointer)                      if ( (UINT32)pointer & 0x1 )                                 \
                                                                 return(SSP_RC_POINTER_NOT_ALLIGNED_16_BIT)
#define CHECK_DMA_MODE(DMAMode)                             if ( DMAMode!=SSP_DMA_Enable )                               \
                                                                 return(SSP_RC_ILLEGAL_DMA_MODE_TYPE)
#define CHECK_RECEIVE_THRESHOLD_VALUE(receiveValue)         if (( receiveValue>15 ) || ( receiveValue==0 ))              \
                                                                 return(SSP_RC_ILLEGAL_RECEIVE_THRESHOLD_VALUE)
#define CHECK_TRANSMIT_THRESHOLD_VALUE(transmitValue)       if (( transmitValue>15 ) || ( transmitValue==0 ))            \
                                                                 return(SSP_RC_ILLEGAL_TRANSMIT_THRESHOLD_VALUE)
#define CHECK_TIME_OUT_VALUE(numberOfBitsBeforeTimeOut)     if ( numberOfBitsBeforeTimeOut>0xFFFFFF )                    \
                                                                 return(SSP_RC_ILLEGAL_TIME_OUT_VALUE)
#define CHECK_RX_MODE(portNumber)                           if (SSP_PortConfigurationArray[portNumber].oprationMode != SSP_Rx_ONLY) \
                                                                return(SSP_RC_RX_ONLY_MODE_NOT_SELECTED)
#define CHECK_TX_MODE(portNumber)                           if (SSP_PortConfigurationArray[portNumber].oprationMode == SSP_Rx_ONLY) \
                                                                return(SSP_RC_TX_MODE_NOT_SELECTED)
#define CHECK_SAMPLE_SIZE_VALUE(numDataBits)                if ((numDataBits > 32) || (numDataBits < 4))                \
                                                                return(SSP_RC_ILLEGAL_NUMBER_OF_DATA_BIT_PER_SAMPLE_VALUE)

#define CLOCK_RATE_IN_HZ                                   (13000000)
#define BAUD_RATE_VALID(baudRate)                          ((CLOCK_RATE_IN_HZ/ (CLOCK_RATE_IN_HZ/(baudRate)) ) == (baudRate))
#define CALCULATE_SCR_FROM_BAUD_RATE(baudRate)             ((CLOCK_RATE_IN_HZ/(baudRate))-1)


#endif


