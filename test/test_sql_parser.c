#include "acutest.h"
#include "SQL_parser.h"
#include <stdlib.h>

void test_parse_constraint_id(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE id == 3", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == ID);
    TEST_CHECK(c.comparator == EQUAL);
    TEST_CHECK(c.fieldVal.id == 3);
}

void test_parse_constraint_age(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE age >= 18", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == AGE);
    TEST_CHECK(c.comparator == GREATER_OR_EQUAL);
    TEST_CHECK(c.fieldVal.age == 18);
}

void test_parse_constraint_height(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE height < 185", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == HEIGHT);
    TEST_CHECK(c.comparator == LOWER);
    TEST_CHECK(c.fieldVal.height == 185);
}

void test_parse_constraint_name(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE name == 'Joe'", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == NAME);
    TEST_CHECK(c.comparator == EQUAL);
    TEST_CHECK(strcmp(c.fieldVal.name, "Joe") == 0);
}

void test_parse_constraint_fail_invalid_field(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE notvalidfield == 3", &c);
    TEST_CHECK(!succ);
}

void test_parse_coinstraint_fail_invalid_val(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE id >= lrsi", &c);
    TEST_CHECK(!succ);
}

void test_parse_select_1(void)
{
    Select_Query query;
    bool succ = parse_select("SELECT * WHERE age >= 18", &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.all == false);

    Constraint* c = &query.constraint;
    TEST_CHECK(c->fieldId == AGE);
    TEST_CHECK(c->comparator == GREATER_OR_EQUAL);
    TEST_CHECK(c->fieldVal.age == 18);
}

void test_parse_select_all(void)
{
    Select_Query query;
    bool succ = parse_select("SELECT * ", &query);
    TEST_CHECK(succ);
    TEST_CHECK(query.all == true);
}


void test_parse_insert(void) {
    Insert_Query query;
    bool succ = parse_insert("INSERT(2, 21, 168.23, 'Joe Brown')",&query);
    TEST_CHECK(succ);
    
    TEST_CHECK(query.record.id == 2);
    TEST_CHECK(query.record.age = 21);
    TEST_CHECK(query.record.height == 168.23);
    TEST_CHECK(strcmp(query.record.name, "Joe Brown") == 0);
}


void test_parse_delete(void) {
    Delete_Query query;
    bool succ = parse_delete("DELETE WHERE id == 2", &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.constraint.fieldId == ID);
    TEST_CHECK(query.constraint.comparator == EQUAL);
    TEST_CHECK(query.constraint.fieldVal.id == 2);    
}


void test_parse_update(void) {
    Update_Query query;
    bool succ = parse_update("UPDATE SET height=183.3 WHERE id == 15", &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.fieldId == HEIGHT);
    TEST_CHECK(query.val.height == 183.3);

    TEST_CHECK(query.constraint.fieldId == ID);
    TEST_CHECK(query.constraint.comparator == EQUAL);
    TEST_CHECK(query.constraint.fieldVal.id == 15);
}


void test_parse_sql_query(void) {
    // test the parse_SQL method as a whole
    SQL_Query query;
    bool succ = parse_SQL("SELECT * WHERE height > 180.1", &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.type == SELECT);
    Select_Query* s_q = &query.query.select_q;
    TEST_CHECK(s_q->all == false);
    TEST_CHECK(s_q->constraint.fieldId == HEIGHT);
    TEST_CHECK(s_q->constraint.comparator == GREATER);
    TEST_CHECK(s_q->constraint.fieldVal.height == 180.1);
}




TEST_LIST = {
    {"test_parse_constraint_id", test_parse_constraint_id},
    {"test_parse_constraint_age", test_parse_constraint_age},
    {"test_parse_constraint_height", test_parse_constraint_height},
    {"test_parse_constraint_name", test_parse_constraint_name},
    {"test_parse_constraint_fail_invalid_field", test_parse_constraint_fail_invalid_field},
    {"test_parse_coinstraint_fail_invalid_val", test_parse_coinstraint_fail_invalid_val},

    {"test_parse_select_1", test_parse_select_1},
    {"test_parse_select_all", test_parse_select_all},

    
    {"test_parse_insert", test_parse_insert},
    {"test_parse_delete", test_parse_delete},
    {"test_parse_update", test_parse_update},
    {"test_parse_sql_query", test_parse_sql_query},

    {0} /* Test suite must be terminated with {0} */
};
