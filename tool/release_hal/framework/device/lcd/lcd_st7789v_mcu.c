#ifdef LCD_ST7789V_MCU
#include "../../chip/lcd/panel_drv.h"
#include "lcd_interface_common.h"
#include <ui_os_api.h>

static int lcd_panel_init(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;
	struct mcu_info *mcu = NULL;

	LCDLOGD("DBG: st7789v panel_init+++\r\n");

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

	mcu_write_cmd(mcu_ctx, 0x11, 8);  
	panel_delay(120);
	
	mcu_write_cmd_data(mcu_ctx, 0x36, 8, 0x00, 8); //0x00
	
	mcu_write_cmd_data(mcu_ctx, 0x3A, 8, 0x06, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0x35, 8, 0x00, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xC7, 8, 0x00, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xCC, 8, 0x09, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xB2, 8, 0x0C, 8);
	mcu_write_data(mcu_ctx, 0x0C, 8);
	mcu_write_data(mcu_ctx, 0x00, 8);
	mcu_write_data(mcu_ctx, 0x33, 8);
	mcu_write_data(mcu_ctx, 0x33, 8);
#if 1
	if(mcu->format == MCU_FORMAT_RGB666)
		mcu_write_cmd_data(mcu_ctx, 0x3a, 8, 0x06, 8); //format is RGB666
	else
		mcu_write_cmd_data(mcu_ctx, 0x3A, 8, 0x05, 8); //format is RGB565
	mcu_write_cmd_data(mcu_ctx, 0x36, 8, 0x00, 8); // 0x00:te and none te;
#endif
	
	mcu_write_cmd_data(mcu_ctx, 0xB7, 8, 0x35, 8); //vgh=13.26 vgl=-12.5
	
	mcu_write_cmd_data(mcu_ctx, 0xBB, 8, 0x36, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xC0, 8, 0x2C, 8);  
	
	mcu_write_cmd_data(mcu_ctx, 0xC2, 8, 0x01, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xC3, 8, 0x0D, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xC4, 8, 0x20, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xC6, 8, 0x0F, 8);   //60hz
	
	mcu_write_cmd_data(mcu_ctx, 0xD0, 8, 0xA4, 8); 
	mcu_write_data(mcu_ctx, 0xA1, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xE0, 8, 0xD0, 8);
	mcu_write_data(mcu_ctx, 0x17, 8);
	mcu_write_data(mcu_ctx, 0x19, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x03, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x32, 8);
	mcu_write_data(mcu_ctx, 0x41, 8);
	mcu_write_data(mcu_ctx, 0x43, 8);
	mcu_write_data(mcu_ctx, 0x09, 8);
	mcu_write_data(mcu_ctx, 0x14, 8);
	mcu_write_data(mcu_ctx, 0x12, 8);
	mcu_write_data(mcu_ctx, 0x33, 8);
	mcu_write_data(mcu_ctx, 0x2C, 8);
	
	mcu_write_cmd_data(mcu_ctx, 0xE1, 8, 0xD0, 8);
	mcu_write_data(mcu_ctx, 0x18, 8);
	mcu_write_data(mcu_ctx, 0x17, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x03, 8);
	mcu_write_data(mcu_ctx, 0x04, 8);
	mcu_write_data(mcu_ctx, 0x31, 8);
	mcu_write_data(mcu_ctx, 0x46, 8);
	mcu_write_data(mcu_ctx, 0x43, 8);
	mcu_write_data(mcu_ctx, 0x09, 8);
	mcu_write_data(mcu_ctx, 0x14, 8);
	mcu_write_data(mcu_ctx, 0x13, 8);
	mcu_write_data(mcu_ctx, 0x31, 8);
	mcu_write_data(mcu_ctx, 0x2D, 8);
	
	mcu_write_cmd(mcu_ctx, 0x29, 8); 

	return 0;
}

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_mcu_ctx* mcu_ctx = NULL;
    uint32_t read_id = 0;

	if(NULL == self){
		LCDLOGE("ERROR: panel_readid, st7789v Invalid param\r\n");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_readid, st7789v mcu has not been inited!\r\n");
		return 0;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_read_data(mcu_ctx, 0xDA, 8, &read_id, 8);
	if(read_id != 0x05){
		LCDLOGE("ERROR: panel_readid, st7789v read 0xDA expect 0x05, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, st7789v read 0xDA receive 0x%x!\r\n",read_id);
	

	mcu_read_data(mcu_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x85){
		LCDLOGE("ERROR: panel_readid, st7789v read 0xDB expect 0x85, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, st7789v read 0xDB receive 0x%x!\r\n",read_id);

	mcu_read_data(mcu_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x52){
		LCDLOGE("ERROR: panel_readid, st7789v read 0xDC expect 0x52, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, st7789v read 0xDC receive 0x%x!\r\n",read_id);

	mcu_read_data(mcu_ctx, 0x04, 8, &read_id, 24);
	if(read_id != 0x858552){
		LCDLOGE("ERROR: panel_readid, st7789v read 0x04 expect 0x858552, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, st7789v read 0x04 receive 0x%x!\r\n",read_id);

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

	LCDLOGI("INFO: lcd_panel_invalid, %d,%d,%d,%d\r\n", start_x, start_y,
		end_x, end_y);

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

static struct panel_operations lcd_st7789v_mcu_ops = {
	lcd_panel_mcu_init,
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

static struct timing_mcu lcd_st7789v_mcu_timing = {
 	200,
	200,
 	15,
	15,
};

static struct mcu_info lcd_st7789v_mcu_info = {
	MCU_BUS_8080,
	MCU_FORMAT_RGB565,
	MCU_ENDIAN_LSB,
	0,
	0,
	0,
	&lcd_st7789v_mcu_timing,
};


struct panel_spec lcd_st7789v_mcu_spec = {
	"st7789v_mcu",
	0x858552,
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
	&lcd_st7789v_mcu_info,
	NULL,
	&lcd_st7789v_mcu_ops,
};
#endif /*LCD_ST7789V_MCU*/
