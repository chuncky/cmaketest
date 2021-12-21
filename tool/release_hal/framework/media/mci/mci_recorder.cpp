//#define LOG_NDEBUG 0

#define LOG_TAG "MciRecorderInterface"

#include "mci.h"
#include "cpu_share.h"
#include "AMediaRecorder.h"
#include "utils/OSA_Camera.h"
#include "utils/Log.h"

class MCIMediaRecorderListener;

static AMediaRecorder *g_AudioRecorder = 0;
static AMediaRecorder *g_VideoRecorder = 0;

static MCIMediaRecorderListener *g_AudioRecorderListener = NULL;
static MCIMediaRecorderListener *g_VideoRecorderListener = NULL;


/******************************************************************************
 * listen to play event
 ******************************************************************************/
class MCIMediaRecorderListener : public AMediaRecorderListener
{
public:
    MCIMediaRecorderListener(MCI_FILE_RECORD_CALLBACK_T callback, MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T data_cb = NULL) {
		mMCICallback = callback;
		mDataCb = data_cb;
	}

    virtual int onEvent(AMediaRecorderListener::EventType event, int data1 = 0, int data2=0) {
        switch(event)
        {
        case AMediaRecorderListener::MEDIARECORDER_EVENT_EOS:
            ALOGI("EOS");
            if (mMCICallback)
            {
    			mMCICallback(MCI_EVENT_EOS, MCI_INFO_NONE, 0);
			}
            break;
        case AMediaRecorderListener::MEDIARECORDER_EVENT_ERROR:
            ALOGE("Media Error: %d", data1);

            if (mMCICallback)
            {
                MCI_ERR_T mci_err = MCI_ERR_ERROR;

				if (data1 == MEDIA_ERROR_IO)
				{
				    mci_err = MCI_ERR_OUT_OF_MEMORY;
				    ALOGE("Out of memory !!");
				}
    			mMCICallback(MCI_EVENT_ERROR, MCI_INFO_ERROR, mci_err);
			}
            break;
        case AMediaRecorderListener::MEDIARECORDER_EVENT_INFO:
            if (data1 == AMediaRecorderListener::MEDIARECORDER_INFO_CURRENT_DURATION_MS)
            {
				if (mMCICallback)
				{
					mMCICallback(MCI_EVENT_INFO, MCI_INFO_RECORDER_CUR_DURATION_MS, data2);
				}
				ALOGV("Info: Current duration: %d ms.", data2);
			}
			else if (data1 == AMediaRecorderListener::MEDIARECORDER_INFO_CURRENT_SIZE)
			{
				if (mMCICallback)
				{
					mMCICallback(MCI_EVENT_INFO, MCI_INFO_RECORDER_CUR_SIZE, data2);
				}
				ALOGV("Info: Current file size: %d.", data2);
			}
            break;
        case AMediaRecorderListener::MEDIARECORDER_EVENT_DATA:
            if (mDataCb)
            {
                unsigned int rec_len=data2;
                mDataCb((unsigned char*)data1, data2, &rec_len);
            }
            break;
        default:
            break;
        }
        return 0;
	}

	void setAudioPlayCallback(MCI_AUDIO_FILE_RECORD_CALLBACK_T callback) {
		mMCICallback = callback;
	}
	~MCIMediaRecorderListener() {}

private:
	MCI_FILE_RECORD_CALLBACK_T mMCICallback;
	MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T mDataCb;
};



// Audio Record to buffer
UINT32 MCI_AudioBufferRecordStart(UINT8 *pBuf, UINT32 bufSize, UINT32 maxDurationMs, mci_type_enum format, U8 quality, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback, MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback)
{
	ALOGI("MCI_AudioBufferRecordStart,callback=%p,usercallback=%p,format=%d",callback, usercallback, format);
	
    if (g_AudioRecorder != 0)
    {
       return MCI_ERR_ACTION_NOT_ALLOWED; 
    }
    AudioParameters audio_param = {MEDIA_TYPE_UNDEFINED, 0, 0, 0};
    MediaContainerFormat container = MEDIA_CONTAINER_UNDEFINED;
	g_AudioRecorder = new AMediaRecorder();
    if (callback != 0 || usercallback != NULL)
    {
        g_AudioRecorderListener = new MCIMediaRecorderListener(callback, usercallback);
		g_AudioRecorder->setListener(g_AudioRecorderListener);
    }
    g_AudioRecorder->setAudioSource(AUDIO_SOURCE_MIC);

    switch(format)
    {
        case MCI_TYPE_AMR:
        {
            audio_param.eType = MEDIA_TYPE_AUDIO_AMRNB;
            audio_param.nChannels = 1;
            audio_param.nSamplingRate = 8000;
            container = MEDIA_CONTAINER_AMR;
            break;
         }
         case MCI_TYPE_AMR_WB:
         {
             audio_param.eType = MEDIA_TYPE_AUDIO_AMRWB;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 16000;
             container = MEDIA_CONTAINER_AMR;
             break;
         }
         case MCI_TYPE_PCM_8K:
         case MCI_TYPE_PCM8_8K:
         {
			  audio_param.eType = MEDIA_TYPE_AUDIO_PCM;
			  if (MCI_TYPE_PCM8_8K == format)
			  {
			      audio_param.eType = MEDIA_TYPE_AUDIO_PCM8;
			  }
			  audio_param.nChannels = 1;
			  audio_param.nSamplingRate = 8000;
			  container = MEDIA_CONTAINER_WAV;
			  break;
		 }
         case MCI_TYPE_PCM_16K:
         {
             audio_param.eType = MEDIA_TYPE_AUDIO_PCM;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 16000;
			 container = MEDIA_CONTAINER_WAV;
             break;
         }
#ifdef ENABLE_MP3_ENCODER
         case MCI_TYPE_MP3:
         {
             audio_param.eType = MEDIA_TYPE_AUDIO_MP3;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 48000;
             container = MEDIA_CONTAINER_MP3;
             break;
         }
#endif
         default:
		 	 ALOGE("unsupport mci audio format-%d", format);
             return MCI_ERR_INVALID_PARAMETER;
    }

    g_AudioRecorder->setOutputBuffer(pBuf, bufSize);
    g_AudioRecorder->setMaxFileDuration(maxDurationMs);
    g_AudioRecorder->setMediaFileFormat(container);
    g_AudioRecorder->setAudioFormat(&audio_param);
    if (usercallback != NULL && maxDurationMs == -1)
    {
        ALOGI("using loop mode");
        g_AudioRecorder->setBufferRecordLoop(true);
    }
    if (0 != g_AudioRecorder->start())
    {
        return MCI_ERR_ERROR;
    }
    return MCI_ERR_NO;
}



UINT32 MCI_AudioBufferRecordStop(UINT32 *recSize, UINT32 *durationMs)
{
	ALOGI("MCI_AudioBufferRecordStop");

    if (g_AudioRecorder != 0)
    {
       g_AudioRecorder->stop();
       g_AudioRecorder->getSize(recSize);
       g_AudioRecorder->getDurationMs(durationMs);
       delete g_AudioRecorder;
       g_AudioRecorder = 0;
    }
    if (g_AudioRecorderListener != 0)
    {
        delete g_AudioRecorderListener;
        g_AudioRecorderListener = NULL;
    }
	return MCI_ERR_NO;
}

// Audio Record
UINT32 MCI_AudioRecordStart(HANDLE fhd, mci_type_enum format, U8 quality, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback, MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback)
{
	ALOGI("MCI_AudioRecordStart,callback=%p,usercallback=%p,format=%d",callback, usercallback, format);
    if (g_AudioRecorder != 0)
    {
       return MCI_ERR_ACTION_NOT_ALLOWED; 
    }
    AudioParameters audio_param = {MEDIA_TYPE_UNDEFINED, 0, 0, 0};
    MediaContainerFormat container = MEDIA_CONTAINER_UNDEFINED;
	g_AudioRecorder = new AMediaRecorder();
    if (callback != 0)
    {
        g_AudioRecorderListener = new MCIMediaRecorderListener(callback, NULL);
		g_AudioRecorder->setListener(g_AudioRecorderListener);
    }
    g_AudioRecorder->setAudioSource(AUDIO_SOURCE_MIC);

    switch(format)
    {
        case MCI_TYPE_AMR:
        {
            audio_param.eType = MEDIA_TYPE_AUDIO_AMRNB;
            audio_param.nChannels = 1;
            audio_param.nSamplingRate = 8000;
            container = MEDIA_CONTAINER_AMR;
            break;
         }
         case MCI_TYPE_AMR_WB:
         {
             audio_param.eType = MEDIA_TYPE_AUDIO_AMRWB;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 16000;
             container = MEDIA_CONTAINER_AMR;
             break;
         }
         case MCI_TYPE_PCM_8K:
         {
			  audio_param.eType = MEDIA_TYPE_AUDIO_PCM;
			  audio_param.nChannels = 1;
			  audio_param.nSamplingRate = 8000;
			  container = MEDIA_CONTAINER_WAV;
			  break;
		 }
         case MCI_TYPE_PCM_16K:
         {
             audio_param.eType = MEDIA_TYPE_AUDIO_PCM;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 16000;
			 container = MEDIA_CONTAINER_WAV;
             break;
         }

#ifdef CONFIG_ADPCM_CODEC_SUPPORT
         case MCI_TYPE_WAV_DVI_ADPCM:
         {            
             ALOGI("use adpcm recoder-format %d", format);
             audio_param.eType = MEDIA_TYPE_AUDIO_ADPCM;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 8000;
             container = MEDIA_CONTAINER_WAV;
             break;
         }
#endif

#ifdef ENABLE_MP3_ENCODER
         case MCI_TYPE_MP3:
         {
             audio_param.eType = MEDIA_TYPE_AUDIO_MP3;
             audio_param.nChannels = 1;
             audio_param.nSamplingRate = 48000;
			 container = MEDIA_CONTAINER_MP3;
             break;
         }
#endif
         default:
		 	 ALOGE("unsupport mci audio format-%d", format);
             return MCI_ERR_INVALID_PARAMETER;
    }

    g_AudioRecorder->setOutputFd(fhd);
    g_AudioRecorder->setMediaFileFormat(container);
    g_AudioRecorder->setAudioFormat(&audio_param);

    if (0 != g_AudioRecorder->start())
    {
        return MCI_ERR_ERROR;
    }
    return MCI_ERR_NO;
}
UINT32 MCI_AudioRecordPause(VOID)
{
	ALOGI("MCI_AudioRecordPause");

	if (g_AudioRecorder == 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}
	g_AudioRecorder->pause();
	return MCI_ERR_NO;
}

UINT32 MCI_AudioRecordResume(VOID)
{
	ALOGI("MCI_AudioRecordPause");

	if (g_AudioRecorder == 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}
	g_AudioRecorder->resume();
	return MCI_ERR_NO;
}


UINT32 MCI_AudioRecordStop(VOID)
{
	ALOGI("MCI_AudioRecordStop");

    if (g_AudioRecorder != 0)
    {
       g_AudioRecorder->stop();
       delete g_AudioRecorder;
       g_AudioRecorder = 0;
    }
    if (g_AudioRecorderListener != 0)
    {
        delete g_AudioRecorderListener;
        g_AudioRecorderListener = NULL;
    }
	return MCI_ERR_NO;
}

VOID MCI_AudioRecordFinished(MCI_ERR_T result)
{
	ALOGI("MCI_AudioRecordFinished");
}

/*get record duartions ms*/
UINT32 MCI_AudioRecordGetRecordInfo(UINT32 *duration_ms)
{
    ALOGV("enter:MCI_AudioRecordGetRecordInfo");

    if (duration_ms == NULL)
    {
        ALOGE("address of duration_ms is NULL");
        return MCI_ERR_ERROR;
    } 

    *duration_ms = 0;
    if (g_AudioRecorder == NULL)
    {        
        ALOGE("record end, pls get the info before stop");
        return MCI_ERR_ERROR;
    } 

    if (g_AudioRecorder->getDurationMs(duration_ms) != MEDIA_OK)
    {
        ALOGE("get the active record file duration_ms is error");
        return MCI_ERR_ERROR;
    }
    ALOGV("MCI_AudioRecordGetRecordInfo:%d ms", *duration_ms);
    return MCI_ERR_NO;
}

UINT32 MCI_AudioRecordGetRecordsize(UINT32 *size)
{
    ALOGV("MCI_AudioRecordGetRecordsize");
    if (size == NULL)
    {
        ALOGE("address of size is NULL");
        return MCI_ERR_ERROR;
    } 

    *size = 0;
    if (g_AudioRecorder == NULL)
    {        
        ALOGE("record end, pls get the size before stop");
        return MCI_ERR_ERROR;
    } 

    if (g_AudioRecorder->getSize(size) != MEDIA_OK)
    {
        ALOGE("get the active record file size is error");
        return MCI_ERR_ERROR;
    }

    ALOGV("MCI_AudioRecordGetRecordsize:%d byte", *size);
    return MCI_ERR_NO;
}

#if !defined(DISABLE_VIDEO_RECORDER)

// Video Record
UINT32  MCI_VideoRecordAdjustSetting(INT32 adjustItem, INT32 value)
{
	ALOGI("MCI_AudioRecordPause");

	if (g_VideoRecorder == 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}

	if (mmf_camera_set_paramter(adjustItem, value))
	{
		return MCI_ERR_INVALID_PARAMETER;
	}
	
    return MCI_ERR_NO;
}
UINT32  MCI_VideoRecordPause(VOID)
{
	ALOGI("MCI_AudioRecordPause");

	if (g_VideoRecorder == 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}
	g_VideoRecorder->pause();
	return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordPreviewStart(MMC_VDOREC_SETTING_STRUCT *previewPara, VOID(*vid_rec_finish_ind)(UINT16 msg_result))
{
	ALOGI("MCI_VideoRecordPreviewStart");

	CAM_PREVIEW_STRUCT cam_preview_params;

	if (g_VideoRecorder != 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}

	cam_preview_params.start_x = previewPara->previewStartX;
	cam_preview_params.start_y =  previewPara->previewStartY;
	cam_preview_params.end_x =  previewPara->previewWidth;
	cam_preview_params.end_y =  previewPara->previewHeight;
	cam_preview_params.image_width =  previewPara->imageWidth;
	cam_preview_params.image_height =  previewPara->imageHeight;
	cam_preview_params.preview_rotate =  previewPara->lcdrotate;
	cam_preview_params.nightmode =  previewPara->nightMode;
	cam_preview_params.brightNess =  previewPara->brightnessLevel;
	cam_preview_params.specialEffect =  previewPara->specialEffect;
	cam_preview_params.whiteBlance =  previewPara->whiteBalance;
	cam_preview_params.contrast =  previewPara->contrast;
	cam_preview_params.factor =  previewPara->zoomFactor;
	if (mmf_camera_start_preview(&cam_preview_params))
	{
		return MCI_ERR_ERROR; 
	}

	VideoParameters video_param = {MEDIA_TYPE_UNDEFINED, 0, 0, 0, 0, 0, 0};
	AudioParameters audio_param = {MEDIA_TYPE_UNDEFINED, 0, 0, 0};
	MediaContainerFormat container = MEDIA_CONTAINER_UNDEFINED;
	g_VideoRecorder = new AMediaRecorder();
    g_VideoRecorderListener = new MCIMediaRecorderListener(NULL);
    g_VideoRecorder->setListener(g_VideoRecorderListener);

	container = MEDIA_CONTAINER_MP4;
	g_VideoRecorder->setMediaFileFormat(container);
	g_VideoRecorder->setVideoSource(VIDEO_SOURCE_CAMERA);

    video_param.eType = MEDIA_TYPE_VIDEO_H263;
    video_param.nFrameWidth = previewPara->imageWidth;
    video_param.nFrameHeight = previewPara->imageHeight;
    video_param.xFramerate = 20;
    video_param.nGopSize = video_param.xFramerate;
    /*by default 36:1 compression ratio */
    video_param.nBitrate = video_param.nFrameWidth*video_param.nFrameHeight*video_param.xFramerate*3/2*8/36;
	g_VideoRecorder->setVideoFormat(&video_param);

	if (previewPara->IfRecordAudio)
	{
		g_VideoRecorder->setAudioSource(AUDIO_SOURCE_MIC);
		audio_param.eType = MEDIA_TYPE_AUDIO_AMRNB;
		audio_param.nChannels = 1;
		audio_param.nSamplingRate = 8000;
	}
	else
	{
		g_VideoRecorder->setAudioSource(AUDIO_SOURCE_UNDEFINED);
	}
	g_VideoRecorder->setAudioFormat(&audio_param);

    return MCI_ERR_NO;
}
UINT32  MCI_VideoRecordPreviewStop(VOID)
{
	ALOGI("MCI_VideoRecordPreviewStop");

	if (g_VideoRecorder == 0)
	{
		return MCI_ERR_ACTION_NOT_ALLOWED;
	}

	g_VideoRecorder->stop();
	delete g_VideoRecorder;

	g_VideoRecorder = 0;

	if (mmf_camera_stop_preview())
	{
		return MCI_ERR_ERROR; 
	}

    return MCI_ERR_NO;
}
UINT32  MCI_VideoRecordResume(VOID)
{
	ALOGI("MCI_VideoRecordResume");

	if (g_VideoRecorder == 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}
    g_VideoRecorder->resume();
    return MCI_ERR_NO;
}
UINT32 MCI_VideoRecordStart2(HANDLE filehandle, MCI_FILE_RECORD_CALLBACK_T callback)
{
	if (callback != 0 && g_VideoRecorder != NULL)
	{
        if (g_VideoRecorderListener != NULL)
        {
            g_VideoRecorder->setListener(NULL);
            delete g_VideoRecorderListener;
            g_VideoRecorderListener = NULL;
        }
        g_VideoRecorderListener = new MCIMediaRecorderListener(callback, NULL);
        g_VideoRecorder->setListener(g_VideoRecorderListener);
    }
    return MCI_VideoRecordStart(filehandle);
}
UINT32  MCI_VideoRecordStart(HANDLE filehandle)
{
	ALOGI("MCI_VideoRecordStart");

	if (g_VideoRecorder == 0)
	{
	   return MCI_ERR_ACTION_NOT_ALLOWED; 
	}

	g_VideoRecorder->setOutputFd(filehandle);

	if (0 != g_VideoRecorder->start())
	{
		return MCI_ERR_ERROR;
	}
	return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordStop(VOID)
{
	ALOGI("MCI_VideoRecordStop");

	if (g_VideoRecorder != 0)
	{
	   g_VideoRecorder->stop();
	   delete g_VideoRecorder;
	   g_VideoRecorder = 0;
	}
	if (g_VideoRecorderListener !=0)
	{
	    delete g_VideoRecorderListener;
	    g_VideoRecorderListener = 0;
	}
	return MCI_ERR_NO;
}

UINT32 MCI_VideoRecordFinishInd(INT32 finishCause)
{
	ALOGI("MCI_VideoRecordFinishInd");

	return MCI_ERR_NO;
}

#else

// Video dummy Record
UINT32  MCI_VideoRecordAdjustSetting(INT32 adjustItem, INT32 value)
{
    return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordPause(VOID)
{
    return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordPreviewStart(MMC_VDOREC_SETTING_STRUCT *previewPara, VOID(*vid_rec_finish_ind)(UINT16 msg_result))
{
    return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordPreviewStop(VOID)
{
    return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordResume(VOID)
{
    return MCI_ERR_NO;
}

UINT32 MCI_VideoRecordStart2(HANDLE filehandle, MCI_FILE_RECORD_CALLBACK_T callback)
{
    return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordStart(HANDLE filehandle)
{
    return MCI_ERR_NO;
}

UINT32  MCI_VideoRecordStop(VOID)
{
    return MCI_ERR_NO;
}

UINT32 MCI_VideoRecordFinishInd(INT32 finishCause)
{
    return MCI_ERR_NO;
}

#endif

UINT32  MCI_DisplayVideoInterface (VOID)
{
	ALOGI("MCI_DisplayVideoInterface");

    return MCI_ERR_NO;
}
