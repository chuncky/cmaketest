#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "hal_conf.h"
#include "bsp_common.h"
#include "hal_disp.h"
#include "hal_board.h"
#include "intc.h"
/* SDK api, not exported in head file in current SDK */
INTC_InterruptSources INTCGetIntVirtualNum(UINT32 IRQ_NO);



/**
 * Add a log
 * @param level the level of log. (From `lv_log_level_t` enum)
 * @param file name of the file when the log added
 * @param line line number in the source code where the log added
 * @param func name of the function when the log added
 * @param format printf-like format string
 * @param ... parameters for `format`
 */
void _lv_log_add(lv_log_level_t level, const char * file, int line, const char * func, const char * format, ...)
{
    if(level >= _LV_LOG_LEVEL_NUM) return; /*Invalid level*/

    if(level >= LV_LOG_LEVEL) {
        va_list args;
        va_start(args, format);
        char buf[256];
        vsnprintf(buf, sizeof(buf), format, args);
        va_end(args);

#if LV_LOG_PRINTF
        /*Use only the file name not the path*/
        size_t p;
        for(p = strlen(file); p > 0; p--) {
            if(file[p] == '/' || file[p] == '\\') {
                p++;    /*Skip the slash*/
                break;
            }
        }

        static const char * lvl_prefix[] = {"Trace", "Info", "Warn", "Error", "User"};
        printf("%s: %s \t(%s #%d %s())\n", lvl_prefix[level], buf, &file[p], line, func);
#else
        if(custom_print_cb) custom_print_cb(level, file, line, func, buf);
#endif
    }
}

struct media_clock_ctx {
    char media_clk_init;
    uint16_t clk_times[MODULE_MAX][SUB_MODULE_MAX];
    uint16_t total_clk_times[SUB_MODULE_MAX];
};


static struct media_clock_ctx g_media_clk_ctx;

static unsigned int media_clock_reg_read(unsigned int reg)
{
    return pmu_read(reg);
}

static void media_clock_reg_write(unsigned int reg, unsigned int val)
{
    pmu_write(reg, val);
}

static void hal_mediaclk_write_mask(unsigned int reg, unsigned int val, unsigned int mask)
{
    uint32_t v;

    v = media_clock_reg_read(reg);
    v = (v & ~mask) | (val & mask);
    media_clock_reg_write(reg, v);
}

void hal_mediaclk_setbit(uint32_t reg, uint32_t val)
{
    hal_mediaclk_write_mask(reg, val, val);
}

void hal_mediaclk_clrbit(uint32_t reg, uint32_t val)
{
    hal_mediaclk_write_mask(reg, 0, val);
}

static int media_clock_set_rate_isp2x(int clk_rate)
{
    int val = 0;

    if(clk_rate != 624 && clk_rate != 416 && clk_rate != 312 && clk_rate != 208)
    {
        printf("media_clock_set_rate_isp2x: error clk_rate (%d)\r\n", clk_rate);
        return -1;
    }
    //select isp2x src and div
    if(clk_rate == 624){
        val = 0;
    }
    else if(clk_rate == 312){
        val = BIT(19);
    }
    else if(clk_rate == 416){
        val = BIT(17);
    }
    else{ //208
        val = BIT(17)|BIT(19);
    }
    hal_mediaclk_write_mask(0x50, val, BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21));
    return 0;

}
static int media_clock_set_rate_offline(int clk_rate)
{
    int val = 0;

    if(clk_rate != 624 && clk_rate != 416 && clk_rate != 312 && clk_rate != 208)
    {
        printf("media_clock_set_rate_offline: error clk_rate (%d)\r\n", clk_rate);
        return -1;
    }
    //select isp2x src and div
    if(clk_rate == 416){
        val = 0;
    }
    if(clk_rate == 208){
        val = BIT(27);
    }
    else if(clk_rate == 624){
        val = BIT(25);
    }
    else{ // 312
        val = BIT(25)|BIT(27);
    }
    hal_mediaclk_write_mask(0x50, val, BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29));
    return 0;

}

static int media_clock_set_rate_phy(int clk_rate)
{
    if(clk_rate != 52 && clk_rate != 104)
    {
        printf("media_clock_set_rate_phy: error clk_rate (%d)\r\n", clk_rate);
        return -1;
    }
    if(clk_rate == 52){
        hal_mediaclk_setbit(0x50, BIT(7));
    }
    else //104
        hal_mediaclk_clrbit(0x50, BIT(7));
    return 0;

}
static int media_clock_set_lcd_ci(int clk_rate)
{
    int val;

    if(clk_rate != 312 && clk_rate != 208 && clk_rate != 156 && clk_rate != 350)
    {
        printf("media_clock_set_lcd_ci: error clk_rate (%d)\r\n", clk_rate);
        return -1;
    }
    if(clk_rate == 312){
        val = 0;
    }
    else if(clk_rate == 208){
        val = BIT(10);
    }else if(clk_rate == 156){
        val = BIT(11);
    }else{
        val =BIT(10)|BIT(11);
    }
    hal_mediaclk_write_mask(0x50, val, BIT(10)|BIT(11));
    return 0;
}

static int media_clock_set_lcd_sclk(int clk_rate)
{
    switch(clk_rate){
    case 312000:
        hal_mediaclk_clrbit(0x4C, BIT(6));
        hal_mediaclk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
        hal_mediaclk_clrbit(0x4C, BIT(9));
        hal_mediaclk_setbit(0x4C, BIT(8));
        writel(0x40000001, 0xd420A000 + 0x1A8);//enable sclk
        break;
    case 156000:
      hal_mediaclk_clrbit(0x4C, BIT(6));
      hal_mediaclk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
      hal_mediaclk_clrbit(0x4C, BIT(9));
      hal_mediaclk_setbit(0x4C, BIT(8));
      writel(0x40000002, 0xd420A000 + 0x1A8);//enable sclk
      break;
    case 125000:
        hal_mediaclk_clrbit(0x4C, BIT(6));
        hal_mediaclk_write_mask(0x4C, 0x2 << 10, 0x1F << 10);
        hal_mediaclk_setbit(0x4C, BIT(9));
        hal_mediaclk_clrbit(0x4C, BIT(8));
        writel(0x432D0001, 0xd420A000 + 0x1A8);//enable sclk
        break;
    case 120000:
      hal_mediaclk_clrbit(0x4C, BIT(6));
      hal_mediaclk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
      hal_mediaclk_clrbit(0x4C, BIT(9));
      hal_mediaclk_setbit(0x4C, BIT(8));
      writel(0x43B10002, 0xd420A000 + 0x1A8);//enable sclk
      break;
    case 104000:
#if 0
        media_clock_clr_bit(0x4C, BIT(6));
        media_clock_write_mask(0x4C, 0x3 << 10, 0x1F << 10);
        media_clock_set_bit(0x4C, BIT(9));
        media_clock_clr_bit(0x4C, BIT(8));
        writel(0x40000001, 0xd420A000 + 0x1A8);//enable sclk
#else /*work around for A0 timing issue*/
        hal_mediaclk_clrbit(0x4C, BIT(6));
        hal_mediaclk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
        hal_mediaclk_clrbit(0x4C, BIT(9));
        hal_mediaclk_setbit(0x4C, BIT(8));
        writel(0x40000003, 0xd420A000 + 0x1A8);//enable sclk
#endif
        break;
    case 62400:
        hal_mediaclk_clrbit(0x4C, BIT(6));
        hal_mediaclk_write_mask(0x4C, 0x5 << 10, 0x1F << 10);
        hal_mediaclk_setbit(0x4C, BIT(9));
        hal_mediaclk_clrbit(0x4C, BIT(8));
        writel(0x40000001, 0xd420A000 + 0x1A8);//enable sclk
        break;
    case 52000:
        hal_mediaclk_setbit(0x4C, BIT(6));
        hal_mediaclk_write_mask(0x4C, 0x0 << 10, 0x1F << 10);
        hal_mediaclk_clrbit(0x4C, BIT(9));
        hal_mediaclk_setbit(0x4C, BIT(8));
        writel(0x40000001, 0xd420A000 + 0x1A8);//enable sclk
        break;
    case 26000:
        hal_mediaclk_setbit(0x4C, BIT(6));
        hal_mediaclk_write_mask(0x4C, 0x2 << 10, 0x1F << 10);
        hal_mediaclk_setbit(0x4C, BIT(9));
        hal_mediaclk_clrbit(0x4C, BIT(8));
        writel(0x40000001, 0xd420A000 + 0x1A8);//enable sclk
        break;
    case 17300:
        hal_mediaclk_setbit(0x4C, BIT(6));//52M
        hal_mediaclk_write_mask(0x4C, 0x3 << 10, 0x1F << 10);
        hal_mediaclk_setbit(0x4C, BIT(9));
        hal_mediaclk_clrbit(0x4C, BIT(8));
        writel(0x40000001, 0xd420A000 + 0x1A8);//enable sclk
        break;
    default:
        printf("media_clock_set_lcd_sclk: unsupport clk_rate (%d)\n", clk_rate);
        return -1;
    }
    return 0;
}

static int media_clock_set_jpeg_func(int clk_rate)
{
    unsigned int val;

    switch (clk_rate) {
        case 156:
            val = BIT(13);
            break;
        case 208:
            val = BIT(12);
            break;
        case 312:
            val = 0;
            break;
        case 350:
            val = BIT(12) | BIT(13);
            break;
        default:
            printf("invalid jpeg func clk %d\r\n", clk_rate);
            return -1;
    }

    hal_mediaclk_write_mask(0x50, val, BIT(12)|BIT(13));
    return 0;
}

int hal_mediaclk_set_rate(int clk_type, int clk_rate)
{
    if(clk_type & MEDIA_CLK_ISP2X){
        media_clock_set_rate_isp2x(clk_rate);
    }
    if(clk_type & MEDIA_CLK_OFFLINE){
        media_clock_set_rate_offline(clk_rate);
    }
    if(clk_type & MEDIA_CLK_PHY){
        media_clock_set_rate_phy(clk_rate);
    }
    if(clk_type & MEDIA_CLK_LCD_CI){
        media_clock_set_lcd_ci(clk_rate);
    }
    if(clk_type & MEDIA_CLK_LCD_SCLK){
        media_clock_set_lcd_sclk(clk_rate);
    }
    if(clk_type & MEDIA_CLK_JPEG_FUNC){
        media_clock_set_jpeg_func(clk_rate);
    }
    return 0;
}


int hal_mediaclk_enable(int clk_type, int module_id)
{
    int error_id = 0;
    int total_clk_error = 0;

    if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
        printf("hal_mediaclk_enable: error module_id [%d]!\r\n", module_id);
        return 0;
    }

    if(clk_type & MEDIA_CLK_ISP2X){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X]++;
        }
        else{
            error_id |= MEDIA_CLK_ISP2X;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_ISP2X;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] == 1)
            hal_mediaclk_setbit(0x50, BIT(16)|BIT(23));
    }
    if(clk_type & MEDIA_CLK_OFFLINE){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE]++;
        }
        else{
            error_id |= MEDIA_CLK_OFFLINE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_OFFLINE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] == 1){
            hal_mediaclk_setbit(0x50, BIT(24)|BIT(31));
        }
    }
    if(clk_type & MEDIA_CLK_PHY){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY]++;
        }
        else{
            error_id |= MEDIA_CLK_PHY;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_PHY;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] == 1) {
            hal_mediaclk_setbit(0x50, BIT(2)|BIT(5));
        }
    }
    if(clk_type & MEDIA_CLK_INTERFACE){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE]++;
        }
        else{
            error_id |= MEDIA_CLK_INTERFACE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_INTERFACE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] == 1)
            hal_mediaclk_setbit(0x50, BIT(0)|BIT(3));
    }
    if(clk_type & MEDIA_CLK_OFFLINE_AXI){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI]++;
        }
        else{
            error_id |= MEDIA_CLK_OFFLINE_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_OFFLINE_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] == 1)
            hal_mediaclk_setbit(0x50, BIT(8)|BIT(9));
    }
    if(clk_type & MEDIA_CLK_LCD_CI){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI]++;
        }
        else{
            error_id |= MEDIA_CLK_LCD_CI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_CI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] == 1)
            hal_mediaclk_setbit(0x50, BIT(1)|BIT(4));
    }
    if(clk_type & MEDIA_CLK_LCD_AHB){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB]++;
        }
        else{
            error_id |= MEDIA_CLK_LCD_AHB;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_AHB;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB]  == 1)
            hal_mediaclk_setbit(0x4C, BIT(2)|BIT(5));
    }
    if(clk_type & MEDIA_CLK_LCD_SCLK){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK]++;
        }
        else{
            error_id |= MEDIA_CLK_LCD_SCLK;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_SCLK;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK]  == 1)
            hal_mediaclk_setbit(0x4C, BIT(4));
            writel(readl(0xd420a000 + 0x9c) & ~BIT(29), 0xd420a000 + 0x9c);
    }
    if(clk_type & MEDIA_CLK_LCD_AXI){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI]++;
        }
        else{
            error_id |= MEDIA_CLK_LCD_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI]  == 1)
            hal_mediaclk_setbit(0x4C, BIT(0)|BIT(3));
    }
    if(clk_type & MEDIA_CLK_JPEG_FUNC){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC] < 65535){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC]++;
        }
        else{
            error_id |= MEDIA_CLK_JPEG_FUNC;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC] < 65535){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC]++;
        }
        else{
            total_clk_error |= MEDIA_CLK_JPEG_FUNC;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC]  == 1)
            hal_mediaclk_setbit(0x50, BIT(14) | BIT(15));
    }

    if(error_id || total_clk_error){
        printf("hal_mediaclk_enable: error, please disable after enable![0x%x][0x%x]\r\n", error_id, total_clk_error);
        return -1;
    }
    return 0;
}

int hal_mediaclk_disable(int clk_type, int module_id)
{
    int error_id = 0;
    int total_clk_error = 0;

    if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
        printf("hal_mediaclk_disable: error module_id [%d]!\r\n", module_id);
        return 0;
    }
    if(clk_type & MEDIA_CLK_ISP2X){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_ISP2X]--;
        }
        else{
            error_id |= MEDIA_CLK_ISP2X;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_ISP2X;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_ISP2X] == 0)
            hal_mediaclk_clrbit(0x50, BIT(16)|BIT(23));
    }
    if(clk_type & MEDIA_CLK_OFFLINE){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE]--;
        }
        else{
            error_id |= MEDIA_CLK_OFFLINE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_OFFLINE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE] == 0)
            hal_mediaclk_clrbit(0x50, BIT(24)|BIT(31));
    }
    if(clk_type & MEDIA_CLK_PHY){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_PHY]--;
        }
        else{
            error_id |= MEDIA_CLK_PHY;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_PHY;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_PHY] == 0)
            hal_mediaclk_clrbit(0x50, BIT(2)|BIT(5));
    }
    if(clk_type & MEDIA_CLK_INTERFACE){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_INTERFACE]--;
        }
        else{
            error_id |= MEDIA_CLK_INTERFACE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_INTERFACE;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_INTERFACE] == 0)
            hal_mediaclk_clrbit(0x50, BIT(0)|BIT(3));
    }
    if(clk_type & MEDIA_CLK_OFFLINE_AXI){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_OFFLINE_AXI]--;
        }
        else{
            error_id |= MEDIA_CLK_OFFLINE_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_OFFLINE_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_OFFLINE_AXI] == 0)
            hal_mediaclk_clrbit(0x50, BIT(8)|BIT(9));
    }
    if(clk_type & MEDIA_CLK_LCD_CI){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_CI]--;
        }
        else{
            error_id |= MEDIA_CLK_LCD_CI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_CI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_CI] == 0)
            hal_mediaclk_clrbit(0x50, BIT(1)|BIT(4));
    }
    if(clk_type & MEDIA_CLK_LCD_AHB){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AHB]--;
        }
        else{
            error_id |= MEDIA_CLK_LCD_AHB;
        }
        //total_clk_times--
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_AHB;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AHB] == 0)
            hal_mediaclk_clrbit(0x4C, BIT(2)|BIT(5));
    }
    if(clk_type & MEDIA_CLK_LCD_SCLK){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_SCLK]--;
        }
        else{
            error_id |= MEDIA_CLK_LCD_SCLK;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_SCLK;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_SCLK] == 0)
            hal_mediaclk_clrbit(0x4C, BIT(4));
            writel(readl(0xd420a000 + 0x9c) | BIT(29), 0xd420a000 + 0x9c);
    }
    if(clk_type & MEDIA_CLK_LCD_AXI){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_LCD_AXI]--;
        }
        else{
            error_id |= MEDIA_CLK_LCD_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_LCD_AXI;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_LCD_AXI] == 0)
            hal_mediaclk_clrbit(0x4C, BIT(0)|BIT(3));
    }
    if(clk_type & MEDIA_CLK_JPEG_FUNC){
        if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC] > 0){
            g_media_clk_ctx.clk_times[module_id][SUB_MODULE_JPEG_FUNC]--;
        }
        else{
            error_id |= MEDIA_CLK_JPEG_FUNC;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC] > 0){
            g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC]--;
        }
        else{
            total_clk_error |= MEDIA_CLK_JPEG_FUNC;
        }
        if(g_media_clk_ctx.total_clk_times[SUB_MODULE_JPEG_FUNC] == 0)
            hal_mediaclk_clrbit(0x50, BIT(14) | BIT(15));
    }

    if(error_id || total_clk_error){
        printf("hal_mediaclk_disable: error, please enable before disable![%d][%d]\r\n", error_id,total_clk_error);
    }
    return 0;
}

int hal_mediaclk_poweron(int module_id)
{
    if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
        printf("hal_mediaclk_poweron: error module_id [%d]!\r\n", module_id);
        return -1;
    }

    if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER] < 65535)
        g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER]++;
    else{
        printf("hal_mediaclk_poweron: clk_times error[%d], please uninit after init!\r\n", (int)SUB_MODULE_POWER);
    }

    if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] < 65535)
        g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER]++;
    else{
        printf("hal_mediaclk_poweron: total_clk_times error[%d], please uninit after init!\n", (int)SUB_MODULE_POWER);
    }
    if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] == 1){
        //enblae Multimedia Power Switch
        hal_mediaclk_setbit(0x4c, BIT(17));
        mdelay(1);
        hal_mediaclk_setbit(0x4c, BIT(18));
        mdelay(1);
        hal_mediaclk_setbit(0x4c, BIT(16));
        //release Multimedia Power reset
        hal_mediaclk_setbit(0x4c, BIT(1));

        //set rate for all isp clock
        hal_mediaclk_set_rate(MEDIA_CLK_ISP2X, 312);
        hal_mediaclk_set_rate(MEDIA_CLK_PHY, 52);
        hal_mediaclk_set_rate(MEDIA_CLK_OFFLINE, 312);
        hal_mediaclk_set_rate(MEDIA_CLK_LCD_CI, 312);
        hal_mediaclk_set_rate(MEDIA_CLK_JPEG_FUNC, 312);

    }

    return 0;
}

int hal_mediaclk_poweroff(int module_id)
{
    if(module_id > MODULE_VIDEO || module_id < MODULE_CAMERA){
        printf("hal_mediaclk_poweroff: error module_id [%d]!\n", module_id);
        return -1;
    }


    if(g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER] > 0)
        g_media_clk_ctx.clk_times[module_id][SUB_MODULE_POWER]--;
    else{
        printf("hal_mediaclk_poweroff: clk_times error[%d], please init before uninit!\n", (int)SUB_MODULE_POWER);
    }
    if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] > 0)
        g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER]--;
    else{
        printf("hal_mediaclk_poweroff: total_clk_times error[%d], please init before uninit!\n", (int)SUB_MODULE_POWER);
    }
    if(g_media_clk_ctx.total_clk_times[SUB_MODULE_POWER] == 0){
        //disable Multimedia Power Switch
        hal_mediaclk_clrbit(0x4c, BIT(16));
        hal_mediaclk_clrbit(0x4c, BIT(18));
        hal_mediaclk_clrbit(0x4c, BIT(17));
        //enable Multimedia Power reset
        hal_mediaclk_clrbit(0x4c, BIT(1));
    }

    return 0;
}

int hal_mediaclk_dump(enum media_clk_dump_level dump_level)
{
    unsigned int val1, val2;
    val1 = media_clock_reg_read(0x4c);
    val2 = media_clock_reg_read(0x50);
    printf("hal_mediaclk_dump:0x4c:0x%x,0x50:0x%x\n", val1, val2);
    if(dump_level == LEVEL_DUMP_CLOCK_TIMES) {
        for(val1 = 0;val1 < MODULE_MAX; val1++) {
            for(val2 = 0;val2 < SUB_MODULE_MAX; val2++){
                printf("hal_mediaclk_dump: clk_times[%d][%d] = %d\n", val1, val2, g_media_clk_ctx.clk_times[val1][val2]);
            }
        }
        for(val1 = 0;val1 < MODULE_MAX; val1++) {
            printf("hal_mediaclk_dump: total_clk_times[%d] = %d\n", val1, g_media_clk_ctx.total_clk_times[val1]);
        }
    }
    return 0;
}


void Lcd_Logo_Mipi_Avdd12_On(int OnOff)
{
    if (PMIC_IS_PM813()||PMIC_IS_PM813S())
    {
		Ningbo_LDO_Set_VOUT(0x8F, (0x0 << 2));
        Ningbo_Ldo_11_set(OnOff);
    }
}

void lcd_ldo11_1v2_on(void)
{
	Lcd_Logo_Mipi_Avdd12_On(1);
}

void lcd_ldo11_1v2_off(void)
{
	Lcd_Logo_Mipi_Avdd12_On(0);
}




extern void Ningbo_INT_CALLBACK_REGISTER(NINGBO_INTC intc,PmicCallback isr);
extern void Ningbo_INT_ENABLE(NINGBO_INTC intc);
extern void InitTimers(void);
extern int uart_printf(const char* fmt, ...);

static alarm_cb_t alarm_cb = NULL;
#if 0
void timer_enable(void)
{
    InitTimers();
}

void timer_disable(void)
{
    timerDeActivate(TS_TIMER_ID);
}

void lv_udelay(unsigned us)
{
    unsigned t, period, end;

    t = timerCountRead(TS_TIMER_ID);
    period = (timerClockRateGet(TS_TIMER_ID) == FAST_TIMER_CLOCK_RATE_IN_HZ) ?
            MICRO_SEC_TO_CLOCK_TICKS(us):MICRO_SEC_TO_CLOCK_TICKS_CLK_32KHZ(us);
    end = t + period;
    while (t < end) {
        t = timerCountRead(TS_TIMER_ID);
    }
}
#endif
extern void mdelay(int ms);
#if 0
void mdelay(int ms)
{
    lv_udelay(ms * 1000);
}
#endif


int bat_remain = 100;
uint8_t pmic_get_bat_remain(void)
{
    return bat_remain;
}

#if 0
unsigned irq_disable(void)
{
    return disableInterrupts();
}

unsigned irq_enable(void)
{
    return enableInterrupts();
}

void irq_restore(unsigned state)
{
    restoreInterrupts(state);
}

int irq_is_in(void)
{
    return OS_Current_Interrupt_Count();
}
#endif
int mask_interrupt(unsigned int irq)
{
    INTC_InterruptSources vir = INTCGetIntVirtualNum(irq);
    return (int)INTCDisable(vir);
}

int unmask_interrupt(unsigned int irq)
{
    INTC_InterruptSources vir = INTCGetIntVirtualNum(irq);
    return (int)INTCEnable(vir);
}

void register_int_handler(unsigned int irq, int_handler handler, void *arg)
{
    INTC_InterruptSources vir = INTCGetIntVirtualNum(irq);
    INTCConfigure(vir, INTC_IRQ, INTC_HIGH_LEVEL);
    INTCBind(vir, (INTC_ISR)handler);
}

void interrupts_init(void)
{
   /*nothing to do for crane modem */
   return;
}

void arch_disable_cache(unsigned flags)
{
   printf("arch_disable_cache is not supported!!\n");
}

void arch_enable_cache(unsigned flags)
{
   printf("arch_enable_cache is not supported!!\n");
}

static void arch_address_align(unsigned long *start, unsigned long *len)
{
    unsigned long buf = *start;
    unsigned long size = *len;
    unsigned offset = buf & CACHE_LINE_MASK;

    if(offset != 0) {
        buf -= offset;
        size += offset;
    }

    offset = size & CACHE_LINE_MASK;
    if(offset != 0) {
        size += (SIZEOF_CACHE_LINE - offset);
    }

    *start = buf;
    *len = size;
}

void arch_clean_cache_range(unsigned start, unsigned len)
{
    unsigned long buf = start;
    unsigned long size = len;
    arch_address_align(&buf, &size);

    CacheCleanMemory((void *)buf, size);
}

void arch_clean_invalidate_cache_range(unsigned start, unsigned len)
{
    unsigned long buf = start;
    unsigned long size = len;
    arch_address_align(&buf, &size);

    CacheCleanAndInvalidateMemory((void *)buf, size);
}

void arch_invalidate_cache_range(unsigned start, unsigned len)
{
    unsigned long buf = start;
    unsigned long size = len;
    arch_address_align(&buf, &size);

    InvalidateDataCache((UINT32)buf, size);
}

void arch_sync_cache_range(unsigned start, unsigned len)
{
   printf("arch_sync_cache_range is dummy!!\n");
}

void flush_cache(unsigned long start, unsigned long len)
{
    unsigned long buf = start;
    unsigned long size = len;
    arch_address_align(&buf, &size);

    CacheCleanAndInvalidateMemory((void *)buf, size);
}

void gpio_init(void)
{
    /*nothing to do for crane modem */
    return;
}

void gpio_set_direction(int gpio_idx, int direction)
{
    GpioSetDirection((UINT32)gpio_idx, (GPIOPinDirection)direction);
}

void gpio_output_set(int gpio_idx, int value)
{
    GpioSetLevel((UINT32)gpio_idx, (UINT32)value);
}

void gpio_enable_edge_detect(int gpio_idx, int edge_flag)
{
    if (edge_flag & EDGE_FLAG_RISE) {
        GpioEnableEdgeDetection((UINT32)gpio_idx, (GPIOTransitionType)GPIO_RISE_EDGE);
    } else {
        GpioDisableEdgeDetection((UINT32)gpio_idx, (GPIOTransitionType)GPIO_RISE_EDGE);
    }

    if (edge_flag & EDGE_FLAG_FALL) {
        GpioEnableEdgeDetection((UINT32)gpio_idx, (GPIOTransitionType)GPIO_FALL_EDGE);
    } else {
        GpioDisableEdgeDetection((UINT32)gpio_idx, (GPIOTransitionType)GPIO_FALL_EDGE);
    }
}

void gpio_register_int_handler(unsigned int gpio_idx, gpio_int_handler func)
{
    GpioBindInterruptCallback((UINT32)gpio_idx, (GPIOCallback)func);
}

#define MS_PER_OS_TICK 5

static unsigned tick_per_second;
static unsigned sys_ticks = 0;
static tick_cb_t tick_cb = NULL;


unsigned lv_get_ticks_per_second(void)
{
    return tick_per_second;
}

