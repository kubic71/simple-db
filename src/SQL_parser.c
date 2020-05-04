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
	if (isspace(**str))
	{
		while (isspace(**str))
			(*str)++;
		return true;
	}
	return false;
}

bool parseString(char *input_str, char **start, int *len)
{
	if (*input_str != '\'')
		return false;

	input_str++;

	*start = input_str;

	while (*input_str != '\'' & *input_str != 0)
	{
		input_str++;
	}

	if(*input_str == 0)    // didn't fing closing quote	
		return false;
	
	// *input_str now points to the closing quote
	*len = input_str - (*start);
	return true;
}

bool parse_constraint(char *constraint_str, Constraint *c)
{
	// constraint_str example:
	// "WHERE AGE <= 10"

	int i = 0;

	int N_TOKENS = 4;
	int MAX_TOKEN_LEN = 15;

	char tokens[N_TOKENS][MAX_TOKEN_LEN];
	char *where = "WHERE";
	char *id = "ID";
	char *age = "AGE";
	char *name = "NAME";
	char *height = "HEIGHT";
	const char *delim = " ";

	// !!! we've got memory leak right here!!!
	
	char *str = strdup(constraint_str); //duplicate the string

	if (str == NULL)
	{
		fprintf(stderr, "strdup failed");
		exit(EXIT_FAILURE);
	}

	// copy constraint tokens to tokens array
	char *token = strtok(str, delim);
	while (token != NULL && i < N_TOKENS)
	{
		strcpy(tokens[i], token);
		i++;
		token = strtok(NULL, delim);
	}

	// We don't want to UPPERCASE the last token, which may be string value
	// We want to distinguish between "Joe", "JOE" and "joe"
	for (i = 0; i < N_TOKENS - 1; i++)
	{
		toUpperStr(tokens[i]);
	}

	if (strcmp(tokens[0], where) != 0)
	{
		return false;
	}

	// parse the < > <= >= ==
	if (strcmp(tokens[2], "<") == 0)
	{
		c->comparator = LOWER;
	}
	else if (strcmp(tokens[2], "<=") == 0)
	{
		c->comparator = LOWER_OR_EQUAL;
	}
	else if (strcmp(tokens[2], ">") == 0)
	{
		c->comparator = GREATER;
	}
	else if (strcmp(tokens[2], ">=") == 0)
	{
		c->comparator = GREATER_OR_EQUAL;
	}
	else if (strcmp(tokens[2], "==") == 0)
	{
		c->comparator = EQUAL;
	}
	else
	{
		return false;
	}

	if (strcmp(tokens[1], id) == 0)
	{
		c->fieldId = ID;
		if (sscanf(tokens[3], "%d", &c->fieldVal.id) < 1)
		{
			return false;
		}
	}
	else if (strcmp(tokens[1], name) == 0)
	{
		c->fieldId = NAME;
		char *s = tokens[3];
		if (s[0] != '\'' || s[strlen(s) - 1] != '\'')
		{
			return false;
		}

		// Remove single quotes
		s[strlen(s) - 1] = '\0';
		s++;
		strcpy(c->fieldVal.name, s);
	}
	else if (strcmp(tokens[1], age) == 0)
	{
		c->fieldId = AGE;
		if (sscanf(tokens[3], "%d", &c->fieldVal.age) < 1)
			return false;
	}
	else if (strcmp(tokens[1], height) == 0)
	{
		c->fieldId = HEIGHT;
		if (sscanf(tokens[3], "%d", &c->fieldVal.height) < 1)
			return false;
	}
	else
	{
		return false;
	}

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
