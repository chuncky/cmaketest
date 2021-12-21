/*--------------------------------------------------------------------------------------------------------------------
(C) Copyright 2006, 2007 Marvell DSPC Ltd. All Rights Reserved.
-------------------------------------------------------------------------------------------------------------------*/

/************************************************************************/
/*                                                                      */
/* Title: Interrupt Controller Header File                              */
/*                                                                      */
/* Filename: Intc.h                                                     */
/*                                                                      */
/* Author: Yossi Gabay                                                  */
/*         Eilam Ben-Dror                                               */
/*                                                                      */
/* Target, subsystem: Common Platform, HAL                              */
/************************************************************************/
#ifndef _INTC_API_H_
#define _INTC_API_H_

#include "plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-- Type definitions (T type) -----------------------------------------*/
// Return code of INTC package operations
typedef enum
{
    INTC_RC_OK = 1,

    INTC_RC_INVALID_SRC = -100,
	INTC_RC_NOT_AVAILABLE,
	INTC_RC_NO_ISR_BOUND,
    INTC_RC_ISR_ALREADY_BOUND,
    INTC_RC_BAD_ISR_PARAMETER,
    INTC_RC_NULL_POINTER,
    INTC_RC_PRIORITY_TABLE_ERROR,
	INTC_RC_WRONG_PARAMETER		// cover all cases of wrong configuration setting
}INTC_ReturnCode;

typedef enum
{
    INTC_IRQ = 0,
    INTC_FIQ
}INTC_FastNormal;

typedef enum
{
    INTC_RISING_EDGE = 0,
    INTC_FALLING_EDGE,
    INTC_BOTH_EDGE,
    INTC_HIGH_LEVEL,
    INTC_LOW_LEVEL,
    INTC_BOTH_LEVEL
}INTC_ActivationType;

//typedef UINT32          INTC_InterruptPriorityTable[MAX_INTERRUPT_CONTROLLER_SOURCES];

/* The parameter that being sent to FISR, with some interrupt info. View API for details */
typedef UINT32          INTC_InterruptInfo;

// Callback function prototype for the interrupt handlers
// source - the interrupt source number that invoke the interrupt
typedef void (*INTC_ISR)(INTC_InterruptInfo interruptInfo);


/*----------------------------------------------------------------------*/


/*-- Constants definitions (C type) ------------------------------------*/

/*----------------------------------------------------------------------*/


/*-- Global variables declarations (Public G type) ---------------------*/

/*----------------------------------------------------------------------*/


/*-- Public Functions Prototypes ---------------------------------------*/
INTC_ReturnCode INTCPhase1Init(void);
void            INTCPhase2Init(void);

typedef UINT32 INTC_InterruptSources;

// ----------- added by PHS_SW_DEMO_TTC
//#if defined(INTC_CONFIGURE_WITH_3_ARGUMENTS)

INTC_InterruptSources INTCGetIntVirtualNum(UINT32 IRQ_NO);
INTC_ReturnCode INTCConfigure(INTC_InterruptSources isrSource, INTC_FastNormal fastNormal,
                              INTC_ActivationType activationType);
INTC_ReturnCode INTCConfigurationGet(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal,
                                     INTC_ActivationType *activationType);//, GPIO_Debounce *debouce);/*AlexCheck*/
//----------------------------------------

INTC_ReturnCode INTCBind(INTC_InterruptSources isrSource, INTC_ISR isr);

INTC_ReturnCode INTCUnbind(INTC_InterruptSources isrSource);

INTC_ReturnCode INTCISRGet(INTC_InterruptSources isrSource, INTC_ISR *isr);

INTC_ReturnCode INTCDisable(INTC_InterruptSources isrSource);

INTC_ReturnCode INTCEnable(INTC_InterruptSources isrSource);

INTC_ReturnCode INTCIdleMaskDisable(void);

INTC_ReturnCode INTCIdleMaskEnable(void);

INTC_ReturnCode INTCClear(INTC_InterruptSources source);
#define INTCClrSrc INTCClear

INTC_ReturnCode INTCDisableAllInts(void);


// copy from bsp_hisr.h
#define HISR_PRIORITY_0 0 //highest priority
#define HISR_PRIORITY_1 1 //medium priority
#define HISR_PRIORITY_2 2 //lowest priority

#define OS_Create_HISR		Os_Create_HISR /*call for NU_Create_HISR*/
#define OS_Activate_HISR 	Os_Activate_HISR
#define OS_Delete_HISR 		Os_Delete_HISR

typedef	void*	OS_HISR;


#ifdef __cplusplus
}
#endif

#endif  /* _INTC_H_ */
