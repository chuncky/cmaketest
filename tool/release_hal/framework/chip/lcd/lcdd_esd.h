#ifndef _LCDD_ESD_H_
#define _LCDD_ESD_H_

#include "plat_types.h"
#include "lcd_predefine.h"
#include "../../chip/lcd/panel_drv.h"

enum {
	LCD_ESD_STATUS_STOP,
	LCD_ESD_STATUS_START
};

struct s_lcd_esd{
	struct s_lcd_ctx* lcd;
	struct panel_spec* panel;
	HANDLE esd_task;
	uint8_t esd_message;
	uint8_t esd_status;
	uint8_t esd_mutex;
	uint8_t esd_flag;
	volatile uint8_t esd_need_exit;
};

struct s_lcd_esd* lcd_esd_init(struct s_lcd_ctx* lcd, struct panel_spec* panel);
int lcd_esd_uninit(struct s_lcd_esd* esd_ctx);
int lcd_esd_start(struct s_lcd_esd* esd_ctx);
int lcd_esd_stop(struct s_lcd_esd* esd_ctx);

#endif /*_LCDD_ESD_H_*/
