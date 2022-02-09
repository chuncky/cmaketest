/******************************************************************************
*              MODULE IMPLEMENTATION FILE
*******************************************************************************
*  COPYRIGHT (C) 2001 Intel Corporation.
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
*  Title: intc
*
*  Filename: intc.c
*
*  Target, subsystem: Common Platform, HOP
*
*  Authors:  Alexander Rivman
*
*  Description:  first release  - Tavor interrupt controller .
*
*  Create : 17-Jul-2005
*
*  Notes:
* Modified:                                                            
*  March 2007 YK - separate INTCD2recover intc_memRetain 
*		to be in memory retain state also when DDR is not available		
******************************************************************************/
#define  _INTC_C_  (1)
/* TEMPORARY*/
#define dlDisableIrq disableInterrupts
/* TEMPORARY*/
#include "common.h"
#include "bsp.h"
#include "intc.h"

//#endif
#ifndef Int16
typedef short Int16;
#endif

static INTC_ISR irqVectorLookup[IRQ_VECT_SIZE];
//static INTC_InterruptInfo Unhandled_Num ;
static void Unhandled_ISR (INTC_InterruptInfo interruptInfo)
{
    ASSERT(0);
	INTCClear((INTC_InterruptSources)interruptInfo);
	//Unhandled_Num =  interruptInfo;
}
//#define UNHANDLED_VEC   (INTC_ISR)0
#define UNHANDLED_VEC   Unhandled_ISR

UINT32 LvlEdgeShadowReg_0;/* Shadow register of WrOnly Edge/Level register-0 */
UINT32 LvlEdgeShadowReg_1;/* Shadow register of WrOnly Edge/Level register-1 */
/*
 * Use interrupts 10..14 for HW
 */
INTC_emulation_priority_table_S INTC_emulation_priority_table[INTC_MAX_INTERRUPT_SOURCES] =
{
// One of the files below should provide a table of INTC->XIRQ mapping and priorities,
// which is maintained in a separate file for each silicon adrchitecture.
/*
 * XIRQ table for TAVOR Harbell (coupled with the interrupt list, see intc_list.h).
 * THIS FILE CAN BE #include'd ONLY ONCE
 */
	{SW_INT_SRC_0  ,  15  },
	{SW_INT_SRC_1  ,  15  },
	{SW_INT_SRC_2  ,  15  },
	{SW_INT_SRC_3  ,  15  },
	{SW_INT_SRC_4  ,  15   },
	{SW_INT_SRC_5  ,  15  },
	{SW_INT_SRC_6  ,  15  },
	{SW_INT_SRC_7  ,  15  },
	{SW_INT_SRC_8  ,  15  },
	{SW_INT_SRC_9  ,  15  },
	{SW_INT_SRC_10 ,  15  },
	{SW_INT_SRC_11 ,  15  },
	{SW_INT_SRC_12 ,  15  },
	{SW_INT_SRC_13 ,  15  },
	{SW_INT_SRC_14 ,  15  },
	{SW_INT_SRC_15 ,  15  },
	/* Internal Interrupts (or register interrupts ) [16:23] , 8 possible sources*/
	{RI_INT_SRC_0  ,  0  },
	{RI_INT_SRC_1  ,  7  },
	{RI_INT_SRC_2  ,  5  },
	{RI_INT_SRC_3  ,  4  },
	{RI_INT_SRC_4  ,  5  },
	{RI_INT_SRC_5  ,  10 },
	{RI_INT_SRC_6  ,  15 },
	{RI_INT_SRC_7  ,  2  },
	{XSWI_INT_SRC_0,  0  },
	/* iNTERNAL EXC  7  },EPTION 3 INTERRUPTS*/
	{EIRQ_INT_0    ,  10 },
	{EIRQ_INT_1    ,  15 },
	{EIRQ_INT_2    ,  2  },
	/* HW interrupts - sourced by GB peripherals (including the Modem) 48 possible interrupts*/
	{INTC_HW_DMA_C3_0 ,  0  },
	{INTC_HW_DMA_C3_1 ,  7  },
	{INTC_HW_DMA_C3_2 ,  5  },
	{INTC_HW_DMA_C3_3 ,  4  },
	{INTC_HW_DMA_C3_4 ,  5  },
	{INTC_HW_DMA_C3_5 ,  10 },
	{INTC_HW_DMA_C3_6 ,  15 },
	{INTC_HW_DMA_C3_7 ,  2  },
	{INTC_HW_DMA_C2_0 ,  0  },

	{INTC_HW_DSPP_3   ,  4  },
	{INTC_HW_DMA_C2_2 ,  7  },
	{INTC_HW_DMA_C2_3 ,  5  },
	{INTC_HW_DMA_C2_4 ,  4  },
	{INTC_HW_DMA_C2_5 ,  5  },
	{INTC_HW_DMA_C2_6 ,  10 },
	{INTC_HW_DMA_C2_7 ,  15 },
	{INTC_HW_SLOW_CLK_SNAP ,  2  },
	{INTC_HW_EarlyWKUP ,7},	
	{INTC_HW_MSL      ,  5  },
	{INTC_HW_EXTPAD_1 ,  4  },
	{INTC_HW_EXTPAD_2 ,  5  },
	{INTC_HW_USIM     ,  10 },
	{INTC_HW_I2C      ,  4 },

	{INTC_SRC_IPC_DATA_ACK ,  7  },
	{INTC_HW_IpcDatBuffAck ,  0 },
	{INTC_SRC_IPC_RD       ,  5  },
	{INTC_SRC_IPC_ACK      ,  4  },
	{INTC_SRC_IPC_GP       ,  5  },


	{INTC_HW_CPMU     ,  4  },
	{INTC_HW_TCU_EXCP ,  7  },
	{INTC_HW_RTU_0    ,  4  },  //{INTC_HW_RTU_0    ,  5  }, //20090909, asked by MaXh
	{INTC_HW_RTU_1    ,  4  },
	{INTC_HW_RTU_2    ,  5  },
	{INTC_HW_Tmr1     ,  10 },
    {INTC_HW_Tmr2     ,  15 },
    {INTC_HW_Tmr3BkUp ,  10  },
    {INTC_HW_Tmr4BkUp ,  0  },
    {INTC_HW_TmrWDT   ,  3  },
	{INTC_HW_ECIPHER  ,  7  },
	{INTC_HW_TCU_GPOB ,  5  },
	{INTC_HW_ADPC     ,  4  },
	{INTC_HW_END_SLEEP_BKUP,5 },
    {INTC_HW_TCU_CLK_EN     , 10 },
    {INTC_HW_CL_GB_HINT , 15 },
    {INTC_SRC_CIPHER,14}
	,{INTC_BOERNE_CLK_CHG,	  0			},
	{INTC_GSM_WAKE_UP,		  1			},
	{INTC_WBCDMA_WAKE_UP,	  2			},
	{INTC_CRXD_32_28_EVENT,	  3			},
	{INTC_CRXD_27_0_EVENT,	  4			},
	{INTC_GSSP1_WAKE_UP,	  0			},
	{INTC_GSSP2_WAKE_UP,	  1			},
	{INTC_GSIM_UDET_UIO,	  2			},
	{INTC_CI2C_SCL_SDA_WAKE_UP, 3		},
	{INTC_COMM_UART,		  4			},
	{INTC_GB_DMA_C2,		  0			},
	{INTC_AC_IPC_0,			  1		    },
	{INTC_D2_MODEM_OK,		  2		    },
	{INTC_AC_IPC_1,			  3		    },
	{INTC_AC_IPC_2,			  4		    },
	{INTC_AC_IPC_3,			  0		    }
	// no setting for: INTC_CI2C_SCL_SDA_WAKE_UP_DUMMY_I2C_2_TEST - NOT A REAL HW INTERRUPT


};   /* INTC_emulation_priority_table */

/*  variables of module, used in D2 recovery */
volatile INTC_HW_CONF_REGS_Y* IntcConfigController = (volatile INTC_HW_CONF_REGS_Y*)IRQCTRL_BASE_ADDR;

#if defined(XIRQ_D2_SPECIFIC)
extern INTC_HW_CONF_REGS_Y    INTC_D2_Backup_str ;
#define INTC_D2_BACKUP_SET(field) INTC_D2_Backup_str.field = IntcConfigController->field
#else
#define INTC_D2_BACKUP_SET(field) 
#endif


//#define INTC_OUT_OF_RANGE(s)            (s >= INTC_MAX_PRIMARY_INTERRUPT_SOURCES)
#ifdef _TCU_ON_PRIMARY_
      #define INTC_TCU_RANGE(s)			((s <= INTC_SRC_TTPCOM_TCU_IRQ0) && (s >= INTC_SRC_TTPCOM_TCU_IRQ2))
#else
      #define INTC_TCU_RANGE(s)			(FALSE)
#endif

#define IS_CHIPID_HELANLTE (((*(volatile UINT32*)0xD4282C00)&0xffff)==0x1188)
#define IS_CHIPID_CRANE (((*(volatile UINT32*)0xD4282C00)&0xffff)==0x1188)  //what is CRANE chipid 

// check source number boundry
#if !defined (INTC_GPIO_01_WIRED_TO_ICU)

#define IS_VIA_GPIO(s)                  ((s >= INTC_SRC_GPIO_0)&&(s < INTC_MAX_PRIMARY_INTERRUPT_SOURCES))

#define CONVERT_INTC_TO_GPIO_PIN(s)     (GPIO_PinNumbers)(s - INTC_SRC_GPIO_0 + GPIO_PIN_0)
// Note: INTC_SRC_GPIO_COMBINED is allowed now (see disabled check in CHECK_VALID_SOURCE).
// Reasons: flexibility (it is possible to access this source via INTC API); INTC code can use it's API to configure this source.
#define CHECK_VALID_SOURCE(s)           if ((s >= INTC_MAX_INTERRUPT_SOURCES) /*|| (s==INTC_SRC_GPIO_COMBINED)*/) \
                                            return (INTC_RC_INVALID_SRC)
#else

#define IS_VIA_GPIO(s)                  ((s >= INTC_SRC_GPIO_2)&&(s < INTC_MAX_PRIMARY_INTERRUPT_SOURCES))

#define CONVERT_INTC_TO_GPIO_PIN(s)     (GPIO_PinNumbers)( ((s == INTC_SRC_GPIO_0) || (s == INTC_SRC_GPIO_1)) ? (s - INTC_SRC_GPIO_0 + GPIO_PIN_0) : s - INTC_SRC_GPIO_0_DUMMY + GPIO_PIN_0)
#define CHECK_VALID_SOURCE(s)           if ((s >= INTC_MAX_INTERRUPT_SOURCES) /*|| (s==INTC_SRC_GPIO_COMBINED)*/ || \
											(s == INTC_SRC_GPIO_0_DUMMY) || (s == INTC_SRC_GPIO_1_DUMMY)) \
                                            return (INTC_RC_INVALID_SRC)
#endif

#define ICU_INT_PRIO(i) 1

       //INTC_ISR _interruptHandlers[MAX_INTERRUPT_CONTROLLER_SOURCES];
       INTC_ISR _interruptHandlers[MAX_INTERRUPT_CONTROLLER_SOURCES_MACRO];
static INTC_ISR _interruptGPIOHandlers[GPIO_MAX_AMOUNT_OF_PINS];

static UINT32   _defaultIsrCounter = 0;
static UINT32   _defaultIsrGPIOCounter = 0;

UINT32 maxInterruptControllerSources = 0;   // replacing MAX_INTERRUPT_CONTROLLER_SOURCES define with runtime variable?
UINT32 interruptNumMask = 0;                // replacing INTERRUPT_NUM_MASK define with runtime variable.
UINT32 numofPrioritiesRegiser1 = 0, numofPrioritiesRegiser2 = 0, numofPrioritiesRegiser3 = 0;   // replacing NUM_OF_PRIORITY_REGISTERS_X define with runtime variable.

static void INTCDefaultGPIOInterruptServiceRoutine(INTC_InterruptInfo irqInfo)
{
    _defaultIsrGPIOCounter++;
    // Need to send error message to UART / error handler

} /* end of INTCDefaultGPIOInterruptServiceRoutine */




UINT32 INTCGetIntVirtualNum(UINT32 IntAbsNum)
{
	return INTC_MAX_PRIMARY_INTERRUPT_SOURCES + IntAbsNum;
}



/************************************************************************
* Function: INTCBindVirtual                                             *
*************************************************************************
* Description: Binds handler to the virtual interrupt number            *
*																		*
* Parameters:                                      						*
*	  virtualIntAbsNum - number from  MAX_NUM_IRQ  to  IRQ_VECT_SIZE    *
*	  isr			   - pointer to interrupt source    				*
* Return value:  None												    *
* Return value:  INTC_RC_OK												*
*                INTC_RC_INVALID_SRC									*
*                INTC_RC_NO_ISR_BOUND									*
************************************************************************/
void INTCBindVirtualInit(UINT32 virtualIntAbsNum)
{
	irqVectorLookup[virtualIntAbsNum] = UNHANDLED_VEC;

}

INTC_ReturnCode INTCBindVirtual(UINT32 virtualIntAbsNum,INTC_ISR isr)
{

	/*Fix coverity[overrun-local]*/

	if(virtualIntAbsNum >= IRQ_VECT_SIZE )
		return INTC_RC_BAD_ISR_PARAMETER;

	if(virtualIntAbsNum < MAX_NUM_IRQ )
		return INTC_RC_BAD_ISR_PARAMETER;

	if (irqVectorLookup[virtualIntAbsNum] != UNHANDLED_VEC)
		return INTC_RC_ISR_ALREADY_BOUND;

	/* Set Isr */
    irqVectorLookup[virtualIntAbsNum] = isr;

    return INTC_RC_OK;

} /*INTCBindVirtual*/

void GPIOClearUsedForInterrupt(GPIO_PinNumbers pinNum) {}
BOOL GPIOSetUsedForInterrupt(GPIO_PinNumbers pinNum) {return TRUE;}



INTC_ReturnCode INTCIdleMaskDisable(void)
{
	    InterruptController.own.ICU_GLOBAL_MASK = 0;
	    
    return INTC_RC_OK;
} /* End of INTCIdleMaskDisable */



INTC_ReturnCode INTC_P_ICU_Init(void)
{
	INTC_ReturnCode rc=INTC_RC_OK;
	//int i;

	/* mask ICU 0 */
	ICU_MASK_INT(0)

	//for(i=0;i<P_ICU_NUM_SOURCES;i++)
	//{
    // if(!ICU_INT_OWNED_BY_OTHER_CORE(i))  ICU_MASK_INT(i);
	//}

	// Don't allow all interrupts to wake the CPU - let only those who are enabled
    INTCIdleMaskDisable();

	return rc;
}

INTC_ReturnCode INTC_P_ICU_SetMask(INTC_InterruptSources isrSource, BOOL enable)
{
	    UINT32        reg, tmp;
		unsigned long cpsrReg;

        cpsrReg = disableInterrupts();
        /* coverity check */
        tmp = (IS_CHIPID_HELANLTE)?P_ICU_NUM_SOURCES_HELANLTE: P_ICU_NUM_SOURCES*2;
        ASSERT(isrSource<tmp);
 		if(isrSource < P_ICU_NUM_SOURCES)
			reg=InterruptController.all.ICU_CONF[isrSource];
		else 
			reg=InterruptController.all.ICU_CONF_H[isrSource -P_ICU_NUM_SOURCES];

		reg|=ICU_CONF_THIS_CORE_INT; // make sure this core owns this interrupt

        if(enable) reg|=ICU_INT_PRIO(isrSource);
		else       reg&=~ICU_CONF_PRIO_BITS;

		if(isrSource < P_ICU_NUM_SOURCES)
			InterruptController.all.ICU_CONF[isrSource]=reg;
		else 
			InterruptController.all.ICU_CONF_H[isrSource - P_ICU_NUM_SOURCES]=reg;
        restoreInterrupts(cpsrReg);
		return INTC_RC_OK;
} // INTC_P_ICU_SetMask


/************************************************************************
* Function: INTCEnable													*
*************************************************************************
* Description: Enables a specific source to invoke an interrupt			*
*																		*
* Parameters: isr - interrupt service routine to bind.  				*
* 			  isrSource - source number to bind							*
*			  IntPri - Priority of interrupt							*
*																		*
* Return value:  INTC_RC_OK												*
*                INTC_RC_INVALID_SRC									*
* Notes:																*
************************************************************************/
INTC_ReturnCode INTCEnable_XIRQ(INTC_InterruptSources isrSource)
{
	IRQCTRL_ENABLE(IRQCTRL_MCU,isrSource);
    return INTC_RC_OK;
} /* End of INTCEnable */

INTC_ReturnCode INTCEnable(INTC_InterruptSources isrSource)
{
    if (isrSource >= INTC_MAX_PRIMARY_INTERRUPT_SOURCES)
         INTC_P_ICU_SetMask((INTC_InterruptSources)(isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES), TRUE);
    else if(isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
	  INTCEnable_XIRQ(isrSource);
    return INTC_RC_OK;
} /* End of INTCEnable */
/***********************************************************************
* Function: INTCDisable                                                *
************************************************************************
* Description: Disables a specific source from invoking an interrupt   *
*                                                                      *
* Parameters: source - source number to disable						   *
*                                                                      *
* Return value:  INTC_RC_OK		                                       *
*                INTC_RC_INVALID_SRC                                   *
* Notes:                                                               *
***********************************************************************/
INTC_ReturnCode INTCDisable_XIRQ(INTC_InterruptSources isrSource)
{
    IRQCTRL_DISABLE(IRQCTRL_MCU, isrSource);
    return INTC_RC_OK;
} /* End of INTCDisable */

INTC_ReturnCode INTCDisable(INTC_InterruptSources isrSource)
{
    if (isrSource >= INTC_MAX_PRIMARY_INTERRUPT_SOURCES)
         INTC_P_ICU_SetMask((INTC_InterruptSources)(isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES), FALSE);
    else if(isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
	  INTCDisable_XIRQ(isrSource);

    return INTC_RC_OK;
} /* End of INTCDisable */

static void INTCDefaultInterruptServiceRoutine(INTC_InterruptInfo irqInfo)
{
    _defaultIsrCounter++;
    // Need to send error message to UART / error handler

} /* end of INTCDefaultInterruptServiceRoutine */




/************************************************************************
* Function: INTCBind													*
*************************************************************************
* Description : Binds an interrupt service routine to an interrupt		*
*               source.													*
*																		*
* Parameters: isr - interrupt service routine to bind.  				*
* 			  isrSource - source number to bind							*
*																		*
* Return value:  INTC_RC_OK		                                        *
*                INTC_RC_INVALID_SRC                                    *
*				 INTC_RC_BAD_ISR_PARAMETER								*
*                INTC_RC_ISR_ALREADY_BOUND                              *
*																		*
* Notes:																*
*	The name was changed since a parameter is discardede (see intc.h	*
*																		*
************************************************************************/
INTC_ReturnCode INTCBind_XIRQ(INTC_InterruptSources isrSource, INTC_ISR isr)
{
	UINT32 IntPri;
    UINT32 IntAbsSrc=isrSource;
	INTC_ReturnCode  res_code;
	
  //  CHECK_VALID_INTC_SOURCE(isrSource);
	if(isrSource >=MAX_NUM_IRQ)
		return INTC_RC_BAD_ISR_PARAMETER;

	if (irqVectorLookup[IntAbsSrc] != UNHANDLED_VEC)
		return INTC_RC_ISR_ALREADY_BOUND;

	/* Set Isr */
    irqVectorLookup[IntAbsSrc] = isr;
	/* Set priority */
	IntPri = INTC_emulation_priority_table[IntAbsSrc].InterruptPriority;
    IRQ_PRI(IRQCTRL_MCU, IntAbsSrc, IntPri);
	/* Enable */
	res_code = INTC_RC_OK;
    return res_code;
} /* End of INTCBind */


INTC_ReturnCode INTCBind(INTC_InterruptSources isrSource, INTC_ISR isr)
{
    CHECK_VALID_SOURCE(isrSource);
	
    if (isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
		return INTCBind_XIRQ(isrSource, isr);
	else if(isrSource < INTC_SRC_VIRTUAL_END)
		return INTCBindVirtual(isrSource, isr);

	if(isr == NULL)
		return INTC_RC_BAD_ISR_PARAMETER;

	//check whether an ISR is already bound to this source
    if(IS_VIA_GPIO(isrSource))
	{
        GPIO_PinNumbers gpioPin = CONVERT_INTC_TO_GPIO_PIN(isrSource);

        // if via GPIO, change pin status to usedForInterrupt
         /*have checked the index*/
	 	/*coverity[overrun-local]*/
        if(_interruptGPIOHandlers[gpioPin] != INTCDefaultGPIOInterruptServiceRoutine)
            return INTC_RC_ISR_ALREADY_BOUND;

        if( !GPIOSetUsedForInterrupt(gpioPin) )
            return INTC_RC_INVALID_SRC;
        _interruptGPIOHandlers[gpioPin] = isr;
	}
    else
    {
        if(_interruptHandlers[isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES] != INTCDefaultInterruptServiceRoutine)
            return INTC_RC_ISR_ALREADY_BOUND;

        _interruptHandlers[isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES] = isr;
    }

    return INTC_RC_OK;
} /* End of INTCBind */
/************************************************************************
* Function: INTCUnbind													*
*************************************************************************
* Description: Unbinds an interrupt service routine from an interrupt   *
*              source, and disables it.									*
*																		*
* Parameters: isr - interrupt service routine to bind.  				*
* 			  isrSource - source number to bind							*
*			  IntPri - Priority of interrupt							*
*																		*
* Return value:  INTC_RC_OK		                                        *
*                INTC_RC_INVALID_SRC                                    *
*				 INTC_RC_NO_ISR_BOUND									*
*																		*
* Notes:																*
************************************************************************/
INTC_ReturnCode INTCUnbind_XIRQ(INTC_InterruptSources isrSource)
{
	// check if the default function is bound
	if (UNHANDLED_VEC == irqVectorLookup[isrSource])
		return INTC_RC_NO_ISR_BOUND;

	/* Remove Isr */
	irqVectorLookup[isrSource] = UNHANDLED_VEC;

    return INTC_RC_OK;
} /* End of INTC_Unbind */

INTC_ReturnCode INTCUnbind(INTC_InterruptSources isrSource)
{
#ifdef _S_INTC_
    if(INTC_IS_SINTC_SOURCE(isrSource))
    {
        return SINTCUnbind(SINTC_SOURCE(isrSource));
    }
#endif

    CHECK_VALID_SOURCE(isrSource);

	// disable the source:
	INTCDisable(isrSource);

//    if (isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
	if (isrSource < INTC_SRC_VIRTUAL_END)
		return INTCUnbind_XIRQ(isrSource);


	// check if the default function is bound
    if(IS_VIA_GPIO(isrSource))
	{
        GPIO_PinNumbers gpioPin = CONVERT_INTC_TO_GPIO_PIN(isrSource);

        // if via GPIO, check whether an ISR is bound:
        /*have checked the index*/
	 	/*coverity[overrun-local]*/
        if(_interruptGPIOHandlers[gpioPin] == INTCDefaultGPIOInterruptServiceRoutine)
            return INTC_RC_NO_ISR_BOUND;

        // if via GPIO, change pin status to  not usedForInterrupt
        GPIOClearUsedForInterrupt(gpioPin);
        _interruptGPIOHandlers[gpioPin] = INTCDefaultGPIOInterruptServiceRoutine;
	}
    else
    {
        if(_interruptHandlers[isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES] == INTCDefaultInterruptServiceRoutine)
            return INTC_RC_NO_ISR_BOUND;

        _interruptHandlers[isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES] = INTCDefaultInterruptServiceRoutine;
    }

    return INTC_RC_OK;
} /* End of INTC_Unbind */


void INTCEdgeLevel_XIRQ(INTC_InterruptSources isrSource , BOOL EdgeFl);

/************************************************************************
* Function: INTCConfigure												*
*************************************************************************
* Description: Configures an interrupt source							*
*																		*
* Parameters: source - source number to be configured					*                                                                      *
*             fastNormal - FIQ (fast) or IRQ (normal)					*
*             activationType - Only on GPIO pins                        *
*             debouce - debouce state - GPIO only                       *
*																		*
* Return value:  INTC_RC_OK		                                        *
*                INTC_RC_INVALID_SRC                                    *
*																		*
* Notes: The virtual GPIO sources are configured in the INTC package    *
*        the INTC package is using GPIOConfigure                        *
************************************************************************/

INTC_ReturnCode INTCConfigure_XIRQ(INTC_InterruptSources isrSource, INTC_FastNormal fastNormal,
                              INTC_ActivationType activationType)
{
    /* AlexR : Let's this function will be empty in 3PtW*/
	switch(activationType)
	{
    	case INTC_RISING_EDGE :
    	case INTC_FALLING_EDGE:
    	case INTC_BOTH_EDGE   :
			INTCEdgeLevel_XIRQ( isrSource , TRUE);
		break;

		default:
			INTCEdgeLevel_XIRQ( isrSource , FALSE);
		break;
	}/* switch */
    return INTC_RC_OK;
} /* End of INTCConfigure */


INTC_ReturnCode INTC_P_ICU_Configure(INTC_InterruptSources isrSource, INTC_FastNormal fastNormal)
{
	    UINT32 reg, tmp;
        unsigned long cpsrReg;

        /* coverity check */
        tmp = (IS_CHIPID_HELANLTE)?P_ICU_NUM_SOURCES_HELANLTE: P_ICU_NUM_SOURCES*2;
        ASSERT(isrSource<tmp);

		if ((fastNormal != INTC_IRQ) && (fastNormal !=INTC_FIQ))
		{
			return INTC_RC_WRONG_PARAMETER;
		}

        // FIQ/IRQ - Interrupt Type Control Register
        cpsrReg = disableInterrupts();

 		if(isrSource < P_ICU_NUM_SOURCES)
			reg=InterruptController.all.ICU_CONF[isrSource];
		else 
			reg=InterruptController.all.ICU_CONF_H[isrSource - P_ICU_NUM_SOURCES];

       // ASSERT(!ICU_INT_OWNED_BY_OTHER_CORE_CONF(reg));

		// RESET THE CONF REGISTER VALUE
		reg = ICU_CONF_THIS_CORE_INT; // make sure this core owns this interrupt

		if(fastNormal == INTC_FIQ) reg|=ICU_CONF_TYPE_FIQ;
        else                       reg|=ICU_CONF_TYPE_IRQ;

		// Leave interrupt disabled, so prio is not set here

		if(isrSource < P_ICU_NUM_SOURCES)
			InterruptController.all.ICU_CONF[isrSource]=reg;
		else 
			InterruptController.all.ICU_CONF_H[isrSource - P_ICU_NUM_SOURCES]=reg;

        restoreInterrupts(cpsrReg);
		return INTC_RC_OK;
} // INTC_P_ICU_Configure


INTC_ReturnCode INTCConfigure(INTC_InterruptSources isrSource, INTC_FastNormal fastNormal,
                              INTC_ActivationType activationType)

{
    CHECK_VALID_SOURCE(isrSource);

#ifdef _TCU_ON_PRIMARY_
	if ( INTC_TCU_RANGE(isrSource) )
	{
 	    TCUPolarity ( isrSource , RISING_EDGE );
 	    TCUEnable ( isrSource );
	}
#endif

    // If it's GPIO source - GPIOConfigure should be called
#if !defined (INTC_GPIO_01_WIRED_TO_ICU)
    if( IS_VIA_GPIO(isrSource) )
#else  //!(INTC_GPIO_01_WIRED_TO_ICU)
    if( (IS_VIA_GPIO(isrSource) || isrSource==INTC_SRC_GPIO_0 || isrSource==INTC_SRC_GPIO_1))
#endif //!(INTC_GPIO_01_WIRED_TO_ICU)
    {
	ASSERT(0);
    }

    if (isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
	return INTCConfigure_XIRQ(isrSource, 
								fastNormal, 
								activationType);

	uart_printf("isrSource:%d\r\n",isrSource);

    // For interrupts connected directly to INTC
    if(isrSource >= INTC_MAX_PRIMARY_INTERRUPT_SOURCES)
    {
	   return INTC_P_ICU_Configure((INTC_InterruptSources)(isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES), fastNormal);
    }

    return INTC_RC_OK;
} /* End of INTCConfigure */


/************************************************************************
* Function: INTCEdgeLevel                                               *
*************************************************************************
* Description: Configure if the sticky bit should be asserted ,when     *
*			   the interrupt occures 									*
*																		*
* Parameters: interrupt number , edge or level flag						*
*			  In case the interrupt is edge, sticky bit will be set		*
*			  every time the interrupt occures							*
* Return value: NONE                                                    *
*											  							*
* Notes:  usefull only for H/W interrupts								*
************************************************************************/
void INTCEdgeLevel_XIRQ(INTC_InterruptSources isrSource , BOOL EdgeFl)
{
	volatile UINT32 *LvlEdgRegPtr ;
	UINT32			*LvlEdgShadowPtr ;
    UINT32 HwIndex;
	UINT32 shft ;
   /* In case the interrupt number is OK*/
   if((isrSource >= HWI0_ID ) && (isrSource < MAX_NUM_IRQ))
   {
	HwIndex =  isrSource - HWI0_ID; /* calculate the seq. number of the HW interrupt*/
	if(HwIndex < 32)
	{
	  /* First register controls H/W interrupts 0-31*/
	  LvlEdgRegPtr =  IRQ_LVL_EDG0;
	  LvlEdgShadowPtr =  &LvlEdgeShadowReg_0;
	  shft =   HwIndex;  /*Calculate the shift value for the first register*/
	}
	else
	{
  		/* Second register controls H/W interrupts 32-47*/
 	 	LvlEdgShadowPtr =  &LvlEdgeShadowReg_1;
  		LvlEdgRegPtr =  IRQ_LVL_EDG1;
  		shft =   HwIndex - 32;/*Calculate the shift value for the second register*/
	}

	/*Set or clear the corresponding bit in shadow register*/
	if(EdgeFl)
	{
	  (*LvlEdgShadowPtr) |= (1 << shft);
	}
	else
	{
	  (*LvlEdgShadowPtr) &= ~(1 << shft) ;
	}
	/* Set the real register by value in the shadow*/
	 *LvlEdgRegPtr =  *LvlEdgShadowPtr;
   } /* if isrSource*/
} /* INTCEdgeLevel */





/************************************************************************
* Function: IRQCTRL_HWI_STATE                                           *
*************************************************************************
* Description: Mask/Unmask H/W-interrupt                                *
*																		*
* Parameters:                                      						*
*			  base - start address of INTC   (0xD4000800)               *
*			  fLAG - IC_SET_ENABLE = 0x0 or IC_SET_DISAABLE = 0x40      *
*			  hWInUM - 0..47 , number of HW interrupt					*
* Return value: NONE                                                    *
*											  							*
* Notes:  usefull only for H/W interrupts								*
************************************************************************/

/*                  _________
	   2 bytes     V         HWI_ENABLE_CLR_OFFSET
  |----------------|
  |  Set Enable 0  | offset 0x20
  |----------------|
  |  Set Enable 1  | offset 0x24
  |----------------|
  |  Set Enable 2  | offset 0x28
  |----------------|
  |----------------|
  |----------------|
  |  Clr Enable 1  |  offset 0x60
  |----------------|
  |  Set Enable 2  |  offset 0x64
  |----------------|
  |  Clr Enable 2  |  offset 0x68
  |----------------|
*/
void IRQCTRL_HWI_STATE(UINT32 bASE, UINT32 fLAG, UINT32 hWInUM)
{
  volatile UINT32* hw_irq_set_clr_base_ptr = NULL;
  UINT32 RegOffset ;   /* Offset in number of pairs of Set/Clear registers from HWI_ENABLE_CLR_OFFSET */
  //UINT32 ByteOffset;
  UINT32 BitMask;
  RegOffset		=	((hWInUM) >> 4)			;/* Every Enable/Clear is responsible for 16 sources 							*/
  //ByteOffset	=	RegOffset << 2			;/* Size of register is 4 bytes                                             	*/
  BitMask   =  (1 << ((hWInUM) & 0x000F))	;/* The shift inside the set or clear byte 										*/
											 /* the bit is togle, therefore the 0 bits does not play any role */

  /* Set base address to start of array Set regs or Clr Regs*/
  switch(fLAG)
  {
	case IC_SET_ENABLE :
		hw_irq_set_clr_base_ptr = &(IntcConfigController->IC_HWI_ENABLE_REG_arr[RegOffset]);       /* Set regs*/
		break;
	case IC_CLR_ENABLE:
		hw_irq_set_clr_base_ptr = &(IntcConfigController->IC_HWI_CLR_ENABLE_REG_arr[RegOffset]);   /* Clear Regs */
		break;
	default:
		ASSERT(0) ;
		break;
  }
  
  /*can't dereference null pointer, have checked the pointer*/
  /*coverity[var_deref_op]*/	
  *(hw_irq_set_clr_base_ptr) =   BitMask   ;
  /* Save the current value of HWI Reg in DDR */
  INTC_D2_BACKUP_SET(IC_HWI_ENABLE_REG_arr[RegOffset]);

} /*IRQCTRL_HWI_STATE*/
/************************************************************************
* Function: IRQCTRL_EIRQ_STATE                                          *
*************************************************************************
* Description: Mask/Unmask EIRQ-interrupt                               *
*																		*
* Parameters:                                      						*
*			  base - start address of INTC   (0xD4000800)               *
*			  fLAG - IC_SET_ENABLE = 0x0 or IC_SET_DISAABLE = 0x40      *
*			  EirqInUM - 0..2 , number of HW interrupt					*
* Return value: NONE                                                    *
*											  							*
* Notes:  usefull only for H/W interrupts								*
************************************************************************/
void IRQCTRL_EIRQ_STATE(UINT32 bASE, UINT32 fLAG, UINT32 EirqInUM)
{
  UINT32 BitMask;
  UINT32 ClearOrSet = 0 ;     //initialized it to 0 so won't get compiler warnings about possibility of using ClearOrSet without seting a value. this value has no meaning anyway.


  //ClearOrSet  = (fLAG == IC_SET_ENABLE ) ? EIRQ_ENABLE_OFFSET :  EIRQ_CLR_OFFSET;
  /* Set base address to start of array Set regs or Clr Regs*/
  switch(fLAG)
  {
	case IC_SET_ENABLE :
		ClearOrSet = (UINT32)&(IntcConfigController->IC_EIRQ_ENABLE_REG);       /* Set regs*/
		break;
	case IC_CLR_ENABLE:
		ClearOrSet = (UINT32)&(IntcConfigController->IC_EIRQ_CLR_ENABLE_REG);   /* Clear Regs */
		break;
	default:
		ASSERT(0) ;
		break;
  }
  /* Calculate the set bitmask according to EIRQ number*/
  BitMask   =  (1 << ((EirqInUM) & 0x0003));
  /* Write to INTC HW*/
  *(volatile UINT32*)ClearOrSet =   BitMask   ;
  /* Save the current value of EIRQ enable Reg in DDR */
  INTC_D2_BACKUP_SET(IC_EIRQ_ENABLE_REG);

} /*IRQCTRL_EIRQ_STATE*/

/************************************************************************
* Function: IRQCTRL_XSWI_STATE                                          *
*************************************************************************
* Description: Mask/Unmask XSWI-interrupt                               *
*																		*
* Parameters:                                      						*
*			  base - start address of INTC   (0xD4000800)               *
*			  fLAG - IC_SET_ENABLE = 0x0 or IC_SET_DISAABLE = 0x40      *
*			  XswiInUM - temporary always 1         					*
* Return value: NONE                                                    *
*											  							*
* Notes:  usefull only for XSWI interrupts								*
************************************************************************/
/*                  _________
	   2 bytes     V         XSWI_ENABLE_OFFSET
  |---------------------|
  |  XSWI Set Enable 0  |  offset 0x08
  |---------------------|
  |---------------------|
  |---------------------|
  |  XSWI Clr Enable 0  |  offset 0x48
  |---------------------|
*/
void IRQCTRL_XSWI_STATE(UINT32 bASE, UINT32 fLAG, UINT32 XswiInUM)
{
  UINT32 BitMask;
  BitMask   =  (1 << ((XswiInUM) & 0x0001));
  /* Write of set bit will couse set ( clear) of the corresponding bit for Enable ( Clear) source*/
  switch(fLAG)
  {
	case IC_SET_ENABLE :
		IntcConfigController->IC_XSWI_ENABLE_REG |= BitMask;       /* Set regs*/
		break;
	case IC_CLR_ENABLE:
		IntcConfigController->IC_XSWI_ENABLE_REG &= ~(BitMask);   /* Clear Regs */
		break;
	default:
		ASSERT(0) ;
		break;
  }

  /* Save the current value of XSWI enable Reg in DDR */
  INTC_D2_BACKUP_SET(IC_XSWI_ENABLE_REG);
} /*IRQCTRL_XSWI_STATE*/

/************************************************************************
* Function: IRQCTRL_RI_STATE                                            *
*************************************************************************
* Description: Mask/Unmask RI-interrupt                                 *
*																		*
* Parameters:                                      						*
*			  base - start address of INTC   (0xD4000800)               *
*			  fLAG - IC_SET_ENABLE = 0x0 or IC_SET_DISAABLE = 0x40      *
*			  RiInUM - 0..7                         					*
* Return value: NONE                                                    *
*											  							*
* Notes:  usefull only for RI interrupts								*
************************************************************************/
/*                  _________
	   2 bytes     V         RI_ENABLE_CLR_OFFSET
  |---------------------|
  |    RI Set Enable 0  |  offset 0x04
  |---------------------|
  |---------------------|
  |    RI Clr Enable 0  |  offset 0x44
  |---------------------|
*/
void IRQCTRL_RI_STATE(UINT32 bASE, UINT32 fLAG, UINT32 RiInUM)
{
  UINT32 BitMask;
  UINT32 ClearOrSet = 0 ;     //initialized it to 0 so won't get compiler warnings about possibility of using ClearOrSet without seting a value. this value has no meaning anyway.

  BitMask   =  (1 << ((RiInUM) & 0x0007));
  /* Set base address to start of array Set regs or Clr Regs*/
  switch(fLAG)
  {
	case IC_SET_ENABLE :
		ClearOrSet = (UINT32)&(IntcConfigController->IC_RI_ENABLE_REG);       /* Set regs*/
		break;
	case IC_CLR_ENABLE:
		ClearOrSet = (UINT32)&(IntcConfigController->IC_RI_CLR_ENABLE_REG);   /* Clear Regs */
		break;
	default:
		ASSERT(0) ;
		break;
  }

  *(volatile UINT32*)ClearOrSet =   BitMask   ;

  /* Save the current value of RI enable Reg in DDR */
  INTC_D2_BACKUP_SET(IC_RI_ENABLE_REG);

}/*IRQCTRL_RI_STATE*/

/************************************************************************
* Function: IRQCTRL_SWI_STATE                                            *
*************************************************************************
* Description: Mask/Unmask SW-interrupt (TCU)                           *
*																		*
* Parameters:                                      						*
*			  base - start address of INTC   (0xD4000800)               *
*			  fLAG - IC_SET_ENABLE = 0x0 or IC_SET_DISAABLE = 0x40      *
*			  SwInUM - 0..15                         					*
* Return value: NONE                                                    *
*											  							*
* Notes:  usefull only for SWI interrupts								*
************************************************************************/
/*                  _________
	   2 bytes     V         SWI_ENABLE_CLR_OFFSET
  |---------------------|
  |    SWI Set Enable 0 |  offset 0x00
  |---------------------|
  |    SWI Clr Enable 0 |  offset 0x40
  |---------------------|
*/
void IRQCTRL_SWI_STATE(UINT32 bASE, UINT32 fLAG, UINT32 SwInUM)
{
  UINT32 BitMask;
  UINT32 ClearOrSet = 0 ;     //initialized it to 0 so won't get compiler warnings about possibility of using ClearOrSet without seting a value. this value has no meaning anyway.;

  BitMask   =  (1 << ((SwInUM) & 0x000f));
  /* Set base address to start of array Set regs or Clr Regs*/
  switch(fLAG)
  {
	case IC_SET_ENABLE :
		ClearOrSet = (UINT32)&(IntcConfigController->IC_SWI_ENABLE_REG);       /* Set regs*/
		break;
	case IC_CLR_ENABLE:
		ClearOrSet = (UINT32)&(IntcConfigController->IC_SWI_CLR_ENABLE_REG);   /* Clear Regs */
		break;
	default:
		ASSERT(0) ;
		break;
  }

  *(volatile UINT32*)ClearOrSet =   BitMask   ;
  /* Save the current value of RI enable Reg in DDR */
  INTC_D2_BACKUP_SET(IC_SWI_ENABLE_REG);

} /*  IRQCTRL_SWI_STATE */




/************************************************************************
* Function: INTCInterruptHandlerIRQ										*
*************************************************************************
* Description: This function handles the FAST interrupt sources.		*
*              In this function, there is a loop over the request		*
*              register as long as there are pending interrupts			*
*																		*
* Parameters: None														*
*																		*
* Return value: None													*
*																		*
* Notes: Clearing the pending interrupt source must be done in the ISR,	*
*		 and is the source's responsibility!!!							*
************************************************************************/
//extern void check_rx_workaround(void);
extern UINT64 timerCountReadU64(void);


UINT32 get_int_status_0(void)
{
	return InterruptController.all.ICU_INT_STATUS_0;

}

UINT32 get_int_status_1(void)
{
	return InterruptController.all.ICU_INT_STATUS_1;

}

UINT32 get_int_status_2(void)
{
	return InterruptController.all.ICU_INT_STATUS_2;

}

UINT32 get_int_status_3(void)
{
	return InterruptController.all.ICU_INT_STATUS_3;

}

UINT64 intc_tick;

void INTCInterruptHandlerIRQ(INTC_InterruptInfo info)
{
	UINT64 tick;

	tick = timerCountReadU64();//timerCountRead(TS_TIMER_ID);
	//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	UINT32 irqPending;
    UINT32 source;
	//[klockwork][issue id: 1627]
	UINT64 cycles = 0;

	// loop for interrupt sources
	while( (irqPending = InterruptController.own.ICU_IRQ_PENDING) & ICU_PENDING_INT_VALID_BITS_CRANE)	
	{
		// identify the SOURCE interrupt
		source = irqPending & ICU_PENDING_INT_NUM_BITS_CRANE;
	
		//if(source != 0)
		//	rti_icu_switch_to(source);
		(_interruptHandlers[source])(source + INTC_MAX_PRIMARY_INTERRUPT_SOURCES);
				
		//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	}
 	/* interrupt tick statistics */
	tick = timerCountReadU64() - tick;
	intc_tick += tick;
}/* End of INTCInterruptHandlerIRQ */




/*******************************************************************************
 *
 * @Function:    INTCPhase1Init @
 *
 * @Interface:   PLK/M@
 *
 * @Parameters:  None @
 *
 * @Returns:     Nothing @
 *
 * @Description: Init the interrupt vector lookkup table.
 *				 From this point, INTCBind can be used.
 *				 This will bind the interrupt, but interrupts will not be
 *				 enabled until the end of DLMicInitialise
 *
 *******************************************************************************/
INTC_ReturnCode INTCPhase1Init_XIRQ (void)
{
	Int16 i;

	dlDisableIrq();
	uart_printf("AIRQ_EDGE_OR_LEVEL0=%lx,INTC_SYS_REG_REG16=%lx\n",AIRQ_EDGE_OR_LEVEL0,INTC_SYS_REG_ADDR);
    /* IRQ Reset flag must be cleared for normal operation*/
    INTC_SYS_REG_SET16(0);
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
/**/

	WR_REG32(AIRQ_EDGE_OR_LEVEL0, 0x0UL, 0xFFFFFFFF);
	WR_REG32(AIRQ_EDGE_OR_LEVEL32, 0x0UL, 0xFFFFFFFF);

	/* Clear all Enable Mask Registers by writing 1 in all clear bits*/
  	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_SWI_CLR_ENABLE, 0xffffL); /* clear 15 SWI interrupts */
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_REG_CLR_ENABLE, 0x00ffL); /* clear 8  RI  interrupts */
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_EIRQ_CLR_ENABLE,0x0007L); /* clear 1  EIRQ  interrupts */
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_HWI0_CLR_ENABLE, 0xffffL); /* clear 16 HW interrupts 0-15*/
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_HWI1_CLR_ENABLE, 0xffffL); /* clear 16 HW interrupts 16-31*/
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_HWI2_CLR_ENABLE, 0xffffL); /* clear 16 HW interrupts 32-47*/
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);


	for (i=0; i<IRQ_VECT_SIZE; i++)
	{
		irqVectorLookup[i] = UNHANDLED_VEC;
	}

	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

	return(INTC_RC_OK);
} /* INTCPhase1Init */

INTC_ReturnCode INTCPhase1Init(void)
{
    int i;
	/*Fix coverity[uninit_use]*/
	INTC_ReturnCode rc = INTC_RC_OK;
	

	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

	XIRQ_init();  //crane
		
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	INTCPhase1Init_XIRQ();
		
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);


    for (i=0;i<P_ICU_NUM_SOURCES;i++)
        InterruptController.all.ICU_CONF[i]=0x0; //clear interrupt config regs

    maxInterruptControllerSources = MAX_INTERRUPT_CONTROLLER_SOURCES_MACRO;
    // Initialize the interrupt handlres array
    for(i=0; i<maxInterruptControllerSources; i++)
    {
        _interruptHandlers[i] = INTCDefaultInterruptServiceRoutine;
    }


    rc=INTC_P_ICU_Init();
    ASSERT(rc==INTC_RC_OK);
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

    return rc;
} /* End of INTCPhase1Init */



/************************************************************************
* Function: INTCDisableInterruptOutput 									*
*************************************************************************
* Description: Disables the output of interrupt controller towards core	*
*																		*
*																		*
* Parameters: None														*
*																		*
* Return value:  None			                                        *
************************************************************************/

extern void XIRQClockOnOff(CLK_OnOff OnOff);
void XIRQ_init(void)
{
   volatile UINT32 *reg;
   UINT8 temp;
   //pei add
   XIRQClockOnOff(CLK_ON);
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	//uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	uart_printf("IRQCTRL_BASE_ADDR=%lx,IRQ_STICKY_CLR0=%lx\n",IRQCTRL_BASE_ADDR,IRQ_STICKY_CLR0);

	reg = (volatile UINT32 *)(IRQCTRL_BASE_ADDR + 0x8c); //XIRQ system reset reg.
   	*reg = 0x01;

	temp = 0x10;
	while(temp--);

	*reg = 0;
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

	reg = (volatile UINT32 *)(IRQCTRL_BASE_ADDR + 0x468);
	*reg = 0x03;
/**/
	reg = (volatile UINT32 *)(IRQ_STICKY_CLR0);	 
	*reg = 0xFFFFFFFF;
	reg = (volatile UINT32 *)(IRQ_STICKY_CLR16);	 
	*reg = 0xFFFFFFFF;
	//reg = (volatile UINT32 *)(IRQ_STICKY_CLR2);  
	//*reg = 0xFFFFFFFF;
	
   uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);

}
UINT32 debug_intc_count = 0;

void INTCInterruptHandlerIRQ_XIRQ(INTC_InterruptInfo info)
{
    UINT16 status;
    UINT16 cin;
	//[klockwork][issue id: 1626]
	UINT64 cycles= 0;


	UINT32 frint_flag = 0;
	debug_intc_count = 0;


   status = RD_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_STATUS_CTRL);

   while (!(status & 0x2UL)) //re-read at the end of while()
   {
	   cin = RD_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_NUM);
	   
	    ASSERT(cin < MAX_NUM_IRQ);
		
	if ((cin == SW_INT_SRC_4) && (frint_flag == 0)) 
	{
		frint_flag = 1;
	}
	else if((cin == SW_INT_SRC_4) && (frint_flag == 1)) 
	{
		WR_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_NUM, cin);
        status = RD_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_STATUS_CTRL);
		continue;
	}

		//rti_xirq_switch_to(cin);
        (irqVectorLookup[cin])((INTC_InterruptInfo)(cin+INTC_SRC_S_START));
		
		/* At the end Cur interrupt number must be written back
	     * It will cause the IRQ module to look up the corresponding priority register value
         * and reset the corrsponding group bit in the ActiveIntMask Register
	    */
  	    WR_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_NUM, cin);
        status = RD_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_STATUS_CTRL);
   }/* while loop*/

}/* End of INTCInterruptHandlerIRQ */



void INTCD2rmD2Register(void)
{

}

/************************************************************************
* Function: INTCPhase2Init                                              *
*************************************************************************
* Description: Phase 2 of the initializes process of the INTC package   *
*																		*
* Parameters: NONE														*
*																		*
* Return value: NONE                                                    *
*																		*
* Notes:																*
************************************************************************/
void INTCPhase2Init_XIRQ(void)
{
    // Enable all the 'combined' GPIO's because they are disabled from the GPIO package
   //enableInterrupts();

   //XIRQ_init();
   uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
   
   INTCConfigure(INTC_SRC_ICU_AIRQ, INTC_IRQ, INTC_RISING_EDGE);
   INTCBind ( INTC_SRC_ICU_AIRQ , INTCInterruptHandlerIRQ_XIRQ );
   INTCEnable(INTC_SRC_ICU_AIRQ);

   uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	INTCD2rmD2Register();
} /* End of INTCPhase2Init */

void INTCPhase2Init(void)
{
	INTCPhase2Init_XIRQ();
} 

/************************************************************************
* Function: INTCClrSrc,INTCClear                                        *
*************************************************************************
* Description: Clears steaky bit of the edge interrupt                  *
*																		*
* Parameters: interrupt number											*
*																		*
* Return value: NONE                                                    *
*																		*
* Notes:																*
************************************************************************/
INTC_ReturnCode INTCClear_XIRQ(INTC_InterruptSources isrSource)
{
   UINT32 HwIndex;
   UINT32 BaseAddr;
   UINT32 WrVal;
     
   if((isrSource >= HWI0_ID ) && (isrSource < MAX_NUM_IRQ))
   {
	HwIndex =  isrSource - HWI0_ID;
   } /* isrSource */
   else
   {
		/* Non-relevant for non-HW*/
		return(INTC_RC_OK);
   }

   	if(HwIndex < 32)
   	{
      WrVal   = (1 << HwIndex);
	  BaseAddr  =  IRQ_STICKY_CLR0 ;
   	}

	else
	{
		WrVal =	(1<<(HwIndex-32));
		BaseAddr	=  IRQ_STICKY_CLR16 ;
	}

   WR_REG32(BaseAddr,0, WrVal);
   return(INTC_RC_OK);
}/*  INTCClrSrc */

void GPIOInterruptClear(GPIO_PinNumbers gpioPin) {}


INTC_ReturnCode INTCClear(INTC_InterruptSources isrSource)
{
	//UINT32 reg, place, mask;

	// check whether the source is GPIO edge driven:
    if(IS_VIA_GPIO(isrSource))
    {
        GPIO_PinNumbers gpioPin;

        // convert the INTC source into GPIO pin number:
        gpioPin = CONVERT_INTC_TO_GPIO_PIN(isrSource);
        GPIOInterruptClear(gpioPin);

        // get the pin's register number and its place in the register
        /*reg = pin >> GPIO_SHIFT_PINS_PER_REG;
        place = pin % GPIO_PINS_PER_REG;

        mask = CONVERT_NUMBER_TO_32BIT_MASK(place);

        GPIOClearEdgeDetectionStatusReg(reg, mask);*/
    }

    if (isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
		return INTCClear_XIRQ(isrSource);

	return INTC_RC_OK;

}/* End of INTCClear */




#if 0

#ifdef PHS_SW_DEMO_TTC   


#endif

/* Definitions*/
//----------------------------------------------------------------------------------------------------------------------------

//void (*INTC_ISR)(INTC_InterruptInfo interruptInfo)
// declaration for registering with PRM
void	INTCD2Prepare(PM_PowerStatesE statetoprepare);
void	INTCD2Recover( PM_PowerStatesE stateexited, BOOL b_DDR_ready, BOOL b_RegsRetainedState);
void	INTCD2rmD2Register(void);


/* Private functions */
/*static*/ void INTCEdgeLevel(INTC_InterruptSources isrSource , BOOL EdgeFl);
/*Some L1 still uses this API*/


/////////////////////////// 20080824 temp ////////////////////////////////////

void GPIOInterruptClear(GPIO_PinNumbers gpioPin) {}


void PlkTcuExceptionHandler ( UINT32 interruptInfo );

////////////////////////////////////////////////////////////////////////////





//static INTC_InterruptInfo Unhandled_Num ;
static void Unhandled_ISR (INTC_InterruptInfo interruptInfo)
{
    ASSERT(0);
	INTCClear((INTC_InterruptSources)interruptInfo);
	//Unhandled_Num =  interruptInfo;
}
//#define UNHANDLED_VEC   (INTC_ISR)0
#define UNHANDLED_VEC   Unhandled_ISR

#if defined(ARIQ_MASK_INTERRUPT_BUG_WORKAROUND)
#if defined(ARIQ_MASK_INTERRUPT_BUG_WORKAROUND_TRACE)
extern AIMBugTraceStruct AIMTrace;
#endif
volatile UINT16 rdForTest;
#endif


UINT32 INTC_Counter			= 0 ; /* Global interrupt counter */
//static UINT32 EntryToggle	= 0 ; /* Entry-exit interrupt flag*/
extern UINT32 LvlEdgeShadowReg_0; /* Shadow register of WrOnly Edge/Level register-0 */
extern UINT32 LvlEdgeShadowReg_1; /* Shadow register of WrOnly Edge/Level register-1 */
#if defined (EDEN_1928) || defined (NEZHA3_1826)
extern UINT32 LvlEdgeShadowReg_2; /* Shadow register of WrOnly Edge/Level register-2 */
#endif
/* Local functions header */

/* Local variables to module */

extern INTC_emulation_priority_table_S INTC_emulation_priority_table[];


UINT32 Get_ICU_INTC_STATUS_0(void)
{
	return (IS_CHIPID_HELANLTE)?(InterruptController_HelanLTE.all.ICU_INT_STATUS_0):(InterruptController.all.ICU_INT_STATUS_0);
}

UINT32 Get_ICU_INTC_STATUS_1(void)
{
	return (IS_CHIPID_HELANLTE)?(InterruptController_HelanLTE.all.ICU_INT_STATUS_1):(InterruptController.all.ICU_INT_STATUS_1);
}




INTC_ReturnCode INTCIdleMaskEnable(void)
{
    if (IS_CHIPID_HELANLTE)
    	InterruptController_HelanLTE.own.ICU_GLOBAL_MASK = ICU_GLOBAL_MASK_ALL;
    else
        InterruptController.own.ICU_GLOBAL_MASK = ICU_GLOBAL_MASK_ALL;
    return INTC_RC_OK;
} /* End of INTCIdleMaskEnable */

INTC_ReturnCode INTCDisableAllInts(void)
{
	int i;

	if (IS_CHIPID_HELANLTE)
	{
    	for(i=0;i<P_ICU_NUM_SOURCES_HELANLTE;i++)
    	{
         if(ICU_INT_OWNED_BY_CORE_HELANLTE(i)) ICU_MASK_INT_HELANLTE(i);
    	}
	}
	else
	{
    	for(i=0;i<P_ICU_NUM_SOURCES;i++)
    	{
         if(ICU_INT_OWNED_BY_CORE(i)) ICU_MASK_INT(i)
    	}
	}
	return(INTC_RC_OK);
}






INTC_ReturnCode INTC_P_ICU_ConfigurationGet(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal)
{
	    UINT32 reg, tmp;

        /* coverity check */
        tmp = (IS_CHIPID_HELANLTE)?P_ICU_NUM_SOURCES_HELANLTE: P_ICU_NUM_SOURCES*2;
        ASSERT(isrSource<tmp);

 		if(isrSource < P_ICU_NUM_SOURCES)
			reg=InterruptController.all.ICU_CONF[isrSource];
		else 
			reg=InterruptController.all.ICU_CONF_H[isrSource -P_ICU_NUM_SOURCES];

        if(!ICU_INT_OWNED_BY_OTHER_CORE_CONF(reg)) return INTC_RC_NOT_AVAILABLE; // not configured

        if((reg&ICU_CONF_TYPE_IRQ)==ICU_CONF_TYPE_IRQ)
            *fastNormal = INTC_IRQ;
        else
            *fastNormal = INTC_FIQ;
		return INTC_RC_OK;
}

static UINT32 gINT = 0;
static UINT32 gXINT = 0;


#ifdef PHS_SW_DEMO_TTC // 20080905
#include "timer.h"
extern void BBU_putstr(char *str);
extern void BBU_puthexd(UINT32);
extern void setGPIO84_HIGH(void);
extern void setGPIO84_LOW(void);
extern UINT32 timerCountRead(TCR_NUMBER tcr_number);
extern void rti_xirq_switch_to(unsigned long isrSource);
extern unsigned long isEnable_performance_count(void);
extern void rti_icu_switch_to(unsigned long isrSource);

void XIRQ_init(void);
#endif

#ifdef PHS_SW_DEMO_TTC
UINT32 debug_intc_num[32];

UINT32 debug_intc_time[32];

typedef struct 
{
	UINT64 cycles;
	UINT32 count;
}INTC_PERFORMANCE_DATA;

extern unsigned long long get_performance_count(void);

#endif







/************************************************************************
* Function: INTCInterruptHandlerFIQ										*
*************************************************************************
* Description: This function handles the FAST interrupt sources.		*
*              In this function, there is a loop over the request		*
*              register as long as there are pending interrupts			*
*																		*
* Parameters: None														*
*																		*
* Return value: None													*
*																		*
* Notes: Clearing the pending interrupt source must be done in the ISR,	*
*		 and is the source's responsibility!!!							*
************************************************************************/
void INTCInterruptHandlerFIQ_XIRQ(void)
{
//	UINT32 fiqPending;
//    UINT32 source;
//
//	// loop for interrupt sources
//#if defined (INTC_USE_APB_ACCESS_ONLY)
//    while( (fiqPending = InterruptController.ICHP) & ICHP_FIQ_VALID_BIT /*ICHP_VALID_BITS*/ )
//#else
//	while( (fiqPending = cp6ReadICHP())            & ICHP_FIQ_VALID_BIT /*ICHP_VALID_BITS*/ )
//#endif //(INTC_USE_APB_ACCESS_ONLY)
//	{
//        // valuate the SOURCE interrupt
//        source = (fiqPending >> FIQ_PENDING_SHIFT) & INTERRUPT_NUM_MASK;
//
//        // call interrupt service routine
//        (_interruptHandlers[source])(source);
//	}
}/* End of INTCInterruptHandlerFIQ */

void INTCInterruptHandlerFIQ(void)
{
	UINT32 fiqPending;
    UINT32 source;

	// loop for interrupt sources
    if (IS_CHIPID_HELANLTE)
    {	
        while( (fiqPending = InterruptController_HelanLTE.own.ICU_FIQ_PENDING) & ICU_PENDING_INT_VALID_BITS_HELANLTE)
    	{
            // identify the SOURCE interrupt
            source = fiqPending & ICU_PENDING_INT_NUM_BITS_HELANLTE;

            // call interrupt service routine
            //(_interruptHandlers[source])(source);
            (_interruptHandlers[source])(source + INTC_MAX_PRIMARY_INTERRUPT_SOURCES);
    	}
	}
	else
	{
        while( (fiqPending = InterruptController.own.ICU_FIQ_PENDING) & ICU_PENDING_INT_VALID_BITS)
        {
        // identify the SOURCE interrupt
        source = fiqPending & ICU_PENDING_INT_NUM_BITS;

            // call interrupt service routine
            //(_interruptHandlers[source])(source);
            (_interruptHandlers[source])(source + INTC_MAX_PRIMARY_INTERRUPT_SOURCES);
        }
	}
}





INTC_ReturnCode INTCISRGet_XIRQ(INTC_InterruptSources isrSource, INTC_ISR *isr)
{
    /* Check NULL pointer */
    if(isr == NULL) return INTC_RC_NULL_POINTER;
    *isr = NULL;

	if (UNHANDLED_VEC == irqVectorLookup[isrSource])
		return INTC_RC_NO_ISR_BOUND;

    *isr = irqVectorLookup[isrSource];
    
	return INTC_RC_OK;	
}

INTC_ReturnCode INTCISRGet(INTC_InterruptSources isrSource, INTC_ISR *isr)
{
#ifdef _S_INTC_
    if(INTC_IS_SINTC_SOURCE(isrSource))
    {
        return SINTCISRGet(SINTC_SOURCE(isrSource), isr);
    }
#endif
    CHECK_VALID_SOURCE(isrSource);

    /* Check NULL pointer */
    if(isr == NULL)
    {
        return INTC_RC_NULL_POINTER;
    }

#ifdef PHS_SW_DEMO_TTC  //20080826
    //if (isrSource < MAX_INTERRUPT_CONTROLLER_SOURCES)
    if (isrSource < INTC_SRC_VIRTUAL_END)
		return INTCISRGet_XIRQ(isrSource, isr);
#endif

    *isr = NULL;
	
	// check if the default function is bound
    if(IS_VIA_GPIO(isrSource))
	{
        GPIO_PinNumbers gpioPin = CONVERT_INTC_TO_GPIO_PIN(isrSource);

        // if via GPIO, change pin status to usedForInterrupt
        /*have checked the index*/
	 	/*coverity[overrun-local]*/
        if(_interruptGPIOHandlers[gpioPin] == INTCDefaultGPIOInterruptServiceRoutine)
            return INTC_RC_NO_ISR_BOUND;

        // if via GPIO, change pin status to  not usedForInterrupt
        *isr = _interruptGPIOHandlers[gpioPin];
	}
    else
    {
        if(_interruptHandlers[isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES] == INTCDefaultInterruptServiceRoutine)
            return INTC_RC_NO_ISR_BOUND;

        *isr = _interruptHandlers[isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES];
    }

    return INTC_RC_OK;
} /* End of INTCISRGet */


#if defined(INTC_CONFIGURE_WITH_3_ARGUMENTS)
INTC_ReturnCode INTCConfigurationGet_XIRQ(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal,
                                     INTC_ActivationType *activationType)
#else
INTC_ReturnCode INTCConfigurationGet_XIRQ(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal,
                                     INTC_ActivationType *activationType, GPIO_Debounce *debouce)
#endif
{
	return INTC_RC_OK;
}

#if defined(INTC_CONFIGURE_WITH_3_ARGUMENTS)
INTC_ReturnCode INTCConfigurationGet(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal,
                                     INTC_ActivationType *activationType)
#else
INTC_ReturnCode INTCConfigurationGet(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal,
                                     INTC_ActivationType *activationType, GPIO_Debounce *debouce)
#endif
{
    unsigned long cpsrReg;
	INTC_ReturnCode rc = INTC_RC_OK;
	// Init values:
#if !defined(INTC_CONFIGURE_WITH_3_ARGUMENTS)	//PHS_SW_DEMO_TTC
	*debounce=GPIO_DEBOUNCE_DISABLE;
#endif
    *activationType = INTC_RISING_EDGE;
	*fastNormal = INTC_IRQ;	// Default IRQ - good for GPIO and combined...

#ifdef _S_INTC_
    if(INTC_IS_SINTC_SOURCE(isrSource))
    {
        return SINTCConfigurationGet(SINTC_SOURCE(isrSource),fastNormal,activationType);
    }
#endif
    CHECK_VALID_SOURCE(isrSource);

    cpsrReg = disableInterrupts();

    if (isrSource  < MAX_INTERRUPT_CONTROLLER_SOURCES)
	{
#if defined(INTC_CONFIGURE_WITH_3_ARGUMENTS)
	 	rc = INTCConfigurationGet_XIRQ(isrSource,
	 								fastNormal, 
	 								activationType);
#else
	 	rc = INTCConfigurationGet_XIRQ(isrSource,
	 								fastNormal, 
	 								activationType,
	 								debounce);
#endif
		restoreInterrupts(cpsrReg);

    		return rc;
    	}
		
    // For a regular interrupt source or GPIO_0 & GPIO_1
    if(isrSource >= INTC_MAX_PRIMARY_INTERRUPT_SOURCES)
    {
		rc=INTC_P_ICU_ConfigurationGet((INTC_InterruptSources)(isrSource - INTC_MAX_PRIMARY_INTERRUPT_SOURCES),fastNormal);
    }

    restoreInterrupts(cpsrReg);

    return rc;
} /* End of INTCGetConfigure */


/************************************************************************
* Function: INTCConfigureSwi 											*
*************************************************************************
* Description: Configures the SW interrupt								*
*																		*
* Parameters: source - source number to be configured					*                                                                      *
*             fastNormal - FIQ (fast) or IRQ (normal)					*
*             activationType - Only on GPIO pins                        *
*             debouce - debouce state - GPIO only                       *
*																		*
* Return value:  INTC_RC_OK		                                        *
*                INTC_RC_INVALID_SRC                                    *
*																		*
* Notes: The virtual GPIO sources are configured in the INTC package    *
*        the INTC package is using GPIOConfigure                        *
************************************************************************/
INTC_ReturnCode INTCConfigureSwi(XIRQ_InterruptSources isrSource, INTC_FastNormal fastNormal,
                              INTC_ActivationType activationType,UINT8 priority){
	 UINT32 IntAbsSrc=isrSource;

//#ifdef PHS_SW_DEMO_TTC  //20080826
//   if (isrSource >= FirstRI_INT)
//	return INTC_RC_INVALID_SRC;   //if this is a ICU source, it should not a SWI source.
//#endif
	 INTC_emulation_priority_table[IntAbsSrc].InterruptPriority = priority;

	//PHS_SW_DEMO_TTC comments: 
	//isrSource subtract INTC_MAX_PRIMARY_INTERRUPT_SOURCES is done in INTCConfigure.
	 INTCConfigure(isrSource,fastNormal,activationType);

	 return INTC_RC_OK;

}















/************************************************************************
* Function: INTCSwIntCrossAssert                                            *
*************************************************************************
* Description: Handles the received cross-assert interrupt              *
*																		*
* Parameters:                                      						*
*			  isrSource - in range of the virtual interrupts numbers    *
*             From  MAX_NUM_IRQ   = (NUM_SWI + NUM_RI + NUM_XSWI + NUM_EIRQ + MAX_NUM_HWI)*
*             to  IRQ_VECT_SIZE =  (MAX_NUM_IRQ + NUM_OF_VIRTUAL_INT)   *
* Return value:  INTC_RC_OK												*
*                INTC_RC_INVALID_SRC									*
*                INTC_RC_NO_ISR_BOUND									*
************************************************************************/
INTC_ReturnCode INTCSwIntCrossAssert(INTC_InterruptSources isrSource)
{
   INTC_ReturnCode ret_code;
   ret_code = INTC_RC_OK;

    if (isrSource >= MAX_INTERRUPT_CONTROLLER_SOURCES)
		return INTC_RC_INVALID_SRC;
    //else
   //	isrSource -= INTC_MAX_PRIMARY_INTERRUPT_SOURCES;
   if((isrSource >= MAX_NUM_IRQ) && (isrSource < IRQ_VECT_SIZE ))  /* If this is in range of the virtual interrupts*/
   {
		if (irqVectorLookup[isrSource] != UNHANDLED_VEC)
		{
			(irqVectorLookup[isrSource])(isrSource);
		}
		else
		{
			ret_code = INTC_RC_NO_ISR_BOUND;
		}

   }
   else
   {
		ret_code = INTC_RC_INVALID_SRC ;
   }

   return ret_code;
}/* INTCSwIntCrossAssert */


/************************************************************************
* Function: INTCXswiAssert                                              *
*************************************************************************
* Description: Triggers cross-assert (called from L1)                   *
*																		*
* Parameters:                                      						*
*			  prio - priority                                           *
* Return value:  None												    *
************************************************************************/
void INTCXswiAssert(UINT16 prio){
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_XSWI_PRI,prio);
    WR_IRQCTRL_REG(IRQCTRL_MCU, IC_XSWI_ASSERT, 1);
}/*INTCXswiAssert*/



#if defined(XIRQ_D2_SPECIFIC)
void INTCD2rmD2Register(void)
{
	PRMRegisterForNonRetainState(PRM_NONRETAINED_SRVC_INTC,INTCD2Prepare,INTCD2Recover);
}

/************************************************************************
* Function: INTCD2Prepare                                            *
*************************************************************************
* Description: CallBack function , called by Resource Manager           *
*			   during the entry into D2 power state						*
* Parameters:                                      						*
************************************************************************/
void  INTCD2Prepare(PM_PowerStatesE statetoprepare)
{
}/*INTCD2Prepare*/
#else
void INTCD2rmD2Register(void)
{}
#endif






UINT32 INTCIsInterruptPending(void)
{
	return ((InterruptController.own.ICU_IRQ_PENDING) & ICU_PENDING_INT_VALID_BITS_CRANE) ? 0x1:0x0;
}


#ifdef TCU_UNIT_TEST

Int16  AirqIsrNum = 0;
Int16  tcuexcIsrNum = 0;
void AirqIsr(void)
{
	volatile int   cin;
	
	cin = RD_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_NUM);
	
	//uart_printf("AIRQ interrupt received cin=%x \n",cin);
	WR_IRQCTRL_REG(IRQCTRL_MCU, IC_IRQ_NUM, cin);

    AirqIsrNum += (Int16)cin;
    //HslLogTest((Int16)LG_L1_DEVCHECK, 0xeeee, 0xeeee, M_TcuDrvStatus());
    //HslLogTest((Int16)0x913, 0xeeee, 0xeeee, (Int16)cin);

    
    if(cin == 44)
    {
        tcuexcIsrNum++;

        PlkTcuExceptionHandler(cin);
    }
    

    INTCClrSrc((INTC_InterruptSources)cin);


}


#if defined (SINTC_XIRQ)  // XIRQ is secondary (starts from non-0 interrupt ID)
#define XIRQ_TO_INTC(iNTnUM) (INTC_InterruptSources)(iNTnUM+INTC_SRC_S_START)
#else                     // XIRQ is primary
#define XIRQ_TO_INTC(iNTnUM) (INTC_InterruptSources)(iNTnUM)
#endif // SINTC_XIRQ - secondary XIRQ

void AirqInterruptInit(void)
{
    INTC_ReturnCode INTCStatus;
	Int16 i;
	int isrNum;
	isrNum = INTC_SRC_ICU_AIRQ;
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	INTCStatus = INTCConfigure((INTC_InterruptSources)isrNum, INTC_IRQ, INTC_RISING_EDGE);
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	ASSERT(INTCStatus==INTC_RC_OK);
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	INTCBind((INTC_InterruptSources)isrNum, (INTC_ISR)AirqIsr);
	//INTCStatus = INTCBind((INTC_InterruptSources)isrNum, (INTC_ISR)INTCInterruptHandlerIRQ_XIRQ);
    
	ASSERT(INTCStatus==INTC_RC_OK);
    
	uart_printf("file:%s,function:%s,line:%d\r\n", __FILE__,__func__,__LINE__);
	INTCEnable(INTC_SRC_ICU_AIRQ);
	{
		for(i=0;i<FIRST_HW_INT;i++)
		{
		INTCStatus=INTCConfigureSwi( (XIRQ_InterruptSources)(i),
									INTC_IRQ,
									INTC_RISING_EDGE,
									1 );	
			INTCStatus = INTCEnable( XIRQ_TO_INTC(i) );
			}
	}

    /**enable TCU excetipn irq*/
    {

        INTCStatus=INTCConfigureSwi( (44),
                                INTC_IRQ,
                                INTC_RISING_EDGE,
                                1 );   

        //XIRQ_TO_INTC(INTC_HW_TCU_EXCP)
        INTCStatus = INTCEnable( 44);
        INTCStatus = INTCBind( 44, (INTC_ISR)PlkTcuExceptionHandler );
        ASSERT(INTCStatus==INTC_RC_OK);
    }

}
#endif
#endif


