#include "../cam_fe.h"
#include "../cam_tuning.h"
#include "cam_pipeline_reg.h"

/* close CAMLOGD */
#undef CAMLOGD
#define CAMLOGD(fmt, args...) do {} while(0)

#define REG_ISP_SPI_MODE1 0x78
#define SPI_MANUAL_MODE_MASK (0x5 << 4)
#define SPI_MANUAL_MODE_EN (0x1 << 3)
#define SPI_MANUAL_HEIGHT_EN (0x1 << 2)
#define SPI_MANUAL_WIDTH_EN (0x1 << 1)
#define SPI_IGNORE_LINE_ID_EN (0x1 << 0)

#define REG_ISP_SPI_MODE2 0x7C
#define SPI_DATA_SWITCH_MASK (0x3 << 4)
#define SPI_NEG_SAMPLE_EN (0x1 << 3)
#define SPI_DDR_MODE_EN (0x1 << 2)
#define SPI_LANE_NUM_MASK (0x3 << 0)
#define LN_SPI1_VAL (0x0)
#define LN_SPI2_VAL (0x1)
#define LN_SPI4_VAL (0x3)

#define REG_ISP_OUT_FMT 0x210
#define INPUT_FMT_RAW 0x00
#define INPUT_FMT_YUV 0x15

#define REG_ISP_TOP_MODE 0x218
#define SPI_CRC_EN (0x1 << 2)
#define SPI_EN (0x1 << 1)
#define SPI_DIS (0x0 << 1)
#define INPUT_SEL_SPI (0x1 << 0)
#define INPUT_SEL_DVP (0x0 << 0)

#define REG_ISP_AEC_ENABLE 0x2D8
#define REG_ISP_AEC_TARGET 0x44C
#define REG_ISP_AEC_STEP 0x498
#define REG_ISP_SATURATION 0xB40
#define REG_ISP_CONTRAST 0xB4C

static struct isp_tuning_tab gamma[] = {
	{0x840 ,0x0d},
	{0x844 ,0x12},
	{0x848 ,0x17},
	{0x84C ,0x1c},
	{0x850 ,0x27},
	{0x854 ,0x34},
	{0x858 ,0x44},
	{0x85C ,0x55},
	{0x860 ,0x6e},
	{0x864 ,0x81},
	{0x868 ,0x91},
	{0x86C ,0x9c},
	{0x870 ,0xaa},
	{0x874 ,0xbb},
	{0x878 ,0xca},
	{0x87C ,0xd5},
	{0x880 ,0xe0},
	{0x884 ,0xe7},
	{0x888 ,0xed},
	{0x88C ,0xf6},
	{0x890 ,0xfb},
	//{0x894 ,0xff},
};

static void isp_cisctl_win_set(uint32_t sensor_w, uint32_t sensor_h)
{
	isp_reg_write(0x34, (sensor_h >> 8) & 0x7);   // CISCTL_win_height
	isp_reg_write(0x38, sensor_h & 0xff);	 // CISCTL_win_height
	isp_reg_write(0x3c, (sensor_w >> 8) & 0x7);   // CISCTL_win_width
	isp_reg_write(0x40, sensor_w & 0xff);	 // CISCTL_win_width
	
	CAMLOGD("sensor width x height = %d x %d", sensor_w, sensor_h);
}

static void isp_output_win_crop(uint32_t start_x, uint32_t end_x, uint32_t start_y, uint32_t end_y)
{
	uint32_t isp_out_width, isp_out_height;

	isp_out_width = end_x - start_x;
	isp_out_height = end_y - start_y;

	isp_reg_write(0x244, (start_y >> 8) & 0x7);	  // crop start y hight
	isp_reg_write(0x248, start_y & 0xff);		     // crop start y low
	isp_reg_write(0x24c, (start_x >> 8) & 0x7);	  //crop start x hight
	isp_reg_write(0x250, start_x & 0xff);		     // crop start x low
	isp_reg_write(0x254, (isp_out_height >> 8) & 0x7);   // out window height
	isp_reg_write(0x258, isp_out_height & 0xff);	 // out window height
	isp_reg_write(0x25c, (isp_out_width >> 8) & 0x7);    // out window width
	isp_reg_write(0x260, isp_out_width & 0xff);	  // out window width

	camera_reg_write(REG_ISP_IMG_SIZE, isp_out_width + (isp_out_height << 16));   // image size from isp

	CAMLOGD("crop window [%d, %d] [%d, %d]", start_x, start_y, end_x, end_y);
}

static int isp_input_pattern(uint32_t mbus_code)
{
	int ret = 0;
	uint8_t is_yuv = 0;

	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SBGGR10_1X10:
		isp_reg_write(0x5c, 0x20);   // [5:4] CFA) =  00:GRBG) =  01:RGGB) =  10:BGGR) =  11:GBRG
		break;
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
		isp_reg_write(0x5c, 0x30);   // [5:4] CFA) =  00:GRBG) =  01:RGGB) =  10:BGGR) =  11:GBRG
		break;
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
		isp_reg_write(0x5c, 0x00);   // [5:4] CFA) =  00:GRBG) =  01:RGGB) =  10:BGGR) =  11:GBRG
		break;
	case MEDIA_BUS_FMT_SRGGB8_1X8:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		isp_reg_write(0x5c, 0x10);   // [5:4] CFA) =  00:GRBG) =  01:RGGB) =  10:BGGR) =  11:GBRG
		break;
	case MEDIA_BUS_FMT_YUYV8_2X8:
		camera_write_mask(REG_DATA_FORMAT_CTRL, (0x0 << 24), MASK_(24, 2));
		is_yuv = 1;
		break;
	case MEDIA_BUS_FMT_YVYU8_2X8:
		camera_write_mask(REG_DATA_FORMAT_CTRL, (0x1 << 24), MASK_(24, 2));
		is_yuv = 1;
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
		camera_write_mask(REG_DATA_FORMAT_CTRL, (0x2 << 24), MASK_(24, 2));
		is_yuv = 1;
		break;
	case MEDIA_BUS_FMT_VYUY8_2X8:
		camera_write_mask(REG_DATA_FORMAT_CTRL, (0x3 << 24), MASK_(24, 2));	
		is_yuv = 1;
		break;
	default:
		CAMLOGE("invalid yuv422 pattern 0x%x", mbus_code);
		return -EINVAL;
	}

	if (is_yuv)
		isp_reg_write(REG_ISP_OUT_FMT, INPUT_FMT_YUV);
	else
		isp_reg_write(REG_ISP_OUT_FMT, INPUT_FMT_RAW);
		
	return ret;
}

void fe_isp_reg_init(SENSOR_CTRL_T *sensor_ctrl)
{
	loading_tuning_data(sensor_ctrl);
	isp_reg_write(0x234, 0x03); // debug_mode3
}

/*config input type,input size,output size,tuning data*/
int fe_isp_precess_config(struct isp_param* isp_cfg,SENSOR_CTRL_T *sensor_ctrl)
{
	int ret = 0;
	uint16_t mbus_code = isp_cfg->snr_mbus_code;

	isp_cisctl_win_set(isp_cfg->isp_input_width, isp_cfg->isp_input_height);

	ret = isp_input_pattern(mbus_code);
	if (ret)
		return ret;

	isp_output_win_crop(isp_cfg->isp_crop.start_x, isp_cfg->isp_crop.end_x,
					isp_cfg->isp_crop.start_y, isp_cfg->isp_crop.end_y);

#ifdef ISP_COLOR_BAR
	isp_reg_write(0x230,   0x04);
	isp_reg_write(0x234,   0x2);    //0x5a:red   0x7a:blue  0x02:color bar
#endif

	return 0;
}

/*bypass isp for raw dump*/
void fe_isp_bypass(uint_t bypass)
{
	if(bypass){
		isp_reg_write(0x210,   0x01);
		CAMLOGI("bypass isp for raw_dump");
	} else {
		isp_reg_write(0x210,   0x00);
		CAMLOGI("enable isp");
	}
}


/**
 * @brief config spi mtk mode, in which sensor output data with
 *		package information
 *
 * @return  jpeg size
 */
static int spi_mtk_mode_set(uint16_t fifo_trig_num, uint16_t manual_w, uint16_t manual_h, SENSOR_DATA_T *sensor_data)
{
	int ret = 0;
	uint16_t ln_val;
	uint16_t lane_num = 0, spi_sdr_en = 0, spi_crc_en = 0;

	lane_num = sensor_data->lane_num;
	spi_sdr_en = sensor_data->spi_sdr;
	spi_crc_en = sensor_data->spi_crc;
	CAMLOGD("fifo triger = 0x%x, manual_w = %d, manual_h = %d, lane_num = %d",
		fifo_trig_num, manual_w, manual_h, lane_num);

	switch (lane_num) {
	case SPI_1_LAN:
		ln_val = LN_SPI1_VAL;
		break;
	case SPI_2_LAN:
		ln_val = LN_SPI2_VAL;
		break;
	case SPI_4_LAN:
		ln_val = LN_SPI4_VAL;
		break;
	default:
		CAMLOGE("spi lane%d not support", lane_num);
		return -EINVAL;
	}

	isp_reg_write(0x044, (fifo_trig_num >> 8) & 0xFF);	// SPI trig num
	isp_reg_write(0x048, (fifo_trig_num & 0xFF));
#if 0
	/* working mode: BT565/Spreadtrum (packaged) */
	isp_write_mask(REG_ISP_SPI_MODE1, 0x0, SPI_MANUAL_MODE_MASK);
	isp_set_bit(REG_ISP_SPI_MODE1, SPI_MANUAL_MODE_EN | SPI_MANUAL_HEIGHT_EN | SPI_MANUAL_WIDTH_EN);
	isp_reg_write(0x084, (manual_w >> 8) & 0x7);   // SPI_manual_width=1280 (in bytes)) =  = image_width*2	when YUV422 mode
	isp_reg_write(0x088, manual_w & 0xff);
	isp_reg_write(0x08c, (manual_h >> 8) & 0x7);   // SPI_manual_height=80
	isp_reg_write(0x090, manual_h & 0xff);
#else
	/* working mode: MTK mode  */
	isp_clr_bit(REG_ISP_SPI_MODE1, SPI_MANUAL_MODE_EN | SPI_MANUAL_HEIGHT_EN | SPI_MANUAL_WIDTH_EN);
#endif
	isp_set_bit(REG_ISP_SPI_MODE1, SPI_IGNORE_LINE_ID_EN);

	isp_write_mask(REG_ISP_SPI_MODE2, ln_val, SPI_LANE_NUM_MASK);
	isp_write_mask(REG_ISP_SPI_MODE2, (ln_val << 4), SPI_DATA_SWITCH_MASK);

	if (spi_sdr_en) {
		isp_clr_bit(REG_ISP_SPI_MODE2, SPI_NEG_SAMPLE_EN);
		isp_set_bit(REG_ISP_SPI_MODE2, SPI_DDR_MODE_EN);
	} else {
		isp_set_bit(REG_ISP_SPI_MODE2, SPI_NEG_SAMPLE_EN);
		isp_clr_bit(REG_ISP_SPI_MODE2, SPI_DDR_MODE_EN);
	}

	isp_reg_write(REG_ISP_TOP_MODE, SPI_EN | INPUT_SEL_SPI);
	if (spi_crc_en) {
		isp_set_bit(REG_ISP_TOP_MODE, SPI_CRC_EN);
	} else {
		isp_clr_bit(REG_ISP_TOP_MODE, SPI_CRC_EN);
	}

	return ret;
}

int spi_config(SENSOR_DATA_T *sensor_data)
{
	int ret = 0;
	uint16_t mbus_code, spi_fifo = 0;
	uint16_t img_w, img_h, manual_w, manual_h;
	uint32_t dvp_if = 0;

	mbus_code = sensor_data->res[sensor_data->cur_res_index].mbus_code;
	img_w = sensor_data->res[sensor_data->cur_res_index].width;
	img_h = sensor_data->res[sensor_data->cur_res_index].height;

	CAMLOGD("mbus_code = 0x%x %dx%d", mbus_code, img_w, img_h);

	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		manual_w = img_w;
		manual_h = img_h;
		spi_fifo = 0xa0;
		dvp_if = ((img_w & 0x7FF) << 16) | 0xC080;
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		manual_w = img_w * 2;
		manual_h = img_h;
		spi_fifo = 0x138;
		// [27]:ISP_OUT_422 '1' valid, <p>=1: use yuv422 from isp
		// george: when isp reg 0x210[4] is_yuv422
		dvp_if = BIT(27) | ((img_w & 0x7FF) << 16) | 0xC080;
		break;
	default:
		CAMLOGE("invalid mbus fmt 0x%x", mbus_code);
		return -EINVAL;
	}

	ret = spi_mtk_mode_set(spi_fifo, manual_w, manual_h, sensor_data);
	if (ret)
		CAMLOGE("spi config failed!");
	camera_reg_write(REG_ISP_DVP_IF_CTRL, dvp_if);

	return ret;
}

static int mipi_csi2_enable(uint16_t lane_num)
{
	if (lane_num != MIPI_1_LAN) {
		CAMLOGE("mipi lane%d not support", lane_num);
		return -EINVAL;
	}

	camera_reg_write(REG_CSI2_CTRL_0, 0x1);		// [0] csi2_en
	camera_reg_write(REG_CSI2_DPHY1, 0x1);		// DPHY 1 ana pu
	camera_reg_write(REG_CSI2_DPHY2, 0xa2840080);   // DPHY 2
	camera_reg_write(REG_CSI2_DPHY3, 0x1500);       // DPHY 3 hs_term_en/hs_settle
	camera_reg_write(REG_CSI2_DPHY5, 0x11);		// DPHY 5 lane_en

	return 0;
}

static int mipi_csi2_disable(void)
{
	camera_reg_write(REG_CSI2_CTRL_0, 0x0);   // [0] csi2_en  disable csi2

	return 0;
}


int mipi_config(SENSOR_DATA_T *sensor_data)
{
	int ret = 0;
	uint16_t mbus_code = 0;
	uint16_t img_w;
	uint32_t dvp_if = 0;

	mbus_code = sensor_data->res[sensor_data->cur_res_index].mbus_code;
	img_w = sensor_data->res[sensor_data->cur_res_index].width;

	CAMLOGD("mbus_code = 0x%x %dx%d", mbus_code, img_w, sensor_data->res[sensor_data->cur_res_index].height);

	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		dvp_if = BIT(28) | ((img_w & 0x7FF) << 16) | 0xC080;
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		dvp_if = ((img_w & 0x7FF) << 16) | 0xC080;
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		dvp_if = BIT(31) | ((img_w & 0x7FF) << 16) | 0xC080; // [31]:BYPASS GC ISP '1' valid, dma use mipi data directly
		break;
	default:
		CAMLOGE("invalid mbus fmt 0x%x", mbus_code);
		return -EINVAL;
	}

	ret = mipi_csi2_enable(sensor_data->lane_num);
	if (ret)
		CAMLOGE("mipi config failed!");
	camera_reg_write(REG_ISP_DVP_IF_CTRL, dvp_if);

	return ret;
}


int parallel_config(SENSOR_DATA_T *sensor_data)
{
	int ret = 0;
	uint16_t mbus_code;
	uint16_t img_w;
	uint32_t dvp_if = 0;

	mbus_code = sensor_data->res[sensor_data->cur_res_index].mbus_code;
	img_w = sensor_data->res[sensor_data->cur_res_index].width;

	CAMLOGD("mbus_code = 0x%x %dx%d", mbus_code, img_w, sensor_data->res[sensor_data->cur_res_index].height);

	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		dvp_if = BIT(28) | ((img_w & 0x7FF) << 16) | 0xC080;
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		dvp_if = ((img_w & 0x7FF) << 16) | 0xC080;
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		dvp_if = BIT(31) | ((img_w & 0x7FF) << 16) | 0xC080; // [31]:BYPASS GC ISP '1' valid, dma use mipi data directly
		break;
	default:
		CAMLOGE("invalid mbus fmt 0x%x", mbus_code);
		return -EINVAL;
	}

	mipi_csi2_disable();
	camera_reg_write(REG_ISP_DVP_IF_CTRL, dvp_if);

	return ret;
}

int cam_ccic_config(SENSOR_CTRL_T *sensor_ctrl)
{
	int inf_type, ret = 0;

	if (NULL == sensor_ctrl || NULL == sensor_ctrl->sensor_data) {
		CAMLOGE("sensor para is NULL");
		return -EINVAL;
	}

	inf_type = sensor_ctrl->sensor_data->interface_type;

	switch (inf_type){
		case CAM_INF_SPI:
			ret = spi_config(sensor_ctrl->sensor_data);
			break;
		case CAM_INF_MIPI:
			ret = mipi_config(sensor_ctrl->sensor_data);
			break;
		case CAM_INF_PARALLEL:
			ret = parallel_config(sensor_ctrl->sensor_data);
			break;
		default:
			CAMLOGE("invalid interface %d", inf_type);
			ret = -EINVAL;
	}
	cam_isp_tuning_file_analyze();
	return ret;
}

int isp_ctl_set_contrast(uint_16 level, struct tuning_param *default_param)
{
	int ret = 0;
	uint_32 value = 0;
	
	CAMLOGI("isp_ctl_set_contrast %d", level);

	if(!default_param->contrast.read_flag){
		default_param->contrast.tuning_set_value = isp_reg_read(REG_ISP_CONTRAST);
		default_param->contrast.read_flag = 1;
	}

	if(level > HAL_CONTRAST_p3){
		CAMLOGE("invalid contrast level %d", level);
		return EINVAL;
	}
	level +=1;
	value = ((float) level / (HAL_CONTRAST_mid + 1)) * default_param->contrast.tuning_set_value;
	CAMLOGI("isp_ctl_set_contrast value= 0x%x default =0x%x", value,default_param->contrast.tuning_set_value);
	isp_reg_write(REG_ISP_CONTRAST, value);

	return ret;
}

int isp_ctl_set_saturation(uint_16 level, struct tuning_param *default_param)
{
	int ret = 0;
	uint_32 value = 0;
	
	CAMLOGI("isp_ctl_set_saturation %d", level);

	if(!default_param->saturation.read_flag){
		default_param->saturation.tuning_set_value = isp_reg_read(REG_ISP_SATURATION);
		default_param->saturation.read_flag = 1;
	}

	if(level > HAL_SATURATION_p3){
		CAMLOGE("invalid saturation level %d", level);
		return EINVAL;
	}
	level +=1;
	value = ((float) level / (HAL_SATURATION_mid + 1)) * default_param->saturation.tuning_set_value;
	CAMLOGI("isp_ctl_set_saturation value= 0x%x default =0x%x", value,default_param->saturation.tuning_set_value);
	isp_reg_write(REG_ISP_SATURATION, value);

	return ret;
}

int isp_ctl_set_brightness(uint_16 level, struct tuning_param *default_param)
{
	int ret = 0;
	uint_32 value = 0;
	int i = 0;
	
	CAMLOGI("isp_ctl_set_brightness %d", level);

	if(!default_param->brightness.read_flag){
		for(i = 0;i < ARRAY_SIZE(gamma) ;i++){
			gamma[i].val = isp_reg_read(gamma[i].reg);
		}		
		default_param->brightness.read_flag = 1;
	}

	if(level > HAL_BRIGHTNESS_9){
		CAMLOGE("invalid brightness level %d", level);
		return EINVAL;
	}

	for(i = 0;i < ARRAY_SIZE(gamma) ;i++){
		value = BOUND((gamma[i].val * ((float) level / HAL_BRIGHTNESS_mid)), 0x00, 0xff);
		isp_reg_write(gamma[i].reg, value);
	}

	return ret;
}

int isp_ctl_set_wb(uint_16 level)
{
	int ret = 0;
	
	CAMLOGI("isp_ctl_set_wb mode %d", level);
	switch (level){
		case HAL_WB_AUTO:
			isp_write_mask(0x208, 0x02,0x02);  // awb enable
			break;
		case HAL_WB_DAYLIGHT:
			isp_write_mask(0x208, 0x00,0x02);
			isp_reg_write(0x2cc, 0x60); //AWB R gain
			isp_reg_write(0x2d0, 0x40); //AWB G gain
			isp_reg_write(0x2d4, 0x58); //AWB B gain
			break;
		case HAL_WB_TUNGSTEN:
			isp_write_mask(0x208, 0x00,0x02);
			isp_reg_write(0x2cc, 0x50);
			isp_reg_write(0x2d0, 0x40);
			isp_reg_write(0x2d4, 0x50);
			break;
		case HAL_WB_OFFICE:
			isp_write_mask(0x208, 0x00,0x02);
			isp_reg_write(0x2cc, 0x5a);
			isp_reg_write(0x2d0, 0x40);
			isp_reg_write(0x2d4, 0x5a);
			break;
		case HAL_WB_CLOUDY:
			isp_write_mask(0x208, 0x00,0x02);
			isp_reg_write(0x2cc, 0x6a);
			isp_reg_write(0x2d0, 0x40);
			isp_reg_write(0x2d4, 0x5a);
			break;
		case HAL_WB_INCANDESCENT:
			isp_write_mask(0x208, 0x00,0x02);
			isp_reg_write(0x2cc, 0x50);
			isp_reg_write(0x2d0, 0x40);
			isp_reg_write(0x2d4, 0x6a);
			break;

		default:
			CAMLOGE("invalid wb mode %d", level);
			ret = -EINVAL;
	}
	return ret;

}

int isp_ctl_set_effect(uint_16 level)
{
	int ret = 0;
	
	CAMLOGI("isp_ctl_set_effect mode %d", level);
	switch (level){
		case HAL_EFFECT_NORMAL:
			isp_reg_write(0x20c, 0x00); // bit[1] enable special effect
			//isp_reg_write(0xb68, 0x00); //Cb
			//isp_reg_write(0xb6c, 0x00); //Cr
			break;
		case HAL_EFFECT_COLORINV:
			isp_reg_write(0x20c, 0x01);
			break;
		case HAL_EFFECT_BLACKBOARD:
			isp_reg_write(0x20c, 0x09);
			break;
		case HAL_EFFECT_WHITEBOARD:
			isp_reg_write(0x20c, 0x08);
			break;
		case HAL_EFFECT_ANTIQUE:
			isp_reg_write(0x20c, 0x82);
			break;
		case HAL_EFFECT_RED:
			isp_reg_write(0x20c, 0x42);
			break;
		case HAL_EFFECT_GREEN:
			isp_reg_write(0x20c, 0x52);
			break;
		case HAL_EFFECT_BLUE:
			isp_reg_write(0x20c, 0x62);
			break;
		case HAL_EFFECT_BLACKWHITE:
			isp_reg_write(0x20c, 0x12);
			break;
		case HAL_EFFECT_NEGATIVE:
			isp_reg_write(0x20c, 0x03);
			break;

		default:
			CAMLOGE("invalid effect mode %d", level);
			ret = -EINVAL;
	}
	return ret;

}

int isp_ctl_set_banding(uint_16 level, SENSOR_CTRL_T *sensor_ctrl )
{
	int ret = 0, index = 0;

	struct crane_isp_tuning_data tuning_data;
	memset(&tuning_data, 0, sizeof(tuning_data));

	CAMLOGI("isp_ctl_set_banding %d", level);
	switch (level){
		case HAL_BANDING_AUTO:
			index = 0;
			break;
		case HAL_BANDING_50HZ:
			index = 0;
			break;
		case HAL_BANDING_60HZ:
			index = 1;
			break;
		default:
			index = 0;
			CAMLOGE("invalid flash mode %d", level);
			ret = -EINVAL;
	}
	
	sensor_ctrl->ops->get_banding_tab(sensor_ctrl, &tuning_data, index);
	if (!tuning_data.tab) {
		CAMLOGW("tuning tab not found!");
	} else {
		CAMLOGI("write %s[%s] tuning tab!", sensor_ctrl->sensor_data->name,index?"60hz":"50hz");
		isp_reg_tab_write(&tuning_data);
	}

	return ret;

}

#if 0
void dvp_inf_spi_config(uint16_t mbus_code, uint16_t img_w)
{
	CAMLOGD("sensor_format = 0x%x isp_outputw:%d", mbus_code, img_w);

	/* spi->dvpinf straight through */
	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		// george: when isp reg 0x210[4] is_yuv422
		camera_reg_write(REG_ISP_DVP_IF_CTRL, 1 << 27); // [27]:ISP_OUT_422 '1' valid, <p>=1: use yuv422 from isp
		break;
	default:
		CAMLOGE("invalid sensor fmt");
		return;
	}
}

void dvp_inf_mipi_config(uint16_t mbus_code, uint16_t img_w)
{
	CAMLOGD("sensor_format = 0x%x isp_outputw:%d", mbus_code, img_w);

	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		camera_clr_bit(REG_ISP_DVP_IF_CTRL, 1 << 31);				  // [31]:through GC ISP
		camera_write_mask(REG_ISP_DVP_IF_CTRL, (img_w << 16) + 0xC080, 0x7FFFFFF);   // [7:0] min hblank [26:16] dvp fifo trigger
		camera_set_bit(REG_ISP_DVP_IF_CTRL, 1 << 28);				  // [28]: Is RAW8 mode, data[7:0] from repack module will be feed in to [9:2], and low 2 bits stuffing 0.
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		camera_clr_bit(REG_ISP_DVP_IF_CTRL, 1 << 31);				  // [31]:through GC ISP
		camera_write_mask(REG_ISP_DVP_IF_CTRL, (img_w << 16) + 0xC080, 0x7FFFFFF);   // [7:0] min hblank [26:16] dvp fifo trigger
		camera_clr_bit(REG_ISP_DVP_IF_CTRL, 1 << 28);				  // [28]: not RAW8 mode
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		camera_set_bit(REG_ISP_DVP_IF_CTRL, 1 << 31);   // [31]:BYPASS GC ISP '1' valid, dma use mipi data directly
		// camera_reg_write(REG_ISP_DVP_IF_CTRL, 1 << 27); // [27]:ISP_OUT_422 '1' valid, <p>=1: use yuv422 from isp
		// when isp reg 0x210 bit4 is_yuv422
		break;
	default:
		CAMLOGE("invalid sensor fmt");
		return;
	}
}

void dvp_inf_parallel_config(uint16_t mbus_code, uint16_t img_w)
{
	CAMLOGD("sensor_format = 0x%x isp_outputw:%d", mbus_code, img_w);

	switch (mbus_code) {
	case MEDIA_BUS_FMT_SBGGR8_1X8:
	case MEDIA_BUS_FMT_SGBRG8_1X8:
	case MEDIA_BUS_FMT_SGRBG8_1X8:
	case MEDIA_BUS_FMT_SRGGB8_1X8:
		camera_clr_bit(REG_ISP_DVP_IF_CTRL, 1 << 31);				  // [31]:through GC ISP
		camera_write_mask(REG_ISP_DVP_IF_CTRL, (img_w << 16) + 0xC080, 0x7FFFFFF);   // [7:0] min hblank [26:16] dvp fifo trigger
		camera_set_bit(REG_ISP_DVP_IF_CTRL, 1 << 28);				  // [28]: Is RAW8 mode, data[7:0] from repack module will be feed in to [9:2], and low 2 bits stuffing 0.
		break;
	case MEDIA_BUS_FMT_SBGGR10_1X10:
	case MEDIA_BUS_FMT_SGBRG10_1X10:
	case MEDIA_BUS_FMT_SGRBG10_1X10:
	case MEDIA_BUS_FMT_SRGGB10_1X10:
		camera_clr_bit(REG_ISP_DVP_IF_CTRL, 1 << 31);				  // [31]:through GC ISP
		camera_write_mask(REG_ISP_DVP_IF_CTRL, (img_w << 16) + 0xC080, 0x7FFFFFF);   // [7:0] min hblank [26:16] dvp fifo trigger
		camera_clr_bit(REG_ISP_DVP_IF_CTRL, 1 << 28);				  // [28]: not RAW8 mode
		break;
	case MEDIA_BUS_FMT_UYVY8_2X8:
	case MEDIA_BUS_FMT_VYUY8_2X8:
	case MEDIA_BUS_FMT_YUYV8_2X8:
	case MEDIA_BUS_FMT_YVYU8_2X8:
		camera_set_bit(REG_ISP_DVP_IF_CTRL, 1 << 31);   // [31]:BYPASS GC ISP '1' valid, dma use mipi data directly
		// camera_reg_write(REG_ISP_DVP_IF_CTRL, 1 << 27); // [27]:ISP_OUT_422 '1' valid, <p>=1: use yuv422 from isp
		// when isp reg 0x210 bit4 is_yuv422
		break;
	default:
		CAMLOGE("invalid sensor fmt");
		return;
	}
}
#endif
