/**
* Copyright (C), Baidu Inc.
* file  live_capture.h
* brief 媒体云直播客户端API接口.
* author wangzhao04
* version 0.1
* date    2015
* History:
*      wangzhao04: 2015/7/20, created
*      wangzhao04: 2015/9/23, add statistics;
*                             add video&audio data callback
**/

#ifndef LIVE_CAPTURE_LIVE_CAPTURE_H
#define LIVE_CAPTURE_LIVE_CAPTURE_H

#ifdef __cplusplus
extern "C" {
#endif

static const char* LC_MODULE_NAME = "LiveCapture";

#define LC_API __stdcall


/**
* module level constants definition
*    LC_MAX_DEVICE_NAME: max device name length
*    LC_MAX_PRESET_NAME: max preset name length
*    LC_MAX_PRESET_DESC: max preset description length
*    LC_MAX_USER_ID_LEN: max user id length
*    LC_MAX_SESSION_ID_LEN:  max session id length
*    LC_MAX_SESSION_DESC: max session description length
*    LC_MAX_BOS_BUCKET_LEN: max BOS bucket path length
*    LC_MAX_USER_DOMAIN_LEN: max user domain length
*    LC_MAX_URL_LEN: max URL length
*    LC_MAX_ERR_CODE: max ERR code length
*    LC_MAX_ERR_MSG: max error message length
*    LC_MAX_BCE_ACCESS_KEY_ID_LEN: max buffer length of BCE access key id string
*    LC_MAX_BCE_SECRET_ACCESS_KEY_LEN: max buffer length of BCE access key secret key string
*    LC_MAX_VIDEO_INPUT: max video input devices supported by one stream
*    LC_MAX_AUDIO_INPUT: max audio input devices supported by one stream
*    LC_MAX_RTMP_KEY_LEN:
**/
typedef
enum _tag_LC_CONSTANTS {
    LC_MAX_DEVICE_NAME = 256,
    LC_MAX_PRESET_NAME = 128,
    LC_MAX_PRESET_DESC = 256,
    LC_MAX_USER_ID_LEN = 64,
    LC_MAX_SESSION_ID_LEN = 64,
    LC_MAX_SESSION_DESC = 256,
    LC_MAX_BOS_BUCKET_LEN = 256,
    LC_MAX_USER_DOMAIN_LEN = 256,
    LC_MAX_URL_LEN = 256,
    LC_MAX_ERR_CODE = 32,
    LC_MAX_ERR_MSG = 256,
    LC_MAX_BCE_ACCESS_KEY_ID_LEN = 128,
    LC_MAX_BCE_SECRET_ACCESS_KEY_LEN = 128,
    LC_MAX_VIDEO_INPUT = 2,
    LC_MAX_AUDIO_INPUT = 2,
    LC_MAX_RTMP_KEY_LEN = 64,
    LC_MAX_BUFFER = 128,
    LC_MAX_NOTIFICATION_NAME_LEN = 64
} LC_CONSTANTS;

/**
* live capture handle type definition
**/
typedef
struct _tag_lc_handle_t {
    int unused;
}* lc_handle_t;

/**
BCE access key structure
**/
typedef
struct _tag_bce_access_key {
    char access_key_id[LC_MAX_BCE_ACCESS_KEY_ID_LEN];
    char secret_access_key[LC_MAX_BCE_SECRET_ACCESS_KEY_LEN];
} lc_bce_access_key_t;

/**
* video format information structure
* width_in_pixel: width dimension in pixel units
* height_in_pixel: height dimension in pixel units
* format_fourcc: video pixel format
* pixel_depth_in_bit: bit count for per pixel
* avg_time_per_frame_in_100ns: frame duration in 100ns units
* interlaced: interlaced or not; zero: not interlaced, otherwise interlaced
**/
typedef
struct _tag_lc_video_info {
    int width_in_pixel;
    int height_in_pixel;
    unsigned long format_fourcc;
    int pixel_depth_in_bit;
    int avg_time_per_frame_in_100ns;
    int interlaced;
} lc_video_info_t;

/**
* video device declaration
* device_name: the device name, length limited to MAX_DEVICE_NAME
* info: the default video format information of this device
**/
typedef
struct _tag_lc_video_device {
    char device_name[LC_MAX_DEVICE_NAME];
    lc_video_info_t info;
} lc_video_device_t;

/**
* audio format information structure
* depth_in_bit: bit count per sample
* samples_per_second: sample count per second
* channels: audio channel count
**/
typedef
struct _tag_lc_audio_info {
    int depth_in_bit;
    int samples_per_second;
    int channels;
} lc_audio_info_t;

/**
* audio device declaration
* device_name: the device name, length limited to MAX_DEVICE_NAME
* info: the default audio format information of this device
**/
typedef
struct _tag_lc_audio_device {
    char device_name[LC_MAX_DEVICE_NAME];
    lc_audio_info_t info;
} lc_audio_device_t;

typedef
enum _tag_LC_DISPLAY_TYPE {
    DISPLAY_FULLSCREEN = 0,
    DISPLAY_WINDOW,
    DISPLAY_RECTANGLE
} LC_DISPLAY_TYPE;


typedef
struct _tag_lc_display_info {
    LC_DISPLAY_TYPE type;
    union {
        unsigned long hwnd;
        struct {
            int xpos;
            int ypos;
            int width;
            int height;
        } rect;
    } info;
} lc_display_info_t;

/**
* video input device information
* device_name : the device name used to recording video
* info: depends on device_name; if device_name is a video device,video_info is used, for display, display_info is used.
* pixel_dst_x : x offset relative to output video
* pixel_dst_y : y offset relative to output video
* pixel_width :   width in pixel for this device
* pixel_height :  height in pixel for this device
**/
typedef
struct _tag_lc_video_input {
    char device_name[LC_MAX_DEVICE_NAME];
    union {
        lc_video_info_t video_info;
        lc_display_info_t display_info;
    } info;
    int pixel_dst_x;
    int pixel_dst_y;
    int pixel_width;
    int pixel_height;
} lc_video_input_t;

/**
* audio input device information
* device_name : the device name used to recording audio
* volume : audio volume for this device
**/
typedef
struct _tag_lc_audio_input {
    char device_name[LC_MAX_DEVICE_NAME];
    lc_audio_info_t info;
    int volume;
} lc_audio_input_t;

/**
* audio codec parameters
**/
typedef
struct _tag_lc_audio_output {
    int bitRateInBps;
    int sampleRateInHz;
    int channels;
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
    int bitRateInBps;
    int maxFrameRate_x100;
    int maxWidthInPixel;
    int maxHeightInPixel;
    LC_VIDEO_SIZING sizingPolicy;
    lc_video_codec_options_t codecOptions;
} lc_video_output_t;

/**
* hls parameters
**/
typedef
struct _tag_lc_hls_config {
    int segmentTimeInSecond;
    int segmentListSize;
    bool adaptive;
} lc_hls_config_t;

/**
* rtmp parameters
**/
typedef
struct _tag_lc_rtmp_config_t {
    bool gopCache;
    bool playToken;
} lc_rtmp_config_t;

typedef enum _tag_LC_RECORDING_FORMAT {
    MP4 = 0,
    M3U8,
} LC_RECORDING_FORMAT;

typedef
struct _tag_lc_recording {
    LC_RECORDING_FORMAT format;
    int periodInMinute;
} lc_recording_t;


typedef
enum _tag_LC_IMAGE_TYPE {
    JPG = 0,
    PNG
} LC_IMAGE_TYPE;

typedef
enum _tag_LC_THUMBNAIL_MODE {
    MANUAL = 0,
    AUTO
} LC_THUMBNAIL_MODE;

typedef
struct _tag_lc_thumbnail_target {
    LC_IMAGE_TYPE format;
    LC_VIDEO_SIZING sizingPolicy;
    int widthInPixel;
    int heightInPixel;
} lc_thumbnail_target_t;

typedef
struct _tag_lc_thumbnail_capture {
    LC_THUMBNAIL_MODE mode;
    long startTimeInSecond;
    long endTimeInSecond;
    long intervalInSecond;
} lc_thumbnail_capture_t;

typedef
struct _tag_lc_thumbnail {
    lc_thumbnail_target_t target;
    lc_thumbnail_capture_t capture;
} lc_thumbnail_t;

/**
* BCE preset parameters
**/
typedef
struct _tag_lc_preset {
    char presetName[LC_MAX_PRESET_NAME];
    char description[LC_MAX_PRESET_DESC];
    bool forwardOnly;
    lc_audio_output_t audio;
    lc_video_output_t video;
    lc_hls_config_t hls;
    lc_rtmp_config_t rtmp;
    lc_recording_t recording;
    long long createTime;
    char userId[LC_MAX_USER_ID_LEN];
    lc_thumbnail_t thumbnail;
} lc_preset_t;

/**
* preset list
**/
typedef
struct _tag_lc_preset_list {
    int count;
    lc_preset_t* preset_list;
} lc_preset_list_t;

typedef
enum _tag_LC_STATUS {
    LC_STATUS_READY,
    LC_STATUS_STARTING,
    LC_STATUS_RUNNING,
    LC_STATUS_RETRYING,
    LC_STATUS_STOPPED,
} LC_STATUS;

typedef enum _tag_LC_CODE {
    LC_OK = 0,

    LC_ERROR_MASK = 0x80000000,
    LC_FAIL,
    LC_INVALID_ARG,
    LC_INVALID_HANDLE,
    LC_CALLBACK_NOT_FOUND,
    LC_NO_DEVICE,
    LC_DEVICE_ERR,
    LC_NO_PERMISSION,
    LC_INVALID_USERNAME_OR_PASSWORD,
    LC_NETWORK_ERR,
    LC_PRESET_NAME_CONFLICT,
    LC_INVALID_PRESET_NAME,
    LC_PRESET_NOT_FOUND,
    LC_OUT_OF_MEMORY,
    LC_TOO_MANY_ADUIO_DEVICES,
    LC_TOO_MANY_VIDEO_DEVICES,
    LC_GST_ERROR,
    LC_WRONG_STATE,
    LC_CREATE_THREAD_FAIL,
    LC_NO_SESSION,
    LC_GST_EOS,

    LC_GST_ERROR_MASK = 0xC0000000,

    LC_UNKNOWN = -1,
} LC_CODE;

const char* LC_API lc_get_error_message(LC_CODE code);
const char* LC_API lc_get_last_error();
typedef
void (LC_API* lc_status_callback_t)(lc_handle_t handle, LC_STATUS status, LC_CODE msg_code,
                                    void* user_data);

//////////////////////////////////////////////////////////////////////////
// memory alloc/free
void* LC_API lc_malloc(int size);
void  LC_API lc_free(void* mem);

//////////////////////////////////////////////////////////////////////////
// LOG API

typedef enum _tag_LC_LOGLEVEL {
    LC_LOG_DEBUG,
    LC_LOG_INFO,
    LC_LOG_WARN,
    LC_LOG_ERROR,
    LC_LOG_FATAL,
} LC_LOGLEVEL;

#define LC_LOG_DEFAULT_LEVEL LC_LOG_WARN

typedef
void (LC_API* lc_log_callback_t)(LC_LOGLEVEL level, const char* msg);

/**
* get log level of live-capture module
**/
LC_LOGLEVEL LC_API lc_log_get_level();

/**
* set log level of live-capture module
* new_level: new level to be set
* return : old level
**/
LC_LOGLEVEL LC_API lc_log_set_level(LC_LOGLEVEL new_level);

/**
* set user defined log handler instead of default
* callback : handler to be set
* return : old handler
**/
lc_log_callback_t LC_API lc_log_set_callback(lc_log_callback_t callback);

//////////////////////////////////////////////////////////////////////////
// Preset API

LC_CODE LC_API lc_preset_init(lc_preset_t* preset);


/**
* create a new live preset
* preset : parameters used to create a new preset
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_preset_create(lc_preset_t* preset);

/**
* delete an existing preset by preset name
* preset_name : the preset name to be deleted
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_preset_delete(const char* preset_name);

/**
* query preset by name
* preset_name: the preset name to be queried
* preset: a pointer to lc_preset_t used to receive preset data
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_preset_query(const char* preset_name, lc_preset_t* preset);

/**
* query all preset
* pp_preset_list : a pointer to lc_preset_list_t
* return : error code, LC_OK: success, others: failed
* NOTICE: if succeed, user should call lc_preset_list_free to free memory of pp_preset
**/
LC_CODE LC_API lc_preset_list(lc_preset_list_t** pp_preset_list);

/**
* free preset list returned by lc_preset_list
* preset_list: the struct pointer to be freed
* return : None
**/
void LC_API lc_preset_list_free(lc_preset_list_t* preset_list);

/**
* target for stream-recording
**/
typedef
struct _tag_lc_target {
    char bosBucket[LC_MAX_BOS_BUCKET_LEN];
    char userDomain[LC_MAX_USER_DOMAIN_LEN];
} lc_target_t ;

typedef
struct _tag_lc_session_publish_config {
    bool pushAuth;
} lc_session_publish_config_t;

/**
* session configuration structure used to create a new live session
**/
typedef
struct _tag_lc_session_config {
    char description[LC_MAX_SESSION_DESC];
    char presetName[LC_MAX_PRESET_NAME];
    lc_target_t target;
    lc_session_publish_config_t publish;
    char notification [LC_MAX_BUFFER];
} lc_session_config_t;

typedef
enum _tag_LC_SESSION_STATUS {
    SESSION_UNKNOWN,
    SESSION_READY,
    SESSION_ONGOING,
    SESSION_PAUSED,
    SESSION_CLOSED
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
    char hlsUrl[LC_MAX_URL_LEN];
    char rtmpUrl[LC_MAX_URL_LEN];
    char rtmpKey[LC_MAX_RTMP_KEY_LEN];
} lc_session_play_t;

typedef
struct _tag_lc_session_error {
    char code[LC_MAX_ERR_CODE];
    char message[LC_MAX_ERR_MSG];
} lc_session_error_t;

typedef
struct _tag_lc_session_records {
    char keyPrefix[LC_MAX_BUFFER];
} lc_session_records_t;
typedef
struct _tag_lc_session_thumbnails {
    char keyPrefix[LC_MAX_BUFFER];
} lc_session_thumbnail_t;


typedef
struct _tag_lc_session_publish {
    bool pushAuth;
    char pushUrl[LC_MAX_URL_LEN];
} lc_session_publish_t;

/**
* session information
**/
typedef
struct _tag_lc_session {
    char userId[LC_MAX_USER_ID_LEN];
    char sessionId[LC_MAX_SESSION_ID_LEN];
    lc_session_config_t config;
    long long createTime;
    long long lastUpdateTime;
    long long closedTime;
    lc_session_publish_t publish;
    lc_session_play_t play;
    LC_SESSION_STATUS status;
    LC_SESSION_STREAMING_STATUS streamingStatus;
    lc_session_error_t error;
    lc_session_records_t record;
    lc_session_thumbnail_t thumbnail;
    char notification[LC_MAX_BUFFER];
} lc_session_t;

typedef
struct _tag_lc_session_list {
    int count;
    lc_session_t* session_list;
} lc_session_list_t;


/**
* capture config, used to create a new capture handle
**/
typedef
struct _tag_lc_config {
    lc_video_input_t video_inputs[LC_MAX_VIDEO_INPUT];
    lc_audio_input_t audio_inputs[LC_MAX_AUDIO_INPUT];

    lc_preset_t preset;
    lc_session_config_t session;
} lc_config_t;

LC_CODE LC_API lc_session_config_init(lc_session_config_t* cfg);

/**
* create a new live session
* cfg : parameters used to create a new session
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_create(lc_session_config_t* cfg, lc_session_t* session);

/**
* query session by session id
* sessionId: the session id to be queried
* session: a pointer to lc_session_t used to receive session data
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_query(const char* sessionId, lc_session_t* session);


/**
* free session returned by lc_session_query
* session: the struct pointer to be freed
* return : None
**/
void LC_API lc_session_free(lc_session_t* session);

/**
* query all sessions
* pp_session : a pointer to lc_session_list_t
* return : error code, LC_OK: success, others: failed
* NOTICE: if succeed, user shall call lc_free to free memory of pp_session
**/
LC_CODE LC_API lc_session_list(lc_session_list_t** pp_session);

/**
* free session list returned by lc_session_list
* session_list: the struct pointer to be freed
* return : None
**/
void LC_API lc_session_list_free(lc_session_list_t* session_list);

/**
* stop live session by session id
* sessionId : the id of the session to be stopped
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_stop(const char* sessionId);

/**
* resume live session by session id
* sessionId : the id of the session to be stopped
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_resume(const char* sessionId);

/**
* stop current push-streaming, refresh the push url of live session by session id.
* sessionId: in, the id of the session whose url is going to be refreshed
* session: out, resulting refreshed session
* return: error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_refresh(const char* sessionId, lc_session_t* session);
/**
* delete live session by session id
* sessionId : the id of the session to be stopped
* return : error code, LC_OK: success, others: failed
**/
LC_CODE LC_API lc_session_delete(const char* sessionId);

/**
* setup BCE access key and initialize this live capture module
**/
LC_CODE LC_API lc_init(lc_bce_access_key_t* ak, const char* host);

/**
* shutdown this module
**/
void LC_API lc_deinit();

/**
* get system level video capture device count, including webcams, screens, and video capture cards;
* return: the count
**/
int LC_API lc_video_device_get_count();

/**
* get a video device by given device index
* index: the index of the device to be queried, start at 0
* device: pointer to receive data
* return: result code; LC_OK: success, others: failed
**/
LC_CODE LC_API lc_video_device_get_device(int index, lc_video_device_t* device);


/**
* callback type to enumerate video format information
* info: one video format
* user_data: user data passed to 'lc_video_device_enum_video_info'
* return: return LC_OK to continue to receive next video format.
*         return other to break enumeration.
**/
typedef
LC_CODE(LC_API* lc_video_info_enum_callback_t)(const lc_video_info_t* info, void* user_data);

/**
* enumerate video format information of a video device.
* device_name: the device name to be enumerated
* fun: callback used to receive video format information
* user_data: user specified data, it will be passed to callback function
* return: LC_OK: successful. others: failed.
**/
LC_CODE LC_API lc_video_device_enum_video_info(const char* device_name,
        lc_video_info_enum_callback_t fun,
        void* user_data);

/**
* get system level audio capture device count
* return: the count
**/
int LC_API lc_audio_device_get_count();

/**
* get a audio device by given device index
* index: the index of the device to be queried, start at 0
* device: pointer to receive data
* return: result code; LC_OK: success, others: failed
**/
LC_CODE LC_API lc_audio_device_get_device(int index, lc_audio_device_t* device);

/**
* session notification
* name:     the unique notification name
* endpoint: the endpoint to receive notification
**/
typedef
struct  _tag_lc_notification {
    char name[LC_MAX_NOTIFICATION_NAME_LEN];
    char endpoint[LC_MAX_URL_LEN];
} lc_notification_t;

/**
* notification list
* count: number of notifications
* list:  array of notifications
**/
typedef
struct _tag_lc_notification_list {
    int count;
    lc_notification_t* list;
} lc_notification_list_t;

/**
* create one notification
* nfn: in, the notification information
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_create(lc_notification_t* nfn);

/**
* query notification by name
* name, in, the notification name
* nfn: out, the notification information
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_query(const char* name, lc_notification_t* nfn);

/**
* delete notification by name
* name, in, the notification name
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_delete(const char* name);

/**
* list all notifications
* list, out, the notification list
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_list(lc_notification_list_t** list);

/**
* free memory of a notification list
* list, in, the notification list
* return : LC_OK
**/
LC_CODE LC_API lc_notification_list_free(lc_notification_list_t* list);

/**
* create a live capture object
* config: configuration used to create the capture object
* err_code: receive result code
* return: a handle to capture object if successful; or NULL if fail
**/
lc_handle_t LC_API lc_create(lc_config_t* config, LC_CODE* err_code);

/**
* duplicate capture configuration base on existing object handle
* handle: existing object handle whose configuration will be duplicated
* config: to receive configuration data
**/
LC_CODE LC_API lc_dup_config(lc_handle_t handle, lc_config_t* config);

/**
* re-config an existing handle
* handle:
* config:
* return:
**/
LC_CODE LC_API lc_reconfig(lc_handle_t handle, lc_config_t* config);

/**
* register/unregister status-changed callback
**/
LC_CODE LC_API lc_register_status_callback(lc_handle_t handle, lc_status_callback_t callback,
        void* user_data);
LC_CODE LC_API lc_unregister_status_callback(lc_handle_t handle, lc_status_callback_t callback);

/**
* get status of the handle
**/
LC_STATUS LC_API lc_get_status(lc_handle_t handle);

/**
* get play url of the handle
**/
LC_CODE LC_API lc_query_play_url(lc_handle_t handle, lc_session_play_t* play_url);

/**
* start live capture,
* handle: the live capture handle to be started
* bce_session_id: this parameter can be NULL or a valid BCE live session id;
*   if NULL, the module will create a new live session to BCE server.
*   if not NULL, it should be a valid session id to BCE server,and the module will query the session information with it
*               so the module can get a rtmp url to push live stream.
* target_rtmp: this parameter can be NULL or a valid RTMP server URL;
*   if NULL, the module will create a new live session or use the session specified by bce_session_id, and start to push live stream to cloud
*   if valid URL, the module will push live stream to this URL directly.
* local_path: this parameter can be NULL or a valid local full path;
*   if NULL, the live stream will not be saved on local disk
*   if valid path: the live stream will be saved on local disk with the given path
* return : result code,  LC_OK: success, others: failed
**/
LC_CODE LC_API lc_start(lc_handle_t handle, const char* bce_session_id, const char* target_rtmp,
                        const char* local_path);

/**
* stop live capture, close live session if created one
**/
LC_CODE LC_API lc_stop(lc_handle_t handle);

/**
* close the given live capture handle
**/
void LC_API lc_close(lc_handle_t handle);

/**
* video statistics
*   width: video width in pixels
*   height: video height in pixels
*   fourcc: video format, such as I420, YUY2...
*   negotiated_framerate_x100: negotiated frame rate
*   framerate_x100: actual,immediate frame rate
*   bitrate_in_bps: actual,immediate bitrate
*   avg_bitrate_in_bps: average bitrate
**/
typedef
struct _tag_lc_statistics_video {
    int width;
    int height;
    int fourcc;
    int negotiated_framerate_x100;
    int framerate_x100;
    int bitrate_in_bps;
    int avg_bitrate_in_bps;
} lc_statistics_video_t;

/**
* audio statistics
*   channels: count of audio channels
*   depth_in_bit: bit depth of PCM Quantification
*   bytes_per_sample: byte counts per audio sample
*   samples_per_second: audio frequency
*   bitrate_in_bps: actual,immediate bitrate
*   avg_bitrate_in_bps: average bitrate
**/
typedef
struct _tag_lc_statistics_audio {
    int channels;
    int depth_in_bit;
    int bytes_per_sample;
    int samples_per_second;
    int bitrate_in_bps;
    int avg_bitrate_in_bps;
} lc_statistics_audio_t;

/**
* stream statistics
*   start_time_utc: the time when streaming started
*   total_bytes: the total bytes of sent data
*   failed_count: the times it has retried.
*   bitrate_in_bps: actual,immediate bitrate
*   avg_bitrate_in_bps:  average bitrate
*   status: current status
*   video: video information
*   audio: audio information
**/
typedef
struct _tag_lc_statistics_stream {
    long long start_time_utc;
    long long total_bytes;
    int failed_count;
    int bitrate_in_bps;
    int avg_bitrate_in_bps;
    LC_STATUS status;
    lc_statistics_video_t video;
    lc_statistics_audio_t audio;
} lc_statistics_stream_t;

/**
* get statistic information of a given object handle.
* handle: in, the object whose information to be queried
* info: out, to receive queried information
* return: LC_OK if succeed, others if failed
**/
LC_CODE LC_API lc_statistics_get(lc_handle_t handle, lc_statistics_stream_t* info);

/**
* avc encoded video sample data
*   avcc_data: avc codec data
*   avcc_data_len: data length in bytes of avcc_data
*   sample_data: avc encoded sample data
*   sample_data_len: data length in bytes of sample_data
*   sample_timestamp_in_100ns: stream timestamp in 100ns unit
*   is_key_frame: 1 if this sample is I frame, 0 if this sample is a non-I frame
*   is_discontinuous: 1 if some samples has been dropped. 0 none sample was dropped.
**/
typedef
struct _tag_lc_sample_avc_video {
    const unsigned char* avcc_data;
    int avcc_data_len;
    const unsigned char* sample_data;
    int sample_data_len;
    long long sample_timestamp_in_100ns;
    int is_key_frame;
    int is_discontinuous;
} lc_sample_avc_video_t;

/**
* aac encoded audio sample data
*   aac_header_data: aac header data
*   avcc_data_len: data length in bytes of aac_header_data
*   sample_data: aac encoded sample data
*   sample_data_len: data length in bytes of sample_data
*   sample_timestamp_in_100ns: stream timestamp in 100ns unit
*   is_discontinuous: 1 if some samples has been dropped. 0 none sample was dropped.
**/
typedef
struct _tag_lc_sample_aac_audio {
    const unsigned char* aac_header_data;
    int aac_header_data_len;
    const unsigned char* sample_data;
    int sample_data_len;
    long long sample_timestamp_in_100ns;
    int is_discontinuous;
} lc_sample_aac_audio_t;

/**
* raw audio sample data
*   format: audio format information
*   sample_data: raw audio sample data
*   sample_data_len: data length in bytes of sample_data
*   sample_timestamp_in_100ns: stream timestamp in 100ns unit
**/
typedef
struct _tag_lc_sample_raw_audio {
    lc_audio_info_t format;
    const unsigned char* sample_data;
    int sample_data_len;
    long long sample_timestamp_in_100ns;
} lc_sample_raw_audio_t;

/**
* raw video sample data
*   format: video format information
*   sample_data: raw video sample data
*   sample_data_len: data length in bytes of sample_data
*   sample_timestamp_in_100ns: stream timestamp in 100ns unit
**/
typedef
struct _tag_lc_sample_raw_video {
    lc_video_info_t format;
    const unsigned char* sample_data;
    int sample_data_len;
    long long sample_timestamp_in_100ns;
} lc_sample_raw_video_t;

typedef
enum _tag_LC_SAMPLE_TYPE {
    LC_SAMPLE_AVC_VIDEO = 1,
    LC_SAMPLE_AAC_AUDIO = 2,
    LC_SAMPLE_RAW_VIDEO = 4,
    LC_SAMPLE_RAW_AUDIO = 8,
} LC_SAMPLE_TYPE;

typedef
void (LC_API* lc_sample_callback_t)(lc_handle_t handle, LC_SAMPLE_TYPE type, void* sample,
                                    void* usr_data);

/**
* register a callback function for video/audio data access.
*   handle: in, the capture object handle
*   callback: in, the callback function address
*   mask: in, one or more LC_SAMPLE_TYPE, indicate what kind of sample types will be passed to callback.
*   usr_data: in, user data, it will be passed back to callback.
**/
LC_CODE lc_sample_set_callback(lc_handle_t handle, lc_sample_callback_t callback,
                               int mask, void* usr_data);

#ifdef __cplusplus
};
#endif

#endif //LIVE_CAPTURE_LIVE_CAPTURE_H
