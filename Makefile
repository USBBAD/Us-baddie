BUILD_ELF_PATH = build/usbad
OBJDUMP = arm-none-eabi-objdump

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
	$(OBJDUMP) -d $(BUILD_ELF_PATH)

disassemble_all:
	$(OBJDUMP) -D $(BUILD_ELF_PATH)

.PHONY: build
