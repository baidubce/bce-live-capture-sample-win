#pragma once
#include "afxwin.h"
#include "FindWindow.h"
#include <live_capture.h>


// CDisplayConfigDlg dialog

class CDisplayConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CDisplayConfigDlg)

public:
	CDisplayConfigDlg(lc_display_info_t* info, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDisplayConfigDlg();

// Dialog Data
	enum { IDD = IDD_DISPLAY_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    virtual BOOL OnInitDialog();

public:
    CComboBox m_cmbTypes;
    int m_nXpos;
    int m_nWidth;
    int m_nHeight;
    int m_nYpos;
    CEdit m_edtXpos;
    CEdit m_edtYpos;
    CEdit m_edtWidth;
    CEdit m_edtHeight;
    CFindWindow m_StaticIcon;
    UINT m_nHwnd;
    afx_msg void OnBnClickedButtonOk();
    afx_msg void OnBnClickedButtonCancel();
    afx_msg void OnUpdateHwnd();
private:
    lc_display_info_t* m_pInfo;
public:
    afx_msg void OnCbnSelchangeComboType();
    CEdit m_edtHwnd;
};
