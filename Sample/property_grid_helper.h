#ifndef TOOLS_PROPERTY_GRID_HELPER_H
#define TOOLS_PROPERTY_GRID_HELPER_H

typedef struct _tagPropertyItem PropertyItem;
typedef struct _tagPropertyNameValue PropertyNameValue;
struct _tagPropertyNameValue {
    LPCTSTR Name;
    long Value;
};

struct _tagPropertyItem {
    LPCTSTR Name;
    int Offset;
    int Size;
    void (*Read)(void* obj, const PropertyItem*, CMFCPropertyGridProperty*);
    void (*Write)(void* obj, const PropertyItem*, CMFCPropertyGridProperty*);
    void* Args;
    const PropertyNameValue* Options;
    const PropertyItem* SubItems;
    BOOL (*Validate)(void* obj, const PropertyItem*, CMFCPropertyGridProperty*);
};

void read_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void write_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
BOOL validate_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

void read_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void write_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
BOOL validate_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

void read_string_array(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

void read_lc_datetime(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
BOOL validate_lc_datetime(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

BOOL validate_bool(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);


BOOL validate_subitems(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

void write_expended(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

#endif //TOOLS_PROPERTY_GRID_HELPER_H