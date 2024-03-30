cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)
project(usbad)
enable_language(ASM)
list(APPEND CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/cmake
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/config)

# Apply configuration
include(config)

# Build libraries
add_subdirectory(lib)

# Recurse into building the taget-dependent code
add_subdirectory(src)
