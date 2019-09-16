# TP1-SO

CC = gcc
# Collects all source files un cwd #
SRC := $(wildcard *.c)
# Group of the binary objects #
BIN := $(SRC:%.c=%)
# Compiling required flags #
CFLAGS = -Wall -g -lrt -pthread

build: $(BIN)

minisat:
	@echo Checking for minisat.
	@type minisat >/dev/null 2>&1 || apt-get install minisat

run:
	@echo Running program
	@./app Prueba/* | ./vista

%: %.c
	@echo Compiling $< file...
	@$(CC) $(CFLAGS) $< -o $@ 
	@echo Binary file $@ generated.

.PHONY: build minisat run



