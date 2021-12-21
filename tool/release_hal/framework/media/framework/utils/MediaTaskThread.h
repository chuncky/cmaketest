#ifndef _ASR_FP_MM_MEDIA_TASK_THREAD_H
#define _ASR_FP_MM_MEDIA_TASK_THREAD_H

#include "utils/ui_osa_utils.h"
#include "utils/Semaphore.h"
#include "utils/Mutex.h"

#define MEDIA_TASK_QUEUE_SIZE_EXP 4
#define MEDIA_TASK_QUEUE_SIZE (1 << MEDIA_TASK_QUEUE_SIZE_EXP)
#define MEDIA_TASK_QUEUE_SIZE_MASK (MEDIA_TASK_QUEUE_SIZE - 1)

class MediaTaskHandler;

typedef enum {
    MEDIA_TASK_SCHEDULE_DECODING = 0,
    MEDIA_TASK_SCHEDULE_AUDIO_ENCODING,
    MEDIA_TASK_SCHEDULE_VIDEO_ENCODING,
    MEDIA_TASK_AV_SYNC
} MediaTaskType;

typedef struct
{
    MediaTaskType type;
    void *data;
    int size;
    MediaTaskHandler *handler;
} MediaTask;

class MediaTaskHandler
{
public:
    virtual int onTaskReceived(MediaTask task) = 0;
};

class MediaTaskThread
{
public:
    MediaTaskThread();
    int start();
    int stop();
    int post(MediaTask task);

    ~MediaTaskThread();
private:
    static void *ThreadWrapper(void *me);
    void threadEntry();
    int execute(MediaTask task);
    bool mDone;
    bool mStarted;
    MediaTask mQueue[MEDIA_TASK_QUEUE_SIZE];
    int mReadPos;
    int mWritePos;
    int mTasks;
    Mutex mLock;
    Semaphore mSem;
    mmf_thread mThread;

    MediaTaskThread(const MediaTaskThread&);
    MediaTaskThread&      operator = (const MediaTaskThread&);
};
#endif
