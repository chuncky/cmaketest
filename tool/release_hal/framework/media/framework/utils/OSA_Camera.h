#ifndef _ASR_FP_MM_OSA_CAM_H
#define _ASR_FP_MM_OSA_CAM_H

#ifdef __cplusplus
extern "C" {
#endif

#define MMF_DUMMY_CAMERA (0)

#if (MMF_DUMMY_CAMERA)

typedef int (*ReleaseBufferCallback)(void *, void *);

typedef struct CamRecordParmeters {
	unsigned recordWidth;
	unsigned recordHeight;
	unsigned recordFormat;
	unsigned recordFramerate;
	void (*setReleaseBufferCallBack)(ReleaseBufferCallback callBackHandle, void *userData);
	void (*enqueueCamRecordBuffer)(void *pBuf, int bufIndex);
	void (*flushCamRecordBuffers)(void);
} CamRecordParmeters;

#else

#include "../../../chip/isp/cam-hal/camera_hal_gui_interface.h"

#endif

int mmf_camera_start_preview(void *preview_params);
int mmf_camera_stop_preview(void);
int mmf_camera_set_paramter(signed item, signed value);
int mmf_camera_start_video_record(CamRecordParmeters *record_params);
int mmf_camera_stop_video_record(void);

#ifdef __cplusplus
}
#endif

#endif
