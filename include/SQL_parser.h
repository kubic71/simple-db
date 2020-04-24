#ifndef SQL_PARSER_H_
#define SQL_PARSER_H_

#include "table.h"

/*
 * --- Supported SQL subset --- 
 * This basic implementation has a single table with a fixed number of columuns
 * columns:
 *  - int id
 *  - int age
 *  - double height 
 *  - int income 
 *  - char name[MAX_STR_LEN] 
 *  - char profession[MAX_STR_LEN] 
 *  - char nationality[MAX_STR_LEN]
 * 
 * --- SQL commands ---
 * 
 * SELECT * [WHERE <numerical_field_name> <= >= < > <number> ]
 * for example: 
 *      SELECT *    // selects the whole table
 *      SELECT * WHERE age >= 18     // get all adults    
 * 
 * INSERT VALUE(id,age,height,income,'name','prof','nationality')
 * for example:
 *      INSERT(2, 21, 180.23, 70800, 'Joe Brown', 'Teacher', 'US')
 * 
 * DELETE WHERE <numerical_field_name> <= >= < > <number>
 * for example:
 *      DELETE WHERE 
 * 
 * UPDATE SET field=value WHERE <numerical_field_name> <= >= < > <number>
 * for example:
 *      UPDATE SET income=0 WHERE age<15    // children cannot legally work for money
 * 
 * 
 * 
 */


enum FieldId{id,age,height,income,name,profession,nationality};
enum Comparator{lower, lower_or_equal_to, greater, greater_or_equal_to, equal};


typedef struct
{

    
} FieldVal;

  
typedef struct  
{
    /* Represents constraint like age>32 */
    FieldId fieldId;
    Comparator comparator;
    FieldVal fieldVal;-
} Constraint;


typedef struct {
    Constraint constraint;
} Select_Query;

typedef struct {
    T_Record record;
} Insert_Query;

typedef struct {
    Constraint constraint;
} Delete_Query;

typedef struct {
    char field[MAX_STR_LEN];

    Constraint constraint;
} Update_Query;


typedef struct {

    /* 
    * 's': SELECT
    * 'i': INSERT
    * 'd': DELETE
    * 'u': UPDATE
    */
    char type;

    union
    {   
        Select_Query select;
        Insert_Query insert;
        Delete_Query delete;
        Update_Query update;
    } query;

} SQL_Query;

/*
 * parse_{select, insert, delete} methods take sql_srt and pointer to allocated query
 * structure. They try to parse the SQL request, and if:
 *  - parsing is successful, SQL_Query structure is updated and true is returned.
 *  - parsing fails, false is returned and the struct is left unchanged
 * 
*/

bool parse(char *sql_str, SQL_Query *query);

bool parse_select(char *sql_str, SQL_Query *query); 
bool parse_insert(char *sql_str, SQL_Query *query);
bool parse_delete(char *sql_str, SQL_Query *query);
bool parse_update(char *sql_str, SQL_Query *query);  

void parse_SQL(char * sql_str, SQL_Query * query);


#endif /* SQL_PARSER_H_ */
