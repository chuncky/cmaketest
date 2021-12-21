
#include "hal_conf.h"
#include "hal_drv_conf.h"
#include "st7789v_spi.h"
#include "hal_disp.h"

#if USE_LCD_PANEL_ST7789V_SPI_WATCH || USE_LCD_PANEL_ST7789V_SPI_PHONE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "halsoc_lcdc.h"
#include "halsoc_lcd_spi.h"
#include "bsp_common.h"


#define LCD_COLOR_DEPTH_BYTE (LV_COLOR_DEPTH / 8)

enum {
    SPI_WRITE_CMD = 0,
    SPI_WRITE_DATA,
    SPI_WRITE_DELAY
};

struct spi_write_t {
    uint8_t type;  /* SPI_WRITE_CMD / SPI_WRITE_DATA / SPI_WRITE_DELAY */
    uint8_t value;
    uint8_t data_length; /* Only for SPI_WRITE_CMD */
};

static lv_disp_drv_t * lcd_disp_drv = NULL;

static lcd_context g_lcd_ctx;
const static struct spi_write_t spi_init_watch[] = {
    { SPI_WRITE_CMD, 0x11, 0 },
    { SPI_WRITE_DELAY, 10, 0 },
    { SPI_WRITE_CMD, 0x36, 1 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_CMD, 0x3A, 1 },
    { SPI_WRITE_DATA, 0x05, 0 },  /* Format is RGB565, if for RGB666, 0x05 -> 0x06 */
    { SPI_WRITE_CMD, 0x35, 1 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_CMD, 0xC7, 1 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_CMD, 0xCC, 1 },
    { SPI_WRITE_DATA, 0x09, 0 },
    { SPI_WRITE_CMD, 0xB2, 5 },
    { SPI_WRITE_DATA, 0x2A, 0 },
    { SPI_WRITE_DATA, 0x2C, 0 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_DATA, 0x33, 0 },
    { SPI_WRITE_DATA, 0x33, 0 },
    { SPI_WRITE_CMD, 0xB7, 1 },
    { SPI_WRITE_DATA, 0x75, 0 },  /* vgh=13.26 vgl=-12.5 */
    { SPI_WRITE_CMD, 0xBB, 1 },
    { SPI_WRITE_DATA, 0x3d, 0 },
    { SPI_WRITE_CMD, 0xC0, 1 },
    { SPI_WRITE_DATA, 0x2C, 0 },
    { SPI_WRITE_CMD, 0xC2, 1 },
    { SPI_WRITE_DATA, 0x01, 0 },
    { SPI_WRITE_CMD, 0xC3, 1 },
    { SPI_WRITE_DATA, 0x19, 0 },
    { SPI_WRITE_CMD, 0xC4, 1 },
    { SPI_WRITE_DATA, 0x20, 0 },
    { SPI_WRITE_CMD, 0xC6, 1 },
    { SPI_WRITE_DATA, 0x1d, 0 },   /* 44hz */
    { SPI_WRITE_CMD, 0xD0, 2 },
    { SPI_WRITE_DATA, 0xA4, 0 },
    { SPI_WRITE_DATA, 0xA1, 0 },
    { SPI_WRITE_CMD, 0xE0, 14 },
    { SPI_WRITE_DATA, 0xD0, 0 },
    { SPI_WRITE_DATA, 0x17, 0 },
    { SPI_WRITE_DATA, 0x19, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x03, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x32, 0 },
    { SPI_WRITE_DATA, 0x41, 0 },
    { SPI_WRITE_DATA, 0x43, 0 },
    { SPI_WRITE_DATA, 0x09, 0 },
    { SPI_WRITE_DATA, 0x14, 0 },
    { SPI_WRITE_DATA, 0x12, 0 },
    { SPI_WRITE_DATA, 0x33, 0 },
    { SPI_WRITE_DATA, 0x2C, 0 },
    { SPI_WRITE_CMD, 0xE1, 14 },
    { SPI_WRITE_DATA, 0xD0, 0 },
    { SPI_WRITE_DATA, 0x18, 0 },
    { SPI_WRITE_DATA, 0x17, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x03, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x31, 0 },
    { SPI_WRITE_DATA, 0x46, 0 },
    { SPI_WRITE_DATA, 0x43, 0 },
    { SPI_WRITE_DATA, 0x09, 0 },
    { SPI_WRITE_DATA, 0x14, 0 },
    { SPI_WRITE_DATA, 0x13, 0 },
    { SPI_WRITE_DATA, 0x31, 0 },
    { SPI_WRITE_DATA, 0x2D, 0 },
//    { SPI_WRITE_CMD, 0xE7, 1 },
//    { SPI_WRITE_DATA, 0x11, 0 },  /* data_lane_num = 2 */
    { SPI_WRITE_CMD, 0x29, 0 },
};

const static struct spi_write_t spi_init_evb[] = {
    { SPI_WRITE_CMD, 0x11, 0 },
    { SPI_WRITE_DELAY, 10, 0 },
    { SPI_WRITE_CMD, 0x36, 1 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_CMD, 0x3A, 1 },
    { SPI_WRITE_DATA, 0x05, 0 },  /* Format is RGB565, if for RGB666, 0x05 -> 0x06 */
    { SPI_WRITE_CMD, 0x35, 1 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_CMD, 0xC7, 1 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_CMD, 0xCC, 1 },
    { SPI_WRITE_DATA, 0x09, 0 },
    { SPI_WRITE_CMD, 0xB2, 5 },
    { SPI_WRITE_DATA, 0x0C, 0 },
    { SPI_WRITE_DATA, 0x0C, 0 },
    { SPI_WRITE_DATA, 0x00, 0 },
    { SPI_WRITE_DATA, 0x33, 0 },
    { SPI_WRITE_DATA, 0x33, 0 },
    { SPI_WRITE_CMD, 0xB7, 1 },
    { SPI_WRITE_DATA, 0x35, 0 },  /* vgh=13.26 vgl=-12.5 */
    { SPI_WRITE_CMD, 0xBB, 1 },
    { SPI_WRITE_DATA, 0x36, 0 },
    { SPI_WRITE_CMD, 0xC0, 1 },
    { SPI_WRITE_DATA, 0x2C, 0 },
    { SPI_WRITE_CMD, 0xC2, 1 },
    { SPI_WRITE_DATA, 0x01, 0 },
    { SPI_WRITE_CMD, 0xC3, 1 },
    { SPI_WRITE_DATA, 0x0D, 0 },
    { SPI_WRITE_CMD, 0xC4, 1 },
    { SPI_WRITE_DATA, 0x20, 0 },
    { SPI_WRITE_CMD, 0xC6, 1 },
    { SPI_WRITE_DATA, 0x0F, 0 },   /* 60hz */
    { SPI_WRITE_CMD, 0xD0, 2 },
    { SPI_WRITE_DATA, 0xA4, 0 },
    { SPI_WRITE_DATA, 0xA1, 0 },
    { SPI_WRITE_CMD, 0xE0, 14 },
    { SPI_WRITE_DATA, 0xD0, 0 },
    { SPI_WRITE_DATA, 0x17, 0 },
    { SPI_WRITE_DATA, 0x19, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x03, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x32, 0 },
    { SPI_WRITE_DATA, 0x41, 0 },
    { SPI_WRITE_DATA, 0x43, 0 },
    { SPI_WRITE_DATA, 0x09, 0 },
    { SPI_WRITE_DATA, 0x14, 0 },
    { SPI_WRITE_DATA, 0x12, 0 },
    { SPI_WRITE_DATA, 0x33, 0 },
    { SPI_WRITE_DATA, 0x2C, 0 },
    { SPI_WRITE_CMD, 0xE1, 14 },
    { SPI_WRITE_DATA, 0xD0, 0 },
    { SPI_WRITE_DATA, 0x18, 0 },
    { SPI_WRITE_DATA, 0x17, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x03, 0 },
    { SPI_WRITE_DATA, 0x04, 0 },
    { SPI_WRITE_DATA, 0x31, 0 },
    { SPI_WRITE_DATA, 0x46, 0 },
    { SPI_WRITE_DATA, 0x43, 0 },
    { SPI_WRITE_DATA, 0x09, 0 },
    { SPI_WRITE_DATA, 0x14, 0 },
    { SPI_WRITE_DATA, 0x13, 0 },
    { SPI_WRITE_DATA, 0x31, 0 },
    { SPI_WRITE_DATA, 0x2D, 0 },
    { SPI_WRITE_CMD, 0xE7, 1 },
    { SPI_WRITE_DATA, 0x11, 0 },  /* data_lane_num = 2 */
    { SPI_WRITE_CMD, 0x29, 0 },
};


const static struct spi_write_t spi_display_on_cmds[] = {
    { SPI_WRITE_CMD, 0x11, 0 },
    { SPI_WRITE_DELAY, 10, 0 },
    { SPI_WRITE_CMD, 0x29, 0 },
    { SPI_WRITE_DELAY, 10, 0 }
};

const static struct spi_write_t spi_display_off_cmds[] = {
    { SPI_WRITE_CMD, 0x28, 0 },
    { SPI_WRITE_DELAY, 10, 0 },
    { SPI_WRITE_CMD, 0x10, 0 },
    { SPI_WRITE_DELAY, 5, 0 }
};

const static struct spi_timing lcd_st7789v_spi_timing = {
    52000, /* 52000, 62500, 31200, 26000 KHz */
    5000, /* KHz*/
};

const static struct spi_info lcd_st7789v_spi_info_evb = {
    3,
    1,
    2,
    SPI_FORMAT_RGB565,
    0,
    SPI_EDGE_RISING,
    SPI_ENDIAN_MSB,
    0,
    &lcd_st7789v_spi_timing,
};

const static struct spi_info lcd_st7789v_spi_info_watch = {
    4,
    1,
    1,
    SPI_FORMAT_RGB565,
    0,
    SPI_EDGE_RISING,
    SPI_ENDIAN_MSB,
    0,
    &lcd_st7789v_spi_timing,
};

#define ID_TABLE_END 0

unsigned st7789v_id_table[] = {
    0x858552,
    ID_TABLE_END,
};

extern void lcdc_resume(void);


static uint8_t suspend_flag = 0;
static void st7789v_suspend(void *arg)
{
    struct crane_panel_t *p = arg;
    printf("st7789v_suspend\n");

    /*since disp_refr can be changed by other register,
     *so if st7789v suspend need use lcd_disp_drv instead of lv_refr_vdb_is_flushing()
     *refer to lv_disp_refr_task() and lv_refr_vdb_flush()*/
    if(lcd_disp_drv && lcd_disp_drv->buffer) {
        // wait flushing finish~~
        while(lcd_disp_drv->buffer->flushing)
            ;
    }

    p->panel_onoff(0);
    suspend_flag = 1;
}

static void st7789v_resume(void *arg)
{
    struct crane_panel_t *p = arg;
    printf("st7789v_resume\n");
    lcdc_resume();
    p->panel_onoff(1);
    suspend_flag = 0;
    lv_obj_invalidate(lv_disp_get_scr_act(NULL));
    lv_refr_now(_lv_refr_get_disp_refreshing());

    if(lcd_disp_drv && lcd_disp_drv->buffer) {
        // wait flushing finish~~
        while(lcd_disp_drv->buffer->flushing)
            ;
    }
}

static void spi_write_function(const struct spi_write_t *spi_data, uint32_t length)
{
    uint16_t i = 0;
    uint8_t data_idx = 0;
    uint8_t data_length = 0;
    const struct spi_write_t *pdata = spi_data;

    if (pdata == NULL || length == 0) {
        printf("%s: %d: param is error!! \n", __FUNCTION__, __LINE__);
        return;
    }

    for (i = 0; i < length;) {
        i++;
        if (pdata->type == SPI_WRITE_CMD) {
            lcd_spi_write_cmd(pdata->value, 8);
            if (pdata->data_length > 0) {
                data_length = pdata->data_length;
                pdata++;
                for (data_idx = 0; (data_idx < data_length) && (i < length); data_idx++, i++) {
                    if (pdata->type != SPI_WRITE_DATA) {
                        printf("%s: %d: SPI data is error[%u, %u], please check!!!!!!!! \n",
                               __FUNCTION__, __LINE__, i, data_idx);
                        return;
                    }
                    else {
                        lcd_spi_write_data(pdata->value, 8);
                        pdata++;
                    }
                }
            }
            else {
                pdata++;
            }
        }
        else if (pdata->type == SPI_WRITE_DELAY) {
            mdelay(pdata->value);
            pdata++;
        }
        else {
            printf("%s: %d: SPI data is error[%u, %u], please check!!!!!!!! \n",
                   __FUNCTION__, __LINE__, i, pdata->type);
            return;
        }
    }

    return;
}

static void st7789v_panel_invalid(uint32_t start_x,
                                  uint32_t start_y,  uint32_t end_x, uint32_t end_y)
{
    lcd_spi_write_cmd(0x2A, 8);
    lcd_spi_write_data(((start_x >> 8) & 0xFF), 8);
    lcd_spi_write_data((start_x & 0xFF), 8);
    lcd_spi_write_data(((end_x >> 8) & 0xFF), 8);
    lcd_spi_write_data((end_x & 0xFF), 8);

    lcd_spi_write_cmd(0x2B, 8);
    lcd_spi_write_data(((start_y >> 8) & 0xFF), 8);
    lcd_spi_write_data((start_y & 0xFF), 8);
    lcd_spi_write_data(((end_y >> 8) & 0xFF), 8);
    lcd_spi_write_data((end_y & 0xFF), 8);

    lcd_spi_write_cmd(0x2C, 8);
}

static int match_id(unsigned id, unsigned *id_table)
{
    int found = 0;
    unsigned *p = id_table;

    while (*p) {
        if (id == *p) {
            found = 1;
            break;
        }
        p++;
    }

    return (found == 1) ? 1 : 0;
}

static int st7789v_spi_read_id(unsigned *id)
{
    return lcd_spi_read_data(0x04, 8, (uint32_t *)id, 24, 0);
}

__attribute__ ((unused)) static int panel_detect_readid(void)
{
    unsigned id;
    if (st7789v_spi_read_id(&id) < 0) {
        printf("panel read id failed\n");
        goto err;
    }

    if (match_id(id, st7789v_id_table) == 0) {
        printf("unknown panel id = 0x%x\n", id);
        goto err;
    }

    printf("Found LCD panel ST7789V-SPI, id: 0x%x\n", id);
    printf("panel detect by readid ok\n");

    return 0;
err:
    printf("panel detect by readid failed\n");
    return -1;
}

static uint32_t rgb666_2_spi(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t val;
    val = ((b & 0x3F) << 2) | ((g & 0x3F) << 10) | ((r & 0x3F) << 18);
    return val;
}

__attribute__ ((unused)) static int panel_detect_fb(void)
{
    int res = 0;
    uint32_t val = 0, val1 = 0;

    lcd_spi_write_cmd(0x3A, 8);
    lcd_spi_write_data(0x66, 8); // to rgb666 mode

    st7789v_panel_invalid(0, 0, 1, 1);

    val = rgb666_2_spi(5, 2, 3);
    lcd_spi_write_data(val, 24);

    lcd_spi_read_data(0x2E, 8, (uint32_t *)&val1, 32, 1);
    if(val == val1) {
        printf(" panel_detect_fb success!!!!\n");
        res = 0;
    } else {
        printf(" panel_detect_fb failed[0x%x, 0x%x]!!!!\n", val, val1);
        res = -1;
    }

    lcd_spi_write_cmd(0x3A, 8);
    lcd_spi_write_data(0x05, 8); // to rgb565 mode

    return res;
}

__attribute__ ((unused)) static int panel_detect_te(void)
{
    lv_color_t color_p[2*2];
    uint32_t width = 2, height = 2;

    /* panel te is enabled in init cmd */
    g_lcd_ctx.layer_config_osd1.buf1 = (void *)color_p;
    g_lcd_ctx.layer_config_osd1.format = LCD_FORMAT_RGB565;
    g_lcd_ctx.layer_config_osd1.layer_enable = 1;
    g_lcd_ctx.layer_config_osd1.stride = width*2;
    g_lcd_ctx.layer_config_osd1.x = 0;
    g_lcd_ctx.layer_config_osd1.y = 0;
    g_lcd_ctx.layer_config_osd1.width = width;
    g_lcd_ctx.layer_config_osd1.height = height;
    g_lcd_ctx.layer_config_osd1.alpha = 0xff;

    g_lcd_ctx.wb_config.wb_enable = 0;
    g_lcd_ctx.layer_config_img.layer_enable = 0;

    lcdc_update_output_setting(&g_lcd_ctx.output_config, width, height);
    st7789v_panel_invalid(0, 0, 1, 1);

    lcd_spi_before_refresh();
    if (hal_lcdc_sync(&g_lcd_ctx, 1) < 0) { /* polling mode */
        printf(" panel detect by te failed\n");
        return -1;
    }

    printf(" panel detect by te ok\n");
    return 0;
}

typedef int (*panel_detect_fn_t)(void);

static panel_detect_fn_t detect_fn[] = {
    panel_detect_readid,
    panel_detect_te,
    panel_detect_fb,
};

static int panel_detect(void)
{
    int cnt = ARRAY_SIZE(detect_fn);
    int i, ret;

    for (i = 0; i < cnt; i++) {
        ret = detect_fn[i]();
        if (ret == 0) {
            return 0;
        }
    }

    return -1;
}

static void st7789v_spi_onoff(int on)
{
    uint32_t length;

    if (on) {
        length = sizeof(spi_display_on_cmds) / sizeof(struct spi_write_t);
        spi_write_function(spi_display_on_cmds, length);
    }
    else {   /* off */
        length = sizeof(spi_display_off_cmds) / sizeof(struct spi_write_t);
        spi_write_function(spi_display_off_cmds, length);
    }
}

__attribute__ ((unused)) static int st7789v_spi_probe_evb(unsigned lcd_clk)
{
    static unsigned inited = 0;
    unsigned id;

    lcd_spi_init(lcd_clk, &lcd_st7789v_spi_info_evb);

    if(inited) {
        return 0;
    }

  
   	hal_lcdc_reset_panel();


    if (st7789v_spi_read_id(&id) < 0) {
        printf("panel read id failed\n");
        return -1;
    }

    if (match_id(id, st7789v_id_table) == 0) {
        printf("unknown panel id = 0x%x\n", id);
        return -1;
    }

    printf("Found LCD panel ST7789V-SPI, id: 0x%x\n", id);

    uint32_t length = sizeof(spi_init_evb) / sizeof(struct spi_write_t);
    spi_write_function(spi_init_evb, length);

    memset(&g_lcd_ctx, 0, sizeof(lcd_context));
    g_lcd_ctx.output_config.data_lane_num = lcd_st7789v_spi_info_evb.data_lane_num;
    g_lcd_ctx.bg_color = 0x0000ff; /*bg color : red*/
    g_lcd_ctx.alpha_mode = LCD_ALPHA_MODE_NORMAL;
    g_lcd_ctx.output_config.format = SPI_FORMAT_RGB565;
    inited = 1;

    return 0;
}

__attribute__ ((unused)) static int st7789v_spi_probe_watch(unsigned lcd_clk)
{
    static unsigned inited = 0;
    lv_pm_info_t pm_info;

    lcd_spi_init(lcd_clk, &lcd_st7789v_spi_info_watch);
    if(inited) {
        return 0;
    }
	
    hal_lcdc_reset_panel();
	
    uint32_t length = sizeof(spi_init_watch) / sizeof(struct spi_write_t);
    spi_write_function(spi_init_watch, length);

    if (panel_detect() < 0) {
        return -1;
    }

    memset(&g_lcd_ctx, 0, sizeof(lcd_context));
    g_lcd_ctx.output_config.data_lane_num = lcd_st7789v_spi_info_watch.data_lane_num;
    g_lcd_ctx.bg_color = 0x0000ff; /*bg color : red*/
    g_lcd_ctx.alpha_mode = LCD_ALPHA_MODE_NORMAL;
    g_lcd_ctx.output_config.format = SPI_FORMAT_RGB565;
    pm_info.suspend = st7789v_suspend;
    pm_info.resume = st7789v_resume;
    pm_info.data = &st7789v_spi;
    pm_info.part = LV_PM_PART_DISPLAY;
    lv_pm_register(&pm_info);
    inited = 1;

    return 0;
}

/*refer to lv_port_disp_template.c, when lcdc_irq come, set lv_disp_flush_ready()*/
static void st7789v_spi_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lcd_disp_drv = disp_drv;

    if(suspend_flag == 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    uint32_t width, height, stride;
    unsigned buf_len;

    width = area->x2 - area->x1 + 1;
    height = area->y2 - area->y1 + 1;

#if (LV_COLOR_DEPTH == 16)
    stride = width * LCD_COLOR_DEPTH_BYTE;
#else
    printf("lv config is not 16 bits color depth !!!!!!!!!!!\n");
    return;
#endif
    buf_len = stride * height;
    flush_cache((unsigned long)color_p, buf_len);
    g_lcd_ctx.layer_config_osd1.buf1 = (void *)color_p;
    g_lcd_ctx.layer_config_osd1.format = LCD_FORMAT_RGB565;
    g_lcd_ctx.layer_config_osd1.layer_enable = 1;
    g_lcd_ctx.layer_config_osd1.stride = stride;
    g_lcd_ctx.layer_config_osd1.x = 0;
    g_lcd_ctx.layer_config_osd1.y = 0;
    g_lcd_ctx.layer_config_osd1.width = width;
    g_lcd_ctx.layer_config_osd1.height = height;
    g_lcd_ctx.layer_config_osd1.alpha = 0xff;

    g_lcd_ctx.wb_config.wb_enable = 0;
    g_lcd_ctx.layer_config_img.layer_enable = 0;

    lcdc_update_output_setting(&g_lcd_ctx.output_config, width, height);
    st7789v_panel_invalid(area->x1, area->y1, area->x2, area->y2);

    lcd_spi_before_refresh();
    hal_lcdc_sync(&g_lcd_ctx, 0); /* irq mode */
}

struct crane_panel_t st7789v_spi = {
#if USE_LCD_PANEL_ST7789V_SPI_WATCH
    .probe = st7789v_spi_probe_watch, /* lcd_init() in board_evb.c also need change */
#else
    .probe = st7789v_spi_probe_evb,
#endif
    .readid = st7789v_spi_read_id,
    .panel_onoff = st7789v_spi_onoff,
    .flush = st7789v_spi_flush,
};

#endif
