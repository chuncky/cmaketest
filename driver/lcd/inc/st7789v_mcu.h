#ifndef _ST7789V_MCU_H
#define _ST7789V_MCU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_drv_conf.h"

#if USE_LCD_PANEL_ST7789V_MCU

#include "halsoc_lcdc.h"

extern struct crane_panel_t st7789v_mcu;

#endif /* USE_LCD_PANEL_ST7789V_MCU */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _ST7789V_MCU_H */
