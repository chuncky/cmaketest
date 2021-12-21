#ifndef _CAM_DRV_OPS_H_
#define _CAM_DRV_OPS_H_

#include "../online_drv/cam_hal_drv_interface.h"
#include "../offline_drv/cam_offline.h"
#include "camera_hal_core.h"

//function
CCError cameraDrvInit(int sensor_id);
CCError cameraDrvDeinit(int sensor_id);
CCError getSensorCapability(CCCoreHandle *coreHandle, int *cameraCnt, int maxCnt, CCISPSensorInfo *ispSensorInfo);
CCError doHardwareJpeg(CCBufferQueue *inBufferQue, int_32 inBufIndex, CCBufferQueue *outBufferQue, int_32 outBufIndex);
CCError configCamera(struct isp_sensor_cfg *sensorCfg, struct isp_pipe_cfg *pipe_cfg);
CCError requestBuffers(CCBufferQueue *pBufferQue, uint_8 bufCnt, uint_8 pipe_id, uint_8 msgQRef);
CCError streamOnPipe(CCIspPipeSwitch *ispPipeSwitch);
CCError streamOffPipe(CCIspPipeSwitch *ispPipeSwitch, uint_32 keep_sensor);
int _setParamterToDrv(void/*struct ispparamter paramter*/);
CCError fillIspOnlineBuffer(CCBufferQueue *pBufferQue);
CCError enqueueBuffer(CCBufferQueue *pBufferQue, int_32 bufIndex, uint_8 pipe_id);
CCError dequeueBuffer(CCBufferQueue *pBufferQue, uint_32 bufIndex, uint_8 pipe_id);
CCError flushBuffer(CCBufferQueue *pBufferQue, CCIspPipeSwitch *ispPipeSwitch);
CCError queueBufIndexAlloc(CCBufferQueue *pBufferQue, int_32 *bufIndex, CCBufferStatus bufStatus);
CCError queueBufIndexFree(CCBufferQueue *pBufferQue, uint_32 bufIndex);
CCError setBufferStatus(CCBufferQueue *pBufferQue, uint_32 bufIndex, CCBufferStatus bufStatus);
CCError appendBufferStatus(CCBufferQueue *pBufferQue, uint_32 bufIndex, CCBufferStatus bufStatus);
CCError clearBufferStatus(CCBufferQueue *pBufferQue, uint_32 bufIndex, CCBufferStatus bufStatus);
CCError getZoomScale(CCOfflineParamter *pOfflineParamter, CCBufferQueue *inBufferQue, int_32 inBufIndex,
                    CCBufferQueue *outBufferQue, int_32 outBufIndex, xbool isReuseBuf);
CCError getRotation(CCOfflineParamter *pOfflineParamter, CCBufferQueue *inBufferQue, int_32 inBufIndex,
                    CCBufferQueue *outBufferQue, int_32 outBufIndex);
CCError streamOnCaptureOffPreview(void);
int streamOnOnlineCapture(struct isp_qbuf_buffer *buffer);
int streamOffOnlineCapture(void);
CCError setCamDrvParamter(CamDrvParaMode paraCode, int32 value, CCIspPipeSwitch *pipeSwitch);
void rawDumpCfg(uint_t raw_on);
CCError getIspRes(struct isp_output_size *info);
CCError setCameraATA(uint_32 ata_on);
CCError setCameraFPS(uint_32 min_fps ,uint_32 max_fps, CAM_BANDING banding);
void getCameraEXPandISO(uint_32 *reciprocal_exp, uint_32 *iso);

#endif

