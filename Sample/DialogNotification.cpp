#include "stdafx.h"
#include "Sample.h"
#include "DialogNotification.h"
#include "ToolsDlg.h"
#include "notification_items.h"

#define ID_DELETE 0x7f0


// CDialogNotification dialog

IMPLEMENT_DYNAMIC(CDialogNotification, CPropertyGridDialog)

CDialogNotification::CDialogNotification(CWnd* pParent /*=NULL*/)
    : CPropertyGridDialog(CDialogNotification::IDD, pParent)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent))
    , m_eMode(NOTIFFICATION_MODE_VIEW)
    , m_pNotificationList(0) {

}

CDialogNotification::~CDialogNotification() {
    if (m_pNotificationList) {
        //����API���ͷ�֪ͨ�б�
        lc_list_free(m_pNotificationList);
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


void CDialogNotification::GetPropertyGridRect(LPRECT rc) {
    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);
    m_treNotifications.GetWindowRect(rcTree);

    ScreenToClient(rcTree);
    rcClient.left = rcTree.right;

    rcClient.DeflateRect(3, 3, 3, 52);
    *rc = rcClient;
}

const PropertyItem* CDialogNotification::GetPropertyList() {
    return &s_notification_items[0];
}

BEGIN_MESSAGE_MAP(CDialogNotification, CPropertyGridDialog)
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


// CDialogNotification message handlers

void CDialogNotification::OnBnClickedButtonCancel() {
    if (IDYES == MessageBox(_T("ȷ��Ҫ�������ڱ༭��֪ͨ��"), _T("��ʾ"),
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
        m_btnMode.SetWindowText(_T("���֪ͨ"));
        m_btnCancel.ShowWindow(SW_HIDE);
    } else {

        m_btnMode.SetWindowText(_T("����"));
        m_btnCancel.ShowWindow(SW_SHOW);
    }
}
void CDialogNotification::OnBnClickedButtonMode() {
    USES_CONVERSION;

    if (m_eMode == NOTIFFICATION_MODE_VIEW) {
        lc_notification_t newNotification = {0};


        UpdatePropertyList(&newNotification, TRUE);

        SetMode(NOTIFFICATION_MODE_ADD);
    } else {
        lc_notification_t notification = {0};
        BuildFromPropertyList(&notification);
        LC_CODE code = LC_OK;

        //����API������֪ͨ
        if (LC_OK == (code = lc_notification_create(&notification))) {
            SetMode(NOTIFFICATION_MODE_VIEW);
            UpdateNotificationList();
        } else {
            CString strmsg;
            strmsg.Format(_T("����ʧ��\r\n%s"), A2T(lc_get_last_error()));
            MessageBox(strmsg, _T("ִ�д���"));
        }
    }
}


void CDialogNotification::OnBnClickedButtonRefresh() {
    UpdateNotificationList();
}

// �ӷ���˸���֪ͨ�б�ˢ���б�ؼ�
void CDialogNotification::UpdateNotificationList() {
    USES_CONVERSION;

    m_treNotifications.DeleteAllItems();

    if (m_pNotificationList) {
        //����API���ͷ�֪ͨ�б�
        lc_list_free(m_pNotificationList);
        m_pNotificationList = NULL;
    }

    //����API���ӷ���˶�ȡ֪ͨ�б�
    if (LC_OK == lc_notification_list(&m_pNotificationList)) {
        for (int i = 0 ;
                i < lc_list_count(m_pNotificationList);
                i ++) {
            lc_notification_t* notification = (lc_notification_t*)lc_list_get_at(m_pNotificationList, i);
            HTREEITEM item = m_treNotifications.InsertItem(A2T(notification->name));
            m_treNotifications.SetItemData(item, (DWORD_PTR)notification);
        }
    } else {
        CString strmsg;
        strmsg.Format(_T("����ʧ��\r\n%s"), A2T(lc_get_last_error()));
        MessageBox(strmsg, _T("ִ�д���"));
    }
}
// ����Ҽ����֪ͨ������ɾ���˵�
void CDialogNotification::OnNMRClickTreeNotifications(NMHDR* pNMHDR, LRESULT* pResult) {
    USES_CONVERSION;

    CPoint ptCursor;
    CPoint ptClient;
    GetCursorPos(&ptCursor);
    ptClient = ptCursor;
    ::ScreenToClient(pNMHDR->hwndFrom, &ptClient);

    HTREEITEM item = m_treNotifications.HitTest(ptClient);

    if (item) {
        m_treNotifications.SelectItem(item);
        HMENU hPop = CreatePopupMenu();
        InsertMenu(hPop, 0, MF_BYPOSITION, ID_DELETE, _T("ɾ��"));
        int cmd = TrackPopupMenu(hPop,
                                 TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                 ptCursor.x, ptCursor.y, 0, GetSafeHwnd(), NULL);
        DestroyMenu(hPop);

        if (cmd == ID_DELETE) {

            lc_notification_t* notification = (lc_notification_t*)m_treNotifications.GetItemData(item);

            if (notification) {
                CString msg;
                msg.Format(_T("ȷ��Ҫɾ��'%s'��?"), notification->name);

                if (IDYES == MessageBox(msg, _T("ȷ��"), MB_YESNO)) {
                    //����API���ӷ����ɾ��֪ͨ
                    if (LC_OK == lc_notification_delete(notification->name)) {
                        m_treNotifications.DeleteItem(item);
                        lc_notification_t notification = {0};
                        UpdatePropertyList(&notification, FALSE);

                    } else {
                        CString strmsg;
                        strmsg.Format(_T("����ʧ��\r\n%s"), A2T(lc_get_last_error()));
                        MessageBox(strmsg, _T("ִ�д���"));
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
