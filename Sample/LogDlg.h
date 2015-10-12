#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CToolsDlg;

class CLogDlg : public CDialog {
    DECLARE_DYNAMIC(CLogDlg)

public:
    CLogDlg(CWnd* pParent = NULL);
    virtual ~CLogDlg();

    enum { IDD = IDD_LOG_DIALOG };
    enum { MAX_LOG_LINES = 2048 };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    void OnOK();
    void OnCancel();
    BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()

private:
    CToolsDlg* m_pParent;

public:
    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnNcDestroy();
    afx_msg void OnBnClickedButtonSaveas();
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnBnClickedButtonShutdown();
    afx_msg void OnCbnSelchangeComboLevels();
    afx_msg LRESULT OnAddMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnClearMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnSetFilter(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSizing(UINT fwSide, LPRECT pRect);

private:
    CRichEditCtrl m_edtLog;
    CComboBox m_cmbLevels;
    CStatic m_Static;
    CStatic m_stLevel;
    CButton m_btSaveAs;
    CButton m_btClear;
    CButton m_btClose;
};
