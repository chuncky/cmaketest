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

/************************************************************************/
/*                                                                      */
/* Title: Interrupt Controller Sources (including GPIO)                 */
/*                                                                      */
/* Filename: Intc_list.h                                                */
/*                                                                      */
/* Author: Yossi Gabay                                                  */
/*         Eilam Ben-Dror                                               */
/*                                                                      */
/* Target, subsystem: Common Platform, HAL                              */
/*																		*/
/*	Yaeli Karni	7 March 06,	Add Tavor_Boerne interrupts and changes		*/
/************************************************************************/
#ifndef _INTC_LIST_H_
#define _INTC_LIST_H_


// Callback function prototype for the interrupt handlers
// source - the interrupt source number that invoke the interrupt
typedef void (*INTC_ISR)(INTC_InterruptInfo interruptInfo);

typedef enum
{
// ------------ START OF TTC XIRQ SECONDARY INTERRUPT CONTROLLER -------------------------
	/* S/W interrupts , initiated by TCU Module , 16 interrupts  */
	INTC_SRC_S_START,
	SW_INT_SRC_0 = INTC_SRC_S_START,
	SW_INT_SRC_1    ,
	SW_INT_SRC_2    ,
	SW_INT_SRC_3    ,
	SW_INT_SRC_4    ,
	SW_INT_SRC_5    ,
	SW_INT_SRC_6    ,
	SW_INT_SRC_7    ,
	SW_INT_SRC_8    ,
	SW_INT_SRC_9    ,
	SW_INT_SRC_10    ,
	SW_INT_SRC_11    ,
	SW_INT_SRC_12    ,
	SW_INT_SRC_13    ,
	SW_INT_SRC_14    ,
	SW_INT_SRC_15    ,
	/* Internal Interrupts (or register interrupts ) [16:23] , 8 possible sources*/
	FirstRI_INT      ,
	RI_INT_SRC_0 = FirstRI_INT,
	RI_INT_SRC_1     ,
	RI_INT_SRC_2     ,
	RI_INT_SRC_3     ,
	RI_INT_SRC_4     ,
	RI_INT_SRC_5     ,
	RI_INT_SRC_6     ,
	RI_INT_SRC_7     ,
	/* Cross-connection interrupt*/
	XSWI_INT_SRC_0   ,
	/* iNTERNAL EXCEPTION 3 INTERRUPTS*/
	EIRQ_INT_0       ,
	EIRQ_INT_1       ,
	EIRQ_INT_2       ,
	/* HW interrupts - sourced by GB peripherals (including the Modem) 48 possible interrupts*/
	FIRST_HW_INT     ,
	///////////////////xIRQ HW begin ///////////////////////////////	
	INTC_SRC_ARM_DMA  = FIRST_HW_INT,         /* HW IRQ [0] */  
	INTC_RIPC_1,                              /* HW IRQ [1] */  
	INTC_GB_AXIS_ARM_HINT,                    /* HW IRQ [2] */  
	INTC_HW_RESERVED0,                        /* HW IRQ [3] */  
	INTC_AP_ICU_SRC_0,                        /* HW IRQ [4] */  
	INTC_I2C_UERR_FLAG,                       /* HW IRQ [5] */  
	INTC_I2C_CERR_FLAG,                       /* HW IRQ [6] */  
	INTC_I2C_PA_ERR_FLAG,                     /* HW IRQ [7] */  
	INTC_HW_DMA_C2_0,         /* HW IRQ [8] */
	INTC_HW_DMA_C2_1,         /* HW IRQ [9] */
	INTC_HW_DMA_C2_2,         /* HW IRQ [10] */
	INTC_HW_DMA_C2_3,         /* HW IRQ [11] */
	INTC_HW_DMA_C2_4,         /* HW IRQ [12] */
	INTC_WB_RX_TIME_SIG13 = INTC_HW_DMA_C2_4,  /* HW IRQ [12] */
	INTC_HW_DMA_C2_5,         /* HW IRQ [13] */
	INTC_WB_TX_TIME_SIG13 = INTC_HW_DMA_C2_5,  /* HW IRQ [13] */
	INTC_HW_DMA_C2_6,         /* HW IRQ [14] */
	INTC_WB_IRAT_TIME_SIG13 = INTC_HW_DMA_C2_6,  /* HW IRQ [14] */
	INTC_HW_DMA_C2_7,         /* HW IRQ [15] */
	INTC_HW_SLOW_CLK_SNAP,    /* HW IRQ [16] */    
	INTC_HW_EarlyWKUP,        /* HW IRQ [17] */    
	INTC_HW_USIM2,            /* HW IRQ [18] */    
	INTC_HW_GSSP_1,           /* HW IRQ [19] */    
	INTC_HW_PMIC_INT_WKUP,    /* HW IRQ [20] */    
	INTC_AP_ICU_SRC_1,                        /* HW IRQ [21] */    
	INTC_HW_EXTPAD_1,         /* HW IRQ [22] */ //For external Interrupt
	INTC_HW_EXTPAD_2,         /* HW IRQ [23] */ //For external Interrupt
	INTC_HW_USIM1, INTC_HW_USIM = INTC_HW_USIM1, INTC_SRC_USIM = INTC_HW_USIM1,      /* HW IRQ [24] */
	INTC_HW_I2C,              /* HW IRQ [25] */ //Slow TWSI
	INTC_HW_DSP2CPU_0,  INTC_SRC_IPC_DATA_ACK =  INTC_HW_DSP2CPU_0,     /* HW IRQ [26] */  // INTC_SRC_IPC_DATA_ACK, INTC_SRC_IPC_RD, INTC_SRC_IPC_ACK, INTC_SRC_IPC_GP
	INTC_HW_DSP2CPU_1,  INTC_SRC_IPC_RD =  INTC_HW_DSP2CPU_1,    /* HW IRQ [27] */  // INTC_HW_IpcDatBuffAck
	INTC_HW_DSP2CPU_2,  INTC_SRC_IPC_ACK =  INTC_HW_DSP2CPU_2,   /* HW IRQ [28] */ 
	INTC_HW_DSP2CPU_3,  INTC_SRC_IPC_GP = INTC_HW_DSP2CPU_3,       /* HW IRQ [29] */ 
	INTC_AP_ICU_SRC_2,                        /* HW IRQ [30] */
	INTC_HW_CPMU,             /* HW IRQ [31] */
	INTC_HW_TCU_EXCP,         /* HW IRQ [32] */
	INTC_HW_RTU_0    ,        /* HW IRQ [33]*/
	INTC_HW_RTU_1    ,        /* HW IRQ [34]*/
	INTC_HW_RTU_2    ,        /* HW IRQ [35]*/
	INTC_HW_Tmr1     ,        /* HW IRQ [36] Timer1 */
	INTC_HW_Tmr2     ,        /* HW IRQ [37] Timer2 */
	INTC_HW_PERFORMANCE_MONITOR,              /* HW IRQ [38] */
	INTC_HW_Tmr3 , INTC_HW_Tmr3BkUp = INTC_HW_Tmr3,        /* HW IRQ [39] Timer3 */
	INTC_HW_TmrWDT   ,        /* HW IRQ [40] Timer Watch Dog */
	INTC_HW_GPB_ECIPHER  ,       /* HW IRQ [41] */  //INTC_SRC_CIPHER, INTC_HW_ECIPHER
	INTC_HW_DTC_CH0,             /* HW IRQ [42] */ 
	INTC_LTE_TX_TIME, INTC_SRC_TSG1_TIME_NZ3 = INTC_LTE_TX_TIME,                       /* HW IRQ [43] */ 
	INTC_HW_DTC_CH1,             /* HW IRQ [44] */ 
	INTC_HW_APB_ECIPHER,         /* HW IRQ [45] */  //INTC_SRC_CIPHER, INTC_HW_ECIPHER
	INTC_LTE_RX_TIME, INTC_SRC_TSG0_TIME_NZ3 = INTC_LTE_RX_TIME,                        /* HW IRQ [46] */
	INTC_HW_DTC_ERROR,           /* HW IRQ [47] */
	INTC_WB_LLTX_MACE,INTC_HW_RTU_NZ3_0=INTC_WB_LLTX_MACE,INTC_HW_SDIO=INTC_WB_LLTX_MACE, /* HW IRQ [48] */  //INTC_CI2C_SCL_SDA_WAKE_UP
	INTC_GSM_WAKE_UP,            /* HW IRQ [49] */
	INTC_WBCDMA_WAKE_UP,    INTC_LTE_WAKE_UP = INTC_WBCDMA_WAKE_UP, INTC_SRC_TSG2_TIME_NZ3 = INTC_WBCDMA_WAKE_UP,INTC_TDCDMA_WAKE_UP = INTC_LTE_WAKE_UP,   /* HW IRQ [50] */
	INTC_CRXD_WAKE_UP,                        /* HW IRQ [51] */
	INTC_CGPIO_WAKE_UP,                       /* HW IRQ [52] */
	INTC_AP_ICU_SRC_3,                        /* HW IRQ [53] */
	INTC_GSSP_1_WAKE_UP,                      /* HW IRQ [54] */
	INTC_AP_ICU_SRC_4, INTC_SRC_USB_NZ3 = INTC_AP_ICU_SRC_4, /* HW IRQ [55] */
	INTC_I2C_WAKE_UP,                         /* HW IRQ [56] */
	INTC_HW_COMM_UART, INTC_COMM_UART = INTC_HW_COMM_UART, /* HW [57] Comm UART Interrupt*/
	INTC_LTE_Z3_WAKE_UP,            /* HW IRQ [58] */                  
		INTC_LTE_TD_WB_WAKE_UP_ = INTC_LTE_Z3_WAKE_UP,
	INTC_AC_IPC_0,               /* HW IRQ [59] AC - IPC [0]*/
	INTC_BB_SLEEP_ENTRY_OK,                   /* HW IRQ [60] */                  
	INTC_AC_IPC_1,               /* HW IRQ [61] AC - IPC [1]*/
	INTC_AC_IPC_2,               /* HW IRQ [62] AC - IPC [2]*/
	INTC_AC_IPC_3,               /* HW IRQ [63] AC - IPC [2]*/
	INTC_AP_ICU_SRC_5,						  /* HW IRQ [64] */
	INTC_AP_ICU_SRC_6,						  /* HW IRQ [65] */
	INTC_AP_ICU_SRC_7,						  /* HW IRQ [66] */
	INTC_AP_ICU_SRC_8,						  /* HW IRQ [67] */
	INTC_AP_ICU_SRC_9,						  /* HW IRQ [68] */
	INTC_AP_ICU_SRC_10,						  /* HW IRQ [69] */
	INTC_AP_ICU_SRC_11,						  /* HW IRQ [70] */
	INTC_AUDIO_IPC_0, 		     			  /* HW IRQ [71] */
	INTC_AUDIO_IPC_1, 					      /* HW IRQ [72] */
	INTC_AUDIO_IPC_2,					      /* HW IRQ [73] */
	INTC_AUDIO_IPC_3, 					      /* HW IRQ [74] */	
	INTC_DSP_WAKE_CPMU,						  /* HW IRQ [75] */ 
	INTC_AP_ICU_SRC_12, 					  /* HW IRQ [76] */ 
///// XIRQ HW end //////////////////////////////////////

	INTC_SRC_S_END,
    MAX_INTERRUPT_CONTROLLER_SOURCES = INTC_SRC_S_END,
// -----------------------------------END OF TTC XIRQ SECONDARY INTERRUPT CONTROLLER -------------------------------------------
// ------------- VIRTUAL INTERRUPT SOURCES --------------------------------------
	VIRTUAL_INT_1 = MAX_INTERRUPT_CONTROLLER_SOURCES,
	VIRTUAL_INT_2,
	VIRTUAL_INT_3,
	VIRTUAL_INT_4,
	INTC_SRC_VIRTUAL_END,
// ------------- END OF VIRTUAL INTERRUPT SOURCES --------------------------------------
// ------------- GPIO INTERRUPT SOURCES --------------------------------------
/*
   GPIO (second level) sources
   The following are virtual GPIO sources, i.e. they are all "or"ed to INTC_SRC_GPIO_COMBINED,
   but may be used as first level sources.
 */
#if defined(INTC_GPIO_01_WIRED_TO_ICU)
	// APP processors have GPIO#0 and #1 edge interrupts directly wired to primary interrupt controller
	// Therefore the below two entires are placeholders only
    INTC_SRC_GPIO_0_DUMMY = INTC_SRC_VIRTUAL_END,INTC_GPIO_SRC_START_PIN=INTC_SRC_GPIO_0_DUMMY,
    INTC_SRC_GPIO_1_DUMMY,
#else
	// Other implementations do not have directly wired GPIO #0 and #1 - normal entries below
	INTC_SRC_GPIO_0 = INTC_SRC_VIRTUAL_END,INTC_GPIO_SRC_START_PIN=INTC_SRC_GPIO_0,
       INTC_SRC_GPIO_1,
#endif

	INTC_SRC_GPIO_2,		INTC_SRC_GPIO_3,		INTC_SRC_GPIO_4,		INTC_SRC_GPIO_5,
	INTC_SRC_GPIO_6,		INTC_SRC_GPIO_7,		INTC_SRC_GPIO_8,		INTC_SRC_GPIO_9,
	INTC_SRC_GPIO_10,		INTC_SRC_GPIO_11,		INTC_SRC_GPIO_12,		INTC_SRC_GPIO_13,
	INTC_SRC_GPIO_14,		INTC_SRC_GPIO_15,		INTC_SRC_GPIO_16,		INTC_SRC_GPIO_17,
	INTC_SRC_GPIO_18,		INTC_SRC_GPIO_19,		INTC_SRC_GPIO_20,		INTC_SRC_GPIO_21,
	INTC_SRC_GPIO_22,		INTC_SRC_GPIO_23,		INTC_SRC_GPIO_24,		INTC_SRC_GPIO_25,
	INTC_SRC_GPIO_26,		INTC_SRC_GPIO_27,		INTC_SRC_GPIO_28,		INTC_SRC_GPIO_29,
	INTC_SRC_GPIO_30,		INTC_SRC_GPIO_31,		INTC_SRC_GPIO_32,		INTC_SRC_GPIO_33,
	INTC_SRC_GPIO_34,		INTC_SRC_GPIO_35,		INTC_SRC_GPIO_36,		INTC_SRC_GPIO_37,
	INTC_SRC_GPIO_38,		INTC_SRC_GPIO_39,		INTC_SRC_GPIO_40,		INTC_SRC_GPIO_41,
	INTC_SRC_GPIO_42,		INTC_SRC_GPIO_43,		INTC_SRC_GPIO_44,		INTC_SRC_GPIO_45,
	INTC_SRC_GPIO_46,		INTC_SRC_GPIO_47,		INTC_SRC_GPIO_48,		INTC_SRC_GPIO_49,
	INTC_SRC_GPIO_50,		INTC_SRC_GPIO_51,		INTC_SRC_GPIO_52,		INTC_SRC_GPIO_53,
	INTC_SRC_GPIO_54,		INTC_SRC_GPIO_55,		INTC_SRC_GPIO_56,		INTC_SRC_GPIO_57,
	INTC_SRC_GPIO_58,		INTC_SRC_GPIO_59,		INTC_SRC_GPIO_60,		INTC_SRC_GPIO_61,
       INTC_SRC_GPIO_62,       INTC_SRC_GPIO_63,

#if defined (GPIO_128_ENTRIES)
    // More GPIOs in Tavor_Boerne
													INTC_SRC_GPIO_64,		INTC_SRC_GPIO_65,
	INTC_SRC_GPIO_66,		INTC_SRC_GPIO_67,		INTC_SRC_GPIO_68,		INTC_SRC_GPIO_69,
	INTC_SRC_GPIO_70,		INTC_SRC_GPIO_71,		INTC_SRC_GPIO_72,		INTC_SRC_GPIO_73,
	INTC_SRC_GPIO_74,		INTC_SRC_GPIO_75,		INTC_SRC_GPIO_76,		INTC_SRC_GPIO_77,
	INTC_SRC_GPIO_78,		INTC_SRC_GPIO_79,		INTC_SRC_GPIO_80,		INTC_SRC_GPIO_81,
	INTC_SRC_GPIO_82,		INTC_SRC_GPIO_83,		INTC_SRC_GPIO_84,		INTC_SRC_GPIO_85,
	INTC_SRC_GPIO_86,		INTC_SRC_GPIO_87,		INTC_SRC_GPIO_88,		INTC_SRC_GPIO_89,
	INTC_SRC_GPIO_90,		INTC_SRC_GPIO_91,		INTC_SRC_GPIO_92,		INTC_SRC_GPIO_93,
	INTC_SRC_GPIO_94,		INTC_SRC_GPIO_95,		INTC_SRC_GPIO_96,		INTC_SRC_GPIO_97,
	INTC_SRC_GPIO_98,		INTC_SRC_GPIO_99,		INTC_SRC_GPIO_100,		INTC_SRC_GPIO_101,
	INTC_SRC_GPIO_102,		INTC_SRC_GPIO_103,		INTC_SRC_GPIO_104,		INTC_SRC_GPIO_105,
	INTC_SRC_GPIO_106,		INTC_SRC_GPIO_107,		INTC_SRC_GPIO_108,		INTC_SRC_GPIO_109,
	INTC_SRC_GPIO_110,		INTC_SRC_GPIO_111,		INTC_SRC_GPIO_112,		INTC_SRC_GPIO_113,
	INTC_SRC_GPIO_114,		INTC_SRC_GPIO_115,		INTC_SRC_GPIO_116,		INTC_SRC_GPIO_117,
	INTC_SRC_GPIO_118,		INTC_SRC_GPIO_119,		INTC_SRC_GPIO_120,		INTC_SRC_GPIO_121,
    INTC_SRC_GPIO_122,      INTC_SRC_GPIO_123,		INTC_SRC_GPIO_124,		INTC_SRC_GPIO_125,
	INTC_SRC_GPIO_126,		INTC_SRC_GPIO_127,
#endif

    INTC_MAX_PRIMARY_INTERRUPT_SOURCES,   /*implies, those source supported by INTC (not secondary)*/


//
// Primary interrupt controller sources (64)
//

//--------- ICU interrupt source -------------------------------------- 
		INTC_SRC_XIRQ = INTC_MAX_PRIMARY_INTERRUPT_SOURCES,  /*HW ICU[0] */ /* GSM Interrupt Controller */
		INTC_SRC_SSP_2,				  /*HW ICU[1] */
		INTC_SRC_SSP_1,				  /*HW ICU[2] */
		INTC_SRC_SSP_0,				  /*HW ICU[3] */
		INTC_SRC_PMIC,					  /*HW ICU[4] */
		INTC_SRC_RTC_1HZ,                         /*HW ICU[5] */
		INTC_SRC_RTC_ALARM,                     /*HW ICU[6] */
		INTC_SRC_I2C_AP,                            /*HW ICU[7] *//*TBD: high speed TWSI*/
		INTC_SRC_ICU_RESERVED1, //INTC_SRC_GPU,                                  /*HW ICU[8] *//*old: INTC_SRC_AP_RESERVED1*/
		INTC_SRC_KEYPAD,                           /*HW ICU[9] */
		INTC_SRC_USIM1, //                        /*HW ICU[10] */
		INTC_SRC_USIM2, //                     /*HW ICU[11] */
		INTC_SRC_1_WIRE_IF,                      /*HW ICU[12] */
		INTC_SRC_TIMER_AP1,                     /*HW ICU[13] */
		INTC_SRC_TIMER_AP2,                     /*HW ICU[14] */
		INTC_SRC_TIMER_AP3,                     /*HW ICU[15] */
		INTC_SRC_IPC_DATA_ACK_ICU,                 /*HW ICU[16] */
		INTC_SRC_IPC_RD_ICU,                      /*HW ICU[17] */
		INTC_SRC_IPC_ACK_ICU,                      /*HW ICU[18] */
		INTC_SRC_IPC_GP_ICU,                          /*HW ICU[19] */
		INTC_AC_IPC_AP_4,                          /*HW ICU[20] */
		INTC_AC_IPC_CP_0,                          /*HW ICU[21] */
		INTC_AC_IPC_CP_1,                          /*HW ICU[22] */
		INTC_AC_IPC_CP_2,                          /*HW ICU[23] */
		INTC_AC_IPC_CP_3,                         /*HW ICU[24] */
		INTC_AC_IPC_CP_4,                         /*HW ICU[25] */
		INTC_SRC_DDR,                                /*HW ICU[26] *//*TBD: L2 cache and DDR errors*/
		INTC_SRC_UART2,   //FAST               /*HW ICU[27] */
		INTC_SRC_UART3,   //FAST                /*HW ICU[28] */
		INTC_SRC_ICU_RESERVED4, //INTC_SRC_TIMER_AP2_1,                  /*HW ICU[29] */
		INTC_SRC_ICU_RESERVED5, //INTC_SRC_TIMER_AP2_2,                  /*HW ICU[30] */
		INTC_SRC_TIMER_CP1,                      /*HW ICU[31] */
		INTC_SRC_TIMER_CP2, INTC_SRC_TIMER_CP3=INTC_SRC_TIMER_CP2,		   /*HW ICU[32] */
		INTC_SRC_I2C_,                      /*HW ICU[33] */
		INTC_SRC_GSSP,                                  /*HW ICU[34] */
		INTC_SRC_WDT,                                   /*HW ICU[35] */
		INTC_SRC_MAIN_PMU,                          /*HW ICU[36] */
		INTC_SRC_FREQ_CHANGE_CP,                /*HW ICU[37] */
		INTC_SRC_FREQ_CHANGE_AP,                /*HW ICU[38] */
		INTC_SRC_MMC, INTC_SRC_MMC_SD=INTC_SRC_MMC,                 /*HW ICU[39] */
		INTC_SRC_AEU,                                              /*HW ICU[40] */
		INTC_SRC_HSI, //INTC_SRC_LCD,                                                /*HW ICU[41] */
		INTC_SRC_SDIO, //INTC_SRC_CI,                                                  /*HW ICU[42] */ /*CCIC*/
		INTC_SRC_ASI, //INTC_SRC_IRE,                                              /*HW ICU[43] */ /*old: INTC_SRC_ROTATION*/
		INTC_SRC_USB,                       // USB1.1 (PHY)   /*HW ICU[44] */
		INTC_SRC_NAND_IF,                                       /*HW ICU[45] */
		INTC_SRC_DMA_SQU,INTC_SRC_HIFI_DMA=INTC_SRC_DMA_SQU,    /*HW ICU[46] */
		INTC_SRC_DMA0,INTC_SRC_DMA=INTC_SRC_DMA0,    /*HW ICU[47] */
		INTC_SRC_DMA1,                                                         /*HW ICU[48] */
		INTC_SRC_GPIO_COMBINED_AP,                                 /*HW ICU[49] */
		INTC_SRC_ICU_RESERVED6, //INTC_SRC_TIMER_AP2_3,                                           /*HW ICU[50] */
		INTC_SRC_ICU_RESERVED7, //INTC_SRC_USB2,                     // USB2.0 (ULPI)           /*HW ICU[51] */
		INTC_SRC_IPC_SRV_CP_0,                                         /*HW ICU[52] */
		INTC_SRC_BATTERY_CHARGER_WAKEUP, //INTC_SRC_IPC_SRV_CP_1,                                          /*HW ICU[53] */
		INTC_SRC_I2C_CP,                                                       /*HW ICU[54] *//*TBD: TWSI*/
		INTC_SRC_GPIO_COMBINED_CP,                                  /*HW ICU[55] */
		INTC_SRC_IPC_SRV_AP_0,                                           /*HW ICU[56] */
		INTC_SRC_PERFORMANCE_COUNTER_CP, //INTC_SRC_IPC_SRV_AP_1,                                           /*HW ICU[57] */
		INTC_SRC_ICU_RESERVED8, //INTC_SRC_IPC_SRV_AP_2,                                           /*HW ICU[58] */
		INTC_SRC_UART1_SLOW,                                             /*HW ICU[59] */
		INTC_SRC_ICU_RESERVED9, //INTC_SRC_AP_PMU,                                                      /*HW ICU[60] */
		INTC_SRC_ICU_RESERVED10, //INTC_SRC_AXI_FABRIC_1_TO,                                    /*HW ICU[61] */
		INTC_SRC_AXI_FABRIC_1_TO, //INTC_SRC_AXI_FABRIC_2_TO,					        /*HW ICU[62] */
		INTC_SRC_SM,                                                             /*HW ICU[63] */
		INTC_SRC_ICU_RESERVED11, //                                                    /*HW ICU[64] */
		INTC_SRC_ICU_RESERVED12, //                                  /*HW ICU[65] */
		INTC_SRC_ICU_RESERVED13, //					        /*HW ICU[66] */
		INTC_SRC_ICU_RESERVED14,                                                             /*HW ICU[67] */
		INTC_SRC_ICU_DTC_CH0, // 								 /*HW ICU[68] */
		INTC_SRC_ICU_DTC_CH1, // 						/*HW ICU[69] */
		INTC_SRC_ICU_DTC_ERROR,															 /*HW ICU[70] */
		INTC_LTE_TD_WB_WAKE_UP = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+92,
		INTC_SRC_ICU_UART3 = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+95,
		INTC_SRC_ICU_AIRQ = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+97,
		INTC_SRC_ICU_SCS_SYNC = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+99,
		INTC_SRC_ARB_TIMEOUT1=INTC_MAX_PRIMARY_INTERRUPT_SOURCES + 103,
		INTC_SRC_ARB_TIMEOUT2=INTC_MAX_PRIMARY_INTERRUPT_SOURCES + 104,
		INTC_SRC_LTE_RX_TIME_SIG13 = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+112,
		INTC_SRC_LTE_TX_TIME_SIG13 = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+113,
		INTC_SRC_LTE_IRAT_TIME_SIG13 = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+114,
		INTC_SRC_ICU_KEY_PRESS = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+116,
		INTC_SRC_CR5_ERROR_RESP = INTC_MAX_PRIMARY_INTERRUPT_SOURCES+123,
		INTC_MAX_INTERRUPT_SOURCES = INTC_MAX_PRIMARY_INTERRUPT_SOURCES + 128,
// -------------------- END OF ICU INTERRUPT CONTROLLER  ----------------------

    //PHS_SW_DEMO_TTC, 20080828, For compile pass to Tavor P
    INTC_HW_MSL,
	INTC_HW_DMA_C3_0 ,
	INTC_HW_DMA_C3_1 ,
	INTC_HW_DMA_C3_2 ,
	INTC_HW_DMA_C3_3 ,
	INTC_HW_DMA_C3_4 ,
	INTC_HW_DMA_C3_5 ,
	INTC_HW_DMA_C3_6 ,
	INTC_HW_DMA_C3_7 ,
    INTC_HW_DSPP_3,
    INTC_HW_GSSP_2,
    INTC_HW_IpcDatBuffAck,
    INTC_HW_ECIPHER,
    INTC_HW_TCU_GPOB,
	INTC_HW_ADPC     ,
	INTC_HW_END_SLEEP_BKUP ,
    INTC_HW_TCU_CLK_EN   ,
    INTC_HW_CL_GB_HINT ,
    INTC_SRC_CIPHER ,
	INTC_BOERNE_CLK_CHG,        
	INTC_CRXD_32_28_EVENT,
	INTC_CRXD_27_0_EVENT,
	INTC_GSSP1_WAKE_UP,
	INTC_GSSP2_WAKE_UP,
	INTC_GSIM_UDET_UIO,
	INTC_CI2C_SCL_SDA_WAKE_UP,
	INTC_GB_DMA_C2,
	INTC_D2_MODEM_OK,
	INTC_HW_Tmr4BkUp,
	INTC_CI2C_SCL_SDA_WAKE_UP_DUMMY_I2C_2_TEST = INTC_MAX_INTERRUPT_SOURCES,                         
	INTC_SRC_DUMMY_I2C_2_TEST
                                 
}INTC_InterruptSources;          
                                 
                                 
// ---------------------------------------  ALIASES  -----------------------------------------------

#if defined (_TAVOR_BOERNE_) || defined(INTEL_2CHIP_PLAT_BVD)
#define	INTC_SRC_LCD1         INTC_MAX_PRIMARY_INTERRUPT_SOURCES
#define INTC_SRC_WDT          INTC_MAX_PRIMARY_INTERRUPT_SOURCES
#define INTC_SRC_LCDIF_READ   INTC_MAX_PRIMARY_INTERRUPT_SOURCES
#define INTC_SRC_GSM_INTERRUPT_SRC              INTC_SRC_XIRQ
#endif
//
#if defined(INTEL_2CHIP_PLAT_BVD)
#if defined (_TAVOR_BOERNE_)
	#define INTC_SRC_PMIC INTC_SRC_GPIO_83
#else
	#define INTC_SRC_PMIC INTC_SRC_GPIO_0
#endif
#endif

#if defined(_HERMON_B0_SILICON_) && !defined (INTEL_2CHIP_PLAT_BVD)
/* Make one definition for GSM Interrupt Controller source */
    #if defined(XIRQ_VIA_SINTC_DISABLE)
        #define INTC_SRC_GSM_INTERRUPT_SRC              INTC_SRC_SINTC_OR_XIRQ
    #else
        #define INTC_SRC_GSM_INTERRUPT_SRC              INTC_SRC_XIRQ
    #endif /*XIRQ_VIA_SINTC_DISABLE*/
	#define INTC_SRC_SINTC_OR_XIRQ INTC_SRC_SINTC
#endif//INTEL_2CHIP_PLAT_BVD

	#if defined(FLAVOR_APP)                 // ACIPC aliases for AP
		#define	INTC_AC_IPC_0 INTC_AC_IPC_AP_0
		#define	INTC_AC_IPC_1 INTC_AC_IPC_AP_1
		#define	INTC_AC_IPC_2 INTC_AC_IPC_AP_2
		#define	INTC_AC_IPC_3 INTC_AC_IPC_AP_3
		#define	INTC_AC_IPC_4 INTC_AC_IPC_AP_4
	#else                                   // ACIPC aliases for CP
		#define	INTC_AC_IPC_0 INTC_AC_IPC_CP_0
		#define	INTC_AC_IPC_1 INTC_AC_IPC_CP_1
		#define	INTC_AC_IPC_2 INTC_AC_IPC_CP_2
		#define	INTC_AC_IPC_3 INTC_AC_IPC_CP_3
		#define	INTC_AC_IPC_4 INTC_AC_IPC_CP_4
	#endif
		#define INTC_SRC_NON_DROWSY_OS_TIMER_0 INTC_SRC_TIMER_AP1
		#define INTC_SRC_NON_DROWSY_OS_TIMER_1 INTC_SRC_TIMER_AP2
		#define INTC_SRC_NON_DROWSY_OS_TIMER_2 INTC_SRC_TIMER_AP3
		#define INTC_SRC_OS_TIMER_0   INTC_SRC_TIMER_AP1//INTC_SRC_TIMER_CP1  //PHS_SW_DEMO_TTC
		#define INTC_SRC_OS_TIMER_1 INTC_SRC_TIMER_AP2//INTC_SRC_TIMER_CP2    //PHS_SW_DEMO_TTC
		#define INTC_SRC_OS_TIMER_2 INTC_SRC_TIMER_AP3//INTC_SRC_TIMER_CP3    //PHS_SW_DEMO_TTC
	#define	INTC_AC_IPC_WAKEUP INTC_AC_IPC_4

    //#if defined(FLAVOR_APP) && defined(FLAVOR_DUALCORE)
	#if 0
		#define	INTC_SRC_I2C INTC_SRC_I2C_AP
	#else
		#define	INTC_SRC_I2C INTC_SRC_I2C_CP
    #endif
	#if defined(SILICON_TTC_CORE_MOHAWK)
	    #define INTC_SRC_GPIO_COMBINED INTC_SRC_GPIO_COMBINED_AP
	#else
	    #define INTC_SRC_GPIO_COMBINED INTC_SRC_GPIO_COMBINED_CP
	#endif    
	#define INTC_SRC_DMEM INTC_SRC_DDR
	#define INTC_SRC_SINTC INTC_SRC_XIRQ

    #if !defined(SINTC_XIRQ) 
    // In case XIRQ is not used (APP case), COMM UART interrupt is available on P-ICU
    // TBD: not clear if COM side should use XIRQ - slower access
	#define INTC_COMM_UART INTC_SRC_UART1_SLOW
	#endif



#if defined(SILICON_TTC)
	#if defined(FLAVOR_APP)                 // ACIPC aliases for AP
		#define	INTC_AC_IPC_0 INTC_AC_IPC_AP_0
		#define	INTC_AC_IPC_1 INTC_AC_IPC_AP_1
		#define	INTC_AC_IPC_2 INTC_AC_IPC_AP_2
		#define	INTC_AC_IPC_3 INTC_AC_IPC_AP_3
		#define	INTC_AC_IPC_4 INTC_AC_IPC_AP_4
	#else                                   // ACIPC aliases for CP
		#define	INTC_AC_IPC_0 INTC_AC_IPC_CP_0
		#define	INTC_AC_IPC_1 INTC_AC_IPC_CP_1
		#define	INTC_AC_IPC_2 INTC_AC_IPC_CP_2
		#define	INTC_AC_IPC_3 INTC_AC_IPC_CP_3
		#define	INTC_AC_IPC_4 INTC_AC_IPC_CP_4
	#endif
		#define INTC_SRC_NON_DROWSY_OS_TIMER_0 INTC_SRC_TIMER_AP1
		#define INTC_SRC_NON_DROWSY_OS_TIMER_1 INTC_SRC_TIMER_AP2
		#define INTC_SRC_NON_DROWSY_OS_TIMER_2 INTC_SRC_TIMER_AP3
		#define INTC_SRC_OS_TIMER_0 INTC_SRC_TIMER_CP1
		#define INTC_SRC_OS_TIMER_1 INTC_SRC_TIMER_CP2
		#define INTC_SRC_OS_TIMER_2 INTC_SRC_TIMER_CP3
	#define	INTC_AC_IPC_WAKEUP INTC_AC_IPC_4

    //#if defined(FLAVOR_APP) && defined(FLAVOR_DUALCORE)
	#if 0
		#define	INTC_SRC_I2C INTC_SRC_I2C_AP
	#else
		#define	INTC_SRC_I2C INTC_SRC_I2C_CP
    #endif
	#if defined(SILICON_TTC_CORE_MOHAWK)
	    #define INTC_SRC_GPIO_COMBINED INTC_SRC_GPIO_COMBINED_AP
	#else
	    #define INTC_SRC_GPIO_COMBINED INTC_SRC_GPIO_COMBINED_CP
	#endif    
	#define INTC_SRC_DMEM INTC_SRC_DDR
	#define INTC_SRC_SINTC INTC_SRC_XIRQ

    #if !defined(SINTC_XIRQ) 
    // In case XIRQ is not used (APP case), COMM UART interrupt is available on P-ICU
    // TBD: not clear if COM side should use XIRQ - slower access
	#define INTC_COMM_UART INTC_SRC_UART1_SLOW
	#endif
#endif // SILICON_TTC

#define INTC_HW_MAC_E INTC_HW_SDIO
// in this file implementation TavorP and PV (both got _TAVOR_BOERNE_ define) have same value of MAX_INTERRUPT_CONTROLLER_SOURCES
// although different number of interrupt sources this macro is here to give the real number of interrupt exist in each product.
#if !defined (_TAVOR_BOERNE_)
    #define MAX_INTERRUPT_CONTROLLER_SOURCES_MACRO    (INTC_MAX_INTERRUPT_SOURCES - INTC_MAX_PRIMARY_INTERRUPT_SOURCES)
#else
    #define MAX_INTERRUPT_CONTROLLER_SOURCES_MACRO    (Sys_intcIsThirdRegisterExist() ? 94 : 53)
#endif

#endif /* _INTC_LIST_H_ */
