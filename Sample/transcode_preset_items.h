#ifndef __TRANSCODE_PRESET_H__
#define __TRANSCODE_PRESET_H__

static const PropertyNameValue s_video_codec_profiles[] = {
    {_T("默认"), -1},
    {_T("baseline"), BASELINE},
    {_T("main"), MAIN},
    {_T("high"), HIGH},
    { NULL,}
};

static const PropertyItem s_transcode_preset_video_codec_items[] = {
    {
        _T("profile"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, codec_options) + offsetof(lc_video_codec_options_t, profile),
        sizeof(LC_VIDEO_CODEC_PROFILE),
        read_int,
        write_int,
        NULL,
        s_video_codec_profiles,
        NULL,
        validate_int
    },
    {0,}
};

static const PropertyNameValue s_video_codecs[] = {
    {_T("默认"), -1},
    {_T("H264"), H264},
    { NULL,}
};
static const PropertyNameValue s_video_framerates[] = {
    {_T("10"), 1000},
    {_T("15"), 1500},
    {_T("20"), 2000},
    {_T("23.97"), 2397},
    {_T("24"), 2400},
    {_T("25"), 2500},
    {_T("29.97"), 2997},
    {_T("30"), 3000},
    {_T("50"), 5000},
    {_T("60"), 6000},
    { NULL,}
};
static const PropertyNameValue s_video_sizingpolicy[] = {
    {_T("默认"), -1},
    {_T("keep"), KEEP},
    {_T("shrinkToFit"), FIT},
    {_T("stretch"), STRETCH},
    NULL,
};

static const PropertyNameValue s_image_type[] = {
    {_T("默认"), -1},
    {_T("jpg"), JPG},
    {_T("png"), PNG},
    NULL,
};

static const PropertyNameValue s_bool[] = {
    {_T("默认"), -1},
    {_T("FALSE"), 0},
    {_T("TRUE"), 1},
    NULL,
};
static const PropertyItem s_transcode_preset_video_items[] = {
    {
        _T("编码方式"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, codec),
        sizeof(LC_VIDEO_CODEC) ,
        read_int,
        write_int,
        NULL,
        s_video_codecs,
        NULL,
        validate_int
    },
    {_T("编码参数"), 0, 0, NULL, NULL, NULL, NULL, s_transcode_preset_video_codec_items},

    {
        _T("码率(bps)"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, bitrate_in_bps),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL,
        validate_int
    },
    {
        _T("帧率"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, max_framerate_x100),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        s_video_framerates,
        NULL,
        validate_int
    },
    {
        _T("画面宽(pixels)"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, max_width_in_pixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL,
        validate_int
    },
    {
        _T("画面高(pixels)"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, max_height_in_pixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL,
        validate_int
    },
    {
        _T("缩放方式"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, sizing_policy),
        sizeof(LC_VIDEO_SIZING) ,
        read_int,
        write_int,
        NULL,
        s_video_sizingpolicy,
        NULL,
        validate_int
    },
    { 0, }
};

static const PropertyItem s_transcode_preset_hls_items[] = {
    {
        _T("切片时长(秒)"),
        offsetof(lc_transcode_preset_t, hls) + offsetof(lc_hls_config_t, segment_time_in_second),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL,
        validate_int
    },
    {
        _T("M3U8切片个数"),
        offsetof(lc_transcode_preset_t, hls) + offsetof(lc_hls_config_t, segment_list_size),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL,
        validate_int
    },
    {
        _T("自适应码率"),
        offsetof(lc_transcode_preset_t, hls) + offsetof(lc_hls_config_t, adaptive),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL,
        validate_int
    },

    {0,}
};

static const PropertyItem s_transcode_preset_rtmp_items[] = {
    {
        _T("缓存GOP"),
        offsetof(lc_transcode_preset_t, rtmp) + offsetof(lc_rtmp_config_t, gop_cache),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL,
        validate_int
    },
    {0,}
};


static const PropertyNameValue s_valid_audio_sample_rate[] = {
    {_T("与输入一致"), -1},
    {_T("22050"), 22050},
    {_T("32000"), 32000},
    {_T("44100"), 44100},
    {_T("48000"), 48000},
    {_T("96000"), 96000},
    {NULL,}
};

static const PropertyNameValue s_valid_audio_channels[] = {
    {_T("与输入一致"), -1},
    {_T("1"), 1},
    {_T("2"), 2},
    {NULL,}
};

static const PropertyItem s_transcode_preset_audio_items[] = {
    {
        _T("码率(bps)"),
        offsetof(lc_transcode_preset_t, audio) + offsetof(lc_audio_output_t, bitrate_in_bps),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL,
        validate_int
    },
    {
        _T("采样率(Hz)"),
        offsetof(lc_transcode_preset_t, audio) + offsetof(lc_audio_output_t, sample_rate_in_hz),
        sizeof(int),
        read_int,
        write_int,
        NULL,
        s_valid_audio_sample_rate,
        NULL,
        validate_int
    },
    {
        _T("声道"),
        offsetof(lc_transcode_preset_t, audio) + offsetof(lc_audio_output_t, channels),
        sizeof(int),
        read_int,
        write_int,
        NULL,
        s_valid_audio_channels,
        NULL,
        validate_int
    },
    { 0,}
};

// preset property map
static const PropertyItem s_transcode_preset_items[] = {
    {
        _T("模板名称"),
        offsetof(lc_transcode_preset_t, name),
        LC_MAX_PRESET_NAME,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string
    },
    {
        _T("模板描述"),
        offsetof(lc_transcode_preset_t, description),
        LC_MAX_PRESET_DESC,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string
    },
    {
        _T("ForwardOnly"),
        offsetof(lc_transcode_preset_t, forward_only),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL,
        validate_int,
    },
    {
        _T("视频"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_transcode_preset_video_items,
        validate_bool
    },
    {
        _T("音频"),
        offsetof(lc_transcode_preset_t, audio) + offsetof(lc_audio_output_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_transcode_preset_audio_items,
        validate_bool
    },
    {
        _T("HLS"),
        offsetof(lc_transcode_preset_t, hls) + offsetof(lc_hls_config_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_transcode_preset_hls_items,
        validate_bool
    },
    {
        _T("RTMP"),
        offsetof(lc_transcode_preset_t, rtmp) + offsetof(lc_rtmp_config_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_transcode_preset_rtmp_items,
        validate_bool
    },
    {
        _T("创建时间"),
        offsetof(lc_transcode_preset_t, create_time),
        sizeof(lc_datetime_t),
        read_lc_datetime,
        NULL,
        NULL,
        NULL,
        NULL,
        validate_lc_datetime
    },
    { 0,}
};


#endif //__TRANSCODE_PRESET_H__