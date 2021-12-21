#ifndef _PANEL_DRV_H_
#define _PANEL_DRV_H_

#include "lcd_predefine.h"
#include "spi_drv.h"
#include "mcu_drv.h"


enum{
	LCD_CAP_NORMAL = 0,
	LCD_CAP_FAKE = 1,
	LCD_CAP_POWERON = 2,
	LCD_CAP_NOTE = 4,
	LCD_CAP_LIMIT
};

enum{
	LCD_MODE_MCU = 0,
	LCD_MODE_SPI = 1,
	LCD_MODE_LIMIT
};

enum{
	LCD_POLARITY_POS = 0,
	LCD_POLARITY_NEG,
	LCD_POLARITY_LIMIT
};

struct panel_spec;

/* LCD operations */
struct panel_operations {
	int (*panel_interface_init)(struct panel_spec *self, uint32_t sclk, int32_t work_mode);
	int (*panel_interface_update)(struct panel_spec *self, int32_t work_mode);
	int (*panel_init)(struct panel_spec *self);
	int (*panel_invalid)(struct panel_spec *self, uint32_t start_x,
						uint32_t start_y,  uint32_t end_x, uint32_t end_y);
	int (*panel_suspend)(struct panel_spec *self);
	int (*panel_resume)(struct panel_spec *self);
	unsigned int (*panel_readid)(struct panel_spec *self);
};

/* LCD abstraction */
struct panel_spec {
	char name[16];
	unsigned int panel_id;
	unsigned int cap;
	unsigned short width;  //short
	unsigned short height; // short
	unsigned short type; /*mcu, spi*/ //short
	uint16_t te_pol;             //uint16_t
	void *info;
	void *panel_if;
	struct panel_operations *ops;
};

#ifndef LCD_ADAPT_PANEL
int panel_init(struct panel_spec* panel, uint32_t sclk, int32_t work_mode);
#else
int panel_init(struct panel_spec** panel, uint32_t sclk, int32_t work_mode);
#endif

#if 0
struct panel_spec* get_panel_info(int id);
#endif
int panel_before_refresh(struct panel_spec* panel, uint32_t start_x,
						uint32_t start_y, uint32_t height, uint32_t width);
int panel_after_refresh(struct panel_spec* panel);
int panel_sleep(struct panel_spec* panel);
int panel_wakeup(struct panel_spec* panel);
int panel_reset(struct panel_spec* panel, uint32_t sclk, int32_t work_mode);
void panel_uninit(struct panel_spec *panel);


#endif /*_PANEL_DRV_H_*/
