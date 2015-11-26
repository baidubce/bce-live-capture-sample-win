// DialogCreateSession.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "DialogCreateSession.h"


// CDialogCreateSession dialog

IMPLEMENT_DYNAMIC(CDialogCreateSession, CDialog)

CDialogCreateSession::CDialogCreateSession(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogCreateSession::IDD, pParent)
    , m_strDescription(_T(""))
    , m_strPreset(_T(""))
    , m_strBucket(_T(""))
    , m_strDomain(_T(""))
    , m_bPushAuth(TRUE)
    , m_strNotification(_T("")) {

}

CDialogCreateSession::~CDialogCreateSession() {

}

BOOL CDialogCreateSession::OnInitDialog() {
    CDialog::OnInitDialog();

    lc_preset_list_t* presets = NULL;
    lc_notification_list_t* notifications = NULL;

    lc_preset_list(&presets);
    lc_notification_list(&notifications);

    if (presets) {
        for (int i = 0; i < presets->count; i ++) {
            m_cmbPresets.AddString(presets->preset_list[i].presetName);
        }

        lc_preset_list_free(presets);
    }

    if (notifications) {
        for (int j = 0; j < notifications->count; j ++) {
            m_cmbNotifications.AddString(notifications->list[j].name);
        }

        lc_notification_list_free(notifications);
    }

    return TRUE;
}

void CDialogCreateSession::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DESC, m_strDescription);
    DDX_CBString(pDX, IDC_COMBO_PRESET, m_strPreset);
    DDX_Text(pDX, IDC_EDIT_BUCKET, m_strBucket);
    DDX_Text(pDX, IDC_EDIT_DOMAIN, m_strDomain);
    DDX_Check(pDX, IDC_CHECK_PUSH_AUTH, m_bPushAuth);
    DDX_CBString(pDX, IDC_COMBO_NOTIFICATION, m_strNotification);
    DDX_Control(pDX, IDC_COMBO_NOTIFICATION, m_cmbNotifications);
    DDX_Control(pDX, IDC_COMBO_PRESET, m_cmbPresets);
}


BEGIN_MESSAGE_MAP(CDialogCreateSession, CDialog)
    ON_BN_CLICKED(IDOK, &CDialogCreateSession::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CDialogCreateSession::OnBnClickedCancel)
END_MESSAGE_MAP()


// CDialogCreateSession message handlers

void CDialogCreateSession::OnBnClickedOk() {
    if (!UpdateData(TRUE)) {
        return;
    }

    if (m_strPreset.Trim().IsEmpty()) {
        MessageBox("模板不能为空,请重新输入.");
        return;
    }

    lc_session_config_t config = {0};
    lc_session_config_init(&config);

    strncpy_s(config.presetName, m_strPreset, -1);

    if (!m_strDescription.Trim().IsEmpty()) {
        strncpy_s(config.description, m_strDescription, -1);
    } else {
        config.description[0] = 0;
    }

    if (!m_strBucket.Trim().IsEmpty()) {
        strncpy_s(config.target.bosBucket, m_strBucket, -1);
    } else {
        config.target.bosBucket[0] = 0;
    }

    if (!m_strDomain.Trim().IsEmpty()) {
        strncpy_s(config.target.userDomain, m_strDomain, -1);
    } else {
        config.target.userDomain[0] = 0;
    }

    config.publish.pushAuth = !!m_bPushAuth;

    if (!m_strNotification.Trim().IsEmpty()) {
        strncpy_s(config.notification, m_strNotification, -1);
    } else {
        config.notification[0] = 0;
    }

    lc_session_t session = {0};

    if (LC_OK == lc_session_create(&config, &session)) {
        CString msg;
        msg.Format("创建会话成功，新会话ID：%s", session.sessionId);
        lc_session_free(&session);
        MessageBox(msg, "成功", MB_OK);
    } else {
        CString err;
        err.Format("创建会话失败\r\n%s,", lc_get_last_error());
        MessageBox(err, "失败", MB_OK);
        return;
    }

    EndDialog(IDOK);
}

void CDialogCreateSession::OnBnClickedCancel() {
    OnCancel();
}
