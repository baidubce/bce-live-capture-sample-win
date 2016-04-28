#pragma once
#include "afxcmn.h"
#include <live_capture.h>
#include "property_grid_helper.h"
#include "PropertyGridDialog.h"

class CToolsDlg;

class CSessionDlg : public CPropertyGridDialog {
    DECLARE_DYNAMIC(CSessionDlg)

public:
    CSessionDlg(CWnd* pParent = NULL, BOOL select_mode = FALSE);
    virtual ~CSessionDlg();

    enum { IDD = IDD_SESSION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    void OnOK();
    void OnCancel();

    DECLARE_MESSAGE_MAP()
public:

    CString GetSelectedSessionId();
private:
    CToolsDlg* m_pParent;
    lc_list_t m_pSessionList;
    lc_session_t* m_pSelectedSession;

    BOOL m_bSelectMode;
    CTreeCtrl m_treeSession;

protected:
    void OnMenuCmd(int cmd, lc_session_t* session, HTREEITEM item);

    void OnDeleteSession(lc_session_t* session, HTREEITEM item);
    void OnResumeSession(lc_session_t* session);
    void OnPauseSession(lc_session_t* session);

    void UpdateSessionList();

    BOOL OnInitDialog();
    void UpdateSession(lc_session_t* session);

	BOOL ValidSession(lc_session_t* session);
protected:
    virtual void GetPropertyGridRect(LPRECT rc);
    virtual const PropertyItem* GetPropertyList();

public:
    afx_msg void OnNcDestroy();
    afx_msg void OnClose();
    afx_msg void OnBnClickedButtonRefresh();
    afx_msg void OnBnClickedButtonClose();
    afx_msg void OnTvnSelchangedTreeSession(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMRClickTreeSession(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMDblclkTreeSession(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedButtonCreate();
};
