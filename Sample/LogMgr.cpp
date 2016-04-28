#include "StdAfx.h"
#include "LogMgr.h"

CLogMgr::CLogMgr(void)
    : m_eLogFilter(LC_LOG_DEFAULT_LEVEL)
    , m_hWnd(0) {
}

CLogMgr::~CLogMgr(void) {
    m_hWnd = 0;
    ClearLog();
}

CLogMgr& CLogMgr::Instance() {
    static CLogMgr g_inst;
    return g_inst;
}

// �����־,�����־���������������ɾ����ɵ���־
void CLogMgr::AppendLog(LC_LOGLEVEL level, LPCTSTR msg) {
    USES_CONVERSION;
    LogItem* item = new LogItem();
    item->level = level;
    item->msg = T2A(msg);
    item->ref = 1;
    HWND hwnd = NULL;
    m_lock.Lock();

    m_lstItems.push_back(item);

    while (m_lstItems.size() > MAX_LOG_RECORD) {
        ReleaseLog(m_lstItems.front());
        m_lstItems.erase(m_lstItems.begin());
    }

    hwnd = m_hWnd;
    m_lock.Unlock();

    if (hwnd) {
        AddRefLog(item);

        if (!PostMessage(hwnd, WM_LOG_ADD, 0, (LPARAM)item)) {
            ReleaseLog(item);
        }
    }
}

//������־�䶯֪ͨ���ھ��������־��ӣ����������ʱ��֪ͨ����
void CLogMgr::SetNotifyHwnd(HWND hNotify) {
    m_lock.Lock();
    m_hWnd = hNotify;
    m_lock.Unlock();

}
void CLogMgr::SetFilterLevel(LC_LOGLEVEL filter) {
    int setted = 0;
    HWND hwnd = NULL;
    m_lock.Lock();

    if (m_eLogFilter != filter) {
        m_eLogFilter = filter;
        lc_log_set_level(m_eLogFilter);
        setted = 1;
    }

    hwnd = m_hWnd;

    m_lock.Unlock();

    if (setted) {
        if (hwnd) {
            PostMessage(hwnd, WM_LOG_SETFILTER, 0, filter);
        }
    }
}

// ������־����
LC_LOGLEVEL CLogMgr::GetFilterLevel() {
    LC_LOGLEVEL level;
    m_lock.Lock();

    level = m_eLogFilter;
    m_lock.Unlock();
    return level;
}

// ���������־
void CLogMgr::ClearLog() {
    HWND hwnd = NULL;

    m_lock.Lock();

    for (std::list<LogItem*>::const_iterator it = m_lstItems.begin();
            it != m_lstItems.end();
            it ++) {
        ReleaseLog(*it);
    }

    m_lstItems.clear();
    hwnd = m_hWnd;
    m_lock.Unlock();

    if (hwnd) {
        PostMessage(hwnd, WM_LOG_CLEAR, 0, 0);
    }
}

// ������־������FILTER���������־�����������msg��
void CLogMgr::FilterLog(LC_LOGLEVEL filter, CString& msg) {
    m_lock.Lock();

    msg.Empty();

    for (std::list<LogItem*>::const_iterator it = m_lstItems.begin();
            it != m_lstItems.end();
            it ++) {
        if ((*it)->level >= filter) {
            msg += (*it)->msg.c_str();
            msg += _T("\r\n");
        }
    }

    m_lock.Unlock();
}

void CLogMgr::ReleaseLog(LogItem* item) {
    if (0 == InterlockedDecrement(&(item->ref))) {
        delete item;
    }
}
void CLogMgr::AddRefLog(LogItem* item) {
    InterlockedIncrement(&(item->ref));
}

// ���Ϊ�ļ�
BOOL CLogMgr::SaveAs(CString& path) {

    FILE* file = NULL;
    _tfopen_s(&file, path, _T("w"));

    if (file) {

        m_lock.Lock();

        for (std::list<LogItem*>::const_iterator it = m_lstItems.begin();
                it != m_lstItems.end();
                it ++) {
            fwrite((*it)->msg.c_str(), 1, (*it)->msg.length(), file);
            fwrite("\r\n", 1, 2, file);
        }

        m_lock.Unlock();
        fclose(file);
        return TRUE;
    } else {
        return FALSE;
    }
}