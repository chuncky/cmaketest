
#include "hal_conf.h"
#include "hal_drv_conf.h"
#include "hal_disp.h"
#include "st7796s_mipi.h"


#ifdef USE_LCD_PANEL_ST7796S_MIPI
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "halsoc_lcdc.h"
#include "halsoc_lcd_mipi.h"
#include "halsoc_lcd_mipi_dsi.h"
#include "bsp_common.h"

#ifndef UNLOCK_DELAY
#define UNLOCK_DELAY 0
#endif

#define LCD_COLOR_DEPTH_BYTE (LV_COLOR_DEPTH / 8)
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))


static lv_disp_drv_t * lcd_disp_drv = NULL;
static lcd_context g_lcd_ctx;

#define ST7796S_ID 0x7796

#define ID_TABLE_END 0
unsigned st7796s_mipi_id_table[] = {
    0x7796,
    ID_TABLE_END,
};




static struct s_dsi_cmd_desc st7796s_init_cmds[] = {
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xF0, 0xC3}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xF0, 0x96}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x36, 0x40}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 9,{0xe8, 0x40, 0x82, 0x07, 0x18, 0x27, 0x0a, 0xb6, 0x33}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xC2, 0xa7}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xC5, 0x25}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xb7, 0x46}},
    {DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 5,{0xb5, 0x00,0x00,0x00,0x00}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 15,{0xe0, 0xf0, 0x07, 0x011, 0x10, 0x11, 0x0b, 0x3c, 0x43, 0x4b, 0x0b, 0x17, 0x16, 0x1b, 0x1d}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 15,{0xe1, 0xf0, 0x07, 0x11, 0x0f, 0x10, 0x1a, 0x39, 0x55, 0x4a, 0x0b, 0x16, 0x15, 0x1a, 0x1d}},
    {DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x35, 0x00}}, //TE
    //{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 5,{0x2a, 0x00,0x00,0x}}, //xstart xend
    //{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 5,{0x2b, 0x00,0x00,0x,0x}}, //ystart yend

	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xf0, 0x3c}},
	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0xf0, 0x69}},
    {DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x3A, 0x77}}, //RGB888  
//	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x3A, 0x66}}, //RGB666
//	{DSI_DCS_LWRITE,DSI_LP_MODE,UNLOCK_DELAY, 2,{0x3A, 0x55}}, //RGB565

//	{DSI_DCS_LWRITE,DSI_LP_MODE,120, 2,{0x11, 0x00}},
//	{DSI_DCS_LWRITE,DSI_LP_MODE,20, 2,{0x29, 0x00}},
};

static struct s_dsi_cmd_desc st7796s_set_cmds[] = {
	{DSI_SET_MAX_PKT_SIZE, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x5}},
};

static struct s_dsi_cmd_desc st7796s_read_id_cmds[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0xD3}},
};


static struct s_dsi_cmd_desc st7796s_read_rddst[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x09}},
};

static struct s_dsi_cmd_desc st7796s_read_rddpm[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x0a}},
};
static struct s_dsi_cmd_desc st7796s_read_dmdctl[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x0b}},
};
static struct s_dsi_cmd_desc st7796s_read_dcolmod[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x0c}},
};
static struct s_dsi_cmd_desc st7796s_read_dim[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x0d}},
};

static struct s_dsi_cmd_desc st7796s_read_sdm[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x0e}},
};

static struct s_dsi_cmd_desc st7796s_read_dsdr[] = {
	{DSI_GENERIC_READ1, DSI_LP_MODE, UNLOCK_DELAY, 1, {0x0f}},
};
static struct s_dsi_cmd_desc st7796s_display_on_cmds[] = {
	{DSI_DCS_SWRITE,DSI_LP_MODE,120,1,{0x11}},
	{DSI_DCS_SWRITE,DSI_LP_MODE,20,1,{0x29}},
};

static struct s_dsi_cmd_desc st7796s_display_off_cmds[] = {
	{DSI_DCS_SWRITE,DSI_LP_MODE,20,1,{0x28}},
	{DSI_DCS_SWRITE,DSI_LP_MODE,120,1,{0x10}},
};





const static struct s_mipi_info lcd_st7796s_mipi_info = {
	320, /*width*/
	480, /*height*/
	38, /*hfp*/
	50, /*hbp*/
	50, /*hsync*/
	4, /*vfp*/
	4, /*vbp*/
	8, /*vsync*/
	60, /*fps*/

	DSI_MODE_CMD, /*work_mode*/
	DSI_INPUT_DATA_RGB_MODE_888, /*rgb_mode*/
	1, /*lane_number*/
	400000, /*phy_freq: KHz*/
	0, /*split_enable*/
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

static void st7796s_mipi_suspend(void *arg)
{
    struct crane_panel_t *p = arg;
    printf("st7796s_mipi_suspend\n");

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

static void st7796s_mipi_resume(void *arg)
{
    struct crane_panel_t *p = arg;
    printf("st7796s_mipi_resume\n");
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

static void st7796s_mipi_panel_invalid(uint32_t start_x,
                                  uint32_t start_y,  uint32_t end_x, uint32_t end_y)
{
    struct s_dsi_cmd_desc invalid_cmds[2];
    //start_x += 12;
    //end_x += 12;

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


static int st7796s_mipi_read_id(unsigned *pid)
{
    struct s_dsi_rx_buf dbuf;
    int id[5];

    lcd_dsi_write_cmds(st7796s_set_cmds, ARR_SIZE(st7796s_set_cmds));

    lcd_dsi_read_cmds(&dbuf, st7796s_read_id_cmds,ARR_SIZE(st7796s_read_id_cmds));
    id[0] = dbuf.data[0];
    id[1] = dbuf.data[1];
    id[2] = dbuf.data[2];
    id[3] = dbuf.data[3];
    id[4] = dbuf.data[4];
    printf("st7796s_mipi_read_id: data 0 - 4 is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n",
         id[0],id[1],id[2],id[3],id[4]);
	*pid = id[1] << 8 | id[2];

    return 0;
}

__attribute__ ((unused)) static int panel_detect_readid(void)
{
    unsigned id;
    if (st7796s_mipi_read_id(&id) < 0) {
        printf("panel read id failed\n");
        goto err;
    }

    if (match_id(id, st7796s_mipi_id_table) == 0) {
        printf("unknown panel id = 0x%x\n", id);
        goto err;
    }

    printf("Found LCD panel st7796s-mipi, id: 0x%x\n", id);
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

static void st7796s_mipi_onoff(int on)
{
    if (on) {
      lcd_dsi_write_cmds(st7796s_display_on_cmds, ARR_SIZE(st7796s_display_on_cmds));
    } else {   /* off */
      lcd_dsi_write_cmds(st7796s_display_off_cmds, ARR_SIZE(st7796s_display_off_cmds));
    }
}
extern int lcd_dsi_before_refresh(void);
extern void lcd_disable_wb(void);
extern void lcd_disable_img(void);
extern void lcd_config_osd1(lcd_layer_config *layer_config);

static int st7796s_mipi_probe(unsigned lcd_clk)
{
    static int inited = 0;
    lv_pm_info_t pm_info;
    uint32_t width, height,stride;
    struct s_dsi_rx_buf dbuf;
    lcd_context *lcd_ctx;
    lcd_mipi_init(lcd_clk, 0, &lcd_st7796s_mipi_info);
    lcd_mipi_reset();

    if(inited) {
        return 0;
    }
	hal_lcdc_reset_panel();
    if (panel_detect() < 0) {
        return -1;
    }

    lcd_dsi_write_cmds(st7796s_init_cmds, ARR_SIZE(st7796s_init_cmds));


    lcd_dsi_read_cmds(&dbuf, st7796s_read_rddst,ARR_SIZE(st7796s_read_rddst));

    lcd_dsi_read_cmds(&dbuf, st7796s_read_rddpm,ARR_SIZE(st7796s_read_rddpm));

    lcd_dsi_read_cmds(&dbuf, st7796s_read_dmdctl,ARR_SIZE(st7796s_read_dmdctl));





    
    width=320;
    height=480;
#if (LV_COLOR_DEPTH == 16)
    stride = width * LCD_COLOR_DEPTH_BYTE;
#else
    printf("lv config is not 16 bits color depth !!!!!!!!!!!\n");
    return;
#endif
    g_lcd_ctx.layer_config_osd1.stride = stride;
    memset(&g_lcd_ctx, 0, sizeof(lcd_context));
    g_lcd_ctx.bg_color = 0x0000ff; /*bg color : red*/
    g_lcd_ctx.alpha_mode = LCD_ALPHA_MODE_NORMAL;
    g_lcd_ctx.output_config.format = MIPI_FORMAT_CMD;
    g_lcd_ctx.layer_config_osd1.x = 0;
    g_lcd_ctx.layer_config_osd1.y = 0;
    g_lcd_ctx.layer_config_osd1.width = width;
    g_lcd_ctx.layer_config_osd1.height = height;
    g_lcd_ctx.layer_config_osd1.alpha = 0xff;
    g_lcd_ctx.wb_config.wb_enable = 0;
    g_lcd_ctx.layer_config_img.layer_enable = 0;
    g_lcd_ctx.layer_config_osd1.format = LCD_FORMAT_RGB565;
    g_lcd_ctx.layer_config_osd1.layer_enable = 1;

    lcdc_update_output_setting(&g_lcd_ctx.output_config, width, height);
    st7796s_mipi_panel_invalid(0,0,(width-1),(height-1));



    //pm_info.suspend = st7796s_mipi_suspend;
    //pm_info.resume = st7796s_mipi_resume;
    //pm_info.data = &st7796s_mipi;
    //pm_info.part = LV_PM_PART_DISPLAY;
    //lv_pm_register(&pm_info);
    lcd_disable_wb();
    lcd_disable_img();
    lcd_dsi_before_refresh();
    st7796s_mipi_onoff(1);
    lcd_ctx=&g_lcd_ctx;
    //lcd_config_osd1(&(lcd_ctx->layer_config_osd1));
    lcd_config_output(&(lcd_ctx->output_config), lcd_ctx->wb_config.wb_enable);
    inited = 1;

    return 0;
}


#if 0
static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_dsi_ctx* dsi_ctx = NULL;
    struct s_dsi_rx_buf dbuf;
    uint32_t read_id = 0;
	int id[5];

	LCDLOGD("st7796s +++");

	if(NULL == self){
		LCDLOGE("Invalid param");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("dsi has not been inited!");
		return 0;
	}

	dsi_ctx = (struct s_dsi_ctx*)self->panel_if;
	dsi_write_cmds(dsi_ctx, st7796s_set_cmds, ARRAY_SIZE(st7796s_set_cmds));
	LCDLOGD("lcd_panel_readid step 1");

	dsi_read_cmds(dsi_ctx, &dbuf, st7796s_read_id_cmds,ARRAY_SIZE(st7796s_read_id_cmds));
	id[0] = dbuf.data[0];
	id[1] = dbuf.data[1];
	id[2] = dbuf.data[2];
	id[3] = dbuf.data[3];
	id[4] = dbuf.data[4];
	
	
	LCDLOGD("read data 0 - 4 is 0x%x, 0x%x, 0x%x, 0x%x, 0x%x",
		id[0],id[1],id[2],id[3],id[4]);

	read_id = dbuf.data[1] << 8 | dbuf.data[2];
	if(read_id != ST7796S_ID)
		LCDLOGE("read panel id fail: read value = 0x%x", read_id);
	else
		LCDLOGE("read panel id OK: read value = 0x%x", read_id);

	LCDLOGD("st7796s ---");
    return read_id;
}
#endif

/*refer to lv_port_disp_template.c, when lcdc_irq come, set lv_disp_flush_ready()*/
static void st7796s_mipi_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
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
    g_lcd_ctx.layer_config_osd1.layer_enable = 1;
    g_lcd_ctx.layer_config_osd1.stride = stride;

    //lcd_dsi_before_refresh();
    hal_lcdc_sync(&g_lcd_ctx, 1); /* polling mode */
}



static int lcd_panel_checkesd(struct panel_spec *self)
{
#if 0
	struct s_dsi_ctx* dsi_ctx = NULL;
	uint32_t power_status = 0;

	LCDLOGD("st7796s +++");

	if(NULL == self){
		LCDLOGE("Invalid param");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("dsi has not been inited!");
		return 0;
	}

	dsi_ctx = (struct s_dsi_ctx*)self->panel_if;

	spi_read_data(spi_ctx, 0x0A, 8, &power_status, 8);
	LCDLOGD("read 0x%X receive 0x%x!",0xA, power_status);
	LCDLOGD("st7789v ---");
	if(power_status != 0x9C){
		LCDLOGE("fail, read 0x%X, expect 0x9C, receive 0x%x!",0xA, power_status);
		return -1;
	} else {
		return 0;
	}
#else
	return 0;
#endif
}


struct crane_panel_t st7796s_mipi = {

    .probe = st7796s_mipi_probe,
    .readid = st7796s_mipi_read_id,
    .panel_onoff = st7796s_mipi_onoff,
    .flush = st7796s_mipi_flush,
};


#endif /*LCD_ST7796S_MIPI*/
