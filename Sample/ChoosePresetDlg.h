#pragma once

#include <live_capture.h>
#include "afxwin.h"

// ѡ��ģ��Ի���
class CChoosePresetDlg : public CDialog {
    DECLARE_DYNAMIC(CChoosePresetDlg)

public:
    // lc_transcode_preset_list_t* list: ����������û���ѡ��ģ���б�
    // lc_transcode_preset_t** preset: ����������û�ѡ����ģ��
    CChoosePresetDlg(lc_list_t list,
                     lc_transcode_preset_t** preset,
                     CWnd* pParent = NULL);
    virtual ~CChoosePresetDlg();

private:
    enum { IDD = IDD_DIALOG_CHOOSE_PRESET };
private:
    // ����OnOk�������û�����Enter��������ѡ����
    void OnOk();
    // ����OnCancel�������û�����ESC��������ȡ������
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
