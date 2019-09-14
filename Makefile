# TP1-SO

CC = gcc
# Collects all source files un cwd #
SRC := $(wildcard *.c)
# Group of the binary objects #
BIN := $(SRC:%.c=%)
# Compiling required flags #
CFLAGS = -Wall -g -lrt -pthread

all: $(BIN)

minisat:
	@echo Checking for minisat.
	@type minisat >/dev/null 2>&1 || apt-get install minisat

%: %.c
	@echo Compiling $< file...
	@$(CC) $(CFLAGS) $< -o $@ 
	@echo Binary file $@ generated.

.PHONY: all minisat 



