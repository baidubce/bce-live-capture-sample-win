#pragma once
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <map>

typedef
enum _tag_PIPELINE_STATUS {
    PS_READY,
    PS_STARTING,
    PS_RUNNING,
    PS_PAUSED,
    PS_RETRYING,
    PS_STOPPED,
} PIPELINE_STATUS;

class GstPipelineBase: public ILock {
public:
    GstPipelineBase();
    virtual ~GstPipelineBase(void);

public:
    int Create(std::map<LPCTSTR, LPCTSTR>& params);
    void Destroy();

    int Run();
    int Stop();
    int Seek(long long time);
    int Pause();
    virtual int CanPause();
    virtual int CanSeek();
    PIPELINE_STATUS GetStatus();

    static LPCTSTR FindVideoSink(CString& videoSink);
    static LPCTSTR FindAudioSink(CString& audioSink);

protected:
    GstPipeline* m_pGstPipeline;
    GstBus* m_pGstBus;

    typedef enum _tag_CMD {
        CMD_ZERO,
        PAUSE_CMD = GST_MAKE_FOURCC('P', 'A', 'U', 0),
        RESUME_CMD = GST_MAKE_FOURCC('R', 'E', 'S', 0),
        SEEK_CMD = GST_MAKE_FOURCC('S', 'E', 'E', 0),
    } BUS_CMD;
protected:
    void Lock();
    void Unlock();
    int IsRunning();
    int __cdecl SendBusCmd(BUS_CMD cmd, int wait, ...);

    virtual int OnBuilt();
    virtual LPCTSTR GetPipelineString(const std::map<LPCTSTR, LPCTSTR>&);

    virtual void OnSyncGstMessage(GstBus* bus, GstMessage* message);
    virtual void OnGstMessage(GstBus* bus, GstMessage* message);
    virtual BOOL OnTimer();

    virtual int OnPreStart();
    virtual void OnDestroy();

    virtual void OnBusCmd(BUS_CMD cmd, va_list args);

    int IsQuitRequest();

    virtual int OnBeforePipelineRun();
    virtual int OnBeforeRestartPipeline();
    virtual LPCTSTR Name();

    int Format(LPCTSTR fmt, const std::map<LPCTSTR, LPCTSTR>& args, CString& dst);
    static int IsUsableGstPlugin(LPCTSTR name, GstCaps* sink, GstCaps* src);

private:
    GMutex m_lock;

    PIPELINE_STATUS m_eStatus;
    GThread* m_pGstThread;
    volatile int __declspec(align(4)) m_nRequestExit;
    GMainLoop* m_pLoop;

    CMutexLock m_eventLock;
    std::map<void*, HANDLE> m_mapEvents;

private:
    static void OnSyncGstMessageFunc(GstBus* bus, GstMessage* message, void* data);
    static void OnGstMessageFunc(GstBus* bus, GstMessage* message, void* data);
    static BOOL OnTimerFunc(void* data);

    static gpointer GstThreadEntry(gpointer data);
    void GstThread();

    int Run_nolock();
    int Stop_nolock();
    int Resume_nolock();
    int Pause_nolock();
    int Seek_nolock(long long time);
    void Destroy_nolock();

    int Start_Gst_nolock(int& counter);
    void Stop_Gst_nolock();
    int Pause_Gst_nolock();
    void Loop_Gst_nolock(GMainContext* ctx);
    void SetStatus_nolock(PIPELINE_STATUS status);

    int PrepareWaitMsg(GstMessage* msg);
    void OnBusCmd(GstMessage* msg);
    int WaitMsgResult(GstMessage* msg);

};
