#ifndef __DMA_H__
#define __DMA_H__

#define DMA_MAX_CHANNELS_NUM    32


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
#define SHIFT0(Val)  (Val)
#define SHIFT1(Val)  ((Val) << 1)
#define SHIFT2(Val)  ((Val) << 2)
#define SHIFT3(Val)  ((Val) << 3)
#define SHIFT4(Val)  ((Val) << 4)
#define SHIFT5(Val)  ((Val) << 5)
#define SHIFT6(Val)  ((Val) << 6)
#define SHIFT7(Val)  ((Val) << 7)
#define SHIFT8(Val)  ((Val) << 8)
#define SHIFT9(Val)  ((Val) << 9)
#define SHIFT10(Val) ((Val) << 10)
#define SHIFT11(Val) ((Val) << 11)
#define SHIFT12(Val) ((Val) << 12)
#define SHIFT13(Val) ((Val) << 13)
#define SHIFT14(Val) ((Val) << 14)
#define SHIFT15(Val) ((Val) << 15)
#define SHIFT16(Val) ((Val) << 16)
#define SHIFT17(Val) ((Val) << 17)
#define SHIFT18(Val) ((Val) << 18)
#define SHIFT19(Val) ((Val) << 19)
#define SHIFT20(Val) ((Val) << 20)
#define SHIFT21(Val) ((Val) << 21)
#define SHIFT22(Val) ((Val) << 22)
#define SHIFT23(Val) ((Val) << 23)
#define SHIFT24(Val) ((Val) << 24)
#define SHIFT25(Val) ((Val) << 25)
#define SHIFT26(Val) ((Val) << 26)
#define SHIFT27(Val) ((Val) << 27)
#define SHIFT28(Val) ((Val) << 28)
#define SHIFT29(Val) ((Val) << 29)
#define SHIFT30(Val) ((Val) << 30)
#define SHIFT31(Val) ((Val) << 31)


typedef struct{
    volatile unsigned int DDADR;                    /* 0x200 DMA Descriptor Address Register [] */
    volatile unsigned int DSADR;                    /* 0x204 DMA Source Address Register [] */
    volatile unsigned int DTADR;                    /* 0x208 DMA Target Address Register [] */
    volatile unsigned int DCMD;                     /* 0x20C DMA Command Register [] */
}DMA_Descriptor_TypeDef;

typedef struct{
    volatile unsigned int DCSR[DMA_MAX_CHANNELS_NUM];                 /* 0x0000  */
    
    volatile unsigned int RESERVED0[(0xa0-0x7c)/4-1];
    
    volatile unsigned int DALGN;                    /* 0x00A0 */
    volatile unsigned int DPCSR;                    //0xA4
    volatile unsigned int RESERVED1[14];
    
    volatile unsigned int DQSR0;                    /* 0x00E0 */
    volatile unsigned int DQSR1;
    volatile unsigned int DQSR2;
    
    volatile unsigned int RESERVED2;
    
    volatile unsigned int DINT;                     /* 0x00F0 */
    
    volatile unsigned int RESERVED3[3];
    
    volatile unsigned int DRCMR1[64];                /* 0x0100 - 0x1FC */
    volatile DMA_Descriptor_TypeDef DMA_DESC[DMA_MAX_CHANNELS_NUM]; /* DMA Descriptor List 0x200 - 0x3FC */
    volatile unsigned int  RESERVED4[(0x1100-0x3fc)/4-1];
    volatile unsigned int DRCMR2[36];                /* 0x1100 - 0x118C */
 
}DMA_TypeDef;

#define    DMA_BASE                     0xD4000000                              
#define    DMA  (( DMA_TypeDef *) DMA_BASE )

/*	DMA Channel Control/Status Registers 0-15 (DCSRx) */
#define	CSR_RUN					BIT_31			/* Run */
#define	CSR_NODESCFETCH			BIT_30			/* No-Descriptor Fetch */
#define	CSR_STOPIRQEN			BIT_29			/* Stop interrupt enabled */
#define	CSR_EORIRQEN            BIT_28		    /* Setting the End-of-Receive interrupt enable */	
#define	CSR_EORJMPEN            BIT_27		    /* Jump to the next descriptor on EOR */	
#define	CSR_EORSTOPEN			BIT_26			/* Stop channel on EOR */
#define	CSR_SETCMPST            BIT_25		    /* Set descriptor compare status */	
#define	CSR_CLRCMPST            BIT_24			/* Clear descriptor compare status */
#define	CSR_RASIRQEN            BIT_23			/* Request after channel stopped interrupt enable */
#define	CSR_MASKRUN				BIT_22			/* MASKRUN */
#define	CSR_CMPST				BIT_10			/* Descriptor compare status */
#define	CSR_EORINT				BIT_9			/* End of Receive Interrupt */
#define	CSR_REQPEND				BIT_8			/* Request pending */
#define	CSR_RASINTR				BIT_4			/* Request after channel stopped */
#define	CSR_STOPINTR            BIT_3			/* Stop interrupt */
#define	CSR_ENDINTR				BIT_2			/* End interrupt */
#define	CSR_STARTINTR			BIT_1			/* Start interrupt */
#define	CSR_BUSERRINTR			BIT_0			/* Bus error interrupt */


/* Alignment control for channel x */
#define	ALGN_DALGN_MSK                  SHIFT0(0xffff)	
#define	ALGN_DALGN_BASE			        0


/*	DMA_PCSR	0x00A4	DMA Programmed I/O Control Status Register */
/* Activate posted writes and split reads */
#define	PCSR_BRGSPLIT			        BIT_31	
#define	PCSR_BRGBUSY			        BIT_0				/* Bridge busy status */


/* Clearing pending request */
#define	RQSR0_CLR                       BIT_8				
/*		Bit(s) DMA_RQSR0_RSRV_7_5 reserved */
#define	RQSR0_REQPEND_MSK               SHIFT0(0x1f)		/* Request pending */
#define	RQSR0_REQPEND_BASE              0

/*	DMA_INT		0x00F0	DMA Interrupt Register */
#define	INT_CHLINTR_MSK                 SHIFT0(0xffff)	/* Channel interrupt */
#define	INT_CHLINTR_BASE		        0

/*		Bit(s) DMA_RCMR_RSRV_31_8 reserved */
#define	RCMR_MAPVLD				        BIT_7				/* Map valid channel */
/*		Bit(s) DMA_RCMR_RSRV_6_5 reserved */
#define	RCMR_CHLNUM_MSK			        SHIFT0(0x1f)		/* Channel number */
#define	RCMR_CHLNUM_BASE                0


/* Descriptor address */
#define	DADR_DESCRIPTOR_ADDRESS_MSK		SHIFT4(0xfffffff)	
#define	DADR_DESCRIPTOR_ADDRESS_BASE    4
/*		Bit(s) DMA_DADR_RSRV_3_2 reserved */
/* Enable Descriptor Branch */
#define	DADR_BREN                       BIT_1				
#define	DADR_STOP						BIT_0				/* Stop */

/*	DMA_SADRx	0x0204	DMA Source Address Registers 0-15 */
#define	SADR_SRCADDR_MSK		        SHIFT3(0x1fffffff)	/* Source address */
#define	SADR_SRCADDR_BASE		        3
#define	SADR_SRCADDR2			        BIT_2				/* SRCADDR2 */
#define	SADR_SRCADDR0_MSK		        SHIFT0(0x3)			/* SRCADDR0 */
#define	SADR_SRCADDR0_BASE		        0


/*	DMA_TADRx	0x0208	DMA Target Address Registers 0-15 */
#define	TADR_TRGADDR_MSK		        SHIFT3(0x1fffffff)	/* Target address */
#define	TADR_TRGADDR_BASE		        3
#define	TADR_TRGADDR2			        BIT_2				/* TRGADDR2 */
#define	TADR_TRGADDR0_MSK		        SHIFT0(0x3)			/* TRGADDR0 */
#define	TADR_TRGADDR0_BASE		        0


/*	DMA_CMDx	0x020C	DMA Command Registers 0-15 */
#define	CMD_INCSRCADDR				    BIT_31			    /* Source address increment */
#define	CMD_INCTRGADDR				    BIT_30			    /* Target address increment */
#define	CMD_FLOWSRC				        BIT_29			    /* Source flow control */
#define	CMD_FLOWTRG				        BIT_28			    /* Target flow control */
/*		Bit(s) DMA_CMD_RSRV_27_26 reserved */
/* Descriptor compare enable */
#define	CMD_CMPEN					    BIT_25			
/*		Bit(s) DMA_CMD_RSRV_24 reserved */
#define	CMD_ADDRMODE				    BIT_23			    /* ADDRMODE */
#define	CMD_STARTIRQEN				    BIT_22			    /* STARTIRQEN */
#define	CMD_ENDIRQEN				    BIT_21			    /* End interrupt enable */
/*		Bit(s) DMA_CMD_RSRV_20_19 reserved */                   
#define	CMD_SIZE_MSK				    SHIFT16(0x7)	    /* Maximum burst size */
#define	CMD_SIZE_BASE				    16                  
#define	CMD_WIDTH_MSK				    SHIFT14(0x3)	    /* WIDTH */
#define	CMD_WIDTH_BASE				    14
/*		Bit(s) DMA_CMD_RSRV_13 reserved */
/* Length of the transfer in bytes */
#define	CMD_LEN_MSK				        SHIFT0(0x1fff)	
#define	CMD_LEN_BASE				    0

#endif
