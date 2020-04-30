#ifndef SQL_PARSER_H_
#define SQL_PARSER_H_

#include <stdbool.h>
#include "table.h"

/*
 * --- Supported SQL subset --- 
 * This basic implementation has a single table with a fixed number of columuns
 * columns:
 *  - int id
 *  - int age
 *  - double height 
 *  - char name[MAX_STR_LEN] 
 * 
 * --- SQL commands ---
 * 
 * SELECT * [WHERE <field_name> <= >= < > == <number> ]
 * for example: 
 *      SELECT *    // selects the whole table
 *      SELECT * WHERE age >= 18     // get all adults    
 * 
 * INSERT VALUE(<id>,<age>,<height>,'<name>')
 * for example:
 *      INSERT(2, 21, 180.23, 'Joe Brown')
 * 
 * DELETE WHERE <field_name> <= >= < > == <number>
 * for example:
 *      DELETE WHERE id == 2
 * 
 * UPDATE SET field=value WHERE <numerical_field_name> <= >= < > == <number>
 * for example:
 *      UPDATE SET income=0 WHERE age<15    // children cannot legally work for money
 * 
 * 
 * 
 */

// To make things simple, we have a static table with int, double and string
typedef enum 
{
    ID,
    AGE,
    HEIGHT,
    NAME
} FieldId;

typedef enum 
{
    LOWER,
    LOWER_OR_EQUAL_TO,
    GREATER,
    GREATER_OR_EQUAL_TO,
    EQUAL
} Comparator;

typedef struct
{
    FieldId id;

    union {
        int id;
        int age;
        double height;
        char name[MAX_STR_LEN];
    } val;

} FieldVal;

/* 
 * Constraint identifies subset of records
 * for example: age >= 18 represents records of all adults
 */
typedef struct
{
    FieldId fieldId;
    Comparator comparator;
    FieldVal fieldVal;
} Constraint;

typedef struct
{
    Constraint constraint;
} Select_Query;

typedef struct
{
    T_Record record;
} Insert_Query;

typedef struct
{
    Constraint constraint;
} Delete_Query;

typedef struct
{
    char field[MAX_STR_LEN];

    Constraint constraint;
} Update_Query;


typedef enum 
{
    SELECT,
    INSERT,
    DELETE,
    UPDATE
} QueryType;

typedef struct
{

    QueryType type;

    union {
        Select_Query select_q;
        Insert_Query insert_q;
        Delete_Query delete_q;
        Update_Query update_q;
    } query;

} SQL_Query;

/*
 * bool parse_{select, insert, update, delete} methods take sql_srt and pointer to allocated query
 * structure. They try to parse the SQL request, and:
 *  - if parsing is successful, SQL_Query structure is filled with the parsed data and true is returned.
 *  - if parsing fails, false is returned and the struct is left unchanged
 * 
*/

bool parse_select(char *sql_str, Select_Query *query);
bool parse_insert(char *sql_str, Insert_Query *query);
bool parse_delete(char *sql_str, Delete_Query *query);
bool parse_update(char *sql_str, Update_Query *query);

bool parse_SQL(char *sql_str, SQL_Query *query);

#endif /* SQL_PARSER_H_ */
