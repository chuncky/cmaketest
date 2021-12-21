#ifndef _CAM_SOFT_JPEG_H_
#define _CAM_SOFT_JPEG_H_

#include "../online_drv/cam_pipeline.h"

typedef struct _CCameraSofJpegInfo {
	unsigned int addr[3];
    unsigned int width;
    unsigned int height;	
	unsigned int bufferLen;
}CCameraSofJpegInfo;

unsigned int CameraSoftJpegEncode(CCameraSofJpegInfo *inJpegInfo, CCameraSofJpegInfo *outJpegInfo, int quality, int orientation);
int CameraCodaJpegEncode(CCameraSofJpegInfo *inJpegInfo, CCameraSofJpegInfo *outJpegInfo, int quality, int orientation);
int CameraOnlineJpegEncode(struct pipeline_param *pipeline, uint32_t output_start_addr, uint32_t size, int quality, int enablePreview);


#endif //end _CAM_SOFT_JPEG_H_
