#include "stdafx.h"
#include "property_grid_helper.h"


#define kUtcTimeFormat "%Y-%m-%dT%H:%M:%SZ"

void read_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    USES_CONVERSION;

    CString str;
    int len = item->Size;

    if (len > 0 && len < MAXINT / 2) {
        _tcsncpy_s(str.GetBuffer(len), len, A2T((char*)obj + item->Offset), -1);
        str.ReleaseBuffer();
        COleVariant var(str);

        prop->SetValue(var);
    }
}

void write_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    USES_CONVERSION;
    CString svalue = prop->FormatProperty();
    int len = item->Size;

    if (len > 0 && len < MAXINT / 2) {
        strncpy_s((char*)obj + item->Offset, len, T2A(svalue), -1);
    }
}

BOOL validate_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    USES_CONVERSION;
    CString str;
    int len = item->Size;

    if (len > 0 && len < MAXINT / 2) {
        _tcsncpy_s(str.GetBuffer(len), len, A2T((char*)obj + item->Offset), -1);
        str.ReleaseBuffer();
        str.Trim();
        return str.GetLength() > 0;
    } else {
        return FALSE;
    }
}

void read_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    long value = 0;
    memcpy(&value, (char*)obj + item->Offset, min(sizeof(long), item->Size));

    if (item->Options) {
        const PropertyNameValue* cur = item->Options;

        int setted = 0;

        while (cur && cur->Name) {
            if (cur->Value == value) {
                COleVariant var(cur->Name);
                prop->SetValue(var);
                setted = 1;
                break;
            }

            cur ++;
        }

        if (!setted) {
            CString strLog;
            strLog.Format(_T("%d"), value);
            COleVariant var(strLog);
            prop->SetValue(var);
        }
    } else {
        CString strLog;
        strLog.Format(_T("%d"), value);
        COleVariant var(strLog);
        prop->SetValue(var);
    }
}

void write_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    CString svalue = prop->FormatProperty();

    if (item->Options) {
        int setted = 0;
        const PropertyNameValue* cur = item->Options;

        while (cur->Name) {
            if (_tcsicmp(cur->Name, (LPCTSTR)svalue) == 0) {
                memcpy((char*)obj + item->Offset, &cur->Value, item->Size);
                setted = 1;
                break;
            }

            cur++;
        }

        if (!setted) {
            memcpy((char*)obj + item->Offset, &item->Options->Value, item->Size);
        }
    } else {
        int val = _ttol(svalue);
        memcpy((char*)obj + item->Offset, &val, item->Size);
    }
}

BOOL validate_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    char* idx = (char*)obj + item->Offset;

    for (int i = 0; i < item->Size; i ++) {
        if (idx[i] != -1) {
            return TRUE;
        }
    }

    return FALSE;
}

static void
safe_read_string_array(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop,
                       CString& strmsg) {

    USES_CONVERSION;

    __try {
        struct {
            int count;
            char** array;
        } items = { 0 };
        memcpy(&items, (char*)obj + item->Offset, sizeof(items));

        for (int i = 0 ; i < items.count && i < 10 ; i ++) {
            strmsg.AppendFormat(_T("%s\r\n"), A2T(items.array[i]));
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {

    }
}
void read_string_array(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    CString msg;
    safe_read_string_array(obj, item, prop, msg);

    COleVariant var(msg);
    prop->SetValue(var);
}

void read_lc_datetime(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {

    lc_datetime_t time = {0};
    memcpy(&time, (char*)obj + item->Offset, min(sizeof(lc_datetime_t), item->Size));
    TCHAR buffer[128] =  { 0 };

    _stprintf_s(buffer, _T("%04d-%02d-%02dT%02d:%02d:%02dZ"), time.year, time.month, time.day,
                time.hour,
                time.minute, time.second);
    COleVariant var(buffer);
    prop->SetValue(var);
}


BOOL validate_lc_datetime(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    lc_datetime_t time = {0};
    memcpy(&time, (char*)obj + item->Offset, min(sizeof(lc_datetime_t), item->Size));

    return time.day > 0 || time.month > 0 || time.year > 0 ||
           time.hour > 0 || time.minute > 0 || time.second > 0;
}



BOOL validate_bool(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    char val = *((char*)obj + item->Offset);

    return val > 0;
}

BOOL validate_subitems(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    if (item->SubItems) {
        const PropertyItem* subs = item->SubItems;

        while (subs->Name) {
            if (subs->Validate) {
                if (subs->Validate(obj, subs, NULL)) {
                    return TRUE;
                }
            } else {
                return TRUE;
            }

            subs ++;
        }

        return FALSE;
    } else {
        return FALSE;
    }
}

void write_expended(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    bool* pval = (bool*)((char*)obj + item->Offset);

    if (prop->IsGroup()) {
        if (prop->IsExpanded()) {
            *pval = true;
        } else {
            *pval = false;
        }
    } else {
        *pval = false;
    }
}