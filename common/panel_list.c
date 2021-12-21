//#include "plat_basic_api.h"
#include "panel_drv.h"
#include "panel_list.h"

extern struct panel_spec lcd_st7789v_spi_spec_evb;
extern struct panel_spec lcd_st7789v_spi_spec_phone;
extern struct panel_spec lcd_st7789v_spi_spec_watch;
extern struct panel_spec lcd_gc9306_spi_spec_evb;
extern struct panel_spec lcd_gc9304_spi_spec_evb;
#ifdef LCD_GC9306VER1
extern struct panel_spec lcd_gc9306_spi_spec;
#endif
#ifdef LCD_GC9306VER2
extern struct panel_spec lcd_gc9306_spi_ver2;
#endif
#ifdef LCD_GC9305VER1
extern struct panel_spec lcd_gc9305_spi_ver1;
#endif

#ifdef LCD_ST7789DKB_MCU_3WIRE_2LANE
extern struct panel_spec lcd_st7789v_mcu_spec;
#endif



struct panel_spec* panels[] = {
	&lcd_st7789v_spi_spec_evb,
	&lcd_st7789v_spi_spec_phone,
#if (defined WATCHLCDST7789_CODE_USE) || (defined LCD_ST7789VER2)
	&lcd_st7789v_spi_spec_watch,
#endif
#ifdef LCD_GC9306VER1
	&lcd_gc9306_spi_spec,
#endif	
#ifdef LCD_GC9306VER2
	&lcd_gc9306_spi_ver2,
#endif
#ifdef LCD_GC9305VER1
	&lcd_gc9305_spi_ver1,
#endif
#ifdef LCD_ST7789DKB_MCU_3WIRE_2LANE
	&lcd_st7789v_mcu_spec,
#endif
};

#ifdef LCD_ADAPT_PANEL
struct panel_spec* panel_list[PANEL_MAX];
int list_num = 0;

/*
***************************************************
* 1. support LCD_ADAPT (generally, defined macro "LCD_ADAPT_PANEL")
* 2. LCD_ADAPT: 
	ST7789_SPI:      3wire,2lane
	GC9306_SPI:	     3wire,2lane
	GC9304_SPI:      3wire,2lane	       
* 3. detail lcd config please refer to code.
***************************************************
*/
void get_panel_info_list(void)
{
	int i = 0;

	if (list_num != 0){
		return;
	}
	panel_list[i++] = &lcd_st7789v_spi_spec_evb;
	panel_list[i++] = &lcd_gc9306_spi_spec_evb;
	panel_list[i++] = &lcd_gc9304_spi_spec_evb;
	list_num = i;
}
#endif	/* LCD_ADAPT_PANEL*/


/*
***************************************************
* 1. Support LCD Drivers for the same model, different configuration(4wire/3wire, 1lane/2lane) 
* 2. NOTE: not define MACRO LCD_ADAPT_PANEL
***************************************************
*/
struct panel_spec* get_panel_info(void)
{
	PANEL_ID id;

#ifdef LCD_ADAPT_PANEL
	uart_printf("[logo][ERROR] func:%s,MACRO [LCD_ADAPT] use ERR!\r\n",__func__);
#endif

#if ( defined LCD_ST7789VER1 ) || ( defined ST7789_3WIRE_2LANE_SPI_LANDSCAPE )
	id = ST7789V_SPI_EVB;
#endif

#if (defined WATCHLCDST7789_CODE_USE) || (defined LCD_ST7789VER2)
	id = ST7789V_SPI_WATCH;
#endif	/* WATCHLCDST7789_CODE_USE */

#ifdef LCD_GC9306VER1
	id = GC9306_SPI_EVB_VER1;
#endif	/* LCD_GC9306VER1 */

#ifdef LCD_GC9306VER2
	id = GC9306_SPI_VER2;
#endif	/* LCD_GC9306VER2 */

#ifdef LCD_GC9305VER1
	id = GC9305_SPI_VER1;
#endif	/* LCD_GC9305VER1 */

#ifdef LCD_ST7789DKB_MCU_3WIRE_2LANE
	id = ST7789V_MCU_DKB;
#endif	/* LCD_ST7789DKB_MCU_3WIRE_2LANE */

	return panels[id];
}

