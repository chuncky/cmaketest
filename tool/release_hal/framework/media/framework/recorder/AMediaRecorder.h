#ifndef ASR_FF_MM_AMEDIARECORDER_H_
#define ASR_FF_MM_AMEDIARECORDER_H_

#include "utils/ui_osa_utils.h"
#include "utils/MediaParameters.h"
#include "utils/MediaDefs.h"
struct VideoParameters;
struct AudioParameters;
struct MediaCodec;
struct MediaSource;
struct MediaWriter;

typedef enum {
    MEDIA_CONTAINER_RAW,
    MEDIA_CONTAINER_WAV,
    MEDIA_CONTAINER_AMR,
    MEDIA_CONTAINER_MP4,
    MEDIA_CONTAINER_AVI,
    MEDIA_CONTAINER_MP3,
    MEDIA_CONTAINER_UNDEFINED,
} MediaContainerFormat;
enum {
	AUDIO_SOURCE_START = 0,
    AUDIO_SOURCE_MIC = AUDIO_SOURCE_START,
    AUDIO_SOURCE_FAKE,
    AUDIO_SOURCE_UNDEFINED,
};
enum {
	VIDEO_SOURCE_START = 0,
    VIDEO_SOURCE_CAMERA = VIDEO_SOURCE_START,
    VIDEO_SOURCE_FAKE,
    VIDEO_SOURCE_UNDEFINED,
};
class AMediaRecorderListener
{
public:
    typedef enum {
        MEDIARECORDER_EVENT_EOS,
		MEDIARECORDER_EVENT_INFO,
        MEDIARECORDER_EVENT_ERROR,
        MEDIARECORDER_EVENT_DATA,
    } EventType;
	typedef enum{
        MEDIARECORDER_INFO_CURRENT_DURATION_MS,
		MEDIARECORDER_INFO_CURRENT_SIZE,
	}InformationType;
    virtual int onEvent(EventType event, int data1=0, int data2=0) = 0;
protected:
    virtual ~AMediaRecorderListener() {}
};


class AMediaRecorder   {
public:
    AMediaRecorder();

    int start();
    int setMediaFileFormat(MediaContainerFormat format);
    int setOutputFile(const char *filename);
    int setOutputFd(int fd);
	int setOutputBuffer(unsigned char *pBuf, unsigned int bufSize);
	int setBufferRecordLoop(bool loop = false){mLoopRecordBuffer = loop; return 0;};
    int setAudioSource(int source) ;
#if !defined(DISABLE_VIDEO_RECORDER)
    int setVideoSource(int source) ;
    int setVideoFormat(VideoParameters *param);
#endif
    int setAudioFormat(AudioParameters *param);
    int stop();
	int pause();
	int resume();
    int setListener(AMediaRecorderListener *listener) {
        mListener = listener;
        return 0;
    }
    int getSize(unsigned int *size);
    int getDurationMs(unsigned int *duartion_ms);
	void setMaxFileDuration(unsigned int duration_ms);
	void setMaxFileSize(unsigned int size);
    int InsertSyncFrame();
    virtual ~AMediaRecorder();
private:
    MediaWriter *mMediaWriter;
    MediaCodec  *mAudioEncoder;
#if !defined(DISABLE_VIDEO_RECORDER)
    MediaCodec  *mVideoEncoder;
#endif
    MediaSource *mAudioSource;
#if !defined(DISABLE_VIDEO_RECORDER)
    MediaSource *mVideoSource;
#endif
    mmf_file_fd mFile;
	bool mFileOpened;
    MediaContainerFormat mFileFormat;
    int mAudioSourceID;
#if !defined(DISABLE_VIDEO_RECORDER)
    int mVideoSourceID;
    VideoParameters mVideoParam;
#endif
    AudioParameters mAudioParam;
    bool mStarted;
	bool mPaused;
	AMediaRecorderListener *mListener;
	unsigned int mMaxFileDurationMs;
	unsigned int mMaxFileSize;
	unsigned char *mRecordBuffer;
	unsigned int mRecorderBufferSize;
	unsigned int mLoopRecordBuffer;
};
#endif

