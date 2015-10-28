#ifndef __COMMON_H_
#define __COMMON_H_
#pragma comment(lib,"Msimg32")

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

//#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>          // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>         // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>
#include <afxmt.h>

class CToolTipCtrlEx : public CToolTipCtrl {
    DECLARE_DYNAMIC(CToolTipCtrlEx);
public:
    CToolTipCtrlEx();
    ~CToolTipCtrlEx();

public:
    static CToolTipCtrlEx* __stdcall Create();
public:
    void EnsureDestroy();
    void OnDestroy();
    DECLARE_MESSAGE_MAP();
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
    afx_msg void OnPaint();
    afx_msg LRESULT OnRelayEvent(WPARAM wParam, LPARAM lParam);
private:
    CFont m_Font;
};

#define LEFT_WIDTH              (30l)
#define RIGHT_WIDTH             LEFT_WIDTH
#define EDIT_TEXT_COLOR         (RGB(40,252,252))


void                CopyBmpToDC(CDC* pMemDC, CBitmap* pBmp, int nWidth, int nHeight,
                                BOOL bStretch = FALSE,
                                int left = LEFT_WIDTH, int right = RIGHT_WIDTH, BOOL Transparent = TRUE, BOOL Tile = FALSE);
CToolTipCtrl&       gGetToolTipCtrl();
CString             gGetChildWindowClassName();



#endif //__COMMON_H_



