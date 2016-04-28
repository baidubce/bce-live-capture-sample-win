#pragma once
#include "PropertyGridDialog.h"

// CDialogUserPreset dialog

class CDialogUserPreset : public CPropertyGridDialog
{
	DECLARE_DYNAMIC(CDialogUserPreset)

public:
	CDialogUserPreset(lc_transcode_preset_t*preset, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogUserPreset();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER_PRESET };
private:
    lc_transcode_preset_t* m_pPreset;
    lc_transcode_preset_t m_sNewPreset;
protected:
    virtual BOOL OnInitDialog();
    virtual void GetPropertyGridRect(LPRECT rc);
    virtual const PropertyItem* GetPropertyList();
    virtual void OnOK();
    virtual void OnCancel();

    BOOL ValidPreset(lc_transcode_preset_t* preset);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
};
