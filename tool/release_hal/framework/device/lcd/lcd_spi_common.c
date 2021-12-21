#include "../../chip/lcd/panel_drv.h"
#include "plat_config_defs.h"

int lcd_panel_spi_init(struct panel_spec *self, uint32_t sclk, int32_t work_mode)
{
	struct s_spi_ctx *spi_ctx = NULL;
	struct spi_info * spi = NULL;

	LCDLOGD("DBG: panel_interface_init, spi+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: panel_interface_init, Invalid param\r\n");
		return -1;
	}

	spi = (struct spi_info *)self->info;
	spi_ctx = spi_init(sclk, spi, work_mode);
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: panel_interface_init, spi init fail!\r\n");
		return -1;
	}
	self->panel_if = (void*)spi_ctx;
	LCDLOGD("DBG: panel_interface_init, spi---\r\n");
	return 0;
}

int lcd_panel_spi_uninit(struct panel_spec *self)
{
	struct s_spi_ctx *spi_ctx = NULL;

	LCDLOGD("DBG: lcd_panel_interface_uninit, spi+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_interface_uninit, Invalid param\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_uninit(spi_ctx);
	self->panel_if = NULL;
	LCDLOGD("DBG: lcd_panel_interface_uninit, spi---\r\n");
	return 0;
}

int lcd_panel_spi_reset(struct panel_spec *self)
{
	struct s_spi_ctx *spi_ctx = NULL;

	LCDLOGD("DBG: lcd_panel_interface_reset, spi+++\r\n");

	if(NULL == self){
		LCDLOGE("ERROR: lcd_panel_interface_reset, Invalid param\r\n");
		return -1;
	}

	spi_ctx = (struct s_spi_ctx*)self->panel_if;
	spi_reset(spi_ctx);
	LCDLOGD("DBG: lcd_panel_interface_reset, spi---\r\n");
	return 0;
}

int lcd_panel_spi_update(struct panel_spec *self, int32_t work_mode)
{
	int ret;

	LCDLOGD("DBG: lcd_panel_spi_update, spi+++\r\n");
	if(NULL == self || NULL == self->panel_if){
		LCDLOGE("ERROR: lcd_panel_interface_update, Invalid param\r\n");
		return -1;
	}
	
	ret = spi_update((struct s_spi_ctx *)self->panel_if, work_mode);
	LCDLOGD("DBG: lcd_panel_spi_update, spi+++\r\n");
	return ret;
}
