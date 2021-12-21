#ifndef _CAM_HAL_TEST_H
#define _CAM_HAL_TEST_H

#define CAMERA_LCD_TEST 1
//#define CAMERA_HAL_SELF_TEST 1

//#define CAMERA_VIDEO_TEST 1
//#define CAMERA_CAPTURE_TEST 1

typedef int (*CameraCallBackHandle)(void *useData, void* pBufIndex);

typedef struct _CameraHalTestCallbackSturct {
	CameraCallBackHandle callBackHandle;
	void *userData;
}CameraHalTestCallbackSturct;

typedef struct _CameraHalTestBufPack {
	void *pBuf;
	int bufIndex;
	int bufStatus; //0 other, 1 us
}CameraHalTestBufPack;


void CameraHalTestDisplayFunction(void *pBuf, int bufIndex);
void CameraHalTestDisplaySetCallBack(CameraCallBackHandle callBackHandle, void *userData);
void CameraHalTestInit(void);
void CameraHalTestVideoFunction(void *pBuf, int bufIndex);
void CameraHalTestVideoSetCallBack(CameraCallBackHandle callBackHandle, void *userData);
void CameraHalTestFlushBuffer(int taskId);

#endif
