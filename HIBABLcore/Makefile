IMG_SIZE = 256M

BOOT_DIR := $(CORE_DIR)/boot

all: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/diskboot.bin $(BUILD_DIR)/disk.img

# Single rule that builds the boot subdirectory (produces both .bin files)
.PHONY: boot_build
.PHONY: core_build
boot_build:
	$(MAKE) -C $(BOOT_DIR)
	
core_build:
	$(MAKE) -f $(CORE_DIR)/Makefile.core

# Both .bin files depend on the phony rule — Make won't double-invoke
$(BUILD_DIR)/boot.bin $(BUILD_DIR)/diskboot.bin: boot_build

$(BUILD_DIR)/core.bin: core_build

$(BUILD_DIR)/disk.img: $(BUILD_DIR)/boot.bin $(BUILD_DIR)/diskboot.bin $(BUILD_DIR)/core.bin
	rm -f $@
	truncate -s $(IMG_SIZE) $@
	dd if=$(BUILD_DIR)/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_DIR)/diskboot.bin of=$@ bs=512 seek=1 conv=notrunc
	dd if=$(BUILD_DIR)/core.bin of=$@ bs=512 seek=2 conv=notrunc

clean:
	$(MAKE) -C $(BOOT_DIR) clean
	rm -f $(BUILD_DIR)/disk.img