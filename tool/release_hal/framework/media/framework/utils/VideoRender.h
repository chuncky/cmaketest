#ifndef _ASR_FP_MM_VIDEO_OVERLAY_H
#define _ASR_FP_MM_VIDEO_OVERLAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MEDIA_ROTATE_0,
    MEDIA_ROTATE_90,
    MEDIA_ROTATE_180,
    MEDIA_ROTATE_270,
    MEDIA_ROTATE_MIRROR_0,
    MEDIA_ROTATE_MIRROR_90,
    MEDIA_ROTATE_MIRROR_180,
    MEDIA_ROTATE_MIRROR_270,
} rotate_t;

typedef struct VideoRenderParameters {
	uint16_t lcdStartX;
	uint16_t lcdStartY;
	uint16_t lcdEndX;
	uint16_t lcdEndY;
	uint16_t lcdCutW;	
	uint16_t lcdCutH;
	uint16_t displayWidth;
	uint16_t displayHeight;
	rotate_t lcdRotate;
} VideoRenderParameters;


#ifdef __cplusplus
}
#endif

#endif
