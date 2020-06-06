#ifndef TABLE_H_
#define TABLE_H_


#define WHERE_STR "WHERE"
#define ID_STR "ID"
#define AGE_STR "AGE"
#define NAME_STR "NAME"
#define HEIGHT_STR "HEIGHT"
#define MAX_QUERY_LEN 100
#define MAX_STR_LEN 100

typedef enum 
{
    ID,
    AGE,
    HEIGHT,
    NAME
} FieldId;


/*
  * DB table has a fixed-sized table-row structure
*/
typedef struct {
  int id;
  int age;
  double height;
  char name[MAX_STR_LEN];
} T_Record;


void* get_col_by_id(T_Record* rec, FieldId id );
#endif /* TABLE_H_ */