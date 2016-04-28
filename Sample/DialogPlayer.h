#pragma once
#include "afxwin.h"
#include "PPM.h"
#include "Volume.h"
#include "GstPlayer.h"
#include "Resource.h"
#include "live_capture.h"
#include "PlayScreen.h"

class CToolsDlg;

class CDialogPlayer : public CDialogEx, public CCriticalSection {
    DECLARE_DYNAMIC(CDialogPlayer)

public:
    CDialogPlayer(CWnd* pParent = NULL);   // standard constructor
    virtual ~CDialogPlayer();

    // Dialog Data
    enum { IDD = IDD_DIALOG_PLAYER };
public:
    void SetHost(CToolsDlg* host);
    void PushVideo(lc_sample_raw_video_t* sample);
    void PushAudio(lc_sample_raw_audio_t* sample);
    void Reset();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL PreTranslateMessage(LPMSG msg);
    DECLARE_MESSAGE_MAP()
    afx_msg LRESULT OnVolume(WPARAM wParam, LPARAM lParam);
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    static void OnLevelCallback(int ch, double val, gpointer data);
    const char* GetFileLocation(CString& location);
    void OnLevel(int ch, double val);
    void SavePosAndSize();
    void RestorePosAndSize();

    CPoint m_ptPos;
    CSize m_szClient;
    CPPM m_winPpm;
    CVolumeEx m_winVol;
    CPlayScreen m_winPic;
    BOOL m_bInited;

    GstPlayer* m_pPlayer;
    CToolsDlg* m_pHost;
    BOOL m_bFloating;
private:
    void Relayout();
};
