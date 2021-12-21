#ifndef _ASR_FP_MM_MEDIA_DEFS_H_
#define _ASR_FP_MM_MEDIA_DEFS_H_
enum MediaErrorCode {
    MEDIA_OK = 0,
    MEDIA_ERROR_EOS = 1,
    MEDIA_ERROR_FORMAT_CHANGED = 2,
    MEDIA_ERROR_BUFFER_FULL,
    MEDIA_ERROR_IO = -1000,
    MEDIA_ERROR_DECODE,
    MEDIA_ERROR_ENCODE,
    MEDIA_ERROR_INVALID,
    MEDIA_ERROR_UNKNOWN,
    MEDIA_ERROR_NO_INIT,
    MEDIA_ERROR_BUSY
};
#endif
