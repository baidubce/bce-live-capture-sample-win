#include "stdafx.h"
#include "Sample.h"
#include "DialogNotification.h"
#include "ToolsDlg.h"

#define ID_DELETE 0x7f0

// notification property map
static const PropertyItem s_notification_items[] = {
    {
        "通知名称",
        offsetof(lc_notification_t, name),
        LC_MAX_NOTIFICATION_NAME_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    {
        "回调地址",
        offsetof(lc_notification_t, endpoint),
        LC_MAX_URL_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL
    },
    { 0, }
};

// CDialogNotification dialog

IMPLEMENT_DYNAMIC(CDialogNotification, CDialog)

CDialogNotification::CDialogNotification(CWnd* pParent /*=NULL*/)
    : CDialog(CDialogNotification::IDD, pParent)
    , m_ppgNotification(0)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent))
    , m_eMode(NOTIFFICATION_MODE_VIEW)
    , m_pNotificationList(0) {

}

CDialogNotification::~CDialogNotification() {
    if (m_ppgNotification) {
        delete m_ppgNotification;
    }

    if (m_pNotificationList) {
        //调用API，释放通知列表
        lc_notification_list_free(m_pNotificationList);
    }
}

void CDialogNotification::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_NOTIFICATIONS, m_treNotifications);
    DDX_Control(pDX, IDC_BUTTON_MODE, m_btnMode);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
}


BOOL CDialogNotification::OnInitDialog() {
    CDialog::OnInitDialog();

    UpdateNotificationList();

    CreatePropertyGrid();
    return TRUE;
}

void CDialogNotification::CreatePropertyGrid() {

    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);
    m_treNotifications.GetWindowRect(rcTree);

    ScreenToClient(rcTree);
    rcClient.left = rcTree.right;

    rcClient.DeflateRect(3, 3, 3, 52);

    m_ppgNotification = new CMFCPropertyGridCtrl();

    m_ppgNotification->Create(WS_VISIBLE | WS_CHILD, rcClient, this, 0);
    m_ppgNotification->EnableHeaderCtrl(FALSE);
    m_ppgNotification->EnableDescriptionArea(FALSE);

    const PropertyItem* pi = &s_notification_items[0];

    while (pi->Name) {
        CMFCPropertyGridProperty* prop = CreatePropertyItem(pi);
        m_ppgNotification->AddProperty(prop);
        pi ++;
    }
}

CMFCPropertyGridProperty* CDialogNotification::CreatePropertyItem(const PropertyItem* pi) {
    CMFCPropertyGridProperty* ret = NULL;

    if (pi->Read || pi->Write) {
        ret = new CMFCPropertyGridProperty(pi->Name, COleVariant(), NULL, (DWORD_PTR)pi);
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

    ret->AllowEdit(FALSE);

    return ret;
}


BEGIN_MESSAGE_MAP(CDialogNotification, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CDialogNotification::OnBnClickedButtonCancel)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDialogNotification::OnBnClickedButtonClose)
    ON_BN_CLICKED(IDC_BUTTON_MODE, &CDialogNotification::OnBnClickedButtonMode)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDialogNotification::OnBnClickedButtonRefresh)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_NOTIFICATIONS,
              &CDialogNotification::OnTvnSelchangedTreeNotifications)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_NOTIFICATIONS, &CDialogNotification::OnNMRClickTreeNotifications)
    ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_NOTIFICATIONS,
              &CDialogNotification::OnTvnSelchangingTreeNotifications)
END_MESSAGE_MAP()

void CDialogNotification::OnClose() {
    DestroyWindow();
}
void CDialogNotification::OnNcDestroy() {
    CDialog::OnNcDestroy();

    if (m_pParent) {
        m_pParent->OnNotificationDlgClosed(this);
    }
}
void CDialogNotification::OnOK() {}
void CDialogNotification::OnCancel() {
    OnClose();
}


void CDialogNotification::OnTvnSelchangedTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    lc_notification_t* notification = NULL;

    if (m_eMode == NOTIFFICATION_MODE_VIEW) {

        if (pNMTreeView && pNMTreeView->itemNew.hItem) {
            notification = (lc_notification_t*) m_treNotifications.GetItemData(pNMTreeView->itemNew.hItem);
        }

        UpdatePropertyList(notification, FALSE);
    }

    *pResult = 0;
}

// 将数据从通知结构体中更新到界面控件中
void CDialogNotification::UpdatePropertyList(lc_notification_t* notification, BOOL allowEdit) {
    int count = m_ppgNotification->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty* pgp = m_ppgNotification->GetProperty(i);

        if (pgp) {
            UpdatePropertyItem(pgp, notification, allowEdit);
        }
    }
}

void CDialogNotification::UpdatePropertyItem(CMFCPropertyGridProperty* pgp,
        lc_notification_t* notification,
        BOOL allowEdit) {
    int count = pgp->GetSubItemsCount();

    for (int i = 0; i < count; i++) {
        CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

        if (subPgp) {
            UpdatePropertyItem(subPgp, notification, allowEdit);
        }
    }

    const PropertyItem* data = (PropertyItem*) pgp->GetData();
    pgp->AllowEdit(allowEdit);

    if (notification && data && data->Read) {
        data->Read(notification, data, pgp);
    } else {
        COleVariant var = pgp->GetValue();
        COleVariant newVar;
        newVar.ChangeType(var.vt);
        pgp->SetValue(newVar);
    }
}


// CDialogNotification message handlers

void CDialogNotification::OnBnClickedButtonCancel() {
    if (IDYES == MessageBox("确定要放弃正在编辑的通知吗？", "提示",
                            MB_YESNO | MB_ICONINFORMATION)) {
        HTREEITEM item = m_treNotifications.GetSelectedItem();
        lc_notification_t* notification = NULL;

        if (item) {
            notification = (lc_notification_t*)m_treNotifications.GetItemData(item);
        }

        UpdatePropertyList(notification, FALSE);

        SetMode(NOTIFFICATION_MODE_VIEW);
    }
}

void CDialogNotification::OnBnClickedButtonClose() {
    DestroyWindow();
}
void CDialogNotification::SetMode(NOTIFFICATION_MODE mode) {
    m_eMode = mode;

    if (m_eMode == NOTIFFICATION_MODE_VIEW) {
        m_btnMode.SetWindowText("添加通知");
        m_btnCancel.ShowWindow(SW_HIDE);
    } else {

        m_btnMode.SetWindowText("保存");
        m_btnCancel.ShowWindow(SW_SHOW);
    }
}
void CDialogNotification::OnBnClickedButtonMode() {
    if (m_eMode == NOTIFFICATION_MODE_VIEW) {
        lc_notification_t newNotification = {0};


        UpdatePropertyList(&newNotification, TRUE);

        SetMode(NOTIFFICATION_MODE_ADD);
    } else {
        lc_notification_t notification = {0};
        BuildNotificationFromPropertyList(&notification);
        LC_CODE code = LC_OK;

        //调用API，创建通知
        if (LC_OK == (code = lc_notification_create(&notification))) {
            SetMode(NOTIFFICATION_MODE_VIEW);
            UpdateNotificationList();
        } else {
            CString strmsg;
            strmsg.Format("操作失败\r\n%s", lc_get_last_error());
            MessageBox(strmsg, "执行错误");
        }
    }
}


void CDialogNotification::OnBnClickedButtonRefresh() {
    UpdateNotificationList();
}

// 从服务端更新通知列表，刷新列表控件
void CDialogNotification::UpdateNotificationList() {
    m_treNotifications.DeleteAllItems();

    if (m_pNotificationList) {
        //调用API，释放通知列表
        lc_notification_list_free(m_pNotificationList);
        m_pNotificationList = NULL;
    }

    //调用API，从服务端读取通知列表
    if (LC_OK == lc_notification_list(&m_pNotificationList)) {
        for (int i = 0 ;
                i < m_pNotificationList->count;
                i ++) {
            lc_notification_t* notification = &m_pNotificationList->list[i];
            HTREEITEM item = m_treNotifications.InsertItem(notification->name);
            m_treNotifications.SetItemData(item, (DWORD_PTR)notification);
        }
    } else {
        CString strmsg;
        strmsg.Format("操作失败\r\n%s", lc_get_last_error());
        MessageBox(strmsg, "执行错误");
    }
}
// 如果右键点击通知，弹出删除菜单
void CDialogNotification::OnNMRClickTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult) {
    CPoint ptCursor;
    CPoint ptClient;
    GetCursorPos(&ptCursor);
    ptClient = ptCursor;
    ::ScreenToClient(pNMHDR->hwndFrom, &ptClient);

    HTREEITEM item = m_treNotifications.HitTest(ptClient);

    if (item) {
        m_treNotifications.SelectItem(item);
        HMENU hPop = CreatePopupMenu();
        InsertMenu(hPop, 0, MF_BYPOSITION, ID_DELETE, "删除");
        int cmd = TrackPopupMenu(hPop,
                                 TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                 ptCursor.x, ptCursor.y, 0, GetSafeHwnd(), NULL);
        DestroyMenu(hPop);

        if (cmd == ID_DELETE) {

            lc_notification_t* notification = (lc_notification_t*)m_treNotifications.GetItemData(item);

            if (notification) {
                CString msg;
                msg.Format("确定要删除'%s'吗?", notification->name);

                if (IDYES == MessageBox(msg, "确认", MB_YESNO)) {
                    //调用API，从服务端删除通知
                    if (LC_OK == lc_notification_delete(notification->name)) {
                        m_treNotifications.DeleteItem(item);
                        lc_notification_t notification = {0};
                        UpdatePropertyList(&notification, FALSE);

                    } else {
                        CString strmsg;
                        strmsg.Format("操作失败\r\n%s", lc_get_last_error());
                        MessageBox(strmsg, "执行错误");
                    }
                }
            }
        }
    }

    *pResult = 0;
}

void CDialogNotification::OnTvnSelchangingTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;
}

// 将数据从界面控件中更新到通知结构体中
void CDialogNotification::BuildNotificationFromPropertyList(lc_notification_t* notification) {
    int count = m_ppgNotification->GetPropertyCount();

    for (int i = 0; i < count; i ++) {
        CMFCPropertyGridProperty*  pgp = m_ppgNotification->GetProperty(i);

        if (pgp) {
            SavePropertyValue(pgp, notification);
        }
    }
}

void CDialogNotification::SavePropertyValue(CMFCPropertyGridProperty* pgp,
        lc_notification_t* notification) {
    const PropertyItem* item = (const PropertyItem*) pgp->GetData();

    int count = pgp->GetSubItemsCount();

    for (int i = 0; i < count ; i ++) {
        CMFCPropertyGridProperty* subPgp = pgp->GetSubItem(i);

        if (subPgp) {
            SavePropertyValue(subPgp, notification);
        }
    }

    if (item && item->Write) {
        item->Write(notification, item, pgp);
    }
}
