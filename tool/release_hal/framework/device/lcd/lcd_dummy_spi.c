#ifdef LCD_DUMMY_SPI_3WIRE_2LANE_1IF
#include "../../chip/lcd/panel_drv.h"
#include "lcd_interface_common.h"

#define LCD_DUMMY_SPI_ID 0xF8F8F8F8

static struct panel_operations lcd_dummy_spi_ops = {
	lcd_panel_spi_init,
	lcd_panel_spi_reset,
	lcd_panel_spi_uninit,
	lcd_panel_spi_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

static struct timing_spi lcd_dummy_spi_3wire_2lane_1if_timing = {
	5000, /*kHz*/
	5000, /*kHz*/
};

static struct spi_info lcd_dummy_spi_3wire_2lane_1if_info = {
	3,
	1, 
	2,
	SPI_FORMAT_RGB565,
	0,
	SPI_EDGE_RISING,
	0,
	SPI_ENDIAN_MSB,
	0,
	&lcd_dummy_spi_3wire_2lane_1if_timing,
};


struct panel_spec lcd_dummy_spi_3wire_2lane_1if_spec = {
	"dummy_spi",
	LCD_DUMMY_SPI_ID,
	LCD_CAP_FAKE,
	240,
	320,
	LCD_INTERFACE_SPI,
	LCD_TYPE_RGB,
	LCD_POLARITY_POS,
	&lcd_dummy_spi_3wire_2lane_1if_info,
	NULL,
	&lcd_dummy_spi_ops,
};
#endif /*LCD_DUMMY_SPI_3WIRE_2LANE_1IF*/
