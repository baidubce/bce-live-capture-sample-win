#ifndef __SESSION_ITEMS_H__
#define __SESSION_ITEMS_H__

static const PropertyNameValue s_bool[] = {
    {_T("默认"), -1},
    {_T("FALSE"), 0},
    {_T("TRUE"), 1},
    NULL,
};

static const PropertyItem s_session_playurl_items[] = {
    {
        _T("HLS"),
        offsetof(lc_session_t, play) + offsetof(lc_session_play_t, hls_url),
        LC_MAX_URL_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
       _T("rtmp"),
        offsetof(lc_session_t, play) + offsetof(lc_session_play_t, rtmp_url),
        LC_MAX_URL_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {0,}
};
static const PropertyItem s_session_publish_items[] = {
    {
        _T("区域"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, publish) + offsetof(lc_session_publish_t, region),
        LC_MAX_BUFFER_32 ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("拉流模式"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, publish) + offsetof(lc_session_publish_t, is_pull),
        sizeof(lc_bool_t) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL,
        validate_int
    },
    {
        _T("URL"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, publish) + offsetof(lc_session_publish_t, url),
        LC_MAX_URL_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("推流名称"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, publish) + offsetof(lc_session_publish_t, push_stream),
        LC_MAX_BUFFER_64,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string
    },
    {0,}
};

static const PropertyItem s_session_error_items[] = {
    {
        _T("代码"),
        offsetof(lc_session_t, error) + offsetof(lc_error_t, code),
        LC_MAX_ERR_CODE ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("信息"),
        offsetof(lc_session_t, error) + offsetof(lc_error_t, message),
        LC_MAX_ERR_MSG ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {0,}
};


static const PropertyNameValue s_valid_session_status[] = {
    {_T(""), SESSION_UNKNOWN},
    {_T("就绪"), SESSION_READY},
    {_T("正在直播"), SESSION_ONGOING},
    {_T("暂停"), SESSION_PAUSED},
    {_T("失败"), SESSION_FAILED},
    {_T("正在连接"), SESSION_CONNECTING},
    {0,}
};

static const PropertyNameValue s_valid_session_streaming_status[] = {
    {_T(""), STREAMING_UNKNOWN},
    {_T("正在推流"), STREAMING_STREAMING},
    {_T("空闲"), STREAMING_NO_SOURCE},
    {_T("失败"), STREAMING_FAILED},
    {0,}
};

static const PropertyItem s_session_items[] = {
    {
        _T("会话ID"),
        offsetof(lc_session_t, session_id),
        LC_MAX_SESSION_ID_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("会话描述"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, description),
        LC_MAX_SESSION_DESC,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("转码模板"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, preset),
        LC_MAX_PRESET_NAME,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("录制模板"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, recording),
        LC_MAX_BUFFER_128,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("状态通知"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, notification),
        LC_MAX_BUFFER_128,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("安全策略"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, security_policy),
        LC_MAX_BUFFER_128,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string,
    },
    {
        _T("会话配置"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, publish) + offsetof(lc_session_publish_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_session_publish_items,
        validate_bool
    },

    {_T("播放地址"), 0, 0, NULL, NULL, NULL, NULL, s_session_playurl_items, validate_subitems },
    {
        _T("会话状态"),
        offsetof(lc_session_t, status),
        sizeof(LC_SESSION_STATUS),
        read_int,
        write_int,
        NULL,
        s_valid_session_status,
        NULL,
        validate_int,
    },
    {
        _T("推流状态"),
        offsetof(lc_session_t, streaming_status),
        sizeof(LC_SESSION_STREAMING_STATUS),
        read_int,
        write_int,
        NULL,
        s_valid_session_streaming_status,
        NULL,
        validate_int,
    },
    {
        _T("错误信息"),
        offsetof(lc_session_t, error) + offsetof(lc_error_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_session_error_items,
        validate_bool
    },
    {
        _T("创建时间"),
        offsetof(lc_session_t, create_time),
        sizeof(lc_datetime_t),
        read_lc_datetime,
        NULL,
        NULL,
        NULL,
        NULL,
        validate_lc_datetime,
    },
    { 0,}
};
#endif //__SESSION_ITEMS_H__