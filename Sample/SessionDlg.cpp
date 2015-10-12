#include "stdafx.h"
#include "Sample.h"
#include "SessionDlg.h"
#include "ToolsDlg.h"
#include "LogMgr.h"

static const PropertyItem s_session_target_items[] = {
    {
        "Bos Bucket",
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, target) + offsetof(lc_target_t, bosBucket),
        LC_MAX_BOS_BUCKET_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "Bucket域名",
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, target) + offsetof(lc_target_t, userDomain),
        LC_MAX_USER_DOMAIN_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {0,}
};

static const PropertyItem s_session_playurl_items[] = {
    {
        "HLS",
        offsetof(lc_session_t, play) + offsetof(lc_session_play_t, hlsUrl),
        LC_MAX_URL_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "rtmp",
        offsetof(lc_session_t, play) + offsetof(lc_session_play_t, rtmpUrl),
        LC_MAX_URL_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "rtmpKey",
        offsetof(lc_session_t, play) + offsetof(lc_session_play_t, rtmpKey),
        LC_MAX_RTMP_KEY_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {0,}
};
static const PropertyItem s_session_publish_items[] = {
    {
        "URL",
        offsetof(lc_session_t, publish) + offsetof(lc_session_publish_t, pushUrl),
        LC_MAX_URL_LEN ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "Auth",
        offsetof(lc_session_t, publish) + offsetof(lc_session_publish_t, pushAuth),
        sizeof(bool) ,
        read_int,
        write_int,
        NULL,
        NULL,
        NULL
    },
    {0,}
};

static const PropertyItem s_session_error_items[] = {
    {
        "代码",
        offsetof(lc_session_t, error) + offsetof(lc_session_error_t, code),
        LC_MAX_ERR_CODE ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "信息",
        offsetof(lc_session_t, error) + offsetof(lc_session_error_t, message),
        LC_MAX_ERR_MSG ,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {0,}
};

static const PropertyItem s_session_record_items[] = {
    {
        "文件列表",
        offsetof(lc_session_t, record) + offsetof(lc_session_records_t, count),
        sizeof(int) + sizeof(char**),
        read_string_array,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {0,}
};

static const PropertyNameValue s_valid_session_status[] = {
    {"", SESSION_UNKNOWN},
    {"就绪", SESSION_READY},
    {"正在直播", SESSION_ONGOING},
    {"暂停", SESSION_PAUSED},
    {"已关闭", SESSION_CLOSED},
    {0,}
};

static const PropertyNameValue s_valid_session_streaming_status[] = {
    {"", STREAMING_UNKNOWN},
    {"正在推流", STREAMING_STREAMING},
    {"空闲", STREAMING_NO_SOURCE},
    {"失败", STREAMING_FAILED},
    {0,}
};

static const PropertyItem s_session_items[] = {
    {
        "用户ID",
        offsetof(lc_session_t, userId),
        LC_MAX_USER_ID_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "会话ID",
        offsetof(lc_session_t, sessionId),
        LC_MAX_SESSION_ID_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "会话描述",
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, description),
        LC_MAX_SESSION_DESC,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "直播模板",
        offsetof(lc_session_t, config) + offsetof(lc_session_config_t, presetName),
        LC_MAX_PRESET_NAME,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {"存储", 0, 0, NULL, NULL, NULL, NULL, s_session_target_items},
    {
        "创建时间",
        offsetof(lc_session_t, createTime),
        sizeof(long long),
        read_ll_time,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "最后更新",
        offsetof(lc_session_t, lastUpdateTime),
        sizeof(long long),
        read_ll_time,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {
        "关闭时间",
        offsetof(lc_session_t, closedTime),
        sizeof(long long),
        read_ll_time,
        NULL,
        NULL,
        NULL,
        NULL
    },
    {"推流地址", 0, 0, NULL, NULL, NULL, NULL, s_session_publish_items},

    {"播放地址", 0, 0, NULL, NULL, NULL, NULL, s_session_playurl_items},
    {
        "状态",
        offsetof(lc_session_t, status),
        sizeof(LC_SESSION_STATUS),
        read_int,
        write_int,
        NULL,
        s_valid_session_status,
        NULL
    },
    {
        "推流状态",
        offsetof(lc_session_t, streamingStatus),
        sizeof(LC_SESSION_STREAMING_STATUS),
        read_int,
        write_int,
        NULL,
        s_valid_session_streaming_status,
        NULL
    },
    {"错误信息", 0, 0, NULL, NULL, NULL, NULL, s_session_error_items},
    {"文件", 0, 0, NULL, NULL, NULL, NULL, s_session_record_items},

    { 0,}
};

typedef enum {
    ID_NONE,
    ID_DELETE,
    ID_STOP,
    ID_RESUME,
};

IMPLEMENT_DYNAMIC(CSessionDlg, CDialog)

CSessionDlg::CSessionDlg(CWnd* pParent /*=NULL*/, BOOL select_mode /*=FALSE*/)
    : CDialog(CSessionDlg::IDD, pParent)
    , m_pSessionList(0)
    , m_ppgSession(0)
    , m_pSelectedSession(0)
    , m_bSelectMode(select_mode)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent)) {
}

CSessionDlg::~CSessionDlg() {
    if (m_ppgSession) {
        delete m_ppgSession;
    }

    if (m_pSessionList) {
        //调用API，释放会话列表
        lc_session_list_free(m_pSessionList);
    }

    m_pSelectedSession = NULL;
}

void CSessionDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_SESSION, m_treeSession);
}


BEGIN_MESSAGE_MAP(CSessionDlg, CDialog)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CSessionDlg::OnBnClickedButtonRefresh)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CSessionDlg::OnBnClickedButtonClose)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SESSION, &CSessionDlg::OnTvnSelchangedTreeSession)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_SESSION, &CSessionDlg::OnNMRClickTreeSession)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_SESSION, &CSessionDlg::OnNMDblclkTreeSession)
END_MESSAGE_MAP()


void CSessionDlg::OnClose() {
    EndDialog(0);
    DestroyWindow();
}

void CSessionDlg::OnOK() {

}
void CSessionDlg::OnCancel() {
    OnClose();
}
void CSessionDlg::OnNcDestroy() {
    CDialog::OnNcDestroy();

    if (m_pParent) {
        m_pParent->OnSessionDlgClosed(this);
    }
}

void CSessionDlg::OnBnClickedButtonRefresh() {
    UpdateSessionList();
}

void CSessionDlg::OnBnClickedButtonClose() {

    HTREEITEM item = m_treeSession.GetSelectedItem();

    if (item) {
        m_pSelectedSession = (lc_session_t*)m_treeSession.GetItemData(item);
    }

    OnClose();
}

CString CSessionDlg::GetSelectedSessionId() {
    if (m_pSelectedSession) {
        return m_pSelectedSession->sessionId;
    } else {
        return "";
    }
}

void CSessionDlg::UpdatePropertyList(lc_session_t* session, BOOL allowEdit) {
    int count = m_ppgSession->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty* pgp = m_ppgSession->GetProperty(i);

        if (pgp) {
            UpdatePropertyItem(pgp, session, allowEdit);
        }
    }
}

void CSessionDlg::UpdatePropertyItem(CMFCPropertyGridProperty* pgp, lc_session_t* session,
                                     BOOL allowEdit) {
    int count = pgp->GetSubItemsCount();

    for (int i = 0; i < count; i++) {
        CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

        if (subPgp) {
            UpdatePropertyItem(subPgp, session, allowEdit);
        }
    }

    const PropertyItem* data = (PropertyItem*) pgp->GetData();
    pgp->AllowEdit(allowEdit);

    if (session && data && data->Read) {
        data->Read(session, data, pgp);
    } else {
        COleVariant var = pgp->GetValue();
        COleVariant newVar;
        newVar.ChangeType(var.vt);
        pgp->SetValue(newVar);
    }
}

void CSessionDlg::OnTvnSelchangedTreeSession(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    lc_session_t* session = NULL;

    if (pNMTreeView && pNMTreeView->itemNew.hItem) {
        session = (lc_session_t*) m_treeSession.GetItemData(pNMTreeView->itemNew.hItem);
    }

    UpdatePropertyList(session, FALSE);

    *pResult = 0;
}

void CSessionDlg::OnNMRClickTreeSession(NMHDR* pNMHDR, LRESULT* pResult) {
    CPoint ptCursor;
    CPoint ptClient;
    GetCursorPos(&ptCursor);
    ptClient = ptCursor;
    ::ScreenToClient(pNMHDR->hwndFrom, &ptClient);

    HTREEITEM item = m_treeSession.HitTest(ptClient);

    if (item) {
        m_treeSession.SelectItem(item);
        HMENU hPop = CreatePopupMenu();

        InsertMenu(hPop, 0, MF_BYPOSITION, ID_STOP, "停止(&S)");
        InsertMenu(hPop, 1, MF_BYPOSITION, ID_RESUME, "恢复(&R)");
        InsertMenu(hPop, 2, MF_BYPOSITION, ID_DELETE, "删除(&D)");

        int cmd = TrackPopupMenu(hPop,
                                 TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                 ptCursor.x, ptCursor.y, 0, GetSafeHwnd(), NULL);
        DestroyMenu(hPop);

        if (cmd) {

            lc_session_t* session = (lc_session_t*)m_treeSession.GetItemData(item);

            if (session) {
                OnMenuCmd(cmd, session, item);
            }
        }
    }

    *pResult = 0;
}

void CSessionDlg::OnDeleteSession(lc_session_t* session, HTREEITEM item) {
    CString strmsg;

    if (session) {
        strmsg.Format("确定要删除'%s(%s)'吗", session->sessionId, session->config.description);

        if (MessageBox(strmsg, "删除提示", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
            //调用API，删除会话
            if (LC_OK == lc_session_delete(session->sessionId)) {
                m_treeSession.DeleteItem(item);
            } else {
                strmsg.Format("删除失败\r\n%s", lc_get_last_error());
                MessageBox(strmsg, "执行错误");
            }
        }
    }
}

void CSessionDlg::OnResumeSession(lc_session_t* session) {
    if (session) {
        //调用API，恢复会话状态
        if (LC_OK == lc_session_resume(session->sessionId)) {
            UpdateSession(session);
        } else {
            CString strmsg;
            strmsg.Format("操作失败\r\n%s", lc_get_last_error());
            MessageBox(strmsg, "执行错误");
        }
    }
}

void CSessionDlg::UpdateSession(lc_session_t* session) {
    if (session) {
        lc_session_t newSession = {0};

        //调用API，从服务端查询会话
        if (LC_OK == lc_session_query(session->sessionId, &newSession)) {
            lc_session_free(session);
            *session = newSession;
            UpdatePropertyList(session, FALSE);
        }
    }
}

void CSessionDlg::OnStopSession(lc_session_t* session) {
    CString strmsg;

    if (session) {
        strmsg.Format("确定要停止'%s(%s)'吗", session->sessionId, session->config.description);

        if (MessageBox(strmsg, "停止提示", MB_YESNO | MB_ICONINFORMATION) == IDYES) {
            //调用API，停止会话
            if (LC_OK == lc_session_stop(session->sessionId)) {
                UpdateSession(session);
            } else {
                strmsg.Format("停止失败\r\n%s", lc_get_last_error());
                MessageBox(strmsg, "执行错误");
            }
        }
    }
}

void CSessionDlg::OnMenuCmd(int cmd, lc_session_t* session, HTREEITEM item) {
    switch (cmd) {
    case ID_DELETE:
        OnDeleteSession(session, item);
        break;

    case ID_RESUME:
        OnResumeSession(session);
        break;

    case ID_STOP:
        OnStopSession(session);
        break;

    default:
        break;
    }
}

BOOL CSessionDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    UpdateSessionList();

    CreatePropertyGrid();
    return TRUE;
}

void CSessionDlg::CreatePropertyGrid() {

    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);
    m_treeSession.GetWindowRect(rcTree);

    ScreenToClient(rcTree);
    rcClient.left = rcTree.right;

    rcClient.DeflateRect(3, 3, 3, 52);

    m_ppgSession = new CMFCPropertyGridCtrl();

    m_ppgSession->Create(WS_VISIBLE | WS_CHILD, rcClient, this, 0);
    m_ppgSession->EnableHeaderCtrl(FALSE);
    m_ppgSession->EnableDescriptionArea(FALSE);

    const PropertyItem* pi = &s_session_items[0];

    while (pi->Name) {
        CMFCPropertyGridProperty* prop = CreatePropertyItem(pi);
        m_ppgSession->AddProperty(prop);
        pi ++;
    }
}

CMFCPropertyGridProperty* CSessionDlg::CreatePropertyItem(const PropertyItem* pi) {
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

void CSessionDlg::UpdateSessionList() {
    m_treeSession.DeleteAllItems();

    if (m_pSessionList) {
        //调用API，释放会话列表
        lc_session_list_free(m_pSessionList);
        m_pSessionList = NULL;
    }

    //调用API，从服务端读取会话列表
    if (LC_OK == lc_session_list(&m_pSessionList)) {
        for (int i = 0 ;
                i < m_pSessionList->count;
                i ++) {
            lc_session_t* session = &m_pSessionList->session_list[i];

            if (session->status != SESSION_CLOSED &&
                    session->status != SESSION_UNKNOWN) {
                CString strItemName;
                strItemName.Format("%s, %s", session->sessionId, session->config.description);
                HTREEITEM item = m_treeSession.InsertItem(strItemName);
                m_treeSession.SetItemData(item, (DWORD_PTR)session);
            }
        }
    } else {
        CLogMgr::Instance().AppendLog(LC_LOG_ERROR, lc_get_last_error());
    }
}

void CSessionDlg::OnNMDblclkTreeSession(NMHDR* pNMHDR, LRESULT* pResult) {

    if (m_bSelectMode) {
        CPoint ptCursor;
        CPoint ptClient;
        GetCursorPos(&ptCursor);
        ptClient = ptCursor;
        ::ScreenToClient(pNMHDR->hwndFrom, &ptClient);

        HTREEITEM item = m_treeSession.HitTest(ptClient);

        if (item) {
            m_pSelectedSession = (lc_session_t*)m_treeSession.GetItemData(item);
            OnClose();
        } else {
            m_pSelectedSession = NULL;
        }
    }

    *pResult = 0;
}
