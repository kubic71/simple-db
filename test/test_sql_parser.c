#include "acutest.h"
#include "SQL_parser.h"
#include <stdlib.h>

void test_example(void)
{
    void *mem;
    int a, b;

    mem = malloc(10);
    TEST_CHECK(mem != NULL);

    mem = realloc(mem, 20);
    TEST_CHECK(mem != NULL);
}

void test_parse_select(void)

{
    Select_Query query;
    parse_select("SELECT * WHERE age >= 18", &query);

    Constraint* c = &query.constraint;
    TEST_CHECK(c->fieldId == AGE);
    TEST_CHECK(c->comparator == GREATER_OR_EQUAL_TO);
    TEST_CHECK(c->fieldVal.val.age == 18);
}

TEST_LIST = {
    {"test_example", test_example},
    {"test_parse_select", test_parse_select},
    {0} /* Must be terminated with {0} */
};