#ifndef LIVE_CAPTURE_LC_TYPES_H
#define LIVE_CAPTURE_LC_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif


#define LC_API __stdcall

typedef char lc_bool_t;


/**
* module level constants definition
*    LC_MAX_DEVICE_NAME: max device name length
*    LC_MAX_PRESET_NAME: max preset name length
*    LC_MAX_PRESET_DESC: max preset description length
*    LC_MAX_SESSION_ID_LEN:  max session id length
*    LC_MAX_SESSION_DESC: max session description length
*    LC_MAX_BOS_BUCKET_LEN: max BOS bucket path length
*    LC_MAX_URL_LEN: max URL length
*    LC_MAX_ERR_CODE: max ERR code length
*    LC_MAX_ERR_MSG: max error message length
*    LC_MAX_BCE_ACCESS_KEY_ID_LEN: max buffer length of BCE access key id string
*    LC_MAX_BCE_SECRET_ACCESS_KEY_LEN: max buffer length of BCE access key secret key string
*    LC_MAX_VIDEO_INPUT: max video input devices supported by one stream
*    LC_MAX_AUDIO_INPUT: max audio input devices supported by one stream
*    LC_MAX_NOTIFICATION_NAME_LEN: max length of notification's name
*    LC_MAX_BUFFER_8:
*    LC_MAX_BUFFER_16:
*    LC_MAX_BUFFER_32:
*    LC_MAX_BUFFER_64:
*    LC_MAX_BUFFER_128:
*    LC_MAX_BUFFER_256:
**/
typedef
enum _tag_LC_CONSTANTS {
    LC_MAX_DEVICE_NAME = 256,
    LC_MAX_PRESET_NAME = 128,
    LC_MAX_PRESET_DESC = 256,
    LC_MAX_SESSION_ID_LEN = 64,
    LC_MAX_SESSION_DESC = 256,
    LC_MAX_BOS_BUCKET_LEN = 256,
    LC_MAX_URL_LEN = 1024,
    LC_MAX_ERR_CODE = 32,
    LC_MAX_ERR_MSG = 256,
    LC_MAX_BCE_ACCESS_KEY_ID_LEN = 128,
    LC_MAX_BCE_SECRET_ACCESS_KEY_LEN = 128,
    LC_MAX_VIDEO_INPUT = 2,
    LC_MAX_AUDIO_INPUT = 2,
    LC_MAX_NOTIFICATION_NAME_LEN = 64,
    LC_MAX_BUFFER_8 = 8,
    LC_MAX_BUFFER_16 = 16,
    LC_MAX_BUFFER_32 = 32,
    LC_MAX_BUFFER_64 = 64,
    LC_MAX_BUFFER_128 = 128,
    LC_MAX_BUFFER_256 = 256,
} LC_CONSTANTS;


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
    LC_TRANSCODE_PRESET_NAME_CONFLICT,
    LC_INVALID_PRESET_NAME,
    LC_TRANSCODE_PRESET_NOT_FOUND,
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

typedef
enum _tag_LC_IMAGE_TYPE {
    JPG = 0,
    PNG,
    GIF
} LC_IMAGE_TYPE;

typedef
struct _tag_lc_list* lc_list_t;

int LC_API lc_list_count(lc_list_t list);

void* LC_API lc_list_get_at(lc_list_t list, int pos);

void LC_API lc_list_append(lc_list_t* list, const void* data, int data_len);

void LC_API lc_list_append_direct(lc_list_t* list, void* data);

bool LC_API lc_list_remove(lc_list_t list, const void* data);

void LC_API lc_list_free(lc_list_t list);

typedef
struct _tag_lc_date {
    int year;
    int month;
    int day;
} lc_date_t;

typedef
struct _tag_lc_datetime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} lc_datetime_t;


#ifdef __cplusplus
}
#endif

#endif //LIVE_CAPTURE_LC_TYPES_H