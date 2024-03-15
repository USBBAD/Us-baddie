# Output ELF file path
BUILD_ELF_PATH = build/src/common/application

# Binary image (firmware) path
OUTPUT_BINARY_IMAGE_PATH = build/firmware.bin

# GCC toolchain software
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

build_release: $(CONFIG_NAME)
	mkdir -p build  \
		&& cd build \
		&& cmake .. -DBUILD_TYPE=Release \
		&& $(MAKE) -j$(shell nproc)

build_debug: $(CONFIG_NAME)
	mkdir -p build  \
		&& cd build \
		&& cmake .. -DBUILD_TYPE=Debug \
		&& $(MAKE) -j$(shell nproc)

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

.PHONY: build_release build_debug
