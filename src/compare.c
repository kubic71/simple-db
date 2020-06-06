#include "compare.h"
#include <string.h>
#include "SQL_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool int_cmp(Comparator comp, int i1, int i2)
{
    switch (comp)
    {
    case GREATER_OR_EQUAL:
        return i1 >= i2;

    case GREATER:
        return i1 > i2;

    case LOWER_OR_EQUAL:
        return i1 <= i2;
    case LOWER:
        return i1 < i2;

    case EQUAL:
        return i1 == i2;

    default:
        perror("Invalid int comparator");
        exit(EXIT_FAILURE);
    }
}

bool double_cmp(Comparator comp, double d1, double d2)
{
    switch (comp)
    {
    case GREATER_OR_EQUAL:
        return d1 >= d2;
        
    case GREATER:
        return d1 > d2;
        
    case LOWER_OR_EQUAL:
        return d1 <= d2;

    case LOWER:
        return d1 < d2;

    case EQUAL:
        return abs(d1 - d2) < 0.0001;

    default:
        perror("Invalid double comparator");
            exit(EXIT_FAILURE);
    }
}

bool string_cmp(Comparator comp, char* s1, char* s2)
{
    switch (comp)
    {
    case GREATER_OR_EQUAL:
        return strcmp(s1, s2) >= 0;
        
    case GREATER:
        return strcmp(s1, s2) > 0;
        
    case LOWER_OR_EQUAL:
        return strcmp(s1, s2) <= 0;
        
    case LOWER:
        return strcmp(s1, s2) < 0;

    case EQUAL:
        return strcmp(s1, s2) == 0;

    default:
        perror("Invalid string comparator");
            exit(EXIT_FAILURE);
    }
}

bool satisfy_constraint(T_Record *rec, Constraint *c)
{
    if (c->fieldId == NAME)
    {
        //  return string_cmp(c->comparator, rec->name, (char*) c->fieldVal);
        return string_cmp(c->comparator, rec->name, c->fieldVal.name);
    }
    else if (c->fieldId == ID)
    {
        return int_cmp(c->comparator, rec->id, c->fieldVal.id);
    }
    else if (c->fieldId == AGE)
    {
        return int_cmp(c->comparator, rec->age, c->fieldVal.age);
    }
    else if (c->fieldId == HEIGHT)
    {
        return double_cmp(c->comparator, rec->height, c->fieldVal.height);
    }
    return false;
}

