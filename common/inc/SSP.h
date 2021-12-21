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
*  Title: SSP
*
*  Filename: SSP.h
*
*  Target, subsystem: Common Platform, HAL
*
*  Authors: Yanai Oron
*			Zafi Dayan (Porting to Manitoba)
*
*  Description:  SSP Header file.
*
*  Last Modified: <date>
*
******************************************************************************/




#ifndef _SSP_H_
#define _SSP_H_

#include "Typedef.h"


/*----------- Global defines -------------------------------------------------*/

/*----------- Global type definitions ----------------------------------------*/


typedef enum
{
    SSP_PORT_0 = 0,
    SSP_PORT_1 = 1,
    SSP_PORT_2 = 2,

    SSP_NUMBER_OF_PORTS
} SSP_Port;



// define call back function to notify client
typedef void (*sspNotifyElementsReceived)(SSP_Port   portNumber, UINT32  *pRxBuffer, UINT32  copiedElementsNumber);
typedef void (*sspNotifyElementsSent)(SSP_Port   portNumber, UINT32  *pRxBuffer, UINT32 *pTxBuffer, UINT32 numberOfElements);
typedef void (*sspNotifyReceiveOverflow)(SSP_Port  portNumber);


/*  Common Parameters for the Configuration Structure : */
typedef enum
{
    SSP_Motorola_SPI,
    SSP_Texas_Instruments_SSP,
    SSP_National_Semiconductor_Microwire,
    SSP_PSP
} SSP_FrameFormat;


typedef enum
{
    SSP_OnChipClock = 0,
    SSP_NetworkClock,
    SSP_ExternalClock
} SSP_ClockSource;

typedef enum
{
	SSP_Rx_ONLY = 0,
    SSP_Tx_Rx,
	SSP_Tx_ONLY
} SSP_OprationMode;

typedef enum
{
    SSP_DMA_Enable = 0,
    SSP_DMA_Disable
} SSP_DMAMode;


typedef enum
{
    SSP_CLOCK_MASTER_MODE = 0,
	SSP_CLOCK_SLAVE_MODE
} SSP_ClockMasterOrSlaveMode;

typedef enum
{
    SSP_FRAME_MASTER_MODE = 0,
	SSP_FRAME_SLAVE_MODE
} SSP_FrameMasterOrSlaveMode;







typedef enum
{
    SSP_NORMAL_MODE 	= 0,
	SSP_NETWORK_MODE    = 1
} SSP_Mode;


typedef enum
{
    SSP_GENERATE_TUR_INT 		= 0,
	SSP_NOT_GENERATE_TUR_INT    = 1
} SSP_TX_UNDER_RUN_INT;


typedef enum
{
    SSP_GENERATE_RIM_INT 		= 0,
	SSP_NOT_GENERATE_RIM_INT    = 1
} SSP_RX_OVER_RUN_INT;


typedef enum
{
    SSP_TRISTATE_ON_SAME_CLK	= 0,
	SSP_TRISTATE_ON_HALF_CLK    = 1
} SSP_TX_TRISTATE;


typedef enum
{
    SSP_DISABLE_TRISTATE	= 0,
	SSP_ENABLE_TRISTATE     = 1
} SSP_TRISTATE;


typedef enum
{
    SSP_ENABLE_COUNT_ERR_INT     = 0,
	SSP_DISABLE_COUNT_ERR_INT    = 1
} SSP_COUNT_ERR_INT;


/* Texas Instruments Synchronous Serial Protocol (SSP): */
/* 4-32 bits in the SSP data frame(sample)*/
typedef struct
{
    SSP_ClockMasterOrSlaveMode    clockMasterOrSlaveMode;
    SSP_FrameMasterOrSlaveMode    frameMasterOrSlaveMode;
} SSP_Texas_Instruments_SSP_Configuration_STR;
/********************************************************/

/* Motorola Serial Peripheral Interface (SPI): */
typedef enum
{
    SSP_OneCycleAtStartOfFrame = 0,
    SSP_HalfCycleAtStartOfFrame
} SSP_SPIClockPhaseSetting;

typedef enum
{
    SSP_InactiveStateIsLow = 0,
    SSP_InactiveStateIsHigh
} SSP_SPIClockPolaritySetting;

typedef struct
{
    SSP_ClockMasterOrSlaveMode  clockMasterOrSlaveMode;
    SSP_FrameMasterOrSlaveMode  frameMasterOrSlaveMode;
    SSP_SPIClockPhaseSetting    SPIClockPhaseSetting;
    SSP_SPIClockPolaritySetting     SPIClockPolaritySetting;
} SSP_Motorola_SPI_Configuration_STR;


/********************************************************/

/*  National Semiconductor Microwire: */
typedef enum
{
    SSP_8_BITS_TRANSMIT =0,
    SSP_16_BITS_TRANSMIT
} SSP_Microwire_Transmitted_WordLen;

typedef struct
{
    SSP_Microwire_Transmitted_WordLen   transmittedNumDataBits; /* 8 or 16 bits in the SSP data frame (sample)*/
} SSP_National_Semiconductor_Microwire_Configuration_STR;


/********************************************************/

/*  Programmable Serial Protocol (PSP): */
typedef enum
{
    SSP_PSP_DataDrivenFallingDataSampledRisingIdleStateLow = 0,
    SSP_PSP_DataDrivenRisingDataSampledFallingIdleStateLow,
    SSP_PSP_DataDrivenRisingDataSampledFallingIdleStateHigh,
    SSP_PSP_DataDrivenFallingDataSampledRisingIdleStateHigh
} SSP_PSP_SerialBitRateClockMode;


typedef enum
{
    SSP_DISABLE_LOOPBACK_MODE 	 = 0,
	SSP_ENABLE_LOOPBACK_MODE     = 1
} SSP_LOOPBACK;


typedef enum
{
    SSP_PSP_PolarityLow = 0,
    SSP_PSP_PolarityHigh
} SSP_PSP_SerialFramePolarity;

typedef enum
{
    SSP_PSP_EndOFTrandferDataLow = 0,
    SSP_PSP_EndOFTrandferDataLastValue
} SSP_PSP_EndOfTransferDataState;

typedef enum
{
    SPP_PSP_DummyStartValueZero = 0,
    SPP_PSP_DummyStartValueOne,
    SPP_PSP_DummyStartValueTwo,
     SPP_PSP_DummyStartValueThree
} SSP_PSP_DummyStart;

typedef enum
{
    SPP_PSP_DummyStopCyclesValueZero = 0,
    SPP_PSP_DummyStopCyclesValueOne,
    SPP_PSP_DummyStopCyclesValueTwo,
    SPP_PSP_DummyStopCyclesValueThree
} SSP_PSP_DummyStopCycles;


typedef enum
{
    SPP_PSP_FrameSyncAfterEndT4 = 0,
    SPP_PSP_FrameSyncWithLsbPreFrame
} SSP_PSP_FrameSync;


typedef enum
{
    SPP_CLK_CONFIG_BY_USER      = 0x1,
	SSP_CONTINUOUS_DMA_MODE      = 0x2,
	SSP_RESERVED_OPTION1		= 0x4,
	SSP_RESERVED_OPTION2		= 0x8,
	SSP_RESERVED_OPTION3		= 0x10
} SSP_options;


typedef struct
{
    SSP_ClockMasterOrSlaveMode      clockMasterOrSlaveMode;
    SSP_FrameMasterOrSlaveMode      frameMasterOrSlaveMode;
    SSP_PSP_SerialBitRateClockMode  serialBitRateClockMode;
    SSP_PSP_SerialFramePolarity     serialFramePolarity;
    SSP_PSP_EndOfTransferDataState  endOfTransferDataState;
    UINT8                           startDelay;
    SSP_PSP_DummyStart              dummyStart;
    UINT8                           serialFrameDelay;
    UINT8                           serialFrameWidth;
    SSP_PSP_DummyStopCycles         dummyStopCycles;
    SSP_PSP_FrameSync               FrameSync;
} SSP_PSP_Configuration_STR;


typedef struct
{
    SSP_Mode    mode;                         // bit [31] in SSCR0
    UINT32      TxTimeSlot;                   // value to be written to SSTSA register
    UINT32      RxTimeSlot;                   // value to be written to SSRSA register
    UINT8       frameRateDivCtrl;             // bits [24:26] in SSCR0
} SSP_networkMode;


typedef struct
{
	UINT32 *    txNonCachedBuffer;
	UINT32	    txNumberOfElements;
} SSP_DMA_ContinuousMode_Configuration_ts;
/****************************************************************************************************************/

typedef struct
{
	UINT8				numDataBits;
	SSP_ClockSource		clockSource;			// On chip, External or Network
	UINT32				baudRate;				// SCR-Serial Clock Rate
	SSP_DMAMode			DMAMode;				// DMA Enable/Disable
	UINT8				transmitThreshold;		// 0-15
	UINT8				receiveThreshold;		// 0-15
	UINT32				numberOfBitsBeforeTimeOut;
	SSP_OprationMode	oprationMode;
	SSP_FrameFormat		formatSelect;
    SSP_networkMode     networkMode;
    UINT32              ssp_option;             // Make or operetion with the enum -- SSP_options.
    BOOL                extendedSizeEDSS;   /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Delete this line, also from file SSp_interface.c ~~~~~~~~~~~~~*/
    union
    {
        SSP_Texas_Instruments_SSP_Configuration_STR                 SSP_Texas_Instruments_SSP_Configuration;
        SSP_Motorola_SPI_Configuration_STR                          SSP_Motorola_SPI_Configuration;
        SSP_National_Semiconductor_Microwire_Configuration_STR      SSP_National_Semiconductor_Microwire_Configuration;
        SSP_PSP_Configuration_STR                                   SSP_PSP_Configuration;
    } SSP_FrameFormatUnion;
	SSP_DMA_ContinuousMode_Configuration_ts configContinuousDma; //used in case of ssp_option includes SSP_CONTINUOUS_DMA_MODE
} SSP_Configuration;


/*****************************************************************************************************************/

/*
  Return Codes:
  -------------
*/

typedef enum
{
    SSP_RC_OK,
    SSP_RC_ALREADY_MAX_REGISTERED,
    SSP_RC_BAD_CLOCK_MASTER_OR_SLAVE_MODE_TYPE,
    SSP_RC_BAD_CLOCK_SOURCE_TYPE,
    SSP_RC_BAD_FRAME_FORMAT_TYPE,
    SSP_RC_BAD_FRAME_MASTER_OR_SLAVE_MODE_TYPE,
    SSP_RC_BAD_PORT_NUM,
    SSP_RC_BAD_OPERATION_MODE_TYPE,
    SSP_RC_BAD_RX_MODE_FORMAT_TYPE,
    SSP_RC_BAD_SEND_BUFFER_MODE_TYPE,
    SSP_RC_BAD_UNIQUE_FORMAT_PARAMETER_VALUE_OR_TYPE,
    SSP_RC_CLIENT_NOT_REGISTERED_FOR_WRITE_CONFIRM_CALLBACK,
    SSP_RC_DATA_LENGTH_VALUE_TOO_BIG,
    SSP_RC_ERROR_IN_CONTROL_PARAMETERS,
    SSP_RC_FRAME_FORMAT_NOT_IMPLEMENTED,
    SSP_RC_ILLEGAL_BAUD_RATE_VALUE,
    SSP_RC_ILLEGAL_CLOCK_RATE_VALUE,
    SSP_RC_ILLEGAL_NUMBER_OF_DATA_BIT_PER_SAMPLE_VALUE,
    SSP_RC_ILLEGAL_RECEIVE_THRESHOLD_VALUE,
    SSP_RC_ILLEGAL_TIME_OUT_VALUE,
    SSP_RC_ILLEGAL_TRANSMIT_THRESHOLD_VALUE,
    SSP_RC_ILLEGAL_BUFFERS_NUMBER_VALUE,
    SSP_RC_RX_ONLY_MODE_NOT_SELECTED,
	SSP_RC_TX_MODE_NOT_SELECTED,
    SSP_RC_INVALID_REGISTER_HANDLE,
    SSP_RC_NO_DATA_TO_WRITE,
    SSP_RC_NULL_POINTER,
    SSP_RC_MODE_ONLY_AVAILABLE_IN_WHITESAIL,
    SSP_RC_POINTER_NOT_ALLIGNED_32_BIT,
    SSP_RC_POINTER_NOT_ALLIGNED_16_BIT,
    SSP_RC_POINTER_TO_BUFFER_NOT_RECOGNIZED,
    SSP_RC_PORT_BUSY_SENDING_PREVIOUS_DATA,
    SSP_RC_PORT_CLOSED,
    SSP_RC_PORT_OPENED,
    SSP_RC_PORT_NOT_AVAILABLE,
    SSP_RC_PORT_NOT_CONFIGURED,
    SSP_RC_PORT_NOT_OPEN,
    SSP_RC_TIMER_EXPIRED_SEND_PROCEDURE_FAILED,
	SSP_DMA_REQUIRED,
    SSP_RC_ILLEGAL_BAUDRATE,
	SSP_RC_MORE_THEN_3_BUFFERS_NEEDED
} SSP_ReturnCode;


//#define SSP_DEBUG_ACAT
//#define SSP_DEBUG_UART

#define	SSP_DEBUG_LEVEL	0x1

#ifdef SSP_DEBUG_ACAT
        #define SSP_TRACE(cat1,cat2,cat3,level,fmt) \
                  if ((level) & sspTraceFilter()) { DIAG_FILTER(cat1, cat2, cat3, level) \
                  diagTextPrintf(fmt);}

        #define SSP_TRACE1(cat1, cat2, cat3, level, fmt, val1) \
                  if ((level) & sspTraceFilter()){ DIAG_FILTER(cat1, cat2, cat3, level) \
                  diagPrintf(fmt, val1);}

        #define SSP_TRACE2(cat1, cat2, cat3, level, fmt, val1, val2) \
                  if ((level) & sspTraceFilter()) {DIAG_FILTER(cat1, cat2, cat3, level) \
                  diagPrintf(fmt, val1, val2);}

        #define SSP_TRACE3(cat1, cat2, cat3, level, fmt, val1, val2, val3) \
                  if ((level) & sspTraceFilter()) {DIAG_FILTER(cat1, cat2, cat3, level) \
                  diagPrintf(fmt, val1, val2, val3);}

        #define SSP_TRACEBUF(cat1, cat2, cat3, level, fmt, buffer, length) \
                  if ((level) & sspTraceFilter()) {DIAG_FILTER(cat1, cat2, cat3, level) \
                  diagStructPrintf(fmt, buffer, (length > 20) ? 20 : length);}

#elif defined	SSP_DEBUG_UART
		#define SSP_TRACE(cat1,cat2,cat3,level,fmt) \
				 if ((level) & sspTraceFilter()) { uart_printf(fmt);}

	   	#define SSP_TRACE1(cat1, cat2, cat3, level, fmt, val1) \
				 if ((level) & sspTraceFilter()) { uart_printf(fmt, val1);}

	   	#define SSP_TRACE2(cat1, cat2, cat3, level, fmt, val1, val2) \
				 if ((level) & sspTraceFilter()) { uart_printf(fmt, val1, val2);}

	   	#define SSP_TRACE3(cat1, cat2, cat3, level, fmt, val1, val2, val3) \
				 if ((level) & sspTraceFilter()) { uart_printf(fmt, val1, val2, val3);}

	   	#define SSP_TRACEBUF(cat1, cat2, cat3, level, fmt, buffer, length)
						  
#else

        #define SSP_TRACE(cat1, cat2, cat3, level, fmt) 

        #define SSP_TRACE1(cat1, cat2, cat3, level, fmt, val1) 

        #define SSP_TRACE2(cat1, cat2, cat3, level, fmt, val1, val2) 

        #define SSP_TRACE3(cat1, cat2, cat3, level, fmt, val1, val2, val3)

        #define SSP_TRACEBUF(cat1, cat2, cat3, level, fmt, buffer, length) 
	
#endif














/*----------- Extern definition ----------------------------------------------*/

/*----------- Global variable declarations -----------------------------------*/

/*----------- Global constant definitions ------------------------------------*/

/*----------- Global function prototypes -------------------------------------*/
/* Specific API */
UINT32 MuxSpiSlaveSend(UINT32* Txbuffer,UINT32 TxLen);
SSP_ReturnCode SSPElementsSend(SSP_Port portNumber,void *pRxBuffer, void *pTxBuffer, UINT32 numberOfElements);

SSP_ReturnCode SSPElementsReceive 		( 	SSP_Port 	portNumber,
											void 		*pRxPoolBase,			// buffer pool base address
											UINT32 		poolNumberOfBuffers,	// number of buffers in pool,
                                    		UINT32      bufferLenInElements  	// number of elements in buffer,
 										);
void SSPReceiveBufferReadCompleted(SSP_Port portNumber);
void SSPReceiveStop(SSP_Port portNumber);
void  sspLoopBackMode( SSP_Port		portNumber , SSP_LOOPBACK    loopback );
SSP_ReturnCode SSPStop ( SSP_Port   portNumber );


/* Common API */
SSP_ReturnCode SSPClose(SSP_Port portNumber);
SSP_ReturnCode SSPOpen(SSP_Port portNumber);
SSP_ReturnCode SSPUnRegister(SSP_Port portNumber);
SSP_ReturnCode SSPRegister(SSP_Port portNumber,
                           sspNotifyElementsReceived	callbackNotifyElementsReceived,
						   sspNotifyElementsSent		callbackNotifyElementsSent,
						   sspNotifyReceiveOverflow		callbackNotifyReceiveOverflow);
SSP_ReturnCode SSPConfigure(SSP_Port portNumber,SSP_Configuration *configuration);
SSP_ReturnCode SSPConfigurationGet(SSP_Port portNumber, SSP_Configuration *configuration);
SSP_ReturnCode SSPPhase1Init(void);
SSP_ReturnCode SSPPhase2Init(void);
//SwVersion      SSPVersionGet(void);
SSP_ReturnCode SSPPackageSizeSendNotify ( SSP_Port   portNumber , UINT16    package_size , sspNotifyElementsSent    PackageSendNotify );
void SspTestTaskCreate(void);


#endif /* _SSP_H_ */

