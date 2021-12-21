#include "lcd_reg.h"
#include "mcu_drv.h"
#include "lcd_drv.h"
#ifdef LCD_INTERRUPT_MODE
#include "lcd_common.h"
#include <ui_os_api.h>
#endif

struct s_mcu_ctx g_mcu_ctx;

#ifdef LCD_INTERRUPT_MODE
extern u8 g_lcd_interrupt_flag;
extern int g_lcd_irq_waiter;
#endif

static void set_mcu_path(struct s_mcu_ctx *mcu_ctx, uint32_t path)
{
	if(path == MCU_PATH_IMAGE)
		lcd_clear_bits(SMPN_CTRL, BIT_4);
	else
		lcd_set_bits(SMPN_CTRL, BIT_4);
	mcu_ctx->cur_path = path;
}

#ifdef LCD_INTERRUPT_MODE
static void trigger_mcu_interrupt(uint32_t value)
{
	uint32_t actual_flags = 0;
	int i = 0;
	u8 ret = OS_FAIL;

	g_lcd_irq_waiter = 1;
	lcd_write(SLV_PORT, value);

	while(i < 10) {
		ret = UOS_WaitFlag(g_lcd_interrupt_flag, LCD_INTER_FLAG_MCUDONE, OSA_FLAG_OR_CLEAR,
			&actual_flags, COMMAND_TIMEOUT);
		if ((ret == OS_SUCCESS) && (0 != (actual_flags & LCD_INTER_FLAG_MCUDONE))){
			break;
		}else{
			LCDLOGW("Warning: trigger_mcu_interrupt:wait to finish (0x%x)(%d)!\r\n", actual_flags, i);
		}
		i++;
	}
	g_lcd_irq_waiter = 0;
	if(i>= 10 & (0 == (actual_flags & LCD_INTER_FLAG_MCUDONE))){
		LCDLOGE("ERROR: trigger_mcu_interrupt: got mcu done timeout!\r\n");
	}
}
#endif

static void trigger_mcu_polling(uint32_t value, int ass_mode)
{
	int reg = 0;

	lcd_write(SLV_PORT, value);
	reg = lcd_read(IRQ_ISR_RAW);
	while (0 == (reg & IRQ_MCU_DONE)){
#if 1
		if(0 == ass_mode){
			//UOS_Sleep(MS_TO_TICKS(5));
			LCDLOGE("ERROR,can not execute here\r\n");
			while(1);
		}
		else
			mdelay(5);
#else
		UOS_Sleep(MS_TO_TICKS(5));
#endif
		LCDLOGI("Info: trigger_mcu_polling, wait to finish(0x%x)\r\n", reg);
		reg = lcd_read(IRQ_ISR_RAW);
	}
	lcd_write(IRQ_ISR_RAW, ~IRQ_MCU_DONE);
}

static void trigger_mcu(uint32_t value, int work_mode)
{
#ifdef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode)
		trigger_mcu_interrupt(value);
	else if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		trigger_mcu_polling(value, 1);
	else
		trigger_mcu_polling(value, 0);
#else /*polling mode*/
	if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		trigger_mcu_polling(value, 1);
	else
		trigger_mcu_polling(value, 0);
#endif
}

struct s_mcu_ctx* mcu_init(uint32_t sclk, struct mcu_info *info, int32_t work_mode)
{
	struct s_mcu_ctx *mcu_ctx = &g_mcu_ctx;
	uint32_t reg = 0,scycle,rl,rh,wl,wh;

	LCDLOGI("INFO: mcu_init+++\r\n");

	if(info == NULL || info->timing == NULL){
		LCDLOGE("ERROR: mcu_init, Invalid param\r\n");
		return NULL;
	}

	if(work_mode >= LCD_WORK_MODE_LIMIT){
		LCDLOGE("ERROR: mcu_init, Invalid work mode\r\n");
		return NULL;
	}

#ifndef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode){
		LCDLOGW("WARNING: mcu_init, no interrupt mode supported! change to polling mode!\r\n");
		work_mode = LCD_WORK_MODE_POLLING;
	}
#endif	

	scycle = 1000000/sclk; /*ns*/
	rl = (info->timing->rlpw + scycle - 1) / scycle - 1;
	rh = (info->timing->rhpw + scycle - 1) / scycle - 1;
	wl = (info->timing->wlpw + scycle - 1) / scycle - 1;
	wh = (info->timing->whpw + scycle - 1) / scycle - 1;
	if((rl > 15) || (rh > 15) || (wl > 15) || (wh > 15)){
		LCDLOGE("ERROR: mcu_init, can't get mcu's clk!\r\n");
		return NULL;
	}

#if 0
	memset(mcu_ctx, 0, sizeof(struct s_mcu_ctx));
#else
	mcu_ctx->cur_path = 0;
	mcu_ctx->cur_cs = 0;
	mcu_ctx->status = 0;
#endif
	mcu_ctx->base_addr = LCD_BASE_ADDR;
	mcu_ctx->sclk = sclk;
#if 0
	memcpy(&mcu_ctx->info, info, sizeof(struct mcu_info));
#else
	mcu_ctx->info.bus_mode = info->bus_mode;
	mcu_ctx->info.format = info->format;
	mcu_ctx->info.endian = info->endian;
	mcu_ctx->info.device_id = info->device_id;
	mcu_ctx->info.force_cs = info->force_cs;
	mcu_ctx->info.timing = info->timing;
#endif
	mcu_ctx->work_mode = work_mode;

	reg = rl<<28 | rh<<24 | wl<<20 | wh<<16;

	if(info->format == MCU_FORMAT_RGB565){
		reg |= (2 << 8);
	} else {
		reg |= (1 << 8);
	}

	if(info->endian == MCU_ENDIAN_MSB)
		reg |= BIT_6;

	if(info->device_id == 0)
		reg &= ~BIT_5;

	if(info->bus_mode == MCU_BUS_8080)
		reg &= ~BIT_2;

	reg |= BIT_0; /*enable mcu interface*/
	lcd_write(SMPN_CTRL, reg);

	lcd_set_bits(MISC_CTRL, BIT_1); /*should be set, otherwith color will error*/

	lcd_set_bits(PN_CTRL0, BIT_25); /*delay cs rising edge one cycle*/

	/*use register path as default*/
	set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);
	mcu_ctx->status = MCU_STATUS_INIT;
	LCDLOGI("INFO: mcu_init---\r\n");
	return mcu_ctx;
}

int mcu_update(struct s_mcu_ctx *mcu_ctx, int32_t work_mode)
{

	LCDLOGI("mcu_update +++\r\n");

#ifndef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode){
		LCDLOGW("WARNING: mcu_update, no interrupt mode supported! change to polling mode!\r\n");
		work_mode = LCD_WORK_MODE_POLLING;
	}
#endif	

	mcu_ctx->work_mode = work_mode;
	LCDLOGI("mcu_update ----\r\n");
	return 0;
}


int mcu_set_cs(struct s_mcu_ctx *mcu_ctx, uint32_t enable)
{
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_set_cs, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_set_cs, Invalid mcu status\r\n");
		return -1;
	}

	if(mcu_ctx->info.force_cs == 0){
		LCDLOGE("ERROR: mcu_set_cs, Not force CS mode, can't set CS!\r\n");
		return -1;
	}

	if(enable)
		lcd_set_bits(SMPN_CTRL, BIT_1);
	else
		lcd_clear_bits(SMPN_CTRL, BIT_1);
	mcu_ctx->cur_cs = enable;
	return 0;
}

int mcu_write_cmd(struct s_mcu_ctx *mcu_ctx, uint32_t cmd, uint32_t bits)
{
	int reg;
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_write_cmd, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_write_cmd, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(bits){
	case 8:
		reg = (cmd & 0xFF) | BIT_8;
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	case 16:
		reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	case 24:
		reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
		trigger_mcu(reg, mcu_ctx->work_mode);
		reg = ((cmd & 0xFF0000)>>16) | BIT_8;
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	case 32:
		reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
		trigger_mcu(reg, mcu_ctx->work_mode);
		reg = ((cmd & 0xFF0000)>>16) | BIT_8 | ((cmd & 0xFF000000)>>8) | BIT_24;	
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	default:
		LCDLOGE("ERROR: mcu_write_cmd, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}

int mcu_write_data(struct s_mcu_ctx *mcu_ctx, uint32_t data, uint32_t bits)
{
	int reg;
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_write_data, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_write_data, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(bits){
	case 8:
		reg = (data & 0xFF) | BIT_8 | BIT_15;
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	case 16:
		reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
			| BIT_31;
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	case 24:
		reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
			| BIT_31;
		trigger_mcu(reg, mcu_ctx->work_mode);
		reg = ((data & 0xFF0000)>>16) | BIT_8;		
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	case 32:
		reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
			| BIT_31;
		trigger_mcu(reg, mcu_ctx->work_mode);
		reg = ((data & 0xFF0000)>>16) | BIT_8 | BIT_15 | ((data & 0xFF000000)>>8) | BIT_24 | BIT_31;	
		trigger_mcu(reg, mcu_ctx->work_mode);
		break;
	default:
		LCDLOGE("ERROR: mcu_write_data, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}


int mcu_read_data(struct s_mcu_ctx *mcu_ctx, uint32_t cmd, uint32_t cmd_bits,
	uint32_t *data,  uint32_t data_bits)
{
	int reg;
	int readdata;
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_read_data, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_read_data, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(cmd_bits){
	case 8:
	case 24:
		if(cmd_bits == 8){
			reg = (cmd & 0xFF) | BIT_8;
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((cmd & 0xFF0000)>>16) | BIT_8;
		}
		switch(data_bits){
		case 8:
			reg |= BIT_25 | BIT_31;
			
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = lcd_read(ISA_RXDATA) & MASK8;
			break;
		case 16:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = lcd_read(ISA_RXDATA) & MASK16;
			break;
		case 24:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			readdata = lcd_read(ISA_RXDATA) & MASK16;
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = (readdata << 8) | (lcd_read(ISA_RXDATA) & MASK8);
			break;
		case 32:
			reg |= BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			readdata = lcd_read(ISA_RXDATA) & MASK16;
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = (readdata << 16) | (lcd_read(ISA_RXDATA) & MASK16);
			break;
		default:
			LCDLOGE("ERROR: mcu_read_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	case 16:
	case 32:
		if(cmd_bits == 16){
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((cmd & 0xFF0000)>>16) | BIT_8 | ((cmd & 0xFF000000)>>8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
		}
		switch(data_bits){
		case 8:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			*data = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			break;
		case 16:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			readdata = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = (readdata << 8)|(lcd_read(ISA_RXDATA) & MASK8);
			break;
		case 24:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			readdata = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = (readdata << 16)|(lcd_read(ISA_RXDATA) & MASK16);
			break;
		case 32:
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);//The first read is dummy
			readdata = (lcd_read(ISA_RXDATA) & MASK16) >> 8;
			reg = BIT_9 | BIT_15 | BIT_25 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			readdata = (readdata << 16)|(lcd_read(ISA_RXDATA) & MASK16);
			reg = BIT_9 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			*data = (readdata << 8)|(lcd_read(ISA_RXDATA) & MASK8);
			break;
		default:
			LCDLOGE("ERROR: mcu_read_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	default:
		LCDLOGE("ERROR: mcu_read_data, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}

int mcu_write_cmd_data(struct s_mcu_ctx *mcu_ctx, uint32_t cmd,
	uint32_t cmd_bits, uint32_t data, uint32_t data_bits)
{
	int reg;
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_write_cmd_data, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_write_cmd_data, Invalid mcu status\r\n");
		return -1;
	}

	if((mcu_ctx->info.force_cs == 1) && (mcu_ctx->cur_cs == 0)){
		LCDLOGE("ERROR: mcu_write_cmd, Invalid CS status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path == MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);

	switch(cmd_bits){
	case 8:
	case 24:
		if(cmd_bits == 8){
			reg = (cmd & 0xFF) | BIT_8;
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((cmd & 0xFF0000)>>16) | BIT_8;
		}
		switch(data_bits){
		case 8:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		case 16:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((data & 0xFF00) >> 8) | BIT_8 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		case 24:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((data & 0xFF00) >> 8) | BIT_8 | BIT_15 |
				((data & 0xFF0000)) | BIT_24 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		case 32:
			reg |= ((data & 0xFF)<<16) | BIT_24 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((data & 0xFF00) >> 8) | BIT_8 | BIT_15 |
				((data & 0xFF0000)) | BIT_24 | BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((data & 0xFF000000)>>24) | BIT_8 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		default:
			LCDLOGE("ERROR: mcu_write_cmd_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	case 16:
	case 32:
		if(cmd_bits == 16){
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
		} else {
			reg = (cmd & 0xFF) | BIT_8 | ((cmd & 0xFF00)<<8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((cmd & 0xFF0000)>>16) | BIT_8 | ((cmd & 0xFF000000)>>8) | BIT_24;
			trigger_mcu(reg, mcu_ctx->work_mode);
		}
		switch(data_bits){
		case 8:
			reg = (data & 0xFF) | BIT_8 | BIT_15;
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		case 16:
			reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
				| BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		case 24:
			reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
				| BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((data & 0xFF0000)>>16) | BIT_8;			
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		case 32:
			reg = (data & 0xFF) | BIT_8 | BIT_15 | ((data & 0xFF00)<<8) | BIT_24
				| BIT_31;
			trigger_mcu(reg, mcu_ctx->work_mode);
			reg = ((data & 0xFF0000)>>16) | BIT_8 | BIT_15 | ((data & 0xFF000000)<<8)
				| BIT_24 | BIT_31;		
			trigger_mcu(reg, mcu_ctx->work_mode);
			break;
		default:
			LCDLOGE("ERROR: mcu_write_cmd_data, Invalid bits!\r\n");
			return -1;
		}
		break;
	default:
		LCDLOGE("ERROR: mcu_write_cmd_data, Invalid bits!\r\n");
		return -1;
	}
	return 0;
}

int mcu_before_refresh(struct s_mcu_ctx *mcu_ctx)
{
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_before_refresh, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_before_refresh, Invalid mcu status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path != MCU_PATH_IMAGE)
		set_mcu_path(mcu_ctx, MCU_PATH_IMAGE);
	return 0;
}

int mcu_after_refresh(struct s_mcu_ctx *mcu_ctx)
{
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_after_refresh, Invalid param\r\n");
		return -1;
	}

	if(MCU_STATUS_INIT != mcu_ctx->status){
		LCDLOGE("ERROR: mcu_after_refresh, Invalid mcu status\r\n");
		return -1;
	}

	if(mcu_ctx->cur_path != MCU_PATH_REGISTER)
		set_mcu_path(mcu_ctx, MCU_PATH_REGISTER);
	return 0;
}

void mcu_uninit(struct s_mcu_ctx *mcu_ctx)
{
	if(NULL == mcu_ctx){
		LCDLOGE("ERROR: mcu_uninit, Invalid param\r\n");
		return;
	}

	lcd_write(SMPN_CTRL, 0);
	mcu_ctx->status = MCU_STATUS_UNINIT;
}

