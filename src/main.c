#include <stdio.h>
#include "SQL_parser.h"
#include "table.h"

int main() {
   SQL_Query test_q;

   parse_SQL("this is test string", &test_q);

   if(test_q.type == INSERT) {
      T_Record* rec = &test_q.query.insert_q.record;

      printf("id: %d, age: %d, height: %f, name:%s\n", rec->id, rec->age, rec->height, rec->name);
   }

   return 0;
}
