#include "StdAfx.h"
#include "PropertyGridDialog.h"

class CMFCPropertyGridPropertyAccess: public CMFCPropertyGridProperty {
public:
    void AllowEditEx(BOOL bAllow = TRUE) {
        CMFCPropertyGridProperty::AllowEdit(bAllow);
        CMFCPropertyGridProperty::Enable(bAllow);
    }
};

class CMFCPropertyGridPropertyGroup: public CMFCPropertyGridProperty {
public:

    CMFCPropertyGridPropertyGroup(const CString& strName, const COleVariant& varValue,
                                  LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
                                  LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL)
        : CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate,
                                   lpszValidChars) {
    }
    CMFCPropertyGridPropertyGroup(const CString& strGroupName, DWORD_PTR dwData = 0,
                                  BOOL bIsValueList = FALSE)
        : CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList) {
    }


    void OnDrawExpandBox(CDC* pDC, CRect rect) {
        ASSERT_VALID(this);
        ASSERT_VALID(pDC);
        ASSERT_VALID(m_pWndList);
        ASSERT(IsGroup());

        CPoint ptCenter = rect.CenterPoint();

        int nBoxSize = min(14, rect.Width());

        rect = CRect(ptCenter, CSize(1, 1));
        rect.InflateRect(nBoxSize / 2, nBoxSize / 2);

        if (m_bIsValueList) {
            pDC->FillRect(rect, &afxGlobalData.brWindow);
        }

        CMFCVisualManager::GetInstance()->OnDrawCheckBox(pDC, rect, FALSE, m_bExpanded
                && !m_lstSubItems.IsEmpty(),  m_bEnabled);

    }
};

IMPLEMENT_DYNAMIC(CPropertyGridDialog, CDialog)


BEGIN_MESSAGE_MAP(CPropertyGridDialog, CDialog)
    ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CPropertyGridDialog::OnPropertyChanged)
END_MESSAGE_MAP()

CPropertyGridDialog::CPropertyGridDialog(UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/)
    : CDialog(nIDTemplate, pParentWnd)
    , m_ppgCtrl(0)
    , m_bAllowEdit(0) {

}

CPropertyGridDialog::CPropertyGridDialog(void)
    : m_ppgCtrl(0)
    , m_bAllowEdit(0) {
}

CPropertyGridDialog::~CPropertyGridDialog(void) {
    if (m_ppgCtrl) {
        delete m_ppgCtrl;
    }
}

void CPropertyGridDialog::CreatePropertyGrid() {

    CRect rcClient;
    GetPropertyGridRect(rcClient);

    m_ppgCtrl = new CMFCPropertyGridCtrl();

    m_ppgCtrl->Create(WS_VISIBLE | WS_CHILD, rcClient, this, 0);
    m_ppgCtrl->EnableHeaderCtrl(FALSE);
    m_ppgCtrl->EnableDescriptionArea(FALSE);

    const PropertyItem* pi = GetPropertyList();

    while (pi->Name) {
        CMFCPropertyGridProperty* prop = CreatePropertyItem(pi);
        m_ppgCtrl->AddProperty(prop);
        pi ++;
    }
}

CMFCPropertyGridProperty* CPropertyGridDialog::CreatePropertyItem(const PropertyItem* pi) {
    CMFCPropertyGridProperty* ret = NULL;

    if (pi->Read || pi->Write) {
        if (pi->SubItems) {
            ret = new CMFCPropertyGridPropertyGroup(pi->Name, (DWORD_PTR)pi);
        } else {
            ret = new CMFCPropertyGridProperty(pi->Name, COleVariant(), NULL, (DWORD_PTR)pi);
        }
    } else {
        ret = new CMFCPropertyGridProperty(pi->Name, (DWORD_PTR)pi);
    }

    if (pi->SubItems) {
        const PropertyItem* subPi = pi->SubItems;

        while (subPi->Name) {
            CMFCPropertyGridProperty* subItem = CreatePropertyItem(subPi);
            ret->AddSubItem(subItem);
            subPi ++;
        }
    }

    if (pi->Options) {
        const PropertyNameValue* cur = pi->Options;

        while (cur->Name) {
            ret->AddOption(cur->Name);
            cur ++;
        }
    }

    ((CMFCPropertyGridPropertyAccess*)ret)->AllowEditEx(m_bAllowEdit);

    return ret;
}

// 将数据从界面控件中更新到模板中
void CPropertyGridDialog::BuildFromPropertyList(void* data) {
    int count = m_ppgCtrl->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty*  pgp = m_ppgCtrl->GetProperty(i);

        if (pgp) {
            SavePropertyValue(pgp, data);
        }
    }
}

void CPropertyGridDialog::SavePropertyValue(CMFCPropertyGridProperty* pgp, void* data) {
    if (pgp && pgp->IsVisible()) {
        const PropertyItem* item = (const PropertyItem*) pgp->GetData();

        int count = pgp->GetSubItemsCount();

        for (int i = 0; i < count ; i ++) {
            CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

            SavePropertyValue(subPgp, data);
        }

        if (item && item->Write) {
            item->Write(data, item, pgp);
        }
    }
}


// 将数据更新到界面控件中
void CPropertyGridDialog::UpdatePropertyList(void* data, BOOL allowEdit) {

    m_bAllowEdit = allowEdit;
    int count = m_ppgCtrl->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty* pgp = m_ppgCtrl->GetProperty(i);

        if (pgp) {
            UpdatePropertyItem(pgp, data, allowEdit);
        }
    }
}

void CPropertyGridDialog::UpdatePropertyItem(CMFCPropertyGridProperty* pgp, void* data,
        BOOL allowEdit) {

    ((CMFCPropertyGridPropertyAccess*)pgp)->AllowEditEx(allowEdit);

    const PropertyItem* itemData = (PropertyItem*) pgp->GetData();

    if (data && itemData) {
        if (!allowEdit) {
            if (itemData->Validate) {
                if (!itemData->Validate(data, itemData, pgp)) {
                    pgp->Show(FALSE);
                    return;
                } else {
                    pgp->Show(TRUE);

                    if (pgp->IsGroup() && !pgp->IsExpanded()) {
                        pgp->Expand();
                    }
                }
            }
        } else {
            pgp->Show();

            if (pgp->IsGroup() && !pgp->IsExpanded()) {
                pgp->Expand();
            }
        }

        if (itemData->Read) {
            itemData->Read(data, itemData, pgp);
        }

        if (allowEdit && !itemData->Write) {
            if (!itemData->Validate || itemData->Validate(data, itemData, pgp)) {
                pgp->Show(TRUE);
                ((CMFCPropertyGridPropertyAccess*)pgp)->AllowEditEx(FALSE);
            } else {
                pgp->Show(FALSE);
            }
        }
    } else {
        COleVariant var = pgp->GetValue();
        COleVariant newVar;
        newVar.ChangeType(var.vt);
        pgp->SetValue(newVar);
        ((CMFCPropertyGridPropertyAccess*)pgp)->AllowEditEx(FALSE);
    }

    int count = pgp->GetSubItemsCount();

    for (int i = 0; i < count; i++) {
        CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

        if (subPgp) {
            UpdatePropertyItem(subPgp, data, allowEdit);
        }
    }
}

// 属性值改变，通知子类更新属性
LRESULT CPropertyGridDialog::OnPropertyChanged(WPARAM wParam, LPARAM lParam) {
    CMFCPropertyGridProperty* pgp = reinterpret_cast<CMFCPropertyGridProperty*>(lParam);
    const PropertyItem* pi = (const PropertyItem*) pgp->GetData();

    OnPropertyChanged(pi, pgp);

    return 0;
}

void CPropertyGridDialog::OnPropertyChanged(const PropertyItem* pi, CMFCPropertyGridProperty* pgp) {

}
