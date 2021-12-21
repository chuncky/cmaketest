#include <ui_os_api.h>

#include "../../../clock/media-clk/media_clk.h"
#include "../../camera_common_log.h"
#include "scaler_rotation.h"
#include "cam_offline.h"
#include "offline_hw_reg.h"
#include "cp_include.h"

/* close CAMLOGD */
#undef CAMLOGD
#define CAMLOGD(fmt, args...) do {} while(0)

#define ALIGN_TO(x, iAlign) ( (((unsigned int)(x)) + (iAlign) - 1) & (~((iAlign) - 1)) )

#define OFFLINE_SCL_TIMEOUT 50   // unit(tick)   1 tick = 5ms
#define OFFLINE_IRE_TIMEOUT 50

u8 g_offline_interrupt_flag = INVALID_FLAG_ID;
struct cam_offline_ctx {
	//OSMutexRef scl_work_mutex;
	//char scl_mutex_init;
	u8 ire_work_mutex;
	char ire_mutex_init;
};
static struct cam_offline_ctx g_cam_offline_ctx;

/*
 * init member variables
 * include: num_planes/bpp/plane[3]/addr
 */
static int offline_buf_init(struct cam_offline_buf *buf)
{
	int rc = 0;
	uint32_t pitch;

	if (buf->width == 0 || buf->height == 0 || (buf->stride < buf->width)) {
		CAMLOGE("bad argument");
		return -1;
	}

	switch (buf->pixelformat) {
	case SCALER_INPUT_FMT_YUV422_YUYV: /*1 planar*/
	case SCALER_INPUT_FMT_YUV422_YVYU:
	case SCALER_INPUT_FMT_YUV422_UYVY:
	case SCALER_INPUT_FMT_YUV422_VYUY:
		buf->num_planes = 1;
		buf->bpp = 16;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->plane[0].pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		ASSERT(buf->plane_addr[0]); /* check addr */
		buf->plane[0].addr = buf->plane_addr[0]; /* init addr */
		/* TODO
		 * if ((buf->width) & 0x07) {
		 *         pr_err("err: width need 8 align\n");
		 *         rc = -1;
		 * }
		 */
		buf->plane[1].pitch = 0;
		buf->plane[1].bytesperline = 0;
		buf->plane[1].usedperline = 0;
		buf->plane[1].sizeimage = 0;
		buf->plane[1].addr = 0; /* init addr */

		buf->plane[2].pitch = 0;
		buf->plane[2].bytesperline = 0; 
		buf->plane[2].usedperline = 0;
		buf->plane[2].sizeimage = 0;
		buf->plane[2].addr = 0; /* init addr */
		break;
	case SCALER_INPUT_FMT_YUV420_NV12: /*2 planar*/
	case SCALER_INPUT_FMT_YUV420_NV21: 
		buf->num_planes = 2;
		buf->bpp = 8;
		pitch = buf->stride;
		buf->plane[0].pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->width;
		buf->plane[0].sizeimage = pitch * buf->height;
		ASSERT(buf->plane_addr[0]); /* check addr */
		buf->plane[0].addr = buf->plane_addr[0]; /* init addr */

		pitch = buf->stride;
		buf->plane[1].pitch = pitch;
		buf->plane[1].bytesperline = pitch;
		buf->plane[1].usedperline = buf->width;
		buf->plane[1].sizeimage = pitch * buf->height / 2;
		ASSERT(buf->plane_addr[1]); /* check addr */
		buf->plane[1].addr = buf->plane_addr[1]; /* init addr */

		buf->plane[2].pitch = 0;
		buf->plane[2].bytesperline = 0; 
		buf->plane[2].usedperline = 0;
		buf->plane[2].sizeimage = 0;
		buf->plane[2].addr = 0; /* init addr */
		break;
	case SCALER_INPUT_FMT_YUV420_I420: /*3 planar  Y, U, V */
	case SCALER_INPUT_FMT_YUV420_YV12: /*3 planar  Y, V, U */
		buf->num_planes = 3;
		buf->bpp = 8;
		pitch = buf->stride;
		buf->plane[0].pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->width;
		buf->plane[0].sizeimage = pitch * buf->height;
		ASSERT(buf->plane_addr[0]); /* check addr */
		buf->plane[0].addr = buf->plane_addr[0]; /* init addr */

		pitch = buf->stride >> 1;
		buf->plane[1].pitch = pitch;
		buf->plane[1].bytesperline = pitch;
		buf->plane[1].usedperline = buf->width >> 1;
		buf->plane[1].sizeimage = pitch * buf->height / 2;
		ASSERT(buf->plane_addr[1]); /* check addr */
		buf->plane[1].addr = buf->plane_addr[1]; /* init addr */

		buf->plane[2].pitch = pitch;
		buf->plane[2].bytesperline = pitch; 
		buf->plane[2].usedperline = buf->width >> 1;
		buf->plane[2].sizeimage = pitch * buf->height / 2;
		ASSERT(buf->plane_addr[2]); /* check addr */
		buf->plane[2].addr = buf->plane_addr[2]; /* init addr */
		break;
	case SCALER_INPUT_FMT_ARGB32:
		buf->num_planes = 1;
		buf->bpp = 32;
		pitch = buf->bpp * (buf->stride) >> 3;
		buf->plane[0].pitch = pitch;
		buf->plane[0].bytesperline = pitch;
		buf->plane[0].usedperline = buf->bpp * (buf->width) >> 3;
		buf->plane[0].sizeimage = pitch * buf->height;
		ASSERT(buf->plane_addr[0]); /* check addr */
		buf->plane[0].addr = buf->plane_addr[0]; /* init addr */
		break;
	default:
		CAMLOGE("offline buffer format not support: %d", buf->pixelformat);
		rc = -1;
	}

#if 0
	pr_info("###buffer %d, wxh = %dx%d, stide=%d",
			buf->index, buf->width, buf->height, buf->stride);
	pr_info("%s: p1 bytesperline=0x%08x, usedperline=0x%08x",
			__func__, buf->plane[0].bytesperline, buf->plane[0].usedperline);
	pr_info("%s: p2 bytesperline=0x%08x, usedperline=0x%08x",
			__func__, buf->plane[1].bytesperline, buf->plane[1].usedperline);
	pr_info("%s: p3 bytesperline=0x%08x, usedperline=0x%08x",
			__func__, buf->plane[2].bytesperline, buf->plane[2].usedperline);
#endif

	return rc;
}

static int calc_zoom_crop_size(uint32_t w_in, uint32_t h_in, uint32_t *w_crop, uint32_t *h_crop,
			       uint32_t *x_start, uint32_t *y_start, uint32_t zoom)
{
	uint32_t crop_start_x, crop_start_y, crop_end_x, crop_end_y;
	uint32_t crop_width, crop_height;

	crop_width = ALIGN_TO((w_in * 0x100 / zoom), 4);
	crop_height = ALIGN_TO((h_in * 0x100 / zoom), 2);

	CAMLOGD("%dx%d ==0x%x==> %dx%d",
		w_in, h_in, zoom, crop_width, crop_height);

	crop_start_x = ALIGN_TO((w_in - crop_width) >> 1, 4);
	crop_end_x = crop_start_x + crop_width;
	crop_start_y = ALIGN_TO((h_in - crop_height) >> 1, 2);
	crop_end_y = crop_start_y + crop_height;

	CAMLOGD("crop window (%d, %d) (%d, %d)",
		crop_start_x, crop_start_y, crop_end_x, crop_end_y);

	if (w_crop != NULL)
		*w_crop = crop_width;
	if (h_crop != NULL)
		*h_crop = crop_height;
	if (x_start != NULL)
		*x_start = crop_start_x;
	if (y_start != NULL)
		*y_start = crop_start_y;

	return 0;
}

static int zoom_base_addr_transform(struct cam_offline_buf *buf_in, struct cam_offline_buf *buf_ot,
				    uint32_t zoom)
{
	int rc;
	uint32_t crop_start_x, crop_start_y;
	uint32_t addr_rd_y = 0;
	uint32_t addr_rd_u = 0;
	uint32_t addr_rd_v = 0;
	uint32_t addr_wr_y = 0;
	uint32_t addr_wr_u = 0;
	uint32_t addr_wr_v = 0;

	if (zoom < 0x100 || zoom > 0x400) {
		CAMLOGE("invalid zoom ratio 0x%x!!!", zoom);
		return -1;
	}

	calc_zoom_crop_size(buf_in->width, buf_in->height, NULL, NULL, &crop_start_x, &crop_start_y, zoom);

	switch (buf_in->pixelformat) {
	case SCALER_INPUT_FMT_YUV422_YUYV: /*1 planar*/
	case SCALER_INPUT_FMT_YUV422_YVYU:
	case SCALER_INPUT_FMT_YUV422_UYVY:
	case SCALER_INPUT_FMT_YUV422_VYUY:
		addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * crop_start_y + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_u = 0;
		addr_rd_v = 0;
		break;
	case SCALER_INPUT_FMT_YUV420_NV12: /*2 planar*/
	case SCALER_INPUT_FMT_YUV420_NV21:
		addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * crop_start_y + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * crop_start_y / 2 + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_v = 0;
		break;
	case SCALER_INPUT_FMT_YUV420_I420: /*3 planar  Y, U, V */
	case SCALER_INPUT_FMT_YUV420_YV12: /*3 planar  Y, V, U */
		addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * crop_start_y + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * crop_start_y / 2 + (buf_in->bpp >> 3) * crop_start_x / 2;
		addr_rd_v = buf_in->plane[2].addr + buf_in->plane[2].bytesperline * crop_start_y / 2 + (buf_in->bpp >> 3) * crop_start_x / 2;
		break;
	default:
		CAMLOGE("invalid fmt %d", buf_in->pixelformat);
		return -1;
	}
	CAMLOGV("buf in  p1[0x%08x], p2[0x%08x], p3[0x%08x]",
		addr_rd_y, addr_rd_u, addr_rd_v);

	addr_wr_y = buf_ot->plane[0].addr;
	addr_wr_u = buf_ot->plane[1].addr;
	addr_wr_v = buf_ot->plane[2].addr;
	CAMLOGV("buf out p1[0x%08x], p2[0x%08x], p3[0x%08x]",
		addr_wr_y, addr_wr_u, addr_wr_v);

	scl_set_addr(addr_rd_y, addr_rd_u, addr_rd_v,
		     addr_wr_y, addr_wr_u, addr_wr_v);

	return rc;
}

static int ire_base_addr_transform(struct cam_offline_buf *buf_in, struct cam_offline_buf *buf_ot,
				       enum offline_rot rot)
{
	int rc = 0;
	uint32_t addr_rd_y = 0;
	uint32_t addr_rd_u = 0;
	uint32_t addr_rd_v = 0;
	uint32_t addr_wr_y = 0;
	uint32_t addr_wr_u = 0;
	uint32_t addr_wr_v = 0;

	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_3) {
		/* no need transform since 2018-12-05 15:18 */
		addr_rd_y = buf_in->plane[0].addr;
		addr_rd_u = buf_in->plane[1].addr;
		addr_rd_v = buf_in->plane[2].addr;
	}else{
		switch (rot) {
		case ROT_90_DEG:
		case ROT_FLIP:
			/* left lower pixel */
			if (buf_in->num_planes > 0) {
				addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * (buf_in->height - 1);
			}
			if (buf_in->num_planes > 1) {
				addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * (buf_in->height / 2 - 1);
			}
			if (buf_in->num_planes > 2) {
				addr_rd_v = buf_in->plane[2].addr + buf_in->plane[2].bytesperline * (buf_in->height / 2 - 1);
			}
			break;
		case ROT_180_DEG:
			/* lower right pixel */
			if (buf_in->num_planes > 0) {
				addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * (buf_in->height - 1) + buf_in->plane[0].usedperline - 1;
			}
			if (buf_in->num_planes > 1) {
				addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * (buf_in->height / 2 - 1) + buf_in->plane[1].usedperline - 1;
			}
			if (buf_in->num_planes > 2) {
				addr_rd_v = buf_in->plane[2].addr + buf_in->plane[2].bytesperline * (buf_in->height / 2 - 1) + buf_in->plane[2].usedperline - 1;
			}
			break;
		case ROT_270_DEG:
		case ROT_MIR:
			/* upper right pixel */
			if (buf_in->num_planes > 0) {
				addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].usedperline - 1;
			}
			if (buf_in->num_planes > 1) {
				addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].usedperline - 1;
			}
			if (buf_in->num_planes > 2) {
				addr_rd_v = buf_in->plane[2].addr + buf_in->plane[2].usedperline - 1;
			}
			break;
		default:
			CAMLOGE("cam_ire_base_addr_transform: transfer [%d]", rot);
			rc = -1;
			break;
		}
	}

	CAMLOGD("buf in  p1[0x%08x], p2[0x%08x], p3[0x%08x]",
	       addr_rd_y, addr_rd_u, addr_rd_v);

	addr_wr_y = buf_ot->plane[0].addr;
	addr_wr_u = buf_ot->plane[1].addr;
	addr_wr_v = buf_ot->plane[2].addr;
	CAMLOGD("buf out p1[0x%08x], p2[0x%08x], p3[0x%08x]",
	      addr_wr_y, addr_wr_u, addr_wr_v);

	ire_set_addr(addr_rd_y, addr_rd_u, addr_rd_v, addr_wr_y, addr_wr_u, addr_wr_v);

	return rc;
}

/*
 * cam_offline_init() should be invoked only once at bootup
 */
int cam_offline_init(void)
{
	CAMLOGI(" ire_mutex_init = %d",g_cam_offline_ctx.ire_mutex_init);
	if(!g_cam_offline_ctx.ire_mutex_init){
		offline_power_on(MODULE_CAMERA);

		//scaler - rotation
		g_offline_interrupt_flag = UOS_CreateFlag();
		offline_irq_init(); // only init once
		g_cam_offline_ctx.ire_work_mutex = UOS_NewMutex("cam_offline");
		g_cam_offline_ctx.ire_mutex_init = 1;

		offline_power_off(MODULE_CAMERA);
	} else {
		CAMLOGI(" cam_offline already inited");
	}

	return 0;
}

int cam_offline_deinit(void)
{
	CAMLOGI(" ire_mutex_init = %d");
	if(g_cam_offline_ctx.ire_mutex_init){
		offline_irq_deinit();
		UOS_DeleteFlag(g_offline_interrupt_flag);

		UOS_FreeMutex(g_cam_offline_ctx.ire_work_mutex);
		g_cam_offline_ctx.ire_mutex_init = 0;
	} else {
		CAMLOGI(" cam_offline not inited");
	}

	return 0;
}

int cam_offline_scaler_zoom(struct cam_offline_buf* buf_src, struct cam_offline_buf* buf_dst, int zoom_level, int module_id)
{
	int rc = 0;
	uint32_t i = 0;
	uint32_t w_in, h_in, w_ot, h_ot;
	uint32_t w_crop, h_crop;
	uint32_t actual_flags = 0;
	uint32_t mask = IRQ_SCL_WF_OVERRUN | IRQ_SCL_EOF;

	if (zoom_level < 0x100 || zoom_level > 0x400) {
		CAMLOGE("zoom_level 0x%x error!", zoom_level);
		return -1;
	}

	if (module_id >= MODULE_MAX) {
		CAMLOGE("error module_id %d!", module_id);
		return -1;
	}

	if(buf_src == NULL || buf_dst == NULL) {
		CAMLOGE("offline buf is NULL!!!");
		return -1;
	}
	
	rc = offline_buf_init(buf_src);
	if (rc < 0)
		return rc;

	rc = offline_buf_init(buf_dst);
	if (rc < 0)
		return rc;

	if(g_cam_offline_ctx.ire_mutex_init == 0) {
		CAMLOGE("####camera offline unintialized####");
		return -1;
	}

	w_in = buf_src->width;
	h_in = buf_src->height;
	w_ot = buf_dst->width;
	h_ot = buf_dst->height;
	rc = calc_zoom_crop_size(w_in, h_in, &w_crop, &h_crop, NULL, NULL, zoom_level);
	if (rc < 0)
		return rc;

	UOS_TakeMutex(g_cam_offline_ctx.ire_work_mutex);
	offline_power_on(module_id);

	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_1) {
		/* workaround: reset 0x50 bit8 and bit24, only for crane z1 ASIC */
		offline_s_reset();
	}

	rc = scl_ctrl_cfg(w_crop, h_crop, w_ot, h_ot,
		buf_src->plane[0].pitch, buf_dst->plane[0].pitch, buf_src->pixelformat);
	if (rc < 0)
		goto cfg_fail;

	rc = zoom_base_addr_transform(buf_src, buf_dst, zoom_level);
	if (rc < 0)
		goto addr_fail;

	scl_trigger();
	
	CAMLOGV("start to wait for scl eof!");
    while (i++ < 4) {
		rc = UOS_WaitFlag(g_offline_interrupt_flag, mask, OSA_FLAG_OR_CLEAR,
			&actual_flags, OFFLINE_SCL_TIMEOUT);
		if (OS_SUCCESS == rc) {
			if (actual_flags & IRQ_SCL_EOF) {
				CAMLOGD(" Buffer DONE");
			} else {
				CAMLOGE(" Buffer ERR");
				scl_ire_regs_dump();
			}
			break;
		}
    }
	if(i >= 4){
		CAMLOGE(" Buffer timeout!");
		scl_ire_regs_dump();
	}

cfg_fail:
addr_fail:
	offline_power_off(module_id);
	UOS_ReleaseMutex(g_cam_offline_ctx.ire_work_mutex);

	return rc;
}

int cam_offline_rotation(struct cam_offline_buf* buf_src, struct cam_offline_buf* buf_dst, enum offline_rot rot, int module_id)
{
	int rc = 0;
	uint32_t i = 0;
	uint32_t actual_flags = 0;
	uint32_t mask = IRQ_DMAC_RD_CH0_ERR | IRQ_DMAC_RD_CH1_ERR | IRQ_DMAC_WR_CH0_ERR
		| IRQ_DMAC_WR_CH1_ERR | IRQ_IRE_EOF;

	if((rot == ROT_0_DEG) || (rot >= ROT_MAX)){
		CAMLOGE("rotation %d invalid, just return!!!", rot);
		return -1;
	}

	if (module_id >= MODULE_MAX){
		CAMLOGE("error module_id %d!", module_id);
		return -1;
	}

	if(buf_src == NULL || buf_dst == NULL){
		CAMLOGE("offline buf is NULL!!!");
		return -1;
	}
	
	rc = offline_buf_init(buf_src);
	if (rc < 0)
		return rc;

	rc = offline_buf_init(buf_dst);
	if (rc < 0)
		return rc;

	if(g_cam_offline_ctx.ire_mutex_init == 0) {
		CAMLOGE("####camera offline unintialized####");
		return -1;
	}

	UOS_TakeMutex(g_cam_offline_ctx.ire_work_mutex);
	offline_power_on(module_id);

	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_3) {
		// TODO: set ire dma_mode to ?
		rc = ire_ctrl_cfg_v2(buf_src->width, buf_src->height, buf_src->plane[0].pitch, buf_dst->plane[0].pitch, buf_src->pixelformat, rot, 1);
	}else{
		rc = ire_ctrl_cfg(buf_src->width, buf_src->height, buf_src->plane[0].pitch, buf_dst->plane[0].pitch, buf_src->pixelformat, rot);
	}
	if (rc < 0)
		goto cfg_fail;

	rc = ire_base_addr_transform(buf_src, buf_dst, rot);
	if (rc < 0)
		goto addr_fail;

	ire_trigger(); // ire stream on
	CAMLOGV("cam_offline_rotation: start to wait for ire eof irq!");
    while (i++ < 4) {
		rc = UOS_WaitFlag(g_offline_interrupt_flag, mask, OSA_FLAG_OR_CLEAR,
			&actual_flags, OFFLINE_IRE_TIMEOUT);
		if (OS_SUCCESS == rc) {
			if (actual_flags & IRQ_IRE_EOF) {
				CAMLOGD(" Buffer DONE");
			} else {
				CAMLOGE(" Buffer ERR");
				scl_ire_regs_dump();
			}
			break;
		}
    }
	if(i >= 4){
		CAMLOGE(" Buffer timeout!");
		scl_ire_regs_dump();
	}

cfg_fail:
addr_fail:
	offline_power_off(module_id);
	UOS_ReleaseMutex(g_cam_offline_ctx.ire_work_mutex);

	return rc;
}

// copy memory in the reverse order
static uint32_t *cropwin_cpy(uint32_t src, uint32_t dst, uint32_t count)
{
	uint32_t *d;
	const uint32_t *s;
	uint32_t c = count >> 2;

	s = (const uint32_t *)(src + count - 4);
	d = (uint32_t *)(dst + count - 4);

//	CacheInvalidateMemory((void *)dst, count);
	while (c > 0) {
		*d-- = *s--;
		c--;
	}
	CacheCleanMemory((void *)dst, count);
	/* flush cache to psram after draw_images */
	//dcache_clean_invalidate_range(dst, count);

	return d;
}

static int cropwin_to_lower_right(struct cam_offline_buf *buf_in, struct cam_offline_buf *buf_ot,
				    uint32_t zoom)
{
	int rc = 0;
	uint32_t crop_start_x, crop_start_y;
	uint32_t crop_w, crop_h;
	uint32_t addr_rd_y, addr_rd_u, addr_rd_v;
	uint32_t addr_wr_y, addr_wr_u, addr_wr_v;
	uint32_t shift_start_x, shift_start_y;
	uint32_t shift_rd_y, shift_rd_u, shift_rd_v;
	uint32_t count_y, count_u, count_v;

	if (zoom < 0x100 || zoom > 0x400) {
		CAMLOGE("invalid zoom ratio 0x%x!!!", zoom);
		return -1;
	}

	calc_zoom_crop_size(buf_in->width, buf_in->height, &crop_w, &crop_h, &crop_start_x, &crop_start_y, zoom);
	shift_start_x = buf_in->width - crop_w;
	shift_start_y = buf_in->height - crop_h;
	CAMLOGD("shift window (%d, %d) (%d, %d)",
		shift_start_x, shift_start_y, buf_in->width,  buf_in->height);

	switch (buf_in->pixelformat) {
	case SCALER_INPUT_FMT_YUV422_YUYV: /*1 planar*/
	case SCALER_INPUT_FMT_YUV422_YVYU:
	case SCALER_INPUT_FMT_YUV422_UYVY:
	case SCALER_INPUT_FMT_YUV422_VYUY:
		addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * crop_start_y + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_u = 0;
		addr_rd_v = 0;
		shift_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * shift_start_y + (buf_in->bpp >> 3) * shift_start_x;
		shift_rd_u = 0;
		shift_rd_v = 0;
		count_y = buf_in->plane[0].bytesperline * crop_h - (buf_in->bpp >> 3) * shift_start_x;
		count_u = 0;
		count_v = 0;
		break;
	case SCALER_INPUT_FMT_YUV420_NV12: /*2 planar*/
	case SCALER_INPUT_FMT_YUV420_NV21:
		addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * crop_start_y + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * crop_start_y / 2 + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_v = 0;
		shift_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * shift_start_y + (buf_in->bpp >> 3) * shift_start_x;
		shift_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * shift_start_y / 2 + (buf_in->bpp >> 3) * shift_start_x;
		shift_rd_v = 0;
		count_y = buf_in->plane[0].bytesperline * crop_h - (buf_in->bpp >> 3) * shift_start_x;
		count_u =  buf_in->plane[0].bytesperline * crop_h / 2 - (buf_in->bpp >> 3) * shift_start_x;
		count_v = 0;
		break;
	case SCALER_INPUT_FMT_YUV420_I420: /*3 planar  Y, U, V */
	case SCALER_INPUT_FMT_YUV420_YV12: /*3 planar  Y, V, U */
		addr_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * crop_start_y + (buf_in->bpp >> 3) * crop_start_x;
		addr_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * crop_start_y / 2 + (buf_in->bpp >> 3) * crop_start_x / 2;
		addr_rd_v = buf_in->plane[2].addr + buf_in->plane[2].bytesperline * crop_start_y / 2 + (buf_in->bpp >> 3) * crop_start_x / 2;
		shift_rd_y = buf_in->plane[0].addr + buf_in->plane[0].bytesperline * shift_start_y + (buf_in->bpp >> 3) * shift_start_x;
		shift_rd_u = buf_in->plane[1].addr + buf_in->plane[1].bytesperline * shift_start_y / 2 + (buf_in->bpp >> 3) * shift_start_x / 2;
		shift_rd_v = buf_in->plane[2].addr + buf_in->plane[2].bytesperline * shift_start_y / 2 + (buf_in->bpp >> 3) * shift_start_x / 2;
		count_y = buf_in->plane[0].bytesperline * crop_h - (buf_in->bpp >> 3) * shift_start_x;
		count_u = buf_in->plane[0].bytesperline * crop_h / 2 - (buf_in->bpp >> 3) * shift_start_x / 2;
		count_v = count_u;
		break;
	default:
		CAMLOGE("invalid fmt %d", buf_in->pixelformat);
		return -1;
	}
	CAMLOGD("buf in  p1[0x%08x], p2[0x%08x], p3[0x%08x]",
		addr_rd_y, addr_rd_u, addr_rd_v);

	addr_wr_y = buf_ot->plane[0].addr;
	addr_wr_u = buf_ot->plane[1].addr;
	addr_wr_v = buf_ot->plane[2].addr;
	CAMLOGD("buf out p1[0x%08x], p2[0x%08x], p3[0x%08x]",
		addr_wr_y, addr_wr_u, addr_wr_v);

	if (count_y)
		cropwin_cpy(addr_rd_y, shift_rd_y, count_y);
	if (count_u)
		cropwin_cpy(addr_rd_u, shift_rd_u, count_u);
	if (count_v)
		cropwin_cpy(addr_rd_v, shift_rd_v, count_v);

	scl_set_addr(shift_rd_y, shift_rd_u, shift_rd_v,
		     addr_wr_y, addr_wr_u, addr_wr_v);

	return rc;
}

/* be careful, zoom in the same buffer */
int cam_offline_homo_zoom(struct cam_offline_buf* buf_src, struct cam_offline_buf* buf_dst,
		int zoom_level, int module_id)
{
	int rc = 0;
	uint32_t i = 0;
	uint32_t w_in, h_in, w_ot, h_ot;
	uint32_t w_crop, h_crop;
	uint32_t actual_flags = 0;
	uint32_t mask = IRQ_SCL_WF_OVERRUN | IRQ_SCL_EOF;
//	CAMLOGD("buf_src->plane_addr[0] 0x%p [%d x %d] ",buf_src->plane_addr[0],buf_src->width,buf_src->height);
//	CAMLOGD("buf_dst->plane_addr[0] 0x%p [%d x %d] stride= [%d]",buf_dst->plane_addr[0],buf_dst->width,buf_dst->height,buf_dst->stride);

	if (zoom_level < 0x100 || zoom_level > 0x400) {
		CAMLOGE("zoom_level 0x%x error!", zoom_level);
		return -1;
	}

	if (module_id >= MODULE_MAX) {
		CAMLOGE("error module_id %d!", module_id);
		return -1;
	}

	if(buf_src->plane_addr[0] != buf_dst->plane_addr[0]) {
		CAMLOGE("are you sure to do homo zoom?");
		return -1;
	}

	/* enlarge dst buffer stride to avoid memory stomp by scaler */
	if (buf_dst->width > 640)
		buf_dst->stride = buf_src->width;

	rc = offline_buf_init(buf_src);
	if (rc < 0)
		return rc;

	rc = offline_buf_init(buf_dst);
	if (rc < 0)
		return rc;

	if(g_cam_offline_ctx.ire_mutex_init == 0) {
		CAMLOGE("####camera offline unintialized####");
		return -1;
	}

	w_in = buf_src->width;
	h_in = buf_src->height;
	w_ot = buf_dst->width;
	h_ot = buf_dst->height;
	calc_zoom_crop_size(w_in, h_in, &w_crop, &h_crop, NULL, NULL, zoom_level);

	UOS_TakeMutex(g_cam_offline_ctx.ire_work_mutex);
	offline_power_on(module_id);

	if(CONFIG_CAM_HW_VERSION == CAM_HW_VERSION_1) {
		/* workaround: reset 0x50 bit8 and bit24, only for crane z1 ASIC */
		offline_s_reset();
	}

	rc = cropwin_to_lower_right(buf_src, buf_dst, zoom_level);
	if (rc < 0)
		goto addr_fail;

	rc = scl_ctrl_cfg(w_crop, h_crop, w_ot, h_ot,
		buf_src->plane[0].pitch, buf_dst->plane[0].pitch, buf_src->pixelformat);
	if (rc < 0)
		goto cfg_fail;

	scl_trigger();
	
	CAMLOGV("start to wait for scl eof!");
	while (i++ < 4) {
		rc = UOS_WaitFlag(g_offline_interrupt_flag, mask, OSA_FLAG_OR_CLEAR,
			&actual_flags, OFFLINE_SCL_TIMEOUT);
		if (OS_SUCCESS == rc) {
			if (actual_flags & IRQ_SCL_EOF) {
				CAMLOGD(" Buffer DONE");
			} else {
				CAMLOGE(" Buffer ERR");
				scl_ire_regs_dump();
			}
			break;
		}
	}
	if(i >= 4){
		CAMLOGE(" Buffer timeout!");
		scl_ire_regs_dump();
	}
	
cfg_fail:
addr_fail:
	offline_power_off(module_id);
	UOS_ReleaseMutex(g_cam_offline_ctx.ire_work_mutex);

	return rc;
}

int CONFIG_CAM_HW_VERSION = CAM_HW_VERSION_3;
int cam_hw_version_init(void)
{
	if(CHIP_IS_CRANE_Z1){
		CONFIG_CAM_HW_VERSION = CAM_HW_VERSION_1;
		CAMLOGI("CAM_HW_VERSION is Z1");
	}else if(CHIP_IS_CRANE_Z2){
		CONFIG_CAM_HW_VERSION = CAM_HW_VERSION_2;
		CAMLOGI("CAM_HW_VERSION is Z2");
	}else if (IsChipCrane()) {
		if(IsChip_Crane_A0_or_Above()) {
			CONFIG_CAM_HW_VERSION = CAM_HW_VERSION_3;
			CAMLOGI("CAM_HW_VERSION is A0 or above");
		} else {
			CAMLOGI("unknown CAM_HW_VERSION ,set as default CAM_HW_VERSION_3");
		}
	} else {
			CAMLOGI("unknown CAM_HW_VERSION ,set as default CAM_HW_VERSION_3");
	}
	CAMLOGI("CONFIG_CAM_HW_VERSION = 0x%x",CONFIG_CAM_HW_VERSION);

	return CONFIG_CAM_HW_VERSION;
}
