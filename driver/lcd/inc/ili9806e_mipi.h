#ifndef _ILI9806E_MIPI_H
#define _ILI9806E_MIPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_LCD_PANEL_ILI9806E_MIPI

#include "halsoc_lcdc.h"

extern struct crane_panel_t ili9806e_mipi;

#endif /* USE_LCD_PANEL_ILI9806E_MIPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ILI9806E_MIPI_H */
