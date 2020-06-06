#include "table.h"
#include "SQL_parser.h"
#include <stdlib.h>

void* get_col_by_id(T_Record* rec, FieldId id) {
    switch (id)
    {
    case ID:
        return &rec->id;
    
    case AGE:
        return &rec->age;
    
    case HEIGHT:
        return &rec->height;
    
    case NAME:
        return rec->name;
    } 

    return NULL;
}
