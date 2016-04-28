#pragma once
#include "afxwin.h"


// CDialogCreateSession dialog

class CDialogCreateSession : public CDialog {
    DECLARE_DYNAMIC(CDialogCreateSession)

public:
    CDialogCreateSession(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDialogCreateSession();

    // Dialog Data
    enum { IDD = IDD_DIALOG_CREATE_SESSION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedCheckPull();
 CString m_strDescription;
    CString m_strPreset;
    CString m_strNotification;
    CComboBox m_cmbNotifications;
    CComboBox m_cmbPresets;
    CEdit m_edtPullUrl;
    CComboBox m_cmbRecPresets;
    CComboBox m_cmbSecurityPolicies;
    CComboBox m_cmbRegions;
    CButton m_chkPull;
    CString m_strRecPreset;
    CString m_strSecurityPolicy;
    CString m_strRegion;
    CString m_strPullUrl;
    BOOL m_bPull;

protected:
    void UpdatePullUrlEdit();
};
