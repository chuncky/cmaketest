#ifndef _BSP_COMMON_H_
#define _BSP_COMMON_H_
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_conf.h"
#include "pmic.h"

//#define FAST_TIMER_CLOCK_RATE_IN_HZ                3250000
#define TIMER_USECS_IN_SECOND                      1000000
#define FAST_TIMER_CLOCK_RATE_IN_HZ                13000000
#define SLOW_TIMER_CLOCK_RATE_IN_HZ                32768
#define SECONDS_TO_MICRO_SEC_RATIO                 1000000
#define MILISEC_TO_MICRO_SEC_RATIO                 1000L








/* memory berrier */
#define barrier() __asm{DSB}

/* timer */
void timer_enable(void);
void timer_disable(void);

/* delay */
void mdelay(int ms);
void lv_udelay(unsigned us);

/* PMIC */
#define PMIC_BASE_PAGE_ADDR 0x30 /* base page 7-bit i2c address */
#define PMIC_POWER_PAGE_ADDR 0x31
#define PMIC_GPADC_PAGE_ADDR 0x32

#define PMIC_ID         0x0

#define ONKEY_RELEASED 0
#define ONKEY_PRESSED 1

enum {
    PM_812 = 1,
    PM_813,
    PM_802,
    PM_803,
    PM_UNKNOWN,
};

enum {    
	PM_SUSPEND,    
	PM_ON,    
	PM_MAX,
};




typedef struct rtc_time_s {
  int tm_sec;	//seconds [0,59]
  int tm_min;	//minutes [0,59]
  int tm_hour;  //hour [0,23]
  int tm_mday;  //day of month [1,31]
  int tm_mon;   //month of year [1,12]
  int tm_year; // since 1970
  int tm_wday; // sunday = 0
}rtc_time_t;


#define SIZEOF_CACHE_LINE               32
#define CACHE_LINE_MASK                 (SIZEOF_CACHE_LINE - 1)

typedef enum
{
    GPIO_IN_PIN = 0,
    GPIO_OUT_PIN = 1
}GPIOPinDirection;

typedef enum
{
    GPIO_NO_EDGE = 0,
    GPIO_RISE_EDGE,
    GPIO_FALL_EDGE,
    GPIO_TWO_EDGE,
}GPIOTransitionType;

typedef void (*GPIOCallback)(void);



#define PMIC_POWERUP_ONKEY      (1 << 0)
#define PMIC_POWERUP_BAT        (1 << 3)
#define PMIC_POWERUP_RTC_ALARM  (1 << 4)
#define PMIC_POWERUP_REBOOT     (1 << 5)
#define PMIC_POWERUP_USB        (1 << 6)

typedef void (*alarm_cb_t)(void);
typedef void (*chg_cb_t)(void);

void pmic_sw_pdown(void);
void pmic_sw_reboot(void);



void pmic_rtc_get_time(rtc_time_t *t);
void pmic_rtc_set_alarm(rtc_time_t *time, alarm_cb_t callback);
void pmic_rtc_get_alarm(rtc_time_t *time);
void pmic_rtc_enable_alarm(uint8_t on_off);
void pmic_register_chg_handle(chg_cb_t handle);
int pmic_get_chg_status(void);
uint8_t pmic_get_bat_remain(void);
uint8_t pmic_powerup_get_reason(void);


/* interrupt */
unsigned irq_disable(void);
unsigned irq_enable(void);
void irq_restore(unsigned state);
int irq_is_in(void);

int mask_interrupt(unsigned int irq);
int unmask_interrupt(unsigned int irq);

typedef void (*int_handler)(void *arg);

void register_int_handler(unsigned int irq, int_handler handler, void *arg);
void interrupts_init(void);

/* Cache */
#define ICACHE 1
#define DCACHE 2
#define UCACHE (ICACHE | DCACHE)

void arch_disable_cache(unsigned flags);
void arch_enable_cache(unsigned flags);
void arch_clean_cache_range(unsigned start, unsigned len);
void arch_clean_invalidate_cache_range(unsigned start, unsigned len);
void arch_invalidate_cache_range(unsigned start, unsigned len);
void arch_sync_cache_range(unsigned start, unsigned len);
void  flush_cache(unsigned long start, unsigned long size);

/* Misc */

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define REG32(addr) ((volatile uint32_t *)(addr))

#define writel(v, a) (*REG32(a) = (v))
#define readl(a) (*REG32(a))

#define CIU_BASE                 0xD4282C00
static inline unsigned int hw_platform_type(void)
{
    return ((readl(CIU_BASE) >> 24) & 0x7);
}

enum {
    GPIO_INVALID    = -1,
    GPIO0           = 0,
    GPIO1,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5,
    GPIO6,
    GPIO7,
    GPIO8,
    GPIO9,
    GPIO10,
    GPIO11,
    GPIO12,
    GPIO13,
    GPIO14,
    GPIO15,
    GPIO16,
    GPIO17,
    GPIO18,
    GPIO19,
    GPIO20,
    GPIO21,
    GPIO22,
    GPIO23,
    GPIO24,
    GPIO25,
    GPIO26,
    GPIO27,
    GPIO28,
    GPIO29,
    GPIO30,
    GPIO31,
    GPIO32,
    GPIO33,
    GPIO34,
    GPIO35,
    GPIO36,
    GPIO37,
    GPIO38,
    GPIO39,
    GPIO40,
    GPIO41,
    GPIO42,
    GPIO43,
    GPIO44,
    GPIO45,
    GPIO46,
    GPIO47,
    GPIO48,
    GPIO49,
    GPIO50,
    GPIO51,
    GPIO52,
    GPIO53,
    GPIO54,
    GPIO55,
    GPIO56,
    GPIO57,
    GPIO58,
    GPIO59,
    GPIO60,
    GPIO61,
    GPIO62,
    GPIO63,
    GPIO64,
    GPIO65,
    GPIO66,
    GPIO67,
    GPIO68,
    GPIO69,
    GPIO70,
    GPIO71,
    GPIO72,
    GPIO73,
    GPIO74,
    GPIO75,
    GPIO76,
    GPIO77,
    GPIO78,
    GPIO79,
    GPIO80,
    GPIO81,
    GPIO82,
    GPIO83,
    GPIO84,
    GPIO85,
    GPIO86,
    GPIO87,
    GPIO88,
    GPIO89,
    GPIO90,
    GPIO91,
    GPIO92,
    GPIO93,
    GPIO94,
    GPIO95,
    GPIO96,
    GPIO97,
    GPIO98,
    GPIO99,
    GPIO100,
    GPIO101,
    GPIO102,
    GPIO103,
    GPIO104,
    GPIO105,
    GPIO106,
    GPIO107,
    GPIO108,
    GPIO109,
    GPIO110,
    GPIO111,
    GPIO112,
    GPIO113,
    GPIO114,
    GPIO115,
    GPIO116,
    GPIO117,
    GPIO118,
    GPIO119,
    GPIO120,
    GPIO121,
    GPIO122,
    GPIO123,
    GPIO124,
    GPIO125,
    GPIO126,
    GPIO127,
    GPIO_NUMS,
};

#define GPIO_IN    0
#define GPIO_OUT   1
#define GPIO_HIGH  1
#define GPIO_LOW   0

#define EDGE_FLAG_RISE 1
#define EDGE_FLAG_FALL 2

typedef void (*gpio_int_handler)(void);

void gpio_init(void);
void gpio_set_direction(int gpio_idx, int direction);
void gpio_output_set(int gpio_idx, int value);
void gpio_enable_edge_detect(int gpio_idx, int edge_flag);
void gpio_register_int_handler(unsigned int gpio_idx, gpio_int_handler func);

typedef void (*tick_cb_t)(uint32_t);
/* The in param "ms" should be a multiple of 5 */
void board_sys_tick_init(unsigned ms, tick_cb_t cb);
void board_sys_tick_start(void);
void board_sys_tick_stop(void);
unsigned lv_get_ticks_per_second(void);

/*sd card mount to U storage*/
//extern bool charger_is_usb(void);
//extern bool sdcard_is_ready(void);

typedef enum
{
    USB_CHARGER_ONLY,
    USB_STORAGE_ONLY,
}USBDeviceType;

extern void USBDeviceSelect(USBDeviceType DeviceSelect);
extern int sdcard_fat_is_ok(void);
void mdelay(int ms);

typedef unsigned char BOOL;
//typedef uint32_t UINT32;
extern void NingboLcdBackLightCtrl(uint8_t level); //level should be 0~5
extern void PM812_Ldo_8_set_2_8(void);
extern void PM812_Ldo_8_set(uint8_t OnOff);
extern void Ningbo_Ldo_1_set_2_8(void);
extern void Ningbo_Ldo_1_set(uint8_t OnOff);
extern void Ningbo_Ldo_10_set(BOOL OnOff);
extern void Ningbo_Ldo_10_set_2_8(void);
extern void Ningbo_Ldo_13_set_1_8(void);
extern void Ningbo_Ldo_13_set(BOOL OnOff);
extern void NingboVibratorEnable(void);
extern void NingboVibratorDisable(void);

typedef void (*OnkeyIntCallback)(void);
extern void PM812_ONKEY_BIND_INTC(OnkeyIntCallback callback);
extern void PM812_ONKEY_INTC_ENABLE(BOOL onoff);

extern UINT32 pm813_get_bat_vol(void);
extern UINT32 pm813_get_battery_voltage_withoutCharger(void);
extern UINT32 pm813_get_Icharge_meas_cur_mA(void);
extern UINT32 pm813_get_battemp_meas_vol_mv(void);
extern BOOL pm813_set_max_cc_current(UINT32 limit);

typedef enum {
    //INT ENABLE REG 2 addr=0x0a
    NINGBO_TINT_INT=0,
    NINGBO_GPADC0_INT,
    NINGBO_GPADC1_INT,
    NINGBO_VINLDO_INT,
    NINGBO_VBAT_INT,
    NINGBO_CP_START_ERROR_DET_INT,
    NINGBO_CLASSD_OUT_DET_INT,
    NINGBO_RTC_INT,

    //INT ENABLE REG 1 addr=0x09
    NINGBO_ONKEY_INT=8,
    NINGBO_EXTON1_INT,
    NINGBO_EXTON2_INT,
    NINGBO_BAT_INT,
    NINGBO_VBUS_UVLO_INT,
    NINGBO_VBUS_OVP_INT,
    NINGBO_VBUS_DET_INT,
    NINGBO_CP_START_DONE_DET_INT,
} NINGBO_INTC ;

typedef void (*PmicCallback)(void);
extern void Ningbo_INT_CALLBACK_REGISTER(NINGBO_INTC intc,PmicCallback isr);
extern void Ningbo_INT_ENABLE(NINGBO_INTC intc);
extern void Ningbo_INT_DISABLE(NINGBO_INTC intc);

typedef enum
{
    NINGBO_BASE_Reg,
    NINGBO_POWER_Reg,
    NINGBO_GPADC_Reg
}Ningbo_Reg_Type;

extern int NingboRead( Ningbo_Reg_Type ningbo_reg_type, unsigned char reg, unsigned char *value );
extern int NingboWrite( Ningbo_Reg_Type ningbo_reg_type, unsigned char reg, unsigned char value );
extern BOOL pm813_measured_current_means_charging(void);

void lcd_backlight_ctrl(int level);
/*********************
 *      DEFINES
 *********************/

/*Possible log level. For compatibility declare it independently from `LV_USE_LOG`*/

#define LV_LOG_LEVEL_TRACE 0 /**< A lot of logs to give detailed information*/
#define LV_LOG_LEVEL_INFO 1  /**< Log important events*/
#define LV_LOG_LEVEL_WARN 2  /**< Log if something unwanted happened but didn't caused problem*/
#define LV_LOG_LEVEL_ERROR 3 /**< Only critical issue, when the system may fail*/
#define LV_LOG_LEVEL_USER 4 /**< Custom logs from the user*/
#define LV_LOG_LEVEL_NONE 5 /**< Do not log anything*/
#define _LV_LOG_LEVEL_NUM 6 /**< Number of log levels */

LV_EXPORT_CONST_INT(LV_LOG_LEVEL_TRACE);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_INFO);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_WARN);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_ERROR);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_USER);
LV_EXPORT_CONST_INT(LV_LOG_LEVEL_NONE);

typedef int8_t lv_log_level_t;

#if LV_USE_LOG
/**********************
 *      TYPEDEFS
 **********************/

/**
 * Log print function. Receives "Log Level", "File path", "Line number", "Function name" and "Description".
 */
typedef void (*lv_log_print_g_cb_t)(lv_log_level_t level, const char *, uint32_t, const char *, const char *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register custom print/write function to call when a log is added.
 * It can format its "File path", "Line number" and "Description" as required
 * and send the formatted log message to a console or serial port.
 * @param print_cb a function pointer to print a log
 */
void lv_log_register_print_cb(lv_log_print_g_cb_t print_cb);

/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param func name of the function when the log added
 * @param format printf-like format string
 * @param ... parameters for `format`
 */
void _lv_log_add(lv_log_level_t level, const char * file, int line, const char * func, const char * format, ...);

/**********************
 *      MACROS
 **********************/

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_TRACE
#define LV_LOG_TRACE(...) _lv_log_add(LV_LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LV_LOG_TRACE(...)
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
#define LV_LOG_INFO(...) _lv_log_add(LV_LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LV_LOG_INFO(...)
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_WARN
#define LV_LOG_WARN(...) _lv_log_add(LV_LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LV_LOG_WARN(...)
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_ERROR
#define LV_LOG_ERROR(...) _lv_log_add(LV_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LV_LOG_ERROR(...)
#endif

#if LV_LOG_LEVEL <= LV_LOG_LEVEL_USER
#define LV_LOG_USER(...) _lv_log_add(LV_LOG_LEVEL_USER, __FILE__, __LINE__, __func__, __VA_ARGS__);
#else
#define LV_LOG_USER(...)
#endif

#else /*LV_USE_LOG*/

/*Do nothing if `LV_USE_LOG  0`*/
#define _lv_log_add(level, file, line, ...)
#define LV_LOG_TRACE(...)
#define LV_LOG_INFO(...)
#define LV_LOG_WARN(...)
#define LV_LOG_ERROR(...)
#define LV_LOG_USER(...)
#endif /*LV_USE_LOG*/


#define MEDIA_CLK_OFFLINE        BIT(0)
#define MEDIA_CLK_ISP2X          BIT(1)
#define MEDIA_CLK_PHY            BIT(2)
#define MEDIA_CLK_INTERFACE      BIT(3)
#define MEDIA_CLK_OFFLINE_AXI    BIT(4)
#define MEDIA_CLK_LCD_CI         BIT(5)
#define MEDIA_CLK_LCD_AHB        BIT(6)
#define MEDIA_CLK_POWER          BIT(7)
#define MEDIA_CLK_LCD_SCLK         BIT(8)
#define MEDIA_CLK_LCD_AXI         BIT(9)
#define MEDIA_CLK_JPEG_FUNC         BIT(10)

enum media_clk_module_id{
    MODULE_CAMERA = 0,
    MODULE_DISPLAY,
    MODULE_VIDEO,
    MODULE_MAX,
};

enum media_clk_dump_level{
    LEVEL_DUMP_CLOCK_ONLY = 0,
    LEVEL_DUMP_CLOCK_TIMES,
};

enum media_clk_sub_module_id{
    SUB_MODULE_OFFLINE = 0,
    SUB_MODULE_ISP2X,
    SUB_MODULE_PHY,
    SUB_MODULE_INTERFACE,
    SUB_MODULE_OFFLINE_AXI,
    SUB_MODULE_LCD_CI,
    SUB_MODULE_LCD_AHB,
    SUB_MODULE_POWER,  // multimedia power
    SUB_MODULE_LCD_SCLK,
    SUB_MODULE_LCD_AXI,
    SUB_MODULE_JPEG_FUNC,
    SUB_MODULE_MAX
};

int hal_mediaclk_set_rate(int clk_type, int clk_rate);
int hal_mediaclk_enable(int clk_type, int module_id);
int hal_mediaclk_disable(int clk_type, int module_id);
int hal_mediaclk_poweron(int module_id);
int hal_mediaclk_poweroff(int module_id);
int hal_mediaclk_dump(enum media_clk_dump_level dump_level);
void hal_mediaclk_setbit(unsigned int  reg, unsigned int  val);
void hal_mediaclk_clrbit(unsigned int  reg, unsigned int  val);
int hal_mediaclk_reset(uint32_t clk_type);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* _BSP_COMMON_H_ */
