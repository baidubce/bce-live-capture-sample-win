#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <live_capture.h>


class CToolsDlg;
class CMFCPropertyGridCtrl;
class CMFCPropertyGridProperty;

#include "property_grid_helper.h"


typedef enum _tagPRESET_MODE {
    MODE_VIEW,
    MODE_ADD,
} PRESET_MODE;


class CPresetDlg : public CDialog {
    DECLARE_DYNAMIC(CPresetDlg)

public:
    CPresetDlg(CWnd* pParent = NULL);
    virtual ~CPresetDlg();
    enum { IDD = IDD_PRESET };

private:
    CToolsDlg* m_pParent;
    CMFCPropertyGridCtrl* m_ppgPreset;
    CButton m_btnMode;
    CButton m_btnCancel;
    PRESET_MODE m_eMode;
    CTreeCtrl m_trePresets;
    lc_preset_list_t* m_pPresetList;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    BOOL OnInitDialog();
    void OnOK();
    void OnCancel();
    void UpdatePropertyList(lc_preset_t* preset, BOOL allowEdit);
    void UpdatePropertyItem(CMFCPropertyGridProperty* pgp, lc_preset_t* preset, BOOL allowEdit);
    void CreatePropertyGrid();
    CMFCPropertyGridProperty* CreatePropertyItem(const PropertyItem* pi);
    void UpdatePresetList();
    BOOL ChoosePreset(lc_preset_list_t* list, lc_preset_t** preset);
    void BuildPresetFromPropertyList(lc_preset_t* preset);
    void SavePropertyValue(CMFCPropertyGridProperty* pgp, lc_preset_t* preset);
    void SetMode(PRESET_MODE mode);

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnNcDestroy();
    afx_msg void OnTvnSelchangedTreePresets(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTcnSelchangeTabPreset(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnClose();
    afx_msg void OnBnClickedButtonClose();
    afx_msg void OnBnClickedButtonCancel();
    afx_msg void OnBnClickedButtonMode();
    afx_msg void OnBnClickedButtonRefresh();
    afx_msg void OnNMRClickTreePresets(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnTvnSelchangingTreePresets(NMHDR* pNMHDR, LRESULT* pResult);
};
