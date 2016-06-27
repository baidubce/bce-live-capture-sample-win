#include "stdafx.h"
#include "Sample.h"
#include "SessionDlg.h"
#include "ToolsDlg.h"
#include "LogMgr.h"
#include "DialogCreateSession.h"
#include "session_items.h"

typedef enum {
    ID_NONE,
    ID_DELETE,
    ID_PAUSE,
    ID_RESUME,
};

IMPLEMENT_DYNAMIC(CSessionDlg, CPropertyGridDialog)

CSessionDlg::CSessionDlg(CWnd* pParent /*=NULL*/, BOOL select_mode /*=FALSE*/)
    : CPropertyGridDialog(CSessionDlg::IDD, pParent)
    , m_pSessionList(0)
    , m_pSelectedSession(0)
    , m_bSelectMode(select_mode)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent)) {
}

CSessionDlg::~CSessionDlg() {
    if (m_pSessionList) {
        //调用API，释放会话列表
        lc_list_free(m_pSessionList);
    }

    m_pSelectedSession = NULL;
}

void CSessionDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_SESSION, m_treeSession);
}


BEGIN_MESSAGE_MAP(CSessionDlg, CPropertyGridDialog)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CSessionDlg::OnBnClickedButtonRefresh)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CSessionDlg::OnBnClickedButtonClose)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SESSION, &CSessionDlg::OnTvnSelchangedTreeSession)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_SESSION, &CSessionDlg::OnNMRClickTreeSession)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_SESSION, &CSessionDlg::OnNMDblclkTreeSession)
    ON_BN_CLICKED(IDC_BUTTON_CREATE, &CSessionDlg::OnBnClickedButtonCreate)
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

	if (m_bSelectMode) {
		if (ValidSession(m_pSelectedSession)) {
			OnClose();
		}
	} else {
        OnClose();
    }
}

BOOL CSessionDlg::ValidSession(lc_session_t* session) {
	//if (session && session->config.security_policy[0]) {
	//	lc_security_policy_t policy = {0};
	//	lc_security_policy_query(&session->config.security_policy[0], &policy);
	//	if (policy.auth.play || policy.auth.push) {
	//		MessageBox(_T("暂时不支持播放认证和推流认证"));
	//		lc_security_policy_free(&policy);
	//		return FALSE;
	//	}
	//	lc_security_policy_free(&policy);
	//}
	return TRUE;
}

CString CSessionDlg::GetSelectedSessionId() {
    USES_CONVERSION;
    if (m_pSelectedSession) {
        return A2T(m_pSelectedSession->session_id);
    } else {
        return _T("");
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

        InsertMenu(hPop, 0, MF_BYPOSITION, ID_PAUSE, _T("停止(&S)"));
        InsertMenu(hPop, 1, MF_BYPOSITION, ID_RESUME, _T("恢复(&R)"));
        InsertMenu(hPop, 2, MF_BYPOSITION, ID_DELETE, _T("删除(&D)"));

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
    USES_CONVERSION;
    CString strmsg;

    if (session) {
        strmsg.Format(_T("确定要删除'%s(%s)'吗"), A2T(session->session_id), A2T(session->config.description));

        if (MessageBox(strmsg, _T("删除提示"), MB_YESNO | MB_ICONINFORMATION) == IDYES) {
            //调用API，删除会话
            if (LC_OK == lc_session_delete(session->session_id)) {
                m_treeSession.DeleteItem(item);
            } else {
                strmsg.Format(_T("删除失败\r\n%s"), A2T(lc_get_last_error()));
                MessageBox(strmsg, _T("执行错误"));
            }
        }
    }
}

void CSessionDlg::OnResumeSession(lc_session_t* session) {
    USES_CONVERSION;
    if (session) {
        //调用API，恢复会话状态
        if (LC_OK == lc_session_resume(session->session_id)) {
            UpdateSession(session);
        } else {
            CString strmsg;
            strmsg.Format(_T("操作失败\r\n%s"), A2T(lc_get_last_error()));
            MessageBox(strmsg, _T("执行错误"));
        }
    }
}

void CSessionDlg::UpdateSession(lc_session_t* session) {
    if (session) {
        lc_session_t newSession = {0};

        //调用API，从服务端查询会话
        if (LC_OK == lc_session_query(session->session_id, &newSession)) {
            *session = newSession;
            UpdatePropertyList(session, FALSE);
        }
    }
}

void CSessionDlg::OnPauseSession(lc_session_t* session) {
    USES_CONVERSION;

    CString strmsg;

    if (session) {
        strmsg.Format(_T("确定要停止'%s(%s)'吗"), A2T(session->session_id), A2T(session->config.description));

        if (MessageBox(strmsg, _T("停止提示"), MB_YESNO | MB_ICONINFORMATION) == IDYES) {
            //调用API，停止会话
            if (LC_OK == lc_session_pause(session->session_id)) {
                UpdateSession(session);
            } else {
                strmsg.Format(_T("停止失败\r\n%s"), A2T(lc_get_last_error()));
                MessageBox(strmsg, _T("执行错误"));
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

    case ID_PAUSE:
        OnPauseSession(session);
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
void CSessionDlg::GetPropertyGridRect(LPRECT rc) {
    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);
    m_treeSession.GetWindowRect(rcTree);

    ScreenToClient(rcTree);
    rcClient.left = rcTree.right;

    rcClient.DeflateRect(3, 3, 3, 52);

    *rc = rcClient;
}

const PropertyItem* CSessionDlg::GetPropertyList() {
    return &s_session_items[0];
}


void CSessionDlg::UpdateSessionList() {
    USES_CONVERSION;

    m_treeSession.SetRedraw(FALSE);
    m_treeSession.DeleteAllItems();

    if (m_pSessionList) {
        //调用API，释放会话列表
        lc_list_free(m_pSessionList);
        m_pSessionList = NULL;
    }

    //调用API，从服务端读取会话列表
    if (LC_OK == lc_session_list(&m_pSessionList, SESSION_UNKNOWN)) {
        for (int i = 0 ;
                i < lc_list_count(m_pSessionList);
                i ++) {
            lc_session_t* session = (lc_session_t*)lc_list_get_at(m_pSessionList, i);

            // 选择模式下，跳过拉流模式的会话，只能选择推流模式
            if (m_bSelectMode) {
                if (session->config.publish.is_pull) {
                    continue;
                }
            }

            if (session->status != SESSION_UNKNOWN) {
                CString strItemName;
                strItemName.Format(_T("%s, %s"), A2T(session->session_id), A2T(session->config.description));
                HTREEITEM item = m_treeSession.InsertItem(strItemName);
                m_treeSession.SetItemData(item, (DWORD_PTR)session);
            }
        }
    } else {
        CLogMgr::Instance().AppendLog(LC_LOG_ERROR, A2T(lc_get_last_error()));
    }

    m_treeSession.SetRedraw(TRUE);
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
			if (ValidSession(m_pSelectedSession)) {
				OnClose();
			}
        } else {
            m_pSelectedSession = NULL;
        }
    }

    *pResult = 0;
}

void CSessionDlg::OnBnClickedButtonCreate() {
    CDialogCreateSession dlg(this);

    if (dlg.DoModal() == IDOK) {
        UpdateSessionList();
    }
}
