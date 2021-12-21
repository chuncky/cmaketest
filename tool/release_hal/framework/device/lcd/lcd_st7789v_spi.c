#if (defined(LCD_ST7789V_SPI_3WIRE_1LANE_1IF) || defined(LCD_ST7789V_SPI_3WIRE_2LANE_1IF) || defined(LCD_ST7789V_SPI_4WIRE_1LANE_1IF) || defined(LCD_ST7789V_SPI_4WIRE_1LANE_2IF))
#include "../../chip/lcd/panel_drv.h"
#include "lcd_interface_common.h"
#include "plat_config_defs.h"
#include <ui_os_api.h>

#if (defined(LCD_ST7789V_SPI_3WIRE_1LANE_1IF) || defined(LCD_ST7789V_SPI_3WIRE_2LANE_1IF) || defined(LCD_ST7789V_SPI_4WIRE_1LANE_1IF))
static int lcd_panel_init(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGD("DBG: st7789v panel_init+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi = (struct spi_info *)self->info;

#if 0
	spi_write_cmd(spi_ctx, 0x11, 8);     
	panel_delay(120);
#endif

	spi_write_cmd(spi_ctx, 0x36, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0x3A, 8);     
	spi_write_data(spi_ctx, 0x06, 8);   
	
	spi_write_cmd(spi_ctx, 0x35, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0xC7, 8);     
	spi_write_data(spi_ctx, 0x00, 8);   
	
	spi_write_cmd(spi_ctx, 0xCC, 8);     
	spi_write_data(spi_ctx, 0x09, 8);   
	
	spi_write_cmd(spi_ctx, 0xB2, 8);     
	spi_write_data(spi_ctx, 0x0C, 8);   
	spi_write_data(spi_ctx, 0x0C, 8);   
	spi_write_data(spi_ctx, 0x00, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x33, 8);  

#if 1
	if(spi->format == SPI_FORMAT_RGB666){
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x06, 8); //format is RGB666
    }
	else{
        spi_write_cmd(spi_ctx, 0x3a, 8);
		spi_write_data(spi_ctx, 0x05, 8); //format is RGB565
    }

#ifdef LCD_ROTATION_FAKE
	spi_write_cmd(spi_ctx, 0x36, 8);
	spi_write_data(spi_ctx, 0xA0, 8);
#else
    spi_write_cmd(spi_ctx, 0x36, 8);
    spi_write_data(spi_ctx, 0x00, 8);
#endif
#endif

	spi_write_cmd(spi_ctx, 0xB7, 8);     
	spi_write_data(spi_ctx, 0x35, 8);   //vgh=13.26 vgl=-12.5
	
	spi_write_cmd(spi_ctx, 0xBB, 8);     
	spi_write_data(spi_ctx, 0x36, 8);   
	
	spi_write_cmd(spi_ctx, 0xC0, 8);     
	spi_write_data(spi_ctx, 0x2C, 8);   
	
	spi_write_cmd(spi_ctx, 0xC2, 8);     
	spi_write_data(spi_ctx, 0x01, 8);   
	
	spi_write_cmd(spi_ctx, 0xC3, 8);     
	spi_write_data(spi_ctx, 0x0D, 8);   
	
	spi_write_cmd(spi_ctx, 0xC4, 8);     
	spi_write_data(spi_ctx, 0x20, 8);   
	
	spi_write_cmd(spi_ctx, 0xC6, 8);     
	spi_write_data(spi_ctx, 0x15, 8);   //50hz
	
	spi_write_cmd(spi_ctx, 0xD0, 8);     
	spi_write_data(spi_ctx, 0xA4, 8);   
	spi_write_data(spi_ctx, 0xA1, 8);   
	
	spi_write_cmd(spi_ctx, 0xE0, 8);     
	spi_write_data(spi_ctx, 0xD0, 8);   
	spi_write_data(spi_ctx, 0x17, 8);   
	spi_write_data(spi_ctx, 0x19, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x32, 8);   
	spi_write_data(spi_ctx, 0x41, 8);   
	spi_write_data(spi_ctx, 0x43, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x12, 8);   
	spi_write_data(spi_ctx, 0x33, 8);   
	spi_write_data(spi_ctx, 0x2C, 8);   

	spi_write_cmd(spi_ctx, 0xE1, 8);     
	spi_write_data(spi_ctx, 0xD0, 8);   
	spi_write_data(spi_ctx, 0x18, 8);   
	spi_write_data(spi_ctx, 0x17, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x03, 8);   
	spi_write_data(spi_ctx, 0x04, 8);   
	spi_write_data(spi_ctx, 0x31, 8);   
	spi_write_data(spi_ctx, 0x46, 8);   
	spi_write_data(spi_ctx, 0x43, 8);   
	spi_write_data(spi_ctx, 0x09, 8);   
	spi_write_data(spi_ctx, 0x14, 8);   
	spi_write_data(spi_ctx, 0x13, 8);   
	spi_write_data(spi_ctx, 0x31, 8);   
	spi_write_data(spi_ctx, 0x2D, 8);   

	if(spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE7, 8);
        spi_write_data(spi_ctx, 0x11, 8);
	}
#if 0	
	spi_write_cmd(spi_ctx, 0x29, 8);
#endif
	return 0;
}
#endif /*LCD_ST7789V_SPI_3WIRE_1LANE_1IF || LCD_ST7789V_SPI_3WIRE_2LANE_1IF || LCD_ST7789V_SPI_4WIRE_1LANE_1IF*/

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_2IF
static int lcd_panel_init_4wire_1lane_2if(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGD("DBG: st7789v panel_init+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi = (struct spi_info *)self->info;

#if 0
	spi_write_cmd(spi_ctx, 0x11, 8);
	panel_delay(120);
#endif
	spi_write_cmd(spi_ctx, 0x36, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	
	spi_write_cmd(spi_ctx, 0x3A, 8);
	spi_write_data(spi_ctx, 0x05, 8);

	spi_write_cmd(spi_ctx, 0xB2, 8);
	spi_write_data(spi_ctx, 0x20, 8);
	spi_write_data(spi_ctx, 0x20, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x33, 8);
	spi_write_data(spi_ctx, 0x33, 8);

	spi_write_cmd(spi_ctx, 0xB7, 8);
	spi_write_data(spi_ctx, 0x71, 8);

	spi_write_cmd(spi_ctx, 0xBB, 8);
	spi_write_data(spi_ctx, 0x37, 8);

	spi_write_cmd(spi_ctx, 0xC0, 8);
	spi_write_data(spi_ctx, 0x2C, 8);

	spi_write_cmd(spi_ctx, 0xC2, 8);
	spi_write_data(spi_ctx, 0x01, 8);

	spi_write_cmd(spi_ctx, 0xC3, 8);
	spi_write_data(spi_ctx, 0x17, 8);

	spi_write_cmd(spi_ctx, 0xC4, 8);
	spi_write_data(spi_ctx, 0x20, 8);

	spi_write_cmd(spi_ctx, 0xC5, 8);
	spi_write_data(spi_ctx, 0x1F, 8);

	spi_write_cmd(spi_ctx, 0xC6, 8);
	spi_write_data(spi_ctx, 0x0F, 8);

	spi_write_cmd(spi_ctx, 0xD0, 8);
	spi_write_data(spi_ctx, 0xA4, 8);
	spi_write_data(spi_ctx, 0x81, 8);

	spi_write_cmd(spi_ctx, 0xE0, 8);
	spi_write_data(spi_ctx, 0xD0, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x12, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x39, 8);
	spi_write_data(spi_ctx, 0x54, 8);
	spi_write_data(spi_ctx, 0x4A, 8);
	spi_write_data(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, 0x17, 8);
	spi_write_data(spi_ctx, 0x16, 8);
	spi_write_data(spi_ctx, 0x1B, 8);
	spi_write_data(spi_ctx, 0x1D, 8);
	
	spi_write_cmd(spi_ctx, 0xE1, 8);
	spi_write_data(spi_ctx, 0xD0, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x39, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_data(spi_ctx, 0x4A, 8);
	spi_write_data(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, 0x17, 8);
	spi_write_data(spi_ctx, 0x16, 8);
	spi_write_data(spi_ctx, 0x1B, 8);
	spi_write_data(spi_ctx, 0x1E, 8);

	if(spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE7, 8);
        spi_write_data(spi_ctx, 0x11, 8);
	}

	spi_write_cmd(spi_ctx, 0x35, 8);
	spi_write_data(spi_ctx, 0x00, 8);
#if 0
	spi_write_cmd(spi_ctx, 0x29, 8);
	panel_delay(20);
#endif
	return 0;
}
#endif /*LCD_ST7789V_SPI_4WIRE_1LANE_2IF*/

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
    uint32_t read_id = 0;

	LCDLOGE("DBG:lcd_panel_readid +++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_readid, Invalid param\r\n");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_readid, spi has not been inited!\r\n");
		return 0;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
#if 1 
	//spi_set_cs(spi_ctx, 1);
	spi_read_data(spi_ctx, 0xDA, 8, &read_id, 8);
	/*if(read_id != 0x85){
		LCDLOGE("ERROR: panel_readid, read 0xDA expect 0x85, but receive 0x%x!\r\n",
			read_id);
		return -1;
	}*/
	
	LCDLOGI("INFO: panel_readid, st7789v read 0xDA receive 0x%x!\r\n",read_id);
	
	spi_read_data(spi_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x85){
//		LCDLOGE("ERROR: panel_readid, read 0xDB expect 0x85, but receive 0x%x!\r\n",
//			read_id);
//		return 0;
	}
	LCDLOGI("INFO: panel_readid, st7789v read 0xDB receive 0x%x!\r\n",read_id);

	spi_read_data(spi_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x52){
		LCDLOGE("ERROR: panel_readid, st7789v read 0xDC expect 0x52, but receive 0x%x!\r\n",
			read_id);
//		return 0;
	}
	LCDLOGI("INFO: panel_readid, st7789v read 0xDC receive 0x%x!\r\n",read_id);
#endif	
    //while(1){
	spi_read_data(spi_ctx, 0x04, 8, &read_id, 24);

	/*if(read_id != 0x858552){
		LCDLOGE("ERROR: panel_readid, read 0x04 expect 0x9305, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}*/
	LCDLOGI("INFO: panel_readid, st7789v read 0x04 receive 0x%x!\r\n",read_id);
    //}
/*read status
	//spi_set_cs(spi_ctx, 1);
	spi_read_data(spi_ctx, 0x09, 8, &read_id, 32);
	LCDLOGI("INFO: panel_readid, read 0x09 receive 0x%x!\r\n", read_id);
	//spi_set_cs(spi_ctx, 0);	
*/
	LCDLOGD("DBG:lcd_panel_readid ---\r\n");

    return read_id;
}

static int lcd_panel_display_on(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info* spi_info = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_display_on, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_display_on, mcu has not been inited!\r\n");
		return -1;
	}

	spi_info = (struct spi_info*)self->info;
	LCDLOGE("INFO: lcd_panel_display_on +++ (%d)\r\n", spi_info->device_id);

	/*
		note: modify 20ms delay to 120ms after sending command 0x29,
		in order to fix flashing white screen when call enter sleep then
		wakeup.
	*/

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x11, 8);
	panel_delay(120);
	spi_write_cmd(spi_ctx, 0x29, 8);
	panel_delay(120);
    return 0;
}

static int lcd_panel_display_off(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info* spi_info = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_display_off, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_display_off, mcu has not been inited!\r\n");
		return -1;
	}

	spi_info = (struct spi_info*)self->info;
	LCDLOGE("INFO: lcd_panel_display_off +++ (%d)\r\n", spi_info->device_id);

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x28, 8);
	panel_delay(20);
	spi_write_cmd(spi_ctx, 0x10, 8);
	panel_delay(120);
    return 0;
}

static int lcd_panel_invalid(struct panel_spec *self, uint32_t start_x,
	uint32_t start_y,  uint32_t end_x, uint32_t end_y)
{
	struct s_spi_ctx* spi_ctx = NULL;

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

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_write_cmd(spi_ctx, 0x2A, 8);
	spi_write_data(spi_ctx, ((start_x >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (start_x & 0xFF), 8);
	spi_write_data(spi_ctx, ((end_x >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (end_x & 0xFF), 8);

	spi_write_cmd(spi_ctx, 0x2B, 8);
	spi_write_data(spi_ctx, ((start_y >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (start_y & 0xFF), 8);
	spi_write_data(spi_ctx, ((end_y >> 8) & 0xFF), 8);
	spi_write_data(spi_ctx, (end_y & 0xFF), 8);

	spi_write_cmd(spi_ctx, 0x2C, 8);
	return 0;
}

static int lcd_panel_checkesd(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	uint32_t power_status = 0;

	LCDLOGD("DBG:lcd_panel_checkesd +++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_checkesd, Invalid param\r\n");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_checkesd, spi has not been inited!\r\n");
		return 0;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;

	spi_read_data(spi_ctx, 0x0A, 8, &power_status, 8);
	LCDLOGI("INFO: panel_checkesd, read 0x%X receive 0x%x!\r\n",0xA, power_status);
	LCDLOGD("DBG:lcd_panel_checkesd ---\r\n");
	if(power_status != 0x9C)
		return -1;
	else
		return 0;
}


#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_2IF
const static struct panel_operations lcd_st7789v_spi_4wire_1lane_2if_ops = {
	lcd_panel_spi_init,
	lcd_panel_spi_reset,
	lcd_panel_spi_uninit,
	lcd_panel_spi_update,
	lcd_panel_init_4wire_1lane_2if,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
	lcd_panel_checkesd,
};
#endif

#if (defined(LCD_ST7789V_SPI_3WIRE_1LANE_1IF) || defined(LCD_ST7789V_SPI_3WIRE_2LANE_1IF) || defined(LCD_ST7789V_SPI_4WIRE_1LANE_1IF))
const static struct panel_operations lcd_st7789v_spi_ops = {
	lcd_panel_spi_init,
	lcd_panel_spi_reset,
	lcd_panel_spi_uninit,
	lcd_panel_spi_update,
	lcd_panel_init,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
	lcd_panel_checkesd,
};
#endif

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_1IF
const static struct timing_spi lcd_st7789v_spi_4wire_timing = {
	52000,//62500, //31200, //26000, // /*kHz*/
	2500, /*kHz*/
};
#endif /*LCD_ST7789V_SPI_4WIRE_1LANE_1IF*/

#if (defined(LCD_ST7789V_SPI_3WIRE_1LANE_1IF) || defined(LCD_ST7789V_SPI_3WIRE_2LANE_1IF) || defined(LCD_ST7789V_SPI_4WIRE_1LANE_2IF))
const static struct timing_spi lcd_st7789v_spi_timing = {
	52000,//62500, //31200, //26000, // /*kHz*/
	5000, /*kHz*/
};
#endif /*LCD_ST7789V_SPI_3WIRE_1LANE_1IF || LCD_ST7789V_SPI_3WIRE_2LANE_1IF || LCD_ST7789V_SPI_4WIRE_1LANE_2IF*/

#ifdef LCD_ST7789V_SPI_3WIRE_1LANE_1IF
const static struct spi_info lcd_st7789v_spi_3wire_1lane_1if_info = {
	3,
	1, 
	1,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_timing,
};

struct panel_spec lcd_st7789v_spi_3wire_1lane_1if_spec = {
	"st7789v_spi_3w_1l_1i",
	0x858552,
#ifdef LCD_HAS_TE_SIGNAL
	LCD_CAP_NORMAL,
#else
	LCD_CAP_NOTE,
#endif
	240,
	320,
	LCD_INTERFACE_SPI,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_3wire_1lane_1if_info,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};
#endif /*LCD_ST7789V_SPI_3WIRE_1LANE_1IF*/

#ifdef LCD_ST7789V_SPI_3WIRE_2LANE_1IF
const static struct spi_info lcd_st7789v_spi_3wire_2lane_1if_info = {
	3,
	1, 
	2,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_timing,
};

struct panel_spec lcd_st7789v_spi_3wire_2lane_1if_spec = {
	"st7789v_spi_3w_2l_1i",
	0x858552,
#ifdef LCD_HAS_TE_SIGNAL
	LCD_CAP_NORMAL,
#else
	LCD_CAP_NOTE,
#endif
#ifdef LCD_ROTATION_FAKE
	320,
	240,
#else
	240,
	320,
#endif
	LCD_INTERFACE_SPI,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_3wire_2lane_1if_info,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};
#endif /*LCD_ST7789V_SPI_3WIRE_2LANE_1IF*/

#ifdef LCD_ST7789V_SPI_3WIRE_2LANE_1IF_SUB
const static struct spi_info lcd_st7789v_spi_3wire_2lane_1if_sub_info = {
	3,
	1, 
	2,
	SPI_FORMAT_RGB565,
	1,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_timing,
};

struct panel_spec lcd_st7789v_spi_3wire_2lane_1if_sub_spec = {
	"st7789v_spi_3w_2l_1i_sub",
	0x858552,
#ifdef LCD_HAS_TE_SIGNAL
	LCD_CAP_NORMAL,
#else
	LCD_CAP_NOTE,
#endif
#ifdef LCD_ROTATION_FAKE
	320,
	240,
#else
	240,
	320,
#endif
	LCD_INTERFACE_SPI,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_3wire_2lane_1if_sub_info,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};
#endif /*LCD_ST7789V_SPI_3WIRE_2LANE_1IF_SUB*/


#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_1IF
const static struct spi_info lcd_st7789v_spi_4wire_1lane_1if_info = {
	4,
	1, 
	1,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_4wire_timing,
};

struct panel_spec lcd_st7789v_spi_4wire_1lane_1if_spec = {
	"st7789v_spi_4w_1l_1i",
	0x858552,
#ifdef LCD_HAS_TE_SIGNAL
	LCD_CAP_NORMAL,
#else
	LCD_CAP_NOTE,
#endif
	240,
	320,
	LCD_INTERFACE_SPI,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_4wire_1lane_1if_info,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_ops,
};
#endif /*LCD_ST7789V_SPI_4WIRE_1LANE_1IF*/

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_2IF
const static struct spi_info lcd_st7789v_spi_4wire_1lane_2if_info = {
	4,
	2, 
	1,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	(struct timing_spi*)&lcd_st7789v_spi_timing,
};

struct panel_spec lcd_st7789v_spi_4wire_1lane_2if_spec = {
	"st7789v_spi_4w_1l_2i",
	0x858552,
#ifdef LCD_HAS_TE_SIGNAL
	LCD_CAP_NORMAL,
#else
	LCD_CAP_NOTE,
#endif
	240,
	320,
	LCD_INTERFACE_SPI,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	(void*)&lcd_st7789v_spi_4wire_1lane_2if_info,
	NULL,
	(struct panel_operations*)&lcd_st7789v_spi_4wire_1lane_2if_ops,
};
#endif /*LCD_ST7789V_SPI_4WIRE_1LANE_2IF*/
#endif /*LCD_ST7789V_SPI_3WIRE_1LANE_1IF || LCD_ST7789V_SPI_3WIRE_2LANE_1IF || LCD_ST7789V_SPI_4WIRE_1LANE_1IF|| LCD_ST7789V_SPI_4WIRE_1LANE_2IF*/
