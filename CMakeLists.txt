cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
project(usbad ASM C CXX)
enable_language(ASM)

# CMake included files
list(APPEND CMAKE_MODULE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/cmake
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/config)

# Apply configuration
#include(config)

# Build libraries
add_subdirectory(lib)

include(TargetUsbad)

# Build application
#add_subdirectory(src)
