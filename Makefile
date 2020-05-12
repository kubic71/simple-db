INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src

TEST_DIR = test
TEST_OBJ_DIR = $(TEST_DIR)/obj

CC=gcc
CFLAGS=-g -Wall -I$(INC_DIR)

LIBS=-lm

_DEPS = SQL_parser.h table.h acutest.h 
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

# sources are compiled into separate obj directory
_OBJ = SQL_parser.o main.o pc_main.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))


.pre-build:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TEST_OBJ_DIR)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) .pre-build 
	$(CC) -c -o $@ $< $(CFLAGS)

protocol_engine: $(OBJ) $(DEPS)
	$(CC) -o $(OBJ_DIR)/$@ $(SRC_DIR)/pc_main.c $(OBJ_DIR)/SQL_parser.o $(CFLAGS) $(LIBS)

test_sql_parser: $(OBJ) $(DEPS)
	$(CC) -o $(TEST_OBJ_DIR)/$@ $(TEST_DIR)/test_sql_parser.c $(OBJ_DIR)/SQL_parser.o  $(CFLAGS) $(LIBS)


.PHONY: clean test

clean:
	rm -f protocol_engine 
	rm -rf $(OBJ_DIR)
	rm -rf $(TEST_OBJ_DIR)
	

test: test_sql_parser
	$(TEST_OBJ_DIR)/test_sql_parser
	
pc: protocol_engine
	$(OBJ_DIR)/protocol_engine
