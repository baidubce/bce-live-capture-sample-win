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
    m_BCESessionId(""),
    m_bTwoVideoSrc(0),
    m_VideoDevice2(""),
    m_ptVideo(0, 0),
    m_ptVideo2(32, 32),
    m_szVideo(0, 0),
    m_szVideo2(200, 150) {
    m_nVersion = VERSION_INT;
}

CToolsModel::~CToolsModel(void) {
}


BOOL CToolsModel::Validate(CString& err) {
    if (m_VideoDevice.IsEmpty()) {
        err = "未设置视频源设备。";
        return FALSE;
    }

    if (m_bTwoVideoSrc) {
        if (m_VideoDevice2.IsEmpty()) {
            err = "未设置视频源设备2。";
            return FALSE;
        }

        if (m_ptVideo.x < 0) {
            err = "主视频水平位置不能小于0。";
            return FALSE;
        }

        if (m_ptVideo.y < 0) {
            err = "主视频垂直位置不能小于0。";
            return FALSE;
        }

        if (m_ptVideo.x > 3968) {
            err = "主视频水平位置不能大于3968。";
            return FALSE;
        }

        if (m_ptVideo.y > 2976) {
            err = "主视频垂直位置不能大于2967。";
            return FALSE;
        }

        if (m_ptVideo2.x < 0) {
            err = "视频2水平位置不能小于0。";
            return FALSE;

        }

        if (m_ptVideo2.y < 0) {
            err = "视频2垂直位置不能小于0。";
            return FALSE;

        }

        if (m_ptVideo2.x > 3968) {
            err = "视频2水平位置不能大于3968。";
            return FALSE;

        }

        if (m_ptVideo2.y > 2976) {
            err = "视频2垂直位置不能大于2967。";
            return FALSE;

        }

        if (m_szVideo.cx < 0) {
            err = "主视频宽不能小于0。";
            return FALSE;
        }

        if (m_szVideo.cy < 0) {
            err = "主视频高不能小于0。";
            return FALSE;

        }

        if (m_szVideo.cx > 0 && m_szVideo.cx < 128) {
            err = "主视频宽不能小于128。";
            return FALSE;

        }

        if (m_szVideo.cy > 0 && m_szVideo.cy < 96) {
            err = "主视频高不能小于96。";
            return FALSE;
        }

        if (m_ptVideo.x + m_szVideo.cx > 4096) {
            err = "主视频水平位置不能超出4096。";
            return FALSE;
        }

        if (m_ptVideo.y + m_szVideo.cy > 3072) {
            err = "主视频垂直位置不能超出3072。";
            return FALSE;
        }

        if (m_szVideo2.cx < 0) {
            err = "视频2宽不能小于0。";
            return FALSE;
        }

        if (m_szVideo2.cy < 0) {
            err = "视频2高不能小于0。";
            return FALSE;
        }

        if (m_szVideo2.cx > 0 && m_szVideo2.cx < 128) {
            err = "视频2宽不能小于128。";
            return FALSE;
        }

        if (m_szVideo2.cy > 0 && m_szVideo2.cy < 96) {
            err = "视频2高不能小于96。";
            return FALSE;
        }

        if (m_ptVideo2.x + m_szVideo2.cx > 4096) {
            err = "视频2水平位置不能超出4096。";
            return FALSE;
        }

        if (m_ptVideo2.y + m_szVideo2.cy > 3072) {
            err = "视频2垂直位置不能超出3072。";
            return FALSE;
        }
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
    op m_UserRTMPUrl \
    op m_bTwoVideoSrc \
    op m_VideoDevice2 \
    op m_ptVideo \
    op m_ptVideo2 \
    op m_szVideo \
    op m_szVideo2


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