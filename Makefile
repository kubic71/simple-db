CC=gcc
CFLAGS=-g -Wall -I include -I src
DEPS = SQL_parser.h 

PROTOCOL_ENG_OBJ = SQL_parser.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

protocol_engine: $(PROTOCOL_ENG_OBJ)
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean

clean:
	rm -f ./*.o 
