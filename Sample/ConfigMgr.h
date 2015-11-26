#pragma once

class ConfigMgr {
protected:
    ConfigMgr(void);
public:
    ~ConfigMgr(void);

public:
    static ConfigMgr& Instance();

private:
    CString m_strConfigFile;
    CString& GetConfigFile();

public:
    void GetAk(CString& ak);
    void GetSk(CString& sk);
    void GetHost(CString& host);

    void SetAk(CString& ak);
    void SetSk(CString& sk);
    void SetHost(CString& host);
};
