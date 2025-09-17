# Makefile for C + Assembly coroutine project

CC = gcc
AS = nasm
CFLAGS ?= -Wall -g
ASFLAGS ?= -f elf64

TARGET ?= main
BUILD_DIR ?= build
OBJ_DIR ?= $(BUILD_DIR)/obj

SRC_C := $(wildcard src/*.c)
SRC_ASM := $(wildcard src/*.asm)
OBJ_C := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRC_C))
OBJ_ASM := $(patsubst src/%.asm,$(OBJ_DIR)/%.o,$(SRC_ASM))
OBJ := $(OBJ_C) $(OBJ_ASM)
TARGET_PATH := $(BUILD_DIR)/$(TARGET)

.PHONY: all clean debug

all: $(TARGET_PATH)

$(TARGET_PATH): $(OBJ) | $(BUILD_DIR)
	$(CC) $(OBJ) -o $@

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: src/%.asm | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR): | $(BUILD_DIR)
	mkdir -p $@

debug:
	$(MAKE) BUILD_DIR=$(BUILD_DIR) OBJ_DIR=$(BUILD_DIR)/obj-debug TARGET=$(TARGET)-debug \
	    CFLAGS="$(CFLAGS) -Og -g3 -DDEBUG" ASFLAGS="$(ASFLAGS) -g"

clean:
	rm -rf $(BUILD_DIR)

# End of Makefile
