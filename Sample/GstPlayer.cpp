#include "StdAfx.h"
#include "GstPlayer.h"
#include <gst/interfaces/xoverlay.h>


GstPlayer::GstPlayer(void)
    : m_hWndId(0)
    , m_pLevelData(0)
    , OnLevel(0)
    , m_pVolume(0)
    , m_pAppsrcV(0)
    , m_pAppsrcA(0)
    , m_bSetVideoCaps(0)
    , m_bSetAudioCaps(0) {
}

GstPlayer::~GstPlayer(void) {
    Destroy();
}

void GstPlayer::PushVideo(lc_sample_raw_video_t* sample) {
    GstBuffer* buf = *(GstBuffer**)(sample + 1);
    buf = gst_buffer_ref(buf);
    gst_app_src_push_buffer(GST_APP_SRC(m_pAppsrcV), buf);
}

void GstPlayer::PushAudio(lc_sample_raw_audio_t* sample) {
    GstBuffer* buf = *(GstBuffer**)(sample + 1);
    buf = gst_buffer_ref(buf);
    gst_app_src_push_buffer(GST_APP_SRC(m_pAppsrcA), buf);
}


void GstPlayer::SetLevelCallback(void OnLevelCallback(int, double, gpointer), void* data) {
    OnLevel = OnLevelCallback;
    m_pLevelData = data;
}
void GstPlayer::OnDestroy() {
    if (m_pVolume) {
        gst_object_unref(m_pVolume);
        m_pVolume = NULL;
    }

    if (m_pAppsrcV) {
        gst_object_unref(m_pAppsrcV);
        m_pAppsrcV = NULL;
    }

    if (m_pAppsrcA) {
        gst_object_unref(m_pAppsrcA);
        m_pAppsrcA = NULL;
    }

    return GstPipelineBase::OnDestroy();
}

const char* GstPlayer::GetPipelineString(const std::map<const char*, const char*>& params) {
    static const char* const pipeline_str =
        " appsrc name=vsrc ! "
        " queue name=pre_v_queue max-size-bytes=0 max-size-buffers=8 max-size-time=0 leaky=0 ! "
        " colorspace ! {VIDEOSINK} sync=false async=false name=vsink "
        " appsrc name=asrc ! "
        " queue name=pre_a_queue max-size-bytes=0 max-size-buffers=25 max-size-time=0 leaky=1 ! "
        " level message=true interval=100000000 ! "
        " volume name=vol mute=false volume=0.8 ! audioconvert ! audioresample ! "
        " {AUDIOSINK} sync=false async=false name=asink ";

    if (!GstPipelineBase::Format(pipeline_str, params, m_strPipelineDesc)) {
        return NULL;
    } else {
        return m_strPipelineDesc;
    }
}

int GstPlayer::CanPause() {
    return FALSE;
}

int GstPlayer::CanSeek() {
    return FALSE;
}

int GstPlayer::OnBuilt() {
    m_pAppsrcV = gst_bin_get_by_name(GST_BIN(m_pGstPipeline), "vsrc");
    m_pAppsrcA = gst_bin_get_by_name(GST_BIN(m_pGstPipeline), "asrc");
    m_pVolume = gst_bin_get_by_name(GST_BIN(m_pGstPipeline), "vol");

    if (!(m_pAppsrcA && m_pAppsrcV && m_pVolume)) {
        return 0;
    }

    return GstPipelineBase::OnBuilt();
}


void GstPlayer::OnSyncGstMessage(GstBus* bus, GstMessage* message) {
    if (m_hWndId &&
            (GST_MESSAGE_TYPE(message) == GST_MESSAGE_ELEMENT) &&
            message->structure &&
            gst_structure_has_name(message->structure, "prepare-xwindow-id")) {
        GstElement* element = GST_ELEMENT(GST_MESSAGE_SRC(message));
        LogInfo(__FILE__, __FUNCTION__, "got prepare-xwindow-id, setting XID %lu", m_hWndId);

        if (g_object_class_find_property(G_OBJECT_GET_CLASS(element),
                                         "force-aspect-ratio")) {
            g_object_set(element, "force-aspect-ratio", TRUE, NULL);
        }

        gst_x_overlay_set_window_handle(GST_X_OVERLAY(element), (guintptr)m_hWndId);
    }
}

void GstPlayer::OnGstMessage(GstBus* bus, GstMessage* message) {
    if (message) {
        if (message->type == GST_MESSAGE_ELEMENT &&
                message->structure &&
                gst_structure_has_name(message->structure, "level")) {
            void (* callback)(int, gdouble, gpointer) = OnLevel;

            if (callback) {
                const GValue* vallist = gst_structure_get_value(message->structure, "rms");

                if (vallist) {
                    int ch = gst_value_list_get_size(vallist);

                    for (int i = 0; i < ch; i ++) {
                        const GValue* chVal = gst_value_list_get_value(vallist, i);

                        if (chVal) {
                            gdouble dbval = g_value_get_double(chVal);
                            callback(i, dbval, m_pLevelData);
                        }
                    }
                }
            }
        } else {
            GstPipelineBase::OnGstMessage(bus, message);
        }
    }
}

int GstPlayer::SetWindowId(guintptr id) {
    m_hWndId = id;
    return !!id;
}

void GstPlayer::SetVolume(float val, int mute) {
    if (m_pVolume) {
        if (mute) {
            g_object_set(m_pVolume, "mute", 1, NULL);
        } else {
            g_object_set(m_pVolume, "mute", 0, NULL);
        }

        g_object_set(m_pVolume, "volume", max(0, min(1.0f, val)), NULL);
    }
}
