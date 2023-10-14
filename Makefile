# Output ELF file path
BUILD_ELF_PATH = build/usbad

# Binary image (firmware) path
OUTPUT_BINARY_IMAGE_PATH = build/firmware.bin

# Project configuration file name
CONFIG_NAME = ProjectConfiguration.make

# GCC toolchain software
OBJDUMP = arm-none-eabi-objdump
OBJCOPY = arm-none-eabi-objcopy

# Debug or release
BUILD_TYPE ?= Release 

# Configure the project

ifneq ($(wildcard ProjectConfiguration.make),)
  include ProjectConfiguration.make
endif

build: $(CONFIG_NAME)
	mkdir -p build  \
		&& cd build \
		&& cmake .. -DBUILD_TYPE=$(BUILD_TYPE) \
		&& $(MAKE) -j$(shell nproc)


# [Re]configure the project
$(CONFIG_NAME):
	@echo --- Configuring project...
	@echo BUILD_TYPE = $(BUILD_TYPE) > ProjectConfiguration.make
	cat ProjectConfiguration.make
	@echo ---

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

.PHONY: build
