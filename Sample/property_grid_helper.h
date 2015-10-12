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
};

void read_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void write_string(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void read_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void write_int(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void read_string_array(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);
void read_ll_time(void* obj, const PropertyItem* item, CMFCPropertyGridProperty* prop);

#endif //TOOLS_PROPERTY_GRID_HELPER_H