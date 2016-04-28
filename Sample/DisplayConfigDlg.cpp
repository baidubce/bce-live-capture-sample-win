// DisplayConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "DisplayConfigDlg.h"


// CDisplayConfigDlg dialog

IMPLEMENT_DYNAMIC(CDisplayConfigDlg, CDialog)

CDisplayConfigDlg::CDisplayConfigDlg(lc_display_info_t* info, CWnd* pParent /*=NULL*/)
    : CDialog(CDisplayConfigDlg::IDD, pParent)
    , m_pInfo(info)
    , m_nXpos(0)
    , m_nWidth(0)
    , m_nHeight(0)
    , m_nYpos(0)
    , m_nHwnd(0) {

    switch (m_pInfo->type) {
    case DISPLAY_FULLSCREEN:
    default:
        break;

    case DISPLAY_WINDOW:
        m_nHwnd = m_pInfo->info.hwnd;
        break;

    case DISPLAY_RECTANGLE:
        m_nXpos = m_pInfo->info.rect.xpos;
        m_nYpos = m_pInfo->info.rect.ypos;
        m_nWidth = m_pInfo->info.rect.width;
        m_nHeight = m_pInfo->info.rect.height;
        break;
    }
}

CDisplayConfigDlg::~CDisplayConfigDlg() {
}

void CDisplayConfigDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_TYPE, m_cmbTypes);
    DDX_Control(pDX, IDC_STATIC_ICON, m_StaticIcon);
    DDX_Control(pDX, IDC_EDIT_X, m_edtXpos);
    DDX_Control(pDX, IDC_EDIT_Y, m_edtYpos);
    DDX_Control(pDX, IDC_EDIT_WIDTH, m_edtWidth);
    DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edtHeight);
    DDX_Control(pDX, IDC_EDIT_HWND, m_edtHwnd);

    if (m_edtXpos.IsWindowEnabled()) {
        DDX_Text(pDX, IDC_EDIT_X, m_nXpos);
        DDV_MinMaxInt(pDX, m_nXpos, 0, 3840);
    }

    if (m_edtWidth.IsWindowEnabled()) {
        DDX_Text(pDX, IDC_EDIT_WIDTH, m_nWidth);
        DDV_MinMaxInt(pDX, m_nWidth, 32, 3840);
    }

    if (m_edtHeight.IsWindowEnabled()) {
        DDX_Text(pDX, IDC_EDIT_HEIGHT, m_nHeight);
        DDV_MinMaxInt(pDX, m_nHeight, 32, 2160);
    }

    if (m_edtYpos.IsWindowEnabled()) {
        DDX_Text(pDX, IDC_EDIT_Y, m_nYpos);
        DDV_MinMaxInt(pDX, m_nYpos, 0, 2160);
    }

    DDX_Text(pDX, IDC_EDIT_HWND, m_nHwnd);
}

BOOL CDisplayConfigDlg::OnInitDialog() {
    BOOL bRet = CDialog::OnInitDialog();

    m_cmbTypes.AddString(_T("录制全屏"));
    m_cmbTypes.AddString(_T("录制窗口"));
    m_cmbTypes.AddString(_T("录制指定区域"));

    m_cmbTypes.SetCurSel(m_pInfo->type);

    OnCbnSelchangeComboType();
    return bRet;
}

BEGIN_MESSAGE_MAP(CDisplayConfigDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_OK, &CDisplayConfigDlg::OnBnClickedButtonOk)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDisplayConfigDlg::OnBnClickedButtonCancel)
    ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CDisplayConfigDlg::OnCbnSelchangeComboType)
    ON_COMMAND(IDC_STATIC_ICON, &CDisplayConfigDlg::OnUpdateHwnd)
END_MESSAGE_MAP()


// CDisplayConfigDlg message handlers

void CDisplayConfigDlg::OnUpdateHwnd() {
    HWND hwnd = m_StaticIcon.GetFindWindow();
    m_nHwnd = (UINT)hwnd;
    CString str;
    str.Format(_T("%d"), m_nHwnd);
    m_edtHwnd.SetWindowText(str);
}

void CDisplayConfigDlg::OnBnClickedButtonOk() {

    if (!UpdateData()) {
        return;
    }

    m_pInfo->type = (LC_DISPLAY_TYPE)m_cmbTypes.GetCurSel();

    switch (m_pInfo->type) {
    case DISPLAY_FULLSCREEN:
    default:
        break;
        memset(&m_pInfo->info, 0, sizeof(m_pInfo->info));

    case DISPLAY_WINDOW:
        m_pInfo->info.hwnd = m_nHwnd;
        if(!IsWindow((HWND)m_nHwnd)) {
            MessageBox(_T("窗口无效，重新选择"), _T("错误"), MB_OK);
            return;
        }
        break;

    case DISPLAY_RECTANGLE:
        m_pInfo->info.rect.xpos = m_nXpos;
        m_pInfo->info.rect.ypos = m_nYpos;
        m_pInfo->info.rect.width = m_nWidth;
        m_pInfo->info.rect.height = m_nHeight;
        break;
    }

    EndDialog(IDOK);
}

void CDisplayConfigDlg::OnBnClickedButtonCancel() {
    CDialog::OnCancel();
}

void CDisplayConfigDlg::OnCbnSelchangeComboType() {
    int n = m_cmbTypes.GetCurSel();

    if (n == DISPLAY_FULLSCREEN)    {
        m_StaticIcon.SetEnable(FALSE);
        m_edtHeight.EnableWindow(FALSE);
        m_edtWidth.EnableWindow(FALSE);
        m_edtXpos.EnableWindow(FALSE);
        m_edtYpos.EnableWindow(FALSE);

    } else if (n == DISPLAY_WINDOW) {
        m_StaticIcon.SetEnable(TRUE);
        m_edtHeight.EnableWindow(FALSE);
        m_edtWidth.EnableWindow(FALSE);
        m_edtXpos.EnableWindow(FALSE);
        m_edtYpos.EnableWindow(FALSE);

    } else if (n == DISPLAY_RECTANGLE) {
        m_StaticIcon.SetEnable(FALSE);
        m_edtHeight.EnableWindow(TRUE);
        m_edtWidth.EnableWindow(TRUE);
        m_edtXpos.EnableWindow(TRUE);
        m_edtYpos.EnableWindow(TRUE);
    }
}
