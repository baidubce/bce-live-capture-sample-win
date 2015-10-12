#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"

class CToolsApp : public CWinAppEx {
public:
    CToolsApp();

public:
    virtual BOOL InitInstance();
};

extern CToolsApp theApp;