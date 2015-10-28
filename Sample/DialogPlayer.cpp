// DialogPlayer.cpp : implementation file
//

#include "stdafx.h"
#include "DialogPlayer.h"
#include <math.h>
#include "ToolsDlg.h"

void CDialogPlayer::OnLevelCallback(int ch, double val, gpointer data) {
    reinterpret_cast<CDialogPlayer*>(data)->OnLevel(ch, val);
}
// CDialogPlayer dialog

IMPLEMENT_DYNAMIC(CDialogPlayer, CDialogEx)

CDialogPlayer::CDialogPlayer(CWnd* pParent /*=NULL*/)
    : CDialogEx(CDialogPlayer::IDD, pParent)
    , m_winPpm(2)
    , m_winVol(IDB_BITMAP_VOLUME_ON, IDB_BITMAP_VOLUME_OFF,
               IDB_BITMAP_VOLUME_FOREGROUND, IDB_BITMAP_VOLUME_BACKGROUND, IDB_BITMAP_VOLUME_SLIDER)
    , m_bInited(0)
    , m_pPlayer(0)
    , m_pHost(0)
    , m_bFloating(0)
    , m_ptPos(0, 0)
    , m_szClient(0, 0) {

}

CDialogPlayer::~CDialogPlayer() {
}

void CDialogPlayer::SetHost(CToolsDlg* host) {
    m_pHost = host;
}

BEGIN_MESSAGE_MAP(CDialogPlayer, CDialogEx)
    ON_MESSAGE(CVolumeEx::WM_VOLUME, &CDialogPlayer::OnVolume)
    ON_WM_SIZE()
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

BOOL CDialogPlayer::PreTranslateMessage(LPMSG msg) {
    if (msg->hwnd == m_winPic.GetSafeHwnd() && msg->message == WM_LBUTTONDBLCLK) {
        OnLButtonDblClk(0, msg->pt);
    }

    return CDialogEx::PreTranslateMessage(msg);
}

void CDialogPlayer::OnLButtonDblClk(UINT nFlags, CPoint point) {
    if (m_bFloating) {
        SavePosAndSize();
        m_pHost->DockPlayer(this);
        m_bFloating = 0;
    } else {
        m_pHost->UndockPlayer(this);
        RestorePosAndSize();

        m_bFloating = 1;
    }
}
void CDialogPlayer::SavePosAndSize() {
    CRect rcWin;
    GetWindowRect(rcWin);
    m_ptPos.SetPoint(rcWin.left, rcWin.top);

    m_szClient.SetSize(rcWin.Width(), rcWin.Height());
}
void CDialogPlayer::RestorePosAndSize() {
    if (!(m_ptPos == CPoint(0, 0) || m_szClient == CSize(0, 0))) {
        MoveWindow(CRect(m_ptPos, m_szClient));
        UpdateWindow();
        ShowWindow(SW_SHOW);
    } else {
        ShowWindow(SW_SHOWMAXIMIZED);
    }
}

void CDialogPlayer::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BOOL CDialogPlayer::OnInitDialog() {

    CDialogEx::OnInitDialog();

    m_bInited = 1;
    Relayout();
    Reset();
    EnableToolTips();
    return TRUE;
}

void CDialogPlayer::PushVideo(lc_sample_raw_video_t* sample) {
    Lock();

    if (m_pPlayer) {
        m_pPlayer->PushVideo(sample);
    }

    Unlock();
}

void CDialogPlayer::PushAudio(lc_sample_raw_audio_t* sample) {
    Lock();

    if (m_pPlayer) {
        m_pPlayer->PushAudio(sample);
    }

    Unlock();
}

void CDialogPlayer::Reset() {
    Lock();

    if (m_pPlayer) {
        delete m_pPlayer;
        m_pPlayer = NULL;
    }

    m_pPlayer = new GstPlayer();
    m_pPlayer->SetWindowId((guintptr)(m_winPic.GetSafeHwnd()));
    m_pPlayer->SetLevelCallback(OnLevelCallback, this);

    std::map<const char*, const char*> params;
    CString videoSink;
    CString audioSink;

    params["VIDEOSINK"] = GstPipelineBase::FindVideoSink(videoSink);
    params["AUDIOSINK"] = GstPipelineBase::FindAudioSink(audioSink);

    if (!m_pPlayer->Create(params)) {
        MessageBox("´´½¨Ô¤ÀÀÊ§°Ü");
    } else {
        m_pPlayer->Run();
    }

    int on = 0;
    int vol = 0;
    m_winVol.GetVolume(on, vol);

    float f = vol / 100.0f;

    if (f > 1.0f) {
        f = 1.0f;
    }

    if (f < 0) {
        f =  0;
    }

    f = log10f(9 * f + 1);
    f = min(1.0f, max(0, f));

    m_pPlayer->SetVolume(f, !on);

    Unlock();
}

void CDialogPlayer::OnDestroy() {
    Lock();

    if (m_pPlayer) {
        delete m_pPlayer;
        m_pPlayer = NULL;
    }

    Unlock();

    CDialogEx::OnDestroy();
}
void CDialogPlayer::OnOK() {

}
void CDialogPlayer::OnCancel() {

}
void CDialogPlayer::Relayout() {
    CRect rc;
    GetClientRect(rc);

    const int PANEL_WIDTH = 45;
    const int MIN_PANEL_HEIGHT = 80;

    if (rc.Width() < PANEL_WIDTH && rc.Height() < MIN_PANEL_HEIGHT) {
        m_winPpm.ShowWindow(SW_HIDE);
        m_winVol.ShowWindow(SW_HIDE);
        m_winPic.ShowWindow(SW_HIDE);
    } else {

        rc.right -= PANEL_WIDTH;
        m_winPic.MoveWindow(rc);

        rc.left = rc.right;
        rc.right = rc.left + PANEL_WIDTH;

        rc.bottom -= MIN_PANEL_HEIGHT;
        m_winPpm.MoveWindow(rc);

        rc.top = rc.bottom;
        rc.bottom = rc.top + MIN_PANEL_HEIGHT;
        m_winVol.MoveWindow(rc);

        m_winPpm.ShowWindow(SW_SHOW);
        m_winVol.ShowWindow(SW_SHOW);
        m_winPic.ShowWindow(SW_SHOW);
    }
}

BOOL CDialogPlayer::OnCreate(LPCREATESTRUCT lpCreateStruct) {
    m_winPpm.Create(gGetChildWindowClassName(), "PPM", WS_CHILD | WS_VISIBLE, CRect(), this, 0x123);
    m_winVol.Create(gGetChildWindowClassName(), "VOL", WS_CHILD | WS_VISIBLE, CRect(), this, 0x345);
    m_winPic.Create(AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_PARENTDC | CS_DBLCLKS, 0,
                                        (HBRUSH)GetStockObject(BLACK_BRUSH)), "PIC", WS_CHILD | WS_VISIBLE, CRect(), this, 0x567);
    return CWnd::OnCreate(lpCreateStruct);
}

void CDialogPlayer::OnWindowPosChanging(WINDOWPOS* lpwndpos) {
    if (lpwndpos->cx < 500) {
        lpwndpos->cx = 500;
    }

    if (lpwndpos->cy < 400) {
        lpwndpos->cy = 400;
    }

    CDialogEx::OnWindowPosChanging(lpwndpos);
}

void CDialogPlayer::OnSize(UINT nType, int cx, int cy) {
    if (m_bInited) {
        Relayout();
    }

    CDialogEx::OnSize(nType, cx, cy);
}

LRESULT CDialogPlayer::OnVolume(WPARAM wParam, LPARAM lParam) {

    float f = lParam / 100.0f;

    if (f > 1.0f) {
        f = 1.0f;
    }

    if (f < 0) {
        f =  0;
    }

    f = log10f(9 * f + 1);
    f = min(1.0f, max(0, f));

    if (m_pPlayer) {
        m_pPlayer->SetVolume(f, !wParam);
    }

    return 0;
}

void CDialogPlayer::OnLevel(int ch, double val) {
    if (ch >= 0 && ch <= 1) {
        m_winPpm.Trigger(ch, (float)val);
    }
}
