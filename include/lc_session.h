#ifndef LIVE_CAPTURE_LC_SESSION_H
#define LIVE_CAPTURE_LC_SESSION_H

#include "lc_types.h"

#ifdef __cplusplus
extern "C" {
#endif



//////////////////////////////////////////////////////////////////////////
// session API
typedef
struct _tag_lc_session_publish {
    char region[LC_MAX_BUFFER_32];
    char url[LC_MAX_URL_LEN];
    bool is_pull;
    bool is_valid;
    char push_stream[LC_MAX_BUFFER_64];
} lc_session_publish_t;

/**
* session configuration structure used to create a new live session
**/
typedef
struct _tag_lc_session_config {
    char description[LC_MAX_SESSION_DESC];
    char preset[LC_MAX_PRESET_NAME];
    lc_session_publish_t publish;
    char recording[LC_MAX_BUFFER_128];
    char notification [LC_MAX_BUFFER_128];
    char security_policy[LC_MAX_BUFFER_128];
} lc_session_config_t;

typedef
enum _tag_LC_SESSION_STATUS {
    SESSION_UNKNOWN,
    SESSION_READY,
    SESSION_ONGOING,
    SESSION_PAUSED,
    SESSION_FAILED,
    SESSION_CONNECTING
} LC_SESSION_STATUS;

typedef
enum _tag_LC_SESSION_STREAMING_STATUS {
    STREAMING_UNKNOWN,
    STREAMING_STREAMING,
    STREAMING_NO_SOURCE,
    STREAMING_FAILED
} LC_SESSION_STREAMING_STATUS;

/**
* session's play url
**/
typedef
struct _tag_lc_session_play {
    char hls_url[LC_MAX_URL_LEN];
    char rtmp_url[LC_MAX_URL_LEN];
} lc_session_play_t;

typedef
struct _tag_lc_error {
    char code[LC_MAX_ERR_CODE];
    char message[LC_MAX_ERR_MSG];

    bool is_valid;
} lc_error_t;

typedef
struct _tag_lc_session_statistic    {
    int play_count;
    int bandwidth_in_bps;
} lc_session_statistic_t;
/**
* session information
**/
typedef
struct _tag_lc_session {
    char session_id[LC_MAX_SESSION_ID_LEN];
    lc_session_config_t config;
    lc_session_play_t play;
    LC_SESSION_STATUS status;
    LC_SESSION_STREAMING_STATUS streaming_status;
    lc_error_t error;
    lc_session_statistic_t statistic;
    lc_datetime_t create_time;
} lc_session_t;

/**
* video codec information 
**/
typedef
struct _tag_lc_video_source_info {
    char codec[LC_MAX_BUFFER_16];
    char profile[LC_MAX_BUFFER_16];
    char level[LC_MAX_BUFFER_16];
    int width_in_pixel;
    int height_in_pixel;
    int framerate_x100;
    int bitrate_in_bps;
    int real_fps_x100;
} lc_video_source_info_t;

/**
* audio codec information 
**/
typedef
struct _tag_lc_audio_source_info {
    char codec[LC_MAX_BUFFER_16];
    char profile[LC_MAX_BUFFER_16];
    int sample_rate_in_hz;
    int channels;
    int bitrate_in_bps;
} lc_audio_source_info_t;

typedef
struct _tag_lc_source_info {
    char session_id[LC_MAX_SESSION_ID_LEN];
    char source_ip[LC_MAX_BUFFER_32];
    int score;
    lc_datetime_t capture_time;
    int input_bitrate_in_bps;
    lc_video_source_info_t video;
    lc_audio_source_info_t audio;
} lc_source_info_t;

/**
* cuepoint argument 
**/
typedef 
struct _tag_lc_session_cuepoint_argument {
    char name[LC_MAX_BUFFER_128];
    char value[LC_MAX_BUFFER_128];
} lc_session_cuepoint_argument_t;

/**
* cuepoint
* type of content in arguments is lc_session_cuepoint_argument_t
* "callback" MUST NOT BE those protocol commands
*    "connect", "createStream", "closeStream", "play", "pause", "onBWDone", 
*    "onStatus", "_result", "_error", "releaseStream", "FCPublish", "FCUnpublish", 
*    "publish", "|RtmpSampleAccess", "@setDataFrame", "onMetaData"
**/
typedef
struct _tag_lc_session_cuepoint {
    char callback[LC_MAX_BUFFER_128];
    lc_list_t arguments;
} lc_session_cuepoint_t;

LC_CODE LC_API lc_session_config_init(lc_session_config_t* cfg);

/**
* create a new live session
* cfg : parameters used to create a new session
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_create(lc_session_config_t* cfg, lc_session_t* session);

/**
* query session by session id
* session_id: the session id to be queried
* session: a pointer to lc_session_t used to receive session data
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_query(const char* session_id, lc_session_t* session);

/**
* query session by session id, if auth is on, build token string for push url, hls url and rtmp url; 
* session_id: the session id to be queried
* session: a pointer to lc_session_t used to receive session data
* expire: the date that token will be expired
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_query_with_auth(const char* session_id, lc_session_t* session, lc_datetime_t* expire);

/**
* query all sessions
* pp_session : a pointer to lc_session_list_t
* return : error code, LC_OK: success, others: failed
* status: query sessions only in the give status,SESSION_UNKNOWN for sessions in any status.
* NOTICE: if succeed, user shall call lc_list_free to free memory of session_list
**/
LC_CODE LC_API lc_session_list(lc_list_t* session_list, LC_SESSION_STATUS status);

/**
* pull live stream from media server
* session_id : the id of the session to be started to pull
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_pull(const char* session_id);

/**
* turn on/off recording for the session
* session_id : the id of the session
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_toggle_recording(const char* session_id, const char* recording_name);

/**
* pause live session by session id
* session_id : the id of the session to be paused
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_pause(const char* session_id);

/**
* resume live session by session id
* session_id : the id of the session to be stopped
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_resume(const char* session_id);

/**
* stop current push-streaming, refresh the push url of live session by session id.
* session_id: in, the id of the session whose url is going to be refreshed
* session: out, resulting refreshed session
* return: error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_refresh(const char* session_id, lc_session_t* session);

/**
* delete live session by session id
* session_id : the id of the session to be stopped
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_delete(const char* session_id);

/**
* query information of the stream source
* session_id : id of the session to be queried
* info: to store resulted information
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_source_info(const char* session_id, lc_source_info_t* info);

/**
* set cuepoint for the given session
* session_id : id of the session
* cuepoint: cuepoint information
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_set_cuepoint(const char* session_id, lc_session_cuepoint_t* cuepoint);



#ifdef __cplusplus
}
#endif

#endif //LIVE_CAPTURE_LC_SESSION_H