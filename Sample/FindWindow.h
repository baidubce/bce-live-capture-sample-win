#pragma once

#include "RectangleWindow.h"

// CFindWindow

class CFindWindow : public CStatic
{
	DECLARE_DYNAMIC(CFindWindow)

public:
	CFindWindow();

	virtual ~CFindWindow();
private:
    void OnInit();
    HWND DoFindWindow(CPoint& point);
    void Hilight(HWND hwnd);
    
private:
    HICON m_hIconSel;
    HICON m_hIconEmp;
    HICON m_hIconDis;
    BOOL m_bBtnDown;
    HCURSOR m_hCursor;
    HWND m_hFindWnd;
    CRectangleWindow m_wndRect;
protected:
    virtual void PreSubclassWindow();

public:
    HWND GetFindWindow();

    void SetEnable(BOOL bEnable);
protected:
	DECLARE_MESSAGE_MAP()

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnCaptureChanged(CWnd* pWnd);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


