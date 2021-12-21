#ifndef _CRANE_LCD_MIPI_H_
#define _CRANE_LCD_MIPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_CRANE_LCD_MIPI

#include <stdint.h>
#include "halsoc_lcd_mipi_dsi.h"

int lcd_mipi_init(uint32_t sclk, int32_t work_mode, const struct s_mipi_info *mipi_info);
int lcd_mipi_uninit(void);
int lcd_mipi_reset(void);

#endif /* USE_CRANE_LCD_MIPI */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
