/*--------------------------------------------------------------------------------------------------------------------
(C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
-------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
INTEL CONFIDENTIAL
Copyright 2006 Intel Corporation All Rights Reserved.
The source code contained or described herein and all documents related to the source code (“Material? are owned
by Intel Corporation or its suppliers or licensors. Title to the Material remains with Intel Corporation or
its suppliers and licensors. The Material contains trade secrets and proprietary and confidential information of
Intel or its suppliers and licensors. The Material is protected by worldwide copyright and trade secret laws and
treaty provisions. No part of the Material may be used, copied, reproduced, modified, published, uploaded, posted,
transmitted, distributed, or disclosed in any way without Intel’s prior express written permission.

No license under any patent, copyright, trade secret or other intellectual property right is granted to or
conferred upon you by disclosure or delivery of the Materials, either expressly, by implication, inducement,
estoppel or otherwise. Any license under such intellectual property rights must be express and approved by
Intel in writing.
-------------------------------------------------------------------------------------------------------------------*/

/*********************************************************************
*                      M O D U L E     B O D Y                       *
**********************************************************************
* Title: I2C package                                                 *
*                                                                    *
* Filename: I2C.c                                                    *
*                                                                    *
* Target, platform: Common Platform, HW platform                     *
*                                                                    *
* Authors: Adi Dayan                                                 *
*                                                                    *
* Description:                                                       *
*                                                                    *
* Notes:                                                             *
**********************************************************************/

/************* External include definitions ***************************/

/************************** MACROS ***************************************/
#include "pmic.h"
#include "ningbo.h"
#include "guilin.h"
#include "guilin_lite.h"

unsigned char pmic_id=PMIC_UNAVAILABLE;

/************* External include definitions ***************************/
volatile unsigned long I2C_REGISTER_BASE_ADDR = 0xD4037000;
#define PMU_XCGR_READ	 			(( volatile UINT32 *)(0xD4051024))
#define I2C_FUN_CLK_ADD       		(( volatile UINT32 *)(0xD4051024))
#define I2C_APB_CLK_ADD				(( volatile UINT32 *)(0xD401502c))
#define PMU_XCGR_EN_I2C_FUN_CLK		((0x1 << 22) |(0x1 << 6))
#define I2C_APB_CLK_ENABLE			0x3
#define I2C_APB_RESET   			0x4

#define I2C_DEVICE_ERROR                        0xDEADBEEF
#define I2C_STATUS_ARRAY_SIZE                   4
#define I2C_RECORD_FLAG                         0x494E4954

/*********************** Sub-Package Parameters *************************/
/* I2C device status*/
typedef struct I2CDeviceStatus
{
    UINT32 RecordFlag;
    UINT32 BaseAddress;
    UINT32 SlaveAddress;
    UINT32 DeviceStatus;
} I2CDeviceStatus;

I2CDeviceStatus I2CStatusArray[I2C_STATUS_ARRAY_SIZE];

typedef void (*I2CMasterNotifyDataReceived)( UINT8 * , UINT16 , UINT16 );

typedef struct
{
    UINT8 							activeSlaveAddress;
    UINT8 							*RxBufferPtr;
    UINT16 							dataSize;
    UINT16 							ID;
	I2CMasterNotifyDataReceived 	callBack;
} I2C_ReceiveRequestParams;

/**********************************************************************************/
static BOOL    _repeat_start = TRUE;

static I2C_ReceiveRequestParams    _receiveReqParams;

/******  These structures are use to send/received data by valye only    *********/

void ustica_I2CInit(void)
{
	*((volatile UINT32 *)0xd401E140) = 0xd0c2|1<<3|1<<9; //PMR_SCL ->CP_TWSI_SCL
	*((volatile UINT32 *)0xd401E144) = 0xd0c2|1<<3|1<<9; //PWR_SDA ->CP_TWSI_SDA
}
void ustica_I2CEnableclockandPin(void)
{
    *(volatile unsigned long*)0xD4090104 |= 0x2; //enable PLL1_614 for PWIC I2C
	return;
}

static void ustica_I2CResetUnitDi ( void )
{
    UINT32  	 ICRRegValue;

	// Read the control register
	I2C_REG_READ(I2C_ICR_REG,ICRRegValue);

	ICRRegValue &= ~I2C_ICR_MASTER_ABORT;        // Clear the abort bit
  	ICRRegValue &= ~I2C_ICR_START;               // Clear the start bit
  	ICRRegValue &= ~I2C_ICR_STOP;                // Clear the stop bit
	ICRRegValue &= ~I2C_ICR_ACK_NACK_CONTROL;    // Clear the ACK bit
  	ICRRegValue &= ~I2C_ICR_TRANSFER_BYTE;       // Clear the transfer_byte bit


    // clear the ICR register
    I2C_REG_WRITE(I2C_ICR_REG, I2C_REG_CLEAR_VALUE);

    // Set the RESET bit in the ICR rewgister
    I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_UNIT_RESET_BIT, SET);

    // clear the ISR register after reset
    I2C_REG_WRITE(I2C_ISR_REG, I2C_ISR_CLEAR_ALL_INTERRUPTS);

    // Clear the RESET bit in the ICR rewgister
    I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_UNIT_RESET_BIT, CLEAR);

    // Restore the ICR register
    I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue);

}


void ustica_I2CConfigureDi( void )
{
    	UINT32 			controlRegValue = I2C_ICR_REG_INIT_VALUE;



		// reset the I2C unit
    	ustica_I2CResetUnitDi();

    	controlRegValue |= FAST_MODE_ENABLE;

    	I2C_REG_WRITE(I2C_ICR_REG, controlRegValue);

        I2C_REG_WRITE(I2C_ISAR_REG, I2C_OWN_SLAVE_ADDRESS);


}


/************************************************************************
* Function: i2cWaitStatusDirect
*************************************************************************
* Description:  - waits for specific status register value
*               - time is limited
*               - breaks when _error_was_detected indicates error detected by interrupt
*               - breaks when _arbitration_loss_was_detected indicates error detected by interrupt
*
* Parameters:   bitsSet     contains '1' where 1 is expected
*               bitsCleared contains '1' where 0 is expected
* Return value: I2C_RC_OK if got it or error code otherwise
*
* Notes:
************************************************************************/
static I2C_ReturnCode ustica_i2cWaitStatusDirect ( UINT32    bitsSet , UINT32    bitsCleared )
{


	UINT32 		statusRegValue;
    UINT32 		countLimit = LOOP_COUNTER_LIMIT;
	UINT32 		mask = bitsSet|bitsCleared;    // all bits we care of
	UINT32 		value = bitsSet;

	do
    {
        I2C_REG_READ(I2C_ISR_REG,statusRegValue);
        countLimit--;
    }
    while ( (( statusRegValue & mask ) != value ) &&
	        (!( I2C_REG_BIT_READ(statusRegValue, I2C_ISR_BUS_ERROR_DETECTED_BIT) ) &&
			(countLimit > 0) &&
			(!( I2C_REG_BIT_READ(statusRegValue, I2C_ISR_ARBITRATION_LOSS_DETECTED_BIT) )) ));

    if ( I2C_REG_BIT_READ(statusRegValue, I2C_ISR_ARBITRATION_LOSS_DETECTED_BIT) )
		return I2C_ISR_ARBITRATION_LOSS;

    // check for timeout, reset the I2C device for next operations and return error if needed
    if ( countLimit == 0 )
    {
		UINT32  ICRRegValue = 0;

		I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
		ICRRegValue |= I2C_ICR_MASTER_ABORT;
		ICRRegValue &= ~I2C_ICR_TRANSFER_BYTE;
    	I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );
    	return I2C_RC_TIMEOUT_ERROR;
	}
    else
	   if ( ( I2C_REG_BIT_READ(statusRegValue, I2C_ISR_BUS_ERROR_DETECTED_BIT) ))
	        return I2C_ISR_BUS_ERROR;

	return I2C_RC_OK;
}

/************************************************************************
* Function: masterSendDirect
*************************************************************************
* Description: This function is used to write data to I2C slaves in the
* 			   boot stage only while the OS system and INTC are not up.
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
static I2C_ReturnCode ustica_masterSendDirect(UINT8 *data, UINT16 dataSize, UINT8 slaveAddress, BOOL repeatedStart , BOOL   MasterReceiveCalled )
{
    UINT32  		ICRRegValue = 0;
    int 			i;
	I2C_ReturnCode 	I2CReturnCode = I2C_RC_OK;



	/* Check the bus for free - If there is an arbitration loss, stay until it become free */
    do
	{
        // check if 'Arbitration Loss' was detected
       	if ( I2CReturnCode == I2C_ISR_ARBITRATION_LOSS )
		{
             I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_ARBITRATION_LOSS_DETECTED_BIT);


			 // Read the control register
			 I2C_REG_READ(I2C_ICR_REG,ICRRegValue);

		  	 ICRRegValue &= ~I2C_ICR_START;               // Clear the start bit
		  	 ICRRegValue &= ~I2C_ICR_TRANSFER_BYTE;       // Clear the transfer_byte bit

    		 // Write to the ICR register
    		 I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue);

			 for ( i = 0; i < 50000; i++ );           	/* A little delay to enable others master to finish without interruption */
														/* the bus arbiter, and reduce the arbitration sequences */
		}

    	// write IDBR register: target slave address and R/W# bit=0 for write transaction.
    	I2C_REG_WRITE(I2C_IDBR_REG, I2C_SLAVE_WRITE(slaveAddress) );

    	// write ICR register: set START bit, clear STOP bit, set Transfer Byte bit to initiate the access
        I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
		ICRRegValue &= ~I2C_ICR_STOP;
		ICRRegValue |= (I2C_ICR_START | I2C_ICR_TRANSFER_BYTE);
    	I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

	    I2CReturnCode = ustica_i2cWaitStatusDirect ( (I2C_REG_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT) | I2C_REG_BIT(I2C_ISR_UNIT_BUSY_BIT)),
			    	                     I2C_REG_BIT(I2C_ISR_READ_WRITE_MODE_BIT)  );

    	// clear 'IDBR Transmit Empty' bit (write '1')
    	I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT);

		if ( ( I2CReturnCode != I2C_ISR_ARBITRATION_LOSS ) && ( I2CReturnCode != I2C_RC_OK ))
		{
        	I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
			ICRRegValue &= 0xfff0;
   			I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

			return I2CReturnCode;
		}
    }
    // check if 'Arbitration Loss' was detected
	while ( I2CReturnCode == I2C_ISR_ARBITRATION_LOSS );

    // write ICR register: clear START bit, clear STOP bit
    I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
  	ICRRegValue &= ~I2C_ICR_STOP;
  	ICRRegValue &= ~I2C_ICR_START;
   	I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

    /*** Send data bytes - all, except the last byte  ***/
    for ( i = 0; i < ( dataSize - 1 ); i++ )
    {
        // write data byte to the IDBR register
        I2C_REG_WRITE(I2C_IDBR_REG, *data);
        data++;

        // Set 'Tranfer Byte' bit to intiate the access
        I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_TRANSFER_BYTE_BIT, SET);

	    if((I2CReturnCode = ustica_i2cWaitStatusDirect(I2C_REG_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT)|I2C_REG_BIT(I2C_ISR_UNIT_BUSY_BIT),
				                          I2C_REG_BIT(I2C_ISR_READ_WRITE_MODE_BIT))) != I2C_RC_OK)
						return I2CReturnCode;

        // clear 'IDBR Transmit Empty' bit (write '1')
        I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT);
    }


    /*** Send the last byte with STOP bit ***/

    // write the last data byte to the IDBR register
    I2C_REG_WRITE(I2C_IDBR_REG, *data);

    // write ICR: clear START bit
    I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_START_BIT, CLEAR);

    if (( repeatedStart == FALSE ) || ( MasterReceiveCalled == FALSE ))
        I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_STOP_BIT, SET);

    I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_TRANSFER_BYTE_BIT, SET);

//	if (repeatedStart)
//	{
//	  if ((I2CReturnCode = i2cWaitStatusDirect (I2C_REG_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT)|I2C_REG_BIT(I2C_ISR_UNIT_BUSY_BIT),
//	  	    	                          I2C_REG_BIT(I2C_ISR_READ_WRITE_MODE_BIT)))!=I2C_RC_OK)
//				return I2CReturnCode;
//	}
//    else
	  if ((I2CReturnCode = ustica_i2cWaitStatusDirect (I2C_REG_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT),
	                                      I2C_REG_BIT(I2C_ISR_READ_WRITE_MODE_BIT)))!=I2C_RC_OK)
				return I2CReturnCode;

	// clear 'IDBR Transmit Empty' bit (write '1')
    I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT);
    I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_STOP_BIT, CLEAR);

    return I2C_RC_OK;
}


/************************************************************************
* Function: I2CBusStateGetDirect
*************************************************************************
* Description: This function is used to read the I2C Bus Monitor
* 			   Register - IBMR.
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/

/************************************************************************
* Function: I2CMasterSendDataDirect
*************************************************************************
* Description: This function is used to write data to I2C slaves in the
* 			   boot stage only while the OS system and INTC are not up.
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
I2C_ReturnCode ustica_I2CMasterSendDataDirect( UINT8 *data , UINT16 dataSize , UINT8 slaveAddress , BOOL protected , UINT16 userId )
{
    //UINT32 			XCGR_val;
    I2C_ReturnCode 	I2CReturnCode;
    BOOL 			chipBusy;

#if 0
    return I2C_RC_OK; //qianying, for FPGA ONLY
     
#ifndef EDEN_1928
	XCGR_val = *PMU_XCGR_READ;
	*I2C_FUN_CLK_ADD = XCGR_val | PMU_XCGR_EN_I2C_FUN_CLK;              // 
	 if(AC_IS_CPONLY)
	 {
	 	*I2C_APB_CLK_ADD = I2C_APB_CLK_ENABLE;       //1920                       // I2c APB Clock ;I2c Functional Clock
	 }
#endif
#endif
    CHECK_IF_CHIP_BUSY(chipBusy);
    if ( chipBusy == TRUE )
        return I2C_RC_CHIP_BUSY;

    I2CReturnCode = ustica_masterSendDirect(data, dataSize, slaveAddress, _repeat_start , FALSE);
#if 0	
#ifndef EDEN_1928 
	if(!AC_IS_SHM)
    {
	    //clock off
		XCGR_val = *PMU_XCGR_READ;
		*I2C_FUN_CLK_ADD = XCGR_val &(~PMU_XCGR_EN_I2C_FUN_CLK);              // 
		if(AC_IS_CPONLY)
		{
			XCGR_val = *I2C_APB_CLK_ADD;//1920
			*I2C_APB_CLK_ADD = XCGR_val & (~I2C_APB_CLK_ENABLE);                              // I2c APB Clock ;I2c Functional Clock
		}
	}
#endif
#endif
    return I2CReturnCode;
}
/************************************************************************/
/************************************************************************/
static UINT16 ustica_I2CIntLISRDirect ( void )
{
    UINT32 			statusRegValue , ICRRegValue = 0;



    //check interrupt source
    I2C_REG_READ(I2C_ISR_REG,statusRegValue);

    if ( I2C_REG_BIT_READ(statusRegValue, I2C_ISR_IDBR_RECEIVE_FULL_BIT) )
    {
        // clear 'IDBR Receive Full' bit (write '1')
        I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_IDBR_RECEIVE_FULL_BIT);
        {
       
            // read the received byte
            I2C_REG_READ(I2C_IDBR_REG,*_receiveReqParams.RxBufferPtr);
            _receiveReqParams.RxBufferPtr++;
            _receiveReqParams.dataSize--;

            // write ICR: clear STOP bit(1), clear ACK/NACK bit(2)
    		I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
			ICRRegValue &= ~I2C_ICR_STOP;
			ICRRegValue &= ~I2C_ICR_ACK_NACK_CONTROL;
    		I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );


            if ( _receiveReqParams.dataSize > 1 )
            {
            	// write ICR: clear START bit, clear STOP bit, send ACK bit (0 for ACK),
				//             set Transfer Byte bit to initiate the access
    			I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
				ICRRegValue &= ~I2C_ICR_START;
				ICRRegValue &= ~I2C_ICR_STOP;
				ICRRegValue &= ~I2C_ICR_ACK_NACK_CONTROL;
				ICRRegValue |= I2C_ICR_TRANSFER_BYTE;
    			I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );
            }
            else if ( _receiveReqParams.dataSize == 1 ) //last byte remaing to read (with STOP signal)
            {
            	// write ICR: clear START bit, set STOP bit, send NACK bit (1 for NACK),
				//            set Transfer Byte bit to initiate the access
    			I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
				ICRRegValue &= ~I2C_ICR_START;
				ICRRegValue |= (I2C_ICR_STOP | I2C_ICR_ACK_NACK_CONTROL | I2C_ICR_TRANSFER_BYTE);
    			I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );
            }
            else
            {
        		// disable 'IDBR Buffer Full' interrupt
        		I2C_REG_BIT_WRITE(I2C_ICR_REG, I2C_ICR_IDBR_RECEIVE_FULL_INT_ENABLE_BIT, CLEAR);

            }
        }
    }

    if ( I2C_REG_BIT_READ(statusRegValue, I2C_ISR_BUS_ERROR_DETECTED_BIT) )
    {
        // clear 'Bus Error Detected' bit (write '1')
        I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_BUS_ERROR_DETECTED_BIT);

    }

    if ( statusRegValue & I2C_ISR_UNEXPECTED_INTERRUPTS )
    {
        // clear 'I2C_ISR_UNEXPECTED_INTERRUPTS ' bits (write '1')
        I2C_REG_WRITE(I2C_ISR_REG,I2C_ISR_UNEXPECTED_INTERRUPTS);

    }

	return _receiveReqParams.dataSize;

}
/************************************************************************
* Function: I2CMasterReceiveDataDirect
*************************************************************************
* Description: This function is used to read data from I2C slaves in the
* 			   boot stage only while the OS system and INTC are not up.
*
* Parameters:
*
* Return value:
*
* Notes:
************************************************************************/
I2C_ReturnCode ustica_I2CMasterReceiveDataDirect ( UINT8 						   *cmd,
											UINT16 	 						cmdLength,
											UINT8 							writeSlaveAddress,
											BOOL 		 					protected,
		                                    UINT8 						   *designatedRxBufferPtr,
											UINT16 	 						dataSize,
											UINT8 							readSlaveAddress )
{
    //UINT32 			XCGR_val;
    I2C_ReturnCode 	I2CReturnCode;
    UINT32 			ICRRegValue = 0;
    BOOL 			chipBusy;
	//int             result;
    // store the request parameters (to be used from the ISR and by the I2C task when error)
    _receiveReqParams.RxBufferPtr = designatedRxBufferPtr;
    _receiveReqParams.dataSize = dataSize;

#if 0
    return I2C_RC_OK; //qianying, for FPGA ONLY

    if ( dataSize == 0 )
        return I2C_RC_INVALID_DATA_SIZE;
#ifndef EDEN_1928

	XCGR_val = *PMU_XCGR_READ;
	*I2C_FUN_CLK_ADD = XCGR_val | PMU_XCGR_EN_I2C_FUN_CLK;              // I2c Functional Clock
	 if(AC_IS_CPONLY)
	{
		*I2C_APB_CLK_ADD = I2C_APB_CLK_ENABLE;                       //1920       // I2c APB Clock
	}
#endif
#endif
    CHECK_IF_CHIP_BUSY(chipBusy);
    if(chipBusy)
        return I2C_RC_CHIP_BUSY;

    if ( (cmdLength != 0) && (cmd != NULL) )  /* if command is associated with the received request */
    {
        I2CReturnCode = ustica_masterSendDirect(cmd, cmdLength, writeSlaveAddress, _repeat_start, TRUE);

        if( I2CReturnCode != I2C_RC_OK )
            return I2CReturnCode;
    }

    /*** send Read Request ***/
    // write IDBR: target slave address and R/W# bit (1 for read)
    I2C_REG_WRITE(I2C_IDBR_REG, I2C_SLAVE_READ(readSlaveAddress) );

    // write ICR: set START bit, clear STOP bit, set Transfer Byte bit to initiate the access
    I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
	ICRRegValue &= ~I2C_ICR_STOP;
	ICRRegValue |= (I2C_ICR_START | I2C_ICR_TRANSFER_BYTE);
    I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

	I2CReturnCode = ustica_i2cWaitStatusDirect(I2C_REG_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT)|I2C_REG_BIT(I2C_ISR_UNIT_BUSY_BIT)|I2C_REG_BIT(I2C_ISR_READ_WRITE_MODE_BIT), 0 );

    // clear 'IDBR Transmit Empty' bit (write '1')
    I2C_STATUS_REG_CLEAR_BIT(I2C_ISR_IDBR_TRANSMIT_EMPTY_BIT);

	if ( I2CReturnCode != I2C_RC_OK )
        return I2CReturnCode;


    // Initiate the read process:

    if ( dataSize == 1 )   // only one byte to read
    {
        // write ICR: set STOP bit, set ACK/NACK bit (1 for NACK)
        I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
  		ICRRegValue &= ~I2C_ICR_START;
  	    ICRRegValue |= (I2C_ICR_STOP | I2C_ICR_ACK_NACK_CONTROL);
   	    I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );
    }
    else   // more than one byte to read
    {
        // write ICR: clear STOP bit, clear ACK/NACK bit (0 for ACK)
    	I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
		ICRRegValue &= ~I2C_ICR_STOP;
		ICRRegValue &= ~I2C_ICR_ACK_NACK_CONTROL;
    	I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );
    }

    I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
  	ICRRegValue &= ~I2C_ICR_START;
  	ICRRegValue |= I2C_ICR_TRANSFER_BYTE;
   	I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

	while ( ustica_I2CIntLISRDirect() );

    return I2C_RC_OK;
}

/*************************************************************************
* Function: I2CRecordDeviceStatus
*
* Descrioption
*   The function record I2C device status.
*
* Parameters:
*   Inputs  :
*       error: The I2C device status.
*       slaveAddress: The slave address of device.
*   Outputs:
*       N/A.
*
* Returns:
*       N/A.
*
* Notes:
*   N/A.
**************************************************************************/
BOOL I2CRecordDeviceStatus(UINT32 SlaveAddress, BOOL error)
{
    UINT8 i = 0;
//    UINT32 cpsr = 0;
    BOOL status = FALSE;

    /*---------------------------------------------------------------------*/
    for ( i = 0; i < I2C_STATUS_ARRAY_SIZE; i++ )
    {
//        cpsr = disableInterrupts();

        if ( I2C_RECORD_FLAG != I2CStatusArray[i].RecordFlag )
        {
            I2CStatusArray[i].RecordFlag = I2C_RECORD_FLAG;
            I2CStatusArray[i].BaseAddress = I2C_REGISTER_BASE_ADDR;
            I2CStatusArray[i].SlaveAddress = SlaveAddress;

            if(error)
            {
                I2CStatusArray[i].DeviceStatus = I2C_DEVICE_ERROR;
            }
            else
            {
                I2CStatusArray[i].DeviceStatus = 0;
            }

            status = TRUE;
        }

//        restoreInterrupts(cpsr);
    }

    return status;
}

/*************************************************************************
* Function: I2CDeviceCheckStatus
*
* Descrioption
*   The function check the I2C device status.
*
* Parameters:
*   Inputs  :
*       slaveAddress: The slave address of decice.
*   Outputs:
*       N/A.
*
* Returns:
*       N/A.
*
* Notes:
*   N/A.
**************************************************************************/
BOOL I2CDeviceCheckStatus(UINT32 SlaveAddress)
{
    UINT8 i = 0;
//    UINT32 cpsr = 0;
    BOOL status = TRUE;

    /*---------------------------------------------------------------------*/

    for ( i = 0; i < I2C_STATUS_ARRAY_SIZE; i++ )
    {
//        cpsr = disableInterrupts();

        if (( I2CStatusArray[i].BaseAddress == I2C_REGISTER_BASE_ADDR )&&
            ( I2CStatusArray[i].SlaveAddress == SlaveAddress )&&
            ( I2CStatusArray[i].RecordFlag == I2C_RECORD_FLAG )&&
            ( I2CStatusArray[i].DeviceStatus == I2C_DEVICE_ERROR))
        {
            status = FALSE;
        }

//        restoreInterrupts(cpsr);
    }

    return status;
} /* End at_client_get_callback_function() */

/*************************************************************************
* Function: MastI2CSend
*
* Descrioption
*   The i2c reading with not re-entery.
*
* Parameters:
*   Inputs  :
*       I2CAddr          : The address of device register.
*       slaveAddress     : The slave address of decice.
*   Outputs:
*       N/A.
*
* Returns:
*       RegData          : The data of reading from device register.
*
* Notes:
*   N/A.
**************************************************************************/
static void MastI2CSend(UINT8 Slave_addr, UINT8 RegAddr, UINT8 RegData )
{
	UINT16      read_times = 0;
	UINT32      i = 0; 
	volatile UINT32 ICRRegValue = 0;
	UINT8	    param_data[3] = {0};
	I2C_ReturnCode	status = I2C_RC_NOT_OK;

	param_data[0] = RegAddr;
	param_data[1] = RegData;
	param_data[2] = 0;

	status = ustica_I2CMasterSendDataDirect(param_data , 2 , Slave_addr , FALSE, 0 );

	read_times = 0;
	while ( status != I2C_RC_OK )
	{
		read_times ++;
    	if (read_times >= 3)
    	{
    	    uart_printf("Send: reset I2Cbus\r\n");

            I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
            ICRRegValue |= I2C_ICR_BUS_RESET;
            I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

            do
            {
                I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
            }while (ICRRegValue & I2C_ICR_BUS_RESET);

            uart_printf("Send: reset I2Cbus ends\r\n");
		}
		usticaInit();

		for( i = 0; i < 0x10000; i++ );                                 /* Short delay.             */

        status = ustica_I2CMasterSendDataDirect(param_data , 2 , Slave_addr , FALSE, 0 );

		if( read_times >= 6 )
		{
            uart_printf("\r\nMastI2CSend: 0x%x, Slave_addr = 0x%x, I2CRegAddr = 0x%x, RegData = 0x%x.\r\n",status, Slave_addr, RegAddr, RegData);
			break;
		}
	}

}

/*************************************************************************
* Function: I2CMastReceive
*
* Descrioption
*   The i2c reading with not re-entery.
*
* Parameters:
*   Inputs  :
*       I2CAddr          : The address of device register.
*       slaveAddress     : The slave address of decice.
*   Outputs:
*       N/A.
*
* Returns:
*       RegData          : The data of reading from device register.
*
* Notes:
*   N/A.
**************************************************************************/
static UINT8 MastI2CReceive(UINT8 Slave_addr, UINT8 I2CRegAddr)
{
	UINT32      i = 0;
	volatile UINT32 ICRRegValue = 0;
	UINT16      read_times = 0;
	UINT8       I2CRegData = 0;
	I2C_ReturnCode	status = I2C_RC_NOT_OK;

	Slave_addr &=0xFE;
    status = ustica_I2CMasterReceiveDataDirect(  &I2CRegAddr,
                                    1 /*cmd lenght*/,
                                    Slave_addr,
                                    FALSE /*not protected*/,
                                    &I2CRegData,
                                    0x1,
                                    (Slave_addr|0x1));

	read_times = 0;
	while ( status != I2C_RC_OK )
	{
		read_times ++;
    	if (read_times >= 3)
    	{
    	    uart_printf("Rcv: reset I2Cbus\r\n");

            I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
            ICRRegValue |= I2C_ICR_BUS_RESET;
            I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue );

            do
            {
                I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
            }while (ICRRegValue & I2C_ICR_BUS_RESET);

            uart_printf("Rcv: reset I2Cbus ends\r\n");
    	}
		usticaInit();
		
		for(i=0;i<0x10000;i++);
        status = ustica_I2CMasterReceiveDataDirect(  &I2CRegAddr,
                                        1 /*cmd lenght*/,
                                        Slave_addr,
                                        FALSE /*not protected*/,
                                        &I2CRegData,
                                        0x1,
                                        (Slave_addr|0x1));
		if( read_times >= 6 )
		{
            uart_printf("\r\nMastI2CReceive: 0x%x, Slave_addr = 0x%x, I2CRegAddr = 0x%x.\r\n",status, Slave_addr, I2CRegAddr);

            break;
		}
	}

	return I2CRegData;
}

/*************************************************************************
* Function: I2CSend
*
* Descrioption
*   The i2c writting with re-entery.
*
* Parameters:
*   Inputs  :
*       RegAddr          : The address of device register.
*       RegData          : The data of writing into device register.
*       slaveAddress     : The slave address of decice.
*   Outputs:
*       N/A.
*
* Returns:
*       N/A.
*
* Notes:
*   N/A.
**************************************************************************/
void I2CSend( UINT8 Slave_addr, UINT8 RegAddr, UINT8 RegData )
{
	MastI2CSend(Slave_addr, RegAddr,RegData);
}


/*************************************************************************
* Function: I2CReceive
*
* Descrioption
*   The i2c reading with re-entery.
*
* Parameters:
*   Inputs  :
*       I2CAddr          : The address of device register.
*       slaveAddress     : The slave address of decice.
*   Outputs:
*       N/A.
*
* Returns:
*       RegData          : The data of reading from device register.
*
* Notes:
*   N/A.
**************************************************************************/
UINT8 I2CReceive(  UINT8 Slave_addr, UINT8 I2CRegAddr )
{
	UINT8       res = 0;

	res = MastI2CReceive( Slave_addr, I2CRegAddr );
	return res;
}

void USTICAI2CWriteDi_base(unsigned char I2CRegAddr, unsigned char I2CRegData)
{
    I2CSend(PMC_I2C_WRITE_BASE_SLAVE_ADDRESS, I2CRegAddr, I2CRegData);
}
unsigned char USTICAI2CReadDi_base(unsigned char I2CRegAddr)
{
    return I2CReceive(PMC_I2C_WRITE_BASE_SLAVE_ADDRESS, I2CRegAddr);        
}


void USTICAI2CWriteDi_power(unsigned char I2CRegAddr, unsigned char I2CRegData)
{
    I2CSend(PMC_I2C_WRITE_POWER_SLAVE_ADDRESS, I2CRegAddr, I2CRegData);
}
unsigned char USTICAI2CReadDi_power(unsigned char I2CRegAddr)
{
    return I2CReceive(PMC_I2C_WRITE_POWER_SLAVE_ADDRESS, I2CRegAddr);        
}

//add by zhangwl for write/read GPADC register
void USTICAI2CWriteDi_GPADC(unsigned char I2CRegAddr, unsigned char I2CRegData)
{
    I2CSend(PMC_I2C_WRITE_GPADC_SLAVE_ADDRESS, I2CRegAddr, I2CRegData);
}
unsigned char USTICAI2CReadDi_GPADC(unsigned char I2CRegAddr)
{
    return I2CReceive(PMC_I2C_WRITE_GPADC_SLAVE_ADDRESS, I2CRegAddr);        
}

void USTICAI2CWriteDi_TEST(unsigned char I2CRegAddr, unsigned char I2CRegData)
{
    I2CSend(PMC_I2C_WRITE_TEST_SLAVE_ADDRESS, I2CRegAddr, I2CRegData);
}
unsigned char USTICAI2CReadDi_TEST(unsigned char I2CRegAddr)
{
    return I2CReceive(PMC_I2C_READ_TEST_SLAVE_ADDRESS, I2CRegAddr);        
}

void USTICAI2CWriteDi_AUDIO(unsigned char I2CRegAddr, unsigned char I2CRegData)
{
    I2CSend(PMC_I2C_WRITE_AUDIO_SLAVE_ADDRESS, I2CRegAddr, I2CRegData);
}
unsigned char USTICAI2CReadDi_AUDIO(unsigned char I2CRegAddr)
{
    return I2CReceive(PMC_I2C_WRITE_AUDIO_SLAVE_ADDRESS, I2CRegAddr);        
}

//ICAT EXPORTED FUNCTION - ADEMO,START_PS,usticaInit
void usticaInit(void)
{
	ustica_I2CEnableclockandPin();
	ustica_I2CConfigureDi();
}

void PM812_Ldo_8_set_2_8(void)
{
        PM812_LDO_VOUT_CFG(PM812_LDO8_VOUT_REG, PM812_LDO3_TO_LDO17_2V80_ACT,PM812_LDO3_TO_LDO17_2V80_SLP);
}

void PM812_Ldo_8_set(BOOL OnOff)
{
        PM812_LDO_Enable(8,OnOff);
}

unsigned char GetPMICID(void)
{
	unsigned char regval;
	
    regval = USTICAI2CReadDi_base(0x00);
    CP_LOGD("PMIC_ID = 0x%x\r\n", regval);
	return regval;
}

static void PI2C_WRITE_REG(UINT8 page,UINT8 addr, UINT8 value)
{
	//INT32   result; 
	//UINT32  ICRRegValue;
	//UINT32  ISARRegValue;

	switch(page)
	{
    	case PMC_I2C_WRITE_BASE_SLAVE_ADDRESS:
    		USTICAI2CWriteDi_base(addr,value);
    		break;
    	case PMC_I2C_WRITE_POWER_SLAVE_ADDRESS:
    		USTICAI2CWriteDi_power(addr,value);
    		break;
    	case PMC_I2C_WRITE_GPADC_SLAVE_ADDRESS:
    		USTICAI2CWriteDi_GPADC(addr,value);
    		break;
    	case PMC_I2C_WRITE_TEST_SLAVE_ADDRESS:
    		USTICAI2CWriteDi_TEST(addr,value);
    		break;
    	case PMC_I2C_WRITE_AUDIO_SLAVE_ADDRESS:
    		USTICAI2CWriteDi_AUDIO(addr,value);
    		break;
    	default:
    		ASSERT(0);
	}
	return;
}

static UINT8 PI2C_READ_REG(UINT8 page,UINT8 addr)
{
	UINT8   ret=0;

	switch(page)
	{
    	case PMC_I2C_READ_BASE_SLAVE_ADDRESS:
    		ret = USTICAI2CReadDi_base(addr);
    		break;
    	case PMC_I2C_READ_POWER_SLAVE_ADDRESS:
    		ret = USTICAI2CReadDi_power(addr);
    		break;
    	case PMC_I2C_READ_GPADC_SLAVE_ADDRESS:
    		ret = USTICAI2CReadDi_GPADC(addr);
    		break;
    	case PMC_I2C_READ_TEST_SLAVE_ADDRESS:
    		ret = USTICAI2CReadDi_TEST(addr);
    		break;
    	case PMC_I2C_READ_AUDIO_SLAVE_ADDRESS:
    		ret = USTICAI2CReadDi_AUDIO(addr);
    		break;
    	default:
    		ASSERT(0);
	}

	return ret;
}

void PMIC_WRITE_REG_GPADC(UINT8 addr, UINT8 value) {PI2C_WRITE_REG(PMC_I2C_WRITE_GPADC_SLAVE_ADDRESS,addr,value);}
UINT8 PMIC_READ_REG_GPADC(UINT8 addr) {return PI2C_READ_REG(PMC_I2C_READ_GPADC_SLAVE_ADDRESS,addr);}

void PMIC_WRITE_REG_POWER(UINT8 addr, UINT8 value) {PI2C_WRITE_REG(PMC_I2C_WRITE_POWER_SLAVE_ADDRESS,addr, value);}
UINT8 PMIC_READ_REG_POWER(UINT8 addr) {return PI2C_READ_REG(PMC_I2C_READ_POWER_SLAVE_ADDRESS,addr);}

void PMIC_WRITE_REG_BASE(UINT8 addr, UINT8 value) {PI2C_WRITE_REG(PMC_I2C_WRITE_BASE_SLAVE_ADDRESS,addr,value);}
UINT8 PMIC_READ_REG_BASE(UINT8 addr) {return PI2C_READ_REG(PMC_I2C_READ_BASE_SLAVE_ADDRESS,addr);}

void PMIC_WRITE_REG_AUDIO(UINT8 addr, UINT8 value) {PI2C_WRITE_REG(PMC_I2C_WRITE_AUDIO_SLAVE_ADDRESS,addr,value);}
UINT8 PMIC_READ_REG_AUDIO(UINT8 addr) {return PI2C_READ_REG(PMC_I2C_READ_AUDIO_SLAVE_ADDRESS,addr);}

UINT8 PM812_GET_POWER_UP_LOG(void)
{
    switch (PMIC_ID_GET())
    {
        case PMIC_813:
        case PMIC_813A3:
        case PMIC_813S:
            return PMIC_READ_REG_BASE(NINGBO_PWRUP_LOG_REG);
        case PMIC_802:
        case PMIC_802S:
            return PMIC_READ_REG_BASE(GUILIN_PWRUP_LOG_REG);
        case PMIC_803:
            return PMIC_READ_REG_BASE(GUILIN_LITE_PWRUP_LOG_REG);
        default:
            ASSERT(0);
            return 0;
    }
}

void PMIC_FAULT_WU_EN_ENABLE()
{
	UINT8 tmp;
	switch(pmic_id){
		case PMIC_812:
			tmp = PMIC_READ_REG_BASE(PM812_RTC_MISC_5_REG);
			tmp |= (PM812_RTC_FAULT_WU_EN_BIT);
			PMIC_WRITE_REG_BASE(PM812_RTC_MISC_5_REG,tmp);
			break;
		case PMIC_813:
        case PMIC_813A3:
        case PMIC_813S:
			tmp = PMIC_READ_REG_BASE(NINGBO_FAULT_WU_REG);
			tmp |= (NINGBO_FAULT_WU_ENABLE_BIT);
			PMIC_WRITE_REG_BASE(NINGBO_FAULT_WU_REG,tmp);
		       break;
		case PMIC_802:
        case PMIC_802S:
			tmp = PMIC_READ_REG_BASE(GUILIN_FAULT_WU_REG);
			tmp |= (GUILIN_FAULT_WU_ENABLE_BIT);
			PMIC_WRITE_REG_BASE(GUILIN_FAULT_WU_REG,tmp);
			break;
		case PMIC_803:
			tmp = PMIC_READ_REG_BASE(GUILIN_LITE_FAULT_WU_REG);
			tmp |= (GUILIN_LITE_FAULT_WU_ENABLE_BIT);
			PMIC_WRITE_REG_BASE(GUILIN_LITE_FAULT_WU_REG,tmp);
		break;
		default:
			break;
	}
}

void PMIC_FAULT_WU_ENABLE()
{
	UINT8 tmp;
	switch(pmic_id){
		case PMIC_812:
		       tmp = PMIC_READ_REG_BASE(PM812_RTC_MISC_5_REG);
		       tmp |= (PM812_RTC_FAULT_WU_BIT);
		       PMIC_WRITE_REG_BASE(PM812_RTC_MISC_5_REG,tmp);
		       break;
		case PMIC_813:
        case PMIC_813A3:
			tmp = PMIC_READ_REG_BASE(NINGBO_FAULT_WU_REG);
			tmp |= (NINGBO_FAULT_WU_BIT);
			PMIC_WRITE_REG_BASE(NINGBO_FAULT_WU_REG,tmp);
		       break;
		case PMIC_802:
        case PMIC_802S:
			tmp = PMIC_READ_REG_BASE(GUILIN_FAULT_WU_REG);
			tmp |= (GUILIN_FAULT_WU_BIT);
			PMIC_WRITE_REG_BASE(GUILIN_FAULT_WU_REG,tmp);
			break;
		case PMIC_803:
			tmp = PMIC_READ_REG_BASE(GUILIN_LITE_FAULT_WU_REG);
			tmp |= (GUILIN_LITE_FAULT_WU_BIT);
			PMIC_WRITE_REG_BASE(GUILIN_LITE_FAULT_WU_REG,tmp);
			break;
        case PMIC_813S:
            tmp = PMIC_READ_REG_BASE(NINGBO_FAULT_WU_REG);
            tmp |= (NINGBO2_FAULT_WU_BIT);
            PMIC_WRITE_REG_BASE(NINGBO_FAULT_WU_REG,tmp);
            break;
		default:
			break;
	}
}

void dump_pmic_registers(void)
{
    unsigned long i;

    uart_printf("\r\nBase Page: \r\n");
    for (i=0;i<0xF8;i++)
    {
        uart_printf("@BP_0x%02x = 0x%02x\r\n",i,PMIC_READ_REG_BASE(i));
        //uart_printf("\r\n");
    }
    uart_printf("\r\nPower Page: \r\n");
    for (i=0;i<0x97;i++)
    {
        uart_printf("@PP_0x%02x = 0x%02x\r\n",i,PMIC_READ_REG_POWER(i));
        //uart_printf("\r\n");
    }
    uart_printf("\r\nGPADC Page: \r\n");
    for (i=0;i<0x72;i++)
    {
        uart_printf("@GP_0x%02x = 0x%02x\r\n",i,PMIC_READ_REG_GPADC(i));
       // uart_printf("\r\n");
    }
    
}

void PM812_SW_PDOWN(void)
{
    UINT8 tmp;

    PM812_FAULT_WU_EN_DISABLE();
    switch (PMIC_ID_GET())
    {
        case PMIC_813:
        case PMIC_813A3:
        case PMIC_813S:
            PMIC_WRITE_REG_BASE(0x4C,0x0); //disable PWM
            tmp = PMIC_READ_REG_BASE(NINGBO_MISC_CFG_REG1);
            tmp |= NINGBO_SW_PDOWN_BIT;
            PMIC_WRITE_REG_BASE(NINGBO_MISC_CFG_REG1,tmp);
            break;
        case PMIC_802:
        case PMIC_802S:
            tmp = PMIC_READ_REG_BASE(GUILIN_WAKEUP_REG1);
            tmp |= GUILIN_SW_PDOWN_BIT;
            PMIC_WRITE_REG_BASE(GUILIN_WAKEUP_REG1,tmp);
            break;
        case PMIC_803:
            tmp = PMIC_READ_REG_BASE(GUILIN_LITE_WAKEUP_REG1);
            tmp |= GUILIN_LITE_SW_PDOWN_BIT;
            PMIC_WRITE_REG_BASE(GUILIN_LITE_WAKEUP_REG1,tmp);
            break;
        default:
            ASSERT(0);
            break;
    }
}

void PMIC_SW_RESET(void)
{
		UINT8 tmp;
		UINT32 callerAddress ;
	
		//dump_pmic_registers();
	
#if defined(__ARMCC_VERSION)
		callerAddress= (__ARMCC_VERSION >= 200000) ? __return_address() : 0 ;  //  Keep first.
#else
		callerAddress = 0;
#endif
	
	
		PMIC_WRITE_REG_BASE(NINGBO_INT_ENABLE_REG1,0x00);
		PMIC_WRITE_REG_BASE(NINGBO_INT_ENABLE_REG2,0x00);
		//PMIC_WRITE_REG_GPADC(NINGBO_GPADC_MODE_CONTROL_REG,0);
		//PMIC_WRITE_REG_BASE(0xef,0x03);

		//mdelay(10000);
		uart_printf("SW_RESET @0x%x\r\n",callerAddress);
		//mdelay(5000);

	PMIC_FAULT_WU_EN_ENABLE();
	PMIC_FAULT_WU_ENABLE();

	//PMIC_WRITE_REG_POWER(NINGBO_VBUCK2_ACTIVE_VOUT_REG,0xDB); //buck2=2.15v
	//uart_printf("BP_0x15 @0x%x\r\n",PMIC_READ_REG_BASE(NINGBO_BAT_TEMP_CTRL_REG));
	//uart_printf("BP_0x12 @0x%x\r\n",PMIC_READ_REG_BASE(0x12));
	
	mdelay(5);

	//SW_PDOWN
	switch(pmic_id){
		case PMIC_812:
		       tmp = PMIC_READ_REG_BASE(PM812_WAKEUP_1_REG);
		       tmp |= PM812_SW_PDOWN_BIT;
		       PMIC_WRITE_REG_BASE(PM812_WAKEUP_1_REG,tmp);
		       break;
		case PMIC_813:
        case PMIC_813A3:
        case PMIC_813S:
		       PMIC_WRITE_REG_BASE(0x4C,0x0); //disable PWM
		       tmp = PMIC_READ_REG_BASE(NINGBO_MISC_CFG_REG1);
		       tmp |= NINGBO_SW_PDOWN_BIT;
		       PMIC_WRITE_REG_BASE(NINGBO_MISC_CFG_REG1,tmp);
		       break;
		case PMIC_802:
        case PMIC_802S:
	    		/*TODO*/
#ifdef FOTA_ASRSPI	
				/* CP reset for 2-chip solution */
				Guilin_SW_Reset();
#endif
				tmp = PMIC_READ_REG_BASE(GUILIN_WAKEUP_REG1);
				tmp |= GUILIN_SW_PDOWN_BIT;
				PMIC_WRITE_REG_BASE(GUILIN_WAKEUP_REG1,tmp);
	    		break;
				
        case PMIC_803:
            tmp = PMIC_READ_REG_BASE(GUILIN_LITE_WAKEUP_REG1);
            tmp |= GUILIN_LITE_SW_PDOWN_BIT;
            PMIC_WRITE_REG_BASE(GUILIN_LITE_WAKEUP_REG1,tmp);
            break;
		default:
			break;

	}
	mdelay(10000);
	uart_printf("RST end\r\n");
	while(1);

}



BOOL PMIC_ID_GET(void)
{
    if (pmic_id == PMIC_UNAVAILABLE)
	    pmic_id = (GetPMICID()&0xF8)>>3;//initial pmic_id after boot
    return pmic_id;
}

/* Following APIs reflect PMIC series. */
BOOL PMIC_IS_PM812(void)
{
	return (PMIC_ID_GET() == PMIC_812);
}

BOOL PMIC_IS_PM813(void)
{
	return ((PMIC_ID_GET() == PMIC_813)||(PMIC_ID_GET() == PMIC_813A3));
}

BOOL PMIC_IS_PM802(void)
{
	return (PMIC_ID_GET() == PMIC_802);
}
BOOL PMIC_IS_PM803(void)
{
	return (PMIC_ID_GET() == PMIC_803);
}

BOOL PMIC_IS_PM813S(void)
{
	return (PMIC_ID_GET() == PMIC_813S);
}

BOOL PMIC_IS_PM802S(void)
{
	return (PMIC_ID_GET() == PMIC_802S);
}

void PMIC_Init(void)
{
    usticaInit(); //init clock and TWSI ctrl
    if (PMIC_IS_PM802()||PMIC_IS_PM802S())
    {
        GuilinClkInit();
    }    
	else if (PMIC_IS_PM813()||PMIC_IS_PM813S())
	{
        NingboClkInit();
	}
}

static void PM812_LDO_VOUT_CFG(int addr,int act_vol,int slp_vol)
{
       UINT32           ICRRegValue;
       UINT32       ISARRegValue;

       I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
       I2C_REG_READ(I2C_ISAR_REG,ISARRegValue);

       usticaInit();

       if(act_vol & slp_vol)ASSERT(0);

       USTICAI2CWriteDi_power(addr,(act_vol|slp_vol));

       I2C_REG_WRITE(I2C_ISAR_REG,ISARRegValue);
       I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue);

}

static void PM812_LDO_Enable(int ldo_num,BOOL OnOff)
{
       unsigned char regval;
       UINT32          ICRRegValue;
       UINT32          ISARRegValue;

       I2C_REG_READ(I2C_ICR_REG,ICRRegValue);
       I2C_REG_READ(I2C_ISAR_REG,ISARRegValue);
       usticaInit();

       //cfg enable reg-1
       regval = USTICAI2CReadDi_power(PM812_LDO_ENABLE_REG_1(ldo_num));
       USTICAI2CWriteDi_power( PM812_LDO_ENABLE_REG_1(ldo_num),
                                                       OnOff?(regval|PM812_LDO_ENABLE_BIT(ldo_num)):(regval &(~ PM812_LDO_ENABLE_BIT(ldo_num)))
                                                       );
       //cfg enable reg-2
       regval = USTICAI2CReadDi_power(PM812_LDO_ENABLE_REG_2(ldo_num));
       USTICAI2CWriteDi_power( PM812_LDO_ENABLE_REG_2(ldo_num),
                                                       OnOff?(regval|PM812_LDO_ENABLE_BIT(ldo_num)):(regval &(~ PM812_LDO_ENABLE_BIT(ldo_num)))
                                                       );

       I2C_REG_WRITE(I2C_ISAR_REG,ISARRegValue);
       I2C_REG_WRITE(I2C_ICR_REG, ICRRegValue);

}

void PM812_Ldo_12_set_1_8(void)
{
       PM812_LDO_VOUT_CFG(PM812_LDO12_VOUT_REG,PM812_LDO3_TO_LDO17_1V80_ACT,PM812_LDO3_TO_LDO17_1V80_SLP);
}

void PM812_Ldo_12_set_2_8(void)
{
    PM812_LDO_VOUT_CFG(PM812_LDO12_VOUT_REG, PM812_LDO3_TO_LDO17_2V80_ACT,PM812_LDO3_TO_LDO17_2V80_SLP);
}

void PM812_Ldo_12_set_3_0(void)
{
       PM812_LDO_VOUT_CFG(PM812_LDO12_VOUT_REG,PM812_LDO3_TO_LDO17_3V00_ACT,PM812_LDO3_TO_LDO17_3V00_SLP);
}

void PM812_Ldo_12_set(BOOL OnOff)
{
       PM812_LDO_Enable(12,OnOff);
}

void PM812_Ldo_13_set_2_8(void)
{
        PM812_LDO_VOUT_CFG(PM812_LDO13_VOUT_REG, PM812_LDO3_TO_LDO17_2V80_ACT,PM812_LDO3_TO_LDO17_2V80_SLP);
}

void PM812_Ldo_13_set(BOOL OnOff)
{
        PM812_LDO_Enable(13,OnOff);
}

BOOL PMIC_COMP_ID_GET(void)
{
    static UINT8 pmic_complete_id = PMIC_UNAVAILABLE;
    if (pmic_complete_id == PMIC_UNAVAILABLE)
	    pmic_complete_id = GetPMICID();
    return pmic_complete_id;
}


BOOL PMIC_IS_PM813_A3(void)
{
	return (PMIC_COMP_ID_GET() == PMIC_813_A3);
}

BOOL PMIC_ONKEY_IS_DETECTED(void)
{
	switch (PMIC_ID_GET())
	{
        case PMIC_813:
        case PMIC_813A3:
        case PMIC_813S:
            return ((PMIC_READ_REG_BASE(NINGBO_STATUS_REG1) & NINGBO_ONKEY_STATUS_BIT) ? TRUE : FALSE);
        case PMIC_802:
        case PMIC_802S:
            return ((PMIC_READ_REG_BASE(GUILIN_STATUS_REG1) & GUILIN_ONKEY_PRESSED) ? TRUE : FALSE);
        case PMIC_803:
            return ((PMIC_READ_REG_BASE(GUILIN_LITE_STATUS_REG1) & GUILIN_LITE_ONKEY_PRESSED) ? TRUE : FALSE);
        default:
            ASSERT(0);
            return FALSE;
	}
}

BOOL PM812_EXTON_IS_DETECTED(void)
{
	//check whether EXTON pin is connected 
	switch (PMIC_ID_GET())
	{
        case PMIC_813:
            return ((PMIC_READ_REG_BASE(NINGBO_STATUS_REG1) & (NINGBO_VBUS_STATUS_BIT | NINGBO_VBUS_OVP_BIT | NINGBO_VBUS_UVLO_BIT)) ? TRUE : FALSE);
        case PMIC_813A3:
        case PMIC_813S:
            return (((PMIC_READ_REG_BASE(NINGBO_READONLY_DATA_2_REG) & (NINGBO_RONLY_DATA_VBUS_OV|NINGBO_RONLY_DATA_VBUS_UV|NINGBO_RONLY_DATA_CHG_OK))==NINGBO_RONLY_DATA_CHG_OK) ? TRUE : FALSE);
        case PMIC_802:
        case PMIC_802S:
            return ((PMIC_READ_REG_BASE(GUILIN_STATUS_REG1) & GUILIN_EXTON1_DETECT) ? TRUE : FALSE);
        case PMIC_803:
            return ((PMIC_READ_REG_BASE(GUILIN_LITE_STATUS_REG1) & GUILIN_LITE_EXTON1_DETECT) ? TRUE : FALSE);
        default:
            ASSERT(0);
            return FALSE;
	}
}

BOOL PM812_CHARGER_IS_DETECTED(void)
{
	//for FIH board only check EXTON_STATUS
	return PM812_EXTON_IS_DETECTED();
}

BOOL PMIC_CHARGER_IS_DETECTED(void)
{
	return PM812_EXTON_IS_DETECTED();
}

void PM812_FAULT_WU_EN_DISABLE()
{
    UINT8 tmp;

    if (PMIC_IS_PM812())
    {    
    	tmp = PMIC_READ_REG_BASE(PM812_RTC_MISC_5_REG);
    	tmp &= ~(PM812_RTC_FAULT_WU_EN_BIT);
    	PMIC_WRITE_REG_BASE(PM812_RTC_MISC_5_REG,tmp);
	}
	else
	{
        tmp = PMIC_READ_REG_BASE(NINGBO_FAULT_WU_REG);
        tmp &= ~(NINGBO_FAULT_WU_ENABLE_BIT);
        PMIC_WRITE_REG_BASE(NINGBO_FAULT_WU_REG,tmp);
	}
}

void PMIC_PowerDown(void)
{
    UINT8 tmp;

	uart_printf("PDOWN!\r\n");

    PM812_FAULT_WU_EN_DISABLE();
        
    if (PMIC_IS_PM812())
    {
    	tmp = PMIC_READ_REG_BASE(PM812_WAKEUP_1_REG);
    	tmp |= PM812_SW_PDOWN_BIT;
    	PMIC_WRITE_REG_BASE(PM812_WAKEUP_1_REG,tmp);
	}
	else
	{
    	PMIC_WRITE_REG_BASE(0x4C,0x0); //disable PWM
    	tmp = PMIC_READ_REG_BASE(NINGBO_MISC_CFG_REG1);
    	tmp |= NINGBO_SW_PDOWN_BIT;
    	PMIC_WRITE_REG_BASE(NINGBO_MISC_CFG_REG1,tmp);
	}
}
void PM812_VBUCK1_CFG(UINT8 value)
{
	PMIC_BUCK_T val = (PMIC_BUCK_T)value;
	if((val<BUCK_0_800)||(val>BUCK_1_300)){
		uart_printf("wrong buck1 value input! \r\n");
		return;
	}
	PMIC_WRITE_REG_POWER(USTICA_VBUCK_1_SET_0_REG,val);
	
}

void PlatformVcoreConfigTop(void)
{
	//uart_printf("PlatformVcoreConfigTop! PMIC_ID_GET[%08x]\r\n",PMIC_ID_GET());
	//Set 1.10V by default for top level
	switch(PMIC_ID_GET())
	{
		case PMIC_812:
			PM812_VBUCK1_CFG(BUCK_1_100);
			break;
		case PMIC_813:
        case PMIC_813A3:
		case PMIC_813S:
			Ningbo_VBUCK1_CFG(NINGBO_VBUCK_1V1000);
			break;
		case PMIC_802:
        case PMIC_802S:
			Guilin_VBUCK1_CFG(GUILIN_VBUCK_1V1000);
			break;
		default:
//			ASSERT(0);
			break;
	}
}


void PlatformVcoreConfigHigh(void)
{
	//uart_printf("PlatformVcoreConfigHigh! PMIC_ID_GET[%08x]\r\n",PMIC_ID_GET());
	//Set 1.05V by default for higher level
	switch(PMIC_ID_GET())
	{
		case PMIC_812:
			PM812_VBUCK1_CFG(BUCK_1_050);
			break;
		case PMIC_813:
        case PMIC_813A3:
		case PMIC_813S:
			Ningbo_VBUCK1_CFG(NINGBO_VBUCK_1V0500);
			break;
		case PMIC_802:
        case PMIC_802S:
			Guilin_VBUCK1_CFG(GUILIN_VBUCK_1V0500);
			break;
		default:
//			ASSERT(0);
			break;
	}
}


void PlatformVcoreConfigLow(void)
{
	//uart_printf("PlatformVcoreConfigLow! PMIC_ID_GET[%08x]\r\n",PMIC_ID_GET());
	//Set 1.00V by default
	switch(PMIC_ID_GET())
	{
		case PMIC_812:
			PM812_VBUCK1_CFG(BUCK_1_000);
			break;
		case PMIC_813:
        case PMIC_813A3:
		case PMIC_813S:
			Ningbo_VBUCK1_CFG(NINGBO_VBUCK_1V0000);
			break;
		case PMIC_802:
        case PMIC_802S:
			Guilin_VBUCK1_CFG(GUILIN_VBUCK_1V0000);
			break;
		default:
//			ASSERT(0);
			break;
	}
}



void pmic_user_defined_flags_set(UINT8 flag)
{
    UINT8 var = 0;

    if(PMIC_IS_PM813()||PMIC_IS_PM813S())
    {
        var=PMIC_READ_REG_BASE(PMIC_REG_FOR_USR_DEF_FLAGS_STR);
        var &= ~0xF;    
        var |= (flag & 0xF); //only use low 4 bits
        PMIC_WRITE_REG_BASE(PMIC_REG_FOR_USR_DEF_FLAGS_STR, var);
    }
    else if(PMIC_IS_PM802()||PMIC_IS_PM803()||PMIC_IS_PM802S())
    {
        var=PMIC_READ_REG_BASE(PM802_REG_FOR_USR_DEF_FLAGS_STR);
        var &= ~0xF0;    
        var |= (flag<<4); //only use high 4 bits
        PMIC_WRITE_REG_BASE(PM802_REG_FOR_USR_DEF_FLAGS_STR, var);
    }
}

UINT8 pmic_user_defined_flags_get(void)
{
    UINT8 var = 0;

    if(PMIC_IS_PM813()||PMIC_IS_PM813S())
    {
        var = PMIC_READ_REG_BASE(PMIC_REG_FOR_USR_DEF_FLAGS_STR);
        return (var & 0xF); //only use low 4 bits 
    }
    else if(PMIC_IS_PM802()||PMIC_IS_PM803()||PMIC_IS_PM802S())
    {
        var=PMIC_READ_REG_BASE(PM802_REG_FOR_USR_DEF_FLAGS_STR);
        return (var>>4); //only use high 4 bits 
    }
	else
	{
		return var;
	}
}

/* Get Restart Reson*/
UINT8 SysRestartReasonGlobal=0;
UINT32 SysRestartReasonGetFlag=0;

UINT8 SysRestartReasonGet(void)
{
    UINT8 desc[40];
    UINT8 reason;

	if(SysRestartReasonGetFlag==0)
		reason=PMIC_READ_REG_BASE(PMIC_REG_FOR_SW_RESTART_REASON_STR);
	else
	    reason = SysRestartReasonGlobal;

	if(reason==0)
		reason='N';
	
    switch (reason)
    {
        case 'C':
        case 'R':
        case 'A':
        case 'N':
        case 'E':
            break;
        default:
            return 0;
    }
    return reason;
}

UINT8 isSysRestartByRdProduction(void)
{
    return (SysRestartReasonGet() == 'R');
}

void Updater_Reset(void)
{
	UINT32 callerAddress ;
		
#if defined(__ARMCC_VERSION)
			callerAddress= (__ARMCC_VERSION >= 200000) ? __return_address() : 0 ;  //  Keep first.
#else
			callerAddress = 0;
#endif
	uart_printf("%s @0x%x\r\n",__func__,callerAddress);
	//uart_printf("01-@BP_0x%02x = 0x%02x\r\n",NINGBO_RTC_USER_DATA_4_REG,PMIC_READ_REG_BASE(NINGBO_RTC_USER_DATA_4_REG));
	pmic_user_defined_flags_set(RESET_FOTA_FLAG);
	//uart_printf("02-@BP_0x%02x = 0x%02x\r\n",NINGBO_RTC_USER_DATA_4_REG,PMIC_READ_REG_BASE(NINGBO_RTC_USER_DATA_4_REG));
	PMIC_SW_RESET();
}









