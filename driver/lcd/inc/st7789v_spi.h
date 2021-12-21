#ifndef _ST7789V_SPI_H
#define _ST7789V_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_LCD_PANEL_ST7789V_SPI_WATCH || USE_LCD_PANEL_ST7789V_SPI_PHONE

#include "halsoc_lcdc.h"

extern struct crane_panel_t st7789v_spi;

#endif /* USE_LCD_PANEL_ST7789V_SPI_WATCH || USE_LCD_PANEL_ST7789V_SPI_PHONE */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ST7789V_SPI_H */
