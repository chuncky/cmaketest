#include "backlight_drv.h"
#include "Gpio_api.h"
#include "pmic_api.h"
#include "gpio_cus.h"

#define BACKLIGHT_GPIO HAL_GPIO_69

uint8_t g_backlight_status = BACKLIGHT_STATUS_MAX ;
uint8_t g_backlight_brightness = 0xFF;

#ifdef LCD_DUAL_PANEL_SUPPORT
uint8_t g_subbacklight_status = BACKLIGHT_STATUS_MAX ;
uint8_t g_subbacklight_brightness = 0xFF;
#endif

static void backlight_on(uint8_t brightness)
{
    /*
    * z2 use pm813, check first
    */
    if(Pmic_is_pm813()){
        PmicLcdBackLightCtrl(brightness);
    }
    else if (Pmic_is_pm812()){
        if(BACKLIGHT_STATUS_OFF == brightness){
            gpio_direction_output(BACKLIGHT_GPIO);
            gpio_set_value(BACKLIGHT_GPIO, 0);
        } else {
            gpio_direction_output(BACKLIGHT_GPIO);
            gpio_set_value(BACKLIGHT_GPIO, 1);
        }
    }
    else if (Pmic_is_pm802()){
		if(BACKLIGHT_STATUS_OFF == brightness){
            Evb1601_Pm802Backlightonoff(0);
			raw_uart_log("####backlight_on(%d)\r\n", brightness);
        } else {
            Evb1601_Pm802Backlightonoff(1);
			raw_uart_log("####backlight_on(%d)\r\n", brightness);
        }
    }
}

#ifdef LCD_DUAL_PANEL_SUPPORT
static void sub_backlight_on(uint8_t brightness)
{
	/*TODO: Need use custom's sub panel's backlight power on code*/
#ifdef CONFIG_BOARD_CUSTOM_B_V02
    if(brightness != 0){
		gpio_direction_output(HAL_GPIO_76);
		gpio_set_value(HAL_GPIO_76, 1);
    } else {
		gpio_direction_output(HAL_GPIO_76);
		gpio_set_value(HAL_GPIO_76, 0);
    }
#else
	backlight_on(brightness);
#endif
}
#endif

void backlight_set_brightness(uint8_t brightness)
{
    uint8_t orig_status = g_backlight_status;
    uint8_t orig_light = g_backlight_brightness;

    if(brightness > 5)
        brightness = 5;

    if(0 == brightness){
        if(BACKLIGHT_STATUS_OFF != g_backlight_status){
            backlight_on(brightness);
            g_backlight_status = BACKLIGHT_STATUS_OFF;
            /*
            * do not save brightness value when OFF
            */
        }
    } else {
        if(BACKLIGHT_STATUS_OFF == g_backlight_status){
            g_backlight_brightness = brightness;
            backlight_on(brightness);
            g_backlight_status = BACKLIGHT_STATUS_ON;
        } else {
            if(brightness != g_backlight_brightness){
                backlight_on(brightness);
                g_backlight_brightness = brightness;
                g_backlight_status = BACKLIGHT_STATUS_ON;
            }
        }
    }

    raw_uart_log("backlight_set_brightness(%d), [%d,%d]->[%d,%d]\r\n", 
            brightness, orig_status, orig_light, g_backlight_status, g_backlight_brightness);

}

#ifdef LCD_DUAL_PANEL_SUPPORT
void sub_backlight_set_brightness(uint8_t brightness)
{
    uint8_t orig_status = g_subbacklight_status;
    uint8_t orig_light = g_subbacklight_brightness;

    if(brightness > 5)
        brightness = 5;

    if(0 == brightness){
        if(BACKLIGHT_STATUS_OFF != g_subbacklight_status){
            sub_backlight_on(brightness);
            g_subbacklight_status = BACKLIGHT_STATUS_OFF;
            /*
            * do not save brightness value when OFF
            */
        }
    } else {
        if(BACKLIGHT_STATUS_OFF == g_subbacklight_status){
            g_subbacklight_brightness = brightness;
            sub_backlight_on(brightness);
            g_subbacklight_status = BACKLIGHT_STATUS_ON;
        } else {
            if(brightness != g_subbacklight_brightness){
                sub_backlight_on(brightness);
                g_subbacklight_brightness = brightness;
                g_subbacklight_status = BACKLIGHT_STATUS_ON;
            }
        }
    }

    raw_uart_log("sub_backlight_set_brightness(%d), [%d,%d]->[%d,%d]\r\n", 
            brightness, orig_status, orig_light, g_subbacklight_status, g_subbacklight_brightness);

}
#endif
