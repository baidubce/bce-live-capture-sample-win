#ifndef LIVE_CAPTURE_LC_RECORDING_PRESET_H
#define LIVE_CAPTURE_LC_RECORDING_PRESET_H

#include "lc_types.h"

#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////
// recording API
typedef 
enum _tag_LC_RECORDING_FORMAT {
    MP4 = 0,
    M3U8,
} LC_RECORDING_FORMAT;

typedef
struct _tag_lc_bos {
    char bucket[LC_MAX_BOS_BUCKET_LEN];
    char region[LC_MAX_BUFFER_32];
} lc_bos_t;

typedef
struct _tag_lc_recording_preset {
    char name[LC_MAX_BUFFER_64];
    char description[LC_MAX_BUFFER_128];
    lc_bos_t bos;
    LC_RECORDING_FORMAT format;
    char pattern[LC_MAX_BUFFER_128];
    int period_in_minute;
    lc_datetime_t create_time;
} lc_recording_preset_t;



LC_CODE LC_API lc_recording_preset_query(const char* name, lc_recording_preset_t* preset);

LC_CODE LC_API lc_recording_preset_list(lc_list_t* preset_list);

#ifdef __cplusplus
}
#endif

#endif //LIVE_CAPTURE_LC_RECORDING_PRESET_H
