# Target platform
include(config_common)
set(US_TARGET_NAME "usbad" CACHE STRING "Sources required by target, in glob format")
set(US_TARGET_SOURCES "target/usbad/src/*.c" "target/usbad/src/*.s" "target/arm/stm32f1/*.c" "application/default/*.c" "target/usbad/src/*.cpp")
set(US_TARGET_INCLUDE_DIRECTORIES "target/usbad/src" "application/default" "target")
set(US_TARGET_LINKED_LIBRARIES "cmsis5" "stm32cubef1")
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-as")

# fdata-sections ffunction-sections: Discard unused data/function sections, to save space, and prevent "symbol not found" errors
# nosys.specs: system calls are replaces w/ stubs returning error
set(US_TARGET_C_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3 -g3 --specs=nosys.specs -fdata-sections -ffunction-sections")
set(US_TARGET_CXX_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3 -g3 --specs=nosys.specs -fdata-sections -ffunction-sections")
set(US_TARGET_ASM_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3 -g3")

# gc-sections: remove unused sections
set(US_TARGET_LINKER_OPTIONS "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/src/target/${US_TARGET_NAME}/${US_TARGET_NAME}.ld,--gc-sections -mcpu=cortex-m3 -mlittle-endian -g3 --specs=nosys.specs -nostartfiles")

