INC_DIR = include
OBJ_DIR = obj
SRC_DIR = src

CC=gcc
CFLAGS=-g -Wall -I$(INC_DIR)

LIBS=-lm

_DEPS = SQL_parser.h table.h 
DEPS = $(patsubst %,$(INC_DIR)/%,$(_DEPS))

# sources are compiled into separate obj directory
_OBJ = SQL_parser.o main.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

protocol_engine: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f protocol_engine $(OBJ_DIR)/*.o  
