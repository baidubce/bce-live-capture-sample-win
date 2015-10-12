#include "stdafx.h"
#include "Sample.h"
#include "LogDlg.h"

#include "ToolsDlg.h"
#include "logMgr.h"


typedef struct _tagLogName {
    const char* Name;
    LC_LOGLEVEL Level;
} LogName;

static const LogName s_LogNames[] = {
    {"DEBUG", LC_LOG_DEBUG},
    {"INFO", LC_LOG_INFO},
    {"WARNING", LC_LOG_WARN},
    {"ERROR", LC_LOG_ERROR},
    {"FATAL", LC_LOG_FATAL},
    { NULL, }
};

const char* LogLevelName(LC_LOGLEVEL level) {
    const LogName* ln = &s_LogNames[0];

    while (ln->Name) {
        if (level == ln->Level) {
            return ln->Name;
        }

        ln ++;
    }

    return "None";
}

#define MIN_WIDTH 500
#define MIN_HEIGHT 360

IMPLEMENT_DYNAMIC(CLogDlg, CDialog)

CLogDlg::CLogDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CLogDlg::IDD, pParent)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent)) {
}

CLogDlg::~CLogDlg() {
}

void CLogDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT21, m_edtLog);
    DDX_Control(pDX, IDC_COMBO_LEVELS, m_cmbLevels);
    DDX_Control(pDX, IDC_STATIC_TOOLS, m_Static);
    DDX_Control(pDX, IDC_STATIC_LEVEL, m_stLevel);
    DDX_Control(pDX, IDC_BUTTON_SAVEAS, m_btSaveAs);
    DDX_Control(pDX, IDC_BUTTON_CLEAR, m_btClear);
    DDX_Control(pDX, IDC_BUTTON_SHUTDOWN, m_btClose);
}

BEGIN_MESSAGE_MAP(CLogDlg, CDialog)
    ON_WM_DESTROY()
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_SAVEAS, &CLogDlg::OnBnClickedButtonSaveas)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CLogDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDC_BUTTON_SHUTDOWN, &CLogDlg::OnBnClickedButtonShutdown)
    ON_CBN_SELCHANGE(IDC_COMBO_LEVELS, &CLogDlg::OnCbnSelchangeComboLevels)
    ON_MESSAGE(WM_LOG_ADD, OnAddMessage)
    ON_MESSAGE(WM_LOG_CLEAR, OnClearMessage)
    ON_MESSAGE(WM_LOG_SETFILTER, OnSetFilter)
    ON_WM_SIZE()
    ON_WM_SIZING()
END_MESSAGE_MAP()


void CLogDlg::OnDestroy() {
    CDialog::OnDestroy();
}

void CLogDlg::OnNcDestroy() {
    CDialog::OnNcDestroy();

    if (m_pParent) {
        m_pParent->OnLogDlgClosed(this);
    }
}

void CLogDlg::OnOK() {

}

void CLogDlg::OnCancel() {
    CDialog::OnCancel();
}


BOOL CLogDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    const LogName* ln =  &s_LogNames[0];

    while (ln->Name) {
        int Id = m_cmbLevels.AddString(ln->Name);

        if (Id >= 0) {
            m_cmbLevels.SetItemData(Id, (DWORD_PTR)ln->Level);
        }

        ln++;
    }

    m_cmbLevels.SelectString(-1, "WARNING");

    CLogMgr::Instance().SetNotifyHwnd(GetSafeHwnd());

    CString msg;
    CLogMgr::Instance().FilterLog(LC_LOG_WARN, msg);

    if (!msg.IsEmpty()) {
        m_edtLog.SetWindowText(msg);
    }

    return TRUE;
}

void CLogDlg::OnClose() {
    CLogMgr::Instance().SetNotifyHwnd(NULL);
    DestroyWindow();
}
void CLogDlg::OnBnClickedButtonSaveas() {
    CFileDialog cfd(FALSE, "txt", "live_tools_log", OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
                    "*.txt|*.txt|All|*||", this);

    if (IDOK == cfd.DoModal()) {
        CLogMgr::Instance().SaveAs(cfd.GetPathName());
    }
}

void CLogDlg::OnBnClickedButtonClear() {
    CLogMgr::Instance().ClearLog();
}

void CLogDlg::OnBnClickedButtonShutdown() {
    OnClose();
}

void CLogDlg::OnCbnSelchangeComboLevels() {
    int id = m_cmbLevels.GetCurSel();

    if (id >= 0) {
        LC_LOGLEVEL level = (LC_LOGLEVEL)m_cmbLevels.GetItemData(id);
        CLogMgr::Instance().SetFilterLevel(level);
        CString log;

        CLogMgr::Instance().FilterLog(level, log);
        m_edtLog.SetSel(0, -1);
        m_edtLog.ReplaceSel((LPCSTR)log);
    }
}

LRESULT CLogDlg::OnAddMessage(WPARAM wParam, LPARAM lParam) {
    LogItem* item = reinterpret_cast<LogItem*>(lParam);

    m_edtLog.SetSel(-1, -1);
    CString msg;
    msg.Format("%s\t%s%s", LogLevelName(item->level), item->msg.c_str(), "\r\n");
    m_edtLog.ReplaceSel((LPCSTR)msg);
    m_edtLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
    CLogMgr::Instance().ReleaseLog(item);

    int nLine = m_edtLog.GetLineCount();

    if (nLine > MAX_LOG_LINES) {
        int nIndex = m_edtLog.LineIndex(100);

        if (nIndex != -1) {
            m_edtLog.SetSel(0, nIndex);
            m_edtLog.ReplaceSel("");
        }
    }

    return 0;
}

LRESULT CLogDlg::OnClearMessage(WPARAM wParam, LPARAM lParam) {
    m_edtLog.SetWindowText("");
    return 0;
}

LRESULT CLogDlg::OnSetFilter(WPARAM wParam, LPARAM lParam) {
    LC_LOGLEVEL level = (LC_LOGLEVEL)(lParam);

    return 0;
}

void CLogDlg::OnSize(UINT nType, int cx, int cy) {
    CDialog::OnSize(nType, cx, cy);

    if (cx > 0 && cy > 0) {
        CRect rcWin;
        m_Static.GetWindowRect(rcWin);
        m_Static.MoveWindow(cx - rcWin.Width() - 3, cy - rcWin.Height() - 3, rcWin.Width(), rcWin.Height());

        CRect rc(0, 0, cx, cy - 40);
        rc.DeflateRect(3, 3);
        m_edtLog.MoveWindow(rc);

        CWnd* ctrls[] = {&m_stLevel, &m_cmbLevels, &m_btSaveAs,
                         &m_btClear, &m_btClose, 0
                        };
        int width [] = {0, 0, 0, 0, 0 };
        int height [] = {0, 0, 0, 0, 0 };

        CRect rcStatic;

        CWnd** cur = &ctrls[0];
        int width_i = 0;
        int total_width = 0;

        while (*cur) {
            CRect rcChild;
            (*cur)->GetWindowRect(rcChild);
            height[width_i] = rcChild.Height();
            width[width_i++] = rcChild.Width();
            total_width += rcChild.Width();
            cur ++ ;
        }

        int interval = (rcWin.Width() - total_width) / (width_i - 1);

        int left = cx - rcWin.Width();
        int top = cy - rcWin.Height();

        for (int i = 0 ; i < width_i; i ++) {
            ctrls[i]->SetWindowPos(NULL, left, top + (rcWin.Height() - height[i]) / 2,
                                   0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            left += width[i] + interval;
            ctrls[i]->Invalidate(FALSE);
        }
    }
}

void CLogDlg::OnSizing(UINT fwSide, LPRECT pRect) {
    CRect* rect = reinterpret_cast<CRect*>(pRect);

    if (rect->Width() < MIN_WIDTH) {
        rect->right = rect->left + MIN_WIDTH;
    }

    if (rect->Height() < MIN_HEIGHT) {
        rect->bottom = rect->top + MIN_HEIGHT;
    }

    CDialog::OnSizing(fwSide, pRect);
}
