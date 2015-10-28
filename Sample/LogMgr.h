#pragma once
#include <live_capture.h>
#include <string>
#include <list>
#include <windows.h>

typedef struct _tagLogItem{
    LC_LOGLEVEL level;
    std::string msg;
    volatile __declspec(align(4)) long ref;
}LogItem;

typedef enum tagWM_LOG
{
    WM_LOG_ADD = WM_APP+1,
    WM_LOG_CLEAR,
    WM_LOG_SETFILTER,
}WM_LOG;

// ��־�����࣬����ģʽ
class CLogMgr
{
    enum { MAX_LOG_RECORD = 2048 };
protected:
    CLogMgr(void);
public:
    ~CLogMgr(void);
public:
    static CLogMgr& Instance();
private:
    std::list<LogItem*> m_lstItems;
    LC_LOGLEVEL m_eLogFilter;
    HWND m_hWnd;
    CCriticalSection m_lock;
public:
    //������־�䶯֪ͨ���ھ��������־��ӣ����������ʱ��֪ͨ����
    void SetNotifyHwnd(HWND hNotify);

    // ������־����
    void SetFilterLevel(LC_LOGLEVEL filter);

    // ���Ϊ�ļ�
    BOOL SaveAs(CString& path);
    
    // ��ȡ��־����
    LC_LOGLEVEL GetFilterLevel();

    // ���������־
    void ClearLog();

    // ������־������FILTER���������־�����������msg��
    void FilterLog(LC_LOGLEVEL filter, CString& msg);

    // �����־
    void AppendLog(LC_LOGLEVEL level, const char* msg);

    // �ͷ���־
    void ReleaseLog(LogItem* item);

    // ����һ�����ü���
    void AddRefLog(LogItem* item);
};

#define LOG_IMPLE(_NAME_, _LEVEL_) \
static void __inline _NAME_ (const char* __mod__, const char* __title__, const char* __fmt__, ...) {\
    va_list list;\
    va_start(list, __fmt__);\
    CString msg;\
    msg.Format("%s\t%s\t", __mod__, __title__);\
    msg.AppendFormatV(__fmt__, list);\
    va_end(list);\
    CLogMgr::Instance().AppendLog(_LEVEL_, msg);\
}

LOG_IMPLE(LogError, LC_LOG_ERROR);
LOG_IMPLE(LogInfo, LC_LOG_INFO);
LOG_IMPLE(LogWarning, LC_LOG_WARN);
