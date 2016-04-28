// DialogUserPreset.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "DialogUserPreset.h"

static const PropertyNameValue s_video_codec_profiles[] = {
    {_T("Ĭ��"), -1},
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
    {_T("Ĭ��"), -1},
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
    {_T("Ĭ��"), -1},
    {_T("keep"), KEEP},
    {_T("shrinkToFit"), FIT},
    {_T("stretch"), STRETCH},
    NULL,
};
static const PropertyNameValue s_bool[] = {
    {_T("Ĭ��"), -1},
    {_T("FALSE"), 0},
    {_T("TRUE"), 1},
    NULL,
};
static const PropertyItem s_transcode_preset_video_items[] = {
    {
        _T("���뷽ʽ"),
        offsetof(lc_transcode_preset_t, video) + offsetof(lc_video_output_t, codec),
        sizeof(LC_VIDEO_CODEC) ,
        read_int,
        write_int,
        NULL,
        s_video_codecs,
        NULL,
        validate_int
    },
    {_T("�������"), 0, 0, NULL, NULL, NULL, NULL, s_transcode_preset_video_codec_items},

    {
        _T("����(bps)"),
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
        _T("֡��"),
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
        _T("�����(pixels)"),
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
        _T("�����(pixels)"),
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
        _T("���ŷ�ʽ"),
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

static const PropertyNameValue s_valid_audio_sample_rate[] = {
    {_T("������һ��"), -1},
    {_T("22050"), 22050},
    {_T("32000"), 32000},
    {_T("44100"), 44100},
    {_T("48000"), 48000},
    {_T("96000"), 96000},
    {NULL,}
};

static const PropertyNameValue s_valid_audio_channels[] = {
    {_T("������һ��"), -1},
    {_T("1"), 1},
    {_T("2"), 2},
    {NULL,}
};

static const PropertyItem s_transcode_preset_audio_items[] = {
    {
        _T("����(bps)"),
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
        _T("������(Hz)"),
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
        _T("����"),
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

static const PropertyItem s_encoding_preset_items[] = {

    {
        _T("��Ƶ"),
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
        _T("��Ƶ"),
        offsetof(lc_transcode_preset_t, audio) + offsetof(lc_audio_output_t, is_valid),
        sizeof(bool),
        NULL,
        write_expended,
        NULL,
        s_bool,
        s_transcode_preset_audio_items,
        validate_bool
    },
    { 0,}
};

// CDialogUserPreset dialog

IMPLEMENT_DYNAMIC(CDialogUserPreset, CPropertyGridDialog)

CDialogUserPreset::CDialogUserPreset(lc_transcode_preset_t* preset, CWnd* pParent /*=NULL*/)
    : CPropertyGridDialog(CDialogUserPreset::IDD, pParent)
    , m_pPreset(preset) {
    m_sNewPreset = *preset;
}

CDialogUserPreset::~CDialogUserPreset() {
}

void CDialogUserPreset::DoDataExchange(CDataExchange* pDX) {
    CPropertyGridDialog::DoDataExchange(pDX);
}

BOOL CDialogUserPreset::OnInitDialog() {
    BOOL bRet = CPropertyGridDialog::OnInitDialog();

    CreatePropertyGrid();

    UpdatePropertyList(&m_sNewPreset, TRUE);
    return bRet;
}

void CDialogUserPreset::GetPropertyGridRect(LPRECT rc) {
    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);

    rcClient.DeflateRect(3, 3, 3, 70);

    *rc = rcClient;
}
const PropertyItem* CDialogUserPreset::GetPropertyList() {
    return &s_encoding_preset_items[0];
}

BEGIN_MESSAGE_MAP(CDialogUserPreset, CPropertyGridDialog)
    ON_BN_CLICKED(IDOK, &CDialogUserPreset::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDialogUserPreset::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDialogUserPreset message handlers

void CDialogUserPreset::OnBnClickedOk() {
    OnOK();
}

void CDialogUserPreset::OnBnClickedCancel() {
    OnCancel();
}

BOOL CDialogUserPreset::ValidPreset(lc_transcode_preset_t* preset) {

    BOOL bRet = FALSE;
    CString strMsg;

    static const int valid_audio_rates[] = {22050, 32000, 44100, 48000, 96000};
    static const int valid_video_rates[] = {1000, 1500, 2000, 2397, 2400, 2500, 2997, 3000, 5000, 6000};
    BOOL valid;

    do {
        // audio check
        if (!preset->audio.is_valid) {
            strMsg = _T("��Ч��Ƶ�������");
            break;
        }

        if (preset->audio.bitrate_in_bps < 1000 || preset->audio.bitrate_in_bps > 1000000) {
            strMsg.Format(_T("��Ч��Ƶ����, �����ý���1000~1000000֮���ֵ"));
            break;
        }

        if (preset->audio.channels != 1 && preset->audio.channels != 2 && preset->audio.channels != -1) {
            strMsg.Format(_T("��Ч��Ƶ��������Чֵ�У�1, 2, ������һ��"));
            break;
        }

        valid = FALSE;

        for (int i = 0 ; i < sizeof(valid_audio_rates) / sizeof(valid_audio_rates[0]); i ++) {
            if (preset->audio.sample_rate_in_hz == valid_audio_rates[i]) {
                valid = TRUE;
                break;
            }
        }

        if (!valid) {
            if (preset->audio.sample_rate_in_hz != -1) {
                strMsg.Format(_T("��Ч��Ƶ�ɼ��ʣ���Чֵ��:"));

                for (int i = 0 ; i < sizeof(valid_audio_rates) / sizeof(valid_audio_rates[0]); i ++) {

                    strMsg.AppendFormat(_T("%d, "), valid_audio_rates[i]);
                }

                strMsg += "������һ��";

                break;
            }
        }

        if (!preset->video.is_valid) {
            strMsg = _T("��Ч��Ƶ�������");
            break;
        }

        if (preset->video.bitrate_in_bps < 32000 || preset->video.bitrate_in_bps > 2000000) {
            strMsg.Format(_T("��Ч��Ƶ����, �����ý���32000~2000000֮���ֵ"));
            break;
        }

        if (preset->video.max_width_in_pixel < 128 || preset->video.max_width_in_pixel > 4096
                || (preset->video.max_width_in_pixel & 1)) {
            strMsg.Format(_T("��Ч��Ƶ��ȣ�������128~4096֮���ż��"));
            break;
        }

        if (preset->video.max_height_in_pixel < 96 || preset->video.max_height_in_pixel > 3072
                || (preset->video.max_height_in_pixel & 1)) {
            strMsg.Format(_T("��Ч��Ƶ�߶ȣ�������96~3072֮���ż��"));
            break;
        }

        valid = FALSE;

        for (int i = 0 ; i < sizeof(valid_video_rates) / sizeof(valid_video_rates[0]); i ++) {
            if (preset->video.max_framerate_x100 == valid_video_rates[i]) {
                valid = TRUE;
                break;
            }
        }

        if (!valid) {
            strMsg.Format(_T("��Ч��Ƶ֡�ʣ���Чֵ��:"));

            for (int i = 0 ; i < sizeof(valid_video_rates) / sizeof(valid_video_rates[0]); i ++) {

                strMsg.AppendFormat(_T("%.2f, "), valid_video_rates[i] / 100.f);
            }

            strMsg.Delete(strMsg.GetLength() - 2, 2);

            break;

        }

        bRet = TRUE;

    } while (0);

    if (!bRet && !strMsg.IsEmpty()) {
        MessageBox(strMsg);
    }

    return bRet;
}

void CDialogUserPreset::OnOK() {
    BuildFromPropertyList(&m_sNewPreset);

    if (memcmp(m_pPreset, &m_sNewPreset, sizeof(lc_transcode_preset_t)) == 0) {
        CPropertyGridDialog::OnCancel();
    } else {
        if (ValidPreset(&m_sNewPreset)) {
            *m_pPreset = m_sNewPreset;
            CPropertyGridDialog::OnOK();
        }
    }
}

void CDialogUserPreset::OnCancel() {
    CPropertyGridDialog::OnCancel();
}