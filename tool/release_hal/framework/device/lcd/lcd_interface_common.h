#ifndef _LCD_INTERFACE_COMMON_H_
#define _LCD_INTERFACE_COMMON_H_


int lcd_panel_mcu_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode);
int lcd_panel_mcu_uninit(struct panel_spec *self);
int lcd_panel_mcu_reset(struct panel_spec *self);
int lcd_panel_mcu_update(struct panel_spec *self, int32_t work_mode);

int lcd_panel_spi_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode);
int lcd_panel_spi_uninit(struct panel_spec *self);
int lcd_panel_spi_reset(struct panel_spec *self);
int lcd_panel_spi_update(struct panel_spec *self, int32_t work_mode);




#endif
