#include <ui_os_api.h>
#include "intc_api.h"

#include "../../../clock/media-clk/media_clk.h"
#include "../../camera_common_log.h"
#include "offline_hw_reg.h"
#include "scaler_rotation.h"

/* close CAMLOGD */
#undef CAMLOGD
#define CAMLOGD(fmt, args...) do {} while(0)

#define SCL_SUBSAMPLE_ENABLE

/* macro for regs */
#define REG_OFFLINE_BASE 0xD420F000
#define IRQ_MMP_OFFLINE 61

struct scl_coef_tab {
	uint32_t val;
	uint32_t min;
	uint32_t max;
};

static const struct scl_coef_tab ctab[] = {
	{SCL_25_COEFF,  0,   275},
	{SCL_30_COEFF,  275, 325},
	{SCL_35_COEFF,  325, 375},
	{SCL_40_COEFF,  375, 425},
	{SCL_45_COEFF,  425, 475},
	{SCL_50_COEFF,  475, 525},
	{SCL_55_COEFF,  525, 575},
	{SCL_60_COEFF,  575, 625},
	{SCL_65_COEFF,  625, 675},
	{SCL_70_COEFF,  675, 725},
	{SCL_75_COEFF,  725, 775},
	{SCL_80_COEFF,  775, 825},
	{SCL_85_COEFF,  825, 875},
	{SCL_90_COEFF,  875, 925},
	{SCL_95_COEFF,  925, 975},
	{SCL_100_COEFF, 975, 4000}, //max 4xzoom
};

static uint32_t scl_ire_reg_read(uint32_t reg)
{
	uint32_t val = 0xBABABABA;
	val = BUREG_READ(REG_OFFLINE_BASE + reg);
	return val;
}

static void scl_ire_reg_write(uint32_t reg, uint32_t val)
{
	BUREG_WRITE(REG_OFFLINE_BASE + reg, val);
}

static  void scl_ire_write_mask(uint32_t reg, uint32_t val, uint32_t mask)
{
	uint32_t v;
	v = scl_ire_reg_read(reg);
	v = (v & ~mask) | (val & mask);
	scl_ire_reg_write(reg, v);
}
static  void scl_ire_set_bit(uint32_t reg, uint32_t val)
{
	scl_ire_write_mask(reg, val, val);
}
static  void scl_ire_clr_bit(uint32_t reg, uint32_t val)
{
	scl_ire_write_mask(reg, 0, val);
}

int scl_ire_regs_dump(void)
{
	uint32_t i, val = 0;

	for(i = 0; i < 0x22; i++){
		val = scl_ire_reg_read(4 * i);
		CAMLOGI("[0x%x] = 0x%x!", 4 * i, val);
	}

	return 0;
}

void offline_s_reset(void)
{
	CAMLOGE("APMU: offline clk and axi bus reset");
	media_clk_clr_bit(0x50, BIT_8);
	media_clk_clr_bit(0x50, BIT_24);
	media_clk_set_bit(0x50, BIT_8);
	media_clk_set_bit(0x50, BIT_24);

	return;
}

static int scl_parse_input_fmt(SCALER_INPUT_FMT fmt, uint32_t *data_fmt, uint32_t *yuv422_order)
{
	uint32_t _fmt, _order;

	switch(fmt){
	case SCALER_INPUT_FMT_YUV422_YUYV:
		_fmt = SCL_FMT_YUV422;
		_order = YUV422_ORDER_YUYV;
		break;
	case SCALER_INPUT_FMT_YUV422_YVYU:
		_fmt = SCL_FMT_YUV422;
		_order = YUV422_ORDER_YVYU;
		break;
	case SCALER_INPUT_FMT_YUV422_UYVY:
		_fmt = SCL_FMT_YUV422;
		_order = YUV422_ORDER_UYVY;
		break;
	case SCALER_INPUT_FMT_YUV422_VYUY:
		_fmt = SCL_FMT_YUV422;
		_order = YUV422_ORDER_VYUY;
		break;
	case SCALER_INPUT_FMT_YUV420_NV12:
	case SCALER_INPUT_FMT_YUV420_NV21:
		_fmt = SCL_FMT_YUV420_2PLANAR;
		_order = 0; //invalid
		break;	
	case SCALER_INPUT_FMT_YUV420_I420: /*3 planar  Y, U, V */
	case SCALER_INPUT_FMT_YUV420_YV12: /*3 planar  Y, V, U */
		_fmt = SCL_FMT_YUV420_3PLANAR;
		_order = 0; //invalid
		break;

	default:
		CAMLOGE("input format not support: %d.", fmt);
		return -1;
	}

	CAMLOGD("data_fmt = %x, yuv422_order = %x", _fmt, _order);
	if (data_fmt)
		*data_fmt = _fmt;

	if (yuv422_order)
		*yuv422_order = _order;

	return 0;
}

static uint32_t scl_calc_subsam(uint32_t in, uint32_t out)
{
	uint32_t subsam_shift = 0;

	if (in <= out * 4)
		subsam_shift = 0;
	else if (in <= out * 8)
		subsam_shift = 1;
	else if (in <= out * 16)
		subsam_shift = 2;
	else if (in <= out * 32)
		subsam_shift = 3;
	else if (in <= out * 64)
		subsam_shift = 4;
	else if (in <= out * 128)
		subsam_shift = 5;

	CAMLOGD("subsample 1/%d", 1 << subsam_shift);

	return subsam_shift;
}

static int scl_calc_coef_h(uint32_t w_in, uint32_t w_ot, uint32_t *coef_sel)
{
	int rc = 0;
	int idx = 0;
	uint32_t prop = 0;
	if(w_ot < (w_in >> 4) || w_ot > (w_in << 2)) {
		CAMLOGE("scaler only support 1/16x~4x.");
		return -1;
	}
	prop = w_ot * 1000 / w_in;
	for (idx = 0; idx < 16; idx++) {
		if (prop > ctab[idx].min && prop <= ctab[idx].max) {
			*coef_sel = ctab[idx].val;
			break;
		}
	}
	return rc;
}

static int scl_calc_coef_v(uint32_t h_in, uint32_t h_ot, uint32_t *coef_sel)
{
	int rc = 0;
	int idx = 0;
	uint32_t prop = 0;

	if(h_ot < (h_in >> 4) || h_ot > (h_in << 2)) {
		CAMLOGE("scaler only support 1/16x~4x");
		return -1;
	}
	prop = h_ot * 1000 / h_in;
	for (idx = 0; idx < 16; idx++) {
		if (prop > ctab[idx].min && prop <= ctab[idx].max) {
			*coef_sel = ctab[idx].val;
			break;
		}
	}
	return rc;
}

static void scl_enable(void)
{
	CAMLOGV("E");
	scl_ire_set_bit(REG_SCLAER_TOP_CTRL, SCL_CTRL_EN);
}

//static void scl_disable(void)
//{
//	CAMLOGV("E");
//	scl_ire_clr_bit(REG_SCLAER_TOP_CTRL, SCL_CTRL_EN);
//}

void scl_trigger(void)
{
	CAMLOGD(" E");
	scl_ire_set_bit(REG_SCLAER_TOP_CTRL, SCL_CTRL_TRIGGER_FRAME);
}

static void scl_set_phase(uint32_t w_in, uint32_t w_ot, uint32_t h_in, uint32_t h_ot)
{
	int h_init_ph = 0;
	int h_delta_ph = 0;
	int v_init_ph = 0;
	int v_delta_ph = 0;
	uint32_t val;

	h_init_ph = (w_in * 16 / w_ot + 1) / 2;
	h_delta_ph = (w_in << 16) / w_ot;  // 4.12

	v_init_ph = (h_in * 16 / h_ot + 1) / 2;
	v_delta_ph = (h_in << 16) / h_ot;  // 4.12

	CAMLOGV("h_init_ph=%x, h_delta_ph=%x, v_init_ph=%x, v_delta_ph=%x",
		h_init_ph, h_delta_ph, v_init_ph, v_delta_ph);
	val = (h_delta_ph & 0x1FFFFF) | ((h_init_ph & 0x7F) << 24);
	scl_ire_reg_write(REG_SCALER_H_PHASE, val);
	val = (v_delta_ph & 0x1FFFFF) | ((v_init_ph & 0x7F) << 24);
	scl_ire_reg_write(REG_SCALER_V_PHASE, val);
	return;
}

static void scl_set_size(uint32_t w_in, uint32_t h_in, uint32_t w_ot, uint32_t h_ot)
{
	uint32_t in_size, ot_size;

	CAMLOGD("w_in=0x%08x, h_in=0x%08x, w_ot=0x%08x, h_ot=0x%08x",
	       w_in, h_in, w_ot, h_ot);
	in_size = (w_in & 0x1FFF) | ((h_in & 0x1FFF) << 16);
	ot_size = (w_ot & 0x1FFF) | ((h_ot & 0x1FFF) << 16);

	scl_ire_reg_write(REG_SCALER_IN_SIZE, in_size);
	scl_ire_reg_write(REG_SCALER_OUT_SIZE, ot_size);
	return;
}

static int scl_set_slice(uint32_t slice_in, uint32_t slice_ot)
{
	uint32_t slice = 0;

	/* increase to 640 since crane z2 */
	if (slice_ot > 640) {
		CAMLOGE("%s: slice out must less than 640 err\n", __func__);
		return -1;
	}

	CAMLOGV("slice_in=0x%x, slice_ot=0x%x",
	       slice_in, slice_ot);

	slice = (slice_in & 0x1FFF) | ((slice_ot & 0x1FFF) << 16);
	scl_ire_reg_write(REG_SCALER_SLICE_WIDTH, slice);

	return 0;
}

static void scl_set_pitch(uint32_t pitch_in, uint32_t pitch_ot)
{
	CAMLOGD(" pitch_in=0x%x, pitch_ot=0x%x",
	       pitch_in, pitch_ot);
	scl_ire_reg_write(REG_SCALER_READ_PITCH, (pitch_in & 0x3FFFF));
	scl_ire_reg_write(REG_SCALER_WRITE_PITCH, (pitch_ot & 0x3FFFF));
	return;
}

static int scl_set_top(SCALER_INPUT_FMT fmt, uint32_t w_in, uint32_t h_in,
		uint32_t w_ot, uint32_t h_ot)
{
	int rc = 0;
	uint32_t data_fmt, yuv422_order, coef_sel_h, coef_sel_v;
	uint32_t val = 0, mask = 0;

	CAMLOGV("E");

	/* data_fmt, yuv422 order */
	rc = scl_parse_input_fmt(fmt, &data_fmt, &yuv422_order);
	if (rc < 0) {
		CAMLOGE("cam_scl_set_top: parse fmt err!");
		return rc;
	}

	rc = scl_calc_coef_h(w_in, w_ot, &coef_sel_h);
	if (rc < 0) {
		CAMLOGE("cam_scl_set_top: calc coef tab h err!");
		return rc;
	}
	rc = scl_calc_coef_v(h_in, h_ot, &coef_sel_v);
	if (rc < 0) {
		CAMLOGE("cam_scl_set_top: calc coef tab v err!");
		return rc;
	}

	val = data_fmt | yuv422_order | (coef_sel_v << 8) | coef_sel_h;
	//no subsample
	mask = SCL_CTRL_DATA_FMT_MASK | SCL_CTRL_YUV422_ORDER_MASK |
		SCL_CTRL_COEF_TAB_SEL_V_MASK | SCL_CTRL_COEF_TAB_SEL_H_MASK;
	scl_ire_write_mask(REG_SCLAER_TOP_CTRL, val, mask);

	return rc;
}

void scl_set_addr(uint32_t addr_rd_y, uint32_t addr_rd_u, uint32_t addr_rd_v,
		  uint32_t addr_wr_y, uint32_t addr_wr_u, uint32_t addr_wr_v)
{
	CAMLOGD("rd_y=0x%08x, rd_u=0x%08x, rd_v=0x%08x, wr_y=0x%08x, wr_u=0x%08x, wr_v=0x%08x",
		addr_rd_y, addr_rd_u, addr_rd_v, addr_wr_y, addr_wr_u, addr_wr_v);
	scl_ire_reg_write(REG_SCALER_RD_Y_BASE_ADDR, addr_rd_y);
	scl_ire_reg_write(REG_SCALER_RD_U_BASE_ADDR, addr_rd_u);
	scl_ire_reg_write(REG_SCALER_RD_V_BASE_ADDR, addr_rd_v);

	scl_ire_reg_write(REG_SCALER_WR_Y_BASE_ADDR, addr_wr_y);
	scl_ire_reg_write(REG_SCALER_WR_U_BASE_ADDR, addr_wr_u);
	scl_ire_reg_write(REG_SCALER_WR_V_BASE_ADDR, addr_wr_v);
}

static int scl_set_subsample(uint32_t subsam)
{
	uint32_t reg_val;

	switch (subsam) {
	case 0:
		reg_val = SCL_1OVER1_SUBSAMPLE;
		break;
	case 1:
		reg_val = SCL_1OVER2_SUBSAMPLE;
		break;
	case 2:
		reg_val = SCL_1OVER4_SUBSAMPLE;
		break;
	case 3:
		reg_val = SCL_1OVER8_SUBSAMPLE;
		break;
	case 4:
		reg_val = SCL_1OVER16_SUBSAMPLE;
		break;
	case 5:
		reg_val = SCL_1OVER32_SUBSAMPLE;
		break;
	default:
		CAMLOGE("invalid subsample level %d", 1 << subsam);
		return -1;
	}

	scl_ire_write_mask(REG_SCLAER_TOP_CTRL, reg_val, SCL_CTRL_SUBSAMLE_MASK);

	return 0;
}

/*
 * w_in---->w_subsam---->w_ot
 * h_in---->h_subsam---->h_ot
 */
int scl_ctrl_cfg(uint32_t w_in, uint32_t h_in, uint32_t w_ot, uint32_t h_ot,
		 uint32_t pitch_in, uint32_t pitch_ot, SCALER_INPUT_FMT fmt)
{
	int rc = 0;
	uint32_t w_subsam, h_subsam;
	uint32_t subsam_shift = 0;

	CAMLOGV("E");

	scl_enable();

#ifdef SCL_SUBSAMPLE_ENABLE
	subsam_shift = scl_calc_subsam(w_in, w_ot);
#endif
	rc = scl_set_subsample(subsam_shift);
	if (rc < 0) {
		CAMLOGE("scaler subsample err!");
		return rc;
	}

	w_subsam = w_in >> subsam_shift;
	h_subsam = h_in >> subsam_shift;

	scl_set_size(w_subsam, h_subsam, w_ot, h_ot);
	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_1) 
		rc = scl_set_slice(512 * w_subsam / w_ot, 512);
	else
		/* linebuffer increased to 640 since 20180625 */
		rc = scl_set_slice(640 * w_subsam / w_ot, 640); 

	if (rc < 0) {
		CAMLOGE("slice err!");
		return rc;
	}
	scl_set_phase(w_subsam, w_ot, h_subsam, h_ot);
	rc = scl_set_top(fmt, w_subsam, h_subsam, w_ot, h_ot);
	if (rc < 0) {
		CAMLOGE("top err!");
		return rc;
	}
	scl_set_pitch(pitch_in, pitch_ot);

	return rc;
}

static int ire_parse_input_fmt(SCALER_INPUT_FMT fmt, uint32_t *img_fmt, uint32_t *yuv422_edian)
{
	uint32_t _img_fmt, _yuv422_edian;

	switch (fmt) {
	case SCALER_INPUT_FMT_YUV422_YUYV:
		_img_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_YUYV_EDIAN;
		break;
	case SCALER_INPUT_FMT_YUV422_YVYU:
		_img_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_YVYU_EDIAN;
		break;
	case SCALER_INPUT_FMT_YUV422_UYVY:
		_img_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_UYVY_EDIAN;
		break;
	case SCALER_INPUT_FMT_YUV422_VYUY:
		_img_fmt = IRE_FMT_YUV422;
		_yuv422_edian = IRE_CTRL_VYUY_EDIAN;
		break;
	case SCALER_INPUT_FMT_YUV420_NV12:
	case SCALER_INPUT_FMT_YUV420_NV21:
		_img_fmt = IRE_FMT_YUV420_2PLANAR;
		_yuv422_edian = 0;  //invalid
		break;
	case SCALER_INPUT_FMT_YUV420_I420: /*3 planar  Y, U, V */
	case SCALER_INPUT_FMT_YUV420_YV12: /*3 planar  Y, V, U */
		_img_fmt = IRE_FMT_YUV420_3PLANAR;
		_yuv422_edian = 0;  //invalid
		break;
	case SCALER_INPUT_FMT_ARGB32:
		_img_fmt = IRE_FMT_ARGB;
		_yuv422_edian = 0; //invalid
		break;	
	default:
		CAMLOGE("input format not support: %d!", fmt);
		return -1;
	}

	CAMLOGD("img_fmt = 0x%08x, yuv422_edian = 0x%08x",
	       _img_fmt, _yuv422_edian);
	if (img_fmt)
	*img_fmt = _img_fmt;

	if (yuv422_edian)
	*yuv422_edian = _yuv422_edian;

	return 0;
}

static void ire_enable(void)
{
	CAMLOGV("E");
	scl_ire_set_bit(REG_IRE_TOP_CTRL, IRE_CTRL_EN);
}

//static void ire_disable(void)
//{
//	CAMLOGV("E");
//	scl_ire_clr_bit(REG_IRE_TOP_CTRL, IRE_CTRL_EN);
//}

void ire_trigger(void)
{
	CAMLOGD(" E");
	scl_ire_set_bit(REG_IRE_TOP_CTRL, IRE_CTRL_PREFETCH);
}

static void ire_set_size(uint32_t w_in, uint32_t h_in)
{
	uint32_t in_size;

	in_size = (w_in & 0x1FFF) | ((h_in & 0xFFF) << 16);
	CAMLOGD("w_in=0x%x, h_in=0x%x", w_in, h_in);
	scl_ire_reg_write(REG_IRE_IN_SIZE, in_size);
}

static void ire_set_pitch(uint32_t pitch_in, uint32_t pitch_ot)
{
	CAMLOGD("pitch_in=0x%x, pitch_ot=0x%x",
	       pitch_in, pitch_ot);

	scl_ire_reg_write(REG_IRE_PRE_PITCH, (pitch_in & 0x3FFFF));
	scl_ire_reg_write(REG_IRE_PST_PITCH, (pitch_ot & 0x3FFFF));
}

static int ire_set_dma_mode(int dma_mode)
{
	uint32_t val = 0, mask = 0;

	switch (dma_mode) {
	case 0:
		val = IRE_CTRL_DMA_MODE0;
		break;
	case 1:
		val = IRE_CTRL_DMA_MODE1;
		break;
	case 2:
		val = IRE_CTRL_DMA_MODE2;
		break;
	case 3:
		val = IRE_CTRL_DMA_MODE3;
		break;
	default:
		CAMLOGE("invalid dma mode %d", dma_mode);
		return -1;
	}
	mask = IRE_CTRL_DMA_MODE_MASK;

	CAMLOGD("dma_mode=0x%x", dma_mode);
	scl_ire_write_mask(REG_IRE_TOP_CTRL, val, mask);
	return 1;
}

static int ire_set_top(SCALER_INPUT_FMT fmt, enum offline_rot rot)
{
	int rc = 0;
	uint32_t angle, img_fmt, yuv422_edian, yuv422_uv_swap;
	uint32_t val = 0, mask = 0;

	switch (rot) {
	case ROT_90_DEG:
		angle = IRE_DEGREE_90;
		yuv422_uv_swap = IRE_UV_NO_SWAP;
		break;
	case ROT_180_DEG:
		angle = IRE_DEGREE_180;
		yuv422_uv_swap = IRE_UV_DO_SWAP;
		break;
	case ROT_270_DEG:
		angle = IRE_DEGREE_270;
		yuv422_uv_swap = IRE_UV_NO_SWAP;
		break;
	case ROT_MIR:
		angle = IRE_MIRROR;
		yuv422_uv_swap = IRE_UV_DO_SWAP;
		break;
	case ROT_FLIP:
		angle = IRE_FLIP;
		yuv422_uv_swap = IRE_UV_NO_SWAP;
		break;
	default:
		CAMLOGE("rot %d err!", rot);
		return -1;
	}
	rc = ire_parse_input_fmt(fmt, &img_fmt, &yuv422_edian);
	if (rc < 0) {
		CAMLOGE("parse input fmt err!");
		return -1;
	}

	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_1) {
		val = angle | img_fmt | yuv422_edian;
		mask = IRE_CTRL_ANGLE_MASK | IRE_CTRL_IMG_FMT_MASK | IRE_CTRL_422_EDIAN_MASK;
	}else{
		val = angle | img_fmt | yuv422_edian | yuv422_uv_swap;
		mask = IRE_CTRL_ANGLE_MASK | IRE_CTRL_IMG_FMT_MASK | IRE_CTRL_422_EDIAN_MASK | IRE_CTRL_UV_SWAP_MASK;
	}
	scl_ire_write_mask(REG_IRE_TOP_CTRL, val, mask);

	return rc;
}

void ire_set_addr(uint32_t addr_rd_y, uint32_t addr_rd_u, uint32_t addr_rd_v,
		  uint32_t addr_wr_y, uint32_t addr_wr_u, uint32_t addr_wr_v)
{
	if(addr_rd_y)
		scl_ire_reg_write(REG_IRE_RD_Y_BASE_ADDR, addr_rd_y);
	if(addr_rd_u)
		scl_ire_reg_write(REG_IRE_RD_U_BASE_ADDR, addr_rd_u);
	if(addr_rd_v)
		scl_ire_reg_write(REG_IRE_RD_V_BASE_ADDR, addr_rd_v);
	if(addr_wr_y)
		scl_ire_reg_write(REG_IRE_WR_Y_BASE_ADDR, addr_wr_y);
	if(addr_wr_u)
		scl_ire_reg_write(REG_IRE_WR_U_BASE_ADDR, addr_wr_u);
	if(addr_wr_v)
		scl_ire_reg_write(REG_IRE_WR_V_BASE_ADDR, addr_wr_v);
	return;
}

// cfg as ASIC recomend
const uint32_t dmac_postwr_en = 0;
const uint32_t dmac_max_req_num = 4;
const uint32_t dmac_scan_mode = 0;
const uint32_t dmac_scan_enable = 0;
const uint32_t dmac_rst_req = 0;
const uint32_t dmac_rst_n_pwr = 1;
const uint32_t dmac_user_id = 0;
const uint32_t dmac_axi_sec = 0;
const uint32_t dmac_arqos = 4;
const uint32_t dmac_awqos = 4;
static void ire_dmac_cfg(void)
{
	uint32_t val = 0;
	val = (dmac_postwr_en << 24) | (dmac_max_req_num << 19) | (dmac_scan_mode << 18) \
	| (dmac_scan_enable << 17) | (dmac_rst_req << 14) | (dmac_rst_n_pwr << 13) | (dmac_user_id << 9) \
	| (dmac_axi_sec << 8) | (dmac_arqos << 4) | dmac_awqos;
	scl_ire_reg_write(REG_IRE_DMAC_CTRL, val);
}

int ire_ctrl_cfg(uint32_t w_in, uint32_t h_in, uint32_t pitch_in, uint32_t pitch_ot,
		 SCALER_INPUT_FMT fmt, enum offline_rot rot)
{
	int rc = 0;

	CAMLOGV("E");

	ire_enable();
	ire_dmac_cfg();
	rc = ire_set_top(fmt, rot);
	if (rc < 0) {
		CAMLOGE("ire_ctrl_cfg: top err!");
		return rc;
	}
	ire_set_size(w_in, h_in);
	ire_set_pitch(pitch_in, pitch_ot);
	return rc;
}

/* ire new function for dma efficiency since 2018-12-01 */
int ire_ctrl_cfg_v2(uint32_t w_in, uint32_t h_in, uint32_t pitch_in, uint32_t pitch_ot,
		 SCALER_INPUT_FMT fmt, enum offline_rot rot, int dma_mode)
{
	int rc = 0;

	CAMLOGV("E");

	ire_enable();
	ire_dmac_cfg();
	ire_set_dma_mode(dma_mode);
	rc = ire_set_top(fmt, rot);
	if (rc < 0) {
		CAMLOGE("top err");
		return rc;
	}
	ire_set_size(w_in, h_in);
	ire_set_pitch(pitch_in, pitch_ot);

	return rc;
}

//static void scl_ire_debug_info(void)
//{
//	uint32_t val = 0;
//	val = scl_ire_reg_read(REG_IRE_DEBUG_INFO_LSB);
//	CAMLOGI("REG_IRE_DEBUG_INFO_LSB: val = 0x%x", val);
//	val = scl_ire_reg_read(REG_IRE_DEBUG_INFO_MSB);
//	CAMLOGI("REG_IRE_DEBUG_INFO_MSB: val = 0x%x", val);
//	val = scl_ire_reg_read(REG_IRE_DMAC_DEBUG_INFO);
//	CAMLOGI("REG_IRE_DMAC_DEBUG_INFO: val = 0x%x", val);
//	val = scl_ire_reg_read(REG_SCL_DEBUG_INFO_0);
//	CAMLOGI("REG_SCL_DEBUG_INFO_0: val = 0x%x", val);
//	val = scl_ire_reg_read(REG_SCL_DEBUG_INFO_1);
//	CAMLOGI("REG_SCL_DEBUG_INFO_1: val = 0x%x", val);
//	val = scl_ire_reg_read(REG_SCL_DEBUG_INFO_2);
//	CAMLOGI("REG_SCL_DEBUG_INFO_2: val = 0x%x", val);
//	return;
//}

static void scl_ire_isr_handler(INTC_InterruptInfo interruptInfo)
{
	u8 rc = 0;
	uint32_t irqs = 0;

	irqs = scl_ire_reg_read(REG_OFFLINE_IRQ_STATUS);
	CAMLOGV("REG_OFFLINE_IRQ_STATUS = 0x%08x", irqs);
	scl_ire_reg_write(REG_OFFLINE_IRQ_STATUS, irqs); //clear irq

	if (irqs & IRQ_IRE_EOF)
		CAMLOGV("offline cam: IRE eof!");

	if (irqs & IRQ_SCL_EOF)
		CAMLOGV("offline cam: SCL eof!");

	if (irqs & IRQ_DMAC_RD_CH0_ERR)
		CAMLOGE("offline cam: dmac_rd_chnl0_err!");

	if (irqs & IRQ_DMAC_RD_CH1_ERR)
		CAMLOGE("offline isr: dmac_rd_chnl1_err!");

	if (irqs & IRQ_DMAC_WR_CH0_ERR)
		CAMLOGE("offline isr: dmac_wr_chnl0_err!");

	if (irqs & IRQ_DMAC_WR_CH1_ERR)
		CAMLOGE("offline isr: dmac_wr_chnl1_err!");

	if (irqs & IRQ_SCL_WF_OVERRUN)
		CAMLOGE("offline isr: scl_wfifo_overrun!");

	rc = UOS_SetFlag(g_offline_interrupt_flag, irqs, OSA_FLAG_OR);
	if (rc != 0)
		CAMLOGE("flag set error");

	return;
}

void offline_irq_init(void)
{
	INTC_ReturnCode INTCStatus;
	uint32_t INT_NUM;
	uint32_t irq_enable_bit = 0;

	CAMLOGI("offline_irq_init");
	irq_enable_bit = IRQ_DMAC_RD_CH0_ERR | IRQ_DMAC_RD_CH1_ERR | IRQ_DMAC_WR_CH0_ERR
		| IRQ_DMAC_WR_CH1_ERR | IRQ_SCL_WF_OVERRUN | IRQ_SCL_EOF | IRQ_IRE_EOF;
	scl_ire_set_bit(REG_OFFLINE_IRQ_MASK, irq_enable_bit);
	
	INT_NUM = INTCGetIntVirtualNum(IRQ_MMP_OFFLINE);
	INTCStatus = INTCBind (INT_NUM , scl_ire_isr_handler);
	INTCStatus = INTCConfigure(INT_NUM, INTC_IRQ, INTC_HIGH_LEVEL);
	INTCStatus = INTCEnable(INT_NUM);
	if(INTC_RC_OK != INTCStatus) {
		CAMLOGE("irq init fail !!!");
	}
}

void offline_irq_deinit(void)
{
	INTC_ReturnCode INTCStatus;
	uint32_t INT_NUM;
	uint32_t irq_enable_bit = 0;

	CAMLOGI("offline_irq_deinit");
	irq_enable_bit = IRQ_DMAC_RD_CH0_ERR | IRQ_DMAC_RD_CH1_ERR | IRQ_DMAC_WR_CH0_ERR
		| IRQ_DMAC_WR_CH1_ERR | IRQ_SCL_WF_OVERRUN | IRQ_SCL_EOF | IRQ_IRE_EOF;
	scl_ire_clr_bit(REG_OFFLINE_IRQ_MASK, irq_enable_bit);

	INT_NUM = INTCGetIntVirtualNum(IRQ_MMP_OFFLINE);
	INTCStatus = INTCUnbind(INT_NUM);
	INTCStatus = INTCDisable(INT_NUM);
	if(INTC_RC_OK != INTCStatus){
		CAMLOGE("deinit irq fail!!!");
	}
}

int offline_power_on(int module_id)
{
	uint32_t irq_enable_bit = 0;

	if(module_id >= MODULE_MAX){
		CAMLOGE("cam_offline_power_on: error module_id [%d]!", module_id);
		return 0;
	}
	CAMLOGV(" E!");

	media_power_on(module_id);
	//media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
	//MEDIA_CLK_LCD_SCLK(bit1) is a must for camera offline module
	media_clk_enable(MEDIA_CLK_LCD_AHB|MEDIA_CLK_LCD_CI|MEDIA_CLK_OFFLINE|MEDIA_CLK_OFFLINE_AXI, module_id);
	irq_enable_bit = IRQ_DMAC_RD_CH0_ERR | IRQ_DMAC_RD_CH1_ERR | IRQ_DMAC_WR_CH0_ERR
		| IRQ_DMAC_WR_CH1_ERR | IRQ_SCL_WF_OVERRUN | IRQ_SCL_EOF | IRQ_IRE_EOF;
	scl_ire_set_bit(REG_OFFLINE_IRQ_MASK, irq_enable_bit);

	//media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);

	return 0;
}

int offline_power_off(int module_id)
 {
 	 if(module_id >= MODULE_MAX){
		CAMLOGE("cam_offline_power_off: error module_id [%d]!", module_id);
		return 0;
 	 }
	 CAMLOGV(" E!");

//	 media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
 	 media_clk_disable(MEDIA_CLK_OFFLINE_AXI|MEDIA_CLK_OFFLINE|MEDIA_CLK_LCD_CI|MEDIA_CLK_LCD_AHB, module_id);
	 media_power_off(module_id);
//	 media_clk_dump(LEVEL_DUMP_CLOCK_ONLY);
 
	 return 0;
 }
