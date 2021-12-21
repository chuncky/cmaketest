
#include "camera_hal_drv_ops.h"

CCError setCamDrvParamter(CamDrvParaMode paraCode, int32 value, CCIspPipeSwitch *pipeSwitch)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;
	uint_8 pipeIdMask = 0;

    if (pipeSwitch->snapshotSwitch) {
        pipeIdMask |= 0x4;
    } else {
        if(pipeSwitch->previewSwitch)
            pipeIdMask |= 0x1;

        if (pipeSwitch->videoSwitch)
            pipeIdMask |= 0x2;
    }

	ret = camdrv_set_param(paraCode, (void*)&value, pipeIdMask);
	if (ret < 0)
		error = CRANE_CAMERA_ERROR_INVALIDOPERATION;

	return error;
}

CCError configCamera(struct isp_sensor_cfg *sensorCfg, struct isp_pipe_cfg *pipe_cfg)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

    if (sensorCfg) {
        ret = camdrv_sensor_cfg(sensorCfg);	
		if (ret < 0) {
			CAMLOGE("camdrv_sensor_cfg falied!");
			return CRANE_CAMERA_ERROR_INVALIDOPERATION;
		}
	}

    if(pipe_cfg) {
        ret = camdrv_pipe_cfg(*pipe_cfg);
		if (ret < 0) {
			CAMLOGE("camdrv_pipe_cfg falied!");
			return CRANE_CAMERA_ERROR_INVALIDOPERATION;
		}
	}

    return error;
}

CCError setCameraATA(uint_32 ata_on)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

	ret = camdrv_set_ata(ata_on); 
	if (ret < 0) {
		CAMLOGE("camdrv_set_ata falied!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	return error;
}

CCError setCameraFPS(uint_32 min_fps ,uint_32 max_fps, CAM_BANDING banding)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

	ret = camdrv_set_fps(min_fps, max_fps, banding); 
	if (ret < 0) {
		CAMLOGE("camdrv_set_fps falied!");
		return CRANE_CAMERA_ERROR_INVALIDOPERATION;
	}

	return error;
}

void getCameraEXPandISO(uint_32 *reciprocal_exp, uint_32 *iso)
{
	camdrv_get_exp_iso(reciprocal_exp, iso); 
}

CCError requestBuffers(CCBufferQueue *pBufferQue, uint_8 bufCnt, uint_8 pipe_id, uint_8 msgQRef)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0, i = 0;

    struct isp_buf_request bufRequest;
    bufRequest.msgQRef = msgQRef;
    bufRequest.num_buf = bufCnt;
    bufRequest.pipe_id = pipe_id;
    ret = camdrv_init_bufq(&bufRequest);
	if (ret < 0)
		error = CRANE_CAMERA_ERROR_INVALIDOPERATION;
	else {
		for (i = 0; i < bufCnt; i++) {
			pBufferQue->bufPack[i].bufQueRef = 1;
		}
	}
    return error;
}

CCError fillIspOnlineBuffer(CCBufferQueue *pBufferQue)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
    uint_8 bufCnt = 0, i = 0;
    struct isp_qbuf_buffer *ispBuffer = NULL;
    uint_8 *pAddr = NULL;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    bufCnt = pBufferQue->bufCnt;
    switch (pBufferQue->bufFormat) {
        case PIPE_OUT_FMT_YUV420_NV12:
            for (i = 0; i < bufCnt; i++) {
                ispBuffer = &(pBufferQue->bufPack[i].ispBuffer.buffer);
                ispBuffer->num_planes = 2;
#ifdef CAMERA_SELF_MEMORY
				pAddr = (uint_8*)(pBufferQue->bufPack[i].addr);
				CAMLOGV("the bufPack addr=0x%x,src=0x%x!", pAddr, pBufferQue->bufPack[i].addr);
#else
				pAddr = (uint_8*)pBufferQue->addr + (i * pBufferQue->unitSize);
#endif		
                ispBuffer->planes[0].addr = (uint_32)pAddr;
				CAMLOGV("the 0 plane addr=0x%x!", ispBuffer->planes[0].addr);
                ispBuffer->planes[0].length = pBufferQue->width * pBufferQue->height;
                ispBuffer->planes[0].offset = 0;
                ispBuffer->planes[1].addr = (uint_32)(pAddr + ispBuffer->planes[0].length);
				CAMLOGV("the 1 plane addr=0x%x!", ispBuffer->planes[1].addr);
                ispBuffer->planes[1].length = pBufferQue->width * pBufferQue->height / 2;
                ispBuffer->planes[1].offset = ispBuffer->planes[0].length;
            }
            break;
		case PIPE_OUT_FMT_JPEG:
            for (i = 0; i < bufCnt; i++) {
                ispBuffer = &(pBufferQue->bufPack[i].ispBuffer.buffer);
                ispBuffer->num_planes = 1;
#ifdef CAMERA_SELF_MEMORY
				pAddr = (uint_8*)(pBufferQue->bufPack[i].addr);
				CAMLOGV("the bufPack addr=0x%x,src=0x%x!", pAddr, pBufferQue->bufPack[i].addr);
#else
				pAddr = (uint_8*)pBufferQue->addr + (i * pBufferQue->unitSize);
#endif		
                ispBuffer->planes[0].addr = (uint_32)pAddr;
				CAMLOGV("the 0 plane addr=0x%x!", ispBuffer->planes[0].addr);
                ispBuffer->planes[0].length = pBufferQue->unitSize;
                ispBuffer->planes[0].offset = 0;
            }
            break;
        default:
            CAMLOGE("unsupported format 0x%x!", pBufferQue->bufFormat);
            return CRANE_CAMERA_ERROR_BADARGUMENT;
    }

    return error;
}

CCError queueBufIndexAlloc(CCBufferQueue *pBufferQue, int_32 *bufIndex, CCBufferStatus bufStatus)
{
    uint_32 bufCnt = 0;
    uint_32 i = 0;

    if (!pBufferQue || !bufIndex)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    bufCnt = pBufferQue->bufCnt;
	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    for (i = 0; i < bufCnt; i++) {
        if (CC_BUFFER_STATUS_IDLE == pBufferQue->bufPack[i].bufStatus) {
            pBufferQue->bufPack[i].bufStatus = bufStatus;
            *bufIndex = i;
            pBufferQue->bufPack[i].index = i;
			UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
            return CRANE_CAMERA_ERROR_NONE;
        }
    }

	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
    return CRANE_CAMERA_ERROR_NOTFOUND;
}

//means buffer have data
CCError queueBufIndexFree(CCBufferQueue *pBufferQue, uint_32 bufIndex)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    pBufferQue->bufPack[bufIndex].bufStatus = CC_BUFFER_STATUS_DATA_READY;
	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
    return error;
}

CCError setBufferStatus(CCBufferQueue *pBufferQue, uint_32 bufIndex, CCBufferStatus bufStatus)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

	if ( bufStatus >= CC_BUFFER_STATUS_MAX)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    pBufferQue->bufPack[bufIndex].bufStatus = bufStatus;
	pBufferQue->bufPack[bufIndex].index = bufIndex;
	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
    return error;
}

CCError appendBufferStatus(CCBufferQueue *pBufferQue, uint_32 bufIndex, CCBufferStatus bufStatus)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

	if ( bufStatus >= CC_BUFFER_STATUS_MAX)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    pBufferQue->bufPack[bufIndex].bufStatus |= bufStatus;
	pBufferQue->bufPack[bufIndex].index = bufIndex;
	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
    return error;
}

CCError clearBufferStatus(CCBufferQueue *pBufferQue, uint_32 bufIndex, CCBufferStatus bufStatus)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

	if ( bufStatus >= CC_BUFFER_STATUS_MAX)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    pBufferQue->bufPack[bufIndex].bufStatus &= ~bufStatus;
	pBufferQue->bufPack[bufIndex].index = bufIndex;
	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
    return error;
}

int _addHoldBufferRef(CCBufferQueue *pBufferQue, uint_32 bufIndex)
{
	int ret = 0;

    if (!pBufferQue)
        return -1;

	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    pBufferQue->bufPack[bufIndex].bufQueRef++;
	ret = pBufferQue->bufPack[bufIndex].bufQueRef;
//	CAMLOGD("add ref(%d) %d buffer!", ret, bufIndex);
	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);
    return ret;
}

int _subHoldBufferRef(CCBufferQueue *pBufferQue, uint_32 bufIndex)
{
	int ret = 0;

    if (!pBufferQue)
        return -1;

	UOS_TakeMutex(pBufferQue->bufQueMutexRef);
    pBufferQue->bufPack[bufIndex].bufQueRef--;
	ret = pBufferQue->bufPack[bufIndex].bufQueRef;
//	CAMLOGD("sub ref(%d) %d buffer!", ret, bufIndex);
	UOS_ReleaseMutex(pBufferQue->bufQueMutexRef);

    return ret;
}

CCError enqueueBuffer(CCBufferQueue *pBufferQue, int_32 bufIndex, uint_8 pipe_id)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
    struct isp_buf_info *ispBuf = NULL;
    int_32 ret = 0;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

	if (pipe_id >= PIPELINE_ID_MAX || bufIndex < 0 || bufIndex > pBufferQue->bufCnt)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	ret = _subHoldBufferRef(pBufferQue, bufIndex);
	if (ret > 0)
		return CRANE_CAMERA_ERROR_NONE;

	if (pBufferQue->bufPack[bufIndex].bufStatus != CC_BUFFER_STATUS_IDLE) {
		CAMLOGE("the %dth buffer status(%d) is not idle!", bufIndex, pBufferQue->bufPack[bufIndex].bufStatus);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

    ispBuf = &(pBufferQue->bufPack[bufIndex].ispBuffer);
    ispBuf->pipe_id = pipe_id;
    ispBuf->buf_idx = bufIndex;

    ret= camdrv_buf_enqueue(ispBuf);
    if (ret < 0) {
        error = CRANE_CAMERA_ERROR_INVALIDOPERATION;
		_addHoldBufferRef(pBufferQue, bufIndex);
   	} else
   		setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_ONLINE);

    return error;
}

CCError dequeueBuffer(CCBufferQueue *pBufferQue, uint_32 bufIndex, uint_8 pipe_id)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
    struct isp_buf_info *ispBuf = NULL;
    int ret = 0;

    if (!pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    if (bufIndex > pBufferQue->bufCnt || pipe_id >= PIPELINE_ID_MAX)
        return CRANE_CAMERA_ERROR_BADARGUMENT;

	ret = _addHoldBufferRef(pBufferQue, bufIndex);
	if (ret > 1)
		return CRANE_CAMERA_ERROR_NONE;

	if (pBufferQue->bufPack[bufIndex].bufStatus != CC_BUFFER_STATUS_ONLINE
		&& pBufferQue->bufPack[bufIndex].bufStatus != CC_BUFFER_STATUS_OFFLINE) {
		CAMLOGE("the %dth buffer status(%d) is not in kernel!", bufIndex, pBufferQue->bufPack[bufIndex].bufStatus);
		return CRANE_CAMERA_ERROR_BADARGUMENT;
	}

    ispBuf = &(pBufferQue->bufPack[bufIndex].ispBuffer);
    ispBuf->buf_idx = bufIndex;
    ispBuf->pipe_id = pipe_id;

    ret = camdrv_buf_dequeue(ispBuf);
    if (ret < 0) {
        error = CRANE_CAMERA_ERROR_INVALIDOPERATION;
		_subHoldBufferRef(pBufferQue, bufIndex);
   	}
    else
        setBufferStatus(pBufferQue, bufIndex, CC_BUFFER_STATUS_DATA_READY);

    return error;
}

/*
 * This function contains the following steps:
 * 1.stream off preview(disable p0 port);
 * 2.stream on capture(enable p2 port).
 * Why: Because we need to reuse the preview buffers and reduce the bandwidth of ddr.
 */
CCError streamOnCaptureOffPreview(void)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	 int ret = 0;

	ret = camdrv_pipe_streamon_capture_off_preview();
    if (ret < 0)
        error = CRANE_CAMERA_ERROR_FATALERROR;

    return error;	
}

int streamOnOnlineCapture(struct isp_qbuf_buffer *buffer)
{
	 int ret = 0;

	ret = camdrv_pipe_streamon_online_capture(buffer);

    return ret;	
}

int streamOffOnlineCapture(void)
{
	 int ret = 0;

	ret = camdrv_pipe_streamoff_online_capture();

    return ret;	
}


CCError streamOnPipe(CCIspPipeSwitch *ispPipeSwitch)
{
    int ret = 0;
    uint_8 pipeIdMask = 0;
    CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!ispPipeSwitch)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    if (ispPipeSwitch->snapshotSwitch) {
        /* snapshot must streamon lonely. */
        pipeIdMask |= 0x4;
    } else {
        if(ispPipeSwitch->previewSwitch)
            pipeIdMask |= 0x1;

        if (ispPipeSwitch->videoSwitch)
            pipeIdMask |= 0x2;
    }

	CAMLOGV("ready to streamon 0x%x pipeline!", pipeIdMask);
    ret = camdrv_pipe_streamon(pipeIdMask);
    if (ret < 0)
        error = CRANE_CAMERA_ERROR_FATALERROR;

    return error;
}

CCError streamOffPipe(CCIspPipeSwitch *ispPipeSwitch, uint_32 keep_sensor)
{
    int ret = 0;
    uint_8 pipeIdMask = 0;
    CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!ispPipeSwitch)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    if (ispPipeSwitch->snapshotSwitch) {
        /* snapshot must lonely. */
        pipeIdMask |= 0x4;
    } else {
        if(ispPipeSwitch->previewSwitch)
            pipeIdMask |= 0x1;

        if (ispPipeSwitch->videoSwitch)
            pipeIdMask |= 0x2;
    }

    ret = camdrv_pipe_streamoff(pipeIdMask, keep_sensor);
    if (ret < 0)
        error = CRANE_CAMERA_ERROR_FATALERROR;

    return error;

}

CCError flushBuffer(CCBufferQueue *pBufferQue, CCIspPipeSwitch *ispPipeSwitch)
{
    int ret = 0, i;
    uint_8 pipeIdMask = 0;
    CCError error = CRANE_CAMERA_ERROR_NONE;

    if (!ispPipeSwitch || !pBufferQue)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    if (ispPipeSwitch->snapshotSwitch) {
        /* snapshot must lonely. */
        pipeIdMask |= 0x4;
    } else {
        if(ispPipeSwitch->previewSwitch)
            pipeIdMask |= 0x1;

        if (ispPipeSwitch->videoSwitch)
            pipeIdMask |= 0x2;
    }

    ret = camdrv_buf_flush(pipeIdMask);
    if (ret < 0)
        error = CRANE_CAMERA_ERROR_FATALERROR;
	else {
		for (i = 0; i < pBufferQue->bufCnt; i++) {
			if (pBufferQue->bufPack[i].bufStatus == CC_BUFFER_STATUS_ONLINE
				|| pBufferQue->bufPack[i].bufStatus == CC_BUFFER_STATUS_DATA_READY) {
				setBufferStatus(pBufferQue, i, CC_BUFFER_STATUS_IDLE);
			}
		}
	}

    return error;
}

CCError _fillIspOfflineBuffer(CCOfflineParamter *pOfflineParamter, struct cam_offline_buf *pOfflineBuf,
                                CCBufferQueue *pBufferQue, int_32 bufIndex)
{
    uint_8 i, planIndex = 0;
    CCError error = CRANE_CAMERA_ERROR_NONE;
    uint_32 dstSize = 0;

    memset(pOfflineBuf, 0, sizeof(struct cam_offline_buf));

    if (pOfflineParamter) {
        /* TODO:this case can be realization. */
        if (pOfflineParamter->format != pBufferQue->bufFormat) {
            CAMLOGE("the queue format and offline buffer format are not equal!");
            return CRANE_CAMERA_ERROR_BADARGUMENT;
        }
#if 0
        if (pOfflineParamter->width > pBufferQue->width
            || pOfflineParamter->height > pBufferQue->height) {
            CAMLOGE("the queue width and height can not meet this case!");
            return CRANE_CAMERA_ERROR_BADARGUMENT;
        }
#endif
        pOfflineBuf->num_planes = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.num_planes;
        if (pOfflineParamter->width != pBufferQue->width
            || pOfflineParamter->height != pBufferQue->height) {
            pOfflineBuf->width = pOfflineParamter->width;
            pOfflineBuf->height = pOfflineParamter->height;

            switch (pOfflineParamter->format) {
                case PIPE_OUT_FMT_YUV420_NV12:
                case PIPE_OUT_FMT_YUV420_NV21:
                    planIndex = pBufferQue->bufPack[bufIndex].planIndex;
                    dstSize = pOfflineBuf->width * pOfflineBuf->height / 2;
                    if (dstSize < (pBufferQue->width * pBufferQue->height)) {
                        pOfflineBuf->plane_addr[0] = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[planIndex].addr;
                        pOfflineBuf->plane_addr[1] = pOfflineBuf->plane_addr[0] + pOfflineBuf->width * pOfflineBuf->height;
                    } else {
                        /* TODO:we can realize this case. */
                        CAMLOGE("just support the size in a plane!", pOfflineParamter->format);
                        return CRANE_CAMERA_ERROR_BADARGUMENT;
                    }
                    break;
                default:
                    CAMLOGE("unsupport this format %d!", pOfflineParamter->format);
                    return CRANE_CAMERA_ERROR_BADARGUMENT;
            }

        } else {
            //width and height are all equal.
            pOfflineBuf->width = pBufferQue->width;
            pOfflineBuf->height = pBufferQue->height;

            for(i = 0; i < pOfflineBuf->num_planes; i++)
                pOfflineBuf->plane_addr[i] = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[i].addr;
        }

    //    pOfflineBuf->rotation = pOfflineParamter->rotation;
    } else {
        //offline input for zoomscaler.
        pOfflineBuf->width = pBufferQue->width;
        pOfflineBuf->height = pBufferQue->height;
        pOfflineBuf->num_planes = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.num_planes;
        for(i = 0; i < pOfflineBuf->num_planes; i++)
            pOfflineBuf->plane_addr[i] = pBufferQue->bufPack[bufIndex].ispBuffer.buffer.planes[i].addr;
    }

    //just asign width, it could be modify for jpeg can receive data form isp directly.
    pOfflineBuf->stride = pOfflineBuf->width;
    pOfflineBuf->pixelformat = (SCALER_INPUT_FMT)pBufferQue->bufFormat;

    return error;
}

//zoom and scale can work together.
CCError getZoomScale(CCOfflineParamter *pOfflineParamter, CCBufferQueue *inBufferQue, int_32 inBufIndex,
                    CCBufferQueue *outBufferQue, int_32 outBufIndex, xbool isReuseBuf)
{
	struct cam_offline_buf buf_src;
	struct cam_offline_buf buf_dst;
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

	if (!inBufferQue || !outBufferQue || !pOfflineParamter)
		return CRANE_CAMERA_ERROR_BADPOINTER;

	if (inBufIndex < 0 || inBufIndex > inBufferQue->bufCnt)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	if(outBufIndex < 0 || outBufIndex > outBufferQue->bufCnt)
		return CRANE_CAMERA_ERROR_BADARGUMENT;

	error = _fillIspOfflineBuffer(NULL, &buf_src, inBufferQue, inBufIndex);
	error = _fillIspOfflineBuffer(pOfflineParamter, &buf_dst, outBufferQue, outBufIndex);

	if (error != CRANE_CAMERA_ERROR_NONE) {
		CAMLOGE("fill offline output buffer failed,%d!", error);
		return error;
	}

	if(isReuseBuf){
		ret = cam_offline_homo_zoom(&buf_src,&buf_dst,pOfflineParamter->zoomLevel,0);
	}else
		ret = cam_offline_scaler_zoom(&buf_src, &buf_dst, pOfflineParamter->zoomLevel, 0);
  

	if (ret < 0)
		error = CRANE_CAMERA_ERROR_FATALERROR;

	//fill the ouput buffer for user.
	memcpy(&pOfflineParamter->offlineOutputBuf, &buf_dst, sizeof(struct cam_offline_buf));
	CAMLOGV("get scale successfull!");
	return error;
}

CCError getRotation(CCOfflineParamter *pOfflineParamter, CCBufferQueue *inBufferQue, int_32 inBufIndex,
                    CCBufferQueue *outBufferQue, int_32 outBufIndex)
{
    struct cam_offline_buf buf_src;
    struct cam_offline_buf buf_dst;
    CCError error = CRANE_CAMERA_ERROR_NONE;
    int ret = 0;

    if (!outBufferQue || !pOfflineParamter)
        return CRANE_CAMERA_ERROR_BADPOINTER;

    if ((inBufferQue && (inBufIndex < 0 || inBufIndex > inBufferQue->bufCnt))
        || ((outBufIndex < 0 || outBufIndex > outBufferQue->bufCnt)))
        return CRANE_CAMERA_ERROR_BADARGUMENT;

    if (inBufferQue)
        _fillIspOfflineBuffer(NULL, &buf_src, inBufferQue, inBufIndex);
    else
        memcpy(&buf_src, &pOfflineParamter->offlineOutputBuf, sizeof(struct cam_offline_buf));

    error = _fillIspOfflineBuffer(pOfflineParamter, &buf_dst, outBufferQue, outBufIndex);
    if (error != CRANE_CAMERA_ERROR_NONE) {
        CAMLOGE("fill offline output buffer failed,%d!", error);
        return error;
    }

    ret = cam_offline_rotation(&buf_src, &buf_dst, pOfflineParamter->rotation, 0);
    if (ret < 0)
        error = CRANE_CAMERA_ERROR_FATALERROR;

    //fill the ouput buffer for user.
    memcpy(&pOfflineParamter->offlineOutputBuf, &buf_dst, sizeof(struct cam_offline_buf));

    return error;
}

CCError doHardwareJpeg(CCBufferQueue *inBufferQue, int_32 inBufIndex, CCBufferQueue *outBufferQue, int_32 outBufIndex)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;

	return error;
}

CCError getSensorCapability(CCCoreHandle *coreHandle, int *cameraCnt, int maxCnt, CCISPSensorInfo *ispSensorInfo)
{
    int ret = 0, cntIndex = 0;
    CCError error = CRANE_CAMERA_ERROR_NONE;
//    struct isp_sensor_info *sensor_info = NULL;
 //   int i,j;

    if (!ispSensorInfo || !cameraCnt || maxCnt <= 0) {
        CAMLOGE("invalid argument!");
        return CRANE_CAMERA_ERROR_BADARGUMENT;
    }

#if 0
    for (i = SENSOR_ID_BACK; i < maxCnt; i++) {
        ispSensorInfo[i].facing = SENSOR_ID_MAX;
        for (j = 0; j < CC_MAX_SENSOR_RESOLUTION_SUPPORT; j++) {
            sensor_info = &(ispSensorInfo[i].res[j]);
            sensor_info->sensor_id = ispSensorInfo[i].facing;
            sensor_info->index = j;
            ret = camdrv_sensorinfo_enum(sensor_info);
            if (ret == -1) {
                ispSensorInfo[i].res_num = j;
                break;
            }
        }
        if (ispSensorInfo[i].res_num == 0) {
            CAMLOGE("%s: the sensor:%d is invalid!",  , i);
        } else {
            cntIndex++;
			ispSensorInfo[i].facing = i;
        }
    }
#endif
	cntIndex = 2;
	ret = camdrv_sensor_detect();
	if (ret < 0){
		error = CRANE_CAMERA_ERROR_NOTSUPPORT;
	}
	CAMLOGV("camdrv_sensor_detect end %d!", ret);
    *cameraCnt = cntIndex;
    return error;
}

void rawDumpCfg(uint_t raw_on)
{
	camdrv_raw_dump(raw_on);
}

CCError getIspRes(struct isp_output_size *info)
{
	CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

	ret = camdrv_get_isp_cfg(info);
	if (ret < 0)
		return CRANE_CAMERA_ERROR_BADPOINTER;

	return error;
}

CCError cameraDrvInit(int sensor_id)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

    if (sensor_id >= SENSOR_ID_MAX || sensor_id < 0) {
        CAMLOGE("the sensor_id:%d is invalid!", sensor_id);
        return CRANE_CAMERA_ERROR_BADARGUMENT;
    }

    ret = camdrv_isp_power_on();
	if (ret < 0) {
        CAMLOGE("camdrv_isp_power_on falied!");
        return CRANE_CAMERA_ERROR_FATALERROR;
	}

    /* sensor and isp power on. */
    ret = camdrv_sensor_power_on((SENSOR_ID)sensor_id);
	if (ret < 0) {
        CAMLOGE("camdrv_sensor_power_on falied!");
		camdrv_isp_power_off();
        return CRANE_CAMERA_ERROR_FATALERROR;
	}

    return error;
}

CCError cameraDrvDeinit(int sensor_id)
{
    CCError error = CRANE_CAMERA_ERROR_NONE;
	int ret = 0;

    if (sensor_id >= SENSOR_ID_MAX) {
        CAMLOGE("the sensor_id:%d is invalid!", sensor_id);
        return CRANE_CAMERA_ERROR_BADARGUMENT;
    }

	CAMLOGV("camdrv_sensor_power_off!");
    ret = camdrv_sensor_power_off((SENSOR_ID)sensor_id);
	if (ret < 0) {
		CAMLOGE("camdrv_sensor_power_off falied!");
	}

	//reset camera register for camdrv_isp_power_off isn't clear the all register
	ret = camdrv_camera_reg_reset();
	if (ret < 0) {
		CAMLOGE("camdrv_camera_reg_reset falied!");
	}


	CAMLOGV("camdrv_isp_power_off!");
    ret = camdrv_isp_power_off();
	if (ret < 0) {
		CAMLOGE("camdrv_isp_power_off falied!");
	}

    return error;
}

