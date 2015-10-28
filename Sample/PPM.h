#if !defined(_DU_PPM_H_)
#define _DU_PPM_H_

#include "common.h"


#define MAX_SAMPLE          (10l)


typedef struct tagPPM_DATA {
    float m_fCurTag;
    float m_fCurBuoy;

    DWORD m_dwTrigger;

    BOOL m_bNeedDraw;

    float m_fSmaplePool[MAX_SAMPLE];
    HANDLE m_hSemFull, m_hSemEmpty, m_hMutex;
    int m_nHead, m_nTail;
} PPM_DATA;

#define  MAX_CHANNEL_COUNT (8l)

class CPPM : public CWnd {
    DECLARE_DYNAMIC(CPPM);
public:
    CPPM(int nChannelNumber);
    virtual ~CPPM();

public:
    void Trigger(int ch, float fTag);

#define RULER_BORDER    (1l)
#define BUOY_THICK      (5)


#define TEXT_COLOR (RGB(0x64,0x64,0x64))


#define TITLE_HEIGHT (8)
#define BOTTOM_HEIGHT (8)

#define MAX_SCALE           (0l)
#define OVER_THRESHOLD      (-5l)
#define NORMAL_THRESHOLD    (-20l)
#define MIN_SCALE           (-60l)

#define MAX_HEIGHT          (10000)

#define BUOY_STOP           (1000) // ms
#define LANDING_DURATION    (3500) //ms


#define IDLE                (35l)   //ms ,

protected:

    typedef enum _tagCHANNEL_TYPE {
        GRAYGREEN = 0,
        GRAYYELLOW,
        GRAYRED,

        GREEN,
        YELLOW,
        RED
    } CHANNEL_TYPE;

    int m_nWidth;
    int m_nHeight;

    float m_fLanding_Acc;

    int m_nStartPos;
    int m_nEndPos;

    CDC m_memDC;
    CDC m_memDotDC;

    CBitmap m_memBmp;
    HGDIOBJ m_hOldBmp;

    CRITICAL_SECTION m_csDraw;
    CEvent  m_eveQuit;
    HANDLE  m_hThreadHandle;

    int m_nOverThresholdPos;
    int m_nNormalThresholdPos;


    int m_nChannel;
    PPM_DATA m_data[MAX_CHANNEL_COUNT];

    CDC m_memForeDC;
    CBitmap m_memForeBmp;
    HGDIOBJ m_memForeOldBmp;

    DWORD m_dwLastDraw;

protected:
    inline void DrawBack(CDC& memDC, int nWidth, int nHeight);
    inline void DrawFore(CDC& memDC, int ch, int nWidth, int nHeight, int nTag, int nBuoy);
    static DWORD __stdcall ProcThread(LPVOID lParam);
    inline DWORD Proc();
    inline int LPtoDP(int nLP, int nMax, int nMin = 0);
    inline void DoPaint(CDC& wndDC, BOOL bforce);
    inline void QueueSample(int ch, float fSample);
    inline BOOL FetchSample(int ch, float& fSample);
    void DrawChannel(CDC& memDC, LPRECT rcSingleRuler, CHANNEL_TYPE dot, LPRECT clipping = NULL);

protected:
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
    afx_msg void OnNcPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    DECLARE_MESSAGE_MAP()
};
#endif // !defined(_DU_PPM_H_)
