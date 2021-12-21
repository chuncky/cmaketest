#ifndef ASR_FP_MM_MEDIA_PARAMETERS_H
#define  ASR_FP_MM_MEDIA_PARAMETERS_H

#include "plat_config_defs.h"

typedef enum {
    MEDIA_TYPE_AUDIO_START = 0x100,
    MEDIA_TYPE_AUDIO_PCM = MEDIA_TYPE_AUDIO_START,
    MEDIA_TYPE_AUDIO_MP3,
    MEDIA_TYPE_AUDIO_AMRNB,
    MEDIA_TYPE_AUDIO_AMRWB,
    MEDIA_TYPE_AUDIO_AAC,
    MEDIA_TYPE_AUDIO_MIDI,
    MEDIA_TYPE_AUDIO_PCM8,
#ifdef AVI_SUPPORT
    MEDIA_TYPE_AUDIO_G711_ALAW,
    MEDIA_TYPE_AUDIO_G711_MLAW,
#endif

#ifdef CONFIG_ADPCM_CODEC_SUPPORT
    MEDIA_TYPE_AUDIO_ADPCM,
#endif

    MEDIA_TYPE_AUDIO_MAX = 0x1FF,
    MEDIA_TYPE_VIDEO_START = 0x1000,
    MEDIA_TYPE_VIDEO_YUV420 = MEDIA_TYPE_VIDEO_START,
    MEDIA_TYPE_VIDEO_H264,
    MEDIA_TYPE_VIDEO_H263,
    MEDIA_TYPE_VIDEO_MPEG4,
    MEDIA_TYPE_VIDEO_NV12,
    MEDIA_TYPE_VIDEO_MAX = 0x1FFF,
    MEDIA_TYPE_UNDEFINED = 0xFFFF,
} MediaType;

typedef struct MediaExtractorMeta {
    int nDurationMs;
    int nTracks;
    bool hasVideo;
    bool hasAudio;
    int nBitrate;
} MediaExtractorMeta;

typedef struct VideoParameters {
    MediaType eType;
    unsigned int nFrameWidth;
    unsigned int nFrameHeight;
    unsigned int nStride;
    unsigned int nSliceHeight;
    unsigned int nBitrate;
    unsigned int xFramerate;
    unsigned int nGopSize;
} VideoParameters;


typedef struct AudioParameters {
    MediaType eType;
    unsigned int nSamplingRate;
    unsigned int nChannels;
    unsigned int nBitRate;
} AudioParameters;
#endif
