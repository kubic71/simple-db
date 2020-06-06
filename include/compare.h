#ifndef COMPARE_H
#define COMPARE_H

#include "SQL_parser.h"
#include <stdbool.h>

bool int_cmp(Comparator comp, int i1, int i2);
bool double_cmp(Comparator comp, double d1, double d2);
bool string_cmp(Comparator comp, char* s1, char* s2);
bool satisfy_constraint(T_Record* rec, Constraint* c); 

#endif
