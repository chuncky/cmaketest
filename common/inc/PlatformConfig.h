/******************************************************************************

** (C) Copyright 2007 Marvell International Ltd. ?

**?All Rights Reserved

**

**  FILENAME:	PlatformConfig.h

**

**  PURPOSE: 	Platform specific header to set parameters

** 			

**                  

******************************************************************************/



#ifndef __platform_config_h

#define __platform_config_h



#include "Typedef.h"
#include "Errors.h"
//#include "smemc.h"
//
// Timers
#define APB_CU_ENABLE (*APB_CU_CR = 0x33)
#define TIMER_FREQ(A)  (((A)*13) >> 2)

// MMC 
#define NUMBER_OF_SUPPORTED_SDMMC_CONFIGURATIONS 1

// SPI
#define SSP_BASE_FOR_SPI		 SSP3_BASE
#define spi_reg_bit_set(reg, val)
#define spi_reg_bit_clr(reg, val)
#define SSP_RX_DMA_DEVICE		DMAC_SSP_2_RX
#define SSP_TX_DMA_DEVICE		DMAC_SSP_2_TX


//
// Clock in nanoseconds. Not likely to be changing all that often...

//

#define NAND_CONTROLLER_CLOCK  104  // MHz

#define UARTCLK    14745600



#define PlatformISRAMLimit	0x12000 //768KB - 76KB (0xC0000 - 0x13000)



//
// Clock in nanoseconds. Not likely to be changing all that often...
//
#define DFC_WORKAROUND 0

// Bit 19 of control register must be written as 0
//#define DFC_INITIAL_CONTROL 0xCC021FFF
#define DFC_INITIAL_CONTROL 0x8C821FFF
#define	DFC_INITIAL_TIMING0	0x003F3F3F
#define DFC_INITIAL_TIMING1 0x100080FF //tR set to 52.4 usec



#define TIMOffset_CS0 0x00000000 // offset for TIM on CS0	



#define CHIPBOOTTYPE (volatile unsigned long *)GEU_FUSE_VAL_APCFG1





#define ASCR	(volatile unsigned long *)PMUM_CPSR //See PMUM.h
#define ARSR	(volatile unsigned long *)PMUM_CRSR 

#define AD3SR	(volatile unsigned long *)PMUM_AWUCRS 
#define AD3R	(volatile unsigned long *)PMUM_AWUCRM 



#define APBC_AIB_CLK_RST (volatile unsigned long *)0xD401503C


#define APPS_PAD_BASE  0xd401e000

#define SMEMC_BASE	   SMC_BASE	



// OneNand boots from CS0 only

#define FLEX_BOOTRAM_MAIN_BASE	CS0Base			





//

// Multifunction Padring Structures

//

typedef struct 

{

  volatile int *    registerAddr;

  unsigned int      regValue;

  unsigned int      defaultValue;

}CS_REGISTER_PAIR_S, *P_CS_REGISTER_PAIR_S;





//Prototypes

UINT_T ChipSelect2(void);

UINT_T ChipSelect0(void);

void ChipSelectDFC(void);

void RestoreDefaultConfig(void);

void SaveDefaultConfig(void);

void ChipSelectOneNAND( void );

void ChipSelectMsys( void );

UINT_T PlatformMdocRdyForXIPAccess( void );

//void PlatformResumeSetup(pQuick_Resume_Function *, pTim_Resume_Function *);

UINT_T getPlatformPortSelection(unsigned int *);

UINT8_T* GetProbeOrder(void);

UINT_T PlatformUARTConfig(void);

UINT_T PlatformAltUARTConfig(void);

#if 0
//UDC 1.1 Functions

UINT_T SetupSingleEndedPort2(void);

UINT_T PlatformUsbTransmit(UINT_T, UINT8_T*);

void PlatformUsbInit(UINT_T);

void PlatformUsbShutdown(void);

void PlatformUsbDmaHandler(void);

void PlatformUsbInterruptHandler(void);

//U2D Functions

void PlatformU2DInit(void);

UINT_T PlatformU2DTransmit(UINT_T, UINT8_T*, UINT_T);

void PlatformU2DShutdown(void);

void PlatformU2DInterruptHandler(void);



//CI2 USB Functions
void PlatformCI2Init(void);
UINT_T PlatformCI2Transmit(UINT_T, UINT8_T*, UINT_T);
void PlatformCI2Shutdown(void);
void PlatformCI2InterruptHandler(void);

#endif

// DCB added these to get things compiling

//------------------------------------------

#define PLATFORM_CHIPBOOTTYPE_FUSE_REG 2

UINT_T CheckSuperSetPortEnablement(UINT_T Port);
//UINT_T  GetPlatformFuses(UINT_T platformFuses, pFUSE_SET pTBR_Fuse);

UINT_T ReturnServicesFuses(UINT_T ServiceNumber);
UINT_T PlatformUARTConfig(void);
UINT_T PlatformAltUARTConfig(void);



//specify if the platform requires save state for trusted operation

UINT_T PlatformSaveStateRequired(void);



//external prototypes


#if 0
extern UINT_T GetOSCR0(void);

extern UINT_T OSCR0IntervalInSec(UINT_T Before, UINT_T After);

extern UINT_T OSCR0IntervalInMilli(UINT_T Before, UINT_T After);

extern UINT_T OSCR0IntervalInMicro(UINT_T Before, UINT_T After);
#endif

#endif