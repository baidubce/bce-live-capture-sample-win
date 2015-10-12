#include "stdafx.h"
#include "Sample.h"
#include "ChoosePresetDlg.h"


IMPLEMENT_DYNAMIC(CChoosePresetDlg, CDialog)

CChoosePresetDlg::CChoosePresetDlg(lc_preset_list_t* list, lc_preset_t** preset,
                                   CWnd* pParent /*=NULL*/)
    : CDialog(CChoosePresetDlg::IDD, pParent)
    , m_pPresetList(list)
    , m_ppPreset(preset) {

}

CChoosePresetDlg::~CChoosePresetDlg() {
}

void CChoosePresetDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_PRESET, m_cmbPresets);
}


BEGIN_MESSAGE_MAP(CChoosePresetDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_OK, &CChoosePresetDlg::OnBnClickedButtonOk)
    ON_BN_CLICKED(IDC_BUTTON_GIVEUP, &CChoosePresetDlg::OnBnClickedButtonGiveup)
END_MESSAGE_MAP()

// 用户确定选择，保存选中的模板，关闭对话框
void CChoosePresetDlg::OnBnClickedButtonOk() {
    int id = m_cmbPresets.GetCurSel();

    if (id >= 0) {
        *m_ppPreset = (lc_preset_t*) m_cmbPresets.GetItemData(id);
    } else {
        *m_ppPreset = NULL;
    }

    EndDialog(IDOK);
}

void CChoosePresetDlg::OnBnClickedButtonGiveup() {
    *m_ppPreset = NULL;
    EndDialog(IDCANCEL);
}

void CChoosePresetDlg::OnOk() {
    OnBnClickedButtonOk();
}

void CChoosePresetDlg::OnCancel() {
    *m_ppPreset = NULL;
    CDialog::OnCancel();
}

// 初始化ComboBox，将模板列表添加到ComboBox中
BOOL CChoosePresetDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    int id = m_cmbPresets.AddString("不选择");
    m_cmbPresets.SetCurSel(id);

    if (m_pPresetList) {
        for (int i = 0; i < m_pPresetList->count;
                i ++) {
            id = m_cmbPresets.AddString(m_pPresetList->preset_list[i].presetName);

            if (id >= 0) {
                m_cmbPresets.SetItemData(id, (DWORD_PTR) &m_pPresetList->preset_list[i]);
            }
        }
    }

    return TRUE;
}