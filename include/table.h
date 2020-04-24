#ifndef TABLE_H_
#define TABLE_H_

#define MAX_STR_LEN 100


/*
  * DB table has a fixed-sized table-row structure
*/
typedef struct {
  int id;
  int age;
  double height;
  int income;

  char name[MAX_STR_LEN];
  char profession[MAX_STR_LEN];
  char nationality[MAX_STR_LEN];
  
} T_Record;

#endif /* TABLE_H_ */