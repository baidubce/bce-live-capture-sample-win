// DialogCreateSession.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "DialogCreateSession.h"
#include "ConfigMgr.h"

#define USE_DEFAULT     _T("默认")
#define DO_NOT_USE      _T("不使用")

// CDialogCreateSession dialog

IMPLEMENT_DYNAMIC(CDialogCreateSession, CDialog)

CDialogCreateSession::CDialogCreateSession(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogCreateSession::IDD, pParent)
    , m_strDescription(_T(""))
    , m_strPreset(_T(""))
    , m_strNotification(_T("")) , m_strRecPreset(_T(""))
    , m_strSecurityPolicy(_T(""))
    , m_strRegion(_T(""))
    , m_strPullUrl(_T(""))
    , m_bPull(FALSE) {

}

CDialogCreateSession::~CDialogCreateSession() {

}

BOOL CDialogCreateSession::OnInitDialog() {

    USES_CONVERSION;

    CDialog::OnInitDialog();

    lc_list_t presets = NULL;
    lc_list_t notifications = NULL;
    lc_list_t recording_presets = NULL;
    lc_list_t security_policies = NULL;

    lc_transcode_preset_list(&presets);
    lc_notification_list(&notifications);
    lc_recording_preset_list(&recording_presets);
    lc_security_policy_list(&security_policies);


    m_cmbPresets.AddString(USE_DEFAULT);

    if (presets) {
        for (int i = 0; i < lc_list_count(presets); i ++) {

            lc_transcode_preset_t* preset = (lc_transcode_preset_t*)lc_list_get_at(presets, i);

            m_cmbPresets.AddString(A2T(preset->name));
        }

        lc_list_free(presets);
    }

    m_cmbPresets.SetCurSel(0);

    m_cmbNotifications.AddString(DO_NOT_USE);

    if (notifications) {
        for (int j = 0; j < lc_list_count(notifications); j ++) {

            lc_notification_t* notification = (lc_notification_t*)lc_list_get_at(notifications, j);
            m_cmbNotifications.AddString(A2T(notification->name));
        }

        lc_list_free(notifications);
    }

    m_cmbNotifications.SetCurSel(0);


    m_cmbRecPresets.AddString(DO_NOT_USE);

    if (recording_presets) {
        for (int i = 0; i < lc_list_count(recording_presets); i ++) {
            lc_recording_preset_t* rec_preset = (lc_recording_preset_t*)lc_list_get_at(recording_presets, i);
            m_cmbRecPresets.AddString(A2T(rec_preset->name));
        }

        lc_list_free(recording_presets);
    }

    m_cmbRecPresets.SetCurSel(0);


    m_cmbSecurityPolicies.AddString(USE_DEFAULT);

    if (security_policies) {

        for (int i = 0; i < lc_list_count(security_policies); i ++) {
            lc_security_policy_t* policy = (lc_security_policy_t*)lc_list_get_at(security_policies, i);

            m_cmbSecurityPolicies.AddString(A2T(policy->name));
            lc_security_policy_free(policy);
        }

        lc_list_free(security_policies);
    }

    m_cmbSecurityPolicies.SetCurSel(0);



    m_cmbRegions.AddString(USE_DEFAULT);
    CStringArray sa;

    ConfigMgr::Instance().GetRegions(sa);

    if (sa.GetSize() > 0) {
        for (int i = 0 ; i < sa.GetSize(); i ++) {
            m_cmbRegions.AddString(sa.GetAt(i));
        }
    }

    m_cmbRegions.SetCurSel(0);


    UpdatePullUrlEdit();
    return TRUE;
}

void CDialogCreateSession::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_NOTIFICATION, m_cmbNotifications);
    DDX_Control(pDX, IDC_COMBO_PRESET, m_cmbPresets);
    DDX_Control(pDX, IDC_EDIT_PULL_URL, m_edtPullUrl);
    DDX_Control(pDX, IDC_COMBO_REC_PRESET, m_cmbRecPresets);
    DDX_Control(pDX, IDC_COMBO_SECURITY_POLICY, m_cmbSecurityPolicies);
    DDX_Control(pDX, IDC_COMBO_REGION, m_cmbRegions);
    DDX_Control(pDX, IDC_CHECK_PULL, m_chkPull);
    DDX_Text(pDX, IDC_EDIT_DESC2, m_strDescription);
    DDX_CBString(pDX, IDC_COMBO_PRESET, m_strPreset);
    DDX_CBString(pDX, IDC_COMBO_NOTIFICATION, m_strNotification);
    DDX_CBString(pDX, IDC_COMBO_REC_PRESET, m_strRecPreset);
    DDX_CBString(pDX, IDC_COMBO_SECURITY_POLICY, m_strSecurityPolicy);
    DDX_CBString(pDX, IDC_COMBO_REGION, m_strRegion);
    DDX_Text(pDX, IDC_EDIT_PULL_URL, m_strPullUrl);
    DDX_Check(pDX, IDC_CHECK_PULL, m_bPull);
}


BEGIN_MESSAGE_MAP(CDialogCreateSession, CDialog)
    ON_BN_CLICKED(IDOK, &CDialogCreateSession::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDialogCreateSession::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_CHECK_PULL, &CDialogCreateSession::OnBnClickedCheckPull)
END_MESSAGE_MAP()


// CDialogCreateSession message handlers

void CDialogCreateSession::OnBnClickedOk() {
    USES_CONVERSION;

    if (!UpdateData(TRUE)) {
        return;
    }

    if (m_bPull) {
        if (m_strPullUrl.Trim().IsEmpty()) {
            MessageBox(_T("请输入拉流地址。"), _T("提示"), MB_OK);
            return;
        }
    }

    if (m_strPreset.Compare(DO_NOT_USE) == 0 || m_strPreset.Compare(USE_DEFAULT) == 0) {
        m_strPreset.Empty();
    }

    if (m_strSecurityPolicy.Compare(DO_NOT_USE) == 0 || m_strSecurityPolicy.Compare(USE_DEFAULT) == 0) {
        m_strSecurityPolicy.Empty();
    }

    if (m_strRecPreset.Compare(DO_NOT_USE) == 0 || m_strRecPreset.Compare(USE_DEFAULT) == 0) {
        m_strRecPreset.Empty();
    }

    if (m_strNotification.Compare(DO_NOT_USE) == 0 || m_strNotification.Compare(USE_DEFAULT) == 0) {
        m_strNotification.Empty();
    }

    if (m_strRegion.Compare(DO_NOT_USE) == 0 || m_strRegion.Compare(USE_DEFAULT) == 0) {
        m_strRegion.Empty();
    }

    lc_session_config_t config = {0};
    lc_session_config_init(&config);

    if (!m_strPreset.Trim().IsEmpty()) {
        strncpy_s(config.preset, T2A(m_strPreset), -1);
    } else {
        config.preset[0] = 0;
    }

    if (!m_strRecPreset.Trim().IsEmpty()) {
        strncpy_s(config.recording, T2A(m_strRecPreset), -1);
    } else {
        config.recording[0] = 0;
    }

    if (!m_strSecurityPolicy.Trim().IsEmpty()) {
        strncpy_s(config.security_policy, T2A(m_strSecurityPolicy), -1);
    } else {
        config.security_policy[0] = 0;
    }

    if (!m_strDescription.Trim().IsEmpty()) {
        strncpy_s(config.description, T2A(m_strDescription), -1);
    } else {
        config.description[0] = 0;
    }

    if (!m_strNotification.Trim().IsEmpty()) {
        strncpy_s(config.notification, T2A(m_strNotification), -1);
    } else {
        config.notification[0] = 0;
    }

    if (!m_strRegion.Trim().IsEmpty()) {
        strncpy_s(config.publish.region, T2A(m_strRegion), -1);
    } else {
        config.publish.region[0] = 0;
    }

    if (m_bPull) {
        config.publish.is_pull = true;
        strncpy_s(config.publish.url, T2A(m_strPullUrl), -1);
    }
    config.publish.is_valid = true;

    lc_session_t session = {0};

    if (LC_OK == lc_session_create(&config, &session)) {
        CString msg;
        msg.Format(_T("创建会话成功，新会话ID：%s"), A2T(session.session_id));
        MessageBox(msg, _T("成功"), MB_OK);
    } else {
        CString err;
        err.Format(_T("创建会话失败\r\n%s"), A2T(lc_get_last_error()));
        MessageBox(err, _T("失败"), MB_OK);
        return;
    }

    EndDialog(IDOK);
}

void CDialogCreateSession::OnBnClickedCancel() {
    OnCancel();
}

void CDialogCreateSession::OnBnClickedCheckPull() {
    UpdatePullUrlEdit();
}

void CDialogCreateSession::UpdatePullUrlEdit() {
    BOOL chk = (m_chkPull.GetCheck() == BST_CHECKED);

    m_edtPullUrl.EnableWindow(chk);
}
