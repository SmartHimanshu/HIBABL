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

	parted $@ --script \
		mklabel msdos \
		mkpart primary fat32 1MiB 100% \
		set 1 boot on

	# Save partition table + signature
	dd if=$@ of=$(BUILD_DIR)/pt.bin bs=1 skip=446 count=66

	# Format the FAT32 partition
	LOOP=$$(sudo losetup --find --show -P $@); \
	sudo mkfs.fat -F 32 $${LOOP}p1; \
	sudo losetup -d $$LOOP

	# Install our boot sector (only first 446 bytes of code,
	# leave partition table intact)
	dd if=$(BUILD_DIR)/boot.bin of=$@ bs=1 count=446 conv=notrunc

	# Restore partition table + signature FIRST, then copy files
	dd if=$(BUILD_DIR)/pt.bin of=$@ bs=1 seek=446 conv=notrunc

	dd if=$(BUILD_DIR)/diskboot.bin of=$@ bs=512 seek=1 conv=notrunc
	dd if=$(BUILD_DIR)/core.bin of=$@ bs=512 seek=2 conv=notrunc

	mcopy -i $@@@1M $(BUILD_DIR)/core.bin ::/CORE.BIN

clean:
	$(MAKE) -C $(BOOT_DIR) clean
	rm -f $(BUILD_DIR)/disk.img