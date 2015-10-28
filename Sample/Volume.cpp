#include "stdafx.h"
#include "common.h"
#include "Volume.h"
#include "DuBitmap.h"
#include <afxwin.h>


IMPLEMENT_DYNAMIC(CVolumeEx, CWnd);

CVolumeEx::CVolumeEx(UINT nIDSwitchOn, UINT nIDSwitchOff, UINT nIDPassedBmp,
                     UINT nIDBackBmp, UINT nIDSliderRes)
    : m_Slider(nIDPassedBmp, nIDBackBmp, nIDSliderRes)
    , m_bSwitch("关闭", FALSE, 20, 20)
    , m_bOn(TRUE) {
    m_nIDSwitchOn = nIDSwitchOn;
    m_nIDSwitchOff = nIDSwitchOff;

    CDuBitmap bmpThumb(nIDSliderRes);
    CDuBitmap bmpBack(nIDBackBmp);

    m_SliderSize.cx = bmpThumb.GetSize().cx / 3;
    m_SliderSize.cy = bmpBack.GetSize().cy + bmpThumb.GetSize().cy;

    m_BtnSize.cx = 20;
    m_BtnSize.cy = 20;
}

CVolumeEx::~CVolumeEx() {
}


BEGIN_MESSAGE_MAP(CVolumeEx, CWnd)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_BN_CLICKED(ID_SWITCH, OnSwitch)
    ON_CONTROL(BN_MOUSEUP, ID_SLIDER, OnSlider)
    ON_CONTROL(BN_DRAG, ID_SLIDER, OnSlider)
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


int CVolumeEx::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (-1 == CWnd::OnCreate(lpCreateStruct)) {
        return -1;
    }

    ASSERT(m_hWnd != NULL);

    if (!m_bSwitch.Create("开启/关闭", WS_CHILD | WS_VISIBLE , CRect(0, 0, 0, 0), this, ID_SWITCH)) {
        return -1;
    }

    m_bSwitch.LoadBitmaps(m_nIDSwitchOn);

    if (!m_Slider.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_SLIDER)) {
        return -1;
    }

    m_Slider.SetRange(0, 100);
    m_Slider.SetPos(80);
    return 0;
}

void CVolumeEx::OnDestroy() {
    if (m_bSwitch.m_hWnd) {
        m_bSwitch.DestroyWindow();
    }

    if (m_Slider.m_hWnd) {
        m_Slider.DestroyWindow();
    }
}


void CVolumeEx::OnSize(UINT nType, int cx, int cy) {
    CWnd::OnSize(nType, cx, cy);

    CSize SwitchSize;
    m_bSwitch.GetSize(SwitchSize);
    int nHei = cy;
    int nWid = cx;

    int nSwitchHei = min(nHei, SwitchSize.cx);
    int nSwitchWid = min(nWid, SwitchSize.cx);

    int nSliderWid = min(nWid, m_SliderSize.cx);
    int nSliderHei = max(0, nHei - nSwitchHei);

    if (m_Slider.m_hWnd) {
        m_Slider.SetWindowPos(NULL, max(0, (nWid - nSliderWid) / 2) , 0, nSliderWid , nSliderHei,
                              SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    }

    if (m_bSwitch.m_hWnd) {
        m_bSwitch.SetWindowPos(NULL, max(0, (nWid - nSwitchWid) / 2), nSliderHei,  nSwitchWid, nSwitchHei,
                               SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    }
}

void CVolumeEx::OnPaint() {
    CRect rect;
    GetClientRect(rect);
    CPaintDC dc(this); // device context for painting
    dc.FillSolidRect(rect, 0);
}

BOOL CVolumeEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
    if (nFlags & (MK_SHIFT | MK_CONTROL)) {
        return FALSE;
    }

    return DoMouseWheel(nFlags, zDelta, pt);

}

BOOL CVolumeEx::DoMouseWheel(UINT fFlags, short zDelta, CPoint point) {
    UNUSED_ALWAYS(point);
    UNUSED_ALWAYS(fFlags);

    return FALSE;
}
void CVolumeEx::OnSwitch() {
    if (m_bOn) {
        m_bSwitch.LoadBitmaps(m_nIDSwitchOff);
        m_bSwitch.SetToolTip("开启");
        m_bOn = FALSE;
    } else {
        m_bSwitch.LoadBitmaps(m_nIDSwitchOn);
        m_bSwitch.SetToolTip("关闭");
        m_bOn = TRUE;
    }

    int db = (int)m_Slider.GetCurrentPos(CProgressCtrlEx::GP_RELATIVE);
    db = db > 100 ? 100 : db;
    db = db < 0 ? 0 : db;
    CWnd* pParentWnd = GetParent();

    if (pParentWnd) {
        pParentWnd->SendMessage(WM_VOLUME, m_bOn, db);
    }
}

void CVolumeEx::OnSlider() {
    int db = (int)m_Slider.GetCurrentPos(CProgressCtrlEx::GP_RELATIVE);
    db = db > 100 ? 100 : db;
    db = db <= 0 ? 0 : db;
    CWnd* pParentWnd = GetParent();

    if (pParentWnd) {
        pParentWnd->SendMessage(WM_VOLUME, m_bOn, db);
    }

    char strDB[5] = {0};
    _ltoa_s(db, strDB, 5, 10);
    m_Slider.SetToolTip(strDB);
}

BOOL CVolumeEx::GetVolume(BOOL& bOn, int& nVolume) {
    int db = (int)m_Slider.GetCurrentPos(CProgressCtrlEx::GP_RELATIVE);
    db = db > 100 ? 100 : db;
    db = db < 0 ? 0 : db;
    bOn = m_bOn;
    nVolume = db;
    return TRUE;
}
void CVolumeEx::SetVolume(BOOL bOn, int nVolume) {
    m_Slider.SetPos(nVolume, TRUE);

    if (m_bOn == bOn) {
        OnSlider();
    } else {
        OnSwitch();
    }
}

BOOL CVolumeEx::GetSize(CSize& size) {
    m_bSwitch.GetSize(size);
    size.cy += m_SliderSize.cy + 4;
    size.cx = max(size.cx, m_SliderSize.cx) + 4;
    return FALSE;
}


CVolumeProgress::CVolumeProgress(UINT nIDPassedBmp, UINT nIDBackBmp,
                                 UINT nIDSliderRes)
    : CProgressCtrlEx(nIDPassedBmp, nIDBackBmp, nIDSliderRes) {
    m_bVertical = TRUE;
    m_bForceShowSlider = TRUE;
}
CVolumeProgress::~CVolumeProgress() {
}

int CVolumeProgress::SetPos(int nPos, BOOL bSelfCall) {
    if (nPos <= m_nLower) {
        nPos = m_nLower + 1;
    }

    return CProgressCtrlEx::SetPos(nPos, bSelfCall);
}


void CVolumeProgress::DrawSlider(CDC& memDC, LPRECT lprcDest, LPRECT lprcSour) {
    CProgressCtrlEx::DrawSlider(memDC, lprcDest, lprcSour);
}

void CVolumeProgress::DrawBackground(CDC& memDC, LPRECT lprcDraw) {
    CDC bkdc;

    if (!bkdc.CreateCompatibleDC(&memDC)) {
        return;
    }

    memDC.FillSolidRect(0, 0, lprcDraw->right - lprcDraw->left, lprcDraw->bottom - lprcDraw->top,
                        0);

    BITMAP bmpobj;
    m_bmpBack.GetBitmap(&bmpobj);
    CBitmap* pOldbmp = bkdc.SelectObject(&m_bmpBack);

    memDC.TransparentBlt(
        max(0, (lprcDraw->right - lprcDraw->left - bmpobj.bmWidth) / 2), //dstX
        0,                                                          //dstY
        min(bmpobj.bmWidth, lprcDraw->right - lprcDraw->left),          //dstWidth
        lprcDraw->bottom - lprcDraw->top,                           //dstHeight
        &bkdc,                                                      //srcDC
        0, 0, bmpobj.bmWidth, bmpobj.bmHeight, RGB(255, 0, 255));

    bkdc.SelectObject(pOldbmp);
    bkdc.DeleteDC();
}

void CVolumeProgress::DrawFore(CDC& memDC, LPRECT lprcDraw) {

    CDC bkdc;

    if (!bkdc.CreateCompatibleDC(&memDC)) {
        return;
    }

    BITMAP bmpobj;
    m_bmpPassed.GetBitmap(&bmpobj);
    CBitmap* pOldbmp = bkdc.SelectObject(&m_bmpPassed);

    CRect rcClient;
    GetClientRect(&rcClient);
    int nHeight = int((float(bmpobj.bmHeight) / rcClient.Height()) * (lprcDraw->bottom -
                      lprcDraw->top));

    memDC.TransparentBlt(
        max(0, (lprcDraw->right - lprcDraw->left - bmpobj.bmWidth) / 2),
        lprcDraw->top,
        min(bmpobj.bmWidth, lprcDraw->right - lprcDraw->left),
        lprcDraw->bottom - lprcDraw->top,
        &bkdc,
        0,
        bmpobj.bmHeight - nHeight,
        bmpobj.bmWidth,
        nHeight,
        RGB(255, 0, 255));

    bkdc.SelectObject(pOldbmp);
    bkdc.DeleteDC();
}
BOOL CVolumeProgress::FormatToolTip(CString& strTip, int nPos) {
    return FALSE;
}
void CVolumeEx::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp) {
    ((CRect*)(lpncsp))->DeflateRect(2, 2, 2, 2);
}

void CVolumeEx::OnNcPaint() {
    CRect rcWin(0, 0, 0, 0);
    GetWindowRect(rcWin);
    rcWin.OffsetRect(-rcWin.left, -rcWin.top);

    CWindowDC dc(this);

    dc.Draw3dRect(rcWin, RGB(28, 28, 28), RGB(57, 57, 57));
    rcWin.DeflateRect(1, 1, 1, 1);
    dc.Draw3dRect(rcWin, RGB(0, 0, 0), RGB(0, 0, 0));
}

BOOL CVolumeEx::OnEraseBkgnd(CDC* pDC) {
    return FALSE;
}
