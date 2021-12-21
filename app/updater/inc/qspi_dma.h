#ifndef __DMA_EXT_H_
#define __DMA_EXT_H_

#include "types.h"

typedef struct
{
	unsigned int DDADR; //descriptor address reg
	unsigned int DSADR; //source address reg
	unsigned int DTADR; //target address reg
	unsigned int DCMD; //command address register
}XLLP_DMAC_DESCRIPTOR_T, *P_XLLP_DMAC_DESCRIPTOR_T;



typedef struct
{
	unsigned int DCSR[32];
	unsigned int RESERVED0[0x8];
	unsigned int DALGN;
	unsigned int DPCSR;
	unsigned int RESERVED1[0xE];
	unsigned int DRQSR0;
	unsigned int DRQSR1;
	unsigned int DRQSR2;
	unsigned int RESERVED2[0x1];
	unsigned int DINT;
	unsigned int RESERVED3[0x3];
	unsigned int DRCMR1[64];
	XLLP_DMAC_DESCRIPTOR_T DMA_DESC[32];
	unsigned int RESERVED4[0x340];
	unsigned int DRCMR2[36];
}XLLP_DMAC_T, *P_XLLP_DMAC_T;


#define RCMR_MAPVLD BIT_7
#define CSR_STOPINTR BIT_3
#define CSR_RUN BIT_31
#define CSR_STOPIRQEN BIT_29

#define CSR_BUSERRINTR BIT_0
#define CSR_STARTINTR BIT_1
#define CSR_ENDINTR BIT_2
#define CSR_EORIRQEN BIT_28

#define CSR_NODESCFETCH BIT_30







#define DMA_DDADR_RESERVED_MASK	    0xFFFFFFF0
#define DMA_STATUS_INTERRUPTS_MASK  0x01000217
#define DMA_DEVICE_PART1_NUM		64

#define DMA_SQU_2_SQU_CHANNEL       0
#define DMA_SQU_2_DDR_CHANNEL       1
#define DMA_DDR_2_SQU_CHANNEL       2
#define DMA_DDR_2_DDR_CHANNEL       3
#define DMA_XFER_CHANNEL            4

#define DMA_GEU_W_CHANNEL           5
#define DMA_GEU_R_CHANNEL           6

#define DMA_SQU_ADDR_1              0xD1000000
#define DMA_SQU_ADDR_2              0xD1004000

#define DMA_MDMA_XFER_SIZE	        0xF000
#define DMA_MAX_BYTES_PER_DESC      0x1F00

#define DMA_MDMA_NF_XFER_SIZE       0x1F00

#define DMA_STRESS_XFER_SIZE        0x5000


typedef struct
{
    uint32_t    DDADR;  // descriptor address reg
    uint32_t    DSADR;  // source address register
    uint32_t    DTADR;  // target address register
    uint32_t    DCMD;   // command address register
}DMA_DESCRIPTOR, *P_DMA_DESCRIPTOR;

struct DMA_CMD_BITS 
{
    uint32_t Length         :13; //Bits [12:0]: Length of transfer in Bytes
    uint32_t Reserved4      :1; //Bit 13: reserved
    uint32_t Width          :2; //Bits [15:14]: Width of onchip peripheral - 0=reserved, 1=1B, 2=2B, 3=4B
    uint32_t MaxBurstSize   :2; //Bits [17:16]: Max Burst Size - 0=reserved, 1=8B, 2=16B, 3=32B
    uint32_t Reserved3      :3; //Bits [20:18]: reserved
    uint32_t EndIRQEn       :1; //Bit 21: end interrupt enable
    uint32_t StartIRQEn     :1; //Bit 22: start interrupt enable
    uint32_t AddrMode       :1; //Bit 23: Addressing mode for descriptor compare - 0=target has addr/1=target has data
    uint32_t Reserved2      :1; //Bit 24: reserved
    uint32_t CmpEn          :1; //Bit 25: Compare Enable - descriptor branching compare enable
    uint32_t Reserved1      :2; //Bits [27:26]: reserved
    uint32_t FlowTrg        :1; //Bit 28: Target flow control - 1=wait for request signal
    uint32_t FlowSrc        :1; //Bit 29: Source flow control - 1=wait for request signal
    uint32_t IncTrgAddr     :1; //Bit 30: Target increment - 0=no increment, 1=increment
    uint32_t IncSrcAddr     :1; //Bit 31: Source increment - 0=no increment, 1=increment
};

typedef union
{
    uint32_t value;
    struct DMA_CMD_BITS bits;   
} DMA_CMDx_T, *P_DMA_CMDx_T;

typedef enum{
    DMA_DREQ0 = 0,            // DREQ 0, Companion Chip request 0
    DMA_DREQ1 = 1,            // DREQ 1, Companion Chip request 1
    DMA_DREQ2 = 2,
    DMA_UART_0_RX = 3,
    DMA_UART_0_TX = 4,
    DMA_UART_1_RX = 5,
    DMA_UART_1_TX = 6,
    DMA_I2C0_TX = 7,
    DMA_I2C0_RX = 8,

    DMA_SSP_0_RX = 9,
    DMA_SSP_0_TX = 10,
    DMA_SSP_2_RX = 11,
    DMA_SSP_2_TX = 12,

    DMA_LCD0 = 13,
    DMA_LCD1 = 14,

    DMA_I2C1_TX = 15,
    DMA_I2C1_RX = 16,

    DMA_NAND_DATA = 44,
    DMA_NAND_CMD = 45,

    DMA_AEU_TX = 46,
    DMA_AEU_RX = 47,

    DMA_MEM2MEM_MOVE = 255  // RESERVED for Memory to Memory moves
}DMA_MAP_DEVICE;

typedef struct DMA_STRESS_ARG_TAG{
    uint32_t channel;
    uint32_t num;
    uint32_t size;
    uint32_t stop_flag;
    uint32_t mask;
    uint32_t mask_t;
    uint32_t src[32];
    uint32_t dst[32];
    uint32_t des[32];
}DMA_STRESS_ARG, *P_DMA_STRESS_ARG;

typedef enum{
    DMA_BURST_SIZE_RSVD = 0,
    DMA_BURST_SIZE_8B,
    DMA_BURST_SIZE_16B,
    DMA_BURST_SIZE_32B
}DMA_BURST_SIZE;

typedef enum{
    DMA_MODE_FETCH = 0,
    DMA_MODE_NONFETCH
}DMA_MODE;



typedef enum
{
	DMA_STOP = 0,
	DMA_END
}DMA_WAIT_DONE_FLAG;



#endif
