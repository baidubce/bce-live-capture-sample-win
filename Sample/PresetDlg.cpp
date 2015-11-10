#include "stdafx.h"
#include "Sample.h"
#include "PresetDlg.h"
#include "ToolsDlg.h"
#include "afxpropertygridctrl.h"
#include "choosepresetdlg.h"
#include <stack>

#define ID_DELETE 0x8001

static const PropertyNameValue s_video_codec_profiles[] = {
    { "baseline", BASELINE },
    { "main", MAIN },
    { "high", HIGH },
    { NULL,}
};

static const PropertyItem s_preset_video_codec_items[] = {
    {
        "profile",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, codecOptions) + offsetof(lc_video_codec_options_t, profile),
        sizeof(LC_VIDEO_CODEC_PROFILE),
        read_int,
        write_int,
        NULL,
        s_video_codec_profiles,
        NULL
    },
    {0,}
};

static const PropertyNameValue s_video_codecs[] = {
    { "H264", H264 },
    { NULL,}
};
static const PropertyNameValue s_video_framerates[] = {
    {"10", 1000},
    {"15", 1500},
    {"20", 2000},
    {"23.97", 2397},
    {"24", 2400},
    {"25", 2500},
    {"29.97", 2997},
    {"30", 3000},
    {"50", 5000},
    {"60", 6000},
    { NULL,}
};
static const PropertyNameValue s_video_sizingpolicy[] = {
    {"keep", KEEP},
    {"shrinkToFit", FIT},
    {"stretch", STRETCH},
    NULL,
};

static const PropertyNameValue s_image_type[] = {
    {"jpg", JPG},
    {"png", PNG},
    NULL,
};

static const PropertyNameValue s_bool[] = {
    {"FALSE", 0},
    {"TRUE", 1},
    NULL,
};
static const PropertyItem s_preset_video_items[] = {
    {
        "���뷽ʽ",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, codec),
        sizeof(LC_VIDEO_CODEC) ,
        read_int,
        write_int,
        NULL,
        s_video_codecs,
        NULL
    },
    {"�������", 0, 0, NULL, NULL, NULL, NULL, s_preset_video_codec_items},

    {
        "����(bps)",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, bitRateInBps),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "֡��",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, maxFrameRate_x100),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        s_video_framerates,
        NULL
    },
    {
        "�����(pixels)",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, maxWidthInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "�����(pixels)",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, maxHeightInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "���ŷ�ʽ",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, sizingPolicy),
        sizeof(LC_VIDEO_SIZING) ,
        read_int,
        write_int,
        NULL,
        s_video_sizingpolicy,
        NULL
    },
    { 0, }
};

static const PropertyItem s_preset_hls_items[] = {
    {
        "��Ƭʱ��(��)",
        offsetof(lc_preset_t, hls) + offsetof(lc_hls_config_t, segmentTimeInSecond),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "M3U8��Ƭ����",
        offsetof(lc_preset_t, hls) + offsetof(lc_hls_config_t, segmentListSize),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "����Ӧ����",
        offsetof(lc_preset_t, hls) + offsetof(lc_hls_config_t, adaptive),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL
    },

    {0,}
};

static const PropertyItem s_preset_rtmp_items[] = {
    {
        "����GOP",
        offsetof(lc_preset_t, rtmp) + offsetof(lc_rtmp_config_t, gopCache),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL
    },
    {
        "������֤",
        offsetof(lc_preset_t, rtmp) + offsetof(lc_rtmp_config_t, playToken),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL
    },
    {0,}
};

static const PropertyNameValue s_valid_recording_formats[] = {
    { "MP4", MP4 },
    { "M3U8", M3U8},
    {NULL,}
};
static const PropertyItem s_preset_recording_items[] = {
    {
        "�ļ���ʽ",
        offsetof(lc_preset_t, recording) + offsetof(lc_recording_t, format),
        sizeof(LC_RECORDING_FORMAT) ,
        read_int,
        write_int,
        NULL,
        s_valid_recording_formats,
        NULL
    },
    {
        "�ֶ�ʱ��(����)",
        offsetof(lc_preset_t, recording) + offsetof(lc_recording_t, periodInMinute),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {0,}
};

static const PropertyNameValue s_valid_audio_sample_rate[] = {
    {"0", 0},
    {"22050", 22050},
    {"32000", 32000},
    {"44100", 44100},
    {"48000", 48000},
    {"96000", 96000},
    {NULL,}
};

static const PropertyNameValue s_valid_audio_channels[] = {
    {"0", 0},
    {"1", 1},
    {"2", 2},
    {NULL,}
};

static const PropertyItem s_preset_audio_items[] = {
    {
        "����(bps)",
        offsetof(lc_preset_t, audio) + offsetof(lc_audio_output_t, bitRateInBps),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "������(Hz)",
        offsetof(lc_preset_t, audio) + offsetof(lc_audio_output_t, sampleRateInHz),
        sizeof(int),
        read_int,
        write_int,
        NULL,
        s_valid_audio_sample_rate,
        NULL
    },
    {
        "����",
        offsetof(lc_preset_t, audio) + offsetof(lc_audio_output_t, channels),
        sizeof(int),
        read_int,
        write_int,
        NULL,
        s_valid_audio_channels,
        NULL
    },
    { 0,}
};

static const PropertyItem s_preset_thumbnail_target_items[] = {
    {
        "ͼƬ��ʽ",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, format),
        sizeof(LC_IMAGE_TYPE) ,
        read_int,
        write_int,
        NULL,
        s_image_type,
        NULL
    },
    {
        "���ŷ�ʽ",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, sizingPolicy),
        sizeof(LC_VIDEO_SIZING) ,
        read_int,
        write_int,
        NULL,
        s_video_sizingpolicy,
        NULL
    },
    {
        "��",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, widthInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "��",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, heightInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },

    { 0,}
};

static const PropertyNameValue s_thumbnail_mode[] = {
    {"�ֶ�", MANUAL},
    {"�Զ�", AUTO },
    NULL,
};

static const PropertyItem s_preset_thumbnail_capture_items[] = {
    {
        "ģʽ",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, mode),
        sizeof(LC_THUMBNAIL_MODE) ,
        read_int,
        write_int,
        NULL,
        s_thumbnail_mode,
        NULL
    },
    {
        "��ʼʱ��(��)",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, startTimeInSecond),
        sizeof(long) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "����ʱ��(��)",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, endTimeInSecond),
        sizeof(long) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "ʱ����(��)",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, intervalInSecond),
        sizeof(long) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    { 0,}
};

static const PropertyItem s_preset_thumbnail_items[] = {
    {"��ʽ", 0, 0, NULL, NULL, NULL, NULL, s_preset_thumbnail_target_items},
    {"����", 0, 0, NULL, NULL, NULL, NULL, s_preset_thumbnail_capture_items},
    { 0,}
};

// preset property map
static const PropertyItem s_preset_items[] = {
    {
        "ģ������",
        offsetof(lc_preset_t, presetName),
        LC_MAX_PRESET_NAME,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "ģ������",
        offsetof(lc_preset_t, description),
        LC_MAX_PRESET_DESC,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "ForwardOnly",
        offsetof(lc_preset_t, forwardOnly),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL
    },
    {"��Ƶ", 0, 0, NULL, NULL, NULL, NULL, s_preset_video_items},
    {"��Ƶ", 0, 0, NULL, NULL, NULL, NULL, s_preset_audio_items},
    {"HLS", 0, 0, NULL, NULL, NULL, NULL, s_preset_hls_items},
    {"RTMP", 0, 0, NULL, NULL, NULL, NULL, s_preset_rtmp_items},
    {"¼��", 0, 0, NULL, NULL, NULL, NULL, s_preset_recording_items},
    {
        "����ʱ��",
        offsetof(lc_preset_t, createTime),
        sizeof(long long),
        read_ll_time,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {"����ͼ", 0, 0, NULL, NULL, NULL, NULL, s_preset_thumbnail_items},
    { 0,}
};

IMPLEMENT_DYNAMIC(CPresetDlg, CDialog)

CPresetDlg::CPresetDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CPresetDlg::IDD, pParent)
    , m_ppgPreset(0)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent))
    , m_eMode(MODE_VIEW)
    , m_pPresetList(0) {

}

CPresetDlg::~CPresetDlg() {
    if (m_ppgPreset) {
        delete m_ppgPreset;
    }

    if (m_pPresetList) {
        //����API���ͷ�ģ���б�
        lc_preset_list_free(m_pPresetList);
    }
}

void CPresetDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_PRESETS, m_trePresets);
    DDX_Control(pDX, IDC_BUTTON_MODE, m_btnMode);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
}

BOOL CPresetDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    UpdatePresetList();

    CreatePropertyGrid();
    return TRUE;
}

void CPresetDlg::CreatePropertyGrid() {

    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);
    m_trePresets.GetWindowRect(rcTree);

    ScreenToClient(rcTree);
    rcClient.left = rcTree.right;

    rcClient.DeflateRect(3, 3, 3, 52);

    m_ppgPreset = new CMFCPropertyGridCtrl();

    m_ppgPreset->Create(WS_VISIBLE | WS_CHILD, rcClient, this, 0);
    m_ppgPreset->EnableHeaderCtrl(FALSE);
    m_ppgPreset->EnableDescriptionArea(FALSE);

    const PropertyItem* pi = &s_preset_items[0];

    while (pi->Name) {
        CMFCPropertyGridProperty* prop = CreatePropertyItem(pi);
        m_ppgPreset->AddProperty(prop);
        pi ++;
    }
}

CMFCPropertyGridProperty* CPresetDlg::CreatePropertyItem(const PropertyItem* pi) {
    CMFCPropertyGridProperty* ret = NULL;

    if (pi->Read || pi->Write) {
        ret = new CMFCPropertyGridProperty(pi->Name, COleVariant(), NULL, (DWORD_PTR)pi);
    } else {
        ret = new CMFCPropertyGridProperty(pi->Name, (DWORD_PTR)pi);
    }

    if (pi->SubItems) {
        const PropertyItem* subPi = pi->SubItems;

        while (subPi->Name) {
            CMFCPropertyGridProperty* subItem = CreatePropertyItem(subPi);
            ret->AddSubItem(subItem);
            subPi ++;
        }
    }

    if (pi->Options) {
        const PropertyNameValue* cur = pi->Options;

        while (cur->Name) {
            ret->AddOption(cur->Name);
            cur ++;
        }
    }

    ret->AllowEdit(FALSE);

    return ret;
}

BEGIN_MESSAGE_MAP(CPresetDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PRESET, &CPresetDlg::OnTcnSelchangeTabPreset)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRESETS, &CPresetDlg::OnTvnSelchangedTreePresets)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPresetDlg::OnBnClickedButtonClose)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CPresetDlg::OnBnClickedButtonCancel)
    ON_BN_CLICKED(IDC_BUTTON_MODE, &CPresetDlg::OnBnClickedButtonMode)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPresetDlg::OnBnClickedButtonRefresh)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_PRESETS, &CPresetDlg::OnNMRClickTreePresets)
    ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_PRESETS, &CPresetDlg::OnTvnSelchangingTreePresets)
END_MESSAGE_MAP()


void CPresetDlg::OnTcnSelchangeTabPreset(NMHDR* pNMHDR, LRESULT* pResult) {
    *pResult = 0;
}

void CPresetDlg::OnClose() {
    DestroyWindow();
}
void CPresetDlg::OnNcDestroy() {
    CDialog::OnNcDestroy();

    if (m_pParent) {
        m_pParent->OnPresetDlgClosed(this);
    }
}
void CPresetDlg::OnOK() {}
void CPresetDlg::OnCancel() {
    OnClose();
}

void CPresetDlg::OnTvnSelchangedTreePresets(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    lc_preset_t* preset = NULL;

    if (m_eMode == MODE_VIEW) {

        if (pNMTreeView && pNMTreeView->itemNew.hItem) {
            preset = (lc_preset_t*) m_trePresets.GetItemData(pNMTreeView->itemNew.hItem);
        }

        UpdatePropertyList(preset, FALSE);
    }

    *pResult = 0;
}

// �����ݴ�ģ���и��µ�����ؼ���
void CPresetDlg::UpdatePropertyList(lc_preset_t* preset, BOOL allowEdit) {
    int count = m_ppgPreset->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty* pgp = m_ppgPreset->GetProperty(i);

        if (pgp) {
            UpdatePropertyItem(pgp, preset, allowEdit);
        }
    }
}

void CPresetDlg::UpdatePropertyItem(CMFCPropertyGridProperty* pgp, lc_preset_t* preset,
                                    BOOL allowEdit) {
    int count = pgp->GetSubItemsCount();

    for (int i = 0; i < count; i++) {
        CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

        if (subPgp) {
            UpdatePropertyItem(subPgp, preset, allowEdit);
        }
    }

    const PropertyItem* data = (PropertyItem*) pgp->GetData();
    pgp->AllowEdit(allowEdit);

    if (preset && data && data->Read) {
        data->Read(preset, data, pgp);
    } else {
        COleVariant var = pgp->GetValue();
        COleVariant newVar;
        newVar.ChangeType(var.vt);
        pgp->SetValue(newVar);
    }
}

void CPresetDlg::OnBnClickedButtonClose() {
    DestroyWindow();
}

void CPresetDlg::OnBnClickedButtonCancel() {
    if (IDYES == MessageBox("ȷ��Ҫ�������ڱ༭��ֱ��ģ����", "��ʾ",
                            MB_YESNO | MB_ICONINFORMATION)) {
        HTREEITEM item = m_trePresets.GetSelectedItem();
        lc_preset_t* preset = NULL;

        if (item) {
            preset = (lc_preset_t*)m_trePresets.GetItemData(item);
        }

        UpdatePropertyList(preset, FALSE);

        SetMode(MODE_VIEW);


    }
}

void CPresetDlg::SetMode(PRESET_MODE mode) {
    m_eMode = mode;

    if (m_eMode == MODE_VIEW) {
        m_btnMode.SetWindowText("�����ģ��");
        m_btnCancel.ShowWindow(SW_HIDE);
    } else {

        m_btnMode.SetWindowText("����");
        m_btnCancel.ShowWindow(SW_SHOW);
    }
}

// �༭��ֻ��ģʽ֮���л�
void CPresetDlg::OnBnClickedButtonMode() {
    if (m_eMode == MODE_VIEW) {
        lc_preset_t* preset = NULL;

        if (ChoosePreset(m_pPresetList, &preset)) {
            lc_preset_t newPreset = {0};

            if (preset) {
                newPreset = *preset;
            }

            newPreset.presetName[0] = '\0';
            UpdatePropertyList(&newPreset, TRUE);

            SetMode(MODE_ADD);
        }
    } else {
        lc_preset_t preset = {0};
        BuildPresetFromPropertyList(&preset);
        LC_CODE code = LC_OK;

        //����API������ģ��
        if (LC_OK == (code = lc_preset_create(&preset))) {
            SetMode(MODE_VIEW);
            UpdatePresetList();
        } else {
            CString strmsg;
            strmsg.Format("����ʧ��\r\n%s", lc_get_last_error());
            MessageBox(strmsg, "ִ�д���");
        }
    }
}

void CPresetDlg::OnBnClickedButtonRefresh() {
    UpdatePresetList();
}

// �ӷ���˸���ģ���б�ˢ���б�ؼ�
void CPresetDlg::UpdatePresetList() {
    m_trePresets.DeleteAllItems();

    if (m_pPresetList) {
        //����API���ͷ�ģ���б�
        lc_preset_list_free(m_pPresetList);
        m_pPresetList = NULL;
    }

    //����API���ӷ���˶�ȡģ���б�
    if (LC_OK == lc_preset_list(&m_pPresetList)) {
        for (int i = 0 ;
                i < m_pPresetList->count;
                i ++) {
            lc_preset_t* preset = &m_pPresetList->preset_list[i];
            HTREEITEM item = m_trePresets.InsertItem(preset->presetName);
            m_trePresets.SetItemData(item, (DWORD_PTR)preset);
        }
    } else {
        CString strmsg;
        strmsg.Format("����ʧ��\r\n%s", lc_get_last_error());
        MessageBox(strmsg, "ִ�д���");
    }
}
// ����Ҽ����ģ�壬����ɾ���˵�
void CPresetDlg::OnNMRClickTreePresets(NMHDR* pNMHDR, LRESULT* pResult) {
    CPoint ptCursor;
    CPoint ptClient;
    GetCursorPos(&ptCursor);
    ptClient = ptCursor;
    ::ScreenToClient(pNMHDR->hwndFrom, &ptClient);

    HTREEITEM item = m_trePresets.HitTest(ptClient);

    if (item) {
        m_trePresets.SelectItem(item);
        HMENU hPop = CreatePopupMenu();
        InsertMenu(hPop, 0, MF_BYPOSITION, ID_DELETE, "ɾ��");
        int cmd = TrackPopupMenu(hPop,
                                 TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                 ptCursor.x, ptCursor.y, 0, GetSafeHwnd(), NULL);
        DestroyMenu(hPop);

        if (cmd == ID_DELETE) {

            lc_preset_t* preset = (lc_preset_t*)m_trePresets.GetItemData(item);

            if (preset) {
                CString msg;
                msg.Format("ȷ��Ҫɾ��'%s'��?", preset->presetName);

                if (IDYES == MessageBox(msg, "ȷ��", MB_YESNO)) {
                    //����API���ӷ����ɾ��ģ��
                    if (LC_OK == lc_preset_delete(preset->presetName)) {
                        m_trePresets.DeleteItem(item);

                        // ��Ϊģ���б�Ϊ�������ͣ��ڴ����������ɾ���м�һ��ģ�壬
                        // ����Ҫ��֮���ģ��������ǰ�ƶ�һ��λ�ã�ͬʱ�����б�ؼ���ItemData


                        int one = sizeof(lc_preset_t);
                        // ģ���б��ڴ���ʼλ��
                        BYTE* s = (BYTE*)&m_pPresetList->preset_list[0];
                        // ģ���б��ڴ����λ��
                        BYTE* e = (BYTE*)&m_pPresetList->preset_list[m_pPresetList->count];
                        BYTE* d = (BYTE*)preset;

                        if (d + one < e) {
                            memmove(d, d + one, e - d - one);
                            HTREEITEM item = m_trePresets.GetRootItem();

                            std::stack<HTREEITEM> items;

                            if (item) {
                                items.push(item);
                            }

                            while (!items.empty()) {
                                item = items.top();
                                items.pop();

                                BYTE* ptr = (BYTE*)m_trePresets.GetItemData(item);

                                // ���λ��ɾ��ģ��֮������ǰ�ƶ�һ��λ�á�
                                if (ptr > d && ptr < e) {
                                    m_trePresets.SetItemData(item, (DWORD_PTR)(ptr - one));
                                }

                                HTREEITEM nextitem = m_trePresets.GetNextItem(item, TVGN_NEXT);

                                if (nextitem) {
                                    items.push(nextitem);
                                }

                                if (m_trePresets.ItemHasChildren(item)) {
                                    item = m_trePresets.GetChildItem(item);

                                    while (item != NULL) {
                                        items.push(item);
                                        item = m_trePresets.GetNextItem(item, TVGN_NEXT);
                                    }
                                }
                            }
                        }

                        // ģ��������һ
                        m_pPresetList->count --;

                    } else {
                        CString strmsg;
                        strmsg.Format("����ʧ��\r\n%s", lc_get_last_error());
                        MessageBox(strmsg, "ִ�д���");
                    }
                }
            }
        }
    }

    *pResult = 0;
}

void CPresetDlg::OnTvnSelchangingTreePresets(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;
}

BOOL CPresetDlg::ChoosePreset(lc_preset_list_t* list, lc_preset_t** preset) {
    CChoosePresetDlg dlg(list, preset, this);

    if (dlg.DoModal() == IDOK) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// �����ݴӽ���ؼ��и��µ�ģ����
void CPresetDlg::BuildPresetFromPropertyList(lc_preset_t* preset) {
    int count = m_ppgPreset->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty*  pgp = m_ppgPreset->GetProperty(i);

        if (pgp) {
            SavePropertyValue(pgp, preset);
        }
    }
}

void CPresetDlg::SavePropertyValue(CMFCPropertyGridProperty* pgp, lc_preset_t* preset) {
    const PropertyItem* item = (const PropertyItem*) pgp->GetData();

    int count = pgp->GetSubItemsCount();

    for (int i = 0; i < count ; i ++) {
        CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

        if (subPgp) {
            SavePropertyValue(subPgp, preset);
        }
    }

    if (item && item->Write) {
        item->Write(preset, item, pgp);
    }
}
