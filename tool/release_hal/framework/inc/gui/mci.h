/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/

#ifndef _MCI_H__
#define _MCI_H__

#include "uhaltype.h"
#include "cpu_share.h"

#define SAMPLE_RATE     44100
#define SAMPLE_CHANNEL  1
#define SAMPLE_WIDTH    16
#define SAMPLE_DATA_RATE    ((SAMPLE_RATE) * (SAMPLE_CHANNEL) * (SAMPLE_WIDTH) / 8)

#ifdef __cplusplus
extern "C" {
#endif

#if 0
#ifndef __BT_NO_A2DP_PROFILE__
#ifndef __BT_A2DP_PROFILE__
#define __BT_A2DP_PROFILE__
#endif
#endif
#endif

#if (CSW_EXTENDED_API_AUDIO_VIDEO==1)

#else

typedef enum {	//audio format
   MCI_TYPE_NONE=-1,
   MCI_TYPE_GSM_FR,             /* 0 */
   MCI_TYPE_GSM_HR,              /* 1 */
   MCI_TYPE_GSM_EFR,             /* 2 */
   MCI_TYPE_AMR,                 /* 3 */
   MCI_TYPE_AMR_WB,              /* 4 */
   MCI_TYPE_DAF,                 /* 5 */
   MCI_TYPE_AAC,                 /* 6 */
   MCI_TYPE_PCM_8K,              /* 7 */
   MCI_TYPE_PCM_16K,             /* 8 */
   MCI_TYPE_G711_ALAW,           /* 9 */
   MCI_TYPE_G711_ULAW,           /* 10 */
   MCI_TYPE_DVI_ADPCM,           /* 11 */
   MCI_TYPE_VR,                  /* 12 */
   MCI_TYPE_WAV,                 /* 13 */
   MCI_TYPE_WAV_ALAW,            /* 14 */
   MCI_TYPE_WAV_ULAW,            /* 15 */
   MCI_TYPE_WAV_DVI_ADPCM,       /* 16 */
   MCI_TYPE_SMF,                    /* 17 */
   MCI_TYPE_IMELODY,                /* 18 */
   MCI_TYPE_SMF_SND,             /* 19 */
   MCI_TYPE_MMF,                /* 20 */
   MCI_TYPE_AU,                 /*  21 */
   MCI_TYPE_AIFF,                 /*  22 */
   MCI_TYPE_M4A,                  /* 23  */
   MCI_TYPE_3GP,                  /*  24 */
   MCI_TYPE_MP4,	                  /* 25 */ 
   MCI_TYPE_JPG,	                  /* 26 */
   MCI_TYPE_GIF,	                  /* 27 */
   MCI_TYPE_MJPG,	                  /* 28 */
   MCI_TYPE_WMA,                  /* 29 */
   MCI_TYPE_MIDI,                  /* 30 */
   MCI_TYPE_RM,			     /*31*/
   MCI_TYPE_AVSTRM,			/*32*/
   MCI_TYPE_SCO,			     /*33*/
   MCI_TYPE_GSM_ALL,                 /*34*/
   MCI_TYPE_PCM8_8K, /*35*/
   MCI_TYPE_MP3,
   MCI_NO_OF_TYPE
}
mci_type_enum;
// ============================================================================
// AUDIO_EQ
// ----------------------------------------------------------------------------
/// Enum describing the various equalizer modes
// ============================================================================
typedef enum
{
    NORMAL,   /*EQ=0*/
    BASS,     /*EQ=1*/
    DANCE,    /*EQ=2*/
    CLASSICAL,/*EQ=3*/
    TREBLE,   /*EQ=4*/
    PARTY,    /*EQ=5*/
    POP,      /*EQ=6*/
    ROCK,     /*EQ=7*/
    AUDIO_EQ_NUM
} AUDIO_EQ;

// ============================================================================
// MCI_AUDIO_PATH_T
// ----------------------------------------------------------------------------
/// Enum describing the various audio paths
// ============================================================================
typedef enum {
	AUDIO_DEVICE_NONE = 0,
	AUDIO_DEVICE_OUT_RECEIVER = 1,	
	AUDIO_DEVICE_OUT_SPEAKER = 2,
	AUDIO_DEVICE_OUT_WIRED_HEADSET = 4,
	AUDIO_DEVICE_OUT_WIRED_HEADPHONE = 8,
	MCI_PATH_NORMAL = AUDIO_DEVICE_OUT_RECEIVER,
	MCI_PATH_LSP = AUDIO_DEVICE_OUT_SPEAKER,
	MCI_PATH_HS = AUDIO_DEVICE_OUT_WIRED_HEADSET,
	MCI_PATH_HP = AUDIO_DEVICE_OUT_WIRED_HEADPHONE,
	MCI_PATH_FM_HP = 16,
	MCI_PATH_FM_LSP_HP = 32,
	MCI_PATH_LSP_AND_HP = 32+1,
	MCI_PATH_BT,
	MCI_PATH_TV,
	MCI_PATH_QTY,
} MCI_AUDIO_PATH_T;

typedef enum {
	AUDIO_STREAM_DEFUALT = -1,
	AUDIO_STREAM_MIN = 0,
	AUDIO_STREAM_MUSIC = 0,
	AUDIO_STREAM_NOTIFICATION = 1,
	AUDIO_STREAM_VOICE_CALL = 2,
	AUDIO_STREAM_RING = AUDIO_STREAM_NOTIFICATION,
	AUDIO_STREAM_ALARM = AUDIO_STREAM_NOTIFICATION,
	AUDIO_STREAM_FM = 3,
	AUDIO_STREAM_CNT = 4,
} audio_stream_type_t;

typedef enum {
	DEVICE_UNAVAILABLE,
	DEVICE_AVAILABLE,
} device_state_t;

typedef enum {
	AUDIO_MODE_INVALID = -1,
	AUDIO_MODE_NORMAL = 0,
	AUDIO_MODE_RINGTONG = 1,
	AUDIO_MODE_IN_CALL = 2,
}audio_mode_t;

// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
/// 
/// 
// =============================================================================

typedef enum
{
    STREAM_STATUS_REQUEST_DATA,
     STREAM_STATUS_NO_MORE_DATA,
       STREAM_STATUS_END,
    STREAM_STATUS_ERR,

} APBS_STREAM_STATUS_T;


typedef enum 
{
	MSG_MMC_AUDIODEC_VOC,
	MSG_MMC_AUDIODEC_PCM,
//	MSG_MMC_AUDIODEC_A2DP,
	MSG_MMC_AUDIODEC_SCO,
	
}AUDIODEC_USER_MSG;


typedef enum 
{
    MCI_ERR_NO,
    MCI_ERR_UNKNOWN_FORMAT,
    MCI_ERR_BUSY,
    MCI_ERR_INVALID_PARAMETER,
    MCI_ERR_ACTION_NOT_ALLOWED,
    MCI_ERR_OUT_OF_MEMORY,
    MCI_ERR_CANNOT_OPEN_FILE,         		           
    MCI_ERR_END_OF_FILE,	     
    MCI_ERR_TERMINATED,		   
    MCI_ERR_BAD_FORMAT,	      
    MCI_ERR_INVALID_FORMAT,   
    MCI_ERR_ERROR,	               	
} MCI_ERR_T;

typedef enum
{
    MCI_EVENT_INFO,
    MCI_EVENT_ERROR,
    MCI_EVENT_EOS,
} MCI_EVNET_T;
typedef enum
{
    MCI_INFO_NONE,
    MCI_INFO_ERROR,
	MCI_INFO_RECORDER_CUR_DURATION_MS,
	MCI_INFO_RECORDER_CUR_SIZE
} MCI_INFO_T;

typedef struct _PlayInfromation 
{
	INT32 PlayProgress;
	INT32 curPlayTimeMs;
} MCI_PlayInf;

typedef struct _ProgressInfromation 
{
	INT32 DurationTime;//ms
	INT32 BitRate;
	INT32 ID3Offset;
	INT32 reserve;
} MCI_ProgressInf;


// =============================================================================
// APBS_PLAY_MODE_T
// -----------------------------------------------------------------------------
/// This type describes the encoding mode used in a stream to play.
// =============================================================================

typedef enum
{
    MCI_PLAY_MODE_AMR475,
    MCI_PLAY_MODE_AMR515,
    MCI_PLAY_MODE_AMR59,
    MCI_PLAY_MODE_AMR67,
    MCI_PLAY_MODE_AMR74,
    MCI_PLAY_MODE_AMR795,
    MCI_PLAY_MODE_AMR102,
    MCI_PLAY_MODE_AMR122,
    MCI_PLAY_MODE_FR,
    MCI_PLAY_MODE_HR,
    MCI_PLAY_MODE_EFR,
    MCI_PLAY_MODE_PCM,
    // TODO Implement that mode
    MCI_PLAY_MODE_AMR_RING,
    MCI_PLAY_MODE_MP3,
    MCI_PLAY_MODE_AAC,
    MCI_PLAY_MODE_WAV,
    MCI_PLAY_MODE_MID,
    MCI_PLAY_MODE_STREAM_PCM, //for TTS stream play
    MCI_PLAY_MODE_QTY
    
} MCI_PLAY_MODE_T;

typedef enum
{
    MCI_AUDIO_DTMF_0 = 0,
    MCI_AUDIO_DTMF_1,
    MCI_AUDIO_DTMF_2,
    MCI_AUDIO_DTMF_3,
    MCI_AUDIO_DTMF_4,
    MCI_AUDIO_DTMF_5,
    MCI_AUDIO_DTMF_6,
    MCI_AUDIO_DTMF_7,
    MCI_AUDIO_DTMF_8,
    MCI_AUDIO_DTMF_9,
    MCI_AUDIO_DTMF_A,
    MCI_AUDIO_DTMF_B,
    MCI_AUDIO_DTMF_C,
    MCI_AUDIO_DTMF_D,
    MCI_AUDIO_DTMF_S,        //* key
    MCI_AUDIO_DTMF_P,        //# key
    MCI_AUDIO_COMFORT_425,
    MCI_AUDIO_COMFORT_950,
    MCI_AUDIO_COMFORT_1400,
    MCI_AUDIO_COMFORT_1800,
} MCI_AUDIO_TONE_TYPE_T;

typedef enum
{
    MCI_AUDIO_TONE_0DB = 0,
    MCI_AUDIO_TONE_M3DB,     // -3 dB
    MCI_AUDIO_TONE_M9DB,     // -9 dB
    MCI_AUDIO_TONE_M15DB,    // -15 dB
    MCI_AUDIO_TONE_QTY
} MCI_AUDIO_TONE_ATTENUATION_T;


// ============================================================================
// MCI_AUDIO_PLAY_CALLBACK_T
// ----------------------------------------------------------------------------
/// Callback function called when the service changes state
// ============================================================================

 typedef VOID (*MCI_AUDIO_PLAY_CALLBACK_T)(MCI_ERR_T result);
// ============================================================================
// Functions
// ============================================================================

// ============================================================================
// MCI_AudioPlay
// ----------------------------------------------------------------------------
/// Play the selected file on selected output
/// @param OutputPath: Selects the audio path to use
/// @param filename: Unix absolute filename
/// @param callback: Callback function called when the state changes (e.g EOF)
///	@playprogress: [0,10000]. If set 0, play from the head; 
///				   set to 10000 ,jump to the end of the file.
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_UNKNOWN_FORMAT, MCI_ERR_NO
// ============================================================================

// MCI_MEDIA_PLAY_REQ,
UINT32 MCI_AudioPlay(INT32 OutputPath,HANDLE fileHandle,mci_type_enum fielType,MCI_AUDIO_PLAY_CALLBACK_T callback,INT32 PlayProgress);
UINT32 MCI_AudioPlayLoop(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress, UINT8 loop);
UINT32 MCI_AudioPlayWithType(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress, audio_stream_type_t type);
UINT32 MCI_AudioPlayWithOffloadMode(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress);
UINT32 MCI_AudioPlayWithBtConnected(INT32 OutputPath, HANDLE fileHandle, mci_type_enum fielType, MCI_AUDIO_PLAY_CALLBACK_T callback, INT32 PlayProgress);


// ============================================================================
// MCI_AudioPause
// ----------------------------------------------------------------------------
/// Pause the current stream
/// @param none
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioPause(VOID);

// ============================================================================
// MCI_AudioSeekTo
// ----------------------------------------------------------------------------
/// seek to position
/// @param seekMs: : ms
/// @return  MCI_ERR_NO success, others error
// ============================================================================
UINT32 MCI_AudioSeekTo(INT32 seekMs);

// ============================================================================
// MCI_AudioResume
// ----------------------------------------------------------------------------
/// Resume previoulsy paused stream
/// @param none
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_NO
// ============================================================================
// MCI_MEDIA_PLAY_REQ,
UINT32 MCI_AudioResume (HANDLE fileHandle);

// ============================================================================
// MCI_AudioStop
// ----------------------------------------------------------------------------
/// Stop current stream
/// @param none
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioStop (VOID);

// ============================================================================
// MCI_AudioSetEQ
// ----------------------------------------------------------------------------
/// select equalizer mode
/// @param EQMode : Equalizer mode
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioSetEQ(AUDIO_EQ EQMode);

// ============================================================================
// MCI_AudioSetOutputPath
// ----------------------------------------------------------------------------
/// Allows to change the audio path while playing
/// @param OutputPath : Selects the audio path to use
/// @param Mute : If Mute=1 the sound will be muted
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_NO
// ============================================================================
UINT32 MCI_AudioSetOutputPath(UINT16 OutputPath,UINT16 Mute);

// ============================================================================
// MCI_AudioSetVolume
// ----------------------------------------------------------------------------
/// Allows to change the audio volume
/// If no audio is currently played, the value will be saved and used for 
/// the next stream played
/// @param volume : Selects the audio volume
/// @return error code MCI_ERR_ACTION_NOT_ALLOWED, MCI_ERR_INVALID_PARAMETER,
/// MCI_ERR_NO
// ============================================================================

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioSetMute
Description: set mute during a voice conversation, the person answering the phone can not hear the sound. 

Parameters: 
			 onoff: 1 --- turn on mute
				    0 --- turn off mute
			
Return value: NONE 
--------------------------------------------------------------------------------------------*/
VOID MCI_AudioSetMute(BOOL onoff);

UINT32 MCI_AudioSetVolume(UINT16 volume);

UINT32 MCI_AudioGetPlayInformation(MCI_PlayInf* PlayInformation);

VOID MCI_AudioFinished(MCI_ERR_T result);
VOID MCI_RingFinished(MCI_ERR_T result);
VOID MCI_AudioRecordFinished(MCI_ERR_T result);

UINT32 MCI_AudioPlayTone(MCI_AUDIO_TONE_TYPE_T tone, MCI_AUDIO_TONE_ATTENUATION_T attenuation);
UINT32 MCI_AudioStopTone(MCI_AUDIO_TONE_TYPE_T tone, MCI_AUDIO_TONE_ATTENUATION_T attenuation);
UINT32 MCI_AudioPauseTone(VOID);
UINT32 MCI_AudioResumeTone(VOID);
UINT32 MCI_SetDeviceConnectionState(MCI_AUDIO_PATH_T device, device_state_t state);
UINT32 MCI_DeviceSetForceUse(MCI_AUDIO_PATH_T device);
UINT32 MCI_SetDeviceMute(MCI_AUDIO_PATH_T device);
UINT32 MCI_GetRoute();
UINT32 MCI_SetVoiceState(audio_mode_t mode);
BOOL MCI_AifSetPhoneStatus(unsigned int status);


#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */















typedef struct
{
  S8 *file_name;//[255];
  UINT16 display_width;
  UINT16 display_height;
  UINT16 image_clip_x1; /* clip window start x for clipping decoded image */
  UINT16 image_clip_x2; /* clip window end x for clipping decoded image */
  UINT16 image_clip_y1; /* clip window start y for clipping decoded image */
  UINT16 image_clip_y2; /* clip window end y for clipping decoded image */
  VOID*  image_buffer_p;
  UINT32 image_buffer_size;
  UINT8  media_mode;
  UINT8  media_type;
  UINT32 file_size;
  UINT16 seq_num;
  UINT8  blocking;
  UINT16 decoded_image_width; // decoded image width
} img_decode_req_struct;

typedef struct {
   UINT8   padA;
   UINT16  padB;
   INT16   result;
   UINT16  image_width;
   UINT16  image_height;
   UINT32  total_frame_num;
   UINT32  total_time;
   UINT32  current_time;
   UINT16  seq_num;
   UINT16  media_type;
   UINT16  aud_channel;
   UINT16  aud_sample_rate;
   UINT16  track;
   UINT16  audio_type;
}mci_vid_file_ready_ind_struct;

typedef struct {
    UINT8    padA;
    UINT16   padB;
    UINT16   result;
    UINT16   seq_num;
}
mci_vid_play_finish_ind_struct;

typedef struct {
    UINT8    padA;
    UINT16   padB;
    UINT16   result;
    UINT16   seq_num;
}
mci_vid_seek_done_ind_struct;

typedef struct {
    UINT8   padA;
    UINT16  padB;
    INT16   result;
    UINT16  seq_num;
}
mci_vid_record_finish_ind_struct;

typedef struct {
    UINT8   padA;
    UINT16  padB;
    UINT8   src_id;
    UINT8   result;
}
mci_aud_record_finish_ind_struct;

typedef enum{
    MBOX_ID_SERVER,
    MBOX_ID_APP,
    MBOX_ID_COCEC,
    //MBOX_ID_MAINCONTROL,
    MBOX_ID_AGENT,
    MBOX_ID_FSAPP,
    MBOX_ID_MMC,
    MBOX_ID_DI,
    MBOX_ID_IMAGE,
    MBOX_ID_NUM
}COSMBOXID;




#define MAX_AUD_DESCRIPTION     80
typedef struct tAudInfoDescription
{
   UINT32 time; // duration in sec
   UINT32 bitRate;
   UINT32 sampleRate;
   BOOL stereo;
   INT16 title[MAX_AUD_DESCRIPTION+1];
   INT16 artist[MAX_AUD_DESCRIPTION+1];
   INT16 album[MAX_AUD_DESCRIPTION+1];
   INT16 author[MAX_AUD_DESCRIPTION+1];
   INT16 copyright[MAX_AUD_DESCRIPTION+1];
   INT16 date[MAX_AUD_DESCRIPTION+1];

      union
   {
      struct
      {
         UINT8 CRC;
         UINT8 channel_mode;
         UINT8 bit_rate_index;
      } DAF;
   } details;
}AudDesInfoStruct;


typedef VOID (*MCI_AUDIO_BUFFER_PLAY_CALLBACK_T)(MCI_ERR_T result);
typedef VOID (*MCI_AUDIO_FILE_RECORD_CALLBACK_T)(MCI_EVNET_T event , MCI_INFO_T info_type, INT32 value);
/* feed data at 'data', feed size should NOT exceed 'size', the return code is the 'feed size' */
typedef int (*MCI_AUDIO_BUFFER_PLAY_DATA_CALLBACK_T)(unsigned int from_pos, unsigned char *data, unsigned int size);

typedef MCI_AUDIO_FILE_RECORD_CALLBACK_T MCI_FILE_RECORD_CALLBACK_T;

typedef VOID (*APBS_STREAM_USER_HANDLER_T)(APBS_STREAM_STATUS_T);
typedef VOID (*MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T)(unsigned char* buf_p,unsigned int len, unsigned int *record_len) ;


// Video
UINT32  MCI_VideoClose (VOID) ;  //MCI_VID_CLOSE_REQ
#if (CSW_EXTENDED_API_AUDIO_VIDEO==1)
UINT32  MCI_VideoOpenFile (INT32 OutputPath,UINT8* filename);
#else
/*
MCI_VideoOpenFile

OutputPath: audio path
open_audio: 1=play audio, 0=no
fhd: file handle or buffer address.
file_mode: 0 file mode, 1 buffer mode
data_Len: data length in fhd buffer when file_mode==1
type: media type

sheen
*/
UINT32  MCI_VideoOpenFile (INT32 OutputPath,UINT8 open_audio, HANDLE fhd, UINT8 file_mode, UINT32 data_Len, mci_type_enum type, VOID(*vid_play_finish_callback)(int32), VOID (*vid_draw_panel_callback)(uint32));   //MCI_VID_OPEN_FILE_REQ
#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */
VOID    MCI_VideoOpenFileInd (mci_vid_file_ready_ind_struct *vdoOpen)  ;   //MCI_VID_FILE_READY_IND;
UINT32  MCI_VideoPlay (UINT16 startX, UINT16 startY);   //MCI_VID_PLAY_REQ
UINT32  MCI_VideoPlayWithBtConnected (UINT16 startX, UINT16 startY);   //MCI_VID_PLAY_REQ
UINT32 MCI_VideoRestore(UINT16 startX, UINT16 startY, UINT32 RestorePositionTimeMs, BOOL paused);
VOID    MCI_VideoPlayInd (VOID) ;  // MCI_VID_PLAY_FINISH_IND
UINT32  MCI_VideoStop (VOID) ;   // MCI_VID_STOP_REQ
UINT32  MCI_VideoPause (VOID) ; //  MCI_VID_PAUSE_REQ
UINT32  MCI_VideoResume (VOID)  ;// MCI_VID_RESUME_REQ
UINT32  MCI_VideoSeek (long long playtime, INT32 time_mode, INT16 startX, INT16 startY)  ;// MCI_VID_SEEK_REQ
//JHH added for #32745 callback begin
INT32   MCI_VideoPlayTimerTrigger(VOID);
//JHH added for #32745 callback end
VOID    MCI_VideoSeekInd (VOID) ; // MCI_VID_SEEK_DONE_IND
UINT32  MCI_VideoSet (UINT16 ZoomWidth, UINT16 ZoomHeight, INT16 startX, INT16 startY, INT16 cutX, INT16 cutY, INT16 cutW, INT16 cutH, UINT16 Rotate)  ;// MCI_VID_SET_MODE_REQ, Rotate=0(no), 1(90clockwise), 2(270clockwise)
VOID  MCI_VideoGetInfo ( UINT16  *image_width, UINT16  *image_height, UINT32  *total_time, UINT16  *aud_channel, UINT16  *aud_sample_rate,UINT16  *track);  
UINT32  MCI_VideoGetPlayTime (VOID) ;

#ifdef BUILD_CAMERA_HAL
// Camera
/*
 *Summary:    MCI_CamPowerUp
              This function is used to power on
 *Parameters: camera_id  . 0:back camera 1:front camera
              cb  this pararam hasn't been used now, please set NULL
 *Retrun:	0      none err
            others err		
 */
UINT32 MCI_CamPowerUp (INT32  camera_id, void (*cb)(int32));   // MCI_CAM_POWER_UP_REQ


/*
 *Summary:    MCI_CamPowerDown
              This function is used to power down
 *Parameters: none
 *Retrun:	0      none err
            others err
 */
UINT32  MCI_CamPowerDown (VOID) ; //MCI_CAM_POWER_DOWN_REQ;
UINT32  MCI_CamPreviewOpen(CAM_PREVIEW_STRUCT* data);//MCI_CAM_PREVIEW_REQ
UINT32  MCI_CamPreviewClose (VOID) ; //MCI_CAM_STOP_REQ

/*
 *Summary:    MCI_CamCapture
              This function is used to take photo
 *Parameters: 
              data  the picture params
 *Retrun:	0      none err
            others err		
 */
UINT32  MCI_CamCapture (CAM_CAPTURE_STRUCT *data);    //MCI_CAM_CAPTURE_REQ

/*
*Summary:	 MCI_CamSavePhoto
			 This function is used to save the picture to filesystem
*Parameters: 
			 file handle
*Retrun:   0	  none err
		   others err	   
*/
INT32  MCI_CamSavePhoto (INT32 file_handle); //MCI_CAM_CAPTURE_REQ 
UINT32  MCI_CamSetPara(INT32 effectCode,INT32 value) ; // MCI_CAM_SET_PARAM_REQ
UINT32  MCI_CamPreviewPause (VOID);
UINT32  MCI_CamPreviewResume (VOID);
UINT32  MCI_CamPreviewSetAlpha (UINT32 value);
#else
#define MCI_CamPowerUp(x, y) 1003
#define MCI_CamPowerDown() 1003
#define MCI_CamPreviewOpen(x) 1003
#define MCI_CamPreviewClose() 1003
#define MCI_CamCapture(x) 1003
#define MCI_CamSavePhoto(x) 1003
#define MCI_CamSetPara(x,y) 1003
#define MCI_CamPreviewPause() 1003
#define MCI_CamPreviewResume() 1003
#define MCI_CamPreviewSetAlpha(x) 1003
#endif
INT32 MCI_CamMdOpen(INT32 sensLevel, VOID(* md_callback)(int32));
INT32 MCI_CamMdClose( VOID);
VOID MCI_CamDoMd( VOID);



// Image
UINT32  MCI_ImgDecode (img_decode_req_struct *imgPara);  // MCI_IMG_DECODE_REQ
UINT32  MCI_ImgDecodeEvenInd (VOID) ; // MCI_IMG_DECODE_EVENT_IND
UINT32  MCI_ImgDecodeFinishInd (VOID) ; //MCI_IMG_DECODE_FINISH_IND
UINT32  MCI_ImgResize (img_decode_req_struct *imgPara)  ;// MCI_IMG_RESIZE_REQ
UINT32  MCI_ImgStop (VOID) ; //MCI_IMG_STOP_REQ

// Init
BOOL    MCI_TaskInit(VOID);     //  MCI_MEDIA_PLAY_REQ,         
BOOL    MCI_LcdSizeSet(INT32 LcdWidth, INT32 LcdHeight);   
#ifdef DUAL_LCD
BOOL    MCI_SubLcdSizeSet(INT32 LcdWidth, INT32 LcdHeight);   
#endif
INT32  MMC_GetBVGA(INT32 bvga);
// Video Record
UINT32  MCI_VideoRecordAdjustSetting(INT32 adjustItem, INT32 value);
UINT32  MCI_VideoRecordPause(VOID);

/*
 *Summary:    MCI_VideoRecordPreviewStart
              This function is used to start preview, before start preview, you must have called the function "mci_LcdInit" and "MCI_CamPowerUp"
 *Parameters: previewPara the preview pararam, such as the start (x,y) point, the image size
              vid_rec_finish_ind  this pararam hasn't been used now, please set NULL
 *Retrun:	0      none err
            others err	  
 */
UINT32  MCI_VideoRecordPreviewStart(MMC_VDOREC_SETTING_STRUCT *previewPara, VOID(*vid_rec_finish_ind)(UINT16 msg_result));


/*
*Summary:	 MCI_VideoRecordPreviewStop
			 This function is used to stop preview
*Parameters: 
			 None
*Retrun:   0	  none err
		   others err	   
*/
UINT32  MCI_VideoRecordPreviewStop(VOID);
UINT32  MCI_VideoRecordResume(VOID);
UINT32  MCI_VideoRecordStart(HANDLE filehandle);
UINT32 MCI_VideoRecordStart2(HANDLE filehandle, MCI_FILE_RECORD_CALLBACK_T callback);
UINT32  MCI_VideoRecordStop(VOID);
UINT32  MCI_DisplayVideoInterface (VOID);

// MCI Task
HANDLE  GetMCITaskHandle(COSMBOXID mod);
BOOL    SetMCITaskHandle(COSMBOXID mod, HANDLE hTask);

INT32 MCI_AudioStreamingPlay(INT32 * pBuffer, UINT32 len, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_AUDIO_BUFFER_PLAY_DATA_CALLBACK_T data_callback);
INT32 MCI_AudioStopStreaming(void);
INT32  MCI_AudioPlayBuffer(INT32 *pBuffer, UINT32 len, UINT8 loop,MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback,MCI_PLAY_MODE_T  format,INT32 startPosition);
INT32 MCI_AudioPlayBufferWithType(INT32 * pBuffer, UINT32 len, UINT8 loop, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_PLAY_MODE_T format, INT32 startPosition, audio_stream_type_t type);
INT32 MCI_AudioPlayBufferWithBtConnected(INT32 * pBuffer, UINT32 len, UINT8 loop, MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback, MCI_PLAY_MODE_T format, INT32 startPosition);

INT32  MCI_AUD_StreamPlay( INT32 *pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback);
INT32  MCI_AUD_StreamPlayPCM( INT32 *pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback,UINT32 sampleRate, UINT32	bitPerSample);

INT32  MCI_AudioStopBuffer(VOID);
UINT32 MCI_AudioPlayPause(VOID);
UINT32 MCI_AudioPlayResume(VOID);
UINT32* MCI_GetBufPosition(VOID);

void MCI_GetWriteBuffer( UINT32 **buffer, UINT32 *buf_len );
UINT32 MCI_GetRemain(VOID);
void MCI_DataFinished( void );
INT32 MCI_AddedData(UINT32 addedDataBytes);

void  MCI_SetBuffer(UINT32 *buffer, uint32 buf_len);

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioRecordStart
Description: start the audio recorder

Parameters: fhd: 		  file handle for the recorder file to be written
			format: 	  MCI_TYPE_AMR, MCI_TYPE_PCM_8K, MCI_TYPE_PCM_16K; 
						  Not realized: MCI_TYPE_AMR_WB
			quality:   	  No Affect, assign a value of 0
			callback:     No Affect, assign a value of NULL
			usercallback: No Affect, assign a value of NULL
						  
			
Return value: If start success, return MCI_ERR_NO
--------------------------------------------------------------------------------------------*/
extern UINT32 MCI_AudioRecordStart (HANDLE fhd,mci_type_enum format,U8 quality,MCI_AUDIO_FILE_RECORD_CALLBACK_T callback,  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback);


/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioRecordPause
Description: pause the started audio recorder

Parameters:  void
						  			
Return value: If pause success, return MCI_ERR_NO
--------------------------------------------------------------------------------------------*/
extern UINT32 MCI_AudioRecordPause(VOID);

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioRecordResume
Description: resume the paused audio recorder

Parameters:  void
						  			
Return value: If resume success, return MCI_ERR_NO
--------------------------------------------------------------------------------------------*/
extern UINT32 MCI_AudioRecordResume(VOID);

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioRecordStop
Description: stop the audio recorder

Parameters:  void
						  			
Return value: If stop success, return MCI_ERR_NO
--------------------------------------------------------------------------------------------*/
extern UINT32 MCI_AudioRecordStop(VOID);

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioBufferRecordStart
Description: start the audio recorder, and store it into buffer.

Parameters: pBuf: 		  buffer for recorder data to be written
            bufSize:      buffer size of 'pBuf'
            maxDuration:  expected maximum recording duration in millisecond.
                          when set to -1, it means callback mode, recording data
                          will send to user by callback "usercallback"
			format: 	  MCI_TYPE_AMR, MCI_TYPE_PCM_8K, MCI_TYPE_PCM8_8K; 
						  Not realized: MCI_TYPE_AMR_WB
			quality:   	  No Affect, assign a value of 0
			callback:     callback for recorder event, such EOS, ERROR etc.
			usercallback: callback for receiving recording data.
						  
			
Return value: If start success, return MCI_ERR_NO
*/

UINT32 MCI_AudioBufferRecordStart(UINT8 *pBuf, UINT32 bufSize, UINT32 maxDurationMs, mci_type_enum format, U8 quality, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback, MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback);

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioBufferRecordStop
Description: stop the audio recorder

Parameters:  recSize: recording data size.
             durationMs: recording data durtion.(in millisecond).
						  			
Return value: If stop success, return MCI_ERR_NO
*/
UINT32 MCI_AudioBufferRecordStop(UINT32 *recSize, UINT32 *durationMs);



/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioRecordGetRecordInfo
Description: get the audio record file's duration time

Parameters: duration_ms: the address which to store the duration time, the unit :ms

Return value: If success, return MCI_ERR_NO
--------------------------------------------------------------------------------------------*/
extern UINT32 MCI_AudioRecordGetRecordInfo(UINT32 *duration_ms);

/*-------------------------------------------------------------------------------------------
Function:    MCI_AudioRecordGetRecordsize
Description: get the audio record file's size

Parameters: size: the address which to store the file size, the unit :bytes

Return value: If success, return MCI_ERR_NO
--------------------------------------------------------------------------------------------*/
extern UINT32 MCI_AudioRecordGetRecordsize(UINT32 *size);

typedef struct
{

	/// X coordinate of the top left corner of the region.	 
	UINT16 StartX;

	/// Y coordinate of the top left corner of the region.
	UINT16 StartY;
	
	/// Width of the region.
	UINT16 ImageWidth;
	
	/// Height of the region.
	UINT16 ImageHeight;

	/// flag of rotate.  1 stand for rotating, o stand for no rotating. 
	UINT16 Rotate;
	
}MCI_ANALOG_TV_PREVIEW_CFG_T;

// Audio Streaming Sender

typedef struct sender_stream_handle SSHdl;
struct sender_stream_handle {
   VOID (*PutData)( SSHdl *hdl, UINT8 *, UINT32 );
   VOID (*Flush)( SSHdl *hdl );

   UINT8 *rb_base;
   INT32 rb_size;
   INT32 rb_read;
   INT32 rb_write;
   INT32 rb_threshold;
   VOID (*callback)( VOID * );
   BOOL wait;
   INT32 rb_read_total;
   /*
   BOOL      overflow;
   INT32      overflow_size;
   UINT8 *overflow_buf; */
} ;

typedef struct {
   UINT32 Emphasis: 2;
   UINT32 Original: 1;
   UINT32 Copyright: 1;
   UINT32 ModeExt: 2;
   UINT32 ChannelMode: 2; // fixed
   UINT32 Private: 1;
   UINT32 Padding: 1;
   UINT32 SampleRateIndex: 2; // fixed
   UINT32 BitRateIndex: 4;
   UINT32 Protection: 1;
   UINT32 Layer: 2; // fixed
   UINT32 VersionID: 2; // fixed
   UINT32 Sync: 11; // fixed
} dafHeaderStruct;

typedef struct {
   SSHdl ss_handle;
   dafHeaderStruct config_header;
   UINT32 last_frame_size;
   UINT32 last_bit_rate;
   UINT32 fragment_start;
} dafRTPSSHdlInternal;





/* SBC event */
typedef enum{
   A2DP_DATA_NOTIFY
} A2DP_Event;



typedef VOID (*A2DP_Callback)(A2DP_Event , VOID*);

typedef struct{
    UINT32 (*GetPayload)(UINT8 *, UINT32, UINT32 *);
    VOID (*GetPayloadDone)(VOID);
    VOID (*QueryPayloadSize)(UINT32 *, UINT32 *);
    UINT32 (*AdjustBitRateFromQos)(UINT8); // return adjusted bit rate
    UINT32 (*SetBitRate)(UINT32); // return adjusted bit rate
    UINT8 state;
} A2DP_codec_struct;





#ifndef __BT_A2DP_CODEC_TYPES__
#define __BT_A2DP_CODEC_TYPES__

typedef struct
{
	UINT8 min_bit_pool;
	UINT8 max_bit_pool;
	UINT8 block_len; // b0: 16, b1: 12, b2: 8, b3: 4
	UINT8 subband_num; // b0: 8, b1: 4
	UINT8 alloc_method; // b0: loudness, b1: SNR
	UINT8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 16000
	UINT8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
} bt_a2dp_sbc_codec_cap_struct;



typedef struct
{
	UINT8 layer; // b0: layerIII, b1: layerII, b0: layerI
	BOOL CRC;
	UINT8 channel_mode; // b0: joint stereo, b1: stereo, b2: dual channel, b3: mono
	BOOL MPF; // is support MPF-2
	UINT8 sample_rate; // b0: 48000, b1: 44100, b2: 32000, b3: 24000, b4: 22050, b5: 16000
	BOOL VBR; // is support VBR
	UINT16 bit_rate; // bit-rate index in ISO 11172-3 , b0:0000 ~ b14: 1110
} bt_a2dp_mp3_codec_cap_struct; /* all MPEG-1,2 Audio */




typedef struct
{
	UINT8 object_type; // b4: M4-scalable, b5: M4-LTP, b6: M4-LC, b7: M2-LC
	UINT16 sample_rate; // b0~b11: 96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000
	UINT8 channels; // b0: 2, b1: 1
	BOOL VBR; // is supported VBR
	UINT32 bit_rate; // constant/peak bits per second in 23 bit UiMsbf, 0:unknown
} bt_a2dp_aac_codec_cap_struct; /* all MPEG-2,4 AAC */

typedef struct
{
	UINT8 version; // 1:ATRAC, 2:ATRAC2, 3:ATRAC3
	UINT8 channel_mode; // b0: joint stereo, b1: dual, b2: single
	UINT8 sample_rate; // b0: 48000, b1: 44100
	BOOL VBR; // is supported VBR
	UINT32 bit_rate; // bit-rate index in ATRAC, b0: 0x0012 ~ b18: 0x0000
	UINT16 max_sul; // sound unit length in 16 bits UiMsbf
} bt_a2dp_atrac_codec_cap_struct; /* all ATRAC family */

typedef union
{
	bt_a2dp_sbc_codec_cap_struct sbc;
	bt_a2dp_mp3_codec_cap_struct mp3;
//	bt_a2dp_aac_codec_cap_struct aac;
//	bt_a2dp_atrac_codec_cap_struct atrac;
} bt_a2dp_audio_codec_cap_struct;

typedef struct
{
	UINT8 codec_type; // SBC, MP3
	bt_a2dp_audio_codec_cap_struct codec_cap;
} bt_a2dp_audio_cap_struct;



#endif /* _MED_STRUCT_H */

BOOL is_SSHdl_valid(SSHdl *hdl);


MCI_ERR_T MCI_AudioGetFileInformation (CONST HANDLE FileHander,
											AudDesInfoStruct  * CONST DecInfo,
											CONST mci_type_enum FileType   );


UINT32 MCI_AudioGetDurationTime(HANDLE fileHandle, 
 								mci_type_enum fielType,
 								INT32 BeginPlayProgress,
 								INT32 OffsetPlayProgress,
 								MCI_ProgressInf* PlayInformation);

A2DP_codec_struct *MCI_A2dpDafOpen(  A2DP_Callback pHandler, 
									    bt_a2dp_audio_cap_struct *daf_config_data, 
									    UINT8 *buf, UINT32 buf_len);

VOID MCI_A2dpDafClose( void);

A2DP_codec_struct *MCI_SBCOpen(   A2DP_Callback pHandler, 
									    bt_a2dp_audio_cap_struct *daf_config_data, 
									    UINT8 *buf, UINT32 buf_len);

VOID MCI_SBCClose( void);

VOID MCI_FmSetOutputDevice(MCI_AUDIO_PATH_T device);

UINT32 MCI_FmPlay(VOID);

UINT32 MCI_FmStop(VOID);

UINT32 MCI_FmSetup(UINT8 volume );

#ifdef AUD_3_IN_1_SPK

extern volatile INT16 g_MCIAudioVibrator;


VOID MCI_AudioVibratorStart(VOID);

VOID MCI_AudioVibratorStop(VOID);

#endif


#define SBC_Out_put_addr                            			 (7916 + 0x0000) //256 short  sbc_output_addr
#define OUTPUT_PCM_BUFFER1_ADDR                 		 (3244 + 0)



typedef VOID (*PNGDEC_USER_HANDLER_T)(INT32 value);

//struct of output parameters
typedef struct
{
    // ----------------
    // common status
    // ----------------
	INT16 mode;
	INT16 errorStatus;

    // ----------------
    // pngdec status
    // ----------------
  	INT32 PngdecFinished;



} MCI_PNGDEC_STATUS_T;


INT32 MCI_PngdecStart(UINT32 *FileContent,
			  UINT32 FileSize,
			  UINT32 *PNGOutBuffer,         
			  UINT32 OutBufferSize,     
			  UINT32 *AlphaOutBuffer,	
			  UINT32 AlphaBufferSize,
			  UINT32 *BkgrdBuffer,		
			  UINT16 BkgrdClipX1,	
			  UINT16 BkgrdClipX2,	
			  UINT16 BkgrdClipY1,	
			  UINT16 BkgrdClipY2,	
			  INT16 BkgrdOffsetX, 
			  INT16 BkgrdOffsetY, 
			  UINT16 BkgrdWidth,
			  UINT16 BkgrdHeight,	
			  UINT16 ZoomWidth,	
			  UINT16 ZoomHeight,	
			  UINT16 DecMode,
			  UINT16 alpha_blending_to_transparentColor,
			  UINT32 *ImgWidth,
			  UINT32 *ImgHeigh ,			  
			  UINT32*WindowBuf,
       		 UINT32 *rgb_temp_ptr,
       		 UINT32 PngFileHandle,
			  PNGDEC_USER_HANDLER_T handle
			  
			  );

BOOL MCI_PngdecGetDecodingStatus(MCI_PNGDEC_STATUS_T *status_png);

#ifdef CONFIG_MMI_SUPPORT_BLUETOOTH
BOOL MCI_init_btVolLevel_table(unsigned char max_level, unsigned int step_level);
//volume:0~0x7f, 0-minial bt volume=0; 0x7f-max bt volume.
UINT32 MCI_AudioSetVolume_Bt(unsigned char volume);
#endif

#ifdef __cplusplus
}
#endif

#endif // _MCI_H__

