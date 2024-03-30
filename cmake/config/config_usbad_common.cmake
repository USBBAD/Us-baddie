# Target platform
include(config_common)
set(US_TARGET_NAME "usbad" CACHE STRING "Sources required by target, in glob format")
set(US_TARGET_SOURCES "target/usbad/src/*.c" "target/usbad/src/*.s" "target/arm/stm32f1/*.c" "application/default/*.c")
set(US_TARGET_INCLUDE_DIRECTORIES "target/usbad/src" "application/default" "target")
set(US_TARGET_LINKED_LIBRARIES "cmsis5" "stm32cubef1")
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "arm-none-eabi-as")
