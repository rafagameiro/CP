#CC=gcc
LLVM=/usr/local/opt/llvm
CC=$(LLVM)/bin/clang
CFLAGS=-c -Wall -g -ansi -pedantic -std=gnu11 -Werror -fopenmp -I/usr/local/include
LDFLAGS=-fopenmp -L/usr/local/lib -L$(LLVM)/lib -lpcre


OUTPUT=glife
SRC=config.c game.c main.c mem.c
OBJ=$(patsubst %.c,%.o,$(SRC))

$(OUTPUT): $(OBJ)
	$(CC) -o $(OUTPUT) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OUTPUT) $(OBJ)
