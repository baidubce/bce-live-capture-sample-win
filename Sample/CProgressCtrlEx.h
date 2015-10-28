#if !defined(_DU_PROGRESSCTRLEX_H_)
#define _DU_PROGRESSCTRLEX_H_

#include "BitmapButtonEx.h"

#define  BN_MOUSEDOWN   0x7fffd
#define  BN_MOUSEUP     0x7fffe
#define  BN_DRAG        0x7ffff

class CProgressCtrlEx : public CProgressCtrl {
public:
    enum {GP_ABSOLUTE = 0 , GP_RELATIVE , GP_PERCENTAGE};
    BOOL    m_bVertical;
    BOOL    m_bForceShowSlider;

    enum {
        WM_BEGINDRAG = WM_APP + 'PR0',
        WM_AFTERDRAG,
    };
public:
    CProgressCtrlEx(UINT nIDPassedBmp, UINT nIDBackBmp, UINT nIDSliderRes);
    virtual int SetPos(int nPos, BOOL bSelfCall = FALSE);
    int     SetStep(int nStep);
    void    SetRange(int nLower, int nUpper);
    void    SetToolTip(CString strTip);

    double  GetCurrentPos(UINT uType);

protected:
    CToolTipCtrl& GetToolTipCtrl() {
        return gGetToolTipCtrl();
    }
    __declspec(property(get = GetToolTipCtrl))
    CToolTipCtrl & m_Tt;
protected:
    int         m_nLower, m_nUpper, m_nStep, m_nCurrentPosition;
    BOOL        m_bTrace;
    BOOL        m_bLBtnDown;

    CString     m_strTip;
    CBitmap     m_bmpPassed, m_bmpBack;
    CBitmap     m_bmpSliderRes;
    SIZE        m_nSliderSize;
    LONGLONG    m_llLastTime;

public:
    virtual void PreSubclassWindow();
    virtual BOOL FormatToolTip(CString& strTip, int nPos);
private:
    BOOL InitCtrl();
public:
    virtual ~CProgressCtrlEx();

protected:
    virtual void DrawSlider(CDC& memDC, LPRECT lprcDest, LPRECT lprcSour);
    virtual void DrawBackground(CDC& memDC, LPRECT lprcDraw);
    virtual void DrawFore(CDC& memDC, LPRECT lprcDraw);

    BOOL GetSliderRect(LPRECT lpRect, int nState);
    BOOL GetSliderImageRect(LPRECT lpRect, int nState);
    BOOL IsBusy();
protected:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
public:
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnCaptureChanged(CWnd* pWnd);
};

#endif // !defined(_DU_PROGRESSCTRLEX_H_)
