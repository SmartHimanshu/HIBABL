# Alpha 2 Bootloader - Root Makefile

ROOT_DIR  	:= $(shell pwd)
CORE_DIR  	:= $(ROOT_DIR)/HIBABLcore
BIN_DIR   	:= $(ROOT_DIR)/bin
BUILD_DIR 	:= $(ROOT_DIR)/build
INCLUDE_DIR	:= -I$(ROOT_DIR)/include

export ROOT_DIR BIN_DIR BUILD_DIR CORE_DIR INCLUDE_DIR

.PHONY: all clean run deploy

all:
	$(MAKE) -C $(CORE_DIR)

run:
	qemu-system-x86_64 -drive format=raw,file=$(BUILD_DIR)/disk.img

clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*