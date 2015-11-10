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
        "编码方式",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, codec),
        sizeof(LC_VIDEO_CODEC) ,
        read_int,
        write_int,
        NULL,
        s_video_codecs,
        NULL
    },
    {"编码参数", 0, 0, NULL, NULL, NULL, NULL, s_preset_video_codec_items},

    {
        "码率(bps)",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, bitRateInBps),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "帧率",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, maxFrameRate_x100),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        s_video_framerates,
        NULL
    },
    {
        "画面宽(pixels)",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, maxWidthInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "画面高(pixels)",
        offsetof(lc_preset_t, video) + offsetof(lc_video_output_t, maxHeightInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "缩放方式",
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
        "切片时长(秒)",
        offsetof(lc_preset_t, hls) + offsetof(lc_hls_config_t, segmentTimeInSecond),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "M3U8切片个数",
        offsetof(lc_preset_t, hls) + offsetof(lc_hls_config_t, segmentListSize),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "自适应码率",
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
        "缓存GOP",
        offsetof(lc_preset_t, rtmp) + offsetof(lc_rtmp_config_t, gopCache),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        s_bool,
        NULL
    },
    {
        "播放认证",
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
        "文件格式",
        offsetof(lc_preset_t, recording) + offsetof(lc_recording_t, format),
        sizeof(LC_RECORDING_FORMAT) ,
        read_int,
        write_int,
        NULL,
        s_valid_recording_formats,
        NULL
    },
    {
        "分段时长(分钟)",
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
        "码率(bps)",
        offsetof(lc_preset_t, audio) + offsetof(lc_audio_output_t, bitRateInBps),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "采样率(Hz)",
        offsetof(lc_preset_t, audio) + offsetof(lc_audio_output_t, sampleRateInHz),
        sizeof(int),
        read_int,
        write_int,
        NULL,
        s_valid_audio_sample_rate,
        NULL
    },
    {
        "声道",
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
        "图片格式",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, format),
        sizeof(LC_IMAGE_TYPE) ,
        read_int,
        write_int,
        NULL,
        s_image_type,
        NULL
    },
    {
        "缩放方式",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, sizingPolicy),
        sizeof(LC_VIDEO_SIZING) ,
        read_int,
        write_int,
        NULL,
        s_video_sizingpolicy,
        NULL
    },
    {
        "宽",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, target) + offsetof(lc_thumbnail_target_t, widthInPixel),
        sizeof(int) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "高",
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
    {"手动", MANUAL},
    {"自动", AUTO },
    NULL,
};

static const PropertyItem s_preset_thumbnail_capture_items[] = {
    {
        "模式",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, mode),
        sizeof(LC_THUMBNAIL_MODE) ,
        read_int,
        write_int,
        NULL,
        s_thumbnail_mode,
        NULL
    },
    {
        "开始时间(秒)",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, startTimeInSecond),
        sizeof(long) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "结束时间(秒)",
        offsetof(lc_preset_t, thumbnail) + offsetof(lc_thumbnail_t, capture) + offsetof(lc_thumbnail_capture_t, endTimeInSecond),
        sizeof(long) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {
        "时间间隔(秒)",
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
    {"格式", 0, 0, NULL, NULL, NULL, NULL, s_preset_thumbnail_target_items},
    {"策略", 0, 0, NULL, NULL, NULL, NULL, s_preset_thumbnail_capture_items},
    { 0,}
};

// preset property map
static const PropertyItem s_preset_items[] = {
    {
        "模板名称",
        offsetof(lc_preset_t, presetName),
        LC_MAX_PRESET_NAME,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "模板描述",
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
    {"视频", 0, 0, NULL, NULL, NULL, NULL, s_preset_video_items},
    {"音频", 0, 0, NULL, NULL, NULL, NULL, s_preset_audio_items},
    {"HLS", 0, 0, NULL, NULL, NULL, NULL, s_preset_hls_items},
    {"RTMP", 0, 0, NULL, NULL, NULL, NULL, s_preset_rtmp_items},
    {"录制", 0, 0, NULL, NULL, NULL, NULL, s_preset_recording_items},
    {
        "创建时间",
        offsetof(lc_preset_t, createTime),
        sizeof(long long),
        read_ll_time,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {"缩略图", 0, 0, NULL, NULL, NULL, NULL, s_preset_thumbnail_items},
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
        //调用API，释放模板列表
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

// 将数据从模板中更新到界面控件中
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
    if (IDYES == MessageBox("确定要放弃正在编辑的直播模板吗？", "提示",
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
        m_btnMode.SetWindowText("添加新模板");
        m_btnCancel.ShowWindow(SW_HIDE);
    } else {

        m_btnMode.SetWindowText("保存");
        m_btnCancel.ShowWindow(SW_SHOW);
    }
}

// 编辑与只读模式之间切换
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

        //调用API，创建模板
        if (LC_OK == (code = lc_preset_create(&preset))) {
            SetMode(MODE_VIEW);
            UpdatePresetList();
        } else {
            CString strmsg;
            strmsg.Format("操作失败\r\n%s", lc_get_last_error());
            MessageBox(strmsg, "执行错误");
        }
    }
}

void CPresetDlg::OnBnClickedButtonRefresh() {
    UpdatePresetList();
}

// 从服务端更新模板列表，刷新列表控件
void CPresetDlg::UpdatePresetList() {
    m_trePresets.DeleteAllItems();

    if (m_pPresetList) {
        //调用API，释放模板列表
        lc_preset_list_free(m_pPresetList);
        m_pPresetList = NULL;
    }

    //调用API，从服务端读取模板列表
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
        strmsg.Format("操作失败\r\n%s", lc_get_last_error());
        MessageBox(strmsg, "执行错误");
    }
}
// 如果右键点击模板，弹出删除菜单
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
        InsertMenu(hPop, 0, MF_BYPOSITION, ID_DELETE, "删除");
        int cmd = TrackPopupMenu(hPop,
                                 TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                 ptCursor.x, ptCursor.y, 0, GetSafeHwnd(), NULL);
        DestroyMenu(hPop);

        if (cmd == ID_DELETE) {

            lc_preset_t* preset = (lc_preset_t*)m_trePresets.GetItemData(item);

            if (preset) {
                CString msg;
                msg.Format("确定要删除'%s'吗?", preset->presetName);

                if (IDYES == MessageBox(msg, "确认", MB_YESNO)) {
                    //调用API，从服务端删除模板
                    if (LC_OK == lc_preset_delete(preset->presetName)) {
                        m_trePresets.DeleteItem(item);

                        // 因为模板列表为数组类型，内存连续，如果删除中间一条模板，
                        // 则需要将之后的模板数据向前移动一个位置，同时更新列表控件的ItemData


                        int one = sizeof(lc_preset_t);
                        // 模板列表内存起始位置
                        BYTE* s = (BYTE*)&m_pPresetList->preset_list[0];
                        // 模板列表内存结束位置
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

                                // 如果位于删除模板之后，则向前移动一个位置。
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

                        // 模板数量减一
                        m_pPresetList->count --;

                    } else {
                        CString strmsg;
                        strmsg.Format("操作失败\r\n%s", lc_get_last_error());
                        MessageBox(strmsg, "执行错误");
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

// 将数据从界面控件中更新到模板中
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
