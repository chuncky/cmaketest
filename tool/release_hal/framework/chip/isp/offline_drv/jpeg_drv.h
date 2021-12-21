#ifndef __JPEG_DRV_H__
#define __JPEG_DRV_H__


#define JPU_TIMEOUT 50 //TODO: 
enum jpu_mode {
	OFFLINE_ENC_MODE,
	ONLINE_ENC_MODE,
	OFFLINE_DEC_MODE,
	ONLINE_DEC_MODE, //unsupported
};

enum jpu_state {
	JPU_STATE_BOOT,
	JPU_STATE_IDLE,
	JPU_STATE_ACTIVE,
	JPU_STATE_OFF,
};

typedef enum {
	JPEG_QUALITY_LOW = 0,
	JPEG_QUALITY_NORMAL,
	JPEG_QUALITY_FINE,
	JPEG_QUALITY_SUPERFINE,
	JPEG_QUALITY_MAX
} JpegQuality;

struct jpu_device {
	enum jpu_state state;
	uint32_t jpeg_irq;
	uint32_t afbc_irq;
	uint32_t jpu_open_cnt;
};

// APIs
int jpeg_quality_set(int level);
int jpeg_enc_cfg(uint32_t in_w, uint32_t in_h, uint32_t out_addr);
int cam_jpu_encopen(int mode);
uint32_t cam_jpu_encfinish(void);
int cam_jpu_encclose(void);

#endif
