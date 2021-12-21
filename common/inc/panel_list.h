#ifndef _PANEL_LIST_H_
#define _PANEL_LIST_H_

typedef enum{
	ST7789V_SPI_EVB = 0,
	ST7789V_SPI_PHONE,	
#if	(defined WATCHLCDST7789_CODE_USE) || (defined LCD_ST7789VER2)
	ST7789V_SPI_WATCH,
#endif
#ifdef LCD_GC9306VER1
	GC9306_SPI_EVB_VER1,
#endif
#ifdef LCD_GC9306VER2
	GC9306_SPI_VER2,
#endif
#ifdef LCD_GC9305VER1
	GC9305_SPI_VER1,
#endif
#ifdef LCD_ST7789DKB_MCU_3WIRE_2LANE
	ST7789V_MCU_DKB,
#endif


#if defined LCD_ADAPT_PANEL
	PANEL_MAX = 20,
#endif
}PANEL_ID;

struct panel_spec* get_panel_info(void);
void get_panel_info_list(void);

#endif /*_PANEL_LIST_H_*/
