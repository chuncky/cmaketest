#ifndef ASR_FF_MM_MEDIABUFFER_H_
#define ASR_FF_MM_MEDIABUFFER_H_

#include "utils/MediaParameters.h"

#define MEDIA_BUFFER_FLAG_EOS        0x1
#define MEDIA_BUFFER_FLAG_ENDOFFRAME (1 << 1)
#define MEDIA_BUFFER_FLAG_SYNC_FRAME (1 << 2)
#define MEDIA_BUFFER_FLAG_AUDIO_TYPE (1 << 3)
#define MEDIA_BUFFER_FLAG_VIDEO_TYPE (1 << 4)
#define MEDIA_BUFFER_FLAG_CODECCONFIG (1 << 5)



class MediaBuffer;

typedef struct MediaBufferMeta_t {
    long long nTimeStamp;         /**< Timestamp corresponding to the sample
                                     starting at the first logical sample
                                     boundary in the buffer. Timestamps of
                                     successive samples within the buffer may
                                     be inferred by adding the duration of the
                                     of the preceding buffer to the timestamp
                                     of the preceding buffer.*/
    unsigned int    nFlags;           /**< buffer specific flags */
    union {
        VideoParameters video;
        AudioParameters audio;
    } format;
} MediaBufferMeta;
class MediaBufferObserver {
public:
    MediaBufferObserver() {}
    virtual ~MediaBufferObserver() {}

    virtual void signalBufferReturned(MediaBuffer *buffer) = 0;

private:
    MediaBufferObserver(const MediaBufferObserver &);
    MediaBufferObserver &operator=(const MediaBufferObserver &);
};

class MediaBuffer {
public:
    MediaBuffer(int size) ;
	MediaBuffer(void* data, int size);

    void release();
    void add_ref();
    void setObserver(MediaBufferObserver *observer);
    void *data() const;
    unsigned int size() const;

    unsigned int range_offset() const;
    unsigned int range_length() const;
    void set_flag(unsigned int flags) {
        mMeta.nFlags = flags;
    }
    void set_timestamp(long long timestamp)
    {
        mMeta.nTimeStamp = timestamp;
    }
    void set_format(VideoParameters *param)
    {
        if (param != 0)
        {
            mMeta.format.video = *param;
        }
    }
    void set_format(AudioParameters *param)
    {
        if (param != 0)
        {
            mMeta.format.audio = *param;
        }
    }
    MediaBufferMeta meta_data()
    {
        return mMeta;
    }
    void set_range(unsigned int offset, unsigned int length);

protected:
    virtual ~MediaBuffer();

private:
    void *mData;
    unsigned int mSize, mRangeOffset, mRangeLength;
    MediaBufferObserver *mObserver;
    int mRefCount;
    MediaBufferMeta mMeta;
	bool mOwnsData;
    MediaBuffer(const MediaBuffer &);
    MediaBuffer &operator=(const MediaBuffer &);
};
#endif
