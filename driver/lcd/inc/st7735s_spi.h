#ifndef _ST7735S_SPI_H
#define _ST7735S_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if (USE_LCD_PANEL_ST7735S_SPI_POC && (!LCD_GENERIC_SPI))

#include "halsoc_lcdc.h"

extern struct crane_panel_t st7735s_spi;

#endif /* USE_LCD_PANEL_ST7735S_SPI_POC */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ST7735S_SPI_H */
