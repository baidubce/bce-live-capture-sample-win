#include "StdAfx.h"
#include "GstPipelineBase.h"
#include <vector>

GstPipelineBase::GstPipelineBase(void)
    : m_pGstPipeline(0)
    , m_pGstBus(0)
    , m_eStatus(PS_READY)
    , m_pGstThread(0)
    , m_nRequestExit(0)
    , m_pLoop(0) {
    g_mutex_init(&m_lock);
}


GstPipelineBase::~GstPipelineBase(void) {
    Destroy();
    g_mutex_clear(&m_lock);
}
LPCTSTR GstPipelineBase::Name() {
    return _T("GstPipelineBase");
}

int GstPipelineBase::IsRunning() {
    PIPELINE_STATUS s = m_eStatus;
    return (s > PS_READY && s < PS_STOPPED);
}
PIPELINE_STATUS GstPipelineBase::GetStatus() {
    return m_eStatus;
}

void GstPipelineBase::Lock() {
    g_mutex_lock(&m_lock);
}

void GstPipelineBase::Unlock() {
    g_mutex_unlock(&m_lock);
}

int GstPipelineBase::IsUsableGstPlugin(LPCTSTR name, GstCaps* sink, GstCaps* src) {
    USES_CONVERSION;

    GstElement* element = gst_element_factory_make(T2A(name), "test");

    if (!element) {
        return 0;
    }

    GParamSpec* spec = g_object_class_find_property(G_OBJECT_GET_CLASS(element), "async");

    if (spec && spec->flags & G_PARAM_WRITABLE) {
        g_object_set(G_OBJECT(element), "async", FALSE, NULL);
    }

    BOOL caps_ok = TRUE;

    if (src || sink) {

        GstIterator* iter = gst_element_iterate_pads(element);

        if (iter) {
            GstPad* pad = NULL;

            while (GST_ITERATOR_OK == gst_iterator_next(iter, (gpointer*)&pad)) {
                if (GST_PAD_DIRECTION(pad) == GST_PAD_SINK && sink) {
                    caps_ok &= gst_pad_set_caps(pad, sink);
                } else if (GST_PAD_DIRECTION(pad) == GST_PAD_SRC && src) {
                    caps_ok &= gst_pad_set_caps(pad, src);
                }

                gst_object_unref(pad);
            }

            gst_iterator_free(iter);
        }
    }

    int ret = (GST_STATE_CHANGE_FAILURE != gst_element_set_state(element, GST_STATE_PAUSED));
    gst_element_set_state(element, GST_STATE_NULL);
    gst_object_unref(element);
    return ret && caps_ok;
}

LPCTSTR GstPipelineBase::FindVideoSink(CString& videoSink) {
    LPCTSTR videosinks[] = {_T("d3dvideosink"), _T("dshowvideosink"), _T("directdrawsink"), 0};
    LPCTSTR* element = videosinks;

    videoSink = _T("fakesink");

    while (*element) {
        if (IsUsableGstPlugin(*element, NULL, NULL)) {
            videoSink = *element;
            break;
        }

        element++;
    }

    return videoSink;
}
LPCTSTR GstPipelineBase::FindAudioSink(CString& audioSink) {
    LPCTSTR audiosinks[] = {_T("waveformsink"), _T("directsoundsink"), 0};
    LPCTSTR* element = audiosinks;

    audioSink = _T("fakesink");

    GstCaps* caps = gst_caps_new_simple(
                        "audio/x-raw-int",
                        "rate", G_TYPE_INT, 44100,
                        "channels", G_TYPE_INT, 2,
                        "width", G_TYPE_INT, 16,
                        "depth", G_TYPE_INT, 16,
                        "signed", G_TYPE_BOOLEAN, FALSE,
                        "endianness", G_TYPE_INT, G_LITTLE_ENDIAN,
                        NULL);

    while (*element) {
        if (IsUsableGstPlugin(*element, caps, NULL)) {
            audioSink = *element;
            break;
        }

        element++;
    }

    gst_caps_unref(caps);

    return audioSink;
}

int GstPipelineBase::Format(LPCTSTR templ, const std::map<LPCTSTR, LPCTSTR>& params,
                            CString& dst) {
    dst.Empty();
    TCHAR* fmt = _tcsdup(templ);

    if (!fmt) {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("strdup failed."));
        return NULL;
    }

    int len = _tcslen(fmt);

    int start = -1;

    std::vector<LPCTSTR> va;

    for (int i = 0; i < len && fmt[i]; i ++) {
        if (fmt[i] == '{') {
            start = i;
        } else if (fmt[i] == '}' && start >= 0) {
            if (i - start - 1 > 0) {
                for (std::map<LPCTSTR, LPCTSTR>::const_iterator it = params.begin();
                        it != params.end();
                        it ++) {
                    if (_tcsnicmp(it->first, &fmt[start + 1], i - start - 1) == 0) {
                        va.push_back(it->second);
                        fmt[start] = '%';
                        fmt[start + 1] = 's';
                        memmove(&fmt[start + 2], &fmt[++i], (len - i + 1)*sizeof(TCHAR));
                        i = start + 1;
                        break;
                    }
                }
            }

            start = -1;
        }
    }

    if (va.size()) {
        dst.FormatV(fmt, (va_list)&va[0]);
    } else {
        dst.Format(_T("%s"), fmt);
    }

    free(fmt);

    return 1;
}

int GstPipelineBase::Create(std::map<LPCTSTR, LPCTSTR>& params) {
    USES_CONVERSION;
    CScopedLock lock(this);

    if (IsRunning()) {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("pipeline is runing.stop it and then try create again"));
        return 0;
    }

    Destroy_nolock();

    LPCTSTR pipestr = GetPipelineString(params);

    if (pipestr) {
        GError* err = NULL;
        GstElement* pipeElement = NULL;

        pipeElement = gst_parse_launch(T2A(pipestr), &err);

        if (pipeElement) {
            m_pGstPipeline = GST_PIPELINE(pipeElement);

            if (m_pGstPipeline) {
                if (!OnBuilt()) {
                    gst_object_unref(pipeElement);
                    LogError(_T(__FILE__), _T(__FUNCTION__), _T("pipeline string parsed, but OnBuilt failed"));
                    return 0;
                }

                return 1;
            } else {
                LogError(_T(__FILE__), _T(__FUNCTION__), _T("pipeline string parsed, but it is not type of GstPipeline"));
                gst_object_unref(pipeElement);
                return 0;
            }
        } else {
            if (err) {
                g_error_free(err);
            }

            LogError(_T(__FILE__), _T(__FUNCTION__), _T("create pipe failed, '%s'"), pipestr);
            return 0;
        }
    } else {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("GetPipelineString failed"));
        return 0;
    }
}

void GstPipelineBase::Destroy() {
    CScopedLock lock(this);
    Destroy_nolock();
}

int GstPipelineBase::Run() {
    CScopedLock lock(this);

    if (m_eStatus == PS_PAUSED) {
        return Resume_nolock();
    }

    if (IsRunning()) {
        return 1;
    }

    if (!OnPreStart()) {
        return 0;
    }

    return Run_nolock();
}

int GstPipelineBase::Run_nolock() {
    if (!m_pGstPipeline) {
        LogError(_T(__FILE__), _T(__FUNCTION__),
                 _T("CCapturePipeline::Start with NULL Pipeline"));
        return 0;
    }

    SetStatus_nolock(PS_STARTING);

    if (m_pGstThread) {
        g_thread_unref(m_pGstThread);
        m_pGstThread = NULL;
    }

    g_atomic_int_set(&m_nRequestExit, 0);

    GError* error = NULL;
    m_pGstThread = g_thread_try_new("gst_thread", GstThreadEntry, this, &error);

    if (error) {
        LogError(_T(__FILE__), _T(__FUNCTION__),
                 _T("create gst thread failed '%s'"), error->message);
        g_error_free(error);
    }

    if (!m_pGstThread) {
        SetStatus_nolock(PS_STOPPED);
        return 0;
    }

    return 1;
}

int GstPipelineBase::OnBeforePipelineRun() {
    return 1;
}

int GstPipelineBase::OnBeforeRestartPipeline() {
    return 1;
}

gpointer GstPipelineBase::GstThreadEntry(gpointer data) {
    reinterpret_cast<GstPipelineBase*>(data)->GstThread();
    return 0;
}

void GstPipelineBase::GstThread() {

    if (!m_pGstBus) {
        m_pGstBus = gst_pipeline_get_bus(m_pGstPipeline);
    }

    GMainContext* ctx = g_main_context_new();
    g_main_context_push_thread_default(ctx);
    gst_bus_add_signal_watch(m_pGstBus);
    gst_bus_enable_sync_message_emission(m_pGstBus);

    gulong sid = g_signal_connect(m_pGstBus, "message", G_CALLBACK(OnGstMessageFunc), this);
    gulong ssid = g_signal_connect(m_pGstBus, "sync-message", G_CALLBACK(OnSyncGstMessageFunc), this);
    gst_bus_set_flushing(m_pGstBus, FALSE);

    PIPELINE_STATUS inter_state = PS_STARTING;
    int loop_count = 0;

    while (!g_atomic_int_get(&m_nRequestExit)) {
        SetStatus_nolock(inter_state);

        if (inter_state == PS_RETRYING) {
            int i = 3;

            while (i-- > 0 && !g_atomic_int_get(&m_nRequestExit)) {
                Sleep(1000);
            }

            if (g_atomic_int_get(&m_nRequestExit)) {
                break;
            }

            if (!OnBeforeRestartPipeline()) {
                LogError(_T(__FILE__), _T(__FUNCTION__), _T("OnBeforeRestartPipeline failed. retrying later"));
                continue;
            }
        } else {
            if (!OnBeforePipelineRun()) {
                LogError(_T(__FILE__), _T(__FUNCTION__), _T("OnBeforePipelineRun failed."));
                break;
            }
        }

        SetStatus_nolock(PS_STARTING);

        gst_bus_set_flushing(m_pGstBus, TRUE);
        gst_bus_set_flushing(m_pGstBus, FALSE);

        if (Start_Gst_nolock(loop_count)) {
            SetStatus_nolock(PS_RUNNING);
            Loop_Gst_nolock(ctx);
        }

        Stop_Gst_nolock();
        inter_state = PS_RETRYING;
        loop_count ++;
    }

    g_signal_handler_disconnect(m_pGstBus, sid);
    GSource* source = g_main_context_find_source_by_id(ctx, m_pGstBus->signal_watch_id);

    if (source) {
        g_source_destroy(source);
    }

    gst_bus_remove_signal_watch(m_pGstBus);
    gst_bus_disable_sync_message_emission(m_pGstBus);

    g_main_context_pop_thread_default(ctx);
    g_main_context_unref(ctx);

    gst_bus_set_flushing(m_pGstBus, TRUE);
    gst_bus_set_flushing(m_pGstBus, FALSE);

    g_thread_exit(0);
}

int GstPipelineBase::Stop() {
    CScopedLock lock(this);
    return Stop_nolock();
}
int GstPipelineBase::CanSeek() {
    return 1;
}

int GstPipelineBase::Seek(long long time) {
    if (!CanSeek()) {
        return 0;
    }

    CScopedLock lock(this);

    if (!IsRunning()) {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("seek was called while pipeline is in wrong state"));
        return 0;
    }

    return Seek_nolock(time);
}

int GstPipelineBase::Pause() {
    if (!CanPause()) {
        return 0;
    }

    CScopedLock lock(this);

    if (m_eStatus == PS_PAUSED) {
        LogWarning(_T(__FILE__), _T(__FUNCTION__), _T("pipe line is already in paused state"));
        return 1;
    }

    if (!IsRunning()) {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("pipe line is not running when pause action requested."));
        return 0;
    }

    return Pause_nolock();
}

void GstPipelineBase::SetStatus_nolock(PIPELINE_STATUS status) {
    if (m_eStatus != status) {
        m_eStatus = status;

        LogInfo(_T("GstPipelineBase"), _T(__FUNCTION__), _T("Status Changed %d"), status);
    }
}

int GstPipelineBase::Stop_nolock() {
    if (!IsRunning()) {
        return 1;
    }

    if (!m_pGstThread) {
        SetStatus_nolock(PS_STOPPED);
        return 1;
    }

    g_atomic_int_inc(&m_nRequestExit);
    g_thread_join(m_pGstThread);
    SetStatus_nolock(PS_STOPPED);
    g_thread_unref(m_pGstThread);
    m_pGstThread = NULL;

    return 1;
}

int GstPipelineBase::Start_Gst_nolock(int& counter) {
    return !(GST_STATE_CHANGE_FAILURE ==
             gst_element_set_state(GST_ELEMENT(m_pGstPipeline), GST_STATE_PLAYING));
}

int GstPipelineBase::Pause_Gst_nolock() {
    return !(GST_STATE_CHANGE_FAILURE ==
             gst_element_set_state(GST_ELEMENT(m_pGstPipeline), GST_STATE_PAUSED));
}

void GstPipelineBase::OnGstMessageFunc(GstBus* bus, GstMessage* message, void* data) {
    reinterpret_cast<GstPipelineBase*>(data)->OnGstMessage(bus, message);
}
void GstPipelineBase::OnSyncGstMessageFunc(GstBus* bus, GstMessage* message, void* data) {
    reinterpret_cast<GstPipelineBase*>(data)->OnSyncGstMessage(bus, message);
}

BOOL GstPipelineBase::OnTimerFunc(void* data) {
    return reinterpret_cast<GstPipelineBase*>(data)->OnTimer();
}

int GstPipelineBase::IsQuitRequest() {
    return g_atomic_int_get(&m_nRequestExit);
}

void GstPipelineBase::OnSyncGstMessage(GstBus* bus, GstMessage* message) {

}

void GstPipelineBase::OnGstMessage(GstBus* bus, GstMessage* message) {
    USES_CONVERSION;

    if (message) {
        switch (message->type) {
        case GST_MESSAGE_EOS:
            LogError(_T(__FILE__), _T(__FUNCTION__),
                     _T("Gst Pipeline Received EOS message"));
            g_main_loop_quit(m_pLoop);
            break;

        case GST_MESSAGE_ERROR : {
            GError* error = NULL;
            gchar* debug = NULL;
            gst_message_parse_error(message, &error, &debug);

            LogError(_T(__FILE__), _T(__FUNCTION__),
                     _T("GstPipeline Received Error Message; Error:%s,Debug:%s"),
                     error ? A2T(error->message) : _T("(NULL)"), debug ? A2T(debug) : _T("(NULL)"));
            g_main_loop_quit(m_pLoop);

            if (error) {
                g_error_free(error);
            }

            if (debug) {
                g_free(debug);
            }
        }
        break;

        case GST_MESSAGE_WARNING : {
            GError* error = NULL;
            gchar* debug = NULL;
            gst_message_parse_warning(message, &error, &debug);
            LogError(_T(__FILE__), _T(__FUNCTION__),
                     _T("GstPipeline Received Warning Message; Error:%s,Debug:%s"),
                     error ? A2T(error->message) : _T("(NULL)"), debug ? A2T(debug) : _T("(NULL)"));

            if (error) {
                if (error->domain == GST_RESOURCE_ERROR &&
                        error->code == GST_RESOURCE_ERROR_WRITE) {
                    g_main_loop_quit(m_pLoop);
                }

                g_error_free(error);
            }

            if (debug) {
                g_free(debug);
            }
        }

        break;

        case GST_MESSAGE_INFO: {
            GError* error = NULL;
            gchar* debug = NULL;
            gst_message_parse_info(message, &error, &debug);
            LogInfo(_T(__FILE__), _T(__FUNCTION__),
                    _T("GstPipeline Received Info Message; Error:%s,Debug:%s"),
                    error ? A2T(error->message) : _T("(NULL)"), debug ? A2T(debug) : _T("(NULL)"));

            if (error) {
                g_error_free(error);
            }

            if (debug) {
                g_free(debug);
            }
        }
        break;

        case GST_MESSAGE_APPLICATION: {
            if (message->structure) {
                OnBusCmd(message);
            }
        }
        break;

        default:
            break;
        }
    }
}

BOOL GstPipelineBase::OnTimer() {
    // 如果没有要求停止，返回TRUE，Timer继续工作。
    if (!g_atomic_int_get(&m_nRequestExit)) {
        return TRUE;
    } else {
        //退出消息循环，返回FALSE，停止Timer
        g_main_loop_quit(m_pLoop);
        return FALSE;
    }
}

void GstPipelineBase::Loop_Gst_nolock(GMainContext* ctx) {
    m_pLoop = g_main_loop_new(ctx, TRUE);
    GSource* source = g_timeout_source_new_seconds(1);

    g_source_set_callback(source, OnTimerFunc, this, 0);
    g_source_attach(source, ctx);

    g_main_loop_run(m_pLoop);

    g_source_unref(source);
    g_main_loop_unref(m_pLoop);
    m_pLoop = NULL;
}

void GstPipelineBase::Stop_Gst_nolock() {
    gst_element_set_state(GST_ELEMENT(m_pGstPipeline), GST_STATE_NULL);
}

int GstPipelineBase::CanPause() {
    return 1;
}

void GstPipelineBase::Destroy_nolock() {

    Stop_nolock();

    OnDestroy();

    if (m_pGstBus) {
        gst_object_unref(m_pGstBus);
        m_pGstBus = NULL;
    }

    if (m_pGstPipeline) {
        gst_object_unref(m_pGstPipeline);
        m_pGstPipeline = NULL;
    }
}

int GstPipelineBase::OnPreStart() {
    if (!m_pGstPipeline) {
        return 0;
    }

    if (!m_pGstBus) {
        m_pGstBus = gst_pipeline_get_bus(m_pGstPipeline);

        if (!m_pGstBus) {
            LogError(_T(__FILE__), _T(__FUNCTION__), _T("gst_pipeline_get_bus failed"));
            return 0;
        }
    }

    return 1;
}

void GstPipelineBase::OnDestroy() {

}

int GstPipelineBase::OnBuilt() {
    if (!m_pGstBus) {
        m_pGstBus = gst_pipeline_get_bus(m_pGstPipeline);

        if (!m_pGstBus) {
            LogError(_T(__FILE__), _T(__FUNCTION__), _T("gst_pipeline_get_bus failed"));
            return 0;
        }
    }

    return 1;
}

LPCTSTR GstPipelineBase::GetPipelineString(const std::map<LPCTSTR, LPCTSTR>&) {
    LogError(_T(__FILE__), _T(__FUNCTION__), _T("child class must override GetPipelineString"));
    return NULL;
}

int GstPipelineBase::Pause_nolock() {
    return SendBusCmd(PAUSE_CMD, TRUE, NULL);
}

int GstPipelineBase::Resume_nolock() {
    return SendBusCmd(RESUME_CMD, TRUE, NULL);
}

int GstPipelineBase::Seek_nolock(long long time) {
    return SendBusCmd(SEEK_CMD, TRUE, "seekto", G_TYPE_INT64, time, NULL);
}

int GstPipelineBase::SendBusCmd(BUS_CMD cmd, int wait, ...) {
    if (!m_pGstBus) {
        return 0;
    }

    va_list args;
    va_start(args, wait);
    const char* params = va_arg(args, const char*);
    char name[5] = {0};

    *((BUS_CMD*)name) = cmd;

    GstStructure* str = gst_structure_new_valist(name, params, args);
    va_end(args);

    if (!str) {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("send bus cmd failed because of GstStructure alloc failure"));
        return 0;
    }

    GstMessage* msg = gst_message_new_custom(GST_MESSAGE_APPLICATION, NULL, str);

    if (!msg) {
        gst_structure_free(str);
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("send bus cmd failed because of GstMessage alloc failure"));
        return 0;
    }

    if (wait) {
        gst_message_ref(msg);
        wait = PrepareWaitMsg(msg);

        if (!wait) {
            gst_message_unref(msg);
        }
    }

    if (gst_bus_post(m_pGstBus, msg)) {
        if (wait) {
            return WaitMsgResult(msg);
        }

        return 1;
    } else {
        if (wait) {
            gst_message_unref(msg);
        }

        LogError(_T(__FILE__), _T(__FUNCTION__), _T("send bus cmd failed because of gst_bus_post failure"));
        return 0;
    }
}
static
gboolean structure_build_va(GQuark   field_id,
                            const GValue* value,
                            gpointer user_data) {
    va_list& v = *((va_list*)(user_data));
    va_arg(v, const char*) = g_quark_to_string(field_id);
    va_arg(v, const GValue*) = value;
    return TRUE;
}

void GstPipelineBase::OnBusCmd(GstMessage* msg) {
    HANDLE hComp = NULL;
    {
        CScopedLock lc(&m_eventLock);
        std::map<void*, HANDLE>::const_iterator it = m_mapEvents.find(msg);

        if (it != m_mapEvents.end()) {
            hComp = it->second;
        }
    }

    if (msg->structure) {
        BUS_CMD cmd = *((BUS_CMD*)gst_structure_get_name(msg->structure));

        int n = gst_structure_n_fields(msg->structure);
        int size = 0;
        int i = 0;

        va_list v = (va_list)malloc(n * sizeof(void*) * 2 + sizeof(void*));
        va_list keep = v;

        if (v) {
            gst_structure_foreach(msg->structure, structure_build_va, &v);
            OnBusCmd(cmd, keep);
            free(keep);
        }
    }

    if (hComp) {
        SetEvent(hComp);
    }
}

int GstPipelineBase::WaitMsgResult(GstMessage* msg) {
    int n = GST_MINI_OBJECT_REFCOUNT_VALUE(msg);

    HANDLE hComp = NULL;
    {
        CScopedLock lc(&m_eventLock);
        std::map<void*, HANDLE>::const_iterator it = m_mapEvents.find(msg);

        if (it != m_mapEvents.end()) {
            hComp = it->second;
        }
    }

    if (hComp) {
        int nwait = -1;

        while (WAIT_TIMEOUT == (nwait = WaitForSingleObject(hComp, 500))) {

            if (GST_MINI_OBJECT_REFCOUNT_VALUE(msg) < n) {
                break;
            }
        }

        {
            CScopedLock lc(&m_eventLock);
            CloseHandle(hComp);
            m_mapEvents.erase(msg);
        }

        gst_message_unref(msg);
        return nwait == WAIT_OBJECT_0;
    }

    gst_message_unref(msg);
    return 0;
}

int GstPipelineBase::PrepareWaitMsg(GstMessage* msg) {
    HANDLE h = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!h) {
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("CreateEvent failed"));
        return 0;
    }

    CScopedLock lc(&m_eventLock);
    m_mapEvents.insert(std::pair<void*, HANDLE>(msg, h));
    return 1;
}

void GstPipelineBase::OnBusCmd(BUS_CMD cmd, va_list args) {
    switch (cmd) {
    case PAUSE_CMD: {
        if (Pause_Gst_nolock()) {
            SetStatus_nolock(PS_PAUSED);
        }
    }
    break;

    case RESUME_CMD: {
        int i = 0;

        if (Start_Gst_nolock(i)) {
            SetStatus_nolock(PS_RUNNING);
        }
    }
    break;

    case SEEK_CMD: {
        const char* seekto = va_arg(args, const char*);
        const GValue* time = va_arg(args, const GValue*);

        gint64 cur = g_value_get_int64(time);
        gst_element_seek(GST_ELEMENT(m_pGstPipeline), 1.0f, GST_FORMAT_TIME,
                         GstSeekFlags(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                         GST_SEEK_TYPE_SET, cur, GST_SEEK_TYPE_NONE, 0);
    }
    break;

    default:
        LogError(_T(__FILE__), _T(__FUNCTION__), _T("Unknown bus cmd:%d"), cmd);
        break;
    }
}
