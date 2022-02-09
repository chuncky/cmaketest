
#include "hal_conf.h"
#include "halsoc_lcdc.h"

#if USE_CRANE_LCD
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "hal_board.h"

#include "halsoc_lcd_spi.h"
#include "halsoc_lcd_mcu.h"
#include "halsoc_lcd_mipi_dsi.h"
#include "bsp_common.h"



/* PMU reg */
#define MULTIMEDIA_CLK_RES_CTRL0 0x4c
#define MULTIMEDIA_CLK_RES_CTRL1 0x50

#define Multimedia_SLEEP2 BIT_18   /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define Multimedia_SLEEP1 BIT_17   /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define Multimedia_ISOB   BIT_16   /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_PST_CKEN      BIT_9    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_PST_OUTDIS    BIT_8    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_CLK_SEL       BIT_6    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_HCLK_EN       BIT_5    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_CLK_EN        BIT_4    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_AXICLK_EN     BIT_3    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_HCLK          BIT_2    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_RST           BIT_1    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/
#define LCD_BUS_RST       BIT_0    /*for reg MULTIMEDIA_CLK_RES_CTRL0*/

#define LCD_CI_ACLK_EN    BIT_4    /*for reg MULTIMEDIA_CLK_RES_CTRL1*/
#define LCD_CI_ACLK_RST   BIT_1    /*for reg MULTIMEDIA_CLK_RES_CTRL1*/

/* LCD reg */
#define LCD_GRA_START_ADDR0 0xF4
#define LCD_GRA_PITCH 0xFC
#define LCD_GRA_OVSA_HPXL_VLN 0x100
#define LCD_GRA_HPXL_VLN 0x104
#define LCD_GRAZM_HPXL_VLN 0x108

#define LCD_DMA_START_ADDR_Y0 0xc0
#define LCD_DMA_START_ADDR_U0 0xC4
#define LCD_DMA_START_ADDR_V0 0xC8

#define LCD_DMA_PITCH_YC 0xe0
#define LCD_DMA_PITCH_UV 0xE4
#define LCD_DMA_OVSA_HPXL_VLN 0xe8
#define LCD_DMA_HPXL_VLN 0xec
#define LCD_DMAZM_HPXL_VLN 0xf0
#define LCD_TV_CTRL0  0x80
#define LCD_TV_CTRL1 0x84

#define LCD_PN_SEPXLCNT 0x13C

#define LCD_PN_CTRL0 0x190
#define LCD_PN_CTRL1 0x194
#define SPU_IRQ_ENA 0x1C0
#define SPU_IRQ_ISR_RAW 0x1C4
#define PN_IOPAD_CONTROL 0x1BC

#define LCD_PN_V_H_TOTAL 0x114
#define LCD_PN_V_H_ACTIVE 0x118
#define LCD_PN_H_PORCH 0x11C
#define LCD_PN_V_PORCH 0x120
#define LCD_TOP_CTRL 0x1DC
#define LCD_SMPN_CTRL 0x188
#define LCD_PN_BLANKCOLOR 0x124
#define LCD_AFA_ALL2ONE 0x1E8
#define LCD_DITHER_CTRL 0x1EC
#define LCD_DITHER_TBL_DATA 0x1F0
#define LCD_SCLK_DIV 0x1A8

#define LCD_WDMA_CTRL1 0x200
#define LCD_WDMA_CTRL2 0x204
#define LCD_WDMA_CTRL3 0x208
#define LCD_WDMA_CTRL4 0x20C

#define LCD_PN_COLORKEY_Y 0x130
#define LCD_PN_COLORKEY_U 0x134
#define LCD_PN_COLORKEY_V 0x138

#define LCD_DUMB_CONTROL   0x1B8
#define LCD_SHADOW_CTRL    0xC

#define CFG_DMA_ENA BIT_0                   /*for reg LCD_PN_CTRL0 */
#define CFG_YUV2RGB_DMA BIT_1               /*for reg LCD_PN_CTRL0 */
#define CFG_DMA_SWAPRB  BIT_4               /*for reg LCD_PN_CTRL0 */
#define CFG_GRA_ENA BIT_8                   /*for reg LCD_PN_CTRL0 */
#define CFG_YUV2RGB_TVG BIT_9               /*for reg LCD_PN_CTRL0 */
#define CFG_TVG_SWAPRB  BIT_12              /*for reg LCD_PN_CTRL0 */
#define CFG_GRAFORMAT_BIT_SHIFT 16          /*for reg LCD_PN_CTRL0 */
#define CFG_GRAFORMAT_BIT_COUNT 4           /*for reg LCD_PN_CTRL0 */
#define CFG_DMAFORMAT_BIT_SHIFT 20          /*for reg LCD_PN_CTRL0 */
#define CFG_DMAFORMAT_BIT_COUNT 4           /*for reg LCD_PN_CTRL0 */
#define CFG_FORCE_BLANKCOLOR_ENA  BIT_24    /*for reg LCD_PN_CTRL0 */
#define CFG_ARBFAST_ENA  BIT_27             /*for reg LCD_PN_CTRL0 */
#define CFG_PN_CBSH_ENA BIT_29              /*for reg LCD_PN_CTRL0 */
#define CFG_PN_GAMMA_ENA BIT_30             /*for reg LCD_PN_CTRL0 */

#define CFG_PXLCMD_VALUE 0x81                       /*for reg LCD_PN_CTRL1 */
#define CFG_PN_ALPHA_SHIFT  8                       /*for reg LCD_PN_CTRL1 */
#define CFG_PN_ALPHA_COUNT  8                       /*for reg LCD_PN_CTRL1 */
#define CFG_PN_ALPHA_MODE BIT_16                    /*for reg LCD_PN_CTRL1 */
#define CFG_PN_CARRY  BIT_23                        /*for reg LCD_PN_CTRL1 */
#define CFG_PN_VSYNC_INV BIT_27                     /*for reg LCD_PN_CTRL1 */
#define CFG_PN_VSYNC_MODE (BIT_28 | BIT_29 | BIT_30)  /*for reg LCD_PN_CTRL1 */
#define CFG_FRAME_TRIG BIT_31                       /*for reg LCD_PN_CTRL1 */

#define CFG_CKEY_GRA   BIT_19        /*for reg LCD_TV_CTRL1 */

#define CFG_SMPN_ENA BIT_0           /*for reg LCD_SMPN_CTRL */
#define CFG_IORDY_MSK BIT_13         /*for reg LCD_SMPN_CTRL */
#define SMPN_RSTB   BIT_3            /*for reg LCD_SMPN_CTRL */
#define CFG_SMPNMODE_BIT_SHIFT 8     /*for reg LCD_SMPN_CTRL */
#define CFG_SMPNMODE_BIT_COUNT 4     /*for reg LCD_SMPN_CTRL */

#define IRQ_WB_DONE   BIT_14        /*for reg SPU_IRQ_ISR_RAW */
#define SPI_FRAMEDONE BIT_20        /*for reg SPU_IRQ_ISR_RAW */
#define TXC_FRAMEDONE BIT_21        /*for reg SPU_IRQ_ISR_RAW */

#define IRQ_TE_DONE BIT_23        /*for reg SPU_IRQ_ISR_RAW */

#if USE_CRANE_LCD_MCU
    #define FRAME_DONE_BIT  TXC_FRAMEDONE
#elif USE_CRANE_LCD_MIPI
  #if USE_CRANE_LCD_MIPI_VIDEO
    #define FRAME_DONE_BIT  BIT_26
  #else
    #define FRAME_DONE_BIT  BIT_30
  #endif
#else
    #define FRAME_DONE_BIT  SPI_FRAMEDONE
#endif

#define CFG_DMA_VM_ENA BIT_13       /*for reg PN_IOPAD_CONTROL */
#define CFG_GRA_VM_ENA BIT_15       /*for reg PN_IOPAD_CONTROL */

#define WDMA_ENA BIT_0              /*for reg LCD_WDMA_CTRL1 */

#define DITHER_EN_PN BIT_0                 /*for reg LCD_DITHER_CTRL */
#define DITHER_4x8_PN BIT_1                /*for reg LCD_DITHER_CTRL */
#define DITHER_MODE_PN_565 BIT_4           /*for reg LCD_DITHER_CTRL */
#define DITHER_MODE_PN_666 BIT_5           /*for reg LCD_DITHER_CTRL */
#define DITHER_TABLE_INDEX_SEL_SHIFT 16    /*for reg LCD_DITHER_CTRL */
#define DITHER_TABLE_INDEX_SEL_COUNT 2     /*for reg LCD_DITHER_CTRL */

#define DITHER_TB_4X4_INDEX0       (0x6e4ca280)    /*for reg LCD_DITHER_TBL_DATA */
#define DITHER_TB_4X4_INDEX1       (0x5d7f91b3)    /*for reg LCD_DITHER_TBL_DATA */
#define DITHER_TB_4X8_INDEX0       (0xb391a280)    /*for reg LCD_DITHER_TBL_DATA */
#define DITHER_TB_4X8_INDEX1       (0x7f5d6e4c)    /*for reg LCD_DITHER_TBL_DATA */
#define DITHER_TB_4X8_INDEX2       (0x80a291b3)    /*for reg LCD_DITHER_TBL_DATA */
#define DITHER_TB_4X8_INDEX3       (0x4c6e5d7f)    /*for reg LCD_DITHER_TBL_DATA */

static lv_disp_t * lcd_disp = NULL;
void hallcdc_set_disp(lv_disp_t * disp)
{
     lcd_disp = disp;
}

static volatile uint8_t is_sync = 0;

static int is_format_yuv(uint32_t format)
{
    int is_yuv = 0;

    switch (format) {
        case LCD_FORMAT_RGB565:
        case LCD_FORMAT_RGB1555:
        case LCD_FORMAT_RGB888_PACKED:
        case LCD_FORMAT_RGB888_UNPACKED:
        case LCD_FORMAT_RGBA888:
        case LCD_FORMAT_RGB888A:
            is_yuv = 0;
            break;
        case LCD_FORMAT_YUV422_PACKED:
        case LCD_FORMAT_YUV422_PLANAR:
        case LCD_FORMAT_YUV420_PLANAR:
        case LCD_FORMAT_YUV420_SEMI:
            is_yuv = 1;
            break;
    }

    return is_yuv;
}

static int get_format_plane(uint32_t format)
{
    int plane = 1;

    switch (format) {
        case LCD_FORMAT_RGB565:
        case LCD_FORMAT_RGB1555:
        case LCD_FORMAT_RGB888_PACKED:
        case LCD_FORMAT_RGB888_UNPACKED:
        case LCD_FORMAT_RGBA888:
        case LCD_FORMAT_RGB888A:
        case LCD_FORMAT_YUV422_PACKED:
            plane = 1;
            break;
        case LCD_FORMAT_YUV422_PLANAR:
        case LCD_FORMAT_YUV420_PLANAR:
            plane = 3;
            break;
        case LCD_FORMAT_YUV420_SEMI:
            plane = 2;
            break;
    }
    return plane;
}

static void multimedia_clk_onoff(struct lcd_info_t *info, int on)
{
    if (on) {
        hal_mediaclk_poweron(MODULE_DISPLAY);
        hal_mediaclk_enable(MEDIA_CLK_LCD_AHB | MEDIA_CLK_LCD_CI, MODULE_DISPLAY);
        hal_mediaclk_set_rate(MEDIA_CLK_LCD_SCLK, info->clk);

        hal_mediaclk_enable(MEDIA_CLK_LCD_SCLK | MEDIA_CLK_LCD_AXI, MODULE_DISPLAY);
        hal_mediaclk_dump(LEVEL_DUMP_CLOCK_ONLY);
    }
    else {   /* off */
        hal_mediaclk_disable(MEDIA_CLK_LCD_AHB | MEDIA_CLK_LCD_CI | MEDIA_CLK_LCD_SCLK | MEDIA_CLK_LCD_AXI, MODULE_DISPLAY);
        hal_mediaclk_poweroff(MODULE_DISPLAY);
    }
}

__attribute__ ((unused)) static void hal_lcdc_clk(struct lcd_info_t *info, int on)
{
    if (on) {
        multimedia_clk_onoff(info, 1);
    }
    else {
        multimedia_clk_onoff(info, 0);
    }
}

static void lcd_config_img_color_format(unsigned color_format)
{
    switch (color_format) {
        case LCD_FORMAT_RGB565:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 0, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_PN_CARRY);
            break;
        case LCD_FORMAT_RGB1555:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 1, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_PN_CARRY);
            break;
        case LCD_FORMAT_RGB888_PACKED:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 2, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_RGB888_UNPACKED:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 3, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_RGBA888:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 4, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_YUV422_PACKED:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 5, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_YUV422_PLANAR:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 6, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_YUV420_PLANAR:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 7, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_RGB888A:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 0xB, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_YUV420_SEMI:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 0xC, CFG_DMAFORMAT_BIT_COUNT, CFG_DMAFORMAT_BIT_SHIFT);
            break;
        default:
            printf("Warning: lcd_config_img_color_format, Invalid color format (%d)! \n", color_format);
    }
    return;
}

static void lcd_config_img_layer(lcd_layer_config *layer_config)
{
    int reg = 0;
    int plane = 0;

    if (0 == layer_config->layer_enable) {
        hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_DMA_ENA);
        return;
    }

    hal_lcdc_write(LCD_DMA_OVSA_HPXL_VLN, (layer_config->x) | (layer_config->y << 16));
    reg = (layer_config->width) | (layer_config->height << 16);
    hal_lcdc_write(LCD_DMA_HPXL_VLN, reg);
    if((layer_config->dst_width!=0) && (layer_config->dst_height!=0)
        && ((layer_config->dst_width!=layer_config->width) || (layer_config->dst_height!=layer_config->height))) {
        reg = (layer_config->dst_width) | (layer_config->dst_height << 16);
    }
    hal_lcdc_write(LCD_DMAZM_HPXL_VLN, reg);
    hal_lcdc_write(LCD_DMA_START_ADDR_Y0, (unsigned)layer_config->buf1);
    hal_lcdc_write(LCD_DMA_PITCH_YC, layer_config->stride);

    plane = get_format_plane(layer_config->format);
    if (plane == 2) { /*u plane*/
        hal_lcdc_write(LCD_DMA_START_ADDR_U0, (unsigned)layer_config->buf2);
        hal_lcdc_write(LCD_DMA_PITCH_UV, layer_config->stride);
    }
    else if (3 == plane) {    /*v plane*/
        printf("Error : yuv plane 3 not supported yet\n");
    }

    lcd_config_img_color_format(layer_config->format);

    if (1 == is_format_yuv(layer_config->format)) {
        hal_lcdc_set_bits(LCD_PN_CTRL0, CFG_YUV2RGB_DMA);             /* support YUV */
        hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_DMA_SWAPRB);
        hal_lcdc_set_bits(LCD_PN_CTRL0, CFG_DMA_ENA);  /* enable layer, RB swap as default*/
    }
    else {
        hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_YUV2RGB_DMA);             /* not support YUV */
        hal_lcdc_set_bits(LCD_PN_CTRL0, CFG_DMA_ENA /*| CFG_DMA_SWAPRB*/);  /* enable layer*/
    }

    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_PN_CBSH_ENA);
    hal_lcdc_clear_bits(PN_IOPAD_CONTROL, CFG_DMA_VM_ENA);
}

void lcd_config_osd1(lcd_layer_config *layer_config)
{
    int reg = 0;
    if (0 == layer_config->layer_enable) {
        hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_GRA_ENA);
        return;
    }

    hal_lcdc_write(LCD_GRA_START_ADDR0, (unsigned)layer_config->buf1);
    hal_lcdc_write(LCD_GRA_PITCH, layer_config->stride);
    hal_lcdc_write(LCD_GRA_OVSA_HPXL_VLN, layer_config->x | (layer_config->y << 16));
    reg = (layer_config->height << 16) | (layer_config->width);
    hal_lcdc_write(LCD_GRA_HPXL_VLN, reg);
    if((layer_config->dst_width!=0) && (layer_config->dst_height!=0)
        && ((layer_config->dst_width!=layer_config->width) || (layer_config->dst_height!=layer_config->height))) {
        reg = (layer_config->dst_width) | (layer_config->dst_height << 16);
    }
    hal_lcdc_write(LCD_GRAZM_HPXL_VLN, reg);

    switch (layer_config->format) {
        case LCD_FORMAT_RGB565:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 0, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_PN_CARRY);
            break;
        case LCD_FORMAT_RGB1555:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 1, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_PN_CARRY);
            break;
        case LCD_FORMAT_RGB888_PACKED:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 2, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_RGB888_UNPACKED:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 3, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_RGBA888:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 4, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            break;
        case LCD_FORMAT_YUV422_PACKED:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 5, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            hal_lcdc_set_bits(LCD_PN_CTRL0, BIT_9);
            break;
        case LCD_FORMAT_RGB888A:
            hal_lcdc_write_bits(LCD_PN_CTRL0, 0xB, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
            break;
        default:
            printf("Warning: lcd_config_osd1, Invalid layer format (%d)!\r\n", layer_config->format);
            hal_lcdc_write_bits(LCD_PN_CTRL0, 0, CFG_GRAFORMAT_BIT_COUNT, CFG_GRAFORMAT_BIT_SHIFT);
    }

    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_YUV2RGB_TVG);
#if !USE_LCD_RB_SWAP_DISABLE
    hal_lcdc_set_bits(LCD_PN_CTRL0, CFG_TVG_SWAPRB);    /* RB swap as default */
#endif
    hal_lcdc_set_bits(LCD_PN_CTRL0, CFG_GRA_ENA);     /*layer enable*/
    if (layer_config->alpha != 0xFF) {
        hal_lcdc_clear_bits(LCD_PN_CTRL1, CFG_PN_ALPHA_MODE);
        hal_lcdc_write_bits(LCD_PN_CTRL1, layer_config->alpha, CFG_PN_ALPHA_COUNT, CFG_PN_ALPHA_SHIFT);
    }
    else {
        hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_PN_ALPHA_MODE);
    }

    hal_lcdc_clear_bits(LCD_TV_CTRL1, CFG_CKEY_GRA);       //color key disable
    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_PN_CBSH_ENA);     //disable panel Video Contrast/Saturation/Brightness/Hue Adjust
    hal_lcdc_clear_bits(PN_IOPAD_CONTROL, CFG_GRA_VM_ENA);   //Panel Path Graphic Vertical Mirror disable
}

void lcd_config_output(lcd_output_config *output_config, uint8_t wb_enable)
{
    int reg = 0;
    hal_lcdc_write(LCD_PN_V_H_ACTIVE, (output_config->width) | (output_config->height << 16));

    if(output_config->format == MIPI_FORMAT_VIDEO) {
        reg = ((output_config->hbp)<<16) | output_config->hfp;
        hal_lcdc_write(LCD_PN_H_PORCH, reg);
        reg = ((output_config->vbp)<<16) | output_config->vfp;
        hal_lcdc_write(LCD_PN_V_PORCH, reg);
        reg = ((output_config->height + output_config->vbp + output_config->vfp + output_config->vsync) << 16) |
               (output_config->width + output_config->hbp + output_config->hfp + output_config->hsync);
        hal_lcdc_write(LCD_PN_V_H_TOTAL, reg);
        reg = ((output_config->width + output_config->hbp + output_config->hfp + output_config->hsync) << 16) | output_config->width;
        hal_lcdc_write(LCD_PN_SEPXLCNT, reg);
        printf("hbp:%d, hfp:%d, hsync:%d, vbp:%d, vfp:%d, vysnc:%d\n", output_config->hbp, output_config->hfp, output_config->hsync,
          output_config->vbp, output_config->vfp, output_config->vsync);
    }

    hal_lcdc_write(LCD_TOP_CTRL, 0x40FC00);

    reg = hal_lcdc_read(LCD_PN_CTRL1);
    reg &= ~CFG_FRAME_TRIG;

    if(output_config->format == MIPI_FORMAT_VIDEO) {
        reg |= BIT_29;
        reg &= ~(BIT_28 | BIT_30);
    } else {
        if(wb_enable) {
            reg |= CFG_PN_VSYNC_MODE;
        } else {
#if LCD_TE_ENABLE
            reg &= ~CFG_PN_VSYNC_MODE;
#else
            reg |= CFG_PN_VSYNC_MODE;
#endif
        }
    }

#if (LCD_TE_POL == 0)
    reg &= ~CFG_PN_VSYNC_INV;
#else
    reg |= CFG_PN_VSYNC_INV;
#endif

    reg |= CFG_PXLCMD_VALUE;
    hal_lcdc_write(LCD_PN_CTRL1, reg);

    hal_lcdc_set_bits(LCD_PN_CTRL0, CFG_ARBFAST_ENA);

    if(wb_enable == 0) {
        if(output_config->format < SPI_FORMAT_LIMIT) {
            switch (output_config->format) {
                case SPI_FORMAT_RGB565:
                    if (output_config->data_lane_num == 2) {
                        reg = 5;
                    }
                    else {
                        reg = 2;
                    }
                    break;
                case SPI_FORMAT_RGB666:
                    if (output_config->data_lane_num == 2) {
                        reg = 4;
                    }
                    else {
                        reg = 1;
                    }
                    break;
                case SPI_FORMAT_RGB666_2_3:
                    reg = 6;
                    break;
                case SPI_FORMAT_RGB888:
                    reg = 3;
                    break;
                case SPI_FORMAT_RGB888_2_3:
                    reg = 0;
                    break;
            }
            hal_lcdc_write_bits(LCD_SMPN_CTRL, reg, CFG_SMPNMODE_BIT_COUNT, CFG_SMPNMODE_BIT_SHIFT);
            hal_lcdc_set_bits(LCD_SMPN_CTRL, CFG_IORDY_MSK);
        } else if(output_config->format < MCU_FORMAT_LIMIT) {
            if(output_config->format == MCU_FORMAT_RGB565){
                reg = 2;
            } else {
                reg = 1;
            }
            hal_lcdc_write_bits(LCD_SMPN_CTRL, reg, CFG_SMPNMODE_BIT_COUNT, CFG_SMPNMODE_BIT_SHIFT);
            hal_lcdc_clear_bits(LCD_SMPN_CTRL, CFG_IORDY_MSK);
        } else /*DSI cmd mode*/{
            if(output_config->format == MIPI_FORMAT_CMD){
                /*for DSI (smart panel), LCDC can only output RGB888!!!!*/
                hal_lcdc_write_bits(LCD_SMPN_CTRL, 3, CFG_SMPNMODE_BIT_COUNT, CFG_SMPNMODE_BIT_SHIFT);
            } else {/*video mode*/
                /*for DSI (smart panel), LCDC can only output RGB888!!!!*/
                hal_lcdc_write_bits(LCD_DUMB_CONTROL, 6, 4, 28);
            }
        }
    }
    if(output_config->format < MCU_FORMAT_LIMIT) {  /* no set for MIPI */
        hal_lcdc_set_bits(LCD_SMPN_CTRL, CFG_SMPN_ENA);
    }
}


void lcd_disable_wb(void)
{
    hal_lcdc_write(LCD_WDMA_CTRL1, 0);
    hal_lcdc_write(LCD_WDMA_CTRL2, 0);
    hal_lcdc_write(LCD_WDMA_CTRL3, 0);
}

void lcd_disable_img(void)
{
    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_DMA_ENA);
}

static void lcd_config_wb(lcd_wb_config *wb_config)
{
    int reg;

    if(0 == wb_config->wb_enable)  {
        hal_lcdc_write(LCD_WDMA_CTRL1, 0);
        hal_lcdc_write(LCD_WDMA_CTRL2, 0);
        hal_lcdc_write(LCD_WDMA_CTRL3, 0);
        return;
    }

    reg = (wb_config->stride << 16) | (16 << 8) | BIT_0;
    switch(wb_config->color_format){
        case LCD_FORMAT_RGB565:
            reg |= (0 << 4);
            hal_lcdc_write_bits(LCD_SMPN_CTRL, 5, 4, 8);
            break;
        case LCD_FORMAT_RGB888_PACKED:
            reg |= (1 << 4);
            hal_lcdc_write_bits(LCD_SMPN_CTRL, 3, 4, 8);
            break;
        case LCD_FORMAT_RGB888A:
            reg |= (3 << 4);
            hal_lcdc_write_bits(LCD_SMPN_CTRL, 3, 4, 8);
            break;
        case LCD_FORMAT_RGBA888:
            reg |= (2 << 4);
            hal_lcdc_write_bits(LCD_SMPN_CTRL, 3, 4, 8);
            break;
        default:
            printf("ERROR: lcd_config_wb: Unsupported format 0x%x\r\n", wb_config->color_format);
            return;
    }
    hal_lcdc_write(LCD_WDMA_CTRL1, reg);
    reg = (wb_config->height<< 16) | wb_config->width;
    hal_lcdc_write(LCD_WDMA_CTRL2, reg);
    hal_lcdc_write(LCD_WDMA_CTRL3, (uint32_t)wb_config->wb_buf);
    hal_lcdc_write(LCD_WDMA_CTRL4, 0x84440);
    hal_lcdc_set_bits(LCD_SMPN_CTRL, CFG_IORDY_MSK);
}

void lcdc_update_output_setting(lcd_output_config *output_config, uint32_t width, uint32_t height)
{
    output_config->height = height;
    output_config->width = width;
}

#if USE_CRANE_LCD_MIPI
void lcdc_update_mipi_output_setting(lcd_output_config *output_config, const struct s_mipi_info *mipi)
{
    output_config->hbp = mipi->hbp;
    output_config->hfp = mipi->hfp;
    output_config->hsync = mipi->hsync;
    output_config->vbp = mipi->vbp;
    output_config->vfp = mipi->vfp;
    output_config->vsync = mipi->vsync;
}
#endif

static unsigned mipi_video_flag = 0;
static void lcdc_after_sync(void)
{
    if(mipi_video_flag) {
        return;
    }

    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_DMA_ENA); /* disable image layer */
    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_GRA_ENA); /* disable OSD1 layer */

    hal_lcdc_clear_bits(LCD_WDMA_CTRL1, WDMA_ENA);
    hal_lcdc_clear_bits(LCD_SMPN_CTRL, CFG_IORDY_MSK);
    hal_lcdc_write_bits(LCD_PN_CTRL1, 0, 3, 28);
}



static int lcdc_wait_tesingnal(void)
{
    unsigned reg = 0;
    unsigned i = 0;
    int ret = 0;

    unsigned check_bit = IRQ_TE_DONE;
    reg = hal_lcdc_read(SPU_IRQ_ISR_RAW);
    hal_lcdc_write(SPU_IRQ_ISR_RAW, ~reg);
    while (1) {
        reg = hal_lcdc_read(SPU_IRQ_ISR_RAW);
        //printf("lcdc_wait_tesingnal: [0x%x][%x] ---\n", reg, check_bit);
        if (reg & check_bit) {
            break;
        }
        //i++;
        //mdelay(1);
    }

    if(i==100000) {
        printf("Timeout: lcdc_wait_complete: [0x%x][%x] ---\n", reg, check_bit);
        ret = -1;
    }

    hal_lcdc_write(SPU_IRQ_ISR_RAW, ~reg);
    return ret;
}


static int lcdc_wait_complete(uint8_t wb_enable)
{
    unsigned reg = 0;
    unsigned i = 0;
    int ret = 0;

    unsigned check_bit = FRAME_DONE_BIT;
    if(wb_enable)
        check_bit = BIT_14;

    while (i<100) {
        reg = hal_lcdc_read(SPU_IRQ_ISR_RAW);
        printf("lcdc_wait_complete: [0x%x][%x] ---\n", reg, check_bit);
        if (reg & check_bit) {
            break;
        }
        i++;
        mdelay(1);
    }

    if(i==100) {
        printf("Timeout: lcdc_wait_complete: [0x%x][%x] ---\n", reg, check_bit);
        ret = -1;
    }

    hal_lcdc_write(SPU_IRQ_ISR_RAW, ~reg);
    hal_lcdc_clear_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG);

    return ret;
}

#if 0
static void lcdc_dump_lcd_registers(void)
{
    int i;
    printf("++++++++\n");
    for(i = 0; i<0x300;i += 16){
        printf("0x%x: 0x%x, 0x%x, 0x%x, 0x%x\n", i,
          lcdc_read(i), lcdc_read(i+4), lcdc_read(i+8), lcdc_read(i+12));
    }
    printf("---------\n");

    printf("0xd4282820 = 0x%x\n", readl(0xd4282820));
    printf("0xd428284C = 0x%x\n", readl(0xd428284C));
    printf("0xd4282850 = 0x%x\n", readl(0xd4282850));

    printf("0xd401e2DC = 0x%x\n", readl(0xd401e2DC));
    printf("0xd401e12C = 0x%x\n", readl(0xd401e12C));
    printf("0xd401e134 = 0x%x\n", readl(0xd401e134));
    printf("0xd401e138 = 0x%x\n", readl(0xd401e138));
    printf("0xd401e13C = 0x%x\n", readl(0xd401e13C));
    printf("0xd401e140 = 0x%x\n", readl(0xd401e140));
    printf("0xd401e144 = 0x%x\n", readl(0xd401e144));
    printf("0xd401e130 = 0x%x\n", readl(0xd401e130));
    printf("0xd401e148 = 0x%x\n", readl(0xd401e148));
}
#endif

static unsigned first_time = 1;

int hal_lcdc_sync(lcd_context *lcd_ctx, int polling_mode)
{
    int ret = 0;
    unsigned reg = 0;
    lcd_config_osd1(&(lcd_ctx->layer_config_osd1));
    //lcd_config_img_layer(&(lcd_ctx->layer_config_img));
    //lcd_config_wb(&(lcd_ctx->wb_config));
    //lcd_config_output(&(lcd_ctx->output_config), lcd_ctx->wb_config.wb_enable);

    switch (lcd_ctx->alpha_mode) {
        case LCD_ALPHA_MODE_NORMAL:
            hal_lcdc_write(LCD_AFA_ALL2ONE, 0);
            break;
        case LCD_ALPHA_MODE_PRE:
            hal_lcdc_write(LCD_AFA_ALL2ONE, 1);
            break;
        case LCD_ALPHA_MODE_SPECIAL:
            hal_lcdc_write(LCD_AFA_ALL2ONE, 2);
            break;
    }

    if (polling_mode) { /* polling mode */

        hal_lcdc_clear_bits(SPU_IRQ_ENA, FRAME_DONE_BIT);
        barrier();
        uart_printf("LCD_PN_CTRL1:0x%x",hal_lcdc_read(LCD_PN_CTRL1));
        reg = hal_lcdc_read(SPU_IRQ_ISR_RAW);
        hal_lcdc_write(SPU_IRQ_ISR_RAW, ~reg);

        if(lcd_ctx->output_config.format != MIPI_FORMAT_VIDEO) {
            hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG); /* trigger transfer */
            mipi_video_flag = 0;
        } else {
            if(1 == first_time){  
                hal_lcdc_set_bits(LCD_SHADOW_CTRL, BIT_31);
                hal_lcdc_set_bits(LCD_DUMB_CONTROL, BIT_0);
                hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG);
                first_time = 0;
            } else {
                hal_lcdc_set_bits(LCD_SHADOW_CTRL, BIT_31);
            }
            mipi_video_flag = 1;
        }
        ret = lcdc_wait_complete(lcd_ctx->wb_config.wb_enable);
        lcdc_after_sync();
        hal_lcdc_set_bits(SPU_IRQ_ENA, FRAME_DONE_BIT);
        barrier();
    } else { /* irq mode */
        barrier();
        is_sync = 1;
        if(lcd_ctx->output_config.format != MIPI_FORMAT_VIDEO) {
            hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG); /* trigger transfer */
            mipi_video_flag = 0;
        } else { /*video mode*/
            if(1 == first_time){
                hal_lcdc_set_bits(LCD_SHADOW_CTRL, BIT_31);
                hal_lcdc_set_bits(LCD_DUMB_CONTROL, BIT_0);
                hal_lcdc_set_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG);
                first_time = 0;
            } else {
                hal_lcdc_set_bits(LCD_SHADOW_CTRL, BIT_31);
            }
            //lcdc_dump_lcd_registers();
            //lcd_dump_dsi_registers();
            mipi_video_flag = 1;
        }
    }

    lcd_ctx->layer_config_img.layer_enable = 0;
    lcd_ctx->layer_config_osd1.layer_enable = 0;

    return ret;
}

int hal_lcdc_stop(lcd_context *lcd_ctx)
{
    if(lcd_ctx->output_config.format == MIPI_FORMAT_VIDEO) {
        hal_lcdc_clear_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG);
        hal_lcdc_clear_bits(LCD_DUMB_CONTROL, BIT_0);
        hal_lcdc_set_bits(LCD_SHADOW_CTRL, BIT_31);
    }
    return 0;
}

static void hal_lcdc_irq(void *arg)
{
    unsigned reg;
    reg = hal_lcdc_read(SPU_IRQ_ISR_RAW);
    hal_lcdc_write(SPU_IRQ_ISR_RAW, ~reg);
    hal_lcdc_clear_bits(LCD_PN_CTRL1, CFG_FRAME_TRIG);
    printf("reg 0x%x\n", reg);
    if (reg & FRAME_DONE_BIT) {
        if(is_sync) {
          //lcd_dump_dsi_registers();
          lcdc_after_sync();
#if USE_CRANE_LCD_MCU
          lcd_mcu_after_refresh();
#elif USE_CRANE_LCD_MIPI
    #if USE_CRANE_LCD_MIPI_CMD
          lcd_dsi_after_refresh();
    #endif
#else
          lcd_spi_after_refresh();
#endif
          lv_disp_flush_ready(&lcd_disp->driver);
          is_sync = 0;
        }
    } else {
        //printf("unknown lcdc irq, reg 0x%x\n", reg);
    }
}

#define LCDC_IRQ 42

int hal_lcdc_init(struct lcd_info_t *info)
{
    if (!info) {
        printf("info is NULL, hal_lcdc_init failed\n");
        return -1;
    }

    if (info->mfp) {
        hal_mfp_config(info->mfp);
    }

    hal_lcdc_clk(info, 1);

    /* disable all layers */
    hal_lcdc_write(LCD_TV_CTRL0, 0); /* disable OSD2 layer */
    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_GRA_ENA); /* disable OSD1 layer */
    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_DMA_ENA); /* disable image layer */

    /* disable wb */
    hal_lcdc_write(LCD_WDMA_CTRL1, 0);
    hal_lcdc_write(LCD_WDMA_CTRL2, 0);
    hal_lcdc_write(LCD_WDMA_CTRL3, 0);

    /* disable gamma */
    hal_lcdc_clear_bits(LCD_PN_CTRL0, CFG_PN_GAMMA_ENA);
    /* disable dither */
    hal_lcdc_clear_bits(LCD_DITHER_CTRL, DITHER_EN_PN);

    register_int_handler(LCDC_IRQ, hal_lcdc_irq, NULL);
    unmask_interrupt(LCDC_IRQ);

    printf("FRAME_DONE_BIT:0x%x\n",FRAME_DONE_BIT);

    hal_lcdc_write(SPU_IRQ_ENA, FRAME_DONE_BIT | IRQ_WB_DONE); /* enable frame done irq */

    first_time = 1;

    return 0;
}

void hal_lcdc_reset_panel(void)
{
    hal_lcdc_set_bits(LCD_SMPN_CTRL, SMPN_RSTB);
    mdelay(20);
    hal_lcdc_clear_bits(LCD_SMPN_CTRL, SMPN_RSTB);
    mdelay(20);
}
extern void invalidate_dcache_range(unsigned long start, unsigned long stop) __attribute__((weak));

unsigned lcdc_rgb565_scale(int32_t src_width,
                   int32_t src_height,
                   int32_t dst_width,
                   int32_t dst_height,
                   uint8_t *src_buf,
                   uint8_t *dst_buf)
{
    return 0;
#if 0   /* no support yet*/
    if(dst_width % 4) {
        printf("Error: rgb565 scale, the width should be the multiple of 4!\n");
        return 0;
    }

    while(is_sync){
        // wait the prev sync finished!!
        ;
    }

    lcd_context lcd_ctx;

    lcd_ctx.layer_config_img.buf1 = (void *)src_buf;
    lcd_ctx.layer_config_img.format = LCD_FORMAT_RGB565;
    lcd_ctx.layer_config_img.layer_enable = 1;
    lcd_ctx.layer_config_img.stride = src_width*2;
    lcd_ctx.layer_config_img.x = 0;
    lcd_ctx.layer_config_img.y = 0;
    lcd_ctx.layer_config_img.width = src_width;
    lcd_ctx.layer_config_img.height = src_height;
    lcd_ctx.layer_config_img.dst_width = dst_width;
    lcd_ctx.layer_config_img.dst_height = dst_height;

    lcd_ctx.layer_config_osd1.layer_enable = 0;

    lcd_ctx.wb_config.wb_enable = 1;
    lcd_ctx.wb_config.color_format = LCD_FORMAT_RGB565;
    lcd_ctx.wb_config.width = dst_width;
    lcd_ctx.wb_config.height = dst_height;
    lcd_ctx.wb_config.stride = dst_width*2;
    lcd_ctx.wb_config.wb_buf = dst_buf;

    lcd_ctx.output_config.width = dst_width;
    lcd_ctx.output_config.height = dst_height;

    flush_cache((unsigned long)src_buf, (unsigned long)src_width*src_height*2);

    invalidate_dcache_range((unsigned long)dst_buf,
                       (unsigned long)dst_buf + dst_width*dst_height*2);
#if USE_CRANE_LCD_MCU
    lcd_mcu_before_refresh();
    hal_lcdc_sync(&lcd_ctx, 1);
    lcd_mcu_after_refresh();
#elif USE_CRANE_LCD_SPI
    lcd_spi_before_refresh();
    hal_lcdc_sync(&lcd_ctx, 1);
    lcd_spi_after_refresh();
#endif
    return 1;
#endif
}

unsigned lcdc_rgb888_2_rgb565_scale(int32_t src_width,
                   int32_t src_height,
                   int32_t dst_width,
                   int32_t dst_height,
                   uint8_t *src_buf,
                   uint8_t *dst_buf)
{
    return 0;
#if 0
    if(dst_width % 4) {
        printf("Error: rgb888 to 565, the width should be the multiple of 4!\n");
        return 0;
    }
    while(is_sync){
        // wait the prev sync finished!!
        ;
    }

    flush_cache((unsigned long)src_buf, (unsigned long)src_width * src_height * 3);

    invalidate_dcache_range((unsigned long)dst_buf,
                       (unsigned long)dst_buf + dst_width*dst_height*2);

    lcd_context lcd_ctx;

    lcd_ctx.layer_config_img.buf1 = (void *)src_buf;
    lcd_ctx.layer_config_img.format = LCD_FORMAT_RGB888_PACKED;
    lcd_ctx.layer_config_img.layer_enable = 1;
    lcd_ctx.layer_config_img.stride = src_width*3;
    lcd_ctx.layer_config_img.x = 0;
    lcd_ctx.layer_config_img.y = 0;
    lcd_ctx.layer_config_img.width = src_width;
    lcd_ctx.layer_config_img.height = src_height;
    lcd_ctx.layer_config_img.dst_width = dst_width;
    lcd_ctx.layer_config_img.dst_height = dst_height;

    lcd_ctx.layer_config_osd1.layer_enable = 0;

    lcd_ctx.wb_config.wb_enable = 1;
    lcd_ctx.wb_config.color_format = LCD_FORMAT_RGB565;
    lcd_ctx.wb_config.width = dst_width;
    lcd_ctx.wb_config.height = dst_height;
    lcd_ctx.wb_config.stride = dst_width*2;
    lcd_ctx.wb_config.wb_buf = dst_buf;

    lcd_ctx.output_config.width = dst_width;
    lcd_ctx.output_config.height = dst_height;

#if USE_CRANE_LCD_MCU
    lcd_mcu_before_refresh();
    hal_lcdc_sync(&lcd_ctx, 1);
    lcd_mcu_after_refresh();
#elif USE_CRANE_LCD_SPI
    lcd_spi_before_refresh();
    hal_lcdc_sync(&lcd_ctx, 1);
    lcd_spi_after_refresh();
#endif
    return 1;
#endif
}

#endif /* USE_CRANE_LCD */
