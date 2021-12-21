/*------------------------------------------------------------
(C) Copyright [2006-2008] Marvell International Ltd.
All Rights Reserved
------------------------------------------------------------*/

/* this file is the SSP_hw.h : it includes all the unions for the registers:  */

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
*  Filename: SSP_HW_ACCESS.h
*
*  Target, subsystem: Common Platform, HAL
*
*  Authors: Yanai Oron
*			Zafi Dayan (Porting to Manitoba)
*
*  Description:  SSP HW Access Header file.
*
*  Last Modified: <date>
*
******************************************************************************/

#ifndef _SSP_HW_ACCESS1_H_
#define _SSP_HW_ACCESS1_H_


//#include "hermon_regs.h"
//#include "dma_list.h"                                  // For DMA_Peripherals
/************* SSP HW defenitions ******************************************/

/* Setting physical addresses for SSP registers */
#define SSP_0_START_ADDRESS   (0xD401B000)
#define SSP_1_START_ADDRESS   (0xD42A0C00)
#define SSP_2_START_ADDRESS   (0xD401C000)
#ifdef jyz
typedef struct
{
    UINT32      SSCR0;     				/* SSP Control Register 0            	SSPBase + 0x00 */
    UINT32      SSCR1;     				/* SSP Control Register 1            	SSPBase + 0x04 */
    UINT32      SSSR;      				/* SSP Status Register               	SSPBase + 0x08 */
    UINT32      SSITR;      			/* SSP Interrupt Test Register       	SSPBase + 0x0C */
    UINT32      SSDR;      				/* SSP Data Read/Write Register      	SSPBase + 0x10 */
    UINT32      Reserved1;  			/*                                   	SSPBase + 0x14 */
    UINT32      Reserved2;  			/*                                   	SSPBase + 0x18 */
    UINT32      Reserved3;  			/*                                   	SSPBase + 0x1C */
    UINT32      Reserved4;  			/*                                   	SSPBase + 0x20 */
    UINT32      Reserved5;  			/*                                   	SSPBase + 0x24 */
    UINT32      SSTO;       			/* SSP Time Out Register             	SSPBase + 0x28 */
    UINT32      SSPSP;      			/* SSP Programmabble Serial Protocol  	SSPBase + 0x2C */

    UINT32      SSTSA;      			/* SSP TX time slot active  		 	SSPBase + 0x30 */
    UINT32      SSRSA;      			/* SSP RX time slot active  		 	SSPBase + 0x34 */
    UINT32      SSTSS;      			/* SSP time slot status              	SSPBase + 0x38 */
    UINT32      Reserved[0x70];
    UINT32      DEFAULT_SSPCLK;         /* Default value to the unused clock */
    UINT32      PIN_SEL;                /* Select between set A and set B of pins */

} SSPPortHWRegisters;
#endif

typedef struct{
    volatile UINT32 TOP_CTRL;
    volatile UINT32 FIFO_CTRL;
    volatile UINT32 INT_EN;    //0x8
    volatile UINT32 TO;        //0xc
    volatile UINT32 SSDR;     //0x10
    volatile UINT32 SSSR;    //0x14
    volatile UINT32 PSP_CTRL;   //0x18
    volatile UINT32 NETWORK_CTRL;   //0x1C
    volatile UINT32 NETWORK_STATUS;   //0x20
    volatile UINT32 RWOT_CTRL;        //0x24
    volatile UINT32 RWOT_CCM;         //0x28
    volatile UINT32 RWOT_CVWRn;       //0x2c
    volatile UINT32 Resv[(0x54-0x2c)/4-1];
    volatile UINT32 THREE_WIRE_CTRL;  //054
}SSPPortHWRegisters;


extern int fatal_printf(const char* fmt, ...);

typedef struct
{
  UINT32		txChannel;
  UINT32		rxChannel;
  UINT32		txCurrDescIndex;
  UINT32		rxCurrDescIndex;
  UINT32		txStopDescIndex;
  UINT32		rxStopDescIndex;
  BOOL		    dmaOn;
} SSP_CHANNEL;


typedef struct
{
  UINT32		txChannel;
  UINT32		rxChannel;
} SSP_DMA_PREQ;
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)
#define BIT_4 (1 << 4)
#define BIT_5 (1 << 5)
#define BIT_6 (1 << 6)
#define BIT_7 (1 << 7)
#define BIT_8 (1 << 8)
#define BIT_9 (1 << 9)
#define BIT_10 (1 << 10)
#define BIT_11 (1 << 11)
#define BIT_12 (1 << 12)
#define BIT_13 (1 << 13)
#define BIT_14 (1 << 14)
#define BIT_15 (1 << 15)
#define BIT_16 (1 << 16)
#define BIT_17 (1 << 17)
#define BIT_18 (1 << 18)
#define BIT_19 (1 << 19)
#define BIT_20 (1 << 20)
#define BIT_21 (1 << 21)
#define BIT_22 (1 << 22)
#define BIT_23 (1 << 23)
#define BIT_24 (1 << 24)
#define BIT_25 (1 << 25)
#define BIT_26 (1 << 26)
#define BIT_27 (1 << 27)
#define BIT_28 (1 << 28)
#define BIT_29 (1 << 29)
#define BIT_30 (1 << 30)
#define BIT_31 ((unsigned)1 << 31)

/* MACRO DEFINITIONS:  */
/* SSCR0 Macros:  */
#define TOP_CTRL_DSS_SET(reg,DSSValue)               (   (reg) = ((reg) & ~(0x1F << 5)) | (DSSValue << 5)       )
#define TOP_CTRL_DSS_GET(reg)               		(   ((reg) & (0x1F << 5)) >> 5       )

#define TOP_CTRL_FRF_SET_MOTOROLA_SPI(reg)           (   (reg) = ((reg) & ~(BIT_1|BIT_2))          )
//#define SSCR0_FRF_SET_TEXAS_INSTRUMENTS_SSP(reg)  (   (reg) = ((reg) & 0xFFFFFFCF) | 0x00000010              )
//#define SSCR0_FRF_SET_NATIONAL_MICROWIRE(reg)     (   (reg) = ((reg) & 0xFFFFFFCF) | 0x00000020              )
//#define SSCR0_FRF_SET_PSP(reg)                    (   (reg) = ((reg) & 0xFFFFFFCF) | 0x00000030              )
//#define SSCR0_ECS_ON_CHIP_CLOCK(reg)              (   (reg) = ((reg) & 0xFFFFFFBF)                           )
//#define SSCR0_ECS_EXTERNAL_CLOCK(reg)             (   (reg) = ((reg) | 0x00000040)                           )
#define TOP_CTRL_SSE_DISABLE(reg)                    (   (reg) = ((reg) & ~BIT_0 ) )
#define TOP_CTRL_SSE_ENABLE(reg)                     (   (reg) = ((reg) | BIT_0 ))
#define TOP_CTRL_SSE_GET(reg)                        (   (reg) & BIT_0                               )
//#define SSCR0_SCR_SET(reg,SCRValue)               (   (reg) = ((reg) & 0xFFF000FF) | ((SCRValue & 0x00000FFF) << 8) )
//#define SSCR0_EDSS_GET(reg)                       (   ((reg) & 0x00100000) >> 20                             )
//#define SSCR0_EDSS_SET(reg)             		  (   (reg) = ((reg) | 0x00100000)                			 )
#define SSCR0_EDSS_CLEAR(reg)                     (   (reg) = ((reg) & 0xFFEFFFFF)                           )
//#define SSCR0_NCS_SET_NO_NETWORK(reg)             (   (reg) = ((reg) & 0xFFDFFFFF)                           )
//#define SSCR0_NCS_SET_NETWORK(reg)                (   (reg) = ((reg) | 0x00200000)                           )
//#define SSCR0_RIM_UNMASK(reg)             		  (   (reg) = ((reg) & 0xFFBFFFFF)     						 )
#define INT_EN_RIM_UNMASK(reg) 					(   (reg) = ((reg) & ~BIT_4)     						 )
//#define SSCR0_RIM_MASK(reg)             		  (   (reg) = ((reg) | 0x00400000)   						 )
#define INT_EN_RIM_MASK(reg) 					(   (reg) = ((reg) | BIT_4)     						 )

//#define SSCR0_TIM_UNMASK(reg)             		  (   (reg) = ((reg) & 0xFF7FFFFF)     						 )
//#define SSCR0_TIM_MASK(reg)             		  (   (reg) = ((reg) | 0x00800000)     						 )
#define INT_EN_TIM_UNMASK(reg) 					(   (reg) = ((reg) & ~BIT_5)     						 )
#define INT_EN_TIM_MASK(reg) 					(   (reg) = ((reg) | BIT_5)     						 )


//#define SSCR0_TIM_GET(reg)                        ((((reg) & 0x00800000) >> 23 ) 							 )
#define INT_EN_TIM_GET(reg) 					(   (reg) = ((reg) & BIT_5) >> 5    						 )


//#define SSCR0_FRDC_SET(reg,FRDCValue)             (   (reg) = ((reg) & 0xF8FFFFFF) | ((FRDCValue) << 24)     )
#define NET_WORK_CTRL_FRDC_SET(reg,FRDCValue)    (   (reg) = ((reg) & 0xFFFFFFF1) | ((FRDCValue) << 1)     )

//#define SSCR0_FPCKE_DISABLE(reg)                  (   (reg) = ((reg) & 0xDFFFFFFF )                          )
#define SSCR0_FPCKE_ENABLE(reg)                   (   (reg) = ((reg) | 0x20000080 )                          )

//#define SSCR0_ACS_CLEAR(reg)                      (   (reg) = ((reg) & 0xBFFFFFFF)                           )
//#define SSCR0_ACS_SET(reg)                        (   (reg) = ((reg) | 0x40000000)                           )
//#define SSCR0_NORMAL_MODE(reg)                    (   (reg) = ((reg) & 0x7FFFFFFF)                           )
//#define SSCR0_NETWORK_MODE(reg)                   (   (reg) = ((reg) | 0x80000000)                           )

                           
  						


/* SSCR1 Macros:  */
//#define SSCR1_RIE_DISABLE(reg)                    (   (reg) = ((reg) & 0xFFFFFFFE)                           )
#define INT_EN_RIE_DISABLE(reg)                    (   (reg) = ((reg) & ~BIT_2)                           )
//#define SSCR1_RIE_ENABLE(reg)                     (   (reg) = ((reg) | 0x00000001)                           )
#define INT_EN_RIE_ENABLE(reg)                    (   (reg) = ((reg) | BIT_2)                           )

//#define SSCR1_TIE_DISABLE(reg)                    (   (reg) = ((reg) & 0xFFFFFFFD)                           )
#define INT_EN_TIE_DISABLE(reg)                    (   (reg) = ((reg) & ~BIT_3)                           )
//#define SSCR1_TIE_ENABLE(reg)                     (   (reg) = ((reg) | 0x00000002)                           )
#define INT_EN_TIE_ENABLE(reg)                    (   (reg) = ((reg) | BIT_3)                           )

//#define SSCR1_LBM_DISABLE(reg)                    (   (reg) = ((reg) & 0xFFFFFFFB)                           )
#define TOP_CTRL_LBM_DISABLE(reg)                    (   (reg) = ((reg) & ~BIT_12)                           )
//#define SSCR1_LBM_ENABLE(reg)                     (   (reg) = ((reg) | 0x00000004)                           )
#define TOP_CTRL_LBM_ENABLE(reg)                     (   (reg) = ((reg) | BIT_12)                           )
  
//#define SSCR1_SPO_SET(reg,SPOValue)               (   (reg) = ((reg) & 0xFFFFFFF7) | ((SPOValue) << 3)       )
#define TOP_CTRL_SPO_SET(reg,SPOValue)               (   (reg) = ((reg) & ~BIT_10) | ((SPOValue) << 10)       )
//#define SSCR1_SPH_SET(reg,SPHValue)               (   (reg) = ((reg) & 0xFFFFFFEF) | ((SPHValue) << 4)       )
#define TOP_CTRL_SPH_SET(reg,SPHValue)               (   (reg) = ((reg) & ~BIT_11) | ((SPHValue) << 11)       )

//#define SSCR1_MWDS_SET(reg,MWDSValue)             (   (reg) = ((reg) & 0xFFFFFFDF) | ((MWDSValue) << 5)      )
//#define SSCR1_TFT_SET(reg,TFTValue)               (   (reg) = ((reg) & 0xFFFFFC3F) | ((TFTValue) << 6)       )
#define FIFO_CTRL_TFT_SET(reg,TFTValue)               (   (reg) = ((reg) & ~0x1f) | (TFTValue)       )
//#define SSCR1_RFT_SET(reg,RFTValue)               (   (reg) = ((reg) & 0xFFFFC3FF) | ((RFTValue) << 10)      )
#define FIFO_CTRL_RFT_SET(reg,RFTValue)               (   (reg) = ((reg) & ~(0x1f << 5) | ((RFTValue) << 5)      ))



//#define SSCR1_EFWR_SET(reg,EFWRValue)             (   (reg) = ((reg) & 0xFFFFBFFF) | ((EFWRValue) << 14)     )
#define FIFO_CTRL_EFWR_SET(reg,EFWRValue)             (   (reg) = ((reg) & ~BIT_18) | ((EFWRValue) << 18)     )


//#define SSCR1_STRF_SET(reg,STRFValue)             (   (reg) = ((reg) & 0xFFFF7FFF) | ((STRFValue) << 15)     )
#define FIFO_CTRL_STRF_SET(reg,STRFValue)             (   (reg) = ((reg) & ~BIT_19) | ((STRFValue) << 19)     )


//#define SSCR1_IFS_SET_ZERO(reg)                  (   (reg) = ((reg) & 0xFFFEFFFF)                           )
//#define SSCR1_IFS_SET_ONE(reg)                   (   (reg) = ((reg) | 0x00010000)                           )


//#define SSCR1_PINTE_DISABLE(reg)                  (   (reg) = ((reg) & 0xFFFBFFFF)                           )
#define INT_EN_PINTE_DISABLE(reg)                  (   (reg) = ((reg) & ~BIT_0)                           )

//#define SSCR1_PINTE_ENABLE(reg)                   (   (reg) = ((reg) | 0x00040000)                           )
//#define SSCR1_TINTE_DISABLE(reg)                  (   (reg) = ((reg) & 0xFFF7FFFF)                           )
#define INT_EN_TINTE_DISABLE(reg)                  (   (reg) = ((reg) & ~BIT_1)                           )

//#define SSCR1_TINTE_ENABLE(reg)                   (   (reg) =  ((reg) | 0x00080000)                           )
#define INT_EN_TINTE_ENABLE(reg)                  (   (reg) = ((reg) | BIT_1)                           )

//#define SSCR1_RSRE_DISABLE(reg)                   (   (reg) = ((reg) & 0xFFEFFFFF)                           )
#define FIFO_CTRL_RSRE_DISABLE(reg)                   (   (reg) = ((reg) & ~BIT_11)                           )
//#define SSCR1_RSRE_ENABLE(reg)                    (   (reg) = ((reg) | 0x00100000)                           )
#define FIFO_CTRL_RSRE_ENABLE(reg)                   (   (reg) = ((reg) | BIT_11)                           )

//#define SSCR1_TSRE_DISABLE(reg)                   (   (reg) = ((reg) & 0xFFDFFFFF)                           )
//#define SSCR1_TSRE_ENABLE(reg)                    (   (reg) = ((reg) | 0x00200000)                           )
#define FIFO_CTRL_TSRE_DISABLE(reg)                   (   (reg) = ((reg) & ~BIT_10)                           )
#define FIFO_CTRL_TSRE_ENABLE(reg)                   (   (reg) = ((reg) | BIT_10)                           )

//#define SSCR1_TRAIL_PROCESSOR(reg)                (   (reg) = ((reg) & 0xFFBFFFFF)                           )
//#define SSCR1_TRAIL_DMA(reg)                      (   (reg) = ((reg) | 0x00400000)                           )
#define TOP_CTRL_TRAIL_PROCESSOR(reg)               (   (reg) = ((reg) & ~BIT_13)        )
#define TOP_CTRL_TRAIL_DMA(reg)               		(   (reg) = ((reg) | BIT_13)        )




//#define SSCR1_RWOT_DISABLE(reg)                   (   (reg) = ((reg) & 0xFF7FFFFF)                           )
//#define SSCR1_RWOT_ENABLE(reg)                    (   (reg) = ((reg) | 0x00800000)                           )
#define RWOT_CTRL_DISABLE(reg)                     (   (reg) = ((reg) & ~BIT_0)        )
#define RWOT_CTRL_ENABLE(reg)                     (   (reg) = ((reg) | BIT_0)        )

//#define SSCR1_SFRMDIR_MASTER(reg)                 (   (reg) = ((reg) & 0xFEFFFFFF)                           )
//#define SSCR1_SFRMDIR_SLAVE(reg)                  (   (reg) = ((reg) | 0x01000000)                           )
#define TOP_CTRL_SFRMDIR_MASTER(reg)               (   (reg) = ((reg) & ~BIT_4)        )
#define TOP_CTRL_SFRMDIR_SLAVE(reg)               (   (reg) = ((reg) | BIT_4)        )

//#define SSCR1_SCLKDIR_MASTER(reg)                 (   (reg) = ((reg) & 0xFDFFFFFF)                           )
//#define SSCR1_SCLKDIR_SLAVE(reg)                  (   (reg) = ((reg) | 0x02000000)                           )
#define TOP_CTRL_SCLKDIR_MASTER(reg)               (   (reg) = ((reg) & ~BIT_3)        )
#define TOP_CTRL_SCLKDIR_SLAVE(reg)               (   (reg) = ((reg) | BIT_3)        )


//#define SSCR1_ECRB_DISABLE(reg)                   (   (reg) = ((reg) & 0xFBFFFFFF)                           )
//#define SSCR1_ECRB_ENABLE(reg)                    (   (reg) = ((reg) | 0x04000000)                           )
//#define SSCR1_ECRA_DISABLE(reg)                   (   (reg) = ((reg) & 0xF7FFFFFF)                           )
//#define SSCR1_ECRA_ENABLE(reg)                    (   (reg) = ((reg) | 0x08000000)                           )

//#define SSCR1_SCFR_SET_ZERO(reg)                  (   (reg) = ((reg) & 0xEFFFFFFF)                           )
//#define SSCR1_SCFR_SET_ONE(reg)                   (   (reg) = ((reg) | 0x10000000)                           )
#define TOP_CTRL_SCFR_SET_ZERO(reg)                  (   (reg) = ((reg) & ~BIT_16)                           )
#define TOP_CTRL_SCFR_SET_ONE(reg)                   (   (reg) = ((reg) | BIT_16)                           )

 
//#define SSCR1_EBCEI_DISABLE(reg)                  (   (reg) = ((reg) & 0xDFFFFFFF)                           )
//#define SSCR1_EBCEI_ENABLE(reg)                   (   (reg) = ((reg) | 0x20000000)                           )
#define INT_EN_EBCEI_DISABLE(reg)                  (   (reg) = ((reg) & ~BIT_6)                           )
#define INT_EN_EBCEI_ENABLE(reg)                   (   (reg) = ((reg) | BIT_6)                           )

//#define SSCR1_TTE_SET_NOT_TRISTATED(reg)          (   (reg) = ((reg) & 0xBFFFFFFF)                           )
//#define SSCR1_TTE_SET_TRISTATED(reg)              (   (reg) = ((reg) | 0x40000000)                           )
#define TOP_CTRL_TTE_SET_NOT_TRISTATED(reg)                  (   (reg) = ((reg) & ~BIT_17)                           )
#define TOP_CTRL_TTE_SET_TRISTATED(reg)                   (   (reg) = ((reg) | BIT_17)                           )


//#define SSCR1_TTELP_SET_TO_SAME_CLK(reg)          (   (reg) = ((reg) & 0x7FFFFFFF)                           )
//#define SSCR1_TTELP_SET_TO_HALF_CLK(reg)          (   (reg) = ((reg) | 0x80000000)                           )
#define TOP_CTRL_TTELP_SET_TO_SAME_CLK(reg)                  (   (reg) = ((reg) & ~BIT_18)                           )
#define TOP_CTRL_TTELP_SET_TO_HALF_CLK(reg)                   (   (reg) = ((reg) | BIT_18)                           )



/* SSSR Macros:  */
/*write 0x1 to make bit clear also clear the interrupt*/
#define SSSR_TNF_GET(reg)                         (   ((reg) & BIT_6) >> 6                              )
#define SSSR_RNE_GET(reg)                         (   ((reg) & BIT_14) >> 14                              )
#define SSSR_BSY_GET(reg)                         (   (reg) & BIT_0                                )
#define SSSR_TFS_GET(reg)                         (   ((reg) & BIT_5) >> 5                              )
#define SSSR_RFS_GET(reg)                         (   ((reg) & BIT_13) >> 13                              )
#define SSSR_ROR_GET(reg)                         (   ((reg) & BIT_20) >> 20                              )
#define SSSR_ROR_CLEAR(reg)                       (    (reg) = ((reg) | BIT_20)                          )
#define SSSR_TFL_GET(reg)                         (   ((reg) & 0x00000F80) >> 7                              )
#define SSSR_RFL_GET(reg)                         (   ((reg) & 0x000f8000) >> 15                             )


#define SSSR_PINT_GET(reg)                        (   ((reg) & BIT_2) >> 2                             )
#define SSSR_PINT_CLEAR(reg)                      (   ((reg) &= ~BIT_2)                                   )
#define SSSR_TINT_GET(reg)                        (   ((reg) & BIT_3) >> 3                             )
#define SSSR_TINT_CLEAR(reg)                      (   ((reg) &= ~BIT_3)                                   )
#define SSSR_EOC_GET(reg)                         (   ((reg) & BIT_4) >> 4                             )
#define SSSR_EOC_CLEAR(reg)                       (   ((reg) &= ~BIT_4)                                   )
#define SSSR_TUR_GET(reg)                         (   ((reg) & BIT_12) >> 12                             )
#define SSSR_TUR_CLEAR(reg)                       (   ((reg) &= ~BIT_12)                                   )

#define SSSR_CSS_GET(reg)                         (   ((reg) & BIT_1) >> 1                             )
#define SSSR_BCE_GET(reg)                         (   ((reg) & BIT_21) >> 21                             )
#define SSSR_BCE_CLEAR(reg)                       (   ((reg) &= ~BIT_21)                                   )


//#define SSCR1_TINTE_GET(reg)                      (   ((reg) & 0x00080000) >> 19                             )
#define INT_EN_TINTE_GET(reg)                  (   ((reg) & BIT_1) >> 1                           )

//#define SSCR1_TIE_GET(reg)						  (   ((reg) & 0x00000002) >> 1                              )
#define INT_EN_TIE_GET(reg)                  (   ((reg) & BIT_3) >> 3                           )
//#define SSCR1_RIE_GET(reg)                        (   ((reg) & 0x00000001) >> 0                              )
#define INT_EN_RIE_GET(reg)                  (   ((reg) & BIT_2) >> 2                           )


//#define SSSR_ALL_CLEAR(reg)                       (   (reg) = ((reg) | 0x00BC0080)                           )
#define SSSR_ALL_CLEAR(reg)                       (   (reg) = ((reg) | 0x0030101C)                           )
                         

#endif /* _SSP_HW_H_ */


