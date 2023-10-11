# Specify source files
file(GLOB SOURCES "src/*.c" "src/*.s")

# Build the library
add_library(stm32f103c6 STATIC ${SOURCES})
target_include_directories(stm32f103c6 PRIVATE "src")

# Set linker script
get_filename_component(LINKER_SCRIPT_PATH "./stm32f103c6.ld" REALPATH)
target_link_options(stm32f103c6 PUBLIC "SHELL:-Wl,-T,${LINKER_SCRIPT_PATH} -mcpu=cortex-m1 -ffreestanding -nostartfiles -Xlinker")
set_target_properties(stm32f103c6 PROPERTIES LINKER_LANGUAGE C)
