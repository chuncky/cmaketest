#include "lcd_drv.h"
#include "lcd_reg.h"
#include "lcd_fns.h"
#ifdef LCD_INTERRUPT_MODE
#include "lcd_common.h"
#include <ui_os_api.h>
#include "plat_types.h"
#include "intc_api.h"
#endif
#include <string.h>
#include "psram_api.h"

enum{
	LCD_DRV_STATUS_UNINIT,
	LCD_DRV_STATUS_INIT,
	LCD_DRV_STATUS_LIMIT
};

struct s_lcd_ctx g_lcd_ctx;

static int g_cal_refresh_time = FALSE;
static int g_lcd_drv_status = LCD_DRV_STATUS_UNINIT;

#ifdef LCD_INTERRUPT_MODE
u8	g_lcd_interrupt_flag = INVALID_FLAG_ID;
int g_lcd_irq_waiter = 0;
#endif

#define DITHER_TB_4X4_INDEX0       (0x6e4ca280)
#define DITHER_TB_4X4_INDEX1       (0x5d7f91b3)
#define DITHER_TB_4X8_INDEX0       (0xb391a280)
#define DITHER_TB_4X8_INDEX1       (0x7f5d6e4c)
#define DITHER_TB_4X8_INDEX2       (0x80a291b3)
#define DITHER_TB_4X8_INDEX3       (0x4c6e5d7f)

void lcd_write(uint32_t reg, uint32_t val)
{
//	LCDLOGD("lcd_write: 0x%x write 0x%x\r\n", reg, val);
	LCD_BU_REG_WRITE(LCD_BASE_ADDR + reg, val);
}

uint32_t lcd_read(uint32_t reg)
{
	return LCD_BU_REG_READ(LCD_BASE_ADDR + reg);
}

void lcd_set_bits(uint32_t reg, uint32_t bits)
{
	lcd_write(reg, lcd_read(reg) | bits);
}

void lcd_clear_bits(uint32_t reg, uint32_t bits)
{
	lcd_write(reg, lcd_read(reg) & ~bits);
}

void lcd_write_bits(uint32_t reg, uint32_t value, uint32_t mask, uint32_t shifts)
{
	uint32_t reg_val;
	reg_val = lcd_read(reg);
	reg_val &= ~(mask << shifts);
	reg_val |= (value << shifts);
	lcd_write(reg, reg_val);
}

void dump_lcd_registers(void)
{
	int i;
	LCDLOGD("INFO: dump_lcd_regists++++\r\n");
	for(i = 0; i<0x300;i += 16){
		LCDLOGD("0x%x: 0x%x, 0x%x, 0x%x, 0x%x\r\n", i,
			lcd_read(i), lcd_read(i+4), lcd_read(i+8), lcd_read(i+12));
	}
	LCDLOGD("INFO: dump_lcd_regists----\r\n");
}

#ifdef LCD_INTERRUPT_MODE
static void lcd_isr_handler(uint32_t irq_num)
{
	struct s_lcd_ctx *lcd_ctx = &g_lcd_ctx;
	int reg;
	uint32_t flag = 0;
	u8 ret;

	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_isr_handler, Invalid param!\r\n");
		return;
	}

	reg = lcd_read(IRQ_ISR_RAW);
	lcd_write(IRQ_ISR_RAW, ~reg);
	LCDLOGI("INFO: lcd_isr_handler, irq = 0x%x!\r\n", reg);

	if(g_lcd_irq_waiter > 0){
		if(0 != (reg & IRQ_MCU_DONE))
			flag |= LCD_INTER_FLAG_MCUDONE;
		if(0 != (reg & IRQ_SPI_DONE))
			flag |= LCD_INTER_FLAG_SPIDONE;
		if(0 != (reg & IRQ_TE_DONE))
			flag |= LCD_INTER_FLAG_TEDONE;
		if(0 != (reg & IRQ_FRAME_DONE))
		{
			flag |= LCD_INTER_FLAG_FRAMEDONE;
			if(g_cal_refresh_time)
			{
				Performance_Exit("lcd_refresh");
				g_cal_refresh_time = FALSE;
			}
		}
		if(0 != (reg & IRQ_WB_DONE))
			flag |= LCD_INTER_FLAG_WBDONE;

		ret = UOS_SetFlag(g_lcd_interrupt_flag, flag, OSA_FLAG_OR);
		if (ret != 0) {
			LCDLOGE("ERROR: lcd_isr_handler: flag set error\n");
		}
	}
}

static void lcd_irq_config(void)
{
	lcd_write(IRQ_ENA, IRQ_FRAME_DONE|IRQ_MCU_DONE|IRQ_SPI_DONE|IRQ_WB_DONE);
}

static void lcd_irq_en(void)
{
	uint32_t int_num;
	INTC_ReturnCode intc_status;

	lcd_irq_config();
	int_num = INTCGetIntVirtualNum(LCD_IRQ);
	intc_status = INTCBind (int_num, lcd_isr_handler);
	intc_status = INTCConfigure(int_num, INTC_IRQ, INTC_HIGH_LEVEL);
	intc_status = INTCEnable(int_num);
	if(INTC_RC_OK != intc_status){
		LCDLOGE("ERR: lcd_irq_en enable irq fail!\r\n");
	}
}

static void lcd_irq_dis(void)
{
	uint32_t int_num;
	INTC_ReturnCode intc_status;

	lcd_write(IRQ_ENA, 0);
	int_num = INTCGetIntVirtualNum(LCD_IRQ);
	intc_status = INTCUnbind(int_num);
	intc_status = INTCDisable(int_num);
	if(INTC_RC_OK != intc_status){
		LCDLOGE("ERR: lcd_irq_dis disable irq fail!\r\n");
	}
}
#endif

static int lcd_check_config(struct s_lcd_ctx *lcd_ctx)
{
	struct s_wb_setting *wb = &lcd_ctx->wb_setting;

	if((wb->format != LCD_FORMAT_RGB565) && (wb->format != LCD_FORMAT_RGB888_PACKED)
		&& (wb->format != LCD_FORMAT_RGB888A) && (wb->format != LCD_FORMAT_RGBA888)){
		LCDLOGE("ERROR: lcd_check_config, Invalid wb format (%d)!\r\n", wb->format);
		return -1;
	}

	if(1 == lcd_ctx->dither_setting.dither_en){
		if(lcd_ctx->output_setting.interf == LCD_INTERFACE_SPI){
			if((lcd_ctx->output_setting.format == SPI_FORMAT_RGB888) ||
				(lcd_ctx->output_setting.format == SPI_FORMAT_RGB888_2_3)){
				LCDLOGE("ERROR: lcd_check_config, Invalid dither setting\r\n");
				return -1;
			}
		}
	}
	//TO BE DEFINED
	return 0;
}

static void lcd_config_OSD2(struct s_lcd_ctx *lcd_ctx)
{
	struct s_layer_setting *layer = &(lcd_ctx->layer_setting[LCD_LAYER_OSD2]);
	int reg = 0;
	struct s_color_key *ck = NULL;

	if(0 == layer->layer_en){
		lcd_write(TV_CTRL0, 0);
		return;
	}

	lcd_write(TVG_START_ADDR0, layer->src_buffer[0].addr);
	lcd_write(TVG_PITCH, layer->src_buffer[0].stride);
	lcd_write(TVG_OVSA_HPXL_VLN, layer->dest_point.x|(layer->dest_point.y<<16));
	reg = (layer->src_buffer[0].height<<16) | layer->src_buffer[0].width;
	lcd_write(TVG_HPXL_VLN, reg);
	lcd_write(TVGZM_HPXL_VLN, reg);

	switch(layer->format){
	case LCD_FORMAT_RGB565:
		reg = 0 << 16 | BIT_23;
		break;
	case LCD_FORMAT_RGB1555:
		reg = 1 << 16 | BIT_23;
		break;
	case LCD_FORMAT_RGB888_PACKED:
		reg = 2 << 16;
		break;
	case LCD_FORMAT_RGB888_UNPACKED:
		reg = 3 << 16;
		break;
	case LCD_FORMAT_RGBA888:
		reg = 4 << 16;
		break;
	case LCD_FORMAT_YUV422_PACKED:
		reg = (5 << 16) | BIT_9;
		break;
	case LCD_FORMAT_RGB888A:
		reg = 0xB << 16;
		break;
	default:
		LCDLOGW("Warning: lcd_config_OSD2, Invalid layer format (%d)!\r\n", layer->format);
		reg = 0 << 16;
	}

	if(0 == is_format_yuv(layer->format))
		reg |= BIT_12; /*RB swap as default*/
	reg |= BIT_8; /*layer enable*/
	lcd_write(TV_CTRL0, reg);

	if(layer->alpha_sel == LCD_ALPHA_LAYER){
		lcd_clear_bits(TV_CTRL1, BIT_16);
		lcd_write_bits(TV_CTRL1, layer->layer_alpha, MASK8, 8);
	} else {
		lcd_set_bits(TV_CTRL1, BIT_16);
	}

	if(layer->ck_setting.ck_en){
		ck = &layer->ck_setting.ck_y;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(TV_COLORKEY_Y, reg);

		ck = &layer->ck_setting.ck_u;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(TV_COLORKEY_U, reg);

		ck = &layer->ck_setting.ck_v;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(TV_COLORKEY_V, reg);

		switch(layer->ck_setting.ck_mode){
		case LCD_CK_MODE_Y:
			reg = 1;
			break;
		case LCD_CK_MODE_U:
			reg = 2;
			break;
		case LCD_CK_MODE_V:
			reg = 4;
			break;
		case LCD_CK_MODE_YUV:
			reg = 5;
			break;
		case LCD_CK_MODE_RGB:
			reg = 3;
			break;
		case LCD_CK_MODE_B:
			reg = 7;
			break;
		case LCD_CK_MODE_LUMA:
			reg = 6;
			break;
		default:
			reg = 0;
		}
		lcd_write_bits(TV_CTRL1, reg, MASK3, 24);
		lcd_set_bits(TV_CTRL1, BIT_21);
	} else {
		lcd_clear_bits(TV_CTRL1, BIT_21);
	}

	if(layer->cbsh_setting.cbsh_en){
		lcd_write(TV_CONTRAST, (layer->cbsh_setting.brightness << 16) |
			layer->cbsh_setting.contrast);
		lcd_write(TV_SATURATION, (layer->cbsh_setting.multiplier << 16) |
			layer->cbsh_setting.saturation);
		lcd_write(TV_CBSH_HUE, (layer->cbsh_setting.hue_sin << 16) |
			layer->cbsh_setting.hue_cos);
		lcd_set_bits(TV_CTRL0, BIT_29);
	} else {
		lcd_clear_bits(TV_CTRL0, BIT_29);
	}

	if(layer->mirror_en){
		lcd_set_bits(IOPAD_CONTROL, BIT_19);
	} else {
		lcd_clear_bits(IOPAD_CONTROL, BIT_19);
	}
}

static void lcd_config_OSD1(struct s_lcd_ctx *lcd_ctx)
{
	struct s_layer_setting *layer = &(lcd_ctx->layer_setting[LCD_LAYER_OSD1]);
	int reg = 0;
	struct s_color_key *ck = NULL;

	if(0 == layer->layer_en){
		lcd_clear_bits(PN_CTRL0, BIT_8);
		return;
	}
	if(!g_cal_refresh_time && (layer->src_buffer[0].height>=(320-10)) && (layer->src_buffer[0].width>=(240-10)))
	{
		g_cal_refresh_time = TRUE;
	}
	lcd_write(GRA_START_ADDR0, layer->src_buffer[0].addr);
	lcd_write(GRA_PITCH, layer->src_buffer[0].stride);
	lcd_write(GRA_OVSA_HPXL_VLN, layer->dest_point.x|(layer->dest_point.y<<16));
	reg = (layer->src_buffer[0].height<<16) | layer->src_buffer[0].width;
	lcd_write(GRA_HPXL_VLN, reg);
	lcd_write(GRAZM_HPXL_VLN, reg);

	switch(layer->format){
	case LCD_FORMAT_RGB565:
		lcd_write_bits(PN_CTRL0, 0, MASK4, 16);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB1555:
		lcd_write_bits(PN_CTRL0, 1, MASK4, 16);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB888_PACKED:
		lcd_write_bits(PN_CTRL0, 2, MASK4, 16);
		break;
	case LCD_FORMAT_RGB888_UNPACKED:
		lcd_write_bits(PN_CTRL0, 3, MASK4, 16);
		break;
	case LCD_FORMAT_RGBA888:
		lcd_write_bits(PN_CTRL0, 4, MASK4, 16);
		break;
	case LCD_FORMAT_YUV422_PACKED:
		lcd_write_bits(PN_CTRL0, 5, MASK4, 16);
		lcd_set_bits(PN_CTRL0, BIT_9);
		break;
	case LCD_FORMAT_RGB888A:
		lcd_write_bits(PN_CTRL0, 0xB, MASK4, 16);
		break;
	default:
		LCDLOGW("Warning: lcd_config_OSD1, Invalid layer format (%d)!\r\n", layer->format);
		lcd_write_bits(PN_CTRL0, 0x0, MASK4, 16);
	}

	if(1 == is_format_yuv(layer->format)){
		lcd_set_bits(PN_CTRL0, BIT_9);
		lcd_clear_bits(PN_CTRL0, BIT_12);//Disable RB swap
		lcd_set_bits(PN_CTRL0, BIT_8);/*layer enable*/
	} else {
		lcd_clear_bits(PN_CTRL0, BIT_9);
		lcd_set_bits(PN_CTRL0, BIT_8 | BIT_12);/*layer enable, RB swap as default*/
	}

	if(layer->alpha_sel == LCD_ALPHA_LAYER){
		lcd_clear_bits(PN_CTRL1, BIT_16);
		lcd_write_bits(PN_CTRL1, layer->layer_alpha, MASK8, 8);
	} else {
		lcd_set_bits(PN_CTRL1, BIT_16);
	}

	if(layer->ck_setting.ck_en){
		ck = &layer->ck_setting.ck_y;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(PN_COLORKEY_Y, reg);

		ck = &layer->ck_setting.ck_u;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(PN_COLORKEY_U, reg);

		ck = &layer->ck_setting.ck_v;
		reg = (ck->ck_threshold2 << 24) | (ck->ck_threshold1 << 16) |
			(ck->ck_dest_value << 8) | ck->ck_dest_alpha;
		lcd_write(PN_COLORKEY_V, reg);

		switch(layer->ck_setting.ck_mode){
		case LCD_CK_MODE_Y:
			reg = 1;
			break;
		case LCD_CK_MODE_U:
			reg = 2;
			break;
		case LCD_CK_MODE_V:
			reg = 4;
			break;
		case LCD_CK_MODE_YUV:
			reg = 5;
			break;
		case LCD_CK_MODE_RGB:
			reg = 3;
			break;
		case LCD_CK_MODE_B:
			reg = 7;
			break;
		case LCD_CK_MODE_LUMA:
			reg = 6;
			break;
		default:
			reg = 0;
		}
		lcd_write_bits(PN_CTRL1, reg, MASK3, 24);
		lcd_set_bits(TV_CTRL1, BIT_19);
	} else {
		lcd_clear_bits(TV_CTRL1, BIT_19);
	}

	if(layer->cbsh_setting.cbsh_en){
		lcd_write(PN_CONTRAST, (layer->cbsh_setting.brightness << 16) |
			layer->cbsh_setting.contrast);
		lcd_write(PN_SATURATION, (layer->cbsh_setting.multiplier << 16) |
			layer->cbsh_setting.saturation);
		lcd_write(PN_CBSH_HUE, (layer->cbsh_setting.hue_sin << 16) |
			layer->cbsh_setting.hue_cos);
		lcd_set_bits(PN_CTRL0, BIT_29);
	} else {
		lcd_clear_bits(PN_CTRL0, BIT_29);
	}

	if(layer->mirror_en){
		lcd_set_bits(IOPAD_CONTROL, BIT_15);
	} else {
		lcd_clear_bits(IOPAD_CONTROL, BIT_15);
	}
}

static void lcd_config_img(struct s_lcd_ctx *lcd_ctx)
{
	struct s_layer_setting *layer = &(lcd_ctx->layer_setting[LCD_LAYER_IMG]);
	int reg = 0;
	int plane = 0;

	if(0 == layer->layer_en){
		lcd_clear_bits(PN_CTRL0, BIT_0);
		return;
	}

	lcd_write(DMA_OVSA_HPXL_VLN, layer->dest_point.x|(layer->dest_point.y<<16));
	reg = (layer->src_buffer[0].height<<16) | layer->src_buffer[0].width;
	lcd_write(DMA_HPXL_VLN, reg);
	lcd_write(DMAZM_HPXL_VLN, reg);

	lcd_write(DMA_START_ADDR_Y0, layer->src_buffer[0].addr);
	lcd_write(DMA_PITCH_YC, layer->src_buffer[0].stride);

	plane = get_format_plane(layer->format);
	LCDLOGD("layer plane is %d\r\n", plane);
	if(3 == plane){ /*v plane*/
		lcd_write(DMA_START_ADDR_V0, layer->src_buffer[2].addr);
		lcd_write(DMA_START_ADDR_U0, layer->src_buffer[1].addr);
		lcd_write(DMA_PITCH_UV, layer->src_buffer[2].stride << 16 |
			layer->src_buffer[1].stride);
	}
	if(plane == 2){ /*u plane*/
		lcd_write(DMA_START_ADDR_U0, layer->src_buffer[1].addr);
		lcd_write(DMA_PITCH_UV, layer->src_buffer[1].stride);
	}

	switch(layer->format){
	case LCD_FORMAT_RGB565:
		lcd_write_bits(PN_CTRL0, 0, MASK4, 20);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB1555:
		lcd_write_bits(PN_CTRL0, 1, MASK4, 20);
		lcd_set_bits(PN_CTRL1, BIT_23);
		break;
	case LCD_FORMAT_RGB888_PACKED:
		lcd_write_bits(PN_CTRL0, 2, MASK4, 20);
		break;
	case LCD_FORMAT_RGB888_UNPACKED:
		lcd_write_bits(PN_CTRL0, 3, MASK4, 20);
		break;
	case LCD_FORMAT_RGBA888:
		lcd_write_bits(PN_CTRL0, 4, MASK4, 20);
		break;
	case LCD_FORMAT_YUV422_PACKED:
		lcd_write_bits(PN_CTRL0, 5, MASK4, 20);
		break;
	case LCD_FORMAT_YUV422_PLANAR:
		lcd_write_bits(PN_CTRL0, 6, MASK4, 20);
		break;
	case LCD_FORMAT_YUV420_PLANAR:
		lcd_write_bits(PN_CTRL0, 7, MASK4, 20);
		break;
	case LCD_FORMAT_RGB888A:
		lcd_write_bits(PN_CTRL0, 0xB, MASK4, 20);
		break;
	case LCD_FORMAT_YUV420_SEMI:
		lcd_write_bits(PN_CTRL0, 0xC, MASK4, 20);
		break;
	default:
		LCDLOGW("Warning: lcd_config_OSD2, Invalid layer format (%d)!\r\n", layer->format);
		reg = 0 << 16;
	}

	if(1 == is_format_yuv(layer->format)){
		lcd_set_bits(PN_CTRL0, BIT_1);
		lcd_clear_bits(PN_CTRL0, BIT_4);//Disable RB swap
		lcd_set_bits(PN_CTRL0, BIT_0); /*enable layer*/
	} else {
		lcd_clear_bits(PN_CTRL0, BIT_1);
		lcd_set_bits(PN_CTRL0, BIT_0 | BIT_4); /*enable layer, RB swap as default*/
	}

	if(layer->cbsh_setting.cbsh_en){
		lcd_write(PN_CONTRAST, (layer->cbsh_setting.brightness << 16) |
			layer->cbsh_setting.contrast);
		lcd_write(PN_SATURATION, (layer->cbsh_setting.multiplier << 16) |
			layer->cbsh_setting.saturation);
		lcd_write(PN_CBSH_HUE, (layer->cbsh_setting.hue_sin << 16) |
			layer->cbsh_setting.hue_cos);
		lcd_set_bits(PN_CTRL0, BIT_29);
	} else {
		lcd_clear_bits(PN_CTRL0, BIT_29);
	}

	if(layer->mirror_en){
		lcd_set_bits(IOPAD_CONTROL, BIT_13);
	} else {
		lcd_clear_bits(IOPAD_CONTROL, BIT_13);
	}
}

static void lcd_config_wb(struct s_lcd_ctx *lcd_ctx)
{
	struct s_wb_setting *wb = &lcd_ctx->wb_setting;
	int reg;

	if(0 == wb->wb_en){
		lcd_write(WDMA_CTRL1, 0);
		lcd_write(WDMA_CTRL2, 0);
		lcd_write(WDMA_CTRL3, 0);
		return;
	}

	reg = (wb->dest_buffer.stride << 16) | (16 << 8) | BIT_0;
	switch(wb->format){
	case LCD_FORMAT_RGB565:
		reg |= (0 << 4);
		lcd_write_bits(SMPN_CTRL, 5, MASK4, 8);
		break;
	case LCD_FORMAT_RGB888_PACKED:
		reg |= (1 << 4);
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	case LCD_FORMAT_RGB888A:
		reg |= (3 << 4);
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	case LCD_FORMAT_RGBA888:
		reg |= (2 << 4);
		lcd_write_bits(SMPN_CTRL, 3, MASK4, 8);
		break;
	default:
		LCDLOGE("ERROR: lcd_config_wb: Unsupported format 0x%x\r\n", wb->format);
		return;
	}
	lcd_write(WDMA_CTRL1, reg);
	reg = (wb->dest_buffer.height << 16) | wb->dest_buffer.width;
	lcd_write(WDMA_CTRL2, reg);
	lcd_write(WDMA_CTRL3, wb->dest_buffer.addr);
	lcd_write(WDMA_CTRL4, 0x84440);
	lcd_set_bits(SMPN_CTRL, BIT_13);
}

static void lcd_config_gamma(struct s_lcd_ctx *lcd_ctx)
{
	int i,j;
	uint32_t reg;

	LCDLOGI("INFO: lcd_config_gamma +++\r\n");
	if(0 == lcd_ctx->gamma_en){
		lcd_clear_bits(PN_CTRL0, BIT_30);
		return;
	}

	for(i=0;i<3;i++){
		for(j=0;j<256;j++){
			lcd_write(SRAM_WRDAT, 255 - j);
			reg = BIT_15 | (i << 8) | j;
			lcd_write(SRAM_CTRL, reg);
		}
	}

	lcd_set_bits(PN_CTRL0, BIT_30);
}

static void lcd_config_dither(struct s_lcd_ctx *lcd_ctx)
{
	uint32_t reg;
	int is_rgb565 = 0;
	struct s_dither_setting *dither = &lcd_ctx->dither_setting;
	struct s_output_setting *output = &lcd_ctx->output_setting;

	LCDLOGI("INFO: lcd_config_dither +++\r\n");
	if(0 == dither->dither_en){
		lcd_clear_bits(DITHER_CTRL, BIT_0);
		return;
	}

	if(output->interf == LCD_INTERFACE_MCU){
		if(output->format == MCU_FORMAT_RGB565)
			is_rgb565 = 1;
	} else {
		if(output->format == SPI_FORMAT_RGB565)
			is_rgb565 = 1;
	}

	if(dither->dither_mode == LCD_DITHER_4X8){
		reg = (0 << 16) | BIT_1 | BIT_0;
		if(1 == is_rgb565)
			reg |= BIT_4;
		else
			reg |= BIT_5;
		lcd_write(DITHER_CTRL, reg);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX0);
	
		lcd_write_bits(DITHER_CTRL, 1, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX1);
		lcd_write_bits(DITHER_CTRL, 2, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX2);
		lcd_write_bits(DITHER_CTRL, 3, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X8_INDEX3);
	} else {
		reg = (0 << 16) | BIT_0;
		if(1 == is_rgb565)
			reg |= BIT_4;
		else
			reg |= BIT_5;
		lcd_write(DITHER_CTRL, reg);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X4_INDEX0);
		lcd_write_bits(DITHER_CTRL, 1, MASK2, 16);
		lcd_write(DITHER_TBL_DATA, DITHER_TB_4X4_INDEX1);
	}
}

static void lcd_config_output(struct s_lcd_ctx *lcd_ctx)
{
	int reg = 0;
	struct s_output_setting *output = &lcd_ctx->output_setting;

	LCDLOGI("INFO: lcd_config_output +++\r\n");

	reg = ((output->height)<<16) | output->width;
	lcd_write(PN_V_H_ACTIVE, reg);
//	lcd_write(TOP_CTRL, 0x400000);//min read burst length
	lcd_write(TOP_CTRL, 0x40fc00);// max read burst length
	reg = lcd_read(PN_CTRL1);
	reg &= ~BIT_31;
#if 1
	if(lcd_ctx->wb_setting.wb_en)
		reg |= (BIT_28 | BIT_29 | BIT_30);
	else
		if(1 == output->te_en)
			reg &= ~(BIT_28 | BIT_29 | BIT_30);
		else
			reg |= (BIT_28 | BIT_29 | BIT_30);
#else
	reg |= (BIT_28 | BIT_29 | BIT_30);
#endif
	if(output->te_pol == LCD_POLARITY_POS)
		reg &= ~BIT_27;
	else
		reg |= BIT_27;
	reg |= 0x81;
	lcd_write(PN_CTRL1, reg);
	lcd_set_bits(PN_CTRL0, BIT_27);

	if(0 == lcd_ctx->wb_setting.wb_en){
		if(output->interf == LCD_INTERFACE_SPI){
			switch(output->format){
			case SPI_FORMAT_RGB565:
				if(output->data_lane_num == 2)
					lcd_write_bits(SMPN_CTRL, 5, MASK4, 8);
				else
					lcd_write_bits(SMPN_CTRL, 2, MASK4, 8);
				break;
			case SPI_FORMAT_RGB666:
				if(output->data_lane_num == 2)
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
			}
			lcd_set_bits(SMPN_CTRL, BIT_13);
		} else {
			if(output->format == MCU_FORMAT_RGB565){
				lcd_write_bits(SMPN_CTRL, 2, MASK4, 8);
			} else {
				lcd_write_bits(SMPN_CTRL, 1, MASK4, 8);
			}
			lcd_clear_bits(SMPN_CTRL, BIT_13);
		}
	}
	LCDLOGI("INFO: lcd_config_output ---\r\n");	
}

static void lcd_start_polling(int wb, int ass_mode)
{
	int reg = 0;
	int condition = 0;
	int timeout = 500;
	if(1 == wb)
		condition = IRQ_WB_DONE;
	else
		condition = IRQ_FRAME_DONE;
	lcd_set_bits(PN_CTRL1, BIT_31);	
	reg = lcd_read(IRQ_ISR_RAW);
	while (0 == (reg & condition) && timeout >0){
		timeout--;
#if 1
		if(0 == ass_mode)
			UOS_Sleep(MS_TO_TICKS(5));
		else
			mdelay(300);
#else
		UOS_Sleep(MS_TO_TICKS(5));
#endif
		LCDLOGE("Info: lcd_start_polling, wait to finish (0x%x)\r\n",reg);
		reg = lcd_read(IRQ_ISR_RAW);
	}
	if(0 == (reg & condition)){
		LCDLOGE("Error: lcd_start_polling, wait for frame done timeout! (0x%x)\r\n",reg);
	}
	lcd_write(IRQ_ISR_RAW, ~reg);
	//lcd_clear_bits(PN_CTRL1, BIT_31);
}

#ifdef LCD_INTERRUPT_MODE
static void lcd_start_interrupt(int wb)
{
	int i = 0;
	uint32_t actual_flags = 0;
	uint32_t expect_flag = 0;
	u8 ret = OS_FAIL;

	if(1 == wb)
		expect_flag = LCD_INTER_FLAG_WBDONE;
	else
		expect_flag = LCD_INTER_FLAG_FRAMEDONE;
	if(g_cal_refresh_time){
		//alarm if time cost greater than 35ms.
		Performance_Entry("lcd_refresh",PERF_CATCH_TIME|PERF_ALARM_TIME,0,35000);
	}
	g_lcd_irq_waiter ++;
	lcd_set_bits(PN_CTRL1, BIT_31);
	LCDLOGI("Info: lcd begin to start!!\r\n");
#define LCD_WAIT_TIMES 10	
    while(i < LCD_WAIT_TIMES) {
		ret = UOS_WaitFlag(g_lcd_interrupt_flag, expect_flag, OSA_FLAG_OR_CLEAR,
			&actual_flags, FRAME_TIMEOUT);
		if ((ret == 0) && (0 != (actual_flags & expect_flag))){
			LCDLOGD("DBG: lcd_start_interrtup: got frame or wb done!\r\n");
			break;
		}else{
			LCDLOGW("Warning: lcd_start_interrtup:wait to finish (0x%x)(%d)!\r\n", actual_flags, i);
		}
		i++;
    }
	g_lcd_irq_waiter --;
	if(i>= LCD_WAIT_TIMES & (0 != (actual_flags & expect_flag))){
		LCDLOGE("ERROR: lcd_start_interrtup: got frame or wb done timeout!\r\n");
	}
	//lcd_clear_bits(PN_CTRL1, BIT_31);
}
#endif

static void lcd_start(int wb, int work_mode)
{
#ifdef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == work_mode)
		lcd_start_interrupt(wb);
	else if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		lcd_start_polling(wb, 1);
	else
		lcd_start_polling(wb, 0);
#else
	if(LCD_WORK_MODE_ASS_POLLING == work_mode)
		lcd_start_polling(wb, 1);
	else
		lcd_start_polling(wb, 0);
#endif
}

int lcd_reset(struct s_lcd_ctx *lcd_ctx)
{
	lcd_irq_config();	
	return 0;
}

struct s_lcd_ctx* lcd_init(int work_mode)
{
	struct s_lcd_ctx *lcd_ctx = &g_lcd_ctx;
	LCDLOGE("INFO: lcd_init++\r\n");

	if(work_mode >= LCD_WORK_MODE_LIMIT) {
		LCDLOGE("ERROR: lcd_init: Invalid work_mode\r\n");
		return NULL;
	}

#ifndef LCD_INTERRUPT_MODE
	if(work_mode == LCD_WORK_MODE_INTERRUPT) {
		LCDLOGW("WARNING: lcd_init: No interrupt mode supported, change to polling mode!\r\n");
		work_mode = LCD_WORK_MODE_POLLING;
	}
#endif

	/*used for assert mode*/
	if(g_lcd_drv_status == LCD_DRV_STATUS_INIT){
		LCDLOGE("INFO: lcd_init has been init!\r\n");
		if(work_mode != lcd_ctx->work_mode){
			if(LCD_WORK_MODE_INTERRUPT == lcd_ctx->work_mode) {
				lcd_irq_dis();
				lcd_ctx->work_mode = work_mode;
				/*In assert mode, must use polling mode*/
				/*g_lcd_interrupt_flag can't be deleted,
				Sinde UOS_DeleteFlag can't be call in assert mode*/
			} else if(LCD_WORK_MODE_INTERRUPT == work_mode){
				lcd_ctx->work_mode = work_mode;
				if(g_lcd_interrupt_flag == INVALID_FLAG_ID){
					g_lcd_interrupt_flag = UOS_CreateFlag();
				}
				lcd_irq_en();
			}
		}
		return lcd_ctx;
	}

	memset(lcd_ctx, 0, sizeof(struct s_lcd_ctx));

	lcd_ctx->base_addr = LCD_BASE_ADDR;
	lcd_ctx->work_mode = work_mode;

#ifdef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == lcd_ctx->work_mode){
		g_lcd_interrupt_flag = UOS_CreateFlag();
		lcd_irq_en();
	}
#endif
	g_lcd_drv_status = LCD_DRV_STATUS_INIT;
	LCDLOGE("INFO: lcd_init --!\r\n");
	return lcd_ctx;
}

int lcd_enable_layer(struct s_lcd_ctx *lcd_ctx, uint32_t layer_id)
{
	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_enable_layer, Invalid param!\r\n");
		return -1;
	}

	if(layer_id >= LCD_LAYER_LIMIT){
		LCDLOGE("ERROR: lcd_enable_layer, Invalid layer_id!\r\n");
		return -1;
	}

	lcd_ctx->layer_setting[layer_id].layer_en = 1;
	return 0;
}

int lcd_disable_layer(struct s_lcd_ctx *lcd_ctx, uint32_t layer_id)
{
	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_enable_layer, Invalid param!\r\n");
		return -1;
	}

	if(layer_id >= LCD_LAYER_LIMIT){
		LCDLOGE("ERROR: lcd_enable_layer, Invalid layer_id!\r\n");
		return -1;
	}

	lcd_ctx->layer_setting[layer_id].layer_en = 0;
	return 0;
}

int lcd_refresh(struct s_lcd_ctx *lcd_ctx)
{
	int ret;

	LCDLOGI("INFO: lcd_refresh +++\r\n");

	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_refresh, Invalid param!\r\n");
		return -1;
	}

	ret = lcd_check_config(lcd_ctx);
	if(0 != ret){
		LCDLOGE("ERROR: lcd_refresh, Invalid config!\r\n");
		return -1;
	}

	lcd_config_OSD2(lcd_ctx);
	lcd_config_OSD1(lcd_ctx);
	lcd_config_img(lcd_ctx);
	lcd_config_wb(lcd_ctx);
	lcd_config_output(lcd_ctx);
	lcd_config_gamma(lcd_ctx);
	lcd_config_dither(lcd_ctx);

	if((lcd_ctx->layer_setting[LCD_LAYER_IMG].layer_en == 0) &&
		(lcd_ctx->layer_setting[LCD_LAYER_OSD1].layer_en == 0) &&
		(lcd_ctx->layer_setting[LCD_LAYER_OSD2].layer_en == 0)){
		LCDLOGI("INFO: set force bg color\r\n");
		lcd_set_bits(PN_CTRL0, BIT_24);
	} else {
		LCDLOGI("INFO: clean force bg color\r\n");
		lcd_clear_bits(PN_CTRL0, BIT_24);
	}

	lcd_write(PN_BLANKCOLOR, lcd_ctx->bg_color);
	switch(lcd_ctx->alpha_mode){
	case LCD_ALPHA_MODE_NORMAL:
		lcd_write(AFA_ALL2ONE, 0);
		break;
	case LCD_ALPHA_MODE_PRE:
		lcd_write(AFA_ALL2ONE, 1);
		break;
	case LCD_ALPHA_MODE_SPECIAL:
		lcd_write(AFA_ALL2ONE, 2);
		break;
	}
	if(lcd_ctx->wb_setting.wb_en)
		lcd_start(1, lcd_ctx->work_mode);
	else
		lcd_start(0, lcd_ctx->work_mode);
	LCDLOGI("INFO: lcd_refresh ---\r\n");
	return 0;
}

int lcd_after_refresh(struct s_lcd_ctx *lcd_ctx)
{
	LCDLOGI("INFO: lcd_after_refresh +++\r\n");

	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_after_refresh, Invalid param!\r\n");
		return -1;
	}
	lcd_ctx->layer_setting[LCD_LAYER_IMG].layer_en = 0;
	lcd_ctx->layer_setting[LCD_LAYER_OSD1].layer_en = 0;
	lcd_ctx->layer_setting[LCD_LAYER_OSD2].layer_en = 0;

	lcd_write(TV_CTRL0, 0);
	lcd_clear_bits(PN_CTRL0, BIT_8);
	lcd_clear_bits(PN_CTRL0, BIT_0);
	lcd_clear_bits(PN_CTRL0, BIT_24);

#if 1 /*workaround for issue: the display frame which after wb has no done interrup*/
	/*If not set these, send mcu command will fail at last frame*/
	lcd_clear_bits(WDMA_CTRL1, BIT_0);
	lcd_clear_bits(SMPN_CTRL, BIT_13);
	lcd_write_bits(PN_CTRL1,0,MASK3,28);
#endif
	return 0;
}

int lcd_output_setting(struct s_lcd_ctx *lcd_ctx, struct panel_spec *panel)
{
	struct s_dither_setting *dither = &lcd_ctx->dither_setting;
	struct s_output_setting *output = NULL;
	struct mcu_info *mcu = NULL;
	struct spi_info *spi = NULL;

	LCDLOGI("INFO: lcd_output_setting +++\r\n");

	if((NULL == lcd_ctx) || (NULL == panel)){
		LCDLOGE("ERROR: lcd_output_setting, Invalid param!\r\n");
		return -1;
	}

	output = &lcd_ctx->output_setting;
	output->type = panel->type;
	if(panel->type == LCD_TYPE_FSTN){
		output->height = panel->height / 8;
		output->width = panel->width / 2;
	} else {
		output->height = panel->height;
		output->width = panel->width;
	}
	output->interf = panel->interf;
	if(LCD_CAP_NOTE == (panel->cap & LCD_CAP_NOTE))
		output->te_en = 0;
	else{
		output->te_en = 1;
		output->te_pol = panel->te_pol;
	}
	if(output->interf == LCD_INTERFACE_SPI){
		spi = (struct spi_info *)panel->info;
		output->format = spi->format;		
		output->data_lane_num = spi->data_lane_num;
	} else {
		mcu = (struct mcu_info *)panel->info;
		output->format = mcu->format;
	}

	if(panel->type == LCD_TYPE_FSTN){
		if(output->interf == LCD_INTERFACE_SPI){
			output->format = SPI_FORMAT_RGB565;
		} else {
			output->format = MCU_FORMAT_RGB565;
		}
	}

	if((output->format == SPI_FORMAT_RGB565) && (panel->type != LCD_TYPE_FSTN))
		dither->dither_en = 1;
	else
		dither->dither_en = 0;
	dither->dither_mode = LCD_DITHER_4X8;

	LCDLOGI("INFO: lcd_output_setting ---\r\n");
	return 0;
}

int lcd_update_output_setting(struct s_lcd_ctx *lcd_ctx, uint16_t width, uint16_t height)
{
	struct s_output_setting *output = NULL;

	LCDLOGI("INFO: lcd_update_output_setting +++\r\n");

	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_update_output_setting, Invalid param!\r\n");
		return -1;
	}
	
	output = &lcd_ctx->output_setting;
	output->height = height;
	output->width = width;
	return 0;
}


#ifdef LCD_INTERRUPT_MODE
void lcd_enable_te_irq(struct s_lcd_ctx *lcd_ctx, int enable)
{
	uint32_t reg;

	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_enable_te_irq, Invalid param!\r\n");
		return;
	}

	reg = lcd_read(IRQ_ENA);
	if(0 != enable)
		reg |= IRQ_TE_DONE;
	else
		reg &= ~IRQ_TE_DONE;
	lcd_write(IRQ_ENA, reg);
}
#endif

void lcd_uninit(struct s_lcd_ctx *lcd_ctx)
{
	if(NULL == lcd_ctx){
		LCDLOGE("ERROR: lcd_uninit, Invalid param!\r\n");
		return;
	}

	if(g_lcd_drv_status == LCD_DRV_STATUS_UNINIT){
		LCDLOGI("INFO: lcd_uninit: lcd driver has been uninited!!\r\n");
		return;
	}

#ifdef LCD_INTERRUPT_MODE
	if(LCD_WORK_MODE_INTERRUPT == lcd_ctx->work_mode){
		lcd_irq_dis();
		UOS_DeleteFlag(g_lcd_interrupt_flag);
	g_lcd_interrupt_flag = INVALID_FLAG_ID;
	}
#endif
	g_lcd_drv_status = LCD_DRV_STATUS_UNINIT;
}

