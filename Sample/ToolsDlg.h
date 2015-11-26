#pragma once
#include "afxwin.h"
#include "ToolsModel.h"
#include "live_capture.h"
#include <vector>
#include "DialogPlayer.h"
#include "DialogNotification.h"
#include "DialogOption.h"

class CLogDlg;
class CSessionDlg;
class CPresetDlg;

class CToolsDlg : public CDialog {
public:
    CToolsDlg(CWnd* pParent = NULL);
    ~CToolsDlg();
    enum { IDD = IDD_TOOLS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    void UpdateRtmpRadio(int bUserRtmp);
    void UpdateLiveCaptureData();
    void UpdateStatus();
    void UpdateStatusUi(LC_STATUS status, LC_CODE code);
    BOOL ValidateData();
    void UpdateVideoInfos();
protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonAutoPreset();
    afx_msg void OnBnClickedButtonSelectPath();
    afx_msg void OnBnClickedButtonClose();
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedButtonStart();
    afx_msg LRESULT OnStatusChanged_WM(WPARAM wParam, LPARAM lParam);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedRadioRtmp();
    afx_msg void OnBnClickedRadioBce();
    afx_msg void OnBnClickedCheckSaveBce();
    afx_msg void OnBnClickedCheckSaveLocal();
    afx_msg void OnBnClickedButtonRefreshData();
    afx_msg void OnMenuPreset();
    afx_msg void OnUpdateMenuPreset(CCmdUI* pCmdUI);
    afx_msg void OnMenuSession();
    afx_msg void OnUpdateMenuSession(CCmdUI* pCmdUI);
    afx_msg void OnMenuLog();
    afx_msg void OnUpdateMenuLog(CCmdUI* pCmdUI);
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonSelectSession();
    afx_msg void OnBnClickedRadioExistingSession();
    afx_msg void OnCbnSelchangeComboVideoInfo();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnCbnSelchangeComboVideo();
    afx_msg void OnCbnSelchangeComboVideo2();

private:
    static void LC_API sample_callback(lc_handle_t handle, LC_SAMPLE_TYPE type, void* sample,
                                       void* usr_data);
    void OnSample(lc_handle_t handle, LC_SAMPLE_TYPE type, void* sample);

protected:
    void OnOK();
    void OnCancel();
    static LC_CODE LC_API enumVideoInfo(const lc_video_info_t* info, void* inst);
    LC_CODE OnEnumVideoInfo(const lc_video_info_t* info);
public:
    void OnLogDlgClosed(CLogDlg* dlg);
    void OnPresetDlgClosed(CPresetDlg* dlg);
    void OnSessionDlgClosed(CSessionDlg* dlg);
    void OnNotificationDlgClosed(CDialogNotification* dlg);
    void OnOptionDlgClosed(CDialogOption* dlg);
    void OnStatusChanged(LC_STATUS status, LC_CODE code);
protected:

    void OnStart();
    void OnStop();
    void EnableUI(BOOL enable);
private:
    CToolsModel m_Model;
    lc_handle_t m_handle;
    lc_preset_list_t* m_presets;
    CLogDlg* m_pLogDlg;
    CPresetDlg* m_pPresetDlg;
    CSessionDlg* m_pSessionDlg;
    CDialogOption* m_pOptionDlg;
    CDialogNotification* m_pNotificationDlg;
    CDialogPlayer m_wndPlayer;

    CEdit m_edtExistingSession;
    CEdit m_edtStatis;
    CEdit m_EdtLocalPath;
    CEdit m_EdtBosBucket;
    CEdit m_EdtUserDomain;
    CEdit m_EdtSessionName;
    CEdit m_EdtUserRtmp;
    CEdit m_EdtHLSPlayUrl;
    CEdit m_EdtRtmpPlayUrl;
    CEdit m_edtErrorDetail;
    CButton m_btnRefresh;
    CButton m_btnSelectSession;
    CButton m_BtnDetectPreset;
    CButton m_BtnSelectPath;
    CButton m_BtnClose;
    CButton m_BtnStop;
    CButton m_BtnStart;
    CButton m_rdoUserRtmp;
    CButton m_rdoBceRtmp;
    CButton m_rdoExistRtmp;
    CButton m_ChkSaveLocal;
    CComboBox m_cmbVideoInfos;
    CComboBox m_CmbVideoDevices;
    CComboBox m_CmbAudioDevices;
    CComboBox m_CmbPresets;
    CStatic m_staStatus;
    CEdit m_edtVideoX;
    CEdit m_edtVideoY;
    CEdit m_edtVideo2X;
    CEdit m_edtVideo2Y;
    CEdit m_edtVideoWidth;
    CEdit m_edtVideoHeight;
    CEdit m_edtVideo2Width;
    CEdit m_edtVideo2Height;
    CButton m_chkVideo2;
    CComboBox m_cmbVideo2;
    CComboBox m_cmbVideoInfo2;
    UINT m_uTimer;

private:
    std::vector<lc_video_info_t> m_vecVideoInfos;
    std::vector<lc_video_info_t> m_vecVideoInfos2;
public:
    void DockPlayer(CDialogPlayer* player);
    void UndockPlayer(CDialogPlayer* player);
    afx_msg void OnOption();
    afx_msg void OnNotification();
    afx_msg void OnUpdateNotification(CCmdUI* pCmdUI);
    afx_msg void OnUpdateOption(CCmdUI* pCmdUI);

    afx_msg void OnBnClickedCheckVideo2();
};
