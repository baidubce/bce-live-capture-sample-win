#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <live_capture.h>
#include "property_grid_helper.h"
#include "PropertyGridDialog.h"

class CToolsDlg;


typedef enum _tagNOTIFFICATION_MODE {
    NOTIFFICATION_MODE_VIEW,
    NOTIFFICATION_MODE_ADD,
} NOTIFFICATION_MODE;



class CDialogNotification : public CPropertyGridDialog {
    DECLARE_DYNAMIC(CDialogNotification)

public:
    CDialogNotification(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDialogNotification();

    // Dialog Data
    enum { IDD = IDD_NOTIFICATION };
private:
    CToolsDlg* m_pParent;
    CButton m_btnMode;
    CButton m_btnCancel;
    NOTIFFICATION_MODE m_eMode;
    CTreeCtrl m_treNotifications;
    lc_list_t m_pNotificationList;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    void UpdateNotificationList();
    void SetMode(NOTIFFICATION_MODE mode);

    DECLARE_MESSAGE_MAP()

    virtual void GetPropertyGridRect(LPRECT rc);
    virtual const PropertyItem* GetPropertyList();
public:
    afx_msg void OnNcDestroy();
    afx_msg void OnTvnSelchangedTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClose();

    afx_msg void OnBnClickedButtonCancel();
    afx_msg void OnBnClickedButtonClose();
    afx_msg void OnBnClickedButtonMode();
    afx_msg void OnBnClickedButtonRefresh();

    afx_msg void OnNMRClickTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTvnSelchangingTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult);

};
