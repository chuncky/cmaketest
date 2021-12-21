#ifndef _PANEL_DRV_H_
#define _PANEL_DRV_H_

#include "lcd_predefine.h"
#include "spi_drv.h"
#include "mcu_drv.h"

enum{
	LCD_CAP_NORMAL = 0,
	LCD_CAP_FAKE = 1, /*virtal panel*/
	LCD_CAP_POWERON = 2,
	LCD_CAP_NOTE = 4, /*has no te signal*/
	LCD_CAP_LIMIT
};

enum{
	LCD_INTERFACE_MCU = 0,
	LCD_INTERFACE_SPI = 1,
	LCD_MODE_LIMIT
};

enum{
	LCD_TYPE_RGB = 0,
	LCD_TYPE_FSTN = 1,
	LCD_TYPE_LIMIT
};

enum{
	LCD_POLARITY_POS = 0,
	LCD_POLARITY_NEG,
	LCD_POLARITY_LIMIT
};

enum{
	LCD_ID_MAIN = 0,
	LCD_ID_SUB,
	LCD_ID_LIMIT
};

enum{
	PANEL_MODE_NORMAL,
	PANEL_MODE_ASSERT,
	PANEL_MODE_LIMIT
};

#ifdef LCD_DUAL_PANEL_SUPPORT
#define MAX_PANEL_COUNT 2
#else
#define MAX_PANEL_COUNT 1
#endif

struct panel_spec;

/* LCD operations */
struct panel_operations {
	int (*panel_interface_init)(struct panel_spec *self, uint32_t sclk, int32_t work_mode);
	int (*panel_interface_reset)(struct panel_spec *self);
	int (*panel_interface_uninit)(struct panel_spec *self);
	int (*panel_interface_update)(struct panel_spec *self, int32_t work_mode);
	int (*panel_init)(struct panel_spec *self);
	int (*panel_invalid)(struct panel_spec *self, uint32_t start_x,
						uint32_t start_y,  uint32_t end_x, uint32_t end_y);
	int (*panel_suspend)(struct panel_spec *self);
	int (*panel_resume)(struct panel_spec *self);
	unsigned int (*panel_readid)(struct panel_spec *self);
	int (*panel_checkesd)(struct panel_spec *self);
};

/* LCD abstraction */
struct panel_spec {
	char name[32];
	unsigned int panel_id;
	unsigned int cap;
	unsigned short width;  //short
	unsigned short height; // short
	unsigned short interf; /*mcu, spi*/ //short
	unsigned short type; /*rgb, fstn*/
	uint16_t te_pol;             //uint16_t
	void *info;
	void *panel_if;
	struct panel_operations *ops;
};

struct panel_spec* find_panel(uint32_t sclk, int32_t work_mode, int panel_is_ready, int lcd_id);
int panel_init(struct panel_spec* panel, uint32_t sclk, int32_t work_mode,int panel_is_ready, int lcd_id);
int panel_before_refresh(struct panel_spec* panel, uint32_t start_x,
						uint32_t start_y, uint32_t height, uint32_t width);
int panel_after_refresh(struct panel_spec* panel);
int panel_before_wb(struct panel_spec* panel);
int panel_sleep(struct panel_spec* panel);
int panel_wakeup(struct panel_spec* panel);
int panel_reset(struct panel_spec* panel);
void panel_uninit(struct panel_spec *panel, int lcd_id);
void panel_delay(int ms);
int panel_esd_reset(struct panel_spec* panel);


#endif /*_PANEL_DRV_H_*/
