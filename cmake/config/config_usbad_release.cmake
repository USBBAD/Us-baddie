# Target platform
include(config_usbad_common)
set(US_TARGET_LINKER_OPTIONS "-Wl,-T,${CMAKE_CURRENT_SOURCE_DIR}/src/target/${US_TARGET_NAME}/${US_TARGET_NAME}.ld -mcpu=cortex-m3 -mlittle-endian -nostartfiles")
set(US_TARGET_COMPILE_OPTIONS "-mthumb -mcpu=cortex-m3")
