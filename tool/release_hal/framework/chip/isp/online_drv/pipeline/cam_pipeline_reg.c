#include "../cam_pipeline.h"
#include "cam_pipeline_reg.h"
#include "../../camera_common_log.h"

#define ALIGN_TO(x, iAlign) ( (((unsigned int)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

uint_32 camera_reg_read(uint_32 reg)
{
	return BUREG_READ(REG_CAMERA_BASE + reg);
}

void camera_reg_write(uint_32 reg, uint_32 val)
{
    uint_32 read_value = 0;

    BUREG_WRITE(REG_CAMERA_BASE + reg, val);
    if (0x50 == reg || 0x30 == reg) {
        return;
    }

//    CAMLOGV("camera_reg_write_debug  reg = 0x%x   val = 0x%x", reg, val);
    read_value = camera_reg_read(reg);
    if (val != read_value) {
        CAMLOGI("camera_reg_write  reg = 0x%x   write_val = 0x%x   read_value = 0x%x", reg, val, read_value);
    }
}

void pmu_reg_write(uint_32 reg, uint_32 val)
{
	BUREG_WRITE(AP_PMU_BASE + reg, val);
}


void camera_write_mask(uint_32 reg, uint_32 val, uint_32 mask)
{
	uint_32 v;

	v = camera_reg_read(reg);
	v = (v & ~mask) | (val & mask);
	camera_reg_write(reg, v);
}

void camera_set_bit(uint_32 reg, uint_32 val)
{
	camera_write_mask(reg, val, val);
}

void camera_clr_bit(uint_32 reg, uint_32 val)
{
	camera_write_mask(reg, 0, val);
}

uint_8 isp_reg_read(uint_32 reg)
{
	return BUREG_READ(ISP_REG_BASE + reg);
}

void isp_reg_write(uint_32 reg, uint_32 val)
{
	uint_32 read_value = 0;
    val &= 0xff;
    //CAMLOGV("isp_reg_write_debug  reg = 0x%x   val = 0x%x ", reg, val);

    BUREG_WRITE(ISP_REG_BASE + reg, val);

    read_value = isp_reg_read(reg);
    if (val != read_value) {
        ;//CAMLOGE("isp_reg_write  error   reg = 0x%x   write_val = 0x%x   read_value = 0x%x", reg, val, read_value);
    }
}


void isp_reg_tab_write( struct crane_isp_tuning_data *regs)
{
	int i = 0;
	int rtn = 0;

	for (i = 0; i < regs->num; i++) {
		BUREG_WRITE(ISP_REG_BASE + regs->tab[i].reg, regs->tab[i].val);
		if (rtn < 0) {
			CAMLOGE("isp_reg_tab_write: Error!!  reg = 0x%x  val = 0x%x", regs->tab[i].reg, regs->tab[i].val);
			return;
		}
	}

	return;
}


void isp_write_mask(uint_32 reg, uint_32 val, uint_32 mask)
{
	uint_32 v;

	v = isp_reg_read(reg);
	v = (v & ~mask) | (val & mask);
	isp_reg_write(reg, v);
}

void isp_set_bit(uint_32 reg, uint_32 val)
{
	isp_write_mask(reg, val, val);
}

void isp_clr_bit(uint_32 reg, uint_32 val)
{
	isp_write_mask(reg, 0, val);
}

void isp_calc_exp(uint_32 min_fps, uint_32 max_fps, CAM_BANDING banding)
{
	uint_8 step_low = 0, step_high = 0;
	uint_32 step = 0;
	uint_32 min_exp = 0, max_exp = 0;
	uint_32 min_exp_l = 0, min_exp_h = 0;
	uint_32 max_exp_l = 0, max_exp_h = 0;

	CAMLOGV("set_fps [%d-%d] banding=%d ",min_fps, max_fps, banding);
	if(min_fps > max_fps){
		CAMLOGE("set_fps param error,min_fps must small or equal than max_fps ");
		return;
	}
	if(0 == min_fps)
		min_fps =1;
	if(0 == max_fps)
		max_fps =1;

	step_high = isp_reg_read(0x0494);  // exp line(10ms)
	step_low = isp_reg_read(0x0498);

	step = ((step_high & 0x1f) << 0x8) | step_low;

	min_exp = 100 / (float)max_fps * step;
	max_exp = 100 / (float)min_fps * step;
	CAMLOGV("set_fps step =0x%x min_exp = 0x%x max_exp = 0x%x",step, min_exp,max_exp);

	min_exp_l = min_exp & 0xff;
	min_exp_h = (min_exp >> 0x8) & 0xff;

	max_exp_l = max_exp & 0xff;
	max_exp_h = (max_exp >> 0x8) & 0xff;

	isp_reg_write( 0x049C ,max_exp_h);
	isp_reg_write( 0x04A0 ,max_exp_l );//exp  level 1
	isp_reg_write( 0x04A4 ,max_exp_h);
	isp_reg_write( 0x04A8 ,max_exp_l + 0x1 );//exp  level 2
	isp_reg_write( 0x04AC ,max_exp_h);
	isp_reg_write( 0x04B0 ,max_exp_l + 0x2 );//exp  level 3
	isp_reg_write( 0x04B4 ,max_exp_h);
	isp_reg_write( 0x04B8 ,max_exp_l + 0x3 );//exp  level 4
	isp_reg_write( 0x04BC ,max_exp_h);
	isp_reg_write( 0x04C0 ,max_exp_l + 0x4 );//exp  level 5
	isp_reg_write( 0x04C4 ,max_exp_h);
	isp_reg_write( 0x04C8 ,max_exp_l + 0x5 );//exp  level 6
	isp_reg_write( 0x04CC ,max_exp_h);
	isp_reg_write( 0x04D0 ,max_exp_l + 0x6 );//exp  level 7

	if( 30 > max_fps ){
		isp_reg_write( 0x04f0 ,min_exp_h); // max_exp_level[6:5] set 0, MaxLevel=1
		isp_reg_write( 0x04f4 ,min_exp_l );//min_exp [12:0]
	}

}

void isp_get_aec(uint_32 *exp_high, uint_32 *exp_low, uint_32 *gain, uint_32 *aec_en, uint_32 *exp_step)
{
	//isp_reg_write(0x00b4,0x1e); //close isp i2c master

	*exp_high =  isp_reg_read(0x000c);
	*exp_low =  isp_reg_read(0x0010);
	*gain = isp_reg_read(0x0094);
	*aec_en = isp_reg_read(0x02d8);
	*exp_step = (isp_reg_read(0x0494)<<8 ) | isp_reg_read(0x0498);

}

int pipeline_update_mac_addr(uint8_t pipeline_id, uint32_t *addr, uint32_t plane)
{
	int i;
	uint_32 mac_reg[3];

	switch (pipeline_id) {
	case PIPELINE_ID_PREV:
		mac_reg[0] = REG_Y0_BASE;
		mac_reg[1] = REG_U0_BASE;
		mac_reg[2] = REG_V0_BASE;
		break;
	case PIPELINE_ID_VIDEO:
		mac_reg[0] = REG_Y1_BASE;
		mac_reg[1] = REG_U1_BASE;
		mac_reg[2] = REG_V1_BASE;
		break;
	case PIPELINE_ID_CAP:
		mac_reg[0] = REG_Y2_BASE;
		mac_reg[1] = REG_U2_BASE;
		mac_reg[2] = REG_V2_BASE;
		break;
	default:
		CAMLOGE("invalid pipeline%d ", pipeline_id);
		return -1;
	}

	for (i = 0; i < plane; i++)
		camera_reg_write(mac_reg[i], addr[i]);

	return 0;
}

int cam_set_addr(uint_8 pipe_id, struct isp_qbuf_buffer buf)
{
	int i;
	uint32_t dmad[3];

	if (buf.num_planes > 3) {
		CAMLOGE("invalid plane number %d ", buf.num_planes);
		return -1;
	}

	for (i = 0; i < buf.num_planes; i++)
		dmad[i] = buf.planes[i].addr;

	pipeline_update_mac_addr(pipe_id, dmad, buf.num_planes);

	return 0;
}

/*sequence: pipe_crop-->subsample-->scaler*/
int cam_set_pipe_crop(struct pipeline_param* pipe_config)
{
	if (PIPELINE_ID_PREV == pipe_config->pipeline_id) {
		camera_reg_write(REG_PIP0_CROP_ROI_X, pipe_config->pipeline_crop.start_x + (pipe_config->pipeline_crop.end_x << 16)); // Pipe 0 ROI Y
		camera_reg_write(REG_PIP0_CROP_ROI_Y, pipe_config->pipeline_crop.start_y + (pipe_config->pipeline_crop.end_y << 16)); // Pipe 0 ROI Y
	} else if (PIPELINE_ID_VIDEO == pipe_config->pipeline_id) {
		camera_reg_write(REG_PIP1_CROP_ROI_X, pipe_config->pipeline_crop.start_x + (pipe_config->pipeline_crop.end_x << 16)); // Pipe 1 ROI X
		camera_reg_write(REG_PIP1_CROP_ROI_Y, pipe_config->pipeline_crop.start_y + (pipe_config->pipeline_crop.end_y << 16)); // Pipe 1 ROI Y
	}
	return 0;
}

int cam_set_pipe_subsmp(struct pipeline_param *pipe_config)
{
	uint_32 value = 0;
	uint_32 reg = 0;
	CAMLOGV("cam_set_pipe_subsmp:  pipeline%d   subsample = %d ", pipe_config->pipeline_id, pipe_config->subsample);

	if (PIPELINE_ID_PREV == pipe_config->pipeline_id) {
		reg = REG_SUBSAMPLE_SCALER_CTRL0;
	} else if (PIPELINE_ID_VIDEO == pipe_config->pipeline_id) {
		reg = REG_SUBSAMPLE_SCALER_CTRL1;
	}

	value = camera_reg_read(reg);
	value &= 0xFFFFFF8F;
	value += (pipe_config->subsample << 4);
	camera_reg_write(reg, value);
	return 0;
}

int cam_set_scaler(struct pipeline_param *pipe_config)
{
	uint_32 reg = 0;
	uint_32 value = 0;
	uint_8 scaler_cofe = 0xf;

	uint_16 crop_out_w = pipe_config->pipeline_crop.end_x-pipe_config->pipeline_crop.start_x;
	uint_16 crop_out_h = pipe_config->pipeline_crop.end_y-pipe_config->pipeline_crop.start_y;
	uint_16 subsmp_out_w = crop_out_w >> pipe_config->subsample;
	uint_16 subsmp_out_h = crop_out_h >> pipe_config->subsample;
	uint_16 scal_out_w = pipe_config->pipeline_outw;
	uint_16 scal_out_h = pipe_config->pipeline_outh;
	uint_16 scal_in_w = subsmp_out_w;
	uint_16 scal_in_h = subsmp_out_h;
	uint_16 scl_ratio = 0;

	uint_32 h_init_ph = 0;
	uint_32 h_delta_ph = 0;
	uint_32 v_init_ph = 0;
	uint_32 v_delta_ph = 0;


	scl_ratio = scal_out_w * 100 / scal_in_w;
	CAMLOGV("cam_set_scaler: scl output_width = %d  scl input width = %d    ratio = %d  %d", scal_out_w, subsmp_out_w, scl_ratio);

	if (scl_ratio > 97) {
		scaler_cofe = 0xf;    //scaler_100_coeff
	} else if (scl_ratio > 92) {
		scaler_cofe = 0xe;    //scaler_95_coeff
	} else if (scl_ratio > 87) {
		scaler_cofe = 0xd;    //scaler_90_coeff
	} else if (scl_ratio > 82) {
		scaler_cofe = 0xc;    //scaler_85_coeff
	} else if (scl_ratio > 77) {
		scaler_cofe = 0xb;    //scaler_80_coeff
	} else if (scl_ratio > 72) {
		scaler_cofe = 0xa;    //scaler_75_coeff
	} else if (scl_ratio > 67) {
		scaler_cofe = 0x9;    //scaler_70_coeff
	} else if (scl_ratio > 62) {
		scaler_cofe = 0x8;    //scaler_65_coeff
	} else if (scl_ratio > 62) {
		scaler_cofe = 0x7;    //scaler_60_coeff
	} else if (scl_ratio > 52) {
		scaler_cofe = 0x6;    //scaler_55_coeff
	} else if (scl_ratio > 52) {
		scaler_cofe = 0x5;    //scaler_50_coeff
	} else if (scl_ratio > 42) {
		scaler_cofe = 0x4;    //scaler_45_coeff
	} else if (scl_ratio > 37) {
		scaler_cofe = 0x3;    //scaler_40_coeff
	} else if (scl_ratio > 32) {
		scaler_cofe = 0x2;    //scaler_35_coeff
	} else if (scl_ratio > 27) {
		scaler_cofe = 0x1;    //scaler_30_coeff
	} else {
		scaler_cofe = 0x0;    //scaler_25_coeff
	}

	h_init_ph     =  (scal_in_w*16/scal_out_w+1)/2;
	h_delta_ph = (scal_in_w <<16)/scal_out_w;
	v_init_ph     = (scal_in_h*16/scal_out_h+1)/2;
	v_delta_ph = (scal_in_h <<16)/scal_out_h;

	CAMLOGV("cam_set_scaler: h_init_ph = 0x%x, h_delta_ph = 0x%x, v_init_ph = 0x%x, v_delta_ph = 0x%x, scal_out_w = %d, scal_out_h = %d",
	h_init_ph, h_delta_ph, v_init_ph, v_delta_ph, scal_out_w, scal_out_h);

	if (PIPELINE_ID_PREV == pipe_config->pipeline_id) {
		reg = REG_SUBSAMPLE_SCALER_CTRL0;

		camera_reg_write(REG_PIP0_SCALER_IN_SIZE, scal_in_w + (scal_in_h<<16)); // Pipe 0 SCALER IN SIZE
		camera_reg_write(REG_PIP0_SCALER_OUT_SIZE, scal_out_w + (scal_out_h<<16)); // Pipe 0 SCALER OUT SIZ
		camera_reg_write(REG_PIP0_SCALER_H_PHASE, h_delta_ph+(h_init_ph<<24)); // Pipe 0 SCALER H phase
		camera_reg_write(REG_PIP0_SCALER_V_PHASE, v_delta_ph+(v_init_ph<<24)); // Pipe 0 SCALER V phase
	} else if (PIPELINE_ID_VIDEO== pipe_config->pipeline_id) {
		reg = REG_SUBSAMPLE_SCALER_CTRL1;

		camera_reg_write(REG_PIP1_SCALER_IN_SIZE, scal_in_w + (scal_in_h<<16)); // Pipe 0 SCALER IN SIZE
		camera_reg_write(REG_PIP1_SCALER_OUT_SIZE, scal_out_w + (scal_out_h<<16)); // Pipe 0 SCALER OUT SIZ
		camera_reg_write(REG_PIP1_SCALER_H_PHASE, h_delta_ph+(h_init_ph<<24)); // Pipe 0 SCALER H phase
		camera_reg_write(REG_PIP1_SCALER_V_PHASE, v_delta_ph+(v_init_ph<<24)); // Pipe 0 SCALER V phase
	}

	value = camera_reg_read(reg);
	value &= 0xFFFFF0FF;
	value += (scaler_cofe << 8);
	camera_reg_write(reg, value);
	return 0;
}

int cam_set_pipe_outfmt(struct pipeline_param* pipe_config)
{
	uint_32 value = 0;
	uint_32 reg_value = camera_reg_read(REG_DATA_FORMAT_CTRL);

	/*
	0x0 422 8 bpp planar
	0x4 422 8 bpp packed
	0x5 420 8 bpp planar
	others Reserved 
	*/
	switch(pipe_config->output_format) {
	    /*
	    <p>0x0 =Y1CbY0Cr (default)  vyuy
	    <p>0x1 = Y1CrY0Cb             uyvy
	    <p>0x2 = CrY1CbY0               yuyv
	    <p>0x3 = CbY1CrY0               yvyu
	    */
	    case PIPE_OUT_FMT_YUV422_YUYV:
	        value += (0x02 << (26 + pipe_config->pipeline_id * 2));  //YUVENDFMT
	        value += (0x4 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    case PIPE_OUT_FMT_YUV422_YVYU:
	        value += (0x03 << (26 + pipe_config->pipeline_id * 2));  //YUVENDFMT
	        value += (0x4 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    case PIPE_OUT_FMT_YUV422_UYVY:
	        value += (0x01 << (26 + pipe_config->pipeline_id * 2));  //YUVENDFMT
	        value += (0x4 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    case PIPE_OUT_FMT_YUV422_VYUY:
	        value += (0x00 << (26 + pipe_config->pipeline_id * 2));  //YUVENDFMT
	        value += (0x4 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    case PIPE_OUT_FMT_YUV420_YV12:
	        value += (0x5 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        value += (0x1 << (2 + pipe_config->pipeline_id * 8));  //SEMI_UV_ENDFMT
	        break;
	    case PIPE_OUT_FMT_YUV420_I420:
	        value += (0x5 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        value += (0x0 << (2 + pipe_config->pipeline_id * 8));  //SEMI_UV_ENDFMT
	        break;
	    case PIPE_OUT_FMT_YUV420_NV12:
	        value += (0x1 << (0 + pipe_config->pipeline_id * 8));  //ISIM_420SP
	        value += (0x0 << (2 + pipe_config->pipeline_id * 8));  //SEMI_UV_ENDFMT
	        value += (0x5 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    case PIPE_OUT_FMT_YUV420_NV21:
	        value += (0x1 << (0 + pipe_config->pipeline_id * 8));  //ISIM_420SP
	        value += (0x1 << (2 + pipe_config->pipeline_id * 8));  //SEMI_UV_ENDFMT
	        value += (0x5 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    case PIPE_OUT_FMT_YUV422_3PLANAR:
	        value += (0x0 << (4 + pipe_config->pipeline_id * 8));  //YUVOUTFMT
	        break;
	    default:
	        break;
	}

	if (0 == pipe_config->pipeline_id) {
	    reg_value &= 0XF3FFFF80;
	} else if (1 == pipe_config->pipeline_id) {
	    reg_value &= 0XCFFF80FF;
	} else if (2 == pipe_config->pipeline_id) {
	    reg_value &= 0X3F80FFFF;
	}
	reg_value += value;
	camera_reg_write(REG_DATA_FORMAT_CTRL, reg_value);
	CAMLOGV("cam_set_pipe_outfmt:  ------pipeline%d   format value = 0x%x-----------", pipe_config->pipeline_id, value);
	return 0;
}

int cam_set_pipe_jpeg_mode(struct pipeline_param* pipe_config)
{
	if (PIPELINE_ID_PREV == pipe_config->pipeline_id) {
	    if (pipe_config->jpeg_mode) {
	        camera_set_bit(REG_DATA_FORMAT_CTRL, 1<<7);
	    } else {
	        camera_clr_bit(REG_DATA_FORMAT_CTRL, 1<<7);
	    }
	} else if (PIPELINE_ID_VIDEO== pipe_config->pipeline_id) {
	    if (pipe_config->jpeg_mode) {
	        camera_set_bit(REG_DATA_FORMAT_CTRL, 1<<15);
	    } else {
	        camera_clr_bit(REG_DATA_FORMAT_CTRL, 1<<15);
	    }
	} else if (PIPELINE_ID_CAP== pipe_config->pipeline_id) {
	    if (pipe_config->jpeg_mode) {
	        camera_set_bit(REG_DATA_FORMAT_CTRL, 1<<23);
	    } else {
	        camera_clr_bit(REG_DATA_FORMAT_CTRL, 1<<23);
	    }
	}
	return 0;
}

void pipeline_set_img_pitch(uint8_t pipeline_id, uint16_t pitch_y, uint16_t pitch_uv)
{
	uint32_t reg;

	switch (pipeline_id) {
	case PIPELINE_ID_PREV:
		reg = REG_PIP0_CCIC_IMG_PITCH;
		break;
	case PIPELINE_ID_VIDEO:
		reg = REG_PIP1_CCIC_IMG_PITCH;
		break;
	case PIPELINE_ID_CAP:
		reg = REG_PIP2_CCIC_IMG_PITCH;
		break;
	default:
		CAMLOGE("invalid pipeline%d ", pipeline_id);
		return;
	}
	camera_reg_write(reg, pitch_y + (pitch_uv << 16));
}

void pipeline_set_img_size(uint8_t pipeline_id, uint16_t width, uint16_t height)
{
	uint32_t reg;

	switch (pipeline_id) {
	case PIPELINE_ID_PREV:
		reg = REG_IMG_SIZE_PIP0;
		break;
	case PIPELINE_ID_VIDEO:
		reg = REG_IMG_SIZE_PIP1;
		break;
	case PIPELINE_ID_CAP:
		reg = REG_IMG_SIZE_PIP2;
		break;
	default:
		CAMLOGE("invalid pipeline%d ", pipeline_id);
		return;
	}
	camera_reg_write(reg, width + (height << 16));
}

int cam_set_pitch_dma_size(struct pipeline_param* pipe_config)
{
	uint16_t img_pitch_y, img_pitch_uv, img_width, img_height;

	if(pipe_config->dma_stride_y == 0) {
		pipe_config->dma_stride_y = pipe_config->pipeline_outw;
	}
	if(0 != pipe_config->dma_stride_y % 8) {
	    pipe_config->dma_stride_y = (( pipe_config->dma_stride_y/8)+1)*8;
	}

	if(pipe_config->jpeg_mode == 1) {
	    pipe_config->dma_stride_y = pipe_config->pipeline_outw;/*jpeg mode no used pitch.*/
	}
	CAMLOGV("pipeline%d   output_format = %d   y_pitch = %d, pipe_config->dma_width = %d, pipe_config->dma_height = %d",
		pipe_config->pipeline_id, pipe_config->output_format, pipe_config->dma_stride_y, pipe_config->pipeline_outw, pipe_config->pipeline_outh);

	switch (pipe_config->output_format) {
		case PIPE_OUT_FMT_YUV422_YUYV:
		case PIPE_OUT_FMT_YUV422_YVYU:
		case PIPE_OUT_FMT_YUV422_UYVY:
		case PIPE_OUT_FMT_YUV422_VYUY:
			img_pitch_y = pipe_config->dma_stride_y * 2;
			img_pitch_uv = pipe_config->dma_stride_y * 2;
			img_width = pipe_config->pipeline_outw * 2;
			img_height = pipe_config->pipeline_outh;
			break;
		case PIPE_OUT_FMT_YUV420_NV12:
		case PIPE_OUT_FMT_YUV420_NV21:
			img_pitch_y = pipe_config->dma_stride_y;
			img_pitch_uv = pipe_config->dma_stride_y;
			img_width = pipe_config->pipeline_outw;
			img_height = pipe_config->pipeline_outh;
			break;
		case PIPE_OUT_FMT_YUV420_I420:
		case PIPE_OUT_FMT_YUV420_YV12:
			img_pitch_y = pipe_config->dma_stride_y;
			img_pitch_uv = pipe_config->dma_stride_y / 2;
			img_width = pipe_config->pipeline_outw;
			img_height = pipe_config->pipeline_outh;
			break;
		default:
			CAMLOGE("invalid pipeline%d format%d", pipe_config->pipeline_id, pipe_config->output_format);
			return -1;
	}

	pipeline_set_img_pitch(pipe_config->pipeline_id, img_pitch_y, img_pitch_uv);
	pipeline_set_img_size(pipe_config->pipeline_id, img_width, img_height);

	return 0;
}

//0x0:no data range(yuv 0-255)   0x1:data range(Y¡ä range of [16..235] and U/V range of [16..240])
/*
data range convert:
output = ((input - 128) * coef1 ))>> 8 + 128  if input < 128
output = ((input - 128) * coef0 + 128)>> 8 + 128  if input >= 128
output = clip(bottom, top)
*/
int cam_config_data_range(struct pipeline_param *pipe_config)
{
	uint_8 u_top = 240;
	uint_8 u_bottom = 16;
	uint_8 uv_coef0 = 224;
	uint_8 uv_coef1 = 224;

	uint_8 y_top = 235;
	uint_8 y_bottom = 16;
	uint_8 y_coef0 = 214;
	uint_8 y_coef1 = 224;

	uint_32 value = 0;
	if (0 == pipe_config->data_range) {
		return 0;
	}
	if (PIPELINE_ID_PREV == pipe_config->pipeline_id) {
		camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL0, 1<<14);   //pipeline0 range enable
		value = (u_top & 0xff) << 24;
		value |= (u_bottom & 0xff) << 16;
		value |= (y_top & 0xff) << 8;
		value |= y_bottom & 0xff;
		camera_reg_write(REG_DATA_RANGE_CTRL0, value);  //pipeline0 y,u top and bottom

		value = (y_coef1 & 0xff) << 16;
		value |= y_coef0 & 0xff;
		camera_reg_write(REG_DATA_RANGE_CTRL2, value);  //pipeline0 y coef
		value = (uv_coef1 & 0xff) << 16;
		value |= uv_coef0 & 0xff;
		camera_reg_write(REG_DATA_RANGE_CTRL3, value);  //pipeline0 u coef
	} else if (PIPELINE_ID_VIDEO== pipe_config->pipeline_id) {
		camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL1, 1<<14); //pipeline1 range enable
		value = (u_top & 0xff) << 24;
		value |= (u_bottom & 0xff) << 16;
		value |= (y_top & 0xff) << 8;
		value |= y_bottom & 0xff;
		camera_reg_write(REG_DATA_RANGE_CTRL1, value);  //pipeline1 y,u top and bottom

		value = (y_coef1 & 0xff) << 16;
		value |= y_coef0 & 0xff;
		camera_reg_write(REG_DATA_RANGE_CTRL4, value);  //pipeline1 y coef
		value = (uv_coef1 & 0xff) << 16;
		value |= uv_coef0 & 0xff;
		camera_reg_write(REG_DATA_RANGE_CTRL5, value);  //pipeline1 u coef
	}
	return 0;
}

void cam_set_pipe_shadow_ready(uint_8 pipe_id)
{
	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_3){
		if(pipe_id == PIPELINE_ID_PREV) {
	        	camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL0, 1 << 13);
		} else if(pipe_id == PIPELINE_ID_VIDEO) {
	        	camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL1, 1 << 13);
		} else if (pipe_id == PIPELINE_ID_CAP) {
			camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL2, 1 << 6);
		}
	}else{
		if(pipe_id == PIPELINE_ID_PREV) {
        		camera_set_bit(REG_SUBSAMPLE_SCALER_CTRL0, 1 << 13);
		}
	}
}

int cam_set_pipe_shadow_mode(uint8_t pipeline_id, uint8_t shadow)
{
	uint32_t reg = 0;
	uint32_t mask = 0;
	uint32_t val = 0;

	if (CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_3)
		return 0;

	switch (pipeline_id) {
		case PIPELINE_ID_PREV:
			reg = REG_SUBSAMPLE_SCALER_CTRL0;
			mask = PIPE0_CTRL_SHADOW_MODE_MASK;
			val = shadow ? PIPE0_CTRL_SHADOW : PIPE0_CTRL_DIRECT;
			break;
		case PIPELINE_ID_VIDEO:
			reg = REG_SUBSAMPLE_SCALER_CTRL1;
			mask = PIPE1_CTRL_SHADOW_MODE_MASK;
			val = shadow ? PIPE1_CTRL_SHADOW : PIPE1_CTRL_DIRECT;
			break;
		case PIPELINE_ID_CAP:
			reg = REG_SUBSAMPLE_SCALER_CTRL2;
			mask = PIPE2_CTRL_SHADOW_MODE_MASK;
			val = shadow ? PIPE2_CTRL_SHADOW : PIPE2_CTRL_DIRECT;
			break;
		default:
			CAMLOGE("invalid pipeline num %d", pipeline_id);
			return -1;
	}

	camera_write_mask(reg, val, mask);
	CAMLOGV("pipeline%d set to %s mode", pipeline_id, shadow ? "shadow" : "direct");

	return 0;
}

int  cam_set_pipe_hdl(struct pipeline_param* pipe_config,int enable)
{
	uint_32 reg = 0;

	if (PIPELINE_ID_PREV == pipe_config->pipeline_id) {
		reg = REG_SUBSAMPLE_SCALER_CTRL0;
	} else if (PIPELINE_ID_VIDEO == pipe_config->pipeline_id) {
		reg = REG_SUBSAMPLE_SCALER_CTRL1;
	} else {
		reg = REG_SUBSAMPLE_SCALER_CTRL2;
	}

	if (enable) {
		camera_set_bit(reg, 1 << 0);
	} else {
		camera_clr_bit(reg, 1 << 0);
	}
	return 0;
}


void set_pipeline_cur_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id,uint_16 value){
	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr->pipe0_cfg.cur_zoom = value;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr->pipe1_cfg.cur_zoom = value;
	else
		return;
}

uint_16 get_pipeline_cur_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id){
	if(pipe_id == PIPELINE_ID_PREV)
		return pipe_mgr->pipe0_cfg.cur_zoom;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		return pipe_mgr->pipe1_cfg.cur_zoom;
	else
		return 0x100;
}

void set_pipeline_target_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id,uint_16 value){
	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr->pipe0_cfg.target_zoom= value;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr->pipe1_cfg.target_zoom = value;
	else
		return;
}

uint_16 get_pipeline_target_zoomratio(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id){
	if(pipe_id == PIPELINE_ID_PREV)
		return pipe_mgr->pipe0_cfg.target_zoom;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		return pipe_mgr->pipe1_cfg.target_zoom;
	else
		return 0x100;
}
/*for stream on set zoom*/
void set_pipeline_zoom_cfg(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id,uint_16 zoom_ratio)
{
	struct pipeline_param *pipe_mgr_cfg = NULL;
	uint_16 isp_out_width;
	uint_16 isp_out_height;
	uint_16 crop_width = 0;
	uint_16 crop_height = 0;
	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr_cfg =  &pipe_mgr->pipe1_cfg;
	else
		return;
	isp_out_width = pipe_mgr->isp_cfg.isp_crop.end_x - pipe_mgr->isp_cfg.isp_crop.start_x;
	isp_out_height = pipe_mgr->isp_cfg.isp_crop.end_y - pipe_mgr->isp_cfg.isp_crop.start_y;
	crop_width = ALIGN_TO(isp_out_width * 0x100 /zoom_ratio,4);
	crop_height = ALIGN_TO(isp_out_height * 0x100 /zoom_ratio,2);

	pipe_mgr_cfg->pipeline_crop.start_x = ALIGN_TO( ((isp_out_width - crop_width) / 2), 4);
	pipe_mgr_cfg->pipeline_crop.end_x = pipe_mgr_cfg->pipeline_crop.start_x + crop_width;
	pipe_mgr_cfg->pipeline_crop.start_y = ALIGN_TO( ((isp_out_height - crop_height) /2), 2);
	pipe_mgr_cfg->pipeline_crop.end_y = pipe_mgr_cfg->pipeline_crop.start_y + crop_height;
	CAMLOGV("set_pipeline_zoom_cfg pipe_id=%d ratio=0x%x wxh[%d x %d],pipe crop (%d,%d)-(%d,%d)",
					pipe_id, zoom_ratio, isp_out_width, isp_out_height,
					pipe_mgr_cfg->pipeline_crop.start_x,pipe_mgr_cfg->pipeline_crop.start_y,
					pipe_mgr_cfg->pipeline_crop.end_x,pipe_mgr_cfg->pipeline_crop.end_y);
	cam_set_pipe_crop(pipe_mgr_cfg);
	cam_set_pipe_subsmp(pipe_mgr_cfg);
	cam_set_scaler(pipe_mgr_cfg);
}

/*for smooth zoom*/
void set_pipeline_smooth_zoom_cfg(struct cam_isp_pipeline * pipe_mgr ,uint_8 pipe_id)
{
	struct pipeline_param *pipe_mgr_cfg = NULL;
	uint_16 cur_zoom;
	uint_16 target_zoom;
	if(pipe_id == PIPELINE_ID_PREV)
		pipe_mgr_cfg = &pipe_mgr->pipe0_cfg;
	else if(pipe_id == PIPELINE_ID_VIDEO)
		pipe_mgr_cfg =  &pipe_mgr->pipe1_cfg;
	else
		return;
	if(pipe_mgr_cfg->cur_zoom == pipe_mgr_cfg->target_zoom)
		return;
	cur_zoom = pipe_mgr_cfg->cur_zoom;
	target_zoom = pipe_mgr_cfg->target_zoom;
	if ( cur_zoom> target_zoom ) {
		cur_zoom = cur_zoom*85/100;
		if ( cur_zoom < 0x100 ) {
			cur_zoom = 0x100;
		}
		if ( cur_zoom < target_zoom ) {
			cur_zoom = target_zoom;
		}
	}
	else if ( cur_zoom < target_zoom ) {
		cur_zoom = cur_zoom*115/100;
		if ( cur_zoom > 0x400 ) {
			cur_zoom = 0x400;
		}
		if ( cur_zoom > target_zoom ) {
			cur_zoom = target_zoom;
		}
	}
	set_pipeline_zoom_cfg(pipe_mgr,pipe_id,cur_zoom);
	set_pipeline_cur_zoomratio(pipe_mgr,pipe_id,cur_zoom);
	return;
}

void set_pipeline_reg_reset(void)
{
	camera_reg_write(REG_PIP0_CCIC_IMG_PITCH, 0x0);
	camera_reg_write(REG_CCIC_IRQ_MASK, 0x0);
	camera_reg_write(REG_IMG_SIZE_PIP0, 0x0);
	camera_reg_write(REG_IMG_OFFSET, 0x0);
	camera_reg_write(REG_CTRL_0, 0x0);
	camera_reg_write(REG_CTRL_1, 0x1400003c);
	camera_reg_write(REG_CTRL_2, 0x0);
	camera_reg_write(REG_CAMERA_IRQ_RAW_MASK, 0x0);
	camera_reg_write(REG_CLOCK_CTRL,0x0);
	camera_reg_write(REG_CSI2_CTRL_0, 0x0);
	camera_reg_write(REG_CSI2_VCCTRL, 0x0);
	camera_reg_write(REG_CSI2_DPHY1, 0x1);
	camera_reg_write(REG_CSI2_DPHY2, 0xa2808888);
	camera_reg_write(REG_CSI2_DPHY3, 0x0);
	camera_reg_write(REG_CSI2_DPHY4, 0x0);
	camera_reg_write(REG_CSI2_DPHY5, 0x0);
	camera_reg_write(REG_CSI2_DPHY6, 0x0);
	camera_reg_write(REG_CSI2_RESET, 0x7f);
	camera_reg_write(REG_PIP1_CCIC_IMG_PITCH, 0x0);
	camera_reg_write(REG_PIP2_CCIC_IMG_PITCH, 0x0);
	camera_reg_write(REG_PIP0_CROP_ROI_X, 0x0);
	camera_reg_write(REG_PIP0_CROP_ROI_Y, 0x0);
	camera_reg_write(REG_PIP1_CROP_ROI_X, 0x0);
	camera_reg_write(REG_PIP1_CROP_ROI_Y, 0x0);
	camera_reg_write(REG_PIP0_SCALER_IN_SIZE, 0x0);
	camera_reg_write(REG_PIP0_SCALER_OUT_SIZE, 0x0);
	camera_reg_write(REG_PIP0_SCALER_H_PHASE, 0x0);
	camera_reg_write(REG_PIP0_SCALER_V_PHASE, 0x0);
	camera_reg_write(REG_PIP1_SCALER_IN_SIZE, 0x0);
	camera_reg_write(REG_PIP1_SCALER_OUT_SIZE, 0x0);
	camera_reg_write(REG_PIP1_SCALER_H_PHASE, 0x0);
	camera_reg_write(REG_PIP1_SCALER_V_PHASE, 0x0);
	camera_reg_write(REG_DATA_FORMAT_CTRL, 0x0);
	camera_reg_write(REG_ISP_DVP_IF_CTRL, 0x80808);
	camera_reg_write(REG_IMG_SIZE_PIP1, 0x0);
	camera_reg_write(REG_IMG_SIZE_PIP2, 0x0);
	camera_reg_write(REG_ISP_IMG_SIZE, 0x0);
	camera_reg_write(REG_DATA_RANGE_CTRL0, 0x0);
	camera_reg_write(REG_DATA_RANGE_CTRL1, 0x0);
	camera_reg_write(REG_DATA_RANGE_CTRL2, 0x0);
	camera_reg_write(REG_DATA_RANGE_CTRL3, 0x0);
	camera_reg_write(REG_DATA_RANGE_CTRL4, 0x0);
	camera_reg_write(REG_DATA_RANGE_CTRL5, 0x0);
	camera_reg_write(REG_SUBSAMPLE_SCALER_CTRL0, 0x0);
	camera_reg_write(REG_SUBSAMPLE_SCALER_CTRL1, 0x0);
	camera_reg_write(REG_SUBSAMPLE_SCALER_CTRL2, 0x0);
	return;
}

