INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src

TEST_DIR = test
TEST_OBJ_DIR = $(TEST_DIR)/obj

CC=gcc
CFLAGS=-g -Wall -I$(INC_DIR)

LIBS=-lm -lrt -lpthread

_DEPS = SQL_parser.h table.h acutest.h pc_main.h transaction_mg.h util.h query_mq.h in_memory_db.h compare.h
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

# sources are compiled into separate obj directory
_OBJ = SQL_parser.o table.o main.o pc_main.o transaction_mg.o util.o in_memory_db.o compare.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))


.pre-build:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TEST_OBJ_DIR)


# compile all source-files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS) .pre-build 
	$(CC) -c -o $@ $< $(CFLAGS)

# create hybrid executable
# To keep the Makefile as simple as possible, we have only 1 binary that can behave
# as protocol-engine or transaction-manager depending on command line params
simple-db: $(OBJ) $(DEPS)
	$(CC) -o $(OBJ_DIR)/$@ $(OBJ) $(CFLAGS) $(LIBS)

test_sql_parser: $(OBJ) $(DEPS)
	$(CC) -o $(TEST_OBJ_DIR)/$@ $(TEST_DIR)/test_sql_parser.c $(OBJ_DIR)/SQL_parser.o  $(CFLAGS) $(LIBS)


.PHONY: clean test

# remove all output of compilation
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(TEST_OBJ_DIR)
	

test: test_sql_parser
	$(TEST_OBJ_DIR)/test_sql_parser

integ-test: simple-db
	./test/integration_tests/run_tests.sh

	
# start protocol-engine
run-pc: simple-db
	$(OBJ_DIR)/simple-db pc

# start transaction-manager
run-tm: simple-db
	$(OBJ_DIR)/simple-db tm 

# start the whole pipeline just to test 
run: clean simple-db
	-killall simple-db
	stdbuf -o 0 $(OBJ_DIR)/simple-db pc | sed "s/^/pc: /" & 
	stdbuf -o 0 $(OBJ_DIR)/simple-db tm | sed "s/^/tm: /" &
	echo "DELETE WHERE AGE > 10" | nc 127.0.0.1 8080