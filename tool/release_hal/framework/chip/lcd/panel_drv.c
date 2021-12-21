#include "panel_drv.h"
#include "lcd_drv.h"
#include "lcd_reg.h"
#include "Gpio_api.h"
#include "ui_os_timer.h"
#include "ui_os_task.h"
#include "gpio_cus.h"
#include "pmic_api.h"
#include "../../device/lcd/panel_list.h"

extern struct s_lcd_ctx *g_plcd;

enum{
	PANEL_STATUS_POWEROFF,
	PANEL_STATUS_POWERON,
	PANEL_STATUS_LIMIT
};

enum{
	LCM_INTERFACE_SPI_3W_1L_1IF,
	LCM_INTERFACE_SPI_3W_2L_1IF,
	LCM_INTERFACE_SPI_3W_1L_2IF,
	LCM_INTERFACE_SPI_3W_2L_2IF,
	LCM_INTERFACE_SPI_4W_1L_1IF,
	LCM_INTERFACE_SPI_4W_1L_2IF,
	LCM_INTERFACE_MCU,
	LCM_INTERFACE_LIMIT
};

enum{
	LCM_POWER_OFF,
	LCM_POWER_ON,
	LCM_POWER_LIMIT
};

int g_panel_status[MAX_PANEL_COUNT] = {
	PANEL_STATUS_POWEROFF,
#ifdef LCD_DUAL_PANEL_SUPPORT
	PANEL_STATUS_POWEROFF,
#endif
};
int g_panel_mode = PANEL_MODE_NORMAL;

static void lcm_reset(void)
{
	lcd_set_bits(SMPN_CTRL, BIT_3);
	panel_delay(20);
	lcd_clear_bits(SMPN_CTRL, BIT_3);
	panel_delay(20);
}

static void lcm_poweron(int poweron)
{
	u32 val;

	val = UOS_GetUpTime();

	if(poweron == LCM_POWER_ON){
	    if (Pmic_is_pm812())
	    {
		    PM812_Ldo_8_set_2_8();
		    PM812_Ldo_8_set(1);
	    }
	    else
	    {
	        Ningbo_Ldo_1_set_2_8();
	        Ningbo_Ldo_1_set(1);
		}
	} else {
	    if (Pmic_is_pm812())
	    {
		    PM812_Ldo_8_set(0);
	    }
	    else
	    {
	        Ningbo_Ldo_1_set(0);
		}
	}

	val = UOS_GetUpTime() - val;
	if (val >= 10)
	{
		raw_uart_log("pmic_debug:  %s cost %d ms\n", __FUNCTION__, val);
	}
}

#ifdef LCD_DUAL_PANEL_SUPPORT
static void sub_lcm_poweron(int poweron)
{
	//sub lcd has the same power with main lcd.
	//so do nothing
}
#endif

static void lcm_pin_cfg(struct panel_spec* panel)
{
	struct spi_info	*spi = NULL;

	if(panel->interf == LCD_INTERFACE_SPI){
		spi = (struct spi_info*)panel->info;
		if(spi->line_num == 3){
			if(spi->data_lane_num == 1){
				if(spi->interface_id == 1){
					LCD_Pinmuxcfg(LCM_INTERFACE_SPI_3W_1L_1IF);
				} else{
					LCD_Pinmuxcfg(LCM_INTERFACE_SPI_3W_1L_2IF);
				}
			} else{
				if(spi->interface_id == 1){
					LCD_Pinmuxcfg(LCM_INTERFACE_SPI_3W_2L_1IF);
				} else{
					LCD_Pinmuxcfg(LCM_INTERFACE_SPI_3W_2L_2IF);
				}
			}
		} else {
			if(spi->interface_id == 1){
				LCD_Pinmuxcfg(LCM_INTERFACE_SPI_4W_1L_1IF);
			} else{
				LCD_Pinmuxcfg(LCM_INTERFACE_SPI_4W_1L_2IF);
			}
		}
	} else {
		LCD_Pinmuxcfg(LCM_INTERFACE_MCU);
	}
}

int panel_init(struct panel_spec* panel, uint32_t sclk, int32_t work_mode, int panel_is_ready, int lcd_id)
{
#ifdef LCD_LOG_LEVEL_INFO
	unsigned int id = 0;
#endif
	extern struct s_lcd_ctx *g_plcd;
	int* p_panel_status;

	LCDLOGD("INFO: panel_init (ready: %d) +++\r\n", panel_is_ready);

	if(NULL == panel){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return -1;
	}

	if(lcd_id >= MAX_PANEL_COUNT){
		LCDLOGE("ERROR: panel_init, Invalid lcd_id (%d)\r\n", lcd_id);
		return -1;
	}

	if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		g_panel_mode = PANEL_MODE_ASSERT;
	else
		g_panel_mode = PANEL_MODE_NORMAL;

	p_panel_status = &g_panel_status[lcd_id];
#if 1
	if(*p_panel_status == PANEL_STATUS_POWERON){
		if(NULL != panel->ops->panel_interface_update){
			panel->ops->panel_interface_update(panel, work_mode);
		}
		return 0;
	}
#endif

	/*for fpga only*/
#ifdef LCD_FPGA_TEST
	if(panel->interf == LCD_INTERFACE_MCU){
		gpio_direction_output(HAL_GPIO_21);
		gpio_set_value(HAL_GPIO_21, 0);
	} else {
		gpio_direction_output(HAL_GPIO_21);
		gpio_set_value(HAL_GPIO_21, 1);
	}
#endif

	if(NULL != panel->ops->panel_interface_init){
		panel->ops->panel_interface_init(panel, sclk, work_mode);
	}

	if((panel_is_ready == 1) && (LCD_ID_MAIN == lcd_id)){
		LCDLOGI("INFO: do not panel init panel_is_ready = %d \r\n",panel_is_ready);
	}else{
		LCDLOGI("INFO: do panel init panel_is_ready = %d \r\n",panel_is_ready);

		if(LCD_CAP_FAKE != (panel->cap & LCD_CAP_FAKE)){
			if(LCD_ID_MAIN == lcd_id){
				lcm_poweron(LCM_POWER_ON);
#ifdef LCD_DUAL_PANEL_SUPPORT
			} else {
				sub_lcm_poweron(LCM_POWER_ON);
#endif
			}
			lcm_pin_cfg(panel);
			if(LCD_ID_MAIN == lcd_id)
				lcm_reset();
		
		if(NULL != panel->ops->panel_init){
				panel->ops->panel_init(panel);
			}
		}
	}

	*p_panel_status = PANEL_STATUS_POWERON;
	LCDLOGD("INFO: panel_init ---\r\n");
	return 0;
}

static void dump_lcd_panel_spec(struct panel_spec* panel)
{
	LCDLOGD("dump_lcd_panel_spec +++\r\n ");
	LCDLOGD("name %s \r\n",panel->name);
	LCDLOGD("width %d \r\n",panel->width);
	LCDLOGD("height %d \r\n",panel->height);
	LCDLOGD("panel_id 0x%x \r\n",panel->panel_id);
	LCDLOGD("dump_lcd_panel_spec ---\r\n");
}

struct panel_spec* find_panel(uint32_t sclk, int32_t work_mode, int panel_is_ready, int lcd_id)
{
	struct panel_spec** panel_list = NULL;
	int panel_num;
	int i, ret;
	int panel_id = 0;

	LCDLOGE("DBG: find_panel (%d)(%d)(%d)++\r\n", work_mode, panel_is_ready, lcd_id);

	if(lcd_id >= MAX_PANEL_COUNT){
		LCDLOGE("ERROR: find_panel, Invalid lcd_id (%d)\r\n", lcd_id);
		return NULL;
	}

	if(LCD_ID_MAIN == lcd_id){
		panel_num = get_panel_list(&panel_list);
#ifdef LCD_DUAL_PANEL_SUPPORT
	} else {
		panel_num = get_sub_panel_list(&panel_list);
#endif
	}

	if(panel_num <= 0){
		LCDLOGE("ERROR: find_panel:panel list is NULL\r\n");
		return NULL;
	}

	if(0 == panel_is_ready){
		if(LCD_ID_MAIN == lcd_id){
			lcm_poweron(LCM_POWER_ON);
#ifdef LCD_DUAL_PANEL_SUPPORT
		} else {
			sub_lcm_poweron(LCM_POWER_ON);
#endif
		}
	}

	//step1: find non_fake panel
	for(i = 0; i < panel_num; i++){
		dump_lcd_panel_spec(panel_list[i]);

		if(LCD_CAP_FAKE == (panel_list[i]->cap & LCD_CAP_FAKE)){
			continue;
		}

		if (NULL != panel_list[i]->ops->panel_interface_init){
			ret = panel_list[i]->ops->panel_interface_init(panel_list[i], sclk, work_mode); // for set clk
			if(ret != 0){
				LCDLOGE("ERROR: find_panel: lcd (%d) interface init fail\r\n",i);
				return NULL;
			}
		}

#ifdef LCD_FPGA_TEST
		if(panel_list[i]->interf == LCD_INTERFACE_MCU){
			gpio_direction_output(HAL_GPIO_21);
			gpio_set_value(HAL_GPIO_21, 0);
		} else {
			gpio_direction_output(HAL_GPIO_21);
			gpio_set_value(HAL_GPIO_21, 1);
		}
#endif

		lcm_pin_cfg(panel_list[i]);
		panel_id = 0;
		if(NULL != panel_list[i]->ops->panel_readid){
			panel_id = panel_list[i]->ops->panel_readid(panel_list[i]);
		}
		LCDLOGE("INFO: find_panel: %d of %d panel: read_id = 0x%x, panels[i]->panel_id = 0x%x\r\n",i, panel_num, panel_id, panel_list[i]->panel_id);

		/*work around for crane_g HW issue*/
		if(CHIP_IS_CRANEG){
			lcd_hw_reset(work_mode);
			lcd_reset(g_plcd);
//				panel_reset(panel_list[i], LCD_SCLK_FREQ, work_mode);
		}

		if (NULL != panel_list[i]->ops->panel_interface_uninit){
			ret = panel_list[i]->ops->panel_interface_uninit(panel_list[i]);
			if(ret != 0){
				LCDLOGE("ERROR: find_panel: lcd (%d) interface un fail\r\n",i);
				return NULL;
			}
		}

		if(panel_id == panel_list[i]->panel_id)
		{
			LCDLOGE("INFO: find_panel: The panel is %s, id = 0x%x \r\n",panel_list[i]->name, panel_id);
			return panel_list[i];
		}
	}

	/*no real panel, power off the lcm*/
	if(0 == panel_is_ready){
		if(LCD_ID_MAIN == lcd_id){
			lcm_poweron(LCM_POWER_OFF);
#ifdef LCD_DUAL_PANEL_SUPPORT
		} else {
			sub_lcm_poweron(LCM_POWER_OFF);
#endif
		}
	}

	//step2: find fake panel 
	for(i = 0; i < panel_num; i++){
		if(LCD_CAP_FAKE == (panel_list[i]->cap & LCD_CAP_FAKE)){
			LCDLOGE("INFO: find_panel: Select the dummy panel %s, id = 0x%x \r\n",panel_list[i]->name, panel_id);
			return panel_list[i];
		}
		
	}

	LCDLOGE("INFO: find_panel: Not find any panel!\r\n");
	LCDLOGE("DBG: find_panel ---\r\n");
	return NULL;
}

int panel_before_refresh(struct panel_spec* panel, uint32_t start_x,
						uint32_t start_y, uint32_t height, uint32_t width)
{
	int ret;
	LCDLOGD("INFO: panel_before_refresh +++\r\n");

	if(panel->ops->panel_invalid){
		panel->ops->panel_invalid(panel, start_x, start_y, start_x + width -1,
			start_y + height - 1);
	}

	if(panel->interf == LCD_INTERFACE_MCU)
		ret = mcu_before_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
		ret = spi_before_refresh((struct s_spi_ctx *)panel->panel_if);
	LCDLOGD("INFO: panel_before_refresh ---\r\n");
	return ret;
}

int panel_before_wb(struct panel_spec* panel)
{
	int ret;
	LCDLOGD("INFO: panel_before_wb +++\r\n");

	if(panel->interf == LCD_INTERFACE_MCU)
		ret = mcu_before_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
		ret = spi_before_refresh((struct s_spi_ctx *)panel->panel_if);
	LCDLOGD("INFO: panel_before_wb ---\r\n");
	return ret;
}

int panel_after_refresh(struct panel_spec* panel)
{
	int ret;
	LCDLOGD("INFO: panel_after_refresh +++\r\n");

	if(panel->interf == LCD_INTERFACE_MCU)
		ret = mcu_after_refresh((struct s_mcu_ctx *)panel->panel_if);
	else
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

int panel_reset(struct panel_spec* panel)
{
	if(NULL != panel->ops->panel_interface_reset){
		panel->ops->panel_interface_reset(panel);
	}
	return 0;
}

int panel_esd_reset(struct panel_spec* panel)
{
	LCDLOGD("INFO: panel_esd_reset +++\r\n");
	lcm_reset();

	if(NULL != panel->ops->panel_init){
		panel->ops->panel_init(panel);
	}

	LCDLOGD("INFO: panel_esd_reset ---\r\n");
	return 0;
}

void panel_uninit(struct panel_spec *panel, int lcd_id)
{
	int* p_panel_status;

	if(NULL == panel){
		LCDLOGE("ERROR: panel_init, Invalid param\r\n");
		return;
	}

	if(lcd_id >= MAX_PANEL_COUNT){
		LCDLOGE("ERROR: panel_uninit, Invalid lcd_id (%d)\r\n", lcd_id);
		return;
	}

	p_panel_status = &g_panel_status[lcd_id];

	lcm_reset();
	*p_panel_status = PANEL_STATUS_POWEROFF;
}

void panel_delay(int ms)
{
	if(PANEL_MODE_ASSERT == g_panel_mode)
		mdelay(ms);
	else
		UOS_Sleep(MS_TO_TICKS(ms));
}
