#include "SQL_parser.h"
#include "table.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


bool parse_constraint(char *constraint_str, Constraint *c) {

	int i = 0;
	char* words[4][15];
	bool result = false;
	bool constraint_result = false;

	char where[] = "WHERE";

	char id[] = "ID";
	char age[] = "AGE";
	char name[] = "NAME";
	char height = "HEIGHT";

    char lower[] = "<";
    char lowerOrEqual = "<=";
    char greater[] = ">";
    char greaterOrEqual = ">=";
    char equal[] = "=";

	bool first = false;
	bool second = false;
	bool third = false;
	bool forth = false;


	const char* delims = " ";  // delimiters - space bar
	char* token;
	char* str = strdup(*constraint_str);	//duplicate the string

	if (str == NULL) {
		fprintf(stderr, "strdup failed");
		exit(EXIT_FAILURE);
	}

//	printf ("Original String: %s\n", ptr);

	token = strtok (str, delims);

	while (token != NULL) {
		strcpy(words[i], token);
		i++;
		//		printf("%s\n", token);
		token = strtok (NULL, delims);
	}

	if(strcmp(toupper(words[1]), where) == 0)
		first = true;

	if(strcmp(toupper(words[2]), id) == 0) {
		second = true;
		c->fieldId = ID;
		c->fieldVal.id = atoi(words[4]); 				//Here should be checking-error
		forth = true;
	}
	else if(strcmp(toupper(words[2]), name) == 0) {
		second = true;
		c->fieldId = NAME;
		strcpy(words[4], *c->fieldVal.name);
		forth = true;
	}
	else if(strcmp(toupper(words[2]), age) == 0) {
		second = true;
		c->fieldId = AGE;
		c->fieldVal.age = atoi(words[4]); 				//Here should be checking-error
		forth = true;
	}
	else if(strcmp(toupper(words[2]), height) == 0) {
		second = true;
		c->fieldId = HEIGHT;
		sscanf(words[4], "%f", c->fieldVal.height); 		//Here should be checking-error
		forth = true;
	}
	else {
		second = false;
//		*c->fieldId = NULL;
		forth = false;
//		*c->fieldVal = NULL;
	}


	if(strcmp(toupper(words[3]), lower) == 0) {
		third = true;
		c->comparator = LOWER;
	}
	else if(strcmp(toupper(words[3]), lowerOrEqual) == 0) {
		third = true;
		c->comparator = LOWER_OR_EQUAL;
	}
	else if(strcmp(toupper(words[3]), greater) == 0) {
		third = true;
		c->comparator = GREATER;
	}
	else if(strcmp(toupper(words[3]), greaterOrEqual) == 0) {
		third = true;
		c->comparator = GREATER_OR_EQUAL;
	}
	else if(strcmp(toupper(words[3]), equal) == 0) {
		third = true;
		c->comparator = EQUAL;
	}
	else {
		third = false;
//		*c->comparator = NULL;
	}


	if(first == true && second == true && third == true && forth == true)
		return true;
	else
		return false;

}

bool parse_select(char *sql_str, Select_Query *query) {
    // dummy implementation
    // always return result for input str:
    // "SELECT * WHERE age >= 18"
//    query->constraint.fieldId = AGE;
//    query->constraint.comparator = GREATER_OR_EQUAL;
//    query->constraint.fieldVal.age = 18;

	int i = 0;
	char* words[6][15];
	bool result = false;
	bool constraint_result = false;

	char select[] = "SELECT";
	char all = "*";
	char* constr_str[100];


	bool first = false;
	bool second = false;
	bool third = false;


	const char* delims = " ";  // delimiters - space bar
	char* token;
	char* str = strdup(*sql_str);	//duplicate the string

	if (str == NULL) {
		fprintf(stderr, "strdup failed");
		exit(EXIT_FAILURE);
	}

//	printf ("Original String: %s\n", ptr);

	token = strtok (str, delims);

	while (token != NULL) {
		strcpy(words[i], token);
		i++;
		//		printf("%s\n", token);
		token = strtok (NULL, delims);
	}

	if(strcmp(toupper(words[1]), select) == 0)
		first = true;

	if(strcmp(words[2], all) == 0)
		second = true;

	if(words[3] == NULL) {
//		query->all = true;
		third = true;
	}
	else {
		strcpy(constr_str, words[3]);
		strcpy(constr_str, " ");
		strcpy(constr_str, words[4]);
		strcpy(constr_str, " ");
		strcpy(constr_str, words[5]);
		strcpy(constr_str, " ");
		strcat(constr_str, words[6]);

		Constraint newConstraint = query->constraint;
		constraint_result = parse_constraint(*constr_str, &newConstraint);
	}


	if(first == true && second == true && third == true && constraint_result == false) {
		query->all = true;
//		query->constraint = NULL;
		result = true;
	}
	else if(first == true && second == true && third == false && constraint_result == true){
		query->all = false;
		result = true;
	}
	else
		result = false;

	free (str);
	return result;

}
bool parse_insert(char *sql_str, Insert_Query *query) {}
bool parse_delete(char *sql_str, Delete_Query *query) {}
bool parse_update(char *sql_str, Update_Query *query) {}

bool parse_SQL(char *sql_str, SQL_Query *query)
{
    query->type = INSERT;
    T_Record* record = &query->query.insert_q.record;

    record->id = 1;
    record->age = 22;
    record->height = 182.3;
    strcpy(&record->name, "Johny Bowen");
}
