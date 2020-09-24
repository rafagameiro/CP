CC=gcc
CFLAGS=-c -g -std=gnu11 -Wall -Werror
LDFLAGS=

# redefine V with "make V=seq" or "make V=par"
# you may also use "make all" to make the two versions
V=seq

# extra arguments for sequential version
SEQCC=
SEQLD=

# extra parameters for Parallel version
PARCC=-fopenmp
PARLD=-fopenmp

DIR=src
S=$(shell find $(DIR) -name '*.c' -print)
O=$(patsubst %.c,%.o,$(S))
UC = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')

UV=$(call UC,$(V))

CFLAGS += $($(UV)CC)
LDFLAGS += $($(UV)LD)


TARGET= main-$(V)

$(TARGET): src/main-$(V).o src/debug.o src/unit.o src/patterns-$(V).o
	$(CC) -o $@ $^ $(LDFLAGS)

ALL=seq par
all: 
	 for i in $(ALL); do make V=$$i; done

src/main-$(V).o: src/main.c
	$(CC) -o $@ $(CFLAGS) -D$(UV) $^ 

src/%.o: src/%.c
	$(CC) -o $@ $(CFLAGS) -D$(UV) $^

.PHONY: clean
clean:
	rm -f $(patsubst %,main-%,$(ALL)) $(O) Makefile.deps

-include Makefile.deps

Makefile.deps:
	$(CC) $(CFLAGS) -MM src/*.[ch] > Makefile.deps
