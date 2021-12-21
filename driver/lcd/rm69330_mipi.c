#include "rm69330_mipi.h"

#if USE_LCD_PANEL_RM69330_MIPI

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "lvgl/src/lv_core/lv_refr.h"
#include "lvgl/src/lv_core/lv_disp.h"
#include "lvgl/src/lv_hal/lv_hal_tick.h"
#include "crane_lcdc.h"
#include "crane_lcd_mipi.h"
#include "crane_lcd_mipi_dsi.h"
#include "../common/utils.h"
#include "../common/lv_pm.h"
#include "../../board.h"
#include "lv_conf.h"

#define UNLOCK_DELAY         0
#define LCD_COLOR_DEPTH_BYTE (LV_COLOR_DEPTH / 8)
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))

static lv_disp_drv_t * lcd_disp_drv = NULL;

static lcd_context g_lcd_ctx;

#define ID_TABLE_END 0
unsigned rm69330_mipi_id_table[] = {
    0xF1,
    ID_TABLE_END,
};

static struct s_dsi_cmd_desc rm69330_init_cmds[] = {
    {DSI_DCS_SWRITE1,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xFE, 0x00}},
    {DSI_DCS_SWRITE1,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x35, 0x00}},
    {DSI_DCS_SWRITE1,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x3A, 0x77}},     //RGB888
    {DSI_DCS_SWRITE1,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x51, 0xFF}},
    {DSI_DCS_SWRITE1,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x51, 0xFF}},
    {DSI_DCS_SWRITE,DSI_LP_MODE,120, 1,{0x11}},
    {DSI_DCS_SWRITE,DSI_LP_MODE,20, 1,{0x29}},
};

static struct s_dsi_cmd_desc rm69330_set_cmds[] = {
    {DSI_SET_MAX_PKT_SIZE, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x2}},
};

static struct s_dsi_cmd_desc rm69330_read_id_cmds[] = {
    {DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0xDB}},
};

static struct s_dsi_cmd_desc rm69330_display_on_cmds[] = {
    {DSI_DCS_SWRITE,DSI_LP_MODE,120,1,{0x11}},
    {DSI_DCS_SWRITE,DSI_LP_MODE,20,1,{0x29}},
};

static struct s_dsi_cmd_desc rm69330_display_off_cmds[] = {
    {DSI_DCS_SWRITE,DSI_LP_MODE,20,1,{0x28}},
    {DSI_DCS_SWRITE,DSI_LP_MODE,120,1,{0x10}},
};

const static struct s_mipi_info lcd_rm69330_mipi_info = {
    454, /*width*/
    454, /*height*/
    20, /*hfp*/
    40, /*hbp*/
    10, /*hsync*/
    20, /*vfp*/
    12, /*vbp*/
    10, /*vsync*/
    60, /*fps*/

    DSI_MODE_CMD, /*work_mode*/ /*can only be cmd mode*/
    DSI_INPUT_DATA_RGB_MODE_888, /*rgb_mode*/
    1, /*lane_number*/ /*can only be 1*/
    400000, /*phy_freq: KHz*/
    0, /*split_enable*/ /*can only be 0*/
    1, /*eotp_enable*/

    DSI_BURST_MODE_BURST, /*burst_mode*/

    /*following force to 0*/
    0, /*te_enable*/
    0, /*vsync_pol*/
    0, /*te_pol*/
    0, /*te_mode*/

    /*The following fields need not be set by panel*/
    0,
};

static uint8_t suspend_flag = 0;
static void rm69330_mipi_suspend(void *arg)
{
    struct crane_panel_t *p = arg;
    printf("rm69330_mipi_suspend\n");

    /*since disp_refr can be changed by other register,
     *so if st7789v suspend need use lcd_disp_drv instead of lv_refr_vdb_is_flushing()
     *refer to lv_disp_refr_task() and lv_refr_vdb_flush()*/
    if(lcd_disp_drv && lcd_disp_drv->buffer) {
        // wait flushing finish~~
        while(lcd_disp_drv->buffer->flushing)
            ;
    }

    p->panel_onoff(0);
    lcd_mipi_uninit();
    suspend_flag = 1;
}

static void rm69330_mipi_resume(void *arg)
{
    struct crane_panel_t *p = arg;
    printf("rm69330_mipi_resume\n");
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

static void rm69330_mipi_panel_invalid(uint32_t start_x,
                                  uint32_t start_y,  uint32_t end_x, uint32_t end_y)
{
    struct s_dsi_cmd_desc invalid_cmds[2];
    start_x += 12;
    end_x += 12;

    lcd_dsi_update_mipi_info(end_x-start_x + 1, end_y-start_y + 1);

    invalid_cmds[0].cmd_type = DSI_DCS_LWRITE;
    invalid_cmds[0].lp = DSI_HS_MODE;
    invalid_cmds[0].delay = UNLOCK_DELAY;
    invalid_cmds[0].length = 5;
    invalid_cmds[0].data[0] = 0x2A;
    invalid_cmds[0].data[1] = (start_x >> 8) & 0xFF;
    invalid_cmds[0].data[2] = (start_x) & 0xFF;
    invalid_cmds[0].data[3] = (end_x >> 8) & 0xFF;
    invalid_cmds[0].data[4] = (end_x) & 0xFF;

    invalid_cmds[1].cmd_type = DSI_DCS_LWRITE;
    invalid_cmds[1].lp = DSI_HS_MODE;
    invalid_cmds[1].delay = UNLOCK_DELAY;
    invalid_cmds[1].length = 5;
    invalid_cmds[1].data[0] = 0x2B;
    invalid_cmds[1].data[1] = (start_y >> 8) & 0xFF;
    invalid_cmds[1].data[2] = (start_y) & 0xFF;
    invalid_cmds[1].data[3] = (end_y >> 8) & 0xFF;
    invalid_cmds[1].data[4] = (end_y) & 0xFF;

    lcd_dsi_write_cmds(invalid_cmds, ARR_SIZE(invalid_cmds));
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

static int rm69330_mipi_read_id(unsigned *pid)
{
    struct s_dsi_rx_buf dbuf;
    int id[5];

    lcd_dsi_write_cmds(rm69330_set_cmds, ARR_SIZE(rm69330_set_cmds));

    lcd_dsi_read_cmds(&dbuf, rm69330_read_id_cmds,ARR_SIZE(rm69330_read_id_cmds));
    id[0] = dbuf.data[0];
    id[1] = dbuf.data[1];
    id[2] = dbuf.data[2];
    id[3] = dbuf.data[3];
    id[4] = dbuf.data[4];
    printf("rm69330_mipi_read_id: data 0 - 4 is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n",
         id[0],id[1],id[2],id[3],id[4]);

    *pid = id[0];
    return 0;
}

__attribute__ ((unused)) static int panel_detect_readid(void)
{
    unsigned id;
    if (rm69330_mipi_read_id(&id) < 0) {
        printf("panel read id failed\n");
        goto err;
    }

    if (match_id(id, rm69330_mipi_id_table) == 0) {
        printf("unknown panel id = 0x%x\n", id);
        goto err;
    }

    printf("Found LCD panel rm69330-mipi, id: 0x%x\n", id);
    printf("panel detect by readid ok\n");

    return 0;
err:
    printf("panel detect by readid failed\n");
    return -1;
}

typedef int (*panel_detect_fn_t)(void);

static panel_detect_fn_t detect_fn[] = {
    panel_detect_readid,
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

static void rm69330_mipi_onoff(int on)
{
    if (on) {
      lcd_dsi_write_cmds(rm69330_display_on_cmds, ARR_SIZE(rm69330_display_on_cmds));
    } else {   /* off */
      lcd_dsi_write_cmds(rm69330_display_off_cmds, ARR_SIZE(rm69330_display_off_cmds));
    }
}

static int rm69330_mipi_probe(unsigned lcd_clk)
{
    static int inited = 0;
    lv_pm_info_t pm_info;

    lcd_mipi_init(lcd_clk, 0, &lcd_rm69330_mipi_info);
    lcd_mipi_reset();

    if(inited) {
        return 0;
    }
    
    hal_lcdc_reset_panel();

    if (panel_detect() < 0) {
        return -1;
    }

    lcd_dsi_write_cmds(rm69330_init_cmds, ARR_SIZE(rm69330_init_cmds));

    memset(&g_lcd_ctx, 0, sizeof(lcd_context));
    g_lcd_ctx.bg_color = 0x0000ff; /*bg color : red*/
    g_lcd_ctx.alpha_mode = LCD_ALPHA_MODE_NORMAL;
    g_lcd_ctx.output_config.format = MIPI_FORMAT_CMD;

    pm_info.suspend = rm69330_mipi_suspend;
    pm_info.resume = rm69330_mipi_resume;
    pm_info.data = &rm69330_mipi;
    pm_info.part = LV_PM_PART_DISPLAY;
    lv_pm_register(&pm_info);
    inited = 1;

    return 0;
}

/*refer to lv_port_disp_template.c, when lcdc_irq come, set lv_disp_flush_ready()*/
static void rm69330_mipi_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
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
    rm69330_mipi_panel_invalid(area->x1, area->y1, area->x2, area->y2);

    lcd_dsi_before_refresh();
    lcdc_sync(&g_lcd_ctx, 0); /* irq mode */
}

struct crane_panel_t rm69330_mipi = {
    .probe = rm69330_mipi_probe, /* lcd_init() in board_evb.c also need change */
    .readid = rm69330_mipi_read_id,
    .panel_onoff = rm69330_mipi_onoff,
    .flush = rm69330_mipi_flush,
};

#endif
