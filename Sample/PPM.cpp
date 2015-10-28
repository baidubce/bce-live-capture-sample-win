#include "stdafx.h"
#include "PPM.h"
#include "DuBitmap.h"
#include <math.h>

IMPLEMENT_DYNAMIC(CPPM, CWnd);

const int ruler[] = {0, -1, -2, -3, -5, -12, -20, -24, -30, -40, -50, -60};
const int rulersize = sizeof(ruler) / sizeof(ruler[0]);

CCriticalSection mLock;

//#define _DEBUG_SAMPLE

CPPM::CPPM(int nCh)
    : m_nWidth(0)
    , m_nHeight(0)
    , m_nStartPos(-1)
    , m_nEndPos(-1)
    , m_hOldBmp(NULL)
    , m_hThreadHandle(NULL)
    , m_nChannel(nCh)
    , m_fLanding_Acc(0.0f)
    , m_eveQuit(FALSE, TRUE)
    , m_memForeOldBmp(NULL)
    , m_dwLastDraw(0) {
    ASSERT(m_nChannel > 0 && m_nChannel <= 8);

    for (int i = 0 ; i < m_nChannel; i ++) {
        m_data[i].m_fCurBuoy = -1.0f;
        m_data[i].m_fCurTag = -1.0f;
        m_data[i].m_nHead = 0;
        m_data[i].m_nTail = 0;
        m_data[i].m_bNeedDraw = FALSE;
        m_data[i].m_dwTrigger = 0;
        m_data[i].m_hSemFull = CreateSemaphore(NULL, 0, MAX_SAMPLE, NULL);
        m_data[i].m_hSemEmpty = CreateSemaphore(NULL, MAX_SAMPLE, MAX_SAMPLE, NULL);
        m_data[i].m_hMutex = CreateSemaphore(NULL, 1, 1, NULL);
    }

    InitializeCriticalSection(&m_csDraw);
    m_eveQuit.ResetEvent();
}

int CPPM::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    DWORD dwThreadID = 0;
    m_hThreadHandle = CreateThread(NULL, NULL, ProcThread, this, 0, &dwThreadID);
    TRACE1("Create PPM Draw Thread :%x\r\n", dwThreadID);

    if (!m_hThreadHandle) {
        return -1;
    }

    return  CWnd::OnCreate(lpCreateStruct);
}

int CPPM::LPtoDP(int nLP, int nMax, int nMin /* = 0 */) {
    if (nLP > 0) {
        nLP = 0;
    }

    if (nLP < -60) {
        nLP = -60;
    }

    ASSERT(nMax > nMin);
    double fScale = (nMax - nMin) / 10.71f;

    int nRet =  int(sqrt((1.0 - pow((nLP + 200.0),
                                    2) / 40000.0) * 225.0) * fScale + 0.5) + nMin;

    if (nRet < nMin) {
        nRet = nMin;
    }

    if (nRet > nMax) {
        nRet = nMax;
    }

    return nRet;
}


DWORD __stdcall CPPM::ProcThread(LPVOID lParam) {
    CPPM* pThis = reinterpret_cast<CPPM*>(lParam);
    return pThis->Proc();
}

DWORD CPPM::Proc() {
    int i = 0;
    float fSample = 0;
    int  nextTag = 0;

    while (TRUE) {
        if (m_nEndPos > m_nStartPos && m_nStartPos > 0) {
            bool bNeedDraw = false;

            for (i = 0; i < m_nChannel; i ++) {
                if (m_data[i].m_bNeedDraw) {
                    int landing = 1;

                    if (FetchSample(i, fSample)) {

                        EnterCriticalSection(&m_csDraw);

                        float fTag = fSample;

                        int nextTag = LPtoDP(int(fTag - 0.5), m_nEndPos - RULER_BORDER, m_nStartPos + RULER_BORDER);

                        if (nextTag < m_data[i].m_fCurTag || m_data[i].m_fCurTag == -1.0f) {
                            landing = 0;

                            m_data[i].m_fCurTag = (float)nextTag;

                            if (m_data[i].m_fCurBuoy > m_data[i].m_fCurTag || m_data[i].m_fCurBuoy == -1.0f) {
                                m_data[i].m_dwTrigger = GetTickCount();
                                m_data[i].m_fCurBuoy = m_data[i].m_fCurTag;
                            } else {
                                int elapsed = (int)(GetTickCount() - m_data[i].m_dwTrigger) - BUOY_STOP;

                                if (elapsed > 0) {
                                    float velocity = m_fLanding_Acc * elapsed;

                                    m_data[i].m_fCurBuoy += velocity * IDLE;

                                    if (m_data[i].m_fCurBuoy > m_data[i].m_fCurTag) {
                                        m_data[i].m_fCurBuoy = m_data[i].m_fCurTag;
                                    }
                                }
                            }
                        }

                        LeaveCriticalSection(&m_csDraw);
                    }

                    int elapsed = (int)(GetTickCount() - m_data[i].m_dwTrigger);

                    if (m_data[i].m_fCurTag != -1.0f) {
                        float velocity = m_fLanding_Acc * elapsed;
                        m_data[i].m_fCurTag += velocity * IDLE;

                        if (m_data[i].m_fCurTag > (float)(m_nEndPos)) {
                            m_data[i].m_fCurTag = -1.0f;
                        }
                    }

                    if (m_data[i].m_fCurBuoy != -1.0f) {
                        elapsed -= BUOY_STOP;

                        if (elapsed > 0) {
                            float velocity = m_fLanding_Acc * elapsed;
                            m_data[i].m_fCurBuoy += velocity * IDLE;

                            if (m_data[i].m_fCurBuoy > (float)m_nEndPos) {
                                m_data[i].m_fCurBuoy = -1.0f;
                                m_data[i].m_bNeedDraw = FALSE;
                            }
                        }
                    }

                    bNeedDraw = true;
                }
            }

            if (bNeedDraw) {
                mLock.Lock();
                DoPaint(CClientDC(this), FALSE);
                mLock.Unlock();
            }
        }

        if (WAIT_OBJECT_0 == WaitForSingleObject(m_eveQuit, IDLE)) {
            break;
        }
    }

    return 0;
}

CPPM::~CPPM() {
    DeleteCriticalSection(&m_csDraw);

    for (int i = 0; i < m_nChannel; i ++) {
        CloseHandle(m_data[i].m_hSemEmpty);
        CloseHandle(m_data[i].m_hSemFull);
        CloseHandle(m_data[i].m_hMutex);
    }

    if (m_memDC.m_hDC) {
        if (m_hOldBmp) {
            m_memDC.SelectObject(m_hOldBmp);
            m_hOldBmp = NULL;
        }

        m_memDC.DeleteDC();
    }

    if (m_memBmp.m_hObject) {
        m_memBmp.DeleteObject();
    }

    if (m_memForeDC.m_hDC) {
        if (m_memForeOldBmp) {
            m_memForeDC.SelectObject(m_memForeOldBmp);
            m_memForeOldBmp = NULL;
        }

        m_memForeDC.DeleteDC();
    }

    if (m_memForeBmp.m_hObject) {
        m_memForeBmp.DeleteObject();
    }

}

BEGIN_MESSAGE_MAP(CPPM, CWnd)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_WM_CREATE()
    ON_WM_NCCALCSIZE()
    ON_WM_NCPAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CPPM::Trigger(int ch, float fTag) {
    QueueSample(ch, fTag);
    m_data[ch].m_bNeedDraw = TRUE;
#ifdef _DEBUG_SAMPLE
    CString log;
    log.Format("%d %.2f\r\n", ch, fTag);
    OutputDebugString(log);
#endif
}

inline void CPPM::QueueSample(int ch, float fSample) {
    WaitForSingleObject(m_data[ch].m_hMutex, INFINITE);

    if (WAIT_OBJECT_0 == WaitForSingleObject(m_data[ch].m_hSemEmpty, 0)) {
        m_data[ch].m_fSmaplePool[m_data[ch].m_nHead] = fSample;
        m_data[ch].m_nHead++;
        m_data[ch].m_nHead %= MAX_SAMPLE;
        ReleaseSemaphore(m_data[ch].m_hSemFull, 1, NULL);
    } else {
        m_data[ch].m_fSmaplePool[m_data[ch].m_nHead] = fSample;
        m_data[ch].m_nHead ++;
        m_data[ch].m_nHead %= MAX_SAMPLE;
        m_data[ch].m_nTail ++ ;
        m_data[ch].m_nTail %= MAX_SAMPLE;
    }

    ReleaseSemaphore(m_data[ch].m_hMutex, 1, NULL);
}
inline BOOL CPPM::FetchSample(int ch, float& fSample) {
    WaitForSingleObject(m_data[ch].m_hMutex, INFINITE);
    BOOL bRet = FALSE;

    if (WAIT_OBJECT_0 == WaitForSingleObject(m_data[ch].m_hSemFull, 0)) {
        fSample = m_data[ch].m_fSmaplePool[m_data[ch].m_nTail];
        m_data[ch].m_nTail++;
        m_data[ch].m_nTail %= MAX_SAMPLE;
        ReleaseSemaphore(m_data[ch].m_hSemEmpty, 1, NULL);
        bRet = TRUE;
    }

    ReleaseSemaphore(m_data[ch].m_hMutex, 1, NULL);
    return bRet;
}

void CPPM::OnSize(UINT nType, int cx, int cy) {
    CWnd::OnSize(nType, cx, cy);

    CWindowDC dc(this);
    EnterCriticalSection(&m_csDraw);

    for (int i = 0 ; i < m_nChannel; i ++) {
        m_data[i].m_fCurBuoy = -1.0f;
        m_data[i].m_fCurTag = -1.0f;
    }

    CRect rect;
    GetClientRect(&rect);
    m_nWidth = rect.Width();
    m_nHeight = rect.Height();

    if (m_nWidth < 2 * RULER_BORDER) {
        m_nStartPos = -1;
        m_nEndPos = -1;
        LeaveCriticalSection(&m_csDraw);
        return ;
    }

    if (m_nHeight < BUOY_THICK + RULER_BORDER + TITLE_HEIGHT + BOTTOM_HEIGHT) {
        m_nStartPos = -1;
        m_nEndPos = -1;
        LeaveCriticalSection(&m_csDraw);
        return ;
    }

    m_nStartPos = LPtoDP(MAX_SCALE,  min(MAX_HEIGHT,
                                         m_nHeight) - RULER_BORDER - TITLE_HEIGHT - BOTTOM_HEIGHT, RULER_BORDER);
    m_nEndPos = LPtoDP(MIN_SCALE,  min(MAX_HEIGHT,
                                       m_nHeight) - TITLE_HEIGHT - RULER_BORDER - BOTTOM_HEIGHT, RULER_BORDER);

    m_nStartPos = m_nHeight - min(MAX_HEIGHT, m_nEndPos - m_nStartPos) - BOTTOM_HEIGHT;
    m_nEndPos = m_nHeight - BOTTOM_HEIGHT;

    if (m_nEndPos - m_nStartPos - 2 * RULER_BORDER < 0) {
        m_nStartPos = -1;
        m_nEndPos = -1;
        LeaveCriticalSection(&m_csDraw);
        return;
    }

    m_nOverThresholdPos = LPtoDP(OVER_THRESHOLD, m_nEndPos - RULER_BORDER, m_nStartPos + RULER_BORDER);
    m_nNormalThresholdPos = LPtoDP(NORMAL_THRESHOLD, m_nEndPos - RULER_BORDER,
                                   m_nStartPos + RULER_BORDER);

    m_fLanding_Acc = ((float)(2 * (m_nEndPos - m_nStartPos - 2 * RULER_BORDER))) /
                     (LANDING_DURATION * LANDING_DURATION);

    if (m_memDC.m_hDC) {
        if (m_hOldBmp) {
            m_memDC.SelectObject(m_hOldBmp);
            m_hOldBmp = NULL;
        }
    } else {
        m_memDC.CreateCompatibleDC(&dc);
    }

    m_memBmp.DeleteObject();

    if (m_memForeDC.m_hDC) {
        if (m_memForeOldBmp) {
            m_memForeDC.SelectObject(m_memForeOldBmp);
            m_memForeOldBmp = NULL;
        }
    } else {
        m_memForeDC.CreateCompatibleDC(&dc);
    }

    m_memForeBmp.DeleteObject();


    m_memBmp.CreateCompatibleBitmap(&dc, m_nWidth, m_nHeight);
    m_hOldBmp = m_memDC.SelectObject(&m_memBmp);
    DrawBack(m_memDC, m_nWidth, m_nHeight);

    m_memForeBmp.CreateCompatibleBitmap(&dc, m_nWidth, m_nHeight);
    m_memForeOldBmp = m_memForeDC.SelectObject(&m_memForeBmp);

    LeaveCriticalSection(&m_csDraw);
}

void CPPM::DrawChannel(CDC& memDC, LPRECT rcSingleRuler, CHANNEL_TYPE dot,
                       LPRECT clipping/*=NULL*/) {
    static COLORREF colors[] = {
        RGB(0, 0x7f, 0),
        RGB(0x7f, 0x7f, 0),
        RGB(0x7f, 0, 0),

        RGB(0, 0xff, 0),
        RGB(0xff, 0xff, 0),
        RGB(0xff, 0, 0)
    };

    COLORREF color = colors[dot];

    memDC.FillSolidRect(rcSingleRuler, color);
}

void CPPM::DrawBack(CDC& memDC, int nWidth, int nHeight) {
    memDC.FillRect(CRect(0, 0, nWidth, nHeight),
                   CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)));

    if (m_nEndPos > m_nStartPos && m_nStartPos > 0) {
        int nTotalRulerWid = 9 * m_nChannel;
        int nScaleWid = nWidth - nTotalRulerWid;

        int nPerRulerWid = nTotalRulerWid / m_nChannel;
        CRect rcRefRuler(0, m_nStartPos, nPerRulerWid, m_nEndPos);

        CFont font;
        int nFontWid = 0;
        int nFontHei = 0;
        int i = 0;
        CFont* pOldFont = 0;
        int nOldMode = 0;
        COLORREF crOldColor = 0;

        do {
            CRect rcRuler(rcRefRuler);

            rcRuler.DeflateRect(RULER_BORDER, RULER_BORDER, RULER_BORDER, RULER_BORDER);
            int nPos = rcRuler.top;

            CRect rcSingleRuler(rcRuler.left, 0, rcRuler.right, 0);

            rcSingleRuler.top = nPos;
            rcSingleRuler.bottom = m_nOverThresholdPos;
            DrawChannel(memDC, rcSingleRuler, GRAYRED);

            rcSingleRuler.top = m_nOverThresholdPos;
            rcSingleRuler.bottom = m_nNormalThresholdPos;
            DrawChannel(memDC, rcSingleRuler, GRAYYELLOW);

            rcSingleRuler.top = m_nNormalThresholdPos;
            rcSingleRuler.bottom = m_nEndPos;
            DrawChannel(memDC, rcSingleRuler, GRAYGREEN);

            rcRefRuler.OffsetRect(nPerRulerWid, 0);

        } while ((++i) < m_nChannel);

        CRect rcScale(nTotalRulerWid, m_nStartPos, nWidth, m_nEndPos);
        rcScale.DeflateRect(RULER_BORDER, RULER_BORDER, RULER_BORDER, RULER_BORDER);

        if (rcScale.Width() < 0) {
            return;
        }

        nFontWid = 5;
        nFontHei = 10;

        font.CreateFont(
            nFontHei ,                  // nHeight
            nFontWid ,                  // nWidth
            0,                          // nEscapement
            0,                          // nOrientation
            FW_THIN,                    // nWeight
            FALSE,                      // bItalic
            FALSE,                      // bUnderline
            0,                          // cStrikeOut
            ANSI_CHARSET,               // nCharSet
            OUT_DEFAULT_PRECIS,         // nOutPrecision
            CLIP_DEFAULT_PRECIS,        // nClipPrecision
            DEFAULT_QUALITY,            // nQuality
            DEFAULT_PITCH | FF_SWISS,   // nPitchAndFamily
            _T("Arial"));               // lpszFacename

        pOldFont = memDC.SelectObject(&font);
        nOldMode = memDC.SetBkMode(TRANSPARENT);
        crOldColor =  memDC.SetTextColor(EDIT_TEXT_COLOR);

        CRect rcSingleScale(rcScale.left, 0, rcScale.right, 0), rcLastScale(0, 0, 0, 0);
        int nSingleScale = 0;
        CString strSingleScale = "" ;

        i = 0;

        while (i < rulersize) {
            rcLastScale = rcSingleScale;

            nSingleScale = LPtoDP(ruler[i], rcScale.bottom, rcScale.top);

            rcSingleScale.top = nSingleScale - nFontHei / 2;
            rcSingleScale.bottom = nSingleScale + nFontHei / 2;

            if (rcSingleScale.top < rcScale.top) {
                rcSingleScale.OffsetRect(0, rcScale.top - rcSingleScale.top);
            }

            if (rcSingleScale.bottom > rcScale.bottom) {
                rcSingleScale.OffsetRect(0, rcScale.bottom - rcSingleScale.bottom);
            }

            if (rcSingleScale.top >= rcLastScale.bottom) {
                strSingleScale.Format("%d", ruler[i]);
                memDC.DrawText(strSingleScale, rcSingleScale, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
            }

            i++;
        }

        memDC.SetBkMode(nOldMode);
        memDC.SetTextColor(crOldColor);
        memDC.SelectObject(pOldFont);
        pOldFont = NULL;
        font.DeleteObject();
    }
}

void CPPM::DrawFore(CDC& memDC, int ch, int nWidth, int nHeight, int nTag, int nBuoy) {
    if (nTag != -1 && nBuoy > nTag) {
        nBuoy = nTag;
    }

    if (m_nEndPos > m_nStartPos && m_nStartPos > 0) {

        int nRulerWid = 9;

        int nPPMHei = min(MAX_HEIGHT, m_nEndPos - m_nStartPos);

        CRect rcRuler(ch * nRulerWid, m_nStartPos, ch * nRulerWid + nRulerWid, m_nEndPos);
        rcRuler.DeflateRect(RULER_BORDER, RULER_BORDER, RULER_BORDER, RULER_BORDER);

        CRect rcSingleRuler(rcRuler.left, 0, rcRuler.right, 0);

        if (nTag > 0) {
            if (m_nOverThresholdPos > nTag) {
                rcSingleRuler.top = max(nTag, rcRuler.top);
                rcSingleRuler.bottom = m_nOverThresholdPos;
                DrawChannel(memDC, rcSingleRuler, RED);
            }

            if (m_nNormalThresholdPos > nTag) {
                rcSingleRuler.top = max(nTag, m_nOverThresholdPos);
                rcSingleRuler.bottom = m_nNormalThresholdPos;
                DrawChannel(memDC, rcSingleRuler, YELLOW);
            }

            if (rcRuler.bottom > nTag) {
                rcSingleRuler.top = max(nTag, m_nNormalThresholdPos);;
                rcSingleRuler.bottom = rcRuler.bottom;
                DrawChannel(memDC, rcSingleRuler, GREEN);
            }
        }

        if (nBuoy > rcRuler.top && nBuoy < rcRuler.bottom) {

            rcSingleRuler.top = nBuoy;
            rcSingleRuler.bottom = min(rcRuler.bottom, nBuoy + BUOY_THICK);

            if (rcSingleRuler.top >= m_nNormalThresholdPos) {
                DrawChannel(memDC, rcSingleRuler, GREEN);
            } else if (rcSingleRuler.top >= m_nOverThresholdPos) {
                if (rcSingleRuler.bottom <= m_nNormalThresholdPos) {
                    DrawChannel(memDC, rcSingleRuler, YELLOW);
                } else {
                    CRect rc(rcSingleRuler);
                    rc.bottom = m_nNormalThresholdPos;
                    DrawChannel(memDC, rc, YELLOW);

                    rcSingleRuler.top = m_nNormalThresholdPos;
                    DrawChannel(memDC, rcSingleRuler, GREEN);
                }
            } else {
                if (rcSingleRuler.bottom <= m_nOverThresholdPos) {
                    DrawChannel(memDC, rcSingleRuler, RED);
                } else {
                    CRect rc(rcSingleRuler);
                    rc.bottom = m_nOverThresholdPos;
                    DrawChannel(memDC, rc, RED);

                    rcSingleRuler.top = m_nOverThresholdPos;

                    if (rcSingleRuler.bottom <= m_nNormalThresholdPos) {
                        DrawChannel(memDC, rcSingleRuler, YELLOW);
                    } else {
                        CRect rc(rcSingleRuler);
                        rc.bottom = m_nNormalThresholdPos;
                        DrawChannel(memDC, rc, YELLOW);

                        rcSingleRuler.top = m_nNormalThresholdPos;
                        DrawChannel(memDC, rcSingleRuler, GREEN);
                    }
                }

            }
        }
    }
}

void CPPM::OnPaint() {
    CPaintDC dc(this); // device context for painting
    DoPaint(dc, true);
}

void CPPM::DoPaint(CDC& wndDC, BOOL bforce) {
    DWORD dwNow = GetTickCount();

    if (!bforce && dwNow < m_dwLastDraw + 20) {
        return;
    }

    m_dwLastDraw = dwNow;

    EnterCriticalSection(&m_csDraw);

    if (m_nHeight <= 0 || m_nWidth <= 0 || m_memDC.m_hDC == NULL) {
        LeaveCriticalSection(&m_csDraw);
        return;
    }

    m_memForeDC.BitBlt(0, 0, m_nWidth, m_nHeight, &m_memDC, 0, 0, SRCCOPY);

    for (int i = 0 ; i < m_nChannel; i ++) {
        if (m_data[i].m_fCurBuoy > 0 || m_data[i].m_fCurTag > 0) {
            DrawFore(m_memForeDC, i, m_nWidth, m_nHeight, (int)(m_data[i].m_fCurTag),
                     (int)(m_data[i].m_fCurBuoy));
        }
    }

    LeaveCriticalSection(&m_csDraw);

    wndDC.BitBlt(0, 0, m_nWidth, m_nHeight, &m_memForeDC, 0, 0, SRCCOPY);
}

void CPPM::OnDestroy() {
    if (m_hThreadHandle) {
        m_eveQuit.SetEvent();
        WaitForSingleObject(m_hThreadHandle, INFINITE);
        CloseHandle(m_hThreadHandle);
        m_hThreadHandle = 0;
    }

    CWnd::OnDestroy();
}


void CPPM::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp) {
    ((CRect*)(lpncsp))->DeflateRect(2, 2, 2, 2);
}

void CPPM::OnNcPaint() {
    CRect rcWin(0, 0, 0, 0);
    GetWindowRect(rcWin);
    rcWin.OffsetRect(-rcWin.left, -rcWin.top);

    CWindowDC dc(this);

    dc.Draw3dRect(rcWin, RGB(28, 28, 28), RGB(57, 57, 57));
    rcWin.DeflateRect(1, 1, 1, 1);
    dc.Draw3dRect(rcWin, RGB(0, 0, 0), RGB(0, 0, 0));
}

BOOL CPPM::OnEraseBkgnd(CDC* pDC) {
    return FALSE;
}
