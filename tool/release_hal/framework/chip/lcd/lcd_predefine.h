#ifndef _LCD_PREDEFINE_H_
#define _LCD_PREDEFINE_H_

#include "plat_types.h"
#include "ui_log_api.h"

/*interrupt mode or polling mode*/
#define LCD_INTERRUPT_MODE
/*fpga or evb*/
//#define LCD_FPGA_TEST
/*psram or squ*/
#define LCD_PSRAM_TEST
/*use media clk interface or set it hardcoded*/
#define USE_MEDIACLK_INTERFACE
/*lcd log level*/
#define LCD_PRINT_WARNING

#ifdef LCD_FPGA_TEST
#define LCD_SCLK_FREQ 10000
#else
#ifdef LCD_MCU
#define LCD_SCLK_FREQ 52000
//#define LCD_SCLK_FREQ 26000
#else
//#define LCD_SCLK_FREQ 62400
//#define LCD_SCLK_FREQ 125000
#define LCD_SCLK_FREQ 104000
#endif
#endif

#ifdef LCD_PRINT_ERROR
#define LCD_LOG_LEVEL_ERROR
#endif

#ifdef LCD_PRINT_WARNING
#define LCD_LOG_LEVEL_ERROR
#define LCD_LOG_LEVEL_WARNING
#endif

#ifdef LCD_PRINT_INFO
#define LCD_LOG_LEVEL_ERROR
#define LCD_LOG_LEVEL_WARNING
#define LCD_LOG_LEVEL_INFO
#endif

#ifdef LCD_PRINT_DEBUG
#define LCD_LOG_LEVEL_ERROR
#define LCD_LOG_LEVEL_WARNING
#define LCD_LOG_LEVEL_INFO
#define LCD_LOG_LEVEL_DEBUG
#endif

#ifdef LCD_LOG_LEVEL_ERROR
#define LCDLOGE(fmt, ...) raw_uart_log(fmt, ##__VA_ARGS__)
#else
#define LCDLOGE(fmt, ...)
#endif

#ifdef LCD_LOG_LEVEL_WARNING
#define LCDLOGW(fmt, ...) raw_uart_log(fmt, ##__VA_ARGS__)
#else
#define LCDLOGW(fmt, ...)
#endif

#ifdef LCD_LOG_LEVEL_INFO
#define LCDLOGI(fmt, ...) raw_uart_log(fmt, ##__VA_ARGS__)
#else
#define LCDLOGI(fmt, ...)
#endif

#ifdef LCD_LOG_LEVEL_DEBUG
#define LCDLOGD(fmt, ...) raw_uart_log(fmt, ##__VA_ARGS__)
#else
#define LCDLOGD(fmt, ...)
#endif

#if 0
typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef signed short  int16_t;
typedef unsigned short    uint16_t;
typedef signed int     int32_t;
typedef unsigned int   uint32_t;
typedef signed long long int    int64_t;
typedef unsigned long long int  uint64_t;

#define NULL 0L
#endif

#define LCD_BU_REG_READ(x) (*(volatile uint32_t *)(x))
#define LCD_BU_REG_WRITE(x,y) ((*(volatile uint32_t *)(x)) = (y) )
#define LCD_ROUND_UP(x, align) (((unsigned int)(x) + (align - 1)) & (~(align - 1)))

#if 0
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
#endif

#if 1
static __inline void uudelay(int us)
{
	volatile uint32_t i;
	for(i=0; i<us*100;i++)
		i = i+1;
}

static __inline void mdelay(int ms){
	volatile uint32_t i;
	for(i=0; i<ms;i++)
		uudelay(100);
}
#endif

#endif /*_LCD_PREDEFINE_H_*/
