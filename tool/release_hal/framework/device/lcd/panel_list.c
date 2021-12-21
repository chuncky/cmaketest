#include "plat_basic_api.h"
#include "panel_drv.h"
#include "panel_list.h"

#ifdef LCD_GC9106_SPI_4WIRE_1LANE_1IF
extern struct panel_spec lcd_gc9106_spi_4wire_1lane_1if_spec;
#endif

#ifdef LCD_GC9203_MCU
extern struct panel_spec lcd_gc9203_mcu_spec;
#endif

#ifdef LCD_GC9305_MCU
extern struct panel_spec lcd_gc9305_mcu_spec;
#endif

#ifdef LCD_GC9305_SPI_3WIRE_1LANE_1IF
extern struct panel_spec lcd_gc9305_spi_3wire_1lane_1if_spec;
#endif

#ifdef LCD_GC9305_SPI_3WIRE_2LANE_1IF
extern struct panel_spec lcd_gc9305_spi_3wire_2lane_1if_spec;
#endif

#ifdef LCD_GC9305_SPI_4WIRE_1LANE_1IF
extern struct panel_spec lcd_gc9305_spi_4wire_1lane_1if_spec;
#endif

#ifdef LCD_GC9306_SPI_3WIRE_1LANE_1IF
extern struct panel_spec lcd_gc9306_spi_3wire_1lane_1if_spec;
#endif

#ifdef LCD_GC9306_SPI_3WIRE_2LANE_1IF
extern struct panel_spec lcd_gc9306_spi_3wire_2lane_1if_spec;
#endif

#ifdef LCD_GC9306_SPI_4WIRE_1LANE_1IF
extern struct panel_spec lcd_gc9306_spi_4wire_1lane_1if_spec;
#endif

#ifdef LCD_ILI9340_SPI_3WIRE_2LANE_1IF
extern struct panel_spec lcd_ili9340_spi_3wire_2lane_1if_spec;
#endif

#ifdef LCD_ILI9340_SPI_4WIRE_1LANE_1IF
extern struct panel_spec lcd_ili9340_spi_4wire_1lane_1if_spec;
#endif

#ifdef LCD_ST7567_SPI_4WIRE_1LANE_1IF
extern struct panel_spec lcd_st7567_spi_4wire_1lane_1if_spec;
#endif

#ifdef LCD_ST7789V_MCU
extern struct panel_spec lcd_st7789v_mcu_spec;
#endif

#ifdef LCD_ST7789V_SPI_3WIRE_1LANE_1IF
extern struct panel_spec lcd_st7789v_spi_3wire_1lane_1if_spec;
#endif

#ifdef LCD_ST7789V_SPI_3WIRE_2LANE_1IF
extern struct panel_spec lcd_st7789v_spi_3wire_2lane_1if_spec;
#endif

#ifdef LCD_ST7789V_SPI_3WIRE_2LANE_1IF_SUB
extern struct panel_spec lcd_st7789v_spi_3wire_2lane_1if_sub_spec;
#endif

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_1IF
extern struct panel_spec lcd_st7789v_spi_4wire_1lane_1if_spec;
#endif

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_2IF
extern struct panel_spec lcd_st7789v_spi_4wire_1lane_2if_spec;
#endif

#ifdef LCD_DUMMY_MCU
extern struct panel_spec lcd_dummy_mcu_spec;
#endif

#ifdef LCD_DUMMY_SPI_3WIRE_2LANE_1IF
extern struct panel_spec lcd_dummy_spi_3wire_2lane_1if_spec;
#endif

static struct panel_spec* panels[] = {
#ifdef LCD_GC9203_MCU
	&lcd_gc9203_mcu_spec,
#endif

#ifdef LCD_GC9305_MCU
	&lcd_gc9305_mcu_spec,
#endif

#ifdef LCD_GC9305_SPI_3WIRE_1LANE_1IF
	&lcd_gc9305_spi_3wire_1lane_1if_spec,
#endif

#ifdef LCD_GC9305_SPI_3WIRE_2LANE_1IF
	&lcd_gc9305_spi_3wire_2lane_1if_spec,
#endif

#ifdef LCD_GC9305_SPI_4WIRE_1LANE_1IF
	&lcd_gc9305_spi_4wire_1lane_1if_spec,
#endif

#ifdef LCD_GC9306_SPI_3WIRE_1LANE_1IF
	&lcd_gc9306_spi_3wire_1lane_1if_spec,
#endif

#ifdef LCD_GC9306_SPI_3WIRE_2LANE_1IF
	&lcd_gc9306_spi_3wire_2lane_1if_spec,
#endif

#ifdef LCD_GC9306_SPI_4WIRE_1LANE_1IF
	&lcd_gc9306_spi_4wire_1lane_1if_spec,
#endif

#ifdef LCD_ILI9340_SPI_3WIRE_2LANE_1IF
	&lcd_ili9340_spi_3wire_2lane_1if_spec
#endif

#ifdef LCD_ILI9340_SPI_4WIRE_1LANE_1IF
	&lcd_ili9340_spi_4wire_1lane_1if_spec
#endif

#ifdef LCD_ST7567_SPI_4WIRE_1LANE_1IF
	&lcd_st7567_spi_4wire_1lane_1if_spec,
#endif

#ifdef LCD_ST7789V_MCU
	&lcd_st7789v_mcu_spec,
#endif

#ifdef LCD_ST7789V_SPI_3WIRE_1LANE_1IF
	&lcd_st7789v_spi_3wire_1lane_1if_spec,
#endif

#ifdef LCD_ST7789V_SPI_3WIRE_2LANE_1IF
	&lcd_st7789v_spi_3wire_2lane_1if_spec,
#endif

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_1IF
	&lcd_st7789v_spi_4wire_1lane_1if_spec,
#endif

#ifdef LCD_ST7789V_SPI_4WIRE_1LANE_2IF
	&lcd_st7789v_spi_4wire_1lane_2if_spec,
#endif

#ifdef LCD_DUMMY_MCU
	&lcd_dummy_mcu_spec,
#endif

#ifdef LCD_DUMMY_SPI_3WIRE_2LANE_1IF
	&lcd_dummy_spi_3wire_2lane_1if_spec,
#endif
};

#ifdef LCD_DUAL_PANEL_SUPPORT
static struct panel_spec* sub_panels[] = {
#ifdef LCD_GC9106_SPI_4WIRE_1LANE_1IF
	&lcd_gc9106_spi_4wire_1lane_1if_spec,
#endif

#ifdef LCD_ST7789V_SPI_3WIRE_2LANE_1IF_SUB
	&lcd_st7789v_spi_3wire_2lane_1if_sub_spec,
#endif
};
#endif /*LCD_DUAL_PANEL_SUPPORT*/

int get_panel_list(struct panel_spec*** plist)
{
	int count = 0;
	if(NULL == plist){
		LCDLOGE("ERROR: get_panel_list: Invalid param!\r\n");
		return -1;
	}
	count = sizeof(panels) / sizeof(struct panel_spec*);
	LCDLOGD("DBG: get_panel_list: total %d main panels\r\n", count);

	*plist = &panels[0];
	return count;
}

#ifdef LCD_DUAL_PANEL_SUPPORT
int get_sub_panel_list(struct panel_spec*** plist)
{
	int count = 0;
	if(NULL == plist){
		LCDLOGE("ERROR: get_sub_panel_list: Invalid param!\r\n");
		return -1;
	}

	count = sizeof(sub_panels) / sizeof(struct panel_spec*);
	LCDLOGD("DBG: get_panel_list: total %d sub panels\r\n", count);

	*plist = &sub_panels[0];
	return count;
}
#endif /*LCD_DUAL_PANEL_SUPPORT*/
