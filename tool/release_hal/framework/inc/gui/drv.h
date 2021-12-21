/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _DRV_H_
#define _DRV_H_

typedef struct _DRV_SCREENINFO {
    unsigned short width;
    unsigned short height;
    unsigned char bitdepth;
    unsigned char isMemoryAddrValid;
    unsigned short nResvered;
    VOID* pMemoryAddr;
}DRV_SCREENINFO;

typedef struct _LCD_INFO {
    short  em_x;	
    short  em_y;    
	short  ScreenWidth;
    short  ScreenHeight;
    short  ScreenBitDepth;
    unsigned int* pMemoryAdd;
}LCD_INFO;

//
//create 24 bit 8/8/8 format pixel (0xRRGGBB) from RGB triplet
//
#define DDI_RGB2PIXEL888(r,g,b)	\
	(((r) << 16) | (((BYTE)(g)) << 8) | ((BYTE)b))

//
//create 16 bit 5/6/5 format pixel (0xRGB) from RGB triplet
//
#define DRV_RGB2PIXEL565(r,g,b)	\
	((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))

#define DRV_LCD_IOCTL_CONTRAST        0x01
#define DRV_LCD_IOCTL_BRIGHTNESS      0x02
#define DRV_LCD_IOCTL_STANDBY         0x03
#define DRV_LCD_IOCTL_UNSTANDBY       0x04
#define DRV_LCD_IOCTL_SLEEP           0x05
#define DRV_LCD_IOCTL_UNSLEEP         0x06
#define DRV_LCD_IOCTL_DEVINFO         0x07
#define DRV_LCD_IOCTL_BACKLIGHT       0x08

unsigned int  DRV_LcdInit (
                    VOID
                    );

unsigned int  DRV_LcdExit (
                    VOID
                    );

unsigned int  DRV_LcdIoControl (
                         unsigned int uCode,
                         unsigned int nParamIn,
                         unsigned int* pParamOut
                         );

unsigned int  DRV_LcdSetPixel16(
                  unsigned short nXDest,
                  unsigned short nYDest,
                  unsigned short pPixelData
                  );

unsigned int  DRV_LcdBlt16(
                  unsigned short nXDest,
                  unsigned short nYDest,
                  unsigned short nSrcWidth,
                  unsigned short nSrcHeight,
                  unsigned short* pPixelData
                  );

unsigned int  DRV_LcdBmpBlt16(
  short  nXDest, // LCD position x
  short  nYDest, // LCD position y
  short  nXSrc,  // bmp   x
  short  nYSrc,  // bmp   y
  short  nWidth, // bmp   w
  short  nHeight, // bmp  h
  unsigned short* pBmpData
);

unsigned int  DRV_SubLcdInit (
                       VOID
                       );

unsigned int  DRV_SubLcdExit (
                       VOID
                       );

unsigned int  DRV_SubLcdIoControl (
                            unsigned int uCode,
                            unsigned int nParamIn,
                            unsigned int* pParamOut
                            );

unsigned int  DRV_SubLcdSetPixel12(
                  unsigned short nXDest,
                  unsigned short nYDest,
                  unsigned short nPixelData
                  );

unsigned int  DRV_SubLcdBlt(
                     unsigned char nXDest,
                     unsigned char nYDest,
                     unsigned char nSrcWidth,
                     unsigned char nSrcHeight,
                     unsigned short* pRgbData
                     );

//
// GPIO 
//

#define DRV_GPIO_0          0
#define DRV_GPIO_1          1     //Added by jinzh:20070619
#define DRV_GPIO_2          2
#define DRV_GPIO_6          6
#define DRV_GPIO_7          7
#define DRV_GPIO_8          8
#define DRV_GPIO_9          9
#define DRV_GPIO_10         10
#define DRV_GPIO_11         11
#define DRV_GPIO_12         12
#define DRV_GPIO_13         13
#define DRV_GPIO_14         14
#define DRV_GPIO_15         15
#define DRV_GPIO_16         16
#define DRV_GPIO_17         17
#define DRV_GPIO_18         18
#define DRV_GPIO_19         19

#define DRV_GPIO_20         20
#define DRV_GPIO_21         21
#define DRV_GPIO_22         22
#define DRV_GPIO_23         23
#define DRV_GPIO_24         24
#define DRV_GPIO_25         25
#define DRV_GPIO_26         26
#define DRV_GPIO_27         27
#define DRV_GPIO_28         28
#define DRV_GPIO_29         29
#define DRV_GPIO_30         30
#define DRV_GPIO_31         31

#define  DRV_GPIO_EDGE_R    0
#define  DRV_GPIO_EDGE_F    1
#define  DRV_GPIO_EDGE_RF   2

#define DRV_GPIO_IN 		0
#define DRV_GPIO_OUT 	    1

#define DRV_GPIO_LOW 		0
#define DRV_GPIO_HIGH 	    1

unsigned int  DRV_GpioSetDirection(
  unsigned char nGpio,     
  unsigned char nDirection     
);

unsigned int DRV_GpioSetMode( 
    unsigned char nGpio,
    unsigned char nMode
);

unsigned int  DRV_GpioGetDirection(
  unsigned char nGpio,     
  unsigned char* pDirection     
);

unsigned int  DRV_GpioSetLevel(
  unsigned char nGpio,
  unsigned char nLevel
);

unsigned int  DRV_GpioGetLevel(
    unsigned char nGgpio,
    unsigned char* pLevel
);

unsigned int  DRV_GpioSetEdgeIndication(
  unsigned char nGpio,
  unsigned char nEdge,
  unsigned int nDebonce
);

unsigned int  DRV_EXTISetEdgeIndication(
    unsigned char  nEdge
);


#define DRV_MEMORY_NO_PAGE   0
#define DRV_MEMORY_4HW_PAGE  1
#define DRV_MEMORY_8HW_PAGE  2
#define DRV_MEMORY_16HW_PAGE 3

#define DRV_MEMORY_0_CYCLE   0
#define DRV_MEMORY_1_CYCLE   1
#define DRV_MEMORY_2_CYCLE   2
#define DRV_MEMORY_4_CYCLE   3

typedef enum 
{	
	DRV_EXTI_EDGE_NUL,
	DRV_EXTI_EDGE_R,
	DRV_EXTI_EDGE_F,	
	DRV_EXTI_EDGE_RF,
}DRV_EXTI_EDGE;

typedef enum 
{
  DRV_MEMORY_FLASH=0,
  DRV_MEMORY_SRAM,
  DRV_MEMORY_CS2,
  DRV_MEMORY_CS3, /* this one can be used for main FLASH too */
  DRV_MEMORY_CS4, /* this one can be used for main SRAM too */
  DRV_MEMORY_CS5,
  DRV_MEMORY_CS6,
  DRV_MEMORY_CS_QTY
}DRV_EBC_CS;

typedef struct _DRV_MEM_CFG {
 BOOL writeAllow;
 BOOL writeByteEnable;
 BOOL waitMode;
 unsigned char pageSize;
 unsigned char tACS;
 unsigned char tCAH;
 unsigned char tCOS;
 unsigned char tOCH;
 int tACC;
 int tPMACC;
}DRV_MEM_CFG;

PVOID  DRV_MemorySetupCS(
  unsigned char nCS,
  DRV_MEM_CFG*  pMemCfg
);

unsigned int  DRV_MemoryDisableCS(
  unsigned char nCS
);

//
// PWML
//
unsigned int  DRV_SetPwl1(
  unsigned char nLevel
);

unsigned int DRV_SetKeyBacklight( unsigned char nLevel);

unsigned int  DRV_SetPwl0(
  unsigned char nLevel
);

/*************************************************
 *** UART
 *************************************************/
//define the error code 
#define DRV_UART_INVALID_UARTID		-1
#define DRV_UART_INVALID_BAUDRATE	-2
#define DRV_UART_IS_USING				-3
#define DRV_UART_ISNOT_USING			-4
#define DRV_UART_INVALID_BUFFER		-5
#define DRV_UART_INVALID_DATABIT		-6
#define DRV_UART_INVALID_STOPBIT		-7
#define DRV_UART_INVALID_PARITY		-8
#define DRV_UART_INVALID_MODE		-9
#define DRV_UART_DMA_NOT_DONE		-10
#define DRV_UART_NO_DMA_AVIAL		-11


#define DRV_UART_INVALID_TXMODE		-(9+12)
#define DRV_UART_INVALID_RXMODE		-(10+12)


/* Defines for interupts. */
#define DRV_UART_MASK_NIL				0
#define DRV_UART_MASK_RX				(1<<1)
#define DRV_UART_MASK_TX				(1<<2)
#define DRV_UART_MASK_TIMEOUT		(1<<3)
#define DRV_UART_MASK_ERROR			(1<<4)
#define DRV_UART_MASK_DMA_TX			(1<<5)
#define DRV_UART_MASK_DMA_RX			(1<<6)


/* UART number */
typedef enum  DRV_UartId{
  DRV_UART_0 = 0,
  DRV_UART_1 = 1,
  DRV_UART_2 = 2,
  DRV_UART_QTY=3
}DRV_UARTID;


/* data bits. */
enum DRV_UartDataBits {
	DRV_UART_7_DATA_BITS , 
	DRV_UART_8_DATA_BITS ,
	DRV_UART_DATA_BITS_QTY
};

/* Stop bit quantity. */
enum DRV_UartStopBitsQty {  
	DRV_UART_1_STOP_BIT , 
	DRV_UART_2_STOP_BITS ,  
	DRV_UART_STOP_BITS_QTY
};

/* Data parity control selection. */
enum DRV_UartParityCfg {
	DRV_UART_NO_PARITY ,
	DRV_UART_ODD_PARITY ,  
	DRV_UART_EVEN_PARITY ,  
	DRV_UART_SPACE_PARITY ,  
	DRV_UART_MARK_PARITY , 
	DRV_UART_PARITY_QTY
};

/* Baudrate with the modifiable system clock. */
enum  DRV_UartBaudRate{
  DRV_UART_BAUD_RATE_2400		=    2400,
  DRV_UART_BAUD_RATE_4800		=    4800,
  DRV_UART_BAUD_RATE_9600		=    9600,
  DRV_UART_BAUD_RATE_14400		=   14400,
  DRV_UART_BAUD_RATE_19200		=   19200,
  DRV_UART_BAUD_RATE_28800		=   28800,
  DRV_UART_BAUD_RATE_33600		=   33600,
  DRV_UART_BAUD_RATE_38400		=   38400,
  DRV_UART_BAUD_RATE_57600		=   57600,
  DRV_UART_BAUD_RATE_115200		=  115200,
  DRV_UART_BAUD_RATE_230400		=  230400,
  DRV_UART_BAUD_RATE_460800		=  460800,
  DRV_UART_BAUD_RATE_921600		=  921600,
  DRV_UART_BAUD_RATE_1843200	= 1843200,
  DRV_UART_BAUD_RATE_QTY
};

/* Data transfert mode: via DMA or direct. */
enum  DRV_UartTransfertMode{	
  DRV_UART_TRANSFERT_MODE_DIRECT_POLLING = 0,	//direct polling,has no irq
  DRV_UART_TRANSFERT_MODE_DIRECT_IRQ = 1,		//direct transfer with the irq callback
  DRV_UART_TRANSFERT_MODE_DMA_POLLING = 2,		//DMA mode with no irq callback
  DRV_UART_TRANSFERT_MODE_DMA_IRQ = 3,			//DMA mode with irq callback
  DRV_UART_TRANSFERT_MODE_QTY
};


typedef struct DRV_UartCfg {
enum DRV_UartDataBits data;			//data bit. 
enum DRV_UartStopBitsQty stop;  		//stop bit
enum DRV_UartParityCfg parity;  		//parity type
enum DRV_UartBaudRate rate;  			//baudrate
enum DRV_UartTransfertMode mode; 		//transfer mode
}DRV_UARTCFG;

//define for the callback function type
typedef void (*UARTCALLBACK)(unsigned short status);

/*
description:	open the uart
parameters:	UartID 		the uart number.
			drv_uartcfg	the configuration for the specific uart
			user_uart_callback		the callback function point, which will be
called when the transfer is complete, or there has received data in the fifo the first byte.
return value:	0 for successful, negative value for fail
*/
short DRV_UartOpen(DRV_UARTID UartID, 
	DRV_UARTCFG *drv_uartcfg, UARTCALLBACK user_uart_callback);

/*
description :	send the length bytes to uart, before using it, please call open first.
it will not be returned until the total length of the data has been transfered
parameters:		UartID 	the uart number
				buff		the buffer of the data which will be transfered
				length	the length of the data which will be transfered
return value:	positive value for successful, negative value for fail
*/
int DRV_UartSend(DRV_UARTID UartID, unsigned char *buff, unsigned short length);

/*
description:		receive the length bytes from uart.before using it, please call 
DRV_UartGetReceiveLen to check the length bytes in the fifo. in the DMA mode you 
should call DRV_uart_RxDmaDone the check if the DMA has been completed.
parameters:		UartID 	the uart number
				buff		the buffer of the data which will be transfered
				length	the length of the data which will be transfered
return value:	positive value for successful, negative value for fail
*/
int DRV_UartReceive(DRV_UARTID UartID, unsigned char *buff, unsigned short length);

/*
after finish using the uart , we can call it to close the uart
parameters:		UartID 	the uart number
return value:	positive value for successful, negative value for fail
*/
short DRV_UartClose(DRV_UARTID UartID);

/*
get the length bytes in the fifo, then call receive function to get the data,it just works
in no-DMA mode
parameters:		UartID 	the uart number
return value:	positive value for successful, negative value for fail
*/
short DRV_UartGetReceiveLen(DRV_UARTID UartID);

/*
check if the DMA receive has been done, it can only be called when the uart
works in DMA mode
parameters:		UartID 	the uart number
return value:	it will return 1, otherwise it will return negative value
*/
short DRV_uart_RxDmaDone( DRV_UARTID UartID );


typedef enum {
    DRV_SPI_CS0 = 0, ///< Chip Select 0
    DRV_SPI_CS1,     ///< Chip Select 1
    DRV_SPI_CS_QTY
} DRV_SpiCs_t;




typedef enum
{
	DRV_SPI_DIRECT_POLLING,	 	
	DRV_SPI_DIRECT_IRQ 	,		
	DRV_SPI_DMA_POLLING ,			
	DRV_SPI_DMA_IRQ 		,				
	DRV_SPI_OFF 		,			
	DRV_SPI_TM_QTY 				
}DRV_SpiTransfertMode;
typedef enum
{
	DRV_SPI_HALF_CLK_PERIOD_0  ,		
	DRV_SPI_HALF_CLK_PERIOD_1 ,	
	DRV_SPI_HALF_CLK_PERIOD_2,	
      DRV_SPI_HALF_CLK_PERIOD_3		
}DRV_Spi_Delay;
typedef enum
{
	DRV_SPI_RX_TRIGGER_1_BYTE      ,		
	DRV_SPI_RX_TRIGGER_4_BYTE	,	
	DRV_SPI_RX_TRIGGER_8_BYTE	,		
	DRV_SPI_RX_TRIGGER_12_BYTE	,	
	DRV_SPI_RX_TRIGGER_QTY			
} DRV_SpiRxTriggerCfg;
typedef enum
{
	DRV_SPI_TX_TRIGGER_1_EMPTY	,	
	DRV_SPI_TX_TRIGGER_4_EMPTY	,
	DRV_SPI_TX_TRIGGER_8_EMPTY	,	
	DRV_SPI_TX_TRIGGER_12_EMPTY	,
	DRV_SPI_TX_TRIGGER_QTY			
}DRV_SpiTxTriggerCfg;

#define  DRV_SPI_MIN_FRAME_SIZE			4
#define  DRV_SPI_MAX_FRAME_SIZE			32
#define  DRV_SPI_MAX_CLK_DIVIDER			0xFFFFFF

#define  DRV_SPI_IRQ_MSK_RX_OVF			0x1
#define  DRV_SPI_IRQ_MSK_TX_TH				0x2
#define  DRV_SPI_IRQ_MSK_TX_DMA			0x4
#define  DRV_SPI_IRQ_MSK_RX_TH				0x8
#define  DRV_SPI_IRQ_MSK_RX_DMA			0x10

typedef  VOID (*DEVICE_SPI_USERVECTOR)(unsigned short);


typedef struct _DRV_SpiCfg {
  DRV_SpiCs_t 		enabledCS;
  BOOL           polarityCS;
  unsigned char  	inputEn;
  unsigned char  	clkFallEdge;
  DRV_Spi_Delay 		clkDelay;
  DRV_Spi_Delay 		doDelay;
  DRV_Spi_Delay 		diDelay;
  DRV_Spi_Delay 		csDelay;
  DRV_Spi_Delay 		csPulse;
  int 			frameSize;
  int 			clkDivider;
  DRV_SpiRxTriggerCfg  		rxTrigger;
  DRV_SpiTxTriggerCfg  		txTrigger;
  DRV_SpiTransfertMode  	rxMode;
  DRV_SpiTransfertMode  	txMode;
  unsigned short  irqMask; ///< Irq Mask for this CS
  DEVICE_SPI_USERVECTOR irqHandler; ///< Irq Handler for this CS

} DRV_SpiCfg;/*this struct to user*/

unsigned int DRV_SPIInit( DRV_SpiCfg*spicfg);


VOID DRV_SPIExit(DRV_SpiCs_t cs);

void DRV_WaitSPIRxDmaDone(
	unsigned char *datain,
	unsigned int inlen
	);

unsigned int DRV_SPIWrite(DRV_SpiCs_t cs,unsigned char *data, unsigned short len);

unsigned int DRV_SPIRead(DRV_SpiCs_t cs,unsigned char *outbuf,unsigned short len);

void DRV_SPISetISRmask(
	unsigned short mask
	);

unsigned char DRV_Spi_TxFinished(DRV_SpiCs_t cs);
unsigned int DRV_ISRSpiRead(
	unsigned char *add ,
	unsigned int len
	);
unsigned int DRV_GetSpiIsrFifoLen(
	void
	);

void DRV_SPI_ChangeCs(
	DRV_SpiCfg *spicfg
	);

#endif // _H_ 

