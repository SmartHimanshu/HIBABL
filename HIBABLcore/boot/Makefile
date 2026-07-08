CC  := gcc
LD  := ld

all: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/diskboot.bin

$(BIN_DIR)/boot.o: i386/boot.S
	$(CC) $(INCLUDE_DIR) \
	      -m32 \
	      -c \
	      -nostdlib \
	      -nostdinc \
	      -ffreestanding \
	      -fno-asynchronous-unwind-tables \
	      -o $@ $<

$(BUILD_DIR)/boot.bin: $(BIN_DIR)/boot.o
	$(LD) -m elf_i386 \
	      -Ttext 0x7C00 \
	      --oformat binary \
	      --gc-sections \
	      -o $@ $<

$(BIN_DIR)/diskboot.o: i386/diskboot.S
	$(CC) $(INCLUDE_DIR) \
	      -m32 \
	      -c \
	      -nostdlib \
	      -nostdinc \
	      -ffreestanding \
	      -fno-asynchronous-unwind-tables \
	      -o $@ $<

$(BUILD_DIR)/diskboot.bin: $(BIN_DIR)/diskboot.o
	$(LD) -m elf_i386 \
	      -Ttext 0x8000 \
	      --oformat binary \
	      --gc-sections \
	      -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/boot.bin $(BIN_DIR)/boot.o $(BUILD_DIR)/diskboot.bin $(BIN_DIR)/diskboot.o 