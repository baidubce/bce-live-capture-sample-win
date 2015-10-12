#include "stdafx.h"
#include "property_grid_helper.h"

#define kUtcTimeFormat "%Y-%m-%dT%H:%M:%SZ"

void read_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    CString str;
    int len = item->Size;

    if (len > 0 && len < MAXINT / 2) {
        strncpy_s(str.GetBuffer(len), len, (char*)obj + item->Offset, -1);
        str.ReleaseBuffer();
        COleVariant var(str);

        prop->SetValue(var);
    }
}

void write_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    CString svalue = prop->FormatProperty();
    int len = item->Size;

    if (len > 0 && len < MAXINT / 2) {
        strncpy_s((char*)obj + item->Offset, len, (LPCSTR)svalue, -1);
    }
}

void read_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {
    long value = 0;
    memcpy(&value, (char*)obj + item->Offset, item->Size);

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
            strLog.Format("%d", value);
            COleVariant var(strLog);
            prop->SetValue(var);
        }
    } else {
        CString strLog;
        strLog.Format("%d", value);
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
            if (_stricmp(cur->Name, (LPCSTR)svalue) == 0) {
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
        int val = atol(svalue);
        memcpy((char*)obj + item->Offset, &val, item->Size);
    }
}

static void
safe_read_string_array(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop,
                       CString& strmsg) {
    __try {
        struct {
            int count;
            char** array;
        } items = { 0 };
        memcpy(&items, (char*)obj + item->Offset, sizeof(items));

        for (int i = 0 ; i < items.count && i < 10 ; i ++) {
            strmsg.AppendFormat("%s\r\n", items.array[i]);
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

void read_ll_time(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop) {

    long long time = 0;
    memcpy(&time, (char*)obj + item->Offset, item->Size);
    char buffer[128] =  { 0 };

    if (time > 0) {
        struct tm result_tm = { 0 };
        localtime_s(&result_tm, &time);

        int size = strftime(buffer, 128, kUtcTimeFormat, &result_tm);
    }

    COleVariant var(buffer);
    prop->SetValue(var);
}
