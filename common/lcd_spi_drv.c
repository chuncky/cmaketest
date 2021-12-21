#include "lcd_reg.h"
#include "spi_drv.h"
//#include "cgpio.h"
#include "lcd_common.h"
//#include "osa_old_api.h"
#include "lcd_drv.h"
#include "mfpr_api.h"

struct s_spi_ctx g_spi_ctx;
#ifdef LCD_INTERRUPT_MODE
extern OSAFlagRef g_lcd_interrupt_flag_ref;
#endif
#if (defined WATCHLCDST7789_CODE_USE) || (defined LCD_GC9306VER2) || (defined LCD_ST7789VER2) || (defined LCD_GC9305VER1)
#define SPI_4LINE_DCX 21
#else
#define SPI_4LINE_DCX 27
#endif

static int set_spi_clk(uint32_t src_clk, uint32_t spi_clk)
{
	uint32_t dividor;

	dividor = src_clk/spi_clk;
	if(dividor > 0xFF){
		LCDLOGE("ERROR: src_clk/clk can't large than 0xFF\r\n");
		return -1;
	}
	lcd_write_bits(SPI_CTRL, dividor, MASK8, 24);

	LCDLOGI("Expect spi_clk = %d KHz, Real spi_clk = %d KHz\r\n",
		spi_clk, src_clk/dividor);
	
	return 0;
}

static void set_spi_path(struct s_spi_ctx* spi_ctx, uint32_t path)
{
	if(path == SPI_PATH_IMAGE)
		lcd_set_bits(MISC_CTRL, BIT_0);
	else
		lcd_clear_bits(MISC_CTRL, BIT_0);
	spi_ctx->cur_path = path;
}
#ifdef LCD_INTERRUPT_MODE
static void trigger_spi_interrupt(void)
{
	uint32_t actual_flags = 0;
	int i = 0;
	OSA_STATUS ret = OS_FAIL;

	lcd_set_bits(SPI_CTRL, BIT_0);
    while(i < 10) {
		ret = OSAFlagWait(g_lcd_interrupt_flag_ref, LCD_INTER_FLAG_SPIDONE, OSA_FLAG_OR_CLEAR, &actual_flags, COMMAND_TIMEOUT);	
        if ((ret == OS_SUCCESS) && (0 != (actual_flags & LCD_INTER_FLAG_SPIDONE))){
            break;
        }else{
			LCDLOGD("Warning: trigger_spi_interrupt:wait to finish (0x%x)(%d)!\r\n", actual_flags, i);
        }
		i++;
    }
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
		//if(0 == ass_mode)
		//	OSATaskSleep(10);
		//else
			mdelay(5);
#else
		OSATaskSleep(MS_TO_TICKS(5));
#endif
		//LCDLOGI("Info: trigger_spi_polling, wait to finish (0x%x)\r\n", reg);
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

struct s_spi_ctx* spi_init(uint32_t sclk, struct spi_info *info, int32_t work_mode)
{
	int ret;
	int dividor;
	struct s_spi_ctx *spi_ctx = &g_spi_ctx;
	int reg = 0;

	LCDLOGI("spi_init +++\r\n");

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

#if 0
	memset(spi_ctx, 0, sizeof(struct s_spi_ctx));
#else
	spi_ctx->cur_path = 0;
	spi_ctx->cur_cs = 0;
	spi_ctx->status = 0;
#endif
	spi_ctx->base_addr = LCD_BASE_ADDR;
	spi_ctx->sclk = sclk;
#if 0
	memcpy(&spi_ctx->info, info, sizeof(struct spi_info));
#else
	spi_ctx->info.line_num = info->line_num;
	spi_ctx->info.interface_id = info->interface_id;
	spi_ctx->info.data_lane_num = info->data_lane_num;
	spi_ctx->info.format = info->format;
	spi_ctx->info.device_id = info->device_id;
	spi_ctx->info.sample_edge = info->sample_edge;
	spi_ctx->info.force_cs = info->force_cs;
	spi_ctx->info.endian = info->endian;
	spi_ctx->info.timing = info->timing;
#endif
	spi_ctx->work_mode = work_mode;

	if(info->interface_id == 1)
		reg |= BIT_1;

	if(info->device_id == 1)
		reg |= BIT_2;

	if(info->endian == SPI_ENDIAN_LSB){
		reg |= BIT_5 | BIT_4;
	}

	if(info->sample_edge == SPI_EDGE_FALLING)
		reg |= BIT_7;

	/*enable spi*/
	reg |= BIT_3;
	lcd_write(SPI_CTRL, reg);

	reg = 0;
	if(info->line_num == 3)
		reg |= BIT_3;
	
	if(info->data_lane_num == 2)
		reg |= BIT_2;

	reg |= BIT_1; /*should be set, otherwith, color will error*/

	if(info->format == SPI_FORMAT_RGB666_2_3)
		reg |= BIT_5;
	else if(info->format == SPI_FORMAT_RGB888_2_3)
		reg |= BIT_4;
	lcd_write(MISC_CTRL, reg);


	if(info->line_num == 4){/*enable GPIO for D/CX pin for 4 line mode*/
//		lcd_write_bits(DUMB_CONTROL, SPI_4LINE_DCX, MASK8, 12);
		//GpioSetDirection(SPI_4LINE_DCX, GPIO_OUT_PIN);
		//GpioSetLevel(SPI_4LINE_DCX, GPIO_LOW);
#if 0
		unsigned int value[2];
		value[0] = MFP_REG(GPIO_27| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
		value[1] = MFP_EOC;
		mfp_config(value);
#endif

		
		//set output
		gpio_set_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX,0);
	}

	
	lcd_set_bits(SMPN_CTRL, BIT_13 | BIT_0);

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
		LCDLOGE("ERROR: spi_init, Invalid format!\r\n");
		return NULL;
	}
	
 	/*set write clk as default*/
	ret = set_spi_clk(sclk, info->timing->wclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_init, set spi clk error!\r\n");
		return NULL;
	}

	/*set register path as default*/
	set_spi_path(spi_ctx, SPI_PATH_REGISTER);
	spi_ctx->status = SPI_STATUS_INIT;
	LCDLOGI("spi_init ----\r\n");
	return spi_ctx;
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

	spi_ctx->work_mode = work_mode;
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
	spi_ctx->cur_cs = enable;
	return 0;
}

int spi_write_cmd(struct s_spi_ctx *spi_ctx, uint32_t cmd, uint32_t bits)
{
	uint32_t wcmd, wbits;

	if(NULL == spi_ctx || bits > 32 || 0 == bits){
		LCDLOGE("ERROR: spi_write_cmd, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_write_cmd, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: spi_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

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
		//GpioSetDirection(SPI_4LINE_DCX, GPIO_OUT_PIN);
		//GpioSetLevel(SPI_4LINE_DCX, GPIO_LOW);
#if 0
								unsigned int value[2];
								value[0] = MFP_REG(GPIO_27| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
								value[1] = MFP_EOC;
								mfp_config(value);
#endif		



		//set output
		gpio_set_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX,0);
	}

	lcd_write(SPI_TXDATA, wcmd);
	lcd_write_bits(SPI_CTRL, wbits - 1, MASK16, 8);
	trigger_spi(spi_ctx->work_mode);
	return 0;
}

int spi_write_data(struct s_spi_ctx *spi_ctx, uint32_t data, uint32_t bits)
{
	uint32_t wdata, wbits;

	if(NULL == spi_ctx || bits > 32 || 0 == bits){
		LCDLOGE("ERROR: spi_write_data, Invalid param\r\n");
		return -1;
	}

	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_write_data, Invalid mcu status\r\n");
		return -1;
	}

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: spi_write_data, Invalid CS status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path == SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);

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
		//GpioSetDirection(SPI_4LINE_DCX, GPIO_OUT_PIN);
		//GpioSetLevel(SPI_4LINE_DCX, GPIO_HIGH);
#if 0
								unsigned int value[2];
								value[0] = MFP_REG(GPIO_27| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
								value[1] = MFP_EOC;
								mfp_config(value);
#endif		



		//set output
		gpio_set_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX,1);
	}

	lcd_write(SPI_TXDATA, wdata);
	lcd_write_bits(SPI_CTRL, wbits - 1, MASK16, 8);
	trigger_spi(spi_ctx->work_mode);
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

	if((spi_ctx->info.force_cs == 1) && (spi_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: spi_read_data, Invalid CS status\r\n");
		return -1;
	}

	ret = set_spi_clk(spi_ctx->sclk, spi_ctx->info.timing->rclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}

	if(spi_ctx->cur_path == SPI_PATH_IMAGE)
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
		//GpioSetDirection(SPI_4LINE_DCX, GPIO_OUT_PIN);
		//GpioSetLevel(SPI_4LINE_DCX, GPIO_LOW);
#if 0
								unsigned int value[2];
								value[0] = MFP_REG(GPIO_27| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
								value[1] = MFP_EOC;
								mfp_config(value);
#endif		



		//set output
		gpio_set_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX,0);
	}

	lcd_write(SPI_TXDATA, wcmd);
	lcd_write_bits(SPI_CTRL, wbits, MASK8, 8);
	lcd_write_bits(SPI_CTRL, rbits, MASK8, 16);
	trigger_spi(spi_ctx->work_mode);

	*data = lcd_read(SPI_RXDATA);
	*data &= value_and;

 	/*set write clk as default*/
	ret = set_spi_clk(spi_ctx->sclk, spi_ctx->info.timing->wclk);
	if(-1 == ret){
		LCDLOGE("ERROR: spi_read_data, set spi clk error!\r\n");
		return -1;
	}
	return 0;
}

int spi_before_refresh(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: spi_before_refresh, Invalid param\r\n");
		return -1;
	}
	
	if(SPI_STATUS_INIT != spi_ctx->status){
		LCDLOGE("ERROR: spi_before_refresh, Invalid spi status\r\n");
		return -1;
	}

	if(spi_ctx->cur_path != SPI_PATH_IMAGE)
		set_spi_path(spi_ctx, SPI_PATH_IMAGE);

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
		//GpioSetDirection(SPI_4LINE_DCX, GPIO_OUT_PIN);
		//GpioSetLevel(SPI_4LINE_DCX, GPIO_HIGH);
#if 0
		unsigned int value[2];
		value[0] = MFP_REG(GPIO_27| MFP_AF0 | MFP_DRIVE_MEDIUM | MFP_PULL_LOW & ~MFP_SLEEP_DIR | MFP_LPM_EDGE_NONE);
		value[1] = MFP_EOC;
		mfp_config(value);
#endif				
		//set output
		gpio_set_output(SPI_4LINE_DCX);
		gpio_set_value(SPI_4LINE_DCX,1);
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

	if(spi_ctx->cur_path != SPI_PATH_REGISTER)
		set_spi_path(spi_ctx, SPI_PATH_REGISTER);
	return 0;
}

void spi_uninit(struct s_spi_ctx *spi_ctx)
{
	if(NULL == spi_ctx){
		LCDLOGE("ERROR: spi_uninit, Invalid param\r\n");
		return;
	}

	lcd_write(SPI_CTRL, 0);
	lcd_write(MISC_CTRL, 0);
	spi_ctx->status = SPI_STATUS_UNINIT;
}
