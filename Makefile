BUILD_ELF_PATH = build/usbad
OUTPUT_BINARY_IMAGE_PATH = build/firmware.bin
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

build:
	mkdir -p build  \
		&& cd build \
		&& cmake .. \
		&& $(MAKE) -j$(shell nproc)

clean:
	cd build && make clean

distclean:
	rm -rf build

disassemble:
	$(OBJDUMP) -dt $(BUILD_ELF_PATH)

disassemble_all:
	$(OBJDUMP) -Dt $(BUILD_ELF_PATH)

image:
	@echo Prepare image for uploading onto target
	$(OBJCOPY) -O binary $(BUILD_ELF_PATH) $(OUTPUT_BINARY_IMAGE_PATH)

.PHONY: build
