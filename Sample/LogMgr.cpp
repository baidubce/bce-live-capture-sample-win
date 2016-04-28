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

// 添加日志,如果日志数大于最大容量，删除最旧的日志
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

//设置日志变动通知窗口句柄，当日志添加，清除，过滤时，通知窗口
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

// 设置日志级别
LC_LOGLEVEL CLogMgr::GetFilterLevel() {
    LC_LOGLEVEL level;
    m_lock.Lock();

    level = m_eLogFilter;
    m_lock.Unlock();
    return level;
}

// 清除所有日志
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

// 过滤日志，根据FILTER级别过滤日志，结果保存在msg中
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

// 另存为文件
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