# Dynamic Makefile for learning assembly from C code
# Automatically handles all C files in src/

CC = gcc
CFLAGS = -Wall -g
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
BINS = $(patsubst $(SRC_DIR)/%.c,%,$(SRC))
ASMS = $(patsubst $(SRC_DIR)/%.c,%.s,$(SRC))

.PHONY: all asm run clean disasm

all: $(BINS)

%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

asm: $(ASMS)

%.s: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -S -o $@ $<

run: all
	@echo "Available executables: $(BINS)"
	@for bin in $(BINS); do \
		if [ -x $$bin ]; then echo "Running $$bin:"; ./$$bin; fi; \
	done

disasm: all
	@for bin in $(BINS); do \
		if [ -x $$bin ]; then echo "Disassembly of $$bin:"; objdump -d $$bin | less; fi; \
	done

clean:
	rm -f $(BINS) $(ASMS)

