# Makefile template for C + Assembly project
# Edit SRC_C, SRC_ASM, and TARGET as needed
# Any new .c file in src/, e.g., queue.c, will be included automatically.

CC = gcc
AS = nasm
CFLAGS = -Wall -g
ASFLAGS = -f elf64

SRC_C := $(wildcard src/*.c)
SRC_ASM := $(wildcard src/*.asm)
OBJ_C := $(SRC_C:.c=.o)
OBJ_ASM := $(SRC_ASM:.asm=.o)
OBJ := $(OBJ_C) $(OBJ_ASM)

TARGET := main

.PHONY: all clean

all: $(TARGET)

$(OBJ_C): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_ASM): %.o : %.asm
	$(AS) $(ASFLAGS) $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

clean:
	rm -f $(OBJ) $(TARGET)

# End of Makefile

