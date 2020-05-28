#ifndef PROTOCOL_ENGIGE_H
#define PROTOCOL_ENGINE_H_

#include <sys/wait.h>
#include "SQL_parser.h"
#include "table.h"

#define MYPORT 8000
#define BACKLOG 10
#define MAXQUERY 100

void waitForChild(int *s);

int protocol_engine();

#endif /* SQL_PARSER_H_ */
