
#include "camera_hal_core.h"
#include "camera_hal_drv_ops.h"
#include "camera_jpeg_encoder.h"
#include "task_cnf.h"
#include "plat_basic_api.h"
#include "fs_api.h"
#include "ui_mem_api.h"
#include "tm.h"
#include "stdio.h"
#include "plat_config_defs.h"

#include "lcdd_asr.h"
#include "lowpower_api.h"
//for exif
#include "camera_exif.h"

CCCoreHandle camCoreHandle;
u8 camCoreInit = 0;
xbool rawDump = false;

CCError CCDumpNV12ToFile(uint_32 *buf, uint_32 width,  uint_32 height);
CCError _CCPreCapture(CCCaputreStruct *capture_data);
void _CCPostCapture(void);
CCError _CCWaitCapturePortDone(u8 captureMsgQRef, CCBufferQueue *pBufferQue, int_32 *bufIndex);
CCError _CCCaptureBottomHalfProcess(CCOfflineOption *offlineOption, CCBufferQueue *inBufferQue,
                                    int_32 inOnlineIndex, CCBufferQueue *outBufferQue, int_32 needBackDisplay);

void _CCPostCapture(void);

void isp_set_rawdump_flag(xbool flag)
{
	CAMLOGI("set rawdump flag = %d",flag);
	rawDump = flag;
}

xbool isp_get_rawdump_flag(void)
{
	return rawDump;
}

//others notify we can free all buffers.
void CCFreeBufferNotify(void *useData, void *pOther)
{
	CCCameraDevice *pCamDevice = NULL;
	CCBufferQueue *pBufferQue = NULL;

	if (!useData)
		return;

	pCamDevice = camCoreHandle.pCamDevice;
	if (!pCamDevice) {
		CAMLOGW("the camera had been closed!");
		return;
	}

	pBufferQue = (CCBufferQueue*)useData;

	if (PIPELINE_ID_PREV == pBufferQue->pipeId
		&& (INVALID_FLAG_ID != pBufferQue->bufQueSyncFlagRef)) {
		UOS_SetFlag(pBufferQue->bufQueSyncFlagRef, PREVIEW_QUEUE_FREEBUFFERS, OSA_FLAG_OR);
	}
}

//video encoder call this function to release buffer for us.
int CCReleaseBufferToCamera(void *useData, void* pBufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCBufferQueue *pBufferQue = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	u8   msgQRef = NULL;
	struct isp_buf_msg *enqueueMsg = NULL;
	int bufIndex = -1;

	if (!useData || !pCamDevice || !pBufIndex) {
		CAMLOGE("invalid pointer!");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	bufIndex = *((int*)pBufIndex);
	pBufferQue = (CCBufferQueue*)useData;

	if (bufIndex < 0 || bufIndex > pBufferQue->bufCnt) {
		CAMLOGE("the index %d is invalid!", bufIndex);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	if (!pCamDevice->previewVideoSamePipe) //preview set idle
		setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_IDLE);
	else
		clearBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_VIDEO);

	if (PIPELINE_ID_CAP != pBufferQue->pipeId) {
		//capture buffer needn't to enqueue to driver.
		if (PIPELINE_ID_PREV == pBufferQue->pipeId)
			msgQRef = pCamDevice->previewMsgQRef;
		else if (PIPELINE_ID_VIDEO == pBufferQue->pipeId)
			msgQRef = pCamDevice->videoMsgQRef;

		if (pCamDevice->previewVideoSamePipe && (pCamDevice->camStatus & CAMERA_STATUS_VIDEO))
			msgQRef = pCamDevice->videoMsgQRef;

		enqueueMsg = UOS_MALLOC(sizeof(struct isp_buf_msg));
		if (!enqueueMsg) {
			CAMLOGE("UOS_MALLOC for enqueueMsg failed!");
			return CRANE_CAMERA_ERROR_OUTOFMEMORY;
		}
		enqueueMsg->msg_type = ISP_MSG_TYPE_HAL_EQUEUE;
		enqueueMsg->u.data_eof.buf_idx = bufIndex;
		enqueueMsg->u.data_eof.pipe_id = pBufferQue->pipeId;
		UOS_SendMsg((void *)(enqueueMsg), msgQRef, UOS_SEND_MSG);
	}
	return error;
}

//fullsize stream call this function to release buffer for us.
int CCReleaseFullSizeBufferToCamera(void *useData, void* pBufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCBufferQueue *pBufferQue = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	u8   msgQRef = NULL;
	struct isp_buf_msg *enqueueMsg = NULL;
	int bufIndex = -1;

	if (!useData || !pCamDevice || !pBufIndex) {
		CAMLOGE("invalid pointer");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	bufIndex = *((int*)pBufIndex);
	pBufferQue = (CCBufferQueue*)useData;

	if (bufIndex < 0 || bufIndex > pBufferQue->bufCnt) {
		CAMLOGE("the index %d is invalid!", bufIndex);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_IDLE);

	if (PIPELINE_ID_CAP == pBufferQue->pipeId)
		msgQRef = pCamDevice->fullSizeStreamMsgQRef;

	enqueueMsg = UOS_MALLOC(sizeof(struct isp_buf_msg));
	if (!enqueueMsg) {
		CAMLOGE("UOS_MALLOC for enqueueMsg failed!");
		return CRANE_CAMERA_ERROR_OUTOFMEMORY;
	}
	enqueueMsg->msg_type = ISP_MSG_TYPE_HAL_EQUEUE;
	enqueueMsg->u.data_eof.buf_idx = bufIndex;
	enqueueMsg->u.data_eof.pipe_id = pBufferQue->pipeId;
	UOS_SendMsg((void *)(enqueueMsg), msgQRef, UOS_SEND_MSG);

	return error;
}

CCError _CCDumpBufferStatus(CCBufferQueue *pBufferQue)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int_32 i = 0, bufCnt = 0;

	//txchk_cur_task_stack_print();

	if (!pBufferQue) {
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	bufCnt = pBufferQue->bufCnt;
	for (i = 0; i < bufCnt; i++) {
		CAMLOGI("the %dth buffer status(0x%x) !", i, pBufferQue->bufPack[i].bufStatus);
	}

	return error;
}

CCError _CCTryEnqueueIdleBuffers(CCBufferQueue *pBufferQue)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int_32 i = 0, bufCnt = 0;

	if (!pBufferQue) {
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	bufCnt = pBufferQue->bufCnt;
	for (i = 0; i < bufCnt; i++) {
		if (CC_BUFFER_STATUS_IDLE == pBufferQue->bufPack[i].bufStatus) {
			error = enqueueBuffer(pBufferQue, i, pBufferQue->pipeId);
			if (CRANE_CAMERA_ERROR_NONE != error) {
				CAMLOGE("enqueue the %d buffer failed!", i);
			}
		}
	}

	return error;
}

//uint_32 g_LcdTimeTick = 0;
//uint_32 g_LcdIndex = 0;
CCError _CCReadyToDisplay(CCBufferQueue *pBufferQue, int_32 bufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	struct asrlcdd_camera_buffer lcdCameraBuffer;
	struct asrlcdd_roi_info lcdDestInfo;
#if DUMP_PREVIEW_YUV
	int fileHandle = 0;
#endif
//	uint_32 halTimeTick1 = 0, halTimeTick2 = 0;
	//txchk_cur_task_stack_print();

	if (!pBufferQue) {
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	//set buffer status
	if(pBufferQue->bufPack[bufIndex].bufStatus == CC_BUFFER_STATUS_VIDEO)
		appendBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_DISPLAY);
	else
		setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_DISPLAY);

#if DUMP_PREVIEW_YUV
	if (pBufferQue->dumpBuffer) {
		fileHandle = FS_Open("D://camera_preview.nv12", FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
		if (fileHandle < 0) {
			CAMLOGE("create file for saving camera_preview.nv12 failed, %d!", fileHandle);
		}
		CacheInvalidateMemory((void *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->unitSize);
		FS_Write(fileHandle, (UINT8 *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->unitSize);
		CAMLOGD("dump preview buffer %d!", fileWriteLen);
		if (fileHandle > 0)
			FS_Close(fileHandle);
	}
#endif

//	CCDumpNV12ToFile((uint_32 *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->width, pBufferQue->height);
	lcdCameraBuffer.width = pBufferQue->width;
	lcdCameraBuffer.height = pBufferQue->height;
	lcdCameraBuffer.planes = 2;
	lcdCameraBuffer.stride[0] = pBufferQue->width; //align
	lcdCameraBuffer.stride[1] = pBufferQue->width; //align
	lcdCameraBuffer.addr[0] = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr;
	lcdCameraBuffer.addr[1] = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[1].addr;
	lcdCameraBuffer.format = 9;

	if (!pCamDevice->previewStreamPause){
		if (pCamDevice->previewScale) {
			memset(&lcdDestInfo, 0, sizeof(lcdDestInfo));
			lcdDestInfo.width = pCamDevice->lcdWidth;
			lcdDestInfo.height = pCamDevice->lcdHeight;
			ret = ASRLCDD_CameraMerge(&lcdCameraBuffer, pCamDevice->previewRotationDegree, &lcdDestInfo);
		} else
			ret = ASRLCDD_CameraMerge(&lcdCameraBuffer, pCamDevice->previewRotationDegree, NULL);

		if (ret < 0) {
			CAMLOGW("give the %d buffer to display timeout!", bufIndex);
		}
#if 0
		else {
		halTimeTick1 = timerCountRead(TCR_2);
			if (g_LcdIndex == 0)
				g_LcdTimeTick = halTimeTick1;

			g_LcdIndex++;
			if (((halTimeTick1 - g_LcdTimeTick) / 32) >= 1000) {
				CAMLOGI("lcd display has %d fps!", g_LcdIndex);
				g_LcdIndex = 0;
			}
		}
#endif
	}
	//the buffer can use because lcd rotated the buffer.
	clearBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_DISPLAY);
	if (pCamDevice->camStatus & CAMERA_STATUS_CAPTURE) {
		//stop enqueue buffer when capture, we need reuse the buffer.
		CAMLOGW("stop enqueue buffer for preivew when capture!");
		UOS_SetFlag(pCamDevice->threadSyncFlagRef, CAPTURE_PREVIEW_DONE, OSA_FLAG_OR);
		return error;
	}

	enqueueBuffer(pBufferQue, bufIndex, pBufferQue->pipeId);
	return error;
}

CCError _CCCaptureBackDisplay(CCOfflineParamter *pOfflineParamter, CCBufferQueue *inBufferQue, int_32 inBufIndex,
                    CCBufferQueue *outBufferQue, int_32 outBufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	xbool needScale = false;
	uint_32 lcdWidth =0, lcdHeight =0;
	uint_32 lcdBufSize = 0, jpegBufSize = 0;

	if (!inBufferQue) {
		CAMLOGE("inBufferQue is NULL");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	lcdWidth = pCamDevice->lcdWidth;
	lcdHeight = pCamDevice->lcdHeight;

	if(lcdWidth * lcdHeight > outBufferQue->width * outBufferQue-> height){
		CAMLOGE("outBufferQue is too small to do capture back display !");
		goto Error_exit;
	}

	CAMLOGI("sensor size wxh[%d %d] lcd size wxh[%d %d]", inBufferQue->width, inBufferQue->height,lcdWidth, lcdHeight);
	if ((inBufferQue->width != lcdWidth && inBufferQue->height != lcdWidth)
		|| (inBufferQue->height != lcdHeight && inBufferQue->width != lcdHeight)) {
		//need to scale
		needScale = true;
//		pOfflineParamter->zoomLevel = 0x100;
		pOfflineParamter->format = inBufferQue->bufFormat;
		pOfflineParamter->width = lcdHeight;
		pOfflineParamter->height = lcdWidth;
		pOfflineParamter->rotation = ROT_0_DEG;
	}

	if(needScale){
		outBufferQue->width = lcdHeight;  //for back display,switch width height
		outBufferQue->height= lcdWidth;
#ifdef REUSE_CAPBUF
		//resue jpeg buf for back display src buf
		outBufferQue->addr = inBufferQue->jpeg_addr; 
		outBufferQue->bufPack[outBufIndex].ispBuffer.buffer.planes[0].addr = (uint_32)outBufferQue->addr;
		outBufferQue->bufPack[outBufIndex].ispBuffer.buffer.planes[1].addr = (uint_32)outBufferQue->addr + outBufferQue->width * outBufferQue->height;
#endif
		error = getZoomScale(pOfflineParamter, inBufferQue, inBufIndex, outBufferQue, outBufIndex,false);

		if(error != CRANE_CAMERA_ERROR_NONE){
			goto Error_exit;
		}
	}

	jpegBufSize = inBufferQue->width * inBufferQue->jpeg_head;
	lcdBufSize = lcdWidth * lcdHeight;
	if(lcdBufSize <= jpegBufSize){
		CAMLOGV("capture back display src addr =0x%p ", outBufferQue->addr);
		_CCReadyToDisplay(outBufferQue, outBufIndex);
	}else{
		CAMLOGW("jpeg buffer is too small to do capture back display");
	}

	return error;
Error_exit:

	CAMLOGE("do capture back display fail");
	return error;
}

//uint_32 g_VideoTimeTick = 0;
//uint_32 g_VideoIndex = 0;
CCError _CCReadyToVideoEncoder(CCBufferQueue *pBufferQue, int_32 bufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	char *pBuf = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
//	uint_32 halTimeTick1 = 0;

	if (!pBufferQue) {
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	//set buffer status
	if(pBufferQue->bufPack[bufIndex].bufStatus == CC_BUFFER_STATUS_DISPLAY)
		appendBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_VIDEO);
	else
		setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_VIDEO);


	//to video encoder.
	pBuf = (char*)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr);
	pCamDevice->qBufForVideoEncoder(pBuf, bufIndex);

#if 0
	halTimeTick1 = timerCountRead(TCR_2);
	if (g_VideoIndex == 0)
		g_VideoTimeTick = halTimeTick1;

	g_VideoIndex++;
	if (((halTimeTick1 - g_VideoTimeTick) / 32) >= 1000) {
		CAMLOGI("we give video has %d fps!", g_VideoIndex);
		g_VideoIndex = 0;
	}
#endif

	return error;
}

CCError _CCReadyToSendData(CCBufferQueue *pBufferQue, int_32 bufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	char *pBuf = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
//	uint_32 halTimeTick1 = 0;

	if (!pBufferQue) {
		return CRANE_CAMERA_ERROR_BADPOINTER;
	}

	//set buffer status
	setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_FULLSIZE);

	//CCDumpNV12ToFile((uint_32 *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->width, pBufferQue->height);

	//buffer to user.
	pBuf = (char*)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr);
	pCamDevice->qBufForFullSizeStream(pBuf, bufIndex);

	return error;
}


CCError _CCCheckBuffersBelongToUs(CCBufferQueue *pBufferQue)
{
	int_32 i = 0, bufCnt = 0;
//	int checkResult = 1;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

	bufCnt = pBufferQue->bufCnt;
	for (i = 0; i < bufCnt; i++) {
		if (CC_BUFFER_STATUS_DISPLAY == pBufferQue->bufPack[i].bufStatus
			|| CC_BUFFER_STATUS_VIDEO == pBufferQue->bufPack[i].bufStatus) {
				//checkResult = 0;
				CAMLOGW("the %d buffer(status=%d) is in others, we cannot free!", i, pBufferQue->bufPack[i].bufStatus);
				//assert(checkResult);
				return CRANE_CAMERA_ERROR_FATALERROR;
		}
	}

	return CRANE_CAMERA_ERROR_NONE;
}

CCError allocBuffers(CCBufferQueue *pBufferQue, uint_32 width, uint_32 height, PIPELINE_ID pipeId)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	uint_32 totalSize = 0, unitSize = 0, extraSize = 0,allocHeight = 0;
	uint_8 bufCnt = 0;
	int remainder = 0;
	uint_8 i = 0;

	if (!pBufferQue)
		return CRANE_CAMERA_ERROR_BADPOINTER;

	allocHeight = height + pBufferQue->jpeg_head;
	//check buffer align
	if (pipeId == PIPELINE_ID_PREV || pipeId == PIPELINE_ID_VIDEO) {
		remainder = width % CC_CAM_ALIGN_TO_4;
		if (remainder > 0) {
			CAMLOGE("the width must align to 4!", __func__);
			return CRANE_CAMERA_ERROR_BADARGUMENT;
		}
	} else if (pipeId == PIPELINE_ID_CAP) {
		//   remainder = width % CC_CAM_ALIGN_TO_16;
		if (remainder > 0) {
			CAMLOGE("the width must align to 16!", __func__);
			return CRANE_CAMERA_ERROR_BADARGUMENT;
		}
	}

	bufCnt = pBufferQue->bufCnt;

	switch (pBufferQue->bufFormat) {
		case PIPE_OUT_FMT_YUV420_NV12:
			unitSize = width * allocHeight * 3 / 2;
			break;
		case PIPE_OUT_FMT_JPEG:
			unitSize = width * allocHeight * 3 / 2;
			//for more than 2M(1600x1200) case, jpeg buffer size use 1/4*(NV12 Size), otherwise 1/3*(NV12 Size)
			if(width >= 1600) {
				unitSize = unitSize / 4;
			} else {
				unitSize = unitSize / 3;
			}
			break;
		default:
			CAMLOGE("unsupported format 0x%x!", pBufferQue->bufFormat);
			return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	totalSize = bufCnt * unitSize + extraSize;
	CAMLOGV("malloc 0x%x size!", totalSize);

	//use UI_MALLOC(CP) to make sure the addr is 32bytes aligned,UOS_MALLOC is 4 bytes aligned.
	if (pipeId == PIPELINE_ID_CAP) {
		//the input addr for hardware jpeg encoder must not be 4 bytes aligned,otherwise it could cause image disorder.
		pBufferQue->jpeg_addr = (void*)UI_MALLOC(totalSize);
	} else
		pBufferQue->jpeg_addr = (void*)UOS_MALLOC(totalSize);

 	if (!pBufferQue->jpeg_addr) {
		CAMLOGE("UI_MALLOC 0x%x sizes for BufferQue falied!", totalSize);
		return CRANE_CAMERA_ERROR_OUTOFMEMORY;
 	}
	pBufferQue->addr = (void*)((uint_32)(pBufferQue->jpeg_addr) + (pBufferQue->jpeg_head * width * 3 /2));
	CacheCleanAndInvalidateMemory((void *)(pBufferQue->jpeg_addr), totalSize);

	CAMLOGV("allocBuffers %d sizes for BufferQue(0x%p) successful!", totalSize, pBufferQue->jpeg_addr);
	pBufferQue->totalSize = totalSize;
	pBufferQue->unitSize = unitSize;
	pBufferQue->width = width;
	pBufferQue->height = height;
	pBufferQue->pipeId = pipeId;
	fillIspOnlineBuffer(pBufferQue);
	for ( i = 0; i < bufCnt; i++) {
		setBufferStatus(pBufferQue, i, CC_BUFFER_STATUS_IDLE);
	}

	return error;
}

CCError freeBuffers(CCBufferQueue *pBufferQue)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

	if (!pBufferQue)
		return CRANE_CAMERA_ERROR_BADPOINTER;

	if (!pBufferQue->jpeg_addr)
		return CRANE_CAMERA_ERROR_BADPOINTER;

	CAMLOGV("freeBuffers %d sizes for BufferQue(0x%p) successful!", pBufferQue->totalSize, pBufferQue->jpeg_addr);

	if (PIPELINE_ID_CAP == pBufferQue->pipeId)
		UI_FREE(pBufferQue->jpeg_addr);
	else
		UOS_FREE(pBufferQue->jpeg_addr);

	return error;
}
int CCSendMsgToThread(u8 msgQRef, ISP_MSG_TYPE msg_type, CCBufferQueue *pBufferQue, uint_8 bufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_buf_msg *enqueueMsg = NULL;

	//txchk_cur_task_stack_print();

	if (bufIndex > pBufferQue->bufCnt) {
		CAMLOGE("the index %d is invalid!", bufIndex);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	if (msgQRef == INVALID_MSGQ_ID)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	enqueueMsg = UOS_MALLOC(sizeof(struct isp_buf_msg));
	if (!enqueueMsg) {
		CAMLOGE("UOS_MALLOC for enqueueMsg failed!");
		return CRANE_CAMERA_ERROR_OUTOFMEMORY;
	}
	enqueueMsg->msg_type = msg_type;
	enqueueMsg->u.data_eof.buf_idx = bufIndex;
	enqueueMsg->u.data_eof.pipe_id = pBufferQue->pipeId;
	UOS_SendMsg((void *)(enqueueMsg), msgQRef, UOS_SEND_MSG);
	return error;
}


void CCCamPreviewThread(void* argv)
{
	CCCameraDevice *pCamDevice = NULL;
	u8   msgQRef = NULL;
	struct isp_buf_msg *ispMsg = NULL;
	OSA_STATUS osaStatus = OS_SUCCESS;
	CCBufferQueue *pBufferQue = NULL;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	uint_8 pipe_id = 0, buf_idx = 0;
	uint_32 frameId = 0, waitTime = 60; //timeout unit is tick(5ms)
	uint_32   actual_flags = 0, onlineWaitTime = ONLINE_TIME_OUT;
	CCIspPipeSwitch ispPipeSwitch;
	uint_32 event[UOS_EVT_MBX_SIZE] = {0};

	pCamDevice = (CCCameraDevice *)argv;
	if (!pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL!");
		return;
	}

	msgQRef = pCamDevice->previewMsgQRef;
	pBufferQue = &pCamDevice->previewBufQue;

	onlineWaitTime = 1000 / pCamDevice->minFps * TIMEOUT_FRAME;
	CAMLOGI("CCCamPreviewThread timeout=%dms",onlineWaitTime);
ReadyToWork:
	CAMLOGV("we are ready to work!");
	//wait start work event.
	while(1) {
		osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, PREVIEW_READYWORK, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS && (actual_flags & PREVIEW_READYWORK))
			break;
	}

	CAMLOGV("receive preview ready work,we enter the work loop, msgQRef=%d!", msgQRef);
	do {
		ispMsg = (struct isp_buf_msg*)UOS_WaitMsg(event, msgQRef, onlineWaitTime / TICK_PER_MS);
		//   osaStatus = OSAMsgQRecv(msgQRef, (UINT8*)&ispMsg, sizeof(struct isp_buf_msg), ONLINE_TIME_OUT);
		if (!ispMsg) {
			CAMLOGW("receive camera drv buffer timeout, continue!");
			pCamDevice->frameDropNum++;
		if((1 != pCamDevice->videoOperation) && !pCamDevice->previewVideoSamePipe)
			_CCDumpBufferStatus(pBufferQue);
		continue;
	}

	switch (ispMsg->msg_type) {
		case ISP_MSG_TYPE_EOF:
			/* some buffer ready,we ready to dequeue. */
			pipe_id = ispMsg->u.data_eof.pipe_id;
			buf_idx = ispMsg->u.data_eof.buf_idx;
			error = dequeueBuffer(pBufferQue, buf_idx, pipe_id);
			if (error != CRANE_CAMERA_ERROR_NONE) {
				CAMLOGE("dequeueBuffer the %d buffer failed for 0x%x!", buf_idx, error);
				UOS_FREE((void*)ispMsg);
				ispMsg = NULL;
				continue;
			}
			if ((1 == pCamDevice->videoOperation) && pCamDevice->previewVideoSamePipe
				&& !pCamDevice->videoStreamPause)
				CCSendMsgToThread(pCamDevice->videoMsgQRef, ISP_MSG_TYPE_EOF, pBufferQue, buf_idx);
			/* ready to display. */
			pBufferQue->lastIndexToOthers = buf_idx;
			_CCReadyToDisplay(pBufferQue, buf_idx);
			if (OS_SUSPEND == onlineWaitTime)
				onlineWaitTime = 1000 / pCamDevice->minFps * TIMEOUT_FRAME;
			break;
		case ISP_MSG_TYPE_ERR_DROP_NO_BUF:
			frameId = ispMsg->u.data_drop_no_buf.frame_id;
			CAMLOGW(" the %d buffer lose!", frameId);
			break;
		case ISP_MSG_TYPE_HAL_EQUEUE:
			if (pCamDevice->camStatus & CAMERA_STATUS_CAPTURE) {
				//stop enqueue buffer when capture, we need reuse the buffer, these buffers enqueue by capture process.
				CAMLOGE("stop enqueue buffer for preivew when capture!");
				continue;
			}
			buf_idx = ispMsg->u.data_eof.buf_idx;
			CAMLOGV("enqueue the %d buffer to preview!", buf_idx);
			enqueueBuffer(pBufferQue, buf_idx, pBufferQue->pipeId);
			break;
		case ISP_MSG_TYPE_HAL_FLUSH:
			memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
			ispPipeSwitch.previewSwitch= 1;
			error = flushBuffer(pBufferQue, &ispPipeSwitch);
			if (error != CRANE_CAMERA_ERROR_NONE) {
				CAMLOGE("flushBuffer for preivew falied, %d!", error);
			}
			CAMLOGI("camframe preview DropNum = %d ", pCamDevice->frameDropNum);
			//set event to the main thread
			UOS_SetFlag(pCamDevice->threadSyncFlagRef, PREVIEW_STOPWORK, OSA_FLAG_OR);
			onlineWaitTime = OS_SUSPEND;
			break;
		case ISP_MSG_TYPE_HAL_EXIT:
			CAMLOGV("receive preview hal exit message!");
			memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
			ispPipeSwitch.previewSwitch= 1;
			error = flushBuffer(pBufferQue, &ispPipeSwitch);
			if (error != CRANE_CAMERA_ERROR_NONE) {
				CAMLOGE("flushBuffer for preivew falied, %d!", error);
			}
			CAMLOGI("camframe preview DropNum = %d ", pCamDevice->frameDropNum);

			//notify lcd we stop work and we need free buffers.
			ASRLCDD_CameraMerge(NULL, ROT_90_DEG, NULL);
			//wait for free all buffers flag.
			actual_flags = 0;
			//wait lcd callback we can free rotation buffers,wait 300ms
			osaStatus = UOS_WaitFlag(pBufferQue->bufQueSyncFlagRef, PREVIEW_QUEUE_FREEBUFFERS, OSA_FLAG_OR_CLEAR, &actual_flags, waitTime);
			if (osaStatus != OS_SUCCESS || !(actual_flags & PREVIEW_QUEUE_FREEBUFFERS)) {
				CAMLOGW("OSAFlagWait preivew free buffers falied, errcode=%d, actual_flags=%d!", osaStatus, actual_flags);
			}

			//reset bufferqueue
			//	_CCCheckBuffersBelongToUs(pBufferQue);
			UOS_FREE((void*)ispMsg);
			//set event to the main thread
			UOS_SetFlag(pCamDevice->threadSyncFlagRef, PREVIEW_STOPWORK, OSA_FLAG_OR);
			goto ReadyToWork;
	   default:
	   		CAMLOGW("unknown this msg_type %d!", ispMsg->msg_type);
			break;
	}

	UOS_FREE((void*)ispMsg);
	ispMsg = NULL;

	}while(1);
}

void CCCamVideoThread(void* argv)
{
	CCCameraDevice *pCamDevice = NULL;
	OSA_STATUS osaStatus = 0;
	uint_32   actual_flags = 0;
	u8   msgQRef = NULL;
	CCBufferQueue *pBufferQue = NULL;
	struct isp_buf_msg *ispMsg = NULL;
	uint_8 pipe_id = 0, buf_idx = 0;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	uint_32 frameId = 0;
	uint_32 onlineWaitTime = ONLINE_TIME_OUT;
	CCIspPipeSwitch ispPipeSwitch;
	uint_32 event[UOS_EVT_MBX_SIZE] = {0};

	pCamDevice = (CCCameraDevice *)argv;

	if (!pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL!");
		return;
	}
	onlineWaitTime = 1000 / pCamDevice->minFps * TIMEOUT_FRAME;
	CAMLOGI("CCCamVideoThread timeout=%dms",onlineWaitTime);

	msgQRef = pCamDevice->videoMsgQRef;
	if (pCamDevice->previewVideoSamePipe)
		pBufferQue = &pCamDevice->previewBufQue; //privew and video use one stream
	else
    		pBufferQue = &pCamDevice->videoBufQue;

	//wait start work event.
	while(1) {
		osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, VIDEO_READYWORK, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS && (actual_flags & VIDEO_READYWORK))
			break;
	}

	do {
		ispMsg = (struct isp_buf_msg*)UOS_WaitMsg(event, msgQRef, onlineWaitTime / TICK_PER_MS);
		//osaStatus = OSAMsgQRecv(msgQRef, (UINT8*)&ispMsg, sizeof(struct isp_buf_msg), ONLINE_TIME_OUT);
		if (!ispMsg) {
			CAMLOGW("receive camera drv buffer timeout, continue!");
			if(1 == pCamDevice->videoOperation)
				_CCDumpBufferStatus(pBufferQue);
			continue;
		}

		switch (ispMsg->msg_type) {
			case ISP_MSG_TYPE_EOF:
				/* some buffer ready,we ready to dequeue. */
				pipe_id = ispMsg->u.data_eof.pipe_id;
				buf_idx = ispMsg->u.data_eof.buf_idx;
				error = dequeueBuffer(pBufferQue, buf_idx, pipe_id);
				if (error != CRANE_CAMERA_ERROR_NONE) {
					CAMLOGE("dequeueBuffer the %d buffer failed for 0x%x!", buf_idx, error);
					UOS_FREE((void*)ispMsg);
					ispMsg = NULL;
					continue;
				}

				/* ready to video encode. */
				pBufferQue->lastIndexToOthers = buf_idx;
				_CCReadyToVideoEncoder(pBufferQue, buf_idx);
				break;
			case ISP_MSG_TYPE_ERR_DROP_NO_BUF:
				frameId = ispMsg->u.data_drop_no_buf.frame_id;
				CAMLOGW("the %d buffer lose!", frameId);
				break;
			case ISP_MSG_TYPE_HAL_EQUEUE:
				buf_idx = ispMsg->u.data_eof.buf_idx;
				CAMLOGV("enqueue the %d buffer to video port!", buf_idx);
				//if use privew buffer, pause operation at previewvideo.
				if (pCamDevice->previewVideoSamePipe || !pCamDevice->videoStreamPause)
					enqueueBuffer(pBufferQue, buf_idx, pBufferQue->pipeId);
				break;
			case ISP_MSG_TYPE_HAL_FLUSH:
				if (!pCamDevice->previewVideoSamePipe) {
				        memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
				        ispPipeSwitch.videoSwitch = 1;
				        error = flushBuffer(pBufferQue, &ispPipeSwitch);
				        if (error != CRANE_CAMERA_ERROR_NONE) {
				            CAMLOGE("flushBuffer for video falied, %d!", error);
				        }
				}
				//flush video buffer
				pCamDevice->flushVideoEncoderBuffers();
				CAMLOGI("flushBuffer for video buffer done!");

				if (pCamDevice->previewVideoSamePipe){
					// make sure  buffer for preview preapared when stop video .
					error= enqueueBuffer(pBufferQue, buf_idx, pBufferQue->pipeId);
				}
				//reset bufferqueue
				//	_CCCheckBuffersBelongToUs(pBufferQue);
				UOS_FREE((void*)ispMsg);
				//set event to the main thread
				UOS_SetFlag(pCamDevice->threadSyncFlagRef, VIDEO_STOPWORK, OSA_FLAG_OR);

				return;
			default:
				break;
		}

		UOS_FREE((void*)ispMsg);
		ispMsg = NULL;

	}while (1);
}

void CCCamFullSizeStreamThread(void* argv)
{
	CCCameraDevice *pCamDevice = NULL;
	OSA_STATUS osaStatus = 0;
	uint_32   actual_flags = 0;
	u8   msgQRef = NULL;
	CCBufferQueue *pBufferQue = NULL;
	struct isp_buf_msg *ispMsg = NULL;
	uint_8 pipe_id = 0, buf_idx = 0;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	uint_32 frameId = 0;
	uint_32 onlineWaitTime = ONLINE_TIME_OUT;
	CCIspPipeSwitch ispPipeSwitch;
	uint_32 event[UOS_EVT_MBX_SIZE] = {0};

	pCamDevice = (CCCameraDevice *)argv;

	if (!pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL!");
		return;
	}
	onlineWaitTime = 1000 / pCamDevice->minFps * TIMEOUT_FRAME;
	CAMLOGI("CCCamFullSizeStreamThread timeout=%dms",onlineWaitTime);

	msgQRef = pCamDevice->fullSizeStreamMsgQRef;
	pBufferQue = &pCamDevice->fullSizeStreamBufQue;

	//wait start work event.
	while(1) {
		osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, FULLSIZE_READYWORK, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS && (actual_flags & FULLSIZE_READYWORK))
			break;
	}

	do {
		ispMsg = (struct isp_buf_msg*)UOS_WaitMsg(event, msgQRef, onlineWaitTime / TICK_PER_MS);
		if (!ispMsg) {
			CAMLOGW("receive camera drv buffer timeout, continue!");
			_CCDumpBufferStatus(pBufferQue);
			continue;
		}

		switch (ispMsg->msg_type) {
			case ISP_MSG_TYPE_EOF:
				/* some buffer ready,we ready to dequeue. */
				pipe_id = ispMsg->u.data_eof.pipe_id;
				buf_idx = ispMsg->u.data_eof.buf_idx;
				error = dequeueBuffer(pBufferQue, buf_idx, pipe_id);
				if (error != CRANE_CAMERA_ERROR_NONE) {
					CAMLOGE("dequeueBuffer the %d buffer failed for 0x%x!", buf_idx, error);
					UOS_FREE((void*)ispMsg);
					ispMsg = NULL;
					continue;
				}

				/* ready to video encode. */
				pBufferQue->lastIndexToOthers = buf_idx;
				_CCReadyToSendData(pBufferQue, buf_idx);
				break;
			case ISP_MSG_TYPE_ERR_DROP_NO_BUF:
				frameId = ispMsg->u.data_drop_no_buf.frame_id;
				CAMLOGW("the %d buffer lose!", frameId);
				break;
			case ISP_MSG_TYPE_HAL_EQUEUE:
				buf_idx = ispMsg->u.data_eof.buf_idx;
				CAMLOGV("enqueue the %d buffer to video port!", buf_idx);
				enqueueBuffer(pBufferQue, buf_idx, pBufferQue->pipeId);
				break;
			case ISP_MSG_TYPE_HAL_FLUSH:
				if (!pCamDevice->previewVideoSamePipe) {
				        memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
				        ispPipeSwitch.snapshotSwitch= 1;
				        error = flushBuffer(pBufferQue, &ispPipeSwitch);
				        if (error != CRANE_CAMERA_ERROR_NONE) {
				            CAMLOGE("flushBuffer for video falied, %d!", error);
				        }
				}
				//flush video buffer
				pCamDevice->flushFullSizeStreamBuffers();
				CAMLOGI("flushBuffer for fullsize stream buffer done!");

				UOS_FREE((void*)ispMsg);
				//set event to the main thread
				UOS_SetFlag(pCamDevice->threadSyncFlagRef, FULLSIZE_STOPWORK, OSA_FLAG_OR);

				return;
			default:
				break;
		}

		UOS_FREE((void*)ispMsg);
		ispMsg = NULL;

	}while (1);
}

void _CamCoreInit()
{
	camCoreHandle.CamDeviceMutex = UOS_NewMutex("cam_core");
}

/*
* return values:
* 0, need to access ddr, not allow to enter c1.
* 1, no data to access ddr, can enter c1(idle status).
*/
static int _CCCameraEnterC1Handler(void)
{
	int ret = 1;
	CCCameraDevice *pCamDevice = NULL;

	//CAMLOGV("enter c1 status!");

	pCamDevice = camCoreHandle.pCamDevice;
	if (pCamDevice) {
		if (pCamDevice->camStatus != 0) {
			//camera on the go.
			ret = 0;
		}
	}

	return ret;
}

static int _CCCameraExitC1Handler(void)
{	
	return 0;
}

CCError CCOpenCamera(int sensor_id)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = NULL;
	CCBufferQueue *pBufferQue = NULL;
	CCRotationBufferPack *pRotationBufPack = NULL;
	size_t rotationSize = 0;
	struct asrlcdd_screen_info lcdInfo;
	int ret;
	int cameraCnt = -1;
	void *pMem = NULL;

	CAMLOGI("open the %d camera start!",sensor_id);
	if (!camCoreInit) {
		_CamCoreInit();
		camCoreInit= 1;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (camCoreHandle.pCamDevice) {
		CAMLOGE("the camera has been opened\n!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_DEVICEWASOPENED;
	}

	error = getSensorCapability(&camCoreHandle, &cameraCnt, 2, camCoreHandle.ispSensorInfo);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("camera getSensorCapability failed for %d!", error);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	pMem = (void *)UOS_MALLOC(sizeof(CCCameraDevice));
	if (!pMem) {
		CAMLOGE("malloc for CameraDevice failed!");
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}

	pCamDevice = (CCCameraDevice*)pMem;
	memset(pCamDevice, 0, sizeof(CCCameraDevice));

	camCoreHandle.pCamDevice = pCamDevice;
	if (pCamDevice->camStatus != CAMERA_STATUS_IDLE) {
		CAMLOGE("camStatus(%d) is not zero!");
		pCamDevice->camStatus = CAMERA_STATUS_IDLE;
	}

	pCamDevice->minFps = 5;
	pCamDevice->maxFps = 30;
	pCamDevice->threadSyncFlagRef = INVALID_FLAG_ID;
	/* create event for thred sync. */
	pCamDevice->threadSyncFlagRef = UOS_CreateFlag();
	//	CAMLOGV("OSAFlagCreate,time=0x%x!", timerCountRead(TCR_2));

	/* create message queue for communication with drv.*/
	pCamDevice->previewMsgQRef = INVALID_MSGQ_ID;
	//needn't check reture value, because UOS_NewMessageQueue function may assert if error.
	pCamDevice->previewMsgQRef = UOS_NewMessageQueue("camPrev", TASK_MSGQ_SIZE_128);

	/* create preivew thread. */
	pCamDevice->previewTaskRef = UOS_CreateTask(
	   CCCamPreviewThread, 
	   (void *)pCamDevice, 
	   TASK_WITHOUT_MSGQ,
	   CAMERA_PREVIEW_TASK_SIZE, 
	   CAMERA_PREVIEW_TASK_PRIORITY,
	   "uiCamPreview");

	if (pCamDevice->previewTaskRef == NULL) {
	   CAMLOGE("UOS_CreateTask for camera preview failed!");
	   error = CRANE_CAMERA_ERROR_FATALERROR;
	   goto Error_exit;

	}

	pBufferQue = &(pCamDevice->previewBufQue);
	pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
	pBufferQue->bufQueSyncFlagRef = INVALID_FLAG_ID;
	pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_open");
	/* create event for bufferque sync. */
	pBufferQue->bufQueSyncFlagRef = UOS_CreateFlag();

	error = cameraDrvInit((SENSOR_ID)sensor_id);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("camera diver init failed for %d!", error);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}
	pCamDevice->sensorId = (SENSOR_ID)sensor_id;

	pCamDevice->currentZoomLevel = 0x100;

	/* get lcd information(width,height) */
	ret = ASRLCDD_GetScreenInfo(&lcdInfo);
	if (ret < 0) {
		CAMLOGE("get lcd information failed!");
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	} else {
		CAMLOGI("get lcd wxh[%d x %d]!",lcdInfo.width, lcdInfo.height);
	}

#if defined(CONFIG_DISPLAY_128x160)
	pCamDevice->lcdWidth = 128; //128
	pCamDevice->lcdHeight = 160;//160
#elif defined(CONFIG_DISPLAY_128x64)
	pCamDevice->lcdWidth = 128; //128
	pCamDevice->lcdHeight = 64;//64
#else
	pCamDevice->lcdWidth = lcdInfo.width; //240
	pCamDevice->lcdHeight = lcdInfo.height;
#endif

	//alloc buffer for display rotation
	pRotationBufPack = &pCamDevice->rotationBufferPack;
	rotationSize = lcdInfo.width * lcdInfo.height * 3 / 2 ; //CAM_FRAME_SIZE;//lcdInfo.width * lcdInfo.height * 3 / 2; //nv12
	pRotationBufPack->bufCnt = 1;
	pRotationBufPack->unitSize = rotationSize;
	pRotationBufPack->pAddr[0] = (char *)UOS_MALLOC(rotationSize * pRotationBufPack->bufCnt);

	if (NULL == pRotationBufPack->pAddr[0]) {
		CAMLOGE("alloc buffer for lcd rotation failed!");
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	pCamDevice->exifParam.exposureMode = 2 ; //auto mode
	pCamDevice->exifParam.meteringMode = 1 ; //average_mode

	//not allow to enter d2
	uiSetSuspendFlag(LP_ID_CAMERA, 0);

	uiC1CallbackRegister(LP_ID_CAMERA, _CCCameraEnterC1Handler, _CCCameraExitC1Handler);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGI("open the %d camera successful!", sensor_id);
	return error;

Error_exit:
	CAMLOGE("open the %d camera failed!",sensor_id);
	if (pCamDevice) {
		/* destory	preivew thread. */
		if (pCamDevice->previewTaskRef) {
			UOS_DeleteTask((TASK_HANDLE *)pCamDevice->previewTaskRef);
			pCamDevice->previewTaskRef = NULL;
		}

		/* destory thread sync mutex. */
		UOS_DeleteFlag(pCamDevice->threadSyncFlagRef);

		 /* destory msgqueue */
		UOS_FreeMessageQueue(pCamDevice->previewMsgQRef);
	}
	if (pBufferQue) {
		UOS_DeleteFlag(pBufferQue->bufQueSyncFlagRef);
		UOS_FreeMutex(pBufferQue->bufQueMutexRef);
	}
	if (pRotationBufPack)
		UOS_FREE(pCamDevice->rotationBufferPack.pAddr[0]);

	if (pCamDevice) {
		UOS_FREE((void*)pCamDevice);
		camCoreHandle.pCamDevice = NULL;
	}

//	memset(&camCoreHandle, 0, sizeof(CCCoreHandle));
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError _CCStartPreview(CCStartPreviewStruct *data, xbool isCfgSensor)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_sensor_cfg sensorCfg;
	struct isp_pipe_cfg pipeCfg;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	int_32 i;
	OSA_STATUS osaStatus = OS_SUCCESS;
	CCIspPipeSwitch ispPipeSwitch;
	CCRotationBufferPack *pRotationBufPack = NULL;
	CCBufferQueue *pBufferQue = NULL;
	uint_32 previewWidth = 0, previewHeight = 0, tempVar = 0;

	CAMLOGI("start preview!");

	if (!pCamDevice) {
		CAMLOGE("must open camera first!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (CAMERA_STATUS_PREVIEW & pCamDevice->camStatus) {
		CAMLOGW("the camera preview has been started!");
		return error;
	}

	if (!data) {
		CAMLOGE("invalid paramter!");
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	pCamDevice->frameDropNum = 0;
	sensorCfg.sensor_id = pCamDevice->sensorId;
	sensorCfg.skip_frame_n = 1;

//	previewWidth = data->image_width;
//	previewHeight = data->image_height;
	previewWidth = pCamDevice->lcdWidth;
	previewHeight = pCamDevice->lcdHeight;

	if (previewWidth * previewHeight > pCamDevice->lcdWidth * pCamDevice->lcdHeight) {
		CAMLOGE("can not support this preview size(%dx%d)!", previewWidth, previewHeight);
		error = CRANE_CAMERA_ERROR_BADARGUMENT;
		goto Error_exit;
	} else if (previewWidth * previewHeight < pCamDevice->lcdWidth * pCamDevice->lcdHeight) {
		//lcd need to scale up
		pCamDevice->previewScale = 1;
	} else
		pCamDevice->previewScale = 0;

	if (previewWidth < previewHeight) {
		tempVar = previewWidth;
		previewWidth = previewHeight;
		previewHeight = tempVar;
	}

	if (pCamDevice->lcdWidth >= pCamDevice->lcdHeight) {
		pCamDevice->previewRotationDegree = ROT_0_DEG;
	} else {
		pCamDevice->previewRotationDegree = ROT_90_DEG;
	}

	//request buffers
	pBufferQue = &(pCamDevice->previewBufQue);
	error = requestBuffers(pBufferQue, PREVIEW_BUFFER_NUM, PIPELINE_ID_PREV, pCamDevice->previewMsgQRef);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE(" requestBuffers for camera preview failed, error code = %d!", error);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	//alloc buffer for preview
	pBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pBufferQue->bufCnt = PREVIEW_BUFFER_NUM;
	error = allocBuffers(pBufferQue, previewWidth, previewHeight, PIPELINE_ID_PREV);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for preview failed, %d!", error);
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}

	pipeCfg.pipe_id = PIPELINE_ID_PREV;
	pipeCfg.pipe_enable = 1;
	pipeCfg.data_range = 0;
	pipeCfg.zoom = pCamDevice->currentZoomLevel;
	pipeCfg.output_format = PIPE_OUT_FMT_YUV420_NV12;
	pipeCfg.pipe_outw = previewWidth;
	pipeCfg.pipe_outh = previewHeight;

	//paramter will add
	if (isCfgSensor){
		 pCamDevice->currentZoomLevel = (data->factor * (0x200 - 0x100) / 15) + 0x100;
		 pipeCfg.zoom = pCamDevice->currentZoomLevel;
		error = configCamera(&sensorCfg, &pipeCfg);
		pCamDevice->alreadyCfgSensor = true;
	} else
		error = configCamera(NULL, &pipeCfg);

	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("configCamera for preivew falied, %d!", error);
		goto Error_exit;
	}

	//enqueue all preview buffer
	pCamDevice->previewBufQue.queueStatus = 1;
	for(i = 0; i < pCamDevice->previewBufQue.bufCnt; i++) {
		error = enqueueBuffer(&pCamDevice->previewBufQue, i, PIPELINE_ID_PREV);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for preview falied, %d!", i, error);
			goto Error_exit;
		}
	}

	//notify the thread ready to work
	if (isCfgSensor)	//if false means capture stop preview which is not need to set PREVIEW_READYWORK
		osaStatus = UOS_SetFlag(pCamDevice->threadSyncFlagRef, PREVIEW_READYWORK, OSA_FLAG_OR);

	if (osaStatus != OS_SUCCESS) {
		CAMLOGE("OSAFlagSet workflag to camera preview thread falied, errcode %d!", osaStatus);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	if (isCfgSensor) {
		pRotationBufPack = &pCamDevice->rotationBufferPack;
		//set rotation buffer to lcd and call back.
		ASRLCDD_SetBuffers(pRotationBufPack->pAddr[0], pRotationBufPack->unitSize * pRotationBufPack->bufCnt, CCFreeBufferNotify, (void*)(&pCamDevice->previewBufQue));
	}

	//stream on
	memset((void *)(&ispPipeSwitch), 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.previewSwitch= 1;
	error = streamOnPipe(&ispPipeSwitch);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOnPipe for preivew falied, %d!", error);
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->previewTaskRef);
		ASRLCDD_CameraMerge(NULL, ROT_90_DEG, NULL);
		goto Error_exit;
	} else {
		pCamDevice->camStatus |= CAMERA_STATUS_PREVIEW;
		if(pCamDevice->uiParam.nightMode)
			setCameraFPS(pCamDevice->uiParam.nightModeFps, pCamDevice->maxFps, BANDING_50HZ);
		else
			setCameraFPS(pCamDevice->minFps, pCamDevice->maxFps, BANDING_50HZ);
	}

	memcpy(&pCamDevice->previewStartInfo, data, sizeof(CCStartPreviewStruct));
	CAMLOGI("start preview successful!");
	return error;

Error_exit:
	CAMLOGE("start preview failed !");

	// free preview buffer
	if (pBufferQue->totalSize > 0){
		freeBuffers(pBufferQue);
		//memset(pBufferQue, 0, sizeof(CCBufferQueue));
	}

	return error;
}

CCError CCStartPreview(CCStartPreviewStruct *data) {
	CCError error = CRANE_CAMERA_ERROR_NONE;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	error = _CCStartPreview(data, true);
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);

	return error;
}

CCError _CCStopPreview(CCPreivewStopType stopType)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	struct isp_buf_msg *ispMsg = NULL;
	uint_32   actual_flags = 0;
	OSA_STATUS osaStatus = 0;
	CCIspPipeSwitch ispPipeSwitch;
	xbool sendFlushMsg = false, keepSensor = false;
	CCBufferQueue *pBufferQue = NULL;

	CAMLOGI("stop preview!");
	if (!pCamDevice) {
		CAMLOGE("the camera has not been opend!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (!(pCamDevice->camStatus & CAMERA_STATUS_PREVIEW)) {
		CAMLOGE("must start preview first!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (CC_PREVIEW_STOP_EXIT == stopType)
		keepSensor = false;
	else
		keepSensor = true;

	//1.streamoff.
	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.previewSwitch = 1;
	error = streamOffPipe(&ispPipeSwitch, keepSensor);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOffPipe for preivew falied, %d!", error);
	}

	//2.send flush buffers msg.
	pCamDevice->previewBufQue.queueStatus = 0;
	ispMsg = (struct isp_buf_msg*)UOS_MALLOC(sizeof(struct isp_buf_msg));
	if (!ispMsg) {
		CAMLOGE("Uos malloc for isp_buf_msg failed!");
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->previewTaskRef);
		goto err_out;
	}

	memset(ispMsg, 0, sizeof(struct isp_buf_msg));
	if (CC_PREVIEW_STOP_EXIT == stopType)
		ispMsg->msg_type = ISP_MSG_TYPE_HAL_EXIT;
	else
		ispMsg->msg_type = ISP_MSG_TYPE_HAL_FLUSH;

	//must send success because UOS_SendMsg could assert if failed.
	UOS_SendMsg((void *)(ispMsg), pCamDevice->previewMsgQRef, UOS_SEND_MSG);
	if (osaStatus != OS_SUCCESS) {
		CAMLOGE("OSAMsgQSend flush message to preview thread falied, errcode %d!", osaStatus);
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->previewTaskRef);
	} else
		sendFlushMsg = true;

	//3.wait thread flush buffer done.
	while (sendFlushMsg) {
		osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, PREVIEW_STOPWORK, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS && (actual_flags & PREVIEW_STOPWORK))
			break;
	}

err_out:
	pBufferQue = &(pCamDevice->previewBufQue);
	// free preview buffer
	if (pBufferQue) {
		if (pBufferQue->totalSize > 0) {
			freeBuffers(pBufferQue);
			//memset(pBufferQue, 0, sizeof(CCBufferQueue));
		}
	}

	pCamDevice->previewScale = 0;
	pCamDevice->camStatus &= (~CAMERA_STATUS_PREVIEW);
	CAMLOGI("stop preview successfull!");

	return error;
}

CCError CCStopPreview(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	error = _CCStopPreview(CC_PREVIEW_STOP_EXIT);
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);

	return error;
}

CCError CCPausePreview(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_PREVIEW)) {
		CAMLOGE("must start preview first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	pCamDevice->previewStreamPause = 1;
	//ASRLCDD_CameraPause();

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError CCResumePreview(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_PREVIEW)) {
		CAMLOGE("must start preview first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	pCamDevice->previewStreamPause = 0;
	//ASRLCDD_CameraContinue();

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;

}

CCError CCSetAlphaPreview(unsigned int value)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
		
	ASRLCDD_alpha_Blending(value);
	CAMLOGE("Alpha=%d", value);

	return error;
}


CCError CCCloseCamera(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCBufferQueue *pBufferQue = NULL;
	CCRotationBufferPack *pRotationBufPack = NULL;
	u8 flagRef = 0;
	int sensor_id = 0;

	CAMLOGI("close camera start");

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!pCamDevice) {
		CAMLOGE("the camera need open first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	sensor_id = pCamDevice->sensorId;

	//video record could close camera without stop preview.
	if (pCamDevice->camStatus & CAMERA_STATUS_PREVIEW) {
		CAMLOGW("Should stop preview first, we help you to stop here!");
		error = _CCStopPreview(CC_PREVIEW_STOP_EXIT);
	}

	cameraDrvDeinit(sensor_id);

	pRotationBufPack = &pCamDevice->rotationBufferPack;
	if (pRotationBufPack)
		UOS_FREE(pCamDevice->rotationBufferPack.pAddr[0]);
	//memset(pCamDevice->rotationBufferPack.pAddr[0], 0, pRotationBufPack->bufCnt * pRotationBufPack->unitSize);

	pBufferQue = &(pCamDevice->previewBufQue);
	if (pBufferQue) {
		flagRef = pBufferQue->bufQueSyncFlagRef;
		pBufferQue->bufQueSyncFlagRef = INVALID_FLAG_ID;
		UOS_DeleteFlag(flagRef);
		UOS_FreeMutex(pBufferQue->bufQueMutexRef);
	}

		/* destory	preivew thread. */
	if (pCamDevice->previewTaskRef) {
		UOS_StopTask((TASK_HANDLE *)pCamDevice->previewTaskRef);
		UOS_DeleteTask((TASK_HANDLE *)pCamDevice->previewTaskRef);
		pCamDevice->previewTaskRef = NULL;
	}

	/* destory thread sync mutex. */
	UOS_DeleteFlag(pCamDevice->threadSyncFlagRef);

	 /* destory msgqueue */
	if (INVALID_MSGQ_ID != pCamDevice->previewMsgQRef) {
		UOS_FreeMessageQueue(pCamDevice->previewMsgQRef);
		pCamDevice->previewMsgQRef = INVALID_MSGQ_ID;
	}

	if (pCamDevice) {
		UOS_FREE((void*)pCamDevice);
	    camCoreHandle.pCamDevice = NULL;
	}

 	//memset(&camCoreHandle, 0, sizeof(CCCoreHandle));

	//allow to enter d2
	uiSetSuspendFlag(LP_ID_CAMERA, 1);
	uiC1CallbackunRegister(LP_ID_CAMERA);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGI("close the %d camera successful!", sensor_id);
	return error;
}

CCError CCStartRecord(CamRecordParmeters *pVideoRecordParam, uint_32 format)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_pipe_cfg pipeCfg;
	CCBufferQueue *pBufferQue = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	OSA_STATUS osaStatus = 0;
	int_32 i;
	CCIspPipeSwitch ispPipeSwitch;
	xbool sendWorkFlag = false;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	if (!pVideoRecordParam || !pVideoRecordParam->enqueueCamRecordBuffer
		|| !pVideoRecordParam->flushCamRecordBuffers) {
		CAMLOGE("the videoRecordParam is invalid handle=0x%p!", pVideoRecordParam);
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
        	return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	if (!pCamDevice || !(pCamDevice->camStatus & CAMERA_STATUS_PREVIEW)) {
		CAMLOGE("recording must open camera and start preview first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (pCamDevice->camStatus & CAMERA_STATUS_VIDEO) {
		CAMLOGE("the video recording is in progress!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}
	pCamDevice->minFps = 5;
	pCamDevice->maxFps = 30;

	if(pCamDevice->lcdWidth * pCamDevice->lcdHeight == pVideoRecordParam->recordWidth * pVideoRecordParam->recordHeight){
		pCamDevice->previewVideoSamePipe = TRUE;
	} else
		pCamDevice->previewVideoSamePipe = FALSE;

	//1.prepare for video thread and msgqueue with drv.
	pCamDevice->videoMsgQRef = INVALID_MSGQ_ID;
	pCamDevice->videoMsgQRef = UOS_NewMessageQueue("camVide", TASK_DEFAULT_MSGQ_SIZE);

	pCamDevice->videoTaskRef = UOS_CreateTask(
		CCCamVideoThread,
		(void *)pCamDevice,
		TASK_WITHOUT_MSGQ,
		CAMERA_VIDEO_TASK_SIZE,
		CAMERA_VIDEO_TASK_PRIORITY,
		"uiCamVideo");
	if (pCamDevice->videoTaskRef == NULL) {
		CAMLOGE("UOS_CreateTask for camera video failed, error code = %d!", osaStatus);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	if(!pCamDevice->previewVideoSamePipe) {
		//2.config isp : enable video port.
		pipeCfg.pipe_id = PIPELINE_ID_VIDEO;
		pipeCfg.pipe_enable = 1;
		pipeCfg.data_range = 0;
		pipeCfg.zoom = pCamDevice->currentZoomLevel;
		pipeCfg.output_format = (PIPE_OUT_FMT)format;
		pipeCfg.pipe_outw = pVideoRecordParam->recordWidth;
		pipeCfg.pipe_outh = pVideoRecordParam->recordHeight;

		error = configCamera(/* paramter, */NULL, &pipeCfg);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("configCamera for camera video failed!");
			goto Error_exit;
		}

		//3.request buffers.
		pBufferQue = &(pCamDevice->videoBufQue);
		error = requestBuffers(pBufferQue, PREVIEW_BUFFER_NUM, PIPELINE_ID_VIDEO, pCamDevice->videoMsgQRef);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("requestBuffers for camera video failed!");
			goto Error_exit;
		}

		//alloc buffer for video
		pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
		pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_start_reocrd");

		pBufferQue->bufFormat = (PIPE_OUT_FMT)format;//PIPE_OUT_FMT_YUV420_NV12;
		pBufferQue->bufCnt = PREVIEW_BUFFER_NUM;
		error = allocBuffers(pBufferQue, pVideoRecordParam->recordWidth, pVideoRecordParam->recordHeight, PIPELINE_ID_VIDEO);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("allocBuffers for video failed, %d!", error);
			error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
			goto Error_exit;
		}

		//4.enqueue all video buffer
		for(i = 0; i < pBufferQue->bufCnt; i++) {
			error = enqueueBuffer(pBufferQue, i, PIPELINE_ID_VIDEO);
			if (error != CRANE_CAMERA_ERROR_NONE) {
				CAMLOGE("enqueue the %d Buffer for video falied, %d!", i, error);
				goto Error_exit;
			}
		}
	}

	//set paramters for video encoder
	if(!pCamDevice->previewVideoSamePipe)
		pVideoRecordParam->setReleaseBufferCallBack(CCReleaseBufferToCamera, (void*)(&pCamDevice->videoBufQue));
	else
		pVideoRecordParam->setReleaseBufferCallBack(CCReleaseBufferToCamera, (void*)(&pCamDevice->previewBufQue));
	pCamDevice->qBufForVideoEncoder = pVideoRecordParam->enqueueCamRecordBuffer;
	pCamDevice->flushVideoEncoderBuffers = pVideoRecordParam->flushCamRecordBuffers;

	//5.notify the thread ready to work
	osaStatus = UOS_SetFlag(pCamDevice->threadSyncFlagRef, VIDEO_READYWORK, OSA_FLAG_OR);
	if (osaStatus != OS_SUCCESS) {
		CAMLOGE("OSAFlagSet workflag for camera video thread, error code = %d!", osaStatus);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	} else
		sendWorkFlag = true;

	if(!pCamDevice->previewVideoSamePipe) {
		//6.stream on
		memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
		ispPipeSwitch.videoSwitch= 1;
		error = streamOnPipe(&ispPipeSwitch);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("streamOnPipe for video falied, %d!", error);
			goto Error_exit;
		} else {
			pCamDevice->camStatus |= CAMERA_STATUS_VIDEO;
			pCamDevice->videoOperation = 1; //start
		}
	} else {
		pCamDevice->camStatus |= CAMERA_STATUS_VIDEO;
		pCamDevice->videoOperation = 1; //start
	}
	if(pCamDevice->uiParam.nightMode)
		setCameraFPS(pCamDevice->uiParam.nightModeFps, pCamDevice->maxFps, BANDING_50HZ);
	else
		setCameraFPS(pCamDevice->minFps, pCamDevice->maxFps, BANDING_50HZ);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGD("start video recorder successful!");
	return error;

Error_exit:
	if (sendWorkFlag) {
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->videoTaskRef);
	}

	if (pCamDevice->videoTaskRef) {
		UOS_DeleteTask((TASK_HANDLE *)pCamDevice->videoTaskRef);
		pCamDevice->videoTaskRef = NULL;
	}

	if (pBufferQue && (!pCamDevice->previewVideoSamePipe)) {
		if (pBufferQue->totalSize > 0) {
			freeBuffers(pBufferQue);
		}

		if (INVALID_MUTEX_ID != pBufferQue->bufQueMutexRef) {
			UOS_FreeMutex(pBufferQue->bufQueMutexRef);
			pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
		}
	}

	if (INVALID_MSGQ_ID != pCamDevice->videoMsgQRef) {
		UOS_FreeMessageQueue(pCamDevice->videoMsgQRef);
		pCamDevice->videoMsgQRef = INVALID_MSGQ_ID;
	}

	pCamDevice->previewVideoSamePipe = FALSE;
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError CCPauseRecord(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_VIDEO)) {
		CAMLOGE("must start record first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	pCamDevice->videoStreamPause = 1;

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError CCResumeRecord(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCBufferQueue *pBufferQue = NULL;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_VIDEO)) {
		CAMLOGE("must start record first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (!pCamDevice->previewVideoSamePipe) {
		pBufferQue = &(pCamDevice->videoBufQue);
		if (pCamDevice->videoStreamPause) {
			_CCTryEnqueueIdleBuffers(pBufferQue);
			pCamDevice->videoStreamPause = 0;
		}
	} else {
		if (pCamDevice->videoStreamPause) {
			pCamDevice->videoStreamPause = 0;
		}
	}

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;

}

CCError CCStopRecord(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_buf_msg *ispMsg = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	uint_32   actual_flags = 0;
	CCIspPipeSwitch ispPipeSwitch;
	OSA_STATUS osaStatus = OS_SUCCESS;
	xbool sendFlushMsg = false;
	u8 tempRef = 0;

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	if (!pCamDevice) {
		CAMLOGE("the camera has not been opend!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (!(pCamDevice->camStatus & CAMERA_STATUS_VIDEO)) {
		CAMLOGE("must start record first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	pCamDevice->videoOperation = 2; //stop

	if (!pCamDevice->previewVideoSamePipe) {
		//1.stream off
		memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
		ispPipeSwitch.videoSwitch = 1;
		error = streamOffPipe(&ispPipeSwitch, false);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("streamOffPipe for preivew falied, %d!", error);
		}
	}

	//2.send flush buffers msg.
	ispMsg = (struct isp_buf_msg*)UOS_MALLOC(sizeof(struct isp_buf_msg));
	if (!ispMsg) {
		CAMLOGE("Uos malloc for isp_buf_msg failed!");
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->videoTaskRef);
		goto err_out;
	}
	memset(ispMsg, 0, sizeof(struct isp_buf_msg));
	ispMsg->msg_type = ISP_MSG_TYPE_HAL_FLUSH;
	UOS_SendMsg((void *)(ispMsg), pCamDevice->videoMsgQRef, UOS_SEND_MSG);
	if (osaStatus != OS_SUCCESS) {
		CAMLOGE("OSAMsgQSend flush message to video thread falied, errcode %d!", osaStatus);
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->videoTaskRef);
	} else
		sendFlushMsg = true;

	//3.wait thread flush buffer done.
	while (sendFlushMsg) {
		osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, VIDEO_STOPWORK, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS && (actual_flags & VIDEO_STOPWORK))
			break;
	}

err_out:
	//4. destory msgqueue and thred
	UOS_DeleteTask((TASK_HANDLE *)pCamDevice->videoTaskRef);

	//5.free all buffer
	if (!pCamDevice->previewVideoSamePipe) {
		freeBuffers(&pCamDevice->videoBufQue);
		UOS_FreeMutex(pCamDevice->videoBufQue.bufQueMutexRef);
		memset(&pCamDevice->videoBufQue, 0, sizeof(CCBufferQueue));
	} else
		pCamDevice->previewVideoSamePipe = FALSE;

	pCamDevice->camStatus &= (~CAMERA_STATUS_VIDEO);
	tempRef = pCamDevice->videoMsgQRef;
	pCamDevice->videoMsgQRef = INVALID_MSGQ_ID;
	UOS_FreeMessageQueue(tempRef);
	pCamDevice->qBufForVideoEncoder = NULL;
	pCamDevice->flushVideoEncoderBuffers = NULL;

	CAMLOGD("stop video recorder successfull!");
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

#ifdef CAM_FULLSIZE_STREAM
CCError CCStartFullSizeStream(CamRecordParmeters *pVideoRecordParam, uint_32 format)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_pipe_cfg pipeCfg;
	CCBufferQueue *pBufferQue = NULL;
	CCCameraDevice *pCamDevice = NULL;
	OSA_STATUS osaStatus = 0;
	int_32 i;
	CCIspPipeSwitch ispPipeSwitch;
	xbool sendWorkFlag = false;
	struct isp_output_size ispResInfo = {0};
	struct isp_sensor_cfg sensorCfg;
	xbool needCfgSensor = true;

	if (!camCoreHandle.pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL, camera has not been opend!");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	} else {
		pCamDevice = camCoreHandle.pCamDevice;
		sensorCfg.sensor_id = pCamDevice->sensorId;
		sensorCfg.skip_frame_n = 1;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	if (!pVideoRecordParam || !pVideoRecordParam->enqueueCamRecordBuffer
		|| !pVideoRecordParam->flushCamRecordBuffers) {
		CAMLOGE("the videoRecordParam is invalid handle=0x%p!", pVideoRecordParam);
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
        	return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	if (!pCamDevice) {
		CAMLOGE("FullSizeStream must open camera");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (pCamDevice->camStatus & CAMERA_STATUS_FULLSIZE_STREAM) {
		CAMLOGE("FullSizeStream is in progress!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}
	pCamDevice->minFps = 5;
	pCamDevice->maxFps = 30;

	if(!pCamDevice->alreadyCfgSensor){
		error = configCamera(&sensorCfg, NULL);
		pCamDevice->alreadyCfgSensor = true;
	}
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("configCamera for camera fullsize failed!");
		goto Error_exit;
	}
		// get sensor size(isp p2 output size)
	error = getIspRes(&ispResInfo);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("getIspRes failed, %d!", error);
		goto Error_exit;
	}
	CAMLOGE("getIspRes wxh [%dx%d]", ispResInfo.width,ispResInfo.height);

	pCamDevice->sensorWidth = ispResInfo.width;
	pCamDevice->sensorHeight = ispResInfo.height;

	//1.prepare for fullsize stream thread and msgqueue with drv.
	pCamDevice->fullSizeStreamMsgQRef = INVALID_MSGQ_ID;
	pCamDevice->fullSizeStreamMsgQRef = UOS_NewMessageQueue("camFull", TASK_DEFAULT_MSGQ_SIZE);

	pCamDevice->fullSizeStreamTaskRef = UOS_CreateTask(
		CCCamFullSizeStreamThread,
		(void *)pCamDevice,
		TASK_WITHOUT_MSGQ,
		CAMERA_VIDEO_TASK_SIZE,
		CAMERA_VIDEO_TASK_PRIORITY,
		"uiCamFull");
	if (pCamDevice->fullSizeStreamTaskRef == NULL) {
		CAMLOGE("UOS_CreateTask for camera fullsize failed, error code = %d!", osaStatus);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	//2.config isp : enable capture port.
	pipeCfg.pipe_id = PIPELINE_ID_CAP;
	pipeCfg.pipe_enable = 1;
	pipeCfg.data_range = 0;
	pipeCfg.zoom = pCamDevice->currentZoomLevel;
	pipeCfg.output_format = PIPE_OUT_FMT_YUV420_NV12;
	pipeCfg.pipe_outw = pCamDevice->sensorWidth;
	pipeCfg.pipe_outh = pCamDevice->sensorHeight;

	error = configCamera(/* paramter, */NULL, &pipeCfg);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("configpipe for  fullsize failed!");
		goto Error_exit;
	}

	//3.request buffers.
	pBufferQue = &(pCamDevice->fullSizeStreamBufQue);
	error = requestBuffers(pBufferQue, PREVIEW_BUFFER_NUM, PIPELINE_ID_CAP, pCamDevice->fullSizeStreamMsgQRef);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("requestBuffers for camera fullsize failed!");
		goto Error_exit;
	}

	//alloc buffer for fullsize
	pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
	pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_start_fullsize");

	pBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pBufferQue->bufCnt = PREVIEW_BUFFER_NUM;
	error = allocBuffers(pBufferQue, pCamDevice->sensorWidth, pCamDevice->sensorHeight, PIPELINE_ID_CAP);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for fullsize failed, %d!", error);
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}

	//4.enqueue all fullsize buffer
	for(i = 0; i < pBufferQue->bufCnt; i++) {
		error = enqueueBuffer(pBufferQue, i, PIPELINE_ID_CAP);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for fullsize falied, %d!", i, error);
			goto Error_exit;
		}
	}

	//set paramters for video encoder
	pVideoRecordParam->setReleaseBufferCallBack(CCReleaseFullSizeBufferToCamera, (void*)(&pCamDevice->fullSizeStreamBufQue));

	pCamDevice->qBufForFullSizeStream = pVideoRecordParam->enqueueCamRecordBuffer;
	pCamDevice->flushFullSizeStreamBuffers = pVideoRecordParam->flushCamRecordBuffers;

	//5.notify the thread ready to work
	osaStatus = UOS_SetFlag(pCamDevice->threadSyncFlagRef, FULLSIZE_READYWORK, OSA_FLAG_OR);
	if (osaStatus != OS_SUCCESS) {
		CAMLOGE("OSAFlagSet workflag for camera fullsize thread, error code = %d!", osaStatus);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	} else
		sendWorkFlag = true;

	//6.stream on
	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.snapshotSwitch= 1;
	error = streamOnPipe(&ispPipeSwitch);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOnPipe for fullsize falied, %d!", error);
		goto Error_exit;
	} else {
		pCamDevice->camStatus |= CAMERA_STATUS_FULLSIZE_STREAM;
	}

	if(pCamDevice->uiParam.nightMode)
		setCameraFPS(pCamDevice->uiParam.nightModeFps, pCamDevice->maxFps, BANDING_50HZ);
	else
		setCameraFPS(pCamDevice->minFps, pCamDevice->maxFps, BANDING_50HZ);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGD("start fullsize stream successful!");
	return error;

Error_exit:
	if (sendWorkFlag) {
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->fullSizeStreamTaskRef);
	}

	if (pCamDevice->fullSizeStreamTaskRef) {
		UOS_DeleteTask((TASK_HANDLE *)pCamDevice->fullSizeStreamTaskRef);
		pCamDevice->fullSizeStreamTaskRef = NULL;
	}

	if (pBufferQue) {
		if (pBufferQue->totalSize > 0) {
			freeBuffers(pBufferQue);
		}

		if (INVALID_MUTEX_ID != pBufferQue->bufQueMutexRef) {
			UOS_FreeMutex(pBufferQue->bufQueMutexRef);
			pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
		}
	}

	if (INVALID_MSGQ_ID != pCamDevice->fullSizeStreamMsgQRef) {
		UOS_FreeMessageQueue(pCamDevice->fullSizeStreamMsgQRef);
		pCamDevice->fullSizeStreamMsgQRef = INVALID_MSGQ_ID;
	}

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError CCPauseFullSizeStream(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = NULL;

	if (!camCoreHandle.pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL, camera has not been opend!");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	} else {
		pCamDevice = camCoreHandle.pCamDevice;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_FULLSIZE_STREAM)) {
		CAMLOGE("must start fullsize stream first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	pCamDevice->fullSizeStreamPause = 1;

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError CCResumeFullSizeStream(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = NULL;

	if (!camCoreHandle.pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL, camera has not been opend!");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	} else {
		pCamDevice = camCoreHandle.pCamDevice;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_FULLSIZE_STREAM)) {
		CAMLOGE("must start fullsize stream first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (pCamDevice->fullSizeStreamPause) {
		pCamDevice->fullSizeStreamPause = 0;
	}

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;

}

CCError CCStopFullSizeStream(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_buf_msg *ispMsg = NULL;
	CCCameraDevice *pCamDevice = NULL;
	uint_32   actual_flags = 0;
	CCIspPipeSwitch ispPipeSwitch;
	OSA_STATUS osaStatus = OS_SUCCESS;
	xbool sendFlushMsg = false;
	u8 tempRef = 0;

	if (!camCoreHandle.pCamDevice) {
		CAMLOGE("the argument pCamDevice is NULL, camera has not been opend!");
		return CRANE_CAMERA_ERROR_BADPOINTER;
	} else {
		pCamDevice = camCoreHandle.pCamDevice;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!(pCamDevice->camStatus & CAMERA_STATUS_FULLSIZE_STREAM)) {
		CAMLOGE("must start fullsize stream first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	//1.stream off
	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.snapshotSwitch= 1;
	error = streamOffPipe(&ispPipeSwitch, false);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOffPipe for fullsize stream falied, %d!", error);
	}

	//2.send flush buffers msg.
	ispMsg = (struct isp_buf_msg*)UOS_MALLOC(sizeof(struct isp_buf_msg));
	if (!ispMsg) {
		CAMLOGE("Uos malloc for isp_buf_msg failed!");
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->fullSizeStreamTaskRef);
		goto err_out;
	}
	memset(ispMsg, 0, sizeof(struct isp_buf_msg));
	ispMsg->msg_type = ISP_MSG_TYPE_HAL_FLUSH;
	UOS_SendMsg((void *)(ispMsg), pCamDevice->fullSizeStreamMsgQRef, UOS_SEND_MSG);
	if (osaStatus != OS_SUCCESS) {
		CAMLOGE("OSAMsgQSend flush message to fullsize stream thread falied, errcode %d!", osaStatus);
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->fullSizeStreamTaskRef);
	} else
		sendFlushMsg = true;

	//3.wait thread flush buffer done.
	while (sendFlushMsg) {
		osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, FULLSIZE_STOPWORK, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS && (actual_flags & FULLSIZE_STOPWORK))
			break;
	}

err_out:
	//4. destory msgqueue and thred
	UOS_DeleteTask((TASK_HANDLE *)pCamDevice->fullSizeStreamTaskRef);

	//5.free all buffer
	freeBuffers(&pCamDevice->fullSizeStreamBufQue);
	UOS_FreeMutex(pCamDevice->fullSizeStreamBufQue.bufQueMutexRef);
	memset(&pCamDevice->fullSizeStreamBufQue, 0, sizeof(CCBufferQueue));


	pCamDevice->camStatus &= (~CAMERA_STATUS_FULLSIZE_STREAM);
	tempRef = pCamDevice->fullSizeStreamMsgQRef;
	pCamDevice->fullSizeStreamMsgQRef = INVALID_MSGQ_ID;
	UOS_FreeMessageQueue(tempRef);
	pCamDevice->qBufForFullSizeStream= NULL;
	pCamDevice->flushFullSizeStreamBuffers= NULL;

	CAMLOGD("stop fullsize stream successfull!");
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

#endif
CCError ataOpenCamera(int sensor_id)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = NULL;
	int cameraCnt = -1;
	void *pMem = NULL;
	struct isp_sensor_cfg sensorCfg;

	CAMLOGI("ataOpenCamera start!");

	if (!camCoreInit) {
		_CamCoreInit();
		cam_hw_version_init();
		camCoreInit= 1;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (camCoreHandle.pCamDevice) {
		CAMLOGE("the camera has been opened\n!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_DEVICEWASOPENED;
	}
	getSensorCapability(&camCoreHandle, &cameraCnt, 2, camCoreHandle.ispSensorInfo);

	pMem = (void *)UOS_MALLOC(sizeof(CCCameraDevice));
	if (!pMem) {
		CAMLOGE("malloc for CameraDevice failed!");
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}

	pCamDevice = (CCCameraDevice*)pMem;
	memset(pCamDevice, 0, sizeof(CCCameraDevice));
	camCoreHandle.pCamDevice = pCamDevice;

	error = cameraDrvInit((SENSOR_ID)sensor_id);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("camera diver init failed for %d!", error);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}
	pCamDevice->sensorId = (SENSOR_ID)sensor_id;
	pCamDevice->currentZoomLevel = 0x100;
	pCamDevice->minFps = 5;
	pCamDevice->maxFps = 30;
	pCamDevice->previewRotationDegree = ROT_90_DEG;

	sensorCfg.sensor_id = pCamDevice->sensorId;
	sensorCfg.skip_frame_n = 0;
	error = configCamera(&sensorCfg, NULL);
	pCamDevice->alreadyCfgSensor = true;
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("configCamera failed, %d!", error);
		goto Error_exit;
	}
	setCameraFPS(pCamDevice->minFps, pCamDevice->maxFps, BANDING_50HZ);
	//not allow to enter d2
	uiSetSuspendFlag(LP_ID_CAMERA, 0);

	uiC1CallbackRegister(LP_ID_CAMERA, _CCCameraEnterC1Handler, _CCCameraExitC1Handler);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGD("ataopen the %d camera successful!", sensor_id);
	return error;

Error_exit:
	if (pCamDevice) {
		UOS_FREE((void*)pCamDevice);
		camCoreHandle.pCamDevice = NULL;
	}

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError ataCloseCamera(int sensor_id)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;

	CAMLOGI("ataCloseCamera");
	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	if (!pCamDevice) {
		CAMLOGE("the camera need open first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	cameraDrvDeinit(sensor_id);

	if (pCamDevice) {
		UOS_FREE((void*)pCamDevice);
	    camCoreHandle.pCamDevice = NULL;
	}
	//allow to enter d2
	uiSetSuspendFlag(LP_ID_CAMERA, 1);
	uiC1CallbackunRegister(LP_ID_CAMERA);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGI("ataclose the %d camera successful!", sensor_id);

	return error;
}

CCError ataTakePicture(int_32 file_handle, CCCaputreStruct *capture_data)
{
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCBufferQueue *pBufferQue = NULL;
	uint_32 halTimeTick1 = 0, halTimeTick2 = 0;
	struct isp_output_size ispResInfo;
	int_32 i = 0, bufIndex = -1;
	CCIspPipeSwitch ispPipeSwitch;
	uint_32 fileWriteLen = 0, jepgBufLen = 0;
	CCOfflineOption offlineOption;
	uint_8 *pJpegBuffer = NULL;

	CAMLOGI("ataTakePicture start");
	halTimeTick1 = timerCountRead(TCR_2);
	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	if (!pCamDevice ) {
		CAMLOGE("must open camera first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	// get sensor size(isp p2 output size)
	error = getIspRes(&ispResInfo);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("getIspRes failed, %d!", error);
		goto Error_exit;
	}

	pCamDevice->sensorWidth = ispResInfo.width;
	pCamDevice->sensorHeight = ispResInfo.height;

	CAMLOGD("ataTakePicture size wxh=[%d x %d]",capture_data->image_width,capture_data->image_height);

	pCamDevice->imageQuality = HAL_QTY_LOW;
	error = _CCPreCapture(capture_data);
	if (error != CRANE_CAMERA_ERROR_NONE)
		goto Error_exit;

	pBufferQue = &(pCamDevice->captureBufQue);
	error = requestBuffers(pBufferQue, SNAPSHOT_BUFFER_NUM, PIPELINE_ID_CAP, pCamDevice->captureMsgQRef);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("requestBuffers the %d Buffer for capture falied, %d!", error);
		goto Error_exit;
	}

	for(i = 0; i < SNAPSHOT_BUFFER_NUM; i++) {
		error = enqueueBuffer(pBufferQue, i, PIPELINE_ID_CAP);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for capture falied, %d!", i, error);
			goto Error_exit;
		}
	}

	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.snapshotSwitch= 1;
	error = streamOnPipe(&ispPipeSwitch);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOnPipe for capture falied, %d!", error);
		goto Error_exit;
	}

	error = _CCWaitCapturePortDone(pCamDevice->captureMsgQRef, pBufferQue, &bufIndex);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("waitCaptureDone falied, %d!", error);
		streamOffPipe(&ispPipeSwitch, false);
		goto Error_exit;;
	}

	error = streamOffPipe(&ispPipeSwitch, false);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOffPipe for capture falied, %d!", error);
	}

	memset(&offlineOption, 0, sizeof(offlineOption));
	offlineOption.thumbnail = false;
	offlineOption.thumb_format = PIPE_OUT_FMT_YUV420_NV12;
	offlineOption.rotationDegree = (enum offline_rot)pCamDevice->previewRotationDegree;
	if (pCamDevice->currentZoomLevel > 0x100) {
		offlineOption.zoomScale = true;
		offlineOption.zoomLevel = pCamDevice->currentZoomLevel;
	} else {
		offlineOption.zoomScale = false;
		offlineOption.zoomLevel = 0x100;
	}

	error = _CCCaptureBottomHalfProcess(&offlineOption, pBufferQue, bufIndex, NULL, false);

	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("capture bottom half process failed!");
		goto Error_exit;
	}

	jepgBufLen = pCamDevice->offlineBufQue.multiplexBufferSize;
	if(jepgBufLen == 0){
		CAMLOGE("jepgBufLen = %d ",jepgBufLen);
		error = CRANE_CAMERA_ERROR_INVALIDOPERATION;
		goto Error_exit;
	}
	pJpegBuffer = (uint_8 *)pBufferQue->jpeg_addr;

	fileWriteLen = FS_Write(file_handle, pJpegBuffer, jepgBufLen );
	if (fileWriteLen == 0) {
		CAMLOGE("write jpeg failed!");
		error = CRANE_CAMERA_ERROR_DISKFULL;
		goto Error_exit;
	}

	_CCPostCapture();

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	halTimeTick2 = timerCountRead(TCR_2);
	CAMLOGI("ataTakePictue cost %dms !", (halTimeTick2 - halTimeTick1) / 32);

	return error;
Error_exit:
	CAMLOGE("ataTakePictue failed!");
	_CCPostCapture();
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);

	return error;

}

CCError CCATATakePicture(int sensor_id, uint_32 ata_on, int_32 file_handle, CCCaputreStruct *capture_data)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

	cam_offline_init();

	error = ataOpenCamera(sensor_id);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("ataOpenCamera failed!");
		goto ERR_EXIT;
	}
	error = setCameraATA(ata_on);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("setCameraATA failed!");
		goto ERR_EXIT;
	}
	error = ataTakePicture(file_handle, capture_data);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("ataTakePicture failed!");
		goto ERR_EXIT;
	}

ERR_EXIT:
	error = ataCloseCamera(sensor_id);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("ataCloseCamera failed!");
		return error;
	}

	return error;
}

CCError CCConfigVideoCall(CamRecordParmeters *pVideoRecordParam)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	SENSOR_ID sensor_id = SENSOR_ID_BACK;
	struct isp_pipe_cfg pipeCfg;
	CCBufferQueue *pBufferQue = NULL;
	OSA_STATUS osaStatus = 0;
	PIPE_OUT_FMT format = PIPE_OUT_FMT_YUV420_NV12;
	int_32 i;
	xbool sendWorkFlag = false;
	CCIspPipeSwitch ispPipeSwitch;

	CAMLOGI("ConfigVideoCall start!");

	if (!pVideoRecordParam || !pVideoRecordParam->enqueueCamRecordBuffer
		|| !pVideoRecordParam->flushCamRecordBuffers){
		CAMLOGE("the videoRecordParam is invalid handle=0x%p!", pVideoRecordParam);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	if (pCamDevice->camStatus & CAMERA_STATUS_VIDEO) {
		CAMLOGE("the video recording is in progress!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	pCamDevice->minFps = 5;
	pCamDevice->maxFps = 30;

	pCamDevice->threadSyncFlagRef = INVALID_FLAG_ID;
	/* create event for thred sync. */
	pCamDevice->threadSyncFlagRef = UOS_CreateFlag();

	pCamDevice->previewVideoSamePipe = false;
   //1.prepare for video thread and msgqueue with drv.
	pCamDevice->videoMsgQRef = INVALID_MSGQ_ID;
	pCamDevice->videoMsgQRef = UOS_NewMessageQueue("camVide", TASK_DEFAULT_MSGQ_SIZE);

	pCamDevice->videoTaskRef = UOS_CreateTask(
		CCCamVideoThread,
		(void *)pCamDevice,
		TASK_WITHOUT_MSGQ,
		CAMERA_VIDEO_TASK_SIZE,
		CAMERA_VIDEO_TASK_PRIORITY,
		"uiCamVideo");
	if (pCamDevice->videoTaskRef == NULL) {
		CAMLOGE("UOS_CreateTask for camera video failed, error code = %d!", osaStatus);
		error = CRANE_CAMERA_ERROR_FATALERROR;
		goto Error_exit;
	}

	//2.config isp : enable video port.
	pipeCfg.pipe_id = PIPELINE_ID_VIDEO;
	pipeCfg.pipe_enable = 1;
	pipeCfg.data_range = 0;
	pipeCfg.zoom = pCamDevice->currentZoomLevel;
	pipeCfg.output_format = format;
	pipeCfg.pipe_outw = pVideoRecordParam->recordWidth;
	pipeCfg.pipe_outh = pVideoRecordParam->recordHeight;

	error = configCamera(NULL, &pipeCfg);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("configCamera for camera video failed!");
		goto Error_exit;
	}

	//3.request buffers.
	pBufferQue = &(pCamDevice->videoBufQue);
	error = requestBuffers(pBufferQue, PREVIEW_BUFFER_NUM, PIPELINE_ID_VIDEO, pCamDevice->videoMsgQRef);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("requestBuffers for camera video failed!");
		goto Error_exit;
	}

	//alloc buffer for video
	pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
	pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_start_reocrd");
	pBufferQue->bufFormat = format;;
	pBufferQue->bufCnt = PREVIEW_BUFFER_NUM;
	error = allocBuffers(pBufferQue, pVideoRecordParam->recordWidth, pVideoRecordParam->recordHeight, PIPELINE_ID_VIDEO);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for video failed, %d!", error);
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}

	//4.enqueue all video buffer
	for(i = 0; i < pBufferQue->bufCnt; i++) {
		error = enqueueBuffer(pBufferQue, i, PIPELINE_ID_VIDEO);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for video falied, %d!", i, error);
			goto Error_exit;
		}
	}

	//set paramters for video encoder
	pVideoRecordParam->setReleaseBufferCallBack(CCReleaseBufferToCamera, (void*)(&pCamDevice->videoBufQue));

	pCamDevice->qBufForVideoEncoder = pVideoRecordParam->enqueueCamRecordBuffer;
	pCamDevice->flushVideoEncoderBuffers = pVideoRecordParam->flushCamRecordBuffers;

	//5.notify the thread ready to work
	osaStatus = UOS_SetFlag(pCamDevice->threadSyncFlagRef, VIDEO_READYWORK, OSA_FLAG_OR);
	sendWorkFlag = true;

	//6.stream on
	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.videoSwitch= 1;
	error = streamOnPipe(&ispPipeSwitch);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOnPipe for video falied, %d!", error);
		goto Error_exit;
	}

	pCamDevice->camStatus |= CAMERA_STATUS_VIDEO;
	pCamDevice->videoOperation = 1; //start

	if(pCamDevice->uiParam.nightMode)
		setCameraFPS(pCamDevice->uiParam.nightModeFps, pCamDevice->maxFps, BANDING_50HZ);
	else
		setCameraFPS(pCamDevice->minFps, pCamDevice->maxFps, BANDING_50HZ);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	CAMLOGI("ConfigVideoCall successful!");
	return error;

Error_exit:
	if (sendWorkFlag) {
		UOS_SuspendTask((TASK_HANDLE*)pCamDevice->videoTaskRef);
	}

	if (pCamDevice->videoTaskRef) {
		UOS_DeleteTask((TASK_HANDLE *)pCamDevice->videoTaskRef);
		pCamDevice->videoTaskRef = NULL;
	}

	if (pBufferQue) {
		if (pBufferQue->totalSize > 0) {
			freeBuffers(pBufferQue);
		}

		if (INVALID_MUTEX_ID != pBufferQue->bufQueMutexRef) {
			UOS_FreeMutex(pBufferQue->bufQueMutexRef);
			pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
		}
	}

	if (INVALID_MSGQ_ID != pCamDevice->videoMsgQRef) {
		UOS_FreeMessageQueue(pCamDevice->videoMsgQRef);
		pCamDevice->videoMsgQRef = INVALID_MSGQ_ID;
	}

	if (pCamDevice) {
		UOS_FREE((void*)pCamDevice);
		camCoreHandle.pCamDevice = NULL;
	}

	CAMLOGE("ConfigVideoCall the %d camera failed!", sensor_id);
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;

}

CCError CCStartVideoCall(CamRecordParmeters *pVideoRecordParam)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	SENSOR_ID sensor_id = SENSOR_ID_BACK;

	CAMLOGI("StartVideoCall start!");

	if (!pVideoRecordParam || !pVideoRecordParam->enqueueCamRecordBuffer
		|| !pVideoRecordParam->flushCamRecordBuffers){
		CAMLOGE("the videoRecordParam is invalid handle=0x%p!", pVideoRecordParam);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

	ataOpenCamera(sensor_id);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("open camera %d failed!", sensor_id);
		return error;
	}

	error = CCConfigVideoCall(pVideoRecordParam);

	return error;
}

CCError CCStopVideoCall(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	SENSOR_ID sensor_id = SENSOR_ID_BACK;

	CAMLOGI("CCStopVideoCall");

	error = CCStopRecord();
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("stop record failed!");
	}
	error = ataCloseCamera(sensor_id);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("close camera %d failed!", sensor_id);
		return error;
	}

	CAMLOGI("CCStopVideoCall the %d camera successful!", sensor_id);

	return error;
}

CCError CCSetPara(CamDrvParaMode paraMode, int32 value)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCIspPipeSwitch ispPipeSwitch ;

	if (!pCamDevice) {
		CAMLOGE("must open camera first!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if ((CAMERA_STATUS_IDLE == pCamDevice->camStatus) && (CAMDRV_PARAM_ZOOM_FACTOR== paraMode)) {
		CAMLOGW("start the camera preview first!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);

	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	if (pCamDevice->camStatus & CAMERA_STATUS_PREVIEW)
		ispPipeSwitch.previewSwitch = 1;

	if (pCamDevice->camStatus & CAMERA_STATUS_VIDEO)
		ispPipeSwitch.videoSwitch= 1;

	error = setCamDrvParamter(paraMode, value, &ispPipeSwitch);
	if(error != CRANE_CAMERA_ERROR_NONE)
		goto err_out;

	switch(paraMode){
		case CAMDRV_PARAM_ZOOM_FACTOR:
			//save zomm value for snapshot
			pCamDevice->currentZoomLevel = value;
			CAMLOGI("set currentZoomLevel = 0x%x",value);
			break;
		case CAMDRV_PARAM_WB:
			pCamDevice->exifParam.whiteBlance = (value == HAL_WB_AUTO)?0:1;
			CAMLOGI("set whiteBlance = %d",value);
			break;
			
		case CAMDRV_PARAM_BRIGHTNESS:
			pCamDevice->exifParam.exposureMode = (value == HAL_BRIGHTNESS_mid)?2:1;
			CAMLOGI("set exposureMode = %d",value);
			break;
		case CAMDRV_PARAM_FLASH:
			if(value != HAL_FLASH_OFF)
				pCamDevice->exifParam.flashMode = 1;
			else
				pCamDevice->exifParam.flashMode = 0;
			pCamDevice->uiParam.flashLevel = value;
			CAMLOGI("set flash_mode = 0x%x",value);
			break;
		case CAMDRV_PARAM_NIGHT_MODE:
			pCamDevice->uiParam.nightModeFps = value;
			pCamDevice->uiParam.nightMode = (value == HAL_SCENE_AUTO)?0:1;
			CAMLOGI("set night_mode = 0x%x",value);
			break;
		case CAMDRV_PARAM_QUALITY:
			//save image quality for snapshot
			pCamDevice->imageQuality = value;
			CAMLOGI("set image quality = 0x%x",value);
			break;
		default:
			break;
	}

err_out:
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

CCError CCDoJpegEncode(struct cam_offline_buf *pOfflineOutputBuf, CCBufferQueue *inBufferQue, int_32 inBufIndex, CCBufferQueue *outBufferQue,
							int_32 outBufIndex, xbool isSoftEncode, int_32 rotationDegree, uint_32 imageQulity)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCameraSofJpegInfo inputSoftJpeg, outputSoftJpeg;
	uint_32 jpegSize = 0;
	uint_32 halTimeTick1 = 0, halTimeTick2 = 0;
//	S8 fileName[256];
#if DUMP_JPEG_YUV
	int fileHandle = 0;
#endif

	//thumbnial need put main buffer together.

	CAMLOGD("ready to jpeg encode!");
	memset(&inputSoftJpeg, 0, sizeof(CCameraSofJpegInfo));
	memset(&outputSoftJpeg, 0, sizeof(CCameraSofJpegInfo));

	if (!pOfflineOutputBuf) {
		inputSoftJpeg.addr[0] = inBufferQue->bufPack[inBufIndex].ispBuffer.buffer.planes[0].addr;
		inputSoftJpeg.addr[1] = inBufferQue->bufPack[inBufIndex].ispBuffer.buffer.planes[1].addr;
		inputSoftJpeg.width = inBufferQue->width;
		inputSoftJpeg.height = inBufferQue->height;
		inputSoftJpeg.bufferLen = inBufferQue->unitSize;
	} else {
		inputSoftJpeg.addr[0] = pOfflineOutputBuf->plane_addr[0];
		inputSoftJpeg.addr[1] = pOfflineOutputBuf->plane_addr[1];
		inputSoftJpeg.width = pOfflineOutputBuf->width;
		inputSoftJpeg.height = pOfflineOutputBuf->height;
		inputSoftJpeg.bufferLen = pOfflineOutputBuf->width * pOfflineOutputBuf->height * 3 / 2;
	}
#ifdef REUSE_CAPBUF
	outputSoftJpeg.addr[0] = (uint_32)outBufferQue->jpeg_addr;
#else
	outputSoftJpeg.addr[0] = outBufferQue->bufPack[outBufIndex].ispBuffer.buffer.planes[0].addr;
//	outputSoftJpeg.addr[1] = outBufferQue->bufPack[outBufIndex].ispBuffer.buffer.planes[1].addr;
#endif	
	outputSoftJpeg.width = outBufferQue->width;
	outputSoftJpeg.height = outBufferQue->height;
	outputSoftJpeg.bufferLen = outBufferQue->unitSize;

	CAMLOGI("ready to do %s jpeg encoding from (p:0x%x)%dx%d to (p:0x%x)%dx%d!", isSoftEncode ? "soft" : "hardware", inputSoftJpeg.addr[0], inputSoftJpeg.width,
		inputSoftJpeg.height, outputSoftJpeg.addr[0], outputSoftJpeg.width, outputSoftJpeg.height);

#if DUMP_JPEG_YUV
//	AnsiiToUnicodeString(fileName, "D://yss.nv12");
	fileHandle = FS_Open("D://yuv_capture.nv12", FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	if (fileHandle < 0) {
		CAMLOGE("create file for saving jpeg failed, %d!", fileHandle);
	}

	FS_Write(fileHandle, (UINT8 *)(inputSoftJpeg.addr[0]), inputSoftJpeg.bufferLen);
	if (fileHandle > 0)
		FS_Close(fileHandle);
#endif

	halTimeTick1 = timerCountRead(TCR_2);

#ifdef ENABLE_SOFT_JPEG
	if (isSoftEncode)
		jpegSize = CameraSoftJpegEncode(&inputSoftJpeg, &outputSoftJpeg, imageQulity, 0);
	else
		jpegSize = CameraCodaJpegEncode(&inputSoftJpeg, &outputSoftJpeg, imageQulity, rotationDegree);
#else
	jpegSize = CameraCodaJpegEncode(&inputSoftJpeg, &outputSoftJpeg, imageQulity, rotationDegree);
#endif

	halTimeTick2 = timerCountRead(TCR_2);
	CAMLOGI("jpeg encode 0x%x size, cost time %dms!", jpegSize, (halTimeTick2 - halTimeTick1) / 32);
	if (jpegSize <= 0)
		error = CRANE_CAMERA_ERROR_FATALERROR;

	outBufferQue->multiplexBufferSize = jpegSize;

    return error;
}

CCError CCSaveJpegToFile(uint_32 *buf, uint_32 size)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	char dump_name[100] ="D://camera_dump_jpeg";
	uint_32 fileWriteLen = 0, fileSize = 0;
	int_32 fd =0;

	strcat(dump_name,".jpeg");

	fileSize = size ;
	CAMLOGI("jpeg_dump dump_name %s	fileSize =%d", dump_name,fileSize);

	fd = FS_Open((uint_8 *)dump_name, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	if (fd < 0) {
		CAMLOGE("create file for saving jpeg failed, %d!", fd);
	}

	fileWriteLen = FS_Write(fd, (uint_8 *)(buf), fileSize);
	if (fileWriteLen == 0)
		CAMLOGE("write jpeg failed!");
	if (fd > 0)
		FS_Close(fd);

	return error;
}


static int dump_index = 0;
CCError CCDumpNV12ToFile(uint_32 *buf, uint_32 width,  uint_32 height)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	char dump_name[100] ="D://camera_";
	char tmp_str[40];
	uint_32 fileWriteLen = 0, fileSize = 0;
	int_32 fd =0;

	sprintf(tmp_str,"%d",width);
	strcat(dump_name,tmp_str);
	strcat(dump_name,"x");
	sprintf(tmp_str,"%d",height);
	strcat(dump_name,tmp_str);
	strcat(dump_name,"_");
	sprintf(tmp_str,"%d",dump_index++);
	strcat(dump_name,tmp_str);

	strcat(dump_name,".NV12");

	fileSize = width * height * 3 / 2 ;
	CAMLOGI("YUV dump_name %s  fileSize =%d", dump_name,fileSize);

	fd = FS_Open((uint_8 *)dump_name, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	if (fd < 0) {
		CAMLOGE("create file for saving nv12 failed, %d!", fd);
	}

	fileWriteLen = FS_Write(fd, (uint_8 *)(buf), fileSize);
	if (fileWriteLen == 0)
		CAMLOGE("write nv12 failed!");
	if (fd > 0)
		FS_Close(fd);

	return error;
}

CCError CCDumpRawToFile(uint_32 *buf, uint_32 width,  uint_32 height)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	char dump_name[100] ="D://camera_";
	char tmp_str[40];
	uint_32 fileWriteLen = 0, fileSize = 0;
	int_32 fd =0;
	xbool ret = FALSE;
	TM_SYSTEMTIME pSystemTime;

	ret = TM_GetSystemTime(&pSystemTime);
	if (ret != TRUE) {
		CAMLOGE("TM_GetSystemTime failed, %d!");
	}

	sprintf(tmp_str,"%d",width);
	strcat(dump_name,tmp_str);
	strcat(dump_name,"x");
	sprintf(tmp_str,"%d",height);
	strcat(dump_name,tmp_str);
	strcat(dump_name,"_");
	sprintf(tmp_str,"%d",pSystemTime.uYear);
	strcat(dump_name,tmp_str);
	sprintf(tmp_str,"%0.2d",pSystemTime.uMonth);
	strcat(dump_name,tmp_str);
	sprintf(tmp_str,"%0.2d",pSystemTime.uDay);
	strcat(dump_name,tmp_str);
	sprintf(tmp_str,"%0.2d",pSystemTime.uHour);
	strcat(dump_name,tmp_str);
	sprintf(tmp_str,"%0.2d",pSystemTime.uMinute);
	strcat(dump_name,tmp_str);
	sprintf(tmp_str,"%0.2d",pSystemTime.uSecond);
	strcat(dump_name,tmp_str);

	strcat(dump_name,".raw");

	fileSize = width * height ;
	CAMLOGI("raw_dump dump_name %s  fileSize =%d", dump_name,fileSize);

	fd = FS_Open((uint_8 *)dump_name, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	if (fd < 0) {
		CAMLOGE("create file for saving raw failed, %d!", fd);
	}

	fileWriteLen = FS_Write(fd, (uint_8 *)(buf), fileSize);
	if (fileWriteLen == 0)
		CAMLOGE("write raw failed!");
	if (fd > 0)
		FS_Close(fd);

	return error;
}

CCError _CCWaitCapturePortDone(u8 captureMsgQRef, CCBufferQueue *pBufferQue, int_32 *bufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	struct isp_buf_msg *ispMsg = NULL;
	uint_8 pipe_id = 100, buf_idx = 100;
	uint_32 onlineWaitTime = ONLINE_TIME_OUT;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;

	onlineWaitTime = 1000 / pCamDevice->minFps * 5;
	ispMsg = (struct isp_buf_msg*)UOS_WaitMsg(NULL, captureMsgQRef, onlineWaitTime / TICK_PER_MS);
	if (!ispMsg) {
		CAMLOGE("receive camera drv buffer failed timeout !");
		return CRANE_CAMERA_ERROR_TIMEOUT;
	}

	switch (ispMsg->msg_type) {
		case ISP_MSG_TYPE_EOF:
			/* some buffer ready,we ready to dequeue. */
			pipe_id = ispMsg->u.data_eof.pipe_id;
			buf_idx = ispMsg->u.data_eof.buf_idx;
			CAMLOGV("receive the %d buffer done for capture,0x%p!",
			buf_idx, pBufferQue->bufPack[buf_idx].ispBuffer.buffer.planes[0].addr);
			error = dequeueBuffer(pBufferQue, buf_idx, pipe_id);
			if (error != CRANE_CAMERA_ERROR_NONE) {
				CAMLOGE("dequeueBuffer the %d buffer failed for 0x%x!", buf_idx, error);
				error = CRANE_CAMERA_ERROR_FATALERROR;
			} else
				*bufIndex = buf_idx;
			break;
		default:
			error = CRANE_CAMERA_ERROR_NOTSUPPORT;
			break;
	}

	UOS_FREE((void*)ispMsg);
	ispMsg = NULL;

	return error;
}

/*
 * @brief: capture bottom half process,such as scale, zoom, rotation, jpeg and so on.
 * @param: offlineOption
 * @param: inBufferQue, the BufferQue contain input buffer for offline.
 * @param: inOnlineIndex, the index of input buffer in inBufferQue, this buffer comes from the online port p2.
 * @param: inOfflineIndex, the index of output buffer in inBufferQue, this buffer use for offline zoom.
 * @param: outBufferQue, the BufferQue contain some output buffer for offline, such as thumbnail, back display, this bufferqueue si previewbufferqueue.
 * @param: needBackDisplay 1.do back display 0.need not back display
 */
CCError _CCCaptureBottomHalfProcess(CCOfflineOption *offlineOption, CCBufferQueue *inBufferQue,
                                    int_32 inOnlineIndex, CCBufferQueue *outBufferQue, int_32 needBackDisplay)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	//CCError tempError = CRANE_CAMERA_ERROR_NONE;
	//int_32 thumOutBufIndex = -1, backDispBufIndex = 0;
	CCOfflineParamter offlineParamter;
	//struct cam_offline_buf *pThumbOutputBuf = NULL;
	struct cam_offline_buf mianZoomOutputBuf;
	uint_32 *pJpegOutIndex = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCBufferQueue *pOfflineBufferQue = NULL;
	int_32 inOfflineIndex = 0, rotationDegree = 0;
	uint_32 halTimeTick1 = 0, halTimeTick2 = 0;
	uint_32 capWidth,capHeight = 0;
	uint_32 imageQuality = 0;

	memset(&offlineParamter, 0, sizeof(CCOfflineParamter));
	memset(&mianZoomOutputBuf, 0, sizeof(struct cam_offline_buf));
	pOfflineBufferQue = &(pCamDevice->offlineBufQue);
	capWidth = pOfflineBufferQue->width;
	capHeight = pOfflineBufferQue->height;
	pJpegOutIndex = &(pCamDevice->jpegOutIndex);
	*pJpegOutIndex = 0;
#if 0
    //thumbnail
    if (offlineOption->thumbnail) {
        pThumbOutputBuf = &pCamDevice->thumbOutputBuf;
        //get idle buffer for output, reuse the prview buffer because the memory resource is shortage.
        error = queueBufIndexAlloc(outBufferQue, &thumOutBufIndex, CC_BUFFER_STATUS_OFFLINE);
        if (error != CRANE_CAMERA_ERROR_NONE || thumOutBufIndex == -1) {
            CAMLOGE("can not find the idle buffer for thumbnail, 0x%x!", error);
            goto Error_exit;
        }
        offlineParamter.format = offlineOption->thumb_format;
        offlineParamter.width = offlineOption->thumb_width;
        offlineParamter.height = offlineOption->thumb_height;
        offlineParamter.zoomLevel = offlineOption->zoomLevel;
        //reuse the first plane buffer.
        outBufferQue->bufPack[thumOutBufIndex].planIndex = 0;
        error = getZoomScale(&offlineParamter, inBufferQue, inOnlineIndex, outBufferQue, thumOutBufIndex);
        if (error != CRANE_CAMERA_ERROR_NONE) {
            CAMLOGE("scale for thumbnail failed, 0x%x!", error);
            goto Error_exit;
        }

        //rotation
        if (offlineOption->rotation) {
            offlineParamter.rotation = offlineOption->rotationDegree;
            outBufferQue->bufPack[thumOutBufIndex].planIndex = 1;
            error = getRotation(&offlineParamter, NULL, -1, outBufferQue, thumOutBufIndex);
            if (error != CRANE_CAMERA_ERROR_NONE) {
                CAMLOGE("thumbnail rotation failed, 0x%x!", error);
                goto Error_exit;
            }
        }

		//set data ready flag to this buffer.
		queueBufIndexFree(outBufferQue, thumOutBufIndex);
        //save the final thumbnail buffer.
        memcpy(pThumbOutputBuf, &offlineParamter.offlineOutputBuf, sizeof(struct cam_offline_buf));
    }
#endif

	//back display
	memset(&offlineParamter, 0, sizeof(CCOfflineParamter));
	offlineParamter.zoomLevel = offlineOption->zoomLevel;
	if(needBackDisplay)
		_CCCaptureBackDisplay(&offlineParamter,inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex);
#ifdef REUSE_CAPBUF
	// recovery offlinebq
	memcpy(pOfflineBufferQue,inBufferQue,sizeof(struct _CCBufferQueue));
	pOfflineBufferQue->multiplexBufferSize = 0;
	pOfflineBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pOfflineBufferQue->bufCnt = 1;
#endif
	pOfflineBufferQue->width = capWidth;
	pOfflineBufferQue->height= capHeight;
	pOfflineBufferQue->unitSize = capWidth * capHeight /3 *2;

	//the main picture zoomScale
	if ((pOfflineBufferQue->width != inBufferQue->width && pOfflineBufferQue->height != inBufferQue->width)
		|| (pOfflineBufferQue->height != inBufferQue->height && pOfflineBufferQue->width != inBufferQue->height)) {
		//need to scale
		offlineOption->zoomScale = true;
	}

	if (offlineOption->zoomScale) {
		memset(&offlineParamter, 0, sizeof(CCOfflineParamter));
		offlineParamter.format = pOfflineBufferQue->bufFormat;
		if (pOfflineBufferQue->width < pOfflineBufferQue->height) {
			//buffer rotation
			offlineParamter.width = pOfflineBufferQue->height;
			offlineParamter.height = pOfflineBufferQue->width;
		} else {
			offlineParamter.width = pOfflineBufferQue->width;
			offlineParamter.height = pOfflineBufferQue->height;
		}
		offlineParamter.zoomLevel = offlineOption->zoomLevel;
		halTimeTick1 = timerCountRead(TCR_2);
#ifdef REUSE_CAPBUF
		error = getZoomScale(&offlineParamter, inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex,true);
#else
		error = getZoomScale(&offlineParamter, inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex,false);
#endif
		halTimeTick2 = timerCountRead(TCR_2);
		CAMLOGV("getZoomScale end cost time =%d ms  ",(halTimeTick2 - halTimeTick1) / 32);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("offline zoomScale  failed, 0x%x!", error);
			goto Error_exit;
		}

		CAMLOGD("capture offline zoomscale,zoomlevel=0x%x!", offlineParamter.zoomLevel);
		//save zoom buffer.
		memcpy(&mianZoomOutputBuf, &offlineParamter.offlineOutputBuf, sizeof(struct cam_offline_buf));
	}

	//main picture rotation, hardware jpeg encoder can do rotation.
	if (offlineOption->rotation && offlineOption->softJpeg) {
		memset(&offlineParamter, 0, sizeof(CCOfflineParamter));
		offlineParamter.format = pOfflineBufferQue->bufFormat;
		offlineParamter.zoomLevel = offlineOption->zoomLevel;
		offlineParamter.rotation = offlineOption->rotationDegree;

		if ((ROT_90_DEG == offlineParamter.rotation || ROT_270_DEG == offlineParamter.rotation)
		&& (pOfflineBufferQue->width > pOfflineBufferQue->height)) {
			offlineParamter.width = pOfflineBufferQue->height;
			offlineParamter.height = pOfflineBufferQue->width;
		} else {
			offlineParamter.width = pOfflineBufferQue->width;
			offlineParamter.height = pOfflineBufferQue->height;
		}

		if (offlineOption->zoomScale) {
			//the input buffer comes from zoom buffer, the output save in inOnlineIndex.
			memcpy(&offlineParamter.offlineOutputBuf, &mianZoomOutputBuf, sizeof(struct cam_offline_buf));
			error = getRotation(&offlineParamter, NULL, -1, inBufferQue, inOnlineIndex);
		} else {
			//the input buffer comes from inBufferQue, the output save in inOfflineIndex.
			error = getRotation(&offlineParamter, inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex);
		}

		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("offline zoomScale  failed, 0x%x!", error);
			goto Error_exit;
		}
		CAMLOGD("capture soft rotation %d!", offlineParamter.rotation);
	}

	switch (pCamDevice->imageQuality) {
		case HAL_QTY_LOW:
			imageQuality = 65;
			break;
		case HAL_QTY_NORMAL:
			imageQuality = 75;
			break;
		case HAL_QTY_FINE:
			imageQuality = 85;
			break;
		case HAL_QTY_SUPER_FINE:
			imageQuality = 95;
			break;
		default:
			imageQuality = 65;
			break;
	}
	CAMLOGI("capture image quality %d!", imageQuality);

#ifdef ENABLE_SOFT_JPEG
	//jpeg
	if (offlineOption->softJpeg) {
		//do jpeg by soft.
		if ((offlineOption->zoomScale && !offlineOption->rotation)
		|| (!offlineOption->zoomScale && offlineOption->rotation)) {
			//this case the final yuv buffer is pOfflineBufferQue inOfflineIndex.
			error = CCDoJpegEncode(&offlineParamter.offlineOutputBuf, NULL, -1, inBufferQue, inOnlineIndex, true, 0, imageQuality);
			*pJpegOutIndex = 1; //jpegout is in capturebufque
		} else if (offlineOption->zoomScale && offlineOption->rotation) {
			//this case the final yuv buffer is inBufferQue inOnlineIndex.
			error = CCDoJpegEncode(&offlineParamter.offlineOutputBuf, NULL, -1, pOfflineBufferQue, inOfflineIndex, true, 0, imageQuality);
			*pJpegOutIndex = 2; //jpegout is in OfflineBufferQue
		} else {
			//no zomm no rotation, the final yuv buffer is inBufferQue inOnlineIndex.
			error = CCDoJpegEncode(NULL, inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex, true, 0, imageQuality);
			*pJpegOutIndex = 2; //jpegout is in OfflineBufferQue
		}
	} else
#endif
	{
	    	switch (offlineOption->rotationDegree) {
				case ROT_0_DEG:
					rotationDegree = 0;
					break;
				case ROT_90_DEG:
					rotationDegree = 90;
					break;
				case ROT_180_DEG:
					rotationDegree = 180;
					break;
				case ROT_270_DEG:
					rotationDegree = 270;
					break;
				default:
					CAMLOGE("unsupported rotation degree %d!", offlineOption->rotationDegree);
					break;
	    	}
	        //do jpeg by hardware.
#ifdef REUSE_CAPBUF
		if (offlineOption->zoomScale) {
			//this case the final yuv buffer is pOfflineBufferQue inOfflineIndex.
			error = CCDoJpegEncode(&offlineParamter.offlineOutputBuf, NULL, -1, pOfflineBufferQue, inOfflineIndex, false, rotationDegree, imageQuality);
			*pJpegOutIndex = 1; //jpegout is in capturebufque
		} else {
			//this case the final yuv buffer is capBufferQue .
			error = CCDoJpegEncode(NULL, inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex, false, rotationDegree, imageQuality);
			*pJpegOutIndex = 2; //jpegout is in OfflineBufferQue
		}
#else
	        if (offlineOption->zoomScale) {
			//this case the final yuv buffer is pOfflineBufferQue inOfflineIndex.
			error = CCDoJpegEncode(&offlineParamter.offlineOutputBuf, NULL, -1, inBufferQue, inOnlineIndex, false, rotationDegree, imageQuality);
			*pJpegOutIndex = 1; //jpegout is in capturebufque
	        } else {
			//this case the final yuv buffer is inBufferQue inOnlineIndex.
			error = CCDoJpegEncode(NULL, inBufferQue, inOnlineIndex, pOfflineBufferQue, inOfflineIndex, false, rotationDegree, imageQuality);
			*pJpegOutIndex = 2; //jpegout is in OfflineBufferQue
	        }
#endif
	}

 //   saveJpegToFile(pThumbOutputBuf, inBufferQue, jpegOutIndex);

Error_exit:

	return error;
}

/*
 *Do somethings like free the memory after capture.
 *We free memory used by capture when close camera which is in order to make capture quickly.
 */
void _CCPostCapture(void)
{
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
 	CCBufferQueue *pBufferQue = NULL;
#ifndef REUSE_CAPBUF
	CCBufferQueue *pOfflineBufferQue = NULL;
#endif
	if (!pCamDevice->captureResourceStatus)
		return;

	pBufferQue = &(pCamDevice->captureBufQue);
	if (pBufferQue) {
		UOS_FreeMutex(pBufferQue->bufQueMutexRef);
		freeBuffers(pBufferQue);
		memset(pBufferQue, 0, sizeof(CCBufferQueue));
	}

#ifndef REUSE_CAPBUF
	pOfflineBufferQue = &(pCamDevice->offlineBufQue);
	if (pOfflineBufferQue) {
		freeBuffers(pOfflineBufferQue);
		memset(pOfflineBufferQue, 0, sizeof(CCBufferQueue));
	}
#endif

	if (pCamDevice->captureMsgQRef != INVALID_MSGQ_ID) {
		UOS_FreeMessageQueue(pCamDevice->captureMsgQRef);
		pCamDevice->captureMsgQRef = INVALID_MSGQ_ID;
	}

	pCamDevice->captureResourceStatus = 0;
}

/*Do somethings like prepare the memory before capture.*/
CCError _CCPreCapture(CCCaputreStruct *capture_data)
{
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	//uint_32 halTimeTick1 = 0, halTimeTick2 = 0;
	struct isp_pipe_cfg pipeCfg;
	uint_16 jpeg_head = 0;
	xbool rawDump = false;
	PIPE_OUT_FMT output_format = PIPE_OUT_FMT_YUV420_NV12;
	CCBufferQueue *pBufferQue = NULL, *pOfflineBufferQue = NULL;

	rawDump = isp_get_rawdump_flag();
	if(rawDump){
		output_format = PIPE_OUT_FMT_YUV420_YV12;
		rawDumpCfg(1);
	} else {
		output_format = PIPE_OUT_FMT_YUV420_NV12;
	}

	//1.config isp : enable snapshot port.
	pipeCfg.pipe_id = PIPELINE_ID_CAP;
	pipeCfg.pipe_enable = 1;
	pipeCfg.data_range = 0;
	pipeCfg.zoom = 0x100;
	pipeCfg.output_format = output_format;
	pipeCfg.pipe_outw = pCamDevice->sensorWidth;
	pipeCfg.pipe_outh = pCamDevice->sensorHeight;
	//jpeg_head = pipeCfg.pipe_outh / 3 ; //jpeg_head = 1/3 image_height
#ifdef CONFIG_JPGE_MEMORY_SIZE
	jpeg_head = pipeCfg.pipe_outh * CONFIG_JPGE_MEMORY_SIZE;
#else
	switch (pCamDevice->imageQuality) {
		case HAL_QTY_LOW:
			jpeg_head = pipeCfg.pipe_outh / 3; //jpeg_head = 1/3 image_height;
			break;
		case HAL_QTY_NORMAL:
			jpeg_head = pipeCfg.pipe_outh / 2; //jpeg_head = 1/2 image_height;
			break;
		case HAL_QTY_FINE:
			jpeg_head = pipeCfg.pipe_outh * 3/5; //jpeg_head = 3/5 image_height;
			break;
		case HAL_QTY_SUPER_FINE:
			jpeg_head = pipeCfg.pipe_outh * 4/5; //jpeg_head = 4/5 image_height;
			break;
		default:
			jpeg_head = pipeCfg.pipe_outh / 3; //jpeg_head = 1/3 image_height
			break;
	}
#endif
	configCamera(/* paramter, */NULL, &pipeCfg);
	
	CAMLOGV("configCamera for capture successfully!");

	//2.alloc buffer for capture.
	pBufferQue = &(pCamDevice->captureBufQue);
	pBufferQue->bufPackIndex = -1;
	pBufferQue->multiplexBufferSize = 0;
	pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
	pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_pre_cap");
	pBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pBufferQue->bufCnt = SNAPSHOT_BUFFER_NUM; //yuv output
	pBufferQue->jpeg_head = jpeg_head;

	error = allocBuffers(pBufferQue, pipeCfg.pipe_outw, pipeCfg.pipe_outh, PIPELINE_ID_CAP);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for capture failed, %d!", error);
		goto Error_exit;
	}

	//3.create msg handle.
	pCamDevice->captureMsgQRef = INVALID_MSGQ_ID;
	pCamDevice->captureMsgQRef = UOS_NewMessageQueue("camCapt", TASK_DEFAULT_MSGQ_SIZE);

	//4.alloc buffer for jpeg or resue capture buffer for jpeg output.
	pOfflineBufferQue = &(pCamDevice->offlineBufQue);

#ifdef REUSE_CAPBUF
	//resue capture buffer for jpeg output.
	memcpy(pOfflineBufferQue,pBufferQue,sizeof(struct _CCBufferQueue));
	pOfflineBufferQue->multiplexBufferSize = 0;
	pOfflineBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pOfflineBufferQue->bufCnt = 1;
	pOfflineBufferQue->width = capture_data->image_width;
	pOfflineBufferQue->height= capture_data->image_height;
 #else
 	pOfflineBufferQue->multiplexBufferSize = 0;
	pOfflineBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pOfflineBufferQue->bufCnt = 1;
	error = allocBuffers(pOfflineBufferQue, capture_data->image_width, capture_data->image_height, PIPELINE_ID_CAP);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for offline failed, %d!", error);
		goto Error_exit;
	}
 #endif

	pCamDevice->captureResourceStatus = 1;
	return error;

Error_exit:
    pCamDevice->camStatus &= (~CAMERA_STATUS_CAPTURE);

	if (INVALID_MUTEX_ID != pBufferQue->bufQueMutexRef) {
		UOS_FreeMutex(pBufferQue->bufQueMutexRef);
		pBufferQue->bufQueMutexRef = INVALID_MUTEX_ID;
	}
	freeBuffers(pBufferQue);
	memset(pBufferQue, 0, sizeof(CCBufferQueue));

#ifndef REUSE_CAPBUF
	if (pOfflineBufferQue) {
		freeBuffers(pOfflineBufferQue);
		memset(pOfflineBufferQue, 0, sizeof(CCBufferQueue));
	}
#endif

	if (pCamDevice->captureMsgQRef != INVALID_MSGQ_ID) {
		UOS_FreeMessageQueue(pCamDevice->captureMsgQRef);
		pCamDevice->captureMsgQRef = INVALID_MSGQ_ID;
	}
   
	pCamDevice->captureResourceStatus = 0;
	return error;
}

void _CCSetSoftjpegOrHardware(CCOfflineOption *pOfflineOption)
{
	CCBufferQueue *pOfflineBufferQue = NULL;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;

	pOfflineBufferQue = &(pCamDevice->offlineBufQue);
	if (pOfflineBufferQue->width < pOfflineBufferQue->height) {
		//need to rotation
		pOfflineOption->rotation= true;
		pOfflineOption->rotationDegree = ROT_90_DEG;
    }

	pOfflineOption->softJpeg = true;

	if(CONFIG_CAM_HW_VERSION != CAM_HW_VERSION_1) {
		if ((pOfflineBufferQue->width % CC_CAM_ALIGN_TO_16) == 0 ) {
			pOfflineOption->softJpeg = false;
		}
	}
}

CCError CCTakePicture(CCCaputreStruct *capture_data)
{
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	//    struct isp_pipe_cfg pipeCfg;
	CCBufferQueue *pBufferQue = NULL;
	int_32 i;
#ifdef REUSE_CAPBUF
	int_32 bufIndex;
	CCIspPipeSwitch ispPipeSwitch;
	CCOfflineOption offlineOption;
#endif
	uint_32 halTimeTick1 = 0, halTimeTick2 = 0;
	struct isp_output_size ispResInfo;
#ifdef ENABLE_ONLINE_JPEG
	struct isp_pipe_cfg pipe_cfg;
	uint_32 jpeg_size = 0;
#endif
	uint_32 reciprocal_exp=0;
	uint_32 iso = 0;
//	int fileHandle = 0, fileWriteLen = 0;
	xbool rawDump = false;

	CAMLOGD("CCTakePicture start size wxh=[%d x %d]",capture_data->image_width,capture_data->image_height);

	halTimeTick1 = timerCountRead(TCR_2);
	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	if (!pCamDevice || !(pCamDevice->camStatus & CAMERA_STATUS_PREVIEW)) {
		CAMLOGE("must open camera and start preview first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}
	if (pCamDevice->camStatus == (CAMERA_STATUS_VIDEO | CAMERA_STATUS_PREVIEW)) {
		CAMLOGE("can't support video snapshot!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}
	if (pCamDevice->camStatus & CAMERA_STATUS_CAPTURE) {
		CAMLOGE("the capture is in progress!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}
	CameraFlashPowerOnOff(pCamDevice->uiParam.flashLevel);
	getCameraEXPandISO(&reciprocal_exp, &iso);
	pCamDevice->exifParam.exposureTime = reciprocal_exp;
	pCamDevice->exifParam.isoSpeed = iso;
	pCamDevice->exifParam.imageWidth = capture_data->image_width;
	pCamDevice->exifParam.imageHeight = capture_data->image_height;

	// get sensor size(isp p2 output size)
	error = getIspRes(&ispResInfo);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("getIspRes failed, %d!", error);
		goto Error_exit;
	}

	pCamDevice->sensorWidth = ispResInfo.width;
	pCamDevice->sensorHeight = ispResInfo.height;

	CAMLOGD("sensor output size wxh[%d %d]", pCamDevice->sensorWidth, pCamDevice->sensorHeight);
	//1.stop enqueue buffer to preview for that we need reuse the preview buffers, and stop preview.
	pCamDevice->camStatus |= CAMERA_STATUS_CAPTURE;
	_CCStopPreview(CC_PREVIEW_STOP_FLUSH);

#ifdef ENABLE_ONLINE_JPEG
	//1.config preview pipeline
	memset(&pipe_cfg, 0, sizeof(pipe_cfg));
	pipe_cfg.pipe_id = PIPELINE_ID_PREV;
	pipe_cfg.pipe_enable = 1;
	pipe_cfg.data_range = 0;
	pipe_cfg.zoom = 0x100;
	pipe_cfg.output_format = PIPE_OUT_FMT_YUV420_NV12;
	pipe_cfg.pipe_outw = pCamDevice->lcdHeight;//pCamDevice->lcdWidth;
	pipe_cfg.pipe_outh = pCamDevice->lcdWidth;//pCamDevice->lcdHeight;
	configCamera(NULL, &pipe_cfg);

	//2.alloc 1 preview buffer
	pBufferQue = &(pCamDevice->previewBufQue);
	pBufferQue->bufPackIndex = -1;
	pBufferQue->multiplexBufferSize = 0;
//	pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_take_pic_preview");
	pBufferQue->bufFormat = PIPE_OUT_FMT_YUV420_NV12;
	pBufferQue->bufCnt = 1;
	error = allocBuffers(pBufferQue, pipe_cfg.pipe_outw, pipe_cfg.pipe_outh, PIPELINE_ID_PREV);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for online preview failed, %d!", error);
		goto Error_exit;
	}

	//3.config jpeg capture pipeline
	memset(&pipe_cfg, 0, sizeof(pipe_cfg));
	pipe_cfg.pipe_id = PIPELINE_ID_CAP;
	pipe_cfg.pipe_enable = 1;
	pipe_cfg.data_range = 0;
	pipe_cfg.zoom = 0x100;
	pipe_cfg.output_format = PIPE_OUT_FMT_YUV420_NV12;
	pipe_cfg.pipe_outw = pCamDevice->sensorWidth; //must set isp output size
	pipe_cfg.pipe_outh = pCamDevice->sensorHeight; //must set isp output size
	configCamera(NULL, &pipe_cfg);

	//4.alloc jpeg buffer for online capture
	pBufferQue = &(pCamDevice->captureBufQue);
	pBufferQue->bufPackIndex = -1;
	pBufferQue->multiplexBufferSize = 0;
	pBufferQue->bufQueMutexRef = UOS_NewMutex("cam_task_pic_online");
	pBufferQue->bufFormat = PIPE_OUT_FMT_JPEG;
	pBufferQue->bufCnt = 1;
	pBufferQue->jpeg_head = 0; //use main buffer, no extra jpeg buffer.
	error = allocBuffers(pBufferQue, pipe_cfg.pipe_outw, pipe_cfg.pipe_outh, PIPELINE_ID_CAP);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("allocBuffers for online capture failed, %d!", error);
		goto Error_exit;
	}
	pCamDevice->captureResourceStatus = 1;

	//5.preview buffer operations, request->enqueue ... dequeue(previewThread)->display(previewThread)
	error = requestBuffers(&pCamDevice->previewBufQue, 1, PIPELINE_ID_PREV, pCamDevice->previewMsgQRef);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE(" requestBuffers for online capture preview failed, error code = %d!", error);
		goto Error_exit;
	}

	pCamDevice->previewBufQue.queueStatus = 1;
	for(i = 0; i < pCamDevice->previewBufQue.bufCnt; i++) {
		error = enqueueBuffer(&pCamDevice->previewBufQue, i, PIPELINE_ID_PREV);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for online capture preview falied, %d!", i, error);
			goto Error_exit;
		}
	}

	//6.online jpeg buffer operations, streamon(with buffer)->streamoff
	pCamDevice->captureMsgQRef = UOS_NewMessageQueue("camOnlineCapt", TASK_DEFAULT_MSGQ_SIZE);
	error = requestBuffers(&pCamDevice->captureBufQue, SNAPSHOT_BUFFER_NUM, PIPELINE_ID_CAP, pCamDevice->captureMsgQRef);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE(" requestBuffers for online capture  failed, error code = %d!", error);
		goto Error_exit;
	}
	for(i = 0; i < SNAPSHOT_BUFFER_NUM; i++) {
		error = enqueueBuffer(&pCamDevice->captureBufQue, i, PIPELINE_ID_CAP);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for capture falied, %d!", i, error);
			goto Error_exit;
		}
	}

	jpeg_size = streamOnOnlineCapture(&pBufferQue->bufPack[0].ispBuffer.buffer);
	error = (jpeg_size > 0) ? CRANE_CAMERA_ERROR_NONE : CRANE_CAMERA_ERROR_FATALERROR;
	pBufferQue->multiplexBufferSize = jpeg_size;
	pCamDevice->jpegOutIndex = 1;
	streamOffOnlineCapture();
#else
	error = _CCPreCapture(capture_data);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		goto Error_exit;
	}
	pBufferQue = &(pCamDevice->captureBufQue);
	requestBuffers(pBufferQue, SNAPSHOT_BUFFER_NUM, PIPELINE_ID_CAP, pCamDevice->captureMsgQRef);

	//5.enqueue all capture buffer
	for(i = 0; i < SNAPSHOT_BUFFER_NUM; i++) {
		error = enqueueBuffer(pBufferQue, i, PIPELINE_ID_CAP);
		if (error != CRANE_CAMERA_ERROR_NONE) {
			CAMLOGE("enqueue the %d Buffer for capture falied, %d!", i, error);
			goto Error_exit;
		}
	}
	//6.stream on
	memset(&ispPipeSwitch, 0, sizeof(CCIspPipeSwitch));
	ispPipeSwitch.snapshotSwitch= 1;
	error = streamOnPipe(&ispPipeSwitch);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOnPipe for capture falied, %d!", error);
		goto Error_exit;
	}
	//7.wait for capture port output data.
	bufIndex = -1;
	error = _CCWaitCapturePortDone(pCamDevice->captureMsgQRef, pBufferQue, &bufIndex);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("waitCaptureDone falied, %d!", error);
		streamOffPipe(&ispPipeSwitch, true);
		goto Error_exit;;
	}
	//8.streamoff
	error = streamOffPipe(&ispPipeSwitch, true);
	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("streamOffPipe for capture falied, %d!", error);
	}

#if 0
	fileHandle = FS_Open("D://online_capture.nv12", FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	if (fileHandle < 0) {
		CAMLOGE("create file for saving jpeg failed, %d!", fileHandle);
	}
	CacheInvalidateMemory((void *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->unitSize);
	fileWriteLen = FS_Write(fileHandle, (UINT8 *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->unitSize);
	CAMLOGD("dump online_capture buffer %d!", fileWriteLen);
	if (fileHandle > 0)
		FS_Close(fileHandle);
#endif

	rawDump = isp_get_rawdump_flag();
	if(rawDump){
		CCDumpRawToFile((uint_32 *)(pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[0].addr), pBufferQue->width, pBufferQue->height);
	}

	//9. capture bottom half process
	memset(&offlineOption, 0, sizeof(offlineOption));
	offlineOption.thumbnail = false;
	offlineOption.thumb_format = PIPE_OUT_FMT_YUV420_NV12;
	if (pCamDevice->currentZoomLevel > 0x100) {
		offlineOption.zoomScale = true;
		offlineOption.zoomLevel = pCamDevice->currentZoomLevel;
	} else {
		offlineOption.zoomScale = false;
		offlineOption.zoomLevel = 0x100;
	}

	_CCSetSoftjpegOrHardware(&offlineOption);

#ifndef ENABLE_SOFT_JPEG
	if(offlineOption.softJpeg == true) {
		CAMLOGE("no soft jpeg lib available!");
		goto Error_exit;
	}
#endif

	//error = _CCCaptureBottomHalfProcess(&offlineOption, pBufferQue, bufIndex, &pCamDevice->previewBufQue);
	error = _CCCaptureBottomHalfProcess(&offlineOption, pBufferQue, bufIndex, NULL, true);

	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("capture bottom half process failed!");
		goto Error_exit;
	}

	if(rawDump){
		rawDumpCfg(0);
	}

#endif
	CameraFlashPowerOnOff(HAL_FLASH_OFF);
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	halTimeTick2 = timerCountRead(TCR_2);
	CAMLOGI("CCTakePicture cost %dms !", (halTimeTick2 - halTimeTick1) / 32);

	return error;
Error_exit:
	//we should make sure the idle buffers in prevewbufferque caused by us are need to enqueue to drvier.
//	_CCTryEnqueueIdleBuffers(&pCamDevice->previewBufQue);
	CAMLOGE("TakePicture failed ");
	_CCPostCapture();
	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	return error;
}

int _CCSumExifSize(uint16_t num_of_ifd0, uint16_t num_of_ifd0_exif, uint16_t* total_exif_len)
{

	uint16_t len1 = 0,len2 = 0,len3 = 0,len4 = 0,len5 = 0,len6 = 0,len7 = 0;
	uint16_t len8= 0 ,len9= 0,len10 = 0 ,len11= 0,len12 = 0,len13 = 0,len14 = 0;
	uint16_t total_app1_len = 0;

	len1 = sizeof(exif_soi_marker) ;
	len2 = sizeof(exif_app1_marker) ;
	len3 = app1_size_item ;
	len4 = sizeof(app1_exif_header_marker);
	len5 = sizeof(app1_tiff_header_marker);
	len6 = ifd0_num_size;
	len7 = (sizeof(IFD0) / sizeof(IFD0[0])) * ifd0_item_size ;

	while(num_of_ifd0) {
		num_of_ifd0 -- ;
		if((IFD0[num_of_ifd0].format == EXIF_ASCII) && ((strlen(IFD0[num_of_ifd0].tagName) + 1) >= 5))
			len8 += (strlen(IFD0[num_of_ifd0].tagName) + 1);
	}

	len9 = ifd0_num_size;
	len10 = (sizeof(IFD0_EXIF) / sizeof(IFD0_EXIF[0])) * ifd0_item_size ;

	while(num_of_ifd0_exif) {
		num_of_ifd0_exif -- ;
		if(IFD0_EXIF[num_of_ifd0_exif].format == EXIF_URATIONAL)
			len11 += 0x8;
	}
	len14 = exif_end_num_size ;
	len12 = last_ifd_dir_size ;
	len13 = last_ifd_data_size ;
	
	total_app1_len = len3 + len4 + len5 + len6 + len7 + len8 + len9 + len10 + len11 + len12 + len13 + len14;
	*total_exif_len = len1 + len2 + total_app1_len;
	CAMLOGV("total_app1_len = 0x%x, total_exif_len 0x%x  len8 0x%x", total_app1_len,*total_exif_len, len8);
	return 0;
}

int _CCAddExifFlag(char* jpeg_exif_marker, char **jpeg_exif, int len1)
{
	memcpy(*jpeg_exif,jpeg_exif_marker,len1);
	*jpeg_exif += len1;

	return 0;
}

int _CCIFDOffsetOfASCII(IFD_S* ifd, uint16_t num_of_ifd, CCExifParamter* exifParamter,int item)
{
//	uint16_t index;
//	uint16_t offset_from_last;
	uint16_t offset_ifd_ascii;
	uint16_t offset_ifd_dir;
	uint16_t ifd0_size_len;
	uint16_t last_ifd_offset;
	uint16_t ifd0_item_len;

    ifd0_size_len = ifd0_length_size;
	last_ifd_offset = last_ifd_dir_size;
	ifd0_item_len = ifd0_item_size;

/*
    for(index = 0 ; index <= item; index++)
    {
        while(item) {
			item -- ;
			if((IFD0[item].format == 0x02) && ((strlen(IFD0[item].tagName) + 1) >= 5))
		    	offset_from_last += (strlen(IFD0[item].tagName) + 1);
        }
	}
*/

	if((ifd[item].format == EXIF_ASCII) && ((strlen(ifd[item].tagName) + 1) >= 5)){
		exifParamter->exif_ifd_offset_cnt++;
	}
	
	offset_ifd_dir = (num_of_ifd - 1 - item) * ifd0_item_len;
	offset_ifd_ascii = (ifd0_size_len  + offset_ifd_dir + last_ifd_offset  + exifParamter->exif_ifd_ascii_len);
	if(exifParamter->exif_ifd_offset_cnt)
		exifParamter->exif_ifd_ascii_len += (strlen(ifd[item].tagName) + 1);
	CAMLOGV("offset_ifd_ascii 0x%x exif_ifd_ascii_len 0x%x", offset_ifd_ascii, exifParamter->exif_ifd_ascii_len);
	return offset_ifd_ascii;
}

int _CCAddIFDASCII(IFD_S* ifd, CCExifParamter* exifParamter, int item, char **jpeg_exif, uint16_t num_of_ifd)
{
	uint16_t len = 0;
	uint16_t ascii_offset = 0;
	uint16_t ifd_dir_offset = 0;

	len = sizeof(ifd[item].tagNO);
	memcpy(*jpeg_exif,&ifd[item].tagNO,len);
	*jpeg_exif += len;

	len = sizeof(ifd[item].format);
	memcpy(*jpeg_exif,&ifd[item].format,len);
	*jpeg_exif += len;

//ifd ascii length's size 
	len = strlen(ifd[item].tagName) + 1;
	*((*jpeg_exif)++) = len;
	*jpeg_exif += 3;

	if( len >= 5)
	{
		ifd_dir_offset = (*jpeg_exif - exifParamter->tiff_header_tag);
		ascii_offset =_CCIFDOffsetOfASCII(ifd,num_of_ifd,exifParamter,item);
		CAMLOGV("ifd_dir_offset =0x%x ascii_offset 0x%x", ifd_dir_offset, ascii_offset);
		strcat((*jpeg_exif +ascii_offset), ifd[item].tagName);
		*((*jpeg_exif)++) = ascii_offset +ifd_dir_offset; //ascii_offset + ifd0_length_size + (2 - item) * 12 + last_ifd_offset_size + (item) * (strlen(IFD0[0].tagName) + 1);
		*jpeg_exif += 3;
	}else{
		strcat(*jpeg_exif, ifd[item].tagName);
		*jpeg_exif += 4;
	}
	CAMLOGV(" _CCAddIFDASCII addr =0x%x", *jpeg_exif);
	return 0;

}

int _CCAddExifIFDPointer(IFD_S* ifd, CCExifParamter* exifParamter, int item, char **jpeg_exif)
{
	uint16_t len = 0;
	uint16_t ifd_dir_offset = 0;

	len = sizeof(ifd[item].tagNO);
	memcpy(*jpeg_exif,&ifd[item].tagNO,len);
	*jpeg_exif += len;

	len = sizeof(ifd[item].format);
	memcpy(*jpeg_exif,&ifd[item].format,len);
	*jpeg_exif += len;

	*((*jpeg_exif)++) = ifd[item].length;
	*jpeg_exif +=3;

	ifd_dir_offset = *jpeg_exif - exifParamter->exif_header_tag + exif_item_num_size +  exifParamter->exif_ifd_ascii_len;
	*((*jpeg_exif)++) = ifd_dir_offset;
	*jpeg_exif += 3;

	CAMLOGV(" _CCAddExifIFDPointer addr =0x%x", *jpeg_exif);
	return 0;

}

int _CCExifOffsetOfRational(IFD_S* ifd, uint16_t num_of_ifd, CCExifParamter* exifParamter,int item)
{
	uint16_t offset_ifd_rational;
	uint16_t offset_ifd_dir;
	uint16_t ifd0_size_len;
	uint16_t last_ifd_offset;
	uint16_t ifd0_item_len;

    ifd0_size_len = ifd0_length_size;
	last_ifd_offset = last_ifd_dir_size;
	ifd0_item_len = ifd0_item_size;

	if(ifd[item].format == EXIF_URATIONAL) {
		exifParamter->exif_offset_cnt++;
	}
	
	offset_ifd_dir = (num_of_ifd - 1 - item) * ifd0_item_len;
	offset_ifd_rational = (ifd0_size_len  + offset_ifd_dir + last_ifd_offset  + exifParamter->exif_ascii_len);
	if(exifParamter->exif_offset_cnt)
		exifParamter->exif_ascii_len += 0x8;
	CAMLOGV("offset_ifd_rational 0x%x exif_ifd_ascii_len 0x%x", offset_ifd_rational, exifParamter->exif_ascii_len);
	return offset_ifd_rational;
}

int _CCAddExifURational(IFD_S* ifd, CCExifParamter* exifParamter, int item, char **jpeg_exif, uint16_t num_of_ifd)
{
	uint16_t len = 0;
	uint16_t rational_offset = 0;
	uint16_t ifd_dir_offset = 0;
	uint32_t numerator = 0x00000001;
	uint32_t denominator= 0x0000000;

	denominator |= ifd[item].offset;
	len = sizeof(ifd[item].tagNO);
	memcpy(*jpeg_exif,&ifd[item].tagNO,len);
	*jpeg_exif += len;

	len = sizeof(ifd[item].format);
	memcpy(*jpeg_exif,&ifd[item].format,len);
	*jpeg_exif += len;

	len = sizeof(ifd[item].length);
	memcpy(*jpeg_exif,&ifd[item].length,len);
	*jpeg_exif += len;

	ifd_dir_offset = (*jpeg_exif - exifParamter->tiff_header_tag);
	rational_offset =_CCExifOffsetOfRational(ifd,num_of_ifd,exifParamter,item);
	CAMLOGV(" ifd_dir_offset =0x%x rational_offset 0x%x num_of_ifd=%d addr=0x%x 0x%x", ifd_dir_offset, rational_offset,num_of_ifd,*jpeg_exif,exifParamter->tiff_header_tag);
	//strcat((*jpeg_exif +rational_offset), numerator);
	*((*jpeg_exif) + rational_offset) = (numerator & 0xff);

	//strcat((*jpeg_exif +rational_offset + 0x4), denominator);
	*((*jpeg_exif + rational_offset) + 0x4) = (denominator & 0x000000ff);
	*((*jpeg_exif + rational_offset) + 0x5) = ((denominator & 0x0000ff00) >> 8);
	*((*jpeg_exif + rational_offset) + 0x6) = ((denominator & 0x00ff0000) >> 16);
	*((*jpeg_exif + rational_offset) + 0x7) = ((denominator & 0xff000000) >> 24);

	*((*jpeg_exif)++) = rational_offset +ifd_dir_offset;
	*jpeg_exif += 3;

	CAMLOGV(" _CCAddExifURational addr =0x%x", *jpeg_exif);
	return 0;

}

int _CCAddExifUShort(IFD_S* ifd, CCExifParamter* exifParamter,char item, char **jpeg_exif)
{
	uint16_t len1 = sizeof(ifd[item].tagNO);
	memcpy(*jpeg_exif,&ifd[item].tagNO,len1);
	*jpeg_exif += len1;

	len1 = sizeof(ifd[item].format);
	memcpy(*jpeg_exif,&ifd[item].format,len1);
	*jpeg_exif += len1;

	*((*jpeg_exif)++) = ifd[item].length;
	*jpeg_exif +=3;

	*((*jpeg_exif)++) = ifd[item].offset;
	*jpeg_exif +=3;
	
	CAMLOGV(" _CCAddExifUShort addr =0x%x", *jpeg_exif);
	return 0;
}

int _CCAddExifULong(IFD_S* ifd, CCExifParamter* exifParamter, int item, char **jpeg_exif)
{
	uint16_t len = 0;

	len = sizeof(ifd[item].tagNO);
	memcpy(*jpeg_exif,&ifd[item].tagNO,len);
	*jpeg_exif += len;

	len = sizeof(ifd[item].format);
	memcpy(*jpeg_exif,&ifd[item].format,len);
	*jpeg_exif += len;

	*((*jpeg_exif)++) = ifd[item].length;
	*jpeg_exif +=3;

	if( ifd[item].offset > 0xff)
	{
		*((*jpeg_exif)++) =  (char)(ifd[item].offset  & 0x00ff);
		*((*jpeg_exif)++) =  (char)((ifd[item].offset & 0xff00) >> 8 ) ; 
		*jpeg_exif += 2;
	}else{
		*((*jpeg_exif)++) = ifd[item].offset;
		*jpeg_exif +=3;
	}
	CAMLOGV(" _CCAddExifULong  offset=0x%x addr =0x%x", ifd[item].offset, *jpeg_exif);
	return 0;

}

int _CCCaptureAddExif(int file_handle)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	uint16_t len1 = 0;
	uint16_t len2 = 0;
//	uint16_t len3 = 0;
	char *jpeg_exif = NULL;
	char *jpeg_exif_mem = NULL;
	uint16_t app1_size = 0;
	uint16_t total_exif_size = 0;
	int fileWriteLen = -1;
	CCExifParamter *exifParamter = NULL;
	uint16_t index_ifd0 = 0;
	uint16_t index_ifd0_exif = 0;
	uint16_t num_of_ifd0 = 0;
	uint16_t num_of_ifd0_exif = 0;
	char *exif_offset_addr = NULL;

	num_of_ifd0 = sizeof(IFD0) / sizeof(IFD0[0]);
	num_of_ifd0_exif = sizeof(IFD0_EXIF) / sizeof(IFD0_EXIF[0]);

	exifParamter = UOS_MALLOC(sizeof(CCExifParamter));
	if (!exifParamter) {
		CAMLOGE("malloc for exifParamter failed!");
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}
	
	memset(exifParamter, 0, sizeof(CCExifParamter));

	_CCSumExifSize(num_of_ifd0, num_of_ifd0_exif, &total_exif_size);
	jpeg_exif_mem = UOS_MALLOC(total_exif_size);
	CAMLOGV("\n exif start_addr = 0x%x end_addr = 0x%x",jpeg_exif_mem,jpeg_exif_mem+total_exif_size);
	if (!jpeg_exif_mem) {
		CAMLOGE("malloc for jpeg_exif memory failed!");
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}
	memset(jpeg_exif_mem,0x00,total_exif_size);
	jpeg_exif = jpeg_exif_mem;
	app1_size = total_exif_size - 4;

//soi marker
	len1 = sizeof(exif_soi_marker) ;
	_CCAddExifFlag(exif_soi_marker,&jpeg_exif,len1);

//app1 marker
	len1 = sizeof(exif_app1_marker) ;
	_CCAddExifFlag(exif_app1_marker,&jpeg_exif,len1);

	exifParamter->exif_app1_flag_tag = jpeg_exif;

//app1 data size
	jpeg_exif ++ ;
	*(jpeg_exif ++) = app1_size;

	exifParamter->exif_header_tag = jpeg_exif;

//exif header marker
	len1 = sizeof(app1_exif_header_marker) ;
	_CCAddExifFlag(app1_exif_header_marker,&jpeg_exif,len1);

	exifParamter->tiff_header_tag =  jpeg_exif;

//tiff header marker
	len1 = sizeof(app1_tiff_header_marker) ;
	_CCAddExifFlag(app1_tiff_header_marker,&jpeg_exif,len1);

//IFD0 number
	*(jpeg_exif ++) =  (char)(num_of_ifd0  & 0x00ff);
	*(jpeg_exif ++) =  (char)((num_of_ifd0 & 0xff00) >> 8 ) ; 

	for(index_ifd0 = 0 ; index_ifd0 < num_of_ifd0; index_ifd0++){
		if(IFD0[index_ifd0].format == EXIF_ASCII)
			_CCAddIFDASCII(IFD0,exifParamter, index_ifd0, &jpeg_exif,num_of_ifd0);
		else if(IFD0[index_ifd0].format == EXIF_USHORT)
			_CCAddExifUShort(IFD0,exifParamter, index_ifd0, &jpeg_exif);
		else if(IFD0[index_ifd0].format == EXIF_ULONG)
			_CCAddExifIFDPointer(IFD0,exifParamter, index_ifd0, &jpeg_exif);
		else
		    CAMLOGW("EXIF not support format 0x%x", IFD0[index_ifd0].format);
   	}

	 CAMLOGV("IFD item end addr =0x%x", jpeg_exif);

//last item offset
	len1 =  jpeg_exif - exifParamter->exif_header_tag;
	exif_offset_addr = jpeg_exif;

	*(jpeg_exif++) = 0x0;
	jpeg_exif +=3;

#if 1
//IFD0_EXIF number
	jpeg_exif = jpeg_exif + exifParamter->exif_ifd_ascii_len;
	CAMLOGV("IFD0_EXIF start addr =0x%x  num=[0x%x 0x%x]", jpeg_exif,(num_of_ifd0_exif  & 0x00ff),((num_of_ifd0_exif & 0xff00) >> 8 ));
	*(jpeg_exif ++) =  (char)(num_of_ifd0_exif  & 0x00ff);
	*(jpeg_exif ++) =  (char)((num_of_ifd0_exif & 0xff00) >> 8 ) ;

	exifParamter->exif_para_header_tag = jpeg_exif;
	for(index_ifd0_exif= 0 ; index_ifd0_exif < num_of_ifd0_exif; index_ifd0_exif++){
		if(IFD0_EXIF[index_ifd0_exif].format == EXIF_URATIONAL)
			_CCAddExifURational(IFD0_EXIF,exifParamter, index_ifd0_exif, &jpeg_exif, num_of_ifd0_exif);
		else if(IFD0_EXIF[index_ifd0_exif].format == EXIF_USHORT)
			_CCAddExifUShort(IFD0_EXIF,exifParamter, index_ifd0_exif, &jpeg_exif);
		else if(IFD0_EXIF[index_ifd0_exif].format == EXIF_ULONG)
			_CCAddExifULong(IFD0_EXIF,exifParamter, index_ifd0_exif, &jpeg_exif);
		else
			CAMLOGW("EXIF not support format 0x%x", IFD0_EXIF[index_ifd0_exif].format);
	}
#endif
	len2 = sizeof(exif_end) ;
	_CCAddExifFlag(exif_end,&jpeg_exif,len2);

	jpeg_exif += last_ifd_data_size;

	*exif_offset_addr = len1 + exifParamter->exif_ifd_ascii_len + exifParamter->exif_ascii_len + num_of_ifd0_exif * ifd0_item_size + last_ifd_data_size;
	CAMLOGI("jpeg_exif - jpeg_head 0x%x total_exif_size 0x%x \n", jpeg_exif - exifParamter->exif_header_tag, total_exif_size);

	//CAMLOGI("jpeg_exif %p jpeg_exif_mem %p offset 0x%x\n", jpeg_exif, jpeg_exif_mem, jpeg_exif - jpeg_exif_mem);
	fileWriteLen = FS_Write(file_handle,(unsigned char*)jpeg_exif_mem, total_exif_size);
	CAMLOGI("jpeg_exif - jpeg_head end\n");
	if (fileWriteLen < 0) {
		CAMLOGE("write photo exif to file failed!");
		error = CRANE_CAMERA_ERROR_DISKFULL;
		goto Error_exit;
	}
Error_exit:
	if (exifParamter) {
		UOS_FREE(exifParamter);
		exifParamter = NULL;
	}
	if (jpeg_exif_mem) {
		UOS_FREE(jpeg_exif_mem);
		jpeg_exif_mem = NULL;
	}
	return error;
}

CCError CCSavePicture(int file_handle)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	CCCameraDevice *pCamDevice = camCoreHandle.pCamDevice;
	CCBufferQueue *pBufferQue = NULL;
	uint_8 *pJpegBuffer = NULL;
	uint_32 jepgBufLen = 0;
	uint_32 jepgHeadSize = 0;
	int fileWriteLen = -1;
	uint_32 halTimeTick1 = 0, halTimeTick2 = 0;

	uint16_t index_ifd0_exif = 0;
	uint16_t num_of_ifd0_exif = 0;
#ifdef ENABLE_ONLINE_JPEG
	OSA_STATUS osaStatus;
	uint_32 actual_flags;
#endif

	num_of_ifd0_exif = sizeof(IFD0_EXIF) / sizeof(IFD0_EXIF[0]);
	halTimeTick1 = timerCountRead(TCR_2);
	UOS_TakeMutex(camCoreHandle.CamDeviceMutex);
	if (!pCamDevice || !(pCamDevice->camStatus & CAMERA_STATUS_CAPTURE)) {
		CAMLOGE("must open camera and start capture first!");
		UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	if (pCamDevice->jpegOutIndex == 0) {
		CAMLOGE("jpegOutIndex is invalid!");
		goto Error_exit;
	}

	if (file_handle < 0) {
		CAMLOGE("the file_handle is invalid!");
		error = CRANE_CAMERA_ERROR_BADARGUMENT;
		goto Error_exit;
	}

#ifdef REUSE_CAPBUF
	pCamDevice->jpegOutIndex = 2;
#endif
#ifdef ENABLE_ONLINE_JPEG
	pCamDevice->jpegOutIndex = 1;
#endif

	if (1 == pCamDevice->jpegOutIndex)
		pBufferQue = &(pCamDevice->captureBufQue);
	else if (2 == pCamDevice->jpegOutIndex)
		pBufferQue = &(pCamDevice->offlineBufQue);

	if(pBufferQue->multiplexBufferSize == 0){
		CAMLOGE("jpeg size is invalid!");
		error = CRANE_CAMERA_ERROR_BADARGUMENT;
		goto Error_exit;
	}

	jepgBufLen = pBufferQue->multiplexBufferSize;

#ifdef ENABLE_ONLINE_JPEG
	pJpegBuffer = (uint_8 *)pBufferQue->jpeg_addr;
	jepgHeadSize = pBufferQue->unitSize;
	if(jepgBufLen > jepgHeadSize){
		CAMLOGE("online jpeg size is too small to write jpeg file failed! jepgBufLen= %d bytes head= %d bytes ",jepgBufLen,jepgHeadSize);
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}
#elif defined REUSE_CAPBUF
	pJpegBuffer = (uint_8 *)pBufferQue->jpeg_addr;
	jepgHeadSize = pCamDevice->captureBufQue.jpeg_head * pCamDevice->captureBufQue.width *3 /2;
	if(jepgBufLen > jepgHeadSize){
		CAMLOGE("jpeg head is too small to write jpeg file. jepgBufLen= %d bytes head= %d bytes ",jepgBufLen,jepgHeadSize);
		error = CRANE_CAMERA_ERROR_OUTOFMEMORY;
		goto Error_exit;
	}
#else
	pJpegBuffer = (uint_8 *)pBufferQue->bufPack[0].ispBuffer.buffer.planes[0].addr;
#endif

	CAMLOGD("jpegOutIndex = %d len= %d bytes  pJpegBuffer =0x%p ",pCamDevice->jpegOutIndex,jepgBufLen,pJpegBuffer);

	for(index_ifd0_exif = 0 ; index_ifd0_exif < num_of_ifd0_exif; index_ifd0_exif++){
		if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_EXPOSURETIME)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.exposureTime;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_EXPOSUREPRAGRAME)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.exposureMode;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_ISOSPEED)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.isoSpeed;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_METERINGMODE)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.meteringMode;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_FLASH)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.flashMode;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_IMAGEWIDTH)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.imageWidth;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_IMAGEHEIGHT)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.imageHeight;
		else if(IFD0_EXIF[index_ifd0_exif].tagNO == EXIF_WHITEBLANCE)
			IFD0_EXIF[index_ifd0_exif].offset = pCamDevice->exifParam.whiteBlance;
		else
			CAMLOGW("EXIF not support tagNO 0x%x", IFD0_EXIF[index_ifd0_exif].tagNO);
	}

	_CCCaptureAddExif(file_handle);
	CAMLOGI("ready to write,time %dms", (timerCountRead(TCR_2) - halTimeTick1) / 32);
	fileWriteLen = FS_Write(file_handle, pJpegBuffer+2, jepgBufLen-2);
	if (fileWriteLen < 0 || fileWriteLen < (jepgBufLen-2)) {
		CAMLOGE("write photo to file failed!");
		error = CRANE_CAMERA_ERROR_DISKFULL;
		goto Error_exit;
	}
	CAMLOGI("write photo to 0x%p file %d bytes,time %dms", file_handle, fileWriteLen,(timerCountRead(TCR_2) - halTimeTick1) / 32);
	//CCSaveJpegToFile((uint_32 *)pJpegBuffer,jepgBufLen);

#ifdef ENABLE_ONLINE_JPEG
	osaStatus = UOS_WaitFlag(pCamDevice->threadSyncFlagRef, CAPTURE_PREVIEW_DONE, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	if (!(osaStatus == OS_SUCCESS && (actual_flags & CAPTURE_PREVIEW_DONE))) {
		CAMLOGE("wait capture preview done failed");
	}
	CAMLOGD("online capture done");

	pCamDevice->previewBufQue.queueStatus = 0;
	pBufferQue = &(pCamDevice->previewBufQue);
	// free preview buffer
	if (pBufferQue) {
		if (pBufferQue->totalSize > 0) {
			freeBuffers(pBufferQue);
		}
	}
#endif

Error_exit:
	_CCPostCapture();
	pCamDevice->camStatus &= (~CAMERA_STATUS_CAPTURE);
	_CCStartPreview(&pCamDevice->previewStartInfo, false);

	UOS_ReleaseMutex(camCoreHandle.CamDeviceMutex);
	halTimeTick2 = timerCountRead(TCR_2);
	CAMLOGI("CCSavePicture cost %dms !", (halTimeTick2 - halTimeTick1) / 32);

	return error;
}

