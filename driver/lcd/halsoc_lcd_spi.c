#include "halsoc_lcd_spi.h"

#if USE_CRANE_LCD_SPI

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "hal_board.h"


//#include "../../board.h"
#include "halsoc_lcdc.h"
#include "bsp_common.h"

#define USE_SW_CS 1

#define LCD_SPI_WRITE_CMD 1
#define LCD_SPI_WRITE_DATA 0

/* LCD reg */
#define LCD_SPI_CTRL 0x180
#define LCD_MISC_CTRL 0x1F8
#define SPU_IRQ_ISR_RAW 0x1C4
#define LCD_SPI_TXDATA 0x184
#define LCD_SPI_RXDATA 0x140

#define CFG_SPI_START BIT_0       /* for reg LCD_SPI_CTRL */
#define CFG_SPI_3W4WB BIT_1       /* for reg LCD_SPI_CTRL */
#define CFG_SPI_SEL BIT_2         /* for reg LCD_SPI_CTRL */
#define CFG_SPI_ENA BIT_3         /* for reg LCD_SPI_CTRL */
#define CFG_TXBITSTO0 BIT_4       /* for reg LCD_SPI_CTRL */
#define CFG_RXBITSTO0 BIT_5       /* for reg LCD_SPI_CTRL */
#define CFG_CLKINV BIT_7          /* for reg LCD_SPI_CTRL */
#define CFG_TXBITS_BIT_COUNT 8    /* for reg LCD_SPI_CTRL */
#define CFG_TXBITS_BIT_SHIFT 8    /* for reg LCD_SPI_CTRL */
#define CFG_RXBITS_BIT_COUNT 8    /* for reg LCD_SPI_CTRL */
#define CFG_RXBITS_BIT_SHIFT 16   /* for reg LCD_SPI_CTRL */
#define CFG_SCLKCNT_BIT_COUNT 8   /* for reg LCD_SPI_CTRL */
#define CFG_SCLKCNT_BIT_SHIFT 24  /* for reg LCD_SPI_CTRL */

#define SPI_HW_CTRL BIT_0       /* for reg LCD_MISC_CTRL */
#define SMPN_SWAPRB   BIT_1     /* for reg LCD_MISC_CTRL */
#define SPI_2IN_MODE  BIT_2     /* for reg LCD_MISC_CTRL */
#define CFG_SPI_3L4LB BIT_3     /* for reg LCD_MISC_CTRL */

#define SPI_IRQ BIT_18          /* for reg SPU_IRQ_ISR_RAW */

#if USE_LCD_PANEL_ST7735S_SPI_POC
#define SPI_4LINE_DCX 27
#else
#define SPI_4LINE_DCX 21
#endif

typedef struct {
    uint32_t sclk; /*KHz*/
    uint16_t cur_path;
    uint16_t status;
    uint16_t reserved;
    const struct spi_info *info;
} spi_context;

static int32_t spi_set_clk(uint32_t src_clk, uint32_t spi_clk);
static void spi_set_path(uint32_t path);
static void spi_trigger(void);
static int32_t spi_write_common(uint32_t data, uint32_t bits, uint32_t is_cmd);

static spi_context g_spi_ctx;

int32_t lcd_spi_init(uint32_t sclk, const struct spi_info *info)
{
    uint32_t reg = 0;

    printf("lcd_spi_init, line_num = %d\n", info->line_num);
    if (!info || !info->timing) {
        printf("ERROR: lcd_spi_init, Invalid param!\n");
        return -1;
    }

    if (info->format != SPI_FORMAT_RGB565) {
        printf("ERROR: lcd_spi_init, Format is error!\n");
        return -1;
    }

    g_spi_ctx.cur_path = 0;
    g_spi_ctx.status = 0;
    g_spi_ctx.sclk = sclk;
    g_spi_ctx.info = info;

    /*enable spi*/
    reg |= ((info->interface_id == 1) ? CFG_SPI_3W4WB : 0);
    reg |= ((info->device_id == 1) ? CFG_SPI_SEL : 0);
    reg |= ((info->endian == SPI_ENDIAN_LSB) ? (CFG_TXBITSTO0 | CFG_RXBITSTO0) : 0);
    reg |= ((info->sample_edge == SPI_EDGE_FALLING) ? CFG_CLKINV : 0);
    reg |= CFG_SPI_ENA;
    hal_lcdc_write(LCD_SPI_CTRL, reg);

    /*MISC set*/
    reg = 0;
    reg |= ((info->line_num == 3) ? CFG_SPI_3L4LB : 0);
    reg |= ((info->data_lane_num == 2) ? SPI_2IN_MODE : 0);
    reg |= SMPN_SWAPRB; /*should be set, otherwith, color will error*/
    hal_lcdc_write(LCD_MISC_CTRL, reg);

    if (info->line_num == 4) {
        /* 4 line mode, set DCX pin first */
        gpio_set_direction(SPI_4LINE_DCX, GPIO_OUT);
        gpio_output_set(SPI_4LINE_DCX, GPIO_LOW);
    }

    /*set write clk*/
    if (spi_set_clk(sclk, info->timing->wclk) < 0) {
        printf("ERROR: lcd_spi_init, set spi clk error!\n");
        return -1;
    }

    /*set register path as default*/
    spi_set_path(SPI_PATH_REGISTER);
    g_spi_ctx.status = SPI_STATUS_INIT;

    return 0;
}

__attribute__ ((unused)) static void lcd_spi_set_cs(uint32_t enable)
{
    if(enable)
        hal_lcdc_set_bits(LCD_SPI_CTRL, BIT_6);
    else
        hal_lcdc_clear_bits(LCD_SPI_CTRL, BIT_6);
}

int32_t lcd_spi_write_cmd(uint32_t cmd, uint32_t bits)
{
    return spi_write_common(cmd, bits, LCD_SPI_WRITE_CMD);
}

int32_t lcd_spi_write_data(uint32_t data, uint32_t bits)
{
    return spi_write_common(data, bits, LCD_SPI_WRITE_DATA);
}

int32_t lcd_spi_read_data(uint32_t cmd, uint32_t cmd_bits,
                      uint32_t *data,  uint32_t data_bits, uint8_t read_gram)
{
    unsigned wdata, wbits, rbits;

    if ((data_bits > 32) || (cmd_bits > 32)) {
        printf("ERROR: lcd_spi_read_data : wrong param, data_bits = 0x%x, cmd_bits = 0x%x\n",
               (unsigned)data_bits, (unsigned)cmd_bits);
        return -1;
    }

    if (SPI_STATUS_INIT != g_spi_ctx.status) {
        printf("ERROR: lcd_spi_read_data, Invalid SPI status\r\n");
        return -1;
    }

    if (spi_set_clk(g_spi_ctx.sclk, g_spi_ctx.info->timing->rclk) < 0) {
        printf("ERROR: lcd_spi_read_data, set spi clk error! \n");
        return -1;
    }

    spi_set_path(SPI_PATH_REGISTER);

    rbits = (data_bits > 8) ? (data_bits) : (data_bits - 1);
    if(read_gram) {
        rbits -= 1;
    }
    wdata = cmd;
    wbits = cmd_bits;
    if (g_spi_ctx.info->line_num == 4) {
        wbits -= 1;
        /*4 line mode, set DCX pin first*/
        gpio_set_direction(SPI_4LINE_DCX, GPIO_OUT);
        gpio_output_set(SPI_4LINE_DCX, GPIO_LOW);
    }

    hal_lcdc_write(LCD_SPI_TXDATA, wdata);
    hal_lcdc_write_bits(LCD_SPI_CTRL, wbits, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
    hal_lcdc_write_bits(LCD_SPI_CTRL, rbits, CFG_RXBITS_BIT_COUNT, CFG_RXBITS_BIT_SHIFT);
    spi_trigger();

    *data = hal_lcdc_read(LCD_SPI_RXDATA);

    /*set write clk as default*/
    if (spi_set_clk(g_spi_ctx.sclk, g_spi_ctx.info->timing->wclk) < 0) {
        printf("ERROR: lcd_spi_read_data, set spi clk error!\r\n");
        return -1;
    }

    return 0;
}

int32_t lcd_spi_before_refresh(void)
{
    spi_context *spi_ctx = &g_spi_ctx;

    spi_set_path(SPI_PATH_IMAGE);

    if (2 == spi_ctx->info->data_lane_num) {
        /*2 data lane*/
        switch (spi_ctx->info->format) {
            case SPI_FORMAT_RGB565:
                hal_lcdc_write_bits(LCD_SPI_CTRL, 15, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
                break;
            case SPI_FORMAT_RGB666:
                hal_lcdc_write_bits(LCD_SPI_CTRL, 17, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
                break;
            case SPI_FORMAT_RGB666_2_3:
                hal_lcdc_write_bits(LCD_SPI_CTRL, 11, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
                break;
            case SPI_FORMAT_RGB888:
                hal_lcdc_write_bits(LCD_SPI_CTRL, 23, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
                break;
            case SPI_FORMAT_RGB888_2_3:
                hal_lcdc_write_bits(LCD_SPI_CTRL, 15, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
                break;
        }
    }
    else {
        /*1 data lane*/
        hal_lcdc_write_bits(LCD_SPI_CTRL, 7, CFG_TXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);
    }

    if (4 == spi_ctx->info->line_num) { /*4 line mode, set DCX pin first*/
        // lcdc_write_bits(DUMB_CONTROL, SPI_4LINE_DCX, MASK8, 20);
        gpio_set_direction(SPI_4LINE_DCX, GPIO_OUT);
        gpio_output_set(SPI_4LINE_DCX, GPIO_HIGH);
    }

#if USE_SW_CS
    lcd_spi_set_cs(1);
#endif
    return 0;
}

int32_t lcd_spi_after_refresh(void)
{
#if USE_SW_CS
    lcd_spi_set_cs(0);
#endif
    spi_set_path(SPI_PATH_REGISTER);

    return 0;
}

static int32_t spi_set_clk(uint32_t src_clk, uint32_t spi_clk)
{
    uint32_t divider;

    divider = src_clk / spi_clk;
    if ((divider > 0xFF) || (divider < 2)) {
        printf("spi_set_clk failed, wrong divider: %d / %d \n", src_clk, spi_clk);
        return -1;
    }

    hal_lcdc_write_bits(LCD_SPI_CTRL, divider, CFG_SCLKCNT_BIT_COUNT, CFG_SCLKCNT_BIT_SHIFT);
    return 0;
}

static void spi_set_path(uint32_t path)
{
    if (g_spi_ctx.cur_path == path) {
        return;
    }

    if (path == SPI_PATH_IMAGE) {
        hal_lcdc_set_bits(LCD_MISC_CTRL, SPI_HW_CTRL);
    }
    else {
        hal_lcdc_clear_bits(LCD_MISC_CTRL, SPI_HW_CTRL);
    }
    g_spi_ctx.cur_path = path;
    return;
}

/* return after transfer complete */
static void spi_trigger(void)
{
    uint32_t val;
#if USE_SW_CS
    lcd_spi_set_cs(1);
#endif
    hal_lcdc_set_bits(LCD_SPI_CTRL, CFG_SPI_START);

    while (1) {
        val = hal_lcdc_read(SPU_IRQ_ISR_RAW);
        if (val & SPI_IRQ) {
            hal_lcdc_write(SPU_IRQ_ISR_RAW, val & ~SPI_IRQ);
            break;
        }
    }

    hal_lcdc_clear_bits(LCD_SPI_CTRL, CFG_SPI_START);
#if USE_SW_CS
    lcd_spi_set_cs(0);
#endif
}

static int32_t spi_write_common(uint32_t data, uint32_t bits, uint32_t is_cmd)
{
    if (bits > 32 || 0 == bits) {
        printf("ERROR: spi_write_common, Invalid param, bits = %d ?\n", bits);
        return -1;
    }

    spi_set_path(SPI_PATH_REGISTER);

    uint32_t wbits;
    uint32_t wdata = data;

    if (3 == g_spi_ctx.info->line_num) {
        if (bits == 32) {
            printf("ERROR: spi_write_common, too many write bits for 3 line mode!\n");
            return -1;
        }
        wbits = bits;
        if (LCD_SPI_WRITE_DATA == is_cmd) {
            wdata = (1 << bits) | data;
        }
    }
    else if (4 == g_spi_ctx.info->line_num) {
        wbits = bits - 1;
        /*4 line mode, set DCX pin first*/
        gpio_set_direction(SPI_4LINE_DCX, GPIO_OUT);
        if (LCD_SPI_WRITE_CMD == is_cmd) {
            gpio_output_set(SPI_4LINE_DCX, GPIO_LOW);
        }
        else {
            gpio_output_set(SPI_4LINE_DCX, GPIO_HIGH);
        }
    }
    else {
        printf("ERROR: spi_write_common, Error line number \n");
        return -1;
    }
    hal_lcdc_write(LCD_SPI_TXDATA, wdata);
    hal_lcdc_write_bits(LCD_SPI_CTRL, wbits, CFG_TXBITS_BIT_COUNT + CFG_RXBITS_BIT_COUNT, CFG_TXBITS_BIT_SHIFT);

    spi_trigger();

    return 0;
}

#endif
