#ifndef _ST7796S_MIPI_H
#define _ST7796S_MIPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_LCD_PANEL_ST7796S_MIPI

#include "halsoc_lcdc.h"

extern struct crane_panel_t st7796s_mipi;

#endif /* USE_LCD_PANEL_ST7796S_MIPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ST7796S_MIPI_H */
