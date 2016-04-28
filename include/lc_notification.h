#ifndef LIVE_CAPTURE_LC_NOTIFICATION_H
#define LIVE_CAPTURE_LC_NOTIFICATION_H

#include "lc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* session notification
* name:     the unique notification name
* endpoint: the endpoint to receive notification
**/
typedef
struct  _tag_lc_notification {
    char name[LC_MAX_NOTIFICATION_NAME_LEN];
    char endpoint[LC_MAX_URL_LEN];
    lc_datetime_t create_time;
} lc_notification_t;


/**
* create one notification
* notification: in, the notification information
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_create(lc_notification_t* notification);

/**
* query notification by name
* name, in, the notification name
* notification: out, the notification information
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_query(const char* name, lc_notification_t* notification);

/**
* delete notification by name
* name, in, the notification name
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_delete(const char* name);

/**
* list all notifications
* notification_list, out, the notification list
* return : LC_OK if succeed, LC_FAIL if failed. call lc_get_last_error() to get detailed error message.
**/
LC_CODE LC_API lc_notification_list(lc_list_t* notification_list);


#ifdef __cplusplus
}
#endif

#endif //LIVE_CAPTURE_LC_NOTIFICATION_H