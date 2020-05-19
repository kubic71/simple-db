#include "SQL_parser.h"
#include "table.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>

static void toUpperStr(char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		str[i] = toupper(str[i]);
		i++;
	}
}

static char *trimWhitespace(char *str)
{
	char *end;

	// Trim leading space
	while (isspace(*str))
		str++;

	if (*str == 0) // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;

	while (end > str && isspace(*end))
		end--;
	// Write new null terminator character

	// end[1] = '\0';

	return str;
}

bool parseSpace(char **str)
{
	// at least one space is mandatory
	if (isspace(**str))
	{
		while (isspace(**str))
			(*str)++;
		return true;
	}
	return false;
}

bool parse_string_in_quotes(char **parsed_str, char *content)
{
	char *next_char = *parsed_str;
	if (*next_char != '\'')
		return false;

	next_char++;

	while (*next_char != '\'' && *next_char != 0)
	{
		*content = *next_char;
		content++;
		next_char++;
	}

	if (*next_char == 0) // didn't find closing quote
		return false;

	*content = '\0';

	// point string pointer to first char after the closing quote
	*parsed_str = next_char + 1;

	return true;
}

/*
 * if parsed_str starts with searched_str, then return true and shift the parsed_str pointer to the first character after the match 
 * otherwise return false and don't modify the parsed_str pointer
 */
static bool parse_str(char **parsed_str, char *searched_str)
{

	char *next_char = *parsed_str;
	while (*searched_str != 0)
	{
		if (*next_char == 0)
			return false;

		// comparing character by character
		if (*next_char != *searched_str)
		{
			return false;
		}

		// move to the next char
		next_char++;
		searched_str++;
	}

	*parsed_str = next_char;
	return true;
}

static void parse_token(char **parsed_str, char *word)
{
	*parsed_str = trimWhitespace(*parsed_str);

	while (**parsed_str != 0 && !isspace(**parsed_str))
	{
		*word = **parsed_str;
		word++;
		(*parsed_str)++;
	}

	*word = '\0';
}

static bool parse_number(char **parsed_str, char *digits, bool decimal)
{

	int decimal_dots = 0;
	int char_n = 0;

	while (**parsed_str != 0 && (isdigit(**parsed_str) || (decimal && **parsed_str == '.' && decimal_dots++ == 0)))
	{
		*digits = **parsed_str;
		digits++;
		(*parsed_str)++;
		char_n++;
	}

	*digits = '\0';

	if (char_n == 0)
		return false;

	return true;
}

static bool parse_int(char **parsed_str, int *val)
{
	char *backup = *parsed_str;
	char int_str[20];

	parse_number(parsed_str, int_str, false);
	if (sscanf(int_str, "%d", val) < 1)
	{
		return false;
		*parsed_str = backup;
	}
	return true;
}

static bool parse_double(char **parsed_str, double *val)
{
	char *backup = *parsed_str;
	char double_str[20];

	parse_number(parsed_str, double_str, true);
	if (sscanf(double_str, "%lf", val) < 1)
	{
		return false;
		*parsed_str = backup;
	}
	return true;
}

bool parse_constraint(char *constraint_str, Constraint *c)
{
	constraint_str = trimWhitespace(constraint_str);	

	int MAX_TOKEN_LEN = 15;
	if (!parse_str(&constraint_str, WHERE_STR))
		return false;

	constraint_str = trimWhitespace(constraint_str);

	char field_name[MAX_TOKEN_LEN];
	parse_token(&constraint_str, field_name);

	char comp_str[MAX_TOKEN_LEN];
	parse_token(&constraint_str, comp_str);

	// parse the < > <= >= ==
	if (strcmp(comp_str, "<") == 0)
	{
		c->comparator = LOWER;
	}
	else if (strcmp(comp_str, "<=") == 0)
	{
		c->comparator = LOWER_OR_EQUAL;
	}
	else if (strcmp(comp_str, ">") == 0)
	{
		c->comparator = GREATER;
	}
	else if (strcmp(comp_str, ">=") == 0)
	{
		c->comparator = GREATER_OR_EQUAL;
	}
	else if (strcmp(comp_str, "==") == 0)
	{
		c->comparator = EQUAL;
	}
	else
	{
		return false;
	}

	constraint_str = trimWhitespace(constraint_str);

	if (strcmp(field_name, ID_STR) == 0)
	{
		c->fieldId = ID;

		return parse_int(&constraint_str, &c->fieldVal.id);
	}
	else if (strcmp(field_name, NAME_STR) == 0)
	{
		c->fieldId = NAME;
		return parse_string_in_quotes(&constraint_str, c->fieldVal.name);
	}
	else if (strcmp(field_name, AGE_STR) == 0)
	{
		c->fieldId = AGE;
		return parse_int(&constraint_str, &c->fieldVal.age);
	}
	else if (strcmp(field_name, HEIGHT_STR) == 0)
	{
		c->fieldId = HEIGHT;
		return parse_double(&constraint_str, &c->fieldVal.height);
	}
	else
	{
		return false;
	}

	// unreachable, make compiler happy
	return true;
}

static bool startsWith(const char *pre, const char *str)
{
	return strncmp(pre, str, strlen(pre)) == 0;
}

bool parse_select(char *sql_str, Select_Query *query)
{
	sql_str = trimWhitespace(sql_str);
	char *PREFIX = "SELECT *";
	if (strcmp(sql_str, PREFIX) == 0)
	{
		query->all = true;
		return true;
	}

	query->all = false;

	if (parse_str(&sql_str, PREFIX))
	{
		sql_str = trimWhitespace(sql_str);
		return parse_constraint(sql_str, &query->constraint);
	}

	return false;
}

static bool parse_comma_delim(char **sql_str)
{

	char *backup_p = *sql_str;

	*sql_str = trimWhitespace(*sql_str);
	// printf("delim_str:\n%s\n", *sql_str);
	if (parse_str(sql_str, ","))
	{
		*sql_str = trimWhitespace(*sql_str);
		return true;
	}

	*sql_str = backup_p;
	return false;
}

bool parse_insert(char *sql_str, Insert_Query *query)
{
	// Example:
	// INSERT(2, 21, 168.23, 'Joe Brown')
	return parse_str(&sql_str, "INSERT(") && parse_int(&sql_str, &query->record.id) && parse_comma_delim(&sql_str) && parse_int(&sql_str, &query->record.age) && parse_comma_delim(&sql_str) && parse_double(&sql_str, &query->record.height) && parse_comma_delim(&sql_str) && parse_string_in_quotes(&sql_str, query->record.name) && parse_str(&sql_str, ")") && *sql_str == 0;
}

bool parse_delete(char *sql_str, Delete_Query *query)
{
	char *PREFIX = "DELETE ";
	if (parse_str(&sql_str, PREFIX))
	{
		return parse_constraint(sql_str, &query->constraint);
		return true;
	}

	return false;
}
bool parse_update(char *sql_str, Update_Query *query)
{
	// Example:
	// UPDATE SET HEIGHT=183.3 WHERE ID == 15

	char* PREFIX = "UPDATE SET ";
	if(!parse_str(&sql_str, PREFIX)) {
		return false;
	}

	// Parse the set-field
	if(parse_str(&sql_str, ID_STR "=") && parse_int(&sql_str, &query->val.id)) {
		query->fieldId=ID;

	} else if (parse_str(&sql_str, AGE_STR "=") && parse_int(&sql_str, &query->val.id)) {
		query->fieldId=AGE;
			
	} else if (parse_str(&sql_str, HEIGHT_STR "=") && parse_double(&sql_str, &query->val.height)) {
		query->fieldId=HEIGHT;

	} else if (parse_str(&sql_str, NAME_STR "=")) {
		query->fieldId=NAME;
		sql_str = trimWhitespace(sql_str);
		if(!parse_string_in_quotes(&sql_str, query->val.name)) {
			return false;
		}
	} else {
		return false;
	} 
	
	return parse_constraint(sql_str, &query->constraint);

	return false;
}

bool parse_SQL(char *sql_str, SQL_Query *query)
{

	// modifies sql_str!
	sql_str = trimWhitespace(sql_str);

	if (parse_insert(sql_str, &query->query.insert_q))
	{
		query->type = INSERT;
	}
	else if (parse_select(sql_str, &query->query.select_q))
	{
		query->type = SELECT;
	}
	else if (parse_delete(sql_str, &query->query.delete_q))
	{
		query->type = DELETE;
	}
	else if (parse_update(sql_str, &query->query.update_q))
	{
		query->type = UPDATE;
	}
	else
	{
		return false;
	}
	return true;
}
