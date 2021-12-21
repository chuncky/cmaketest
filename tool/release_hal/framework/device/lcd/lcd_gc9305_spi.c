#if (defined(LCD_GC9305_SPI_3WIRE_1LANE_1IF) || defined(LCD_GC9305_SPI_3WIRE_2LANE_1IF) || defined(LCD_GC9305_SPI_4WIRE_1LANE_1IF))
#include "../../chip/lcd/panel_drv.h"
#include "lcd_interface_common.h"
#include "Gpio_api.h"
#include <ui_os_api.h>

static int lcd_panel_interface_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode)
{
	struct s_spi_ctx *spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGD("DBG: gc9305 panel_interface_init, spi+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_interface_init, Invalid param\r\n");
		return -1;
	}

	spi = (struct spi_info *)self->info;
#ifdef LCD_FPGA_TEST
	gpio_direction_output(HAL_GPIO_24);
	gpio_direction_output(HAL_GPIO_25);
	gpio_direction_output(HAL_GPIO_26);
	gpio_direction_output(HAL_GPIO_27);
	if(spi->line_num == 3){
		if(spi->interface_id == 1){
			gpio_set_value(HAL_GPIO_24, 1); /*IM0 = 1 for spi 3-wire interfacd1*/
			gpio_set_value(HAL_GPIO_25, 0); /*IM1 = 0 for spi 3-wire interfacd1*/
			gpio_set_value(HAL_GPIO_26, 1); /*IM2 = 1 for spi 3-wire interfacd1*/
			gpio_set_value(HAL_GPIO_27, 0); /*IM3 = 0 for spi 3-wire interfacd1*/
		} else {
			gpio_set_value(HAL_GPIO_24, 1); /*IM0 = 1 for spi 3-wire interfacd2*/
			gpio_set_value(HAL_GPIO_25, 0); /*IM1 = 0 for spi 3-wire interfacd2*/
			gpio_set_value(HAL_GPIO_26, 1); /*IM2 = 1 for spi 3-wire interfacd2*/
			gpio_set_value(HAL_GPIO_27, 1); /*IM3 = 1 for spi 3-wire interfacd2*/
		}
	} else {
		if(spi->interface_id == 1){
			gpio_set_value(HAL_GPIO_24, 0); /*IM0 = 0 for spi 4-wire interfacd1*/
			gpio_set_value(HAL_GPIO_25, 1); /*IM1 = 1 for spi 4-wire interfacd1*/
			gpio_set_value(HAL_GPIO_26, 1); /*IM2 = 1 for spi 4-wire interfacd1*/
			gpio_set_value(HAL_GPIO_27, 0); /*IM3 = 0 for spi 4-wire interfacd2*/
		} else {
			gpio_set_value(HAL_GPIO_24, 0); /*IM0 = 0 for spi 4-wire interfacd2*/
			gpio_set_value(HAL_GPIO_25, 1); /*IM1 = 1 for spi 4-wire interfacd2*/
			gpio_set_value(HAL_GPIO_26, 1); /*IM2 =1 for spi 4-wire interfacd2*/
			gpio_set_value(HAL_GPIO_27, 1); /*IM3 = 1 for spi 4-wire interfacd2*/
		}		
	}
#endif

	spi_ctx = spi_init(sclk, spi, work_mode);
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: panel_interface_init, spi init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)spi_ctx;
	return 0;
}

/*__BOARD_CUSTOM_V__*/
#ifdef LCD_GC9305_SPI_4WIRE_1LANE_1IF
static int lcd_panel_init_4wire_1lane_1if(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;
	LCDLOGD("DBG: gc9305 lcd_panel_init weixinke\r\n");

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

	spi_write_cmd(spi_ctx, 0xfe, 8);
	spi_write_cmd(spi_ctx, 0xef, 8);
	spi_write_cmd(spi_ctx, 0x36, 8);
	spi_write_data(spi_ctx, 0x48, 8);
	spi_write_cmd(spi_ctx, 0x3a, 8);
	spi_write_data(spi_ctx, 0x05, 8);
	spi_write_cmd(spi_ctx, 0xa4, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_cmd(spi_ctx, 0xa5, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_cmd(spi_ctx, 0xaa, 8);
	spi_write_data(spi_ctx, 0x88, 8);
	spi_write_data(spi_ctx, 0x88, 8);
	spi_write_cmd(spi_ctx, 0xe8, 8);
	spi_write_data(spi_ctx, 0x21, 8);
	spi_write_data(spi_ctx, 0x0b, 8);
	spi_write_cmd(spi_ctx, 0xe3, 8);
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x18, 8);
	spi_write_cmd(spi_ctx, 0xe1, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_data(spi_ctx, 0x0a, 8);
	spi_write_cmd(spi_ctx, 0xAC, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_cmd(spi_ctx, 0xAf, 8);
	spi_write_data(spi_ctx, 0x67, 8);
	spi_write_cmd(spi_ctx, 0xa6, 8);
	spi_write_data(spi_ctx, 0x29, 8);
	spi_write_data(spi_ctx, 0x29, 8);
	spi_write_cmd(spi_ctx, 0xa7, 8);
	spi_write_data(spi_ctx, 0x27, 8);
	spi_write_data(spi_ctx, 0x27, 8);
	spi_write_cmd(spi_ctx, 0xa8, 8);
	spi_write_data(spi_ctx, 0x17, 8);
	spi_write_data(spi_ctx, 0x17, 8);
	spi_write_cmd(spi_ctx, 0xa9, 8);
	spi_write_data(spi_ctx, 0x26, 8);
	spi_write_data(spi_ctx, 0x26, 8);

	//----gamma setting---------//
	spi_write_cmd(spi_ctx, 0xf0, 8);
	spi_write_data(spi_ctx, 0x02, 8);
	spi_write_data(spi_ctx, 0x02, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x18, 8);
	spi_write_data(spi_ctx, 0x0D, 8);
	spi_write_cmd(spi_ctx, 0xf1, 8);
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x03, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x1C, 8);
	spi_write_data(spi_ctx, 0x22, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_cmd(spi_ctx, 0xf2, 8);
	spi_write_data(spi_ctx, 0x0D, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x39, 8);
	spi_write_data(spi_ctx, 0x04, 8);
	spi_write_data(spi_ctx, 0x04, 8);
	spi_write_data(spi_ctx, 0x4A, 8);
	spi_write_cmd(spi_ctx, 0xf3, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_data(spi_ctx, 0x03, 8);
	spi_write_data(spi_ctx, 0x05, 8);
	spi_write_data(spi_ctx, 0x4F, 8);
	spi_write_cmd(spi_ctx, 0xf4, 8);
	spi_write_data(spi_ctx, 0x08, 8);
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x12, 8);
	spi_write_data(spi_ctx, 0x22, 8);
	spi_write_data(spi_ctx, 0x29, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_cmd(spi_ctx, 0xf5, 8);
	spi_write_data(spi_ctx, 0x07, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_data(spi_ctx, 0x22, 8);
	spi_write_data(spi_ctx, 0x29, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	//---end gamma setting-----//
#if 0
	spi_write_cmd(spi_ctx, 0x11, 8);
	panel_delay(120); 
	spi_write_cmd(spi_ctx, 0x29, 8);
	spi_write_cmd(spi_ctx, 0x2c, 8);
#endif
	return 0;
}
#endif /*LCD_GC9305_SPI_4WIRE_1LANE_1IF*/

#if (defined(LCD_GC9305_SPI_3WIRE_1LANE_1IF) || defined(LCD_GC9305_SPI_3WIRE_2LANE_1IF))
static int lcd_panel_init(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGD("DBG: gc9305 lcd_panel_init\r\n");

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

	spi_write_cmd(spi_ctx, 0xFE, 8);
	spi_write_cmd(spi_ctx, 0xEF, 8);
	spi_write_cmd(spi_ctx, 0x3A, 8);
	if(spi->format == SPI_FORMAT_RGB666)
		spi_write_data(spi_ctx, 0x06, 8); //format is RGB666
	else
		spi_write_data(spi_ctx, 0x05, 8); //format is RGB565
	spi_write_cmd(spi_ctx, 0x36, 8);
	spi_write_data(spi_ctx, 0x48, 8);

	spi_write_cmd(spi_ctx, 0xA4, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_cmd(spi_ctx, 0xA5, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_data(spi_ctx, 0x42, 8);
	spi_write_cmd(spi_ctx, 0xAA, 8);
	spi_write_data(spi_ctx, 0x88, 8);
	spi_write_data(spi_ctx, 0x88, 8);

	spi_write_cmd(spi_ctx, 0xE8, 8);//frame rate is 71.8Hz
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x0B, 8);
//	spi_write_cmd(spi_ctx, 0xE8, 8);//frame rate is 20Hz
//	spi_write_data(spi_ctx, 0x1F, 8);
//	spi_write_data(spi_ctx, 0x40, 8);
	spi_write_cmd(spi_ctx, 0xE3, 8);//source ps=01
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_cmd(spi_ctx, 0xFF, 8);
	spi_write_data(spi_ctx, 0x61, 8);

	if(spi->data_lane_num == 2){
		spi_write_cmd(spi_ctx, 0xE9, 8);
		switch(spi->format){
		case SPI_FORMAT_RGB565:
			spi_write_data(spi_ctx, 0x08, 8); //RGB565
			break;
		case SPI_FORMAT_RGB666:
			spi_write_data(spi_ctx, 0x09, 8); //RGB666 1 pixel
			break;
		case SPI_FORMAT_RGB666_2_3:
			spi_write_data(spi_ctx, 0x0A, 8); //RGB666 2-3 pixel
			break;
		case SPI_FORMAT_RGB888:
			spi_write_data(spi_ctx, 0x0C, 8); //RGB888 1 pixel
			break;
		case SPI_FORMAT_RGB888_2_3:
			spi_write_data(spi_ctx, 0x0E, 8); //RGB888 2-3 pixel
			break;
		}
	}

	spi_write_cmd(spi_ctx, 0xAC, 8);//ldo enable
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_cmd(spi_ctx, 0xAD, 8);//DIG_VREFAD_VRDD[2]
	spi_write_data(spi_ctx, 0x33, 8);
	spi_write_cmd(spi_ctx, 0xAF, 8);//DIG_VREFAD_VRDD[2]
	spi_write_data(spi_ctx, 0x55, 8);

    /*GAMMA*/
	spi_write_cmd(spi_ctx, 0xF0, 8);
	spi_write_data(spi_ctx, 0x02, 8);
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x0E, 8);
	spi_write_data(spi_ctx, 0x14, 8);
	spi_write_cmd(spi_ctx, 0xF1, 8);
	spi_write_data(spi_ctx, 0x01, 8);
	spi_write_data(spi_ctx, 0x02, 8);
	spi_write_data(spi_ctx, 0x00, 8);
	spi_write_data(spi_ctx, 0x11, 8);
	spi_write_data(spi_ctx, 0x18, 8);
	spi_write_data(spi_ctx, 0x09, 8);
	spi_write_cmd(spi_ctx, 0xF2, 8);
	spi_write_data(spi_ctx, 0x13, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x3E, 8);
	spi_write_data(spi_ctx, 0x03, 8);
	spi_write_data(spi_ctx, 0x04, 8);
	spi_write_data(spi_ctx, 0x50, 8);
	spi_write_cmd(spi_ctx, 0xF3, 8);
	spi_write_data(spi_ctx, 0x0B, 8);
	spi_write_data(spi_ctx, 0x0A, 8);
	spi_write_data(spi_ctx, 0x37, 8);
	spi_write_data(spi_ctx, 0x06, 8);
	spi_write_data(spi_ctx, 0x04, 8);
	spi_write_data(spi_ctx, 0x44, 8);
	spi_write_cmd(spi_ctx, 0xF4, 8);
	spi_write_data(spi_ctx, 0x0B, 8);
	spi_write_data(spi_ctx, 0x19, 8);
	spi_write_data(spi_ctx, 0x16, 8);
	spi_write_data(spi_ctx, 0x24, 8);
	spi_write_data(spi_ctx, 0x24, 8);
	spi_write_data(spi_ctx, 0x0F, 8);
	spi_write_cmd(spi_ctx, 0xF5, 8);
	spi_write_data(spi_ctx, 0x0C, 8);
	spi_write_data(spi_ctx, 0x18, 8);
	spi_write_data(spi_ctx, 0x15, 8);
	spi_write_data(spi_ctx, 0x10, 8);
	spi_write_data(spi_ctx, 0x12, 8);
	spi_write_data(spi_ctx, 0x0F, 8);

	spi_write_cmd(spi_ctx, 0x35, 8);//enable TE

#if 0
	spi_write_cmd(spi_ctx, 0x11, 8);
	panel_delay(120);
	spi_write_cmd(spi_ctx, 0x29, 8);
	panel_delay(20);
#endif
	return 0;
}
#endif

static unsigned int lcd_panel_readid(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;
    uint32_t read_id = 0;

	if(NULL == self){
		LCDLOGE("ERROR: panel_readid, gc9305 Invalid param\r\n");
		return 0;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_readid, gc9305 spi has not been inited!\r\n");
		return 0;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_read_data(spi_ctx, 0xDA, 8, &read_id, 8);
	if(read_id != 0x0){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0xDA expect 0x0, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0xDA receive 0x%x!\r\n",read_id);

	spi_read_data(spi_ctx, 0xDB, 8, &read_id, 8);
	if(read_id != 0x93){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0xDB expect 0x93, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0xDB receive 0x%x!\r\n",read_id);

	spi_read_data(spi_ctx, 0xDC, 8, &read_id, 8);
	if(read_id != 0x05){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0xDC expect 0x05, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0xDC receive 0x%x!\r\n",read_id);
	
	spi_read_data(spi_ctx, 0x04, 8, &read_id, 24);
	if(read_id != 0x9305){
		LCDLOGE("ERROR: panel_readid, gc9305 read 0x04 expect 0x9305, but receive 0x%x!\r\n",
			read_id);
		return 0;
	}
	LCDLOGI("INFO: panel_readid, gc9305 read 0x04 receive 0x%x!\r\n",read_id);

#if 0
/*read status*/
	spi_read_data(spi_ctx, 0x09, 8, &read_id, 32);
	LCDLOGI("INFO: panel_readid, read 0x09 receive 0x%x!\r\n", read_id);
#endif
    return read_id;
}

static int lcd_panel_display_on(struct panel_spec *self)
{
	struct s_spi_ctx* spi_ctx = NULL;

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

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

	if(NULL == self){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

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
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(NULL == self->panel_if){
		LCDLOGE("ERROR: panel_init, mcu has not been inited!\r\n");
		return -1;
	}

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

#if (defined(LCD_GC9305_SPI_3WIRE_1LANE_1IF) || defined(LCD_GC9305_SPI_3WIRE_2LANE_1IF))
static struct panel_operations lcd_gc9305_spi_ops = {
	lcd_panel_interface_init,
	lcd_panel_spi_reset,
	lcd_panel_spi_uninit,
	lcd_panel_spi_update,
	lcd_panel_init,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
	NULL,
};
#endif /*LCD_GC9305_SPI_3WIRE_1LANE_1IF || LCD_GC9305_SPI_3WIRE_2LANE_1IF*/

#ifdef LCD_GC9305_SPI_4WIRE_1LANE_1IF
static struct panel_operations lcd_gc9305_spi_4wire_1lane_1if_ops = {
	lcd_panel_interface_init,
	lcd_panel_spi_reset,
	lcd_panel_spi_uninit,
	lcd_panel_spi_update,
	lcd_panel_init_4wire_1lane_1if,
	lcd_panel_invalid,
	lcd_panel_display_off,
	lcd_panel_display_on,
	lcd_panel_readid,
	NULL,
};
#endif /*LCD_GC9305_SPI_4WIRE_1LANE_1IF*/

static struct timing_spi lcd_gc9305_spi_timing = {
#ifdef LCD_FPGA_TEST
	5000, /*kHz*/
#else
	52000,
#endif
	5000, /*kHz*/
};

/*__BOARD_CUSTOM_B__*/
#ifdef LCD_GC9305_SPI_3WIRE_1LANE_1IF
static struct spi_info lcd_gc9305_spi_3wire_1lane_1if_info = {
	3,
	1, 
	1,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	&lcd_gc9305_spi_timing,
};

struct panel_spec lcd_gc9305_spi_3wire_1lane_1if_spec = {
	"gc9305_spi_3w_1l_1i",
	0x9305,
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
	&lcd_gc9305_spi_3wire_1lane_1if_info,
	NULL,
	&lcd_gc9305_spi_ops,
};
#endif /*LCD_GC9305_SPI_3WIRE_1LANE_1IF*/

#ifdef LCD_GC9305_SPI_3WIRE_2LANE_1IF
static struct spi_info lcd_gc9305_spi_3wire_2lane_1if_info = {
	3,
	1, 
	2,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	&lcd_gc9305_spi_timing,
};

struct panel_spec lcd_gc9305_spi_3wire_2lane_1if_spec = {
	"gc9305_spi_3w_2l_1i",
	0x9305,
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
	&lcd_gc9305_spi_3wire_2lane_1if_info,
	NULL,
	&lcd_gc9305_spi_ops,
};
#endif /*LCD_GC9305_SPI_3WIRE_2LANE_1IF*/

/*__BOARD_CUSTOM_V__*/
#ifdef LCD_GC9305_SPI_4WIRE_1LANE_1IF
static struct spi_info lcd_gc9305_spi_4wire_1lane_1if_info = {
	4,
	1, 
	1,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	&lcd_gc9305_spi_timing,
};

struct panel_spec lcd_gc9305_spi_4wire_1lane_1if_spec = {
	"gc9305_spi_4w_1l_1i",
	0x9305,
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
	&lcd_gc9305_spi_4wire_1lane_1if_info,
	NULL,
	&lcd_gc9305_spi_4wire_1lane_1if_ops,
};
#endif /*LCD_GC9305_SPI_4WIRE_1LANE_1IF*/

#endif /*LCD_GC9305_SPI_3WIRE_1LANE_1IF || LCD_GC9305_SPI_3WIRE_2LANE_1IF || LCD_GC9305_SPI_4WIRE_1LANE_1IF*/
