#include "stdafx.h"
#include "Sample.h"
#include "ChoosePresetDlg.h"

IMPLEMENT_DYNAMIC(CChoosePresetDlg, CDialog)

CChoosePresetDlg::CChoosePresetDlg(lc_list_t list, lc_transcode_preset_t** preset,
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
        *m_ppPreset = (lc_transcode_preset_t*) m_cmbPresets.GetItemData(id);
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

    USES_CONVERSION;

    int id = m_cmbPresets.AddString(_T("不选择"));
    m_cmbPresets.SetCurSel(id);

    if (m_pPresetList) {
        for (int i = 0; i < lc_list_count(m_pPresetList);
                i ++) {
                    lc_transcode_preset_t* preset = (lc_transcode_preset_t*)lc_list_get_at(m_pPresetList, i);
                    id = m_cmbPresets.AddString(A2T(preset->name));

            if (id >= 0) {
                m_cmbPresets.SetItemData(id, (DWORD_PTR)preset);
            }
        }
    }

    return TRUE;
}