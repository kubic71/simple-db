#include "acutest.h"
#include "SQL_parser.h"
#include <stdlib.h>


void test_parse_constraint_id(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE ID  ==   3 ", &c);

    // prinf("succ: %d, ")
    
    
    TEST_CHECK(succ);
    TEST_CHECK(c.fieldId == ID);
    TEST_CHECK(c.comparator == EQUAL);
    TEST_CHECK(c.fieldVal.id == 3);
}

void test_parse_constraint_age(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE AGE >= 18", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == AGE);
    TEST_CHECK(c.comparator == GREATER_OR_EQUAL);
    TEST_CHECK(c.fieldVal.age == 18);
}

void test_parse_constraint_height(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE HEIGHT < 185.4", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == HEIGHT);
    TEST_CHECK(c.comparator == LOWER);
    TEST_CHECK(c.fieldVal.height == 185.4);
}

void test_parse_constraint_name(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE NAME == 'Joe Brown'", &c);
    TEST_CHECK(succ);
    
    TEST_CHECK(c.fieldId == NAME);
    TEST_CHECK(c.comparator == EQUAL);

    // printf("%s\n", c.fieldVal.name);
    TEST_CHECK(strcmp(c.fieldVal.name, "Joe Brown") == 0);
}

void test_parse_constraint_fail_invalid_field(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE notvalidfield == 3", &c);
    TEST_CHECK(!succ);
}

void test_parse_coinstraint_fail_invalid_val(void) {
    Constraint c;
    bool succ = parse_constraint("WHERE ID >= lrsi", &c);
    TEST_CHECK(!succ);
}

void test_parse_constraint_fail_random_bs(void) {
    Constraint c;
    bool succ = parse_constraint("This is some random BS, that shouldn't be parsed successfully and should fail!  ", &c);
    TEST_CHECK(!succ);

    succ = parse_constraint("This", &c);
    TEST_CHECK(!succ);
}



void test_parse_select_1(void)
{
    Select_Query query;
    bool succ = parse_select("SELECT * WHERE AGE >= 18", &query);
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
    bool succ = parse_select("SELECT *", &query);
    TEST_CHECK(succ);
    TEST_CHECK(query.all == true);
}


void test_parse_insert(void) {
    Insert_Query query;
    char* query_str = "INSERT(2, 21, 168.23, 'Joe Brown')";
    bool succ = parse_insert(query_str ,&query);
    TEST_CHECK(succ);
    
    TEST_CHECK(query.record.id == 2);
    TEST_CHECK(query.record.age = 21);
    TEST_CHECK(query.record.height == 168.23);
    TEST_CHECK(strcmp(query.record.name, "Joe Brown") == 0);
}


void test_parse_delete(void) {
    Delete_Query query;
    char* testStr = "DELETE WHERE ID == 2";
    bool succ = parse_delete(testStr, &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.constraint.fieldId == ID);
    TEST_CHECK(query.constraint.comparator == EQUAL);
    TEST_CHECK(query.constraint.fieldVal.id == 2);    
}


void test_parse_update(void) {
    Update_Query query;
    bool succ = parse_update("UPDATE SET HEIGHT=183.3 WHERE ID == 15", &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.fieldId == HEIGHT);
    TEST_CHECK(query.val.height == 183.3);

    TEST_CHECK(query.constraint.fieldId == ID);
    TEST_CHECK(query.constraint.comparator == EQUAL);
    TEST_CHECK(query.constraint.fieldVal.id == 15);
}

void test_parse_update_bs(void) {
    Update_Query query;
    bool succ = parse_update("UPDATE SET NAME=This is some BS that shouldn't be parsed", &query);
    TEST_CHECK(!succ);
}



void test_parse_sql_query(void) {
    // test the parse_SQL method as a whole
    SQL_Query query;
    // bool succ = parse_SQL("SELECT * WHERE height > 180.1", &query);
    char* testStr = "SELECT * WHERE HEIGHT > 180.23";

    bool succ = parse_SQL(testStr, &query);
    TEST_CHECK(succ);

    TEST_CHECK(query.type == SELECT);
    Select_Query* s_q = &query.query.select_q;
    TEST_CHECK(s_q->all == false);
    TEST_CHECK(s_q->constraint.fieldId == HEIGHT);
    TEST_CHECK(s_q->constraint.comparator == GREATER);
    // TEST_CHECK(s_q->constraint.fieldVal.height == 180.1);
    TEST_CHECK(s_q->constraint.fieldVal.height == 180.23);
}


TEST_LIST = {
    {"test_parse_constraint_id", test_parse_constraint_id},
    {"test_parse_constraint_age", test_parse_constraint_age},
    {"test_parse_constraint_height", test_parse_constraint_height},
    {"test_parse_constraint_name", test_parse_constraint_name},
    {"test_parse_constraint_fail_invalid_field", test_parse_constraint_fail_invalid_field},
    {"test_parse_coinstraint_fail_invalid_val", test_parse_coinstraint_fail_invalid_val},
    {"test_parse_constraint_fail_random_bs", test_parse_constraint_fail_random_bs},

    {"test_parse_select_1", test_parse_select_1},
    {"test_parse_select_all", test_parse_select_all},

    
    {"test_parse_insert", test_parse_insert},
    {"test_parse_delete", test_parse_delete},
    {"test_parse_update", test_parse_update},
    {"test_parse_sql_query", test_parse_sql_query},
    {"test_parse_update_bs", test_parse_update_bs},
    // {"", },

    {0} /* Test suite must be terminated with {0} */
};
