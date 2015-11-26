#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <live_capture.h>


class CToolsDlg;
class CMFCPropertyGridCtrl;
class CMFCPropertyGridProperty;

#include "property_grid_helper.h"

typedef enum _tagNOTIFFICATION_MODE {
    NOTIFFICATION_MODE_VIEW,
    NOTIFFICATION_MODE_ADD,
} NOTIFFICATION_MODE;



class CDialogNotification : public CDialog {
    DECLARE_DYNAMIC(CDialogNotification)

public:
    CDialogNotification(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDialogNotification();

    // Dialog Data
    enum { IDD = IDD_NOTIFICATION };
private:
    CToolsDlg* m_pParent;
    CMFCPropertyGridCtrl* m_ppgNotification;
    CButton m_btnMode;
    CButton m_btnCancel;
    NOTIFFICATION_MODE m_eMode;
    CTreeCtrl m_treNotifications;
    lc_notification_list_t* m_pNotificationList;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    void UpdatePropertyList(lc_notification_t* notification, BOOL allowEdit);
    void UpdatePropertyItem(CMFCPropertyGridProperty* pgp, lc_notification_t* notification,
                            BOOL allowEdit);
    void CreatePropertyGrid();
    CMFCPropertyGridProperty* CreatePropertyItem(const PropertyItem* pi);
    void UpdateNotificationList();
    void BuildNotificationFromPropertyList(lc_notification_t* notification);
    void SavePropertyValue(CMFCPropertyGridProperty* pgp, lc_notification_t* notification);
    void SetMode(NOTIFFICATION_MODE mode);

    DECLARE_MESSAGE_MAP()
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
