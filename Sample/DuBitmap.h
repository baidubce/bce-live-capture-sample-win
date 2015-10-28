#pragma once

#include "common.h"
#include <AfxTempl.h>

class CDuBitmap;
class CDuBitmapList;
class CDuBitmapCache;


class CDuBitmap {
public:
    class CData: public CBitmap {
    public:
        long m_lUserCount;

    public:
        inline CData() : m_lUserCount(0) {};

        void AddRef();
        CData* Release();
    };

public:
    CData*  m_pObject;

    inline CData* operator ->() {
        return m_pObject ? m_pObject : Create();
    }

public:
    inline CDuBitmap(): m_pObject(0) {

    }
    inline CDuBitmap(int nID): m_pObject(0) {
        Load(nID);
    }
    inline CDuBitmap(LPCSTR lpID): m_pObject(0) {
        Load(lpID);
    }
    inline CDuBitmap(CData& Data): m_pObject(&Data) {
        m_pObject->AddRef();
    }
    inline CDuBitmap(CDuBitmap& bmp): m_pObject(bmp.m_pObject) {
        m_pObject->AddRef();
    }

    inline ~CDuBitmap() {
        m_pObject = m_pObject->Release();
    }

public:
    inline void Assign(CData* pData) {
        m_pObject->Release();
        m_pObject = pData;
        m_pObject->AddRef();
    }

    inline CData* Create() {
        Assign(new CData);
        return m_pObject;
    }

    inline void Delete() {
        m_pObject->Release();
        m_pObject = NULL;
    }

    inline void operator =(CData& Data) {
        Assign(&Data);
    }
    inline void operator =(CDuBitmap&  Bmp) {
        Assign(Bmp.m_pObject);
    }

    inline int Valid()const {
        return (int)m_pObject;
    }

    int Load(int nID);
    int Load(LPCSTR lpID);
    int LoadDirect(int nID);
    int LoadDirect(LPCSTR lpID);

    CSize GetSize();

    inline operator CBitmap* () {
        return m_pObject;
    }
    inline operator HBITMAP() {
        return (HBITMAP)(*m_pObject);
    }
};

class CDuBitmapCache {
public:
    long __stdcall LockServer(long fLock);
    CDuBitmap __stdcall Load(int nID);
    CDuBitmap __stdcall Load(LPCSTR lpID);

public:
    CDuBitmapCache();
    ~CDuBitmapCache();

public:
    static CDuBitmapCache& GetDefBitmapCache();

public:
    struct CItem {
        CDuBitmap m_Bitmap;
        int m_nID;
        CString m_strID;
    };

    CArray<CItem, CItem> m_Cache;
    CCriticalSection m_Lock;

public:
    long ClearInvalid();

protected:
    int Find(int nID);
    int Find(LPCSTR lpID);
};


class CDuBitmapList : public CArray<CDuBitmap, CDuBitmap> {
public:
    typedef CArray<CDuBitmap, CDuBitmap> _BaseT;

public:
    inline _BaseT& Base() {
        return *this;
    }

    inline int Add(CDuBitmap bmp) {
        return bmp.Valid() ? _BaseT::Add(bmp) : -1;
    }
    inline int Add(LPCSTR lpID) {
        return Add(CDuBitmap(lpID));
    }
    inline int Add(int nID) {
        return Add(CDuBitmap(nID));
    }

    inline int SetAt(int nInd, CDuBitmap Bmp) {
        if (Bmp.Valid()) {
            _BaseT::SetAtGrow(nInd, Bmp);
        }

        return Bmp.Valid();
    }

    inline int SetAt(int nInd, LPCSTR lpID) {
        return SetAt(nInd, CDuBitmap(lpID));
    }
    inline int SetAt(int nInd, int nID) {
        return SetAt(nInd, CDuBitmap(nID));
    }

    inline int GetAt(int nInd, CDuBitmap& bmp) {
        int nRet = (nInd < _BaseT::GetSize());

        if (nRet) {
            bmp = _BaseT::GetAt(nInd);
        }

        return nRet;
    }
};
