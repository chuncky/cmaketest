#include <string.h>

#include "aud_m.h"
#include "mdi_testbench.h"
#include "fs_api.h"
#include "task_cnf.h"
#include "ui_mem_api.h"

#if 0	/* Buffer Play */
#include "amrnb_8K_1ch.h" 
#endif	/* Buffer Play */


#define LOG_TAG "MdiTestbench"
#define LOG_PRINTF fatal_printf
#define AUDIO_PLAYER_TEST		//if test audioplay, please open this MACRO. Note: SD must have "audio file"(ref AudioSoureList array)
#define VIDEO_RECORDER_TEST
#define AUDIO_RECORDER_TEST


#define TEST_WAVE_INTERFACE
#define TEST_AMR_INTERFACE
#define TEST_MP3_INTERFACE
#define TEST_WAVE_44_1K_INTERFACE


/** player task **/
#define MM_TEST_STACK_SIZE 2048
static HANDLE mmIntfTaskRef ;

static u8 gMutexRef = INVALID_MUTEX_ID;
static u8 threadSyncFlag = INVALID_FLAG_ID;
static u8 playFinishedFlag = INVALID_FLAG_ID;

struct MidMediaSource {
	char *mediaSourceName;
	mci_type_enum media_type;
	char *foramt;
};

#define MDI_VIDEO_PLAY_CASE1 0
#define MDI_VIDEO_PLAY_CASE2 1
#define MDI_VIDEO_PLAY_CASE3 0
#define MDI_VIDEO_PLAY_CASE4 0
#define MDI_VIDEO_PLAY_CASE5 0
#define VIDEO_PLAYER_TEST    1

#ifdef VIDEO_PLAYER_TEST

static struct video_player *mVideoPlayer = NULL;

#define TEST_H263_MP4_INTERFACE 1
/** Video Source **/
static struct MidMediaSource VideoSourceList[] = {
#ifdef TEST_H263_3GP_INTERFACE
	{"d:/H263-NarrowBand-176x144_2.3gp", MCI_TYPE_3GP, "3gp"},
//	{"d:/H263-NarrowBand-352x288_2.3gp", MCI_TYPE_3GP, "3gp"},
#endif
#ifdef TEST_H263_MP4_INTERFACE
	{"d:/H263-NarrowBand-176x144_2.mp4", MCI_TYPE_MP4, "mp4"},
#endif
#ifdef TEST_MPEG4_3GP_INTERFACE
	{"d:/MPEG4_NarrowBand-176x144.3gp", MCI_TYPE_3GP, "3gp"},
#endif
	{NULL},
};

#if MDI_VIDEO_PLAY_CASE5
static struct MidMediaSource loopMediaSource = {"sample.mp4", MCI_TYPE_MP4, "mp4"};
#endif

static void Video_play_callback(int32 state)
{
	UINT32 flags = 0;

	LOG_PRINTF("play callback, state %d", state);

	if (state == MCI_ERR_END_OF_FILE)
		flags =  AV_PLAY_FLAGS_PLAY_EOF;
	else 
		flags = AV_PLAY_FLAGS_PLAY_ERR;
	
	UOS_SetFlag(playFinishedFlag, flags, OSA_FLAG_OR);
}

static void vid_draw_panel_callback(uint32 state)
{
	LOG_PRINTF("draw video");
}

#endif

#ifdef AUDIO_PLAYER_TEST

static struct audio_player *mAudioPlayer = NULL;

/** Audio Source **/
static struct MidMediaSource AudioSoureList[] = {
#ifdef TEST_WAVE_INTERFACE
	{"d:/Audio_8k_2ch_16b_30s.wav", MCI_TYPE_WAV, "wave"},
#endif
#ifdef TEST_MP3_INTERFACE
	{"d:/Portland_MP3_8KHz_2ch_10s.mp3", MCI_TYPE_DAF, "mp3"},
#endif
#ifdef TEST_AMR_INTERFACE
	{"d:/WhatDoYouMean_AMRNB_8K_1ch_4750bits_5s.amr", MCI_TYPE_AMR, "amrnb"},
#endif
#ifdef TEST_WAVE_44_1K_INTERFACE
	{"d:/Audio_44_1k_2ch_16b_10s.wav", MCI_TYPE_WAV, "wave"},
#endif
#ifdef TEST_MIDI_INTERFACE
	{"d:/ants.mid", MCI_TYPE_MIDI, "midi"},
#endif
	{NULL},
};

static void Audio_play_callback(MCI_ERR_T state)
{
	UINT32 flags = 0;

	uart_printf("%s play callback, state %d\n",LOG_TAG, state);	

	if (state == MCI_ERR_END_OF_FILE)
		flags =  AV_PLAY_FLAGS_PLAY_EOF;
	else 
		flags = AV_PLAY_FLAGS_PLAY_ERR;
	
	UOS_SetFlag(playFinishedFlag, flags, OSA_FLAG_OR);	
}

#endif

static void mm_mci_thread_handler(void *argv)
{
	OSA_STATUS osaStatus = OS_SUCCESS;
	UINT32 actual_flags = 0;

	uart_printf("%s multimedia interface self test thread running\n",LOG_TAG);

	for (; ;) {
		osaStatus = UOS_WaitFlag(playFinishedFlag, AV_PLAY_FLAGS_PLAY_ERR | AV_PLAY_FLAGS_PLAY_EOF | AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		if (osaStatus == OS_SUCCESS) {
			if (actual_flags & AV_PLAY_FLAGS_PLAY_EOF) {				
				uart_printf("%s %s: play finished\n",LOG_TAG, __func__);				
			} else if (actual_flags & AV_PLAY_FLAGS_PLAY_ERR) {			
				uart_printf("%s play file error\n",LOG_TAG);
			} else {			
				uart_printf("%s testbench thread stop\n",LOG_TAG);
			}

			UOS_SetFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR);
			if (actual_flags & AV_PLAY_FLAGS_PLAY_STOP) {
				break;
			}
		}
	}
}

#ifdef AUDIO_PLAYER_TEST

#define MDI_AUDIO_PLAY_CASE1  (1)
#define MDI_AUDIO_PLAY_CASE2  (0)



#if 0	/* Buffer Play */
/*---------------------------------------------
Function: MdiAudioTest_Buffer
Buffer Play and stop test

----------------------------------------------*/
static int MdiAudioTest_Buffer(void)
{
	int result;
	uint32 actual_flags;
	MCI_ERR_T error = MCI_ERR_NO;

	uart_printf("\n/************************************************\n");
	uart_printf("Audio_Buffer [%s] module play testing...\n", "amrnb");
	uart_printf("*************************************************/\n");
	
	/////////////////////////////////////////////////////////////////////////////
	// mulitimedia system using file hander to parse media file  
	/////////////////////////////////////////////////////////////////////////////
	result = sizeof(g_amrnb);
	
	if (result <= 0) {
		uart_printf("%s filesystem get file size failed\n",LOG_TAG);
		return 0;
	}
	
	uart_printf("%s %s: file size %d\n", LOG_TAG,__func__, result);
	
	error = (MCI_ERR_T)_MCI_AudioPlayBuffer( AUD_ITF_EAR_PIECE, g_amrnb,sizeof(g_amrnb), 
											  MCI_TYPE_AMR, Audio_play_callback, 0
											  );
	if (error != MCI_ERR_NO) {
		uart_printf("%s play %s file failed, error code %d\n",LOG_TAG, "g_amrnb", error);
		return 0;
	}
	
	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	uart_printf("%s playback finished\n",LOG_TAG);

	error = (MCI_ERR_T)_MCI_AudioStopBuffer();
	if (error != MCI_ERR_NO) {
		uart_printf("%s stop media frameworks player error, code %d", LOG_TAG,error);
		return 0;
	}
	return 1;
}
#endif	/* Buffer Play */


static int MdiAudioTest(struct audio_player *mPlayer, struct MidMediaSource *mSource)
{
	int result;
	uint32 actual_flags;
	MCI_ERR_T error = MCI_ERR_NO;

	uart_printf("\n/************************************************\n");
	uart_printf("Audio [%s] module play testing...\n", mSource->foramt);
	uart_printf("*************************************************/\n");
	
	if (FS_IsExist((uint8_t*)mSource->mediaSourceName) == FALSE) {
		uart_printf("%s %s: reference file is not exist\n",LOG_TAG,__func__);
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////
	// mulitimedia system using file hander to parse media file  
	/////////////////////////////////////////////////////////////////////////////
	mPlayer->fhd = FS_Open((uint8_t*)mSource->mediaSourceName, FS_O_RDONLY, 0);
	if (mPlayer->fhd <= 0) {
		uart_printf("%s open file %s failed, error code %d\n",LOG_TAG, mSource->mediaSourceName, mPlayer->fhd);
		return 0;
	}

	result = FS_GetFileSize(mPlayer->fhd);
	if (result <= 0) {
		uart_printf("%s filesystem get file size failed\n",LOG_TAG);
		return 0;
	}
	
	uart_printf("%s %s: file size %d\n", LOG_TAG,__func__, result);
	FS_Seek(mPlayer->fhd, 0, FS_SEEK_SET);
	mPlayer->filetype = mSource->media_type;
	mPlayer->output_device = AUD_ITF_EAR_PIECE;

#if MDI_AUDIO_PLAY_CASE1

	/** case 1 **/

	///note: playprogress: [0,10000]. If set 0, Play from the head; 10000 -- Jump to the end of the file
	error = (MCI_ERR_T)mPlayer->selector.play(mPlayer->output_device, mPlayer->fhd, mPlayer->filetype, Audio_play_callback, 0);	
	if (error != MCI_ERR_NO) {
		uart_printf("%s play %s file failed, error code %d\n",LOG_TAG, mSource->mediaSourceName, error);
		return 0;
	}

	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);	
 	uart_printf("%s playback finished",LOG_TAG);

	error = (MCI_ERR_T)mPlayer->selector.stop();
	if (error != MCI_ERR_NO) {
		uart_printf("%s stop media frameworks player error, code %d", LOG_TAG,error);
		return 0;
	}
#endif	/* MDI_AUDIO_PLAY_CASE1 */

#if MDI_AUDIO_PLAY_CASE2 
	
		/** case 2 **/
		FS_Seek(mPlayer->fhd, 0, FS_SEEK_SET);
		error = (MCI_ERR_T)mPlayer->selector.play(mPlayer->output_device, mPlayer->fhd, mPlayer->filetype, Audio_play_callback, 0);
		if (error != MCI_ERR_NO) {			
			uart_printf("play %s file failed, error code %d\n", mSource->mediaSourceName, error);
			return 0;
		}
	
		OSATaskSleep(400);	//sleep 2s
	
		error = (MCI_ERR_T)mPlayer->selector.pause();
		if (error != MCI_ERR_NO) {
			uart_printf("stop audio playback pipeline fail, error code %d\n", error);
			return 0;
		}
	
		OSATaskSleep(400);
		error = (MCI_ERR_T)mPlayer->selector.resume(mPlayer->fhd);
		if (error != MCI_ERR_NO) {
			uart_printf("resume audio playback pipeline fail, error code %d\n", error);
			return 0;
		}
		
		UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
		uart_printf("playback finished\n");
	
		error = (MCI_ERR_T)mPlayer->selector.stop();
		if (error != MCI_ERR_NO) {
			uart_printf("stop media frameworks player error, code %d", error);
			return 0;
		}
#endif  /* MDI_AUDIO_PLAY_CASE2 */



	FS_Close(mPlayer->fhd);

	return 1;
}

#endif		/* AUDIO_PLAYER_TEST */


#if 0	/* Buffer Play */
//ICAT EXPORTED FUNCTION - HW_PLAT,AUDIO_TEST,MultiMediaPlayerTestInit_Buffer
void MultiMediaPlayerTestInit_Buffer (void)		
{
	int result = -1;
	int i;
	uint32 actual_flags;

	uart_printf("%s %s: ready to multimedia system interface_Buffer self test\n",LOG_TAG, __func__);
	
	gMutexRef = UOS_NewMutex("TestBenchB");		//Create a mutex for inter-thread mutual exclusion for resource protection
	threadSyncFlag = UOS_CreateFlag();	
	playFinishedFlag= UOS_CreateFlag();
	
	uart_printf("%s %s: create multimedia interface test task\n", LOG_TAG,__func__);
	mmIntfTaskRef = UOS_CreateTask( mm_mci_thread_handler, NULL, MM_TEST_STACK_SIZE, 
									ASYNFS_TASK_PRIORITY + 1, "MdiTestThread" );
	if (mmIntfTaskRef == NULL) {
		uart_printf("%s %s: osa create multimedia interface task fail\n",LOG_TAG, __func__);		
		goto TestFail;
	}

#ifdef AUDIO_PLAYER_TEST		
	result = MdiAudioTest_Buffer();
	if (result) {
		uart_printf("*********************** PASSED **********************/\n");	
	} else {	
		uart_printf("************************ FAILED ********************/\n");
	}
#endif /* AUDIO_PLAYER_TEST */


	/** stop player thread **/
	UOS_SetFlag(playFinishedFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR);
	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);

TestFail:
#ifdef AUDIO_PLAYER_TEST
#endif

	/** always success **/
	UOS_DeleteFlag(threadSyncFlag);
	UOS_DeleteFlag(playFinishedFlag);
	UOS_FreeMutex(gMutexRef);  //delete the specified mutex. All threads suspended waiting for the mutex are resumed

		
	if (mmIntfTaskRef) {
	/**	UOS_DeleteTask((TASK_HANDLE *)mmIntfTaskRef) **/;
	}
	
	return;
}
#endif	/* Buffer Play */

#if 1	/* Test MCI_AudioSetVolume */
//ICAT EXPORTED FUNCTION - HW_PLAT,AUDIO_TEST,Test_MCI_AudioSetVolume
void Test_MCI_AudioSetVolume( void *p_volume)
{
	int result = -1;
	unsigned short volume = *(UINT16 *)p_volume;

	uart_printf("%s %s: [*p_volume = %x]\n",LOG_TAG, __func__,volume);
	
	result = MCI_AudioSetVolume(volume);
	
	uart_printf("%s %s: result:%d",LOG_TAG, __func__,result);	
}
#endif	/* Test MCI_AudioSetVolume*/

//ICAT EXPORTED FUNCTION - HW_PLAT,AUDIO_TEST,Test_MCI_AudioSetMute
void Test_MCI_AudioSetMute( char *p_onoff)
{
	uart_printf("%s %s: [*p_onoff = %x]\n",LOG_TAG, __func__,*(BOOL *)p_onoff);
	
	MCI_AudioSetMute(*(BOOL *)p_onoff);		
}

//ICAT EXPORTED FUNCTION - HW_PLAT,AUDIO_VIDEO_PLAY_TEST,MultiMediaPlayerTestInit
void MultiMediaPlayerTestInit (void)
{
	int result = -1;
	int i;
	uint32 actual_flags;

	uart_printf("%s %s: ready to multimedia system interface self test\n",LOG_TAG, __func__);
	
	gMutexRef = UOS_NewMutex("TestBench");		//Create a mutex for inter-thread mutual exclusion for resource protection
	threadSyncFlag = UOS_CreateFlag();	
	playFinishedFlag= UOS_CreateFlag();
	
	uart_printf("%s %s: create multimedia interface test task\n", LOG_TAG,__func__);
    mmIntfTaskRef = UOS_CreateTask(mm_mci_thread_handler, NULL, TASK_WITHOUT_MSGQ, MM_TEST_STACK_SIZE, ASYNFS_TASK_PRIORITY + 1, "uiMdiThd");

	if (mmIntfTaskRef == NULL) {
		uart_printf("%s %s: osa create multimedia interface task fail\n",LOG_TAG, __func__);		
		goto TestFail;
	}

#ifdef AUDIO_PLAYER_TEST
	mAudioPlayer = (struct audio_player*)UOS_MALLOC(sizeof(*mAudioPlayer));
	if (mAudioPlayer == NULL) {
		uart_printf("%s none audio player structure allocated\n",LOG_TAG);
		goto TestFail;
	}

	memset(mAudioPlayer, 0x0, sizeof(*mAudioPlayer));

	mAudioPlayer->selector.play = MCI_AudioPlay;
	mAudioPlayer->selector.pause = MCI_AudioPause;
	mAudioPlayer->selector.resume = MCI_AudioResume;
	mAudioPlayer->selector.stop = MCI_AudioStop;
	mAudioPlayer->selector.set_equalizer = MCI_AudioSetEQ;
	mAudioPlayer->selector.set_output_path = MCI_AudioSetOutputPath;
	mAudioPlayer->selector.set_volume = MCI_AudioSetVolume;
	mAudioPlayer->selector.get_play_information = MCI_AudioGetPlayInformation;

	uart_printf("%s %s: Audio playback test running @", LOG_TAG,__func__);
	for (i = 0; i < sizeof(AudioSoureList) / sizeof(AudioSoureList[0]); i++) {
		if (AudioSoureList[i].mediaSourceName == NULL)
			continue;
		
		result = MdiAudioTest(mAudioPlayer, &AudioSoureList[i]);
		if (result) {
			uart_printf("\n*********************** PASSED **********************/\n");	
		} else {	
			uart_printf("\n************************ FAILED ********************/\n");
		}
	}
#endif /* AUDIO_PLAYER_TEST */

#ifdef VIDEO_PLAYER_TEST
	mVideoPlayer = (struct video_player*)UOS_MALLOC(sizeof(*mVideoPlayer));
	if (mVideoPlayer == NULL) {
		LOG_PRINTF("none video player structure allocated");
		goto VideoTestFail;
	}
	
	memset(mVideoPlayer, 0x0, sizeof(*mVideoPlayer));
		
	mVideoPlayer->selector.open = MCI_VideoOpenFile;
	mVideoPlayer->selector.close = MCI_VideoClose;
	mVideoPlayer->selector.set = MCI_VideoSet;
	mVideoPlayer->selector.play = MCI_VideoPlay;
	mVideoPlayer->selector.resume = MCI_VideoResume;
	mVideoPlayer->selector.stop = MCI_VideoStop;	
	mVideoPlayer->selector.pause = MCI_VideoPause;
	mVideoPlayer->selector.seek = MCI_VideoSeek;
	mVideoPlayer->selector.get_play_time = MCI_VideoGetPlayTime;
	
	LOG_PRINTF("%s: Video playback test running @", __func__);

#if MDI_VIDEO_PLAY_CASE5
	test_media_recorder_main(loopMediaSource.mediaSourceName);
	result = MdiVideoTest(mVideoPlayer, &loopMediaSource);
	if (result) {
		LOG_PRINTF("*********************** Loopback play PASSED **********************/");
	} else {	
		LOG_PRINTF("************************ Loopback play FAILED ********************/");
	}
#endif
    
	for (i = 0; i < sizeof(VideoSourceList) / sizeof(VideoSourceList[0]); i++) {
		if (VideoSourceList[i].mediaSourceName == NULL)
			continue;
		
		result = MdiVideoTest(mVideoPlayer, &VideoSourceList[i]);
		if (result) {
			LOG_PRINTF("*********************** PASSED **********************/");
		} else {	
			LOG_PRINTF("************************ FAILED ********************/");
		}
	}
	
VideoTestFail:
#endif
	/** stop player thread **/
	UOS_SetFlag(playFinishedFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR);
	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	if(mVideoPlayer)
		UOS_FREE(mVideoPlayer);

TestFail:
#ifdef AUDIO_PLAYER_TEST
	if (mAudioPlayer) {
		UOS_FREE(mAudioPlayer);
	}
#endif

	
	/** always success **/
	UOS_DeleteFlag(threadSyncFlag);
	UOS_DeleteFlag(playFinishedFlag);
	UOS_FreeMutex(gMutexRef);  //delete the specified mutex. All threads suspended waiting for the mutex are resumed

		
	if (mmIntfTaskRef) {
	/**	UOS_DeleteTask((TASK_HANDLE *)mmIntfTaskRef) **/;
	}
	
	return;
}
/** CIF && QCIF **/
static int MdiVideoTest(struct video_player *mPlayer, struct MidMediaSource *mSource)
{
	int result;
	uint32 actual_flags;
	MCI_ERR_T error = MCI_ERR_NO;

	LOG_PRINTF("/************************************************");
	LOG_PRINTF("Video module %s play testing....", mSource->foramt);
	LOG_PRINTF("*************************************************/");

	if (FS_IsExist((uint8_t*)mSource->mediaSourceName) == FALSE) {
		LOG_PRINTF("%s: reference file is not exist", __func__);
		return 0;
	}
    uart_printf("xxx1\n");
	/////////////////////////////////////////////////////////////////////////////
	// mulitimedia system using file hander to parse media file  
	/////////////////////////////////////////////////////////////////////////////
	mPlayer->fhd = FS_Open((uint8_t*)mSource->mediaSourceName, FS_O_RDONLY, 0);
	if (mPlayer->fhd <= 0) {
		LOG_PRINTF("open file %s failed, error code %d", mSource->mediaSourceName, mPlayer->fhd);
		return 0;
	}
    uart_printf("xxx2\n");
	result = FS_GetFileSize(mPlayer->fhd);
	if (result <= 0) {
		LOG_PRINTF("filesystem get file size failed");
		return 0;
	}
    uart_printf("xxx3:%d\n", result);
	mPlayer->fileSize = result;
	FS_Seek(mPlayer->fhd, 0, FS_SEEK_SET);
	mPlayer->filetype = mSource->media_type;
	mPlayer->output_device = AUD_ITF_LOUD_SPEAKER;

#if MDI_VIDEO_PLAY_CASE1
    uart_printf("xxx4\n");
	/** case 1: cif scaling down, qcif normal playback **/
	error = (MCI_ERR_T)mPlayer->selector.open(mPlayer->output_device, 1, mPlayer->fhd, 0, mPlayer->fileSize, mPlayer->filetype, Video_play_callback, vid_draw_panel_callback);
	if (error) {
		LOG_PRINTF("open %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}
	uart_printf("xxx5\n");	
	error = (MCI_ERR_T)mPlayer->selector.set(144,176, 0, 0, 0, 0, 144,176, 0);
	if (error) {
		LOG_PRINTF("set video vender information fail");
		return 0;
	}
    uart_printf("xxx6\n");
	error = (MCI_ERR_T)mPlayer->selector.play(0, 0);
	if (error) {
		LOG_PRINTF("play %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}
    uart_printf("xxx7\n");
	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	uart_printf("xxx8\n");
	mPlayer->selector.close();
	LOG_PRINTF("video playback finished");
#endif

#if MDI_VIDEO_PLAY_CASE2
	/** case 2: cif scaling down, qcif scaling up, rotation **/
	LOG_PRINTF("2video playback\r\n");

	error = (MCI_ERR_T)mPlayer->selector.open(mPlayer->output_device, 1, mPlayer->fhd, 0, mPlayer->fileSize, mPlayer->filetype, Video_play_callback, vid_draw_panel_callback);
	if (error) {
		LOG_PRINTF("open %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}

	error = (MCI_ERR_T)mPlayer->selector.set(240, 320, 0, 0, 0, 0, 240, 320, 3);
	if (error) {
		LOG_PRINTF("set video vender information fail");
		return 0;
	}

	error = (MCI_ERR_T)mPlayer->selector.play(0, 0);
	if (error) {
		LOG_PRINTF("play %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}

	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	mPlayer->selector.close();
	LOG_PRINTF("2playback finished\r\n");
#endif


#if MDI_VIDEO_PLAY_CASE3
	/** case 3: pause-->resume, cif scaling down, qcif scaling up, rotation**/
	LOG_PRINTF("3video playback\r\n");

	FS_Seek(mPlayer->fhd, 0, FS_SEEK_SET);
	error = (MCI_ERR_T)mPlayer->selector.open(mPlayer->output_device, 0, mPlayer->fhd, 0, mPlayer->fileSize, mPlayer->filetype, Video_play_callback, vid_draw_panel_callback);
	if (error) {
		LOG_PRINTF("open %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}

	error = (MCI_ERR_T)mPlayer->selector.set(240, 320, ,0, 0, 0, 0, 240,320, 1);
	if (error) {
		LOG_PRINTF("set video vender information fail");
		return 0;
	}

	error = (MCI_ERR_T)mPlayer->selector.play(0, 0);
	if (error) {
		LOG_PRINTF("play %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}

	UOS_Sleep(400); // sleep 2s

	error = (MCI_ERR_T)mPlayer->selector.pause();
	if (error) {
		LOG_PRINTF("stop audio playback pipeline fail, error code %d", error);
		return 0;
	}

	UOS_Sleep(800); // sleep 4s, 100ms
	error = (MCI_ERR_T)mPlayer->selector.resume();
	if (error) {
		LOG_PRINTF("resume audio playback pipeline fail, error code %d", error);
		return 0;
	}

	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	mPlayer->selector.close();
	
	LOG_PRINTF("3playback finished");
#endif

#if MDI_VIDEO_PLAY_CASE4
	/** case 4: seek, cif scaling down, qcif scaling up, rotation**/
	LOG_PRINTF("4video playback\r\n");

	error = (MCI_ERR_T)mPlayer->selector.open(mPlayer->output_device, 0, mPlayer->fhd, 0, mPlayer->fileSize, mPlayer->filetype, Video_play_callback, vid_draw_panel_callback);
	if (error) {
		LOG_PRINTF("open %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}

	error = (MCI_ERR_T)mPlayer->selector.set(240,320, 0, 0, 0, 0, 240,320, 1);
	if (error) {
		LOG_PRINTF("set video vender information fail");
		return 0;
	}

	LOG_PRINTF("%s seek ...", __func__);

	error = (MCI_ERR_T)mPlayer->selector.seek(2000, 0, 0, 0);
	if (error) {
		LOG_PRINTF("seek error %d", error);
		return 0;
	}

	error = (MCI_ERR_T)mPlayer->selector.play(0, 0);
	if (error) {
		LOG_PRINTF("play %s file failed, error code %d", mSource->mediaSourceName, error);
		return 0;
	}

	UOS_WaitFlag(threadSyncFlag, AV_PLAY_FLAGS_PLAY_STOP, OSA_FLAG_OR_CLEAR, &actual_flags, OS_SUSPEND);
	mPlayer->selector.close();

	LOG_PRINTF("4seek finished");	
#endif

	FS_Close(mPlayer->fhd);

	return 1;
}

#ifdef VIDEO_RECORDER_TEST
//ICAT EXPORTED FUNCTION - HW_PLAT,CAMERA_TEST,MultiMediaVideoRecorderTestInit
void MultiMediaVideoRecorderTestInit(void)
{		
		MMC_VDOREC_SETTING_STRUCT previewData = {0};
		int time = 0;
		UINT32 error;
		const UINT8 *filename = "d:/sample.mp4";
        UINT8 fileName[255];
	    int fileHandle = 0;
		int index;
		fatal_printf("--------	MCI Video Recorder test (%s)-------- ", filename);
		/** set preview parameters **/
		previewData.previewStartX = 0;
		previewData.previewStartY = 0;
		previewData.previewWidth = 320;
		previewData.previewHeight = 240;
		previewData.imageWidth = 176;
		previewData.imageHeight = 144;
        //#ifdef VIDEO_RECORDER_ONLY_TEST
		previewData.IfRecordAudio = 0;
        //#else
        //previewData.IfRecordAudio = 1;
        //#endif
        int32 fhd = FS_Open(filename,  FS_O_CREAT | FS_O_TRUNC | FS_O_WRONLY, 0);
		if (fhd <= 0) {
			fatal_printf(" can not open output file %s", filename);
			//UOS_Free(mVideoRecorder);
			return;
		}
        fatal_printf("MMI :%d\n", __LINE__);


		if (MCI_CamPowerUp(1, NULL)) {
			fatal_printf("open camera fail");
			return;
		}
        fatal_printf("MMI :%d\n", __LINE__);
		error = MCI_VideoRecordPreviewStart(&previewData, NULL);
		if (error != MCI_ERR_NO) {
			fatal_printf("start preview fail");
			return;
		}
		fatal_printf("MMI :%d\n", __LINE__);
		error = MCI_VideoRecordStart(fhd);
		if (error != MCI_ERR_NO) {
			fatal_printf("start record fail");
			//UOS_Free(mVideoRecorder);
			return;
		}
		fatal_printf("MMI :%d\n", __LINE__);
		while(time++ < 10) {
			OSATaskSleep(200);
			fatal_printf("recording %d secs", time);
		}
        fatal_printf("MMI :%d\n", __LINE__);
       	
#if 0
	    CAM_CAPTURE_STRUCT capture_data;
	    capture_data.image_width = 640;
	    capture_data.image_height = 480;

	    for (index = 0; index < 2; index++) {
	        MCI_CamCapture(&capture_data);
	        sprintf((char *)fileName, "D:\\capture%d.jpeg", index);
	        fileHandle = FS_Open(fileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	        if (fileHandle < 0) {
		        fatal_printf("create file for saving jpeg failed, %d!", fileHandle);
		        return;
	        }

	        MCI_CamSavePhoto(fileHandle);
	        if (fileHandle > 0)
			    FS_Close(fileHandle);
	    }

	    fatal_printf("take picture success\n");
	    OSATaskSleep(10);
#endif
		error = MCI_VideoRecordStop();
		if (error != MCI_ERR_NO) {
			fatal_printf("stop record fail");
			//UOS_Free(mVideoRecorder);
			//return;
		}
		error = MCI_VideoRecordPreviewStop();
		if (error != MCI_ERR_NO) {
			// FIXME: following record stop, recorder delete 
			if (mmf_camera_stop_preview()) {
				LOG_PRINTF("stop preview fail");
				//UOS_FREE(mVideoRecorder);
				//return;
			}
		}
		MCI_CamPowerDown();

		FS_Close(fhd);

		fatal_printf("--------	MCI Video Recorder test done -------- \n");
}

//ICAT EXPORTED FUNCTION - HW_PLAT,CAMERA_TEST,MultiMediaTakePhotoTestInit
void MultiMediaTakePhotoTestInit(void)
{		
		MMC_VDOREC_SETTING_STRUCT previewData = {0};
		int time = 0;
		UINT32 error;
		//const UINT8 *filename = "d:/sample.mp4";
        UINT8 fileName[255];
	    int fileHandle = 0;
		int index;
		fatal_printf("--------	MCI Camera takePhoto test-------- ");
		/** set preview parameters **/
		previewData.previewStartX = 0;
		previewData.previewStartY = 0;
		previewData.previewWidth = 320;
		previewData.previewHeight = 240;
		previewData.imageWidth = 176;
		previewData.imageHeight = 144;
        //#ifdef VIDEO_RECORDER_ONLY_TEST
		previewData.IfRecordAudio = 0;
        //#else
        //previewData.IfRecordAudio = 1;
        //#endif
        #if 0
        int32 fhd = FS_Open(filename,  FS_O_CREAT | FS_O_TRUNC | FS_O_WRONLY, 0);
		if (fhd <= 0) {
			fatal_printf(" can not open output file %s", filename);
			//UOS_Free(mVideoRecorder);
			return;
		}
        fatal_printf("MMI :%d\n", __LINE__);
        #endif

		if (MCI_CamPowerUp(1, NULL)) {
			fatal_printf("open camera fail");
			return;
		}
        fatal_printf("MMI :%d\n", __LINE__);
		error = MCI_VideoRecordPreviewStart(&previewData, NULL);
		if (error != MCI_ERR_NO) {
			fatal_printf("start preview fail");
			return;
		}
		fatal_printf("MMI :%d\n", __LINE__);
		while(time++ < 10) {
			OSATaskSleep(200);
			fatal_printf("recording %d secs", time);
		}
        fatal_printf("MMI :%d\n", __LINE__);
       	
#if 1
	    CAM_CAPTURE_STRUCT capture_data;
	    capture_data.image_width = 640;
	    capture_data.image_height = 480;

	    for (index = 0; index < 2; index++) {
	        MCI_CamCapture(&capture_data);
	        sprintf((char *)fileName, "D:\\capture%d.jpeg", index);
	        fileHandle = FS_Open(fileName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
	        if (fileHandle < 0) {
		        fatal_printf("create file for saving jpeg failed, %d!", fileHandle);
		        return;
	        }

	        MCI_CamSavePhoto(fileHandle);
	        if (fileHandle > 0)
			    FS_Close(fileHandle);
	    }

	    fatal_printf("take picture success\n");
	    OSATaskSleep(10);
#endif
		
		error = MCI_VideoRecordPreviewStop();
		if (error != MCI_ERR_NO) {
			// FIXME: following record stop, recorder delete 
			if (mmf_camera_stop_preview()) {
				LOG_PRINTF("stop preview fail");
				//UOS_FREE(mVideoRecorder);
				//return;
			}
		}
		MCI_CamPowerDown();
		fatal_printf("--------	MCI Camera takePhoto test done -------- \n");
}


#endif

#ifdef AUDIO_RECORDER_TEST
static struct audio_recorder *mAudioRecorder = NULL;

//ICAT EXPORTED FUNCTION - HW_PLAT,AUDIO_RECORDER_TEST,MultiMediaAudioRecorderTestInit
void MultiMediaAudioRecorderTestInit(void)
{
	int time = 0;	
	UINT32 error;
	const UINT8 *filename = "d:/sample.amr";

	uart_printf("--------	MCI Audio Recorder test (%s)-------- \n", filename);

	mAudioRecorder = (struct audio_recorder*)UOS_MALLOC(sizeof(*mAudioRecorder));
	if (mAudioRecorder == NULL) {
		uart_printf("none audio recorder structure allocated\n");
		return;
	}

	memset(mAudioRecorder, 0x0, sizeof(*mAudioRecorder));

	/** creating output file **/
	mAudioRecorder->fhd = FS_Open(filename,  FS_O_CREAT | FS_O_TRUNC | FS_O_WRONLY, 0);
	if (mAudioRecorder->fhd <= 0) {
		uart_printf(" can not open output file %s\n", filename);
		UOS_FREE(mAudioRecorder);
		return;
	}

	mAudioRecorder->filetype = MCI_TYPE_AMR;
	mAudioRecorder->selector.record_start = MCI_AudioRecordStart;
	mAudioRecorder->selector.record_stop = MCI_AudioRecordStop;
	mAudioRecorder->selector.record_pause = MCI_AudioRecordPause;
	mAudioRecorder->selector.record_resume = MCI_AudioRecordResume;

	error = mAudioRecorder->selector.record_start(mAudioRecorder->fhd, mAudioRecorder->filetype, 0, NULL, NULL);
	if (error != MCI_ERR_NO) {
		uart_printf("starting Audio recording fail\n");		
		UOS_FREE(mAudioRecorder);
		return;
	}

	while(time++ < 10) {
		OSATaskSleep(200);		//hsy
		uart_printf("recording %d secs\n", time);
	}

	error = mAudioRecorder->selector.record_stop();
	if (error != MCI_ERR_NO) {
		uart_printf("stop record fail\n");
		UOS_FREE(mAudioRecorder);
		return;
	}
	
	FS_Close(mAudioRecorder->fhd);

	if(mAudioRecorder)	//hsy
		UOS_FREE(mAudioRecorder);
	mAudioRecorder = 0;

	uart_printf("--------	MCI Audio Recorder test done -------- \n");
}

#endif

//ICAT EXPORTED FUNCTION - HW_PLAT,MultiMediaTest,MultiMediaTestInit
void MultiMediaTestInit(void)
{
	#if defined(AUDIO_PLAYER_TEST)	//If test audioplay, please open this MACRO. Note: SD must have "audio file"(ref "AudioSoureList" array in this file)
	MultiMediaPlayerTestInit();
	#endif
	
	#if defined(VIDEO_RECORDER_TEST)
	MultiMediaVideoRecorderTestInit();		
	#endif

	#if defined(AUDIO_RECORDER_TEST)
	MultiMediaAudioRecorderTestInit();
	#endif

	return;
}

