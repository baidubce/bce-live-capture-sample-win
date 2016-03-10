// RectangleWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "RectangleWindow.h"


// CRectangleWindow

IMPLEMENT_DYNAMIC(CRectangleWindow, CWnd)

CRectangleWindow::CRectangleWindow() {

}

CRectangleWindow::~CRectangleWindow() {
}


BEGIN_MESSAGE_MAP(CRectangleWindow, CWnd)
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CRectangleWindow message handlers
void CRectangleWindow::Hilight(LPRECT rect) {
    MoveWindow(rect);
    SetWindowPos(&CWnd::wndTopMost, rect->left, rect->top, rect->right - rect->left,
                 rect->bottom - rect->top,
                 SWP_SHOWWINDOW);
}
void CRectangleWindow::Hide() {
    ShowWindow(SW_HIDE);
}

void CRectangleWindow::OnSize(UINT nType, int cx, int cy) {

    if (cx > 5 && cy > 5) {
        HRGN hin = CreateRectRgn(2, 2, cx - 2, cy - 2);
        HRGN hout = CreateRectRgn(0, 0, cx, cy);
        CombineRgn(hout, hout, hin, RGN_DIFF);
        DeleteObject(hin);

        SetWindowRgn(hout, TRUE);
        DeleteObject(hout);
    }

    CWnd::OnSize(nType, cx, cy);
}