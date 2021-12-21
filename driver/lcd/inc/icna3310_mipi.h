#ifndef _ICNA3310_MIPI_H
#define _ICNA3310_MIPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_LCD_PANEL_ICNA3310_MIPI

#include "halsoc_lcdc.h"

extern struct crane_panel_t icna3310_mipi;

#endif /* USE_LCD_PANEL_ICNA3310_MIPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ICNA3310_MIPI_H */
