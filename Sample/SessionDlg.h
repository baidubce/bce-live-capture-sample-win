#pragma once
#include "afxcmn.h"
#include <live_capture.h>

class CToolsDlg;
class CMFCPropertyGridCtrl;
class CMFCPropertyGridProperty;

#include "property_grid_helper.h"


class CSessionDlg : public CDialog {
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
    CMFCPropertyGridCtrl* m_ppgSession;
    lc_session_list_t* m_pSessionList;
    lc_session_t* m_pSelectedSession;

    BOOL m_bSelectMode;
    CTreeCtrl m_treeSession;

protected:
    void OnMenuCmd(int cmd, lc_session_t* session, HTREEITEM item);

    void OnDeleteSession(lc_session_t* session, HTREEITEM item);
    void OnResumeSession(lc_session_t* session);
    void OnStopSession(lc_session_t* session);

    void UpdateSessionList();

    BOOL OnInitDialog();
    void UpdatePropertyList(lc_session_t* session, BOOL allowEdit);
    void UpdatePropertyItem(CMFCPropertyGridProperty* pgp, lc_session_t* session,
                            BOOL allowEdit);
    void UpdateSession(lc_session_t* session);

private:
    void CreatePropertyGrid();
    CMFCPropertyGridProperty* CreatePropertyItem(const PropertyItem* pi);

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
