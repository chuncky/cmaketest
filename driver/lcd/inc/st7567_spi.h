#ifndef _ST7567V_SPI_H
#define _ST7567V_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_LCD_PANEL_ST7567_SPI_4WIRE_1LANE_1IF

#include "halsoc_lcdc.h"

extern struct crane_panel_t st7567_spi;

#endif /* USE_LCD_PANEL_ST7567_SPI_4WIRE_1LANE_1IF */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ST7567V_SPI_H */
