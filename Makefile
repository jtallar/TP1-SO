# TP1-SO

CC = gcc
# Collects all source files un cwd #
SRC = $(wildcard *.c)
# Group of the binaries #
BINS = $(SRC:%.c=%)
# Compiling required flags #
CFLAGS = -Wall -g -lrt -pthread

all: $(BINS)

%: %.c
	@$(CC) $(CFLAGS) $< -o $@

.PHONY: all