////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2018-2023, ASR Microelectronice, Inc.
//                       All Rights Reserved
////////////////////////////////////////////////////////////////////////////////
//#define LOG_NDEBUG 0

#define LOG_TAG "MciVideoInterface"

#include "mci.h"
#include "utils/Log.h"
#include "utils/MediaDefs.h"
#include "AMediaPlayer.h"

class PlayerListener;

/******************************************************************************
 * Static variables
 ******************************************************************************/
static AMediaPlayer *g_VideoPlayer = 0;
static PlayerListener *g_PlayerListener = 0;

/******************************************************************************
 * listen to play event
 ******************************************************************************/
class PlayerListener : public AMediaPlayerListener
{
public:
	typedef void (*PlayFinishedCallback) (int32_t);
	typedef void (*DrawPanelCallback) (uint32_t);

    PlayerListener(PlayFinishedCallback finishedCallback, DrawPanelCallback drawCallback) {
		mPlayFinishedCallback = finishedCallback;
		mDrawPanelCallback = drawCallback;
	}
	
    virtual int onEvent(AMediaPlayerListener::EventType event, void * data = NULL) {
		int param;
		ALOGV("onEvent");
        switch(event)
        {
        case AMediaPlayerListener::MEDIAPLAYER_EOS:
            if (mPlayFinishedCallback)
            {
                mPlayFinishedCallback(MCI_ERR_END_OF_FILE);
            }
            ALOGI("EOS");
            break;
        case AMediaPlayerListener::MEDIAPLAYER_ERROR:
            if (mPlayFinishedCallback)
            {
                mPlayFinishedCallback(MCI_ERR_ERROR);
            }
            ALOGI("Error");
            break;
		//JHH added for #32745 callback begin
		case AMediaPlayerListener::MEDIAPLAYER_TIMER:
			param = (data == NULL) ? 0 : *(int *)data;
            if (mDrawPanelCallback)
            {
                mDrawPanelCallback(param);
            }
			break;
		//JHH added for #32745 callback  end
        default:
            break;
        }
        return 0;
	}

private:
	PlayFinishedCallback mPlayFinishedCallback;
	DrawPanelCallback mDrawPanelCallback;
};

/******************************************************************************
 * MCI interface implement
 ******************************************************************************/
UINT32 MCI_VideoClose(VOID)
{
	ALOGI("MCI_VideoClose");
	if (g_VideoPlayer)
	{
		g_VideoPlayer->stop();
		delete g_VideoPlayer;
		g_VideoPlayer = 0;
	}

	if (g_PlayerListener)
	{
		delete g_PlayerListener;
		g_PlayerListener = 0;
	}
	
	return MCI_ERR_NO;
}

UINT32 MCI_VideoOpenFile(INT32 OutputPath, uint8 open_audio, HANDLE fhd, UINT8 file_mode, UINT32 data_Len, mci_type_enum type, void (*vid_play_finish_callback) (int32), void (*vid_draw_panel_callback) (uint32))	//MCI_VID_OPEN_FILE_REQ
{
	ALOGI("MCI_VideoOpenFile");
    int err = MCI_ERR_NO;

	if (g_VideoPlayer)
	{
		return MCI_ERR_BUSY;
	}

	g_VideoPlayer = new AMediaPlayer();
    g_PlayerListener = new PlayerListener(vid_play_finish_callback, vid_draw_panel_callback);
	g_VideoPlayer->setListener(g_PlayerListener);

	if (open_audio == 0)
	{
		ALOGI("Audio track disable");
		g_VideoPlayer->disableAudioTrack();
	}
	else
	{
		g_VideoPlayer->setAudioSinkPort(OutputPath);
	}
	
	if (file_mode == 1)
	{
		ALOGV("buffer mode");
		err = g_VideoPlayer->setDataSource((uint8_t*)file_mode, (size_t)data_Len);
	}
	else
	{		
		err = g_VideoPlayer->setDataSourceFd((int)fhd);
	}

	if (err)
	{
		err = MCI_ERR_INVALID_PARAMETER;
		delete g_VideoPlayer;
		g_VideoPlayer = 0;
		if (g_PlayerListener)
    	{
    		delete g_PlayerListener;
    		g_PlayerListener = 0;
    	}
	}
	//JHH added for #33384 begin
	if (err == MCI_ERR_NO) {
		INT32 zTime = AMediaPlayerListener::MEDIAPLAYER_STATE_DATA_SRC_SET * 1000;
		g_PlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_TIMER, &zTime);	
	}
	//JHH added for #33384 end

	return err;
}

UINT32 MCI_VideoPlayFinishInd(mci_vid_play_finish_ind_struct * plyFinish)
{
	ALOGI("MCI_VideoPlayFinishInd");
	return MCI_ERR_NO;
}

//JHH added for #32745 callback begin
INT32 MCI_VideoPlayTimerTrigger()
{
	if (g_VideoPlayer == 0) {
		return 0;
	}
	UINT32 curTime = MCI_VideoGetPlayTime();
	g_PlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_TIMER, &curTime);
	
	return 1;
} 
//JHH added for #32745 callback end

UINT32 MCI_VideoGetPlayTime(VOID)
{
	ALOGV("MCI_VideoGetPlayTime");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}
	
	int32_t currentTimeMs = 0;
	g_VideoPlayer->getCurrentPosition(&currentTimeMs);
	return currentTimeMs;
}

UINT32 MCI_VideoSeekDoneInd(mci_vid_seek_done_ind_struct * pSeekDone)
{
	ALOGI("MCI_VideoSeekDoneInd");
	return MCI_ERR_NO;
}

VOID MCI_VideoOpenFileInd(mci_vid_file_ready_ind_struct * vdoOpen)
{
	ALOGI("MCI_VideoOpenFileInd");
}

UINT32 MCI_VideoRestore(UINT16 startX, UINT16 startY, UINT32 RestorePositionTimeMs, BOOL paused)
{
	ALOGI("MCI_VideoRestore: time: %d, (%d)",RestorePositionTimeMs, paused);
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}
	//JHH added for #33384 begin
	INT32 zTime = AMediaPlayerListener::MEDIAPLAYER_STATE_BEGIN_PLAY * 1000;
	g_PlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_TIMER, &zTime);
	//JHH added for #33384 end
	
	VideoRenderParameters renderParameters;
	g_VideoPlayer->getVideoRenderParameters(&renderParameters);
	renderParameters.lcdStartX = startX;
	renderParameters.lcdStartY = startY;
	if (g_VideoPlayer->configVideoRenderParameters(&renderParameters))
	{
		return MCI_ERR_INVALID_PARAMETER;
	}

	int32_t seekTimeMs = RestorePositionTimeMs;

    g_VideoPlayer->seekTo(&seekTimeMs);

	if (g_VideoPlayer->start(paused))
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}



UINT32 MCI_VideoPlay(UINT16 startX, UINT16 startY)
{
	ALOGI("MCI_VideoPlay");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}
	//JHH added for #33384 begin
	INT32 zTime = AMediaPlayerListener::MEDIAPLAYER_STATE_BEGIN_PLAY * 1000;
	g_PlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_TIMER, &zTime);
	//JHH added for #33384 end
	
	VideoRenderParameters renderParameters;
	g_VideoPlayer->getVideoRenderParameters(&renderParameters);
	renderParameters.lcdStartX = startX;
	renderParameters.lcdStartY = startY;
	if (g_VideoPlayer->configVideoRenderParameters(&renderParameters))
	{
		return MCI_ERR_INVALID_PARAMETER;
	}

	if (g_VideoPlayer->start())
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

#if defined(ENABLE_A2DP_AUDIOSINK)

UINT32 MCI_VideoPlayWithBtConnected(UINT16 startX, UINT16 startY)
{
	ALOGI("MCI_VideoPlayWithBtConnected");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}

	INT32 zTime = AMediaPlayerListener::MEDIAPLAYER_STATE_BEGIN_PLAY * 1000;
	g_PlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_TIMER, &zTime);

	VideoRenderParameters renderParameters;
	g_VideoPlayer->getVideoRenderParameters(&renderParameters);
	renderParameters.lcdStartX = startX;
	renderParameters.lcdStartY = startY;
	if (g_VideoPlayer->configVideoRenderParameters(&renderParameters))
	{
		return MCI_ERR_INVALID_PARAMETER;
	}

	g_VideoPlayer->setUsingA2dpSink(true);

	if (g_VideoPlayer->start())
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

#endif

VOID MCI_VideoPlayInd(VOID)
{
	ALOGI("MCI_VideoPlayInd");
}

UINT32 MCI_VideoStop(VOID)
{
	ALOGI("MCI_VideoStop");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}

	if (g_VideoPlayer->stop())
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

UINT32 MCI_VideoPause(VOID)
{
	ALOGI("MCI_VideoPause");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}

	if (g_VideoPlayer->pause())
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

UINT32 MCI_VideoResume(VOID)
{
	ALOGI("MCI_VideoResume");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}

	if (g_VideoPlayer->resume())
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

UINT32 MCI_VideoSeek(long long playtime, INT32 time_mode, INT16 startX, INT16 startY)
{
	ALOGI("MCI_VideoSeek");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}

	VideoRenderParameters renderParameters;
	g_VideoPlayer->getVideoRenderParameters(&renderParameters);
	renderParameters.lcdStartX = startX;
	renderParameters.lcdStartY = startY;
	if(g_VideoPlayer->configVideoRenderParameters(&renderParameters))
	{
		return MCI_ERR_INVALID_PARAMETER;
	}

	int32_t seekTimeMs = (int32_t)playtime;
	if (time_mode == 0)
	{
		g_VideoPlayer->seekTo(&seekTimeMs);
	}
	else if (time_mode == 1)
	{
		int32_t currentTimeMs = 0;
		g_VideoPlayer->getCurrentPosition(&currentTimeMs);
		currentTimeMs += seekTimeMs;
		g_VideoPlayer->seekTo(&currentTimeMs);
	}
	else
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

VOID MCI_VideoSeekInd(VOID)
{
	ALOGI("MCI_VideoSeekInd");
	return;
}

UINT32 MCI_VideoSet(UINT16 ZoomWidth, UINT16 ZoomHeight, INT16 startX, INT16 startY, INT16 cutX, INT16 cutY, INT16 cutW, INT16 cutH, UINT16 Rotate)
{
	ALOGI("MCI_VideoSet");
	if (g_VideoPlayer == 0)
	{
		return MCI_ERR_ERROR;
	}

	ALOGI("zoomWidth %u, ZoomHeight %u, startX %d, startY %d, cutX %d, cutY %d, cutW %d, cutH %d, rotate %u", 
		ZoomWidth, ZoomHeight, startX, startY, cutX, cutY, cutW, cutH, Rotate);
	
	VideoRenderParameters renderParameters;
	g_VideoPlayer->getVideoRenderParameters(&renderParameters);	
	renderParameters.displayWidth = ZoomWidth;
	renderParameters.displayHeight = ZoomHeight;
	renderParameters.lcdStartX= startX;
	renderParameters.lcdStartY= startY;
	renderParameters.lcdEndX = cutX;
	renderParameters.lcdEndY = cutY;
	renderParameters.lcdCutW = cutW;
	renderParameters.lcdCutH = cutH;
	renderParameters.lcdRotate = (rotate_t)Rotate;
	if(g_VideoPlayer->configVideoRenderParameters(&renderParameters))
	{
		return MCI_ERR_INVALID_PARAMETER;
	}

	return MCI_ERR_NO;
}

VOID  MCI_VideoGetInfo( UINT16  *image_width, UINT16  *image_height, UINT32  *total_time, UINT16  *aud_channel, UINT16 *aud_sample_rate, UINT16  *track)
{
	ALOGV("MCI_VideoGetInfo");
	if (g_VideoPlayer == 0)
	{
		return;
	}

	if (image_width != NULL)
	{
		*image_width = 0;
	}
	if (image_height != NULL)
	{
		*image_height = 0;
	}
	if (total_time != NULL)
	{
		*total_time = 0;
	}
	if (aud_channel != NULL)
	{
		*aud_channel = 0;
	}
	if (aud_sample_rate != NULL)
	{
		*aud_sample_rate = 0;
	}
	if (track != NULL)
	{
		*track = MEDIA_TRACK_NONE;
	}
	
	g_VideoPlayer->getTrackInfo(image_width, image_height, total_time, aud_channel, aud_sample_rate, track);
}

