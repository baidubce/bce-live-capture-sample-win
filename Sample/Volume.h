#if !defined(_DU_VOLUME_H_)
#define _DU_VOLUME_H_

#include "CProgressCtrlEx.h"
#include "BitmapButtonEx.h"


class CVolumeProgress : public CProgressCtrlEx {
public:
    CVolumeProgress(UINT nIDPassedBmp, UINT nIDBackBmp, UINT nIDSliderRes);
    ~CVolumeProgress();
protected:
    virtual void DrawSlider(CDC& memDC, LPRECT lprcDest, LPRECT lprcSour);
    virtual void DrawBackground(CDC& memDC, LPRECT lprcDraw);
    virtual void DrawFore(CDC& memDC, LPRECT lprcDraw);
    virtual BOOL FormatToolTip(CString& strTip, int nPos);
public:
    virtual int SetPos(int nPos, BOOL bSelfCall = FALSE);
};

class CVolumeEx : public CWnd {
    DECLARE_DYNAMIC(CVolumeEx);

public:
    enum {WM_VOLUME = WM_APP + 'VOL'};

public:
    CVolumeEx(UINT nIDSwitchOn, UINT nIDSwitchOff, UINT nIDPassedBmp, UINT nIDBackBmp,
              UINT nIDSliderRes);
    virtual ~CVolumeEx();

public:
    CVolumeProgress m_Slider;
    UINT            m_nIDSwitchOn, m_nIDSwitchOff;
    CBitmapButtonEx m_bSwitch;
    SIZE            m_SliderSize;
    SIZE            m_BtnSize;
    BOOL            m_bOn;

public:
    BOOL GetVolume(BOOL& bOn, int&  nVolume);
    void SetVolume(BOOL bOn, int nVolume);

    BOOL GetSize(CSize& size);
private:

#define ID_SWITCH   (100)
#define ID_SLIDER   (101)
#define INTERVAL    (0)

protected:
    BOOL DoMouseWheel(UINT fFlags, short zDelta, CPoint point);
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSwitch();
    afx_msg void OnSlider();
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

    DECLARE_MESSAGE_MAP()
};

#endif // !defined(_DU_VOLUME_H_)
