#ifndef LIVE_CAPTURE_LC_SECURITY_POLICY_H
#define LIVE_CAPTURE_LC_SECURITY_POLICY_H

#include "lc_types.h"

#ifdef __cplusplus
extern "C" {
#endif


//////////////////////////////////////////////////////////////////////////
//security policy

typedef
struct _tag_lc_auth {
    lc_bool_t push;
    lc_bool_t play;
    char key[LC_MAX_BUFFER_64];

    bool is_valid;
} lc_auth_t;

typedef
struct _tag_lc_white_black_list {
    lc_list_t white_list;
    lc_list_t black_list;
} lc_white_black_list;

typedef
struct _tag_lc_anti_leech {
    lc_white_black_list refer;
    lc_white_black_list ip;

    bool is_valid;
} lc_anti_leech_t;

typedef
enum _tag_LC_ENCRYPTION_STRATEGY {
    PLAYER_BINDING,
    SAFE_CODE
} LC_ENCRYPTION_STRATEGY;

typedef
enum _tag_LC_PLAYER_TYPE {
    WEB,
    IOS,
    ANDROID
} LC_PLAYER_TYPE;

typedef
struct _tag_lc_safe_code {
    LC_PLAYER_TYPE player_type;
    char safe_code[LC_MAX_BUFFER_32];
} lc_safe_code_t;

typedef
struct _tag_lc_hls_encryption {
    LC_ENCRYPTION_STRATEGY strategy;
    lc_list_t safe_codes;

    bool is_valid;
} lc_hls_encryption_t;


typedef
struct _tag_lc_encryption {
    lc_hls_encryption_t hls;

    bool is_valid;
} lc_encryption_t;


typedef
struct _tag_lc_security_policy  {
    char name[LC_MAX_BUFFER_128];
    lc_auth_t auth;
    lc_anti_leech_t anti_leech;
    lc_encryption_t encryption;
    lc_datetime_t create_time;
} lc_security_policy_t;

/**
* query security policy information by name
* policy_name: the policy name
* policy: to store policy information. it SHOULD BE freed by calling "lc_security_policy_free"
* return LC_OK if succeeded, LC_FAIL if failed. call "lc_get_last_error" for detailed information
**/
LC_CODE LC_API lc_security_policy_query(const char* policy_name, lc_security_policy_t* policy);

/**
* list all security policies
* policy_list: contains all the policies.
*        call "lc_security_policy_free" for each element in policy_list.
*        call "lc_list_free" at last.
* return LC_OK if succeeded, LC_FAIL if failed. call "lc_get_last_error" for detailed information
**/
LC_CODE LC_API lc_security_policy_list(lc_list_t* policy_list);

/**
* update security policy
* return LC_OK if succeeded, LC_FAIL if failed. call "lc_get_last_error" for detailed information
**/
LC_CODE LC_API lc_security_policy_update(lc_security_policy_t* policy);

/**
* free memory of a security policy object
**/
LC_CODE LC_API lc_security_policy_free(lc_security_policy_t* policy);

#ifdef __cplusplus
}
#endif
#endif // LIVE_CAPTURE_LC_SECURITY_POLICY_H