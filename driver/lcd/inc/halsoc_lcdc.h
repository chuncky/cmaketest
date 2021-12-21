#ifndef _CRANE_LCDC_H_
#define _CRANE_LCDC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"
#include "hal_disp.h"
#include "halsoc_lcd_mipi_dsi.h"

struct lcd_info_t {
    uint32_t *mfp;
    unsigned clk;
};

struct crane_panel_t {
    int (*probe)(unsigned lcd_clk);
    void (*backlight_ctrl)(unsigned level);
    void (*panel_onoff)(int on);
    int (*readid)(unsigned *id);
    void (*flush)(struct _disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p); /*lv_hal_disp.h*/
    void (*idle_onoff)(int on);
    void (*idle_flush)(void);
};

void hallcdc_set_disp(lv_disp_t * disp);
#if USE_CRANE_LCD

enum {
    LCD_FORMAT_RGB565,
    LCD_FORMAT_RGB1555,
    LCD_FORMAT_RGB888_PACKED,
    LCD_FORMAT_RGB888_UNPACKED,
    LCD_FORMAT_RGBA888, /*ARGB*/
    LCD_FORMAT_YUV422_PACKED,
    LCD_FORMAT_YUV422_PLANAR,
    LCD_FORMAT_YUV420_PLANAR,
    LCD_FORMAT_RGB888A, /*RGBA*/
    LCD_FORMAT_YUV420_SEMI,
    LCD_FORMAT_LIMIT
};

enum {
    LCD_ALPHA_MODE_NORMAL,
    LCD_ALPHA_MODE_PRE,
    LCD_ALPHA_MODE_SPECIAL,
    LCD_ALPHA_MODE_LIMIT
};

enum {
    SPI_FORMAT_RGB565       = 0,
    SPI_FORMAT_RGB666       = 1,
    SPI_FORMAT_RGB666_2_3   = 2,
    SPI_FORMAT_RGB888       = 3,
    SPI_FORMAT_RGB888_2_3   = 4,
    SPI_FORMAT_LIMIT,
    MCU_FORMAT_RGB565 = SPI_FORMAT_LIMIT,
    MCU_FORMAT_RGB666,
    MCU_FORMAT_LIMIT,
    MIPI_FORMAT_CMD = MCU_FORMAT_LIMIT,
    MIPI_FORMAT_VIDEO,
    MIPI_FORMAT_LIMIT
};

typedef struct {
    uint8_t layer_enable;
    uint8_t alpha;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t stride;
    uint32_t format;
    void *buf1;
    void *buf2; // for yuv mode
    void *buf3; // for yuv mode
    uint16_t dst_width;
    uint16_t dst_height;
}lcd_layer_config;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t data_lane_num; /*1 or 2*/
    uint8_t format;
    uint8_t te_pol;
    uint8_t hbp;
    uint8_t hfp;
    uint8_t hsync;
    uint8_t vbp;
    uint8_t vfp;
    uint8_t vsync;
}lcd_output_config;

typedef struct {
    uint8_t wb_enable;
    uint8_t color_format;
    uint16_t stride;
    uint16_t width;
    uint16_t height;
    uint8_t *wb_buf;
}lcd_wb_config;

typedef struct {
    lcd_layer_config layer_config_img;
    lcd_layer_config layer_config_osd1;
    lcd_output_config output_config;
    lcd_wb_config wb_config;
    unsigned bg_color;
    unsigned alpha_mode;
}lcd_context;


struct crane_lcd_t {
    lv_disp_drv_t disp_drv;
    void (*backlight_ctrl)(int level);
    void (*lcdc_onoff)(int on);
    void (*panel_onoff)(int on);
    unsigned (*readid)(void);
};

extern struct crane_lcd_t crane_lcd;

int hal_lcdc_sync(lcd_context *lcd_ctx, int polling_mode);
void hal_lcdc_onoff(int on);
void hal_lcdc_reset_panel(void);
int hal_lcdc_init(struct lcd_info_t *info);
void lcdc_update_output_setting(lcd_output_config *output_config, uint32_t width, uint32_t height);
#if USE_CRANE_LCD_MIPI
void lcdc_update_mipi_output_setting(lcd_output_config *output_config, const struct s_mipi_info *mipi);
#endif
unsigned lcdc_rgb565_scale(int32_t src_width,
                   int32_t src_height,
                   int32_t dst_width,
                   int32_t dst_height,
                   uint8_t *src_buf,
                   uint8_t *dst_buf);
unsigned lcdc_rgb888_2_rgb565_scale(int32_t src_width,
                   int32_t src_height,
                   int32_t dst_width,
                   int32_t dst_height,
                   uint8_t *src_buf,
                   uint8_t *dst_buf);

int hal_lcdc_stop(lcd_context *lcd_ctx);

#endif /* USE_CRANE_LCD */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
