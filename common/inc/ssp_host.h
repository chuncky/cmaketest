#ifndef _SSP_HOST_H_
#define _SSP_HOST_H_

#include "NSSP.h"
#include "APBCLK.h"
//#include "timer_dec.h"
//#include "cache_ops.h"
//#include "gpio_dec.h"
//#include "mfp-emei.h"
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

#define SSP_TIMEOUT_MAX					(0x2000)
#define SSP_RX_TIMEOUT					(0x2000)
#define SSP_CS							(GPIO25)
#define SSP_CR1_IE						(SSP_SSCR1_TINTE|SSP_SSCR1_TIE|SSP_SSCR1_RIE)
#define SSP_SR_CLR						(SSP_SSSR_ROR|SSP_SSSR_TINT)

#define SSP_XFER_BEGIN					0x01
#define SSP_XFER_END					0x02
#define SSP_XFER_BE						0x03

#define	SSP_DMA_TX_CHANNEL				6
#ifdef FOTA_ASRSPI
#define SSP_DMA_RX_CHANNEL				7
#else
#define SSP_DMA_RX_CHANNEL				20
#endif

#define SSP_CS_GPIO                     13
#define SSP_CS_GPIO_SLAVE               34

#define SPI_DSS8                        0x7
#define SPI_DSS16                       0xf
#define SPI_DSS32                       0x1f
/* TXD Three-state Enable On Last Phase */
#define SSP_TCR_TTELP			        BIT_18

    /* TXD Three-State Enable */
#define SSP_TCR_TTE				        BIT_17

    /* Data Size Select */
#define SSP_TCR_DSS_MASK		        SHIFT5(0x1f)
#define SSP_TCR_DSS32			        SHIFT5(0x1f)
#define SSP_TCR_DSS24			        SHIFT5(0x17)
#define SSP_TCR_DSS16			        SHIFT5(0xf)
#define SSP_TCR_DDS8			        SHIFT5(0x7)
#define SSP_TCR_HFL				        SHIFT14(1);

    /* Transmit FIFO Underrun Interrupt Mask */
#define SSP_IER_TIM				        BIT_5

    /* Receive FIFO Overrun Interrupt Mask */
#define SSP_IER_RIM				        BIT_4

#define SSP_TCR_INITIAL (SSP_TCR_TTELP | SSP_TCR_TTE | SSP_TCR_DDS8)
#define SSP_FCR_INITIAL (SSP_FCR_RFT(0x1)|SSP_FCR_TFT(0x1)) 
#define SSP_IER_INITIAL 0

/* Rx FIFO Auto Full Control */
#define SSP_FCR_RAFC                    BIT_17

/* Receive Service Request Enable */
#define SSP_FCR_RSRE			        BIT_11

/* Transmit Service Request Enable */
#define SSP_FCR_TSRE			        BIT_10

#define SSP_FCR_RFT_MASK		        SHIFT5(0x1f)
#define SSP_FCR_RFT(x)			        (x << 5)

/* TXFIFO Trigger Threshold */
/* MaxBurstSize(DMA)/Data Size Select(SSP) - 1 */
#define SSP_FCR_TFT_MASK		        SHIFT0(0x1f)
#define SSP_FCR_TFT(x)			        (x << 0)

#define TOP_INIT_DATA8                  0x000620E0
#define TOP_INIT_DATA16                 0x000621E0
#define TOP_INIT_DATA32                 0x000623E0
#define FIFO_INIT                       0x00000108
#define FIFO_INIT_READ                  0xCE7
#define FIFO_INIT_WRITE                 0x4E7
#define INT_EN_INIT                     0x30

static SSP_TypeDef *ssp_ports[3] = {SSP0, SSP1, SSP2};
#define ssp_port(x)                     ssp_ports[x]

typedef enum{
	SSP_CLK_6_5M = 0,
	SSP_CLK_13M,
	SSP_CLK_26M,
	SSP_CLK_52M
}SSP_CLOCK;

typedef enum{
	SSP_DATA_LEN_8 = 0,
	SSP_DATA_LEN_16,
	SSP_DATA_LEN_18,
	SSP_DATA_LEN_32
}SSP_DATA_LEN;

typedef struct {
	unsigned int mode;
	unsigned int data_length;
	unsigned int speed;
	unsigned int tx_watermark;
	unsigned int rx_watermark;
	unsigned int fmt;
	unsigned int i2s_width;
	int irq;
}ssp_init_param;

#define SPI_CPHA    0x01	/* clock phase */
#define SPI_CPOL    0x02	/* clock polarity */
#define SPI_MODE_0  (0|0)	/* (original MicroWire) */
#define SPI_MODE_1  (0|SPI_CPHA)
#define SPI_MODE_2  (SPI_CPOL|0)
#define SPI_MODE_3  (SPI_CPOL|SPI_CPHA)

/*psp params*/
#define SSP_SPI_MODE			(0x0 << 1)
#define SSP_SSP_MODE			(0x1 << 1)
#define SSP_PSP_MODE			(0x3 << 1)
#define SSP_ONCHIP_CLOCK		(0x0 << 6)
#define SSP_EXTER_CLOCK		(0x1 << 6)
#define SSP_DATA_16BIT		16
#define SSP_DATA_8BIT		8
#define SSP_DATA_32BIT		32
#define SSP_DATA_18BIT		18


extern unsigned int Endian_Convert (unsigned int in);
extern int spi_wait_complete(int port);
extern void spi_assert_cs(int port);
extern void spi_deassert_cs(int port);

#if 0
#define spi_dbg(fmt,...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define spi_dbg(fmt,...)
#endif



#endif
