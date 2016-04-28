#include "stdafx.h"
#include "DuBitmap.h"

void CDuBitmap::CData::AddRef() {
    if (!this) {
        return ;
    }

    InterlockedIncrement(&m_lUserCount);
}

CDuBitmap::CData* CDuBitmap::CData::Release() {
    if (!this) {
        return NULL;
    }

    if (0 == InterlockedDecrement(&m_lUserCount)) {
        delete this;
        return NULL;
    }

    return this;
}

CSize CDuBitmap::GetSize() {
    CSize size(0, 0);

    if (m_pObject) {
        BITMAP bitmapColon;
        m_pObject->GetBitmap(&bitmapColon);
        size.cx = bitmapColon.bmWidth;
        size.cy = bitmapColon.bmHeight;
    }

    return size;
}


int CDuBitmap::LoadDirect(int nID) {
    ASSERT(nID != 0);

    Create();

    if (!(m_pObject->LoadBitmap(nID))) {
        m_pObject->Release();
        m_pObject = NULL;
    }

    return Valid();
}

int CDuBitmap::LoadDirect(LPCTSTR lpID) {
    ASSERT(lpID);

    Create();

    if (!(m_pObject->LoadBitmap(lpID))) {
        m_pObject->Release();
        m_pObject = NULL;
    }

    return Valid();
}


int CDuBitmap::Load(int nID) {
    if (nID == 0) {
        return FALSE;
    }

    CDuBitmap bmp = CDuBitmapCache::GetDefBitmapCache().Load(nID);

    if (bmp.Valid()) {
        *this = bmp;
    }

    return bmp.Valid();
}

int CDuBitmap::Load(LPCTSTR lpID) {
    if (lpID == 0) {
        return FALSE;
    }

    CDuBitmap bmp =  CDuBitmapCache::GetDefBitmapCache().Load(lpID);

    if (bmp.Valid()) {
        *this = bmp;
    }

    return bmp.Valid();
}


int CDuBitmapCache::Find(int nID) {
    for (int i = 0; i < m_Cache.GetSize(); i++) {
        if (m_Cache[i].m_nID == nID) {
            return i;
        }
    }

    return -1;
}


int CDuBitmapCache::Find(LPCTSTR lpID) {
    for (int i = 0; i < m_Cache.GetSize(); i++) {
        if (m_Cache[i].m_strID == lpID) {
            return i;
        }
    }

    return -1;
}

CDuBitmap __stdcall CDuBitmapCache::Load(int nID) {
    if (nID == 0) {
        return CDuBitmap();
    }

    m_Lock.Lock();

    int nInd = Find(nID);

    CDuBitmap Bmp;

    if (nInd < 0) {
        int nRet = Bmp.LoadDirect(nID);

        if (nRet) {
            CItem Item;
            Item.m_Bitmap = Bmp;
            Item.m_nID = nID;
            m_Cache.Add(Item);
        }

    } else {
        Bmp = m_Cache[nInd].m_Bitmap;

        if (! Bmp.Valid()) {
            Bmp.LoadDirect(nID);
        }
    }

    m_Lock.Unlock();

    return Bmp;
}


CDuBitmap __stdcall CDuBitmapCache::Load(LPCTSTR lpID) {
    if (lpID == 0) {
        return CDuBitmap();
    }

    m_Lock.Lock();

    int nInd = Find(lpID);

    CDuBitmap Bmp;

    if (nInd < 0) {
        int nRet = Bmp.LoadDirect(lpID);

        if (nRet) {
            CItem Item;
            Item.m_Bitmap = Bmp;
            Item.m_strID = lpID;
            m_Cache.Add(Item);
        }

    } else {
        Bmp = m_Cache[nInd].m_Bitmap;

        if (! Bmp.Valid()) {
            Bmp.LoadDirect(lpID);
        }
    }

    m_Lock.Unlock();

    return Bmp;
}

CDuBitmapCache& CDuBitmapCache::GetDefBitmapCache() {
    static CDuBitmapCache Cache;
    return Cache;
}

CDuBitmapCache::CDuBitmapCache() {
}

CDuBitmapCache::~CDuBitmapCache() {
}

long __stdcall CDuBitmapCache::LockServer(long fLock) {
    if (fLock) {
        m_Lock.Lock();
    } else {
        m_Lock.Unlock();
    }

    return TRUE;
}

long CDuBitmapCache::ClearInvalid() {
    long nCount = 0;

    m_Lock.Lock();

    for (int i = m_Cache.GetSize() - 1; i >= 0; i--) {
        if (m_Cache[i].m_Bitmap.Valid())
            if (m_Cache[i].m_Bitmap->m_lUserCount > 1) {
                continue;
            }

        nCount ++;
        m_Cache.RemoveAt(i);
    }

    m_Lock.Unlock();

    return nCount;
}