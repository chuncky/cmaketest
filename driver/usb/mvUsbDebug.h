/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/*******************************************************************************
** File          : $HeadURL$
** Author        : $Author$
** Project       : HSCTRL
** Instances     :
** Creation date :
********************************************************************************
********************************************************************************
** ChipIdea Microelectronica - IPCS
** TECMAIA, Rua Eng. Frederico Ulrich, n 2650
** 4470-920 MOREIRA MAIA
** Portugal
** Tel: +351 229471010
** Fax: +351 229471011
** e_mail: chipidea.com
********************************************************************************
** ISO 9001:2000 - Certified Company
** (C) 2005 Copyright Chipidea(R)
** Chipidea(R) - Microelectronica, S.A. reserves the right to make changes to
** the information contained herein without notice. No liability shall be
** incurred as a result of its use or application.
********************************************************************************
** Modification history:
** $Date$
** $Revision$
*******************************************************************************
*** Comments:
***   This file contains definitions for debugging the software stack
***
**************************************************************************
**END*********************************************************/
#ifndef __mvUsbDebug_h__
#define __mvUsbDebug_h__

#define MAX_STRING_SIZE  200

#include "udc2_hw.h"
#if defined MV_USB_TRACE_PRINT || defined MV_USB_TRACE_LOG || defined MV_RT_DEBUG
#include "UART.h"
#include "stdio.h"      // for sprintf
#include "utils.h"      // for disableInterrupts definition


extern char    _usbDeviceDebugBuffer[];

#endif  /* MV_USB_TRACE_PRINT || MV_USB_TRACE_LOG */


/************************************************************
The following array is used to make a run time trace route
inside the USB stack.
*************************************************************/

#define MV_USB_DEBUG_FLAG_ANY           0x00000000

#define MV_USB_DEBUG_FLAG_TRACE         0x00000001
#define MV_USB_DEBUG_FLAG_CTRL          0x00000002
#define MV_USB_DEBUG_FLAG_RX            0x00000004
#define MV_USB_DEBUG_FLAG_TX            0x00000008
#define MV_USB_DEBUG_FLAG_STALL         0x00000010
#define MV_USB_DEBUG_FLAG_STATUS        0x00000020
#define MV_USB_DEBUG_FLAG_TRANSFER      0x00000040
#define MV_USB_DEBUG_FLAG_INIT          0x00000080
#define MV_USB_DEBUG_FLAG_ISR           0x00000100
#define MV_USB_DEBUG_FLAG_ERROR         0x00000200
#define MV_USB_DEBUG_FLAG_ADDR          0x00000400
#define MV_USB_DEBUG_FLAG_DUMP          0x00000800
#define MV_USB_DEBUG_FLAG_SETUP         0x00001000
#define MV_USB_DEBUG_FLAG_CLASS         0x00002000
#define MV_USB_DEBUG_FLAG_SPEED         0x00004000
#define MV_USB_DEBUG_FLAG_RESET         0x00008000
#define MV_USB_DEBUG_FLAG_SUSPEND       0x00010000
#define MV_USB_DEBUG_FLAG_RESUME        0x00020000
#define MV_USB_DEBUG_FLAG_EP0           0x00040000
#define MV_USB_DEBUG_FLAG_NON_EP0       0x00080000
#define MV_USB_DEBUG_FLAG_STATS         0x00100000
#define MV_USB_DEBUG_FLAG_WARNING       0x00200000
#define MV_USB_DEBUG_FLAG_INFO          0x00400000
#define MV_USB_DEBUG_FLAG_STATISTICS     (1 << 23)
#define MV_USB_DEBUG_FLAG_REGS           (1 << 24)
#define MV_USB_DEBUG_FLAG_EP_STATUS      (1 << 25)
#define MV_USB_DEBUG_FLAG_LOG_PRINTS     (1 << 26)
#define MV_USB_DEBUG_FLAG_TEST           (1 << 27)
#define MV_USB_DEBUG_FLAG_UNSTALL        (1 << 28)
#define MV_USB_DEBUG_FLAG_PMU_SLP    	 (1 << 29)
#define MV_USB_DEBUG_FLAG_PMU_WK         (1 << 30)



#define MV_USB_DEBUG_FLAG_ALL           0xffffffff

extern uint_32  usbDebugFlags;


#if defined(MV_USB_TRACE_LOG)
    #define USB_sprintf(bUFF, fRMT, x...)      \
        sprintf(bUFF, fRMT, ##x);

#else
    #define USB_sprintf(frmt, x...)
#endif


#if defined (MV_USB_TRACE_PRINT)
//    #define USB_printf(frmt, x...)      armprintf(frmt, ##x)
/*    #define USB_printf(fRMT,x...)        \
    {   \
            sprintf(_usbDeviceDebugBuffer, fRMT, ##x);                                  \
            UARTPrintString(UART_PORT_FFUART, _usbDeviceDebugBuffer) ;                  \
    }
    // #define USB_printf(fRMT, x...)
    //void USB_printf(const char *fRMT,...);

#else  */
    #define USB_printf(fRMT, x...)
#endif /* MV_USB_TRACE_PRINT */


#ifdef MV_RT_DEBUG
    #define MV_USB_DEBUG_CODE(fLAGS, cODE)          \
        if( (usbDebugFlags & (fLAGS)) == (fLAGS) )  \
            cODE
#else
    #define MV_USB_DEBUG_CODE(fLAGS, cODE)
#endif

#if defined(MV_USB_TRACE_LOG)

    #define TRACE_ARRAY_SIZE 400

    extern uint_16 DEBUG_TRACE_ARRAY_COUNTER;
    extern char    DEBUG_TRACE_ARRAY[TRACE_ARRAY_SIZE][MAX_STRING_SIZE];


    #define MV_USB_DEBUG_TRACE(fLAGS, fORMAT, x...)                                 \
    {                                                                               \
        if( ((usbDebugFlags) & (fLAGS)) == (fLAGS))                                 \
        {                                                                           \
            unsigned long cpsr;                                                     \
            unsigned int idx;                                                       \
            cpsr = disableInterrupts();                                             \
            idx = DEBUG_TRACE_ARRAY_COUNTER++;                                      \
            if(DEBUG_TRACE_ARRAY_COUNTER >= TRACE_ARRAY_SIZE)                       \
                {DEBUG_TRACE_ARRAY_COUNTER = 0;}                                    \
            restoreInterrupts(cpsr);                                                \
            USB_sprintf(DEBUG_TRACE_ARRAY[idx], fORMAT, ##x);                       \
			if ( ( (fLAGS) == MV_USB_DEBUG_FLAG_ERROR) || ((fLAGS) == MV_USB_DEBUG_FLAG_WARNING ) )          \
			{																		\
				ASSERT (FALSE);														\
			}																		\
        }                                                                           \
    }

#elif defined(MV_USB_TRACE_PRINT)

    #define MV_USB_DEBUG_TRACE(fLAGS, fORMAT, x...)     \
          if (((usbDebugFlags) & (fLAGS)) == (fLAGS))   \
          { \
                unsigned long cpsr;                     \
                unsigned int idx;                       \
                cpsr = disableInterrupts();             \
                USB_printf(fORMAT, ##x)                 \
                restoreInterrupts(cpsr);                \
		   		if ( ((fLAGS) == MV_USB_DEBUG_FLAG_ERROR) || ((fLAGS) == MV_USB_DEBUG_FLAG_WARNING ) )          \
				{																		\
					ASSERT (FALSE);														\
				}																		\
          }

/*if trace switch is not enabled define debug log trace to empty*/
#else
	extern int uart_printf(const char* fmt, ...);
#define MV_USB_DEBUG_TRACE(fLAGS, fORMAT, x...) 	\
		 if (((usbDebugFlags) & (fLAGS)) == (fLAGS))   \
		 { \
			   uart_printf(fORMAT, ##x);				 \
		 }
#endif


#endif /* __mvUsbDebug_h__ */
/* EOF */
