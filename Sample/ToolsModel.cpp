#include "StdAfx.h"
#include "ToolsModel.h"

CToolsModel::CToolsModel(void):
    m_RTMPPlayUrl(""),
    m_HLSPlayUrl(""),
    m_UserRTMPUrl(""),
    m_BCESessionName(""),
    m_RtmpOption(TRUE),
    m_BCEUserDomain(""),
    m_BCEBucket(""),
    m_SaveLocal(FALSE),
    m_LocalSavePath(""),
    m_Status("就绪"),
    m_Preset(""),
    m_AudioDevice(""),
    m_VideoDevice(""),
    m_BCESessionId("") {
    m_nVersion = VERSION_INT;
}

CToolsModel::~CToolsModel(void) {
}


BOOL CToolsModel::Validate(CString& err) {
    if (m_VideoDevice.IsEmpty()) {
        err = "未设置视频源设备。";
        return FALSE;
    }

    if (m_AudioDevice.IsEmpty()) {
        err = "未设置音频源设备。";
        return FALSE;
    }

    if (m_Preset.IsEmpty()) {
        err = "没有选择编码模板";
        return FALSE;
    }


    if (m_SaveLocal) {
        if (m_LocalSavePath.IsEmpty()) {
            err = "没有设置本地保存文件名称";
            return FALSE;
        }
    }

    switch (m_RtmpOption) {
    case EXISTING_SESSION:
        if (m_BCESessionId.IsEmpty()) {
            err = "没有输入已经存在的SessionId";
            return FALSE;
        }

        break;

    case  CREATE_SESSION: {
        if (m_BCEBucket.IsEmpty()) {
            err = "没有输入BOS存储Bucket";
            return FALSE;
        }

        if (m_BCEUserDomain.IsEmpty()) {
            err = "没有输入BOS存储域名";
            return FALSE;
        }
    }
    break;

    case USER_RTMP: {
        if (m_UserRTMPUrl.IsEmpty()) {
            err = "没有输入用户RTMP地址";
            return FALSE;
        }
    }
    break;

    default:
        err = "没有选择RTMP配置";
        return FALSE;
    }

    return TRUE;
}

#define SERAILIZE_HELPER(__ar__, op) \
    __ar__ op m_VideoDevice \
    op m_AudioDevice \
    op m_Preset \
    op m_Status \
    op m_LocalSavePath \
    op m_SaveLocal \
    op m_BCEBucket \
    op m_BCEUserDomain \
    op m_RtmpOption \
    op m_BCESessionId \
    op m_BCESessionName \
    op m_UserRTMPUrl


void CToolsModel::Serailize() {
    CFile file;

    TRY {
        if (GetStorageFile(&file, 1)) {
            CArchive ar(&file, CArchive::store);
            ar << m_nVersion;
            SERAILIZE_HELPER(ar, <<);
            ar.Flush();
            file.Close();
        }
    }
    CATCH_ALL(e) {
        DELETE_EXCEPTION(e);
    }
    END_CATCH_ALL

}

void CToolsModel::Deserailize() {
    CFile file;

    TRY {
        if (GetStorageFile(&file, 0)) {
            CArchive ar(&file, CArchive::load);
            int version;
            ar >> version;

            if (m_nVersion == version) {
                SERAILIZE_HELPER(ar, >>);
            }

            ar.Flush();
            file.Close();
        }
    }
    CATCH_ALL(e) {
        DELETE_EXCEPTION(e);

    }
    END_CATCH_ALL
}

BOOL CToolsModel::GetStorageFile(CFile* file, BOOL write) {
    char* pgm = NULL;
    _get_pgmptr(&pgm);

    if (!pgm) {
        return FALSE;
    }

    CString strPath(pgm);
    int n = max(strPath.ReverseFind('/'), strPath.ReverseFind('\\'));

    if (n < 0) {
        return FALSE;
    }

    strPath.SetAt(n + 1, 0);
    strPath.ReleaseBuffer();

    strPath.Append("sample.dat");

    return file->Open(strPath,
                      write ? (CFile::modeWrite | CFile::modeCreate) : CFile::modeRead);
}