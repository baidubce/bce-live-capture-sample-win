#pragma once


typedef enum _tagRTMP_OPTION {
    EXISTING_SESSION = 0,
    CREATE_SESSION,
    USER_RTMP
} RTMP_OPTION;


typedef enum _tagMODEL_VERSION {
    VERSION_INT = 0xABCDEF02
} MODEL_VERSION;

class CToolsModel {
public:
    CToolsModel(void);
    ~CToolsModel(void);
public:
    BOOL Validate(CString& err);
    void Serailize();
    void Deserailize();

public:
    CString m_RTMPPlayUrl;
    CString m_HLSPlayUrl;
    CString m_UserRTMPUrl;
    CString m_BCESessionName;
    CString m_BCEUserDomain;
    CString m_BCEBucket;
    CString m_LocalSavePath;
    CString m_Status;
    CString m_Preset;
    CString m_AudioDevice;
    CString m_VideoDevice;
    CString m_BCESessionId;
    int m_nVersion;
    int m_RtmpOption;
    BOOL m_SaveLocal;

    BOOL m_bTwoVideoSrc;
    CString m_VideoDevice2;
    CPoint m_ptVideo;
    CPoint m_ptVideo2;
    CSize m_szVideo;
    CSize m_szVideo2;
private:
    BOOL GetStorageFile(CFile* file, BOOL write);
};
