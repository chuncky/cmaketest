#ifndef _AVIDEO_H
#define _AVIDEO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    unsigned char *pBitstream;
    int nBufferSize;
    int nFilledLen;
    int nFrameType; // 0: I frame, 1: P frame.
}ABitStream;

typedef struct {
    unsigned char *pFrame;
    int nSize;
    int nWidth;
    int nHeight;
    int nStride;
    int nStrideY;
    int format;//0: I420; 1: NV12
}AVideoFrame;

typedef enum{
    AVideo_RC_VARIABLE = 0,
    AVideo_RC_CONSTANT
}AVideoRateControlMode;
typedef struct {
    int nWidth;
    int nHeight;
    int nFramerate;
    int nBitrate;
    int nIFrameInterval;
    AVideoRateControlMode  nRateControlMode;
}AVideoEncodeParam;

void* AVideo_MPEG4Decoder_Init(int predefined_width, int predefined_height);
int AVideo_MPEG4Decoder_Decode(void* handle, ABitStream *bitstream /*in*/, AVideoFrame **ppFrame /*out*/);
int AVideo_MPEG4Decoder_Deinit(void* handle);

void* AVideo_H263Decoder_Init(int width, int height);
int AVideo_H263Decoder_Decode(void* handle, ABitStream *bitstream/*in*/, AVideoFrame **ppFrame/*out*/);
int AVideo_H263Decoder_Deinit(void* handle);

void* AVideo_MPEG4Encoder_Init(AVideoEncodeParam *param);
int AVideo_MPEG4Encoder_Encode(void *handle, AVideoFrame *frame /*in*/, ABitStream *bitstream/*out*/);
int AVideo_MPEG4Encoder_Deinit(void* handle);
int AVideo_MPEG4Encoder_GetParam(void *handle, AVideoEncodeParam *param);
int AVideo_MPEG4Encoder_SetParam(void *handle, AVideoEncodeParam *param);

void* AVideo_H263Encoder_Init(AVideoEncodeParam *param);
int AVideo_H263Encoder_Encode(void *handle, AVideoFrame *frame/*in*/, ABitStream *bitstream/*out*/);
int AVideo_H263Encoder_Deinit(void* handle);
int AVideo_H263Encoder_GetParam(void *handle,AVideoEncodeParam *param);
int AVideo_H263Encoder_SetParam(void *handle,AVideoEncodeParam *param);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
