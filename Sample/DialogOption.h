#pragma once


class CToolsDlg;

// CDialogOption dialog

class CDialogOption : public CDialog {
    DECLARE_DYNCREATE(CDialogOption)

public:
    CDialogOption(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDialogOption();

    // Dialog Data
    enum { IDD = IDD_OPTION };
private:
    CToolsDlg* m_pParent;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonOk();
    afx_msg void OnBnClickedButtonClose();
    afx_msg void OnNcDestroy();
    afx_msg void OnClose();
    CString m_strAK;
    CString m_strSK;
    CString m_strHost;

    CString m_strAKbak;
    CString m_strSKbak;
    CString m_strHostbak;
    afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickSyslink2(NMHDR *pNMHDR, LRESULT *pResult);
};
