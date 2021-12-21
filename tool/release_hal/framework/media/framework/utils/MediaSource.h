#ifndef ASR_FP_MM_MEDIASOURCE_H
#define ASR_FP_MM_MEDIASOURCE_H

#include <stdint.h>
#include "utils/MediaBuffer.h"
#include "utils/MediaParameters.h"
#include "utils/MediaDefs.h"

class MediaSource {
public:
    //MediaSource();
    virtual int start() = 0;
    virtual int stop() = 0;
    virtual int read(MediaBuffer **buf,  int64_t *seekTimeus  = 0) {
        return -1;
    };
    virtual int getVideoFormat(VideoParameters *param) {
        return -1;
    }
    virtual int getAudioFormat(AudioParameters *param) {
        return -1;
    }

    virtual ~MediaSource() {}
};
#endif
