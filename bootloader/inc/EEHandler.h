/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/


/*******************************************************************************
*               MODULE HEADER FILE
********************************************************************************
* Title: Error Handler header
*
* Filename: EEHandler.h
*
* Target, platform: Manitoba
*
* Authors: Yossi Hanin
*
* Description: Error handler component header file
*
* Last Updated:
*
* Notes:
*******************************************************************************/

#ifndef _EEHANDLER_H_
#define _EEHANDLER_H_

#include "common.h"


/******************************************************************************************/
/*                                 GENERAL                                                */
/******************************************************************************************/

//ICAT EXPORTED ENUM
typedef enum
{
	//enumerated
    EE_SYS_RESET_EN,
    EE_ASSERT_EN,
    EE_EXCEPTION_EN,
    EE_WARNING_EN,
	EE_NUM_ENTRY_TYPES,
	// Codes
    EE_SYS_RESET = 300,
    EE_ASSERT = 350,
    EE_EXCEPTION = 450,
    EE_WARNING = 550
} EE_entry_t;


//ICAT EXPORTED ENUM
typedef enum
{
    PMU_POR = 1,
    PMU_EMR,
    PMU_WDTR = (PMU_EMR+2)
}PMU_LastResetStatus;

//ICAT EXPORTED ENUM
typedef enum
{
  EEE_DataAbort,
  EEE_PrefetchAbort,
  EEE_FatalError,
  EEE_SWInterrupt,
  EEE_UndefInst,
  EEE_ReservedInt
} EE_ExceptionType_t;


//ICAT EXPORTED ENUM
typedef enum
{
    EE_NO_RESET_SOURCE,
    EE_POWER_ON_RESET = PMU_POR,
    EE_EXT_MASTER_RESET,
    EE_WDT_RESET =(PMU_EMR+2)
} EE_PMU_t;

/* secondary exception types */
typedef enum
{
    VERTICAL = 0,
    ALIGNMENT_1 = 1,
    TERMINAL = 2,
    ALIGNMENT_2 = 3,
    EXTERNAL_ABORT_ON_TRANSLATION = 12,
    EXTERNAL_ABORT_ON_TRANSLATION_2 = 14,
    TRANSLATION = 5,
    TRANSLATION_2 = 7,
    EE_DOMAIN = 9,
    EE_DOMAIN_2 = 11,
    PERMISSION = 13,
    PERMISSION_2 = 15,
    IMPRECISE_EXTERNAL_DATA_ABORT = 6,
    LOCK_ABORT = 4,
    DATA_CACHE_PARITY_ERROR = 8,
    INSTRUCTION_MMU = 0 ,
    EXTERNAL_INSTRUCTION_ERROR = 6,
    INSTRUCTION_CACHE_PARITY_ERROR = 8
} EXCEPTION_TYPE;

/******************************************************************************************/
/*                            CPU register context format                                 */
/******************************************************************************************/

//ICAT EXPORTED STRUCT
typedef struct
{
    UINT32              r0;    /* register r0 contents */
    UINT32              r1;    /* register r1 contents */
    UINT32              r2;    /* register r2 contents */
    UINT32              r3;    /* register r3 contents */
    UINT32              r4;    /* register r4 contents */
    UINT32              r5;    /* register r5 contents */
    UINT32              r6;    /* register r6 contents */
    UINT32              r7;    /* register r7 contents */
    UINT32              r8;    /* register r8 contents */
    UINT32              r9;    /* register r9 contents */
    UINT32              r10;   /* register r10 contents */
    UINT32              r11;   /* register r11 contents */
    UINT32              r12;   /* register r12 contents */
    UINT32              SP;   /* register r13 contents */
    UINT32              LR;   /* register r14 contents (excepted mode)*/
	UINT32              PC;   /* PC - excepted instruction */
    UINT32              cpsr;  /* saved program status register contents */
    UINT32              FSR;   /* Fault status register */
    UINT32              FAR_R; /* Fault address register */
    EE_PMU_t            PMU_reg;  /* saved reset cause - should be last */

   // UINT32              PESR;  /* Extension */
   // UINT32              XESR;
   // UINT32              PEAR;
   // UINT32              FEAR;
   // UINT32              SEAR;
   // UINT32              GEAR;
} EE_RegInfo_Data_t;

//

#endif//end of _ERROR_HANDLER_H_

