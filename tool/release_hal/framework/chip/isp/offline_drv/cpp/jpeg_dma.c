#include <plat_types.h>
#include "../../camera_common_log.h"

#include "jpeg_dma.h"

/* close CAMLOGD */
#undef CAMLOGD
#define CAMLOGD(fmt, args...) do {} while(0)

/* macro for regs */
#define REG_OFFLINE_BASE 0xD420F000

/* offline JPEG DMA registers */
#define REG_JDMA_TOP_CTRL 0x90
#define REG_JDMA_IMG_SIZE 0x94
#define REG_JDMA_RD_YADDR_BASE 0x98
#define REG_JDMA_RD_UADDR_BASE 0x9c
#define REG_JDMA_RD_YPITCH 0xa0
#define REG_JDMA_RD_YSTRIDE 0xa4
#define REG_JDMA_RD_USTRIDE 0xa8

#define JDMA_TRIGGER_FRAME (0x1 << 0)

#define JDMA_ROT_ANGLE_MASK (0x3 << 1)
#define JDMA_DEGREE_0   (0x0 << 1)
#define JDMA_DEGREE_90  (0x1 << 1)
#define JDMA_DEGREE_180 (0x2 << 1)
#define JDMA_DEGREE_270 (0x3 << 1)

#define JDMA_IMG_W_MASK (0xfff << 0)
#define JDMA_IMG_H_MASK (0xfff << 16)

#define JDMA_PITCH_Y_MASK (0xffff << 0)
#define JDMA_PITCH_U_MASK (0xffff0000)

static uint32_t jdma_reg_read(uint32_t reg)
{
	uint32_t val = 0xBABABABA;
	val = BUREG_READ(REG_OFFLINE_BASE + reg);
#ifdef JPGDMA_REG_DUMP
	CAMLOGE("r 0x%08x = 0x%08x\r\n", REG_OFFLINE_BASE + reg, val);
#endif

	return val;
}

static void jdma_reg_write(uint32_t reg, uint32_t val)
{
	BUREG_WRITE(REG_OFFLINE_BASE + reg, val);
#ifdef JPGDMA_REG_DUMP
	CAMLOGE("w 0x%08x = 0x%08x\r\n", REG_OFFLINE_BASE + reg, val);
#endif
}

static void jdma_write_mask(uint32_t reg, uint32_t val, uint32_t mask)
{
	uint32_t v;

	v = jdma_reg_read(reg);
	v = (v & ~mask) | (val & mask);
	jdma_reg_write(reg, v);
}

static void jdma_set_bit(uint32_t reg, uint32_t val)
{
	jdma_write_mask(reg, val, val);
}

void jdma_clr_bit(uint32_t reg, uint32_t val)
{
	jdma_write_mask(reg, 0, val);
}

void jdma_trigger(void)
{
	CAMLOGD("%s E", __func__);
	jdma_set_bit(REG_JDMA_TOP_CTRL, JDMA_TRIGGER_FRAME);
}

static int jdma_set_top(uint32_t rot)
{
	uint32_t angle;

	switch (rot) {
	case 0:
		angle = JDMA_DEGREE_0;
		break;
	case 90:
		angle = JDMA_DEGREE_90;
		break;
	case 180:
		angle = JDMA_DEGREE_180;
		break;
	case 270:
		angle = JDMA_DEGREE_270;
		break;
	default:
		CAMLOGE("%s: rot %d err", rot);
		return -1;
	}
	jdma_write_mask(REG_JDMA_TOP_CTRL, angle, JDMA_ROT_ANGLE_MASK);

	return 0;
}

static int jdma_set_img_size(uint32_t w_in, uint32_t h_in)
{
	uint32_t in_size;

	CAMLOGD("%s E, w_in=0x%08x, h_in=0x%08x",
	       __func__, w_in, h_in);
	in_size = (w_in & JDMA_IMG_W_MASK) | ((h_in << 16) & JDMA_IMG_H_MASK);
	jdma_reg_write(REG_JDMA_IMG_SIZE, in_size);

	return 0;
}

static int jdma_set_rd_pitch(uint32_t ypitch, uint32_t upitch)
{
	uint32_t rd_pitch;

	CAMLOGD("%s E, ypitch=0x%08x, ypitch=0x%08x",
	       __func__, ypitch, upitch);
	rd_pitch = (ypitch & JDMA_PITCH_Y_MASK) | ((upitch << 16) & JDMA_PITCH_U_MASK);
	jdma_reg_write(REG_JDMA_RD_YPITCH, rd_pitch);

	return 0;
}

static int jdma_set_rd_stride(uint32_t ystride)
{
	uint32_t rd_ystride, rd_ustride;

	rd_ystride = ystride;
	rd_ustride = ystride >> 1;
	CAMLOGD("%s E, ystride=0x%08x, ustride=0x%08x",
	       __func__, rd_ystride, rd_ustride);
	jdma_reg_write(REG_JDMA_RD_YSTRIDE, rd_ystride);
	jdma_reg_write(REG_JDMA_RD_USTRIDE, rd_ustride);

	return 0;
}

static int jdma_base_addr_transform(uint32_t base_y_addr, uint32_t base_u_addr, uint32_t *p_y_trans_addr, uint32_t *p_u_trans_addr,
		uint32_t rot, uint32_t width, uint32_t height, uint32_t pitch)
{
	switch (rot) {
	case 0:
		/* left upper */
		*p_y_trans_addr = base_y_addr;
		*p_u_trans_addr = base_u_addr;
		break;
	case 90:
		/* left lower */
		*p_y_trans_addr = base_y_addr + pitch * (height - 1);
		*p_u_trans_addr = base_u_addr + pitch * (height / 2 - 1);
		break;
	case 180:
		/* lower right */
		*p_y_trans_addr = (base_y_addr + pitch * (height - 1) + width) - 16;
		*p_u_trans_addr = (base_u_addr + pitch * (height / 2 - 1) + width) - 16;
		break;
	case 270:
		/* upper right */
		*p_y_trans_addr = (base_y_addr + width) - 16;
		*p_u_trans_addr = (base_u_addr + width) - 16;
		break;
	default:
		CAMLOGE("%s: rot %d err", __func__, rot);
		return -1;
	}

	return 0;
}

static int jdma_set_addr(uint32_t y_addr, uint32_t u_addr)
{
	CAMLOGD("%s E, y_addr=0x%08x, u_addr=0x%08x",
	       __func__, y_addr, u_addr);

	jdma_reg_write(REG_JDMA_RD_YADDR_BASE, y_addr);
	jdma_reg_write(REG_JDMA_RD_UADDR_BASE, u_addr);

	return 0;
}

/*
 * input yuv420 as default
 */
int jdma_cfg(uint32_t width, uint32_t height, uint32_t pitch,
	     uint32_t rot, uint32_t y_base_addr, uint32_t u_base_addr)
{
	int rc = 0;
	uint32_t y_trans_addr, u_trans_addr;
	uint32_t ystride;

	if ((width & 0xF) || (height & 0xF)) {
		CAMLOGE("%s: jpeg encoder src img %dx%d not 16 byte aligned",
		       __func__, width, height);
		return -1;
	}
	rc = jdma_set_top(rot);
	if (rc)
		goto out;

	jdma_set_img_size(width, height);

	rc = jdma_base_addr_transform(y_base_addr, u_base_addr,
		&y_trans_addr, &u_trans_addr, rot, width, height, pitch);
	if (rc)
		goto out;
	jdma_set_addr(y_trans_addr, u_trans_addr);

	/* rdma require fmt yuv420 ypitch = upitch */
	jdma_set_rd_pitch(pitch, pitch);

	ystride = pitch * height;
	jdma_set_rd_stride(ystride);

out:
	return rc;
}

int jdma_regs_dump(void)
{
	CAMLOGI("%s E", __func__);
	jdma_reg_read(REG_JDMA_TOP_CTRL);
	jdma_reg_read(REG_JDMA_IMG_SIZE);
	jdma_reg_read(REG_JDMA_RD_YADDR_BASE);
	jdma_reg_read(REG_JDMA_RD_UADDR_BASE);
	jdma_reg_read(REG_JDMA_RD_YPITCH);
	jdma_reg_read(REG_JDMA_RD_YSTRIDE);
	jdma_reg_read(REG_JDMA_RD_USTRIDE);

	return 0;
}

int jdma_regs_check(void)
{
	CAMLOGI("%s E", __func__);
	jdma_reg_write(REG_JDMA_TOP_CTRL, 0xffffffff);
	jdma_reg_write(REG_JDMA_IMG_SIZE, 0xffffffff);
	jdma_reg_write(REG_JDMA_RD_YADDR_BASE, 0xffffffff);
	jdma_reg_write(REG_JDMA_RD_UADDR_BASE, 0xffffffff);
	jdma_reg_write(REG_JDMA_RD_YPITCH, 0xffffffff);
	jdma_reg_write(REG_JDMA_RD_YSTRIDE, 0xffffffff);
	jdma_reg_write(REG_JDMA_RD_USTRIDE, 0xffffffff);

	return 0;
}
