#pragma once
#include "gstpipelinebase.h"
#include "live_capture.h"

class GstPlayer :
    public GstPipelineBase {
public:
    GstPlayer(void);
    ~GstPlayer(void);
private:
    guintptr m_hWndId;
    void (*OnLevel)(int, double, gpointer);
    void* m_pLevelData;
    GstElement* m_pVolume;
    CString m_strPipelineDesc;
    GstElement* m_pAppsrcV;
    GstElement* m_pAppsrcA;

    int m_bSetVideoCaps;
    int m_bSetAudioCaps;

public:
    void PushVideo(lc_sample_raw_video_t* sample);
    void PushAudio(lc_sample_raw_audio_t* sample);
public:
    virtual const char* GetPipelineString(const std::map<const char*, const char*>&);
    virtual int CanPause();
    virtual int CanSeek();
    virtual int OnBuilt();
    virtual void OnGstMessage(GstBus* bus, GstMessage* message);
    virtual void OnSyncGstMessage(GstBus* bus, GstMessage* message);
    virtual void OnDestroy();

    int SetWindowId(guintptr hwnd);
    void SetLevelCallback(void OnLevelCallback(int, double, gpointer), void* data);
    void SetVolume(float vol, int mute);
};
