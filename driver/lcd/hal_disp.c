#if 1
#include <stdio.h>
#include "hal_conf.h"
#include "loadtable.h"
#include "guilin_lite.h"
#include "bsp_common.h"
#include "halsoc_lcdc.h"
#include "st7789v_spi.h"
#include "st7789v_mcu.h"
#include "st7735s_spi.h"
#include "st7567_spi.h"
#include "rm69330_mipi.h"
#include "st7796s_mipi.h"
#include "icna3310_mipi.h"
#include "ili9806e_mipi.h"
#include "hal_board.h"
extern int uart_printf(const char* fmt, ...);


extern void HalTaskSleep(UINT32 time);


static int backlight_valid = 0;
static int backlight_level_cur = 0;
static int backlight_level_last = 0;


void hal_lcd_backlight(int level)
{
    uart_printf("lcd_backlight [%d]: %d\n", backlight_valid, level);
    lcd_backlight_ctrl(level);
}

/* PWM registers and bits definitions */
#define PWM_BASE                        0xD401A000
#define PWMCR                           0x00
#define PWMDCR                          0x04
#define PWMPCR                          0x08
#define PWMCR_SD                        (1 << 6)
#define PWMDCR_FD                       (1 << 10)

/* apb clk */
#define APBCLK_PWM_BASE                 0xD401500C
#define FCLK                            (1 << 1)
#define APBCLK                          (1 << 0)
#define RESET                           (1 << 2)

/* using 13M function clk */
#define PERIOD_FACTOR_FOR_13M           769

static uint8_t pwm_apb_clk[PWM_DEV_MAX];

static void
apbclk_disable(void)
{
  unsigned val;

  if((pwm_apb_clk[0] == 0) && (pwm_apb_clk[1] == 0)) {
    val = readl(APBCLK_PWM_BASE);
    if(val & APBCLK) {
      printf("apb clk 0 disable\n");
      writel(RESET, APBCLK_PWM_BASE);
    }
  }

  if((pwm_apb_clk[2] == 0) && (pwm_apb_clk[3] == 0)) {
    val = readl(APBCLK_PWM_BASE + 0x8);
    if(val & APBCLK) {
      printf("apb clk 2 disable\n");
      writel(RESET, APBCLK_PWM_BASE + 0x8);
    }
  }
}
static void
apbclk_enable(void)
{
  unsigned val;

  if((pwm_apb_clk[0] == 1) || (pwm_apb_clk[1] == 1)) {
    val = readl(APBCLK_PWM_BASE);
    if((val & APBCLK) == 0) {
      printf("apb clk 0 enable\n");
      writel((val | APBCLK) & (~RESET), APBCLK_PWM_BASE);
    }
  }

  if((pwm_apb_clk[2] == 1) || (pwm_apb_clk[3] == 1)) {
    val = readl(APBCLK_PWM_BASE + 0x8);
    if((val & APBCLK) == 0) {
      printf("apb clk 2 enable\n");
      writel((val | APBCLK) & (~RESET), APBCLK_PWM_BASE + 0x8);
    }
  }
}
static void hal_clk_enable(int id)
{
  pwm_apb_clk[id] = 1;
  apbclk_enable();
  writel(FCLK | APBCLK, APBCLK_PWM_BASE + 0x4 * id);
}
static void hal_clk_disable(int id)
{
  unsigned val;
  pwm_apb_clk[id] = 0;
  val = readl(APBCLK_PWM_BASE + 0x4 * id);
  writel(val & ~FCLK, APBCLK_PWM_BASE + 0x4 * id);
  apbclk_disable();
}
int hal_pwm_enable(pwm_dev_t id, int duty_ns, int period_ns)
{
  unsigned period_cycles, prescale, pv, dc;
  unsigned reg_base;
  
  if(id >= PWM_DEV_MAX) {
    printf("pwm_enable: invalid id%d\n", id);
    return -1;
  }
  reg_base = PWM_BASE + 0x400 * id;
  if(duty_ns == 0) {
    writel(0x0, reg_base + PWMDCR);
    return 0;
  }

  period_cycles = (period_ns * 10) / PERIOD_FACTOR_FOR_13M;

  if(period_cycles < 1) {
    period_cycles = 1;
  }
  prescale = (period_cycles - 1) / 1024;
  pv = period_cycles / (prescale + 1) - 1;

  if(prescale > 63) {
    printf("pwm_enable: invalid prescale %u\n", prescale);
    return -1;
  }

  if(duty_ns == period_ns) {
    dc = PWMDCR_FD;
  } else {
    dc = (pv + 1) * duty_ns / period_ns;
  }

  hal_clk_enable(id);

  writel(prescale, reg_base + PWMCR);
  writel(dc, reg_base + PWMDCR);
  writel(pv, reg_base + PWMPCR);
  
  return 0;
}
void hal_pwm_disable(pwm_dev_t id)
{
  if(id >= PWM_DEV_MAX) {
    printf("pwm_disable: invalid id%d\n", id);
    return;
  }
  hal_pwm_enable(id, 0, 0);
  hal_clk_disable(id);
}


/**
 * Initialize a display driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void hal_disp_drv_init(lv_disp_drv_t * driver)
{
    memset(driver,0x00, sizeof(lv_disp_drv_t));

    driver->flush_cb         = NULL;
    driver->hor_res          = LV_HOR_RES_MAX;
    driver->ver_res          = LV_VER_RES_MAX;
    driver->buffer           = NULL;
    driver->rotated          = 0;
    driver->color_chroma_key = LV_COLOR_TRANSP;
    driver->dpi = LV_DPI;

#if LV_ANTIALIAS
    driver->antialiasing = true;
#endif

#if LV_COLOR_SCREEN_TRANSP
    driver->screen_transp = 1;
#endif

#if LV_USE_GPU
    driver->gpu_blend_cb = NULL;
    driver->gpu_fill_cb  = NULL;
#endif

#if LV_USE_USER_DATA
    driver->user_data = NULL;
#endif

    driver->set_px_cb = NULL;
}


/**
 * Initialize a display buffer
 * @param disp_buf pointer `lv_disp_buf_t` variable to initialize
 * @param buf1 A buffer to be used by LVGL to draw the image.
 *             Always has to specified and can't be NULL.
 *             Can be an array allocated by the user. E.g. `static lv_color_t disp_buf1[1024 * 10]`
 *             Or a memory address e.g. in external SRAM
 * @param buf2 Optionally specify a second buffer to make image rendering and image flushing
 *             (sending to the display) parallel.
 *             In the `disp_drv->flush` you should use DMA or similar hardware to send
 *             the image to the display in the background.
 *             It lets LVGL to render next frame into the other buffer while previous is being
 * sent. Set to `NULL` if unused.
 * @param size_in_px_cnt size of the `buf1` and `buf2` in pixel count.
 */
void haldisp_buf_init(lv_disp_buf_t * disp_buf, void * buf1, void * buf2, uint32_t size_in_px_cnt)
{
    memset(disp_buf, 0x00,sizeof(lv_disp_buf_t));

    disp_buf->buf1    = buf1;
    disp_buf->buf2    = buf2;
    disp_buf->buf_act = disp_buf->buf1;
    disp_buf->size    = size_in_px_cnt;
}

/**
 * Call in the display driver's `flush_cb` function when the flushing is finished
 * @param disp_drv pointer to display driver in `flush_cb` where this function is called
 */
LV_ATTRIBUTE_FLUSH_READY void lv_disp_flush_ready(lv_disp_drv_t * disp_drv)
{
    /*If the screen is transparent initialize it when the flushing is ready*/
#if LV_COLOR_SCREEN_TRANSP
    if(disp_drv->screen_transp) {
        memset(disp_drv->buffer->buf_act, 0x00, disp_drv->buffer->size * sizeof(lv_color32_t));
    }
#endif

    disp_drv->buffer->flushing = 0;
    disp_drv->buffer->flushing_last = 0;
}



#if USE_SDK_API
/* override SDK config */
#if !defined(MINIGUI)
unsigned char mmiPool[SDK_MMI_POOL_SIZE];
const unsigned int mmiPoolSize = SDK_MMI_POOL_SIZE;
#endif
#endif

#define GUI_TASK_PRIORITY    200
//TASK_HANDLE *gui_task_id;
/*supported panels */
static struct crane_panel_t *panels[] = {
#if USE_LCD_PANEL_ST7735S_SPI_POC
    &st7735s_spi,
#elif USE_LCD_PANEL_ST7567_SPI_4WIRE_1LANE_1IF
    &st7567_spi,
#elif USE_LCD_PANEL_ST7789V_MCU
    &st7789v_mcu,
#elif USE_LCD_PANEL_RM69330_MIPI
    &rm69330_mipi,
#elif USE_LCD_PANEL_ICNA3310_MIPI
    &icna3310_mipi,
#elif USE_LCD_PANEL_ILI9806E_MIPI
    &ili9806e_mipi,
#elif USE_LCD_PANEL_ST7796S_MIPI
    &st7796s_mipi,
#else
    &st7789v_spi,
#endif
};

#if USE_LCD_PANEL_ST7789V_SPI_WATCH
static uint32_t lcd_watch_mfp_cfgs[] = {
    MFP_REG(GPIO_20) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DCLK */
    MFP_REG(GPIO_21) | MFP_AF0 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS1 */
    MFP_REG(GPIO_22) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS0 */
    MFP_REG(GPIO_23) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DOUT */
    MFP_REG(GPIO_24) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DIN0 */
    MFP_REG(GPIO_25) | MFP_AF2 | MFP_PULL_LOW  | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* VSYNC */
    MFP_REG(GPIO_26) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SMPN_RST */
    MFP_EOC
};

static struct lcd_info_t lcd_info = {
    .mfp = (uint32_t *)lcd_watch_mfp_cfgs,
    .clk = 312000, /* 104000 */
};
#endif

#if USE_LCD_PANEL_ST7789V_SPI_PHONE
static uint32_t lcd_mfp_cfgs[] = {
    MFP_REG(GPIO_20) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DCLK */
    MFP_REG(GPIO_21) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS1 */
    MFP_REG(GPIO_22) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS0 */
    MFP_REG(GPIO_23) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DOUT */
    MFP_REG(GPIO_24) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DIN0 */
    MFP_REG(GPIO_25) | MFP_AF2 | MFP_PULL_LOW  | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* VSYNC */
    MFP_REG(GPIO_26) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SMPN_RST */
    MFP_REG(GPIO_27) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE,
    MFP_REG(GPIO_69_TDS_DIO2) | MFP_AF0 | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE,
    MFP_EOC
};

static struct lcd_info_t lcd_info = {
    .mfp = (uint32_t *)lcd_mfp_cfgs,
    .clk = 312000, /* 104000 */
};
#endif

#if USE_LCD_PANEL_ST7789V_MCU
static uint32_t lcd_mfp_cfgs[] = {
    MFP_REG(GPIO_22) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*CS0*/
    MFP_REG(GPIO_26) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*RST*/
    MFP_REG(GPIO_20) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB0*/
    MFP_REG(GPIO_31) | MFP_AF3 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB1*/
    MFP_REG(GPIO_32) | MFP_AF3 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB2*/
    MFP_REG(GPIO_121) | MFP_AF3 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB3*/
    MFP_REG(GPIO_124) | MFP_AF3 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB4*/
    MFP_REG(GPIO_21) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB5*/
    MFP_REG(GPIO_27) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB6*/
    MFP_REG(GPIO_28) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DB7*/
    MFP_REG(GPIO_23) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*RDB*/
    MFP_REG(GPIO_24) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*WRB*/
    MFP_REG(GPIO_SLAVE_RESET_OUT_N) | MFP_AF5 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*DCX*/
#if LCD_TE_ENABLE
    MFP_REG(GPIO_25) | MFP_AF6 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*TE*/
#endif
    MFP_EOC
};

struct lcd_info_t lcd_info = {
    .mfp = (uint32_t *)lcd_mfp_cfgs,
    .clk = 52000, /* 52000 */
};
#endif


#if USE_LCD_PANEL_ST7735S_SPI_POC
static uint32_t lcd_poc_mfp_cfgs[] = {
    MFP_REG(GPIO_20) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DCLK */
    MFP_REG(GPIO_27) | MFP_AF0 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS1 */
    MFP_REG(GPIO_22) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS0 */
    MFP_REG(GPIO_23) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DOUT */
    MFP_REG(GPIO_24) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DIN0 */
    MFP_REG(GPIO_25) | MFP_AF2 | MFP_PULL_LOW  | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* VSYNC */
    MFP_REG(GPIO_26) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SMPN_RST */
    MFP_EOC
};

static struct lcd_info_t lcd_info = {
    .mfp = (uint32_t *)lcd_poc_mfp_cfgs,
    .clk = 104000, /* 104000 */
};
#endif

#if USE_LCD_PANEL_ST7567_SPI_4WIRE_1LANE_1IF
static uint32_t lcd_poc_mfp_cfgs[] = {
    MFP_REG(GPIO_20) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DCLK */
    MFP_REG(GPIO_21) | MFP_AF0 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS1 */
    MFP_REG(GPIO_22) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_CS0 */
    MFP_REG(GPIO_23) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DOUT */
    MFP_REG(GPIO_24) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SPI_DIN0 */
    //MFP_REG(GPIO_25) | MFP_AF2 | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* VSYNC */
    MFP_REG(GPIO_26) | MFP_AF2 | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_DRIVE_MEDIUM | MFP_LPM_EDGE_NONE, /* SMPN_RST */
    MFP_EOC
};

static struct lcd_info_t lcd_info = {
    .mfp = (uint32_t *)lcd_poc_mfp_cfgs,
    .clk = 104000, /* 104000 */
};
#endif

#if USE_CRANE_LCD_MIPI
unsigned int lcd_mipi_mfp_cfgs[] = {
    MFP_REG(GPIO_26) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*RST*/
#if LCD_TE_ENABLE
    MFP_REG(GPIO_25) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*TE*/
#endif
#if LCD_BACKLIGHT_ENABLE
    MFP_REG(GPIO_31) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*pwm for backlight*/
    //MFP_REG(GPIO_31) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*config to gpio before pwm ready*/
#endif
    MFP_EOC
};

static struct lcd_info_t lcd_info = {
    .mfp = (uint32_t *)lcd_mipi_mfp_cfgs,
#if USE_CRANE_LCD_MIPI_CMD
    .clk = 26000,
#else
    .clk = 17300,
#endif
};
#endif

#if USE_LCD_PANEL_ICNA3310_MIPI
unsigned int oled_power_mfp_cfgs[] = {
    MFP_REG(GPIO_76) | MFP_AF1 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH | MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE, /*power*/
    MFP_EOC
};





static void oled_power_set(unsigned char on)
{
    uint32_t *mfp;

    mfp = (uint32_t *)&oled_power_mfp_cfgs;

    hal_mfp_config((uint32_t *)mfp);
    gpio_set_direction(GPIO76, GPIO_OUT);
    gpio_output_set(GPIO76, on);
    printf("oled_power_set=%d\n",on);
}
#endif

__attribute__((__used__)) static const uint32_t pm812_bg_mfp_cfgs[] = {
    MFP_REG(GPIO_77) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_LPM_EDGE_NONE,
    MFP_EOC /*End of configuration, must have */
};

__attribute__((__used__)) static const uint32_t pm802_bg_mfp_cfgs[] = {
    MFP_REG(GPIO_84) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW | MFP_LPM_EDGE_NONE,
    MFP_EOC /*End of configuration, must have */
};


#if USE_PWM
/* CraneG evb backlight config */
#define LEDK_PWM                          GPIO_31
#define LEDK_PWM_GPIO                     GPIO31
#define PWM_DEV_BACKLIGHT                 PWM_DEV_0
#define PERIOD_NS                         33333 /* 30KHz */
#define LEVEL_MAX                         5

__attribute__((__used__)) static const uint32_t pwm_bg_mfp_cfgs[] = {
    MFP_REG(LEDK_PWM) | MFP_AF2 | MFP_DRIVE_MEDIUM | MFP_PULL_HIGH,
    MFP_EOC /*End of configuration, must have */
};

__attribute__((__used__)) static const uint32_t pwm_bgoff_mfp_cfgs[] = {
    MFP_REG(LEDK_PWM) | MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW,
    MFP_EOC /*End of configuration, must have */
};
#endif


extern void GuilinLcdBackLightStatusRecord(UINT8 status);
extern void GuilinLcdBackLightLevelRecord(UINT8 level);


static void lcdc_pwr_onoff(int on)
{
    int pm_type = pmic_get_type();

    if (pm_type == PM_812) { // PM812
        if (on) {
            /* enable ldo8 and set t0 2.8V */
            PM812_Ldo_8_set_2_8();
            PM812_Ldo_8_set(1);
        }
        else {
            /* disable ldo8 */
            PM812_Ldo_8_set(0);
        }
    }
    else if (pm_type == PM_813) {  // PM813
        if (on) {
            Ningbo_Ldo_1_set_2_8();
            Ningbo_Ldo_1_set(1);
#if USE_LCD_PANEL_ICNA3310_MIPI
            oled_power_set(1);
#endif
        }
        else {
            Ningbo_Ldo_1_set(0);
#if USE_LCD_PANEL_ICNA3310_MIPI
            oled_power_set(0);
#endif
        }
    }
    else if (pm_type == PM_802) {  // PM802
        /* ldo is enabled by default */
    }
    else if (pm_type == PM_803) {  // PM803
        GuilinLite_Ldo_6_set_2_8();
        GuilinLite_Ldo_6_set(TRUE);
    }
    else {
        printf("unknown PMIC, lcd_pwr_onoff failed.\n");
    }
}

static struct crane_panel_t *hal_lcd_init(void)
{
    int cnt, i;
    struct crane_panel_t *p;

    lcdc_pwr_onoff(1);
    if (hal_lcdc_init(&lcd_info) < 0) {
        goto error;
    }

    cnt = ARRAY_SIZE(panels);

    for (i = 0; i < cnt; i++) {
        p = panels[i];
        if (p->probe(lcd_info.clk) == 0) {
            return p;
        }
    }

error:
    return NULL;
}

struct crane_panel_t *lcd = NULL;

void lcdc_resume(void)
{
    hal_lcdc_init(&lcd_info);
    lcd->probe(lcd_info.clk);
}

void lcd_idle_refresh(void)
{
    if(lcd && lcd->idle_flush) {
        lcd->idle_flush();
    }
}

int pm_state_flag = -1;
static uint8_t pm_state = PM_ON;
#define OSA_SUSPEND             0xFFFFFFFF
static int  sdk_c1_enter()
{
    if(pm_state == PM_ON)
        return 1;

    return 0;
}

static int  sdk_c1_exit()
{
    return 0;
}

static int sdk_d2_enter()
{
#if USE_CRANE_KEYPADC
    keypad_clear_wakup_interrupt();
#endif
    if(pm_state == PM_ON) {
        return 1;
    }

    return 0;
}

static int sdk_d2_exit(BOOL ExitFromD2)
{
    return 0;
}

static int sdk_uiSuspend(void)
{
    if(pm_state == PM_ON)
        return 1;

    return 0;
}

extern void pm_unlock(void);



static void gui_disp_wait(struct _disp_drv_t * disp_drv)
{
    (void)disp_drv;
    //HalTaskSleep(1);
    uart_printf("Call gui_disp_wait\r\n");
}

#if USE_LV_WATCH_REFR_MONITOR_CB
/** OPTIONAL: Called after every refresh cycle to tell the rendering and flushing time + the
 * number of flushed pixels */
static void refr_monitor_cb(struct _disp_drv_t * disp_drv, uint32_t time, uint32_t px)
{
    (void)disp_drv;
    printf("t: %u, pixel: %u\n", (unsigned)time, (unsigned)px);
}
#endif


struct asrlcdd_screen_info{
    unsigned int width;
    unsigned int height;
};
extern void ASRLCDD_SetUIInfo(struct asrlcdd_screen_info *info);
//extern unsigned char *g_asrlcd_framebuffer;
 /*Create a display*/
lv_disp_drv_t fotadisp_drv;

typedef unsigned short UINT16;
 lv_area_t fotaarea;
#define LCD_COLOR_DEPTH_BYTE (LV_COLOR_DEPTH / 8)

void fotaLcdflush(UINT16* buffer, UINT16 startx,UINT16 starty,UINT16 endx,UINT16 endy)
{
    lv_disp_buf_t * vdb = fotadisp_drv.buffer;
    lv_area_t *area_t =&fotaarea;
    uint32_t width, height, stride;
    unsigned buf_len;
    lv_color_t *color_t=(lv_color_t *)vdb->buf_act;

    printf("fotaLcdflush+++:0x%x,0x%x,0x%x,0x%x\n",color_t,vdb->buf_act,vdb->buf1,vdb->buf2);

    area_t->x1=startx;
    area_t->x2=endx;
    area_t->y1=starty;
    area_t->y2=endy;

    width = area_t->x2 - area_t->x1 + 1;
    height = area_t->y2 - area_t->y1 + 1;
#if (LV_COLOR_DEPTH == 16)
    stride = width * LCD_COLOR_DEPTH_BYTE;
#else
    printf("lv config is not 16 bits color depth !!!!!!!!!!!\n");
    return;
#endif
    buf_len = stride * height;
    memcpy(vdb->buf_act,buffer,buf_len);
    vdb->flushing = 1;
    fotadisp_drv.flush_cb(&fotadisp_drv,area_t,color_t);



    if(vdb->buf1 && vdb->buf2) {
        if(vdb->buf_act == vdb->buf1)
            vdb->buf_act = vdb->buf2;
        else
            vdb->buf_act = vdb->buf1;
    }
    printf("fotaLcdflush----:0x%x,0x%x,0x%x,0x%x\n",color_t,vdb->buf_act,vdb->buf1,vdb->buf2);

}


//__attribute__((aligned (32))) unsigned char lcd_framebuffer[LV_HOR_RES_MAX*LV_VER_RES_MAX*2];	//fixme-asr:rel-190311

lv_disp_t fotadisp; 
unsigned char *g_asrlcd_framebuffer;
static lv_color_t asrlcd_framebuffer[LV_HOR_RES_MAX * LV_VER_RES_MAX];			/*An other screen sized buffer*/

/*Create a display buffer*/
static lv_disp_buf_t disp_buf;
//static lv_color_t buf1[LV_HOR_RES_MAX * LV_VER_RES_MAX];			/*A screen sized buffer*/
//static lv_color_t buf2[LV_HOR_RES_MAX * LV_VER_RES_MAX];			/*An other screen sized buffer*/


int hal_getlcdinfo(unsigned int *width, unsigned int *height)
{

	*width = LV_HOR_RES_MAX;
	*height = LV_VER_RES_MAX;

	return 0;
}

int fotalcd_init(void)
{

    lv_disp_t * disp =&fotadisp;
    uart_printf("%s-01\n\r",__func__);
    lcd = hal_lcd_init();
    if (!lcd) {
        printf("lcd init failed\n");
        //return -1;
    }

    static lv_color_t buf1[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*A screen sized buffer*/
    static lv_color_t buf2[LV_HOR_RES_MAX * LV_VER_RES_MAX];            /*An other screen sized buffer*/

    uart_printf("%s-02,0x%x,0x%x\n\r",__func__,buf1,buf2);

    haldisp_buf_init(&disp_buf, buf1, buf2, LV_HOR_RES_MAX * LV_VER_RES_MAX);   /*Initialize the display buffer*/
    uart_printf("%s-03\n\r",__func__);
    /*Create a display*/

    hal_disp_drv_init(&fotadisp_drv);            /*Basic initialization*/
    fotadisp_drv.buffer = &disp_buf;
    fotadisp_drv.hor_res = LV_HOR_RES_MAX;
    fotadisp_drv.ver_res = LV_VER_RES_MAX;
    if(lcd) fotadisp_drv.flush_cb = lcd->flush;
    fotadisp_drv.wait_cb = gui_disp_wait;
#if USE_LV_WATCH_REFR_MONITOR_CB
    fotadisp_drv.monitor_cb = refr_monitor_cb;
#endif
    uart_printf("%s-04\n\r",__func__);
    //register lcd driver.
    memset(disp,0x00, sizeof(lv_disp_t));
    memcpy(&disp->driver, &fotadisp_drv, sizeof(lv_disp_drv_t));
    disp->last_activity_time = 0;
    hallcdc_set_disp(disp);
    uart_printf("%s-05\n\r",__func__);
    g_asrlcd_framebuffer=(unsigned char *)asrlcd_framebuffer;
    //g_asrlcd_framebuffer=lcd_framebuffer;
    hal_pmic_init();
    uart_printf("%s-06\n\r",__func__);
    mdelay(100); /* wait for the logo display complete */
    hal_lcd_backlight(2);


    printf("%s-07\n",__func__);
    return 0;

}

#endif
