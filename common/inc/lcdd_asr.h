#ifndef _LCDD_ASR_H_
#define _LCDD_ASR_H_

#include "panel_drv.h"
#ifdef __cplusplus
extern "C" {
#endif

enum{
	ASRLCDD_CB_TYPE_DONE,
	ASRLCDD_CB_TYPE_BUFDONE,
	ASRLCDD_CB_TYPE_MAX
};

enum{
	ASRLCDD_FORMAT_RGB565,
	ASRLCDD_FORMAT_RGB1555,
	ASRLCDD_FORMAT_RGB888_PACKED,
	ASRLCDD_FORMAT_RGB888_UNPACKED,
	ASRLCDD_FORMAT_RGBA888, /*ARGB*/
	ASRLCDD_FORMAT_YUV422_PACKED,
	ASRLCDD_FORMAT_YUV422_PLANAR,
	ASRLCDD_FORMAT_YUV420_PLANAR,
	ASRLCDD_FORMAT_RGB888A,/*RGBA*/
	ASRLCDD_FORMAT_YUV420_SEMI,
	ASRLCDD_FORMAT_LIMIT
};


struct asrlcdd_ctx{
	int img_en;
	unsigned int layer_alpha;
};

struct asrlcdd_screen_info{
    unsigned int width;
    unsigned int height;
};

struct asrlcdd_framebuffer_info{
	void* pbuffer;
	unsigned int height;
	unsigned int stride;
	unsigned int bitdepth;
};

struct asrlcdd_roi_info{
	int startX;
	int startY;
	unsigned short width;
	unsigned short height;
};

struct asrlcdd_framebuffer_window{
	struct asrlcdd_framebuffer_info frame_info;
	struct asrlcdd_roi_info roi_info;
};

struct asrlcdd_camera_buffer{
	unsigned int width;
	unsigned int height;
	unsigned int format;
	unsigned int planes;
	unsigned int addr[3];
	unsigned int stride[3];
};

typedef void (*ASRLCDD_CB_FUNC)(void* param, void* data);



void ASRLCDD_Phase1Init();
void ASRLCDD_Phase2Init();
int ASRLCDD_Close(void);
int ASRLCDD_Sleep(void);
int ASRLCDD_WakeUp(void);
int ASRLCDD_SetBuffers(void* pBuffer, int size, ASRLCDD_CB_FUNC cb, void* param);
#ifdef ASR_LCD_CAMERA_ENABLE
int ASRLCDD_CameraMerge(struct asrlcdd_camera_buffer *pbuffer, unsigned int rotation, struct asrlcdd_roi_info *dest_roi);
#endif
int ASRLCDD_Blit16(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);
int ASRLCDD_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);
int ASRLCDD_FillRect16(struct asrlcdd_roi_info *roi_info, unsigned int bgColor);
int ASRLCDD_SetBrightness(unsigned int brightness);
struct panel_spec * ASRLCDD_GetPanelInfo(void);
int ASRLCDD_GetScreenInfo(struct asrlcdd_screen_info *info);
/*********Code for assert mode********/
int ASRLCDD_Logo_Open(void);
int ASRLCDD_Logo_Close(void);
int ASRLCDD_Ass_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);

#ifdef __cplusplus
}
#endif

#endif /*_LCDD_ASR_H_*/
