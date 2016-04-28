#ifndef __SESSION_ITEMS_H__
#define __SESSION_ITEMS_H__

static const PropertyNameValue s_bool[] = {
    {_T("Ĭ��"), -1},
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
        _T("����"),
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
        _T("����ģʽ"),
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
        _T("��������"),
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
        _T("����"),
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
        _T("��Ϣ"),
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
    {_T("����"), SESSION_READY},
    {_T("����ֱ��"), SESSION_ONGOING},
    {_T("��ͣ"), SESSION_PAUSED},
    {_T("ʧ��"), SESSION_FAILED},
    {_T("��������"), SESSION_CONNECTING},
    {0,}
};

static const PropertyNameValue s_valid_session_streaming_status[] = {
    {_T(""), STREAMING_UNKNOWN},
    {_T("��������"), STREAMING_STREAMING},
    {_T("����"), STREAMING_NO_SOURCE},
    {_T("ʧ��"), STREAMING_FAILED},
    {0,}
};

static const PropertyItem s_session_items[] = {
    {
        _T("�ỰID"),
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
        _T("�Ự����"),
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
        _T("ת��ģ��"),
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
        _T("¼��ģ��"),
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
        _T("״̬֪ͨ"),
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
        _T("��ȫ����"),
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
        _T("�Ự����"),
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, publish) + offsetof(lc_session_publish_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_session_publish_items,
        validate_bool
    },

    {_T("���ŵ�ַ"), 0, 0, NULL, NULL, NULL, NULL, s_session_playurl_items, validate_subitems },
    {
        _T("�Ự״̬"),
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
        _T("����״̬"),
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
        _T("������Ϣ"),
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
        _T("����ʱ��"),
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