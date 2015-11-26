#include "StdAfx.h"
#include "ConfigMgr.h"
#include "LogMgr.h"
#include <io.h>

ConfigMgr::ConfigMgr(void) {
    // 读取配置文件
    CString& config = GetConfigFile();


    if (!config.IsEmpty()) {
        // 如果配置文件不存在，创建默认的配置文件
        if (0 != _access(config, 0)) {
            CLogMgr::Instance().AppendLog(LC_LOG_ERROR, "配置文件不存存在，创建默认配置文件");

            static char s_default_config[] =
                "[system]\r\n"
                "access_key_id=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\r\n"
                "secret_access_key=bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\r\n"
                "host=http://lss.bj.baidubce.com\r\n";

            CFile file;
            TRY {
                if (!file.Open(config, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite)) {
                    CLogMgr::Instance().AppendLog(LC_LOG_ERROR, "创建默认配置文件失败");
                } else {
                    file.Write(s_default_config, sizeof(s_default_config));
                    file.Close();
                }
            }
            CATCH_ALL(e) {
                DELETE_EXCEPTION(e);
                CLogMgr::Instance().AppendLog(LC_LOG_ERROR, "创建默认配置文件异常");
            }
            END_CATCH_ALL
        }
    } else {
        CLogMgr::Instance().AppendLog(LC_LOG_ERROR, "加载配置文件失败，无法定位程序目录");
    }
}

ConfigMgr::~ConfigMgr(void) {
}


ConfigMgr& ConfigMgr::Instance() {
    static ConfigMgr g_inst;
    return g_inst;
}

// 根据程序目录，生成配置文件路径
CString& ConfigMgr::GetConfigFile() {
    if (m_strConfigFile.IsEmpty()) {
        char* pgm = NULL;
        _get_pgmptr(&pgm);

        if (!pgm) {
            return m_strConfigFile;
        }

        CString strPath(pgm);
        int n = max(strPath.ReverseFind('/'), strPath.ReverseFind('\\'));

        if (n < 0) {
            return m_strConfigFile;
        }

        strPath.SetAt(n + 1, 0);
        strPath.ReleaseBuffer();

        strPath.Append("config.ini");
        m_strConfigFile = strPath;
    }

    return m_strConfigFile;
}

void ConfigMgr::GetAk(CString& ak) {
    GetPrivateProfileString("system", "access_key_id", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
                            ak.GetBuffer(128), 128, m_strConfigFile);
    ak.ReleaseBuffer();
}
void ConfigMgr::GetSk(CString& sk) {
    GetPrivateProfileString("system", "secret_access_key", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
                            sk.GetBuffer(128), 128, m_strConfigFile);
    sk.ReleaseBuffer();

}
void ConfigMgr::GetHost(CString& host) {
    GetPrivateProfileString("system", "host", "http://lss.bj.baidubce.com",
                            host.GetBuffer(256), 256, m_strConfigFile);
    host.ReleaseBuffer();
}

void ConfigMgr::SetAk(CString& ak) {
    WritePrivateProfileString("system", "access_key_id", ak, m_strConfigFile);
}
void ConfigMgr::SetSk(CString& sk) {
    WritePrivateProfileString("system", "secret_access_key", sk, m_strConfigFile);

}
void ConfigMgr::SetHost(CString& host) {
    WritePrivateProfileString("system", "host", host, m_strConfigFile);
}
