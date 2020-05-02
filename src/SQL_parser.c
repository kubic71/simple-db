#include "SQL_parser.h"
#include "table.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


bool parse_constraint(char *constraint_str, Constraint *c) {

	int i = 0;
	char words[4][15];
	bool result = false;
	bool constraint_result = false;

	char where[] = "WHERE";

	char id[] = "ID";
	char age[] = "AGE";
	char name[] = "NAME";
	char height[] = "HEIGHT";

    char lower[] = "<";
    char lowerOrEqual[] = "<=";
    char greater[] = ">";
    char greaterOrEqual[] = ">=";
    char equal[] = "==";

	bool first = false;
	bool second = false;
	bool third = false;
	bool forth = false;


	const char* delim = " ";  // delimiters - space bar

	char* str = strdup(constraint_str);	//duplicate the string

	if (str == NULL) {
		fprintf(stderr, "strdup failed");
		exit(EXIT_FAILURE);
	}


	char *token = strtok(str, delim);

	while (token != NULL) {
		strcpy(words[i], token);
		i++;
		token = strtok (NULL, delim);
	}

	int j = 0;
	char ch;

	for(int k = 0; k <= 3; k++){
		j = 0;
		while (words[k][j]) {
				ch = words[k][j];
				words[k][j] = toupper(ch);
				j++;
		}
	}


	if(strcmp(words[0], where) == 0){
		first = true;
	}

	if(strcmp(words[1], id) == 0) {
		second = true;
		c->fieldId = ID;
		c->fieldVal.id = atoi(words[3]); 				//Here should be checking-error
		forth = true;
	}
	else if(strcmp(words[1], name) == 0) {
		second = true;
		c->fieldId = NAME;
//		c->fieldVal.name = words[3];
		strcpy(c->fieldVal.name, words[3]);				//doesnt work
//		c->fieldVal.name = name;
		forth = true;
	}
	else if(strcmp(words[1], age) == 0) {
		second = true;
		c->fieldId = AGE;
		c->fieldVal.age = atoi(words[3]); 				//Here should be checking-error
		forth = true;
	}
	else if(strcmp(words[1], height) == 0) {
		second = true;
		c->fieldId = HEIGHT;
		int number;
		sscanf(words[3], "%d", &number); 				//Here should be checking-error
		c->fieldVal.height = number;
		forth = true;
	}
	else {
		second = false;
//		*c->fieldId = NULL;
		forth = false;
//		*c->fieldVal = NULL;
	}

	if(strcmp(words[2], lower) == 0) {
		third = true;
		c->comparator = LOWER;
	}
	else if(strcmp(words[2], lowerOrEqual) == 0) {
		third = true;
		c->comparator = LOWER_OR_EQUAL;
	}
	else if(strcmp(words[2], greater) == 0) {
		third = true;
		c->comparator = GREATER;
	}
	else if(strcmp(words[2], greaterOrEqual) == 0) {
		third = true;
		c->comparator = GREATER_OR_EQUAL;
	}
	else if(strcmp(words[2], equal) == 0) {
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
	int j = 0;
	char ch;
	char words[6][15];

	int result = 0;
	bool constraint_result = false;

	char select[] = "SELECT";
	char all[] = "*";
	char empty[] = "EMPTY";

	bool first = false;
	bool second = false;
	bool third = false;


	for(int k = 0; k <= 5; k++){				//Fill the words array
		strcpy(words[k], "EMPTY");
	}

	const char* delim = " ";  		// delimiters - space bar

	char* str = strdup(sql_str);	//duplicate the string
	if (str == NULL) {
		fprintf(stderr, "strdup failed");
		exit(EXIT_FAILURE);
	}

	char *token = strtok(str, delim);

	while (token != NULL) {
		strcpy(words[i], token);				//Splitting words
		i++;
		token = strtok(NULL, delim);
	}


	for(int k = 0; k <= 5; k++){				//Changing letters to uppercase
		j = 0;
		while (words[k][j]) {
				ch = words[k][j];
				words[k][j] = toupper(ch);
				j++;
		}
	}

	if(strcmp(words[0], select) == 0)
		first = true;

	if(strcmp(words[1], all) == 0)
		second = true;

	if(strcmp(words[2], empty) == 0){
			third = true;
	}
	else if(first == true && second == true){

	    char constr_str[1000] = {0};
	    snprintf(constr_str, sizeof(constr_str), "%s %s %s %s",  words[2], words[3], words[4], words[5]);	//Add words

//		constraint_result = parse_constraint(*constr_str, &query->constraint);								//doesnt work (error)
	}


	free (str);
	if(first == true && second == true && third == true){
		query->all = true;
		return true;
	}
	else if(first == true && second == true && third == false && constraint_result == true){
		query->all = false;
		return true;
	}
	else {
		return false;
	}

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
