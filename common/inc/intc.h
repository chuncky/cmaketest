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
#ifndef _INTC_H_
#define _INTC_H_

#include "common.h"
#include "gpio.h"

#define NUM_SWI               16
#define NUM_RI                8
#define NUM_XSWI              1
#define NUM_EIRQ              3
/*Fix coverity[overrun-call]*/

#define MAX_NUM_HWI           77

#define NUM_HWI_REGS          ((MAX_NUM_HWI + 15)/16)

#define SWI0_ID               0
#define RI0_ID                (SWI0_ID + NUM_SWI)
#define XSWI_ID               (RI0_ID + NUM_RI)
#define EIRQ0_ID              (XSWI_ID + NUM_XSWI)
#define SWI_EIRQ_ID           (EIRQ0_ID)
#define RI_EIRQ_ID            (EIRQ0_ID + 1)
#define XSWI_EIRQ_ID          (EIRQ0_ID + 2)
#define HWI0_ID               (EIRQ0_ID + NUM_EIRQ)

#define MAX_NUM_IRQ           (NUM_SWI + NUM_RI + NUM_XSWI + NUM_EIRQ + MAX_NUM_HWI)






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


/* The parameter that being sent to FISR, with some interrupt info. View API for details */
typedef UINT32          INTC_InterruptInfo;

// Callback function prototype for the interrupt handlers
// source - the interrupt source number that invoke the interrupt
typedef void (*INTC_ISR)(INTC_InterruptInfo interruptInfo);
#include "intc_list.h"
typedef UINT32          INTC_InterruptPriorityTable[MAX_INTERRUPT_CONTROLLER_SOURCES];

/*-- Type definitions (T type) -----------------------------------------*/
// Return code of INTC package operations



/*----------------------------------------------------------------------*/


/*-- Constants definitions (C type) ------------------------------------*/

/*----------------------------------------------------------------------*/


/*-- Global variables declarations (Public G type) ---------------------*/

/*----------------------------------------------------------------------*/


/*-- Public Functions Prototypes ---------------------------------------*/
INTC_ReturnCode INTCPhase1Init(void);
void            INTCPhase2Init(void);

// ----------- added by PHS_SW_DEMO_TTC

INTC_ReturnCode INTCConfigure(INTC_InterruptSources isrSource, INTC_FastNormal fastNormal,
                              INTC_ActivationType activationType);
INTC_ReturnCode INTCConfigurationGet(INTC_InterruptSources isrSource, INTC_FastNormal *fastNormal,
                                     INTC_ActivationType *activationType);//, GPIO_Debounce *debouce);/*AlexCheck*/

//----------------------------------------
INTC_ReturnCode INTCPriorityTableSet(const INTC_InterruptPriorityTable table);

void            INTCPriorityTableGet(INTC_InterruptPriorityTable table);

INTC_ReturnCode INTCBind(INTC_InterruptSources isrSource, INTC_ISR isr);

INTC_ReturnCode INTCUnbind(INTC_InterruptSources isrSource);

INTC_ReturnCode INTCISRGet(INTC_InterruptSources isrSource, INTC_ISR *isr);

INTC_ReturnCode INTCDisable(INTC_InterruptSources isrSource);

INTC_ReturnCode INTCEnable(INTC_InterruptSources isrSource);

INTC_ReturnCode INTCIdleMaskDisable(void);

INTC_ReturnCode INTCIdleMaskEnable(void);

INTC_ReturnCode INTCSourceToGPIOPinConvert(INTC_InterruptSources source, GPIO_PinNumbers *pin);

INTC_ReturnCode INTCSourceFromGPIOPinConvert(GPIO_PinNumbers pin,INTC_InterruptSources *source);

INTC_ReturnCode INTCClear(INTC_InterruptSources source);
#define INTCClrSrc INTCClear

INTC_ReturnCode INTCDisableAllInts(void);


/***************************************************************************
 * Manifest Constants
 **************************************************************************/

/* Masks */
#define IC_SWI_EN_MASK        0xFFFF
#define IC_RI_EN_MASK         0x00FF
#define IC_XSWI_EN_MASK       0x0001
#define IC_EIRQ_EN_MASK       0x0007
/* Mask of valid priority bits written to IC_xxx_PRI registers above */
#define IC_PRI_MASK           0xF


/* Flags */
#define IC_CLR_ENABLE         0x40
#define IC_SET_ENABLE         0x00


// Interrupt controller base address: IC_BASE_ADDRESS is defeatured, use IRQCTRL_BASE_ADDR

/* INTC registers offsets */
#define IC_SWI_ENABLE         0x00UL
#define IC_RI_ENABLE          0x04UL
#define IC_XSWI_ENABLE        0x08UL
#define IC_EIRQ_ENABLE        0x0CUL


/* NOTE: The number of HWIx_ENABLE registers actually implemented on target
 *       hardware depends on the NumHwi VHDL parameter that the IrqCtrl
 *       instance was instatiated with.
 */
#define IC_HWI0_ENABLE        0x20UL
#define IC_HWI1_ENABLE        0x24UL
#define IC_HWI2_ENABLE        0x28UL

#define IC_SWI_CLR_ENABLE     0x40UL
#define IC_REG_CLR_ENABLE     0x44UL
#define IC_EIRQ_CLR_ENABLE    0x48UL
#define IC_HWI0_CLR_ENABLE    0x60UL
#define IC_HWI1_CLR_ENABLE    0x64UL
#define IC_HWI2_CLR_ENABLE    0x68UL


#define IC_IRQ_PRI            0x80UL       /* Current Interrupt Priority */
#define IC_IRQ_NUM            0x84UL       /* Current Interrupt Number */

#define IC_ACTIVE_IRQ_MASK    0x88UL      /* Active Interrupt Mask */

#define IC_SYSTEM             0x8CUL       /* System Register */
#define IC_SYSTEM_RESET     0x0001       /* Reset bit in System Register */

#define IC_SWI_CARRY          0x90UL       /* SWI Carry Status (Bits 15:0) */
#define IC_RI_CARRY           0x94UL       /* RI Carry Status (Bits 7:0) */
#define IC_XSWI_CARRY         0x98UL       /* XSWI Carry Status (Bit 0) */
#define IC_CIN_VAL_REG        0x9CUL       /* Current Interrupt Number Value Register - same value as IC_IRQ_NUM, but
                                          * reading does not affect IC_ACTIVE_IRQ_MASK */

#define IC_SWI0_PRI           0xa0UL       /* SWIx Priority (bits 3:0) */
#define IC_SWI1_PRI           0xa4UL
#define IC_SWI2_PRI           0xa8UL
#define IC_SWI3_PRI           0xacUL
#define IC_SWI4_PRI           0xb0UL
#define IC_SWI5_PRI           0xb4UL
#define IC_SWI6_PRI           0xb8UL
#define IC_SWI7_PRI           0xbcUL
#define IC_SWI8_PRI           0xc0UL

#define IC_SWI9_PRI           0xc4UL
#define IC_SWI10_PRI          0xc8UL
#define IC_SWI11_PRI          0xccUL
#define IC_SWI12_PRI          0xd0UL
#define IC_SWI13_PRI          0xd4UL
#define IC_SWI14_PRI          0xd8UL
#define IC_SWI15_PRI          0xdcUL
//#define IC_SWIn_PRI(swi)     (IC_SWI0_PRI + (2*(swi)))
#define IC_SWIn_PRI(swi)     (IC_SWI0_PRI + (4*(swi)))

#define IC_RI0_PRI            0xe0UL       /* RIx Priority (bits 3:0) */
#define IC_RI1_PRI            0xe4UL
#define IC_RI2_PRI            0xe8UL
#define IC_RI3_PRI            0xecUL
#define IC_RI4_PRI            0xf0UL
#define IC_RI5_PRI            0xf4UL
#define IC_RI6_PRI            0xf8UL
#define IC_RI7_PRI            0xfcUL
//#define IC_RIn_PRI(ri)        (IC_RI0_PRI + (2*(ri)))
#define IC_RIn_PRI(ri)        (IC_RI0_PRI + (4*(ri)))

#define IC_XSWI_PRI           0x120UL       /* XSWI Priority (bits 3:0) */
# define IC_XSWI_EIRQ_PRI     0x140UL       /* Exception IRQ Priority (bits 3:0) */
# define IC_RI_EIRQ_PRI       0x144UL
# define IC_SWI_EIRQ_PRI      0x148UL
#define IC_EIRQ0_PRI          (IC_XSWI_EIRQ_PRI)       /* Exception IRQ Priority (bits 3:0) */


/* NOTE: The number of HWIx_PRI registers actually implemented on target
 *       hardware depends on the NumHwi VHDL parameter that the IrqCtrl
 *       instance was instatiated with.
 */
#define IC_HWI0_PRI           0x200UL      /* HWI0 Priority (bits 3:0) */
#define IC_HWIn_PRI(hwi)      (IC_HWI0_PRI + (4*(hwi)))



#define IC_SWI0_CNT           0x400UL      /* SWIx Count (bits 4:0) */
#define IC_SWI1_CNT           0x404UL
#define IC_SWI2_CNT           0x408UL
#define IC_SWI3_CNT           0x40cUL
#define IC_SWI4_CNT           0x410UL
#define IC_SWI5_CNT           0x414UL
#define IC_SWI6_CNT           0x418UL
#define IC_SWI7_CNT           0x41cUL
#define IC_SWI8_CNT           0x420UL
#define IC_SWI9_CNT           0x424UL
#define IC_SWI10_CNT          0x428UL
#define IC_SWI11_CNT          0x42cUL
#define IC_SWI12_CNT          0x430UL
#define IC_SWI13_CNT          0x434UL
#define IC_SWI14_CNT          0x438UL
#define IC_SWI15_CNT          0x43CUL
#define IC_SWIn_CNT(swi)      (IC_SWI0_CNT + (4*(swi)))

#define IC_RI_ASSERT          0x440UL      /* RI Register (bits 7:0) */

#define IC_XSWI_CNT           0x460UL      /* XSWI Count (bits 4:0) */
#define IC_XSWI_ASSERT        0x464UL      /* XSWI Assert (write any value) */
#define IC_IRQ_STATUS_CTRL    0x468UL

//#define IC_STICKY_CLR0        0x500UL - DUPLICATED, use IRQ_STICKY_CLR0
//#define IC_STICKY_CLR1        0x580UL - WRONG, UNUSED













/***************************************************************************
 *  Macros
 **************************************************************************/

#define  WR_REG16(bASE, rEGoFFSET, vALUE)    (*(volatile UINT16 *) ((bASE) + (rEGoFFSET)) = (vALUE))
#define  RD_REG16(bASE, rEGoFFSET)           (*(volatile UINT16 *) ((bASE) + (rEGoFFSET)))
#define  WR_REG32(bASE, rEGoFFSET, vALUE)    (*(volatile UINT32 *) ((bASE) + (rEGoFFSET)) = (vALUE))
#define  RD_REG32(bASE, rEGoFFSET)           (*(volatile UINT32 *) ((bASE) + (rEGoFFSET)))
//  #define   WR_IRQCTRL_REG(bASE, rEG, vALUE)    WR_REG32((bASE), (rEG), (vALUE))
#define   WR_IRQCTRL_REG(bASE, rEG, vALUE)    WR_REG16((bASE), (rEG), (vALUE))
/*******************************************************************************
 *
 * @Define:         RD_IRQCTRL_REG @
 *
 * @Interface:      PLK/M@
 *
 * @Description:    Macro to read a value from specified ABP IRQCTRL register @
 *
 ******************************************************************************/
//#  define   RD_IRQCTRL_REG(bASE, rEG)           RD_REG16(bASE, rEG)
//#  define     RD_IRQCTRL_REG(bASE, rEG)           RD_REG32((bASE), (rEG))
#  define     RD_IRQCTRL_REG(bASE, rEG)           RD_REG16((bASE), (rEG))
/****************************************************************************/

#define SWI_ENABLE_CLR_OFFSET   (0x0)
#define RI_ENABLE_CLR_OFFSET    (0x4)
#define XSWI_ENABLE_OFFSET      (0x8)

#define EIRQ_ENABLE_OFFSET      (0xC)
#define EIRQ_CLR_OFFSET         (0x8)

#define HWI_ENABLE_CLR_OFFSET   (0x20)

#define RD_WR_TYPE      UINT16

void IRQCTRL_HWI_STATE  (UINT32 bASE, UINT32 fLAG, UINT32 hWInUM);
void IRQCTRL_EIRQ_STATE (UINT32 bASE, UINT32 fLAG, UINT32 EirqInUM);
void IRQCTRL_XSWI_STATE (UINT32 bASE, UINT32 fLAG, UINT32 XswiInUM);
void IRQCTRL_RI_STATE   (UINT32 bASE, UINT32 fLAG, UINT32 RiInUM);
void IRQCTRL_SWI_STATE  (UINT32 bASE, UINT32 fLAG, UINT32 SwInUM);

#define REG_BYTE_SZ      (4)

/****************************************************************************/

/* IRQCTRL_STATE enables or disables the interrupt specified by it's absolute
 * interrupt number within the IRQCTRL. See IRQCTRL_ENABLE and IRQCTRL_DISABLE for
 * possible values of fLAG.
 */
#define IRQCTRL_STATE(bASE, fLAG, iRQnUM)                                                 \
      if (iRQnUM < NUM_SWI)                                                               \
         IRQCTRL_SWI_STATE(bASE, fLAG, (iRQnUM - SWI0_ID));                               \
      else                                                                                \
         if (iRQnUM < (NUM_SWI + NUM_RI))                                                 \
			IRQCTRL_RI_STATE(bASE, fLAG, (iRQnUM - RI0_ID));                              \
         else                                                                             \
            if (iRQnUM < (NUM_SWI + NUM_RI + NUM_XSWI))                                   \
			   IRQCTRL_XSWI_STATE(bASE, fLAG, (iRQnUM - XSWI_ID));                        \
            else                                                                          \
               if (iRQnUM < (NUM_SWI + NUM_RI + NUM_XSWI + NUM_EIRQ))                     \
				  IRQCTRL_EIRQ_STATE(bASE, fLAG, (iRQnUM - EIRQ0_ID));\
               else                                                                       \
                  IRQCTRL_HWI_STATE(bASE, fLAG , (iRQnUM - HWI0_ID));

/****************************************************************************/


/* IRQ_PRI sets the priority of interrupt given the absolute IRQ number */
#define IRQ_PRI(bASE, iRQnUM, pRI)                                                        \
      if (iRQnUM < NUM_SWI)                                                               \
         WR_IRQCTRL_REG(bASE, IC_SWI0_PRI + (REG_BYTE_SZ*(iRQnUM)), pRI);                           \
      else                                                                                \
         if (iRQnUM < (NUM_SWI + NUM_RI))                                                 \
            WR_IRQCTRL_REG(bASE, IC_RI0_PRI + (REG_BYTE_SZ*(iRQnUM - RI0_ID)), pRI);                \
         else                                                                             \
            if (iRQnUM < (NUM_SWI + NUM_RI + NUM_XSWI))                                   \
               WR_IRQCTRL_REG(bASE, IC_XSWI_PRI + (REG_BYTE_SZ*(iRQnUM - XSWI_ID)), pRI);           \
            else                                                                          \
               if (iRQnUM < (NUM_SWI + NUM_RI + NUM_XSWI + NUM_EIRQ))                     \
                WR_IRQCTRL_REG(bASE, IC_EIRQ0_PRI + (REG_BYTE_SZ*(iRQnUM - EIRQ0_ID)), pRI);      \
               else                                                                       \
                  WR_IRQCTRL_REG(bASE, IC_HWI0_PRI + (REG_BYTE_SZ*(iRQnUM - HWI0_ID)), pRI);

/****************************************************************************/

/* IRQCTRL_ENABLE/DISABLE the interrupt specified by it's absolute IRQ number */
#define IRQCTRL_ENABLE(bASE, iRQnUM)      IRQCTRL_STATE(bASE, IC_SET_ENABLE, iRQnUM)
#define IRQCTRL_DISABLE(bASE, iRQnUM)     IRQCTRL_STATE(bASE, IC_CLR_ENABLE, iRQnUM)
#define IRQCTRL_BASE_ADDR     0xD4041000//0xD0231000////pei changed

#define   IRQCTRL_MCU    IRQCTRL_BASE_ADDR

/* Sticky bit clears*/
#define   IRQ_STICKY_CLR0  (IRQCTRL_BASE_ADDR+XIRQ_STICKY_OFFSET)
#define   IRQ_STICKY_CLR16 (IRQ_STICKY_CLR0+0x04)
//#define   IRQ_STICKY_CLR1  (IRQ_STICKY_CLR0+0x80) - WRONG, UNUSED
#define INTC_CONFIGURE_WITH_3_ARGUMENTS // Matches the SINTC definition, see sintc.h
#define XIRQ_STICKY_OFFSET 0x8f8
#define XIRQ_EDGELEVEL_OFFSET 0x8f0
/* Level or edge interrupt control */
//#define   IRQ_LVL_EDG0    (volatile UINT32 *)(0xD4000E00)
#define   IRQ_LVL_EDG0    (volatile UINT32 *)(IRQCTRL_BASE_ADDR+XIRQ_EDGELEVEL_OFFSET)

//#define   IRQ_LVL_EDG1    (volatile UINT32 *)(0xD4000E04)
#define   IRQ_LVL_EDG1    (volatile UINT32 *)(IRQCTRL_BASE_ADDR+XIRQ_EDGELEVEL_OFFSET+0x04)
//#define   WR_IRQCTRL_REG(bASE, rEG, vALUE)    WR_REG16(bASE, rEG, vALUE)
/* AlexR :Register value is multiplied by 2 , becouse all offsets (rEG) defined for 16-bits registers*/


#define NUM_OF_VIRTUAL_INT       4
typedef struct
{
	UINT32	InterruptNumber;	/* Absolute interrupt number */
	UINT32	InterruptPriority;
} INTC_emulation_priority_table_S;

typedef struct INTC_HW_REGS_ST
{
	UINT32 IC_SWI_ENABLE_REG	;   /*0x00UL*/
	UINT32 IC_RI_ENABLE_REG		;   /*0x04UL*/
	UINT32 IC_XSWI_ENABLE_REG	;   /*0x08UL*/
	UINT32 IC_EIRQ_ENABLE_REG	;   /*0x0CUL*/
	UINT32 Reserved_1	[4]		;   /*0x10 0x14 0x18 0x1c*/
	UINT32 IC_HWI_ENABLE_REG_arr[4]	;   /*0x20 0x24 0x28 0x2c*/
	UINT32 Reserved_2   [4]		;   /* 0x30 0x34 0x38 0x3c */
	UINT32 IC_SWI_CLR_ENABLE_REG	; /*0x40UL*/
	UINT32 IC_RI_CLR_ENABLE_REG	; /*0x44UL*/
	UINT32 IC_EIRQ_CLR_ENABLE_REG	; /*0x48UL*/
	UINT32 Reserved_3   [5]		;   /*0x4c 0x50 0x54 0x58 0x5c */
	UINT32 IC_HWI_CLR_ENABLE_REG_arr[4]	; /*0x60 0x64 0x68 0x6C*/

}INTC_HW_CONF_REGS_Y;


#define IRQ_VECT_SIZE   (MAX_NUM_IRQ + NUM_OF_VIRTUAL_INT)

#define INTERRUPT_CONTROLLER_HW_ADDRESS     0xD4282000L
#define P_ICU_NUM_SOURCES                   64
#define P_ICU_NUM_SOURCES_HELANLTE          96

/*-----------------------------------------------------------------------**
** Following mapping depicts the Interrupt controller registers          **
** location with offset of hte basic address.                            **
**-----------------------------------------------------------------------*/
union InterruptHWRegisters
{
  struct
  {
    UINT32       ICU_CONF[P_ICU_NUM_SOURCES];  // Mask, configuration, priority for 64 sources
	UINT32       ICU_CP_FIQ_PENDING;           // CP FIQ selected pending interrupt
	UINT32       ICU_CP_IRQ_PENDING;           // CP IRQ selected pending interrupt
	UINT32       ICU_AP_FIQ_PENDING;           // AP FIQ selected pending interrupt
	UINT32       ICU_AP_IRQ_PENDING;           // AP IRQ selected pending interrupt
	UINT32       ICU_CP_GLOBAL_MASK;           // CP mask all interrupts (for IDLE entry, automatically cleared by PMU)
	UINT32       ICU_AP_GLOBAL_MASK;           // AP mask all interrupts (for IDLE entry, automatically cleared by PMU)
	UINT32       ICU_CP_DMA_INT_MASK;          // CP interrupt mask for 32 DMA channels
	UINT32       ICU_AP_DMA_INT_MASK;          // AP interrupt mask for 32 DMA channels
	UINT32       ICU_CP_DMA_INT_STATUS;        // CP interrupt status for 32 DMA channels
	UINT32       ICU_AP_DMA_INT_STATUS;        // AP interrupt status for 32 DMA channels
	UINT32       ICU_INT_STATUS_0;             // Interrupt status (after masking) for sources [31..0]
	UINT32       ICU_INT_STATUS_1;             // Interrupt status (after masking) for sources [63..32]
	UINT32       ICU_VPRO_INT_MASK;            // VPRO interrupt mask
	UINT32       ICU_VPRO_INT_STATUS;          // VPRO interrupt status
	UINT32       ICU_INT_STATUS_2;             // Interrupt status (after masking) for sources 
	UINT32       ICU_INT_STATUS_3;             // Interrupt status (after masking) for sources 
  	UINT32	     ICU_CONF_H[P_ICU_NUM_SOURCES];  // Mask, configuration, priority for 64-127 sources
  }all;
  struct
  {
#if defined(INTC_CORE_AP) ||defined(_TAVOR_BOERNE_) //PHS_SW_DEMO_TTC
    UINT32       reserved0[P_ICU_NUM_SOURCES];
	UINT32       reserved1[2];
	UINT32       ICU_FIQ_PENDING;              // FIQ selected pending interrupt
	UINT32       ICU_IRQ_PENDING;              // IRQ selected pending interrupt
	UINT32       reserved2;
	UINT32       ICU_GLOBAL_MASK;              // mask all interrupts (for IDLE entry, automatically cleared by PMU)
	UINT32       reserved3;
	UINT32       ICU_DMA_INT_MASK;             // interrupt mask for 32 DMA channels
	UINT32       reserved4;
	UINT32       ICU_DMA_INT_STATUS;           // interrupt status for 32 DMA channels
	UINT32       reserved5[4];
#endif

 //PHS_SW_DEMO_TTC
    UINT32       reserved0[P_ICU_NUM_SOURCES];
	UINT32       ICU_FIQ_PENDING;              // FIQ selected pending interrupt
	UINT32       ICU_IRQ_PENDING;              // IRQ selected pending interrupt
	UINT32       reserved1[2];
	UINT32       ICU_GLOBAL_MASK;              // mask all interrupts (for IDLE entry, automatically cleared by PMU)
	UINT32       reserved2;
	UINT32       ICU_DMA_INT_MASK;             // interrupt mask for 32 DMA channels
	UINT32       reserved3;
	UINT32       ICU_DMA_INT_STATUS;           // interrupt status for 32 DMA channels
	UINT32       reserved4;
	UINT32       reserved5[4];

  } own;
};

/*
 * NOTES:
 * - ICU_INT_STATUS_n registers indicate interrupt status for interrupts routed to all cores (AP and CP)
 *   Therefore these cannot be used to determine if specific core has an interrupt pending.
*/

// ICU_CONF registers structure
#define ICU_CONF_PRIO_BITS                 0x0F
#define ICU_CONF_PRIO_MASKED               0x0
#define ICU_CONF_TYPE_BITS                 0x10
#define ICU_CONF_TYPE_FIQ                  0x00
#define ICU_CONF_TYPE_IRQ                  0x10

#define ICU_CONF_CP_INT                    0x20
#define ICU_CONF_AP_INT                    0x40
#define ICU_CONF_OWNER_BITS                (ICU_CONF_CP_INT|ICU_CONF_AP_INT)
#if defined(INTC_CORE_AP) ||defined(_TAVOR_BOERNE_) //PHS_SW_DEMO_TTC
#define ICU_CONF_THIS_CORE_INT             ICU_CONF_AP_INT
#endif
//#if defined(INTC_CORE_CP) || defined(_TAVOR_HARBELL_) || defined (SILICON_PV2) //PHS_SW_DEMO_TTC
#if defined(INTC_CORE_CP) || defined (SILICON_PV2) //PHS_SW_DEMO_TTC
#define ICU_CONF_THIS_CORE_INT             ICU_CONF_CP_INT
#endif
#define ICU_CONF_OTHER_CORE_INT            (ICU_CONF_THIS_CORE_INT^ICU_CONF_OWNER_BITS)

// all _PENDING  registers structure
#define ICU_PENDING_INT_NUM_BITS           0x7F
#define ICU_PENDING_INT_VALID_BITS         0x80

#define ICU_PENDING_INT_NUM_BITS_CRANE           0x7F
#define ICU_PENDING_INT_VALID_BITS_CRANE         0x80


#define ICU_PENDING_INT_NUM_BITS_HELANLTE     0x7F
#define ICU_PENDING_INT_VALID_BITS_HELANLTE   0x80

// ICU_CP_GLOBAL_MASK/ICU_AP_GLOBAL_MASK registers structure
#define ICU_GLOBAL_MASK_FIQ                1
#define ICU_GLOBAL_MASK_IRQ                2
#define ICU_GLOBAL_MASK_ALL                (ICU_GLOBAL_MASK_FIQ|ICU_GLOBAL_MASK_IRQ)

// ICU_VPRO_INT_MASK/ICU_VPRO_INT_STATUS registers structure
#define ICU_VPRO_DMA_BITS                  0x7             // bits [2..0]
#define ICU_VPRO_SEM_BITS                  0x38            // bits [5..3]



#define     InterruptController    (* (volatile union InterruptHWRegisters *) INTERRUPT_CONTROLLER_HW_ADDRESS)

#define ICU_MASK_INT(i) {(InterruptController.all.ICU_CONF[i] &= ~ICU_CONF_PRIO_BITS);(InterruptController.all.ICU_CONF_H[i] &= ~ICU_CONF_PRIO_BITS);}




#define ICU_CONF_THIS_CORE_INT             ICU_CONF_CP_INT
#define ICU_CONF_OTHER_CORE_INT            (ICU_CONF_THIS_CORE_INT^ICU_CONF_OWNER_BITS)







 #endif  /* _INTC_H_ */


