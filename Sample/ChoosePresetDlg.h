#pragma once

#include <live_capture.h>
#include "afxwin.h"

// 选择模板对话框
class CChoosePresetDlg : public CDialog {
    DECLARE_DYNAMIC(CChoosePresetDlg)

public:
    // lc_transcode_preset_list_t* list: 输入参数，用户可选的模板列表。
    // lc_transcode_preset_t** preset: 输出参数，用户选定的模板
    CChoosePresetDlg(lc_list_t list,
                     lc_transcode_preset_t** preset,
                     CWnd* pParent = NULL);
    virtual ~CChoosePresetDlg();

private:
    enum { IDD = IDD_DIALOG_CHOOSE_PRESET };
private:
    // 重载OnOk方法，用户按下Enter键，触发选择动作
    void OnOk();
    // 重载OnCancel方法，用户按下ESC键，触发取消运行
    void OnCancel();
    BOOL OnInitDialog();

private:
    lc_list_t m_pPresetList;
    lc_transcode_preset_t** m_ppPreset;
    CComboBox m_cmbPresets;
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonOk();
    afx_msg void OnBnClickedButtonGiveup();
};
