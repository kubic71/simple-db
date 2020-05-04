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

	// *input_str now points to the closing quote
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

static bool parse_int(char **parsed_str, int *val)
{
	char* backup = *parsed_str;
	char int_str[20];
	
	parse_token(parsed_str, int_str);
	if (sscanf(int_str, "%d", val) < 1)
	{
		return false;
		*parsed_str = backup;
	}
	return true;
}

static bool parse_double(char **parsed_str, double *val)
{
	char* backup = *parsed_str;
	char double_str[20];
	
	parse_token(parsed_str, double_str);
	if (sscanf(double_str, "%lf", val) < 1)
	{
		return false;
		*parsed_str = backup;
	}
	return true;
}

bool parse_constraint(char *constraint_str, Constraint *c)
{
	// constraint_str example:
	// "WHERE AGE <= 10"
	// int N_TOKENS = 4;
	int MAX_TOKEN_LEN = 15;

	// char tokens[N_TOKENS][MAX_TOKEN_LEN];
	char *where = "WHERE";
	char *id = "ID";
	char *age = "AGE";
	char *name = "NAME";
	char *height = "HEIGHT";
	// const char *delim = " ";

	/*	
	char *str = strdup(constraint_str); //duplicate the string

	if (str == NULL)
	{
		fprintf(stderr, "strdup failed");
		exit(EXIT_FAILURE);
	}
	*/

	// copy constraint tokens to tokens array
	/*
	char *token = strtok(str, delim);
	while (token != NULL && i < N_TOKENS)
	{
		strcpy(tokens[i], token);
		i++;
		token = strtok(NULL, delim);
	}
	*/

	// We don't want to UPPERCASE the last token, which may be string value
	// We want to distinguish between "Joe", "JOE" and "joe"
	/*
	for (i = 0; i < N_TOKENS - 1; i++)
	{
		toUpperStr(tokens[i]);
	}
	*/

	if (!parse_str(&constraint_str, where))
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

	if (strcmp(field_name, id) == 0)
	{
		c->fieldId = ID;

		return parse_int(&constraint_str, &c->fieldVal.id);
	}
	else if (strcmp(field_name, name) == 0)
	{
		c->fieldId = NAME;
		return parse_string_in_quotes(&constraint_str, c->fieldVal.name);
	}
	else if (strcmp(field_name, age) == 0)
	{
		c->fieldId = AGE;
		return parse_int(&constraint_str, &c->fieldVal.age);
	}
	else if (strcmp(field_name, height) == 0)
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
	char *PREFIX = "SELECT *";
	if (strcmp(sql_str, PREFIX) == 0)
	{
		query->all = true;
		return true;
	}

	query->all = false;

	if (startsWith(PREFIX, sql_str))
	{
		sql_str += strlen(PREFIX);
		return parse_constraint(sql_str, &query->constraint);
	}

	return false;
}

bool parse_insert(char *sql_str, Insert_Query *query)
{
	return false;
}

bool parse_delete(char *sql_str, Delete_Query *query)
{
	char *PREFIX = "DELETE ";
	if (startsWith(PREFIX, sql_str))
	{
		sql_str += strlen(PREFIX);
		return parse_constraint(sql_str, &query->constraint);
		return true;
	}

	return false;
}
bool parse_update(char *sql_str, Update_Query *query) { return false; }

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
