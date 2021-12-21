////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2018-2023, ASR Microelectronice, Inc.
//                       All Rights Reserved
////////////////////////////////////////////////////////////////////////////////
//#define LOG_NDEBUG 0

#define LOG_TAG "MciAudioInterface"

#include "mci.h"
#include "utils/Log.h"
#include "utils/MediaDefs.h"
#include "AMediaPlayer.h"
#include "cp_include.h"
#include "audiopolicy/audiopolicymanager.h"
//#include "plat_config_defs.h"

class AudioPlayerListener;

/******************************************************************************
 * Static variables.
 ******************************************************************************/
static AMediaPlayer *gAudioPlayer = NULL;
static AudioPlayerListener *gAudioPlayerListener = NULL;
static bool gAudioPlayerIsBusy = 0;
static u8 gAudioPlayerMutex = MAX_MUTEX_SIZE;
static AudioPolicyManager* mAudioPolicy = NULL;
static AUDIOHAL_ITF_T gMciAudioPath= AUDIOHAL_ITF_NONE;

#ifdef CONFIG_MMI_SUPPORT_BLUETOOTH
static char g_btVol_table[MAX_ITEM_BTVOL]={-1, };	//range: valid:0~0x7f; -1: not init.
static char g_btVol_table_ready = 0;				//0-not init; 1-inited
#endif

static void LockAudioPlayer()
{
	if (MAX_MUTEX_SIZE != gAudioPlayerMutex)
	{
		UOS_TakeMutex(gAudioPlayerMutex);
	}
}
static void UnlockAudioPlayer()
{
	if (MAX_MUTEX_SIZE != gAudioPlayerMutex)
	{
		UOS_ReleaseMutex(gAudioPlayerMutex);
	}
}

static bool AquireAudioPlayerRes()
{
	bool isBusy = false;
	if (MAX_MUTEX_SIZE == gAudioPlayerMutex)
	{
		gAudioPlayerMutex = UOS_NewMutex("audio_player");
	}

	LockAudioPlayer();

	/*change busy flag. */
	isBusy = gAudioPlayerIsBusy;
	gAudioPlayerIsBusy = true;

	UnlockAudioPlayer();

	return !isBusy;
}
static void ReleaseAudioPlayerRes()
{
	if (MAX_MUTEX_SIZE == gAudioPlayerMutex)
	{
		gAudioPlayerMutex = UOS_NewMutex("audio_player");
	}
	LockAudioPlayer();

	/*change busy flag. */
	gAudioPlayerIsBusy = false;


	UnlockAudioPlayer();
}
/******************************************************************************
 * listen to play event
 ******************************************************************************/
class AudioPlayerListener : public AMediaPlayerListener
{
public:
	AudioPlayerListener(MCI_AUDIO_PLAY_CALLBACK_T callback) {
		mPlayCallback = callback;
	}

	virtual int onEvent(AMediaPlayerListener::EventType event, void * data = NULL) {
		ALOGI("onEvent");
		switch(event)
		{
		case AMediaPlayerListener::MEDIAPLAYER_EOS:
			mPlayCallback(MCI_ERR_END_OF_FILE);
			ALOGI("EOS");
			break;
		case AMediaPlayerListener::MEDIAPLAYER_ERROR:
			mPlayCallback(MCI_ERR_ERROR);			
			ALOGI("Error");
			break;
		default:
			break;
		}
		return 0;
	}

	void setAudioPlayCallback(MCI_AUDIO_PLAY_CALLBACK_T callback) {
		mPlayCallback = callback;
	}

private:
	MCI_AUDIO_PLAY_CALLBACK_T mPlayCallback;
};

VOID MCI_AudioFinished(MCI_ERR_T result)
{
	ALOGI("MCI_AudioFinished");
}

VOID MCI_RingFinished(MCI_ERR_T result)
{
	ALOGI("MCI_RingFinished");
}

UINT32 MCI_AudioPlayTone(MCI_AUDIO_TONE_TYPE_T tone, MCI_AUDIO_TONE_ATTENUATION_T attenuation)
{
	int err;

	ALOGI("MCI_AudioPlayTone");
	err = AudioHAL_AifTone((AUDIOHAL_TONE_TYPE_T)tone, (AUDIOHAL_TONE_ATTENUATION_T)attenuation, TRUE);
	if (err) {
		ALOGE("MCI_AudioPlayTone failed");
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

UINT32 MCI_AudioStopTone(MCI_AUDIO_TONE_TYPE_T tone, MCI_AUDIO_TONE_ATTENUATION_T attenuation)
{
	int err;

	ALOGI("MCI_AudioStopTone");
	err = AudioHAL_AifTone((AUDIOHAL_TONE_TYPE_T)tone, (AUDIOHAL_TONE_ATTENUATION_T)attenuation, FALSE);
	if (err) {
		ALOGE("MCI_AudioStopTone failed");
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}
UINT32 MCI_AudioPauseTone(VOID)
{
	int err;

	ALOGI("MCI_AudioPauseTone");
	err = AudioHAL_AifTonePause(TRUE);
	if (err) {
		ALOGE("MCI_AudioPauseTone failed");
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}
UINT32 MCI_AudioResumeTone(VOID)
{
	int err;

	ALOGI("MCI_AudioResumeTone");
	err = AudioHAL_AifTonePause(FALSE);
	if (err) {
		ALOGE("MCI_AudioResumeTone failed");
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

UINT32 MCI_AudioPlayWithType(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress, audio_stream_type_t type)
{
	ALOGI("MCI_AudioPlay");
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(OutputPath);
	gAudioPlayer->setAudioStreamType(type);
	err = gAudioPlayer->setDataSourceFd((int)fileHandle);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t seekTo = (int32_t)((double)durationMs * PlayProgress / 10000.f);
	err = gAudioPlayer->seekTo(&seekTo);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGI("Audio playback started");
	ReleaseAudioPlayerRes();

	return MCI_ERR_NO;
}
UINT32 MCI_AudioPlayLoop(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress, UINT8 loop)	//  MCI_MEDIA_PLAY_REQ,
{
    ALOGI("MCI_AudioPlay");
    int err = MCI_ERR_NO;

    if (false == AquireAudioPlayerRes())
    {
        ALOGW("AudioPlayer is busy.");
        return MCI_ERR_BUSY;
    }

    if (gAudioPlayer)
    {
        ReleaseAudioPlayerRes();
        return MCI_ERR_BUSY;
    }

    gAudioPlayer = new AMediaPlayer();
    gAudioPlayerListener = new AudioPlayerListener(callback);
    gAudioPlayer->setListener(gAudioPlayerListener);
    gAudioPlayer->setAudioSinkPort(OutputPath);
    err = gAudioPlayer->setDataSourceFd((int)fileHandle);
    if (err != MEDIA_OK)
    {
        ALOGI("dataSource malformed");
        ReleaseAudioPlayerRes();
        gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
        return MCI_ERR_ERROR;
    }

    int32_t durationMs = 0;
    err = gAudioPlayer->getDuration(&durationMs);
    if (err != MEDIA_OK)
    {
        ReleaseAudioPlayerRes();
        gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
        return MCI_ERR_ERROR;
    }

    int32_t seekTo = (int32_t)((double)durationMs * PlayProgress / 10000.f);
    err = gAudioPlayer->seekTo(&seekTo);
    if (err != MEDIA_OK)
    {
        ReleaseAudioPlayerRes();
        gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
        return MCI_ERR_ERROR;
    }
	if (loop)
	{
		gAudioPlayer->setLooping(true);
	}
    err = gAudioPlayer->start();
    if (err != MEDIA_OK)
    {
        ReleaseAudioPlayerRes();
        gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
        return MCI_ERR_ERROR;
    }

    ALOGI("Audio playback started");
    ReleaseAudioPlayerRes();

    return MCI_ERR_NO;
}

UINT32 MCI_AudioPlay(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress)	//  MCI_MEDIA_PLAY_REQ,
{
	ALOGI("MCI_AudioPlay");
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(OutputPath);
	err = gAudioPlayer->setDataSourceFd((int)fileHandle);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t seekTo = (int32_t)((double)durationMs * PlayProgress / 10000.f);
	err = gAudioPlayer->seekTo(&seekTo);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGI("Audio playback started");
	ReleaseAudioPlayerRes();

	return MCI_ERR_NO;
}

UINT32 MCI_AudioPlayWithOffloadMode(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress)	//  MCI_MEDIA_PLAY_REQ,
{
	ALOGI("MCI_AudioPlayWithOffloadMode");
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(OutputPath);
	err = gAudioPlayer->setDataSourceFd((int)fileHandle);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	err = gAudioPlayer->setAudioSinkWithOffloadMode();
	if (err != MEDIA_OK)
	{
		ALOGI("using offload forbidden");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t seekTo = (int32_t)((double)durationMs * PlayProgress / 10000.f);
	err = gAudioPlayer->seekTo(&seekTo);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGI("Audio playback started");
	ReleaseAudioPlayerRes();

	return MCI_ERR_NO;
}

#if defined(ENABLE_A2DP_AUDIOSINK)

UINT32 MCI_AudioPlayWithBtConnected(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress)	//  MCI_MEDIA_PLAY_REQ,
{
	ALOGI("MCI_AudioPlayWithBtConnected");
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(OutputPath);
	err = gAudioPlayer->setDataSourceFd((int)fileHandle);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	gAudioPlayer->setUsingA2dpSink(true);

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t seekTo = (int32_t)((double)durationMs * PlayProgress / 10000.f);
	err = gAudioPlayer->seekTo(&seekTo);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGI("Audio playback started");
	ReleaseAudioPlayerRes();

	return MCI_ERR_NO;
}

#endif

UINT32 MCI_AudioPause(VOID)
{
	ALOGI("MCI_AudioPause");
	if (gAudioPlayer == NULL)
	{
		return MCI_ERR_ERROR;
	}

	if (gAudioPlayer->pause())
	{
		return MCI_ERR_ERROR;
	}
	return MCI_ERR_NO;
}

UINT32 MCI_AudioGetDurationTime(HANDLE fileHandle, mci_type_enum fielType, INT32 BeginPlayProgress, INT32 OffsetPlayProgress, MCI_ProgressInf * PlayInformation)
{
	ALOGI("MCI_AudioGetDurationTime");	
	int err = MCI_ERR_NO;
	AMediaPlayer *player = NULL;

	player = new AMediaPlayer();
	err = player->setDataSourceFd((int)fileHandle);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		delete player;
        player = NULL;
		return MCI_ERR_ERROR;
	}

	if (PlayInformation)
	{
		err = player->getDuration(&PlayInformation->DurationTime);
		if (err != MEDIA_OK)
		{
            delete player;
            player = NULL;
			return MCI_ERR_ERROR;
		}
	}
    if (player != NULL)
    {
	    delete player;
    	player = NULL;
    }

	return MCI_ERR_NO;
}

UINT32 MCI_AudioSeekTo(INT32 seekMs)
{
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer == NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_NO;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_ERROR;
	}

	if (seekMs >= durationMs || seekMs < 0)
	{
		seekMs = 0;
	}

	err = gAudioPlayer->seekTo(&seekMs);

	ReleaseAudioPlayerRes();

	return err;
}

UINT32 MCI_AudioResume(HANDLE fileHandle)
{
	ALOGI("MCI_AudioResume");	
	if (gAudioPlayer == NULL)
	{
		return MCI_ERR_ERROR;
	}
	if (gAudioPlayer->resume())
	{
		return MCI_ERR_ERROR;
	}

	return MCI_ERR_NO;
}

UINT32 MCI_AudioStop(VOID)
{
	ALOGI("MCI_AudioStop");

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer == NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_NO;
	}
	if (gAudioPlayer->stop())
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_ERROR;
	}

	delete gAudioPlayer;
	gAudioPlayer = NULL;

	if (gAudioPlayerListener)
	{
		delete gAudioPlayerListener;
		gAudioPlayerListener = NULL;
	}
	ReleaseAudioPlayerRes();

	return MCI_ERR_NO;	
}

UINT32 MCI_AudioGetID3(char *pfilename)
{
	ALOGI("MCI_AudioGetID3");
	return MCI_ERR_NO;
}

UINT32 MCI_AudioGetPlayInformation(MCI_PlayInf * PlayInformation)
{
	ALOGV("MCI_AudioGetPlayInformation");
	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy (MCI_AudioGetPlayInformation).");
		return MCI_ERR_BUSY;
	}

	if (PlayInformation == NULL || gAudioPlayer == NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_NO;
	} 

	int32_t durationMs = 0, currentPosition = 0;
	if (gAudioPlayer->getDuration(&durationMs) != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_ERROR;
	}
	if (gAudioPlayer->getCurrentPosition(&currentPosition) != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_ERROR;
	}

	PlayInformation->PlayProgress = (int32_t)((double)currentPosition / durationMs * 10000.f);
	PlayInformation->curPlayTimeMs = currentPosition;

	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;	
}


VOID MCI_AudioSetMute(BOOL onoff)
{
	ALOGI("MCI_AudioSetMute onoff = %d", onoff);
	AudioHAL_AifSetMute(onoff);
}
#ifdef CONFIG_USE_AUDIO_POLICY

UINT32 MCI_AudioSetVolumebyPolicy(UINT16 volume, volume_strategy_t vol_strategy)
{
	ALOGV("MCI_AudioSetVolume by Policy");
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	mAudioPolicy->setAudioVolume(volume, vol_strategy);
	return volume;
}
INT32 MCI_AudioGetVolumebyPolicy(volume_strategy_t vol_strategy)
{
	ALOGV("MCI_AudioGetVolume by Policy");
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	return mAudioPolicy->getAudioVolume(vol_strategy);
}
void MCI_AudioSetVolumebyPolicy_stub(UINT16 volume)
{
	volume_strategy_t vol_strategy=VOLUME_MEDIA;
	MCI_AudioSetVolumebyPolicy(volume, vol_strategy);
	ALOGI("MCI_AudioGetVolumebyPolicy VOLUME_MEDIA Volume = %d", MCI_AudioGetVolumebyPolicy(vol_strategy));
	vol_strategy=VOLUME_PHONE;
	MCI_AudioSetVolumebyPolicy(volume, vol_strategy);
	ALOGI("MCI_AudioGetVolumebyPolicy VOLUME_PHONE Volume = %d", MCI_AudioGetVolumebyPolicy(vol_strategy));
	vol_strategy=VOLUME_NOTIFICATION;
	MCI_AudioSetVolumebyPolicy(volume, vol_strategy);
	ALOGI("MCI_AudioGetVolumebyPolicy VOLUME_NOTIFICATION Volume = %d", MCI_AudioGetVolumebyPolicy(vol_strategy));

	MCI_AudioSetVolumebyPolicy(volume, NUM_VOLUME_STRATEGIES);
	
	MCI_AudioSetVolumebyPolicy(20, vol_strategy);
}

#endif

UINT32 MCI_AudioSetVolume(UINT16 volume)
{
	//ALOGI("MCI_AudioSetVolume");
	AudioHAL_AifSetVolume(volume);
	//ALOGI("MCI_AudioSetVolume exit");
	return volume;
}
UINT32 MCI_AudioSetOutputPath(UINT16 OutputPath, UINT16 Onoff)
{
	AUDIOHAL_ITF_T itf;

	//ALOGI("MCI_AudioSetOutputPath");

	switch(OutputPath)
	{
		case MCI_PATH_NORMAL:
			itf = AUDIOHAL_ITF_RECEIVER;
			break;
		case MCI_PATH_HP:
			itf = AUDIOHAL_ITF_HEADPHONE;
			break;
		case MCI_PATH_LSP:
			itf = AUDIOHAL_ITF_LOUDSPEAKER;
			break;
		case MCI_PATH_FM_HP:
			itf = AUDIOHAL_ITF_FM;
			break;
		case MCI_PATH_FM_LSP_HP:
			itf = AUDIOHAL_ITF_FM2SPK;
			break;
		//itf = AUDIOHAL_ITF_BLUETOOTH;
		default:
			itf = AUDIOHAL_ITF_RECEIVER;
			break;
	}
#if 0
	//ALOGI("%s, itf = %d, OutputPath = %d, Onoff = %d.", __func__, itf, OutputPath, Onoff);
	//fixme: need be update
	//AudioHAL_AifOpen(..., ...);
#else
	//ALOGI("%s: itf = %d, do nothing, not switch.", __func__, itf);
#endif	

	return MCI_ERR_NO;
}

extern "C" void MCI_AudioOpenPath(UINT32 PATH);
extern "C" void MCI_AudioClosePath(void);
extern "C" UINT32 MCI_AudioGetOpenedPath(void);

VOID MCI_AudioOpenPath(UINT32 PATH)
{
	AUDIOHAL_AIF_DEVICE_CFG_T AudioHAL_config;
	AUDIOHAL_ITF_T AudioHAL_itf = AUDIOHAL_ITF_RECEIVER;
	AUDIOHAL_ERR_T ret;

	switch(PATH)
	{
		case MCI_PATH_NORMAL:
			AudioHAL_itf = AUDIOHAL_ITF_RECEIVER;
			break;
		case MCI_PATH_HP:
			AudioHAL_itf = AUDIOHAL_ITF_HEADPHONE;
			break;
		case MCI_PATH_LSP:
			AudioHAL_itf = AUDIOHAL_ITF_LOUDSPEAKER;
			break;
		case MCI_PATH_FM_HP:
			AudioHAL_itf = AUDIOHAL_ITF_FM;
			break;
		case MCI_PATH_FM_LSP_HP:
			AudioHAL_itf = AUDIOHAL_ITF_FM2SPK;
			break;
		case MCI_PATH_LSP_AND_HP:
			AudioHAL_itf = AUDIOHAL_ITF_LOUDSPEAKER_AND_HEADPHONE;
			break;
		case MCI_PATH_BT:
			AudioHAL_itf = AUDIOHAL_ITF_BLUETOOTH;
			break;
		case MCI_PATH_TV:
			AudioHAL_itf = AUDIOHAL_ITF_TV;
			break;
		case MCI_PATH_QTY:
			AudioHAL_itf = AUDIOHAL_ITF_QTY;
			break;
		default:
			AudioHAL_itf = AUDIOHAL_ITF_RECEIVER;
			break;
	}
	gMciAudioPath = AudioHAL_itf;

	ALOGI("MCI_AudioOpenPath AudioHAL_itf %d, PATH %d Caller0x%x:", AudioHAL_itf, PATH, __return_address());

	memset(&AudioHAL_config, 0, sizeof(AUDIOHAL_AIF_DEVICE_CFG_T));
	ret = AudioHAL_AifOpen(AudioHAL_itf, &AudioHAL_config);
	ALOGI("AudioHAL_AifOpen ret: %d\n", ret);
}

UINT32 MCI_AudioGetOpenedPath(void)
{
	UINT32 PATH = MCI_PATH_NORMAL;
	AUDIOHAL_ITF_T AudioHAL_itf = AUDIOHAL_ITF_RECEIVER;
	AudioHAL_itf = AudioHAL_AifGetOpenedDevice();
	switch(AudioHAL_itf)
	{
		case AUDIOHAL_ITF_RECEIVER:
			PATH = MCI_PATH_NORMAL;
			break;
		case AUDIOHAL_ITF_HEADPHONE:
			PATH = MCI_PATH_HP;
			break;
		case AUDIOHAL_ITF_LOUDSPEAKER:
			PATH = MCI_PATH_LSP;
			break;
		case AUDIOHAL_ITF_FM:
			PATH = MCI_PATH_FM_HP;
			break;
		case AUDIOHAL_ITF_FM2SPK:
			PATH = MCI_PATH_FM_LSP_HP;
			break;
		case AUDIOHAL_ITF_LOUDSPEAKER_AND_HEADPHONE:
			PATH = MCI_PATH_LSP_AND_HP;
			break;
		case AUDIOHAL_ITF_BLUETOOTH:
			PATH = MCI_PATH_BT;
			break;
		case AUDIOHAL_ITF_TV:
			PATH = MCI_PATH_TV;
			break;
		case AUDIOHAL_ITF_QTY:
			PATH = MCI_PATH_QTY;
			break;
		default:
			PATH = MCI_PATH_NORMAL;
			break;
	}
	return PATH;
}

VOID MCI_AudioClosePath(void)
{
	ALOGI("MCI_AudioClosePath");
	AudioHAL_AifClose();
	gMciAudioPath = AUDIOHAL_ITF_NONE;
}

AUDIOHAL_ITF_T MCI_GetMciAudioPath(void)
{
	ALOGI("gMciAudioPath=%d", gMciAudioPath);
	return gMciAudioPath;
}

#ifdef CONFIG_USE_AUDIO_POLICY
UINT32 MCI_SetDeviceConnectionState(MCI_AUDIO_PATH_T device, device_state_t state)
{
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	ALOGI("MCI_SetDeviceConnectionState");
	mAudioPolicy->setOutputDeviceConectionState(device, state);
	return MCI_ERR_NO;
}

UINT32 MCI_DeviceSetForceUse(MCI_AUDIO_PATH_T device)
{
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	ALOGI("MCI_DeviceSetForceUse");
	mAudioPolicy->setDeviceForceUse(device);

	return MCI_ERR_NO;
}

UINT32 MCI_SetDeviceMute(MCI_AUDIO_PATH_T device)
{
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	ALOGI("MCI_SetDeviceMute");
	mAudioPolicy->setDeviceMute(device);

	return MCI_ERR_NO;
}

UINT32 MCI_GetRoute()
{
	UINT32 device;
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	ALOGI("MCI_GetRoute");
	device = mAudioPolicy->getRoute();

	return device;
}

UINT32 MCI_SetVoiceState(audio_mode_t mode)
{
	if (mAudioPolicy == NULL)
		mAudioPolicy = AudioPolicyManager::getInstance();

	ALOGI("MCI_SetVoiceState");
	mAudioPolicy->setPhoneState(mode);

	return MCI_ERR_NO;
}
#else
UINT32 MCI_SetDeviceConnectionState(MCI_AUDIO_PATH_T device, device_state_t state)
{
	ALOGI("MCI_SetDeviceConnectionState");
	return MCI_ERR_NO;
}

UINT32 MCI_DeviceSetForceUse(MCI_AUDIO_PATH_T device)
{
	ALOGI("MCI_DeviceSetForceUse");

	return MCI_ERR_NO;
}

UINT32 MCI_SetDeviceMute(MCI_AUDIO_PATH_T device)
{
	ALOGI("MCI_SetDeviceMute");

	return MCI_ERR_NO;
}

UINT32 MCI_GetRoute()
{
	ALOGI("MCI_GetRoute");

	return MCI_ERR_NO;
}

UINT32 MCI_SetVoiceState(audio_mode_t mode)
{
	ALOGI("MCI_SetVoiceState");

	return MCI_ERR_NO;
}

#endif
UINT32 MCI_AudioSetEQ(AUDIO_EQ EQMode)
{
	ALOGI("MCI_AudioSetEQ");
	return MCI_ERR_NO;
}

MCI_ERR_T MCI_AudioGetFileInformation(CONST HANDLE FileHander, AudDesInfoStruct * CONST DecInfo, CONST mci_type_enum FileType)
{
	ALOGI("MCI_AudioGetFileInformation");
	AMediaPlayer *player = NULL;
	int err = MCI_ERR_NO;

	player = new AMediaPlayer();
	err = player->setDataSourceFd((int)FileHander);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		delete player;
		return MCI_ERR_ERROR;
	}

	if (DecInfo)
	{
		//err = player->getDuration(&PlayInformation->DurationTime);
		AMediaFileMetadata meta;
		memset(&meta, 0, sizeof(meta));
		err = player->getMediaMeta(&meta);
		if (err != MEDIA_OK)
		{           
			delete player;
			return MCI_ERR_ERROR;
		}
		DecInfo->time = meta.mDurationMs/1000;
        DecInfo->bitRate = meta.mBitrate;
        DecInfo->sampleRate = meta.mSampleRate;
        DecInfo->stereo = meta.mChannels == 2;
        strncpy((char*)DecInfo->title, (const char*)meta.mTitle, MAX_AUD_DESCRIPTION*sizeof(INT16));
        strncpy((char*)DecInfo->artist, (const char*)meta.mArtist, MAX_AUD_DESCRIPTION*sizeof(INT16));
        strncpy((char*)DecInfo->album, (const char*)meta.mAlbum, MAX_AUD_DESCRIPTION*sizeof(INT16));
        strncpy((char*)DecInfo->author, (const char*)meta.mAuthor, MAX_AUD_DESCRIPTION*sizeof(INT16));
        strncpy((char*)DecInfo->date, (const char*)meta.mYear, MAX_AUD_DESCRIPTION*sizeof(INT16));
        DecInfo->copyright[0] = '\0';
	}
    if (player != NULL)
    {
	    delete player;
    	player = NULL;
    }

	return MCI_ERR_NO;
}

INT32 MCI_AudioPlayBufferWithType(INT32 * pBuffer, UINT32 len, UINT8 loop, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_PLAY_MODE_T format, INT32 startPosition, audio_stream_type_t type)
{
	ALOGV("MCI_AudioPlayBuffer, %p ", callback);
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer != NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(MCI_PATH_NORMAL);
	gAudioPlayer->setAudioStreamType(type);
	err = gAudioPlayer->setDataSource((uint8_t*)pBuffer, len);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	if (startPosition > 0)
	{
		int32_t seekTo = (int32_t)((double)durationMs * startPosition * 1.f / len);
		if (seekTo > 0) 
		{
			err = gAudioPlayer->seekTo(&seekTo);
			if (err != MEDIA_OK)
			{
				ReleaseAudioPlayerRes();
				gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
				return MCI_ERR_ERROR;
			}
		}
	}
	if (loop)
	{
		gAudioPlayer->setLooping(true);
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGV("Audio playback started");
	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;

}

INT32 MCI_AudioPlayBuffer(INT32 * pBuffer, UINT32 len, UINT8 loop, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_PLAY_MODE_T format, INT32 startPosition)
{
	ALOGI("MCI_AudioPlayBuffer, %p ", callback);
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer != NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(MCI_PATH_NORMAL);
	err = gAudioPlayer->setDataSource((uint8_t*)pBuffer, len);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	if (startPosition > 0)
	{
		int32_t seekTo = (int32_t)((double)durationMs * startPosition * 1.f / len);
		if (seekTo > 0) 
		{
			err = gAudioPlayer->seekTo(&seekTo);
			if (err != MEDIA_OK)
			{
				ReleaseAudioPlayerRes();
				gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
				return MCI_ERR_ERROR;
			}
		}
	}
	if (loop)
	{
		gAudioPlayer->setLooping(true);
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGV("Audio playback started");
	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;

}

#if defined(ENABLE_A2DP_AUDIOSINK)

INT32 MCI_AudioPlayBufferWithBtConnected(INT32 * pBuffer, UINT32 len, UINT8 loop, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_PLAY_MODE_T format, INT32 startPosition)
{
	ALOGI("MCI_AudioPlayBufferWithBtConnected, %p ", callback);
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer != NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(MCI_PATH_NORMAL);
	err = gAudioPlayer->setDataSource((uint8_t*)pBuffer, len);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	gAudioPlayer->setUsingA2dpSink(true);

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	if (startPosition > 0)
	{
		int32_t seekTo = (int32_t)((double)durationMs * startPosition * 1.f / len);
		if (seekTo > 0) 
		{
			err = gAudioPlayer->seekTo(&seekTo);
			if (err != MEDIA_OK)
			{
				ReleaseAudioPlayerRes();
				gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
				return MCI_ERR_ERROR;
			}
		}
	}
	if (loop)
	{
		gAudioPlayer->setLooping(true);
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGV("Audio playback started");
	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;
}

#endif

INT32 MCI_AUD_StreamPlayPCM(INT32 * pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback, UINT32 sampleRate, UINT32 bitPerSample)
{
	ALOGI("MCI_AUD_StreamPlayPCM");
	return MCI_ERR_NO;
}

INT32 MCI_AUD_StreamPlay(INT32 * pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback)
{
	ALOGI("MCI_AUD_StreamPlay");
	return MCI_ERR_NO;
}

INT32 MCI_AudioStopBuffer(void)
{
	ALOGI("MCI_AudioStopBuffer");
	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer == NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_NO;
	}
	if (gAudioPlayer->stop())
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_ERROR;
	}

	delete gAudioPlayer;
	gAudioPlayer = NULL;

	if (gAudioPlayerListener)
	{
		delete gAudioPlayerListener;
		gAudioPlayerListener = NULL;
	}
	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;
}


INT32 MCI_AudioStreamingPlay(INT32 * pBuffer, UINT32 len, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_AUDIO_BUFFER_PLAY_DATA_CALLBACK_T data_callback)
{
	ALOGI("MCI_AudioStreamingPlay, %p ", callback);
	int err = MCI_ERR_NO;

	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer != NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_BUSY;
	}

	gAudioPlayer = new AMediaPlayer();
	gAudioPlayerListener = new AudioPlayerListener(callback);
	gAudioPlayer->setListener(gAudioPlayerListener);
	gAudioPlayer->setAudioSinkPort(MCI_PATH_NORMAL);
	err = gAudioPlayer->setDataSource((uint8_t*)pBuffer, len, data_callback);
	if (err != MEDIA_OK)
	{
		ALOGI("dataSource malformed");
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	int32_t durationMs = 0;
	err = gAudioPlayer->getDuration(&durationMs);
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}
	err = gAudioPlayer->start();
	if (err != MEDIA_OK)
	{
		ReleaseAudioPlayerRes();
		gAudioPlayerListener->onEvent(AMediaPlayerListener::MEDIAPLAYER_ERROR, NULL);
		return MCI_ERR_ERROR;
	}

	ALOGI("Audio playback started");
	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;

}

INT32 MCI_AudioStopStreaming(void)
{
	ALOGI("MCI_AudioStopStreaming");
	if (false == AquireAudioPlayerRes())
	{
		ALOGW("AudioPlayer is busy.");
		return MCI_ERR_BUSY;
	}

	if (gAudioPlayer == NULL)
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_NO;
	}
	if (gAudioPlayer->stop())
	{
		ReleaseAudioPlayerRes();
		return MCI_ERR_ERROR;
	}

	delete gAudioPlayer;
	gAudioPlayer = NULL;

	if (gAudioPlayerListener)
	{
		delete gAudioPlayerListener;
		gAudioPlayerListener = NULL;
	}
	ReleaseAudioPlayerRes();
	return MCI_ERR_NO;
}


UINT32 MCI_AudioPlayPause(VOID)
{
	ALOGI("MCI_AudioPlayPause");
	return MCI_ERR_NO;
}

UINT32 MCI_AudioPlayResume(VOID)
{
	ALOGI("MCI_AudioPlayResume");
	return MCI_ERR_NO;
}

UINT32 *MCI_GetBufPosition(VOID)
{
	ALOGI("MCI_GetBufPosition");
	return 0;
}

void MCI_GetWriteBuffer(UINT32 ** buffer, UINT32 * buf_len)
{	
	ALOGI("MCI_GetWriteBuffer");
}

UINT32 MCI_GetRemain(VOID)
{
	ALOGI("MCI_GetRemain");
	return MCI_ERR_NO;
}

void MCI_DataFinished(void)
{
	ALOGI("MCI_DataFinished");
}

INT32 MCI_AddedData(UINT32 addedDataBytes)
{
	ALOGI("MCI_AddedData");
	return addedDataBytes;
}

void MCI_SetBuffer(UINT32 * buffer, uint32 buf_len)
{
	ALOGI("MCI_SetBuffer");
}

BOOL MCI_AifSetPhoneStatus(unsigned int status)
{
	AUDIOHAL_ERR_T rtn = AUDIOHAL_ERR_NO;
	ALOGI("MCI_AifSetPhoneStatus %d", status);
	rtn = AudioHAL_AifSetPhoneStatus(status);
	if(rtn == AUDIOHAL_ERR_NO)
		return true;
	else
		return false;
}

#ifdef CONFIG_MMI_SUPPORT_BLUETOOTH
BOOL MCI_init_btVolLevel_table(unsigned char max_level, unsigned int step_level)
{
	int i = 0;
	
	ALOGI("%s,max=%d, step=%d", __func__,max_level,step_level);
	if(g_btVol_table_ready == 1)
	{
		ALOGI("%s,error-1", __func__);
		return true;
	}
	
	if((max_level <= 2) || (max_level >= MAX_ITEM_BTVOL))
	{
		ALOGI("%s,error-2", __func__);
		return false;
	}

	if( (max_level * step_level) > MAX_BTVOL)
	{
		ALOGI("%s,error-3", __func__);
		return false;
	}

	memset(&g_btVol_table , 0xff, sizeof(g_btVol_table));
	for(i=0; i<max_level; i++)
	{
		g_btVol_table[max_level-i] = MAX_BTVOL - i*step_level;
	}
	g_btVol_table[0] = 0;	//mute

	//set flag:inited
	g_btVol_table_ready = 1;
	return true;
}

//set bluetooth music volume
//volume:0~0x7f, 0-minial bt volume=0; 0x7f-max bt volume.
UINT32 MCI_AudioSetVolume_Bt(unsigned char level)
{
	unsigned char val = 0;
	ALOGI("%s,level=%d", __func__, level);
	if(g_btVol_table_ready == 0)
	{
		ALOGI("%s,error-1", __func__);
		return 0xffffffff;
	}
	if(level > MAX_ITEM_BTVOL)
	{
		ALOGI("%s,error-2", __func__);
		return 0xfffffffe;
	}
	val = g_btVol_table[level];
	appbt_a2dp_set_volume(val);
	return val;
}
#endif	//CONFIG_MMI_SUPPORT_BLUETOOTH

