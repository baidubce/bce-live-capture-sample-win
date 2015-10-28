#include "stdafx.h"
#include "common.h"
#include "CProgressCtrlEx.h"

CProgressCtrlEx::CProgressCtrlEx(UINT nIDPassedBmp, UINT nIDBackBmp, UINT nIDSliderRes)
    : m_nLower(0)
    , m_nUpper(100)
    , m_nCurrentPosition(0)
    , m_nStep(1)
    , m_bTrace(FALSE)
    , m_bLBtnDown(FALSE)
    , m_llLastTime(0)
    , m_bVertical(FALSE)
    , m_bForceShowSlider(FALSE) {
    m_bmpPassed.LoadBitmap(nIDPassedBmp);
    m_bmpBack.LoadBitmap(nIDBackBmp);
    m_bmpSliderRes.LoadBitmap(nIDSliderRes);
    BITMAP l_bmpObj = {0};
    m_bmpSliderRes.GetBitmap(&l_bmpObj);
    m_nSliderSize.cx = l_bmpObj.bmWidth / 3;
    m_nSliderSize.cy = l_bmpObj.bmHeight;
}

CProgressCtrlEx::~CProgressCtrlEx() {
}

BEGIN_MESSAGE_MAP(CProgressCtrlEx, CProgressCtrl)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_SETCURSOR()
    ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

void CProgressCtrlEx::SetRange(int nLower, int nUpper) {
    //This Function is to Set Range of the progress
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_nCurrentPosition = nLower;

    CProgressCtrl::SetRange32(nLower, nUpper);
}

int CProgressCtrlEx::SetStep(int nStep) {
    m_nStep = nStep;
    return (CProgressCtrl::SetStep(nStep));
}

BOOL CProgressCtrlEx::OnEraseBkgnd(CDC* pDC) {
    return FALSE;//CProgressCtrl::OnEraseBkgnd(pDC);
}

BOOL CProgressCtrlEx::IsBusy() {
    return FALSE;
}

void CProgressCtrlEx::OnPaint() {
    CPaintDC dc(this); // device context for painting

    CDC memDC;

    if (! memDC.CreateCompatibleDC(&dc)) {
        return;
    }

    CRect rectClient(0, 0, 0, 0);
    GetClientRect(rectClient);
    CBitmap memBmp;

    if (!memBmp.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height())) {
        memDC.DeleteDC();
        return ;
    }

    CBitmap* pOldmemBmp = memDC.SelectObject(&memBmp);

    // »­±³¾°
    CRect rect(rectClient);
    DrawBackground(memDC, &rect);

    // »­Ç°¾°
    CRect rcFore(rectClient);

    if (m_bVertical) {
        float maxWidth = (float)(m_nCurrentPosition - m_nLower) * ((float)rectClient.Height() /
                         (m_nUpper - m_nLower));
        rcFore.top = rcFore.bottom - int(maxWidth);
    } else {
        float maxWidth = (float)(m_nCurrentPosition - m_nLower) * ((float)rectClient.Width() /
                         (m_nUpper - m_nLower));
        rcFore.right = rcFore.left + int(maxWidth);
    }

    DrawFore(memDC, &rcFore);


    // »­Thumb
    int nState = 0;

    if (m_bTrace && !m_bLBtnDown) {
        nState = 1;
    }

    if (m_bTrace && m_bLBtnDown) {
        nState = 2;
    }

    CRect rcDest(0, 0, 0, 0);
    CRect rcSour(0, 0, 0, 0);

    if (GetSliderRect(&rcDest, nState) && GetSliderImageRect(&rcSour, nState)) {
        DrawSlider(memDC, &rcDest, &rcSour);
    }

    // ¸´ÖÆµ½WindowµÄDC
    dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &memDC, 0, 0, SRCCOPY);

    memDC.SelectObject(pOldmemBmp);
    memDC.DeleteDC();
    memBmp.DeleteObject();
    // Do not call CProgressCtrl::OnPaint() for painting messages
}
void CProgressCtrlEx::DrawBackground(CDC& memDC, LPRECT lprcDraw) {
    memDC.FillSolidRect(lprcDraw, RGB(0, 0, 0));
    CopyBmpToDC(&memDC, &m_bmpBack, lprcDraw->right - lprcDraw->left, lprcDraw->bottom - lprcDraw->top,
                FALSE, 5, 5);
}

void CProgressCtrlEx::DrawFore(CDC& memDC, LPRECT lprcDraw) {
    CopyBmpToDC(&memDC, &m_bmpPassed, lprcDraw->right - lprcDraw->left,
                lprcDraw->bottom - lprcDraw->top, FALSE, 5, 5);
}

void CProgressCtrlEx::DrawSlider(CDC& memDC, LPRECT lprcDest, LPRECT lprcSour) {
    CDC sliderDC;

    if (!sliderDC.CreateCompatibleDC(&memDC)) {
        return;
    }

    CBitmap* pOldSliderbmp = sliderDC.SelectObject(&m_bmpSliderRes);

    TransparentBlt(memDC, lprcDest->left, lprcDest->top, lprcDest->right - lprcDest->left,
                   lprcDest->bottom - lprcDest->top,
                   sliderDC, lprcSour->left, lprcSour->top, lprcSour->right - lprcSour->left,
                   lprcSour->bottom - lprcSour->top, RGB(255, 0, 255));

    sliderDC.SelectObject(pOldSliderbmp);
    sliderDC.DeleteDC();
}

BOOL CProgressCtrlEx::GetSliderRect(LPRECT lpRect, int nState) {
    CRect rectClient(0, 0, 0, 0);
    GetClientRect(&rectClient);

    if (rectClient.Height() <= 2) {
        return FALSE;
    }

    if (!m_bForceShowSlider && (m_nCurrentPosition == m_nLower && nState == 0)) {
        return FALSE;
    }

    if (m_bVertical) {
        lpRect->left = 0;
        lpRect->right = rectClient.Width();

        int nSliderCenter =  rectClient.Height() - m_nSliderSize.cy / 2 - int((float)(
                                 m_nCurrentPosition - m_nLower) * ((float)(rectClient.Height() - m_nSliderSize.cy) /
                                         (m_nUpper - m_nLower)) + 0.5);
        int nOffset = (m_nSliderSize.cy) / 2;

        if (nOffset <= 0) {
            return FALSE;
        }

        lpRect->top = nSliderCenter - nOffset;
        lpRect->bottom = nSliderCenter + nOffset + 1;

        if (lpRect->top < 0) {
            OffsetRect(lpRect, 0, -lpRect->top);
        }

        if (lpRect->bottom > rectClient.bottom) {
            OffsetRect(lpRect, 0, rectClient.bottom - lpRect->bottom);
        }

        if (lpRect->top < 0) {
            return FALSE;
        }
    } else {
        lpRect->top = 0;
        lpRect->bottom = rectClient.Height();
        int nSliderCenter = int((float)(m_nCurrentPosition - m_nLower) * ((float)rectClient.Width() /
                                (m_nUpper - m_nLower)) + 0.5);
        int nOffset = (m_nSliderSize.cx) / 2;

        if (nOffset <= 0) {
            return FALSE;
        }

        lpRect->left = nSliderCenter - nOffset;
        lpRect->right = nSliderCenter + nOffset + 1;

        if (lpRect->left < 0) {
            OffsetRect(lpRect, -lpRect->left, 0);
        }

        if (lpRect->right > rectClient.right) {
            OffsetRect(lpRect, rectClient.right - lpRect->right, 0);
        }

        if (lpRect->left < 0) {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CProgressCtrlEx::GetSliderImageRect(LPRECT lpRect, int nState) {
    if (m_nSliderSize.cx <= 0 || m_nSliderSize.cy <= 0) {
        return FALSE;
    }

    lpRect->left = 0;
    lpRect->top = 0;
    lpRect->right = m_nSliderSize.cx;
    lpRect->bottom = m_nSliderSize.cy;
    BOOL l_bRet = FALSE;

    if (nState == 0) {
        l_bRet = TRUE;
    } else if (nState == 1) {
        l_bRet = TRUE;
        OffsetRect(lpRect, m_nSliderSize.cx, 0);
    } else if (nState == 2) {
        l_bRet = TRUE;
        OffsetRect(lpRect, m_nSliderSize.cx * 2, 0);
    } else {
        l_bRet = FALSE;
    }

    return l_bRet;
}

int CProgressCtrlEx::SetPos(int nPos, BOOL bSelfCall /*=FALSE*/) {
    //Set the Position of the Progress
    if (bSelfCall || !m_bLBtnDown) {
        CRect rect;
        RECT btnrect;
        GetClientRect(&rect);
        btnrect.left = 0;
        btnrect.top = 0;
        btnrect.bottom = rect.Height();
        btnrect.right = (btnrect.bottom * 2);

        if (nPos > m_nUpper) {
            m_nCurrentPosition = m_nUpper;
        } else if (nPos < m_nLower) {
            m_nCurrentPosition = m_nLower;
        } else {
            m_nCurrentPosition = nPos;
        }

        int nRet = (CProgressCtrl::SetPos(m_nCurrentPosition));

        RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_FRAME |
                     RDW_ERASE);
        return nRet;
    }

    return 0;
}


double  CProgressCtrlEx::GetCurrentPos(UINT uType) {
    switch (uType) {
    case GP_ABSOLUTE:
        return m_nCurrentPosition;

    case GP_RELATIVE:
        return (m_nCurrentPosition - m_nLower);

    case GP_PERCENTAGE:
        return (double)(m_nCurrentPosition - m_nLower) / (m_nUpper - m_nLower);

    default:
        return UINT(-1);
    }
}

void CProgressCtrlEx::OnMouseMove(UINT nFlags, CPoint point) {
    // TODO: Add your message handler code here and/or call default
    if (!m_bTrace) {
        m_bTrace = TRUE;
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        tme.dwHoverTime = 1;
        m_bTrace = _TrackMouseEvent(&tme);
        RedrawWindow();
    }

    CRect rect;
    GetClientRect(&rect);

    float perpos = 0.0f;
    int pos = 0;

    if (m_bVertical) {
        perpos = (float)rect.Height() / (m_nUpper - m_nLower);
        pos = (int)((float)(rect.Height() - point.y) / perpos) + m_nLower;
    } else {
        perpos = (float)rect.Width() / (m_nUpper - m_nLower);
        pos = (int)((float)point.x / perpos) + m_nLower;
    }

    pos = pos > m_nUpper ? m_nUpper : pos;
    pos = pos < m_nLower ? m_nLower : pos;


    CString strTip = "";

    if (FormatToolTip(strTip, pos)) {
        SetToolTip(strTip);
    }

    if (m_bLBtnDown) {
        SetPos(pos, TRUE);
        CWnd* pWndParent = GetParent();

        if (pWndParent != NULL) {
            ::SendMessage(pWndParent->GetSafeHwnd(), WM_COMMAND, (WPARAM)(MAKELONG(GetDlgCtrlID(), BN_DRAG)),
                          (LPARAM)(this->GetSafeHwnd()));
        }
    }

    CProgressCtrl::OnMouseMove(nFlags, point);
}

BOOL CProgressCtrlEx::FormatToolTip(CString& strTip, int nPos) {
    return FALSE;
}

void CProgressCtrlEx::SetToolTip(CString strTip) {
    m_strTip = strTip;

    if (m_Tt.m_hWnd != NULL) {
        m_Tt.UpdateTipText(m_strTip, this);
        m_Tt.Update();
    }
}

void CProgressCtrlEx::OnLButtonDown(UINT nFlags, CPoint point) {
    CProgressCtrl::OnLButtonDown(nFlags, point);

    m_bLBtnDown = TRUE;
    SetCapture();

    RedrawWindow();
    CRect rect;
    GetClientRect(&rect);

    float perpos = 0.0f;
    int pos = 0;

    if (m_bVertical) {
        perpos = (float)rect.Height() / (m_nUpper - m_nLower);
        pos = (int)((float)(rect.Height() - point.y) / perpos) + m_nLower;
    } else {
        perpos = (float)rect.Width() / (m_nUpper - m_nLower);
        pos = (int)((float)point.x / perpos) + m_nLower;
    }

    pos = pos > m_nUpper ? m_nUpper : pos;
    pos = pos < m_nLower ? m_nLower : pos;
    SetPos(pos, TRUE);

    CWnd* pWndParent = GetParent();

    if (pWndParent != NULL) {
        ::SendMessage(pWndParent->GetSafeHwnd(), WM_BEGINDRAG, 0, 0);
        ::SendMessage(pWndParent->GetSafeHwnd(), WM_COMMAND, (WPARAM)(MAKELONG(GetDlgCtrlID(),
                      BN_MOUSEDOWN)), (LPARAM)(this->GetSafeHwnd()));
    }
}
void CProgressCtrlEx::OnLButtonUp(UINT nFlags, CPoint point) {
    CProgressCtrl::OnLButtonUp(nFlags, point);

    if (m_bLBtnDown) {
        m_bLBtnDown = FALSE;
        CWnd* pWndParent = GetParent();

        if (pWndParent != NULL) {
            ::SendMessage(pWndParent->GetSafeHwnd(), WM_COMMAND, (WPARAM)(MAKELONG(GetDlgCtrlID(), BN_MOUSEUP)),
                          (LPARAM)(this->GetSafeHwnd()));
            ::SendMessage(pWndParent->GetSafeHwnd(), WM_AFTERDRAG, 0, 0);
        }
    }

    ReleaseCapture();
    RedrawWindow();
}

int CProgressCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (-1 == CProgressCtrl::OnCreate(lpCreateStruct)) {
        return -1;
    }

    InitCtrl();
    return 0;
}

void CProgressCtrlEx::PreSubclassWindow() {
    CProgressCtrl::PreSubclassWindow();
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();

    if (pThreadState->m_pWndInit == this) {
        return;
    }

    InitCtrl();
}

BOOL CProgressCtrlEx::InitCtrl() {
    ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_NOSIZE);
    BOOL pass = TRUE;

    pass = m_Tt.AddTool(this, m_strTip);

    if (!pass) {
        return pass;
    }

    m_nCurrentPosition = 0;
    return pass;
}

void CProgressCtrlEx::OnSize(UINT nType, int cx, int cy) {
    CProgressCtrl::OnSize(nType, cx, cy);
}

HRESULT CProgressCtrlEx::OnMouseLeave(WPARAM wparam, LPARAM lparam) {
    m_bLBtnDown = FALSE;
    m_bTrace = FALSE;
    RedrawWindow();

    return 0;
}

BOOL CProgressCtrlEx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    return CProgressCtrl::OnSetCursor(pWnd, nHitTest, message);
}

void CProgressCtrlEx::OnCaptureChanged(CWnd* pWnd) {
    if (m_bLBtnDown) {
        m_bLBtnDown = FALSE;
        CWnd* pWndParent = GetParent();

        if (pWndParent != NULL) {
            ::SendMessage(pWndParent->GetSafeHwnd(), WM_COMMAND, (WPARAM)(MAKELONG(GetDlgCtrlID(), BN_MOUSEUP)),
                          (LPARAM)(this->GetSafeHwnd()));
            ::SendMessage(pWndParent->GetSafeHwnd(), WM_AFTERDRAG, 0, 0);
        }
    }

    CProgressCtrl::OnCaptureChanged(pWnd);
}
