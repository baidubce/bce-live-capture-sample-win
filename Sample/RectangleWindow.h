#pragma once


// CRectangleWindow

class CRectangleWindow : public CWnd
{
	DECLARE_DYNAMIC(CRectangleWindow)

public:
	CRectangleWindow();
	virtual ~CRectangleWindow();

protected:
	DECLARE_MESSAGE_MAP()

    afx_msg void OnSize(UINT nType, int cx, int cy);

public:
    void Hilight(LPRECT rect);
    void Hide();
};


