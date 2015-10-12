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
    m_Status("����"),
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
        err = "δ������ƵԴ�豸��";
        return FALSE;
    }

    if (m_AudioDevice.IsEmpty()) {
        err = "δ������ƵԴ�豸��";
        return FALSE;
    }

    if (m_Preset.IsEmpty()) {
        err = "û��ѡ�����ģ��";
        return FALSE;
    }


    if (m_SaveLocal) {
        if (m_LocalSavePath.IsEmpty()) {
            err = "û�����ñ��ر����ļ�����";
            return FALSE;
        }
    }

    switch (m_RtmpOption) {
    case EXISTING_SESSION:
        if (m_BCESessionId.IsEmpty()) {
            err = "û�������Ѿ����ڵ�SessionId";
            return FALSE;
        }

        break;

    case  CREATE_SESSION: {
        if (m_BCEBucket.IsEmpty()) {
            err = "û������BOS�洢Bucket";
            return FALSE;
        }

        if (m_BCEUserDomain.IsEmpty()) {
            err = "û������BOS�洢����";
            return FALSE;
        }
    }
    break;

    case USER_RTMP: {
        if (m_UserRTMPUrl.IsEmpty()) {
            err = "û�������û�RTMP��ַ";
            return FALSE;
        }
    }
    break;

    default:
        err = "û��ѡ��RTMP����";
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