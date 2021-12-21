#ifdef LCD_DUMMY_MCU#include "../../chip/lcd/panel_drv.h"#include "lcd_interface_common.h"#define LCD_DUMMY_MCU_ID 0xF7F7F7F7

static struct panel_operations lcd_dummy_mcu_ops = {
	lcd_panel_mcu_init,	lcd_panel_mcu_reset,	lcd_panel_mcu_uninit,	lcd_panel_mcu_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,	NULL,
};

static struct timing_mcu lcd_dummy_mcu_timing = {
 	200,
	200,
 	15,
	15,
};

static struct mcu_info lcd_dummy_mcu_info = {
	MCU_BUS_8080,
	MCU_FORMAT_RGB565,
	MCU_ENDIAN_MSB,
	0,
	0,
	0,
	&lcd_dummy_mcu_timing,
};


struct panel_spec lcd_dummy_mcu_spec = {	"dummy_mcu",	LCD_DUMMY_MCU_ID,
	LCD_CAP_FAKE,
	240,
	320,
	LCD_INTERFACE_MCU,
	LCD_TYPE_RGB,	LCD_POLARITY_POS,
	&lcd_dummy_mcu_info,
	NULL,
	&lcd_dummy_mcu_ops,
};
#endif /*LCD_DUMMY_MCU*/