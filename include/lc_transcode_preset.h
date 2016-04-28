#ifndef LIVE_CAPTURE_LC_TRANSCODE_PRESET_H
#define LIVE_CAPTURE_LC_TRANSCODE_PRESET_H

#include "lc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* audio codec parameters
**/
typedef
struct _tag_lc_audio_output {
    int bitrate_in_bps;
    int sample_rate_in_hz;
    int channels;

    bool is_valid;
} lc_audio_output_t;

typedef
enum _tag_LC_VIDEO_CODEC {
    H264 = 0,
} LC_VIDEO_CODEC;

/**
* video resizing policy
**/
typedef
enum _tag_LC_VIDEO_SIZING {
    KEEP = 0,
    FIT,
    STRETCH
} LC_VIDEO_SIZING;

typedef
enum _tag_LC_VIDEO_CODEC_PROFILE {
    BASELINE = 0,
    MAIN,
    HIGH
} LC_VIDEO_CODEC_PROFILE;

/**
* additional video codec parameters
**/
typedef
struct _tag_lc_video_codec_options {
    LC_VIDEO_CODEC_PROFILE profile;
} lc_video_codec_options_t;

/**
* video codec parameters
**/
typedef
struct _tag_lc_video_output {
    LC_VIDEO_CODEC codec;
    int bitrate_in_bps;
    int max_framerate_x100;
    int max_width_in_pixel;
    int max_height_in_pixel;
    LC_VIDEO_SIZING sizing_policy;
    lc_video_codec_options_t codec_options;
    bool is_valid;
} lc_video_output_t;

/**
* hls parameters
**/
typedef
struct _tag_lc_hls_config {
    int segment_time_in_second;
    int segment_list_size;
    lc_bool_t adaptive;
    bool is_valid;
} lc_hls_config_t;

/**
* rtmp parameters
**/
typedef
struct _tag_lc_rtmp_config_t {
    lc_bool_t gop_cache;
    bool is_valid;
} lc_rtmp_config_t;

typedef
struct _tag_lc_transcode_preset {
    char name[LC_MAX_PRESET_NAME];
    char description[LC_MAX_PRESET_DESC];
    lc_bool_t forward_only;
    lc_audio_output_t audio;
    lc_video_output_t video;
    lc_hls_config_t hls;
    lc_rtmp_config_t rtmp;
    lc_datetime_t create_time;
} lc_transcode_preset_t;


//////////////////////////////////////////////////////////////////////////
// Preset API

LC_CODE LC_API lc_transcode_preset_init(lc_transcode_preset_t* preset);


/**
* create a new live preset
* preset : parameters used to create a new preset
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_transcode_preset_create(lc_transcode_preset_t* preset);

/**
* delete an existing preset by preset name
* preset_name : the preset name to be deleted
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_transcode_preset_delete(const char* preset_name);

/**
* query preset by name
* preset_name: the preset name to be queried
* preset: a pointer to lc_transcode_preset_t used to receive preset data
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_transcode_preset_query(const char* preset_name, lc_transcode_preset_t* preset);

/**
* query all preset
* pp_preset_list : a pointer to lc_transcode_preset_list_t
* return : error code, LC_OK: success, others: failed
* NOTICE: if succeed, user should call lc_transcode_preset_list_free to free memory of pp_preset
**/
LC_CODE LC_API lc_transcode_preset_list(lc_list_t* preset_list);

#ifdef __cplusplus
}
#endif

#endif //LIVE_CAPTURE_LC_TRANSCODE_PRESET_H