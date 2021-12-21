#ifndef ASR_FP_MM_MEDIAEXTRACTOR_H
#define ASR_FP_MM_MEDIAEXTRACTOR_H

#include "utils/MediaSource.h"
#include "utils/MediaParameters.h"

struct TrackMeta {
    union {
        VideoParameters video;
        AudioParameters audio;
    }format;
    MediaType type;
    int track_id;
    int64_t duration;
    int max_frame_size;
    int rotation;
    int display_width;
    int display_height;
    unsigned char *config_data;
    int config_data_size;
};

class MediaExtractor {

public:
    //MediaExtractor();

    enum MediaExtractorType {
        MP4_EXTRACTOR = 1,
        AAC_EXTRACTOR,
        MP3_EXTRACTOR,
        AMR_EXTRACTOR,
        MIDI_EXTRACTOR,
        WAV_EXTRACTOR,
#ifdef AVI_SUPPORT
        AVI_EXTRACTOR,
#endif
        UNDEFINED_MAX = 0xFFFF,
    };
    virtual MediaSource *getTrack(int trackIndex) = 0;
    virtual MediaType getTrackType(int trackIndex) = 0;
    virtual int countTracks() = 0;
    virtual MediaExtractorType getType() = 0;

    virtual int getMediaMeta(MediaExtractorMeta *meta) = 0;

    virtual ~MediaExtractor() {}
};

#endif
