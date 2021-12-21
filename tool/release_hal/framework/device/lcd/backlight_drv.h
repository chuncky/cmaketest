#ifndef _BACKLIGHT_DRV_H_
#define _BACKLIGHT_DRV_H_

#include "lcd_predefine.h"

enum{
    BACKLIGHT_STATUS_OFF,
    BACKLIGHT_STATUS_ON,
    BACKLIGHT_STATUS_MAX
};

extern uint8_t g_backlight_status;
extern uint8_t g_backlight_brightness;

void backlight_set_brightness(uint8_t brightness);
#ifdef LCD_DUAL_PANEL_SUPPORT
void sub_backlight_set_brightness(uint8_t brightness);
#endif
#endif /*_BACKLIGHT_DRV_H_*/
