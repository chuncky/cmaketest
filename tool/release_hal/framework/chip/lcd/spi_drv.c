#include "lcd_reg.h"
#include "spi_drv.h"
#include "Gpio_api.h"
#include "lcd_drv.h"
#ifdef LCD_INTERRUPT_MODE
#include "lcd_common.h"
#include <ui_os_api.h>
#endif

#ifdef LCD_DUAL_PANEL_SUPPORT
#define MAX_SPI_REF_COUNT 2
#else
#define MAX_SPI_REF_COUNT 1
#endif

struct s_spi_base_ctx g_spi_base_ctx;
struct s_spi_ctx g_spi_ctx[MAX_SPI_REF_COUNT];
int g_spi_ref_cnt = 0;

#ifdef LCD_INTERRUPT_MODE
extern u8 g_lcd_interrupt_flag;
extern int g_lcd_irq_waiter;
#endif

#ifdef LCD_DCX_GPIO_21
#define SPI_4LINE_DCX HAL_GPIO_21
#else
#define SPI_4LINE_DCX HAL_GPIO_27
#endif

static void spi_enable(void)
{
	LCDLOGD("DBG: spi_enable+++\r\n");
	lcd_write(SPI_CTRL, BIT_3);

	/*spi need set bit13, otherwith will lost some data*/
	/*spi need set bit6, otherwith color will error*/
	/*spi need open mcu, otherwidth can't send image data*/
	//lcd_set_bits(SMPN_CTRL, BIT_13 | BIT_6 | BIT_0);
	lcd_set_bits(SMPN_CTRL, BIT_13 | BIT_0);

}

static void spi_disable(void)
{
	LCDLOGD("DBG: spi_disable+++\r\n");
	lcd_write(SPI_CTRL, 0);
}

static int set_spi_clk(uint32_t src_clk, uint32_t spi_clk)
{
	uint32_t dividor;

	dividor = src_clk/spi_clk;
	if(dividor > 0xFF){
		LCDLOGE("ERROR: src_clk/clk can't large than 0xFF\r\n");
		return -1;
	}
	lcd_write_bits(SPI_CTRL, dividor, MASK8, 24);

//	LCDLOGD("Expect spi_clk = %d KHz, Real spi_clk = %d KHz\r\n",
//		spi_clk, src_clk/dividor);
	
	return 0;
}

static void set_spi_path(struct s_spi_ctx* spi_ctx, uint32_t path)
{
	if(path == SPI_PATH_IMAGE)
		lcd_set_bits(MISC_CTRL, BIT_0);
	else
		lcd_clear_bits(MISC_CTRL, BIT_0);
	spi_ctx->base_ctx->cur_path = path;
}

#ifdef LCD_INTERRUPT_MODE
static void trigger_spi_interrupt(void)
{
	uint32_t actual_flags = 0;
	int i = 0;
	u8 ret = OS_FAIL;

	g_lcd_irq_waiter ++;
	lcd_set_bits(SPI_CTRL, BIT_0);
    while(i < 10) {
		ret = UOS_WaitFlag(g_lcd_interrupt_flag, LCD_INTER_FLAG_SPIDONE, OSA_FLAG_OR_CLEAR,
			&actual_flags, COMMAND_TIMEOUT);
        if ((ret == OS_SUCCESS) && (0 != (actual_flags & LCD_INTER_FLAG_SPIDONE))){
            break;
        }else{
			LCDLOGW("Warning: trigger_spi_interrupt:wait to finish (0x%x)(%d)!\r\n", actual_flags, i);
        }
		i++;
    }
	g_lcd_irq_waiter --;

	if(i>= 10 & (0 != (actual_flags & LCD_INTER_FLAG_SPIDONE))){
		LCDLOGE("ERROR: trigger_spi_interrupt: got spi done timeout!\r\n");
	}
	lcd_clear_bits(SPI_CTRL, BIT_0);
}
#endif

static void trigger_spi_polling(int ass_mode)
{
	int reg = 0;
	
	lcd_set_bits(SPI_CTRL, BIT_0);
	reg = lcd_read(IRQ_ISR_RAW);
	while (0 == (reg & IRQ_SPI_DONE)){
#if 1		
		if(0 == ass_mode)
			UOS_Sleep(MS_TO_TICKS(5));
		else
			mdelay(5);
#else
		UOS_Sleep(MS_TO_TICKS(5));
#endif
		LCDLOGI("Info: trigger_spi_polling, wait to finish (0x%x)\r\n", reg);
		reg = lcd_read(IRQ_ISR_RAW);
	}
	lcd_write(IRQ_ISR_RAW, ~IRQ_SPI_DONE);
	lcd_clear_bits(SPI_CTRL, BIT_0);
}

static void trigger_spi(int work_mode)
{
#ifdef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode)
		trigger_spi_interrupt();
	else if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		trigger_spi_polling(1);
	else
		trigger_spi_polling(0);
#else /*polling mode*/
	if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		trigger_spi_polling(1);
	else
		trigger_spi_polling(0);
#endif
}

static void spi_config(struct spi_info *info)
{
	int reg = 0;

	reg = lcd_read(SPI_CTRL);
	if(info->interface_id == 1)
		reg |= BIT_1;
	else
		reg &= ~BIT_1;

	if(info->device_id == 1)
		reg |= BIT_2;
	else
		reg &= ~BIT_2;

	if(info->endian == SPI_ENDIAN_LSB){
		reg |= BIT_5 | BIT_4;
	} else {
		reg &= ~(BIT_5 | BIT_4);
	}

	if(info->sample_edge == SPI_EDGE_FALLING)
		reg |= BIT_7;
	else
		reg &= ~BIT_7;
	lcd_write(SPI_CTRL, reg);

	reg = lcd_read(MISC_CTRL);
	if(info->line_num == 3)
		reg |= BIT_3;
	else
		reg &= ~BIT_3;
	lcd_write(MISC_CTRL, reg);
}

static void spi_img_config(struct spi_info *info)
{
	int reg = 0;

	reg = lcd_read(MISC_CTRL);
	
	if(info->data_lane_num == 2)
		reg |= BIT_2;
	else
		reg &= ~BIT_2;

	reg |= BIT_1; /*should be set, otherwith, color will error*/

	if(info->format == SPI_FORMAT_RGB666_2_3){
		reg |= BIT_5;
		reg &= ~BIT_4;
	} else if(info->format == SPI_FORMAT_RGB888_2_3){
		reg |= BIT_4;
		reg &= ~BIT_5;
	} else{
		reg &= ~(BIT_4 | BIT_5);
	}
	lcd_write(MISC_CTRL, reg);

	switch(info->format){
	case SPI_FORMAT_RGB565:
		if(info->data_lane_num == 2)
			lcd_write_bits(SMPN_CTRL, 5, MASK4, 8);
		else
			lcd_write_bits(SMPN_CTRL, 2, MASK4, 8);
		break;
	case SPI_FORMAT_RGB666:
		if(info->data_lane_num == 2)
			lcd_write_bits(SMPN_CTRL, 4, MASK4, 8);
		else
			lcd_write_bits(SMPN_CTRL, 1, MASK4, 8);
		break;
	case SPI_FORMAT_RGB666_2_3:
		lcd_write_bits(SMPN_CTRL, 6, MASK4, 8);
		break;
	case SPI_FORMAT_RGB888:
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	case SPI_FORMAT_RGB888_2_3:
		lcd_write_bits(SMPN_CTRL, 0, MASK4, 8);
		break;
	default:
		LCDLOGE("ERROR: spi_img_config, Invalid format!\r\n");
	}
}

struct s_spi_ctx* spi_init(uint32_t sclk, struct spi_info *info, int32_t work_mode)
{
	int dividor;
	struct s_spi_base_ctx *spi_base_ctx = &g_spi_base_ctx;
	struct s_spi_ctx *spi_ctx = NULL;
	int i;

	LCDLOGD("spi_init (ref_cnt: %d, dev_id:%d)+++\r\n", g_spi_ref_cnt, info->device_id);

	if(info == NULL || info->timing == NULL){
		LCDLOGE("ERROR: spi_init, Invalid param!\r\n");
		return NULL;
	}

#ifndef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode){
		LCDLOGW("WARNING: spi_init, no interrupt mode supported! change to polling mode!\r\n");
		work_mode = LCD_WORK_MODE_POLLING;
	}
#endif	

	if((info->format != SPI_FORMAT_RGB565) &&
		(info->format != SPI_FORMAT_RGB666) && (info->data_lane_num == 1)){
		LCDLOGE("ERROR: spi_init, Format error!\r\n");
		return NULL;
	}

	dividor = sclk/info->timing->rclk;
	if((dividor > 0xFF) || (dividor < 2)){
		LCDLOGE("ERROR: spi_init, Invalid read timing!\r\n");
		return NULL;
	}

	dividor = sclk/info->timing->wclk;	
	if((dividor > 0xFF) || (dividor < 2)){
		LCDLOGE("ERROR: spi_init, Invalid write timing!\r\n");
		return NULL;
	}

	if(g_spi_ref_cnt >= MAX_SPI_REF_COUNT){
		LCDLOGE("ERROR: too many spi devices, max count %d!\r\n", MAX_SPI_REF_COUNT);
		return NULL;
	}

	if(g_spi_ref_cnt > 0 && spi_base_ctx->sclk != sclk){
		LCDLOGE("ERROR: The new spi device's sclk(%d) is not same as the old sclk(%d)!\r\n", sclk, spi_base_ctx->sclk);
		return NULL;
	}

	if(g_spi_ref_cnt == 0){
		spi_base_ctx->base_addr = LCD_BASE_ADDR;
		spi_base_ctx->sclk = sclk;
		spi_base_ctx->cur_path = SPI_PATH_IMAGE;
		spi_base_ctx->cur_cs = 0;
	}

	for(i=0; i<MAX_SPI_REF_COUNT; i++){
		spi_ctx = &g_spi_ctx[i];
		if(SPI_STATUS_UNINIT == spi_ctx->status)
			break;
	}
	if(i == MAX_SPI_REF_COUNT){
		LCDLOGE("ERROR: spi devices are full!\r\n");
		return NULL;
	}
	g_spi_ref_cnt++;
	
	spi_ctx->base_ctx = spi_base_ctx;
	spi_ctx->info.line_num = info->line_num;
	spi_ctx->info.interface_id = info->interface_id;
	spi_ctx->info.data_lane_num = info->data_lane_num;
	spi_ctx->info.format = info->format;
	spi_ctx->info.device_id = info->device_id;
	spi_ctx->info.sample_edge = info->sample_edge;
	spi_ctx->info.force_cs = info->force_cs;
	spi_ctx->info.endian = info->endian;
	spi_ctx->info.timing = info->timing;

	spi_base_ctx->work_mode = work_mode;

	if(g_spi_ref_cnt == 1)
		spi_enable();

	spi_ctx->status = SPI_STATUS_INIT;
	LCDLOGD("spi_init (ref_cnt: %d)---\r\n", g_spi_ref_cnt);
	return spi_ctx;
}

void spi_reset(struct s_spi_ctx *spi_ctx)
{
	spi_enable();
}

int spi_update(struct s_spi_ctx *spi_ctx, int32_t work_mode)
{

	LCDLOGI("spi_update +++\r\n");

#ifndef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode){
		LCDLOGW("WARNING: spi_init, no interrupt mode supported! change to polling mode!\r\n");
		work_mode = LCD_WORK_MODE_POLLING;
	}
#endif	

	spi_ctx->base_ctx->work_mode = work_mode;
	LCDLOGI("spi_update ----\r\n");
	return 0;
}

int spi_set_cs(struct s_spi_ctx *spi_ctx, uint32_t enable)
{
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: spi_set_cs, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_set_cs, Invalid mcu status\r\n");
		return -1;
	}

	if(spi_ctx->info.force_cs == 0){
		LCDLOGE("ERROR: spi_set_cs, Not force CS mode, can't set CS!\r\n");
		return -1;
	}

	if(enable)
		lcd_set_bits(SPI_CTRL, BIT_6);
	else
		lcd_clear_bits(SPI_CTRL, BIT_6);
	spi_ctx->base_ctx->cur_cs = enable;
	return 0;
}

int spi_write_cmd(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t bits)
{
	uint32_t wcmd, wbits;
	int ret;

	if(NULL == spi_ctx || bits > 32 || 0 == bits){
		LCDLOGE("ERROR: spi_write_cmd, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_write_cmd, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->base_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: spi_write_cmd, Invalid CS status\r\n");
		return -1;
	}

//	LCDLOGD("spi_write_cmd: device_id: %d\r\n", spi_ctx->info.device_id);

	spi_config(&spi_ctx->info);

	if(spi_ctx->base_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

	ret = set_spi_clk(spi_ctx->base_ctx->sclk, spi_ctx->info.timing->wclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}

	if(3 == spi_ctx->info.line_num){/*3 line mode*/
		if(bits == 32){
			LCDLOGE("ERROR: spi_write_cmd, too many write bits for 3 line mode!\r\n");
			return -1;
		}
		wbits = bits + 1;
		wcmd = cmd;/*0 -command, 1-data*/
	} else { /*4 line mode*/
		wbits = bits;
		wcmd = cmd;
	}

	if(4 == spi_ctx->info.line_num){/*4 line mode, set DCX pin first*/
//		lcd_write_bits(DUMB_CONTROL, 0, MASK8, 20);
		gpio_direction_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX, 0);
	}

	lcd_write(SPI_TXDATA, wcmd);
	lcd_write_bits(SPI_CTRL, wbits - 1, MASK16, 8);
	trigger_spi(spi_ctx->base_ctx->work_mode);
	return 0;
}

int spi_write_data(struct s_spi_ctx *spi_ctx, uint32_t data, uint32_t bits)
{
	uint32_t wdata, wbits;
	int ret;

	if(NULL == spi_ctx || bits > 32 || 0 == bits){
		LCDLOGE("ERROR: spi_write_data, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_write_data, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->base_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: spi_write_data, Invalid CS status\r\n");
		return -1;
	}

//	LCDLOGD("spi_write_data: device_id: %d\r\n", spi_ctx->info.device_id);

	spi_config(&spi_ctx->info);

	if(spi_ctx->base_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

 	/*set write clk as default*/
	ret = set_spi_clk(spi_ctx->base_ctx->sclk, spi_ctx->info.timing->wclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}

	if(3 == spi_ctx->info.line_num){
		if(bits == 32){
			LCDLOGE("ERROR: spi_write_data, too many write bits for 3 line mode!\r\n");
			return -1;
		}
		wbits = bits + 1;
		wdata = (1 << bits) | data;/*0 -command, 1-data*/
	} else { /*4 line mode*/
		wbits = bits;
		wdata = data;
	}

	if(4 == spi_ctx->info.line_num){/*4 line mode, set DCX pin first*/
		//lcd_write_bits(DUMB_CONTROL, SPI_4LINE_DCX, MASK8, 20);
		gpio_direction_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX, 1);
	}

	lcd_write(SPI_TXDATA, wdata);
	lcd_write_bits(SPI_CTRL, wbits - 1, MASK16, 8);
	trigger_spi(spi_ctx->base_ctx->work_mode);
	return 0;
}

int spi_read_data(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t cmd_bits,
	uint32_t *data,  uint32_t data_bits)
{
	uint32_t wcmd, wbits, rbits;
	int ret;
	uint32_t value_and = 0;

	value_and = (1 << data_bits) - 1;

	if(NULL == spi_ctx || cmd_bits > 32 || data_bits > 32){
		LCDLOGE("ERROR: spi_read_data, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_read_data, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->base_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: spi_read_data, Invalid CS status\r\n");
		return -1;
	}

//	LCDLOGD("spi_read_data: device_id: %d\r\n", spi_ctx->info.device_id);

	ret = set_spi_clk(spi_ctx->base_ctx->sclk, spi_ctx->info.timing->rclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}

	spi_config(&spi_ctx->info);

	if(spi_ctx->base_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

	if(data_bits > 8)
		rbits = data_bits;
	else
		rbits = data_bits -1;
		

	if(3 == spi_ctx->info.line_num){
		if(cmd == 32){
			LCDLOGE("ERROR: spi_read_data, too many write bits for 3 line mode!\r\n");
			return -1;
		}
		wbits = cmd_bits;
		wcmd = cmd;/*0 -command, 1-data*/
	} else { /*4 line mode*/
		wbits = cmd_bits - 1;
		wcmd = cmd;
	}

	if(4 == spi_ctx->info.line_num){/*4 line mode, set DCX pin first*/
		//lcd_write_bits(DUMB_CONTROL, 0, MASK8, 20);
		gpio_direction_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX, 0);
	}

	lcd_write(SPI_TXDATA, wcmd);
	lcd_write_bits(SPI_CTRL, wbits, MASK8, 8);
	lcd_write_bits(SPI_CTRL, rbits, MASK8, 16);
	trigger_spi(spi_ctx->base_ctx->work_mode);

	*data = lcd_read(SPI_RXDATA);
	*data &= value_and;

	return 0;
}

int spi_before_refresh(struct s_spi_ctx *spi_ctx)
{
	int ret;
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: spi_before_refresh, Invalid param\r\n");
		return -1;
	}
	
	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_before_refresh, Invalid spi status\r\n");
		return -1;
	}

	LCDLOGD("spi_before_refresh: device_id: %d\r\n", spi_ctx->info.device_id);

	spi_config(&spi_ctx->info);
	spi_img_config(&spi_ctx->info);

	if(spi_ctx->base_ctx->cur_path != SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_IMAGE);

 	/*set write clk as default*/
	ret = set_spi_clk(spi_ctx->base_ctx->sclk, spi_ctx->info.timing->wclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}

	if(2 == spi_ctx->info.data_lane_num){
		/*2 data lane*/
		switch(spi_ctx->info.format){
		case SPI_FORMAT_RGB565:
			lcd_write_bits(SPI_CTRL, 15, MASK8, 8);
			break;
		case SPI_FORMAT_RGB666:
			lcd_write_bits(SPI_CTRL, 17, MASK8, 8);
			break;
		case SPI_FORMAT_RGB666_2_3:
			lcd_write_bits(SPI_CTRL, 11, MASK8, 8);
			break;
		case SPI_FORMAT_RGB888:
			lcd_write_bits(SPI_CTRL, 23, MASK8, 8);
			break;
		case SPI_FORMAT_RGB888_2_3:
			lcd_write_bits(SPI_CTRL, 15, MASK8, 8);
			break;
		}
	} else{
		/*1 data lane*/
		lcd_write_bits(SPI_CTRL, 7, MASK8, 8);
	}

	if(4 == spi_ctx->info.line_num){/*4 line mode, set DCX pin first*/
		//lcd_write_bits(DUMB_CONTROL, SPI_4LINE_DCX, MASK8, 20);
		gpio_direction_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX, 1);
	}

	return 0;

}

int spi_after_refresh(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: spi_after_refresh, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_after_refresh, Invalid spi status\r\n");
		return -1;
	}

	if(spi_ctx->base_ctx->cur_path != SPI_PATH_REGISTER)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);
	return 0;
}

void spi_uninit(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: spi_uninit, Invalid param\r\n");
		return;
	}

	LCDLOGD("spi_uninit (ref_cnt: %d)+++\r\n", g_spi_ref_cnt);
	if(spi_ctx->status != SPI_STATUS_INIT){
		LCDLOGE("ERROR: spi_uninit, This spi device has not been initialized!\r\n");
		return;
	}

	spi_ctx->status = SPI_STATUS_UNINIT;
	g_spi_ref_cnt--;

	if(0 == g_spi_ref_cnt)
		spi_disable();
	LCDLOGD("spi_uninit (ref_cnt: %d)+++\r\n", g_spi_ref_cnt);
}
