#ifndef __MULTIMEDIA_INTF_TESTBENCH_H
#define __MULTIMEDIA_INTF_TESTBENCH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ui_os_api.h>
#include "mci.h"		

struct audio_player_selector {
	UINT32 (*play)(INT32 OutputPath, HANDLE fhd, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T cb, INT32 PlayProgress);
	UINT32 (*pause)(VOID);	
	UINT32 (*resume)(HANDLE fileHandle);
	UINT32 (*stop)(VOID);	
	UINT32 (*set_equalizer)(AUDIO_EQ EQMode);
	UINT32 (*set_output_path)(UINT16 OutputPath, UINT16 Mute);	
	UINT32 (*set_volume)(UINT16 volume);	
	UINT32 (*get_play_information)(MCI_PlayInf* PlayInformation);
};

struct video_player_selector {
	UINT32  (*open) (INT32 OutputPath, UINT8 open_audio, HANDLE fhd, UINT8 file_mode, UINT32 data_Len, mci_type_enum type, VOID(*vid_play_finish_callback)(int32), VOID (*vid_draw_panel_callback)(uint32));
	VOID    (*open_ind) (mci_vid_file_ready_ind_struct *vdoOpen);
	UINT32  (*close)(VOID);
	UINT32  (*play) (UINT16 startX, UINT16 startY);
	VOID    (*play_ind) (VOID);
	UINT32  (*stop) (VOID);
	UINT32  (*pause) (VOID); 
	UINT32  (*resume) (VOID);
	UINT32  (*seek) (long long playtime, INT32 time_mode, INT16 startX, INT16 startY);
	VOID    (*seek_ind) (VOID);
	UINT32  (*set) (UINT16 ZoomWidth, UINT16 ZoomHeight, INT16 startX, INT16 startY, INT16 cutX, INT16 cutY, INT16 cutW, INT16 cutH, UINT16 Rotate);
	VOID  (*get_infomation) ( UINT16  *image_width, UINT16  *image_height, UINT32  *total_time, UINT16  *aud_channel, UINT16  *aud_sample_rate,UINT16  *track);  
	UINT32  (*get_play_time) (VOID);
};


struct video_recorder_selector {
	UINT32  (*preview_start) (MMC_VDOREC_SETTING_STRUCT *previewPara, VOID(*vid_rec_finish_ind)(UINT16 msg_result));
	UINT32 	(*preview_stop)(VOID);
	UINT32  (*set_preview_parameter)(INT32 adjustItem, INT32 value);
	UINT32	(*record_start) (HANDLE filehandle);
	UINT32	(*record_stop) (VOID);
	UINT32	(*record_resume) (VOID);
	UINT32  (*record_pause)(void);
};

struct audio_recorder_selector {
	UINT32 	(*record_start) (HANDLE fhd, mci_type_enum format, U8 quality, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback, MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback);
	UINT32	(*record_stop) (VOID);
	UINT32	(*record_resume) (VOID);
	UINT32  (*record_pause)(void);
};

enum {
	AV_PLAY_FLAGS_PLAY_ERR = 1,
	AV_PLAY_FLAGS_PLAY_EOF = 2,	
	AV_PLAY_FLAGS_PLAY_STOP = 4,
};

struct audio_player {
	struct audio_player_selector selector;

	/** audio player information **/
	int32 fhd;
	mci_type_enum filetype;
	INT32 output_device;

	OSATaskRef *mTask;
};

struct video_player {
	struct video_player_selector selector;

	/** audio player information **/
	int32 fhd;
	int32 fileSize;
	mci_type_enum filetype;
	INT32 output_device;

	OSATaskRef *mTask;
};

struct video_recorder {
	struct video_recorder_selector selector;

	int32 fhd;	
	mci_type_enum filetype;
};

struct audio_recorder {
	struct audio_recorder_selector selector;

	int32 fhd;	
	mci_type_enum filetype;
};

//Audio test code
/*----------------------------------------------------------------------------------------------------------
Function: MultiMediaPlayerTestInit_Buffer
Description: test audio interface: _MCI_AudioPlayBuffer and _MCI_AudioStopBuffer

Parameters: NONE

Return value: NONE

Note: 1. Currently the api("MultiMediaPlayerTestInit_Buffer") in mdi_testbench.c is denoted because of Audio array too large.
	     If you want to test the interface, can open macro("#if 0")
-----------------------------------------------------------------------------------------------------------*/
void MultiMediaPlayerTestInit_Buffer (void);


/*----------------------------------------------------------------------------------------------------------
Function: MultiMediaPlayerTestInit
Description: test audio interface

Parameters: NONE

Return value: NONE

Note: 1. The audio file is read from SD card. ( Currently, suport audio format including: .wav   .mp3   .amr)
		 The audio filename defaults as follow:
			Audio_8K_2ch_16b_30s.wav
			Portland_MP3_8KHz_2ch_10s.mp3
			WhatDoYouMean_AMRNB_8K_1ch_4750bits_5s.amr
			
			"8K" --- sample rate:8000Hz
			"2ch" --- 2 channels
			"16b" --- bit-depth: 16bits
		 ADVICE: Recommend audio files to be played convert parameters by software, named "Gold Wave".
	  2. Default: test the play and stop interface
	  	 When play to the end of the audio file, call the stop interface.  
-----------------------------------------------------------------------------------------------------------*/
void MultiMediaPlayerTestInit (void);

/*
*Summary:    MultiMediaVideoRecorderTestInit
             This function is a example for using camera APIs, after run this function, 
             you will see the preview and the captured picture on the SD card
             Notes: you must assure that the camera and lcd are on you Crane DKB
             
*Parameters: None
             
*Retrun: None                  
*/
void MultiMediaVideoRecorderTestInit(void);


/*----------------------------------------------------------------------------------------------------------
Function: MultiMediaTestInit
Description: test audio and camera interface

Parameters: NONE

Return value: NONE

Note: 1.  MACRO "AUDIO_PLAYER_TEST" is controlled the audio test
	  2.  MACRO "VIDEO_RECORDER_TEST" is controlled the camera test
-----------------------------------------------------------------------------------------------------------*/
void MultiMediaTestInit(void);

#ifdef __cplusplus
}
#endif

#endif
