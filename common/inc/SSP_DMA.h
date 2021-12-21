//prepare for DMA HW access

#define DMA_HW_BASE_ADDRESS                         0xD4000000L
#define DMA_MAX_TRANSFER_LENGTH                     0x1FFF  //8192 bytes
#define DMA_NUMBER_OF_HW_CHANNEL_SUPPORTED      32
#define DMA_NUMBER_OF_DEVICES_SUPPORTED         64
#define DMA_NUMBER_OF_ADDED_DEVICES_SUPPORTED   36



#define DMA_INTERRUPT_NONE                  0x00L
#define DMA_INTERRUPT_TRANSFER_STARTED      0x01L
#define DMA_INTERRUPT_TRANSFER_ENDED        0x02L
#define DMA_INTERRUPT_TRANSFER_STOPPED      0x04L




typedef struct
{
    UINT32      DDADR;  // DMA Descriptor Address
    UINT32      DSADR;  // DMA Source Address
    UINT32      DTADR;  // DMA Target Address
    UINT32      DCMD;   // DMA Command Register
} DMA_ChannelDescHWRegisters;

typedef struct DMA_HardwareRegistersS
{
    UINT32                      DCSR[DMA_NUMBER_OF_HW_CHANNEL_SUPPORTED];   //Control/Status Reg.   0x4000 0000
    UINT32                      Reserved0[8];
    UINT32                      DALGN;                                      //Align Reg.            0x4000 00a0


	UINT32						DPCSR;                  					//Programmed I/O Control/Status Reg. 0x4000 00A4
	UINT32                      Reserved1[14];
	UINT32						DRQSR[3];              						// DREQ0/1/2 Status Reg. 0x4000 00E0
    UINT32                      Reserved2[1];

    UINT32                      InterruptPendingRegister;          			//Interrupt Pend. Reg.  0x4000 00f0
    UINT32                      Reserved3[3];

    /* DRCMR 0..63 - DMA Request to Channel Map Registers - DREQ 0/1, PREQ 0..61 mapping */
	/* in Borne IAS DRCMR23 is reserved*/
    UINT32                      DRCMR[DMA_NUMBER_OF_DEVICES_SUPPORTED];     //Req to Channl Map     0x4000 0100

    //Descriptor-DDADR, Source-DSADR, Target-DTADR, Command-DCMD
    DMA_ChannelDescHWRegisters  ChannelRegister[DMA_NUMBER_OF_HW_CHANNEL_SUPPORTED];   //          0x4000 0200

    UINT32                      Reserved5[0x340];

	UINT32    DRCMR_2[DMA_NUMBER_OF_ADDED_DEVICES_SUPPORTED];						/*Req to Channl Map  0x40001100-0x4000118c*/

} DMA_HardwareRegisters;

#define DMAIsChannelRunning(dmaChan)            (((DmaHwRegisters.DCSR[dmaChan] & (DCSR_BIT_RUN | DCSR_BIT_STOP)) \
                                                    == DCSR_BIT_RUN) ? TRUE : FALSE)

// read the current information inside the DMA (client must make sure that channel is STOP)
#define DMADescriptorAddressGet(dmaChan)      ((DmaHwRegisters.ChannelRegister[dmaChan].DDADR) & DDADR_BIT_MASK_VALUE)
#define DMADestinationAddressGet(dmaChan)     (DmaHwRegisters.ChannelRegister[dmaChan].DTADR)
#define DMASourceAddressGet(dmaChan)          (DmaHwRegisters.ChannelRegister[dmaChan].DSADR)

//ues dma channel 0 for ssp0 tx ,1 for ssp0 rx
#define  SSP2_TX_CHANNEL  		0
#define  SSP2_RX_CHANNEL  		1


// DCSR
#define DCSR_BIT_BUSERROR               0x00000001L
#define DCSR_BIT_START                  0x00000002L
#define DCSR_BIT_END                    0x00000004L
#define DCSR_BIT_STOP                   0x00000008L

#define DCSR_BIT_EOR_INT                0x00000200L
#define DCSR_BIT_EOR_STOP_EN            0x04000000L
#define DCSR_BIT_EOR_INT_EN			    0x10000000L

#define DCSR_BIT_STOP_EN                0x20000000L

#define DCSR_BIT_RUN                    0x80000000L
#define DCSR_BIT_DESC_MASK              0x40000000L
#define DCSR_BIT_NO_DESC                0x40000000L
#define DCSR_BIT_DESCRIPTOR_MODE        0x00000000L

// DCSR combinations
#define DCSR_BIT_INTR_CLEAR_MASK        (DCSR_BIT_BUSERROR | \
                                         DCSR_BIT_START | \
                                         DCSR_BIT_END | \
                                         DCSR_BIT_EOR_INT | \
                                         DCSR_BIT_STOP)

#define DCSR_BIT_INTR_READ_MASK         (DCSR_BIT_RUN | \
                                         DCSR_BIT_DESC_MASK | \
                                         DCSR_BIT_STOP_EN | \
                                         DCSR_BIT_EOR_STOP_EN | \
                                         DCSR_BIT_INTR_CLEAR_MASK)
                                         
#define DCSR_BIT_STOP_CHANNEL_MASK      (DCSR_BIT_INTR_READ_MASK & ~(DCSR_BIT_RUN))

// DCMD
#define DCMD_SHIFT_BITS_LENGTH          0
#define DCMD_SHIFT_BITS_DATA_WIDTH      14
#define DCMD_SHIFT_BITS_BURST_SIZE      16

#define DCMD_SHIFT_BITS_END_IRQ_EN      21
#define DCMD_SHIFT_BITS_START_IRQ_EN    22
#define DCMD_SHIFT_BITS_FLOW_TARGET     28
#define DCMD_SHIFT_BITS_FLOW_SOURCE     29
#define DCMD_SHIFT_BITS_INCR_TARGET     30
#define DCMD_SHIFT_BITS_INCR_SOURCE     31

#define CMD_SET_BITS_LENGTH(cmd,len)            ((cmd) |= ((len)  << DCMD_SHIFT_BITS_LENGTH))
#define CMD_SET_BITS_DATA_WIDTH(cmd,dw)         ((cmd) |= ((dw)   << DCMD_SHIFT_BITS_DATA_WIDTH))
#define CMD_SET_BITS_BURST_SIZE(cmd,bu)         ((cmd) |= ((bu)   << DCMD_SHIFT_BITS_BURST_SIZE))

#define CMD_SET_BITS_END_IRQ_EN(cmd,eirq)       ((cmd) |= ((eirq) << DCMD_SHIFT_BITS_END_IRQ_EN))
#define CMD_SET_BITS_START_IRQ_EN(cmd,sirq)     ((cmd) |= ((sirq) << DCMD_SHIFT_BITS_START_IRQ_EN))

#define CMD_SET_BITS_FLOW_TARGET(cmd,ft)        ((cmd) |= ((ft)   << DCMD_SHIFT_BITS_FLOW_TARGET))
#define CMD_SET_BITS_FLOW_SOURCE(cmd,fs)        ((cmd) |= ((fs)   << DCMD_SHIFT_BITS_FLOW_SOURCE))
#define CMD_SET_BITS_INCR_TARGET(cmd,inct)      ((cmd) |= ((inct) << DCMD_SHIFT_BITS_INCR_TARGET))
#define CMD_SET_BITS_INCR_SOURCE(cmd,incs)      ((cmd) |= ((UINT32)(incs) << DCMD_SHIFT_BITS_INCR_SOURCE))


/* In case of external-peripheral or Companion-chip, user must use DMA_ADDRESS_NOT_ALIGNED */
typedef enum
{
    DMA_ADDRESS_ALIGNED = 0, // Normal case
    DMA_ADDRESS_NOT_ALIGNED
} DMA_AddressAlignment;

typedef enum
{
    DMA_ADDRESS_INCREMENT_NONE = 0,
    DMA_ADDRESS_INCREMENT_SOURCE,
    DMA_ADDRESS_INCREMENT_TARGET,
    DMA_ADDRESS_INCREMENT_BOTH
}DMA_AddressIncrement;

typedef enum
{
    DMA_FLOW_CONTROL_NONE = 0,
    DMA_FLOW_CONTROL_SOURCE,
    DMA_FLOW_CONTROL_TARGET
}DMA_FlowControl;


/* Maximum burst size of each data transfer */
typedef enum
{
    DMA_BURST_INVALID = 0,
    DMA_BURST_8_BYTES,
    DMA_BURST_16_BYTES,
    DMA_BURST_32_BYTES
}DMA_BurstSize;

/* Width of the on-chip peripheral */
typedef enum
{
    DMA_MEMORY_WIDTH = 0,
    DMA_DATA_WIDTH_ONE_BYTE,
    DMA_DATA_WIDTH_TWO_BYTES,
    DMA_DATA_WIDTH_FOUR_BYTES
}DMA_DataWidth;

typedef struct
{
    UINT32                      interruptsEnableBits;
    DMA_AddressIncrement        addressIncrement;
    DMA_FlowControl             flowControl;
    DMA_BurstSize               burstSize;
    DMA_DataWidth               dataWidth;
	UINT32						totallength;
}DMA_DescriptorsHWConfig;

typedef enum
{
    DMA_RC_OK = 1,
    DMA_RC_GENERAL_ERROR = -100,
    DMA_RC_CHANNAL_INVALID,
    DMA_RC_CHANNEL_NOT_CLOSED,
    DMA_RC_CHANNEL_NOT_OPEN,
    DMA_RC_CHANNEL_NOT_STOPPED,
    DMA_RC_CHANNEL_MAPPING_ALREADY_IN_USE,
    DMA_RC_PERIHPERAL_ALREADY_MAPPED,
    DMA_RC_DESCRIPTOR_SIZE_NOT_ENOUGH,
    DMA_RC_BUFFER_ERROR,
    DMA_RC_BUFFER_NOT_ALIGNED,
    DMA_RC_SIZE_ERROR
}DMA_ReturnCode;

typedef void (*DMATransferErrorNotifyFn)(UINT32 channel, UINT32 callbackCookie);
typedef void (*DMATransferStartedNotifyFn)(UINT32 channel, UINT32 callbackCookie);
typedef void (*DMATransferEndedNotifyFn)(UINT32 channel, UINT32 callbackCookie);
typedef void (*DMATransferStoppedNotifyFn)(UINT32 channel, UINT32 callbackCookie);

// 4 callback func ,may we do not need so much 
typedef struct
{
    
    DMATransferErrorNotifyFn        errorNotifyFn;
    DMATransferStartedNotifyFn      startNotifyFn;
    DMATransferEndedNotifyFn        endNotifyFn;
    DMATransferStoppedNotifyFn      stopNotifyFn;
	UINT32 							callbackCookie;

} SSP2DMA_ChannelDatabase;





#define DmaHwRegisters      (*(volatile DMA_HardwareRegisters*)DMA_HW_BASE_ADDRESS)
#define SSP0_TX_PREQ		53
#define SSP0_RX_PREQ		52
#define SSP2_TX_PREQ		61
#define SSP2_RX_PREQ		60

#define DMA_WRITE_TO_DRCMR_REG(tper,tval)        (DmaHwRegisters.DRCMR[tper] = (tval))


#define DRCMR_BIT_VALID  						0x00000080L     

#define DMA_SET_ALIGN_BIT(dmaChan,adrAlign)                                                             \
            (                                                                                           \
                DmaHwRegisters.DALGN =                                                                  \
                    ((adrAlign) == DMA_ADDRESS_ALIGNED) ?                                               \
                        (DmaHwRegisters.DALGN & (~(0x01UL << (dmaChan))))                               \
                        :                                                                               \
                        (DmaHwRegisters.DALGN | (0x01UL << (dmaChan)))                                  \
            )

// Macro-API to start Running the channel
#define DMAChannelStart(dmaChan)                {UINT32 cpsr; \
                                                cpsr = disableInterrupts(); \
												DmaHwRegisters.DCSR[dmaChan] |= (DCSR_BIT_RUN); \
                                                restoreInterrupts(cpsr);}


#define DESCRIPTOR_MASK_BIT_ALIGN           0x0000000FL
#define BUFFER_MASK_BIT_ALIGN               0x00000007L

// Return if buffer is properly aligned - FALSE -> Not aligned.
#define DMAIsDescriptorAligned(buff)          ((((UINT32)buff) & DESCRIPTOR_MASK_BIT_ALIGN) ? FALSE : TRUE)
#define DMAIsBufferAligned(buff)              ((((UINT32)buff) & BUFFER_MASK_BIT_ALIGN) ? FALSE : TRUE)



int DMA_INT_Init(void);

int SSPDMAChannelRegister(UINT32 channel,
                                    DMATransferStartedNotifyFn startNotifyFn,
                                    DMATransferEndedNotifyFn endNotifyFn,
                                    DMATransferStoppedNotifyFn stopNotifyFn,
                                    DMATransferErrorNotifyFn errorNotifyFn,
                                    UINT32 callbackCookie);
int DMAPreq2ChannelMAP(void);

DMA_ReturnCode DMA_HW_SetUp(UINT32 channel, void *srcBaseAddr, void *dstBaseAddr,DMA_DescriptorsHWConfig *pDescHWConfig);

void SSPDMAChannelStop(UINT32 channel);



//some marco for gpio operation
#define __REG(x)        *(volatile unsigned long*)(x)
			/* GPIO0, 1, 2*/
#define GPIO_PLR(n)     __REG(0xD4019000 + n*4)     /* GPIO pin level register */
//#define GPIO_PDR(n)     __REG(0xD401900C + n*4)     /* GPIO pin direction register */

//#define GPIO_PSR(n)     __REG(0xD4019018 + n*4)     /* GPIO pin output set register */
//#define GPIO_PCR(n)     __REG(0xD4019024 + n*4)     /* GPIO pin output clear register */
//#define GPIO_RER(n)     __REG(0xD4019030 + n*4)     /* GPIO rising-edge enable register */
//#define GPIO_FER(n)     __REG(0xD401903C + n*4)     /* GPIO Falling-Edge Detect Enable Register */
//#define GPIO_EDR(n)     __REG(0xD4019048 + n*4)     /* GPIO Edge Detect Status Register */
#define GPIO_SDR(n)     __REG(0xD4019054 + n*4)     /* Bit-wise Set of GPIO Direction Register */
#define GPIO_CDR(n)     __REG(0xD4019060 + n*4)     /* Bit-wise Clear of GPIO Direction Register */
#define GPIO_SRER(n)    __REG(0xD401906C + n*4)     /* Bit-wise Set of GPIO Rising Edge Detect Enable Register */
#define GPIO_CRER(n)    __REG(0xD4019078 + n*4)     /* Bit-wise Clear of GPIO Rising Edge Detect Enable Register */
#define GPIO_SFER(n)    __REG(0xD4019084 + n*4)     /* Bit-wise Set of GPIO Falling Edge Detect Enable Register */
#define GPIO_CFER(n)    __REG(0xD4019090 + n*4)     /* Bit-wise Clear of GPIO Falling Edge Detect Enable Register */
//#define GPIO_APMASK(n)  __REG(0xD401909C + n*4)     /* AP bit-wise mask */
//#define GPIO_CPMASK(n)  __REG(0xD40190A8 + n*4)     /* CP bit-wise mask */
#define GPIOn(n)        (n/32)
//#define GPIO_3_PDR(n)     __REG(0xD401910C)     /* GPIO pin direction register */
//#define GPIO_3_PCR(n)     __REG(0xD4019124)     /* GPIO pin output clear register */
//#define GPIO_3_PSR(n)     __REG(0xD4019118)     /* GPIO pin output set register */
//#define GPIO_3_RER(n)     __REG(0xD4019130)     /* GPIO rising-edge enable register */
//#define GPIO_3_FER(n)     __REG(0xD401913C)     /* GPIO Falling-Edge Detect Enable Register */
//#define GPIO_3_APMASK(n)  __REG(0xD401919C)     /* AP bit-wise mask */
//#define GPIO_3_CPMASK(n)  __REG(0xD40191A8)     /* CP bit-wise mask */
//#define GPIO_3_EDR(n)     __REG(0xD4019148)     /* GPIO Edge Detect Status Register */




//#define GPIO_PDR(n)     ((n==3)? __REG(0xD401910C) :__REG(0xD401900C + n*4))     /* GPIO pin direction register */
//#define GPIO_PCR(n)     ((n==3)? __REG(0xD4019124) :__REG(0xD4019024 + n*4))     /* GPIO pin output clear register */
//#define GPIO_PSR(n)     ((n==3)? __REG(0xD4019118) :__REG(0xD4019018 + n*4))      /* GPIO pin output set register */
//#define GPIO_RER(n)     ((n==3)? __REG(0xD4019130) :__REG(0xD4019030 + n*4))      /* GPIO rising-edge enable register */
//#define GPIO_FER(n)    	((n==3)? __REG(0xD401913C) :__REG(0xD401903C + n*4))      /* GPIO Falling-Edge Detect Enable Register */
//#define GPIO_APMASK(n)  ((n==3)? __REG(0xD401919C) :__REG(0xD401909C + n*4))     /* AP bit-wise mask */
//#define GPIO_CPMASK(n) 	((n==3)? __REG(0xD40191A8) :__REG(0xD40190A8 + n*4))     /* CP bit-wise mask */
//#define GPIO_EDR(n)     ((n==3)? __REG(0xD4019148) :__REG(0xD4019048 + n*4))     /* GPIO Edge Detect Status Register */
//#define GPIO_EDR(n)     __REG(0xD4019048 + n*4)     /* GPIO Edge Detect Status Register */


#define GPIO_PDR(n)     __REG(0xD401900C + n*4)     /* GPIO pin direction register */

#define GPIO_PSR(n)     __REG(0xD4019018 + n*4)     /* GPIO pin output set register */
#define GPIO_PCR(n)     __REG(0xD4019024 + n*4)     /* GPIO pin output clear register */
#define GPIO_RER(n)     __REG(0xD4019030 + n*4)     /* GPIO rising-edge enable register */
#define GPIO_FER(n)     __REG(0xD401903C + n*4)     /* GPIO Falling-Edge Detect Enable Register */
#define GPIO_EDR(n)     __REG(0xD4019048 + n*4)     /* GPIO Edge Detect Status Register */
#define GPIO_APMASK(n)  __REG(0xD401909C + n*4)     /* AP bit-wise mask */
#define GPIO_CPMASK(n)  __REG(0xD40190A8 + n*4)     /* CP bit-wise mask */







#define GPIO00_ADDRESS 0xD401E074
#define GPIO02_ADDRESS 0xD401E07C
#define GPIO01_ADDRESS 0xD401E078
#define GPIO03_ADDRESS 0xD401E080
#define GPIO72_ADDERSS 0xD401E2C8  //TDS_DIO5 FUNC1 AS GPIO_72
#define GPIO73_ADDERSS 0xD401E2CC  //TDS_DIO6 FUNC1 AS GPIO_73
#define GPIO126_ADDRESS 0xD401E06C  //SM_nBE0 FUN0 AS GPIO_126
#define GPIO74_ADDRESS 0xd401e2d0
#define GPIO77_ADDERSS 0xD401E2DC  //TDS_DIO6 FUNC1 AS GPIO_73
#define GPIO71_ADDERSS 0xD401E2C4  //TDS_DIO6 FUNC1 AS GPIO_71
#define GPIO78_ADDERSS 0xD401E2E0  //TDS_DIO6 FUNC1 AS GPIO_73

#define GPIO111_ADDERSS 0xD401E29C
#define GPIO112_ADDERSS 0xD401E2A0  //gpio113 is connected to DCDC_EN we should pull it high 

#define GPIO113_ADDERSS 0xD401E2A4  //gpio113 is connected to DCDC_EN we should pull it high 
#define MMC1_CLK_ADDERSS 0xD401E0A8
#define GPIO51_CLK_ADDERSS 0xD401E1A8
#define GPIO60_ADDERSS 0xD401E304
#define GPIO50_RXD_ADDRESS (0xD401E1A4)
#define MMCA_WP_SSP_RXD_ADDRESS (0xD401E0B0)

#define GPIO0_ADDRESS (0xD401E074)
#define GPIO2_ADDRESS (0xD401E07C)
#define GPIO11_ADDRESS (0xD401E108)
#define GPIO13_ADDRESS (0xD401E110)
#define GPIO17_ADDRESS (0xD401E120)
#define GPIO124_ADDRESS (0xD401E0D0)
#define GPIO33_CLK_ADDRESS (0xD401E160)
#define GPIO34_FRM_ADDRESS (0xD401E164)
#define GPIO35_RXD_ADDRESS (0xD401E168)
#define GPIO36_TXD_ADDRESS (0xD401E16C)
#define GPIO50_RXD_ADDRESS (0xD401E1A4)
#define SM_NCS0_GPIO87_ADDRESS (0xD401E04C)
#define SM_NCS0_GPIO88_ADDRESS (0xD401E050)
#define PULL_UP_C          0xC000  //Enable pull up resistor                                                             
#define PULL_DN_C          0xA000  //Enable pull down resistor                                                           
#define DRV_SLOW_C         0x0800  //Use slow drive strength                                                             
#define DRV_MED_C          0x1000  //Use medium drive strength                                                             
#define DRV_FAST_C         0x1800  //Use fast drive strength                                                              
#define AF0_C              0X0000  //Alternate function 0                                                                 
#define AF1_C              0X0001  //Alternate function 1                                                                 
#define AF2_C              0X0002  //Alternate function 2                                                                 
#define AF3_C              0X0003  //Alternate function 3                                                                 
#define AF4_C              0X0004  //Alternate function 4                                                                 
#define AF5_C              0X0005  //Alternate function 5                                                                 
#define AF6_C              0X0006  //Alternate function 6                                                                 
#define AF7_C              0X0007  //Alternate function 7                                                                 
#define RESERVED_C         0x0000  //RESERVED_C bits that must be set
#define Raising_Edge_Detect   0x10
