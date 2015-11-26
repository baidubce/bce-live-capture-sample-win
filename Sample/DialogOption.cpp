#include "stdafx.h"
#include "Sample.h"
#include "DialogOption.h"
#include "ConfigMgr.h"
#include "AES.h"
#include "ToolsDlg.h"

// CDialogOption dialog

IMPLEMENT_DYNCREATE(CDialogOption, CDialog)

CDialogOption::CDialogOption(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogOption::IDD, pParent) , m_strAK(_T(""))
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent))
    , m_strSK(_T(""))
    , m_strHost(_T("")) {

}

CDialogOption::~CDialogOption() {
}

void CDialogOption::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_AK, m_strAK);
    DDX_Text(pDX, IDC_EDIT_SK, m_strSK);
    DDX_Text(pDX, IDC_EDIT_HOST, m_strHost);
}

void CDialogOption::OnOK() {
    if (!UpdateData(TRUE)) {
        return;
    }

    AES aes;
    CString ak;
    CString sk;

    if (m_strAK.Trim().IsEmpty()) {
        MessageBox("请输入'AK'。");
        return;
    }

    if (m_strSK.Trim().IsEmpty()) {
        MessageBox("请输入'SK'。");
        return;
    }

    if (m_strHost.Trim().IsEmpty())  {
        MessageBox("请输入'服务器地址'。");
        return;
    }

    lc_bce_access_key_t key = { 0 };
    strncpy_s(key.access_key_id, (LPCSTR)m_strAK, -1);
    strncpy_s(key.secret_access_key, (LPCSTR)m_strSK, -1);

    // 初始化采集SDK
    if (LC_OK != lc_init(&key, m_strHost)) {
        CLogMgr::Instance().AppendLog(LC_LOG_ERROR, "lc_init failed.");
        MessageBox("初始化采集模块失败");
        return;
    }

    aes.Encrypt(m_strAK, ak);
    aes.Encrypt(m_strSK, sk);

    ConfigMgr::Instance().SetAk(ak);
    ConfigMgr::Instance().SetSk(sk);
    ConfigMgr::Instance().SetHost(m_strHost);

    EndDialog(IDOK);
    DestroyWindow();
}
void CDialogOption::OnCancel() {
    OnClose();
}

void CDialogOption::OnNcDestroy() {
    CDialog::OnNcDestroy();

    if (m_pParent) {
        m_pParent->OnOptionDlgClosed(this);
    }
}

BOOL CDialogOption::OnInitDialog() {
    CString ak;
    CString sk;
    CString host;
    ConfigMgr::Instance().GetAk(ak);
    ConfigMgr::Instance().GetSk(sk);
    ConfigMgr::Instance().GetHost(host);

    AES aes;

    aes.Decrypt(ak, ak);
    aes.Decrypt(sk, sk);

    m_strAK = ak;
    m_strSK = sk;
    m_strHost = host;

    CDialog::OnInitDialog();
    return TRUE;
}

BEGIN_MESSAGE_MAP(CDialogOption, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_OK, &CDialogOption::OnBnClickedButtonOk)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDialogOption::OnBnClickedButtonClose)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
END_MESSAGE_MAP()

void CDialogOption::OnClose() {
    DestroyWindow();
}
void CDialogOption::OnBnClickedButtonOk() {
    OnOK();
}

void CDialogOption::OnBnClickedButtonClose() {
    DestroyWindow();
}
