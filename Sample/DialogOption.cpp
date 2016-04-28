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
    USES_CONVERSION;

    if (!UpdateData(TRUE)) {
        return;
    }

    AES aes;
    CString ak;
    CString sk;

    if (m_strAK.Trim().IsEmpty()) {
        MessageBox(_T("请输入'AK'。"));
        return;
    }

    if (m_strSK.Trim().IsEmpty()) {
        MessageBox(_T("请输入'SK'。"));
        return;
    }

    if (m_strHost.Trim().IsEmpty())  {
        MessageBox(_T("请输入'服务器地址'。"));
        return;
    }

    lc_bce_access_key_t key = { 0 };
    strncpy_s(key.access_key_id, T2A(m_strAK), -1);
    strncpy_s(key.secret_access_key, T2A(m_strSK), -1);

    if (m_pParent->DoAuth(&key, m_strHost, FALSE)) {

        aes.Encrypt(m_strAK, ak);
        aes.Encrypt(m_strSK, sk);

        ConfigMgr::Instance().SetAk(ak);
        ConfigMgr::Instance().SetSk(sk);
        ConfigMgr::Instance().SetHost(m_strHost);
        m_pParent->UpdateLiveCaptureData();

        EndDialog(IDOK);
        DestroyWindow();
    }
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

    m_strAK = m_strAKbak = ak;
    m_strSK = m_strSKbak = sk;
    m_strHost = m_strHostbak = host;

    CDialog::OnInitDialog();
    return TRUE;
}

BEGIN_MESSAGE_MAP(CDialogOption, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_OK, &CDialogOption::OnBnClickedButtonOk)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDialogOption::OnBnClickedButtonClose)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CDialogOption::OnNMClickSyslink1)
    ON_NOTIFY(NM_CLICK, IDC_SYSLINK2, &CDialogOption::OnNMClickSyslink2)
END_MESSAGE_MAP()

void CDialogOption::OnClose() {
    USES_CONVERSION;
    lc_bce_access_key_t key = { 0 };
    strncpy_s(key.access_key_id, T2A(m_strAKbak), -1);
    strncpy_s(key.secret_access_key, T2A(m_strSKbak), -1);

    m_pParent->DoAuth(&key, m_strHostbak, FALSE, FALSE);
    DestroyWindow();
}
void CDialogOption::OnBnClickedButtonOk() {
    OnOK();
}

void CDialogOption::OnBnClickedButtonClose() {
    OnClose();
}

void CDialogOption::OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult) {
    PNMLINK pNMLink = (PNMLINK) pNMHDR;
    ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
    *pResult = 0;
}

void CDialogOption::OnNMClickSyslink2(NMHDR* pNMHDR, LRESULT* pResult) {
    PNMLINK pNMLink = (PNMLINK) pNMHDR;
    ShellExecuteW(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOWNORMAL);
    *pResult = 0;
}
