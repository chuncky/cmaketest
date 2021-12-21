#include "camera_hal_core.h"

int mci_videocall_camera_openpreview(int width, int height, int fps, void * setReleaseBufferCallBack, void * enqueueCamRecordBuffer, void * flushCamRecordBuffers)
{
	int error = CRANE_CAMERA_ERROR_NONE;
	CamRecordParmeters pVideoRecordParam = {0x0};

	pVideoRecordParam.recordWidth = width;
	pVideoRecordParam.recordHeight = height;
	pVideoRecordParam.recordFramerate = fps;
	pVideoRecordParam.setReleaseBufferCallBack = setReleaseBufferCallBack;
	pVideoRecordParam.enqueueCamRecordBuffer = enqueueCamRecordBuffer;
	pVideoRecordParam.flushCamRecordBuffers = flushCamRecordBuffers;

	error = CCStartVideoCall(&pVideoRecordParam);
	return error;

}
int mci_videocall_camera_stoppreview(void)
{
	int error = CRANE_CAMERA_ERROR_NONE;

	error = CCStopVideoCall();
	return error;
}


