/*------------------------------------------------------------
(C) Copyright [2018-2020] ASR Ltd.
All Rights Reserved
------------------------------------------------------------*/
#ifndef __GUI_CP_INTERNEL_HEADER_FILE__
#error "Please don't include this file directly, please include cp_include.h"
#endif

#ifndef __CP_AUDIO_HAL_API_H__
#define __CP_AUDIO_HAL_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************
	CP audio local enumerations
*******************************************************************/
//ICAT EXPORTED ENUM
typedef enum{
    HEAD_PHONE_INIT = 0,
    HEAD_PHONE_OUT,		// 1
    HEAD_TYPE_UNKNOWN,	// 2
    HEAD_PHONE_TYPE3,	// 3
    HEAD_PHONE_TYPE4,	// 4

    HEAD_PHONE_ERROR = 0xFFFF
}HEADPHONE_STATE;


//ICAT EXPORTED ENUM
typedef enum{
    HOOK_KEY_EVENT_NULL = 0,
    VOL_UP_PRESS,		// 1
    VOL_DOWN_PRESS,		// 2
    HOOK_KEY_PRESS,		// 3
    VOL_UP_RELEASE,		// 4
    VOL_DOWN_RELEASE,	// 5
    HOOK_KEY_RELEASE,	// 6

    HEADPHONE_EVENT_ERROR = 0xFFFF    
}HEADPHONE_EVENT;


//AudioHAL task flags:
#define AUDIO_RESAMPLE_FLAGS_PLAYBACK   (0x01)
#define AUDIO_RESAMPLE_FLAGS_RECORD     (0x02)
#define AUDIO_HEADSET_FLAG              (0x04)
#define AUDIO_VOICE_CALL_START          (0x10)
#define AUDIO_VOICE_CALL_END            (0x20)
#define AUDIO_MEDIA_STOP                (0x40)
#define AUDIO_MEDIA_START               (0x80)

#define AUDIO_GETVOICE_EVENT       		(0x100)
#define AUDIO_PMIC_CHECK                (0x200)
#define AUDIO_POC_DATA_EVENT            (0x400)
#define AUDIO_DTMFDETECTION_EVENT       (0x800)


#define AUDIOHAL_FLAGS_MASK    (AUDIO_RESAMPLE_FLAGS_PLAYBACK\
                                | AUDIO_RESAMPLE_FLAGS_RECORD\
                                | AUDIO_HEADSET_FLAG\
                                | AUDIO_VOICE_CALL_END\
                                | AUDIO_VOICE_CALL_START\
                                | AUDIO_MEDIA_START\
                                | AUDIO_MEDIA_STOP\
                                | AUDIO_PMIC_CHECK\
                                | AUDIO_POC_DATA_EVENT\
                                | AUDIO_DTMFDETECTION_EVENT\
                                | AUDIO_GETVOICE_EVENT)                                




/******************************************************************
	Enumerations shared with upper layer
*******************************************************************/
#define AUDIOHAL_VOLCNT	9		//0~8

//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_ERR_NO = 0,  //No error
    AUDIOHAL_ERR_RESOURCE_RESET,
    AUDIOHAL_ERR_RESOURCE_BUSY,
    AUDIOHAL_ERR_RESOURCE_TIMEOUT,
    AUDIOHAL_ERR_RESOURCE_NOT_ENABLED,
    AUDIOHAL_ERR_BAD_PARAMETER,
    
    AUDIOHAL_ERR_UART_RX_OVERFLOW,
    AUDIOHAL_ERR_UART_TX_OVERFLOW,
    AUDIOHAL_ERR_UART_PARITY,
    AUDIOHAL_ERR_UART_FRAMING,
    AUDIOHAL_ERR_UART_BREAK_INT,

    AUDIOHAL_ERR_TIM_RTC_NOT_VALID,
    AUDIOHAL_ERR_TIM_RTC_ALARM_NOT_ENABLED,
    AUDIOHAL_ERR_TIM_RTC_ALARM_NOT_DISABLED,

    AUDIOHAL_ERR_COMMUNICATION_FAILED,
    
    /* Must be at the end */
    AUDIOHAL_ERR_QTY,


	AUDIOHAL_ERR_ENUM_32_BIT		    	= 0x7FFFFFFF //32bit enum compiling enforcement
} AUDIOHAL_ERR_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_ITF_RECEIVER         = 0,
    AUDIOHAL_ITF_EARPIECE, 
    AUDIOHAL_ITF_HEADPHONE = AUDIOHAL_ITF_EARPIECE,
    AUDIOHAL_ITF_LOUDSPEAKER,
    AUDIOHAL_ITF_LOUDSPEAKER_AND_HEADPHONE,
    //AUDIOHAL_ITF_LOUDSPEAKER_AND_HEADPHONE = AUDIOHAL_ITF_LOUDSPEAKER_AND_EARPIECE,
    AUDIOHAL_ITF_BLUETOOTH,
    AUDIOHAL_ITF_FM,
    AUDIOHAL_ITF_FM2SPK,
    AUDIOHAL_ITF_TV,
    AUDIOHAL_ITF_BLUETOOTH_WB,

    AUDIOHAL_ITF_QTY,
    AUDIOHAL_ITF_NONE = 255,
} AUDIOHAL_ITF_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_SPK_RECEIVER         = 0,
    AUDIOHAL_SPK_EARPIECE,
    AUDIOHAL_SPK_LOUDSPEAKER,
    AUDIOHAL_SPK_LOUDSPEAKER_EARPIECE,  //Output on both hands-free loud speaker and earpiece

    AUDIOHAL_SPK_QTY,
    AUDIOHAL_SPK_DISABLE = 255,
} AUDIOHAL_SPK_T;

//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_SPEAKER_STEREO         = 0,
    AUDIOHAL_SPEAKER_MONO_RIGHT,
    AUDIOHAL_SPEAKER_MONO_LEFT,
    AUDIOHAL_SPEAKER_STEREO_NA,  //Output is mono only

    AUDIOHAL_SPEAKER_QTY,
    AUDIOHAL_SPEAKER_DISABLE = 255,
} AUDIOHAL_SPEAKER_TYPE_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_MIC_RECEIVER         = 0,
    AUDIOHAL_MIC_EARPIECE,
    AUDIOHAL_MIC_LOUDSPEAKER,

    AUDIOHAL_MIC_QTY,
    AUDIOHAL_MIC_DISABLE = 255,
} AUDIOHAL_MIC_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_SPK_MUTE         = 0,
    AUDIOHAL_SPK_VOL_1,
    AUDIOHAL_SPK_VOL_2,
    AUDIOHAL_SPK_VOL_3,
    AUDIOHAL_SPK_VOL_4,
    AUDIOHAL_SPK_VOL_5,
    AUDIOHAL_SPK_VOL_6,
    AUDIOHAL_SPK_VOL_7,
    AUDIOHAL_SPK_VOL_8,
    AUDIOHAL_SPK_VOL_9,
    AUDIOHAL_SPK_VOL_10,
    AUDIOHAL_SPK_VOL_11,	// 11

	AUDIOHAL_SPK_VOL_QTY	// 12
} AUDIOHAL_SPK_LEVEL_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_MIC_MUTE         = 0,
    AUDIOHAL_MIC_ENABLE,

    AUDIOHAL_MIC_VOL_QTY,
} AUDIOHAL_MIC_LEVEL_T;

//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_SIDE_MUTE         = 0,
    AUDIOHAL_SIDE_VOL_1,
    AUDIOHAL_SIDE_VOL_2,
    AUDIOHAL_SIDE_VOL_3,
    AUDIOHAL_SIDE_VOL_4,
    AUDIOHAL_SIDE_VOL_5,
    AUDIOHAL_SIDE_VOL_6,
    AUDIOHAL_SIDE_VOL_7,
    AUDIOHAL_SIDE_VOL_TEST,
    AUDIOHAL_SIDE_VOL_QTY,
} AUDIOHAL_SIDE_LEVEL_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_FREQ_8000HZ  =  8000,
    AUDIOHAL_FREQ_11025HZ = 11025,
    AUDIOHAL_FREQ_12000HZ = 12000,
    AUDIOHAL_FREQ_16000HZ = 16000,
    AUDIOHAL_FREQ_22050HZ = 22050,
    AUDIOHAL_FREQ_24000HZ = 24000,
    AUDIOHAL_FREQ_32000HZ = 32000,
    AUDIOHAL_FREQ_44100HZ = 44100,
    AUDIOHAL_FREQ_48000HZ = 48000,    
    AUDIOHAL_FREQ_64000HZ = 64000,    
    AUDIOHAL_FREQ_88200HZ = 88200,    
    AUDIOHAL_FREQ_96000HZ = 96000,   
} AUDIOHAL_FREQ_T;

//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_MONO         = 1,
    AUDIOHAL_STEREO       = 2,
} AUDIOHAL_CH_NB_T;

//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_DTMF_0 = 0,
    AUDIOHAL_DTMF_1,
    AUDIOHAL_DTMF_2,
    AUDIOHAL_DTMF_3,
    AUDIOHAL_DTMF_4,
    AUDIOHAL_DTMF_5,
    AUDIOHAL_DTMF_6,
    AUDIOHAL_DTMF_7,
    AUDIOHAL_DTMF_8,
    AUDIOHAL_DTMF_9,
    AUDIOHAL_DTMF_A,
    AUDIOHAL_DTMF_B,
    AUDIOHAL_DTMF_C,
    AUDIOHAL_DTMF_D,
    AUDIOHAL_DTMF_S,        //* key
    AUDIOHAL_DTMF_P,        //# key
    AUDIOHAL_COMFORT_425,
    AUDIOHAL_COMFORT_950,
    AUDIOHAL_COMFORT_1400,
    AUDIOHAL_COMFORT_1800,
} AUDIOHAL_TONE_TYPE_T;


//ICAT EXPORTED ENUM
typedef enum
{
    AUDIOHAL_TONE_0DB = 0,
    AUDIOHAL_TONE_M3DB,     // -3 dB
    AUDIOHAL_TONE_M9DB,     // -9 dB
    AUDIOHAL_TONE_M15DB,    // -15 dB
    AUDIOHAL_TONE_QTY
} AUDIOHAL_TONE_ATTENUATION_T;


//ICAT EXPORTED ENUM
typedef enum{
    AUDIOHAL_HEADPHONE_PLUG_OUT = 0,
    AUDIOHAL_HEADPHONE_PLUG_IN	= 1
}AUDIOHAL_HEADPHONE_PLUG_T;


//ICAT EXPORTED ENUM
typedef enum{
    AUDIOHAL_HEADPHONE_TYPE_NULL 	= 0,
	AUDIOHAL_HEADPHONE_TYPE_UNKNOWN = 1,

    AUDIOHAL_HEADPHONE_TYPE_3 		= 3,	//HEAD_PHONE_TYPE3
    AUDIOHAL_HEADPHONE_TYPE_4 		= 4		//HEAD_PHONE_TYPE4
}AUDIOHAL_HEADPHONE_TYPE_T;


//ICAT EXPORTED ENUM
typedef enum{
    AUDIOHAL_HEADPHONE_EVENT_NULL		= 0,
    AUDIOHAL_HEADPHONE_EVENT_VOLUME_UP 	= 1,
    AUDIOHAL_HEADPHONE_EVENT_VOLUME_DOWN= 2,
    AUDIOHAL_HEADPHONE_EVENT_HOOK_KEY	= 3
}AUDIOHAL_HEADPHONE_EVENT_T;



typedef void (*AUDIOHAL_HANDLER_T) (void);
typedef void (*AUDIOHAL_HeadsetReport_T) (UINT32 plug, UINT32 type, UINT32 event);
typedef void (*AUDIOHAL_SpeakerPA_T) (UINT32 on);
typedef void (*AUDIOHAL_Codec_T) (UINT32 on);

//ICAT EXPORTED STRUCT
typedef struct {
    UINT32 *            startAddress;
    UINT16              length;

    AUDIOHAL_FREQ_T     sampleRate;
    AUDIOHAL_CH_NB_T    channelNb;
    //BOOL                voiceQuality;
    //BOOL                playSyncWithRecord;
    INT                voiceQuality;
    INT                playSyncWithRecord;

    AUDIOHAL_HANDLER_T  halfHandler;
    AUDIOHAL_HANDLER_T  endHandler;
} AUDIOHAL_STREAM_T;

typedef struct {
	BOOL				isVoice;
	BOOL				isMaster;
    AUDIOHAL_FREQ_T     samplerate;
    AUDIOHAL_CH_NB_T    channelNb;
} AUDIOHAL_AIF_CONFIG;


typedef struct {
    AUDIOHAL_SPK_LEVEL_T        spkLevel;
    AUDIOHAL_MIC_LEVEL_T        micLevel;
    AUDIOHAL_SIDE_LEVEL_T       sideLevel;
    AUDIOHAL_TONE_ATTENUATION_T toneLevel;
} AUDIOHAL_AIF_LEVEL_T;


typedef struct {
    AUDIOHAL_SPK_T          spkSel;
    AUDIOHAL_SPEAKER_TYPE_T spkType;    //Kind of speaker(stereo/mono/etc)
    AUDIOHAL_MIC_T          micSel;
    AUDIOHAL_AIF_LEVEL_T  * level;
} AUDIOHAL_AIF_DEVICE_CFG_T;




///////////////////////Control API for MMI///////////////////////////
#if 0
typedef union
{
    UINT32 reg;
    struct
    {
        BOOL auxMicSel:1;
        BOOL spkSel:1;
        BOOL spkmonoSel:1;
        BOOL rcvSel:1;
        BOOL headSel:1;
        BOOL fmSel:1;
    };
} AUDIOHAL_AUD_SEL_T;

typedef union
{
    UINT32 reg;
    struct
    {
        UINT32  MicGain:4;
        BOOL    MicMute:1;
    };
} AUDIOHAL_MIC_CFG_T;


typedef union
{
    UINT32 reg;
    struct
    {
        UINT32  SpkGain:4;
        BOOL    SpkMute:1;
    };
} AUDIOHAL_SPK_CFG_T;

BOOL AudioHAL_AudOpen(unsigned int user);
BOOL AudioHAL_AudClose(void);
BOOL AudioHAL_AudEarpieceDetectInit(void);
void AudioHAL_AudForceReceiverMicSelection(BOOL on);
void AudioHAL_AudForcePowerDown(void);
void AudioHAL_AudSetVoiceMode(BOOL isVoiceMode);
void AudioHAL_AudSetAdcVolume(UINT32 vol);
void AudioHAL_AudSetDacVolume(UINT32 vol);
void AudioHAL_AudSetHeadGain(UINT32 gain);
void AudioHAL_AudSetRcvGain(UINT32 gain);
void AudioHAL_AudSetMicCfg(UINT32 cfg);
void AudioHAL_AudSetSpkCfg(UINT32 cfg);
void AudioHAL_AudSetSelcfg(UINT32 selCfg);
AUDIOHAL_SPK_CFG_T AudioHAL_AudGetSpkCfg(void);
UINT32 AudioHAL_AudGetHeadGain(void);
UINT32 AudioHAL_AudGetRcvGain(void);
BOOL AudioHAL_AudMuteOutput(BOOL mute);
void AudioHAL_AudLcdPowerOnCallback(void);
void AudioHAL_AudLcdPowerOffCallback(void);
#endif


///////////////////////Control API for CP Audio///////////////////////////
int AudioHAL_Init(void);
void AudioHAL_TaskInit(void);
void AudioHAL_ReloadFDI(void);
int AudioHAL_UpdateCodecGain(void);
void AudioHAL_HeadsetOps(UINT16 plug, UINT16 state, UINT16 event);
void AudioHAL_StreamStart(BOOL onoff);



///////////////////////Control API for MMI///////////////////////////
AUDIOHAL_ERR_T AudioHAL_AifLoopBack(BOOL loop);
AUDIOHAL_ERR_T AudioHAL_AifConfigure(AUDIOHAL_AIF_CONFIG * config);
AUDIOHAL_ERR_T AudioHAL_AifOpen(AUDIOHAL_ITF_T itf, AUDIOHAL_AIF_DEVICE_CFG_T *config);
AUDIOHAL_ERR_T AudioHAL_AifTone(AUDIOHAL_TONE_TYPE_T tone, AUDIOHAL_TONE_ATTENUATION_T attenuation, BOOL start);
AUDIOHAL_ERR_T AudioHAL_AifTonePause(BOOL pause);
AUDIOHAL_ERR_T AudioHAL_AifDTMF(BOOL start, int f1, int f2);


void AudioHAL_AifClose(void);
void AudioHAL_AifHeadsetDetection(BOOL onoff);
void AudioHAL_AifBindHeadsetDetectionCB(AUDIOHAL_HeadsetReport_T cb);
void AudioHAL_AifBindSpeakerPA_CB(AUDIOHAL_SpeakerPA_T cb);
void AudioHAL_AifBindCodec_CB(AUDIOHAL_SpeakerPA_T cb);
void AudioHAL_AifGetHeadsetInfo(UINT32* plug, UINT32* type);
void AudioHAL_AifSetVolume(UINT32 vol);
void AudioHAL_AifSetMute(BOOL onoff);
void AudioHAL_AifSetSideTone(UINT32 vol);
void AudioHAL_AifSwitch(AUDIOHAL_ITF_T itf);

UINT32 AudioHAL_AifGetVolume(void);
AUDIOHAL_ITF_T AudioHAL_AifGetOpenedDevice(void);



///////////////////////Stream API for MMI///////////////////////////
AUDIOHAL_ERR_T AUDIOHAL_GetBufferSize(AUDIOHAL_FREQ_T rate, AUDIOHAL_CH_NB_T chan, int* min, int* max);
AUDIOHAL_ERR_T AudioHAL_AifPlayStream(AUDIOHAL_STREAM_T *playedStream);
AUDIOHAL_ERR_T AudioHAL_AifRecordStream(AUDIOHAL_STREAM_T *recordedStream);
AUDIOHAL_ERR_T AudioHAL_AifPause(BOOL pause);
void AudioHAL_AifStopPlay(void);
void AudioHAL_AifStopRecord(void);

#define AUDIOHAL_PLAY_RES_FRAMES_COUNT_LARGE  	(20)
#define AUDIOHAL_PLAY_RES_FRAMES_COUNT_DEFAULT  (4)
AUDIOHAL_ERR_T AudioHAL_AifDrain(void);
AUDIOHAL_ERR_T AudioHAL_SetResBufCnt(unsigned int bufCnt);

UINT16 AudioHAL_vibration_setGain(UINT16 gain);
void AudioHAL_vibration_control(UINT16 onoff);
AUDIOHAL_ERR_T AudioHAL_speaker_protection(UINT16 freq);

UINT16 AudioHAL_DRCReset(UINT16 gain, UINT16 index);
void AudioHAL_DRCDisable(UINT16 disable);
void AudioHAL_set_close_delay(unsigned int cnt);

int AudioHAL_IsMediaStreamOn(void);
void AudioHAL_FadingControl(int on);
signed short AudioHAL_SetFadingStep(signed short step);



/**********************************************************************
 *
 * Function: AudioHAL_AifSetPhoneStatus
 *
 * Description: set phone voice call status 
 *
 * status:  
    0:  let audio hal decide Voice Call status
    1:  in Media Audio play status
    2:  in Voice Call status

 *  return value: 
    AUDIOHAL_ERR_NO: success
    AUDIOHAL_ERR_BAD_PARAMETER: status large than 2
 
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 22-Oct-2004	yg                          Initial version
 **********************************************************************/
AUDIOHAL_ERR_T AudioHAL_AifSetPhoneStatus(unsigned int status);

/**********************************************************************
 *
 * Function: AudioHAL_SetUsedMIC
 *
 * Description: set used mic
 *
 * status:  
    0(default):  let audio hal decide which mic to use  
    1:  use mic1
    2:  use mic2

 *  return value: 
    0: success
    -1:large than 2 
 *  NOTE:  need to disable Headset detection
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 7-April-2020	Chaoliu                      Initial version
 **********************************************************************/
int AudioHAL_SetUsedMIC(int mic);


/**********************************************************************
 *
 * Function: AudioHAL_sspaSetDelay
 *
 * Description: set PCM interface SSPA's data delay
 *
 * status:  
    0: no delay;  
    1: 1 bitclk delay; (default) 


 *  return value: 
    0: success
    -1:fail 
 *  NOTE:  
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 7-April-2020	Chaoliu                      Initial version
 **********************************************************************/
int AudioHAL_sspaSetDelay(unsigned int delay) ;


/**********************************************************************
 *
 * Function: AudioHAL_sspaSetSampleRate
 *
 * Description: set PCM interface SSPA's sample Rate
 *
 * status:  
    1: 8k; 
    2: 16k; 
    6: 48k

 *  return value: 
    0: success
    -1:fail 
 *  NOTE:  
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 7-April-2020	Chaoliu                      Initial version
 **********************************************************************/
int AudioHAL_sspaSetSampleRate(unsigned int rate);


/**********************************************************************
 *
 * Function: AudioHAL_sspaSetBCLKType
 *
 * Description: set PCM interface SSPA's bitclk frequency
 *
 * status:  
    0: 32fs; 
    1: 64fs; 
    2: 128fs; 
    3: 256fs; 
    4: 512fs; 
    5: 1024fs;  
    6: 2048fs; 


 *  return value: 
    0: success
    -1:fail 
 *  NOTE:  
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 7-April-2020	Chaoliu                      Initial version
 **********************************************************************/
int AudioHAL_sspaSetBCLKType(unsigned int type) ;

/**********************************************************************
 *
 * Function: AudioHAL_sspaSetBCLKInvert
 *
 * Description: set PCM interface SSPA's bitclk INVERT
 *
 * status:  
    0: keep default; 
    1: invert; 


 *  return value: 
    0: success
    -1:fail 
 *  NOTE:  
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 7-April-2020	Chaoliu                      Initial version
 **********************************************************************/


/**********************************************************************
 *
 * Function: AudioHAL_sspaSetI2S
 *
 * Description: set PCM interface SSPA's PCM/I2S format
 *
 * status:  
    0: PCM; 
    1: I2S; 


 *  return value: 
    0: success
    -1:fail 
 *  NOTE:  
 *
 * --------------------------------------------------------------------
 * Revision History
 *
 * Date         Who        Version           Description
 * --------------------------------------------------------------------
 * 7-April-2020	Chaoliu                      Initial version
 **********************************************************************/
int AudioHAL_sspaSetI2S(unsigned int i2s) ;

void AudioHAL_AifSetTxDSPGain(signed char db);
char AudioHAL_AifGetTxDSPGain(void);
void AudioHAL_AifSetTxCodecGain(short pgaStage1, short pgaStage2);
short AudioHAL_AifGetTxCodecGainStage1(void);
short AudioHAL_AifGetTxCodecGainStage2(void);






void AudioHAL_TestAudioLog(char* str);


#ifdef __cplusplus
}
#endif
#endif  /* _AUDIOHAL_H_ */
