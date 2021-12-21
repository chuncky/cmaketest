#ifndef _LCD_PREDEFINE_H_
#define _LCD_PREDEFINE_H_

#include "common.h"

//#define LCD_INTERRUPT_MODE
//#define ASR_LCD_CAMERA_ENABLE

#ifdef ASR_LCD_CAMERA_ENABLE
#define USE_MEDIACLK_INTERFACE
#endif


#if (defined WATCHLCDST7789_CODE_USE) || (defined LCD_GC9306VER2) || (defined LCD_ST7789VER2) || (defined LCD_GC9305VER1)
	#define HW_PLATFORM_4_LINE_SUPPORT
#endif


#define OSA_TICK_FREQ_IN_MILLISEC   5
#define MS_TO_TICKS(n) ((n) / OSA_TICK_FREQ_IN_MILLISEC ? (n) / OSA_TICK_FREQ_IN_MILLISEC : 1)


extern int uart_printf(const char* fmt, ...);
#if 0
typedef unsigned char uint8_t,u8,uint8;
typedef unsigned short uint16_t,u16,uint16;
typedef unsigned int uint32_t,u32,uint_32;
typedef long long uint64_t;
typedef char int8_t;
typedef signed int int32_t, HANDLE;
typedef signed short int16_t;
typedef void*	PVOID;
typedef char* PSTR;
typedef const UINT8* PCSTR;
#endif
#define LCD_ASSERT(cOND) {if (!(cOND)) {utilsAssertFail(#cOND, __FILE__, (short)__LINE__, 1);}}
static void uudelay(int us)
{
	volatile uint32_t i;
	for(i=0; i<us*100;i++);
}

static void mdelay(int ms){
	volatile uint32_t i;
	for(i=0; i<ms;i++)
		uudelay(10);
}


#if defined (LCD_ST7789DKB_MCU_3WIRE_2LANE)
#define TEST_MCU_PANEL
#else
#define TEST_SPI_PANEL
#endif


/*psram or squ*/
#define LCD_PSRAM_TEST

#ifdef TEST_MCU_PANEL
#define LCD_SCLK_FREQ 52000
#else
#define LCD_SCLK_FREQ 104000
#endif


#if 1
#define LCD_PRINT_WARNING
#else
#define LCD_PRINT_DEBUG
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
#define LCDLOGE(fmt, ...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define LCDLOGE(fmt, ...)
#endif

#ifdef LCD_LOG_LEVEL_WARNING
#define LCDLOGW(fmt, ...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define LCDLOGW(fmt, ...)
#endif

#ifdef LCD_LOG_LEVEL_INFO
#define LCDLOGI(fmt, ...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define LCDLOGI(fmt, ...)
#endif

#ifdef LCD_LOG_LEVEL_DEBUG
#define LCDLOGD(fmt, ...) uart_printf(fmt, ##__VA_ARGS__)
#else
#define LCDLOGD(fmt, ...)
#endif
#ifndef BU_REG_READ
#define BU_REG_READ(x) (*(volatile uint32_t *)(x))
#define BU_REG_WRITE(x,y) ((*(volatile uint32_t *)(x)) = (y) )
#endif

enum{
	GPIO_LOW,
	GPIO_HIGH
};

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



#endif /*_LCD_PREDEFINE_H_*/
