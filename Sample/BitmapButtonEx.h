#if !defined(_DU_BITMAPBUTTONEX_H_)
#define _DU_BITMAPBUTTONEX_H_

#include "common.h"

class CBitmapButtonEx : public CBitmapButton {
    DECLARE_DYNAMIC(CBitmapButtonEx);
public:
    CBitmapButtonEx(CString Strtip = "", BOOL bFocusRect = TRUE, int prefredWidth = 38,
                    int minWidth = 30);

public:
    enum BUTTONSTATE {
        SBS_NORMAL = 0,
        SBS_HOVER,
        SBS_CLICKED,
        SBS_DISABLED
    } m_nStatus;

public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

public:
    virtual ~CBitmapButtonEx();

public:
    void SetToolTip(LPCSTR strTip);
    void SetToolTip(CString& strTip);
    BOOL LoadBitmaps(UINT nIDBitmap);
    void SizeToContent();
    void UseSmallBmp(BOOL bResize = TRUE);
    void UseLargeBmp(BOOL bResize = TRUE);
    virtual BOOL GetSize(CSize& size);
    virtual BOOL GetMinSize(CSize& size);

    void Expand(int left, int right) {
        (left >= 0) ? (m_nExpandLeft = left) : (0);
        (right >= 0) ? (m_nExpandRight = right) : (0);
    }

private:
    int m_nExpandLeft;
    int m_nExpandRight;

protected:
    CToolTipCtrl& GetToolTipCtrl() {
        return gGetToolTipCtrl();
    }
    __declspec(property(get = GetToolTipCtrl))
    CToolTipCtrl & m_Tt;
    void InitToolTip();
protected:

    UINT        m_IDBmp;

    SIZE        m_szBmp;
    SIZE        m_szPrefredSize;
    SIZE        m_szMinSize;


    CString     m_strTip;
    BOOL        m_bClicked;
    BOOL        m_bFocusRect;
    BOOL        m_bTrace;
protected:
    void PreSubclassWindow();

protected:
    BOOL GetImageRect(LPRECT prect, int iState);
    void UpdateBmp(BOOL bResize = TRUE);

    void DrawBackground(CDC& memDC, LPRECT lprc, int iState);
protected:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMove(int x, int y);
    afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnKillFocus(CWnd* pNewWnd);

    DECLARE_MESSAGE_MAP()
};


#endif // !defined(_DU_BITMAPBUTTONEX_H_)
