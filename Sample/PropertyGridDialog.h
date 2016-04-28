#ifndef __PROPERTY_GRID_DIALOG_H__
#define __PROPERTY_GRID_DIALOG_H__

#include "afxwin.h"
#include "property_grid_helper.h"


class CMFCPropertyGridCtrl;
class CMFCPropertyGridProperty;



class CPropertyGridDialog :
    public CDialog {
        DECLARE_DYNAMIC(CPropertyGridDialog);
public:
    CPropertyGridDialog(void);

    CPropertyGridDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

    ~CPropertyGridDialog(void);

protected:
    CMFCPropertyGridCtrl* m_ppgCtrl;

    BOOL m_bAllowEdit;

    DECLARE_MESSAGE_MAP()

    afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
protected:
    void CreatePropertyGrid();
    CMFCPropertyGridProperty* CreatePropertyItem(const PropertyItem* pi);
    void UpdatePropertyList(void* data, BOOL allowEdit);
    void UpdatePropertyItem(CMFCPropertyGridProperty* pgp, void* data, BOOL allowEdit);
    void BuildFromPropertyList(void* data);
    void SavePropertyValue(CMFCPropertyGridProperty* pgp, void* data);

    virtual void GetPropertyGridRect(LPRECT rc) = 0;
    virtual const PropertyItem* GetPropertyList() = 0;
    virtual void OnPropertyChanged(const PropertyItem* pi, CMFCPropertyGridProperty* pgp);
};

#endif //__PROPERTY_GRID_DIALOG_H__