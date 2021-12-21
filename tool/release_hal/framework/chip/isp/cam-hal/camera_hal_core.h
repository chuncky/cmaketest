#ifndef _CAM_CORE_H_
#define _CAM_CORE_H_

#include "ui_os_api.h"
#include "../online_drv/cam_hal_drv_interface.h"
#include "../offline_drv/cam_offline.h"
#include "plat_types.h"
#include "../camera_common_log.h"

/* reuse capture buffer for offline capture */
#define REUSE_CAPBUF

//#define ENABLE_ONLINE_JPEG

/* camera core use struct */
typedef struct _CCStartPreviewStruct {
    unsigned short start_x;
    unsigned short start_y;
    unsigned short end_x;
    unsigned short end_y;
    unsigned int image_width;
    unsigned int image_height;
	unsigned short factor;
}CCStartPreviewStruct;

typedef struct _CCCaputreStruct {
    uint_16 image_width;
    uint_16 image_height;
	unsigned char media_mode;
}CCCaputreStruct;

//#define CAMERA_SELF_MEMORY 1
//#define DUMP_JPEG_YUV 1
//#define DUMP_PREVIEW_YUV 1

typedef unsigned int  xbool;  /* Machine representation of a boolean */
#define false	0
#define true	1

//#define OSA_FLAG_OR_CLEAR 0x1
//#define OSA_FLAG_OR       0x2
//#define OSA_FIXED         0x3

/*****end fake*****/

#define TASK_STACK_SIZE        1024
#define CAM_BYTE_POOL_SIZE     3 * TASK_STACK_SIZE//9120

//for camera reserved memory
#define CAM_FRAME_SIZE	115200 //320x240x1.5 yuv420
#define CAM_VIDEO_FRAME_SIZE 152064 //352*288*1.5
#define CAM_CAP_FRAM_SIZE	460800 // 640*480*1.5
#define CAM_PREVIEW_MEMORY_SIZE CAM_FRAME_SIZE * 6 //4+2(display roation)
#define CAM_VIDEO_MEMORY_SIZE	CAM_VIDEO_FRAME_SIZE * 4
#define CAM_CAPTURE_MEMORY_SIZE	CAM_CAP_FRAM_SIZE * 2
#define CAM_MEM_MAX_USER_COUNT	6

#define PREVIEW_READYWORK   0x1
#define PREVIEW_STOPWORK    0x2
#define VIDEO_READYWORK     0x10
#define VIDEO_STOPWORK      0x20
#define CAPTURE_PREVIEW_DONE 0x40
#define FULLSIZE_READYWORK      0x50
#define FULLSIZE_STOPWORK      0x60

#define PREVIEW_QUEUE_FREEBUFFERS 0x4

#define CC_MAX_SENSOR_RESOLUTION_SUPPORT    3
#define CC_MAX_SENSOR_SUPPORT               2
#define PREVIEW_SNAPSHOT_W                  320
#define PREVIEW_SNAPSHOT_H                  240
#define PREVIEW_VIDEO_W                     294
#define PREVIEW_VIDEO_H                     240
#define STATS_MAX_BUFFER_PER_QUEUE          4
#define SENSOR_SETTING_W					640
#define SENSOR_SETTING_H					480

#ifdef ISP_USE_HARDWARE_TIMER
#define PREVIEW_BUFFER_NUM         	2
#else
#define PREVIEW_BUFFER_NUM         	3
#endif

#define SNAPSHOT_BUFFER_NUM         1

#define THUMBNAIL_W                         160
#define THUMBNAIL_H                         120
#define BACK_DISPLAY_W                      320
#define BACK_DISPLAY_H                      240

#define ONLINE_TIME_OUT                     24 // 120ms,1 tick means 5ms
#define TICK_PER_MS				5 // 1 tick means 5ms
#define TIMEOUT_FRAME			2

//align
#define CC_CAM_ALIGN_TO_4                   4
#define CC_CAM_ALIGN_TO_16                  16
#define DOALIGN(x, iAlign)                    (((x) + (iAlign) - 1) & (~((iAlign) - 1)))

//error define
typedef int CCError;

#define CRANE_CAMERA_ERROR_NONE             0
#define CRANE_CAMERA_ERROR_BADPOINTER       1001
#define CRANE_CAMERA_ERROR_BADARGUMENT      1002
#define CRANE_CAMERA_ERROR_INVALIDOPERATION 1003
#define CRANE_CAMERA_ERROR_OUTOFMEMORY      1004
#define CRANE_CAMERA_ERROR_DISKFULL         1005
#define CRANE_CAMERA_ERROR_NOTFOUND         1006
#define CRANE_CAMERA_ERROR_NOTSUPPORT       1007
#define CRANE_CAMERA_ERROR_TIMEOUT          1008
#define CRANE_CAMERA_ERROR_DEVICEWASOPENED  1009
#define CRANE_CAMERA_ERROR_FATALERROR       1010

typedef enum {
	CAMERA_STATUS_IDLE =0x0,
	CAMERA_STATUS_PREVIEW = 0x1,
	CAMERA_STATUS_VIDEO  =  0x2,
	CAMERA_STATUS_CAPTURE = 0x4,
	CAMERA_STATUS_FULLSIZE_STREAM = 0x8,
}CamStaus;

typedef int (*ReleaseBufferCallback)(void *, void *);
typedef struct _CCamRecordParmeters {
	unsigned recordWidth;
	unsigned recordHeight;
	unsigned recordFormat;
	unsigned recordFramerate;
	void (*setReleaseBufferCallBack)(ReleaseBufferCallback callBackHandle, void *userData);
	void (*enqueueCamRecordBuffer)(void *pBuf, int bufIndex);
	void (*flushCamRecordBuffers)(void);
} CamRecordParmeters;

typedef struct _CCMemoryUnitStatus {
	char *pUnitAddr;
	uint_32 memIndex;
	uint_32 memStatus; //0:free,1:used
}CCMemoryUnitStatus;

typedef struct _CCMemoryPool {
	char *pMemAddr;
	uint_32 unitSize;
	uint_32 userCnt;
	CCMemoryUnitStatus unitArray[CAM_MEM_MAX_USER_COUNT];
}CCMemoryPool;

typedef struct {
    uint_32 width;
    uint_32 height;
    float max_fps;
    uint_32 index;
}CCISPSensorRes;

typedef struct _CCISPSensorInfo {
    char name[32]; //sensor module name
    SENSOR_ID facing; //back, front
    int orientation; //90, 180, 0, 270
    struct isp_sensor_info res[CC_MAX_SENSOR_RESOLUTION_SUPPORT];
    uint_32 res_num;
} CCISPSensorInfo;

typedef enum {
    CC_BUFFER_STATUS_IDLE = 0x0,
    CC_BUFFER_STATUS_ONLINE = 0x01,
    CC_BUFFER_STATUS_OFFLINE = 0x02,
    CC_BUFFER_STATUS_DATA_READY = 0x4,
    CC_BUFFER_STATUS_DISPLAY = 0x8,
    CC_BUFFER_STATUS_VIDEO = 0x10,
    CC_BUFFER_STATUS_DISPLAY_VIDEO = 0x18,
    CC_BUFFER_STATUS_FULLSIZE = 0x20,
    CC_BUFFER_STATUS_MAX = 0xFF,
} CCBufferStatus;

typedef struct _CCBufferPack {
    struct isp_buf_info ispBuffer;
    CCBufferStatus bufStatus;
    int_32 index; //no user
    int_32 planIndex; //for fillofflinebuffer user
	int bufQueRef; //the number of users hold this buffer.
    void *addr;
}CCBufferPack;

typedef struct _CCBufferQueue {
    CCBufferPack bufPack[STATS_MAX_BUFFER_PER_QUEUE];
    int_32 bufPackIndex; //the index means something useful depends on the user.
    int_32 multiplexBufferSize; //this size means bufPack[bufPackIndex] contains buffer size.
    PIPE_OUT_FMT bufFormat;
	uint_32 jpeg_head; // for jpeg use
	void *jpeg_addr;
	void *addr; // cap_addr = jpeg_addr + jpeg_head * height *3/2
    uint_32 unitSize;  // unitSize = (jpeg_head + width) * height *3/2
    uint_32 width;
    uint_32 height;
    uint_32 totalSize; //totalSize = unitSize * bufCnt.
    uint_8 bufCnt;
    u8 bufQueMutexRef;
    PIPELINE_ID pipeId;
	uint_32 queueStatus; //0:not work; 1:working
	int_32 lastIndexToOthers; //the last buffer index to display or video encoder.
	u8   bufQueSyncFlagRef;
	u8 dumpBuffer;
}CCBufferQueue;

typedef struct _CCRotationBufferPack {
	uint_32 bufCnt;
	char *pAddr[2];
	uint_32 unitSize;
}CCRotationBufferPack;

typedef struct _CCExifParameters {
	uint_32 exposureTime;
	uint_32 exposureMode;
	uint_32 isoSpeed;
	uint_32 meteringMode;
	uint_32 flashMode;
	uint_32 imageWidth;
	uint_32 imageHeight;
	uint_32 whiteBlance;
}CCExifParameters;

typedef struct _CCUIParameters {
	u8 flashLevel;
	uint_32 nightMode;
	uint_32 nightModeFps;
}CCUIParameters;

typedef struct _CCCameraDevice {
	SENSOR_ID sensorId; //current open sensor id.
	u8   threadSyncFlagRef;
	u8   previewMsgQRef;
	u8   videoMsgQRef;
	u8   captureMsgQRef;
	u8   fullSizeStreamMsgQRef;
	HANDLE  previewTaskRef;
	HANDLE  videoTaskRef;
	HANDLE  fullSizeStreamTaskRef;
	CCBufferQueue previewBufQue;
	CCBufferQueue videoBufQue;
	CCBufferQueue captureBufQue;
	CCBufferQueue fullSizeStreamBufQue;
	CCBufferQueue offlineBufQue; //reuse from captureBufQue
	struct cam_offline_buf thumbOutputBuf;
	CamStaus camStatus; //0x0:invalid,0x1:preview,0x2:video,0x4:sanpshot.0x5 fullsize
	CCRotationBufferPack rotationBufferPack;
	uint_32	jpegOutIndex; //0:invalid,1:captureBufQue,2:offlineBufQue
	uint_32 captureResourceStatus; //0:virginal,1:alloced.
	void (*qBufForVideoEncoder)(void *pBuf, int bufIndex);
	void (*flushVideoEncoderBuffers)(void);
	void (*qBufForFullSizeStream)(void *pBuf, int bufIndex);
	void (*flushFullSizeStreamBuffers)(void);
	uint_16 currentZoomLevel; //for snapshot use
	CCExifParameters exifParam; //for exif use
	uint_16 previewRotationDegree;
	CCStartPreviewStruct previewStartInfo;
	uint_8 previewScale;
	uint_32 lcdWidth;
	uint_32 lcdHeight;
	uint_32 sensorWidth;
	uint_32 sensorHeight;
	uint_32 minFps;
	uint_32 maxFps;
	xbool previewVideoSamePipe; //privew and video use one stream
	u8 previewStreamPause; //stream pause flag
	u8 videoStreamPause; //stream pause flag
	u8 fullSizeStreamPause; //stream pause flag
	u8 videoOperation;	// 1:start, 2:stop, for display thread use to send msg
	u8 imageQuality;
	CCUIParameters  uiParam;
	uint_32 frameDropNum;
	xbool alreadyCfgSensor;
}CCCameraDevice;

typedef struct _CCCoreHandle {
    CCISPSensorInfo ispSensorInfo[CC_MAX_SENSOR_SUPPORT];
    CCCameraDevice *pCamDevice;
	u8 CamDeviceMutex;
}CCCoreHandle;

typedef struct _CCIspPipeSwitch {
    uint_8 previewSwitch;
    uint_8 videoSwitch;
    uint_8 snapshotSwitch;
}CCIspPipeSwitch;

typedef struct _CCOfflineParamter {
    PIPE_OUT_FMT format;
    uint_32 width;
    uint_32 height;
    uint_32 zoomLevel;
    enum offline_rot rotation;
    struct cam_offline_buf offlineOutputBuf;
}CCOfflineParamter;

typedef struct _CCOfflineOption {
    xbool thumbnail;
    PIPE_OUT_FMT thumb_format;
    uint_32 thumb_width;
    uint_32 thumb_height;
    xbool zoomScale;
    uint_32 zoomLevel;
    xbool rotation;
    enum offline_rot rotationDegree;
    xbool softJpeg;
}CCOfflineOption;

typedef enum _CCPreivewStopType {
	CC_PREVIEW_STOP_EXIT,
	CC_PREVIEW_STOP_FLUSH,
}CCPreivewStopType;


//function
/*****************************************************/
CCError CCOpenCamera(int sensor_id);
CCError CCCloseCamera(void);
CCError CCStartPreview(CCStartPreviewStruct *data);
CCError CCStopPreview(void);
CCError CCPausePreview(void);
CCError CCResumePreview(void);
CCError CCSetAlphaPreview(unsigned int value);
//CCError CCStartRecord(uint_32 width, uint_32 height, uint_32 format);
CCError CCStartRecord(CamRecordParmeters *pVideoRecordParam, uint_32 format);
CCError CCPauseRecord(void);
CCError CCResumeRecord(void);
CCError CCStopRecord(void);
#ifdef CAM_FULLSIZE_STREAM
CCError CCStartFullSizeStream(CamRecordParmeters *pVideoRecordParam, uint_32 format);
CCError CCPauseFullSizeStream(void);
CCError CCResumeFullSizeStream(void);
CCError CCStopFullSizeStream(void);
#endif
CCError CCTakePicture(CCCaputreStruct *capture_data);
CCError CCSavePicture(int file_handle);
CCError CCSetPara(CamDrvParaMode paraMode, int32 value);
CCError CCATATakePicture(int sensor_id, uint_32 ata_on, int_32 file_handle, CCCaputreStruct *capture_data);
CCError CCStartVideoCall(CamRecordParmeters *pVideoRecordParam);
CCError CCStopVideoCall(void);
void isp_set_rawdump_flag(xbool flag);
xbool isp_get_rawdump_flag(void);

#endif
