/**
* Copyright (C) 2016, Baidu Inc.
* file  live_capture.h
* brief 百度开放云 LSS SDK.
* author wangzhao04
* version v2.0
* date    2015
* History:
*      wangzhao04: 2015/7/20, created
*      wangzhao04: 2015/9/23, add statistics;
*                             add video&audio data callback
*      wangzhao04: 2016/3/3, add region&HWND recording
*      wangzhao04: 2016/3/11, BCE LSS v2.0 support
**/

#ifndef LIVE_CAPTURE_LIVE_CAPTURE_H
#define LIVE_CAPTURE_LIVE_CAPTURE_H


#include "lc_types.h"
#include "lc_transcode_preset.h"
#include "lc_session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LC_MODULE_NAME "LiveCapture"

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


typedef
enum _tag_LC_STATUS {
    LC_STATUS_READY,
    LC_STATUS_STARTING,
    LC_STATUS_RUNNING,
    LC_STATUS_RETRYING,
    LC_STATUS_STOPPED,
} LC_STATUS;


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



/**
* capture config, used to create a new capture handle
**/

typedef
struct _tag_lc_encoding_config {
    lc_video_output_t video;
    lc_audio_output_t audio;
} lc_encoding_config_t;


typedef
struct _tag_lc_config {
    lc_video_input_t video_inputs[LC_MAX_VIDEO_INPUT];
    lc_audio_input_t audio_inputs[LC_MAX_AUDIO_INPUT];

    lc_encoding_config_t encoding;
    lc_session_config_t session;
} lc_config_t;

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
* get play url of the handle,
* if play auth is turned on, expire date will be 9999-12-32 23:59:59
**/
LC_CODE LC_API lc_query_play_url(lc_handle_t handle, lc_session_play_t* play_url);


/**
* get play url of the handle with the expire date
* if play auth is not turned on, expire is ignored.
**/
LC_CODE LC_API lc_query_play_url_ex(lc_handle_t handle, lc_session_play_t* play_url, lc_datetime_t* expire);


/**
* start live capture,
* handle: the live capture handle to be started
* lss_session_id: this parameter can be NULL or a valid BCE live session id;
*   if NULL, the module will create a new live session to BCE server.
*   if not NULL, it should be a valid session id to BCE server,and the module will query the session information with it
*               so the module can get a rtmp url to push live stream.
* target_rtmp: this parameter can be NULL or a valid RTMP server URL;
*   if NULL, the module will create a new live session or use the session specified by lss_session_id, and start to push live stream to cloud
*   if valid URL, the module will push live stream to this URL directly.
* local_path: this parameter can be NULL or a valid local full path;
*   if NULL, the live stream will not be saved on local disk
*   if valid path: the live stream will be saved on local disk with the given path
* return : result code,  LC_OK: success, others: failed
**/
LC_CODE LC_API lc_start(lc_handle_t handle, const char* lss_session_id, const char* target_rtmp,
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
