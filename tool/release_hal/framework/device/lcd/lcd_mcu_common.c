#include "../../chip/lcd/panel_drv.h"
#include "plat_config_defs.h"

int lcd_panel_mcu_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode)
{
	struct s_mcu_ctx *mcu_ctx = NULL;
	struct mcu_info *mcu = NULL;

	LCDLOGD("DBG: lcd_panel_mcu_init, mcu+++\r\n");
	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_mcu_init, Invalid param\r\n");
		return -1;
	}

#ifdef FPGA_TEST
	gpio_direction_output(HAL_GPIO_24); /*IM0 = 0 for mcu*/
	gpio_direction_output(HAL_GPIO_25); /*IM1 = 0 for mcu*/
	gpio_direction_output(HAL_GPIO_26); /*IM2 = 0 for mcu*/
	gpio_direction_output(HAL_GPIO_27); /*IM3 = 0 for mcu*/
	gpio_set_value(HAL_GPIO_24, 0);
	gpio_set_value(HAL_GPIO_24, 0);
	gpio_set_value(HAL_GPIO_24, 0);
	gpio_set_value(HAL_GPIO_24, 0);
#endif

	mcu = (struct mcu_info*)self->info;
	mcu_ctx = mcu_init(sclk, mcu, work_mode);
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: lcd_panel_mcu_init, mcu init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)mcu_ctx;
	LCDLOGD("DBG: lcd_panel_mcu_init, mcu---\r\n");
	return 0;
}

int lcd_panel_mcu_uninit(struct panel_spec *self)
{
	struct s_mcu_ctx *mcu_ctx = NULL;

	LCDLOGD("DBG: lcd_panel_mcu_uninit, mcu+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_mcu_uninit, Invalid param\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_uninit(mcu_ctx);
	self->panel_if = NULL;
	LCDLOGD("DBG: lcd_panel_mcu_uninit, mcu---\r\n");
	return 0;
}

int lcd_panel_mcu_reset(struct panel_spec *self)
{
	struct s_mcu_ctx *mcu_ctx = NULL;

	LCDLOGD("DBG: lcd_panel_mcu_reset, mcu+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_mcu_reset, Invalid param\r\n");
		return -1;
	}

	mcu_ctx = (struct s_mcu_ctx*)self->panel_if;
	mcu_reset(mcu_ctx);
	LCDLOGD("DBG: lcd_panel_mcu_reset, mcu---\r\n");
	return 0;
}

int lcd_panel_mcu_update(struct panel_spec *self, int32_t work_mode)
{
	int ret;

	LCDLOGD("DBG: lcd_panel_mcu_update, mcu+++\r\n");
	if(NULL == self || NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_mcu_update, Invalid param\r\n");
		return -1;
	}
	
	ret = mcu_update((struct s_mcu_ctx*)self->panel_if, work_mode);
	LCDLOGD("DBG: lcd_panel_mcu_update, mcu---\r\n");
	return ret;
}