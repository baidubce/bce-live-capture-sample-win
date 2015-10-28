#include "stdafx.h"
#include "common.h"
#include "resource.h"

void CopyBmpToDC(CDC* pMemDC, CBitmap* pBmp, int nWidth, int nHeight, BOOL bStretch /*= FALSE*/,
                 int left/*=LEFT_WIDTH*/, int right /*= RIGHT_WIDTH*/, BOOL Transparent/*= TRUE*/,
                 BOOL Tile/*=FALSE*/) {
    CDC tDC;

    if (!tDC.CreateCompatibleDC(pMemDC)) {
        return;
    }

    BITMAP l_bmpObj = {0};
    pBmp->GetObject(sizeof(l_bmpObj), &l_bmpObj);
    CBitmap* pOldBmp = tDC.SelectObject(pBmp);
    pMemDC->SetStretchBltMode(COLORONCOLOR);

    if (nWidth < left + right || bStretch) {
        if (Transparent) {
            TransparentBlt(*pMemDC, 0, 0, nWidth, nHeight, tDC, 0, 0, l_bmpObj.bmWidth, l_bmpObj.bmHeight,
                           RGB(0xff, 0, 0xff));
        } else {
            StretchBlt(*pMemDC, 0, 0, nWidth, nHeight, tDC, 0, 0, l_bmpObj.bmWidth, l_bmpObj.bmHeight,
                       SRCCOPY);
        }
    } else {
        if (Transparent) {
            TransparentBlt(*pMemDC, 0, 0, left, nHeight, tDC, 0, 0, left, l_bmpObj.bmHeight, RGB(0xff, 0,
                           0xff));
            TransparentBlt(*pMemDC, nWidth - right, 0, right, nHeight, tDC, l_bmpObj.bmWidth - right, 0, right,
                           l_bmpObj.bmHeight, RGB(0xff, 0, 0xff));
        } else {
            StretchBlt(*pMemDC, 0, 0, left, nHeight, tDC, 0, 0, left, l_bmpObj.bmHeight, SRCCOPY);
            StretchBlt(*pMemDC, nWidth - right, 0, right, nHeight, tDC, l_bmpObj.bmWidth - right, 0, right,
                       l_bmpObj.bmHeight, SRCCOPY);
        }

        if (Tile) {
            CRect rcBmp(left, 0, l_bmpObj.bmWidth - right, l_bmpObj.bmHeight);

            int start = left;
            int len = nWidth - right;

            while (start < len) {
                if (Transparent) {
                    TransparentBlt(*pMemDC, start, 0, min(rcBmp.Width(), len - start), nHeight, tDC, rcBmp.left, 0,
                                   min(rcBmp.Width(), len - start), l_bmpObj.bmHeight, RGB(0xff, 0, 0xff));
                } else {
                    StretchBlt(*pMemDC, start, 0, min(rcBmp.Width(), len - start), nHeight, tDC, rcBmp.left, 0,
                               min(rcBmp.Width(), len - start), l_bmpObj.bmHeight , SRCCOPY);
                }

                start += rcBmp.Width();
            }
        } else {
            if (Transparent) {
                TransparentBlt(*pMemDC, left, 0, nWidth - left - right, nHeight, tDC, left, 0,
                               l_bmpObj.bmWidth - left - right, l_bmpObj.bmHeight, RGB(0xff, 0, 0xff));
            } else {
                StretchBlt(*pMemDC, left, 0, nWidth - left - right, nHeight, tDC, left, 0,
                           l_bmpObj.bmWidth - left - right, l_bmpObj.bmHeight, SRCCOPY);
            }
        }


    }

    tDC.SelectObject(pOldBmp);
    tDC.DeleteDC();
}



CString gGetChildWindowClassName() {
    UINT nClassStyle = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;
    // Returns a temporary string name for the class
    //  Save in a CString if you want to use it for a long time
    LPTSTR lpszName = AfxGetThreadState()->m_szTempClassName;

    // generate a synthetic name for this class
    HINSTANCE hInst = AfxGetInstanceHandle();
    wsprintf(lpszName, _T("DuWindow:Child:%x"), (UINT)hInst);

    // see if the class already exists
    WNDCLASS wndcls;

    if (::GetClassInfo(hInst, lpszName, &wndcls)) {
        // already registered, assert everything is good
        ASSERT(wndcls.style == nClassStyle);

        // NOTE: We have to trust that the hIcon, hbrBackground, and the
        //  hCursor are semantically the same, because sometimes Windows does
        //  some internal translation or copying of those handles before
        //  storing them in the internal WNDCLASS retrieved by GetClassInfo.
        return lpszName;
    }

    // otherwise we need to register a new class
    wndcls.style = nClassStyle;
    wndcls.lpfnWndProc = DefWindowProc;
    wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
    wndcls.hInstance = hInst;
    wndcls.hIcon = 0;
    wndcls.hCursor = NULL;
    wndcls.hbrBackground = NULL;//(HBRUSH)GetStockObject(BLACK_BRUSH);
    wndcls.lpszMenuName = NULL;
    wndcls.lpszClassName = lpszName;

    if (!AfxRegisterClass(&wndcls)) {
        AfxThrowResourceException();
    }

    // return thread-local pointer
    return lpszName;
}

CToolTipCtrl&  gGetToolTipCtrl() {
    AFX_MODULE_THREAD_STATE* pModuleThreadState = AfxGetModuleThreadState();
    CToolTipCtrl* ctrl = pModuleThreadState->m_pToolTip;

    if (!ctrl) {
        pModuleThreadState->m_pToolTip = ctrl = CToolTipCtrlEx::Create();
        ctrl->Create(AfxGetMainWnd(), TTS_ALWAYSTIP | WS_VISIBLE | WS_POPUP);
        ctrl->ModifyStyleEx(0, WS_EX_TOPMOST);
        ctrl->SetDelayTime(-1);
        ctrl->Activate(TRUE);
    }

    return *ctrl;
}

BEGIN_MESSAGE_MAP(CToolTipCtrlEx, CToolTipCtrl)
    ON_WM_DESTROY()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_PAINT()
    ON_MESSAGE(TTM_RELAYEVENT, &CToolTipCtrlEx::OnRelayEvent)
END_MESSAGE_MAP()


IMPLEMENT_DYNAMIC(CToolTipCtrlEx, CToolTipCtrl);

CToolTipCtrlEx::CToolTipCtrlEx() {

}

void CToolTipCtrlEx::OnDestroy() {
    CToolTipCtrl::OnDestroy();
}

CToolTipCtrlEx* __stdcall CToolTipCtrlEx::Create() {
    return new CToolTipCtrlEx();
}

void CToolTipCtrlEx::EnsureDestroy() {
    if (::IsWindow(m_hWnd)) {
        ::DestroyWindow(m_hWnd);
    }

    m_hWnd = 0;
}

CToolTipCtrlEx::~CToolTipCtrlEx() {
    EnsureDestroy();
}
LRESULT CToolTipCtrlEx::OnRelayEvent(WPARAM wParam, LPARAM lParam) {
    return Default();
}

void CToolTipCtrlEx::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp) {
    ((CRect*)(lpncsp))->InflateRect(2, 2, 2, 2);
}

void CToolTipCtrlEx::OnNcPaint() {
    CRect rcWin(0, 0, 0, 0);
    GetWindowRect(rcWin);
    rcWin.OffsetRect(-rcWin.TopLeft());

    CWindowDC dc(this);
    dc.Draw3dRect(rcWin, RGB(0, 0, 0), RGB(0, 0, 0));

    rcWin.DeflateRect(1, 1, 1, 1);
    dc.Draw3dRect(rcWin, RGB(0x7f, 0x7f, 0x7f), RGB(0x5f, 0x5f, 0x5f));
}

BOOL CToolTipCtrlEx::OnEraseBkgnd(CDC* pDC) {
    CRect rcWindows;
    GetClientRect(&rcWindows);
    pDC->FillSolidRect(rcWindows, RGB(0x40, 0x40, 0x40));
    return FALSE;
}

int CToolTipCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    if (CToolTipCtrl::OnCreate(lpCreateStruct) == -1) {
        return -1;
    }

    SetTipBkColor(RGB(0x40, 0x40, 0x40));
    SetTipTextColor(EDIT_TEXT_COLOR);
    SetMargin(CRect(0, 0, 2, 2));
    return 0;
}

void CToolTipCtrlEx::OnWindowPosChanging(WINDOWPOS* lpwndpos) {
    lpwndpos->flags &= ~SWP_NOREDRAW;
    lpwndpos->flags |= SWP_FRAMECHANGED;

    CToolTipCtrl::OnWindowPosChanging(lpwndpos);
}

void CToolTipCtrlEx::OnPaint() {
    CPaintDC dc(this);
    CRect rcWindow;
    GetClientRect(rcWindow);

    CString str;
    GetWindowText(str);

    if (!m_Font.m_hObject) {
        m_Font.CreateFont(
            12,             // nHeight
            6,          // nWidth
            0,                         // nEscapement
            0,                         // nOrientation
            FW_NORMAL,                 // nWeight
            FALSE,                     // bItalic
            FALSE,                     // bUnderline
            0,                         // cStrikeOut
            ANSI_CHARSET,              // nCharSet
            OUT_DEFAULT_PRECIS,        // nOutPrecision
            CLIP_DEFAULT_PRECIS,       // nClipPrecision
            DEFAULT_QUALITY,           // nQuality
            DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
            _T("ËÎÌו"));                 // lpszFacename
    };

    CFont* pOldFount = dc.SelectObject(&m_Font);

    dc.SetTextColor(EDIT_TEXT_COLOR);

    dc.DrawText(str, rcWindow , DT_VCENTER | DT_CENTER | DT_SINGLELINE);

    dc.SelectObject(pOldFount);
}
