#ifdef LCD_GC9305_MCU
#include "../../chip/lcd/panel_drv.h"
#include "lcd_interface_common.h"
#include "Gpio_api.h"
#include <ui_os_api.h>

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode)
{
	struct s_mcu_ctx *mcu_ctx = NULL;
	struct mcu_info *mcu = NULL;

	LCDLOGD("DBG: lcd_panel_mcu_init, mcu+++\r\n");
	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_mcu_init, Invalid param\r\n");
		return -1;
	}

#ifdef FPGA_TEST
	gpio_direction_output(HAL_GPIO_24); /*IM0 = 0 for mcu*/
	gpio_direction_output(HAL_GPIO_25); /*IM1 = 0 for mcu*/
	gpio_direction_output(HAL_GPIO_26); /*IM2 = 0 for mcu*/
	gpio_direction_output(HAL_GPIO_27); /*IM3 = 0 for mcu*/
	gpio_set_value(HAL_GPIO_24, 0);
	gpio_set_value(HAL_GPIO_24, 0);
	gpio_set_value(HAL_GPIO_24, 0);
	gpio_set_value(HAL_GPIO_24, 0);
#endif

	mcu = (struct mcu_info*)self->info;
	mcu_ctx = mcu_init(sclk, mcu, work_mode);
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: lcd_panel_mcu_init, mcu init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)mcu_ctx;
	LCDLOGD("DBG: lcd_panel_mcu_init, mcu---\r\n");
	return 0;
}

static int lcd_panel_init(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;
	struct mcu_info *mcu = NULL;

	LCDLOGD("DBG: gc9305 panel_init+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu = (struct mcu_info*)self->info;

	mcu_write_cmd(mcu_ctx, 0xFE, 8);
	mcu_write_cmd(mcu_ctx, 0xEF, 8);
	if(mcu->format == MCU_FORMAT_RGB666)
		mcu_write_cmd_data(mcu_ctx, 0x3a, 8, 0x06, 8); //format is RGB666
	else
		mcu_write_cmd_data(mcu_ctx, 0x3A, 8, 0x05, 8); //format is RGB565
	mcu_write_cmd_data(mcu_ctx, 0x36, 8, 0x48, 8);

	mcu_write_cmd_data(mcu_ctx, 0xA4, 8, 0x44, 8);
	mcu_write_data(mcu_ctx, 0x44, 8);
	mcu_write_cmd_data(mcu_ctx, 0xA5, 8, 0x42, 8);
	mcu_write_data(mcu_ctx, 0x42, 8);
	mcu_write_cmd_data(mcu_ctx, 0xAA, 8, 0x88, 8);
	mcu_write_data(mcu_ctx, 0x88, 8);

	mcu_write_cmd_data(mcu_ctx, 0xe8, 8, 0x11, 8);//frame rate is 71.8Hz
	mcu_write_data(mcu_ctx, 0x0b, 8);
//	mcu_write_cmd_data(mcu_ctx, 0xE8, 8, 0x1F, 8);//frame rate is 20Hz
//	mcu_write_data(mcu_ctx, 0x40, 8);
	mcu_write_cmd_data(mcu_ctx, 0xE3, 8, 0x01, 8); //source ps=01
	mcu_write_data(mcu_ctx, 0x10, 8);
	mcu_write_cmd_data(mcu_ctx, 0xFF, 8, 0x61, 8);

	mcu_write_cmd_data(mcu_ctx, 0xAC, 8, 0x00, 8); //ldo enable
	mcu_write_cmd_data(mcu_ctx, 0xAD, 8, 0x33, 8); //DIG_VREFAD_VRDD[2]
	mcu_write_cmd_data(mcu_ctx, 0xAF, 8, 0x55, 8); //DIG_VREFAD_VRDD[2]

    /*GAMMA*/
	mcu_write_cmd_data(mcu_ctx, 0xF0, 8, 0x02, 8);
	mcu_write_data(mcu_ctx, 0x01, 8);
	mcu_write_data(mcu_ctx, 0x00, 8);
	mcu_write_data(mcu_ctx, 0x0A, 8);
	mcu_write_data(mcu_ctx, 0x0E, 8);
	mcu_write_data(mcu_ctx, 0x14, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF1, 8, 0x01, 8);
	mcu_write_data(mcu_ctx, 0x02, 8);
	mcu_write_data(mcu_ctx, 0x00, 8);
	mcu_write_data(mcu_ctx, 0x11, 8);
	mcu_write_data(mcu_ctx, 0x18, 8);
	mcu_write_data(mcu_ctx, 0x09, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF2, 8, 0x13, 8);
	mcu_write_data(mcu_ctx, 0x0A, 8);
	mcu_write_data(mcu_ctx, 0x3E, 8);
	mcu_write_data(mcu_ctx, 0x03, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x50, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF3, 8, 0x0B, 8);
	mcu_write_data(mcu_ctx, 0x0A, 8);
	mcu_write_data(mcu_ctx, 0x37, 8);
	mcu_write_data(mcu_ctx, 0x06, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x44, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF4, 8, 0x0B, 8);
	mcu_write_data(mcu_ctx, 0x19, 8);
	mcu_write_data(mcu_ctx, 0x16, 8);
	mcu_write_data(mcu_ctx, 0x24, 8);
	mcu_write_data(mcu_ctx, 0x24, 8);
	mcu_write_data(mcu_ctx, 0x0F, 8);
	mcu_write_cmd_data(mcu_ctx, 0xF5, 8, 0x0C, 8);
	mcu_write_data(mcu_ctx, 0x18, 8);
	mcu_write_data(mcu_ctx, 0x15, 8);
	mcu_write_data(mcu_ctx, 0x10, 8);
	mcu_write_data(mcu_ctx, 0x12, 8);
	mcu_write_data(mcu_ctx, 0x0F, 8);

	mcu_write_cmd(mcu_ctx, 0x35, 8); //enable TE

#if 0
	mcu_write_cmd(mcu_ctx, 0x11, 8);
	panel_delay(120);
	mcu_write_cmd(mcu_ctx, 0x29, 8);
	panel_delay(20);
#endif
	return 0;
}

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;
    uint32_t read_id = 0;

	if(NULL == self){
		LCDLOGE("ERROR: panel_readid, gc9305 Invalid param\r\n");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_readid, gc9305 mcu has not been inited!\r\n");
		return 0;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_read_data(mcu_ctx, 0xDA, 8, &read_id, 8);
	if(read_id != 0x0){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0xDA expect 0x0, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0xDA receive 0x%x!\r\n",read_id);
	

	mcu_read_data(mcu_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x93){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0xDB expect 0x93, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0xDB receive 0x%x!\r\n",read_id);

	mcu_read_data(mcu_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x05){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0xDC expect 0x05, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0xDC receive 0x%x!\r\n",read_id);

	mcu_read_data(mcu_ctx, 0x04, 8, &read_id, 24);
	if(read_id != 0x9305){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0x04 expect 0x9305, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0x04 receive 0x%x!\r\n",read_id);

/*read status
	mcu_read_data(mcu_ctx, 0x09, 8, &read_id, 32);
	LCDLOGI("INFO: panel_readid, read 0x09 receive 0x%x!\r\n", read_id);
*/

    return read_id;
}

static int lcd_panel_display_on(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_write_cmd(mcu_ctx, 0x11, 8);
	panel_delay(120);
	mcu_write_cmd(mcu_ctx, 0x29, 8);
	panel_delay(20);
    return 0;
}

static int lcd_panel_display_off(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_write_cmd(mcu_ctx, 0x28, 8);
	panel_delay(20);
	mcu_write_cmd(mcu_ctx, 0x10, 8);
	panel_delay(120);
    return 0;
}

static int lcd_panel_invalid(struct panel_spec *self, uint32_t start_x,
	uint32_t start_y, uint32_t end_x, uint32_t end_y)
{
	struct s_mcu_ctx* mcu_ctx = NULL;

	LCDLOGI("INFO: lcd_panel_invalid++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_invalid, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_invalid, mcu has not been inited!\r\n");
		return -1;
	}
	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_write_cmd_data(mcu_ctx, 0x2A, 8, ((start_x >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (start_x & 0xFF), 8);
	mcu_write_data(mcu_ctx, ((end_x >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (end_x & 0xFF), 8);

	mcu_write_cmd_data(mcu_ctx, 0x2B, 8, ((start_y >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (start_y & 0xFF), 8);
	mcu_write_data(mcu_ctx, ((end_y >> 8) & 0xFF), 8);
	mcu_write_data(mcu_ctx, (end_y & 0xFF), 8);

	mcu_write_cmd(mcu_ctx, 0x2C, 8);
	return 0;
}

static struct panel_operations lcd_gc9305_mcu_ops = {
	lcd_panel_interface_init,
	lcd_panel_mcu_reset,
	lcd_panel_mcu_uninit,
	lcd_panel_mcu_update,
	lcd_panel_init,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
	NULL,
};

static struct timing_mcu lcd_gc9305_mcu_timing = {
 	200,
	200,
 	40,
	40,
};

static struct mcu_info lcd_gc9305_mcu_info = {
	MCU_BUS_8080,
	MCU_FORMAT_RGB565,
	MCU_ENDIAN_LSB,
	0,
	0,
	0,
	&lcd_gc9305_mcu_timing,
};


struct panel_spec lcd_gc9305_mcu_spec = {
	"gc9305_mcu",
	0x9305,
#ifdef LCD_HAS_TE_SIGNAL
	LCD_CAP_NORMAL,
#else
	LCD_CAP_NOTE,
#endif
	240,
	320,
	LCD_INTERFACE_MCU,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	 &lcd_gc9305_mcu_info,
	NULL,
	&lcd_gc9305_mcu_ops,
};
#endif /*LCD_GC9305_MCU*/
