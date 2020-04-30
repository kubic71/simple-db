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
 * SELECT * [WHERE <field_name> <= >= < > == <value> ]
 * for example: 
 *      SELECT *    // selects the whole table
 *      SELECT * WHERE age >= 18     // get all adults    
 * 
 * INSERT VALUE(<id>,<age>,<height>,'<name>')
 * for example:
 *      INSERT(2, 21, 180.23, 'Joe Brown')
 * 
 * DELETE WHERE <field_name> <= >= < > == <value>
 * for example:
 *      DELETE WHERE id == 2
 * 
 * UPDATE SET field=value WHERE <field_name> <= >= < > == <value>
 * for example:
 *      UPDATE SET height=183.3 WHERE id == 15    // update record number 15
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
    LOWER_OR_EQUAL,
    GREATER,
    GREATER_OR_EQUAL,
    EQUAL
} Comparator;

typedef union {
        int id;
        int age;
        double height;
        char name[MAX_STR_LEN];
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
    bool all;
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
    FieldId fieldId;
    FieldVal val;
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

// subroutine used by parse_{select, insert, delete, update}
// parses the part "WHERE age >= 10"
bool parse_constraint(char *constraint_str, Constraint *c);

bool parse_select(char *sql_str, Select_Query *query);
bool parse_insert(char *sql_str, Insert_Query *query);
bool parse_delete(char *sql_str, Delete_Query *query);
bool parse_update(char *sql_str, Update_Query *query);

bool parse_SQL(char *sql_str, SQL_Query *query);

#endif /* SQL_PARSER_H_ */
