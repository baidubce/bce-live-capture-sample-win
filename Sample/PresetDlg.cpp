#include "stdafx.h"
#include "Sample.h"
#include "PresetDlg.h"
#include "ToolsDlg.h"
#include "afxpropertygridctrl.h"
#include "choosepresetdlg.h"
#include <stack>
#include "transcode_preset_items.h"

#define ID_DELETE 0x8001

IMPLEMENT_DYNAMIC(CPresetDlg, CPropertyGridDialog)

CPresetDlg::CPresetDlg(CWnd* pParent /*=NULL*/)
    : CPropertyGridDialog(CPresetDlg::IDD, pParent)
    , m_pParent(DYNAMIC_DOWNCAST(CToolsDlg, pParent))
    , m_eMode(MODE_VIEW)
    , m_pPresetList(0) {

}

CPresetDlg::~CPresetDlg() {

    if (m_pPresetList) {
        //调用API，释放模板列表
        lc_list_free(m_pPresetList);
    }
}

void CPresetDlg::GetPropertyGridRect(LPRECT rc)  {
    CRect rcTree;
    CRect rcClient;
    GetClientRect(rcClient);
    m_trePresets.GetWindowRect(rcTree);

    ScreenToClient(rcTree);
    rcClient.left = rcTree.right;

    rcClient.DeflateRect(3, 3, 3, 52);

    *rc = rcClient;
}

const PropertyItem* CPresetDlg::GetPropertyList() {

    return &s_transcode_preset_items[0];
}

void CPresetDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_PRESETS, m_trePresets);
    DDX_Control(pDX, IDC_BUTTON_MODE, m_btnMode);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
}

BOOL CPresetDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    UpdatePresetList();

    CreatePropertyGrid();
    return TRUE;
}

BEGIN_MESSAGE_MAP(CPresetDlg, CPropertyGridDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PRESET, &CPresetDlg::OnTcnSelchangeTabPreset)
    ON_WM_NCDESTROY()
    ON_WM_CLOSE()
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRESETS, &CPresetDlg::OnTvnSelchangedTreePresets)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CPresetDlg::OnBnClickedButtonClose)
    ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CPresetDlg::OnBnClickedButtonCancel)
    ON_BN_CLICKED(IDC_BUTTON_MODE, &CPresetDlg::OnBnClickedButtonMode)
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CPresetDlg::OnBnClickedButtonRefresh)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_PRESETS, &CPresetDlg::OnNMRClickTreePresets)
    ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_PRESETS, &CPresetDlg::OnTvnSelchangingTreePresets)
END_MESSAGE_MAP()


void CPresetDlg::OnTcnSelchangeTabPreset(NMHDR* pNMHDR, LRESULT* pResult) {
    *pResult = 0;
}

void CPresetDlg::OnClose() {
    DestroyWindow();
}
void CPresetDlg::OnNcDestroy() {
    CDialog::OnNcDestroy();

    if (m_pParent) {
        m_pParent->OnPresetDlgClosed(this);
    }
}
void CPresetDlg::OnOK() {}
void CPresetDlg::OnCancel() {
    OnClose();
}

void CPresetDlg::OnTvnSelchangedTreePresets(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    lc_transcode_preset_t* preset = NULL;

    if (m_eMode == MODE_VIEW) {

        if (pNMTreeView && pNMTreeView->itemNew.hItem) {
            preset = (lc_transcode_preset_t*) m_trePresets.GetItemData(pNMTreeView->itemNew.hItem);
        }

        UpdatePropertyList(preset, FALSE);
    }

    *pResult = 0;
}


void CPresetDlg::OnBnClickedButtonClose() {
    DestroyWindow();
}

void CPresetDlg::OnBnClickedButtonCancel() {
    if (IDYES == MessageBox(_T("确定要放弃正在编辑的转码模板吗？"), _T("提示"),
                            MB_YESNO | MB_ICONINFORMATION)) {
        HTREEITEM item = m_trePresets.GetSelectedItem();
        lc_transcode_preset_t* preset = NULL;

        if (item) {
            preset = (lc_transcode_preset_t*)m_trePresets.GetItemData(item);
        }

        UpdatePropertyList(preset, FALSE);

        SetMode(MODE_VIEW);


    }
}

void CPresetDlg::SetMode(PRESET_MODE mode) {
    m_eMode = mode;

    if (m_eMode == MODE_VIEW) {
        m_btnMode.SetWindowText(_T("添加新模板"));
        m_btnCancel.ShowWindow(SW_HIDE);
    } else {

        m_btnMode.SetWindowText(_T("保存"));
        m_btnCancel.ShowWindow(SW_SHOW);
    }
}

// 编辑与只读模式之间切换
void CPresetDlg::OnBnClickedButtonMode() {
    USES_CONVERSION;

    if (m_eMode == MODE_VIEW) {
        lc_transcode_preset_t* preset = NULL;

        if (ChoosePreset(m_pPresetList, &preset)) {
            lc_transcode_preset_t newPreset = {0};

            if (preset) {
                newPreset = *preset;
            }
            memset(&newPreset.create_time, 0 ,sizeof(lc_datetime_t));

            newPreset.name[0] = '\0';
            UpdatePropertyList(&newPreset, TRUE);

            setForwardOnly(newPreset.forward_only);

            SetMode(MODE_ADD);
        }
    } else {
        lc_transcode_preset_t preset;
        lc_transcode_preset_init(&preset);
        BuildFromPropertyList(&preset);
        LC_CODE code = LC_OK;

        //调用API，创建模板
        if (LC_OK == (code = lc_transcode_preset_create(&preset))) {
            SetMode(MODE_VIEW);
            UpdatePresetList();
        } else {
            CString strmsg;
            strmsg.Format(_T("操作失败\r\n%s"), A2T(lc_get_last_error()));
            MessageBox(strmsg, _T("执行错误"));
        }
    }
}

void CPresetDlg::OnBnClickedButtonRefresh() {
    UpdatePresetList();
}

// 从服务端更新模板列表，刷新列表控件
void CPresetDlg::UpdatePresetList() {
    USES_CONVERSION;

    m_trePresets.DeleteAllItems();

    if (m_pPresetList) {
        //调用API，释放模板列表
        lc_list_free(m_pPresetList);
        m_pPresetList = NULL;
    }

    //调用API，从服务端读取模板列表
    if (LC_OK == lc_transcode_preset_list(&m_pPresetList)) {
        for (int i = 0 ;
                i < lc_list_count(m_pPresetList);
                i ++) {
            lc_transcode_preset_t* preset = (lc_transcode_preset_t*)lc_list_get_at(m_pPresetList, i);
            HTREEITEM item = m_trePresets.InsertItem(A2T(preset->name));
            m_trePresets.SetItemData(item, (DWORD_PTR)preset);
        }
    } else {
        CString strmsg;
        strmsg.Format(_T("操作失败\r\n%s"), A2T(lc_get_last_error()));
        MessageBox(strmsg, _T("执行错误"));
    }
}
// 如果右键点击模板，弹出删除菜单
void CPresetDlg::OnNMRClickTreePresets(NMHDR* pNMHDR, LRESULT* pResult) {
    USES_CONVERSION;

    CPoint ptCursor;
    CPoint ptClient;
    GetCursorPos(&ptCursor);
    ptClient = ptCursor;
    ::ScreenToClient(pNMHDR->hwndFrom, &ptClient);

    HTREEITEM item = m_trePresets.HitTest(ptClient);

    if (item) {
        m_trePresets.SelectItem(item);
        HMENU hPop = CreatePopupMenu();
        InsertMenu(hPop, 0, MF_BYPOSITION, ID_DELETE, _T("删除"));
        int cmd = TrackPopupMenu(hPop,
                                 TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                 ptCursor.x, ptCursor.y, 0, GetSafeHwnd(), NULL);
        DestroyMenu(hPop);

        if (cmd == ID_DELETE) {

            lc_transcode_preset_t* preset = (lc_transcode_preset_t*)m_trePresets.GetItemData(item);

            if (preset) {
                CString msg;
                msg.Format(_T("确定要删除'%s'吗?"), A2T(preset->name));

                if (IDYES == MessageBox(msg, _T("确认"), MB_YESNO)) {
                    //调用API，从服务端删除模板
                    if (LC_OK == lc_transcode_preset_delete(preset->name)) {
                        m_trePresets.DeleteItem(item);
                        lc_list_remove(m_pPresetList, preset);
                    } else {
                        CString strmsg;
                        strmsg.Format(_T("操作失败\r\n%s"), A2T(lc_get_last_error()));
                        MessageBox(strmsg, _T("执行错误"));
                    }
                }
            }
        }
    }

    *pResult = 0;
}

void CPresetDlg::OnTvnSelchangingTreePresets(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    *pResult = 0;
}

BOOL CPresetDlg::ChoosePreset(lc_list_t list, lc_transcode_preset_t** preset) {
    CChoosePresetDlg dlg(list, preset, this);

    if (dlg.DoModal() == IDOK) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void CPresetDlg::OnPropertyChanged(const PropertyItem* pi, CMFCPropertyGridProperty* pgp) {
    if (0 == _tcsicmp(pi->Name, _T("ForwardOnly"))) {

        COleVariant var = pgp->GetValue();
        BOOL forwardOnly = (_wcsicmp(var.bstrVal, L"TRUE") == 0);

        setForwardOnly(forwardOnly);
    }
}

void CPresetDlg::setForwardOnly(BOOL forwardOnly) {
    const PropertyItem* item = GetPropertyList();

    while (item->Name) {
        if (0 == _tcsicmp(item->Name, _T("视频"))) {
            CMFCPropertyGridProperty* videopgp = m_ppgCtrl->FindItemByData((DWORD_PTR)item);

            if (videopgp) {
                videopgp->Show(!forwardOnly);
            }
        } else if (0 == _tcsicmp(item->Name, _T("音频"))) {
            CMFCPropertyGridProperty* audiopgp = m_ppgCtrl->FindItemByData((DWORD_PTR)item);

            if (audiopgp) {
                audiopgp->Show(!forwardOnly);
            }
        }

        item ++;
    }
}