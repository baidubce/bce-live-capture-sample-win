#include "stdafx.h"
#include "Sample.h"
#include "ToolsDlg.h"

CToolsApp::CToolsApp() {
}

CToolsApp theApp;

BOOL CToolsApp::InitInstance() {
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);
    CWinAppEx::InitInstance();
    AfxEnableControlContainer();
    AfxInitRichEdit2();
    CToolsDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    lc_deinit();
    return FALSE;
}
