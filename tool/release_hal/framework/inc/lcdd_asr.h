#ifndef _LCDD_ASR_H_
#define _LCDD_ASR_H_

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

struct asrlcdd_point{
	unsigned short startX;
	unsigned short startY;
};

struct asrlcdd_buffer_info{
	unsigned int addr;
	unsigned int stride; /*byte*/
};

struct asrlcdd_src_buf_des{
	unsigned int format;
	unsigned short planes;
	unsigned short alpha_value; /*layer alpha*/
	struct asrlcdd_buffer_info buffer[3]; /*input buffer info*/
	struct asrlcdd_roi_info	src_roi; /*the crop info of input buffer*/
	struct asrlcdd_point dest_point; /*the point to lcm*/
};

/*output only support RGB, so only one buffer is OK*/
struct asrlcdd_dest_buf_des{
	unsigned int format;
	struct asrlcdd_buffer_info buffer; /*output buffer info*/
	unsigned int width;
	unsigned int height;
};


typedef void (*ASRLCDD_CB_FUNC)(void* param, void* data);

//do not delete, will be called by other modual
typedef enum 
{
	BG_BLCAK_,
	BG_RED_,
	BG_GREEN_,
	BG_BLUE_,
	BG_WHITE_,
	BG_END_
} LCD_BGCOLOR_INDEX_;


int ASRLCDD_Open(int panel_is_ready);
int ASRLCDD_Close(void);
int ASRLCDD_Sleep(void);
int ASRLCDD_WakeUp(void);
int ASRLCDD_SetBuffers(void* pBuffer, int size, ASRLCDD_CB_FUNC cb, void* param);
void ASRLCDD_alpha_Blending(unsigned int val);
int ASRLCDD_CameraMerge(struct asrlcdd_camera_buffer *pbuffer, unsigned int rotation, struct asrlcdd_roi_info *dest_roi);
int ASRLCDD_CameraPause(void);
int ASRLCDD_CameraContinue(void);
int ASRLCDD_Blit16(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);
int ASRLCDD_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);
int ASRLCDD_FillRect16(struct asrlcdd_roi_info *roi_info, unsigned int bgColor);
int ASRLCDD_SetBrightness(unsigned int brightness);
int ASRLCDD_GetScreenInfo(struct asrlcdd_screen_info *info);
BOOL ASRLCDD_LcdIsActive(void);
void ASRLCDD_SetUIInfo(struct asrlcdd_screen_info *info);
BOOL ASRLCDD_AnyLcdIsActive(void);

/*********Code for uboot logo mode********/
int ASRLCDD_Logo_Open(void);
int ASRLCDD_Logo_Close(void);
int ASRLCDD_Logo_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);

/*********Code for assert logo mode********/
int ASRLCDD_Assert_Open(void);
int ASRLCDD_Assert_Blit16_sync(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);

/*********Code for Hipad****************/

/* Compose the layers, up to 3 layers.
*  Support write back to memory .
*
* img_buffer: the buffer descripiton of image layer. NULL means not use this layer.
* osd1_buffer: the buffer descripiton of osd1 layer. NULL means not use this layer.
* dest_buffer: the buffer descripiton of write back. NULL means not need to write back.
*
* Return 0 mean ok. Return others mean fail.
*/
int ASRLCDD_Capture(struct asrlcdd_src_buf_des *img_buffer,
	struct asrlcdd_src_buf_des *osd1_buffer,
	struct asrlcdd_dest_buf_des *dest_buffer);

/* Compose the layers, up to 3 layers.
*  Support refresh to panel .
*
* img_buffer: the buffer descripiton of image layer. NULL means not use this layer.
* osd1_buffer: the buffer descripiton of osd1 layer. NULL means not use this layer.
*
* Return 0 mean ok. Return others mean fail.
*/
int ASRLCDD_Refresh(struct asrlcdd_src_buf_des *img_buffer,
	struct asrlcdd_src_buf_des *osd1_buffer);

void ASRLCDD_Read_Panel_ID(void);

#ifdef LCD_DUAL_PANEL_SUPPORT
int ASRLCDD_GetSubScreenInfo(struct asrlcdd_screen_info *info);
int ASRLCDD_SubSleep(void);
int ASRLCDD_SubWakeUp(void);
int ASRLCDD_SubBlit16(struct asrlcdd_framebuffer_window *frame_window,
	unsigned short startX, unsigned short startY);
int ASRLCDD_SubFillRect16(struct asrlcdd_roi_info *roi_info, unsigned int bgColor);
int ASRLCDD_SetSubBrightness(uint32_t brightness);
BOOL ASRLCDD_SubLcdIsActive(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /*_LCDD_ASR_H_*/
