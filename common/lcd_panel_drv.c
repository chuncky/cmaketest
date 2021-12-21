#include "panel_drv.h"
#include "lcd_drv.h"
#include "lcd_reg.h"
#include "panel_list.h"

enum{
	PANEL_STATUS_POWEROFF,
	PANEL_STATUS_POWERON,
	PANEL_STATUS_LIMIT
};


int g_panel_status = PANEL_STATUS_POWEROFF;


#ifdef LCD_ADAPT_PANEL
extern struct panel_spec* panel_list[PANEL_MAX];
extern int list_num;
#endif


static void lcm_reset(void)
{
	lcd_set_bits(SMPN_CTRL, BIT_3);
	mdelay(20);
//	OSATaskSleep(MS_TO_TICKS(20));
	lcd_clear_bits(SMPN_CTRL, BIT_3);
	mdelay(120);
//	OSATaskSleep(MS_TO_TICKS(120));
}
#if 0
#ifndef LCD_ADAPT_PANEL
struct panel_spec* get_panel_info(int id)
{
	int panel_id;
	if((id >= 0) && (id < PANEL_MAX))
		panel_id = id;
	else
		panel_id = ST7789V_SPI_EVB; /*use ST7789V_SPI_EVB as default*/
	return panels[panel_id];
}
#endif
#endif
//***************************************************************
#ifndef LCD_ADAPT_PANEL
int panel_init(struct panel_spec* panel, uint32_t sclk, int32_t work_mode)
{
//#ifdef LCD_PRINT_INFO
#if 1
	unsigned int id = 0;
#endif
	LCDLOGD("INFO: panel_init +++\r\n");

	if(NULL == panel){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}
#if 1
	if(g_panel_status == PANEL_STATUS_POWERON){
		if(NULL != panel->ops->panel_interface_update){
			panel->ops->panel_interface_update(panel, work_mode);
		}
		return 0;
	}
#endif

	if(NULL != panel->ops->panel_interface_init){
		panel->ops->panel_interface_init(panel, sclk, work_mode);
	}

	if(LCD_CAP_FAKE != (panel->cap & LCD_CAP_FAKE)){
		lcm_reset();
//#ifdef LCD_PRINT_INFO
#if 1
		if(NULL != panel->ops->panel_readid){
			id = panel->ops->panel_readid(panel);
			LCDLOGI("INFO: panel_init: panel id is 0x%x\r\n", id);
		}
#endif
		if(NULL != panel->ops->panel_init){
			panel->ops->panel_init(panel);
		}
	}
	g_panel_status = PANEL_STATUS_POWERON;
	LCDLOGD("INFO: panel_init ---\r\n");
	return 0;
}
#else	/* LCD_ADAPT_PANEL */
static void dump_lcd_panel_spec(struct panel_spec* panel)
{
	LCDLOGI("dump_lcd_panel_spec +++\r\n ");
	LCDLOGI("name %s \r\n",panel->name);
	LCDLOGI("width %d \r\n",panel->width);
	LCDLOGI("height %d \r\n",panel->height);
	LCDLOGI("panel_id 0x%x \r\n",panel->panel_id);
	LCDLOGI("dump_lcd_panel_spec ---\r\n");
}

static struct panel_spec* adapt_panel_from_readid(uint32_t sclk, int32_t work_mode)
{
	int i = 0;
	int size = 0;
	unsigned int id = 0;
	int ret = 0;
	LCDLOGI("INFO: adapt_panel_from_readid lcd +++\r\n");

	size = list_num;
	for(i = 0; i < size; i++){
		dump_lcd_panel_spec(panel_list[i]);
		if ((NULL != panel_list[i]->ops->panel_interface_init) && (i == 0)){
			ret = panel_list[i]->ops->panel_interface_init(panel_list[i], sclk, work_mode); // for set clk
			if(ret != 0){
				LCDLOGE("ERROR: panel_interface_init return -1 i = %d\r\n",i);
				return NULL;
			}
		}

		if(i > 0)
			panel_list[i]->panel_if = panel_list[0]->panel_if;
		
		if(NULL != panel_list[i]->ops->panel_readid){
			id = panel_list[i]->ops->panel_readid(panel_list[i]);
		}
		LCDLOGI("INFO: adapt_panel_from_readid lcd size = %d read_id = 0x%x panels[i]->panel_id = 0x%x, i = %d\r\n",size,id, panel_list[i]->panel_id,i);
		if(id == panel_list[i]->panel_id)
		{
			LCDLOGW("WARNING:lcd panel 0x%x\r\n",id);
			return panel_list[i];
		}

		id = 0;
	}
	LCDLOGI("INFO: adapt_panel_from_readid lcd --- \r\n");
	return NULL;
}

int panel_init(struct panel_spec** ppanel, uint32_t sclk, int32_t work_mode)
{
	struct panel_spec* panel = NULL;

	LCDLOGI("Line:%d,INFO: panel_init +++\r\n",__LINE__);
#if 1
	if(g_panel_status == PANEL_STATUS_POWERON){
		panel = *ppanel;
		LCDLOGE("INFO: power on *ppanel = 0x%x panel = 0x%x\r\n",*ppanel,panel);
		if(NULL != panel->ops->panel_interface_update){
			panel->ops->panel_interface_update(panel, work_mode);
		}
		return 0;
	}
#endif

	lcm_reset();

	//WARNING: read panel id must operate after the "lcm_reset".
	panel = adapt_panel_from_readid(sclk, work_mode);
	if(panel == NULL)
		return -1;
	
	*ppanel = panel;
	LCDLOGI("INFO: *ppanel = 0x%x panel = 0x%x\r\n",*ppanel,panel);
	if(LCD_CAP_FAKE != (panel->cap & LCD_CAP_FAKE)){
		if(NULL != panel->ops->panel_init){
			panel->ops->panel_init(panel);
		}
	}


	g_panel_status = PANEL_STATUS_POWERON;
	LCDLOGI("INFO: panel_init ---\r\n");
	return 0;
}

#endif	/* LCD_ADAPT_PANEL */
//******************************************************
int panel_before_refresh(struct panel_spec* panel, uint32_t start_x,
						uint32_t start_y, uint32_t height, uint32_t width)
{
	int ret;
	LCDLOGD("INFO: panel_before_refresh +++\r\n");

	if(panel->ops->panel_invalid){
		panel->ops->panel_invalid(panel, start_x, start_y, start_x + width -1,
			start_y + height - 1);
	}
#if (defined TEST_MCU_PANEL)
	if(panel->type == LCD_MODE_MCU)
		ret = mcu_before_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
#endif		
		ret = spi_before_refresh((struct s_spi_ctx *)panel->panel_if);
	LCDLOGD("INFO: panel_before_refresh ---\r\n");
	return ret;
}

int panel_after_refresh(struct panel_spec* panel)
{
	int ret;
	LCDLOGD("INFO: panel_after_refresh +++\r\n");
#if (defined TEST_MCU_PANEL)
	if(panel->type == LCD_MODE_MCU)
		ret = mcu_after_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
#endif		
	ret = spi_after_refresh((struct s_spi_ctx *)panel->panel_if);
	LCDLOGD("INFO: panel_after_refresh ---\r\n");
	return ret;
}

int panel_sleep(struct panel_spec* panel)
{
	int ret = 0;
	LCDLOGD("INFO: panel_sleep +++\r\n");

	if(panel->ops->panel_suspend){
		ret = panel->ops->panel_suspend(panel);
	}
	return ret;
}

int panel_wakeup(struct panel_spec* panel)
{
	int ret = 0;
	LCDLOGD("INFO: panel_wakeup +++\r\n");

	if(panel->ops->panel_resume){
		ret = panel->ops->panel_resume(panel);
	}
	return ret;
}

int panel_reset(struct panel_spec* panel, uint32_t sclk, int32_t work_mode)
{
	if(NULL != panel->ops->panel_interface_init){
		panel->ops->panel_interface_init(panel, sclk, work_mode);
	}
	return 0;
}

void panel_uninit(struct panel_spec *panel)
{
	if(NULL == panel){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return;
	}

	lcm_reset();

	g_panel_status = PANEL_STATUS_POWEROFF;
}
