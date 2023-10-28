# Specify source files
file(GLOB SOURCES "src/*.c" "src/*.s")

# Build the library
add_library(stm32f103c6 STATIC ${SOURCES})
target_include_directories(stm32f103c6 PRIVATE "src")

# Set linker script
get_filename_component(LINKER_SCRIPT_PATH "./stm32f103c6.ld" REALPATH)
# Specify linker options
string(JOIN " " LINKER_OPTIONS
	"-Wl,-T,${LINKER_SCRIPT_PATH}"
	"-mcpu=cortex-m3"  # Target platform
	"-mlittle-endian"  # Use little-endian format
	"-ffreestanding"  # Imply there is no operating system, or standard library on the target platform
	"-nostartfiles"  # Do not run startup operations, such as setting global variables to their initial values. Do not use default startup files.
)
if (${BUILD_TYPE} STREQUAL "Debug")
	string(JOIN " " LINKER_OPTIONS
		${LINKER_OPTIONS}
		"-g3"
	)
endif()
message(STATUS "Using linker options: `${LINKER_OPTIONS}`")
target_link_options(stm32f103c6 PUBLIC "SHELL:${LINKER_OPTIONS}")
# Specify compiler options
string(JOIN " " COMPILE_OPTIONS
	"-mthumb"  # Use thumb code
	"-mcpu=cortex-m3"  # Specify CPU architecture
)
if (${BUILD_TYPE} STREQUAL "Debug")
	string(JOIN " " COMPILE_OPTIONS
		${COMPILE_OPTIONS}
		"-g3"
	)
endif()
target_link_libraries(stm32cubef1)  # To provide the opportunity to include headers
message(STATUS "Using compiler options: `${COMPILE_OPTIONS}`")
target_compile_options(stm32f103c6 PUBLIC "SHELL:${COMPILE_OPTIONS}")
set_target_properties(stm32f103c6 PROPERTIES LINKER_LANGUAGE C)
target_include_directories(stm32f103c6 PRIVATE ${COMMON_INCLUDE_DIRECTORIES})
