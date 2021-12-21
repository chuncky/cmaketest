#ifdef LCD_DUMMY_MCU

static struct panel_operations lcd_dummy_mcu_ops = {
	lcd_panel_mcu_init,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
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


struct panel_spec lcd_dummy_mcu_spec = {
	LCD_CAP_FAKE,
	240,
	320,
	LCD_INTERFACE_MCU,
	LCD_TYPE_RGB,
	&lcd_dummy_mcu_info,
	NULL,
	&lcd_dummy_mcu_ops,
};
#endif /*LCD_DUMMY_MCU*/