#ifndef _AAUDIO_H
#define _AAUDIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define    kAAudioMp3DecoderOutputBufferSize   (4608*2)


typedef enum {
    AAUDIO_OK = 0,
    AAUDIO_ERR_NO_MORE_DATA,
    AAUDIO_ERR_INVALID_PARAM,
    AAUDIO_ERR_MEMORY,
    AAUDIO_ERR_UNKNOWN,
} AAudioErrCode;

typedef enum
{
    AMR_MODE_MR475 = 0,/* 4.75 kbps */
    AMR_MODE_MR515,    /* 5.15 kbps */
    AMR_MODE_MR59,     /* 5.90 kbps */
    AMR_MODE_MR67,     /* 6.70 kbps */
    AMR_MODE_MR74,     /* 7.40 kbps */
    AMR_MODE_MR795,    /* 7.95 kbps */
    AMR_MODE_MR102,    /* 10.2 kbps */
    AMR_MODE_MR122,    /* 12.2 kbps */
    AMR_MODE_MRDTX,    /* DTX       */
    AMR_MODE_N_MODES   /* Not Used  */
} AMRNB_Mode_t;

typedef struct {
    unsigned char *pBitstream;/* buffer pointer of bitstream */
    int nOffset;              /* start offset of valid bitstream,
                                 codec read/write from pointer: pBitstream + nOffset,
                                 codec will update 'nOffset' when bytes consumed by codec.
                              */
    int nBufferSize;          /* size of bitstream buffer */
    int nFilledLen;           /* size of valid bitstream */
} AAudioBitStream;

typedef struct {
    int nDtx; /*1: DTX is on; 0: DTX is off. */
    AMRNB_Mode_t nMode; /*AMR bitrate mode. */
} AAudioAMREncodeParam;

typedef struct {
    int nSampleRate; /*sample rate of mp3 */
    int nChannel;    /*channles of mp3 */
    int nBitrate;    /*bitrate of mp3 */
} AAudioMp3EncodeParam;

typedef struct {
    unsigned char *pData; /*  pointer of PCM buffer */
    int nBufferSize;      /*  size of bitstream buffer*/
    int nOffset;          /*  start offset of valid PCM data. */
    int nFilledLen;       /*  size of valid PCM data. */
    int nSampleRate;      /*  sample rate of PCM data. */
    int nChannel;         /*channles of PCM data */
} AAudioPCMFrame;

// ============================================================================
// AAudio_AMRNBDecoder_Init
// ----------------------------------------------------------------------------
/// Create a AMR-NB decoder
/// @param void :
/// @return void*: return handle of decoder.
// ============================================================================
void* AAudio_AMRNBDecoder_Init();

// ============================================================================
// AAudio_AMRNBDecoder_Decode
// ----------------------------------------------------------------------------
/// Decode AMR-NB bitstream
/// @param handle : handle of decoder.
/// @param bitstream : input bitstream structure, decoder will update it after decoding.
/// @param ppFrame : output buffer structure, decoder will update it after decoding.
/// @return AAudioErrCode:
///             AAUDIO_OK: one frame is decoded successfully.
///             AAUDIO_ERR_NO_MORE_DATA: input bitstream didn't contains one complete frame, feed more data for decoding.
///             AAUDIO_ERR_INVALID_PARAM: parameters is invalide.
///             AAUDIO_ERR_MEMORY:        in/out buffer is too small or unexpected.
///             AAUDIO_ERR_UNKNOWN:       decoder error.
// ============================================================================
int AAudio_AMRNBDecoder_Decode(void* handle, AAudioBitStream *bitstream /*in*/, AAudioPCMFrame *ppFrame /*out*/);

// ============================================================================
// AAudio_AMRNBDecoder_Deinit
// ----------------------------------------------------------------------------
/// Destory AMR-NB decoder.
/// @param handle : handle of decoder.
/// @return AAudioErrCode:
///             AAUDIO_OK: one frame is decoded successfully.
///             AAUDIO_ERR_INVALID_PARAM: parameters is invalide.
///             AAUDIO_ERR_UNKNOWN:       decoder error.
// ============================================================================
int AAudio_AMRNBDecoder_Deinit(void* handle);

// ============================================================================
// AAudio_MP3Decoder_Init
// ----------------------------------------------------------------------------
/// Create a AMR-NB decoder
/// @param void :
/// @return void*: return handle of decoder.
// ============================================================================
void* AAudio_MP3Decoder_Init();

// ============================================================================
// AAudio_MP3Decoder_Decode
// ----------------------------------------------------------------------------
/// Decode MP3 bitstream
/// @param handle : handle of decoder.
/// @param bitstream : input bitstream structure, decoder will update it after decoding.
/// @param ppFrame : output buffer structure, decoder will update it after decoding.
/// @return AAudioErrCode:
///             AAUDIO_OK: one frame is decoded successfully.
///             AAUDIO_ERR_NO_MORE_DATA: input bitstream didn't contains one complete frame, feed more data for decoding.
///             AAUDIO_ERR_INVALID_PARAM: parameters is invalide.
///             AAUDIO_ERR_MEMORY:        in/out buffer is too small or unexpected.
///             AAUDIO_ERR_UNKNOWN:       decoder error.
// ============================================================================
int AAudio_MP3Decoder_Decode(void* handle, AAudioBitStream *bitstream/*in*/, AAudioPCMFrame *ppFrame/*out*/);

// ============================================================================
// AAudio_MP3Decoder_Deinit
// ----------------------------------------------------------------------------
/// Destory MP3 decoder.
/// @param handle : handle of decoder.
/// @return AAudioErrCode:
///             AAUDIO_OK: one frame is decoded successfully.
///             AAUDIO_ERR_INVALID_PARAM: parameters is invalide.
///             AAUDIO_ERR_UNKNOWN:       decoder error.
// ============================================================================
int AAudio_MP3Decoder_Deinit(void* handle);

#if 0
void* AAudio_MP3Encoder_Init(AAudioMp3EncodeParam *param);
int AAudio_MP3Encoder_Encode(void *handle, AAudioPCMFrame *frame /*in*/, AAudioBitStream *bitstream/*out*/);
int AAudio_MP3Encoder_Deinit(void* handle);
#endif

// ============================================================================
// AAudio_AMRNBEncoder_Init
// ----------------------------------------------------------------------------
/// Create a AMR-NB encoder
/// @param void :
/// @return void*: return handle of encoder.
// ============================================================================
void* AAudio_AMRNBEncoder_Init(AAudioAMREncodeParam *param);

// ============================================================================
// AAudio_AMRNBEncoder_Encode
// ----------------------------------------------------------------------------
/// Encode AMR-NB bitstream
/// @param handle : handle of encoder.
/// @param frame :    input PCM frame structure, encoder will update it after encoding.
/// @param bitstream : output bitstream structure, encoder will update it after encoding.
/// @return AAudioErrCode:
///             AAUDIO_OK: one frame is encoded successfully.
///             AAUDIO_ERR_NO_MORE_DATA: input PCM frame didn't contains enough samples, feed more data.
///             AAUDIO_ERR_INVALID_PARAM: parameters is invalide.
///             AAUDIO_ERR_MEMORY:        in/out buffer is too small or unexpected.
///             AAUDIO_ERR_UNKNOWN:       encoder error.
// ============================================================================
int AAudio_AMRNBEncoder_Encode(void *handle, AAudioPCMFrame *frame/*in*/, AAudioBitStream *bitstream/*out*/);

// ============================================================================
// AAudio_AMRNBEncoder_Deinit
// ----------------------------------------------------------------------------
/// Destory AMR-NB encoder.
/// @param handle : handle of encoder.
/// @return AAudioErrCode:
///             AAUDIO_OK: one frame is encoded successfully.
///             AAUDIO_ERR_INVALID_PARAM: parameters is invalide.
///             AAUDIO_ERR_UNKNOWN:       encoder error.
// ============================================================================
int AAudio_AMRNBEncoder_Deinit(void* handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

