#ifndef __NOTIFICATION_H__
#define __NOTIFICATION_H__

// notification property map
static const PropertyItem s_notification_items[] = {
    {
        _T("֪ͨ����"),
        offsetof(lc_notification_t, name),
        LC_MAX_NOTIFICATION_NAME_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string
    },
    {
        _T("�ص���ַ"),
        offsetof(lc_notification_t, endpoint),
        LC_MAX_URL_LEN,
        read_string,
        write_string,
        NULL,
        NULL,
        NULL,
        validate_string
    },
    {
        _T("����ʱ��"),
        offsetof(lc_notification_t, create_time),
        sizeof(lc_datetime_t),
        read_lc_datetime,
        NULL,
        NULL,
        NULL,
        NULL,
        validate_lc_datetime
    },
    { 0, }
};

#endif //__NOTIFICATION_H__