#ifndef _ASR_FP_MM_AMEDIAPLAYER_H_
#define _ASR_FP_MM_AMEDIAPLAYER_H_

#include "utils/Mutex.h"
#include "utils/Semaphore.h"
#include "utils/MediaBuffer.h"
#include "utils/VideoRender.h"
#include "extractor/MediaExtractor.h"
#include "mci.h"

#define RING_BUFFER_COUNT_MASK  (0x3)
#define RING_BUFFER_COUNT (RING_BUFFER_COUNT_MASK + 1)
class MediaBuffer;
class MediaSource;
class AudioSink;
class VideoSink;
class MediaCodec;
class DataSource;
typedef struct {
    unsigned int mDurationMs;
    unsigned int mTracks;
    unsigned int mBitrate;
	unsigned int mSampleRate;
	unsigned int mChannels;
	unsigned int mWidth;
	unsigned int mHeight;

    unsigned char mAlbum[128];
    unsigned char mArtist[128];
    unsigned char mAlbumArtist[128];
    unsigned char mComposer[128];
    unsigned char mGenre[128];
    unsigned char mTitle[128];
    unsigned char mYear[128];
    unsigned char mCompilation[128];
    unsigned char mAuthor[128];

    unsigned char mAlbumArtMime[128];
    unsigned char *mAlbumArtData;
    unsigned int mAlbumArtDataSize;
    
}AMediaFileMetadata;

typedef enum
{
    MEDIA_TRACK_NONE,   /* Video has no track in it */
    MEDIA_TRACK_AV,     /* Video has both audio and video tracks */
    MEDIA_TRACK_A_ONLY, /* Video has audio track only */
    MEDIA_TRACK_V_ONLY, /* Video has video track only */
    MEDIA_TRACK_COUNT   /* Count of video track enum */
}MEDIA_TRACK;
typedef int (*AMediaPlayerDataCallback)(unsigned int from_pos, unsigned char *data, unsigned int size);
class AMediaPlayerListener
{
public:
    typedef enum {
        MEDIAPLAYER_EOS,
        MEDIAPLAYER_ERROR,
        //JHH added for #32745 callback begin
        MEDIAPLAYER_TIMER
        //JHH added for #32745 callback end
    } EventType;
	//JHH added for #33384 begin
	static const int MEDIAPLAYER_STATE_DATA_SRC_SET = -1000;
	static const int MEDIAPLAYER_STATE_BEGIN_PLAY = -1001;
	//JHH added for #33384 end
    virtual int onEvent(EventType event, void *data=NULL) = 0;
protected:
    virtual ~AMediaPlayerListener() {}
};

class AMediaPlayer
{
public:
    AMediaPlayer();
    int start(bool inPaused = false);
    int setDataSource(const char *filename);
    int setDataSource(uint8_t *cache, size_t cacheSize);
    int setDataSourceFd(mmf_file_fd fd);
    int setDataSource(MediaSource *source);
    int setDataSource(uint8_t *cache, size_t cacheSize, AMediaPlayerDataCallback data_cb);
    int setListener(AMediaPlayerListener *listener) {
        mListener = listener;
        return 0;
    }
	int setAudioSinkPort(int32_t tagSinkPort) {
		mTagAudioSinkPort = tagSinkPort;
		return 0;
	}

	int setAudioStreamType(audio_stream_type_t type) {
		mType = type;
		return 0;
	}

	int disableAudioTrack() {
		mAudioTrackDisable = true;
		return 0;
	}
	int getVideoRenderParameters(VideoRenderParameters *VideoRenderParams) {
		*VideoRenderParams = mVideoRenderParams;
		return 0;
	}	
	int configVideoRenderParameters(VideoRenderParameters *VideoRenderParams);	
	int setAudioSinkWithOffloadMode();
	void getTrackInfo(uint16_t *image_width, uint16_t *image_height, uint32_t *total_time, uint16_t *aud_channel, uint16_t *aud_sample_rate, uint16_t *track);
    int stop();
    int pause();
    int resume();
    int seekTo(int *seekTimeMs);
    int getCurrentPosition(int *msec);
    int getDuration(int *msec);
    int getMediaMeta(AMediaFileMetadata *meta);
	int setLooping(bool loop) {mLooping = loop; return 0;}
#if defined(ENABLE_A2DP_AUDIOSINK)
	int setUsingA2dpSink(bool enable);
#endif
	bool isLooping() {return mLooping;}
    int AthreadEntry();
	mmf_thread getAThreadHandler() const { return mRendererThreadA;};
    int VthreadEntry();	
	mmf_thread getVThreadHandler() const { return mRendererThreadV;};
    virtual ~AMediaPlayer();

protected:
	void signalEOS(bool audio);
	void signalERROR(int error_code);

private:
	friend class AifAudioSink;
	friend class AifOffloadAudioSink;
	friend class A2dpAudioSink;
	class AVSync {
	public:
		AVSync(int64_t systemTimeBaseUs);		
		virtual ~AVSync();

		int64_t getNowUs();
		bool isDriftTooLarge(int64 frameTimeUs, int64 releaseTimeUs);		
		int64 adjustReleaseTime(int64 framePresentationTimeUs, int64 unadjustedReleaseTimeUs);
		int64 getRealTimeUsForMedaiTime(int64 mediaTimeUs);
	private:
		int64 maxAllowedDriftUs;
		int64 minFramesForAdjustment;
		int64 lastFramePresentationTimeUs;
		int64 adjustedLastFrameTimeUs;
		int64 pendingAdjustedFrameTimeUs;
		bool haveSync;
		int64 syncUnadjustedReleaseTimeUs;
		int64 syncFramePresentationTimeUs;
		int64 syncFrameCount;
		int64_t mSystemTimeBaseUs;
	};
	
	enum MediaExtractor::MediaExtractorType sniff(DataSource *Source, unsigned int *first_pos = NULL);
	int createMediaExtractor(enum MediaExtractor::MediaExtractorType extractorType, DataSource *Source, unsigned int first_pos = 0);
    int initAudioDecoder();
    int initVideoDecoder();
    int initRenderer();
    int deinitRenderer();
    static int AudioSinkCallback(void * data, int size, void *cookie);
    int audio_data_cb(void *data, int size);
    Mutex mLock;
    //Condition mBufferAvailableCond;
    AMediaPlayerListener *mListener;
    MediaExtractor *mMediaExtractor;
    MediaSource *mAudioSource;
    MediaSource *mVideoSource;
    MediaCodec  *mAudioDecoder;
    MediaCodec  *mVideoDecoder;
    AudioSink   *mAudioSink;
    VideoSink   *mVideoSink;
    MediaBuffer *mFrameBuffer;
    mmf_thread mRendererThreadV;
    mmf_thread mRendererThreadA;	
    int mSeekTimeMs;
	bool mSeeking;
	bool mVideoSeekComplete;
	bool mWatchForVideoSeekComplete;
    Semaphore mSemVideoSeekComplete;
    int64_t mCurrentTimeUs;
    int  mStreamEos;
    bool mSendEos;
    bool mStarted;
    bool mDone;
	bool mSignalledError;
    bool mPaused;
	bool mVideoTaskSuspend;
    Semaphore mSemVideoResume;	
	bool mAudioTaskSuspend;
    Semaphore mSemAudioResume;
    bool mFirstAudio;
	bool mResumed;
	bool mAudioTrackDisable;
	int32_t mTagAudioSinkPort;
	audio_stream_type_t mType;
    int64_t mTimeDeltaUs;
    bool mFirstVideo;
    int64_t mVideoAnchorTimeUs;
	AVSync *mAVSynchro;
    int32_t mDroppedFrames;
    int32_t mDecodedFrames;
	int32_t mContDroppedFrames;
	VideoRenderParameters mVideoRenderParams;
	bool mLooping;
#ifdef AAC_SUPPORT
	bool mIsADTS;
#endif
    bool mAudioOffload;
#if defined(ENABLE_A2DP_AUDIOSINK)
    bool mUsingA2dpSink;
#endif
};
#endif
