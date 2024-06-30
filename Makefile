# Output ELF file path
BUILD_ELF_PATH = build/src/application

# Binary image (firmware) path
OUTPUT_BINARY_IMAGE_PATH = build/firmware.bin
OUTPUT_HEX_IMAGE_PATH = build/firmware.hex

# GCC toolchain software
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

main: build

build: 
	mkdir -p build  \
		&& cd build \
		&& cmake .. \
		&& $(MAKE) -j$(shell nproc)

.PHONY: build

# Run `make clean` in build directory
clean:
	cd build && make clean

# Remove the build directory completely, start from scratch
distclean:
	rm -rf build
	rm -f ProjectConfiguration.make

disassemble:
	$(OBJDUMP) -dt $(BUILD_ELF_PATH)

disassemble_all:
	$(OBJDUMP) -Dt $(BUILD_ELF_PATH)

# Prepare binary image
image:
	@echo Prepare image for uploading onto target
	$(OBJCOPY) -O binary $(BUILD_ELF_PATH) $(OUTPUT_BINARY_IMAGE_PATH)
	$(OBJCOPY) -O ihex $(BUILD_ELF_PATH) $(OUTPUT_HEX_IMAGE_PATH)

.PHONY: build_release build_debug
