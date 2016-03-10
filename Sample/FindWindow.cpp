// FindWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "FindWindow.h"


// CFindWindow

IMPLEMENT_DYNAMIC(CFindWindow, CStatic)

CFindWindow::CFindWindow()
    : m_bBtnDown(0)
    , m_hIconSel(0)
    , m_hIconEmp(0)
    , m_hIconDis(0)
    , m_hCursor(0)
    , m_hFindWnd(0) {

}

CFindWindow::~CFindWindow() {
    DestroyIcon(m_hIconEmp);
    DestroyIcon(m_hIconSel);
    DestroyIcon(m_hIconDis);
    DestroyCursor(m_hCursor);
}


BEGIN_MESSAGE_MAP(CFindWindow, CStatic)
    ON_WM_LBUTTONDOWN()
    ON_WM_CAPTURECHANGED()
    ON_WM_LBUTTONUP()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CFindWindow::PreSubclassWindow() {
    CStatic::PreSubclassWindow();
    OnInit();
}

void CFindWindow::OnInit() {
    m_hIconSel = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SEL));
    m_hIconEmp = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_EMP));
    m_hIconDis = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_DIS));
    SetIcon(m_hIconSel);

    m_hCursor = LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSOR_SEL));
    m_wndRect.CreateEx(WS_EX_TOPMOST, 
        AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,0,(HBRUSH) ::GetStockObject(BLACK_BRUSH)),
        "rect", WS_POPUP, CRect(), this, 0);
}

// CFindWindow message handlers


void CFindWindow::OnLButtonDown(UINT nFlags, CPoint point) {
    SetIcon(m_hIconEmp);
    Invalidate();
    SetCapture();
    m_bBtnDown = TRUE;
    ::SetCursor(m_hCursor);
}

void CFindWindow::OnLButtonUp(UINT nFlags, CPoint point) {
    SetIcon(m_hIconSel);
    Invalidate();
    ReleaseCapture();
    m_wndRect.Hide();
    m_bBtnDown = FALSE;
}

void CFindWindow::OnCaptureChanged(CWnd* pWnd) {
    if (m_bBtnDown) {
        ReleaseCapture();
        SetIcon(m_hIconSel);
        m_wndRect.Hide();
        m_bBtnDown = FALSE;
    }
}

BOOL CFindWindow::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
    if (m_bBtnDown) {
        ::SetCursor(m_hCursor);
    }

    return CStatic::OnSetCursor(pWnd, nHitTest, message);
}


void CFindWindow::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_bBtnDown) {
        HWND hwnd = DoFindWindow(point);

        if (hwnd && m_hFindWnd != hwnd) {

            m_hFindWnd = hwnd;
            Hilight(m_hFindWnd);
            ::SendMessage(GetParent()->GetSafeHwnd(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CN_COMMAND),
                          (LPARAM)GetSafeHwnd());
        }
    }
}

HWND CFindWindow::GetFindWindow() {
    return m_hFindWnd;
}

HWND CFindWindow::DoFindWindow(CPoint& point) {

    ClientToScreen(&point);
    HWND hwnd = ::WindowFromPoint(point);

    DWORD pid =0;
    ::GetWindowThreadProcessId(hwnd, &pid);
    if( pid != GetCurrentProcessId())
        return hwnd;
    else 
        return NULL;
}

void CFindWindow::Hilight(HWND hwnd) {
    CRect rc;
    if(::GetWindowRect(hwnd, rc)) {
        m_wndRect.Hilight(rc);
    } else {
        m_wndRect.Hide();
    }
}

void CFindWindow::SetEnable(BOOL bEnable) {
    EnableWindow(bEnable);
    SetIcon(bEnable ? m_hIconSel : m_hIconDis);
}