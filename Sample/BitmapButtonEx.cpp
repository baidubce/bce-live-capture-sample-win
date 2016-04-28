#include "stdafx.h"
#include "DuBitmap.h"
#include "BitmapButtonEx.h"


IMPLEMENT_DYNAMIC(CBitmapButtonEx, CBitmapButton);


CBitmapButtonEx::CBitmapButtonEx(CString strTip, BOOL bFocusRect,
                                 int prefredWidth /*=50*/, int minWidth/* = 35*/)
    : m_strTip(strTip)
    , m_bClicked(FALSE)
    , m_bFocusRect(bFocusRect)
    , m_nStatus(SBS_NORMAL)
    , m_bTrace(FALSE)
    , m_IDBmp(0)
    , m_nExpandLeft(0)
    , m_nExpandRight(0) {
    m_szPrefredSize.cx = prefredWidth;
    m_szPrefredSize.cy = 0;

    m_szMinSize.cx = minWidth;
    m_szMinSize.cy = 0;
    m_szBmp.cx = 0;
    m_szBmp.cy = 0;
}

CBitmapButtonEx::~CBitmapButtonEx() {
}

void CBitmapButtonEx::SetToolTip(LPCTSTR strTip) {
    m_strTip = strTip;

    if (m_Tt.m_hWnd) {
        m_Tt.UpdateTipText(m_strTip, this);
        m_Tt.Update();
    }
}

void CBitmapButtonEx::SetToolTip(CString& strTip) {
    SetToolTip((LPCTSTR)strTip);
}

BEGIN_MESSAGE_MAP(CBitmapButtonEx, CBitmapButton)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_WM_MOVE()
    ON_WM_MOUSEMOVE()
    ON_WM_CREATE()
    ON_WM_KILLFOCUS()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


BOOL CBitmapButtonEx::LoadBitmaps(UINT nIDBitmap) {
    ASSERT(nIDBitmap);
    m_IDBmp = nIDBitmap;
    UpdateBmp(FALSE);
    return TRUE;
}

void CBitmapButtonEx::OnLButtonDown(UINT nFlags, CPoint point) {
    CBitmapButton::OnLButtonDown(nFlags, point);
    m_bClicked = TRUE;
    m_nStatus = (m_nStatus == SBS_NORMAL || m_nStatus == SBS_HOVER) ? SBS_CLICKED : SBS_NORMAL;
    RedrawWindow();
}

void CBitmapButtonEx::OnLButtonUp(UINT nFlags, CPoint point) {
    m_bClicked = FALSE;
    CRect rcClient(0, 0, 0, 0);
    GetClientRect(&rcClient);

    if (rcClient.PtInRect(point)) {
        m_nStatus = SBS_HOVER;
    } else {
        m_nStatus = SBS_NORMAL;
    }

    RedrawWindow();
    CBitmapButton::OnLButtonUp(nFlags, point);
}

void CBitmapButtonEx::OnMouseMove(UINT nFlags, CPoint point) {
    CBitmapButton::OnMouseMove(nFlags, point);

    if (!m_bTrace) {
        m_bTrace = TRUE;
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 1;
        m_bTrace = _TrackMouseEvent(&tme);
    }

    if (m_nStatus == SBS_NORMAL && m_bClicked == FALSE) {
        m_nStatus = SBS_HOVER;
        RedrawWindow();
    }
}

int CBitmapButtonEx::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (-1 == CBitmapButton::OnCreate(lpCreateStruct)) {
        return -1;
    }

    InitToolTip();
    return 0;
}

void CBitmapButtonEx::PreSubclassWindow() {
    CBitmapButton::PreSubclassWindow();
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();

    if (pThreadState->m_pWndInit == this) {
        return;
    }

    InitToolTip();
}

void CBitmapButtonEx::InitToolTip() {
    ModifyStyle(0, BS_OWNERDRAW);

    m_Tt.AddTool(this, m_strTip);
}

void CBitmapButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDIS) {
    ASSERT(lpDIS != NULL);
    // must have the bitmap loaded before calling DrawItem

    CDuBitmap bmp(m_IDBmp);

    if (!bmp.Valid()) {
        return ;
    }

    BOOL bEnable = TRUE;

    if (lpDIS->itemState & ODS_DISABLED) {
        bEnable = FALSE;
    }

    CRect  rcSour(0, 0, 0, 0);

    if (!GetImageRect(&rcSour, bEnable ? m_nStatus : SBS_DISABLED)) {
        return ;
    }

    // draw the whole button
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);

    CDC destDC, srcDC;
    CBitmap destBmp;

    if (!(destDC.CreateCompatibleDC(pDC) && srcDC.CreateCompatibleDC(pDC))) {
        return;
    }

    CRect wrect(0, 0, 0, 0);
    GetClientRect(wrect);

    if (!destBmp.CreateCompatibleBitmap(pDC, wrect.Width() + m_nExpandLeft + m_nExpandRight,
                                        wrect.Height())) {
        return ;
    }

    CBitmap* pOldDest = destDC.SelectObject(&destBmp);
    CBitmap* pOldsrc = srcDC.SelectObject(bmp);

    DrawBackground(destDC, CRect(0, 0, wrect.Width() + m_nExpandLeft + m_nExpandRight, wrect.Height()),
                   bEnable ? m_nStatus : SBS_DISABLED);

    destDC.SetStretchBltMode(COLORONCOLOR);

    int left = m_nExpandLeft + (wrect.Width() - rcSour.Width()) / 2;
    int top = (wrect.Height() - rcSour.Height()) / 2;

    TransparentBlt(destDC, left, top, m_szBmp.cx, m_szBmp.cy, srcDC, rcSour.left, rcSour.top,
                   m_szBmp.cx, m_szBmp.cy, RGB(255, 0, 255));

    pDC->BitBlt(0, 0, wrect.Width(), wrect.Height(), &destDC, m_nExpandLeft, 0, SRCCOPY);
    destDC.SelectObject(pOldDest);
    srcDC.SelectObject(pOldsrc);
    destDC.DeleteDC();
    srcDC.DeleteDC();
    destBmp.DeleteObject();

    if (((lpDIS->itemState & ODS_FOCUS) != 0) && (m_bFocusRect == TRUE)) {
        wrect.DeflateRect(1, 1);
        pDC->DrawFocusRect(&wrect);
    }
}

void CBitmapButtonEx::DrawBackground(CDC& memDC, LPRECT lprc, int iState) {

}

BOOL CBitmapButtonEx::OnEraseBkgnd(CDC* pDC) {
    return FALSE;
}

void CBitmapButtonEx::SizeToContent() {
    CRect wrect;
    GetWindowRect(wrect);
    CWnd* pParent = GetParent();
    ASSERT(pParent != NULL);

    if (pParent != NULL) {
        pParent->ScreenToClient(wrect);
        ASSERT(m_szBmp.cx != 0 && m_szBmp.cy != 0);
        int x = int(float(wrect.left + wrect.right - m_szBmp.cx) / 2 + 0.5);
        int y = int(float(wrect.top + wrect.bottom - m_szBmp.cy) / 2 + 0.5);

        VERIFY(SetWindowPos(NULL, x, y, m_szBmp.cx, m_szBmp.cy, SWP_NOZORDER));
    }
}

void CBitmapButtonEx::OnMove(int x, int y) {
    CBitmapButton::OnMove(x, y);
}


BOOL CBitmapButtonEx::GetImageRect(LPRECT prect, int iState) {
    BOOL bRet = FALSE;
    prect->left = prect->top = 0;
    prect->right = m_szBmp.cx;
    prect->bottom = m_szBmp.cy;

    if (iState == SBS_NORMAL) {
        bRet = TRUE;
    } else if (iState == SBS_HOVER) {
        OffsetRect(prect, m_szBmp.cx, 0);
        bRet = TRUE;
    } else if (iState == SBS_CLICKED) {
        OffsetRect(prect, m_szBmp.cx * 2, 0);
        bRet = TRUE;
    } else if (iState == SBS_DISABLED) {
        OffsetRect(prect, m_szBmp.cx * 3, 0);
        bRet = TRUE;
    }

    return bRet;
}

HRESULT CBitmapButtonEx::OnMouseLeave(WPARAM wparam, LPARAM lparam) {
    m_bTrace = FALSE;
    m_nStatus = SBS_NORMAL;
    RedrawWindow();
    return 0;
}

void CBitmapButtonEx::UpdateBmp(BOOL bResize/* = TRUE*/) {
    CDuBitmap bmp(m_IDBmp);
    ASSERT(bmp.Valid());

    if (bmp.Valid()) {
        m_szBmp.cx = bmp.GetSize().cx / 4;
        m_szBmp.cy = bmp.GetSize().cy;
    }

    if (m_hWnd) {
        bResize ? SizeToContent() : (0);
        RedrawWindow();
    }
}

void CBitmapButtonEx::OnKillFocus(CWnd* pNewWnd) {
    CBitmapButton::OnKillFocus(pNewWnd);
    RedrawWindow();
}

BOOL CBitmapButtonEx::GetSize(CSize& size) {
    size = m_szPrefredSize;
    return TRUE;
}

BOOL CBitmapButtonEx::GetMinSize(CSize& size) {
    size = m_szMinSize;
    return TRUE;
}